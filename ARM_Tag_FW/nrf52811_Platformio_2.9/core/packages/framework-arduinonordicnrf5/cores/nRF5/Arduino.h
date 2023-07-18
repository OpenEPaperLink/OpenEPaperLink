#ifndef Arduino_h
#define Arduino_h

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "nrf.h"
#include "nrf_peripherals.h"

typedef bool boolean;
typedef uint8_t byte;
typedef uint16_t word;

// some libraries and sketches depend on this AVR stuff,
// assuming Arduino.h or WProgram.h automatically includes it...

#include "avr/pgmspace.h"
#include "avr/interrupt.h"

#include "itoa.h"

#ifdef __cplusplus
extern "C"{
#endif // __cplusplus

#include "wiring_constants.h"

#define clockCyclesPerMicrosecond() ( SystemCoreClock / 1000000L )
#define clockCyclesToMicroseconds(a) ( ((a) * 1000L) / (SystemCoreClock / 1000L) )
#define microsecondsToClockCycles(a) ( (a) * (SystemCoreClock / 1000000L) )

void yield( void ) ;

/* sketch */
void setup( void ) ;
void loop( void ) ;

#include "WVariant.h"

#ifdef __cplusplus
} // extern "C"
#endif

// The following headers are for C++ only compilation
#ifdef __cplusplus
  #include "WCharacter.h"
  #include "WString.h"
  // #include "Tone.h"
  #include "WMath.h"
  #include "HardwareSerial.h"
  #include "pulse.h"
#endif
#include "delay.h"
#include "binary.h"
#ifdef __cplusplus
  #include "Uart.h"
#endif

// Include board variant
#include "variant.h"

#include "wiring.h"
#include "wiring_digital.h"
#include "wiring_analog.h"
#include "wiring_shift.h"
#include "WInterrupts.h"

// undefine stdlib's abs if encountered
#ifdef abs
#undef abs
#endif // abs

#define min(a,b) ((a)<(b)?(a):(b))
#define max(a,b) ((a)>(b)?(a):(b))
#define abs(x) ((x)>0?(x):-(x))
#define constrain(amt,low,high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))
#define round(x)     ((x)>=0?(long)((x)+0.5):(long)((x)-0.5))
#define radians(deg) ((deg)*DEG_TO_RAD)
#define degrees(rad) ((rad)*RAD_TO_DEG)
#define sq(x) ((x)*(x))

#define interrupts() __enable_irq()
#define noInterrupts() __disable_irq()

#define lowByte(w) ((uint8_t) ((w) & 0xff))
#define highByte(w) ((uint8_t) ((w) >> 8))

#define bitRead(value, bit) (((value) >> (bit)) & 0x01)
#define bitSet(value, bit) ((value) |= (1UL << (bit)))
#define bitClear(value, bit) ((value) &= ~(1UL << (bit)))
#define bitWrite(value, bit, bitvalue) (bitvalue ? bitSet(value, bit) : bitClear(value, bit))

#define bit(b) (1UL << (b))

#if (GPIO_COUNT == 1)
#define gpioBaseForPin(P)          ( NRF_GPIO )
#define digitalPinToPort(P)        ( (NRF_GPIO_Type *) gpioBaseForPin(P) )
#define digitalPinToPin(P)         ( g_ADigitalPinMap[P] )
#define digitalPinToBitMask(P)     ( 1 << digitalPinToPin(P) )
#elif (GPIO_COUNT == 2)
#define gpioBaseForPin(P)          ( (g_ADigitalPinMap[P] & 0x20) ? NRF_P1 : NRF_P0 )
#define digitalPinToPort(P)        ( (NRF_GPIO_Type *) gpioBaseForPin(P) )
#define digitalPinToPin(P)         ( g_ADigitalPinMap[P] & 0x1f )
#define digitalPinToBitMask(P)     ( 1 << digitalPinToPin(P) )
#else
#error "Unsupported GPIO_COUNT"
#endif

#define portOutputRegister(port)   ( &(port->OUT) )
#define portInputRegister(port)    ( &(port->IN) )
#define portModeRegister(port)     ( &(port->DIR) )
#define digitalPinHasPWM(P)        ( true )

/*
 * digitalPinToTimer(..) is AVR-specific and is not defined for nRF52
 * architecture. If you need to check if a pin supports PWM you must
 * use digitalPinHasPWM(..).
 *
 * https://github.com/arduino/Arduino/issues/1833
 */
// #define digitalPinToTimer(P)

// Interrupts
#define digitalPinToInterrupt(P)   ( P )

#ifdef __cplusplus
#include "Uart.h"
#endif // __cplusplus

#endif // Arduino_h
