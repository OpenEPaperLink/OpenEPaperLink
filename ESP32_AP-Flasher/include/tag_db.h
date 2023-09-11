#include <Arduino.h>
#include <ArduinoJson.h>

#include <unordered_map>
#include <vector>

#pragma pack(push, 1)
#pragma once

#define WAKEUP_REASON_TIMED 0
#define WAKEUP_REASON_BOOTUP 1
#define WAKEUP_REASON_GPIO 2
#define WAKEUP_REASON_NFC 3
#define WAKEUP_REASON_BUTTON1 4
#define WAKEUP_REASON_BUTTON2 5

#define RUNSTATUS_STOP 0
#define RUNSTATUS_PAUSE 1
#define RUNSTATUS_RUN 2
#define RUNSTATUS_INIT 3

class tagRecord {
   public:
    tagRecord() : mac{0}, alias(""), lastseen(0), nextupdate(0), contentMode(0), pending(false), md5{0}, md5pending{0}, expectedNextCheckin(0), modeConfigJson(""), LQI(0), RSSI(0), temperature(0), batteryMv(0), hwType(0), wakeupReason(0), capabilities(0), lastfullupdate(0), isExternal(false), pendingIdle(0), hasCustomLUT(false), rotate(0), lut(0), tagSoftwareVersion(0), currentChannel(0), dataType(0), filename(""), data(nullptr), len(0) {}

    uint8_t mac[8];
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
    uint16_t pendingIdle;
    bool hasCustomLUT;
    uint8_t rotate;
    uint8_t lut;
    uint16_t tagSoftwareVersion;
    uint8_t currentChannel;

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
    uint8_t language;
    uint8_t maxsleep;
    uint8_t stopsleep;
    uint8_t runStatus;
    uint8_t preview;
    uint8_t wifiPower;
    char timeZone[52];
    uint8_t sleepTime1;
    uint8_t sleepTime2;
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

// extern SemaphoreHandle_t tagDBOwner;
extern Config config;
extern std::vector<tagRecord*> tagDB;
extern std::unordered_map<int, HwType> hwtype;
extern std::unordered_map<std::string, varStruct> varDB;
extern DynamicJsonDocument APconfig;
extern String tagDBtoJson(const uint8_t mac[8] = nullptr, uint8_t startPos = 0);
extern bool deleteRecord(const uint8_t mac[8]);
extern void fillNode(JsonObject& tag, const tagRecord* taginfo);
extern void saveDB(const String& filename);
extern void loadDB(const String& filename);
extern void destroyDB();
extern uint32_t getTagCount();
extern uint32_t getTagCount(uint32_t& timeoutcount);
extern void mac2hex(const uint8_t* mac, char* hexBuffer);
extern bool hex2mac(const String& hexString, uint8_t* mac);
extern void clearPending(tagRecord* taginfo);
extern void initAPconfig();
extern void saveAPconfig();
extern HwType getHwType(const uint8_t id);
extern bool setVarDB(const std::string& key, const String& value);
extern void cleanupCurrent();

#pragma pack(pop)
