/**
 * @file main.c
 * @brief Main application entry point for CH32X035
 *
 * This file contains the main application code that initializes peripherals
 * and runs the main program loop with LED toggling and logging functionality.
 */

#include "ch32x035_conf.h"

#include "delay.h"
#include "timer.h"
#include "gpio.h"
#include "usb.h"
#include "joystick.h"
#include "log.h"

/**
 * @brief Main application entry point
 *
 * Initializes the system and peripherals, then enters an infinite loop
 * that toggles the running LED and outputs log messages every 500ms.
 *
 * Initialization sequence:
 * 1. Configure NVIC priority grouping
 * 2. Update system core clock
 * 3. Initialize delay functions
 * 4. Initialize UART for logging
 * 5. Initialize GPIO for LED control
 * 6. Configure xprintf output
 *
 * @return Never returns (infinite loop)
 */
int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
    SystemCoreClockUpdate();

    /* Initizalize Peripherals */
    delay_init();
    timer_init();      // Initialize TIM1 for 1ms tick (required for I2C timeouts)
    gpio_init();
    usb_init();
    joystick_init();

    xprintf_init(usb_putc, usb_flush);

    LOG_INFO("System initialized");
    LOG_INFO("Timer tick: %lu ms", get_tick());

    while(1)
    {
        joystick_poll();
        delay_ms(10);
    }
}
