/**
 ******************************************************************************
 * @file    i2c.h
 * @author  snow yang
 * @mail    yangsw@mxchip.com
 * @version v1.0.0
 * @date    2019/06/21
 * @brief   This file provides mxchip I2C APIs.
 ******************************************************************************
 */
#ifndef __I2C_H__
#define __I2C_H__

#include "stdint.h"
#include "merr.h"

/** 
 * @addtogroup hal
 * @{
 */

/**
 * @defgroup i2c I2C
 * @brief I2C driver APIs
 * @{
 */

/**
 * @brief I2C address width
 */
typedef enum
{
    I2C_ADDR_WIDTH_7BIT,  /**< 7  bit address width */
    I2C_ADDR_WIDTH_10BIT, /**< 10 bit address width */
    I2C_ADDR_WIDTH_16BIT  /**< 16 bit address width */
} mhal_i2c_addr_width_t;

typedef struct
{
    int sda; /**< SDA pin */
    int scl; /**< SCL pin */
} mhal_i2c_pinmux_t;

/**
 * @brief   Open an I2C device
 * 
 * @param   i2c     I2C name
 * @param   width   Address width
 * @param   freq    Frequency in Hz
 * @param   pinmux  Pinmux to set SDA and SCL
 * 
 * @retval  kNoErr  Success
 * @retval  others  Failure
 */
merr_t mhal_i2c_open(int i2c, mhal_i2c_addr_width_t width, int freq, mhal_i2c_pinmux_t *pinmux);

/**
 * @brief   Write data on I2c bus
 * 
 * @param   i2c     I2C name
 * @param   addr    Slave device address
 * @param   buf     Point to the data buffer that will be written on I2C bus
 * @param   n       The length of the buffer in bytes
 * 
 * @retval  kNoErr  Success
 * @retval  others  Failure
 */
merr_t mhal_i2c_write(int i2c, uint8_t addr, uint8_t *buf, uint32_t n);

/**
 * @brief   Read data on I2c bus
 * 
 * @param   i2c     I2C name
 * @param   addr    Slave device address
 * @param   buf     Point to the data buffer that will be read on I2C bus
 * @param   n       The length of the buffer in bytes
 * 
 * @retval  kNoErr  Success
 * @retval  others  Failure
 */
merr_t mhal_i2c_read(int i2c, uint8_t addr, uint8_t *buf, uint32_t n);

/**
 * @brief   Close an I2C device
 * 
 * @param   i2c     I2C name
 */
void mhal_i2c_close(int i2c);

/** 
 * @} 
 */
/** 
 * @} 
 */

#endif
