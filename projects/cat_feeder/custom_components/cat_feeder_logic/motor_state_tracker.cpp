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
  this->volt_per_ms_threshold_ = 0.0F;
  this->amps_per_ms_threshold_ = 0.0F;
  this->delta_time_ = 1.0F;
  this->is_setup_ = false;
}

void MotorStateTracker::SetThresholds(float volt_per_ms, float amps_per_ms, float delta_time) {
  // Check for junk
  if (std::isnan(volt_per_ms) || std::isinf(volt_per_ms) || std::isnan(amps_per_ms) || std::isinf(amps_per_ms) ||
      std::isnan(delta_time) || std::isinf(delta_time)) {
    return;
  }
  // Check for lower bounds (could provide constants for these)
  if (volt_per_ms <= 2.0F * std::numeric_limits<float>::epsilon()) {  // NOLINT
    volt_per_ms = 2.0F * std::numeric_limits<float>::epsilon();       // NOLINT
  }
  if (amps_per_ms <= 2.0F * std::numeric_limits<float>::epsilon()) {  // NOLINT
    amps_per_ms = 2.0F * std::numeric_limits<float>::epsilon();       // NOLINT
  }
  // Less than 0.1 ms will be lower bound (arbitrary)
  if (delta_time <= 0.1F) {  // NOLINT
    delta_time = 0.1F;       // NOLINT
  }
  this->volt_per_ms_threshold_ = volt_per_ms;
  this->amps_per_ms_threshold_ = amps_per_ms;
  this->delta_time_ = delta_time;
  this->is_setup_ = true;
}

void MotorStateTracker::IngestReadings(float voltage, float amperage) {
  // If buffers are not ready, don't start doing the slope calculations yet
  if (this->past_voltage_.Size() < kHistoryBufferSize && this->past_current_.Size() < kHistoryBufferSize) {
    this->past_voltage_.Push(voltage);
    this->past_current_.Push(amperage);
    return;
  }

  // If we are here, that means the buffers are full and we can calculate slopes with the new info
  /// \todo create a private helper function to reduce copy-paste here since we do the same op for volt & current
  float volt_der = BDF2Method(voltage, this->past_voltage_[0], this->past_voltage_[1], this->delta_time_);
  float amps_der = BDF2Method(amperage, this->past_current_[0], this->past_current_[1], this->delta_time_);
  if (volt_der >= this->volt_per_ms_threshold_) {
    this->volt_state_ = UpDownState::kUp;
  }
  if (volt_der <= -this->volt_per_ms_threshold_) {
    this->volt_state_ = UpDownState::kDown;
  }
  if (amps_der >= this->amps_per_ms_threshold_) {
    this->curr_state_ = UpDownState::kUp;
  }
  if (amps_der <= -this->amps_per_ms_threshold_) {
    this->curr_state_ = UpDownState::kDown;
  }

  // Use the states of voltage and current to dictate the motor state
  if (this->volt_state_ == UpDownState::kUp && this->curr_state_ == UpDownState::kUp) {
    this->motor_state_ = OnOffState::kOn;
  }
  if (this->volt_state_ == UpDownState::kDown && this->curr_state_ == UpDownState::kDown) {
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

OnOffState MotorStateTracker::GetMotorState() const { return this->motor_state_; }

UpDownState MotorStateTracker::GetVoltageState() const { return this->volt_state_; }

UpDownState MotorStateTracker::GetCurrentState() const { return this->curr_state_; }

float MotorStateTracker::CalculateAvgVolts() const { return this->on_voltage_.Average(); }

float MotorStateTracker::CalculateAvgCurrent() const { return this->on_current_.Average(); }

bool MotorStateTracker::IsReady() const { return this->is_setup_; }

}  // namespace cat_feeder_logic
}  // namespace esphome
