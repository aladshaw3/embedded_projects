#include "cat_feeder_logic.h"

#include <algorithm>

#include "esphome/core/log.h"

namespace esphome {
namespace cat_feeder_logic {

static const char *const TAG = "cat_feeder_logic";

void CatFeederLogic::loop() {
  uint32_t now = millis();
  if (now - this->last_update_ >= 250) {
    this->last_update_ = now;
    this->update_logic_();
  }
}

void CatFeederLogic::update_logic_() {
  float voltage =
      (this->bus_voltage_sensor_ && this->bus_voltage_sensor_->has_state()) ? this->bus_voltage_sensor_->state : 0.0f;
  float current = (this->current_sensor_ && this->current_sensor_->has_state()) ? this->current_sensor_->state : 0.0f;
  float power = (this->power_sensor_ && this->power_sensor_->has_state()) ? this->power_sensor_->state : 0.0f;
  bool d14_val = (this->digital_sensor_ && this->digital_sensor_->has_state()) ? this->digital_sensor_->state : false;
  float a0_val = (this->analog_sensor_ && this->analog_sensor_->has_state()) ? this->analog_sensor_->state : 0.0f;
  float weight = (this->weight_sensor_ && this->weight_sensor_->has_state()) ? this->weight_sensor_->state : 0.0f;

  // --- NeoPixel Output Logic ---
  if (this->status_light_) {
    float green_brightness = std::min(1.0f, std::max(0.0f, a0_val / 3.3f));
    float red = d14_val ? 1.0f : 0.0f;
    float green = d14_val ? 1.0f : green_brightness;
    float blue = 0.0f;

    auto call = this->status_light_->turn_on();
    call.set_rgb(red, green, blue);
    call.set_brightness(0.3f);
    call.set_transition_length(0);
    call.perform();
  }

  // --- Logging and Display Formatting ---
  if (this->distance_sensor_ && this->distance_sensor_->has_state()) {
    float dist_m = this->distance_sensor_->state;
    int dist_mm = static_cast<int>(dist_m * 1000.0f);

    ESP_LOGI(TAG, "Dist: %.3fm (%dmm) | Weight: %.1fg | V: %.3fV | I: %.3fA | P: %.3fW | A0: %.3fV | D14: %s", dist_m,
             dist_mm, weight, voltage, current, power, a0_val, d14_val ? "HIGH" : "LOW");
  } else {
    ESP_LOGI(TAG, "Dist: [Wait] | Weight: %.1fg | V: %.3fV | I: %.3fA | P: %.3fW | A0: %.3fV | D14: %s", weight,
             voltage, current, power, a0_val, d14_val ? "HIGH" : "LOW");
  }
}

}  // namespace cat_feeder_logic
}  // namespace esphome
