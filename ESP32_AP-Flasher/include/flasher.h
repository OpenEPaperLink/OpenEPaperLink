#include <Arduino.h>

uint16_t getAPUpdateVersion(uint8_t type);
bool checkForcedAPFlash();
bool doForcedAPFlash();
bool doAPFlash();
bool doAPUpdate(uint8_t type);
bool doTagFlash();

#define FLASHER_AP_PORT 0
#ifdef OPENEPAPERLINK_PCB
#define FLASHER_EXT_PORT 1
#define FLASHER_ALTRADIO_PORT 2
#endif