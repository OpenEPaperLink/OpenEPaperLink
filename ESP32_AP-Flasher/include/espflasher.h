#include <Arduino.h>
#include <LittleFS.h>

#if defined HAS_H2
   #define SHORT_CHIP_NAME "H2"
   #define OTA_BIN_DIR     "ESP32-H2"
   #define ESP_CHIP_TYPE   ESP32H2_CHIP
#elif defined HAS_ELECROW_C6
   #define SHORT_CHIP_NAME "ELECROW_C6"
   #define OTA_BIN_DIR     "ESP32-C6"
   #define ESP_CHIP_TYPE   ESP32C6_CHIP
#elif defined C6_OTA_FLASHING
   #define SHORT_CHIP_NAME "C6"
   #define OTA_BIN_DIR     "ESP32-C6"
   #define ESP_CHIP_TYPE   ESP32C6_CHIP
#elif defined HAS_TSLR
   /* Last in the chain on purpose. A board can carry either module, and these
      names belong to the serial loader path - letting a declared TLSR consume
      them would leave a fitted C6 unflashable. The TLSR path does not use them
      at all; it is written over SWS and gets its directory from the caller.
      ESP_CHIP_TYPE stays undefined here because a TLSR is not an ESP. */
   #define SHORT_CHIP_NAME "TSLR"
   #define OTA_BIN_DIR     "TLSR"
#endif

/* Which firmware manifest to fetch. Normally that is the one named after the
   chip, but a board whose co-processor hangs on a single shared UART needs a
   different build of the C6 firmware - one that talks over UART0 instead of a
   second port. That is a property of the wiring, not of the chip, so it keys
   off FLASHER_DEBUG_SHARED and only the manifest name changes: SHORT_CHIP_NAME
   still says "C6" everywhere it is shown to a user. The UART0 build only exists
   for the C6: a board with an H2 on a shared UART (LilyGo T-Panel) uses the
   normal H2 manifest (#621). */
#if defined(C6_OTA_FLASHING) && defined(FLASHER_DEBUG_SHARED) && !defined(HAS_H2)
   #define OTA_FW_JSON "C6_Uart0"
#elif defined(SHORT_CHIP_NAME)
   #define OTA_FW_JSON SHORT_CHIP_NAME
#endif

bool downloadAndWriteBinary(String &filename, const char *url);
bool FlashC6_H2(const char *Url);
