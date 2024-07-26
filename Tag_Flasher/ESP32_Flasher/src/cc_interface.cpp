#include <Arduino.h>
#include "cc_interface.h"

uint16_t CC_interface::begin(uint8_t CC, uint8_t DD, uint8_t RESET)
{
  _CC_PIN = CC;
  _DD_PIN = DD;
  _RESET_PIN = RESET;

  pinMode(_CC_PIN, OUTPUT);
  pinMode(_DD_PIN, OUTPUT);
  pinMode(_RESET_PIN, OUTPUT);
  digitalWrite(_CC_PIN, LOW);
  digitalWrite(_DD_PIN, HIGH);
  digitalWrite(_RESET_PIN, HIGH);

  enable_cc_debug();
  uint16_t device_id_answer = send_cc_cmd(0x68);
  opcode(0x00); // NOP
  clock_init(); // Even with failed clock init return the device id, because if the device is locked setting clock will fail
  return device_id_answer;
}

void CC_interface::set_callback(callbackPtr callBack)
{
  _callback = callBack;
}

uint8_t CC_interface::set_lock_byte(uint8_t lock_byte)
{
  lock_byte = lock_byte & 0x1f; // Max lock byte value
  WR_CONFIG(0x01);              // Select flash info Page
  opcode(0x00);                 // NOP

  opcode(0xE5, 0x92);
  opcode(0x75, 0x92, 0x00);
  opcode(0xE5, 0x83);
  opcode(0xE5, 0x82);
  opcode(0x90, 0xF0, 0x00);
  opcode(0x74, 0xFF);
  opcode(0xF0);
  opcode(0xA3);            // Increase Pointer
  opcode(0x74, lock_byte); // Transmit the set lock byte
  opcode(0xF0);
  opcode(0xA3); // Increase Pointer
  opcode(0x90, 0x00, 0x00);
  opcode(0x75, 0x92, 0x00);
  opcode(0x74, 0x00);

  opcode(0x00); // NOP

  opcode(0xE5, 0x92);
  opcode(0x75, 0x92, 0x00);
  opcode(0xE5, 0x83);
  opcode(0xE5, 0x82);
  opcode(0x90, 0xF8, 0x00);
  opcode(0x74, 0xF0);
  opcode(0xF0);
  opcode(0xA3); // Increase Pointer
  opcode(0x74, 0x00);
  opcode(0xF0);
  opcode(0xA3); // Increase Pointer
  opcode(0x74, 0xDF);
  opcode(0xF0);
  opcode(0xA3); // Increase Pointer
  opcode(0x74, 0xAF);
  opcode(0xF0);
  opcode(0xA3); // Increase Pointer
  opcode(0x74, 0x00);
  opcode(0xF0);
  opcode(0xA3); // Increase Pointer
  opcode(0x74, 0x02);
  opcode(0xF0);
  opcode(0xA3); // Increase Pointer
  opcode(0x74, 0x12);
  opcode(0xF0);
  opcode(0xA3); // Increase Pointer
  opcode(0x74, 0x4A);
  opcode(0xF0);
  opcode(0xA3); // Increase Pointer
  opcode(0x90, 0x00, 0x00);
  opcode(0x75, 0x92, 0x00);
  opcode(0x74, 0x00);

  opcode(0x00); // NOP

  opcode(0xE5, 0xC6);
  opcode(0x74, 0x00);
  opcode(0x75, 0xAB, 0x23);
  opcode(0x75, 0xD5, 0xF8);
  opcode(0x75, 0xD4, 0x00);
  opcode(0x75, 0xD6, 0x01);
  opcode(0x75, 0xAD, 0x00);
  opcode(0x75, 0xAC, 0x00);
  opcode(0x75, 0xAE, 0x02);

  opcode(0x00); // NOP

  opcode(0xE5, 0xAE);
  opcode(0x74, 0x00);

  return WR_CONFIG(0x00); // Select normal flash page
}

uint8_t CC_interface::erase_chip()
{
  opcode(0x00); // NOP
  send_cc_cmdS(0x14);
  int timeout = millis() + 100;
  while (!(send_cc_cmdS(0x34) & 0x80))
  {
    if (millis() > timeout)
    {
      return 1;
    }
  }
  return 0;
}

void CC_interface::read_code_memory(uint16_t address, uint16_t len, uint8_t buffer[])
{
  int last_callback = 0;
  opcode(0x75, 0xc7, 0x01);
  opcode(0x90, address >> 8, address);
  for (int i = 0; i < len; i++)
  {
    opcode(0xe4);
    buffer[i] = opcode(0x93);
    opcode(0xa3);
    if (i - last_callback > 100)
    {
      last_callback = i;
      if (_callback != nullptr)
      {
        uint8_t percent = ((float)((float)i / (float)len) * 100.0);
        if (percent < 0)
          percent = 0;
        if (percent > 100)
          percent = 100;
        _callback(percent);
      }
    }
  }
  if (_callback != nullptr)
    _callback(100);
}

void CC_interface::read_xdata_memory(uint16_t address, uint16_t len, uint8_t buffer[])
{
  opcode(0x90, address >> 8, address);
  for (int i = 0; i < len; i++)
  {
    buffer[i] = opcode(0xe0);
    opcode(0xa3);
  }
}

void CC_interface::write_xdata_memory(uint16_t address, uint16_t len, uint8_t buffer[])
{
  opcode(0x90, address >> 8, address);
  for (int i = 0; i < len; i++)
  {
    opcode(0x74, buffer[i]);
    opcode(0xf0);
    opcode(0xa3);
  }
}

void CC_interface::set_pc(uint16_t address)
{
  opcode(0x02, address >> 8, address);
}

uint8_t CC_interface::clock_init()
{
  opcode(0x75, 0xc6, 0x00);
  int timeout = millis() + 100;
  while (!(opcode(0xe5, 0xbe) & 0x40))
  {
    if (millis() > timeout)
    {
      return 1;
    }
  }
  return 0;
}

uint8_t CC_interface::write_code_memory(uint16_t address, uint8_t buffer[], int len)
{
  int entry_len = len;
  if (len % 2 != 0)
    len++;
  int position = 0;
  int len_per_transfer = 64;
  address = address / 2;
  while (len)
  {
    flash_opcode[2] = (address >> 8) & 0xff;
    flash_opcode[5] = address & 0xff;
    flash_opcode[13] = (len > len_per_transfer) ? (len_per_transfer / 2) : (len / 2);
    write_xdata_memory(0xf000, len_per_transfer, &buffer[position]);
    write_xdata_memory(0xf100, sizeof(flash_opcode), flash_opcode);
    opcode(0x75, 0xC7, 0x51);
    set_pc(0xf100);
    send_cc_cmdS(0x4c);
    int timeout = millis() + 500;
    while (!(send_cc_cmdS(0x34) & 0x08))
    {
      if (millis() > timeout)
      {
        if (_callback != nullptr)
          _callback(0);
        return 1;
      }
    }
    if (_callback != nullptr)
    {
      uint8_t percent = 100 - ((float)((float)len / (float)entry_len) * 100.0);
      if (percent < 0)
        percent = 0;
      if (percent > 100)
        percent = 100;
      _callback(percent);
    }
    len -= flash_opcode[13] * 2;
    position += flash_opcode[13] * 2;
    address += flash_opcode[13];
  }
  if (_callback != nullptr)
    _callback(100);
  return 0;
}

uint8_t CC_interface::verify_code_memory(uint16_t address, uint8_t buffer[], int len)
{
  int last_callback = 0;
  opcode(0x75, 0xc7, 0x01);
  opcode(0x90, address >> 8, address);
  for (int i = 0; i < len; i++)
  {
    opcode(0xe4);
    if (buffer[i] != opcode(0x93))
    {
      if (_callback != nullptr)
        _callback(0);
      return 1;
    }
    opcode(0xa3);
    if (i - last_callback > 100)
    {
      last_callback = i;
      if (_callback != nullptr)
      {
        uint8_t percent = ((float)((float)i / (float)len) * 100.0);
        if (percent < 0)
          percent = 0;
        if (percent > 100)
          percent = 100;
        _callback(percent);
      }
    }
  }
  if (_callback != nullptr)
    _callback(100);
  return 0;
}

uint8_t CC_interface::opcode(uint8_t opCode)
{
  cc_send_byte(0x55);
  cc_send_byte(opCode);
  return cc_receive_byte();
}

uint8_t CC_interface::opcode(uint8_t opCode, uint8_t opCode1)
{
  cc_send_byte(0x56);
  cc_send_byte(opCode);
  cc_send_byte(opCode1);
  return cc_receive_byte();
}

uint8_t CC_interface::opcode(uint8_t opCode, uint8_t opCode1, uint8_t opCode2)
{
  cc_send_byte(0x57);
  cc_send_byte(opCode);
  cc_send_byte(opCode1);
  cc_send_byte(opCode2);
  return cc_receive_byte();
}

uint8_t CC_interface::WR_CONFIG(uint8_t config)
{
  cc_send_byte(0x1d);
  cc_send_byte(config);
  return cc_receive_byte();
}

uint8_t CC_interface::WD_CONFIG()
{
  cc_send_byte(0x24);
  return cc_receive_byte();
}

uint8_t CC_interface::send_cc_cmdS(uint8_t cmd)
{
  cc_send_byte(cmd);
  return cc_receive_byte();
}

uint16_t CC_interface::send_cc_cmd(uint8_t cmd)
{
  cc_send_byte(cmd);
  return (cc_receive_byte() << 8) + cc_receive_byte();
}

void CC_interface::cc_send_byte(uint8_t in_byte)
{
  if (dd_direction == 1)
  {
    dd_direction = 0;
    pinMode(_DD_PIN, OUTPUT);
    digitalWrite(_DD_PIN, LOW);
  }
  for (int i = 8; i; i--)
  {
    if (in_byte & 0x80)
      digitalWrite(_DD_PIN, HIGH);
    else
      digitalWrite(_DD_PIN, LOW);

    digitalWrite(_CC_PIN, HIGH);
    in_byte <<= 1;
    delayMicroseconds(5);
    digitalWrite(_CC_PIN, LOW);
  }
}

uint8_t CC_interface::cc_receive_byte()
{
  uint8_t out_byte = 0x00;
  if (dd_direction == 0)
  {
    dd_direction = 1;
    pinMode(_DD_PIN, INPUT);
    digitalWrite(_DD_PIN, LOW);
  }
  for (int i = 8; i; i--)
  {
    digitalWrite(_CC_PIN, HIGH);
    delayMicroseconds(5);
    out_byte <<= 1;
    if (digitalRead(_DD_PIN))
      out_byte |= 0x01;
    digitalWrite(_CC_PIN, LOW);
    delayMicroseconds(5);
  }
  return out_byte;
}

void CC_interface::enable_cc_debug()
{
  if (dd_direction == 0)
  {
    dd_direction = 1;
    pinMode(_DD_PIN, INPUT);
    digitalWrite(_DD_PIN, HIGH);
  }
  delay(5);
  digitalWrite(_RESET_PIN, LOW);
  delay(2);
  digitalWrite(_CC_PIN, HIGH);
  delayMicroseconds(5);
  digitalWrite(_CC_PIN, LOW);
  delayMicroseconds(5);
  digitalWrite(_CC_PIN, HIGH);
  delayMicroseconds(5);
  digitalWrite(_CC_PIN, LOW);
  delay(2);
  digitalWrite(_RESET_PIN, HIGH);
  delay(2);
}

void CC_interface::reset_cc()
{
  if (dd_direction == 0)
  {
    dd_direction = 1;
    pinMode(_DD_PIN, INPUT);
    digitalWrite(_DD_PIN, HIGH);
  }
  delay(5);
  digitalWrite(_RESET_PIN, LOW);
  delay(5);
  digitalWrite(_RESET_PIN, HIGH);
  delay(2);
}
