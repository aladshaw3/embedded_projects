/*
 * Feeder Scale
 *
 */
#pragma once

#include <cstdint>

#include "low_pass_filter.h"

namespace esphome {
namespace cat_feeder_logic {

/// Class for handling data from the NAU7802 load cell
/**
    This class object will take the raw ADC readings from
    the load cells and create a representation of the weight
    on the custom scale. Additional parameters are used to
    transform the generic weight into amounts of cat food.

    weight = gain * raw_adc_volts + offset
*/
class FeederScale {
 public:
  constexpr static int32_t kADCFloor = -10000;      ///< Floor of expected readings
  constexpr static int32_t kADCCeiling = 10000;     ///< Ceiling of expected readings
  constexpr static uint32_t kMinStartupCycles = 2;  ///< Min number of startup cycles

  /// Constructor
  FeederScale();

  /// Set startup cycles (optional)
  void SetStartupCycles(uint32_t startup_cycles);

  /// Set filter params
  void SetFilterParams(float delta_time, float cutoff_freq_hz);

  /// Set ADC calibrations
  void SetCalibrations(float gain, float offset);

  /// Set feeder weight params
  void SetFeederWeights(float empty_weight, float full_weight, float weight_per_cycle);

  /// Ingest the readings
  void IngestReadings(float raw_adc_voltage);

  /// Get weight on load cells
  float GetTotalWeight() const;

  /// Get weight of the food
  float GetFoodWeight() const;

  /// Get the percentage of food remaining
  float GetFoodPercentage() const;

  /// Get the number of cycles of food remainging
  uint32_t GetFoodCyclesRemaining() const;

  /// Returns true if calibration set and filter params set
  bool IsScaleReady() const;

  /// Returns true if all params are set
  bool IsFeederLogicReady() const;

 private:
  uint32_t discarded_samples_;  ///< number of consecutive discarded samples
  uint32_t startup_cycles_;     ///< number of cycles to ignore readings on startup
  uint32_t cycle_count_;        ///< counter for number of cycles (up to startup)

  float delta_time_;            ///< Sampling time in seconds
  float gain_;                  ///< calibration gain (grams/volt)
  float offset_;                ///< calibration offset (grams)
  float empty_weight_;          ///< empty weight of the feeder (grams)
  float full_weight_;           ///< weight of the feeder at max fill level (grams)
  float single_cycle_weight_;   ///< weight of about 1/8th cup of dry cat food (grams)
  LowPassFilter scale_weight_;  ///< low pass filter for the scale weight

  bool is_calibration_set_;    ///< bool to denote if calibration is set
  bool is_feeder_weight_set_;  ///< bool to denote if the feeder weight params are set
  bool is_filter_param_set_;   ///< bool to denote if the filter parameters are set
  bool is_filter_ready_;       ///< bool to denote if the filter is ready
};

}  // namespace cat_feeder_logic
}  // namespace esphome
