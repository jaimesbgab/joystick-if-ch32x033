/**
 * @file uart.h
 * @brief UART driver interface for CH32X035
 *
 * This file provides both hardware UART (USART2) and software UART
 * interfaces for serial communication on the CH32X035 microcontroller.
 */

#ifndef __UART_H
#define __UART_H

/**
 * @brief Initialize hardware UART (USART2)
 *
 * Configures USART2 with the following settings:
 * - Baud rate: 115200
 * - Data bits: 8
 * - Stop bits: 1
 * - Parity: None
 * - Mode: Transmit only
 * - TX pin: PA2 (alternate function push-pull)
 *
 * @return None
 */
void uart_init(void);

/**
 * @brief Transmit a single character via hardware UART
 *
 * Sends one character through USART2. This function blocks until
 * the transmission is complete.
 *
 * @param ch Character to transmit
 * @return None
 */
void uart_putc(char ch);

/**
 * @brief Initialize software UART
 *
 * Configures PA3 as a GPIO output for software UART transmission.
 * The pin is initialized to high state (UART idle state).
 * Software UART operates at 9600 baud.
 *
 * @return None
 */
void soft_uart_init(void);

/**
 * @brief Transmit a single character via software UART
 *
 * Sends one character through software UART on PA3 at 9600 baud.
 * Uses bit-banging to generate the UART signal with proper timing:
 * - Start bit (LOW)
 * - 8 data bits (LSB first)
 * - Stop bit (HIGH)
 *
 * @param ch Character to transmit
 * @return None
 */
void soft_uart_putc(char ch);

#endif