#include <stdbool.h>
#include "printf.h"
#include "screen.h"
#include "board.h"
#include "flash.h"
#include "uart.h"
#include "spi.h"
#include "cpu.h"
#include "wdt.h"
#include "adc.h"


void powerPortsDownForSleep(void)
{
	P0FUNC = 0;
	P1FUNC = 0;
	P2FUNC = 0;
	P0DIR = 0;
	P0 = 0;
	P0PULL = 0;
	P1DIR = 0;
	P1 = 2;
	P1PULL = 0;
	P2DIR = 2;
	P2 =1;
	P2PULL = 0;
}

void boardInit(void)
{
	wdtOff();
	
	//set up pins for spi(0.0,0.1,0.2), UART (0.6)
	P0FUNC |= (1 << 0) | (1 << 1) | (1 << 2) | (1 << 6);
	P0DIR = (P0DIR &~ ((1 << 0) | (1 << 1) | (1 << 6))) | (1 << 2);
	
	//pulls for spi in
	P0PULL = (P0PULL &~ ((1 << 0) | (1 << 1) | (1 << 6))) | (1 << 2);

	//setup 1.1(eeprom_nCS), 1.2(eink_BS1), 1.7(eink_nCS)
	P1FUNC &=~ ((1 << 1) | (1 << 2) | (1 << 7));
	P1DIR &= ~((1 << 1) | (1 << 2) | (1 << 7));
	
	//setup 2.0(eink_nRST), 2.1(eink_BUSY), 2.2(eink_D/nC)
	P2FUNC &= ~((1 << 0) | (1 << 1) | (1 << 2));
	P2DIR = (P2DIR &~ ((1 << 0) | (1 << 2))) | (1 << 1);
	
	//raise chip select(s)
	P1_1 = 1;
	P1_7 = 1;
	
	//BS1 = low
	P1_2 = 0;
	
	uartInit();
	spiInit();
}

void boardInitStage2(void)
{
	//nothing yet
}

__bit boardGetOwnMac(uint8_t __xdata *mac)
{
	return flashRead(FLASH_INFOPAGE_ADDR + 0x10, mac, 8);
}