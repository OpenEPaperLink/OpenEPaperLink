#pragma once

#include "mz100.h"
/** @addtogroup  88MZ100_Periph_Driver
 *  @{
 */

/** @addtogroup  DRIVER_COMMON
 *  @{
 */

/** @defgroup DRIVER_Public_Types DRIVER_Public_Types
 *  @brief DRIVER type definition
 *  @{
 */

/**
 * @brief Functional State Definition
 */
typedef enum
{
  DISABLE  = 0,
  ENABLE   = 1
}FunctionalState;

/**
 * @brief status type definition
 */
typedef enum
{
  ERROR = 0,
  SUCCESS = !ERROR
}Status;

/**
 * @brief Flag status Type definition
 */
typedef enum
{
  RESET = 0,
  SET = !RESET
}FlagStatus, IntStatus;

/**
 * @brief Logical status Type definition
 */
typedef enum
{
  LOGIC_LO = 0,
  LOGIC_HI = !LOGIC_LO
}LogicalStatus;

/**
 * @brief Active status Type definition
 */
typedef enum
{
  DEACTIVE = 0,
  ACTIVE = !DEACTIVE
}ActiveStatus;

/**
 * @brief interrupt mask Type definition
 */
typedef enum
{
  UNMASK = 0,
  MASK = 1
}IntMask_Type;

/**
 * @brief Null Type definition
 */
#ifndef NULL
#define NULL   0
#endif

/**
 *  @brief Interrupt callback function type
 */
typedef void (intCallback_Type)(void);

/**
 * @brief MZ100 peripheral type definitions
 */
typedef enum
{
  INT_UART1,
  INT_UART2,
  INT_RTC,
  INT_I2C1,
  INT_I2C2,
  INT_SSP1,
  INT_SSP2,
  INT_GPT1,
  INT_GPT2,
  INT_CRC,
  INT_AES,
  INT_ADC,
  INT_DAC,
  INT_ACOMP,
  INT_GPIO,
  INT_DMA_CH0,
  INT_DMA_CH1,
  INT_DMA_CH2,
  INT_DMA_CH3,
  INT_DMA_CH4,
  INT_DMA_CH5,
  INT_QSPI,
  INT_WDT,
  INT_RC32M,
  INT_XTAL32M,
  INT_MPU,
  INT_THREEDG,
  INT_KEYSCAN,
  INT_IR,
  INT_TRACKBALL,
  LAST,
}INT_Peripher_Type;

/*@} end of group DRIVER_Public_Types */

/** @defgroup DRIVER_Public_Constants
 *  @{
 */
/** @defgroup DRIVER_FUNCTIONALSTATE
 *  @{
 */
#define PARAM_FUNCTIONALSTATE(State) (((State) == DISABLE) || ((State) == ENABLE))
/*@} end of group DRIVER_FUNCTIONALSTATE */

/** @defgroup DRIVER_INT_PERIPH
 *  @{
 */
#define IS_INT_PERIPH(INT_PERIPH)       ((INT_PERIPH) < LAST)
/*@} end of group DRIVER_INT_PERIPH */

/** @defgroup DRIVER_INT_MASK
 *  @{
 */
#define IS_INTMASK(INTMASK)          (((INTMASK) == MASK) || ((INTMASK) == UNMASK))
/*@} end of group DRIVER_INT_MASK */

/*@} end of group DRIVER_Public_Constants */

/** @defgroup DRIVER_Public_Macro
 *  @{
 */

#ifdef  DEBUG
/*************************************************************************************//**
* @brief        The CHECK_PARAM macro is used for function's parameters check.
*                 It is used only if the library is compiled in DEBUG mode.
* @param[in]    expr: - If expr is false, it calls check_failed() function
*               which reports the name of the source file and the source
*               line number of the call that failed.
*                     - If expr is true, it returns no value.
* @return  none
*****************************************************************************************/
void check_failed(uint8_t *file, uint32_t line);
#define CHECK_PARAM(expr) ((expr) ? (void)0 : check_failed((uint8_t *)__FILE__, __LINE__))
#else
#define CHECK_PARAM(expr) ((void)0)
#endif /* DEBUG */

/*@} end of group DRIVER_Public_Macro */

/** @defgroup DRIVER_Public_FunctionDeclaration
 *  @brief DRIVER functions declaration
 *  @{
 */
extern  intCallback_Type ** intCbfArra[];
#ifndef USE_BASICDRV_IN_ROM
void install_int_callback(INT_Peripher_Type intPeriph, uint32_t intType, intCallback_Type * cbFun);
#else
#include "mz100_driver_isr.h"
#endif // USE_BASICDRV_IN_ROM
