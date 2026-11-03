/*
 * Helper functions to calculate derivatives of a signal
 *
 * WARNING: Methods here are not safe from zero division
 * or overflow. User should sanitize inputs.
 *
 */
#pragma once

namespace esphome {
namespace cat_feeder_logic {

/// First order accurate derivative
/**
    Centered between current point and last point
*/
float EulerMethod(float y_now, float y_old, float delta_time);

/// Second order accurate derivative
/**
    Centered on the prior point
*/
float BDF2Method(float y_now, float y_old, float y_older, float delta_time);

/// Forth order accurate derivative
/**
    Centered on the sample from 2 cycles ago (y_2, not used in calculation)
*/
float FivePointStencilMethod(float y_0, float y_1, float y_3, float y_4, float delta_time);

}  // namespace cat_feeder_logic
}  // namespace esphome
