/**
 * @file gpio.h
 * @brief GPIO driver interface for CH32X035
 *
 * This file provides GPIO initialization and control functions
 * for the CH32X035 microcontroller.
 */

#ifndef __GPIO_H
#define __GPIO_H

#include <stdint.h>

/**
 * @brief Initialize GPIO pins
 *
 * Configures the running LED on PA1 as push-pull output at 50MHz.
 * Enables the GPIOA peripheral clock.
 *
 * @return None
 */
void gpio_init(void);

/**
 * @brief Toggle the running LED state
 *
 * Toggles the state of the LED on PA1 between on and off.
 * Uses a static variable to maintain the current state.
 *
 * @return None
 */
void gpio_toggle_running_led(void);

void gpio_cs_flash_memory(uint8_t enable);

#endif