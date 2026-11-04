#include "blockcache.h"

#include <string.h>

#include "../../../oepl-radio-v2.h"
#include "hal.h"
#include "proto.h"

#define CACHE_SENT_SIZE (BLOCK_MAX_PARTS * BLOCK_PART_DATA_SIZE)
#define CHUNKS_PER_BLOCK ((BLOCK_DATA_SIZE + V2_CHUNK_MAX - 1) / V2_CHUNK_MAX)

// --- a block in the arena -----------------------------------------------------------------------
// A block is stored as what the tag gets - struct blockData (size, checksum) and the data - followed
// by its slot: no padding to 4 KB, and no 0xFF after the data, which cacheCopyPart makes up as a part
// goes out. The slot is at the TOP of the block's bytes because the blocks are packed against the
// arena's top and walked downwards (EACH_BLOCK, cacheCompact): the walk reads a block's length at
// the address it already has, the block's end, and steps to the next one below.
//
//   ... | blockData | data ........ | pad | slot | blockData | data .. | pad | slot |  top
//       <------------ cacheCost(len) -----------><------- cacheCost(len) ------->
//
// In the 20-byte slot, len is the only way to find the next block down; lastUse is 16 bits,
// renormalised before it would wrap (cacheTick), as it only orders blocks by age; id stays with the
// block while its bytes move; the flags share the chunk bitmap's word (at most 18 chunks a block).
// packed and aligned(4): the host would otherwise pad after the uint64_t and differ from the
// -fpack-struct firmware in sizes the tests check, and the TC32, which has no unaligned access, can
// then load ver by words.
struct cacheSlot {
    uint64_t ver;
    uint32_t state;   // bits 0..CHUNKS_PER_BLOCK-1: chunk n is stored; from bit 24, ST_*
    uint16_t len;     // data bytes in this block
    uint16_t lastUse;  // useClock at the last chunk or use
    uint8_t blockId;
    uint8_t wantedFrom;  // the block this data's tag asked for last; below it the tag has been served
    uint8_t id;          // the slot number ap.c holds
    uint8_t doom;        // 0, or this block's place in the evictions being chosen (cacheMakeRoom)
} __attribute__((packed, aligned(4)));
#define ST_COMPLETE (1UL << 24)
#define ST_SERVED (1UL << 25)   // has gone out to a tag in a burst
#define ST_OFFERED (1UL << 26)  // a pending entry offered this data when the chunk was stored
// The block at wantedFrom of this data is held and has gone out, so the tag's next block is the one
// after it (cacheNextWanted). A property of the data, kept in every block of it by cacheMarkPast so
// that the eviction policy needs no walk per candidate.
#define ST_PAST (1UL << 27)
#define ST_DEAD (1UL << 28)  // dropped or evicted: its bytes are free, and go at the next compaction

V2_STATIC_ASSERT(sizeof(struct cacheSlot) == 20, "the slot the figures below are worked out with");
V2_STATIC_ASSERT(CHUNKS_PER_BLOCK <= 24, "chunk bitmap below the flags");
V2_STATIC_ASSERT(CACHE_SENT_SIZE >= BLOCK_DATA_SIZE + sizeof(struct blockData), "the parts a burst sends cover a block and its header");
V2_STATIC_ASSERT(CACHE_BLOCKS >= 1 && CACHE_BLOCKS <= 15, "the floor: at least a block, and inside 64 KB of SRAM");

// What a block costs the arena: header, data, slot, to a granule of 4 bytes. Holes are closed by
// compaction rather than reused, so a coarser granule would only round; 4 is what the slot's word
// loads and cacheMove's word copy need.
#define CACHE_GRAN 4u
static uint32_t cacheCost(uint32_t len) {
    return (len + sizeof(struct blockData) + sizeof(struct cacheSlot) + CACHE_GRAN - 1) & ~(CACHE_GRAN - 1);
}
#define CACHE_FULL_COST 4120u  // cacheCost(BLOCK_DATA_SIZE), as a constant for the link's arithmetic
#define CACHE_OVERHEAD 24u     // cacheCost(0)
// The floor, in bytes: CACHE_BLOCKS full blocks at what they cost
#define CACHE_FLOOR_BYTES ((uint32_t)CACHE_BLOCKS * CACHE_FULL_COST)
// Slot numbers run 0..126 (the ids in use are a bitmap of 128); ap.c holds one in an int, with -1
// and the CACHE_* codes outside that range. Only an arena of very small blocks reaches the cap, and
// the cache then evicts by its policy for a slot as it does for bytes.
#define CACHE_SLOTS_LIMIT 127

// --- the arena ----------------------------------------------------------------------------------
// The arena's layout, worked out once from what halArena() hands over (cacheLayout). Nothing in it is
// a compile-time size: the linker gives the arena every byte the rest of the image leaves above the
// stack's floor.
//
//   arena | store --> |      free      | dead | block | block | dead | block | top
//                     ^ listBytes      ^ dataLow                             ^ arena + arenaBytes
//
// The first thing above the main stack's floor is the top block's slot, so a stack overrun would
// break the walk. The stack sizing rules that out: 4/3 of a static bound of the deepest path
// (tools/stackdepth.py), with the high-water mark reported on the stk line and warned about past 3/4.
static uint8_t *arena;            // what the store and the blocks share
static uint8_t *top;              // its end, where the blocks start from
static uint8_t *dataLow;          // the lowest block's first byte; top when there is none
static uint32_t arenaBytes = 0;   // its size, a multiple of 8
static uint16_t listBytesMax = 0;  // what the store may have with the cache at its floor
static uint16_t listBytes = 0;    // the store's end, from the arena's low end
// Bytes of the blocks still held, and how many there are. The rest of [dataLow, top) is dead blocks,
// free at the next compaction.
static uint32_t liveBytes = 0;
static uint8_t liveCount = 0;
static uint32_t idUsed[4];  // the slot numbers in use
static uint16_t useClock = 0;
static int pinned = -1;
// Blocks evicted since boot (for a block or for the tag store), and those of them that had never gone
// out to a tag, each of which turned a 15 ms request into a round trip to the S3 that would otherwise
// look like a slow S3. Both saturate rather than wrap.
static uint16_t evictions = 0;
static uint16_t evictionsUnserved = 0;

// The least arena this file is built for, as a symbol the link asserts (_cache_arena_need_ in
// boot_8258_ap.link): the floor's blocks, and the allow-list at its cap at the widest a tag can cost
// (storeInit in ap.c: a 12-byte record and an 8-byte profile, in granules of 16 and 4, beside a granule
// of 4 16-byte offers), below which the inventory lock's guarantee is gone. An assembler expression so
// that it follows CACHE_BLOCKS and MAX_ALLOWED_MACS; the static assert pins the sizes it cannot see.
V2_STATIC_ASSERT(CACHE_FULL_COST == ((BLOCK_DATA_SIZE + 24u + 3u) & ~3u) && CACHE_OVERHEAD == 24u &&
                     sizeof(struct blockData) + sizeof(struct cacheSlot) == 24u,
                 "the figures in _cache_arena_need_");
#define BC_STR_(x) #x
#define BC_STR(x) BC_STR_(x)
__asm__(".global _cache_arena_need_\n\t.set _cache_arena_need_, (" BC_STR(CACHE_BLOCKS) ") * 4120 + ((" BC_STR(
    MAX_ALLOWED_MACS) ") + 4) / 4 * 4 * 8 + ((" BC_STR(MAX_ALLOWED_MACS) ") + 16) / 16 * 16 * 12 + 64");

// The arena's geometry, from the bytes halArena() reports. Once per boot, on the first call of
// whichever function needs it, because ap.c lays its tables out at the store's base before it calls
// cacheInit().
static void cacheLayout(void) {
    if (arena) return;
    uint32_t bytes = 0;
    uint8_t *base = halArena(&bytes);
    // Both should already be 8-aligned (the linker aligns the start, the stack floor is 8-aligned).
    const uint32_t skew = (uint32_t)((8u - ((uintptr_t)base & 7u)) & 7u);
    bytes = bytes > skew ? (bytes - skew) & ~7u : 0;
    arena = base + skew;
    arenaBytes = bytes;
    top = dataLow = arena + arenaBytes;
    // The TLSR link asserts that the floor and the allow-list's room fit; a host test build that
    // breaks that gets what there is rather than a wrapped count.
    uint32_t room = arenaBytes > CACHE_FLOOR_BYTES ? arenaBytes - CACHE_FLOOR_BYTES : 0;
    if (room > 0xFFF8u) room = 0xFFF8u;  // the store's byte count is a uint16_t
    listBytesMax = (uint16_t)room;
}

// The slot of the block that ends at `end`, and that block's first byte
static struct cacheSlot *slotEnding(uint8_t *end) { return (struct cacheSlot *)(void *)(end - sizeof(struct cacheSlot)); }
static uint8_t *blockStart(struct cacheSlot *s) { return (uint8_t *)s + sizeof(struct cacheSlot) - cacheCost(s->len); }
// Every block, live or dead, from the top down: `e` is the end of each in turn
#define EACH_BLOCK(e) for (uint8_t *e = top; e > dataLow; e -= cacheCost(slotEnding(e)->len))
static bool slotLive(const struct cacheSlot *s) { return !(s->state & ST_DEAD) && !s->doom; }

static struct cacheSlot *slotOf(int slot) {
    if (slot < 0 || slot >= CACHE_SLOTS_LIMIT) return NULL;
    EACH_BLOCK(e) {
        struct cacheSlot *s = slotEnding(e);
        if (slotLive(s) && s->id == slot) return s;
    }
    return NULL;
}
static struct cacheSlot *slotFind(uint64_t ver, uint8_t blockId) {
    EACH_BLOCK(e) {
        struct cacheSlot *s = slotEnding(e);
        if (slotLive(s) && s->blockId == blockId && s->ver == ver) return s;
    }
    return NULL;
}

static uint8_t idTake(void) {
    for (uint8_t i = 0; i < CACHE_SLOTS_LIMIT; i++) {
        if (!(idUsed[i >> 5] & (1UL << (i & 31)))) {
            idUsed[i >> 5] |= 1UL << (i & 31);
            return i;
        }
    }
    return 0;  // unreachable: cacheMakeRoom keeps fewer than CACHE_SLOTS_LIMIT live
}

// A block gone: its bytes stay where they are until a compaction needs them, so a walk that kills
// blocks as it goes can carry on.
static void cacheKill(struct cacheSlot *s) {
    s->state |= ST_DEAD;
    idUsed[s->id >> 5] &= ~(1UL << (s->id & 31));
    liveBytes -= cacheCost(s->len);
    liveCount--;
}

// The age order, in 16 bits: halved back before it would wrap, so a comparison is a plain one. A
// block more than 32768 uses old ends up at 0 - the oldest - which is what it is.
static uint16_t cacheTick(void) {
    if (useClock == 0xFFFF) {
        EACH_BLOCK(e) {
            struct cacheSlot *s = slotEnding(e);
            s->lastUse = s->lastUse > 0x8000 ? (uint16_t)(s->lastUse - 0x8000) : 0;
        }
        useClock = 0x7FFF;
    }
    return ++useClock;
}

// Whether the block at this data's wantedFrom is held and has gone out, in every block of the data
static void cacheMarkPast(uint64_t ver) {
    bool past = false;
    EACH_BLOCK(e) {
        const struct cacheSlot *s = slotEnding(e);
        if (slotLive(s) && s->ver == ver && s->blockId == s->wantedFrom && (s->state & ST_SERVED)) past = true;
    }
    EACH_BLOCK(e) {
        struct cacheSlot *s = slotEnding(e);
        if (s->ver != ver) continue;
        if (past) s->state |= ST_PAST;
        else s->state &= ~ST_PAST;
    }
}

void cacheInit(void) {
    cacheLayout();
    dataLow = top;
    liveBytes = 0;
    liveCount = 0;
    memset(idUsed, 0, sizeof(idUsed));
    listBytes = 0;
    useClock = 0;
    pinned = -1;
    evictions = 0;
    evictionsUnserved = 0;
}

uint16_t cacheEvictions(void) { return evictions; }
uint16_t cacheEvictionsUnserved(void) { return evictionsUnserved; }

void cacheRequested(uint64_t ver, uint8_t blockId) {
    if (ver == 0) return;
    EACH_BLOCK(e) {
        struct cacheSlot *s = slotEnding(e);
        if (slotLive(s) && s->ver == ver) s->wantedFrom = blockId;
    }
    cacheMarkPast(ver);
}

// Where this data's transfer stands, for a block of it about to be stored: from another block of the
// same data, else block 0. A block sent in answer to a request for block n then looks n ahead of its
// tag; harmless, as nothing else competes for room while its chunks arrive, and cacheServed corrects
// it once the burst has gone out.
static uint8_t cacheWantedFrom(uint64_t ver) {
    EACH_BLOCK(e) {
        const struct cacheSlot *s = slotEnding(e);
        if (slotLive(s) && s->ver == ver) return s->wantedFrom;
    }
    return 0;
}

// The block this block's tag asks for next: the one it asked for last, or the one after it when that
// one has gone out already. A block below this line has been delivered; the one at it is what the
// tag comes back for, and that is the block no other transfer may take the room of.
static uint8_t cacheNextWanted(const struct cacheSlot *s) {
    return ((s->state & ST_PAST) && s->wantedFrom < 0xFF) ? (uint8_t)(s->wantedFrom + 1) : s->wantedFrom;
}

// The most block data `bytes` of arena can hold: full blocks at 4120, and in what is left one shorter
// block at its own 24 bytes. Full blocks are the most data per byte there is, so no mix of shorter
// ones holds more; at most sixteen fit in 64 KB, so the slot cap does not come into it.
static uint32_t cacheDataFit(uint32_t bytes) {
    uint32_t data = bytes / CACHE_FULL_COST * BLOCK_DATA_SIZE;
    const uint32_t rest = (bytes % CACHE_FULL_COST) & ~(CACHE_GRAN - 1);
    if (rest > CACHE_OVERHEAD) data += rest - CACHE_OVERHEAD;
    return data;
}

// The S3 (V2Transport, v2transport.cpp) gets cacheKb in HELLO_REPLY and cacheUsedKb at every STATUS,
// and preloads an item only when ceil(dataSize / 4096) * 4 KB fits in cacheKb - cacheUsedKb, taking
// that much off until the next STATUS. Both are counted in block data, so a full block takes exactly
// 4 KB off the free figure (cacheDataFit(F) - 4096 == cacheDataFit(F - 4120)) and a shorter block
// less: the S3 can under-preload between two STATUS frames, never over-. The free figure counts held
// blocks at their arena cost and everything the tag store has taken, so it never claims room that is
// not there. It can still go stale before the next STATUS (the store grows, or very small blocks run
// out of slot numbers, CACHE_SLOTS_LIMIT); the cache then evicts by its policy, counted in cev/cevu.
uint16_t cacheKb(void) {
    cacheLayout();
    return (uint16_t)(cacheDataFit(arenaBytes) / 1024);
}
uint16_t cacheUsedKb(void) {
    const uint32_t taken = (uint32_t)listBytes + liveBytes;
    const uint32_t freeKb = taken < arenaBytes ? cacheDataFit(arenaBytes - taken) / 1024 : 0;
    return (uint16_t)(cacheKb() - freeKb);
}
uint8_t cacheBlocks(void) { return liveCount; }
uint32_t cacheHeldBytes(void) { return liveBytes; }
uint32_t cacheRoomBytes(void) {
    cacheLayout();
    return arenaBytes - listBytes;
}

int cacheFind(uint64_t ver, uint8_t blockId) {
    const struct cacheSlot *s = slotFind(ver, blockId);
    return s ? s->id : -1;
}

bool cacheComplete(int slot) {
    const struct cacheSlot *s = slotOf(slot);
    return s && (s->state & ST_COMPLETE);
}

void cacheCopyPart(int slot, uint8_t part, uint8_t *dst) {
    struct cacheSlot *s = slotOf(slot);
    const uint32_t have = s ? sizeof(struct blockData) + s->len : 0;
    const uint32_t at = (uint32_t)part * BLOCK_PART_DATA_SIZE;
    uint32_t n = at < have ? have - at : 0;
    if (n > BLOCK_PART_DATA_SIZE) n = BLOCK_PART_DATA_SIZE;
    if (n) memcpy(dst, blockStart(s) + at, n);
    memset(dst + n, 0xFF, BLOCK_PART_DATA_SIZE - n);  // the parts run on past what a block stores
}

void cacheServed(int slot) {
    struct cacheSlot *s = slotOf(slot);
    if (!s) return;
    s->state |= ST_SERVED;
    s->lastUse = cacheTick();
    cacheRequested(s->ver, s->blockId);
}

// The block whose tag is least likely to ask for it next. Candidates fall into three groups, the
// first to go first:
//
//   0  no pending entry offers this data any more, so nobody will ever ask for it
//   1  the tag getting this data has had this block already
//   2  the tag has still to ask for it, and the one furthest ahead of it goes first
//
// A tag walks its image upwards, so in group 2 the block it is waiting for is worth most although it
// is the oldest of its image; least-recently-used would evict exactly that one. Within groups 0 and 1
// age decides, which keeps the block that has just gone out for a tag asking again for parts it lost.
// Size does not come into it: see cacheMakeRoom.
//
// noinline keeps this frame separate from cacheMakeRoom's, its only caller, which is on the deepest
// main-stack path (ALLOW_ADD -> storeGrow -> cacheListReserve); inlined, the combined frame is larger.
__attribute__((noinline)) static struct cacheSlot *cacheClaim(void) {
    struct cacheSlot *victim = NULL;
    uint8_t victimGroup = 0, victimAhead = 0;
    EACH_BLOCK(e) {
        struct cacheSlot *s = slotEnding(e);
        if (!slotLive(s) || s->id == pinned) continue;
        const uint8_t next = cacheNextWanted(s);
        uint8_t group = 2, ahead = 0;
        if (!(s->state & ST_OFFERED)) {
            group = 0;
        } else if (s->blockId < next) {
            group = 1;
        } else {
            ahead = (uint8_t)(s->blockId - next);
        }
        bool better;
        if (!victim) {
            better = true;
        } else if (group != victimGroup) {
            better = group < victimGroup;
        } else if (group == 2) {
            better = ahead > victimAhead;
        } else {
            better = s->lastUse < victim->lastUse;
        }
        if (better) {
            victim = s;
            victimGroup = group;
            victimAhead = ahead;
        }
    }
    return victim;
}

// Moves n bytes (a multiple of 4, both ends word-aligned) up to dst > src, from the top down so that
// the two may overlap; a word at a time, since memmove is byte by byte on the TLSR. may_alias for the
// reason src/tlsr/string_min.c gives: the slots are read through their own type around these word
// copies, and -O2 must not reorder the two.
typedef uint32_t cacheWord __attribute__((may_alias));
static void cacheMove(uint8_t *dst, const uint8_t *src, uint32_t n) {
    cacheWord *d = (cacheWord *)(void *)(dst + n);
    const cacheWord *s = (const cacheWord *)(const void *)(src + n);
    for (n /= 4; n; n--) *--d = *--s;
}

// Gets the cache's lowest byte up to `floorAt` by closing the holes dead blocks left, the lowest ones
// first and only as many as it takes: closing a hole moves every block below it up, so the walk
// leaves every hole whose space the holes below it already make up. The caller has made sure the dead
// bytes are enough.
//
// A block that moves keeps its slot number, and ap.c finds its slot by number (slotOf) every time it
// is used, so a burst reading cacheCopyPart(burstSlot, ...) across main-loop passes reads the same
// block wherever it has moved to.
static void cacheCompact(uint8_t *floorAt) {
    if (dataLow >= floorAt) return;
    const uint32_t deficit = (uint32_t)(floorAt - dataLow);
    uint32_t dead = (uint32_t)(top - dataLow) - liveBytes;  // dead bytes at or below the walk
    uint8_t *w = top;  // where the next block kept goes: its end
    bool moving = false;
    for (uint8_t *e = top; e > dataLow;) {
        struct cacheSlot *s = slotEnding(e);
        const uint32_t n = cacheCost(s->len);
        uint8_t *const b = e - n;
        if (s->state & ST_DEAD) {
            dead -= n;
            if (!moving && dead >= deficit) w = b;  // the holes below make up the room: this one stays
            else moving = true;                     // ... or not, and from here down the holes close
        } else {
            if (moving) cacheMove(w - n, b, n);
            w -= n;
        }
        e = b;
    }
    dataLow = w;
}

// Room for a block of `need` bytes (with a slot number for it, when `slot`) between the store's end at
// `low` and the blocks: evicts through the policy until the blocks kept leave that much, then compacts
// so that it is in one piece just below them. `need` 0 and a `low` past the store's end is the store
// taking `low` bytes. False when even evicting everything but the pinned block would not do, which
// the floor (at least seven full blocks) prevents.
//
// Blocks go in the policy's order only, never by size: an eviction costs a tag the same wait for the
// S3 whatever the block's size. The chosen blocks are then looked at again in reverse order, the best
// placed first, and each one the others already make room without is spared.
static bool cacheMakeRoom(uint32_t low, uint32_t need, bool slot) {
    const uint32_t room = low < arenaBytes ? arenaBytes - low : 0;
    uint32_t keep = liveBytes;
    uint8_t kept = liveCount, doomed = 0;
    while (keep + need > room || (slot && kept >= CACHE_SLOTS_LIMIT)) {
        struct cacheSlot *v = cacheClaim();
        if (!v) break;
        v->doom = ++doomed;
        keep -= cacheCost(v->len);
        kept--;
        cacheMarkPast(v->ver);  // the next choice sees this one gone, as one eviction at a time would
    }
    const bool ok = keep + need <= room && !(slot && kept >= CACHE_SLOTS_LIMIT);
    for (uint8_t k = doomed; k; k--) {
        struct cacheSlot *v = NULL;
        EACH_BLOCK(e) {
            if (slotEnding(e)->doom == k) v = slotEnding(e);
        }
        if (!v) continue;  // unreachable
        const uint32_t c = cacheCost(v->len);
        if (ok && (keep + c + need > room || (slot && kept + 1 >= CACHE_SLOTS_LIMIT))) continue;  // needed
        v->doom = 0;
        keep += c;
        kept++;
        cacheMarkPast(v->ver);
    }
    if (!ok) return false;  // every block given back: nothing is lost by trying
    if (doomed) {
        EACH_BLOCK(e) {
            struct cacheSlot *s = slotEnding(e);
            if (!s->doom) continue;
            s->doom = 0;
            if (evictions != 0xFFFF) evictions++;
            if (!(s->state & ST_SERVED) && evictionsUnserved != 0xFFFF) evictionsUnserved++;
            cacheKill(s);
        }
    }
    cacheCompact(arena + low + need);
    return true;
}

uint32_t *cacheListWords(void) {
    cacheLayout();
    return (uint32_t *)(void *)arena;
}
uint16_t cacheListBytesMax(void) {
    cacheLayout();
    return listBytesMax;
}

// Growing, the store takes its bytes from the free space first, then from dead blocks, and only then
// from blocks still held, by the policy's choice and moved up out of its way. Shrinking (ALLOW_CLEAR,
// a tag deleted, housekeeping's trim) gives them back by doing nothing: the free space between the
// two is the cache's to allocate from.
bool cacheListReserve(uint16_t bytes) {
    cacheLayout();
    if (bytes > listBytesMax) return false;
    if (!cacheMakeRoom(bytes, 0, false)) return false;  // unreachable inside the floor
    listBytes = bytes;
    return true;
}

// The block's length is known from its first chunk - the block's share of totalSize - so it is
// stored at that length from the start, and a chunk that implies another length starts it again.
int cacheChunk(uint64_t ver, uint32_t offset, uint32_t totalSize, const uint8_t *bytes, size_t n, bool offered) {
    const uint32_t blockStartAt = offset - offset % BLOCK_DATA_SIZE;
    const uint32_t pos = offset - blockStartAt;
    if (blockStartAt / BLOCK_DATA_SIZE > 0xFF || pos % V2_CHUNK_MAX || n == 0 || totalSize <= offset ||
        n > totalSize - offset || pos + n > BLOCK_DATA_SIZE) {
        return CACHE_BAD_CHUNK;
    }
    uint32_t len = totalSize - blockStartAt;
    if (len > BLOCK_DATA_SIZE) len = BLOCK_DATA_SIZE;

    const uint8_t blockId = (uint8_t)(blockStartAt / BLOCK_DATA_SIZE);
    struct cacheSlot *s = slotFind(ver, blockId);
    if (s && s->len != len) {
        if (s->id == pinned) return CACHE_BAD_CHUNK;
        cacheKill(s);  // another size for the same data: start the block again
        cacheMarkPast(ver);
        s = NULL;
    }
    if (!s) {
        const uint8_t wantedFrom = cacheWantedFrom(ver);  // before cacheMakeRoom, which may evict its source
        const uint32_t cost = cacheCost(len);
        if (!cacheMakeRoom(listBytes, cost, true)) return CACHE_FULL;
        uint8_t *const end = dataLow;
        dataLow -= cost;
        s = slotEnding(end);
        s->ver = ver;
        s->state = 0;
        s->len = (uint16_t)len;
        s->blockId = blockId;
        s->wantedFrom = wantedFrom;
        s->id = idTake();
        s->doom = 0;
        struct blockData *bd = (struct blockData *)(void *)blockStart(s);
        bd->size = (uint16_t)len;
        bd->checksum = 0;
        liveBytes += cost;
        liveCount++;
        cacheMarkPast(ver);  // a new block at wantedFrom has not gone out, so the data is not past it
    }
    if (offered) s->state |= ST_OFFERED;
    else s->state &= ~ST_OFFERED;
    s->lastUse = cacheTick();
    if (s->state & ST_COMPLETE) return s->id;  // a retransmission, or the S3 sending a block twice

    struct blockData *bd = (struct blockData *)(void *)blockStart(s);
    memcpy(bd->data + pos, bytes, n);
    s->state |= 1UL << (pos / V2_CHUNK_MAX);
    const uint32_t needed = (1UL << ((len + V2_CHUNK_MAX - 1) / V2_CHUNK_MAX)) - 1;
    if ((s->state & needed) != needed) return s->id;

    // complete: the checksum the tag checks in the blockData header
    uint16_t sum = 0;
    for (uint32_t c = 0; c < len; c++) sum += bd->data[c];
    bd->checksum = sum;
    s->state |= ST_COMPLETE;
    return s->id;
}

void cachePin(int slot) { pinned = slot; }

void cacheDrop(int slot) {
    struct cacheSlot *s = slotOf(slot);
    if (!s || slot == pinned) return;
    cacheKill(s);
    cacheMarkPast(s->ver);  // its slot is still readable: a dead block's bytes stay until compaction
}

void cacheDropVer(uint64_t ver) {
    EACH_BLOCK(e) {
        struct cacheSlot *s = slotEnding(e);
        if (slotLive(s) && s->ver == ver && s->id != pinned) cacheKill(s);
    }
    cacheMarkPast(ver);
}
