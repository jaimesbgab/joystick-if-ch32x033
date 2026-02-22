#ifndef __USB_H
#define __USB_H

#include <stdint.h>
#include <stdbool.h>

/* USB API Return Codes */
typedef enum {
    USB_OK = 0,
    USB_ERROR = -1,
    USB_BUSY = -2,
    USB_NO_DATA = -3
} usb_status_t;

/* Initialize USB device */
void usb_init(void);

/* Send data on EP2 (main data endpoint) */
usb_status_t usb_write(const uint8_t *data, uint16_t len);

/* Send event/periodic data on EP3 */
usb_status_t usb_send_event(const uint8_t *data, uint16_t len);

/* Read data from EP1 (returns number of bytes read, or negative on error) */
int usb_read(uint8_t *data, uint16_t max_len);

/* Check if data is available to read from EP1 */
uint16_t usb_data_available(void);

/* Check if USB is ready to send on EP2 */
bool usb_is_ready(void);

/* Check if USB is ready to send on EP3 */
bool usb_event_is_ready(void);

/* Check if USB is connected and enumerated */
bool usb_is_connected(void);

#endif /* __USB_H */