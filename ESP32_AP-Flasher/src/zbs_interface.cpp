
/*   Autor: Aaron Christophel ATCnetz.de   */
#include "zbs_interface.h"

#include <Arduino.h>
#include <SPI.h>
#include <stdint.h>
#include <stdio.h>

#include "powermgt.h"

uint8_t ZBS_interface::begin(uint8_t SS, uint8_t CLK, uint8_t MOSI, uint8_t MISO, uint8_t RESET, uint8_t POWER, uint32_t spi_speed) {
    _SS_PIN = SS;
    _CLK_PIN = CLK;
    _MOSI_PIN = MOSI;
    _MISO_PIN = MISO;
    _RESET_PIN = RESET;
    _POWER_PIN = POWER;
    pinMode(_SS_PIN, OUTPUT);
    pinMode(_RESET_PIN, OUTPUT);
    digitalWrite(_SS_PIN, HIGH);
    digitalWrite(_RESET_PIN, HIGH);
    set_power(ZBS_ON);
    pinMode(_CLK_PIN, OUTPUT);
    pinMode(_MOSI_PIN, OUTPUT);
    pinMode(_MISO_PIN, INPUT);
    digitalWrite(_CLK_PIN, LOW);
    digitalWrite(_MOSI_PIN, HIGH);

    if (!spi) spi = new SPIClass(HSPI);

    spiSettings = SPISettings(spi_speed, MSBFIRST, SPI_MODE0);
    spi_ready = 0;

    if (spi_speed != 8000000) {
        after_byte_delay = 10;
    } else {
        after_byte_delay = 10;
    }
    enable_debug();
    return check_connection();
}

void ZBS_interface::setSpeed(uint32_t speed) {
    spiSettings = SPISettings(speed, MSBFIRST, SPI_MODE0);
}

ZBS_interface::~ZBS_interface() {
    if(spi)delete spi;
}
void ZBS_interface::set_power(uint8_t state) {
    if (_POWER_PIN != 255) {
        rampTagPower(_POWER_PIN, state);
    }
}

void ZBS_interface::enable_debug() {
    digitalWrite(_RESET_PIN, HIGH);
    digitalWrite(_SS_PIN, HIGH);
    digitalWrite(_CLK_PIN, LOW);
    digitalWrite(_MOSI_PIN, HIGH);
    delay(30);
    digitalWrite(_RESET_PIN, LOW);
    delay(33);
    digitalWrite(_CLK_PIN, HIGH);
    delay(1);
    digitalWrite(_CLK_PIN, LOW);
    delay(1);
    digitalWrite(_CLK_PIN, HIGH);
    delay(1);
    digitalWrite(_CLK_PIN, LOW);
    delay(1);
    digitalWrite(_CLK_PIN, HIGH);
    delay(1);
    digitalWrite(_CLK_PIN, LOW);
    delay(1);
    digitalWrite(_CLK_PIN, HIGH);
    delay(1);
    digitalWrite(_CLK_PIN, LOW);
    delay(9);
    digitalWrite(_RESET_PIN, HIGH);
    delay(100);
}

void ZBS_interface::reset() {
    spi->end();
    pinMode(_SS_PIN, INPUT);
    pinMode(_CLK_PIN, INPUT);
    pinMode(_MOSI_PIN, INPUT);
    pinMode(_MISO_PIN, INPUT);
    digitalWrite(_RESET_PIN, LOW);
    set_power(ZBS_OFF);
    delay(500);
    digitalWrite(_RESET_PIN, HIGH);
    set_power(ZBS_ON);
    pinMode(_RESET_PIN, INPUT);
}

void ZBS_interface::send_byte(uint8_t data) {
    digitalWrite(_SS_PIN, LOW);
    delayMicroseconds(5);
    if (!spi_ready) {
        spi_ready = 1;
        spi->begin(_CLK_PIN, _MISO_PIN, _MOSI_PIN);
    }
    spi->beginTransaction(spiSettings);
    spi->transfer(data);
    spi->endTransaction();

    delayMicroseconds(2);
    digitalWrite(_SS_PIN, HIGH);
}

uint8_t ZBS_interface::read_byte() {
    uint8_t data = 0x00;
    digitalWrite(_SS_PIN, LOW);
    delayMicroseconds(5);
    if (!spi_ready) {
        spi_ready = 1;
        spi->begin(_CLK_PIN, _MISO_PIN, _MOSI_PIN);
    }
    spi->beginTransaction(spiSettings);
    data = spi->transfer(0xff);
    spi->endTransaction();
    delayMicroseconds(2);
    digitalWrite(_SS_PIN, HIGH);
    return data;
}

void ZBS_interface::write_byte(uint8_t cmd, uint8_t addr, uint8_t data) {
    send_byte(cmd);
    send_byte(addr);
    send_byte(data);
    delayMicroseconds(after_byte_delay);
}

uint8_t ZBS_interface::read_byte(uint8_t cmd, uint8_t addr) {
    uint8_t data = 0x00;
    send_byte(cmd);
    send_byte(addr);
    data = read_byte();
    delayMicroseconds(after_byte_delay);
    return data;
}

void ZBS_interface::write_flash(uint16_t addr, uint8_t data) {
    send_byte(ZBS_CMD_W_FLASH);
    send_byte(addr >> 8);
    send_byte(addr);
    send_byte(data);
    delayMicroseconds(after_byte_delay);
}

uint8_t ZBS_interface::read_flash(uint16_t addr) {
    uint8_t data = 0x00;
    send_byte(ZBS_CMD_R_FLASH);
    send_byte(addr >> 8);
    send_byte(addr);
    data = read_byte();
    delayMicroseconds(after_byte_delay);
    return data;
}

void ZBS_interface::write_ram(uint8_t addr, uint8_t data) {
    write_byte(ZBS_CMD_W_RAM, addr, data);
}

uint8_t ZBS_interface::read_ram(uint8_t addr) {
    return read_byte(ZBS_CMD_R_RAM, addr);
}

void ZBS_interface::write_sfr(uint8_t addr, uint8_t data) {
    write_byte(ZBS_CMD_W_SFR, addr, data);
}

uint8_t ZBS_interface::read_sfr(uint8_t addr) {
    return read_byte(ZBS_CMD_R_SFR, addr);
}

uint8_t ZBS_interface::check_connection() {
    uint8_t test_byte = 0xA5;
    write_ram(0xba, test_byte);
    delay(1);
    return read_ram(0xba) == test_byte;
}

uint8_t ZBS_interface::select_flash(uint8_t page) {
    uint8_t sfr_low_bank = page ? 0x80 : 0x00;
    write_sfr(0xd8, sfr_low_bank);
    delay(1);
    return read_sfr(0xd8) == sfr_low_bank;
}

void ZBS_interface::erase_flash() {
    send_byte(ZBS_CMD_ERASE_FLASH);
    send_byte(0x00);
    send_byte(0x00);
    send_byte(0x00);
    delay(100);
}

void ZBS_interface::erase_infoblock() {
    send_byte(ZBS_CMD_ERASE_INFOBLOCK);
    send_byte(0x00);
    send_byte(0x00);
    send_byte(0x00);
    delay(100);
}
