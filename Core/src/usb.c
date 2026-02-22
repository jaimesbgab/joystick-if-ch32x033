#include "usb.h"

#include "ch32x035_conf.h"
#include <ch32x035_usbfs_device.h>
#include <usb_desc.h>
#include <string.h>

void usb_init(void) {
    USBFS_RCC_Init();
    USBFS_Device_Init(ENABLE, PWR_VDD_SupplyVoltage());
}

usb_status_t usb_write(const uint8_t *data, uint16_t len) {
    if (data == NULL || len == 0) {
        return USB_ERROR;
    }

    if (len > DEF_USB_EP2_FS_SIZE) {
        return USB_ERROR;
    }

    if (USBFS_Endp_Busy[DEF_UEP2]) {
        return USB_BUSY;
    }

    memcpy(USBFS_EP2_Buf, data, len);

    USBFS_Endp_Busy[DEF_UEP2] = 1;
    USBFSD->UEP2_TX_LEN = len;
    USBFSD->UEP2_CTRL_H = (USBFSD->UEP2_CTRL_H & ~USBFS_UEP_T_RES_MASK) | USBFS_UEP_T_RES_ACK;

    return USB_OK;
}

int usb_read(uint8_t *data, uint16_t max_len) {
    if (data == NULL || max_len == 0) {
        return USB_ERROR;
    }

    if (RingBuffer_Comm.RemainPack == 0) {
        return USB_NO_DATA;
    }

    uint16_t pack_len = RingBuffer_Comm.PackLen[RingBuffer_Comm.DealPtr];

    if (pack_len > max_len) {
        return USB_ERROR;
    }

    memcpy(data, &Data_Buffer[RingBuffer_Comm.DealPtr * DEF_USBD_FS_PACK_SIZE], pack_len);

    RingBuffer_Comm.DealPtr++;
    if (RingBuffer_Comm.DealPtr == DEF_Ring_Buffer_Max_Blks) {
        RingBuffer_Comm.DealPtr = 0;
    }

    RingBuffer_Comm.RemainPack--;

    /* Resume reception if it was flow-controlled */
    if (RingBuffer_Comm.StopFlag && (RingBuffer_Comm.RemainPack < DEF_RING_BUFFER_RESTART)) {
        RingBuffer_Comm.StopFlag = 0;
        USBFSD->UEP1_CTRL_H = (USBFSD->UEP1_CTRL_H & ~USBFS_UEP_R_RES_MASK) | USBFS_UEP_R_RES_ACK;
    }

    return (int)pack_len;
}

uint16_t usb_data_available(void) {
    if (RingBuffer_Comm.RemainPack == 0) {
        return 0;
    }
    return RingBuffer_Comm.PackLen[RingBuffer_Comm.DealPtr];
}

bool usb_is_ready(void) {
    return (USBFS_Endp_Busy[DEF_UEP2] == 0);
}

bool usb_is_connected(void) {
    return (USBFS_DevEnumStatus == 0x01);
}

/* Internal TX buffer for xprintf character-by-character output */
static uint8_t usb_log_buf[DEF_USB_EP2_FS_SIZE];
static uint8_t usb_log_len = 0;

void usb_flush(void) {
    if (usb_log_len == 0) return;
    if (!usb_is_connected()) {
        usb_log_len = 0;
        return;
    }
    /* Spin-wait for endpoint — USB ISR will clear the busy flag */
    uint32_t t = 50000;
    while (USBFS_Endp_Busy[DEF_UEP2] && --t);
    if (!USBFS_Endp_Busy[DEF_UEP2]) {
        usb_write(usb_log_buf, usb_log_len);
    }
    usb_log_len = 0;
}

void usb_putc(char c) {
    usb_log_buf[usb_log_len++] = (uint8_t)c;
    if (usb_log_len >= sizeof(usb_log_buf) || c == '\n') {
        usb_flush();
    }
}
