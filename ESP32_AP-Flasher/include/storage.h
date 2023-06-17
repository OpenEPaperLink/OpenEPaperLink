#ifndef _DYN_STORAGE_H_
#define _DYN_STORAGE_H_

#include "FS.h"

#ifdef HAS_SDCARD
#ifndef SD_CARD_SS
#error SD_CARD_SS UNDEFINED
#endif

#ifndef SD_CARD_CLK
#define SD_CARD_CLK 18
#endif

#ifndef SD_CARD_MISO
#define SD_CARD_MISO 19
#endif

#ifndef SD_CARD_MOSI
#define SD_CARD_MOSI 23
#endif
#endif

class DynStorage {
   public:
    DynStorage();
    void begin();
    void listFiles();

   private:
    bool isInited;
};

extern DynStorage Storage;
extern fs::FS *contentFS;

#endif