#ifndef _UI_H_
#define _UI_H_
#include <stdint.h>

void showSplashScreen();
void showApplyUpdate();
void showScanningWindow();
void addScanResult(uint8_t channel, uint8_t lqi);
void showAPFound();
void showNoAP();
#endif