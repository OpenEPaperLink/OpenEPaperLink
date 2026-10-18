// The S3 <-> radio protocol that every radio speaks today: 4-character ASCII commands, fixed-size
// binary payloads and ACK>/NOK>/NOQ> replies (docs/design/radio-chain-analysis.md, "Architecture and
// protocol"). Moved here from serialap.cpp without changes in behaviour, behind the RadioTransport
// interface, so the framed protocol (docs/design/radio-protocol-v2.md) can be added next to it.

#include "radiotransport.h"

#include <Arduino.h>

#include "commstructs.h"
#include "newproto.h"
#include "serialap.h"
#include "tag_db.h"

// If a command is sent, it will wait for a reply here
#define CMD_REPLY_WAIT 0x00
#define CMD_REPLY_ACK 0x01
#define CMD_REPLY_NOK 0x02
#define CMD_REPLY_NOQ 0x03
static volatile uint8_t cmdReplyValue = CMD_REPLY_WAIT;

#define ZBS_RX_WAIT_HEADER 0
#define ZBS_RX_WAIT_PKT_LEN 1
#define ZBS_RX_WAIT_PKT_RX 2
#define ZBS_RX_WAIT_SEP1 3
#define ZBS_RX_WAIT_SEP2 4
#define ZBS_RX_WAIT_VER 6
#define ZBS_RX_BLOCK_REQUEST 7
#define ZBS_RX_WAIT_XFERCOMPLETE 8
#define ZBS_RX_WAIT_DATA_REQ 9
#define ZBS_RX_WAIT_JOINNETWORK 10
#define ZBS_RX_WAIT_XFERTIMEOUT 11
#define ZBS_RX_WAIT_MAC 12
#define ZBS_RX_WAIT_CHANNEL 13
#define ZBS_RX_WAIT_POWER 14
#define ZBS_RX_WAIT_PENDING 15
#define ZBS_RX_WAIT_NOP 16
#define ZBS_RX_WAIT_TYPE 17
#define ZBS_RX_WAIT_TAG_RETURN_DATA 18
#define ZBS_RX_WAIT_SUBCHANNEL 19
#define ZBS_RX_WAIT_CAP 20

static bool txStart() {
    while (1) {
        if (xPortInIsrContext()) {
            if (xSemaphoreTakeFromISR(txActive, NULL) == pdTRUE) return true;
        } else {
            if (xSemaphoreTake(txActive, portTICK_PERIOD_MS)) return true;
        }
        vTaskDelay(10 / portTICK_PERIOD_MS);
        Serial.println("wait... tx busy");
    }
    // this never happens. Should we make a timeout?
    return false;
}
static void txEnd() {
    if (xPortInIsrContext()) {
        xSemaphoreGiveFromISR(txActive, NULL);
    } else {
        xSemaphoreGive(txActive);
    }
}
static bool waitCmdReply(uint32_t timeoutMs = 200) {
    uint32_t val = millis();
    while (millis() - val < timeoutMs) {
        switch (cmdReplyValue) {
            case CMD_REPLY_WAIT:
                break;
            case CMD_REPLY_ACK:
                lastAPActivity = millis();
                if (apInfo.isOnline == false)
                    setAPstate(true, AP_STATE_ONLINE);
                return true;
                break;
            case CMD_REPLY_NOK:
                lastAPActivity = millis();
                return false;
                break;
            case CMD_REPLY_NOQ:
                lastAPActivity = millis();
                return false;
                break;
        }
        vTaskDelay(1 / portTICK_RATE_MS);
    }
    return false;
}

class LegacyTransport : public RadioTransport {
   public:
    const char* name() override { return "legacy"; }
    uint16_t sendBlock(const void* data, uint16_t len, uint64_t ver, uint32_t offset, uint32_t totalSize) override;
    bool sendDataAvail(struct pendingData* pending) override;
    bool cancelPending(struct pendingData* pending) override;
    bool setChannelPower(struct espSetChannelPower* scp) override;
    bool ping() override;
    bool requestInfo() override;
    bool enableFastLink() override;
    bool enableFraming() override;
    void onIdle() override;
    void onByte(uint8_t c) override;

   private:
    char cmdbuffer[4] = {0};
    uint8_t* packetp = nullptr;
    uint8_t pktindex = 0;  // length of the command
    uint8_t RXState = ZBS_RX_WAIT_HEADER;
    uint8_t charindex = 0;
    uint32_t lastByteMs = 0;

    // States that own the calloc'ed packetp until they hand it to addRXQueue()
    static bool ownsPacket(uint8_t state) {
        return state == ZBS_RX_BLOCK_REQUEST || state == ZBS_RX_WAIT_XFERCOMPLETE || state == ZBS_RX_WAIT_XFERTIMEOUT ||
               state == ZBS_RX_WAIT_DATA_REQ || state == ZBS_RX_WAIT_TAG_RETURN_DATA;
    }
};

static LegacyTransport legacyTransport;
RadioTransport* radioTransport = &legacyTransport;
RadioTransport* legacyRadioTransport() { return &legacyTransport; }

// Send data to the AP
uint16_t LegacyTransport::sendBlock(const void* data, uint16_t len, uint64_t ver, uint32_t offset, uint32_t totalSize) {
    time_t timeCanary = millis();
    if (!txStart()) return 0;
    // don't retry now, as it collides with communication from the tag
    for (uint8_t attempt = 0; attempt < 1; attempt++) {
        cmdReplyValue = CMD_REPLY_WAIT;
        AP_SERIAL_PORT.print(">D>");
        // The radio reads no serial data while it sends a block burst (up to about 200 ms), so wait
        // longer than one burst. Giving up too early left the radio waiting for block data that
        // never came, and it then took the next commands as data (6 % of block hand-overs in an
        // hour with 8 tags, docs/design/radio-chain-results.md).
        if (waitCmdReply(500)) goto blksend;
        Serial.printf("block send failed in try %d\r\n", attempt);
    }
    Serial.print("Failed sending block...\r\n");
    txEnd();
    return 0;
blksend:
    uint8_t blockbuffer[sizeof(struct blockData)];
    struct blockData* bd = (struct blockData*)blockbuffer;
    bd->size = len;
    bd->checksum = 0;

    // calculate checksum
    const uint8_t* dataBytes = reinterpret_cast<const uint8_t*>(data);
    for (uint16_t c = 0; c < len; c++) {
        bd->checksum += dataBytes[c];
    }

    // send blockData header
    dataBytes = reinterpret_cast<const uint8_t*>(&blockbuffer);
    const size_t bufferSize = sizeof(struct blockData);
    uint8_t* modifiedHeader = static_cast<uint8_t*>(malloc(bufferSize));
    if (modifiedHeader != nullptr) {
        for (size_t i = 0; i < bufferSize; i++) {
            modifiedHeader[i] = 0xAA ^ dataBytes[i];
        }
        AP_SERIAL_PORT.write(modifiedHeader, bufferSize);
        free(modifiedHeader);
    }

    // send an entire block of data
    uint16_t c;
    dataBytes = reinterpret_cast<const uint8_t*>(data);
    uint8_t* modifiedBuffer = static_cast<uint8_t*>(malloc(len));
    if (modifiedBuffer != nullptr) {
        for (c = 0; c < len; c++) {
            modifiedBuffer[c] = 0xAA ^ dataBytes[c];
        }
        AP_SERIAL_PORT.write(modifiedBuffer, len);
        free(modifiedBuffer);
    }

    // fill the rest of the block-length filled with something else (will end up as 0xFF in the buffer)
    const size_t remainingBytes = BLOCK_DATA_SIZE - c;
    if (remainingBytes > 0) {
        uint8_t fillBuffer[remainingBytes];
        memset(fillBuffer, 0x55, remainingBytes);
        AP_SERIAL_PORT.write(fillBuffer, remainingBytes);
    }

    // dummy bytes in case some bytes were missed, makes sure the AP gets kicked out of data-loading mode
    uint8_t dummyBuffer[32];
    memset(dummyBuffer, 0xF5, 32);
    AP_SERIAL_PORT.write(dummyBuffer, 32);

    if (apInfo.type != ESP32_C6) delay(10);
    txEnd();
    Serial.println("Sendblock complete, " + String(millis() - timeCanary) + "ms");
    return bd->checksum;
}

bool LegacyTransport::sendDataAvail(struct pendingData* pending) {
    if (!txStart()) return false;
    addCRC(pending, sizeof(struct pendingData));
    for (uint8_t attempt = 0; attempt < 5; attempt++) {
        cmdReplyValue = CMD_REPLY_WAIT;
        AP_SERIAL_PORT.print("SDA>");
        for (uint8_t c = 0; c < sizeof(struct pendingData); c++) {
            AP_SERIAL_PORT.write(((uint8_t*)pending)[c]);
        }
        if (waitCmdReply()) {
            txEnd();
            return true;
        }
        if (cmdReplyValue == CMD_REPLY_NOQ) {
            // the radio has no free pending slot; trying again 200 ms later won't change that
            Serial.print("SDA refused: radio pending table full\r\n");
            break;
        }
        Serial.printf("SDA send failed in try %d\r\n", attempt);
        delay(200);
    }
    Serial.print("SDA failed to send...\r\n");
    txEnd();
    return false;
}
bool LegacyTransport::cancelPending(struct pendingData* pending) {
    if (!txStart()) return false;
    addCRC(pending, sizeof(struct pendingData));
    for (uint8_t attempt = 0; attempt < 5; attempt++) {
        cmdReplyValue = CMD_REPLY_WAIT;
        AP_SERIAL_PORT.print("CXD>");
        for (uint8_t c = 0; c < sizeof(struct pendingData); c++) {
            AP_SERIAL_PORT.write(((uint8_t*)pending)[c]);
        }
        if (waitCmdReply()) {
            txEnd();
            return true;
        }
        Serial.printf("CXD send failed in try %d\r\n", attempt);
    }
    Serial.print("CXD failed to send...\r\n");
    txEnd();
    return false;
}
bool LegacyTransport::setChannelPower(struct espSetChannelPower* scp) {
    if (!txStart()) return false;
    // The payload size depends on how the radio was built, not on HAS_SUBGHZ here: C6/H2 firmware
    // since 0x0019 always expects the sub-GHz channel byte, every other radio expects 3 bytes. A size
    // mismatch made the radio take the first byte of the next command as payload.
    uint8_t wire[4] = {0, scp->channel, scp->power, 0};
#ifdef HAS_SUBGHZ
    wire[3] = scp->subghzchannel;
#endif  // without HAS_SUBGHZ nobody changes the radio's sub-GHz channel, so 0 (its default) is no change
    const uint8_t wireLen = (apInfo.type == ESP32_C6 && apInfo.version >= 0x0019) ? 4 : 3;
    addCRC(wire, wireLen);
    for (uint8_t attempt = 0; attempt < 5; attempt++) {
        cmdReplyValue = CMD_REPLY_WAIT;
        AP_SERIAL_PORT.print("SCP>");
        AP_SERIAL_PORT.write(wire, wireLen);
        if (waitCmdReply()) {
            txEnd();
            return true;
        }
        Serial.printf("SCP send failed in try %d\r\n", attempt);
    }
    Serial.print("SCP failed to send...\r\n");
    txEnd();
    return false;
}
bool LegacyTransport::ping() {
    if (!txStart()) return false;
    for (uint8_t attempt = 0; attempt < 3; attempt++) {
        cmdReplyValue = CMD_REPLY_WAIT;
        AP_SERIAL_PORT.print("RDY?");
        if (waitCmdReply()) {
            txEnd();
            return true;
        }
    }
    txEnd();
    return false;
}
bool LegacyTransport::requestInfo() {
    if (!txStart()) return false;
    for (uint8_t attempt = 0; attempt < 5; attempt++) {
        cmdReplyValue = CMD_REPLY_WAIT;
        AP_SERIAL_PORT.print("NFO?");
        if (waitCmdReply()) {
            txEnd();
            return true;
        }
    }
    txEnd();
    return false;
}
bool LegacyTransport::enableFastLink() {
    if (!txStart()) return false;
    for (uint8_t attempt = 0; attempt < 5; attempt++) {
        cmdReplyValue = CMD_REPLY_WAIT;
        AP_SERIAL_PORT.print("HSPD");
        if (waitCmdReply()) {
            txEnd();
            return true;
        }
    }
    txEnd();
    return false;
}

// Protocol v2 radios (CAP> in the NFO? answer) switch to framing on FRM1, after their ACK>
bool LegacyTransport::enableFraming() {
    if (!txStart()) return false;
    for (uint8_t attempt = 0; attempt < 3; attempt++) {
        cmdReplyValue = CMD_REPLY_WAIT;
        AP_SERIAL_PORT.print("FRM1");
        if (waitCmdReply()) {
            txEnd();
            return true;
        }
    }
    txEnd();
    return false;
}

void LegacyTransport::onIdle() {
    // A message whose bytes stopped arriving (lost bytes) must not keep the parser in a payload
    // state, where it would swallow the next messages as payload.
    if (RXState != ZBS_RX_WAIT_HEADER && !AP_SERIAL_PORT.available() && millis() - lastByteMs > 100) {
        if (ownsPacket(RXState)) free(packetp);
        packetp = nullptr;
        RXState = ZBS_RX_WAIT_HEADER;
        Serial.print("AP serial: incomplete message dropped\r\n");
    }
}

void LegacyTransport::onByte(uint8_t c) {
    const char lastchar = (char)c;
    lastByteMs = millis();
    if (ownsPacket(RXState) && packetp == nullptr) RXState = ZBS_RX_WAIT_HEADER;  // calloc failed
    switch (RXState) {
        case ZBS_RX_WAIT_HEADER:

            Serial.write(lastchar);

            //  shift characters in
            for (uint8_t c = 0; c < 3; c++) {
                cmdbuffer[c] = cmdbuffer[c + 1];
            }
            cmdbuffer[3] = lastchar;

            if ((strncmp(cmdbuffer, "ACK>", 4) == 0)) cmdReplyValue = CMD_REPLY_ACK;
            if ((strncmp(cmdbuffer, "NOK>", 4) == 0)) cmdReplyValue = CMD_REPLY_NOK;
            if ((strncmp(cmdbuffer, "NOQ>", 4) == 0)) cmdReplyValue = CMD_REPLY_NOQ;

            if ((strncmp(cmdbuffer, "VER>", 4) == 0)) {
                pktindex = 0;
                RXState = ZBS_RX_WAIT_VER;
                charindex = 0;
                memset(cmdbuffer, 0x00, 4);
            }
            if ((strncmp(cmdbuffer, "MAC>", 4) == 0)) {
                RXState = ZBS_RX_WAIT_MAC;
                charindex = 0;
                memset(cmdbuffer, 0x00, 4);
            }
            if ((strncmp(cmdbuffer, "ZCH>", 4) == 0)) {
                RXState = ZBS_RX_WAIT_CHANNEL;
                charindex = 0;
                memset(cmdbuffer, 0x00, 4);
            }
#ifdef HAS_SUBGHZ
            if ((strncmp(cmdbuffer, "SCH>", 4) == 0)) {
                RXState = ZBS_RX_WAIT_SUBCHANNEL;
                charindex = 0;
                memset(cmdbuffer, 0x00, 4);
            }
#endif
            if ((strncmp(cmdbuffer, "ZPW>", 4) == 0)) {
                RXState = ZBS_RX_WAIT_POWER;
                charindex = 0;
                memset(cmdbuffer, 0x00, 4);
            }
            if ((strncmp(cmdbuffer, "PEN>", 4) == 0)) {
                RXState = ZBS_RX_WAIT_PENDING;
                charindex = 0;
                memset(cmdbuffer, 0x00, 4);
            }
            if ((strncmp(cmdbuffer, "NOP>", 4) == 0)) {
                RXState = ZBS_RX_WAIT_NOP;
                charindex = 0;
                memset(cmdbuffer, 0x00, 4);
            }
            if ((strncmp(cmdbuffer, "CAP>", 4) == 0)) {
                RXState = ZBS_RX_WAIT_CAP;
                charindex = 0;
                memset(cmdbuffer, 0x00, 4);
            }
            if ((strncmp(cmdbuffer, "TYP>", 4) == 0)) {
                RXState = ZBS_RX_WAIT_TYPE;
                charindex = 0;
                memset(cmdbuffer, 0x00, 4);
            }
            if (strncmp(cmdbuffer, "RES>", 4) == 0) {
                addRXQueue(NULL, 0, RX_CMD_RSET);
            }
            if (strncmp(cmdbuffer, "RQB>", 4) == 0) {
                RXState = ZBS_RX_BLOCK_REQUEST;
                charindex = 0;
                pktindex = 0;
                packetp = (uint8_t*)calloc(sizeof(struct espBlockRequest) + 8, 1);
                memset(cmdbuffer, 0x00, 4);
                lastAPActivity = millis();
                // don't set APstate heree, as it interferes with the flashing process
                // if (apInfo.isOnline == false && config.runStatus == RUNSTATUS_RUN) setAPstate(true, AP_STATE_ONLINE);
            }
            if (strncmp(cmdbuffer, "ADR>", 4) == 0) {
                RXState = ZBS_RX_WAIT_DATA_REQ;
                charindex = 0;
                pktindex = 0;
                packetp = (uint8_t*)calloc(sizeof(struct espAvailDataReq) + 8, 1);
                memset(cmdbuffer, 0x00, 4);
                lastAPActivity = millis();
                // don't set APstate heree, as it interferes with the flashing process
                // if (apInfo.isOnline == false && config.runStatus == RUNSTATUS_RUN) setAPstate(true, AP_STATE_ONLINE);
            }
            if (strncmp(cmdbuffer, "XFC>", 4) == 0) {
                RXState = ZBS_RX_WAIT_XFERCOMPLETE;
                pktindex = 0;
                packetp = (uint8_t*)calloc(sizeof(struct espXferComplete) + 8, 1);
                memset(cmdbuffer, 0x00, 4);
            }
            if (strncmp(cmdbuffer, "XTO>", 4) == 0) {
                RXState = ZBS_RX_WAIT_XFERTIMEOUT;
                pktindex = 0;
                packetp = (uint8_t*)calloc(sizeof(struct espXferComplete) + 8, 1);
                memset(cmdbuffer, 0x00, 4);
            }
            if (strncmp(cmdbuffer, "RDY>", 4) == 0) {
                addRXQueue(NULL, 0, RX_CMD_RDY);
            }
            if (strncmp(cmdbuffer, "TRD>", 4) == 0) {
                RXState = ZBS_RX_WAIT_TAG_RETURN_DATA;
                pktindex = 0;
                packetp = (uint8_t*)calloc(sizeof(struct espTagReturnData) + 8, 1);
                memset(cmdbuffer, 0x00, 4);
                lastAPActivity = millis();
            }
            break;
        case ZBS_RX_BLOCK_REQUEST:
            packetp[pktindex] = lastchar;
            pktindex++;
            if (pktindex == sizeof(struct espBlockRequest)) {
                addRXQueue(packetp, pktindex, RX_CMD_RQB);
                RXState = ZBS_RX_WAIT_HEADER;
            }
            break;
        case ZBS_RX_WAIT_XFERCOMPLETE:
            packetp[pktindex] = lastchar;
            pktindex++;
            if (pktindex == sizeof(struct espXferComplete)) {
                addRXQueue(packetp, pktindex, RX_CMD_XFC);
                RXState = ZBS_RX_WAIT_HEADER;
            }
            break;
        case ZBS_RX_WAIT_XFERTIMEOUT:
            packetp[pktindex] = lastchar;
            pktindex++;
            if (pktindex == sizeof(struct espXferComplete)) {
                addRXQueue(packetp, pktindex, RX_CMD_XTO);
                RXState = ZBS_RX_WAIT_HEADER;
            }
            break;
        case ZBS_RX_WAIT_DATA_REQ:
            packetp[pktindex] = lastchar;
            pktindex++;
            if (pktindex == sizeof(struct espAvailDataReq)) {
                addRXQueue(packetp, pktindex, RX_CMD_ADR);
                RXState = ZBS_RX_WAIT_HEADER;
            }
            break;
        case ZBS_RX_WAIT_TAG_RETURN_DATA: {
            packetp[pktindex] = lastchar;
            pktindex++;
            // byte 9 is the length of the tag data; an impossible value (corrupted or lost
            // bytes) would let pktindex run past the buffer and wrap around
            if (pktindex == 10 && packetp[9] > sizeof(struct tagReturnData)) {
                free(packetp);
                packetp = nullptr;
                RXState = ZBS_RX_WAIT_HEADER;
                Serial.print("AP serial: TRD with invalid length dropped\r\n");
                break;
            }
            if ((pktindex > 10) && (pktindex >= (packetp[9] + 10))) {
                addRXQueue(packetp, pktindex, RX_CMD_TRD);
                RXState = ZBS_RX_WAIT_HEADER;
            }
        } break;
        case ZBS_RX_WAIT_VER:
            cmdbuffer[charindex] = lastchar;
            charindex++;
            if (charindex == 4) {
                charindex = 0;
                // cmdbuffer holds exactly 4 characters and no terminator
                const char ver[5] = {cmdbuffer[0], cmdbuffer[1], cmdbuffer[2], cmdbuffer[3], 0};
                apInfo.version = (uint16_t)strtoul(ver, NULL, 16);
                RXState = ZBS_RX_WAIT_HEADER;
            }
            break;
        case ZBS_RX_WAIT_CAP:
            cmdbuffer[charindex] = lastchar;
            charindex++;
            if (charindex == 4) {
                charindex = 0;
                const char caps[5] = {cmdbuffer[0], cmdbuffer[1], cmdbuffer[2], cmdbuffer[3], 0};
                apInfo.caps = (uint16_t)strtoul(caps, NULL, 16);
                RXState = ZBS_RX_WAIT_HEADER;
            }
            break;
        case ZBS_RX_WAIT_MAC:
            cmdbuffer[charindex] = lastchar;
            charindex++;
            if (charindex == 2) {
                charindex = 0;
                apInfo.mac[pktindex] = (uint8_t)strtoul(cmdbuffer, NULL, 16);
                pktindex++;
            }
            if (pktindex == 8) {
                RXState = ZBS_RX_WAIT_HEADER;
            }
            break;
        case ZBS_RX_WAIT_CHANNEL:
            cmdbuffer[charindex] = lastchar;
            charindex++;
            if (charindex == 2) {
                RXState = ZBS_RX_WAIT_HEADER;
                apInfo.channel = (uint8_t)strtoul(cmdbuffer, NULL, 16);
            }
            break;
#ifdef HAS_SUBGHZ
        case ZBS_RX_WAIT_SUBCHANNEL:
            cmdbuffer[charindex] = lastchar;
            charindex++;
            if (charindex == 3) {
                RXState = ZBS_RX_WAIT_HEADER;
                int Channel = atoi(cmdbuffer);
                if (Channel != NO_SUBGHZ_CHANNEL) {
                    apInfo.hasSubGhz = true;
                    apInfo.SubGhzChannel = Channel;
                } else {
                    apInfo.hasSubGhz = false;
                    apInfo.SubGhzChannel = 0;
                }
            }
            break;
#endif
        case ZBS_RX_WAIT_POWER:
            cmdbuffer[charindex] = lastchar;
            charindex++;
            if (charindex == 2) {
                RXState = ZBS_RX_WAIT_HEADER;
                apInfo.power = (uint8_t)strtoul(cmdbuffer, NULL, 16);
            }
            break;
        case ZBS_RX_WAIT_PENDING:
            cmdbuffer[charindex] = lastchar;
            charindex++;
            if (charindex == 2) {
                RXState = ZBS_RX_WAIT_HEADER;
                apInfo.pendingBuffer = (uint8_t)strtoul(cmdbuffer, NULL, 16);
            }
            break;
        case ZBS_RX_WAIT_NOP:
            cmdbuffer[charindex] = lastchar;
            charindex++;
            if (charindex == 2) {
                RXState = ZBS_RX_WAIT_HEADER;
                apInfo.nop = (uint8_t)strtoul(cmdbuffer, NULL, 16);
            }
            break;
        case ZBS_RX_WAIT_TYPE:
            cmdbuffer[charindex] = lastchar;
            charindex++;
            if (charindex == 2) {
                RXState = ZBS_RX_WAIT_HEADER;
                apInfo.type = (uint8_t)strtoul(cmdbuffer, NULL, 16);
            }
            break;
    }
}
