#include "led.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "proto.h"
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NUM_LEDS 2

const gpio_num_t led_pins[NUM_LEDS] = {LED1, LED2};
TimerHandle_t led_timers[NUM_LEDS] = {0};

void led_timer_callback(TimerHandle_t xTimer) {
	int led_index = (int)pvTimerGetTimerID(xTimer);
	if (led_index >= 0 && led_index < NUM_LEDS) {
		gpio_set_level(led_pins[led_index], 0);
	}
}

void init_led() {
	gpio_config_t led1 = {};
	led1.intr_type = GPIO_INTR_DISABLE;
	led1.mode = GPIO_MODE_OUTPUT;
	led1.pin_bit_mask = ((1ULL << LED1) | (1ULL << LED2));
	led1.pull_down_en = 0;
	led1.pull_up_en = 0;
	gpio_config(&led1);

	for (int i = 0; i < NUM_LEDS; i++) {
		led_timers[i] = xTimerCreate("led_timer", pdMS_TO_TICKS(100), pdFALSE, (void *)i, led_timer_callback);
	}
}

void led_flash(int nr) {
	gpio_set_level(led_pins[nr], 1);
	if (nr >= 0 && nr < NUM_LEDS) {
		xTimerStart(led_timers[nr], 0);
	}
}

void led_set(int nr, bool state) {
	gpio_set_level(nr, state);
}