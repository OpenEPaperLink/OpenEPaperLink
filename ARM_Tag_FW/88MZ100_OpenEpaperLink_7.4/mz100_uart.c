#include "mz100_uart.h"
#include "mz100.h"
#include "mz100_driver.h"
#include "mz100_clock.h"

/** @addtogroup  88MZ100_Periph_Driver
 *  @{
 */

/** @defgroup UART UART
 *  @brief UART driver modules
 *  @{
 */

/** @defgroup UART_Private_Type
 *  @{
 */

/*@} end of group UART_Private_Type*/

/** @defgroup UART_Private_Defines
 *  @{
 */

/**
 *  @brief UART interrupt IID definition
 */
#define UART_INTSTATUS_IID_MODEM       0x00        
#define UART_INTSTATUS_IID_TBEI        0x02
#define UART_INTSTATUS_IID_RBFI        0x04        
#define UART_INTSTATUS_IID_LSI         0x06        
#define UART_INTSTATUS_IID_BYDET       0x07
#define UART_INTSTATUS_IID_RCVRTO      0x0C

/*@} end of group UART_Private_Defines */

/** @defgroup UART_Private_Variables
 *  @{
 */

/**
 *  @brief UART1 and UART2 address array
 */
static const uint32_t uartAddr[2] = {UART1_BASE,UART2_BASE};

/*@} end of group UART_Private_Variables */

/** @defgroup UART_Global_Variables
 *  @{
 */

/*@} end of group UART_Global_Variables */

/** @defgroup UART_Private_FunctionDeclaration
 *  @{
 */
void UART_IntHandler(INT_Peripher_Type intPeriph, UART_ID_Type uartNo);

/*@} end of group UART_Private_FunctionDeclaration */

/** @defgroup UART_Private_Functions
 *  @{
 */

/****************************************************************************//**
 * @brief      UART interrupt handle
 *
 * @param[in]  intPeriph: Select the peripheral, such as INT_UART1,INT_AES
 * @param[in]  uartNo:  Select the UART port
 *
 * @return none
 *
 *******************************************************************************/
void UART_IntHandler(INT_Peripher_Type intPeriph, UART_ID_Type uartNo)
{
  uint32_t intId;
  volatile uint32_t dummyData;

  uart_reg_t * UARTx = (uart_reg_t *)(uartAddr[uartNo]);

  /* Check the parameters */
  CHECK_PARAM(IS_UART_ID(uartNo));

  intId = UARTx->IIR_FCR.BF_IIR.IID;

  /* Transmit holding register empty inerrupt */
  if(intId == UART_INTSTATUS_IID_TBEI)
  {
    if(intCbfArra[intPeriph][UART_INTSTATUS_TBEI] != NULL)
    {
      /* call the callback function */
      intCbfArra[intPeriph][UART_INTSTATUS_TBEI]();
    }
    /* Disable the interrupt if callback function is not setup */
    else
    {
      UARTx->IER_DLH.BF_IER.ETBEI = 0;
    }
  }

  /* Mode status inerrupt */
  if(intId == UART_INTSTATUS_IID_MODEM)
  {
    /* Clear it by reading the UART modem status register */
    dummyData = UARTx->MSR.WORDVAL;

    if(intCbfArra[intPeriph][UART_INTSTATUS_MODEM] != NULL)
    {
      /* call the callback function */
      intCbfArra[intPeriph][UART_INTSTATUS_MODEM]();
    }
    /* Disable the interrupt if callback function is not setup */
    else
    {
      UARTx->IER_DLH.BF_IER.EDSSI = 0;
    }
  }

  /* Receiver line status inerrupt */
  if(intId == UART_INTSTATUS_IID_LSI)
  {
    if(intCbfArra[intPeriph][UART_INTSTATUS_LSI] != NULL)
    {
      /* call the callback function */
      intCbfArra[intPeriph][UART_INTSTATUS_LSI]();
    }
    /* Disable the interrupt if callback function is not setup */
    else
    {
      UARTx->IER_DLH.BF_IER.ELSI = 0;
    }

    /* Clear it by reading the UART status register */
    dummyData = UARTx->LSR.WORDVAL;
  }

  /* Received data available interrupt or timeout indication */
  if(intId == UART_INTSTATUS_IID_RBFI)
  {
    if(intCbfArra[intPeriph][UART_INTSTATUS_RBFI] != NULL)
    {
      /* call the callback function */
      intCbfArra[intPeriph][UART_INTSTATUS_RBFI]();
    }
    /* Disable the interrupt if callback function is not setup */
    else
    {
      UARTx->IER_DLH.BF_IER.ERBFI = 0;
    }
  }

  /* Character timeout indication */
  if(intId == UART_INTSTATUS_IID_RCVRTO)
  {
    if(intCbfArra[intPeriph][UART_INTSTATUS_RCVRTO] != NULL)
    {
      /* call the callback function */
      intCbfArra[intPeriph][UART_INTSTATUS_RCVRTO]();
    }
    /* Disable the interrupt if callback function is not setup */
    else
    {
      UARTx->IER_DLH.BF_IER.ERBFI = 0;
    }
  }

  /* Busy detect indication */
  if(intId == UART_INTSTATUS_IID_BYDET)
  {
    /* Clear it by reading the UART status register */
    dummyData = UARTx->USR.WORDVAL;

    if(intCbfArra[intPeriph][UART_INTSTATUS_BYDET] != NULL)
    {
      /* call the callback function */
      intCbfArra[intPeriph][UART_INTSTATUS_BYDET]();
    }
    /* Disable the interrupt if callback function is not setup */
    else
    {
      UARTx->IER_DLH.BF_IER.ERBFI = 0;
    }
  }
}
/*@} end of group UART_Private_Functions */

/** @defgroup UART_Public_Functions
 *  @{
 */

/****************************************************************************//**
 * @brief      UART reset
 *
 * @param[in]  uartNo:  Select the UART port, should be UART1_ID, UART2_ID.
 *
 * @return none
 *
 *******************************************************************************/
void UART_Reset(UART_ID_Type uartNo)
{
  uart_reg_t * UARTx = (uart_reg_t *)(uartAddr[uartNo]);

  /* Check the parameters */
  CHECK_PARAM(IS_UART_ID(uartNo));

  /* Software reset */
  UARTx->SRR.BF.UR = 1;
}

/****************************************************************************//**
 * @brief      Initializes the UART
 *
 * @param[in]  uartNo:  Select the UART port,should be UART1_ID, UART2_ID.
 * @param[in]  uartCfgStruct:  Pointer to a UART configuration structure
 *
 * @return none
 *
 *******************************************************************************/
void UART_Init(UART_ID_Type uartNo, UART_CFG_Type* uartCfgStruct)
{
  uint32_t baudRateDivisor;

  uart_reg_t * UARTx = (uart_reg_t *)(uartAddr[uartNo]);

  /* check the parameters */
  CHECK_PARAM(IS_UART_ID(uartNo));
  CHECK_PARAM(IS_UART_STOPBITS(uartCfgStruct->stopBits));
  CHECK_PARAM(IS_UART_DATABITS(uartCfgStruct->dataBits));
  CHECK_PARAM(IS_UART_PARITY(uartCfgStruct->parity));

  /************Configure the Baud Rate*********************************/

  /* Enable DLAB to set baud rate */
  UARTx->LCR.BF.DLAB = 0x01;

  baudRateDivisor = CLK_GetUARTClk()/(16 * uartCfgStruct->baudRate);

  /* Config the DLL and DLH registers */
  UARTx->RBR_DLL_THR.BF_DLL.DLL = baudRateDivisor & 0xFF;
  UARTx->IER_DLH.BF_DLH.DLH = (baudRateDivisor >> 0x08)& 0xFF;

  /* Disable DLAB */
  UARTx->LCR.BF.DLAB = 0x0;

  /***********Configure Line control register (LCR)********************/

  /* Set parity */
  if(uartCfgStruct->parity == UART_PARITY_NONE)
  {
    UARTx->LCR.BF.PEN = 0x0;
  }
  else if(uartCfgStruct->parity == UART_PARITY_ODD)
  {
    UARTx->LCR.BF.PEN = 0x01;
    UARTx->LCR.BF.EPS = 0;
  }
  else
  {
    UARTx->LCR.BF.PEN = 0x01;
    UARTx->LCR.BF.EPS = 1;
  }

  /* Set UART Data Length and stop bit */
  UARTx->LCR.BF.DLS = uartCfgStruct->dataBits;
  UARTx->LCR.BF.STOP = uartCfgStruct->stopBits;

  /***********Configure auto flow control register (MCR)*******************/

  /* Configure the auto flow control */
  UARTx->MCR.BF.AFCE = uartCfgStruct->autoFlowControl;

}

/****************************************************************************//**
 * @brief      Set UART loopback mode
 *
 * @param[in]  uartNo:  UART peripheral selected, should be UART1_ID, UART2_ID.
 * @param[in]  newState: Enable/Disable function state
 *
 * @return none
 *
 *******************************************************************************/
void UART_SetLoopBackMode(UART_ID_Type uartNo, FunctionalState newState)
{
  uart_reg_t * UARTx = (uart_reg_t *)(uartAddr[uartNo]);

  /* check the parameter */
  CHECK_PARAM(IS_UART_ID(uartNo));

  /* Configurate the loopback */
  UARTx->MCR.BF.LOOPBACK = ((newState == ENABLE)? 0x01 : 0x00);

}

/****************************************************************************//**
 * @brief      Send break characters
 *
 * @param[in]  uartNo:  UART peripheral selected, should be UART1_ID, UART2_ID.
 * @param[in]  newState: Enable/Disable function state
 *
 * @return none
 *
 *******************************************************************************/
void UART_SendBreakCmd(UART_ID_Type uartNo, FunctionalState newState)
{
  uart_reg_t * UARTx = (uart_reg_t *)(uartAddr[uartNo]);

  /* check the parameter */
  CHECK_PARAM(IS_UART_ID(uartNo));

  /* send break characters */
  UARTx->LCR.BF.BREAK = ((newState == ENABLE)? 0x01 : 0x00);

}

/****************************************************************************//**
 * @brief      Receive one byte data from the UART peripheral
 *
 * @param[in]  uartNo:  Select the UART port, should be UART1_ID, UART2_ID.
 *
 * @return     The received data
 *
 *******************************************************************************/
uint8_t UART_ReceiveByte(UART_ID_Type uartNo)
{
  uart_reg_t * UARTx = (uart_reg_t *)(uartAddr[uartNo]);

  /* check the parameter */
  CHECK_PARAM(IS_UART_ID(uartNo));

  /* Receive data */
  return (uint8_t)(UARTx->RBR_DLL_THR.BF_RBR.RBR);
}

/****************************************************************************//**
 * @brief      Send one byte data to the UART peripheral
 *
 * @param[in]  uartNo:  Select the UART port,should be UART1_ID, UART2_ID.
 * @param[in]  data:  The data to be send
 *
 * @return none
 *
 *******************************************************************************/
void UART_SendByte(UART_ID_Type uartNo, uint8_t data)
{
  uart_reg_t * UARTx = (uart_reg_t *)(uartAddr[uartNo]);

  /* check the parameter */
  CHECK_PARAM(IS_UART_ID(uartNo));

  /* Send data */
  UARTx->RBR_DLL_THR.WORDVAL = ((uint32_t)data) & 0xFF;
}

void UART_SendBytes(UART_ID_Type uartNo, char *string, int len)
{  
  uart_reg_t * UARTx = (uart_reg_t *)(uartAddr[uartNo]);

  while ( len )
  {
    if (UARTx->LSR.BF.TEMT)
    {
      UART_SendByte(1, *string++);
      --len;
    }
  }
}
/****************************************************************************//**
 * @brief      Config UART FIFO function
 *
 * @param[in]  uartNo:  Select the UART port, should be UART1_ID, UART2_ID.
 * @param[in]  fifoCfg:  Pointer to a UART_FIFO_CFG_Type Structure
 *
 * @return none
 *
 *******************************************************************************/
void UART_FIFOConfig(UART_ID_Type uartNo, UART_FIFO_Type * fifoCfg)
{
  uart_reg_t * UARTx = (uart_reg_t *)(uartAddr[uartNo]);

  /* Check the parameters */
  CHECK_PARAM(IS_UART_ID(uartNo));
  CHECK_PARAM(IS_UART_FIFOTET(fifoCfg->FIFO_TxEmptyTrigger));
  CHECK_PARAM(IS_UART_FIFORT(fifoCfg->FIFO_RcvrTrigger));

  /* configure FIFO function */
  UARTx->IIR_FCR.BF_FCR.FIFOE = fifoCfg->FIFO_Function;

  /* configure the FIFO trigger level */
  if(fifoCfg->FIFO_Function == ENABLE)
  {
    /* configure FIFO transmit and receive reset */
    UARTx->IIR_FCR.BF_FCR.RFIFOR = fifoCfg->FIFO_ResetRx;
    UARTx->IIR_FCR.BF_FCR.XFIFOR = fifoCfg->FIFO_ResetTx;

    /* set RX and TX trigger level */
    UARTx->IIR_FCR.BF_FCR.TET = fifoCfg->FIFO_TxEmptyTrigger;
    UARTx->IIR_FCR.BF_FCR.RT = fifoCfg->FIFO_RcvrTrigger;
  }
}

/****************************************************************************//**
 * @brief      Reset uart receive fifo
 *
 * @param[in]  uartNo:  Select the UART port, should be UART1_ID, UART2_ID.
 *
 * @return none
 *
 *******************************************************************************/
void UART_ResetRxFifo(UART_ID_Type uartNo)
{
  uart_reg_t * UARTx = (uart_reg_t *)(uartAddr[uartNo]);

  /* Check the parameters */
  CHECK_PARAM(IS_UART_ID(uartNo));

  UARTx->IIR_FCR.BF_FCR.RFIFOR = 1;
}

/****************************************************************************//**
 * @brief      Reset uart transmit fifo
 *
 * @param[in]  uartNo:  Select the UART port, should be UART1_ID, UART2_ID.
 *
 * @return none
 *
 *******************************************************************************/
void UART_ResetTxFifo(UART_ID_Type uartNo)
{
  uart_reg_t * UARTx = (uart_reg_t *)(uartAddr[uartNo]);

  /* Check the parameters */
  CHECK_PARAM(IS_UART_ID(uartNo));

  UARTx->IIR_FCR.BF_FCR.XFIFOR = 1;
}

/****************************************************************************//**
 * @brief      Get current value of Line Status register
 *
 * @param[in]  uartNo:  Select the UART port, should be UART1_ID, UART2_ID.
 * @param[in]  lineStatus:  Checks whether the specified UART Line status bit
                       is set or not
 *
 * @return The state value of UART Line Status register
 *
 *******************************************************************************/
FlagStatus UART_GetLineStatus(UART_ID_Type uartNo,UART_LineStatus_Type lineStatus)
{
  FlagStatus bitStatus = RESET;
  uart_reg_t * UARTx = (uart_reg_t *)(uartAddr[uartNo]);

  /* Check the parameters */
  CHECK_PARAM(IS_UART_ID(uartNo));
  CHECK_PARAM(IS_UART_LINESTATUS(lineStatus));

  switch(lineStatus)
  {
    case UART_LINESTATUS_DR:
      bitStatus = ((UARTx->LSR.BF.DR == 1) ? SET : RESET);
      break;

    case UART_LINESTATUS_OE:
      bitStatus = ((UARTx->LSR.BF.OE == 1) ? SET : RESET);
      break;

    case UART_LINESTATUS_PE:
      bitStatus = ((UARTx->LSR.BF.PE == 1) ? SET : RESET);
      break;

    case UART_LINESTATUS_FE:
      bitStatus = ((UARTx->LSR.BF.FE == 1) ? SET : RESET);
      break;

    case UART_LINESTATUS_BI:
      bitStatus = ((UARTx->LSR.BF.BI == 1) ? SET : RESET);
      break;

    case UART_LINESTATUS_THRE:
      bitStatus = ((UARTx->LSR.BF.THRE == 1) ? SET : RESET);
      break;

    case UART_LINESTATUS_TEMT:
      bitStatus = ((UARTx->LSR.BF.TEMT == 1) ? SET : RESET);
      break;

    case UART_LINESTATUS_RFE:
      bitStatus = ((UARTx->LSR.BF.RFE == 1) ? SET : RESET);
      break;

    case UART_LINESTATUS_TRANS_ERR:
      bitStatus = ((UARTx->LSR.WORDVAL & 0x9e) ? SET : RESET);

    default:
      break;
  }

  return bitStatus;
}

/****************************************************************************//**
 * @brief      Get current value of UART Modem Status register
 *
 * @param[in]  uartNo:  Select the UART port, should be UART1_Port, UART2_Port.
 * @param[in]  modemStatus:  Checks whether the specified UART modem status bit
                       is set or not
 *
 * @return The state value of UART Modem Status register
 *
 *******************************************************************************/
FlagStatus UART_GetModemStatus(UART_ID_Type uartNo,UART_ModemStatus_Type modemStatus)
{
  FlagStatus bitStatus = RESET;
  uart_reg_t * UARTx = (uart_reg_t *)(uartAddr[uartNo]);

  /* Check the parameters */
  CHECK_PARAM(IS_UART_ID(uartNo));
  CHECK_PARAM(IS_UART_MODEMSTATUS(modemStatus));

  switch(modemStatus)
  {
    case UART_MODEMSTATUS_DCTS:
      bitStatus = ((UARTx->MSR.BF.DCTS == 1) ? SET : RESET);
      break;

    case UART_MODEMSTATUS_DDSR:
      bitStatus = ((UARTx->MSR.BF.DDSR == 1) ? SET : RESET);
      break;

    case UART_MODEMSTATUS_TERI:
      bitStatus = ((UARTx->MSR.BF.TERI == 1) ? SET : RESET);
      break;

    case UART_MODEMSTATUS_DDCD:
      bitStatus = ((UARTx->MSR.BF.DDCD == 1) ? SET : RESET);
      break;

    case UART_MODEMSTATUS_CTS:
      bitStatus = ((UARTx->MSR.BF.CTS == 1) ? SET : RESET);
      break;

    case UART_MODEMSTATUS_DSR:
      bitStatus = ((UARTx->MSR.BF.DSR == 1) ? SET : RESET);
      break;

    case UART_MODEMSTATUS_RI:
      bitStatus = ((UARTx->MSR.BF.RI == 1) ? SET : RESET);
      break;

    case UART_MODEMSTATUS_DCD:
      bitStatus = ((UARTx->MSR.BF.DCD == 1) ? SET : RESET);
      break;

    default:
      break;
  }

  return bitStatus;
}

/****************************************************************************//**
 * @brief      Get current value of UART Status register
 *
 * @param[in]  uartNo:  Select the UART port, should be UART1_ID, UART2_ID.
 * @param[in]  uartStatus:  Checks whether the specified UART status bit
                       is set or not
 *
 * @return The state value of UART Status register
 *
 *******************************************************************************/
FlagStatus UART_GetUartStatus(UART_ID_Type uartNo,UART_Status_Type uartStatus)
{
  FlagStatus bitStatus = RESET;
  uart_reg_t * UARTx = (uart_reg_t *)(uartAddr[uartNo]);

  /* Check the parameters */
  CHECK_PARAM(IS_UART_ID(uartNo));
  CHECK_PARAM(IS_UART_STATUS(uartStatus));

  switch(uartStatus)
  {
    case UART_STATUS_BUSY:
      bitStatus = ((UARTx->USR.BF.BUSY == 1) ? SET : RESET);
      break;

    case UART_STATUS_TFNF:
      bitStatus = ((UARTx->USR.BF.TFNF == 1) ? SET : RESET);
      break;

    case UART_STATUS_TFE:
      bitStatus = ((UARTx->USR.BF.TFE == 1) ? SET : RESET);
      break;

    case UART_STATUS_RFNE:
      bitStatus = ((UARTx->USR.BF.RFNE == 1) ? SET : RESET);
      break;

    case UART_STATUS_RFF:
      bitStatus = ((UARTx->USR.BF.RFF == 1) ? SET : RESET);
      break;

    default:
      break;
  }

  return bitStatus;
}

/****************************************************************************//**
 * @brief      Get current UART interrupt Status
 *
 * @param[in]  uartNo:  Select the UART port, should be UART1_ID, UART2_ID.
 * @param[in]  intStatusType:  Checks whether the specified UART interrupt status bit
               is set or not
 *
 * @return The state value of UART Status register
 *
 *******************************************************************************/
IntStatus UART_GetIntStatus(UART_ID_Type uartNo, UART_INTStatus_Type intStatusType)
{
  IntStatus bitStatus = RESET;
  uart_reg_t * UARTx = (uart_reg_t *)(uartAddr[uartNo]);

  /* Check the parameters */
  CHECK_PARAM(IS_UART_ID(uartNo));
  CHECK_PARAM(IS_UART_INT_STATUS(intStatusType));

  switch(intStatusType)
  {
    case UART_INTSTATUS_MODEM:
      if(UARTx->IIR_FCR.BF_IIR.IID == UART_INTSTATUS_IID_MODEM)
      {
        bitStatus = SET;
      }
      break;

    case UART_INTSTATUS_LSI:
      if(UARTx->IIR_FCR.BF_IIR.IID == UART_INTSTATUS_IID_LSI)
      {
        bitStatus = SET;
      }
      break;

    case UART_INTSTATUS_TBEI:
      if(UARTx->IIR_FCR.BF_IIR.IID == UART_INTSTATUS_IID_TBEI)
      {
        bitStatus = SET;
      }
      break;

    case UART_INTSTATUS_RBFI:
      if(UARTx->IIR_FCR.BF_IIR.IID == UART_INTSTATUS_IID_RBFI)
      {
        bitStatus = SET;
      }
      break;

    case UART_INTSTATUS_BYDET:
      if(UARTx->IIR_FCR.BF_IIR.IID == UART_INTSTATUS_IID_BYDET)
      {
        bitStatus = SET;
      }
      break;

    case UART_INTSTATUS_RCVRTO:
      if(UARTx->IIR_FCR.BF_IIR.IID == UART_INTSTATUS_IID_RCVRTO)
      {
        bitStatus = SET;
      }
      break;

    case UART_INTSTATUS_ALL:
      if(UARTx->IIR_FCR.BF_IIR.IID != 0x01)
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
 * @brief      Get current UART receive FIFO level
 *
 * @param[in]  uartNo:  Select the UART port, should be UART1_ID, UART2_ID.
 *
 * @return The number of data entries in the receive FIFO
 *
 *******************************************************************************/
uint32_t UART_GetRxFIFOLevel(UART_ID_Type uartNo)
{
  uart_reg_t * UARTx = (uart_reg_t *)(uartAddr[uartNo]);

  /* Check the parameters */
  CHECK_PARAM(IS_UART_ID(uartNo));

  return(UARTx->RFL.BF.RFL);
}

/****************************************************************************//**
 * @brief      Get current UART transmit FIFO level
 *
 * @param[in]  uartNo:  Select the UART port, should be UART1_ID, UART2_ID.
 *
 * @return The number of data entries in the transmit FIFO
 *
 *******************************************************************************/
uint32_t UART_GetTxFIFOLevel(UART_ID_Type uartNo)
{
  uart_reg_t * UARTx = (uart_reg_t *)(uartAddr[uartNo]);

  /* Check the parameters */
  CHECK_PARAM(IS_UART_ID(uartNo));

  return(UARTx->TFL.BF.TFL);
}

/****************************************************************************//**
 * @brief      Assert UART RTS bit in MCR if Auto Flow Control Disabled
 *
 * @param[in]  uartNo:  Select the UART port,should be UART0_ID, UART1_ID,
 *                                                     UART2_ID, UART3_ID
 *
 * @return     None
 *
 *******************************************************************************/
void UART_RTS_Assert(UART_ID_Type uartNo)
{
  uart_reg_t * UARTx = (uart_reg_t *)(uartAddr[uartNo]);

  /* Check the parameters */
  CHECK_PARAM(IS_UART_ID(uartNo));

  UARTx->MCR.BF.RTS = 1;
}

/****************************************************************************//**
 * @brief      Deassert UART RTS bit in MCR if Auto Flow Control Disabled
 *
 * @param[in]  uartNo:  Select the UART port,should be UART0_ID, UART1_ID,
 *                                                     UART2_ID, UART3_ID
 *
 * @return     None
 *
 *******************************************************************************/
void UART_RTS_Deassert(UART_ID_Type uartNo)
{
  uart_reg_t * UARTx = (uart_reg_t *)(uartAddr[uartNo]);

  /* Check the parameters */
  CHECK_PARAM(IS_UART_ID(uartNo));

  UARTx->MCR.BF.RTS = 0;
}

/****************************************************************************//**
 * @brief      Mask/Unmask the UART interrupt
 *
 * @param[in]  uartNo:  Select the UART port, should be UART1_ID, UART2_ID.
 * @param[in]  intType:  Specifies the interrupt type
 * @param[in]  newState:  Enable/Disable Specified interrupt type
 *
 * @return none
 *
 *******************************************************************************/
void UART_IntMask(UART_ID_Type uartNo,UART_INT_Type intType, IntMask_Type intMask)
{
  uart_reg_t * UARTx = (uart_reg_t *)(uartAddr[uartNo]);

  /* Check the parameters */
  CHECK_PARAM(IS_UART_ID(uartNo));
  CHECK_PARAM(IS_UART_INT(intType));

  switch(intType)
  {
    case UART_INT_EDSSI:
      UARTx->IER_DLH.BF_IER.EDSSI = ((intMask == UNMASK)? 1 : 0);
      break;

    case UART_INT_ELSI:
      UARTx->IER_DLH.BF_IER.ELSI = ((intMask == UNMASK)? 1 : 0);
      break;

    case UART_INT_ETBEI:
      if(intMask == UNMASK)
      {
        /* Enable THRE interrupt mode function */
        UARTx->IER_DLH.BF_IER.PTIME = 1;
        UARTx->IER_DLH.BF_IER.ETBEI = 1;
      }
      else
      {
        /* Disable THRE interrupt mode function */
        UARTx->IER_DLH.BF_IER.PTIME = 0;
        UARTx->IER_DLH.BF_IER.ETBEI = 0;
      }

      break;

    case UART_INT_ERBFI:
      UARTx->IER_DLH.BF_IER.ERBFI = ((intMask == UNMASK)? 1 : 0);
      break;

    case UART_INT_ALL:
      if(intMask == UNMASK)
      {
        UARTx->IER_DLH.WORDVAL |= 0x8F;
      }
      else
      {
        UARTx->IER_DLH.WORDVAL &= ~0x8F;
      }

      break;

    default:
      break;
  }
}

/****************************************************************************//**
 * @brief  UART1 interrupt function
 *
 * @param  none
 *
 * @return none
 *
 *******************************************************************************/
__attribute__((weak)) void UART1_IRQHandler(void)
{
   UART_IntHandler(INT_UART1,UART1_ID);
}
/****************************************************************************//**
 * @brief  UART2 interrupt function
 *
 * @param  none
 *
 * @return none
 *
 *******************************************************************************/
__attribute__((weak)) void UART2_IRQHandler(void)
{
   UART_IntHandler(INT_UART2,UART2_ID);
}
