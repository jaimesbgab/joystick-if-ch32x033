/**
 * @file uart.h
 * @brief UART driver interface for CH32X035
 *
 * Software UART: bit-bang TX on PA10, 9600 8N1.
 */

#ifndef __UART_H
#define __UART_H

/**
 * @brief Initialize the software UART.
 *
 * Configures PA10 as a push-pull output and drives it high (idle state).
 * Must be called before uart_putc().
 */
void uart_init(void);

/**
 * @brief Transmit a single byte via software UART.
 *
 * Bit-bangs one start bit, 8 data bits (LSB first), and one stop bit
 * on PA10 at 9600 baud. Blocks for the full frame duration (~1.1 ms).
 *
 * @param ch Byte to transmit.
 */
void uart_putc(char ch);

#endif
