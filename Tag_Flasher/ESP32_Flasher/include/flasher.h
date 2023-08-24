#include <Arduino.h>
#include <LittleFS.h>

void flashCountDown(uint8_t c);

#ifdef OPENEPAPERLINK_PCB
bool extTagConnected();
bool doTagFlash();
#endif

class flasher {
   public:
    class ZBS_interface *zbs = nullptr;
    uint8_t md5[16] = {0};
    char md5char[34];
    uint8_t tagtype;
    uint8_t *infoblock = nullptr;

    // Infoblock structure:
    // 0x00-0x0F - Calibration data
    // 0x10-0x17 - MAC
    // 0x19      - OpenEPaperLink Type
    // 0x30      - Original firmware MD5

    uint8_t mac[8] = {0};
    uint8_t mac_format = 0;
    uint16_t mac_suffix = 0;
    uint16_t mac_offset = 0;

    flasher();
    ~flasher();
    bool connectTag(uint8_t port);
    void getFirmwareMD5();
    bool getFirmwareMac();
    bool findTagByMD5();
    bool findTagByType(uint8_t type);
    bool getInfoBlockMD5();
    bool getInfoBlockMac();
    bool getInfoBlockType();
    void getMacFromWiFi();
    bool prepareInfoBlock();

    bool backupFlash();

    bool writeFlash(uint8_t *flashbuffer, uint16_t size);
    bool writeFlashFromPack(String filename, uint8_t type);
    bool writeFlashFromPackOffset(fs::File *file, uint16_t length);

    bool readBlock(uint16_t offset, uint8_t* data, uint16_t len, bool infopage);
    bool writeBlock(uint16_t offset, uint8_t* data, uint16_t len, bool infopage);

    bool readInfoBlock();
    bool writeInfoBlock();

   protected:
    bool writeBlock256(uint16_t offset, uint8_t *flashbuffer);
    void get_mac_format1();
    void get_mac_format2();
};
