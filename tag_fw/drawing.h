#ifndef _DRAWING_H_
#define _DRAWING_H_

#include <stdint.h>

#define DRAWING_MIN_BITMAP_SIZE		(128)		//minimum size we'll consider

void set_offline(__bit state);
#pragma callee_saves drawImageAtAddress
void drawImageAtAddress(uint32_t addr);

#pragma callee_saves drawImageAtAddress
void drawFullscreenMsg(const char *str);



//expected external funcs
/*
#pragma callee_saves fwVerString
const char __xdata* fwVerString(void);
#pragma callee_saves voltString
const char __xdata* voltString(void);
void getVolt();
*/

#pragma callee_saves macString
//const char __xdata* macString(void);
#pragma callee_saves macSmallString
//const char __xdata* macSmallString(void);

extern uint8_t __xdata mSelfMac[];


#endif
