#ifndef SETTINGS_H
#define SETTINGS_H

#include <stdint.h>

#define FW_VERSION 013 // version number (max 2.5.5 :) )
#define FW_VERSION_SUFFIX "-rf99" // suffix, like -RC1 or whatever.
#define HAS_BUTTON  // uncomment to enable reading a push button (connect between 'TEST' en 'GND' on the tag, along with a 100nF capacitor in parallel).
#define DEBUGBLOCKS // uncomment to enable extra debug information on the block transfers

#endif