#ifndef _CHARS_H_
#define _CHARS_H_

#include <stdbool.h>
#include <stdint.h>

#define CHAR_WIDTH		8
#define CHAR_HEIGHT		16

#define CHAR_COLOR_TRANSPARENT	(0xff)

#define CHAR_SIGNAL_PT1			(1)
#define CHAR_SIGNAL_PT2			(2)

#define CHAR_NO_SIGNAL_PT1		(3)
#define CHAR_NO_SIGNAL_PT2		(4)

struct CharDrawingParams
{
	const char *str;
	int16_t x;
	uint8_t imgRow;
	uint8_t foreColor;
	uint8_t backColor;
	uint8_t magnify;
};

void charsDrawString(const struct CharDrawingParams *params);


#endif
