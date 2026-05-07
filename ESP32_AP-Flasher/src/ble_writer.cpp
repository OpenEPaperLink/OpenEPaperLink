#ifdef HAS_BLE_WRITER
#include <Arduino.h>
#include <MD5Builder.h>
#include <mbedtls/aes.h>

#include "BLEDevice.h"
#include "ble_filter.h"
#include "newproto.h"
#include "tag_db.h"

#define INTERVAL_BLE_SCANNING_SECONDS 60
#define INTERVAL_HANDLE_PENDING_SECONDS 10
#define BUFFER_MAX_SIZE_COMPRESSING 135000

#define BLE_MAIN_STATE_IDLE 0
#define BLE_MAIN_STATE_PREPARE 1
#define BLE_MAIN_STATE_CONNECT 2
#define BLE_MAIN_STATE_UPLOAD 3
#define BLE_MAIN_STATE_ATC_BLE_OEPL_UPLOAD 4

int ble_main_state = BLE_MAIN_STATE_IDLE;
uint32_t last_ble_scan = 0;

#define BLE_UPLOAD_STATE_INIT 0
#define BLE_UPLOAD_STATE_SIZE 1
#define BLE_UPLOAD_STATE_START 2
#define BLE_UPLOAD_STATE_UPLOAD 5
int BLE_upload_state = BLE_UPLOAD_STATE_INIT;

#define BLE_CMD_ACK_CMD 99
#define BLE_CMD_AVAILDATA 100
#define BLE_CMD_BLK_DATA 101
#define BLE_CMD_ERR_BLKPRT 196
#define BLE_CMD_ACK_BLKPRT 197
#define BLE_CMD_REQ 198
#define BLE_CMD_ACK 199
#define BLE_CMD_ACK_IS_SHOWN 200
#define BLE_CMD_ACK_FW_UPDATED 201

struct AvailDataInfo BLEavaildatainfo = {0};
struct blockRequest BLEblkRequst = {0};

bool BLE_connected = false;
bool BLE_new_notify = false;

static BLEUUID ATC_BLE_OEPL_ServiceUUID((uint16_t)0x1337);
static BLEUUID ATC_BLE_OEPL_CtrlUUID((uint16_t)0x1337);

static BLEUUID gicServiceUUID((uint16_t)0xfef0);
static BLEUUID gicCtrlUUID((uint16_t)0xfef1);
static BLEUUID gicImgUUID((uint16_t)0xfef2);

// Wolink/Zhsunyco BLE ESL service and characteristic UUIDs.
// Source: NickWaterton/Wolink wolink_ble.py.
static BLEUUID wolinkServiceUUID("30323032-4c53-4545-4c42-4b4e494c4f57");
static BLEUUID wolinkDataUUID("31323032-4c53-4545-4c42-4b4e494c4f57");
static BLEUUID wolinkAuthUUID("33323032-4c53-4545-4c42-4b4e494c4f57");
static BLEUUID wolinkStatusUUID("35323032-4c53-4545-4c42-4b4e494c4f57");

static const uint8_t WOLINK_AES_KEY[16] = {
    0x9B, 0x60, 0x9F, 0x28, 0xBC, 0x49, 0xE2, 0x57,
    0x29, 0xBD, 0x7B, 0x8D, 0xF2, 0x2B, 0x44, 0x20};

BLERemoteCharacteristic* ctrlChar;
BLERemoteCharacteristic* imgChar;
BLEAdvertisedDevice* myDevice;
BLEClient* pClient;

uint8_t BLE_notify_buffer[256] = {0};

uint32_t BLE_err_counter = 0;
uint32_t BLE_curr_part = 0;
uint32_t BLE_max_block_parts = 0;
uint8_t BLE_mini_buff[256];

uint32_t BLE_last_notify = 0;
uint32_t BLE_last_pending_check = 0;
uint8_t BLE_curr_address[8] = {0};

uint32_t BLE_compressed_len = 0;
uint8_t* BLE_image_buffer;

static void notifyCallback(
    BLERemoteCharacteristic* pBLERemoteCharacteristic,
    uint8_t* pData,
    size_t length,
    bool isNotify) {
    Serial.print("Notify callback for characteristic ");
    Serial.print(pBLERemoteCharacteristic->getUUID().toString().c_str());
    Serial.print(" of data length ");
    Serial.println(length);
    Serial.print("data: ");
    for (int i = 0; i < length; i++) {
        Serial.printf("%02X", pData[i]);
        BLE_notify_buffer[1 + i] = pData[i];
    }
    BLE_notify_buffer[0] = length;
    Serial.println();
    BLE_new_notify = true;
}

class MyClientCallback : public BLEClientCallbacks {
    void onConnect(BLEClient* pclient) {
        Serial.println("BLE onConnect");
        BLE_connected = true;
    }

    void onDisconnect(BLEClient* pclient) {
        Serial.println("BLE onDisconnect");
        pclient->disconnect();
        BLE_connected = false;
        ble_main_state = BLE_MAIN_STATE_IDLE;
    }
};

enum BLE_CONNECTION_TYPE {
    BLE_TYPE_GICISKY = 0,
    BLE_TYPE_ATC_BLE_OEPL
};

bool BLE_connect(uint8_t addr[8], BLE_CONNECTION_TYPE conn_type) {
    BLE_err_counter = 0;
    uint8_t temp_Address[] = {addr[5], addr[4], addr[3], addr[2], addr[1], addr[0]};
    Serial.printf("BLE Connecting to: %02X:%02X:%02X:%02X:%02X:%02X\r\n", addr[5], addr[4], addr[3], addr[2], addr[1], addr[0]);
    pClient = BLEDevice::createClient();
    pClient->setClientCallbacks(new MyClientCallback());
    if (!pClient->connect(BLEAddress(temp_Address))) {
        Serial.printf("BLE connection failed\r\n");
        pClient->disconnect();
        return false;
    }
    uint32_t timeStart = millis();
    while (millis() - timeStart <= 5000) {  // We wait for a few seconds as otherwise the connection might not be ready!
        delay(100);
    }
    if (!BLE_connected)
        return false;
    Serial.printf("BLE starting to get service\r\n");
    BLERemoteService* pRemoteService = pClient->getService((conn_type == BLE_TYPE_GICISKY) ? gicServiceUUID : ATC_BLE_OEPL_ServiceUUID);
    if (pRemoteService == nullptr) {
        Serial.printf("BLE Service failed\r\n");
        pClient->disconnect();
        return false;
    }
    if (conn_type == BLE_TYPE_GICISKY) {
        imgChar = pRemoteService->getCharacteristic(gicImgUUID);
        if (imgChar == nullptr) {
            Serial.printf("BLE IMG Char failed\r\n");
            pClient->disconnect();
            return false;
        }
    }
    ctrlChar = pRemoteService->getCharacteristic((conn_type == BLE_TYPE_GICISKY) ? gicCtrlUUID : ATC_BLE_OEPL_CtrlUUID);
    if (ctrlChar == nullptr) {
        Serial.printf("BLE ctrl Char failed\r\n");
        pClient->disconnect();
        return false;
    }
    if (ctrlChar->canNotify()) {
        ctrlChar->registerForNotify(notifyCallback);
    } else {
        Serial.printf("BLE Notify failed\r\n");
        pClient->disconnect();
        return false;
    }
    if (pClient->setMTU(255) == false) {
        Serial.printf("BLE MTU failed\r\n");
        pClient->disconnect();
        return false;
    }
    Serial.printf("BLE Connected fully to: %02X:%02X:%02X:%02X:%02X:%02X\r\n", addr[5], addr[4], addr[3], addr[2], addr[1], addr[0]);
    return true;
}

class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice advertisedDevice) {
        BLE_filter_add_device(advertisedDevice);
    }
};

void BLE_startScan(uint32_t timeout) {
    BLEScan* pBLEScan = BLEDevice::getScan();
    pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
    pBLEScan->setInterval(1349);
    pBLEScan->setWindow(449);
    pBLEScan->setActiveScan(true);
    pBLEScan->start(timeout, false);
}

#define BLOCK_DATA_SIZE_BLE 4096
#define BLOCK_PART_DATA_SIZE_BLE 230
uint8_t tempBlockBuffer[BLOCK_DATA_SIZE_BLE + 4];
uint8_t tempPacketBuffer[2 + 3 + BLOCK_PART_DATA_SIZE_BLE];
void ATC_BLE_OEPL_PrepareBlk(uint8_t indexBlockId) {
    if (BLE_image_buffer == nullptr) {
        return;
    }
    uint32_t bufferPosition = (BLOCK_DATA_SIZE_BLE * indexBlockId);
    uint32_t lenNow = BLOCK_DATA_SIZE_BLE;
    uint16_t crcCalc = 0;
    if ((BLE_compressed_len - bufferPosition) < BLOCK_DATA_SIZE_BLE)
        lenNow = (BLE_compressed_len - bufferPosition);
    tempBlockBuffer[0] = lenNow & 0xff;
    tempBlockBuffer[1] = (lenNow >> 8) & 0xff;
    for (uint16_t c = 0; c < lenNow; c++) {
        tempBlockBuffer[4 + c] = BLE_image_buffer[c + bufferPosition];
        crcCalc += tempBlockBuffer[4 + c];
    }
    tempBlockBuffer[2] = crcCalc & 0xff;
    tempBlockBuffer[3] = (crcCalc >> 8) & 0xff;
    BLE_max_block_parts = (4 + lenNow) / BLOCK_PART_DATA_SIZE_BLE;
    if ((4 + lenNow) % BLOCK_PART_DATA_SIZE_BLE)
        BLE_max_block_parts++;
    Serial.println("Preparing block: " + String(indexBlockId) + " BuffPos: " + String(bufferPosition) + " LenNow: " + String(lenNow) + " MaxBLEparts: " + String(BLE_max_block_parts));
    BLE_curr_part = 0;
}

void ATC_BLE_OEPL_SendPart(uint8_t indexBlockId, uint8_t indexPkt) {
    uint8_t crcCalc = indexBlockId + indexPkt;
    for (uint16_t c = 0; c < BLOCK_PART_DATA_SIZE_BLE; c++) {
        tempPacketBuffer[5 + c] = tempBlockBuffer[c + (BLOCK_PART_DATA_SIZE_BLE * indexPkt)];
        crcCalc += tempPacketBuffer[5 + c];
    }
    tempPacketBuffer[0] = 0x00;
    tempPacketBuffer[1] = 0x65;
    tempPacketBuffer[2] = crcCalc;
    tempPacketBuffer[3] = indexBlockId;
    tempPacketBuffer[4] = indexPkt;
    Serial.println("BLE Sending packet Len " + String(sizeof(tempPacketBuffer)));
    ctrlChar->writeValue(tempPacketBuffer, sizeof(tempPacketBuffer), true);
}

// Wolink upload runs to completion inside the BLE task (single-threaded).
// State for the status notify callback lives at file scope so we can capture
// the two-byte status word that the ESL emits while it processes the upload.
volatile bool wolink_status_received = false;
volatile uint8_t wolink_status[2] = {0xFF, 0xFF};

static void wolinkStatusNotifyCallback(
    BLERemoteCharacteristic* pBLERemoteCharacteristic,
    uint8_t* pData,
    size_t length,
    bool isNotify) {
    if (length >= 2) {
        wolink_status[0] = pData[0];
        wolink_status[1] = pData[1];
    } else if (length == 1) {
        wolink_status[0] = pData[0];
        wolink_status[1] = 0;
    }
    wolink_status_received = true;
    Serial.printf("Wolink status notify: ");
    for (size_t i = 0; i < length; i++) Serial.printf("%02X", pData[i]);
    Serial.println();
}

// AES-128-CBC encrypt the 16-byte challenge with the hardcoded Wolink key,
// zero IV, and PKCS#7 padding to 32 bytes; we keep only the first 16-byte
// ciphertext block as the response.
static void wolink_aes_encrypt_challenge(const uint8_t challenge[16], uint8_t out[16]) {
    mbedtls_aes_context ctx;
    mbedtls_aes_init(&ctx);
    mbedtls_aes_setkey_enc(&ctx, WOLINK_AES_KEY, 128);
    uint8_t iv[16] = {0};
    uint8_t padded[32];
    memcpy(padded, challenge, 16);
    memset(padded + 16, 0x10, 16);  // PKCS#7: full second block of 0x10
    uint8_t cipher[32];
    mbedtls_aes_crypt_cbc(&ctx, MBEDTLS_AES_ENCRYPT, 32, iv, padded, cipher);
    memcpy(out, cipher, 16);
    mbedtls_aes_free(&ctx);
}

// Run a complete Wolink image upload: connect, authenticate, send pixels,
// trigger refresh, wait for the EPD to report idle. Returns true on success.
static bool wolink_upload(uint8_t address[8]) {
    uint8_t temp_Address[] = {address[5], address[4], address[3], address[2], address[1], address[0]};
    Serial.printf("Wolink connecting to %02X:%02X:%02X:%02X:%02X:%02X\r\n",
                  address[5], address[4], address[3], address[2], address[1], address[0]);

    pClient = BLEDevice::createClient();
    pClient->setClientCallbacks(new MyClientCallback());
    if (!pClient->connect(BLEAddress(temp_Address))) {
        Serial.println("Wolink connect failed");
        pClient->disconnect();
        return false;
    }

    uint32_t t0 = millis();
    while (!BLE_connected && millis() - t0 < 5000) delay(50);
    if (!BLE_connected) {
        Serial.println("Wolink connect did not settle");
        pClient->disconnect();
        return false;
    }
    if (!pClient->setMTU(247)) {
        Serial.println("Wolink MTU negotiation failed (continuing)");
    }

    BLERemoteService* svc = pClient->getService(wolinkServiceUUID);
    if (!svc) {
        Serial.println("Wolink service not found");
        pClient->disconnect();
        return false;
    }
    BLERemoteCharacteristic* dataChar = svc->getCharacteristic(wolinkDataUUID);
    BLERemoteCharacteristic* authChar = svc->getCharacteristic(wolinkAuthUUID);
    BLERemoteCharacteristic* statusChar = svc->getCharacteristic(wolinkStatusUUID);
    if (!dataChar || !authChar || !statusChar) {
        Serial.println("Wolink characteristic discovery failed");
        pClient->disconnect();
        return false;
    }
    if (statusChar->canNotify()) {
        wolink_status_received = false;
        wolink_status[0] = 0xFF;
        wolink_status[1] = 0xFF;
        statusChar->registerForNotify(wolinkStatusNotifyCallback);
    } else {
        Serial.println("Wolink status char does not notify");
    }

    // 1) Authenticate: read 16-byte challenge, write AES-CBC response.
    std::string challenge = authChar->readValue();
    if (challenge.size() < 16) {
        Serial.printf("Wolink auth challenge too short (%u)\r\n", (unsigned)challenge.size());
        pClient->disconnect();
        return false;
    }
    uint8_t resp[16];
    wolink_aes_encrypt_challenge((const uint8_t*)challenge.data(), resp);
    Serial.printf("Wolink auth challenge=");
    for (int i = 0; i < 16; i++) Serial.printf("%02X", (uint8_t)challenge[i]);
    Serial.printf(" resp=");
    for (int i = 0; i < 16; i++) Serial.printf("%02X", resp[i]);
    Serial.println();
    authChar->writeValue(resp, 16, false);

    // 2) Pack the pixel buffer.
    BLE_image_buffer = (uint8_t*)malloc(BUFFER_MAX_SIZE_COMPRESSING);
    if (BLE_image_buffer == nullptr) {
        Serial.println("Wolink: pixel buffer alloc failed");
        pClient->disconnect();
        return false;
    }
    BLE_compressed_len = pack_wolink_image(address, BLE_image_buffer, BUFFER_MAX_SIZE_COMPRESSING);
    if (BLE_compressed_len == 0) {
        Serial.println("Wolink: pack failed");
        free(BLE_image_buffer);
        BLE_image_buffer = nullptr;
        pClient->disconnect();
        return false;
    }
    Serial.printf("Wolink packed %u bytes\r\n", (unsigned)BLE_compressed_len);

    // 3) Upload chunks of (mtu-9) bytes with [0x00,0xA5] + offset(LE32) header.
    const size_t chunk_size = 238;  // matches MTU 247 - 9 byte overhead
    uint8_t pkt[6 + 238];
    for (uint32_t off = 0; off < BLE_compressed_len; off += chunk_size) {
        size_t this_n = (BLE_compressed_len - off) < chunk_size ? (BLE_compressed_len - off) : chunk_size;
        pkt[0] = 0x00;
        pkt[1] = 0xA5;
        pkt[2] = off & 0xFF;
        pkt[3] = (off >> 8) & 0xFF;
        pkt[4] = (off >> 16) & 0xFF;
        pkt[5] = (off >> 24) & 0xFF;
        memcpy(pkt + 6, BLE_image_buffer + off, this_n);
        dataChar->writeValue(pkt, 6 + this_n, true);
        delay(15);
    }

    // 4) Trigger refresh: [0x01,0xA5] + total_len(LE32).
    uint8_t refresh[6];
    refresh[0] = 0x01;
    refresh[1] = 0xA5;
    refresh[2] = BLE_compressed_len & 0xFF;
    refresh[3] = (BLE_compressed_len >> 8) & 0xFF;
    refresh[4] = (BLE_compressed_len >> 16) & 0xFF;
    refresh[5] = (BLE_compressed_len >> 24) & 0xFF;
    dataChar->writeValue(refresh, 6, true);
    Serial.println("Wolink refresh triggered, waiting for status idle");

    // 5) Wait for status notify with first byte == 0x00 (idle/done).
    bool ok = false;
    uint32_t t_start = millis();
    while (millis() - t_start < 30000) {
        if (wolink_status_received) {
            wolink_status_received = false;
            if (wolink_status[0] == 0x00) {
                ok = (wolink_status[1] == 0x00);
                Serial.printf("Wolink upload %s (status %02X %02X)\r\n",
                              ok ? "OK" : "ERR", wolink_status[0], wolink_status[1]);
                break;
            }
        }
        delay(50);
    }
    if (!ok && millis() - t_start >= 30000) {
        Serial.println("Wolink: status wait timed out");
    }

    free(BLE_image_buffer);
    BLE_image_buffer = nullptr;
    pClient->disconnect();
    return ok;
}

void BLETask(void* parameter) {
    vTaskDelay(5000 / portTICK_PERIOD_MS);
    Serial.println("BLE task started");
    BLEDevice::init("");
    while (1) {
        switch (ble_main_state) {
            default:
            case BLE_MAIN_STATE_IDLE:
                if (millis() - last_ble_scan > (INTERVAL_BLE_SCANNING_SECONDS * 1000)) {
                    last_ble_scan = millis();
                    Serial.println("Doing the BLE Scan");
                    BLE_startScan(10);  // timeout in seconds, this is blocking but only for this thread!
                }
                if (millis() - BLE_last_pending_check >= (INTERVAL_HANDLE_PENDING_SECONDS * 1000)) {
                    if (BLE_is_image_pending(BLE_curr_address)) {
                        Serial.println("BLE Image is pending but we wait a bit");
                        delay(5000);                                                       // We better wait here, since the pending image needs to be created first
                        tagRecord* curTag = tagRecord::findByMAC(BLE_curr_address);
                        uint8_t curHwType = (curTag != nullptr) ? curTag->hwType : 0;
                        if (curHwType != 0 && (curHwType & 0xF0) == 0xD0) {
                            // Wolink/Zhsunyco family. The whole upload runs synchronously
                            // inside this task, so we stay in IDLE and report completion
                            // (or cancel) directly when wolink_upload() returns.
                            bool ok = wolink_upload(BLE_curr_address);
                            if (ok) {
                                struct espXferComplete reportStruct;
                                memcpy((uint8_t*)&reportStruct.src, BLE_curr_address, 8);
                                processXferComplete(&reportStruct, true);
                                BLE_err_counter = 0;
                            } else {
                                if (BLE_err_counter++ >= 5) {
                                    struct espXferComplete reportStruct;
                                    memcpy((uint8_t*)&reportStruct.src, BLE_curr_address, 8);
                                    processXferComplete(&reportStruct, true);
                                    BLE_err_counter = 0;
                                }
                            }
                            BLE_last_pending_check = millis();
                        } else if (BLE_curr_address[7] == 0x13 && BLE_curr_address[6] == 0x37) {  // This is an ATC BLE OEPL display
                            // Here we create the compressed buffer
                            BLE_image_buffer = (uint8_t*)malloc(BUFFER_MAX_SIZE_COMPRESSING);
                            if (BLE_image_buffer == nullptr) {
                                Serial.println("BLE Could not create buffer!");
                                BLE_compressed_len = 0;
                            } else {
                                uint8_t dataType = 0x00;
                                uint8_t dataTypeArgument = 0x00;
                                uint16_t nextCheckin = 0x00;
                                BLE_compressed_len = get_ATC_BLE_OEPL_image(BLE_curr_address, BLE_image_buffer, BUFFER_MAX_SIZE_COMPRESSING, &dataType, &dataTypeArgument, &nextCheckin);
                                Serial.printf("BLE data Length: %i\r\n", BLE_compressed_len);
                                // then we connect to BLE to send the compressed data
                                if (BLE_compressed_len && BLE_connect(BLE_curr_address, BLE_TYPE_ATC_BLE_OEPL)) {
                                    BLE_err_counter = 0;
                                    BLE_curr_part = 0;
                                    memset(BLE_notify_buffer, 0x00, sizeof(BLE_notify_buffer));

                                    uint8_t md5bytes[16];
                                    MD5Builder md5;
                                    md5.begin();
                                    md5.add(BLE_image_buffer, BLE_compressed_len);
                                    md5.calculate();
                                    md5.getBytes(md5bytes);

                                    BLEavaildatainfo.dataType = dataType;
                                    BLEavaildatainfo.dataVer = *((uint64_t*)md5bytes);
                                    BLEavaildatainfo.dataSize = BLE_compressed_len;
                                    BLEavaildatainfo.dataTypeArgument = dataTypeArgument;
                                    BLEavaildatainfo.nextCheckIn = nextCheckin;
                                    BLEavaildatainfo.checksum = 0;
                                    for (uint16_t c = 1; c < sizeof(struct AvailDataInfo); c++) {
                                        BLEavaildatainfo.checksum += (uint8_t)((uint8_t*)&BLEavaildatainfo)[c];
                                    }
                                    BLE_upload_state = BLE_UPLOAD_STATE_INIT;
                                    ble_main_state = BLE_MAIN_STATE_ATC_BLE_OEPL_UPLOAD;
                                    BLE_new_notify = true;  // trigger the upload here
                                } else {
                                    free(BLE_image_buffer);
                                    if (BLE_err_counter++ >= 5) {  // 5 Retries for a BLE Connection
                                        struct espXferComplete reportStruct;
                                        memcpy((uint8_t*)&reportStruct.src, BLE_curr_address, 8);
                                        processXferComplete(&reportStruct, true);
                                    }
                                }
                            }
                        } else {  // This is a Gicisky display
                            // Here we create the compressed buffer
                            BLE_image_buffer = (uint8_t*)malloc(BUFFER_MAX_SIZE_COMPRESSING);
                            if (BLE_image_buffer == nullptr) {
                                Serial.println("BLE Could not create buffer!");
                                BLE_compressed_len = 0;
                            } else {
                                BLE_compressed_len = compress_image(BLE_curr_address, BLE_image_buffer, BUFFER_MAX_SIZE_COMPRESSING);
                                Serial.printf("BLE Compressed Length: %i\r\n", BLE_compressed_len);
                                // then we connect to BLE to send the compressed data
                                if (BLE_compressed_len && BLE_connect(BLE_curr_address, BLE_TYPE_GICISKY)) {
                                    BLE_err_counter = 0;
                                    BLE_curr_part = 0;
                                    memset(BLE_notify_buffer, 0x00, sizeof(BLE_notify_buffer));
                                    BLE_upload_state = BLE_UPLOAD_STATE_INIT;
                                    ble_main_state = BLE_MAIN_STATE_UPLOAD;
                                    BLE_new_notify = true;  // trigger the upload here
                                } else {
                                    free(BLE_image_buffer);
                                    if (BLE_err_counter++ >= 5) {  // 5 Retries for a BLE Connection
                                        struct espXferComplete reportStruct;
                                        memcpy((uint8_t*)&reportStruct.src, BLE_curr_address, 8);
                                        processXferComplete(&reportStruct, true);
                                    }
                                }
                            }
                        }
                        BLE_last_pending_check = millis();
                    }
                }
                break;
            case BLE_MAIN_STATE_UPLOAD: {
                if (BLE_connected && BLE_new_notify) {
                    BLE_new_notify = false;
                    BLE_last_notify = millis();
                    BLE_upload_state = BLE_notify_buffer[1];

                    switch (BLE_upload_state) {
                        default:
                        case BLE_UPLOAD_STATE_INIT:
                            BLE_mini_buff[0] = 1;
                            ctrlChar->writeValue(BLE_mini_buff, 1);
                            break;
                        case BLE_UPLOAD_STATE_SIZE:
                            BLE_mini_buff[0] = 0x02;
                            BLE_mini_buff[1] = BLE_compressed_len & 0xff;
                            BLE_mini_buff[2] = (BLE_compressed_len >> 8) & 0xff;
                            BLE_mini_buff[3] = (BLE_compressed_len >> 16) & 0xff;
                            BLE_mini_buff[4] = (BLE_compressed_len >> 24) & 0xff;
                            BLE_mini_buff[5] = 0x00;
                            ctrlChar->writeValue(BLE_mini_buff, 6);
                            break;
                        case BLE_UPLOAD_STATE_START:
                            BLE_mini_buff[0] = 0x03;
                            ctrlChar->writeValue(BLE_mini_buff, 1);
                            break;
                        case BLE_UPLOAD_STATE_UPLOAD:
                            if (BLE_notify_buffer[2] == 0x08) {
                                free(BLE_image_buffer);
                                pClient->disconnect();
                                ble_main_state = BLE_MAIN_STATE_IDLE;
                                BLE_last_pending_check = millis();
                                // Done and the image is refreshing now
                                struct espXferComplete reportStruct;
                                memcpy((uint8_t*)&reportStruct.src, BLE_curr_address, 8);
                                processXferComplete(&reportStruct, true);
                                BLE_err_counter = 0;
                                BLE_curr_part = 0;
                            } else {
                                uint32_t req_curr_part = (BLE_notify_buffer[6] << 24) | (BLE_notify_buffer[5] << 24) | (BLE_notify_buffer[4] << 24) | BLE_notify_buffer[3];
                                if (req_curr_part != BLE_curr_part) {
                                    Serial.printf("Something went wrong, expected req part: %i but got: %i we better abort here.\r\n", req_curr_part, BLE_curr_part);
                                    free(BLE_image_buffer);
                                    pClient->disconnect();
                                    ble_main_state = BLE_MAIN_STATE_IDLE;
                                    BLE_last_pending_check = millis();
                                }
                                uint32_t curr_len = 240;
                                if (BLE_compressed_len - (BLE_curr_part * 240) < 240)
                                    curr_len = BLE_compressed_len - (BLE_curr_part * 240);
                                BLE_mini_buff[0] = BLE_curr_part & 0xff;
                                BLE_mini_buff[1] = (BLE_curr_part >> 8) & 0xff;
                                BLE_mini_buff[2] = (BLE_curr_part >> 16) & 0xff;
                                BLE_mini_buff[3] = (BLE_curr_part >> 24) & 0xff;
                                memcpy((uint8_t*)&BLE_mini_buff[4], (uint8_t*)&BLE_image_buffer[BLE_curr_part * 240], curr_len);
                                imgChar->writeValue(BLE_mini_buff, curr_len + 4);
                                Serial.printf("BLE sending part: %i\r\n", BLE_curr_part);
                                BLE_curr_part++;
                            }
                            break;
                    }
                } else {
                    if (millis() - BLE_last_notify > 30000) {  // Something odd, better reset connection!
                        Serial.println("BLE err going back to IDLE");
                        free(BLE_image_buffer);
                        pClient->disconnect();
                        BLE_err_counter = 0;
                        ble_main_state = BLE_MAIN_STATE_IDLE;
                        BLE_last_pending_check = millis();
                    }
                }
                break;
            }
            case BLE_MAIN_STATE_ATC_BLE_OEPL_UPLOAD: {
                if (BLE_connected && BLE_new_notify) {
                    BLE_new_notify = false;
                    BLE_last_notify = millis();
                    switch (BLE_upload_state) {
                        default:
                        case BLE_UPLOAD_STATE_INIT:
                            BLE_mini_buff[0] = 0x00;
                            BLE_mini_buff[1] = 0x64;
                            memcpy((uint8_t*)&BLE_mini_buff[2], &BLEavaildatainfo, sizeof(struct AvailDataInfo));
                            ctrlChar->writeValue(BLE_mini_buff, sizeof(struct AvailDataInfo) + 2);
                            BLE_upload_state = BLE_UPLOAD_STATE_UPLOAD;
                            break;
                        case BLE_UPLOAD_STATE_UPLOAD: {
                            uint8_t notifyLen = BLE_notify_buffer[0];
                            uint16_t notifyCMD = (BLE_notify_buffer[1] << 8) | BLE_notify_buffer[2];
                            Serial.println("BLE CMD " + String(notifyCMD));
                            switch (notifyCMD) {
                                case BLE_CMD_REQ:
                                    if (notifyLen == (sizeof(struct blockRequest) + 2)) {
                                        Serial.println("We got a request for a BLK");
                                        memcpy(&BLEblkRequst, &BLE_notify_buffer[3], sizeof(struct blockRequest));
                                        BLE_curr_part = 0;
                                        ATC_BLE_OEPL_PrepareBlk(BLEblkRequst.blockId);
                                        ATC_BLE_OEPL_SendPart(BLEblkRequst.blockId, BLE_curr_part);
                                    }
                                    break;
                                case BLE_CMD_ACK_BLKPRT:
                                    BLE_curr_part++;
                                    BLE_err_counter = 0;
                                case BLE_CMD_ERR_BLKPRT:
                                    if (BLE_curr_part <= BLE_max_block_parts && BLE_err_counter++ < 15) {
                                        ATC_BLE_OEPL_SendPart(BLEblkRequst.blockId, BLE_curr_part);
                                        break;
                                    }  // FALLTROUGH!!! We cancel the upload if we land here since we dont have so many parts of a block!
                                case BLE_CMD_ACK:
                                case BLE_CMD_ACK_IS_SHOWN:
                                case BLE_CMD_ACK_FW_UPDATED:
                                    Serial.println("BLE Upload done");
                                    free(BLE_image_buffer);
                                    pClient->disconnect();
                                    ble_main_state = BLE_MAIN_STATE_IDLE;
                                    BLE_last_pending_check = millis();
                                    // Done and the image is refreshing now
                                    struct espXferComplete reportStruct;
                                    memcpy((uint8_t*)&reportStruct.src, BLE_curr_address, 8);
                                    processXferComplete(&reportStruct, true);
                                    BLE_err_counter = 0;
                                    BLE_max_block_parts = 0;
                                    BLE_curr_part = 0;
                                    break;
                            }
                        } break;
                    }
                } else {
                    if (millis() - BLE_last_notify > 30000) {  // Something odd, better reset connection!
                        Serial.println("BLE err going back to IDLE");
                        free(BLE_image_buffer);
                        pClient->disconnect();
                        BLE_err_counter = 0;
                        ble_main_state = BLE_MAIN_STATE_IDLE;
                        BLE_last_pending_check = millis();
                    }
                }
                break;
            }
        }
        vTaskDelay(15 / portTICK_PERIOD_MS);
    }
}

#endif
