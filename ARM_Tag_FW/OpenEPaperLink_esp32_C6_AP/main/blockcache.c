#include "blockcache.h"

#include <stdlib.h>
#include <string.h>

#include "../../../oepl-radio-v2.h"
#include "esp_heap_caps.h"
#include "esp_log.h"
#include "proto.h"

static const char *TAG = "CACHE";

// A burst sends whole parts, so the last part reads up to here
#define CACHE_BUF_SIZE    (BLOCK_MAX_PARTS * BLOCK_PART_DATA_SIZE)
#define CACHE_MAX_BLOCKS  64
#define CACHE_HEAP_RESERVE (48 * 1024)  // left free for the rest of the firmware

_Static_assert(CACHE_BUF_SIZE >= BLOCK_XFER_BUFFER_SIZE, "a block and its header fit");
_Static_assert((BLOCK_DATA_SIZE + V2_CHUNK_MAX - 1) / V2_CHUNK_MAX < 32, "chunk bitmap");

struct cacheSlot {
    uint8_t *buf;
    uint64_t ver;
    uint32_t chunks;   // bit n: chunk n is in buf
    uint32_t lastUse;  // useClock at the last chunk or use
    uint16_t len;      // data bytes in this block
    uint8_t blockId;
    bool used;
    bool complete;
};

static struct cacheSlot slots[CACHE_MAX_BLOCKS];
static uint8_t slotCount = 0;
static uint8_t firstBuf[CACHE_BUF_SIZE];  // so there is always one, whatever the heap holds
static uint32_t useClock = 0;
static int pinned = -1;

void cacheInit(void) {
    slots[0].buf = firstBuf;
    slotCount = 1;
    while (slotCount < CACHE_MAX_BLOCKS && heap_caps_get_free_size(MALLOC_CAP_8BIT) > CACHE_HEAP_RESERVE + CACHE_BUF_SIZE) {
        uint8_t *buf = malloc(CACHE_BUF_SIZE);
        if (buf == NULL) break;
        slots[slotCount++].buf = buf;
    }
    ESP_LOGW(TAG, "%d blocks, %lu bytes heap left", slotCount, (unsigned long) heap_caps_get_free_size(MALLOC_CAP_8BIT));
}

uint16_t cacheKb(void) { return slotCount * (BLOCK_DATA_SIZE / 1024); }

uint16_t cacheUsedKb(void) {
    uint16_t used = 0;
    for (uint8_t c = 0; c < slotCount; c++) {
        if (slots[c].used) used++;
    }
    return used * (BLOCK_DATA_SIZE / 1024);
}

int cacheFind(uint64_t ver, uint8_t blockId) {
    for (uint8_t c = 0; c < slotCount; c++) {
        if (slots[c].used && slots[c].ver == ver && slots[c].blockId == blockId) return c;
    }
    return -1;
}

bool cacheComplete(int slot) { return slot >= 0 && slot < slotCount && slots[slot].used && slots[slot].complete; }

const uint8_t *cacheBuffer(int slot) { return slots[slot].buf; }

void cacheTouch(int slot) {
    if (slot >= 0 && slot < slotCount) slots[slot].lastUse = ++useClock;
}

// A free slot, or the least recently used one that is not pinned
static int cacheClaim(void) {
    int lru = -1;
    for (uint8_t c = 0; c < slotCount; c++) {
        if (!slots[c].used) return c;
        if (c != pinned && (lru == -1 || (int32_t) (slots[c].lastUse - slots[lru].lastUse) < 0)) lru = c;
    }
    if (lru != -1) ESP_LOGI(TAG, "evicted block %d of %016llX", slots[lru].blockId, slots[lru].ver);
    return lru;
}

int cacheChunk(uint64_t ver, uint32_t offset, uint32_t totalSize, const uint8_t *bytes, size_t n) {
    const uint32_t blockStart = offset - offset % BLOCK_DATA_SIZE;
    const uint32_t pos        = offset - blockStart;
    if (blockStart / BLOCK_DATA_SIZE > 0xFF || pos % V2_CHUNK_MAX || n == 0 || totalSize <= offset || n > totalSize - offset ||
        pos + n > BLOCK_DATA_SIZE) {
        return CACHE_BAD_CHUNK;
    }
    uint32_t len = totalSize - blockStart;
    if (len > BLOCK_DATA_SIZE) len = BLOCK_DATA_SIZE;

    const uint8_t blockId = blockStart / BLOCK_DATA_SIZE;
    int slot = cacheFind(ver, blockId);
    if (slot >= 0 && slots[slot].len != len) {
        if (slot == pinned) return CACHE_BAD_CHUNK;
        slots[slot].used = false;  // another size for the same data: start the block again
        slot = -1;
    }
    if (slot < 0) {
        slot = cacheClaim();
        if (slot < 0) return CACHE_FULL;
        struct cacheSlot *s = &slots[slot];
        s->ver      = ver;
        s->blockId  = blockId;
        s->len      = len;
        s->chunks   = 0;
        s->complete = false;
        s->used     = true;
    }
    struct cacheSlot *s = &slots[slot];
    s->lastUse          = ++useClock;
    if (s->complete) return slot;  // a retransmission, or the S3 sending a block twice

    struct blockData *bd = (struct blockData *) s->buf;
    memcpy(bd->data + pos, bytes, n);
    s->chunks |= 1UL << (pos / V2_CHUNK_MAX);
    const uint32_t needed = (1UL << ((len + V2_CHUNK_MAX - 1) / V2_CHUNK_MAX)) - 1;
    if ((s->chunks & needed) != needed) return slot;

    // complete: the blockData header the tag checks, and 0xFF after the data
    uint16_t sum = 0;
    for (uint16_t c = 0; c < len; c++) sum += bd->data[c];
    bd->size     = len;
    bd->checksum = sum;
    memset(bd->data + len, 0xFF, CACHE_BUF_SIZE - sizeof(struct blockData) - len);
    s->complete = true;
    return slot;
}

void cachePin(int slot) { pinned = slot; }

void cacheDrop(int slot) {
    if (slot >= 0 && slot < slotCount && slot != pinned) slots[slot].used = false;
}

void cacheDropVer(uint64_t ver) {
    for (uint8_t c = 0; c < slotCount; c++) {
        if (slots[c].used && slots[c].ver == ver) cacheDrop(c);
    }
}
