#pragma once
#include "esphome.h"
#include "esphome/core/application.h"

extern "C" {
  #include "platform.h"
  #include "VL53L4CD_api.h"
}

namespace esphome {
namespace vl53l4cd {

static const char *const TAG = "vl53l4cd";

class VL53L4CDComponent : public PollingComponent, public i2c::I2CDevice, public sensor::Sensor {
 public:
  VL53L4CDComponent() : PollingComponent(250) {}

  void setup() override {
    ESP_LOGI(TAG, "Starting VL53L4CD setup...");
    Dev_t dev = this;
    uint8_t id = 0;

    App.feed_wdt();

    // Check Model ID register (0x010F)
    if (VL53L4CD_RdByte(dev, 0x010F, &id) != 0 || id != 0xEB) {
      ESP_LOGE(TAG, "Model ID check failed! Expected 0xEB, got 0x%02X", id);
      this->mark_failed();
      return;
    }

    ESP_LOGI(TAG, "Model ID verified (0xEB). Initializing sensor...");
    App.feed_wdt();

    if (VL53L4CD_SensorInit(dev) != 0) {
      ESP_LOGE(TAG, "SensorInit failed.");
      this->mark_failed();
      return;
    }

    ESP_LOGI(TAG, "SensorInit complete. Starting ranging...");
    App.feed_wdt();

    if (VL53L4CD_StartRanging(dev) != 0) {
      ESP_LOGE(TAG, "StartRanging failed.");
      this->mark_failed();
      return;
    }

    ESP_LOGI(TAG, "VL53L4CD initialized successfully!");
  }

  void update() override {
    
    Dev_t dev = this;
    uint8_t ready = 0;

    if (VL53L4CD_CheckForDataReady(dev, &ready) != 0) {
      return;
    }

    if (ready) {
      VL53L4CD_ResultsData_t results;
      VL53L4CD_GetResult(dev, &results);
      VL53L4CD_ClearInterrupt(dev);

      if (results.range_status == 0) {
        float distance_m = results.distance_mm / 1000.0f;
        ESP_LOGI(TAG, "Distance: %.3f m (%u mm)", distance_m, (unsigned int) results.distance_mm);
        this->publish_state(distance_m);
      }
    }
    
  }
};

}  // namespace vl53l4cd
}  // namespace esphome