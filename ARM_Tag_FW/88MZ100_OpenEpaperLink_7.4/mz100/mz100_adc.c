#include "mz100.h"
#include "mz100_adc.h"
#include "mz100_drv_config.h"

/****************************************************************************//**
 * @brief  ADC interrupt function 
 *
 * @param  none
 *
 * @return none
 *
 *******************************************************************************/
__attribute__((weak)) void ADC_IRQHandler(void)
{
  uint32_t intStatus;
  
  /* Store unmasked interrupt flags for later use */
  intStatus = (~ADC->IMR.WORDVAL) & 0x1F;
  intStatus &= ADC->ISR.WORDVAL;
    
  /* Clear all unmasked interrupt flags */
  ADC->ICR.WORDVAL = intStatus;
  
  /* ADC data ready inerrupt */
  if( intStatus & (1 << ADC_RDY) )
  {
    if(intCbfArra[INT_ADC][ADC_RDY] != NULL)
    {
      /* call the callback function */
      intCbfArra[INT_ADC][ADC_RDY]();
    }
    else
    {
      ADC->IMR.BF.RDY_MASK = 1;
    }
  }

  /* ADC gain correction saturation interrupt */
  if( intStatus & (1 << ADC_GAINSAT) )
  {
    if(intCbfArra[INT_ADC][ADC_GAINSAT] != NULL)
    {
      /* call the callback function */
      intCbfArra[INT_ADC][ADC_GAINSAT]();
    }
    else
    {
      ADC->IMR.BF.GAINSAT_MASK = 1;
    }
  }
  
  /* ADC offset correcion saturation interrupt */
  if( intStatus & (1 << ADC_OFFSAT) )
  {
    if(intCbfArra[INT_ADC][ADC_OFFSAT] != NULL)
    {
      /* call the callback function */
      intCbfArra[INT_ADC][ADC_OFFSAT]();
    }
    else
    {
      ADC->IMR.BF.OFFSAT_MASK = 1;
    } 
  }  
 
  /* ADC DMA data transfer failure interrupt */
  if( intStatus & (1 << ADC_DMAERR) )
  {
    if(intCbfArra[INT_ADC][ADC_DMAERR] != NULL)
    {
      /* call the callback function */
      intCbfArra[INT_ADC][ADC_DMAERR]();
    }
    else
    {
      ADC->IMR.BF.DMA_ERR_MASK = 1;
    } 
  }  

  /* ADC digital filtering saturation interrupt */
  if( intStatus & (1 << ADC_FILTERSAT) )
  {
    if(intCbfArra[INT_ADC][ADC_FILTERSAT] != NULL)
    {
      /* call the callback function */
      intCbfArra[INT_ADC][ADC_FILTERSAT]();
    }
    else
    {
      ADC->IMR.BF.FILTERSAT_MASK = 1;
    } 
  }    
}

/*@} end of group ADC_Private_Functions */

/** @defgroup ADC_Public_Functions
 *  @{
 */

/****************************************************************************//**
 * @brief      Reset ADC block 
 *
 * @param none
 *
 * @return none
 *
 *******************************************************************************/
void ADC_Reset(void)
{
  volatile uint32_t i;
  
  /* Software reset the ADC block */
  ADC->CLKRST.BF.SOFT_RST = 0x1;  
  
  /* Delay */
  for(i=0; i<10; i++);
  
  ADC->CLKRST.BF.SOFT_RST = 0x0;  
}

/****************************************************************************//**
 * @brief      Initializes the ADC with pre-defined ADC configuration structure
 *
 * @param[in]  adcConfigSet:  Pointer to a ADC configuration structure
 *
 * @return none
 *
 * Initializes the ADC 
 *******************************************************************************/
void ADC_Init(ADC_CFG_Type* adcConfigSet)
{
  volatile uint32_t i;
  
  /* Set ADC conversion resolution */
  ADC->ANA.BF.OSR = adcConfigSet->adcResolution;
  
  /* Set ADC reference source */
  ADC->ANA.BF.VREF_SEL = adcConfigSet->adcVrefSource; 
  
  /* Set ADC PGA gain */
  ADC->ANA.BF.BUF_GAIN = adcConfigSet->adcGainSel; 
  
  /* Set ADC internal clock divider */
  ADC->CLKRST.BF.SOFT_CLK_RST = 1;
  
  /* Delay */
  for(i=0; i<10; i++);
  
  ADC->CLKRST.BF.SOFT_CLK_RST = 0;
  
  ADC->CLKRST.BF.INT_CLK_DIV = adcConfigSet->adcClockDivider;
  
  /* Set ADC bias mode */
  ADC->ANA.BF.BIAS_SEL = adcConfigSet->adcBiasMode;
}

/****************************************************************************//**
 * @brief      Enable the ADC
 *
 * @param none
 *
 * @return none
 *
 *******************************************************************************/
void ADC_Enable(void)
{
  ADC->PWR.BF.GLOBAL_EN = 1; 
}

/****************************************************************************//**
 * @brief      Disable the ADC
 *
 * @param none
 *
 * @return none
 *
 *******************************************************************************/
void ADC_Disable(void)
{
  ADC->PWR.BF.GLOBAL_EN = 0; 
}

/****************************************************************************//**
 * @brief      Start the ADC
 *
 * @param none
 *
 * @return none
 *
 *******************************************************************************/
void ADC_ConversionStart(void)
{
  ADC->CMD.BF.CONV_START = 1; 
}

/****************************************************************************//**
 * @brief      Stop the ADC
 *
 * @param none
 *
 * @return none
 *
 *******************************************************************************/
void ADC_ConversionStop(void)
{
  ADC->CMD.BF.CONV_START = 0;
}

/****************************************************************************//**
 * @brief      Set the ADC channel
 *
 * @param[in]  adcChannelType:  ADC channel selection
 *
 * @return none
 *
 * Set the ADC channel
 *******************************************************************************/
void ADC_ChannelConfig(ADC_Channel_Type adcChannelType)
{
  /* single or differential mode */
  ADC->ANA.BF.SINGLEDIFF = (adcChannelType >> 4);
  
  /* Assign channel */
  ADC->ANA.BF.AMUX_SEL = (adcChannelType & 0xF);
}

/****************************************************************************//**
 * @brief      Get the ADC conversion final lower 16bit result
 *
 * @param none
 *
 * @return     ADC conversion final lower 16bit result
 *
 *******************************************************************************/
int16_t ADC_GetConversionResult(void)
{
  return (int16_t) ADC->RESULT.BF.DATA1;
}

/****************************************************************************//**
 * @brief      Select the operation mode --- ADC / Tsensor 
 *
 * @param[in]  mode: select the mode.
 *
 * @return     none
 *
 *******************************************************************************/
void ADC_ModeSelect(ADC_Mode_Type mode)
{
  ADC->ANA.BF.TS_EN = mode;
}

/****************************************************************************//**
 * @brief      Configure the ADC temparature sensor
 *
 * @param[in]  adcChannelType:  ADC temparature sensor channel selection
 * @param[in]  adcTSensorMode:  ADC temparature internal/external sensor mode selection
 *
 * @return none
 *
 * Configure the ADC temparature sensor
 *******************************************************************************/
void ADC_TSensorConfig(ADC_Channel_Type adcChannelType, ADC_TSensorMode_Type adcTSensorMode)
{
  /* single or differential mode */
  ADC->ANA.BF.SINGLEDIFF = (adcChannelType >> 4);
  
  /* Assign TSensor channel */
  ADC->ANA.BF.AMUX_SEL = (adcChannelType & 0xF);
  
  /* select the diode for Tsensor */
  ADC->ANA.BF.EXT_SEL = adcTSensorMode;
}

/****************************************************************************//**
 * @brief      Set ADC DMA mode 
 *
 * @param[in]  newCmd:  Enable/disable ADC DMA mode
 *
 * @return none
 *
 *******************************************************************************/
void ADC_DmaCmd(FunctionalState newCmd)
{  
  ADC->DMAR.BF.DMA_EN = newCmd;
}

/****************************************************************************//**
 * @brief      Set ADC buffer mode 
 *
 * @param[in]  adcInBuf:  ADC inbut gain buffer mode
 * @param[in]  adcVrefBuf:  ADC Vref buffer mode
 *
 * @return none
 *
 *******************************************************************************/
void ADC_SetBufferMode(FunctionalState adcInGainBuf, FunctionalState adcVrefBuf)
{
  /* Set input buffer mode */
  ADC->ANA.BF.IN_BFSEL = adcInGainBuf;
  
  /* Set reference buffer mode */
  ADC->ANA.BF.VREF_BFSEL = adcVrefBuf;
}

/****************************************************************************//**
 * @brief      Start ADC calibration 
 *
 * @param[in]  sysOffsetCalVal: system offset calibration value
 *
 * @return     ADC calibration status
 *
 *******************************************************************************/
Status ADC_Calibration(int16_t sysOffsetCalVal)
{
  uint32_t adcScale;
  
  uint32_t internalGainConst;
  uint32_t externalGainConst;
  
  uint32_t adcVrefSource;
  uint32_t adcResolution;
  uint32_t adcGainSel;
  uint32_t adcModeSel;
  
  uint8_t sysGainCal;
  uint32_t fullScale;
  
  /* save ANA DMACmd MASK register value*/
  uint32_t tempConfig;
  uint32_t tempDmaCmdConfig;
  uint32_t tempMaskConfig;
  
  Status statusOverflow;
  uint16_t adcResult1, adcResult2;
    
  statusOverflow = SUCCESS;
  
  adcResolution = ADC->ANA.BF.OSR;
  adcVrefSource = ADC->ANA.BF.VREF_SEL;
  adcGainSel = ADC->ANA.BF.BUF_GAIN;
  adcModeSel = ADC->ANA.BF.TS_EN;
  
  /* judge internal Vref or external Vref*/
  if( (adcVrefSource != ADC_VREF_EXTERNAL) || (ADC_EXT_VREF < 1200) )
  {
    sysGainCal = 0;
  }
  else
  {
    sysGainCal = 1;    
  }
  
  /* Adjust adc scale constant according to resolution */
  switch(adcResolution)
  {
    case ADC_RESOLUTION_16BIT:
      adcScale = ADC_SCALE_16B;
      break;
    case ADC_RESOLUTION_14BIT:
      adcScale = ADC_SCALE_14B;
      break;
    case ADC_RESOLUTION_12BIT:
      adcScale = ADC_SCALE_12B;
      break;
    case ADC_RESOLUTION_10BIT:
      adcScale = ADC_SCALE_10B;
      break;
    default:
      return ERROR;
  }
  
  /* if in temp_sensor mode, resolution = 14bit*/
  if(adcModeSel == ADC_MODE_TSENSOR)
  {
    adcScale = ADC_SCALE_14B;
  }
  
  /* when gain is 0.5, when Vref input, it value is half of full scale*/
  if(adcGainSel == ADC_GAIN_0P5)
  {
    adcScale = adcScale>>1;
  }
  
  /*calculate internal gain const and external gain const*/
  internalGainConst = adcScale << 15;
  fullScale = adcScale;
  if(sysGainCal)
  {
    externalGainConst = ((ADC_INT_VREF * adcScale) / ADC_EXT_VREF ) << 15;
  }
  
  
  /********************** system offset calibration *****************************/
  
  /* write system offset calibrition value */
  ADC->OFF_CAL.BF.SYS_CAL = sysOffsetCalVal;
    
  /********************** self offset calibration *****************************/
  
  /* Save ADC setting */
  tempConfig = ADC->ANA.WORDVAL;
  tempDmaCmdConfig = ADC->DMAR.WORDVAL;
  tempMaskConfig = ADC->IMR.WORDVAL;
  
  /* Stop Conversion */
  ADC_ConversionStop();
  
  /* mask all interrupt*/
  ADC_IntMask(ADC_INT_ALL,MASK);
  
  /* Disbale dma cmd*/
  ADC->DMAR.BF.DMA_EN = 0;
  
  /* adc mode*/
  ADC->ANA.BF.TS_EN = 0;
  
  /* if in temp_sensor mode, using 14 bit resolution*/
  if(adcModeSel == ADC_MODE_TSENSOR)
  {
    ADC->ANA.BF.OSR = ADC_RESOLUTION_14BIT;
  }
  
  /* Set ADC calibration mode */
  ADC->ANA.BF.CAL = 1;
  
  /* Assign channel VSSA for self offset calibration */
  ADC_ChannelConfig( ADC_VSSA);
  
  /* Set ADC PGA gain */
  if(adcGainSel == ADC_GAIN_2)
  {
    ADC->ANA.BF.BUF_GAIN = ADC_GAIN_1;
  }
  
  /* Clear GPADC_RDY_RAW */
  ADC_IntClr( ADC_RDY);
  
  /* Start ADC conversion */  
  ADC_ConversionStart();
  
  /* Waitting for ADC conversion done */
  while(ADC_GetStatus(ADC_STATUS_RDY) != SET);
  
  /* Stop Conversion */
  ADC_ConversionStop();
  
  ADC->ANA.BF.CAL = 0;
  
  /********************** gain calibration *****************************/
  
  /********************** self gain calibration ********************/  
  /* Assign channel VREF for self gain calibration */
  ADC_ChannelConfig( ADC_VREF);
  
  /* select internal 1.2V reference */
  ADC->ANA.BF.VREF_SEL = ADC_VREF_INTERNAL;
  
  /* Clear RDY_RAW */
  ADC_IntClr(ADC_RDY);
  
  /* Start ADC conversion */ 
  ADC_ConversionStart();
  
  /* Waitting for ADC conversion done */
  while(ADC_GetStatus(ADC_STATUS_RDY) != SET);
  
  /* Get ADC conversion result via self gain calibration */
  adcResult1 = ADC_GetConversionResult();
  
  /* Stop Conversion */
  ADC_ConversionStop();
  
  if ( adcResult1 < (fullScale >>1) )
  {
    /* skip the system calibration */
    statusOverflow = ERROR;
  }
  else
  {
    if(sysGainCal == 0)
    {
      /* Fill in gain calibration coefficient */
      ADC->GAIN_CAL.BF.GAIN_CAL = ((internalGainConst / adcResult1) & 0xFFFF);
    }
    else
    {
      /* calculate self gain calibration result */
      adcResult1 = ((internalGainConst / adcResult1) & 0xFFFF);
    
      /*********************** system gain calibration ***********************/  
      /* Assign channel VREF for self gain calibration */
      ADC_ChannelConfig(ADC_VREF);
  
      /* select external 1.25V reference */
      ADC->ANA.BF.VREF_SEL = ADC_VREF_EXTERNAL;
  
      /* Clear RDY_RAW */
      ADC_IntClr(ADC_RDY);
  
      /* Start ADC conversion */    
      ADC_ConversionStart();
  
      /* Waitting for ADC conversion done */
      while(ADC_GetStatus(ADC_STATUS_RDY) != SET);
  
      /* Get ADC conversion result via offset calibration */
      adcResult2 = ADC_GetConversionResult();
      
      /* Stop Conversion */
      ADC_ConversionStop();  
    
      /* calculate gain calibration coefficient */
      if ( adcResult2 < (fullScale >>1) )
      {
        statusOverflow = ERROR;
      }
      else
      {              
        /* calculate system gain calibration result */
        adcResult2 = ((externalGainConst / adcResult2) & 0xFFFF);
      
        if( ( (uint32_t) adcResult1 * (uint32_t) adcResult2 ) > 0x7FFFFFFF )
        { 
          statusOverflow = ERROR;
        }
        else
        {
          /* Fill in gain calibration coefficient */
          ADC->GAIN_CAL.BF.GAIN_CAL = (((uint32_t) adcResult1) * adcResult2)>>15;
        }
      }
    }    
  }
  
  /* clear all interrupt*/
  ADC_IntClr(ADC_INT_ALL);
  
  /* Restore ADC setting */
  ADC->ANA.WORDVAL = tempConfig;
  ADC->DMAR.WORDVAL = tempDmaCmdConfig;
  ADC->IMR.WORDVAL = tempMaskConfig;
  
  return statusOverflow;
}

/****************************************************************************//**
 * @brief      Enable/Disable event trigger signals to ADC 
 *
 * @param[in]  state:  enable/disable
 *
 * @return     none
 *
 *******************************************************************************/
void ADC_TriggerCmd(FunctionalState state)
{  
  ADC->CMD.BF.TRIGGER_EN = state;
}

/****************************************************************************//**
 * @brief      Select event trigger source for ADC 
 *
 * @param[in]  trigSrc:  select the trigger source
 *
 * @return     none
 *
 *******************************************************************************/
void ADC_TriggerSourceSel(ADC_TrigSource_Type trigSrc)
{
  ADC->CMD.BF.TRIGGER_SEL = trigSrc;
}

/****************************************************************************//**
 * @brief      Get ADC status 
 *
 * @param[in]  statusType:  Specifies the status type
 *
 * @return     State value of the specified ADC status type
 *
 *******************************************************************************/
FlagStatus ADC_GetStatus(ADC_Status_Type statusType)
{
  FlagStatus bitStatus = RESET;

  switch(statusType)
  {
    /* ADC ready status */
    case ADC_STATUS_RDY:    
      if (ADC->IRSR.BF.RDY_RAW == 1)
      {
        bitStatus = SET;
      }   
      break;

    /* ADC gain saturation status */
    case ADC_STATUS_GAINSAT:    
      if (ADC->IRSR.BF.GAINSAT_RAW == 1)
      {
        bitStatus = SET;
      }   
      break;
    
    /* ADC offset saturation status */
    case ADC_STATUS_OFFSAT:    
      if (ADC->IRSR.BF.OFFSAT_RAW == 1)
      {
        bitStatus = SET;
      }   
      break;      

    /* ADC dma error status */
    case ADC_STATUS_DMAERR:    
      if (ADC->IRSR.BF.DMA_ERR_RAW == 1)
      {
        bitStatus = SET;
      }   
      break; 
      
    /* ADC filter saturation status */
    case ADC_STATUS_FILTERSAT:    
      if (ADC->IRSR.BF.FILTERSAT_RAW == 1)
      {
        bitStatus = SET;
      }   
      break; 
      
    /* ADC active status */
    case ADC_STATUS_ACTIVE:
      if(ADC->STATUS.BF.ACT == 1)
      {
        bitStatus = SET;
      }
      break;      
      
    default:
      break;
  }

  return bitStatus;
}

/****************************************************************************//**
 * @brief      Get ADC interrupt status 
 *
 * @param[in]  intType:  Specifies the interrupt type
 *
 * @return     The state value of ADC interrupt status register
 *
 *******************************************************************************/
IntStatus ADC_GetIntStatus(ADC_INT_Type intType)
{
  IntStatus bitStatus = RESET;

  switch(intType)
  {
    /* ADC ready interrupt */
    case ADC_RDY:    
      if (ADC->ISR.BF.RDY == 1)
      {
        bitStatus = SET;
      }   
      break;

    /* ADC gain saturation interrupt */
    case ADC_GAINSAT:    
      if (ADC->ISR.BF.GAINSAT == 1)
      {
        bitStatus = SET;
      }   
      break;
      
    /* ADC offset saturation interrupt */
    case ADC_OFFSAT:    
      if (ADC->ISR.BF.OFFSAT == 1)
      {
        bitStatus = SET;
      }   
      break;      
 
    /* ADC dma error interrupt */
    case ADC_DMAERR:    
      if (ADC->ISR.BF.DMA_ERR == 1)
      {
        bitStatus = SET;
      }   
      break;  
     
    /* ADC filter saturation interrupt */
    case ADC_FILTERSAT:    
      if (ADC->ISR.BF.FILTERSAT == 1)
      {
        bitStatus = SET;
      }   
      break;     
      
    /* Any ADC interrupt */
    case ADC_INT_ALL:    
      if (ADC->ISR.WORDVAL & 0x1F)
      {
        bitStatus = SET;
      }   
      break;       
      
    default:
      break;
  }

  return bitStatus;
}

/****************************************************************************//**
 * @brief      Mask/Unmask specified ADC interrupt type 
 *
 * @param[in]  intType:  Specifies the interrupt type
 * @param[in]  intMask:  Mask/Unmask specified interrupt type
 *
 * @return none
 *
 *******************************************************************************/
void ADC_IntMask(ADC_INT_Type intType, IntMask_Type intMask)
{
  uint32_t cmdAll;
  
  if (intMask == UNMASK)
  {
    cmdAll = 0;
  }
  else
  {
    cmdAll = 0x1F;
  }  
  
  switch(intType)
  {
    /* ADC ready interrupt */
    case ADC_RDY:
      ADC->IMR.BF.RDY_MASK = intMask;
      break;
     
    /* ADC gain saturation interrupt */
    case ADC_GAINSAT:
      ADC->IMR.BF.GAINSAT_MASK = intMask;
      break;   
      
    /* ADC offset saturation interrupt */
    case ADC_OFFSAT:
      ADC->IMR.BF.OFFSAT_MASK = intMask;
      break;      
      
    /* ADC dma error interrupt */
    case ADC_DMAERR:
      ADC->IMR.BF.DMA_ERR_MASK = intMask;
      break;  
      
    /* ADC filter saturation interrupt */
    case ADC_FILTERSAT:
      ADC->IMR.BF.FILTERSAT_MASK = intMask;
      break;    

    /* All ADC interrupts */
    case ADC_INT_ALL:  
      ADC->IMR.WORDVAL = cmdAll;
      break; 
      
    default:
      break;
  }
}

 /****************************************************************************//**
 * @brief      Clear ADC interrupt flag 
 *
 * @param[in]  intType:  Specifies the interrupt type
 *
 * @return none
 *
 *******************************************************************************/
void ADC_IntClr(ADC_INT_Type intType)
{
  switch(intType)
  {
    /* ADC ready interrupt */
    case ADC_RDY:    
      ADC->ICR.BF.RDY_CLR = 1;
      break;
    
    /* ADC gain saturation interrupt */
    case ADC_GAINSAT:    
      ADC->ICR.BF.GAINSAT_CLR = 1;
      break;   
      
    /* ADC offset saturation interrupt */
    case ADC_OFFSAT:    
      ADC->ICR.BF.OFFSAT_CLR = 1;
      break;      

    /* ADC dma error interrupt */
    case ADC_DMAERR:    
      ADC->ICR.BF.DMA_ERR_CLR = 1;
      break;   
      
    /* ADC filter saturation interrupt */
    case ADC_FILTERSAT:    
      ADC->ICR.BF.FILTERSAT_CLR = 1;
      break;   

    /* All ADC interrupts */
    case ADC_INT_ALL:    
      ADC->ICR.WORDVAL = 0x1F;
      break;          
      
    default:
      break;
  }
}

/****************************************************************************//**
 * @brief      Select ADC audio pga output common mode
 *
 * @param[in]  voiceLevel:  common mode selection
 *
 * @return none
 *
 * Select adc audio pga common mode
 *******************************************************************************/
void ADC_AudioCommonSel(ADC_AudioCommon_Type commonMode)
{
  ADC->AUDIO.BF.PGA_CM = (commonMode & 0x7);
}
/****************************************************************************//**
 * @brief      Set ADC audio mode 
 *
 * @param[in]  newCmd:  Enable/disable ADC audio mode
 *
 * @return none
 *
 *******************************************************************************/
void ADC_AudioPgaCmd(FunctionalState newCmd)
{
  ADC->AUDIO.BF.EN = newCmd;
}

/****************************************************************************//**
 * @brief      Configure ADC audio with pre-defined audio configuration structure
 *
 * @param[in]  audioConfigSet:  Pointer to a audio configuration structure
 *
 * @return none
 *
 * Configure the ADC audio
 *******************************************************************************/
void ADC_AudioConfig(ADC_AudioCfg_Type* audioConfigSet)
{
  volatile uint32_t i;
  
  /* Set audio decimation rate */
  ADC->AUDIO.BF.OSR = audioConfigSet->audioDecimation;
  
  /* Set audio voltage gain */
  ADC->AUDIO.BF.PGA_GAIN = audioConfigSet->audioGain;
  
  /* Set ADC reference source */
  ADC->ANA.BF.VREF_SEL = audioConfigSet->adcVrefSource; 

  /* Set ADC PGA gain */
  ADC->ANA.BF.BUF_GAIN = audioConfigSet->adcGainSel; 
  
  /* Set ADC internal clock divider */
  ADC->CLKRST.BF.SOFT_CLK_RST = 1;
  
  /* Delay */
  for(i=0; i<10; i++);
  
  ADC->CLKRST.BF.SOFT_CLK_RST = 0;
  
  ADC->CLKRST.BF.INT_CLK_DIV = audioConfigSet->adcClockDivider;
  
  /* Set ADC bias mode */
  ADC->ANA.BF.BIAS_SEL = audioConfigSet->adcBiasMode;
}

/****************************************************************************//**
 * @brief      Select ADC audio voice level
 *
 * @param[in]  voiceLevel:  voice level selection
 *
 * @return none
 *
 * Select adc voice level
 *******************************************************************************/
void ADC_VoiceLevelSel(ADC_VoiceLevel_Type voiceLevel)
{
  ADC->VOICE_DET.BF.LEVEL_SEL = (voiceLevel & 0x3);
}

/****************************************************************************//**
 * @brief      Set ADC voice level detection mode 
 *
 * @param[in]  newCmd:  Enable/disable ADC voive level detection mode
 *
 * @return none
 *
 *******************************************************************************/
void ADC_VoiceLevelDetectCmd(FunctionalState newCmd)
{
  ADC->VOICE_DET.BF.DET_EN = newCmd;
}

/****************************************************************************//**
 * @brief      Select ADC final data buffer width
 *
 * @param[in]  widthType:  result width selection
 *
 * @return none
 *
 * Select adc final data width
 *******************************************************************************/
void ADC_ResultWidthConfig(ADC_ResultWidth_Type widthType)
{
  ADC->RESULT_BUF.BF.WIDTH_SEL = (widthType & 0x1);
}

/****************************************************************************//**
 * @brief      Get the ADC conversion final 32bit result
 *
 * @param none
 *
 * @return     ADC conversion final 32bit result
 *
 *******************************************************************************/
int32_t ADC_GetConversion32bitResult(void)
{
  return (int32_t) ADC->RESULT.WORDVAL;
}
