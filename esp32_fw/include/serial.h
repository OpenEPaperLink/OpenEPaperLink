#include <Arduino.h>

void zbsTx(uint8_t* packetdata, uint8_t len);
void zbsRxTask(void* parameter);

void sendDataAvail(struct pendingData* pending);