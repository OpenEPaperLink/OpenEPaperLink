#include <stdio.h>
#include <stdint.h>
#include "core_cm3.h"
#include "mz100_gpio.h"
#include "mz100_interrupt.h"
#include "mz100_rtc.h"
#include "mz100_sleep.h"
#include "mz100_pinmux.h"
#include "mz100_uart.h"
#include "mz100_wdt.h"
#include "mz100_pmu.h"
#include "mz100_sleep.h"
#include "util.h"
#include "gpio.h"
#include "main.h"
#include "proto.h"

void AON_level_VDD(int state)
{
  PMU->PMIP_AON_LVL.BF.VDD_AON_CTRL = state;
}

void AON_level_VAA(int state)
{
  PMU->PMIP_AON_LVL.BF.VAA_AON_CTRL = state;
}

void PAD_Power_Down(int PAD, int state)
{
  if (PAD == 0)
  {
    if (state)
      PMU->PAD_PDB.BF.PAD_FL_PDB = 1;
    else
      PMU->PAD_PDB.BF.PAD_FL_PDB = 0;
  }
  else if (PAD == 1)
  {
    if (state)
      PMU->PAD_PDB.BF.PAD_AON_PDB = 1;
    else
      PMU->PAD_PDB.BF.PAD_AON_PDB = 0;
  }
  else if (PAD == 2)
  {
    if (state)
      PMU->PAD_PDB.BF.PAD_VDD2_PDB = 1;
    else
      PMU->PAD_PDB.BF.PAD_VDD2_PDB = 0;
  }
}

void Set_Wakeup_pin_rf(int a1)
{
  PMU_WakeupTrigMode_Type v2; // r1

  GPIO_PinMuxFun(RF_WAKEUP_PIN, GPIO5_IRQ_5);
  if (a1 == 1)
  {
    GPIO_PinModeConfig(RF_WAKEUP_PIN, PINMODE_PULLDOWN);
    v2 = PMU_WAKEUP_EDGE_RISING;
  }
  else
  {
    GPIO_PinModeConfig(RF_WAKEUP_PIN, PINMODE_PULLUP);
    v2 = PMU_WAKEUP_EDGE_FALLING;
  }
  PMU_ConfigWakeupPin(PMU_GPIO5_INT, v2);
  PMU_ClearWakeupExtpin(PMU_GPIO5_INT);
  GPIO_IntClr(RF_WAKEUP_PIN);
  NVIC_ClearPendingIRQ(ExtPin5_IRQn);
  NVIC_EnableIRQ(ExtPin5_IRQn);
}

void Set_Wakeup_pin_nfc()
{
  PMU_WakeupTrigMode_Type v2; // r1

  GPIO_PinMuxFun(NFC_IRQ, GPIO7_IRQ_7);
  GPIO_PinModeConfig(NFC_IRQ, PINMODE_PULLUP);
  PMU_ConfigWakeupPin(PMU_GPIO7_INT, PMU_WAKEUP_EDGE_FALLING);
  PMU_ClearWakeupExtpin(PMU_GPIO7_INT);
  GPIO_IntClr(NFC_IRQ);
  NVIC_ClearPendingIRQ(ExtPin7_IRQn);
  NVIC_EnableIRQ(ExtPin7_IRQn);
}

extern struct blockRequest curBlock;     // used by the block-requester, contains the next request that we'll send
extern struct AvailDataInfo curDataInfo; // last 'AvailDataInfo' we received from the AP
extern bool requestPartialBlock;         // if we should ask the AP to get this block from the host or not
void sleep_with_with_wakeup(uint32_t sleep_time_ms)
{
  memcpy((uint8_t *)&(*(volatile unsigned int *)0x130500), (uint8_t *)&curBlock, sizeof(struct blockRequest));
  memcpy((uint8_t *)&(*(volatile unsigned int *)0x130600), (uint8_t *)&curDataInfo, sizeof(struct AvailDataInfo));
  printf("sleep: %u\r\n", sleep_time_ms);
  uint32_t sleep_time_ms_1;
  AON_level_VDD(7);
  AON_level_VAA(0);
  PAD_Power_Down(1, 0);
  PAD_Power_Down(2, 0);
  init_GPIO_sleep();
  PMU_SetSleepMode(PMU_PM4);
  if (sleep_time_ms <= 131000) // longer sleep needs different calculation
    sleep_time_ms_1 = 32767 * sleep_time_ms / 1000;
  else
    sleep_time_ms_1 = 32767 * (sleep_time_ms / 1000);
  RTC_Stop();
  RTC_CounterReset();
  RTC_IntClr(RTC_INT_CNT_UPP);
  NVIC_ClearPendingIRQ(RTC_IRQn);
  RTC_IntMask(RTC_INT_CNT_UPP, UNMASK);
  NVIC_EnableIRQ(RTC_IRQn);
  RTC_SetCounterUppVal(sleep_time_ms_1);
  if (sleep_time_ms_1)
    RTC_Start();
  PMU->CLK_SRC.BF.MAIN_CLK_SOURCE = 1;
  Set_Wakeup_pin_rf(1);
  Set_Wakeup_pin_nfc();
  __WFI();
}