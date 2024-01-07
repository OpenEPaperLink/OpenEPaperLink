#include <string.h>
#include <stdbool.h>
#include "tl_common.h"
#include "proto.h"
#include "zigbee.h"
#include "comms.h"

#define ADDR_MODE_NONE (0)
#define ADDR_MODE_SHORT (2)
#define ADDR_MODE_LONG (3)

#define FRAME_TYPE_BEACON (0)
#define FRAME_TYPE_DATA (1)
#define FRAME_TYPE_ACK (2)
#define FRAME_TYPE_MAC_CMD (3)

extern uint8_t mSelfMac[8];
uint8_t mLastLqi = 0;
int8_t mLastRSSI = 0;

uint8_t commsGetLastPacketLQI(void)
{
    return mLastLqi;
}

int8_t commsGetLastPacketRSSI(void)
{
    return mLastRSSI;
}

int8_t commsRxUnencrypted(uint8_t *data)
{
    memset(data, 0, 128);
    int8_t rxedLen = radioRxDequeuePkt(data, 128, &mLastRSSI, &mLastLqi);
    if (rxedLen < 0)
        return COMMS_RX_ERR_NO_PACKETS;
    if (memcmp((void *)&data[5], mSelfMac, 8)) // Here we filter for our MAC Address... in software...
        return COMMS_RX_ERR_INVALID_PACKET;
    return rxedLen;
}

bool commsTxNoCpy(const void *packetp)
{
    return radioTxLL(packetp);
}
