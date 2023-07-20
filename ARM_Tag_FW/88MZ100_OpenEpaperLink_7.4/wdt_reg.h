#pragma once

struct wdt_reg {
    /* 0x00: Control Register */
    union {
        struct {
            uint32_t EN                :  1;  /* [0]     r/w */
            uint32_t RMOD              :  1;  /* [1]     r/w */
            uint32_t RPL               :  3;  /* [4:2]   r/w */
            uint32_t RESERVED          : 27;  /* [31:5]  r/o */
        } BF;
        uint32_t WORDVAL;
    } CR;

    /* 0x04: Timeout Range Register */
    union {
        struct {
            uint32_t TOP               :  4;  /* [3:0]   r/w */
            uint32_t RESERVED_7_4      :  4;  /* [7:4]   rsvd */
            uint32_t RESERVED          : 24;  /* [31:8]  r/o */
        } BF;
        uint32_t WORDVAL;
    } TORR;

    /* 0x08: Current Counter Value Register */
    union {
        struct {
            uint32_t CCVR              : 32;  /* [31:0]  r/o */
        } BF;
        uint32_t WORDVAL;
    } CCVR;

    /* 0x0c: Counter Restart Register */
    union {
        struct {
            uint32_t CRR               :  8;  /* [7:0]   w/o */
            uint32_t RESERVED_31_8     : 24;  /* [31:8]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } CRR;

    /* 0x10: Interrupt Status Register */
    union {
        struct {
            uint32_t STAT              :  1;  /* [0]     r/o */
            uint32_t RESERVED_31_1     : 31;  /* [31:1]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } STAT;

    /* 0x14: Interrupt Clear Register */
    union {
        struct {
            uint32_t EOI               :  1;  /* [0]     r/o */
            uint32_t RESERVED_31_1     : 31;  /* [31:1]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } EOI; 

};

typedef volatile struct wdt_reg wdt_reg_t;

#ifdef WDT_IMPL
BEGIN_REG_SECTION(wdt_registers)
wdt_reg_t WDTREG;
END_REG_SECTION(wdt_registers)
#else
extern wdt_reg_t WDTREG;
#endif
