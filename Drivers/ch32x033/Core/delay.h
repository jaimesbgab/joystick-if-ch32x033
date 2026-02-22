#ifndef __DELAY_H
#define __DELAY_H

#include <stdint.h>

void delay_init(void);
void delay_us(uint32_t n);
void delay_ms(uint32_t n);

/* Note: get_tick() is provided by timer.c (timer_init() must be called) */
uint32_t get_tick(void);

#endif