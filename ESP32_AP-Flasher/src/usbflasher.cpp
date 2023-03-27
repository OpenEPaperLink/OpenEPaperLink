#include <Arduino.h>

#include "USB.h"
#include "powermgt.h"
#include "settings.h"
#include "zbs_interface.h"

USBCDC USBSerial;

QueueHandle_t flasherCmdQueue;

#define FLASHER_WAIT_A 0
#define FLASHER_WAIT_T 1
#define FLASHER_WAIT_CMD 2
#define FLASHER_WAIT_LEN 3
#define FLASHER_WAIT_DATA 4
#define FLASHER_WAIT_CRCH 5
#define FLASHER_WAIT_CRCL 6

struct flasherCommand {
    uint8_t command = 0;
    uint8_t len = 0;
    uint8_t* data = nullptr;
};

void sendFlasherAnswer(uint8_t answer_cmd, uint8_t* ans_buff, uint8_t len) {
    uint8_t* answer_buffer = (uint8_t*)calloc(2 + 2 + len + 2, 1);
    if (answer_buffer == nullptr) return;
    uint32_t CRC_value = 0xAB34;
    answer_buffer[0] = 'A';
    answer_buffer[1] = 'T';
    answer_buffer[2] = answer_cmd;
    CRC_value += answer_cmd;
    answer_buffer[3] = len;
    CRC_value += len;
    for (int i = 0; i < len; i++) {
        answer_buffer[4 + i] = ans_buff[i];
        CRC_value += ans_buff[i];
    }
    answer_buffer[2 + 2 + len] = CRC_value >> 8;
    answer_buffer[2 + 2 + len + 1] = CRC_value;
    USBSerial.write(answer_buffer, 2 + 2 + len + 2);
    free(answer_buffer);
}

void flasherUartHandler(uint8_t* data, uint8_t len) {
    static struct flasherCommand* cmd;
    static uint8_t flasherSerialState = FLASHER_WAIT_A;
    static uint8_t flasherCmdDataIndex = 0;
    static uint16_t flasherCRC = 0xAB34;
    static uint32_t flasherLastCmd = 0;

    if ((flasherSerialState != FLASHER_WAIT_A) && (millis() - flasherLastCmd >= 225)) {
        flasherSerialState = FLASHER_WAIT_A;
    }

    while (len--) {
        uint8_t usbbyte = *(data++);
        switch (flasherSerialState) {
            case FLASHER_WAIT_A:
                if (usbbyte == 'A') {
                    flasherSerialState = FLASHER_WAIT_T;
                    flasherLastCmd = millis();
                }
                break;
            case FLASHER_WAIT_T:
                if (usbbyte == 'T') {
                    flasherSerialState = FLASHER_WAIT_CMD;
                    cmd = new flasherCommand;
                    flasherCRC = 0xAB34;
                    flasherCmdDataIndex = 0;
                } else {
                    flasherSerialState = FLASHER_WAIT_A;
                }
                break;
            case FLASHER_WAIT_CMD:
                cmd->command = usbbyte;
                flasherCRC += usbbyte;
                flasherSerialState = FLASHER_WAIT_LEN;
                break;
            case FLASHER_WAIT_LEN:
                flasherCRC += usbbyte;
                if (usbbyte) {
                    cmd->len = usbbyte;
                    cmd->data = (uint8_t*)calloc(usbbyte, 1);
                    flasherSerialState = FLASHER_WAIT_DATA;
                } else {
                    flasherSerialState = FLASHER_WAIT_CRCH;
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
                    Serial.printf("CRC failed for flasher command :( %04X\n", flasherCRC);
                    cmd = nullptr;
                } else {
                    BaseType_t queuestatus = xQueueSend(flasherCmdQueue, &cmd, 0);
                    if (queuestatus == pdFALSE) {
                        if (cmd->data != nullptr) free(cmd->data);
                        delete cmd;
                    }
                    cmd = nullptr;
                }
                flasherSerialState = FLASHER_WAIT_A;
                break;
        }
    }
}

static void usbEventCallback(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data) {
    if (event_base == ARDUINO_USB_EVENTS) {
        arduino_usb_event_data_t* data = (arduino_usb_event_data_t*)event_data;
        switch (event_id) {
            case ARDUINO_USB_STARTED_EVENT:
                Serial.println("USB PLUGGED");
                break;
            case ARDUINO_USB_STOPPED_EVENT:
                Serial.println("USB UNPLUGGED");
                break;
            case ARDUINO_USB_SUSPEND_EVENT:
                Serial.printf("USB SUSPENDED: remote_wakeup_en: %u\n", data->suspend.remote_wakeup_en);
                break;
            case ARDUINO_USB_RESUME_EVENT:
                Serial.println("USB RESUMED");
                break;

            default:
                break;
        }
    } else if (event_base == ARDUINO_USB_CDC_EVENTS) {
        arduino_usb_cdc_event_data_t* data = (arduino_usb_cdc_event_data_t*)event_data;
        switch (event_id) {
            case ARDUINO_USB_CDC_CONNECTED_EVENT:
                Serial.println("CDC CONNECTED");
                break;
            case ARDUINO_USB_CDC_DISCONNECTED_EVENT:
                Serial.println("CDC DISCONNECTED");
                break;
            case ARDUINO_USB_CDC_LINE_STATE_EVENT:
                Serial.printf("CDC LINE STATE: dtr: %u, rts: %u\n", data->line_state.dtr, data->line_state.rts);
                break;
            case ARDUINO_USB_CDC_LINE_CODING_EVENT:
                Serial.printf("CDC LINE CODING: bit_rate: %u, data_bits: %u, stop_bits: %u, parity: %u\n", data->line_coding.bit_rate, data->line_coding.data_bits, data->line_coding.stop_bits, data->line_coding.parity);
                break;
            case ARDUINO_USB_CDC_RX_EVENT:
                // Serial.printf("CDC RX [%u]:", data->rx.len);
                {
                    uint8_t buf[data->rx.len];
                    size_t len = USBSerial.read(buf, data->rx.len);
                    flasherUartHandler(buf, len);
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

typedef enum {
    CMD_GET_VERSION = 1,
    CMD_RESET_ESP = 2,
    CMD_ZBS_BEGIN = 10,
    CMD_RESET_ZBS = 11,
    CMD_SELECT_PAGE = 12,
    CMD_SET_POWER = 13,
    CMD_READ_RAM = 20,
    CMD_WRITE_RAM = 21,
    CMD_READ_FLASH = 22,
    CMD_WRITE_FLASH = 23,
    CMD_READ_SFR = 24,
    CMD_WRITE_SFR = 25,
    CMD_ERASE_FLASH = 26,
    CMD_ERASE_INFOBLOCK = 27,
    CMD_SAVE_MAC_FROM_FW = 40,
    CMD_PASS_THROUGH = 50,
} ZBS_UART_PROTO;
uint32_t FLASHER_VERSION = 0x0000002F;

static class ZBS_interface* zbs = nullptr;

void processFlasherCommand(struct flasherCommand* cmd) {
    uint8_t* tempbuffer;
    uint8_t temp_buff[16];
    uint32_t spi_speed = 0;
    static uint32_t curspeed = 0;

    switch (cmd->command) {
        case CMD_GET_VERSION:
            temp_buff[0] = FLASHER_VERSION >> 24;
            temp_buff[1] = FLASHER_VERSION >> 16;
            temp_buff[2] = FLASHER_VERSION >> 8;
            temp_buff[3] = FLASHER_VERSION;
            sendFlasherAnswer(cmd->command, temp_buff, 4);
            break;
        case CMD_RESET_ESP:
            sendFlasherAnswer(cmd->command, NULL, 0);
            delay(100);
            ESP.restart();
            break;
        case CMD_ZBS_BEGIN:
            if (zbs != nullptr) {
                delete zbs;
            }
            zbs = new ZBS_interface;
            if (cmd->data[0] & 1) {
                spi_speed = 1000000;
            } else {
                spi_speed = 8000000;
            }
            curspeed = spi_speed;
            if (cmd->data[0] & 2) {
                temp_buff[0] = zbs->begin(FLASHER_AP_SS, FLASHER_AP_CLK, FLASHER_AP_MOSI, FLASHER_AP_MISO, FLASHER_AP_RESET, FLASHER_AP_POWER, spi_speed);
            } else if (cmd->data[0] & 4) {
                temp_buff[0] = zbs->begin(FLASHER_ALT_SS, FLASHER_ALT_CLK, FLASHER_ALT_MOSI, FLASHER_ALT_MISO, FLASHER_ALT_RESET, 255, spi_speed);
            } else {
                temp_buff[0] = zbs->begin(FLASHER_EXT_SS, FLASHER_EXT_CLK, FLASHER_EXT_MOSI, FLASHER_EXT_MISO, FLASHER_EXT_RESET, FLASHER_EXT_POWER, spi_speed);
            }
            sendFlasherAnswer(cmd->command, temp_buff, 1);
            break;
        case CMD_RESET_ZBS:
            zbs->reset();
            temp_buff[0] = 1;
            sendFlasherAnswer(cmd->command, temp_buff, 1);
            break;
        case CMD_SELECT_PAGE:
            temp_buff[0] = zbs->select_flash(cmd->data[0] ? 1 : 0);
            sendFlasherAnswer(cmd->command, temp_buff, 1);
            break;
        case CMD_SET_POWER:
            zbs->set_power(cmd->data[0] ? 1 : 0);
            temp_buff[0] = 1;
            sendFlasherAnswer(cmd->command, temp_buff, 1);
            break;
        case CMD_READ_RAM:
            temp_buff[0] = zbs->read_ram(cmd->data[0]);
            sendFlasherAnswer(cmd->command, temp_buff, 1);
            break;
        case CMD_WRITE_RAM:
            zbs->write_ram(cmd->data[0], cmd->data[1]);
            temp_buff[0] = 1;
            sendFlasherAnswer(cmd->command, temp_buff, 1);
            break;
        case CMD_READ_FLASH:
            tempbuffer = (uint8_t*)calloc(cmd->data[0], 1);
            // cmd_buff[0] = len
            // cmd_buff[1] << 8 | cmd_buff[2] = position
            //Serial.printf("Loading %d bytes from %04X \n", cmd->data[0], (cmd->data[1] << 8 | cmd->data[2]));
            for (int i = 0; i < cmd->data[0]; i++) {
                tempbuffer[i] = zbs->read_flash((cmd->data[1] << 8 | cmd->data[2]) + i);
            }
            sendFlasherAnswer(cmd->command, tempbuffer, cmd->data[0]);
            free(tempbuffer);
            break;
        case CMD_WRITE_FLASH:
            // cmd_buff[0] = len
            // cmd_buff[1] << 8 | cmd_buff[2] = position
            // cmd_buff[3+i] = data
            if (cmd->data[0] >= (0xff - 3)) {  // Len too high, only 0xFF - header len possible
                temp_buff[0] = 0xEE;
                sendFlasherAnswer(cmd->command, temp_buff, 1);
                break;
            }
            //Serial.printf("Writing %d bytes to %04X \n", cmd->data[0], (cmd->data[1] << 8 | cmd->data[2]));
            for (int i = 0; i < cmd->data[0]; i++) {
                if (cmd->data[3 + i] != 0xff) {
                    for (uint8_t attempts = 0; attempts < 10; attempts++) {
                        zbs->write_flash((cmd->data[1] << 8 | cmd->data[2]) + i, cmd->data[3 + i]);
                        if (zbs->read_flash((cmd->data[1] << 8 | cmd->data[2]) + i) == cmd->data[3 + i]) {
                            goto flash_pass;
                        }
                        curspeed -= 100000;
                        zbs->setSpeed(curspeed);
                    }
                flash_fail:
                    temp_buff[0] = 0;
                    sendFlasherAnswer(cmd->command, temp_buff, 1);
                    break;
                flash_pass:
                    continue;
                }
            }
            temp_buff[0] = 1;
            sendFlasherAnswer(cmd->command, temp_buff, 1);
            break;
        case CMD_READ_SFR:
            temp_buff[0] = zbs->read_sfr(cmd->data[0]);
            sendFlasherAnswer(cmd->command, temp_buff, 1);
            break;
        case CMD_WRITE_SFR:
            zbs->write_sfr(cmd->data[0], cmd->data[1]);
            temp_buff[0] = 1;
            sendFlasherAnswer(cmd->command, temp_buff, 1);
            break;
        case CMD_ERASE_FLASH:
            zbs->erase_flash();
            temp_buff[0] = 1;
            sendFlasherAnswer(cmd->command, temp_buff, 1);
            break;
        case CMD_ERASE_INFOBLOCK:
            zbs->erase_infoblock();
            temp_buff[0] = 1;
            sendFlasherAnswer(cmd->command, temp_buff, 1);
            break;
        case CMD_SAVE_MAC_FROM_FW:
        case CMD_PASS_THROUGH:
            break;
    }
}

void usbFlasherTask(void* parameter) {
    flasherCmdQueue = xQueueCreate(10, sizeof(struct flasherCommand*));
#if ARDUINO_USB_MODE
#warning Wrong USB mode is in use, check settings in platformio.ini
#endif
    USB.onEvent(usbEventCallback);
    USBSerial.onEvent(usbEventCallback);
    USBSerial.begin();
    USB.begin();
    struct flasherCommand* cmd;
    while (true) {
        BaseType_t queuereceive = xQueueReceive(flasherCmdQueue, &cmd, portMAX_DELAY);
        if (queuereceive == pdTRUE) {
            processFlasherCommand(cmd);
            if (cmd->data != nullptr) {
                free(cmd->data);
            }
            delete cmd;
        }
    }
}