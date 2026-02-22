/**
 * @file uart.c
 * @brief UART driver implementation for CH32X035
 *
 * This file provides both hardware UART (USART2) and software UART
 * implementations for serial communication on the CH32X035 microcontroller.
 */

#include "uart.h"

#include "ch32x035_conf.h"
#include "delay.h"

/* Software UART bit timing for 9600 baud */
#define SOFT_UART_BIT_TIME_US  104  /**< Bit time for 9600 baud: 1/9600 = 104.167 us */

void uart_init(void) {
  GPIO_InitTypeDef  GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure;

  /* Enable clocks first: USART2, GPIOA, and AFIO */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

  /* Ensure USART2 is using default pins (no remap) */
  GPIO_PinRemapConfig(GPIO_PartialRemap1_USART2, DISABLE);
  GPIO_PinRemapConfig(GPIO_PartialRemap2_USART2, DISABLE);
  GPIO_PinRemapConfig(GPIO_PartialRemap3_USART2, DISABLE);
  GPIO_PinRemapConfig(GPIO_FullRemap_USART2, DISABLE);

  /* Configure PA2 as USART2 TX (alternate function push-pull) */
  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_2;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* Configure USART2 */
  USART_InitStructure.USART_BaudRate = 115200;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Tx;

  USART_Init(USART2, &USART_InitStructure);
  USART_Cmd(USART2, ENABLE);
}

void uart_putc(char ch) {
  /* Wait until transmit data register is empty */
  while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
  USART_SendData(USART2, ch);
  /* Wait for transmission to complete */
  while(USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);
}

void soft_uart_init(void) {
  GPIO_InitTypeDef GPIO_InitStructure;

  /* Enable GPIOA clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

  /* Configure PA3 as output push-pull */
  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* Set PA3 high (UART idle state) */
  GPIO_SetBits(GPIOA, GPIO_Pin_3);
}

void soft_uart_putc(char ch) {
  uint8_t i;

  /* Start bit (LOW) */
  GPIO_ResetBits(GPIOA, GPIO_Pin_3);
  delay_us(SOFT_UART_BIT_TIME_US);

  /* Send 8 data bits (LSB first) */
  for (i = 0; i < 8; i++) {
    if (ch & 0x01) {
      GPIO_SetBits(GPIOA, GPIO_Pin_3);  /* Send 1 */
    } else {
      GPIO_ResetBits(GPIOA, GPIO_Pin_3);  /* Send 0 */
    }
    delay_us(SOFT_UART_BIT_TIME_US);
    ch >>= 1;  /* Shift to next bit */
  }

  /* Stop bit (HIGH) */
  GPIO_SetBits(GPIOA, GPIO_Pin_3);
  delay_us(SOFT_UART_BIT_TIME_US);
}