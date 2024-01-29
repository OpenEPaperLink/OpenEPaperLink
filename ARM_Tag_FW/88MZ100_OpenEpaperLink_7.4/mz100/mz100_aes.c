#include "mz100.h"
#include "mz100_aes.h"
#include "mz100_driver.h"

/****************************************************************************//**
 * @brief      Init AES with predefined configurations
 *
 * @param[in]  aesConfig:  AES configuration
 *
 * @return none
 *
 *******************************************************************************/
void AES_Init(AES_Config_Type * aesConfig)
{
  uint32_t localCnt;

  /* reset AES */
  AES->CTRL2.BF.CORE_RESET = 1;
  for(localCnt = 0; localCnt < 0x20; localCnt++);
  AES->CTRL2.BF.CORE_RESET = 0;

  /* disbale AES */
  AES->CTRL1.BF.START = 0;

  /* set AES mode */
  AES->CTRL1.BF.MODE = aesConfig->mode;

  /* select AES encryption / decryption mode */
  AES->CTRL1.BF.DECRYPT = aesConfig->encDecSel;

  /* set init vector */
  for(localCnt = 0; localCnt < 4; localCnt++)
  {
    AES->IV[localCnt].WORDVAL = aesConfig->initVect[localCnt];
  }

  /* set key size and key */
  AES->CTRL1.BF.KEY_SIZE = aesConfig->keySize;
  for(localCnt = 0 ; localCnt < 8; localCnt++)
  {
    AES->KEY[localCnt].WORDVAL = aesConfig->key[localCnt];
  }

  /* set A_str length */
  if(aesConfig->mode == AES_MODE_CCM)
  {
    AES->ASTR_LEN.WORDVAL = aesConfig->aStrLen;
  }

  /* set M_str length */
  AES->MSTR_LEN.WORDVAL = aesConfig->mStrLen;

  /* set MIC length and control */
  AES->CTRL1.BF.MIC_LEN = aesConfig->micLen;
  AES->CTRL1.BF.OUT_MIC = aesConfig->micEn;
}

/****************************************************************************//**
 * @brief      Reset AES module
 *
 * @param none
 *
 * @return none
 *
 *******************************************************************************/
void AES_Reset(void)
{
  volatile uint32_t tmpCnt = 0x20;
  
  /* reset AES */
  AES->CTRL2.BF.CORE_RESET = 1;
  while(tmpCnt--);
  AES->CTRL2.BF.CORE_RESET = 0;
}

/****************************************************************************//**
**
 * @brief      enable automatic reset AES module after  lock successfully
 *
 * @param none
 *
 * @return none
 *
 
*******************************************************************************/
void AES_EnableAutoReset(void)
{
  AES->CTRL2.BF.AUTO_RESET_EN = 1;
}

/****************************************************************************//**
 * @brief      Enable AES module
 *
 * @param none
 *
 * @return none
 *
 *******************************************************************************/
void AES_Enable(void)
{
  /* enable AES module */
  AES->CTRL1.BF.START = 1;
}

/****************************************************************************//**
 * @brief      Disable AES module
 *
 * @param none
 *
 * @return none
 *
 *******************************************************************************/
void AES_Disable(void)
{
  /* disable AES module */
  AES->CTRL1.BF.START = 0;
}

/****************************************************************************//**
 * @brief      Lock AES module from CM3 side
 *
 * @param[in]  state:  enable / disable
 *
 * @return none
 *
 *******************************************************************************/
void AES_Lock(FunctionalState state)
{
  /* lock AES from CM3 side */
  AES->CTRL1.BF.LOCK0 = ((state == ENABLE) ? 1 : 0);
}

/****************************************************************************//**
 * @brief      Get AES lock status
 *
 * @param none
 *
 * @return lock status
 *
 *******************************************************************************/
FunctionalState AES_GetLockStatus()
{
  return (FunctionalState)AES->STATUS.BF.RSVD0;
}

/****************************************************************************//**
 * @brief      Set CTR mode's counter modular
 *
 * @param[in]  cntmod:  CTR mode's counter modular
 *
 * @return none
 *
 *******************************************************************************/
void AES_SetCTRCntMod(uint32_t cntmod)
{
  /* set CTR mode's counter modular */
  AES->CTRL1.BF.CTR_MOD = cntmod;
}

/****************************************************************************//**
 * @brief      Set Cipher stealing mode of CBC
 *
 * @param[in]  :  Cipher stealing mode of CBC
 *
 * @return none
 *
 *******************************************************************************/
void AES_SetCtsMode(AES_CtsMode_Type ctsMode)
{
  /* set Cipher stealing mode of CBC */
  AES->CTRL1.BF.CTS_MODE = ctsMode;
}

/****************************************************************************//**
 * @brief      AES output stream from output FIFO enable / disable
 *
 * @param[in]  state:  enable / disable
 *
 * @return none
 *
 *******************************************************************************/
void AES_OutmsgCmd(FunctionalState state)
{
  /* output stream from output FIFO forward /  block */
  AES->CTRL1.BF.OUT_MSG = ((state == ENABLE) ? 1 : 0);
}

/****************************************************************************//**
 * @brief      AES DMA function enable / disable
 *
 * @param[in]  state:  enable / disable
 *
 * @return none
 *
 *******************************************************************************/
void AES_DmaCmd(FunctionalState state)
{
  /* choose message pass interface through DMA / register */
  AES->CTRL1.BF.IO_SRC = ((state == ENABLE) ? 1 : 0);
  
  /* DMA function enable /disable */
  AES->CTRL1.BF.DMA_EN = ((state == ENABLE) ? 1 : 0);  
}

/****************************************************************************//**
 * @brief      Feed data in AES stream
 *
 * @param[in]  data:  input data
 *
 * @return none
 *
 *******************************************************************************/
void AES_FeedData(uint32_t data)
{
  /* Feed data in AES input stream */
  AES->STR_IN.WORDVAL = data;
}

/****************************************************************************//**
 * @brief      Read AES output data
 *
 * @param none
 *
 * @return output data
 *
 *******************************************************************************/
uint32_t AES_ReadData(void)
{
  /* read AES output data */
  return AES->STR_OUT.WORDVAL;
}

/****************************************************************************//**
 * @brief      Read AES MIC result
 *
 * @param[out]  micData calculated MIC data
 * @param[in] len  expected MIC data length
 *
 * @return none
 *
 *******************************************************************************/
void AES_ReadMIC(uint32_t* micData, uint8_t len)
{
  uint32_t cnt;

  /* check for len */
  if((len == 0) || (len > 4))
    return;

  /* captured the mic data */
  for(cnt = 0; cnt < len; cnt++)
  {
    micData[cnt] = AES->OV[cnt].WORDVAL;
  }
}

/****************************************************************************//**
 * @brief      Clear AES Input FIFO
 *
 * @param none
 *
 * @return none
 *
 *******************************************************************************/
void AES_ClrInputFIFO(void)
{
  volatile uint32_t tmpCnt = 0x20;
  
  /* Feed data in AES input stream */
  AES->CTRL1.BF.IF_CLR = 1;
  while(tmpCnt--);  
  AES->CTRL1.BF.IF_CLR = 0;
}

/****************************************************************************//**
 * @brief      Clear AES Output FIFO
 *
 * @param none
 *
 * @return none
 *
 *******************************************************************************/
void AES_ClrOutputFIFO(void)
{
  volatile uint32_t tmpCnt = 0x20;
  
  /* Feed data in AES input stream */
  AES->CTRL1.BF.OF_CLR = 1;
  while(tmpCnt--);  
  AES->CTRL1.BF.OF_CLR = 0;
}

/****************************************************************************//**
 * @brief      Mask / Unmask AES interrupt
 *
 * @param[in]  intType:  Interrupt type
 * @param[in]  maskState:  Mask / Unmask control
 *
 * @return none
 *
 *******************************************************************************/
void AES_IntMask(AES_INT_Type intType, IntMask_Type maskState)
{  
  uint32_t mask = 0;
  
  mask = (maskState == UNMASK) ? 0 : 1;
  
  switch(intType)
  {
    case AES_INT_OUTFIFO_EMPTY:
      /* unmask/mask interrupt */
      AES->IMR.WORDVAL = (AES->IMR.WORDVAL & 0xFFFFFFFB) | (mask << 2);
      break;

    case AES_INT_INFIFO_FULL:
      /* unmask/mask interrupt */
      AES->IMR.WORDVAL = (AES->IMR.WORDVAL & 0xFFFFFFFD) | (mask << 1);
      break;

    case AES_INT_DONE:
      /* unmask/mask interrupt */
      AES->IMR.WORDVAL = (AES->IMR.WORDVAL & 0xFFFFFFFE) | (mask << 0);
      break;

    case AES_INT_ALL:
      /* unmask/mask all interrupt */
      if(maskState == UNMASK)
      {
        AES->IMR.WORDVAL &= 0xFFFFFFF8;
      }
      else
        AES->IMR.WORDVAL |= 0x7;
        

      break;

    default:
      break;
  }
}

/****************************************************************************//**
 * @brief      Clear AES interrupt flag
 *
 * @param[in]  intType:  Interrupt type
 *
 * @return none
 *
 *******************************************************************************/
void AES_IntClr(AES_INT_Type intType)
{
  switch(intType)
  {
    case AES_INT_OUTFIFO_EMPTY:
      /* clear interrupt */
      AES->ICR.WORDVAL |= 0x4;
      break;

    case AES_INT_INFIFO_FULL:
      /* clear interrupt */
      AES->ICR.WORDVAL |= 0x2;
      break;

    case AES_INT_DONE:
      /* clear interrupt */
      AES->ICR.WORDVAL |= 0x1;
      break;

    case AES_INT_ALL:
      /* clear all interrupt */
      AES->ICR.WORDVAL |= 0x7;
      break;

    default:
      break;
  }
}

/****************************************************************************//**
 * @brief      Get AES interrupt status
 *
 * @param[in]  intType:  Interrupt type
 *
 * @return interrupt status
 *
 *******************************************************************************/
IntStatus AES_GetIntStatus(AES_INT_Type intType)
{
  IntStatus status = RESET;

  /* get the interrupt status */
  switch(intType)
  {
    /* AES output FIFO empty interrupt */
    case AES_INT_OUTFIFO_EMPTY:
      status = ((AES->ISR.WORDVAL & 0x4) ? SET : RESET);
      break;

    /* AES input FIFO full interrupt */
    case AES_INT_INFIFO_FULL:
      status = ((AES->ISR.WORDVAL & 0x2) ? SET : RESET);
      break;

    /* AES done interrupt */
    case AES_INT_DONE:
      status = ((AES->ISR.WORDVAL & 0x1) ? SET : RESET);
      break;

    /* all interrupt */ 
    case AES_INT_ALL:
      status = ((AES->ISR.WORDVAL & 0x7) ? SET : RESET);
      break;
      
    default:
      break;
  }

  return status;
}

/****************************************************************************//**
 * @brief      Get AES status
 *
 * @param[in]  statusType:  Status type
 *
 * @return AES status
 *
 *******************************************************************************/
FlagStatus AES_GetStatus(AES_Status_Type statusType)
{
  FlagStatus status = RESET;

  /* get the AES status */
  switch(statusType)
  {
    /* AES output FIFO empty status*/
    case AES_STATUS_OUTFIFO_EMPTY:
      status = ((AES->STATUS.BF.OF_EMPTY) ? SET : RESET);
      break;

    /* AES input FIFO full status */
    case AES_STATUS_INFIFO_FULL:
      status = ((AES->STATUS.BF.IF_FULL) ? SET : RESET);
      break;

    /* AES output FIFO can be read status */
    case AES_STATUS_OUTFIFO_RDY:
      status = ((AES->STATUS.BF.OF_RDY) ? SET : RESET);
      break;

    /* AES done status */
    case AES_STATUS_DONE:
      status = ((AES->STATUS.BF.DONE) ? SET : RESET);
      break;
            
    /* Input stream size is less than 16 bytes in ECB, CBC and CTR mode */
    case AES_STATUS_ERROR_0:
      status = ((AES->STATUS.BF.STATUS & 0x1) ? SET : RESET);
      break;

    /* Data is more than 2^13-1 bytes in MMO mode 
       Data is not multiple of 16 bytes in ECB mode*/
    case AES_STATUS_ERROR_1:
      status = ((AES->STATUS.BF.STATUS & 0x2) ? SET : RESET);
      break;

    /* MIC mismatch during decryption in CCM* mode */
    case AES_STATUS_ERROR_2:
      status = ((AES->STATUS.BF.STATUS & 0x4) ? SET : RESET);
      break;

    default:
      break;
  }

  return status;
}

/****************************************************************************//**
 * @brief  AES interrupt function
 *
 * @param  none
 *
 * @return none
 *
 *******************************************************************************/
__attribute__((weak)) void AES_IRQHandler(void)
{
  uint32_t intMask = 0;

  /* Store interrupt flags for later use */
  intMask = AES->ISR.WORDVAL;

  /* Clear unmask interrupt flag */
  AES->ICR.WORDVAL = intMask;
  
  /* check for output FIFO empty interrupt */
  if(intMask & 0x4)
  {
    /* check call back function availability */
    if(intCbfArra[INT_AES][AES_INT_OUTFIFO_EMPTY] != NULL)
    {
      /* call the call back function */
      intCbfArra[INT_AES][AES_INT_OUTFIFO_EMPTY]();
    }
    else
    {
      /* mask the interrupt if call back function not available */
      AES->IMR.WORDVAL |= 0x4;
    }
  }

  /* check for input FIFO full interrupt */
  if(intMask & 0x2)
  {
    /* check call back function availability */
    if(intCbfArra[INT_AES][AES_INT_INFIFO_FULL] != NULL)
    {
      /* call the call back function */
      intCbfArra[INT_AES][AES_INT_INFIFO_FULL]();
    }
    else
    {
      /* mask the interrupt if call back function not available */
      AES->IMR.WORDVAL |= 0x2;
    }
  }

  /* check for AES done interrupt */
  if(intMask & 0x1)
  {
    /* check call back function availability */
    if(intCbfArra[INT_AES][AES_INT_DONE] != NULL)
    {
      /* call the call back function */
      intCbfArra[INT_AES][AES_INT_DONE]();
    }
    else
    {
      /* mask the interrupt if call back function not available */
      AES->IMR.WORDVAL |= 0x1;
    }
  }
}
