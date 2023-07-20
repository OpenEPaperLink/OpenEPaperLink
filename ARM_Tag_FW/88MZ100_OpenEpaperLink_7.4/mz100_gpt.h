#pragma once

#include "mz100.h"
#include "mz100_driver.h"

/** @addtogroup  88MZ100_Periph_Driver
 *  @{
 */

/** @addtogroup  GPT
 *  @{
 */

/** @defgroup GPT_Public_Types GPT_Public_Types
 *  @brief GPT configuration structure type definition
 *  @{
 */

/**
 *  @brief GPT No. type definition
 */
typedef enum
{
  GPT1_ID = 0,                      /*!< GPT1 module define */
  GPT2_ID,                        /*!< GPT2 module define */
}GPT_ID_Type;

/**
 *  @brief GPT clock source type definition
 */
typedef enum
{
  GPT_CLOCK_0,                  /*!< Clock 0 : clock from PMU */
  GPT_CLOCK_1,                  /*!< Clock 1 : external Pin input clock */
}GPT_ClockSrc_Type;

/**
 *  @brief Counter value register update mode type definition
 */
typedef enum
{
  GPT_CNT_VAL_UPDATE_OFF    = 0,     /*!< Counter value register update mode: off */
  GPT_CNT_VAL_UPDATE_AUTO   = 1,     /*!< Counter value register update mode: auto-update */
  GPT_CNT_VAL_UPDATE_FAST   = 2,     /*!< Counter value register update mode: auto-update fast */

}GPT_CntUpdate_Type;

/**
 *  @brief GPT counter mode type definition
 */
typedef enum
{
  GPT_COUNTER_MODE_UP = 0,       /*!< Counter Up mode */
  GPT_COUNTER_MODE_DOWN,         /*!< Counter Down mode */
  GPT_COUNTER_MODE_UPDOWN,       /*!< Counter Up and Down mode */
}GPT_CounterMode_Type;

/**
 *  @brief GPT input capture type definition
 */
typedef enum
{
  GPT_IC_RISING_EDGE = 0,       /*!< Input capture rising edge selected */
  GPT_IC_FALLING_EDGE,          /*!< Input capture falling edge selected */
}GPT_ICEdge_Type;

/**
 *  @brief GPT channel function type definition
 */
typedef enum
{
  GPT_CH_FUNC_OUTPUT = 0,      /*!< GPT channel function: output compare */
  GPT_CH_FUNC_INPUT,           /*!< GPT channel function: input capture */
}GPT_ChannelFunc_Type;

/**
 *  @brief GPT channel action type definition
 */
typedef enum
{
  GPT_CH_ACT_NO         = 0,      /*!< GPT channel action: no action */
  GPT_CH_ACT_CLEAR      = 1,      /*!< GPT channel action: clear 0 */
  GPT_CH_ACT_SET        = 3,      /*!< GPT channel action: set 1 */
  GPT_CH_ACT_TOGGLE     = 5,      /*!< GPT channel action: toggle */
}GPT_ChannelAction_Type;

/**
 *  @brief GPT channel number type definition
 */
typedef enum
{
  GPT_CH_0 = 0,        /*!< GPT channel No.: 0 */
  GPT_CH_1,            /*!< GPT channel No.: 1 */
  GPT_CH_2,            /*!< GPT channel No.: 2 */
  GPT_CH_3,            /*!< GPT channel No.: 3 */
  GPT_CH_4,            /*!< GPT channel No.: 4 */
}GPT_ChannelNumber_Type;

/**
 *  @brief GPT interrupt type definition
 */
typedef enum
{
  GPT_INT_CH0_CUP,                    /*!< GPT channel 0 cup int flag */
  GPT_INT_CH1_CUP,                    /*!< GPT channel 1 cup int flag */
  GPT_INT_CH2_CUP,                    /*!< GPT channel 2 cup int flag */
  GPT_INT_CH3_CUP,                    /*!< GPT channel 3 cup int flag */
  GPT_INT_CH4_CUP,                    /*!< GPT channel 4 cup int flag */

  GPT_INT_CH0_CDN,                    /*!< GPT channel 0 cdn int flag */
  GPT_INT_CH1_CDN,                    /*!< GPT channel 1 cdn int flag */
  GPT_INT_CH2_CDN,                    /*!< GPT channel 2 cdn int flag */
  GPT_INT_CH3_CDN,                    /*!< GPT channel 3 cdn int flag */
  GPT_INT_CH4_CDN,                    /*!< GPT channel 4 cdn int flag */

  GPT_INT_CNT_UPP,                   /*!< GPT counter upper overflow flag */
  GPT_INT_CNT_LOW,                   /*!< GPT counter low overflow flag */

  GPT_INT_DMA0_OF,                       /*!< GPT DMA channel 0 overflow flag */
  GPT_INT_DMA1_OF,                       /*!< GPT DMA channel 1 overflow flag */

  GPT_INT_ALL,                             /*!< All GPT interrupts flag */
}GPT_INT_Type;

/**
 *  @brief GPT trig repetition type definition
 */
typedef enum
{
  GPT_TRIG_REP_1,             /*!< GPT input trig repetition: 1 */
  GPT_TRIG_REP_2,             /*!< GPT input trig repetition: 2 */
  GPT_TRIG_REP_3,             /*!< GPT input trig repetition: 3 */
  GPT_TRIG_REP_4,             /*!< GPT input trig repetition: 4 */
  GPT_TRIG_REP_5,             /*!< GPT input trig repetition: 5 */
  GPT_TRIG_REP_6,             /*!< GPT input trig repetition: 6 */
  GPT_TRIG_REP_7,             /*!< GPT input trig repetition: 7 */
  GPT_TRIG_REP_8,             /*!< GPT input trig repetition: 8 */
}GPT_TrigRep_Type;

/**
 *  @brief GPT input filter type definition
 */
typedef enum
{
  GPT_INPUT_FILTER_1,             /*!< GPT input filter : 1 */
  GPT_INPUT_FILTER_2,             /*!< GPT input filter : 2 */
  GPT_INPUT_FILTER_3,             /*!< GPT input filter : 3 */
  GPT_INPUT_FILTER_4,             /*!< GPT input filter : 4 */
  GPT_INPUT_FILTER_5,             /*!< GPT input filter : 5 */
  GPT_INPUT_FILTER_6,             /*!< GPT input filter : 6 */
  GPT_INPUT_FILTER_7,             /*!< GPT input filter : 7 */
}GPT_InputFilter_Type;

/**
 *  @brief GPT DMA channel type definition
 */
typedef enum
{
  GPT_DMA0,             /*!< GPT DMA channel : 0 */
  GPT_DMA1,             /*!< GPT DMA channel : 1 */
}GPT_DMAChannel_Type;

/**
 *  @brief GPT config struct type definition
 */
typedef struct
{
  GPT_ClockSrc_Type clockSrc;        /*!< GPT counter clock source selection
                                          GPT_CLOCK_INT (0): clock from PMU
                                          GPT_CLOCK_EXT (1): External pin input clock */

  GPT_CounterMode_Type counterMode;  /*!< Counter mode selection
                                          GPT_COUNTER_MODE_UP (0):     Counter Up mode
                                          GPT_COUNTER_MODE_DOWN (1):   Counter Down mode
                                          GPT_COUNTER_MODE_UPDOWN (2): Counter Up and Down mode */

  GPT_CntUpdate_Type cntUpdate;     /*!< GPT counter value update mode selection
                                         GPT_CNT_VAL_UPDATE_OFF (0): update off
                                         GPT_CNT_VAL_UPDATE_AUTO (3): auto-update normal */

  uint32_t clockDivider;             /*!< Clock divider value(range: 0~15).
                                          The divided clock is calculated by:
                                          CLK_div = CLK / (2^clockDivider)*/

  uint32_t clockPrescaler;           /*!< Clock prescaler value(range: 0~127).
                                          The divided clock is calculated by :
                                          CLK_ps = CLK_div / (2 x clockPrescaler + 1)*/

  uint32_t uppVal;                   /*!< Counter overflow value */

}GPT_Config_Type;

/**
 *  @brief GPT input config type definition
 */
typedef struct
{
  GPT_ICEdge_Type edgeSel;           /*!< Input capture edge select
                                          GPT_IC_RISING_EDGE (0):  Rising edge
                                          GPT_IC_FALLING_EDGE (1): Falling edge */

  GPT_TrigRep_Type trigRep;          /*!< Input trigger repetition.
                                          Capture the counter after sampling how many valid edges of the input trigger.
                                          GPT_TRIG_REP_1 (0): 1
                                          GPT_TRIG_REP_2 (1): 2
                                          GPT_TRIG_REP_3 (2): 3
                                          GPT_TRIG_REP_4 (3): 4
                                          GPT_TRIG_REP_5 (4): 5
                                          GPT_TRIG_REP_6 (5): 6
                                          GPT_TRIG_REP_7 (6): 7
                                          GPT_TRIG_REP_8 (7): 8 */

  uint32_t trigClkDivider;           /*!< Input trigger sampling clock divider (range: 0~7).
                                          It can be calculated by:
                                          SCLK_DIV = SCLK / (2 ^ trigClkDivider) */

  GPT_InputFilter_Type trigFilter;   /*!< Input trigger filtering.
                                          It indicates how many consecutive cycles the input trigger should be sampled
                                          before it is considered as a valid edge.
                                          GPT_INPUT_FILTER_1 (0): 1
                                          GPT_INPUT_FILTER_2 (1): 2
                                          GPT_INPUT_FILTER_3 (2): 3
                                          GPT_INPUT_FILTER_4 (3): 4
                                          GPT_INPUT_FILTER_5 (4): 5
                                          GPT_INPUT_FILTER_6 (5): 6
                                          GPT_INPUT_FILTER_7 (6): 7 */

}GPT_InputConfig_Type;

/**
 *  @brief GPT channel config type definition
 */
typedef struct
{
  GPT_ChannelAction_Type lowAct;       /*!< Channel low action selection.
                                            When counter reaches zero during Count down or Count-up-and-down mode, do the
                                            following action:
                                            GPT_CH_ACT_NO (0):     No action
                                            GPT_CH_ACT_CLEAR (1):  Clear 0
                                            GPT_CH_ACT_SET (3):    Set 1
                                            GPT_CH_ACT_TOGGLE (5): Toggle */

  GPT_ChannelAction_Type uppAct;       /*!< Channel upp action selection.
                                            When counter reaches the upper during Count up or Count-up-and-down mode, do
                                            the following action:
                                            GPT_CH_ACT_NO (0):     No action
                                            GPT_CH_ACT_CLEAR (1):  Clear 0
                                            GPT_CH_ACT_SET (3):    Set 1
                                            GPT_CH_ACT_TOGGLE (5): Toggle */

  GPT_ChannelAction_Type cdnAct;       /*!< Channel cdn action selection.
                                            When counter reaches the Channel counting down match value during Count down or
                                            Count-up-and-down mode, do the following action:
                                            GPT_CH_ACT_NO (0):     No action
                                            GPT_CH_ACT_CLEAR (1):  Clear 0
                                            GPT_CH_ACT_SET (3):    Set 1
                                            GPT_CH_ACT_TOGGLE (5): Toggle */

  GPT_ChannelAction_Type cupAct;       /*!< Channel cup action selection.
                                            When counter reaches the Channel counting up match value during Count up or
                                            Count-up-and-down mode, do the following action:
                                            GPT_CH_ACT_NO (0):     No action
                                            GPT_CH_ACT_CLEAR (1):  Clear 0
                                            GPT_CH_ACT_SET (3):    Set 1
                                            GPT_CH_ACT_TOGGLE (5): Toggle */

  uint32_t cupVal;                     /*!< Channel counting up match value */

  uint32_t cdnVal;                     /*!< Channel counting down match value */

}GPT_ChannelOutputConfig_Type;

/*@} end of group GPT_Public_Types definitions */

/** @defgroup GPT_Public_Constants
 *  @{
 */

/** @defgroup GPT_INT_MASK
 *  @{
 */
#define GPT_INT_CH0_CUP_MSK              0x00000001
#define GPT_INT_CH1_CUP_MSK              0x00000002
#define GPT_INT_CH2_CUP_MSK              0x00000004
#define GPT_INT_CH3_CUP_MSK              0x00000008
#define GPT_INT_CH4_CUP_MSK              0x00000010
#define GPT_INT_CH_CUP_ALL_MSK           0x0000001F
#define GPT_INT_CH0_CDN_MSK              0x00000100
#define GPT_INT_CH1_CDN_MSK              0x00000200
#define GPT_INT_CH2_CDN_MSK              0x00000400
#define GPT_INT_CH3_CDN_MSK              0x00000800
#define GPT_INT_CH4_CDN_MSK              0x00001000
#define GPT_INT_CH_CDN_ALL_MSK           0x00001F00
#define GPT_INT_CNT_UPP_MSK              0x00010000
#define GPT_INT_CNT_LOW_MSK              0x00020000
#define GPT_INT_DMA0_OF_MSK              0x01000000
#define GPT_INT_DMA1_OF_MSK              0x02000000
#define GPT_INT_ALL_MSK                  0x03031F1F
/*@} end of group GPT_INT_MASK */

/**
 *  @brief Total channel number
 */
#define GPT_CHANNEL_NUMBER        5

/**
 *  @brief Total interrupt number
 */
#define GPT_INT_NUMBER            14

/**
 *  @brief Total channel number
 */
#define GPT_DMA_CHANNEL_NUMBER        2

/**
 *  @brief GPT modules check
 */
#define IS_GPT_PERIPH(PERIPH)        (((PERIPH) == GPT1_ID) || ((PERIPH) == GPT2_ID))

/**
 *  @brief GPT interrupt type check
 */
#define IS_GPT_INT_TYPE(INT_TYPE)    ((INT_TYPE) <= GPT_INT_NUMBER)

/**
 *  @brief GPT channel number check
 */
#define IS_GPT_CHANNEL_ID(CHID)      ((CHID) < GPT_CHANNEL_NUMBER)

/**
 *  @brief GPT DMA channel number check
 */
#define IS_GPT_DMA_CHANNEL_ID(CHID)    ((CHID) < GPT_DMA_CHANNEL_NUMBER)

/*@} end of group GPT_Public_Constants */

/** @defgroup GPT_Public_Macro
 *  @{
 */

/*@} end of group GPT_Public_Macro */

/** @defgroup GPT_Public_FunctionDeclaration
 *  @brief GPT functions statement
 *  @{
 */

void GPT_Init(GPT_ID_Type gptID, GPT_Config_Type* gptConfig);
void GPT_InputConfig(GPT_ID_Type gptID, GPT_InputConfig_Type* gptInputConfig);
void GPT_ChannelFuncSelect(GPT_ID_Type gptID, GPT_ChannelNumber_Type channelID,
                           GPT_ChannelFunc_Type function);
void GPT_ChannelOutputConfig(GPT_ID_Type gptID, GPT_ChannelNumber_Type channelID,
                          GPT_ChannelOutputConfig_Type* gptChOutConfig);
Status GPT_CounterReset(GPT_ID_Type gptID);
void GPT_Start(GPT_ID_Type gptID);
void GPT_Stop(GPT_ID_Type gptID);
#ifndef USE_BASICDRV_IN_ROM
void GPT_RegClear(GPT_ID_Type gptID);
#endif // USE_BASICDRV_IN_ROM

void GPT_SetCounterUppVal(GPT_ID_Type gptID, uint32_t uppVal);
void GPT_SetChannelUpVal(GPT_ID_Type gptID, GPT_ChannelNumber_Type channelID, uint32_t cupVal);
void GPT_SetChannelDownVal(GPT_ID_Type gptID, GPT_ChannelNumber_Type channelID, uint32_t cdnVal);
void GPT_SetChannelUpDownVal(GPT_ID_Type gptID, GPT_ChannelNumber_Type channelID, uint32_t cupVal, uint32_t cdnVal);
uint32_t GPT_GetCaptureVal(GPT_ID_Type gptID, GPT_ChannelNumber_Type channelID);
uint32_t GPT_GetCounterVal(GPT_ID_Type gptID);

void GPT_DMACmd(GPT_ID_Type gptID, GPT_DMAChannel_Type dmaChannel, FunctionalState state);
void GPT_DMAChannelSelect(GPT_ID_Type gptID, GPT_DMAChannel_Type dmaChannel, GPT_ChannelNumber_Type channelID);

void GPT_IntMask(GPT_ID_Type gptID, GPT_INT_Type intType, IntMask_Type intMsk);
IntStatus GPT_GetIntStatus(GPT_ID_Type gptID, GPT_INT_Type intType);
void GPT_IntClr(GPT_ID_Type gptID, GPT_INT_Type intType);

void GPT1_IRQHandler(void);
void GPT2_IRQHandler(void);
