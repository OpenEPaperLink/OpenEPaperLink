#include <Arduino.h>

#include "web.h"

void handleSysinfoRequest(AsyncWebServerRequest* request);
void handleCheckFile(AsyncWebServerRequest* request);
void handleGetExtUrl(AsyncWebServerRequest* request);
void handleLittleFSUpload(AsyncWebServerRequest* request, String filename, size_t index, uint8_t* data, size_t len, bool final);
void handleUpdateOTA(AsyncWebServerRequest* request);
void firmwareUpdateTask(void* parameter);
void updateFirmware(const char* url, const char* expectedMd5, const size_t size);
void handleRollback(AsyncWebServerRequest* request);
void handleUpdateActions(AsyncWebServerRequest* request);
