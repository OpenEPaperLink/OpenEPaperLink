#include <Arduino.h>

#include "usbflasher.h"

#ifdef HAS_USB
// flashing via 2nd USB port
#include "USB.h"
USBCDC USBSerial;
#define cmdSerial USBSerial
#else
#define cmdSerial Serial
#endif

// #include "esp32-hal-tinyusb.h"
#include "flasher.h"
#include "leds.h"
#include "powermgt.h"
#include "settings.h"
#include "swd.h"
#include "web.h"
#include "webflasher.h"
#include "zbs_interface.h"

QueueHandle_t flasherCmdQueue;

uint32_t usbConnectedStartTime = 0;
extern bool serialPassthroughState;

#define FLASHER_WAIT_A 0
#define FLASHER_WAIT_T 1
#define FLASHER_WAIT_CMD 2
#define FLASHER_WAIT_LEN 3
#define FLASHER_WAIT_DATA 4
#define FLASHER_WAIT_CRCH 5
#define FLASHER_WAIT_CRCL 6
#define FLASHER_RESET 7

struct flasherCommand {
    uint8_t command = 0;
    uint32_t len = 0;
    uint8_t* data = nullptr;
};

int8_t powerPins[] = FLASHER_AP_POWER;
#ifdef HAS_EXT_FLASHER
int8_t powerPins2[] = FLASHER_EXT_POWER;
int8_t powerPins3[] = FLASHER_ALT_POWER;
#endif

bool autoFlash(flasher* f) {
    f->getFirmwareMD5();

    if (f->findTagByMD5()) {
        // this tag currently contains original firmware, found its fingerprint
        cmdSerial.printf("Found original firmware tag, recognized its fingerprint (%s)\r\n", f->md5char);
        f->readInfoBlock();
        f->getFirmwareMac();
        f->prepareInfoBlock();
        f->writeInfoBlock();
        cmdSerial.printf("Attempting to perform a flash...\r\n");
        if (f->writeFlashFromPack("/Tag_FW_Pack.bin", f->tagtype)) {
            cmdSerial.printf("Successfully flashed the tag!\r\n");
            return true;
        } else {
            cmdSerial.printf("Couldn't flash the tag, for some reason...\r\n");
        }
    } else if (f->getInfoBlockMD5()) {
        // did find an infoblock MD5 that looks valid
        if (f->findTagByMD5()) {
            // did find the md5 in the database
            cmdSerial.printf("Found an already-flashed tag, recognized its fingerprint (%s)\r\n", f->md5char);
            f->getInfoBlockMac();
            f->getInfoBlockType();
            f->readInfoBlock();
            cmdSerial.printf("Attempting to perform a flash...\r\n");
            if (f->writeFlashFromPack("/Tag_FW_Pack.bin", f->tagtype)) {
                cmdSerial.printf("Successfully flashed the tag!\r\n");
                return true;
            } else {
                cmdSerial.printf("Couldn't flash the tag, for some reason...\r\n");
            }
        } else {
            // couldn't find the md5 from the infoblock
            cmdSerial.printf("Found an already-flashed tag, but we couldn't find its fingerprint (%s) in the database\r\n", f->md5char);
            return false;
        }
    } else {
        // We couldn't recognize the tag from it's fingerprint...
        cmdSerial.printf("Found a tag but didn't recognize its fingerprint\r\n", f->md5char);
        f->backupFlash();
        cmdSerial.printf("Saved this MD5 binary to filesystem\r\n");
    }
    return false;
}

void sendFlasherAnswer(uint8_t answer_cmd, uint8_t* ans_buff, uint32_t len, uint8_t transportType) {
    uint8_t* answer_buffer = (uint8_t*)calloc(3 + 2 + 2 + len + 2 + 13, 1);
    if (answer_buffer == nullptr) return;
    uint32_t CRC_value = 0xAB34;
    answer_buffer[0] = 'A';
    answer_buffer[1] = 'T';
    answer_buffer[2] = answer_cmd;
    CRC_value += answer_cmd;

    for (uint8_t c = 0; c < 4; c++) {
        answer_buffer[3 + c] = (uint8_t)(len >> (24 - (c * 8)));
        CRC_value += answer_buffer[3 + c];
    }

    for (int i = 0; i < len; i++) {
        answer_buffer[7 + i] = ans_buff[i];
        CRC_value += ans_buff[i];
    }

    answer_buffer[3 + 2 + 2 + len] = CRC_value >> 8;
    answer_buffer[3 + 2 + 2 + len + 1] = CRC_value;
    if (transportType == TRANSPORT_USB) {
        cmdSerial.write(answer_buffer, 3 + 2 + 2 + len + 2);
    } else {
        sendDataToClient(answer_buffer, 3 + 2 + 2 + len + 2);
    }
    // for(uint16_t c = 0; c< 3+2+2+len+2; c++){

    //}
    free(answer_buffer);
}

void flasherDataHandler(uint8_t* data, size_t len, uint8_t transportType) {
    static struct flasherCommand* cmd = nullptr;
    static uint8_t flasherSerialState = FLASHER_WAIT_A;
    static uint32_t flasherCmdDataIndex = 0;
    static uint16_t flasherCRC = 0xAB34;
    static uint32_t flasherLastCmd = 0;
    static uint8_t curLenIndex = 0;

    if ((flasherSerialState != FLASHER_WAIT_A) && (millis() - flasherLastCmd >= 3000)) {
        flasherSerialState = FLASHER_RESET;
    }

    while (len--) {
        uint8_t usbbyte = *(data++);
        switch (flasherSerialState) {
            case FLASHER_RESET:
                if (transportType == TRANSPORT_TCP && cmd != nullptr) {
                    if (cmd->data != nullptr) {
                        free(cmd->data);
                        cmd->data = nullptr;
                    }
                    delete cmd;
                    cmd = nullptr;
                }
                flasherSerialState = FLASHER_WAIT_A;
            case FLASHER_WAIT_A:
                if (usbbyte == 'A') {
                    flasherSerialState = FLASHER_WAIT_T;
                    flasherLastCmd = millis();
                } else {
                    // enterConsoleMode();
                }
                break;
            case FLASHER_WAIT_T:
                if (usbbyte == 'T') {
                    flasherSerialState = FLASHER_WAIT_CMD;
                    cmd = new flasherCommand;
                    flasherCRC = 0xAB34;
                    flasherCmdDataIndex = 0;
                } else {
                    flasherSerialState = FLASHER_RESET;
                }
                break;
            case FLASHER_WAIT_CMD:
                cmd->command = usbbyte;
                flasherCRC += usbbyte;
                flasherSerialState = FLASHER_WAIT_LEN;
                curLenIndex = 0;
                break;
            case FLASHER_WAIT_LEN:
                flasherCRC += usbbyte;
                cmd->len |= ((uint32_t)usbbyte) << (24 - (8 * curLenIndex));
                curLenIndex++;
                if (curLenIndex == sizeof(cmd->len)) {
                    if (cmd->len) {
                        // not 0
                        cmd->data = (uint8_t*)calloc(cmd->len, 1);
                        if (cmd->data == nullptr) {
                            delete cmd;
                            cmd = nullptr;
                            flasherSerialState = FLASHER_WAIT_A;
                        } else {
                            flasherSerialState = FLASHER_WAIT_DATA;
                        }
                    } else {
                        // 0 len, so skip to CRC immediately
                        flasherSerialState = FLASHER_WAIT_CRCH;
                    }
                }
                break;
            case FLASHER_WAIT_DATA:
                flasherCRC += usbbyte;
                cmd->data[flasherCmdDataIndex++] = usbbyte;
                if (flasherCmdDataIndex == cmd->len) {
                    flasherSerialState = FLASHER_WAIT_CRCH;
                }
                break;
            case FLASHER_WAIT_CRCH:
                flasherCRC -= ((uint16_t)usbbyte << 8);
                flasherSerialState = FLASHER_WAIT_CRCL;
                break;
            case FLASHER_WAIT_CRCL:
                flasherCRC -= ((uint16_t)usbbyte);
                if (flasherCRC) {
                    flasherSerialState = FLASHER_RESET;
                    wsSerial("failed CRC");
                } else {
                    if (transportType == TRANSPORT_USB) {
                        BaseType_t queuestatus = xQueueSend(flasherCmdQueue, &cmd, 0);
                    } else {
                        processFlasherCommand(cmd, TRANSPORT_TCP);
                    }
                    flasherSerialState = FLASHER_RESET;
                }
                break;
        }
    }
}

void resetFlasherState() {
    if (serialPassthroughState) {
        Serial2.end();
    }
    serialPassthroughState = false;
}

#ifdef HAS_USB
static void usbEventCallback(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data) {
    if (event_base == ARDUINO_USB_EVENTS) {
        arduino_usb_event_data_t* data = (arduino_usb_event_data_t*)event_data;
        switch (event_id) {
            case ARDUINO_USB_STARTED_EVENT:
                Serial.println("USB PLUGGED");
                resetFlasherState();
                break;
            case ARDUINO_USB_STOPPED_EVENT:
                Serial.println("USB UNPLUGGED");
                resetFlasherState();
                break;
            case ARDUINO_USB_SUSPEND_EVENT:
                Serial.printf("USB SUSPENDED: remote_wakeup_en: %u\r\n", data->suspend.remote_wakeup_en);
                break;
            case ARDUINO_USB_RESUME_EVENT:
                Serial.println("USB RESUMED");
                break;
            default:
                ets_printf("other USB event %d\n", event_id);
                break;
        }
    } else if (event_base == ARDUINO_USB_CDC_EVENTS) {
        arduino_usb_cdc_event_data_t* data = (arduino_usb_cdc_event_data_t*)event_data;
        switch (event_id) {
            case ARDUINO_USB_CDC_CONNECTED_EVENT:
                ets_printf("CDC CONNECTED\n");
                resetFlasherState();
                usbConnectedStartTime = millis();
                break;
            case ARDUINO_USB_CDC_DISCONNECTED_EVENT:
                ets_printf("CDC DISCONNECTED\n");
                resetFlasherState();
                break;
            case ARDUINO_USB_CDC_LINE_STATE_EVENT:
                ets_printf("CDC LINE STATE: dtr: %u, rts: %u\n", data->line_state.dtr, data->line_state.rts);
                if (data->line_state.dtr == 0) resetFlasherState();
                break;
            case ARDUINO_USB_CDC_LINE_CODING_EVENT:
                ets_printf("CDC LINE CODING: bit_rate: %u, data_bits: %u, stop_bits: %u, parity: %u\n", data->line_coding.bit_rate, data->line_coding.data_bits, data->line_coding.stop_bits, data->line_coding.parity);
                resetFlasherState();
                break;
            case ARDUINO_USB_CDC_RX_EVENT:
                // Serial.printf("CDC RX [%u]:", data->rx.len);
                {
                    uint8_t buf[data->rx.len];
                    size_t len = cmdSerial.read(buf, data->rx.len);
                    if (serialPassthroughState) {
                        Serial2.write(buf, len);
                    } else {
                        flasherDataHandler(buf, len, TRANSPORT_USB);
                    }
                }
                break;
            case ARDUINO_USB_CDC_RX_OVERFLOW_EVENT:
                Serial.printf("CDC RX Overflow of %d bytes", data->rx_overflow.dropped_bytes);
                break;

            default:
                break;
        }
    }
}
#endif

typedef enum {
    CMD_GET_VERSION = 1,
    CMD_RESET_ESP = 2,
    CMD_RESET = 11,
    CMD_SET_POWER = 13,

    CMD_ERASE_FLASH = 26,
    CMD_ERASE_INFOPAGE = 27,
    CMD_SAVE_MAC_FROM_FW = 40,
    CMD_PASS_THROUGH = 50,

    CMD_SELECT_ZBS243 = 60,
    CMD_SELECT_NRF82511 = 61,

    CMD_SELECT_PORT = 70,

    CMD_READ_INFOPAGE = 80,
    CMD_READ_FLASH = 81,
    CMD_WRITE_INFOPAGE = 82,
    CMD_WRITE_FLASH = 83,
    CMD_AUTOFLASH = 87,
    CMD_COMPLETE = 88,

    CMD_WRITE_ERROR = 99,

} ZBS_UART_PROTO;
uint32_t FLASHER_VERSION = 0x00000031;

#define CONTROLLER_ZBS243 0
#define CONTROLLER_NRF82511 1
uint8_t selectedController = 0;
uint8_t selectedFlasherPort;
uint32_t currentFlasherOffset;
flasher* zbsflasherp = nullptr;
nrfswd* nrfflasherp = nullptr;

void processFlasherCommand(struct flasherCommand* cmd, uint8_t transportType) {
    uint8_t* tempbuffer;
    uint8_t temp_buff[16];
    uint32_t spi_speed = 0;
    uint8_t powerPinCount = 1;
    static uint32_t curspeed = 0;
    uint8_t numPowerPins;

#ifdef HAS_RGB_LED
    shortBlink(CRGB::White);
#else
    quickBlink(2);
#endif

    switch (cmd->command) {
        case CMD_GET_VERSION:
            wsSerial("> get version");
            temp_buff[0] = FLASHER_VERSION >> 24;
            temp_buff[1] = FLASHER_VERSION >> 16;
            temp_buff[2] = FLASHER_VERSION >> 8;
            temp_buff[3] = FLASHER_VERSION;
            sendFlasherAnswer(cmd->command, temp_buff, 4, transportType);
            break;
        case CMD_RESET_ESP:
            wsSerial("reset");
            sendFlasherAnswer(cmd->command, NULL, 0, transportType);
            delay(100);
            ESP.restart();
            break;
        case CMD_SET_POWER:
            wsSerial("> power");
            switch (selectedFlasherPort) {
                case 0:
                    numPowerPins = sizeof(powerPins);
                    powerControl(cmd->data[0], (uint8_t*)powerPins, numPowerPins);
                    break;
                case 1:
                    numPowerPins = sizeof(powerPins2);
                    powerControl(cmd->data[0], (uint8_t*)powerPins2, numPowerPins);
                    break;
                case 2:
                    numPowerPins = sizeof(powerPins3);
                    powerControl(cmd->data[0], (uint8_t*)powerPins3, numPowerPins);
                    break;
            }
            sendFlasherAnswer(CMD_SET_POWER, NULL, 0, transportType);
            break;
        case CMD_RESET:
            wsSerial("> reset tag");
            if (zbsflasherp != nullptr) {
                zbsflasherp->zbs->reset();
                delete zbsflasherp;
                zbsflasherp = nullptr;
            }
            if (nrfflasherp != nullptr) {
                nrfflasherp->nrf_soft_reset();
                delete nrfflasherp;
                nrfflasherp = nullptr;
            }
            sendFlasherAnswer(CMD_RESET, NULL, 0, transportType);
            break;
        case CMD_ERASE_FLASH:
            wsSerial("> erase flash");
            if (selectedController == CONTROLLER_NRF82511) {
            } else if (selectedController == CONTROLLER_ZBS243) {
                if (zbsflasherp == nullptr) return;
                zbsflasherp->zbs->erase_flash();
            }
            sendFlasherAnswer(CMD_ERASE_FLASH, NULL, 0, transportType);
            break;
        case CMD_ERASE_INFOPAGE:
            wsSerial("> erase infopage");
            if (selectedController == CONTROLLER_NRF82511) {
                nrfflasherp->erase_uicr();
            } else if (selectedController == CONTROLLER_ZBS243) {
                if (zbsflasherp == nullptr) return;
                zbsflasherp->zbs->erase_infoblock();
            }
            sendFlasherAnswer(CMD_ERASE_INFOPAGE, NULL, 0, transportType);
            break;
        case CMD_SELECT_PORT:
            wsSerial("> select port");
            selectedFlasherPort = cmd->data[0];
            Serial.printf("Port selected = %d\r\n", cmd->data[0]);
            break;
        case CMD_SELECT_ZBS243:
            wsSerial("> connect zbs");
            zbsflasherp = new flasher;
            temp_buff[0] = zbsflasherp->connectTag(selectedFlasherPort);
            sendFlasherAnswer(CMD_SELECT_ZBS243, temp_buff, 1, transportType);
            currentFlasherOffset = 0;
            selectedController = CONTROLLER_ZBS243;
            break;
        case CMD_SELECT_NRF82511:
            wsSerial("> connect nrf");
            switch (selectedFlasherPort) {
                case 0:
                    numPowerPins = sizeof(powerPins);
                    powerControl(true, (uint8_t*)powerPins, numPowerPins);
                    nrfflasherp = new nrfswd(FLASHER_AP_MISO, FLASHER_AP_CLK);
                    break;
                case 1:
                    numPowerPins = sizeof(powerPins2);
                    powerControl(true, (uint8_t*)powerPins2, numPowerPins);
                    nrfflasherp = new nrfswd(FLASHER_EXT_MISO, FLASHER_EXT_CLK);
                    break;
                case 2:
                    numPowerPins = sizeof(powerPins3);
                    powerControl(true, (uint8_t*)powerPins3, numPowerPins);
                    nrfflasherp = new nrfswd(FLASHER_ALT_MISO, FLASHER_ALT_CLK);
                    break;
            }
            nrfflasherp->init();
            temp_buff[0] = (nrfflasherp->isConnected && !nrfflasherp->isLocked);
            sendFlasherAnswer(CMD_SELECT_NRF82511, temp_buff, 1, transportType);
            currentFlasherOffset = 0;
            selectedController = CONTROLLER_NRF82511;
            break;
        case CMD_READ_FLASH:
            wsSerial("> read flash");
            uint8_t* bufferp;
            uint32_t cur_len;
            if (selectedController == CONTROLLER_NRF82511) {
                if (nrfflasherp == nullptr) return;
                if (currentFlasherOffset >= nrfflasherp->nrf_info.flash_size) {
                    sendFlasherAnswer(CMD_COMPLETE, temp_buff, 1, transportType);
                } else {
                    bufferp = (uint8_t*)malloc(1024);
                    if (bufferp == nullptr) return;
                    cur_len = (nrfflasherp->nrf_info.flash_size - currentFlasherOffset >= 1024) ? 1024 : nrfflasherp->nrf_info.flash_size - currentFlasherOffset;
                    nrfflasherp->nrf_read_bank(currentFlasherOffset, (uint32_t*)bufferp, cur_len);
                    currentFlasherOffset += cur_len;
                    sendFlasherAnswer(CMD_READ_FLASH, bufferp, cur_len, transportType);
                    if (bufferp != nullptr) free(bufferp);
                }
            } else if (selectedController == CONTROLLER_ZBS243) {
                if (zbsflasherp == nullptr) return;
                if (currentFlasherOffset >= 65536) {
                    sendFlasherAnswer(CMD_COMPLETE, temp_buff, 1, transportType);
                } else {
                    bufferp = (uint8_t*)malloc(1024);
                    if (bufferp == nullptr) return;
                    cur_len = (65536 - currentFlasherOffset >= 1024) ? 1024 : 65536 - currentFlasherOffset;
                    zbsflasherp->readBlock(currentFlasherOffset, bufferp, cur_len, false);
                    currentFlasherOffset += cur_len;
                    sendFlasherAnswer(CMD_READ_FLASH, bufferp, cur_len, transportType);
                    if (bufferp != nullptr) free(bufferp);
                }
            }
            break;
        case CMD_READ_INFOPAGE:
            wsSerial("> read infopage");
            uint8_t* ibufferp;
            uint32_t icur_len;
            if (selectedController == CONTROLLER_NRF82511) {
                if (nrfflasherp == nullptr) return;
                if (currentFlasherOffset >= 4096) {
                    sendFlasherAnswer(CMD_COMPLETE, temp_buff, 1, transportType);
                } else {
                    ibufferp = (uint8_t*)malloc(1024);
                    if (ibufferp == nullptr) return;
                    icur_len = (4096 - currentFlasherOffset >= 256) ? 256 : 4096 - currentFlasherOffset;
                    nrfflasherp->nrf_read_bank(0x10001000 + currentFlasherOffset, (uint32_t*)ibufferp, icur_len);
                    currentFlasherOffset += icur_len;
                    sendFlasherAnswer(CMD_READ_INFOPAGE, ibufferp, icur_len, transportType);
                    if (ibufferp != nullptr) free(ibufferp);
                }
            } else if (selectedController == CONTROLLER_ZBS243) {
                if (zbsflasherp == nullptr) return;
                if (currentFlasherOffset >= 1024) {
                    sendFlasherAnswer(CMD_COMPLETE, temp_buff, 1, transportType);
                } else {
                    ibufferp = (uint8_t*)malloc(1024);
                    if (ibufferp == nullptr) return;
                    icur_len = (1024 - currentFlasherOffset >= 256) ? 256 : 1024 - currentFlasherOffset;
                    zbsflasherp->readBlock(currentFlasherOffset, ibufferp, icur_len, true);
                    currentFlasherOffset += icur_len;
                    sendFlasherAnswer(CMD_READ_INFOPAGE, ibufferp, icur_len, transportType);
                    if (ibufferp != nullptr) free(ibufferp);
                }
            }
            break;
        case CMD_WRITE_FLASH:
            Serial.println("> write flash");
            if (selectedController == CONTROLLER_NRF82511) {
                if (nrfflasherp == nullptr) return;
                if (currentFlasherOffset >= nrfflasherp->nrf_info.flash_size) {
                    sendFlasherAnswer(CMD_COMPLETE, temp_buff, 1, transportType);
                } else {
                    for (uint32_t c = currentFlasherOffset; c < (currentFlasherOffset + cmd->len);) {
                        // very ugly and naive way to find out what page we're in, and erase all relevant pages before writing
                        if (c % nrfflasherp->nrf_info.codepage_size == 0) {
                            nrfflasherp->erase_page(c);
                            Serial.printf("Erasing page %lu\r\n", c);
                            c += nrfflasherp->nrf_info.codepage_size;
                        } else {
                            c++;
                        }
                    }
                    uint8_t result = nrfflasherp->nrf_write_bank(currentFlasherOffset, (uint32_t*)cmd->data, cmd->len);
                    Serial.printf("wrote page offset %lu to nrf\r\n", currentFlasherOffset);
                    currentFlasherOffset += cmd->len;
                    if (result == 3) {
                        sendFlasherAnswer(CMD_WRITE_ERROR, NULL, 0, transportType);
                        return;
                    }
                    sendFlasherAnswer(CMD_WRITE_FLASH, NULL, 0, transportType);
                }
            } else if (selectedController == CONTROLLER_ZBS243) {
                if (zbsflasherp == nullptr) return;
                if (currentFlasherOffset >= 65536) {
                    sendFlasherAnswer(CMD_COMPLETE, temp_buff, 1, transportType);
                } else {
                    zbsflasherp->writeBlock(currentFlasherOffset, cmd->data, cmd->len, false);
                    currentFlasherOffset += cmd->len;
                    sendFlasherAnswer(CMD_WRITE_FLASH, NULL, 0, transportType);
                }
            }
            break;
        case CMD_WRITE_INFOPAGE:
            wsSerial("> write infopage");
            if (selectedController == CONTROLLER_NRF82511) {
                if (nrfflasherp == nullptr) return;
                if (currentFlasherOffset >= 4096) {
                    sendFlasherAnswer(CMD_COMPLETE, temp_buff, 1, transportType);
                } else {
                    uint8_t result =  nrfflasherp->nrf_write_bank(0x10001000 + currentFlasherOffset, (uint32_t*)cmd->data, cmd->len);
                    Serial.printf("wrote infopage to nrf\r\n");
                    currentFlasherOffset += cmd->len;
                    if (result == 3) {
                        sendFlasherAnswer(CMD_WRITE_ERROR, NULL, 0, transportType);
                        return;
                    }
                    sendFlasherAnswer(CMD_WRITE_INFOPAGE, NULL, 0, transportType);
                }
            } else if (selectedController == CONTROLLER_ZBS243) {
                if (zbsflasherp == nullptr) return;
                if (currentFlasherOffset >= 1024) {
                    sendFlasherAnswer(CMD_COMPLETE, temp_buff, 1, transportType);
                } else {
                    zbsflasherp->writeBlock(currentFlasherOffset, cmd->data, cmd->len, true);
                    currentFlasherOffset += cmd->len;
                    sendFlasherAnswer(CMD_WRITE_INFOPAGE, NULL, 0, transportType);
                }
            }
            break;
        case CMD_PASS_THROUGH:
            wsSerial("> pass through");

            extern bool rxSerialStopTask2;
            rxSerialStopTask2 = true;
            vTaskDelay(500 / portTICK_PERIOD_MS);

            if (Serial2) Serial2.end();
            Serial2.begin(115200, SERIAL_8N1, FLASHER_EXT_RXD, FLASHER_EXT_TXD);
            cmdSerial.println(">>>");
            serialPassthroughState = true;
            break;
        case CMD_AUTOFLASH:
            wsSerial("> autoflash");
            if (selectedController == CONTROLLER_ZBS243) {
                autoFlash(zbsflasherp);
                zbsflasherp->zbs->reset();
                delete zbsflasherp;
                zbsflasherp = nullptr;
                cmdSerial.write(0x04);
            } else {
                cmdSerial.println("Not yet implemented!");
            }
            break;
    }
}

uint32_t lastCmdTimeStamp = 0;
#define USBFLASHER_CONNECTION_TIMEOUT 5000
void flasherCommandTimeout() {
    // this is triggered if no command has been received for a couple of seconds; this makes sure everything is cleaned up when the USB connection is lost
    if (zbsflasherp != nullptr) {
        delete zbsflasherp;
        zbsflasherp = nullptr;
    }
    if (nrfflasherp != nullptr) {
        delete nrfflasherp;
        nrfflasherp = nullptr;
    }
    lastCmdTimeStamp = 0;
}

void tagDebugPassthrough() {
    // static String accumulatedData = "";
    // static unsigned long flushTimer = 0;
    uint16_t len = Serial2.available();

    if (len > 0) {
        uint8_t* buf = (uint8_t*)malloc(len);
        Serial2.readBytes(buf, len);
        cmdSerial.printf("%d bytes: ", len);
        cmdSerial.write(buf, len);
        cmdSerial.print("\r\n");
        //String dataString((char*)buf, len);
        //wsSerial(dataString, "cyan");
        //  accumulatedData += dataString;
        free(buf);
    }
    /*
    if (millis() - flushTimer > 500 && accumulatedData.length() > 0) {
        // wsSerial("*", "cyan");
        flushTimer = millis();
        accumulatedData = "";
    }
    */
}

#ifdef HAS_USB
void usbFlasherTask(void* parameter) {
    flasherCmdQueue = xQueueCreate(10, sizeof(struct flasherCommand*));

#ifndef ARDUINO_USB_MODE
#error This ESP32 SoC has no Native USB interface
#elif ARDUINO_USB_MODE == 1
#warning This sketch should be used when USB is in OTG mode. Wrong USB mode is in use, check settings in platformio.ini
#endif
    USB.onEvent(usbEventCallback);
    cmdSerial.onEvent(usbEventCallback);
    cmdSerial.setTimeout(1000);
    USB.productName("OpenEpaperLink-flasher");
    USB.begin();
    cmdSerial.begin();
    Serial.println("Task started");
    uint32_t notificationValue;
    struct flasherCommand* cmd = nullptr;
    while (true) {
        while (serialPassthroughState) {
            tagDebugPassthrough();
            vTaskDelay(1 / portTICK_PERIOD_MS);
        }

        BaseType_t queuereceive = xQueueReceive(flasherCmdQueue, &cmd, 1000 / portTICK_PERIOD_MS);  // timeout every second to make sure the timeout gets triggered after a while
        if (queuereceive == pdTRUE) {
            processFlasherCommand(cmd, TRANSPORT_USB);
            lastCmdTimeStamp = millis();
            if (cmd->data != nullptr) {
                free(cmd->data);
                cmd->data = nullptr;
            }
            delete cmd;
            cmd = nullptr;
        } else {
            if (lastCmdTimeStamp) {
                if (millis() - lastCmdTimeStamp > USBFLASHER_CONNECTION_TIMEOUT)
                flasherCommandTimeout();
            }
        }

        vTaskDelay(50 / portTICK_PERIOD_MS);
    }
}
#endif

