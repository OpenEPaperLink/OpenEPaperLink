#ifndef _UI_H_
#define _UI_H_
#include <stdint.h>

void addOverlay();

void showSplashScreen();
void showApplyUpdate();
void showScanningWindow();
void addScanResult(uint8_t channel, uint8_t lqi);
void showAPFound();
void showNoAP();
void showLongTermSleep();
void showNoEEPROM();
void showNoMAC();

extern const uint8_t __code fwVersion;
extern const char __code fwVersionSuffix[];
extern bool __xdata lowBatteryShown;
extern bool __xdata noAPShown;

#endif