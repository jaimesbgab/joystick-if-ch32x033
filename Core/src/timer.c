#include "timer.h"

#include "ch32x035_conf.h"

volatile uint32_t timer_tick = 0;

/**
 * @brief TIM1 Update interrupt handler - called every 1ms
 */
void TIM1_UP_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void TIM1_UP_IRQHandler(void)
{
  if(TIM_GetITStatus(TIM1, TIM_IT_Update) == SET)
  {
    timer_tick++;
  }
  TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
}

/**
 * @brief Initialize TIM1 to generate 1ms tick interrupt
 *
 * For 48MHz system clock:
 * - Prescaler: 48 - 1 = 47 (divides to 1MHz)
 * - Period: 1000 - 1 = 999 (1MHz / 1000 = 1ms)
 */
void timer_init(void) {
  NVIC_InitTypeDef NVIC_InitStructure = {0};
  TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure = {0};

  /* Enable TIM1 clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);

  /* Configure TIM1 for 1ms interrupt
   * TIM1 clock = 48MHz (APB2 clock)
   * Prescaler = 48 -> Timer clock = 48MHz / 48 = 1MHz
   * Period = 1000 -> Interrupt frequency = 1MHz / 1000 = 1kHz (1ms)
   */
  TIM_TimeBaseInitStructure.TIM_Period = 1000 - 1;        // Auto-reload value (ARR)
  TIM_TimeBaseInitStructure.TIM_Prescaler = 48 - 1;       // Prescaler (PSC)
  TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;    // No repetition
  TIM_TimeBaseInit(TIM1, &TIM_TimeBaseInitStructure);

  /* Clear update interrupt flag */
  TIM_ClearITPendingBit(TIM1, TIM_IT_Update);

  /* Configure NVIC for TIM1 update interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  /* Enable TIM1 update interrupt */
  TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);

  /* Start TIM1 */
  TIM_Cmd(TIM1, ENABLE);
}

uint32_t get_tick(void)
{
  return timer_tick;
}