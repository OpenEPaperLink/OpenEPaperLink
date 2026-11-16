# OpenEPaperLink TLSR8258 AP radio firmware (802.15.4)

Open-source firmware for the TLSR8258 radio next to the ESP32-S3 of an OpenEPaperLink AP, such as the
OpenEPaperLink Nano TLSR. It replaces the closed-source `binaries/TLSR/OEPL_TLSR_AP.bin` (AP type
`0xF8`, "TLSR Zigbee 802.15.4", version `0x007C`, listed as `0001` in `firmware_TLSR.json`) and
reports version `0x0080`.

`src/ap.c` is the AP logic of the ESP32-C6/H2 radio firmware 0x0024
(`../OpenEPaperLink_esp32_C6_AP/main/main.c`) on a small hardware abstraction (`src/hal.h`). It talks
[protocol v2](../../docs/design/radio-protocol-v2.md) to the S3, so the S3 needs a firmware with
protocol v2 support, as it does for the C6/H2 from 0x0023. An older S3 won't recognise it.

## Status

Beta. It runs on an OpenEPaperLink Nano TLSR serving ZBS243 tags. Other tag families haven't been tried
with this radio; what the code assumes about them comes from reading their firmware. `firmware_TLSR.json` still offers the
closed-source image by default; the beta build is `binaries/TLSR/OEPL_TLSR_AP_0080_beta.bin`.

## Compared with the closed-source firmware

| | closed-source (`0x007C`) | this firmware (`0x0080`) |
| --- | --- | --- |
| S3 link | ASCII protocol, 115200 baud | protocol v2 at 2 Mbaud: COBS frames, CRC16, sequence numbers, ACKs and retransmission |
| Block data | fetched from the S3 for every request; the tag waits 550 ms | block cache in the radio, preloaded by the S3: a cached block costs the tag no wait; otherwise the wait starts at 140 ms and rises if the S3 proves slower |
| Cache size | none | all the SRAM the image leaves, shared with the tag store: about 53 KB of block data in a default build, never less than 7 full blocks |
| Block burst | 42 frames, blocking | up to 260 ms, which fits the tag's listening window (about 52 frames); copies per part adapt to the channel; one frame per main-loop pass, so the S3 is served in between |
| Radio TX | radio re-initialised and a 500 µs wait before every frame | radio stays initialised; replies started by the radio's state machine 1220 µs after the request |
| Radio RX | frames copied in the interrupt | zero-copy DMA ring of 4 slots |
| TX power | driver default | what the S3 configures, on every frame |
| Tag inventory lock | none | allow-list in the radio (up to 1024 MACs), with learning mode |
| Late requests | answered anyway | skipped when too old for the tag to take the reply |
| Diagnostics | none | STATUS every second, BLOCK_SERVED per burst, LOG lines on request |

## How it works

- **One arena.** Everything the link leaves between `.bss` and the main stack is one arena, shared by
  the block cache and the tag store (allow-list, pending entries). When the store needs room,
  the cache evicts the block the tags are least likely to ask for next. Nothing has a compile-time
  size: at boot the firmware derives the maxima from the arena and announces them (`cacheKb` in
  HELLO_REPLY; `ckb`, `tags`, `pend` and `ofr` on the `cfg` LOG line). The cache always keeps 7 full
  blocks (`CACHE_BLOCKS`), and the store always keeps room for the allow-list to reach its cap of 1024
  (`MAX_ALLOWED_MACS`), because an `ALLOW_ADD` refused with `NO_SLOT` makes the S3 drop the radio-side
  lock until the link restarts. A block costs its length plus 24 bytes (4120 for a full 4 KB block). A
  tag costs a 12-byte record plus a share of an 8-byte profile (tags whose MACs differ only in their
  serial bytes share one), plus a 16-byte offer while content waits for it.
- **One burst timing for every tag.** How a tag spends a wait depends on its chip and on its firmware
  version, so the radio doesn't try to know. A block in the cache gets a wait of 0, which every tag
  answers by receiving straight away, and the burst follows 3 ms after the ack (an nRF52811 before
  firmware 0x002D restarts its radio first). After any other wait the burst starts 5 ms after it ends.
- **Bursts are sized to the window.** Every requested part goes out at least once, and parts are
  repeated as long as the tag's window allows. Copies per part start at 2 and move with what the radio
  measures: fewer when its bursts are estimated to cost other tags' requests more than the repeats
  save, more when tags come back for missed parts and the channel is less than 25 % busy. No signal,
  no change.
- **Bursts listen.** A part that falls due while a frame is arriving waits for it, so that frame is
  answered instead of cut off. It also waits while the channel reads above -80 dBm, for what the radio
  can't decode, such as WiFi, so a clean channel costs nothing. Holds no frame comes of are limited to
  30 ms a burst, and on a channel busy 25 % of the time or more the burst doesn't wait for energy at
  all. A receive flag that stays up longer than a frame lasts is stuck, and is ignored for the rest of
  the burst. A tag that leaves once it has the block asks for the next one at once, so after a pass the
  radio pauses to listen for that request, for as long as tags have been seen to take; a tag that
  listens out its whole window loses at most the repeats the pause pushes past its window. A burst
  stops when its tag asks for something else.
- **Replies are scheduled.** The radio stays initialised and switches from TX back to RX in the TX
  interrupt. A reply is started by the radio's own state machine at a fixed time after the request
  (1220 µs, 1700 µs for a PONG), however long the processing took. Register use follows the Telink
  driver library and, where `hal_tlsr.c` says so, Telink's
  [802.15.4 SDK](https://github.com/telink-semi/telink_802154_sdk).
- **A second tag waits its turn.** A tag that sends a block request during another tag's burst is
  acked with a wait that ends just after that burst, its block is asked of the S3 at once, and its
  burst follows. There is one such slot; only a third tag at the same moment gets CANCEL_XFER (`cxlb`).

## Building

You need Telink's `tc32-elf-gcc` (GCC 4.5.1) and the SDK in `../OpenEPaperLink_TLSR/tl_zigbee_sdk`.

- **Linux:** the toolchain is a submodule. From the repository root:

  ```
  git submodule update --init ARM_Tag_FW/OpenEPaperLink_TLSR/tc32_linux
  cd ARM_Tag_FW/OpenEPaperLink_TLSR_AP && make
  ```

- **Windows:** the toolchain is in `../OpenEPaperLink_TLSR/tc32_windows`. From this directory, run
  `..\OpenEPaperLink_TLSR\tc32_windows\bin\make.exe`.
- **macOS and others:** there is no tc32 toolchain; build in a Linux container or VM.

Neither `TC32_PATH` nor the checkout's path may contain spaces. In a git worktree the submodule isn't
checked out, so the makefile uses the main checkout's copy (git 2.31 or later).

Python 3 is needed for `tools/stackdepth.py`, which sizes both stacks at build time: it reads the
deepest main and interrupt call paths off a first link, and the second link reserves 4/3 of each.
Without Python 3 the stacks are a fixed 2048 and 512 bytes, and the arena is about 1 KB smaller.

`make` writes `out/OEPL_TLSR_AP.bin`, with `.elf`, `.lst` and `.map` beside it. There are no size
options: the cache floor and the allow-list cap are constants in `src/blockcache.h`, the UART receive
ring in `src/tlsr/hal_tlsr.c`. The options are:

| | |
| --- | --- |
| `make AP_DEBUG=1` | the instrumented build, in `out/debug`: LOG lines always on, plus per-ping, per-join and per-check-in lines and a noise histogram. Nothing sent on the air changes |
| `make TC32_PATH=/opt/tc32/bin/` | use a toolchain elsewhere (with the trailing slash) |
| `make PYTHON=...` | the Python 3 interpreter (default `python3`, `python` on Windows) |
| `make STACK_ANALYSIS=0` | skip the stack analysis and use the fixed stacks |
| `make RAMCODE_SPILL_OK=1` | link even when RAM code ends just past a 256-byte boundary, which costs 256 bytes of SRAM |
| `make clean` | remove `out/` |

### RAM

The TLSR8258 has 64 KB of SRAM. `boot_8258_ap.link` places the RAM code, `.data`, `.bss` and the
interrupt stack, pins the main stack's top at `0x84ff00`, and gives everything in between to the
arena, so added `.bss` costs cached blocks and tags rather than failing the link. The link fails only
when the arena drops below the cache floor plus the allow-list's room, or when RAM code ends just past
a 256-byte boundary (`.data` starts at the RAM code's end rounded up to 256). Check a build with
`tc32-elf-nm out/OEPL_TLSR_AP.elf`:

| symbol | |
| --- | --- |
| `_cache_arena_size_`, `_cache_arena_need_` | the arena this build has, and the least it may have |
| `_stack_size_`, `_stack_bound_` | the main stack, and the deepest path the analysis found |
| `_irq_stk_size_`, `_irq_stk_bound_` | the same for the interrupt stack |
| `_ramcode_tail_` | how far RAM code ends below the next 256-byte boundary; `0xc0` or more is refused unless `RAMCODE_SPILL_OK=1` |
| `_bss_headroom_` | bytes lost to the arena's 8-byte alignment |

## Flashing

- **From the AP's web interface**, on boards that flash the TLSR over SWS (`FLASHER_AP_SWS`, such as
  the Nano TLSR): upload `out/OEPL_TLSR_AP.bin` (or the beta binary) to the AP's file system as
  `/TLSR_AP_FW.bin` and press "Flash TLSR (SWS)" on the update page, which flashes that file
  (`/flash_tlsr` without a URL). The release download on the same page installs the closed-source
  image listed in `firmware_TLSR.json`, which is also the way back.
- **With a Telink USB programmer:**
  `../OpenEPaperLink_TLSR/flashing/telink_flasher.exe 1 8258 wf 0 -i out/OEPL_TLSR_AP.bin`.

## Diagnostics

The radio answers the S3's HEARTBEAT, once a second, with STATUS: uptime, channel, power, reset
reason, pending entries, cache use, radio frames, late requests, serial errors and retransmits.

LOG text lines are sent when the S3 sets `V2_CFG_LOG_FRAMES` in SET_CONFIG (which the S3 firmware
doesn't do) or always in an `AP_DEBUG=1` build. The S3 prints them on its USB serial console, prefixed
`radio:`. Every 15 s while there is activity (otherwise every 5 minutes) the radio sends the first
four lines below and at most one of the next four:

| line | contents |
| --- | --- |
| `ci` | check-ins answered, with data, repeated within 300 ms (`rep`: the tag missed the reply), reply times, late requests skipped |
| `xfer` | block requests, bursts, XFC, re-requests, re-sent acks, copies per part (`cpy`), pings |
| `blk cxlb` | turned-away and queued block requests (`cxlb`, `cxln`, `bq`), evictions (`cev`, `cevu`: never served), cache use, S3 answer times and the current wait (`dw`) |
| `rf` | radio frames, CRC failures, channel busy %, UART DMA counters |
| `boot` | once after start: PA and timing registers, applied power, reset reason, crystal registers |
| `hw` | high-water marks of the UART ring, main-loop time, queues and pending entries, when one rises |
| `stk` | stack high-water marks (main, `irq`), with a warning if one passes 3/4 of its size |
| `air` | after bursts: holds while the channel was in use (`yld`), those that ended in a frame (`ylf`), ms of the others (`yms`), other tags heard, burst airtime, estimated cost to others, learned waits (`dn`, `dx`) |

Besides these: `cfg` when SET_CONFIG changes a setting (channel, power, lock, LED, and the derived
maxima `ckb`, `tags`, `pend`, `ofr`, `prof`); `blk <n> parts ...` for every block request and
`rereq blk ...` when a tag asks again for parts of the burst it was just sent; and warnings: once a
minute for late requests and for frames ignored by the inventory lock, and when block data from the S3
came too late for its burst. An `AP_DEBUG=1` build adds `ping`, `joined` and `fo` lines (`fo` per
check-in: crystal cap code and frequency offset) and the `nf` noise histogram on the `rf` line.

## Tests

```
make -C test
```

builds `src/ap.c`, `v2link.c` and `blockcache.c` for the PC with `test/host_hal.c` (virtual clock,
serial line and radio) under AddressSanitizer and UBSan, in several arena geometries and with
`AP_DEBUG=1`, and runs `test/test_ap.py`, which plays the S3 (protocol v2 written from the
specification) and the tags. `test/test_stackdepth.py` tests `tools/stackdepth.py`, and
`test/test_string_min.c` tests `src/tlsr/string_min.c`. `src/tlsr/hal_tlsr.c` is not covered. The
tests need Linux, gcc with AddressSanitizer and UBSan (libasan, libubsan) and Python 3.

## Layout

```
src/ap.c, ap.h          AP logic: tag store, block requests and bursts, v2 requests and events, diagnostics
src/v2link.c, .h        protocol v2, radio side: framing, duplicates, event and LOG queue, retransmission
src/blockcache.c, .h    block cache and the arena it shares with the tag store
src/proto.h             tag <-> AP air protocol
src/hal.h               hardware interface: time, stacks, arena, UART, radio, LED
src/tlsr/hal_tlsr.c     hal.h on the TLSR8258: radio, UART, interrupts, board start
src/tlsr/main.c         entry point
src/tlsr/string_min.c   the C library functions, in place of the SDK's string.c
src/tlsr/*_cfg.h        SDK configuration and board_8258_diy.h, from the TLSR tag firmware
boot_8258_ap.link       linker script: stacks, arena and the RAM checks
tools/stackdepth.py     static stack depth from the listing, run by the makefile
test/                   host build of the AP logic and its tests
../../oepl-radio-v2.h   protocol v2 definitions, shared with the S3 and the C6/H2 firmware
```
