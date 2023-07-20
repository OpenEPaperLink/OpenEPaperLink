#pragma once

#include "mz100_driver.h"

/** @addtogroup  88MZ100_Periph_Driver
 *  @{
 */

/** @addtogroup  Clock
 *  @{
 */

/** @defgroup Clock_Public_Types Clock_Public_Types
 *  @{
 */

/**
 *  @brief system clock source option
 */
typedef enum
{
  CLK_SYS_XTAL64M = 0,                          /*!< XT64M System Clock Select */
  CLK_SYS_RC32M   = 1,                          /*!< RC32M System Clock select */
}CLK_SysSrc_Type;

/**
 *  @brief GP Timer clock source option
 */
typedef enum
{
  CLK_GPT_XTAL32M = 0,                          /*!< XT32M Clock Select */
  CLK_GPT_RC32M   = 1,                          /*!< RC32M Clock select */
  CLK_GPT_XTAL32K = 2,                          /*!< XT32K Clock select */
  CLK_GPT_RC32K   = 3,                          /*!< RC32K Clock select */
}CLK_GPTSrc_Type;

/**
 *  @brief RC32K calibration clock source option
 */
typedef enum
{
  CLK_RC32KCAL_XTAL64M = 0,                          /*!< XT64M Clock Select */
  CLK_RC32KCAL_XTAL32K = 1,                          /*!< XT32K Clock select */
}CLK_RC32KCalSrc_Type;

/**
 *  @brief RTC clock source option
 */
typedef enum
{
  CLK_RTC_XTAL32K = 0,                          /*!< XT32K Clock select */
  CLK_RTC_RC32K   = 1,                          /*!< RC32K Clock select */
}CLK_RTCSrc_Type;

/**
 *  @brief I2S clock source option //may need change the order
 */
typedef enum
{
  CLK_I2S_XTAL32M = 0,                          /*!< XT32M Clock Select */
  CLK_I2S_RC32M   = 1,                          /*!< RC32M Clock select */
  CLK_I2S_XTAL32K = 2,                          /*!< XT32K Clock select */
  CLK_I2S_RC32K   = 3,                          /*!< RC32K Clock select */
}CLK_I2SSrc_Type;

/**
 *  @brief Keyscan clock source option
 */
typedef enum
{
  CLK_KEYSCAN_XTAL32K = 0,                          /*!< XT32K Clock select */
  CLK_KEYSCAN_RC32K   = 1,                          /*!< RC32K Clock select */
}CLK_KeyscanSrc_Type;

/**
 *  @brief Trackball clock source option
 */
typedef enum
{
  CLK_TRACKBALL_XTAL32K = 0,                          /*!< XT32K Clock select */
  CLK_TRACKBALL_RC32K   = 1,                          /*!< RC32K Clock select */
}CLK_TrackballSrc_Type;

/**
 *  @brief SSP clock source option
 */
typedef enum
{
  CLK_SSP_I2S = 0,                               /*!< I2S Clock select */
  CLK_SSP_SYSTEM   = 1,                          /*!< System Clock select */
}CLK_SSPSrc_Type;

/**
 *  @brief 3D Glass clock source option
 */
typedef enum
{
  CLK_3DG_XTAL32K = 0,                          /*!< XT32K Clock select */
  CLK_3DG_RC32K   = 1,                          /*!< RC32K Clock select */
}CLK_3DGSrc_Type;

/**
 *  @brief clock source option
 */
typedef enum
{
  CLK_OUT_CM3     = 0,                          /*!< CM3 Clock Select */
  CLK_OUT_MEM     = 1,                          /*!< Memery Clock Select */
  CLK_OUT_SRC     = 2,                          /*!< Main System Clock Select */
  CLK_OUT_FIXED32M = 3,                         /*!< Fixed 32MHz Clock Select */
  CLK_OUT_XTAL64M = 4,                          /*!< XT64M Clock Select */
  CLK_OUT_RC32M   = 5,                          /*!< RC32M Clock select */
  CLK_OUT_XTAL32K = 6,                          /*!< XT32K Clock select */
  CLK_OUT_RC32K   = 7,                          /*!< RC32K Clock select */
}CLK_OutSrc_Type;

/**
 *  @brief crystal mode option
 */
typedef enum
{
  CLK_OSC_INTERN = 0,                         /*!< Internal oscillator mode */
  CLK_OSC_EXTERN = 1,                         /*!< External oscillator mode */
}CLK_CrystalMode_Type;

/**
 *  @brief rc32m clock interrupt type definition
 */
typedef enum
{
  RC32M_CALDON        = 0,                  /*!< RC32M calibration done interrupt */
  RC32M_CKRDY         = 1,                  /*!< RC32M clock out ready interrupt */
}RC32M_INT_Type;

/**
 *  @brief xtal32m clock interrupt type definition
 */
typedef enum
{
  X32M_DIG_32M_RDY,                         /*!< Digital 32M clock out ready interrupt */
  X32M_ANA_32M_RDY,                         /*!< Analog 32M clock out ready interrupt */
  X32M_DIG_64M_RDY,                         /*!< Digital 64M clock out ready interrupt */
}X32M_INT_Type;

/**
 *  @brief clock module option
 */
typedef enum
{
  CLK_OUT,                                  /*!< OUT Clock */
  CLK_RC32K_CAL,                            /*!< RC32K_CAL Clock */
  CLK_SYSCTRL,                              /*!< SYSCTRL Clock */
  CLK_CAU1,                                 /*!< CAU1 Clock */
  CLK_SSP2,                                 /*!< SSP2 Clock */
  CLK_SSP1,                                 /*!< SSP1 Clock */
  CLK_QSPI,                                /*!< FLASH Clock */
  CLK_GPIO,                                 /*!< GPIO Clock */
  CLK_GPT2,                                 /*!< GPT2 Clock */
  CLK_GPT1,                                 /*!< GPT1 Clock */
  CLK_PINMUX,                               /*!< PINMUX Clock */
  CLK_RTC,                                  /*!< RTC Clock */
  CLK_CAU0,                                 /*!< CAU0 Clock */
  CLK_WDT,                                  /*!< WDT Clock */
  CLK_UART2,                                /*!< UART2 Clock */
  CLK_UART1,                                /*!< UART1 Clock */
  CLK_I2C2,                                 /*!< I2C2 Clock */
  CLK_I2C1,                                 /*!< I2C1 Clock */
  CLK_APB0,                                 /*!< APB0 Clock */
  CLK_AES_CRC,                              /*!< AES/CRC Clock */
  CLK_PHY,                                  /*!< PHY Clock */
  CLK_DMA,                                  /*!< DMA Clock */
  CLK_COMP,                                 /*!< COMP Clock */
  CLK_DAC,                                  /*!< DAC Clock */
  CLK_ADC,                                  /*!< ADC Clock */
  CLK_I2S,                                  /*!< I2S Clock */
  CLK_KEYSCAN,                              /*!< keyscan Clock */
  CLK_TRACKBALL,                            /*!< Trackball Clock */
  CLK_IR,                                   /*!< IR Clock */
  CLK_3DG,                                  /*!< 3D Glass Clock */
  CLK_XTAL32K_CLK,                          /*!< RC32K_CAL Clock */
}CLK_Module_Type;

/**
 *  @brief memery block clock option
 */
typedef enum
{
  CLK_MEM_ROM0,
  CLK_MEM_ROM1,
  CLK_MEM_RAM0,
  CLK_MEM_RAM1,
  CLK_MEM_RAM2,
  CLK_MEM_RAM3,
  CLK_MEM_RAM4,
  CLK_MEM_AON_MEM,
}CLK_MemBlock_Type;

/**
 *  @brief system clock frequency option
 */
typedef enum
{
  CLK_SYS_64M = 64,                         /*!< 64M System Clock Select */
  CLK_SYS_32M = 32,                         /*!< 32M System Clock select */
  CLK_SYS_16M = 16,                         /*!< 16M System Clock select */
  CLK_SYS_8M  = 8,                          /*!< 8M System Clock select */
  CLK_SYS_4M  = 4,                          /*!< 4M System Clock select */
  CLK_SYS_2M  = 2,                          /*!< 2M System Clock select */
  CLK_SYS_1M  = 1,                          /*!< 1M System Clock select */
}CLK_SysFreq_Type;

/**
 *  @brief memory clock frequency option
 */
typedef enum
{
  CLK_MEMFREQ_ESYS = 0,                     /*!< Memory Clock is same with System Clock */
  CLK_MEMFREQ_DSYS = 1,                     /*!< Memory Clock is double with System Clock */
}CLK_MemFreq_Type;

/**
 *  @brief calibration option(manual or Auto)
 */
typedef enum
{
  CLK_MANUAL_CAL  = 0,                      /*!< RC32K/RC32M Manual Calibration option */
  CLK_AUTO_CAL    = 1,                      /*!< RC32K/RC32M Auto Calibration option */
}CLK_RCCal_Type;

/**
 *  @brief RC32M output frequency definition
 */
typedef enum
{
  RC32M_SELFREQ_16M        = 0,                  /*!< RC32M output frequency 16 MHz */
  RC32M_SELFREQ_32M        = 1,                  /*!< RC32M output frequency 32 MHz */
}RC32M_SELFREQ_Type;

/**
 *  @brief gpt id option
 */
typedef enum
{
  CLK_GPT_ID_1   = 0,                       /*!< GPT1 */
  CLK_GPT_ID_2   = 1,                       /*!< GPT2 */
}CLK_GptID_Type;

/**
 *  @brief spi id option
 */
typedef enum
{
  CLK_SSP_ID_1   = 0,                       /*!< SPI1 */
  CLK_SSP_ID_2   = 1,                       /*!< SPI2 */
}CLK_SspID_Type;

/*@} end of group Clock_Public_Types definitions */

/** @defgroup Clock_Public_Constants
 *  @{
 */

/** @defgroup Clock_Command
 *  @{
 */


/*@} end of group Clock_Command */


/** @defgroup Clock_Interrupt_Flag
 *  @{
 */

/*@} end of group Clock_Interrupt_Flag */

/*@} end of group Clock_Public_Constants */

/** @defgroup Clock_Public_Macro
 *  @{
 */

/**
 *  @brief RC32M frequency
 */
#define RC32M_FREQ 32000000
#define XTAL64M_FREQ (MAINXTAL_FREQ*2)

/*@} end of group Clock_Public_Macro */

/** @defgroup Clock_Public_FunctionDeclaration
 *  @brief Clock functions statement
 *  @{
 */

void CLK_Xtal32MEnable(CLK_CrystalMode_Type oscMode);
void CLK_Xtal32MDisable(void);

void CLK_Xtal32KEnable(CLK_CrystalMode_Type oscMode);
void CLK_Xtal32KDisable(void);

void CLK_RC32MEnable(void);
void CLK_RC32MDisable(void);

void CLK_RC32KEnable(void);
void CLK_RC32KDisable(void);

int32_t CLK_RC32MCalibration(CLK_RCCal_Type rcCalAutoManOption, uint32_t extCalCode);
int32_t CLK_RC32KCalibration(CLK_RC32KCalSrc_Type referenceClk, CLK_RCCal_Type rcCalAutoManOption, uint32_t extCalCode);

FlagStatus CLK_GetClkStatus(CLK_OutSrc_Type clockSource);
uint32_t CLK_GetUARTClk(void);

void CLK_SystemClkInit(CLK_SysSrc_Type clockSource, CLK_SysFreq_Type clockSys );
uint32_t CLK_GetSystemClkFreq(void);

FunctionalState CLK_GetModuleClock(CLK_Module_Type clkModule, uint32_t * freq);
FunctionalState CLK_GetModuleStatus(CLK_Module_Type clkModule);

void CLK_OUTClkSrc(CLK_OutSrc_Type clockSourc);
void CLK_GPTInternalClkSrc(CLK_GptID_Type gptIndex, CLK_GPTSrc_Type clockSource);
void CLK_RTCClkSrc(CLK_RTCSrc_Type clockSource);
void CLK_I2SClkSrc(CLK_I2SSrc_Type clockSource);
void CLK_KEYSCANClkSrc(CLK_KeyscanSrc_Type clockSource);
void CLK_TRACKBALLClkSrc(CLK_TrackballSrc_Type clockSource);
void CLK_SSPClkSrc(CLK_SspID_Type sspIndex, CLK_SSPSrc_Type clockSource);
void CLK_3DGClkSrc(CLK_3DGSrc_Type clockSource);

void CLK_ModuleClkEnable(CLK_Module_Type clkModule);
void CLK_ModuleClkDisable(CLK_Module_Type clkModule);
void CLK_MemClkEnable(CLK_MemBlock_Type clockMemBlock);
void CLK_MemClkDisable(CLK_MemBlock_Type clockMemBlock);

void CLK_I2SClkDivider(uint32_t divider1, uint32_t divider2);
void CLK_IRClkDivider(uint32_t divider);
void RC32K_CalClk_Div(uint32_t divider1, uint32_t divider2);

IntStatus RC32M_GetIntStatus(RC32M_INT_Type intType);
#ifndef USE_BASICDRV_IN_ROM
void RC32M_SelOutput32MHz(RC32M_SELFREQ_Type freqType);
#endif // USE_BASICDRV_IN_ROM
void RC32M_IntMask(RC32M_INT_Type intType, IntMask_Type intMask);
void RC32M_IntClr(RC32M_INT_Type intType);
void RC32M_IRQHandler(void);

IntStatus XTAL64M_GetIntStatus(X32M_INT_Type intType);
void XTAL64M_IntMask(X32M_INT_Type intType, IntMask_Type intMask);
void XTAL64M_IntClr(X32M_INT_Type intType);
void XTAL64M_IRQHandler(void);
