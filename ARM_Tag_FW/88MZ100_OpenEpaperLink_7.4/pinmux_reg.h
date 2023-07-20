#pragma once

struct pinmux_reg {
    uint8_t zReserved0x00[4];  /* pad 0x00 - 0x03 */

    /* 0x04-0x7c: GPIO_PINMUX Configuration register */
    union {
        struct {
            uint32_t FSEL_XR              :  4;  /* [3:0]   r/w */
            uint32_t RESERVED_8_4         :  5;  /* [8:4]   r/w */
            uint32_t DO_VAL               :  1;  /* [9]     r/w */
            uint32_t DO_EN                :  1;  /* [10]    r/w */
            uint32_t DO_SEL               :  1;  /* [11]    r/w */
            uint32_t DI_EN                :  1;  /* [12]    r/w */
            uint32_t PIO_PULLDN_R         :  1;  /* [13]    r/w */
            uint32_t PIO_PULLUP_R         :  1;  /* [14]    r/w */
            uint32_t PIO_PULL_SEL_R       :  1;  /* [15]    r/w */
            uint32_t RESERVED_31_16       : 16;  /* [31:16] rsvd */
        } BF;
        uint32_t WORDVAL;
    } GPIO_PINMUX[31];

};

typedef volatile struct pinmux_reg pinmux_reg_t;

#ifdef PINMUX_IMPL
BEGIN_REG_SECTION(pinmux_registers)
pinmux_reg_t PINMUXREG;
END_REG_SECTION(pinmux_registers)
#else
extern pinmux_reg_t PINMUXREG;
#endif
