#include "epd_spi.h"

#include <Arduino.h>

#include "HAL_Newton_M3.h"
#include "wdt.h"
#include "stdarg.h"


bool epdGPIOActive = false;

void epdReset() {
    digitalWrite(EPD_RST, HIGH);
    delay(12);
    digitalWrite(EPD_RST, LOW);
    delay(20);
    digitalWrite(EPD_RST, HIGH);
    delay(20);
}

void epdConfigGPIO(bool setup) {
    if (epdGPIOActive == setup)
        return;
    if (setup) {
        pinMode(EPD_POWER, OUTPUT);
        digitalWrite(EPD_POWER, HIGH);
        pinMode(EPD_RST, OUTPUT);
        pinMode(EPD_BS, OUTPUT);
        pinMode(EPD_CS, OUTPUT);
        pinMode(EPD_DC, OUTPUT);
        pinMode(EPD_BUSY, INPUT);
        pinMode(EPD_CLK, OUTPUT);
        pinMode(EPD_MOSI, OUTPUT);

        pinMode(EPD_HLT, OUTPUT);
        pinMode(EPD_VPP, INPUT);

        digitalWrite(EPD_BS, LOW);  // low works!
        digitalWrite(EPD_CS, HIGH);
        digitalWrite(EPD_HLT, HIGH);

        epdHardSPI(true);

    } else {
        epdHardSPI(false);

        digitalWrite(EPD_POWER, LOW);
        pinMode(EPD_RST, OUTPUT);
        pinMode(EPD_BS, OUTPUT);
        pinMode(EPD_CS, OUTPUT);
        pinMode(EPD_DC, OUTPUT);
        pinMode(EPD_BUSY, OUTPUT);
        pinMode(EPD_CLK, OUTPUT);
        pinMode(EPD_MOSI, OUTPUT);
        pinMode(EPD_HLT, OUTPUT);
        pinMode(EPD_VPP, OUTPUT);

        digitalWrite(EPD_RST, LOW);
        digitalWrite(EPD_BS, LOW);
        digitalWrite(EPD_CS, LOW);
        digitalWrite(EPD_DC, LOW);
        digitalWrite(EPD_BUSY, LOW);
        digitalWrite(EPD_CLK, LOW);
        digitalWrite(EPD_MOSI, LOW);
        digitalWrite(EPD_HLT, LOW);
        digitalWrite(EPD_VPP, LOW);
    }
    epdGPIOActive = setup;
}

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
            wdt30s();
            digitalPinToInterrupt(EPD_BUSY);
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

static uint16_t epdSPIXferBytes;
static uint8_t* epdSPIWrite;
static uint8_t* epdSPIRead;
static volatile bool epdSPIWriteComplete = false;

#ifdef __cplusplus
extern "C" {
#endif
// Yes, this is the wrong interrupt handler. We're supposed to be using SPI0, I think. Something is not right in the definition :)
void SPIM1_SPIS1_TWIM1_TWIS1_SPI1_TWI1_IRQHandler(void) {
    NRF_SPI0->EVENTS_READY = 0;
    if (epdSPIRead != nullptr)
        *epdSPIRead++ = NRF_SPI0->RXD;
    else {
        uint8_t dummy = NRF_SPI0->RXD;
        (void)dummy;
    }
    epdSPIXferBytes--;
    if (epdSPIXferBytes) {
        if (epdSPIWrite != nullptr)
            NRF_SPI0->TXD = *(epdSPIWrite++);
        else
            NRF_SPI0->TXD = 0x00;
    } else {
        epdSPIWriteComplete = true;
    }
}
#ifdef __cplusplus
}
#endif

void epdSPIWait() {
    while (!epdSPIWriteComplete)
        ;
}

bool HWSPIStatus = false;
bool HWSPIFirst = true;
void epdHardSPI(bool enable) {
    if (HWSPIStatus != enable) {
        if (enable) {
            // is this the first time we're called to activate HW SPI? Do the full setup
            if (HWSPIFirst) {
                HWSPIFirst = false;
                NRF_SPI0->PSELSCK = EPD_CLK;
                NRF_SPI0->PSELMOSI = EPD_MOSI;
                NRF_SPI0->PSELMISO = EPD_VPP;
                NRF_SPI0->ENABLE = (SPI_ENABLE_ENABLE_Enabled << SPI_ENABLE_ENABLE_Pos);
                NRF_SPI0->CONFIG = (SPI_CONFIG_CPOL_ActiveHigh << SPI_CONFIG_CPOL_Pos) | (SPI_CONFIG_CPHA_Leading << SPI_CONFIG_CPHA_Pos);
                NRF_SPI0->FREQUENCY = SPI_FREQUENCY_FREQUENCY_M8;
                NRF_SPI0->EVENTS_READY = 0;
                NVIC_ClearPendingIRQ(SPIM0_SPIS0_SPI0_IRQn);
                NRF_SPI0->INTENSET = 0x04;
                NVIC_EnableIRQ(SPIM0_SPIS0_SPI0_IRQn);
            } else {
                NRF_SPI0->ENABLE = (SPI_ENABLE_ENABLE_Enabled << SPI_ENABLE_ENABLE_Pos);
            }
        } else {
            NRF_SPI0->ENABLE = (SPI_ENABLE_ENABLE_Disabled << SPI_ENABLE_ENABLE_Pos);
        }
        HWSPIStatus = enable;
    }
}

void epdSPIAsyncWrite(uint8_t* data, uint16_t len) {
    epdSPIWriteComplete = false;
    epdSPIXferBytes = len;
    epdSPIWrite = data;
    epdSPIRead = nullptr;
    NRF_SPI0->TXD = *(epdSPIWrite++);
}

void epdSPIReadBlock(uint8_t* data, uint16_t len) {
    epdSPIWriteComplete = false;
    epdSPIXferBytes = len;
    epdSPIRead = data;
    epdSPIWrite = nullptr;
    NRF_SPI0->TXD = 0x00;
    epdSPIWait();
}

void spi_write(uint8_t data) {
    epdSPIWriteComplete = false;
    epdSPIXferBytes = 1;
    epdSPIWrite = &data;
    epdSPIRead = nullptr;
    NRF_SPI0->TXD = *(epdSPIWrite++);
    epdSPIWait();
}

// This function is used by the UC8151 version of the code, for reading EEPROM LUT data
uint8_t spi_trans(uint8_t data) {
    uint8_t b = 0;
    for (uint8_t i = 0; i < 8; i++) {
        b <<= 1;
        digitalWrite(EPD_CLK, LOW);
        if (data & 0x80) {
            digitalWrite(EPD_MOSI, HIGH);
        } else {
            digitalWrite(EPD_MOSI, LOW);
        }
        if (digitalRead(EPD_VPP)) b |= 1;
        data = (data << 1);
        digitalWrite(EPD_CLK, HIGH);
    }
    return b;
}

// for reading 3-wire SPI data back from the EPD
uint8_t spi3_read() {
    uint8_t b = 0;
    digitalWrite(EPD_BS, HIGH);
    digitalWrite(EPD_CLK, LOW);
    digitalWrite(EPD_MOSI, HIGH);
    digitalWrite(EPD_CS, LOW);
    digitalWrite(EPD_CLK, HIGH);
    pinMode(EPD_MOSI, INPUT);
    for (uint8_t i = 0; i < 8; i++) {
        b <<= 1;
        digitalWrite(EPD_CLK, LOW);
        if (digitalRead(EPD_MOSI)) b |= 1;
        digitalWrite(EPD_CLK, HIGH);
    }
    pinMode(EPD_MOSI, OUTPUT);
    digitalWrite(EPD_MOSI, LOW);
    digitalWrite(EPD_CS, HIGH);
    digitalWrite(EPD_BS, LOW);
    return b;
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

void epdWrite(uint8_t reg, uint8_t len, ...) {
    va_list valist;
    va_start(valist, len);
    markCommand();
    epdSelect();
    spi_write(reg);
    epdDeselect();
    markData();
    for (uint8_t i = 0; i < len; i++) {
        epdSelect();
        spi_write(va_arg(valist, int));
        epdDeselect();
    }
    va_end(valist);
}