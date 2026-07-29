#pragma once
#ifdef HAS_BLE_WRITER
#include "BLEDevice.h"

struct espAvailDataReq;

// Hands a parsed advertisement over to the BLE task for processing.
// MUST be used instead of calling processDataReq() directly from a BLE
// callback: the scan/advertisement callbacks run in the Bluedroid host task
// which only has a few KB of stack, and processDataReq() (tagDB, websocket
// JSON, UDP sync, filesystem) overflows that stack and reboots the ESP32.
void BLE_enqueue_data_req(struct espAvailDataReq* req);

uint8_t gicToOEPLtype(uint8_t gicType);
bool BLE_filter_add_device(BLEAdvertisedDevice advertisedDevice);
bool BLE_is_image_pending(uint8_t address[8]);
void BLE_clear_attempts(uint8_t address[8]);
uint32_t compress_image(uint8_t address[8], uint8_t* buffer, uint32_t max_len);
uint32_t get_ATC_BLE_OEPL_image(uint8_t address[8], uint8_t* buffer, uint32_t max_len, uint8_t* dataType, uint8_t* dataTypeArgument, uint16_t* nextCheckIn);

// If the next pending item for an ATC BLE OEPL tag is a command (DATATYPE_COMMAND_DATA)
// rather than an image, this builds the ready-to-write BLE control frame for it and
// returns true. 'frame' must be at least 2 + sizeof(struct AvailDataInfo) bytes.
// frameLen == 0 means the command has no BLE equivalent (e.g. a network scan) and the
// caller should just complete/clear it. isDeepsleep is set for a deep-sleep request so
// the caller can mark the tag as sleeping once it is acknowledged.
bool get_ATC_BLE_OEPL_command(uint8_t address[8], uint8_t* frame, uint8_t* frameLen, bool* isDeepsleep);

#endif
