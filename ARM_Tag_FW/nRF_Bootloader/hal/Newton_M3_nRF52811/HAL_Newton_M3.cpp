#include <Arduino.h>

#include "hal.h"
#include "wdt.h"

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
bool interruped = false;

//uint8_t ledcfg[12] = {0b00100010,0x78,0b00100100,5,0x03,0b01000011,1,0xC2,0b1100001,10,10,0};
//uint8_t ledcfg[12] = {0b00010010,0x7D,0,0,0x03,0xE8,0,0,0,0,0,0};
uint8_t ledcfg[12] = {255,0,0,0,0,0,0,0,0,0,0,0};

void setled(uint64_t parta,u_int32_t partb){
    ledcfg[0] = parta & 0xFF;
    ledcfg[1] = (parta >> 8)& 0xFF;
    ledcfg[2] = (parta >> 16)& 0xFF;
    ledcfg[3] = (parta >> 24)& 0xFF;
    ledcfg[4] = (parta >> 32)& 0xFF;
    ledcfg[5] = (parta >> 40)& 0xFF;
    ledcfg[6] = (parta >> 48)& 0xFF;
    ledcfg[7] = (parta >> 56)& 0xFF;
    ledcfg[8] = partb & 0xFF;
    ledcfg[9] = (partb >> 8)& 0xFF;
    ledcfg[10] = (partb >> 16)& 0xFF;
    ledcfg[11] = (partb >> 24)& 0xFF;
}

void resettimer(){
    //tell the sleep function to net sleep again
    sleepMsEntry = sleepMsEntry - 999999999;
    loops = 0;
    interruped = true;
}

void flashled(uint8_t color,uint8_t brightnes){
    uint8_t colorred = (color >> 5) & 0b00000111;
    uint8_t colorgreen = (color >> 2) & 0b00000111;
    uint8_t colorblue = color & 0b00000011;
    for(uint16_t i = 0;i < brightnes;i++){
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

void sleepwithinterrupts(uint32_t sleepinterval){
   yield();
   sleepMsEntry = millis();
   //sometimes we wake up early
   while (millis() - sleepMsEntry < sleepinterval){
        initRTC0(sleepinterval);
        __WFE();
        __SEV();
        __WFE();
    }
}

void ledflashlogic(uint32_t ms){
    watchdog_enable(ms + 1000);
    uint8_t brightnes = ledcfg[0] >> 4 & 0b00001111;
    uint8_t mode = ledcfg[0] & 0b00001111;
    //lets not blink for short delays
    if(ms < 2000)mode = 15;
    //if(mode == 0)sleepwithinterrupts(ms);
    if(mode == 1){
    uint8_t color = ledcfg[1];
    uint32_t ledinerv = (ledcfg[2] << 24 )+ (ledcfg[3] << 16)+ (ledcfg[4] << 8)+ ledcfg[5];
    uint32_t sleepinterval = ledinerv;
    loops = ms / ledinerv;
    if(loops == 0){
        loops = 1;
        sleepinterval = ms;
    }
    if(sleepinterval > ms)sleepinterval = ms;
    for(uint32_t i = 0;i < loops;i++){
    flashled(color,brightnes);
    sleepwithinterrupts(sleepinterval);
    }
    }
    else if(mode == 0){
        interruped = false;
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
        int fittingrepeats = (int) ms /  looptimesum;

        grouprepeats = fittingrepeats;
        if(grouprepeats == 0)grouprepeats = 1;

        for(int j = 0;j < grouprepeats;j++){

        if(!interruped){
        for(int i = 0;i < loopcnt1;i++){
            flashled(c1,brightnes);
            sleepwithinterrupts(loop1delay * loopdelayfactor);
            if(interruped)break;
        }
        sleepwithinterrupts(ildelay1 * interloopdelayfactor);
        }
        if(!interruped){
        for(int i = 0;i < loopcnt2;i++){
            flashled(c2,brightnes);
            sleepwithinterrupts(loop2delay * loopdelayfactor);
            if(interruped)break;
        }
        sleepwithinterrupts(ildelay2 * interloopdelayfactor);
        }
        
        if(!interruped){
        for(int i = 0;i < loopcnt3;i++){
            flashled(c3,brightnes);
            sleepwithinterrupts(loop3delay * loopdelayfactor);
            if(interruped)break;
        }
        sleepwithinterrupts(ildelay3 * interloopdelayfactor);
        }
        if(interruped)break;
        }
    }
    else sleepwithinterrupts(ms);
}

void sleepForMs(uint32_t ms) {
    // Turn everything off for minimal deep sleep current
    //radioRxEnable(0);
    NRF_CLOCK->TASKS_HFCLKSTOP = 1U;
    while ((NRF_CLOCK->HFCLKSTAT & 0x10001) == 0x10001);
    Serial.end();
    yield();
    //led and sleep stuff
    ledflashlogic(ms);
    //we have to restart the serial
    Serial.begin(115200);
    Serial.println("wu");
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
