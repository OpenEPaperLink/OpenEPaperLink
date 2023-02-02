#include "newproto.h"

#include <Arduino.h>
#include <MD5Builder.h>
#include <makeimage.h>
#include <time.h>

#include "LittleFS.h"
#include "commstructs.h"
#include "pendingdata.h"
#include "serial.h"
#include "settings.h"
#include "tag_db.h"
#include "web.h"

extern void sendBlock(const void* data, const uint16_t len);

void addCRC(void* p, uint8_t len) {
    uint8_t total = 0;
    for (uint8_t c = 1; c < len; c++) {
        total += ((uint8_t*)p)[c];
    }
    ((uint8_t*)p)[0] = total;
    // pr("%d",total);
}
bool checkCRC(void* p, uint8_t len) {
    uint8_t total = 0;
    for (uint8_t c = 1; c < len; c++) {
        total += ((uint8_t*)p)[c];
    }
    return ((uint8_t*)p)[0] == total;
}

uint8_t* getDataForFile(fs::File* file) {
    uint8_t* ret = nullptr;
    ret = (uint8_t*)malloc(file->size());
    if (ret) {
        file->seek(0);
        file->readBytes((char*)ret, file->size());
    } else {
        Serial.print("malloc failed for file...\n");
    }
    return ret;
}

void prepareCancelPending(uint64_t ver) {
    struct pendingData pending = {0};
    pending.availdatainfo.dataVer = ver;
    sendCancelPending(&pending);
}

bool prepareDataAvail(String* filename, uint8_t dataType, uint8_t* dst, uint16_t nextCheckin) {

    if (nextCheckin > 1440) {
        //to prevent very long sleeps of the tag
        nextCheckin = 0;
    }

    *filename = "/" + *filename;
    if (!LittleFS.exists(*filename)) return false;
    fs::File file = LittleFS.open(*filename);

    if (file.size() == 0) {
        Serial.print("opened a file with size 0??\n");
        return false;
    }

    if (filename->endsWith(".bmp") || filename->endsWith(".BMP")) {
        struct BitmapFileHeader hdr;
        file.read((uint8_t*)&hdr, sizeof(hdr));
        if (hdr.width == 296 && hdr.height == 128) {
            //sorry, can't rotate
            Serial.println("when using BMP files, remember to only use 128px width and 296px height");
            wsString("when using BMP files, remember to only use 128px width and 296px height");
            return false;
        }
        if (hdr.sig[0] == 'B' && hdr.sig[1] == 'M' && hdr.bpp == 24) {
            Serial.println("converting 24bpp bmp to grays");
            wsString("converting 24bpp bmp to grays");
            char fileout[64];
            sprintf(fileout, "/temp/%02X%02X%02X%02X%02X%02X.bmp\0", dst[5], dst[4], dst[3], dst[2], dst[1], dst[0]);
            bmp2grays(*filename,(String)fileout);
            *filename = (String)fileout;
            file.close();
            file = LittleFS.open(*filename);
        }
    }

    if (filename->endsWith(".jpg") || filename->endsWith(".JPG")) {
        Serial.println("converting jpg to grays");
        wsString("converting jpg to grays");
        char fileout[64];
        sprintf(fileout, "/temp/%02X%02X%02X%02X%02X%02X.bmp\0", dst[5], dst[4], dst[3], dst[2], dst[1], dst[0]);
        jpg2grays(*filename, (String)fileout);
        *filename = (String)fileout;
        file.close();
        file = LittleFS.open(*filename);
    }

    uint8_t md5bytes[16];
    {
        MD5Builder md5;
        md5.begin();
        md5.addStream(file, file.size());
        md5.calculate();
        md5.getBytes(md5bytes);
    }

    uint8_t src[8];
    *((uint64_t*)src) = swap64(*((uint64_t*)dst));
    uint8_t mac[6];
    memcpy(mac, src + 2, sizeof(mac));
    tagRecord* taginfo = nullptr;
    taginfo = tagRecord::findByMAC(mac);
    if (taginfo != nullptr) {
        if (memcmp(md5bytes, taginfo->md5pending, 16) == 0) {
            wsString("new image is the same as current image. not updating tag.");
            wsSendTaginfo(mac);
            return false;
        }
    }

    // the message that will be sent to the AP to tell the tag there is data pending
    struct pendingData pending = {0};
    memcpy(pending.targetMac, dst, 8);
    pending.availdatainfo.dataType = dataType;
    pending.availdatainfo.dataVer = *((uint64_t*)md5bytes);
    pending.availdatainfo.dataSize = file.size();
    pending.availdatainfo.nextCheckIn = nextCheckin;
    pending.attemptsLeft = 10;
    sendDataAvail(&pending);

    // data for the cache on the esp32; needs to hold the data longer than the maximum timeout on the AP
    pendingdata* pendinginfo = nullptr;
    pendinginfo = new pendingdata;
    pendinginfo->filename = *filename;
    pendinginfo->ver = pending.availdatainfo.dataVer;
    pendinginfo->len = pending.availdatainfo.dataSize;
    pendinginfo->data = nullptr;
    pendinginfo->timeout = PENDING_TIMEOUT;
    //pendinginfo->data = getDataForFile(&file);
    pendinginfo->timeout = 1800;  // ***fixme... a tag can sleep for a long time when ttl is used.
    pendingfiles.push_back(pendinginfo);

    if (dataType != DATATYPE_UPDATE) {
        char dst_path[64];
        sprintf(dst_path, "/current/%02X%02X%02X%02X%02X%02X.pending\0", dst[5], dst[4], dst[3], dst[2], dst[1], dst[0]);
        fs::File dstfile = LittleFS.open(dst_path, "w");
        //int bytes_written = dstfile.write(pendinginfo->data, pendinginfo->len);
        file.seek(0);
        const int chunkSize = 512;
        uint8_t buffer[chunkSize];
        size_t bytesRead = 0;
        while ((bytesRead = file.read(buffer, chunkSize)) > 0) {
            dstfile.write(buffer, bytesRead);
        }
        dstfile.close();

        wsString("new image pending: " + String(dst_path));
        if (taginfo != nullptr) {
            taginfo->pending = true;
            taginfo->CheckinInMinPending = nextCheckin + 1;
            memcpy(taginfo->md5pending, md5bytes, sizeof(md5bytes));
            }
        }
        else {
            Serial.println("firmware upload pending");
        }
    file.close();

    wsSendTaginfo(mac);

    return true;
}

void processBlockRequest(struct espBlockRequest* br) {
    if (!checkCRC(br, sizeof(struct espBlockRequest))) {
        Serial.print("Failed CRC on a blockrequest received by the AP");
        return;
    }

    pendingdata* pd = pendingdata::findByVer(br->ver);
    if (pd == nullptr) {
        prepareCancelPending(br->ver);
        Serial.printf("Couldn't find pendingdata info for ver %llu", br->ver);
        return;
    } else {
        if (pd->data == nullptr) {
            // not cached. open file, cache the data
            fs::File file = LittleFS.open(pd->filename);
            if (!file) {
                Serial.print("Dunno how this happened... File pending but deleted in the meantime?\n");
            }
            pd->data = getDataForFile(&file);
            pd->datatimeout = PENDING_DATA_TIMEOUT;
            file.close();
        } else {
            // file is already cached, refresh the timeout
            pd->datatimeout = PENDING_DATA_TIMEOUT;
        }
    }
    // check if we're not exceeding max blocks (to prevent sendBlock from exceeding its boundary)
    uint8_t totalblocks = (pd->len / BLOCK_DATA_SIZE);
    if (pd->len % BLOCK_DATA_SIZE) totalblocks++;
    if (br->blockId >= totalblocks) {
        br->blockId = totalblocks - 1;
    }

    uint32_t len = pd->len - (BLOCK_DATA_SIZE * br->blockId);
    if (len > BLOCK_DATA_SIZE) len = BLOCK_DATA_SIZE;
    sendBlock(pd->data + (br->blockId * BLOCK_DATA_SIZE), len);
    char buffer[64];
    sprintf(buffer, "< Block Request received for MD5 %llu, block %d\n\0", br->ver, br->blockId);
    wsString((String)buffer);
    Serial.printf("<BlockId=%d\n", br->blockId);
}

void processXferComplete(struct espXferComplete* xfc) {
    char buffer[64];
    uint8_t src[8];
    *((uint64_t*)src) = swap64(*((uint64_t*)xfc->src));
    sprintf(buffer, "< %02X%02X%02X%02X%02X%02X reports xfer complete\n\0", src[2], src[3], src[4], src[5], src[6], src[7]);
    wsString((String)buffer);
    Serial.print(buffer);
    uint8_t mac[6];
    memcpy(mac, src + 2, sizeof(mac));

    char src_path[64];
    char dst_path[64];
    char tmp_path[64];
    sprintf(src_path, "/current/%02X%02X%02X%02X%02X%02X.pending\0", src[2], src[3], src[4], src[5], src[6], src[7]);
    sprintf(dst_path, "/current/%02X%02X%02X%02X%02X%02X.bmp\0", src[2], src[3], src[4], src[5], src[6], src[7]);
    sprintf(tmp_path, "/temp/%02X%02X%02X%02X%02X%02X.bmp\0", src[2], src[3], src[4], src[5], src[6], src[7]);
    if (LittleFS.exists(dst_path)) {
        LittleFS.remove(dst_path);
    }
    LittleFS.rename(src_path, dst_path);
    if (LittleFS.exists(tmp_path)) {
        LittleFS.remove(tmp_path);
    }

    time_t now;
    time(&now);
    tagRecord* taginfo = nullptr;
    taginfo = tagRecord::findByMAC(mac);
    if (taginfo != nullptr) {
        taginfo->pending = false;
        taginfo->expectedNextCheckin = now + 60 * taginfo->CheckinInMinPending + 30;
        memcpy(taginfo->md5, taginfo->md5pending, sizeof(taginfo->md5pending));
    }
    wsSendTaginfo(mac);
}

void processDataReq(struct espAvailDataReq* eadr) {
    char buffer[64];
    uint8_t src[8];
    *((uint64_t*)src) = swap64(*((uint64_t*)eadr->src));

    tagRecord* taginfo = nullptr;
    uint8_t mac[6];
    memcpy(mac, src + 2, sizeof(mac));

    taginfo = tagRecord::findByMAC(mac);
    if (taginfo == nullptr) {
        taginfo = new tagRecord;
        memcpy(taginfo->mac, src + 2, sizeof(taginfo->mac));
        taginfo->pending = false;
        tagDB.push_back(taginfo);
    }
    time_t now;
    time(&now);
    taginfo->lastseen = now;
    taginfo->expectedNextCheckin = now + 300;
    taginfo->button = (eadr->adr.buttonState == 1);

    sprintf(buffer, "<ADR %02X%02X%02X%02X%02X%02X\n\0", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    Serial.print(buffer);
    //wsString((String)buffer);
    wsSendTaginfo(mac);
}
