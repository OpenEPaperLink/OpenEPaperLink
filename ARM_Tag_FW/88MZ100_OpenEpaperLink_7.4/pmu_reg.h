#pragma once

struct pmu_reg {
    /* 0x00: Overall power mode control */
    union {
        struct {
            uint32_t PWR_MODE              :  2;  /* [1:0]   r/w */
            uint32_t CAU_ON                :  1;  /* [2]     r/w */
            uint32_t RESERVED_31_3         : 29;  /* [31:3]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } PWR_MODE;

    /* 0x04: Power Status Register */
    union {
        struct {
            uint32_t VDD_MCU               :  2;  /* [1:0]   r/o */
            uint32_t VDD_MEM               :  2;  /* [3:2]   r/o */
            uint32_t VDD_CAU               :  2;  /* [5:4]   r/o */
            uint32_t VDD_PHY               :  2;  /* [7:6]   r/o */
            uint32_t VAA_CAU               :  1;  /* [8]     r/o */
            uint32_t VFL                   :  1;  /* [9]     r/o */
            uint32_t VAA_RF                :  1;  /* [10]    r/o */
            uint32_t RESERVED_31_11        : 21;  /* [31:11] rsvd */
        } BF;
        uint32_t WORDVAL;
    } PWR_STATUS;

    /* 0x08: Clock Source Register */
    union {
        struct {
            uint32_t MAIN_CLK_SOURCE       :  1;  /* [0]     r/w */
            uint32_t RTC_CLK_SOURCE        :  1;  /* [1]     r/w */
            uint32_t GPT1_CLK_SOURCE       :  2;  /* [3:2]   r/w */
            uint32_t GPT2_CLK_SOURCE       :  2;  /* [5:4]   r/w */
            uint32_t OUT_CLK_SOURCE        :  3;  /* [8:6]   r/w */
            uint32_t I2S_CLK_SOURCE        :  2;  /* [10:9]  r/w */
            uint32_t KEYSCAN_CLK_SOURCE    :  1;  /* [11]    r/w */
            uint32_t TRACKBALL_CLK_SOURCE  :  1;  /* [12]    r/w */
            uint32_t SPI1_CLK_SOURCE       :  1;  /* [13]    r/w */
            uint32_t SPI2_CLK_SOURCE       :  1;  /* [14]    r/w */
            uint32_t _3DG_CLK_SOURCE       :  1;  /* [15]    r/w */
            uint32_t RESERVED_31_16        : 16;  /* [31:16] rsvd */
        } BF;
        uint32_t WORDVAL;
    } CLK_SRC;

    /* 0x0c: CM3 Frequency Register */
    union {
        struct {
            uint32_t MAIN_CLK_FREQ         :  7;  /* [6:0]   r/w */
            uint32_t RESERVED_31_7         : 25;  /* [31:7]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } CM3_FREQ;

    /* 0x10: I2S Clock Dividers Register */
    union {
        struct {
            uint32_t I2S_CLK_DIV2          :  6;  /* [5:0]   r/w */
            uint32_t I2S_CLK_DIV1          :  6;  /* [11:6]  r/w */
            uint32_t RESERVED_31_12        : 20;  /* [31:12] rsvd */
        } BF;
        uint32_t WORDVAL;
    } I2S_DIV;

    /* 0x14: IR Clock Divider Register */
    union {
        struct {
            uint32_t IR_CLK_DIV            :  6;  /* [5:0]   r/w */
            uint32_t RESERVED_31_6         : 26;  /* [31:6]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } IR_DIV;

    /* 0x18: Output Clock Divider Register */
    union {
        struct {
            uint32_t OUT_CLK_DIV           :  6;  /* [5:0]   r/w */
            uint32_t RESERVED_31_6         : 26;  /* [31:6]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } OUT_DIV;

    /* 0x1c: rc32k_cal Clock Dividers Register */
    union {
        struct {
            uint32_t RC32K_CAL_CLK_DIV2    :  6;  /* [5:0]   r/w */
            uint32_t RC32K_CAL_CLK_DIV1    :  6;  /* [11:6]  r/w */
            uint32_t RESERVED_31_12        : 20;  /* [31:12] rsvd */
        } BF;
        uint32_t WORDVAL;
    } RC32K_CAL_CLK_SRC_DIV;

    /* 0x20: Clock Enable Register */
    union {
        struct {
            uint32_t DISABLE_OUT_CLK       :  1;  /* [0]     r/w */
            uint32_t DISABLE_RC32K_CAL     :  1;  /* [1]     r/w */
            uint32_t DISABLE_SYSCTRL       :  1;  /* [2]     r/w */
            uint32_t DISABLE_CAU1          :  1;  /* [3]     r/w */
            uint32_t DISABLE_SPI2          :  1;  /* [4]     r/w */
            uint32_t DISABLE_SPI1          :  1;  /* [5]     r/w */
            uint32_t DISABLE_QSPI          :  1;  /* [6]     r/w */
            uint32_t DISABLE_GPIO          :  1;  /* [7]     r/w */
            uint32_t DISABLE_GPT2          :  1;  /* [8]     r/w */
            uint32_t DISABLE_GPT1          :  1;  /* [9]     r/w */
            uint32_t DISABLE_PINMUX        :  1;  /* [10]    r/w */
            uint32_t DISABLE_RTC           :  1;  /* [11]    r/w */
            uint32_t DISABLE_CAU0          :  1;  /* [12]    r/w */
            uint32_t DISABLE_WDT           :  1;  /* [13]    r/w */
            uint32_t DISABLE_UART2         :  1;  /* [14]    r/w */
            uint32_t DISABLE_UART1         :  1;  /* [15]    r/w */
            uint32_t DISABLE_I2C2          :  1;  /* [16]    r/w */
            uint32_t DISABLE_I2C1          :  1;  /* [17]    r/w */
            uint32_t DISABLE_APB0          :  1;  /* [18]    r/w */
            uint32_t DISABLE_AES           :  1;  /* [19]    r/w */
            uint32_t DISABLE_PHY           :  1;  /* [20]    r/w */
            uint32_t DISABLE_DMA           :  1;  /* [21]    r/w */
            uint32_t DISABLE_COMP          :  1;  /* [22]    r/w */
            uint32_t DISABLE_DAC           :  1;  /* [23]    r/w */
            uint32_t DISABLE_ADC           :  1;  /* [24]    r/w */
            uint32_t DISABLE_KEYSCAN       :  1;  /* [25]    r/w */
            uint32_t DISABLE_TRACKBALL     :  1;  /* [26]    r/w */
            uint32_t DISABLE_IR            :  1;  /* [27]    r/w */
            uint32_t DISABLE_3DG           :  1;  /* [28]    r/w */
            uint32_t DISABLE_XTAL_CLK_32K  :  1;  /* [29]    r/w */
            uint32_t RESERVED_31_30        :  2;  /* [31:30] rsvd */
        } BF;
        uint32_t WORDVAL;
    } CLK_ENABLE;

    /* 0x24: Clock Enable Register */
    union {
        struct {
            uint32_t DISABLE_ROM0          :  1;  /* [0]     r/w */
            uint32_t DISABLE_ROM1          :  1;  /* [1]     r/w */
            uint32_t DISABLE_RAM0          :  1;  /* [2]     r/w */
            uint32_t DISABLE_RAM1          :  1;  /* [3]     r/w */
            uint32_t DISABLE_RAM2          :  1;  /* [4]     r/w */
            uint32_t DISABLE_RAM3          :  1;  /* [5]     r/w */
            uint32_t DISABLE_RAM4          :  1;  /* [6]     r/w */
            uint32_t DISABLE_AON_MEM       :  1;  /* [7]     r/w */
            uint32_t RESERVED_31_8         : 24;  /* [31:8]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } MEM_CLK_ENABLE;

    /* 0x28: Xtal Clock Switch Register */
    union {
        struct {
            uint32_t XTAL_32K_EXT_OSC_EN   :  1;  /* [0]     r/w */
            uint32_t XTAL_32K_EN           :  1;  /* [1]     r/w */
            uint32_t XTAL_32K_VDDXO        :  2;  /* [3:2]   r/w */
            uint32_t XTAL_32K_STUP_ASSIST  :  2;  /* [5:4]   r/w */
            uint32_t RESERVED_31_6         : 26;  /* [31:6]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } XTAL_CLK_SWITCH;

    /* 0x2c: Clock Status Register */
    union {
        struct {
            uint32_t XTAL_64MHZ            :  1;  /* [0]     r/o */
            uint32_t XTAL_32KHZ            :  1;  /* [1]     r/o */
            uint32_t RC_32MHZ              :  1;  /* [2]     r/o */
            uint32_t RC_32KHZ              :  1;  /* [3]     r/o */
            uint32_t RESERVED_31_4         : 28;  /* [31:4]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } CLK_STATUS;

    /* 0x30: Reset Control Register */
    union {
        struct {
            uint32_t PHY_RST_N             :  1;  /* [0]     r/w */
            uint32_t RESERVED_31_1         : 31;  /* [31:1]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } RST_CTRL;

    /* 0x34: Last Reset Cause Register */
    union {
        struct {
            uint32_t CM3_SYSRESETREQ       :  1;  /* [0]     rclr */
            uint32_t CM3_LOCKUP            :  1;  /* [1]     rclr */
            uint32_t WDT_TIMEOUT           :  1;  /* [2]     rclr */
            uint32_t RESERVED_31_3         : 29;  /* [31:3]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } LAST_RESET;

    /* 0x38: External Triggering Condition Configuration Register */
    union {
        struct {
            uint32_t PIN0_TRIGGER          :  2;  /* [1:0]   r/w */
            uint32_t PIN1_TRIGGER          :  2;  /* [3:2]   r/w */
            uint32_t PIN2_TRIGGER          :  2;  /* [5:4]   r/w */
            uint32_t PIN3_TRIGGER          :  2;  /* [7:6]   r/w */
            uint32_t PIN4_TRIGGER          :  2;  /* [9:8]   r/w */
            uint32_t PIN5_TRIGGER          :  2;  /* [11:10] r/w */
            uint32_t PIN6_TRIGGER          :  2;  /* [13:12] r/w */
            uint32_t PIN7_TRIGGER          :  2;  /* [15:14] r/w */
            uint32_t PIN8_TRIGGER          :  2;  /* [17:16] r/w */
            uint32_t PIN9_TRIGGER          :  2;  /* [19:18] r/w */
            uint32_t PIN10_TRIGGER         :  2;  /* [21:20] r/w */
            uint32_t PIN11_TRIGGER         :  2;  /* [23:22] r/w */
            uint32_t PIN12_TRIGGER         :  2;  /* [25:24] r/w */
            uint32_t RESERVED_31_26        :  6;  /* [31:26] rsvd */
        } BF;
        uint32_t WORDVAL;
    } EXT_TRIG_CFG;

    /* 0x3c: External Interrupt Clear Register */
    union {
        struct {
            uint32_t PIN0_CLR              :  1;  /* [0]     w/o */
            uint32_t PIN1_CLR              :  1;  /* [1]     w/o */
            uint32_t PIN2_CLR              :  1;  /* [2]     w/o */
            uint32_t PIN3_CLR              :  1;  /* [3]     w/o */
            uint32_t PIN4_CLR              :  1;  /* [4]     w/o */
            uint32_t PIN5_CLR              :  1;  /* [5]     w/o */
            uint32_t PIN6_CLR              :  1;  /* [6]     w/o */
            uint32_t PIN7_CLR              :  1;  /* [7]     w/o */
            uint32_t PIN8_CLR              :  1;  /* [8]     w/o */
            uint32_t PIN9_CLR              :  1;  /* [9]     w/o */
            uint32_t PIN10_CLR             :  1;  /* [10]    w/o */
            uint32_t PIN11_CLR             :  1;  /* [11]    w/o */
            uint32_t PIN12_CLR             :  1;  /* [12]    w/o */
            uint32_t KEYSCAN_CLR           :  1;  /* [13]    w/o */
            uint32_t RESERVED_31_14        : 18;  /* [31:14] rsvd */
        } BF;
        uint32_t WORDVAL;
    } EXT_INT_CLR;

    /* 0x40: PMIP CHP Control Register;This register controls the PMIP charge */
    /* pump */
    union {
        struct {
            uint32_t CHP_MODE              :  3;  /* [2:0]   r/w */
            uint32_t CHP_ON_OFF            :  1;  /* [3]     r/w */
            uint32_t CHP_CLK_NOV           :  2;  /* [5:4]   r/w */
            uint32_t CHP_CLK_ROSC          :  1;  /* [6]     r/w */
            uint32_t CHP_MODLCK_BYPASS     :  1;  /* [7]     r/w */
            uint32_t CHP_SPREADSP          :  2;  /* [9:8]   r/w */
            uint32_t CHP_DRV_HALVE         :  1;  /* [10]    r/w */
            uint32_t RESERVED_31_11        : 21;  /* [31:11] rsvd */
        } BF;
        uint32_t WORDVAL;
    } PMIP_CHP_CTRL;

    /* 0x44: PMIP VFL Mode Register */
    union {
        struct {
            uint32_t VFL_MODE              :  1;  /* [0]     r/w */
            uint32_t VFL_PUSKIIP_EN        :  1;  /* [1]     r/w */
            uint32_t RESERVED_31_2         : 30;  /* [31:2]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } PMIP_VFL_MODE;

    /* 0x48: PMIP CP Transition Register */
    union {
        struct {
            uint32_t CP_TRANS_LO           :  6;  /* [5:0]   r/w */
            uint32_t CP_TRANS_HI           :  6;  /* [11:6]  r/w */
            uint32_t RESERVED_31_12        : 20;  /* [31:12] rsvd */
        } BF;
        uint32_t WORDVAL;
    } PMIP_CP_TRANSITION;

    /* 0x4c: PMIP CP REF Register */
    union {
        struct {
            uint32_t CP_REF_LO             :  9;  /* [8:0]   r/w */
            uint32_t CP_REF_HI             :  9;  /* [17:9]  r/w */
            uint32_t RESERVED_31_18        : 14;  /* [31:18] rsvd */
        } BF;
        uint32_t WORDVAL;
    } PMIP_CP_REF;

    /* 0x50: PMIP LDO Register */
    union {
        struct {
            uint32_t V20LDO_PWRSW_TRIG     :  2;  /* [1:0]   r/w */
            uint32_t V20LDO_PWRSW_EN       :  1;  /* [2]     r/w */
            uint32_t V18LDO_PWRSW_EN       :  1;  /* [3]     r/w */
            uint32_t V20LDO_SOFTST         :  1;  /* [4]     r/w */
            uint32_t V18LDO_SOFTST         :  1;  /* [5]     r/w */
            uint32_t V11LDO_SOFTST         :  1;  /* [6]     r/w */
            uint32_t DLDO_PDN_EN           :  2;  /* [8:7]   r/w */
            uint32_t LDO_V18               :  3;  /* [11:9]  r/w */
            uint32_t RESERVED_31_12        : 20;  /* [31:12] rsvd */
        } BF;
        uint32_t WORDVAL;
    } PMIP_LDO;

    /* 0x54: PMIP CPREG Register */
    union {
        struct {
            uint32_t CPREG_BIASSET         :  2;  /* [1:0]   r/w */
            uint32_t CPREG_VOUT            :  3;  /* [4:2]   r/w */
            uint32_t RESERVED_31_5         : 27;  /* [31:5]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } PMIP_CPREG;

    /* 0x58: PMIP Pump Clock Divider Register */
    union {
        struct {
            uint32_t PUMPCLK_DIV           :  2;  /* [1:0]   r/w */
            uint32_t RESERVED_31_2         : 30;  /* [31:2]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } PMIP_PUMPCLK_DIV;

    /* 0x5c: PMIP V18 Mode Register */
    union {
        struct {
            uint32_t V18_MODE              :  1;  /* [0]     r/w */
            uint32_t RESERVED_31_1         : 31;  /* [31:1]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } PMIP_V18_MODE;

    /* 0x60: PMIP VREF IPOLYGEN Level Control Register */
    union {
        struct {
            uint32_t VREF_IPOLYGEN         :  2;  /* [1:0]   r/w */
            uint32_t RESERVED_31_2         : 30;  /* [31:2]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } PMIP_VREF_IPOLYGEN;

    /* 0x64: PMIP AON Level Register */
    union {
        struct {
            uint32_t VAA_AON_CTRL          :  3;  /* [2:0]   r/w */
            uint32_t VDD_AON_CTRL          :  3;  /* [5:3]   r/w */
            uint32_t VAA_SNK_RU            :  1;  /* [6]     r/w */
            uint32_t RESERVED_31_7         : 25;  /* [31:7]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } PMIP_AON_LVL;

    /* 0x68: PMIP Retention Level Register */
    union {
        struct {
            uint32_t RETVGEN_CTRL          :  3;  /* [2:0]   r/w */
            uint32_t RET_VDD_AON           :  1;  /* [3]     r/w */
            uint32_t RESERVED_31_4         : 28;  /* [31:4]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } PMIP_RET_LVL;

    /* 0x6c: PMIP Status Delay Register */
    union {
        struct {
            uint32_t STATUS_DELAY          :  2;  /* [1:0]   r/w */
            uint32_t RESERVED_31_2         : 30;  /* [31:2]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } PMIP_STATUS_DELAY;

    /* 0x70: RC 32K Clock Power Control Register */
    union {
        struct {
            uint32_t RC32K_PWRDN           :  1;  /* [0]     r/w */
            uint32_t RESERVED_31_1         : 31;  /* [31:1]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } RC32K_PWR_CNTL;

    /* 0x74: RC 32K Calibration Control Register */
    union {
        struct {
            uint32_t RC32K_CAL_EN          :  1;  /* [0]     r/w */
            uint32_t RC32K_CAL_DIV         :  2;  /* [2:1]   r/w */
            uint32_t RC32K_CAL_SRC         :  1;  /* [3]     r/w */
            uint32_t RESERVED_31_4         : 28;  /* [31:4]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } RC32K_CAL_CNTL;

    /* 0x78: RC 32K External Calibration Register */
    union {
        struct {
            uint32_t RC32K_EXT_CODE_EN     :  1;  /* [0]     r/w */
            uint32_t RC32K_CODE_FR_EXT     :  7;  /* [7:1]   r/w */
            uint32_t RESERVED_31_8         : 24;  /* [31:8]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } RC32K_EXT_CAL;

    /* 0x7c: RC 32K Calibration Status */
    union {
        struct {
            uint32_t RC32K_CAL_DONE        :  1;  /* [0]     r/o */
            uint32_t RC32K_CODE_FR_CAL     :  7;  /* [7:1]   r/o */
            uint32_t RC32K_CAL_IN_PROGRESS :  1;  /* [8]     r/o */
            uint32_t RC32K_CAL_ALLOW       :  1;  /* [9]     r/o */
            uint32_t RESERVED_31_10        : 22;  /* [31:10] rsvd */
        } BF;
        uint32_t WORDVAL;
    } RC32K_CAL_STAT;

    /* 0x80: Xtal 32K Test Register */
    union {
        struct {
            uint32_t X32K_TEST_EN          :  1;  /* [0]     r/w */
            uint32_t X32K_TMODE            :  4;  /* [4:1]   r/w */
            uint32_t RESERVED_31_5         : 27;  /* [31:5]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } XTAL_32K_TEST;

    /* 0x84: PMIP BRN Register */
    union {
        struct {
            uint32_t BRNDET_EN             :  1;  /* [0]     r/w */
            uint32_t BRNTRIG_CTRL          :  3;  /* [3:1]   r/w */
            uint32_t BRNHYST_CTRL          :  2;  /* [5:4]   r/w */
            uint32_t BRNDET_FILT           :  2;  /* [7:6]   r/w */
            uint32_t BRNDET_RDY            :  1;  /* [8]     r/o */
            uint32_t BRNDET_OUT            :  1;  /* [9]     r/o */
            uint32_t RESERVED_31_10        : 22;  /* [31:10] rsvd */
        } BF;
        uint32_t WORDVAL;
    } PMIP_BRN;

    /* 0x88: PMIP Comparator Control Register */
    union {
        struct {
            uint32_t COMP_EN               :  1;  /* [0]     r/w */
            uint32_t COMP_DIFF_EN          :  1;  /* [1]     r/w */
            uint32_t COMP_HYST             :  2;  /* [3:2]   r/w */
            uint32_t COMP_REF              :  3;  /* [6:4]   r/w */
            uint32_t COMP_RDY              :  1;  /* [7]     r/o */
            uint32_t COMP_OUT              :  1;  /* [8]     r/o */
            uint32_t COMP_IRQ_POLARITY     :  1;  /* [9]     r/w */
            uint32_t RESERVED_31_10        : 22;  /* [31:10] rsvd */
        } BF;
        uint32_t WORDVAL;
    } PMIP_COMP_CTRL;

    /* 0x8c: PMIP Test Register */
    union {
        struct {
            uint32_t PMIP_TEST_EN          :  1;  /* [0]     r/w */
            uint32_t PMIP_TEST             :  4;  /* [4:1]   r/w */
            uint32_t RESERVED_31_5         : 27;  /* [31:5]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } PMIP_TEST;

    /* 0x90: PMIP CAU Reference Register */
    union {
        struct {
            uint32_t CAU_REF_EN            :  1;  /* [0]     r/w */
            uint32_t RESERVED_31_1         : 31;  /* [31:1]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } PMIP_CAU;

    /* 0x94: PAD Power Down Register */
    union {
        struct {
            uint32_t PAD_FL_PDB            :  1;  /* [0]     r/w */
            uint32_t PAD_AON_PDB           :  1;  /* [1]     r/w */
            uint32_t PAD_VDD2_PDB          :  1;  /* [2]     r/w */
            uint32_t PAD_V18EN_CORE        :  1;  /* [3]     r/w */
            uint32_t RESERVED_31_4         : 28;  /* [31:4]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } PAD_PDB;

    /* 0x98: DFT Enable Register */
    union {
        struct {
            uint32_t DFT_EN                :  1;  /* [0]     r/w */
            uint32_t RESERVED_31_1         : 31;  /* [31:1]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } DFT_EN;

    uint8_t zReserved0x9c[92];  /* pad 0x9c - 0xf7 */

    /* 0xf8: Power Control Register */
    union {
        struct {
            uint32_t VDD_MCU_REQ           :  2;  /* [1:0]   r/w */
            uint32_t VDD_MEM_REQ           :  2;  /* [3:2]   r/w */
            uint32_t VDD_CAU_REQ           :  2;  /* [5:4]   r/w */
            uint32_t VDD_PHY_REQ           :  2;  /* [7:6]   r/w */
            uint32_t VAA_CAU_REQ           :  1;  /* [8]     r/w */
            uint32_t VFL_REQ               :  1;  /* [9]     r/w */
            uint32_t VAA_RF_REQ            :  1;  /* [10]    r/w */
            uint32_t RESERVED_31_11        : 21;  /* [31:11] rsvd */
        } BF;
        uint32_t WORDVAL;
    } PWR_CTRL;

    /* 0xfc: PMX Control Register */
    union {
        struct {
            uint32_t PMX                   :  1;  /* [0]     r/w */
            uint32_t RESERVED_31_1         : 31;  /* [31:1]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } PMX_CTRL;

};

typedef volatile struct pmu_reg pmu_reg_t;

#ifdef PMU_IMPL
BEGIN_REG_SECTION(pmu_registers)
pmu_reg_t PMUREG;
END_REG_SECTION(pmu_registers)
#else
extern pmu_reg_t PMUREG;
#endif
