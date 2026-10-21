#include "newproto.h"

#include <Arduino.h>
#include <FS.h>
#include <HTTPClient.h>
#include <MD5Builder.h>
#include <time.h>

#include <algorithm>
#include <cstring>
#include <mutex>
#include <unordered_map>
#include <vector>

#include "../../oepl-radio-v2.h"
#include "od_keys.h"
#include "od_session.h"
#include "serialap.h"
#include "settings.h"
#include "storage.h"
#include "system.h"
#include "tag_db.h"
#include "tagdata.h"
#include "udp.h"
#include "util.h"
#include "web.h"

extern uint16_t sendBlock(const void* data, const uint16_t len, uint64_t ver, uint32_t offset, uint32_t totalSize);
extern bool cacheBlock(const void* data, const uint16_t len, uint64_t ver, uint32_t offset, uint32_t totalSize);
extern UDPcomm udpsync;
// Queued items are heap allocated, so a PendingItem* stays valid when the vector grows. Dequeued items
// are kept for a while before they are freed, because other tasks may still use them: a block request
// being sent, a BLE upload or an asynchronous /getdata response. Guarded by queueMutex.
std::vector<PendingItem*> pendingQueue;
std::recursive_mutex queueMutex;
struct RetiredItem {
    PendingItem* item;
    uint32_t retiredAt;
};
static std::vector<RetiredItem> retiredItems;
static constexpr uint32_t RETIRED_ITEM_GRACE_MS = 120000;

// What the radio holds for each tag (from the last SDA; cleared on cancel, xfer complete and timeout), and
// what a tag got offered at its last check-in. An xfer complete carries no hash, so this tells which item a
// tag finished; and the offered nextCheckIn tells when the tag will check in again (#466).
// Only for tags on this AP's radio. Guarded by queueMutex.
struct Announced {
    uint64_t dataVer;
    uint16_t nextCheckIn;   // minutes, or seconds with bit 15 set
    uint16_t attemptsLeft;  // minutes the radio keeps it
    uint32_t announcedAt;   // millis()
};
static std::unordered_map<uint64_t, Announced> announced;
static std::unordered_map<uint64_t, uint64_t> offeredDataVer;

static uint64_t macKey(const uint8_t* mac) {
    uint64_t key;
    memcpy(&key, mac, sizeof(key));
    return key;
}

/// @brief Remember what was sent to the radio for a tag; nullptr: the radio has nothing for it
static void setAnnounced(const uint8_t* mac, const struct pendingData* pending) {
    std::lock_guard<std::recursive_mutex> lock(queueMutex);
    if (pending == nullptr) {
        announced[macKey(mac)] = {0, 0, 0, millis()};
    } else {
        announced[macKey(mac)] = {pending->availdatainfo.dataVer, pending->availdatainfo.nextCheckIn, pending->attemptsLeft, millis()};
    }
}

/// @brief Remember what the radio just offered the tag that checked in
/// @param sleepSeconds set to the time until the tag's next check-in it was told (0: no instruction)
/// @return false if unknown (for example after a reboot)
static bool recordOffered(const uint8_t* mac, uint32_t& sleepSeconds) {
    std::lock_guard<std::recursive_mutex> lock(queueMutex);
    sleepSeconds = 0;
    auto it = announced.find(macKey(mac));
    if (it == announced.end()) {
        offeredDataVer.erase(macKey(mac));
        return false;
    }
    const Announced& a = it->second;
    // the radio counts nextCheckIn and attemptsLeft down once a minute; round up, so the AP rather
    // expects the tag too early than too late
    const uint32_t elapsedMinutes = (millis() - a.announcedAt + 59999) / 60000;
    if (a.attemptsLeft == 0 || elapsedMinutes >= a.attemptsLeft) {
        offeredDataVer[macKey(mac)] = 0;  // the radio dropped it
        return true;
    }
    offeredDataVer[macKey(mac)] = a.dataVer;
    if (a.nextCheckIn & 0x8000) {
        sleepSeconds = a.nextCheckIn & 0x7FFF;
    } else if (a.nextCheckIn > elapsedMinutes) {
        sleepSeconds = (a.nextCheckIn - elapsedMinutes) * 60;
    }
    return true;
}

/// @brief False if the tag was offered (and so finished) another item than the queued one
static bool offeredMatches(const uint8_t* mac, const uint64_t dataVer) {
    std::lock_guard<std::recursive_mutex> lock(queueMutex);
    auto it = offeredDataVer.find(macKey(mac));
    // unknown, or the radio had dropped it by our count: assume it matches
    return it == offeredDataVer.end() || it->second == 0 || it->second == dataVer;
}

void addCRC(void* p, uint8_t len) {
    uint8_t total = 0;
    for (uint8_t c = 1; c < len; c++) {
        total += ((uint8_t*)p)[c];
    }
    ((uint8_t*)p)[0] = total;
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
        Serial.printf("malloc failed for file with size %d\r\n", fileSize);
        wsErr("malloc failed while reading file");
        util::printHeap();
    }
    return ret;
}

void prepareCancelPending(const uint8_t dst[8]) {
    struct pendingData pending = {0};
    memcpy(pending.targetMac, dst, 8);
    sendCancelPending(&pending);
    setAnnounced(dst, nullptr);

    tagRecord* taginfo = tagRecord::findByMAC(dst);
    if (taginfo == nullptr) {
        if (config.lock) return;
        wsErr("Tag not found, this shouldn't happen.");
        return;
    }
    clearPending(taginfo);
    dequeueAllItems(dst);
    taginfo->pendingCount = countQueueItem(dst);
    wsSendTaginfo(dst, SYNC_TAGSTATUS);
}

void prepareIdleReq(const uint8_t* dst, uint16_t nextCheckin) {
    if (nextCheckin > 0 && countQueueItem(dst) == 0) {
        struct pendingData pending = {0};
        memcpy(pending.targetMac, dst, 8);
        pending.availdatainfo.dataType = DATATYPE_NOUPDATE;
        pending.availdatainfo.nextCheckIn = nextCheckin;
        pending.attemptsLeft = 10 + config.maxsleep;

        Serial.printf(">SDA %02X%02X%02X%02X%02X%02X%02X%02X sleeping %d minutes\r\n", dst[7], dst[6], dst[5], dst[4], dst[3], dst[2], dst[1], dst[0], nextCheckin);
        if (sendDataAvail(&pending)) setAnnounced(dst, &pending);
    }
}

void prepareDataAvail(const uint8_t* dst) {
    // resend
    tagRecord* taginfo = tagRecord::findByMAC(dst);
    if (taginfo == nullptr) {
        if (config.lock) return;
        wsErr("Tag not found, this shouldn't happen.");
        return;
    }

    taginfo->pendingCount++;
    taginfo->pendingIdle = 0;

    struct pendingData pending = {0};
    memcpy(pending.targetMac, dst, 8);
    pending.availdatainfo.dataSize = taginfo->len;
    pending.availdatainfo.dataType = taginfo->dataType;
    pending.availdatainfo.nextCheckIn = 0;
    memcpy(&pending.availdatainfo.dataVer, taginfo->md5, sizeof(uint64_t));
    pending.attemptsLeft = 10;

    queueDataAvail(&pending, !taginfo->isExternal);
    if (taginfo->isExternal) {
        udpsync.netSendDataAvail(&pending);
    }

    wsSendTaginfo(dst, SYNC_TAGSTATUS);
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

    uint8_t md5bytes[16];
    {
        MD5Builder md5;
        md5.begin();
        md5.add(data, len);
        md5.calculate();
        md5.getBytes(md5bytes);
    }

    memcpy(taginfo->data, data, len);
    taginfo->pendingCount++;
    taginfo->len = len;
    taginfo->pendingIdle = 0;
    taginfo->filename = String();
    taginfo->dataType = dataType;

    struct pendingData pending = {0};
    memcpy(pending.targetMac, dst, 8);
    pending.availdatainfo.dataSize = len;
    pending.availdatainfo.dataType = dataType;
    pending.availdatainfo.nextCheckIn = 5 | 0x8000;  // 5 seconds
    pending.availdatainfo.dataVer = *((uint64_t*)md5bytes);
    pending.attemptsLeft = 10;

    queueDataAvail(&pending, !taginfo->isExternal);
    if (taginfo->isExternal) {
        udpsync.netSendDataAvail(&pending);
    }

    wsSendTaginfo(dst, SYNC_TAGSTATUS);
}

static bool isImageDataType(const uint8_t dataType) {
    return (dataType >= DATATYPE_IMG_RAW_1BPP && dataType <= DATATYPE_IMG_RAW_4BPP) || dataType == DATATYPE_IMG_ZLIB || dataType == DATATYPE_IMG_G5;
}

bool prepareDataAvail(String& filename, uint8_t dataType, uint8_t dataTypeArgument, const uint8_t* dst, uint16_t nextCheckin, bool resend) {
    if ((nextCheckin & 0x8000) == 0 && nextCheckin > config.maxsleep) nextCheckin = config.maxsleep;
    if ((nextCheckin & 0x8000) == 0 && wsClientCount() && (config.stopsleep == 1)) nextCheckin = 0;
#ifdef HAS_TFT
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

    if (!filename.startsWith("/")) {
        filename = "/" + filename;
    }

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

    if (memcmp(md5bytes, taginfo->md5, 8) == 0) {
        wsLog("new image is the same as current image. not updating tag.");
        wsSendTaginfo(dst, SYNC_TAGSTATUS);
        if (contentFS->exists(filename) && resend == false) {
            contentFS->remove(filename);
        }
        return true;
    }

    if (dataType != DATATYPE_FW_UPDATE) {
        char dst_path[64];
        sprintf(dst_path, "/current/%02X%02X%02X%02X%02X%02X%02X%02X_%lu.pending", dst[7], dst[6], dst[5], dst[4], dst[3], dst[2], dst[1], dst[0], millis() % 1000000);
        if (contentFS->exists(dst_path)) {
            contentFS->remove(dst_path);
        }
        if (resend == false) {
            contentFS->rename(filename, dst_path);
            filename = String(dst_path);
            wsLog("new image: " + filename);
        }

        time_t now;
        time(&now);

        taginfo->pendingIdle = (nextCheckin & 0x8000) ? (nextCheckin & 0x7FFF) + 5 : (nextCheckin * 60) + 60;
        clearPending(taginfo);
    } else {
        wsLog("firmware upload pending");
        clearPending(taginfo);
    }
    taginfo->filename = filename;
    taginfo->len = filesize;
    taginfo->dataType = dataType;
    if (isImageDataType(dataType)) taginfo->imageDataType = dataType;
    taginfo->pendingCount++;

    struct pendingData pending = {0};
    memcpy(pending.targetMac, dst, 8);
    pending.availdatainfo.dataType = dataType;
    pending.availdatainfo.dataVer = *((uint64_t*)md5bytes);
    pending.availdatainfo.dataSize = filesize;
    pending.availdatainfo.dataTypeArgument = dataTypeArgument;
    pending.availdatainfo.nextCheckIn = nextCheckin;
    pending.attemptsLeft = MAX_XFER_ATTEMPTS;
    checkMirror(taginfo, &pending);
    queueDataAvail(&pending, !taginfo->isExternal);
    if (taginfo->isExternal == false) {
        Serial.printf(">SDA %02X%02X%02X%02X%02X%02X%02X%02X TYPE 0x%02X\r\n", dst[7], dst[6], dst[5], dst[4], dst[3], dst[2], dst[1], dst[0], pending.availdatainfo.dataType);
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
            case DATATYPE_IMG_ZLIB:
            case DATATYPE_IMG_RAW_1BPP:
            case DATATYPE_IMG_RAW_2BPP:
            case DATATYPE_IMG_G5:
            case DATATYPE_IMG_RAW_3BPP:
            case DATATYPE_IMG_RAW_4BPP: {
                char hexmac[17];
                mac2hex(pending->targetMac, hexmac);
                String filename = "/current/" + String(hexmac) + "_" + String(millis() % 1000000) + ".pending";
                char md5[17];
                mac2hex(reinterpret_cast<uint8_t*>(&pending->availdatainfo.dataVer), md5);
                char imageUrl[80];
                snprintf(imageUrl, sizeof(imageUrl), "http://%s/getdata?mac=%s&md5=%s", remoteIP.toString().c_str(), hexmac, md5);
                wsLog("prepareExternalDataAvail GET " + String(imageUrl));
                HTTPClient http;
                http.begin(imageUrl);
                int httpCode = http.GET();
                if (httpCode == 200) {
                    // LockedFile: don't hold fsMutex while waiting for the network
                    LockedFile file(filename, "w");
                    http.writeToStream(&file);
                    file.close();
                } else if (httpCode == 404) {
                    snprintf(imageUrl, sizeof(imageUrl), "http://%s/current/%s.raw", remoteIP.toString().c_str(), hexmac);
                    // imageUrl = "http://" + remoteIP.toString() + "/current/" + String(hexmac) + ".raw";
                    http.end();
                    http.begin(imageUrl);
                    httpCode = http.GET();
                    if (httpCode == 200) {
                        LockedFile file(filename, "w");
                        http.writeToStream(&file);
                        file.close();
                    }
                } else {
                    logLine("prepareExternalDataAvail " + String(imageUrl) + " error " + String(httpCode));
                    wsLog("error " + String(httpCode));
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
                taginfo->pendingCount++;
                break;
            }
            case DATATYPE_NFC_RAW_CONTENT:
            case DATATYPE_NFC_URL_DIRECT: {
                char hexmac[17];
                mac2hex(pending->targetMac, hexmac);
                char dataUrl[80];
                char md5[17];
                mac2hex(reinterpret_cast<uint8_t*>(&pending->availdatainfo.dataVer), md5);
                snprintf(dataUrl, sizeof(dataUrl), "http://%s/getdata?mac=%s&md5=%s", remoteIP.toString().c_str(), hexmac, md5);
                wsLog("GET " + String(dataUrl));
                HTTPClient http;
                logLine("http DATATYPE_NFC_* " + String(dataUrl));
                http.begin(dataUrl);
                int httpCode = http.GET();
                if (httpCode == 200) {
                    size_t len = http.getSize();
                    if (len > 0) {
                        clearPending(taginfo);
                        // malloc, not new[]: the buffer is released with free() (clearPending)
                        taginfo->data = (uint8_t*)malloc(len);
                        if (taginfo->data == nullptr) {
                            wsErr("no memory allocation for data");
                            http.end();
                            return;
                        }
                        WiFiClient* stream = http.getStreamPtr();
                        stream->readBytes(taginfo->data, len);
                        taginfo->dataType = pending->availdatainfo.dataType;
                        taginfo->pendingCount++;
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
        checkMirror(taginfo, pending);
        queueDataAvail(pending, !taginfo->isExternal);

        wsSendTaginfo(pending->targetMac, SYNC_NOSYNC);
    }
}

void processBlockRequest(struct espBlockRequest* br) {
    uint32_t t = millis();
    if (config.runStatus == RUNSTATUS_STOP) {
        return;
    }
    if (!checkCRC(br, sizeof(struct espBlockRequest))) {
        Serial.print("Failed CRC on a blockrequest received by the AP");
        return;
    }

    PendingItem* queueItem = getQueueItem(br->src, br->ver);
    if (queueItem == nullptr && countQueueItem(br->src) > 0) {
        // The tag asks for data that has been replaced by a newer item (for example a newer image).
        // Don't cancel everything: announce the current item again, so the radio replaces what it has
        // for this tag and the tag gets the new data on its next check-in.
        Serial.printf("blockrequest: outdated data requested by %02X%02X%02X%02X%02X%02X%02X%02X, announcing current item\r\n", br->src[7], br->src[6], br->src[5], br->src[4], br->src[3], br->src[2], br->src[1], br->src[0]);
        checkQueue(br->src);
        return;
    }
    if (queueItem == nullptr) {
        prepareCancelPending(br->src);
        Serial.printf("blockrequest: couldn't find taginfo %02X%02X%02X%02X%02X%02X%02X%02X\r\n", br->src[7], br->src[6], br->src[5], br->src[4], br->src[3], br->src[2], br->src[1], br->src[0]);
        return;
    }
    if (!loadQueueItemData(queueItem)) {
        Serial.print("No data for " + String(queueItem->filename) + ", canceling request\r\n");
        prepareCancelPending(br->src);
        return;
    }

    // check if we're not exceeding max blocks (to prevent sendBlock from exceeding its boundary)
    uint8_t totalblocks = (queueItem->len / BLOCK_DATA_SIZE);
    if (queueItem->len % BLOCK_DATA_SIZE) totalblocks++;
    if (br->blockId >= totalblocks) {
        br->blockId = totalblocks - 1;
    }
    uint32_t len = queueItem->len - (BLOCK_DATA_SIZE * br->blockId);
    if (len > BLOCK_DATA_SIZE) len = BLOCK_DATA_SIZE;
    uint16_t checksum = sendBlock(queueItem->data + (br->blockId * BLOCK_DATA_SIZE), len, br->ver, br->blockId * BLOCK_DATA_SIZE, queueItem->len);
    char buffer[150];
    sprintf(buffer, "%02X%02X%02X%02X%02X%02X%02X%02X block request %s block %d, len %d checksum %u\0", br->src[7], br->src[6], br->src[5], br->src[4], br->src[3], br->src[2], br->src[1], br->src[0], queueItem->filename, br->blockId, len, checksum);
    wsLog((String)buffer);
    wsSendUploadProgress(br->src, br->blockId + 1, totalblocks);  // live block x/total for the tag card
    Serial.printf("<RQB file %s block %d, len %d checksum %u\r\n\0", queueItem->filename, br->blockId, len, checksum);
}

// Protocol v2 radios cache blocks: send the whole item while the tag sleeps, so the radio answers its
// block requests without waiting for the S3. Without it (cache full, radio reset) the radio asks
// for each block as before.
void processPreload(struct espBlockRequest* br) {
    if (config.runStatus == RUNSTATUS_STOP) return;
    PendingItem* queueItem = getQueueItem(br->src, br->ver);
    if (queueItem == nullptr || !loadQueueItemData(queueItem)) return;
    const uint32_t t = millis();
    uint16_t blocks = 0;
    for (uint32_t offset = 0; offset < queueItem->len; offset += BLOCK_DATA_SIZE) {
        const uint32_t len = queueItem->len - offset > BLOCK_DATA_SIZE ? BLOCK_DATA_SIZE : queueItem->len - offset;
        if (!cacheBlock(queueItem->data + offset, len, br->ver, offset, queueItem->len)) break;
        blocks++;
    }
    const uint16_t total = (queueItem->len + BLOCK_DATA_SIZE - 1) / BLOCK_DATA_SIZE;
    char buffer[150];
    sprintf(buffer, "%02X%02X%02X%02X%02X%02X%02X%02X %s sent to the radio in advance, %d of %d blocks in %lu ms", br->src[7], br->src[6], br->src[5], br->src[4], br->src[3], br->src[2], br->src[1], br->src[0], queueItem->filename, blocks, total, (unsigned long)(millis() - t));
    wsLog((String)buffer);
}

// The radio sent a block to a tag: the live block x/total on the tag card, also for cached blocks
// A v2 radio served a block from its cache. The log line starts with the MAC, so the tag card
// flashes as it does for a block request on the old protocol.
void processBlockServed(const struct v2BlockServed* bs) {
    PendingItem* queueItem = getQueueItem(bs->mac, bs->dataVer);
    if (queueItem == nullptr) return;
    const uint16_t total = (queueItem->len + BLOCK_DATA_SIZE - 1) / BLOCK_DATA_SIZE;
    char buffer[150];
    snprintf(buffer, sizeof(buffer), "%02X%02X%02X%02X%02X%02X%02X%02X block served %s block %d, %d parts, %d frames, %d ms late",
             bs->mac[7], bs->mac[6], bs->mac[5], bs->mac[4], bs->mac[3], bs->mac[2], bs->mac[1], bs->mac[0],
             queueItem->filename, bs->blockId, bs->partsRequested, bs->framesSent, bs->lateMs);
    wsLog((String)buffer);
    wsSendUploadProgress(bs->mac, bs->blockId + 1, total);
}

void processXferComplete(struct espXferComplete* xfc, bool local) {
    if (config.runStatus == RUNSTATUS_STOP) {
        return;
    }
    char buffer[64];
    sprintf(buffer, "%02X%02X%02X%02X%02X%02X%02X%02X reports xfer complete\r\n\0", xfc->src[7], xfc->src[6], xfc->src[5], xfc->src[4], xfc->src[3], xfc->src[2], xfc->src[1], xfc->src[0]);
    wsLog((String)buffer);

    if (local) {
        Serial.printf("<XFC %02X%02X%02X%02X%02X%02X%02X%02X\r\n", xfc->src[7], xfc->src[6], xfc->src[5], xfc->src[4], xfc->src[3], xfc->src[2], xfc->src[1], xfc->src[0]);
    } else {
        Serial.printf("<REMOTE XFC %02X%02X%02X%02X%02X%02X%02X%02X\r\n", xfc->src[7], xfc->src[6], xfc->src[5], xfc->src[4], xfc->src[3], xfc->src[2], xfc->src[1], xfc->src[0]);
    }

    time_t now;
    time(&now);

    char dst_path[64];
    sprintf(dst_path, "/current/%02X%02X%02X%02X%02X%02X%02X%02X.raw\0", xfc->src[7], xfc->src[6], xfc->src[5], xfc->src[4], xfc->src[3], xfc->src[2], xfc->src[1], xfc->src[0]);

    uint8_t md5bytes[16];
    bool haveMd5 = false;  // false for a repeated xfer complete, when the item is no longer queued
    bool wasDeepSleepCmd = false;
    if (local) setAnnounced(xfc->src, nullptr);  // the radio cleared its slot for this tag
    PendingItem* queueItem = getQueueItem(xfc->src);
    if (local && queueItem != nullptr && !offeredMatches(xfc->src, queueItem->pendingdata.availdatainfo.dataVer)) {
        // The tag finished data that was replaced while it was downloading. The queued item has not been
        // delivered: keep it, and announce it again so the tag gets it on its next check-in.
        Serial.printf("xfer complete for replaced data, keeping the current item\r\n");
        wsLog("xfer complete for replaced data, current item kept");
        checkQueue(xfc->src);
        return;
    }
    if (queueItem != nullptr) {
        // A completed deep-sleep command means the tag acknowledged it and is now
        // going to sleep. Flag it so we can mark the tag "in deep sleep" below;
        // this is deterministic, unlike the check-in based sentinel in
        // processDataReq which the delivering check-in can race and reset.
        if (queueItem->pendingdata.availdatainfo.dataType == DATATYPE_COMMAND_DATA &&
            queueItem->pendingdata.availdatainfo.dataTypeArgument == CMD_DO_DEEPSLEEP) {
            wasDeepSleepCmd = true;
        }
        // a resent /current/<mac>.raw is already in place: don't remove it
        const bool isCurrentImage = strcmp(queueItem->filename, dst_path) == 0;
        if (!isCurrentImage && contentFS->exists(dst_path) && contentFS->exists(queueItem->filename)) {
            contentFS->remove(dst_path);
        }
        if (!isCurrentImage && contentFS->exists(queueItem->filename)) {
            uint8_t dataType = queueItem->pendingdata.availdatainfo.dataType;
            if (config.preview && dataType != DATATYPE_FW_UPDATE && dataType != DATATYPE_NOUPDATE) {
                contentFS->rename(queueItem->filename, String(dst_path));
                }
            else {
                if (queueItem->pendingdata.availdatainfo.dataType != DATATYPE_FW_UPDATE) contentFS->remove(queueItem->filename);
            }
        }
        memcpy(md5bytes, &queueItem->pendingdata.availdatainfo.dataVer, sizeof(uint64_t));
        memset(md5bytes + sizeof(uint64_t), 0, 16 - sizeof(uint64_t));
        haveMd5 = true;
        dequeueItem(xfc->src);
    }

    tagRecord* taginfo = tagRecord::findByMAC(xfc->src);
    if (taginfo != nullptr) {
        clearPending(taginfo);
        if (haveMd5) memcpy(taginfo->md5, md5bytes, sizeof(md5bytes));
        taginfo->updateCount++;
        taginfo->updateLast = now;
        taginfo->pendingCount = countQueueItem(xfc->src);
        taginfo->wakeupReason = 0;
        if (wasDeepSleepCmd) {
            // Show "in deep sleep" until the tag checks in again (which
            // processDataReq treats as a wake-up and clears).
            taginfo->expectedNextCheckin = 3216153600;
        }
        if (taginfo->contentMode == 12 && local == false) {
            if (contentFS->exists(dst_path)) {
                contentFS->remove(dst_path);
            }
        }
        if (taginfo->contentMode == 5 || taginfo->contentMode == 17 || taginfo->contentMode == 18) {
            popTagInfo(xfc->src);
            taginfo = tagRecord::findByMAC(xfc->src);
            taginfo->nextupdate = now;
        }
    }

    // more in the queue?
    if (local) checkQueue(xfc->src);

    wsSendTaginfo(xfc->src, SYNC_TAGSTATUS);
    if (local) udpsync.netProcessXferComplete(xfc);
}

void processXferTimeout(struct espXferComplete* xfc, bool local) {
    if (config.runStatus == RUNSTATUS_STOP) {
        return;
    }
    char buffer[64];
    sprintf(buffer, "< %02X%02X%02X%02X%02X%02X%02X%02X xfer timeout\r\n\0", xfc->src[7], xfc->src[6], xfc->src[5], xfc->src[4], xfc->src[3], xfc->src[2], xfc->src[1], xfc->src[0]);
    wsErr((String)buffer);

    if (local) {
        Serial.printf("<XTO %02X%02X%02X%02X%02X%02X%02X%02X\r\n", xfc->src[7], xfc->src[6], xfc->src[5], xfc->src[4], xfc->src[3], xfc->src[2], xfc->src[1], xfc->src[0]);
    } else {
        Serial.printf("<REMOTE XTO %02X%02X%02X%02X%02X%02X%02X%02X\r\n", xfc->src[7], xfc->src[6], xfc->src[5], xfc->src[4], xfc->src[3], xfc->src[2], xfc->src[1], xfc->src[0]);
    }

    time_t now;
    time(&now);
    if (local) setAnnounced(xfc->src, nullptr);  // the radio dropped its slot for this tag
    tagRecord* taginfo = tagRecord::findByMAC(xfc->src);
    if (taginfo != nullptr) {
        taginfo->pendingIdle = 60;
        clearPending(taginfo);
    }
    dequeueAllItems(xfc->src);
    if (taginfo != nullptr) taginfo->pendingCount = countQueueItem(xfc->src);

    checkQueue(xfc->src);

    wsSendTaginfo(xfc->src, SYNC_TAGSTATUS);
    if (local) udpsync.netProcessXferTimeout(xfc);
}

void processDataReq(struct espAvailDataReq* eadr, bool local, IPAddress remoteIP, CheckinVia via) {
    if (config.runStatus == RUNSTATUS_STOP) {
        return;
    }
    char buffer[64];

#ifdef HAS_BLE_WRITER
    // a BLE display added on this AP stays here while this AP has it (roaming rules in od_session)
    if (!local && bleFamily(eadr->src, 0) == BLE_FAMILY_OPENDISPLAY && odKeyAdded(eadr->src) && !odRemoteCheckin(eadr->src, remoteIP)) return;
#endif

    char hexmac[17];
    mac2hex(eadr->src, hexmac);

    tagRecord* taginfo = tagRecord::findByMAC(eadr->src);
    if (taginfo == nullptr) {
        if (config.lock == 1 || (config.lock == 2 && eadr->adr.wakeupReason != WAKEUP_REASON_FIRSTBOOT)) return;
#ifdef HAS_SUBGHZ
        if (apInfo.hasSubGhz && eadr->adr.currentChannel > 0 && eadr->adr.currentChannel == apInfo.SubGhzChannel) {
            // Empty intentionally
        } else
#endif
            if (local == true && eadr->adr.currentChannel > 0 && eadr->adr.currentChannel != apInfo.channel) {
            Serial.printf("Tag %s reports illegal channel %d\r\n", hexmac, eadr->adr.currentChannel);
            return;
        }
        taginfo = addRecord(eadr->src);
    }
    // Set before checkQueue() below, so the first check-in after a reboot is already announced.
    if (via != CHECKIN_VIA_OTHER) taginfo->lastHeardOnRadio = (via == CHECKIN_VIA_RADIO);
    time_t now;
    time(&now);

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

    uint32_t offeredSleep = 0;
    const bool offeredKnown = local && recordOffered(eadr->src, offeredSleep);
    // BLE displays don't get their instructions from the radio
    const BleFamily family = tagBleFamily(eadr->src, taginfo, eadr->adr.hwType);
    const bool bleTag = family != BLE_FAMILY_NONE;
    if (offeredKnown && !bleTag) {
        // The tag sleeps what the radio told it just now, counted down since it was announced (#466).
        // Without an instruction it checks in again after about 40 seconds.
        taginfo->expectedNextCheckin = now + (offeredSleep > 60 ? offeredSleep : 60);
    } else if (taginfo->pendingIdle == 0 || countQueueItem(eadr->src) > 0) {
        // A tag that checks in is awake right now. Raise the expected checkin to at
        // least now+60, and pull it back down from the "in deep sleep" sentinel
        // (3216153600): once a tag checks in again after a deep sleep it has woken
        // up, so it must no longer show as sleeping - independent of the checkin
        // interval. The marker is (re)applied below the moment a deep-sleep command
        // is actually delivered, after which the tag sleeps and stops checking in.
        if (taginfo->expectedNextCheckin < now + 60 || taginfo->expectedNextCheckin == 3216153600)
            taginfo->expectedNextCheckin = now + 60;
    } else {
        taginfo->expectedNextCheckin = now + taginfo->pendingIdle;
    }
    taginfo->pendingIdle = 0;
    taginfo->lastseen = now;

    if (eadr->adr.lastPacketRSSI != 0) {
        if (eadr->adr.wakeupReason >= 0xE0) {
            if (taginfo->pendingCount == 0) {
                taginfo->nextupdate = 0;
                memset(taginfo->md5, 0, sizeof(taginfo->md5));
            }

            if (local) {
                const char* reason = "";
                if (eadr->adr.wakeupReason == WAKEUP_REASON_FIRSTBOOT)
                    reason = "Booting";
                else if (eadr->adr.wakeupReason == WAKEUP_REASON_NETWORK_SCAN)
                    reason = "Network scan";
                else if (eadr->adr.wakeupReason == WAKEUP_REASON_WDT_RESET)
                    reason = "Watchdog reset";
                else if (eadr->adr.wakeupReason == WAKEUP_REASON_FAILED_OTA_FW)
                    reason = "Firmware update rejected";
                sprintf(buffer, "%02X%02X%02X%02X%02X%02X%02X%02X %s", eadr->src[7], eadr->src[6], eadr->src[5], eadr->src[4], eadr->src[3], eadr->src[2], eadr->src[1], eadr->src[0], reason);
                logLine(buffer);
            }
        }

        taginfo->LQI = eadr->adr.lastPacketLQI;
        taginfo->RSSI = eadr->adr.lastPacketRSSI;
        taginfo->temperature = eadr->adr.temperature;
        taginfo->batteryMv = eadr->adr.batteryMv;
        taginfo->wakeupReason = eadr->adr.wakeupReason;
        taginfo->capabilities = eadr->adr.capabilities;
        taginfo->currentChannel = eadr->adr.currentChannel;
        if (family == BLE_FAMILY_OPENDISPLAY) {
            // type and firmware version come from the tag's config, the advertisement has neither
            if (taginfo->hwType == 0) taginfo->hwType = HWTYPE_OPENDISPLAY_UNKNOWN;
        } else {
            taginfo->hwType = eadr->adr.hwType;
            taginfo->tagSoftwareVersion = eadr->adr.tagSoftwareVersion;
        }
    }
    if (local) {
        sprintf(buffer, "<ADR %02X%02X%02X%02X%02X%02X%02X%02X\r\n\0", eadr->src[7], eadr->src[6], eadr->src[5], eadr->src[4], eadr->src[3], eadr->src[2], eadr->src[1], eadr->src[0]);
        Serial.print(buffer);
        checkQueue(eadr->src);   // experiemental 3/26/25: redundant check

        // A deep-sleep command makes the tag sleep without ever reporting xfer
        // complete. The radio co-processor only drops its cached "data available"
        // on an xfer-complete or an explicit cancel, so it would keep re-delivering
        // the deepsleep on every future check-in - re-sleeping the tag the moment it
        // is woken. This check-in means the tag is listening and has just received
        // it, so: mark it "in deep sleep" (the deterministic moment of delivery),
        // remove it from our queue, and cancel it in the radio so it is delivered
        // exactly once. A later check-in is treated as a wake-up (above) and clears
        // the marker. BLE displays are excluded: their deep sleep is delivered and
        // marked by the BLE writer, not over the 802.15.4 radio.
        if (family != BLE_FAMILY_ATC) {
            PendingItem* sent = getQueueItem(eadr->src);
            if (sent != nullptr &&
                sent->pendingdata.availdatainfo.dataType == DATATYPE_COMMAND_DATA &&
                sent->pendingdata.availdatainfo.dataTypeArgument == CMD_DO_DEEPSLEEP) {
                dequeueItem(eadr->src, sent->pendingdata.availdatainfo.dataVer);
                taginfo->pendingCount = countQueueItem(eadr->src);
                taginfo->expectedNextCheckin = 3216153600;
                struct pendingData cancel = {0};
                memcpy(cancel.targetMac, eadr->src, 8);
                sendCancelPending(&cancel);
                setAnnounced(eadr->src, nullptr);
            }
        }
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
    sprintf(buffer, "<TRD %02X%02X%02X%02X%02X%02X%02X%02X\r\n", trd->src[7], trd->src[6], trd->src[5], trd->src[4], trd->src[3], trd->src[2], trd->src[1], trd->src[0]);
    wsLog((String)buffer);
    sprintf(buffer, "TRD Data: len=%d, type=%d, ver=0x%08X\r\n", payloadLength, trd->returnData.dataType, trd->returnData.dataVer);
    wsLog((String)buffer);

#ifndef SAVE_SPACE
    TagData::parse(trd->src, trd->returnData.dataType, trd->returnData.data, payloadLength);
#endif
}

void refreshAllPending() {
    for (tagRecord* taginfo : tagDBSnapshot()) {
        if (taginfo->pendingCount > 0 && taginfo->version == 0) {
            clearPending(taginfo);
            taginfo->nextupdate = 0;
            wsSendTaginfo(taginfo->mac, SYNC_TAGSTATUS);
        }
    }
};

void updateContent(const uint8_t* dst) {
    tagRecord* taginfo = tagRecord::findByMAC(dst);
    if (taginfo != nullptr) {
        clearPending(taginfo);
        memset(taginfo->md5, 0, sizeof(taginfo->md5));
        taginfo->nextupdate = 0;
        wsSendTaginfo(taginfo->mac, SYNC_TAGSTATUS);
    }
}

extern int16_t radioLedBrightness;
extern int16_t radioLock;

void setAPchannel() {
    // v2 radios dim their LEDs along, and apply the tag inventory lock
    bool bSendRadioLayer = config.led != radioLedBrightness || config.lock != radioLock;
    struct espSetChannelPower tmp;
    if (config.channel == 0) {
        // trigger channel autoselect
        UDPcomm udpsync;
        udpsync.getAPList();
    } else {
        if (curChannel.channel != config.channel) {
            curChannel.channel = config.channel;
            bSendRadioLayer = true;
        }
    }
#ifdef HAS_SUBGHZ
    if (curChannel.subghzchannel != config.subghzchannel) {
        curChannel.subghzchannel = config.subghzchannel;
        apInfo.SubGhzChannel = config.subghzchannel;
        bSendRadioLayer = true;
    }
#endif
    if (bSendRadioLayer) {
        tmp = curChannel;
        if (config.channel == 0) {
            tmp.channel = 0;  // don't set the 802.15.4 channel
        }
        sendChannelPower(&tmp);
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
    pending.attemptsLeft = MAX_XFER_ATTEMPTS;
    Serial.printf(">AP Segmented Data %02X%02X%02X%02X%02X%02X%02X%02X\r\n\0", dst[7], dst[6], dst[5], dst[4], dst[3], dst[2], dst[1], dst[0]);
    if (local) {
        return queueDataAvail(&pending, true);
    } else {
        queueDataAvail(&pending, false);
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
    pending.attemptsLeft = MAX_XFER_ATTEMPTS;
    Serial.printf(">SDA %02X%02X%02X%02X%02X%02X%02X%02X\r\n\0", dst[7], dst[6], dst[5], dst[4], dst[3], dst[2], dst[1], dst[0]);
    if (local) {
        return queueDataAvail(&pending, true);
    } else {
        queueDataAvail(&pending, false);
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
    pending.attemptsLeft = MAX_XFER_ATTEMPTS;
    Serial.printf(">Tag CMD %02X%02X%02X%02X%02X%02X%02X%02X\r\n\0", dst[7], dst[6], dst[5], dst[4], dst[3], dst[2], dst[1], dst[0]);

    tagRecord* taginfo = tagRecord::findByMAC(dst);
    if (taginfo != nullptr) {
        taginfo->pendingCount++;
        wsSendTaginfo(taginfo->mac, SYNC_TAGSTATUS);
    }

    if (local) {
        return queueDataAvail(&pending, true);
    } else {
        queueDataAvail(&pending, false);
        udpsync.netSendDataAvail(&pending);
        return true;
    }
}

bool sendTagMac(const uint8_t* dst, const uint64_t newmac, bool local) {
    struct pendingData pending = {0};
    memcpy(pending.targetMac, dst, 8);
    pending.availdatainfo.dataType = DATATYPE_COMMAND_DATA;
    pending.availdatainfo.dataTypeArgument = 0x23;
    pending.availdatainfo.nextCheckIn = 0;
    

    pending.availdatainfo.dataVer = newmac;
    pending.availdatainfo.dataSize = 0;

    pending.attemptsLeft = MAX_XFER_ATTEMPTS;
    Serial.printf(">Tag %02X%02X%02X%02X%02X%02X%02X%02X Mac set\r\n\0", dst[7], dst[6], dst[5], dst[4], dst[3], dst[2], dst[1], dst[0]);

    tagRecord* taginfo = tagRecord::findByMAC(dst);
    if (taginfo != nullptr) {
        taginfo->pendingCount++;
        wsSendTaginfo(taginfo->mac, SYNC_TAGSTATUS);
    }

    if (local) {
        return queueDataAvail(&pending, true);
    } else {
        queueDataAvail(&pending, false);
        udpsync.netSendDataAvail(&pending);
        return true;
    }
}

void updateTaginfoitem(struct TagInfo* taginfoitem, IPAddress remoteIP) {
    tagRecord* taginfo = tagRecord::findByMAC(taginfoitem->mac);

    if (taginfo == nullptr) {
        if (config.lock) return;
        taginfo = addRecord(taginfoitem->mac);
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
            taginfo->pendingCount = taginfoitem->pendingCount;
            taginfo->expectedNextCheckin = taginfoitem->expectedNextCheckin;
            taginfo->hwType = taginfoitem->hwType;
            taginfo->wakeupReason = taginfoitem->wakeupReason;
            taginfo->capabilities = taginfoitem->capabilities;
            taginfo->pendingIdle = taginfoitem->pendingIdle;
            // the image hash, for the preview, of a tag whose content another AP makes
            if (taginfo->contentMode == 12) memcpy(taginfo->md5, taginfoitem->md5, sizeof(taginfoitem->md5));
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
    for (tagRecord* taginfo2 : tagDBSnapshot()) {
        if (taginfo2->contentMode == 20 && taginfo2->version == 0) {
            JsonDocument doc;
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
                taginfo2->pendingCount++;
                taginfo2->nextupdate = 3216153600;

                struct pendingData pending2 = {0};
                memcpy(pending2.targetMac, taginfo2->mac, 8);
                pending2.availdatainfo.dataType = taginfo2->dataType;
                pending2.availdatainfo.dataVer = pending->availdatainfo.dataVer;
                pending2.availdatainfo.dataSize = taginfo2->len;
                pending2.availdatainfo.dataTypeArgument = pending->availdatainfo.dataTypeArgument;
                pending2.availdatainfo.nextCheckIn = pending->availdatainfo.nextCheckIn;
                pending2.attemptsLeft = pending->attemptsLeft;

                if (taginfo2->isExternal == false) {
                    queueDataAvail(&pending2, true);
                } else {
                    char dst_path[64];
                    sprintf(dst_path, "/current/%02X%02X%02X%02X%02X%02X%02X%02X_%lu.pending", taginfo2->mac[7], taginfo2->mac[6], taginfo2->mac[5], taginfo2->mac[4], taginfo2->mac[3], taginfo2->mac[2], taginfo2->mac[1], taginfo2->mac[0], millis() % 1000000);
                    xSemaphoreTake(fsMutex, portMAX_DELAY);
                    File file = contentFS->open(dst_path, "w");
                    if (file) {
                        file.write(taginfo2->data, taginfo2->len);
                        file.close();
                        xSemaphoreGive(fsMutex);
                        queueDataAvail(&pending2, false);
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

void enqueueItem(struct PendingItem& item) {
    std::lock_guard<std::recursive_mutex> lock(queueMutex);
    pendingQueue.push_back(new PendingItem(item));
}

/// @brief Number of queued and retired items that use this data buffer
static int dataUsers(const uint8_t* data) {
    int count = 0;
    for (const PendingItem* item : pendingQueue) {
        if (item->data == data) count++;
    }
    for (const RetiredItem& retired : retiredItems) {
        if (retired.item->data == data) count++;
    }
    return count;
}

/// @brief Move a removed item to the retired list (caller holds queueMutex)
static void retireItem(PendingItem* item) {
    retiredItems.push_back({item, millis()});
}

void freeRetiredQueueItems() {
    std::lock_guard<std::recursive_mutex> lock(queueMutex);
    for (size_t c = 0; c < retiredItems.size();) {
        if (millis() - retiredItems[c].retiredAt > RETIRED_ITEM_GRACE_MS) {
            PendingItem* item = retiredItems[c].item;
            retiredItems.erase(retiredItems.begin() + c);
            if (item->data != nullptr && dataUsers(item->data) == 0) free(item->data);
            delete item;
        } else {
            c++;
        }
    }
}

bool dequeueItem(const uint8_t* targetMac) {
    return dequeueItem(targetMac, 0);
}

bool dequeueItem(const uint8_t* targetMac, const uint64_t dataVer) {
    std::lock_guard<std::recursive_mutex> lock(queueMutex);
    auto it = std::find_if(pendingQueue.begin(), pendingQueue.end(),
                           [targetMac, dataVer](const PendingItem* item) {
                               bool macMatches = memcmp(item->pendingdata.targetMac, targetMac, sizeof(item->pendingdata.targetMac)) == 0;
                               bool dataVerMatches = (dataVer == 0) || (dataVer == item->pendingdata.availdatainfo.dataVer);
                               return macMatches && dataVerMatches;
                           });
    if (it != pendingQueue.end()) {
        PendingItem* item = *it;
        pendingQueue.erase(it);
        retireItem(item);
        return true;
    }
    return false;
}

/// @brief Leave only the .pending files that no queued item uses (mirrored tags share the file), and drop keep
/// @note Caller holds queueMutex
static void keepUnusedPendingFiles(std::vector<String>& files, const char* keep) {
    files.erase(std::remove_if(files.begin(), files.end(),
                               [keep](const String& filename) {
                                   if (!filename.endsWith(".pending") || (keep != nullptr && filename == keep)) return true;
                                   for (const PendingItem* item : pendingQueue) {
                                       if (filename == item->filename) return true;
                                   }
                                   return false;
                               }),
                files.end());
}

static void removeFiles(const std::vector<String>& files) {
    for (const String& filename : files) {
        xSemaphoreTake(fsMutex, portMAX_DELAY);
        contentFS->remove(filename);
        xSemaphoreGive(fsMutex);
    }
}

void dequeueAllItems(const uint8_t* targetMac) {
    // the .pending files go too: nothing removes them later, until the next boot
    std::vector<String> obsoleteFiles;
    {
        std::lock_guard<std::recursive_mutex> lock(queueMutex);
        for (size_t c = 0; c < pendingQueue.size();) {
            PendingItem* item = pendingQueue[c];
            if (memcmp(item->pendingdata.targetMac, targetMac, sizeof(item->pendingdata.targetMac)) == 0) {
                pendingQueue.erase(pendingQueue.begin() + c);
                retireItem(item);
                obsoleteFiles.push_back(item->filename);
            } else {
                c++;
            }
        }
        keepUnusedPendingFiles(obsoleteFiles, nullptr);
    }
    removeFiles(obsoleteFiles);
}

static size_t queueSize() {
    std::lock_guard<std::recursive_mutex> lock(queueMutex);
    return pendingQueue.size();
}

uint16_t countQueueItem(const uint8_t* targetMac) {
    std::lock_guard<std::recursive_mutex> lock(queueMutex);
    int count = std::count_if(pendingQueue.begin(), pendingQueue.end(),
                              [targetMac](const PendingItem* item) {
                                  return memcmp(item->pendingdata.targetMac, targetMac, sizeof(item->pendingdata.targetMac)) == 0;
                              });
    return count;
}

PendingItem* getQueueItem(const uint8_t* targetMac) {
    return getQueueItem(targetMac, 0);
}

PendingItem* getQueueItem(const uint8_t* targetMac, const uint64_t dataVer) {
    std::lock_guard<std::recursive_mutex> lock(queueMutex);
    auto it = std::find_if(pendingQueue.begin(), pendingQueue.end(),
                           [targetMac, dataVer](const PendingItem* item) {
                               bool macMatches = memcmp(item->pendingdata.targetMac, targetMac, sizeof(item->pendingdata.targetMac)) == 0;
                               bool dataVerMatches = (dataVer == 0) || (dataVer == item->pendingdata.availdatainfo.dataVer);
                               return macMatches && dataVerMatches;
                           });
    return it != pendingQueue.end() ? *it : nullptr;
}

bool loadQueueItemData(PendingItem* queueItem) {
    std::lock_guard<std::recursive_mutex> lock(queueMutex);
    if (queueItem->data != nullptr) return true;
    fs::File file = contentFS->open(queueItem->filename);
    if (!file) {
        Serial.print("No current file. " + String(queueItem->filename) + "\r\n");
        return false;
    }
    queueItem->data = getDataForFile(file);
    file.close();
    return queueItem->data != nullptr;
}

void checkQueue(const uint8_t* targetMac) {
    uint16_t queueCount;
    queueCount = countQueueItem(targetMac);
    if (queueCount > 0) {
        Serial.printf("queue: total %d elements\r\n", queueSize());
        PendingItem* queueItem = getQueueItem(targetMac);
        if (queueItem == nullptr) {
            return;
        }
        if (queueCount > 1) queueItem->pendingdata.availdatainfo.nextCheckIn = 5 | 0x8000;
        if (sendDataAvail(&queueItem->pendingdata)) setAnnounced(targetMac, &queueItem->pendingdata);
    }
}

/// @brief An image to show right away: not preloaded (preload bit 2 and special type bits 3-7 are zero)
static bool isPlainImage(const struct AvailDataInfo& adi) {
    return isImageDataType(adi.dataType) && (adi.dataTypeArgument & 0xFC) == 0;
}

bool queueDataAvail(struct pendingData* pending, bool local) {
    PendingItem newPending;
    newPending.pendingdata.availdatainfo = pending->availdatainfo;
    newPending.pendingdata.attemptsLeft = pending->attemptsLeft;
    std::copy(pending->targetMac, pending->targetMac + sizeof(pending->targetMac), newPending.pendingdata.targetMac);

    tagRecord* taginfo = tagRecord::findByMAC(pending->targetMac);

    if (taginfo == nullptr) {
        return false;
    }

    std::strcpy(newPending.filename, taginfo->filename.c_str());
    if (taginfo->data != nullptr) {
        // move data pointer
        newPending.data = taginfo->data;
        taginfo->data = nullptr;
    } else {
        newPending.data = nullptr;
        
        if (queueSize() < 5) {   // maximized to 5 to save some memory
            // optional: read data early, don't wait for block request.
            fs::File file = contentFS->open(newPending.filename);
            if (file) {
                newPending.data = getDataForFile(file);
                Serial.println("Reading file " + String(newPending.filename));
                file.close();
            } else {
                Serial.println("Warning: not found: " + String(newPending.filename));
            }
        }
    }
    newPending.len = taginfo->len;

    // A new image replaces the images that are still queued for this tag: showing an outdated image
    // first is pointless. Preloaded images, commands and firmware updates are never replaced.
    if (isPlainImage(pending->availdatainfo)) {
        std::vector<String> obsoleteFiles;
        {
            std::lock_guard<std::recursive_mutex> lock(queueMutex);
            for (size_t c = 0; c < pendingQueue.size();) {
                PendingItem* item = pendingQueue[c];
                if (memcmp(item->pendingdata.targetMac, pending->targetMac, sizeof(item->pendingdata.targetMac)) == 0 && isPlainImage(item->pendingdata.availdatainfo)) {
                    pendingQueue.erase(pendingQueue.begin() + c);
                    retireItem(item);
                    obsoleteFiles.push_back(item->filename);
                } else {
                    c++;
                }
            }
            keepUnusedPendingFiles(obsoleteFiles, newPending.filename);
        }
        removeFiles(obsoleteFiles);
    }

    enqueueItem(newPending);
    taginfo->pendingCount = countQueueItem(pending->targetMac);
    if (taginfo->pendingCount == 1) {
        Serial.printf("queue item added, first in line\r\n");
        // if (local) sendDataAvail(pending);
    } else {
        Serial.printf("queue item added, total %d elements\r\n", taginfo->pendingCount);
        // to do: notify C6 to shorten the checkin time for the current SDA
    }

    if (local) checkQueue(pending->targetMac);

    return true;
}
