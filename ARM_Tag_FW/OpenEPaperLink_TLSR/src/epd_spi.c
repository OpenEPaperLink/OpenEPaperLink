#include <stdint.h>
#include "tl_common.h"
#include "main.h"
#include "epd.h"
#include "epd_spi.h"

 void EPD_init(void)
{
    gpio_set_func(EPD_RESET, AS_GPIO);
    gpio_set_output_en(EPD_RESET, 1);
    gpio_setup_up_down_resistor(EPD_RESET, PM_PIN_PULLUP_1M);

    gpio_set_func(EPD_DC, AS_GPIO);
    gpio_set_output_en(EPD_DC, 1);
    gpio_setup_up_down_resistor(EPD_DC, PM_PIN_PULLUP_1M);

    gpio_set_func(EPD_BUSY, AS_GPIO);
    gpio_set_output_en(EPD_BUSY, 0);
    gpio_set_input_en(EPD_BUSY, 1);
    gpio_setup_up_down_resistor(EPD_BUSY, PM_PIN_PULLUP_1M);

    gpio_set_func(EPD_CS, AS_GPIO);
    gpio_set_output_en(EPD_CS, 1);
    gpio_setup_up_down_resistor(EPD_CS, PM_PIN_PULLUP_1M);

    gpio_set_func(EPD_CLK, AS_GPIO);
    gpio_set_output_en(EPD_CLK, 1);
    gpio_setup_up_down_resistor(EPD_CLK, PM_PIN_PULLUP_1M);

    gpio_set_func(EPD_MOSI, AS_GPIO);
    gpio_set_output_en(EPD_MOSI, 1);
    gpio_setup_up_down_resistor(EPD_MOSI, PM_PIN_PULLUP_1M);

    gpio_set_output_en(EPD_ENABLE, 0);
    gpio_set_input_en(EPD_ENABLE, 1);
    gpio_setup_up_down_resistor(EPD_ENABLE, PM_PIN_PULLUP_1M);
}

 void EPD_SPI_Write(unsigned char value)
{
    unsigned char i;

    WaitUs(10);
    for (i = 0; i < 8; i++)
    {
        gpio_write(EPD_CLK, 0);
        if (value & 0x80)
        {
            gpio_write(EPD_MOSI, 1);
        }
        else
        {
            gpio_write(EPD_MOSI, 0);
        }
        value = (value << 1);
        gpio_write(EPD_CLK, 1);
    }
}

 uint8_t EPD_SPI_read(void)
{
    unsigned char i;
    uint8_t value = 0;

    gpio_shutdown(EPD_MOSI);
    gpio_set_output_en(EPD_MOSI, 0);
    gpio_set_input_en(EPD_MOSI, 1);
    gpio_write(EPD_CS, 0);
    EPD_ENABLE_WRITE_DATA();
    WaitUs(10);
    for (i = 0; i < 8; i++)
    {
        gpio_write(EPD_CLK, 0);
        gpio_write(EPD_CLK, 1);
        value <<= 1;
        if (gpio_read(EPD_MOSI) != 0)
        {
            value |= 1;
        }
    }
    gpio_set_output_en(EPD_MOSI, 1);
    gpio_set_input_en(EPD_MOSI, 0);
    gpio_write(EPD_CS, 1);
    return value;
}

 void EPD_WriteCmd(unsigned char cmd)
{
    gpio_write(EPD_CS, 0);
    EPD_ENABLE_WRITE_CMD();
    EPD_SPI_Write(cmd);
    gpio_write(EPD_CS, 1);
}

 void EPD_WriteData(unsigned char data)
{
    gpio_write(EPD_CS, 0);
    EPD_ENABLE_WRITE_DATA();
    EPD_SPI_Write(data);
    gpio_write(EPD_CS, 1);
}

 void EPD_CheckStatus(int max_ms)
{
    unsigned long timeout_start = clock_time();
    unsigned long timeout_ticks = max_ms * CLOCK_16M_SYS_TIMER_CLK_1MS;
    WaitMs(1);
    while (EPD_IS_BUSY())
    {
        if (clock_time() - timeout_start >= timeout_ticks)
            return; // Here we had a timeout
    }
}

 void EPD_CheckStatus_inverted(int max_ms)
{
    unsigned long timeout_start = clock_time();
    unsigned long timeout_ticks = max_ms * CLOCK_16M_SYS_TIMER_CLK_1MS;
    WaitMs(1);
    while (!EPD_IS_BUSY())
    {
        if (clock_time() - timeout_start >= timeout_ticks)
            return; // Here we had a timeout
    }
}

 void EPD_send_lut(uint8_t lut[], int len)
{
    EPD_WriteCmd(lut[0]);
    for (int r = 1; r <= len; r++)
    {
        EPD_WriteData(lut[r]);
    }
}

 void EPD_send_empty_lut(uint8_t lut, int len)
{
    EPD_WriteCmd(lut);
    for (int r = 0; r <= len; r++)
        EPD_WriteData(0x00);
}

 void EPD_LoadImage(unsigned char *image, int size, uint8_t cmd)
{
    int i;
    EPD_WriteCmd(cmd);
    for (i = 0; i < size; i++)
    {
        EPD_WriteData(image[i]);
    }
    WaitMs(2);
}