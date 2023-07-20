#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "core_cm3.h"
#include "main.h"
#include "mz100_pinmux.h"
#include "mz100_gpio.h"
#include "mz100_uart.h"
#include "mz100_clock.h"
#include "mz100_sleep.h"
#include "mz100_flash.h"
#include "epd.h"
#include "mz100_ssp.h"
#include "mz100_pmu.h"
#include "core_cminstr.h"
#include "zigbee.h"
#include "util.h"
#include "settings.h"
#include "eeprom.h"
#include "proto.h"
#include "comms.h"
#include "chars.h"
#include "mz100.h"
#include "timer.h"
#include "util.h"
#include "ccm.h"
#include "nfc.h"
#include "gpio.h"
#include "compression.h"
#include "syncedproto.h"
#include "proto.h"
#include "powermgt.h"

#define SW_VER_CURRENT (0x0000011300000000ull) // top 16 bits are off limits, xxxx.VV.tt.vvvv.mmmm means version V.t.v.m
#define SW_DEFAULT_MAC (0x0000000000000014ull)

uint64_t __attribute__((section(".ver"))) mCurVersionExport = SW_VER_CURRENT;
uint64_t __attribute__((section(".default_mac"))) default_mac = SW_DEFAULT_MAC;

char macStr[32];
char macStr1[32];
uint8_t mSelfMac[8];

struct BitmapFileHeader
{
    uint8_t sig[2];
    uint32_t fileSz;
    uint8_t rfu[4];
    uint32_t dataOfst;
    uint32_t headerSz; // 40
    int32_t width;
    int32_t height;
    uint16_t colorplanes; // must be one
    uint16_t bpp;
    uint32_t compression;
    uint32_t dataLen; // may be 0
    uint32_t pixelsPerMeterX;
    uint32_t pixelsPerMeterY;
    uint32_t numColors; // if zero, assume 2^bpp
    uint32_t numImportantColors;

} __attribute__((packed));

struct BitmapClutEntry
{
    uint8_t b, g, r, x;
} __attribute__((packed));

void prvEepromIndex(struct EepromContentsInfo *eci)
{
    struct EepromImageHeaderOld eih;
    uint32_t addr;

    for (addr = EEPROM_IMG_START; addr - EEPROM_IMG_START < EEPROM_IMG_LEN; addr += EEPROM_IMG_EACH)
    {

        uint32_t *addrP, *szP = NULL;
        uint64_t *verP = NULL;

        FLASH_Read(0, addr, (uint8_t *)&eih, sizeof(struct EepromImageHeaderOld));
        printf("DATA slot 0x%06x: type 0x%08x ver 0x%08x%08x\r\n", addr, eih.validMarker, (uint32_t)(eih.version >> 32), (uint32_t)eih.version);

        switch (eih.validMarker)
        {
        case EEPROM_IMG_INPROGRESS:
            verP = &eci->latestInprogressImgVer;
            addrP = &eci->latestInprogressImgAddr;
            break;

        case EEPROM_IMG_VALID:
            verP = &eci->latestCompleteImgVer;
            addrP = &eci->latestCompleteImgAddr;
            szP = &eci->latestCompleteImgSize;
            break;
        }

        if (verP && eih.version >= *verP)
        {
            *verP = eih.version;
            *addrP = addr;
            if (szP)
                *szP = eih.size;
        }
    }
}

static bool prvApplyUpdateIfNeeded(struct Settings *settings) // return true if a reboot is needed
{
    struct EepromImageHeaderOld eih;
    uint32_t ofst, now, size, pieceSz = 0x2000;
    uint8_t chunkStore[0x2000];
    bool reboot = false;

    FLASH_Read(0, EEPROM_UPDATE_START, (uint8_t *)&eih, sizeof(struct EepromImageHeaderOld));

    if (eih.validMarker != EEPROM_IMG_VALID)
    {
        printf("Not IMG_VALID 0x%08x\r\n", eih.validMarker);
        return false;
    }

    if (eih.version > mCurVersionExport)
    {
        printf("Applying update to ver 0x%08x%08x\r\n", (uint32_t)(eih.version >> 32), (uint32_t)eih.version);

        printf("Erz 0x%06x .. 0x%06x\r\n", EEPROM_OS_START, EEPROM_OS_START + eih.size - 1);
        qspiEraseRange(EEPROM_OS_START, eih.size);

        size = eih.size;
        // from now on, eih (screenbuffer) will be reused for temp storage

        for (ofst = 0; ofst < size; ofst += now)
        {

            now = size - ofst;
            if (now > pieceSz)
                now = pieceSz;

            printf("Cpy 0x%06x + 0x%04x to 0x%06x\r\n", EEPROM_UPDATE_START + ofst + sizeof(struct EepromImageHeaderOld), now, EEPROM_OS_START + ofst);
            FLASH_Read(0, EEPROM_UPDATE_START + ofst + sizeof(struct EepromImageHeaderOld), chunkStore, now);
            FLASH_Write(false, EEPROM_OS_START + ofst, chunkStore, now);

            WDT_RestartCounter();
        }

        reboot = true;
    }

    settings->isPaired = 0;
    printf("Erz IMAGES\r\n");
    qspiEraseRange(EEPROM_IMG_START, EEPROM_IMG_LEN);
    printf("Erz update\r\n");
    qspiEraseRange(EEPROM_UPDATE_START, EEPROM_UPDATE_LEN);
    return reboot;
}

static void prvFillTagState(struct Settings *settings, struct TagState *state)
{
    state->hwType = HW_TYPE_74_INCH_BWR;
    state->swVer = mCurVersionExport;
    state->batteryMv = measureBattery();
}

static bool prvSendCheckin(struct Settings *settings, struct CommsInfo *ci, struct PendingInfo *out)
{
    struct
    {
        uint8_t pktTyp;
        struct CheckinInfo cii;
    } packet = {};
    uint8_t rx[COMMS_MAX_PACKET_SZ], fromMac[8];
    uint64_t now;

    packet.pktTyp = PKT_CHECKIN;
    prvFillTagState(settings, &packet.cii.state);

    packet.cii.lastPacketLQI = settings->lastRxedLQI;
    packet.cii.lastPacketRSSI = settings->lastRxedRSSI;
    packet.cii.temperature = measureTemp();
    packet.cii.rfu[0] = 1;

    if (!commsTx(ci, false, &packet, sizeof(packet)))
    {
        printf("Fail to TX checkin\r\n");
        return false;
    }

    now = timerGet();
    while (timerGet() - now < TIMER_TICKS_PER_MSEC * COMMS_MAX_RADIO_WAIT_MSEC)
    {

        int32_t ret;

        WDT_RestartCounter();
        ret = commsRx(ci, rx, fromMac);

        if (ret == COMMS_RX_ERR_NO_PACKETS)
            continue;

        printf("RX pkt: 0x%02x + %d\r\n", rx[0], ret);

        if (ret == COMMS_RX_ERR_MIC_FAIL)
        {

            printf("RX: invalid MIC\r\n");
            return false;
        }

        if (ret < sizeof(uint8_t) + sizeof(struct PendingInfo))
        {

            printf("RX: %d < %d\r\n", ret, sizeof(uint8_t) + sizeof(struct PendingInfo));
            return false;
        }

        if (rx[0] != PKT_CHECKOUT)
        {
            printf("RX: pkt 0x%02x @ %s\r\n", rx[0], "checkin");
            return false;
        }

        *out = *(struct PendingInfo *)(rx + 1);
        return true;
    }

    return false;
}

static const char *fwVerString(void)
{
    static char fwVer[64] = {};
    sprintf(fwVer, "FW v%u.%u.%u.%u Batt: %imV Temp: %iC",
            (uint8_t)(mCurVersionExport >> 40),
            (uint8_t)(mCurVersionExport >> 32),
            (uint16_t)(mCurVersionExport >> 16),
            (uint16_t)(mCurVersionExport),
            measureBattery(),
            measureTemp());

    return fwVer;
}

uint8_t mScreenRow[DISPLAY_WIDTH / 2];
void uiPrvFullscreenMsg(const char *str, const char *line2, const char *line3)
{
    struct CharDrawingParams cdp;
    uint16_t i, r, textRow, textRowEnd;
    static const char zero = 0;
    uint8_t rowIdx;
    const char *strA[] = {str ? str : "", line2 ? line2 : "", line3 ? line3 : ""};

    printf("MESSAGE: '%s', '%s', '%s'\r\n", strA[0], strA[1], strA[2]);

    if (NO_GUI == 1)
        return; // Make everything faster for debugging.!!!!
    init_epd();
    display_send_start(0);

    rowIdx = 0;

    cdp.magnify = MAGNIFY1;
    cdp.str = strA[0];
    cdp.x = 1;

    cdp.foreColor = FORE_COLOR_1;
    cdp.backColor = BACK_COLOR;
    textRow = 10;

    textRowEnd = textRow + CHAR_HEIGHT * cdp.magnify;
    for (r = 0; r < DISPLAY_HEIGHT; r++)
    {

        // clear the row
        for (i = 0; i < sizeof(mScreenRow); i++)
            mScreenRow[i] = 0x33;

        if (r >= textRowEnd)
        {

            switch (rowIdx)
            {
            case 0:
                rowIdx = 1;

                textRow = textRowEnd + 20;
                cdp.magnify = MAGNIFY2;
                cdp.foreColor = FORE_COLOR_2;
                cdp.str = strA[1];
                cdp.x = 1;
                textRowEnd = textRow + CHAR_HEIGHT * cdp.magnify;
                break;

            case 1:
                rowIdx = 2;

                textRow = DISPLAY_HEIGHT - CHAR_HEIGHT;
                cdp.magnify = MAGNIFY3;
                cdp.foreColor = FORE_COLOR_3;
                cdp.str = strA[2];
                cdp.x = 1;
                textRowEnd = textRow + CHAR_HEIGHT;
                break;

            case 2:
                cdp.str = (const char *)&zero;
                break;
            }
        }
        else if (r > textRow)
        {
            cdp.imgRow = r - textRow;
            charsDrawString(&cdp);
        }

        for (i = 0; i < sizeof(mScreenRow); i++)
            display_tx_byte(mScreenRow[i]);
    }
    WDT_RestartCounter();

    display_send_stop();
    epd_refresh_and_sleep();
}

uint8_t offline_state = 0;
void set_offline(uint8_t state)
{
    offline_state = state;
}

static void uiPrvDrawBitmap(uint32_t address, uint32_t size)
{
    uint32_t w, h, s, r, c, i, bpp, effectiveH, effectiveW, clutSz;
    struct BitmapFileHeader bmp;
    uint8_t clutOurs[64], rowBuf[1000];
    struct BitmapClutEntry clut[64];
    uint32_t curr_address;

    if (size < sizeof(struct BitmapFileHeader))
        return;

    FLASH_Read(0, address, (uint8_t *)&bmp, sizeof(struct BitmapFileHeader));

    if (bmp.sig[0] == 0x1F || bmp.sig[1] == 0x8b)
    {
        char error_reason[100];
        memset(error_reason, 0x00, sizeof(error_reason));
        printf("Decomp img\r\n");
        size = decompress_file(address, size, EEPROM_UPDATE_START + sizeof(struct EepromImageHeaderOld), EEPROM_UPDATE_LEN - sizeof(struct EepromImageHeaderOld), error_reason); // Use the OTA so save file temporarly
        if (size == 0)
        {
            printf("failed\r\n");
            uiPrvFullscreenMsg("Decomp failed", error_reason, fwVerString());
            return;
        }
        FLASH_Read(0, address, (uint8_t *)&bmp, sizeof(struct BitmapFileHeader));
        printf("Decomp draw\r\n");
    }

    if (bmp.sig[0] != 'B' || bmp.sig[1] != 'M')
    {
        printf("Sig wrong!!!\r\n");
        uiPrvFullscreenMsg("Sig wrong", NULL, fwVerString());
        return;
    }

    if (bmp.headerSz < 40 || bmp.colorplanes != 1 || bmp.compression || !bmp.height || bmp.width <= 0 || bmp.bpp > 8)
    {
        printf("Header wrong\r\n");
        uiPrvFullscreenMsg("Header wrong", NULL, fwVerString());
        return;
    }

    if (bmp.height >= 0)
    {
        h = bmp.height;
    }
    else
    {
        h = -bmp.height;
    }
    w = bmp.width;
    bpp = bmp.bpp;
    s = (w * bpp + 31) / 32 * 4;
    clutSz = bmp.numColors;
    if (!clutSz)
        clutSz = 1 << bpp;

    // clut does not always follow header(ask GIMP why), but it does precede data
    // clut = (struct BitmapClutEntry *)(((char *)data) + bmp.dataOfst - sizeof(struct BitmapClutEntry) * clutSz);
    FLASH_Read(0, address + bmp.dataOfst - (sizeof(struct BitmapClutEntry) * clutSz), (uint8_t *)&clut, sizeof(struct BitmapClutEntry) * clutSz);

    // convert clut to our understanding of color
    for (i = 0; i < clutSz; i++)
    {
        uint32_t intensity = 0;

        intensity += 13988 * clut[i].r;
        intensity += 47055 * clut[i].g;
        intensity += 4750 * clut[i].b;
        // our colors are opposite of brightness, so we need to invert this too
        intensity ^= 0x00ffffff;

        clutOurs[i] = intensity >> (24 - 3);
    }
    curr_address = address + size - s;

    effectiveH = (h > DISPLAY_HEIGHT) ? DISPLAY_HEIGHT : h;
    effectiveW = (w > DISPLAY_WIDTH) ? DISPLAY_WIDTH : w;

    init_epd();
    display_send_start(0);
    uint8_t prevVal = 0;
    for (r = 0; r < effectiveH; r++)
    {
        // get a row
        if (offline_state && r == 0)
            memset(&rowBuf, 0xAA, s);
        else if (offline_state && r == 1)
            memset(&rowBuf, 0xff, s);
        else if (offline_state && r == 2)
            memset(&rowBuf, 0xAA, s);
        else
        {
            FLASH_Read(0, curr_address, rowBuf, s);
        }
        //  convert to our format
        for (c = effectiveW; c > 0; c--)
        {
            uint32_t val, inByteIdx = (c - 1) * bpp / 8, inSubbyteIdx = (c - 1) * bpp % 8, inBitMask = (1 << bpp) - 1, inBitIdx = 8 - bpp - inSubbyteIdx;
            // get value
            val = (rowBuf[inByteIdx] >> inBitIdx) & inBitMask;
            // look up in our clut
            val = clutOurs[val];

            switch (val)
            {
            case 0:
                val = 3;
                break;
            case 6:
                val = 4;
                break;
            case 7:
                val = 0;
                break;
            }

            if ((uint8_t)(c - 1) & 1)
            {
                prevVal = val;
            }
            else
            {
                display_tx_byte((prevVal << 4) | val);
            }
        }
        WDT_RestartCounter();
        curr_address -= s;
    }
    display_send_stop();
    epd_refresh_and_sleep();
}

static void uiPrvDrawImageAtAddress(uint32_t addr, uint32_t size)
{
    printf("Drawing image now\r\n");
    if (size < 6) // we need enough size to even sort out what this is, that needs 6 bytes
        return;

    uiPrvDrawBitmap(addr + sizeof(struct EepromImageHeaderOld), size);
}

void uiPrvDrawLatestImage(const struct EepromContentsInfo *eci)
{
    if (eci->latestCompleteImgAddr)
        uiPrvDrawImageAtAddress(eci->latestCompleteImgAddr, eci->latestCompleteImgSize);
}

static uint32_t prvDriveDownload(struct Settings *settings, struct CommsInfo *ci, struct EepromImageHeaderOld *eih, uint32_t addr, bool isOS)
{
    struct
    {
        uint8_t pktTyp;
        struct ChunkReqInfo cri;
    } packet = {
        .pktTyp = PKT_CHUNK_REQ,
        .cri = {
            .osUpdatePlz = isOS,
        },
    };
    uint8_t rx[COMMS_MAX_PACKET_SZ];
    const uint32_t nPieces = (eih->size + EEPROM_PIECE_SZ - 1) / EEPROM_PIECE_SZ;
    struct ChunkInfo *chunk = (struct ChunkInfo *)(rx + 1);
    uint8_t *data = (uint8_t *)(chunk + 1);
    bool progressMade = false;
    uint32_t curPiece;

    // sanity check
    if (nPieces > sizeof(eih->piecesMissing) * 8)
    {
        printf("DL too large: %u\r\n", eih->size);
        return settings->checkinDelay;
    }

    // prepare the packet
    packet.cri.versionRequested = eih->version;

    // find where we are in downloading
    for (curPiece = 0; curPiece < nPieces && !((eih->piecesMissing[curPiece / 8] >> (curPiece % 8)) & 1); curPiece++)
        ;

    printf("Requesting piece %u/%u of %s\r\n", curPiece, nPieces, isOS ? "UPDATE" : "IMAGE");

    // download
    for (; curPiece < nPieces; curPiece++)
    {

        uint_fast8_t now, nRetries;
        uint64_t nowStart;
        int32_t ret;

        // any piece that is not last will be of standard size
        if (curPiece != nPieces - 1)
            now = EEPROM_PIECE_SZ;
        else
            now = eih->size - (nPieces - 1) * EEPROM_PIECE_SZ;

        packet.cri.offset = curPiece * EEPROM_PIECE_SZ;
        packet.cri.len = now;

        for (nRetries = 0; nRetries < 5; nRetries++)
        {

            commsTx(ci, false, &packet, sizeof(packet));

            nowStart = timerGet();
            while (1)
            {

                if (timerGet() - nowStart > TIMER_TICKS_PER_MSEC * COMMS_MAX_RADIO_WAIT_MSEC)
                {
                    printf("RX timeout in download Piece: %i\r\n", curPiece);
                    break;
                }

                WDT_RestartCounter();
                ret = commsRx(ci, rx, settings->masterMac);

                if (ret == COMMS_RX_ERR_NO_PACKETS)
                    continue; // let it time out
                else if (ret == COMMS_RX_ERR_INVALID_PACKET)
                    continue; // let it time out
                else if (ret == COMMS_RX_ERR_MIC_FAIL)
                {
                    printf("RX: invalid MIC\r\n");

                    // mic errors are unlikely unless someone is deliberately messing with us - check in later
                    goto checkin_again;
                }
                else if ((uint8_t)ret < (uint8_t)(sizeof(uint8_t) + sizeof(struct ChunkInfo)))
                {
                    printf("RX: %d < %d\r\n", ret, sizeof(uint8_t) + sizeof(struct AssocInfo));

                    // server glitch? check in later
                    return settings->checkinDelay;
                }
                else if (rx[0] != PKT_CHUNK_RESP)
                {
                    printf("RX: pkt 0x%02x @ %s\r\n", rx[0], "DL");

                    // weird packet? worth retrying soner
                    break;
                }

                // get payload len
                ret -= sizeof(uint8_t) + sizeof(struct ChunkInfo);

                if (chunk->osUpdatePlz != isOS)
                {
                    printf("RX: wrong data type @ DL: %d\r\n", chunk->osUpdatePlz);
                    continue; // could be an accidental RX of older packet - ignore
                }
                else if (chunk->offset != packet.cri.offset)
                {
                    printf("RX: wrong offset @ DL 0x%08lx != 0x%08lx\r\n", chunk->offset, packet.cri.offset);
                    continue; // could be an accidental RX of older packet - ignore
                }
                else if (!ret)
                {
                    printf("RX: DL no longer avail\r\n");

                    // just check in later
                    goto checkin_again;
                }
                else if (ret != packet.cri.len)
                {

                    printf("RX: Got %ub, reqd %u\r\n", ret, packet.cri.len);

                    // server glitch? check in later
                    goto checkin_again;
                }

                // write data
                FLASH_Write(false, addr + curPiece * EEPROM_PIECE_SZ + sizeof(struct EepromImageHeaderOld), data, ret);

                // write marker
                eih->piecesMissing[curPiece / 8] &= ~(1 << (curPiece % 8));
                FLASH_Write(false, addr + offsetof(struct EepromImageHeaderOld, piecesMissing[curPiece / 8]), &eih->piecesMissing[curPiece / 8], 1);

                progressMade = true;
                nRetries = 100; // so we break the loop
                break;
            }
        }
        if (nRetries == 5)
        {
            printf("retried too much\r\n");
            if (progressMade)
                goto retry_later;
            else
                goto checkin_again;
        }
    }

    // downloadDone:
    printf("Done at piece %u/%u\r\n", curPiece, nPieces);
    radioShutdown();

    // if we are here, we succeeded in finishing the download
    eih->validMarker = EEPROM_IMG_VALID;
    FLASH_Write(false, addr + offsetof(struct EepromImageHeaderOld, validMarker), (uint8_t *)&eih->validMarker, sizeof(eih->validMarker));

    printf("DL completed\r\n");
    settings->prevDlProgress = 0xffff;
    // act on it
    if (!isOS)
        uiPrvDrawImageAtAddress(addr, eih->size);
    else if (prvApplyUpdateIfNeeded(settings))
    {
        printf("reboot post-update\r\n");
        return 100;
    }
    // fallthrough

checkin_again:
    return settings->checkinDelay;

retry_later:
    return settings->retryDelay;
}

void prvWriteNewHeader(struct EepromImageHeaderOld *eih, uint32_t addr, uint32_t eeSize, uint64_t ver, uint32_t size)
{
    qspiEraseRange(addr, eeSize);
    bzero(eih, sizeof(struct EepromImageHeaderOld));
    eih->version = ver;
    eih->validMarker = EEPROM_IMG_INPROGRESS;
    eih->size = size;
    memset(eih->piecesMissing, 0xff, sizeof(eih->piecesMissing));
    FLASH_Write(false, addr, (uint8_t *)eih, sizeof(struct EepromImageHeaderOld));
}

static uint32_t prvDriveUpdateDownload(struct Settings *settings, struct CommsInfo *ci, uint64_t ver, uint32_t size)
{
    struct EepromImageHeaderOld eih;
    uint32_t ret;

    // see what's there already
    FLASH_Read(0, EEPROM_UPDATE_START, (uint8_t *)&eih, sizeof(struct EepromImageHeaderOld));
    if (eih.version != ver)
        prvWriteNewHeader(&eih, EEPROM_UPDATE_START, EEPROM_UPDATE_LEN, ver, size);

    ret = prvDriveDownload(settings, ci, &eih, EEPROM_UPDATE_START, true);

    return ret;
}

static uint32_t prvDriveImageDownload(struct Settings *settings, struct CommsInfo *ci, const struct EepromContentsInfo *eci, uint64_t ver, uint32_t size)
{
    struct EepromImageHeaderOld eih;
    uint32_t addr, ret;

    // sort out where next image should live
    if (eci->latestInprogressImgAddr)
        addr = eci->latestInprogressImgAddr;
    else if (!eci->latestCompleteImgAddr)
        addr = EEPROM_IMG_START;
    else
    {
        addr = eci->latestCompleteImgAddr + EEPROM_IMG_EACH;
        if (addr >= EEPROM_IMG_START + EEPROM_IMG_LEN)
            addr = EEPROM_IMG_START;
    }

    // see what's there already
    FLASH_Read(0, addr, (uint8_t *)&eih, sizeof(struct EepromImageHeaderOld));
    if (eih.version != ver)
        prvWriteNewHeader(&eih, addr, EEPROM_IMG_EACH, ver, size);

    ret = prvDriveDownload(settings, ci, &eih, addr, false);

    return ret;
}

static void radioInitialize(uint8_t channel)
{
    RF_init(channel);
    ZIGBEE_set_filter(PROTO_PAN_ID, 0xffff, (uint32_t *)mSelfMac, 1);
}

static uint32_t uiPaired(struct Settings *settings, struct CommsInfo *ci)
{
    struct EepromContentsInfo eci;
    uint32_t i;
    struct PendingInfo pi;

    // do this before we turn on the radio, for power reasons

    memset(&eci, 0x00, sizeof(eci));
    prvEepromIndex(&eci);

    radioInitialize(SETTING_CHANNEL_OFFSET + settings->channel);

    // try five times
    for (i = 0; i < 5 && !prvSendCheckin(settings, ci, &pi); i++)
        ;

    if (i == 5)
    { // fail
        radioShutdown();
        printf("checkin fails\r\n");
        settings->numFailedCheckins++;

        if (settings->failedCheckinsTillDissoc && settings->numFailedCheckins == settings->failedCheckinsTillDissoc)
        {
            printf("Disassoc as %u = %u\r\n", settings->numFailedCheckins, settings->failedCheckinsTillDissoc);
            settings->isPaired = 0;
            settings->helperInit = 0;

            return 1000; // wake up in a second to try to pair
        }

        if (settings->failedCheckinsTillBlank && settings->numFailedCheckins == settings->failedCheckinsTillBlank)
        {
            printf("Blank as %u = %u\r\n", settings->numFailedCheckins, settings->failedCheckinsTillBlank);
            uiPrvFullscreenMsg("NO SIGNAL FOR TOO LONG", macStr, fwVerString());
        }

        // try again in due time
        return settings->checkinDelay;
    }

    // if we got here, we succeeded with the check-in. if screen was blanked, redraw it
    if (settings->failedCheckinsTillBlank && settings->numFailedCheckins >= settings->failedCheckinsTillBlank)
        uiPrvDrawLatestImage(&eci);
    settings->numFailedCheckins = 0;

    printf("Base: %s ver 0x%08x%08x, us 0x%08x%08x\r\n", " OS",
           (uint32_t)(pi.osUpdateVer >> 32), (uint32_t)pi.osUpdateVer,
           (uint32_t)(mCurVersionExport >> 32), (uint32_t)mCurVersionExport);

    printf("Base: %s ver 0x%08x%08x, us 0x%08x%08x\r\n", "IMG",
           (uint32_t)(pi.imgUpdateVer >> 32), (uint32_t)pi.imgUpdateVer,
           (uint32_t)(eci.latestCompleteImgVer >> 32), (uint32_t)eci.latestCompleteImgVer);

    // if there is an update, we want it
    if ((pi.osUpdateVer & VERSION_SIGNIFICANT_MASK) > (mCurVersionExport & VERSION_SIGNIFICANT_MASK))
        return prvDriveUpdateDownload(settings, ci, pi.osUpdateVer, pi.osUpdateSize);

    if (pi.imgUpdateVer > eci.latestCompleteImgVer)
        return prvDriveImageDownload(settings, ci, &eci, pi.imgUpdateVer, pi.imgUpdateSize);

    // nothing? guess we'll check again later
    return settings->checkinDelay;
}

static uint32_t uiNotPaired(struct Settings *settings, struct CommsInfo *ci)
{
    struct
    {
        uint8_t pktType;
        struct TagInfo ti;
    } __attribute__((packed)) packet = {};
    uint8_t rx[COMMS_MAX_PACKET_SZ];
    uint64_t waitEnd, nowStart;
    uint_fast8_t ch;
    int32_t ret;
    struct EepromContentsInfo eci;

    packet.pktType = PKT_ASSOC_REQ;
    packet.ti.protoVer = PROTO_VER_CURRENT;
    prvFillTagState(settings, &packet.ti.state);
    packet.ti.screenPixWidth = DISPLAY_WIDTH;
    packet.ti.screenPixHeight = DISPLAY_HEIGHT;
    packet.ti.screenMmWidth = DISPLAY_WIDTH_MM;
    packet.ti.screenMmHeight = DISPLAY_HEIGHT_MM;
    packet.ti.compressionsSupported = 0;
    packet.ti.maxWaitMsec = COMMS_MAX_RADIO_WAIT_MSEC;
    packet.ti.screenType = TagScreenEink_BW_1bpp;

    memset(&eci, 0x00, sizeof(eci));
    prvEepromIndex(&eci);
    if (settings->helperInit != 1)
    {
        settings->helperInit = 1;
        if (eci.latestCompleteImgAddr)
        {
            set_offline(1);
            uiPrvDrawLatestImage(&eci);
            set_offline(0);
        }
        else
        {
            sprintf(macStr, "(" MACFMT ")", MACCVT(mSelfMac));
            uiPrvFullscreenMsg("READY TO ASSOCIATE", macStr, fwVerString());
        }
    }
    else
    {
        printf("Assoc not displayed\n");
    }
    for (ch = 11; ch <= 11; ch++)
    {

        printf("try ch %u\r\n", ch);
        radioInitialize(ch);

        waitEnd = timerGet() + TIMER_TICKS_PER_SEC / 5; // try for 1/5 second per channel
        while (timerGet() < waitEnd)
        {

            commsTx(ci, true, &packet, sizeof(packet));
            nowStart = timerGet();
            printf("try ch %u\r\n", ch);
            while (timerGet() - nowStart < TIMER_TICKS_PER_MSEC * 150 /* wait 150 ms before retransmitting */)
            {

                WDT_RestartCounter();
                ret = commsRx(ci, rx, settings->masterMac);

                if (ret != COMMS_RX_ERR_NO_PACKETS)
                    printf("RX pkt: 0x%02x + %d\r\n", rx[0], ret);

                if (ret == COMMS_RX_ERR_MIC_FAIL)
                    printf("RX: invalid MIC\r\n");
                else if (ret <= 0)
                    ; // nothing
                else if (ret < sizeof(uint8_t) + sizeof(struct AssocInfo))
                    printf("RX: %d < %d\r\n", ret, sizeof(uint8_t) + sizeof(struct AssocInfo));
                else if (rx[0] != PKT_ASSOC_RESP)
                    printf("RX: pkt 0x%02x @ %s\r\n", rx[0], "pair");
                else
                {
                    struct AssocInfo *ai = (struct AssocInfo *)(rx + 1);

                    settings->checkinDelay = ai->checkinDelay;
                    settings->retryDelay = ai->retryDelay;
                    settings->failedCheckinsTillBlank = ai->failedCheckinsTillBlank;
                    settings->failedCheckinsTillDissoc = ai->failedCheckinsTillDissoc;
                    settings->channel = ch - SETTING_CHANNEL_OFFSET;
                    settings->numFailedCheckins = 0;
                    settings->nextIV = 0;
                    memcpy(settings->encrKey, ai->newKey, sizeof(settings->encrKey));
                    settings->isPaired = 1;
                    settings->helperInit = 0;

                    radioShutdown();
                    printf("Associated to master " MACFMT "\r\n", MACCVT(settings->masterMac));

                    settings->prevDlProgress = 0xffff;

                    WDT_RestartCounter();

                    // printf("Erz IMG\r\n");
                    // qspiEraseRange(EEPROM_IMG_START, EEPROM_IMG_LEN);

                    printf("Erz UPD\r\n");
                    qspiEraseRange(EEPROM_UPDATE_START, EEPROM_UPDATE_LEN);

                    if (eci.latestCompleteImgAddr)
                    {
                        uiPrvDrawLatestImage(&eci);
                    }
                    else
                    {
                        sprintf(macStr, "(" MACFMT ")", MACCVT(mSelfMac));
                        uiPrvFullscreenMsg("\x01\x02", macStr, fwVerString()); // signal icon
                    }

                    return 1000; // wake up in a second to check in
                }
            }
        }
    }
    radioShutdown();
    settings->prevDlProgress = 0xffff;
    // uiPrvFullscreenMsg("\x03\x04", NULL, fwVerString()); // no signal icon

    return 1000 * 60 * 30;
}

static void prvGetSelfMac(void)
{
    FLASH_Read(0, EEPROM_MAC_INFO_START, mSelfMac, 8);

    if ((((uint32_t *)mSelfMac)[0] | ((uint32_t *)mSelfMac)[1]) == 0 || (((uint32_t *)mSelfMac)[0] & ((uint32_t *)mSelfMac)[1]) == 0xffffffff)
    { // fastest way to check for all ones or all zeroes

        printf("mac unknown\r\n");
        // Write a blank mac to have something to work with.
        memcpy(&mSelfMac, (uint8_t *)&default_mac, 8);
        FLASH_Write(0, EEPROM_MAC_INFO_START, mSelfMac, 8);
        // sleep_with_with_wakeup(0);
    }
}

static void showVersionAndVerifyMatch(void)
{
    // the &mCurVersionExport access is necessary to make sure mCurVersionExport is referenced
    /* printf("Booting FW ver 0x%08x%08x (at 0x%08x)\r\n",
            (unsigned)(mCurVersionExport >> 32), (unsigned)mCurVersionExport, &mCurVersionExport);
     if (((uint32_t)&mCurVersionExport) - 0x100000 != HW_TYPE_74_INCH_BWR_ROM_VER_OFST - 0x20)
     {
         printf("ver loc mismatch\r\n");
         sleep_with_with_wakeup(0);
     }
     if (mCurVersionExport & ~VERSION_SIGNIFICANT_MASK)
     {
         printf("ver num @ red zone\r\n");
         sleep_with_with_wakeup(0);
     }*/
}

uint8_t showChannelSelect()
{ // returns 0 if no accesspoints were found
    uint8_t result[sizeof(channelList)];
    memset(result, 0, sizeof(result));
    powerUp(INIT_RADIO);
    //uiPrvFullscreenMsg("Scanning", NULL, NULL);
    for (uint8_t i = 0; i < 4; i++)
    {
        for (uint8_t c = 0; c < sizeof(channelList); c++)
        {
            if (detectAP(channelList[c]))
            {
                if (mLastLqi > result[c])
                    result[c] = mLastLqi;
                printf("Channel: %d - LQI: %d RSSI %d\n", channelList[c], mLastLqi, mLastRSSI);
            }
        }
    }

    uint8_t highestLqi = 0;
    uint8_t highestSlot = 0;
    for (uint8_t c = 0; c < sizeof(result); c++)
    {
        if (result[c] > highestLqi)
        {
            highestSlot = channelList[c];
            highestLqi = result[c];
        }
    }
    mLastLqi = highestLqi;
    return highestSlot;
}

uint8_t channelSelect()
{ // returns 0 if no accesspoints were found
    uint8_t result[16];
    memset(result, 0, sizeof(result));

    for (uint8_t i = 0; i < 2; i++)
    {
        for (uint8_t c = 0; c < sizeof(channelList); c++)
        {
            if (detectAP(channelList[c]))
            {
                if (mLastLqi > result[c])
                    result[c] = mLastLqi;
            }
        }
    }

    uint8_t highestLqi = 0;
    uint8_t highestSlot = 0;
    for (uint8_t c = 0; c < sizeof(result); c++)
    {
        if (result[c] > highestLqi)
        {
            highestSlot = channelList[c];
            highestLqi = result[c];
        }
    }

    mLastLqi = highestLqi;
    return highestSlot;
}

void __attribute__((interrupt)) NMIException(void)
{
    printf("-----------> NMIException\r\n");
    PMU->CLK_SRC.BF.MAIN_CLK_SOURCE = 1;
    PMU->PWR_MODE.BF.PWR_MODE = 0;
    NVIC_SystemReset();
}

void __attribute__((interrupt)) HardFaultException(void)
{
    printf("-----------> HardFaultException\r\n");
    PMU->CLK_SRC.BF.MAIN_CLK_SOURCE = 1;
    PMU->PWR_MODE.BF.PWR_MODE = 0;
    NVIC_SystemReset();
}

void __attribute__((interrupt)) MemManageException(void)
{
    printf("-----------> MemManageException\r\n");
    PMU->CLK_SRC.BF.MAIN_CLK_SOURCE = 1;
    PMU->PWR_MODE.BF.PWR_MODE = 0;
    NVIC_SystemReset();
}

void __attribute__((interrupt)) BusFaultException(void)
{
    printf("-----------> BusFaultException\r\n");
    PMU->CLK_SRC.BF.MAIN_CLK_SOURCE = 1;
    PMU->PWR_MODE.BF.PWR_MODE = 0;
    NVIC_SystemReset();
}

void __attribute__((interrupt)) UsageFaultException(void)
{
    printf("-----------> UsageFaultException\r\n");
    PMU->CLK_SRC.BF.MAIN_CLK_SOURCE = 1;
    PMU->PWR_MODE.BF.PWR_MODE = 0;
    NVIC_SystemReset();
}

void __attribute__((interrupt)) SVCHandler(void)
{
}

void __attribute__((interrupt)) DebugMonitor(void)
{
}

void __attribute__((interrupt)) PendSVC(void)
{
}

int main(void)
{
    uint8_t currentChannel = 0;
    struct Settings settings;
    uint32_t sleepDuration = 5000;
    struct CommsInfo ci;
    uint32_t reset_reason = PMU_GetLastResetCause();
    uint8_t pwr_mode_at_boot = PMU->PWR_MODE.BF.PWR_MODE;

    (*(volatile unsigned int *)0x20124000) = 0x100004;   // On WARM RESET: Goto this address. -> entry
    (*(volatile unsigned int *)0xE000ED08) = 0x20100000; // Vector table in RAM and offset 0x4000
    (*(volatile unsigned int *)0xE000E41A) = 0x40;

    CLK_SystemClkInit(CLK_SYS_XTAL64M, CLK_SYS_64M);
    CLK_Xtal32MEnable(CLK_OSC_INTERN);
    while (CLK_GetClkStatus(CLK_OUT_XTAL64M) != 1)
        ;

    // UART 1 DEBUG OUT
    GPIO_PinModeConfig(UART_TX, PINMODE_DEFAULT);
    GPIO_PinModeConfig(UART_RX, PINMODE_DEFAULT);
    GPIO_PinMuxFun(UART_TX, GPIO4_UART2_TXD); // UART
    GPIO_PinMuxFun(UART_RX, GPIO6_UART2_RXD); // UART
    UART_CFG_Type uartcfg;
    uartcfg.baudRate = 115200;
    uartcfg.dataBits = UART_DATABITS_8;
    uartcfg.stopBits = 1;
    uartcfg.parity = UART_PARITY_NONE;
    uartcfg.autoFlowControl = DISABLE;
    UART_Init(1, &uartcfg);

    UART_FIFO_Type uartFifo;
    uartFifo.FIFO_ResetRx = 1;
    uartFifo.FIFO_ResetTx = 1;
    uartFifo.FIFO_Function = 1;
    uartFifo.FIFO_RcvrTrigger = 2;
    uartFifo.FIFO_TxEmptyTrigger = 3;
    UART_FIFOConfig(1, &uartFifo);
    // UART 1 DEBUG OUT

    if (!(~(*(volatile unsigned int *)0x4A080000) << 30))
    {
        NVIC_EnableIRQ(ExtPin5_IRQn);
        NVIC_EnableIRQ(RTC_IRQn);
    }

    (*(volatile unsigned int *)0x4A070004) = ((*(volatile unsigned int *)0x4A070004) & 0xFFFFFFE0) + 2;
    PMU->PWR_MODE.BF.PWR_MODE = 2;
    uint32_t v0 = FLASH_WordRead(FLASH_NORMAL_READ, 4u);
    char v1;
    if (!(~v0 << 25))
    {
        CLK_RC32MEnable();
        while (CLK_GetClkStatus(CLK_OUT_RC32M) != 1)
            ;
        v1 = CLK_RC32MCalibration(CLK_AUTO_CAL, 0);
        FLASH_WordWrite(FLASH_PROGRAM_NORMAL, 4u, (v0 & 0xFFFFFF00) | (v1 & 0x7F));
    }

    //** WATCHDOG
    CLK_ModuleClkEnable(CLK_WDT);
    WDT_SetMode(WDT_MODE_RESET);
    WDT_SetResetPulseLen(WDT_RESET_PULSE_LEN_256);
    WDT_SetTimeoutVal(30);
    WDT_RestartCounter();
    WDT_Enable();
    //** WATCHDOG

    //** GPIOS
    init_GPIO_boot();
    // NFC POWER Should be on if NFC is wanted to be used
    GPIO_PinOutputModeConfig(NFC_POWER, PIN_OUTPUT_MODE_NORMAL_FUNCTION);
    GPIO_PinModeConfig(NFC_POWER, PINMODE_DEFAULT);
    GPIO_PinMuxFun(NFC_POWER, 0);
    GPIO_SetPinDir(NFC_POWER, GPIO_OUTPUT);
    GPIO_WritePinOutput(NFC_POWER, 1); // Better power NFC up so IRQ will work unpowered later
    //** GPIOS
    //** RTC
    CLK_Xtal32MEnable(CLK_OSC_INTERN);
    while (!CLK_GetClkStatus(CLK_OUT_XTAL64M))
        ;
    RC32K_CalClk_Div(63, 31);
    CLK_ModuleClkEnable(CLK_RC32K_CAL);
    CLK_RC32KEnable();
    while (!CLK_GetClkStatus(CLK_OUT_RC32K))
        ;
    PMU->RC32K_CAL_CNTL.BF.RC32K_CAL_DIV = 0;
    CLK_RC32KCalibration(CLK_RC32KCAL_XTAL64M, CLK_AUTO_CAL, 0);
    CLK_ModuleClkEnable(CLK_RTC);
    CLK_RTCClkSrc(CLK_RTC_RC32K);
    RTC_Stop();
    RTC_Config_Type rtc_conf;
    rtc_conf.CntValUpdateMode = RTC_CNT_VAL_UPDATE_AUTO;
    rtc_conf.clockDivider = 0;
    rtc_conf.uppVal = 0xffffffff;
    RTC_Init(&rtc_conf);
    RTC_IntClr(RTC_INT_CNT_UPP);
    NVIC_ClearPendingIRQ(RTC_IRQn);
    RTC_IntMask(RTC_INT_CNT_UPP, UNMASK);
    NVIC_EnableIRQ(RTC_IRQn);
    //*** RTC

    //** Get the real wakeup reason
    printf("Rst reason: %i\r\n", reset_reason);
    uint32_t real_reason = 0;
    if ((*(volatile unsigned int *)0x130400) != 0x11223344)
    {
        (*(volatile unsigned int *)0x130400) = 0x11223344;
        currentChannel = 0;
        (*(volatile unsigned int *)0x130404) = currentChannel;

        nfc_i2c_init(); // This is only needed on a complete reboot

        if (reset_reason == 5)
        {
            if (pwr_mode_at_boot)
            {
                real_reason = 1; // POR Reset
            }
            else
            {
                real_reason = 3; // WDT Reset
            }
        }
        else
        {
            if (reset_reason != 4)
            {
                if (real_reason == 1 && !pwr_mode_at_boot)
                {
                    real_reason = 2; // System Reset
                }
                else
                {
                    real_reason = 1; // POR Reset
                }
            }
            else if (pwr_mode_at_boot != 1)
            {
                real_reason = 1; // POR Reset
            }
        }
    }
    else
    {
        currentChannel = (*(volatile unsigned int *)0x130404);
    }

    showVersionAndVerifyMatch();
    timerInit();
    WDT_RestartCounter();
    prvGetSelfMac();
    initializeProto();

    printf("Booot OpenEPaperLink\r\n");
    if (real_reason == 1)
    {
        printf("Erz data\r\n");
        qspiEraseRange(EEPROM_IMG_START, EEPROM_IMG_LEN);
        qspiEraseRange(EEPROM_UPDATE_START, EEPROM_UPDATE_LEN);
        qspiEraseRange(EEPROM_SETTINGS_AREA_START, EEPROM_SETTINGS_AREA_LEN);

        sprintf(macStr, "(" MACFMT ")", MACCVT(mSelfMac));
        //uiPrvFullscreenMsg("OpenEPaperLink", macStr, fwVerString());
        currentChannel = showChannelSelect();
        (*(volatile unsigned int *)0x130404) = currentChannel;

        WDT_RestartCounter();

        if (currentChannel)
        {
            printf("AP Found\r\n");
            sprintf(macStr1, "OpenEPaperLink Ch: %i", currentChannel);
            uiPrvFullscreenMsg("AP Found", macStr1, macStr);
            timerDelay(TIMER_TICKS_PER_MSEC * 1000);
        }
        else
        {
            printf("No AP found\r\n");
            uiPrvFullscreenMsg("No AP Found", "OpenEPaperLink", macStr);
            sleep_with_with_wakeup(120000UL);
        }
    }

    while (1 == 1)
    {
        wdt10s();
        if (currentChannel)
        {
            // associated

            struct AvailDataInfo *avail;
            // Is there any reason why we should do a long (full) get data request (including reason, status)?
            if ((longDataReqCounter > LONG_DATAREQ_INTERVAL) || wakeUpReason != WAKEUP_REASON_TIMED)
            {
                // check if we should do a voltage measurement (those are pretty expensive)
                /*if (voltageCheckCounter == VOLTAGE_CHECK_INTERVAL)
                {
                    powerUp(INIT_RADIO); // load down the battery using the radio to get a good reading
                    powerUp(INIT_TEMPREADING | INIT_EPD_VOLTREADING);
                    powerDown(INIT_RADIO);
                    voltageCheckCounter = 0;
                }
                else
                {
                    powerUp(INIT_TEMPREADING);
                }
                voltageCheckCounter++;*/

                // check if the battery level is below minimum, and force a redraw of the screen
                /*if ((lowBattery && !lowBatteryShown) || (noAPShown))
                {
                    // Check if we were already displaying an image
                    if (curImgSlot != 0xFF)
                    {
                        powerUp(INIT_EEPROM | INIT_EPD);
                        wdt60s();
                        drawImageFromEeprom(curImgSlot);
                        powerDown(INIT_EEPROM | INIT_EPD);
                    }
                    else
                    {
                        powerUp(INIT_EPD);
                        showAPFound();
                        powerDown(INIT_EPD);
                    }
                }*/
                powerUp(INIT_RADIO);
                avail = getAvailDataInfo();
                powerDown(INIT_RADIO);

                if (avail != NULL)
                {
                    // we got some data!
                    longDataReqCounter = 0;
                    // since we've had succesful contact, and communicated the wakeup reason succesfully, we can now reset to the 'normal' status
                    wakeUpReason = WAKEUP_REASON_TIMED;
                }
            }
            else
            {
                powerUp(INIT_RADIO);
                avail = getShortAvailDataInfo();
                powerDown(INIT_RADIO);
            }

            addAverageValue();

            if (avail == NULL)
            {
                // no data :(
                nextCheckInFromAP = 0; // let the power-saving algorithm determine the next sleep period
            }
            else
            {
                nextCheckInFromAP = avail->nextCheckIn;
                // got some data from the AP!
                if (avail->dataType != DATATYPE_NOUPDATE)
                {
                    // data transfer
                    if (processAvailDataInfo(avail))
                    {
                        // succesful transfer, next wake time is determined by the NextCheckin;
                    }
                    else
                    {
                        // failed transfer, let the algorithm determine next sleep interval (not the AP)
                        nextCheckInFromAP = 0;
                    }
                }
                else
                {
                    // no data transfer, just sleep.
                }
            }

            uint16_t nextCheckin = getNextSleep();
            longDataReqCounter += nextCheckin;
            if (nextCheckin == INTERVAL_AT_MAX_ATTEMPTS)
            {
                // disconnected, obviously...
                currentChannel = 0;
                (*(volatile unsigned int *)0x130404) = currentChannel;
            }

            // if the AP told us to sleep for a specific period, do so.
            if (nextCheckInFromAP)
            {
                sleep_with_with_wakeup(nextCheckInFromAP * 60000UL);
            }
            else
            {
                sleep_with_with_wakeup(getNextSleep() * 1000UL);
            }
        }
        else
        {
            // not associated
            if (((scanAttempts != 0) && (scanAttempts % VOLTAGEREADING_DURING_SCAN_INTERVAL == 0)) || (scanAttempts > (INTERVAL_1_ATTEMPTS + INTERVAL_2_ATTEMPTS)))
            {
                powerUp(INIT_RADIO); // load down the battery using the radio to get a good reading
                powerUp(INIT_EPD_VOLTREADING);
                powerDown(INIT_RADIO);
            }
            // try to find a working channel
            powerUp(INIT_RADIO);
            currentChannel = channelSelect();
            (*(volatile unsigned int *)0x130404) = currentChannel;
            powerDown(INIT_RADIO);

            /*if ((!currentChannel && !noAPShown) || (lowBattery && !lowBatteryShown) || (scanAttempts == (INTERVAL_1_ATTEMPTS + INTERVAL_2_ATTEMPTS - 1)))
            {
                powerUp(INIT_EPD);
                wdt60s();
                if (curImgSlot != 0xFF)
                {
                    powerUp(INIT_EEPROM);
                    drawImageFromEeprom(curImgSlot);
                    powerDown(INIT_EEPROM);
                }
                else if ((scanAttempts >= (INTERVAL_1_ATTEMPTS + INTERVAL_2_ATTEMPTS - 1)))
                {
                    showLongTermSleep();
                }
                else
                {
                    showNoAP();
                }
                powerDown(INIT_EPD);
            }*/

            // did we find a working channel?
            if (currentChannel)
            {
                // now associated!
                scanAttempts = 0;
                wakeUpReason = WAKEUP_REASON_NETWORK_SCAN;
                initPowerSaving(INTERVAL_BASE);
                sleep_with_with_wakeup(getNextSleep() * 1000UL);
            }
            else
            {
                // still not associated
                sleep_with_with_wakeup(getNextScanSleep(true) * 1000UL);
            }
        }
    }
    printf("sleep: %u\r\n", sleepDuration);
    if (sleepDuration == 0)
        sleepDuration = 60 * 1000 * 15; // never sleep forever!
    sleep_with_with_wakeup(sleepDuration);
    return 0;
}

int _write(int file, char *ptr, int len)
{
    UART_SendBytes(1, ptr, len);
    return len;
}