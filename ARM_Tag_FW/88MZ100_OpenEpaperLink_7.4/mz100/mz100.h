#pragma once

#include <stdint.h>

#include "mz100_interrupt.h"

#ifdef __cplusplus
 extern "C" {
#endif

/** \brief Global Memory Map Block Definition
*/

#define CODE_AREA_BASE        ((uint32_t)0x00000000)            /*!< Code area base address */
#define SRAM_AREA_BASE        ((uint32_t)0x20000000)            /*!< SRAM area base address */
#define PERIPHERAL_BASE       ((uint32_t)0x40000000)            /*!< Peripheral area base address */
#define EXT_RAM_BASE          ((uint32_t)0x60000000)            /*!< EXternal RAM base address */
#define EXT_DEV_BASE          ((uint32_t)0xA0000000)            /*!< External Device base address */
#define PRI_PERI_INT_BASE     ((uint32_t)0xE0000000)            /*!< Private Peripheral - Internal base address */
#define PRI_PERI_EXT_BASE     ((uint32_t)0xE0040000)            /*!< Private Peripheral - External base address */
#define VENDOR_SPEC_BASE      ((uint32_t)0xE0100000)            /*!< Vendor Specific area base address */

#define PERI_AHB1_BASE        ((uint32_t)0x45000000)            /*!< AHB1 Peripheral base address */
#define PERI_APB0_BASE        ((uint32_t)0x48000000)            /*!< APB0 Peripheral base address */
#define PERI_APB1_BASE        ((uint32_t)0x4A000000)            /*!< APB1 Peripheral base address */

/** \brief Peripheral memory map */

#define DMA_BASE              (PERIPHERAL_BASE + 0x4000000)     /*!< DMA controller base address */
#define AES_BASE              (PERI_AHB1_BASE + 0x0000)         /*!< AES module base address */
#define CRC_BASE              (PERI_AHB1_BASE + 0x1000000)      /*!< CRC module base address */
#define CAU_APB0_BASE         (PERI_APB0_BASE + 0x00000)        /*!< CAU module base address */
#define UART1_BASE            (PERI_APB0_BASE + 0x10000)        /*!< UART1 module base address */
#define UART2_BASE            (PERI_APB0_BASE + 0x20000)        /*!< UART2 module base address */
#define WDT_BASE              (PERI_APB0_BASE + 0x30000)        /*!< WDT module base address */
#define RTC_BASE              (PERI_APB0_BASE + 0x40000)        /*!< RTC module base address */
#define I2C1_BASE             (PERI_APB0_BASE + 0x50000)        /*!< I2C1 module base address */
#define I2C2_BASE             (PERI_APB0_BASE + 0x60000)        /*!< I2C2 module base address */
#define PINMUX_BASE           (PERI_APB0_BASE + 0x70000)        /*!< PINMUX module base address */
#define KEYSCAN_BASE          (PERI_APB0_BASE + 0x80000)        /*!< KEYSCAN module base address */
#define IR_BASE               (PERI_APB0_BASE + 0x90000)        /*!< IR module base address */
#define RC32M_BASE            (CAU_APB0_BASE + 0x00100)         /*!< RC32M oscillator module base address */
#define X32M_BASE             (CAU_APB0_BASE + 0x00200)         /*!< X32M oscillator module base address */
#define SYS_CONTROL_BASE      (PERI_APB1_BASE + 0x00000)        /*!< System Control module base address */
#define GPT1_BASE             (PERI_APB1_BASE + 0x10000)        /*!< GPT1 module base address */
#define GPT2_BASE             (PERI_APB1_BASE + 0x20000)        /*!< GPT2 module base address */
#define CAU_APB1_BASE         (PERI_APB1_BASE + 0x30000)        /*!< CAU_APB1  module base address */
#define GPIO_BASE             (PERI_APB1_BASE + 0x40000)        /*!< GPIO module base address */
#define SSP1_BASE             (PERI_APB1_BASE + 0x50000)        /*!< SSP1 module base address */
#define SSP2_BASE             (PERI_APB1_BASE + 0x60000)        /*!< SSP2 module base address */
#define QSPI_BASE             (PERI_APB1_BASE + 0x70000)        /*!< QSPI module base address */
#define PMU_BASE              (PERI_APB1_BASE + 0x80000)        /*!< PMU module base address */
#define TRACKBALL_BASE        (PERI_APB1_BASE + 0x90000)        /*!< TRACKBALL module base address */
#define THREEDG_BASE          (PERI_APB1_BASE + 0xA0000)        /*!< 3DG module base address */
#define ADC_BASE              (CAU_APB1_BASE + 0x00000)         /*!< ADC module base address */
#define DAC_BASE              (CAU_APB1_BASE + 0x00100)         /*!< DAC module base address */
#define ACOMP_BASE            (CAU_APB1_BASE + 0x00200)         /*!< ACOMP module base address */
#define DWT_BASE              (PRI_PERI_INT_BASE + 0x1000)      /*!< DWT module base address */
#define FPB_BASE              (PRI_PERI_INT_BASE + 0x2000)      /*!< FPB module base address */
#define INT_SYS_CTRL_BASE     (PRI_PERI_INT_BASE + 0xE000)      /*!< Internal SYS Control module base address */
#define TPIU_BASE             (PRI_PERI_EXT_BASE + 0x0000)      /*!< TPIU module base address */
#define ETM_BASE              (PRI_PERI_EXT_BASE + 0x1000)      /*!< ETM module base address */

/*@} end of group Global_Memory_Map */

/** \brief Configuration of the Cortex-M3 Processor and Core Peripherals
*/
#define __MPU_PRESENT             1         /*!< MPU present or not                               */
#define __NVIC_PRIO_BITS          4         /*!< Number of Bits used for Priority Levels          */
#define __Vendor_SysTickConfig    0         /*!< Set to 1 if different SysTick Config is used     */

/** \brief Processor and Core Peripheral definition is defined in core_cm3.h
 */
#include "core_cm3.h"                       /*!< Cortex-M3 processor and core peripherals           */

/** \defgroup Peripheral_Section Debivice Peripheral Struct Definition
   Contain below sections:

     - Power Management Unit
     - Direct Memory Access
     - General Purpose I/O
     - Watchdog Timer
     - Real Time Clock
     - AES
     - CRC
     - General Purpose Timer
     - Analog Digital Converter
     - Analog Comparactor
     - Digital Analog Converter
     - UART
     - I2C
     - SSP
     - TRACKBALL
     - 3D GLASS
     - KEYSCAN
     - IR

   @{
 */
#ifndef _MZ100_TOP_H
#include "aes_reg.h"
#include "adc_reg.h"
#include "qspi_reg.h"
#include "pinmux_reg.h"
#include "gpt_reg.h"
#include "pmu_reg.h"
#include "rc32m_reg.h"
#include "rtc_reg.h"
#include "sysctrl_reg.h"
#include "uart_reg.h"
#include "wdt_reg.h"
#include "xtal32m_reg.h"
#include "cauclk_reg.h"
#include "ssp_reg.h"
#endif
#include "gpio_reg.h"
/*@} end of group Peripheral_Section */

/** \defgroup Peripheral_Register_Section Debivice Peripheral Register
   Peripheral register implemenation in the device.

   @{
 */
#ifndef _MZ100_TOP_H
#define RTC           ((rtc_reg_t *) RTC_BASE)                /*!< RTC Register */
#define PINMUX        ((pinmux_reg_t *) PINMUX_BASE)          /*!< PINMUX Register */
#define RC32M         ((rc32m_reg_t *) RC32M_BASE)            /*!< RC32M Register */
#define X32M          ((xtal32m_reg_t *) X32M_BASE)           /*!< X32M Register */
#endif
#define DMA          ((dma_reg_t *) DMA_BASE)                 /*!< DMA Register */
#define AES           ((aes_reg_t *) AES_BASE)                /*!< AES Register */
#define CRC           ((crc_reg_t *) CRC_BASE)                /*!< CRC Register */
#define UART1         ((uart_reg_t *) UART1_BASE)             /*!< UART1 Register */
#define UART2         ((uart_reg_t *) UART2_BASE)             /*!< UART2 Register */
#define WDT           ((wdt_reg_t *) WDT_BASE)                /*!< WDT Register */
#define I2C1          ((i2c_reg_t *) I2C1_BASE)               /*!< I2C1 Register */
#define I2C2          ((i2c_reg_t *) I2C2_BASE)               /*!< I2C2 Register */
#define KEYSCAN       ((keyscan_reg_t *) KEYSCAN_BASE)        /*!< KEYSCAN Register */
#define IR            ((ir_reg_t *) IR_BASE)                  /*!< IR Register */
#define SYS_CTRL      ((sysctrl_reg_t *) SYS_CONTROL_BASE)    /*!< SYS_CTRL Register */
#define GPT1          ((gpt_reg_t *) GPT1_BASE)               /*!< GPT1 Register */
#define GPT2          ((gpt_reg_t *) GPT2_BASE)               /*!< GPT2 Register */
#define ADC           ((adc_reg_t *) ADC_BASE)                /*!< ADC Register */
#define DAC           ((dac_reg_t *) DAC_BASE)                /*!< DAC Register */
#define ACOMP         ((acomp_reg_t *) ACOMP_BASE)            /*!< ACOMP Register */
#define GPIO          ((gpio_reg_t *) GPIO_BASE)              /*!< GPIO Register */
#define SSP1          ((ssp_reg_t *) SSP1_BASE)               /*!< SSP1 Register */
#define SSP2          ((ssp_reg_t *) SSP2_BASE)               /*!< SSP2 Register */
#define QSPI          ((qspi_reg_t *) QSPI_BASE)              /*!< QSPI Register */
#define PMU           ((pmu_reg_t *) PMU_BASE)                /*!< PMU Register */
#define TRACKBALL     ((trackball_reg_t *) TRACKBALL_BASE)    /*!< TRACKBALL Register */
#define THREEDG       ((threedg_reg_t *) THREEDG_BASE)            /*!< 3DG Register */

#ifdef __cplusplus
}
#endif
