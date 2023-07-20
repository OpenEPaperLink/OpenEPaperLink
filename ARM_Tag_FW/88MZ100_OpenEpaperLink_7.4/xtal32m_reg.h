#pragma once

struct xtal32m_reg {
    /* 0x00: Control Register */
    union {
        struct {
            uint32_t RESERVED_2_0        :  3;  /* [2:0]   rsvd */
            uint32_t DOUBLER_RESET       :  1;  /* [3]     r/w */
            uint32_t RESERVED_16_4       : 13;  /* [16:4]  rsvd */
            uint32_t CLK_ANA_32M_EN      :  1;  /* [17]    r/w */
            uint32_t CLK_DIG_32M_EN      :  1;  /* [18]    r/w */
            uint32_t RESERVED_28_19      : 10;  /* [28:19] rsvd */
            uint32_t PD                  :  1;  /* [29]    r/w */
            uint32_t RESERVED_31_30      :  2;  /* [31:30] rsvd */
        } BF;
        uint32_t WORDVAL;
    } CNTL;

    uint8_t zReserved0x04[8];  /* pad 0x04 - 0x0b */

    /* 0x0c: State Register */
    union {
        struct {
            uint32_t EXT_OSC_EN          :  1;  /* [0]     r/w */
            uint32_t REG_BYPASS_EN       :  1;  /* [1]     r/w */
            uint32_t CLK_SLEEP_ON        :  1;  /* [2]     r/w */
            uint32_t RESERVED_31_3       : 29;  /* [31:3]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } STATE;

    /* 0x10: Interrupt Status Register */
    union {
        struct {
            uint32_t DIG_32M_RDY_INT     :  1;  /* [0]     r/o */
            uint32_t ANA_32M_RDY_INT     :  1;  /* [1]     r/o */
            uint32_t DIG_64M_RDY_INT     :  1;  /* [2]     r/o */
            uint32_t RESERVED_31_3       : 29;  /* [31:3]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } ISR;

    /* 0x14: Interrupt Mask Register */
    union {
        struct {
            uint32_t DIG_32M_RDY_INT_MSK :  1;  /* [0]     r/w */
            uint32_t ANA_32M_RDY_INT_MSK :  1;  /* [1]     r/w */
            uint32_t DIG_64M_RDY_INT_MSK :  1;  /* [2]     r/w */
            uint32_t RESERVED_31_3       : 29;  /* [31:3]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } IMR;

    /* 0x18: Interrupt Raw Status Register */
    union {
        struct {
            uint32_t DIG_32M_RDY_INT_RAW :  1;  /* [0]     r/o */
            uint32_t ANA_32M_RDY_INT_RAW :  1;  /* [1]     r/o */
            uint32_t DIG_64M_RDY_INT_RAW :  1;  /* [2]     r/o */
            uint32_t RESERVED_31_3       : 29;  /* [31:3]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } IRSR;

    /* 0x1c: Interrupt Clear Register */
    union {
        struct {
            uint32_t DIG_32M_RDY_INT_CLR :  1;  /* [0]     r/w */
            uint32_t ANA_32M_RDY_INT_CLR :  1;  /* [1]     r/w */
            uint32_t DIG_64M_RDY_INT_CLR :  1;  /* [2]     r/w */
            uint32_t RESERVED_31_3       : 29;  /* [31:3]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } ICR;

    /* 0x20: Soft Reset Register */
    union {
        struct {
            uint32_t SOFT_RST            :  1;  /* [0]     r/w */
            uint32_t RESERVED_31_1       : 31;  /* [31:1]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } RST;

};

typedef volatile struct xtal32m_reg xtal32m_reg_t;

#ifdef XTAL32M_IMPL
BEGIN_REG_SECTION(xtal32m_registers)
xtal32m_reg_t XTAL32MREG;
END_REG_SECTION(xtal32m_registers)
#else
extern xtal32m_reg_t XTAL32MREG;
#endif
