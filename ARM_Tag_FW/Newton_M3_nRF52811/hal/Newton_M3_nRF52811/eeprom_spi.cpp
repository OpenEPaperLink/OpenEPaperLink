#include "eeprom_spi.h"
#include <Arduino.h>
#include "HAL_Newton_M3.h"
#include "wdt.h"

bool eepromHWSPIStatus = false;
bool eepromHWSPIFirst = true;
void setupEepromHWSPI(bool enable) {
    if (eepromHWSPIStatus != enable) {
        if (enable) {
            pinMode(FLASH_MISO, INPUT);
            // is this the first time we're called to activate HW SPI? Do the full setup
            if (eepromHWSPIFirst) {
                eepromHWSPIFirst = false;
                NRF_SPI1->PSELSCK = FLASH_CLK;
                NRF_SPI1->PSELMOSI = FLASH_MOSI;
                NRF_SPI1->PSELMISO = FLASH_MISO;
                NRF_SPI1->ENABLE = (SPI_ENABLE_ENABLE_Enabled << SPI_ENABLE_ENABLE_Pos);
                NRF_SPI1->CONFIG = (SPI_CONFIG_CPOL_ActiveHigh << SPI_CONFIG_CPOL_Pos) | (SPI_CONFIG_CPHA_Leading << SPI_CONFIG_CPHA_Pos);
                NRF_SPI1->FREQUENCY = SPI_FREQUENCY_FREQUENCY_M8;
                NRF_SPI1->EVENTS_READY = 0;
            } else {
                NRF_SPI1->ENABLE = (SPI_ENABLE_ENABLE_Enabled << SPI_ENABLE_ENABLE_Pos);
            }
        } else {
            NRF_SPI1->ENABLE = (SPI_ENABLE_ENABLE_Disabled << SPI_ENABLE_ENABLE_Pos);
            digitalWrite(FLASH_CS, HIGH);
            digitalWrite(FLASH_MISO, HIGH);
            pinMode(FLASH_MISO, OUTPUT);
        }
        eepromHWSPIStatus = enable;
    }
}

uint8_t eepromSPIByte(uint8_t data) {
    // setupEepromHWSPI(true);
    NRF_SPI1->TXD = data;
    while (!NRF_SPI1->EVENTS_READY) {
    }
    data = NRF_SPI1->RXD;

    NRF_SPI1->EVENTS_READY = 0;
    return data;
}

void eepromSPIBlockRead(uint8_t* dst, uint16_t len) {
    while (len--) {
        NRF_SPI1->TXD = 0x00;
        while (!NRF_SPI1->EVENTS_READY) {
        }
        *dst++ = NRF_SPI1->RXD;
        NRF_SPI1->EVENTS_READY = 0;
    }
}

void eepromSPIBlockWrite(uint8_t* src, uint16_t len) {
    uint8_t dummy = 0;
    while (len--) {
        NRF_SPI1->TXD = *src++;
        while (!NRF_SPI1->EVENTS_READY) {
        }
        dummy = NRF_SPI1->RXD;  // we have to read some data, otherwise SPI will clog up.
        NRF_SPI1->EVENTS_READY = 0;
    }
    (void)dummy;  // shut up that unused warning
}