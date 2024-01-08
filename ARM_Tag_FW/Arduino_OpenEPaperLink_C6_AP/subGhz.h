
// This code is heavily depending on Dmitrys wonderful Work !
// https://dmitry.gr/?r=05.Projects&proj=29.%20eInk%20Price%20Tags
// Ported and modified to fit the OpenEPaperLink Project by ATC1441 (ATCnetz.de) ~01.2024

#pragma once
#include <Arduino.h>
#include <stdbool.h>

void tiRadioTxConfigure(const uint8_t *myLongMac, uint32_t myShortMac, uint16_t pan);
void tiRadioRxFilterCfg(const uint8_t *myMac, uint32_t myShortMac, uint16_t myPan, bool promisc);
static bool tiRadioPrvSelect(void);
static void tiRadioPrvDeselect(void);
static void read_multiple(uint8_t *dst, uint_fast8_t rxLen);
static int_fast16_t tiRadioPrvStrobe(uint8_t cmd);
static bool tiRadioPrvRegWrite(uint_fast8_t reg, uint_fast8_t val);
static int_fast16_t tiRadioPrvRegRead(uint_fast8_t reg);
static bool tiRadioPrvRegWriteLong(uint8_t reg, const uint8_t *valP, uint8_t len);
bool tiRadioRxEnable(bool on, bool autoAck);
static bool radioPrvMacsEqual(const uint8_t *macA, const uint8_t *macB);
static uint_fast8_t tiRadioPrvGetState(void);
static void tiRadioPrvPacketRx(void);
void tiRadioRxAckReset(void);
int16_t tiRadioRxAckGetLast(void);
int32_t tiRadioRxDequeuePkt(void* dstBuf, uint32_t maxLen, int8_t *rssiP, uint8_t *lqiP);
void data_input_interrupt(void);
static void tiRadioPrvIfInit(void);
static void tiRadioPrvIrqInit(void);
bool tiRadioSetChannel(uint_fast8_t channel);
bool tiRadioTxLL(const void* pkt);

bool init_subGhz(void);
