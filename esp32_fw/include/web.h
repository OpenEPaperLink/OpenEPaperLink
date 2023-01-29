#include <Arduino.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

void init_web();
void doImageUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final);

extern void webSocketSendProcess(void *parameter);
extern void wsString(String text);
extern void wsSendTaginfo(uint8_t src[8]);

extern uint64_t swap64(uint64_t x);
extern AsyncWebSocket ws;//("/ws");

extern SemaphoreHandle_t wsMutex;
extern TaskHandle_t websocketUpdater;