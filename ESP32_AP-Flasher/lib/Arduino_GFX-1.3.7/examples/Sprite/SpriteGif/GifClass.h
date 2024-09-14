/*******************************************************************************
 * GIFDEC Wrapper Class
 *
 * Rewrite from: https://github.com/BasementCat/arduino-tft-gif
 ******************************************************************************/
#ifndef _GIFCLASS_H_
#define _GIFCLASS_H_

/* Wio Terminal */
#if defined(ARDUINO_ARCH_SAMD) && defined(SEEED_GROVE_UI_WIRELESS)
#include <Seeed_FS.h>
#elif defined(ESP32) || defined(ESP8266)
#include <FS.h>
#else
#include <SD.h>
#endif

#include <sys/types.h>

#ifndef MIN
#define MIN(A, B) ((A) < (B) ? (A) : (B))
#endif

#ifndef MAX
#define MAX(A, B) ((A) > (B) ? (A) : (B))
#endif

#define GIF_BUF_SIZE 1024

typedef struct gd_Palette
{
    int16_t len;
    uint16_t colors[256];
} gd_Palette;

typedef struct gd_GCE
{
    uint16_t delay;
    uint8_t tindex;
    uint8_t disposal;
    uint8_t input;
    uint8_t transparency;
} gd_GCE;

typedef struct gd_Entry
{
    int32_t len;
    uint16_t prefix;
    uint8_t suffix;
} gd_Entry;

typedef struct gd_Table
{
    int16_t bulk;
    int16_t nentries;
    gd_Entry *entries;
} gd_Table;

typedef struct gd_GIF
{
    File *fd;
    off_t anim_start;
    uint16_t width, height;
    uint16_t depth;
    uint16_t loop_count;
    gd_GCE gce;
    gd_Palette *palette;
    gd_Palette lct, gct;
    void (*plain_text)(
        struct gd_GIF *gif, uint16_t tx, uint16_t ty,
        uint16_t tw, uint16_t th, uint8_t cw, uint8_t ch,
        uint8_t fg, uint8_t bg);
    void (*comment)(struct gd_GIF *gif);
    void (*application)(struct gd_GIF *gif, char id[8], char auth[3]);
    uint16_t fx, fy, fw, fh;
    uint8_t bgindex;
    gd_Table *table;
    bool read_first_frame;
} gd_GIF;

class GifClass
{
public:
    gd_GIF *gd_open_gif(File *fd)
    {
        uint8_t sigver[3];
        uint16_t width, height, depth;
        uint8_t fdsz, bgidx, aspect;
        int16_t gct_sz;
        gd_GIF *gif;

        // init global variables
        gif_buf_last_idx = GIF_BUF_SIZE;
        gif_buf_idx = gif_buf_last_idx; // no buffer yet
        file_pos = 0;

        /* Header */
        gif_buf_read(fd, sigver, 3);
        if (memcmp(sigver, "GIF", 3) != 0)
        {
            Serial.println(F("invalid signature"));
            return NULL;
        }
        /* Version */
        gif_buf_read(fd, sigver, 3);
        if (memcmp(sigver, "89a", 3) != 0)
        {
            Serial.println(F("invalid version"));
            return NULL;
        }
        /* Width x Height */
        width = gif_buf_read16(fd);
        height = gif_buf_read16(fd);
        /* FDSZ */
        gif_buf_read(fd, &fdsz, 1);
        /* Presence of GCT */
        if (!(fdsz & 0x80))
        {
            Serial.println(F("no global color table"));
            return NULL;
        }
        /* Color Space's Depth */
        depth = ((fdsz >> 4) & 7) + 1;
        /* Ignore Sort Flag. */
        /* GCT Size */
        gct_sz = 1 << ((fdsz & 0x07) + 1);
        /* Background Color Index */
        gif_buf_read(fd, &bgidx, 1);
        /* Aspect Ratio */
        gif_buf_read(fd, &aspect, 1);
        /* Create gd_GIF Structure. */
        gif = (gd_GIF *)calloc(1, sizeof(*gif));
        gif->fd = fd;
        gif->width = width;
        gif->height = height;
        gif->depth = depth;
        /* Read GCT */
        read_palette(fd, &gif->gct, gct_sz);
        gif->palette = &gif->gct;
        gif->bgindex = bgidx;
        gif->anim_start = file_pos; // fd->position();
        gif->table = new_table();
        gif->read_first_frame = false;
        return gif;
    }

    /* Return 1 if got a frame; 0 if got GIF trailer; -1 if error. */
    int32_t gd_get_frame(gd_GIF *gif, uint8_t *frame)
    {
        char sep;

        while (1)
        {
            gif_buf_read(gif->fd, (uint8_t *)&sep, 1);
            if (sep == 0)
            {
                gif_buf_read(gif->fd, (uint8_t *)&sep, 1);
            }
            if (sep == ',')
            {
                break;
            }
            if (sep == ';')
            {
                return 0;
            }
            if (sep == '!')
            {
                read_ext(gif);
            }
            else
            {
                Serial.print(F("Read sep: ["));
                Serial.print(sep);
                Serial.println(F("].\n"));
                return -1;
            }
        }
        // Serial.println("Do read image");
        if (read_image(gif, frame) == -1)
            return -1;
        return 1;
    }

    void gd_rewind(gd_GIF *gif)
    {
#if defined(ESP32) || defined(ESP8266)
        gif->fd->seek(gif->anim_start, SeekSet);
#else
        gif->fd->seek(gif->anim_start);
#endif
        file_pos = gif->anim_start;
        gif_buf_idx = gif_buf_last_idx; // reset buffer
    }

    void gd_close_gif(gd_GIF *gif)
    {
        gif->fd->close();
        free(gif->table);
        free(gif);
    }

private:
    bool gif_buf_seek(File *fd, int16_t len)
    {
        if (len > (gif_buf_last_idx - gif_buf_idx))
        {
#if defined(ESP32) || defined(ESP8266)
            // fd->seek(len - (gif_buf_last_idx - gif_buf_idx), SeekCur);
            fd->seek(file_pos + len - (gif_buf_last_idx - gif_buf_idx), SeekSet);
#else
            fd->seek(file_pos + len - (gif_buf_last_idx - gif_buf_idx));
#endif

            gif_buf_idx = gif_buf_last_idx;
        }
        else
        {
            gif_buf_idx += len;
        }
        file_pos += len;

        return true;
    }

    int16_t gif_buf_read(File *fd, uint8_t *dest, int16_t len)
    {
        while (len--)
        {
            if (gif_buf_idx == gif_buf_last_idx)
            {
                gif_buf_last_idx = fd->read(gif_buf, GIF_BUF_SIZE);
                gif_buf_idx = 0;
            }

            file_pos++;
            *(dest++) = gif_buf[gif_buf_idx++];
        }
        return len;
    }

    uint8_t gif_buf_read(File *fd)
    {
        if (gif_buf_idx == gif_buf_last_idx)
        {
            gif_buf_last_idx = fd->read(gif_buf, GIF_BUF_SIZE);
            gif_buf_idx = 0;
        }

        file_pos++;
        return gif_buf[gif_buf_idx++];
    }

    uint16_t gif_buf_read16(File *fd)
    {
        return gif_buf_read(fd) + (((uint16_t)gif_buf_read(fd)) << 8);
    }

    void read_palette(File *fd, gd_Palette *dest, int16_t num_colors)
    {
        uint8_t r, g, b;
        dest->len = num_colors;
        for (int16_t i = 0; i < num_colors; i++)
        {
            r = gif_buf_read(fd);
            g = gif_buf_read(fd);
            b = gif_buf_read(fd);
            dest->colors[i] = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | ((b & 0xF8) >> 3);
        }
    }

    void discard_sub_blocks(gd_GIF *gif)
    {
        uint8_t len;

        do
        {
            gif_buf_read(gif->fd, &len, 1);
            gif_buf_seek(gif->fd, len);
        } while (len);
    }

    void read_plain_text_ext(gd_GIF *gif)
    {
        if (gif->plain_text)
        {
            uint16_t tx, ty, tw, th;
            uint8_t cw, ch, fg, bg;
            gif_buf_seek(gif->fd, 1); /* block size = 12 */
            tx = gif_buf_read16(gif->fd);
            ty = gif_buf_read16(gif->fd);
            tw = gif_buf_read16(gif->fd);
            th = gif_buf_read16(gif->fd);
            cw = gif_buf_read(gif->fd);
            ch = gif_buf_read(gif->fd);
            fg = gif_buf_read(gif->fd);
            bg = gif_buf_read(gif->fd);
            gif->plain_text(gif, tx, ty, tw, th, cw, ch, fg, bg);
        }
        else
        {
            /* Discard plain text metadata. */
            gif_buf_seek(gif->fd, 13);
        }
        /* Discard plain text sub-blocks. */
        discard_sub_blocks(gif);
    }

    void read_graphic_control_ext(gd_GIF *gif)
    {
        uint8_t rdit;

        /* Discard block size (always 0x04). */
        gif_buf_seek(gif->fd, 1);
        gif_buf_read(gif->fd, &rdit, 1);
        gif->gce.disposal = (rdit >> 2) & 3;
        gif->gce.input = rdit & 2;
        gif->gce.transparency = rdit & 1;
        gif->gce.delay = gif_buf_read16(gif->fd);
        gif_buf_read(gif->fd, &gif->gce.tindex, 1);
        /* Skip block terminator. */
        gif_buf_seek(gif->fd, 1);
    }

    void read_comment_ext(gd_GIF *gif)
    {
        if (gif->comment)
        {
            gif->comment(gif);
        }
        /* Discard comment sub-blocks. */
        discard_sub_blocks(gif);
    }

    void read_application_ext(gd_GIF *gif)
    {
        char app_id[8];
        char app_auth_code[3];

        /* Discard block size (always 0x0B). */
        gif_buf_seek(gif->fd, 1);
        /* Application Identifier. */
        gif_buf_read(gif->fd, (uint8_t *)app_id, 8);
        /* Application Authentication Code. */
        gif_buf_read(gif->fd, (uint8_t *)app_auth_code, 3);
        if (!strncmp(app_id, "NETSCAPE", sizeof(app_id)))
        {
            /* Discard block size (0x03) and constant byte (0x01). */
            gif_buf_seek(gif->fd, 2);
            gif->loop_count = gif_buf_read16(gif->fd);
            /* Skip block terminator. */
            gif_buf_seek(gif->fd, 1);
        }
        else if (gif->application)
        {
            gif->application(gif, app_id, app_auth_code);
            discard_sub_blocks(gif);
        }
        else
        {
            discard_sub_blocks(gif);
        }
    }

    void read_ext(gd_GIF *gif)
    {
        uint8_t label;

        gif_buf_read(gif->fd, &label, 1);
        switch (label)
        {
        case 0x01:
            read_plain_text_ext(gif);
            break;
        case 0xF9:
            read_graphic_control_ext(gif);
            break;
        case 0xFE:
            read_comment_ext(gif);
            break;
        case 0xFF:
            read_application_ext(gif);
            break;
        default:
            Serial.print("unknown extension: ");
            Serial.println(label, HEX);
        }
    }

    gd_Table *new_table()
    {
        // uint16_t key;
        // int16_t init_bulk = MAX(1 << (key_size + 1), 0x100);
        // Table *table = (Table*) malloc(sizeof(*table) + sizeof(Entry) * init_bulk);
        // if (table) {
        //     table->bulk = init_bulk;
        //     table->nentries = (1 << key_size) + 2;
        //     table->entries = (Entry *) &table[1];
        //     for (key = 0; key < (1 << key_size); key++)
        //         table->entries[key] = (Entry) {1, 0xFFF, key};
        // }
        // return table;
        int32_t s = sizeof(gd_Table) + (sizeof(gd_Entry) * 4096);
        gd_Table *table = (gd_Table *)malloc(s);
        if (table)
        {
            Serial.print(F("new_table() malloc: "));
            Serial.println(s);
        }
        else
        {
            Serial.print(F("new_table() malloc failed: "));
            Serial.println(s);
        }
        table->entries = (gd_Entry *)&table[1];
        return table;
    }

    void reset_table(gd_Table *table, uint16_t key_size)
    {
        table->nentries = (1 << key_size) + 2;
        for (uint16_t key = 0; key < (1 << key_size); key++)
        {
            table->entries[key] = (gd_Entry){1, 0xFFF, (uint8_t)key};
        }
    }

    /* Add table entry. Return value:
     *  0 on success
     *  +1 if key size must be incremented after this addition
     *  -1 if could not realloc table */
    int32_t add_entry(gd_Table *table, int32_t len, uint16_t prefix, uint8_t suffix)
    {
        // Table *table = *tablep;
        // if (table->nentries == table->bulk) {
        //     table->bulk *= 2;
        //     table = (Table*) realloc(table, sizeof(*table) + sizeof(Entry) * table->bulk);
        //     if (!table) return -1;
        //     table->entries = (Entry *) &table[1];
        //     *tablep = table;
        // }
        table->entries[table->nentries] = (gd_Entry){len, prefix, suffix};
        table->nentries++;
        if ((table->nentries & (table->nentries - 1)) == 0)
            return 1;
        return 0;
    }

    uint16_t get_key(gd_GIF *gif, uint16_t key_size, uint8_t *sub_len, uint8_t *shift, uint8_t *byte)
    {
        int16_t bits_read;
        int16_t rpad;
        int16_t frag_size;
        uint16_t key;

        key = 0;
        for (bits_read = 0; bits_read < key_size; bits_read += frag_size)
        {
            rpad = (*shift + bits_read) % 8;
            if (rpad == 0)
            {
                /* Update byte. */
                if (*sub_len == 0)
                    gif_buf_read(gif->fd, sub_len, 1); /* Must be nonzero! */
                gif_buf_read(gif->fd, byte, 1);
                (*sub_len)--;
            }
            frag_size = MIN(key_size - bits_read, 8 - rpad);
            key |= ((uint16_t)((*byte) >> rpad)) << bits_read;
        }
        /* Clear extra bits to the left. */
        key &= (1 << key_size) - 1;
        *shift = (*shift + key_size) % 8;
        return key;
    }

    /* Compute output index of y-th input line, in frame of height h. */
    int16_t interlaced_line_index(int16_t h, int16_t y)
    {
        int16_t p; /* number of lines in current pass */

        p = (h - 1) / 8 + 1;
        if (y < p) /* pass 1 */
            return y * 8;
        y -= p;
        p = (h - 5) / 8 + 1;
        if (y < p) /* pass 2 */
            return y * 8 + 4;
        y -= p;
        p = (h - 3) / 4 + 1;
        if (y < p) /* pass 3 */
            return y * 4 + 2;
        y -= p;
        /* pass 4 */
        return y * 2 + 1;
    }

    /* Decompress image pixels.
     * Return 0 on success or -1 on out-of-memory (w.r.t. LZW code table). */
    int8_t read_image_data(gd_GIF *gif, int16_t interlace, uint8_t *frame)
    {
        uint8_t sub_len, shift, byte, table_is_full = 0;
        uint16_t init_key_size, key_size;
        int32_t frm_off, str_len = 0, p, x, y;
        uint16_t key, clear, stop;
        int32_t ret;
        gd_Entry entry = {0, 0, 0};

        // Serial.println("Read key size");
        gif_buf_read(gif->fd, &byte, 1);
        key_size = (uint16_t)byte;
        // Serial.println("Set pos, discard sub blocks");
        // start = gif->fd->position();
        // discard_sub_blocks(gif);
        // end = gif->fd->position();
        // gif_buf_seek(gif->fd, start, SeekSet);
        clear = 1 << key_size;
        stop = clear + 1;
        // Serial.println("New LZW table");
        // table = new_table(key_size);
        reset_table(gif->table, key_size);
        key_size++;
        init_key_size = key_size;
        sub_len = shift = 0;
        // Serial.println("Get init key");
        key = get_key(gif, key_size, &sub_len, &shift, &byte); /* clear code */
        frm_off = 0;
        ret = 0;
        while (1)
        {
            if (key == clear)
            {
                // Serial.println("Clear key, reset nentries");
                key_size = init_key_size;
                gif->table->nentries = (1 << (key_size - 1)) + 2;
                table_is_full = 0;
            }
            else if (!table_is_full)
            {
                // Serial.println("Add entry to table");
                ret = add_entry(gif->table, str_len + 1, key, entry.suffix);
                // if (ret == -1) {
                //     // Serial.println("Table entry add failure");
                //     free(table);
                //     return -1;
                // }
                if (gif->table->nentries == 0x1000)
                {
                    // Serial.println("Table is full");
                    ret = 0;
                    table_is_full = 1;
                }
            }
            // Serial.println("Get key");
            key = get_key(gif, key_size, &sub_len, &shift, &byte);
            if (key == clear)
                continue;
            if (key == stop)
                break;
            if (ret == 1)
                key_size++;
            entry = gif->table->entries[key];
            str_len = entry.len;
            uint8_t tindex = gif->gce.tindex;
            // Serial.println("Interpret key");
            while (1)
            {
                p = frm_off + entry.len - 1;
                x = p % gif->fw;
                y = p / gif->fw;
                if (interlace)
                {
                    y = interlaced_line_index((int16_t)gif->fh, y);
                }
                if ((!gif->read_first_frame) || (tindex != entry.suffix))
                {
                    frame[(gif->fy + y) * gif->width + gif->fx + x] = entry.suffix;
                }
                if (entry.prefix == 0xFFF)
                    break;
                else
                    entry = gif->table->entries[entry.prefix];
            }
            frm_off += str_len;
            if (key < gif->table->nentries - 1 && !table_is_full)
                gif->table->entries[gif->table->nentries - 1].suffix = entry.suffix;
        }
        // Serial.println("Done w/ img data, free table and seek to end");
        // free(table);
        gif_buf_read(gif->fd, &sub_len, 1); /* Must be zero! */
        // gif_buf_seek(gif->fd, end, SeekSet);

        gif->read_first_frame = true;

        return 0;
    }

    /* Read image.
     * Return 0 on success or -1 on out-of-memory (w.r.t. LZW code table). */
    int8_t read_image(gd_GIF *gif, uint8_t *frame)
    {
        uint8_t fisrz;
        int16_t interlace;

        /* Image Descriptor. */
        // Serial.println("Read image descriptor");
        gif->fx = gif_buf_read16(gif->fd);
        gif->fy = gif_buf_read16(gif->fd);
        gif->fw = gif_buf_read16(gif->fd);
        gif->fh = gif_buf_read16(gif->fd);
        // Serial.println("Read fisrz?");
        gif_buf_read(gif->fd, &fisrz, 1);
        interlace = fisrz & 0x40;
        /* Ignore Sort Flag. */
        /* Local Color Table? */
        if (fisrz & 0x80)
        {
            /* Read LCT */
            // Serial.println("Read LCT");
            read_palette(gif->fd, &gif->lct, 1 << ((fisrz & 0x07) + 1));
            gif->palette = &gif->lct;
        }
        else
        {
            gif->palette = &gif->gct;
        }
        /* Image Data. */
        // Serial.println("Read image data");
        return read_image_data(gif, interlace, frame);
    }

    void render_frame_rect(gd_GIF *gif, uint16_t *buffer, uint8_t *frame)
    {
        int16_t i, j, k;
        uint8_t index;
        i = gif->fy * gif->width + gif->fx;
        for (j = 0; j < gif->fh; j++)
        {
            for (k = 0; k < gif->fw; k++)
            {
                index = frame[(gif->fy + j) * gif->width + gif->fx + k];
                // color = &gif->palette->colors[index*2];
                if ((!gif->gce.transparency) || (index != gif->gce.tindex))
                {
                    buffer[(i + k)] = gif->palette->colors[index];
                }
                // memcpy(&buffer[(i+k)*2], color, 2);
            }
            i += gif->width;
        }
    }

    int16_t gif_buf_last_idx, gif_buf_idx, file_pos;
    uint8_t gif_buf[GIF_BUF_SIZE];
};

#endif /* _GIFCLASS_H_ */
