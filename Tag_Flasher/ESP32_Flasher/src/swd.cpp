/*
   Copyright (c) 2021 Aaron Christophel ATCnetz.de
   SPDX-License-Identifier: GPL-3.0-or-later
*/
#include "swd.h"

#include "Arduino.h"

// Many thanks to scanlime for the work on the ESP8266 SWD Library, parts of this code have inspiration and help from it
// https://github.com/scanlime/esp8266-arm-swd

#define DP_IDCODE 0x00

swd::swd(uint8_t swdio, uint8_t swdclk) {
    this->swdio_pin = swdio;
    this->swdclk_pin = swdclk;
    this->swd_Begin();
}
void swd::swd_Begin() {
    pinMode(this->swdclk_pin, OUTPUT);
    pinMode(this->swdio_pin, INPUT_PULLUP);
}
uint32_t swd::swd_Init() {  // Returns the ID
    swd_Write(0xffffffff, 32);
    swd_Write(0xffffffff, 32);
    swd_Write(0xe79e, 16);
    swd_Write(0xffffffff, 32);
    swd_Write(0xffffffff, 32);
    swd_Write(0, 32);
    swd_Write(0, 32);

    DP_Read(DP_IDCODE, this->idCode);
    return this->idCode;
}
bool swd::AP_Write(unsigned addr, uint32_t data) {
    uint8_t retry = 15;
    while (retry--) {
        bool state = swd_Transfer(addr, 1, 0, data);
        if (state)
            return true;
    }
    return false;
}
bool swd::AP_Read(unsigned addr, uint32_t &data) {
    uint8_t retry = 15;
    while (retry--) {
        bool state = swd_Transfer(addr, 1, 1, data);
        if (state)
            return true;
    }
    return false;
}
bool swd::DP_Write(unsigned addr, uint32_t data) {
    uint8_t retry = 15;
    while (retry--) {
        bool state = swd_Transfer(addr, 0, 0, data);
        if (state)
            return true;
    }
    return false;
}
bool swd::DP_Read(unsigned addr, uint32_t &data) {
    uint8_t retry = 15;
    while (retry--) {
        bool state = swd_Transfer(addr, 0, 1, data);
        if (state)
            return true;
    }
    return false;
}
bool swd::swd_Transfer(unsigned port_address, bool APorDP, bool RorW, uint32_t &data) {
    bool parity = APorDP ^ RorW ^ ((port_address >> 2) & 1) ^ ((port_address >> 3) & 1);
    uint8_t filled_address = (1 << 0) | (APorDP << 1) | (RorW << 2) | ((port_address & 0xC) << 1) | (parity << 5) | (1 << 7);
    swd_Write(filled_address, 8);
    if (swd_Read(3) == 1) {
        if (RorW) {  // Reading 32 bits from SWD
            data = swd_Read(32);
            if (swd_Read(1) == calculate_Parity(data)) {
                swd_Write(0, 1);
                return true;
            }
        } else {  // Writing 32bits to SWD
            swd_Write(data, 32);
            swd_Write(calculate_Parity(data), 1);
            swd_Write(0, 1);
            return true;
        }
    }
    swd_Write(0, 32);
    return false;
}
bool swd::calculate_Parity(uint32_t in_data) {
    in_data = (in_data & 0xFFFF) ^ (in_data >> 16);
    in_data = (in_data & 0xFF) ^ (in_data >> 8);
    in_data = (in_data & 0xF) ^ (in_data >> 4);
    in_data = (in_data & 0x3) ^ (in_data >> 2);
    in_data = (in_data & 0x1) ^ (in_data >> 1);
    return in_data;
}
void swd::swd_Write(uint32_t in_data, uint8_t bits) {
    if (cur_swd_direction == 0)
        swd_Direction(1);
    while (bits--) {
        digitalWrite(this->swdio_pin, in_data & 1);
        digitalWrite(this->swdclk_pin, LOW);
        delayMicroseconds(2);
        in_data >>= 1;
        digitalWrite(this->swdclk_pin, HIGH);
        delayMicroseconds(2);
    }
}
uint32_t swd::swd_Read(uint8_t bits) {
    uint32_t out_data = 0;
    uint32_t input_bit = 1;
    if (cur_swd_direction == 1)
        swd_Direction(0);
    while (bits--) {
        if (digitalRead(this->swdio_pin)) {
            out_data |= input_bit;
        }
        digitalWrite(this->swdclk_pin, LOW);
        delayMicroseconds(2);
        input_bit <<= 1;
        digitalWrite(this->swdclk_pin, HIGH);
        delayMicroseconds(2);
    }
    return out_data;
}
void swd::swd_Direction(bool WorR) {  // 1 = Write 0 = Read
    digitalWrite(this->swdio_pin, HIGH);
    pinMode(this->swdio_pin, INPUT_PULLUP);
    digitalWrite(this->swdclk_pin, LOW);
    delayMicroseconds(2);
    digitalWrite(this->swdclk_pin, HIGH);
    delayMicroseconds(2);
    if (WorR)
        pinMode(this->swdio_pin, OUTPUT);
    cur_swd_direction = WorR;
}

#define PORT_AP true
#define PORT_DP false

#define AP_NRF_RESET 0x00
#define AP_NRF_ERASEALL 0x04
#define AP_NRF_ERASEALLSTATUS 0x08
#define AP_NRF_ERASEALL 0x04
#define AP_NRF_APPROTECTSTATUS 0x0c
#define AP_NRF_IDR 0xfc

#define AP_CSW 0x00
#define AP_TAR 0x04
#define AP_DRW 0x0c
#define AP_BD0 0x10
#define AP_BD1 0x14
#define AP_BD2 0x18
#define AP_BD3 0x1c
#define AP_DBGDRAR 0xf8
#define AP_IDR 0xfc

#define DP_ABORT 0x00
#define DP_CTRLSTAT 0x04
#define DP_SELECT 0x08
#define DP_RDBUFF 0x0c

nrfswd::nrfswd(uint8_t swdio, uint8_t swdclk) : swd(swdio, swdclk) {
}
bool nrfswd::init() {
    uint32_t temp = swd_Init();
    nrf_abort_all();
    if (temp == 0x2ba01477) {  // if core id is readable the connection is working
        if (showDebug) Serial0.printf("Connected to nRF\n");
        isConnected = true;
        if (nrf_read_lock_state()) {  // nRF is unlocked so we can talk to the debugging interface
            if (showDebug) Serial0.printf("nRF is unlocked!\n");
            isLocked = false;
            nrf_halt();
            nrf_read_ufcr();
            return true;
        } else {
            if (showDebug) Serial0.printf("nRF is locked ;_;\n");
            isLocked = true;
        }
    } else {
        if (showDebug) Serial0.printf("No nRF core ID found, got 0x%lx\n", temp);
        isConnected = false;
        isLocked = true;
    }
    return false;
}
uint32_t nrfswd::nrf_read_port(bool APorDP, uint8_t address) {
    uint32_t temp = 0;
    if (APorDP == PORT_AP)
        AP_Read(address, temp);
    else
        DP_Read(address, temp);
    DP_Read(DP_RDBUFF, temp);
    DP_Read(DP_RDBUFF, temp);
    if (showDebug) Serial0.printf("%s Read reg: 0x%02x : 0x%08x\r\n", APorDP ? "AP" : "DP", address, temp);
    return temp;
}
void nrfswd::nrf_write_port(bool APorDP, uint8_t address, uint32_t value) {
    uint32_t temp = 0;
    if (APorDP == PORT_AP)
        AP_Write(address, value);
    else
        DP_Write(address, value);
    DP_Read(DP_RDBUFF, temp);
    DP_Read(DP_RDBUFF, temp);
    if (showDebug) Serial0.printf("%s Write reg: 0x%02x : 0x%08x\r\n", APorDP ? "AP" : "DP", address, value);
}
void nrfswd::nrf_abort_all() {
    nrf_write_port(0, DP_ABORT, 0x1e);
    nrf_write_port(0, DP_CTRLSTAT, 0x50000000);
}
void nrfswd::nrf_halt() {
    AP_Write(AP_CSW, 0xa2000002);
    AP_Write(AP_TAR, 0xe000edf0);
    uint32_t retry = 500;
    while (retry--) {
        AP_Write(AP_DRW, 0xA05F0003);
    }
}
void nrfswd::nrf_port_selection(bool new_port) {
    DP_Write(DP_SELECT, new_port ? 0x01000000 : 0x00);  // Select AP
}
bool nrfswd::nrf_read_lock_state() {
    uint32_t temp;
    nrf_port_selection(1);
    temp = nrf_read_port(1, AP_NRF_APPROTECTSTATUS);
    nrf_port_selection(0);
    return temp & 1;
}
void nrfswd::nrf_read_ufcr() {
    nrf_info.codepage_size = read_register(0x10000010);
    nrf_info.codesize = read_register(0x10000014);
    nrf_info.flash_size = nrf_info.codepage_size * nrf_info.codesize;
    nrf_info.config_id = read_register(0x1000005c);
    nrf_info.device_id0 = read_register(0x10000060);
    nrf_info.device_id1 = read_register(0x10000064);
    nrf_info.info_part = read_register(0x10000100);
    nrf_info.info_variant = read_register(0x10000104);
    nrf_info.info_package = read_register(0x10000108);
    nrf_info.sd_info_area = read_register(0x0000300C) & 0xffff;
    nrf_info.ucir_lock = read_register(0x10001208);

    if (showDebug) {
        Serial0.printf("Device: nRF%8X\n", nrf_info.info_part);
        Serial0.printf("Flash size: %i\r\n", nrf_info.flash_size);
    }
}
uint32_t nrfswd::read_register(uint32_t address) {
    uint32_t temp = 0;
    bool state1 = AP_Write(AP_TAR, address);
    bool state2 = AP_Read(AP_DRW, temp);
    bool state3 = DP_Read(DP_RDBUFF, temp);
    bool state4 = DP_Read(DP_RDBUFF, temp);
    if (showDebug)
        Serial0.printf("%i%i%i%i Read Register: 0x%08x : 0x%08x\r\n", state1, state2, state3, state4, address, temp);
    return temp;
}
void nrfswd::write_register(uint32_t address, uint32_t value) {
    uint32_t temp = 0;
    bool state1 = AP_Write(AP_TAR, address);
    bool state2 = AP_Write(AP_DRW, value);
    bool state3 = DP_Read(DP_RDBUFF, temp);
    if (showDebug)
        Serial0.printf("%i%i%i Write Register: 0x%08x : 0x%08x\r\n", state1, state2, state3, address, value);
}

uint8_t nrfswd::erase_all_flash() {
    write_register(0x4001e504, 2);
    long timeout = millis();
    while (read_register(0x4001e400) != 1) {
        if (millis() - timeout > 100) return 1;
    }
    write_register(0x4001e50c, 1);
    timeout = millis();
    while (read_register(0x4001e400) != 1) {
        if (millis() - timeout > 1000) return 1;
    }
    write_register(0x4001e504, 0);
    timeout = millis();
    while (read_register(0x4001e400) != 1) {
        if (millis() - timeout > 100) return 1;
    }
    return 0;
}

uint8_t nrfswd::erase_uicr() {
    write_register(0x4001e504, 2);
    long timeout = millis();
    while (read_register(0x4001e400) != 1) {
        if (millis() - timeout > 100) return 1;
    }
    write_register(0x4001e514, 1);
    timeout = millis();
    while (read_register(0x4001e400) != 1) {
        if (millis() - timeout > 1000) return 1;
    }
    write_register(0x4001e504, 0);
    timeout = millis();
    while (read_register(0x4001e400) != 1) {
        if (millis() - timeout > 100) return 1;
    }
    return 0;
}

uint8_t nrfswd::erase_page(uint32_t page) {
    write_register(0x4001e504, 2);
    long timeout = millis();
    while (read_register(0x4001e400) != 1) {
        if (millis() - timeout > 100) return 1;
    }
    write_register(0x4001e508, page);
    timeout = millis();
    while (read_register(0x4001e400) != 1) {
        if (millis() - timeout > 500) return 1;
    }
    write_register(0x4001e504, 0);
    timeout = millis();
    while (read_register(0x4001e400) != 1) {
        if (millis() - timeout > 100) return 1;
    }
    return 0;
}

void nrfswd::nrf_soft_reset() {
    nrf_port_selection(1);
    nrf_write_port(1, AP_NRF_RESET, 1);
    delay(100);
    nrf_write_port(1, AP_NRF_RESET, 0);
    nrf_port_selection(0);
}

uint8_t nrfswd::nrf_write_bank(uint32_t address, uint32_t buffer[], int size) {
    if (!isConnected)
        return 1;  // not connected to an nRF

    if (size > 4096)
        return 2;  // buffer bigger then a bank

    uint32_t temp;

    write_register(0x4001e504, 1);  // NVIC Enable writing
    long timeout = millis();
    while (read_register(0x4001e400) != 1) {
        if (millis() - timeout > 100) return 3;
    }

    AP_Write(AP_CSW, 0xa2000012);
    AP_Write(AP_TAR, address);

    for (int posi = 0; posi < size; posi += 4) {
        long end_micros = micros() + 400;  // wait till writing of nRF memory is done without asking for ready state
        AP_Write(AP_DRW, buffer[posi / 4]);
        while (micros() < end_micros) {
        };
    }

    AP_Write(AP_CSW, 0xa2000002);
    DP_Read(DP_RDBUFF, temp);
    DP_Read(DP_RDBUFF, temp);

    write_register(0x4001e504, 0);  // NVIC Diasble writing
    timeout = millis();
    while (read_register(0x4001e400) != 1) {
        if (millis() - timeout > 100) return 3;
    }

    return 0;
}
uint8_t nrfswd::nrf_read_bank(uint32_t address, uint32_t buffer[], int size) {
    if (!isConnected)
        return 1;  // not connected to an nRF

    uint32_t temp;

    AP_Write(AP_CSW, 0xa2000012);
    AP_Write(AP_TAR, address);
    AP_Read(AP_DRW, temp);

    uint32_t curr_word = 0;
    for (int posi = 0; posi < size; posi += 4) {
        AP_Read(AP_DRW, curr_word);
        buffer[posi / 4] = curr_word;
    }

    AP_Write(AP_CSW, 0xa2000002);
    DP_Read(DP_RDBUFF, temp);
    DP_Read(DP_RDBUFF, temp);

    return 0;
}