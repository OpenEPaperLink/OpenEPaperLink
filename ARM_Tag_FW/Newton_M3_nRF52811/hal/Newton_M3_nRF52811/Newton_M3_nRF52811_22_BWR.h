#ifndef _BOARDHEADER_H_
#define _BOARDHEADER_H_

#include "../../../../tag_types.h"
#include "HAL_Newton_M3.h"

// Mac fixed part
// 7E77B949B19A (B19)
#define MAC_ID_0 0xB1
#define MAC_ID_1 0x90

// hw types
#define HW_TYPE SOLUM_M3_BWR_22

#include "../include/ssd1619.h"

#define SCREEN_WIDTH 160
#define SCREEN_HEIGHT 296
#define SCREEN_XOFFSET 8
#define SCREEN_YOFFSET 0

#define EEPROM_IMG_EACH 0x3000UL //  ((SCREEN_WIDTH*SCREEN_HEIGHT/8*2)+sizeof(struct eepromImageHeader)) rounded up to the nearest sector size (4096)

#define EPD_MIRROR_H
#define EPD_DRAW_DIRECTION_RIGHT
//#define CUSTOM_LUT_SUPPORT
#endif
