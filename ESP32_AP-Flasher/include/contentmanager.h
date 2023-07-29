#include <Arduino.h>
#include <LittleFS.h>
#include <TFT_eSPI.h>

#include "U8g2_for_TFT_eSPI.h"
#include "makeimage.h"
#include "tag_db.h"

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
bool updateTagImage(String &filename, uint8_t *dst, uint16_t nextCheckin, tagRecord *&taginfo, imgParam &imageParams);
void drawString(TFT_eSprite &spr, String content, uint16_t posx, uint16_t posy, String font, byte align = 0, uint16_t color = TFT_BLACK, uint16_t size = 0);
void initSprite(TFT_eSprite &spr, int w, int h, imgParam &imageParams);
void drawDate(String &filename, tagRecord *&taginfo, imgParam &imageParams);
void drawNumber(String &filename, int32_t count, int32_t thresholdred, tagRecord *&taginfo, imgParam &imageParams);
void drawWeather(String &filename, JsonObject &cfgobj, tagRecord *&taginfo, imgParam &imageParams);
void drawForecast(String &filename, JsonObject &cfgobj, tagRecord *&taginfo, imgParam &imageParams);
void drawIdentify(String &filename, tagRecord *&taginfo, imgParam &imageParams);
int getImgURL(String &filename, String URL, time_t fetched, imgParam &imageParams, String MAC);
bool getRssFeed(String &filename, String URL, String title, tagRecord *&taginfo, imgParam &imageParams);
bool getCalFeed(String &filename, String URL, String title, tagRecord *&taginfo, imgParam &imageParams);
void drawQR(String &filename, String qrcontent, String title, tagRecord *&taginfo, imgParam &imageParams);
uint8_t drawBuienradar(String &filename, JsonObject &cfgobj, tagRecord *&taginfo, imgParam &imageParams);
int getJsonTemplate(String URL, JsonDocument &jsondoc, time_t fetched, String MAC);
void drawJsonTemplate(JsonDocument &doc, String filename, tagRecord *&taginfo, imgParam &imageParams);
void drawElement(const JsonObject &element, TFT_eSprite &spr);
uint16_t getColor(uint8_t color);
char *formatHttpDate(time_t t);
String urlEncode(const char *msg);
int windSpeedToBeaufort(float windSpeed);
String windDirectionIcon(int degrees);
void getLocation(JsonObject &cfgobj);
void prepareNFCReq(uint8_t* dst, const char* url);
void prepareLUTreq(uint8_t *dst, String input);
void prepareConfigFile(uint8_t *dst, JsonObject config);
void getTemplate(JsonDocument &json, const char *filePath, uint8_t id, uint8_t hwtype);
void setU8G2Font(const String &title, U8g2_for_TFT_eSPI &u8f);
void showIpAddress(String dst);
