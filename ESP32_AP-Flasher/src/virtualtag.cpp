#include "virtualtag.h"

#include <Arduino.h>
#include <cstring>
#include <mutex>
#include <vector>

#include "newproto.h"
#include "system.h"
#include "tag_db.h"
#include "web.h"

// Requests coming from the web (async_tcp) task, drained on the loop task.
struct VtagRequest {
    bool isCreate;
    uint8_t mac[8];
    uint8_t hwType;
    String alias;
    uint8_t wakeupReason;
};
static std::vector<VtagRequest> vtagRequests;
static std::mutex vtagReqMutex;

bool vtagIsVirtual(const uint8_t* mac) {
    if (mac == nullptr) return false;
    const tagRecord* taginfo = tagRecord::findByMAC(mac);
    return taginfo != nullptr && taginfo->isVirtual;
}

void vtagCreate(const uint8_t* mac, uint8_t hwType, const String& alias) {
    tagRecord* taginfo = tagRecord::findByMAC(mac);
    if (taginfo == nullptr) {
        taginfo = new tagRecord;
        memcpy(taginfo->mac, mac, sizeof(taginfo->mac));
        tagDB.push_back(taginfo);
    }

    taginfo->isVirtual = true;
    taginfo->isExternal = false;
    taginfo->hwType = hwType;
    if (alias.length() > 0) taginfo->alias = alias;

    // Pretend to be a fully featured tag so the UI offers LED/button controls.
    taginfo->capabilities = CAPABILITY_HAS_LED | CAPABILITY_HAS_WAKE_BUTTON | CAPABILITY_HAS_NFC;
    taginfo->batteryMv = 2800;
    taginfo->temperature = 21;
    taginfo->RSSI = -50;  // must be non-zero, otherwise content generation is skipped
    taginfo->LQI = 100;
    taginfo->currentChannel = 0;
    // keep tagSoftwareVersion at 0 so images are sent as plain RAW (no zlib/g5),
    // which the browser preview can always decode.
    taginfo->tagSoftwareVersion = 0;
    taginfo->pendingCount = 0;

    char hexmac[17];
    mac2hex(mac, hexmac);
    wsLog("Created virtual tag " + String(hexmac));

    // simulate first boot so default content gets drawn
    vtagEvent(mac, WAKEUP_REASON_FIRSTBOOT);
}

void vtagEvent(const uint8_t* mac, uint8_t wakeupReason) {
    tagRecord* taginfo = tagRecord::findByMAC(mac);
    if (taginfo == nullptr || !taginfo->isVirtual) return;

    struct espAvailDataReq eadr;
    memset(&eadr, 0, sizeof(eadr));
    memcpy(eadr.src, mac, sizeof(eadr.src));
    eadr.adr.lastPacketLQI = taginfo->LQI ? taginfo->LQI : 100;
    eadr.adr.lastPacketRSSI = taginfo->RSSI ? taginfo->RSSI : -50;
    eadr.adr.temperature = taginfo->temperature;
    eadr.adr.batteryMv = taginfo->batteryMv ? taginfo->batteryMv : 2800;
    eadr.adr.hwType = taginfo->hwType;
    eadr.adr.wakeupReason = wakeupReason;
    eadr.adr.capabilities = taginfo->capabilities;
    eadr.adr.tagSoftwareVersion = taginfo->tagSoftwareVersion;
    eadr.adr.currentChannel = 0;  // 0 = don't validate against AP channel
    eadr.adr.customMode = 0;

    processDataReq(&eadr, true);

    // For a button/GPIO/NFC press, force the content to be regenerated so the
    // simulated screen reacts (content modes can branch on wakeupReason).
    if (wakeupReason == WAKEUP_REASON_BUTTON1 || wakeupReason == WAKEUP_REASON_BUTTON2 ||
        wakeupReason == WAKEUP_REASON_BUTTON3 || wakeupReason == WAKEUP_REASON_GPIO ||
        wakeupReason == WAKEUP_REASON_NFC) {
        taginfo->nextupdate = 0;
    }
}

void vtagEnqueueCreate(const uint8_t* mac, uint8_t hwType, const String& alias) {
    VtagRequest r;
    r.isCreate = true;
    memcpy(r.mac, mac, sizeof(r.mac));
    r.hwType = hwType;
    r.alias = alias;
    r.wakeupReason = 0;
    std::lock_guard<std::mutex> lock(vtagReqMutex);
    vtagRequests.push_back(r);
}

void vtagEnqueueEvent(const uint8_t* mac, uint8_t wakeupReason) {
    VtagRequest r;
    r.isCreate = false;
    memcpy(r.mac, mac, sizeof(r.mac));
    r.hwType = 0;
    r.wakeupReason = wakeupReason;
    std::lock_guard<std::mutex> lock(vtagReqMutex);
    vtagRequests.push_back(r);
}

void vtagProcessPending() {
    std::vector<VtagRequest> local;
    {
        std::lock_guard<std::mutex> lock(vtagReqMutex);
        if (vtagRequests.empty()) return;
        local.swap(vtagRequests);
    }
    for (const VtagRequest& r : local) {
        if (r.isCreate) {
            vtagCreate(r.mac, r.hwType, r.alias);
        } else {
            vtagEvent(r.mac, r.wakeupReason);
        }
    }
}

#define VIRTUAL_CHECKIN_INTERVAL 60  // seconds, like a normal tag's wake interval

void vtagAutoCheckin() {
    if (config.runStatus != RUNSTATUS_RUN) return;
    time_t now;
    time(&now);
    for (tagRecord* taginfo : tagDB) {
        if (!taginfo->isVirtual || taginfo->version != 0) continue;
        if (now - static_cast<time_t>(taginfo->lastseen) >= VIRTUAL_CHECKIN_INTERVAL) {
            // processDataReq() inside vtagEvent() sets lastseen = now, so the next
            // automatic check-in happens one interval later.
            vtagEvent(taginfo->mac, WAKEUP_REASON_TIMED);
        }
    }
}

bool vtagHandleDataAvail(struct pendingData* pending) {
    // copy out everything we need first: dequeueItem()/processXferComplete()
    // below erase the queue entry that `pending` points into.
    uint8_t mac[8];
    memcpy(mac, pending->targetMac, sizeof(mac));
    const uint8_t dataType = pending->availdatainfo.dataType;

    if (dataType == DATATYPE_NOUPDATE) {
        // just a "go back to sleep" instruction, nothing to display
        return true;
    }

    if (dataType == DATATYPE_COMMAND_DATA) {
        const uint8_t cmd = pending->availdatainfo.dataTypeArgument;
        if (cmd == CMD_DO_LEDFLASH) {
            // the 12-byte ledFlash struct is packed into dataVer(8) + dataSize(4)
            uint8_t pattern[12];
            memcpy(pattern, &pending->availdatainfo.dataVer, 8);
            memcpy(pattern + 8, &pending->availdatainfo.dataSize, 4);
            wsSendVirtualLed(mac, pattern);
        }
        // other commands (reboot/scan/deepsleep/...) need no simulated action
        dequeueItem(mac);
        tagRecord* taginfo = tagRecord::findByMAC(mac);
        if (taginfo != nullptr) {
            taginfo->pendingCount = countQueueItem(mac);
            wsSendTaginfo(mac, SYNC_TAGSTATUS);
        }
        checkQueue(mac);  // drain any further queued items (e.g. a pending image)
        return true;
    }

    // image or other payload: pretend the tag downloaded and displayed it.
    // processXferComplete() finalizes the transfer: it renames the queued
    // .pending file to /current/<mac>.raw (when preview is enabled) so the
    // browser shows the new screen, updates the md5/hash, and clears pending.
    struct espXferComplete xfc;
    memset(&xfc, 0, sizeof(xfc));
    memcpy(xfc.src, mac, sizeof(xfc.src));
    processXferComplete(&xfc, true);
    return true;
}
