#pragma once

#include <inttypes.h>

void init_second_uart();
void uart_switch_speed(int baudrate);

void uartTx(uint8_t data);
bool getRxCharSecond(uint8_t *newChar);

void uart_printf(const char *format, ...);

#define pr uart_printf



  #define CONFIG_OEPL_HARDWARE_UART_TX 24
  #define CONFIG_OEPL_HARDWARE_UART_RX 23
