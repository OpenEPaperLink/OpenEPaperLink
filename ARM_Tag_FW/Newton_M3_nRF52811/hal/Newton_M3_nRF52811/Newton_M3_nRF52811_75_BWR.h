#ifndef _BOARDHEADER_H_
#define _BOARDHEADER_H_

#include "../../../../tag_types.h"
#include "hal_Newton_M3.h"

// eeprom map
#define EEPROM_SETTINGS_AREA_START (0x01000UL)
#define EEPROM_SETTINGS_AREA_LEN (0x03000UL)
#define EEPROM_UPDATE_AREA_START (0x04000UL)
#define EEPROM_UPDATE_AREA_LEN (0x10000UL)
#define EEPROM_IMG_START (0x14000UL) 
#define EEPROM_IMG_EACH (0x18000UL) // 800 * 480 * 2 / 8 = 0x17700

// Mac fixed part
// 7E22CC67B298 (B29)
#define MAC_ID_0 0xB2
#define MAC_ID_1 0x90  // FIX FOR THIS TYPE!!

// AP mode definitions
#define HAS_EEPROM 1
#define HAS_SCREEN 1
#define AP_EMULATE_TAG 1

// hw type
#define HW_TYPE SOLUM_M3_BWR_75

#include "../include/uc8179.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 480

#endif