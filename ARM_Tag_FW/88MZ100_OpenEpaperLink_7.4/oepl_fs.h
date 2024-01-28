#pragma once

#include <stdint.h>
#include <vector>

extern "C" {
extern __attribute__((section(".aonshadow"))) uint32_t fsEntry;
extern __attribute__((section(".aonshadow"))) uint32_t fsEnd;
}
#define OEPLFS_CACHE_SIZE 128

#define FILENAME_LENGTH 32

typedef struct __attribute__((packed)) {
    char name[FILENAME_LENGTH];
    uint32_t offset;
    uint32_t len;
} OEPLFSFile;

class OEPLFile {
   public:
    ~OEPLFile();
    OEPLFile(uint32_t begin);
    uint32_t length;
    uint8_t operator()(uint32_t i);
    uint8_t readByte(uint32_t i);
    void getBlock(uint32_t offset, uint8_t* target, uint32_t len);

   protected:
    uint8_t* buffer = nullptr;
    uint32_t bufferBegin = 0;
    uint32_t eepromOffset = 0;
};

class OEPLFs {
   public:
    ~OEPLFs();
    OEPLFs();
    std::vector<OEPLFSFile*> files;
    OEPLFile* getFile(char* name);
    void init();
    bool isStarted = false;

   protected:
    bool silent;
    uint32_t findEntry();
    void populateFiles();
};

// extern __attribute__((section(".aonshadow"))) OEPLFs fs;
extern OEPLFs* fs;