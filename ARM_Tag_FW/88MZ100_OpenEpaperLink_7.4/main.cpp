// #include "main.h"

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "epd_interface.h"
#include "powermgt.h"

#include "main.h"

#include "settings.h"
extern "C" {

#include "comms.h"
#include "mz100/core_cm3.h"
#include "mz100/eeprom.h"

#include "mz100/gpio.h"
#include "mz100/mz100.h"
#include "mz100/mz100_aon_ram.h"
#include "mz100/mz100_clock.h"
#include "mz100/mz100_flash.h"
#include "mz100/mz100_gpio.h"
#include "mz100/mz100_pinmux.h"
#include "mz100/mz100_pmu.h"
#include "mz100/mz100_sleep.h"
#include "mz100/mz100_ssp.h"
#include "mz100/mz100_uart.h"
#include "mz100/printf.h"
#include "proto.h"

#include "mz100/timer.h"

#include "mz100/util.h"
#include "zigbee.h"
extern void dump(const uint8_t *a, const uint16_t l);
}

#include "oepl-protocol.h"

#include "compression.h"
#include "userinterface.h"

#include "oepl_fs.h"

#define SW_VER_CURRENT (0x0000011300000000ull)  // top 16 bits are off limits, xxxx.VV.tt.vvvv.mmmm means version V.t.v.m
#define FW_MAGIC (0x14AFEEBCDC14AE5Aull)

uint64_t __attribute__((section(".ver"))) mCurVersionExport = SW_VER_CURRENT;
uint64_t __attribute__((section(".fwmagic"))) magic = FW_MAGIC;

#define TAG_MODE_CHANSEARCH 0
#define TAG_MODE_ASSOCIATED 1

__attribute__((section(".aon"))) uint8_t currentTagMode = TAG_MODE_CHANSEARCH;
__attribute__((section(".aon"))) volatile struct zigbeeCalibDataStruct zigbeeCalibData;

bool protectedFlashWrite(uint32_t address, uint8_t *buffer, uint32_t num) {
    uint8_t attempt = 3;
    uint8_t *buf2 = (uint8_t *)malloc(num);
    while (attempt--) {
        qspiEraseRange(address, num);
        delay(50);
        FLASH_Write((FLASH_ProgramMode_Type) false, address, buffer, num);
        FLASH_Read((FLASH_ReadMode_Type)0, address, buf2, num);
        if (memcmp(buffer, buf2, num) == 0) {
            printf("Flash block at %06X written successfully\n", address);
            free(buf2);
            return true;
        }
        printf("Failed attempt to write flash block at %lu\n", address);
    }
    free(buf2);
    printf("Giving up on writing block at %lu\n", address);
    return false;
}

static void initTagProfile() {
    for (uint8_t c = 0; c < 8; c++) {
        mSelfMac[c] = tagProfile.macAddress[7 - c];
    }
    if ((((uint32_t *)mSelfMac)[0] | ((uint32_t *)mSelfMac)[1]) == 0 || (((uint32_t *)mSelfMac)[0] & ((uint32_t *)mSelfMac)[1]) == 0xffffffff) {  // fastest way to check for all ones or all zeroes

        printf("MAC: mac unknown, taking random Flash ID\n");
        *((uint64_t *)&tagProfile.macAddress) = FLASH_GetUniqueID();
    }

    tag.imageSize = flashRoundUp(sizeof(struct EepromImageHeader) + (tagProfile.xRes * tagProfile.yRes * tagProfile.bpp) / 8);
    tag.OEPLtype = tagProfile.OEPLType;
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
#ifdef DEBUG_MAIN
                printf("MAIN: Channel: %d - LQI: %d RSSI %d\n", channelList[c], mLastLqi, mLastRSSI);
#endif
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
    printf("Doing chansearch...\n");
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
    delay(1000);
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
    printf("-----------> SVCHandler\r\n");
    PMU->CLK_SRC.BF.MAIN_CLK_SOURCE = 1;
    PMU->PWR_MODE.BF.PWR_MODE = 0;
    NVIC_SystemReset();
}

void __attribute__((interrupt)) DebugMonitor(void) {
    printf("-----------> DebugMonitor\r\n");
    PMU->CLK_SRC.BF.MAIN_CLK_SOURCE = 1;
    PMU->PWR_MODE.BF.PWR_MODE = 0;
    NVIC_SystemReset();
}

void __attribute__((interrupt)) PendSVC(void) {
    printf("-----------> PendSVC\r\n");
    PMU->CLK_SRC.BF.MAIN_CLK_SOURCE = 1;
    PMU->PWR_MODE.BF.PWR_MODE = 0;
    NVIC_SystemReset();
}

int32_t setupRTC(uint32_t calibrate) {
    // CLK_Xtal32MEnable(CLK_OSC_INTERN);
    // while (!CLK_GetClkStatus(CLK_OUT_XTAL64M))
    //     ;
    RC32K_CalClk_Div(63, 31);
    CLK_ModuleClkEnable(CLK_RC32K_CAL);
    CLK_RC32KEnable();
    while (!CLK_GetClkStatus(CLK_OUT_RC32K))
        ;
    PMU->RC32K_CAL_CNTL.BF.RC32K_CAL_DIV = 0;
    int32_t calres;
    if (!calibrate) {
        calres = CLK_RC32KCalibration(CLK_RC32KCAL_XTAL64M, CLK_AUTO_CAL, 0);
    } else {
        calres = CLK_RC32KCalibration(CLK_RC32KCAL_XTAL64M, CLK_MANUAL_CAL, calibrate);
    }
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
    return calres;
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
    uartcfg.stopBits = (UART_StopBits_Type)1;
    uartcfg.parity = UART_PARITY_NONE;
    uartcfg.autoFlowControl = DISABLE;
    UART_Init((UART_ID_Type)1, &uartcfg);

    UART_FIFO_Type uartFifo;
    uartFifo.FIFO_ResetRx = (FunctionalState)1;
    uartFifo.FIFO_ResetTx = (FunctionalState)1;
    uartFifo.FIFO_Function = (FunctionalState)1;
    uartFifo.FIFO_RcvrTrigger = (UART_RxFIFOLevel_Type)2;
    uartFifo.FIFO_TxEmptyTrigger = (UART_TxFIFOLevel_Type)3;
    UART_FIFOConfig((UART_ID_Type)1, &uartFifo);
    // UART 1 DEBUG OUT
}

void setupGPIO() {
    //** GPIOS
    init_GPIO_boot();
    // NFC POWER Should be on if NFC is wanted to be used
    GPIO_PinOutputModeConfig(NFC_POWER, PIN_OUTPUT_MODE_NORMAL_FUNCTION);
    GPIO_PinModeConfig(NFC_POWER, PINMODE_DEFAULT);
    GPIO_PinMuxFun(NFC_POWER, (GPIO_PinMuxFunc_Type)0);
    GPIO_SetPinDir(NFC_POWER, GPIO_OUTPUT);
    GPIO_WritePinOutput(NFC_POWER, (GPIO_IO_Type)1);  // Better power NFC up so IRQ will work unpowered later
    //** GPIOS
    if (!(~(*(volatile unsigned int *)0x4A080000) << 30)) {
        NVIC_EnableIRQ(ExtPin5_IRQn);
        NVIC_EnableIRQ(RTC_IRQn);
    }
}

int32_t setupCLKCalib() {
    (*(volatile unsigned int *)0x4A070004) = ((*(volatile unsigned int *)0x4A070004) & 0xFFFFFFE0) + 2;
    // PMU->PWR_MODE.BF.PWR_MODE = 2; // hmmm
    uint32_t v0 = FLASH_WordRead(FLASH_NORMAL_READ, 4u);
    char v1;
    if (!(~v0 << 25)) {
        CLK_RC32MEnable();
        while (CLK_GetClkStatus(CLK_OUT_RC32M) != 1)
            ;
        v1 = CLK_RC32MCalibration(CLK_AUTO_CAL, 0);
        FLASH_WordWrite(FLASH_PROGRAM_NORMAL, 4u, (v0 & 0xFFFFFF00) | (v1 & 0x7F));
        return v1;
    }
    return -1;
}

void checkWDT() {
    uint32_t val1 = WDT_GetCounterVal();
    delay(10000);
    uint32_t val2 = WDT_GetCounterVal();
    if (val1 == val2) {
        printf("WDT: Not running!\n");
    } else {
        printf("WDT: 1: %lu 2: %lu divider is now %lu\n", val1, val2, (val2 - val1) / 10000);
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

        if ((lowBattery && !lowBatteryShown && tagSettings.enableLowBatSymbol) ||
            (!lowBattery && lowBatteryShown) ||
            (noAPShown && tagSettings.enableNoRFSymbol)) {
            // Check if we were already displaying an image
            if (curImgSlot != 0xFF) {
                powerUp(INIT_EEPROM | INIT_EPD);
                wdt60s();
                drawImageFromEeprom(curImgSlot, 0);
                powerDown(INIT_EEPROM | INIT_EPD);
            } else {
                wdt60s();
                showAPFound();
                wdt60s();
            }
        }

        powerUp(INIT_RADIO);
#ifdef DEBUG_MAIN
        printf("MAIN: full request\n");
#endif
        avail = getAvailDataInfo();
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
        (!lowBattery && lowBatteryShown) ||
        (scanAttempts == (INTERVAL_1_ATTEMPTS + INTERVAL_2_ATTEMPTS - 1))) {
        powerUp(INIT_EPD);
        wdt60s();
        if (curImgSlot != 0xFF) {
            powerUp(INIT_EEPROM);
            drawImageFromEeprom(curImgSlot, 0);
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
#ifdef DEBUG_PROTO
        printf("PROTO: Found a working channel from the TagChanSearch loop\n");
#endif
        // now associated! set up and bail out of this loop.
        scanAttempts = 0;
        wakeUpReason = WAKEUP_REASON_NETWORK_SCAN;
        initPowerSaving(INTERVAL_BASE);
        currentTagMode = TAG_MODE_ASSOCIATED;
        sleep_with_with_wakeup(getNextSleep() * 1000UL);
        return;
    } else {
        // still not associated
        sleep_with_with_wakeup(getNextScanSleep(true) * 1000UL);
    }
}

int main(void) {
    (*(volatile unsigned int *)0x20124000) = 0x100004;    // On WARM RESET: Goto this address. -> entry
    (*(volatile unsigned int *)0xE000ED08) = 0x20100000;  // Vector table in RAM and offset 0x4000
    (*(volatile unsigned int *)0xE000E41A) = 0x40;        // ??

    CLK_SystemClkInit(CLK_SYS_XTAL64M, CLK_SYS_64M);
    CLK_Xtal32MEnable(CLK_OSC_INTERN);
    while (CLK_GetClkStatus(CLK_OUT_XTAL64M) != 1)
        ;
    setupUART();
    setupCLKCalib();
    if (!loadValidateAonRam() || PMU_GetLastResetCause()) {
        // cold boot!
        printf("BOOT: Cold boot!\n");
        // calibrate the 32K RC oscillator (autocal), we'll store the result to flash later
        uint32_t rtccal = setupRTC(0);
        setupGPIO();
        timerInit();
        setupWDT();

        clearAonRam();
        // all variables are set to 0 now. This might not be appropriate for all variables, such as:
        curImgSlot = 0xFF;

        // try to load settings
        if (!loadSettings()) {
            // if we couldn't load settings, we'll try to get it from the tagprofile file. Useful during development
            fs->init();
            if (!loadProfileFromFile((char *)"tagprofile.bin")) {
                // whoops. Empty profile, that shouldn't really ever happen, ever.
                printf("We don't know the type of this tag. That's kinda bad, I guess...\n");
            } else {
                fs->deleteFile((char *)"tagprofile.bin");
            }
        } else {
            printf("BOOT: Loaded tag settings from EEPROM\n");
        }
        printf("BOOT: MAC: %02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X\n", tagProfile.macAddress[0], tagProfile.macAddress[1], tagProfile.macAddress[2], tagProfile.macAddress[3], tagProfile.macAddress[4], tagProfile.macAddress[5], tagProfile.macAddress[6], tagProfile.macAddress[7]);
        tagProfile.RC32Kcal = rtccal;

        printf("BOOT: Rst reason: %i\r\n", PMU_GetLastResetCause());

        initTagProfile();

        wdt10s();
        showSplashScreen();
        delay(10000);
        currentChannel = 0;
        zigbeeCalibData.isValid = false;
        wakeUpReason = WAKEUP_REASON_FIRSTBOOT;
        initializeProto();
        initPowerSaving(INTERVAL_BASE);

        doVoltageReading();

        currentChannel = showChannelSelect();
        radioShutdown();
        if (currentChannel) {
            printf("BOOT: AP Found\n");
            wdt10s();
            delay(10000);
            showAPFound();
            wdt10s();
            timerDelay(TIMER_TICKS_PER_MSEC * 1000);
            currentTagMode = TAG_MODE_ASSOCIATED;
        } else {
            printf("BOOT: No AP found\n");
            wdt10s();
            delay(10000);
            showNoAP();
            wdt10s();
            timerDelay(TIMER_TICKS_PER_MSEC * 1000);
            currentTagMode = TAG_MODE_CHANSEARCH;
        }
        writeSettings();
        printf("BOOT: Cold boot complete\n");
        sleep_with_with_wakeup(5 * 1000UL);

    } else {
        setupRTC(tagProfile.RC32Kcal);
        setupWDT();
        setupGPIO();
        timerInit();
    }

    while (1) {
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
    UART_SendBytes((UART_ID_Type)1, ptr, len);
    return len;
}

void _putchar(char c) {
    _write(0, &c, 1);
}

void applyUpdate(uint32_t size) {
    uint32_t ofst, now, pieceSz = 0x2000;
    uint8_t chunkStore[0x2000];

    printf("Applying update\r\n");

    // apparently, the flash process is more reliable if we do these two first
    setupCLKCalib();
    setupRTC(0);

    uint64_t test;
    FLASH_Read((FLASH_ReadMode_Type)0, fsEnd + 0x0168, (uint8_t *)&test, 8);
    if (test != magic) {
        printf("Update didn't have the correct magic number!\n");
        printf("got %llu\n", test);
        delay(1000);
        return;
    }

    showApplyUpdate();

    printf("Applying update\r\n");
    // qspiEraseRange(EEPROM_OS_START, EEPROM_OS_LEN);

    for (ofst = 0; ofst < size; ofst += now) {
        now = size - ofst;
        if (now > pieceSz)
            now = pieceSz;
        printf("Cpy 0x%06x + 0x%04x to 0x%06x\r\n", fsEnd + ofst, now, ofst);
        FLASH_Read((FLASH_ReadMode_Type)0, fsEnd + ofst, chunkStore, now);
        // qspiEraseRange(ofst, now);
        protectedFlashWrite(ofst, chunkStore, now);
        WDT_RestartCounter();
    }
    printf("Resetting!\n");
    delay(1000);
    NVIC_SystemReset();
    while (1)
        ;
}
