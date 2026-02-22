/**
 * @file joystick_events.c
 * @brief Event layer for the analog joystick driver.
 */

#include "joystick_events.h"
#include "joystick.h"
#include "log.h"

#include <stddef.h>

#if JOYSTICK_DEBUG
#  define EVT_DBG(fmt, ...)  LOG_DEBUG(fmt, ##__VA_ARGS__)
#else
#  define EVT_DBG(fmt, ...)  ((void)0)
#endif

/* Internal direction state (not part of the public API). */
typedef enum {
    DIR_CENTER = 0,
    DIR_UP,
    DIR_DOWN,
    DIR_LEFT,
    DIR_RIGHT,
} direction_t;

typedef struct {
    joystick_cb_t  cb;
    void          *user_data;
} subscriber_t;

static subscriber_t s_subs[JOYSTICK_MAX_SUBSCRIBERS];
static bool         s_initialized = false;
static direction_t  s_prev_dir    = DIR_CENTER;
static uint8_t      s_prev_sw     = 0U;

/**
 * @brief Dispatch an event to every registered subscriber.
 */
static void fire_event(joystick_event_t event)
{
    for (uint8_t i = 0U; i < (uint8_t)JOYSTICK_MAX_SUBSCRIBERS; i++) {
        if (s_subs[i].cb != NULL) {
            s_subs[i].cb(event, s_subs[i].user_data);
        }
    }
}

/**
 * @brief Classify raw ADC readings into a direction.
 *
 * When both axes exceed a threshold simultaneously (diagonal push),
 * the axis with the larger absolute deflection from centre wins.
 * If neither axis is outside its dead zone the result is DIR_CENTER.
 *
 * @param vrx  Raw X-axis ADC value (0-4095).
 * @param vry  Raw Y-axis ADC value (0-4095).
 * @return Detected direction.
 */
static direction_t classify(uint16_t vrx, uint16_t vry)
{
    bool x_high = (vrx > (uint16_t)JOYSTICK_THRESHOLD_HIGH);
    bool x_low  = (vrx < (uint16_t)JOYSTICK_THRESHOLD_LOW);
    bool y_high = (vry > (uint16_t)JOYSTICK_THRESHOLD_HIGH);
    bool y_low  = (vry < (uint16_t)JOYSTICK_THRESHOLD_LOW);

    if (!x_high && !x_low && !y_high && !y_low) {
        return DIR_CENTER;
    }

    /* Absolute deflection from nominal centre (safe: values are 0-4095,
     * centre is 2048, so dx/dy fit in int16_t; using int32_t for clarity). */
    int32_t  dx  = (int32_t)vrx - (int32_t)JOYSTICK_CENTER;
    int32_t  dy  = (int32_t)vry - (int32_t)JOYSTICK_CENTER;
    uint32_t adx = (dx < 0) ? (uint32_t)(-dx) : (uint32_t)(dx);
    uint32_t ady = (dy < 0) ? (uint32_t)(-dy) : (uint32_t)(dy);

    if (adx >= ady) {
        if (x_low)  return DIR_RIGHT;
        if (x_high) return DIR_LEFT;
    } else {
        if (y_high) return DIR_UP;
        if (y_low)  return DIR_DOWN;
    }

    return DIR_CENTER;
}

const char *joystick_event_name(joystick_event_t event)
{
    static const char *const names[(uint8_t)JOYSTICK_EVENT_COUNT] = {
        "UP", "DOWN", "LEFT", "RIGHT", "CENTER", "PRESS", "RELEASE",
    };

    if ((uint8_t)event < (uint8_t)JOYSTICK_EVENT_COUNT) {
        return names[(uint8_t)event];
    }

    return "UNKNOWN";
}

bool joystick_events_init(void)
{
    for (uint8_t i = 0U; i < (uint8_t)JOYSTICK_MAX_SUBSCRIBERS; i++) {
        s_subs[i].cb        = NULL;
        s_subs[i].user_data = NULL;
    }

    s_prev_dir    = DIR_CENTER;
    s_prev_sw     = 0U;
    s_initialized = true;

    return true;
}

bool joystick_events_subscribe(joystick_cb_t cb, void *user_data)
{
    if (cb == NULL) {
        return false;
    }

    for (uint8_t i = 0U; i < (uint8_t)JOYSTICK_MAX_SUBSCRIBERS; i++) {
        if (s_subs[i].cb == NULL) {
            s_subs[i].cb        = cb;
            s_subs[i].user_data = user_data;
            return true;
        }
    }

    return false;
}

void joystick_events_unsubscribe(joystick_cb_t cb)
{
    if (cb == NULL) {
        return;
    }

    for (uint8_t i = 0U; i < (uint8_t)JOYSTICK_MAX_SUBSCRIBERS; i++) {
        if (s_subs[i].cb == cb) {
            s_subs[i].cb        = NULL;
            s_subs[i].user_data = NULL;
        }
    }
}

void joystick_events_poll(void)
{
    if (!s_initialized) {
        return;
    }

    /* Refresh hardware state (time-gated to JOYSTICK_POLL_PERIOD_MS). */
    joystick_poll();

    joystick_data_t data;
    if (!joystick_get_data(&data)) {
        return;
    }

    /* Direction event: fire only on state transition. */
    direction_t dir = classify(data.vrx, data.vry);
    if (dir != s_prev_dir) {
        joystick_event_t event;

        switch (dir) {
            case DIR_UP:    event = JOYSTICK_EVENT_UP;     break;
            case DIR_DOWN:  event = JOYSTICK_EVENT_DOWN;   break;
            case DIR_LEFT:  event = JOYSTICK_EVENT_LEFT;   break;
            case DIR_RIGHT: event = JOYSTICK_EVENT_RIGHT;  break;
            default:        event = JOYSTICK_EVENT_CENTER; break;
        }

        EVT_DBG("joystick event: %s", joystick_event_name(event));
        fire_event(event);
        s_prev_dir = dir;
    }

    /* Button event: fire on press and release edges. */
    if (data.sw != s_prev_sw) {
        joystick_event_t event = (data.sw != 0U)
                                 ? JOYSTICK_EVENT_PRESS
                                 : JOYSTICK_EVENT_RELEASE;

        EVT_DBG("joystick event: %s", joystick_event_name(event));
        fire_event(event);
        s_prev_sw = data.sw;
    }
}
