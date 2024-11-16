#pragma once

#include <inttypes.h>

void init_second_uart();
void uart_switch_speed(int baudrate);

void uartTx(uint8_t data);
bool getRxCharSecond(uint8_t *newChar);

void uart_printf(const char *format, ...);

#define pr uart_printf

#if defined(CONFIG_OEPL_HARDWARE_PROFILE_DEFAULT)
 #define CONFIG_OEPL_HARDWARE_UART_TX 3
 #define CONFIG_OEPL_HARDWARE_UART_RX 2
#elif defined(CONFIG_OEPL_HARDWARE_PROFILE_LILYGO)
 #define CONFIG_OEPL_HARDWARE_UART_TX 24
 #define CONFIG_OEPL_HARDWARE_UART_RX 23
#elif defined(CONFIG_OEPL_HARDWARE_PROFILE_POE_AP)
  #define CONFIG_OEPL_HARDWARE_UART_TX 5
  #define CONFIG_OEPL_HARDWARE_UART_RX 18
#elif defined(CONFIG_OEPL_HARDWARE_PROFILE_CUSTOM)
  #if !defined(CONFIG_OEPL_HARDWARE_UART_TX) || !defined(CONFIG_OEPL_HARDWARE_UART_RX)
    #error "No UART TX / RX pins defined. Please check menuconfig" 
  #endif
#endif
