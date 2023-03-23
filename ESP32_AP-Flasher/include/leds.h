#include <Arduino.h>

#ifdef OPENEPAPERLINK_PCB
#include <FastLED.h>
#endif 

void ledTask(void* parameter);

#ifdef OPENEPAPERLINK_PCB
void shortBlink(CRGB cname);
#endif