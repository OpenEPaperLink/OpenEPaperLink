#include "gzipfile.h"

#include "rom/miniz.h"

// Skip the gzip header (RFC 1952). Returns the offset of the deflate data, or 0 if the header is invalid.
static size_t gzipDataOffset(const uint8_t *data, const size_t size) {
    if (size < 18 || data[0] != 0x1F || data[1] != 0x8B || data[2] != 8) return 0;
    const uint8_t flags = data[3];
    size_t offset = 10;
    if (flags & 0x04) {  // FEXTRA
        if (offset + 2 > size) return 0;
        offset += 2 + (data[offset] | (data[offset + 1] << 8));
    }
    if (flags & 0x08) {  // FNAME
        while (offset < size && data[offset++] != 0);
    }
    if (flags & 0x10) {  // FCOMMENT
        while (offset < size && data[offset++] != 0);
    }
    if (flags & 0x02) offset += 2;  // FHCRC
    return offset + 8 <= size ? offset : 0;
}

uint8_t *readGzipFile(fs::FS &fs, const String &path, size_t &size) {
    size = 0;
    File file = fs.open(path, "r");
    if (!file) return nullptr;
    const size_t gzSize = file.size();
    uint8_t *gzData = (uint8_t *)malloc(gzSize);
    if (gzData == nullptr) {
        file.close();
        return nullptr;
    }
    const size_t read = file.read(gzData, gzSize);
    file.close();

    const size_t offset = read == gzSize ? gzipDataOffset(gzData, gzSize) : 0;
    if (offset == 0) {
        free(gzData);
        return nullptr;
    }
    // the last 4 bytes hold the decompressed size
    const size_t outSize = gzData[gzSize - 4] | (gzData[gzSize - 3] << 8) | (gzData[gzSize - 2] << 16) | ((uint32_t)gzData[gzSize - 1] << 24);
    uint8_t *out = (uint8_t *)malloc(outSize);
    // the decompressor state is about 11 KB: keep it off the stack
    tinfl_decompressor *decomp = (tinfl_decompressor *)malloc(sizeof(tinfl_decompressor));
    if (out == nullptr || decomp == nullptr) {
        free(out);
        free(decomp);
        free(gzData);
        return nullptr;
    }

    tinfl_init(decomp);
    size_t inBytes = gzSize - offset - 8;
    size_t outBytes = outSize;
    const tinfl_status status = tinfl_decompress(decomp, gzData + offset, &inBytes, out, out, &outBytes, TINFL_FLAG_USING_NON_WRAPPING_OUTPUT_BUF);
    free(decomp);
    free(gzData);
    if (status != TINFL_STATUS_DONE || outBytes != outSize) {
        free(out);
        return nullptr;
    }
    size = outSize;
    return out;
}
