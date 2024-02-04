#include <Arduino.h>
#include <TFT_eSPI.h>

#ifdef HAS_TFT

extern TFT_eSPI tft2;
extern uint8_t YellowSense;
extern bool tftOverride;

void TFTLog(String text);
void sendAvail(uint8_t wakeupReason);

#endif
