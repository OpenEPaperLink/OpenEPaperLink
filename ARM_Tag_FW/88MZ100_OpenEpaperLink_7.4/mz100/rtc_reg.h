#pragma once

struct rtc_reg {
    /* 0x000: Counter Enable Register */
    union {
        struct {
            uint32_t CNT_START      :  1;  /* [0]     w/o */
            uint32_t CNT_STOP       :  1;  /* [1]     w/o */
            uint32_t CNT_RESET      :  1;  /* [2]     w/o */
            uint32_t RESERVED_15_3  : 13;  /* [15:3]  rsvd */
            uint32_t CNT_RUN        :  1;  /* [16]    r/o */
            uint32_t CNT_RST_DONE   :  1;  /* [17]    r/o */
            uint32_t STS_RESETN     :  1;  /* [18]    r/o */
            uint32_t RESERVED_31_19 : 13;  /* [31:19] rsvd */
        } BF;
        uint32_t WORDVAL;
    } CNT_EN_REG;

    uint8_t zReserved0x004[28];  /* pad 0x004 - 0x01f */

    /* 0x020: Interrupt Raw Register */
    union {
        struct {
            uint32_t CH0_OC_INT     :  1;  /* [0]     r/w1clr */
            uint32_t CH1_OC_INT     :  1;  /* [1]     r/w1clr */
            uint32_t RESERVED_15_2  : 14;  /* [15:2]  rsvd */
            uint32_t CNT_UPP_INT    :  1;  /* [16]    r/w1clr */
            uint32_t RESERVED_31_17 : 15;  /* [31:17] rsvd */
        } BF;
        uint32_t WORDVAL;
    } INT_RAW_REG;

    /* 0x024: Interrupt Register */
    union {
        struct {
            uint32_t CH0_OC_INTR    :  1;  /* [0]     r/o */
            uint32_t CH1_OC_INTR    :  1;  /* [1]     r/o */
            uint32_t RESERVED_15_2  : 14;  /* [15:2]  rsvd */
            uint32_t CNT_UPP_INTR   :  1;  /* [16]    r/o */
            uint32_t RESERVED_31_17 : 15;  /* [31:17] rsvd */
        } BF;
        uint32_t WORDVAL;
    } INT_REG;

    /* 0x028: Interrupt Mask Register */
    union {
        struct {
            uint32_t CH0_OC_MSK     :  1;  /* [0]     r/w */
            uint32_t CH1_OC_MSK     :  1;  /* [1]     r/w */
            uint32_t RESERVED_15_2  : 14;  /* [15:2]  rsvd */
            uint32_t CNT_UPP_MSK    :  1;  /* [16]    r/w */
            uint32_t RESERVED_31_17 : 15;  /* [31:17] rsvd */
        } BF;
        uint32_t WORDVAL;
    } INT_MSK_REG;

    uint8_t zReserved0x02c[20];  /* pad 0x02c - 0x03f */

    /* 0x040: Counter Control Register */
    union {
        struct {
            uint32_t RESERVED_3_0   :  4;  /* [3:0]   rsvd */
            uint32_t CNT_DBG_ACT    :  1;  /* [4]     r/w */
            uint32_t RESERVED_7_5   :  3;  /* [7:5]   rsvd */
            uint32_t CNT_UPDT_MOD   :  2;  /* [9:8]   r/w */
            uint32_t RESERVED_31_10 : 22;  /* [31:10] rsvd */
        } BF;
        uint32_t WORDVAL;
    } CNT_CNTL_REG;

    uint8_t zReserved0x044[12];  /* pad 0x044 - 0x04f */

    /* 0x050: Counter Value Register */
    union {
        struct {
            uint32_t CNT_VAL        : 32;  /* [31:0]  r/o */
        } BF;
        uint32_t WORDVAL;
    } CNT_VAL_REG;

    uint8_t zReserved0x054[12];  /* pad 0x054 - 0x05f */

    /* 0x060: Counter Upper Value Register */
    union {
        struct {
            uint32_t UPP_VAL        : 32;  /* [31:0]  r/w */
        } BF;
        uint32_t WORDVAL;
    } CNT_UPP_VAL_REG;

    uint8_t zReserved0x064[28];  /* pad 0x064 - 0x07f */

    /* 0x080: Clock Control Register */
    union {
        struct {
            uint32_t RESERVED_7_0   :  8;  /* [7:0]   rsvd */
            uint32_t CLK_DIV        :  4;  /* [11:8]  r/w */
            uint32_t RESERVED_31_12 : 20;  /* [31:12] rsvd */
        } BF;
        uint32_t WORDVAL;
    } CLK_CNTL_REG;

//    uint8_t zReserved0x084[396];  /* pad 0x084 - 0x20f */
    uint8_t zReserved0x084[108];  /* pad 0x084 - 0x0ef */

    /* 0x0f0: User Request Register */
    union {
        struct {
            uint32_t RESERVED_15_0  : 16;  /* [15:0]  rsvd */
            uint32_t CH0_VAL_UPDT   :  1;  /* [16]    w/o */
            uint32_t CH1_VAL_UPDT   :  1;  /* [17]    w/o */
            uint32_t RESERVED_31_18 : 14;  /* [31:18] rsvd */
        } BF;
        uint32_t WORDVAL;
    } USER_REQ_REG;

    uint8_t zReserved0x0f4[284];  /* pad 0x0f4 - 0x1ff */

    /* 0x210: Channel 0 CUP low register */
    union {
        struct {
            uint32_t CUP_VAL        : 32;  /* [31:0]  r/w */
        } BF;
        uint32_t WORDVAL;
    } CH0_CUP_VAL_REG;

    uint8_t zReserved0x214[60];  /* pad 0x214 - 0x24f */

    /* 0x250: Channel 1 CUP low register */
    union {
        struct {
            uint32_t CUP_VAL        : 32;  /* [31:0]  r/w */
        } BF;
        uint32_t WORDVAL;
    } CH1_CUP_VAL_REG;

};

typedef volatile struct rtc_reg rtc_reg_t;

#ifdef RTC_IMPL
BEGIN_REG_SECTION(rtc_registers)
rtc_reg_t RTCREG;
END_REG_SECTION(rtc_registers)
#else
extern rtc_reg_t RTCREG;
#endif
