#include "mz100.h"
#include "mz100_driver.h"
#include "mz100_flash.h"
#include "mz100_qspi.h"

/** @addtogroup MZ100_Periph_Driver
 *  @{
 */

/** @defgroup FLASH FLASH
 *  @brief FLASH driver modules
 *  @{
 */

/** @defgroup FLASH_Private_Type
 *  @{
 */

/*@} end of group FLASH_Private_Type*/

/** @defgroup FLASH_Private_Defines
 *  @{
 */

/**  
 *  @brief FLASH read mode
 */ 
typedef enum
{
  FLASH_STATUS_LO,                    /*!< STATUS[7:0] */
  FLASH_STATUS_HI,                    /*!< STATUS[15:8] */
}FLASH_Status_Type; 

/** @defgroup FLASH_INSTRUCTIONS       
 *  @{
 */     
#define FLASH_INS_CODE_WE      0x06   /*!< Write enable */
#define FLASH_INS_CODE_WE_VSR  0x50   /*!< Write enable for volatile status register */
#define FLASH_INS_CODE_WD      0x04   /*!< Write disable */
#define FLASH_INS_CODE_RSR1    0x05   /*!< Read status register 1 */
#define FLASH_INS_CODE_RSR2    0x35   /*!< Read status register 2 */
#define FLASH_INS_CODE_WSR     0x01   /*!< Write status register */
#define FLASH_INS_CODE_PP      0x02   /*!< Page program */
#define FLASH_INS_CODE_QPP     0x32   /*!< Quad page program */
#define FLASH_INS_CODE_SE      0x20   /*!< Sector(4k) erase */
#define FLASH_INS_CODE_BE_32K  0x52   /*!< Block(32k) erase */
#define FLASH_INS_CODE_BE_64K  0xD8   /*!< Block(64k) erase */
#define FLASH_INS_CODE_CE      0xC7   /*!< Chip erase */
#define FLASH_INS_CODE_PD      0xB9   /*!< Power down */

#define FLASH_INS_CODE_RD      0x03   /*!< Read data */
#define FLASH_INS_CODE_FR      0x0B   /*!< Fast read */
#define FLASH_INS_CODE_FRDO    0x3B   /*!< Fast read dual output */ 
#define FLASH_INS_CODE_FRQO    0x6B   /*!< Fast read quad output */ 
#define FLASH_INS_CODE_FRDIO   0xBB   /*!< Fast read dual IO */
#define FLASH_INS_CODE_FRQIO   0xEB   /*!< Fast read quad IO */
#define FLASH_INS_CODE_WFRQIO  0xE7   /*!< Word Fast read quad IO, A0 must be zero */
#define FLASH_INS_CODE_OWFRQIO 0xE3   /*!< Octal word Fast read quad IO, A[3:0] must be zero */

#define FLASH_INS_CODE_RPD_DI  0xAB   /*!< Release power down or device ID */
#define FLASH_INS_CODE_RUID    0x4B   /*!< Read unique ID number */

/*@} end of group FLASH_INSTRUCTIONS */

/** @defgroup FLASH_CONSTANTS       
 *  @{
 */   
#define FLASH_PAGE_SIZE             0x100   /*!< 256 bytes */
#define FLASH_SECTOR_SIZE           0x1000  /*!< 4k */
#define FLASH_32K_BLOCK_SIZE        0x8000  /*!< 32k */
#define FLASH_64K_BLOCK_SIZE        0x10000 /*!< 64k */
#define FLASH_CHIP_SIZE             0x80000 /*!< 512k */
#define FLASH_LAST_SECTOR           ((FLASH_CHIP_SIZE/FLASH_SECTOR_SIZE) - 1)
#define FLASH_LAST_32K_BLOCK        ((FLASH_CHIP_SIZE/FLASH_32K_BLOCK_SIZE) - 1)
#define FLASH_LAST_64K_BLOCK        ((FLASH_CHIP_SIZE/FLASH_64K_BLOCK_SIZE) - 1)

/*@} end of group FLASH_CONSTANTS */

/* get the page number according to the address with the page space */
#define FLASH_PAGE_NUM(addr)        ((addr)>>8)

/* get the page begin and end address according to the address with the page space */
#define FLASH_PAGE_BEGIN_ADDR(addr) (FLASH_PAGE_SIZE * FLASH_PAGE_NUM(addr))
#define FLASH_PAGE_END_ADDR(addr)   (FLASH_PAGE_SIZE * (FLASH_PAGE_NUM(addr)+1) - 1)

/*@} end of group FLASH_Private_Defines */

/** @defgroup FLASH_Private_Variables
 *  @{
 */

/*@} end of group FLASH_Private_Variables */

/** @defgroup FLASH_Global_Variables
 *  @{
 */

/*@} end of group FLASH_Global_Variables */

/** @defgroup FLASH_Private_FunctionDeclaration
 *  @{
 */
void FLASH_SetWriteEnableBit(FunctionalState newCmd);
void FLASH_WriteEnableVSR(void);
uint8_t FLASH_GetStatus(FLASH_Status_Type statusIdx);
Status FLASH_WriteStatus(uint16_t status);

Status FLASH_IsSectorBlank(uint32_t sectorNumber);
Status Flash_IsBlank(void);

/*@} end of group FLASH_Private_FunctionDeclaration */

/** @defgroup FLASH_Private_Functions
 *  @{
 */
/****************************************************************************//**
 * @brief      Set flash write enable / disable 
 *
 * @param[in]  newCmd:  Enable/disable flash write
 *
 * @return none
 *
 *******************************************************************************/
void FLASH_SetWriteEnableBit(FunctionalState newCmd) 
{
  /* Clear QSPI FIFO */ 
  QSPI_FlushFIFO();
  
  /* Set Header count register: instruction counter, address counter, read mode counter and dummy counter */
  QSPI_SetHdrcnt(QSPI_INSTR_CNT_1BYTE, QSPI_ADDR_CNT_0BYTE, QSPI_RM_CNT_0BYTE, QSPI_DUMMY_CNT_0BYTE);
  
  /* Set data in counter */
  QSPI_SetDInCnt(0);
  
  if(newCmd == ENABLE)
  {
    /* Set instruction */
    QSPI_SetInstr(FLASH_INS_CODE_WE);
  }
  else
  {
    /* Set instruction */
    QSPI_SetInstr(FLASH_INS_CODE_WD);
  }
        
  /* Set QSPI write */
  QSPI_StartTransfer(QSPI_W_EN);
  
  /* Stop QSPI transfer */
  QSPI_StopTransfer();
}

/****************************************************************************//**
 * @brief      Write enable for volatile status register
 *
 * @param none
 *
 * @return none
 *
 *******************************************************************************/
void FLASH_WriteEnableVSR(void)
{
  /* Clear QSPI FIFO */ 
  QSPI_FlushFIFO();
  
  /* Set Header count register: instruction counter, address counter, read mode counter and dummy counter */
  QSPI_SetHdrcnt(QSPI_INSTR_CNT_1BYTE, QSPI_ADDR_CNT_0BYTE, QSPI_RM_CNT_0BYTE, QSPI_DUMMY_CNT_0BYTE);
  
  /* Set data in counter */
  QSPI_SetDInCnt(0);
  
  /* Set instruction */
  QSPI_SetInstr(FLASH_INS_CODE_WE_VSR);
  
  /* Set QSPI write */
  QSPI_StartTransfer(QSPI_W_EN);
  
  /* Stop QSPI transfer */
  QSPI_StopTransfer();  
}

/****************************************************************************//**
 * @brief      Get the flash status
 *
* @param[in]   statusIdx:  Status[7:0] or Status[15:8]
 *
 * @return     Specified status
 *
 *******************************************************************************/
uint8_t FLASH_GetStatus(FLASH_Status_Type statusIdx)
{
  uint8_t status;
  
  /* Clear QSPI FIFO */ 
  QSPI_FlushFIFO();  
  
  /* Set Header count register: instruction counter, address counter, read mode counter and dummy counter */
  QSPI_SetHdrcnt(QSPI_INSTR_CNT_1BYTE, QSPI_ADDR_CNT_0BYTE, QSPI_RM_CNT_0BYTE, QSPI_DUMMY_CNT_0BYTE);
  
  /* Set data in counter */
  QSPI_SetDInCnt(1);
  
  /* Set QSPI data pin mode */
  QSPI->CONF.BF.DATA_PIN = QSPI_DATA_PIN_SINGLE; 
  
  if(statusIdx == FLASH_STATUS_LO)
  {
    /* Set instruction */
    QSPI_SetInstr(FLASH_INS_CODE_RSR1);
  }
  else if(statusIdx == FLASH_STATUS_HI)
  {
    /* Set instruction */
    QSPI_SetInstr(FLASH_INS_CODE_RSR2);
  }
  
  /* QSPI one-byte length mode */
  QSPI->CONF.BF.BYTE_LEN = QSPI_BYTE_LEN_1BYTE;

  /* Set QSPI read */
  QSPI_StartTransfer(QSPI_R_EN);
  
  /* Get flash busy status */
  status = QSPI_ReadByte();
  
  /* Disable QSPI */
  QSPI_SetSSEnable(DISABLE);  
  
  return status;
}

/****************************************************************************//**
 * @brief      Write flash status register
 *
 * @param[in]  status:  status
 *
 * @return     SUCCESS or ERROR
 *
 *******************************************************************************/
Status FLASH_WriteStatus(uint16_t status)
{
  volatile uint32_t localCnt = 0;
  uint8_t byte;
   
  /* Enable flash write */
  FLASH_WriteEnableVSR();
  
  /* Set QSPI data pin mode */
  QSPI->CONF.BF.DATA_PIN = QSPI_DATA_PIN_SINGLE; 

  /* Set instruction */
  QSPI_SetInstr(FLASH_INS_CODE_WSR);    
    
  /* QSPI one-byte length mode */
  QSPI->CONF.BF.BYTE_LEN = QSPI_BYTE_LEN_1BYTE;

  /* Set QSPI write */
  QSPI_StartTransfer(QSPI_W_EN);
  
  /* Write status[7:0] */
  byte = status & 0xFF;
  QSPI_WriteByte(byte);
  
  /* Write status[15:8] */
  byte = (status >> 8) & 0xFF;
  QSPI_WriteByte(byte);
  
  /* Stop QSPI transfer */
  QSPI_StopTransfer();
  
  while(localCnt++ < 100000)
  {
    /* Check flash busy status */ 
    if( FLASH_GetBusyStatus() == RESET )
    {
      return SUCCESS;
    }
  }
  
  return ERROR;
}

/****************************************************************************//**
 * @brief      Check sector is blank or not
 *
 * @param[in]  sectorNumber:  Sector to be checked
 *
 * @return     Status
 *
 *******************************************************************************/
Status FLASH_IsSectorBlank(uint32_t sectorNumber) 
{
  Status funcStatus = SUCCESS;
  uint32_t sectorAddress;
  uint32_t count;
  uint32_t data;

  if(!(sectorNumber > FLASH_LAST_SECTOR)) 
  {
    /* Get sector start address */
    sectorAddress = sectorNumber * FLASH_SECTOR_SIZE;
    
    for(count = 0; count < FLASH_SECTOR_SIZE; count++)
    {
      data = FLASH_WordRead(FLASH_NORMAL_READ, sectorAddress+count);
      if(data != 0xFFFFFFFF) 
      {
        funcStatus = ERROR;
        break;
      }
    }
  } 
  else
  {
    funcStatus = ERROR;
  }

  return funcStatus;
}

/****************************************************************************//**
 * @brief      Check flash is blank or not
 *
 * @param none
 *
 * @return     Status
 *
 *******************************************************************************/
Status FLASH_IsBlank(void)
{
  Status funcStatus = SUCCESS;
  uint32_t flashData;
  uint32_t i;
  uint32_t maxWordAddr;
  
  maxWordAddr = FLASH_CHIP_SIZE >> 2;
  
  for(i=0; i<maxWordAddr; i++)
  {
    flashData = FLASH_WordRead(FLASH_NORMAL_READ, i<<2);
    if(flashData != 0xFFFFFFFF)
    {
      funcStatus = ERROR;
      break;
    }
  }
  
  return funcStatus;
}

/*@} end of group FLASH_Private_Functions */

/** @defgroup FLASH_Public_Functions
 *  @{
 */
/****************************************************************************//**
 * @brief      Get the flash busy status
 *
 * @param none
 *
 * @return     Flash busy status
 *
 *******************************************************************************/
FlagStatus FLASH_GetBusyStatus(void)
{
  FlagStatus funcStatus;  
  
  /* Clear QSPI FIFO */ 
  QSPI_FlushFIFO();  
  
  /* Set Header count register: instruction counter, address counter, read mode counter and dummy counter */
  QSPI_SetHdrcnt(QSPI_INSTR_CNT_1BYTE, QSPI_ADDR_CNT_0BYTE, QSPI_RM_CNT_0BYTE, QSPI_DUMMY_CNT_0BYTE);
    
  /* Set data in counter */
  QSPI_SetDInCnt(1);
  
  /* Set QSPI data pin mode */
  QSPI->CONF.BF.DATA_PIN = QSPI_DATA_PIN_SINGLE; 

  /* Set instruction */
  QSPI_SetInstr(FLASH_INS_CODE_RSR1);
  
  /* QSPI one-byte length mode */
  QSPI->CONF.BF.BYTE_LEN = QSPI_BYTE_LEN_1BYTE;

  /* Set QSPI read */
  QSPI_StartTransfer(QSPI_R_EN);
  
  /* Get flash busy status */
  funcStatus = (QSPI_ReadByte() & 0x01) ? SET : RESET;
  
  /* Disable QSPI */
  QSPI_SetSSEnable(DISABLE);  
  
  return funcStatus;
}

/****************************************************************************//**
 * @brief      Set flash protection mode 
 *
 * @param[in]  protectMode:  Protection mode
 *
 * @return     SUCCESS or ERROR
 *
 *******************************************************************************/
Status FLASH_SetProtectionMode(FLASH_Protection_Type protectMode)
{
  volatile uint32_t localCnt = 0;
  
  /* Enable flash write */
  FLASH_SetWriteEnableBit(ENABLE);
  
  /* Set QSPI data pin mode */
  QSPI->CONF.BF.DATA_PIN = QSPI_DATA_PIN_SINGLE; 
  
  /* Set instruction */
  QSPI_SetInstr(FLASH_INS_CODE_WSR);    
  
  /* Set QSPI write */
  QSPI_StartTransfer(QSPI_W_EN);
  
  /* Write protection mode (SEC, TB, BP2, BP1, BP0) */
  QSPI_WriteByte(protectMode & 0x7F);
  
  /* Write protection mode (CMP) */
  QSPI_WriteByte((protectMode & 0x80)>>1);
  
  /* Stop QSPI transfer */
  QSPI_StopTransfer();
  
  while(localCnt++ < 100000)
  {
    /* Check flash busy status */ 
    if( FLASH_GetBusyStatus() == RESET )
    {
      return SUCCESS;
    }
  }
  
  return ERROR;
}

/****************************************************************************//**
 * @brief      Whole flash erase
 *
 * @param none
 *
 * @return     Status
 *
 *******************************************************************************/
Status FLASH_EraseAll(void) 
{
  volatile uint32_t localCnt = 0;
    
  /* Enable flash write */
  FLASH_SetWriteEnableBit(ENABLE);  
  
  /* Set instruction */
  QSPI_SetInstr(FLASH_INS_CODE_CE);
  
  /* Set QSPI write */
  QSPI_StartTransfer(QSPI_W_EN);
  
  /* Stop QSPI transfer */
  QSPI_StopTransfer();
  
  while(localCnt++ < 0xFFFFFFF)
  {
    /* Check flash busy status */ 
    if( FLASH_GetBusyStatus() == RESET )
    {
      return SUCCESS;
    }
  }
  
  return ERROR;    
}

/****************************************************************************//**
 * @brief      Flash sector erase
 *
 * @param[in]  sectorNumber:  Sector number to be erased
 *
 * @return     SUCCESS or ERROR
 *
 *******************************************************************************/
Status FLASH_SectorErase(uint32_t sectorNumber) 
{
  uint32_t sectorAddress;
  volatile uint32_t localCnt = 0;

  if(!(sectorNumber > FLASH_LAST_SECTOR)) 
  {
    /* Enable flash write */
    FLASH_SetWriteEnableBit(ENABLE);
    
    /* Get start address for sector to be erased */
    sectorAddress = sectorNumber* FLASH_SECTOR_SIZE;
    
    /* Set address counter */
    QSPI_SetAddrCnt(QSPI_ADDR_CNT_3BYTE);
   
    /* Set QSPI address pin mode */
    QSPI->CONF.BF.ADDR_PIN = QSPI_ADDR_PIN_SINGLE;
    
    /* Set address */ 
    QSPI_SetAddr(sectorAddress);
    
    /* Set instruction */
    QSPI_SetInstr(FLASH_INS_CODE_SE);
    
    /* Set QSPI write */
    QSPI_StartTransfer(QSPI_W_EN);
    
    /* Stop QSPI transfer */
    QSPI_StopTransfer();
    
    while(localCnt++ < 1000000)
    {
      /* Check flash busy status */ 
      if( FLASH_GetBusyStatus() == RESET )
      {
        return SUCCESS;
      }
    }
  }
  
  return ERROR;
}

/****************************************************************************//**
 * @brief      Flash 32KB block erase
 *
 * @param[in]  sectorNumber:  block number to be erased
 *
 * @return     SUCCESS or ERROR
 *
 *******************************************************************************/
Status FLASH_Block32KErase(uint32_t blockNumber) 
{
  uint32_t blockAddress;
  volatile uint32_t localCnt = 0;
  
  if(!(blockNumber > FLASH_LAST_32K_BLOCK) ) 
  {
    /* Enable flash write */
    FLASH_SetWriteEnableBit(ENABLE);
    
    /* Get start address of the block to be erased */
    blockAddress = blockNumber * FLASH_32K_BLOCK_SIZE;
    
    /* Set address counter */
    QSPI_SetAddrCnt(QSPI_ADDR_CNT_3BYTE);
    
    /* Set QSPI address pin mode */
    QSPI->CONF.BF.ADDR_PIN = QSPI_ADDR_PIN_SINGLE;
    
    /* Set address */ 
    QSPI_SetAddr(blockAddress);
    
    /* Set instruction */
    QSPI_SetInstr(FLASH_INS_CODE_BE_32K);
    
    /* Set QSPI write */
    QSPI_StartTransfer(QSPI_W_EN);
    
    /* Stop QSPI transfer */
    QSPI_StopTransfer();
    
    while(localCnt++ < 2000000)
    {
      /* Check flash busy status */ 
      if( FLASH_GetBusyStatus() == RESET )
      {
        return SUCCESS;
      }
    }
  }
  
  return ERROR;
}

/****************************************************************************//**
 * @brief      Flash 64KB block erase
 *
 * @param[in]  sectorNumber:  block number to be erased
 *
 * @return     SUCCESS or ERROR
 *
 *******************************************************************************/
Status FLASH_Block64KErase(uint32_t blockNumber) 
{
  uint32_t blockAddress;
  volatile uint32_t localCnt = 0;
  
  if(!(blockNumber > FLASH_LAST_64K_BLOCK) ) 
  {
    /* Enable flash write */
    FLASH_SetWriteEnableBit(ENABLE);
    
    /* Get start address of the block to be erased */
    blockAddress = blockNumber * FLASH_64K_BLOCK_SIZE;
    
    /* Set address counter */
    QSPI_SetAddrCnt(QSPI_ADDR_CNT_3BYTE);
    
    /* Set QSPI address pin mode */
    QSPI->CONF.BF.ADDR_PIN = QSPI_ADDR_PIN_SINGLE;
    
    /* Set address */ 
    QSPI_SetAddr(blockAddress);
    
    /* Set instruction */
    QSPI_SetInstr(FLASH_INS_CODE_BE_64K);
    
    /* Set QSPI write */
    QSPI_StartTransfer(QSPI_W_EN);
    
    /* Stop QSPI transfer */
    QSPI_StopTransfer();
    
    while(localCnt++ < 2000000)
    {
      /* Check flash busy status */ 
      if( FLASH_GetBusyStatus() == RESET )
      {
        return SUCCESS;
      }
    }
  }
  
  return ERROR;
}

/****************************************************************************//**
 * @brief      Erase specfied address of the flash
 *
 * @param[in]  startAddr:  Start address to be erased
 * @param[in]  endAddr:  End address to be erased
 *
 * @return     SUCCESS or ERROR
 *
 *******************************************************************************/
Status FLASH_Erase(uint32_t startAddr, uint32_t endAddr) 
{
  uint32_t startSector, endSector, sectorNumber;

  /* Get sector number of start address */
  startSector = (startAddr / FLASH_SECTOR_SIZE);
  /* Get sector number of end address */
  endSector = (endAddr / FLASH_SECTOR_SIZE);

  for(sectorNumber = startSector; sectorNumber <= endSector; sectorNumber++)
  {
    if(FLASH_SectorErase(sectorNumber) == ERROR)
    {
      return ERROR;
    }
  }
  
  return SUCCESS;
}

/****************************************************************************//**
 * @brief      Read flash from specified address to buffer
 *
 * @param[in]  readMode:  Flash reading mode to be set
 * @param[in]  address:  Flash address to be read
 * @param[in]  buffer:  Buffer to hold data read from flash
 * @param[in]  num:  Number of data to be read from flash
 *
 * @return     Number of data read out from flash, in byte
 *
 *******************************************************************************/
uint32_t FLASH_Read(FLASH_ReadMode_Type readMode, uint32_t address, uint8_t *buffer, uint32_t num) 
{
  uint32_t i;
  uint32_t readBytes;
  uint16_t statusLow, statusHigh, statusWrite; 
    
  readBytes = 0;
  
  if( (readMode == FLASH_FAST_READ_QUAD_OUT) || (readMode == FLASH_FAST_READ_QUAD_IO)
    ||(readMode == FLASH_WORD_FAST_READ_QUAD_IO) || (readMode == FLASH_OCTAL_WORD_FAST_READ_QUAD_IO) )
  {
    statusLow = FLASH_GetStatus(FLASH_STATUS_LO);
    statusHigh = FLASH_GetStatus(FLASH_STATUS_HI);
    statusWrite = ((statusHigh<<8) | statusLow) | 0x0200;
    FLASH_WriteStatus(statusWrite);    
  }  
  
  /* Clear QSPI FIFO */ 
  QSPI_FlushFIFO();  

  /* Set Header count register: instruction counter, address counter, read mode counter and dummy counter */
  QSPI_SetHdrcnt(QSPI_INSTR_CNT_1BYTE, QSPI_ADDR_CNT_3BYTE, QSPI_RM_CNT_0BYTE, QSPI_DUMMY_CNT_0BYTE);
    
  /* Set read mode */
  QSPI_SetRMode(0);  

  /* Set data in counter */
  QSPI_SetDInCnt(num);  
  
  /* Set address */ 
  QSPI_SetAddr(address);
  
  /* Set QSPI address pin mode */
  QSPI->CONF.BF.ADDR_PIN = QSPI_ADDR_PIN_SINGLE;     
  
  /* Set QSPI data pin mode */
  QSPI->CONF.BF.DATA_PIN = QSPI_DATA_PIN_SINGLE;   

  if(readMode == FLASH_NORMAL_READ)
  {
    /* Set instruction */
    QSPI_SetInstr(FLASH_INS_CODE_RD);
  }
  else if(readMode == FLASH_FAST_READ)
  {
    /* Set instruction */
    QSPI_SetInstr(FLASH_INS_CODE_FR);
    
    /* Set dummy counter */
    QSPI_SetDummyCnt(QSPI_DUMMY_CNT_1BYTE);
  }
  else if(readMode == FLASH_FAST_READ_DUAL_OUT)
  {
    /* Set instruction */
    QSPI_SetInstr(FLASH_INS_CODE_FRDO);
    
    /* Set dummy counter */
    QSPI_SetDummyCnt(QSPI_DUMMY_CNT_1BYTE);
    
    /* Set QSPI data pin mode */
    QSPI->CONF.BF.DATA_PIN = QSPI_DATA_PIN_DUAL;  
  }
  else if(readMode == FLASH_FAST_READ_DUAL_IO)
  {
    /* Set instruction */
    QSPI_SetInstr(FLASH_INS_CODE_FRDIO);
    
    /* Set read mode counter */
    QSPI_SetRModeCnt(QSPI_RM_CNT_1BYTE);
    
    /* Set QSPI data pin mode */
    QSPI->CONF.BF.DATA_PIN = QSPI_DATA_PIN_DUAL;
    
    /* Set QSPI address pin mode */
    QSPI->CONF.BF.ADDR_PIN = QSPI_ADDR_PIN_AS_DATA;   
  }    
  else if(readMode == FLASH_FAST_READ_QUAD_OUT)
  {
    /* Set instruction */
    QSPI_SetInstr(FLASH_INS_CODE_FRQO);
    
    /* Set dummy counter */
    QSPI_SetDummyCnt(QSPI_DUMMY_CNT_1BYTE);
    
    /* Set QSPI data pin mode */
    QSPI->CONF.BF.DATA_PIN = QSPI_DATA_PIN_QUAD;  
  }
  else if(readMode == FLASH_FAST_READ_QUAD_IO)
  {
    /* Set instruction */
    QSPI_SetInstr(FLASH_INS_CODE_FRQIO);
 
    /* Set read mode counter */
    QSPI_SetRModeCnt(QSPI_RM_CNT_1BYTE);
        
    /* Set dummy counter */
    QSPI_SetDummyCnt(QSPI_DUMMY_CNT_2BYTE);    
    
    /* Set QSPI data pin mode */
    QSPI->CONF.BF.DATA_PIN = QSPI_DATA_PIN_QUAD;
    
    /* Set QSPI address pin mode */
    QSPI->CONF.BF.ADDR_PIN = QSPI_ADDR_PIN_AS_DATA;      
  }
  else if(readMode == FLASH_WORD_FAST_READ_QUAD_IO)
  {
    /* Set instruction */
    QSPI_SetInstr(FLASH_INS_CODE_WFRQIO);
    
    /* Set read mode counter */
    QSPI_SetRModeCnt(QSPI_RM_CNT_1BYTE);
        
    /* Set dummy counter */
    QSPI_SetDummyCnt(QSPI_DUMMY_CNT_1BYTE);    
    
    /* Set QSPI data pin mode */
    QSPI->CONF.BF.DATA_PIN = QSPI_DATA_PIN_QUAD;
    
    /* Set QSPI address pin mode */
    QSPI->CONF.BF.ADDR_PIN = QSPI_ADDR_PIN_AS_DATA;         
  }
  else if(readMode == FLASH_OCTAL_WORD_FAST_READ_QUAD_IO)
  {
    /* Set instruction */
    QSPI_SetInstr(FLASH_INS_CODE_OWFRQIO);
    
    /* Set read mode counter */
    QSPI_SetRModeCnt(QSPI_RM_CNT_1BYTE);
        
    /* Set QSPI data pin mode */
    QSPI->CONF.BF.DATA_PIN = QSPI_DATA_PIN_QUAD;
    
    /* Set QSPI address pin mode */
    QSPI->CONF.BF.ADDR_PIN = QSPI_ADDR_PIN_AS_DATA;         
  }
    
  /* QSPI one-byte length mode */
  QSPI->CONF.BF.BYTE_LEN = QSPI_BYTE_LEN_1BYTE;
  
  /* Set QSPI read */
  QSPI_StartTransfer(QSPI_R_EN);
    
  for (i=0; i<num; i++)
  {
    /* Waiting for RFIFO not empty */
    while(QSPI->CNTL.BF.RFIFO_EMPTY == 1);
    
    buffer[i] = (QSPI->DIN.WORDVAL) & 0xFF;
    readBytes++;
  } 
  
  /* Disable QSPI */
  QSPI_SetSSEnable(DISABLE); 
  
  return readBytes;
}

/****************************************************************************//**
 * @brief      Read a word from specified flash address
 *
 * @param[in]  readMode:  Flash reading mode to be set
 * @param[in]  address:  Flash address to be read
 *
 * @return     Data in word
 *
 *******************************************************************************/
uint32_t FLASH_WordRead(FLASH_ReadMode_Type readMode, uint32_t address) 
{
  uint32_t data = 0;
  FLASH_Read(readMode, address, (uint8_t*)&data, 4);
  return data;
}

/****************************************************************************//**
 * @brief      Read a byte from specified flash address
 *
 * @param[in]  readMode:  Flash reading mode to be set
 * @param[in]  address:  Flash address to be read
 *
 * @return     Data in byte
 *
 *******************************************************************************/
uint8_t FLASH_ByteRead(FLASH_ReadMode_Type readMode, uint32_t address) 
{
  uint8_t data = 0;
  FLASH_Read(readMode, address, (uint8_t*)&data, 1);
  return data;
}

/****************************************************************************//**
 * @brief      Write flash within a page
 *
 * @param[in]  programMode:  Flash program mode to be set
 * @param[in]  address:  Page address
 * @param[in]  buffer:  Buffer data to be programmed to flash
 * @param[in]  num:  Number of data to be programmed to flash
 *
 * @return     SUCCESS or ERROR
 *
 *******************************************************************************/
Status FLASH_PageWrite(FLASH_ProgramMode_Type programMode, uint32_t address, uint8_t *buffer, uint32_t num) 
{
  uint32_t i;
  volatile uint32_t localCnt = 0;
  uint16_t statusLow, statusHigh, statusWrite; 
  
  if(programMode == FLASH_PROGRAM_QUAD)
  {
    statusLow = FLASH_GetStatus(FLASH_STATUS_LO);
    statusHigh = FLASH_GetStatus(FLASH_STATUS_HI);
    statusWrite = ((statusHigh<<8) | statusLow) | 0x0200;
    FLASH_WriteStatus(statusWrite);    
  }    
  
  /* Check address validity */
  if ((FLASH_PAGE_NUM(address+num-1) > FLASH_PAGE_NUM(address)) || num == 0)
  {
    return ERROR;
  }
  
  /* Enable flash write */
  FLASH_SetWriteEnableBit(ENABLE);
  
  /* Set Header count register: instruction counter, address counter, read mode counter and dummy counter */
  QSPI_SetHdrcnt(QSPI_INSTR_CNT_1BYTE, QSPI_ADDR_CNT_3BYTE, QSPI_RM_CNT_0BYTE, QSPI_DUMMY_CNT_0BYTE);
  
  /* Set QSPI address pin mode */
  QSPI->CONF.BF.ADDR_PIN = QSPI_ADDR_PIN_SINGLE;
  
  /* Set QSPI data pin mode */
  QSPI->CONF.BF.DATA_PIN = QSPI_DATA_PIN_SINGLE;   
  
  /* Set address */ 
  QSPI_SetAddr(address);
  
  if(programMode == FLASH_PROGRAM_NORMAL)
  {
    /* Set instruction */
    QSPI_SetInstr(FLASH_INS_CODE_PP);    
  }
  else if(programMode == FLASH_PROGRAM_QUAD)
  {
    /* Set instruction */
    QSPI_SetInstr(FLASH_INS_CODE_QPP);    
    
    /* Set QSPI data pin mode */
    QSPI->CONF.BF.DATA_PIN = QSPI_DATA_PIN_QUAD;  
  }
  
  /* QSPI one-byte length mode */
  QSPI->CONF.BF.BYTE_LEN = QSPI_BYTE_LEN_1BYTE;
  
  /* Set QSPI write */
  QSPI_StartTransfer(QSPI_W_EN);
    
  for (i=0; i<num; i++) 
  {
    QSPI_WriteByte(buffer[i]);
  }
  
  /* Stop QSPI transfer */
  QSPI_StopTransfer();
  
  while(localCnt++ < 1000000)
  {
    /* Check flash busy status */ 
    if( FLASH_GetBusyStatus() == RESET )
    {
      return SUCCESS;
    }
  }  

  return ERROR;
}

/****************************************************************************//**
 * @brief      Write flash with any address and size
 *
 * @param[in]  programMode:  Flash program mode to be set
 * @param[in]  address:  Page address
 * @param[in]  buffer:  Buffer data to be programmed to flash
 * @param[in]  num:  Number of data to be programmed to flash
 *
 * @return     SUCCESS or ERROR
 *
 *******************************************************************************/
Status FLASH_Write(FLASH_ProgramMode_Type programMode, uint32_t address, uint8_t *buffer, uint32_t num) 
{
  uint8_t *pBuf;
  uint32_t begPgNum;
  uint32_t endPgNum;
  uint32_t step;
  uint32_t addrCur;
  uint32_t i;
  Status funcStatus = SUCCESS;  
  
  pBuf = buffer;
  addrCur = address;
  
  /* Get page number of start address */
  begPgNum = FLASH_PAGE_NUM(address);
  /* Get page number of end address */
  endPgNum = FLASH_PAGE_NUM(address + num - 1);

  /* Both start address and end address are within the same page */
  if(begPgNum == endPgNum)
  {
    return( FLASH_PageWrite(programMode, address, buffer, num) );
  } 
  /* Start address and end address are not in the same page */
  else
  {
    /* For first page */
    step = FLASH_PAGE_END_ADDR(address)-address+1;
    funcStatus = FLASH_PageWrite(programMode, address, pBuf, step);
    if(funcStatus == ERROR)
    {
      return ERROR;
    }
    
    pBuf += step;
    addrCur += step;

    for(i=begPgNum+1; i<=endPgNum; i++)
    {
      /* For last page */
      if(i == endPgNum)
      {
        step = (address + num) & 0xFF;
        
        /* If step is 0, the last page has 256 bytes data to be writen ( num of data is 0x100 ) */
        if(step == 0)
        {
          step = 0x100;
        }
        
        return( FLASH_PageWrite(programMode, addrCur, pBuf, step) );
      } 
      else
      {
        funcStatus = FLASH_PageWrite(programMode, addrCur, pBuf, FLASH_PAGE_SIZE);
        if(funcStatus == ERROR)
        {
          return ERROR;
        }
        
        pBuf += FLASH_PAGE_SIZE;
        addrCur += FLASH_PAGE_SIZE;
      }
    }
  }
  
  return funcStatus;
}

/****************************************************************************//**
 * @brief      Write a word to specified flash address
 *
 * @param[in]  programMode:  Flash program mode to be set
 * @param[in]  address:  Flash address to be programmed
 * @param[in]  data:  Data to be programmed to flash
 *
 * @return     SUCCESS or ERROR
 *
 *******************************************************************************/
Status FLASH_WordWrite(FLASH_ProgramMode_Type programMode, uint32_t address, uint32_t data) 
{
  return FLASH_Write(programMode, address, (uint8_t*)&data, 4);
}

/****************************************************************************//**
 * @brief      Write a byte to specified flash address
 *
 * @param[in]  address:  Flash address to be programmed
 * @param[in]  data:  Data to be programmed to flash
 *
 * @return     SUCCESS or ERROR
 *
 *******************************************************************************/
Status FLASH_ByteWrite(FLASH_ProgramMode_Type programMode, uint32_t address, uint8_t data) 
{
  return FLASH_Write(programMode, address, (uint8_t*)&data, 1);
}

/****************************************************************************//**
 * @brief      Get flash unique ID
 *
 * @param none
 *
 * @return     Unique ID
 *
 *******************************************************************************/
uint64_t FLASH_GetUniqueID(void)
{
  uint64_t uniqueID;
  
  QSPI_FlushFIFO();  
  
  /* Set Header count register: instruction counter, address counter, read mode counter and dummy counter*/
  QSPI_SetHdrcnt(QSPI_INSTR_CNT_1BYTE, QSPI_ADDR_CNT_1BYTE, QSPI_RM_CNT_0BYTE, QSPI_DUMMY_CNT_3BYTE);
  
  QSPI_SetDInCnt(8);
  
  QSPI_SetAddr(0);
  
  /* Set QSPI address pin mode */
  QSPI->CONF.BF.ADDR_PIN = QSPI_ADDR_PIN_SINGLE;
  
  /* Set QSPI data pin mode */
  QSPI->CONF.BF.DATA_PIN = QSPI_DATA_PIN_SINGLE;   

  /* Read Unique ID number */
  QSPI_SetInstr(FLASH_INS_CODE_RUID);
    
  /* QSPI one-byte length mode */
  QSPI->CONF.BF.BYTE_LEN = QSPI_BYTE_LEN_1BYTE;

  QSPI_StartTransfer(QSPI_R_EN);

  uniqueID = QSPI_ReadByte();
  uniqueID <<= 8;
  uniqueID |= QSPI_ReadByte();
  uniqueID <<= 8;
  uniqueID |= QSPI_ReadByte();
  uniqueID <<= 8;
  uniqueID |= QSPI_ReadByte();
  uniqueID <<= 8;
  uniqueID |= QSPI_ReadByte();
  uniqueID <<= 8;
  uniqueID |= QSPI_ReadByte();
  uniqueID <<= 8;
  uniqueID |= QSPI_ReadByte();
  uniqueID <<= 8;
  uniqueID |= QSPI_ReadByte();  
  
  QSPI_SetSSEnable(DISABLE);
  
  return uniqueID;
}
