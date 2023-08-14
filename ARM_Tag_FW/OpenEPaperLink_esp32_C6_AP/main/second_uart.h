#pragma once

void init_second_uart();

void uartTx(uint8_t data);
bool getRxCharSecond(uint8_t *newChar);

int  u_printf(const char *fmt, ...);
int  u_sprintf(char *s, const char *fmt, ...);
void u_array_printf(unsigned char *data, unsigned int len);

#define pr        u_printf
#define sprf      u_sprintf
#define array_prf u_array_printf
