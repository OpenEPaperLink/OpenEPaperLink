#ifndef _TIMER_H_
#define _TIMER_H_

#include <stdint.h>

#define TIMER_TICKS_PER_SEC			(32000000)
#define TIMER_TICKS_PER_MSEC		(TIMER_TICKS_PER_SEC / 1000)

void timerInit(void);
uint64_t timerGet(void);
void timerStop(void);

void timerDelay(uint64_t cycles);



#endif
