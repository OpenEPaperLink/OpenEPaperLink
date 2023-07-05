#include <Arduino.h>

#include "web.h"

void handleSysinfoRequest(AsyncWebServerRequest* request);
void handleCheckFile(AsyncWebServerRequest* request);
void handleGetExtUrl(AsyncWebServerRequest* request);
void handleLittleFSUpload(AsyncWebServerRequest* request, String filename, size_t index, uint8_t* data, size_t len, bool final);
void handleUpdateOTA(AsyncWebServerRequest* request);
void firmwareUpdateTask(void* parameter);
void updateFirmwareFromURL(const char* url, const char* expectedMd5, size_t size);
bool executeUpdate(const char* expectedMd5, size_t size, Stream &stream, size_t stream_size);
void handleRollback(AsyncWebServerRequest* request);
void handleUpdateActions(AsyncWebServerRequest* request);
