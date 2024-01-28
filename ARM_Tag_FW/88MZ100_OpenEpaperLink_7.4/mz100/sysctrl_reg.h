#pragma once

struct sysctrl_reg {
    /* 0x00: Chip revision ID Register */
    union {
        struct {
            uint32_t PROJECT_REV_THIRD_PART  :  3;  /* [2:0]   r/o */
            uint32_t PROJECT_REV_SECOND_PART :  3;  /* [5:3]   r/o */
            uint32_t PROJECT_REV_FIRST_PART  :  3;  /* [8:6]   r/o */
            uint32_t PROJECT_ID              :  5;  /* [13:9]  r/o */
            uint32_t RESERVED_15_14          :  2;  /* [15:14] rsvd */
            uint32_t PROCESS_ID              :  4;  /* [19:16] r/o */
            uint32_t FOUNDRY_ID              :  4;  /* [23:20] r/o */
            uint32_t COMPANY_ID              :  8;  /* [31:24] r/o */
        } BF;
        uint32_t WORDVAL;
    } REV_ID;

    uint8_t zReserved0x04[44];  /* pad 0x04 - 0x2f */

    /* 0x30: DMA Handshaking Mapping Register */
    union {
        struct {
            uint32_t MAPPING_0               :  3;  /* [2:0]   r/w */
            uint32_t MAPPING_1               :  3;  /* [5:3]   r/w */
            uint32_t MAPPING_2               :  3;  /* [8:6]   r/w */
            uint32_t MAPPING_3               :  3;  /* [11:9]  r/w */
            uint32_t MAPPING_4               :  3;  /* [14:12] r/w */
            uint32_t MAPPING_5               :  3;  /* [17:15] r/w */
            uint32_t RESERVED_31_18          : 14;  /* [31:18] rsvd */
        } BF;
        uint32_t WORDVAL;
    } DMA_HS;

};

typedef volatile struct sysctrl_reg sysctrl_reg_t;

#ifdef SYSCTRL_IMPL
BEGIN_REG_SECTION(sysctrl_registers)
sysctrl_reg_t SYSCTRLREG;
END_REG_SECTION(sysctrl_registers)
#else
extern sysctrl_reg_t SYSCTRLREG;
#endif
