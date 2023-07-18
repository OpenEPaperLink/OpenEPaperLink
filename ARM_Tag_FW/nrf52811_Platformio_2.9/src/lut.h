#pragma once
#include <Arduino.h>
#include "screen.h"

#define LUTGROUP_NEGATIVE           0
#define LUTGROUP_FASTBLINK          1
#define LUTGROUP_SLOWBLINK          2
#define LUTGROUP_SET                3
#define LUTGROUP_IMPROVE_SHARPNESS  4
#define LUTGROUP_IMPROVE_REDS       5
#define LUTGROUP_UNUSED             6

#define LUTGROUP_UNKNOWN            7
#define LUTGROUP_UNUSED3            8  
#define LUTGROUP_UNUSED4            9


#pragma pack(1)
struct vgroup {
    uint8_t A : 2;
    uint8_t B : 2;
    uint8_t C : 2;
    uint8_t D : 2;
};

struct lut {
    struct vgroup group[7];
};

struct lut10 {
    struct vgroup group[10];
} ;

struct group {
    uint8_t phaselength[4];
    uint8_t repeat;
} ;

struct waveform {
    struct lut lut[5];
    struct group group[7];
    uint8_t gatelevel;
    uint8_t sourcelevel[3];
    uint8_t dummyline;
    uint8_t gatewidth;
} ;

struct waveform10 {
    struct lut10 lut[5];
    struct group group[10];
    uint8_t gatelevel;
    uint8_t sourcelevel[3];
    uint8_t dummyline;
    uint8_t gatewidth;
} ;
#pragma pack(0)