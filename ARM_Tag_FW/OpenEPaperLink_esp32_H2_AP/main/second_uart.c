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
//#include "soc/lp_uart_reg.h"
#include "second_uart.h"

//static const char *TAG = "SECOND_UART";

#define BUF_SIZE    (1024)
#define RD_BUF_SIZE (BUF_SIZE)
static QueueHandle_t uart0_queue;

#define MAX_BUFF_POS 8000
volatile int     curr_buff_pos   = 0;
volatile int     worked_buff_pos = 0;
volatile uint8_t buff_pos[MAX_BUFF_POS + 5];

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
    ESP_ERROR_CHECK(uart_driver_install(1, BUF_SIZE * 2, BUF_SIZE * 2, 20, &uart0_queue, 0));
    ESP_ERROR_CHECK(uart_param_config(1, &uart_config));
	ESP_ERROR_CHECK(uart_set_pin(1, CONFIG_OEPL_HARDWARE_UART_TX, CONFIG_OEPL_HARDWARE_UART_RX, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));

	xTaskCreate(uart_event_task, "uart_event_task", 16384, NULL, 12, NULL);
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


bool getRxCharSecond(uint8_t *newChar) {
    if (curr_buff_pos != worked_buff_pos) {
        *newChar = buff_pos[worked_buff_pos];
        worked_buff_pos++;
        worked_buff_pos %= MAX_BUFF_POS;
        return true;
    }
    return false;
}

static void uart_event_task(void *pvParameters) {
    uart_event_t event;
    uint8_t     *dtmp = (uint8_t *) malloc(RD_BUF_SIZE);
    for (;;) {
        if (xQueueReceive(uart0_queue, (void *) &event, (TickType_t) portMAX_DELAY)) {
            bzero(dtmp, RD_BUF_SIZE);
            switch (event.type) {
                case UART_DATA:
                    uart_read_bytes(1, dtmp, event.size, portMAX_DELAY);
                    for (int i = 0; i < event.size; i++) {
                        buff_pos[curr_buff_pos] = dtmp[i];
                        curr_buff_pos++;
                        curr_buff_pos %= MAX_BUFF_POS;
                    }
                    break;
                default:
                    // ESP_LOGI(TAG, "uart event type: %d", event.type);
                    break;
            }
        }
    }
    free(dtmp);
    dtmp = NULL;
    vTaskDelete(NULL);
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