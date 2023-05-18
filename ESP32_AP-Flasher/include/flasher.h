#include <Arduino.h>

uint16_t getAPUpdateVersion(uint8_t type);
bool checkForcedAPFlash();
bool doForcedAPFlash();
bool doAPFlash();
bool doAPUpdate(uint8_t type);
void flashCountDown(uint8_t c);

#ifdef OPENEPAPERLINK_PCB
bool extTagConnected();
bool doTagFlash();
#endif