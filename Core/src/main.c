/**
 * @file main.c
 * @brief Main application entry point for CH32X035
 *
 * Reads an analog joystick (VRx/VRy/SW) and forwards directional and button
 * commands over UART as single-byte device_cmd_t values.  Log output is
 * routed to the USB CDC serial port.
 */

#include "ch32x035_conf.h"

#include "delay.h"
#include "timer.h"
#include "gpio.h"
#include "usb.h"
#include "joystick.h"
#include "joystick_events.h"
#include "uart.h"
#include "log.h"

/**
 * @brief Command byte sent over UART for each joystick event.
 *
 * Values start at 1 so that 0 is never a valid command, making it easy
 * for the receiver to detect framing errors or idle lines.
 *
 * DEVICE_CMD_START : single press of the SW button.
 * DEVICE_CMD_COIN  : double press of the SW button within DOUBLE_PRESS_WINDOW_MS.
 */
typedef enum {
    DEVICE_CMD_UP    = 1,
    DEVICE_CMD_DOWN  = 2,
    DEVICE_CMD_LEFT  = 3,
    DEVICE_CMD_RIGHT = 4,
    DEVICE_CMD_START = 5,
    DEVICE_CMD_COIN  = 6,
} device_cmd_t;

/** Maximum gap between two presses to count as a double-press (ms). */
#define DOUBLE_PRESS_WINDOW_MS  300U

static uint32_t s_press_time_ms = 0U;
static bool     s_press_pending = false;

/**
 * @brief Transmit a device command over UART.
 *
 * Validates that @p cmd is within the defined range before sending.
 * Out-of-range values are silently discarded.
 *
 * @param cmd  Command to send.
 */
void send_cmd(device_cmd_t cmd)
{
    if ((uint8_t)cmd < (uint8_t)DEVICE_CMD_UP ||
        (uint8_t)cmd > (uint8_t)DEVICE_CMD_COIN) {
        return;
    }

    uart_putc((char)cmd);
}

/**
 * @brief Resolve a pending single press once the double-press window expires.
 *
 * Call every main-loop iteration.  If a press was recorded and
 * DOUBLE_PRESS_WINDOW_MS has elapsed without a second press, the deferred
 * DEVICE_CMD_START is sent.
 */
static void device_cmd_poll(void)
{
    if (!s_press_pending) {
        return;
    }

    if ((get_tick() - s_press_time_ms) > DOUBLE_PRESS_WINDOW_MS) {
        s_press_pending = false;
        LOG_INFO("joystick -> START");
        send_cmd(DEVICE_CMD_START);
    }
}

/**
 * @brief Joystick event handler — maps events to device commands.
 *
 * Registered via joystick_events_subscribe().  Logs the event name and
 * forwards directional and press events as device_cmd_t bytes over UART.
 * CENTER and RELEASE events are intentionally not forwarded.
 *
 * SW button press logic:
 *   - First press arms a timer.
 *   - Second press within DOUBLE_PRESS_WINDOW_MS → DEVICE_CMD_COIN (immediate).
 *   - No second press before the window expires → DEVICE_CMD_START (deferred).
 */
static void on_joystick_event(joystick_event_t event, void *user_data)
{
    (void)user_data;

    switch (event) {
        case JOYSTICK_EVENT_UP:
            LOG_INFO("joystick -> UP");
            send_cmd(DEVICE_CMD_UP);
            break;
        case JOYSTICK_EVENT_DOWN:
            LOG_INFO("joystick -> DOWN");
            send_cmd(DEVICE_CMD_DOWN);
            break;
        case JOYSTICK_EVENT_LEFT:
            LOG_INFO("joystick -> LEFT");
            send_cmd(DEVICE_CMD_LEFT);
            break;
        case JOYSTICK_EVENT_RIGHT:
            LOG_INFO("joystick -> RIGHT");
            send_cmd(DEVICE_CMD_RIGHT);
            break;
        case JOYSTICK_EVENT_PRESS: {
            uint32_t now = get_tick();
            if (s_press_pending &&
                (now - s_press_time_ms) <= DOUBLE_PRESS_WINDOW_MS) {
                /* Second press within window → COIN */
                s_press_pending = false;
                LOG_INFO("joystick -> COIN");
                send_cmd(DEVICE_CMD_COIN);
            } else {
                /* First press — arm timer, defer START */
                s_press_pending  = true;
                s_press_time_ms  = now;
            }
            break;
        }
        default:
            break;
    }
}

/**
 * @brief Main application entry point.
 *
 * Initialization sequence:
 * 1. Configure NVIC priority grouping and update system clock
 * 2. Initialize delay and timer (1 ms tick)
 * 3. Initialize UART (command output) and USB CDC (log output)
 * 4. Initialize joystick driver and event layer
 * 5. Route xprintf log output to USB CDC
 * 6. Subscribe the joystick event handler
 *
 * The main loop calls joystick_events_poll() and device_cmd_poll() every
 * 10 ms; the joystick event layer is time-gated to JOYSTICK_POLL_PERIOD_MS.
 *
 * @return Never returns (infinite loop).
 */
int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
    SystemCoreClockUpdate();

    /* Initialize peripherals */
    delay_init();
    timer_init();      /* TIM1 — 1 ms tick used by joystick poll gating */
    uart_init();
    usb_init();
    joystick_init();
    joystick_events_init();

    xprintf_init(usb_putc, usb_flush);

    joystick_events_subscribe(on_joystick_event, NULL);

    LOG_INFO("System initialized");
    LOG_INFO("Timer tick: %lu ms", get_tick());

    while(1)
    {
        joystick_events_poll();
        device_cmd_poll();
        delay_ms(10);
    }
}
