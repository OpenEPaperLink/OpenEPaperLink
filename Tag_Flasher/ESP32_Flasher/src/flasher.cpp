#include "flasher.h"

#include <Arduino.h>
#include <ArduinoJson.h>
#include <MD5Builder.h>

#include "LittleFS.h"
#include "storage.h"
// #include <FS.h>

#include "leds.h"
#include "settings.h"
#include "time.h"
#include "zbs_interface.h"

#define FINGERPRINT_FLASH_SIZE 10240

#ifdef OPENEPAPERLINK_PCB
bool extTagConnected() {
    // checks if the TEST (P1.0) pin on the ZBS243 will come up high. If it doesn't, there's probably a tag connected.
    pinMode(FLASHER_EXT_TEST, INPUT_PULLDOWN);
    vTaskDelay(5 / portTICK_PERIOD_MS);
    pinMode(FLASHER_EXT_TEST, INPUT_PULLUP);
    vTaskDelay(5 / portTICK_PERIOD_MS);
    return !digitalRead(FLASHER_EXT_TEST);
}
#endif

void dump(uint8_t *a, uint16_t l) {
    if (a == nullptr) {
        Serial.print("Tried to dump the contents of a nullptr, this is probably not what you want.\n");
    }
    Serial.printf("        ");
#define ROWS 16
    for (uint8_t c = 0; c < ROWS; c++) {
        Serial.printf(" %02X", c);
    }
    Serial.printf("\n--------");
    for (uint8_t c = 0; c < ROWS; c++) {
        Serial.printf("---");
    }
    for (uint16_t c = 0; c < l; c++) {
        if ((c % ROWS) == 0) {
            Serial.printf("\n0x%04X | ", c);
        }
        Serial.printf("%02X ", a[c]);
    }
    Serial.printf("\n--------");
    for (uint8_t c = 0; c < ROWS; c++) {
        Serial.printf("---");
    }
    Serial.printf("\n");
}


int8_t powerPinsExt[] = FLASHER_EXT_POWER;

uint8_t pinsExt[] = {FLASHER_EXT_CLK, FLASHER_EXT_MISO, FLASHER_EXT_MOSI, FLASHER_EXT_RESET, FLASHER_EXT_RXD, FLASHER_EXT_SS, FLASHER_EXT_TEST, FLASHER_EXT_TXD};

flasher::flasher() {
    zbs = new ZBS_interface;
    Storage.end();
}
flasher::~flasher() {
    delete zbs;
    Storage.begin();
}

static uint8_t validatePowerPinCount(int8_t *powerPin, uint8_t pinCount) {

    return pinCount;
}

#ifndef FLASHER_AP_SPEED
#define FLASHER_AP_SPEED 4000000
#endif

bool flasher::connectTag(uint8_t port) {
    bool result;
    uint8_t power_pins = 0;
    switch (port) {

        case 1:
            power_pins = validatePowerPinCount(powerPinsExt, sizeof(powerPinsExt));
            result = zbs->begin(FLASHER_EXT_SS, FLASHER_EXT_CLK, FLASHER_EXT_MOSI, FLASHER_EXT_MISO, FLASHER_EXT_RESET, (uint8_t *)powerPinsExt, power_pins, FLASHER_AP_SPEED);
            break;

        default:
            Serial.printf("Tried to connect to port %d, but this port isn't available. Some dev borked it up, probably Jelmer.\n", port);
            return false;
    }
    if (!result) Serial.printf("I tried connecting to port %d, but I couldn't establish a link to the tag. That's all I know.\n", port);
    return result;
}

void flasher::getFirmwareMD5() {
    uint8_t *buffer = (uint8_t *)malloc(FINGERPRINT_FLASH_SIZE);
    if (buffer == nullptr) {
        Serial.print("couldn't malloc bytes for firmware MD5\n");
        return;
    }

    zbs->select_flash(0);
    for (uint16_t c = 0; c < FINGERPRINT_FLASH_SIZE; c++) {
        buffer[c] = zbs->read_flash(c);
    }

    {
        MD5Builder md5calc;
        md5calc.begin();
        md5calc.add(buffer, FINGERPRINT_FLASH_SIZE);
        md5calc.calculate();
        md5calc.getBytes(md5);
    }

    for (uint8_t c = 0; c < 16; c++) {
        sprintf(md5char + (2 * c), "%02X", md5[c]);
    }
    Serial.printf("MD5=%s\n", md5char);
    free(buffer);
}

bool flasher::getInfoBlockMac() {
    if (!zbs->select_flash(1)) return false;
    for (uint16_t c = 7; c < 8; c--) {
        mac[7 - c] = zbs->read_flash(c + 0x10);
    }
    Serial.printf("Infopage mac=");
    uint16_t macsum = 0;
    for (uint8_t c = 0; c < 8; c++) {
        macsum += mac[c];
        Serial.printf("%02X", mac[c]);
    }
    Serial.printf("\n");
    if (macsum == 0) return false;
    if (macsum > 0x5F9) return false;
    return true;
}

bool flasher::getInfoBlockMD5() {
    if (!zbs->select_flash(1)) return false;
    for (uint16_t c = 0; c < 16; c++) {
        md5[c] = zbs->read_flash(c + 0x30);
    }
    uint16_t macsum = 0;
    for (uint8_t c = 0; c < 16; c++) {
        macsum += md5[c];
        sprintf(md5char + (2 * c), "%02X", md5[c]);
    }
    Serial.printf("Infoblock MD5=%s\n", md5char);
    if (macsum == 0) return false;     // invalid mac
    if (macsum > 0xF00) return false;  // *probably* an invalid mac
    return true;
}

bool flasher::getInfoBlockType() {
    if (!zbs->select_flash(1)) return false;
    tagtype = zbs->read_flash(0x19);
    return true;
}

bool flasher::findTagByMD5() {
    StaticJsonDocument<3000> doc;
    DynamicJsonDocument APconfig(600);
    fs::File readfile = contentFS->open("/tag_md5_db.json", "r");
    DeserializationError err = deserializeJson(doc, readfile);
    if (!err) {
        for (JsonObject elem : doc.as<JsonArray>()) {
            const char *jsonmd5 = elem["MD5"];
            if (jsonmd5 != nullptr) {
                if (strncmp(md5char, jsonmd5, 32) == 0) {
                    Serial.print("MD5 Matches > ");
                    const char *name = elem["name"];
                    Serial.println(name);
                    mac_suffix = strtoul(elem["mac_suffix"], 0, 16);
                    mac_format = elem["mac_format"];
                    mac_offset = elem["mac_offset"];
                    tagtype = elem["type"];
                    readfile.close();
                    return true;
                }
            }
        }
        Serial.print("Failed to find this tag's current firmware MD5 in the json database. If this tag is already OpenEpaperLink, this is to be expected.\n");
    } else {
        Serial.print("Failed to read json file\n");
    }
    readfile.close();
    return false;
}

bool flasher::findTagByType(uint8_t type) {
    StaticJsonDocument<3000> doc;
    DynamicJsonDocument APconfig(600);
    fs::File readfile = contentFS->open("/tag_md5_db.json", "r");
    DeserializationError err = deserializeJson(doc, readfile);
    if (!err) {
        for (JsonObject elem : doc.as<JsonArray>()) {
            if (elem["type"] != nullptr) {
                uint8_t jtype = elem["type"];
                if (jtype == type) {
                    Serial.print("Type Matches > ");
                    const char *name = elem["name"];
                    Serial.println(name);
                    const char *jsonmd5 = elem["MD5"];

                    for (uint8_t c = 0; c < 16; c++) {
                        uint32_t n = 0;
                        sscanf(jsonmd5 + (2 * c), "%02X", &n);
                        md5[c] = (uint8_t)n;
                    }

                    for (uint8_t c = 0; c < 16; c++) {
                        sprintf(md5char + (2 * c), "%02X", md5[c]);
                    }

                    mac_suffix = strtoul(elem["mac_suffix"], 0, 16);
                    mac_format = elem["mac_format"];
                    mac_offset = elem["mac_offset"];
                    tagtype = elem["type"];
                    readfile.close();
                    return true;
                }
            }
        }
        Serial.print("Failed to find this tag's type in the json database.\n");
    } else {
        Serial.print("Failed to read json file\n");
    }
    readfile.close();
    return false;
}

bool flasher::getFirmwareMac() {
    if (!mac_offset) return false;
    switch (mac_format) {
        case 1:
            get_mac_format1();
            break;
        case 2:
            get_mac_format2();
            break;
        default:
            return false;
    }
    return true;
}

void flasher::getMacFromWiFi() {
    mac[0] = 0x00;
    mac[1] = 0x00;
    esp_read_mac(mac + 2, ESP_MAC_WIFI_SOFTAP);
}

bool flasher::backupFlash() {
    getFirmwareMD5();
    if (!zbs->select_flash(0)) return false;
    md5char[16] = 0x00;
    fs::File backup = contentFS->open("/" + (String)md5char + "_backup.bin", "w", true);
    for (uint32_t c = 0; c < 65535; c++) {
        backup.write(zbs->read_flash(c));
    }
    backup.close();
    return true;
}

// extract original mac from firmware (1.54" and 2.9") and make it 2 bytes longer based on info in settings.h
void flasher::get_mac_format1() {
    zbs->select_flash(0);
    for (uint8_t c = 0; c < 6; c++) {
        mac[c + 2] = zbs->read_flash(mac_offset + c);  // 0xFC06
    }
    mac[0] = (uint8_t)(CUSTOM_MAC_HDR >> 8);
    mac[1] = (uint8_t)CUSTOM_MAC_HDR;

    mac[6] = (uint8_t)(mac_suffix >> 8);
    mac[7] = (uint8_t)(mac_suffix & 0xFF);

    uint8_t xorchk = 0;
    for (uint8_t c = 2; c < 8; c++) {
        xorchk ^= (mac[c] & 0x0F);
        xorchk ^= (mac[c] >> 4);
    }
    mac[7] |= xorchk;
}

// extract original mac from segmented tag
void flasher::get_mac_format2() {
    zbs->select_flash(0);

    for (uint8_t c = 0; c < 3; c++) {
        mac[c] = zbs->read_flash(mac_offset + c);  // 0x7802
    }
    for (uint8_t c = 3; c < 6; c++) {
        mac[c] = zbs->read_flash(mac_offset + 2 + c);
    }

    uint16_t type = mac_suffix;
    mac[6] = (uint8_t)(type >> 8);
    mac[7] = (uint8_t)(type & 0xFF);
}

// erase flash and program from flash buffer
bool flasher::writeFlash(uint8_t *flashbuffer, uint16_t size) {
    if (!zbs->select_flash(0)) return false;
    zbs->erase_flash();
    if (!zbs->select_flash(0)) return false;
    Serial.printf("Starting flash, size=%d\n", size);
    for (uint16_t c = 0; c < size; c++) {
        if (flashbuffer[c] == 0xFF) goto flashWriteSuccess;
        for (uint8_t i = 0; i < MAX_WRITE_ATTEMPTS; i++) {
            zbs->write_flash(c, flashbuffer[c]);
            if (zbs->read_flash(c) == flashbuffer[c]) {
                goto flashWriteSuccess;
            }
        }
        return false;
    flashWriteSuccess:
        if (c % 256 == 0) {
#ifdef HAS_RGB_LED
            shortBlink(CRGB::Yellow);
#else
            quickBlink(2);
#endif
            Serial.printf("\rNow flashing, %d/%d  ", c, size);
            vTaskDelay(1 / portTICK_PERIOD_MS);
        }
    }
    return true;
}

bool flasher::writeBlock256(uint16_t offset, uint8_t *flashbuffer) {
    for (uint16_t c = 0; c < 256; c++) {
        if (flashbuffer[c] == 0xFF) goto flashWriteSuccess;
        for (uint8_t i = 0; i < MAX_WRITE_ATTEMPTS; i++) {
            zbs->write_flash(offset + c, flashbuffer[c]);
            if (zbs->read_flash(offset + c) == flashbuffer[c]) {
                goto flashWriteSuccess;
            }
        }
        return false;
    flashWriteSuccess:
        continue;
    }
    return true;
}

// get info from infoblock (eeprom flash, kinda)
bool flasher::readInfoBlock() {
    if (!zbs->select_flash(1)) return false;
    if (infoblock == nullptr) {
        infoblock = (uint8_t *)malloc(1024);
        if (infoblock == nullptr) return false;
    }
    for (uint16_t c = 0; c < 1024; c++) {
        infoblock[c] = zbs->read_flash(c);
    }
    return true;
}

// write info to infoblock
bool flasher::writeInfoBlock() {
    if (infoblock == nullptr) return false;
    if (!zbs->select_flash(1)) return false;
    zbs->erase_infoblock();
    if (!zbs->select_flash(1)) return false;
    // select info page

    for (uint16_t c = 0; c < 1024; c++) {
        if (infoblock[c] == 0xFF) goto ifBlockWriteSuccess;
        for (uint8_t i = 0; i < MAX_WRITE_ATTEMPTS; i++) {
            zbs->write_flash(c, infoblock[c]);
            if (zbs->read_flash(c) == infoblock[c]) {
                goto ifBlockWriteSuccess;
            }
        }
        return false;
    ifBlockWriteSuccess:
        continue;
    }
    return true;
}

bool flasher::prepareInfoBlock() {
    if (infoblock == nullptr) return false;
    for (uint8_t c = 7; c < 8; c--) {
        infoblock[0x10 + (7 - c)] = mac[c];
    }
    infoblock[0x19] = tagtype;
    for (uint8_t c = 0; c < 16; c++) {
        infoblock[0x30 + c] = md5[c];
    }
    return true;
}

bool flasher::writeFlashFromPackOffset(fs::File *file, uint16_t length) {
    if (!zbs->select_flash(0)) return false;
    zbs->erase_flash();
    if (!zbs->select_flash(0)) return false;
    Serial.printf("Starting flash, size=%d\n", length);

    uint8_t *buf = (uint8_t *)malloc(256);
    uint16_t offset = 0;
    while (length) {
        if (length > 256) {
            file->read(buf, 256);
            length -= 256;
        } else {
            file->read(buf, length);
            length = 0;
        }
#ifdef HAS_RGB_LED
        shortBlink(CRGB::Yellow);
#else
        quickBlink(2);
#endif
        Serial.printf("\rFlashing, %d bytes left     ", length);
        bool res = writeBlock256(offset, buf);
        offset += 256;
        if (!res) {
            Serial.printf("Failed writing block to tag, probably a hardware failure\n");
            return false;
        }
        vTaskDelay(1 / portTICK_PERIOD_MS);
    }
    Serial.printf("\nFlashing done\n");
    return true;
}

bool flasher::writeFlashFromPack(String filename, uint8_t type) {
    StaticJsonDocument<512> doc;
    DynamicJsonDocument APconfig(512);
    fs::File readfile = contentFS->open(filename, "r");
    DeserializationError err = deserializeJson(doc, readfile);
    if (!err) {
        for (JsonObject elem : doc.as<JsonArray>()) {
            if (elem["type"] != nullptr) {
                uint8_t jtype = elem["type"];
                if (jtype == type) {
                    const char *name = elem["name"];
                    Serial.print("Flashing from FW pack: ");
                    Serial.println(name);

                    uint32_t offset = elem["offset"];
                    uint16_t length = elem["length"];
                    readfile.seek(offset);
                    bool result = writeFlashFromPackOffset(&readfile, length);
                    readfile.close();
                    return result;
                }
            }
        }
        Serial.print("Failed to find this tag's type in the FW pack database.\n");
    } else {
        Serial.print("Failed to read json header from FW pack\n");
    }
    readfile.close();
    return false;
}

bool flasher::readBlock(uint16_t offset, uint8_t *data, uint16_t len, bool infopage) {
    if (infopage) {
        if (!zbs->select_flash(1)) return false;
        if (offset > 1024) return false;
    } else {
        if (!zbs->select_flash(0)) return false;
        if (offset > 65535) return false;
    }
    for (uint32_t c = 0; c < len; c++) {
        data[c] = zbs->read_flash(offset + c);
    }
    return true;
}

bool flasher::writeBlock(uint16_t offset, uint8_t *data, uint16_t len, bool infopage) {
    if (infopage) {
        if (!zbs->select_flash(1)) return false;
        if (offset > 1024) return false;
    } else {
        if (!zbs->select_flash(0)) return false;
        if (offset > 65535) return false;
    }
    for (uint32_t c = 0; c < len; c++) {
        zbs->write_flash(c + offset, data[c]);
    }
    return true;
}


void flashCountDown(uint8_t c) {
    Serial.printf("\r%d  ", c);
    for (c -= 1; c < 254; c--) {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        Serial.printf("\r%d  ", c);
    }
}

// perform device flash, save mac, everything
bool doTagFlash() {
    class flasher *f = new flasher();
    if (!f->connectTag(FLASHER_EXT_PORT)) {
        Serial.printf("Sorry, failed to connect to this tag...\n");
        return false;
    }

    f->getFirmwareMD5();

    if (f->findTagByMD5()) {
        // this tag currently contains original firmware, found its fingerprint
        Serial.printf("Found original firmware tag, recognized its fingerprint (%s)\n", f->md5char);
        f->readInfoBlock();
        f->getFirmwareMac();
        f->prepareInfoBlock();
        f->writeInfoBlock();
        f->writeFlashFromPack("/Tag_FW_Pack.bin", f->tagtype);
        f->zbs->reset();
    } else if (f->getInfoBlockMD5()) {
        // did find an infoblock MD5 that looks valid
        if (f->findTagByMD5()) {
            // did find the md5 in the database
            Serial.printf("Found an already-flashed tag, recognized its fingerprint (%s)\n", f->md5char);
            f->getInfoBlockMac();
            f->getInfoBlockType();
            f->readInfoBlock();
            f->writeFlashFromPack("/Tag_FW_Pack.bin", f->tagtype);
            f->zbs->reset();
        } else {
            // couldn't find the md5 from the infoblock
            Serial.printf("Found an already-flashed tag, but we couldn't find its fingerprint (%s) in the database\n", f->md5char);
            return false;
        }
    } else {
        // We couldn't recognize the tag from it's fingerprint...
        Serial.printf("Found a tag but didn't recognize its fingerprint\n", f->md5char);
        f->backupFlash();
        Serial.printf("Saved this MD5 binary to filesystem\n");
    }
    delete f;
    return false;
}
