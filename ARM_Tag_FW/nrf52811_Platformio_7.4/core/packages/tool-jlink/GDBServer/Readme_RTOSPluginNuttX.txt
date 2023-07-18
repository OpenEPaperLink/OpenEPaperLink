/*********************************************************************
*                   (c) SEGGER Microcontroller GmbH                  *
*                        The Embedded Experts                        *
*                           www.segger.com                           *
**********************************************************************
----------------------------------------------------------------------
*/

The "NuttX" RTOS-Plugin for the J-Link GDB Server is provided by SEGGER as it is.
The plugin is provided *without* support from SEGGER.

The plugin requires some additional configuration commands. These commands can be
sent automatically by creating a .gdbinit file as follows:

define hookpost-file
  eval "monitor rtosconf pid_offset %d", &((struct tcb_s *)(0))->pid
  eval "monitor rtosconf regs_offset %d", &((struct tcb_s *)(0))->xcp.regs
  eval "monitor rtosconf state_offset %d", &((struct tcb_s *)(0))->task_state
  eval "monitor rtosconf name_offset %d", &((struct tcb_s *)(0))->name
  eval "monitor rtosconf prio_offset %d", &((struct tcb_s *)(0))->sched_priority
end
