#ifndef _EEPROM_H_
#define _EEPROM_H_

#include <stdint.h>

#define EEPROM_WRITE_PAGE_SZ		256		//max write size & alignment
#define EEPROM_ERZ_SECTOR_SZ		4096	//erase size and alignment
//pages are 4K in size
//an update can be stored in any 2 image slots
#define EEPROM_UPDATA_AREA_START		(0x17000UL)
#define EEPROM_UPDATE_AREA_LEN			(0x10000UL)
#define EEPROM_PAGE_SIZE			(0x01000UL)

#define EEPROM_OS_START				(0x00000UL)
#define EEPROM_OS_LEN				(0x13FFFUL)	//0xE820 of image, rounded up to 4K

#define EEPROM_IMG_START			(0x17000UL)
#define EEPROM_IMG_EACH				(0x1F000UL)
#define EEPROM_IMG_LEN				(EEPROM_IMG_START + 0x13FFFUL)

#define EEPROM_UPDATE_START			(0x17000UL)	//same header as images
#define EEPROM_UPDATE_LEN			(0x13FFFUL)

#define EEPROM_SETTINGS_AREA_START		(0x14000UL)
#define EEPROM_SETTINGS_AREA_LEN		(0x03000UL)

#define EEPROM_MAC_INFO_START		(0x6c000UL)	//not same as stock
#define EEPROM_MAC_INFO_LEN			(0x01000UL)


#define EEPROM_IMG_INPROGRESS		(0x7fffffff)
#define EEPROM_IMG_VALID			(0x494d4721)


#define EEPROM_PIECE_SZ				(88)
struct EepromImageHeaderOld {
	uint64_t version;
	uint32_t validMarker;
	uint32_t size;
	uint32_t rfu[8];				//zero-filled for now
	uint8_t piecesMissing[256];		//each bit represents a 64-byte piece
	
	//image data here
	//os update here possibly (EEPROM_OS_UPDATE_SZ_PER_IMG bytes each piece)
	//we pre-erase so progress can be calculated by finding the first non-0xff byte
};

struct EepromImageHeader {				//each image space is 0x17000 bytes, we have space for ten of them
	uint64_t version;
	uint32_t validMarker;
	uint32_t size;
	uint8_t dataType;
	uint32_t id;
	
	//image data here
	//we pre-erase so progress can be calculated by finding the first non-0xff byte
};


#endif
