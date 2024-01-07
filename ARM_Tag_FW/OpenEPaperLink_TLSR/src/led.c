#include <stdint.h>
#include "tl_common.h"
#include "led.h"
#include "main.h"

 void init_led(void)
{
    gpio_setup_up_down_resistor(LED_BLUE, PM_PIN_PULLUP_1M);
    gpio_write(LED_BLUE, 1);
    gpio_set_func(LED_BLUE, AS_GPIO);
    gpio_set_output_en(LED_BLUE, 1);
    gpio_set_input_en(LED_BLUE, 0);

    gpio_write(LED_RED, 1);
    gpio_setup_up_down_resistor(LED_RED, PM_PIN_PULLUP_1M);
    gpio_set_func(LED_RED, AS_GPIO);
    gpio_set_output_en(LED_RED, 1);
    gpio_set_input_en(LED_RED, 0);

    gpio_setup_up_down_resistor(LED_GREEN, PM_PIN_PULLUP_1M);
    gpio_write(LED_GREEN, 1);
    gpio_set_func(LED_GREEN, AS_GPIO);
    gpio_set_output_en(LED_GREEN, 1);
    gpio_set_input_en(LED_GREEN, 0);
}

 void set_led_color(uint8_t color)
{
    switch (color)
    {
    case 1:
        gpio_write(LED_BLUE, 1);
        gpio_write(LED_RED, 0);
        gpio_write(LED_GREEN, 1);
        break;
    case 2:
        gpio_write(LED_BLUE, 1);
        gpio_write(LED_RED, 1);
        gpio_write(LED_GREEN, 0);
        break;
    case 3:
        gpio_write(LED_BLUE, 0);
        gpio_write(LED_RED, 1);
        gpio_write(LED_GREEN, 1);
        break;
    default:
        gpio_write(LED_BLUE, 1);
        gpio_write(LED_RED, 1);
        gpio_write(LED_GREEN, 1);
        break;
    }
}
