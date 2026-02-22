/********************************** (C) COPYRIGHT *******************************
 * File Name          : usb_desc.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2024/04/16
 * Description        : USB CDC-ACM device descriptor, configuration descriptor,
 *                      string descriptors and other descriptors.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

#include "usb_desc.h"

/* Device Descriptor */
const uint8_t  MyDevDescr[] =
{
    0x12,                                                // bLength
    0x01,                                                // bDescriptorType (Device)
    0x10, 0x01,                                          // bcdUSB 1.10
    0x02,                                                // bDeviceClass (CDC)
    0x00,                                                // bDeviceSubClass
    0x00,                                                // bDeviceProtocol
    DEF_USBD_UEP0_SIZE,                                  // bMaxPacketSize0
    (uint8_t)DEF_USB_VID, (uint8_t)(DEF_USB_VID >> 8),  // idVendor 0x1A86
    (uint8_t)DEF_USB_PID, (uint8_t)(DEF_USB_PID >> 8),  // idProduct 0xFE07
    0x00, DEF_IC_PRG_VER,                                // bcdDevice 1.00
    0x01,                                                // iManufacturer (String Index)
    0x02,                                                // iProduct (String Index)
    0x03,                                                // iSerialNumber (String Index)
    0x01,                                                // bNumConfigurations 1
};

/*
 * Configuration Descriptor
 *
 * Total length = 9 (Config) + 9 (IF0 CDC Control) + 5 (Header) + 4 (ACM) +
 *                5 (Union) + 5 (Call Mgmt) + 7 (EP3 IN Interrupt) +
 *                9 (IF1 CDC Data) + 7 (EP1 OUT Bulk) + 7 (EP2 IN Bulk)
 *              = 67 = 0x43
 */
const uint8_t  MyCfgDescr[] =
{
    /* Configuration Descriptor (9 bytes) */
    0x09,                           // bLength
    0x02,                           // bDescriptorType (Configuration)
    0x43, 0x00,                     // wTotalLength 67
    0x02,                           // bNumInterfaces 2
    0x01,                           // bConfigurationValue
    0x00,                           // iConfiguration
    0x80,                           // bmAttributes: Bus-powered
    0x32,                           // bMaxPower 100mA

    /* Interface 0 - CDC Control Interface (9 bytes) */
    0x09,                           // bLength
    0x04,                           // bDescriptorType (Interface)
    0x00,                           // bInterfaceNumber 0
    0x00,                           // bAlternateSetting
    0x01,                           // bNumEndpoints 1 (notification)
    0x02,                           // bInterfaceClass: CDC
    0x02,                           // bInterfaceSubClass: ACM
    0x01,                           // bInterfaceProtocol: AT commands
    0x00,                           // iInterface

    /* CDC Header Functional Descriptor (5 bytes) */
    0x05,                           // bLength
    0x24,                           // bDescriptorType: CS_INTERFACE
    0x00,                           // bDescriptorSubType: Header
    0x10, 0x01,                     // bcdCDC 1.10

    /* CDC ACM Functional Descriptor (4 bytes) */
    0x04,                           // bLength
    0x24,                           // bDescriptorType: CS_INTERFACE
    0x02,                           // bDescriptorSubType: ACM
    0x02,                           // bmCapabilities: supports Set/GetLineCoding and SetControlLineState

    /* CDC Union Functional Descriptor (5 bytes) */
    0x05,                           // bLength
    0x24,                           // bDescriptorType: CS_INTERFACE
    0x06,                           // bDescriptorSubType: Union
    0x00,                           // bMasterInterface: 0 (CDC Control)
    0x01,                           // bSlaveInterface0: 1 (CDC Data)

    /* CDC Call Management Functional Descriptor (5 bytes) */
    0x05,                           // bLength
    0x24,                           // bDescriptorType: CS_INTERFACE
    0x01,                           // bDescriptorSubType: Call Management
    0x00,                           // bmCapabilities: device handles call management
    0x01,                           // bDataInterface: 1

    /* Endpoint 3 IN Interrupt - CDC Notification (7 bytes) */
    0x07,                           // bLength
    0x05,                           // bDescriptorType (Endpoint)
    0x83,                           // bEndpointAddress: IN EP3
    0x03,                           // bmAttributes: Interrupt
    0x08, 0x00,                     // wMaxPacketSize 8
    0xFF,                           // bInterval: 255ms

    /* Interface 1 - CDC Data Interface (9 bytes) */
    0x09,                           // bLength
    0x04,                           // bDescriptorType (Interface)
    0x01,                           // bInterfaceNumber 1
    0x00,                           // bAlternateSetting
    0x02,                           // bNumEndpoints 2
    0x0A,                           // bInterfaceClass: CDC Data
    0x00,                           // bInterfaceSubClass
    0x00,                           // bInterfaceProtocol
    0x00,                           // iInterface

    /* Endpoint 1 OUT Bulk - Data RX from host (7 bytes) */
    0x07,                           // bLength
    0x05,                           // bDescriptorType (Endpoint)
    0x01,                           // bEndpointAddress: OUT EP1
    0x02,                           // bmAttributes: Bulk
    0x40, 0x00,                     // wMaxPacketSize 64
    0x00,                           // bInterval (ignored for bulk)

    /* Endpoint 2 IN Bulk - Data TX to host (7 bytes) */
    0x07,                           // bLength
    0x05,                           // bDescriptorType (Endpoint)
    0x82,                           // bEndpointAddress: IN EP2
    0x02,                           // bmAttributes: Bulk
    0x40, 0x00,                     // wMaxPacketSize 64
    0x00,                           // bInterval (ignored for bulk)
};

/* Language Descriptor */
const uint8_t  MyLangDescr[] =
{
    0x04, 0x03, 0x09, 0x04
};

/* Manufacturer Descriptor */
const uint8_t  MyManuInfo[] =
{
    0x0E, 0x03, 'w', 0, 'c', 0, 'h', 0, '.', 0, 'c', 0, 'n', 0
};

/* Product Information */
const uint8_t  MyProdInfo[] =
{
    0x12, 0x03, 'C', 0, 'H', 0, '3', 0, '2', 0, 'x', 0, '0', 0, '3', 0, '5', 0
};

/* Serial Number Information */
const uint8_t  MySerNumInfo[] =
{
    0x16, 0x03, '0', 0, '1', 0, '2', 0, '3', 0, '4', 0, '5', 0
              , '6', 0, '7', 0, '8', 0, '9', 0
};
