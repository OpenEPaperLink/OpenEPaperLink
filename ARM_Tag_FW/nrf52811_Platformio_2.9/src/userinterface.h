#ifndef _UI_H_
#define _UI_H_
#include <Arduino.h>
#include <stdint.h>

void addOverlay();

void afterFlashScreenSaver();
void showSplashScreen();
void showApplyUpdate();
void showScanningWindow();
void addScanResult(uint8_t channel, uint8_t lqi);
void showAPFound();
void showNoAP();
void showLongTermSleep();
void showNoEEPROM();
void showNoMAC();

extern const uint8_t fwVersion;
extern const char fwVersionSuffix[];
extern bool lowBatteryShown;
extern bool noAPShown;

#endif