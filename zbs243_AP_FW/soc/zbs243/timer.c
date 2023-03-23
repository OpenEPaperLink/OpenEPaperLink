#include "timer.h"
#include "cpu.h"


static volatile uint16_t mTmrHi;

void T0_ISR(void) __interrupt (1)
{
	TCON &=~ 0x20;	//clear flag
	mTmrHi++;
}

uint32_t timerGet(void)
{
	union {
		struct {
			uint8_t tL;
			uint8_t tH;
			uint16_t hi;
		};
		uint32_t ret;
	} val;
	
	do {
		val.hi = mTmrHi;
		val.tH = TH0;
		val.tL = TL0;	//read order is important due ot hardware buffering
	} while (val.hi != mTmrHi || val.tH != TH0);
	
	return val.ret;
}

uint8_t timerGetLowBits(void)
{
	return TL0;
}

void timerInit(void)
{
	//clock up timers
	CLKEN |= 0x01;
	
	
	//stop and clear int flags
	TCON &=~ 0xf0;
	
	//reset
	mTmrHi = 0;
	T0 = 0;
	
	//timer 0 in 16 bit mode, timer 1 off
	TMOD = 0x31;
	
	//start
	TCON |= 0x10;
	
	//int on
	IEN_TMR0 = 1;
}

void timerDelay(uint32_t ticks)
{
	uint32_t start = timerGet();
	while (timerGet() - start <= ticks);
}
