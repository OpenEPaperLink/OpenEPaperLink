#ifndef _COMMS_H_
#define _COMMS_H_

#include <stdint.h>

#define COMMS_MAX_RADIO_WAIT_MSEC		200

#define COMMS_IV_SIZE					(4)		//zeroes except these 4 counter bytes

#define COMMS_RX_ERR_NO_PACKETS			(-1)
#define COMMS_RX_ERR_INVALID_PACKET		(-2)
#define COMMS_RX_ERR_MIC_FAIL			(-3)

#define COMMS_MAX_PACKET_SZ				(127)

int8_t commsRxUnencrypted(void __xdata *data);
bool commsTxUnencrypted(const void __xdata *packetP, uint8_t len);
bool commsTxNoCpy(const void __xdata *packetp);

static uint8_t __xdata mLastLqi;
static int8_t __xdata mLastRSSI;

#pragma callee_saves commsGetLastPacketLQI
uint8_t commsGetLastPacketLQI(void);

#pragma callee_saves commsGetLastPacketRSSI
int8_t commsGetLastPacketRSSI(void);


#endif
