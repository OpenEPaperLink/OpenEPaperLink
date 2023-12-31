#include <stdbool.h>
#include "printf.h"
#include "screen.h"
#include "board.h"
#include "flash.h"
#include "uart.h"
#include "spi.h"
#include "cpu.h"
#include "wdt.h"
#include "i2c.h"
#include <stdlib.h>

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
	
	spiInit();
}


__bit boardGetOwnMac(uint8_t __xdata *mac)
{
	return flashRead(FLASH_INFOPAGE_ADDR + 0x10, mac, 8);
}

#pragma callee_saves prvUpdateApplierGet
static uint32_t prvUpdateApplierGet(void) __naked
{
	__asm__(
		"	mov   DPTR, #00098$			\n"
		"	mov   A, #00099$			\n"
		"	clr   C						\n"
		"	subb  A, DPL				\n"
		"	mov   B, A					\n"
		"	ret							\n"
		
		///actual updater code
		"00098$:						\n"


	//copied to last page of flash for updating, called with ints off and eeprom ready to read update
	//flashes 63 flash pages, uses xram for buffer. uses combined erase+flash flash op
	
		"	mov   _CLKSPEED, #0x21		\n"
		"	mov   _CFGPAGE, #0x04		\n"
		"	mov   R0, #0				\n"
		
		"00001$:						\n"
		
		//read a page of update
		"	mov   DPTR, #0xe000			\n"
		"	mov   R1, #0x04				\n"
		"	mov   R2, #0x00				\n"
		"000010$:						\n"
		"	mov   _SPITX, #0x00			\n"
		"	mov   _SPICFG, #0xa0		\n"
		"000011$:						\n"
		"	mov   A, _SPICFG			\n"
		"	jb    A.5, 000011$			\n"
		"	mov   A, _SPIRX				\n"
		"	movx  @DPTR, A				\n"
		"	inc   DPTR					\n"
		"	djnz  R2, 000010$			\n"
		"	djnz  R1, 000010$			\n"
		
		//flash it
		"	clr   A						\n"
		"	orl   _SETTINGS, #0x38		\n"
		"	mov   _FWRTHREE, #0x03		\n"
		"	mov   _FPGNO, R0			\n"
		"	mov   _FWRDSTL, A			\n"
		"	mov   _FWRDSTH, A			\n"
		"	mov   _FWRLENL, #0xff		\n"
		"	mov   _FWRLENH, #0x03		\n"
		"	mov   _FWRSRCL, A			\n"
		"	mov   _FWRSRCH, #0xe0		\n"
		"	orl   _TRIGGER, #0x08		\n"
		"00050$:						\n"
		"	mov   A, _TCON2				\n"
		"	jnb   A.3, 00050$			\n"
		"	anl   _TCON2, #~0x48		\n"
		"	anl   _SETTINGS, #~0x10		\n"
		
		//go do next page
		"	inc   R0					\n"
		"	cjne  R0, #63, 00001$		\n"
		
		//done? reset
		"	mov   _WDTCONF, #0x80		\n"
		"	mov   _WDTENA, #0x01		\n"
		"	mov   A, #0xff				\n"
		"	mov   _WDTRSTVALH, A		\n"
		"	mov   _WDTRSTVALM, A		\n"
		"	mov   _WDTRSTVALL, A		\n"
		"00090$:						\n"
		"	sjmp  00090$				\n"
		
		"00099$:						\n"
	);
}

void selfUpdate(void)
{

	uint8_t __xdata *tempBuffer;
	tempBuffer = malloc(320);
	uint32_t updaterInfo = prvUpdateApplierGet();
	uint8_t __code *src = (uint8_t __code*)updaterInfo;
	uint8_t i, len = updaterInfo >> 16;
	uint8_t __xdata *dst = tempBuffer;
	if(!tempBuffer)wdtDeviceReset();
	
	for (i = len; i ; i--)
		*dst++ = *src++;

	if (!flashWrite(0xfc00, tempBuffer, len, true))
		pr("failed to write updater\n");

	IEN_EA = 0;	//ints off
	
	__asm__(
		"	mov dptr, #0xfc00			\n"
		"	clr a						\n"
		"	jmp @a+dptr					\n"
	);
}