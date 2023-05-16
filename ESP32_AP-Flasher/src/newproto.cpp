#include "newproto.h"

#include <Arduino.h>
#include <FS.h>
#include <HTTPClient.h>
#include <LittleFS.h>
#include <MD5Builder.h>
#include <makeimage.h>
#include <time.h>

#include "LittleFS.h"
#include "commstructs.h"
#include "serialap.h"
#include "settings.h"
#include "tag_db.h"
#include "udp.h"
#include "web.h"

extern uint16_t sendBlock(const void* data, const uint16_t len);
extern UDPcomm udpsync;

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

void prepareCancelPending(uint8_t dst[8]) {
    struct pendingData pending = {0};
    memcpy(pending.targetMac, dst, 8);
    sendCancelPending(&pending);

    uint8_t src[8];
    *((uint64_t*)src) = swap64(*((uint64_t*)dst));
    uint8_t mac[6];
    memcpy(mac, src + 2, sizeof(mac));
    tagRecord* taginfo = nullptr;
    taginfo = tagRecord::findByMAC(mac);
    if (taginfo == nullptr) {
        wsErr("Tag not found, this shouldn't happen.");
        return;
    }
    clearPending(taginfo);

    wsSendTaginfo(mac);
}

void prepareIdleReq(uint8_t* dst, uint16_t nextCheckin) {
    if (nextCheckin > MIN_RESPONSE_TIME) nextCheckin = MIN_RESPONSE_TIME;
    if (nextCheckin > 0) {
        struct pendingData pending = {0};
        memcpy(pending.targetMac, dst, 8);
        pending.availdatainfo.dataType = DATATYPE_NOUPDATE;
        pending.availdatainfo.nextCheckIn = nextCheckin;
        pending.attemptsLeft = 10 + MIN_RESPONSE_TIME;

        char buffer[64];
        uint8_t src[8];
        *((uint64_t*)src) = swap64(*((uint64_t*)dst));
        sprintf(buffer, ">SDA %02X%02X%02X%02X%02X%02X NOP\n\0", src[2], src[3], src[4], src[5], src[6], src[7]);
        Serial.print(buffer);
        sendDataAvail(&pending);
    }
}

bool prepareDataAvail(String* filename, uint8_t dataType, uint8_t* dst, uint16_t nextCheckin) {
    if (nextCheckin > MIN_RESPONSE_TIME) nextCheckin = MIN_RESPONSE_TIME;
    if (wsClientCount()) nextCheckin=0;
    
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

        wsLog("new image: " + String(dst_path));
        time_t now;
        time(&now);
        taginfo->expectedNextCheckin = now + nextCheckin * 60 + 60;
        taginfo->filename = *filename;
        taginfo->len = filesize;
        clearPending(taginfo);
        taginfo->pending = true;
        memcpy(taginfo->md5pending, md5bytes, sizeof(md5bytes));

    } else {
        wsLog("firmware upload pending");
        taginfo->filename = *filename;
        taginfo->len = filesize;
        clearPending(taginfo);
        taginfo->pending = true;
    }

    struct pendingData pending = {0};
    memcpy(pending.targetMac, dst, 8);
    pending.availdatainfo.dataType = dataType;
    pending.availdatainfo.dataVer = *((uint64_t*)md5bytes);
    pending.availdatainfo.dataSize = filesize;
    pending.availdatainfo.dataTypeArgument = lut;
    pending.availdatainfo.nextCheckIn = nextCheckin;
    pending.attemptsLeft = attempts;
    if (taginfo->isExternal == false) {
        char buffer[64];
        uint8_t src[8];
        *((uint64_t*)src) = swap64(*((uint64_t*)dst));
        sprintf(buffer, ">SDA %02X%02X%02X%02X%02X%02X TYPE 0x%02X\n\0", src[2], src[3], src[4], src[5], src[6], src[7], pending.availdatainfo.dataType);
        Serial.print(buffer);
        sendDataAvail(&pending);
    } else {
        udpsync.netSendDataAvail(&pending);
    }

    wsSendTaginfo(mac);

    return true;
}

void prepareExternalDataAvail(struct pendingData* pending, IPAddress remoteIP) {
    uint8_t src[8];
    *((uint64_t*)src) = swap64(*((uint64_t*)pending->targetMac));
    uint8_t mac[6];
    memcpy(mac, src + 2, sizeof(mac));
    tagRecord* taginfo = nullptr;
    taginfo = tagRecord::findByMAC(mac);
    if (taginfo == nullptr) {
        return;
    }
    if (taginfo->isExternal == false) {
        LittleFS.begin();

        char buffer[64];
        sprintf(buffer, "%02X%02X%02X%02X%02X%02X\0", src[2], src[3], src[4], src[5], src[6], src[7]);
        String filename = "/current/" + (String)buffer + ".pending";
        String imageUrl = "http://" + remoteIP.toString() + filename;
        wsLog("GET " + imageUrl);
        HTTPClient http;
        http.begin(imageUrl);
        int httpCode = http.GET();
        if (httpCode == 200) {
            File file = LittleFS.open(filename, "w");
            http.writeToStream(&file);
            file.close();
        }
        http.end();

        fs::File file = LittleFS.open(filename);
        uint32_t filesize = file.size();
        if (filesize == 0) {
            file.close();
            wsErr("File has size 0. " + filename);
            return;
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
        sendDataAvail(pending);

        taginfo->filename = filename;
        taginfo->len = filesize;
        clearPending(taginfo);
        taginfo->pending = true;
        memcpy(taginfo->md5pending, md5bytes, sizeof(md5bytes));
        taginfo->contentMode = 12;
        taginfo->nextupdate = 3216153600;

        wsSendTaginfo(mac);
    }
}

void processBlockRequest(struct espBlockRequest* br) {
    if (!checkCRC(br, sizeof(struct espBlockRequest))) {
        Serial.print("Failed CRC on a blockrequest received by the AP");
        return;
    }

    uint8_t src[8];
    *((uint64_t*)src) = swap64(*((uint64_t*)br->src));
    uint8_t mac[6];
    memcpy(mac, src + 2, sizeof(mac));
    tagRecord* taginfo = nullptr;
    taginfo = tagRecord::findByMAC(mac);
    if (taginfo == nullptr) {
        prepareCancelPending(br->src);
        Serial.printf("blockrequest: couldn't find taginfo %02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X\n", br->src[7], br->src[6], br->src[5], br->src[4], br->src[3], br->src[2], br->src[1], br->src[0]);
        return;
    }

    if (taginfo->data == nullptr) {
        // not cached. open file, cache the data
        fs::File file = LittleFS.open(taginfo->filename);
        if (!file) {
            Serial.print("Dunno how this happened... File pending but deleted in the meantime?\n");
            prepareCancelPending(br->src);
            return;
        }
        taginfo->data = getDataForFile(&file);
        file.close();
    }

    // check if we're not exceeding max blocks (to prevent sendBlock from exceeding its boundary)
    uint8_t totalblocks = (taginfo->len / BLOCK_DATA_SIZE);
    if (taginfo->len % BLOCK_DATA_SIZE) totalblocks++;
    if (br->blockId >= totalblocks) {
        br->blockId = totalblocks - 1;
    }

    uint32_t len = taginfo->len - (BLOCK_DATA_SIZE * br->blockId);
    if (len > BLOCK_DATA_SIZE) len = BLOCK_DATA_SIZE;
    uint16_t checksum = sendBlock(taginfo->data + (br->blockId * BLOCK_DATA_SIZE), len);
    char buffer[150];
    sprintf(buffer, "< Block Request received for file %s block %d, len %d checksum %u\0", taginfo->filename.c_str(), br->blockId, len, checksum);
    wsLog((String)buffer);
    Serial.printf("<RQB file %s block %d, len %d checksum %u\n\0", taginfo->filename.c_str(), br->blockId, len, checksum);
}

void processXferComplete(struct espXferComplete* xfc, bool local) {
    char buffer[64];
    uint8_t src[8];
    *((uint64_t*)src) = swap64(*((uint64_t*)xfc->src));
    sprintf(buffer, "< %02X%02X%02X%02X%02X%02X reports xfer complete\n\0", src[2], src[3], src[4], src[5], src[6], src[7]);
    wsLog((String)buffer);

    if (local) {
        Serial.printf("<XFC %02X%02X%02X%02X%02X%02\n", src[2], src[3], src[4], src[5], src[6], src[7]);
    } else {
        Serial.printf("<REMOTE XFC %02X%02X%02X%02X%02X%02\n", src[2], src[3], src[4], src[5], src[6], src[7]);
    }
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
    }

    time_t now;
    time(&now);
    tagRecord* taginfo = nullptr;
    taginfo = tagRecord::findByMAC(mac);
    if (taginfo != nullptr) {
        memcpy(taginfo->md5, taginfo->md5pending, sizeof(taginfo->md5pending));
        clearPending(taginfo);
        taginfo->wakeupReason = 0;
    }
    wsSendTaginfo(mac);
    if (local) udpsync.netProcessXferComplete(xfc);
}

void processXferTimeout(struct espXferComplete* xfc, bool local) {
    char buffer[64];
    uint8_t src[8];
    *((uint64_t*)src) = swap64(*((uint64_t*)xfc->src));
    sprintf(buffer, "< %02X%02X%02X%02X%02X%02X xfer timeout\n\0", src[2], src[3], src[4], src[5], src[6], src[7]);
    wsErr((String)buffer);

    if (local) {
        Serial.printf("<XTO %02X%02X%02X%02X%02X%02\n", src[2], src[3], src[4], src[5], src[6], src[7]);
    } else {
        Serial.printf("<REMOTE XTO %02X%02X%02X%02X%02X%02\n", src[2], src[3], src[4], src[5], src[6], src[7]);
    }
    uint8_t mac[6];
    memcpy(mac, src + 2, sizeof(mac));

    time_t now;
    time(&now);
    tagRecord* taginfo = nullptr;
    taginfo = tagRecord::findByMAC(mac);
    if (taginfo != nullptr) {
        taginfo->expectedNextCheckin = now + 60;
        memset(taginfo->md5pending, 0, 16 * sizeof(uint8_t));
        clearPending(taginfo);
    }
    wsSendTaginfo(mac);
    if (local) udpsync.netProcessXferTimeout(xfc);
}

void processDataReq(struct espAvailDataReq* eadr, bool local) {
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

    if (!local) {
        taginfo->isExternal = true;
    } else {
        taginfo->isExternal = false;
    }

    if (taginfo->pendingIdle == 0) {
        taginfo->expectedNextCheckin = now + 60;
    } else {
        taginfo->expectedNextCheckin = now + 60 * taginfo->pendingIdle;
        taginfo->pendingIdle = 0;
    }
    taginfo->lastseen = now;

    if (eadr->adr.lastPacketRSSI != 0) {
        taginfo->LQI = eadr->adr.lastPacketLQI;
        taginfo->hwType = eadr->adr.hwType;
        taginfo->RSSI = eadr->adr.lastPacketRSSI;
        taginfo->temperature = eadr->adr.temperature;
        taginfo->batteryMv = eadr->adr.batteryMv;
        taginfo->hwType = eadr->adr.hwType;
        taginfo->wakeupReason = eadr->adr.wakeupReason;
        taginfo->capabilities = eadr->adr.capabilities;
        if (eadr->adr.wakeupReason >= 0xF0) {
            if (!taginfo->pending) taginfo->nextupdate = 0;
            memset(taginfo->md5, 0, 16 * sizeof(uint8_t));
            memset(taginfo->md5pending, 0, 16 * sizeof(uint8_t));
        }
    }
    if (local) {
        sprintf(buffer, "<ADR %02X%02X%02X%02X%02X%02X\n\0", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    } else {
        sprintf(buffer, "<REMOTE ADR %02X%02X%02X%02X%02X%02X\n\0", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    }

    Serial.print(buffer);
    wsSendTaginfo(mac);
    if (local) udpsync.netProcessDataReq(eadr);
}

void refreshAllPending() {
    for (int16_t c = 0; c < tagDB.size(); c++) {
        tagRecord* taginfo = nullptr;
        taginfo = tagDB.at(c);
        if (taginfo->pending) {
            clearPending(taginfo);
            taginfo->nextupdate = 0;
            memset(taginfo->md5, 0, 16 * sizeof(uint8_t));
            memset(taginfo->md5pending, 0, 16 * sizeof(uint8_t));
            wsSendTaginfo(taginfo->mac);
        }
    }
};

void setAPchannel() {
    if (APconfig["channel"].as<int>() == 0) {
        // trigger channel autoselect
        UDPcomm udpsync;
        udpsync.getAPList();
    } else {
        if (curChannel.channel != APconfig["channel"].as<int>()) {
            curChannel.channel = APconfig["channel"].as<int>();
            sendChannelPower(&curChannel);
        }
    }
}

bool sendAPSegmentedData(uint8_t* dst, String data, uint16_t icons, bool inverted) {
    struct pendingData pending = {0};
    memcpy(pending.targetMac, dst, 8);
    pending.availdatainfo.dataType = 0x51;
    pending.availdatainfo.dataSize = icons << 16;
    memcpy((void*)&(pending.availdatainfo.dataVer), data.c_str(), 10);
    pending.availdatainfo.dataTypeArgument = inverted;
    pending.availdatainfo.nextCheckIn = 0;
    pending.attemptsLeft = 120;
    char buffer[64];
    uint8_t srcc[8];
    *((uint64_t*)srcc) = swap64(*((uint64_t*)dst));
    sprintf(buffer, ">AP Segmented Data %02X%02X%02X%02X%02X%02X%02X%02X\n\0", srcc[0], srcc[1], srcc[2], srcc[3], srcc[4], srcc[5], srcc[6], srcc[7]);
    Serial.print(buffer);
    return sendDataAvail(&pending);
}

bool showAPSegmentedInfo(uint8_t* dst) {
    struct pendingData pending = {0};
    memcpy(pending.targetMac, dst, 8);
    pending.availdatainfo.dataType = 0x51;
    pending.availdatainfo.dataSize = 0x00;
    pending.availdatainfo.dataVer = 0x00;
    pending.availdatainfo.dataTypeArgument = 0;
    pending.availdatainfo.nextCheckIn = 0;
    pending.attemptsLeft = 120;
    char buffer[64];
    uint8_t srcc[8];
    *((uint64_t*)srcc) = swap64(*((uint64_t*)dst));
    sprintf(buffer, ">SDA %02X%02X%02X%02X%02X%02X%02X%02X\n\0", srcc[0], srcc[1], srcc[2], srcc[3], srcc[4], srcc[5], srcc[6], srcc[7]);
    Serial.print(buffer);
    return sendDataAvail(&pending);
}