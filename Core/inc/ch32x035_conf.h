/********************************** (C) COPYRIGHT *******************************
 * File Name          : ch32x035_conf.h
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2023/04/06
 * Description        : Library configuration file.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#ifndef __CH32X035_CONF_H
#define __CH32X035_CONF_H

// MCU supply voltage
#define USB_VDD             1         // 0: 3.3V, 1: 5V

// USB device descriptor
#define USB_VENDOR_ID       0x16C0    // VID (shared www.voti.nl)
#define USB_PRODUCT_ID      0x27DD    // PID (shared CDC-ACM)
#define USB_DEVICE_VERSION  0x0100    // v1.0 (BCD-format)
#define USB_LANGUAGE        0x0409    // US English

// USB configuration descriptor
#define USB_MAX_POWER_mA    50        // max power in mA 

// USB descriptor strings
#define MANUF_STR           "WCH"
#define PROD_STR            "CH32X035-USB"
#define SERIAL_STR          "CH32X035CDC"
#define INTERF_STR          "CDC-Serial"

#include "ch32x035.h"
#include "ch32x035_adc.h"
//#include "ch32x035_awu.h"
//#include "ch32x035_dbgmcu.h"
//#include "ch32x035_dma.h"
//#include "ch32x035_exti.h"
//#include "ch32x035_flash.h"
#include "ch32x035_gpio.h"
#include "ch32x035_i2c.h"
//#include "ch32x035_iwdg.h"
#include "ch32x035_pwr.h"
#include "ch32x035_rcc.h"
#include "ch32x035_spi.h"
#include "ch32x035_tim.h"
#include "ch32x035_usart.h"
//#include "ch32x035_wwdg.h"
#include "ch32x035_it.h"
#include "ch32x035_misc.h"

#endif
