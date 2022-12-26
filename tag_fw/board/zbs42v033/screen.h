#ifndef _SCREEN_H_
#define _SCREEN_H_

#include <stdbool.h>
#include <stdint.h>


//i hate globals, but for 8051 this makes life a lot easier, sorry :(
extern uint8_t __xdata mScreenVcom;
extern int8_t __xdata mCurTemperature;


#define SCREEN_WIDTH				400
#define SCREEN_HEIGHT				300

#define SCREEN_NUM_GREYS			5
#define SCREEN_FIRST_GREY_IDX		0
#define SCREEN_EXTRA_COLOR_INDEX	5		//set to negative if nonexistent
#define SCREEN_TX_BPP				4		//in transit

#define SCREEN_WIDTH_MM				84
#define SCREEN_HEIGHT_MM			63

#define SCREEN_BYTE_FILL			0x44	//white

#define SCREEN_TYPE					TagScreenEink_BWR_6colors

#define SCREEN_DATA_PASSES			2

void screenShutdown(void);


void screenTest(void);

__bit screenTxStart(__bit forPartial);

void screenEndPass(void);	//at end of each pass

#pragma callee_saves screenByteTx
void screenByteTx(uint8_t byte);
void screenTxEnd(void);

void screenSleep(void);


extern uint8_t __xdata mScreenRow[];	//320 bytes used as temp by many on cc where memory is tight

#endif


