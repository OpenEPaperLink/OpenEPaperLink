#include <Arduino.h>
#include "comms.h"

#include <string.h>

#include "../../../oepl-proto.h"
#include "../../../oepl-definitions.h"

#include "hal.h"


extern uint8_t mSelfMac[8];
uint8_t mLastLqi = 0;
int8_t mLastRSSI = 0;

uint8_t commsGetLastPacketLQI(void) {
    return mLastLqi;
}

int8_t commsGetLastPacketRSSI(void) {
    return mLastRSSI;
}

int8_t commsRxUnencrypted(uint8_t *data) {
    memset(data, 0, 128);
    int8_t rxedLen = radioRxDequeuePkt(data, 128, &mLastRSSI, &mLastLqi);
    if (rxedLen < 0)
        return COMMS_RX_ERR_NO_PACKETS;
    if (memcmp((void *)&data[5], mSelfMac, 8))  // Here we filter for our MAC Address... in software...
        return COMMS_RX_ERR_INVALID_PACKET;
    return rxedLen;
}

bool commsTxNoCpy(const void *packetp) {
    return radioTxLL(packetp);
}
