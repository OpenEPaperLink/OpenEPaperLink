#ifndef _TIMER_H_
#define _TIMER_H_

#include <stdint.h>

#define TIMER_TICKS_PER_SECOND			(16000000 / 12)		//overflows every 53 minutes
#define TIMER_TICKS_PER_MS              1333UL
//this is a requirement by SDCC. is this prototype is missing when compiling main(), we get no irq handler
void T0_ISR(void) __interrupt (1);


void timerInit(void);

#pragma callee_saves timerGet
uint32_t timerGet(void);

#pragma callee_saves timerGetLowBits
uint8_t timerGetLowBits(void);	//probaly only useful for random seeds

void timerDelay(uint32_t ticks);



#endif
