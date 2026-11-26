/*
 * Feeder scale
 *
 */

#include "feeder_scale.h"

#include <cmath>
#include <limits>

namespace esphome {
namespace cat_feeder_logic {

FeederScale::FeederScale() {
  this->discarded_samples_ = 0U;
  this->startup_cycles_ = kMinStartupCycles;
  this->cycle_count_ = 0U;
  this->delta_time_ = 1.0F;
  this->gain_ = 1.0F;
  this->offset_ = 0.0F;
  this->empty_weight_ = 0.0F;
  this->full_weight_ = 1.0F;
  this->single_cycle_weight_ = 1.0F;
  this->is_calibration_set_ = false;
  this->is_feeder_weight_set_ = false;
  this->is_filter_param_set_ = false;
  this->is_filter_ready_ = false;
}

void FeederScale::SetStartupCycles(uint32_t startup_cycles) {
  if (startup_cycles < kMinStartupCycles) {
    this->startup_cycles_ = kMinStartupCycles;
    return;
  }
  this->startup_cycles_ = startup_cycles;
}

void FeederScale::SetFilterParams(float delta_time, float cutoff_freq_hz) {
  if (std::isnan(delta_time) || std::isinf(delta_time)) {
    return;
  }
  // Less than 0.001 s will be lower bound (arbitrary)
  if (delta_time <= 0.001F) {  // NOLINT
    delta_time = 0.001F;       // NOLINT
  }
  this->delta_time_ = delta_time;
  this->scale_weight_.SetInitialValues(0.0F, cutoff_freq_hz);
  this->is_filter_param_set_ = true;
}

void FeederScale::SetCalibrations(float gain, float offset) {
  if (std::isnan(gain) || std::isinf(gain) || std::isnan(offset) || std::isinf(offset)) {
    return;
  }
  this->gain_ = gain;
  this->offset_ = offset;
  this->is_calibration_set_ = true;
}

void FeederScale::SetFeederWeights(float empty_weight, float full_weight, float weight_per_cycle) {
  if (std::isnan(empty_weight) || std::isinf(empty_weight) || std::isnan(full_weight) || std::isinf(full_weight) ||
      std::isnan(weight_per_cycle) || std::isinf(weight_per_cycle)) {
    return;
  }
  // Check for illogical inputs and make back-of-the-hand corrections
  if (empty_weight < 0.0F) {
    empty_weight = 0.0F;
  }
  // max weight possible is 20 kg
  if (full_weight > 20000.0F) {  // NOLINT
    full_weight = 20000.0F;      // NOLINT
  }
  if (full_weight < 1.0F) {
    full_weight = 1.0F;
  }
  if (empty_weight > full_weight) {
    empty_weight = 0.5F * full_weight;  // NOLINT
  }
  if (weight_per_cycle > (full_weight - empty_weight)) {
    weight_per_cycle = (full_weight - empty_weight);
  }
  if (weight_per_cycle < 0.0F) {
    weight_per_cycle = 0.5F * (full_weight - empty_weight);  // NOLINT
  }

  // Set the values
  this->empty_weight_ = empty_weight;
  this->full_weight_ = full_weight;
  this->single_cycle_weight_ = weight_per_cycle;
  this->is_feeder_weight_set_ = true;
}

void FeederScale::IngestReadings(float raw_adc_voltage) {
  // Check the startup
  if (this->cycle_count_ < this->startup_cycles_) {
    this->cycle_count_++;
    return;
  }

  // Check the calibration status (if not set, do nothing)
  if (!this->is_calibration_set_ || !this->is_filter_param_set_) {
    return;
  }

  // Check for valid readings
  if (raw_adc_voltage > static_cast<float>(kADCCeiling) || raw_adc_voltage < static_cast<float>(kADCFloor)) {
    this->discarded_samples_ = (this->discarded_samples_ == std::numeric_limits<uint32_t>::max())
                                   ? this->discarded_samples_
                                   : this->discarded_samples_ + 1;
    return;
  }

  // Check if filter is ready, then do filter calculations
  if (!this->is_filter_ready_) {
    this->scale_weight_.SetInitialValues((raw_adc_voltage * this->gain_ + this->offset_),
                                         this->scale_weight_.ReadCutoffFreqHz());
    this->is_filter_ready_ = true;
  }
  // Calculation accounts for jitter in discarded samples
  this->scale_weight_.Filter((raw_adc_voltage * this->gain_ + this->offset_),
                             this->delta_time_ * (1.0F + static_cast<float>(this->discarded_samples_)));

  // Reset number of discarded samples
  this->discarded_samples_ = 0U;
}

float FeederScale::GetTotalWeight() const { return this->scale_weight_.ReadPriorFilter(); }

float FeederScale::GetFoodWeight() const {
  if (this->is_feeder_weight_set_ && this->is_filter_ready_) {
    float total = this->GetTotalWeight();
    return (total > this->empty_weight_) ? (total - this->empty_weight_) : 0.0F;
  }
  return 0.0F;
}

float FeederScale::GetFoodPercentage() const {
  if (this->is_feeder_weight_set_) {
    float food = this->GetFoodWeight();
    return (food > this->full_weight_) ? 100.0F : (food / (this->full_weight_ - this->empty_weight_)) * 100.0F;
  }
  return 0.0F;
}

uint32_t FeederScale::GetFoodCyclesRemaining() const {
  if (this->is_feeder_weight_set_) {
    return static_cast<uint32_t>(this->GetFoodWeight() / this->single_cycle_weight_);
  }
  return 0U;
}

bool FeederScale::IsScaleReady() const { return (this->is_calibration_set_ && this->is_filter_param_set_); }

bool FeederScale::IsFeederLogicReady() const { return (this->IsScaleReady() && this->is_feeder_weight_set_); }

}  // namespace cat_feeder_logic
}  // namespace esphome
