/**
 ******************************************************************************
 * @file    spi_slave.h
 * @author  snow yang
 * @version v1.0.0
 * @date    2019/07/24
 * @brief   This file provides mxchip SPI APIs.
 ******************************************************************************
 */
#ifndef __SPI_SLAVE_H__
#define __SPI_SLAVE_H__

#include "stdint.h"
#include "merr.h"

/** 
* @addtogroup hal
* @{
*/

/**
* @defgroup spi SPI
* @brief SPI driver APIs
* @{
*/

typedef struct
{
    int miso; /**< MISO pin */
    int mosi; /**< MOSI pin */
    int clk;  /**< Clock pin */
    int cs;   /**< Chip Select pin */
} mhal_spi_slave_pinmux_t;

typedef void (*mhal_spi_slave_callback_t)(void);

/**@brief Open the SPI interface for a given SPI device
 *
 * @note  Prepares a SPI hardware interface for communication as a slave
 *
 * @param   spi     the SPI device to be initialised
 *
 * @retval  kNoErr  on success.
 * @retval  others  if an error occurred with any step
 */
merr_t mhal_spi_slave_open(int spi, mhal_spi_slave_pinmux_t *pinmux);

/**@brief Format the SPI interface for a given SPI device
 *
 * @param   spi     the SPI device to be initialised
 * @param   mode    SPI mode
 *              @arg 0 : [Polarity,Phase]=[0,0]
 *              @arg 1 : [Polarity,Phase]=[0,1]
 *              @arg 2 : [Polarity,Phase]=[1,0]
 *              @arg 3 : [Polarity,Phase]=[1,1]
 * @param   bits    SPI bits
 *
 * @retval  kNoErr  on success.
 * @retval  others  if an error occurred with any step
 */
merr_t mhal_spi_slave_format(int spi, uint8_t mode, uint8_t bits);

merr_t mhal_spi_slave_reg_callback(int spi, mhal_spi_slave_callback_t usr_rx_callback, mhal_spi_slave_callback_t usr_tx_callback);

/**@brief Transmits and/or receives data from a SPI device
 *
 * @param   spi     the SPI device to be initialised
 * @param   buf   ponter to the buffer of data to be sent
 * @param   n     length of buffer
 * @param   timeout timeout in ms
 *
 * @retval  kNoErr  on success
 * @retval  others  if an error occurred with any step
 */
merr_t mhal_spi_slave_write(int spi, uint8_t *buf, uint32_t n, uint32_t timeout);

/**@brief Transmits and/or receives data from a SPI device
 *
 * @param   spi     the SPI device to be initialised
 * @param   buf   ponter to the buffer to store received data
 * @param   n     length of buffer
 * @param   timeout timeout in ms
 *
 * @retval  kNoErr  on success
 * @retval  others  if an error occurred with any step
 */
merr_t mhal_spi_slave_read(int spi, uint8_t *buf, uint32_t n, uint32_t timeout);

/**@brief Transmits and/or receives data from a SPI device
 *
 * @param   spi     the SPI device to be initialised
 * @param   txbuf   ponter to the buffer of data to be sent
 * @param   rxbuf   ponter to the buffer to store received data
 * @param   n     length of buffer
 * @param   timeout timeout in ms
 *
 * @retval  kNoErr  on success
 * @retval  others  if an error occurred with any step
 */
merr_t mhal_spi_slave_read_and_write(int spi, uint8_t *txbuf, uint8_t *rxbuf, uint32_t n, uint32_t timeout);

/**@brief Close a SPI interface
 *
 * @note Turns off a SPI hardware interface
 *
 * @param   spi     the SPI device to be de-initialised
 *
 * @retval  kNoErr  on success.
 * @retval  others  if an error occurred with any step
 */
merr_t mhal_spi_slave_close(int spi);

/** @} */
/** @} */
#endif
