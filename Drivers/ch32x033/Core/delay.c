#include "delay.h"

#include "ch32x035_conf.h"

static uint8_t p_us = 0;

/**
 * @brief Initialize delay functions
 * Note: Only initializes delay_us. delay_ms uses timer tick from timer.c
 */
void delay_init(void) {
  /* Calculate for delay_us only */
  p_us = SystemCoreClock / 8000000;
}

void delay_us(uint32_t n)
{
  uint32_t i;

  SysTick->SR &= ~(1 << 0);
  i = (uint32_t)n * p_us;

  SysTick->CMP = i;
  SysTick->CTLR |= (1 << 4);
  SysTick->CTLR |= (1 << 5) | (1 << 0);

  while ((SysTick->SR & (1 << 0)) != (1 << 0))
    ;
  SysTick->CTLR &= ~(1 << 0);
}

/**
 * @brief Delay for specified milliseconds
 * @param n Number of milliseconds to delay
 *
 * Note: Uses timer tick from timer.c (timer_init() must be called first)
 */
void delay_ms(uint32_t n)
{
  uint32_t start_tick = get_tick();

  /* Wait until the required time has elapsed */
  while ((get_tick() - start_tick) < n) {
    __NOP();
  }
}
