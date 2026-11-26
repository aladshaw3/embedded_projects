/*
 * Motor State Tracker
 *
 */

#include "motor_state_tracker.h"

#include <cmath>
#include <limits>

#include "derivatives.h"

namespace esphome {
namespace cat_feeder_logic {

MotorStateTracker::MotorStateTracker() {
  this->volt_state_ = UpDownState::kUnknown;
  this->curr_state_ = UpDownState::kUnknown;
  this->motor_state_ = OnOffState::kUnknown;
  this->volt_per_s_threshold_ = 0.0F;
  this->amps_per_s_threshold_ = 0.0F;
  this->delta_time_ = 1.0F;
  this->is_setup_ = false;
  this->is_ready_ = false;
}

void MotorStateTracker::Initialize(float volt_per_s, float amps_per_s, float delta_time, float cutoff_freq_hz) {
  // Check for junk
  if (std::isnan(volt_per_s) || std::isinf(volt_per_s) || std::isnan(amps_per_s) || std::isinf(amps_per_s) ||
      std::isnan(delta_time) || std::isinf(delta_time)) {
    return;
  }
  // Check for lower bounds (could provide constants for these)
  if (volt_per_s <= 2.0F * std::numeric_limits<float>::epsilon()) {  // NOLINT
    volt_per_s = 2.0F * std::numeric_limits<float>::epsilon();       // NOLINT
  }
  if (amps_per_s <= 2.0F * std::numeric_limits<float>::epsilon()) {  // NOLINT
    amps_per_s = 2.0F * std::numeric_limits<float>::epsilon();       // NOLINT
  }
  // Less than 0.001 s will be lower bound (arbitrary)
  if (delta_time <= 0.001F) {  // NOLINT
    delta_time = 0.001F;       // NOLINT
  }
  this->voltage_filter_.SetInitialValues(0.0F, cutoff_freq_hz);
  this->amperage_filter_.SetInitialValues(0.0F, cutoff_freq_hz);
  this->volt_per_s_threshold_ = volt_per_s;
  this->amps_per_s_threshold_ = amps_per_s;
  this->delta_time_ = delta_time;
  this->is_setup_ = true;
}

void MotorStateTracker::IngestReadings(float voltage, float amperage) {
  // If we are not setup yet, don't ingest info
  if (!this->is_setup_) {
    return;
  }
  // Start by filtering the raw inputs (if ready, otherwise initialize filter to first observations)
  if (!this->is_ready_) {
    this->voltage_filter_.SetInitialValues(voltage, this->voltage_filter_.ReadCutoffFreqHz());
    this->amperage_filter_.SetInitialValues(amperage, this->amperage_filter_.ReadCutoffFreqHz());
  } else {
    voltage = this->voltage_filter_.Filter(voltage, this->delta_time_);
    amperage = this->amperage_filter_.Filter(amperage, this->delta_time_);
  }

  // If buffers are not ready, don't start doing the slope calculations yet
  if (this->past_voltage_.Size() < kHistoryBufferSize && this->past_current_.Size() < kHistoryBufferSize) {
    this->past_voltage_.Push(voltage);
    this->past_current_.Push(amperage);
    return;
  }
  this->is_ready_ = true;

  // If we are here, that means the buffers are full and we can calculate slopes with the new info
  this->volt_state_ = DetermineMeasurementState(voltage, this->past_voltage_[0], this->past_voltage_[1],
                                                this->volt_per_s_threshold_, this->volt_state_);
  this->curr_state_ = DetermineMeasurementState(amperage, this->past_current_[0], this->past_current_[1],
                                                this->amps_per_s_threshold_, this->curr_state_);

  // Use the states of voltage and current to dictate the motor state
  // During use, voltage is expected to be down and current is expected to be up
  if (this->volt_state_ == UpDownState::kDown && this->curr_state_ == UpDownState::kUp) {
    this->motor_state_ = OnOffState::kOn;
  }
  if (this->volt_state_ == UpDownState::kUp && this->curr_state_ == UpDownState::kDown) {
    this->motor_state_ = OnOffState::kOff;
  }

  // If motor is determined to be on, push the voltage and amperage into the On buffers for aggregation
  if (this->motor_state_ == OnOffState::kOn) {
    this->on_voltage_.Push(voltage);
    this->on_current_.Push(amperage);
  }

  // Update buffers as last step
  this->past_voltage_.Push(voltage);
  this->past_current_.Push(amperage);
}

UpDownState MotorStateTracker::DetermineMeasurementState(float now, float old, float older, float threshold,
                                                         UpDownState last_state) const {
  float derivative = BDF2Method(now, old, older, this->delta_time_);
  if (derivative >= threshold) {
    return UpDownState::kUp;
  }
  if (derivative <= -threshold) {
    return UpDownState::kDown;
  }
  return last_state;
}

OnOffState MotorStateTracker::GetMotorState() const { return this->motor_state_; }

UpDownState MotorStateTracker::GetVoltageState() const { return this->volt_state_; }

UpDownState MotorStateTracker::GetCurrentState() const { return this->curr_state_; }

float MotorStateTracker::CalculateAvgVolts() const { return this->on_voltage_.Average(); }

float MotorStateTracker::CalculateAvgCurrent() const { return this->on_current_.Average(); }

bool MotorStateTracker::IsSetup() const { return this->is_setup_; }

bool MotorStateTracker::IsReady() const { return this->is_ready_; }

}  // namespace cat_feeder_logic
}  // namespace esphome
