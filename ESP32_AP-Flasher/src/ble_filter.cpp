#ifdef HAS_BLE_WRITER
#include <Arduino.h>
#include <FS.h>

#include <map>

#include "BLEDevice.h"
#include "ble_filter.h"
#include "newproto.h"
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

uint8_t* Mirrorbuffer;

struct BleAdvDataStructV1 {
    uint16_t manu_id;  // 0x1337 for us
    uint8_t version;
    uint16_t hw_type;
    uint16_t fw_version;
    uint16_t capabilities;
    uint16_t battery_mv;
    uint8_t counter;
} __packed;
struct BleAdvDataStructV2 {
    uint16_t manu_id;  // 0x1337 for us
    uint8_t version;
    uint16_t hw_type;
    uint16_t fw_version;
    uint16_t capabilities;
    uint16_t battery_mv;
    int8_t temperature;
    uint8_t counter;
} __packed;
// Appended by tag firmware 134 and up, behind the v2 layout rather than behind a
// version byte of its own: the switch below ends in "not supported, return
// false", so a tag announcing version 3 would not lose a field on an AP that has
// not been updated - it would disappear from it altogether. Everything above is
// byte for byte v2, and CAPABILITY_ADV_EXTENDED says these are there.
struct BleAdvDataStructV2Ext {
    uint16_t img_crc;     // CRC-16/CCITT-FALSE over the 8 byte dataVer on the panel
    uint8_t wake_reason;  // event counter : 4 | reason : 4, see bleWakeEvent()
} __packed;

// The wake-up reason byte of such a tag, turned into a one-shot event.
//
// The tag cannot be acknowledged over an advertisement - there is no reply to
// one - so it leaves the reason standing until its next check-in clears it and
// moves a counter for each new event instead. Keeping the last byte per tag and
// reporting only a change makes that idempotent without a timeout: a tap is
// handled once however many advertisements carry it, two presses of the same
// button are still two events because the counter moved, and an event survives
// until it is seen rather than expiring while this AP was busy. The same thing
// odButtonEvent() does for an OpenDisplay's button bytes.
//
// The two halves of the byte are read differently, because they are different
// kinds of thing. A counter is an edge: without a previous value there is
// nothing to compare it against, so a tag seen for the first time reports no
// event - after a restart of this AP every button in range would otherwise
// look as if it had just been pressed. A reason code is a state the tag is in
// ("I booted and nobody has served me since"), and that is worth acting on the
// first time it is seen, which is exactly the case after a restart of this AP.
//
// A tag whose byte does not move is quiet either way. That costs one case: a
// tag that reboots twice without being served in between advertises 0x08 both
// times and the second reboot passes unreported - but the first one already
// set nextupdate to 0, so the content is on its way regardless.
//
// Runs in the BLE host task - keep it to the map lookup.
static uint8_t bleWakeEvent(const uint8_t mac[8], const uint8_t advByte) {
    static std::map<uint64_t, uint8_t> lastSeen;
    uint64_t key = 0;
    for (int i = 0; i < 8; i++) key = (key << 8) | mac[i];
    const auto it = lastSeen.find(key);
    const bool known = it != lastSeen.end();
    const bool same = known && it->second == advByte;
    lastSeen[key] = advByte;
    // Codes 0 to 6 are the values this protocol uses itself, so they pass
    // straight through; the four the protocol numbers from 0xE0 up do not fit
    // in a nibble and get one of their own. See wakeReasonAdvByte() in the tag.
    const uint8_t code = advByte & 0x0F;
    // 0x00 is a tag with nothing pending: the tag puts the counter back to
    // zero along with the reason, so this is the whole byte and not just the
    // low nibble of one.
    if (code == 0 || same) return 0;
    if (!known && code <= 6) return 0;
    switch (code) {
        case 7:
            return WAKEUP_REASON_FAILED_OTA_FW;
        case 8:
            return WAKEUP_REASON_FIRSTBOOT;
        case 9:
            return WAKEUP_REASON_NETWORK_SCAN;
        case 10:
            return WAKEUP_REASON_WDT_RESET;
        default:
            // 0 means the tag has nothing to report, which a changed counter
            // alone does not override: it reset the reason after a check-in.
            return code <= 6 ? code : 0;
    }
}

// OpenDisplay advertisement, after the company id: v1 (14 bytes) or the older format (11 bytes).
// The display type isn't in it; that needs the config, read over a connection.
static bool BLE_filter_add_opendisplay(BLEAdvertisedDevice& advertisedDevice, const uint8_t* manuData, int manuDatalen) {
    struct espAvailDataReq theAdvData;
    memset((uint8_t*)&theAdvData, 0x00, sizeof(espAvailDataReq));
    const uint8_t* p = manuData + 2;
    if (manuDatalen >= 16) {
        // 0-10 buttons/sensors, 11 temperature (C + 40) * 2, 12 + bit 0 of 13 battery in 10 mV,
        // 13 bit 1 rebooted, bit 2 connection requested, bits 4-7 loop counter
        theAdvData.adr.temperature = (int8_t)((p[11] + 1) / 2 - 40);
        theAdvData.adr.batteryMv = (p[12] | ((p[13] & 1) << 8)) * 10;
    } else {
        theAdvData.adr.batteryMv = p[7] | (p[8] << 8);
        theAdvData.adr.temperature = (int8_t)p[9];
    }
    theAdvData.adr.lastPacketRSSI = advertisedDevice.getRSSI();

    uint8_t macReversed[6];
    memcpy(&macReversed, (uint8_t*)advertisedDevice.getAddress().getNative(), 6);
    for (int i = 0; i < 6; i++) theAdvData.src[i] = macReversed[5 - i];
    theAdvData.src[6] = 0x46;  // marks the family, see bleFamily()
    theAdvData.src[7] = 0x24;
    odSetAddrType(theAdvData.src, advertisedDevice.getAddressType());
    if (manuDatalen >= 16) {
        odRebootFlag(theAdvData.src, p[13] & 0x02);
        theAdvData.adr.wakeupReason = odButtonEvent(theAdvData.src, p);
    }
    BLE_enqueue_data_req(&theAdvData);
    return true;
}

// Called for every advertisement of every BLE device around (the scan runs continuously):
// keep it quiet and quick
bool BLE_filter_add_device(BLEAdvertisedDevice advertisedDevice) {
    uint8_t payloadData[100];
    int payloadDatalen = advertisedDevice.getPayloadLength();
    if (payloadDatalen > sizeof(payloadData))
        payloadDatalen = sizeof(payloadData);  // Never copy more than the buffer holds (extended advertising can exceed 31 bytes)
    memcpy(&payloadData, (uint8_t*)advertisedDevice.getPayload(), payloadDatalen);

    if (advertisedDevice.haveManufacturerData()) {
        int manuDatalen = advertisedDevice.getManufacturerData().length();
        uint8_t manuData[100];
        if (manuDatalen > sizeof(manuData))
            return false;  // Manu data too big, could never happen but better make sure here
#if ESP_ARDUINO_VERSION_MAJOR == 2
        memcpy(&manuData, (uint8_t*)advertisedDevice.getManufacturerData().data(), manuDatalen);
#else
        // [Nic] suggested fix for arduino 3.x by copilot, but I cannot test it
        memcpy(&manuData, (uint8_t*)advertisedDevice.getManufacturerData().c_str(), manuDatalen);
#endif
        if (manuDatalen == 7 && manuData[0] == 0x53 && manuData[1] == 0x50) {  // Lets check for a Gicisky E-Paper display

            struct espAvailDataReq theAdvData;
            memset((uint8_t*)&theAdvData, 0x00, sizeof(espAvailDataReq));

            uint8_t macReversed[6];
            memcpy(&macReversed, (uint8_t*)advertisedDevice.getAddress().getNative(), 6);
            theAdvData.src[0] = macReversed[5];
            theAdvData.src[1] = macReversed[4];
            theAdvData.src[2] = macReversed[3];
            theAdvData.src[3] = macReversed[2];
            theAdvData.src[4] = macReversed[1];
            theAdvData.src[5] = macReversed[0];
            theAdvData.src[6] = manuData[2];  // We use this do find out what type of display we got for compression^^
            theAdvData.src[7] = manuData[6];
            theAdvData.adr.batteryMv = manuData[3] * 100;
            theAdvData.adr.lastPacketRSSI = advertisedDevice.getRSSI();
            theAdvData.adr.lastPacketLQI = advertisedDevice.getRSSI();
            theAdvData.adr.hwType = gicToOEPLtype(manuData[2]);
            theAdvData.adr.tagSoftwareVersion = manuData[4] << 8 | manuData[5];
            theAdvData.adr.capabilities = 0x00;

            BLE_enqueue_data_req(&theAdvData);
            return true;
        } else if (manuDatalen >= 13 && manuData[0] == 0x46 && manuData[1] == 0x24) {  // OpenDisplay, company id 0x2446
            return BLE_filter_add_opendisplay(advertisedDevice, manuData, manuDatalen);
        } else if (manuDatalen >= 3 && manuData[0] == 0x37 && manuData[1] == 0x13) {  // Lets check for a Gicisky E-Paper display
            struct espAvailDataReq theAdvData;
            memset((uint8_t*)&theAdvData, 0x00, sizeof(espAvailDataReq));
            uint8_t versionAdvData = manuData[2];
            struct BleAdvDataStructV2Ext extAdvData;
            bool haveExtAdvData = false;

            switch (versionAdvData) {
                case 1: {
                    if (manuDatalen >= sizeof(BleAdvDataStructV1)) {
                        struct BleAdvDataStructV1 inAdvData;
                        memcpy(&inAdvData, manuData, sizeof(BleAdvDataStructV1));
                        /*Serial.printf("manu_id %04X\r\n", inAdvData.manu_id);
                        Serial.printf("version %02X\r\n", inAdvData.version);
                        Serial.printf("hw_type %04X\r\n", inAdvData.hw_type);
                        Serial.printf("fw_version %04X\r\n", inAdvData.fw_version);
                        Serial.printf("capabilities %04X\r\n", inAdvData.capabilities);
                        Serial.printf("battery_mv %u\r\n", inAdvData.battery_mv);
                        Serial.printf("counter %u\r\n", inAdvData.counter);*/
                        theAdvData.adr.batteryMv = inAdvData.battery_mv;
                        theAdvData.adr.lastPacketRSSI = advertisedDevice.getRSSI();
                        theAdvData.adr.hwType = inAdvData.hw_type & 0xff;
                        theAdvData.adr.tagSoftwareVersion = inAdvData.fw_version;
                        theAdvData.adr.capabilities = inAdvData.capabilities & 0xff;
                    } else {
                        printf("Version 1 data length incorrect!\r\n");
                        return false;
                    }
                } break;
                case 2: {
                    if (manuDatalen >= sizeof(BleAdvDataStructV2)) {
                        struct BleAdvDataStructV2 inAdvData;
                        memcpy(&inAdvData, manuData, sizeof(BleAdvDataStructV2));
                        /*Serial.printf("manu_id %04X\r\n", inAdvData.manu_id);
                        Serial.printf("version %02X\r\n", inAdvData.version);
                        Serial.printf("hw_type %04X\r\n", inAdvData.hw_type);
                        Serial.printf("fw_version %04X\r\n", inAdvData.fw_version);
                        Serial.printf("capabilities %04X\r\n", inAdvData.capabilities);
                        Serial.printf("battery_mv %u\r\n", inAdvData.battery_mv);
                        Serial.printf("temperature %i\r\n", inAdvData.temperature);
                        Serial.printf("counter %u\r\n", inAdvData.counter);*/
                        theAdvData.adr.batteryMv = inAdvData.battery_mv;
                        theAdvData.adr.temperature = inAdvData.temperature;
                        theAdvData.adr.lastPacketRSSI = advertisedDevice.getRSSI();
                        theAdvData.adr.hwType = inAdvData.hw_type & 0xff;
                        theAdvData.adr.tagSoftwareVersion = inAdvData.fw_version;
                        theAdvData.adr.capabilities = inAdvData.capabilities & 0xff;
                        // The full 16 bits here, not the byte stored above: the
                        // flag for the extra fields lives in the high one.
                        if ((inAdvData.capabilities & CAPABILITY_ADV_EXTENDED) &&
                            manuDatalen >= (int)(sizeof(BleAdvDataStructV2) + sizeof(BleAdvDataStructV2Ext))) {
                            memcpy(&extAdvData, manuData + sizeof(BleAdvDataStructV2), sizeof(BleAdvDataStructV2Ext));
                            haveExtAdvData = true;
                        }
                    } else {
                        printf("Version 2 data length incorrect!\r\n");
                        return false;
                    }
                } break;
                default:
                    printf("Version %02X currently not supported!\r\n", versionAdvData);
                    return false;
                    break;
            }
            uint8_t macReversed[6];
            memcpy(&macReversed, (uint8_t*)advertisedDevice.getAddress().getNative(), 6);
            theAdvData.src[0] = macReversed[5];
            theAdvData.src[1] = macReversed[4];
            theAdvData.src[2] = macReversed[3];
            theAdvData.src[3] = macReversed[2];
            theAdvData.src[4] = macReversed[1];
            theAdvData.src[5] = macReversed[0];
            theAdvData.src[6] = manuData[0];  // We use this do find out what type of display we got for compression^^
            theAdvData.src[7] = manuData[1];
            if (haveExtAdvData) {
                // Handed on in the reserved bytes rather than compared here: the
                // comparison needs the tag's record, and this runs in the BLE
                // host task. See the note on AvailDataReq::reserved.
                theAdvData.adr.reserved[0] = (uint8_t)(extAdvData.img_crc & 0xff);
                theAdvData.adr.reserved[1] = (uint8_t)(extAdvData.img_crc >> 8);
                theAdvData.adr.reserved[2] = 1;
                // Set before the enqueue on purpose: BLE_enqueue_data_req()
                // passes on one advertisement per tag per minute, except when
                // there is a reason, which it sends through at once.
                theAdvData.adr.wakeupReason = bleWakeEvent(theAdvData.src, extAdvData.wake_reason);
            }
            BLE_enqueue_data_req(&theAdvData);
            return true;
        }
    }
    if (payloadDatalen >= 17) {  // Lets check for an ATC Mi Thermometer
        uint8_t macReversed[6];
        memcpy(&macReversed, (uint8_t*)advertisedDevice.getAddress().getNative(), 6);
        if (payloadData[9] == macReversed[5] && payloadData[8] == macReversed[4] && payloadData[7] == macReversed[3]) {  // Here we found an ATC Mi Thermometer
            struct espAvailDataReq theAdvData;
            memset((uint8_t*)&theAdvData, 0x00, sizeof(espAvailDataReq));

            theAdvData.src[0] = macReversed[5];
            theAdvData.src[1] = macReversed[4];
            theAdvData.src[2] = macReversed[3];
            theAdvData.src[3] = macReversed[2];
            theAdvData.src[4] = macReversed[1];
            theAdvData.src[5] = macReversed[0];
            theAdvData.src[6] = 0x00;
            theAdvData.src[7] = 0x00;
            theAdvData.adr.batteryMv = payloadData[14] << 8 | payloadData[15];
            theAdvData.adr.temperature = (payloadData[10] << 8 | payloadData[11]) / 10;
            theAdvData.adr.lastPacketLQI = payloadData[12];
            theAdvData.adr.lastPacketRSSI = advertisedDevice.getRSSI();
            theAdvData.adr.hwType = ATC_MI_THERMOMETER;
            theAdvData.adr.tagSoftwareVersion = 0x00;
            theAdvData.adr.capabilities = 0x00;
            BLE_enqueue_data_req(&theAdvData);
            return true;
        }
    }
    return false;
}

// Remembers how often we already tried to reach each BLE tag (keyed by MAC).
// Used to spread attempts evenly instead of getting stuck on the first (maybe
// offline) tag forever.
static std::map<uint64_t, uint32_t> BLE_attemptCounts;
static std::map<uint64_t, uint32_t> BLE_attemptLast;  // millis() of the last try
#define BLE_RETRY_WAIT_MS 30000

static uint64_t BLE_macToKey(const uint8_t mac[8]) {
    uint64_t key = 0;
    for (int i = 0; i < 8; i++) key = (key << 8) | mac[i];
    return key;
}

static bool BLE_isPendingCandidate(tagRecord* taginfo) {
    if (taginfo->pendingCount == 0 || taginfo->version != 0) return false;
    // pendingCount goes up just before the item is queued; the image is ready once it is there
    if (getQueueItem(taginfo->mac) == nullptr) return false;
    // after a failed try, wait a bit: the pending check runs every second
    auto last = BLE_attemptLast.find(BLE_macToKey(taginfo->mac));
    if (last != BLE_attemptLast.end() && millis() - last->second < BLE_RETRY_WAIT_MS) return false;
    const BleFamily family = tagBleFamily(taginfo->mac, taginfo);
    // an OpenDisplay tag only once its type, and so its config, is known, while it is added, and
    // only on the AP that has it now (the AP with its content queues the image too)
    if (family == BLE_FAMILY_OPENDISPLAY)
        return taginfo->hwType != HWTYPE_OPENDISPLAY_UNKNOWN && !taginfo->isExternal && odKeyAdded(taginfo->mac) && odIsOwner(taginfo->mac);
    return family == BLE_FAMILY_GICISKY || family == BLE_FAMILY_ATC;
}

// Call once an upload finished (success or given up) so a later re-add of the
// same tag starts fresh and isn't unfairly deprioritized.
void BLE_clear_attempts(uint8_t address[8]) {
    BLE_attemptCounts.erase(BLE_macToKey(address));
    BLE_attemptLast.erase(BLE_macToKey(address));
}

uint32_t BLE_get_attempts(uint8_t address[8]) {
    auto it = BLE_attemptCounts.find(BLE_macToKey(address));
    return it == BLE_attemptCounts.end() ? 0 : it->second;
}

bool BLE_is_image_pending(uint8_t address[8]) {
    // Pick the pending BLE tag we have tried the fewest times. This way an
    // unreachable display no longer blocks the queue: every attempt rotates to
    // the next-least-tried tag, so all reachable uploads get their turn and
    // finish even while some tags stay offline.
    tagRecord* best = nullptr;
    uint32_t bestAttempts = 0;
    for (tagRecord* taginfo : tagDBSnapshot()) {
        if (!BLE_isPendingCandidate(taginfo)) continue;
        uint32_t attempts = BLE_attemptCounts[BLE_macToKey(taginfo->mac)];
        if (best == nullptr || attempts < bestAttempts) {
            best = taginfo;
            bestAttempts = attempts;
        }
    }
    if (best == nullptr) return false;

    memcpy(address, best->mac, 8);
    BLE_attemptCounts[BLE_macToKey(best->mac)]++;  // count this attempt so the next round prefers the others
    BLE_attemptLast[BLE_macToKey(best->mac)] = millis();
    if (bleFamily(best->mac, best->hwType) == BLE_FAMILY_ATC) {
        Serial.printf("ATC BLE OEPL data Waiting\r\n");
    }
    return true;
}

uint8_t swapBits(uint8_t num) {
    uint8_t result = 0;
    for (int i = 0; i < 8; ++i) {
        result |= ((num >> i) & 0x01) << (7 - i);
    }
    return result;
}

// Packed 2-bit palette data is already in its final on-air representation and
// must bypass the older Gicisky bit-plane compressor.
static uint32_t copy_packed_2bit_image(uint8_t* buffer, uint8_t* src, uint32_t src_len,
                                       uint16_t width_display, uint16_t height_display,
                                       uint32_t max_len) {
    const uint32_t output_len = ((uint32_t)width_display * height_display + 3) / 4;
    if (src_len != output_len || output_len > max_len) {
        Serial.printf("BLE invalid packed 2bpp frame: src=%u expected=%u max=%u\r\n",
                      src_len, output_len, max_len);
        return 0;
    }

    memcpy(buffer, src, output_len);
    Serial.printf("BLE direct packed 2bpp frame: %u bytes\r\n", output_len);
    return output_len;
}

uint32_t compress_image(uint8_t address[8], uint8_t* buffer, uint32_t max_len, uint8_t imageBpp) {
    uint32_t t = millis();
    PendingItem* queueItem = getQueueItem(address, 0);
    if (queueItem == nullptr) {
        prepareCancelPending(address);
        Serial.printf("blockrequest: couldn't find taginfo %02X%02X%02X%02X%02X%02X%02X%02X\r\n", address[7], address[6], address[5], address[4], address[3], address[2], address[1], address[0]);
        return 0;
    }
    if (!loadQueueItemData(queueItem)) {
        Serial.print("No data for " + String(queueItem->filename) + ", canceling request\r\n");
        prepareCancelPending(address);
        return 0;
    }

    uint16_t giciType = (address[7] << 8) | address[6];  // here we "extract" the display info again

    uint8_t screenResolution = (giciType >> 5) & 63;
    uint8_t dispPtype = (giciType >> 3) & 3;
    uint8_t availColors = ((giciType >> 1) & 3);
    uint8_t special_color = ((giciType >> 10) & 12);
    uint8_t singleDoubleMirror = giciType & 1;
    uint8_t canDoCompression = (giciType & 0x4000) ? 0 : 1;

    bool extra_color = false;
    bool extra_tft_bitshifting = false;
    bool mirror_width = false;
    uint16_t width_display = 104;
    uint16_t height_display = 212;

    switch (screenResolution) {
        case 0:
            width_display = 216;
            height_display = 104;
            break;
        case 1:
            width_display = 296;
            height_display = 128;
            break;
        case 2:
            width_display = 300;
            height_display = 400;
            break;
        case 3:
            width_display = 640;
            height_display = 384;
            break;
        case 4:
            width_display = 960;
            height_display = 640;
            break;
        case 5:
            width_display = 250;
            height_display = 136;
            break;
        case 6:
            width_display = 196;
            height_display = 96;
            break;
        case 7:
            width_display = 640;
            height_display = 480;
            break;
        case 8:
            width_display = 250;
            height_display = 128;
            break;
        case 9:
            width_display = 800;
            height_display = 480;
            break;
        case 10:
            width_display = 280;
            height_display = 480;
            break;
    }

    switch (dispPtype) {
        case 0:  // TFT
            mirror_width = false;
            extra_tft_bitshifting = true;  // Special case for the TFT Type
            break;
        case 1:  // EPA
            mirror_width = false;
            break;
        case 2:  // EPA1
            mirror_width = false;
            break;
        case 3:  // EPA2
            mirror_width = false;
            break;
    }

    if (giciType & 0x100)  // Some special case, needs to be tested if always correct
        mirror_width = true;

    switch (availColors) {
        case 0:  // BW
            extra_color = false;
            break;
        case 1:  // BWR
            extra_color = true;
            break;
        case 2:  // BWY
            extra_color = true;
            break;
        case 3:  // BWRY
            extra_color = true;
            break;
        case 4:  // BWRGBYO
            extra_color = true;
            break;
    }
    switch (singleDoubleMirror) {
        case 0:  // Single image
            break;
        case 1:  // 2 Images
            break;
    }

    if (imageBpp == BPP_PACKED_2BIT) {
        return copy_packed_2bit_image(buffer, queueItem->data, queueItem->len,
                                      width_display, height_display, max_len);
    }

    uint32_t len_compressed = 0;
    if (canDoCompression)
        len_compressed = 4;
    uint32_t curr_input_posi = 0;
    uint32_t byte_per_line = (height_display / 8);
    if (height_display % 8 != 0)
        byte_per_line++;
    Mirrorbuffer = (uint8_t*)malloc(byte_per_line + 1);
    if (Mirrorbuffer == nullptr) {
        Serial.println("BLE Could not create Mirrorbuffer!");
        return 0;
    }
    Serial.printf("BLE Filter options:\r\n");
    Serial.printf("screenResolution %d\r\n", screenResolution);
    Serial.printf("dispPtype %d\r\n", dispPtype);
    Serial.printf("availColors %d\r\n", availColors);
    Serial.printf("special_color %d\r\n", special_color);
    Serial.printf("singleDoubleMirror %d\r\n", singleDoubleMirror);
    Serial.printf("canDoCompression %d\r\n", canDoCompression);
    Serial.printf("byte_per_line %d\r\n", byte_per_line);
    Serial.printf("width_display %d\r\n", width_display);
    Serial.printf("height_display %d\r\n", height_display);
    Serial.printf("mirror_width %d\r\n", mirror_width);
    for (int i = 0; i < width_display; i++) {
        if (canDoCompression) {
            buffer[len_compressed++] = 0x75;
            buffer[len_compressed++] = byte_per_line + 7;
            buffer[len_compressed++] = byte_per_line;
            buffer[len_compressed++] = 0x00;
            buffer[len_compressed++] = 0x00;
            buffer[len_compressed++] = 0x00;
            buffer[len_compressed++] = 0x00;
        }
        if (mirror_width) {
            for (int b = 0; b < byte_per_line; b++) {
                Mirrorbuffer[b] = ~queueItem->data[curr_input_posi++];
            }
            for (int b = byte_per_line - 1; b >= 0; b--) {
                buffer[len_compressed++] = swapBits(Mirrorbuffer[b]);
            }
        } else {
            for (int b = 0; b < byte_per_line; b++) {
                buffer[len_compressed++] = ~queueItem->data[curr_input_posi++];
            }
        }
    }
    if (extra_color) {
        for (int i = 0; i < width_display; i++) {
            if (canDoCompression) {
                buffer[len_compressed++] = 0x75;
                buffer[len_compressed++] = byte_per_line + 7;
                buffer[len_compressed++] = byte_per_line;
                buffer[len_compressed++] = 0x00;
                buffer[len_compressed++] = 0x00;
                buffer[len_compressed++] = 0x00;
                buffer[len_compressed++] = 0x00;
            }
            if (mirror_width) {
                for (int b = 0; b < byte_per_line; b++) {
                    if (queueItem->len <= curr_input_posi)
                        Mirrorbuffer[b] = 0x00;  // Do not anything outside of the buffer!
                    else
                        Mirrorbuffer[b] = queueItem->data[curr_input_posi++];
                }
                for (int b = byte_per_line - 1; b >= 0; b--) {
                    buffer[len_compressed++] = swapBits(Mirrorbuffer[b]);
                }
            } else {
                for (int b = 0; b < byte_per_line; b++) {
                    if (queueItem->len <= curr_input_posi) {
                        buffer[len_compressed++] = 0x00;  // Do not anything outside of the buffer!
                    } else {
                        buffer[len_compressed++] = queueItem->data[curr_input_posi++];
                    }
                }
            }
        }
    }
    if (canDoCompression) {
        buffer[0] = len_compressed & 0xff;
        buffer[1] = (len_compressed >> 8) & 0xff;
        buffer[2] = (len_compressed >> 16) & 0xff;
        buffer[3] = (len_compressed >> 24) & 0xff;
    }
    free(Mirrorbuffer);
    return len_compressed;
}

// How many bytes the item queued for this display will need, 0 when there is
// none. The caller allocates by this instead of by one fixed size: a firmware
// is larger than any image, and an image is a fraction of what a firmware
// needs, so a single constant is either too small for the one or wasteful for
// the other. It was too small - 135000 against a 160900 byte firmware.
uint32_t get_ATC_BLE_OEPL_pending_len(uint8_t address[8]) {
    PendingItem* queueItem = getQueueItem(address, 0);
    if (queueItem == nullptr) return 0;
    return queueItem->len;
}

uint32_t get_ATC_BLE_OEPL_image(uint8_t address[8], uint8_t* buffer, uint32_t max_len, uint8_t* dataType, uint8_t* dataTypeArgument, uint16_t* nextCheckIn, uint64_t* dataVer) {
    uint32_t t = millis();
    PendingItem* queueItem = getQueueItem(address, 0);
    if (queueItem == nullptr) {
        prepareCancelPending(address);
        Serial.printf("blockrequest: couldn't find taginfo %02X%02X%02X%02X%02X%02X%02X%02X\r\n", address[7], address[6], address[5], address[4], address[3], address[2], address[1], address[0]);
        return 0;
    }
    // Neither of these cancels the item any more. prepareCancelPending() empties
    // the queue, and both causes are about this attempt rather than about the
    // content: the file may appear, the memory may free up. Left queued, the
    // writer's own attempt counter ends in a visible "xfer timeout" after five
    // tries instead of a blue flash in the web UI and nothing else.
    if (!loadQueueItemData(queueItem)) {
        char hexmac[17];
        mac2hex(address, hexmac);
        wsErr(String(hexmac) + " BLE: no data for " + String(queueItem->filename));
        return 0;
    }
    // Not a size policy - that belongs to the display, which refuses what it
    // cannot store - but the bound on the memcpy below. The caller sizes the
    // buffer to this very item, so this only fires if those two ever disagree.
    if (queueItem->len > max_len) {
        char hexmac[17];
        mac2hex(address, hexmac);
        wsErr(String(hexmac) + " BLE: " + String(queueItem->len) + " bytes do not fit in a " +
              String(max_len) + " byte buffer");
        return 0;
    }
    *dataType = queueItem->pendingdata.availdatainfo.dataType;
    *dataTypeArgument = queueItem->pendingdata.availdatainfo.dataTypeArgument;
    *nextCheckIn = queueItem->pendingdata.availdatainfo.nextCheckIn;
    // The item's own version, rather than one computed again from the buffer.
    // prepareDataAvail() took it as the md5 of the whole file; recomputing it in
    // the writer went through MD5Builder::add(), whose length is a uint16_t, so
    // a 160900 byte firmware was hashed over its first 29828 bytes and the
    // AvailDataInfo carried a version that matched nothing. One source for it,
    // and the delivery report can be checked against the item it belongs to.
    *dataVer = queueItem->pendingdata.availdatainfo.dataVer;
    uint32_t len_compressed = queueItem->len;
    memcpy(buffer, queueItem->data, queueItem->len);
    Serial.print("Data is prepared Len: " + String(queueItem->len) + "\r\n");
    return queueItem->len;
}

#define BLE_CMD_RESET_TO_DEFAULT 10
#define BLE_CMD_SET_CUSTOM_MAC 9
#define BLE_CMD_DEEPSLEEP 20
#define BLE_CMD_SET_LED 21
#define BLE_CMD_REBOOT 23
#define BLE_CMD_AVAILDATA 100

bool get_ATC_BLE_OEPL_command(uint8_t address[8], uint8_t* frame, uint8_t* frameLen, bool* isDeepsleep) {
    *frameLen = 0;
    *isDeepsleep = false;

    PendingItem* queueItem = getQueueItem(address, 0);
    if (queueItem == nullptr) return false;

    struct AvailDataInfo* adi = &queueItem->pendingdata.availdatainfo;
    if (adi->dataType != DATATYPE_COMMAND_DATA) return false;  // an actual image, let the upload path handle it

    uint8_t arg = adi->dataTypeArgument;

    // A screen-type change (argument >= 0xA0) is understood by the display through
    // the normal AVAILDATA / COMMAND_DATA path, so forward the availdatainfo as-is.
    if (arg >= 0xA0) {
        frame[0] = 0x00;
        frame[1] = BLE_CMD_AVAILDATA;
        memcpy(&frame[2], adi, sizeof(struct AvailDataInfo));
        *frameLen = 2 + sizeof(struct AvailDataInfo);
        return true;
    }

    uint16_t bleCmd = 0;
    switch (arg) {
        case CMD_DO_REBOOT:
            bleCmd = BLE_CMD_REBOOT;
            break;
        case CMD_DO_DEEPSLEEP:
            bleCmd = BLE_CMD_DEEPSLEEP;
            *isDeepsleep = true;
            break;
        case CMD_DO_RESET_SETTINGS:
            // The display requires the 0x1234 confirmation payload for a reset.
            frame[0] = 0x00;
            frame[1] = BLE_CMD_RESET_TO_DEFAULT;
            frame[2] = 0x12;
            frame[3] = 0x34;
            *frameLen = 4;
            return true;
        case CMD_DO_LEDFLASH:
            // The 12 LED-pattern bytes are stored in dataVer (8) + dataSize (4).
            frame[0] = 0x00;
            frame[1] = BLE_CMD_SET_LED;
            memcpy(&frame[2], &adi->dataVer, 8);
            memcpy(&frame[2 + 8], &adi->dataSize, 4);
            *frameLen = 2 + 12;
            return true;
        case 0x23:  // set custom MAC (sendTagMac stores the new MAC in dataVer)
            frame[0] = 0x00;
            frame[1] = BLE_CMD_SET_CUSTOM_MAC;
            memcpy(&frame[2], &adi->dataVer, 8);
            *frameLen = 2 + 8;
            return true;
        case CMD_DO_SCAN:
        default:
            // No sensible BLE equivalent (a scan targets the 802.15.4 radio).
            // Report it as a command so the caller acknowledges/clears it and the
            // queue does not get stuck retrying forever.
            *frameLen = 0;
            return true;
    }

    frame[0] = (bleCmd >> 8) & 0xff;
    frame[1] = bleCmd & 0xff;
    *frameLen = 2;
    return true;
}

#endif
