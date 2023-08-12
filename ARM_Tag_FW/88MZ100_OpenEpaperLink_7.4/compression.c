
#include "compression.h"
#include "uzlib/src/uzlib.h"
//#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "mz100_flash.h"
#include "mz100_wdt.h"
#include "util.h"
#include "eeprom.h"

#define OUT_CHUNK_SIZE 0x100
struct EepromImageHeader backup_header;

uint8_t out_buffer[OUT_CHUNK_SIZE];
uint8_t dict_buffer[32768];

uint32_t current_in_pos = 0;

int callback_read_more_data(TINF_DATA *d)
{
    return FLASH_ByteRead(FLASH_NORMAL_READ, current_in_pos++);
}

uint32_t decompress_file(uint32_t address_in, uint32_t size_compressed, uint32_t address_temp, uint32_t max_len_temp, char error_reason[100])
{
    FLASH_Read(0, address_in - sizeof(struct EepromImageHeader), (uint8_t *)&backup_header, sizeof(struct EepromImageHeader)); // Save the image header to write it back later
    int res;
    struct uzlib_uncomp d;
    uint32_t decomp_len;
    uint32_t decomp_left;
    uint32_t current_out_pos = 0;
    current_in_pos = address_in;
    FLASH_Read(0, address_in + size_compressed - 4, (uint8_t *)&decomp_len, 4); // Read the expected decompressed len
    printf("Compressed size is: %i\r\n", size_compressed);
    printf("Decompressed size is: %i\r\n", decomp_len);

    if (decomp_len + 1 > max_len_temp)
    {
        printf("Error decompiled file will get too big\r\n");
        sprintf(error_reason, "Out too big %i of max %i bytes", decomp_len, max_len_temp);
        return 0;
    }

    decomp_left = decomp_len + 1;
    uzlib_init();
    uzlib_uncompress_init(&d, dict_buffer, 32768);

    d.source = NULL;
    d.source_limit = 0;
    d.source_read_cb = callback_read_more_data; // Read each byte in the callback to save RAM

    res = uzlib_gzip_parse_header(&d);
    if (res != TINF_OK)
    {
        printf("Error parsing header: %d\r\n", res);
        sprintf(error_reason, "Error parsing header %i", res);
        return 0;
    }
    printf("Header parsed !\r\n");

    qspiEraseRange(address_temp, max_len_temp);
    printf("OTA Area erased\r\n");
    while (decomp_left)
    {
        WDT_RestartCounter();
        unsigned int chunk_len = decomp_left < OUT_CHUNK_SIZE ? decomp_left : OUT_CHUNK_SIZE;
        d.dest = out_buffer;
        d.dest_start = d.dest;
        d.dest_limit = d.dest + sizeof(out_buffer);
        // printf("One round: %i\r\n", decomp_left);
        // printf("chunk_len: %i\r\n", chunk_len);
        res = uzlib_uncompress_chksum(&d);
        // printf("res: %i\r\n", res);
        decomp_left -= chunk_len;
        FLASH_Write(false, address_temp + current_out_pos, out_buffer, chunk_len);
        current_out_pos += chunk_len;

        if (res == TINF_DONE)
        {
            break;
        }
        if (res != TINF_OK)
        {
            printf("Some error inner decomp %i\r\n", res);
            sprintf(error_reason, "Inner decomp error %i", res);
            return 0;
        }
    }
    if (res != TINF_DONE)
    {
        printf("Some error in decomp %i\r\n", res);
        sprintf(error_reason, "Decomp error %i", res);
        return 0;
    }
    printf("Decompression done\r\n");
    printf("Writing image to correct position\r\n");
    qspiEraseRange(address_in, EEPROM_IMG_EACH);
    uint32_t copy_left = decomp_len;
    current_out_pos = 0;
    while (copy_left)
    {
        WDT_RestartCounter();
        unsigned int chunk_len = copy_left < OUT_CHUNK_SIZE ? copy_left : OUT_CHUNK_SIZE;
        FLASH_Read(0, address_temp + current_out_pos, (uint8_t *)&out_buffer, chunk_len);
        FLASH_Write(false, address_in + current_out_pos, out_buffer, chunk_len);
        current_out_pos += chunk_len;
        copy_left -= chunk_len;
    }
    backup_header.size = decomp_len;
    // Writing back the header of an uncompressed image
    FLASH_Write(false, address_in - sizeof(struct EepromImageHeader), (uint8_t *)&backup_header, sizeof(struct EepromImageHeader));
    printf("Everything done\r\n");
    return decomp_len;
}