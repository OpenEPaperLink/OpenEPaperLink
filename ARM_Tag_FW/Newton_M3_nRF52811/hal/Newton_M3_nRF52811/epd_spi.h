#pragma once
#include <stdint.h>

void spi_write(uint8_t data);
void epd_cmd(uint8_t data);
void epd_data(uint8_t data);
void waitBusy();
void epdBusyWaitFalling(uint32_t timeout);
void epdBusyWaitRising(uint32_t timeout);

uint8_t spi3_read();
uint8_t spi_trans(uint8_t data);
void epdWrite(uint8_t reg, uint8_t len, ...);

void epdReset();
void epdConfigGPIO(bool setup);
extern bool epdGPIOActive;

void epdSPIAsyncWrite(uint8_t* data, uint16_t len);
void epdSPIWait();
void epdSPIReadBlock(uint8_t* data, uint16_t len);
void epdHardSPI(bool enable);

#define epdSelect()                \
    do                             \
    {                              \
        digitalWrite(EPD_CS, LOW); \
    } while (0)

#define epdDeselect()               \
    do                              \
    {                               \
        digitalWrite(EPD_CS, HIGH); \
    } while (0)

#define markCommand()              \
    do {                           \
        digitalWrite(EPD_DC, LOW); \
    } while (0)

#define markData()                  \
    do {                            \
        digitalWrite(EPD_DC, HIGH); \
    } while (0)
