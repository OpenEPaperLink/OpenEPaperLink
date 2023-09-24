#include <Arduino.h>

#include "hal.h"
#include "wdt.h"

int8_t temperature = 0;
uint32_t batteryRaw = 0;
uint16_t batteryVoltage = 0;
bool lowBattery = false;

bool disablePinInterruptSleep = false;

int8_t startHFCLK(void) {
    if (!isHFCLKstable()) {
        NRF_CLOCK->EVENTS_HFCLKSTARTED = 0;
        NRF_CLOCK->TASKS_HFCLKSTART = 1U;

        while (!isHFCLKstable())
            ;

        return 0;
    } else {
        return -1;
    }
}

uint8_t isHFCLKstable(void) {
    uint32_t stable = (uint32_t)(CLOCK_HFCLKSTAT_STATE_Running << CLOCK_HFCLKSTAT_STATE_Pos) | (CLOCK_HFCLKSTAT_SRC_Xtal << CLOCK_HFCLKSTAT_SRC_Pos);

    if (NRF_CLOCK->HFCLKSTAT == stable) {
        NRF_CLOCK->EVENTS_HFCLKSTARTED = 0;

        return 1;
    } else {
        return 0;
    }
}

void setupBatteryVoltage() {
    NRF_SAADC->CH[0]
        .CONFIG = (SAADC_CH_CONFIG_GAIN_Gain1_6 << SAADC_CH_CONFIG_GAIN_Pos) |
                  (SAADC_CH_CONFIG_MODE_SE << SAADC_CH_CONFIG_MODE_Pos) |
                  (SAADC_CH_CONFIG_REFSEL_Internal << SAADC_CH_CONFIG_REFSEL_Pos) |
                  (SAADC_CH_CONFIG_RESN_Bypass << SAADC_CH_CONFIG_RESN_Pos) |
                  (SAADC_CH_CONFIG_RESP_Bypass << SAADC_CH_CONFIG_RESP_Pos) |
                  (SAADC_CH_CONFIG_TACQ_3us << SAADC_CH_CONFIG_TACQ_Pos);
    NRF_SAADC->CH[0].PSELP = SAADC_CH_PSELP_PSELP_VDD << SAADC_CH_PSELP_PSELP_Pos;
    NRF_SAADC->CH[0].PSELN = SAADC_CH_PSELN_PSELN_NC << SAADC_CH_PSELN_PSELN_Pos;
    NRF_SAADC->RESOLUTION = SAADC_RESOLUTION_VAL_14bit << SAADC_RESOLUTION_VAL_Pos;
    NRF_SAADC->RESULT.MAXCNT = 1;
    NRF_SAADC->RESULT.PTR = (uint32_t)&batteryRaw;
    NRF_SAADC->SAMPLERATE = SAADC_SAMPLERATE_MODE_Task << SAADC_SAMPLERATE_MODE_Pos;
    NRF_SAADC->ENABLE = SAADC_ENABLE_ENABLE_Enabled << SAADC_ENABLE_ENABLE_Pos;
    NRF_SAADC->TASKS_CALIBRATEOFFSET = 1;
    while (NRF_SAADC->EVENTS_CALIBRATEDONE == 0)
        ;
    NRF_SAADC->EVENTS_CALIBRATEDONE = 0;
    while (NRF_SAADC->STATUS == (SAADC_STATUS_STATUS_Busy << SAADC_STATUS_STATUS_Pos))
        ;
    NRF_SAADC->ENABLE = SAADC_ENABLE_ENABLE_Disabled << SAADC_ENABLE_ENABLE_Pos;
}

void getVoltage() {
    NRF_SAADC->ENABLE = SAADC_ENABLE_ENABLE_Enabled << SAADC_ENABLE_ENABLE_Pos;

    NRF_SAADC->TASKS_START = 1;
    while (NRF_SAADC->EVENTS_STARTED == 0)
        ;
    NRF_SAADC->EVENTS_STARTED = 0;

    NRF_SAADC->TASKS_SAMPLE = 1;
    while (NRF_SAADC->EVENTS_END == 0)
        ;
    NRF_SAADC->EVENTS_END = 0;

    // Convert the result to voltage
    // Result = [V(p) - V(n)] * GAIN/REFERENCE * 2^(RESOLUTION)
    // Result = (VDD - 0) * ((1/6) / 0.6) * 2^14
    // VDD = Result / 4551.1
    batteryRaw *= 10000;
    batteryRaw /= 45511;
    batteryVoltage = (uint16_t)batteryRaw;

    printf("voltage = %d\n", batteryVoltage);

    // Stop the SAADC, since it's not used anymore.
    NRF_SAADC->TASKS_STOP = 1;
    while (NRF_SAADC->EVENTS_STOPPED == 0)
        ;
    NRF_SAADC->EVENTS_STOPPED = 0;
    NRF_SAADC->ENABLE = SAADC_ENABLE_ENABLE_Disabled << SAADC_ENABLE_ENABLE_Pos;
}

void setupTemperature() {
    // Trigger temperature measurement

    NRF_TEMP->A0 = NRF_FICR->TEMP.A0;
    NRF_TEMP->A1 = NRF_FICR->TEMP.A1;
    NRF_TEMP->A2 = NRF_FICR->TEMP.A2;
    NRF_TEMP->A3 = NRF_FICR->TEMP.A3;
    NRF_TEMP->A4 = NRF_FICR->TEMP.A4;
    NRF_TEMP->A5 = NRF_FICR->TEMP.A5;

    // top two 'B0' and 'B1' are also in binary.h by the Arduino framework...
    NRF_TEMP->B0 = NRF_FICR->TEMP.B0;
    NRF_TEMP->B1 = NRF_FICR->TEMP.B1;
    NRF_TEMP->B2 = NRF_FICR->TEMP.B2;
    NRF_TEMP->B3 = NRF_FICR->TEMP.B3;
    NRF_TEMP->B4 = NRF_FICR->TEMP.B4;
    NRF_TEMP->B5 = NRF_FICR->TEMP.B5;

    NRF_TEMP->T0 = NRF_FICR->TEMP.T0;
    NRF_TEMP->T1 = NRF_FICR->TEMP.T1;
    NRF_TEMP->T2 = NRF_FICR->TEMP.T2;
    NRF_TEMP->T3 = NRF_FICR->TEMP.T3;
    NRF_TEMP->T4 = NRF_FICR->TEMP.T4;
}

void getTemperature() {
    NRF_TEMP->TASKS_START = 1;

    // Wait until measurement is finished
    while (NRF_TEMP->EVENTS_DATARDY == 0)
        ;
    NRF_TEMP->EVENTS_DATARDY = 0;

    // Read temperature and convert to celcius (rounding down)
    int32_t temp = (int32_t)NRF_TEMP->TEMP;
    temp *= 25;

    temperature = (uint8_t)(temp / 100);

    printf("temp = %lu.%lu\n", temp / 100, temp % 100);
}

void boardGetOwnMac(uint8_t *mac) {
    mac[0] = MAC_ID_1;
    mac[1] = MAC_ID_0;
    mac[2] = (NRF_UICR->CUSTOMER[0]) & 0xFF;
    mac[3] = (NRF_UICR->CUSTOMER[0] >> 8) & 0xFF;
    mac[4] = (NRF_UICR->CUSTOMER[0] >> 16) & 0xFF;
    mac[5] = (NRF_UICR->CUSTOMER[0] >> 24);
    mac[6] = 0;
    mac[7] = 0;
    uint8_t cksum = 0;
    for (uint8_t c = 0; c < 8; c++) {
        cksum ^= mac[c];
        cksum ^= mac[c] >> 4;
    }
    mac[0] += cksum & 0x0F;
}

void watchdog_enable(int timeout) {
    NRF_WDT->CONFIG = NRF_WDT->CONFIG = (WDT_CONFIG_HALT_Pause << WDT_CONFIG_HALT_Pos) | (WDT_CONFIG_SLEEP_Pause << WDT_CONFIG_SLEEP_Pos);
    NRF_WDT->CRV = (32768 * timeout) / 1000;
    NRF_WDT->RREN |= WDT_RREN_RR0_Msk;
    NRF_WDT->TASKS_START = 1;
    NRF_WDT->RR[0] = WDT_RR_RR_Reload;
}

uint32_t sleepMsEntry = 0;
uint32_t loops = 0;
bool interrupted = false;

// uint8_t ledcfg[12] = {0b00100010,0x78,0b00100100,5,0x03,0b01000011,1,0xC2,0b1100001,10,10,0};
// uint8_t ledcfg[12] = {0b00010010,0x7D,0,0,0x03,0xE8,0,0,0,0,0,0};
uint8_t ledcfg[12] = {255, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

void setled(uint64_t parta, u_int32_t partb) {
    ledcfg[0] = parta & 0xFF;
    ledcfg[1] = (parta >> 8) & 0xFF;
    ledcfg[2] = (parta >> 16) & 0xFF;
    ledcfg[3] = (parta >> 24) & 0xFF;
    ledcfg[4] = (parta >> 32) & 0xFF;
    ledcfg[5] = (parta >> 40) & 0xFF;
    ledcfg[6] = (parta >> 48) & 0xFF;
    ledcfg[7] = (parta >> 56) & 0xFF;
    ledcfg[8] = partb & 0xFF;
    ledcfg[9] = (partb >> 8) & 0xFF;
    ledcfg[10] = (partb >> 16) & 0xFF;
    ledcfg[11] = (partb >> 24) & 0xFF;
}

void resettimer() {
    // tell the sleep function to net sleep again
    sleepMsEntry = sleepMsEntry - 999999999;
    loops = 0;
    interrupted = true;
}

void flashled(uint8_t color, uint8_t brightness) {
    uint8_t colorred = (color >> 5) & 0b00000111;
    uint8_t colorgreen = (color >> 2) & 0b00000111;
    uint8_t colorblue = color & 0b00000011;
    for (uint16_t i = 0; i < brightness; i++) {
        digitalWrite(LED_RED, !(colorred >= 7));
        digitalWrite(LED_GREEN, !(colorgreen >= 7));
        digitalWrite(LED_BLUE, !(colorblue >= 3));
        nrf_delay_us(100);
        digitalWrite(LED_RED, !(colorred >= 1));
        digitalWrite(LED_GREEN, !(colorgreen >= 1));
        nrf_delay_us(100);
        digitalWrite(LED_RED, !(colorred >= 6));
        digitalWrite(LED_GREEN, !(colorgreen >= 6));
        digitalWrite(LED_BLUE, !(colorblue >= 1));
        nrf_delay_us(100);
        digitalWrite(LED_RED, !(colorred >= 2));
        digitalWrite(LED_GREEN, !(colorgreen >= 2));
        nrf_delay_us(100);
        digitalWrite(LED_RED, !(colorred >= 5));
        digitalWrite(LED_GREEN, !(colorgreen >= 5));
        nrf_delay_us(100);
        digitalWrite(LED_RED, !(colorred >= 3));
        digitalWrite(LED_GREEN, !(colorgreen >= 3));
        digitalWrite(LED_BLUE, !(colorblue >= 2));
        nrf_delay_us(100);
        digitalWrite(LED_RED, !(colorred >= 4));
        digitalWrite(LED_GREEN, !(colorgreen >= 4));
        nrf_delay_us(100);
        digitalWrite(LED_RED, HIGH);
        digitalWrite(LED_GREEN, HIGH);
        digitalWrite(LED_BLUE, HIGH);
    }
}

void sleepwithinterrupts(uint32_t sleepinterval) {
    yield();
    sleepMsEntry = millis();

    // disable GPIOTE
    if (disablePinInterruptSleep) {
        for (uint8_t c = 0; c < 8; c++) {
            for (uint8_t d = 0; d < 32; d++) {
                uint32_t e = ((uint32_t)d) << 8;
                NRF_GPIOTE->CONFIG[c] = e;
            }
        }
    }

    // sometimes we wake up early
    while (millis() - sleepMsEntry < sleepinterval) {
        initRTC0(sleepinterval);
        __WFE();
        __SEV();
        __WFE();
    }
}

void ledflashlogic(uint32_t ms) {
    watchdog_enable(ms + 1000);
    uint8_t brightness = ledcfg[0] >> 4 & 0b00001111;
    uint8_t mode = ledcfg[0] & 0b00001111;
    // lets not blink for short delays
    if (ms < 2000) mode = 15;
    if (mode == 1) {
        uint8_t color = ledcfg[1];
        uint32_t ledinerv = (ledcfg[2] << 24) + (ledcfg[3] << 16) + (ledcfg[4] << 8) + ledcfg[5];
        uint32_t sleepinterval = ledinerv;
        loops = ms / ledinerv;
        if (loops == 0) {
            loops = 1;
            sleepinterval = ms;
        }
        if (sleepinterval > ms) sleepinterval = ms;
        for (uint32_t i = 0; i < loops; i++) {
            flashled(color, brightness);
            sleepwithinterrupts(sleepinterval);
        }
    } 
    else if (mode == 0) {
        interrupted = false;
        uint8_t interloopdelayfactor = 100;
        u_int8_t loopdelayfactor = 100;
        uint8_t c1 = ledcfg[1];
        uint8_t c2 = ledcfg[3];
        uint8_t c3 = ledcfg[5];
        uint8_t loop1delay = (ledcfg[2] >> 4) & 0b00001111;
        uint8_t loop2delay = (ledcfg[4] >> 4) & 0b00001111;
        uint8_t loop3delay = (ledcfg[6] >> 4) & 0b00001111;
        uint8_t loopcnt1 = ledcfg[2] & 0b00001111;
        uint8_t loopcnt2 = ledcfg[4] & 0b00001111;
        uint8_t loopcnt3 = ledcfg[6] & 0b00001111;
        uint8_t ildelay1 = 0;
        uint8_t ildelay2 = 0;
        uint8_t ildelay3 = 0;
        uint8_t grouprepeats = ledcfg[7];
        uint32_t fulllooptime1 = loopcnt1 * loop1delay * loopdelayfactor + ildelay1 * interloopdelayfactor;
        uint32_t fulllooptime2 = loopcnt2 * loop2delay * loopdelayfactor + ildelay2 * interloopdelayfactor;
        uint32_t fulllooptime3 = loopcnt3 * loop3delay * loopdelayfactor + ildelay3 * interloopdelayfactor;
        uint32_t looptimesum = fulllooptime1 + fulllooptime2 + fulllooptime3;
        int fittingrepeats = (int)ms / looptimesum;

        //catch edge case
        if (grouprepeats == 0) sleepwithinterrupts(ms);

        for (int j = 0; j < fittingrepeats; j++) {
            if(j > grouprepeats){
                    brightness = 0;
                    ledcfg[0] = 0xff;
            }
            if (!interrupted) {
                for (int i = 0; i < loopcnt1; i++) {
                    flashled(c1, brightness);
                    sleepwithinterrupts(loop1delay * loopdelayfactor);
                    if (interrupted) break;
                }
                sleepwithinterrupts(ildelay1 * interloopdelayfactor);
            }
            if (!interrupted) {
                for (int i = 0; i < loopcnt2; i++) {
                    flashled(c2, brightness);
                    sleepwithinterrupts(loop2delay * loopdelayfactor);
                    if (interrupted) break;
                }
                sleepwithinterrupts(ildelay2 * interloopdelayfactor);
            }

            if (!interrupted) {
                for (int i = 0; i < loopcnt3; i++) {
                    flashled(c3, brightness);
                    sleepwithinterrupts(loop3delay * loopdelayfactor);
                    if (interrupted) break;
                }
                sleepwithinterrupts(ildelay3 * interloopdelayfactor);
            }
            if (interrupted) break;
        }
    } else
        sleepwithinterrupts(ms);
}

void sleepForMs(uint32_t ms) {
    // Turn everything off for minimal deep sleep current
    radioRxEnable(0);

    // disable brownout
    NRF_POWER->POFCON &= ~((uint32_t)1);

    NRF_CLOCK->TASKS_HFCLKSTOP = 1U;
    while ((NRF_CLOCK->HFCLKSTAT & 0x10001) == 0x10001)
        ;
    Serial.end();
    yield();
    // led and sleep stuff
    ledflashlogic(ms);
    // we have to restart the serial
    Serial.begin(115200);
}

#define LF_FREQUENCY 32768UL
#define SECONDS(x) ((uint32_t)((LF_FREQUENCY * (float)((float)x / (float)1000)) + 0.5))
uint32_t wakeUpSeconds = 500;

void initRTC0(uint32_t ms) {
    NVIC_DisableIRQ(RTC0_IRQn);
    NVIC_ClearPendingIRQ(RTC0_IRQn);
    NVIC_SetPriority(RTC0_IRQn, 15);
    NRF_RTC0->PRESCALER = 0;
    NRF_RTC0->INTENSET = RTC_EVTENSET_COMPARE0_Enabled << RTC_EVTENSET_COMPARE0_Pos;
    NRF_RTC0->EVTENSET = RTC_INTENSET_COMPARE0_Enabled << RTC_INTENSET_COMPARE0_Pos;
    wakeUpSeconds = ms;
    NRF_RTC0->CC[0] = NRF_RTC0->COUNTER + SECONDS(wakeUpSeconds);
    NVIC_EnableIRQ(RTC0_IRQn);
    NRF_RTC0->TASKS_START = 1;
}

#ifdef __cplusplus
extern "C" {
#endif
void RTC0_IRQHandler(void) {
    volatile uint32_t dummy;
    if (NRF_RTC0->EVENTS_COMPARE[0] == 1) {
        NRF_RTC0->EVENTS_COMPARE[0] = 0;
        NRF_RTC0->CC[0] = NRF_RTC0->COUNTER + SECONDS(wakeUpSeconds);
        dummy = NRF_RTC0->EVENTS_COMPARE[0];
        dummy;
        NRF_RTC0->TASKS_STOP = 1;
    }
}
#ifdef __cplusplus
}
#endif
