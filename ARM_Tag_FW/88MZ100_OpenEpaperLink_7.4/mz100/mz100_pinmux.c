#include "mz100.h"
#include "mz100_driver.h"
#include "mz100_gpio.h"
#include "mz100_pinmux.h"

/** @defgroup PINMUX_FUNCTION_MASK     
 *  @{
 */
#define IS_GPIO_PINMUXFUNMASK(GPIONO, PINMUXFUNMASK)           1
/*@} end of group PINMUX_FUNCTION_MASK */

#ifndef USE_BASICDRV_IN_ROM
/** @defgroup PINMUX_Public_Functions
 *  @{
 */
 
/****************************************************************************//**
 * @brief      GPIO pin mux function define
 *
 * @param[in]  gpioNo:  Select the GPIO pin.
 * @param[in]  PinMuxFun:  GPIO pin function, should be GPIOxx_PinMuxFunc_Type
 *
 * @return     Status: ERROR or SUCCESS
 *
 *******************************************************************************/
void GPIO_PinMuxFun(GPIO_NO_Type gpioNo, GPIO_PinMuxFunc_Type PinMuxFun)
{
  CHECK_PARAM(IS_GPIO_NO(gpioNo));
  CHECK_PARAM(IS_GPIO_PINMUXFUN(PinMuxFun));
  CHECK_PARAM(IS_GPIO_PINMUXFUNMASK(gpioNo, PinMuxFun));
  
  /* When function to be set is not GPIO function, clear PIO_PULL_SEL_R to make the pin to default mode */
  /* For GPIO1~31, function 0 is GPIO function */
  if(PinMuxFun==PINMUX_FUNCTION_0)
  {
    PINMUX->GPIO_PINMUX[gpioNo-1].BF.FSEL_XR = (PinMuxFun & 0x0F);
  }
  else
  {  
    PINMUX->GPIO_PINMUX[gpioNo-1].BF.PIO_PULL_SEL_R = 0;
    PINMUX->GPIO_PINMUX[gpioNo-1].BF.FSEL_XR = (PinMuxFun & 0x0F);
  }
}

/****************************************************************************//**
 * @brief      GPIO pin mode function define
 *
 * @param[in]  gpioNo:  Select the GPIO pin.
 * @param[in]  gpioPinMode:  GPIO pin mode, should be PINMODE_DEFAULT, PINMODE_PULLUP, 
 *                           PINMODE_PULLDOWN, PINMODE_NOPULL or PINMODE_TRISTATE.
 *                           when this pin is not configured as GPIO function, 
 *                           or the data transfer direction is not input,
 *                           PINMODE_PULLUP, PINMODE_PULLDOWN or PINMODE_TRISTATE has no use.
 *
 * @return none
 *
 *******************************************************************************/
void GPIO_PinModeConfig(GPIO_NO_Type gpioNo, GPIO_PINMODE_Type gpioPinMode)
{
  CHECK_PARAM(IS_GPIO_NO(gpioNo));
  CHECK_PARAM(IS_GPIO_PINMODE(gpioPinMode));
   
  switch(gpioPinMode)
  {
    case PINMODE_DEFAULT:
      /* Default */
      PINMUX->GPIO_PINMUX[gpioNo-1].BF.DI_EN = 0;
      PINMUX->GPIO_PINMUX[gpioNo-1].BF.PIO_PULL_SEL_R = 0;
      break;

    case PINMODE_PULLUP:
      /* Pullup */
      PINMUX->GPIO_PINMUX[gpioNo-1].BF.DI_EN = 0;
      PINMUX->GPIO_PINMUX[gpioNo-1].BF.PIO_PULL_SEL_R = 1;
      PINMUX->GPIO_PINMUX[gpioNo-1].BF.PIO_PULLUP_R = 1;
      PINMUX->GPIO_PINMUX[gpioNo-1].BF.PIO_PULLDN_R = 0;
      break;

    case PINMODE_PULLDOWN:
      /* Pulldown */
      PINMUX->GPIO_PINMUX[gpioNo-1].BF.DI_EN = 0;
      PINMUX->GPIO_PINMUX[gpioNo-1].BF.PIO_PULL_SEL_R = 1;
      PINMUX->GPIO_PINMUX[gpioNo-1].BF.PIO_PULLUP_R = 0;
      PINMUX->GPIO_PINMUX[gpioNo-1].BF.PIO_PULLDN_R = 1;
      break;

    case PINMODE_NOPULL:
      /* Nopull */
      PINMUX->GPIO_PINMUX[gpioNo-1].BF.DI_EN = 0;
      PINMUX->GPIO_PINMUX[gpioNo-1].BF.PIO_PULL_SEL_R = 1;
      PINMUX->GPIO_PINMUX[gpioNo-1].BF.PIO_PULLUP_R = 0;
      PINMUX->GPIO_PINMUX[gpioNo-1].BF.PIO_PULLDN_R = 0;
      break;

    case PINMODE_TRISTATE:
      /* Tristate */
      PINMUX->GPIO_PINMUX[gpioNo-1].BF.DI_EN = 1;
      PINMUX->GPIO_PINMUX[gpioNo-1].BF.PIO_PULL_SEL_R = 1;
      PINMUX->GPIO_PINMUX[gpioNo-1].BF.PIO_PULLUP_R = 0;
      PINMUX->GPIO_PINMUX[gpioNo-1].BF.PIO_PULLDN_R = 0;
      break;

    default:
      break;
  }
}

/****************************************************************************//**
 * @brief      GPIO pin mode function define
 *
 * @param[in]  gpioNo:  Select the GPIO pin.
 * @param[in]  pinOutputMode:  GPIO pin output mode, should be PIN_OUTPUT_MODE_NORMAL_FUNCTION 
 *                             or PIN_OUTPUT_MODE_PAD.
 *
 * @return none
 *
 *******************************************************************************/
void GPIO_PinOutputModeConfig(GPIO_NO_Type gpioNo, GPIO_PinOutputMode_Type pinOutputMode)
{
  CHECK_PARAM(IS_GPIO_NO(gpioNo));
  CHECK_PARAM(IS_GPIO_PIN_OUTPUT_MODE(pinOutputMode));
  
  if(pinOutputMode == PIN_OUTPUT_MODE_PAD)
  {
    PINMUX->GPIO_PINMUX[gpioNo-1].BF.DO_SEL = 1;   
  }
  else
  {
    PINMUX->GPIO_PINMUX[gpioNo-1].BF.DO_SEL = 0;   
  }
}
#endif // USE_BASICDRV_IN_ROM

/****************************************************************************//**
 * @brief      GPIO pin pad output enable function define
 *
 * @param[in]  gpioNo:  Select the GPIO pin.
 *
 * @return none
 *
 *******************************************************************************/
void GPIO_PinPadOutputEnable(GPIO_NO_Type gpioNo)
{
  CHECK_PARAM(IS_GPIO_NO(gpioNo));

  /* Pin pad output enable */
  PINMUX->GPIO_PINMUX[gpioNo-1].BF.DO_EN = 1;   
}

/****************************************************************************//**
 * @brief      GPIO pin pad output disable function define
 *
 * @param[in]  gpioNo:  Select the GPIO pin.
 *
 * @return none
 *
 *******************************************************************************/
void GPIO_PinPadOutputDisable(GPIO_NO_Type gpioNo)
{
  CHECK_PARAM(IS_GPIO_NO(gpioNo));

  /* Pin pad output disable */
  PINMUX->GPIO_PINMUX[gpioNo-1].BF.DO_EN = 0;   
}

/****************************************************************************//**
 * @brief      GPIO pin pad output level function define
 *
 * @param[in]  gpioNo:  Select the GPIO pin.
 * @param[in]  pinPadOutput:  Set GPIO pin pad output level, should be 
 *                            PIN_PAD_OUTPUT_LOW or PIN_PAD_OUTPUT_HIGH.
 *
 * @return none
 *
 *******************************************************************************/
void GPIO_PinPadOutputLevel(GPIO_NO_Type gpioNo, GPIO_PinPadOutput_Type pinPadOutput)
{
  CHECK_PARAM(IS_GPIO_NO(gpioNo));
  CHECK_PARAM(IS_GPIO_PIN_PAD_OUTPUT(pinPadOutput));
    
  /* Set pin pad output value */
  if(pinPadOutput == PIN_PAD_OUTPUT_LOW)
  {
    PINMUX->GPIO_PINMUX[gpioNo-1].BF.DO_VAL = 0;   
  }
  else
  {
    PINMUX->GPIO_PINMUX[gpioNo-1].BF.DO_VAL = 1;   
  }
  
}
