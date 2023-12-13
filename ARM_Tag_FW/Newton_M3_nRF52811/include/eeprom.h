#ifndef _EEPROM_H_
#define _EEPROM_H_

#include <stdbool.h>
#include <stdint.h>

#define EEPROM_WRITE_PAGE_SZ		256		//max write size & alignment
#define EEPROM_ERZ_SECTOR_SZ		4096	//erase size and alignment

//device has 256 sectors, so eepromErase() cannot erase thw whole device...i can live with that

bool eepromInit(void);
void eepromOtpModeEnter(void);
void eepromOtpModeExit(void);

void eepromRead(uint32_t addr, void  *dst, uint16_t len) ;

bool eepromWrite(uint32_t addr, const void  *src, uint16_t len) ;

bool eepromErase(uint32_t addr, uint16_t numSectors) ;

void eepromDeepPowerDown(void);

uint32_t eepromGetSize(void);

//this is for firmware update use
void eepromReadStart(uint32_t addr)  ;

//structures
#define EEPROM_IMG_INPROGRESS			(0x7fffffffUL)
#define EEPROM_IMG_VALID				(0x494d4721UL)

struct EepromImageHeader {				
	uint64_t version;
	uint32_t validMarker;
	uint32_t size;
	uint8_t dataType;
	uint32_t id;
	uint8_t argument;
};

#endif
