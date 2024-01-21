#pragma once
#include <stdio.h>
#include <stdint.h>
#include <SPI.h>

/*   Autor: Aaron Christophel ATCnetz.de   */
#include <Arduino.h>

class ZBS_interface
{
public:
    uint8_t begin(uint8_t SS, uint8_t CLK, uint8_t MOSI, uint8_t MISO, uint8_t RESET, uint8_t* POWER, uint8_t powerPins, uint32_t spi_speed = 8000000);
    void setSpeed(uint32_t speed);
    void set_power(uint8_t state);
    void enable_debug();
    void reset(bool leavepower = true);
    void send_byte(uint8_t data);
    uint8_t read_byte();
    void write_byte(uint8_t cmd, uint8_t addr, uint8_t data);
    uint8_t read_byte(uint8_t cmd, uint8_t addr);
    void write_flash(uint16_t addr, uint8_t data);
    uint8_t read_flash(uint16_t addr);
    void write_ram(uint8_t addr, uint8_t data);
    uint8_t read_ram(uint8_t addr);
    void write_sfr(uint8_t addr, uint8_t data);
    uint8_t read_sfr(uint8_t addr);
    uint8_t check_connection();
    uint8_t select_flash(uint8_t page);
    void erase_flash();
    void erase_infoblock();
    ~ZBS_interface();

private:
    SPIClass *spi = NULL;
    SPISettings spiSettings;
    uint8_t _SS_PIN = -1;
    uint8_t _CLK_PIN = -1;
    uint8_t _MOSI_PIN = -1;
    uint8_t _MISO_PIN = -1;
    uint8_t _RESET_PIN = -1;
    uint8_t* _POWER_PIN = nullptr;
    uint8_t _POWER_PINS = 1;
    int ZBS_spi_delay = 1;
    uint8_t spi_ready = 0;
    uint32_t after_byte_delay = 10;

    typedef enum
    {
        ZBS_CMD_W_RAM = 0x02,
        ZBS_CMD_R_RAM = 0x03,
        ZBS_CMD_W_FLASH = 0x08,
        ZBS_CMD_R_FLASH = 0x09,
        ZBS_CMD_W_SFR = 0x12,
        ZBS_CMD_R_SFR = 0x13,
        ZBS_CMD_ERASE_FLASH = 0x88,
        ZBS_CMD_ERASE_INFOBLOCK = 0x48,
    } ZBS_CMD_LIST;

    typedef enum
    {
        ZBS_ON = 1,
        ZBS_OFF = 0,
    } ZBS_POWER_STATE;
};