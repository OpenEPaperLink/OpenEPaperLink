#include "comms.h"

#include <stdint.h>
//#include <stdio.h>
#include <string.h>

#include "ccm.h"
#include "proto.h"

extern uint8_t Zigbee_tx_buffer(uint8_t tx_buffer[], int len);

uint8_t mLastLqi = 0;
int8_t mLastRSSI = 0;

uint8_t commsGetLastPacketLQI(void) {
    return mLastLqi;
}

int8_t commsGetLastPacketRSSI(void) {
    return mLastRSSI;
}

static inline void __attribute__((always_inline)) macCopy(uint8_t *restrict dst, const uint8_t *restrict src) {
    ((uint32_t *)dst)[0] = ((const uint32_t *)src)[0];
    ((uint32_t *)dst)[1] = ((const uint32_t *)src)[1];
}

static inline bool __attribute__((always_inline)) macIsEq(const uint8_t *restrict dst, const uint8_t *restrict src) {
    return ((uint32_t *)dst)[0] == ((const uint32_t *)src)[0] && ((uint32_t *)dst)[1] == ((const uint32_t *)src)[1];
}

extern volatile uint8_t rx_buffer[0x400];
extern volatile uint8_t new_rx;
extern volatile uint8_t new_rssi;
extern volatile int rx_len;

int32_t __attribute__((noinline)) commsRxUnenc(void *data) {
    if (!new_rx)
        return COMMS_RX_ERR_NO_PACKETS;
    memcpy(data, (uint8_t*)&rx_buffer, rx_len);
    mLastLqi = 255 - new_rssi;
    mLastRSSI = new_rssi;
    new_rx = 0;
    return rx_len;
}

void commsTxNoCpy(uint8_t *packetp) {
    Zigbee_tx_buffer((uint8_t *)&packetp[1], (packetp[0] - 2));
}
