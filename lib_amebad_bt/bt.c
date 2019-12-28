#include "gap_le.h"
#include "gap_msg.h"
#include "gap_config.h"
#include "gap_adv.h"
#include "gap_conn_le.h"
#include "gap_scan.h"
#include "app_msg.h"
#include "mxos.h"
#include "mbt.h"
#include "bte.h"

#define app_log(format, ...)          custom_log("bt", format, ##__VA_ARGS__)

#define APP_TASK_PRIORITY             MOS_APPLICATION_PRIORITY         //!< Task priorities
#define APP_TASK_STACK_SIZE           256 * 8   //!<  Task stack size
#define MAX_NUMBER_OF_GAP_MESSAGE     0x20      //!<  GAP message queue size
#define MAX_NUMBER_OF_IO_MESSAGE      0x20      //!<  IO message queue size
#define MAX_NUMBER_OF_EVENT_MESSAGE   (MAX_NUMBER_OF_GAP_MESSAGE + MAX_NUMBER_OF_IO_MESSAGE)    //!< Event message queue size
#define BT_INIT_COUNT_MAX             150

extern void app_ftl_init( void );
extern void bt_coex_init( void );
extern void wifi_btcoex_set_bt_on( void );
extern void wifi_btcoex_set_bt_off( void );

static mos_thread_id_t app_task_handle;   //!< APP Task handle
static mos_queue_id_t evt_queue_handle;  //!< Event queue handle
static mos_queue_id_t io_queue_handle;   //!< IO queue handle
static mos_mutex_id_t bt_mutex;
static uint8_t user_start_bt = 0, is_bt_on = 0, user_start_adv = 0, user_conn_flag = 0, user_start_scan = 0;
static uint8_t max_link_num = 1;
static MBT_STATE_CBS *mbt_cbs;

#ifndef WEAK
#define WEAK             __attribute__((weak))
#endif

/** Pre-handle all the BT GAP MSGs  */
WEAK void mbt_app_handle_gap_msg( T_IO_MSG *p_gap_msg )
{

}

static void mbt_app_handle_io_msg( T_IO_MSG io_msg )
{
    uint16_t msg_type = io_msg.type;

    switch (msg_type)
    {
    case IO_MSG_TYPE_BT_STATUS:
    {
        mbt_app_handle_gap_msg(&io_msg);
    }
    break;
    default:
        break;
    }
}

/** Initialize GATT profiles */
WEAK void mbt_app_le_profile_init( void )
{
//    app_le_profile_init( );
}

/** Initialize GAP scan/adv/connect/bond... parameters */
WEAK void mbt_app_le_gap_init( void )
{
//    app_le_gap_init( );
}

static void mbt_gap_config_max_le_link_num( void )
{
    gap_config_max_le_link_num( max_link_num );
}

static bool mbt_le_gap_init( void )
{
    return le_gap_init( max_link_num );
}

static void bt_app_main_task( void *p_param )
{
    uint8_t event;
    io_queue_handle = mos_queue_new( sizeof(T_IO_MSG), MAX_NUMBER_OF_IO_MESSAGE );
    evt_queue_handle = mos_queue_new( sizeof(uint8_t), MAX_NUMBER_OF_EVENT_MESSAGE );

    gap_start_bt_stack( evt_queue_handle, io_queue_handle, MAX_NUMBER_OF_GAP_MESSAGE );

    while ( true )
    {
        if ( mos_queue_pop( evt_queue_handle, &event, 0xFFFFFFFF ) == kNoErr )
        {
            if ( event == EVENT_IO_TO_APP )
            {
                T_IO_MSG io_msg;
                if ( mos_queue_pop( io_queue_handle, &io_msg, 0 ) == kNoErr )
                {
                    mbt_app_handle_io_msg( io_msg );
                }
            } else if ( event == 0xfe )
            {
                goto exit;
            } else
            {
                gap_handle_msg( event );
            }
        }
    }

    exit:
    //gap_stop_bt_stack
    if ( io_queue_handle )
    {
        mos_queue_delete( io_queue_handle );
        io_queue_handle = NULL;
    }
    if ( evt_queue_handle )
    {
        mos_queue_delete( evt_queue_handle );
        evt_queue_handle = NULL;
    }

    mos_thread_delete( NULL );
}

static void bt_app_task_init( )
{
    if ( app_task_handle == NULL )
        app_task_handle = mos_thread_new( APP_TASK_PRIORITY, "bt", bt_app_main_task, APP_TASK_STACK_SIZE, NULL );
}

static void bt_app_task_deinit( )
{
    if ( evt_queue_handle && app_task_handle )
    {
        uint8_t event = 0xfe;
        mos_queue_push( evt_queue_handle, &event, 0 );

        mos_thread_join( app_task_handle );
        app_task_handle = NULL;
    }
}

merr_t mbt_on( void )
{
    T_GAP_DEV_STATE new_state;
    uint8_t bt_init_count = 0;
    merr_t err = kGeneralErr;

    if ( user_start_bt == 0 )
        return kNoErr;

    if ( is_bt_on )
        return kNoErr;

    if ( bt_mutex == NULL )
        bt_mutex = mos_mutex_new( );

    app_log("Initializing BT ...");

    mos_mutex_lock( bt_mutex );

    mwifi_ps_off( );

    mbt_gap_config_max_le_link_num( );
    bte_init( );

    mbt_le_gap_init( );

    mbt_app_le_gap_init( );
    mbt_app_le_profile_init( );

    bt_app_task_init( );

    bt_coex_init( );

    do
    {
        mos_msleep( 20 );
        le_get_gap_param( GAP_PARAM_DEV_STATE, &new_state );
        if ( bt_init_count >= BT_INIT_COUNT_MAX )
        {
            err = kGeneralErr;
            goto exit;
        }
        bt_init_count++;
    } while ( new_state.gap_init_state != GAP_INIT_STATE_STACK_READY );

    wifi_btcoex_set_bt_on( );

    if ( mbt_cbs != NULL && mbt_cbs->bt_on_cb != NULL )
        mbt_cbs->bt_on_cb( );

    is_bt_on = 1;

    if ( user_start_adv )
        le_adv_start( );

    if ( user_start_scan )
        le_scan_start( );

    app_log("BT initialized");

    exit:
    mos_mutex_unlock( bt_mutex );
    return err;
}

void mbt_off( void )
{
    if ( user_start_bt == 0 )
        return;
    if ( is_bt_on == 0 )
        return;
    mos_mutex_lock( bt_mutex );
    if ( mbt_cbs != NULL && mbt_cbs->bt_off_cb != NULL )
        mbt_cbs->bt_off_cb( );

    if ( user_start_adv )
        le_adv_stop( );

    if ( user_start_scan )
        le_scan_stop( );

    wifi_btcoex_set_bt_off( );
    bt_app_task_deinit( );
    bte_deinit( );
    is_bt_on = 0;
    mos_mutex_unlock( bt_mutex );

    app_log("bt off");
}

void mbt_callback_init( MBT_STATE_CBS *cbs )
{
    if ( cbs )
        mbt_cbs = cbs;
}

merr_t mbt_init( uint8_t link_num )
{
    merr_t err = kNoErr;

    user_start_bt = 1;
    max_link_num = link_num;
#if ENBALE_BLE_CENTRAL
    app_ftl_init();
#endif
    mbt_on( );

    return err;
}

merr_t mbt_deinit( void )
{
    mbt_off( );
    user_start_bt = 0;

    return kNoErr;
}

merr_t mbt_le_adv_start( void )
{
    T_GAP_CAUSE cause = le_adv_start( );
    if ( cause == GAP_CAUSE_SUCCESS )
    {
        user_start_adv = 1;
        return kNoErr;
    } else
    {
        user_start_adv = 0;
        return kGeneralErr;
    }
}

merr_t mbt_le_adv_stop( void )
{
    T_GAP_CAUSE cause = le_adv_stop( );
    if ( cause == GAP_CAUSE_SUCCESS )
    {
        user_start_adv = 0;
        return kNoErr;
    } else
    {
        return kGeneralErr;
    }
}

merr_t mbt_le_conn( uint8_t *addr, T_GAP_REMOTE_ADDR_TYPE type, uint16_t scan_timeout )
{
    T_GAP_CAUSE cause;
    merr_t err = kGeneralErr;

    require( is_bt_on == 1, exit );

    cause = le_connect( GAP_PHYS_CONN_INIT_1M_BIT, addr, type, GAP_LOCAL_ADDR_LE_PUBLIC, scan_timeout );
    if ( cause == GAP_CAUSE_SUCCESS )
    {
        user_conn_flag = 1;
        err = kNoErr;
    } else
    {
        user_conn_flag = 0;
        err = kGeneralErr;
    }

    exit:
    return err;
}

merr_t mbt_le_disconnect( uint8_t conn_id )
{
    T_GAP_CAUSE cause;
    merr_t err = kGeneralErr;

    cause = le_disconnect( conn_id );
    if ( cause == GAP_CAUSE_SUCCESS )
    {
        err = kNoErr;
    } else
    {
        err = kGeneralErr;
    }

    user_conn_flag = 0;

    return err;
}

merr_t mbt_scan_start( void )
{
    T_GAP_CAUSE cause;
    merr_t err = kGeneralErr;

    cause = le_scan_start( );
    if ( cause == GAP_CAUSE_SUCCESS || cause == GAP_CAUSE_ALREADY_IN_REQ )
    {
        user_start_scan = 1;
        err = kNoErr;
    } else
    {
        err = kGeneralErr;
    }

    return err;
}

merr_t mbt_scan_stop( void )
{
    T_GAP_CAUSE cause;
    merr_t err = kGeneralErr;

    cause = le_scan_stop( );
    if ( cause == GAP_CAUSE_SUCCESS || cause == GAP_CAUSE_ALREADY_IN_REQ )
    {
        user_start_scan = 0;
        err = kNoErr;
    } else
    {
        err = kGeneralErr;
    }

    return err;
}

