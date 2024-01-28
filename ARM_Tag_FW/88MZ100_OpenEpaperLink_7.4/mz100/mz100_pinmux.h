#pragma once

#include "mz100.h"
#include "mz100_driver.h"
#include "mz100_gpio.h"

/** @addtogroup  88MZ100_Periph_Driver
 *  @{
 */

/** @addtogroup  PINMUX
 *  @{
 */

/** @defgroup PINMUX_Public_Types PINMUX_Public_Types
 *  @{
 */

/**
 *  @brief GPIO Pinmux function type definition
 */
typedef enum
{
  PINMUX_FUNCTION_0 = 0,                   /*!< GPIO pin mux function 0 define */
  PINMUX_FUNCTION_1,                       /*!< GPIO pin mux function 1 define */
  PINMUX_FUNCTION_2,                       /*!< GPIO pin mux function 2 define */
  PINMUX_FUNCTION_3,                       /*!< GPIO pin mux function 3 define */
  PINMUX_FUNCTION_4,                       /*!< GPIO pin mux function 4 define */
  PINMUX_FUNCTION_5,                       /*!< GPIO pin mux function 5 define */
  PINMUX_FUNCTION_6,                       /*!< GPIO pin mux function 6 define */
  PINMUX_FUNCTION_7,                       /*!< GPIO pin mux function 7 define */
  PINMUX_FUNCTION_8,                       /*!< GPIO pin mux function 8 define */
  PINMUX_FUNCTION_9,                       /*!< GPIO pin mux function 9 define */
  PINMUX_FUNCTION_10,                      /*!< GPIO pin mux function 10 define */
  PINMUX_FUNCTION_11,                      /*!< GPIO pin mux function 11 define */
  PINMUX_FUNCTION_12,                      /*!< GPIO pin mux function 12 define */
  PINMUX_FUNCTION_13,                      /*!< GPIO pin mux function 13 define */
  PINMUX_FUNCTION_14,                      /*!< GPIO pin mux function 14 define */
  PINMUX_FUNCTION_15,                      /*!< GPIO pin mux function 15 define */
}GPIO_PinMuxFunc_Type;

/**
 *  @brief GPIO pin mode type definition
 */
#ifndef USE_BASICDRV_IN_ROM
typedef enum
{
  PINMODE_DEFAULT = 0,                      /*!< GPIO pin mode normal define */
  PINMODE_PULLUP,                          /*!< GPIO pin mode pullup define */
  PINMODE_PULLDOWN,                        /*!< GPIO pin mode pulldown define */
  PINMODE_NOPULL,                          /*!< GPIO pin mode nopull define */
  PINMODE_TRISTATE,                        /*!< GPIO pin mode tristate define */
}GPIO_PINMODE_Type;
#else // USE_BASICDRV_IN_ROM
typedef enum
{
  PINMODE_DEFAULT = 0,                      /*!< GPIO pin mode normal define */
  PINMODE_NORMAL = 0,                      /*!< GPIO pin mode normal define */
  PINMODE_PULLUP,                          /*!< GPIO pin mode pullup define */
  PINMODE_PULLDOWN,                        /*!< GPIO pin mode pulldown define */
  PINMODE_TRISTATE,                        /*!< GPIO pin mode tristate define */
}GPIO_PINMODE_Type;
#endif // USE_BASICDRV_IN_ROM

/**
 *  @brief GPIO pin output mode type definition
 */
typedef enum
{
  PIN_OUTPUT_MODE_NORMAL_FUNCTION = 0,     /*!< Set GPIO pad output low */
  PIN_OUTPUT_MODE_PAD,                       /*!< Set GPIO pad Output high */
}GPIO_PinOutputMode_Type;

/**
 *  @brief GPIO pin pad Output type definition
 */
typedef enum
{
  PIN_PAD_OUTPUT_LOW = 0,                   /*!< Set GPIO pad output low */
  PIN_PAD_OUTPUT_HIGH,                       /*!< Set GPIO pad Output high */
}GPIO_PinPadOutput_Type;

/*@} end of group PINMUX_Public_Types */

/** @defgroup PINMUX_Public_Constants
 *  @{
 */

/** @defgroup PINMUX_Pinmux
 *  @{
 */

/* GPIO1 pinmux function define */
#define GPIO1_GPIO1                            PINMUX_FUNCTION_0
#define GPIO1_TRACEDATA0_SWO                   PINMUX_FUNCTION_2
#define GPIO1_GPT1_OUT1                        PINMUX_FUNCTION_4
#define GPIO1_IRQ_1                            PINMUX_FUNCTION_7
#define GPIO1_I2C1_SCL                         PINMUX_FUNCTION_8
#define GPIO1_IR_IN                            PINMUX_FUNCTION_9
#define GPIO1_3D_OUT1                          PINMUX_FUNCTION_12
#define GPIO1_Z_PLUS                           PINMUX_FUNCTION_13
#define GPIO1_KS_MKIN0                         PINMUX_FUNCTION_14
#define GPIO1_KS_MKOUT0                        PINMUX_FUNCTION_15

/* GPIO2 pinmux function define */
#define GPIO2_GPIO2                            PINMUX_FUNCTION_0
#define GPIO2_ADC0                             PINMUX_FUNCTION_1
#define GPIO2_ACOMPI0                          PINMUX_FUNCTION_1
#define GPIO2_TRACEDATA1                       PINMUX_FUNCTION_2
#define GPIO2_GPT1_OUT2                        PINMUX_FUNCTION_4
#define GPIO2_IRQ_2                            PINMUX_FUNCTION_7
#define GPIO2_I2C1_SDA                         PINMUX_FUNCTION_8
#define GPIO2_SSP2_SCK                         PINMUX_FUNCTION_9
#define GPIO2_UART1_TXD                        PINMUX_FUNCTION_10
#define GPIO2_I2C2_SCL                         PINMUX_FUNCTION_11
#define GPIO2_3D_OUT2                          PINMUX_FUNCTION_12
#define GPIO2_Z_MINUS                          PINMUX_FUNCTION_13
#define GPIO2_KS_MKIN1                         PINMUX_FUNCTION_14
#define GPIO2_KS_MKOUT1                        PINMUX_FUNCTION_15

/* GPIO3 pinmux function define */
#define GPIO3_GPIO3                            PINMUX_FUNCTION_0
#define GPIO3_ADC1                             PINMUX_FUNCTION_1
#define GPIO3_ACOMPI1                          PINMUX_FUNCTION_1
#define GPIO3_DAC_B                            PINMUX_FUNCTION_1
#define GPIO3_TRACEDATA2                       PINMUX_FUNCTION_2
#define GPIO3_GPT1_OUT3                        PINMUX_FUNCTION_4
#define GPIO3_IRQ_3                            PINMUX_FUNCTION_7
#define GPIO3_TRST_N                           PINMUX_FUNCTION_8
#define GPIO3_SSP2_TXD                         PINMUX_FUNCTION_9
#define GPIO3_UART1_RXD                        PINMUX_FUNCTION_10
#define GPIO3_I2C2_SDA                         PINMUX_FUNCTION_11
#define GPIO3_3D_OUT3                          PINMUX_FUNCTION_12
#define GPIO3_KS_MKIN2                         PINMUX_FUNCTION_14
#define GPIO3_KS_MKOUT2                        PINMUX_FUNCTION_15

/* GPIO4 pinmux function define */
#define GPIO4_GPIO4                            PINMUX_FUNCTION_0
#define GPIO4_ADC2                             PINMUX_FUNCTION_1
#define GPIO4_ACOMPI2                          PINMUX_FUNCTION_1
#define GPIO4_DAC_A                            PINMUX_FUNCTION_1
#define GPIO4_UART2_TXD                        PINMUX_FUNCTION_4
#define GPIO4_RXEV                             PINMUX_FUNCTION_6
#define GPIO4_IRQ_4                            PINMUX_FUNCTION_7
#define GPIO4_TDO                              PINMUX_FUNCTION_8
#define GPIO4_IR_IN                            PINMUX_FUNCTION_9
#define GPIO4_I2C1_SCL                         PINMUX_FUNCTION_10
#define GPIO4_SSP1_SCK                         PINMUX_FUNCTION_11
#define GPIO4_3D_OUT1                          PINMUX_FUNCTION_12
#define GPIO4_X_PLUS                           PINMUX_FUNCTION_13
#define GPIO4_KS_MKIN3                         PINMUX_FUNCTION_14
#define GPIO4_KS_MKOUT3                        PINMUX_FUNCTION_15

/* GPIO5 pinmux function define */
#define GPIO5_GPIO5                            PINMUX_FUNCTION_0
#define GPIO5_ADC3                             PINMUX_FUNCTION_1
#define GPIO5_ACOMPI3                          PINMUX_FUNCTION_1
#define GPIO5_EXT_VREF                         PINMUX_FUNCTION_1
#define GPIO5_IRQ_5                            PINMUX_FUNCTION_7
#define GPIO5_TCK                              PINMUX_FUNCTION_8
#define GPIO5_SWCLK                            PINMUX_FUNCTION_8
#define GPIO5_SSP2_RXD                         PINMUX_FUNCTION_9
#define GPIO5_I2C1_SDA                         PINMUX_FUNCTION_10
#define GPIO5_SSP1_TXD                         PINMUX_FUNCTION_11
#define GPIO5_3D_OUT2                          PINMUX_FUNCTION_12
#define GPIO5_X_MINUS                          PINMUX_FUNCTION_13
#define GPIO5_KS_MKIN4                         PINMUX_FUNCTION_14
#define GPIO5_KS_MKOUT4                        PINMUX_FUNCTION_15

/* GPIO6 pinmux function define */
#define GPIO6_GPIO6                            PINMUX_FUNCTION_0
#define GPIO6_ADC4                             PINMUX_FUNCTION_1
#define GPIO6_ACOMPI4                          PINMUX_FUNCTION_1
#define GPIO6_ANALOG_IR_OUT                    PINMUX_FUNCTION_2
#define GPIO6_UART2_RXD                        PINMUX_FUNCTION_4
#define GPIO6_IRQ_6                            PINMUX_FUNCTION_7
#define GPIO6_TMS                              PINMUX_FUNCTION_8
#define GPIO6_SWDIO                            PINMUX_FUNCTION_8
#define GPIO6_IR_OUT                           PINMUX_FUNCTION_9
#define GPIO6_UART2_TXD                        PINMUX_FUNCTION_10
#define GPIO6_SSP1_RXD                         PINMUX_FUNCTION_11
#define GPIO6_3D_OUT3                          PINMUX_FUNCTION_12
#define GPIO6_Y_PLUS                           PINMUX_FUNCTION_13
#define GPIO6_KS_MKIN5                         PINMUX_FUNCTION_14
#define GPIO6_KS_MKOUT5                        PINMUX_FUNCTION_15

/* GPIO7 pinmux function define */
#define GPIO7_GPIO7                            PINMUX_FUNCTION_0
#define GPIO7_ADC5                             PINMUX_FUNCTION_1
#define GPIO7_ACOMPI5                          PINMUX_FUNCTION_1
#define GPIO7_TRACEDATA3                       PINMUX_FUNCTION_2
#define GPIO7_GPT1_OUT4                        PINMUX_FUNCTION_4
#define GPIO7_IRQ_7                            PINMUX_FUNCTION_7
#define GPIO7_TDI                              PINMUX_FUNCTION_8
#define GPIO7_SSP2_FRM                         PINMUX_FUNCTION_9
#define GPIO7_UART2_RXD                        PINMUX_FUNCTION_10
#define GPIO7_SSP1_FRM                         PINMUX_FUNCTION_11
#define GPIO7_3D_OUT4                          PINMUX_FUNCTION_12
#define GPIO7_Y_MINUS                          PINMUX_FUNCTION_13
#define GPIO7_KS_MKIN6                         PINMUX_FUNCTION_14
#define GPIO7_KS_MKOUT6                        PINMUX_FUNCTION_15

/* GPIO8 pinmux function define */
#define GPIO8_GPIO8                            PINMUX_FUNCTION_0
#define GPIO8_SSP1_TXD                         PINMUX_FUNCTION_2
#define GPIO8_GPT1_OUT5                        PINMUX_FUNCTION_4
#define GPIO8_IRQ_8                            PINMUX_FUNCTION_7
#define GPIO8_UART1_TXD                        PINMUX_FUNCTION_9
#define GPIO8_SSP2_TXD                         PINMUX_FUNCTION_10
#define GPIO8_I2C1_SCL                         PINMUX_FUNCTION_11
#define GPIO8_X_PLUS                           PINMUX_FUNCTION_13
#define GPIO8_KS_MKIN13                        PINMUX_FUNCTION_14
#define GPIO8_KS_MKOUT13                       PINMUX_FUNCTION_15

/* GPIO9 pinmux function define */
#define GPIO9_GPIO9                            PINMUX_FUNCTION_0
#define GPIO9_UART1_CTSn                       PINMUX_FUNCTION_1
#define GPIO9_EVENT_WAKEUP_OUT                 PINMUX_FUNCTION_2
#define GPIO9_GPT2_OUT1                        PINMUX_FUNCTION_4
#define GPIO9_IRQ_9                            PINMUX_FUNCTION_7
#define GPIO9_SSP1_RXD                         PINMUX_FUNCTION_8
#define GPIO9_UART1_RXD                        PINMUX_FUNCTION_9
#define GPIO9_SSP2_RXD                         PINMUX_FUNCTION_10
#define GPIO9_I2C1_SDA                         PINMUX_FUNCTION_11
#define GPIO9_X_MINUS                          PINMUX_FUNCTION_13
#define GPIO9_KS_MKIN14                        PINMUX_FUNCTION_14
#define GPIO9_KS_MKOUT14                       PINMUX_FUNCTION_15

/* GPIO10 pinmux function define */
#define GPIO10_GPIO10                          PINMUX_FUNCTION_0
#define GPIO10_UART1_RTSn                      PINMUX_FUNCTION_1
#define GPIO10_GPT2_OUT2                       PINMUX_FUNCTION_4
#define GPIO10_IRQ_10                          PINMUX_FUNCTION_7
#define GPIO10_SSP1_FRM                        PINMUX_FUNCTION_8
#define GPIO10_SSP2_FRM                        PINMUX_FUNCTION_10
#define GPIO10_UART1_TXD                       PINMUX_FUNCTION_11
#define GPIO10_Y_PLUS                          PINMUX_FUNCTION_13
#define GPIO10_KS_MKIN15                       PINMUX_FUNCTION_14
#define GPIO10_KS_MKOUT15                      PINMUX_FUNCTION_15

/* GPIO11 pinmux function define */
#define GPIO11_GPIO11                          PINMUX_FUNCTION_0
#define GPIO11_SSP1_RXD                        PINMUX_FUNCTION_2
#define GPIO11_GPT2_OUT3                       PINMUX_FUNCTION_4
#define GPIO11_IRQ_11                          PINMUX_FUNCTION_7
#define GPIO11_SSP1_SCK                        PINMUX_FUNCTION_8
#define GPIO11_I2C2_SCL                        PINMUX_FUNCTION_9
#define GPIO11_SSP2_SCK                        PINMUX_FUNCTION_10
#define GPIO11_UART1_RXD                       PINMUX_FUNCTION_11
#define GPIO11_I2C1_SDA                        PINMUX_FUNCTION_12
#define GPIO11_Y_MINUS                         PINMUX_FUNCTION_13
#define GPIO11_KS_MKIN0                        PINMUX_FUNCTION_14
#define GPIO11_KS_MKOUT0                       PINMUX_FUNCTION_15

/* GPIO12 pinmux function define */
#define GPIO12_GPIO12                          PINMUX_FUNCTION_0
#define GPIO12_I2C1_SDA                        PINMUX_FUNCTION_1
#define GPIO12_GPT1_CLKIN                      PINMUX_FUNCTION_2
#define GPIO12_GPT1_IN5                        PINMUX_FUNCTION_3
#define GPIO12_GPT1_OUT5                       PINMUX_FUNCTION_4
#define GPIO12_RF_DB_CLK                       PINMUX_FUNCTION_5
#define GPIO12_SSP2_TXD                        PINMUX_FUNCTION_6
#define GPIO12_IRQ_12                          PINMUX_FUNCTION_7
#define GPIO12_TRST_N                          PINMUX_FUNCTION_8
#define GPIO12_I2C2_SDA                        PINMUX_FUNCTION_9
#define GPIO12_I2C1_SCL                        PINMUX_FUNCTION_12
#define GPIO12_Z_PLUS                          PINMUX_FUNCTION_13
#define GPIO12_KS_MKIN1                        PINMUX_FUNCTION_14
#define GPIO12_KS_MKOUT1                       PINMUX_FUNCTION_15

/* GPIO13 pinmux function define */
#define GPIO13_GPIO13                          PINMUX_FUNCTION_0
#define GPIO13_I2C1_SCL                        PINMUX_FUNCTION_1
#define GPIO13_GPT1_IN1                        PINMUX_FUNCTION_2
#define GPIO13_GPT1_OUT1                       PINMUX_FUNCTION_3
#define GPIO13_GPT2_IN1                        PINMUX_FUNCTION_4
#define GPIO13_GPT2_OUT1                       PINMUX_FUNCTION_5
#define GPIO13_SSP2_RXD                        PINMUX_FUNCTION_6
#define GPIO13_IRQ_13                          PINMUX_FUNCTION_7
#define GPIO13_TDO                             PINMUX_FUNCTION_8
#define GPIO13_UART2_TXD                       PINMUX_FUNCTION_9
#define GPIO13_SSP2_SCK                        PINMUX_FUNCTION_11
#define GPIO13_SSP1_SCK                        PINMUX_FUNCTION_12
#define GPIO13_Z_MINUS                         PINMUX_FUNCTION_13
#define GPIO13_KS_MKIN2                        PINMUX_FUNCTION_14
#define GPIO13_KS_MKOUT2                       PINMUX_FUNCTION_15

/* GPIO14 pinmux function define */
#define GPIO14_GPIO14                          PINMUX_FUNCTION_0
#define GPIO14_TCK                             PINMUX_FUNCTION_1
#define GPIO14_SWCLK                           PINMUX_FUNCTION_1
#define GPIO14_IRQ_14                          PINMUX_FUNCTION_7
#define GPIO14_UART2_RXD                       PINMUX_FUNCTION_9
#define GPIO14_I2C1_SDA                        PINMUX_FUNCTION_10
#define GPIO14_SSP2_TXD                        PINMUX_FUNCTION_11
#define GPIO14_SSP1_TXD                        PINMUX_FUNCTION_12
#define GPIO14_X_PLUS                          PINMUX_FUNCTION_13
#define GPIO14_KS_MKIN3                        PINMUX_FUNCTION_14
#define GPIO14_KS_MKOUT3                       PINMUX_FUNCTION_15

/* GPIO15 pinmux function define */
#define GPIO15_GPIO15                          PINMUX_FUNCTION_0
#define GPIO15_TMS                             PINMUX_FUNCTION_1
#define GPIO15_SWDIO                           PINMUX_FUNCTION_1
#define GPIO15_IRQ_15                          PINMUX_FUNCTION_7
#define GPIO15_I2C1_SCL                        PINMUX_FUNCTION_9
#define GPIO15_UART1_TXD                       PINMUX_FUNCTION_10
#define GPIO15_SSP2_RXD                        PINMUX_FUNCTION_11
#define GPIO15_SSP1_RXD                        PINMUX_FUNCTION_12
#define GPIO15_X_MINUS                         PINMUX_FUNCTION_13
#define GPIO15_KS_MKIN4                        PINMUX_FUNCTION_14
#define GPIO15_KS_MKOUT4                       PINMUX_FUNCTION_15

/* GPIO16 pinmux function define */
#define GPIO16_GPIO16                          PINMUX_FUNCTION_0
#define GPIO16_TDI                             PINMUX_FUNCTION_1
#define GPIO16_IRQ_16                          PINMUX_FUNCTION_7
#define GPIO16_I2C1_SDA                        PINMUX_FUNCTION_9
#define GPIO16_UART1_RXD                       PINMUX_FUNCTION_10
#define GPIO16_SSP2_FRM                        PINMUX_FUNCTION_11
#define GPIO16_SSP1_FRM                        PINMUX_FUNCTION_12
#define GPIO16_Y_PLUS                          PINMUX_FUNCTION_13
#define GPIO16_KS_MKIN5                        PINMUX_FUNCTION_14
#define GPIO16_KS_MKOUT5                       PINMUX_FUNCTION_15

/* GPIO17 pinmux function define */
#define GPIO17_GPIO17                          PINMUX_FUNCTION_0
#define GPIO17_TDO                             PINMUX_FUNCTION_1
#define GPIO17_I2C2_SCL                        PINMUX_FUNCTION_6
#define GPIO17_IRQ_17                          PINMUX_FUNCTION_7
#define GPIO17_TRST_N                          PINMUX_FUNCTION_8
#define GPIO17_I2C1_SCL                        PINMUX_FUNCTION_9
#define GPIO17_UART1_TXD                       PINMUX_FUNCTION_10
#define GPIO17_Y_MINUS                         PINMUX_FUNCTION_13
#define GPIO17_KS_MKIN6                        PINMUX_FUNCTION_14
#define GPIO17_KS_MKOUT6                       PINMUX_FUNCTION_15

/* GPIO18 pinmux function define */
#define GPIO18_GPIO18                          PINMUX_FUNCTION_0
#define GPIO18_TRST_n                          PINMUX_FUNCTION_1
#define GPIO18_GPT1_IN2                        PINMUX_FUNCTION_2
#define GPIO18_GPT1_OUT2                       PINMUX_FUNCTION_3
#define GPIO18_GPT2_IN2                        PINMUX_FUNCTION_4
#define GPIO18_GPT2_OUT2                       PINMUX_FUNCTION_5
#define GPIO18_I2C2_SDA                        PINMUX_FUNCTION_6
#define GPIO18_IRQ_18                          PINMUX_FUNCTION_7
#define GPIO18_TDO                             PINMUX_FUNCTION_8
#define GPIO18_I2C1_SDA                        PINMUX_FUNCTION_9
#define GPIO18_UART1_RXD                       PINMUX_FUNCTION_10
#define GPIO18_SSP1_SCK                        PINMUX_FUNCTION_11
#define GPIO18_SSP2_SCK                        PINMUX_FUNCTION_12
#define GPIO18_Z_PLUS                          PINMUX_FUNCTION_13
#define GPIO18_KS_MKIN7                        PINMUX_FUNCTION_14
#define GPIO18_KS_MKOUT7                       PINMUX_FUNCTION_15

/* GPIO19 pinmux function define */
#define GPIO19_GPIO19                          PINMUX_FUNCTION_0
#define GPIO19_UART1_TXD                       PINMUX_FUNCTION_1
#define GPIO19_IRQ_19                          PINMUX_FUNCTION_7
#define GPIO19_UART2_TXD                       PINMUX_FUNCTION_9
#define GPIO19_I2C1_SCL                        PINMUX_FUNCTION_10
#define GPIO19_I2C2_SCL                        PINMUX_FUNCTION_11
#define GPIO19_Z_MINUS                         PINMUX_FUNCTION_13
#define GPIO19_KS_MKIN8                        PINMUX_FUNCTION_14
#define GPIO19_KS_MKOUT8                       PINMUX_FUNCTION_15

/* GPIO20 pinmux function define */
#define GPIO20_GPIO20                          PINMUX_FUNCTION_0
#define GPIO20_UART1_RXD                       PINMUX_FUNCTION_1
#define GPIO20_IRQ_20                          PINMUX_FUNCTION_7
#define GPIO20_UART2_RXD                       PINMUX_FUNCTION_9
#define GPIO20_I2C1_SDA                        PINMUX_FUNCTION_10
#define GPIO20_I2C2_SDA                        PINMUX_FUNCTION_11
#define GPIO20_X_PLUS                          PINMUX_FUNCTION_13
#define GPIO20_KS_MKIN9                        PINMUX_FUNCTION_14
#define GPIO20_KS_MKOUT9                       PINMUX_FUNCTION_15

/* GPIO21 pinmux function define */
#define GPIO21_GPIO21                          PINMUX_FUNCTION_0
#define GPIO21_UART2_CTSn                      PINMUX_FUNCTION_1
#define GPIO21_GPT2_CLKIN                      PINMUX_FUNCTION_2
#define GPIO21_GPT2_IN5                        PINMUX_FUNCTION_3
#define GPIO21_GPT2_OUT5                       PINMUX_FUNCTION_4
#define GPIO21_CLKOUT                          PINMUX_FUNCTION_5
#define GPIO21_RF_DB_DATA                      PINMUX_FUNCTION_6
#define GPIO21_IRQ_21                          PINMUX_FUNCTION_7
#define GPIO21_TCK                             PINMUX_FUNCTION_8
#define GPIO21_SWCLK                           PINMUX_FUNCTION_8
#define GPIO21_I2C2_SCL                        PINMUX_FUNCTION_9
#define GPIO21_UART2_TXD                       PINMUX_FUNCTION_10
#define GPIO21_SSP1_TXD                        PINMUX_FUNCTION_11
#define GPIO21_SSP2_TXD                        PINMUX_FUNCTION_12
#define GPIO21_X_MINUS                         PINMUX_FUNCTION_13
#define GPIO21_KS_MKIN10                       PINMUX_FUNCTION_14
#define GPIO21_KS_MKOUT10                      PINMUX_FUNCTION_15

/* GPIO22 pinmux function define */
#define GPIO22_GPIO22                          PINMUX_FUNCTION_0
#define GPIO22_UART2_RTSn                      PINMUX_FUNCTION_1
#define GPIO22_GPT1_IN3                        PINMUX_FUNCTION_2
#define GPIO22_GPT1_OUT3                       PINMUX_FUNCTION_3
#define GPIO22_GPT2_IN3                        PINMUX_FUNCTION_4
#define GPIO22_GPT2_OUT3                       PINMUX_FUNCTION_5
#define GPIO22_SSP2_SCK                        PINMUX_FUNCTION_6
#define GPIO22_IRQ_22                          PINMUX_FUNCTION_7
#define GPIO22_TMS                             PINMUX_FUNCTION_8
#define GPIO22_SWDIO                           PINMUX_FUNCTION_8
#define GPIO22_I2C2_SDA                        PINMUX_FUNCTION_9
#define GPIO22_UART2_RXD                       PINMUX_FUNCTION_10
#define GPIO22_SSP1_RXD                        PINMUX_FUNCTION_11
#define GPIO22_SSP2_RXD                        PINMUX_FUNCTION_12
#define GPIO22_Y_PLUS                          PINMUX_FUNCTION_13
#define GPIO22_KS_MKIN11                       PINMUX_FUNCTION_14
#define GPIO22_KS_MKOUT11                      PINMUX_FUNCTION_15

/* GPIO23 pinmux function define */
#define GPIO23_GPIO23                          PINMUX_FUNCTION_0
#define GPIO23_GPT1_IN4                        PINMUX_FUNCTION_2
#define GPIO23_GPT1_OUT4                       PINMUX_FUNCTION_3
#define GPIO23_GPT2_IN4                        PINMUX_FUNCTION_4
#define GPIO23_GPT2_OUT4                       PINMUX_FUNCTION_5
#define GPIO23_IRQ_23                          PINMUX_FUNCTION_7
#define GPIO23_TDI                             PINMUX_FUNCTION_8
#define GPIO23_IR_OUT                          PINMUX_FUNCTION_9
#define GPIO23_SSP1_FRM                        PINMUX_FUNCTION_11
#define GPIO23_SSP2_FRM                        PINMUX_FUNCTION_12
#define GPIO23_Y_MINUS                         PINMUX_FUNCTION_13
#define GPIO23_KS_MKIN12                       PINMUX_FUNCTION_14
#define GPIO23_KS_MKOUT12                      PINMUX_FUNCTION_15

/* GPIO24 pinmux function define */
#define GPIO24_GPIO24                          PINMUX_FUNCTION_0
#define GPIO24_SSP1_SCK                        PINMUX_FUNCTION_2
#define GPIO24_I2C2_SCL                        PINMUX_FUNCTION_3
#define GPIO24_GPT2_OUT4                       PINMUX_FUNCTION_4
#define GPIO24_IRQ_24                          PINMUX_FUNCTION_7
#define GPIO24_I2C1_SCL                        PINMUX_FUNCTION_8
#define GPIO24_UART2_TXD                       PINMUX_FUNCTION_9
#define GPIO24_UART1_TXD                       PINMUX_FUNCTION_11
#define GPIO24_Z_PLUS                          PINMUX_FUNCTION_13
#define GPIO24_KS_MKIN13                       PINMUX_FUNCTION_14
#define GPIO24_KS_MKOUT13                      PINMUX_FUNCTION_15

/* GPIO25 pinmux function define */
#define GPIO25_GPIO25                          PINMUX_FUNCTION_0
#define GPIO25_I2C2_SDA                        PINMUX_FUNCTION_3
#define GPIO25_GPT2_OUT5                       PINMUX_FUNCTION_4
#define GPIO25_IRQ_25                          PINMUX_FUNCTION_7
#define GPIO25_I2C1_SDA                        PINMUX_FUNCTION_8
#define GPIO25_UART2_RXD                       PINMUX_FUNCTION_9
#define GPIO25_UART1_RXD                       PINMUX_FUNCTION_11
#define GPIO25_Z_MINUS                         PINMUX_FUNCTION_13
#define GPIO25_KS_MKIN14                       PINMUX_FUNCTION_14
#define GPIO25_KS_MKOUT14                      PINMUX_FUNCTION_15

/* GPIO26 pinmux function define */
#define GPIO26_GPIO26                          PINMUX_FUNCTION_0
#define GPIO26_TRACECLKOUT                     PINMUX_FUNCTION_1
#define GPIO26_VCOMPA                          PINMUX_FUNCTION_2
#define GPIO26_SSP2_TXD                        PINMUX_FUNCTION_3
#define GPIO26_IRQ_26                          PINMUX_FUNCTION_7
#define GPIO26_I2C1_SCL                        PINMUX_FUNCTION_8
#define GPIO26_UART1_TXD                       PINMUX_FUNCTION_9
#define GPIO26_SSP1_SCK                        PINMUX_FUNCTION_10
#define GPIO26_SSP2_SCK                        PINMUX_FUNCTION_11
#define GPIO26_3D_OUT1                         PINMUX_FUNCTION_12
#define GPIO26_X_PLUS                          PINMUX_FUNCTION_13
#define GPIO26_KS_MKIN7                        PINMUX_FUNCTION_14
#define GPIO26_KS_MKOUT7                       PINMUX_FUNCTION_15

/* GPIO27 pinmux function define */
#define GPIO27_GPIO27                          PINMUX_FUNCTION_0
#define GPIO27_TRACECLK                        PINMUX_FUNCTION_1
#define GPIO27_VCOMPB                          PINMUX_FUNCTION_2
#define GPIO27_SSP2_RXD                        PINMUX_FUNCTION_3
#define GPIO27_IRQ_27                          PINMUX_FUNCTION_7
#define GPIO27_I2C1_SDA                        PINMUX_FUNCTION_8
#define GPIO27_UART1_RXD                       PINMUX_FUNCTION_9
#define GPIO27_SSP1_TXD                        PINMUX_FUNCTION_10
#define GPIO27_SSP2_TXD                        PINMUX_FUNCTION_11
#define GPIO27_3D_OUT2                         PINMUX_FUNCTION_12
#define GPIO27_X_MINUS                         PINMUX_FUNCTION_13
#define GPIO27_KS_MKIN8                        PINMUX_FUNCTION_14
#define GPIO27_KS_MKOUT8                       PINMUX_FUNCTION_15

/* GPIO28 pinmux function define */
#define GPIO28_GPIO28                          PINMUX_FUNCTION_0
#define GPIO28_ACOMP0_OUT                      PINMUX_FUNCTION_1
#define GPIO28_ACOMP0_EDGE                     PINMUX_FUNCTION_2
#define GPIO28_OSC32KIN                        PINMUX_FUNCTION_5
#define GPIO28_CLK_32K                         PINMUX_FUNCTION_6
#define GPIO28_IRQ_28                          PINMUX_FUNCTION_7
#define GPIO28_I2C2_SCL                        PINMUX_FUNCTION_8
#define GPIO28_UART2_TXD                       PINMUX_FUNCTION_9
#define GPIO28_I2C1_SCL                        PINMUX_FUNCTION_10
#define GPIO28_UART1_TXD                       PINMUX_FUNCTION_11
#define GPIO28_3D_OUT3                         PINMUX_FUNCTION_12
#define GPIO28_Z_PLUS                          PINMUX_FUNCTION_13
#define GPIO28_KS_MKIN9                        PINMUX_FUNCTION_14
#define GPIO28_KS_MKOUT9                       PINMUX_FUNCTION_15

/* GPIO29 pinmux function define */
#define GPIO29_GPIO29                          PINMUX_FUNCTION_0
#define GPIO29_ACOMP1_OUT                      PINMUX_FUNCTION_1
#define GPIO29_ACOMP1_EDGE                     PINMUX_FUNCTION_2
#define GPIO29_IR_OUT                          PINMUX_FUNCTION_3
#define GPIO29_OSC32KOUT                       PINMUX_FUNCTION_5
#define GPIO29_X32K_OUT                        PINMUX_FUNCTION_7
#define GPIO29_IRQ_29                          PINMUX_FUNCTION_7
#define GPIO29_I2C2_SDA                        PINMUX_FUNCTION_8
#define GPIO29_UART2_RXD                       PINMUX_FUNCTION_9
#define GPIO29_I2C1_SDA                        PINMUX_FUNCTION_10
#define GPIO29_UART1_RXD                       PINMUX_FUNCTION_11
#define GPIO29_3D_OUT4                         PINMUX_FUNCTION_12
#define GPIO29_Z_MINUS                         PINMUX_FUNCTION_13
#define GPIO29_KS_MKIN10                       PINMUX_FUNCTION_14
#define GPIO29_KS_MKOUT10                      PINMUX_FUNCTION_15

/* GPIO30 pinmux function define */
#define GPIO30_GPIO30                          PINMUX_FUNCTION_0
#define GPIO30_ADC6                            PINMUX_FUNCTION_1
#define GPIO30_ACOMPI6                         PINMUX_FUNCTION_1
#define GPIO30_SSP2_SCK                        PINMUX_FUNCTION_2
#define GPIO30_X32K_OUT                        PINMUX_FUNCTION_6
#define GPIO30_IRQ_30                          PINMUX_FUNCTION_7
#define GPIO30_UART2_TXD                       PINMUX_FUNCTION_8
#define GPIO30_I2C1_SCL                        PINMUX_FUNCTION_9
#define GPIO30_SSP1_RXD                        PINMUX_FUNCTION_10
#define GPIO30_SSP2_RXD                        PINMUX_FUNCTION_11
#define GPIO30_I2C2_SCL                        PINMUX_FUNCTION_12
#define GPIO30_Y_PLUS                          PINMUX_FUNCTION_13
#define GPIO30_KS_MKIN11                       PINMUX_FUNCTION_14
#define GPIO30_KS_MKOUT11                      PINMUX_FUNCTION_15

/* GPIO31 pinmux function define */
#define GPIO31_GPIO31                          PINMUX_FUNCTION_0
#define GPIO31_ADC7                            PINMUX_FUNCTION_1
#define GPIO31_ACOMPI7                         PINMUX_FUNCTION_1
#define GPIO31_IRQ_31                          PINMUX_FUNCTION_7
#define GPIO31_UART2_RXD                       PINMUX_FUNCTION_8
#define GPIO31_I2C1_SDA                        PINMUX_FUNCTION_9
#define GPIO31_SSP1_FRM                        PINMUX_FUNCTION_10
#define GPIO31_SSP2_FRM                        PINMUX_FUNCTION_11
#define GPIO31_I2C2_SDA                        PINMUX_FUNCTION_12
#define GPIO31_Y_MINUS                         PINMUX_FUNCTION_13
#define GPIO31_KS_MKIN12                       PINMUX_FUNCTION_14
#define GPIO31_KS_MKOUT12                      PINMUX_FUNCTION_15

/*@} end of group GPIO_Pinmux */

/** @defgroup GPIO_PIN_MUX
 *  @{
 */
#define IS_GPIO_PINMUXFUN(PINMUXFUN)           ((PINMUXFUN) <= PINMUX_FUNCTION_15)

/*@} end of group GPIO_PIN_MUX */

/** @defgroup GPIO_PIN_MODE
 *  @{
 */
#ifndef USE_BASICDRV_IN_ROM
#define IS_GPIO_PINMODE(PINMODE)               (((PINMODE) == PINMODE_DEFAULT) || \
                                                ((PINMODE) == PINMODE_PULLUP) || \
                                                ((PINMODE) == PINMODE_PULLDOWN) || \
                                                ((PINMODE) == PINMODE_NOPULL) || \
                                                ((PINMODE) == PINMODE_TRISTATE))
#else // USE_BASICDRV_IN_ROM
#define IS_GPIO_PINMODE(PINMODE)               (((PINMODE) == PINMODE_NORMAL) || \
                                                ((PINMODE) == PINMODE_PULLUP) || \
                                                ((PINMODE) == PINMODE_PULLDOWN) || \
                                                ((PINMODE) == PINMODE_TRISTATE))
#endif // USE_BASICDRV_IN_ROM

/*@} end of group GPIO_PIN_MODE */

/** @defgroup GPIO_PIN_OUTPUT_MODE
 *  @{
 */
#define IS_GPIO_PIN_OUTPUT_MODE(PINOUTMODE)    (((PINOUTMODE) == PIN_OUTPUT_MODE_NORMAL_FUNCTION) || \
                                                ((PINOUTMODE) == PIN_OUTPUT_MODE_PAD))

/*@} end of group GPIO_PIN_OUTPUT_MODE */

/** @defgroup GPIO_PIN_PAD_OUTPUT
 *  @{
 */
#define IS_GPIO_PIN_PAD_OUTPUT(PINPADOUT)      (((PINPADOUT) == PIN_PAD_OUTPUT_LOW) || \
                                                ((PINPADOUT) == PIN_PAD_OUTPUT_HIGH))

/*@} end of group GPIO_PIN_PAD_OUTPUT */

/*@} end of group PINMUX_Public_Constants */

/** @defgroup PINMUX_Public_Macro
 *  @{
 */

/*@} end of group PINMUX_Public_Macro */

/** @defgroup PINMUX_Public_FunctionDeclaration
 *  @brief PINMUX functions declaration
 *  @{
 */
void GPIO_PinMuxFun(GPIO_NO_Type gpioNo, GPIO_PinMuxFunc_Type pinMuxFun);
void GPIO_PinModeConfig(GPIO_NO_Type gpioNo, GPIO_PINMODE_Type gpioPinMode);

void GPIO_PinOutputModeConfig(GPIO_NO_Type gpioNo, GPIO_PinOutputMode_Type pinOutputMode);
void GPIO_PinPadOutputEnable(GPIO_NO_Type gpioNo);
void GPIO_PinPadOutputDisable(GPIO_NO_Type gpioNo);
void GPIO_PinPadOutputLevel(GPIO_NO_Type gpioNo, GPIO_PinPadOutput_Type pinPadOutput);
#ifdef USE_BASICDRV_IN_ROM
void GPIO_PinPadOutput(GPIO_NO_Type gpioNo, GPIO_PinPadOutput_Type pinPadOutput);
#endif // USE_BASICDRV_IN_ROM
