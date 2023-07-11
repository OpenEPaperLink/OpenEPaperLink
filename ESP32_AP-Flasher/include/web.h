
#include <Arduino.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

void init_web();
void doImageUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final);
void doJsonUpload(AsyncWebServerRequest *request);
extern void webSocketSendProcess(void *parameter);
void wsLog(String text);
void wsErr(String text);
void wsSendTaginfo(uint8_t *mac, uint8_t syncMode);
void wsSendSysteminfo();
void wsSendAPitem(struct APlist* apitem);
void wsSerial(String text);
uint8_t wsClientCount();

extern AsyncWebSocket ws;

extern SemaphoreHandle_t wsMutex;
