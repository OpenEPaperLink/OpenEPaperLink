#pragma once
//#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "core_cm3.h"

#define NO_GUI 0 // Set this to one for faster developing, not Refresh on the E-Paper while showing Full screen messages

#define TEST_PIN 3
#define RF_WAKEUP_PIN 5

#define UART_TX 4
#define UART_RX 6

#define NFC_POWER 1
#define NFC_IRQ 7
#define NFC_SCL 28
#define NFC_SDA 29

#define EPD_BS 2
#define EPD_MOSI 12
#define EPD_MISO 13
#define EPD_CLK 22
#define EPD_CS 23
#define EPD_RESET 24
#define EPD_DC 26
#define EPD_BUSY 27
#define EPD_HLT_CTRL 25


#define RADIO_FIRST_CHANNEL				(11)		//2.4-GHz channels start at 11

void applyUpdate();