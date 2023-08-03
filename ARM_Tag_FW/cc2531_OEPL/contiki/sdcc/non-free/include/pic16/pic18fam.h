/*
 * pic18fam.h - PIC16 families
 *
 * This file is has been generated using ./pic18fam-h-gen.pl .
 */
#ifndef __SDCC_PIC18FAM_H__
#define __SDCC_PIC18FAM_H__ 1

/*
 * Define device families.
 */
#undef  __SDCC_PIC16_FAMILY

#if    defined(__SDCC_PIC18F242) \
    || defined(__SDCC_PIC18F252) \
    || defined(__SDCC_PIC18F442) \
    || defined(__SDCC_PIC18F452) \
    || defined(__SDCC_PIC18LF242) \
    || defined(__SDCC_PIC18LF252) \
    || defined(__SDCC_PIC18LF442) \
    || defined(__SDCC_PIC18LF452)
#define __SDCC_PIC16_FAMILY 1802420

#elif  defined(__SDCC_PIC18F248) \
    || defined(__SDCC_PIC18F258) \
    || defined(__SDCC_PIC18F448) \
    || defined(__SDCC_PIC18F458) \
    || defined(__SDCC_PIC18LF248) \
    || defined(__SDCC_PIC18LF258) \
    || defined(__SDCC_PIC18LF448) \
    || defined(__SDCC_PIC18LF458)
#define __SDCC_PIC16_FAMILY 1802480

#elif  defined(__SDCC_PIC18F1220) \
    || defined(__SDCC_PIC18F1320) \
    || defined(__SDCC_PIC18LF1220) \
    || defined(__SDCC_PIC18LF1320)
#define __SDCC_PIC16_FAMILY 1812200

#elif  defined(__SDCC_PIC18F1230) \
    || defined(__SDCC_PIC18F1330) \
    || defined(__SDCC_PIC18LF1230) \
    || defined(__SDCC_PIC18LF1330)
#define __SDCC_PIC16_FAMILY 1812300

#elif  defined(__SDCC_PIC18F13K50) \
    || defined(__SDCC_PIC18F14K50) \
    || defined(__SDCC_PIC18LF13K50) \
    || defined(__SDCC_PIC18LF14K50)
#define __SDCC_PIC16_FAMILY 1813502

#elif  defined(__SDCC_PIC18F2220) \
    || defined(__SDCC_PIC18F2320) \
    || defined(__SDCC_PIC18F4220) \
    || defined(__SDCC_PIC18F4320) \
    || defined(__SDCC_PIC18LF2220) \
    || defined(__SDCC_PIC18LF2320) \
    || defined(__SDCC_PIC18LF4220) \
    || defined(__SDCC_PIC18LF4320)
#define __SDCC_PIC16_FAMILY 1822200

#elif  defined(__SDCC_PIC18F2221) \
    || defined(__SDCC_PIC18F2321) \
    || defined(__SDCC_PIC18F4221) \
    || defined(__SDCC_PIC18F4321) \
    || defined(__SDCC_PIC18LF2221) \
    || defined(__SDCC_PIC18LF2321) \
    || defined(__SDCC_PIC18LF4221) \
    || defined(__SDCC_PIC18LF4321)
#define __SDCC_PIC16_FAMILY 1822210

#elif  defined(__SDCC_PIC18F23K20) \
    || defined(__SDCC_PIC18F24K20) \
    || defined(__SDCC_PIC18F25K20) \
    || defined(__SDCC_PIC18F26K20) \
    || defined(__SDCC_PIC18F43K20) \
    || defined(__SDCC_PIC18F44K20) \
    || defined(__SDCC_PIC18F45K20) \
    || defined(__SDCC_PIC18F46K20)
#define __SDCC_PIC16_FAMILY 1823202

#elif  defined(__SDCC_PIC18F23K22) \
    || defined(__SDCC_PIC18F24K22) \
    || defined(__SDCC_PIC18F25K22) \
    || defined(__SDCC_PIC18F26K22) \
    || defined(__SDCC_PIC18F43K22) \
    || defined(__SDCC_PIC18F44K22) \
    || defined(__SDCC_PIC18F45K22) \
    || defined(__SDCC_PIC18F46K22) \
    || defined(__SDCC_PIC18LF23K22) \
    || defined(__SDCC_PIC18LF24K22) \
    || defined(__SDCC_PIC18LF25K22) \
    || defined(__SDCC_PIC18LF26K22) \
    || defined(__SDCC_PIC18LF43K22) \
    || defined(__SDCC_PIC18LF44K22) \
    || defined(__SDCC_PIC18LF45K22) \
    || defined(__SDCC_PIC18LF46K22)
#define __SDCC_PIC16_FAMILY 1823222

#elif  defined(__SDCC_PIC18F2331) \
    || defined(__SDCC_PIC18F2431) \
    || defined(__SDCC_PIC18F4331) \
    || defined(__SDCC_PIC18F4431) \
    || defined(__SDCC_PIC18LF2331) \
    || defined(__SDCC_PIC18LF2431) \
    || defined(__SDCC_PIC18LF4331) \
    || defined(__SDCC_PIC18LF4431)
#define __SDCC_PIC16_FAMILY 1823310

#elif  defined(__SDCC_PIC18F2410) \
    || defined(__SDCC_PIC18F2510) \
    || defined(__SDCC_PIC18F2515) \
    || defined(__SDCC_PIC18F2610) \
    || defined(__SDCC_PIC18F4410) \
    || defined(__SDCC_PIC18F4510) \
    || defined(__SDCC_PIC18F4515) \
    || defined(__SDCC_PIC18F4610) \
    || defined(__SDCC_PIC18LF2410) \
    || defined(__SDCC_PIC18LF2510) \
    || defined(__SDCC_PIC18LF2515) \
    || defined(__SDCC_PIC18LF2610) \
    || defined(__SDCC_PIC18LF4410) \
    || defined(__SDCC_PIC18LF4510) \
    || defined(__SDCC_PIC18LF4515) \
    || defined(__SDCC_PIC18LF4610)
#define __SDCC_PIC16_FAMILY 1824100

#elif  defined(__SDCC_PIC18F24J10) \
    || defined(__SDCC_PIC18F25J10) \
    || defined(__SDCC_PIC18F44J10) \
    || defined(__SDCC_PIC18F45J10) \
    || defined(__SDCC_PIC18LF24J10) \
    || defined(__SDCC_PIC18LF25J10) \
    || defined(__SDCC_PIC18LF44J10) \
    || defined(__SDCC_PIC18LF45J10)
#define __SDCC_PIC16_FAMILY 1824101

#elif  defined(__SDCC_PIC18F2420) \
    || defined(__SDCC_PIC18F2520) \
    || defined(__SDCC_PIC18F4420) \
    || defined(__SDCC_PIC18F4520) \
    || defined(__SDCC_PIC18LF2420) \
    || defined(__SDCC_PIC18LF2520) \
    || defined(__SDCC_PIC18LF4420) \
    || defined(__SDCC_PIC18LF4520)
#define __SDCC_PIC16_FAMILY 1824200

#elif  defined(__SDCC_PIC18F2423) \
    || defined(__SDCC_PIC18F2523) \
    || defined(__SDCC_PIC18F4423) \
    || defined(__SDCC_PIC18F4523) \
    || defined(__SDCC_PIC18LF2423) \
    || defined(__SDCC_PIC18LF2523) \
    || defined(__SDCC_PIC18LF4423) \
    || defined(__SDCC_PIC18LF4523)
#define __SDCC_PIC16_FAMILY 1824230

#elif  defined(__SDCC_PIC18F2450) \
    || defined(__SDCC_PIC18F4450) \
    || defined(__SDCC_PIC18LF2450) \
    || defined(__SDCC_PIC18LF4450)
#define __SDCC_PIC16_FAMILY 1824500

#elif  defined(__SDCC_PIC18F24J50) \
    || defined(__SDCC_PIC18F25J50) \
    || defined(__SDCC_PIC18F26J50) \
    || defined(__SDCC_PIC18F44J50) \
    || defined(__SDCC_PIC18F45J50) \
    || defined(__SDCC_PIC18F46J50) \
    || defined(__SDCC_PIC18LF24J50) \
    || defined(__SDCC_PIC18LF25J50) \
    || defined(__SDCC_PIC18LF26J50) \
    || defined(__SDCC_PIC18LF44J50) \
    || defined(__SDCC_PIC18LF45J50) \
    || defined(__SDCC_PIC18LF46J50)
#define __SDCC_PIC16_FAMILY 1824501

#elif  defined(__SDCC_PIC18F2455) \
    || defined(__SDCC_PIC18F2458) \
    || defined(__SDCC_PIC18F2550) \
    || defined(__SDCC_PIC18F2553) \
    || defined(__SDCC_PIC18F4455) \
    || defined(__SDCC_PIC18F4458) \
    || defined(__SDCC_PIC18F4550) \
    || defined(__SDCC_PIC18F4553) \
    || defined(__SDCC_PIC18LF2455) \
    || defined(__SDCC_PIC18LF2458) \
    || defined(__SDCC_PIC18LF2550) \
    || defined(__SDCC_PIC18LF2553) \
    || defined(__SDCC_PIC18LF4455) \
    || defined(__SDCC_PIC18LF4458) \
    || defined(__SDCC_PIC18LF4550) \
    || defined(__SDCC_PIC18LF4553)
#define __SDCC_PIC16_FAMILY 1824550

#elif  defined(__SDCC_PIC18F2480) \
    || defined(__SDCC_PIC18F2580) \
    || defined(__SDCC_PIC18F4480) \
    || defined(__SDCC_PIC18F4580) \
    || defined(__SDCC_PIC18LF2480) \
    || defined(__SDCC_PIC18LF2580) \
    || defined(__SDCC_PIC18LF4480) \
    || defined(__SDCC_PIC18LF4580)
#define __SDCC_PIC16_FAMILY 1824800

#elif  defined(__SDCC_PIC18F2525) \
    || defined(__SDCC_PIC18F2620) \
    || defined(__SDCC_PIC18F4525) \
    || defined(__SDCC_PIC18F4620) \
    || defined(__SDCC_PIC18LF2525) \
    || defined(__SDCC_PIC18LF2620) \
    || defined(__SDCC_PIC18LF4525) \
    || defined(__SDCC_PIC18LF4620)
#define __SDCC_PIC16_FAMILY 1825250

#elif  defined(__SDCC_PIC18F2585) \
    || defined(__SDCC_PIC18F2680) \
    || defined(__SDCC_PIC18F4585) \
    || defined(__SDCC_PIC18F4680) \
    || defined(__SDCC_PIC18LF2585) \
    || defined(__SDCC_PIC18LF2680) \
    || defined(__SDCC_PIC18LF4585) \
    || defined(__SDCC_PIC18LF4680)
#define __SDCC_PIC16_FAMILY 1825850

#elif  defined(__SDCC_PIC18F2682) \
    || defined(__SDCC_PIC18F2685) \
    || defined(__SDCC_PIC18F4682) \
    || defined(__SDCC_PIC18F4685) \
    || defined(__SDCC_PIC18LF2682) \
    || defined(__SDCC_PIC18LF2685) \
    || defined(__SDCC_PIC18LF4682) \
    || defined(__SDCC_PIC18LF4685)
#define __SDCC_PIC16_FAMILY 1826820

#elif  defined(__SDCC_PIC18F6520) \
    || defined(__SDCC_PIC18F6620) \
    || defined(__SDCC_PIC18F6720) \
    || defined(__SDCC_PIC18F8520) \
    || defined(__SDCC_PIC18F8620) \
    || defined(__SDCC_PIC18F8720) \
    || defined(__SDCC_PIC18LF6520) \
    || defined(__SDCC_PIC18LF6620) \
    || defined(__SDCC_PIC18LF6720) \
    || defined(__SDCC_PIC18LF8520) \
    || defined(__SDCC_PIC18LF8620) \
    || defined(__SDCC_PIC18LF8720)
#define __SDCC_PIC16_FAMILY 1865200

#elif  defined(__SDCC_PIC18F6527) \
    || defined(__SDCC_PIC18F6622) \
    || defined(__SDCC_PIC18F6627) \
    || defined(__SDCC_PIC18F6722) \
    || defined(__SDCC_PIC18F8527) \
    || defined(__SDCC_PIC18F8622) \
    || defined(__SDCC_PIC18F8627) \
    || defined(__SDCC_PIC18F8722) \
    || defined(__SDCC_PIC18LF6527) \
    || defined(__SDCC_PIC18LF6622) \
    || defined(__SDCC_PIC18LF6627) \
    || defined(__SDCC_PIC18LF6722) \
    || defined(__SDCC_PIC18LF8527) \
    || defined(__SDCC_PIC18LF8622) \
    || defined(__SDCC_PIC18LF8627) \
    || defined(__SDCC_PIC18LF8722)
#define __SDCC_PIC16_FAMILY 1865270

#elif  defined(__SDCC_PIC18F65J50) \
    || defined(__SDCC_PIC18F66J50) \
    || defined(__SDCC_PIC18F66J55) \
    || defined(__SDCC_PIC18F67J50) \
    || defined(__SDCC_PIC18F85J50) \
    || defined(__SDCC_PIC18F86J50) \
    || defined(__SDCC_PIC18F86J55) \
    || defined(__SDCC_PIC18F87J50)
#define __SDCC_PIC16_FAMILY 1865501

#elif  defined(__SDCC_PIC18F6585) \
    || defined(__SDCC_PIC18F6680) \
    || defined(__SDCC_PIC18F8585) \
    || defined(__SDCC_PIC18F8680) \
    || defined(__SDCC_PIC18LF6585) \
    || defined(__SDCC_PIC18LF6680) \
    || defined(__SDCC_PIC18LF8585) \
    || defined(__SDCC_PIC18LF8680)
#define __SDCC_PIC16_FAMILY 1865850

#elif  defined(__SDCC_PIC18F66J60) \
    || defined(__SDCC_PIC18F66J65) \
    || defined(__SDCC_PIC18F67J60) \
    || defined(__SDCC_PIC18F86J60) \
    || defined(__SDCC_PIC18F86J65) \
    || defined(__SDCC_PIC18F87J60) \
    || defined(__SDCC_PIC18F96J60) \
    || defined(__SDCC_PIC18F96J65) \
    || defined(__SDCC_PIC18F97J60)
#define __SDCC_PIC16_FAMILY 1866601

#else
#warning No family associated with the target device. Please update your pic16/pic18fam.h manually and/or inform the maintainer.
#endif

/*
 * Define ADC style per device family.
 */
#undef  __SDCC_ADC_STYLE

#if    (__SDCC_PIC16_FAMILY == 1823310)
#define __SDCC_ADC_STYLE    0

#elif  (__SDCC_PIC16_FAMILY == 1802420) \
    || (__SDCC_PIC16_FAMILY == 1802480)
#define __SDCC_ADC_STYLE    1802420

#elif  (__SDCC_PIC16_FAMILY == 1812200)
#define __SDCC_ADC_STYLE    1812200

#elif  (__SDCC_PIC16_FAMILY == 1812300)
#define __SDCC_ADC_STYLE    1812300

#elif  (__SDCC_PIC16_FAMILY == 1813502)
#define __SDCC_ADC_STYLE    1813502

#elif  (__SDCC_PIC16_FAMILY == 1822200) \
    || (__SDCC_PIC16_FAMILY == 1822210) \
    || (__SDCC_PIC16_FAMILY == 1823202) \
    || (__SDCC_PIC16_FAMILY == 1824100) \
    || (__SDCC_PIC16_FAMILY == 1824101) \
    || (__SDCC_PIC16_FAMILY == 1824200) \
    || (__SDCC_PIC16_FAMILY == 1824230) \
    || (__SDCC_PIC16_FAMILY == 1824500) \
    || (__SDCC_PIC16_FAMILY == 1824550) \
    || (__SDCC_PIC16_FAMILY == 1824800) \
    || (__SDCC_PIC16_FAMILY == 1825250) \
    || (__SDCC_PIC16_FAMILY == 1825850) \
    || (__SDCC_PIC16_FAMILY == 1826820) \
    || (__SDCC_PIC16_FAMILY == 1865200) \
    || (__SDCC_PIC16_FAMILY == 1865270) \
    || (__SDCC_PIC16_FAMILY == 1865850) \
    || (__SDCC_PIC16_FAMILY == 1866601)
#define __SDCC_ADC_STYLE    1822200

#elif  (__SDCC_PIC16_FAMILY == 1823222)
#define __SDCC_ADC_STYLE    1823222

#elif  (__SDCC_PIC16_FAMILY == 1824501)
#define __SDCC_ADC_STYLE    1824501

#elif  (__SDCC_PIC16_FAMILY == 1865501)
#define __SDCC_ADC_STYLE    1865501

#else
#warning No ADC style associated with the target device. Please update your pic16/pic18fam.h manually and/or inform the maintainer.
#endif

/*
 * Define (E)USART style per device family.
 */
#undef  __SDCC_USART_STYLE

#if    (__SDCC_PIC16_FAMILY == 1812200)
#define __SDCC_USART_STYLE  1812200

#elif  (__SDCC_PIC16_FAMILY == 1812300)
#define __SDCC_USART_STYLE  1812300

#elif  (__SDCC_PIC16_FAMILY == 1813502)
#define __SDCC_USART_STYLE  1813502

#elif  (__SDCC_PIC16_FAMILY == 1802420) \
    || (__SDCC_PIC16_FAMILY == 1802480) \
    || (__SDCC_PIC16_FAMILY == 1822200)
#define __SDCC_USART_STYLE  1822200

#elif  (__SDCC_PIC16_FAMILY == 1822210) \
    || (__SDCC_PIC16_FAMILY == 1823202) \
    || (__SDCC_PIC16_FAMILY == 1823222) \
    || (__SDCC_PIC16_FAMILY == 1823310) \
    || (__SDCC_PIC16_FAMILY == 1824100) \
    || (__SDCC_PIC16_FAMILY == 1824101) \
    || (__SDCC_PIC16_FAMILY == 1824200) \
    || (__SDCC_PIC16_FAMILY == 1824230) \
    || (__SDCC_PIC16_FAMILY == 1824550) \
    || (__SDCC_PIC16_FAMILY == 1825250)
#define __SDCC_USART_STYLE  1822210

#elif  (__SDCC_PIC16_FAMILY == 1824500) \
    || (__SDCC_PIC16_FAMILY == 1824800) \
    || (__SDCC_PIC16_FAMILY == 1825850) \
    || (__SDCC_PIC16_FAMILY == 1826820)
#define __SDCC_USART_STYLE  1824500

#elif  (__SDCC_PIC16_FAMILY == 1824501) \
    || (__SDCC_PIC16_FAMILY == 1865270) \
    || (__SDCC_PIC16_FAMILY == 1865501) \
    || (__SDCC_PIC16_FAMILY == 1866601)
#define __SDCC_USART_STYLE  1824501

#elif  (__SDCC_PIC16_FAMILY == 1865200)
#define __SDCC_USART_STYLE  1865200

#elif  (__SDCC_PIC16_FAMILY == 1865850)
#define __SDCC_USART_STYLE  1865850

#else
#warning No (E)USART style associated with the target device. Please update your pic16/pic18fam.h manually and/or inform the maintainer.
#endif

#endif /* !__SDCC_PIC18FAM_H__ */
