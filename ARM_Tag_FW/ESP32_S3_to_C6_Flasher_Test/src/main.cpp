#include <Arduino.h>
#include "esp32_port.h"
#include "esp_loader.h"
#include "example_common.h"
#include "flash_files_c6.h"

esp_loader_error_t connect_to_target1(uint32_t higher_transmission_rate)
{
    esp_loader_connect_args_t connect_config = ESP_LOADER_CONNECT_DEFAULT();

    esp_loader_error_t err = esp_loader_connect(&connect_config);
    if (err != ESP_LOADER_SUCCESS)
    {
        printf("Cannot connect to target. Error: %u\n", err);
        return err;
    }
    printf("Connected to target\n");

    if (higher_transmission_rate && esp_loader_get_target() != ESP8266_CHIP)
    {
        err = esp_loader_change_transmission_rate(higher_transmission_rate);
        if (err == ESP_LOADER_ERROR_UNSUPPORTED_FUNC)
        {
            printf("ESP8266 does not support change transmission rate command.");
            return err;
        }
        else if (err != ESP_LOADER_SUCCESS)
        {
            printf("Unable to change transmission rate on target.");
            return err;
        }
        else
        {
            err = loader_port_change_transmission_rate(higher_transmission_rate);
            if (err != ESP_LOADER_SUCCESS)
            {
                printf("Unable to change transmission rate.");
                return err;
            }
            printf("Transmission rate changed changed\n");
        }
    }
    return ESP_LOADER_SUCCESS;
}

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))
esp_loader_error_t flash_binary1(const uint8_t *bin, size_t size, size_t address)
{
    esp_loader_error_t err;
    static uint8_t payload[1024];
    const uint8_t *bin_addr = bin;

    printf("Erasing flash (this may take a while)...\n");
    err = esp_loader_flash_start(address, size, sizeof(payload));
    if (err != ESP_LOADER_SUCCESS)
    {
        printf("Erasing flash failed with error %d.\n", err);
        return err;
    }
    printf("Start programming\n");

    size_t binary_size = size;
    size_t written = 0;

    while (size > 0)
    {
        size_t to_read = MIN(size, sizeof(payload));
        memcpy(payload, bin_addr, to_read);

        err = esp_loader_flash_write(payload, to_read);
        if (err != ESP_LOADER_SUCCESS)
        {
            printf("\nPacket could not be written! Error %d.\n", err);
            return err;
        }

        size -= to_read;
        bin_addr += to_read;
        written += to_read;

        int progress = (int)(((float)written / binary_size) * 100);
        printf("\rProgress: %d %%", progress);
        fflush(stdout);
    };

    printf("\nFinished programming\n");

#if MD5_ENABLED
    err = esp_loader_flash_verify();
    if (err == ESP_LOADER_ERROR_UNSUPPORTED_FUNC)
    {
        printf("ESP8266 does not support flash verify command.");
        return err;
    }
    else if (err != ESP_LOADER_SUCCESS)
    {
        printf("MD5 does not match. err: %d\n", err);
        return err;
    }
    printf("Flash verified\n");
#endif

    return ESP_LOADER_SUCCESS;
}

void setup()
{
    pinMode(17, INPUT_PULLUP);
    pinMode(18, INPUT_PULLUP);
    Serial.begin(115200);
    delay(1000);
    Serial.println("ESP_Flasher_hi");

    const loader_esp32_config_t config = {
        .baud_rate = 115200,
        .uart_port = 2,
        .uart_rx_pin = GPIO_NUM_19,
        .uart_tx_pin = GPIO_NUM_20,
        .reset_trigger_pin = GPIO_NUM_47,
        .gpio0_trigger_pin = GPIO_NUM_21,
    };

    Serial.printf("serial initialization: %i \r\n", loader_port_esp32_init(&config));

    if (connect_to_target1(230400) == ESP_LOADER_SUCCESS)
    {
    Serial.printf("We got the following ESP: %i\r\n", esp_loader_get_target());
        Serial.println("Loading bootloader...");
        flash_binary1(data_bootloader, sizeof(data_bootloader), 0x0);
        Serial.println("Loading partition table...");
        flash_binary1(data_table, sizeof(data_table), 0x8000);
        Serial.println("Loading app...");
        flash_binary1(data_application, sizeof(data_application), 0x10000);
        Serial.println("Done!");
    }
}

void loop()
{
    Serial.printf("MS: %u\r\n", millis());
    delay(1000);
}