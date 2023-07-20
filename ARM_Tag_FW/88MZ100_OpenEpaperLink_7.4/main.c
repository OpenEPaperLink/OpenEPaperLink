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

void prvApplyUpdateIfNeeded()
{
    uint32_t ofst, now, size, pieceSz = 0x2000;
    uint8_t chunkStore[0x2000];
    (*(volatile unsigned int *)0x130000) = 0; // Invalidate RAM in any case so the next boot will be a full one
    (*(volatile unsigned int *)0x130400) = 0;

    printf("Applying update\r\n");
    qspiEraseRange(EEPROM_OS_START, EEPROM_OS_LEN);

    size = EEPROM_OS_LEN;
    for (ofst = 0; ofst < size; ofst += now)
    {
        now = size - ofst;
        if (now > pieceSz)
            now = pieceSz;
        printf("Cpy 0x%06x + 0x%04x to 0x%06x\r\n", EEPROM_UPDATE_START + ofst, now, EEPROM_OS_START + ofst);
        FLASH_Read(0, EEPROM_UPDATE_START + ofst, chunkStore, now);
        FLASH_Write(false, EEPROM_OS_START + ofst, chunkStore, now);
        WDT_RestartCounter();
    }

    printf("Erz IMAGES\r\n");
    qspiEraseRange(EEPROM_IMG_START, EEPROM_IMG_LEN);
    printf("Erz update\r\n");
    qspiEraseRange(EEPROM_UPDATE_START, EEPROM_UPDATE_LEN);
    sleep_with_with_wakeup(1000);
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
    display_send_start(false);

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

void prvEepromIndex(struct EepromContentsInfo *eci)
{
    struct EepromImageHeader eih;
    uint32_t addr;

    for (addr = EEPROM_IMG_START; addr - EEPROM_IMG_START < EEPROM_IMG_LEN; addr += EEPROM_IMG_EACH)
    {

        uint32_t *addrP, *szP = NULL;
        uint64_t *verP = NULL;

        FLASH_Read(0, addr, (uint8_t *)&eih, sizeof(struct EepromImageHeader));
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

static void uiPrvDrawImageAtAddress(uint32_t addr, uint32_t size)
{
    printf("Drawing image now\r\n");
    if (size < 6) // we need enough size to even sort out what this is, that needs 6 bytes
        return;

    // uiPrvDrawBitmap(addr + sizeof(struct EepromImageHeader), size);
}
void uiPrvDrawLatestImage(const struct EepromContentsInfo *eci)
{
    if (eci->latestCompleteImgAddr)
        uiPrvDrawImageAtAddress(eci->latestCompleteImgAddr, eci->latestCompleteImgSize);
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
    // uiPrvFullscreenMsg("Scanning", NULL, NULL);
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

extern struct blockRequest curBlock;     // used by the block-requester, contains the next request that we'll send
extern struct AvailDataInfo curDataInfo; // last 'AvailDataInfo' we received from the AP
extern bool requestPartialBlock;         // if we should ask the AP to get this block from the host or not
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
        memcpy((uint8_t *)&curBlock, (uint8_t *)&(*(volatile unsigned int *)0x130500), sizeof(struct blockRequest));
        memcpy((uint8_t *)&curDataInfo, (uint8_t *)&(*(volatile unsigned int *)0x130600), sizeof(struct AvailDataInfo));
        memset(curBlock.requestedParts, 0x00, BLOCK_REQ_PARTS_BYTES);
        requestPartialBlock = false;
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
        // uiPrvFullscreenMsg("HELLO OTA", macStr, fwVerString());
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

    if (nfc_handle()) // If an image was uploaded via NFC lets display it.
    {
        struct EepromContentsInfo eci;
        memset(&eci, 0x00, sizeof(eci));
        prvEepromIndex(&eci);
        uiPrvDrawLatestImage(&eci);
        sleep_with_with_wakeup(30 * 1000);
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
                sleep_with_with_wakeup(60 * 1000UL);
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
                //sleep_with_with_wakeup(getNextScanSleep(true) * 1000UL);
                sleep_with_with_wakeup(10 * 60 * 1000UL);
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