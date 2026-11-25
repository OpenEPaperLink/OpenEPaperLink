# Host tests of the AP logic: make -C test
# gcc, not make's default cc: the tests preload gcc's libasan (CC=... on the command line still wins)
CC := gcc
SRC := ../src/ap.c ../src/v2link.c ../src/blockcache.c host_hal.c
CFLAGS := -std=gnu99 -Wall -Wextra -Werror -Wno-unused-parameter -O1 -g -fPIC -shared \
	-fno-omit-frame-pointer -fsanitize=address,undefined -fno-sanitize-recover=undefined
# Each build picks its arena (HOST_ARENA_BYTES, what test/host_hal.c's halArena hands over; on the
# TLSR the link decides), and everything else follows from it as in the firmware: a full block costs
# 4120 bytes (its data, the tag's header and the cache's 20-byte slot), and the store's room is what
# is left above the floor of CACHE_BLOCKS full blocks.
#
# The default build: the eviction-policy tests count full 4096-byte blocks and were written against a
# cache of six, so the arena holds six full blocks and no seventh (6 * 4120 + 4040; a seventh needs 80
# more), with a floor of 1. The 4040 beside the blocks is room for the few tags most tests make, so
# none of them costs a block; the store still holds back 20 bytes a tag for the allow-list to reach
# its cap of 1024 (20704 in granules), which leaves 244 offers beside a full list and 144 unlisted
# tags (the_pending_table_fills_up_and_frees_up). Blocks going to the store and coming back is what
# libap_split.so below is for.
GEOM := -DHOST_ARENA_BYTES=28760 -DCACHE_BLOCKS=1
# The instrumented build is firmware someone runs too, and the only one in which the AP_DEBUG code is
# compiled at all, so it is tested rather than assumed to work. No geometry: host_hal.c's default
# arena is the release firmware's own, 54984 bytes (thirteen full blocks and 1424 beside them).
AP_DEBUG_ON := -DAP_DEBUG=1

all: test

build/libap.so: $(SRC) ../src/*.h ../../../oepl-radio-v2.h
	@mkdir -p build
	$(CC) $(CFLAGS) $(GEOM) -o $@ $(SRC)

# The arena split in miniature: 24672 bytes with a floor of 2 full blocks, so the cache starts with
# room for 5 (20600) and 4072 beside them, and the tag store has 16432 bytes above the floor. That
# guarantees the list 815 tags at the widest a tag can cost (20 bytes) and holds 1024 of a real fleet
# at 12, so the cap is exercised too. The cache gives up a block as the list passes 336, 672 and 1008
# tags of a real fleet, or 196, 400 and 608 strangers: both widths' thresholds in one build. The
# firmware's own arena reaches them only on a fleet larger than its cap, so this is where the
# mechanism is driven.
SPLIT := -DHOST_ARENA_BYTES=24672 -DCACHE_BLOCKS=2 -DMAX_ALLOWED_MACS=1024

build/libap_split.so: $(SRC) ../src/*.h ../../../oepl-radio-v2.h
	@mkdir -p build
	$(CC) $(CFLAGS) $(SPLIT) -o $@ $(SRC)

# Pending entries in the store: the split geometry with a list cap of 64, so that content may have
# nearly all of the 16432 bytes above the floor (532 unlisted tags, each a record and an offer, beside
# the 64 listed). The cache gives a block up as they pass 140 and takes it back once housekeeping sees
# them gone.
PEND := -DHOST_ARENA_BYTES=24672 -DCACHE_BLOCKS=2 -DMAX_ALLOWED_MACS=64

build/libap_pend.so: $(SRC) ../src/*.h ../../../oepl-radio-v2.h
	@mkdir -p build
	$(CC) $(CFLAGS) $(PEND) -o $@ $(SRC)

build/libap_debug.so: $(SRC) ../src/*.h ../../../oepl-radio-v2.h
	@mkdir -p build
	$(CC) $(CFLAGS) $(AP_DEBUG_ON) -o $@ $(SRC)

# src/tlsr/string_min.c is firmware-only, so it gets its own binary rather than a place in libap.so,
# where its memcpy() would replace the host's. -fstrict-aliasing is deliberate: it is what the real
# -O2 build does, and memcpy() reads a byte buffer through a uint32_t*.
STRCFLAGS := -std=gnu99 -Wall -Wextra -Werror -O2 -g -fstrict-aliasing -fno-omit-frame-pointer \
	-fsanitize=address,undefined -fno-sanitize-recover=undefined

# tools/stackdepth.py, which sizes the firmware's stacks, on listings written in test_stackdepth.py
# (the firmware's own listing needs the tc32 toolchain, so the makefile there runs it on every build)

build/test_string_min: test_string_min.c ../src/tlsr/string_min.c
	@mkdir -p build
	$(CC) $(STRCFLAGS) -o $@ test_string_min.c

test: build/libap.so build/libap_split.so build/libap_pend.so \
	build/libap_debug.so build/test_string_min
	ASAN_OPTIONS=detect_leaks=0 ./build/test_string_min
	LD_PRELOAD=$$($(CC) -print-file-name=libasan.so) ASAN_OPTIONS=detect_leaks=0 python3 test_ap.py
	python3 test_stackdepth.py

clean:
	rm -rf build

.PHONY: all test clean
