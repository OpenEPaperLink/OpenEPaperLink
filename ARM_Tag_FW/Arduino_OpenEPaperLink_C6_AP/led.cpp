#include "led.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "proto.h"
#include <Arduino.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NUM_LEDS 2

const uint8_t led_pins[NUM_LEDS] = { LED1, LED2 };
TimerHandle_t led_timers[NUM_LEDS] = { 0 };

void led_timer_callback(TimerHandle_t xTimer) {
  int led_index = (int)pvTimerGetTimerID(xTimer);
  if (led_index >= 0 && led_index < NUM_LEDS) {
    digitalWrite(led_pins[led_index], 0);
  }
}

void init_led() {
  pinMode(LED1,OUTPUT);
  pinMode(LED2,OUTPUT);

  for (int i = 0; i < NUM_LEDS; i++) {
    led_timers[i] = xTimerCreate("led_timer", pdMS_TO_TICKS(50), pdFALSE, (void *)i, led_timer_callback);
  }
}

void led_flash(int nr) {
  digitalWrite(led_pins[nr], 1);
  if (nr >= 0 && nr < NUM_LEDS) {
    xTimerStart(led_timers[nr], 0);
  }
}

void led_set(int nr, bool state) {
  digitalWrite(nr, state);
}