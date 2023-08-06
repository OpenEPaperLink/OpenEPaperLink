
#ifdef YELLOW_IPS_AP
#include <Arduino.h>
#include <Arduino_GFX_Library.h>
#include <WiFi.h>

#include "tag_db.h"

Arduino_DataBus *bus = new Arduino_ESP32SPI(11 /* DC */, 10 /* CS */, 12 /* SCK */, 13 /* MOSI */, GFX_NOT_DEFINED /* MISO */);
Arduino_GFX *gfx = new Arduino_ST7789(bus, 1 /* RST */, 3 /* rotation */, true /* IPS */, 170 /* width */, 320 /* height */, 35 /* col offset 1 */, 0 /* row offset 1 */, 35 /* col offset 2 */, 0 /* row offset 2 */);

void yellow_ap_display_init(void) {
    gfx->begin();
    gfx->fillScreen(BLACK);
    gfx->setCursor(10, 10);
    gfx->setTextColor(RED);
    gfx->println("OpenEPaperLink");
    gfx->setCursor(10, 40);
    gfx->setTextColor(GREEN);
    gfx->println("Also on 16MB Flash 8MB RAM");
    gfx->setCursor(10, 80);
    gfx->setTextColor(WHITE);
    gfx->println("Booting");
}

bool first_clear = 0;
uint32_t last_update = 0;
void yellow_ap_display_loop(void) {
    if (millis() - last_update >= 100) {
        if (first_clear == 0) {
            first_clear = 1;
            gfx->fillScreen(BLACK);
            gfx->setTextSize(3);
        }
        last_update = millis();
        gfx->setCursor(0, 0);
        gfx->setTextColor(RED, BLACK);
        gfx->println("OpenEPaperLink");
        gfx->setTextColor(GREEN, BLACK);
        gfx->println("Millis :");
        gfx->println(millis());
        gfx->setTextColor(WHITE, BLACK);
        gfx->println("IP Address :");
        gfx->println(WiFi.localIP().toString());
        gfx->setTextColor(WHITE, BLACK);
        gfx->println("Tag Count :");
        gfx->println(getTagCount());
    }
}
#endif