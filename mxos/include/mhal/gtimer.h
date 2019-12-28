/**
 ******************************************************************************
 * @file    gtimer.h
 * @author  snow yang
 * @mail    yangsw@mxchip.com
 * @version v1.0.0
 * @date    2019/06/21
 * @brief   This file provides mxchip TIMER APIs.
 ******************************************************************************
 */
#ifndef __GTIMER_H__
#define __GTIMER_H__

#include "stdint.h"
#include "merr.h"

/** 
* @addtogroup hal
* @{
*/

/**
* @defgroup gtimer TIMER
* @brief TIMER driver APIs
* @{
*/

/**
 * @brief Timer mode
 */
typedef enum
{
    ONE_SHOT,
    PERIOIC,
} mhal_gtimer_mode_t;

/**
 * @brief timer interrupt callback handler
 */
typedef void (*mhal_gtimer_irq_callback_t)(void *arg);

/**
 * @brief   Open a hardware timer
 * 
 * @param   timer   The timer for which the hardware timer should be initialised
 * @param   mode    The mode for which the timer reload
 * @param   time    Timeout in us
 * @param   cb      The callback function which the timer
 * @param   arg  
 * 
 * @retval  kNoErr  Success
 * @retval  others  Failure
 */
merr_t mhal_gtimer_open(int timer, mhal_gtimer_mode_t mode, uint32_t time, mhal_gtimer_irq_callback_t cb, void *arg);

/**
 * @brief   Start a hardware timer
 * 
 * @param   timer   The timer for which the hardware timer should be start
 */
void mhal_gtimer_start(int timer);

/**
 * @brief   Stop a hardware timer
 * 
 * @param   timer   The timer for which the hardware timer should be stop
 * 
 * @retval  kNoErr  success
 * @retval  others  failure
 */
void mhal_gtimer_stop(int timer);

/**
 * @brief   Close a hardware timer
 * 
 * @param   timer   The timer for which the hardware timer should be closed
 * 
 * @retval  kNoErr  success
 * @retval  others  failure
 */
void mhal_gtimer_close(int timer);

/** @} */
/** @} */
#endif