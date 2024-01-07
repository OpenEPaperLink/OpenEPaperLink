#include <stdint.h>
#include "tl_common.h"
#include "uart.h"
#include "main.h"

#define RXD 		GPIO_PA0
#define TXD			GPIO_PB1

void init_uart(void)
{
	gpio_set_func(TXD, AS_GPIO);
	gpio_set_output_en(TXD, 1);
	gpio_write(TXD, 0);
	gpio_set_func(RXD, AS_GPIO);
	gpio_set_input_en(RXD, 1);
	gpio_set_output_en(RXD, 0);

	uart_gpio_set(UART_TX_PB1, UART_RX_PA0);
	uart_reset();
	uart_init(12, 15, PARITY_NONE, STOP_BIT_ONE); // baud rate: 115200
	uart_dma_enable(0, 0);
	dma_chn_irq_enable(0, 0);
	uart_irq_enable(0, 0);
	uart_ndma_irq_triglevel(0, 0);
}

 void puts(const char *str)
{
	while (*str != '\0')
	{
		putchar_custom((char)*str);
		str++;
	}
}

int putchar_custom(int c)
{
	uart_ndma_send_byte((char)c);
	while (uart_tx_is_busy())
	{
		sleep_us(10);
	};
	return 0;
}