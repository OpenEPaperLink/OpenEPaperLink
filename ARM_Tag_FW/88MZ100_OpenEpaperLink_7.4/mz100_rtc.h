#pragma once

#include "mz100.h"
#include "mz100_driver.h"

/** @addtogroup  88MZ100_Periph_Driver
 *  @{
 */

/** @addtogroup  RTC
 *  @{
 */

/** @defgroup RTC_Public_Types RTC_Public_Types
 *  @brief RTC configuration structure type definition
 *  @{
 */

/**
 *  @brief RTC counter value register update mode type definition
 */
typedef enum
{
  RTC_CNT_VAL_UPDATE_OFF       = 0,   /*!< Counter value register update mode: off */
  RTC_CNT_VAL_UPDATE_AUTO      = 2,   /*!< Counter value register update mode: Auto  */
}RTC_CntValUpdateMode_Type;

/**
 *  @brief RTC config struct type definition
 */
typedef struct
{
  RTC_CntValUpdateMode_Type CntValUpdateMode;     /*!< Counter value register update mode:
                                                        RTC_CNT_VAL_UPDATE_OFF (0):  Off
                                                        RTC_CNT_VAL_UPDATE_AUTO (2): Auto   */

  uint32_t clockDivider;                          /*!< Clock divider value(not available in this version, default 0).*/

  uint32_t uppVal;                                /*!< Counter overflow value */
}RTC_Config_Type;

/**
 *  @brief RTC channel number type definition
 */
typedef enum
{
  RTC_CH_0 = 0,            /*!< RTC channel No.: 0 */
  RTC_CH_1 = 1,            /*!< RTC channel No.: 1 */
}RTC_ChannelNumber_Type;

/**
 *  @brief RTC interrupt type definition
 */
#ifndef USE_BASICDRV_IN_ROM
typedef enum
{
  RTC_INT_CH0_CUP = 0,                    /*!< RTC channel 0 cup int flag */
  RTC_INT_CH1_CUP = 1,                    /*!< RTC channel 1 cup int flag */
  RTC_INT_CNT_UPP,                   /*!< RTC counter upper overflow flag */

  RTC_INT_ALL,                             /*!< All RTC interrupts flag */
}RTC_INT_Type;
#else // USE_BASICDRV_IN_ROM
typedef enum
{
  RTC_INT_CNT_UPP = 0,                 /*!< RTC counter upper overflow flag */
  RTC_INT_CH0_CUP,                  /*!< RTC channel 0 cup int flag */
  RTC_INT_CH1_CUP,                  /*!< RTC channel 1 cup int flag */

  RTC_INT_ALL,                             /*!< All RTC interrupts flag */
}RTC_INT_Type;
#endif // USE_BASICDRV_IN_ROM

/*@} end of group RTC_Public_Types definitions */

/** @defgroup RTC_Public_Constants
 *  @{
 */

//#define RTC_INT_CNT_UPP   0

/** @defgroup GPT_INT_MASK
 *  @{
 */
#define RTC_INT_CH0_CUP_MSK              0x00000001
#define RTC_INT_CH1_CUP_MSK              0x00000002
#define RTC_INT_CH_CUP_ALL_MSK           0x00000003
#define RTC_INT_CNT_UPP_MSK              0x00010000
#define RTC_INT_ALL_MSK                  0x00010003
/*@} end of group GPT_INT_MASK */

/*@} end of group RTC_Public_Constants */

/** @defgroup RTC_Public_Macro
 *  @{
 */

/**
 *  @brief Total channel number
 */
#define RTC_CHANNEL_NUMBER        2

/**
 *  @brief Total interrupt number
 */
#define RTC_INT_NUMBER            3

/**
 *  @brief RTC interrupt type check
 */
#define IS_RTC_INT_TYPE(INT_TYPE)    ((INT_TYPE) <= RTC_INT_NUMBER)

/**
 *  @brief RTC channel number check
 */
#define IS_RTC_CHANNEL_ID(CHID)      ((CHID) < RTC_CHANNEL_NUMBER)

/*@} end of group RTC_Public_Macro */

/** @defgroup RTC_Public_FunctionDeclaration
 *  @brief RTC functions statement
 *  @{
 */
void RTC_Init(RTC_Config_Type* rtcConfig);

Status RTC_CounterReset(void);
void RTC_Start(void);
void RTC_Stop(void);

void RTC_SetCounterUppVal( uint32_t uppVal);
uint32_t RTC_GetCounterVal(void);
void RTC_SetChannelCupVal(RTC_ChannelNumber_Type channelID, uint32_t cupVal);

void RTC_IntMask(RTC_INT_Type intType, IntMask_Type intMsk);
FlagStatus RTC_GetStatus(void);
void RTC_IntClr(RTC_INT_Type intType);

void RTC_IRQHandler(void);
