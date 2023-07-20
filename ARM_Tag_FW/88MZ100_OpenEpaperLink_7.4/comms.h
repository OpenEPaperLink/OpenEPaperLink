#ifndef _COMMS_H_
#define _COMMS_H_

#include <stdint.h>
#include "ccm.h"

struct CommsInfo {
	const uint8_t *myMac;
	const uint8_t *masterMac;
	const void *encrKey;
	uint32_t *nextIV;
};
extern uint8_t  mLastLqi;
extern int8_t  mLastRSSI;

#define COMMS_MAX_RADIO_WAIT_MSEC		100

#define COMMS_IV_SIZE					(4)		//zeroes except these 4 counter bytes

#define COMMS_RX_ERR_NO_PACKETS			(-1)
#define COMMS_RX_ERR_INVALID_PACKET		(-2)
#define COMMS_RX_ERR_MIC_FAIL			(-3)

#define COMMS_MAX_PACKET_SZ				(127 /* max phy len */ - 21 /* max mac frame with panID compression */ - 2 /* FCS len */ - AES_CCM_MIC_SIZE - COMMS_IV_SIZE)

bool commsTx(struct CommsInfo *info, bool bcast, const void *packet, uint32_t len);
int32_t commsRx(struct CommsInfo *info, void *data, uint8_t *fromMac);	//returns length or COMMS_RX_ERR_*

uint8_t commsGetLastPacketLQI(void);
int8_t commsGetLastPacketRSSI(void);

int32_t commsRxUnenc(void *data);


void commsTxNoCpy(uint8_t *packetp);

#endif
