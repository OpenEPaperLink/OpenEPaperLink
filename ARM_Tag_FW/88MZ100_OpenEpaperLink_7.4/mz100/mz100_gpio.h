#pragma once

#include "mz100.h"
#include "mz100_driver.h"

/** @addtogroup  88MZ100_Periph_Driver
 *  @{
 */

/** @addtogroup  GPIO 
 *  @{
 */
  
/** @defgroup GPIO_Public_Types GPIO_Public_Types 
 *  @{
 */

/**  
 *  @brief GPIO No. type definition 
 */
typedef enum
{
  GPIO_1 = 1,                       /*!< GPIO1 Pin define */
  GPIO_2,                           /*!< GPIO2 Pin define */
  GPIO_3,                           /*!< GPIO3 Pin define */
  GPIO_4,                           /*!< GPIO4 Pin define */
  GPIO_5,                           /*!< GPIO5 Pin define */
  GPIO_6,                           /*!< GPIO6 Pin define */
  GPIO_7,                           /*!< GPIO7 Pin define */
  GPIO_8,                           /*!< GPIO8 Pin define */
  GPIO_9,                           /*!< GPIO9 Pin define */
  GPIO_10,                          /*!< GPIO10 Pin define */
  GPIO_11,                          /*!< GPIO11 Pin define */
  GPIO_12,                          /*!< GPIO12 Pin define */
  GPIO_13,                          /*!< GPIO13 Pin define */
  GPIO_14,                          /*!< GPIO14 Pin define */
  GPIO_15,                          /*!< GPIO15 Pin define */
  GPIO_16,                          /*!< GPIO16 Pin define */
  GPIO_17,                          /*!< GPIO17 Pin define */
  GPIO_18,                          /*!< GPIO18 Pin define */
  GPIO_19,                          /*!< GPIO19 Pin define */
  GPIO_20,                          /*!< GPIO20 Pin define */
  GPIO_21,                          /*!< GPIO21 Pin define */
  GPIO_22,                          /*!< GPIO22 Pin define */
  GPIO_23,                          /*!< GPIO23 Pin define */
  GPIO_24,                          /*!< GPIO24 Pin define */
  GPIO_25,                          /*!< GPIO25 Pin define */
  GPIO_26,                          /*!< GPIO26 Pin define */
  GPIO_27,                          /*!< GPIO27 Pin define */
  GPIO_28,                          /*!< GPIO28 Pin define */
  GPIO_29,                          /*!< GPIO29 Pin define */
  GPIO_30,                          /*!< GPIO30 Pin define */
  GPIO_31,                          /*!< GPIO31 Pin define */ 
}GPIO_NO_Type;

/**  
 *  @brief GPIO data direction type definition 
 */
typedef enum
{
  GPIO_INPUT = 0,                        /*!< Set GPIO port data direction as input */  
  GPIO_OUTPUT,                             /*!< Set GPIO port data direction as output */
}GPIO_Dir_Type;

/**  
 *  @brief GPIO data Input/Output type definition 
 */
typedef enum
{
  GPIO_IO_LOW = 0,                          /*!< Set GPIO port data Input/Output value */  
  GPIO_IO_HIGH,                             /*!< Set GPIO port data Input/Output value */
}GPIO_IO_Type;

/**  
 *  @brief GPIO interrut level type definition 
 */
typedef enum
{
  GPIO_INT_LEVEL_SENS = 0,                   /*!< Interrupt type : Level sensiive */                                       
  GPIO_INT_EDGE_SENS,                        /*!< Interrupt type : Edge sensiive */
}GPIO_IntLevel_Type;

/**  
 *  @brief GPIO interrut polarity type definition 
 */
typedef enum
{
  GPIO_INT_ACTIVE_LOW = 0,                   /*!< For polarity of edge, falling-edge sensiive                                         
                                                  For polarity of level, active-low */
  GPIO_INT_ACTIVE_HIGH,                      /*!< For polarity of edge, rising-edge sensiive
                                                  For polarity of level, active-high */
}GPIO_IntPolarity_Type;

/**  
 *  @brief GPIO interrut polarity type definition 
 */
typedef struct
{
  FunctionalState  gpioIntFunc;             /*!< Enable/Disable GPIO interrupt function */                                         
  GPIO_IntLevel_Type intLevel;              /*!< Interrupt level */
  GPIO_IntPolarity_Type intPol;             /*!< Interrupt polarity */
}GPIO_IntConfig_Type;

 
/*@} end of group GPIO_Public_Types */

/** @defgroup GPIO_Public_Constants
 *  @{
 */ 
#define GPIO_MinNo        GPIO_1
#define GPIO_MaxNo        GPIO_31
                                      
/** @defgroup GPIO_Num        
 *  @{
 */
#define IS_GPIO_NO(GPIONO)                     (((GPIONO) >= GPIO_MinNo) && ((GPIONO) <= GPIO_MaxNo))
/*@} end of group GPIO_Num */

/** @defgroup GPIO_DataDir       
 *  @{
 */
#define IS_GPIO_DDR(GPIODDR)                   (((GPIODDR) == GPIO_INPUT) || ((GPIODDR) == GPIO_OUTPUT))                                                                        
/*@} end of group GPIO_DataDir */

/** @defgroup GPIO_IO      
 *  @{
 */
#define IS_GPIO_IO(GPIODDRIO)                  (((GPIODDRIO) == GPIO_IO_LOW) || ((GPIODDRIO) == GPIO_IO_HIGH))
/*@} end of group GPIO_IO */

/** @defgroup GPIO_INT_LEVEL      
 *  @{
 */
#define IS_GPIO_INTLEVEL(INTLEVEL)             (((INTLEVEL) == GPIO_INT_LEVEL_SENS) || ((INTLEVEL) == GPIO_INT_EDGE_SENS))
/*@} end of group GPIO_INT_LEVEL */

/** @defgroup GPIO_INT_POLA     
 *  @{
 */
#define IS_GPIO_INTPOLA(INTPOLA)               (((INTPOLA) == GPIO_INT_ACTIVE_LOW) || ((INTPOLA) == GPIO_INT_ACTIVE_HIGH))
/*@} end of group GPIO_INT_POLA */

/*@} end of group GPIO_Public_Constants */

/** @defgroup GPIO_Public_Macro
 *  @{
 */

/*@} end of group GPIO_Public_Macro */

/** @defgroup GPIO_Public_FunctionDeclaration
 *  @brief GPIO functions declaration
 *  @{
 */
void GPIO_SetPinDir(GPIO_NO_Type gpioNo, GPIO_Dir_Type dir);
void GPIO_SetDir(uint32_t dataDir);
void GPIO_WritePinOutput(GPIO_NO_Type gpioNo, GPIO_IO_Type bitVal);
void GPIO_WriteOutput(uint32_t data);
GPIO_IO_Type GPIO_ReadPinLevel(GPIO_NO_Type gpioNo);
uint32_t GPIO_ReadLevel(void);

void GPIO_IntConfig(GPIO_NO_Type gpioNo, GPIO_IntConfig_Type* intConfg);
void GPIO_IntMask(GPIO_NO_Type gpioNo, IntMask_Type intMask);
FlagStatus GPIO_GetIntStatus(GPIO_NO_Type gpioNo);
void GPIO_IntClr(GPIO_NO_Type gpioNo);
