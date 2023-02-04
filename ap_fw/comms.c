#define __packed
#include "comms.h"
#include <string.h>
#include "asmUtil.h"
#include "printf.h"
#include "proto.h"
#include "radio.h"

static uint8_t __xdata mCommsBuf[127];

uint8_t commsGetLastPacketLQI(void) {
    return mLastLqi;
}

int8_t commsGetLastPacketRSSI(void) {
    return mLastRSSI;
}

int8_t commsRxUnencrypted(void __xdata *data) {
    uint8_t __xdata *dstData = (uint8_t __xdata *)data;
    uint8_t __xdata *__xdata rxedBuf;
    int8_t ret = COMMS_RX_ERR_INVALID_PACKET;

    int8_t rxedLen = radioRxDequeuePktGet((void __xdata *__xdata) & rxedBuf, &mLastLqi, &mLastRSSI);

    if (rxedLen < 0)
        return COMMS_RX_ERR_NO_PACKETS;

    xMemCopyShort(dstData, rxedBuf, rxedLen);
    radioRxDequeuedPktRelease();
    return rxedLen;
}

bool commsTxUnencrypted(const void __xdata *packetP, uint8_t len) {
    const uint8_t __xdata *packet = (const uint8_t __xdata *)packetP;

    if (len > COMMS_MAX_PACKET_SZ)
        return false;
    memset(mCommsBuf, 0, COMMS_MAX_PACKET_SZ);
    xMemCopyShort(mCommsBuf + 1, packet, len);

    mCommsBuf[0] = len + RADIO_PAD_LEN_BY;
    
    return radioTx(mCommsBuf);;
}

bool commsTxNoCpy(const void __xdata *packetp) {
   return radioTx(packetp);
}