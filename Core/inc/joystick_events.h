/**
 * @file joystick_events.h
 * @brief Event layer for the analog joystick driver.
 *
 * Sits on top of joystick.c and classifies raw ADC readings into
 * directional and button events.  Consumers register callbacks via
 * joystick_events_subscribe(); events are fired only on state
 * transitions (edge-detection), never continuously while held.
 *
 * Thresholds (12-bit ADC, 0-4095)
 * ---------------------------------
 * Derived from real hardware measurements:
 *   Center : VRx ~2072, VRy ~1958  (nominal mid = 2048)
 *   Up     : VRy ~4091             (VRy > JOYSTICK_THRESHOLD_HIGH)
 *   Down   : VRy ~11               (VRy < JOYSTICK_THRESHOLD_LOW)
 *   Left   : VRx ~4090             (VRx > JOYSTICK_THRESHOLD_HIGH)
 *   Right  : VRx ~11               (VRx < JOYSTICK_THRESHOLD_LOW)
 *
 * Override thresholds before including this header if your hardware
 * differs, e.g. -DJOYSTICK_THRESHOLD_HIGH=3200.
 *
 * Debug output
 * ------------
 * Controlled by JOYSTICK_DEBUG (defined in joystick.h).
 * Set to 0 or -DJOYSTICK_DEBUG=0 to compile out all log calls.
 */

#ifndef __JOYSTICK_EVENTS_H
#define __JOYSTICK_EVENTS_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef JOYSTICK_THRESHOLD_HIGH
#define JOYSTICK_THRESHOLD_HIGH   3500U
#endif

#ifndef JOYSTICK_THRESHOLD_LOW
#define JOYSTICK_THRESHOLD_LOW    500U
#endif

/** Nominal ADC center value (12-bit mid-scale). */
#define JOYSTICK_CENTER           2048U

/** Maximum number of simultaneous subscribers. */
#ifndef JOYSTICK_MAX_SUBSCRIBERS
#define JOYSTICK_MAX_SUBSCRIBERS  4U
#endif

/** Events fired to subscribers. */
typedef enum {
    JOYSTICK_EVENT_UP      = 0,
    JOYSTICK_EVENT_DOWN    = 1,
    JOYSTICK_EVENT_LEFT    = 2,
    JOYSTICK_EVENT_RIGHT   = 3,
    JOYSTICK_EVENT_CENTER  = 4,  /**< Joystick returned to the dead zone. */
    JOYSTICK_EVENT_PRESS   = 5,  /**< SW button pressed (active-low edge). */
    JOYSTICK_EVENT_RELEASE = 6,  /**< SW button released.                  */
    JOYSTICK_EVENT_COUNT   = 7,
} joystick_event_t;

/**
 * @brief Callback type for joystick events.
 *
 * @param event      The event that occurred.
 * @param user_data  Opaque pointer supplied at subscribe time.
 */
typedef void (*joystick_cb_t)(joystick_event_t event, void *user_data);

/**
 * @brief Initialize the event layer.
 *
 * Clears all subscriber slots and resets internal direction state.
 * Must be called after joystick_init().
 *
 * @return true always (reserved for future error reporting).
 */
bool joystick_events_init(void);

/**
 * @brief Register a callback to receive joystick events.
 *
 * @param cb         Callback function; must not be NULL.
 * @param user_data  Opaque pointer passed back on every call; may be NULL.
 * @return true  Registration succeeded.
 * @return false @p cb is NULL or all subscriber slots are occupied.
 */
bool joystick_events_subscribe(joystick_cb_t cb, void *user_data);

/**
 * @brief Remove a previously registered callback.
 *
 * No-op if @p cb is not found or is NULL.
 *
 * @param cb  Callback to remove.
 */
void joystick_events_unsubscribe(joystick_cb_t cb);

/**
 * @brief Process joystick data and fire events on state changes.
 *
 * Call this every main-loop iteration.  Internally it calls
 * joystick_poll() (which is already time-gated to JOYSTICK_POLL_PERIOD_MS),
 * then classifies the reading and dispatches callbacks only when the
 * direction or button state changes.
 */
void joystick_events_poll(void);

/**
 * @brief Return a human-readable name for an event (for logging).
 *
 * @param event  Event to name.
 * @return Pointer to a constant string; never NULL.
 */
const char *joystick_event_name(joystick_event_t event);

#ifdef __cplusplus
}
#endif

#endif /* __JOYSTICK_EVENTS_H */
