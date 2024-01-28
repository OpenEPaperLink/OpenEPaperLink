
#include "nfc.h"
//#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "core_cm3.h"
#include "../main.h"
#include "mz100_rtc.h"
#include "mz100_wdt.h"
#include "mz100_pmu.h"
#include "mz100_pinmux.h"
#include "mz100_gpio.h"
#include "util.h"
#include "printf.h"


void NVIC_some_IRQ1(unsigned int a1)
{
    *(uint32_t *)(4 * (a1 >> 5) - 0x1FFF1E80) = 1 << (a1 & 0x1F);
}
void NIVC_some_IRQ(unsigned int a1)
{
    *(uint32_t *)(4 * (a1 >> 5) - 0x1FFF1D80) = 1 << (a1 & 0x1F);
}

void Pin_pad_set_Low(int pin)
{
    GPIO_PinPadOutputEnable(pin);
    GPIO_PinPadOutputLevel(pin, PIN_PAD_OUTPUT_LOW);
    GPIO_PinOutputModeConfig(pin, PIN_OUTPUT_MODE_PAD);
}

void Pin_pad_set_High(int pin)
{
    GPIO_PinPadOutputEnable(pin);
    GPIO_PinPadOutputLevel(pin, PIN_PAD_OUTPUT_HIGH);
    GPIO_PinOutputModeConfig(pin, PIN_OUTPUT_MODE_PAD);
}

void Pin_pad_set_Normal(int pin)
{
    GPIO_PinPadOutputEnable(pin);
    GPIO_PinPadOutputLevel(pin, PIN_PAD_OUTPUT_LOW);
    GPIO_PinOutputModeConfig(pin, PIN_OUTPUT_MODE_NORMAL_FUNCTION);
}
void init_GPIO_boot()
{
    Pin_pad_set_Normal(NFC_POWER);
    Pin_pad_set_Normal(NFC_IRQ);
    Pin_pad_set_Normal(EPD_MOSI);
    Pin_pad_set_Normal(EPD_MISO);
    Pin_pad_set_Normal(EPD_CLK);
    Pin_pad_set_Normal(EPD_DC);
    Pin_pad_set_Normal(NFC_SCL);
    Pin_pad_set_Normal(NFC_SDA);
    Pin_pad_set_Normal(EPD_BS);
    Pin_pad_set_Normal(EPD_CS);
    Pin_pad_set_Normal(EPD_RESET);
    Pin_pad_set_Normal(EPD_HLT_CTRL);
}
void init_GPIO_sleep()
{
    Pin_pad_set_Low(NFC_POWER);
    //Pin_pad_set_Low(NFC_IRQ);
    Pin_pad_set_Low(EPD_MOSI);
    Pin_pad_set_Low(EPD_MISO);
    Pin_pad_set_Low(EPD_CLK);
    Pin_pad_set_Low(EPD_DC);
    Pin_pad_set_Low(NFC_SCL);
    Pin_pad_set_Low(NFC_SDA);
    Pin_pad_set_High(EPD_BS);
    Pin_pad_set_High(EPD_CS);
    Pin_pad_set_High(EPD_RESET);
    Pin_pad_set_High(EPD_HLT_CTRL);
}

uint8_t WAKEUP_RF = 0;

void __attribute__((interrupt)) ExtPin5_IRQHandler(void)
{
    if (!WAKEUP_RF)
    {
        NVIC_ClearPendingIRQ(ExtPin5_IRQn);
        GPIO_IntMask(RF_WAKEUP_PIN, MASK);
        NVIC_some_IRQ1(ExtPin5_IRQn);
        PMU_ClearWakeupExtpin(PMU_GPIO5_INT);
        NVIC_ClearPendingIRQ(ExtPin5_IRQn);
        WAKEUP_RF = 1;
    }
}

uint32_t gSleepRtcCounter = 0;
uint8_t Ext_Pin27_triggered = 0;
void __attribute__((interrupt)) ExtPin27_IRQHandler(void)
{
    WDT_RestartCounter();
    printf(">>PIN_27_IRQHandler\r\n");
    NVIC_ClearPendingIRQ(ExtPin27_IRQn);
    GPIO_IntMask(EPD_BUSY, MASK);
    NVIC_some_IRQ1(ExtPin27_IRQn);
    PMU_ClearWakeupExtpin(PMU_GPIO27_INT);
    NVIC_ClearPendingIRQ(ExtPin27_IRQn);
    Ext_Pin27_triggered = 1;
}

void enable_irq_for_pin(int a1, unsigned int a2)
{
    PMU_WakeupPinSrc_Type v4;   // r0
    PMU_WakeupPinSrc_Type v5;   // r5
    char v6;                    // r7
    PMU_WakeupTrigMode_Type v7; // r1

    GPIO_PinMuxFun(a2, 7);
    if (a2 > 7)
    {
        if (a2 - 26 > 5)
            return;
        v4 = a2 - 19;
    }
    else
    {
        v4 = a2 - 1;
    }
    v5 = v4;
    v6 = a2 + 31;
    if (a1 == 1)
    {
        GPIO_PinModeConfig(a2, PINMODE_PULLDOWN);
        v7 = PMU_WAKEUP_EDGE_RISING;
    }
    else
    {
        if (a1 != 2)
            goto LABEL_11;
        GPIO_PinModeConfig(a2, PINMODE_PULLUP);
        v7 = PMU_WAKEUP_EDGE_FALLING;
    }
    PMU_ConfigWakeupPin(v5, v7);
LABEL_11:
    PMU_ClearWakeupExtpin(v5);
    GPIO_IntClr((GPIO_NO_Type)a2);
    NVIC_ClearPendingIRQ((IRQn_Type)v6);
    NVIC_EnableIRQ(v6);
}

void wait_busy_sleep(int a1)
{
    unsigned int v1 = 0;
    gSleepRtcCounter = 0;
    printf("=> EPD_BUSYN_PIN : %d\r\n", 27);
    while (1)
    {
        RTC_CounterReset();
        RTC_IntClr(RTC_INT_CNT_UPP);
        NIVC_some_IRQ(0);
        RTC_IntMask(RTC_INT_CNT_UPP, UNMASK);
        (*(volatile unsigned int *)0xE000E100) = 1;
        RTC_SetCounterUppVal(0x7FFF * a1 / 0x3E8u);
        RTC_Start();
        enable_irq_for_pin(1, 27);
        __WFI();
        if (Ext_Pin27_triggered == 1)
            break;
        v1++;
        delay(2000);
        printf("busypin:%d,SCNT:%d\r\n", GPIO_ReadPinLevel(EPD_BUSY), v1);
        if (v1 >= 0x5A)
        {
            printf("DRF BUSY CHECK FAIL\r\n");
            break;
        }
    }
    RTC_Stop();
    RTC_IntClr(RTC_INT_CNT_UPP);
    NIVC_some_IRQ(0);
    (*(volatile unsigned int *)0xE000E180) = 1;
    gSleepRtcCounter = 1000 * RTC_GetCounterVal() / 0x7FFFu + a1 * v1;
    printf("RTC_GetCounterVal(): %d, gSleepRtcCounter:%d(ms)\r\n", RTC_GetCounterVal(), gSleepRtcCounter);
    RTC_CounterReset();
    Ext_Pin27_triggered = 0;
    GPIO_SetPinDir(EPD_BUSY, GPIO_INPUT);
    GPIO_PinMuxFun(EPD_BUSY, 0);
    GPIO_PinModeConfig(EPD_BUSY, PINMODE_PULLUP);
}

void do_sleeped_epd_refresh()
{
    printf("PM2 MODE START!\r\n");
    PMU->PMIP_BRN.BF.BRNDET_EN = 0;
    PMU->PWR_MODE.BF.CAU_ON = 0;
    PMU->PMIP_CHP_CTRL.BF.CHP_ON_OFF = 1;
    PMU_SetSleepMode(PMU_PM2);
    PMU_ClearWakeupExtpin(PMU_GPIO5_INT);
    wait_busy_sleep(2000);
    printf("uDisTime : %d ms\r\n", gSleepRtcCounter);
}
