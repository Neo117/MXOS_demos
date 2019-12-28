/**
 ******************************************************************************
 * @file    rtc.h
 * @author  snow yang
 * @mail    yangsw@mxchip.com
 * @version v1.0.0
 * @date    2019/06/21
 * @brief   This file provides mxchip RTC APIs.
 ******************************************************************************
 */
#ifndef __RTC_H__
#define __RTC_H__

#include "stdint.h"
#include "merr.h"

/** 
* @addtogroup hal
* @{
*/

/**
* @defgroup rtc RTC
* @brief RTC driver APIs
* @{
*/

/**
 * @brief RTC interrupt callback handler
 */
typedef void (*mhal_rtc_irq_hanlder_t)(void);

/**
 * @brief   Open RTC device
 *
 * @retval  kNoErr  Success
 * @retval  others  Failure
 */
merr_t mhal_rtc_open(void);

/**
 * @brief   Get time
 * 
 * @return  Seconds since 00:00:00 UTC, January 1, 1970
 */
time_t mhal_rtc_get(void);

/**
 * @brief   Set time
 * 
 * @param   t       Seconds since 00:00:00 UTC, January 1, 1970
 */
void mhal_rtc_set(time_t t);

/**
 * @brief   Set time alarm
 * 
 * @param   t       Seconds since 00:00:00 UTC, January 1, 1970
 * @param   handler Alarm IRQ handler
 */
void mhal_rtc_alarm(time_t t, mhal_rtc_irq_hanlder_t handler);

/**
 * @brief   Close RTC device
 */
void mhal_rtc_close(void);

/** @} */
/** @} */
#endif