#include "mz100.h"
#include "mz100_gpt.h"
#include "mz100_driver.h"

/** @addtogroup  88MZ100_Periph_Driver
 *  @{
 */

/** @defgroup GPT GPT
 *  @brief GPT driver modules
 *  @{
 */

/** @defgroup GPT_Private_Type
 *  @{
 */

/*@} end of group GPT_Private_Type*/

/** @defgroup GPT_Private_Defines
 *  @{
 */

/*@} end of group GPT_Private_Defines */

/** @defgroup GPT_Private_Variables
 *  @{
 */
 
/**  
 *  @brief GPT1 and GPT2 address array 
 */
const static uint32_t gptAddr[2] = {GPT1_BASE, GPT2_BASE};

/*@} end of group GPT_Private_Variables */

/** @defgroup GPT_Global_Variables
 *  @{
 */

/*@} end of group GPT_Global_Variables */

/** @defgroup GPT_Private_FunctionDeclaration
 *  @{
 */

/*@} end of group GPT_Private_FunctionDeclaration */

/** @defgroup GPT_Private_Functions
 *  @{
 */

/*@} end of group GPT_Private_Functions */

/** @defgroup GPT_Public_Functions
 *  @{
 */

/****************************************************************************//**
 * @brief      Reset GPT counter 
 *
 * @param[in]  gptID:  Select the GPT module
 *
 * @return reset status
 *  
 * Reset the GPT counter
 *******************************************************************************/
Status GPT_CounterReset(GPT_ID_Type gptID)
{
  gpt_reg_t * GPTx = (gpt_reg_t *)(gptAddr[gptID]);
  volatile uint32_t cnt = 0;
  
  CHECK_PARAM(IS_GPT_PERIPH(gptID));
  
  /* Reset the GPT counter */
  GPTx->CNT_EN_REG.BF.CNT_RESET = 0x1;
  
  /* Wating until the counter reset is done */
  while(cnt < 0x200000)
  {
    /* Read the counter reset status */
    if(GPTx->CNT_EN_REG.BF.CNT_RST_DONE)
    {
      return SUCCESS;
    }
    
    cnt++;
  }
  
  return ERROR;
}

/****************************************************************************//**
 * @brief      Initialize the GPT 
 *
 * @param[in]  gptID:  Select the GPT module
 * @param[in]  gptConfig:  Pointer to a GPT configuration structure
 *
 * @return none
 *
 * Initialize the GPT 
 *******************************************************************************/
void GPT_Init(GPT_ID_Type gptID, GPT_Config_Type* gptConfig)
{
  gpt_reg_t * GPTx = (gpt_reg_t *)(gptAddr[gptID]);

  CHECK_PARAM(IS_GPT_PERIPH(gptID));

  /**************************Configure the clock*******************************/
  
  /* select clock source */
  GPTx->CLK_CNTL_REG.BF.CLK_SRC = gptConfig->clockSrc;
  
  /* set clock prescaler */
  GPTx->CLK_CNTL_REG.BF.CLK_PRE = gptConfig->clockPrescaler;

  /* set clock divider */
  GPTx->CLK_CNTL_REG.BF.CLK_DIV = gptConfig->clockDivider;

  /************************Configure the counter *****************************/

  /* set counter mode */
  GPTx->CNT_CNTL_REG.BF.CNT_DIR = gptConfig->counterMode;
  
  /* set counter value register update mode */
  GPTx->CNT_CNTL_REG.BF.CNT_UPDT_MOD = gptConfig->cntUpdate;

  /* set upp value */
  GPTx->CNT_UPP_VAL_REG.BF.UPP_VAL = gptConfig->uppVal;
}

/****************************************************************************//**
 * @brief      Start the GPT counter
 *
 * @param[in]  gptID:  Select the GPT module
 *
 * @return none
 *
 * Start the GPT 
 *******************************************************************************/
void GPT_Start(GPT_ID_Type gptID)
{
  gpt_reg_t * GPTx = (gpt_reg_t *)(gptAddr[gptID]);

  CHECK_PARAM(IS_GPT_PERIPH(gptID));

  /* start GPT counter */
  GPTx->CNT_EN_REG.BF.CNT_START = 1;
}

/****************************************************************************//**
 * @brief      Stop the GPT counter
 *
 * @param[in]  gptID:  Select the GPT module
 *
 * @return none
 *
 * Stop the GPT 
 *******************************************************************************/
void GPT_Stop(GPT_ID_Type gptID)
{
  gpt_reg_t * GPTx = (gpt_reg_t *)(gptAddr[gptID]);

  CHECK_PARAM(IS_GPT_PERIPH(gptID));
  
  /* stop GPT counter */
  GPTx->CNT_EN_REG.BF.CNT_STOP = 1;
}

/****************************************************************************//**
 * @brief      Select the channel function
 *
 * @param[in]  gptID:  Select the GPT module
 * @param[in]  channelID:  Channel ID
 * @param[in]  function : select input/output function
 *
 * @return none
 *
 *******************************************************************************/
void GPT_ChannelFuncSelect(GPT_ID_Type gptID, GPT_ChannelNumber_Type channelID, 
                           GPT_ChannelFunc_Type function)
{
  gpt_reg_t * GPTx = (gpt_reg_t *)(gptAddr[gptID]);

  CHECK_PARAM(IS_GPT_PERIPH(gptID));  
  
  /* select channel function */
  GPTx->CH[channelID].CHX_CNTL_REG.BF.CHX_IO = function;
}

/****************************************************************************//**
 * @brief      Initialize GPT input function 
 *
 * @param[in]  gptID:  Select the GPT module
 * @param[in]  gptInputConfig : Pointer to a input function configuration
 *
 * @return none
 *
 *******************************************************************************/
void GPT_InputConfig(GPT_ID_Type gptID, GPT_InputConfig_Type* gptInputConfig)
{
  gpt_reg_t * GPTx = (gpt_reg_t *)(gptAddr[gptID]);

  CHECK_PARAM(IS_GPT_PERIPH(gptID));
  
  /* select input capturing edge */
  GPTx->IC_CNTL_REG.BF.CHX_IC_EDGE = gptInputConfig->edgeSel;

  /* select trigger repetition */
  GPTx->IC_CNTL_REG.BF.CHX_IC_REP = gptInputConfig->trigRep;

  /* select trigger sampling clock divider */
  GPTx->IC_CNTL_REG.BF.CHX_IC_DIV = gptInputConfig->trigClkDivider;

  /* select input filtering */
  GPTx->IC_CNTL_REG.BF.CHX_IC_WIDTH = gptInputConfig->trigFilter;
}

/****************************************************************************//**
 * @brief      Initialize GPT channel output config 
 *
 * @param[in]  gptID:  Select the GPT module
 * @param[in]  channelID:  Channel ID
 * @param[in]  gptChOutConfig:  Pointer to a channel output configuration
 *
 * @return none
 *
 *******************************************************************************/
void GPT_ChannelOutputConfig(GPT_ID_Type gptID, GPT_ChannelNumber_Type channelID, 
                          GPT_ChannelOutputConfig_Type* gptChOutConfig)
{
  gpt_reg_t * GPTx = (gpt_reg_t *)(gptAddr[gptID]);

  CHECK_PARAM(IS_GPT_PERIPH(gptID));
  CHECK_PARAM(IS_GPT_CHANNEL_ID(channelID));

  /* set channel action: LOW ACT */
  GPTx->CH[channelID].CHX_ACT_REG.BF.CHX_LOW_ACT_T = gptChOutConfig->lowAct;

  /* set channel action: UPP ACT */
  GPTx->CH[channelID].CHX_ACT_REG.BF.CHX_UPP_ACT_T = gptChOutConfig->uppAct;

  /* set channel action: CUP ACT */
  GPTx->CH[channelID].CHX_ACT_REG.BF.CHX_CUP_ACT_T = gptChOutConfig->cupAct;

  /* set channel action: CDN ACT */
  GPTx->CH[channelID].CHX_ACT_REG.BF.CHX_CDN_ACT_T = gptChOutConfig->cdnAct;

  /************************set channel match value*****************************/

  /* set cup value */
  GPTx->CH[channelID].CHX_CUP_VAL_REG.BF.CUP_VAL = gptChOutConfig->cupVal;

  /* set cdn value */
  GPTx->CH[channelID].CHX_CDN_VAL_REG.BF.CDN_VAL = gptChOutConfig->cdnVal;
  
  /* update them to internal shadow registers */
  GPTx->USER_REQ_REG.WORDVAL |= (0x00010000 << channelID);
}

/****************************************************************************//**
 * @brief      Set GPT counter overflow value 
 *
 * @param[in]  gptID:  Select the GPT module
 * @param[in]  uppVal: Counter overflow value
 *
 * @return none
 *
 *******************************************************************************/
void GPT_SetCounterUppVal(GPT_ID_Type gptID, uint32_t uppVal)
{
  gpt_reg_t * GPTx = (gpt_reg_t *)(gptAddr[gptID]);

  CHECK_PARAM(IS_GPT_PERIPH(gptID));

  /* set counter overflow value */
  GPTx->CNT_UPP_VAL_REG.BF.UPP_VAL = uppVal;
}
 
/****************************************************************************//**
 * @brief      Set channel count up value 
 *
 * @param[in]  gptID:  Select the GPT module
 * @param[in]  channelID:  Channel ID
 * @param[in]  cupVal:  Channel count up value
 *
 * @return none
 *
 *******************************************************************************/
void GPT_SetChannelUpVal(GPT_ID_Type gptID, GPT_ChannelNumber_Type channelID, uint32_t cupVal)
{
  gpt_reg_t * GPTx = (gpt_reg_t *)(gptAddr[gptID]);

  CHECK_PARAM(IS_GPT_PERIPH(gptID));
  CHECK_PARAM(IS_GPT_CHANNEL_ID(channelID));

  /* set channel count up value */
  GPTx->CH[channelID].CHX_CUP_VAL_REG.BF.CUP_VAL = cupVal;
  
  /* update them to internal shadow registers */
  GPTx->USER_REQ_REG.WORDVAL |= 0x010000 << channelID;  
}
 
/****************************************************************************//**
 * @brief      Set channel count down value 
 *
 * @param[in]  gptID:  Select the GPT module
 * @param[in]  channelID:  Channel ID
 * @param[in]  cdnVal:  Channel count down value
 *
 * @return none
 *
 *******************************************************************************/
void GPT_SetChannelDownVal(GPT_ID_Type gptID, GPT_ChannelNumber_Type channelID, uint32_t cdnVal)
{
  gpt_reg_t * GPTx = (gpt_reg_t *)(gptAddr[gptID]);

  CHECK_PARAM(IS_GPT_PERIPH(gptID));
  CHECK_PARAM(IS_GPT_CHANNEL_ID(channelID));

  /* set channel count down value */
  GPTx->CH[channelID].CHX_CDN_VAL_REG.BF.CDN_VAL = cdnVal;
  
  /* update them to internal shadow registers */
  GPTx->USER_REQ_REG.WORDVAL |= 0x010000 << channelID; 
}

/****************************************************************************//**
 * @brief      Set channel count up and down value 
 *
 * @param[in]  gptID:  Select the GPT module
 * @param[in]  channelID:  Channel ID
 * @param[in]  cupVal:  Channel count up value
 * @param[in]  cdnVal:  Channel count down value
 *
 * @return none
 *
 *******************************************************************************/
void GPT_SetChannelUpDownVal(GPT_ID_Type gptID, GPT_ChannelNumber_Type channelID, uint32_t cupVal, uint32_t cdnVal)
{
  gpt_reg_t * GPTx = (gpt_reg_t *)(gptAddr[gptID]);

  CHECK_PARAM(IS_GPT_PERIPH(gptID));
  CHECK_PARAM(IS_GPT_CHANNEL_ID(channelID));

  /* set channel count down value */
  GPTx->CH[channelID].CHX_CUP_VAL_REG.BF.CUP_VAL = cupVal;
  GPTx->CH[channelID].CHX_CDN_VAL_REG.BF.CDN_VAL = cdnVal;
  
  /* update them to internal shadow registers */
  GPTx->USER_REQ_REG.WORDVAL |= 0x010000 << channelID; 
}

/****************************************************************************//**
 * @brief      Get GPT capture value 
 *
 * @param[in]  gptID:  Select the GPT module
 * @param[in]  channelID:  Channel ID
 *
 * @return capture value
 *
 *******************************************************************************/
uint32_t GPT_GetCaptureVal(GPT_ID_Type gptID, GPT_ChannelNumber_Type channelID)
{
  uint32_t capValue;
  gpt_reg_t * GPTx = (gpt_reg_t *)(gptAddr[gptID]);

  CHECK_PARAM(IS_GPT_PERIPH(gptID));
  CHECK_PARAM(IS_GPT_CHANNEL_ID(channelID));

  /* read the value */
  capValue = GPTx->CH[channelID].CHX_CUP_VAL_REG.BF.CUP_VAL;

  return capValue;
}
 
 /****************************************************************************//**
 * @brief      Get counter value 
 *
 * @param[in]  gptID:  Select the GPT module
 *
 * @return counter value
 *
 *******************************************************************************/
uint32_t GPT_GetCounterVal(GPT_ID_Type gptID)
{
  uint32_t cntValue;
  gpt_reg_t * GPTx = (gpt_reg_t *)(gptAddr[gptID]);

  CHECK_PARAM(IS_GPT_PERIPH(gptID));

  /* read the value */
  cntValue = GPTx->CNT_VAL_REG.BF.CNT_VAL;

  return cntValue;
} 

/****************************************************************************//**
 * @brief  GPT DMA channel enable / disable function 
 *
 * @param[in]  gptID: Select the GPT module
 * @param[in]  dmaChannel: Select the DMA channel
 * @param[in]  state: Enable / Disable  
 *
 * @return none
 *
 *******************************************************************************/
void GPT_DMACmd(GPT_ID_Type gptID, GPT_DMAChannel_Type dmaChannel, FunctionalState state)
{  
  gpt_reg_t * GPTx = (gpt_reg_t *)(gptAddr[gptID]);
  
  CHECK_PARAM(IS_GPT_PERIPH(gptID));
  CHECK_PARAM(IS_GPT_DMA_CHANNEL_ID(dmaChannel));  

  /* Enable / Disable DMA channel  */
  if(ENABLE == state)
  {
    GPTx->DMA_CNTL_EN_REG.WORDVAL |= (1<<dmaChannel);
  }
  if(DISABLE == state)
  {
    GPTx->DMA_CNTL_EN_REG.WORDVAL &= (~(1<<dmaChannel));
  }
}

/****************************************************************************//**
 * @brief  GPT channel selection for DMA channels function 
 * 
 * @param[in]  gptID: Select the GPT module
 * @param[in]  dmaChannel: Select the DMA channel 
 * @param[in]  channelID: GPT channel ID
 *
 * @return none
 *
 *******************************************************************************/
void GPT_DMAChannelSelect(GPT_ID_Type gptID, GPT_DMAChannel_Type dmaChannel,
                          GPT_ChannelNumber_Type channelID)
{
  gpt_reg_t * GPTx = (gpt_reg_t *)(gptAddr[gptID]);
  
  CHECK_PARAM(IS_GPT_PERIPH(gptID));
  CHECK_PARAM(IS_GPT_CHANNEL_ID(channelID));
  CHECK_PARAM(IS_GPT_DMA_CHANNEL_ID(dmaChannel)); 
  
  /* select a gpt channel for DMA channel 0 */
  if(GPT_DMA0 == dmaChannel)
  {
    GPTx->DMA_CNTL_CH_REG.BF.DMA0_CH = channelID;
  }
  
  /* select a GPT channel for DMA channel 1 */
  if(GPT_DMA1 == dmaChannel)
  {
    GPTx->DMA_CNTL_CH_REG.BF.DMA1_CH = channelID;
  }
}

/****************************************************************************//**
 * @brief      Clear GPT interrupt flag
 *
 * @param[in]  gptID:  Select the GPT module
 * @param[in]  intType:  Interrupt type
 *
 * @return none
 *
 *******************************************************************************/
void GPT_IntClr(GPT_ID_Type gptID, GPT_INT_Type intType)
{
  gpt_reg_t * GPTx = (gpt_reg_t *)(gptAddr[gptID]);

  CHECK_PARAM(IS_GPT_PERIPH(gptID));
  CHECK_PARAM(IS_GPT_INT_TYPE(intType));

  /* clear channel cup interrupt flag */
  if(intType < GPT_CHANNEL_NUMBER)
  {
    GPTx->INT_RAW_REG.WORDVAL = (1<<(intType));
  }
  
  /* clear channel cdn interrupt flag */
  if((intType <= GPT_INT_CH4_CDN) && (intType >= GPT_INT_CH0_CDN))
  {
    GPTx->INT_RAW_REG.WORDVAL = (1<<(intType + 3));
  }
  
  switch(intType)
  {
    /* clear counter up interrupt */
    case GPT_INT_CNT_UPP:
      GPTx->INT_RAW_REG.WORDVAL = GPT_INT_CNT_UPP_MSK;
      break;

    /* clear counter low interrupt */
    case GPT_INT_CNT_LOW:
      GPTx->INT_RAW_REG.WORDVAL = GPT_INT_CNT_LOW_MSK;
      break;

    /* clear DMA CH0 interrupt */
    case GPT_INT_DMA0_OF:
      GPTx->INT_RAW_REG.WORDVAL = GPT_INT_DMA0_OF_MSK;
      break;

    /* clear DMA CH1 interrupt */
    case GPT_INT_DMA1_OF:
      GPTx->INT_RAW_REG.WORDVAL = GPT_INT_DMA1_OF_MSK;
      break;
      
     /* clear all the GPT interrupts */
    case GPT_INT_ALL:
      GPTx->INT_RAW_REG.WORDVAL = GPT_INT_ALL_MSK;
      break;
      
    default:
      break;
  }  
}
/****************************************************************************//**
 * @brief     MASK / UNMASK  GPT interrupt 
 *
 * @param[in]  gptID:  Select the GPT module
 * @param[in]  intType:  Interrupt type
 * @param[in]  intMsk:   MASK / UNMASK control
 *
 * @return none
 *
 *******************************************************************************/
void GPT_IntMask(GPT_ID_Type gptID, GPT_INT_Type intType, IntMask_Type intMsk)
{
  gpt_reg_t * GPTx = (gpt_reg_t *)(gptAddr[gptID]);
  
  CHECK_PARAM(IS_GPT_PERIPH(gptID));
  CHECK_PARAM(IS_GPT_INT_TYPE(intType));
  CHECK_PARAM(IS_INTMASK(intMsk));

   /* set channel cup interrupt mask/unmask */
  if(intType < GPT_CHANNEL_NUMBER)
  {
    if(intMsk == UNMASK )
    {
      GPTx->INT_MSK_REG.WORDVAL &= (~(1<<(intType)));
    }
    else
    {
      GPTx->INT_MSK_REG.WORDVAL |= (1<<(intType));
    }   
  }
  
  /* set channel cdn interrupt mask/unmask */
  if((intType >= GPT_INT_CH0_CDN)&& (intType < (GPT_INT_CH0_CDN + GPT_CHANNEL_NUMBER)))
  {
    if(intMsk == UNMASK )
    {
      GPTx->INT_MSK_REG.WORDVAL &= (~(1<<(intType+3)));
    }
    else
    {
      GPTx->INT_MSK_REG.WORDVAL |= (1<<(intType+3));
    }   
  }
  
  switch(intType)
  {
    /* set counter up interrupt mask/unmask*/
    case GPT_INT_CNT_UPP:
      GPTx->INT_MSK_REG.BF.CNT_UPP_MSK = intMsk;
      break;

    /* set counter low interrupt mask/unmask*/
    case GPT_INT_CNT_LOW:
      GPTx->INT_MSK_REG.BF.CNT_LOW_MSK = intMsk;
      break;

    /* set DMA CH0 interrupt mask/unmask*/
    case GPT_INT_DMA0_OF:
      GPTx->INT_MSK_REG.BF.DMA0_OF_MSK = intMsk;
      break;

    /* set DMA CH1 interrupt mask/unmask*/
    case GPT_INT_DMA1_OF:
      GPTx->INT_MSK_REG.BF.DMA1_OF_MSK = intMsk;
      break;
      
    /* set all interrupt mask/unmask*/
    case GPT_INT_ALL:
      if(intMsk == UNMASK)
      {
        GPTx->INT_MSK_REG.WORDVAL = 0;
      }
      else
      {
        GPTx->INT_MSK_REG.WORDVAL |= 0x03031F1F; 
      }
      break;
      
    default:
      break;
  }  
}

/****************************************************************************//**
 * @brief      Clear the GPT counter register to default value
 *
 * @param[in]  gptID:  Select the GPT module
 *
 * @return none
 *
 * Stop the GPT 
 *******************************************************************************/
void GPT_RegClear(GPT_ID_Type gptID)
{
  gpt_reg_t * GPTx = (gpt_reg_t *)(gptAddr[gptID]);
  uint32_t ch = 0;

  CHECK_PARAM(IS_GPT_PERIPH(gptID));
  
  /* stop GPT counter */
  GPTx->CNT_EN_REG.BF.CNT_STOP = 1;    
  
  /* GPT counter reset */
  GPTx->CNT_EN_REG.BF.CNT_RESET = 1;
  while (!GPTx->CNT_EN_REG.BF.CNT_RST_DONE);
  
  /* disable all interrupt */
  GPTx->INT_MSK_REG.WORDVAL |= GPT_INT_ALL_MSK;
  
  /* clear all pending interrupts */
  GPTx->INT_RAW_REG.WORDVAL = GPT_INT_ALL_MSK;      
  
  /* GPT counter control register reset */
  GPTx->CNT_CNTL_REG.BF.CNT_DIR = 0;
  GPTx->CNT_CNTL_REG.BF.CNT_DBG_ACT = 0;
  GPTx->CNT_CNTL_REG.BF.CNT_UPDT_MOD = 0;
  
  /* set default reload value */
  GPTx->CNT_UPP_VAL_REG.BF.UPP_VAL = 0xFFFFFFFF;
  
  /* config clock frequency by default */
  GPTx->CLK_CNTL_REG.BF.CLK_SRC = 0;
  GPTx->CLK_CNTL_REG.BF.CLK_DIV = 0;
  GPTx->CLK_CNTL_REG.BF.CLK_PRE = 0;
  
  /* GPT counter input capture control register reset */
  GPTx->IC_CNTL_REG.BF.CHX_IC_EDGE = 0;  
  GPTx->IC_CNTL_REG.BF.CHX_IC_REP = 0;  
  GPTx->IC_CNTL_REG.BF.CHX_IC_DIV = 0;
  GPTx->IC_CNTL_REG.BF.CHX_IC_WIDTH = 0;
  
  /* GPT counter DMA register reset */
  GPTx->DMA_CNTL_EN_REG.BF.DMA0_EN = 0;
  GPTx->DMA_CNTL_EN_REG.BF.DMA1_EN = 0;
    
  GPTx->DMA_CNTL_CH_REG.BF.DMA0_CH = 0;
  GPTx->DMA_CNTL_CH_REG.BF.DMA1_CH = 0;
    
  /* GPT counter channelregister reset */
  for(ch = 0; ch < GPT_CHANNEL_NUMBER; ch ++)
  {
    GPTx->CH[ch].CHX_CNTL_REG.BF.CHX_IO = 0;    
    GPTx->CH[ch].CHX_ACT_REG.WORDVAL = 0;
    
    GPTx->CH[ch].CHX_CUP_VAL_REG.BF.CUP_VAL = 0;
    GPTx->CH[ch].CHX_CDN_VAL_REG.BF.CDN_VAL = 0;
  }
    
  /* GPT counter reset */
  GPTx->CNT_EN_REG.BF.CNT_RESET = 1;
  while (!GPTx->CNT_EN_REG.BF.CNT_RST_DONE);
}

/****************************************************************************//**
 * @brief      Get GPT interrupt status
 *
 * @param[in]  gptID:  Select the GPT module
 * @param[in]  intType:  Interrupt type
 *
 * @return interrupt status
 *
 *******************************************************************************/
IntStatus GPT_GetIntStatus(GPT_ID_Type gptID, GPT_INT_Type intType)
{
  gpt_reg_t * GPTx = (gpt_reg_t *)(gptAddr[gptID]);
  uint32_t retStatus = 0;

  CHECK_PARAM(IS_GPT_PERIPH(gptID));
  CHECK_PARAM(IS_GPT_INT_TYPE(intType));

  /* get channel cup interrupt status */
  if(intType < GPT_CHANNEL_NUMBER)
  {
    retStatus = ((GPTx->INT_REG.WORDVAL) & (1<<(intType)));
  }
  
  /* get channel cdn interrupt status */
  if((intType <= GPT_INT_CH4_CDN) && (intType >= GPT_INT_CH0_CDN))
  {
    retStatus = ((GPTx->INT_REG.WORDVAL) & (1<<(intType+3)));
  }
    
  switch(intType)
  {
    /* get counter up interrupt status */
    case GPT_INT_CNT_UPP:
      retStatus = GPTx->INT_REG.BF.CNT_UPP_INTR;
      break;

    /* get counter low interrupt status */
    case GPT_INT_CNT_LOW:
      retStatus = GPTx->INT_REG.BF.CNT_LOW_INTR;
      break;

    /* get DMA CH0 interrupt status */
    case GPT_INT_DMA0_OF:
      retStatus = GPTx->INT_REG.BF.DMA0_OF_INTR;
      break;

    /* get DMA CH1 interrupt status */
    case GPT_INT_DMA1_OF:
      retStatus = GPTx->INT_REG.BF.DMA1_OF_INTR;
      break;

    /* get all interrupt status. If there is any interrupt, return SET, otherwise return RESET. */
    case GPT_INT_ALL:
      retStatus = (GPTx->INT_REG.WORDVAL)&(GPT_INT_ALL_MSK);
      break;
      
    default:
      break;
  }

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
 * @brief  GPT channel CUP interrupt function 
 *
 * @param[in]  intPeriph: Select the peripheral
 * @param[in]  gptID:  Select the GPT module
 * @param[in]  cupIntStatus: Channel interrupt status 
 *
 * @return none
 *
 *******************************************************************************/
static void GPT_ChCupIRQHandler(INT_Peripher_Type intPeriph, GPT_ID_Type gptID, uint32_t cupIntStatus)
{
  gpt_reg_t * GPTx = (gpt_reg_t *)(gptAddr[gptID]);
  uint8_t cnt = 0;

  CHECK_PARAM(IS_GPT_PERIPH(gptID)); 
  
  /* scan the channels */
  for(cnt = 0; cnt < GPT_CHANNEL_NUMBER; cnt++)
  {
    /* check if interrupt available for the specific channel */
    if(cupIntStatus & 0x01 == 0x01)
    {
      if(intCbfArra[intPeriph][cnt] != NULL)
      {      
        /* call the callback function */
        intCbfArra[intPeriph][cnt]();
      }
      /* Disable the interrupt if callback function is not setup */
      else
      {
        GPTx->INT_MSK_REG.WORDVAL |= (1<<cnt) ;
      }
      
    }
    
    cupIntStatus = cupIntStatus >> 1;
  }
}
/****************************************************************************//**
 * @brief  GPT channel CDN interrupt function 
 *
 * @param[in]  intPeriph: Select the peripheral
 * @param[in]  gptID:  Select the GPT module
 * @param[in]  cdnIntStatus: Channel CDN interrupt status 
 *
 * @return none
 *
 *******************************************************************************/
static void GPT_ChCdnIRQHandler(INT_Peripher_Type intPeriph, GPT_ID_Type gptID, uint32_t cdnIntStatus)
{
  gpt_reg_t * GPTx = (gpt_reg_t *)(gptAddr[gptID]);
  uint8_t cnt = 0;

  CHECK_PARAM(IS_GPT_PERIPH(gptID)); 
  
  /* scan the channels */
  for(cnt = 0; cnt < GPT_CHANNEL_NUMBER; cnt++)
  {
    /* check if interrupt available for the specific channel */
    if((cdnIntStatus & 0x0100) != 0)
    {
      if(intCbfArra[intPeriph][GPT_INT_CH0_CDN + cnt] != NULL)
      {      
        /* call the callback function */
        intCbfArra[intPeriph][GPT_INT_CH0_CDN + cnt]();
      }
      /* Disable the interrupt if callback function is not setup */
      else
      {
        GPTx->INT_MSK_REG.WORDVAL |= (0x0100 << cnt) ;
      }
      
    }
    
    cdnIntStatus = cdnIntStatus >> 1;
  }
}
/****************************************************************************//**
 * @brief  GPT interrupt function 
 *
 * @param[in]  intPeriph: Select the peripheral
 * @param[in]  gptID:  Select the GPT module
 *
 * @return none
 *
 *******************************************************************************/
static void GPT_IRQHandler(INT_Peripher_Type intPeriph, GPT_ID_Type gptID)
{
  gpt_reg_t * GPTx = (gpt_reg_t *)(gptAddr[gptID]);
  uint32_t intStatus, cupIntStatus, cdnIntStatus;  

  CHECK_PARAM(IS_GPT_PERIPH(gptID));
  
  /* keep a copy of current interrupt status */
  intStatus = GPTx->INT_REG.WORDVAL;
  cupIntStatus = intStatus & 0x1F;  
  cdnIntStatus = intStatus & 0x1F00;  
  
  /* clear the generated interrupts */
  GPTx->INT_RAW_REG.WORDVAL = intStatus;
  
  /* channel cup interrupt */
  if((intStatus & (GPT_INT_CH_CUP_ALL_MSK))!= 0x00)
  {
    GPT_ChCupIRQHandler(intPeriph, gptID, cupIntStatus);
  }
  
  /* channel cdn interrupt */
  if((intStatus & (GPT_INT_CH_CDN_ALL_MSK))!= 0x00)
  {
    GPT_ChCdnIRQHandler(intPeriph, gptID, cdnIntStatus);
  }
  
  /* counter upp interrupt */
  if((intStatus & (GPT_INT_CNT_UPP_MSK))!= 0x00)
  {
    if(intCbfArra[intPeriph][GPT_INT_CNT_UPP] != NULL)
    {
      /* call the callback function */
      intCbfArra[intPeriph][GPT_INT_CNT_UPP]();
    }
    /* Disable the interrupt if callback function is not setup */
    else
    {
      GPTx->INT_MSK_REG.BF.CNT_UPP_MSK = 1;
    }
  }
  
  /* counter low interrupt */
  if((intStatus & (GPT_INT_CNT_LOW_MSK))!= 0x00)
  {
    if(intCbfArra[intPeriph][GPT_INT_CNT_LOW] != NULL)
    {
      /* call the callback function */
      intCbfArra[intPeriph][GPT_INT_CNT_LOW]();
    }
    /* Disable the interrupt if callback function is not setup */
    else
    {
      GPTx->INT_MSK_REG.BF.CNT_LOW_MSK = 1;
    }
  }

  /* DMA channel 0 overflow interrupt */
  if((intStatus & (GPT_INT_DMA0_OF_MSK))!= 0x00)
  {
    if(intCbfArra[intPeriph][GPT_INT_DMA0_OF] != NULL)
    {
      /* call the callback function */
      intCbfArra[intPeriph][GPT_INT_DMA0_OF]();
    }
    /* Disable the interrupt if callback function is not setup */
    else
    {
      GPTx->INT_MSK_REG.BF.DMA0_OF_MSK = 1;
    }
  }  

  /* DMA channel 1 overflow interrupt */
  if((intStatus & (GPT_INT_DMA1_OF_MSK))!= 0x00)
  {
    if(intCbfArra[intPeriph][GPT_INT_DMA1_OF] != NULL)
    {
      /* call the callback function */
      intCbfArra[intPeriph][GPT_INT_DMA1_OF]();
    }
    /* Disable the interrupt if callback function is not setup */
    else
    {
      GPTx->INT_MSK_REG.BF.DMA1_OF_MSK = 1;
    }
  }  
}

/****************************************************************************//**
 * @brief  GPT1 interrupt handle function 
 *
 * @param  none
 *
 * @return none
 *
 *******************************************************************************/
__attribute__((weak)) void GPT1_IRQHandler(void)
{
  GPT_IRQHandler(INT_GPT1, GPT1_ID);
}

/****************************************************************************//**
 * @brief  GPT2 interrupt handle function 
 *
 * @param  none
 *
 * @return none
 *
 *******************************************************************************/
__attribute__((weak)) void GPT2_IRQHandler(void)
{
  GPT_IRQHandler(INT_GPT2, GPT2_ID);
}
