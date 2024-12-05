#pragma once
#ifdef HAS_BLE_WRITER
#include "BLEDevice.h"

uint8_t gicToOEPLtype(uint8_t gicType);
bool BLE_filter_add_device(BLEAdvertisedDevice advertisedDevice);
bool BLE_is_image_pending(uint8_t address[8]);
uint32_t compress_image(uint8_t address[8], uint8_t* buffer, uint32_t max_len);
uint32_t get_ATC_BLE_OEPL_image(uint8_t address[8], uint8_t* buffer, uint32_t max_len, uint8_t* dataType, uint8_t* dataTypeArgument, uint16_t* nextCheckIn);

#endif
