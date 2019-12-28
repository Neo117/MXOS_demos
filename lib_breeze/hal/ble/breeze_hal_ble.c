/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <breeze_hal_ble.h>
#include "mxos.h"
#include "gap.h"
#include "gap_config.h"
#include "bte.h"
#include "gap_le.h"
#include "gap_bond_le.h"
#include "gap_scan.h"
#include "gap_le_types.h"
#include "profile_client.h"
#include "profile_server.h"
#include "breeze_hal_os.h"
#include <app_msg.h>
#include "gap_adv.h"
#include "gap_conn_le.h"
#include "bt_config_service.h"
#include "peripheral_app.h"
#include "mbt.h"

static void breeze_ble_disconnect( void );

ais_bt_init_t * bt_init_info = NULL;
static uint8_t *adv_info = NULL;

extern uint8_t bt_config_conn_id;

#define app_log(format, ...)  custom_log("ble", format, ##__VA_ARGS__)

/** @brief  Default minimum advertising interval when device is discoverable (units of 625us, 160=100ms) */
#define DEFAULT_ADVERTISING_INTERVAL_MIN            320
/** @brief  Default maximum advertising interval */
#define DEFAULT_ADVERTISING_INTERVAL_MAX            400

#define APP_MAX_LINKS       1
#define BLE_ADV_DATA_LEN    31

#if !MXOS_APP_USE_BT
void mbt_app_le_gap_init( void )
{
    /* Device name and device appearance */
    uint8_t device_name[GAP_DEVICE_NAME_LEN] = "AOS Device";
    uint16_t appearance = GAP_GATT_APPEARANCE_UNKNOWN;
    uint8_t slave_init_mtu_req = false;

    /* Advertising parameters */
    uint8_t adv_evt_type = GAP_ADTYPE_ADV_IND;
    uint8_t adv_direct_type = GAP_REMOTE_ADDR_LE_PUBLIC;
    uint8_t adv_direct_addr[GAP_BD_ADDR_LEN] = { 0 };
    uint8_t adv_chann_map = GAP_ADVCHAN_ALL;
    uint8_t adv_filter_policy = GAP_ADV_FILTER_ANY;
    uint16_t adv_int_min = DEFAULT_ADVERTISING_INTERVAL_MIN;
    uint16_t adv_int_max = DEFAULT_ADVERTISING_INTERVAL_MAX;

    /* GAP Bond Manager parameters */
    uint8_t auth_pair_mode = GAP_PAIRING_MODE_PAIRABLE;
    uint16_t auth_flags = GAP_AUTHEN_BIT_BONDING_FLAG;
    uint8_t auth_io_cap = GAP_IO_CAP_NO_INPUT_NO_OUTPUT;
#if F_BT_LE_SMP_OOB_SUPPORT
    uint8_t auth_oob = false;
#endif
    uint8_t auth_use_fix_passkey = false;
    uint32_t auth_fix_passkey = 0;
    uint8_t auth_sec_req_enable = false;
    uint16_t auth_sec_req_flags = GAP_AUTHEN_BIT_BONDING_FLAG;

    /* Set device name and device appearance */
    le_set_gap_param( GAP_PARAM_DEVICE_NAME, GAP_DEVICE_NAME_LEN, device_name );
    le_set_gap_param( GAP_PARAM_APPEARANCE, sizeof(appearance), &appearance );
    le_set_gap_param( GAP_PARAM_SLAVE_INIT_GATT_MTU_REQ, sizeof(slave_init_mtu_req),
                      &slave_init_mtu_req );

    /* Set advertising parameters */
    le_adv_set_param( GAP_PARAM_ADV_EVENT_TYPE, sizeof(adv_evt_type), &adv_evt_type );
    le_adv_set_param( GAP_PARAM_ADV_DIRECT_ADDR_TYPE, sizeof(adv_direct_type), &adv_direct_type );
    le_adv_set_param( GAP_PARAM_ADV_DIRECT_ADDR, sizeof(adv_direct_addr), adv_direct_addr );
    le_adv_set_param( GAP_PARAM_ADV_CHANNEL_MAP, sizeof(adv_chann_map), &adv_chann_map );
    le_adv_set_param( GAP_PARAM_ADV_FILTER_POLICY, sizeof(adv_filter_policy), &adv_filter_policy );
    le_adv_set_param( GAP_PARAM_ADV_INTERVAL_MIN, sizeof(adv_int_min), &adv_int_min );
    le_adv_set_param( GAP_PARAM_ADV_INTERVAL_MAX, sizeof(adv_int_max), &adv_int_max );
//    le_adv_set_param(GAP_PARAM_ADV_DATA, sizeof(adv_data), (void *)adv_data);
    //le_adv_set_param(GAP_PARAM_SCAN_RSP_DATA, sizeof(scan_rsp_data), (void *)scan_rsp_data);

    /* Setup the GAP Bond Manager */
    gap_set_param( GAP_PARAM_BOND_PAIRING_MODE, sizeof(auth_pair_mode), &auth_pair_mode );
    gap_set_param( GAP_PARAM_BOND_AUTHEN_REQUIREMENTS_FLAGS, sizeof(auth_flags), &auth_flags );
    gap_set_param( GAP_PARAM_BOND_IO_CAPABILITIES, sizeof(auth_io_cap), &auth_io_cap );
#if F_BT_LE_SMP_OOB_SUPPORT
    gap_set_param( GAP_PARAM_BOND_OOB_ENABLED, sizeof(auth_oob), &auth_oob );
#endif
    le_bond_set_param( GAP_PARAM_BOND_FIXED_PASSKEY, sizeof(auth_fix_passkey), &auth_fix_passkey );
    le_bond_set_param( GAP_PARAM_BOND_FIXED_PASSKEY_ENABLE, sizeof(auth_use_fix_passkey),
                       &auth_use_fix_passkey );
    le_bond_set_param( GAP_PARAM_BOND_SEC_REQ_ENABLE, sizeof(auth_sec_req_enable), &auth_sec_req_enable );
    le_bond_set_param( GAP_PARAM_BOND_SEC_REQ_REQUIREMENT, sizeof(auth_sec_req_flags),
                       &auth_sec_req_flags );

    /* register gap message callback */
    le_register_app_cb( bt_config_gap_callback );
}

void mbt_app_le_profile_init( void )
{
    server_init( 1 );

    bt_config_srv_id = bt_config_service_add_service( (void *) bt_config_profile_callback );
    server_register_app_cb( bt_config_profile_callback );
}
#else
void breeze_profile_init( void )
{
    server_init(1);

    app_log("breeze profile init");

    bt_config_srv_id = bt_config_service_add_service((void *)bt_config_profile_callback);
    server_register_app_cb(bt_config_profile_callback);
}

void breeze_adv_param_init(void)
{
    /* Advertising parameters */
    uint8_t adv_evt_type = GAP_ADTYPE_ADV_IND;
    uint8_t adv_direct_type = GAP_REMOTE_ADDR_LE_PUBLIC;
    uint8_t adv_direct_addr[GAP_BD_ADDR_LEN] =
    {   0};
    uint8_t adv_chann_map = GAP_ADVCHAN_ALL;
    uint8_t adv_filter_policy = GAP_ADV_FILTER_ANY;
    uint16_t adv_int_min = DEFAULT_ADVERTISING_INTERVAL_MIN;
    uint16_t adv_int_max = DEFAULT_ADVERTISING_INTERVAL_MAX;

    /* Set advertising parameters */
    le_adv_set_param(GAP_PARAM_ADV_EVENT_TYPE, sizeof(adv_evt_type), &adv_evt_type);
    le_adv_set_param(GAP_PARAM_ADV_DIRECT_ADDR_TYPE, sizeof(adv_direct_type), &adv_direct_type);
    le_adv_set_param(GAP_PARAM_ADV_DIRECT_ADDR, sizeof(adv_direct_addr), adv_direct_addr);
    le_adv_set_param(GAP_PARAM_ADV_CHANNEL_MAP, sizeof(adv_chann_map), &adv_chann_map);
    le_adv_set_param(GAP_PARAM_ADV_FILTER_POLICY, sizeof(adv_filter_policy), &adv_filter_policy);
    le_adv_set_param(GAP_PARAM_ADV_INTERVAL_MIN, sizeof(adv_int_min), &adv_int_min);
    le_adv_set_param(GAP_PARAM_ADV_INTERVAL_MAX, sizeof(adv_int_max), &adv_int_max);
}
#endif

static MBT_STATE_CBS mbt_cbs = {
    .bt_on_cb = NULL,
    .bt_off_cb = breeze_ble_disconnect
};

static void breeze_ble_disconnect( void )
{
    uint8_t connid = bt_config_conn_id;
    mbt_le_disconnect( connid );
}

ais_err_t ble_stack_init( ais_bt_init_t *info )
{
    ais_err_t err = AIS_ERR_SUCCESS;
    uint32_t size;
    T_GAP_DEV_STATE new_state;
    ais_char_init_t *c;

    bt_init_info = info;
#if !MXOS_APP_USE_BT
    mbt_callback_init( &mbt_cbs );
    mbt_init( APP_MAX_LINKS );
#endif
    return err;
}

ais_err_t ble_stack_deinit( )
{
    app_log("ble_stack_deinit");

    T_GAP_DEV_STATE new_state;

    mbt_le_adv_stop( );

#if !MXOS_APP_USE_BT
    mbt_deinit( );
#endif
    if ( adv_info )
    {
        free( adv_info );
        adv_info = NULL;
    }
    return AIS_ERR_SUCCESS;
}

ais_err_t ble_send_notification( uint8_t *p_data, uint16_t length )
{
//    app_log("ble_send_notification");
    bool ret = false;
    uint8_t connid = bt_config_conn_id;

    ret = bt_config_service_send_notify( connid, p_data, length );
    return ret == true ? AIS_ERR_SUCCESS : AIS_ERR_GATT_NOTIFY_FAIL;
}

typedef void (*txdone)( uint8_t res );
txdone txdone_cb;
ais_err_t ble_send_indication( uint8_t *p_data, uint16_t length, void (*txdone)( uint8_t res ) )
{
//    app_log("ble_send_indication");
    bool ret = false;
    uint8_t connid = bt_config_conn_id;

    ret = bt_config_service_send_ind( connid, p_data, length );
    txdone_cb = txdone;
    return ret == true ? AIS_ERR_SUCCESS : AIS_ERR_GATT_INDICATE_FAIL;
}

void ble_disconnect( uint8_t reason )
{
    uint8_t connid = bt_config_conn_id;
    app_log("ble_disconnect conn id = %d", connid);
    le_disconnect( connid );
}

ais_err_t ble_advertising_start( ais_adv_init_t *adv )
{
    app_log("ble_advertising_start");
    uint8_t adv_idx, adv_name_len;

    if ( adv_info == NULL )
        adv_info = malloc( BLE_ADV_DATA_LEN );

    memset( adv_info, 0x00, BLE_ADV_DATA_LEN );

    adv_idx = 0;
    adv_info[adv_idx] = 0x02;
    adv_idx++;
    adv_info[adv_idx] = 0x01;
    adv_idx++;
    adv_info[adv_idx] = 0x06;
    adv_idx++;

    adv_info[adv_idx] = adv->vdata.len + 1;
    adv_idx += 1;
    adv_info[adv_idx] = 0xFF;
    adv_idx += 1;
    memcpy( &adv_info[adv_idx], adv->vdata.data, adv->vdata.len );
    adv_idx += adv->vdata.len;

    adv_name_len = strlen( adv->name.name ) + 1;
    adv_info[adv_idx] = adv_name_len;
    adv_idx += 1;
    adv_info[adv_idx] = adv->name.ntype + 0x08;
    adv_idx += 1; //name
    memcpy( &adv_info[adv_idx], adv->name.name, strlen( adv->name.name ) );
    adv_idx += strlen( adv->name.name );

    le_adv_set_param( GAP_PARAM_ADV_DATA, adv_idx, (void *) adv_info );
    mbt_le_adv_start( );

    return AIS_ERR_SUCCESS;
}

ais_err_t ble_advertising_stop( )
{
    app_log("ble_advertising_stop");
    mbt_le_adv_stop( );

    return AIS_ERR_SUCCESS;
}

ais_err_t ble_get_mac( uint8_t *mac )
{
    gap_get_param( GAP_PARAM_BD_ADDR, mac );
    app_log("local bd addr: 0x%2x:%2x:%2x:%2x:%2x:%2x\r\n",
        mac[5],
        mac[4],
        mac[3],
        mac[2],
        mac[1],
        mac[0]);

    return AIS_ERR_SUCCESS;
}
