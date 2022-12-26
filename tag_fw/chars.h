#ifndef _CHARS_H_
#define _CHARS_H_

#include <stdbool.h>
#include <stdint.h>


#define CHAR_WIDTH		8
#define CHAR_HEIGHT		16

#define CHAR_COLOR_TRANSPARENT	(0xff)

#define CHAR_SIGNAL_PT1			(0x1c)
#define CHAR_SIGNAL_PT2			(0x1d)

#define CHAR_NO_SIGNAL_PT1		(0x1e)
#define CHAR_NO_SIGNAL_PT2		(0x1f)

struct CharDrawingParams {
	const char *str;
	int16_t x;
	uint8_t imgRow;
	uint8_t foreColor;
	uint8_t backColor;
	uint8_t magnify;
}; 

//draws to screen the rows of this string. will draw 16 * magnify rows
void charsDrawString(const struct CharDrawingParams __xdata *params);


#endif
