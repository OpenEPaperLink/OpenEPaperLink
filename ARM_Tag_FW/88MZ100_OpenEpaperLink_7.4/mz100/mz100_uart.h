#pragma once

#include "mz100.h"
#include "mz100_driver.h"

/**
 *  @brief UART No. type definition
 */
typedef enum
{
  UART1_ID = 0, /*!< UART1 port define */
  UART2_ID,     /*!< UART2 port define */
} UART_ID_Type;

/**
 *  @brief UART parity type definition
 */
typedef enum
{
  UART_PARITY_NONE = 0, /*!< UART none define */
  UART_PARITY_ODD,      /*!< UART odd define */
  UART_PARITY_EVEN,     /*!< UART even define */
} UART_Parity_Type;

/**
 *  @brief UART data bits type definition
 */
typedef enum
{
  UART_DATABITS_5, /*!< UART data bits length: 5 bits */
  UART_DATABITS_6, /*!< UART data bits length: 6 bits */
  UART_DATABITS_7, /*!< UART data bits length: 7 bits */
  UART_DATABITS_8, /*!< UART data bits length: 8 bits */
} UART_DataBits_Type;

/**
 *  @brief UART stop bits type definition
 */
typedef enum
{
  UART_STOPBITS_1,     /*!< UART stop bits length: 1 bits */
  UART_STOPBITS_1P5_2, /*!< UART stop bits length: 1.5 or 2 bits */
} UART_StopBits_Type;

/**
 *  @brief UART TX FIFO level type definition
 */
typedef enum
{
  UART_FIFO_TET_EMPTY,        /*!< UART FIFO empty */
  UART_FIFO_TET_CHARACTERS_2, /*!< UART 2 characters in the FIFO */
  UART_FIFO_TET_QUATER_FULL,  /*!< UART FIFO 1/4 full */
  UART_FIFO_TET_HALF_FULL,    /*!< UART FIFO 1/2 full */
} UART_TxFIFOLevel_Type;

/**
 *  @brief UART RX FIFO level type definition
 */
typedef enum
{
  UART_FIFO_RT_CHARACTERS_1,    /*!< UART 1 character in the FIFO  */
  UART_FIFO_RT_QUATER_FULL,     /*!< UART 1/4 characters in the FIFO */
  UART_FIFO_RT_HALF_FULL,       /*!< UART 1/2 full */
  UART_FIFO_RT_2_LESSTHAN_FULL, /*!< UART FIFO 2 less than full */
} UART_RxFIFOLevel_Type;

/**
 *  @brief UART Line status bit type definition
 */
typedef enum
{
  UART_LINESTATUS_DR,        /*!< Data ready bit */
  UART_LINESTATUS_OE,        /*!< Overrun error bit */
  UART_LINESTATUS_PE,        /*!< Parity error bit */
  UART_LINESTATUS_FE,        /*!< Framing Error bit */
  UART_LINESTATUS_BI,        /*!< Break interrupt bit */
  UART_LINESTATUS_THRE,      /*!< Transmit holding register empty bit */
  UART_LINESTATUS_TEMT,      /*!< Transmit empty bit */
  UART_LINESTATUS_RFE,       /*!< Receiver FIFO error bit */
  UART_LINESTATUS_TRANS_ERR, /*!< Indicate whether some of 'OE','PE','FE','BI' and 'RFE' happen */
} UART_LineStatus_Type;

/**
 *  @brief UART Modem status bit type definition
 */
typedef enum
{
  UART_MODEMSTATUS_DCTS, /*!< Delta clear to send bit */
  UART_MODEMSTATUS_DDSR, /*!< Delta data set ready bit */
  UART_MODEMSTATUS_TERI, /*!< Trailing edge of ring indicator bit */
  UART_MODEMSTATUS_DDCD, /*!< Delta data carrier detect bit */
  UART_MODEMSTATUS_CTS,  /*!< Clear to send bit */
  UART_MODEMSTATUS_DSR,  /*!< Data set ready bit */
  UART_MODEMSTATUS_RI,   /*!< Ring indicator bit */
  UART_MODEMSTATUS_DCD,  /*!< Data carrier ditect bit */
} UART_ModemStatus_Type;

/**
 *  @brief UART status bit type definition
 */
typedef enum
{
  UART_STATUS_BUSY, /*!< UART busy bit */
  UART_STATUS_TFNF, /*!< Transmit FIFO not full bit */
  UART_STATUS_TFE,  /*!< Transmit FIFO empty bit */
  UART_STATUS_RFNE, /*!< Receive FIFO not empty bit */
  UART_STATUS_RFF,  /*!< Receive FIFO full */
} UART_Status_Type;

/**
 *  @brief UART interrupt type definition
 */
typedef enum
{
  UART_INT_EDSSI, /*!< Modem status interrupt enable */
  UART_INT_ELSI,  /*!< RX line status interrupt enable*/
  UART_INT_ETBEI, /*!< Transmit holding register empty interrupt enable */
  UART_INT_ERBFI, /*!< Received data available interrupt enable */
  UART_INT_ALL,   /*!< All the interrupt */
} UART_INT_Type;

/**
 *  @brief UART interrupt type definition
 */
typedef enum
{
  UART_INTSTATUS_MODEM,  /*!< Modem interrupt status */
  UART_INTSTATUS_TBEI,   /*!< Transmit holding register empty interrupt status */
  UART_INTSTATUS_RBFI,   /*!< Received data available interrupt status */
  UART_INTSTATUS_LSI,    /*!< Received line status interrupt */
  UART_INTSTATUS_BYDET,  /*!< Busy detect indication */
  UART_INTSTATUS_RCVRTO, /*!< Character timeout indication */
  UART_INTSTATUS_ALL,    /*!< Interrupt status */
} UART_INTStatus_Type;

/**
 *  @brief UART Configuration Structure type definition
 */
typedef struct
{
  uint32_t baudRate;               /*!< Configures the UART communication baud rate */
  UART_DataBits_Type dataBits;     /*!< Data size, 5/6/7/8 bits */
  UART_Parity_Type parity;         /*!< Parity Enable */
  UART_StopBits_Type stopBits;     /*!< Number of stop bits */
  FunctionalState autoFlowControl; /*!< Auto flow control enable */
} UART_CFG_Type;

/**
 *  @brief UART FIFO Configuration Structure type definition
 */
typedef struct
{
  FunctionalState FIFO_Function;             /*!< Enable or disable FIFO function*/
  FunctionalState FIFO_ResetTx;              /*!< Reset transmit FIFO */
  FunctionalState FIFO_ResetRx;              /*!< Reset receive FIFO */
  UART_TxFIFOLevel_Type FIFO_TxEmptyTrigger; /*!< TX Empty trigger level */
  UART_RxFIFOLevel_Type FIFO_RcvrTrigger;    /*!< RCVR trigger level */
} UART_FIFO_Type;

/*@} end of group UART_Public_Types */

/** @defgroup UART_Public_Constants
 *  @{
 */
/** @defgroup UART_Stop_Bit
 *  @{
 */
/** @defgroup UART_ID
 *  @{
 */
#define IS_UART_ID(UART_ID) (UART_ID <= UART2_ID)

/*@} end of group UART_ID */

/** @defgroup UART_Data_Bit
 *  @{
 */
#define IS_UART_DATABITS(DATABITS) (DATABITS <= UART_DATABITS_8)

/*@} end of group UART_Data_Bit */

/** @defgroup UART_STOP_BIT
 *  @{
 */
#define IS_UART_STOPBITS(STOPBITS) (STOPBITS <= UART_STOPBITS_1P5_2)

/*@} end of group UART_STOP_BIT */

/** @defgroup UART_Parity
 *  @{
 */
#define IS_UART_PARITY(PARITY) (PARITY <= UART_PARITY_EVEN)

/*@} end of group UART_Parity */

/** @defgroup UART_FIFO_CFG
 *  @{
 */
#define IS_UART_FIFOTET(FIFOTET) (FIFOTET <= UART_FIFO_TET_HALF_FULL)

/*@} end of group UART_FIFO_CFG */

/** @defgroup UART_FIFO_CFG
 *  @{
 */
#define IS_UART_FIFORT(FIFORT) (FIFORT <= UART_FIFO_RT_2_LESSTHAN_FULL)

/*@} end of group UART_FIFO_CFG */

/** @defgroup UART_LINE_STATUS
 *  @{
 */
#define IS_UART_LINESTATUS(LINE_STATUS) (LINE_STATUS <= UART_LINESTATUS_TRANS_ERR)

/*@} end of group UART_LINE_STATUS */

/** @defgroup UART_MODEM_STATUS
 *  @{
 */
#define IS_UART_MODEMSTATUS(MODEM_STATUS) (MODEM_STATUS <= UART_MODEMSTATUS_DCD)

/*@} end of group UART_MODEM_STATUS */

/** @defgroup UART_STATUS
 *  @{
 */
#define IS_UART_STATUS(STATUS) (STATUS <= UART_STATUS_RFF)

/*@} end of group UART_STATUS */

/** @defgroup UART_INT_TYPE
 *  @{
 */
#define IS_UART_INT(INT) (INT <= UART_INT_ALL)

/** @defgroup UART_INT_TYPE
 *  @{
 */
#define IS_UART_INT_STATUS(INT_STATUS) (INT_STATUS <= UART_INTSTATUS_ALL)

/*@} end of group UART_INT_TYPE */

/*@} end of group UART_Public_Constants */

/** @defgroup UART_Public_Macro
 *  @{
 */

/*@} end of group UART_Public_Macro */

/** @defgroup UART_Public_FunctionDeclaration
 *  @brief UART functions declaration
 *  @{
 */
void UART_Init(UART_ID_Type uartNo, UART_CFG_Type *uartCfgStruct);
void UART_Reset(UART_ID_Type uartNo);
void UART_ResetRxFifo(UART_ID_Type uartNo);
void UART_ResetTxFifo(UART_ID_Type uartNo);

void UART_SendBreakCmd(UART_ID_Type uartNo, FunctionalState newState);
void UART_SetLoopBackMode(UART_ID_Type uartNo, FunctionalState newState);
uint8_t UART_ReceiveByte(UART_ID_Type uartNo);
void UART_SendByte(UART_ID_Type uartNo, uint8_t data);
void UART_SendBytes(UART_ID_Type uartNo, char *string, int len);

void UART_FIFOConfig(UART_ID_Type uartNo, UART_FIFO_Type *fifoCfg);
void UART_RTS_Assert(UART_ID_Type uartNo);
void UART_RTS_Deassert(UART_ID_Type uartNo);

FlagStatus UART_GetLineStatus(UART_ID_Type uartNo, UART_LineStatus_Type lineStatus);
FlagStatus UART_GetModemStatus(UART_ID_Type uartNo, UART_ModemStatus_Type modemStatus);
FlagStatus UART_GetUartStatus(UART_ID_Type uartNo, UART_Status_Type uartStatus);
IntStatus UART_GetIntStatus(UART_ID_Type uartNo, UART_INTStatus_Type intStatusType);
uint32_t UART_GetRxFIFOLevel(UART_ID_Type uartNo);
uint32_t UART_GetTxFIFOLevel(UART_ID_Type uartNo);

void UART_IntMask(UART_ID_Type uartNo, UART_INT_Type intType, IntMask_Type intMask);
void UART1_IRQHandler(void);
void UART2_IRQHandler(void);
