#include <string.h>

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

    struct MacFcs *fcs = (struct MacFcs *)data;
    if ((fcs->frameType == 1) && (fcs->destAddrType == 2) && (fcs->srcAddrType == 3) && (fcs->panIdCompressed == 0))
    {
        // broadcast frame
        struct MacFrameBcast *rxframe = (struct MacFrameBcast *)data;
        if (rxframe->dstPan != PROTO_PAN_ID)
            return 0;
    }
    else if ((fcs->frameType == 1) && (fcs->destAddrType == 3) && (fcs->srcAddrType == 3) && (fcs->panIdCompressed == 1))
    {
        // normal frame
        struct MacFrameNormal *txHeader = (struct MacFrameNormal *)data;
        if (memcmp(txHeader->dst, mSelfMac, 8))
            return 0;
    }
    return rxedLen; // Remove appended RSSI and LQI, maybe its used later again
}

bool commsTxNoCpy(const void *packetp)
{
    return radioTxLL(packetp);
}
