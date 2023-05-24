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

    tagRecord* taginfo = nullptr;
    taginfo = tagRecord::findByMAC(dst);
    if (taginfo == nullptr) {
        wsErr("Tag not found, this shouldn't happen.");
        return;
    }
    clearPending(taginfo);

    wsSendTaginfo(dst, SYNC_TAGSTATUS);
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
        sprintf(buffer, ">SDA %02X%02X%02X%02X%02X%02X%02X%02X NOP\n", dst[7], dst[6], dst[5], dst[4], dst[3], dst[2], dst[1], dst[0]);
        Serial.print(buffer);
        sendDataAvail(&pending);
    }
}

void prepareNFCReq(uint8_t* dst, const char* url) {
    tagRecord* taginfo = nullptr;
    taginfo = tagRecord::findByMAC(dst);
    if (taginfo == nullptr) {
        wsErr("Tag not found, this shouldn't happen.");
        return;
    }

    clearPending(taginfo);
    size_t len = strlen(url);
    taginfo->data = new uint8_t[len + 8];

    // TLV
    taginfo->data[0] = 0x03;  // NDEF message (TLV type)
    taginfo->data[1] = 4 + len + 1;

    // ndef record
    taginfo->data[2] = 0xD1;
    taginfo->data[3] = 0x01;  // well known record type
    taginfo->data[4] = len + 1;  // payload length
    taginfo->data[5] = 0x55;     // payload type (URI record)
    taginfo->data[6] = 0x00;     // URI identifier code (no prepending)

    memcpy(taginfo->data + 7, reinterpret_cast<const uint8_t*>(url), len);
    len = 7 + len;
    taginfo->data[len] = 0xFE;
    len = 1 + len;

    taginfo->pending = true;
    taginfo->len = len;

    struct pendingData pending = {0};
    memcpy(pending.targetMac, dst, 8);
    pending.availdatainfo.dataSize = len;
    pending.availdatainfo.dataType = DATATYPE_NFC_RAW_CONTENT;
    pending.availdatainfo.nextCheckIn = 0;
    pending.availdatainfo.dataVer = millis();
    pending.attemptsLeft = 10;

    if (taginfo->isExternal) {
        udpsync.netSendDataAvail(&pending);
    } else {
        sendDataAvail(&pending);
    }
    wsSendTaginfo(dst, SYNC_TAGSTATUS);
}

bool prepareDataAvail(String* filename, uint8_t dataType, uint8_t* dst, uint16_t nextCheckin) {
    if (nextCheckin > MIN_RESPONSE_TIME) nextCheckin = MIN_RESPONSE_TIME;
    if (wsClientCount()) nextCheckin=0;
    
    tagRecord* taginfo = nullptr;
    taginfo = tagRecord::findByMAC(dst);
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
        wsSendTaginfo(dst, SYNC_TAGSTATUS);
        if (LittleFS.exists(*filename)) {
            LittleFS.remove(*filename);
        }
        return true;
    }

    time_t now;
    time(&now);
    time_t last_midnight = now - now % (24 * 60 * 60) + 3 * 3600;  // somewhere in the middle of the night
    if (taginfo->lastfullupdate < last_midnight || taginfo->hwType == SOLUM_29_UC8151) {
        lut = EPD_LUT_DEFAULT;  // full update once a day
        taginfo->lastfullupdate = now;
    }

    if (dataType != DATATYPE_FW_UPDATE) {
        char dst_path[64];
        sprintf(dst_path, "/current/%02X%02X%02X%02X%02X%02X%02X%02X.pending\0", dst[7], dst[6], dst[5], dst[4], dst[3], dst[2], dst[1], dst[0]);
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
        sprintf(buffer, ">SDA %02X%02X%02X%02X%02X%02X%02X%02X TYPE 0x%02X\n\0", dst[7], dst[6], dst[5], dst[4], dst[3], dst[2], dst[1], dst[0], pending.availdatainfo.dataType);
        Serial.print(buffer);
        sendDataAvail(&pending);
    } else {
        udpsync.netSendDataAvail(&pending);
    }

    wsSendTaginfo(dst, SYNC_TAGSTATUS);

    return true;
}

void prepareExternalDataAvail(struct pendingData* pending, IPAddress remoteIP) {
    tagRecord* taginfo = nullptr;
    taginfo = tagRecord::findByMAC(pending->targetMac);
    if (taginfo == nullptr) {
        return;
    }
    if (taginfo->isExternal == false) {
        switch (pending->availdatainfo.dataType) {
            case DATATYPE_IMG_DIFF:
            case DATATYPE_IMG_RAW_1BPP:
            case DATATYPE_IMG_RAW_2BPP:
            case DATATYPE_IMG_RAW_1BPP_DIRECT: {
                LittleFS.begin();

                char hexmac[17];
                mac2hex(pending->targetMac, hexmac);
                String filename = "/current/" + String(hexmac) + ".pending";
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
                    wsErr("Remote file not found. " + filename);
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
                taginfo->filename = filename;
                taginfo->len = filesize;
                clearPending(taginfo);
                taginfo->pending = true;
                memcpy(taginfo->md5pending, md5bytes, sizeof(md5bytes));
                break;
            }
            case DATATYPE_NFC_RAW_CONTENT:
            case DATATYPE_NFC_URL_DIRECT:
            case DATATYPE_CUSTOM_LUT_OTA: {
                char hexmac[17];
                mac2hex(pending->targetMac, hexmac);
                String dataUrl = "http://" + remoteIP.toString() + "/getdata?mac=" + String(hexmac);
                wsLog("GET " + dataUrl);
                HTTPClient http;
                http.begin(dataUrl);
                int httpCode = http.GET();
                if (httpCode == 200) {
                    size_t len = http.getSize();
                    if (len > 0) {
                        clearPending(taginfo);
                        taginfo->data = new uint8_t[len];
                        WiFiClient* stream = http.getStreamPtr();
                        stream->readBytes(taginfo->data, len);
                        taginfo->pending = true;
                        taginfo->len = len;
                    } 
                }
                http.end();
                break;
            }
            case DATATYPE_FW_UPDATE: {
                return;
            }
        }
        taginfo->contentMode = 12;
        taginfo->nextupdate = 3216153600;
        sendDataAvail(pending);

        wsSendTaginfo(pending->targetMac, SYNC_NOSYNC);
    }
}

void processBlockRequest(struct espBlockRequest* br) {
    if (!checkCRC(br, sizeof(struct espBlockRequest))) {
        Serial.print("Failed CRC on a blockrequest received by the AP");
        return;
    }

    tagRecord* taginfo = nullptr;
    taginfo = tagRecord::findByMAC(br->src);
    if (taginfo == nullptr) {
        prepareCancelPending(br->src);
        Serial.printf("blockrequest: couldn't find taginfo %02X%02X%02X%02X%02X%02X%02X%02X\n", br->src[7], br->src[6], br->src[5], br->src[4], br->src[3], br->src[2], br->src[1], br->src[0]);
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
    sprintf(buffer, "< %02X%02X%02X%02X%02X%02X%02X%02X reports xfer complete\n\0", xfc->src[7], xfc->src[6], xfc->src[5], xfc->src[4], xfc->src[3], xfc->src[2], xfc->src[1], xfc->src[0]);
    wsLog((String)buffer);

    if (local) {
        Serial.printf("<XFC %02X%02X%02X%02X%02X%02X%02X%02X\n", xfc->src[7], xfc->src[6], xfc->src[5], xfc->src[4], xfc->src[3], xfc->src[2], xfc->src[1], xfc->src[0]);
    } else {
        Serial.printf("<REMOTE XFC %02X%02X%02X%02X%02X%02X%02X%02X\n", xfc->src[7], xfc->src[6], xfc->src[5], xfc->src[4], xfc->src[3], xfc->src[2], xfc->src[1], xfc->src[0]);
    }

    char src_path[64];
    char dst_path[64];
    sprintf(src_path, "/current/%02X%02X%02X%02X%02X%02X%02X%02X.pending\0", xfc->src[7], xfc->src[6], xfc->src[5], xfc->src[4], xfc->src[3], xfc->src[2], xfc->src[1], xfc->src[0]);
    sprintf(dst_path, "/current/%02X%02X%02X%02X%02X%02X%02X%02X.raw\0", xfc->src[7], xfc->src[6], xfc->src[5], xfc->src[4], xfc->src[3], xfc->src[2], xfc->src[1], xfc->src[0]);
    if (LittleFS.exists(dst_path) && LittleFS.exists(src_path)) {
        LittleFS.remove(dst_path);
    }
    if (LittleFS.exists(src_path)) {
        LittleFS.rename(src_path, dst_path);
    }

    time_t now;
    time(&now);
    tagRecord* taginfo = nullptr;
    taginfo = tagRecord::findByMAC(xfc->src);
    if (taginfo != nullptr) {
        memcpy(taginfo->md5, taginfo->md5pending, sizeof(taginfo->md5pending));
        clearPending(taginfo);
        taginfo->wakeupReason = 0;
        if (taginfo->contentMode == 12 && local == false) {
            if (LittleFS.exists(dst_path)) {
                LittleFS.remove(dst_path);
            }
        }
    }
    wsSendTaginfo(xfc->src, SYNC_TAGSTATUS);
    if (local) udpsync.netProcessXferComplete(xfc);
}

void processXferTimeout(struct espXferComplete* xfc, bool local) {
    char buffer[64];
    sprintf(buffer, "< %02X%02X%02X%02X%02X%02X%02X%02X xfer timeout\n\0", xfc->src[7], xfc->src[6], xfc->src[5], xfc->src[4], xfc->src[3], xfc->src[2], xfc->src[1], xfc->src[0]);
    wsErr((String)buffer);

    if (local) {
        Serial.printf("<XTO %02X%02X%02X%02X%02X%02X%02X%02X\n", xfc->src[7], xfc->src[6], xfc->src[5], xfc->src[4], xfc->src[3], xfc->src[2], xfc->src[1], xfc->src[0]);
    } else {
        Serial.printf("<REMOTE XTO %02X%02X%02X%02X%02X%02X%02X%02X\n", xfc->src[7], xfc->src[6], xfc->src[5], xfc->src[4], xfc->src[3], xfc->src[2], xfc->src[1], xfc->src[0]);
    }

    time_t now;
    time(&now);
    tagRecord* taginfo = nullptr;
    taginfo = tagRecord::findByMAC(xfc->src);
    if (taginfo != nullptr) {
        taginfo->expectedNextCheckin = now + 60;
        memset(taginfo->md5pending, 0, 16 * sizeof(uint8_t));
        clearPending(taginfo);
    }
    wsSendTaginfo(xfc->src, SYNC_TAGSTATUS);
    if (local) udpsync.netProcessXferTimeout(xfc);
}

void processDataReq(struct espAvailDataReq* eadr, bool local) {
    char buffer[64];

    tagRecord* taginfo = nullptr;
    taginfo = tagRecord::findByMAC(eadr->src);
    if (taginfo == nullptr) {
        taginfo = new tagRecord;
        memcpy(taginfo->mac, eadr->src, sizeof(taginfo->mac));
        taginfo->pending = false;
        tagDB.push_back(taginfo);
    }
    time_t now;
    time(&now);

    char hexmac[17];
    mac2hex(eadr->src, hexmac);

    if (!local) {
        if (taginfo->isExternal == false) {
            wsLog("moved AP from local to external " + String(hexmac));
        }
        taginfo->isExternal = true;
    } else {
        if (taginfo->isExternal == true) {
            wsLog("moved AP from external to local " + String(hexmac));
        }
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
        sprintf(buffer, "<ADR %02X%02X%02X%02X%02X%02X%02X%02X\n\0", eadr->src[7], eadr->src[6], eadr->src[5], eadr->src[4], eadr->src[3], eadr->src[2], eadr->src[1], eadr->src[0]);
    } else {
        sprintf(buffer, "<REMOTE ADR %02X%02X%02X%02X%02X%02X%02X%02X\n\0", eadr->src[7], eadr->src[6], eadr->src[5], eadr->src[4], eadr->src[3], eadr->src[2], eadr->src[1], eadr->src[0]);
    }

    Serial.print(buffer);
    wsSendTaginfo(eadr->src, SYNC_TAGSTATUS);
    if (local) {
        udpsync.netProcessDataReq(eadr);
    }
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
            wsSendTaginfo(taginfo->mac, SYNC_TAGSTATUS);
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

bool sendAPSegmentedData(uint8_t* dst, String data, uint16_t icons, bool inverted, bool local) {
    struct pendingData pending = {0};
    memcpy(pending.targetMac, dst, 8);
    pending.availdatainfo.dataType = DATATYPE_UK_SEGMENTED;
    pending.availdatainfo.dataSize = icons << 16;
    memcpy((void*)&(pending.availdatainfo.dataVer), data.c_str(), 10);
    pending.availdatainfo.dataTypeArgument = inverted;
    pending.availdatainfo.nextCheckIn = 0;
    pending.attemptsLeft = 120;
    char buffer[64];
    sprintf(buffer, ">AP Segmented Data %02X%02X%02X%02X%02X%02X%02X%02X\n\0", dst[7], dst[6], dst[5], dst[4], dst[3], dst[2], dst[1], dst[0]);
    Serial.print(buffer);
    if (local) {
        return sendDataAvail(&pending);
    } else {
        udpsync.netSendDataAvail(&pending);
        return true;
    }
}

bool showAPSegmentedInfo(uint8_t* dst, bool local) {
    struct pendingData pending = {0};
    memcpy(pending.targetMac, dst, 8);
    pending.availdatainfo.dataType = DATATYPE_UK_SEGMENTED;
    pending.availdatainfo.dataSize = 0x00;
    pending.availdatainfo.dataVer = 0x00;
    pending.availdatainfo.dataTypeArgument = 0;
    pending.availdatainfo.nextCheckIn = 0;
    pending.attemptsLeft = 120;
    char buffer[64];
    sprintf(buffer, ">SDA %02X%02X%02X%02X%02X%02X%02X%02X\n\0", dst[7], dst[6], dst[5], dst[4], dst[3], dst[2], dst[1], dst[0]);
    Serial.print(buffer);
    if (local) {
        return sendDataAvail(&pending);
    } else {
        udpsync.netSendDataAvail(&pending);
        return true;
    }
}

void updateTaginfoitem(struct TagInfo* taginfoitem) {
    tagRecord* taginfo = nullptr;
    taginfo = tagRecord::findByMAC(taginfoitem->mac);

    if (taginfo == nullptr) {
        taginfo = new tagRecord;
        memcpy(taginfo->mac, taginfoitem->mac, sizeof(taginfo->mac));
        taginfo->pending = false;
        tagDB.push_back(taginfo);
    }
    tagRecord initialTagInfo = *taginfo;

    switch (taginfoitem->syncMode) {
        case SYNC_USERCFG:
            taginfo->alias = String(taginfoitem->alias);
            taginfo->nextupdate = taginfoitem->nextupdate;
            break;
        case SYNC_TAGSTATUS:
            taginfo->lastseen = taginfoitem->lastseen;
            taginfo->nextupdate = taginfoitem->nextupdate;
            taginfo->pending = taginfoitem->pending;
            taginfo->expectedNextCheckin = taginfoitem->expectedNextCheckin;
            taginfo->hwType = taginfoitem->hwType;
            taginfo->wakeupReason = taginfoitem->wakeupReason;
            taginfo->capabilities = taginfoitem->capabilities;
            taginfo->pendingIdle = taginfoitem->pendingIdle;
            break;
    }

    char hexmac[17];
    mac2hex(taginfo->mac, hexmac);
    if (taginfo->contentMode != 12 && taginfoitem->contentMode != 12) {
        wsLog("Remote AP takes control over tag " + String(hexmac));
        taginfo->contentMode = 12;
    }

    if (taginfoitem->syncMode == SYNC_DELETE) {
        taginfo->contentMode = 255;
        wsSendTaginfo(taginfo->mac, SYNC_NOSYNC);
        deleteRecord(taginfoitem->mac);
    } else {
        bool hasChanges = (memcmp(&initialTagInfo, taginfo, sizeof(tagRecord)) != 0);
        if (hasChanges) {
            wsSendTaginfo(taginfo->mac, SYNC_NOSYNC);
        }
    }
}