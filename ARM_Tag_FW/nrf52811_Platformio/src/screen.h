#ifndef _SCREEN_H_
#define _SCREEN_H_

#include <stdbool.h>
#include <stdint.h>
#include "ssd1619.h"

#define SCREEN_WIDTH				168
#define SCREEN_HEIGHT				384

#define SCREEN_NUM_GREYS			5
#define SCREEN_FIRST_GREY_IDX		0
#define SCREEN_EXTRA_COLOR_INDEX	5		//set to negative if nonexistent
#define SCREEN_TX_BPP				4		//in transit

#define SCREEN_WIDTH_MM				29
#define SCREEN_HEIGHT_MM			67

#define SCREEN_BYTE_FILL			0x44	//white

#define SCREEN_TYPE					TagScreenEink_BWR_6colors

#define SCREEN_DATA_PASSES			2

#endif
