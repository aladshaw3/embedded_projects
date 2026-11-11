/*
 * Motor State Tracker
 *
 */
#pragma once

#include "binary_state.h"
#include "circular_buffer.h"

namespace esphome {
namespace cat_feeder_logic {

/// Class to track the state of the feeder motor
/**
    We will have a voltage sense and current sense for the motor.
    Raw voltage and current (DC) will be fed into the algorithm and
    placed into a circular buffer. The circular buffer will be used
    to track signal histeresis. The slope of the signal will be used
    as an indicator of state changes, where the sign of the slope will
    be used to denote if the edge is rising or falling. That will in turn
    be used to determine if the motor is on or off.

    \todo Input sanitization - may want to track whether or not the readings are ok

    \todo Stats tracking - may want to aggregate average voltage/current readings in
    a more efficient way (single sample update stats, instead of buffer)

    \todo Need to do some experiments to quantify what the actual motor looks like.
    Much of this is just theory crafted. Should also keep track of lagging/leading
    voltage/current that could create state disagreement.
*/
class MotorStateTracker {
 public:
  constexpr static uint8_t kHistoryBufferSize = 2U;    ///< History buffer size
  constexpr static uint8_t kAggregateBufferSize = 5U;  ///< History buffer size

  /// Constructor
  MotorStateTracker();

  /// Initialization
  void SetThresholds(float volt_per_ms, float amps_per_ms, float delta_time);

  /// Run function
  /// \todo may be useful to add a mechanism to reset the circular buffer
  void IngestReadings(float voltage, float amperage);

  /// Return the motor state
  OnOffState GetMotorState() const;

  /// Return voltage state
  UpDownState GetVoltageState() const;

  /// Return current state
  UpDownState GetCurrentState() const;

  /// Calculate average on voltage
  float CalculateAvgVolts() const;

  /// Calculate average on current
  float CalculateAvgCurrent() const;

  /// Return if we are setup or not
  bool IsReady() const;

 private:
  // Tracking of past voltages and current are used to estimate the slopes with BDF2
  CircularBuffer<float, kHistoryBufferSize> past_voltage_;  ///< Prior voltage readings
  CircularBuffer<float, kHistoryBufferSize> past_current_;  ///< Prior current readings

  // Used to maintain a rolling average on voltages/currents while the motor is determined to be on
  CircularBuffer<float, kAggregateBufferSize> on_voltage_;  ///< Rolling buffer of voltages when motor is on
  CircularBuffer<float, kAggregateBufferSize> on_current_;  ///< Rolling buffer of currents when motor is on

  // State tracking for each signal - aggregated to agree on a motor state
  UpDownState volt_state_;  ///< Voltage state tracking
  UpDownState curr_state_;  ///< Current state tracking
  OnOffState motor_state_;  ///< Motor state

  // Thresholds for determination of slopes that dictate an edge
  /// \note If the waveform is symmetric in rise & fall, then we only need 1 threshold each
  float volt_per_ms_threshold_;  ///< Threshold above which we assert a state change
  float amps_per_ms_threshold_;  ///< Threshold above which we assert a state change
  float delta_time_;             ///< Run rate of the algorithm (assuming no jitter)
  bool is_setup_;                ///< Boolean state of the algorithm (set to true after SetThresholds() called)
};

}  // namespace cat_feeder_logic
}  // namespace esphome
