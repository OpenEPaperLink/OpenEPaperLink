#include <Arduino.h>
#include <stdint.h>

/*
   Copyright (c) 2021 Aaron Christophel ATCnetz.de
   SPDX-License-Identifier: GPL-3.0-or-later
*/
#pragma once

class swd {
   public:
    swd(uint8_t swdio, uint8_t swdclk);

    uint32_t swd_Init();

    bool AP_Write(unsigned addr, uint32_t data);
    bool AP_Read(unsigned addr, uint32_t &data);
    bool DP_Write(unsigned addr, uint32_t data);
    bool DP_Read(unsigned addr, uint32_t &data);

    uint32_t idCode;

   protected:
    void swd_Begin();
    void swd_Direction(bool WorR);
    bool swd_Transfer(unsigned port_address, bool APorDP, bool RorW, uint32_t &data);
    bool calculate_Parity(uint32_t in_data);

    void swd_Write(uint32_t in_data, uint8_t bits);
    uint32_t swd_Read(uint8_t bits);

    uint8_t swdio_pin;
    uint8_t swdclk_pin;
    bool cur_swd_direction = 0;
};

struct nrf_info_struct {
    int flash_size;
    uint32_t connected;
    uint32_t codepage_size;
    uint32_t codesize;
    uint32_t config_id;
    uint32_t device_id0;
    uint32_t device_id1;
    uint32_t info_part;
    uint32_t info_variant;
    uint32_t info_package;
    uint16_t sd_info_area;
    uint32_t ucir_lock;
};

class nrfswd : protected swd {
   public:
    nrfswd(uint8_t swdio, uint8_t swdclk);
    bool init();

    uint8_t nrf_read_bank(uint32_t address, uint32_t buffer[], int size);
    uint8_t nrf_write_bank(uint32_t address, uint32_t buffer[], int size);
    uint8_t nrf_erase_all();
    uint8_t erase_all_flash();
    uint8_t erase_uicr();
    uint8_t erase_page(uint32_t page);
    void nrf_soft_reset();

    bool isConnected = false;
    bool isLocked = false;
    bool showDebug = true;

    struct nrf_info_struct nrf_info;

   protected:
    uint32_t nrf_read_port(bool APorDP, uint8_t address);
    void nrf_write_port(bool APorDP, uint8_t address, uint32_t value);

    void nrf_abort_all();
    void nrf_halt();

    void nrf_port_selection(bool new_port);
    bool nrf_read_lock_state();
    void nrf_read_ufcr();

    void write_register(uint32_t address, uint32_t value);
    uint32_t read_register(uint32_t address);
};