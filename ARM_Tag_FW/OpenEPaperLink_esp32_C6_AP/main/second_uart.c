#include <esp_mac.h>
#include <math.h>
#include <stdarg.h>
#include <string.h>
#include "driver/gpio.h"
#include "driver/uart.h"
#include "esp_err.h"
#include "esp_event.h"
#include "esp_ieee802154.h"
#include "esp_log.h"
#include "esp_phy_init.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "main.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "proto.h"
#include "sdkconfig.h"
#include "soc/uart_struct.h"
#ifdef CONFIG_IDF_TARGET_ESP32C6
#include "soc/lp_uart_reg.h"
#endif
static const char *TAG = "SECOND_UART";
#include "second_uart.h"
#include "radio.h"


// One receive buffer: the driver's. The main loop reads it directly (uartRead), woken by the event
// task. 4 KB holds 20 ms at 2 Mbaud, more than the S3 sends unanswered (4 frames of data).
#define RX_BUF_SIZE 4096
#define TX_BUF_SIZE 2048
static QueueHandle_t uart0_queue;
volatile uint32_t uartOverflows = 0;

static void uart_event_task(void *pvParameters);
void init_second_uart() {
    uart_config_t uart_config = {
        .baud_rate  = 115200,
        .data_bits  = UART_DATA_8_BITS,
        .parity     = UART_PARITY_DISABLE,
        .stop_bits  = UART_STOP_BITS_1,
        .flow_ctrl  = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };
    ESP_LOGI(TAG, "HARDWARE_UART_TX %d, CONFIG_OEPL_HARDWARE_UART_RX %d", 
             CONFIG_OEPL_HARDWARE_UART_TX,CONFIG_OEPL_HARDWARE_UART_RX);
    ESP_ERROR_CHECK(uart_driver_install(1, RX_BUF_SIZE, TX_BUF_SIZE, 20, &uart0_queue, 0));
    ESP_ERROR_CHECK(uart_param_config(1, &uart_config));
	ESP_ERROR_CHECK(uart_set_pin(1, CONFIG_OEPL_HARDWARE_UART_TX, CONFIG_OEPL_HARDWARE_UART_RX, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));

	xTaskCreate(uart_event_task, "uart_event_task", 3072, NULL, 12, NULL);
}

void uart_switch_speed(int baudrate) {
	uart_config_t uart_config = {
		.baud_rate = baudrate,
		.data_bits = UART_DATA_8_BITS,
		.parity = UART_PARITY_DISABLE,
		.stop_bits = UART_STOP_BITS_1,
		.flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
		.source_clk = UART_SCLK_DEFAULT,
	};
	ESP_ERROR_CHECK(uart_param_config(1, &uart_config));
}

void uartTx(uint8_t data) { uart_write_bytes(1, (const char *) &data, 1); }

void uartWrite(const uint8_t *data, size_t len) { uart_write_bytes(1, (const char *) data, len); }

int uartRead(uint8_t *buf, int max) {
    size_t avail = 0;
    if (uart_get_buffered_data_len(1, &avail) != ESP_OK || avail == 0) return 0;
    if (avail > (size_t) max) avail = max;
    return uart_read_bytes(1, buf, avail, 0);
}

void uartFlushInput() { uart_flush_input(1); }

// Only wakes the main loop; reading is done there
static void uart_event_task(void *pvParameters) {
    uart_event_t event;
    for (;;) {
        if (xQueueReceive(uart0_queue, (void *) &event, (TickType_t) portMAX_DELAY)) {
            switch (event.type) {
                case UART_DATA:
                    if (radioWakeTask) xTaskNotifyGive(radioWakeTask);  // handle it now, not after a radio timeout
                    break;
                case UART_FIFO_OVF:
                case UART_BUFFER_FULL:
                    // data was lost; the framing resynchronises at the next delimiter
                    uartOverflows++;
                    uart_flush_input(1);
                    xQueueReset(uart0_queue);
                    break;
                default:
                    break;
            }
        }
    }
}

void uart_printf(const char *format, ...) {
	va_list args;
	va_start(args, format);

	char buffer[128];
	int len = vsnprintf(buffer, sizeof(buffer), format, args);

	va_end(args);

	if (len > 0) {
		uart_write_bytes(1, buffer, len);
	}
}