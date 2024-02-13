// #include <vector>

std::vector<decompress *> decompContexts;

int decompCallback(TINF_DATA *d) {
    // in the callback for uzlib we'll get a uzlib context from which it originates
    for (uint8_t i = 0; i < decompContexts.size(); i++) {
        // see if we can find which decompression context that callback belongs to
        if (decompContexts.at(i)->ctx == d) {
            decompress *dec = decompContexts.at(i);
#ifdef ENABLE_OEPLFS
            if (dec->fromFile)
                return dec->getNextCompressedBlockFromFile();
            else
#endif
                return dec->getNextCompressedBlockFromFlash();
        }
    }
    printf("FS: Couldn't find callback...\n");
    return -1;
}

decompress::decompress() {
    decompContexts.push_back(this);
}

void decompress::seek(uint32_t address) {
}

bool decompress::readHeader() {
    uzlib_init();

    // read the window size from the zlib header
    int res = uzlib_zlib_parse_header(this->ctx);
    if (res < 0) {
        printf("FS: Invalid zlib header\n");
        return false;
    }

    // the window size is reported as 2^(x-8).
    uint16_t window = 0x100 << res;

    // check if the file served has a sensible window size
    if (window > MAX_WINDOW_SIZE) {
        printf("FS: Asked to decompress a file with a specified window size of %d, I don't see that happening\n", window);
        return false;
    } else {
        //printf("FS: Opened compressed file with dictionary size %d\n", window);
    }

    window = 8192;

    // allocate dict/window if not already allocated
    if (!this->dictionary) this->dictionary = (uint8_t *)malloc(window);
    if (!this->dictionary) printf("FS: window malloc failed\n");

    uzlib_uncompress_init(this->ctx, this->dictionary, window);
    return true;
}

#ifdef ENABLE_OEPLFS
bool decompress::openFromFile(char *filename) {
    this->file = fs->getFile(filename);
    if (!this->file) {
        return false;
    }
    this->setupContext();
    this->ctx->source_read_cb = decompCallback;
    this->file->getBlock(4, this->compBuffer, ZLIB_CACHE_SIZE);
    this->compressedPos += ZLIB_CACHE_SIZE;
    this->compressedSize = file->length - 4;
    this->file->getBlock(0, (uint8_t *)&this->decompressedSize, 4);
    this->fromFile = true;
    return this->readHeader();
}
#endif

bool decompress::openFromFlash(uint32_t eepBase, uint32_t cSize) {
    this->setupContext();
    this->ctx->source_read_cb = decompCallback;
    this->compressedPos += ZLIB_CACHE_SIZE;
    this->compressedSize = cSize - 4;
    this->eepromBase = eepBase;
    HAL_flashRead(this->eepromBase, (uint8_t *)&this->decompressedSize, 4);
    HAL_flashRead(this->eepromBase + 4, this->compBuffer, ZLIB_CACHE_SIZE);
    this->fromFile = false;
    return this->readHeader();
}

void decompress::setupContext() {
    if (!this->ctx) this->ctx = new struct uzlib_uncomp;
    if (!this->compBuffer) this->compBuffer = (uint8_t *)malloc(ZLIB_CACHE_SIZE);
    this->ctx->source = this->compBuffer;
    this->ctx->source_limit = this->compBuffer + ZLIB_CACHE_SIZE;
    compressedPos = 0;
    decompressedPos = 0;
}

decompress::~decompress() {
    for (uint8_t i = 0; i < decompContexts.size(); i++) {
        if (decompContexts.at(i) == this)
            decompContexts.erase(decompContexts.begin() + i);
    }
    if (this->dictionary) free(this->dictionary);
    if (this->ctx) delete this->ctx;
    if (this->compBuffer) free(this->compBuffer);
}

#ifdef ENABLE_OEPLFS
int decompress::getNextCompressedBlockFromFile() {
    int32_t bytesLeft = compressedSize - compressedPos;
    if (bytesLeft <= 0) return -1;
    if (bytesLeft > ZLIB_CACHE_SIZE) bytesLeft = ZLIB_CACHE_SIZE;
    this->file->getBlock(4 + compressedPos, this->compBuffer, ZLIB_CACHE_SIZE);
    ctx->source = this->compBuffer + 1;
    ctx->source_limit = this->compBuffer + bytesLeft;
    this->compressedPos += ZLIB_CACHE_SIZE;
    return this->compBuffer[0];
}
#endif

int decompress::getNextCompressedBlockFromFlash() {
    int32_t bytesLeft = compressedSize - compressedPos;
    if (bytesLeft <= 0) return -1;
    if (bytesLeft > ZLIB_CACHE_SIZE) bytesLeft = ZLIB_CACHE_SIZE;
    HAL_flashRead(this->eepromBase + 4 + compressedPos, this->compBuffer, bytesLeft);
    ctx->source = this->compBuffer + 1;
    ctx->source_limit = this->compBuffer + bytesLeft;
    this->compressedPos += bytesLeft;
    return this->compBuffer[0];
}

uint32_t decompress::getBlock(uint32_t address, uint8_t *target, uint32_t len) {
    if (address + len > decompressedSize) return 0;
    if (address < this->decompressedPos) {
        // reload file, start from scratch
        this->ctx->source = this->compBuffer;
        compressedPos = 0;
        decompressedPos = 0;
#ifdef ENABLE_OEPLFS
        if (this->fromFile)
            this->getNextCompressedBlockFromFile();
        else
#endif
            this->getNextCompressedBlockFromFlash();
        this->ctx->source = this->compBuffer;
        this->readHeader();
    }

    // skip to the next part of the output stream

    if (address != decompressedPos) {
        uint8_t temp[ZLIB_CACHE_SIZE];
        while (this->decompressedPos < address) {
            uint32_t readBytes = address - decompressedPos;
            if (readBytes > ZLIB_CACHE_SIZE) readBytes = ZLIB_CACHE_SIZE;
            decompressedPos += readBytes;
            this->ctx->dest = temp;
            ctx->dest_start = ctx->dest;
            ctx->dest_limit = ctx->dest + readBytes;
            uzlib_uncompress(ctx);
        }
    }
    uint32_t bytesLeft = this->decompressedSize - this->decompressedPos;
    if (len > bytesLeft) len = bytesLeft;

    this->ctx->dest = (unsigned char *)target;
    ctx->dest_start = ctx->dest;
    ctx->dest_limit = ctx->dest + len;

    uzlib_uncompress(ctx);
    this->decompressedPos += len;
    return len;
}

uint8_t decompress::readByte(uint32_t address) {
    uint8_t a;
    this->getBlock(address, &a, 1);
    return a;
}
