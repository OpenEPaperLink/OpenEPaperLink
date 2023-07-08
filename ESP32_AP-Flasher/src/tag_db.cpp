#include "tag_db.h"

#include <Arduino.h>
#include <ArduinoJson.h>
#include <FS.h>
#include <vector>

#include "storage.h"
#include "language.h"

std::vector<tagRecord*> tagDB;

Config config;
// SemaphoreHandle_t tagDBOwner;

tagRecord* tagRecord::findByMAC(uint8_t mac[8]) {
    for (int16_t c = 0; c < tagDB.size(); c++) {
        tagRecord* tag = nullptr;
        tag = tagDB.at(c);
        if (memcmp(tag->mac, mac, 8) == 0) {
            return tag;
        }
    }
    return nullptr;
}

bool deleteRecord(uint8_t mac[8]) {
    for (int16_t c = 0; c < tagDB.size(); c++) {
        tagRecord* tag = nullptr;
        tag = tagDB.at(c);
        if (memcmp(tag->mac, mac, 8) == 0) {
            if (tag->data != nullptr) {
                free(tag->data);
            }
            tag->data = nullptr;
            delete tagDB[c];
            tagDB.erase(tagDB.begin() + c);
            return true;
        }
    }
    return false;
}

void mac2hex(uint8_t* mac, char* hexBuffer) {
    sprintf(hexBuffer, "%02X%02X%02X%02X%02X%02X%02X%02X",
            mac[7], mac[6], mac[5], mac[4], mac[3], mac[2], mac[1], mac[0]);
}

bool hex2mac(const String& hexString, uint8_t* mac) {
    size_t hexLength = hexString.length();
    if (hexLength != 12 && hexLength != 16) {
        return false;
    }
    if (hexLength / 2 == 6) {
        mac[6] = 0;
        mac[7] = 0;
        return (sscanf(hexString.c_str(), "%02hhX%02hhX%02hhX%02hhX%02hhX%02hhX",
                        &mac[5], &mac[4], &mac[3], &mac[2], &mac[1], &mac[0]) == 6);
    } else {
        return (sscanf(hexString.c_str(), "%02hhX%02hhX%02hhX%02hhX%02hhX%02hhX%02hhX%02hhX",
                        &mac[7], &mac[6], &mac[5], &mac[4], &mac[3], &mac[2], &mac[1], &mac[0]) == 8);
    }
}

String tagDBtoJson(uint8_t mac[8], uint8_t startPos) {
    DynamicJsonDocument doc(5000);
    JsonArray tags = doc.createNestedArray("tags");

    for (int16_t c = startPos; c < tagDB.size(); c++) {
        tagRecord* taginfo = nullptr;
        taginfo = tagDB.at(c);

        bool select = false;
        if (mac) {
            if (memcmp(taginfo->mac, mac, 8) == 0) {
                select = true;
            }
        } else {
            select = true;
        }
        if (select) {
            JsonObject tag = tags.createNestedObject();
            fillNode(tag, taginfo);
            if (mac) {
                break;
            }
        }
        if (doc.capacity() - doc.memoryUsage() < doc.memoryUsage()/(c+1) + 150) {
            doc["continu"] = c+1;
            break;
        }
    }
    return doc.as<String>();
}

void fillNode(JsonObject &tag, tagRecord* &taginfo) {
    char hexmac[17];
    mac2hex(taginfo->mac, hexmac);
    tag["mac"] = String(hexmac);
    char hex[33];
    for (uint8_t i = 0; i < 16; i++) {
        sprintf(hex + (i * 2), "%02x", taginfo->md5[i]);
    }
    tag["hash"] = (String)hex;
    tag["lastseen"] = taginfo->lastseen;
    tag["nextupdate"] = taginfo->nextupdate;
    tag["nextcheckin"] = taginfo->expectedNextCheckin;
    tag["pending"] = taginfo->pending;
    tag["alias"] = taginfo->alias;
    tag["contentMode"] = taginfo->contentMode;
    tag["LQI"] = taginfo->LQI;
    tag["RSSI"] = taginfo->RSSI;
    tag["temperature"] = taginfo->temperature;
    tag["batteryMv"] = taginfo->batteryMv;
    tag["hwType"] = taginfo->hwType;
    tag["wakeupReason"] = taginfo->wakeupReason;
    tag["capabilities"] = taginfo->capabilities;
    tag["modecfgjson"] = taginfo->modeConfigJson;
    tag["isexternal"] = taginfo->isExternal;
    tag["rotate"] = taginfo->rotate;
    tag["lut"] = taginfo->lut;
    tag["ch"] = taginfo->currentChannel;
    tag["ver"] = taginfo->tagSoftwareVersion;
}

void saveDB(String filename) {
    DynamicJsonDocument doc(2500);

    long t = millis();

    Storage.begin();
    fs::File file = contentFS->open(filename, "w");
    if (!file) {
        Serial.println("saveDB: Failed to open file");
        return;
    }

    file.write('[');

    for (int16_t c = 0; c < tagDB.size(); c++) {
        doc.clear();
        tagRecord* taginfo = nullptr;
        taginfo = tagDB.at(c);

        JsonObject tag = doc.createNestedObject();
        fillNode(tag, taginfo);
        if (c > 0) {
            file.write(',');
        }
        serializeJsonPretty(doc, file);
    }
    file.write(']');

    file.close();
    Serial.println("DB saved " + String(millis() - t) + "ms");

    return;
}

void loadDB(String filename) {
    StaticJsonDocument<1000> doc;

    Serial.println("reading DB from file");
    long t = millis();

    Storage.begin();
    fs::File readfile = contentFS->open(filename, "r");
    if (!readfile) {
        Serial.println("loadDB: Failed to open file");
        return;
    }

    time_t now;
    time(&now);
    bool parsing = true;

    if (readfile.find("[")) {
        while (parsing) {
            DeserializationError err = deserializeJson(doc, readfile);
            if (!err) {
                JsonObject tag = doc[0];
                String dst = tag["mac"].as<String>();
                uint8_t mac[8];
                if (hex2mac(dst, mac)) {
                    tagRecord* taginfo = nullptr;
                    taginfo = tagRecord::findByMAC(mac);
                    if (taginfo == nullptr) {
                        taginfo = new tagRecord;
                        memcpy(taginfo->mac, mac, sizeof(taginfo->mac));
                        tagDB.push_back(taginfo);
                    }
                    String md5 = tag["hash"].as<String>();
                    if (md5.length() >= 32) {
                        for (int i = 0; i < 16; i++) {
                            taginfo->md5[i] = strtoul(md5.substring(i * 2, i * 2 + 2).c_str(), NULL, 16);
                        }
                    }
                    memcpy(taginfo->md5pending, taginfo->md5, sizeof(taginfo->md5));
                    taginfo->lastseen = (uint32_t)tag["lastseen"];
                    taginfo->nextupdate = (uint32_t)tag["nextupdate"];
                    taginfo->expectedNextCheckin = (uint16_t)tag["nextcheckin"];
                    if (taginfo->expectedNextCheckin < now - 1800) { 
                        taginfo->expectedNextCheckin = now + 1800; 
                    }
                    taginfo->pending = false;
                    taginfo->alias = tag["alias"].as<String>();
                    taginfo->contentMode = tag["contentMode"];
                    taginfo->LQI = tag["LQI"]; 
                    taginfo->RSSI = tag["RSSI"]; 
                    taginfo->temperature = tag["temperature"]; 
                    taginfo->batteryMv = tag["batteryMv"]; 
                    taginfo->hwType = (uint8_t)tag["hwType"];
                    taginfo->wakeupReason = tag["wakeupReason"];
                    taginfo->capabilities = tag["capabilities"];
                    taginfo->modeConfigJson = tag["modecfgjson"].as<String>();
                    taginfo->isExternal = tag["isexternal"].as<bool>();
                    taginfo->rotate = tag["rotate"] | 0;
                    taginfo->lut = tag["lut"] | 0;
                    taginfo->currentChannel = tag["ch"] | 0;
                    taginfo->tagSoftwareVersion = tag["ver"] | 0;
                }
            } else {
                Serial.print(F("deserializeJson() failed: "));
                Serial.println(err.c_str());
                parsing = false;
            }
            parsing = parsing && readfile.find(",");
        }
    }

    readfile.close();
    return;
}

void destroyDB() {
    Serial.println("destoying DB");
    Serial.printf("before, free heap: %d\n", ESP.getFreeHeap());
    for (int16_t c = 0; c < tagDB.size(); c++) {
        tagRecord* tag = nullptr;
        tag = tagDB.at(c);
        if (tag->data != nullptr) {
            free(tag->data);
        }
        tag->data = nullptr;
        delete tagDB[c];
        tagDB.erase(tagDB.begin() + c);
    }
    Serial.printf("after, free heap: %d\n", ESP.getFreeHeap());
}

uint8_t getTagCount() {
    uint8_t tagcount = 0;
    for (int16_t c = 0; c < tagDB.size(); c++) {
        tagRecord* taginfo = nullptr;
        taginfo = tagDB.at(c);
        if (taginfo->isExternal == false) tagcount++;
    }
    return tagcount;
}

void clearPending(tagRecord* taginfo) {
    taginfo->filename = String();
    if (taginfo->data != nullptr) {
        free(taginfo->data);
        taginfo->data = nullptr;
    }
    taginfo->pending = false;
}

void initAPconfig() {
    Storage.begin();
    DynamicJsonDocument APconfig(500);
    File configFile = contentFS->open("/current/apconfig.json", "r");
    if (configFile) {
        DeserializationError error = deserializeJson(APconfig, configFile);
        if (error) {
            configFile.close();
            Serial.println("failed to read apconfig.json. Using default config");
            Serial.println(error.c_str());
        }
        configFile.close();
    }
    config.channel = APconfig["channel"] | 0;
    if (APconfig["alias"]) strlcpy(config.alias, APconfig["alias"], sizeof(config.alias));
    config.led = APconfig["led"] | 255;
    config.language = APconfig["language"] | getDefaultLanguage();
    config.maxsleep = APconfig["maxsleep"] | 10;
    config.stopsleep = APconfig["stopsleep"] | 1;
    // default wifi power 8.5 dbM
    // see https://github.com/espressif/arduino-esp32/blob/master/libraries/WiFi/src/WiFiGeneric.h#L111
    config.wifiPower = APconfig["wifipower"] | 34;
    if (APconfig["timezone"]) {
        strlcpy(config.timeZone, APconfig["timezone"], sizeof(config.timeZone));
    } else {
        strlcpy(config.timeZone, "CET-1CEST,M3.5.0,M10.5.0/3", sizeof(config.timeZone));
    }
}

void saveAPconfig() {
    fs::File configFile = contentFS->open("/current/apconfig.json", "w");
    DynamicJsonDocument APconfig(500);
    APconfig["channel"] = config.channel;
    APconfig["alias"] = config.alias;
    APconfig["led"] = config.led;
    APconfig["language"] = config.language;
    APconfig["maxsleep"] = config.maxsleep;
    APconfig["stopsleep"] = config.stopsleep;
    APconfig["wifipower"] = config.wifiPower;
    APconfig["timezone"] = config.timeZone;
    serializeJsonPretty(APconfig, configFile);
    configFile.close();
}