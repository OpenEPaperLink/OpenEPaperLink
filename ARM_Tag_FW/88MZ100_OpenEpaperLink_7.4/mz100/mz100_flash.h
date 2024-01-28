#pragma once

#include "mz100.h"
#include "mz100_driver.h"

/** @addtogroup MZ100_Periph_Driver
 *  @{
 */

/** @addtogroup FLASH 
 *  @{
 */
  
/** @defgroup FLASH_Public_Types FLASH_Public_Types
 *  @{
 */

/**  
 *  @brief FLASH protection type
 */ 
typedef enum
{
  FLASH_PROT_NONE        = 0x00,      /*!< None protection */
  FLASH_PROT_UPPER_64KB  = 0x04,      /*!< Protect upper 64KB  0x070000 ~ 0x07FFFF */
  FLASH_PROT_UPPER_128KB = 0x08,      /*!< Protect upper 128KB 0x060000 ~ 0x07FFFF */
  FLASH_PROT_UPPER_256KB = 0x0C,      /*!< Protect upper 256KB 0x040000 ~ 0x07FFFF */
  FLASH_PROT_LOWER_64KB  = 0x24,      /*!< Protect lower 64KB  0x000000 ~ 0x00FFFF */
  FLASH_PROT_LOWER_128KB = 0x28,      /*!< Protect lower 128KB 0x000000 ~ 0x01FFFF */
  FLASH_PROT_LOWER_256KB = 0x2C,      /*!< Protect lower 256KB 0x000000 ~ 0x03FFFF */  
  FLASH_PROT_ALL         = 0x10,      /*!< Protect all         0x000000 ~ 0x07FFFF */
  FLASH_PROT_UPPER_4KB   = 0x44,      /*!< Protect upper 4KB   0x07F000 ~ 0x07FFFF */
  FLASH_PROT_UPPER_8KB   = 0x48,      /*!< Protect upper 8KB   0x07E000 ~ 0x07FFFF */
  FLASH_PROT_UPPER_16KB  = 0x4C,      /*!< Protect upper 16KB  0x07C000 ~ 0x07FFFF */
  FLASH_PROT_UPPER_32KB  = 0x50,      /*!< Protect upper 32KB  0x078000 ~ 0x07FFFF */
  FLASH_PROT_LOWER_4KB   = 0x64,      /*!< Protect lower 4KB   0x000000 ~ 0x000FFF */
  FLASH_PROT_LOWER_8KB   = 0x68,      /*!< Protect lower 8KB   0x000000 ~ 0x001FFF */
  FLASH_PROT_LOWER_16KB  = 0x6C,      /*!< Protect lower 16KB  0x000000 ~ 0x003FFF */
  FLASH_PROT_LOWER_32KB  = 0x70,      /*!< Protect lower 32KB  0x000000 ~ 0x007FFF */
  FLASH_PROT_LOWER_448KB = 0x84,      /*!< Protect lower 448KB 0x000000 ~ 0x06FFFF */
  FLASH_PROT_LOWER_384KB = 0x88,      /*!< Protect lower 384KB 0x000000 ~ 0x05FFFF */
  FLASH_PROT_UPPER_448KB = 0xA4,      /*!< Protect upper 448KB 0x010000 ~ 0x07FFFF */
  FLASH_PROT_UPPER_384KB = 0xA8,      /*!< Protect upper 384KB 0x020000 ~ 0x07FFFF */
  FLASH_PROT_LOWER_508KB = 0xC4,      /*!< Protect lower 508KB 0x000000 ~ 0x07EFFF */
  FLASH_PROT_LOWER_504KB = 0xC8,      /*!< Protect lower 504KB 0x000000 ~ 0x07DFFF */
  FLASH_PROT_LOWER_496KB = 0xCC,      /*!< Protect lower 496KB 0x000000 ~ 0x07BFFF */
  FLASH_PROT_LOWER_480KB = 0xD0,      /*!< Protect lower 480KB 0x000000 ~ 0x077FFF */
  FLASH_PROT_UPPER_508KB = 0xE4,      /*!< Protect upper 508KB 0x001000 ~ 0x07FFFF */
  FLASH_PROT_UPPER_504KB = 0xE8,      /*!< Protect upper 504KB 0x002000 ~ 0x07FFFF */
  FLASH_PROT_UPPER_496KB = 0xEC,      /*!< Protect upper 496KB 0x004000 ~ 0x07FFFF */
  FLASH_PROT_UPPER_480KB = 0xF0,      /*!< Protect upper 480KB 0x008000 ~ 0x07FFFF */
}FLASH_Protection_Type; 

/**  
 *  @brief FLASH read mode
 */ 
typedef enum
{
  FLASH_NORMAL_READ,                  /*!< Normal read mode */
  FLASH_FAST_READ,                    /*!< Fast read mode */
  FLASH_FAST_READ_DUAL_OUT,           /*!< Fast read dual output mode */
  FLASH_FAST_READ_DUAL_IO,            /*!< Fast read dual IO mode */
  FLASH_FAST_READ_QUAD_OUT,           /*!< Fast read quad output mode */
  FLASH_FAST_READ_QUAD_IO,            /*!< Fast read quad IO mode */
  FLASH_WORD_FAST_READ_QUAD_IO,       /*!< Word fast read quad IO mode */
  FLASH_OCTAL_WORD_FAST_READ_QUAD_IO, /*!< Octal word fast read quad IO mode */
}FLASH_ReadMode_Type; 

/**  
 *  @brief FLASH program mode
 */ 
typedef enum
{
  FLASH_PROGRAM_NORMAL,               /*!< Normal page program mode */
  FLASH_PROGRAM_QUAD,                 /*!< Quad page program mode */
}FLASH_ProgramMode_Type; 

/*@} end of group FLASH_Public_Types definitions */

/** @defgroup FLASH_Public_Constants
 *  @{
 */ 

/*@} end of group FLASH_Public_Constants */

/** @defgroup FLASH_Public_Macro
 *  @{
 */

/*@} end of group FLASH_Public_Macro */

/** @defgroup FLASH_Public_FunctionDeclaration
 *  @brief FLASH functions statement
 *  @{
 */
FlagStatus FLASH_GetBusyStatus(void);

Status FLASH_SetProtectionMode(FLASH_Protection_Type protectMode);

Status FLASH_EraseAll(void);
Status FLASH_SectorErase(uint32_t sectorNumber);
Status FLASH_Block64KErase(uint32_t blockNumber);
Status FLASH_Block32KErase(uint32_t blockNumber);
Status FLASH_Erase(uint32_t startAddr, uint32_t endAddr);

uint32_t FLASH_Read(FLASH_ReadMode_Type readMode, uint32_t address, uint8_t *buffer, uint32_t num);
uint32_t FLASH_WordRead(FLASH_ReadMode_Type readMode, uint32_t address);
uint8_t FLASH_ByteRead(FLASH_ReadMode_Type readMode, uint32_t address);

Status FLASH_Write(FLASH_ProgramMode_Type programMode, uint32_t address, uint8_t *buffer, uint32_t num);
Status FLASH_PageWrite(FLASH_ProgramMode_Type programMode, uint32_t address, uint8_t *buffer, uint32_t num);
Status FLASH_WordWrite(FLASH_ProgramMode_Type programMode, uint32_t address, uint32_t data);
Status FLASH_ByteWrite(FLASH_ProgramMode_Type programMode, uint32_t address, uint8_t data);

uint64_t FLASH_GetUniqueID(void);
