#include <stdint.h>
#include "tl_common.h"
#include "main.h"
#include "zigbee.h"
#include "proto.h"
#include "comms.h"
#include "board.h"
#include "powermgt.h"
#include "eeprom.h"
#include "tl_common.h"

uint32_t eepromGetSize(void)
{
    return EEPROM_IMG_LEN;
}

void eepromRead(uint32_t addr, uint8_t *dstP, uint32_t len)
{
    printf("Eeprom read %X Len: %d\r\n", addr, len);
    flash_read_page(addr, len, dstP);
}

bool eepromWrite(uint32_t addr, uint8_t *srcP, uint32_t len)
{
    printf("Eeprom write %X Len: %d\r\n", addr, len);
    flash_write_page(addr, len, srcP);
    return true;
}

bool eepromErase(uint32_t addr, uint32_t len)
{
    printf("Eeprom erase %X Len: %d\r\n", addr, len);
    // round starting address down
    if (addr % EEPROM_PAGE_SIZE)
    {
        len += addr % EEPROM_PAGE_SIZE;
        addr = addr / EEPROM_PAGE_SIZE * EEPROM_PAGE_SIZE;
    }
    // round length up
    len = (len + EEPROM_PAGE_SIZE - 1) / EEPROM_PAGE_SIZE * EEPROM_PAGE_SIZE;

    while (len)
    {
        uint32_t now;
        flash_erase_sector(addr);
        now = 0x1000;

        addr += now;
        len -= now;
    }
    return true;
}
