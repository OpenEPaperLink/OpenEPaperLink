#pragma once

struct gpt_reg {
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

    /* 0x020: Interrupt Raw Status Register */
    union {
        struct {
            uint32_t CH0_CUP_INT    :  1;  /* [0]     r/w1clr */
            uint32_t CH1_CUP_INT    :  1;  /* [1]     r/w1clr */
            uint32_t CH2_CUP_INT    :  1;  /* [2]     r/w1clr */
            uint32_t CH3_CUP_INT    :  1;  /* [3]     r/w1clr */
            uint32_t CH4_CUP_INT    :  1;  /* [4]     r/w1clr */
            uint32_t RESERVED_7_5   :  3;  /* [7:5]   rsvd */
            uint32_t CH0_CDN_INT    :  1;  /* [8]     r/w1clr */
            uint32_t CH1_CDN_INT    :  1;  /* [9]     r/w1clr */
            uint32_t CH2_CDN_INT    :  1;  /* [10]    r/w1clr */
            uint32_t CH3_CDN_INT    :  1;  /* [11]    r/w1clr */
            uint32_t CH4_CDN_INT    :  1;  /* [12]    r/w1clr */
            uint32_t RESERVED_15_13 :  3;  /* [15:13] rsvd */
            uint32_t CNT_UPP_INT    :  1;  /* [16]    r/w1clr */
            uint32_t CNT_LOW_INT    :  1;  /* [17]    r/w1clr */
            uint32_t RESERVED_23_18 :  6;  /* [23:18] rsvd */
            uint32_t DMA0_OF_INT    :  1;  /* [24]    r/w1clr */
            uint32_t DMA1_OF_INT    :  1;  /* [25]    r/w1clr */
            uint32_t RESERVED_31_26 :  6;  /* [31:26] rsvd */
        } BF;
        uint32_t WORDVAL;
    } INT_RAW_REG;

    /* 0x024: Interrupt Register */
    union {
        struct {
            uint32_t CH0_CUP_INTR   :  1;  /* [0]     r/o */
            uint32_t CH1_CUP_INTR   :  1;  /* [1]     r/o */
            uint32_t CH2_CUP_INTR   :  1;  /* [2]     r/o */
            uint32_t CH3_CUP_INTR   :  1;  /* [3]     r/o */
            uint32_t CH4_CUP_INTR   :  1;  /* [4]     r/o */
            uint32_t RESERVED_7_5   :  3;  /* [7:5]   rsvd */
            uint32_t CH0_CDN_INTR   :  1;  /* [8]     r/o */
            uint32_t CH1_CDN_INTR   :  1;  /* [9]     r/o */
            uint32_t CH2_CDN_INTR   :  1;  /* [10]    r/o */
            uint32_t CH3_CDN_INTR   :  1;  /* [11]    r/o */
            uint32_t CH4_CDN_INTR   :  1;  /* [12]    r/o */
            uint32_t RESERVED_15_13 :  3;  /* [15:13] rsvd */
            uint32_t CNT_UPP_INTR   :  1;  /* [16]    r/o */
            uint32_t CNT_LOW_INTR   :  1;  /* [17]    r/o */
            uint32_t RESERVED_23_18 :  6;  /* [23:18] rsvd */
            uint32_t DMA0_OF_INTR   :  1;  /* [24]    r/o */
            uint32_t DMA1_OF_INTR   :  1;  /* [25]    r/o */
            uint32_t RESERVED_31_26 :  6;  /* [31:26] rsvd */
        } BF;
        uint32_t WORDVAL;
    } INT_REG;

    /* 0x028: Interrupt Mask Register */
    union {
        struct {
            uint32_t CH0_CUP_MSK    :  1;  /* [0]     r/w */
            uint32_t CH1_CUP_MSK    :  1;  /* [1]     r/w */
            uint32_t CH2_CUP_MSK    :  1;  /* [2]     r/w */
            uint32_t CH3_CUP_MSK    :  1;  /* [3]     r/w */
            uint32_t CH4_CUP_MSK    :  1;  /* [4]     r/w */
            uint32_t RESERVED_7_5   :  3;  /* [7:5]   rsvd */
            uint32_t CH0_CDN_MSK    :  1;  /* [8]     r/w */
            uint32_t CH1_CDN_MSK    :  1;  /* [9]     r/w */
            uint32_t CH2_CDN_MSK    :  1;  /* [10]    r/w */
            uint32_t CH3_CDN_MSK    :  1;  /* [11]    r/w */
            uint32_t CH4_CDN_MSK    :  1;  /* [12]    r/w */
            uint32_t RESERVED_15_13 :  3;  /* [15:13] rsvd */
            uint32_t CNT_UPP_MSK    :  1;  /* [16]    r/w */
            uint32_t CNT_LOW_MSK    :  1;  /* [17]    r/w */
            uint32_t RESERVED_23_18 :  6;  /* [23:18] rsvd */
            uint32_t DMA0_OF_MSK    :  1;  /* [24]    r/w */
            uint32_t DMA1_OF_MSK    :  1;  /* [25]    r/w */
            uint32_t RESERVED_31_26 :  6;  /* [31:26] rsvd */
        } BF;
        uint32_t WORDVAL;
    } INT_MSK_REG;

    uint8_t zReserved0x02c[20];  /* pad 0x02c - 0x03f */

    /* 0x040: Counter Control Register */
    union {
        struct {
            uint32_t CNT_DIR        :  2;  /* [1:0]   r/w */
            uint32_t RESERVED_3_2   :  2;  /* [3:2]   rsvd */
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
            uint32_t CLK_SRC        :  1;  /* [0]     r/w */
            uint32_t RESERVED_7_1   :  7;  /* [7:1]   rsvd */
            uint32_t CLK_DIV        :  4;  /* [11:8]  r/w */
            uint32_t RESERVED_15_12 :  4;  /* [15:12] rsvd */
            uint32_t CLK_PRE        :  8;  /* [23:16] r/w */
            uint32_t RESERVED_31_24 :  8;  /* [31:24] rsvd */
        } BF;
        uint32_t WORDVAL;
    } CLK_CNTL_REG;

    uint8_t zReserved0x084[4];  /* pad 0x084 - 0x087 */

    /* 0x088: Input Capture Control Register */
    union {
        struct {
            uint32_t CHX_IC_WIDTH   :  3;  /* [2:0]   r/w */
            uint32_t RESERVED_3     :  1;  /* [3]     rsvd */
            uint32_t CHX_IC_DIV     :  3;  /* [6:4]   r/w */
            uint32_t RESERVED_7     :  1;  /* [7]     rsvd */
            uint32_t CHX_IC_REP     :  3;  /* [10:8]  r/w */
            uint32_t RESERVED_11    :  1;  /* [11]    rsvd */
            uint32_t CHX_IC_EDGE    :  1;  /* [12]    r/w */
            uint32_t RESERVED_31_13 : 19;  /* [31:13] rsvd */
        } BF;
        uint32_t WORDVAL;
    } IC_CNTL_REG;

    uint8_t zReserved0x08c[20];  /* pad 0x08c - 0x09f */

    /* 0x0a0: DMA Control Enable Register */
    union {
        struct {
            uint32_t DMA0_EN        :  1;  /* [0]     r/w */
            uint32_t DMA1_EN        :  1;  /* [1]     r/w */
            uint32_t RESERVED_31_2  : 30;  /* [31:2]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } DMA_CNTL_EN_REG;

    /* 0x0a4: DMA Control Channel Register */
    union {
        struct {
            uint32_t DMA0_CH        :  3;  /* [2:0]   r/w */
            uint32_t RESERVED_3     :  1;  /* [3]     rsvd */
            uint32_t DMA1_CH        :  3;  /* [6:4]   r/w */
            uint32_t RESERVED_31_7  : 25;  /* [31:7]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } DMA_CNTL_CH_REG;

    uint8_t zReserved0x0a8[72];  /* pad 0x0a8 - 0x0ef */

    /* 0x0f0: User Request Register */
    union {
        struct {
            uint32_t CH0_USER_ITRIG :  1;  /* [0]     w/o */
            uint32_t CH1_USER_ITRIG :  1;  /* [1]     w/o */
            uint32_t CH2_USER_ITRIG :  1;  /* [2]     w/o */
            uint32_t CH3_USER_ITRIG :  1;  /* [3]     w/o */
            uint32_t CH4_USER_ITRIG :  1;  /* [4]     w/o */
            uint32_t RESERVED_15_5  : 11;  /* [15:5]  rsvd */
            uint32_t CH0_VAL_UPDT   :  1;  /* [16]    w/o */
            uint32_t CH1_VAL_UPDT   :  1;  /* [17]    w/o */
            uint32_t CH2_VAL_UPDT   :  1;  /* [18]    w/o */
            uint32_t CH3_VAL_UPDT   :  1;  /* [19]    w/o */
            uint32_t CH4_VAL_UPDT   :  1;  /* [20]    w/o */
            uint32_t RESERVED_31_21 : 11;  /* [31:21] rsvd */
        } BF;
        uint32_t WORDVAL;
    } USER_REQ_REG;

    uint8_t zReserved0x0f4[268];  /* pad 0x0f4 - 0x1ff */

    struct {
        /* 0x200: Channel 0 Control Register */
        union {
            struct {
                uint32_t CHX_IO         :  1;  /* [0]     r/w */
                uint32_t RESERVED_31_1  : 31;  /* [31:1]  rsvd */
            } BF;
            uint32_t WORDVAL;
        } CHX_CNTL_REG;

        uint8_t zReserved0x204[4];  /* pad 0x204 - 0x207 */

        /* 0x208: Channel 0 Action Register */
        union {
            struct {
                uint32_t RESERVED_1_0   :  2;  /* [1:0]   rsvd */
                uint32_t CHX_CUP_ACT_T  :  3;  /* [4:2]   r/w */
                uint32_t RESERVED_9_5   :  5;  /* [9:5]   rsvd */
                uint32_t CHX_CDN_ACT_T  :  3;  /* [12:10] r/w */
                uint32_t RESERVED_17_13 :  5;  /* [17:13] rsvd */
                uint32_t CHX_UPP_ACT_T  :  3;  /* [20:18] r/w */
                uint32_t RESERVED_25_21 :  5;  /* [25:21] rsvd */
                uint32_t CHX_LOW_ACT_T  :  3;  /* [28:26] r/w */
                uint32_t RESERVED_31_29 :  3;  /* [31:29] rsvd */
            } BF;
            uint32_t WORDVAL;
        } CHX_ACT_REG;

        uint8_t zReserved0x20c[4];  /* pad 0x20c - 0x20f */

        /* 0x210: Channel 0 Count Up Value Low Register */
        union {
            struct {
                uint32_t CUP_VAL        : 32;  /* [31:0]  r/w */
            } BF;
            uint32_t WORDVAL;
        } CHX_CUP_VAL_REG;

        uint8_t zReserved0x214[12];  /* pad 0x214 - 0x21f */

        /* 0x220: Channel 0 Count Down Value Low Register */
        union {
            struct {
                uint32_t CDN_VAL        : 32;  /* [31:0]  r/w */
            } BF;
            uint32_t WORDVAL;
        } CHX_CDN_VAL_REG;

        uint8_t zReserved0x224[28];  /* pad 0x224 - 0x23f */
    } CH[5];//REGFILE0[5];

};

typedef volatile struct gpt_reg gpt_reg_t;

#ifdef GPT1_IMPL
BEGIN_REG_SECTION(gpt1_registers)
gpt_reg_t GPT1REG;
END_REG_SECTION(gpt1_registers)
#else
extern gpt_reg_t GPT1REG;
#endif

#ifdef GPT2_IMPL
BEGIN_REG_SECTION(gpt2_registers)
gpt_reg_t GPT2REG;
END_REG_SECTION(gpt2_registers)
#else
extern gpt_reg_t GPT2REG;
#endif
