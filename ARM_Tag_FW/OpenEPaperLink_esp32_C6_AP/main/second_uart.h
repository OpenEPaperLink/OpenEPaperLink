#pragma once

#include <inttypes.h>

#if defined(CONFIG_HARDWARE_PROFILE_DEFAULT)
#define CONFIG_HARDWARE_UART_TX_PIN 3
#define CONFIG_HARDWARE_UART_RX_PIN 2
#elif defined(CONFIG_HARDWARE_PROFILE_POE_AP)
#define CONFIG_HARDWARE_UART_TX_PIN 5
#define CONFIG_HARDWARE_UART_RX_PIN 18
#elif defined(CONFIG_HARDWARE_PROFILE_CUSTOM)
#if !defined(CONFIG_HARDWARE_UART_TX_PIN) || !defined(CONFIG_HARDWARE_UART_RX_PIN)
#error "No UART TX/RX pins defined. Please check the menuconfig"
#endif
#endif

void init_second_uart();
void uart_switch_speed(int baudrate);

void uartTx(uint8_t data);
bool getRxCharSecond(uint8_t *newChar);

void uart_printf(const char *format, ...);

#define pr uart_printf
