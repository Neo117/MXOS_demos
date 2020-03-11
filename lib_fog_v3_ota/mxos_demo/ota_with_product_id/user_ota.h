#ifndef _USER_OTA_H
#define _USER_OTA_H

#include "fog_v3_ota.h"

#define USER_OTA_FILE_URL_MAX_LEN                 (256)
#define USER_OTA_COMPONENT_MD5_MAX_LEN            (64)

typedef enum {
    USER_OTA_COMPONENT_WIFI_MODULE,   //update wifi module
    USER_OTA_COMPONENT_MAX,
    USER_OTA_COMPONENT_NONE
}USER_OTA_COMPONENT_E;

typedef struct _FOG_V3_OTA_TASK
{
    bool is_update;
    USER_OTA_COMPONENT_E component;
    char file_url[USER_OTA_FILE_URL_MAX_LEN];
    char file_md5[USER_OTA_COMPONENT_MD5_MAX_LEN];
} USER_OTA_TASK;

typedef struct _USER_OTA_CONTEXT
{
    int32_t ota_task_id;
    USER_OTA_TASK wifi;
} USER_OTA_CONTEXT;

merr_t user_ota_check_with_product_id(bool *is_need_ota, USER_OTA_CONTEXT *user_ota_context);

merr_t user_ota_start(USER_OTA_CONTEXT *user_ota_context, FOG_V3_OTA_STATE_CB wifi_ota_callback);

merr_t user_ota_upload_log_with_product_id(int32_t ota_task_id);
#endif
