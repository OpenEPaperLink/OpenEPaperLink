#pragma once

#include "mz100_driver.h"

/** @addtogroup  88MZ100_Periph_Driver
 *  @{
 */

/** @addtogroup  PMU
 *  @{
 */

/** @defgroup PMU_Public_Types PMU_Public_Types
 *  @{
 */

/** 
 *  @brief PMU enable or disable definition 
 */

/** 
 *  @brief sleep mode definition 
 */
typedef enum
{
  PMU_PM1 = 0,                            /*!< Power Mode 1 */
  PMU_PM2,                                /*!< Power Mode 2 */
  PMU_PM3,                                /*!< Power Mode 3 */
  PMU_PM4,                                /*!< Power Mode 4 */
}PMU_SleepMode_Type;

/** 
 *  @brief wakeup source list 
 */
typedef enum
{
  PMU_GPIO1_INT = 0,                       /*!< WKUP Int PIN1 */
  PMU_GPIO2_INT,                           /*!< WKUP Int PIN2 */
  PMU_GPIO3_INT,                           /*!< WKUP Int PIN3 */
  PMU_GPIO4_INT,                           /*!< WKUP Int PIN4 */
  PMU_GPIO5_INT,                           /*!< WKUP Int PIN5 */
  PMU_GPIO6_INT,                           /*!< WKUP Int PIN6 */
  PMU_GPIO7_INT,                           /*!< WKUP Int PIN7 */
  PMU_GPIO26_INT,                          /*!< WKUP Int PIN26 */
  PMU_GPIO27_INT,                          /*!< WKUP Int PIN27 */
  PMU_GPIO28_INT,                          /*!< WKUP Int PIN28 */
  PMU_GPIO29_INT,                          /*!< WKUP Int PIN29 */
  PMU_GPIO30_INT,                          /*!< WKUP Int PIN30 */
  PMU_GPIO31_INT,                          /*!< WKUP Int PIN31 */
}PMU_WakeupPinSrc_Type;

/** 
 *  @brief wakeup trigger mode list 
 */
typedef enum
{
  PMU_WAKEUP_DISABLE = 0,                 /*!< Disable wakeup pin */
  PMU_WAKEUP_EDGE_RISING,                 /*!< Wakeup at the rising edge */
  PMU_WAKEUP_EDGE_FALLING,                /*!< Wakeup at the falling edge */
}PMU_WakeupTrigMode_Type;

/** 
 *  @brief last reset cause list 
 */
typedef enum
{
  PMU_CM3_SYSRESETREQ = 0x01,             /*!< CM3 SYSRESETREQ reset */
  PMU_CM3_LOCKUP      = 0x02,             /*!< CM3 lockup reset */
  PMU_WDTOUT          = 0x04,             /*!< WDT-out reset */
  PMU_RESETSRC_ALL    = 0x07,             /*!< All reset source */
}PMU_LastResetCause_Type;

/** @defgroup PMU_Public_Constants
 *  @{
 */

/** @defgroup PMU_Command
 *  @{
 */

/*@} end of group PMU_Command */

/** @defgroup PMU_Interrupt_Flag
 *  @{
 */

/*@} end of group PMU_Interrupt_Flag */

/*@} end of group PMUPublic_Constants */

/** @defgroup PMU_Public_Macro
 *  @{
 */

/*@} end of group PMU_Public_Macro */

/** @defgroup PMU_Public_FunctionDeclaration
 *  @brief PMU functions statement
 *  @{
 */
void PMU_SetSleepMode(PMU_SleepMode_Type pmuMode);

void PMU_EnableCAUInPM2(void);
void PMU_DisableCAUInPM2(void);

void PMU_ConfigWakeupPin(PMU_WakeupPinSrc_Type wakeupPin, PMU_WakeupTrigMode_Type trigMode);
void PMU_ClearWakeupExtpin(PMU_WakeupPinSrc_Type wakeupPin);
void PMU_ClearWakeupKeyscan(void);

uint32_t PMU_GetLastResetCause(void);
