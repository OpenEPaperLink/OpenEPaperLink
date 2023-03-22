#include <Arduino.h>

#include <LittleFS.h>
#include "makeimage.h"
#include <time.h>
#include "tag_db.h"
#include <TFT_eSPI.h>

struct contentTypes {
    uint16_t id;
    String name;
	uint16_t tagTypes;
    void (*functionname)();
	String description;
	String optionList;
};

void contentRunner();
void drawNew(uint8_t mac[8], bool buttonPressed, tagRecord *&taginfo);
bool updateTagImage(String &filename, uint8_t *dst, uint16_t nextCheckin, imgParam &imageParams);
void drawString(TFT_eSprite &spr, String content, uint16_t posx, uint16_t posy, String font, byte align = 0, uint16_t color = TFT_BLACK);
void initSprite(TFT_eSprite &spr, int w, int h);
void drawDate(String &filename, tagRecord *&taginfo, imgParam &imageParams);
void drawNumber(String &filename, int32_t count, int32_t thresholdred, tagRecord *&taginfo, imgParam &imageParams);
void drawWeather(String &filename, String location, tagRecord *&taginfo, imgParam &imageParams);
void drawForecast(String &filename, String location, tagRecord *&taginfo, imgParam &imageParams);
void drawIdentify(String &filename, tagRecord *&taginfo, imgParam &imageParams);
bool getImgURL(String &filename, String URL, time_t fetched, imgParam &imageParams);
bool getRSSfeed(String &filename, String URL, String title, tagRecord *&taginfo, imgParam &imageParams);
char *formatHttpDate(time_t t);
String urlEncode(const char *msg);
int windSpeedToBeaufort(float windSpeed);
String windDirectionIcon(int degrees);
String mac62hex(uint8_t *mac);
