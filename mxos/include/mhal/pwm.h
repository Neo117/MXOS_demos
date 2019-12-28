/**
 ******************************************************************************
 * @file    pwm.h
 * @author  snow yang
 * @mail    yangsw@mxchip.com
 * @version v1.0.0
 * @date    2019/06/21
 * @brief   This file provides mxchip PWM APIs.
 ******************************************************************************
 */
#ifndef __PWM_H__
#define __PWM_H__

#include "stdint.h"
#include "merr.h"

/** 
* @addtogroup hal
* @{
*/

/**
* @defgroup pwm PWM
* @brief PWM driver APIs
* @{
*/

/**
 * @brief   Open a PWM device
 *
 * @param   pwm         PWM name
 * @param   polarity    Polarity
 *                      @arg 0  Idle low
 *                      @arg 1  Idle high
 * @param   pin         Pin name of PWM device
 *
 * @retval  kNoErr      Success
 * @retval  others      Failure
 */
merr_t mhal_pwm_open(int pwm, int polarity, int pin);

/**
 * @brief   Set PWM frequency
 * 
 * @param   pwm     PWM name
 * @param   freq    Frequency in Hz
 */
void mhal_pwm_set_freq(int pwm, uint32_t freq);

/**
 * @brief   Set PWM duty cycle
 * 
 * @param   pwm     PWM name
 * @param   duty    Duty cycle of signal as a floating-point percentage (0.0 to 100.0)
 */
void mhal_pwm_set_duty(int pwm, float duty);

/**
 * @brief   Close a PWM pin
 *
 * @param   pwm     The PWM interface which should be initialised
 */
void mhal_pwm_close(int pwm);

/** @} */
/** @} */
#endif
