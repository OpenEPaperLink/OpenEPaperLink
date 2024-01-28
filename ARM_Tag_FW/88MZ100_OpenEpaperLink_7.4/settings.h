#ifndef SETTINGS_H
#define SETTINGS_H

#include <stdint.h>

#define FW_VERSION 0x0027            // version number (max 2.5.5 :) )
#define FW_VERSION_SUFFIX "-75"  // suffix, like -RC1 or whatever.
// #define DEBUGBLOCKS                 // uncomment to enable extra debug information on the block transfers
// #define PRINT_LUT                   // uncomment if you want the tag to print the LUT for the current temperature bracket


#define DEBUG_EPD
#define DEBUG_FS
#define DEBUG_BUILD
#define DEBUG_COMPRESSION


#define SETTINGS_STRUCT_VERSION 0x01

#define DEFAULT_SETTING_FASTBOOT 0
#define DEFAULT_SETTING_RFWAKE 0
#define DEFAULT_SETTING_TAGROAMING 0
#define DEFAULT_SETTING_SCANFORAP 1
#define DEFAULT_SETTING_LOWBATSYMBOL 1
#define DEFAULT_SETTING_NORFSYMBOL 1

extern __attribute__((section(".aonshadow")))struct tagsettings tagSettings;

void loadDefaultSettings();
void writeSettings();
void loadSettings();
void loadSettingsFromBuffer(uint8_t* p);
#endif