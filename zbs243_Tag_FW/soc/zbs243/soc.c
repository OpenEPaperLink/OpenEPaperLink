#include "asmUtil.h"
#include "printf.h"
#include "screen.h"
#include "cpu.h"


void clockingAndIntsInit(void)
{
	IEN0 = 0;
	CLKEN = 0x00;		//timers only for now
	CLKSPEED = 0x01;	//fast crystal
}