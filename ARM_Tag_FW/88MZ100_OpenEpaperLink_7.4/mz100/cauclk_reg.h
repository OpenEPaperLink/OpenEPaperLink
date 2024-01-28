#pragma once

struct cauclk_reg {
    /* 0x0: Control Register */
    union {
        struct {
            uint32_t SRC_SEL       :  2;  /* [1:0]   r/w */
            uint32_t RESERVED_31_2 : 30;  /* [31:2]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } CTRL;

};

typedef volatile struct cauclk_reg cauclk_reg_t;

#ifdef CAUCLK_IMPL
BEGIN_REG_SECTION(cauclk_registers)
cauclk_reg_t CAUCLKREG;
END_REG_SECTION(cauclk_registers)
#else
extern cauclk_reg_t CAUCLKREG;
#endif
