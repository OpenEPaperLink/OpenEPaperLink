#include <stdbool.h>
#include "asmUtil.h"
#include "printf.h"
#include "flash.h"
#include "cpu.h"



#pragma callee_saves flashAddrCheck
static uint8_t flashAddrCheck(uint32_t flashAddr, uint16_t len) __reentrant /* to save ram space */ // return 0xff for error, pgNo
{
	uint16_t dstOfst;
	uint8_t pgNo;
	
	if (!len)
		return 0xff;
	
	//verify address
	if ((uint8_t)(flashAddr >> 24))
		return 0xff;
	
	switch ((uint8_t)(flashAddr >> 16))  {
		case 0x00:
			pgNo = ((uint8_t)(flashAddr >> 8)) >> 2;
			break;
		
		case 0x80:
			if (pgNo)//hmm
				return 0xff;
			pgNo = 0x80;
			break;
		
		default:
			return 0xff;
	}
	
	dstOfst = flashAddr & 0x3ff;
	//verify no boundary crossing
	if (((uint8_t)((dstOfst + len - 1) >> 8)) >> 2)	//yeah...sdcc
		return 0xff;
	
	return pgNo;
}

//info page is "at" 0x10000
#pragma callee_saves flashWrite
__bit flashWrite(uint32_t dstAddr, const void __xdata *src, uint16_t len, __bit alsoErase /*whole page */)
{
	uint8_t pgNo, cfgPg, speed;
	__bit irq;
	
	pgNo = flashAddrCheck(dstAddr, len);
	if (pgNo == 0xff)
		return false;

	irq = IEN_EA;
	IEN_EA = false;
	
	speed = CLKSPEED;
	CLKSPEED = 0x21;	//flash ops only work at this speed
	
	cfgPg = CFGPAGE;
	CFGPAGE = 4;
	
	if (alsoErase)
		SETTINGS |= 0x38;
	else
		SETTINGS = 0x18;
	FWRTHREE = 3;
	FPGNO = pgNo;
	FWRDSTL = (uint8_t)dstAddr;
	FWRDSTH = (((uint16_t)dstAddr) >> 8) & 0x03;
	FWRLENL = (uint8_t)(len - 1);
	FWRLENH = (len - 1) >> 8;
	FWRSRCL = (uint8_t)src;
	FWRSRCH = ((uint16_t)src) >> 8;
	TRIGGER |= 8;				//start
	while (!(TCON2 & 0x08));	//wait
	
	TCON2 &=~ 0x48;
	SETTINGS &=~ 0x10;
	
	CFGPAGE = cfgPg;
	
	CLKSPEED = speed;
	
	IEN_EA = irq;

	return true;
}

#pragma callee_saves flashRead
__bit flashRead(uint32_t srcAddr, void __xdata *dst, uint16_t len)
{
	uint8_t pgNo, cfgPg, speed;
	__bit irq;
	
	pgNo = flashAddrCheck(srcAddr, len);
	if (pgNo == 0xff)
		return false;

	irq = IEN_EA;
	IEN_EA = false;
	
	speed = CLKSPEED;
	CLKSPEED = 0x21;	//flash ops only work at this speed
	
	cfgPg = CFGPAGE;
	CFGPAGE = 4;
	
	SETTINGS = 0x8;
	FWRTHREE = 3;
	FPGNO = pgNo;
	FWRDSTL = (uint8_t)dst;
	FWRDSTH = ((uint16_t)dst) >> 8;
	FWRSRCL = (uint8_t)srcAddr;
	FWRSRCH = (((uint16_t)srcAddr) >> 8) & 0x03;
	FWRLENL = (uint8_t)(len - 1);
	FWRLENH = (len - 1) >> 8;
	TRIGGER |= 8;				//start

	while (!(TCON2 & 0x08));	//wait

	TCON2 &=~ 0x48;
	SETTINGS &=~ 0x10;
	
	CFGPAGE = cfgPg;
	
	CLKSPEED = speed;
	
	IEN_EA = irq;

	return true;
}

#pragma callee_saves flashErase
__bit  flashErase(uint32_t dstAddr)
{
	uint8_t __xdata dummyByte = 0xff;
	uint8_t pgNo, cfgPg, speed;
	__bit irq;
	
	pgNo = flashAddrCheck(dstAddr, 1);
	
	if (pgNo == 0xff)
		return false;

	irq = IEN_EA;
	IEN_EA = false;
	
	speed = CLKSPEED;
	CLKSPEED = 0x21;	//flash ops only work at this speed
	
	cfgPg = CFGPAGE;
	CFGPAGE = 4;
	
	//this command does an erase AND a write (erase of page, write up to a page at given offset and len)
	// i found no way to JUST erase. As len is encoded at "minus one", we cannot ask the hardware to write
	// zeor bytes, so we write one - a 0xFF dummy
	SETTINGS |= 0x38;
	FWRTHREE = 3;
	FPGNO = pgNo;
	FWRDSTL = 0;
	FWRDSTH = 0;
	FWRLENL = 0;
	FWRLENH = 0;
	FWRSRCL = (uint8_t)&dummyByte;
	FWRSRCH = ((uint16_t)&dummyByte) >> 8;
	TRIGGER |= 8;				//start
	while (!(TCON2 & 0x08));	//wait
	
	TCON2 &=~ 0x48;
	SETTINGS &=~ 0x10;
	
	CFGPAGE = cfgPg;
	
	CLKSPEED = speed;
	
	IEN_EA = irq;

	return true;
}
