#pragma once
//#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
//#include "core_cm3.h"

#define NO_GUI 0 // Set this to one for faster developing, not Refresh on the E-Paper while showing Full screen messages

#define TEST_PIN GPIO_3
#define RF_WAKEUP_PIN GPIO_5

#define UART_TX GPIO_4
#define UART_RX GPIO_6

#define NFC_POWER GPIO_1
#define NFC_IRQ GPIO_7
#define NFC_SCL GPIO_28
#define NFC_SDA GPIO_29

#define EPD_BS GPIO_2
#define EPD_MOSI GPIO_12
#define EPD_MISO GPIO_13
#define EPD_CLK GPIO_22
#define EPD_CS GPIO_23
#define EPD_RESET GPIO_24
#define EPD_DC GPIO_26
#define EPD_BUSY GPIO_27
#define EPD_HLT_CTRL GPIO_25


#define RADIO_FIRST_CHANNEL				(11)		//2.4-GHz channels start at 11
void applyUpdate(uint32_t len);
void executeCommand(uint8_t cmd);