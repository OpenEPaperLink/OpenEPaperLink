#include "serialconsole.h"

#include <Arduino.h>

#include "USB.h"

QueueHandle_t consoleCmdQueue;
TaskHandle_t consoleTaskHandle;
extern USBCDC USBSerial;

struct consoleCommand {
    uint8_t command = 0;
    uint8_t len = 0;
    uint8_t* data = nullptr;
};

void consoleStopTask() {
    if (consoleTaskHandle) vTaskDelete(consoleTaskHandle);
    consoleTaskHandle = NULL;
}
/*
.in>1B 5B 3C 30 3B 33 39 3B 39 4D
,in>1B 5B 3C 30 3B 33 39 3B 39 6D
,in>1B 5B 3C 32 3B 33 39 3B 39 4D
,in>1B 5B 3C 32 3B 33 39 3B 39 6D

in>1B 5B 3C 30 3B 32 38 3B 31 32 4D
in>1B 5B 3C 30 3B 32 38 3B 31 32 6D

in>1B 5B 3C 32 3B 32 38 3B 31 32 4D
in>1B 5B 3C 32 3B 32 38 3B 31 32 6D

in>1B 5B 3C 36 34 3B 32 39 3B 31 32 4D
in>1B 5B 3C 36 35 3B 32 39 3B 31 32 4D
*/

bool escapeCommandComplete(struct consoleCommand* cmd) {
    return true;
}

void consoleUartHandler(uint8_t* data, uint8_t len) {
    static struct consoleCommand* cmd = nullptr;
    static bool commandStarted = false;

    while (len--) {
        uint8_t usbbyte = *(data++);

        if (cmd == nullptr) {
            cmd = new struct consoleCommand;
            cmd->data = (uint8_t*)calloc(65, 1);
            cmd->len = 0;
        }

        // check if we've started a command in this byte
        if ((!commandStarted) && (usbbyte == 0x1B)) {
            commandStarted = true;
            if (cmd->len != 0) {
                BaseType_t queuestatus = xQueueSend(consoleCmdQueue, &cmd, 0);
                if (queuestatus == pdFALSE) {
                    if (cmd->data != nullptr) free(cmd->data);
                    delete cmd;
                }
                cmd = nullptr;
            }
        }

        if (cmd == nullptr) {
            cmd = new struct consoleCommand;
            cmd->data = (uint8_t*)calloc(65, 1);
            cmd->len = 0;
        }

        cmd->data[cmd->len++] = usbbyte;
        cmd->len %= 64;

        if (commandStarted) {
            if (escapeCommandComplete(cmd) || cmd->len == 0x00) {
                BaseType_t queuestatus = xQueueSend(consoleCmdQueue, &cmd, 0);
                if (queuestatus == pdFALSE) {
                    if (cmd->data != nullptr) free(cmd->data);
                    delete cmd;
                }
                commandStarted = false;
                cmd = nullptr;
            }
        }
    }

    if (!commandStarted && cmd != nullptr) {
        BaseType_t queuestatus = xQueueSend(consoleCmdQueue, &cmd, 0);
        if (queuestatus == pdFALSE) {
            if (cmd->data != nullptr) free(cmd->data);
            delete cmd;
        }
        cmd = nullptr;
    }
}

void consoleTask(void* parameter) {
    struct consoleCommand* cmd;

    USBSerial.print("\e[?1000;1006;1015h");  // works

    while (true) {
        BaseType_t queuereceive = xQueueReceive(consoleCmdQueue, &cmd, 1500 / portTICK_PERIOD_MS);
        if (queuereceive == pdTRUE) {
            uint8_t c = 0;
            Serial.printf("queue>");
            while (cmd->len--) {
                Serial.printf(" %02X", cmd->data[c]);
                c++;
            }
            if (cmd->data != nullptr) free(cmd->data);
            delete cmd;
            Serial.printf("\n");
        }
    }
}
