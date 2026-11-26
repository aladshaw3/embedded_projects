/*
 * Battery Monitor
 *
 */
#pragma once

#include <cstdint>

#include "low_pass_filter.h"

namespace esphome {
namespace cat_feeder_logic {

/// Enum for battery alarm states
enum class BatteryStatus : uint8_t {
  kUnknown = 0,
  kOk = 1,
  kAlarmed = 2,
};

/// Class for monitoring battery backup for the feeder
/**
    Inputs: raw_adc_voltage
    Outputs: battery_voltage, battery_percentage
    Params: divider_ratio, nominal_threshold, warning_threshold, dead_threshold

    battery_voltage = raw_adc_voltage / divider_ratio

    \note Calibration of parameters required with real data collected
*/
class BatteryMonitor {
 public:
  /// Constructor
  BatteryMonitor();

  /// Initialization
  void Initialize(float div_ratio, float nominal_threshold, float warning_threshold, float dead_threshold,
                  float delta_time, float cutoff_freq_hz);

  /// Ingestion
  void IngestReadings(float raw_adc_voltage);

  /// Read battery status
  BatteryStatus GetBatteryStatus() const;

  /// Read battery voltage
  float GetBatteryVoltage() const;

  /// Read battery percentage
  float GetBatteryPercentage() const;

  /// Read back setup status
  bool IsSetup() const;

  /// Read back the new alarm status
  bool IsNewlyAlarmed() const;

 private:
  BatteryStatus status_;           ///< status of the battery
  LowPassFilter battery_voltage_;  ///< filtered battery voltage result
  float divider_ratio_;            ///< ratio to convert raw ADC voltage into battery voltage
  float nominal_threshold_;        ///< threshold at which battery is considered fully healthy (in volts)
  float dead_threshold_;           ///< threshold at which battery is considered fully dead (in volts)
  float warning_threshold_;        ///< threshold below which a warning should be created (in volts)
  float delta_time_;               ///< sample time of the algorithm (in seconds)
  bool is_setup_;                  ///< set to true after the algo is setup
  bool filter_ready_;              ///< boolean to mark if filter is ready
  bool is_new_alarm_;              ///< set to true if the status is alarmed and the prior status was not
};

}  // namespace cat_feeder_logic
}  // namespace esphome
