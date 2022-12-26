#ifndef _FLASH_ZBS_H_
#define _FLASH_ZBS_H_

#include <stdint.h>

#define FLASH_PAGE_SHIFT		(10)
#define FLASH_PAGE_SIZE			(1 << FLASH_PAGE_SHIFT)
#define FLASH_NUM_PAGES			(64)

#define FLASH_INFOPAGE_ADDR		(0x00800000ul)



#pragma callee_saves flashErase
__bit flashErase(uint32_t dstAddr);

#pragma callee_saves flashRead
__bit flashRead(uint32_t srcAddr, void __xdata *dst, uint16_t len);
	
#pragma callee_saves flashWrite
__bit flashWrite(uint32_t dstAddr, const void __xdata *src, uint16_t len, __bit alsoErase /*whole page */);

#endif
