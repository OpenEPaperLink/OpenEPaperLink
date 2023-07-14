#include <Arduino.h>
#include "main.h"
#include "epd_spi.h"

void spi_write(uint8_t data)
{
  for (uint8_t i = 0; i < 8; i++)
  {
    digitalWrite(EPD_CLK, LOW);
    if (data & 0x80)
    {
      digitalWrite(EPD_MOSI, HIGH);
    }
    else
    {
      digitalWrite(EPD_MOSI, LOW);
    }
    data = (data << 1);
    digitalWrite(EPD_CLK, HIGH);
  }
}

void epd_cmd(uint8_t data)
{
  digitalWrite(EPD_DC, LOW);
  digitalWrite(EPD_CS, LOW);
  spi_write(data);
  digitalWrite(EPD_CS, HIGH);
}

void epd_data(uint8_t data)
{
  digitalWrite(EPD_DC, HIGH);
  digitalWrite(EPD_CS, LOW);
  spi_write(data);
  digitalWrite(EPD_CS, HIGH);
}

void waitBusy()
{
  while (digitalRead(EPD_BUSY) == HIGH)
  {
  }
}
