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

#define NO_SUBGHZ_CHANNEL  255
class tagRecord {
   public:
    tagRecord() : mac{0}, version(0), alias(""), lastseen(0), nextupdate(0), contentMode(0), pendingCount(0), md5{0}, expectedNextCheckin(0), modeConfigJson(""), LQI(0), RSSI(0), temperature(0), batteryMv(0), hwType(0), wakeupReason(0), capabilities(0), lastfullupdate(0), isExternal(false), apIp(IPAddress(0, 0, 0, 0)), pendingIdle(0), rotate(0), lut(0), tagSoftwareVersion(0), currentChannel(0), dataType(0), imageDataType(0), filename(""), data(nullptr), len(0), invert(0), updateCount(0), updateLast(0) {}

    uint8_t mac[8];
    uint8_t version;
    String alias;
    uint32_t lastseen;
    uint32_t nextupdate;
    uint8_t contentMode;
    uint16_t pendingCount;
    uint8_t md5[16];
    uint32_t expectedNextCheckin;
    String modeConfigJson;
    uint8_t LQI;
    int8_t RSSI;
    int8_t temperature;
    uint16_t batteryMv;
    uint16_t hwType;
    uint8_t wakeupReason;
    uint8_t capabilities;
    uint32_t lastfullupdate;
    bool isExternal;
    IPAddress apIp;
    uint16_t pendingIdle;
    uint8_t rotate;
    uint8_t lut;
    uint16_t tagSoftwareVersion;
    uint8_t currentChannel;
    uint8_t invert;
    uint32_t updateCount;
    uint32_t updateLast;

    uint8_t dataType;
    uint8_t imageDataType;  // data type of the last image sent, needed to resend /current/<mac>.raw
    String filename;
    uint8_t* data;
    uint32_t len;

    static tagRecord* findByMAC(const uint8_t mac[8]);
};

/* The build time pinout, used as the default for the flash-only pin settings.
   A board that does not have a given pin says -1, which every consumer already
   treats as "not present". Keeping the fallbacks here rather than at each use
   means an unset config entry and an old config file both behave exactly like
   the build did. */
#ifdef FLASHER_AP_SWS
#define FLASH_DEF_SWS FLASHER_AP_SWS
#else
#define FLASH_DEF_SWS (-1)
#endif
#ifdef FLASHER_AP_RESET
#define FLASH_DEF_RESET FLASHER_AP_RESET
#else
#define FLASH_DEF_RESET (-1)
#endif
#ifdef FLASHER_AP_TXD
#define FLASH_DEF_TXD FLASHER_AP_TXD
#else
#define FLASH_DEF_TXD (-1)
#endif
#ifdef FLASHER_AP_RXD
#define FLASH_DEF_RXD FLASHER_AP_RXD
#else
#define FLASH_DEF_RXD (-1)
#endif
#ifdef FLASHER_DEBUG_PROG
#define FLASH_DEF_PROG FLASHER_DEBUG_PROG
#else
#define FLASH_DEF_PROG (-1)
#endif
#ifdef FLASHER_DEBUG_TXD
#define FLASH_DEF_DBGTXD FLASHER_DEBUG_TXD
#else
#define FLASH_DEF_DBGTXD (-1)
#endif
#ifdef FLASHER_DEBUG_RXD
#define FLASH_DEF_DBGRXD FLASHER_DEBUG_RXD
#else
#define FLASH_DEF_DBGRXD (-1)
#endif

struct Config {
    uint8_t channel;
    uint8_t subghzchannel;
    char alias[32];
    uint8_t led;
    uint8_t tft;
    uint8_t language;
    uint8_t maxsleep;
    uint8_t stopsleep;
    volatile uint8_t runStatus;
    uint8_t preview;
    uint8_t nightlyreboot;
    uint8_t lock;
    uint8_t wifiPower;
    char timeZone[52];
    uint8_t sleepTime1;
    uint8_t sleepTime2;
    uint8_t ble;
    uint8_t discovery;
    String repo;
    String env;
    uint8_t showtimestamp;

    /* Pin numbers used while FLASHING a co-processor, and only then. Normal
       operation keeps the compile time pinout: the serial link to the radio,
       its reset line and the presence check in serialap.cpp all stay fixed,
       so a wrong value here can cost a flash attempt but never the running
       access point - and never the way back in.

       Signed, because -1 means "this board has no such pin". Defaults come
       from the build (see FLASH_PIN_DEFAULTS in tag_db.cpp), so an entry that
       was never set behaves exactly like before. */
    int8_t flashPinSws;
    int8_t flashPinReset;
    int8_t flashPinTxd;
    int8_t flashPinRxd;
    int8_t flashPinProg;
    int8_t flashPinDbgTxd;
    int8_t flashPinDbgRxd;
};

struct Color {
    uint8_t r, g, b;
    Color() : r(0), g(0), b(0) {}
    Color(uint16_t value_) : r((value_ >> 8) & 0xF8 | (value_ >> 13) & 0x07), g((value_ >> 3) & 0xFC | (value_ >> 9) & 0x03), b((value_ << 3) & 0xF8 | (value_ >> 2) & 0x07) {}
    Color(uint8_t r_, uint8_t g_, uint8_t b_) : r(r_), g(g_), b(b_) {}
};

// bpp normally contains the literal bits per pixel. Values 1 and 2 use OEPL's
// bit-plane layout, so packed two-bit palette indices need a distinct value.
constexpr uint8_t BPP_PACKED_2BIT = 5;

struct HwType {
    uint16_t id;
    uint16_t width;
    uint16_t height;
    uint8_t rotatebuffer;
    uint8_t bpp;
    uint8_t shortlut;
    uint8_t zlib;
    uint8_t g5;
    uint16_t highlightColor;
    std::vector<Color> colortable;
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
extern uint32_t getTagCount(uint32_t& timeoutcount, uint32_t& lowbattcount);
extern void mac2hex(const uint8_t* mac, char* hexBuffer);
extern bool hex2mac(const String& hexString, uint8_t* mac);
extern void clearPending(tagRecord* taginfo);
extern void initAPconfig();
extern void saveAPconfig();
extern HwType getHwType(const uint16_t id);

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

/// @brief Locks tagDB (the vector itself) for the lifetime of the object
/// @note Recursive. Keep it short, and never take it while holding fsMutex (code holding this lock may take fsMutex).
/// Deleted records stay allocated for a while (see freeDeletedRecords), so a tagRecord pointer obtained
/// under the lock stays valid for a short time after the lock is released.
class TagDBLock {
   public:
    TagDBLock();
    ~TagDBLock();
    TagDBLock(const TagDBLock&) = delete;
    TagDBLock& operator=(const TagDBLock&) = delete;
};

/// @brief Copy of the tagDB pointers, for loops that do real work per tag without holding the lock
extern std::vector<tagRecord*> tagDBSnapshot();
/// @brief Find the record for this mac, or add a new one
extern tagRecord* addRecord(const uint8_t mac[8]);
/// @brief Free records that were deleted some time ago. Call regularly from the loop task.
extern void freeDeletedRecords();

#pragma pack(pop)
