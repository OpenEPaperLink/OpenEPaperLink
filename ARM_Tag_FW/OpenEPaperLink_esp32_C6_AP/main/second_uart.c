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
#include "soc/lp_uart_reg.h"

static const char *TAG = "SECOND_UART";

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
    ESP_ERROR_CHECK(uart_set_pin(1, 3, 2, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));

    xTaskCreate(uart_event_task, "uart_event_task", 16384, NULL, 12, NULL);
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
                    ESP_LOGI(TAG, "uart event type: %d", event.type);
                    break;
            }
        }
    }
    free(dtmp);
    dtmp = NULL;
    vTaskDelete(NULL);
}

static void printchar(char **str, int c) {
    if (str) {
        **str = c;
        ++(*str);
    } else {
        uart_write_bytes(1, (const char *) &c, 1);
    }
}

#define PAD_RIGHT 1
#define PAD_ZERO  2

static int prints(char **out, const char *string, int width, int pad) {
    register int pc = 0, padchar = ' ';

    if (width > 0) {
        register int         len = 0;
        register const char *ptr;
        for (ptr = string; *ptr; ++ptr) ++len;
        if (len >= width)
            width = 0;
        else
            width -= len;
        if (pad & PAD_ZERO) padchar = '0';
    }
    if (!(pad & PAD_RIGHT)) {
        for (; width > 0; --width) {
            printchar(out, padchar);
            ++pc;
        }
    }
    for (; *string; ++string) {
        printchar(out, *string);
        ++pc;
    }
    for (; width > 0; --width) {
        printchar(out, padchar);
        ++pc;
    }

    return pc;
}

/* the following should be enough for 32 bit int */
#define PRINT_BUF_LEN 12

static int printi(char **out, int i, int b, int sg, int width, int pad, int letbase) {
    char                  print_buf[PRINT_BUF_LEN];
    register char        *s;
    register int          t, neg = 0, pc = 0;
    register unsigned int u = i;

    if (i == 0) {
        print_buf[0] = '0';
        print_buf[1] = '\0';
        return prints(out, print_buf, width, pad);
    }

    if (sg && b == 10 && i < 0) {
        neg = 1;
        u   = -i;
    }

    s  = print_buf + PRINT_BUF_LEN - 1;
    *s = '\0';

    while (u) {
        t = u % b;
        if (t >= 10) t += letbase - '0' - 10;
        *--s = t + '0';
        u /= b;
    }

    if (neg) {
        if (width && (pad & PAD_ZERO)) {
            printchar(out, '-');
            ++pc;
            --width;
        } else {
            *--s = '-';
        }
    }

    return pc + prints(out, s, width, pad);
}

static int print(char **out, const char *format, va_list args) {
    register int width, pad;
    register int pc = 0;
    char         scr[2];

    for (; *format != 0; ++format) {
        if (*format == '%') {
            ++format;
            width = pad = 0;
            if (*format == '\0') break;
            if (*format == '%') goto out;
            if (*format == '-') {
                ++format;
                pad = PAD_RIGHT;
            }
            while (*format == '0') {
                ++format;
                pad |= PAD_ZERO;
            }
            for (; *format >= '0' && *format <= '9'; ++format) {
                width *= 10;
                width += *format - '0';
            }
            if (*format == 's') {
                register char *s = (char *) va_arg(args, int);
                pc += prints(out, s ? s : "(null)", width, pad);
                continue;
            }
            if (*format == 'd') {
                pc += printi(out, va_arg(args, int), 10, 1, width, pad, 'a');
                continue;
            }
            if (*format == 'x') {
                pc += printi(out, va_arg(args, int), 16, 0, width, pad, 'a');
                continue;
            }
            if (*format == 'X') {
                pc += printi(out, va_arg(args, int), 16, 0, width, pad, 'A');
                continue;
            }
            if (*format == 'u') {
                pc += printi(out, va_arg(args, int), 10, 0, width, pad, 'a');
                continue;
            }
            if (*format == 'c') {
                /* char are converted to int then pushed on the stack */
                scr[0] = (char) va_arg(args, int);
                scr[1] = '\0';
                pc += prints(out, scr, width, pad);
                continue;
            }
        } else {
        out:
            printchar(out, *format);
            ++pc;
        }
    }
    if (out) **out = '\0';
    va_end(args);
    return pc;
}

int u_printf(const char *format, ...) {
    va_list args;
    va_start(args, format);
    print(0, format, args);
    return 0;
}

int u_sprintf(char *out, const char *format, ...) {
    va_list args;
    va_start(args, format);
    return print(&out, format, args);
}

void u_array_printf(unsigned char *data, unsigned int len) {
    u_printf("{");
    for (int i = 0; i < len; ++i) {
        u_printf("%X%s", data[i], i < (len) -1 ? ":" : " ");
    }
    u_printf("}\n");
}