/**
 * @file joystick.h
 * @brief Analog joystick driver for CH32X035
 *
 * Reads a 2-axis analog joystick with a push-button via:
 *   VRx  -> PA1  (ADC1, channel 1)
 *   VRy  -> PA2  (ADC1, channel 2)
 *   SW   -> PA3  (digital input, active-low, internal pull-up)
 *
 * @note PA1 is shared with the running-LED in gpio.c.  Call
 *       joystick_init() AFTER gpio_init(); the ADC analog-input
 *       mode will override the output configuration.
 *
 * Debug output
 * ------------
 * Set JOYSTICK_DEBUG to 1 (or pass -DJOYSTICK_DEBUG=1 in the build
 * system) to enable LOG_DEBUG messages during every poll cycle.
 * Set to 0 (or -DJOYSTICK_DEBUG=0) to compile them out completely.
 */

#ifndef __JOYSTICK_H
#define __JOYSTICK_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Enable (1) or disable (0) debug log output from the joystick driver.
 *  Override at the compiler command line: -DJOYSTICK_DEBUG=0            */
#ifndef JOYSTICK_DEBUG
#define JOYSTICK_DEBUG  1
#endif

/** Polling period in milliseconds. */
#define JOYSTICK_POLL_PERIOD_MS   250U

/** 12-bit ADC full-scale count. */
#define JOYSTICK_ADC_MAX          4095U

/** Safety timeout for ADC conversion polling (loop iterations). */
#define JOYSTICK_ADC_TIMEOUT      100000U

/** Snapshot of joystick state captured during one poll cycle. */
typedef struct {
    uint16_t vrx;   /**< X-axis raw ADC count  (0 - JOYSTICK_ADC_MAX). */
    uint16_t vry;   /**< Y-axis raw ADC count  (0 - JOYSTICK_ADC_MAX). */
    uint8_t  sw;    /**< Button: 1 = pressed, 0 = released.            */
} joystick_data_t;

/**
 * @brief Initialize the joystick driver.
 *
 * Configures PA1/PA2 as ADC analog inputs, PA3 as a digital input with
 * internal pull-up, enables ADC1, then takes an initial sample so that
 * joystick_get_data() is valid immediately.
 */
void joystick_init(void);

/**
 * @brief Non-blocking poll — call every main-loop iteration.
 *
 * Reads VRx, VRy and SW only when JOYSTICK_POLL_PERIOD_MS has elapsed
 * since the last successful read.  When JOYSTICK_DEBUG == 1 a debug
 * message is emitted after each sample.
 *
 * Must be called after joystick_init().
 */
void joystick_poll(void);

/**
 * @brief Copy the most recently polled joystick state to @p out.
 *
 * @param[out] out  Pointer to a caller-allocated joystick_data_t.
 *                  Must not be NULL.
 * @return true  on success.
 * @return false if @p out is NULL or joystick_init() was never called.
 */
bool joystick_get_data(joystick_data_t *out);

#ifdef __cplusplus
}
#endif

#endif /* __JOYSTICK_H */
