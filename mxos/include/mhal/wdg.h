/**
 ******************************************************************************
 * @file    wdg.h
 * @author  snow yang
 * @mail    yangsw@mxchip.com
 * @version v1.0.0
 * @date    2019/06/21
 * @brief   This file provides mxchip WDG APIs.
 ******************************************************************************
 */
#ifndef __WDG_H__
#define __WDG_H__

#include "stdint.h"
#include "merr.h"

/** 
 * @addtogroup hal
 * @{
 */

/**
 * @defgroup wdg WDG
 * @brief WDG driver APIs
 * @{
 */

/**
 * @brief   Open watchdog device
 * 
 * @param   timeout Timeout in ms
 *
 * @retval  kNoErr  Success
 * @retval  others  Failure
 */
merr_t mhal_wdg_open(uint32_t timeout);

/**
 * @brief   Reload watchdog counter.
 */
void mhal_wdg_feed(void);

/**
 * @brief   Close watchdog device
 */
void mhal_wdg_close(void);

/** @} */
/** @} */
#endif