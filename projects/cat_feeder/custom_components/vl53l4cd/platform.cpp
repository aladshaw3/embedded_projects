#include "platform.h"
#include "vl53l4cd.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// Cast generic dev handle to ESPHome I2CDevice pointer
#define COMPONENT ((esphome::vl53l4cd::VL53L4CDComponent*) dev)

extern "C" {

uint8_t VL53L4CD_WrByte(Dev_t dev, uint16_t index, uint8_t data) {
  if (dev == nullptr) return 1;
  return COMPONENT->write_register16(index, &data, 1) == esphome::i2c::ERROR_OK ? 0 : 1;
}

uint8_t VL53L4CD_WrWord(Dev_t dev, uint16_t index, uint16_t data) {
  if (dev == nullptr) return 1;
  uint8_t buf[2] = {static_cast<uint8_t>(data >> 8), static_cast<uint8_t>(data & 0xFF)};
  return COMPONENT->write_register16(index, buf, 2) == esphome::i2c::ERROR_OK ? 0 : 1;
}

uint8_t VL53L4CD_WrDWord(Dev_t dev, uint16_t index, uint32_t data) {
  if (dev == nullptr) return 1;
  uint8_t buf[4] = {
      static_cast<uint8_t>(data >> 24),
      static_cast<uint8_t>((data >> 16) & 0xFF),
      static_cast<uint8_t>((data >> 8) & 0xFF),
      static_cast<uint8_t>(data & 0xFF)
  };
  return COMPONENT->write_register16(index, buf, 4) == esphome::i2c::ERROR_OK ? 0 : 1;
}

uint8_t VL53L4CD_RdByte(Dev_t dev, uint16_t index, uint8_t *pdata) {
  if (dev == nullptr || pdata == nullptr) return 1;
  return COMPONENT->read_register16(index, pdata, 1) == esphome::i2c::ERROR_OK ? 0 : 1;
}

uint8_t VL53L4CD_RdWord(Dev_t dev, uint16_t index, uint16_t *pdata) {
  if (dev == nullptr || pdata == nullptr) return 1;
  uint8_t buf[2] = {0, 0};
  if (COMPONENT->read_register16(index, buf, 2) != esphome::i2c::ERROR_OK) {
    return 1;
  }
  *pdata = (static_cast<uint16_t>(buf[0]) << 8) | buf[1];
  return 0;
}

uint8_t VL53L4CD_RdDWord(Dev_t dev, uint16_t index, uint32_t *pdata) {
  if (dev == nullptr || pdata == nullptr) return 1;
  uint8_t buf[4] = {0, 0, 0, 0};
  if (COMPONENT->read_register16(index, buf, 4) != esphome::i2c::ERROR_OK) {
    return 1;
  }
  *pdata = (static_cast<uint32_t>(buf[0]) << 24) |
           (static_cast<uint32_t>(buf[1]) << 16) |
           (static_cast<uint32_t>(buf[2]) << 8) |
           buf[3];
  return 0;
}

uint8_t VL53L4CD_WaitMs(Dev_t dev, uint32_t TimeMs) {
  // Pass 0 ticks if TimeMs is 0 to prevent unnecessary context switching
  if (TimeMs > 0) {
    vTaskDelay(pdMS_TO_TICKS(TimeMs));
  }
  return 0;
}

} // extern "C"