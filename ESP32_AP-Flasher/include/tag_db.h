#include <Arduino.h>
#include <ArduinoJson.h>

#include <unordered_map>
#include <vector>

#pragma pack(push, 1)
#pragma once

#define RUNSTATUS_STOP 0
#define RUNSTATUS_PAUSE 1
#define RUNSTATUS_RUN 2
#define RUNSTATUS_INIT 3

class tagRecord {
   public:
    tagRecord() : mac{0}, version(0), alias(""), lastseen(0), nextupdate(0), contentMode(0), pending(false), md5{0}, md5pending{0}, expectedNextCheckin(0), modeConfigJson(""), LQI(0), RSSI(0), temperature(0), batteryMv(0), hwType(0), wakeupReason(0), capabilities(0), lastfullupdate(0), isExternal(false), apIp(IPAddress(0, 0, 0, 0)), pendingIdle(0), hasCustomLUT(false), rotate(0), lut(0), tagSoftwareVersion(0), currentChannel(0), dataType(0), filename(""), data(nullptr), len(0), invert(0) {}

    uint8_t mac[8];
    uint8_t version;
    String alias;
    uint32_t lastseen;
    uint32_t nextupdate;
    uint8_t contentMode;
    bool pending;
    uint8_t md5[16];
    uint8_t md5pending[16];
    uint32_t expectedNextCheckin;
    String modeConfigJson;
    uint8_t LQI;
    int8_t RSSI;
    int8_t temperature;
    uint16_t batteryMv;
    uint8_t hwType;
    uint8_t wakeupReason;
    uint8_t capabilities;
    uint32_t lastfullupdate;
    bool isExternal;
    IPAddress apIp;
    uint16_t pendingIdle;
    bool hasCustomLUT;
    uint8_t rotate;
    uint8_t lut;
    uint16_t tagSoftwareVersion;
    uint8_t currentChannel;
    uint8_t invert;

    uint8_t dataType;
    String filename;
    uint8_t* data;
    uint32_t len;

    static tagRecord* findByMAC(const uint8_t mac[8]);
};

struct Config {
    uint8_t channel;
    char alias[32];
    int16_t led;
    uint8_t tft;
    uint8_t language;
    uint8_t maxsleep;
    uint8_t stopsleep;
    uint8_t runStatus;
    uint8_t preview;
    uint8_t lock;
    uint8_t wifiPower;
    char timeZone[52];
    uint8_t sleepTime1;
    uint8_t sleepTime2;
    String repo;
    String env;
};

struct HwType {
    uint16_t width;
    uint16_t height;
    uint8_t rotatebuffer;
    uint8_t bpp;
    uint8_t shortlut;
};

struct varStruct {
    String value;
    bool changed;
};

extern Config config;
extern std::vector<tagRecord*> tagDB;
extern std::unordered_map<int, HwType> hwtype;
extern std::unordered_map<std::string, varStruct> varDB;
extern String tagDBtoJson(const uint8_t mac[8] = nullptr, uint8_t startPos = 0);
extern bool deleteRecord(const uint8_t mac[8], bool allVersions = true);
extern void fillNode(JsonObject& tag, const tagRecord* taginfo);
extern void saveDB(const String& filename);
extern bool loadDB(const String& filename);
extern void destroyDB();
extern uint32_t getTagCount();
extern uint32_t getTagCount(uint32_t& timeoutcount);
extern void mac2hex(const uint8_t* mac, char* hexBuffer);
extern bool hex2mac(const String& hexString, uint8_t* mac);
extern void clearPending(tagRecord* taginfo);
extern void initAPconfig();
extern void saveAPconfig();
extern HwType getHwType(const uint8_t id);
/// @brief Update a variable with the given key and value
///
/// @param key Variable key
/// @param value Variable value
/// @param notify Should the change be notified (true, default) or not (false)
/// @return true If variable was created/updated
/// @return false If not
extern bool setVarDB(const std::string& key, const String& value, const bool notify = true);
extern void cleanupCurrent();
extern void pushTagInfo(tagRecord* taginfo);
extern void popTagInfo(const uint8_t mac[8] = nullptr);

#pragma pack(pop)
