#include "tag_db.h"

#include <Arduino.h>
#include <ArduinoJson.h>
#include <FS.h>

#include <unordered_map>
#include <vector>

#include "language.h"
#include "storage.h"
#include "util.h"

std::vector<tagRecord*> tagDB;
std::unordered_map<std::string, varStruct> varDB;
std::unordered_map<int, HwType> hwdata = {
    {0, {152, 152, 0, 2}},
    {1, {296, 128, 1, 2}},
    {2, {400, 300, 0, 2}}};

Config config;

tagRecord* tagRecord::findByMAC(const uint8_t mac[8]) {
    for (tagRecord* tag : tagDB) {
        if (memcmp(tag->mac, mac, 8) == 0) {
            return tag;
        }
    }
    return nullptr;
}

bool deleteRecord(const uint8_t mac[8]) {
    for (uint32_t c = 0; c < tagDB.size(); c++) {
        tagRecord* tag = tagDB.at(c);
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

void mac2hex(const uint8_t* mac, char* hexBuffer) {
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

String tagDBtoJson(const uint8_t mac[8], uint8_t startPos) {
    DynamicJsonDocument doc(5000);
    JsonArray tags = doc.createNestedArray("tags");

    for (uint32_t c = startPos; c < tagDB.size(); ++c) {
        const tagRecord* taginfo = tagDB.at(c);

        const bool select = !mac || memcmp(taginfo->mac, mac, 8) == 0;
        if (select) {
            JsonObject tag = tags.createNestedObject();
            fillNode(tag, taginfo);
            if (mac) {
                break;
            }
        }

        if (doc.capacity() - doc.memoryUsage() < doc.memoryUsage() / (c + 1) + 500) {
            doc["continu"] = c + 1;
            break;
        }
    }

    return doc.as<String>();
}

void fillNode(JsonObject& tag, const tagRecord* taginfo) {
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

void saveDB(const String& filename) {
    DynamicJsonDocument doc(2500);

    const long t = millis();

    Storage.begin();
    xSemaphoreTake(fsMutex, portMAX_DELAY);
    fs::File file = contentFS->open(filename, "w");
    if (!file) {
        Serial.println("saveDB: Failed to open file");
        xSemaphoreGive(fsMutex);
        return;
    }

    file.write('[');
    for (size_t c = 0; c < tagDB.size(); c++) {
        const tagRecord* taginfo = tagDB.at(c);
        doc.clear();

        JsonObject tag = doc.createNestedObject();
        fillNode(tag, taginfo);
        if (c > 0) {
            file.write(',');
        }
        serializeJsonPretty(doc, file);
    }
    file.write(']');

    file.close();
    xSemaphoreGive(fsMutex);
    Serial.println("DB saved " + String(millis() - t) + "ms");
}

void loadDB(const String& filename) {
    Serial.println("reading DB from file");
    const long t = millis();

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
        StaticJsonDocument<1000> doc;
        while (parsing) {
            DeserializationError err = deserializeJson(doc, readfile);
            if (!err) {
                JsonObject tag = doc[0];
                String dst = tag["mac"].as<String>();
                uint8_t mac[8];
                if (hex2mac(dst, mac)) {
                    tagRecord* taginfo = tagRecord::findByMAC(mac);
                    if (taginfo == nullptr) {
                        taginfo = new tagRecord;
                        memcpy(taginfo->mac, mac, sizeof(taginfo->mac));
                        tagDB.push_back(taginfo);
                    }
                    String md5 = tag["hash"].as<String>();
                    if (md5.length() >= 32) {
                        for (uint8_t i = 0; i < 16; i++) {
                            taginfo->md5[i] = strtoul(md5.substring(i * 2, i * 2 + 2).c_str(), NULL, 16);
                        }
                    }
                    memcpy(taginfo->md5pending, taginfo->md5, sizeof(taginfo->md5));
                    taginfo->lastseen = (uint32_t)tag["lastseen"];
                    taginfo->nextupdate = (uint32_t)tag["nextupdate"];
                    taginfo->expectedNextCheckin = (uint16_t)tag["nextcheckin"];
                    if (taginfo->expectedNextCheckin < now) {
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
    Serial.println("loadDB took " + String(millis() - t) + "ms");
}

void destroyDB() {
    Serial.println("destoying DB");
    util::printHeap();
    for (tagRecord*& tag : tagDB) {
        if (tag->data != nullptr) {
            free(tag->data);
        }
        tag->data = nullptr;
        delete tag;
    }
    tagDB.clear();
    util::printHeap();
}

uint32_t getTagCount() {
    uint32_t temp = 0;
    return getTagCount(temp);
}

uint32_t getTagCount(uint32_t& timeoutcount) {
    uint32_t tagcount = 0;
    time_t now;
    time(&now);
    for (const tagRecord* taginfo : tagDB) {
        if (!taginfo->isExternal) tagcount++;
        const int32_t timeout = now - taginfo->lastseen;
        if (taginfo->expectedNextCheckin < 3600) {
            // not initialised, timeout if not seen last 10 minutes
            if (timeout > 600) timeoutcount++;
        } else if (now - taginfo->expectedNextCheckin > 600) {
            //expected checkin is behind, timeout if not seen last 10 minutes
            if (timeout > 600) timeoutcount++;
        }
    }
    return tagcount;
}

void clearPending(tagRecord* taginfo) {
    taginfo->filename = String();
    if (taginfo->data != nullptr) {
        // check if this is the last copy of the buffer
        int datacount = 0;
        for (const tagRecord* tag : tagDB) {
            if (tag->data == taginfo->data) {
                datacount++;
            }
        }
        if (datacount == 1) {
            free(taginfo->data);
        }
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
    config.preview = APconfig["preview"] | 1;
    config.sleepTime1 = APconfig["sleeptime1"] | 0;
    config.sleepTime2 = APconfig["sleeptime2"] | 0;
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
    xSemaphoreTake(fsMutex, portMAX_DELAY);
    fs::File configFile = contentFS->open("/current/apconfig.json", "w");
    DynamicJsonDocument APconfig(500);
    APconfig["channel"] = config.channel;
    APconfig["alias"] = config.alias;
    APconfig["led"] = config.led;
    APconfig["language"] = config.language;
    APconfig["maxsleep"] = config.maxsleep;
    APconfig["stopsleep"] = config.stopsleep;
    APconfig["preview"] = config.preview;
    APconfig["wifipower"] = config.wifiPower;
    APconfig["timezone"] = config.timeZone;
    APconfig["sleeptime1"] = config.sleepTime1;
    APconfig["sleeptime2"] = config.sleepTime2;
    serializeJsonPretty(APconfig, configFile);
    configFile.close();
    xSemaphoreGive(fsMutex);
}

HwType getHwType(const uint8_t id) {
    auto it = hwdata.find(id);
    if (it != hwdata.end()) {
        return it->second;
    } else {
        char filename[20];
        snprintf(filename, sizeof(filename), "/tagtypes/%02X.json", id);
        Serial.printf("read %s\n", filename);
        File jsonFile = contentFS->open(filename, "r");

        if (jsonFile) {
            StaticJsonDocument<100> filter;
            filter["width"] = true;
            filter["height"] = true;
            filter["rotatebuffer"] = true;
            filter["bpp"] = true;
            filter["shortlut"] = true;
            StaticJsonDocument<250> doc;
            DeserializationError error = deserializeJson(doc, jsonFile, DeserializationOption::Filter(filter));
            jsonFile.close();
            if (error) {
                Serial.println("json error in " + String(filename));
                Serial.println(error.c_str());
            } else {
                hwdata[id].width = doc["width"];
                hwdata[id].height = doc["height"];
                hwdata[id].rotatebuffer = doc["rotatebuffer"];
                hwdata[id].bpp = doc["bpp"];
                hwdata[id].shortlut = doc["shortlut"];
                return hwdata.at(id);
            }
        }
        return {0, 0, 0, 0, 0};
    }
}

bool setVarDB(const std::string& key, const String& value) {
    auto it = varDB.find(key);
    if (it == varDB.end()) {
        varStruct newVar;
        newVar.value = value;
        newVar.changed = true;
        varDB[key] = newVar;
        return true;
    }

    if (it->second.value != value) {
        it->second.value = value;
        it->second.changed = true;
        return true;
    } else {
        return false;
    }
}

String getBaseName(const String& filename) {
    int lastDotIndex = filename.lastIndexOf('.');
    return lastDotIndex != -1 ? filename.substring(0, lastDotIndex) : filename;
}

void cleanupCurrent() {
    // clean unknown previews
    File dir = contentFS->open("/current");
    File file = dir.openNextFile();
    while (file) {
        String filename = file.name();
        uint8_t mac[8];
        if (hex2mac(getBaseName(filename), mac)) {
            bool found = false;
            for (tagRecord* record : tagDB) {
                if (memcmp(record->mac, mac, 8) == 0) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                filename = file.path();
                file.close();
                contentFS->remove(filename);
            }
        }
        file = dir.openNextFile();
    }
    dir.close();
}