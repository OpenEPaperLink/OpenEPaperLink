#include "mz100.h"
#include "mz100_driver.h"
#include "mz100_ssp.h"

/**
 *  @brief SSP1 and SSP2 address array
 */
static const uint32_t sspAddr[2] = {SSP1_BASE,SSP2_BASE};

/*@} end of group SSP_Private_Variables */

/** @defgroup SSP_Global_Variables
 *  @{
 */

/*@} end of group SSP_Global_Variables */

/** @defgroup SSP_Private_FunctionDeclaration
 *  @{
 */
void SSP_IntHandler(INT_Peripher_Type intPeriph, SSP_ID_Type sspNo);

/*@} end of group SSP_Private_FunctionDeclaration */

/** @defgroup SSP_Private_Functions
 *  @{
 */

/****************************************************************************//**
 * @brief      SSP interrupt handle
 *
 * @param[in]  intPeriph: Select the peripheral, such as INT_SSP1,INT_SSP2
 * @param[in]  sspNo:  Select the SSP port
 *
 * @return none
 *
 *******************************************************************************/
void SSP_IntHandler(INT_Peripher_Type intPeriph, SSP_ID_Type sspNo)
{
  uint32_t statusVal;

  ssp_reg_t * SSPx = (ssp_reg_t *)(sspAddr[sspNo]);

  statusVal = SSPx->SSSR.WORDVAL;

  /* Transmit fifo underrun interrupt */
  if(statusVal & (0x01<<21))
  {
    if(intCbfArra[intPeriph][SSP_INT_TFURI] != NULL)
    {
      /* call the callback function */
      intCbfArra[intPeriph][SSP_INT_TFURI]();
    }
    /* Disable the interrupt if callback function is not setup */
    else
    {
      SSPx->SSCR0.BF.TIM = 1;
    }
  }

  /* Receive fifo overrun interrupt */
  if(statusVal & (0x01<<7))
  {
    if(intCbfArra[intPeriph][SSP_INT_RFORI] != NULL)
    {
      /* call the callback function */
      intCbfArra[intPeriph][SSP_INT_RFORI]();
    }
    /* Disable the interrupt if callback function is not setup */
    else
    {
      SSPx->SSCR0.BF.RIM = 1;
    }
  }

  /* Bit count error inerrupt */
  if(statusVal & (0x01<<23))
  {
    if(intCbfArra[intPeriph][SSP_INT_BCEI] != NULL)
    {
      /* call the callback function */
      intCbfArra[intPeriph][SSP_INT_BCEI]();
    }
    /* Disable the interrupt if callback function is not setup */
    else
    {
      SSPx->SSCR1.BF.EBCEI = 0;
    }
  }

  /* Receive timeout interrupt indication */
  if(statusVal & (0x01<<19))
  {
    if(intCbfArra[intPeriph][SSP_INT_RTOI] != NULL)
    {
      /* call the callback function */
      intCbfArra[intPeriph][SSP_INT_RTOI]();
    }
    /* Disable the interrupt if callback function is not setup */
    else
    {
      SSPx->SSCR1.BF.TINTE = 0;
    }
  }

  /* Transmit Fifo Empty interrupt */
  if(statusVal & (0x01<<5))
  {
    if(intCbfArra[intPeriph][SSP_INT_TFEI] != NULL)
    {
      /* call the callback function */
      intCbfArra[intPeriph][SSP_INT_TFEI]();
    }
    /* Disable the interrupt if callback function is not setup */
    else
    {
      SSPx->SSCR1.BF.TIE = 0;
    }
  }
  /* Receive Fifo Full interrupt */
  if(statusVal & (0x01<<6))
  {
    if(intCbfArra[intPeriph][SSP_INT_RFFI] != NULL)
    {
      /* call the callback function */
      intCbfArra[intPeriph][SSP_INT_RFFI]();
    }
    /* Disable the interrupt if callback function is not setup */
    else
    {
      SSPx->SSCR1.BF.RIE = 0;
    }
  }
}
/*@} end of group SSP_Private_Functions */

/** @defgroup SSP_Public_Functions
 *  @{
 */

/****************************************************************************//**
 * @brief      Initializes the SSP
 *
 * @param[in]  sspNo:  Select the SSP port,should be SSP1_ID,SSP2_ID.
 * @param[in]  sspCfgStruct:  Pointer to a SSP configuration structure
 *
 * @return none
 *
 *******************************************************************************/
void SSP_Init(SSP_ID_Type sspNo, SSP_CFG_Type* sspCfgStruct)
{
  ssp_reg_t * SSPx = (ssp_reg_t *)(sspAddr[sspNo]);

  /* check the parameters */
  CHECK_PARAM(IS_SSP_PERIPH(sspNo));
  CHECK_PARAM(IS_SSP_FRAMEFORMAT(sspCfgStruct->frameFormat));
  CHECK_PARAM(IS_SSP_MS(sspCfgStruct->masterOrSlave));
  CHECK_PARAM(IS_SSP_TRTYPE(sspCfgStruct->trMode));
  CHECK_PARAM(IS_SSP_DATASIZE(sspCfgStruct->dataSize));
  CHECK_PARAM(IS_SSP_FRMPOLARITY(sspCfgStruct->sfrmPola));
  CHECK_PARAM(IS_SSP_SLAVECLK(sspCfgStruct->slaveClkRunning));
  CHECK_PARAM(IS_SSP_TRAILBYTE_MODE(sspCfgStruct->trailByte));

  /* configurate ssp mode */
  SSPx->SSCR0.BF.MOD = sspCfgStruct->mode;

  if(SSP_NETWORK == SSPx->SSCR0.BF.MOD)
  {
    /* Configurate PSP start delay time = 0 */
    SSPx->SSPSP.BF.STRTDLY = 0;

    /* Configurate PSP dummy start time = 0 */
    SSPx->SSPSP.BF.EDMYSTRT = 0;
    SSPx->SSPSP.BF.DMYSTRT = 0;

    /* Configurate PSP dummy stop time = 0 */
    SSPx->SSPSP.BF.EDMYSTOP = 0;
    SSPx->SSPSP.BF.DMYSTOP = 0;

    /* Configurate PSP serial frame delay and frame width time = 0 */
    SSPx->SSPSP.BF.SFRMDLY = 0;
  }

  /* config trail byte handle mode */
  SSPx->SSCR1.BF.TRAIL = sspCfgStruct->trailByte;

  /* configurate ssp data format */
  SSPx->SSCR0.BF.FRF = sspCfgStruct->frameFormat;

  /* configurate ssp function mode: Master or Slave */
  SSPx->SSCR1.BF.SFRMDIR = sspCfgStruct->masterOrSlave;
  SSPx->SSCR1.BF.SCLKDIR = sspCfgStruct->masterOrSlave;

  /* configurate ssp receive without transmit bit */
  SSPx->SSCR1.BF.RWOT = sspCfgStruct->trMode;

  /* configurate ssp data size */
  switch(sspCfgStruct->dataSize)
  {
    case SSP_DATASIZE_8:
      SSPx->SSCR0.BF.EDSS = 0;
      SSPx->SSCR0.BF.DSS = 0x7;
      break;
    case SSP_DATASIZE_16:
      SSPx->SSCR0.BF.EDSS = 0;
      SSPx->SSCR0.BF.DSS = 0xf;
      break;
    case SSP_DATASIZE_18:
      SSPx->SSCR0.BF.EDSS = 1;
      SSPx->SSCR0.BF.DSS = 0x1;
      break;
    case SSP_DATASIZE_32:
      SSPx->SSCR0.BF.EDSS = 1;
      SSPx->SSCR0.BF.DSS = 0xf;
      break;
  }

  /* configurate ssp frame polarity type */
  SSPx->SSCR1.BF.IFS = sspCfgStruct->sfrmPola;

  /* configurate ssp slave serial clock running type */
  SSPx->SSCR1.BF.SCFR = sspCfgStruct->slaveClkRunning;

  /* configurate when ssp txd change to 3 state after lsb beginning */
  SSPx->SSCR1.BF.TTE = sspCfgStruct->txd3StateEnable;
  SSPx->SSCR1.BF.TTELP = sspCfgStruct->txd3StateType;
  SSPx->SSTO.BF.TIMEOUT = sspCfgStruct->timeOutVal;
}

/****************************************************************************//**
 * @brief      Config SSP FIFO Parameter
 *
 * @param[in]  sspNo:  Select the SSP port,should be SSP1_ID,SSP2_ID.
 * @param[in]  sspFifoCfg:  Pointer to a SSP_FIFO_Type Structure
 *
 * @return none
 *
 *******************************************************************************/
void SSP_FifoConfig(SSP_ID_Type sspNo, SSP_FIFO_Type* sspFifoCfg)
{
  ssp_reg_t * SSPx = (ssp_reg_t *)(sspAddr[sspNo]);

  /* check the parameters */
  CHECK_PARAM(IS_SSP_PERIPH(sspNo));

  /* Enable(1) or disable(0) fifo packed mode */
  SSPx->SSCR0.BF.FPCKE = sspFifoCfg->fifoPackMode;

  /* Configurate rx fifo and tx fifo threshold */
  SSPx->SSCR1.BF.RFT = sspFifoCfg->rxFifoFullLevel;
  SSPx->SSCR1.BF.TFT = sspFifoCfg->txFifoEmptyLevel;

  /* Enable or disable transmit/receive dma service request */
  SSPx->SSCR1.BF.RSRE = sspFifoCfg->rxDmaService;
  SSPx->SSCR1.BF.TSRE = sspFifoCfg->txDmaService;
}

/****************************************************************************//**
 * @brief      Config SSP Network type Parameter, only used in Nerwork Mode
 *
 * @param[in]  sspNo:  Select the SSP port,should be SSP1_ID,SSP2_ID.
 * @param[in]  sspNwkCfg:  Pointer to a SSP_NWK_Type Structure
 *
 * @return none
 *
 *******************************************************************************/
void SSP_NwkConfig(SSP_ID_Type sspNo, SSP_NWK_Type* sspNwkCfg)
{
  ssp_reg_t * SSPx = (ssp_reg_t *)(sspAddr[sspNo]);

  /* check the parameters */
  CHECK_PARAM(IS_SSP_PERIPH(sspNo));

  /* Frame Rate Divider Control */
  SSPx->SSCR0.BF.FRDC = sspNwkCfg->frameRateDiv;

  /* configurate in which time slot ssp transmit data */
  SSPx->SSTSA.BF.TTSA = sspNwkCfg->txTimeSlotActive;

  /* configurate in which time slot ssp receive data */
  SSPx->SSRSA.BF.RTSA = sspNwkCfg->rxTimeSlotActive;
}

/****************************************************************************//**
 * @brief      Config SPI format specifical Parameter
 *
 * @param[in]  sspNo:  Select the SSP port,should be SSP1_ID,SSP2_ID.
 * @param[in]  spiParaStruct:  Pointer to a SPI_Param_Type Structure
 *
 * @return none
 *
 *******************************************************************************/
void SPI_Config(SSP_ID_Type sspNo, SPI_Param_Type* spiParaStruct)
{
  ssp_reg_t * SSPx = (ssp_reg_t *)(sspAddr[sspNo]);

  /* check the parameters */
  CHECK_PARAM(IS_SSP_PERIPH(sspNo));
  CHECK_PARAM(IS_SPI_CLKPHASE(spiParaStruct->spiClkPhase));
  CHECK_PARAM(IS_SPI_CLKPOLARITY(spiParaStruct->spiClkPolarity));

  /* Configurate SPI serial clock phase and polarity */
  SSPx->SSCR1.BF.SPH = spiParaStruct->spiClkPhase;
  SSPx->SSCR1.BF.SPO = spiParaStruct->spiClkPolarity;
}

/****************************************************************************//**
 * @brief      Config PSP format specifical Parameter
 *
 * @param[in]  sspNo:  Select the SSP port,should be SSP1_ID,SSP2_ID.
 * @param[in]  pspParaStruct:  Pointer to a SPI_Param_Type Structure
 *
 * @return none
 *
 *******************************************************************************/
void PSP_Config(SSP_ID_Type sspNo, PSP_Param_Type* pspParaStruct)
{
  ssp_reg_t * SSPx = (ssp_reg_t *)(sspAddr[sspNo]);

  /* check the parameters */
  CHECK_PARAM(IS_SSP_PERIPH(sspNo));
  CHECK_PARAM(IS_PSP_FSRTTYPE(pspParaStruct->pspFsrtType));
  CHECK_PARAM(IS_PSP_CLKMODE(pspParaStruct->pspClkMode));
  CHECK_PARAM(IS_PSP_FRMPOLARITY(pspParaStruct->pspFrmPola));
  CHECK_PARAM(IS_PSP_ENDTRANSTYPE(pspParaStruct->pspEndTransState));

  /* Configurate PSP clock and frame parameter */
  SSPx->SSPSP.BF.FSRT = pspParaStruct->pspFsrtType;    /* Set frame sync relative timing bit */
  SSPx->SSPSP.BF.SCMODE = pspParaStruct->pspClkMode;   /* PSP serial clock mode */
  SSPx->SSPSP.BF.SFRMP = pspParaStruct->pspFrmPola;    /* PSP serial frame polarity */
  SSPx->SSPSP.BF.ETDS = pspParaStruct->pspEndTransState; /* PSP end of transfer data state */

  if(SSP_NORMAL == SSPx->SSCR0.BF.MOD)
  {
    /* Configurate PSP start delay time */
    SSPx->SSPSP.BF.STRTDLY = pspParaStruct->startDelay;

    /* Configurate PSP dummy start time */
    SSPx->SSPSP.BF.EDMYSTRT = ((pspParaStruct->dummyStart)&(0x0c))>>2;
    SSPx->SSPSP.BF.DMYSTRT = (pspParaStruct->dummyStart)&(0x03);

    /* Configurate PSP dummy stop time */
    SSPx->SSPSP.BF.EDMYSTOP = ((pspParaStruct->dummyStop)&(0x0c))>>2;
    SSPx->SSPSP.BF.DMYSTOP = (pspParaStruct->dummyStop)&(0x03);

    /* Configurate PSP serial frame delay and frame width time */
    SSPx->SSPSP.BF.SFRMDLY = pspParaStruct->frmDelay;
  }
  SSPx->SSPSP.BF.SFRMWDTH = pspParaStruct->frmLength;
}

/****************************************************************************//**
 * @brief   SSP exit from network mode
 *
 * @param[in]  sspNo:  Select the SSP port,should be SSP1_ID,SSP2_ID.
 *
 * @return "ERROR" or "SUCCESS"
 *
 *******************************************************************************/
Status PSP_ExitFromNwk(SSP_ID_Type sspNo)
{
  ssp_reg_t * SSPx = (ssp_reg_t *)(sspAddr[sspNo]);

  /* check the parameters */
  CHECK_PARAM(IS_SSP_PERIPH(sspNo));

  /* clear SSP_SSCR0[MOD] bit, SSP_SSCR0[SSE] doesn't need to change */
  SSPx->SSCR0.BF.MOD = SSP_NORMAL;

  if(SSP_NWK_BUSY == SSPx->SSTSS.BF.NMBSY)
  {
    /* return ERROR if network mode is busy */
    return ERROR;
  }
  else
  {
    /* disable ssp if network mode is free */
    SSPx->SSCR0.BF.SSE = 0;

    return SUCCESS;
  }
}

/****************************************************************************//**
 * @brief      Enable SSP module
 *
 * @param[in]  sspNo:  Select the SSP port,should be SSP1_ID,SSP2_ID.
 *
 * @return none
 *
 *******************************************************************************/
void SSP_Enable(SSP_ID_Type sspNo)
{
  ssp_reg_t * SSPx = (ssp_reg_t *)(sspAddr[sspNo]);

  /* check the parameters */
  CHECK_PARAM(IS_SSP_PERIPH(sspNo));

  /* enable ssp */
  SSPx->SSCR0.BF.SSE = 1;
}

/****************************************************************************//**
 * @brief      Disable SSP module
 *
 * @param[in]  sspNo:  Select the SSP port,should be SSP1_ID,SSP2_ID.
 *
 * @return none
 *
 *******************************************************************************/
void SSP_Disable(SSP_ID_Type sspNo)
{
  ssp_reg_t * SSPx = (ssp_reg_t *)(sspAddr[sspNo]);

  /* check the parameters */
  CHECK_PARAM(IS_SSP_PERIPH(sspNo));

  /* disable ssp */
  SSPx->SSCR0.BF.SSE = 0;
}

/****************************************************************************//**
 * @brief      Send one "data size" length data to SSP peripheral
 *
 * @param[in]  sspNo:  Select the SSP port,should be SSP1_ID,SSP2_ID.
 * @param[in]  data:  The data to be send
 *
 * @return none
 *
 *******************************************************************************/
void SSP_SendData(SSP_ID_Type sspNo, uint32_t data)
{
  ssp_reg_t * SSPx = (ssp_reg_t *)(sspAddr[sspNo]);

  /* check the parameters */
  CHECK_PARAM(IS_SSP_PERIPH(sspNo));

  /* Send data */
  SSPx->SSDR.WORDVAL = data;
}

/****************************************************************************//**
 * @brief      Receive one "data size" length data from the SSP peripheral
 *
 * @param[in]  sspNo:  Select the SSP port,should be SSP1_ID,SSP2_ID.
 *
 * @return     The received data
 *
 *******************************************************************************/
uint32_t SSP_RecvData(SSP_ID_Type sspNo)
{
  ssp_reg_t * SSPx = (ssp_reg_t *)(sspAddr[sspNo]);

  /* check the parameters */
  CHECK_PARAM(IS_SSP_PERIPH(sspNo));

  /* Receive data */
  return (uint32_t)(SSPx->SSDR.BF.DATA);
}

/****************************************************************************//**
 * @brief      Set SSP loopback mode
 *
 * @param[in]  sspNo:  Select the SSP port,should be SSSP1_ID,SSP2_ID.
 * @param[in]  newState: Enable/Disable function state
 *
 * @return none
 *
 *******************************************************************************/
void SSP_SetLoopBackMode(SSP_ID_Type sspNo, FunctionalState newState)
{
  ssp_reg_t * SSPx = (ssp_reg_t *)(sspAddr[sspNo]);

  /* check the parameters */
  CHECK_PARAM(IS_SSP_PERIPH(sspNo));

  /* Configurate the loopback */
  SSPx->SSCR1.BF.LBM = ((newState == ENABLE)? 0x01 : 0x00);
}

/****************************************************************************//**
 * @brief      Get SSP Transmit Fifo Level
 *
 * @param[in]  sspNo:  Select the SSP port,should be SSP1_ID,SSP2_ID.
 *
 * @return none
 *
 *******************************************************************************/
uint32_t SSP_GetTxFifoLevel(SSP_ID_Type sspNo)
{
  ssp_reg_t * SSPx = (ssp_reg_t *)(sspAddr[sspNo]);

  /* check the parameters */
  CHECK_PARAM(IS_SSP_PERIPH(sspNo));

  /* Return ssp tx fifo level */
  return(SSPx->SSSR.BF.TFL);
}

/****************************************************************************//**
 * @brief      Get SSP Receive Fifo Level
 *
 * @param[in]  sspNo:  Select the SSP port,should be SSP1_ID,SSP2_ID.
 *
 * @return none
 *
 *******************************************************************************/
uint32_t SSP_GetRxFifoLevel(SSP_ID_Type sspNo)
{
  ssp_reg_t * SSPx = (ssp_reg_t *)(sspAddr[sspNo]);

  /* check the parameters */
  CHECK_PARAM(IS_SSP_PERIPH(sspNo));

  /* Return ssp rx fifo level */
  return(SSPx->SSSR.BF.RFL);
}

/****************************************************************************//**
 * @brief      Mask/Unmask the SSP interrupt
 *
 * @param[in]  sspNo:  Select the SSP port,should be SSP1_ID,SSP2_ID.
 * @param[in]  intType:  Specifies the interrupt type
 * @param[in]  intMask:  MASK or UNMASK Specified interrupt type
 *
 * @return none
 *
 *******************************************************************************/
void SSP_IntMask(SSP_ID_Type sspNo, SSP_INT_Type intType, IntMask_Type intMask)
{
  ssp_reg_t * SSPx = (ssp_reg_t *)(sspAddr[sspNo]);

  /* check the parameters */
  CHECK_PARAM(IS_SSP_PERIPH(sspNo));
  CHECK_PARAM(IS_SSP_INT_TYPE(intType));

  switch(intType)
  {
    case SSP_INT_TFURI:
      SSPx->SSCR0.BF.TIM = ((intMask == MASK)? 1 : 0);
      break;

    case SSP_INT_RFORI:
      SSPx->SSCR0.BF.RIM = ((intMask == MASK)? 1 : 0);
      break;

    case SSP_INT_BCEI:
      SSPx->SSCR1.BF.EBCEI = ((intMask == MASK)? 0 : 1);
      break;

    case SSP_INT_RTOI:
      SSPx->SSCR1.BF.TINTE = ((intMask == MASK)? 0 : 1);
      break;

    case SSP_INT_TFEI:
      SSPx->SSCR1.BF.TIE = ((intMask == MASK)? 0 : 1);
      break;

    case SSP_INT_RFFI:
      SSPx->SSCR1.BF.RIE = ((intMask == MASK)? 0 : 1);
      break;

    case SSP_INT_ALL:
      if(intMask == MASK)
      {
        SSPx->SSCR0.BF.TIM = 1;
        SSPx->SSCR0.BF.RIM = 1;
        SSPx->SSCR1.BF.EBCEI = 0;
        SSPx->SSCR1.BF.TINTE = 0;
        SSPx->SSCR1.BF.TIE = 0;
        SSPx->SSCR1.BF.RIE = 0;
      }
      else
      {
        SSPx->SSCR0.BF.TIM = 0;
        SSPx->SSCR0.BF.RIM = 0;
        SSPx->SSCR1.BF.EBCEI = 1;
        SSPx->SSCR1.BF.TINTE = 1;
        SSPx->SSCR1.BF.TIE = 1;
        SSPx->SSCR1.BF.RIE = 1;
      }
      break;

    default:
      break;
  }
}

/****************************************************************************//**
 * @brief      Clear the SSP interrupt
 *
 * @param[in]  sspNo:  Select the SSP port,should be SSP1_ID,SSP2_ID.
 * @param[in]  intType:  only support "SSP_INT_TFURI", "SSP_INT_RFORI",
 *                                    "SSP_INT_BCEI", "SSP_INT_RTOI"
 * @return "Status" type: 'SET'->SUCCESS, 'RESET'->FAIL
 *
 *******************************************************************************/
Status SSP_IntClr(SSP_ID_Type sspNo, SSP_INT_Type intType)
{
  Status bitStatus = ERROR;
  ssp_reg_t * SSPx = (ssp_reg_t *)(sspAddr[sspNo]);

  /* check the parameters */
  CHECK_PARAM(IS_SSP_PERIPH(sspNo));
  CHECK_PARAM(IS_SSP_INT_TYPE(intType));

  switch(intType)
  {
    case SSP_INT_BCEI:
      SSPx->SSSR.WORDVAL &= 0xffd7ff7f;
      bitStatus = SUCCESS;
      break;

    case SSP_INT_TFURI:
      SSPx->SSSR.WORDVAL &= 0xff77ff7f;
      bitStatus = SUCCESS;
      break;

    case SSP_INT_RTOI:
      SSPx->SSSR.WORDVAL &= 0xff5fff7f;
      bitStatus = SUCCESS;
      break;

    case SSP_INT_RFORI:
      SSPx->SSSR.WORDVAL &= 0xff57ffff;
      bitStatus = SUCCESS;
      break;

    default:
      bitStatus = ERROR;
      break;
  }
  return(bitStatus);
}

/****************************************************************************//**
 * @brief      Get current value of SSP Status register
 *
 * @param[in]  sspNo:  Select the SSP port,should be SSP1_ID,SSP2_ID.
 * @param[in]  sspStatus:  Checks whether the specified ssp status bit be set or not
 *
 * @return: SET or RESET
 *
 *******************************************************************************/
FlagStatus SSP_GetStatus(SSP_ID_Type sspNo, SSP_Status_Type sspStatus)
{
  FlagStatus bitStatus = RESET;
  ssp_reg_t * SSPx = (ssp_reg_t *)(sspAddr[sspNo]);

  /* check the parameters */
  CHECK_PARAM(IS_SSP_PERIPH(sspNo));
  CHECK_PARAM(IS_SSP_STATUS_TYPE(sspStatus));

  switch(sspStatus)
  {
    case SSP_STATUS_RXOSS:
      bitStatus = ((SSPx->SSSR.BF.OSS == 1) ? SET : RESET);
      break;

    case SSP_STATUS_TXOSS:
      bitStatus = ((SSPx->SSSR.BF.TX_OSS == 1) ? SET : RESET);
      break;

    case SSP_STATUS_BCE:
      bitStatus = ((SSPx->SSSR.BF.BCE == 1) ? SET : RESET);
      break;

    case SSP_STATUS_CSS:
      bitStatus = ((SSPx->SSSR.BF.CSS == 1) ? SET : RESET);
      break;

    case SSP_STATUS_TFUR:
      bitStatus = ((SSPx->SSSR.BF.TUR == 1) ? SET : RESET);
      break;

    case SSP_STATUS_RTOI:
      bitStatus = ((SSPx->SSSR.BF.TINT == 1) ? SET : RESET);
      break;

    case SSP_STATUS_RFOR:
      bitStatus = ((SSPx->SSSR.BF.ROR == 1) ? SET : RESET);
      break;

    case SSP_STATUS_RFF:
      bitStatus = ((SSPx->SSSR.BF.RFS == 1) ? SET : RESET);
      break;

    case SSP_STATUS_TFE:
      bitStatus = ((SSPx->SSSR.BF.TFS == 1) ? SET : RESET);
      break;

    case SSP_STATUS_BUSY:
      bitStatus = ((SSPx->SSSR.BF.BSY == 1) ? SET : RESET);
      break;

    case SSP_STATUS_RFNE:
      bitStatus = ((SSPx->SSSR.BF.RNE == 1) ? SET : RESET);
      break;

    case SSP_STATUS_TFNF:
      bitStatus = ((SSPx->SSSR.BF.TNF == 1) ? SET : RESET);
      break;

    default:
      break;
  }

  return bitStatus;
}

/****************************************************************************//**
 * @brief      Get current SSP Network status
 *
 * @param[in]  sspNo:  Select the SSP port,should be SSP1_ID,SSP2_ID.
 *
 * @return: SSP_NWK_STAT type "SSP_NWK_FREE" or "SSP_NWK_BUSY"
 *
 *******************************************************************************/
SSP_NWK_Stat SSP_GetNwkStatus(SSP_ID_Type sspNo)
{
  ssp_reg_t * SSPx = (ssp_reg_t *)(sspAddr[sspNo]);

  /* check the parameters */
  CHECK_PARAM(IS_SSP_PERIPH(sspNo));

  return((SSP_NWK_Stat)SSPx->SSTSS.BF.NMBSY);
}

/****************************************************************************//**
 * @brief      Get current SSP Active Time Slot
 *
 * @param[in]  sspNo:  Select the SSP port,should be SSP1_ID,SSP2_ID.
 *
 * @return: value '0'-'7' indicate current active time slot
 *
 *******************************************************************************/
uint8_t SSP_GetActiveTimeSlot(SSP_ID_Type sspNo)
{
  ssp_reg_t * SSPx = (ssp_reg_t *)(sspAddr[sspNo]);

  /* check the parameters */
  CHECK_PARAM(IS_SSP_PERIPH(sspNo));

  return(SSPx->SSTSS.BF.TSS);
}

/****************************************************************************//**
 * @brief  SSP1 interrupt function
 *
 * @param  none
 *
 * @return none
 *
 *******************************************************************************/
__attribute__((weak)) void SSP1_IRQHandler(void)
{
   SSP_IntHandler(INT_SSP1, SSP1_ID);
}

/****************************************************************************//**
 * @brief  SSP2 interrupt function
 *
 * @param  none
 *
 * @return none
 *
 *******************************************************************************/
__attribute__((weak)) void SSP2_IRQHandler(void)
{
   SSP_IntHandler(INT_SSP2, SSP2_ID);
}
