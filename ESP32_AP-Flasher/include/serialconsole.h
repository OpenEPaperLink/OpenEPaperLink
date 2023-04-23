#include <Arduino.h>
extern QueueHandle_t consoleCmdQueue;
extern TaskHandle_t consoleTaskHandle;


void consoleStopTask();
void consoleTask(void* parameter);
void consoleUartHandler(uint8_t* data, uint8_t len);