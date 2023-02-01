#include "spi.h"
#include "cpu.h"


void spiInit(void)
{
	uint8_t bcp;
	
	//clock it up
	CLKEN |= 0x08;

	//enable the unit
	bcp = CFGPAGE;
	CFGPAGE = 4;
	SPIENA = 0x81;
	CFGPAGE = bcp;
}

uint8_t spiByte(uint8_t val)
{
	uint8_t bcp = CFGPAGE;
	CFGPAGE = 4;
	
	SPITX = val;
	SPICFG = 0xa0;	//spi at 4mhz, mode 0
	while(SPICFG & 0x20);
	val = SPIRX;
	
	CFGPAGE = bcp;
	
	return val;
}

void spiTXByte(uint8_t val)
{
	uint8_t bcp = CFGPAGE;
	CFGPAGE = 4;
	SPITX = val;
	SPICFG = 0xa0;	//spi at 4mhz, mode 0
	while(SPICFG & 0x20);	
	CFGPAGE = bcp;
}