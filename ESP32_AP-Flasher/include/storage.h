#ifndef _DYN_STORAGE_H_
#define _DYN_STORAGE_H_

#include "FS.h"

#include <mutex>

#ifdef HAS_SDCARD
#ifndef SD_CARD_SDMMC

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
#endif

class DynStorage {
   public:
    DynStorage();
    void begin();
    void end();
    void listFiles();
    uint64_t freeSpace();
    uint64_t totalSpace();
    bool reserveSpace(uint64_t bytes, uint64_t margin);

   private:
    bool isInited;
    // last freeSpace() result minus what was reserved since then; UINT64_MAX until the first measurement
    uint64_t knownFree = UINT64_MAX;
    std::mutex knownFreeMutex;
};

extern SemaphoreHandle_t fsMutex;
extern DynStorage Storage;
extern fs::FS *contentFS;

/// @brief File for writing that takes fsMutex for each operation, instead of for as long as it is open
/// @note Use it when writing is interleaved with slow work (compression, network downloads), so other
/// tasks (like the web server, which has a 5 s watchdog) don't have to wait for fsMutex all that time.
class LockedFile : public Stream {
   public:
    LockedFile(const String &path, const char *mode) {
        xSemaphoreTake(fsMutex, portMAX_DELAY);
        _file = contentFS->open(path, mode);
        xSemaphoreGive(fsMutex);
    }
    ~LockedFile() { close(); }
    LockedFile(const LockedFile &) = delete;
    LockedFile &operator=(const LockedFile &) = delete;

    explicit operator bool() { return (bool)_file; }

    using Print::write;
    size_t write(uint8_t c) override { return write(&c, 1); }
    size_t write(const uint8_t *buffer, size_t size) override {
        if (!_file) return 0;
        xSemaphoreTake(fsMutex, portMAX_DELAY);
        const size_t written = _file.write(buffer, size);
        xSemaphoreGive(fsMutex);
        return written;
    }
    bool seek(uint32_t pos) {
        if (!_file) return false;
        xSemaphoreTake(fsMutex, portMAX_DELAY);
        const bool result = _file.seek(pos);
        xSemaphoreGive(fsMutex);
        return result;
    }
    void close() {
        if (!_file) return;
        xSemaphoreTake(fsMutex, portMAX_DELAY);
        _file.close();
        xSemaphoreGive(fsMutex);
    }

    // write-only: nothing to read
    int available() override { return 0; }
    int read() override { return -1; }
    int peek() override { return -1; }

   private:
    fs::File _file;
};
#ifndef SD_CARD_ONLY
extern void copyFile(File in, File out);
#endif

#endif


