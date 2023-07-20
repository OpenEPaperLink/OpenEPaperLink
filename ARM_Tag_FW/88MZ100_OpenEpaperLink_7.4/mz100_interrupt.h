#pragma once
typedef void( *intfunc )( void );
typedef union { intfunc __fun; void * __ptr; } intvec_elem;

typedef enum IRQn
{
/******  Cortex-M3 Processor Exceptions Numbers ***************************************************/
  NonMaskableInt_IRQn         = -14,    /*!< 2 Non Maskable Interrupt                             */
  MemoryManagement_IRQn       = -12,    /*!< 4 Cortex-M3 Memory Management Interrupt              */
  BusFault_IRQn               = -11,    /*!< 5 Cortex-M3 Bus Fault Interrupt                      */
  UsageFault_IRQn             = -10,    /*!< 6 Cortex-M3 Usage Fault Interrupt                    */
  SVCall_IRQn                 = -5,     /*!< 11 Cortex-M3 SV Call Interrupt                       */
  DebugMonitor_IRQn           = -4,     /*!< 12 Cortex-M3 Debug Monitor Interrupt                 */
  PendSV_IRQn                 = -2,     /*!< 14 Cortex-M3 Pend SV Interrupt                       */
  SysTick_IRQn                = -1,     /*!< 15 Cortex-M3 System Tick Interrupt                   */

/******  specific Interrupt Numbers ***************************************************************/

  RTC_IRQn                    = 0,      /*!< RTC Interrupt                                        */
  RC32K_IRQn                  = 1,      /*!< RTC Interrupt                                        */  
  XTAL32K_IRQn                = 2,      /*!< RTC Interrupt                                        */ 
  RC32M_IRQn                  = 3,      /*!< RTC Interrupt                                        */  
  XTAL64M_IRQn                = 4,      /*!< RTC Interrupt                                        */
  RC32KCAL_IRQn               = 5,      /*!< RTC Interrupt                                        */    
  BRNDET_IRQn                 = 6,      /*!< RTC Interrupt                                        */  
  LPCOMP_IRQn                 = 7,      /*!< RTC Interrupt                                        */
  ADC_IRQn                    = 8,      /*!< ADC Interrupt                                        */
  DAC_IRQn                    = 9,      /*!< DAC Interrupt                                        */ 
  ACOMP_IRQn                  = 10,     /*!< ACOMP Interrupt                                      */

  CRC_IRQn                    = 11,     /*!< CRC Interrupt                                        */
  AES_IRQn                    = 12,     /*!< AES Interrupt                                        */
  I2C1_IRQn                   = 13,     /*!< I2C1 Interrupt                                       */
  I2C2_IRQn                   = 14,     /*!< I2C2 Interrupt                                       */
  DMA_IRQn                    = 15,     /*!< DMA Interrupt                                        */
  GPIO_IRQn                   = 16,     /*!< GPIO Interrupt                                       */
  SSP1_IRQn                   = 17,     /*!< SSP1 Interrupt                                       */
  SSP2_IRQn                   = 18,     /*!< SSP2 Interrupt                                       */
  QSPI_IRQn                   = 19,     /*!< QSPI Interrupt                                       */  
  GPT1_IRQn                   = 20,     /*!< GPT1 Interrupt                                       */
  GPT2_IRQn                   = 21,     /*!< GPT2 Interrupt                                       */
  UART1_IRQn                  = 22,     /*!< UART1 Interrupt                                      */
  UART2_IRQn                  = 23,     /*!< UART2 Interrupt                                      */
  PHY1_IRQn                   = 24,     /*!< PHY1 Interrupt                                       */
  PHY2_IRQn                   = 25,     /*!< PHY2 Interrupt                                       */
  PHY3_IRQn                   = 26,     /*!< PHY3 Interrupt                                       */
  PHY4_IRQn                   = 27,     /*!< PHY4 Interrupt                                       */
  THREEDG_IRQn                = 28,     /*!< 3DG Interrupt                                        */
  KEYSCAN_IRQn                = 29,     /*!< KEYSCAN Interrupt                                    */
  IR_IRQn                     = 30,     /*!< IR Interrupt                                         */
  TRACKBALL_IRQn              = 31,     /*!< TRACKBALL Interrupt                                  */ 
  ExtPin1_IRQn                = 32,     /*!< GPIO1 Interrupt                                      */
  ExtPin2_IRQn                = 33,     /*!< GPIO2 Interrupt                                      */
  ExtPin3_IRQn                = 34,     /*!< GPIO3 Interrupt                                      */
  ExtPin4_IRQn                = 35,     /*!< GPIO4 Interrupt                                      */
  ExtPin5_IRQn                = 36,     /*!< GPIO5 Interrupt                                      */
  ExtPin6_IRQn                = 37,     /*!< GPIO6 Interrupt                                      */
  ExtPin7_IRQn                = 38,     /*!< GPIO7 Interrupt                                      */
  ExtPin8_IRQn                = 39,     /*!< GPIO8 Interrupt                                      */
  ExtPin9_IRQn                = 40,     /*!< GPIO9 Interrupt                                      */
  ExtPin10_IRQn               = 41,     /*!< GPIO10 Interrupt                                     */
  ExtPin11_IRQn               = 42,     /*!< GPIO11 Interrupt                                     */
  ExtPin12_IRQn               = 43,     /*!< GPIO12 Interrupt                                     */
  ExtPin13_IRQn               = 44,     /*!< GPIO13 Interrupt                                     */
  ExtPin14_IRQn               = 45,     /*!< GPIO14 Interrupt                                     */
  ExtPin15_IRQn               = 46,     /*!< GPIO15 Interrupt                                     */
  ExtPin16_IRQn               = 47,     /*!< GPIO16 Interrupt                                     */
  ExtPin17_IRQn               = 48,     /*!< GPIO17 Interrupt                                     */
  ExtPin18_IRQn               = 49,     /*!< GPIO18 Interrupt                                     */
  ExtPin19_IRQn               = 50,     /*!< GPIO19 Interrupt                                     */
  ExtPin20_IRQn               = 51,     /*!< GPIO20 Interrupt                                     */
  ExtPin21_IRQn               = 52,     /*!< GPIO21 Interrupt                                     */
  ExtPin22_IRQn               = 53,     /*!< GPIO22 Interrupt                                     */
  ExtPin23_IRQn               = 54,     /*!< GPIO23 Interrupt                                     */
  ExtPin24_IRQn               = 55,     /*!< GPIO24 Interrupt                                     */
  ExtPin25_IRQn               = 56,     /*!< GPIO25 Interrupt                                     */
  ExtPin26_IRQn               = 57,     /*!< GPIO26 Interrupt                                     */
  ExtPin27_IRQn               = 58,     /*!< GPIO27 Interrupt                                     */
  ExtPin28_IRQn               = 59,     /*!< GPIO28 Interrupt                                     */
  ExtPin29_IRQn               = 60,     /*!< GPIO29 Interrupt                                     */
  ExtPin30_IRQn               = 61,     /*!< GPIO30 Interrupt                                     */
  ExtPin31_IRQn               = 62,     /*!< GPIO31 Interrupt                                     */

} IRQn_Type;
