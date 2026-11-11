/*
 * Binary state definitions
 *
 */
#pragma once

#include <cstdint>

namespace esphome {
namespace cat_feeder_logic {

/// Enum for Up/Down states
enum class UpDownState : uint8_t {
  kUnknown = 0,
  kUp = 1,
  kDown = 2,
};

/// Enum for On/Off states
enum class OnOffState : uint8_t {
  kUnknown = 0,
  kOn = 1,
  kOff = 2,
};

/// Enum for Open/Closed states
enum class OpenClosedState : uint8_t {
  kUnknown = 0,
  kOpen = 1,
  kClosed = 2,
};

}  // namespace cat_feeder_logic
}  // namespace esphome
