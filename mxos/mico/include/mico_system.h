/**
 ******************************************************************************
 * @file    mico_system.h
 * @author  William Xu
 * @version V1.0.0
 * @date    11-Aug-2015
 * @brief  This file provides all the headers for MiCO system functions, this is
 *         a application framework based on MiCO core APIs
 ******************************************************************************
 *
 *  The MIT License
 *  Copyright (c) 2014 MXCHIP Inc.
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is furnished
 *  to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 *  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR
 *  IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 ******************************************************************************
 */

#pragma once

#include "mxos_system.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef system_state_t          mico_system_state_t;
typedef system_config_t         mico_Context_t;
typedef system_status_wlan_t    mico_system_status_wlan_t;

#define mico_sdk_version mxos_sdk_version
#define mico_app_info mxos_app_info
#define mico_ota_switch_to_new_fw mxos_ota_switch_to_new_fw
#define mico_system_delegate_config_will_start mxos_system_delegate_config_will_start
#define mico_system_delegate_config_will_stop mxos_system_delegate_config_will_stop
#define mico_system_delegate_config_recv_ssid mxos_system_delegate_config_recv_ssid

typedef mxos_system_monitor_t mico_system_monitor_t;
#define mico_system_monitor_daemen_start mxos_system_monitor_daemen_start
#define mico_system_monitor_register mxos_system_monitor_register
#define mico_system_monitor_update mxos_system_monitor_update
#define mico_system_power_perform mxos_system_power_perform

/** @brief MICO system defined notifications */ 
typedef enum{
  mico_notify_WIFI_STATUS_CHANGED = mxos_notify_WIFI_STATUS_CHANGED,
  mico_notify_WiFI_PARA_CHANGED = mxos_notify_WiFI_PARA_CHANGED,
  mico_notify_EASYLINK_WPS_COMPLETED = mxos_notify_EASYLINK_WPS_COMPLETED,
  mico_notify_EASYLINK_GET_EXTRA_DATA = mxos_notify_EASYLINK_GET_EXTRA_DATA,
  mico_notify_TCP_CLIENT_CONNECTED = mxos_notify_TCP_CLIENT_CONNECTED,
  mico_notify_DNS_RESOLVE_COMPLETED = mxos_notify_DNS_RESOLVE_COMPLETED,
  mico_notify_SYS_WILL_POWER_OFF = mxos_notify_SYS_WILL_POWER_OFF,
  mico_notify_WIFI_CONNECT_FAILED = mxos_notify_WIFI_CONNECT_FAILED,
  mico_notify_WIFI_Fatal_ERROR = mxos_notify_WIFI_Fatal_ERROR,
  mico_notify_Stack_Overflow_ERROR = mxos_notify_Stack_Overflow_ERROR,
  mico_notify_GPRS_STATUS_CHANGED = mxos_notify_GPRS_STATUS_CHANGED,
  mico_notify_WIFI_SCAN_COMPLETED,
  mico_notify_WIFI_SCAN_ADV_COMPLETED,
  mico_notify_DHCP_COMPLETED,
  mico_notify_MAX = mxos_notify_MAX,
 
} mico_notify_types_t;

typedef mxos_time_t mico_time_t ;
typedef mxos_utc_time_t mico_utc_time_t;
typedef mxos_utc_time_ms_t mico_utc_time_ms_t;

#define mico_time_get_time mxos_time_get_time
#define mico_time_set_time mxos_time_set_time
#define mico_time_get_utc_time mxos_time_get_utc_time
#define mico_time_get_utc_time_ms mxos_time_get_utc_time_ms
#define mico_time_set_utc_time_ms mxos_time_set_utc_time_ms
#define mico_time_get_iso8601_time mxos_time_get_iso8601_time
#define mico_time_convert_utc_ms_to_iso8601 mxos_time_convert_utc_ms_to_iso8601

/**
  * @brief  Initialize core data used by MiCO system framework. System and 
  *         application's configuration are read from non-volatile storage: 
  *         flash etc.
  * @param  size_of_user_data: The length of config data used by application
  * @retval Address of core data.
  */
void* mico_system_context_init( uint32_t size_of_user_data );

/**
  * @brief  Get the address of the core data.
  * @param  none
  * @retval Address of core data.
  */
mico_Context_t* mico_system_context_get( void );

/**
  * @brief  Get the address of the application's config data.
  * @param  in_context: The address of the core data.
  * @retval Address of the application's config data.
  */
void* mico_system_context_get_user_data( mico_Context_t* const in_context );

/**
  * @brief  Restore configurations to default settings, it will cause a delegate
  *         function: appRestoreDefault_callback( ) to give a default value for
  *         application's config data.
  * @param  in_context: The address of the core data.
  * @retval kNoErr is returned on success, otherwise, kXXXErr is returned.
  */
OSStatus mico_system_context_restore( mico_Context_t* const in_context );

/**
  * @brief  Application should give a default value for application's config data
  *         on address: user_config_data
  * @note   This a delegate function, can be completed by developer.
  * @param  user_config_data: The address of the application's config data.
  * @param  size: The size of the application's config data. 
  * @retval None
  */
void appRestoreDefault_callback(void * const user_config_data, uint32_t size);

/**
  * @brief  Write config data hosted by core data to non-volatile storage
  * @param  in_context: The address of the core data.
  * @retval kNoErr is returned on success, otherwise, kXXXErr is returned.
  */
OSStatus mico_system_context_update( mico_Context_t* const in_context );

#ifdef __cplusplus
} /*extern "C" */
#endif
