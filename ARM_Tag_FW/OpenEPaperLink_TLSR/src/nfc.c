#include <stdint.h>
#include "tl_common.h"
#include "nfc.h"
#include "main.h"
#include "i2c.h"

uint8_t nfc_reset[] = {0x03, 0xb5, 0xa0};

 void init_nfc(void)
{
    gpio_write(NFC_CS, 1);
    gpio_set_func(NFC_CS, AS_GPIO);
    gpio_set_output_en(NFC_CS, 1);
    gpio_set_input_en(NFC_CS, 0);
    gpio_setup_up_down_resistor(NFC_CS, PM_PIN_PULLUP_10K);

    gpio_set_func(NFC_IRQ, AS_GPIO);
    gpio_set_output_en(NFC_IRQ, 0);
    gpio_set_input_en(NFC_IRQ, 1);
    gpio_setup_up_down_resistor(NFC_IRQ, PM_PIN_PULLUP_10K);

    gpio_write(NFC_CS, 0);
    sleep_us(500);
    send_i2c(0xae, nfc_reset, sizeof(nfc_reset));
    gpio_write(NFC_CS, 1);
}
