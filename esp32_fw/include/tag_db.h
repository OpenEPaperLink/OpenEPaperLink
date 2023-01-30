#include <Arduino.h>

#include <vector>

#pragma pack(push, 1)
#pragma once

enum contentModes {
    Image,
    Today,
    CountDays,
    CountHours,
    Weather,
    PubTrans,
    Memo,
};

class tagRecord {
	public:
         tagRecord() : mac{0}, model(0), alias(""), lastseen(0), nextupdate(0), contentMode(Image), pending(false), button(false), currFilename(""), pendingFilename("") {}

         uint8_t mac[6];
		 u_int8_t model;
         String alias;
         uint32_t lastseen;
         uint32_t nextupdate;
         contentModes contentMode;
		 bool pending;
		 bool button;
         String currFilename;
         String pendingFilename;
         static tagRecord* findByMAC(uint8_t mac[6]);
};

extern std::vector<tagRecord*> tagDB;
String tagDBtoJson(uint8_t mac[6] = nullptr);