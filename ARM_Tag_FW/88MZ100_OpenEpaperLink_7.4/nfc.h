#pragma once
//#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "core_cm3.h"
#include "mz100_pinmux.h"
#include "mz100_gpio.h"
#include "gpio.h"
#include "util.h"

void i2c_pins_init();
void i2c_pins_sleep();
void i2c_start();
void i2c_stop();
void i2c_send_byte(char a1);
uint8_t i2c_receive_byte(int a1);
void i2c_send_buffer(uint8_t buffer[], uint8_t addr, uint8_t reg, int empty);
void i2c_receive_buffer(uint8_t buffer[], uint8_t addr, uint8_t reg, uint32_t len, int a5);
void i2c_send_one_buffer(uint8_t new_address, int a1);
void i2c_receive_one_byte(uint8_t *test_byte, uint8_t addr, uint8_t reg, int a4, int a5);
void nfc_send_16_byte_blocks(uint8_t buffer[], uint32_t len);
void nfc_i2c_init();
uint8_t nfc_handle();
void nfc_erase();