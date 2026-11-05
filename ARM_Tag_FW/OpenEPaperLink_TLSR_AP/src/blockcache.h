#pragma once

// Block cache: blocks of tag data (up to 4 KB each), keyed by data version and block number, filled by
// the S3 with DATA_CHUNKs (protocol v2). A block request for a complete block is served without asking
// the S3. Everything runs in the main loop.
//
// Each block is stored at its own length plus 24 bytes (the tag's 4-byte blockData header and the
// cache's 20-byte slot) rather than in a fixed 4 KB region, since most blocks are much smaller.
//
// Two transfers at once may not both fit, so the cache has to choose what to evict. It does not
// choose by age: a tag walks its image from block 0 upwards, so the oldest block of an image is the
// one its tag needs next. The cache is told what each tag is asking for (cacheRequested) and what has
// gone out to one (cacheServed), and evicts the block whose tag is least likely to ask for it next -
// see cacheClaim.
//
// The blocks and the tag store (ap.c: allow-list, pending entries) share one arena,
// partitioned at run time. The store grows from the low end, the blocks are packed against the high
// end, and when the store needs bytes the blocks hold, the cache gives way through its eviction
// policy. The store never waits for room: an allow-list overflow costs the whole inventory lock until
// the link restarts, while a cache miss costs one tag a round trip to the S3 (counted as cev/cevu).
//
// The arena is whatever SRAM the build leaves (halArena; placed by boot_8258_ap.link), and every
// maximum is derived from its size once, at boot. See the RAM section of README.md.

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// The cache's floor, in full blocks (4120 bytes each, slot included): the arena the cache keeps
// whatever the store holds, and so also what bounds the store's capacity. 7 guarantees one transfer
// of a three-colour image (six blocks raw, about four as transferred). The host tests override it to
// exercise the mechanism in a smaller arena.
#ifndef CACHE_BLOCKS
#define CACHE_BLOCKS 7
#endif
// The allow-list's cap, the same as the ESP32-C6 radio's. The list's room comes from the arena, but
// the store holds room back for the list to reach the cap at the widest a tag can cost (listGuarantee
// in ap.c). The host tests set their own.
#ifndef MAX_ALLOWED_MACS
#define MAX_ALLOWED_MACS 1024
#endif

#define CACHE_FULL      (-1)  // no room and nothing to evict for it
#define CACHE_BAD_CHUNK (-2)  // chunk outside its block, or conflicting with what is cached

void cacheInit(void);
// What HELLO_REPLY and STATUS report, in kilobytes of block data (see the note at cacheKb in
// blockcache.c for how the S3 uses them):
// - cacheKb: the most block data the arena can ever hold, rounded down. Sent once per link.
// - cacheUsedKb: cacheKb less the block data the cache could take now without evicting anything,
//   rounded up, so that cacheKb - cacheUsedKb never overstates the room.
uint16_t cacheKb(void);
uint16_t cacheUsedKb(void);
uint8_t cacheBlocks(void);       // blocks held now
uint32_t cacheHeldBytes(void);   // ... and the arena they take, their own 24 bytes each included
uint32_t cacheRoomBytes(void);   // the arena the cache may have now, beside the tag store

int cacheFind(uint64_t ver, uint8_t blockId);  // slot, or -1
bool cacheComplete(int slot);
// Part `part` of the block as the tag gets it - struct blockData (size, checksum), the data, then
// 0xFF - into dst, BLOCK_PART_DATA_SIZE bytes. The 42 parts cover 4158 bytes, more than a block
// stores, so the padding is generated here.
void cacheCopyPart(int slot, uint8_t part, uint8_t *dst);

// Stores one chunk; returns the slot it went into, or CACHE_FULL / CACHE_BAD_CHUNK. `offered` is
// whether a pending entry offers this data: a block nothing offers is still cached (that is how the
// S3 preloads) but is the first to be evicted.
int cacheChunk(uint64_t ver, uint32_t offset, uint32_t totalSize, const uint8_t *bytes, size_t n, bool offered);

// A tag asked for this block of this data, cached or not. Blocks of the same data below it have
// been delivered, the one at it is wanted now, and the ones above it are what the tag asks for next,
// which keeps them out of reach of another tag's download.
void cacheRequested(uint64_t ver, uint8_t blockId);
void cacheServed(int slot);  // the block in this slot has gone out to a tag, and is in use now

// Blocks evicted to make room since boot, and how many of those had never gone out to a tag (the
// costly case: that tag then waits for the S3). Both saturate at 0xFFFF rather than wrap.
uint16_t cacheEvictions(void);
uint16_t cacheEvictionsUnserved(void);

// A slot is a block's handle, and keeps its number while the block's bytes move (see cacheCompact):
// ap.c holds one across a whole burst. One pinned slot at a time, -1 to unpin; a pinned block is never
// evicted or dropped.
void cachePin(int slot);
void cacheDrop(int slot);
void cacheDropVer(uint64_t ver);  // all blocks of this data, except a pinned one

// --- the arena the tag store shares with the blocks ---------------------------------------------

// Where the store lives: the low end of the arena, 8-byte aligned, never moved. Valid before
// cacheInit(), since apInit() lays its tables out first; what it keeps there is ap.c's business.
// Words, because the check-in scan wants aligned 32-bit loads, and reading one type's storage
// through another's pointer is punning that -O2 may reorder.
uint32_t *cacheListWords(void);
// Bytes the store may have with the cache at its floor: the arena less the floor. Also valid before
// cacheInit().
uint16_t cacheListBytesMax(void);
// The store needs this many bytes now: the cache evicts and compacts to free them, or takes them back
// when the store shrinks. False only past cacheListBytesMax. Call it BEFORE writing into the bytes.
bool cacheListReserve(uint16_t bytes);
