#include <Arduino.h>

extern struct espSetChannelPower curChannel;
extern uint16_t version;

void zbsTx(uint8_t* packetdata, uint8_t len);
void zbsRxTask(void* parameter);

void sendCancelPending(struct pendingData* pending);
void sendDataAvail(struct pendingData* pending);
void Ping();

bool sendChannelPower(struct espSetChannelPower* scp);