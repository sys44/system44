// int_handler.h
/* Comments added by VeryEpicKebap */
#pragma once
#include <stdint.h>

/* Initialize the IDT and PIC */
void int_init(void);
extern volatile uint32_t ticks;
extern volatile uint8_t tick_flag;
/* C handlers you can optionally call or hook into (fixed early test names) */
void irq0h(void);
void irq1h(void);
void isr0h(uint32_t n);

/* Assembly stubs (used internally, can be extern if needed) */
extern void isr0(void);
extern void irq0(void);
extern void irq1(void);
