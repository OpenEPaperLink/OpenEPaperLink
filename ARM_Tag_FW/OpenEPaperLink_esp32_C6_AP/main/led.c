#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "led.h"
#include "proto.h"

void init_led()
{
    gpio_config_t led1 = {};
    led1.intr_type = GPIO_INTR_DISABLE;
    led1.mode = GPIO_MODE_OUTPUT;
    led1.pin_bit_mask = ((1ULL<<LED1) | (1ULL<<LED2));
    led1.pull_down_en = 0;
    led1.pull_up_en = 0;
    gpio_config(&led1);
}

void led_set(int nr, bool state)
{
    gpio_set_level(nr, state);
}