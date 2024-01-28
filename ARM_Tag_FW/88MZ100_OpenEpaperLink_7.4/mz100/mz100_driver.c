#include "mz100_driver.h"
#include "mz100.h"

/**  
 *  @brief UART interrupt callback function pointer array
 */
intCallback_Type * uart1IntCb[]= {NULL, NULL, NULL, NULL, NULL, NULL};
intCallback_Type * uart2IntCb[]= {NULL, NULL, NULL, NULL, NULL, NULL};

/**  
 *  @brief RTC interrupt callback function pointer array
 */
intCallback_Type * rtcIntCb[]= {NULL, NULL, NULL, NULL,NULL,NULL,NULL,NULL};

/**  
 *  @brief I2C interrupt callback function pointer array
 */
intCallback_Type * i2c1IntCb[]= {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};
intCallback_Type * i2c2IntCb[]= {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};

/**  
 *  @brief SPI interrupt callback function pointer array
 */
intCallback_Type * ssp1IntCb[]= {NULL, NULL, NULL, NULL,NULL,NULL,NULL,NULL};
intCallback_Type * ssp2IntCb[]= {NULL, NULL, NULL, NULL,NULL,NULL,NULL,NULL};

/**  
 *  @brief GPT interrupt callback function pointer array
 */
intCallback_Type * gpt1IntCb[]= {NULL, NULL, NULL, NULL,NULL,NULL,NULL,NULL, NULL, NULL, NULL, NULL, NULL, NULL,};
intCallback_Type * gpt2IntCb[]= {NULL, NULL, NULL, NULL,NULL,NULL,NULL,NULL, NULL, NULL, NULL, NULL, NULL, NULL,};

/**  
 *  @brief CRC interrupt callback function pointer array
 */
intCallback_Type * crcIntCb[]= {NULL, NULL, NULL, NULL,NULL,NULL,NULL,NULL};

/**  
 *  @brief AES interrupt callback function pointer array
 */
intCallback_Type * aesIntCb[]= {NULL, NULL, NULL, NULL,NULL,NULL,NULL,NULL};

/**  
 *  @brief ADC interrupt callback function pointer array
 */
intCallback_Type * adcIntCb[]= {NULL, NULL, NULL, NULL, NULL};

/**  
 *  @brief DAC interrupt callback function pointer array
 */
intCallback_Type * dacIntCb[]= {NULL, NULL, NULL, NULL, NULL};

/**  
 *  @brief ACOMP interrupt callback function pointer array
 */
intCallback_Type * acompIntCb[]= {NULL, NULL, NULL, NULL};

/**  
 *  @brief GPIO interrupt callback function pointer array
 */
intCallback_Type * gpioIntCb[]= { NULL, NULL, NULL, NULL,NULL,NULL,NULL,NULL,NULL,NULL,
                                  NULL, NULL, NULL, NULL,NULL,NULL,NULL,NULL,NULL,NULL,
                                  NULL, NULL, NULL, NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL};

/**  
 *  @brief DMA interrupt callback function pointer array
 */
intCallback_Type * dmaCh0IntCb[]= {NULL, NULL, NULL, NULL,NULL};
intCallback_Type * dmaCh1IntCb[]= {NULL, NULL, NULL, NULL,NULL};
intCallback_Type * dmaCh2IntCb[]= {NULL, NULL, NULL, NULL,NULL};
intCallback_Type * dmaCh3IntCb[]= {NULL, NULL, NULL, NULL,NULL};
intCallback_Type * dmaCh4IntCb[]= {NULL, NULL, NULL, NULL,NULL};
intCallback_Type * dmaCh5IntCb[]= {NULL, NULL, NULL, NULL,NULL};

/**  
 *  @brief QSPI interrupt callback function pointer array
 */
intCallback_Type * qspiIntCb[]= {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};

/**  
 *  @brief WDT interrupt callback function pointer array
 */
intCallback_Type * wdtIntCb[]= {NULL, NULL, NULL, NULL, NULL};

/**  
 *  @brief RC32M interrupt callback function pointer array
 */
intCallback_Type * rc32mIntCb[]= {NULL, NULL};

/**  
 *  @brief XTAL32M interrupt callback function pointer array
 */
intCallback_Type * xtal32mIntCb[]= {NULL, NULL, NULL};

/**  
 *  @brief MPU interrupt callback function pointer array
 */
intCallback_Type * mpuIntCb[]= {NULL, NULL, NULL, NULL};

/**  
 *  @brief MPU interrupt callback function pointer array
 */
intCallback_Type * threedgIntCb[]= {NULL, NULL, NULL, NULL, NULL, NULL,};

/**  
 *  @brief MPU interrupt callback function pointer array
 */
intCallback_Type * keyscanIntCb[]= {NULL, NULL,};

/**  
 *  @brief MPU interrupt callback function pointer array
 */
intCallback_Type * irIntCb[]= {NULL, NULL, NULL, NULL, NULL, NULL,};

/**  
 *  @brief MPU interrupt callback function pointer array
 */
intCallback_Type * trackballIntCb[]= {NULL, NULL, NULL, NULL, NULL, NULL,};

/**  
 *  @brief MZ100 interrupt callback function pointer array
 */
intCallback_Type ** intCbfArra[] = {uart1IntCb, uart2IntCb, rtcIntCb, i2c1IntCb, i2c2IntCb,
                                    ssp1IntCb, ssp2IntCb, gpt1IntCb, gpt2IntCb, crcIntCb,
                                    aesIntCb, adcIntCb, dacIntCb, acompIntCb, 
                                    gpioIntCb, dmaCh0IntCb, dmaCh1IntCb, dmaCh2IntCb,dmaCh3IntCb, 
                                    dmaCh4IntCb, dmaCh5IntCb, qspiIntCb, wdtIntCb, rc32mIntCb, xtal32mIntCb, 
                                    mpuIntCb,threedgIntCb, keyscanIntCb, irIntCb, trackballIntCb};  

/*@} end of group DRIVER_Global_Variables */

/** @defgroup DRIVER_Private_FunctionDeclaration
 *  @{
 */

/*@} end of group DRIVER_Private_FunctionDeclaration */

/** @defgroup DRIVER_Private_Functions
 *  @{
 */

/*@} end of group DRIVER_Private_Functions */

/** @defgroup DRIVER_Public_Functions
 *  @{
 */
 
/****************************************************************************//**
 * @brief      Install interrupt callback function for given interrupt type and peripheral 
 *
 * @param[in]  intPeriph:  Select the peripheral, such as INT_UART1,INT_AES
 * @param[in]  intType: Specifies the interrupt type. must be select interrupt enum type 
 *             defined as XXXX_INT_Type(XXXX here is peripher name) in mz100_xxx.h. 
               There is a exception for GPIO,that interrupt type is the GPIO_NO_Type.
 * @param[in]  cbFun:  Pointer to interrupt callback function. The type should be 
 *             void (*fn)(void).
 *
 * @return none
 *
 *******************************************************************************/
void install_int_callback(INT_Peripher_Type intPeriph, uint32_t intType, intCallback_Type * cbFun)
{
  /* Check the parameters */
  CHECK_PARAM(IS_INT_PERIPH(intPeriph));

  intCbfArra[intPeriph][intType] = cbFun;
}
