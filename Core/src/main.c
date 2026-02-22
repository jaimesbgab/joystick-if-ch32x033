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
 */
typedef enum {
  DEVICE_CMD_UP    = 1,
  DEVICE_CMD_DOWN  = 2,
  DEVICE_CMD_LEFT  = 3,
  DEVICE_CMD_RIGHT = 4,
  DEVICE_CMD_PRESS = 5,
} device_cmd_t;

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
        (uint8_t)cmd > (uint8_t)DEVICE_CMD_PRESS) {
        return;
    }

    uart_putc((char)cmd);
}

/**
 * @brief Joystick event handler — maps events to device commands.
 *
 * Registered via joystick_events_subscribe().  Logs the event name and
 * forwards directional and press events as device_cmd_t bytes over UART.
 * CENTER and RELEASE events are intentionally not forwarded.
 */
static void on_joystick_event(joystick_event_t event, void *user_data)
{
    (void)user_data;
    LOG_INFO("joystick -> %s", joystick_event_name(event));

    switch (event) {
        case JOYSTICK_EVENT_UP:
            send_cmd(DEVICE_CMD_UP);
            break;
        case JOYSTICK_EVENT_DOWN:
            send_cmd(DEVICE_CMD_DOWN);
            break;
        case JOYSTICK_EVENT_LEFT:
            send_cmd(DEVICE_CMD_LEFT);
            break;
        case JOYSTICK_EVENT_RIGHT:
            send_cmd(DEVICE_CMD_RIGHT);
            break;
        case JOYSTICK_EVENT_PRESS:
            send_cmd(DEVICE_CMD_PRESS);
            break;
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
 * The main loop calls joystick_events_poll() every 10 ms; the event
 * layer itself is time-gated to JOYSTICK_POLL_PERIOD_MS (50 ms).
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
        delay_ms(10);
    }
}
