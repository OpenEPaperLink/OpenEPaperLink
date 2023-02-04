#include <Arduino.h>

#include <LittleFS.h>
#include "makeimage.h"
#include <time.h>
#include "tag_db.h"
#include <TFT_eSPI.h>

void contentRunner();
void drawNew(uint8_t mac[8], bool buttonPressed, tagRecord *&taginfo);
bool updateTagImage(String &filename, uint8_t *dst, uint16_t nextCheckin);
void drawDate(String &filename, tagRecord *&taginfo);
void drawNumber(String &filename, int32_t count, int32_t thresholdred, tagRecord *&taginfo);
void drawWeather(String &filename, String location, tagRecord *&taginfo);
bool getImgURL(String &filename, String URL, time_t fetched);
char *formatHttpDate(time_t t);
String urlEncode(const char *msg);
int windSpeedToBeaufort(float windSpeed);
String windDirectionIcon(int degrees);
