#include "sleep.h"
#include "cpu.h"

void sleepForMsec(uint32_t length)
{
	__bit irqEn = IEN_EA;
	uint8_t prescaler, cfgPg;
	IEN_EA = 0;
	
	if (!length)
		length = 0xfffffffful;
	
	RADIO_IRQ4_pending = 0;
	UNK_C1 &=~ 0x81;
	TCON &=~ 0x20;
	cfgPg = CFGPAGE;
	CFGPAGE = 4;
	RADIO_command = 0xCA;
	RADIO_command = 0xC5;

	if (length <= 0x00008000ul) {
		
		length <<= 5;
		prescaler = 0x56;		//0x56 = one tick is 1/32k of sec
	}
	else {
		if (length != 0xfffffffful)
			length += 500;
		
		length /= 1000;
		prescaler = 0x16;		//0x16 = one tick is 1 second
	}
	
	if (length > 0x000fffff) {
		
		RADIO_SleepTimerLo = 0xff;
		RADIO_SleepTimerMid = 0xff;
		RADIO_SleepTimerHi = 0x0f;
	}
	else {
		
		RADIO_SleepTimerLo = length;
		RADIO_SleepTimerMid = length >> 8;
		RADIO_SleepTimerHi = ((uint8_t)(length >> 16)) & 0x0f;
	}

	__asm__("nop");
	RADIO_SleepTimerSettings = prescaler;
	__asm__("nop\nnop\nnop\nnop\n");
	RADIO_SleepTimerSettings |= 0x80;
	__asm__("nop\nnop\n");
	RADIO_RadioPowerCtl = 0x44;
	__asm__("nop\nnop\n");
	
	CFGPAGE = cfgPg;
	
	//make sure time does not run backwards
	TL0 = 0x0;
	TH0 = 0xFF;
	while (TH0 == 0xFF);
	
	UNK_C1 |= 0x81;
	TCON |= 0x20;
	
	IEN_EA = irqEn;
}