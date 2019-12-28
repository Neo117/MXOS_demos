/**
 ******************************************************************************
 * @file    adc.h
 * @author  snow yang
 * @mail    yangsw@mxchip.com
 * @version v1.0.0
 * @date    2019/06/21
 * @brief   This file provides mxchip ADC APIs.
 * 
 ******************************************************************************
 */

#ifndef __ADC_H__
#define __ADC_H__

#include "stdint.h"
#include "merr.h"

/** 
* @addtogroup hal
* @{
*/

/**
* @defgroup adc ADC
* @brief ADC driver APIs
* @{
*/

/**
 * @brief   Open an ADC device
 * 
 * @param   adc     ADC device name
 * @param   pin     Pin name of ADC device
 *
 * @retval  kNoErr  Success
 * @retval  others  Failure
 */
merr_t mhal_adc_open(int adc, int pin);

/**
 * @brief   Read voltage value at an ADC device
 * 
 * @param   adc     ADC device name
 * 
 * @return  Voltage value in Volt
 */
float mhal_adc_read(int adc);

/**
 * @brief   Close an ADC device
 * 
 * @param   adc     ADC device name
 */
void mhal_adc_close(int adc);

/** @} */
/** @} */

#endif
