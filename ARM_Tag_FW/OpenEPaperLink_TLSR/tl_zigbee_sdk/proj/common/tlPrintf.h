#pragma once

int  u_printf(const char *fmt, ...);
int  u_sprintf(char* s, const char *fmt, ...);
void u_array_printf(unsigned char*data, unsigned int len);

#define printf	 		u_printf
#define sprintf	 		u_sprintf
#define array_printf	u_array_printf