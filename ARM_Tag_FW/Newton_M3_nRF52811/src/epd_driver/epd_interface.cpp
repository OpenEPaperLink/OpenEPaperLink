
#include <Arduino.h>
#include "hal.h"
#include "epd_driver/epd_interface.h"


void epdSetup() {
    epd->epdSetup();
}

void epdEnterSleep() {
    epd->epdEnterSleep();
}
void draw() {
    epd->draw();
}
void drawNoWait() {
    epd->drawNoWait();
}

void epdWaitRdy() {
    epd->epdWaitRdy();
}

void selectLUT(uint8_t sel) {
}
