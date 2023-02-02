#define __packed

#include "screen.h"

#define LUTGROUP_NEGATIVE           0
#define LUTGROUP_FASTBLINK          1
#define LUTGROUP_SLOWBLINK          2
#define LUTGROUP_SET                3
#define LUTGROUP_IMPROVE_SHARPNESS  4
#define LUTGROUP_IMPROVE_REDS       5
#define LUTGROUP_UNUSED             6

#if (SCREEN_LUT_LENGTH == 10)
#define LUTGROUP_UNKNOWN            7
#define LUTGROUP_UNUSED3            8  
#define LUTGROUP_UNUSED4            9
#endif

struct vgroup {
    uint8_t A : 2;
    uint8_t B : 2;
    uint8_t C : 2;
    uint8_t D : 2;
} __packed;

struct lut {
    struct vgroup group[7];
} __packed;

struct lut10 {
    struct vgroup group[10];
} __packed;

struct group {
    uint8_t phaselength[4];
    uint8_t repeat;
} __packed;

struct waveform {
    struct lut lut[5];
    struct group group[7];
    uint8_t gatelevel;
    uint8_t sourcelevel[3];
    uint8_t dummyline;
    uint8_t gatewidth;
} __packed;

struct waveform10 {
    struct lut10 lut[5];
    struct group group[10];
    uint8_t gatelevel;
    uint8_t sourcelevel[3];
    uint8_t dummyline;
    uint8_t gatewidth;
} __packed;