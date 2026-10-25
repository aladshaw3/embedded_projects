/*
 * Simple low pass filter implementation
 *
 */
#pragma once

#include <limits>

namespace esphome {
namespace cat_feeder_logic {

/// Low Pass Filter Class
class LowPassFilter {
 public:
  /// Minimum allowable cutoff frequency
  constexpr static float kMinCutoffFreq = std::numeric_limits<float>::epsilon();

  /// Constructor
  LowPassFilter();

  /// Initializer
  void SetInitialValues(float init_y, float cutoff_freq_hz);

  /// Run function
  float Filter(float input_value, float delta_time_seconds);

  /// Reset Function
  void Reset();

  /// Read back the cutoff frequency
  float ReadCutoffFreqHz() const { return this->cutoff_freq_hz_; }

  /// Read back the prior filtered result
  float ReadPriorFilter() const { return this->y_old_; }

  /// Read back the alpha parameter
  float ReadAlpha() const { return this->alpha_; }

 private:
  float cutoff_freq_hz_;  ///< Cut off frequency in Hz
  float y_old_;           ///< Prior filtered value
  float alpha_;           ///< Computed parameter from cutoff frequency (Hz)
};

}  // namespace cat_feeder_logic
}  // namespace esphome
