#include "led.h"
#include "driver/ledc.h"
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
#include "proto.h"

#define NUM_LEDS 2

// The activity LEDs run on LEDC PWM, so the S3 can dim them (SET_CONFIG ledBrightness) or switch
// them off, as it does with its own LEDs (config.led).
const int led_pins[NUM_LEDS] = {LED1, LED2};
TimerHandle_t led_timers[NUM_LEDS] = {0};
static uint32_t led_duty = 255;  // 8-bit duty while a LED is lit; 0 = off

void led_timer_callback(TimerHandle_t xTimer) {
	int led_index = (int)pvTimerGetTimerID(xTimer);
	if (led_index >= 0 && led_index < NUM_LEDS) {
		ledc_set_duty_and_update(LEDC_LOW_SPEED_MODE, led_index, 0, 0);
	}
}

void init_led() {
	ledc_timer_config_t timer = {
		.speed_mode = LEDC_LOW_SPEED_MODE,
		.duty_resolution = LEDC_TIMER_8_BIT,
		.timer_num = LEDC_TIMER_0,
		.freq_hz = 5000,
		.clk_cfg = LEDC_AUTO_CLK,
	};
	ledc_timer_config(&timer);
	for (int i = 0; i < NUM_LEDS; i++) {
		ledc_channel_config_t channel = {
			.gpio_num = led_pins[i],
			.speed_mode = LEDC_LOW_SPEED_MODE,
			.channel = i,
			.timer_sel = LEDC_TIMER_0,
			.duty = 0,
		};
		ledc_channel_config(&channel);
		led_timers[i] = xTimerCreate("led_timer", pdMS_TO_TICKS(50), pdFALSE, (void *)i, led_timer_callback);
	}
	ledc_fade_func_install(0);  // ledc_set_duty_and_update() needs it; it is safe from both tasks
}

void led_flash(int nr) {
	if (nr < 0 || nr >= NUM_LEDS || led_duty == 0) return;
	ledc_set_duty_and_update(LEDC_LOW_SPEED_MODE, nr, led_duty, 0);
	xTimerStart(led_timers[nr], 0);
}

void led_set_brightness(uint8_t brightness) {
	// roughly even steps to the eye, as the S3 does with its gamma table; 1..255 stays visible
	led_duty = brightness ? 1 + (uint32_t)brightness * brightness / 256 : 0;
}
