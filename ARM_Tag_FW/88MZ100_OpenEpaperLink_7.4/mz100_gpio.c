#include "mz100.h"
#include "mz100_driver.h"
#include "mz100_gpio.h"

/** @addtogroup  88MZ100_Periph_Driver
 *  @{
 */

/** @defgroup GPIO GPIO
 *  @brief GPIO driver modules
 *  @{
 */

/** @defgroup GPIO_Private_Type
 *  @{
 */

/*@} end of group GPIO_Private_Type*/

/** @defgroup GPIO_Private_Defines
 *  @{
 */

/*@} end of group GPIO_Private_Defines */

/** @defgroup GPIO_Private_Variables
 *  @{
 */

/*@} end of group GPIO_Private_Variables */

/** @defgroup GPIO_Global_Variables
 *  @{
 */

/*@} end of group GPIO_Global_Variables */

/** @defgroup GPIO_Private_FunctionDeclaration
 *  @{
 */

/*@} end of group GPIO_Private_FunctionDeclaration */

/** @defgroup GPIO_Private_Functions
 *  @{
 */

/*@} end of group GPIO_Private_Functions */

/** @defgroup GPIO_Public_Functions
 *  @{
 */
 
/****************************************************************************//**
 * @brief      Set one GPIO pin pin direction 
 *
 * @param[in]  gpioNo:  Select the GPIO pin.
 * @param[in]  dir:  Pin data direction, should be GPIO_INPUT or GPIO_OUTPUT.
 *
 * @return none
 *
 *******************************************************************************/
void GPIO_SetPinDir(GPIO_NO_Type gpioNo, GPIO_Dir_Type dir)
{
  CHECK_PARAM(IS_GPIO_NO(gpioNo));
  CHECK_PARAM(IS_GPIO_DDR(dir));

  /* Set GPIOx pin direction */
  if(dir == GPIO_INPUT)
  {
    GPIO->SWPORTA_DDR.WORDVAL &= ~(0x01 << gpioNo);
  }
  else
  {
    GPIO->SWPORTA_DDR.WORDVAL |= (0x01 << gpioNo);
  }
}

/****************************************************************************//**
 * @brief      Set GPIO pins pin direction 
 *
 * @param[in]  dataDir:  Pin data transfer direction.
 *
 * @return none
 *
 *******************************************************************************/
void GPIO_SetDir(uint32_t dataDir)
{
  /* Set GPIOs pin direction */
  GPIO->SWPORTA_DDR.WORDVAL = dataDir;
}

/****************************************************************************//**
 * @brief      Write GPIO pin output 
 *
 * @param[in]  gpioNo:  Select the GPIO pin.
 * @param[in]  bitVal:  Value to be write to GPIO pin, should be GPIO_LOW or GPIO_HIGH.
 *
 * @return none
 *
 *******************************************************************************/
void GPIO_WritePinOutput(GPIO_NO_Type gpioNo, GPIO_IO_Type bitVal)
{
  CHECK_PARAM(IS_GPIO_NO(gpioNo));
  CHECK_PARAM(IS_GPIO_IO(bitVal));
 
  /* Set GPIOx pin level */
  if(bitVal == GPIO_IO_LOW)
  {
    GPIO->SWPORTA_DR.WORDVAL &= ~(0x01 << gpioNo);
  }
  else
  {
    GPIO->SWPORTA_DR.WORDVAL |= (0x01 << gpioNo);
  }
}

/****************************************************************************//**
 * @brief      Write GPIO DR register value 
 *
 * @param[in]  data:  Value to be write to GPIO pin.
 *
 * @return none
 *
 *******************************************************************************/
void GPIO_WriteOutput(uint32_t data)
{
  /* Set GPIO pin data value */
  GPIO->SWPORTA_DR.WORDVAL = data;
}

/****************************************************************************//**
 * @brief      Read GPIO pin level of input or output
 *
 * @param[in]  gpioNo:  Select the GPIO pin.
 *
 * @return  Value read from GPIO pin, Should be GPIO_IO_LOW or GPIO_IO_HIGH
 *
 *******************************************************************************/
GPIO_IO_Type GPIO_ReadPinLevel(GPIO_NO_Type gpioNo)
{
  uint32_t gpioVal; 

  GPIO_IO_Type retVal;
  
  CHECK_PARAM(IS_GPIO_NO(gpioNo));

  /* Get the GPIO pin value regardless of input and output */ 
  gpioVal = GPIO->EXT_PORTA.WORDVAL & (0x01 << gpioNo); 

  retVal = (gpioVal? GPIO_IO_HIGH : GPIO_IO_LOW);

  return retVal;
}

/****************************************************************************//**
 * @brief      Read GPIO pin level of input or output
 *
 * @param[in]  none
 *
 * @return  Value read from GPIO exteral register
 *
 *******************************************************************************/
uint32_t GPIO_ReadLevel(void)
{
  return(GPIO->EXT_PORTA.WORDVAL);
}

/****************************************************************************//**
 * @brief      Configurate GPIOx interrupt level and polarity
 *
 * @param[in]  gpioNo:  Select the GPIO pin.
 * @param[in]  intLevel:  Interrupt level, Should be LEVEL_SENS or EDGE_SENS
 * @param[in]  intPolar:  Interrupt polarity, Should be ACTIVE_LOW or ACTIVE_HIGH
 *             
 * @return none
 *
 *******************************************************************************/
void GPIO_IntConfig(GPIO_NO_Type gpioNo, GPIO_IntConfig_Type* intConfg)
{
  CHECK_PARAM(IS_GPIO_NO(gpioNo));
  CHECK_PARAM(IS_GPIO_INTLEVEL(intConfg->intLevel));
  CHECK_PARAM(IS_GPIO_INTPOLA(intConfg->intPol));

  /* Enable/Disabe GPIO interrupt function */
  if(intConfg->gpioIntFunc == DISABLE)
  {
    GPIO->INTEN.WORDVAL &= ~(0x01 << gpioNo);
  }
  else
  {
    /* Set interrupt level */
    if(intConfg->intLevel == GPIO_INT_LEVEL_SENS)
    {
      GPIO->INTTYPE_LEVEL.WORDVAL &= ~(0x01 << gpioNo);
    }
    else
    {
      GPIO->INTTYPE_LEVEL.WORDVAL |= (0x01 << gpioNo);
    }
  
    /* Set interrupt polarity */
    if(intConfg->intPol == GPIO_INT_ACTIVE_LOW)
    {
      GPIO->INT_POLARITY.WORDVAL &= ~(0x01 << gpioNo);
    }
    else
    {
      GPIO->INT_POLARITY.WORDVAL |= (0x01 << gpioNo);
    }

    /* Enable the interrupt */
    GPIO->INTEN.WORDVAL |= (0x01 << gpioNo);
  }
}

/****************************************************************************//**
 * @brief      Mask/Unmask GPIOx interrupt function
 *
 * @param[in]  gpioNo:  Select the GPIO pin.
 * @param[in]  intMask:  Mask/Unmask Specified GPIO interrupt, 
 *             should be MASK or UNMASK.
 *
 * @return none
 *
 *******************************************************************************/
void GPIO_IntMask(GPIO_NO_Type gpioNo, IntMask_Type intMask)
{
  CHECK_PARAM(IS_GPIO_NO(gpioNo));
  CHECK_PARAM(IS_INTMASK(intMask));;

  /* Set interrupt mask */
  if(intMask == UNMASK)
  {
    GPIO->INTMASK.WORDVAL &= ~(0x01 << gpioNo);
  }
  else
  {
    GPIO->INTMASK.WORDVAL |= (0x01 << gpioNo);
  }
}

/****************************************************************************//**
 * @brief      Get GPIO pin interrupt status
 *
 * @param[in]  gpioNo:  Select the GPIO pin.
 *
 * @return  The state value of GPIOx interrupt
 *
 *******************************************************************************/
FlagStatus GPIO_GetIntStatus(GPIO_NO_Type gpioNo)
{
  FlagStatus bitStatus = RESET;

  CHECK_PARAM(IS_GPIO_NO(gpioNo));

  if(GPIO->INTSTATUS.WORDVAL & (0x01 << gpioNo))
  {
    bitStatus = SET;
  }

  return bitStatus;  
}

/****************************************************************************//**
 * @brief      Clear GPIO pin interrupt 
 *
 * @param[in]  gpioNo:  Select the GPIO pin.
 *
 * @return  none
 *
 *******************************************************************************/
void GPIO_IntClr(GPIO_NO_Type gpioNo)
{
  CHECK_PARAM(IS_GPIO_NO(gpioNo));

  GPIO->PORTA_EOI.WORDVAL = (0x01 << gpioNo);
}

/****************************************************************************//**
 * @brief      GPIO interrupt handler 
 *
 * @param   none
 *
 * @return  none
 *
 *******************************************************************************/
__attribute__((weak)) void GPIO_IRQHandler(void)
{
  uint32_t i, intStatus;

  /* Get current interrupt status */
  intStatus = GPIO->INTSTATUS.WORDVAL;

  /* Clear the generated interrupts */
  GPIO->PORTA_EOI.WORDVAL = intStatus;
  
  /* Check which GPIO pin has interrupt */ 
  for(i = GPIO_MinNo; i <= GPIO_MaxNo; i++)
  {
    if(intStatus & (0x01 << i))
    {
      /* Call interrupt callback function */
      if(intCbfArra[INT_GPIO][i] != NULL)
      {
        intCbfArra[INT_GPIO][i]();
      }
      /* Disable interrupt if interrupt callback is not install */
      else
      {
        GPIO->INTEN.WORDVAL &= ~(0x01 << i);
      }
    }
  }
}
