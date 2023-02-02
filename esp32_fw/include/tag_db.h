#include <Arduino.h>
#include <ArduinoJson.h>

#include <vector>

#pragma pack(push, 1)
#pragma once

enum contentModes {
    Image,
    Today,
    CountDays,
    CountHours,
    Weather,
    Firmware,
    Memo,
    ImageUrl,
};

class tagRecord {
   public:
    uint16_t nextCheckinpending;
    tagRecord() : mac{0}, model(0), alias(""), lastseen(0), nextupdate(0), contentMode(Image), pending(false), button(false), md5{0}, md5pending{0}, CheckinInMinPending(0), expectedNextCheckin(0), modeConfigJson("") {}

    uint8_t mac[6];
    u_int8_t model;
    String alias;
    uint32_t lastseen;
    uint32_t nextupdate;
    contentModes contentMode;
    bool pending;
    bool button;
    uint8_t md5[16];
    uint8_t md5pending[16];
    uint16_t CheckinInMinPending;
    uint32_t expectedNextCheckin;
    String modeConfigJson;
    static tagRecord* findByMAC(uint8_t mac[6]);
};

extern std::vector<tagRecord*> tagDB;
String tagDBtoJson(uint8_t mac[6] = nullptr, uint8_t startPos = 0);
void fillNode(JsonObject &tag, tagRecord* &taginfo);
void saveDB(String filename);
void loadDB(String filename);

#pragma pack(pop)