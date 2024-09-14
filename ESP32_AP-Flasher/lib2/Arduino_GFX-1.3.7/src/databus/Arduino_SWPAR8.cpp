#include "Arduino_SWPAR8.h"

Arduino_SWPAR8::Arduino_SWPAR8(
    int8_t dc, int8_t cs, int8_t wr, int8_t rd,
    int8_t d0, int8_t d1, int8_t d2, int8_t d3, int8_t d4, int8_t d5, int8_t d6, int8_t d7)
    : _dc(dc), _cs(cs), _wr(wr), _rd(rd),
      _d0(d0), _d1(d1), _d2(d2), _d3(d3), _d4(d4), _d5(d5), _d6(d6), _d7(d7)
{
}

bool Arduino_SWPAR8::begin(int32_t speed, int8_t dataMode)
{
  UNUSED(speed);
  UNUSED(dataMode);

  pinMode(_dc, OUTPUT);
  digitalWrite(_dc, HIGH); // Data mode
  if (_cs != GFX_NOT_DEFINED)
  {
    pinMode(_cs, OUTPUT);
    digitalWrite(_cs, HIGH); // Deselect
  }
  pinMode(_wr, OUTPUT);
  digitalWrite(_wr, HIGH);
  if (_rd != GFX_NOT_DEFINED)
  {
    pinMode(_rd, OUTPUT);
    digitalWrite(_rd, HIGH);
  }
  pinMode(_d0, OUTPUT);
  digitalWrite(_d0, LOW);
  pinMode(_d1, OUTPUT);
  digitalWrite(_d1, LOW);
  pinMode(_d2, OUTPUT);
  digitalWrite(_d2, LOW);
  pinMode(_d3, OUTPUT);
  digitalWrite(_d3, LOW);
  pinMode(_d4, OUTPUT);
  digitalWrite(_d4, LOW);
  pinMode(_d5, OUTPUT);
  digitalWrite(_d5, LOW);
  pinMode(_d6, OUTPUT);
  digitalWrite(_d6, LOW);
  pinMode(_d7, OUTPUT);
  digitalWrite(_d7, LOW);

#if defined(USE_FAST_PINIO)
#if defined(HAS_PORT_SET_CLR)
#if defined(ARDUINO_ARCH_NRF52840)
  uint32_t pin = digitalPinToPinName((pin_size_t)_dc);
  NRF_GPIO_Type *reg = nrf_gpio_pin_port_decode(&pin);
  _dcPortSet = &reg->OUTSET;
  _dcPortClr = &reg->OUTCLR;
  _dcPinMask = 1UL << pin;
  if (_cs != GFX_NOT_DEFINED)
  {
    pin = digitalPinToPinName((pin_size_t)_cs);
    reg = nrf_gpio_pin_port_decode(&pin);
    _csPortSet = &reg->OUTSET;
    _csPortClr = &reg->OUTCLR;
    _csPinMask = 1UL << pin;
  }
  pin = digitalPinToPinName((pin_size_t)_wr);
  reg = nrf_gpio_pin_port_decode(&pin);
  _wrPortSet = &reg->OUTSET;
  _wrPortClr = &reg->OUTCLR;
  _wrPinMask = 1UL << pin;
  if (_rd != GFX_NOT_DEFINED)
  {
    pin = digitalPinToPinName((pin_size_t)_rd);
    reg = nrf_gpio_pin_port_decode(&pin);
    _rdPortSet = &reg->OUTSET;
    _rdPortClr = &reg->OUTCLR;
    _rdPinMask = 1UL << pin;
  }
  pin = digitalPinToPinName((pin_size_t)_d0);
  reg = nrf_gpio_pin_port_decode(&pin);
  _d0PortSet = &reg->OUTSET;
  _d0PortClr = &reg->OUTCLR;
  _d0PinMask = 1UL << pin;
  pin = digitalPinToPinName((pin_size_t)_d1);
  reg = nrf_gpio_pin_port_decode(&pin);
  _d1PortSet = &reg->OUTSET;
  _d1PortClr = &reg->OUTCLR;
  _d1PinMask = 1UL << pin;
  pin = digitalPinToPinName((pin_size_t)_d2);
  reg = nrf_gpio_pin_port_decode(&pin);
  _d2PortSet = &reg->OUTSET;
  _d2PortClr = &reg->OUTCLR;
  _d2PinMask = 1UL << pin;
  pin = digitalPinToPinName((pin_size_t)_d3);
  reg = nrf_gpio_pin_port_decode(&pin);
  _d3PortSet = &reg->OUTSET;
  _d3PortClr = &reg->OUTCLR;
  _d3PinMask = 1UL << pin;
  pin = digitalPinToPinName((pin_size_t)_d4);
  reg = nrf_gpio_pin_port_decode(&pin);
  _d4PortSet = &reg->OUTSET;
  _d4PortClr = &reg->OUTCLR;
  _d4PinMask = 1UL << pin;
  pin = digitalPinToPinName((pin_size_t)_d5);
  reg = nrf_gpio_pin_port_decode(&pin);
  _d5PortSet = &reg->OUTSET;
  _d5PortClr = &reg->OUTCLR;
  _d5PinMask = 1UL << pin;
  pin = digitalPinToPinName((pin_size_t)_d6);
  reg = nrf_gpio_pin_port_decode(&pin);
  _d6PortSet = &reg->OUTSET;
  _d6PortClr = &reg->OUTCLR;
  _d6PinMask = 1UL << pin;
  pin = digitalPinToPinName((pin_size_t)_d7);
  reg = nrf_gpio_pin_port_decode(&pin);
  _d7PortSet = &reg->OUTSET;
  _d7PortClr = &reg->OUTCLR;
  _d7PinMask = 1UL << pin;
#elif defined(TARGET_RP2040)
  _dcPinMask = digitalPinToBitMask(_dc);
  _dcPortSet = (PORTreg_t)&sio_hw->gpio_set;
  _dcPortClr = (PORTreg_t)&sio_hw->gpio_clr;
  if (_cs != GFX_NOT_DEFINED)
  {
    _csPinMask = digitalPinToBitMask(_cs);
    _csPortSet = (PORTreg_t)&sio_hw->gpio_set;
    _csPortClr = (PORTreg_t)&sio_hw->gpio_clr;
  }
  _wrPinMask = digitalPinToBitMask(_wr);
  _wrPortSet = (PORTreg_t)&sio_hw->gpio_set;
  _wrPortClr = (PORTreg_t)&sio_hw->gpio_clr;
  if (_rd != GFX_NOT_DEFINED)
  {
    _rdPinMask = digitalPinToBitMask(_rd);
    _rdPortSet = (PORTreg_t)&sio_hw->gpio_set;
    _rdPortClr = (PORTreg_t)&sio_hw->gpio_clr;
  }
  _d0PinMask = digitalPinToBitMask(_d0);
  _d0PortSet = (PORTreg_t)&sio_hw->gpio_set;
  _d0PortClr = (PORTreg_t)&sio_hw->gpio_clr;
  _d1PinMask = digitalPinToBitMask(_d1);
  _d1PortSet = (PORTreg_t)&sio_hw->gpio_set;
  _d1PortClr = (PORTreg_t)&sio_hw->gpio_clr;
  _d2PinMask = digitalPinToBitMask(_d2);
  _d2PortSet = (PORTreg_t)&sio_hw->gpio_set;
  _d2PortClr = (PORTreg_t)&sio_hw->gpio_clr;
  _d3PinMask = digitalPinToBitMask(_d3);
  _d3PortSet = (PORTreg_t)&sio_hw->gpio_set;
  _d3PortClr = (PORTreg_t)&sio_hw->gpio_clr;
  _d4PinMask = digitalPinToBitMask(_d4);
  _d4PortSet = (PORTreg_t)&sio_hw->gpio_set;
  _d4PortClr = (PORTreg_t)&sio_hw->gpio_clr;
  _d5PinMask = digitalPinToBitMask(_d5);
  _d5PortSet = (PORTreg_t)&sio_hw->gpio_set;
  _d5PortClr = (PORTreg_t)&sio_hw->gpio_clr;
  _d6PinMask = digitalPinToBitMask(_d6);
  _d6PortSet = (PORTreg_t)&sio_hw->gpio_set;
  _d6PortClr = (PORTreg_t)&sio_hw->gpio_clr;
  _d7PinMask = digitalPinToBitMask(_d7);
  _d7PortSet = (PORTreg_t)&sio_hw->gpio_set;
  _d7PortClr = (PORTreg_t)&sio_hw->gpio_clr;
#elif defined(ESP32) && (CONFIG_IDF_TARGET_ESP32C3)
  _dcPinMask = digitalPinToBitMask(_dc);
  _dcPortSet = (PORTreg_t)&GPIO.out_w1ts;
  _dcPortClr = (PORTreg_t)&GPIO.out_w1tc;
  if (_cs != GFX_NOT_DEFINED)
  {
    _csPinMask = digitalPinToBitMask(_cs);
    _csPortSet = (PORTreg_t)&GPIO.out_w1ts;
    _csPortClr = (PORTreg_t)&GPIO.out_w1tc;
  }
  _wrPinMask = digitalPinToBitMask(_wr);
  _wrPortSet = (PORTreg_t)&GPIO.out_w1ts;
  _wrPortClr = (PORTreg_t)&GPIO.out_w1tc;
  if (_rd != GFX_NOT_DEFINED)
  {
    _rdPinMask = digitalPinToBitMask(_rd);
    _rdPortSet = (PORTreg_t)&GPIO.out_w1ts;
    _rdPortClr = (PORTreg_t)&GPIO.out_w1tc;
  }
  _d0PinMask = digitalPinToBitMask(_d0);
  _d0PortSet = (PORTreg_t)&GPIO.out_w1ts;
  _d0PortClr = (PORTreg_t)&GPIO.out_w1tc;
  _d1PinMask = digitalPinToBitMask(_d1);
  _d1PortSet = (PORTreg_t)&GPIO.out_w1ts;
  _d1PortClr = (PORTreg_t)&GPIO.out_w1tc;
  _d2PinMask = digitalPinToBitMask(_d2);
  _d2PortSet = (PORTreg_t)&GPIO.out_w1ts;
  _d2PortClr = (PORTreg_t)&GPIO.out_w1tc;
  _d3PinMask = digitalPinToBitMask(_d3);
  _d3PortSet = (PORTreg_t)&GPIO.out_w1ts;
  _d3PortClr = (PORTreg_t)&GPIO.out_w1tc;
  _d4PinMask = digitalPinToBitMask(_d4);
  _d4PortSet = (PORTreg_t)&GPIO.out_w1ts;
  _d4PortClr = (PORTreg_t)&GPIO.out_w1tc;
  _d5PinMask = digitalPinToBitMask(_d5);
  _d5PortSet = (PORTreg_t)&GPIO.out_w1ts;
  _d5PortClr = (PORTreg_t)&GPIO.out_w1tc;
  _d6PinMask = digitalPinToBitMask(_d6);
  _d6PortSet = (PORTreg_t)&GPIO.out_w1ts;
  _d6PortClr = (PORTreg_t)&GPIO.out_w1tc;
  _d7PinMask = digitalPinToBitMask(_d7);
  _d7PortSet = (PORTreg_t)&GPIO.out_w1ts;
  _d7PortClr = (PORTreg_t)&GPIO.out_w1tc;
#elif defined(ESP32)
  _dcPinMask = digitalPinToBitMask(_dc);
  if (_dc >= 32)
  {
    _dcPortSet = (PORTreg_t)&GPIO.out1_w1ts.val;
    _dcPortClr = (PORTreg_t)&GPIO.out1_w1tc.val;
  }
  else
  {
    _dcPortSet = (PORTreg_t)&GPIO.out_w1ts;
    _dcPortClr = (PORTreg_t)&GPIO.out_w1tc;
  }
  if (_cs >= 32)
  {
    _csPinMask = digitalPinToBitMask(_cs);
    _csPortSet = (PORTreg_t)&GPIO.out1_w1ts.val;
    _csPortClr = (PORTreg_t)&GPIO.out1_w1tc.val;
  }
  else if (_cs != GFX_NOT_DEFINED)
  {
    _csPinMask = digitalPinToBitMask(_cs);
    _csPortSet = (PORTreg_t)&GPIO.out_w1ts;
    _csPortClr = (PORTreg_t)&GPIO.out_w1tc;
  }
  _wrPinMask = digitalPinToBitMask(_wr);
  if (_wr >= 32)
  {
    _wrPortSet = (PORTreg_t)&GPIO.out1_w1ts.val;
    _wrPortClr = (PORTreg_t)&GPIO.out1_w1tc.val;
  }
  else
  {
    _wrPortSet = (PORTreg_t)&GPIO.out_w1ts;
    _wrPortClr = (PORTreg_t)&GPIO.out_w1tc;
  }
  if (_rd >= 32)
  {
    _rdPinMask = digitalPinToBitMask(_rd);
    _rdPortSet = (PORTreg_t)&GPIO.out1_w1ts.val;
    _rdPortClr = (PORTreg_t)&GPIO.out1_w1tc.val;
  }
  else if (_rd != GFX_NOT_DEFINED)
  {
    _rdPinMask = digitalPinToBitMask(_rd);
    _rdPortSet = (PORTreg_t)&GPIO.out_w1ts;
    _rdPortClr = (PORTreg_t)&GPIO.out_w1tc;
  }
  _d0PinMask = digitalPinToBitMask(_d0);
  if (_d0 >= 32)
  {
    _d0PortSet = (PORTreg_t)&GPIO.out1_w1ts.val;
    _d0PortClr = (PORTreg_t)&GPIO.out1_w1tc.val;
  }
  else
  {
    _d0PortSet = (PORTreg_t)&GPIO.out_w1ts;
    _d0PortClr = (PORTreg_t)&GPIO.out_w1tc;
  }
  _d1PinMask = digitalPinToBitMask(_d1);
  if (_d1 >= 32)
  {
    _d1PortSet = (PORTreg_t)&GPIO.out1_w1ts.val;
    _d1PortClr = (PORTreg_t)&GPIO.out1_w1tc.val;
  }
  else
  {
    _d1PortSet = (PORTreg_t)&GPIO.out_w1ts;
    _d1PortClr = (PORTreg_t)&GPIO.out_w1tc;
  }
  _d2PinMask = digitalPinToBitMask(_d2);
  if (_d2 >= 32)
  {
    _d2PortSet = (PORTreg_t)&GPIO.out1_w1ts.val;
    _d2PortClr = (PORTreg_t)&GPIO.out1_w1tc.val;
  }
  else
  {
    _d2PortSet = (PORTreg_t)&GPIO.out_w1ts;
    _d2PortClr = (PORTreg_t)&GPIO.out_w1tc;
  }
  _d3PinMask = digitalPinToBitMask(_d3);
  if (_d3 >= 32)
  {
    _d3PortSet = (PORTreg_t)&GPIO.out1_w1ts.val;
    _d3PortClr = (PORTreg_t)&GPIO.out1_w1tc.val;
  }
  else
  {
    _d3PortSet = (PORTreg_t)&GPIO.out_w1ts;
    _d3PortClr = (PORTreg_t)&GPIO.out_w1tc;
  }
  _d4PinMask = digitalPinToBitMask(_d4);
  if (_d4 >= 32)
  {
    _d4PortSet = (PORTreg_t)&GPIO.out1_w1ts.val;
    _d4PortClr = (PORTreg_t)&GPIO.out1_w1tc.val;
  }
  else
  {
    _d4PortSet = (PORTreg_t)&GPIO.out_w1ts;
    _d4PortClr = (PORTreg_t)&GPIO.out_w1tc;
  }
  _d5PinMask = digitalPinToBitMask(_d5);
  if (_d5 >= 32)
  {
    _d5PortSet = (PORTreg_t)&GPIO.out1_w1ts.val;
    _d5PortClr = (PORTreg_t)&GPIO.out1_w1tc.val;
  }
  else
  {
    _d5PortSet = (PORTreg_t)&GPIO.out_w1ts;
    _d5PortClr = (PORTreg_t)&GPIO.out_w1tc;
  }
  _d6PinMask = digitalPinToBitMask(_d6);
  if (_d6 >= 32)
  {
    _d6PortSet = (PORTreg_t)&GPIO.out1_w1ts.val;
    _d6PortClr = (PORTreg_t)&GPIO.out1_w1tc.val;
  }
  else
  {
    _d6PortSet = (PORTreg_t)&GPIO.out_w1ts;
    _d6PortClr = (PORTreg_t)&GPIO.out_w1tc;
  }
  _d7PinMask = digitalPinToBitMask(_d7);
  if (_d7 >= 32)
  {
    _d7PortSet = (PORTreg_t)&GPIO.out1_w1ts.val;
    _d7PortClr = (PORTreg_t)&GPIO.out1_w1tc.val;
  }
  else
  {
    _d7PortSet = (PORTreg_t)&GPIO.out_w1ts;
    _d7PortClr = (PORTreg_t)&GPIO.out_w1tc;
  }
#elif defined(CORE_TEENSY)
#if !defined(KINETISK)
  _dcPinMask = digitalPinToBitMask(_dc);
#endif
  _dcPortSet = portSetRegister(_dc);
  _dcPortClr = portClearRegister(_dc);
  if (_cs != GFX_NOT_DEFINED)
  {
#if !defined(KINETISK)
    _csPinMask = digitalPinToBitMask(_cs);
#endif
    _csPortSet = portSetRegister(_cs);
    _csPortClr = portClearRegister(_cs);
  }
#if !defined(KINETISK)
  _wrPinMask = digitalPinToBitMask(_wr);
#endif
  _wrPortSet = portSetRegister(_wr);
  _wrPortClr = portClearRegister(_wr);
  if (_rd != GFX_NOT_DEFINED)
  {
#if !defined(KINETISK)
    _rdPinMask = digitalPinToBitMask(_rd);
#endif
    _rdPortSet = portSetRegister(_rd);
    _rdPortClr = portClearRegister(_rd);
  }
#if !defined(KINETISK)
  _d0PinMask = digitalPinToBitMask(_d0);
#endif
  _d0PortSet = portSetRegister(_d0);
  _d0PortClr = portClearRegister(_d0);
#if !defined(KINETISK)
  _d1PinMask = digitalPinToBitMask(_d1);
#endif
  _d1PortSet = portSetRegister(_d1);
  _d1PortClr = portClearRegister(_d1);
#if !defined(KINETISK)
  _d2PinMask = digitalPinToBitMask(_d2);
#endif
  _d2PortSet = portSetRegister(_d2);
  _d2PortClr = portClearRegister(_d2);
#if !defined(KINETISK)
  _d3PinMask = digitalPinToBitMask(_d3);
#endif
  _d3PortSet = portSetRegister(_d3);
  _d3PortClr = portClearRegister(_d3);
#if !defined(KINETISK)
  _d4PinMask = digitalPinToBitMask(_d4);
#endif
  _d4PortSet = portSetRegister(_d4);
  _d4PortClr = portClearRegister(_d4);
#if !defined(KINETISK)
  _d5PinMask = digitalPinToBitMask(_d5);
#endif
  _d5PortSet = portSetRegister(_d5);
  _d5PortClr = portClearRegister(_d5);
#if !defined(KINETISK)
  _d6PinMask = digitalPinToBitMask(_d6);
#endif
  _d6PortSet = portSetRegister(_d6);
  _d6PortClr = portClearRegister(_d6);
#if !defined(KINETISK)
  _d7PinMask = digitalPinToBitMask(_d7);
#endif
  _d7PortSet = portSetRegister(_d7);
  _d7PortClr = portClearRegister(_d7);
#else  // !CORE_TEENSY
  _dcPinMask = digitalPinToBitMask(_dc);
  _dcPortSet = &(PORT->Group[g_APinDescription[_dc].ulPort].OUTSET.reg);
  _dcPortClr = &(PORT->Group[g_APinDescription[_dc].ulPort].OUTCLR.reg);
  if (_cs != GFX_NOT_DEFINED)
  {
    _csPinMask = digitalPinToBitMask(_cs);
    _csPortSet = &(PORT->Group[g_APinDescription[_cs].ulPort].OUTSET.reg);
    _csPortClr = &(PORT->Group[g_APinDescription[_cs].ulPort].OUTCLR.reg);
  }
  _wrPinMask = digitalPinToBitMask(_wr);
  _wrPortSet = &(PORT->Group[g_APinDescription[_wr].ulPort].OUTSET.reg);
  _wrPortClr = &(PORT->Group[g_APinDescription[_wr].ulPort].OUTCLR.reg);
  if (_rd != GFX_NOT_DEFINED)
  {
    _rdPinMask = digitalPinToBitMask(_rd);
    _rdPortSet = &(PORT->Group[g_APinDescription[_rd].ulPort].OUTSET.reg);
    _rdPortClr = &(PORT->Group[g_APinDescription[_rd].ulPort].OUTCLR.reg);
  }
  _d0PinMask = digitalPinToBitMask(_d0);
  _d0PortSet = &(PORT->Group[g_APinDescription[_d0].ulPort].OUTSET.reg);
  _d0PortClr = &(PORT->Group[g_APinDescription[_d0].ulPort].OUTCLR.reg);
  _d1PinMask = digitalPinToBitMask(_d1);
  _d1PortSet = &(PORT->Group[g_APinDescription[_d1].ulPort].OUTSET.reg);
  _d1PortClr = &(PORT->Group[g_APinDescription[_d1].ulPort].OUTCLR.reg);
  _d2PinMask = digitalPinToBitMask(_d2);
  _d2PortSet = &(PORT->Group[g_APinDescription[_d2].ulPort].OUTSET.reg);
  _d2PortClr = &(PORT->Group[g_APinDescription[_d2].ulPort].OUTCLR.reg);
  _d3PinMask = digitalPinToBitMask(_d3);
  _d3PortSet = &(PORT->Group[g_APinDescription[_d3].ulPort].OUTSET.reg);
  _d3PortClr = &(PORT->Group[g_APinDescription[_d3].ulPort].OUTCLR.reg);
  _d4PinMask = digitalPinToBitMask(_d4);
  _d4PortSet = &(PORT->Group[g_APinDescription[_d4].ulPort].OUTSET.reg);
  _d4PortClr = &(PORT->Group[g_APinDescription[_d4].ulPort].OUTCLR.reg);
  _d5PinMask = digitalPinToBitMask(_d5);
  _d5PortSet = &(PORT->Group[g_APinDescription[_d5].ulPort].OUTSET.reg);
  _d5PortClr = &(PORT->Group[g_APinDescription[_d5].ulPort].OUTCLR.reg);
  _d6PinMask = digitalPinToBitMask(_d6);
  _d6PortSet = &(PORT->Group[g_APinDescription[_d6].ulPort].OUTSET.reg);
  _d6PortClr = &(PORT->Group[g_APinDescription[_d6].ulPort].OUTCLR.reg);
  _d7PinMask = digitalPinToBitMask(_d7);
  _d7PortSet = &(PORT->Group[g_APinDescription[_d7].ulPort].OUTSET.reg);
  _d7PortClr = &(PORT->Group[g_APinDescription[_d7].ulPort].OUTCLR.reg);
#endif // end !CORE_TEENSY
#else  // !HAS_PORT_SET_CLR
  _dcPort = (PORTreg_t)portOutputRegister(digitalPinToPort(_dc));
  _dcPinMaskSet = digitalPinToBitMask(_dc);
  _dcPinMaskClr = ~_dcPinMaskSet;
  if (_cs != GFX_NOT_DEFINED)
  {
    _csPort = (PORTreg_t)portOutputRegister(digitalPinToPort(_cs));
    _csPinMaskSet = digitalPinToBitMask(_cs);
  _csPinMaskClr = ~_csPinMaskSet;
  }
  _wrPort = (PORTreg_t)portOutputRegister(digitalPinToPort(_wr));
  _wrPinMaskSet = digitalPinToBitMask(_wr);
  _wrPinMaskClr = ~_wrPinMaskSet;
  if (_rd != GFX_NOT_DEFINED)
  {
    _rdPort = (PORTreg_t)portOutputRegister(digitalPinToPort(_rd));
    _rdPinMaskSet = digitalPinToBitMask(_rd);
  _rdPinMaskClr = ~_rdPinMaskSet;
  }
  _d0Port = (PORTreg_t)portOutputRegister(digitalPinToPort(_d0));
  _d0PinMaskSet = digitalPinToBitMask(_d0);
  _d0PinMaskClr = ~_d0PinMaskSet;
  _d1Port = (PORTreg_t)portOutputRegister(digitalPinToPort(_d1));
  _d1PinMaskSet = digitalPinToBitMask(_d1);
  _d1PinMaskClr = ~_d1PinMaskSet;
  _d2Port = (PORTreg_t)portOutputRegister(digitalPinToPort(_d2));
  _d2PinMaskSet = digitalPinToBitMask(_d2);
  _d2PinMaskClr = ~_d2PinMaskSet;
  _d3Port = (PORTreg_t)portOutputRegister(digitalPinToPort(_d3));
  _d3PinMaskSet = digitalPinToBitMask(_d3);
  _d3PinMaskClr = ~_d3PinMaskSet;
  _d4Port = (PORTreg_t)portOutputRegister(digitalPinToPort(_d4));
  _d4PinMaskSet = digitalPinToBitMask(_d4);
  _d4PinMaskClr = ~_d4PinMaskSet;
  _d5Port = (PORTreg_t)portOutputRegister(digitalPinToPort(_d5));
  _d5PinMaskSet = digitalPinToBitMask(_d5);
  _d5PinMaskClr = ~_d5PinMaskSet;
  _d6Port = (PORTreg_t)portOutputRegister(digitalPinToPort(_d6));
  _d6PinMaskSet = digitalPinToBitMask(_d6);
  _d6PinMaskClr = ~_d6PinMaskSet;
  _d7Port = (PORTreg_t)portOutputRegister(digitalPinToPort(_d7));
  _d7PinMaskSet = digitalPinToBitMask(_d7);
  _d7PinMaskClr = ~_d7PinMaskSet;
#endif // !HAS_PORT_SET_CLR
#endif // USE_FAST_PINIO

  return true;
}

void Arduino_SWPAR8::beginWrite()
{
  DC_HIGH();
  CS_LOW();
}

void Arduino_SWPAR8::endWrite()
{
  CS_HIGH();
}

void Arduino_SWPAR8::writeCommand(uint8_t c)
{
  DC_LOW();

  WRITE(c);

  DC_HIGH();
}

void Arduino_SWPAR8::writeCommand16(uint16_t c)
{
  DC_LOW();

  _data16.value = c;
  WRITE(_data16.msb);
  WRITE(_data16.lsb);

  DC_HIGH();
}

void Arduino_SWPAR8::write(uint8_t d)
{
  WRITE(d);
}

void Arduino_SWPAR8::write16(uint16_t d)
{
  _data16.value = d;
  WRITE(_data16.msb);
  WRITE(_data16.lsb);
}

void Arduino_SWPAR8::writeRepeat(uint16_t p, uint32_t len)
{
#if defined(ESP8266)
  while (len > (ESP8266SAFEBATCHBITSIZE / 8))
  {
    WRITEREPEAT(p, ESP8266SAFEBATCHBITSIZE / 8);
    len -= ESP8266SAFEBATCHBITSIZE / 8;
    yield();
  }
  WRITEREPEAT(p, len);
#else
  WRITEREPEAT(p, len);
#endif
}

void Arduino_SWPAR8::writePixels(uint16_t *data, uint32_t len)
{
  while (len--)
  {
    _data16.value = *data++;
    WRITE(_data16.msb);
    WRITE(_data16.lsb);
  }
}

#if !defined(LITTLE_FOOT_PRINT)
void Arduino_SWPAR8::writeC8D8(uint8_t c, uint8_t d)
{
  DC_LOW();

  WRITE(c);

  DC_HIGH();

  WRITE(d);
}

void Arduino_SWPAR8::writeC8D16(uint8_t c, uint16_t d)
{
  DC_LOW();

  WRITE(c);

  DC_HIGH();

  _data16.value = d;
  WRITE(_data16.msb);
  WRITE(_data16.lsb);
}

void Arduino_SWPAR8::writeC8D16D16(uint8_t c, uint16_t d1, uint16_t d2)
{
  DC_LOW();

  WRITE(c);

  DC_HIGH();

  _data16.value = d1;
  WRITE(_data16.msb);
  WRITE(_data16.lsb);

  _data16.value = d2;
  WRITE(_data16.msb);
  WRITE(_data16.lsb);
}

void Arduino_SWPAR8::writeBytes(uint8_t *data, uint32_t len)
{
  while (len--)
  {
    WRITE(*data++);
  }
}

void Arduino_SWPAR8::writeIndexedPixels(uint8_t *data, uint16_t *idx, uint32_t len)
{
  while (len--)
  {
    _data16.value = idx[*data++];
    WRITE(_data16.msb);
    WRITE(_data16.lsb);
  }
}

void Arduino_SWPAR8::writeIndexedPixelsDouble(uint8_t *data, uint16_t *idx, uint32_t len)
{
  while (len--)
  {
    _data16.value = idx[*data++];
    WRITE(_data16.msb);
    WRITE(_data16.lsb);
    WRITE(_data16.msb);
    WRITE(_data16.lsb);
  }
}
#endif // !defined(LITTLE_FOOT_PRINT)

void Arduino_SWPAR8::WRITE(uint8_t d)
{
  if (d & 0x01)
  {
    D0_HIGH();
  }
  else
  {
    D0_LOW();
  }
  if (d & 0x02)
  {
    D1_HIGH();
  }
  else
  {
    D1_LOW();
  }
  if (d & 0x04)
  {
    D2_HIGH();
  }
  else
  {
    D2_LOW();
  }
  if (d & 0x08)
  {
    D3_HIGH();
  }
  else
  {
    D3_LOW();
  }
  if (d & 0x10)
  {
    D4_HIGH();
  }
  else
  {
    D4_LOW();
  }
  if (d & 0x20)
  {
    D5_HIGH();
  }
  else
  {
    D5_LOW();
  }
  if (d & 0x40)
  {
    D6_HIGH();
  }
  else
  {
    D6_LOW();
  }
  if (d & 0x80)
  {
    D7_HIGH();
  }
  else
  {
    D7_LOW();
  }
  WR_LOW();
  WR_HIGH();
}

void Arduino_SWPAR8::WRITEREPEAT(uint16_t p, uint32_t len)
{
  _data16.value = p;
  if (_data16.msb == _data16.lsb)
  {
    WRITE(_data16.msb);
    len <<= 1;
    while (--len)
    {
      WR_LOW();
      WR_HIGH();
    }
  }
  else
  {
    while (len--)
    {
      WRITE(_data16.msb);
      WRITE(_data16.lsb);
    }
  }
}

/******** low level bit twiddling **********/

INLINE void Arduino_SWPAR8::DC_HIGH(void)
{
#if defined(USE_FAST_PINIO)
#if defined(HAS_PORT_SET_CLR)
#if defined(KINETISK)
  *_dcPortSet = 1;
#else  // !KINETISK
  *_dcPortSet = _dcPinMask;
#endif // end !KINETISK
#else  // !HAS_PORT_SET_CLR
  *_dcPort |= _dcPinMaskSet;
#endif // end !HAS_PORT_SET_CLR
#else  // !USE_FAST_PINIO
  digitalWrite(_dc, HIGH);
#endif // end !USE_FAST_PINIO
}

INLINE void Arduino_SWPAR8::DC_LOW(void)
{
#if defined(USE_FAST_PINIO)
#if defined(HAS_PORT_SET_CLR)
#if defined(KINETISK)
  *_dcPortClr = 1;
#else  // !KINETISK
  *_dcPortClr = _dcPinMask;
#endif // end !KINETISK
#else  // !HAS_PORT_SET_CLR
  *_dcPort &= _dcPinMaskClr;
#endif // end !HAS_PORT_SET_CLR
#else  // !USE_FAST_PINIO
  digitalWrite(_dc, LOW);
#endif // end !USE_FAST_PINIO
}

INLINE void Arduino_SWPAR8::CS_HIGH(void)
{
  if (_cs != GFX_NOT_DEFINED)
  {
#if defined(USE_FAST_PINIO)
#if defined(HAS_PORT_SET_CLR)
#if defined(KINETISK)
    *_csPortSet = 1;
#else  // !KINETISK
    *_csPortSet = _csPinMask;
#endif // end !KINETISK
#else  // !HAS_PORT_SET_CLR
    *_csPort |= _csPinMaskSet;
#endif // end !HAS_PORT_SET_CLR
#else  // !USE_FAST_PINIO
    digitalWrite(_cs, HIGH);
#endif // end !USE_FAST_PINIO
  }
}

INLINE void Arduino_SWPAR8::CS_LOW(void)
{
  if (_cs != GFX_NOT_DEFINED)
  {
#if defined(USE_FAST_PINIO)
#if defined(HAS_PORT_SET_CLR)
#if defined(KINETISK)
    *_csPortClr = 1;
#else  // !KINETISK
    *_csPortClr = _csPinMask;
#endif // end !KINETISK
#else  // !HAS_PORT_SET_CLR
    *_csPort &= _csPinMaskClr;
#endif // end !HAS_PORT_SET_CLR
#else  // !USE_FAST_PINIO
    digitalWrite(_cs, LOW);
#endif // end !USE_FAST_PINIO
  }
}

INLINE void Arduino_SWPAR8::WR_HIGH(void)
{
#if defined(USE_FAST_PINIO)
#if defined(HAS_PORT_SET_CLR)
#if defined(KINETISK)
  *_wrPortSet = 1;
#else  // !KINETISK
  *_wrPortSet = _wrPinMask;
#endif // end !KINETISK
#else  // !HAS_PORT_SET_CLR
  *_wrPort |= _wrPinMaskSet;
#endif // end !HAS_PORT_SET_CLR
#else  // !USE_FAST_PINIO
  digitalWrite(_wr, HIGH);
#endif // end !USE_FAST_PINIO
}

INLINE void Arduino_SWPAR8::WR_LOW(void)
{
#if defined(USE_FAST_PINIO)
#if defined(HAS_PORT_SET_CLR)
#if defined(KINETISK)
  *_wrPortClr = 1;
#else  // !KINETISK
  *_wrPortClr = _wrPinMask;
#endif // end !KINETISK
#else  // !HAS_PORT_SET_CLR
  *_wrPort &= _wrPinMaskClr;
#endif // end !HAS_PORT_SET_CLR
#else  // !USE_FAST_PINIO
  digitalWrite(_wr, LOW);
#endif // end !USE_FAST_PINIO
}

INLINE void Arduino_SWPAR8::RD_HIGH(void)
{
  if (_rd != GFX_NOT_DEFINED)
  {
#if defined(USE_FAST_PINIO)
#if defined(HAS_PORT_SET_CLR)
#if defined(KINETISK)
    *_rdPortSet = 1;
#else  // !KINETISK
    *_rdPortSet = _rdPinMask;
#endif // end !KINETISK
#else  // !HAS_PORT_SET_CLR
    *_rdPort |= _rdPinMaskSet;
#endif // end !HAS_PORT_SET_CLR
#else  // !USE_FAST_PINIO
    digitalWrite(_rd, HIGH);
#endif // end !USE_FAST_PINIO
  }
}

INLINE void Arduino_SWPAR8::RD_LOW(void)
{
  if (_rd != GFX_NOT_DEFINED)
  {
#if defined(USE_FAST_PINIO)
#if defined(HAS_PORT_SET_CLR)
#if defined(KINETISK)
    *_rdPortClr = 1;
#else  // !KINETISK
    *_rdPortClr = _rdPinMask;
#endif // end !KINETISK
#else  // !HAS_PORT_SET_CLR
    *_rdPort &= _rdPinMaskClr;
#endif // end !HAS_PORT_SET_CLR
#else  // !USE_FAST_PINIO
    digitalWrite(_rd, LOW);
#endif // end !USE_FAST_PINIO
  }
}

INLINE void Arduino_SWPAR8::D0_HIGH(void)
{
#if defined(USE_FAST_PINIO)
#if defined(HAS_PORT_SET_CLR)
#if defined(KINETISK)
  *_d0PortSet = 1;
#else  // !KINETISK
  *_d0PortSet = _d0PinMask;
#endif // end !KINETISK
#else  // !HAS_PORT_SET_CLR
  *_d0Port |= _d0PinMaskSet;
#endif // end !HAS_PORT_SET_CLR
#else  // !USE_FAST_PINIO
  digitalWrite(_d0, HIGH);
#endif // end !USE_FAST_PINIO
}

INLINE void Arduino_SWPAR8::D0_LOW(void)
{
#if defined(USE_FAST_PINIO)
#if defined(HAS_PORT_SET_CLR)
#if defined(KINETISK)
  *_d0PortClr = 1;
#else  // !KINETISK
  *_d0PortClr = _d0PinMask;
#endif // end !KINETISK
#else  // !HAS_PORT_SET_CLR
  *_d0Port &= _d0PinMaskClr;
#endif // end !HAS_PORT_SET_CLR
#else  // !USE_FAST_PINIO
  digitalWrite(_d0, LOW);
#endif // end !USE_FAST_PINIO
}

INLINE void Arduino_SWPAR8::D1_HIGH(void)
{
#if defined(USE_FAST_PINIO)
#if defined(HAS_PORT_SET_CLR)
#if defined(KINETISK)
  *_d1PortSet = 1;
#else  // !KINETISK
  *_d1PortSet = _d1PinMask;
#endif // end !KINETISK
#else  // !HAS_PORT_SET_CLR
  *_d1Port |= _d1PinMaskSet;
#endif // end !HAS_PORT_SET_CLR
#else  // !USE_FAST_PINIO
  digitalWrite(_d1, HIGH);
#endif // end !USE_FAST_PINIO
}

INLINE void Arduino_SWPAR8::D1_LOW(void)
{
#if defined(USE_FAST_PINIO)
#if defined(HAS_PORT_SET_CLR)
#if defined(KINETISK)
  *_d1PortClr = 1;
#else  // !KINETISK
  *_d1PortClr = _d1PinMask;
#endif // end !KINETISK
#else  // !HAS_PORT_SET_CLR
  *_d1Port &= _d1PinMaskClr;
#endif // end !HAS_PORT_SET_CLR
#else  // !USE_FAST_PINIO
  digitalWrite(_d1, LOW);
#endif // end !USE_FAST_PINIO
}

INLINE void Arduino_SWPAR8::D2_HIGH(void)
{
#if defined(USE_FAST_PINIO)
#if defined(HAS_PORT_SET_CLR)
#if defined(KINETISK)
  *_d2PortSet = 1;
#else  // !KINETISK
  *_d2PortSet = _d2PinMask;
#endif // end !KINETISK
#else  // !HAS_PORT_SET_CLR
  *_d2Port |= _d2PinMaskSet;
#endif // end !HAS_PORT_SET_CLR
#else  // !USE_FAST_PINIO
  digitalWrite(_d2, HIGH);
#endif // end !USE_FAST_PINIO
}

INLINE void Arduino_SWPAR8::D2_LOW(void)
{
#if defined(USE_FAST_PINIO)
#if defined(HAS_PORT_SET_CLR)
#if defined(KINETISK)
  *_d2PortClr = 1;
#else  // !KINETISK
  *_d2PortClr = _d2PinMask;
#endif // end !KINETISK
#else  // !HAS_PORT_SET_CLR
  *_d2Port &= _d2PinMaskClr;
#endif // end !HAS_PORT_SET_CLR
#else  // !USE_FAST_PINIO
  digitalWrite(_d2, LOW);
#endif // end !USE_FAST_PINIO
}

INLINE void Arduino_SWPAR8::D3_HIGH(void)
{
#if defined(USE_FAST_PINIO)
#if defined(HAS_PORT_SET_CLR)
#if defined(KINETISK)
  *_d3PortSet = 1;
#else  // !KINETISK
  *_d3PortSet = _d3PinMask;
#endif // end !KINETISK
#else  // !HAS_PORT_SET_CLR
  *_d3Port |= _d3PinMaskSet;
#endif // end !HAS_PORT_SET_CLR
#else  // !USE_FAST_PINIO
  digitalWrite(_d3, HIGH);
#endif // end !USE_FAST_PINIO
}

INLINE void Arduino_SWPAR8::D3_LOW(void)
{
#if defined(USE_FAST_PINIO)
#if defined(HAS_PORT_SET_CLR)
#if defined(KINETISK)
  *_d3PortClr = 1;
#else  // !KINETISK
  *_d3PortClr = _d3PinMask;
#endif // end !KINETISK
#else  // !HAS_PORT_SET_CLR
  *_d3Port &= _d3PinMaskClr;
#endif // end !HAS_PORT_SET_CLR
#else  // !USE_FAST_PINIO
  digitalWrite(_d3, LOW);
#endif // end !USE_FAST_PINIO
}

INLINE void Arduino_SWPAR8::D4_HIGH(void)
{
#if defined(USE_FAST_PINIO)
#if defined(HAS_PORT_SET_CLR)
#if defined(KINETISK)
  *_d4PortSet = 1;
#else  // !KINETISK
  *_d4PortSet = _d4PinMask;
#endif // end !KINETISK
#else  // !HAS_PORT_SET_CLR
  *_d4Port |= _d4PinMaskSet;
#endif // end !HAS_PORT_SET_CLR
#else  // !USE_FAST_PINIO
  digitalWrite(_d4, HIGH);
#endif // end !USE_FAST_PINIO
}

INLINE void Arduino_SWPAR8::D4_LOW(void)
{
#if defined(USE_FAST_PINIO)
#if defined(HAS_PORT_SET_CLR)
#if defined(KINETISK)
  *_d4PortClr = 1;
#else  // !KINETISK
  *_d4PortClr = _d4PinMask;
#endif // end !KINETISK
#else  // !HAS_PORT_SET_CLR
  *_d4Port &= _d4PinMaskClr;
#endif // end !HAS_PORT_SET_CLR
#else  // !USE_FAST_PINIO
  digitalWrite(_d4, LOW);
#endif // end !USE_FAST_PINIO
}

INLINE void Arduino_SWPAR8::D5_HIGH(void)
{
#if defined(USE_FAST_PINIO)
#if defined(HAS_PORT_SET_CLR)
#if defined(KINETISK)
  *_d5PortSet = 1;
#else  // !KINETISK
  *_d5PortSet = _d5PinMask;
#endif // end !KINETISK
#else  // !HAS_PORT_SET_CLR
  *_d5Port |= _d5PinMaskSet;
#endif // end !HAS_PORT_SET_CLR
#else  // !USE_FAST_PINIO
  digitalWrite(_d5, HIGH);
#endif // end !USE_FAST_PINIO
}

INLINE void Arduino_SWPAR8::D5_LOW(void)
{
#if defined(USE_FAST_PINIO)
#if defined(HAS_PORT_SET_CLR)
#if defined(KINETISK)
  *_d5PortClr = 1;
#else  // !KINETISK
  *_d5PortClr = _d5PinMask;
#endif // end !KINETISK
#else  // !HAS_PORT_SET_CLR
  *_d5Port &= _d5PinMaskClr;
#endif // end !HAS_PORT_SET_CLR
#else  // !USE_FAST_PINIO
  digitalWrite(_d5, LOW);
#endif // end !USE_FAST_PINIO
}

INLINE void Arduino_SWPAR8::D6_HIGH(void)
{
#if defined(USE_FAST_PINIO)
#if defined(HAS_PORT_SET_CLR)
#if defined(KINETISK)
  *_d6PortSet = 1;
#else  // !KINETISK
  *_d6PortSet = _d6PinMask;
#endif // end !KINETISK
#else  // !HAS_PORT_SET_CLR
  *_d6Port |= _d6PinMaskSet;
#endif // end !HAS_PORT_SET_CLR
#else  // !USE_FAST_PINIO
  digitalWrite(_d6, HIGH);
#endif // end !USE_FAST_PINIO
}

INLINE void Arduino_SWPAR8::D6_LOW(void)
{
#if defined(USE_FAST_PINIO)
#if defined(HAS_PORT_SET_CLR)
#if defined(KINETISK)
  *_d6PortClr = 1;
#else  // !KINETISK
  *_d6PortClr = _d6PinMask;
#endif // end !KINETISK
#else  // !HAS_PORT_SET_CLR
  *_d6Port &= _d6PinMaskClr;
#endif // end !HAS_PORT_SET_CLR
#else  // !USE_FAST_PINIO
  digitalWrite(_d6, LOW);
#endif // end !USE_FAST_PINIO
}

INLINE void Arduino_SWPAR8::D7_HIGH(void)
{
#if defined(USE_FAST_PINIO)
#if defined(HAS_PORT_SET_CLR)
#if defined(KINETISK)
  *_d7PortSet = 1;
#else  // !KINETISK
  *_d7PortSet = _d7PinMask;
#endif // end !KINETISK
#else  // !HAS_PORT_SET_CLR
  *_d7Port |= _d7PinMaskSet;
#endif // end !HAS_PORT_SET_CLR
#else  // !USE_FAST_PINIO
  digitalWrite(_d7, HIGH);
#endif // end !USE_FAST_PINIO
}

INLINE void Arduino_SWPAR8::D7_LOW(void)
{
#if defined(USE_FAST_PINIO)
#if defined(HAS_PORT_SET_CLR)
#if defined(KINETISK)
  *_d7PortClr = 1;
#else  // !KINETISK
  *_d7PortClr = _d7PinMask;
#endif // end !KINETISK
#else  // !HAS_PORT_SET_CLR
  *_d7Port &= _d7PinMaskClr;
#endif // end !HAS_PORT_SET_CLR
#else  // !USE_FAST_PINIO
  digitalWrite(_d7, LOW);
#endif // end !USE_FAST_PINIO
}
