#include <Arduino.h>
#include <AsyncDelay.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "eeprom.h"
#include "hal.h"
#include "wdt.h"

#define UICR_BASE 0x10001000
#define UICR_VTOR_OFFSET 0x14

#define FW_LOC 0
#define FW_METADATA_LOC 196608

#define MAGIC1 0xABBAABBA
#define MAGIC2 0xDEADBEEF

#define BOOTLOADER_SIZE 16384
#define FLASH_SIZE 196608
#define FLASH_PAGE_SIZE 4096

#define APP_ENTRY BOOTLOADER_SIZE

struct fwmetadata {
    uint32_t fwsize;
    uint32_t magic1;
    uint32_t magic2;
};

extern "C" int _write(int file, char *ptr, int len) {
    (void)file; /* Not used, avoid warning */
    Serial.write(ptr, len);
    return len;
}

bool fwFlashPending() {
    struct fwmetadata metadata;
    eepromRead(FW_METADATA_LOC, &metadata, sizeof(struct fwmetadata));
    if (metadata.magic1 == MAGIC1 && metadata.magic2 == MAGIC2) {
        return true;
    } else {
        return false;
    }
}

uint32_t fwFlashPendingSize() {
    struct fwmetadata metadata;
    eepromRead(FW_METADATA_LOC, &metadata, sizeof(struct fwmetadata));
    return metadata.fwsize;
}

void writeFlashPage(uint32_t address, uint8_t *source) {
    NRF_NVMC->CONFIG = NVMC_CONFIG_WEN_Een;
    while (NRF_NVMC->READY == NVMC_READY_READY_Busy) {
    }
    NRF_NVMC->ERASEPAGE = address;
    while (NRF_NVMC->READY == NVMC_READY_READY_Busy) {
    }
    NRF_NVMC->CONFIG = NVMC_CONFIG_WEN_Ren;
    while (NRF_NVMC->READY == NVMC_READY_READY_Busy) {
    }

    // write bytes
    NRF_NVMC->CONFIG = NVMC_CONFIG_WEN_Wen;
    while (NRF_NVMC->READY == NVMC_READY_READY_Busy) {
    }

    for (uint16_t i = 0; i < FLASH_PAGE_SIZE; i += 4) {
        uint32_t data_word = *((uint32_t *)&(source[i]));
        *((uint32_t *)(address + i)) = data_word;
        while (NRF_NVMC->READY == NVMC_READY_READY_Busy) {
        }
    }

    NRF_NVMC->CONFIG = NVMC_CONFIG_WEN_Ren;
    while (NRF_NVMC->READY == NVMC_READY_READY_Busy) {
    }
}

void removeFwFlashPending() {
    struct fwmetadata metadata;
    metadata.magic1 = 0;
    metadata.magic2 = 0;
    metadata.fwsize = 0;
    eepromWrite(FW_METADATA_LOC, &metadata, sizeof(struct fwmetadata));
}

void setupPortsInitial() {
    digitalWrite(LED_RED, HIGH);
    digitalWrite(LED_GREEN, HIGH);
    digitalWrite(LED_BLUE, HIGH);
    pinMode(LED_RED, OUTPUT);
    pinMode(LED_GREEN, OUTPUT);
    pinMode(LED_BLUE, OUTPUT);
    pinMode(BUTTON1, INPUT_PULLUP);
    pinMode(BUTTON2, INPUT_PULLUP);
    pinMode(NFC_POWER, INPUT_PULLDOWN);
    pinMode(NFC_IRQ, INPUT_PULLDOWN);

    pinMode(FLASH_MISO, INPUT);
    pinMode(FLASH_CLK, OUTPUT);
    pinMode(FLASH_MOSI, OUTPUT);
    digitalWrite(FLASH_CS, HIGH);
    pinMode(FLASH_CS, OUTPUT);
}

typedef void (*app_reset_handler_t)(void);
void jumpToApp(void) {
    // Disable interrupts while we configure the vector table
    __disable_irq();
    SCB->VTOR = APP_ENTRY;
    __set_MSP(*(uint32_t *)APP_ENTRY);
    app_reset_handler_t app_reset_handler = (app_reset_handler_t) * (uint32_t *)(APP_ENTRY + 4);
    __enable_irq();
    app_reset_handler();
}

void ledBlink(uint8_t led) {
    digitalWrite(led, LOW);
    delay(3);
    digitalWrite(led, HIGH);
}

void performFWFlash() {
    uint32_t filesize = fwFlashPendingSize();
    
    for (uint8_t c = 0; c < 15; c++) {
        ledBlink(LED_BLUE);
        delay(70);
    }

    if ((filesize < 8192) || (filesize > (FLASH_SIZE - BOOTLOADER_SIZE))) {
        for (uint8_t c = 0; c < 20; c++) {
            ledBlink(LED_RED);
            delay(100);
        }
        return;
    }
    printf("Flashing %lu bytes from SPI EEPROM\n", filesize);
    uint8_t *eepromdata = (uint8_t *)malloc(FLASH_PAGE_SIZE);
    for (uint32_t offset = 0; offset < filesize; offset += FLASH_PAGE_SIZE) {
        eepromRead(FW_LOC + offset, eepromdata, FLASH_PAGE_SIZE);
        // dump(eepromdata, FLASH_PAGE_SIZE);
        writeFlashPage(APP_ENTRY + offset, eepromdata);
        printf("\rFlashing page %lu...", offset);
        ledBlink(LED_BLUE);
    }
    printf("\nAll done, booting OEPL!\n");
    digitalWrite(LED_GREEN, LOW);
    delay(500);
    digitalWrite(LED_GREEN, HIGH);
}

void setup() {
    Serial.begin(115200);
    delay(300);
    printf("OEPL-BOOT\n");
    setupPortsInitial();
    eepromInit();
    if (fwFlashPending()) {
        performFWFlash();
        removeFwFlashPending();
    } else {
        ledBlink(LED_GREEN);
    }
    jumpToApp();
}

void loop() {
}