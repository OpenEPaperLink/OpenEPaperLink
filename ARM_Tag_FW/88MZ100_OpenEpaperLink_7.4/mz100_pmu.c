#include "mz100_pmu.h"

/** @defgroup PMU_Public_Functions
 *  @{
 */
/****************************************************************************//**
 * @brief     Set system will be in sleep mode
 *
 * @param[in]  pmuMode:  power mode option
 * @param[in]  cauStatus:  cau option in PM2 mode
 *
 * @return none
 *
 *******************************************************************************/
void PMU_SetSleepMode(PMU_SleepMode_Type pmuMode)
{
  /* set PMU basic mode */
  PMU->PWR_MODE.BF.PWR_MODE = pmuMode;

  /* select deepsleep or not */
  if(pmuMode == PMU_PM1)
  {
    SCB->SCR &= !SCB_SCR_SLEEPDEEP_Msk;
  }
  else
  {
    SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
  }
}

/****************************************************************************//**
 * @brief     Disable CAU power in PM2 mode
 *
 * @param none
 *
 * @return none
 *
 *******************************************************************************/
void PMU_DisableCAUInPM2(void)
{
  PMU->PWR_MODE.BF.CAU_ON = 0;
}

/****************************************************************************//**
 * @brief     Enable CAU power in PM2 mode
 *
 * @param none
 *
 * @return none
 *
 *******************************************************************************/
void PMU_EnableCAUInPM2(void)
{
  PMU->PWR_MODE.BF.CAU_ON = 1;
}

/****************************************************************************//**
 * @brief     wake up pin trigger type set
 *
 * @param[in]  wakeupPin: wake up source
 * @param[in]  wakeupPin: trigger mode selector
 *
 * @return none
 *
 *******************************************************************************/
void PMU_ConfigWakeupPin(PMU_WakeupPinSrc_Type wakeupPin, PMU_WakeupTrigMode_Type trigMode)
{
  switch(wakeupPin)
  {
    /* case GPIO1 interrupt */
    case PMU_GPIO1_INT:
      if(trigMode == PMU_WAKEUP_DISABLE)
      {
        PMU->EXT_TRIG_CFG.BF.PIN0_TRIGGER = 0;
      }
      else if(trigMode == PMU_WAKEUP_EDGE_RISING)
      {
        PMU->EXT_TRIG_CFG.BF.PIN0_TRIGGER = 1;
      }
      else if(trigMode == PMU_WAKEUP_EDGE_FALLING)
      {
        PMU->EXT_TRIG_CFG.BF.PIN0_TRIGGER = 2;
      }
      break;
      
      /* case GPIO2 interrupt */
    case PMU_GPIO2_INT:
      if(trigMode == PMU_WAKEUP_DISABLE)
      {
        PMU->EXT_TRIG_CFG.BF.PIN1_TRIGGER = 0;
      }
      else if(trigMode == PMU_WAKEUP_EDGE_RISING)
      {
        PMU->EXT_TRIG_CFG.BF.PIN1_TRIGGER = 1;
      }
      else if(trigMode == PMU_WAKEUP_EDGE_FALLING)
      {
        PMU->EXT_TRIG_CFG.BF.PIN1_TRIGGER = 2;
      }
      break;
      
      /* case GPIO3 interrupt */
    case PMU_GPIO3_INT:
      if(trigMode == PMU_WAKEUP_DISABLE)
      {
        PMU->EXT_TRIG_CFG.BF.PIN2_TRIGGER = 0;
      }
      else if(trigMode == PMU_WAKEUP_EDGE_RISING)
      {
        PMU->EXT_TRIG_CFG.BF.PIN2_TRIGGER = 1;
      }
      else if(trigMode == PMU_WAKEUP_EDGE_FALLING)
      {
        PMU->EXT_TRIG_CFG.BF.PIN2_TRIGGER = 2;
      }
      break;
      
      /* case GPIO4 interrupt */
    case PMU_GPIO4_INT:
      if(trigMode == PMU_WAKEUP_DISABLE)
      {
        PMU->EXT_TRIG_CFG.BF.PIN3_TRIGGER = 0;
      }
      else if(trigMode == PMU_WAKEUP_EDGE_RISING)
      {
        PMU->EXT_TRIG_CFG.BF.PIN3_TRIGGER = 1;
      }
      else if(trigMode == PMU_WAKEUP_EDGE_FALLING)
      {
        PMU->EXT_TRIG_CFG.BF.PIN3_TRIGGER = 2;
      }
      break;
      
      /* case GPIO5 interrupt */
    case PMU_GPIO5_INT:
      if(trigMode == PMU_WAKEUP_DISABLE)
      {
        PMU->EXT_TRIG_CFG.BF.PIN4_TRIGGER = 0;
      }
      else if(trigMode == PMU_WAKEUP_EDGE_RISING)
      {
        PMU->EXT_TRIG_CFG.BF.PIN4_TRIGGER = 1;
      }
      else if(trigMode == PMU_WAKEUP_EDGE_FALLING)
      {
        PMU->EXT_TRIG_CFG.BF.PIN4_TRIGGER = 2;
      }
      break;
      
    /* case GPIO6 interrupt */
    case PMU_GPIO6_INT:
      if(trigMode == PMU_WAKEUP_DISABLE)
      {
        PMU->EXT_TRIG_CFG.BF.PIN5_TRIGGER = 0;
      }
      else if(trigMode == PMU_WAKEUP_EDGE_RISING)
      {
        PMU->EXT_TRIG_CFG.BF.PIN5_TRIGGER = 1;
      }
      else if(trigMode == PMU_WAKEUP_EDGE_FALLING)
      {
        PMU->EXT_TRIG_CFG.BF.PIN5_TRIGGER = 2;
      }
      break;

    /* case GPIO7 interrupt */
    case PMU_GPIO7_INT:
      if(trigMode == PMU_WAKEUP_DISABLE)
      {
        PMU->EXT_TRIG_CFG.BF.PIN6_TRIGGER = 0;
      }
      else if(trigMode == PMU_WAKEUP_EDGE_RISING)
      {
        PMU->EXT_TRIG_CFG.BF.PIN6_TRIGGER = 1;
      }
      else if(trigMode == PMU_WAKEUP_EDGE_FALLING)
      {
        PMU->EXT_TRIG_CFG.BF.PIN6_TRIGGER = 2;
      }
      break;
      
      /* case GPIO26 interrupt */
    case PMU_GPIO26_INT:
      if(trigMode == PMU_WAKEUP_DISABLE)
      {
        PMU->EXT_TRIG_CFG.BF.PIN7_TRIGGER = 0;
      }
      else if(trigMode == PMU_WAKEUP_EDGE_RISING)
      {
        PMU->EXT_TRIG_CFG.BF.PIN7_TRIGGER = 1;
      }
      else if(trigMode == PMU_WAKEUP_EDGE_FALLING)
      {
        PMU->EXT_TRIG_CFG.BF.PIN7_TRIGGER = 2;
      }
      break;
      
      /* case GPIO27 interrupt */
    case PMU_GPIO27_INT:
      if(trigMode == PMU_WAKEUP_DISABLE)
      {
        PMU->EXT_TRIG_CFG.BF.PIN8_TRIGGER = 0;
      }
      else if(trigMode == PMU_WAKEUP_EDGE_RISING)
      {
        PMU->EXT_TRIG_CFG.BF.PIN8_TRIGGER = 1;
      }
      else if(trigMode == PMU_WAKEUP_EDGE_FALLING)
      {
        PMU->EXT_TRIG_CFG.BF.PIN8_TRIGGER = 2;
      }
      break;
      
      /* case GPIO28 interrupt */
    case PMU_GPIO28_INT:
      if(trigMode == PMU_WAKEUP_DISABLE)
      {
        PMU->EXT_TRIG_CFG.BF.PIN9_TRIGGER = 0;
      }
      else if(trigMode == PMU_WAKEUP_EDGE_RISING)
      {
        PMU->EXT_TRIG_CFG.BF.PIN9_TRIGGER = 1;
      }
      else if(trigMode == PMU_WAKEUP_EDGE_FALLING)
      {
        PMU->EXT_TRIG_CFG.BF.PIN9_TRIGGER = 2;
      }
      break;
      
      /* case GPIO29 interrupt */
    case PMU_GPIO29_INT:
      if(trigMode == PMU_WAKEUP_DISABLE)
      {
        PMU->EXT_TRIG_CFG.BF.PIN10_TRIGGER = 0;
      }
      else if(trigMode == PMU_WAKEUP_EDGE_RISING)
      {
        PMU->EXT_TRIG_CFG.BF.PIN10_TRIGGER = 1;
      }
      else if(trigMode == PMU_WAKEUP_EDGE_FALLING)
      {
        PMU->EXT_TRIG_CFG.BF.PIN10_TRIGGER = 2;
      }
      break;
      
      /* case GPIO30 interrupt */
    case PMU_GPIO30_INT:
      if(trigMode == PMU_WAKEUP_DISABLE)
      {
        PMU->EXT_TRIG_CFG.BF.PIN11_TRIGGER = 0;
      }
      else if(trigMode == PMU_WAKEUP_EDGE_RISING)
      {
        PMU->EXT_TRIG_CFG.BF.PIN11_TRIGGER = 1;
      }
      else if(trigMode == PMU_WAKEUP_EDGE_FALLING)
      {
        PMU->EXT_TRIG_CFG.BF.PIN11_TRIGGER = 2;
      }
      break;
      
      /* case GPIO31 interrupt */
    case PMU_GPIO31_INT:
      if(trigMode == PMU_WAKEUP_DISABLE)
      {
        PMU->EXT_TRIG_CFG.BF.PIN12_TRIGGER = 0;
      }
      else if(trigMode == PMU_WAKEUP_EDGE_RISING)
      {
        PMU->EXT_TRIG_CFG.BF.PIN12_TRIGGER = 1;
      }
      else if(trigMode == PMU_WAKEUP_EDGE_FALLING)
      {
        PMU->EXT_TRIG_CFG.BF.PIN12_TRIGGER = 2;
      }
      break;

    /* default */
    default:
      break;
  }
}

/****************************************************************************//**
 * @brief     clear pending wakeup external pin interrupt
 *
 * @param[in]  wakeupPin: wakeup external pin to be cleared
 *
 * @return none
 *
 *******************************************************************************/
void PMU_ClearWakeupExtpin(PMU_WakeupPinSrc_Type wakeupPin)
{
  switch(wakeupPin)
  {
    /* case GPIO1 interrupt */
    case PMU_GPIO1_INT:
      PMU->EXT_INT_CLR.BF.PIN0_CLR = 1;
      break;
      
      /* case GPIO2 interrupt */
    case PMU_GPIO2_INT:
      PMU->EXT_INT_CLR.BF.PIN1_CLR = 1;
      break;
      
      /* case GPIO3 interrupt */
    case PMU_GPIO3_INT:
      PMU->EXT_INT_CLR.BF.PIN2_CLR = 1;
      break;
      
      /* case GPIO4 interrupt */
    case PMU_GPIO4_INT:
      PMU->EXT_INT_CLR.BF.PIN3_CLR = 1;
      break;
      
      /* case GPIO5 interrupt */
    case PMU_GPIO5_INT:
      PMU->EXT_INT_CLR.BF.PIN4_CLR = 1;
      break;
      
    /* case GPIO6 interrupt */
    case PMU_GPIO6_INT:
      PMU->EXT_INT_CLR.BF.PIN5_CLR = 1;
      break;

    /* case GPIO7 interrupt */
    case PMU_GPIO7_INT:
      PMU->EXT_INT_CLR.BF.PIN6_CLR = 1;
      break;
      
      /* case GPIO26 interrupt */
    case PMU_GPIO26_INT:
      PMU->EXT_INT_CLR.BF.PIN7_CLR = 1;
      break;
      
      /* case GPIO27 interrupt */
    case PMU_GPIO27_INT:
      PMU->EXT_INT_CLR.BF.PIN8_CLR = 1;
      break;
      
      /* case GPIO28 interrupt */
    case PMU_GPIO28_INT:
      PMU->EXT_INT_CLR.BF.PIN9_CLR = 1;
      break;
      
      /* case GPIO29 interrupt */
    case PMU_GPIO29_INT:
      PMU->EXT_INT_CLR.BF.PIN10_CLR = 1;
      break;
      
      /* case GPIO30 interrupt */
    case PMU_GPIO30_INT:
      PMU->EXT_INT_CLR.BF.PIN11_CLR = 1;
      break;
      
      /* case GPIO31 interrupt */
    case PMU_GPIO31_INT:
      PMU->EXT_INT_CLR.BF.PIN12_CLR = 1;
      break;
      
    /* default */
    default:
      break;
  }
}

/****************************************************************************//**
 * @brief     clear pending keyscan wakeup interrupt
 *
 * @param[in]  wakeupPin: wakeup external pin to be cleared
 *
 * @return none
 *
 *******************************************************************************/
void PMU_ClearWakeupKeyscan(void)
{
  PMU->EXT_INT_CLR.BF.KEYSCAN_CLR = 1;
}

/****************************************************************************//**
 * @brief      Get last reset cause
 *
 * @param  none
 *
 * @return     Return last reset cause
 *
 *******************************************************************************/
uint32_t PMU_GetLastResetCause(void)
{
  return PMU->LAST_RESET.WORDVAL;
}
