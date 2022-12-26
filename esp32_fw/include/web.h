#include <Arduino.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

void init_web();

extern void webSocketSendProcess(void *parameter);
extern void wsString(String text);
extern uint64_t swap64(uint64_t x);
extern AsyncWebSocket ws;//("/ws");

extern SemaphoreHandle_t wsMutex;
extern TaskHandle_t websocketUpdater;