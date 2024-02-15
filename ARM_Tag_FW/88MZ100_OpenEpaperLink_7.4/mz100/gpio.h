#pragma once

void setupWDT();

void NVIC_some_IRQ1(unsigned int a1);
void NIVC_some_IRQ(unsigned int a1);
void init_GPIO_boot();
void init_GPIO_sleep();
void enable_irq_for_pin(int a1, unsigned int a2);
void wait_busy_sleep(int a1);
void do_sleeped_epd_refresh();