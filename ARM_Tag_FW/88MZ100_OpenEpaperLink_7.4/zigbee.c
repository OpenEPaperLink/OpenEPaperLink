//#include <stdio.h>
#include <stdint.h>
#include "util.h"
#include "zigbee.h"
#include "printf.h"

volatile uint8_t calibration_irq_ocoured = 0;
volatile uint8_t zigbee_tx_done = 0;
volatile uint8_t zigbee_rx_enabled = 0;

volatile uint8_t crc_or_ack_rx1 = 0;
volatile uint8_t crc_or_ack_rx = 0;

volatile uint8_t rx_buffer[0x400];
volatile uint8_t new_rx = 0;
volatile uint8_t new_rssi = 0;
volatile int rx_len = 0;

uint8_t channelList[6] = {11, 15, 20, 25, 26, 27};

extern uint8_t mSelfMac[8];


void sub_102142(int a1)
{
    unsigned int v1;
    unsigned int v2;

    switch (a1)
    {
    case 15:
        (*(volatile unsigned int *)0x4C010040) &= 0x3FFFFFFFu;
        v1 = ((*(volatile unsigned int *)0x4C010004) & 0xFFFFFFF) | 0x20000000;
        goto LABEL_9;
    case 18:
        (*(volatile unsigned int *)0x4C010040) &= 0x3FFFFFFFu;
        v1 = ((*(volatile unsigned int *)0x4C010004) & 0xFFFFFFF) | 0x10000000;
        goto LABEL_9;
    case 20:
        (*(volatile unsigned int *)0x4C010040) &= 0x3FFFFFFFu;
        v1 = ((*(volatile unsigned int *)0x4C010004) & 0xFFFFFFF) | 0x40000000;
        goto LABEL_9;
    case 22:
        (*(volatile unsigned int *)0x4C010004) &= 0xFFFFFFFu;
        v2 = ((*(volatile unsigned int *)0x4C010040) & 0x3FFFFFFF) | 0x40000000;
        goto LABEL_7;
    case 24:
        (*(volatile unsigned int *)0x4C010004) &= 0xFFFFFFFu;
        v2 = ((*(volatile unsigned int *)0x4C010040) & 0x3FFFFFFF) | 0x80000000;
    LABEL_7:
        (*(volatile unsigned int *)0x4C010040) = v2;
        return;
    case 25:
        (*(volatile unsigned int *)0x4C010040) &= 0x3FFFFFFFu;
        v1 = ((*(volatile unsigned int *)0x4C010004) & 0xFFFFFFF) | 0x80000000;
        goto LABEL_9;
    default:
        (*(volatile unsigned int *)0x4C01003C) = (*(volatile unsigned int *)0x4C010040) & 0x3FFFFFFF;
        v1 = (*(volatile unsigned int *)0x4C010004) & 0xFFFFFFF;
    LABEL_9:
        (*(volatile unsigned int *)0x4C010004) = v1;
        return;
    }
}

int sub_102124()
{
    return ((*(volatile unsigned int *)0x4C010008) >> 18) & 0x3F;
}

void sub_10212E()
{
    (*(volatile unsigned int *)0x4C010040) &= 0x3FFFFFFFu;
    (*(volatile unsigned int *)0x4C010004) &= 0xFFFFFFFu;
}

int get_register(int a1)
{
    return *(uint32_t *)a1;
}

void set_register(uint32_t result, int a2)
{
    *(uint32_t *)result = a2;
}

void fill_rx_regs()
{
    unsigned int v0;
    v0 = (*(volatile unsigned int *)0xFFC);
    (*(volatile unsigned int *)0x4C010000) |= 0x20u;
    (*(volatile unsigned int *)0x4C010000) &= ~1u;
    (*(volatile unsigned int *)0x4C010000) |= 0x40u;
    while (((*(volatile unsigned int *)0x4C010008) & 0x1000000) == 0)
        ;
    for (int i = 0; i < v0 >> 2; ++i)
        set_register(4 * i + 0x4C014000, *(uint32_t *)(4 * i + 0x1000));
    set_register(0x4C014088u, 0x638B4E1C);
    set_register(0x4C0140A4u, 0x13121010);
    set_register(0x4C0140E0u, 0xC2F00844);
    set_register(0x4C014F5Cu, 0x6F0F02F0);
    set_register(0x4C014F60u, 0x75003B75);
    set_register(0x4C014F64u, 0x1002003C);
    set_register(0x4C014F68u, 0x90FAE06F);
    set_register(0x4C01505Cu, 0x600F0205);
    set_register(0x4C015060u, 0x756F1002);
    set_register(0x4C015064u, 0xFC7580FB);
    set_register(0x4C015068u, 0x3EFD751C);
    set_register(0x4C01506Cu, 0x75651102);
    set_register(0x4C015168u, 0x4301FD43);
    set_register(0x4C01527Cu, 0x75F0FB43);
    set_register(0x4C01404Cu, 0xD21E1200);
    set_register(0x4C01405Cu, 0xE5381F12);
    set_register(0x4C014F38u, 0x1E125F00);
    set_register(0x4C014F3Cu, 0x241F12D2);
    set_register(0x4C015ED0u, 0x467522F0);
    set_register(0x4C015ED4u, 0x88477589);
    set_register(0x4C015ED8u, 0xE04B3C90);
    set_register(0x4C015EDCu, 0x5A3F74FA);
    set_register(0x4C015EE0u, 0x3C9026F5);
    set_register(0x4C015EE4u, 0x53FAE04B);
    set_register(0x4C015EE8u, 0xBAC002);
    set_register(0x4C015EECu, 0xBA0F8002);
    set_register(0x4C015EF0u, 0x12800240);
    set_register(0x4C015EF4u, 0x800280BA);
    set_register(0x4C015EF8u, 0x20C0BA15);
    set_register(0x4C015EFCu, 0x2741880);
    set_register(0x4C015F00u, 0x25F52625);
    set_register(0x4C015F04u, 0x4741680);
    set_register(0x4C015F08u, 0x25F52625);
    set_register(0x4C015F0Cu, 0x6740E80);
    set_register(0x4C015F10u, 0x25F52625);
    set_register(0x4C015F14u, 0x8740680);
    set_register(0x4C015F18u, 0x25F52625);
    set_register(0x4C015F1Cu, 0x85432585);
    set_register(0x4C015F20u, 0x224426);
    set_register(0x4C015F24u, 0x3C202C2);
    set_register(0x4C015F28u, 0x75002775);
    set_register(0x4C015F2Cu, 0x42750045);
    set_register(0x4C015F30u, 0xF97500);
    set_register(0x4C015F34u, 0x2200FB75);
    set_register(0x4C015F38u, 0x7500E675);
    set_register(0x4C015F3Cu, 0x22F0EB);
    (*(volatile unsigned int *)0x4C010000) &= ~0x40u;
    while (((*(volatile unsigned int *)0x4C010008) & 0x1000000) != 0)
        ;
}

void load_calib()
{
    int v0;
    unsigned int v1;
    unsigned int i;

    v0 = (*(volatile unsigned int *)0x4C01000C);
    v1 = zigbeeCalibData.len;
    (*(volatile unsigned int *)0x4C010000) |= 4u;
    while (((*(volatile unsigned int *)0x4C010008) & 0x1000000) == 0)
        ;
    for (i = 0; i < v1; ++i)
        set_register(v0 + 4 * i + 0x4C014000, zigbeeCalibData.data[i]);
    (*(volatile unsigned int *)0x4C010000) &= ~4u;
    while (((*(volatile unsigned int *)0x4C010008) & 0x1000000) != 0)
        ;
}



// It is from 0x130000 up to 0x1301000 But you can only use 0x130404 up to the 0x1301000


void save_calib_in_ram()
{
    int v0;
    unsigned int v1;
    unsigned int i;

    v0 = (*(volatile unsigned int *)0x4C01000C) + 0x4C014000;
    v1 = ((unsigned int)(uint8_t)((*(volatile unsigned int *)0x4C010008) >> 2) + 3) >> 2;
    zigbeeCalibData.isValid = true;
    (*(volatile unsigned int *)0x4C01001C) = -5;
    (*(volatile unsigned int *)0x4C010000) |= 2u;
    while (((*(volatile unsigned int *)0x4C010008) & 0x1000000) == 0)
        ;
    for (i = 0; i < v1; ++i){
        zigbeeCalibData.data[i] = *(uint32_t *)(v0 + 4 * i);
    }
    (*(volatile unsigned int *)0x4C010000) &= ~2u;
    while (((*(volatile unsigned int *)0x4C010008) & 0x1000000) != 0)
        ;
}

int inner_calibration()
{
    int is_in_ram;

    (*(volatile unsigned int *)0x4C010000) |= 0x20u;
    if(zigbeeCalibData.isValid)
    {
        is_in_ram = 1;
        (*(volatile unsigned int *)0x4C010000) |= 8u;
        (*(volatile unsigned int *)0x4C010000) &= ~0x10u;
        load_calib();
    }
    else
    {
        is_in_ram = 0;
        (*(volatile unsigned int *)0x4C010000) &= ~8u;
        (*(volatile unsigned int *)0x4C010000) |= 0x10u;
    }
    (*(volatile unsigned int *)0x4C010018) = -1;
    (*(volatile unsigned int *)0x4C01001C) = -4;
    (*(volatile unsigned int *)0xE000E100) = 0x1000000;
    (*(volatile unsigned int *)0x4C010000) &= ~0x20u;
    while (calibration_irq_ocoured != 1)
        ;
    calibration_irq_ocoured = 0;
    while (((*(volatile unsigned int *)0x4C010008) & 2) == 0)
    {
    }
    if (is_in_ram)
        return 2;
    int timeout = 10;
    while (((*(volatile unsigned int *)0x4C010008) & 2) == 0)
    {
        if (timeout-- == 0)
        {
            return 0;
        }
    }
    save_calib_in_ram();
    return 1;
}
void set_zig_channel(int a1)
{
    unsigned int *v1;
    unsigned int v2;
    unsigned int v3;

    (*(volatile unsigned int *)0x4C010004) = ((*(volatile unsigned int *)0x4C010004) & 0xF0FFFFFF) | ((a1 & 0xF) << 24);
    v1 = (unsigned int *)(8 * a1 + 0x35A4);
    v2 = *v1++;
    (*(volatile unsigned int *)0x4C010024) = ((*(volatile unsigned int *)0x4C010024) & 0xFC01FFFF) | (((v2 >> 8) & 0x1FF) << 17);
    v3 = *v1;
    v1 += 33;
    (*(volatile unsigned int *)0x4C010024) = ((*(volatile unsigned int *)0x4C010024) & 0xFFFE0000) | (v3 & 0x1FFFF);
    (*(volatile unsigned int *)0x4C010030) = ((*(volatile unsigned int *)0x4C010030) & 0xFC01FFFF) | (((*v1 >> 8) & 0x1FF) << 17);
    (*(volatile unsigned int *)0x4C010030) = ((*(volatile unsigned int *)0x4C010030) & 0xFFFE0000) | (v1[1] & 0x1FFFF);
}

void RF_calibration(int mode)
{
    fill_rx_regs();
    calibration_irq_ocoured = 0;
    if (inner_calibration())
    {
        set_zig_channel(mode);
        sub_102142(18);
        while (sub_102124() != 18)
            ;
        sub_102142(20);
        while (sub_102124() != 20)
            ;
        sub_10212E();
    }
    else
    {
        printf("RF Calibration failed\n\r");
    }
}

void __attribute__((interrupt)) PHY1_IRQHandler(void)
{
    (*(volatile unsigned int *)0x4C010018) = 0xFFFFFFFF;
    calibration_irq_ocoured = 1;
}

void Zigbee_set_cal_values()
{
    (*(volatile unsigned int *)0x4C000400) = 0x28C0;
    (*(volatile unsigned int *)0x4C000404) = 0x100800;
    (*(volatile unsigned int *)0x4C000408) = 0x800080;
    (*(volatile unsigned int *)0x4C00040C) = 0x145434CC;
    (*(volatile unsigned int *)0x4C000410) = 0x1D0E8;
    (*(volatile unsigned int *)0x4C000414) = 0x6555406;
    (*(volatile unsigned int *)0x4C000418) = 0xC13193D9;
    (*(volatile unsigned int *)0x4C00041C) = 0xD06A428;
    (*(volatile unsigned int *)0x4C000420) = 0x58B14289;
    (*(volatile unsigned int *)0x4C000424) = 0x54093012;
    (*(volatile unsigned int *)0x4C000428) = 0x44133AD5;
    (*(volatile unsigned int *)0x4C000438) = 0x25DF1;
    (*(volatile unsigned int *)0x4C00043C) = 0x1FC;
    (*(volatile unsigned int *)0x4C000440) = 0x2096824C;
}

void ZIGBEE_something_16()
{
    (*(volatile unsigned int *)0x4C000828) = 0x90BD;
    (*(volatile unsigned int *)0x4C000830) = 0x10908000;
}

void ZIGBEE_set_filter_state(int a1)
{
    unsigned int v1;
    (*(volatile unsigned int *)0x4C00084C) |= 0x2000u;
    (*(volatile unsigned int *)0x4C00084C) |= 0x1000u;
    (*(volatile unsigned int *)0x4C00084C) |= 0x800u;
    if (a1 == 1)
    {
        (*(volatile unsigned int *)0x4C00084C) |= 0x400u;
        (*(volatile unsigned int *)0x4C00084C) |= 0x200u;
        (*(volatile unsigned int *)0x4C00084C) &= ~0x100u;
        (*(volatile unsigned int *)0x4C00084C) |= 0x80u;
        (*(volatile unsigned int *)0x4C00084C) |= 0x40u;
        (*(volatile unsigned int *)0x4C00084C) |= 0x20u;
        v1 = (*(volatile unsigned int *)0x4C00084C) | 0x10;
    }
    else
    {
        (*(volatile unsigned int *)0x4C00084C) &= ~0x400u;
        (*(volatile unsigned int *)0x4C00084C) &= ~0x200u;
        (*(volatile unsigned int *)0x4C00084C) &= ~0x100u;
        (*(volatile unsigned int *)0x4C00084C) &= ~0x80u;
        (*(volatile unsigned int *)0x4C00084C) |= 0x40u;
        (*(volatile unsigned int *)0x4C00084C) |= 0x20u;
        v1 = (*(volatile unsigned int *)0x4C00084C) & 0xFFFFFFEF;
    }
    (*(volatile unsigned int *)0x4C00080C) = ((*(volatile unsigned int *)0x4C00080C) & 0xF3FFFFFF) + 0x4000000;
    (*(volatile unsigned int *)0x4C00084C) = v1 | 0xF;
}

void ZIGBEE_something_13(int result)
{
    (*(volatile unsigned int *)0x4C00003C) = result;
}

void sub_101D40(int result)
{
    (*(volatile unsigned int *)0x4C000034) = result;
}

void sub_101D48(int a1)
{
    (*(volatile unsigned int *)0x4C000800) = (*(volatile unsigned int *)0x4C000800) & 0xFFFFFDFF;
    (*(volatile unsigned int *)0x4C000804) = ((*(volatile unsigned int *)0x4C000804) & 0xFFFFFFFC) | (a1 & 3);
    (*(volatile unsigned int *)0x4C000800) |= 0x8000u;
}

void sub_101D10()
{
    (*(volatile unsigned int *)0x4C000000) |= 2u;
    (*(volatile unsigned int *)0x4C001020) = 15;
    delay_us(10u);
    (*(volatile unsigned int *)0x4C000000) &= ~2u;
    (*(volatile unsigned int *)0x4C001020) = 0;
    (*(volatile unsigned int *)0x4C00002C) = -1;
    (*(volatile unsigned int *)0x4C00003C) = -1;
    (*(volatile unsigned int *)0x4C00004C) = -1;
}

void ZIGBEE_something_24()
{
    sub_101D10();
    (*(volatile unsigned int *)0x4C000800) |= 0x800000u;
    (*(volatile unsigned int *)0x4C000808) |= 0x10000000u;
    (*(volatile unsigned int *)0x4C000808) &= ~0x2000000u;
    (*(volatile unsigned int *)0x4C00081C) &= ~1u;
    ZIGBEE_something_13(-1);
    sub_101D40(-16);
    (*(volatile unsigned int *)0xE000E100) = 0x4000000;
    sub_101D48(0);
}

void ZIGBEE_something_17(unsigned int a1, int a2, int a3, int a4, uint32_t *a5, int a6)
{
    unsigned int v10; // r6
    int v11;          // r7
    int v12;          // r0
    unsigned int v13; // r4

    v10 = ((*(volatile unsigned int *)0x4C000830) >> 18) & 0x3F;
    if (a1 < v10)
    {
        v11 = 4 * (((*(volatile unsigned int *)0x4C000828) >> 10) & 0x3FF) + 1275071488;
        v12 = get_register((a1 & 0xFFFFFFFC) + v11);
        set_register((a1 & 0xFFFFFFFC) + v11, (v12 & ~(255 << ((8 * a1) & 0x1F))) | (a2 << ((8 * a1) & 0x1F)));
        v13 = ((v10 + 3) & 0xFFFFFFFC) + v11 + 16 * a1;
        set_register(v13, a3 | (a4 << 16));
        get_register(v13);
        set_register(v13 + 4, *a5);
        set_register(v13 + 8, a5[1]);
        set_register(v13 + 12, a6);
    }
}

int get_crc_or_ack_rx()
{
    return (*(volatile unsigned int *)0x4C000846);
}

int get_crc_or_ack_rx1()
{
    return (uint8_t)((*(volatile unsigned int *)0x4C000834) >> 9);
}

uint8_t ZIGBEE_get_RSSI()
{
  return (uint8_t)((*(volatile unsigned int *)0x4C00042C) >> 14);
}

void __attribute__((interrupt)) PHY3_IRQHandler(void)
{
    int irq_reason = (*(volatile unsigned int *)0x4C000804) & 3;
    sub_101D40(0xFFFFFFFF);
    if (irq_reason)
    {
        if (irq_reason == 3)
        {
            zigbee_tx_done = 1;
            crc_or_ack_rx = get_crc_or_ack_rx() == 0;
            crc_or_ack_rx1 = get_crc_or_ack_rx1() == 11;
        }
    }
    else
    {
        int rx_posi = 0;
        rx_len = (*(volatile unsigned int *)0x4C000444) & 0x3ff;
        for (int i = 0; i < ((rx_len - 1) / 4 + 1); ++i)
            *((uint32_t *)&rx_buffer[0] + rx_posi++) = (*(volatile unsigned int *)0x4C001004);
        new_rssi = ZIGBEE_get_RSSI();
        new_rx = 1;
    }
    ZIGBEE_something_13(-1);
    if (zigbee_rx_enabled)
        ZIGBEE_something_24();
}
void Zigbee_RX_enable()
{
    zigbee_rx_enabled = 1;
    ZIGBEE_something_13(-1);
    ZIGBEE_something_24();
}
void Zigbee_RX_disable()
{
    zigbee_rx_enabled = 0;
    ZIGBEE_something_13(-1);
}

uint8_t Zigbee_tx_buffer(uint8_t *tx_buffer, int len)
{
    //printf("TX ZIGBEE Len: %d\r\n", len);
    int v7 = 0xFFFFFF;
    uint8_t ack_expected = (tx_buffer[0] & 0x20) != 0;
    sub_101D10();
    (*(volatile unsigned int *)0x4C00081C) &= ~1u;
    (*(volatile unsigned int *)0x4C000804) = (*(volatile unsigned int *)0x4C000804) & 0xFFFFFFF3;
    (*(volatile unsigned int *)0x4C000808) = ((*(volatile unsigned int *)0x4C000808) & 0xFE00FFFF) | ((len & 0x1FF) << 16);
    for (int i = 0; i < ((len - 1) / 4 + 1); i++)
    {
        (*(volatile unsigned int *)0x4C001000) = *(uint32_t *)&tx_buffer[i * 4];
    }
    (*(volatile unsigned int *)0x4C000808) = ((*(volatile unsigned int *)0x4C000808) & 0xFDFFFFFF) | ((ack_expected & 1) << 25);
    (*(volatile unsigned int *)0x4C000804) = ((*(volatile unsigned int *)0x4C000804) & 0xFF00FFFF) | ((uint8_t)0 << 16);
    (*(volatile unsigned int *)0x4C000800) &= ~0x800000u;
    ZIGBEE_something_13(-1);
    sub_101D40(0xFFFFFFF0);
    (*(volatile unsigned int *)0xE000E100) = 0x4000000;
    zigbee_tx_done = 0;
    sub_101D48(3);
    while (--v7)
    {
        if (zigbee_tx_done)
        {
            if (!ack_expected)
            {
                // printf("ZIGBEE MESSAGE TXED!!!!\r\n");
                return 0;
            }
            else
            {
                if (crc_or_ack_rx1)
                {
                    // printf("ZIGBEE MESSAGE ACKed\r\n");
                    return 0;
                }
                else
                {
                    return 1;
                }
            }
        }
    }
    printf("TIMEOUT ZIGBEE TX\r\n");
    return 1;
}

void ZIGBEE_set_filter(uint16_t dest_pan, uint16_t source_addr, uint32_t own_mac[], uint8_t rx_filter)
{
    zigbee_rx_enabled = 1;
    ZIGBEE_something_16();
    ZIGBEE_something_17(0, 21, dest_pan, source_addr, own_mac, 0);
    ZIGBEE_set_filter_state(rx_filter & 1);
    ZIGBEE_something_24();
}

void radioSetTxPower(char result)
{
    // Sets TX power to 40 -> +10dBm
    (*(volatile unsigned int *)0x4C000404) |= 2u;
    (*(volatile unsigned int *)0x4C000400) = ((*(volatile unsigned int *)0x4C000400) & 0xFFFFC0FF) | ((result & 0x3F) << 8);
}

void RF_init(uint8_t channel)
{
    channel -= 11;
    // Seems to enable something in the radio, only with this radio start correctly
    (*(volatile unsigned int *)0x4A08005C) &= 0xFFFFFFFE;
    // RF CALIB INIT
    RF_calibration(18);
    (*(volatile unsigned int *)0x4C010000) = ((*(volatile unsigned int *)0x4C010000) & 0xFFFF1FFF) | (1 << 13);
    (*(volatile unsigned int *)0x4C000810) |= 0x10000000u;
    (*(volatile unsigned int *)0x4C00040C) = ((*(volatile unsigned int *)0x4C00040C) & 0xFFFFFF9F) | (32);
    (*(volatile unsigned int *)0x4C00040C) = (*(volatile unsigned int *)0x4C00040C) & 0xFFFFF87F;
    (*(volatile unsigned int *)0x4C000810) = ((*(volatile unsigned int *)0x4C000810) & 0xF800FFFF) | 0x800000;
    (*(volatile unsigned int *)0x4C00080C) = ((*(volatile unsigned int *)0x4C00080C) & 0xFFFFFF00) + 125;
    Zigbee_set_cal_values();

    radioSetTxPower(40);
    // DATA init
    set_zig_channel(channel);
}
void radioInit()
{
    RF_init(11);
    ZIGBEE_set_filter(0x4447, 0xffff, (uint32_t *)mSelfMac, 1);
}

void radioSetChannel(uint8_t channel)
{
    RF_init(channel);
    ZIGBEE_set_filter(0x4447, 0xffff, (uint32_t *)mSelfMac, 1);
}

void radioRxEnable(uint8_t state)
{
   /* if (state)
        Zigbee_RX_enable();
    else
        Zigbee_RX_disable();*/
}

void radioRxFlush()
{
}