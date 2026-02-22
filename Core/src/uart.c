/**
 * @file uart.c
 * @brief UART driver implementation for CH32X035
 *
 * Software UART: bit-bang TX on PA10, 9600 8N1.
 * No hardware USART peripheral is used.
 */

#include "uart.h"

#include "ch32x035_conf.h"
#include "delay.h"

/* Bit time for 9600 baud: 1/9600 ≈ 104 µs */
#define UART_BIT_TIME_US  104U

#define UART_PORT   GPIOA
#define UART_PIN    GPIO_Pin_10

void uart_init(void) {
    GPIO_InitTypeDef gpio = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    gpio.GPIO_Pin   = UART_PIN;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    gpio.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_Init(UART_PORT, &gpio);

    /* Idle state is high */
    GPIO_SetBits(UART_PORT, UART_PIN);
}

void uart_putc(char ch) {
    uint8_t i;

    __disable_irq();

    /* Start bit */
    GPIO_ResetBits(UART_PORT, UART_PIN);
    delay_us(UART_BIT_TIME_US);

    /* 8 data bits, LSB first */
    for (i = 0; i < 8U; i++) {
        if ((uint8_t)ch & 0x01U) {
            GPIO_SetBits(UART_PORT, UART_PIN);
        } else {
            GPIO_ResetBits(UART_PORT, UART_PIN);
        }
        delay_us(UART_BIT_TIME_US);
        ch >>= 1;
    }

    /* Stop bit */
    GPIO_SetBits(UART_PORT, UART_PIN);
    delay_us(UART_BIT_TIME_US);

    __enable_irq();
}
