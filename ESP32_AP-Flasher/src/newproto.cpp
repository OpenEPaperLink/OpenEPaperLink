#include "newproto.h"

#include <Arduino.h>
#include <FS.h>
#include <HTTPClient.h>
#include <MD5Builder.h>
#include <time.h>

#include "commstructs.h"
#include "serialap.h"
#include "settings.h"
#include "storage.h"
#include "system.h"
#include "tag_db.h"
#include "tagdata.h"
#include "udp.h"
#include "util.h"
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

uint8_t* getDataForFile(fs::File& file) {
    const size_t fileSize = file.size();
    uint8_t* ret = (uint8_t*)malloc(fileSize);
    if (ret) {
        file.seek(0);
        file.readBytes((char*)ret, fileSize);
    } else {
        Serial.printf("malloc failed for file with size %d\n", fileSize);
        util::printHeap();
    }
    return ret;
}

void prepareCancelPending(const uint8_t dst[8]) {
    struct pendingData pending = {0};
    memcpy(pending.targetMac, dst, 8);
    sendCancelPending(&pending);

    tagRecord* taginfo = tagRecord::findByMAC(dst);
    if (taginfo == nullptr) {
        if (config.lock) return;
        wsErr("Tag not found, this shouldn't happen.");
        return;
    }
    clearPending(taginfo);

    wsSendTaginfo(dst, SYNC_TAGSTATUS);
}

void prepareIdleReq(const uint8_t* dst, uint16_t nextCheckin) {
    if (nextCheckin > 0) {
        struct pendingData pending = {0};
        memcpy(pending.targetMac, dst, 8);
        pending.availdatainfo.dataType = DATATYPE_NOUPDATE;
        pending.availdatainfo.nextCheckIn = nextCheckin;
        pending.attemptsLeft = 10 + config.maxsleep;

        Serial.printf(">SDA %02X%02X%02X%02X%02X%02X%02X%02X NOP\n", dst[7], dst[6], dst[5], dst[4], dst[3], dst[2], dst[1], dst[0]);
        sendDataAvail(&pending);
    }
}

void prepareDataAvail(uint8_t* data, uint16_t len, uint8_t dataType, const uint8_t* dst) {
    tagRecord* taginfo = tagRecord::findByMAC(dst);
    if (taginfo == nullptr) {
        if (config.lock) return;
        wsErr("Tag not found, this shouldn't happen.");
        return;
    }

    clearPending(taginfo);
    taginfo->data = (uint8_t*)malloc(len);
    if (taginfo->data == nullptr) {
        wsErr("no memory allocation for data");
        return;
    }
    memcpy(taginfo->data, data, len);
    taginfo->pending = true;
    taginfo->len = len;
    taginfo->pendingIdle = 0;
    taginfo->filename = String();
    taginfo->dataType = dataType;
    memset(taginfo->md5pending, 0, 16 * sizeof(uint8_t));

    struct pendingData pending = {0};
    memcpy(pending.targetMac, dst, 8);
    pending.availdatainfo.dataSize = len;
    pending.availdatainfo.dataType = dataType;
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

bool prepareDataAvail(String& filename, uint8_t dataType, uint8_t dataTypeArgument, const uint8_t* dst, uint16_t nextCheckin, bool resend) {
    if (nextCheckin > config.maxsleep) nextCheckin = config.maxsleep;
    if (wsClientCount() && config.stopsleep == 1) nextCheckin = 0;
#ifdef YELLOW_IPS_AP
    if (filename == "direct") {
        char dst_path[64];
        sprintf(dst_path, "/current/%02X%02X%02X%02X%02X%02X%02X%02X.raw\0", dst[7], dst[6], dst[5], dst[4], dst[3], dst[2], dst[1], dst[0]);
        contentFS->remove(dst_path);
        return true;
    }
#endif

    tagRecord* taginfo = tagRecord::findByMAC(dst);
    if (taginfo == nullptr) {
        if (config.lock) return true;
        wsErr("Tag not found, this shouldn't happen.");
        return true;
    }

    filename = "/" + filename;
    Storage.begin();

    if (!contentFS->exists(filename)) {
        wsErr("File not found. " + filename);
        return false;
    }

    fs::File file = contentFS->open(filename);
    uint32_t filesize = file.size();
    if (filesize == 0) {
        file.close();
        wsErr("File has size 0. " + filename);
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

    if (memcmp(md5bytes, taginfo->md5pending, 16) == 0) {
        wsLog("new image is the same as current or already pending image. not updating tag.");
        wsSendTaginfo(dst, SYNC_TAGSTATUS);
        if (contentFS->exists(filename) && resend == false) {
            contentFS->remove(filename);
        }
        return true;
    }

    if (dataType != DATATYPE_FW_UPDATE) {
        char dst_path[64];
        sprintf(dst_path, "/current/%02X%02X%02X%02X%02X%02X%02X%02X.pending\0", dst[7], dst[6], dst[5], dst[4], dst[3], dst[2], dst[1], dst[0]);
        if (contentFS->exists(dst_path)) {
            contentFS->remove(dst_path);
        }
        if (resend == false) {
            contentFS->rename(filename, dst_path);
            filename = String(dst_path);
            wsLog("new image: " + String(dst_path));
        }

        time_t now;
        time(&now);
        taginfo->pendingIdle = nextCheckin * 60 + 60;
        clearPending(taginfo);
        taginfo->filename = filename;
        taginfo->len = filesize;
        taginfo->dataType = dataType;
        taginfo->pending = true;
        memcpy(taginfo->md5pending, md5bytes, sizeof(md5bytes));
    } else {
        char dst_path[64];
        sprintf(dst_path, "/current/%02X%02X%02X%02X%02X%02X%02X%02X.raw\0", dst[7], dst[6], dst[5], dst[4], dst[3], dst[2], dst[1], dst[0]);
        contentFS->remove(dst_path);
        sprintf(dst_path, "/current/%02X%02X%02X%02X%02X%02X%02X%02X.pending\0", dst[7], dst[6], dst[5], dst[4], dst[3], dst[2], dst[1], dst[0]);
        contentFS->remove(dst_path);
        wsLog("firmware upload pending");
        clearPending(taginfo);
        taginfo->filename = filename;
        taginfo->len = filesize;
        taginfo->dataType = dataType;
        taginfo->pending = true;
    }

    struct pendingData pending = {0};
    memcpy(pending.targetMac, dst, 8);
    pending.availdatainfo.dataType = dataType;
    pending.availdatainfo.dataVer = *((uint64_t*)md5bytes);
    pending.availdatainfo.dataSize = filesize;
    pending.availdatainfo.dataTypeArgument = dataTypeArgument;
    pending.availdatainfo.nextCheckIn = nextCheckin;
    pending.attemptsLeft = attempts;
    checkMirror(taginfo, &pending);
    if (taginfo->isExternal == false) {
        Serial.printf(">SDA %02X%02X%02X%02X%02X%02X%02X%02X TYPE 0x%02X\n", dst[7], dst[6], dst[5], dst[4], dst[3], dst[2], dst[1], dst[0], pending.availdatainfo.dataType);
        sendDataAvail(&pending);
    } else {
        udpsync.netSendDataAvail(&pending);
    }

    wsSendTaginfo(dst, SYNC_TAGSTATUS);
    return true;
}

void prepareExternalDataAvail(struct pendingData* pending, IPAddress remoteIP) {
    tagRecord* taginfo = tagRecord::findByMAC(pending->targetMac);
    if (taginfo == nullptr) {
        return;
    }
    if (taginfo->isExternal == false) {
        switch (pending->availdatainfo.dataType) {
            case DATATYPE_IMG_DIFF:
            case DATATYPE_IMG_RAW_1BPP:
            case DATATYPE_IMG_RAW_2BPP:
            case DATATYPE_IMG_RAW_1BPP_DIRECT: {
                Storage.begin();

                char hexmac[17];
                mac2hex(pending->targetMac, hexmac);
                String filename = "/current/" + String(hexmac) + ".pending";
                String imageUrl = "http://" + remoteIP.toString() + filename;
                wsLog("GET " + imageUrl);
                HTTPClient http;
                logLine("http prepareExternalDataAvail " + imageUrl);
                http.begin(imageUrl);
                int httpCode = http.GET();
                if (httpCode == 200) {
                    xSemaphoreTake(fsMutex, portMAX_DELAY);
                    File file = contentFS->open(filename, "w");
                    http.writeToStream(&file);
                    file.close();
                    xSemaphoreGive(fsMutex);
                } else if (httpCode == 404) {
                    imageUrl = "http://" + remoteIP.toString() + "/current/" + String(hexmac) + ".raw";
                    http.end();
                    logLine("http prepareExternalDataAvail " + imageUrl);
                    http.begin(imageUrl);
                    httpCode = http.GET();
                    if (httpCode == 200) {
                        xSemaphoreTake(fsMutex, portMAX_DELAY);
                        File file = contentFS->open(filename, "w");
                        http.writeToStream(&file);
                        file.close();
                        xSemaphoreGive(fsMutex);
                    }
                }
                http.end();

                fs::File file = contentFS->open(filename);
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
                clearPending(taginfo);
                taginfo->filename = filename;
                taginfo->len = filesize;
                taginfo->dataType = pending->availdatainfo.dataType;
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
                logLine("http DATATYPE_CUSTOM_LUT_OTA " + dataUrl);
                http.begin(dataUrl);
                int httpCode = http.GET();
                if (httpCode == 200) {
                    size_t len = http.getSize();
                    if (len > 0) {
                        clearPending(taginfo);
                        taginfo->data = new uint8_t[len];
                        WiFiClient* stream = http.getStreamPtr();
                        stream->readBytes(taginfo->data, len);
                        taginfo->dataType = pending->availdatainfo.dataType;
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
        // taginfo->contentMode = 12;
        // taginfo->nextupdate = 3216153600;
        checkMirror(taginfo, pending);
        sendDataAvail(pending);

        wsSendTaginfo(pending->targetMac, SYNC_NOSYNC);
    }
}

void processBlockRequest(struct espBlockRequest* br) {
    if (config.runStatus == RUNSTATUS_STOP) {
        return;
    }
    if (!checkCRC(br, sizeof(struct espBlockRequest))) {
        Serial.print("Failed CRC on a blockrequest received by the AP");
        return;
    }

    tagRecord* taginfo = tagRecord::findByMAC(br->src);
    if (taginfo == nullptr) {
        if (config.lock) return;
        prepareCancelPending(br->src);
        Serial.printf("blockrequest: couldn't find taginfo %02X%02X%02X%02X%02X%02X%02X%02X\n", br->src[7], br->src[6], br->src[5], br->src[4], br->src[3], br->src[2], br->src[1], br->src[0]);
        return;
    }

    if (taginfo->data == nullptr) {
        // not cached. open file, cache the data
        fs::File file = contentFS->open(taginfo->filename);
        if (!file) {
            Serial.print("No current file. Canceling request\n");
            prepareCancelPending(br->src);
            return;
        }
        taginfo->data = getDataForFile(file);
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
    if (config.runStatus == RUNSTATUS_STOP) {
        return;
    }
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
    if (contentFS->exists(dst_path) && contentFS->exists(src_path)) {
        contentFS->remove(dst_path);
    }
    if (contentFS->exists(src_path)) {
        if (config.preview) {
            contentFS->rename(src_path, dst_path);
        } else {
            contentFS->remove(src_path);
        }
    }

    time_t now;
    time(&now);
    tagRecord* taginfo = tagRecord::findByMAC(xfc->src);
    if (taginfo != nullptr) {
        memcpy(taginfo->md5, taginfo->md5pending, sizeof(taginfo->md5pending));
        clearPending(taginfo);
        taginfo->wakeupReason = 0;
        if (taginfo->contentMode == 12 && local == false) {
            if (contentFS->exists(dst_path)) {
                contentFS->remove(dst_path);
            }
        }
    }
    wsSendTaginfo(xfc->src, SYNC_TAGSTATUS);
    if (local) udpsync.netProcessXferComplete(xfc);
}

void processXferTimeout(struct espXferComplete* xfc, bool local) {
    if (config.runStatus == RUNSTATUS_STOP) {
        return;
    }
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
    tagRecord* taginfo = tagRecord::findByMAC(xfc->src);
    if (taginfo != nullptr) {
        taginfo->pendingIdle = 60;
        memset(taginfo->md5pending, 0, 16 * sizeof(uint8_t));
        clearPending(taginfo);
    }
    wsSendTaginfo(xfc->src, SYNC_TAGSTATUS);
    if (local) udpsync.netProcessXferTimeout(xfc);
}

void processDataReq(struct espAvailDataReq* eadr, bool local, IPAddress remoteIP) {
    if (config.runStatus == RUNSTATUS_STOP) {
        return;
    }
    char buffer[64];

    tagRecord* taginfo = tagRecord::findByMAC(eadr->src);
    if (taginfo == nullptr) {
        if (config.lock == 1 || (config.lock == 2 && eadr->adr.wakeupReason != WAKEUP_REASON_FIRSTBOOT)) return;
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
            taginfo->isExternal = true;
        }
        taginfo->apIp = remoteIP;
    } else {
        if (taginfo->isExternal == true) {
            wsLog("moved AP from external to local " + String(hexmac));
            taginfo->isExternal = false;
        }
        taginfo->apIp = IPAddress(0, 0, 0, 0);
    }

    if (taginfo->pendingIdle == 0) {
        taginfo->expectedNextCheckin = now + 60;
    } else if (taginfo->pendingIdle == 9999) {
        taginfo->expectedNextCheckin = 3216153600;
        taginfo->pendingIdle = 0;
    } else {
        taginfo->expectedNextCheckin = now + 60 * taginfo->pendingIdle;
        taginfo->pendingIdle = 0;
    }
    taginfo->lastseen = now;

    if (eadr->adr.lastPacketRSSI != 0) {
        if (eadr->adr.wakeupReason >= 0xF0) {
            if (!taginfo->pending) taginfo->nextupdate = 0;
            memset(taginfo->md5, 0, 16 * sizeof(uint8_t));
            memset(taginfo->md5pending, 0, 16 * sizeof(uint8_t));

            if (local) {
                const char* reason = "";
                if (eadr->adr.wakeupReason == WAKEUP_REASON_FIRSTBOOT)
                    reason = "Booting";
                else if (eadr->adr.wakeupReason == WAKEUP_REASON_NETWORK_SCAN)
                    reason = "Network scan";
                else if (eadr->adr.wakeupReason == WAKEUP_REASON_WDT_RESET)
                    reason = "Watchdog reset";
                sprintf(buffer, "%02X%02X%02X%02X%02X%02X%02X%02X %s", eadr->src[7], eadr->src[6], eadr->src[5], eadr->src[4], eadr->src[3], eadr->src[2], eadr->src[1], eadr->src[0], reason);
                logLine(buffer);
            }
        }
        if (local && taginfo->batteryMv != eadr->adr.batteryMv) {
            sprintf(buffer, "%02X%02X%02X%02X%02X%02X%02X%02X battery went from %.2fV to %.2fV", eadr->src[7], eadr->src[6], eadr->src[5], eadr->src[4], eadr->src[3], eadr->src[2], eadr->src[1], eadr->src[0], static_cast<float>(taginfo->batteryMv) / 1000.0, static_cast<float>(eadr->adr.batteryMv) / 1000.0);
            logLine(buffer);
        }

        taginfo->LQI = eadr->adr.lastPacketLQI;
        taginfo->hwType = eadr->adr.hwType;
        taginfo->RSSI = eadr->adr.lastPacketRSSI;
        taginfo->temperature = eadr->adr.temperature;
        taginfo->batteryMv = eadr->adr.batteryMv;
        taginfo->hwType = eadr->adr.hwType;
        taginfo->wakeupReason = eadr->adr.wakeupReason;
        taginfo->capabilities = eadr->adr.capabilities;
        taginfo->currentChannel = eadr->adr.currentChannel;
        taginfo->tagSoftwareVersion = eadr->adr.tagSoftwareVersion;
    }
    if (local) {
        sprintf(buffer, "<ADR %02X%02X%02X%02X%02X%02X%02X%02X\n\0", eadr->src[7], eadr->src[6], eadr->src[5], eadr->src[4], eadr->src[3], eadr->src[2], eadr->src[1], eadr->src[0]);
        Serial.print(buffer);
    } else {
        // sprintf(buffer, "<REMOTE ADR %02X%02X%02X%02X%02X%02X%02X%02X\n\0", eadr->src[7], eadr->src[6], eadr->src[5], eadr->src[4], eadr->src[3], eadr->src[2], eadr->src[1], eadr->src[0]);
    }

    if (local) {
        wsSendTaginfo(eadr->src, SYNC_TAGSTATUS);
        udpsync.netProcessDataReq(eadr);
    } else {
        wsSendTaginfo(eadr->src, SYNC_NOSYNC);
    }
}

void processTagReturnData(struct espTagReturnData* trd, uint8_t len, bool local) {
    if (!checkCRC(trd, len)) {
        return;
    }

    const uint8_t payloadLength = trd->len - 11;

    // Replace this stuff with something that handles the data coming from the tag. This is here for demo purposes!
    char buffer[64];
    sprintf(buffer, "<TRD %02X%02X%02X%02X%02X%02X%02X%02X\n", trd->src[7], trd->src[6], trd->src[5], trd->src[4], trd->src[3], trd->src[2], trd->src[1], trd->src[0]);
    wsLog((String)buffer);
    sprintf(buffer, "TRD Data: len=%d, type=%d, ver=0x%08X\n", payloadLength, trd->returnData.dataType, trd->returnData.dataVer);
    wsLog((String)buffer);

    TagData::parse(trd->src, trd->returnData.dataType, trd->returnData.data, payloadLength);
}

void refreshAllPending() {
    for (int16_t c = 0; c < tagDB.size(); c++) {
        tagRecord* taginfo = tagDB.at(c);
        if (taginfo->pending) {
            clearPending(taginfo);
            taginfo->nextupdate = 0;
            memset(taginfo->md5, 0, 16 * sizeof(uint8_t));
            memset(taginfo->md5pending, 0, 16 * sizeof(uint8_t));
            wsSendTaginfo(taginfo->mac, SYNC_TAGSTATUS);
        }
    }
};

void updateContent(const uint8_t* dst) {
    tagRecord* taginfo = tagRecord::findByMAC(dst);
    if (taginfo != nullptr) {
        clearPending(taginfo);
        taginfo->nextupdate = 0;
        memset(taginfo->md5, 0, 16 * sizeof(uint8_t));
        memset(taginfo->md5pending, 0, 16 * sizeof(uint8_t));
        wsSendTaginfo(taginfo->mac, SYNC_TAGSTATUS);
    }
}

void setAPchannel() {
    if (config.channel == 0) {
        // trigger channel autoselect
        UDPcomm udpsync;
        udpsync.getAPList();
    } else {
        if (curChannel.channel != config.channel) {
            curChannel.channel = config.channel;
            sendChannelPower(&curChannel);
        }
    }
}

bool sendAPSegmentedData(const uint8_t* dst, String data, uint16_t icons, bool inverted, bool local) {
    struct pendingData pending = {0};
    memcpy(pending.targetMac, dst, 8);
    pending.availdatainfo.dataType = DATATYPE_UK_SEGMENTED;
    pending.availdatainfo.dataSize = icons << 16;
    memcpy((void*)&(pending.availdatainfo.dataVer), data.c_str(), 10);
    pending.availdatainfo.dataTypeArgument = inverted;
    pending.availdatainfo.nextCheckIn = 0;
    pending.attemptsLeft = 120;
    Serial.printf(">AP Segmented Data %02X%02X%02X%02X%02X%02X%02X%02X\n\0", dst[7], dst[6], dst[5], dst[4], dst[3], dst[2], dst[1], dst[0]);
    if (local) {
        return sendDataAvail(&pending);
    } else {
        udpsync.netSendDataAvail(&pending);
        return true;
    }
}

bool showAPSegmentedInfo(const uint8_t* dst, bool local) {
    struct pendingData pending = {0};
    memcpy(pending.targetMac, dst, 8);
    pending.availdatainfo.dataType = DATATYPE_UK_SEGMENTED;
    pending.availdatainfo.dataSize = 0x00;
    pending.availdatainfo.dataVer = 0x00;
    pending.availdatainfo.dataTypeArgument = 0;
    pending.availdatainfo.nextCheckIn = 0;
    pending.attemptsLeft = 120;
    Serial.printf(">SDA %02X%02X%02X%02X%02X%02X%02X%02X\n\0", dst[7], dst[6], dst[5], dst[4], dst[3], dst[2], dst[1], dst[0]);
    if (local) {
        return sendDataAvail(&pending);
    } else {
        udpsync.netSendDataAvail(&pending);
        return true;
    }
}

bool sendTagCommand(const uint8_t* dst, uint8_t cmd, bool local, const uint8_t* payload) {
    struct pendingData pending = {0};
    memcpy(pending.targetMac, dst, 8);
    pending.availdatainfo.dataType = DATATYPE_COMMAND_DATA;
    pending.availdatainfo.dataTypeArgument = cmd;
    pending.availdatainfo.nextCheckIn = 0;
    if (payload != nullptr) {
        memcpy(&pending.availdatainfo.dataVer, payload, sizeof(uint64_t));
        memcpy(&pending.availdatainfo.dataSize, payload + sizeof(uint64_t), sizeof(uint32_t));
    }
    pending.attemptsLeft = 120;
    Serial.printf(">Tag CMD %02X%02X%02X%02X%02X%02X%02X%02X\n\0", dst[7], dst[6], dst[5], dst[4], dst[3], dst[2], dst[1], dst[0]);
    if (local) {
        return sendDataAvail(&pending);
    } else {
        udpsync.netSendDataAvail(&pending);
        return true;
    }
}

void updateTaginfoitem(struct TagInfo* taginfoitem, IPAddress remoteIP) {
    tagRecord* taginfo = tagRecord::findByMAC(taginfoitem->mac);

    if (taginfo == nullptr) {
        if (config.lock) return;
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
    if (taginfo->contentMode != 12 && taginfoitem->contentMode != 12 && taginfoitem->contentMode != 0) {
        wsLog("Remote AP at " + remoteIP.toString() + " takes control over tag " + String(hexmac));
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

bool checkMirror(struct tagRecord* taginfo, struct pendingData* pending) {
    for (int16_t c = 0; c < tagDB.size(); c++) {
        tagRecord* taginfo2 = tagDB.at(c);
        if (taginfo2->contentMode == 20) {
            DynamicJsonDocument doc(500);
            deserializeJson(doc, taginfo2->modeConfigJson);
            JsonObject cfgobj = doc.as<JsonObject>();
            uint8_t mac[8] = {0};
            if (hex2mac(cfgobj["mac"], mac) && memcmp(mac, taginfo->mac, sizeof(mac)) == 0) {
                if (taginfo->data == nullptr) {
                    fs::File file = contentFS->open(taginfo->filename);
                    if (!file) {
                        return false;
                    }
                    taginfo->data = getDataForFile(file);
                    file.close();
                }

                clearPending(taginfo2);
                taginfo2->expectedNextCheckin = taginfo->expectedNextCheckin;
                taginfo2->filename = taginfo->filename;
                taginfo2->len = taginfo->len;
                taginfo2->data = taginfo->data;  // copy buffer pointer
                taginfo2->dataType = taginfo->dataType;
                taginfo2->pending = true;
                taginfo2->nextupdate = 3216153600;
                memcpy(taginfo2->md5pending, taginfo->md5pending, sizeof(taginfo->md5pending));

                struct pendingData pending2 = {0};
                memcpy(pending2.targetMac, taginfo2->mac, 8);
                pending2.availdatainfo.dataType = taginfo2->dataType;
                pending2.availdatainfo.dataVer = *((uint64_t*)taginfo2->md5pending);
                pending2.availdatainfo.dataSize = taginfo2->len;
                pending2.availdatainfo.dataTypeArgument = pending->availdatainfo.dataTypeArgument;
                pending2.availdatainfo.nextCheckIn = pending->availdatainfo.nextCheckIn;
                pending2.attemptsLeft = pending->attemptsLeft;

                if (taginfo2->isExternal == false) {
                    sendDataAvail(&pending2);
                } else {
                    char dst_path[64];
                    sprintf(dst_path, "/current/%02X%02X%02X%02X%02X%02X%02X%02X.pending\0", taginfo2->mac[7], taginfo2->mac[6], taginfo2->mac[5], taginfo2->mac[4], taginfo2->mac[3], taginfo2->mac[2], taginfo2->mac[1], taginfo2->mac[0]);
                    xSemaphoreTake(fsMutex, portMAX_DELAY);
                    File file = contentFS->open(dst_path, "w");
                    if (file) {
                        file.write(taginfo2->data, taginfo2->len);
                        file.close();
                        xSemaphoreGive(fsMutex);
                        udpsync.netSendDataAvail(&pending2);
                    } else {
                        xSemaphoreGive(fsMutex);
                    }
                }

                wsSendTaginfo(taginfo2->mac, SYNC_TAGSTATUS);
            }
        }
    }
    return false;
}
