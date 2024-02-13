#define EEPROM_XFER_BLOCKSIZE 512  // shouldn't be any less than 256 bytes probably
#define DRAWITEM_LIST_SIZE 24

struct __attribute__((packed)) imageHeader {
    uint16_t width;
    uint16_t height;
    uint8_t bpp : 4;
    uint8_t reserved : 4;
};

static drawItem *drawItems[DRAWITEM_LIST_SIZE] = {0};

void addBufferedImage(uint16_t x, uint16_t y, bool color, enum rotation ro, const uint8_t *image, bool mask) {
    drawItem *di = new drawItem;

    di->setRotation(ro);
    if (di->direction ^ epd->drawDirectionRight) {
        int16_t temp = x;
        x = y;
        y = temp;
    }

    uint16_t originalWidthBytes = (((uint16_t *)image)[0]) / 8;
    uint16_t size = 0;
    uint16_t width = ((uint16_t *)image)[0];

    // find out if the original data was aligned in one byte; if not, add a byte
    if (((uint16_t *)image)[0] % 8) originalWidthBytes++;

    // if we're drawing in X direction, we shift the content here. Add extra space for shifting!
    if (!di->direction) {
        width += x % 8;
    }

    // check if the size is aligned in bytes; if not, add an extra for good measure;
    if (width % 8) {
        width /= 8;
        width++;
    } else {
        width /= 8;
    }

    size = width * ((uint16_t *)image)[1];
    size += 2;  // not needed

    uint8_t *im = (uint8_t *)calloc(size, 1);

    for (uint16_t copyY = 0; copyY < ((uint16_t *)image)[1]; copyY++) {
        memcpy(im + (copyY * width), image + 4 + (copyY * originalWidthBytes), originalWidthBytes);

        // if we draw in X direction, we need to shift bytes in the array
        if (!di->direction && (x % 8)) {
            drawItem::shiftBytesRight(im + (copyY * width), x % 8, width);
        }
    }

    di->addItem(im, width * 8, ((uint16_t *)image)[1]);

    di->xpos = x;
    di->ypos = y;
    di->color = color;
    if (mask)
        di->type = drawItem::drawType::DRAW_MASK;
    else
        di->type = drawItem::drawType::DRAW_BUFFERED_1BPP;

    di->addToList();
}

void addFlashImage(uint16_t x, uint16_t y, bool color, enum rotation ro, const uint8_t *image) {
    drawItem *di = new drawItem;

    di->setRotation(ro);

    if (di->direction ^ epd->drawDirectionRight) {
        int16_t temp = x;
        x = y;
        y = temp;
    }

    di->addItem((uint8_t *)(image + 4), ((uint16_t *)image)[0], ((uint16_t *)image)[1]);

    di->xpos = x;
    di->ypos = y;
    di->color = color;
    di->cleanUp = false;
    di->type = drawItem::drawType::DRAW_BUFFERED_1BPP;
    di->addToList();
}
#ifdef ENABLE_OEPLFS
void addFSImage(uint16_t x, uint16_t y, uint8_t color, enum rotation ro, char *name) {
    drawItem *di = new drawItem;

    di->setRotation(ro);

    if (di->direction ^ epd->drawDirectionRight) {
        int16_t temp = x;
        x = y;
        y = temp;
    }

    OEPLFile *file = fs->getFile(name);
    if (!file) {
        delete di;
        return;
    }
    uint16_t width, height;

    file->getBlock(0, (uint8_t *)&width, 2);
    file->getBlock(2, (uint8_t *)&height, 2);

    di->addItem((uint8_t *)file, width, height);
    di->xpos = x;
    di->ypos = y;
    di->color = color;
    di->cleanUp = true;
    if (color == 2) {
        di->type = drawItem::drawType::DRAW_OEPLFS_2BPP;
    } else {
        di->type = drawItem::drawType::DRAW_OEPLFS_1BPP;
    }
    di->addToList();
}

void addCompressedFSImage(uint16_t x, uint16_t y, enum rotation ro, char *name) {
    drawItem *di = new drawItem;
    di->type = drawItem::drawType::DRAW_COMPRESSED;

    di->setRotation(ro);

    if (di->direction ^ epd->drawDirectionRight) {
        int16_t temp = x;
        x = y;
        y = temp;
    }

    decompress *decomp = new decompress;
    if (!decomp->openFromFile(name)) {
        delete di;
        delete decomp;
        return;
    }

    di->imageHeaderOffset = decomp->readByte(0);

    struct imageHeader imgheader;
    decomp->getBlock(1, (uint8_t *)&imgheader, sizeof(struct imageHeader));

    di->addItem((uint8_t *)decomp, imgheader.width, imgheader.height);

    di->xpos = x;
    di->ypos = y;
    di->color = imgheader.bpp;
    if (di->color == 1) di->color = 0;
    di->cleanUp = true;
    di->addToList();
}
#endif

void addQR(uint16_t x, uint16_t y, uint8_t version, uint8_t scale, const char *c, ...) {
    char out_buffer[256];
    va_list lst;
    va_start(lst, c);
    vsnprintf(out_buffer, 255, c, lst);
    va_end(lst);

    QRCode qrcode;
    // Allocate a chunk of memory to store the QR code
    uint8_t qrcodeBytes[qrcode_getBufferSize(version)];
    qrcode_initText(&qrcode, qrcodeBytes, version, ECC_LOW, out_buffer);

    drawItem *di = new drawItem;
    di->setRotation(rotation::ROTATE_0);

    uint8_t xbytes = (qrcode.size * scale) / 8;
    if (qrcode.size % 8) xbytes++;

    uint16_t size = xbytes * (qrcode.size * scale);
    uint8_t *im = (uint8_t *)calloc(size + 1, 1);

    uint8_t *qrbuf = im;

    for (uint8_t qry = 0; qry < qrcode.size; qry++) {
        for (uint8_t scale_y = 0; scale_y < scale; scale_y++) {
            for (uint8_t qrx = 0; qrx < qrcode.size; qrx++) {
                for (uint8_t scale_x = 0; scale_x < scale; scale_x++) {
                    if (qrcode_getModule(&qrcode, qrx, qry)) {
                        // Calculate the position in the framebuffer for the scaled pixel
                        uint8_t scaled_qrx = (qrx * scale) + scale_x;
                        uint8_t scaled_qry = (qry * scale) + scale_y;

                        // Calculate the byte and bit positions in the framebuffer
                        uint8_t fb_byte = scaled_qrx / 8;
                        uint8_t fb_bit = 7 - (scaled_qrx % 8);

                        // Set the bit in the framebuffer
                        qrbuf[fb_byte + (scaled_qry * xbytes)] |= (1 << fb_bit);
                    }
                }
            }
        }
    }

    di->addItem(im, xbytes * 8, qrcode.size * scale);

    di->xpos = x;
    di->ypos = y;
    di->color = 0;
    di->type = drawItem::drawType::DRAW_BUFFERED_1BPP;
    di->addToList();
}

void drawImageAtAddressWrap(uint32_t addr, uint8_t lut) {
    // powerUp(INIT_EEPROM);
    // epdSetup();
    selectLUT(lut);
    struct EepromImageHeader eih;
    HAL_flashRead(addr, (uint8_t *)&eih, sizeof(struct EepromImageHeader));
#ifdef DEBUG_DRAWING
    printf("Drawing image of type 0x%02X from location 0x%08X\n", eih.dataType, addr);
#endif
    switch (eih.dataType) {
        case DATATYPE_IMG_RAW_1BPP: {
            drawItem *di = new drawItem;
            // di->setRotation(ro);
            di->xpos = 0;
            di->ypos = 0;
            di->color = 0;
            di->addItem((uint8_t *)addr, epd->effectiveXRes, epd->effectiveYRes);
            di->type = drawItem::drawType::DRAW_EEPROM_1BPP;
            di->direction = false;
            if (di->mirrorH) di->mirrorV = !di->mirrorV;
            di->cleanUp = false;
            di->addToList();
        } break;
        case DATATYPE_IMG_RAW_2BPP: {
            drawItem *di = new drawItem;
            // di->setRotation(ro);
            di->xpos = 0;
            di->ypos = 0;
            di->color = 0;
            di->addItem((uint8_t *)addr, epd->effectiveXRes, epd->effectiveYRes);
            di->type = drawItem::drawType::DRAW_EEPROM_2BPP;
            di->direction = false;
            if (di->mirrorH) di->mirrorV = !di->mirrorV;
            di->cleanUp = false;
            di->addToList();
        } break;
        case DATATYPE_IMG_ZLIB: {
#ifdef DEBUG_DRAWING
            printf("DRAW: drawing compressed image\n");
#endif
            drawItem *di = new drawItem;
            decompress *decomp = new decompress;
            di->type = drawItem::drawType::DRAW_COMPRESSED;

            addr += sizeof(struct EepromImageHeader);

            if (!decomp->openFromFlash(addr, eih.size)) {
                printf("DRAW: failed to open\n");
                delete di;
                delete decomp;
                return;
            }

            di->imageHeaderOffset = decomp->readByte(0);

            struct imageHeader imgheader;
            decomp->getBlock(1, (uint8_t *)&imgheader, sizeof(struct imageHeader));

            di->addItem((uint8_t *)decomp, imgheader.width, imgheader.height);

            di->xpos = 0;
            di->ypos = 0;
            di->direction = false;
            if (di->mirrorH) di->mirrorV = !di->mirrorV;
            if (imgheader.bpp == 1) di->color = 0;
            if (imgheader.bpp == 2) di->color = 2;
            di->cleanUp = true;
            di->addToList();
        } break;
    }
    addOverlay();

    draw();
}

extern "C" {
void drawImageAtAddress(uint32_t addr, uint8_t lut) {
    drawImageAtAddressWrap(addr, lut);
}
}

void drawRoundedRectangle(uint16_t xpos, uint16_t ypos, uint16_t width, uint16_t height, bool color) {
    uint16_t widthBytes = width / 8;
    if (width % 8) widthBytes++;
    uint32_t framebufferSize = (widthBytes + 1) * height;
    uint8_t *framebuffer = (uint8_t *)calloc(framebufferSize + 4, 1);
    if (framebuffer == NULL) {
        return;
    }

    ((uint16_t *)framebuffer)[0] = width + 1;
    ((uint16_t *)framebuffer)[1] = height;

    framebuffer += 4;

    uint16_t w = width - 1;
    uint16_t x = 1;
    while (w--) {
        framebuffer[(x / 8)] |= (uint8_t)(1 << (7 - ((uint8_t)x % 8)));
        x++;
    }
    for (uint16_t curY = 1; curY < (height - 1); curY++) {
        framebuffer[widthBytes * curY] = 0x80;
        if (width % 8)
            framebuffer[(widthBytes * curY) + widthBytes - 1] = (uint8_t)(1 << (7 - ((uint8_t)width % 8)));
        else
            framebuffer[(widthBytes * curY) + widthBytes - 1] = 0x01;
    }
    w = width - 1;
    x = 1;
    while (w--) {
        framebuffer[(x / 8) + ((height - 1) * widthBytes)] |= (uint8_t)(1 << (7 - ((uint8_t)x % 8)));
        x++;
    }
    framebuffer -= 4;
    addBufferedImage(xpos, ypos, color, rotation::ROTATE_0, framebuffer, DRAW_NORMAL);
    free(framebuffer);
}

void drawMask(uint16_t xpos, uint16_t ypos, uint16_t width, uint16_t height, bool color) {
    uint16_t widthBytes = width / 8;
    if (width % 8) widthBytes++;
    uint32_t framebufferSize = widthBytes * height;
    uint8_t *framebuffer = (uint8_t *)calloc(framebufferSize + 4, 1);
    if (framebuffer == NULL) {
        return;
    }

    ((uint16_t *)framebuffer)[0] = width;
    ((uint16_t *)framebuffer)[1] = height;

    framebuffer += 4;

    for (uint16_t curY = 0; curY < height; curY++) {
        uint16_t w = width;
        uint16_t x = 0;
        while (w--) {
            framebuffer[(x / 8) + (curY * widthBytes)] |= (uint8_t)(1 << (7 - ((uint8_t)x % 8)));
            x++;
        }
    }

    framebuffer -= 4;
    addBufferedImage(xpos, ypos, color, rotation::ROTATE_0, framebuffer, DRAW_INVERTED);
    free(framebuffer);
}

// drawItem (sprite) functions
void drawItem::shiftBytesRight(uint8_t *data, uint8_t shift, uint8_t len) {
    // Ensure the shift value is within bounds (0 to 7)
    shift = shift % 8;

    // Handle the case where shift is 0 or len is 0
    if (shift == 0 || len == 0) {
        return;
    }

    // Loop through the array from right to left
    for (int i = len - 1; i > 0; i--) {
        // Perform the shift by combining bits from the current byte
        // and the next byte to its right
        data[i] = (data[i] >> shift) | (data[i - 1] << (8 - shift));
    }

    // For the leftmost byte, simply shift it to the right
    data[0] >>= shift;
}

uint8_t drawItem::bitReverse(uint8_t byte) {
    byte = ((byte >> 1) & 0x55) | ((byte << 1) & 0xAA);
    byte = ((byte >> 2) & 0x33) | ((byte << 2) & 0xCC);
    byte = (byte >> 4) | (byte << 4);
    return byte;
}

void drawItem::reverseBytes(uint8_t *src, uint8_t src_len) {
    // Check for valid input
    if (src == NULL || src_len == 0) {
        return;
    }

    // Reverse the entire source array
    for (uint8_t i = 0; i < src_len / 2; i++) {
        uint8_t temp = src[i];
        src[i] = src[src_len - i - 1];
        src[src_len - i - 1] = temp;
    }
    // Reverse the bits within the bytes
    for (uint8_t i = 0; i < src_len; i++) {
        src[i] = bitReverse(src[i]);
    }
}

void drawItem::copyWithByteShift(uint8_t *dst, uint8_t *src, uint8_t src_len, uint8_t offset) {
    switch (type) {
        case DRAW_MASK:
            for (uint8_t i = 0; i < src_len; i++) {
                dst[i + offset] &= ~(src[i]);
            }
            break;
        default:
            for (uint8_t i = 0; i < src_len; i++) {
                dst[i + offset] |= src[i];
            }
            break;
    }
}

void drawItem::renderDrawLine(uint8_t *line, uint16_t number, uint8_t c) {
    drawItem *curDrawItem;
    for (uint8_t i = 0; i < DRAWITEM_LIST_SIZE; i++) {
        curDrawItem = drawItems[i];
        if (curDrawItem != nullptr) {
            curDrawItem->getDrawLine(line, number, c);
        }
    }
}

void drawItem::flushDrawItems() {
    drawItem *curDrawItem;
    for (uint8_t i = 0; i < DRAWITEM_LIST_SIZE; i++) {
        curDrawItem = drawItems[i];
        if (curDrawItem != nullptr) {
            delete curDrawItem;
            drawItems[i] = nullptr;
        }
    }
}

void drawItem::getXLine(uint8_t *line, uint16_t y, uint8_t c) {
    switch (type) {
        case DRAW_FONT:
        case DRAW_BUFFERED_1BPP:
        case DRAW_MASK:
            if (c != color) return;
            if ((y >= ypos) && (y < height + ypos)) {  // was y > ypos, not >=
                // y = height-y;
                if (mirrorV) {
                    if (mirrorH) {
                        reverseBytes(&buffer[((height - (y - ypos)) * widthBytes)], widthBytes);
                        // reverseBytes(&buffer[((y - ypos) * widthBytes)], widthBytes);
                    } else {
                        reverseBytes(&buffer[((y - ypos) * widthBytes)], widthBytes);
                    }
                }
                if (mirrorH) {
                    copyWithByteShift(line, &buffer[((height - (y - ypos)) * widthBytes)], widthBytes, xpos / 8);
                } else {
                    copyWithByteShift(line, &buffer[((y - ypos) * widthBytes)], widthBytes, xpos / 8);
                }
            }
            break;
#ifdef ENABLE_OEPLFS
        case DRAW_OEPLFS_1BPP:
        case DRAW_OEPLFS_2BPP:
            if ((color < 2) && (c != color)) return;
            if ((y >= ypos) && (y < height + ypos)) {  // was y > ypos, not >=
                uint32_t offset = 4;
                offset += c * height * widthBytes;
                OEPLFile *file = (OEPLFile *)this->buffer;
                uint8_t *dbuffer = (uint8_t *)malloc(widthBytes);
                if (mirrorH) {
                    file->getBlock(offset + (height - (y - ypos)) * widthBytes, dbuffer, widthBytes);
                } else {
                    file->getBlock(offset + (y - ypos) * widthBytes, dbuffer, widthBytes);
                }
                if (mirrorV) {
                    reverseBytes(dbuffer, widthBytes);
                }
                copyWithByteShift(line, dbuffer, widthBytes, xpos / 8);
                free(dbuffer);
            }
            break;
#endif
        case DRAW_COMPRESSED:
            if ((color < 2) && (c != color)) return;
            if ((y >= ypos) && (y < height + ypos)) {
                uint32_t offset = this->imageHeaderOffset;
                offset += c * height * widthBytes;
                decompress *decomp = (decompress *)this->buffer;
                uint8_t *dbuffer = (uint8_t *)malloc(widthBytes);
                if (mirrorH) {
                    decomp->getBlock(offset + (height - (y - ypos)) * widthBytes, dbuffer, widthBytes);
                } else {
                    decomp->getBlock(offset + (y - ypos) * widthBytes, dbuffer, widthBytes);
                }
                if (mirrorV) {
                    reverseBytes(dbuffer, widthBytes);
                }
                copyWithByteShift(line, dbuffer, widthBytes, xpos / 8);
                free(dbuffer);
            }
            break;
        case DRAW_EEPROM_1BPP:
            if (c != color) return;
            if (epd->drawDirectionRight)
                y = epd->effectiveYRes - 1 - y;
            HAL_flashRead((uint32_t)buffer + sizeof(struct EepromImageHeader) + (y * (epd->effectiveXRes / 8)), line, (epd->effectiveXRes / 8));
            break;
        case DRAW_EEPROM_2BPP:
            if (epd->drawDirectionRight)
                y = epd->effectiveYRes - 1 - y;
            HAL_flashRead((uint32_t)(buffer + sizeof(struct EepromImageHeader) + ((y + (c * epd->effectiveYRes)) * (epd->effectiveXRes / 8))), line, (epd->effectiveXRes / 8));
            break;
        default:
            printf("DRAW: Not supported mode!\n");
            break;
    }
}

void drawItem::getYLine(uint8_t *line, uint16_t x, uint8_t c) {
    switch (type) {
        case DRAW_FONT:
        case DRAW_BUFFERED_1BPP:
            if (c != color) return;
            if ((x >= xpos) && (x < width + xpos)) {
                x -= xpos;
                for (uint16_t curY = 0; curY < height; curY++) {
                    uint16_t curYMirrored = curY;
                    if (!mirrorH) curYMirrored = height - 1 - curY;
                    if (mirrorV) {
                        if (buffer[((width - x) / 8) + (curYMirrored * widthBytes)] & (1 << (7 - ((width - x) % 8)))) {
                            line[(curY + ypos) / 8] |= (1 << (7 - ((curY + ypos) % 8)));
                        }
                    } else {
                        if (buffer[(x / 8) + (curYMirrored * widthBytes)] & (1 << (7 - (x % 8)))) {
                            line[(curY + ypos) / 8] |= (1 << (7 - ((curY + ypos) % 8)));
                        }
                    }
                }
            }
            break;
        case DRAW_MASK:
            if (c != color) return;
            if ((x >= xpos) && (x < width + xpos)) {
                x -= xpos;
                for (uint16_t curY = 0; curY < height; curY++) {
                    uint16_t curYMirrored = curY;
                    if (!mirrorH) curYMirrored = height - 1 - curY;
                    if (mirrorV) {
                        if (buffer[((width - x) / 8) + (curYMirrored * widthBytes)] & (1 << (7 - ((width - x) % 8)))) {
                            line[(curY + ypos) / 8] &= ~(1 << (7 - ((curY + ypos) % 8)));
                        }
                    } else {
                        if (buffer[(x / 8) + (curYMirrored * widthBytes)] & (1 << (7 - (x % 8)))) {
                            line[(curY + ypos) / 8] &= ~(1 << (7 - ((curY + ypos) % 8)));
                        }
                    }
                }
            }
            break;
#ifdef ENABLE_OEPLFS
        case DRAW_OEPLFS_1BPP:
        case DRAW_OEPLFS_2BPP:
            // this is incredibly slow. For larger images, it'll probably read 128 bytes of eeprom for every -bit- in the image.
            if ((color < 2) && (c != color)) return;
            if ((x >= xpos) && (x < width + xpos)) {
                uint32_t offset = 4;
                offset += c * height * widthBytes;
                OEPLFile *file = (OEPLFile *)this->buffer;
                x -= xpos;
                for (uint16_t curY = 0; curY < height; curY++) {
                    uint16_t curYMirrored = curY;
                    if (!mirrorH) curYMirrored = height - 1 - curY;
                    if (mirrorV) {
                        if (file->readByte(offset + ((width - x) / 8) + (curYMirrored * widthBytes)) & (1 << (7 - ((width - x) % 8)))) {
                            line[(curY + ypos) / 8] |= (1 << (7 - ((curY + ypos) % 8)));
                        }
                    } else {
                        if (file->readByte(offset + (x / 8) + (curYMirrored * widthBytes)) & (1 << (7 - (x % 8)))) {
                            line[(curY + ypos) / 8] |= (1 << (7 - ((curY + ypos) % 8)));
                        }
                    }
                }
            }
            break;
#endif
        case DRAW_COMPRESSED:
            // this is incredibly slow, and very naive. Just don't. Maybe if you want to test if everything works. It'll decompress about 50% of the file *PER PIXEL*. Good for load testing
            if ((color < 2) && (c != color)) return;
            if ((x >= xpos) && (x < width + xpos)) {
                uint32_t offset = this->imageHeaderOffset;
                offset += c * height * widthBytes;
                decompress *decomp = (decompress *)this->buffer;
                x -= xpos;
                for (uint16_t curY = 0; curY < height; curY++) {
                    uint16_t curYMirrored = curY;
                    if (!mirrorH) curYMirrored = height - 1 - curY;
                    if (mirrorV) {
                        if (decomp->readByte(offset + ((width - x) / 8) + (curYMirrored * widthBytes)) & (1 << (7 - ((width - x) % 8)))) {
                            line[(curY + ypos) / 8] |= (1 << (7 - ((curY + ypos) % 8)));
                        }
                    } else {
                        if (decomp->readByte(offset + (x / 8) + (curYMirrored * widthBytes)) & (1 << (7 - (x % 8)))) {
                            line[(curY + ypos) / 8] |= (1 << (7 - ((curY + ypos) % 8)));
                        }
                    }
                }
            }
            break;

        default:
            break;
    }
}

void drawItem::getDrawLine(uint8_t *line, uint16_t number, uint8_t c) {
    if (direction) {
        getYLine(line, number, c);
    } else {
        getXLine(line, number, c);
    }
}

void drawItem::addItem(uint8_t *data, uint16_t w, uint16_t h) {
    width = w;
    height = h;
    widthBytes = w / 8;
    if (w % 8) widthBytes++;
    buffer = data;
}

bool drawItem::addToList() {
    for (uint8_t i = 0; i < DRAWITEM_LIST_SIZE; i++) {
        if (drawItems[i] == nullptr) {
            drawItems[i] = this;
            return true;
        };
    }
    return false;
}

drawItem::~drawItem() {
    if (cleanUp) {
        switch (this->type) {
#ifdef ENABLE_OEPLFS
            case drawItem::drawType::DRAW_OEPLFS_1BPP:
            case drawItem::drawType::DRAW_OEPLFS_2BPP: {
                OEPLFile *file = (OEPLFile *)this->buffer;
                if (file) delete file;
            } break;
#endif
            case drawItem::drawType::DRAW_COMPRESSED: {
                decompress *dec = (decompress *)this->buffer;
                if (dec) delete dec;
            } break;
            default:
                free(buffer);
                break;
        }
    }
}

drawItem::drawItem() {
    if (epd->drawDirectionRight) {
        direction = true;
        mirrorH = true;
    }
}

void drawItem::setRotation(enum rotation ro) {
    if (epd->drawDirectionRight) {
        direction = true;
        mirrorH = true;
    }

    switch (ro) {
        case ROTATE_0:
            break;
        case ROTATE_270:
            direction = !direction;
            mirrorH = !mirrorH;
            mirrorV = !mirrorV;
            break;
        case ROTATE_180:
            mirrorH = !mirrorH;
            mirrorV = !mirrorV;
            break;
        case ROTATE_90:
            direction = !direction;
            break;
    };
}

void fontrender::drawFastHLine(uint16_t x, uint16_t y, uint16_t w) {
    while (w--) {
        fb[(x / 8) + (y * bufferByteWidth)] |= (uint8_t)(1 << (7 - ((uint8_t)x % 8)));
        x++;
    }
}

void fontrender::fillRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h) {
    for (uint16_t curY = y; curY < y + h; curY++) {
        drawFastHLine(x, curY, w);
    }
}

#ifdef ENABLE_OEPLFS
// font rendering functions for OEPLFS
fontrender::fontrender(char *name) {
    this->setFont(name);
}

void fontrender::setFont(char *name) {
    if (this->glyphFile) delete this->glyphFile;
    if (this->bitmapFile) delete this->bitmapFile;
    OEPLFile *font = fs->getFile(name);
    if (!font) {
#ifdef DEBUG_DRAWING
        printf("DRAW: Couldn't open font file %s\n", name);
#endif
    }
    font->getBlock(0, (uint8_t *)&this->gfxFont, sizeof(GFXFontOEPL));
    this->glyphFile = fs->getFile(this->gfxFont.glyphFile);
    this->bitmapFile = fs->getFile(this->gfxFont.bitmapFile);

    if (!this->glyphFile) printf("DRAW: Couldn't open font glyph file %s\n", name);
    if (!this->bitmapFile) printf("DRAW: Couldn't open font bitmap file %s\n", name);
    if (font) delete (font);
}

fontrender::~fontrender() {
    if (this->glyphFile) delete this->glyphFile;
    if (this->bitmapFile) delete this->bitmapFile;
}

uint8_t fontrender::getCharWidth(uint16_t c) {
    if ((c >= gfxFont.first) && (c <= gfxFont.last)) {
        c -= gfxFont.first;
        GFXglyph glyph = this->getGlyph(c);
        return glyph.xAdvance;
    }
    return 0;
}

GFXglyph fontrender::getGlyph(uint16_t c) {
    GFXglyph g;
    this->glyphFile->getBlock(sizeof(GFXglyph) * c, (uint8_t *)&g, sizeof(GFXglyph));
    return g;
}

#else

// 'regular' font rendering functions
uint8_t fontrender::getCharWidth(uint16_t c) {
    if ((c >= gfxFont->first) && (c <= gfxFont->last)) {
        c -= gfxFont->first;
        GFXglyph *glyph = &(gfxFont->glyph[c]);
        return glyph->xAdvance;
    }
    return 0;
}

fontrender::fontrender(const GFXfont *font) {
    gfxFont = (GFXfont *)font;
}

void fontrender::setFont(const GFXfont *font) {
    gfxFont = (GFXfont *)font;
}

#endif

uint8_t fontrender::drawChar(int32_t x, int32_t y, uint16_t c, uint8_t size) {
    // Filter out bad characters not present in font

#ifdef ENABLE_OEPLFS
    if ((c >= gfxFont.first) && (c <= gfxFont.last)) {
        c -= gfxFont.first;
        GFXglyph glyph = this->getGlyph(c);
        // uint8_t *bitmap = (uint8_t *)gfxFont->bitmap;
        uint32_t bo = glyph.bitmapOffset;
        uint8_t w = glyph.width,
                h = glyph.height;
        int8_t xo = glyph.xOffset,
               yo = glyph.yOffset;
#else
    if ((c >= gfxFont->first) && (c <= gfxFont->last)) {
        c -= gfxFont->first;
        GFXglyph *glyph = &(gfxFont->glyph[c]);
        uint8_t *bitmap = (uint8_t *)gfxFont->bitmap;
        uint32_t bo = glyph->bitmapOffset;
        uint8_t w = glyph->width,
                h = glyph->height;
        int8_t xo = glyph->xOffset,
               yo = glyph->yOffset;
#endif

        uint8_t xx, yy, bits = 0, bit = 0;
        int16_t xo16 = 0, yo16 = 0;

        if (size > 1) {
            xo16 = xo;
            yo16 = yo;
        }

        // GFXFF rendering speed up
        uint16_t hpc = 0;  // Horizontal foreground pixel count
        for (yy = 0; yy < h; yy++) {
            for (xx = 0; xx < w; xx++) {
                if (bit == 0) {
#ifdef ENABLE_OEPLFS
                    bits = (*this->bitmapFile)(bo++);
#else
                    bits = bitmap[bo++];
#endif

                    bit = 0x80;
                }
                if (bits & bit)
                    hpc++;
                else {
                    if (hpc) {
                        if (size == 1)
                            drawFastHLine(x + xo + xx - hpc, y + yo + yy, hpc);
                        else
                            fillRect(x + (xo16 + xx - hpc) * size, y + (yo16 + yy) * size, size * hpc, size);
                        hpc = 0;
                    }
                }
                bit >>= 1;
            }
            // Draw pixels for this line as we are about to increment yy
            if (hpc) {
                if (size == 1)
                    drawFastHLine(x + xo + xx - hpc, y + yo + yy, hpc);
                else
                    fillRect(x + (xo16 + xx - hpc) * size, y + (yo16 + yy) * size, size * hpc, size);
                hpc = 0;
            }
        }
#ifdef ENABLE_OEPLFS
        return glyph.xAdvance;
#else
        return glyph->xAdvance;
#endif
    }
    return 0;
}

void fontrender::epdPrintf(uint16_t x, uint16_t y, bool color, enum rotation ro, const char *c, ...) {
    drawItem *di = new drawItem;
    if (di == nullptr) return;
    di->setRotation(ro);

    // prepare a drawItem, exchange x/y if necessary.
    if (di->direction ^ epd->drawDirectionRight) {
        int16_t temp = x;
        x = y;
        y = temp;
    }

    // output string using vsnprintf
    char out_buffer[256];
    va_list lst;
    va_start(lst, c);
    uint8_t len = vsnprintf(out_buffer, 255, c, lst);
    va_end(lst);

    // account for offset in font rendering
    if (!di->direction) {
        Xpixels = x % 8;  // total drawing width increased by x%8
    } else {
        Xpixels = 0;
    }

    // find out the total length of the string
    for (uint8_t c = 0; c < len; c++) {
        Xpixels += (uint16_t)getCharWidth(out_buffer[c]);
    }

    // find out the high and low points for given font
    int8_t high = 0;
    int8_t low = 0;
    for (uint8_t curchar = 0; curchar < len; curchar++) {
        uint8_t c = out_buffer[curchar];

#ifdef ENABLE_OEPLFS
        if ((c >= gfxFont.first) && (c <= gfxFont.last)) {
            c -= gfxFont.first;
            GFXglyph g = this->getGlyph(c);
            int8_t glyphUL = g.yOffset;
            if (glyphUL < high) high = glyphUL;
            int8_t glyphHeight = g.height;
#else
        if ((c >= gfxFont->first) && (c <= gfxFont->last)) {
            c -= gfxFont->first;
            int8_t glyphUL = gfxFont->glyph[c].yOffset;
            if (glyphUL < high) high = glyphUL;
            int8_t glyphHeight = gfxFont->glyph[c].height;
#endif

            if ((glyphUL + glyphHeight) > low) low = glyphUL + glyphHeight;
        }
    }
    // Actual font height (reduces memory footprint)
    int8_t height = -1 * (high - low) + 1;

    // determine actual width
    bufferByteWidth = Xpixels / 8;
    if (Xpixels % 8) bufferByteWidth++;

    // allocate framebuffer
    fb = (uint8_t *)calloc(bufferByteWidth * height, 1);

    if (!fb) {
        printf("DRAW: Tried to allocate a buffer %d x %d\n and failed...", bufferByteWidth, height);
        delete di;
        return;
    }

    uint16_t curX;
    if (!di->direction) {
        curX = x % 8;  // start drawing at x%8s
    } else {
        curX = 0;
    }
    for (uint8_t c = 0; c < len; c++) {
        curX += (uint16_t)drawChar(curX, height - low, out_buffer[c], 1);
    }

    di->addItem(fb, curX, height);
    di->ypos = y;
    di->xpos = x;
    di->color = color;
    di->type = drawItem::drawType::DRAW_FONT;
    di->addToList();
}
