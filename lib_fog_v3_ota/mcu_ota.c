#include "mxos.h"
#include "http_file_download.h"
#include "fog_v3_ota.h"
#include "mcu_ota.h"

#if (MCU_OTA_DEBUG_ENABLE == 1)
#define app_log(M, ...)             custom_log("MCU_OTA", M, ##__VA_ARGS__)
#elif (MCU_OTA_DEBUG_ENABLE == 0)
#define app_log(M, ...)
#else
#error "MCU_OTA_DEBUG_ENABLE not define!"
#endif

static uint8_t fog_v3_module_md5[MCU_OTA_MD5_MAX_LEN] = {0};
static FILE_DOWNLOAD_CONTEXT ota_file_context = NULL;

//strupr
static unsigned char* user_strupr(unsigned char* szMsg)
{
    unsigned char *pcMsg = NULL;

    for (pcMsg = szMsg; ('\0' != *pcMsg); pcMsg++)
    {
        if (('a' <= *pcMsg) && (*pcMsg <= 'z'))
        {
            *pcMsg += ('A' - 'a');
        }
    }

    return szMsg;
}

static bool user_str2hex(unsigned char *src, uint8_t *dest, uint32_t dest_size)
{
    unsigned char hb = 0;
    unsigned char lb = 0;
    uint32_t i = 0, j = 0;
    uint32_t src_size = strlen((const char *)src);

    if ( (src_size % 2 != 0) || (src_size <= 0))
        return false;

    src = user_strupr( src );

    for ( i = 0; i < src_size; i ++ )
    {
        if(i > dest_size * 2)
            return false;

        hb = src[i];
        if ( hb >= 'A' && hb <= 'F' )
            hb = hb - 'A' + 10;
        else if ( hb >= '0' && hb <= '9' )
            hb = hb - '0';
        else
            return false;

        i++;
        lb = src[i];
        if ( lb >= 'A' && lb <= 'F' )
            lb = lb - 'A' + 10;
        else if ( lb >= '0' && lb <= '9' )
            lb = lb - '0';
        else
            return false;

        dest[j++] = (hb << 4) | (lb);
    }

    return true;
}

static int fog_v3_ota_check(uint32_t ota_file_len, FOG_V3_OTA_SUCCESS_INFO_S *info)
{
    int err = kGeneralErr;
    md5_context ctx;
    uint8_t md5_calc[16] = {0};
    uint8_t md5_recv[16] = {0};
    uint16_t crc = 0;
    CRC16_Context crc16_contex;
    uint8_t *bin_buf = NULL;
    uint32_t read_index = 0;
    uint32_t file_len = ota_file_len;
    uint32_t need_read_len = 0;
    bool ret = 0;

    require_string(mhal_flash_get_info( MODULE_PARTITION_OTA_TEMP )->partition_owner != MODULE_PARTITION_NONE, exit, "OTA storage is not exist");

    InitMd5(&ctx);
    CRC16_Init( &crc16_contex );

    bin_buf = malloc(MCU_V3_OTA_BIN_BUFF_LEN);
    require_string(bin_buf != NULL, exit, "malloc bin_buff failed");

    while(1)
    {
        if(file_len - read_index >=  MCU_V3_OTA_BIN_BUFF_LEN)
        {
            need_read_len = MCU_V3_OTA_BIN_BUFF_LEN;
        }else
        {
            need_read_len = file_len - read_index;
        }

        err = mhal_flash_read(MODULE_PARTITION_OTA_TEMP, &read_index, bin_buf, need_read_len);
        require_noerr(err, exit);

        Md5Update(&ctx, bin_buf, need_read_len);
        CRC16_Update( &crc16_contex, bin_buf, need_read_len );

        if((read_index == ota_file_len) && (read_index != 0))
        {
            break;
        }
    }

    Md5Final(&ctx, md5_calc);
    CRC16_Final( &crc16_contex, &crc );

    app_log("FLASH READ: %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X",
            md5_calc[0],md5_calc[1],md5_calc[2],md5_calc[3],
            md5_calc[4],md5_calc[5],md5_calc[6],md5_calc[7],
            md5_calc[8],md5_calc[9],md5_calc[10],md5_calc[11],
            md5_calc[12],md5_calc[13],md5_calc[14],md5_calc[15]);

    //str2hex(fog_v3_module_md5, md5_recv, sizeof(md5_recv));
    ret = user_str2hex(fog_v3_module_md5, md5_recv, sizeof(md5_recv));
    require_action_string(ret == true, exit, err = kGeneralErr, "user_str2hex() is error");

    if ( memcmp( md5_recv, md5_calc, sizeof(md5_recv) ) == 0 )
    {
        info->ota_len = ota_file_len;
        info->crc = crc;
        app_log( "OTA SUCCESS!\r\n" );
    }else
    {
        app_log("ERROR!! MD5 Error.");
        app_log("HTTP RECV:   %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X",
                 md5_recv[0],md5_recv[1],md5_recv[2],md5_recv[3],
                 md5_recv[4],md5_recv[5],md5_recv[6],md5_recv[7],
                 md5_recv[8],md5_recv[9],md5_recv[10],md5_recv[11],
                 md5_recv[12],md5_recv[13],md5_recv[14],md5_recv[15]);

        err = kGeneralErr;
    }

 exit:
    if(bin_buf != NULL)
    {
        free(bin_buf);
        bin_buf = NULL;
    }

    return err;
}

//file download state callback
void file_download_state_cb( void *context, HTTP_FILE_DOWNLOAD_STATE_E state, uint32_t sys_args, uint32_t user_args )
{
    int err = kGeneralErr;
    FOG_V3_OTA_STATE_CB fog_v3_ota_callback = (FOG_V3_OTA_STATE_CB)user_args;
    FILE_DOWNLOAD_CONTEXT file_download_context = (FILE_DOWNLOAD_CONTEXT) context;
    FOG_V3_OTA_SUCCESS_INFO_S info;

    require(file_download_context != NULL && fog_v3_ota_callback != NULL && state < HTTP_FILE_DOWNLOAD_STATE_MAX, exit);

    //app_log("file download state:%d, sys_args:%lu", state, sys_args);

    if(state == HTTP_FILE_DOWNLOAD_STATE_START)
    {
        //erase flash
        require_string(mhal_flash_get_info( MODULE_PARTITION_OTA_TEMP )->partition_owner != MODULE_PARTITION_NONE, exit_http_start, "OTA storage is not exist!");

        app_log("erase MODULE_PARTITION_OTA_TEMP flash start");

        err = mhal_flash_erase(MODULE_PARTITION_OTA_TEMP, 0, mhal_flash_get_info( MODULE_PARTITION_OTA_TEMP )->partition_length);
        require_noerr(err, exit_http_start);

        app_log("erase MODULE_PARTITION_OTA_TEMP flash success");

        fog_v3_ota_callback(FOG_V3_OTA_STATE_START, 0);

        exit_http_start:
        if(err != kNoErr)
        {
            http_file_download_stop(&file_download_context, false);
        }
    }else if(state == HTTP_FILE_DOWNLOAD_STATE_SUCCESS)
    {
       //calculate MD5 and crc16
       err =  fog_v3_ota_check(http_file_download_get_total_file_len(&file_download_context), &info);
       if(err == kNoErr)
       {
           /* user should call mico_ota_switch_to_new_fw() to finish OTA */
           fog_v3_ota_callback(FOG_V3_OTA_STATE_MD5_SUCCESS, (uint32_t)&info);
       }else
       {
           fog_v3_ota_callback(FOG_V3_OTA_STATE_MD5_FAILED, 0);
       }
    }else if(state == HTTP_FILE_DOWNLOAD_STATE_LOADING)
    {
        fog_v3_ota_callback(FOG_V3_OTA_STATE_LOADING, sys_args);
    }else if(state == HTTP_FILE_DOWNLOAD_STATE_FAILED)
    {
        fog_v3_ota_callback(FOG_V3_OTA_STATE_FAILED, sys_args);
    }else if(state == HTTP_FILE_DOWNLOAD_STATE_FAILED_AND_RETRY)
    {
        fog_v3_ota_callback(FOG_V3_OTA_STATE_FAILED_AND_RETRY, sys_args);
    }else if(state == HTTP_FILE_DOWNLOAD_STATE_SUCCESS)
    {
        fog_v3_ota_callback(FOG_V3_OTA_STATE_FILE_DOWNLOAD_SUCCESS, sys_args);
    }

    exit:
    return;
}

//file download data callback
bool file_download_data_cb( void *context, const char *data, uint32_t data_len, uint32_t user_args)
{
    int err = kGeneralErr;
    FILE_DOWNLOAD_CONTEXT file_download_context = (FILE_DOWNLOAD_CONTEXT)context;
    mxos_logic_partition_t  *ota_partition = mhal_flash_get_info(MODULE_PARTITION_OTA_TEMP);
    uint32_t index = http_file_download_get_download_len(&file_download_context);

    //app_log("file download, total len:%lu, get:%lu, len:%lu", file_download_context->file_info.file_total_len, file_download_context->file_info.download_len, data_len);

    require_action_string(file_download_check_control_state(file_download_context) == true, exit, err = kGeneralErr, "user set stop download!");
    require_string(mhal_flash_get_info( MODULE_PARTITION_OTA_TEMP )->partition_length > http_file_download_get_total_file_len(&file_download_context), exit, "file len error!");

    //copy data into flash
    require_string(ota_partition->partition_owner != MODULE_PARTITION_NONE, exit, "OTA storage is not exist");

    err = mhal_flash_write(MODULE_PARTITION_OTA_TEMP, &index, (uint8_t *) data, data_len);
    require_noerr(err, exit);

    exit:
    if(err == kNoErr)
    {
        return true;
    }else
    {
        return false;
    }
}

/**
 * @brief fog v3 module ota function
 * @param  ota_url[in]: module ota url,sopport http and https.
 * @param  ota_file_md5[in]: ota file md5.
 * @param  fog_v3_ota_state_callback[in]: fog v3 ota state callback
 * @param  is_block[in]: is block until file download finish
 * @retval kNoErr is returned on success, otherwise, kXXXErr is returned.
 */
int fog_v3_module_ota( const char *ota_url, const char *ota_file_md5, FOG_V3_OTA_STATE_CB fog_v3_ota_state_callback, bool is_block)
{
    int err = kGeneralErr;

    require_action((ota_url != NULL) && (ota_file_md5 != NULL) && (strlen(ota_file_md5) < MCU_OTA_MD5_MAX_LEN), exit, err = kParamErr);

    memset(fog_v3_module_md5, 0, sizeof(fog_v3_module_md5));
    memcpy( fog_v3_module_md5, ota_file_md5, strlen(ota_file_md5));

    //download ota file
    err = http_file_download_start( &ota_file_context, ota_url, file_download_state_cb, file_download_data_cb, (uint32_t)fog_v3_ota_state_callback );
    require_noerr_string(err, exit, "http file download error");

    if(is_block == true)
    {
        while(http_file_download_get_state(&ota_file_context) != HTTP_FILE_DOWNLOAD_CONTROL_STATE_NONE)
        {
            app_log("waiting module_ota......");
            mos_msleep(300);
        }
    }

    exit:
    return err;
}

