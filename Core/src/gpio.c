/**
 * @file gpio.c
 * @brief GPIO driver implementation for CH32X035
 *
 * This file provides GPIO initialization and control functions
 * for the CH32X035 microcontroller.
 */

#include "gpio.h"

#include "ch32x035_conf.h"

void gpio_init(void) {
  GPIO_InitTypeDef GPIO_InitStructure = {0};

  /* Initialize running led */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* Initialize running led */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
}

void gpio_toggle_running_led(void) {
  static uint8_t state = 0;

  GPIO_WriteBit(GPIOA, GPIO_Pin_1, (state == 0) ? (state = Bit_SET) : (state = Bit_RESET));
}

void gpio_cs_flash_memory(uint8_t enable) {
  GPIO_WriteBit(GPIOB, GPIO_Pin_7, (enable != 0) ? Bit_SET :  Bit_RESET);
}