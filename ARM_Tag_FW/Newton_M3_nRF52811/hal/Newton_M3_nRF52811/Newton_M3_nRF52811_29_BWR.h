#ifndef _BOARDHEADER_H_
#define _BOARDHEADER_H_

#include "../../../../tag_types.h"
#include "HAL_Newton_M3.h"

// eeprom map
#define EEPROM_SETTINGS_AREA_START (0x01000UL)
#define EEPROM_SETTINGS_AREA_LEN (0x03000UL)
#define EEPROM_UPDATE_AREA_START (0x04000UL)
#define EEPROM_UPDATE_AREA_LEN (0x10000UL)
#define EEPROM_IMG_START (0x14000UL)
#define EEPROM_IMG_EACH (0x04000UL) // 168*384 / 8 * 2 = 0x3F00

// Mac fixed part
// 7E22CC67B298 (B29)
#define MAC_ID_0 0xB2
#define MAC_ID_1 0x90

// AP mode definitions
#define HAS_EEPROM 1
#define HAS_SCREEN 1
#define AP_EMULATE_TAG 1

// hw types
#define HW_TYPE SOLUM_M3_BWR_29

#include "../include/ssd1619.h"

#define SCREEN_WIDTH 168
#define SCREEN_HEIGHT 384


#endif
