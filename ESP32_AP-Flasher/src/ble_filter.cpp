#ifdef HAS_BLE_WRITER
#include <Arduino.h>
#include <FS.h>

#include "BLEDevice.h"
#include "newproto.h"
#include "serialap.h"
#include "settings.h"
#include "storage.h"
#include "system.h"
#include "tag_db.h"
#include "tagdata.h"
#include "udp.h"
#include "util.h"
#include "web.h"

uint8_t gicToOEPLtype(uint8_t gicType) {
    switch (gicType) {
        case 0x08:
            return GICI_BLE_EPD_21_BW;
            break;
        case 0x0B:
            return GICI_BLE_EPD_21_BWR;
            break;
        case 0x28:
        case 0x30:
            return GICI_BLE_EPD_29_BW;
            break;
        case 0x2B:
            return GICI_BLE_EPD_29_BWR;
            break;
        case 0x33:
            return GICI_BLE_EPD_29_BWR1;
            break;
        case 0x48:
            return GICI_BLE_EPD_BW_42;
            break;
        case 0x4B:
            return GICI_BLE_EPD_BWR_42;
            break;
        case 0x40:
            return GICI_BLE_TFT_BW_42;
            break;
        case 0x42:
            return GICI_BLE_TFT_BWR_42;
            break;
        case 0x68:
            return GICI_BLE_EPD_BW_74;
            break;
        case 0x6A:
            return GICI_BLE_EPD_BWR_74;
            break;
        default:
            return GICI_BLE_UNKNOWN;  // Should never happen, return 1.54"
            break;
    }
}

bool BLE_filter_add_device(BLEAdvertisedDevice advertisedDevice) {
    if (advertisedDevice.haveManufacturerData()) {
        int manuDatalen = advertisedDevice.getManufacturerData().length();
        uint8_t manuData[100];
        if (manuDatalen > sizeof(manuData))
            return false;  // Manu data too big, could never happen but better make sure here
        memcpy(&manuData, (uint8_t*)advertisedDevice.getManufacturerData().data(), manuDatalen);
        if (manuDatalen == 7 && manuData[0] == 0x53 && manuData[1] == 0x50) {
            Serial.print("BLE Advertised Device found: ");
            Serial.println(advertisedDevice.toString().c_str());
            Serial.printf(" Address type: %02X Manu data: ", advertisedDevice.getAddressType());
            for (int i = 0; i < advertisedDevice.getManufacturerData().length(); i++)
                Serial.printf("%02X", manuData[i]);
            Serial.printf("\r\n");

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
            theAdvData.src[7] = 0x00;
            theAdvData.adr.batteryMv = manuData[3] * 100;
            theAdvData.adr.lastPacketRSSI = advertisedDevice.getRSSI();
            theAdvData.adr.hwType = gicToOEPLtype(manuData[2]);
            theAdvData.adr.tagSoftwareVersion = manuData[4] << 8 | manuData[5];
            theAdvData.adr.capabilities = 0x00;

            processDataReq(&theAdvData, true);
            return true;
        }
    }

    return false;
}

bool BLE_is_image_pending(uint8_t address[8]) {
    for (int16_t c = 0; c < tagDB.size(); c++) {
        tagRecord* taginfo = tagDB.at(c);
        if (taginfo->pendingCount > 0 && taginfo->version == 0 && (taginfo->hwType & 0xB0)) {
            memcpy(address, taginfo->mac, 8);
            return true;
        }
    }
    return false;
}

uint32_t compress_image(uint8_t address[8], uint8_t* buffer, uint32_t max_len) {
    uint32_t t = millis();
    PendingItem* queueItem = getQueueItem(address, 0);
    if (queueItem == nullptr) {
        prepareCancelPending(address);
        Serial.printf("blockrequest: couldn't find taginfo %02X%02X%02X%02X%02X%02X%02X%02X\n", address[7], address[6], address[5], address[4], address[3], address[2], address[1], address[0]);
        return 0;
    }
    if (queueItem->data == nullptr) {
        fs::File file = contentFS->open(queueItem->filename);
        if (!file) {
            Serial.print("No current file. " + String(queueItem->filename) + " Canceling request\n");
            prepareCancelPending(address);
            return 0;
        }
        queueItem->data = getDataForFile(file);
        Serial.println("Reading file " + String(queueItem->filename) + " in  " + String(millis() - t) + "ms");
        file.close();
    }

    uint8_t giciType = address[6];  // here we "extract" the display info again

    bool extra_color = false;
    uint16_t width_display = 104;
    uint16_t height_display = 212;
    switch ((giciType >> 5) & 7)  // Resolution
    {
        default:
        case 0:
            width_display = 104;
            height_display = 212;
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
            width_display = 384;
            height_display = 640;
            break;
    }
    switch ((giciType >> 1) & 3)  // Extra color
    {
        default:
        case 0:
            extra_color = false;
            break;
        case 1:
        case 2:
            extra_color = true;
            break;
    }

    // Yeah, this is no real compression, but maybe it will be solved in the future to the "real" RLE Compression
    uint32_t len_compressed = 4;
    uint32_t curr_input_posi = 0;
    uint32_t byte_per_line = (height_display / 8);
    for (int i = 0; i < width_display; i++) {
        buffer[len_compressed++] = 0x75;
        buffer[len_compressed++] = byte_per_line + 7;
        buffer[len_compressed++] = byte_per_line;
        buffer[len_compressed++] = 0x00;
        buffer[len_compressed++] = 0x00;
        buffer[len_compressed++] = 0x00;
        buffer[len_compressed++] = 0x00;
        for (int b = 0; b < byte_per_line; b++) {
            buffer[len_compressed++] = ~queueItem->data[curr_input_posi++];
        }
    }
    if (extra_color) {
        for (int i = 0; i < width_display; i++) {
            buffer[len_compressed++] = 0x75;
            buffer[len_compressed++] = byte_per_line + 7;
            buffer[len_compressed++] = byte_per_line;
            buffer[len_compressed++] = 0x00;
            buffer[len_compressed++] = 0x00;
            buffer[len_compressed++] = 0x00;
            buffer[len_compressed++] = 0x00;
            for (int b = 0; b < byte_per_line; b++) {
                if (queueItem->len <= curr_input_posi)
                    buffer[len_compressed++] = 0x00;
                else
                    buffer[len_compressed++] = queueItem->data[curr_input_posi++];
            }
        }
    }
    buffer[0] = len_compressed & 0xff;
    buffer[1] = (len_compressed >> 8) & 0xff;
    buffer[2] = (len_compressed >> 16) & 0xff;
    buffer[3] = (len_compressed >> 24) & 0xff;
    return len_compressed;
}

#endif
