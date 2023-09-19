#ifndef _BOARDHEADER_H_
#define _BOARDHEADER_H_

#include "../../../../tag_types.h"
#include "HAL_Newton_M3.h"


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
#define SCREEN_XOFFSET 8
#define SCREEN_YOFFSET 0

#define EEPROM_IMG_EACH 0x4000UL //  ((SCREEN_WIDTH*SCREEN_HEIGHT/8*2)+sizeof(struct eepromImageHeader)) rounded up to the nearest sector size (4096)

#define EPD_MIRROR_H
#define EPD_DRAW_DIRECTION_RIGHT
//#define CUSTOM_LUT_SUPPORT

#endif
