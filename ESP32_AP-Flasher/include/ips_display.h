#include <Arduino.h>
#include <TFT_eSPI.h>

#ifdef YELLOW_IPS_AP

extern TFT_eSPI tft2;
extern int32_t tftid;
extern uint8_t YellowSense;
extern bool tftOverride;

void TFTLog(String text);
void sendAvail(uint8_t wakeupReason);

#endif
