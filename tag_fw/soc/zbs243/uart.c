#include "uart.h"
#include "cpu.h"



void uartInit(void)
{
	//clock it up
	CLKEN |= 0x20;
	
	//configure
	UARTBRGH = 0x00;	//config for 115200
	UARTBRGL = 0x8A;
	UARTSTA = 0x12;		//also set the "empty" bit else we wait forever for it to go up
}

void uartTx(uint8_t val)
{
	while (!(UARTSTA & (1 << 1)));
	UARTSTA &=~ (1 << 1);
	UARTBUF = val;
}


