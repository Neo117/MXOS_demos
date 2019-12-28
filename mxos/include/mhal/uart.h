/**
 ******************************************************************************
 * @file    uart.h
 * @author  snow yang
 * @mail    yangsw@mxchip.com
 * @version v1.0.0
 * @date    2019/06/21
 * @brief   This file provides mxchip UART APIs.
 ******************************************************************************
 */

#ifndef __UART_H__
#define __UART_H__

#include "stdint.h"
#include "merr.h"

/** 
* @addtogroup hal
* @{
*/

/**
* @defgroup uart UART
* @brief UART driver APIs
* @{
*/

enum
{
    DATA_WIDTH_5BIT, /**< 5 bit data width */
    DATA_WIDTH_6BIT, /**< 6 bit data width */
    DATA_WIDTH_7BIT, /**< 7 bit data width */
    DATA_WIDTH_8BIT, /**< 8 bit data width */
    DATA_WIDTH_9BIT  /**< 9 bit data width */
};
/**
 * @brief UART data width
 */
typedef uint8_t mhal_uart_data_width_t;

enum
{
    STOP_BITS_1, /**< 1 stop bits */
    STOP_BITS_2, /**< 2 stop bits */
};
/**
 * @brief UART stop bits
 */
typedef uint8_t mhal_uart_stop_bits_t;

enum
{
    FLOW_CONTROL_DISABLED, /**< Flow control disabled */
    FLOW_CONTROL_CTS,      /**< Flow contrl CTS */
    FLOW_CONTROL_RTS,      /**< Flow contrl RTS */
    FLOW_CONTROL_CTS_RTS   /**< Flow contrl CTS and RTS */
};
/**
 * @brief UART flow control
 */
typedef uint8_t mhal_uart_flow_control_t;

enum
{
    NO_PARITY,   /**< No parity */
    ODD_PARITY,  /**< Odd parity */
    EVEN_PARITY, /**< Even parity */
};
/**
 * @brief UART parity
 */
typedef uint8_t mhal_uart_parity_t;

/**
 * @brief UART configuration
 */
typedef struct
{
    uint32_t baudrate;                     /**< Baudrate */
    mhal_uart_data_width_t data_width;     /**< Date width */
    mhal_uart_parity_t parity;             /**< Parity */
    mhal_uart_stop_bits_t stop_bits;       /**< Stop bits */
    mhal_uart_flow_control_t flow_control; /**< Flow control */
    uint32_t buffersize;                   /**< Receive buffer size */
} mhal_uart_config_t;

typedef struct
{
    int tx;  /**< TXD pin */
    int rx;  /**< TXD pin */
    int rts; /**< RTS pin */
    int cts; /**< CTS pin */
} mhal_uart_pinmux_t;

/**
 * @brief Callback function to receive an UART byte.
 * 
 * @note register this callback function at mhal_uart_open_irq_mode
 * @param  data       The received uart data
 * 
 */
typedef void (*uart_input_byte_t)(uint8_t data);

/**
 * @brief   Open a UART device
 * 
 * @param   uart        UART name
 * @param   config      UART configuration structure
 * @param   pinmux      Pinmux to set TXD, RXD, RTS and CTS
 * 
 * @retval  kNoErr      Success
 * @retval  others      Failure
 */
merr_t mhal_uart_open(int uart, const mhal_uart_config_t *config, mhal_uart_pinmux_t *pinmux);

/**
 * @brief   Close a UART device
 * 
 * @param   uart        UART name
 */
void mhal_uart_close(int uart);

/**
 * @brief   Transmit data on a UART interface
 *
 * @param   uart        UART name
 * @param   buf         Pointer to the buffer of data to be sent
 * @param   n           Length of the data to be sent
 */
void mhal_uart_write(int uart, const void *buf, uint32_t n);

/**
 * @brief   Read data on a UART bus
 * 
 * @note    This function will store the received data into buffer evenif 
 *          it is less than expected length
 *
 * @param   uart        UART name
 * @param   buf         Point to the buffer which will store incoming data
 * @param   n           Point to number that expected to read and point to the 
 *                      actually received length after this function is returned
 * @param   timeout     Timeout in millisecond
 *  
 * @retval  kNoErr      Success
 * @retval  others      Failure
 */
merr_t mhal_uart_read(int uart, void *buf, uint32_t *n, uint32_t timeout);

/**
 * @brief   Read data on a UART bus
 *
 * @note    This function will not store the read data into buffer untill
 *          the exepected length of data was received
 * 
 * @param   uart        UART name
 * @param   buf         Pointer to the buffer which will store incoming data
 * @param   n           Number of bytes to receive
 * @param   timeout     Timeout in millisecond
 *  
 * @retval  kNoErr      Success
 * @retval  others      Failure
 */
merr_t mhal_uart_read_buf(int uart, void *buf, uint32_t n, uint32_t timeout);

/**
 * @brief   Get the length of the data that is already received
 * 
 * @param   uart        UART name
 *
 * @return  Received data length
 */
uint32_t mhal_uart_recved_len(int uart);
/** 
 * @} 
 */
/** 
 * @} 
 */

#endif
