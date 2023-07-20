#pragma once

struct scs_reg {
    /* 0x00000-0x0007c: ITM Stimulus Ports 0-31 */
    union {
        struct {
            uint32_t ITM_STIMULUS_PORT_VAL       : 32;  /* [31:0]  r/w */
        } BF;
        uint32_t WORDVAL;
    } ITM_STIMULUS_PORT_REG[32];

    uint8_t zReserved0x00080[3456];  /* pad 0x00080 - 0x00dff */

    /* 0x00e00: ITM Trace Enable Register */
    union {
        struct {
            uint32_t ITM_TRACE_ENA               : 32;  /* [31:0]  r/w */
        } BF;
        uint32_t WORDVAL;
    } ITM_TRACE_ENA_REG;

    uint8_t zReserved0x00e04[60];  /* pad 0x00e04 - 0x00e3f */

    /* 0x00e40: ITM Trace Privilege Register */
    union {
        struct {
            uint32_t ITM_TRACE_PRI_MASK          :  4;  /* [3:0]   r/w */
            uint32_t RESERVED_31_4               : 28;  /* [31:4]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } ITM_TRACE_PRI_REG;

    uint8_t zReserved0x00e44[60];  /* pad 0x00e44 - 0x00e7f */

    /* 0x00e80: ITM Trace Control Register */
    union {
        struct {
            uint32_t ITM_TRACE_ITM_ENA           :  1;  /* [0]     r/w */
            uint32_t ITM_TRACE_TS_ENA            :  1;  /* [1]     r/w */
            uint32_t ITM_TRACE_SYNC_ENA          :  1;  /* [2]     r/w */
            uint32_t ITM_TRACE_DWT_ENA           :  1;  /* [3]     r/w */
            uint32_t ITM_TRACE_SWO_ENA           :  1;  /* [4]     r/w */
            uint32_t RESERVED_7_5                :  3;  /* [7:5]   rsvd */
            uint32_t ITM_TRACE_TS_PRESCALE       :  2;  /* [9:8]   r/w */
            uint32_t RESERVED_15_10              :  6;  /* [15:10] rsvd */
            uint32_t ITM_TRACE_ATB_ID            :  7;  /* [22:16] r/w */
            uint32_t ITM_TRACE_BUSY              :  1;  /* [23]    r/w */
            uint32_t RESERVED_31_24              :  8;  /* [31:24] rsvd */
        } BF;
        uint32_t WORDVAL;
    } ITM_TRACE_CTRL_REG;

    uint8_t zReserved0x00e84[116];  /* pad 0x00e84 - 0x00ef7 */

    /* 0x00ef8: ITM Integration Write Register */
    union {
        struct {
            uint32_t ITM_INTG_WRITE_ATVALIDM     :  1;  /* [0]     w/o */
            uint32_t RESERVED_31_1               : 31;  /* [31:1]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } ITM_INTG_WRITE_REG;

    /* 0x00efc: ITM Integration Read Register */
    union {
        struct {
            uint32_t ITM_INTG_READ_ATREADYM      :  1;  /* [0]     r/o */
            uint32_t RESERVED_31_1               : 31;  /* [31:1]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } ITM_INTG_READ_REG;

    /* 0x00f00: ITM Integration Mode Control Register */
    union {
        struct {
            uint32_t ITM_INTRGRATION_VAL         :  1;  /* [0]     r/w */
            uint32_t RESERVED_31_1               : 31;  /* [31:1]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } ITM_INTG_MODE_CTRL_REG;

    uint8_t zReserved0x00f04[172];  /* pad 0x00f04 - 0x00faf */

    /* 0x00fb0: ITM Lock Access Register */
    union {
        struct {
            uint32_t ITM_LOCK_ACC_VAL            : 32;  /* [31:0]  w/o */
        } BF;
        uint32_t WORDVAL;
    } ITM_LOCK_ACC_REG;

    /* 0x00fb4: ITM Lock Status Register */
    union {
        struct {
            uint32_t ITM_LOCK_STAT_PRESENT       :  1;  /* [0]     r/o */
            uint32_t ITM_LOCK_STAT_ACCESS        :  1;  /* [1]     r/o */
            uint32_t ITM_LOCK_STAT_BYTE_ACC      :  1;  /* [2]     r/o */
            uint32_t RESERVED_31_3               : 29;  /* [31:3]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } ITM_LOCK_STAT_REG;

    uint8_t zReserved0x00fb8[24];  /* pad 0x00fb8 - 0x00fcf */

    /* 0x00fd0: ITM PID Register 4 */
    union {
        struct {
            uint32_t ITM_PID4_VAL                : 32;  /* [31:0]  r/o */
        } BF;
        uint32_t WORDVAL;
    } ITM_PID4_REG;

    /* 0x00fd4: ITM PID Register 5 */
    union {
        struct {
            uint32_t ITM_PID5_VAL                : 32;  /* [31:0]  r/o */
        } BF;
        uint32_t WORDVAL;
    } ITM_PID5_REG;

    /* 0x00fd8: ITM PID Register 6 */
    union {
        struct {
            uint32_t ITM_PID6_VAL                : 32;  /* [31:0]  r/o */
        } BF;
        uint32_t WORDVAL;
    } ITM_PID6_REG;

    /* 0x00fdc: ITM PID Register 7 */
    union {
        struct {
            uint32_t ITM_PID7_VAL                : 32;  /* [31:0]  r/o */
        } BF;
        uint32_t WORDVAL;
    } ITM_PID7_REG;

    /* 0x00fe0: ITM PID Register 0 */
    union {
        struct {
            uint32_t ITM_PID0_VAL                : 32;  /* [31:0]  r/o */
        } BF;
        uint32_t WORDVAL;
    } ITM_PID0_REG;

    /* 0x00fe4: ITM PID Register 1 */
    union {
        struct {
            uint32_t ITM_PID1_VAL                : 32;  /* [31:0]  r/o */
        } BF;
        uint32_t WORDVAL;
    } ITM_PID1_REG;

    /* 0x00fe8: ITM PID Register 2 */
    union {
        struct {
            uint32_t ITM_PID2_VAL                : 32;  /* [31:0]  r/o */
        } BF;
        uint32_t WORDVAL;
    } ITM_PID2_REG;

    /* 0x00fec: ITM PID Register 3 */
    union {
        struct {
            uint32_t ITM_PID3_VAL                : 32;  /* [31:0]  r/o */
        } BF;
        uint32_t WORDVAL;
    } ITM_PID3_REG;

    /* 0x00ff0: ITM CID Register 0 */
    union {
        struct {
            uint32_t ITM_CID0_VAL                : 32;  /* [31:0]  r/o */
        } BF;
        uint32_t WORDVAL;
    } ITM_CID0_REG;

    /* 0x00ff4: ITM CID Register 1 */
    union {
        struct {
            uint32_t ITM_CID1_VAL                : 32;  /* [31:0]  r/o */
        } BF;
        uint32_t WORDVAL;
    } ITM_CID1_REG;

    /* 0x00ff8: ITM CID Register 2 */
    union {
        struct {
            uint32_t ITM_CID2_VAL                : 32;  /* [31:0]  r/o */
        } BF;
        uint32_t WORDVAL;
    } ITM_CID2_REG;

    /* 0x00ffc: ITM CID Register 3 */
    union {
        struct {
            uint32_t ITM_CID3_VAL                : 32;  /* [31:0]  r/o */
        } BF;
        uint32_t WORDVAL;
    } ITM_CID3_REG;

    /* 0x01000: DWT Control Register */
    union {
        struct {
            uint32_t DWT_CYCCNT_ENA              :  1;  /* [0]     r/w */
            uint32_t DWT_CTRL_POST_PRESET        :  4;  /* [4:1]   r/w */
            uint32_t DWT_CTRL_POST_CNT           :  4;  /* [8:5]   r/w */
            uint32_t DWT_CTRL_CYC_TAP            :  1;  /* [9]     r/w */
            uint32_t DWT_CTRL_SYNC_TAP           :  2;  /* [11:10] r/w */
            uint32_t DWT_CTRL_PCSAMPLE_ENA       :  1;  /* [12]    r/w */
            uint32_t RESERVED_15_13              :  3;  /* [15:13] rsvd */
            uint32_t DWT_CTRL_EXCTRC_ENA         :  1;  /* [16]    r/w */
            uint32_t DWT_CTRL_CPIEVT_ENA         :  1;  /* [17]    r/w */
            uint32_t DWT_CTRL_EXCEVT_ENA         :  1;  /* [18]    r/w */
            uint32_t DWT_CTRL_SLEEPEVT_ENA       :  1;  /* [19]    r/w */
            uint32_t DWT_CTRL_LSUEVT_ENA         :  1;  /* [20]    r/w */
            uint32_t DWT_CTRL_FOLDEVT_ENA        :  1;  /* [21]    r/w */
            uint32_t DWT_CTRL_CYCEVT_ENA         :  1;  /* [22]    r/w */
            uint32_t RESERVED_23                 :  1;  /* [23]    rsvd */
            uint32_t DWT_CTRL_NO_PRFCNT          :  1;  /* [24]    r/w */
            uint32_t DWT_CTRL_NO_CYCCNT          :  1;  /* [25]    r/w */
            uint32_t DWT_CTRL_NO_EXTTRIG         :  1;  /* [26]    r/w */
            uint32_t DWT_CTRL_NO_TRCPKT          :  1;  /* [27]    r/w */
            uint32_t DWT_CTRL_NUMCOMP            :  4;  /* [31:28] r/w */
        } BF;
        uint32_t WORDVAL;
    } DWT_CTRL_REG;

    /* 0x01004: DWT Current PC Sampler Cycle Count Register */
    union {
        struct {
            uint32_t DWT_CYCCNT_VAL              : 32;  /* [31:0]  r/w */
        } BF;
        uint32_t WORDVAL;
    } DWT_CYCCNT_REG;

    /* 0x01008: DWT CPI Count Register */
    union {
        struct {
            uint32_t DWT_CPICNT_VAL              :  8;  /* [7:0]   r/w */
            uint32_t RESERVED_31_8               : 24;  /* [31:8]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } DWT_CPICNT_REG;

    /* 0x0100c: DWT Exception Overhead Count Register */
    union {
        struct {
            uint32_t DWT_EXCCNT_VAL              :  8;  /* [7:0]   r/w */
            uint32_t RESERVED_31_8               : 24;  /* [31:8]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } DWT_EXCCNT_REG;

    /* 0x01010: DWT Sleep Count Register */
    union {
        struct {
            uint32_t DWT_SLEEPCNT_VAL            :  8;  /* [7:0]   r/w */
            uint32_t RESERVED_31_8               : 24;  /* [31:8]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } DWT_SLEEPCNT_REG;

    /* 0x01014: DWT LSU Count Register */
    union {
        struct {
            uint32_t DWT_LSUCNT_VAL              :  8;  /* [7:0]   r/w */
            uint32_t RESERVED_31_8               : 24;  /* [31:8]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } DWT_LSUCNT_REG;

    /* 0x01018: DWT Fold Count Register */
    union {
        struct {
            uint32_t DWT_FOLDCNT_VAL             :  8;  /* [7:0]   r/w */
            uint32_t RESERVED_31_8               : 24;  /* [31:8]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } DWT_FOLDCNT_REG;

    /* 0x0101c: DWT Program Counter Sample Register */
    union {
        struct {
            uint32_t DWT_PCSR_EIA_SAMPLE         : 32;  /* [31:0]  r/o */
        } BF;
        uint32_t WORDVAL;
    } DWT_PCSR_REG;

    struct {
        /* 0x01020: DWT Comparator Registers 0 */
        union {
            struct {
                uint32_t DWT_COMP_VAL                : 32;  /* [31:0]  r/w */
            } BF;
            uint32_t WORDVAL;
        } DWT_COMP_REG;

        /* 0x01024: DWT Mask Registers 0 */
        union {
            struct {
                uint32_t DWT_MASK_VAL                :  4;  /* [3:0]   r/w */
                uint32_t RESERVED_31_4               : 28;  /* [31:4]  rsvd */
            } BF;
            uint32_t WORDVAL;
        } DWT_MASK_REG;

        /* 0x01028: DWT Function Registers 0 */
        union {
            struct {
                uint32_t DWT_FUNCTION_SETTING        :  4;  /* [3:0]   r/w */
                uint32_t RESERVED_4                  :  1;  /* [4]     rsvd */
                uint32_t DWT_FUNCTION_EMITRANGE      :  1;  /* [5]     r/w */
                uint32_t RESERVED_6                  :  1;  /* [6]     rsvd */
                uint32_t DWT_FUNCTION_CYCMATCH       :  1;  /* [7]     r/w */
                uint32_t DWT_FUNCTION_DATAVMATCH     :  1;  /* [8]     r/w */
                uint32_t DWT_FUNCTION_LNK1_ENA       :  1;  /* [9]     r/w */
                uint32_t DWT_FUNCTION_DATAVSIZE      :  2;  /* [11:10] r/w */
                uint32_t DWT_FUNCTION_DATAVADDR0     :  4;  /* [15:12] r/w */
                uint32_t DWT_FUNCTION_DATAVADDR1     :  4;  /* [19:16] r/w */
                uint32_t RESERVED_23_20              :  4;  /* [23:20] rsvd */
                uint32_t DWT_FUNCTION_MATCHED        :  1;  /* [24]    r/w */
                uint32_t RESERVED_31_25              :  7;  /* [31:25] rsvd */
            } BF;
            uint32_t WORDVAL;
        } DWT_FUNCTION_REG;

        uint8_t zReserved0x0102c[4];  /* pad 0x0102c - 0x0102f */
    } REGFILE0[4];

    uint8_t zReserved0x01060[3952];  /* pad 0x01060 - 0x01fcf */

    /* 0x01fd0: DWT PID Register 4 */
    union {
        struct {
            uint32_t DWT_PID4_VAL                : 32;  /* [31:0]  r/o */
        } BF;
        uint32_t WORDVAL;
    } DWT_PID4_REG;

    /* 0x01fd4: DWT PID Register 5 */
    union {
        struct {
            uint32_t DWT_PID5_VAL                : 32;  /* [31:0]  r/o */
        } BF;
        uint32_t WORDVAL;
    } DWT_PID5_REG;

    /* 0x01fd8: DWT PID Register 6 */
    union {
        struct {
            uint32_t DWT_PID6_VAL                : 32;  /* [31:0]  r/o */
        } BF;
        uint32_t WORDVAL;
    } DWT_PID6_REG;

    /* 0x01fdc: DWT PID Register 7 */
    union {
        struct {
            uint32_t DWT_PID7_VAL                : 32;  /* [31:0]  r/o */
        } BF;
        uint32_t WORDVAL;
    } DWT_PID7_REG;

    /* 0x01fe0: DWT PID Register 0 */
    union {
        struct {
            uint32_t DWT_PID0_VAL                : 32;  /* [31:0]  r/o */
        } BF;
        uint32_t WORDVAL;
    } DWT_PID0_REG;

    /* 0x01fe4: DWT PID Register 1 */
    union {
        struct {
            uint32_t DWT_PID1_VAL                : 32;  /* [31:0]  r/o */
        } BF;
        uint32_t WORDVAL;
    } DWT_PID1_REG;

    /* 0x01fe8: DWT PID Register 2 */
    union {
        struct {
            uint32_t DWT_PID2_VAL                : 32;  /* [31:0]  r/o */
        } BF;
        uint32_t WORDVAL;
    } DWT_PID2_REG;

    /* 0x01fec: DWT PID Register 3 */
    union {
        struct {
            uint32_t DWT_PID3_VAL                : 32;  /* [31:0]  r/o */
        } BF;
        uint32_t WORDVAL;
    } DWT_PID3_REG;

    /* 0x01ff0: DWT PID Register 0 */
    union {
        struct {
            uint32_t DWT_CID0_VAL                : 32;  /* [31:0]  r/o */
        } BF;
        uint32_t WORDVAL;
    } DWT_CID0_REG;

    /* 0x01ff4: DWT PID Register 1 */
    union {
        struct {
            uint32_t DWT_CID1_VAL                : 32;  /* [31:0]  r/o */
        } BF;
        uint32_t WORDVAL;
    } DWT_CID1_REG;

    /* 0x01ff8: DWT PID Register 2 */
    union {
        struct {
            uint32_t DWT_CID2_VAL                : 32;  /* [31:0]  r/o */
        } BF;
        uint32_t WORDVAL;
    } DWT_CID2_REG;

    /* 0x01ffc: DWT PID Register 3 */
    union {
        struct {
            uint32_t DWT_CID3_VAL                : 32;  /* [31:0]  r/o */
        } BF;
        uint32_t WORDVAL;
    } DWT_CID3_REG;

    /* 0x02000: Flash Patch Control Register */
    union {
        struct {
            uint32_t FP_CTRL_ENA                 :  1;  /* [0]     r/w */
            uint32_t FP_CTRL_KEY                 :  1;  /* [1]     r/w */
            uint32_t RESERVED_3_2                :  2;  /* [3:2]   rsvd */
            uint32_t FP_CTRL_NUM_CODE1           :  4;  /* [7:4]   r/w */
            uint32_t FP_CTRL_NUM_LIT             :  4;  /* [11:8]  r/w */
            uint32_t FP_CTRL_NUM_CODE2           :  3;  /* [14:12] r/w */
            uint32_t RESERVED_31_15              : 17;  /* [31:15] rsvd */
        } BF;
        uint32_t WORDVAL;
    } FP_CTRL_REG;

    /* 0x02004: Flash Patch Remap Register */
    union {
        struct {
            uint32_t RESERVED_4_0                :  5;  /* [4:0]   rsvd */
            uint32_t FP_REMAP_BASE_ADDR          : 24;  /* [28:5]  r/w */
            uint32_t RESERVED_31_29              :  3;  /* [31:29] rsvd */
        } BF;
        uint32_t WORDVAL;
    } FP_REMAP_REG;

    /* 0x02008-0x02024: System Handler Priority Registers */
    union {
        struct {
            uint32_t FP_COMP_ENA                 :  1;  /* [0]     r/w */
            uint32_t RESERVED_1                  :  1;  /* [1]     rsvd */
            uint32_t FP_COMP_ADDR                : 27;  /* [28:2]  r/w */
            uint32_t RESERVED_29                 :  1;  /* [29]    rsvd */
            uint32_t FP_COMP_REPLACE             :  2;  /* [31:30] r/w */
        } BF;
        uint32_t WORDVAL;
    } FP_COMP_REG[8];

    uint8_t zReserved0x02028[4008];  /* pad 0x02028 - 0x02fcf */

    /* 0x02fd0: Flash patch PID register4 */
    union {
        struct {
            uint32_t FP_PID4_VAL                 : 32;  /* [31:0]  r/o */
        } BF;
        uint32_t WORDVAL;
    } FP_PID4_REG;

    /* 0x02fd4: Flash patch PID register5 */
    union {
        struct {
            uint32_t FP_PID5_VAL                 : 32;  /* [31:0]  r/o */
        } BF;
        uint32_t WORDVAL;
    } FP_PID5_REG;

    /* 0x02fd8: Flash patch PID register6 */
    union {
        struct {
            uint32_t FP_PID6_VAL                 : 32;  /* [31:0]  r/o */
        } BF;
        uint32_t WORDVAL;
    } FP_PID6_REG;

    /* 0x02fdc: Flash patch PID register7 */
    union {
        struct {
            uint32_t FP_PID7_VAL                 : 32;  /* [31:0]  r/o */
        } BF;
        uint32_t WORDVAL;
    } FP_PID7_REG;

    /* 0x02fe0: Flash patch PID register 0 */
    union {
        struct {
            uint32_t FP_PID0_VAL                 : 32;  /* [31:0]  r/o */
        } BF;
        uint32_t WORDVAL;
    } FP_PID0_REG;

    /* 0x02fe4: Flash patch PID register 1 */
    union {
        struct {
            uint32_t FP_PID1_VAL                 : 32;  /* [31:0]  r/o */
        } BF;
        uint32_t WORDVAL;
    } FP_PID1_REG;

    /* 0x02fe8: Flash patch PID register 2 */
    union {
        struct {
            uint32_t FP_PID2_VAL                 : 32;  /* [31:0]  r/o */
        } BF;
        uint32_t WORDVAL;
    } FP_PID2_REG;

    /* 0x02fec: Flash patch PID register 3 */
    union {
        struct {
            uint32_t FP_PID3_VAL                 : 32;  /* [31:0]  r/o */
        } BF;
        uint32_t WORDVAL;
    } FP_PID3_REG;

    /* 0x02ff0: Flash patch CID register 0 */
    union {
        struct {
            uint32_t FP_CID0_VAL                 : 32;  /* [31:0]  r/o */
        } BF;
        uint32_t WORDVAL;
    } FP_CID0_REG;

    /* 0x02ff4: Flash patch CID register 1 */
    union {
        struct {
            uint32_t FP_CID1_VAL                 : 32;  /* [31:0]  r/o */
        } BF;
        uint32_t WORDVAL;
    } FP_CID1_REG;

    /* 0x02ff8: Flash patch CID register 2 */
    union {
        struct {
            uint32_t FP_CID2_VAL                 : 32;  /* [31:0]  r/o */
        } BF;
        uint32_t WORDVAL;
    } FP_CID2_REG;

    /* 0x02ffc: Flash patch CID register 3 */
    union {
        struct {
            uint32_t FP_CID3_VAL                 : 32;  /* [31:0]  r/o */
        } BF;
        uint32_t WORDVAL;
    } FP_CID3_REG;

    uint8_t zReserved0x03000[45060];  /* pad 0x03000 - 0x0e003 */

    /* 0x0e004: Interrupt Controller Type Register */
    union {
        struct {
            uint32_t INT_LINES_NUM               :  5;  /* [4:0]   r/o */
            uint32_t RESERVED_31_5               : 27;  /* [31:5]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } INT_CTRL_TYPE_REG;

    /* 0x0e008: Auxiliary Control Register */
    union {
        struct {
            uint32_t AUX_CTRL_DISMCYCINT         :  1;  /* [0]     r/w */
            uint32_t AUX_CTRL_DISDEFWBUF         :  1;  /* [1]     r/w */
            uint32_t AUX_CTRL_DISFOLD            :  1;  /* [2]     r/w */
            uint32_t RESERVED_31_3               : 29;  /* [31:3]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } AUX_CTRL_REG;

    uint8_t zReserved0x0e00c[4];  /* pad 0x0e00c - 0x0e00f */

    /* 0x0e010: SysTick Control and Status Register */
    union {
        struct {
            uint32_t SYSTICK_ENA                 :  1;  /* [0]     r/w */
            uint32_t SYSTICK_TICK_INT            :  1;  /* [1]     r/w */
            uint32_t SYSTICK_CLK_SOURCE          :  1;  /* [2]     r/w */
            uint32_t RESERVED_15_3               : 13;  /* [15:3]  rsvd */
            uint32_t SYSTICK_CNT_FLAG            :  1;  /* [16]    r/w */
            uint32_t RESERVED_31_17              : 15;  /* [31:17] rsvd */
        } BF;
        uint32_t WORDVAL;
    } SYSTICK_CTRL_STAT_REG;

    /* 0x0e014: SysTick Reload Value Register */
    union {
        struct {
            uint32_t SYSTICK_RLD_VAL             : 24;  /* [23:0]  r/w */
            uint32_t RESERVED_31_24              :  8;  /* [31:24] rsvd */
        } BF;
        uint32_t WORDVAL;
    } SYSTICK_RLD_VAL_REG;

    /* 0x0e018: SysTick Current Value Register */
    union {
        struct {
            uint32_t SYSTICK_CRT_VAL             : 24;  /* [23:0]  r/w */
            uint32_t RESERVED_31_24              :  8;  /* [31:24] rsvd */
        } BF;
        uint32_t WORDVAL;
    } SYSTICK_CRT_VAL_REG;

    /* 0x0e01c: SysTick Calibration Value Register */
    union {
        struct {
            uint32_t SYSTICK_TENMS_VAL           : 24;  /* [23:0]  r/o */
            uint32_t RESERVED_29_24              :  6;  /* [29:24] rsvd */
            uint32_t SYSTICK_SKEW                :  1;  /* [30]    r/o */
            uint32_t SYSTICK_NO_REF              :  1;  /* [31]    r/o */
        } BF;
        uint32_t WORDVAL;
    } SYSTICK_CALIB_VAL_REG;

    uint8_t zReserved0x0e020[224];  /* pad 0x0e020 - 0x0e0ff */

    /* 0x0e100-0x0e11c: Interrupt Set-Enable Register */
    union {
        struct {
            uint32_t INT_SET_ENA                 : 32;  /* [31:0]  r/w */
        } BF;
        uint32_t WORDVAL;
    } INT_SET_ENA_REG[8];

    uint8_t zReserved0x0e120[96];  /* pad 0x0e120 - 0x0e17f */

    /* 0x0e180-0x0e19c: Interrupt Clear-Enable Register */
    union {
        struct {
            uint32_t INT_CLR_ENA                 : 32;  /* [31:0]  r/w */
        } BF;
        uint32_t WORDVAL;
    } INT_CLR_ENA_REG[8];

    uint8_t zReserved0x0e1a0[96];  /* pad 0x0e1a0 - 0x0e1ff */

    /* 0x0e200-0x0e21c: Interrupt Set-Pending Register */
    union {
        struct {
            uint32_t INT_SET_PENDING             : 32;  /* [31:0]  r/w */
        } BF;
        uint32_t WORDVAL;
    } INT_SET_PENDING_REG[8];

    uint8_t zReserved0x0e220[96];  /* pad 0x0e220 - 0x0e27f */

    /* 0x0e280-0x0e29c: Interrupt Clear-Pending Register */
    union {
        struct {
            uint32_t INT_CLR_PENDING             : 32;  /* [31:0]  r/w */
        } BF;
        uint32_t WORDVAL;
    } INT_CLR_PENDING_REG[8];

    uint8_t zReserved0x0e2a0[96];  /* pad 0x0e2a0 - 0x0e2ff */

    /* 0x0e300-0x0e31c: Active Bit Register */
    union {
        struct {
            uint32_t INT_ACTIVE_BIT              : 32;  /* [31:0]  r/o */
        } BF;
        uint32_t WORDVAL;
    } INT_ACTIVE_BIT_REG[8];

    uint8_t zReserved0x0e320[224];  /* pad 0x0e320 - 0x0e3ff */

    /* 0x0e400-0x0e41c: Interrupt Priority Registers */
    union {
        struct {
            uint32_t INT_PRI                     : 32;  /* [31:0]  r/w */
        } BF;
        uint32_t WORDVAL;
    } INT_PRI_REG[8];

    uint8_t zReserved0x0e420[2272];  /* pad 0x0e420 - 0x0ecff */

    /* 0x0ed00: CPUID Base Register */
    union {
        struct {
            uint32_t CPUID_VREVISION             :  4;  /* [3:0]   r/o */
            uint32_t CPUID_PART_NO               : 12;  /* [15:4]  r/o */
            uint32_t CPUID_CONSTANT              :  4;  /* [19:16] r/o */
            uint32_t CPUID_VARIANT               :  4;  /* [23:20] r/o */
            uint32_t CPUID_IMPLEMENTER           :  8;  /* [31:24] r/o */
        } BF;
        uint32_t WORDVAL;
    } CPUID_BASE_REG;

    /* 0x0ed04: Interrupt Control State Register */
    union {
        struct {
            uint32_t INT_VECT_ACTIVE             :  9;  /* [8:0]   r/o */
            uint32_t RESERVED_10_9               :  2;  /* [10:9]  rsvd */
            uint32_t INT_RET_TO_BASE             :  1;  /* [11]    r/o */
            uint32_t INT_VECT_PENDING            : 10;  /* [21:12] r/o */
            uint32_t INT_ISR_PENDING             :  1;  /* [22]    r/o */
            uint32_t INT_ISR_PREEMPT             :  1;  /* [23]    r/o */
            uint32_t RESERVED_24                 :  1;  /* [24]    rsvd */
            uint32_t INT_PENDST_CLR              :  1;  /* [25]    w/o */
            uint32_t INT_PENDST_SET              :  1;  /* [26]    r/w */
            uint32_t INT_PENDSV_CLR              :  1;  /* [27]    w/o */
            uint32_t INT_PENDSV_SET              :  1;  /* [28]    r/w */
            uint32_t RESERVED_30_29              :  2;  /* [30:29] rsvd */
            uint32_t INT_NMI_PEND_SET            :  1;  /* [31]    r/w */
        } BF;
        uint32_t WORDVAL;
    } INT_CTRL_STATE_REG;

    /* 0x0ed08: Vector Table Offset Register */
    union {
        struct {
            uint32_t RESERVED_6_0                :  7;  /* [6:0]   rsvd */
            uint32_t VECT_TBL_OFF                : 22;  /* [28:7]  r/w */
            uint32_t VECT_TBL_BASE               :  1;  /* [29]    r/w */
            uint32_t RESERVED_31_30              :  2;  /* [31:30] rsvd */
        } BF;
        uint32_t WORDVAL;
    } VECT_TABLE_OFFSET_REG;

    /* 0x0ed0c: Application Interrupt and Reset Control Register */
    union {
        struct {
            uint32_t AIRC_VECT_RESET             :  1;  /* [0]     r/w */
            uint32_t AIRC_VECT_CLR_ACTIVE        :  1;  /* [1]     r/w */
            uint32_t AIRC_SYS_RESET_REQ          :  1;  /* [2]     r/w */
            uint32_t RESERVED_7_3                :  5;  /* [7:3]   rsvd */
            uint32_t AIRC_PRI_GROUP              :  3;  /* [10:8]  r/w */
            uint32_t RESERVED_14_11              :  4;  /* [14:11] rsvd */
            uint32_t AIRC_ENDIANESS              :  1;  /* [15]    r/w */
            uint32_t AIRC_VECT_KEY               : 16;  /* [31:16] r/w */
        } BF;
        uint32_t WORDVAL;
    } AIRC_REG;

    /* 0x0ed10: System Control Register */
    union {
        struct {
            uint32_t RESERVED_0                  :  1;  /* [0]     rsvd */
            uint32_t SC_SLEEP_ON_EXIT            :  1;  /* [1]     r/w */
            uint32_t SC_SLEEP_DEEP               :  1;  /* [2]     r/w */
            uint32_t RESERVED_3                  :  1;  /* [3]     rsvd */
            uint32_t SC_SEV_ON_PEND              :  1;  /* [4]     r/w */
            uint32_t RESERVED_31_5               : 27;  /* [31:5]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } SYS_CTRL_REG;

    /* 0x0ed14: Configuration Control Register */
    union {
        struct {
            uint32_t CC_NONE_BASE_THRD_ENA       :  1;  /* [0]     r/w */
            uint32_t CC_USER_SET_MPEND           :  1;  /* [1]     r/w */
            uint32_t RESERVED_2                  :  1;  /* [2]     rsvd */
            uint32_t CC_UNALIGN_TRP              :  1;  /* [3]     r/w */
            uint32_t CC_DIV_0_TRP                :  1;  /* [4]     r/w */
            uint32_t RESERVED_7_5                :  3;  /* [7:5]   rsvd */
            uint32_t CC_BFHFNMIGN                :  1;  /* [8]     r/w */
            uint32_t CC_STK_ALIGN                :  1;  /* [9]     r/w */
            uint32_t RESERVED_31_10              : 22;  /* [31:10] rsvd */
        } BF;
        uint32_t WORDVAL;
    } CONFIG_CTRL_REG;

    /* 0x0ed18-0x0ed20: System Handler Priority Registers */
    union {
        struct {
            uint32_t SYS_HANDLER_PRI             : 32;  /* [31:0]  r/w */
        } BF;
        uint32_t WORDVAL;
    } SYS_HANDLER_PRI_REG[3];

    /* 0x0ed24: System Handler Control and State Register */
    union {
        struct {
            uint32_t SHCSR_MEM_FAULT_ACT         :  1;  /* [0]     r/w */
            uint32_t SHCSR_BUS_FAULT_ACT         :  1;  /* [1]     r/w */
            uint32_t RESERVED_2                  :  1;  /* [2]     rsvd */
            uint32_t SHCSR_USG_FAULT_ACT         :  1;  /* [3]     r/w */
            uint32_t RESERVED_6_4                :  3;  /* [6:4]   rsvd */
            uint32_t SHCSR_SVCALL_ACT            :  1;  /* [7]     r/w */
            uint32_t SHCSR_MONITOR_ACT           :  1;  /* [8]     r/w */
            uint32_t RESERVED_9                  :  1;  /* [9]     rsvd */
            uint32_t SHCSR_PENDSV_ACT            :  1;  /* [10]    r/w */
            uint32_t SHCSR_SYSTICK_ACT           :  1;  /* [11]    r/w */
            uint32_t SHCSR_USG_FAULT_PENDED      :  1;  /* [12]    r/w */
            uint32_t SHCSR_MEM_FAULT_PENDED      :  1;  /* [13]    r/w */
            uint32_t SHCSR_BUS_FAULT_PENDED      :  1;  /* [14]    r/w */
            uint32_t SHCSR_SVCALL_PENDED         :  1;  /* [15]    r/w */
            uint32_t SHCSR_MEM_FAULT_ENA         :  1;  /* [16]    r/w */
            uint32_t SHCSR_BUS_FAULT_ENA         :  1;  /* [17]    r/w */
            uint32_t SHCSR_USG_FAULT_ENA         :  1;  /* [18]    r/w */
            uint32_t RESERVED_31_19              : 13;  /* [31:19] rsvd */
        } BF;
        uint32_t WORDVAL;
    } SYS_HANDLER_CTRL_STAT_REG;

    /* 0x0ed28: Memory Manage Fault Status Register */
    union {
        struct {
            uint32_t MEM_IACC_VIOL               :  1;  /* [0]     r/w */
            uint32_t MEM_DACC_VIOL               :  1;  /* [1]     r/w */
            uint32_t RESERVED_2                  :  1;  /* [2]     rsvd */
            uint32_t MEM_MUNSTK_ERR              :  1;  /* [3]     r/w */
            uint32_t MEM_MSTK_ERR                :  1;  /* [4]     r/w */
            uint32_t RESERVED_6_5                :  2;  /* [6:5]   rsvd */
            uint32_t MEM_MAR_VALID               :  1;  /* [7]     r/w */
            uint32_t BUS_FAULT_IBUS_ERR          :  1;  /* [8]     r/w */
            uint32_t BUS_FAULT_PRECIS_ERR        :  1;  /* [9]     r/w */
            uint32_t BUS_FAULT_IMPRECIS_ERR      :  1;  /* [10]    r/w */
            uint32_t BUS_FAULT_UNSTK_ERR         :  1;  /* [11]    r/w */
            uint32_t BUS_FAULT_STK_ERR           :  1;  /* [12]    r/w */
            uint32_t RESERVED_14_13              :  2;  /* [14:13] rsvd */
            uint32_t BUS_FAULT_ARVALID           :  1;  /* [15]    r/w */
            uint32_t USAGE_FAULT_UNDEFINSTER     :  1;  /* [16]    r/w */
            uint32_t USAGE_FAULT_INV_STATE       :  1;  /* [17]    r/w */
            uint32_t USAGE_FALUT_INV_PC          :  1;  /* [18]    r/w */
            uint32_t USAGE_FAULT_NO_CP           :  1;  /* [19]    r/w */
            uint32_t RESERVED_23_20              :  4;  /* [23:20] rsvd */
            uint32_t USAGE_FAULT_UNALIGNED       :  1;  /* [24]    r/w */
            uint32_t USAGE_FAULT_DIV_BY_ZERO     :  1;  /* [25]    r/w */
            uint32_t RESERVED_31_26              :  6;  /* [31:26] rsvd */
        } BF;
        uint32_t WORDVAL;
    } CONFIG_FAULT_STAT_REG;

    /* 0x0ed2c: Hard Fault Status Register */
    union {
        struct {
            uint32_t RESERVED_0                  :  1;  /* [0]     rsvd */
            uint32_t HFSR_VECT_TBL               :  1;  /* [1]     r/w */
            uint32_t RESERVED_29_2               : 28;  /* [29:2]  rsvd */
            uint32_t HFSR_FORCED                 :  1;  /* [30]    r/w */
            uint32_t HFSR_DEBUG_EVT              :  1;  /* [31]    r/w */
        } BF;
        uint32_t WORDVAL;
    } HARD_FAULT_STAT_REG;

    /* 0x0ed30: Debug Fault Status Register */
    union {
        struct {
            uint32_t DFSR_HALTED                 :  1;  /* [0]     r/w */
            uint32_t DFSR_BKPT                   :  1;  /* [1]     r/w */
            uint32_t DFSR_DWT_TRAP               :  1;  /* [2]     r/w */
            uint32_t DFSR_VCATCH                 :  1;  /* [3]     r/w */
            uint32_t DFSR_EXTERNAL               :  1;  /* [4]     r/w */
            uint32_t RESERVED_31_5               : 27;  /* [31:5]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } DBG_FAULT_STAT_REG;

    /* 0x0ed34: Memory Manage Fault Address Register */
    union {
        struct {
            uint32_t MMFAR_ADDR                  : 32;  /* [31:0]  r/w */
        } BF;
        uint32_t WORDVAL;
    } MEM_MANAGE_FAULT_ADDR_REG;

    /* 0x0ed38: Bus Fault Address Register */
    union {
        struct {
            uint32_t BFAR_ADDR                   : 32;  /* [31:0]  r/w */
        } BF;
        uint32_t WORDVAL;
    } BUS_FAULT_ADDRESS_REG;

    /* 0x0ed3c: Auxiliary Fault Status Register */
    union {
        struct {
            uint32_t AFSR_IMP_DEF                : 32;  /* [31:0]  r/w */
        } BF;
        uint32_t WORDVAL;
    } AUX_FAULT_STATUS_REG;

    /* 0x0ed40-0x0ed44: Processor Feature register0 */
    union {
        struct {
            uint32_t PFR0_VAL                    : 32;  /* [31:0]  r/o */
        } BF;
        uint32_t WORDVAL;
    } PROC_FEATURE_REG[2];

    /* 0x0ed48: Debug Feature register0 */
    union {
        struct {
            uint32_t DFR0_VAL                    : 32;  /* [31:0]  r/o */
        } BF;
        uint32_t WORDVAL;
    } DBG_FEATURE_REG0;

    /* 0x0ed4c: Auxiliary Feature register0 */
    union {
        struct {
            uint32_t AFR0_VAL                    : 32;  /* [31:0]  r/o */
        } BF;
        uint32_t WORDVAL;
    } AUX_FEATURE_REG0;

    /* 0x0ed50-0x0ed5c: Memory Model Feature register0 */
    union {
        struct {
            uint32_t MMFR0_VAL                   : 32;  /* [31:0]  r/o */
        } BF;
        uint32_t WORDVAL;
    } MEM_MODEL_FEATURE_REG[4];

    /* 0x0ed60-0x0ed70: ISA Feature register0 */
    union {
        struct {
            uint32_t ISAR0_VAL                   : 32;  /* [31:0]  r/o */
        } BF;
        uint32_t WORDVAL;
    } ISA_FEATURE_REG[5];

    uint8_t zReserved0x0ed74[28];  /* pad 0x0ed74 - 0x0ed8f */

    /* 0x0ed90: MPU Type Register */
    union {
        struct {
            uint32_t MPU_SEPA_RATE               :  1;  /* [0]     r/o */
            uint32_t RESERVED_7_1                :  7;  /* [7:1]   rsvd */
            uint32_t MPU_DREGION                 :  8;  /* [15:8]  r/o */
            uint32_t MPU_IREGION                 :  8;  /* [23:16] r/o */
            uint32_t RESERVED_31_24              :  8;  /* [31:24] rsvd */
        } BF;
        uint32_t WORDVAL;
    } MPU_TYPE_REG;

    /* 0x0ed94: MPU Control Register */
    union {
        struct {
            uint32_t MPU_ENABLE                  :  1;  /* [0]     r/w */
            uint32_t MPU_HFNMI_ENA               :  1;  /* [1]     r/w */
            uint32_t MPU_PRIVDEF_ENA             :  1;  /* [2]     r/w */
            uint32_t RESERVED_31_3               : 29;  /* [31:3]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } MPU_CTRL_REG;

    /* 0x0ed98: MPU Region Number Register */
    union {
        struct {
            uint32_t MPU_REGION                  :  8;  /* [7:0]   r/w */
            uint32_t RESERVED_31_8               : 24;  /* [31:8]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } MPU_REGION_NUM_REG;

    /* 0x0ed9c: MPU Region Base Address Register */
    union {
        struct {
            uint32_t MPU_REGION_BASE_ADDR_REGION :  4;  /* [3:0]   r/w */
            uint32_t MPU_REGION_BASE_ADDR_VALID  :  1;  /* [4]     r/w */
            uint32_t MPU_REGION_BASE_ADDR        : 27;  /* [31:5]  r/w */
        } BF;
        uint32_t WORDVAL;
    } MPU_REGION_BASE_ADDR_REG;

    /* 0x0eda0: MPU Region Attribute and Size Register */
    union {
        struct {
            uint32_t MPU_REGION_ATTR_SIZE_ENA    :  1;  /* [0]     r/w */
            uint32_t MPU_REGION_ATTR_SIZE_SIZE   :  5;  /* [5:1]   r/w */
            uint32_t RESERVED_7_6                :  2;  /* [7:6]   rsvd */
            uint32_t MPU_REGION_ATTR_SIZE_SRD    :  8;  /* [15:8]  r/w */
            uint32_t MPU_REGION_ATTR_SIZE_B      :  1;  /* [16]    r/w */
            uint32_t MPU_REGION_ATTR_SIZE_C      :  1;  /* [17]    r/w */
            uint32_t MPU_REGION_ATTR_SIZE_S      :  1;  /* [18]    r/w */
            uint32_t MPU_REGION_ATTR_SIZE_TEX    :  3;  /* [21:19] r/w */
            uint32_t RESERVED_23_22              :  2;  /* [23:22] rsvd */
            uint32_t MPU_REGION_ATTR_SIZE_AP     :  3;  /* [26:24] r/w */
            uint32_t RESERVED_27                 :  1;  /* [27]    rsvd */
            uint32_t MPU_REGION_ATTR_SIZE_XN     :  1;  /* [28]    r/w */
            uint32_t RESERVED_31_29              :  3;  /* [31:29] rsvd */
        } BF;
        uint32_t WORDVAL;
    } MPU_REGION_ATTR_SIZE_REG;

    uint8_t zReserved0x0eda4[76];  /* pad 0x0eda4 - 0x0edef */

    /* 0x0edf0: Debug Halting Control and Status Register */
    union {
        struct {
            uint32_t DHCSR_C_DEBUGEN             :  1;  /* [0]     r/w */
            uint32_t DHCSR_C_HALT                :  1;  /* [1]     r/w */
            uint32_t DHCSR_C_STEP                :  1;  /* [2]     r/w */
            uint32_t DHCSR_C_MASKINTS            :  1;  /* [3]     r/w */
            uint32_t RESERVED_4                  :  1;  /* [4]     rsvd */
            uint32_t DHCSR_C_SNAPSTALL           :  1;  /* [5]     r/w */
            uint32_t RESERVED_15_6               : 10;  /* [15:6]  rsvd */
            uint32_t DHCSR_S_REGRDY              :  1;  /* [16]    r/o */
            uint32_t DHCSR_S_HALT                :  1;  /* [17]    r/o */
            uint32_t DHCSR_S_SLEEP               :  1;  /* [18]    r/o */
            uint32_t DHCSR_S_LOCKUP              :  1;  /* [19]    r/o */
            uint32_t RESERVED_23_20              :  4;  /* [23:20] rsvd */
            uint32_t DHCSR_S_RETIRE_ST           :  1;  /* [24]    r/o */
            uint32_t DHCSR_S_RESET_ST            :  1;  /* [25]    r/o */
            uint32_t RESERVED_31_26              :  6;  /* [31:26] rsvd */
        } BF;
        uint32_t WORDVAL;
    } DBG_HAL_CTRL_STAT_REG;

    /* 0x0edf4: Debug Core Register Selector Register */
    union {
        struct {
            uint32_t DCRSR_REG_SEL               :  5;  /* [4:0]   w/o */
            uint32_t RESERVED_15_5               : 11;  /* [15:5]  rsvd */
            uint32_t DCRSR_REGWNR                :  1;  /* [16]    w/o */
            uint32_t RESERVED_31_17              : 15;  /* [31:17] rsvd */
        } BF;
        uint32_t WORDVAL;
    } DBG_CORE_REG_SEL_REG;

    /* 0x0edf8: Debug Core Register Data Register */
    union {
        struct {
            uint32_t DCRDR_VAL                   : 32;  /* [31:0]  r/w */
        } BF;
        uint32_t WORDVAL;
    } DBG_CORE_REG_DATA_REG;

    /* 0x0edfc: Debug Exception and Monitor Control Register. */
    union {
        struct {
            uint32_t DEMCR_VC_CORE_RESET         :  1;  /* [0]     r/w */
            uint32_t RESERVED_3_1                :  3;  /* [3:1]   rsvd */
            uint32_t DEMCR_VC_MM_ERR             :  1;  /* [4]     r/w */
            uint32_t DEMCR_VC_NOCP_ERR           :  1;  /* [5]     r/w */
            uint32_t DEMCR_VC_CHK_ERR            :  1;  /* [6]     r/w */
            uint32_t DEMCR_VC_STAT_ERR           :  1;  /* [7]     r/w */
            uint32_t DEMCR_VC_BUS_ERR            :  1;  /* [8]     r/w */
            uint32_t DEMCR_VC_INT_ERR            :  1;  /* [9]     r/w */
            uint32_t DEMCR_VC_HARD_ERR           :  1;  /* [10]    r/w */
            uint32_t RESERVED_15_11              :  5;  /* [15:11] rsvd */
            uint32_t DEMCR_MON_EN                :  1;  /* [16]    r/w */
            uint32_t DEMCR_MON_PEND              :  1;  /* [17]    r/w */
            uint32_t DEMCR_MON_STEP              :  1;  /* [18]    r/w */
            uint32_t DEMCR_MON_REQ               :  1;  /* [19]    r/w */
            uint32_t RESERVED_23_20              :  4;  /* [23:20] rsvd */
            uint32_t DEMCR_TRC_ENA               :  1;  /* [24]    r/w */
            uint32_t RESERVED_31_25              :  7;  /* [31:25] rsvd */
        } BF;
        uint32_t WORDVAL;
    } DBG_EXC_MON_CTRL_REG;

    uint8_t zReserved0x0ee00[256];  /* pad 0x0ee00 - 0x0eeff */

    /* 0x0ef00: Software Trigger Interrupt Register */
    union {
        struct {
            uint32_t STIR_INT_ID                 :  9;  /* [8:0]   w/o */
            uint32_t RESERVED_31_9               : 23;  /* [31:9]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } SOFT_TRIGGER_INT_REG;

    uint8_t zReserved0x0ef04[204];  /* pad 0x0ef04 - 0x0efcf */

    /* 0x0efd0: NVIC Peripheral identification register (PID4) */
    union {
        struct {
            uint32_t NVIC_PID4_VAL               : 32;  /* [31:0]  r/o */
        } BF;
        uint32_t WORDVAL;
    } NVIC_PID4_REG;

    /* 0x0efd4: NVIC Peripheral identification register (PID5) */
    union {
        struct {
            uint32_t NVIC_PID5_VAL               : 32;  /* [31:0]  r/o */
        } BF;
        uint32_t WORDVAL;
    } NVIC_PID5_REG;

    /* 0x0efd8: NVIC Peripheral identification register (PID6) */
    union {
        struct {
            uint32_t NVIC_PID6_VAL               : 32;  /* [31:0]  r/o */
        } BF;
        uint32_t WORDVAL;
    } NVIC_PID6_REG;

    /* 0x0efdc: NVIC Peripheral identification register (PID7) */
    union {
        struct {
            uint32_t NVIC_PID7_VAL               : 32;  /* [31:0]  r/o */
        } BF;
        uint32_t WORDVAL;
    } NVIC_PID7_REG;

    /* 0x0efe0: NVIC Peripheral identification register Bits [7:0] (PID0) */
    union {
        struct {
            uint32_t NVIC_PID0_VAL               : 32;  /* [31:0]  r/o */
        } BF;
        uint32_t WORDVAL;
    } NVIC_PID0_REG;

    /* 0x0efe4: NVIC Peripheral identification register Bits [15:8] (PID1) */
    union {
        struct {
            uint32_t NVIC_PID1_VAL               : 32;  /* [31:0]  r/o */
        } BF;
        uint32_t WORDVAL;
    } NVIC_PID1_REG;

    /* 0x0efe8: NVIC Peripheral identification register Bits [23:16] (PID2) */
    union {
        struct {
            uint32_t NVIC_PID2_VAL               : 32;  /* [31:0]  r/o */
        } BF;
        uint32_t WORDVAL;
    } NVIC_PID2_REG;

    /* 0x0efec: NVIC Peripheral identification register Bits [31:24] (PID3) */
    union {
        struct {
            uint32_t NVIC_PID3_VAL               : 32;  /* [31:0]  r/o */
        } BF;
        uint32_t WORDVAL;
    } NVIC_PID3_REG;

    /* 0x0eff0: NVIC Component identification register Bits [7:0] (CID0) */
    union {
        struct {
            uint32_t NVIC_CID0_VAL               : 32;  /* [31:0]  r/o */
        } BF;
        uint32_t WORDVAL;
    } NVIC_CID0_REG;

    /* 0x0eff4: NVIC Component identification register Bits [15:8] (CID1) */
    union {
        struct {
            uint32_t NVIC_CID1_VAL               : 32;  /* [31:0]  r/o */
        } BF;
        uint32_t WORDVAL;
    } NVIC_CID1_REG;

    /* 0x0eff8: NVIC Component identification register Bits [23:16] (CID2) */
    union {
        struct {
            uint32_t NVIC_CID2_VAL               : 32;  /* [31:0]  r/o */
        } BF;
        uint32_t WORDVAL;
    } NVIC_CID2_REG;

    /* 0x0effc: NVIC Component identification register Bits [31:24] (CID3) */
    union {
        struct {
            uint32_t NVIC_CID3_VAL               : 32;  /* [31:0]  r/o */
        } BF;
        uint32_t WORDVAL;
    } NVIC_CID3_REG;

    uint8_t zReserved0x0f000[200704];  /* pad 0x0f000 - 0x3ffff */

    /* 0x40000: Supported Sync Port Sizes Register */
    union {
        struct {
            uint32_t TPIU_SSPS_VAL               :  4;  /* [3:0]   r/w */
            uint32_t RESERVED_31_4               : 28;  /* [31:4]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } TPIU_SUPT_SYNC_PORT_SIZE_REG;

    /* 0x40004: Current Sync Port Size Register */
    union {
        struct {
            uint32_t TPIU_CSPS_VAL               : 32;  /* [31:0]  r/w */
        } BF;
        uint32_t WORDVAL;
    } TPIU_CRT_SYNC_PORT_SIZE_REG;

    uint8_t zReserved0x40008[8];  /* pad 0x40008 - 0x4000f */

    /* 0x40010: Async Clock Prescaler Register */
    union {
        struct {
            uint32_t TPIU_ACP_PRESCALER          : 13;  /* [12:0]  r/w */
            uint32_t RESERVED_31_13              : 19;  /* [31:13] rsvd */
        } BF;
        uint32_t WORDVAL;
    } TPIU_ASYNC_CLK_PSCL_REG;

    uint8_t zReserved0x40014[220];  /* pad 0x40014 - 0x400ef */

    /* 0x400f0: Selected Pin Protocol Register */
    union {
        struct {
            uint32_t TPIU_SPP_PROTOCOL           :  2;  /* [1:0]   r/w */
            uint32_t RESERVED_31_2               : 30;  /* [31:2]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } TPIU_SEL_PIN_PTL_REG;

    uint8_t zReserved0x400f4[524];  /* pad 0x400f4 - 0x402ff */

    /* 0x40300: Formatter and Flush Status Register */
    union {
        struct {
            uint32_t TPIU_FFS_FLIN_PROG          :  1;  /* [0]     r/o */
            uint32_t TPIU_FFS_FT_STOPPED         :  1;  /* [1]     r/o */
            uint32_t TPIU_FFS_TC_PRESENT         :  1;  /* [2]     r/o */
            uint32_t TPIU_FFS_FT_NONSTOP         :  1;  /* [3]     r/o */
            uint32_t RESERVED_31_4               : 28;  /* [31:4]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } TPIU_FMT_FLUSH_STAT_REG;

    /* 0x40304: Formatter and Flush Control Register */
    union {
        struct {
            uint32_t TPIU_FFC_EN_FTC             :  1;  /* [0]     r/w */
            uint32_t TPIU_FFC_EN_FCONT           :  1;  /* [1]     r/w */
            uint32_t RESERVED_3_2                :  2;  /* [3:2]   rsvd */
            uint32_t TPIU_FFC_FON_FLLN           :  1;  /* [4]     r/w */
            uint32_t TPIU_FFC_FON_TRIG           :  1;  /* [5]     r/w */
            uint32_t TPIU_FFC_FON_MAN            :  1;  /* [6]     r/w */
            uint32_t RESERVED_7                  :  1;  /* [7]     rsvd */
            uint32_t TPIU_FFC_TRIG_IN            :  1;  /* [8]     r/w */
            uint32_t TPIU_FFC_TRIG_EVT           :  1;  /* [9]     r/w */
            uint32_t TPIU_FFC_TRIG_FI            :  1;  /* [10]    r/w */
            uint32_t RESERVED_11                 :  1;  /* [11]    rsvd */
            uint32_t TPIU_FFC_STOP_FI            :  1;  /* [12]    r/w */
            uint32_t TPIU_FFC_STOP_TRIG          :  1;  /* [13]    r/w */
            uint32_t RESERVED_31_14              : 18;  /* [31:14] rsvd */
        } BF;
        uint32_t WORDVAL;
    } TPIU_FMT_FLUSH_CTRL_REG;

    /* 0x40308: Formatter Synchronization Counter Register */
    union {
        struct {
            uint32_t TPIU_FSC_VAL                : 32;  /* [31:0]  r/o */
        } BF;
        uint32_t WORDVAL;
    } TPIU_FMT_SYNC_CNT_REG;

    uint8_t zReserved0x4030c[3036];  /* pad 0x4030c - 0x40ee7 */

    /* 0x40ee8: Integration Register: TRIGGER */
    union {
        struct {
            uint32_t TPIU_INTG_TRIGGER_INPUT_VAL :  1;  /* [0]     r/o */
            uint32_t RESERVED_31_1               : 31;  /* [31:1]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } TPIU_INTG_TRIGGER_REG;

    /* 0x40eec: Integration Register : FIFO data 0 */
    union {
        struct {
            uint32_t TPIU_INTG_FIFO_D0_FIFO1_D0  :  8;  /* [7:0]   r/o */
            uint32_t TPIU_INTG_FIFO_D0_FIFO1_D1  :  8;  /* [15:8]  r/o */
            uint32_t TPIU_INTG_FIFO_D0_FIFO1_D2  :  8;  /* [23:16] r/o */
            uint32_t TPIU_INTG_FIFO_D0_WP1       :  2;  /* [25:24] r/o */
            uint32_t TPIU_INTG_FIFO_D0_ATVALID1S :  1;  /* [26]    r/o */
            uint32_t TPIU_INTG_FIFO_D0_WP2       :  2;  /* [28:27] r/o */
            uint32_t TPIU_INTG_FIFO_D0_ATVALID2S :  1;  /* [29]    r/o */
            uint32_t RESERVED_31_30              :  2;  /* [31:30] rsvd */
        } BF;
        uint32_t WORDVAL;
    } TPIU_INTG_FIFO_D0_REG;

    /* 0x40ef0: Integration Test Register-ITATBCTR2 */
    union {
        struct {
            uint32_t TPIU_ITATBCTR2_ATREADY      :  1;  /* [0]     r/o */
            uint32_t RESERVED_31_1               : 31;  /* [31:1]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } TPIU_ITATBCTR2_REG;

    uint8_t zReserved0x40ef4[4];  /* pad 0x40ef4 - 0x40ef7 */

    /* 0x40ef8: Integration Test Register-ITATBCTR0 */
    union {
        struct {
            uint32_t TPIU_ITATBCTR0_ATVALID      :  1;  /* [0]     r/o */
            uint32_t RESERVED_31_1               : 31;  /* [31:1]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } TPIU_ITATBCTR0_REG;

    /* 0x40efc: Integration Register : FIFO data 1 */
    union {
        struct {
            uint32_t TPIU_INTG_FIFO_D1_FIFO2_D0  :  8;  /* [7:0]   r/o */
            uint32_t TPIU_INTG_FIFO_D1_FIFO2_D1  :  8;  /* [15:8]  r/o */
            uint32_t TPIU_INTG_FIFO_D1_FIFO2_D2  :  8;  /* [23:16] r/o */
            uint32_t TPIU_INTG_FIFO_D1_WP1       :  2;  /* [25:24] r/o */
            uint32_t TPIU_INTG_FIFO_D1_ATVALID1S :  1;  /* [26]    r/o */
            uint32_t TPIU_INTG_FIFO_D1_WP2       :  2;  /* [28:27] r/o */
            uint32_t TPIU_INTG_FIFO_D1_ATVALID2S :  1;  /* [29]    r/o */
            uint32_t RESERVED_31_30              :  2;  /* [31:30] rsvd */
        } BF;
        uint32_t WORDVAL;
    } TPIU_INTG_FIFO_D1_REG;

    /* 0x40f00: Integration Mode Control Register */
    union {
        struct {
            uint32_t TPIU_IMC_INTG_TEST_MODE     :  1;  /* [0]     r/w */
            uint32_t TPIU_IMC_FIFO_TEST_MODE     :  1;  /* [1]     r/w */
            uint32_t RESERVED_31_2               : 30;  /* [31:2]  rsvd */
        } BF;
        uint32_t WORDVAL;
    } TPIU_INTG_MODE_CTRL_REG;

    uint8_t zReserved0x40f04[156];  /* pad 0x40f04 - 0x40f9f */

    /* 0x40fa0: Claim Tag Set Register */
    union {
        struct {
            uint32_t TPIU_CTS_VAL                : 32;  /* [31:0]  r/w */
        } BF;
        uint32_t WORDVAL;
    } TPIU_CLAIM_TAG_SET_REG;

    /* 0x40fa4: Claim Tag Clear Register */
    union {
        struct {
            uint32_t TPIU_CTC_VAL                : 32;  /* [31:0]  r/w */
        } BF;
        uint32_t WORDVAL;
    } TPIU_CLAIM_TAG_CLR_REG;

    uint8_t zReserved0x40fa8[32];  /* pad 0x40fa8 - 0x40fc7 */

    /* 0x40fc8: Device ID Register */
    union {
        struct {
            uint32_t TPIU_DEV_ID_VAL             : 32;  /* [31:0]  r/o */
        } BF;
        uint32_t WORDVAL;
    } TPIU_DEV_ID_REG;

    uint8_t zReserved0x40fcc[4];  /* pad 0x40fcc - 0x40fcf */

    /* 0x40fd0: TPIU PID Register 4 */
    union {
        struct {
            uint32_t TPIU_PID4_VAL               : 32;  /* [31:0]  r/o */
        } BF;
        uint32_t WORDVAL;
    } TPIU_PID4_REG;

    /* 0x40fd4: TPIU PID Register 5 */
    union {
        struct {
            uint32_t TPIU_PID5_VAL               : 32;  /* [31:0]  r/o */
        } BF;
        uint32_t WORDVAL;
    } TPIU_PID5_REG;

    /* 0x40fd8: TPIU PID Register 6 */
    union {
        struct {
            uint32_t TPIU_PID6_VAL               : 32;  /* [31:0]  r/o */
        } BF;
        uint32_t WORDVAL;
    } TPIU_PID6_REG;

    /* 0x40fdc: TPIU PID Register 7 */
    union {
        struct {
            uint32_t TPIU_PID7_VAL               : 32;  /* [31:0]  r/o */
        } BF;
        uint32_t WORDVAL;
    } TPIU_PID7_REG;

    /* 0x40fe0: TPIU PID Register 0 */
    union {
        struct {
            uint32_t TPIU_PID0_VAL               : 32;  /* [31:0]  r/o */
        } BF;
        uint32_t WORDVAL;
    } TPIU_PID0_REG;

    /* 0x40fe4: TPIU PID Register 1 */
    union {
        struct {
            uint32_t TPIU_PID1_VAL               : 32;  /* [31:0]  r/o */
        } BF;
        uint32_t WORDVAL;
    } TPIU_PID1_REG;

    /* 0x40fe8: TPIU PID Register 2 */
    union {
        struct {
            uint32_t TPIU_PID2_VAL               : 32;  /* [31:0]  r/o */
        } BF;
        uint32_t WORDVAL;
    } TPIU_PID2_REG;

    /* 0x40fec: TPIU PID Register 3 */
    union {
        struct {
            uint32_t TPIU_PID3_VAL               : 32;  /* [31:0]  r/o */
        } BF;
        uint32_t WORDVAL;
    } TPIU_PID3_REG;

    /* 0x40ff0: TPIU CID Register 0 */
    union {
        struct {
            uint32_t TPIU_CID0_VAL               : 32;  /* [31:0]  r/o */
        } BF;
        uint32_t WORDVAL;
    } TPIU_CID0_REG;

    /* 0x40ff4: TPIU CID Register 1 */
    union {
        struct {
            uint32_t TPIU_CID1_VAL               : 32;  /* [31:0]  r/o */
        } BF;
        uint32_t WORDVAL;
    } TPIU_CID1_REG;

    /* 0x40ff8: TPIU CID Register 2 */
    union {
        struct {
            uint32_t TPIU_CID2_VAL               : 32;  /* [31:0]  r/o */
        } BF;
        uint32_t WORDVAL;
    } TPIU_CID2_REG;

    /* 0x40ffc: TPIU CID Register 3 */
    union {
        struct {
            uint32_t TPIU_CID3_VAL               : 32;  /* [31:0]  r/o */
        } BF;
        uint32_t WORDVAL;
    } TPIU_CID3_REG;

};

typedef volatile struct scs_reg scs_reg_t;

#ifdef SCS_IMPL
BEGIN_REG_SECTION(scs_registers)
scs_reg_t SCSREG;
END_REG_SECTION(scs_registers)
#else
extern scs_reg_t SCSREG;
#endif
