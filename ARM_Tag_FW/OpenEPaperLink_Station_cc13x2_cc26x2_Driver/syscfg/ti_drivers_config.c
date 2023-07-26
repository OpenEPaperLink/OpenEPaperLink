/*
 *  ======== ti_drivers_config.c ========
 *  Configured TI-Drivers module definitions
 *
 *  DO NOT EDIT - This file is generated for the LP_CC2652PSIP
 *  by the SysConfig tool.
 */

#include <stddef.h>
#include <stdint.h>

#ifndef DeviceFamily_CC26X2
#define DeviceFamily_CC26X2
#endif

#include <ti/devices/DeviceFamily.h>

#include "ti_drivers_config.h"

/*
 *  =============================== DMA ===============================
 */

#include <ti/drivers/dma/UDMACC26XX.h>
#include <ti/devices/cc13x2_cc26x2/driverlib/udma.h>
#include <ti/devices/cc13x2_cc26x2/inc/hw_memmap.h>

UDMACC26XX_Object udmaCC26XXObject;

const UDMACC26XX_HWAttrs udmaCC26XXHWAttrs = {
    .baseAddr        = UDMA0_BASE,
    .powerMngrId     = PowerCC26XX_PERIPH_UDMA,
    .intNum          = INT_DMA_ERR,
    .intPriority     = (~0)
};

const UDMACC26XX_Config UDMACC26XX_config[1] = {
    {
        .object         = &udmaCC26XXObject,
        .hwAttrs        = &udmaCC26XXHWAttrs,
    },
};

/*
 *  =============================== GPIO ===============================
 */

#include <ti/drivers/GPIO.h>
#include <ti/drivers/gpio/GPIOCC26XX.h>

/* The range of pins available on this device */
const uint_least8_t GPIO_pinLowerBound = 0;
const uint_least8_t GPIO_pinUpperBound = 31;

/*
 *  ======== gpioPinConfigs ========
 *  Array of Pin configurations
 */
GPIO_PinConfig gpioPinConfigs[32] = {
    /* Owned by /ti/drivers/RF as RF Antenna Pin 0 */
    GPIO_CFG_OUTPUT_INTERNAL | GPIO_CFG_OUT_STR_HIGH | GPIO_CFG_OUT_LOW, /* CONFIG_RF_STANDARD_PA */
    GPIO_CFG_NO_DIR, /* DIO_1 */
    GPIO_CFG_NO_DIR, /* DIO_2 */
    GPIO_CFG_NO_DIR, /* DIO_3 */
    GPIO_CFG_NO_DIR, /* DIO_4 */
    GPIO_CFG_NO_DIR, /* DIO_5 */
    GPIO_CFG_NO_DIR, /* DIO_6 */
    GPIO_CFG_NO_DIR, /* DIO_7 */
    GPIO_CFG_NO_DIR, /* DIO_8 */
    GPIO_CFG_NO_DIR, /* DIO_9 */
    GPIO_CFG_NO_DIR, /* DIO_10 */
    GPIO_CFG_NO_DIR, /* DIO_11 */
    /* Owned by CONFIG_UART2_0 as RX */
    GPIO_CFG_INPUT_INTERNAL | GPIO_CFG_IN_INT_NONE | GPIO_CFG_PULL_DOWN_INTERNAL, /* CONFIG_GPIO_UART2_0_RX */
    /* Owned by CONFIG_UART2_0 as TX */
    GPIO_CFG_OUTPUT_INTERNAL | GPIO_CFG_OUT_STR_MED | GPIO_CFG_OUT_HIGH, /* CONFIG_GPIO_UART2_0_TX */
    GPIO_CFG_NO_DIR, /* DIO_14 */
    GPIO_CFG_NO_DIR, /* DIO_15 */
    GPIO_CFG_NO_DIR, /* DIO_16 */
    GPIO_CFG_NO_DIR, /* DIO_17 */
    GPIO_CFG_NO_DIR, /* DIO_18 */
    GPIO_CFG_NO_DIR, /* DIO_19 */
    GPIO_CFG_NO_DIR, /* DIO_20 */
    GPIO_CFG_NO_DIR, /* DIO_21 */
    GPIO_CFG_NO_DIR, /* DIO_22 */
    GPIO_CFG_NO_DIR, /* DIO_23 */
    GPIO_CFG_NO_DIR, /* DIO_24 */
    GPIO_CFG_NO_DIR, /* DIO_25 */
    GPIO_CFG_NO_DIR, /* DIO_26 */
    GPIO_CFG_NO_DIR, /* DIO_27 */
    GPIO_CFG_NO_DIR, /* DIO_28 */
    /* Owned by /ti/drivers/RF as RF Antenna Pin 1 */
    GPIO_CFG_OUTPUT_INTERNAL | GPIO_CFG_OUT_STR_HIGH | GPIO_CFG_OUT_LOW, /* CONFIG_RF_HIGH_PA */
    GPIO_CFG_NO_DIR, /* DIO_30 */
    GPIO_CFG_NO_DIR, /* DIO_31 */
};

/*
 *  ======== gpioCallbackFunctions ========
 *  Array of callback function pointers
 *  Change at runtime with GPIO_setCallback()
 */
GPIO_CallbackFxn gpioCallbackFunctions[32];

/*
 *  ======== gpioUserArgs ========
 *  Array of user argument pointers
 *  Change at runtime with GPIO_setUserArg()
 *  Get values with GPIO_getUserArg()
 */
void* gpioUserArgs[32];

const uint_least8_t CONFIG_RF_STANDARD_PA_CONST = CONFIG_RF_STANDARD_PA;
const uint_least8_t CONFIG_RF_HIGH_PA_CONST = CONFIG_RF_HIGH_PA;
const uint_least8_t CONFIG_GPIO_UART2_0_TX_CONST = CONFIG_GPIO_UART2_0_TX;
const uint_least8_t CONFIG_GPIO_UART2_0_RX_CONST = CONFIG_GPIO_UART2_0_RX;

/*
 *  ======== GPIO_config ========
 */
const GPIO_Config GPIO_config = {
    .configs = (GPIO_PinConfig *)gpioPinConfigs,
    .callbacks = (GPIO_CallbackFxn *)gpioCallbackFunctions,
    .userArgs = gpioUserArgs,
    .intPriority = (~0)
};

/*
 *  =============================== Power ===============================
 */
#include <ti/drivers/Power.h>
#include <ti/drivers/power/PowerCC26X2.h>
#include "ti_drivers_config.h"

extern void PowerCC26XX_standbyPolicy(void);
extern bool PowerCC26XX_calibrate(unsigned int);

const PowerCC26X2_Config PowerCC26X2_config = {
    .enablePolicy             = true,
    .policyInitFxn            = NULL,
    .policyFxn                = PowerCC26XX_standbyPolicy,
    .calibrateFxn             = PowerCC26XX_calibrate,
    .calibrateRCOSC_LF        = true,
    .calibrateRCOSC_HF        = true,
    .enableTCXOFxn            = NULL
};


/*
 *  =============================== RF Driver ===============================
 */
#include <ti/drivers/GPIO.h>
#include <ti/devices/DeviceFamily.h>
#include DeviceFamily_constructPath(driverlib/ioc.h)
#include <ti/drivers/rf/RF.h>

/*
 * RF driver callback function, called by the driver on global driver events.
 */
static void RF_globalCallbackFunction (RF_Handle client, RF_GlobalEvent events, void* arg);

/*
 * Callback function to handle custom / application specific behavior
 */
extern void __attribute__((weak)) rfDriverCallback (RF_Handle client, RF_GlobalEvent events, void *arg);

/*
 * Callback function to handle antenna switching
 */
extern void __attribute__((weak)) rfDriverCallbackAntennaSwitching (RF_Handle client, RF_GlobalEvent events, void *arg);

/*
 * Platform-specific driver configuration
 */
const RFCC26XX_HWAttrsV2 RFCC26XX_hwAttrs = {
    .hwiPriority        = (~0),
    .swiPriority        = (uint8_t)0,
    .xoscHfAlwaysNeeded = true,
    .globalCallback     = &RF_globalCallbackFunction,
    .globalEventMask    = RF_GlobalEventInit | RF_GlobalEventRadioPowerDown | RF_GlobalEventRadioSetup
};

/*
 *  ======== RF_globalCallbackFunction ========
 *  This function is called by the driver on global driver events.
 *  It will call specific callback functions to further handle the triggering events.
 */
static void RF_globalCallbackFunction(RF_Handle client, RF_GlobalEvent events, void *arg)
{
    rfDriverCallback(client, events, arg);
    rfDriverCallbackAntennaSwitching(client, events, arg);
}

/*
 *  ======== rfDriverCallback ========
 *  Handle events triggered by the RF driver for custom / application specific behavior.
 */
void __attribute__((weak)) rfDriverCallback(RF_Handle client, RF_GlobalEvent events, void *arg)
{
    /* ======== PLEASE READ THIS ========
    *
    * This function is declared weak for the application to override it.
    * A new definition of 'rfDriverCallback' is required if you want to
    * handle the events listed in '.globalEventMask'.
    *
    * Please copy this function definition to create your own, but make
    * sure to remove '__attribute__((weak))' for your definition.
    *
    * According to '.globalEventMask', this function will be triggered by:
    *   - RF_GlobalEventInit
    *   - RF_GlobalEventRadioPowerDown
    *   - RF_GlobalEventRadioSetup
    *
    * An example of how to handle these events would be:
    *
    *   --- Code snippet begin ---
    *
    *   if(events & RF_GlobalEventInit) {
    *       // Perform action for this event
    *   }
    *   else if (events & RF_GlobalEventRadioPowerDown) {
    *       // Perform action for this event
    *   }
    *   else if (events & RF_GlobalEventRadioSetup) {
    *       // Perform action for this event
    *   }
    *
    *   --- Code snippet end ---
    */
}



/*
 * ======== Antenna switching ========
 */
/*
 * ======== rfDriverCallbackAntennaSwitching ========
 * Sets up the antenna switch according to the TX power table PA type.
 *
 * Truth table:
 *
 * Path       DIO0  DIO29
 * ========== ===== ===== 
 * Off        0     0
 * STANDARD PA 1     0
 * HIGH PA    0     1
 */
void __attribute__((weak)) rfDriverCallbackAntennaSwitching(RF_Handle client, RF_GlobalEvent events, void *arg)
{

    if (events & RF_GlobalEventRadioSetup) {
        /* Switch off all paths. */
        GPIO_write(CONFIG_RF_STANDARD_PA, 0);
        GPIO_write(CONFIG_RF_HIGH_PA, 0);

        /* Decode the current PA configuration. */
        RF_TxPowerTable_PAType paType = (RF_TxPowerTable_PAType)RF_getTxPower(client).paType;

        if (paType == RF_TxPowerTable_HighPA) {
            /*
            * Mux GPIOs to RF Core signals:
            * - High PA    --> HIGH_PA
            * - LNA enable --> STANDARD_PA
            */
            GPIO_setMux(CONFIG_RF_STANDARD_PA, IOC_PORT_RFC_GPO0);
            GPIO_setMux(CONFIG_RF_HIGH_PA, IOC_PORT_RFC_GPO3);
        } else {
            /* RF Core active --> 5_DBM */
            GPIO_write(CONFIG_RF_STANDARD_PA, 1);
        }
    }
    else if (events & RF_GlobalEventRadioPowerDown) {
        /* Switch off all paths. */
        GPIO_write(CONFIG_RF_STANDARD_PA, 0);
        GPIO_write(CONFIG_RF_HIGH_PA, 0);

        /* Reset the IO multiplexer to GPIO functionality */
        GPIO_setMux(CONFIG_RF_STANDARD_PA, IOC_PORT_GPIO);
        GPIO_setMux(CONFIG_RF_HIGH_PA, IOC_PORT_GPIO);
    }
}

/*
 *  =============================== Timer ===============================
 */

#include <ti/drivers/Timer.h>
#include <ti/drivers/timer/TimerCC26XX.h>

#define CONFIG_TIMER_COUNT 1

/*
 *  ======== timerCC26XXObjects ========
 */
TimerCC26XX_Object timerCC26XXObjects[CONFIG_TIMER_COUNT];

/*
 *  ======== timerCC26XXHWAttrs ========
 */
const TimerCC26XX_HWAttrs timerCC26XXHWAttrs[CONFIG_TIMER_COUNT] = {
    {
        .gpTimerUnit = CONFIG_GPTIMER_0,
        .subTimer    = TimerCC26XX_timer32

    },
};

/*
 *  ======== Timer_config ========
 */
const Timer_Config Timer_config[CONFIG_TIMER_COUNT] = {
    /* CONFIG_TIMER_0 */
    {
        .object    = &timerCC26XXObjects[CONFIG_TIMER_0],
        .hwAttrs   = &timerCC26XXHWAttrs[CONFIG_TIMER_0]
    },
};

const uint_least8_t CONFIG_TIMER_0_CONST = CONFIG_TIMER_0;
const uint_least8_t Timer_count = CONFIG_TIMER_COUNT;

/*
 *  =============================== UART2 ===============================
 */

#include <ti/drivers/UART2.h>
#include <ti/drivers/uart2/UART2CC26X2.h>
#include <ti/drivers/GPIO.h>
#include <ti/drivers/Power.h>
#include <ti/drivers/dma/UDMACC26XX.h>
#include <ti/drivers/power/PowerCC26X2.h>
#include <ti/devices/cc13x2_cc26x2/driverlib/ioc.h>
#include <ti/devices/cc13x2_cc26x2/inc/hw_memmap.h>
#include <ti/devices/cc13x2_cc26x2/inc/hw_ints.h>

#define CONFIG_UART2_COUNT 1

UART2CC26X2_Object uart2CC26X2Objects[CONFIG_UART2_COUNT];

static unsigned char uart2RxRingBuffer0[32];
/* TX ring buffer allocated to be used for nonblocking mode */
static unsigned char uart2TxRingBuffer0[32];

ALLOCATE_CONTROL_TABLE_ENTRY(dmaUart0RxControlTableEntry, UDMA_CHAN_UART0_RX);
ALLOCATE_CONTROL_TABLE_ENTRY(dmaUart0TxControlTableEntry, UDMA_CHAN_UART0_TX);

static const UART2CC26X2_HWAttrs uart2CC26X2HWAttrs[CONFIG_UART2_COUNT] = {
  {
    .baseAddr           = UART0_BASE,
    .intNum             = INT_UART0_COMB,
    .intPriority        = (~0),
    .rxPin              = CONFIG_GPIO_UART2_0_RX,
    .txPin              = CONFIG_GPIO_UART2_0_TX,
    .ctsPin             = GPIO_INVALID_INDEX,
    .rtsPin             = GPIO_INVALID_INDEX,
    .flowControl        = UART2_FLOWCTRL_NONE,
    .powerId            = PowerCC26XX_PERIPH_UART0,
    .rxBufPtr           = uart2RxRingBuffer0,
    .rxBufSize          = sizeof(uart2RxRingBuffer0),
    .txBufPtr           = uart2TxRingBuffer0,
    .txBufSize          = sizeof(uart2TxRingBuffer0),
    .txPinMux           = IOC_PORT_MCU_UART0_TX,
    .rxPinMux           = IOC_PORT_MCU_UART0_RX,
    .ctsPinMux          = IOC_PORT_MCU_UART0_CTS,
    .rtsPinMux          = IOC_PORT_MCU_UART0_RTS,
    .dmaTxTableEntryPri = &dmaUart0TxControlTableEntry,
    .dmaRxTableEntryPri = &dmaUart0RxControlTableEntry,
    .rxChannelMask      = 1 << UDMA_CHAN_UART0_RX,
    .txChannelMask      = 1 << UDMA_CHAN_UART0_TX,
    .txIntFifoThr       = UART2CC26X2_FIFO_THRESHOLD_1_8,
    .rxIntFifoThr       = UART2CC26X2_FIFO_THRESHOLD_4_8
  },
};

const UART2_Config UART2_config[CONFIG_UART2_COUNT] = {
    {   /* CONFIG_UART2_0 */
        .object      = &uart2CC26X2Objects[CONFIG_UART2_0],
        .hwAttrs     = &uart2CC26X2HWAttrs[CONFIG_UART2_0]
    },
};

const uint_least8_t CONFIG_UART2_0_CONST = CONFIG_UART2_0;
const uint_least8_t UART2_count = CONFIG_UART2_COUNT;


/*
 *  =============================== GPTimer ===============================
 */

#include <ti/drivers/timer/GPTimerCC26XX.h>
#include <ti/drivers/power/PowerCC26XX.h>
#include <ti/devices/cc13x2_cc26x2/inc/hw_memmap.h>
#include <ti/devices/cc13x2_cc26x2/inc/hw_ints.h>

#define CONFIG_GPTIMER_COUNT 1

/*
 *  ======== gptimerCC26XXObjects ========
 */
GPTimerCC26XX_Object gptimerCC26XXObjects[CONFIG_GPTIMER_COUNT];

/*
 *  ======== gptimerCC26XXHWAttrs ========
 */
const GPTimerCC26XX_HWAttrs gptimerCC26XXHWAttrs[CONFIG_GPTIMER_COUNT] = {
    /* CONFIG_GPTIMER_0, used by CONFIG_TIMER_0 */
    {
        .baseAddr = GPT0_BASE,
        .intNum      = INT_GPT0A,
        .intPriority = (~0),
        .powerMngrId = PowerCC26XX_PERIPH_GPT0,
        .pinMux      = GPT_PIN_0A
    },
};

/*
 *  ======== GPTimer_config ========
 */
const GPTimerCC26XX_Config GPTimerCC26XX_config[CONFIG_GPTIMER_COUNT] = {
    /* CONFIG_GPTIMER_0 */
    {
        .object    = &gptimerCC26XXObjects[CONFIG_GPTIMER_0],
        .hwAttrs   = &gptimerCC26XXHWAttrs[CONFIG_GPTIMER_0],
        .timerPart = GPT_A
    },
};

const uint_least8_t CONFIG_GPTIMER_0_CONST = CONFIG_GPTIMER_0;
const uint_least8_t GPTimer_count = CONFIG_GPTIMER_COUNT;

#include <stdbool.h>

#include <ti/devices/cc13x2_cc26x2/driverlib/ioc.h>
#include <ti/devices/cc13x2_cc26x2/driverlib/cpu.h>

#include <ti/drivers/GPIO.h>

/* Board GPIO defines */
#define BOARD_EXT_FLASH_SPI_CS      20
#define BOARD_EXT_FLASH_SPI_CLK     10
#define BOARD_EXT_FLASH_SPI_MOSI    9
#define BOARD_EXT_FLASH_SPI_MISO    8


/*
 *  ======== Board_sendExtFlashByte ========
 */
void Board_sendExtFlashByte(uint8_t byte)
{
    uint8_t i;

    /* SPI Flash CS */
    GPIO_write(BOARD_EXT_FLASH_SPI_CS, 0);

    for (i = 0; i < 8; i++) {
        GPIO_write(BOARD_EXT_FLASH_SPI_CLK, 0); /* SPI Flash CLK */

        /* SPI Flash MOSI */
        GPIO_write(BOARD_EXT_FLASH_SPI_MOSI, (byte >> (7 - i)) & 0x01);
        GPIO_write(BOARD_EXT_FLASH_SPI_CLK, 1);  /* SPI Flash CLK */

        /*
         * Waste a few cycles to keep the CLK high for at
         * least 45% of the period.
         * 3 cycles per loop: 8 loops @ 48 Mhz = 0.5 us.
         */
        CPUdelay(8);
    }

    GPIO_write(BOARD_EXT_FLASH_SPI_CLK, 0);  /* CLK */
    GPIO_write(BOARD_EXT_FLASH_SPI_CS, 1);  /* CS */

    /*
     * Keep CS high at least 40 us
     * 3 cycles per loop: 700 loops @ 48 Mhz ~= 44 us
     */
    CPUdelay(700);
}

/*
 *  ======== Board_wakeUpExtFlash ========
 */
void Board_wakeUpExtFlash(void)
{
    /* SPI Flash CS*/
    GPIO_setConfig(BOARD_EXT_FLASH_SPI_CS, GPIO_CFG_OUTPUT | GPIO_CFG_OUT_HIGH | GPIO_CFG_OUT_STR_MED);

    /*
     *  To wake up we need to toggle the chip select at
     *  least 20 ns and ten wait at least 35 us.
     */

    /* Toggle chip select for ~20ns to wake ext. flash */
    GPIO_write(BOARD_EXT_FLASH_SPI_CS, 0);
    /* 3 cycles per loop: 1 loop @ 48 Mhz ~= 62 ns */
    CPUdelay(1);
    GPIO_write(BOARD_EXT_FLASH_SPI_CS, 1);
    /* 3 cycles per loop: 560 loops @ 48 Mhz ~= 35 us */
    CPUdelay(560);
}

/*
 *  ======== Board_shutDownExtFlash ========
 */
void Board_shutDownExtFlash(void)
{
    /*
     *  To be sure we are putting the flash into sleep and not waking it,
     *  we first have to make a wake up call
     */
    Board_wakeUpExtFlash();

    /* SPI Flash CS*/
    GPIO_setConfig(BOARD_EXT_FLASH_SPI_CS, GPIO_CFG_OUTPUT | GPIO_CFG_OUT_HIGH | GPIO_CFG_OUT_STR_MED);
    /* SPI Flash CLK */
    GPIO_setConfig(BOARD_EXT_FLASH_SPI_CLK, GPIO_CFG_OUTPUT | GPIO_CFG_OUT_LOW | GPIO_CFG_OUT_STR_MED);
    /* SPI Flash MOSI */
    GPIO_setConfig(BOARD_EXT_FLASH_SPI_MOSI, GPIO_CFG_OUTPUT | GPIO_CFG_OUT_LOW | GPIO_CFG_OUT_STR_MED);
    /* SPI Flash MISO */
    GPIO_setConfig(BOARD_EXT_FLASH_SPI_MISO, GPIO_CFG_IN_PD);

    uint8_t extFlashShutdown = 0xB9;

    Board_sendExtFlashByte(extFlashShutdown);

    GPIO_resetConfig(BOARD_EXT_FLASH_SPI_CS);
    GPIO_resetConfig(BOARD_EXT_FLASH_SPI_CLK);
    GPIO_resetConfig(BOARD_EXT_FLASH_SPI_MOSI);
    GPIO_resetConfig(BOARD_EXT_FLASH_SPI_MISO);
}


#include <ti/drivers/Board.h>

/*
 *  ======== Board_initHook ========
 *  Perform any board-specific initialization needed at startup.  This
 *  function is declared weak to allow applications to override it if needed.
 */
void __attribute__((weak)) Board_initHook(void)
{
}

/*
 *  ======== Board_init ========
 *  Perform any initialization needed before using any board APIs
 */
void Board_init(void)
{
    /* ==== /ti/drivers/Power initialization ==== */
    Power_init();

    /* ==== /ti/devices/CCFG initialization ==== */

    /* ==== /ti/drivers/GPIO initialization ==== */
    /* Setup GPIO module and default-initialise pins */
    GPIO_init();

    /* ==== /ti/drivers/RF initialization ==== */

    Board_shutDownExtFlash();

    Board_initHook();
}

