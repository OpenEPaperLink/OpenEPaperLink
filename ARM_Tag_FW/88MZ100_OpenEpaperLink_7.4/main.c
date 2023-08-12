#include "main.h"

#include <stdbool.h>
#include <stdint.h>
// #include <stdio.h>
#include <string.h>

#include "ccm.h"
#include "chars.h"
#include "comms.h"
#include "core_cm3.h"
#include "eeprom.h"
#include "epd.h"
#include "gpio.h"
#include "mz100.h"
#include "mz100_aon_ram.h"
#include "mz100_clock.h"
#include "mz100_flash.h"
#include "mz100_gpio.h"
#include "mz100_pinmux.h"
#include "mz100_pmu.h"
#include "mz100_sleep.h"
#include "mz100_ssp.h"
#include "mz100_uart.h"
#include "powermgt.h"
#include "printf.h"
#include "proto.h"
#include "settings.h"
#include "syncedproto.h"
#include "timer.h"
#include "userinterface.h"
#include "util.h"
#include "zigbee.h"

#define SW_VER_CURRENT (0x0000011300000000ull)  // top 16 bits are off limits, xxxx.VV.tt.vvvv.mmmm means version V.t.v.m
#define SW_DEFAULT_MAC (0x0000000000000014ull)

uint64_t __attribute__((section(".ver"))) mCurVersionExport = SW_VER_CURRENT;
uint64_t __attribute__((section(".default_mac"))) default_mac = SW_DEFAULT_MAC;

char macStr[32];
char macStr1[32];
// uint8_t mSelfMac[8];

#define TAG_MODE_CHANSEARCH 0
#define TAG_MODE_ASSOCIATED 1

__attribute__((section(".aon"))) uint8_t currentTagMode = TAG_MODE_CHANSEARCH;
__attribute__((section(".aon"))) volatile struct zigbeeCalibDataStruct zigbeeCalibData;

void prvApplyUpdateIfNeeded() {
    uint32_t ofst, now, size, pieceSz = 0x2000;
    uint8_t chunkStore[0x2000];
    (*(volatile unsigned int *)0x130000) = 0;  // Invalidate RAM in any case so the next boot will be a full one
    (*(volatile unsigned int *)0x130400) = 0;

    printf("Applying update\r\n");
    qspiEraseRange(EEPROM_OS_START, EEPROM_OS_LEN);

    size = EEPROM_OS_LEN;
    for (ofst = 0; ofst < size; ofst += now) {
        now = size - ofst;
        if (now > pieceSz)
            now = pieceSz;
        printf("Cpy 0x%06x + 0x%04x to 0x%06x\r\n", EEPROM_UPDATE_START + ofst, now, EEPROM_OS_START + ofst);
        FLASH_Read(0, EEPROM_UPDATE_START + ofst, chunkStore, now);
        FLASH_Write(false, EEPROM_OS_START + ofst, chunkStore, now);
        WDT_RestartCounter();
    }

    // printf("Erz IMAGES\r\n");
    // qspiEraseRange(EEPROM_IMG_START, EEPROM_IMG_LEN);
    // printf("Erz update\r\n");
    // qspiEraseRange(EEPROM_UPDATE_START, EEPROM_UPDATE_LEN);
    sleep_with_with_wakeup(1000);
}

void prvEepromIndex(struct EepromContentsInfo *eci) {
    struct EepromImageHeader eih;
    uint32_t addr;

    for (addr = EEPROM_IMG_START; addr - EEPROM_IMG_START < EEPROM_IMG_LEN; addr += EEPROM_IMG_EACH) {
        uint32_t *addrP, *szP = NULL;
        uint64_t *verP = NULL;

        FLASH_Read(0, addr, (uint8_t *)&eih, sizeof(struct EepromImageHeader));
        printf("DATA slot 0x%06x: type 0x%08x ver 0x%08x%08x\r\n", addr, eih.validMarker, (uint32_t)(eih.version >> 32), (uint32_t)eih.version);

        switch (eih.validMarker) {
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

        if (verP && eih.version >= *verP) {
            *verP = eih.version;
            *addrP = addr;
            if (szP)
                *szP = eih.size;
        }
    }
}
void prvWriteNewHeader(struct EepromImageHeaderOld *eih, uint32_t addr, uint32_t eeSize, uint64_t ver, uint32_t size) {
    qspiEraseRange(addr, eeSize);
    // bzero(eih, sizeof(struct EepromImageHeaderOld));
    eih->version = ver;
    eih->validMarker = EEPROM_IMG_INPROGRESS;
    eih->size = size;
    memset(eih->piecesMissing, 0xff, sizeof(eih->piecesMissing));
    FLASH_Write(false, addr, (uint8_t *)eih, sizeof(struct EepromImageHeaderOld));
}

static void prvGetSelfMac(void) {
    FLASH_Read(0, EEPROM_MAC_INFO_START, mSelfMac, 8);

    if ((((uint32_t *)mSelfMac)[0] | ((uint32_t *)mSelfMac)[1]) == 0 || (((uint32_t *)mSelfMac)[0] & ((uint32_t *)mSelfMac)[1]) == 0xffffffff) {  // fastest way to check for all ones or all zeroes

        printf("mac unknown\r\n");
        // Write a blank mac to have something to work with.
        memcpy(&mSelfMac, (uint8_t *)&default_mac, 8);
        FLASH_Write(0, EEPROM_MAC_INFO_START, mSelfMac, 8);
        // sleep_with_with_wakeup(0);
    }
}

uint8_t showChannelSelect() {  // returns 0 if no accesspoints were found
    uint8_t result[sizeof(channelList)];
    memset(result, 0, sizeof(result));
    powerUp(INIT_RADIO);
    // uiPrvFullscreenMsg("Scanning", NULL, NULL);
    for (uint8_t i = 0; i < 4; i++) {
        for (uint8_t c = 0; c < sizeof(channelList); c++) {
            if (detectAP(channelList[c])) {
                if (mLastLqi > result[c])
                    result[c] = mLastLqi;
                printf("Channel: %d - LQI: %d RSSI %d\n", channelList[c], mLastLqi, mLastRSSI);
            }
        }
    }

    uint8_t highestLqi = 0;
    uint8_t highestSlot = 0;
    for (uint8_t c = 0; c < sizeof(result); c++) {
        if (result[c] > highestLqi) {
            highestSlot = channelList[c];
            highestLqi = result[c];
        }
    }
    mLastLqi = highestLqi;
    return highestSlot;
}

uint8_t channelSelect() {  // returns 0 if no accesspoints were found
    uint8_t result[16];
    memset(result, 0, sizeof(result));

    for (uint8_t i = 0; i < 2; i++) {
        for (uint8_t c = 0; c < sizeof(channelList); c++) {
            if (detectAP(channelList[c])) {
                if (mLastLqi > result[c])
                    result[c] = mLastLqi;
            }
        }
    }

    uint8_t highestLqi = 0;
    uint8_t highestSlot = 0;
    for (uint8_t c = 0; c < sizeof(result); c++) {
        if (result[c] > highestLqi) {
            highestSlot = channelList[c];
            highestLqi = result[c];
        }
    }

    mLastLqi = highestLqi;
    return highestSlot;
}

void __attribute__((interrupt)) NMIException(void) {
    printf("-----------> NMIException\r\n");
    PMU->CLK_SRC.BF.MAIN_CLK_SOURCE = 1;
    PMU->PWR_MODE.BF.PWR_MODE = 0;
    NVIC_SystemReset();
}

void __attribute__((interrupt)) HardFaultException(void) {
    printf("-----------> HardFaultException\r\n");
    PMU->CLK_SRC.BF.MAIN_CLK_SOURCE = 1;
    PMU->PWR_MODE.BF.PWR_MODE = 0;
    NVIC_SystemReset();
}

void __attribute__((interrupt)) MemManageException(void) {
    printf("-----------> MemManageException\r\n");
    PMU->CLK_SRC.BF.MAIN_CLK_SOURCE = 1;
    PMU->PWR_MODE.BF.PWR_MODE = 0;
    NVIC_SystemReset();
}

void __attribute__((interrupt)) BusFaultException(void) {
    printf("-----------> BusFaultException\r\n");
    PMU->CLK_SRC.BF.MAIN_CLK_SOURCE = 1;
    PMU->PWR_MODE.BF.PWR_MODE = 0;
    NVIC_SystemReset();
}

void __attribute__((interrupt)) UsageFaultException(void) {
    printf("-----------> UsageFaultException\r\n");
    PMU->CLK_SRC.BF.MAIN_CLK_SOURCE = 1;
    PMU->PWR_MODE.BF.PWR_MODE = 0;
    NVIC_SystemReset();
}

void __attribute__((interrupt)) SVCHandler(void) {
}

void __attribute__((interrupt)) DebugMonitor(void) {
}

void __attribute__((interrupt)) PendSVC(void) {
}

void setupRTC() {
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
}

void setupUART() {
    // UART 1 DEBUG OUT
    GPIO_PinModeConfig(UART_TX, PINMODE_DEFAULT);
    GPIO_PinModeConfig(UART_RX, PINMODE_DEFAULT);
    GPIO_PinMuxFun(UART_TX, GPIO4_UART2_TXD);  // UART
    GPIO_PinMuxFun(UART_RX, GPIO6_UART2_RXD);  // UART
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
}

void setupWDT() {
    //** WATCHDOG
    CLK_ModuleClkEnable(CLK_WDT);
    WDT_SetMode(WDT_MODE_RESET);
    WDT_SetResetPulseLen(WDT_RESET_PULSE_LEN_256);
    WDT_SetTimeoutVal(30);
    WDT_RestartCounter();
    WDT_Enable();
    //** WATCHDOG
}

void setupGPIO() {
    //** GPIOS
    init_GPIO_boot();
    // NFC POWER Should be on if NFC is wanted to be used
    GPIO_PinOutputModeConfig(NFC_POWER, PIN_OUTPUT_MODE_NORMAL_FUNCTION);
    GPIO_PinModeConfig(NFC_POWER, PINMODE_DEFAULT);
    GPIO_PinMuxFun(NFC_POWER, 0);
    GPIO_SetPinDir(NFC_POWER, GPIO_OUTPUT);
    GPIO_WritePinOutput(NFC_POWER, 1);  // Better power NFC up so IRQ will work unpowered later
    //** GPIOS
    if (!(~(*(volatile unsigned int *)0x4A080000) << 30)) {
        NVIC_EnableIRQ(ExtPin5_IRQn);
        NVIC_EnableIRQ(RTC_IRQn);
    }
}

void setupCLKCalib() {
    (*(volatile unsigned int *)0x4A070004) = ((*(volatile unsigned int *)0x4A070004) & 0xFFFFFFE0) + 2;
    PMU->PWR_MODE.BF.PWR_MODE = 2;
    uint32_t v0 = FLASH_WordRead(FLASH_NORMAL_READ, 4u);
    char v1;
    if (!(~v0 << 25)) {
        CLK_RC32MEnable();
        while (CLK_GetClkStatus(CLK_OUT_RC32M) != 1)
            ;
        v1 = CLK_RC32MCalibration(CLK_AUTO_CAL, 0);
        FLASH_WordWrite(FLASH_PROGRAM_NORMAL, 4u, (v0 & 0xFFFFFF00) | (v1 & 0x7F));
    }
}

void TagAssociated() {
    // associated
    struct AvailDataInfo *avail;
    // Is there any reason why we should do a long (full) get data request (including reason, status)?
    if ((longDataReqCounter > LONG_DATAREQ_INTERVAL) || wakeUpReason != WAKEUP_REASON_TIMED) {
        // check if we should do a voltage measurement (those are pretty expensive)
        if (voltageCheckCounter == VOLTAGE_CHECK_INTERVAL) {
            doVoltageReading();
            voltageCheckCounter = 0;
        } else {
            powerUp(INIT_TEMPREADING);
        }
        voltageCheckCounter++;

        // check if the battery level is below minimum, and force a redraw of the screen

        if ((lowBattery && !lowBatteryShown && tagSettings.enableLowBatSymbol) || (noAPShown && tagSettings.enableNoRFSymbol)) {
            printf("For some reason, we're going to redraw the image. lowbat=%d, lowbatshown=%d, noAPShown=%d\n", lowBattery, lowBatteryShown, noAPShown);
            // Check if we were already displaying an image
            /*
            if (curImgSlot != 0xFF) {
                powerUp(INIT_EEPROM | INIT_EPD);
                wdt60s();
                drawImageFromEeprom(curImgSlot);
                powerDown(INIT_EEPROM | INIT_EPD);
            } else {
                powerUp(INIT_EPD);
                showAPFound();
                powerDown(INIT_EPD);
            }
            */
        }

        powerUp(INIT_RADIO);
        avail = getAvailDataInfo();
        powerDown(INIT_RADIO);

        if (avail != NULL) {
            // we got some data!
            longDataReqCounter = 0;
            // since we've had succesful contact, and communicated the wakeup reason succesfully, we can now reset to the 'normal' status
            wakeUpReason = WAKEUP_REASON_TIMED;
        }
        if (tagSettings.enableTagRoaming) {
            uint8_t roamChannel = channelSelect();
            if (roamChannel) currentChannel = roamChannel;
        }
    } else {
        powerUp(INIT_RADIO);
        avail = getShortAvailDataInfo();
        powerDown(INIT_RADIO);
    }

    addAverageValue();

    if (avail == NULL) {
        // no data :( this means no reply from AP
        nextCheckInFromAP = 0;  // let the power-saving algorithm determine the next sleep period
    } else {
        nextCheckInFromAP = avail->nextCheckIn;
        // got some data from the AP!
        if (avail->dataType != DATATYPE_NOUPDATE) {
            // data transfer
            if (processAvailDataInfo(avail)) {
                // succesful transfer, next wake time is determined by the NextCheckin;
            } else {
                // failed transfer, let the algorithm determine next sleep interval (not the AP)
                nextCheckInFromAP = 0;
            }
        } else {
            // no data transfer, just sleep.
        }
    }

    uint16_t nextCheckin = getNextSleep();
    longDataReqCounter += nextCheckin;

    if (nextCheckin == INTERVAL_AT_MAX_ATTEMPTS) {
        // We've averaged up to the maximum interval, this means the tag hasn't been in contact with an AP for some time.
        if (tagSettings.enableScanForAPAfterTimeout) {
            currentTagMode = TAG_MODE_CHANSEARCH;
            return;
        }
    }

    // if the AP told us to sleep for a specific period, do so.
    if (nextCheckInFromAP) {
        sleep_with_with_wakeup(nextCheckInFromAP * 60000UL);
    } else {
        sleep_with_with_wakeup(getNextSleep() * 1000UL);
    }
}

void TagChanSearch() {
    // not associated
    if (((scanAttempts != 0) && (scanAttempts % VOLTAGEREADING_DURING_SCAN_INTERVAL == 0)) || (scanAttempts > (INTERVAL_1_ATTEMPTS + INTERVAL_2_ATTEMPTS))) {
        doVoltageReading();
    }

    // try to find a working channel
    currentChannel = channelSelect();

    // Check if we should redraw the screen with icons, info screen or screensaver
    if ((!currentChannel && !noAPShown && tagSettings.enableNoRFSymbol) ||
        (lowBattery && !lowBatteryShown && tagSettings.enableLowBatSymbol) ||
        (scanAttempts == (INTERVAL_1_ATTEMPTS + INTERVAL_2_ATTEMPTS - 1))) {
        powerUp(INIT_EPD);
        wdt60s();
        if (curImgSlot != 0xFF) {
            powerUp(INIT_EEPROM);
            drawImageFromEeprom(curImgSlot);
            powerDown(INIT_EEPROM);
        } else if ((scanAttempts >= (INTERVAL_1_ATTEMPTS + INTERVAL_2_ATTEMPTS - 1))) {
            showLongTermSleep();
        } else {
            showNoAP();
        }
        powerDown(INIT_EPD);
    }

    // did we find a working channel?
    if (currentChannel) {
        // now associated! set up and bail out of this loop.
        scanAttempts = 0;
        wakeUpReason = WAKEUP_REASON_NETWORK_SCAN;
        initPowerSaving(INTERVAL_BASE);
        doSleep(getNextSleep() * 1000UL);
        currentTagMode = TAG_MODE_ASSOCIATED;
        return;
    } else {
        // still not associated
        doSleep(getNextScanSleep(true) * 1000UL);
    }
}

int main(void) {
    (*(volatile unsigned int *)0x20124000) = 0x100004;    // On WARM RESET: Goto this address. -> entry
    (*(volatile unsigned int *)0xE000ED08) = 0x20100000;  // Vector table in RAM and offset 0x4000
    (*(volatile unsigned int *)0xE000E41A) = 0x40;        // ??

    timerInit();

    CLK_SystemClkInit(CLK_SYS_XTAL64M, CLK_SYS_64M);
    CLK_Xtal32MEnable(CLK_OSC_INTERN);
    while (CLK_GetClkStatus(CLK_OUT_XTAL64M) != 1)
        ;

    if (!loadValidateAonRam() || PMU_GetLastResetCause()) {
        setupWDT();
        setupGPIO();
        setupCLKCalib();
        setupUART();
        printf("Rst reason: %i\r\n", PMU_GetLastResetCause());
        printf("AON is not valid!\n");
        setupRTC();
        clearAonRam();
        currentChannel = 0;
        zigbeeCalibData.isValid = false;
        wakeUpReason = WAKEUP_REASON_FIRSTBOOT;
        prvGetSelfMac();
        initializeProto();
        printf("Erz data\r\n");
        initPowerSaving(INTERVAL_BASE);
        loadDefaultSettings();
        doVoltageReading();

        qspiEraseRange(EEPROM_IMG_START, EEPROM_IMG_LEN);
        qspiEraseRange(EEPROM_UPDATE_START, EEPROM_UPDATE_LEN);
        qspiEraseRange(EEPROM_SETTINGS_AREA_START, EEPROM_SETTINGS_AREA_LEN);

        sprintf(macStr, "(" MACFMT ")", MACCVT(mSelfMac));
        currentChannel = showChannelSelect();
        WDT_RestartCounter();
        if (currentChannel) {
            printf("AP Found\r\n");
            showAPFound();
            sprintf(macStr1, "OpenEPaperLink Ch: %i", currentChannel);
            timerDelay(TIMER_TICKS_PER_MSEC * 1000);
            currentTagMode = TAG_MODE_ASSOCIATED;
        } else {
            printf("No AP found\r\n");
            sleep_with_with_wakeup(120000UL);
            currentTagMode = TAG_MODE_CHANSEARCH;
        }

    } else {
        // setupWDT();
        setupGPIO();
        // setupCLKCalib();
        // setupUART();
        // setupRTC();
        memset(curBlock.requestedParts, 0x00, BLOCK_REQ_PARTS_BYTES);
    }

    while (1) {
        powerUp(INIT_UART);
        wdt10s();
        switch (currentTagMode) {
            case TAG_MODE_ASSOCIATED:
                TagAssociated();
                break;
            case TAG_MODE_CHANSEARCH:
                TagChanSearch();
                break;
        }
    }
    return 0;
}

int _write(int file, char *ptr, int len) {
    UART_SendBytes(1, ptr, len);
    return len;
}

void _putchar(char c) {
    _write(0, &c, 1);
}