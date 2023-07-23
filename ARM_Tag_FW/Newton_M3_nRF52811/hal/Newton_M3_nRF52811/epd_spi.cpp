#include "epd_spi.h"

#include <Arduino.h>

#include "hal_Newton_M3.h"
#include "wdt.h"

void epd_busy() {
}

void epdBusyWaitFalling(uint32_t timeout) {
    uint32_t start = millis();
    wdt120s();
    attachInterrupt(digitalPinToInterrupt(EPD_BUSY), epd_busy, FALLING);
    NRF_GPIO->PIN_CNF[EPD_BUSY] &= ~((uint32_t)GPIO_PIN_CNF_SENSE_Msk);
    NRF_GPIO->PIN_CNF[EPD_BUSY] |= ((uint32_t)GPIO_PIN_CNF_SENSE_Low << GPIO_PIN_CNF_SENSE_Pos);
    while (millis() - start < timeout) {
        initRTC0(100);
        __WFE();
        __SEV();
        __WFE();
        if (!digitalRead(EPD_BUSY)) {
            wdt30s();
            return;
        }
    }
    printf("screen timeout %lu ms :(\n", millis() - start);
    while (1) {
        if (!digitalRead(EPD_BUSY)) {
            wdt30s();
            break;
        }
    }
}

void epdBusyWaitRising(uint32_t timeout) {
    uint32_t start = millis();
    wdt120s();
    attachInterrupt(digitalPinToInterrupt(EPD_BUSY), epd_busy, RISING);
    NRF_GPIO->PIN_CNF[EPD_BUSY] &= ~((uint32_t)GPIO_PIN_CNF_SENSE_Msk);
    NRF_GPIO->PIN_CNF[EPD_BUSY] |= ((uint32_t)GPIO_PIN_CNF_SENSE_High << GPIO_PIN_CNF_SENSE_Pos);
    while (millis() - start < timeout) {
        initRTC0(100);
        __WFE();
        __SEV();
        __WFE();
        if (digitalRead(EPD_BUSY)) {
            printf("Screen done %lu ms\n", millis() - start);
            wdt30s();
            return;
        }
    }
    printf("screen timeout %lu ms :(\n", millis() - start);
    while (1) {
        if (digitalRead(EPD_BUSY)) {
            printf("Screen done %lu ms\n", millis() - start);
            wdt30s();
            break;
        }
    }
}

void spi_write(uint8_t data) {
    for (uint8_t i = 0; i < 8; i++) {
        digitalWrite(EPD_CLK, LOW);
        if (data & 0x80) {
            digitalWrite(EPD_MOSI, HIGH);
        } else {
            digitalWrite(EPD_MOSI, LOW);
        }
        data = (data << 1);
        digitalWrite(EPD_CLK, HIGH);
    }
}

void epd_cmd(uint8_t data) {
    digitalWrite(EPD_DC, LOW);
    digitalWrite(EPD_CS, LOW);
    spi_write(data);
    digitalWrite(EPD_CS, HIGH);
}

void epd_data(uint8_t data) {
    digitalWrite(EPD_DC, HIGH);
    digitalWrite(EPD_CS, LOW);
    spi_write(data);
    digitalWrite(EPD_CS, HIGH);
}

void waitBusy() {
    while (digitalRead(EPD_BUSY) == HIGH) {
    }
}
