#ifndef _COMMS_H_
#define _COMMS_H_

#include <stdint.h>

extern uint8_t  mLastLqi;
extern int8_t  mLastRSSI;

#define COMMS_RX_ERR_NO_PACKETS			(-1)

uint8_t commsGetLastPacketLQI(void);
int8_t commsGetLastPacketRSSI(void);

int32_t commsRxUnenc(void *data);

void commsTxNoCpy(uint8_t *packetp);

#endif
