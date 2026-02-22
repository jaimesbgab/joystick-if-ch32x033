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

/* Initialize USB CDC device */
void usb_init(void);

/* Send data to host via CDC bulk IN (EP2) */
usb_status_t usb_write(const uint8_t *data, uint16_t len);

/* Read data received from host via CDC bulk OUT (EP1 ring buffer).
 * Returns number of bytes copied, or negative on error/no data. */
int usb_read(uint8_t *data, uint16_t max_len);

/* Returns the byte count of the next available packet (0 if none) */
uint16_t usb_data_available(void);

/* Returns true if CDC bulk IN (EP2) is free to send */
bool usb_is_ready(void);

/* Returns true if the device is enumerated by the host */
bool usb_is_connected(void);

/* Single-character output for xprintf — buffers and flushes on '\n' or full */
void usb_putc(char c);

/* Flush the internal TX buffer over USB CDC (no-op if nothing buffered) */
void usb_flush(void);

#endif /* __USB_H */
