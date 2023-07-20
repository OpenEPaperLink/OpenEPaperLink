#include "mz100_clock.h"
#include "mz100_drv_config.h"

void CLK_Xtal32MEnable(CLK_CrystalMode_Type oscMode)
{
  switch (oscMode)
  {
  case CLK_OSC_INTERN:
    /* Internal oscillator mode */
    X32M->STATE.BF.REG_BYPASS_EN = 0;
    X32M->STATE.BF.EXT_OSC_EN = 0;
    break;

  case CLK_OSC_EXTERN:
    /* External oscillator mode */
    X32M->STATE.BF.REG_BYPASS_EN = 1;
    X32M->STATE.BF.EXT_OSC_EN = 1;
    break;

  default:
    break;
  }

  /* Power up */
  X32M->STATE.BF.CLK_SLEEP_ON = 0;
  X32M->CNTL.BF.CLK_ANA_32M_EN = 1;
  X32M->CNTL.BF.CLK_DIG_32M_EN = 1;
  X32M->CNTL.BF.PD = 0;
}

/****************************************************************************/ /**
                                                                                * @brief      Disable main crystal oscillator
                                                                                *
                                                                                * @param  none
                                                                                *
                                                                                * @return none
                                                                                *
                                                                                *******************************************************************************/
void CLK_Xtal32MDisable(void)
{
  /* Power down */
  X32M->STATE.BF.CLK_SLEEP_ON = 1;
  X32M->CNTL.BF.CLK_ANA_32M_EN = 0;
  X32M->CNTL.BF.CLK_DIG_32M_EN = 0;
  X32M->CNTL.BF.PD = 1;
}

/****************************************************************************/ /**
                                                                                * @brief      Enable RC32M oscillator
                                                                                *
                                                                                * @param  none
                                                                                *
                                                                                * @return none
                                                                                *
                                                                                *******************************************************************************/
void CLK_RC32MEnable(void)
{
  /* Power up */
  RC32M->CTRL.BF.PD = 0;
  RC32M->CTRL.BF.EN = 1;
}

/****************************************************************************/ /**
                                                                                * @brief      Disable RC32M oscillator
                                                                                *
                                                                                * @param  none
                                                                                *
                                                                                * @return none
                                                                                *
                                                                                *******************************************************************************/
void CLK_RC32MDisable(void)
{
  /* Power down */
  RC32M->CTRL.BF.EN = 0;
  RC32M->CTRL.BF.PD = 1;
}

/****************************************************************************/ /**
                                                                                * @brief      Enable 32khz crystal oscillator
                                                                                *
                                                                                * @param[in]  oscMode: Select the oscillator
                                                                                *
                                                                                * @return none
                                                                                *
                                                                                *******************************************************************************/
void CLK_Xtal32KEnable(CLK_CrystalMode_Type oscMode)
{
  switch (oscMode)
  {
  case CLK_OSC_INTERN:
    /* Internal oscillator mode */
    PMU->XTAL_CLK_SWITCH.BF.XTAL_32K_EXT_OSC_EN = 0;
    break;

  case CLK_OSC_EXTERN:
    /* External oscillator mode */
    PMU->XTAL_CLK_SWITCH.BF.XTAL_32K_EXT_OSC_EN = 1;
    break;

  default:
    break;
  }

  /* Power up */
  PMU->XTAL_CLK_SWITCH.BF.XTAL_32K_EN = 1;
}

/****************************************************************************/ /**
                                                                                * @brief      Disable 32khz crystal oscillator
                                                                                *
                                                                                * @param  none
                                                                                *
                                                                                * @return none
                                                                                *
                                                                                *******************************************************************************/
void CLK_Xtal32KDisable(void)
{
  /* Power down */
  PMU->XTAL_CLK_SWITCH.BF.XTAL_32K_EN = 0;
}

/****************************************************************************/ /**
                                                                                * @brief      Enable RC32K oscillator
                                                                                *
                                                                                * @param  none
                                                                                *
                                                                                * @return none
                                                                                *
                                                                                *******************************************************************************/
void CLK_RC32KEnable(void)
{
  /* Power up */
  PMU->RC32K_PWR_CNTL.BF.RC32K_PWRDN = 0;
}

/****************************************************************************/ /**
                                                                                * @brief      Disable RC32K oscillator
                                                                                *
                                                                                * @param  none
                                                                                *
                                                                                * @return none
                                                                                *
                                                                                *******************************************************************************/
void CLK_RC32KDisable(void)
{
  /* Power down */
  PMU->RC32K_PWR_CNTL.BF.RC32K_PWRDN = 1;
}

/****************************************************************************/ /**
                                                                                * @brief     Get the clock for UART module
                                                                                *
                                                                                * @param[in] none
                                                                                *
                                                                                * @return    UART clock frequency
                                                                                *
                                                                                *******************************************************************************/
uint32_t CLK_GetUARTClk(void)
{
  uint32_t uartClkSource;
  uint32_t clkSourceFreq = MAINXTAL_FREQ;

  uartClkSource = PMU->CLK_SRC.BF.MAIN_CLK_SOURCE & 0x1;

  if (uartClkSource)
  {
    clkSourceFreq = RC32M_FREQ;
  }

  return clkSourceFreq;
}

/****************************************************************************/ /**
                                                                                * @brief      RC32M clock calibration function
                                                                                *
                                                                                * @param[in]  rcCalAutoManOption: Calibration option, use manual or auto way
                                                                                * @param[in]  extCalCode: Select manual way, need input the calibration code
                                                                                *
                                                                                * @return     return the calibration result
                                                                                *             -1, calibration failure
                                                                                *             else, internal calibration code
                                                                                *
                                                                                * @note       1. Select CLK_XTAL32M as system clock before RC32M calibration.
                                                                                *
                                                                                *             2. While calibrating RTC/GPT1/GPT2/CLKOUT clock source will be
                                                                                *             temporarily set to CLK_XTAL32K/CLK_XTAL32M/CLK_XTAL32M/CLK_XTAL32M
                                                                                *             respectively. And restore to original value after calibration done.
                                                                                *
                                                                                *******************************************************************************/
int32_t CLK_RC32MCalibration(CLK_RCCal_Type rcCalAutoManOption, uint32_t extCalCode)
{
  volatile uint32_t localCnt;

  uint32_t clkSource;

  /* Soft reset RC32M */
  RC32M->RST.BF.SOFT_RST = 1;
  for (localCnt = 0; localCnt < 10; localCnt++)
    ;
  RC32M->RST.BF.SOFT_RST = 0;

  /* Power up RC32M */
  RC32M->CTRL.BF.PD = 0;
  RC32M->CTRL.BF.EN = 1;

  localCnt = 0;
  while (1)
  {
    /* Check RC32M_RDY status */
    if (PMU->CLK_STATUS.BF.RC_32MHZ == 1)
    {
      break;
    }
    else if (localCnt > 100000)
    {
      return -1;
    }

    localCnt++;
  }

  /* Temporarily set CLK_SRC */
  clkSource = PMU->CLK_SRC.WORDVAL;
  PMU->CLK_SRC.WORDVAL = 0x100;
  //  PMU->CLK_SRC.WORDVAL = 0x100 | (clkSource & 0xF80);

  switch (rcCalAutoManOption)
  {
  case CLK_AUTO_CAL:
    /* Auto */
    RC32M->CTRL.BF.EXT_CODE_EN = 0;
    RC32M->CTRL.BF.CAL_EN = 1;
    break;

  case CLK_MANUAL_CAL:
    /* Manual */
    RC32M->CTRL.BF.CODE_FR_EXT = extCalCode;
    RC32M->CTRL.BF.EXT_CODE_EN = 1;
    break;

  default:
    break;
  }

  localCnt = 0;
  while (1)
  {
    /* Check CAL_DONE status */
    if ((RC32M->STATUS.BF.CAL_DONE == 1) || (RC32M->CTRL.BF.EXT_CODE_EN == 1))
    {
      break;
    }
    else if (localCnt > 100000)
    {
      PMU->CLK_SRC.WORDVAL = clkSource;
      RC32M->CTRL.BF.CAL_EN = 0;
      return -1;
    }

    localCnt++;
  }

  RC32M->CTRL.BF.CAL_EN = 0;

  /* Restore CLK_SRC */
  PMU->CLK_SRC.WORDVAL = clkSource;

  return RC32M->STATUS.BF.CODE_FR_CAL;
}

/****************************************************************************/ /**
                                                                                * @brief      RC32K clock calibration function
                                                                                *
                                                                                * @param[in]  referencClk: reference clock source, use xtal32k or xtal64m/2048
                                                                                * @param[in]  rcCalAutoManOption: Calibration option, use manual or auto way
                                                                                * @param[in]  extCalCode: Select manual way, need input the calibration code
                                                                                *
                                                                                * @return     return the calibration result
                                                                                *             -1, calibration failure
                                                                                *             else, internal calibration code
                                                                                *
                                                                                * @note       1. Clock CLK_XTAL32K or CLK_XTAL32M must be ready before RC32K
                                                                                *             calibration, depending on refence clock source.
                                                                                *
                                                                                *             2. While calibrating RTC/GPT1/GPT2/CLKOUT clock source will be
                                                                                *             temporarily set to CLK_XTAL32K/CLK_XTAL32M/CLK_XTAL32M/CLK_XTAL32M
                                                                                *             respectively. And restore to original value after calibration done.
                                                                                *******************************************************************************/
int32_t CLK_RC32KCalibration(CLK_RC32KCalSrc_Type referenceClk, CLK_RCCal_Type rcCalAutoManOption, uint32_t extCalCode)
{
  volatile uint32_t localCnt;

  uint32_t clkSource;

  /* Power up RC32K */
  PMU->RC32K_PWR_CNTL.BF.RC32K_PWRDN = 0;

  localCnt = 0;

  while (1)
  {
    /* Check RC32K_RDY status */
    if (PMU->CLK_STATUS.BF.RC_32KHZ == 1)
    {
      break;
    }
    else if (localCnt > 100000)
    {
      return -1;
    }
    localCnt++;
  }

  /* Temporarily set CLK_SRC */
  clkSource = PMU->CLK_SRC.WORDVAL;
  PMU->CLK_SRC.WORDVAL = 0x100 | (clkSource & 0x1);

  switch (rcCalAutoManOption)
  {
  case CLK_AUTO_CAL:
    if (referenceClk == CLK_RC32KCAL_XTAL32K)
    {
      /* Select XTAL32K as calibration reference clock */
      PMU->RC32K_CAL_CNTL.BF.RC32K_CAL_SRC = 1;
    }
    else if (referenceClk == CLK_RC32KCAL_XTAL64M)
    {
      /* Select XTAL64M/2048 as calibration reference clock */
      PMU->RC32K_CAL_CNTL.BF.RC32K_CAL_SRC = 0;
    }

    localCnt = 0;
    while (1)
    {
      /* Check CAL_ALLOW */
      if (PMU->RC32K_CAL_STAT.BF.RC32K_CAL_ALLOW == 1)
      {
        break;
      }
      else if (localCnt > 100000)
      {
        PMU->CLK_SRC.WORDVAL = clkSource;
        return -1;
      }
      localCnt++;
    }

    /* Auto */
    PMU->RC32K_EXT_CAL.BF.RC32K_EXT_CODE_EN = 0;

    /* Start Calibration */
    PMU->RC32K_CAL_CNTL.BF.RC32K_CAL_EN = 1;

    localCnt = 0;
    while (1)
    {
      if ((PMU->RC32K_CAL_STAT.BF.RC32K_CAL_DONE == 0) || (PMU->RC32K_EXT_CAL.BF.RC32K_EXT_CODE_EN == 1))
      {
        break;
      }
      else if (localCnt > 50000)
      {
        break;
      }
      localCnt++;
    }

    localCnt = 0;
    while (1)
    {
      /* Check CAL_DONE status */
      if ((PMU->RC32K_CAL_STAT.BF.RC32K_CAL_DONE == 1) || (PMU->RC32K_EXT_CAL.BF.RC32K_EXT_CODE_EN == 1))
      {
        break;
      }
      else if (localCnt > 100000)
      {
        PMU->CLK_SRC.WORDVAL = clkSource;
        return -1;
      }

      localCnt++;
    }
    /* Stop Calibration */
    PMU->RC32K_CAL_CNTL.BF.RC32K_CAL_EN = 0;

    break;

  /* Manual */
  case CLK_MANUAL_CAL:
    if (referenceClk == CLK_RC32KCAL_XTAL32K)
    {
      /* Select XTAL32K as calibration reference clock */
      PMU->RC32K_CAL_CNTL.BF.RC32K_CAL_SRC = 1;
    }
    else if (referenceClk == CLK_RC32KCAL_XTAL64M)
    {
      /* Select XTAL64M/2048 as calibration reference clock */
      PMU->RC32K_CAL_CNTL.BF.RC32K_CAL_SRC = 0;
    }

    PMU->RC32K_EXT_CAL.BF.RC32K_CODE_FR_EXT = extCalCode;
    PMU->RC32K_EXT_CAL.BF.RC32K_EXT_CODE_EN = 1;

    /* Start Calibration */
    PMU->RC32K_CAL_CNTL.BF.RC32K_CAL_EN = 1;

    /* Add a short delay to wait manual cal result update */
    for (volatile uint32_t i = 0; i < 0x100; i++)
      ;

    /* Stop Calibration */
    PMU->RC32K_CAL_CNTL.BF.RC32K_CAL_EN = 0;
    break;

  default:
    break;
  }

  /* Restore CLK_SRC */
  PMU->CLK_SRC.WORDVAL = clkSource;

  return PMU->RC32K_CAL_STAT.BF.RC32K_CODE_FR_CAL;
}

/****************************************************************************/ /**
                                                                                * @brief     Select the system clock source
                                                                                *
                                                                                * @param[in]  clockSource:  Clock source for system clock(RC32M/XT32M)
                                                                                * @param[in]  clockSys   :  System clock frequency value
                                                                                *
                                                                                * @return none
                                                                                *
                                                                                *******************************************************************************/
void CLK_SystemClkInit(CLK_SysSrc_Type clockSource, CLK_SysFreq_Type clockSys)
{
  PMU->CLK_SRC.BF.MAIN_CLK_SOURCE = clockSource;
  PMU->CM3_FREQ.BF.MAIN_CLK_FREQ = clockSys;
}

/****************************************************************************/ /**
                                                                                * @brief     Get the system clock frequency, according to CLK_SRC and CM3_FREQ Register
                                                                                *
                                                                                * @param[out]  sysClkFreq:  Clock source for system clock(RC32M/XT32M)
                                                                                *
                                                                                * @return none
                                                                                *
                                                                                *******************************************************************************/
uint32_t CLK_GetSystemClkFreq(void)
{
  uint32_t sysClkFreq = 0;
  switch (PMU->CLK_SRC.BF.MAIN_CLK_SOURCE)
  {
  case 0x0:
    if (PMU->CM3_FREQ.BF.MAIN_CLK_FREQ == 0x01)
    {
      sysClkFreq = 1016000;
    }
    else
    {
      sysClkFreq = PMU->CM3_FREQ.BF.MAIN_CLK_FREQ * 1000000;
    }
    break;
  case 0x1:
    if (PMU->CM3_FREQ.BF.MAIN_CLK_FREQ == 0x40)
    {
      sysClkFreq = XTAL64M_FREQ;
    }
    else
    {
      sysClkFreq = (XTAL64M_FREQ / 64) * PMU->CM3_FREQ.BF.MAIN_CLK_FREQ;
    }
    break;
  default:
    break;
  }
  return sysClkFreq;
}

/****************************************************************************/ /**
                                                                                * @brief     Select the module clock source
                                                                                *
                                                                                * @param[in]  clkModule    :  Clock module need for clock status
                                                                                * @param[in]  freq         :  Clock frequency into module, ignore rc frequency error and clock divider/prescaler in module
                                                                                *
                                                                                * @return module clock status
                                                                                *
                                                                                * Now support IR, UART1/2
                                                                                *******************************************************************************/
FunctionalState CLK_GetModuleClock(CLK_Module_Type clkModule, uint32_t *freq)
{
  FunctionalState clkStatus;
  uint32_t clockSource = PMU->CLK_SRC.BF.MAIN_CLK_SOURCE;
  switch (clkModule)
  {
  case CLK_UART2:
    clkStatus = (PMU->CLK_ENABLE.BF.DISABLE_UART2) ? (DISABLE) : (ENABLE);
    *freq = MAINXTAL_FREQ;
    break;
  case CLK_UART1:
    clkStatus = (PMU->CLK_ENABLE.BF.DISABLE_UART1) ? (DISABLE) : (ENABLE);
    *freq = MAINXTAL_FREQ;
    break;
  case CLK_IR:
    clkStatus = (PMU->CLK_ENABLE.BF.DISABLE_IR) ? (DISABLE) : (ENABLE);
    *freq = MAINXTAL_FREQ * ((clockSource == 1) ? (1) : (2)) / (PMU->IR_DIV.BF.IR_CLK_DIV);
    break;
  default:
    break;
  }
  return clkStatus;
}

/****************************************************************************/ /**
                                                                                * @brief     Select the output clock source
                                                                                *
                                                                                * @param[in]  clockSourc   :  Clock source for output
                                                                                *
                                                                                * @return none
                                                                                *
                                                                                *******************************************************************************/
void CLK_OUTClkSrc(CLK_OutSrc_Type clockSourc)
{
  PMU->CLK_SRC.BF.OUT_CLK_SOURCE = clockSourc;
}

/****************************************************************************/ /**
                                                                                * @brief     Select the clock source for GPT intrnal clock
                                                                                *
                                                                                * @param[in]  gptIndex:  Index to GPT (GPT0/1/2/3)
                                                                                * @param[in]  clockSource:  Clock source for gpt (RC32M/XT32M/RC32K/XTAL32K/SYSCLK)
                                                                                *
                                                                                * @return none
                                                                                *
                                                                                *******************************************************************************/
void CLK_GPTInternalClkSrc(CLK_GptID_Type gptIndex, CLK_GPTSrc_Type clockSource)
{
  if (gptIndex == CLK_GPT_ID_1)
  {
    switch (clockSource)
    {
    case CLK_GPT_RC32K:
      PMU->CLK_SRC.BF.GPT1_CLK_SOURCE = 3;
      break;

    case CLK_GPT_XTAL32K:
      PMU->CLK_SRC.BF.GPT1_CLK_SOURCE = 2;
      break;

    case CLK_GPT_RC32M:
      PMU->CLK_SRC.BF.GPT1_CLK_SOURCE = 1;
      break;

    case CLK_GPT_XTAL32M:
      PMU->CLK_SRC.BF.GPT1_CLK_SOURCE = 0;
      break;

    default:
      break;
    }
  }
  else if (gptIndex == CLK_GPT_ID_2)
  {
    switch (clockSource)
    {
    case CLK_GPT_RC32K:
      PMU->CLK_SRC.BF.GPT2_CLK_SOURCE = 3;
      break;

    case CLK_GPT_XTAL32K:
      PMU->CLK_SRC.BF.GPT2_CLK_SOURCE = 2;
      break;

    case CLK_GPT_RC32M:
      PMU->CLK_SRC.BF.GPT2_CLK_SOURCE = 1;
      break;

    case CLK_GPT_XTAL32M:
      PMU->CLK_SRC.BF.GPT2_CLK_SOURCE = 0;
      break;

    default:
      break;
    }
  }
}

/****************************************************************************/ /**
                                                                                * @brief     Select the clock source for RTC
                                                                                *
                                                                                * @param[in]  clockSource:  Clock source for RTC (RC32K/XT32K)
                                                                                *
                                                                                * @return none
                                                                                *
                                                                                *******************************************************************************/
void CLK_RTCClkSrc(CLK_RTCSrc_Type clockSource)
{
  switch (clockSource)
  {
  case CLK_RTC_RC32K:
    PMU->CLK_SRC.BF.RTC_CLK_SOURCE = 1;
    break;

  case CLK_RTC_XTAL32K:
    PMU->CLK_SRC.BF.RTC_CLK_SOURCE = 0;
    break;

  default:
    break;
  }
}

/****************************************************************************/ /**
                                                                                * @brief     Select the clock source for I2S
                                                                                *
                                                                                * @param[in]  clockSource:  Clock source for I2S (RC32M/X32M/RC32K/X32K)
                                                                                *
                                                                                * @return none
                                                                                *
                                                                                *******************************************************************************/
void CLK_I2SClkSrc(CLK_I2SSrc_Type clockSource)
{
  switch (clockSource)
  {
  case CLK_I2S_XTAL32M:
    PMU->CLK_SRC.BF.I2S_CLK_SOURCE = 0;
    break;

  case CLK_I2S_RC32M:
    PMU->CLK_SRC.BF.I2S_CLK_SOURCE = 1;
    break;

  case CLK_I2S_XTAL32K:
    PMU->CLK_SRC.BF.I2S_CLK_SOURCE = 2;
    break;

  case CLK_I2S_RC32K:
    PMU->CLK_SRC.BF.I2S_CLK_SOURCE = 3;
    break;

  default:
    break;
  }
}

/****************************************************************************/ /**
                                                                                * @brief     Select the clock source for Keyscan
                                                                                *
                                                                                * @param[in]  clockSource:  Clock source for KEYSCAN (RC32K/XT32K)
                                                                                *
                                                                                * @return none
                                                                                *
                                                                                *******************************************************************************/
void CLK_KEYSCANClkSrc(CLK_KeyscanSrc_Type clockSource)
{
  switch (clockSource)
  {
  case CLK_KEYSCAN_XTAL32K:
    PMU->CLK_SRC.BF.KEYSCAN_CLK_SOURCE = 0;
    break;

  case CLK_KEYSCAN_RC32K:
    PMU->CLK_SRC.BF.KEYSCAN_CLK_SOURCE = 1;
    break;

  default:
    break;
  }
}

/****************************************************************************/ /**
                                                                                * @brief     Select the clock source for Trackball
                                                                                *
                                                                                * @param[in]  clockSource:  Clock source for Trackball (RC32K/XT32K)
                                                                                *
                                                                                * @return none
                                                                                *
                                                                                *******************************************************************************/
void CLK_TRACKBALLClkSrc(CLK_TrackballSrc_Type clockSource)
{
  switch (clockSource)
  {
  case CLK_TRACKBALL_XTAL32K:
    PMU->CLK_SRC.BF.TRACKBALL_CLK_SOURCE = 0;
    break;

  case CLK_TRACKBALL_RC32K:
    PMU->CLK_SRC.BF.TRACKBALL_CLK_SOURCE = 1;
    break;

  default:
    break;
  }
}

/****************************************************************************/ /**
                                                                                * @brief     Select the clock source for SSP1/2
                                                                                *
                                                                                * @param[in]  clockSource:  Clock source for SSp1/2 (I2S CLOCK/SYSTEM CLOCK)
                                                                                *
                                                                                * @return none
                                                                                *
                                                                                *******************************************************************************/
void CLK_SSPClkSrc(CLK_SspID_Type sspIndex, CLK_SSPSrc_Type clockSource)
{
  if (sspIndex == CLK_SSP_ID_1)
  {
    switch (clockSource)
    {
    case CLK_SSP_I2S:
      PMU->CLK_SRC.BF.SPI1_CLK_SOURCE = 0;
      break;

    case CLK_SSP_SYSTEM:
      PMU->CLK_SRC.BF.SPI1_CLK_SOURCE = 1;
      break;

    default:
      break;
    }
  }
  else if (sspIndex == CLK_SSP_ID_2)
  {
    switch (clockSource)
    {
    case CLK_SSP_I2S:
      PMU->CLK_SRC.BF.SPI2_CLK_SOURCE = 0;
      break;

    case CLK_SSP_SYSTEM:
      PMU->CLK_SRC.BF.SPI2_CLK_SOURCE = 1;
      break;

    default:
      break;
    }
  }
}

/****************************************************************************/ /**
                                                                                * @brief     Select the clock source for 3D Glass
                                                                                *
                                                                                * @param[in]  clockSource:  Clock source for 3D Glass (RC32K/XT32K)
                                                                                *
                                                                                * @return none
                                                                                *
                                                                                *******************************************************************************/
void CLK_3DGClkSrc(CLK_3DGSrc_Type clockSource)
{
  switch (clockSource)
  {
  case CLK_3DG_XTAL32K:
    PMU->CLK_SRC.BF._3DG_CLK_SOURCE = 0;
    break;

  case CLK_3DG_RC32K:
    PMU->CLK_SRC.BF._3DG_CLK_SOURCE = 1;
    break;

  default:
    break;
  }
}

/****************************************************************************/ /**
                                                                                * @brief      Module clock enable
                                                                                *
                                                                                * @param[in]  clockModule: Select the clock module
                                                                                *
                                                                                * @return none
                                                                                *
                                                                                *******************************************************************************/
void CLK_ModuleClkEnable(CLK_Module_Type clockModule)
{
  switch (clockModule)
  {
  case CLK_OUT:
    PMU->CLK_ENABLE.BF.DISABLE_OUT_CLK = 0;
    break;

  case CLK_RC32K_CAL:
    PMU->CLK_ENABLE.BF.DISABLE_RC32K_CAL = 0;
    break;

  case CLK_SYSCTRL:
    PMU->CLK_ENABLE.BF.DISABLE_SYSCTRL = 0;
    break;

  case CLK_CAU1:
    PMU->CLK_ENABLE.BF.DISABLE_CAU1 = 0;
    break;

  case CLK_SSP2:
    PMU->CLK_ENABLE.BF.DISABLE_SPI2 = 0;
    break;

  case CLK_SSP1:
    PMU->CLK_ENABLE.BF.DISABLE_SPI1 = 0;
    break;

  case CLK_QSPI:
    PMU->CLK_ENABLE.BF.DISABLE_QSPI = 0;
    break;

  case CLK_GPIO:
    PMU->CLK_ENABLE.BF.DISABLE_GPIO = 0;
    break;

  case CLK_GPT2:
    PMU->CLK_ENABLE.BF.DISABLE_GPT2 = 0;
    break;

  case CLK_GPT1:
    PMU->CLK_ENABLE.BF.DISABLE_GPT1 = 0;
    break;

  case CLK_PINMUX:
    PMU->CLK_ENABLE.BF.DISABLE_PINMUX = 0;
    break;

  case CLK_RTC:
    PMU->CLK_ENABLE.BF.DISABLE_RTC = 0;
    break;

  case CLK_CAU0:
    PMU->CLK_ENABLE.BF.DISABLE_CAU0 = 0;
    break;

  case CLK_WDT:
    PMU->CLK_ENABLE.BF.DISABLE_WDT = 0;
    break;

  case CLK_UART2:
    PMU->CLK_ENABLE.BF.DISABLE_UART2 = 0;
    break;

  case CLK_UART1:
    PMU->CLK_ENABLE.BF.DISABLE_UART1 = 0;
    break;

  case CLK_I2C2:
    PMU->CLK_ENABLE.BF.DISABLE_I2C2 = 0;
    break;

  case CLK_I2C1:
    PMU->CLK_ENABLE.BF.DISABLE_I2C1 = 0;
    break;

  case CLK_APB0:
    PMU->CLK_ENABLE.BF.DISABLE_APB0 = 0;
    break;

  case CLK_AES_CRC:
    PMU->CLK_ENABLE.BF.DISABLE_AES = 0;
    break;

  case CLK_PHY:
    PMU->CLK_ENABLE.BF.DISABLE_PHY = 0;
    break;

  case CLK_DMA:
    PMU->CLK_ENABLE.BF.DISABLE_DMA = 0;
    break;

  case CLK_COMP:
    PMU->CLK_ENABLE.BF.DISABLE_COMP = 0;
    break;

  case CLK_DAC:
    PMU->CLK_ENABLE.BF.DISABLE_DAC = 0;
    break;

  case CLK_ADC:
    PMU->CLK_ENABLE.BF.DISABLE_ADC = 0;
    break;

  case CLK_KEYSCAN:
    PMU->CLK_ENABLE.BF.DISABLE_KEYSCAN = 0;
    break;

  case CLK_TRACKBALL:
    PMU->CLK_ENABLE.BF.DISABLE_TRACKBALL = 0;
    break;

  case CLK_IR:
    PMU->CLK_ENABLE.BF.DISABLE_IR = 0;
    break;

  case CLK_3DG:
    PMU->CLK_ENABLE.BF.DISABLE_3DG = 0;
    break;

  case CLK_XTAL32K_CLK:
    PMU->CLK_ENABLE.BF.DISABLE_XTAL_CLK_32K = 0;
    break;

  default:
    break;
  }
}

/****************************************************************************/ /**
                                                                                * @brief      Module clock disable
                                                                                *
                                                                                * @param[in]  clockModule: Select the clock module
                                                                                *
                                                                                * @return none
                                                                                *
                                                                                *******************************************************************************/
void CLK_ModuleClkDisable(CLK_Module_Type clockModule)
{
  switch (clockModule)
  {
  case CLK_OUT:
    PMU->CLK_ENABLE.BF.DISABLE_OUT_CLK = 1;
    break;

  case CLK_RC32K_CAL:
    PMU->CLK_ENABLE.BF.DISABLE_RC32K_CAL = 1;
    break;

  case CLK_SYSCTRL:
    PMU->CLK_ENABLE.BF.DISABLE_SYSCTRL = 1;
    break;

  case CLK_CAU1:
    PMU->CLK_ENABLE.BF.DISABLE_CAU1 = 1;
    break;

  case CLK_SSP2:
    PMU->CLK_ENABLE.BF.DISABLE_SPI2 = 1;
    break;

  case CLK_SSP1:
    PMU->CLK_ENABLE.BF.DISABLE_SPI1 = 1;
    break;

  case CLK_QSPI:
    PMU->CLK_ENABLE.BF.DISABLE_QSPI = 1;
    break;

  case CLK_GPIO:
    PMU->CLK_ENABLE.BF.DISABLE_GPIO = 1;
    break;

  case CLK_GPT2:
    PMU->CLK_ENABLE.BF.DISABLE_GPT2 = 1;
    break;

  case CLK_GPT1:
    PMU->CLK_ENABLE.BF.DISABLE_GPT1 = 1;
    break;

  case CLK_PINMUX:
    PMU->CLK_ENABLE.BF.DISABLE_PINMUX = 1;
    break;

  case CLK_RTC:
    PMU->CLK_ENABLE.BF.DISABLE_RTC = 1;
    break;

  case CLK_CAU0:
    PMU->CLK_ENABLE.BF.DISABLE_CAU0 = 1;
    break;

  case CLK_WDT:
    PMU->CLK_ENABLE.BF.DISABLE_WDT = 1;
    break;

  case CLK_UART2:
    PMU->CLK_ENABLE.BF.DISABLE_UART2 = 1;
    break;

  case CLK_UART1:
    PMU->CLK_ENABLE.BF.DISABLE_UART1 = 1;
    break;

  case CLK_I2C2:
    PMU->CLK_ENABLE.BF.DISABLE_I2C2 = 1;
    break;

  case CLK_I2C1:
    PMU->CLK_ENABLE.BF.DISABLE_I2C1 = 1;
    break;

  case CLK_APB0:
    PMU->CLK_ENABLE.BF.DISABLE_APB0 = 1;
    break;

  case CLK_AES_CRC:
    PMU->CLK_ENABLE.BF.DISABLE_AES = 1;
    break;

  case CLK_PHY:
    PMU->CLK_ENABLE.BF.DISABLE_PHY = 1;
    break;

  case CLK_DMA:
    PMU->CLK_ENABLE.BF.DISABLE_DMA = 1;
    break;

  case CLK_COMP:
    PMU->CLK_ENABLE.BF.DISABLE_COMP = 1;
    break;

  case CLK_DAC:
    PMU->CLK_ENABLE.BF.DISABLE_DAC = 1;
    break;

  case CLK_ADC:
    PMU->CLK_ENABLE.BF.DISABLE_ADC = 1;
    break;

  case CLK_KEYSCAN:
    PMU->CLK_ENABLE.BF.DISABLE_KEYSCAN = 1;
    break;

  case CLK_TRACKBALL:
    PMU->CLK_ENABLE.BF.DISABLE_TRACKBALL = 1;
    break;

  case CLK_IR:
    PMU->CLK_ENABLE.BF.DISABLE_IR = 1;
    break;

  case CLK_3DG:
    PMU->CLK_ENABLE.BF.DISABLE_3DG = 1;
    break;

  case CLK_XTAL32K_CLK:
    PMU->CLK_ENABLE.BF.DISABLE_XTAL_CLK_32K = 1;
    break;

  default:
    break;
  }
}

/****************************************************************************/ /**
                                                                                * @brief      Memery block clock enable
                                                                                *
                                                                                * @param[in]  clockModule: Select the clock memery block
                                                                                *
                                                                                * @return none
                                                                                *
                                                                                *******************************************************************************/
void CLK_MemClkEnable(CLK_MemBlock_Type clockMemBlock)
{
  switch (clockMemBlock)
  {
  case CLK_MEM_ROM0:
    PMU->MEM_CLK_ENABLE.BF.DISABLE_ROM0 = 0;
    break;

  case CLK_MEM_ROM1:
    PMU->MEM_CLK_ENABLE.BF.DISABLE_ROM1 = 0;
    break;

  case CLK_MEM_RAM0:
    PMU->MEM_CLK_ENABLE.BF.DISABLE_RAM0 = 0;
    break;

  case CLK_MEM_RAM1:
    PMU->MEM_CLK_ENABLE.BF.DISABLE_RAM1 = 0;
    break;

  case CLK_MEM_RAM2:
    PMU->MEM_CLK_ENABLE.BF.DISABLE_RAM2 = 0;
    break;

  case CLK_MEM_RAM3:
    PMU->MEM_CLK_ENABLE.BF.DISABLE_RAM3 = 0;
    break;

  case CLK_MEM_RAM4:
    PMU->MEM_CLK_ENABLE.BF.DISABLE_RAM4 = 0;
    break;

  case CLK_MEM_AON_MEM:
    PMU->MEM_CLK_ENABLE.BF.DISABLE_AON_MEM = 0;
    break;

  default:
    break;
  }
}

/****************************************************************************/ /**
                                                                                * @brief      Memery block clock disable
                                                                                *
                                                                                * @param[in]  clockModule: Select the clock memery block
                                                                                *
                                                                                * @return none
                                                                                *
                                                                                *******************************************************************************/
void CLK_MemClkDisable(CLK_MemBlock_Type clockMemBlock)
{
  switch (clockMemBlock)
  {
  case CLK_MEM_ROM0:
    PMU->MEM_CLK_ENABLE.BF.DISABLE_ROM0 = 1;
    break;

  case CLK_MEM_ROM1:
    PMU->MEM_CLK_ENABLE.BF.DISABLE_ROM1 = 1;
    break;

  case CLK_MEM_RAM0:
    PMU->MEM_CLK_ENABLE.BF.DISABLE_RAM0 = 1;
    break;

  case CLK_MEM_RAM1:
    PMU->MEM_CLK_ENABLE.BF.DISABLE_RAM1 = 1;
    break;

  case CLK_MEM_RAM2:
    PMU->MEM_CLK_ENABLE.BF.DISABLE_RAM2 = 1;
    break;

  case CLK_MEM_RAM3:
    PMU->MEM_CLK_ENABLE.BF.DISABLE_RAM3 = 1;
    break;

  case CLK_MEM_RAM4:
    PMU->MEM_CLK_ENABLE.BF.DISABLE_RAM4 = 1;
    break;

  case CLK_MEM_AON_MEM:
    PMU->MEM_CLK_ENABLE.BF.DISABLE_AON_MEM = 1;
    break;

  default:
    break;
  }
}

/****************************************************************************/ /**
                                                                                * @brief      Set I2S clock divider
                                                                                *
                                                                                * @return none
                                                                                *
                                                                                *******************************************************************************/
void CLK_I2SClkDivider(uint32_t divider1, uint32_t divider2)
{
  /* check the parameters */
  //    CHECK_PARAM( IS_I2S_DIVIDER(divider) );
  PMU->I2S_DIV.BF.I2S_CLK_DIV1 = (divider1 & 0x3F);
  PMU->I2S_DIV.BF.I2S_CLK_DIV2 = (divider2 & 0x3F);
}

/****************************************************************************/ /**
                                                                                * @brief      Set I2S clock divider
                                                                                *
                                                                                * @return none
                                                                                *
                                                                                *******************************************************************************/
void CLK_IRClkDivider(uint32_t divider)
{
  /* check the parameters */
  //    CHECK_PARAM( IS_I2S_DIVIDER(divider) );
  PMU->IR_DIV.BF.IR_CLK_DIV = (divider & 0x3F);
}

/****************************************************************************/ /**
                                                                                * @brief      Get the clock source status
                                                                                *
                                                                                * @param[in]  clockSource: clock sourcem just for RC32M/XTAL32M/RC32K/XTAL32K
                                                                                *
                                                                                * @return     The clock source status
                                                                                *
                                                                                *******************************************************************************/
FlagStatus CLK_GetClkStatus(CLK_OutSrc_Type clockSource)
{
  FlagStatus bitStatus = RESET;

  switch (clockSource)
  {
  case CLK_OUT_RC32K:
    bitStatus = ((PMU->CLK_STATUS.BF.RC_32KHZ) ? SET : RESET);
    break;

  case CLK_OUT_RC32M:
    bitStatus = ((PMU->CLK_STATUS.BF.RC_32MHZ) ? SET : RESET);
    break;

  case CLK_OUT_XTAL32K:
    bitStatus = ((PMU->CLK_STATUS.BF.XTAL_32KHZ) ? SET : RESET);
    break;

  case CLK_OUT_XTAL64M:
    bitStatus = ((PMU->CLK_STATUS.BF.XTAL_64MHZ) ? SET : RESET);
    break;

  default:
    break;
  }

  return bitStatus;
}

/****************************************************************************/ /**
                                                                                * @brief      Get the RC32M interrupt status
                                                                                *
                                                                                * @param[in]  intType:  Specifies the interrupt type
                                                                                *
                                                                                * @return     Interrupt status value of the specified interrupt type
                                                                                *
                                                                                *******************************************************************************/
IntStatus RC32M_GetIntStatus(RC32M_INT_Type intType)
{
  IntStatus bitStatus = RESET;

  switch (intType)
  {
  /* RC32M calibration done interrupt */
  case RC32M_CALDON:
    bitStatus = ((RC32M->ISR.BF.CALDON_INT) ? SET : RESET);
    break;

  /* RC32M clock out ready interrupt */
  case RC32M_CKRDY:
    bitStatus = ((RC32M->ISR.BF.CKRDY_INT) ? SET : RESET);
    break;

  default:
    break;
  }

  return bitStatus;
}

/****************************************************************************/ /**
                                                                                * @brief      Mask/Unmask the specified RC32M interrupt type
                                                                                *
                                                                                * @param[in]  intType:  Specifies the interrupt type
                                                                                * @param[in]  intMask:  Mask/Unmask the specified interrupt type
                                                                                *
                                                                                * @return none
                                                                                *
                                                                                *******************************************************************************/
void RC32M_IntMask(RC32M_INT_Type intType, IntMask_Type intMask)
{
  switch (intType)
  {
  /* RC32M calibration done interrupt */
  case RC32M_CALDON:
    RC32M->IMR.BF.CALDON_INT_MSK = intMask;
    break;

  /* RC32M clock out ready interrupt */
  case RC32M_CKRDY:
    RC32M->IMR.BF.CKRDY_INT_MSK = intMask;
    break;

  default:
    break;
  }
}

/****************************************************************************/ /**
                                                                                * @brief      Clear the RC32M interrupt flag
                                                                                *
                                                                                * @param[in]  intType:  Specifies the interrupt type
                                                                                *
                                                                                * @return none
                                                                                *
                                                                                *******************************************************************************/
void RC32M_IntClr(RC32M_INT_Type intType)
{
  switch (intType)
  {
  /* RC32M calibration done interrupt */
  case RC32M_CALDON:
    RC32M->ICR.BF.CALDON_INT_CLR = 1;
    break;

  /* RC32M clock out ready interrupt */
  case RC32M_CKRDY:
    RC32M->ICR.BF.CKRDY_INT_CLR = 1;
    break;

  default:
    break;
  }
}

/****************************************************************************/ /**
                                                                                * @brief  RC32M interrupt function
                                                                                *
                                                                                * @param  none
                                                                                *
                                                                                * @return none
                                                                                *
                                                                                *******************************************************************************/
__attribute__((weak)) void RC32M_IRQHandler(void)
{
  uint32_t intStatus;

  /* Store unmasked interrupt flags for later use */
  intStatus = (~RC32M->IMR.WORDVAL) & 0x00000003;
  intStatus &= RC32M->ISR.WORDVAL;

  /* Clear all unmasked interrupt flags */
  RC32M->ICR.WORDVAL = intStatus;

  /* RC32M calibration done interrupt */
  if (intStatus & (1 << RC32M_CALDON))
  {
    if (intCbfArra[INT_RC32M][RC32M_CALDON] != NULL)
    {
      /* call the callback function */
      intCbfArra[INT_RC32M][RC32M_CALDON]();
    }
    else
    {
      RC32M->IMR.BF.CALDON_INT_MSK = 1;
    }
  }

  /* RC32M clock out ready interrupt */
  if (intStatus & (1 << RC32M_CKRDY))
  {
    if (intCbfArra[INT_RC32M][RC32M_CKRDY] != NULL)
    {
      /* call the callback function */
      intCbfArra[INT_RC32M][RC32M_CKRDY]();
    }
    else
    {
      RC32M->IMR.BF.CKRDY_INT_MSK = 1;
    }
  }
}

/****************************************************************************/ /**
                                                                                * @brief      Get the XTAL64M interrupt status
                                                                                *
                                                                                * @param[in]  intType:  Specifies the interrupt type
                                                                                *
                                                                                * @return     Interrupt status value of the specified interrupt type
                                                                                *
                                                                                *******************************************************************************/
IntStatus XTAL64M_GetIntStatus(X32M_INT_Type intType)
{
  IntStatus bitStatus = RESET;

  switch (intType)
  {
  /* XTAL32M DIG_32M_RDY interrupt */
  case X32M_DIG_32M_RDY:
    bitStatus = ((X32M->ISR.BF.DIG_32M_RDY_INT) ? SET : RESET);
    break;

  /* XTAL32M ANA_32M_RDY interrupt */
  case X32M_ANA_32M_RDY:
    bitStatus = ((X32M->ISR.BF.ANA_32M_RDY_INT) ? SET : RESET);
    break;

  /* XTAL32M DIG_64M_RDY interrupt */
  case X32M_DIG_64M_RDY:
    bitStatus = ((X32M->ISR.BF.DIG_64M_RDY_INT) ? SET : RESET);
    break;

  default:
    break;
  }

  return bitStatus;
}

/****************************************************************************/ /**
                                                                                * @brief      Mask/Unmask the specified XTAL64M interrupt type
                                                                                *
                                                                                * @param[in]  intType:  Specifies the interrupt type
                                                                                * @param[in]  intMask:  Mask/Unmask the specified interrupt type
                                                                                *
                                                                                * @return none
                                                                                *
                                                                                *******************************************************************************/
void XTAL64M_IntMask(X32M_INT_Type intType, IntMask_Type intMask)
{
  switch (intType)
  {
  /* XTAL32M DIG_32M_RDY interrupt */
  case X32M_DIG_32M_RDY:
    X32M->IMR.BF.DIG_32M_RDY_INT_MSK = intMask;
    break;

  /* XTAL32M ANA_32M_RDY interrupt */
  case X32M_ANA_32M_RDY:
    X32M->IMR.BF.ANA_32M_RDY_INT_MSK = intMask;
    break;

  /* XTAL32M DIG_64M_RDY interrupt */
  case X32M_DIG_64M_RDY:
    X32M->IMR.BF.DIG_64M_RDY_INT_MSK = intMask;
    break;

  default:
    break;
  }
}

/****************************************************************************/ /**
                                                                                * @brief      Clear the XTAL64M interrupt flag
                                                                                *
                                                                                * @param[in]  intType:  Specifies the interrupt type
                                                                                *
                                                                                * @return none
                                                                                *
                                                                                *******************************************************************************/
void XTAL64M_IntClr(X32M_INT_Type intType)
{
  switch (intType)
  {
  /* XTAL32M DIG_32M_RDY interrupt */
  case X32M_DIG_32M_RDY:
    X32M->ICR.BF.DIG_32M_RDY_INT_CLR = 1;
    break;

  /* XTAL32M ANA_32M_RDY interrupt */
  case X32M_ANA_32M_RDY:
    X32M->ICR.BF.ANA_32M_RDY_INT_CLR = 1;
    break;

  /* XTAL32M DIG_64M_RDY interrupt */
  case X32M_DIG_64M_RDY:
    X32M->ICR.BF.DIG_64M_RDY_INT_CLR = 1;
    break;

  default:
    break;
  }
}

/****************************************************************************/ /**
                                                                                * @brief  XTAL64M interrupt function
                                                                                *
                                                                                * @param  none
                                                                                *
                                                                                * @return none
                                                                                *
                                                                                *******************************************************************************/
__attribute__((weak)) void XTAL64M_IRQHandler(void)
{
  uint32_t intStatus;

  /* Store unmasked interrupt flags for later use */
  intStatus = (~X32M->IMR.WORDVAL) & 0x7;
  intStatus &= X32M->ISR.WORDVAL;

  /* Clear all unmasked interrupt flags */
  X32M->ICR.WORDVAL = intStatus;

  /* XTAL32M digital 32M clock ready interrupt */
  if (intStatus & (uint32_t)((uint32_t)1 << X32M_DIG_32M_RDY))
  {
    if (intCbfArra[INT_XTAL32M][X32M_DIG_32M_RDY] != NULL)
    {
      /* call the callback function */
      intCbfArra[INT_XTAL32M][X32M_DIG_32M_RDY]();
    }
    else
    {
      X32M->IMR.BF.DIG_32M_RDY_INT_MSK = 1;
    }
  }

  /* XTAL32M analog 32M clock ready interrupt */
  if (intStatus & (uint32_t)((uint32_t)1 << X32M_ANA_32M_RDY))
  {
    if (intCbfArra[INT_XTAL32M][X32M_ANA_32M_RDY] != NULL)
    {
      /* call the callback function */
      intCbfArra[INT_XTAL32M][X32M_ANA_32M_RDY]();
    }
    else
    {
      X32M->IMR.BF.ANA_32M_RDY_INT_MSK = 1;
    }
  }

  /* XTAL32M digital 64M clock ready interrupt */
  if (intStatus & (uint32_t)((uint32_t)1 << X32M_DIG_64M_RDY))
  {
    if (intCbfArra[INT_XTAL32M][X32M_DIG_64M_RDY] != NULL)
    {
      /* call the callback function */
      intCbfArra[INT_XTAL32M][X32M_DIG_64M_RDY]();
    }
    else
    {
      X32M->IMR.BF.DIG_64M_RDY_INT_MSK = 1;
    }
  }
}

/****************************************************************************/ /**
                                                                                * @brief      Set RC32K calibration clock divider
                                                                                *
                                                                                * @param[in]  divider1:  Specifies the interrupt type
                                                                                * @param[in]  divider2:  Specifies the interrupt type
                                                                                *
                                                                                * @return     Interrupt status value of the specified interrupt type
                                                                                *
                                                                                *******************************************************************************/
void RC32K_CalClk_Div(uint32_t divider1, uint32_t divider2)
{
  PMU->RC32K_CAL_CLK_SRC_DIV.BF.RC32K_CAL_CLK_DIV1 = (divider1 & 0x3F);
  PMU->RC32K_CAL_CLK_SRC_DIV.BF.RC32K_CAL_CLK_DIV2 = (divider2 & 0x3F);
}

/****************************************************************************/ /**
                                                                                * @brief      RC32M clock output frequency select function
                                                                                *
                                                                                * @param[in]  freqType: Output frequency option, 16 MHz or 32 MHz
                                                                                *
                                                                                * @return     none
                                                                                *
                                                                                *******************************************************************************/
void RC32M_SelOutput32MHz(RC32M_SELFREQ_Type freqType)
{
  switch (freqType)
  {
  case RC32M_SELFREQ_16M:
    RC32M->CLK.BF.SEL_32M = 0;
    break;
  case RC32M_SELFREQ_32M:
    RC32M->CLK.BF.SEL_32M = 1;
    break;
  default:
    break;
  }
}

/****************************************************************************/ /**
                                                                                * @brief     Get the module status
                                                                                *
                                                                                * @param[in]  clkModule    :  Clock module need for clock status
                                                                                *
                                                                                * @return module clock status
                                                                                *******************************************************************************/

FunctionalState CLK_GetModuleStatus(CLK_Module_Type clkModule)
{
  FunctionalState clkStatus;

  switch (clkModule)
  {
  case CLK_OUT:
    clkStatus = (PMU->CLK_ENABLE.BF.DISABLE_OUT_CLK) ? (DISABLE) : (ENABLE);
    break;

  case CLK_RC32K_CAL:
    clkStatus = (PMU->CLK_ENABLE.BF.DISABLE_RC32K_CAL) ? (DISABLE) : (ENABLE);
    break;

  case CLK_SYSCTRL:
    clkStatus = (PMU->CLK_ENABLE.BF.DISABLE_SYSCTRL) ? (DISABLE) : (ENABLE);
    break;

  case CLK_CAU1:
    clkStatus = (PMU->CLK_ENABLE.BF.DISABLE_CAU1) ? (DISABLE) : (ENABLE);
    break;

  case CLK_SSP2:
    clkStatus = (PMU->CLK_ENABLE.BF.DISABLE_SPI2) ? (DISABLE) : (ENABLE);
    break;

  case CLK_SSP1:
    clkStatus = (PMU->CLK_ENABLE.BF.DISABLE_SPI1) ? (DISABLE) : (ENABLE);
    break;

  case CLK_QSPI:
    clkStatus = (PMU->CLK_ENABLE.BF.DISABLE_QSPI) ? (DISABLE) : (ENABLE);
    break;

  case CLK_GPIO:
    clkStatus = (PMU->CLK_ENABLE.BF.DISABLE_GPIO) ? (DISABLE) : (ENABLE);
    break;

  case CLK_GPT2:
    clkStatus = (PMU->CLK_ENABLE.BF.DISABLE_GPT2) ? (DISABLE) : (ENABLE);
    break;

  case CLK_GPT1:
    clkStatus = (PMU->CLK_ENABLE.BF.DISABLE_GPT1) ? (DISABLE) : (ENABLE);
    break;

  case CLK_PINMUX:
    clkStatus = (PMU->CLK_ENABLE.BF.DISABLE_PINMUX) ? (DISABLE) : (ENABLE);
    break;

  case CLK_RTC:
    clkStatus = (PMU->CLK_ENABLE.BF.DISABLE_RTC) ? (DISABLE) : (ENABLE);
    break;

  case CLK_CAU0:
    clkStatus = (PMU->CLK_ENABLE.BF.DISABLE_CAU0) ? (DISABLE) : (ENABLE);
    break;

  case CLK_WDT:
    clkStatus = (PMU->CLK_ENABLE.BF.DISABLE_WDT) ? (DISABLE) : (ENABLE);
    break;

  case CLK_UART2:
    clkStatus = (PMU->CLK_ENABLE.BF.DISABLE_UART2) ? (DISABLE) : (ENABLE);
    break;

  case CLK_UART1:
    clkStatus = (PMU->CLK_ENABLE.BF.DISABLE_UART1) ? (DISABLE) : (ENABLE);
    break;

  case CLK_I2C2:
    clkStatus = (PMU->CLK_ENABLE.BF.DISABLE_I2C2) ? (DISABLE) : (ENABLE);
    break;

  case CLK_I2C1:
    clkStatus = (PMU->CLK_ENABLE.BF.DISABLE_I2C1) ? (DISABLE) : (ENABLE);
    break;

  case CLK_APB0:
    clkStatus = (PMU->CLK_ENABLE.BF.DISABLE_APB0) ? (DISABLE) : (ENABLE);
    break;

  case CLK_AES_CRC:
    clkStatus = (PMU->CLK_ENABLE.BF.DISABLE_AES) ? (DISABLE) : (ENABLE);
    break;

  case CLK_PHY:
    clkStatus = (PMU->CLK_ENABLE.BF.DISABLE_PHY) ? (DISABLE) : (ENABLE);
    break;

  case CLK_DMA:
    clkStatus = (PMU->CLK_ENABLE.BF.DISABLE_DMA) ? (DISABLE) : (ENABLE);
    break;

  case CLK_COMP:
    clkStatus = (PMU->CLK_ENABLE.BF.DISABLE_COMP) ? (DISABLE) : (ENABLE);
    break;

  case CLK_DAC:
    clkStatus = (PMU->CLK_ENABLE.BF.DISABLE_DAC) ? (DISABLE) : (ENABLE);
    break;

  case CLK_ADC:
    clkStatus = (PMU->CLK_ENABLE.BF.DISABLE_ADC) ? (DISABLE) : (ENABLE);
    break;

  case CLK_KEYSCAN:
    clkStatus = (PMU->CLK_ENABLE.BF.DISABLE_KEYSCAN) ? (DISABLE) : (ENABLE);
    break;

  case CLK_TRACKBALL:
    clkStatus = (PMU->CLK_ENABLE.BF.DISABLE_TRACKBALL) ? (DISABLE) : (ENABLE);
    break;

  case CLK_IR:
    clkStatus = (PMU->CLK_ENABLE.BF.DISABLE_IR) ? (DISABLE) : (ENABLE);
    break;

  case CLK_3DG:
    clkStatus = (PMU->CLK_ENABLE.BF.DISABLE_3DG) ? (DISABLE) : (ENABLE);
    break;

  case CLK_XTAL32K_CLK:
    clkStatus = (PMU->CLK_ENABLE.BF.DISABLE_XTAL_CLK_32K) ? (DISABLE) : (ENABLE);
    break;

  case CLK_I2S:
    break;
  }

  return clkStatus;
}
