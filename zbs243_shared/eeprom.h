#ifndef _EEPROM_H_
#define _EEPROM_H_

#include <stdbool.h>
#include <stdint.h>

#define EEPROM_WRITE_PAGE_SZ 256   // max write size & alignment
#define EEPROM_ERZ_SECTOR_SZ 4096  // erase size and alignment

// device has 256 sectors, so eepromErase() cannot erase thw whole device...i can live with that

__bit eepromInit(void);
void eepromOtpModeEnter(void);
void eepromOtpModeExit(void);

#pragma callee_saves eepromRead
void eepromRead(uint32_t addr, void __xdata *dst, uint16_t len) __reentrant;

#pragma callee_saves eepromWrite
bool eepromWrite(uint32_t addr, const void __xdata *src, uint16_t len) __reentrant;

#pragma callee_saves eepromErase
bool eepromErase(uint32_t addr, uint16_t numSectors) __reentrant;

void eepromDeepPowerDown(void);

#pragma callee_saves eepromGetSize
uint32_t eepromGetSize(void);

// this is for firmware update use
void eepromReadStart(uint32_t addr) __reentrant;

// structures
#define EEPROM_IMG_INPROGRESS (0x7fffffffUL)
//#define EEPROM_IMG_VALID (0x494d4721UL)
#define EEPROM_IMG_VALID (0x474d4721UL)

#include "board.h"

struct EepromImageHeader {  // each image space is 0x17000 bytes, we have space for ten of them
    uint64_t version;
    uint32_t validMarker;
    uint32_t size;
    uint8_t dataType;
    uint32_t id;
	uint8_t argument;
    // image data here
    // we pre-erase so progress can be calculated by finding the first non-0xff byte
};

#define CUSTOM_IMAGE_NOCUSTOM 0x00         // regular image type
#define CUSTOM_IMAGE_SPLASHSCREEN 0x01     // will show at first boot/powerup
#define CUSTOM_IMAGE_LOST_CONNECTION 0x02  // this image will be shown (if it exists on the tag) if the tag looses its connection
#define CUSTOM_IMAGE_APFOUND 0x03          // this image will be shown during bootup if an AP was found
#define CUSTOM_IMAGE_NOAPFOUND 0x04        // shown if during bootup no AP was found
// UNUSED: 0x05-0x0F
#define CUSTOM_IMAGE_SLIDESHOW 0x0F  // image is part of a slideshow
#define CUSTOM_IMAGE_BUTTON1 0x10
#define CUSTOM_IMAGE_BUTTON2 0x11
// UNUSED: 0x12 to 0x1C
#define CUSTOM_IMAGE_GPIO 0x1D
#define CUSTOM_IMAGE_NFC_WAKE 0x1E



#endif
