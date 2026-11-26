/*
 * Battery Monitor
 *
 */

#include "battery_monitor.h"

#include <cmath>

namespace esphome {
namespace cat_feeder_logic {

BatteryMonitor::BatteryMonitor() {
  this->status_ = BatteryStatus::kUnknown;
  this->divider_ratio_ = 1.0F;
  this->nominal_threshold_ = 1.0F;
  this->warning_threshold_ = 0.0F;
  this->dead_threshold_ = 0.0F;
  this->delta_time_ = 1.0F;
  this->is_setup_ = false;
  this->filter_ready_ = false;
  this->is_new_alarm_ = false;
}

void BatteryMonitor::Initialize(float div_ratio, float nominal_threshold, float warning_threshold, float dead_threshold,
                                float delta_time, float cutoff_freq_hz) {
  // Check for junk
  if (std::isnan(div_ratio) || std::isinf(div_ratio) || std::isnan(nominal_threshold) ||
      std::isinf(nominal_threshold) || std::isnan(warning_threshold) || std::isinf(warning_threshold) ||
      std::isnan(dead_threshold) || std::isinf(dead_threshold) || std::isnan(delta_time) || std::isinf(delta_time)) {
    return;
  }
  // Upper bound on nominal voltage (should be no more than 12 V and no less than 1 V)
  if (nominal_threshold > 12.0F) {  // NOLINT
    nominal_threshold = 12.0F;      // NOLINT
  }
  if (nominal_threshold < 1.0F) {
    nominal_threshold = 1.0F;
  }
  // Dead threshold hold MUST be smaller than nominal and larger than 0 V
  if (dead_threshold >= nominal_threshold) {
    dead_threshold = 0.5F * nominal_threshold;  // NOLINT
  }
  if (dead_threshold < 0.0F) {
    dead_threshold = 0.0F;
  }
  // Warning threshold must be between nominal and dead
  if (warning_threshold > nominal_threshold || warning_threshold < dead_threshold) {
    warning_threshold = 0.5F * (nominal_threshold - dead_threshold) + dead_threshold;  // NOLINT
  }
  // Less than 0.001 s will be lower bound (arbitrary)
  if (delta_time <= 0.001F) {  // NOLINT
    delta_time = 0.001F;       // NOLINT
  }
  // Less than 0.1 V will be lower bound (arbitrary)
  if (div_ratio <= 0.1F) {  // NOLINT
    div_ratio = 0.1F;       // NOLINT
  }

  // Now, set the initial values
  this->divider_ratio_ = div_ratio;
  this->nominal_threshold_ = nominal_threshold;
  this->warning_threshold_ = warning_threshold;
  this->dead_threshold_ = dead_threshold;
  this->delta_time_ = delta_time;
  this->battery_voltage_.SetInitialValues(0.0F, cutoff_freq_hz);
  this->is_setup_ = true;
}

void BatteryMonitor::IngestReadings(float raw_adc_voltage) {
  // Don't run calculations if we are no setup
  if (!is_setup_) {
    return;
  }

  // If the filter is not fully initialized, then we set its first initial value
  if (!this->filter_ready_) {
    this->battery_voltage_.SetInitialValues((raw_adc_voltage / this->divider_ratio_),
                                            this->battery_voltage_.ReadCutoffFreqHz());
    this->filter_ready_ = true;
  }
  this->battery_voltage_.Filter((raw_adc_voltage / this->divider_ratio_), this->delta_time_);

  // Check filtered voltage for status
  if (this->GetBatteryVoltage() > this->warning_threshold_) {
    this->is_new_alarm_ = false;
    this->status_ = BatteryStatus::kOk;
  } else {
    this->is_new_alarm_ = (this->status_ != BatteryStatus::kAlarmed);
    this->status_ = BatteryStatus::kAlarmed;
  }
}

BatteryStatus BatteryMonitor::GetBatteryStatus() const { return this->status_; }

float BatteryMonitor::GetBatteryVoltage() const { return this->battery_voltage_.ReadPriorFilter(); }

float BatteryMonitor::GetBatteryPercentage() const {
  if (this->GetBatteryVoltage() > this->nominal_threshold_) {
    return 100.0F;
  }
  if (this->GetBatteryVoltage() < this->dead_threshold_) {
    return 0.0F;
  }
  return 100.0F * (this->GetBatteryVoltage() - this->dead_threshold_) /
         (this->nominal_threshold_ - this->dead_threshold_);
}

bool BatteryMonitor::IsSetup() const { return this->is_setup_; }

bool BatteryMonitor::IsNewlyAlarmed() const { return this->is_new_alarm_; }

}  // namespace cat_feeder_logic
}  // namespace esphome
