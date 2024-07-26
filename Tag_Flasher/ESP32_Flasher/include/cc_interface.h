#pragma once
#include <Arduino.h>

typedef void (*callbackPtr)(uint8_t percent);

class CC_interface
{
  public:
    uint16_t begin(uint8_t CC, uint8_t DD, uint8_t RESET);
    void set_callback(callbackPtr callBack = nullptr);
    uint8_t set_lock_byte(uint8_t lock_byte);
    uint8_t erase_chip();
    void read_code_memory(uint16_t address, uint16_t len, uint8_t buffer[]);
    void read_xdata_memory(uint16_t address, uint16_t len, uint8_t buffer[]);
    void write_xdata_memory(uint16_t address, uint16_t len, uint8_t buffer[]);
    void set_pc(uint16_t address);
    uint8_t clock_init();
    uint8_t write_code_memory(uint16_t address, uint8_t buffer[], int len);
    uint8_t verify_code_memory(uint16_t address, uint8_t buffer[], int len);
    uint8_t opcode(uint8_t opCode);
    uint8_t opcode(uint8_t opCode, uint8_t opCode1);
    uint8_t opcode(uint8_t opCode, uint8_t opCode1, uint8_t opCode2);
    uint8_t WR_CONFIG(uint8_t config);
    uint8_t WD_CONFIG();
    /* Send one byte and return one byte as answer */
    uint8_t send_cc_cmdS(uint8_t cmd);
    /* Send one byte and returns two bytes as answer */
    uint16_t send_cc_cmd(uint8_t cmd);
    void cc_send_byte(uint8_t in_byte);
    uint8_t cc_receive_byte();
    void enable_cc_debug();
    void reset_cc();

  private:
    boolean dd_direction = 0; // 0=OUT 1=IN
    uint8_t _CC_PIN = -1;
    uint8_t _DD_PIN = -1;
    uint8_t _RESET_PIN = -1;
    uint8_t flash_opcode[30] = {
      0x75, 0xAD, 0x00, // Set Flash Address HIGH
      0x75, 0xAC, 0x00, // Set Flash Address LOW
      0x90, 0xF0, 0x00, // Set RAM Address to 0xF000
      0x75, 0xAE, 0x02, // Enable Flash Writing
      0x7D, 0x08, // Set Loop value to Half size as we write 2bytes at once 0x100
      0xE0, // Put DPTR to A
      0xF5, 0xAF, // Put A to FWDATA
      0xA3, // Increase DPTR;
      0xE0, // Put DPTR to A
      0xF5, 0xAF, // Put A to FWDATA
      0xA3, // Increase DPTR
      0xE5, 0xAE, // Wait till writing is done
      0x20, 0xE6, 0xFB,
      0xDD, 0xF1, // Loop trough all bytes
      0xA5 // Breakpoint
    };
    callbackPtr _callback = nullptr;
};

