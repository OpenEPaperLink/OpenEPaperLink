#include <Arduino.h>
#include <TFT_eSPI.h>

#ifdef YELLOW_IPS_AP

extern TFT_eSPI tft2;
extern int32_t tftid;
extern uint8_t YellowSense;

void TFTLog(String text);

#endif
