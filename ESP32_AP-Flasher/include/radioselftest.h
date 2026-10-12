#pragma once

// S3 <-> radio wiring self-test (task #32, docs/design/radio-selftest.md). Checks every line between
// the S3 and an ESP32-C6/H2 radio and says which one is wrong. S3 only: it uses what the radio does
// anyway (ROM text after a reset, RES>/RDY> from the OEPL firmware, ACK> to RDY?, the ROM's serial
// bootloader).

#include <Arduino.h>

// All pins named from the S3's side; -1 = the board has no such line.
struct RadioPins {
    int8_t protoTx;  // S3 TX -> radio protocol RX
    int8_t protoRx;  // radio protocol TX -> S3 RX
    int8_t dbgTx;    // S3 TX -> radio UART0 RX (ROM bootloader)
    int8_t dbgRx;    // radio UART0 TX (ROM text, debug log) -> S3 RX
    int8_t reset;    // S3 -> radio EN/RESET
    int8_t prog;     // S3 -> radio GPIO9 (boot mode)
};

// The pins this AP uses: protocol UART from the build, debug/RESET/PROG from the flash pinout config.
RadioPins radioPinsFromConfig();

// Stops the AP service, tests the wiring, then tries to bring the radio online again (returns
// whether that worked). Takes 5-10 s. Progress and result go to the web UI console (wsSerial), the
// serial console and log.txt; the result also to radioSelfTestResult().
bool radioSelfTest(const RadioPins& pins);

// The same in a task of its own, for the web UI. False when a test is already running or the
// board has no ESP radio.
bool startRadioSelfTest(const RadioPins& pins);

// Last result as JSON: {"running":bool, "ok":bool, "messages":[...], "observations":{...}}.
String radioSelfTestResult();
