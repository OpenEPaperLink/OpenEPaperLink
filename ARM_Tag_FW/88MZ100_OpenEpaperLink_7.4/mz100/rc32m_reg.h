#pragma once

struct rc32m_reg {
    /* 0x00: Control Register */
    union {
        struct {
            uint32_t EN             :  1;  /* [0]     r/w */
            uint32_t CAL_EN         :  1;  /* [1]     r/w */
            uint32_t EXT_CODE_EN    :  1;  /* [2]     r/w */
            uint32_t PD             :  1;  /* [3]     r/w */
            uint32_t CODE_FR_EXT    :  7;  /* [10:4]  r/w */
            uint32_t RESERVED_31_11 : 21;  /* [31:11] rsvd */
        } BF;
        uint32_t WORDVAL;
    } CTRL;

    /* 0x04: Status Register */
    union {
        struct {
            uint32_t CLK_RDY        :  1;  /* [0]     r/o */
            uint32_t CAL_DONE       :  1;  /* [1]     r/o */
            uint32_t CODE_FR_CAL    :  7;  /* [8:2]   r/o */
            uint32_t RESERVED_31_9  : 23;  /* [31:9]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } STATUS;

    /* 0x08: Interrupt Status Register */
    union {
        struct {
            uint32_t CALDON_INT     :  1;  /* [0]     r/o */
            uint32_t CKRDY_INT      :  1;  /* [1]     r/o */
            uint32_t RESERVED_31_2  : 30;  /* [31:2]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } ISR;

    /* 0x0c: Interrupt Mask Register */
    union {
        struct {
            uint32_t CALDON_INT_MSK :  1;  /* [0]     r/w */
            uint32_t CKRDY_INT_MSK  :  1;  /* [1]     r/w */
            uint32_t RESERVED_31_2  : 30;  /* [31:2]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } IMR;

    /* 0x10: Interrupt Raw Status Register */
    union {
        struct {
            uint32_t CALDON_INT_RAW :  1;  /* [0]     r/o */
            uint32_t CKRDY_INT_RAW  :  1;  /* [1]     r/o */
            uint32_t RESERVED_31_2  : 30;  /* [31:2]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } IRSR;

    /* 0x14: Interrupt Clear Register */
    union {
        struct {
            uint32_t CALDON_INT_CLR :  1;  /* [0]     r/w */
            uint32_t CKRDY_INT_CLR  :  1;  /* [1]     r/w */
            uint32_t RESERVED_31_2  : 30;  /* [31:2]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } ICR;

    /* 0x18: Clock Register */
    union {
        struct {
            uint32_t RESERVED_1_0   :  2;  /* [1:0]   rsvd */
            uint32_t SEL_32M        :  1;  /* [2]     r/w */
            uint32_t SOFT_CLK_RST   :  1;  /* [3]     r/w */
            uint32_t RESERVED_31_4  : 28;  /* [31:4]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } CLK;

    /* 0x1c: Soft Reset Register */
    union {
        struct {
            uint32_t SOFT_RST       :  1;  /* [0]     r/w */
            uint32_t RESERVED_31_1  : 31;  /* [31:1]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } RST;

};

typedef volatile struct rc32m_reg rc32m_reg_t;

#ifdef RC32M_IMPL
BEGIN_REG_SECTION(rc32m_registers)
rc32m_reg_t RC32MREG;
END_REG_SECTION(rc32m_registers)
#else
extern rc32m_reg_t RC32MREG;
#endif
