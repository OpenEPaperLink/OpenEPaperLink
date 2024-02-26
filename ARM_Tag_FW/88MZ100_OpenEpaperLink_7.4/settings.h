#ifndef SETTINGS_H
#define SETTINGS_H

#include <stdint.h>

#define FW_VERSION 0x0027          // version number (max 2.5.5 :) )
#define FW_VERSION_SUFFIX "-zlib"  // suffix, like -RC1 or whatever.

// #define DEBUGBLOCKS                 // uncomment to enable extra debug information on the block transfers
// #define PRINT_LUT                   // uncomment if you want the tag to print the LUT for the current temperature bracket

// #define DEBUG_EPD
// #define DEBUG_FS
// #define DEBUG_COMPRESSION
// #define DEBUG_PROTO
// #define DEBUG_SETTINGS
// #define DEBUG_MAIN
// #define DEBUG_FAST_CHECK_IN
// #define DEBUG_DONTVALIDATEPROTO

#if defined(DEBUG_SETTINGS) || defined(DEBUG_EPD) || defined(DEBUGBLOCKS) || defined(DEBUG_PROTO) || defined(DEBUG_COMPRESSION) || defined(DEBUG_FS) || defined(DEBUG_MAIN) || defined(DEBUG_FAST_CHECK_IN)
#define DEBUG_BUILD
#endif

#define SETTINGS_STRUCT_VERSION 0x01

#define DEFAULT_SETTING_FASTBOOT 0
#define DEFAULT_SETTING_RFWAKE 0
#define DEFAULT_SETTING_TAGROAMING 0
#define DEFAULT_SETTING_SCANFORAP 1
#define DEFAULT_SETTING_LOWBATSYMBOL 1
#define DEFAULT_SETTING_NORFSYMBOL 1

#define MAGIC_NUMBER_SETTINGS (0xD34DBEEFD0D0CAFEull)

#include "tagprofile_struct.h"

extern __attribute__((section(".aonshadow"))) struct tagsettings tagSettings;
extern __attribute__((section(".aonshadow"))) struct tagHardwareProfile tagProfile;

void invalidateSettings();
void loadDefaultSettings();
void writeSettings();
bool loadSettings();
void loadSettingsFromBuffer(uint8_t* p);
bool loadProfileFromFile(char* filename);
#endif