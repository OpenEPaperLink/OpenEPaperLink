#ifndef _UI_H_
#define _UI_H_
#include <stdint.h>

void addOverlay();

bool displayCustomImage(uint8_t imagetype);

void afterFlashScreenSaver();
void showSplashScreen();
void showApplyUpdate();
void showAPFound();
void showNoAP();
void showLongTermSleep();
void showNoEEPROM();
void showNoMAC();



// wakeups from external stimuli
void externalWakeHandler(uint8_t type);

extern const uint16_t __code fwVersion;
extern const char __code fwVersionSuffix[];
extern bool __xdata lowBatteryShown;
extern bool __xdata noAPShown;

#endif