#ifdef HAS_BLE_WRITER
#include <Arduino.h>
#include <MD5Builder.h>

#include <map>

#include "BLEDevice.h"
#include "ble_filter.h"
#include "newproto.h"
#include "od_keys.h"
#include "od_session.h"
#include "tag_db.h"
#include "web.h"

// Continuous passive scan, paused only while the AP holds a BLE connection. The radio is shared
// with WiFi, so it listens a small part of the time (the duty cycle ESPHome's BLE proxies use).
#define BLE_SCAN_INTERVAL_MS 320
#define BLE_SCAN_WINDOW_MS 30
// Once a minute a short active burst: a display that has its data only in the scan response
// is found too (not known for Gicisky's factory firmware; OpenDisplay and ATC don't need it)
#define BLE_ACTIVE_EVERY_MS 60000
#define BLE_ACTIVE_BURST_MS 5000
#define BLE_ACTIVE_INTERVAL_MS 100
#define BLE_ACTIVE_WINDOW_MS 50
#define INTERVAL_HANDLE_PENDING_MS 1000
// a display reports at most this often, unless a button was pressed
#define BLE_CHECKIN_INTERVAL_MS 60000
#define BUFFER_MAX_SIZE_COMPRESSING 135000
// Internal RAM that has to stay free after a transfer buffer is taken. BLE and
// WiFi allocate out of it continuously and cannot use PSRAM, so handing the last
// of it to one upload takes the access point down instead of failing the upload.
#define BLE_TRANSFER_INTERNAL_RESERVE (48 * 1024)
// PSRAM that stays free beside it, for the image conversion's own sprite - the
// same reserve the stored uploads keep, for the same reason.
#define BLE_TRANSFER_PSRAM_RESERVE (512 * 1024)

#define BLE_MAIN_STATE_IDLE 0
#define BLE_MAIN_STATE_PREPARE 1
#define BLE_MAIN_STATE_CONNECT 2
#define BLE_MAIN_STATE_UPLOAD 3
#define BLE_MAIN_STATE_ATC_BLE_OEPL_UPLOAD 4
#define BLE_MAIN_STATE_ATC_BLE_OEPL_CMD 5

// expectedNextCheckin sentinel that the web frontend renders as "In deep sleep"
#define BLE_DEEPSLEEP_CHECKIN 3216153600

// The BLE library waits without a timeout for some events (connecting, registering for notify, a
// write with response). When one of those gets lost, the task hangs until a reboot (#622). A round
// of the task loop takes seconds; an OpenDisplay upload with its refresh a few minutes at most.
#define BLE_TASK_HUNG_MS (10 * 60 * 1000)
static volatile uint32_t BLE_task_alive = 0;

bool BLE_taskHung() {
    const uint32_t alive = BLE_task_alive;
    return alive != 0 && millis() - alive > BLE_TASK_HUNG_MS;
}

// A write the library could not send keeps its semaphore taken, and a failed service search never
// gives its one back: the next call then waits for as long as the display keeps the link. A
// disconnect releases all those waits, so a guarded call that takes too long drops the link.
// Connecting itself is bounded by the BLE stack (CONFIG_BT_BLE_ESTAB_LINK_CONN_TOUT, 30 s).
static esp_timer_handle_t BLE_guardTimer = nullptr;
static esp_bd_addr_t BLE_guardPeer;
static volatile bool BLE_guardFired = false;

static void BLE_guardExpired(void*) {
    BLE_guardFired = true;
    esp_ble_gap_disconnect(BLE_guardPeer);
}

void BLE_guardStart(const uint8_t peer[6], uint32_t timeoutMs) {
    if (BLE_guardTimer == nullptr) {
        const esp_timer_create_args_t args = {.callback = BLE_guardExpired, .arg = nullptr, .dispatch_method = ESP_TIMER_TASK, .name = "BLE guard"};
        if (esp_timer_create(&args, &BLE_guardTimer) != ESP_OK) return;
    }
    esp_timer_stop(BLE_guardTimer);
    memcpy(BLE_guardPeer, peer, sizeof(esp_bd_addr_t));
    BLE_guardFired = false;
    esp_timer_start_once(BLE_guardTimer, (uint64_t)timeoutMs * 1000);
}

void BLE_guardStop() {
    if (BLE_guardTimer != nullptr) esp_timer_stop(BLE_guardTimer);
    if (BLE_guardFired) {
        BLE_guardFired = false;
        Serial.println("BLE call timed out, link dropped");
    }
}

int ble_main_state = BLE_MAIN_STATE_IDLE;
bool BLE_cmd_isDeepsleep = false;

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
// Whether the upload now running is a firmware update, and whether the block
// being transferred is its last. Together they say when a firmware update is
// finished - see ATC_BLE_OEPL_SendLastablePart() below.
static bool BLE_upload_isFw = false;
static bool BLE_upload_lastBlock = false;
struct blockRequest BLEblkRequst = {0};

bool BLE_connected = false;
bool BLE_new_notify = false;

static BLEUUID ATC_BLE_OEPL_ServiceUUID((uint16_t)0x1337);
static BLEUUID ATC_BLE_OEPL_CtrlUUID((uint16_t)0x1337);

static BLEUUID gicServiceUUID((uint16_t)0xfef0);
static BLEUUID gicCtrlUUID((uint16_t)0xfef1);
static BLEUUID gicImgUUID((uint16_t)0xfef2);

BLERemoteCharacteristic* ctrlChar;
BLERemoteCharacteristic* imgChar;
BLEAdvertisedDevice* myDevice;
BLEClient* pClient;

#define BLE_CONNECT_TIMEOUT_MS 45000
#define BLE_WRITE_TIMEOUT_MS 10000

static void BLE_write(BLERemoteCharacteristic* chr, uint8_t* data, size_t len, bool response = false) {
    BLE_guardStart(*pClient->getPeerAddress().getNative(), BLE_WRITE_TIMEOUT_MS);
    chr->writeValue(data, len, response);
    BLE_guardStop();
}

uint8_t BLE_notify_buffer[256] = {0};

uint32_t BLE_err_counter = 0;
uint32_t BLE_curr_part = 0;
uint32_t BLE_max_block_parts = 0;
uint8_t BLE_mini_buff[256];

uint32_t BLE_last_notify = 0;
uint32_t BLE_last_pending_check = 0;
uint8_t BLE_curr_address[8] = {0};

// Connect-failure counter for the command path. We cannot reuse BLE_err_counter
// here because BLE_connect() resets it to 0 on every call, so a command aimed at
// an unreachable / already-sleeping display would otherwise retry forever and
// block all other BLE work.
uint32_t BLE_cmd_conn_fails = 0;

uint32_t BLE_compressed_len = 0;
uint8_t BLE_image_bpp = 0;
uint8_t* BLE_image_buffer = nullptr;

// A transfer buffer, or nullptr when this ESP cannot spare the room.
//
// There is deliberately no size limit here. Whether a display will take a
// transfer is the display's own decision and it already makes it: the tag
// refuses a firmware larger than its update area with BLE_CMD_ERR and an image
// larger than an image slot with BLE_CMD_ERR_FILE_TOO_BIG. An access point that
// second-guesses that needs a copy of the tag's flash map, and a wrong copy is
// what cancelled every firmware update: 135000 bytes of buffer against a 160900
// byte firmware, thrown away without a word.
//
// What is left is a memory question, and it is asked of the heap. PSRAM first,
// because that is where a 200 kB buffer belongs and what these access points
// have; internal RAM only when there is none, and then only while enough stays
// free for the BLE and WiFi stacks, which cannot allocate anywhere else.
// Failing here is safe: the caller logs it and leaves the item queued.
static uint8_t* BLE_allocTransferBuffer(const uint32_t len) {
    if (len == 0) return nullptr;
    if (ESP.getPsramSize() > 0) {
        if (heap_caps_get_free_size(MALLOC_CAP_SPIRAM) < len + BLE_TRANSFER_PSRAM_RESERVE) return nullptr;
        return (uint8_t*)heap_caps_malloc(len, MALLOC_CAP_SPIRAM);
    }
    if (heap_caps_get_free_size(MALLOC_CAP_INTERNAL) < len + BLE_TRANSFER_INTERNAL_RESERVE) return nullptr;
    return (uint8_t*)heap_caps_malloc(len, MALLOC_CAP_INTERNAL);
}

static void BLE_freeImageBuffer() {
    heap_caps_free(BLE_image_buffer);
    BLE_image_buffer = nullptr;
}

// Advertisements discovered during a scan are queued here and processed from
// within BLETask (which has a large stack). The scan callback runs in the
// Bluedroid host task with only a few KB of stack, so doing the full
// processDataReq() there overflows the stack and reboots the ESP32.
#define BLE_DATA_REQ_QUEUE_LEN 64
QueueHandle_t BLE_dataReqQueue = nullptr;

void BLE_enqueue_data_req(struct espAvailDataReq* req) {
    if (BLE_dataReqQueue == nullptr) return;
    // The scan reports every advertisement (a display sends one every second or so): pass one on
    // per minute, and every button press right away. Only the BLE host task gets here.
    static std::map<uint64_t, uint32_t> lastPassed;
    uint64_t key = 0;
    for (int i = 0; i < 8; i++) key = (key << 8) | req->src[i];
    auto it = lastPassed.find(key);
    if (it != lastPassed.end() && req->adr.wakeupReason == 0 && millis() - it->second < BLE_CHECKIN_INTERVAL_MS) return;
    lastPassed[key] = millis();
    // Non-blocking: if the queue is full we simply drop this advertisement,
    // a later one will pick the tag up again. Never block the BLE host task.
    xQueueSend(BLE_dataReqQueue, req, 0);
}

// Emits a websocket log line that starts with the tag MAC (same MAC byte
// order as processBlockRequest). The web frontend blinks the tag card yellow
// ("loading") for every such line, so this gives per-block progress feedback
// during a BLE upload instead of only the single flash at the very end.
static void BLE_reportProgress(const char* what, uint32_t value) {
    char buffer[80];
    sprintf(buffer, "%02X%02X%02X%02X%02X%02X%02X%02X BLE %s %u",
            BLE_curr_address[7], BLE_curr_address[6], BLE_curr_address[5], BLE_curr_address[4],
            BLE_curr_address[3], BLE_curr_address[2], BLE_curr_address[1], BLE_curr_address[0],
            what, value);
    wsLog((String)buffer);
}

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

static bool BLE_connectSession(uint8_t addr[8], BLE_CONNECTION_TYPE conn_type) {
    BLE_err_counter = 0;
    uint8_t temp_Address[] = {addr[5], addr[4], addr[3], addr[2], addr[1], addr[0]};
    Serial.printf("BLE Connecting to: %02X:%02X:%02X:%02X:%02X:%02X\r\n", addr[5], addr[4], addr[3], addr[2], addr[1], addr[0]);
    static MyClientCallback clientCallback;
    // Free the client from the previous transfer. BLEDevice::createClient()
    // does 'new BLEClient()' on every call and never reuses or frees them, so
    // without this every connection attempt leaked one client. We only get here
    // from the IDLE state: the previous transfer already disconnected, the
    // GATTC interface was released in ESP_GATTC_DISCONNECT_EVT, and nothing
    // references the old client anymore, so a plain delete is safe here.
    if (pClient != nullptr) {
        delete pClient;
        pClient = nullptr;
    }
    pClient = BLEDevice::createClient();
    pClient->setClientCallbacks(&clientCallback);
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
    // registerForNotify() on a link that just dropped waits for an event that never comes
    if (ctrlChar->canNotify() && pClient->isConnected()) {
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

bool BLE_connect(uint8_t addr[8], BLE_CONNECTION_TYPE conn_type) {
    const uint8_t peer[] = {addr[5], addr[4], addr[3], addr[2], addr[1], addr[0]};
    BLE_guardStart(peer, BLE_CONNECT_TIMEOUT_MS);
    const bool connected = BLE_connectSession(addr, conn_type);
    BLE_guardStop();
    return connected;
}

class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice advertisedDevice) {
        BLE_filter_add_device(advertisedDevice);
    }
};

static bool BLE_scanning = false;
static bool BLE_scanActive = false;  // mode of the running scan
static uint32_t BLE_burstStart = 0;

static void BLE_scanEnded(BLEScanResults) {
    BLE_scanning = false;
}

// Keeps the background scan running in the mode it should have now: passive, or the active burst
static void BLE_scanRun() {
    const uint32_t now = millis();
    if (now - BLE_burstStart >= BLE_ACTIVE_EVERY_MS) BLE_burstStart = now;
    const bool active = now - BLE_burstStart < BLE_ACTIVE_BURST_MS;
    if (BLE_scanning && BLE_scanActive == active) return;
    BLE_scanStop();  // the scan parameters can't change while it runs
    static MyAdvertisedDeviceCallbacks advertisedDeviceCallbacks;
    BLEScan* pBLEScan = BLEDevice::getScan();
    // every advertisement, also repeats: a button press is a repeat of a device already seen
    pBLEScan->setAdvertisedDeviceCallbacks(&advertisedDeviceCallbacks, true);
    pBLEScan->setInterval(active ? BLE_ACTIVE_INTERVAL_MS : BLE_SCAN_INTERVAL_MS);
    pBLEScan->setWindow(active ? BLE_ACTIVE_WINDOW_MS : BLE_SCAN_WINDOW_MS);
    pBLEScan->setActiveScan(active);
    BLE_scanActive = active;
    BLE_scanning = pBLEScan->start(0, BLE_scanEnded, false);  // 0: until stopped, doesn't block
}

void BLE_scanStop() {
    if (!BLE_scanning) return;
    BLEDevice::getScan()->stop();
    BLE_scanning = false;
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
    BLE_write(ctrlChar, tempPacketBuffer, sizeof(tempPacketBuffer), true);
}

// True when the item queued for BLE_curr_address is no longer the one this
// upload was about.
//
// A report of either kind carries nothing but the MAC: processXferComplete()
// dequeues whichever item is first in line for it, and processXferTimeout()
// drops every item it has. That is fine as long as the item that was worked on
// is still the item that is queued - and it is not, whenever newer content was
// drawn in the meantime, because queueDataAvail() retires the old one on
// purpose ("a new image replaces the images that are still queued for this
// tag"). An upload that finishes a moment later then acknowledges content the
// display has never seen, and the access point stores the old dataVer as its
// md5, so the new content is never offered again.
//
// The radio side has offeredMatches() for exactly this. It cannot help here:
// it reads offeredDataVer, which is filled from setAnnounced(), and a BLE
// display is never announced to the radio - sendDataAvail() returns false for
// one. So the BLE writer has to make the comparison itself.
//
// deliveredVer of 0 means "no dataVer of its own to compare", which is the
// case for a command frame and for an OpenDisplay session; those report as
// before.
static bool BLE_itemWasReplaced(const uint64_t deliveredVer) {
    if (deliveredVer == 0) return false;
    const PendingItem* queued = getQueueItem(BLE_curr_address);
    if (queued == nullptr) return false;
    return queued->pendingdata.availdatainfo.dataVer != deliveredVer;
}

// Says so once, with the MAC, in the same shape as the progress lines.
static void BLE_reportSuperseded(const char* what) {
    char buffer[96];
    sprintf(buffer, "%02X%02X%02X%02X%02X%02X%02X%02X BLE %s, newer content kept",
            BLE_curr_address[7], BLE_curr_address[6], BLE_curr_address[5], BLE_curr_address[4],
            BLE_curr_address[3], BLE_curr_address[2], BLE_curr_address[1], BLE_curr_address[0], what);
    wsLog((String)buffer);
}

// The queued item for BLE_curr_address reached the display: clear it and count the update.
static void BLE_reportDelivered(const uint64_t deliveredVer = 0) {
    if (BLE_itemWasReplaced(deliveredVer)) {
        // Not acknowledged: the newer item stays queued and is picked up on the
        // next pending check, a second later.
        BLE_reportSuperseded("upload finished for replaced data");
        BLE_clear_attempts(BLE_curr_address);
        return;
    }
    struct espXferComplete reportStruct;
    memcpy((uint8_t*)&reportStruct.src, BLE_curr_address, 8);
    processXferComplete(&reportStruct, true);
    BLE_clear_attempts(BLE_curr_address);
}

// The item could not be delivered. Reporting it complete would make it the image the AP thinks the
// display shows: its preview, its hash and one more update. The next run of the same content is
// then "the same as the current image" and never queued again, so a display the AP never reached
// keeps its old image for good, while the web interface shows the new one. A timeout clears the
// queue, logs that the transfer failed and leaves the hash of the image on the display alone.
static void BLE_reportFailed(const uint64_t deliveredVer = 0) {
    if (BLE_itemWasReplaced(deliveredVer)) {
        // Worse here than on the success path: processXferTimeout() calls
        // dequeueAllItems(), so reporting a failure for an item that is already
        // gone would throw away the newer content as well.
        BLE_reportSuperseded("upload failed for replaced data");
        BLE_clear_attempts(BLE_curr_address);
        return;
    }
    struct espXferComplete reportStruct;
    memcpy((uint8_t*)&reportStruct.src, BLE_curr_address, 8);
    processXferTimeout(&reportStruct, true);
    BLE_clear_attempts(BLE_curr_address);
}

static void BLE_endAtcUpload(bool delivered);

// One part on its way, and for a firmware update the acknowledgement that goes
// with the last of them.
//
// The access point acknowledges a firmware transfer itself rather than waiting
// to be told. The display answers the last block with BLE_CMD_ACK_FW_UPDATED
// and reboots 100 ms later (the DATATYPE_FW_UPDATE case in the tag's
// app_bleCmdHandle.c), so that notify is lost about as often as it arrives and
// the link goes down in either case - which is indistinguishable from an upload
// that failed. Left to wait for it, the item stayed queued and the whole 160 kB
// went out again, against a display that was already running it. Measured three
// rounds in a row, and it does not stop by itself.
//
// This is the last moment there is anything to know: parts go out as
// write-with-response, so when the write returns the display has taken the
// packet at the ATT layer, and the display asks for blocks in order, so the
// last block means every earlier one arrived.
//
// What it gives up: if the display were to find the last block's checksum
// wrong, it would ask for that block again, and by then the item is gone. The
// deep-sleep command is handled the same way a few lines down and for the same
// reason - after some commands a display simply has nothing left to say.
static void ATC_BLE_OEPL_SendLastablePart(uint8_t blockId, uint8_t part) {
    ATC_BLE_OEPL_SendPart(blockId, part);
    if (!BLE_upload_isFw || !BLE_upload_lastBlock) return;
    if (part + 1 < BLE_max_block_parts) return;
    char buffer[96];
    sprintf(buffer, "%02X%02X%02X%02X%02X%02X%02X%02X firmware sent in full, rebooting into it",
            BLE_curr_address[7], BLE_curr_address[6], BLE_curr_address[5], BLE_curr_address[4],
            BLE_curr_address[3], BLE_curr_address[2], BLE_curr_address[1], BLE_curr_address[0]);
    wsLog((String)buffer);
    BLE_endAtcUpload(true);
}

// Leave an ATC BLE OEPL image upload: free the buffer, drop the link and report what happened
static void BLE_endAtcUpload(bool delivered) {
    // Reported here, so a disconnect that follows must not report it again.
    BLE_upload_isFw = false;
    BLE_upload_lastBlock = false;
    BLE_freeImageBuffer();
    pClient->disconnect();
    ble_main_state = BLE_MAIN_STATE_IDLE;
    BLE_last_pending_check = millis();
    // BLEavaildatainfo.dataVer is the md5 of the buffer that was actually sent,
    // which is what the report has to be checked against.
    if (delivered) {
        BLE_reportDelivered(BLEavaildatainfo.dataVer);
    } else {
        BLE_reportFailed(BLEavaildatainfo.dataVer);
    }
    BLE_err_counter = 0;
    BLE_max_block_parts = 0;
    BLE_curr_part = 0;
}

// Finish handling a queued command for BLE_curr_address: report the transfer as
// complete (clears the pending item), reset the retry counters, and - for a deep
// sleep - flag the tag as sleeping so the UI shows "In deep sleep" until it is
// seen in a scan again.
static void BLE_finishCommand(bool markSleep) {
    BLE_reportDelivered();
    BLE_err_counter = 0;
    BLE_cmd_conn_fails = 0;
    if (markSleep) {
        tagRecord* taginfo = tagRecord::findByMAC(BLE_curr_address);
        if (taginfo != nullptr) {
            taginfo->expectedNextCheckin = BLE_DEEPSLEEP_CHECKIN;
            wsSendTaginfo(BLE_curr_address, SYNC_TAGSTATUS);
        }
    }
}

void BLETask(void* parameter) {
    vTaskDelay(5000 / portTICK_PERIOD_MS);
    Serial.println("BLE task started");
    BLE_dataReqQueue = xQueueCreate(BLE_DATA_REQ_QUEUE_LEN, sizeof(struct espAvailDataReq));
    BLEDevice::init("");
    odInit();
    while (1) {
        BLE_task_alive = millis();
        // Process advertisements collected by the scan callback here, in this
        // task's large stack, instead of inside the tiny BLE host task stack.
        struct espAvailDataReq incomingDataReq;
        while (BLE_dataReqQueue != nullptr && xQueueReceive(BLE_dataReqQueue, &incomingDataReq, 0) == pdTRUE) {
            // Every AP in range hears a BLE display. One added here goes by the roaming rules
            // (odLocalCheckin); one that isn't added here and that another AP already has stays
            // with that AP, instead of moving between them with every check-in.
            if (bleFamily(incomingDataReq.src, 0) == BLE_FAMILY_OPENDISPLAY) {
                if (odKeyAdded(incomingDataReq.src)) {
                    if (!odLocalCheckin(incomingDataReq.src)) continue;
                } else {
                    const tagRecord* taginfo = tagRecord::findByMAC(incomingDataReq.src);
                    if (taginfo != nullptr && taginfo->isExternal) continue;
                }
            }
            processDataReq(&incomingDataReq, true, IPAddress(0, 0, 0, 0), CHECKIN_VIA_BLE);
        }
        switch (ble_main_state) {
            default:
            case BLE_MAIN_STATE_IDLE:
                // onDisconnect() puts us back in IDLE in the middle of an upload; the
                // upload states free the buffer on every other way out
                if (BLE_image_buffer != nullptr) {
                    Serial.println("BLE upload ended by a disconnect");
                    // A firmware update always ends exactly like this. The display
                    // acknowledges the last block with BLE_CMD_ACK_FW_UPDATED and then
                    // reboots 100 ms later (the DATATYPE_FW_UPDATE case in the tag's
                    // app_bleCmdHandle.c), so the link goes down with that notify still
                    // in flight and it is lost about as often as it arrives. A dropped
                    // link reports nothing, so the item stayed queued and the whole
                    // 160 kB went out again - against a display already running it.
                    // Measured: three times over in a row.
                    //
                    // Having served the last block is the proof of delivery here. The
                    // display asks for blocks in order, so the last request means all of
                    // it arrived, and nothing after that can be heard from a display
                    // that is busy becoming the firmware. The deep-sleep command is
                    // treated the same way a few lines down, for the same reason.
                    const bool fwDelivered = BLE_upload_isFw && BLE_upload_lastBlock;
                    BLE_freeImageBuffer();
                    if (fwDelivered) {
                        char buffer[96];
                        sprintf(buffer, "%02X%02X%02X%02X%02X%02X%02X%02X firmware delivered, rebooting into it",
                                BLE_curr_address[7], BLE_curr_address[6], BLE_curr_address[5], BLE_curr_address[4],
                                BLE_curr_address[3], BLE_curr_address[2], BLE_curr_address[1], BLE_curr_address[0]);
                        wsLog((String)buffer);
                        BLE_reportDelivered(BLEavaildatainfo.dataVer);
                    }
                    BLE_upload_isFw = false;
                    BLE_upload_lastBlock = false;
                }
                BLE_scanRun();
                if (millis() - BLE_last_pending_check >= INTERVAL_HANDLE_PENDING_MS) {
                    // only tags whose pending item is queued, so the image is ready
                    if (BLE_is_image_pending(BLE_curr_address)) {
                        BLE_scanStop();  // no scanning while connected; IDLE starts it again
                        if (bleFamily(BLE_curr_address, 0) == BLE_FAMILY_OPENDISPLAY) {
                            // one blocking session: connect, authenticate, send, wait for the refresh
                            if (odUpload(BLE_curr_address)) {
                                BLE_reportDelivered();
                            } else if (BLE_get_attempts(BLE_curr_address) >= 5) {  // 5 tries per display
                                BLE_reportFailed();
                            }
                        } else if (bleFamily(BLE_curr_address, 0) == BLE_FAMILY_ATC) {  // This is an ATC BLE OEPL display
                            // A pending item may be a command (deep sleep, reboot, reset, ...)
                            // instead of an image. Handle that first and skip the image upload
                            // path when it is a command.
                            uint8_t cmdFrame[2 + sizeof(struct AvailDataInfo)];
                            uint8_t cmdFrameLen = 0;
                            bool isDeepsleep = false;
                            if (get_ATC_BLE_OEPL_command(BLE_curr_address, cmdFrame, &cmdFrameLen, &isDeepsleep)) {
                                if (cmdFrameLen == 0) {
                                    // No BLE equivalent (e.g. a network scan); just clear it so
                                    // the queue does not get stuck retrying it forever.
                                    Serial.println("BLE command has no BLE equivalent, clearing it");
                                    BLE_finishCommand(false);
                                } else if (BLE_connect(BLE_curr_address, BLE_TYPE_ATC_BLE_OEPL)) {
                                    BLE_cmd_conn_fails = 0;
                                    memset(BLE_notify_buffer, 0x00, sizeof(BLE_notify_buffer));
                                    BLE_new_notify = false;
                                    BLE_cmd_isDeepsleep = isDeepsleep;
                                    BLE_write(ctrlChar, cmdFrame, cmdFrameLen, true);  // write-with-response
                                    Serial.printf("BLE command frame sent (%u bytes)\r\n", cmdFrameLen);
                                    if (isDeepsleep) {
                                        // The display shuts its BLE stack down right after ACK-ing a
                                        // deep sleep, so we usually can't read the ACK before it
                                        // disconnects. Having connected and written the command (with
                                        // response) is proof enough of delivery, so finish here -
                                        // otherwise reconnect attempts to the now sleeping display
                                        // would loop and block all other BLE work.
                                        pClient->disconnect();
                                        ble_main_state = BLE_MAIN_STATE_IDLE;
                                        BLE_finishCommand(true);
                                    } else {
                                        BLE_last_notify = millis();
                                        ble_main_state = BLE_MAIN_STATE_ATC_BLE_OEPL_CMD;
                                    }
                                } else {
                                    // Connect failed. Give up after a few tries so a command aimed
                                    // at an unreachable / already-sleeping display cannot block the
                                    // BLE task forever (BLE_connect resets BLE_err_counter, hence the
                                    // separate counter here).
                                    if (++BLE_cmd_conn_fails >= 5) {
                                        Serial.println("BLE command: giving up after repeated connect failures");
                                        BLE_cmd_conn_fails = 0;
                                        BLE_err_counter = 0;
                                        BLE_reportFailed();
                                    }
                                }
                                BLE_last_pending_check = millis();
                                break;  // handled this pending item, skip the image path
                            }
                            // Sized to the item that is waiting, not to one constant.
                            // An ATC display is sent what is queued byte for byte - the
                            // compression happened when the image was made - so the
                            // buffer has to hold exactly that, and a firmware is bigger
                            // than BUFFER_MAX_SIZE_COMPRESSING ever allowed for (135000
                            // against 160900 bytes, which is what silently cancelled
                            // every firmware update).
                            //
                            // Costs the firmware twice over while the upload runs: once
                            // in the queue item, once here. That is the existing design -
                            // get_ATC_BLE_OEPL_image() memcpy's from the one into the
                            // other - and on an access point without PSRAM a firmware
                            // update will fail on this allocation. It now says so in the
                            // log rather than looking like a delivery.
                            const uint32_t bufferLen = get_ATC_BLE_OEPL_pending_len(BLE_curr_address);
                            BLE_image_buffer = BLE_allocTransferBuffer(bufferLen);
                            if (BLE_image_buffer == nullptr) {
                                char hexmac[17];
                                mac2hex(BLE_curr_address, hexmac);
                                wsErr(String(hexmac) + " BLE: no memory for a " + String(bufferLen) +
                                      " byte transfer, leaving it queued");
                                Serial.println("BLE Could not create buffer!");
                                BLE_compressed_len = 0;
                            } else {
                                uint8_t dataType = 0x00;
                                uint8_t dataTypeArgument = 0x00;
                                uint16_t nextCheckin = 0x00;
                                uint64_t itemDataVer = 0;
                                BLE_compressed_len = get_ATC_BLE_OEPL_image(BLE_curr_address, BLE_image_buffer, bufferLen, &dataType, &dataTypeArgument, &nextCheckin, &itemDataVer);
                                Serial.printf("BLE data Length: %i\r\n", BLE_compressed_len);
                                // then we connect to BLE to send the compressed data
                                if (BLE_compressed_len && BLE_connect(BLE_curr_address, BLE_TYPE_ATC_BLE_OEPL)) {
                                    BLE_err_counter = 0;
                                    BLE_curr_part = 0;
                                    memset(BLE_notify_buffer, 0x00, sizeof(BLE_notify_buffer));

                                    BLEavaildatainfo.dataType = dataType;
                                    BLE_upload_isFw = dataType == DATATYPE_FW_UPDATE;
                                    BLE_upload_lastBlock = false;
                                    // The item's own version, not one computed again here: MD5Builder::add()
                                    // takes a uint16_t length, so a 160900 byte firmware was hashed over its
                                    // first 29828 bytes and the display was handed a version that matched
                                    // nothing. The delivery could then never be recognised as the item it
                                    // belonged to, and the whole firmware went out again, round after round.
                                    BLEavaildatainfo.dataVer = itemDataVer;
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
                                    BLE_freeImageBuffer();
                                    // Said out loud, because this is where the wait between a
                                    // display asking for content and getting it is spent, and
                                    // nothing above the serial port used to show it.
                                    char buffer[96];
                                    sprintf(buffer, "%02X%02X%02X%02X%02X%02X%02X%02X BLE connect failed, try %u",
                                            BLE_curr_address[7], BLE_curr_address[6], BLE_curr_address[5], BLE_curr_address[4],
                                            BLE_curr_address[3], BLE_curr_address[2], BLE_curr_address[1], BLE_curr_address[0],
                                            BLE_get_attempts(BLE_curr_address));
                                    wsLog((String)buffer);
                                    if (BLE_get_attempts(BLE_curr_address) >= 5) {  // 5 tries per display (BLE_connect() resets BLE_err_counter)
                                        BLE_reportFailed();
                                    }
                                }
                            }
                        } else {  // This is a Gicisky display
                            // Here we create the compressed buffer
                            // A Gicisky frame is compressed in place here, so this one is
                            // scratch space of an unknown size and keeps its constant.
                            BLE_image_buffer = BLE_allocTransferBuffer(BUFFER_MAX_SIZE_COMPRESSING);
                            if (BLE_image_buffer == nullptr) {
                                Serial.println("BLE Could not create buffer!");
                                BLE_compressed_len = 0;
                            } else {
                                tagRecord* taginfo = tagRecord::findByMAC(BLE_curr_address);
                                BLE_image_bpp = taginfo == nullptr ? 0 : getHwType(taginfo->hwType).bpp;
                                BLE_compressed_len = compress_image(BLE_curr_address, BLE_image_buffer, BUFFER_MAX_SIZE_COMPRESSING, BLE_image_bpp);
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
                                    BLE_freeImageBuffer();
                                    if (BLE_get_attempts(BLE_curr_address) >= 5) {  // 5 tries per display (BLE_connect() resets BLE_err_counter)
                                        BLE_reportFailed();
                                    }
                                }
                            }
                        }
                        BLE_last_pending_check = millis();
                    }
                }
                if (ble_main_state == BLE_MAIN_STATE_IDLE) odService();
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
                            BLE_write(ctrlChar, BLE_mini_buff, 1);
                            break;
                        case BLE_UPLOAD_STATE_SIZE:
                            BLE_mini_buff[0] = 0x02;
                            BLE_mini_buff[1] = BLE_compressed_len & 0xff;
                            BLE_mini_buff[2] = (BLE_compressed_len >> 8) & 0xff;
                            BLE_mini_buff[3] = (BLE_compressed_len >> 16) & 0xff;
                            BLE_mini_buff[4] = (BLE_compressed_len >> 24) & 0xff;
                            BLE_mini_buff[5] = 0x00;
                            if (BLE_image_bpp == BPP_PACKED_2BIT) {
                                // Packed 2-bit displays use the complete 8-byte size
                                // command: 0x02, uint32 LE size, then 3 zeros.
                                BLE_mini_buff[6] = 0x00;
                                BLE_mini_buff[7] = 0x00;
                                BLE_write(ctrlChar, BLE_mini_buff, 8);
                            } else {
                                BLE_write(ctrlChar, BLE_mini_buff, 6);
                            }
                            break;
                        case BLE_UPLOAD_STATE_START:
                            BLE_mini_buff[0] = 0x03;
                            BLE_write(ctrlChar, BLE_mini_buff, 1);
                            break;
                        case BLE_UPLOAD_STATE_UPLOAD:
                            if (BLE_notify_buffer[2] == 0x08) {
                                BLE_freeImageBuffer();
                                pClient->disconnect();
                                ble_main_state = BLE_MAIN_STATE_IDLE;
                                BLE_last_pending_check = millis();
                                // Done and the image is refreshing now
                                BLE_reportDelivered();
                                BLE_err_counter = 0;
                                BLE_curr_part = 0;
                            } else {
                                uint32_t req_curr_part = (BLE_notify_buffer[6] << 24) | (BLE_notify_buffer[5] << 16) | (BLE_notify_buffer[4] << 8) | BLE_notify_buffer[3];
                                if (req_curr_part != BLE_curr_part) {
                                    Serial.printf("Something went wrong, expected req part: %i but got: %i we better abort here.\r\n", req_curr_part, BLE_curr_part);
                                    BLE_freeImageBuffer();
                                    pClient->disconnect();
                                    ble_main_state = BLE_MAIN_STATE_IDLE;
                                    BLE_last_pending_check = millis();
                                    break;  // buffer is freed and we left the upload state, do not fall through into a use-after-free
                                }
                                uint32_t curr_len = 240;
                                if (BLE_compressed_len - (BLE_curr_part * 240) < 240)
                                    curr_len = BLE_compressed_len - (BLE_curr_part * 240);
                                BLE_mini_buff[0] = BLE_curr_part & 0xff;
                                BLE_mini_buff[1] = (BLE_curr_part >> 8) & 0xff;
                                BLE_mini_buff[2] = (BLE_curr_part >> 16) & 0xff;
                                BLE_mini_buff[3] = (BLE_curr_part >> 24) & 0xff;
                                memcpy((uint8_t*)&BLE_mini_buff[4], (uint8_t*)&BLE_image_buffer[BLE_curr_part * 240], curr_len);
                                BLE_write(imgChar, BLE_mini_buff, curr_len + 4);
                                Serial.printf("BLE sending part: %i\r\n", BLE_curr_part);
                                uint32_t byteOffset = BLE_curr_part * 240;
                                if (byteOffset % BLOCK_DATA_SIZE < 240)  // once per 4096-byte block, same cadence as the 802.15.4 path
                                    BLE_reportProgress("block", byteOffset / BLOCK_DATA_SIZE);
                                uint16_t totalParts = (BLE_compressed_len + 239) / 240;
                                wsSendUploadProgress(BLE_curr_address, BLE_curr_part + 1, totalParts);  // fine-grained part x/total for the tag card
                                BLE_curr_part++;
                            }
                            break;
                    }
                } else {
                    if (millis() - BLE_last_notify > 30000) {  // Something odd, better reset connection!
                        Serial.println("BLE err going back to IDLE");
                        BLE_freeImageBuffer();
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
                            BLE_write(ctrlChar, BLE_mini_buff, sizeof(struct AvailDataInfo) + 2);
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
                                        BLE_reportProgress("block", BLEblkRequst.blockId);
                                        uint16_t totalBlocks = (BLE_compressed_len + BLOCK_DATA_SIZE_BLE - 1) / BLOCK_DATA_SIZE_BLE;
                                        // The display asks for blocks in order, so being asked
                                        // for the last one means every earlier one arrived.
                                        BLE_upload_lastBlock = BLEblkRequst.blockId + 1 >= totalBlocks;
                                        wsSendUploadProgress(BLE_curr_address, BLEblkRequst.blockId + 1, totalBlocks);
                                        ATC_BLE_OEPL_SendLastablePart(BLEblkRequst.blockId, BLE_curr_part);
                                    }
                                    break;
                                case BLE_CMD_ACK_BLKPRT:
                                    BLE_curr_part++;
                                    BLE_err_counter = 0;
                                case BLE_CMD_ERR_BLKPRT:
                                    if (BLE_curr_part <= BLE_max_block_parts && BLE_err_counter++ < 15) {
                                        ATC_BLE_OEPL_SendLastablePart(BLEblkRequst.blockId, BLE_curr_part);
                                        break;
                                    }
                                    // too many errors on one block: the display didn't get the image
                                    Serial.println("BLE upload failed, too many block part errors");
                                    BLE_endAtcUpload(false);
                                    break;
                                case BLE_CMD_ACK:
                                case BLE_CMD_ACK_IS_SHOWN:
                                case BLE_CMD_ACK_FW_UPDATED:
                                    Serial.println("BLE Upload done");
                                    BLE_endAtcUpload(true);  // the image is refreshing now
                                    break;
                            }
                        } break;
                    }
                } else {
                    if (millis() - BLE_last_notify > 30000) {  // Something odd, better reset connection!
                        Serial.println("BLE err going back to IDLE");
                        // The same thing the IDLE branch reads: a firmware update whose
                        // last block was served and that then went quiet has arrived.
                        const bool fwDelivered = BLE_upload_isFw && BLE_upload_lastBlock;
                        BLE_freeImageBuffer();
                        pClient->disconnect();
                        BLE_err_counter = 0;
                        ble_main_state = BLE_MAIN_STATE_IDLE;
                        BLE_last_pending_check = millis();
                        if (fwDelivered) {
                            char buffer[96];
                            sprintf(buffer, "%02X%02X%02X%02X%02X%02X%02X%02X firmware delivered, rebooting into it",
                                    BLE_curr_address[7], BLE_curr_address[6], BLE_curr_address[5], BLE_curr_address[4],
                                    BLE_curr_address[3], BLE_curr_address[2], BLE_curr_address[1], BLE_curr_address[0]);
                            wsLog((String)buffer);
                            BLE_reportDelivered(BLEavaildatainfo.dataVer);
                        }
                        BLE_upload_isFw = false;
                        BLE_upload_lastBlock = false;
                    }
                }
                break;
            }
            case BLE_MAIN_STATE_ATC_BLE_OEPL_CMD: {
                // We sent a single command frame and now wait for the display's
                // acknowledgement notify. Any reply (BLE_CMD_ACK_CMD / BLE_CMD_ACK)
                // means the command was accepted. No image buffer is involved here,
                // so we must never free BLE_image_buffer in this state.
                bool finish = false;
                if (BLE_connected && BLE_new_notify) {
                    BLE_new_notify = false;
                    uint16_t notifyCMD = (BLE_notify_buffer[1] << 8) | BLE_notify_buffer[2];
                    Serial.println("BLE command reply " + String(notifyCMD));
                    finish = true;
                } else if (millis() - BLE_last_notify > 30000) {
                    // A deep-sleeping display stops its BLE stack right after ACK-ing,
                    // so a missed notify is expected there; treat the timeout as done.
                    Serial.println("BLE command timeout, going back to IDLE");
                    finish = true;
                }
                if (finish) {
                    pClient->disconnect();
                    ble_main_state = BLE_MAIN_STATE_IDLE;
                    BLE_last_pending_check = millis();
                    // The display stops advertising after a deep sleep; show it as sleeping
                    // until it is found in a scan again (see processDataReq wake handling).
                    BLE_finishCommand(BLE_cmd_isDeepsleep);
                    BLE_cmd_isDeepsleep = false;
                }
                break;
            }
        }
        vTaskDelay(15 / portTICK_PERIOD_MS);
    }
}

#endif
