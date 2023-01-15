#pragma pack(push, 1)
#include "newproto.h"

#include <Arduino.h>
#include <MD5Builder.h>

#include "LittleFS.h"
#include "commstructs.h"
#include "pendingdata.h"
#include "serial.h"
#include "settings.h"
#include "web.h"

extern void sendBlock(const void* data, const uint16_t len);

void addCRC(void* p, uint8_t len) {
    uint8_t total = 0;
    for (uint8_t c = 1; c < len; c++) {
        total += ((uint8_t*)p)[c];
    }
    ((uint8_t*)p)[0] = total;
    // pr("%d",total);
}
bool checkCRC(void* p, uint8_t len) {
    uint8_t total = 0;
    for (uint8_t c = 1; c < len; c++) {
        total += ((uint8_t*)p)[c];
    }
    return ((uint8_t*)p)[0] == total;
}

uint8_t* getDataForFile(File* file) {
    uint8_t* ret = nullptr;
    ret = (uint8_t*)malloc(file->size());
    if (ret) {
        file->seek(0);
        file->readBytes((char*)ret, file->size());
    } else {
        Serial.print("malloc failed for file...\n");
    }
    return ret;
}

bool prepareDataAvail(String* filename, uint8_t dataType, uint8_t* dst) {
    *filename = "/" + *filename;
    if (!LittleFS.exists(*filename)) return false;
    File file = LittleFS.open(*filename);

    if (file.size() == 0) {
        Serial.print("opened a file with size 0??\n");
        return false;
    }

    uint8_t md5bytes[16];
    {
        MD5Builder md5;
        md5.begin();
        md5.addStream(file, file.size());
        md5.calculate();
        md5.getBytes(md5bytes);
    }

    // the message that will be sent to the AP to tell the tag there is data pending
    struct pendingData pending = {0};
    memcpy(pending.targetMac, dst, 8);
    pending.availdatainfo.dataType = dataType;
    pending.availdatainfo.dataVer = *((uint64_t*)md5bytes);
    pending.availdatainfo.dataSize = file.size();
    pending.attemptsLeft = 10;
    sendDataAvail(&pending);

    // data for the cache on the esp32; needs to hold the data longer than the maximum timeout on the AP
    pendingdata* pendinginfo = nullptr;
    pendinginfo = new pendingdata;
    pendinginfo->filename = *filename;
    pendinginfo->ver = pending.availdatainfo.dataVer;
    pendinginfo->len = pending.availdatainfo.dataSize;
    pendinginfo->data = nullptr;
    pendinginfo->timeout = PENDING_TIMEOUT;
    // pendinginfo->data = getDataForFile(&file);
    file.close();
    pendinginfo->timeout = 1800;
    pendingfiles.push_back(pendinginfo);
    return true;
}

void processBlockRequest(struct espBlockRequest* br) {
    if (!checkCRC(br, sizeof(struct espBlockRequest))) {
        Serial.print("Failed CRC on a blockrequest received by the AP");
        return;
    }

    pendingdata* pd = pendingdata::findByVer(br->ver);
    if (pd == nullptr) {
        Serial.printf("Couldn't find pendingdata info for ver %llu", br->ver);
        return;
    } else {
        if (pd->data == nullptr) {
            // not cached. open file, cache the data
            File file = LittleFS.open(pd->filename);
            if (!file) {
                Serial.print("Dunno how this happened... File pending but deleted in the meantime?\n");
            }
            pd->data = getDataForFile(&file);
            pd->datatimeout = PENDING_DATA_TIMEOUT;
            file.close();
        } else {
            // file is already cached, refresh the timeout
            pd->datatimeout = PENDING_DATA_TIMEOUT;
        }
    }
    // check if we're not exceeding max blocks (to prevent sendBlock from exceeding its boundary)
    uint8_t totalblocks = (pd->len / BLOCK_DATA_SIZE);
    if (pd->len % BLOCK_DATA_SIZE) totalblocks++;
    if (br->blockId >= totalblocks) {
        br->blockId = totalblocks - 1;
    }

    uint32_t len = pd->len - (BLOCK_DATA_SIZE * br->blockId);
    if (len > BLOCK_DATA_SIZE) len = BLOCK_DATA_SIZE;
    sendBlock(pd->data + (br->blockId * BLOCK_DATA_SIZE), len);
    char buffer[64];
    sprintf(buffer, "< Block Request received for MD5 %llu, block %d\n\0", br->ver, br->blockId);
    wsString((String)buffer);
    Serial.printf("<BlockId=%d\n", br->blockId);
}

void processXferComplete(struct espXferComplete* xfc) {
    char buffer[64];
    uint8_t src[8];
    *((uint64_t*)src) = swap64(*((uint64_t*)xfc->src));
    sprintf(buffer, "< %02X%02X%02X%02X%02X%02X reports xfer complete\n\0", src[2], src[3], src[4], src[5], src[6], src[7]);
    wsString((String)buffer);
    Serial.print(buffer);
}

void processDataReq(struct espAvailDataReq* eadr) {
    char buffer[64];
    uint8_t src[8];
    //    *((uint64_t*)src) = swap64(*((uint64_t*)adr->));
    sprintf(buffer, "<ADR %02X%02X%02X%02X%02X%02X%02X%02X\n\0", eadr->src[7], eadr->src[6], eadr->src[5], eadr->src[4], eadr->src[3], eadr->src[2], eadr->src[1], eadr->src[0]);
    //sprintf(buffer, "<DATA REQ - version=%d, voltage=%d\n", adr->softVer, adr->batteryMv);
    wsString((String)buffer);
    Serial.print(buffer);
}
