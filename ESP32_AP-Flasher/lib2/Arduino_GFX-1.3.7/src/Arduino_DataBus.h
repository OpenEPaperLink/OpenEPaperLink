/*
 * start rewrite from:
 * https://github.com/adafruit/Adafruit-GFX-Library.git
 */
#ifndef _ARDUINO_DATABUS_H_
#define _ARDUINO_DATABUS_H_

#include <Arduino.h>

#define GFX_SKIP_OUTPUT_BEGIN -2
#define GFX_NOT_DEFINED -1
#define GFX_STR_HELPER(x) #x
#define GFX_STR(x) GFX_STR_HELPER(x)

#if defined(__AVR__)
#define LITTLE_FOOT_PRINT // reduce program size for limited flash MCU
#define USE_FAST_PINIO    ///< Use direct PORT register access
typedef uint8_t ARDUINOGFX_PORT_t;
#elif defined(ARDUINO_ARCH_NRF52840)
#define USE_FAST_PINIO   ///< Use direct PORT register access
#define HAS_PORT_SET_CLR ///< PORTs have set & clear registers
typedef uint32_t ARDUINOGFX_PORT_t;
#elif defined(TARGET_RP2040)
#define USE_FAST_PINIO   ///< Use direct PORT register access
#define HAS_PORT_SET_CLR ///< PORTs have set & clear registers
typedef uint32_t ARDUINOGFX_PORT_t;
#elif defined(ESP32)
#define USE_FAST_PINIO   ///< Use direct PORT register access
#define HAS_PORT_SET_CLR ///< PORTs have set & clear registers
typedef uint32_t ARDUINOGFX_PORT_t;
#elif defined(ESP8266)
#define ESP8266SAFEBATCHBITSIZE (2048 * 8 * 9)
#define USE_FAST_PINIO ///< Use direct PORT register access
typedef uint32_t ARDUINOGFX_PORT_t;
#elif defined(ARDUINO_ARCH_STM32)
#define USE_FAST_PINIO ///< Use direct PORT register access
typedef uint32_t ARDUINOGFX_PORT_t;
#elif defined(__arm__)
#if defined(ARDUINO_ARCH_SAMD)
// Adafruit M0, M4
#define USE_FAST_PINIO                  ///< Use direct PORT register access
#define HAS_PORT_SET_CLR                ///< PORTs have set & clear registers
typedef uint32_t ARDUINOGFX_PORT_t;
#elif defined(CONFIG_ARCH_CHIP_CXD56XX) // Sony Spresense
#define USE_FAST_PINIO                  ///< Use direct PORT register access
typedef uint8_t ARDUINOGFX_PORT_t;
#elif defined(RTL8722DM)
#define USE_FAST_PINIO ///< Use direct PORT register access
typedef uint32_t ARDUINOGFX_PORT_t;
#elif defined(CORE_TEENSY)
#define USE_FAST_PINIO   ///< Use direct PORT register access
#define HAS_PORT_SET_CLR ///< PORTs have set & clear registers
#if defined(__IMXRT1052__) || defined(__IMXRT1062__)
// PJRC Teensy 4.x
typedef uint32_t ARDUINOGFX_PORT_t;
#else
// PJRC Teensy 3.x
typedef uint8_t ARDUINOGFX_PORT_t;
#endif
#else
// Arduino Due?
// USE_FAST_PINIO not available here (yet)...Due has a totally different
// GPIO register set and will require some changes elsewhere (e.g. in
// constructors especially).
#endif
#else  // !ARM
// Unknow architecture, USE_FAST_PINIO is not available here (yet)
// but don't worry about it too much...the digitalWrite() implementation
// on these platforms is reasonably efficient and already RAM-resident,
// only gotcha then is no parallel connection support for now.
#endif // !ARM

#ifdef USE_FAST_PINIO
typedef volatile ARDUINOGFX_PORT_t *PORTreg_t;
#endif

#if defined(ARDUINO_ARCH_ARC32) || defined(ARDUINO_MAXIM)
#define SPI_DEFAULT_FREQ 16000000
// Teensy 3.0, 3.1/3.2, 3.5, 3.6
#elif defined(__MK20DX128__) || defined(__MK20DX256__) || defined(__MK64FX512__) || defined(__MK66FX1M0__)
#define SPI_DEFAULT_FREQ 40000000
// Teensy 4.x
#elif defined(__IMXRT1052__) || defined(__IMXRT1062__)
#define SPI_DEFAULT_FREQ 40000000
#elif defined(__AVR__) || defined(TEENSYDUINO)
#define SPI_DEFAULT_FREQ 8000000
#elif defined(ARDUINO_ARCH_NRF52840)
#define SPI_DEFAULT_FREQ 8000000
#elif defined(ESP8266) || defined(ESP32)
#define SPI_DEFAULT_FREQ 40000000
#elif defined(RTL8722DM)
#define SPI_DEFAULT_FREQ 20000000
#elif defined(RASPI)
#define SPI_DEFAULT_FREQ 80000000
#elif defined(ARDUINO_ARCH_STM32F1)
#define SPI_DEFAULT_FREQ 36000000
#elif defined(ARDUINO_BLACKPILL_F411CE)
#define SPI_DEFAULT_FREQ 50000000
#elif defined(F_CPU)
#define SPI_DEFAULT_FREQ (F_CPU / 4)
#else
#define SPI_DEFAULT_FREQ 24000000 ///< Default SPI data clock frequency
#endif

#ifndef UNUSED
#define UNUSED(x) (void)(x)
#endif
#define ATTR_UNUSED __attribute__((unused))

#define MSB_16(val) (((val)&0xFF00) >> 8) | (((val)&0xFF) << 8)
#define MSB_16_SET(var, val) \
  {                          \
    (var) = MSB_16(val);     \
  }
#define MSB_32_SET(var, val)                                  \
  {                                                           \
    uint8_t *v = (uint8_t *)&(val);                           \
    (var) = v[3] | (v[2] << 8) | (v[1] << 16) | (v[0] << 24); \
  }
#define MSB_32_16_16_SET(var, v1, v2)                                                                                   \
  {                                                                                                                     \
    (var) = (((uint32_t)v2 & 0xff00) << 8) | (((uint32_t)v2 & 0xff) << 24) | ((v1 & 0xff00) >> 8) | ((v1 & 0xff) << 8); \
  }
#define MSB_32_8_ARRAY_SET(var, a)                                  \
  {                                                                 \
    (var) = ((uint32_t)a[0] << 8 | a[1] | a[2] << 24 | a[3] << 16); \
  }

#if defined(ESP32)
#define INLINE __attribute__((always_inline)) inline
#else
#define INLINE inline
#endif

#if defined(ESP32) && (CONFIG_IDF_TARGET_ESP32S3)
#include <esp_lcd_panel_io.h>
#include <esp_lcd_panel_io_interface.h>
#include <esp_pm.h>
#include <esp_private/gdma.h>
#include <hal/dma_types.h>
#include <hal/lcd_hal.h>
#include <soc/dport_reg.h>
#include <soc/gpio_sig_map.h>
#include <soc/lcd_cam_reg.h>
#include <soc/lcd_cam_struct.h>

typedef struct esp_lcd_i80_bus_t esp_lcd_i80_bus_t;
typedef struct lcd_panel_io_i80_t lcd_panel_io_i80_t;
typedef struct lcd_i80_trans_descriptor_t lcd_i80_trans_descriptor_t;

struct esp_lcd_i80_bus_t
{
  int bus_id;                            // Bus ID, index from 0
  portMUX_TYPE spinlock;                 // spinlock used to protect i80 bus members(hal, device_list, cur_trans)
  lcd_hal_context_t hal;                 // Hal object
  size_t bus_width;                      // Number of data lines
  intr_handle_t intr;                    // LCD peripheral interrupt handle
  esp_pm_lock_handle_t pm_lock;          // Power management lock
  size_t num_dma_nodes;                  // Number of DMA descriptors
  uint8_t *format_buffer;                // The driver allocates an internal buffer for DMA to do data format transformer
  size_t resolution_hz;                  // LCD_CLK resolution, determined by selected clock source
  gdma_channel_handle_t dma_chan;        // DMA channel handle
  size_t psram_trans_align;              // DMA transfer alignment for data allocated from PSRAM
  size_t sram_trans_align;               // DMA transfer alignment for data allocated from SRAM
  lcd_i80_trans_descriptor_t *cur_trans; // Current transaction
  lcd_panel_io_i80_t *cur_device;        // Current working device
  LIST_HEAD(i80_device_list, lcd_panel_io_i80_t)
  device_list; // Head of i80 device list
  struct
  {
    unsigned int exclusive : 1; // Indicate whether the I80 bus is owned by one device (whose CS GPIO is not assigned) exclusively
  } flags;
  dma_descriptor_t dma_nodes[]; // DMA descriptor pool, the descriptors are shared by all i80 devices
};

struct lcd_i80_trans_descriptor_t
{
  lcd_panel_io_i80_t *i80_device;                       // i80 device issuing this transaction
  int cmd_value;                                        // Command value
  uint32_t cmd_cycles;                                  // Command cycles
  const void *data;                                     // Data buffer
  uint32_t data_length;                                 // Data buffer size
  void *user_ctx;                                       // private data used by trans_done_cb
  esp_lcd_panel_io_color_trans_done_cb_t trans_done_cb; // transaction done callback
};

struct lcd_panel_io_i80_t
{
  esp_lcd_panel_io_t base;                                    // Base class of generic lcd panel io
  esp_lcd_i80_bus_t *bus;                                     // Which bus the device is attached to
  int cs_gpio_num;                                            // GPIO used for CS line
  unsigned int pclk_hz;                                       // PCLK clock frequency
  size_t clock_prescale;                                      // Prescaler coefficient, determined by user's configured PCLK frequency
  QueueHandle_t trans_queue;                                  // Transaction queue, transactions in this queue are pending for scheduler to dispatch
  QueueHandle_t done_queue;                                   // Transaction done queue, transactions in this queue are finished but not recycled by the caller
  size_t queue_size;                                          // Size of transaction queue
  size_t num_trans_inflight;                                  // Number of transactions that are undergoing (the descriptor not recycled yet)
  int lcd_cmd_bits;                                           // Bit width of LCD command
  int lcd_param_bits;                                         // Bit width of LCD parameter
  void *user_ctx;                                             // private data used when transfer color data
  esp_lcd_panel_io_color_trans_done_cb_t on_color_trans_done; // color data trans done callback
  LIST_ENTRY(lcd_panel_io_i80_t)
  device_list_entry; // Entry of i80 device list
  struct
  {
    unsigned int dc_idle_level : 1;  // Level of DC line in IDLE phase
    unsigned int dc_cmd_level : 1;   // Level of DC line in CMD phase
    unsigned int dc_dummy_level : 1; // Level of DC line in DUMMY phase
    unsigned int dc_data_level : 1;  // Level of DC line in DATA phase
  } dc_levels;
  struct
  {
    unsigned int cs_active_high : 1;     // Whether the CS line is active on high level
    unsigned int reverse_color_bits : 1; // Reverse the data bits, D[N:0] -> D[0:N]
    unsigned int swap_color_bytes : 1;   // Swap adjacent two data bytes before sending out
    unsigned int pclk_active_neg : 1;    // The display will write data lines when there's a falling edge on WR line
    unsigned int pclk_idle_low : 1;      // The WR line keeps at low level in IDLE phase
  } flags;
  lcd_i80_trans_descriptor_t trans_pool[]; // Transaction pool
};
#endif // #if defined(ESP32) && (CONFIG_IDF_TARGET_ESP32S3)

typedef enum
{
  BEGIN_WRITE,
  WRITE_COMMAND_8,
  WRITE_COMMAND_16,
  WRITE_DATA_8,
  WRITE_DATA_16,
  WRITE_BYTES,
  WRITE_C8_D8,
  WRITE_C8_D16,
  WRITE_C16_D16,
  END_WRITE,
  DELAY,
} spi_operation_type_t;

union
{
  uint16_t value;
  struct
  {
    uint8_t lsb;
    uint8_t msb;
  };
} _data16;

class Arduino_DataBus
{
public:
  Arduino_DataBus();

  void unused() { UNUSED(_data16); } // avoid compiler warning

  virtual bool begin(int32_t speed = SPI_DEFAULT_FREQ, int8_t dataMode = GFX_NOT_DEFINED) = 0;
  virtual void beginWrite() = 0;
  virtual void endWrite() = 0;
  virtual void writeCommand(uint8_t c) = 0;
  virtual void writeCommand16(uint16_t c) = 0;
  virtual void write(uint8_t) = 0;
  virtual void write16(uint16_t) = 0;
  virtual void writeC8D8(uint8_t c, uint8_t d);
  virtual void writeC16D16(uint16_t c, uint16_t d);
  virtual void writeC8D16(uint8_t c, uint16_t d);
  virtual void writeC8D16D16(uint8_t c, uint16_t d1, uint16_t d2);
  virtual void writeC8D16D16Split(uint8_t c, uint16_t d1, uint16_t d2);
  virtual void writeRepeat(uint16_t p, uint32_t len) = 0;
  virtual void writePixels(uint16_t *data, uint32_t len) = 0;

  void sendCommand(uint8_t c);
  void sendCommand16(uint16_t c);
  void sendData(uint8_t d);
  void sendData16(uint16_t d);

  void batchOperation(const uint8_t *operations, size_t len);

#if !defined(LITTLE_FOOT_PRINT)
  virtual void writeBytes(uint8_t *data, uint32_t len) = 0;
  virtual void writePattern(uint8_t *data, uint8_t len, uint32_t repeat);
  virtual void writeIndexedPixels(uint8_t *data, uint16_t *idx, uint32_t len);
  virtual void writeIndexedPixelsDouble(uint8_t *data, uint16_t *idx, uint32_t len);
#endif // !defined(LITTLE_FOOT_PRINT)

  virtual void digitalWrite(uint8_t pin, uint8_t val);
  virtual int digitalRead(uint8_t pin);

protected:
  int32_t _speed;
  int8_t _dataMode;
};

#endif // _ARDUINO_DATABUS_H_
