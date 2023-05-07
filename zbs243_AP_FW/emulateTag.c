#define __packed
#include "board.h"

#if (HAS_SCREEN == 1 && AP_EMULATE_TAG == 1)
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "asmUtil.h"
#include "comms.h"
#include "cpu.h"
#include "printf.h"
#include "proto.h"
#include "radio.h"
#include "screen.h"
#include "timer.h"
#include "uart.h"
#include "wdt.h"

uint8_t __xdata fakeTagMac[8] = {0x0, 0x00, 0x55, 0xCE, 0xAC, 0x00, 0x00};

extern uint8_t __xdata radiorxbuffer[];
extern uint8_t __xdata blockbuffer[];

extern void espNotifyAvailDataReq(const struct AvailDataReq *adr, const uint8_t *src);
extern void espBlockRequest(const struct blockRequest *br, uint8_t *src);
extern void espNotifyXferComplete(const uint8_t *src);

extern void addCRC(void *p, uint8_t len);
bool __xdata fakeTagTrafficPending = false;

static struct AvailDataInfo __xdata ad;
static uint8_t blockCount = 0;
static uint32_t __xdata lastRequest = 0;
static bool dataRequested = false;

static bool validateBlockData() {
    struct blockData *bd = (struct blockData *)blockbuffer;
    // pr("expected len = %04X, checksum=%04X\n", bd->size, bd->checksum);
    uint16_t t = 0;
    for (uint16_t c = 0; c < bd->size; c++) {
        t += bd->data[c];
    }
    return bd->checksum == t;
}

uint16_t epdByteCounter = 0;
void epdWriteByte(uint8_t b) {
    epdSend(b);
    epdByteCounter++;
    // check if we need to switch to a
    if (epdByteCounter == (SCREEN_HEIGHT * (SCREEN_WIDTH / 8))) {
        epdDeselect();
        endWriteFramebuffer();
        beginWriteFramebuffer(EPD_COLOR_RED);
        epdSelect();
    }
}

void fakeTagGetData() {
    if ((timerGet() - lastRequest) > (1200UL * 1333)) {
        if (dataRequested == false) {
            if (ad.dataSize) {
                struct blockRequest br;
                br.blockId = blockCount;
                br.ver = ad.dataVer;
                espBlockRequest(&br, fakeTagMac);
                dataRequested = true;
            }
        } else {
            if (validateBlockData()) {
                struct blockData *bd = (struct blockData *)blockbuffer;
                if (blockCount == 0) {
                    epdByteCounter = 0;
                    clearScreen();
                    selectLUT(ad.dataTypeArgument % 3);
                    beginFullscreenImage();
                    beginWriteFramebuffer(EPD_COLOR_BLACK);
                    epdSelect();
                }
                for (uint16_t c = 0; c < bd->size; c++) {
                    epdWriteByte(bd->data[c]);
                }
                ad.dataSize -= bd->size;
                blockCount++;
            } else {
                // request data again!
            }
            if (ad.dataSize) {
            } else {
                epdDeselect();
                endWriteFramebuffer();
                espNotifyXferComplete(fakeTagMac);
                fakeTagTrafficPending = false;
                drawNoWait();
            }
            dataRequested = false;
        }
    }
}

void fakePendingData(struct pendingData *pd) {
    if ((pd->availdatainfo.dataType == DATATYPE_IMG_RAW_1BPP) || (pd->availdatainfo.dataType == DATATYPE_IMG_RAW_2BPP) || (pd->availdatainfo.dataType == DATATYPE_IMG_RAW_1BPP_DIRECT)) {
        memcpy(&ad, &(pd->availdatainfo), sizeof(struct AvailDataInfo));
        blockCount = 0;
        fakeTagTrafficPending = true;
        dataRequested = false;
    }
}

void fakeTagCheckIn() {
    static bool __xdata firstboot = true;
    struct AvailDataReq *adr = (struct AvailDataReq *)radiorxbuffer;

    memset(adr, 0, sizeof(struct AvailDataReq));
    adr->batteryMv = 1337;
    adr->hwType = HW_TYPE;
    adr->lastPacketLQI = 100;
    adr->lastPacketRSSI = 100;
    if (firstboot) {
        adr->wakeupReason = 0xFC;
        firstboot = false;
    } else {
        adr->wakeupReason = 0;
    }
    addCRC(adr, sizeof(struct AvailDataReq));
    espNotifyAvailDataReq(adr, fakeTagMac);
}

#endif