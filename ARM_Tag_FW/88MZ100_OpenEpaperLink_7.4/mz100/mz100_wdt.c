#include "mz100.h"
#include "mz100_wdt.h"
#include "mz100_driver.h"

/** @addtogroup  88MZ100_Periph_Driver
 *  @{
 */

/** @defgroup WDT WDT
 *  @brief WDT driver modules
 *  @{
 */

/** @defgroup WDT_Private_Type
 *  @{
 */

/*@} end of group WDT_Private_Type*/

/** @defgroup WDT_Private_Defines
 *  @{
 */

/**
 *  @brief WDT feed pattern 
 */
#define WDT_FEED_PATTERN            0x76 

/*@} end of group WDT_Private_Defines */

/** @defgroup WDT_Private_Variables
 *  @{
 */

/*@} end of group WDT_Private_Variables */

/** @defgroup WDT_Global_Variables
 *  @{
 */

/*@} end of group WDT_Global_Variables */

/** @defgroup WDT_Private_FunctionDeclaration
 *  @{
 */

/*@} end of group WDT_Private_FunctionDeclaration */

/** @defgroup WDT_Private_Functions
 *  @{
 */

/*@} end of group WDT_Private_Functions */

/** @defgroup WDT_Public_Functions
 *  @{
 */

/****************************************************************************//**
 * @brief      Initialize the WDT 
 *
 * @param[in]  WDTConfig:  Pointer to a WDT configuration structure
 *
 * @return none
 *
 *******************************************************************************/
void WDT_Init(WDT_Config_Type* wdtConfig)
{
  /* set watchdog tomeout value */
  WDT->TORR.BF.TOP = wdtConfig->timeoutVal;

  /* set watchdog mode */
  WDT->CR.BF.RMOD = wdtConfig->mode;

  /* set watchdog reset pulse width */
  WDT->CR.BF.RPL = wdtConfig->resetPulseLen;
}

/****************************************************************************//**
 * @brief      Enable WDT counter 
 *
 * @param  none
 *
 * @return none
 *
 *******************************************************************************/
void WDT_Enable(void)
{
  /* enable WDT counter */
  WDT->CR.BF.EN = 1;
}

/****************************************************************************//**
 * @brief      Disable WDT counter 
 *
 * @param  none
 *
 * @return none
 *
 *******************************************************************************/
void WDT_Disable(void)
{
  /* disable WDT counter */
  WDT->CR.BF.EN = 0;
}

/****************************************************************************//**
 * @brief      Set WDT response mode 
 *
 * @param[in]  mode:  WDT response mode
 *
 * @return none
 *
 *******************************************************************************/
void WDT_SetMode(WDT_Mode_Type mode)
{
  /* set WDT response mode */
  WDT->CR.BF.RMOD = mode;
}

/****************************************************************************//**
 * @brief      Set WDT timeout value 
 *
 * @param[in]  timeoutVal:  counter timeout value
 *
 * @return none
 *
 *******************************************************************************/
void WDT_SetTimeoutVal(uint32_t timeoutVal)
{
  /* set WDT timeout value */
  // This value can be anything between 0x00 and 0x0F; this configures the WDT to reset at 2^(16+value) cycles (2^16 - 2^31)
  WDT->TORR.BF.TOP = timeoutVal;
}
 
/****************************************************************************//**
 * @brief      Set WDT reset pulse lenghth value 
 *
 * @param[in]  resetPulseLen:  WDT reset pulse lenghth
 *
 * @return none
 *
 *******************************************************************************/
void WDT_SetResetPulseLen(WDT_ResetPulseLen_Type resetPulseLen)
{
  /* set WDT reset pulse length value */
  WDT->CR.BF.RPL = resetPulseLen;
}

/****************************************************************************//**
 * @brief      Get WDT current counter value 
 *
 * @param  none
 *
 * @return WDT current counter value
 *
 *******************************************************************************/
uint32_t WDT_GetCounterVal(void)
{
  return WDT->CCVR.WORDVAL;
}

/****************************************************************************//**
 * @brief      restart WDT counter value
 *
 * @param  none
 *
 * @return none
 *
 *******************************************************************************/
void WDT_RestartCounter(void)
{
  /* disable irq interrupt */
  __disable_irq();
  /* feed the dog with fixed pattern */
  WDT->CRR.BF.CRR = WDT_FEED_PATTERN;
  /* then enable irq interrupt */ 
  __enable_irq();
}
    
/****************************************************************************//**
 * @brief      Clear WDT interrupt flag
 *
 * @param  none
 *
 * @return none
 *
 *******************************************************************************/
void WDT_IntClr(void)
{
  volatile uint32_t tmpData;
  /* clear interrupt flag */
  tmpData = WDT->EOI.WORDVAL;
}

/****************************************************************************//**
 * @brief      Get WDT interrupt status
 *
 * @param  none
 *
 * @return interrupt status
 *
 *******************************************************************************/
IntStatus WDT_GetIntStatus(void)
{
  /* get interrupt status */
  return (IntStatus)WDT->STAT.BF.STAT;
}
