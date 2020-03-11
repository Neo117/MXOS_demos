#include "mxos.h"
#include "user_ota.h"
#include "user_config.h"
#include "common_api.h"

#define app_log(M, ...)             custom_log("FOG_V3_OTA", M, ##__VA_ARGS__)

static merr_t fog_v3_ota_version_compare( bool *is_need_update, uint8_t local_vsersion_major, uint8_t local_vsersion_minor, uint8_t local_vsersion_revision, const char *fog_version)
{
    merr_t err = kGeneralErr;
    uint32_t fog_version_major = 0, fog_version_minor = 0, fog_version_revision = 0;

    *is_need_update = false;

    require(fog_version != NULL, exit);

    sscanf(fog_version, "%lu.%lu.%lu", &fog_version_major, &fog_version_minor, &fog_version_revision);

    require_string(fog_version_major <= 0xFF && fog_version_minor <= 0xFF && fog_version_revision <= 0xFF, exit, "fog version error!");

    //app_log("%lu.%lu.%lu", fog_version_major, fog_version_minor, fog_version_revision);

    if(local_vsersion_major < fog_version_major)
    {
        goto IS_NEED_UPDATE;
    }

    if(local_vsersion_major == fog_version_major && local_vsersion_minor < fog_version_minor)
    {
        goto IS_NEED_UPDATE;
    }

    if(local_vsersion_major == fog_version_major && local_vsersion_minor == fog_version_minor && local_vsersion_revision < fog_version_revision)
    {
        goto IS_NEED_UPDATE;
    }

    *is_need_update = false;
    return kNoErr;

IS_NEED_UPDATE:
    *is_need_update = true;
    err = kNoErr;

    exit:
    return err;
}

merr_t user_ota_check_with_product_id(bool *is_need_ota, USER_OTA_CONTEXT *user_ota_context)
{
    merr_t err = kGeneralErr;
    FOG_V3_OTA_CHECK_RESPONSE_S ota_response;
    uint8_t i = 0;
    char device_mac[20] = {0};

    require(is_need_ota != NULL && user_ota_context != NULL, exit);

    *is_need_ota = false;
    memset(user_ota_context, 0, sizeof(USER_OTA_CONTEXT));

    err = user_get_device_mac(device_mac, sizeof(device_mac));
    require_noerr( err, exit );

    err = fog_v3_ota_check_with_product_id( NULL, false, FOG_V3_PRODUCT_ID, device_mac, NULL, &ota_response);
    require_noerr( err, exit );

    require_action_string( (ota_response.ota_task_id > 0) && (ota_response.ota_component_num > 0) && (ota_response.ota_component_num < FOG_V3_OTA_MAX_COMPONENT_NUM), exit, err = kNoErr, "no ota task!");

    for(i = 0; i < ota_response.ota_component_num; i++ )
    {
        app_log("[%s]url:%s", ota_response.ota_component[i].component_name, ota_response.ota_component[i].file_url);
        app_log("[%s]md5:%s", ota_response.ota_component[i].component_name, ota_response.ota_component[i].file_md5);
        app_log("[%s]version:%s", ota_response.ota_component[i].component_name, ota_response.ota_component[i].ota_version);
        app_log("[%s]custom_string:%s", ota_response.ota_component[i].component_name, ota_response.ota_component[i].custom_string);

        if(0 == strcmp(ota_response.ota_component[i].component_name, FOG_V3_WIFI_MODULE_NAME)) //check wifi module
        {
            //compare version...
            err = fog_v3_ota_version_compare(&(user_ota_context->wifi.is_update), FOG_V3_WIFI_MODULE_VERSION_MAJOR, FOG_V3_WIFI_MODULE_VERSION_MINOR, FOG_V3_WIFI_MODULE_VERSION_REVISION, ota_response.ota_component[i].ota_version);
            require_noerr(err, exit);

            user_ota_context->wifi.component = USER_OTA_COMPONENT_WIFI_MODULE;

            app_log("[wifi module]fog version:%s, local:%d.%d.%d", ota_response.ota_component[i].ota_version, FOG_V3_WIFI_MODULE_VERSION_MAJOR, FOG_V3_WIFI_MODULE_VERSION_MINOR, FOG_V3_WIFI_MODULE_VERSION_REVISION);

            if(user_ota_context->wifi.is_update == true)
            {
                app_log("wifi module need update!");
                *is_need_ota = true;
                strcpy(user_ota_context->wifi.file_md5, ota_response.ota_component[i].file_md5);
                strcpy(user_ota_context->wifi.file_url, ota_response.ota_component[i].file_url);
                user_ota_context->ota_task_id = ota_response.ota_task_id;
            }else
            {
                memset(user_ota_context->wifi.file_md5, 0, USER_OTA_COMPONENT_MD5_MAX_LEN);
                memset(user_ota_context->wifi.file_url, 0, USER_OTA_FILE_URL_MAX_LEN);
                app_log("wifi module don't need update!");
            }
        }
    }

    exit:
    return err;
}

merr_t user_ota_start(USER_OTA_CONTEXT *user_ota_context, FOG_V3_OTA_STATE_CB wifi_ota_callback)
{
    merr_t err = kGeneralErr;

    require(user_ota_context != NULL && wifi_ota_callback != NULL, exit);

    if(user_ota_context->wifi.is_update == true) //wifi module update
    {
        app_log("update EMW3080B firmware");
        err = fog_v3_module_ota(user_ota_context->wifi.file_url, user_ota_context->wifi.file_md5, wifi_ota_callback, true);
        require_noerr(err, exit);
    }

    exit:
    return err;
}

merr_t user_ota_upload_log_with_product_id(int32_t ota_task_id)
{
    merr_t err = kGeneralErr;
    FOG_V3_OTA_UPLOAD_LOG_INFO_PRODUCT_ID_S ota_upload_log_info;
    char device_mac[20] = {0};

    memset(&ota_upload_log_info, 0, sizeof(FOG_V3_OTA_UPLOAD_LOG_INFO_DEVICE_ID_S));

    err = user_get_device_mac(device_mac, sizeof(device_mac));
    require_noerr( err, exit );

    ota_upload_log_info.fog_v3_product_id = FOG_V3_PRODUCT_ID;
    ota_upload_log_info.fog_v3_device_sn = device_mac;
    ota_upload_log_info.ota_task_id = ota_task_id;
    ota_upload_log_info.is_ota_success = true;
    ota_upload_log_info.component_name_array[0] = FOG_V3_WIFI_MODULE_NAME;
    ota_upload_log_info.customize_str = "user string....";

    err = fog_v3_ota_upload_log_with_product_id(NULL, false, &ota_upload_log_info);
    require_noerr( err, exit );

    exit:
    return err;
}
