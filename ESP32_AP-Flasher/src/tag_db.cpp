#include "tag_db.h"

#include <Arduino.h>
#include <ArduinoJson.h>
#include <FS.h>
#include <vector>

#include "LittleFS.h"
#include "language.h"

std::vector<tagRecord*> tagDB;
DynamicJsonDocument APconfig(150);

tagRecord* tagRecord::findByMAC(uint8_t mac[6]) {
    for (int16_t c = 0; c < tagDB.size(); c++) {
        tagRecord* tag = nullptr;
        tag = tagDB.at(c);
        if (memcmp(tag->mac, mac, 6) == 0) {
            return tag;
        }
    }
    return nullptr;
}

bool deleteRecord(uint8_t mac[6]) {
    for (int16_t c = 0; c < tagDB.size(); c++) {
        tagRecord* tag = nullptr;
        tag = tagDB.at(c);
        if (memcmp(tag->mac, mac, 6) == 0) {
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

String tagDBtoJson(uint8_t mac[6], uint8_t startPos) {
    DynamicJsonDocument doc(2500);
    JsonArray tags = doc.createNestedArray("tags");

    for (int16_t c = startPos; c < tagDB.size(); c++) {
        tagRecord* taginfo = nullptr;
        taginfo = tagDB.at(c);

        bool select = false;
        if (mac) {
            if (memcmp(taginfo->mac, mac, 6) == 0) {
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
        if (doc.capacity()-doc.memoryUsage() < doc.memoryUsage()/(c+1) + 100) {
            doc["continu"] = c+1;
            break;
        }
    }
    return doc.as<String>();
}

void fillNode(JsonObject &tag, tagRecord* &taginfo) {
    char buffer[16];
    sprintf(buffer, "%02X%02X%02X%02X%02X%02X\0", taginfo->mac[0], taginfo->mac[1], taginfo->mac[2], taginfo->mac[3], taginfo->mac[4], taginfo->mac[5]);
    tag["mac"] = (String)buffer;
    
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
}

void saveDB(String filename) {
    DynamicJsonDocument doc(2500);

    long t = millis();

    LittleFS.begin();
    fs::File file = LittleFS.open(filename, "w");
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
        serializeJson(doc, file);
    }
    file.write(']');

    file.close();
    Serial.println("DB saved " + String(millis() - t) + "ms");

    return;
}

void loadDB(String filename) {
    StaticJsonDocument<1000> doc;

    Serial.println("start reading DB from file");
    long t = millis();

    LittleFS.begin();
    fs::File readfile = LittleFS.open(filename, "r");
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
                uint8_t mac[12];
                if (sscanf(dst.c_str(), "%02X%02X%02X%02X%02X%02X", &mac[0], &mac[1], &mac[2], &mac[3], &mac[4], &mac[5]) == 6) {
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
    Serial.println(millis() - t);
    Serial.println("finished reading file");

    return;
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
    if (taginfo->data != nullptr) {
        free(taginfo->data);
        taginfo->data = nullptr;
    }
    taginfo->pending = false;
}

void initAPconfig() {
    LittleFS.begin(true);
    File configFile = LittleFS.open("/current/apconfig.json", "r");
    if (!configFile) {
        // default values'
        Serial.println("APconfig not found");
        APconfig["channel"] = 0;
        APconfig["alias"] = String();
        APconfig["ledbrightness"] = 255;
        APconfig["language"] = getDefaultLanguage();
        return;
    }
    DeserializationError error = deserializeJson(APconfig, configFile);
    if (error) {
        configFile.close();
        Serial.println("apconfig.json file corrupted, or not enough space in apconfig to hold it");
        return;
    }
    configFile.close();
}

void saveAPconfig() {
    fs::File configFile = LittleFS.open("/current/apconfig.json", "w");
    serializeJson(APconfig, configFile);
    configFile.close();
}