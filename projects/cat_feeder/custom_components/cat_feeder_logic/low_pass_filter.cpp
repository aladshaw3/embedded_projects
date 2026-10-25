/*
 * Simple low pass filter implementation
 *
 */

#include "low_pass_filter.h"

#include <cmath>
#include <numbers>

namespace esphome {
namespace cat_feeder_logic {

LowPassFilter::LowPassFilter() {
  this->cutoff_freq_hz_ = 1.0F;
  this->y_old_ = 0.0F;
  this->alpha_ = 0.0F;
}

void LowPassFilter::SetInitialValues(float init_y, float cutoff_freq_hz) {
  if (std::isnan(init_y) || std::isinf(init_y) || std::isnan(cutoff_freq_hz) || std::isinf(cutoff_freq_hz)) {
    return;
  }
  this->y_old_ = init_y;
  if (cutoff_freq_hz < LowPassFilter::kMinCutoffFreq) {
    this->cutoff_freq_hz_ = LowPassFilter::kMinCutoffFreq;
  }
  this->cutoff_freq_hz_ = cutoff_freq_hz;
  this->alpha_ = 2.0F * std::numbers::pi_v<float> * this->cutoff_freq_hz_;  // NOLINT
}

float LowPassFilter::Filter(float input_value, float delta_time_seconds) {
  if (std::isnan(input_value) || std::isinf(input_value) || std::isnan(delta_time_seconds) ||
      std::isinf(delta_time_seconds)) {
    return this->y_old_;
  }
  if (delta_time_seconds < LowPassFilter::kMinCutoffFreq) {
    this->y_old_ = input_value;
    return input_value;
  }
  float beta = this->alpha_ * delta_time_seconds;
  this->y_old_ = (this->y_old_ + beta * input_value) / (1.0F + beta);
  return this->y_old_;
}

void LowPassFilter::Reset() { this->y_old_ = 0.0F; }

}  // namespace cat_feeder_logic
}  // namespace esphome
