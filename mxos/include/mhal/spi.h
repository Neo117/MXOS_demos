/**
 ******************************************************************************
 * @file    spi.h
 * @author  snow yang
 * @mail    yangsw@mxchip.com
 * @version v1.0.0
 * @date    2019/06/21
 * @brief   This file provides mxchip SPI APIs.
 ******************************************************************************
 */
#ifndef __SPI_H__
#define __SPI_H__

#include "stdint.h"
#include "merr.h"

/* SPI mode constants */
#define SPI_CLOCK_RISING_EDGE (1 << 0)
#define SPI_CLOCK_FALLING_EDGE (0 << 0)
#define SPI_CLOCK_IDLE_HIGH (1 << 1)
#define SPI_CLOCK_IDLE_LOW (0 << 1)
#define SPI_USE_DMA (1 << 2)
#define SPI_NO_DMA (0 << 2)
#define SPI_MSB_FIRST (1 << 3)
#define SPI_LSB_FIRST (0 << 3)

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
} mhal_spi_pinmux_t;

/**@brief   Open a SPI device
 *
 * @param   spi     SPI name
 * @param   pinmux  Pinmux to set MISO, MOSI and CLK
 *
 * @retval  kNoErr  Success
 * @retval  others  Failure
 */
merr_t mhal_spi_open(int spi, mhal_spi_pinmux_t *pinmux);

/**@brief   Format SPI device
 *
 * @param   spi     SPI name
 * @param   freq    Frequency in Hz
 * @param   mode    SPI mode
 *                  @arg 0 : [Polarity,Phase]=[0,0]
 *                  @arg 1 : [Polarity,Phase]=[0,1]
 *                  @arg 2 : [Polarity,Phase]=[1,0]
 *                  @arg 3 : [Polarity,Phase]=[1,1]
 * @param   bits    Data bits
 */
void mhal_spi_format(int spi, uint32_t freq, uint8_t mode, uint8_t bits);

/**@brief   Write and read data on SPI bus
 *
 * @param   spi     The SPI device to be initialised
 * @param   txbuf   Pointer to the buffer of data to be sent
 * @param   rxbuf   Pointer to the buffer to store received data
 * @param   n       Length of the buffer
 */
void mhal_spi_write_and_read(int spi, uint8_t *txbuf, uint8_t *rxbuf, uint32_t n);

/**@brief   Close a SPI device
 * 
 * @param   spi     SPI name
 */
void mhal_spi_close(int spi);

/** @} */
/** @} */
#endif
