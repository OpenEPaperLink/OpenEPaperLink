#pragma once

// Block cache: 4 KB blocks of tag data, keyed by data version and block number, filled by the S3
// with DATA_CHUNKs (protocol v2). A block request for a complete block is served without asking
// the S3. Least recently used blocks make room for new ones; the block a burst reads is pinned.
// Everything runs in the main loop.

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define CACHE_FULL      (-1)  // no slot free and none to evict
#define CACHE_BAD_CHUNK (-2)  // chunk outside its block, or conflicting with what is cached

void cacheInit(void);  // after all other allocations: takes the free heap minus a reserve
uint16_t cacheKb(void);
uint16_t cacheUsedKb(void);

int cacheFind(uint64_t ver, uint8_t blockId);  // slot, or -1
bool cacheComplete(int slot);
// The block as the tag gets it: struct blockData (size, checksum) and the data, 0xFF-padded to
// BLOCK_MAX_PARTS * BLOCK_PART_DATA_SIZE bytes
const uint8_t *cacheBuffer(int slot);
void cacheTouch(int slot);

// Stores one chunk; returns the slot it went into, or CACHE_FULL / CACHE_BAD_CHUNK
int cacheChunk(uint64_t ver, uint32_t offset, uint32_t totalSize, const uint8_t *bytes, size_t n);

void cachePin(int slot);  // -1 to unpin; one pinned slot at a time
void cacheDrop(int slot);
void cacheDropVer(uint64_t ver);  // all blocks of this data, except a pinned one
