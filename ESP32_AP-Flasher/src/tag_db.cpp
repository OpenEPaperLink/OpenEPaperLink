#include "tag_db.h"
#include "od_session.h"

#include <Arduino.h>
#include <ArduinoJson.h>
#include <FS.h>

#include <unordered_map>
#include <vector>

#include "language.h"
#include "radiotransport.h"
#include "storage.h"
#include "util.h"

#define STR_IMPL(x) #x
#define STR(x) STR_IMPL(x)

std::vector<tagRecord*> tagDB;
std::unordered_map<std::string, varStruct> varDB;
std::unordered_map<int, HwType> hwdata = {};

Config config;

// tagDB is used from the loop task, the web server (async_tcp) and the AP/BLE tasks
static SemaphoreHandle_t tagDBMutex = xSemaphoreCreateRecursiveMutex();

TagDBLock::TagDBLock() {
    xSemaphoreTakeRecursive(tagDBMutex, portMAX_DELAY);
}

TagDBLock::~TagDBLock() {
    xSemaphoreGiveRecursive(tagDBMutex);
}

// Records removed from tagDB are kept here for a while before they are freed, because other tasks
// may still hold a pointer to them (from findByMAC or a snapshot). Guarded by tagDBMutex.
struct DeletedRecord {
    tagRecord* tag;
    uint32_t deletedAt;
};
static std::vector<DeletedRecord> deletedRecords;
static constexpr uint32_t DELETED_RECORD_GRACE_MS = 30000;

std::vector<tagRecord*> tagDBSnapshot() {
    TagDBLock lock;
    return tagDB;
}

/// @brief True if another record (live or deleted) uses the same pending data buffer
static bool isDataShared(const tagRecord* taginfo) {
    for (const tagRecord* tag : tagDB) {
        if (tag != taginfo && tag->data == taginfo->data) return true;
    }
    for (const DeletedRecord& deleted : deletedRecords) {
        if (deleted.tag != taginfo && deleted.tag->data == taginfo->data) return true;
    }
    return false;
}

/// @brief Free the pending data buffer of a record, unless another record still uses it
static void releaseData(tagRecord* taginfo) {
    if (taginfo->data != nullptr && !isDataShared(taginfo)) {
        free(taginfo->data);
    }
    taginfo->data = nullptr;
}

tagRecord* tagRecord::findByMAC(const uint8_t mac[8]) {
    TagDBLock lock;
    for (tagRecord* tag : tagDB) {
        if (memcmp(tag->mac, mac, 8) == 0 && tag->version == 0) {
            return tag;
        }
    }
    return nullptr;
}

tagRecord* addRecord(const uint8_t mac[8]) {
    TagDBLock lock;
    tagRecord* taginfo = tagRecord::findByMAC(mac);
    if (taginfo == nullptr) {
        taginfo = new tagRecord;
        memcpy(taginfo->mac, mac, sizeof(taginfo->mac));
        tagDB.push_back(taginfo);
        radioAllowListChanged(mac, true);
    }
    return taginfo;
}

bool deleteRecord(const uint8_t mac[8], bool allVersions) {
    TagDBLock lock;
    for (uint32_t c = 0; c < tagDB.size(); c++) {
        tagRecord* tag = tagDB.at(c);
        if (memcmp(tag->mac, mac, 8) == 0 && (allVersions || tag->version == 0)) {
            tagDB.erase(tagDB.begin() + c);
            releaseData(tag);
            deletedRecords.push_back({tag, millis()});
            if (tagRecord::findByMAC(mac) == nullptr) radioAllowListChanged(mac, false);
            return true;
        }
    }
    return false;
}

void freeDeletedRecords() {
    TagDBLock lock;
    for (size_t c = 0; c < deletedRecords.size();) {
        if (millis() - deletedRecords[c].deletedAt > DELETED_RECORD_GRACE_MS) {
            tagRecord* tag = deletedRecords[c].tag;
            deletedRecords.erase(deletedRecords.begin() + c);
            releaseData(tag);  // it may have got new data while it was waiting here
            delete tag;
        } else {
            c++;
        }
    }
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
    JsonDocument doc;
    JsonArray tags = doc["tags"].to<JsonArray>();

    TagDBLock lock;
    for (uint32_t c = startPos; c < tagDB.size(); ++c) {
        const tagRecord* taginfo = tagDB.at(c);

        const bool select = !mac || memcmp(taginfo->mac, mac, 8) == 0;
        if (select && taginfo->version == 0) {
            JsonObject tag = tags.add<JsonObject>();
            fillNode(tag, taginfo);
#ifdef HAS_BLE_WRITER
            if (bleFamily(taginfo->mac, taginfo->hwType) == BLE_FAMILY_OPENDISPLAY) odDescribe(taginfo->mac, tag["od"].to<JsonObject>());
#endif
            if (measureJson(doc) > 5000) {
                doc["continu"] = c + 1;
                break;
            }
            if (mac) {
                break;
            }
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
    tag["pending"] = taginfo->pendingCount;
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
    tag["apip"] = taginfo->apIp.toString();
    tag["rotate"] = taginfo->rotate;
    tag["lut"] = taginfo->lut;
    tag["invert"] = taginfo->invert;
    tag["updatecount"] = taginfo->updateCount;
    tag["updatelast"] = taginfo->updateLast;
    tag["ch"] = taginfo->currentChannel;
    tag["ver"] = taginfo->tagSoftwareVersion;
    tag["imgtype"] = taginfo->imageDataType;
}

void saveDB(const String& filename) {
    JsonDocument doc;

    const long t = millis();
    const std::vector<tagRecord*> tags = tagDBSnapshot();

    xSemaphoreTake(fsMutex, portMAX_DELAY);

    if (contentFS->exists(filename)) {
        String backupFilename = filename + ".bak";
        if (!contentFS->rename(filename.c_str(), backupFilename.c_str())) {
            xSemaphoreGive(fsMutex);
            logLine("error renaming tagDB to .bak");
            wsErr("error renaming tagDB to .bak");
            xSemaphoreTake(fsMutex, portMAX_DELAY);
        }
    }

    fs::File file = contentFS->open(filename, "w");
    if (!file) {
        Serial.println("saveDB: Failed to open file for writing");
        xSemaphoreGive(fsMutex);
        return;
    }

    file.write('[');
    for (size_t c = 0; c < tags.size(); c++) {
        const tagRecord* taginfo = tags.at(c);
        doc.clear();

        if (taginfo->version == 0) {
            JsonObject tag = doc.add<JsonObject>();
            fillNode(tag, taginfo);
            if (c > 0) {
                file.write(',');
            }
            serializeJsonPretty(doc, file);
        }
    }
    file.write(']');

    file.close();
    xSemaphoreGive(fsMutex);
    Serial.println("DB saved " + String(millis() - t) + "ms");
}

bool loadDB(const String& filename) {
    Serial.println("reading DB from " + String(filename));
    const long t = millis();

    fs::File readfile = contentFS->open(filename, "r");
    if (!readfile) {
        Serial.println("loadDB: Failed to open file");
        return false;
    }

    time_t now;
    time(&now);
    bool parsing = true;

    if (readfile.find("[")) {
        JsonDocument doc;
        while (parsing) {
            DeserializationError err = deserializeJson(doc, readfile);
            if (!err) {
                JsonObject tag = doc[0];
                String dst = tag["mac"].as<String>();
                uint8_t mac[8];
                if (hex2mac(dst, mac)) {
                    tagRecord* taginfo = addRecord(mac);
                    String md5 = tag["hash"].as<String>();
                    if (md5.length() >= 32) {
                        for (uint8_t i = 0; i < 16; i++) {
                            taginfo->md5[i] = strtoul(md5.substring(i * 2, i * 2 + 2).c_str(), NULL, 16);
                        }
                    }
                    taginfo->lastseen = (uint32_t)tag["lastseen"];
                    taginfo->nextupdate = (uint32_t)tag["nextupdate"];
                    taginfo->expectedNextCheckin = (uint32_t)tag["nextcheckin"];
                    if (taginfo->expectedNextCheckin < now) {
                        taginfo->expectedNextCheckin = now + 60;
                    }
                    taginfo->pendingCount = 0;
                    taginfo->alias = tag["alias"].as<String>();
                    taginfo->contentMode = tag["contentMode"];
                    taginfo->LQI = tag["LQI"];
                    taginfo->RSSI = tag["RSSI"];
                    taginfo->temperature = tag["temperature"];
                    taginfo->batteryMv = tag["batteryMv"];
                    taginfo->hwType = (uint16_t)tag["hwType"];
                    taginfo->wakeupReason = tag["wakeupReason"];
                    taginfo->capabilities = tag["capabilities"];
                    taginfo->modeConfigJson = tag["modecfgjson"].as<String>();
                    taginfo->isExternal = tag["isexternal"].as<bool>();
                    taginfo->apIp.fromString(tag["apip"].as<String>());
                    taginfo->rotate = tag["rotate"] | 0;
                    taginfo->lut = tag["lut"] | 0;
                    taginfo->invert = tag["invert"] | 0;
                    taginfo->updateCount = tag["updatecount"] | 0;
                    taginfo->updateLast = tag["updatelast"] | 0;
                    taginfo->currentChannel = tag["ch"] | 0;
                    taginfo->tagSoftwareVersion = tag["ver"] | 0;
                    taginfo->imageDataType = tag["imgtype"] | 0;
                }
            } else {
                Serial.print(F("deserializeJson() failed: "));
                Serial.println(err.c_str());
                parsing = false;
                readfile.close();
                return false;
            }
            parsing = parsing && readfile.find(",");
        }
    } else {
        readfile.close();
        return false;
    }

    readfile.close();
    Serial.println("loadDB took " + String(millis() - t) + "ms");
    return true;
}

void destroyDB() {
    Serial.println("destroying DB");
    util::printHeap();
    TagDBLock lock;
    while (!tagDB.empty()) {
        tagRecord* tag = tagDB.back();
        tagDB.pop_back();
        releaseData(tag);
        deletedRecords.push_back({tag, millis()});
    }
    util::printHeap();
}

uint32_t getTagCount() {
    uint32_t temp = 0;
    return getTagCount(temp, temp);
}

uint32_t getTagCount(uint32_t& timeoutcount, uint32_t& lowbattcount) {
    uint32_t tagcount = 0;
    time_t now;
    time(&now);
    TagDBLock lock;
    for (const tagRecord* taginfo : tagDB) {
        if (!taginfo->isExternal) tagcount++;
        const int32_t timeout = now - taginfo->lastseen;
        if (taginfo->expectedNextCheckin < 3600) {
            // not initialised, timeout if not seen last 5 minutes
            if (timeout > config.maxsleep * 60 + 300) timeoutcount++;
        } else if (now - static_cast<time_t>(taginfo->expectedNextCheckin) > 600) {
            // expected checkin is behind, timeout if not seen last 5 minutes
            if (timeout > config.maxsleep * 60 + 300) timeoutcount++;
        }
        if (taginfo->batteryMv < 2400 && taginfo->batteryMv != 0 && taginfo->batteryMv != 1337) lowbattcount++;
    }
    return tagcount;
}

void clearPending(tagRecord* taginfo) {
    taginfo->filename = String();
    TagDBLock lock;
    releaseData(taginfo);
}

void initAPconfig() {
    JsonDocument APconfig;
    File configFile = contentFS->open("/current/apconfig.json", "r");
    const bool firstStart = !configFile;
    if (configFile) {
        DeserializationError error = deserializeJson(APconfig, configFile);
        if (error) {
            configFile.close();
            Serial.println("failed to read apconfig.json. Using default config");
            Serial.println(error.c_str());
        }
        configFile.close();
    }
    config.channel = APconfig["channel"].is<uint8_t>() ? APconfig["channel"] : 0;
    config.subghzchannel = APconfig["subghzchannel"].is<uint8_t>() ? APconfig["subghzchannel"] : 0;
    if (APconfig["alias"]) strlcpy(config.alias, APconfig["alias"], sizeof(config.alias));
    config.led = APconfig["led"].is<uint8_t>() ? APconfig["led"] : 255;
    config.tft = APconfig["tft"].is<uint8_t>() ? APconfig["tft"] : 255;
    config.language = APconfig["language"].is<uint8_t>() ? APconfig["language"] : 0;
    config.maxsleep = APconfig["maxsleep"].is<uint8_t>() ? APconfig["maxsleep"] : 10;
    config.stopsleep = APconfig["stopsleep"].is<uint8_t>() ? APconfig["stopsleep"] : 1;
    config.preview = APconfig["preview"].is<uint8_t>() ? APconfig["preview"] : 1;
    config.nightlyreboot = APconfig["nightlyreboot"].is<uint8_t>() ? APconfig["nightlyreboot"] : 1;
    config.lock = APconfig["lock"].is<uint8_t>() ? APconfig["lock"] : 0;
    config.sleepTime1 = APconfig["sleeptime1"].is<uint8_t>() ? APconfig["sleeptime1"] : 0;
    config.sleepTime2 = APconfig["sleeptime2"].is<uint8_t>() ? APconfig["sleeptime2"] : 0;
    config.ble = APconfig["ble"].is<uint8_t>() ? APconfig["ble"] : 0;
    config.discovery = APconfig["discovery"].is<uint8_t>() ? APconfig["discovery"] : 0;
    config.showtimestamp = APconfig["showtimestamp"].is<uint8_t>() ? APconfig["showtimestamp"] : 0;
    config.flashPinSws    = APconfig["flashpin_sws"].is<int8_t>()    ? APconfig["flashpin_sws"].as<int8_t>()    : (int8_t)FLASH_DEF_SWS;
    config.flashPinReset  = APconfig["flashpin_reset"].is<int8_t>()  ? APconfig["flashpin_reset"].as<int8_t>()  : (int8_t)FLASH_DEF_RESET;
    config.flashPinTxd    = APconfig["flashpin_txd"].is<int8_t>()    ? APconfig["flashpin_txd"].as<int8_t>()    : (int8_t)FLASH_DEF_TXD;
    config.flashPinRxd    = APconfig["flashpin_rxd"].is<int8_t>()    ? APconfig["flashpin_rxd"].as<int8_t>()    : (int8_t)FLASH_DEF_RXD;
    config.flashPinProg   = APconfig["flashpin_prog"].is<int8_t>()   ? APconfig["flashpin_prog"].as<int8_t>()   : (int8_t)FLASH_DEF_PROG;
    config.flashPinDbgTxd = APconfig["flashpin_dbgtxd"].is<int8_t>() ? APconfig["flashpin_dbgtxd"].as<int8_t>() : (int8_t)FLASH_DEF_DBGTXD;
    config.flashPinDbgRxd = APconfig["flashpin_dbgrxd"].is<int8_t>() ? APconfig["flashpin_dbgrxd"].as<int8_t>() : (int8_t)FLASH_DEF_DBGRXD;
#ifdef BLE_ONLY
        config.ble = true;
#endif
    // default wifi power 8.5 dbM
    // see https://github.com/espressif/arduino-esp32/blob/master/libraries/WiFi/src/WiFiGeneric.h#L111
    config.wifiPower = APconfig["wifipower"].is<uint8_t>() ? APconfig["wifipower"] : 34;
    config.repo = APconfig["repo"].is<String>() ? APconfig["repo"].as<String>() : String("OpenEPaperLink/OpenEPaperLink");
    config.env = APconfig["env"].is<String>() ? APconfig["env"].as<String>() : String(STR(BUILD_ENV_NAME));
    if (APconfig["timezone"]) {
        strlcpy(config.timeZone, APconfig["timezone"], sizeof(config.timeZone));
    } else {
        strlcpy(config.timeZone, "CET-1CEST,M3.5.0,M10.5.0/3", sizeof(config.timeZone));
    }
    // A new AP has no settings file until they are first saved. The web UI reads its settings
    // from that file (/get_ap_config), so without it it saw none, and hid the previews.
    if (firstStart) saveAPconfig();
}

void saveAPconfig() {
    xSemaphoreTake(fsMutex, portMAX_DELAY);
    fs::File configFile = contentFS->open("/current/apconfig.json", "w");
    JsonDocument APconfig;
    APconfig["channel"] = config.channel;
    APconfig["subghzchannel"] = config.subghzchannel;
    APconfig["alias"] = config.alias;
    APconfig["led"] = config.led;
    APconfig["tft"] = config.tft;
    APconfig["language"] = config.language;
    APconfig["maxsleep"] = config.maxsleep;
    APconfig["stopsleep"] = config.stopsleep;
    APconfig["preview"] = config.preview;
    APconfig["nightlyreboot"] = config.nightlyreboot;
    APconfig["lock"] = config.lock;
    APconfig["wifipower"] = config.wifiPower;
    APconfig["timezone"] = config.timeZone;
    APconfig["sleeptime1"] = config.sleepTime1;
    APconfig["sleeptime2"] = config.sleepTime2;
    APconfig["flashpin_sws"] = config.flashPinSws;
    APconfig["flashpin_reset"] = config.flashPinReset;
    APconfig["flashpin_txd"] = config.flashPinTxd;
    APconfig["flashpin_rxd"] = config.flashPinRxd;
    APconfig["flashpin_prog"] = config.flashPinProg;
    APconfig["flashpin_dbgtxd"] = config.flashPinDbgTxd;
    APconfig["flashpin_dbgrxd"] = config.flashPinDbgRxd;
    APconfig["ble"] = config.ble;
    APconfig["repo"] = config.repo;
    APconfig["env"] = config.env;
    APconfig["discovery"] = config.discovery;
    APconfig["showtimestamp"] = config.showtimestamp;
    serializeJsonPretty(APconfig, configFile);
    configFile.close();
    xSemaphoreGive(fsMutex);
}

void forgetHwType(const uint16_t id) {
    hwdata.erase(id);
}

HwType getHwType(const uint16_t id) {
    auto it = hwdata.find(id);
    if (it != hwdata.end()) {
        return it->second;
    } else {
        char filename[20];
        snprintf(filename, sizeof(filename), id > 0xFF ? "/tagtypes/%04X.json" : "/tagtypes/%02X.json", id);
        Serial.printf("read %s\r\n", filename);
        File jsonFile = contentFS->open(filename, "r");

        if (jsonFile) {
            JsonDocument filter;
            filter["width"] = true;
            filter["height"] = true;
            filter["rotatebuffer"] = true;
            filter["bpp"] = true;
            filter["shortlut"] = true;
            filter["zlib_compression"] = true;
            filter["g5_compression"] = true;
            filter["highlight_color"] = true;
            filter["colortable"] = true;
            JsonDocument doc;
            DeserializationError error = deserializeJson(doc, jsonFile, DeserializationOption::Filter(filter));
            jsonFile.close();
            if (error) {
                Serial.println("json error in " + String(filename));
                Serial.println(error.c_str());
            } else {
                HwType& hwType = hwdata[id];
                hwType.id = id;
                hwType.width = doc["width"];
                hwType.height = doc["height"];
                hwType.rotatebuffer = doc["rotatebuffer"];
                hwType.bpp = doc["bpp"];
                hwType.shortlut = doc["shortlut"];
                if (doc["zlib_compression"].is<const char*>()) {
                    hwType.zlib = strtol(doc["zlib_compression"], nullptr, 16);
                } else {
                    hwType.zlib = 0;
                }
                if (doc["g5_compression"].is<const char*>()) {
                    hwType.g5 = strtol(doc["g5_compression"], nullptr, 16);
                } else {
                    hwType.g5 = 0;
                }
                hwType.highlightColor = doc["highlight_color"].is<uint16_t>() ? doc["highlight_color"].as<uint16_t>() : 2;
                JsonObject colorTable = doc["colortable"];
                for (auto kv : colorTable) {
                    JsonArray color = kv.value();
                    Color c;
                    c.r = color[0];
                    c.g = color[1];
                    c.b = color[2];
                    hwType.colortable.push_back(c);
                }
                return hwdata.at(id);
            }
        }
        return {0, 0, 0, 0, 0, 0, 0};
    }
}

bool setVarDB(const std::string& key, const String& value, const bool notify) {
    auto it = varDB.find(key);
    if (it == varDB.end()) {
        varStruct newVar;
        newVar.value = value;
        newVar.changed = notify;
        varDB[key] = newVar;
        return true;
    }

    if (it->second.value != value) {
        it->second.value = value;
        it->second.changed = notify;
        return true;
    } else {
        return false;
    }
}

String getBaseName(const String& filename) {
    // int lastDotIndex = filename.lastIndexOf('.');
    // return lastDotIndex != -1 ? filename.substring(0, lastDotIndex) : filename;
    return filename.substring(0, 16);
}

void cleanupCurrent() {
    // clean unknown previews
    Serial.println("Cleaning up temporary files");
    // getNextFileName() only reads the directory; openNextFile() would also stat and open every file
    File dir = contentFS->open("/current");
    String path = dir.getNextFileName();
    while (path.length()) {
        const String filename = path.substring(path.lastIndexOf('/') + 1);
        uint8_t mac[8];
        if (hex2mac(getBaseName(filename), mac)) {
            bool found = false;
            TagDBLock lock;
            for (tagRecord* record : tagDB) {
                if (memcmp(record->mac, mac, 8) == 0) {
                    found = true;
                    break;
                }
            }
            if (!found || filename.endsWith(".pending")) {
                Serial.println("remove " + path);
                contentFS->remove(path);
            }
        }
        path = dir.getNextFileName();
    }
    dir.close();

    dir = contentFS->open("/temp");
    path = dir.getNextFileName();
    while (path.length()) {
        contentFS->remove(path);
        path = dir.getNextFileName();
    }
    dir.close();
}

static tagRecord* findSavedTagInfo(const uint8_t mac[8]) {
    TagDBLock lock;
    for (tagRecord* tag : tagDB) {
        if (memcmp(tag->mac, mac, 8) == 0 && tag->version == 1) return tag;
    }
    return nullptr;
}

void pushTagInfo(tagRecord* taginfo) {
    TagDBLock lock;
    // keep the first saved copy: that one holds the settings to restore
    if (findSavedTagInfo(taginfo->mac) != nullptr) return;
    tagRecord* taginfo2 = new tagRecord(*taginfo);
    taginfo2->version = 1;
    // the pending data buffer stays owned by the original record (it is freed in deleteRecord)
    taginfo2->data = nullptr;
    taginfo2->len = 0;
    tagDB.push_back(taginfo2);
}

void popTagInfo(const uint8_t mac[8]) {
    TagDBLock lock;
    // find first, then delete: deleteRecord() erases from tagDB, so it can't be called while iterating over it
    tagRecord* saved = findSavedTagInfo(mac);
    if (saved == nullptr) return;
    deleteRecord(mac, false);
    saved->version = 0;
    radioAllowListChanged(mac, true);
}
