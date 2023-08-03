#include "contiki.h"
#include "soc.h"
#include "stack.h"
#include "sys/clock.h"
#include "sys/autostart.h"
#include "dev/serial-line.h"
#include "dev/slip.h"
#include "dev/io-arch.h"
#include "dev/dma.h"
#include "dev/cc2530-rf.h"
#include "dev/radio.h"
#include "dev/watchdog.h"
#include "dev/clock-isr.h"
#include "dev/port2.h"
#include "dev/lpm.h"
#include "net/rime/rime.h"
#include "net/netstack.h"
#include "net/mac/frame802154.h"
#include "debug.h"
#include "cc253x.h"
#include "sfr-bits.h"
#include "contiki-lib.h"
#include "contiki-net.h"
int main(void) CC_NON_BANKED
{
  clock_init();
  soc_init();
  rtimer_init();
  stack_poison();
  process_init();

#if DMA_ON
  dma_init();
#endif

  io_arch_init();

#if SLIP_ARCH_CONF_ENABLE
  slip_arch_init(0);
#else
  io_arch_set_input(serial_line_input_byte);
  serial_line_init();
#endif

  random_init(0);
  process_start(&etimer_process, NULL);
  ctimer_init();
  netstack_init();
  energest_init();
  ENERGEST_ON(ENERGEST_TYPE_CPU);

  autostart_start(autostart_processes);
  while(1) {
    uint8_t r;
    do {
      r = process_run();
    } while(r > 0);
  }
}
