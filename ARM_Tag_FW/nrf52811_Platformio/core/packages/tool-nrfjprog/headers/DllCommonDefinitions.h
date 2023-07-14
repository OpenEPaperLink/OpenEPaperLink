#ifndef DLL_COMMON_DEFINITIONS_H
#define DLL_COMMON_DEFINITIONS_H

#include <stdint.h>

#if defined(__cplusplus)
extern "C" {
#endif


#define MIN_JLINK_MAJOR_VERSION                         (5UL)
#define MIN_JLINK_MINOR_VERSION                         (2UL)

#define JLINKARM_SWD_MIN_SPEED_KHZ                      (125UL)
#define JLINKARM_SWD_DEFAULT_SPEED_KHZ                  (2000UL)
#define JLINKARM_SWD_MAX_SPEED_KHZ                      (50000UL)
    
/* Deprecated Macro. Use result of NRFJPROG_read_ram_sections_count() function instead. */
#define MAX_RAM_BLOCKS      (16)
    
typedef enum {
    RAM_OFF = 0,
    RAM_ON,
} ram_section_power_status_t;


/* Identified device versions of nRF devices. */
typedef enum {
    R0 = 0,
    R1,
    R2,
    R3,
    R4,
    R5,
    R6,
    R7,
    R8,
    R9,
    R10,
    R11,
    R12,
    R13,
    R14,
    R15,
    XPSR,
    MSP,
    PSP 
} cpu_registers_t;


/* Possible readback protection status. */
typedef enum {
    NONE = 0,
    REGION_0,
    ALL,
    BOTH,
} readback_protection_status_t;


/* Possible region 0 source. */
typedef enum {
    NO_REGION_0 = 0,
    FACTORY,
    USER,
} region_0_source_t;


/* Identified device versions of nRF devices. */
typedef enum {
    UNKNOWN                             = 0,
    
    /* nRF51 versions. */
    /* See http://infocenter.nordicsemi.com/topic/com.nordic.infocenter.nrf51/dita/nrf51/pdflinks/nrf51_comp_matrix.html */
    NRF51xxx_xxAA_REV1                  = 1, 
    NRF51xxx_xxAA_REV2                  = 2, 
    
    NRF51xxx_xxAA_REV3                  = 3, 
    NRF51xxx_xxAB_REV3                  = 4, 
    NRF51xxx_xxAC_REV3                  = 5, 

    NRF51802_xxAA_REV3                  = 6, 
    NRF51801_xxAB_REV3                  = 17,

    /* Deprecated NRF51 enumerators. */
    NRF51_XLR1                          = 1,  /* Please do not use in new code. Kept for backwards compatibility. */
    NRF51_XLR2                          = 2,  /* Please do not use in new code. Kept for backwards compatibility. */ 
    NRF51_XLR3                          = 3,  /* Please do not use in new code. Kept for backwards compatibility. */ 
    NRF51_L3                            = 4,  /* Please do not use in new code. Kept for backwards compatibility. */ 
    NRF51_XLR3P                         = 5,  /* Please do not use in new code. Kept for backwards compatibility. */ 
    NRF51_XLR3LC                        = 6,  /* Please do not use in new code. Kept for backwards compatibility. */ 
    
    /* nRF52832 versions. */
    /* See http://infocenter.nordicsemi.com/topic/com.nordic.infocenter.nrf52/dita/nrf52/compatibility_matrix/nrf52832_comp_matrix.html */
    NRF52832_xxAA_ENGA                  = 7,
    NRF52832_xxAA_ENGB                  = 8,
    NRF52832_xxAA_REV1                  = 9, 
    NRF52832_xxAA_REV2                  = 19,
    NRF52832_xxAA_FUTURE                = 11,
    
    NRF52832_xxAB_REV1                  = 15,
    NRF52832_xxAB_REV2                  = 20,
    NRF52832_xxAB_FUTURE                = 16,   

    /* nRF52840 versions. */
    /* See http://infocenter.nordicsemi.com/topic/com.nordic.infocenter.nrf52/dita/nrf52/compatibility_matrix/nrf52840_comp_matrix.html */
    NRF52840_xxAA_ENGA                  = 10,
    NRF52840_xxAA_ENGB                  = 21,
    NRF52840_xxAA_REV1                  = 18,
    NRF52840_xxAA_FUTURE                = 12,
    
    /* nRF52810 versions. */
    NRF52810_xxAA_REV1                  = 13,
    NRF52810_xxAA_FUTURE                = 14,

    /* Deprecated nRF52 enumerators. */
    NRF52_FP1_ENGA                      = 7,  /* Please do not use in new code. Kept for backwards compatibility. */
    NRF52_FP1_ENGB                      = 8,  /* Please do not use in new code. Kept for backwards compatibility. */
    NRF52_FP1                           = 9,  /* Please do not use in new code. Kept for backwards compatibility. */
    NRF52_FP1_FUTURE                    = 11, /* Please do not use in new code. Kept for backwards compatibility. */
    NRF52_FP2_ENGA                      = 10, /* Please do not use in new code. Kept for backwards compatibility. */
 
} device_version_t;

/* Identified types of nRF devices */
typedef enum {
    NRF51_FAMILY,
    NRF52_FAMILY,
    UNKNOWN_FAMILY = 99
} device_family_t;

/* Possible rtt channel directions */
typedef enum {
    UP_DIRECTION = 0,
    DOWN_DIRECTION = 1
} rtt_direction_t;

/* QSPI erase length */
typedef enum {
    ERASE4KB  = 0,
    ERASE32KB = 3,
    ERASE64KB = 1,
    ERASEALL  = 2
} qspi_erase_len_t;

/* QSPI read and write modes */
typedef enum {
    FASTREAD = 0,
    READ2O   = 1,
    READ2IO  = 2,
    READ4O   = 3,
    READ4IO  = 4
} qspi_read_mode_t;

typedef enum {
    PP    = 0,
    PP2O  = 1,
    PP4O  = 2,
    PP4IO = 3
} qspi_write_mode_t;

typedef enum {
    BIT24 = 0,
    BIT32 = 1
} qspi_address_mode_t;

/* QSPI frequency */
typedef enum {
    M2  = 15,
    M4  = 7,
    M8  = 3,
    M16 = 1,
    M32 = 0
} qspi_frequency_t;

/* QSPI SPI mode */
typedef enum {
    MODE0 = 0,
    MODE3 = 1
} qspi_spi_mode_t;

/* QSPI custom level IO2 and IO3 */
typedef enum {
    LEVEL_HIGH = 1,
    LEVEL_LOW = 0
} qspi_custom_level_io_t;

/* QSPI custom PPSize */
typedef enum {
    PAGE256 = 0,
    PAGE512 = 1
} qspi_page_program_size_t;

/* QSPI initialization  */
typedef struct {
    qspi_read_mode_t read_mode;
    qspi_write_mode_t write_mode;
    qspi_address_mode_t address_mode;
    qspi_frequency_t frequency;
    qspi_spi_mode_t spi_mode;
    uint32_t sck_delay;
    qspi_custom_level_io_t custom_instruction_io2_level;
    qspi_custom_level_io_t custom_instruction_io3_level;
    uint32_t CSN_pin;
    uint32_t CSN_port;
    uint32_t SCK_pin;
    uint32_t SCK_port;
    uint32_t DIO0_pin;
    uint32_t DIO0_port;
    uint32_t DIO1_pin;
    uint32_t DIO1_port;
    uint32_t DIO2_pin;
    uint32_t DIO2_port;
    uint32_t DIO3_pin;
    uint32_t DIO3_port;
    uint32_t WIP_index;
    qspi_page_program_size_t pp_size;
} qspi_init_params_t;


/* Every DLL function has either a void or nrfjprogdll_err_t return type. */
typedef enum
{
    SUCCESS                                     = 0,
    
    /* PC Issues */
    OUT_OF_MEMORY                               = -1, 
    
    /* Wrong use of dll errors. */
    INVALID_OPERATION                           = -2,
    INVALID_PARAMETER                           = -3,
    INVALID_DEVICE_FOR_OPERATION                = -4,
    WRONG_FAMILY_FOR_DEVICE                     = -5,
    
    /* Connection issues. */
    EMULATOR_NOT_CONNECTED                      = -10,
    CANNOT_CONNECT                              = -11,
    LOW_VOLTAGE                                 = -12,
    NO_EMULATOR_CONNECTED                       = -13,

    /* Device issues. */
    NVMC_ERROR                                  = -20,
    RECOVER_FAILED                              = -21,

    /* Operation not available. */
    NOT_AVAILABLE_BECAUSE_PROTECTION            = -90,
    NOT_AVAILABLE_BECAUSE_MPU_CONFIG            = -91,
    
    /* JlinkARM DLL errors. */
    JLINKARM_DLL_NOT_FOUND                      = -100,
    JLINKARM_DLL_COULD_NOT_BE_OPENED            = -101,
    JLINKARM_DLL_ERROR                          = -102,
    JLINKARM_DLL_TOO_OLD                        = -103,

    /* nrfjprog sub DLL errors. */
    NRFJPROG_SUB_DLL_NOT_FOUND                  = -150,
    NRFJPROG_SUB_DLL_COULD_NOT_BE_OPENED        = -151,
    NRFJPROG_SUB_DLL_COULD_NOT_LOAD_FUNCTIONS   = -152,

    /* High Level DLL */
    VERIFY_ERROR                                = -160,
    RAM_IS_OFF_ERROR                            = -161,
        
    /* Not implemented. */
    NOT_IMPLEMENTED_ERROR                       = -255,

} nrfjprogdll_err_t;

/* Expected log function prototype for logging operations. */
typedef void msg_callback(const char * msg_str);

#if defined(__cplusplus)
}
#endif

#endif /* DLL_COMMON_DEFINITIONS_H */
