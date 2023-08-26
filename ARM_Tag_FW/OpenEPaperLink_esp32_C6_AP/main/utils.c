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
#include "proto.h"
#include "sdkconfig.h"
#include "soc/uart_struct.h"
#include "soc/lp_uart_reg.h"
#include "nvs_flash.h"

void delay(int ms) { vTaskDelay(pdMS_TO_TICKS(ms)); }

uint32_t getMillis() { return (uint32_t) (esp_timer_get_time() / 1000); }

void init_nvs()
{	
    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
	ESP_ERROR_CHECK( ret );
}