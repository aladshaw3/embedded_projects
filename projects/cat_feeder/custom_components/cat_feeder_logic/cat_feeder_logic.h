#pragma once

#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/binary_sensor/binary_sensor.h"
#include "esphome/components/light/light_state.h"
#include "../ht16k33_alpha/ht16k33_display.h"

namespace esphome {
namespace cat_feeder_logic {

class CatFeederLogic : public Component {
 public:
  void set_distance_sensor(sensor::Sensor *s) { this->distance_sensor_ = s; }
  void set_bus_voltage_sensor(sensor::Sensor *s) { this->bus_voltage_sensor_ = s; }
  void set_current_sensor(sensor::Sensor *s) { this->current_sensor_ = s; }
  void set_power_sensor(sensor::Sensor *s) { this->power_sensor_ = s; }
  void set_analog_sensor(sensor::Sensor *s) { this->analog_sensor_ = s; }
  void set_digital_sensor(binary_sensor::BinarySensor *s) { this->digital_sensor_ = s; }
  void set_display(ht16k33_alpha::HT16K33AlphaDisplay *d) { this->display_ = d; }
  void set_status_light(light::LightState *l) { this->status_light_ = l; }

  void loop() override;
  float get_setup_priority() const override { return setup_priority::LATE; }

 protected:
  sensor::Sensor *distance_sensor_{nullptr};
  sensor::Sensor *bus_voltage_sensor_{nullptr};
  sensor::Sensor *current_sensor_{nullptr};
  sensor::Sensor *power_sensor_{nullptr};
  sensor::Sensor *analog_sensor_{nullptr};
  binary_sensor::BinarySensor *digital_sensor_{nullptr};
  ht16k33_alpha::HT16K33AlphaDisplay *display_{nullptr};
  light::LightState *status_light_{nullptr};

  uint32_t last_update_{0};
  void update_logic_();
};

}  // namespace cat_feeder_logic
}  // namespace esphome