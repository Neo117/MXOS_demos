/**
 ******************************************************************************
 * @file    module.h
 * @author  Snow Yang
 * @version V1.0.0
 * @date    02-Sept-2019
 * @brief   This file provides current module's pin and partiton define.
 ******************************************************************************
 */

#include "module.h"

/* MXKit Log UART */
#define MXKIT_LOG_UART UART_2
#define MXKIT_LOG_UART_RXD MODULE_PIN_22
#define MXKIT_LOG_UART_TXD MODULE_PIN_21

/* MXKit APP UART */
#define MXKIT_APP_UART UART_0
#define MXKIT_APP_UART_RXD MODULE_PIN_10
#define MXKIT_APP_UART_TXD MODULE_PIN_9
#define MXKIT_APP_UART_RTS MODULE_PIN_7
#define MXKIT_APP_UART_CTS MODULE_PIN_8

/* MXKit Mode Pins */
#define MXKIT_BOOT_PIN MODULE_PIN_19
#define MXKIT_STATUS_PIN MODULE_PIN_NC
#define MXKIT_ELINK_PIN MODULE_PIN_23

/* MXKit Extention Connector */
#define MXKIT_UART MXKIT_APP_UART
#define MXKIT_I2C I2C_0
#define MXKIT_SPI SPI_1

#define MXKIT_RXD MXKIT_APP_UART_RXD
#define MXKIT_TXD MXKIT_APP_UART_TXD

#define MXKIT_D2 MODULE_PIN_NC
#define MXKIT_D3 MODULE_PIN_NC
#define MXKIT_D4 MODULE_PIN_NC
#define MXKIT_D5 MODULE_PIN_NC
#define MXKIT_D6 MODULE_PIN_NC
#define MXKIT_D7 MODULE_PIN_14

#define MXKIT_D8 MODULE_PIN_13
#define MXKIT_D9 MODULE_PIN_12
#define MXKIT_CS MODULE_PIN_8
#define MXKIT_MOSI MODULE_PIN_9
#define MXKIT_MISO MODULE_PIN_7
#define MXKIT_SCK MODULE_PIN_10
#define MXKIT_SDA MODULE_PIN_8
#define MXKIT_SCL MODULE_PIN_7

#define MXKIT_A0 MODULE_PIN_NC
#define MXKIT_A1 MODULE_PIN_NC
#define MXKIT_A2 MODULE_PIN_NC
#define MXKIT_A3 MODULE_PIN_NC
#define MXKIT_A4 MODULE_PIN_NC
#define MXKIT_A5 MODULE_PIN_NC

/* For HAL demo use */
#define MXKIT_PWM PWM_3
#define MXKIT_PWM_PIN MXKIT_D8