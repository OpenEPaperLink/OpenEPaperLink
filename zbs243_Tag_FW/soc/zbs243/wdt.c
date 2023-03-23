#include "cpu.h"
#include "wdt.h"

void wdtOn(void)
{
	uint8_t cfgPageBck;
	
	cfgPageBck = CFGPAGE;
	CFGPAGE = 4;
	WDTCONF |= 0x80;
	WDTENA = 1;
	CFGPAGE = cfgPageBck;
}

void wdtOff(void)
{
	uint8_t cfgPageBck;
	
	cfgPageBck = CFGPAGE;
	CFGPAGE = 4;
	WDTENA = 0;
	WDTCONF &=~ 0x80;
	CFGPAGE = cfgPageBck;
}

void wdtPet(void)
{
	uint8_t cfgPageBck;
	
	cfgPageBck = CFGPAGE;
	CFGPAGE = 4;
	WDTPET = 0;
	CFGPAGE = cfgPageBck;
}

void wdtSetResetVal(uint32_t val)	//also pets it
{
	uint8_t cfgPageBck;
	
	cfgPageBck = CFGPAGE;
	CFGPAGE = 4;
	WDTPET = 0;
	WDTRSTVALH = val >> 16;
	WDTRSTVALM = val >> 8;
	WDTRSTVALL = val;
	
	CFGPAGE = cfgPageBck;
}

void wdtDeviceReset(void)
{
	CFGPAGE = 4;
	WDTCONF = 0x80;
	WDTENA = 1;
	WDTRSTVALH = 0xff;
	WDTRSTVALM = 0xff;
	WDTRSTVALL = 0xff;
	while(1);
}