#pragma pack(push, 1)
#include "newproto.h"

#include <Arduino.h>
#include <MD5Builder.h>
#include <makeimage.h>

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

uint8_t* getDataForFile(fs::File* file) {
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

void prepareCancelPending(uint64_t ver) {
    struct pendingData pending = {0};
    pending.availdatainfo.dataVer = ver;
    sendCancelPending(&pending);
}

bool prepareDataAvail(String* filename, uint8_t dataType, uint8_t* dst, uint16_t nextCheckin) {
    *filename = "/" + *filename;
    if (!LittleFS.exists(*filename)) return false;
    fs::File file = LittleFS.open(*filename);

    if (file.size() == 0) {
        Serial.print("opened a file with size 0??\n");
        return false;
    }

    if (filename->endsWith(".bmp") || filename->endsWith(".BMP")) {
        struct BitmapFileHeader hdr;
        file.read((uint8_t*)&hdr, sizeof(hdr));
        if (hdr.width == 296 && hdr.height == 128) {
            //sorry, can't rotate
            Serial.println("when using BMP files, remember to only use 128px width and 296px height");
            wsString("when using BMP files, remember to only use 128px width and 296px height");
            return false;
        }
        if (hdr.sig[0] == 'B' && hdr.sig[1] == 'M' && hdr.bpp == 24) {
            Serial.println("converting 24bpp bmp to grays");
            wsString("converting 24bpp bmp to grays");
            char fileout[64];
            sprintf(fileout, "/temp/%02X%02X%02X%02X%02X%02X.bmp\0", dst[5], dst[4], dst[3], dst[2], dst[1], dst[0]);
            bmp2grays(*filename,(String)fileout);
            *filename = (String)fileout;
            file.close();
            file = LittleFS.open(*filename);
        }
    }

    if (filename->endsWith(".jpg") || filename->endsWith(".JPG")) {
        Serial.println("converting jpg to grays");
        wsString("converting jpg to grays");
        char fileout[64];
        sprintf(fileout, "/temp/%02X%02X%02X%02X%02X%02X.bmp\0", dst[5], dst[4], dst[3], dst[2], dst[1], dst[0]);
        jpg2grays(*filename, (String)fileout);
        *filename = (String)fileout;
        file.close();
        file = LittleFS.open(*filename);
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
    pending.availdatainfo.nextCheckIn = nextCheckin;
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
    pendinginfo->data = getDataForFile(&file);
    file.close();
    pendinginfo->timeout = 1800;
    pendingfiles.push_back(pendinginfo);

    char dst_path[64];
    sprintf(dst_path, "/current/%02X%02X%02X%02X%02X%02X.pending\0", dst[5], dst[4], dst[3], dst[2], dst[1], dst[0]);
    file = LittleFS.open(dst_path, "w");
    int bytes_written = file.write(pendinginfo->data, pendinginfo->len);
    file.close();

    return true;
}

void processBlockRequest(struct espBlockRequest* br) {
    if (!checkCRC(br, sizeof(struct espBlockRequest))) {
        Serial.print("Failed CRC on a blockrequest received by the AP");
        return;
    }

    pendingdata* pd = pendingdata::findByVer(br->ver);
    if (pd == nullptr) {
        prepareCancelPending(br->ver);
        Serial.printf("Couldn't find pendingdata info for ver %llu", br->ver);
        return;
    } else {
        if (pd->data == nullptr) {
            // not cached. open file, cache the data
            fs::File file = LittleFS.open(pd->filename);
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
    wsSendTaginfo(src);

    char src_path[64];
    char dst_path[64];
    char tmp_path[64];
    sprintf(src_path, "/current/%02X%02X%02X%02X%02X%02X.pending\0", src[2], src[3], src[4], src[5], src[6], src[7]);
    sprintf(dst_path, "/current/%02X%02X%02X%02X%02X%02X.bmp\0", src[2], src[3], src[4], src[5], src[6], src[7]);
    sprintf(tmp_path, "/temp/%02X%02X%02X%02X%02X%02X.bmp\0", src[2], src[3], src[4], src[5], src[6], src[7]);
    LittleFS.rename(src_path, dst_path);
    if (LittleFS.exists(tmp_path)) {
        LittleFS.remove(tmp_path);
    }
}

void processDataReq(struct espAvailDataReq* eadr) {
    char buffer[64];
    uint8_t src[8];
    *((uint64_t*)src) = swap64(*((uint64_t*)eadr->src));
    sprintf(buffer, "<ADR %02X%02X%02X%02X%02X%02X button: %02X\n\0", src[2], src[3], src[4], src[5], src[6], src[7], eadr->adr.buttonState);
    wsString((String)buffer);
    Serial.print(buffer);
    wsSendTaginfo(src);
}
