
#include <Arduino.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

void init_web();
void doImageUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final);
void doJsonUpload(AsyncWebServerRequest *request);
void dotagDBUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final);
void wsLog(const String &text);
void wsErr(const String &text);
void wsSendTaginfo(const uint8_t *mac, uint8_t syncMode);
void wsSendSysteminfo();
void wsSendAPitem(struct APlist *apitem);
void wsSerial(const String &text);
void wsSerial(const String &text, const String &color);
void wsSendTouch(uint8_t count, const uint16_t *xs, const uint16_t *ys, const uint8_t *ids, const uint8_t *sizes);
void wsSendUploadProgress(const uint8_t *mac, uint16_t current, uint16_t total);
uint8_t wsClientCount();

extern AsyncWebSocket ws;
extern SemaphoreHandle_t wsMutex;
