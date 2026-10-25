#pragma once
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// Define Dev_t as a generic pointer to pass the ESPHome component reference
typedef void *Dev_t;

uint8_t VL53L4CD_WrByte(Dev_t dev, uint16_t index, uint8_t data);
uint8_t VL53L4CD_WrWord(Dev_t dev, uint16_t index, uint16_t data);
uint8_t VL53L4CD_WrDWord(Dev_t dev, uint16_t index, uint32_t data);
uint8_t VL53L4CD_RdByte(Dev_t dev, uint16_t index, uint8_t *pdata);
uint8_t VL53L4CD_RdWord(Dev_t dev, uint16_t index, uint16_t *pdata);
uint8_t VL53L4CD_RdDWord(Dev_t dev, uint16_t index, uint32_t *pdata);
uint8_t VL53L4CD_WaitMs(Dev_t dev, uint32_t TimeMs);

#ifdef __cplusplus
}
#endif
