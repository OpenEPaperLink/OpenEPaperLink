#ifndef _UI_H_
#define _UI_H_
#include <stdint.h>
#include <stdbool.h>

void addOverlay();

void afterFlashScreenSaver();
void showSplashScreen();
void showApplyUpdate();
void showAPFound();
void showNoAP();
void showLongTermSleep();
void showNoEEPROM();
void showNoMAC();

extern const uint16_t fwVersion;
extern const char fwVersionSuffix[];
extern __attribute__((section(".aon"))) bool lowBatteryShown;
extern __attribute__((section(".aon"))) bool noAPShown;

#endif