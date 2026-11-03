/*
 * Helper functions to calculate derivatives of a signal
 *
 * WARNING: Methods here are not safe from zero division
 * or overflow. User should sanitize inputs.
 *
 */

#include "derivatives.h"

namespace esphome {
namespace cat_feeder_logic {

float EulerMethod(float y_now, float y_old, float delta_time) { return (y_now - y_old) / delta_time; }

float BDF2Method(float y_now, float y_old, float y_older, float delta_time) {
  return (1.5F * y_now - 2.0F * y_old + 0.5F * y_older) / delta_time;  // NOLINT
}

float FivePointStencilMethod(float y_0, float y_1, float y_3, float y_4, float delta_time) {
  return (8.0F * y_1 - 8.0F * y_3 - y_0 + y_4) / (12.0F * delta_time);  // NOLINT
}

}  // namespace cat_feeder_logic
}  // namespace esphome
