#pragma once

#include <inttypes.h>

void init_second_uart();
void uart_switch_speed(int baudrate);

void uartTx(uint8_t data);
bool getRxCharSecond(uint8_t *newChar);

void uart_printf(const char *format, ...);

#define pr uart_printf
