#include "usb.h"

#include "ch32x035_conf.h"
#include <ch32x035_usbfs_device.h>
#include <usb_desc.h>
#include <string.h>

/* External variables from driver */
extern volatile uint8_t USBFS_Endp_Busy[];
extern volatile uint8_t USBFS_DevEnumStatus;
extern RING_BUFF_COMM RingBuffer_Comm;
extern uint8_t Data_Buffer[];
extern uint8_t USBFS_EP2_Buf[];
extern uint8_t USBFS_EP3_Buf[];

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

  /* Check if endpoint is busy */
  if (USBFS_Endp_Busy[DEF_UEP2]) {
    return USB_BUSY;
  }

  /* Copy data to EP2 buffer */
  memcpy(USBFS_EP2_Buf, data, len);

  /* Set endpoint busy flag */
  USBFS_Endp_Busy[DEF_UEP2] = 1;

  /* Set transmit length and trigger transmission */
  USBFSD->UEP2_TX_LEN = len;
  USBFSD->UEP2_CTRL_H = (USBFSD->UEP2_CTRL_H & ~USBFS_UEP_T_RES_MASK) | USBFS_UEP_T_RES_ACK;

  return USB_OK;
}

usb_status_t usb_send_event(const uint8_t *data, uint16_t len) {
  if (data == NULL || len == 0) {
    return USB_ERROR;
  }

  if (len > DEF_USB_EP3_FS_SIZE) {
    return USB_ERROR;
  }

  /* Check if endpoint is busy */
  if (USBFS_Endp_Busy[DEF_UEP3]) {
    return USB_BUSY;
  }

  /* Copy data to EP3 buffer */
  memcpy(USBFS_EP3_Buf, data, len);

  /* Set endpoint busy flag */
  USBFS_Endp_Busy[DEF_UEP3] = 1;

  /* Set transmit length and trigger transmission */
  USBFSD->UEP3_TX_LEN = len;
  USBFSD->UEP3_CTRL_H = (USBFSD->UEP3_CTRL_H & ~USBFS_UEP_T_RES_MASK) | USBFS_UEP_T_RES_ACK;

  return USB_OK;
}

int usb_read(uint8_t *data, uint16_t max_len) {
  if (data == NULL || max_len == 0) {
    return USB_ERROR;
  }

  /* Check if data is available in ring buffer */
  if (RingBuffer_Comm.RemainPack == 0) {
    return USB_NO_DATA;
  }

  /* Get packet length from ring buffer */
  uint16_t pack_len = RingBuffer_Comm.PackLen[RingBuffer_Comm.DealPtr];

  if (pack_len > max_len) {
    return USB_ERROR;
  }

  /* Copy data from ring buffer */
  memcpy(data, &Data_Buffer[RingBuffer_Comm.DealPtr * DEF_USBD_FS_PACK_SIZE], pack_len);

  /* Update ring buffer pointer */
  RingBuffer_Comm.DealPtr++;
  if (RingBuffer_Comm.DealPtr == DEF_Ring_Buffer_Max_Blks) {
    RingBuffer_Comm.DealPtr = 0;
  }

  /* Decrement remaining packet count */
  RingBuffer_Comm.RemainPack--;

  /* Resume reception if it was stopped */
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

  /* Return the length of the next packet */
  return RingBuffer_Comm.PackLen[RingBuffer_Comm.DealPtr];
}

bool usb_is_ready(void) {
  return (USBFS_Endp_Busy[DEF_UEP2] == 0);
}

bool usb_event_is_ready(void) {
  return (USBFS_Endp_Busy[DEF_UEP3] == 0);
}

bool usb_is_connected(void) {
  /* Device is connected and enumerated when DevEnumStatus is set to 0x01 */
  return (USBFS_DevEnumStatus == 0x01);
}