#include <Arduino.h>

#include <vector>

#pragma pack(push, 1)
#pragma once

class pendingdata {
   public:
    String filename;
    uint64_t ver;
    uint32_t timeout;
    uint8_t datatimeout;
    uint8_t* data = nullptr;
    uint32_t len;
    static pendingdata* findByVer(uint64_t ver);
    static void garbageCollection();
};

void garbageCollection(void* parameter);
extern std::vector<pendingdata*> pendingfiles;

#pragma pack(pop)