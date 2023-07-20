#pragma once

struct gpio_reg {
    /* 0x00: - Name */
    union {
        struct {
            uint32_t SWPORTA_DR    : 32;  /* [31:0]  r/w */
        } BF;
        uint32_t WORDVAL;
    } SWPORTA_DR;

    /* 0x04: - Name */
    union {
        struct {
            uint32_t SWPORTA_DDR   : 32;  /* [31:0]  r/w */
        } BF;
        uint32_t WORDVAL;
    } SWPORTA_DDR;

    uint8_t zReserved0x08[40];  /* pad 0x08 - 0x2f */

    /* 0x30: - Name */
    union {
        struct {
            uint32_t INTEN         : 32;  /* [31:0]  r/w */
        } BF;
        uint32_t WORDVAL;
    } INTEN;

    /* 0x34: - Name */
    union {
        struct {
            uint32_t INTMASK       : 32;  /* [31:0]  r/w */
        } BF;
        uint32_t WORDVAL;
    } INTMASK;

    /* 0x38: - Name */
    union {
        struct {
            uint32_t INTTYPE_LEVEL : 32;  /* [31:0]  r/w */
        } BF;
        uint32_t WORDVAL;
    } INTTYPE_LEVEL;

    /* 0x3c: - Name */
    union {
        struct {
            uint32_t INT_POLARITY  : 32;  /* [31:0]  r/w */
        } BF;
        uint32_t WORDVAL;
    } INT_POLARITY;

    /* 0x40: - Name */
    union {
        struct {
            uint32_t INTSTATUS     : 32;  /* [31:0]  r/o */
        } BF;
        uint32_t WORDVAL;
    } INTSTATUS;

    /* 0x44: - Name */
    union {
        struct {
            uint32_t RAW_INTSTATUS : 32;  /* [31:0]  r/o */
        } BF;
        uint32_t WORDVAL;
    } RAW_INTSTATUS;

    uint8_t zReserved0x48[4];  /* pad 0x48 - 0x4b */

    /* 0x4c: - Name */
    union {
        struct {
            uint32_t PORTA_EOI     : 32;  /* [31:0]  w/o */
        } BF;
        uint32_t WORDVAL;
    } PORTA_EOI;

    /* 0x50: - Name */
    union {
        struct {
            uint32_t EXT_PORTA     : 32;  /* [31:0]  r/o */
        } BF;
        uint32_t WORDVAL;
    } EXT_PORTA;

};

typedef volatile struct gpio_reg gpio_reg_t;

#ifdef GPIO_IMPL
BEGIN_REG_SECTION(gpio_registers)
gpio_reg_t GPIOREG;
END_REG_SECTION(gpio_registers)
#else
extern gpio_reg_t GPIOREG;
#endif
