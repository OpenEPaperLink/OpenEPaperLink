#include "newproto.h"

#include <Arduino.h>
#include <MD5Builder.h>
#include <makeimage.h>
#include <time.h>

#include "LittleFS.h"
#include "commstructs.h"
#include "pendingdata.h"
#include "serial.h"
#include "settings.h"
#include "tag_db.h"
#include "web.h"

extern uint16_t sendBlock(const void* data, const uint16_t len);

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

void prepareIdleReq(uint8_t* dst, uint16_t nextCheckin) {
    if (nextCheckin > MIN_RESPONSE_TIME) {
        // to prevent very long sleeps of the tag
        nextCheckin = MIN_RESPONSE_TIME;
    }
    struct pendingData pending = {0};
    memcpy(pending.targetMac, dst, 8);
    pending.availdatainfo.dataType = DATATYPE_NOUPDATE;
    pending.availdatainfo.nextCheckIn = nextCheckin;
    pending.attemptsLeft = 10 + MIN_RESPONSE_TIME;

    char buffer[64];
    uint8_t src[8];
    *((uint64_t*)src) = swap64(*((uint64_t*)dst));
    Serial.print(buffer);

    sendDataAvail(&pending);
}

bool prepareDataAvail(String* filename, uint8_t dataType, uint8_t* dst, uint16_t nextCheckin) {
    if (nextCheckin > MIN_RESPONSE_TIME) {
        //to prevent very long sleeps of the tag
        nextCheckin = MIN_RESPONSE_TIME;
    }

    uint8_t src[8];
    *((uint64_t*)src) = swap64(*((uint64_t*)dst));
    uint8_t mac[6];
    memcpy(mac, src + 2, sizeof(mac));
    tagRecord* taginfo = nullptr;
    taginfo = tagRecord::findByMAC(mac);
    if (taginfo == nullptr) {
        wsErr("Tag not found, this shouldn't happen.");
        return true;
    }

    *filename = "/" + *filename;
    LittleFS.begin();

    if (!LittleFS.exists(*filename)) {
        wsErr("File not found. " + *filename);
        return false;
    }

    fs::File file = LittleFS.open(*filename);
    uint32_t filesize = file.size();
    if (filesize == 0) {
        file.close();
        wsErr("File has size 0. " + *filename);
        return false;
    }

    uint8_t md5bytes[16];
    {
        MD5Builder md5;
        md5.begin();
        md5.addStream(file, filesize);
        md5.calculate();
        md5.getBytes(md5bytes);
    }

    file.close();
    uint16_t attempts = 60 * 24;
    uint8_t lut = EPD_LUT_NO_REPEATS;

    if (memcmp(md5bytes, taginfo->md5pending, 16) == 0) {
        wsLog("new image is the same as current or already pending image. not updating tag.");
        wsSendTaginfo(mac);
        return true;
    }

    time_t now;
    time(&now);
    time_t last_midnight = now - now % (24 * 60 * 60) + 3 * 3600;  // somewhere in the middle of the night
    if (taginfo->lastfullupdate < last_midnight) {
        lut = EPD_LUT_DEFAULT;  // full update once a day
        taginfo->lastfullupdate = now;
    }

    if (dataType != DATATYPE_FW_UPDATE) {

        char dst_path[64];
        sprintf(dst_path, "/current/%02X%02X%02X%02X%02X%02X.pending\0", dst[5], dst[4], dst[3], dst[2], dst[1], dst[0]);
        if (LittleFS.exists(dst_path)) {
            LittleFS.remove(dst_path);
        }
        LittleFS.rename(*filename, dst_path);
        *filename = String(dst_path);

        wsLog("new image pending: " + String(dst_path));
        time_t now;
        time(&now);
        taginfo->pending = true;
        taginfo->expectedNextCheckin = now + nextCheckin * 60 + 60;
        memcpy(taginfo->md5pending, md5bytes, sizeof(md5bytes));

    } else {
        wsLog("firmware upload pending");
    }

    // the message that will be sent to the AP to tell the tag there is data pending
    struct pendingData pending = {0};
    memcpy(pending.targetMac, dst, 8);
    pending.availdatainfo.dataType = dataType;
    pending.availdatainfo.dataVer = *((uint64_t*)md5bytes);
    pending.availdatainfo.dataSize = filesize;
    pending.availdatainfo.dataTypeArgument = lut;
    pending.availdatainfo.nextCheckIn = nextCheckin;
    pending.attemptsLeft = attempts;
    sendDataAvail(&pending);

    // data for the cache on the esp32; needs to hold the data longer than the maximum timeout on the AP
    pendingdata* pendinginfo = nullptr;
    pendinginfo = new pendingdata;
    pendinginfo->filename = *filename;
    pendinginfo->ver = pending.availdatainfo.dataVer;
    pendinginfo->len = pending.availdatainfo.dataSize;
    pendinginfo->data = nullptr;
    pendinginfo->timeout = PENDING_TIMEOUT;
    pendingfiles.push_back(pendinginfo);

    wsSendTaginfo(mac);

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
                prepareCancelPending(br->ver);
                return;
            }
            pd->data = getDataForFile(&file);
            pd->datatimeout = PENDING_DATA_TIMEOUT;
            file.close();
        } else {
            // file is already cached, refresh the timeout
            //pd->datatimeout = PENDING_DATA_TIMEOUT;
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
    uint16_t checksum = sendBlock(pd->data + (br->blockId * BLOCK_DATA_SIZE), len);
    char buffer[150];
    sprintf(buffer, "< Block Request received for file %s block %d, len %d checksum %u\0", pd->filename.c_str(), br->blockId, len, checksum);
    wsLog((String)buffer);
    Serial.printf("< Block Request received for MD5 %llu, file %s block %d, len %d checksum %u\n", br->ver, pd->filename.c_str(), br->blockId, len, checksum);
}

void processXferComplete(struct espXferComplete* xfc) {
    char buffer[64];
    uint8_t src[8];
    *((uint64_t*)src) = swap64(*((uint64_t*)xfc->src));
    sprintf(buffer, "< %02X%02X%02X%02X%02X%02X reports xfer complete\n\0", src[2], src[3], src[4], src[5], src[6], src[7]);
    wsLog((String)buffer);
    Serial.print(buffer);
    uint8_t mac[6];
    memcpy(mac, src + 2, sizeof(mac));

    char src_path[64];
    char dst_path[64];
    sprintf(src_path, "/current/%02X%02X%02X%02X%02X%02X.pending\0", src[2], src[3], src[4], src[5], src[6], src[7]);
    sprintf(dst_path, "/current/%02X%02X%02X%02X%02X%02X.raw\0", src[2], src[3], src[4], src[5], src[6], src[7]);
    if (LittleFS.exists(dst_path) && LittleFS.exists(src_path)) {
        LittleFS.remove(dst_path);
    }
    if (LittleFS.exists(src_path)) {
        LittleFS.rename(src_path, dst_path);
    } else {
        wsErr("hm, weird, no pending image found after xfercomplete.");
    }

    time_t now;
    time(&now);
    tagRecord* taginfo = nullptr;
    taginfo = tagRecord::findByMAC(mac);
    if (taginfo != nullptr) {

        uint16_t minutesUntilNextUpdate = 0;
        if (taginfo->nextupdate > now + 2 * 60) {
            minutesUntilNextUpdate = (taginfo->nextupdate - now) / 60;
            if (minutesUntilNextUpdate > MIN_RESPONSE_TIME) minutesUntilNextUpdate = MIN_RESPONSE_TIME;
            taginfo->expectedNextCheckin = now + 60 * minutesUntilNextUpdate + 60;
            if (minutesUntilNextUpdate > 1) prepareIdleReq (xfc->src, minutesUntilNextUpdate);
        } else {
            taginfo->expectedNextCheckin = now + 60;
        }

        taginfo->pending = false;
        memcpy(taginfo->md5, taginfo->md5pending, sizeof(taginfo->md5pending));
    }
    wsSendTaginfo(mac);
}

void processXferTimeout(struct espXferComplete* xfc) {
    char buffer[64];
    uint8_t src[8];
    *((uint64_t*)src) = swap64(*((uint64_t*)xfc->src));
    sprintf(buffer, "< %02X%02X%02X%02X%02X%02X xfer timeout\n\0", src[2], src[3], src[4], src[5], src[6], src[7]);
    wsErr((String)buffer);
    Serial.print(buffer);
    uint8_t mac[6];
    memcpy(mac, src + 2, sizeof(mac));

    time_t now;
    time(&now);
    tagRecord* taginfo = nullptr;
    taginfo = tagRecord::findByMAC(mac);
    if (taginfo != nullptr) {
        taginfo->expectedNextCheckin = now + 60;
        taginfo->pending = false;
        memset(taginfo->md5pending, 0, 16 * sizeof(uint8_t));
    }
    wsSendTaginfo(mac);
}

void processDataReq(struct espAvailDataReq* eadr) {

    digitalWrite(ONBOARD_LED, LOW);

    char buffer[64];
    uint8_t src[8];
    *((uint64_t*)src) = swap64(*((uint64_t*)eadr->src));

    tagRecord* taginfo = nullptr;
    uint8_t mac[6];
    memcpy(mac, src + 2, sizeof(mac));

    taginfo = tagRecord::findByMAC(mac);
    if (taginfo == nullptr) {
        taginfo = new tagRecord;
        memcpy(taginfo->mac, src + 2, sizeof(taginfo->mac));
        taginfo->pending = false;
        tagDB.push_back(taginfo);
    }
    time_t now;
    time(&now);
    taginfo->lastseen = now;

    uint16_t minutesUntilNextUpdate = 0;
    if (taginfo->nextupdate > now + 2 * 60) {
        minutesUntilNextUpdate = (taginfo->nextupdate - now) / 60;
        if (minutesUntilNextUpdate > MIN_RESPONSE_TIME) minutesUntilNextUpdate = MIN_RESPONSE_TIME;
        taginfo->expectedNextCheckin = now + 60 * minutesUntilNextUpdate + 60;
        if (minutesUntilNextUpdate > 1 && taginfo->pending == false) prepareIdleReq(eadr->src, minutesUntilNextUpdate);
    } else {
        taginfo->expectedNextCheckin = now + 60;
    }

    if (eadr->adr.lastPacketRSSI != 0) {
        taginfo->LQI = eadr->adr.lastPacketLQI;
        taginfo->hwType = eadr->adr.hwType;
        taginfo->RSSI = eadr->adr.lastPacketRSSI;
        taginfo->temperature = eadr->adr.temperature;
        taginfo->batteryMv = eadr->adr.batteryMv;
        taginfo->hwType = eadr->adr.hwType;
        taginfo->wakeupReason = eadr->adr.wakeupReason;
        taginfo->capabilities = eadr->adr.capabilities;
        if (eadr->adr.wakeupReason == WAKEUP_REASON_FIRSTBOOT && !taginfo->pending) {
            taginfo->nextupdate = 0;
            memset(taginfo->md5, 0, 16 * sizeof(uint8_t));
            memset(taginfo->md5pending, 0, 16 * sizeof(uint8_t));
        }
    }

    sprintf(buffer, "<ADR %02X%02X%02X%02X%02X%02X\n\0", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    Serial.print(buffer);
    wsSendTaginfo(mac);

    digitalWrite(ONBOARD_LED, HIGH);
}

void refreshAllPending() {
    for (int16_t c = 0; c < tagDB.size(); c++) {
        tagRecord* taginfo = nullptr;
        taginfo = tagDB.at(c);
        if (taginfo->pending) {
            taginfo->pending = false;
            taginfo->nextupdate = 0;
            memset(taginfo->md5, 0, 16 * sizeof(uint8_t));
            memset(taginfo->md5pending, 0, 16 * sizeof(uint8_t));
            wsSendTaginfo(taginfo->mac);
        }
    }
};