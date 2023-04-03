#include "flasher.h"

#include <Arduino.h>
#include <LittleFS.h>

#include "settings.h"
#include "time.h"
#include "zbs_interface.h"

typedef enum {
    CMD_GET_VERSION = 1,
    CMD_RESET_ESP = 2,
    CMD_ZBS_BEGIN = 10,
    CMD_RESET_ZBS = 11,
    CMD_SELECT_PAGE = 12,
    CMD_SET_POWER = 13,
    CMD_READ_RAM = 20,
    CMD_WRITE_RAM = 21,
    CMD_READ_FLASH = 22,
    CMD_WRITE_FLASH = 23,
    CMD_READ_SFR = 24,
    CMD_WRITE_SFR = 25,
    CMD_ERASE_FLASH = 26,
    CMD_ERASE_INFOBLOCK = 27,
} ZBS_UART_PROTO;

uint8_t *infoblock = nullptr;
uint8_t *flashbuffer = nullptr;

static class ZBS_interface *zbs;

// look for the latest version of the firmware file... It's supposed to be something like zigbeebase0003.bin
String lookupFirmwareFile(uint16_t &version) {
    String filename;
    File root = LittleFS.open("/");
    File file = root.openNextFile();
    while (file) {
        if (strncmp(file.name(), "zigbeebase", 10) == 0) {
            filename = "/" + ((String)file.name());
        }
        file.close();
        file = root.openNextFile();
    }
    if (filename.length()) {
        char buffer[16];
        memset(buffer, 0, 16);
        strncpy(buffer, filename.c_str() + 11, 4);
        version = strtoul(buffer, NULL, 16);
        root.close();
        return filename;
    } else {
        version = 0;
        root.close();
        return "";
    }
}

// guess device type based on flash memory contents
uint16_t getDeviceType() {
    uint8_t type29[8] = {0x7d, 0x22, 0xff, 0x02, 0xa4, 0x58, 0xf0, 0x90};
    uint8_t type154[8] = {0xa1, 0x23, 0x22, 0x02, 0xa4, 0xc3, 0xe4, 0xf0};
    uint8_t buffer[8] = {0};
    zbs->select_flash(0);
    for (uint8_t c = 0; c < 8; c++) {
        buffer[c] = zbs->read_flash(0x08 + c);
    }
    if (memcmp(buffer, type29, 8) == 0) {
        return 0x3B10;
    }
    if (memcmp(buffer, type154, 8) == 0) {
        return 0x3410;
    }
    return 0xFFF0;
}

// extract original mac from firmware and make it 2 bytes longer based on info in settings.h
uint64_t getOriginalTagMac() {
    zbs->select_flash(0);
    uint8_t mac[8] = {0};
    for (uint8_t c = 0; c < 6; c++) {
        mac[c + 2] = zbs->read_flash(0xFC06 + c);
    }
    mac[0] = (uint8_t)(CUSTOM_MAC_HDR >> 8);
    mac[1] = (uint8_t)CUSTOM_MAC_HDR;

    uint16_t type = getDeviceType();
    mac[6] = (uint8_t)(type >> 8);
    mac[7] = (uint8_t)(type & 0xFF);

    uint8_t xorchk = 0;
    for (uint8_t c = 2; c < 8; c++) {
        xorchk ^= (mac[c] & 0x0F);
        xorchk ^= (mac[c] >> 4);
    }
    mac[7] |= xorchk;
    return *((uint64_t *)(mac));
}

// extract custom firmware mac from Infoblock
uint64_t getInfoBlockMac() {
    zbs->select_flash(1);
    uint8_t mac[8] = {0};
    for (uint8_t c = 0; c < 8; c++) {
        mac[c] = zbs->read_flash(0x10 + c);
    }
    return *((uint64_t *)(mac));
}

// get info from infoblock (eeprom flash, kinda)
void readInfoBlock() {
    if (infoblock == nullptr) {
        // allocate room for infopage
        infoblock = (uint8_t *)calloc(1024, 1);
    }
    zbs->select_flash(1);  // select info page
    for (uint16_t c = 0; c < 1024; c++) {
        infoblock[c] = zbs->read_flash(c);
    }
}

// write info to infoblock
void writeInfoBlock() {
    if (infoblock == nullptr) {
        return;
    }
    zbs->select_flash(1);
    zbs->erase_infoblock();
    zbs->select_flash(1);  // select info page
    for (uint16_t c = 0; c < 1024; c++) {
        for (uint8_t i = 0; i < MAX_WRITE_ATTEMPTS; i++) {
            zbs->write_flash(c, infoblock[c]);
            if (zbs->read_flash(c) == infoblock[c]) {
                break;
            }
        }
    }
}

// erase flash and program from flash buffer
void writeFlashBlock(uint16_t size) {
    if (flashbuffer == nullptr) {
        return;
    }
    zbs->select_flash(0);
    zbs->erase_flash();
    zbs->select_flash(0);
    Serial.printf("Starting flash, size=%d\n", size);
    uint8_t i = 0;
    for (uint16_t c = 0; c < size; c++) {
        for (i = 0; i < MAX_WRITE_ATTEMPTS; i++) {
            zbs->write_flash(c, flashbuffer[c]);
            if (zbs->read_flash(c) == flashbuffer[c]) {
                break;
            }
        }
        if (i == MAX_WRITE_ATTEMPTS) {
            Serial.printf("x");
        } else {
            Serial.printf(".");
        }
        if (c % 256 == 0) {
            Serial.printf("\rNow flashing, %d/%d  ", c, size);
            vTaskDelay(1 / portTICK_PERIOD_MS);
        }
    }
}

// perform device flash, save mac, everything
bool performDeviceFlash() {
    uint8_t interfaceWorking = 0;
    zbs = new ZBS_interface;
    interfaceWorking = zbs->begin(FLASHER_AP_SS, FLASHER_AP_CLK, FLASHER_AP_MOSI, FLASHER_AP_MISO, FLASHER_AP_RESET, FLASHER_AP_POWER, 8000000);
    if (!interfaceWorking) {
        Serial.print("I wasn't able to connect to a ZBS tag, please check wiring and definitions in the settings.h file.\n");
        delete zbs;
        return false;
    }

    readInfoBlock();
    uint8_t mac[8] = {0};
    *((uint64_t *)(mac)) = getInfoBlockMac();
    // check if the mac has been set at all, 0xFF- is not allowed
    if (*((uint64_t *)(mac)) == 0xFFFFFFFFFFFFFFFF) {
        // mac not set in infopage, get it from the original firmware
        *((uint64_t *)(mac)) = getOriginalTagMac();
        zbs->select_flash(1);
        for (uint8_t c = 0; c < 8; c++) {
            infoblock[0x17 - c] = mac[c];
            // write mac directly to infoblock without erasing; the bytes should all be 0xFF anyway
            zbs->write_flash(0x17 - c, mac[c]);
        }
    }

    uint16_t version = 0;
    File file = LittleFS.open(lookupFirmwareFile(version));
    if (!file) {
        // couldn't find a valid firmware version
        delete zbs;
        return false;
    } else {
        Serial.printf("Preparing to flash version %04X (%d bytes) to the tag\n", version, file.size());
    }

    // load buffer with zbs firmware
    uint16_t flashbuffersize = file.size();
    flashbuffer = (uint8_t *)calloc(flashbuffersize, 1);
    uint32_t index = 0;
    while (file.available()) {
        flashbuffer[index] = file.read();
        index++;
        portYIELD();
    }

    // write firmware from buffer and finish by rewriting the info block again
    writeFlashBlock(flashbuffersize);
    writeInfoBlock();
    Serial.print("All done with flashing\n");

    file.close();
    free(infoblock);
    infoblock = nullptr;
    free(flashbuffer);
    flashbuffer = nullptr;
    zbs->reset();
    zbs->set_power(1);
    delete zbs;
    return true;
}
