#include "mxos.h"
#include "user_config.h"
#include "user_ota.h"
#include "common_api.h"

#define app_log(M, ...)             custom_log("MAIN", M, ##__VA_ARGS__)

static mos_semphr_id_t wait_sem = NULL;
USER_OTA_CONTEXT user_ota_context;

static void micoNotify_WifiStatusHandler( WiFiEvent status, void* const inContext )
{
    UNUSED_PARAMETER( inContext );
    switch ( status )
    {
        case NOTIFY_STATION_UP:
            mos_semphr_release( wait_sem );
            break;
        case NOTIFY_STATION_DOWN:
            case NOTIFY_AP_UP:
            case NOTIFY_AP_DOWN:
            break;
    }
}

void wifi_module_ota_state_callback( FOG_V3_OTA_STATE_E state, uint32_t sys_args )
{
    FOG_V3_OTA_SUCCESS_INFO_S *ota_info = NULL;

    if ( FOG_V3_OTA_STATE_MD5_SUCCESS == state )
    {
        ota_info = (FOG_V3_OTA_SUCCESS_INFO_S *)sys_args;

        require( ota_info != NULL, exit );

        app_log("ota filed download and check success, file len: %ld, crc16:%d", ota_info->ota_len, ota_info->crc );

        user_ota_upload_log_with_product_id( user_ota_context.ota_task_id );

        app_log("ota len:%ld, crc:%d", ota_info->ota_len, ota_info->crc);
        mxos_ota_switch_to_new_fw( ota_info->ota_len, ota_info->crc );
        mxos_sys_reboot( );
    }

    exit:
    return;
}


int main( void )
{
    merr_t err = kNoErr;
    bool is_need_ota = false;

    wait_sem = mos_semphr_new( 1 );

    /*Register user function for MXOS nitification: WiFi status changed */
    err = mxos_system_notify_register( mxos_notify_WIFI_STATUS_CHANGED, (void *)micoNotify_WifiStatusHandler, NULL );
    require_noerr( err, exit );

    err = mxos_system_init( );
    require_noerr( err, exit );

    /* Wait for wlan connection*/
    mos_semphr_acquire( wait_sem, MOS_WAIT_FOREVER );
    app_log( "wifi connected successful" );

    err = user_ota_check_with_product_id( &is_need_ota, &user_ota_context );
    require_noerr( err, exit );

    if ( is_need_ota == true )
    {
        err = user_ota_start( &user_ota_context, wifi_module_ota_state_callback );
        require_noerr( err, exit );
    }

    exit:
    return err;
}



