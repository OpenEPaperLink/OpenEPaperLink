#include "mz100.h"
#include "mz100_rtc.h"
#include "mz100_driver.h"

/** @addtogroup  88MZ100_Periph_Driver
 *  @{
 */

/** @defgroup RTC RTC
 *  @brief RTC driver modules
 *  @{
 */

/** @defgroup RTC_Private_Type
 *  @{
 */

/*@} end of group RTC_Private_Type*/

/** @defgroup RTC_Private_Defines
 *  @{
 */

/*@} end of group RTC_Private_Defines */

/** @defgroup RTC_Private_Variables
 *  @{
 */

/*@} end of group RTC_Private_Variables */

/** @defgroup RTC_Global_Variables
 *  @{
 */

/*@} end of group RTC_Global_Variables */

/** @defgroup RTC_Private_FunctionDeclaration
 *  @{
 */

/*@} end of group RTC_Private_FunctionDeclaration */

/** @defgroup RTC_Private_Functions
 *  @{
 */

/*@} end of group RTC_Private_Functions */

/** @defgroup RTC_Public_Functions
 *  @{
 */

 
/****************************************************************************//**
 * @brief      Reset RTC counter 
 *
 * @param[in]  None
 *
 * @return reset status
 *  
 * Reset the RTC counter
 *******************************************************************************/
Status RTC_CounterReset(void)
{  
  volatile uint32_t cnt = 0;
  
  /* Reset the RTC counter */
  RTC->CNT_EN_REG.BF.CNT_RESET = 1;  
  
  /* Wating until the counter reset is done */
  while(cnt < 0x200000)
  {
    /* Read the counter reset status */
    if(RTC->CNT_EN_REG.BF.CNT_RST_DONE)
    {
      return SUCCESS;
    }
    
    cnt++;
  }
  
  return ERROR;
}

/****************************************************************************//**
 * @brief      Initialize the RTC 
 *
 * @param[in]  rtcConfig:  Pointer to a RTC configuration structure
 *
 * @return none
 *
 * Initialize the RTC 
 *******************************************************************************/
void RTC_Init(RTC_Config_Type * rtcConfig)
{
  /* set counter value register update mode */
  RTC->CNT_CNTL_REG.BF.CNT_UPDT_MOD = rtcConfig->CntValUpdateMode;
  
  /* counter clock divider not available, default 0 */
  RTC->CLK_CNTL_REG.BF.CLK_DIV = 0;
  
  /* set counter overflow value */
  RTC->CNT_UPP_VAL_REG.BF.UPP_VAL = rtcConfig->uppVal;    
}

/****************************************************************************//**
 * @brief      Start the RTC counter
 *
 * @param[in]  None
 *
 * @return none
 *              
 * Start the RTC counter
 *******************************************************************************/
void RTC_Start(void)
{
  /* start RTC counter */
  RTC->CNT_EN_REG.BF.CNT_START = 1;
}

/****************************************************************************//**
 * @brief      Stop the RTC counter
 *
 * @param[in]  None
 *
 * @return none
 *
 * Stop the RTC counter
 *******************************************************************************/
void RTC_Stop(void)
{
  /* stop RTC counter */
  RTC->CNT_EN_REG.BF.CNT_STOP = 1;
}

/****************************************************************************//**
 * @brief      Set RTC counter overflow value 
 *
 * @param[in]  uppVal: Counter overflow value.The range is 0-0xffffffff.
 *             If it is greater than the maximum, the high bits will be neglected.
 *
 * @return none
 *
 *******************************************************************************/
void RTC_SetCounterUppVal(uint32_t uppVal)
{
  /* set counter overflow value */
  RTC->CNT_UPP_VAL_REG.BF.UPP_VAL = uppVal;
}

/****************************************************************************//**
 * @brief      Set RTC channel count-up match value 
 *
 *@param[in]   channelID: RTC channel number.
 *@param[in]   cupVal: Channel count-up match value.The range is 0-0xffffffff.
 *             If it is greater than the maximum, the high bits will be neglected.
 *
 * @return none
 *
 *******************************************************************************/
void RTC_SetChannelCupVal(RTC_ChannelNumber_Type channelID, uint32_t cupVal)
{
  if(channelID == RTC_CH_0)
  {
    RTC->CH0_CUP_VAL_REG.BF.CUP_VAL = cupVal;
  }
  else if(channelID == RTC_CH_1)
  {
    RTC->CH1_CUP_VAL_REG.BF.CUP_VAL = cupVal;
  }
  
  /* update them to internal shadow registers */
  RTC->USER_REQ_REG.WORDVAL |= (0x010000 << channelID); 
}

/****************************************************************************//**
 * @brief      Get RTC counter value in any counter update mode
 *
 * @param[in]  None
 *
 * @return counter value
 *
 *******************************************************************************/
uint32_t RTC_GetCounterVal(void)
{
  uint32_t cntVal;

  /* read counter value register */
  cntVal = RTC->CNT_VAL_REG.BF.CNT_VAL;
  
  return cntVal;
}

/****************************************************************************//**
 * @brief     MASK / UNMASK  RTC interrupt 
 *
 * @param[in]  intType:  Interrupt type
 * @param[in]  intMsk:   MASK / UNMASK control
 *
 * @return none
 *
 *******************************************************************************/
void RTC_IntMask(RTC_INT_Type intType, IntMask_Type intMsk)
{
  CHECK_PARAM(IS_INTMASK(intMsk));
  CHECK_PARAM(IS_RTC_INT_TYPE(intType));
  
  switch(intType)
  {
    case RTC_INT_CNT_UPP:
      RTC->INT_MSK_REG.BF.CNT_UPP_MSK = intMsk;
      break;
    case RTC_INT_CH0_CUP:
      RTC->INT_MSK_REG.BF.CH0_OC_MSK = intMsk;
      break;
    case RTC_INT_CH1_CUP:
      RTC->INT_MSK_REG.BF.CH1_OC_MSK = intMsk;
      break;
    case RTC_INT_ALL:
      if(intMsk == UNMASK)
      {
        RTC->INT_MSK_REG.WORDVAL = 0;
      }
      else
      {
        RTC->INT_MSK_REG.WORDVAL |= 0x00010003; 
      }
      break;
      
    default:
      break;
  }
}

/****************************************************************************//**
 * @brief      Clear RTC interrupt flag
 *
 * @param[in]  intType:  Interrupt type
 *
 * @return none
 *
 *******************************************************************************/
void RTC_IntClr(RTC_INT_Type intType)
{
  CHECK_PARAM(IS_RTC_INT_TYPE(intType));
  
  switch(intType)
  {
    case RTC_INT_CNT_UPP:
      RTC->INT_RAW_REG.BF.CNT_UPP_INT = 1;
      break;
    case RTC_INT_CH0_CUP:
      RTC->INT_RAW_REG.BF.CH0_OC_INT = 1;
      break;
    case RTC_INT_CH1_CUP:
      RTC->INT_RAW_REG.BF.CH1_OC_INT = 1;
      break;
    case RTC_INT_ALL:
      
      RTC->INT_RAW_REG.WORDVAL |= 0x00010003; 
      
      break;
      
    default:
      break;
  }
  RTC->INT_RAW_REG.WORDVAL = 0x010000;
}

/****************************************************************************//**
 * @brief      Get RTC  status
 *
 * @param[in] none
 *
 * @return status
 *
 *******************************************************************************/
FlagStatus RTC_GetStatus(void)
{
  uint32_t retStatus = 0;
  
  retStatus = RTC->INT_RAW_REG.BF.CNT_UPP_INT;
 
  if(retStatus)
  {
    return SET;
  }
  else
  {
    return RESET;
  }
}

/****************************************************************************//**
 * @brief  Timer interrupt handler 
 *
 * @param[in]  intPeriph: Select the peripheral
 *
 * @return none
 *
 *******************************************************************************/
__attribute__((weak)) void RTC_IRQHandler(void)
{
  uint32_t intStatus = RTC->INT_REG.WORDVAL;
////  uint32_t cupIntStatus = intStatus & 0x03; 
  uint8_t cnt = 0;
  
  /* clear the generated interrupts */
  RTC->INT_RAW_REG.WORDVAL = intStatus;
  
  /* channel cup interrupt */
  if((intStatus & (RTC_INT_CH_CUP_ALL_MSK))!= 0x00)
  {
    /* scan the channels */
    for(cnt = 0; cnt < RTC_CHANNEL_NUMBER; cnt++)
    {
      /* check if interrupt available for the specific channel */
      if(intStatus & (0x01<<cnt))
      {
        if(intCbfArra[INT_RTC][cnt] != NULL)
        {      
          /* call the callback function */
          intCbfArra[INT_RTC][cnt]();
        }
        /* Disable the interrupt if callback function is not setup */
        else
        {
          RTC->INT_MSK_REG.WORDVAL |= (1<<cnt) ;
        }      
      }    
    }
  }
  
  /* counter upp interrupt */
  if((intStatus & (RTC_INT_CNT_UPP_MSK))!= 0x00)
  {
    if(intCbfArra[INT_RTC][RTC_INT_CNT_UPP] != NULL)
    {
      /* call the callback function */
      intCbfArra[INT_RTC][RTC_INT_CNT_UPP]();
    }
    /* Disable the interrupt if callback function is not setup */
    else
    {
      RTC->INT_MSK_REG.BF.CNT_UPP_MSK = 1;
    }
  }
}
