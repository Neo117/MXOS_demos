#ifndef _FOG_V3_OTA_H_
#define _FOG_V3_OTA_H_

#include "http_short_connection.h"
#include "mcu_ota.h"

#ifndef FOG_V3_OTA_DEBUG_ENABLE
#define FOG_V3_OTA_DEBUG_ENABLE                     (1)
#endif

#define FOG_V3_OTA_NUM_TOKENS                       (120)
#define FOG_V3_OTA_UPLOAD_RES_TOKENS                (40)

#define FOG_V3_OTA_CHECK_PRODUCT_ID_MAX_LEN         (64)
#define FOG_V3_OTA_CHECK_DEVICE_ID_MAX_LEN          (64)
#define FOG_V3_OTA_CHECK_DEVICE_PASSWORD_MAX_LEN    (64)

#define FOG_V3_OTA_COMPONENT_MD5_MAX_LEN            (64)
#define FOG_V3_OTA_COMPONENT_VERSION_MAX_LEN        (32)
#define FOG_V3_OTA_COMPONENT_NAME_MAX_LEN           (64)

#define FOG_V3_OTA_CUSTOM_STRING_MAX_LEN            (256)
#define FOG_V3_OTA_FILE_URL_MAX_LEN                 (256)

#define FOG_V3_OTA_MAX_COMPONENT_NUM                (4)

#define FOG_V3_OTA_REQ_BUFF_LEN                     (512)       //request buff len
#define FOG_V3_OTA_RES_BUFF_LEN                     (2048)      //resonse buff len

#define FOG_V3_OTA_DOMAIN_MAX_LEN                   (64)

#define FOG_V3_OTA_DOMAIN                           ("ota.fogcloud.io")

#define FOG_V3_PRODUCT_ID_OTA_DOMAIN                FOG_V3_OTA_DOMAIN
#define FOG_V3_PRODUCT_ID_OTA_PORT                  (443)
#define FOG_V3_PRODUCT_ID_OTA_PORT_NOSSL            (80)
#define FOG_V3_PRODUCT_ID_OTA_URI                   ("v3/ota/product/")
#define FOG_V3_PRODUCT_ID_OTA_STR                   ("PUT /%s HTTP/1.1\r\nHost: %s\r\nConnection: Close\r\nContent-Type: application/json\r\nContent-Length: %d\r\n\r\n%s")
#define FOG_V3_PRODUCT_ID_OTA_TIMEOUT               (4000) //4s

#define FOG_V3_DEVICE_ID_OTA_DOMAIN                 FOG_V3_OTA_DOMAIN
#define FOG_V3_DEVICE_ID_OTA_PORT                   (443)
#define FOG_V3_DEVICE_ID_OTA_PORT_NOSSL             (80)
#define FOG_V3_DEVICE_ID_OTA_URI                    ("v3/ota/device/")
#define FOG_V3_DEVICE_ID_OTA_STR                    ("PUT /%s HTTP/1.1\r\nHost: %s\r\nConnection: Close\r\nContent-Type: application/json\r\nContent-Length: %d\r\n\r\n%s")
#define FOG_V3_DEVICE_ID_OTA_TIMEOUT                (4000) //4s

#define FOG_V3_PRODUCT_ID_OTA_LOG_DOMAIN            FOG_V3_OTA_DOMAIN
#define FOG_V3_PRODUCT_ID_OTA_LOG_PORT              (443)
#define FOG_V3_PRODUCT_ID_OTA_LOG_PORT_NOSSL        (80)
#define FOG_V3_PRODUCT_ID_OTA_LOG_URI               ("v3/ota/product/log/")
#define FOG_V3_PRODUCT_ID_OTA_LOG_STR               ("PUT /%s HTTP/1.1\r\nHost: %s\r\nConnection: Close\r\nContent-Type: application/json\r\nContent-Length: %d\r\n\r\n%s")
#define FOG_V3_PRODUCT_ID_OTA_LOG_TIMEOUT           (4000) //4s

#define FOG_V3_DEVICE_ID_OTA_LOG_DOMAIN             FOG_V3_OTA_DOMAIN
#define FOG_V3_DEVICE_ID_OTA_LOG_PORT               (443)
#define FOG_V3_DEVICE_ID_OTA_LOG_PORT_NOSSL         (80)
#define FOG_V3_DEVICE_ID_OTA_LOG_URI                ("v3/ota/device/log/")
#define FOG_V3_DEVICE_ID_OTA_LOG_STR                ("PUT /%s HTTP/1.1\r\nHost: %s\r\nConnection: Close\r\nContent-Type: application/json\r\nContent-Length: %d\r\n\r\n%s")
#define FOG_V3_DEVICE_ID_OTA_LOG_TIMEOUT            (4000) //4s

typedef struct _FOG_V3_OTA_SUCCESS_INFO_S
{
    uint32_t ota_len;
    uint16_t crc;
} FOG_V3_OTA_SUCCESS_INFO_S;

typedef struct _FOG_V3_OTA_MEMBER_INFO_S
{
    char file_url[FOG_V3_OTA_FILE_URL_MAX_LEN];
    char ota_version[FOG_V3_OTA_COMPONENT_VERSION_MAX_LEN];
    char component_name[FOG_V3_OTA_COMPONENT_NAME_MAX_LEN];
    char file_md5[FOG_V3_OTA_COMPONENT_MD5_MAX_LEN];
    char custom_string[FOG_V3_OTA_CUSTOM_STRING_MAX_LEN];
} FOG_V3_OTA_COMPONENT_INFO_S;

typedef struct _FOG_V3_OTA_CHECK_RESPONSE_S
{
    int32_t ota_task_id;
    uint8_t ota_component_num;
    FOG_V3_OTA_COMPONENT_INFO_S ota_component[FOG_V3_OTA_MAX_COMPONENT_NUM];
} FOG_V3_OTA_CHECK_RESPONSE_S;

typedef struct _FOG_V3_OTA_UPLOAD_LOG_INFO_DEVICE_ID_S
{
    bool is_ota_success;            //is ota success(must)
    char *fog_v3_device_id;         //fog v3 device id(must)
    char *fog_v3_device_password;   //fog v3 device password(must)
    int32_t ota_task_id;            //fog v3 ota task id(must)
    char *component_name_array[FOG_V3_OTA_MAX_COMPONENT_NUM]; //component name array, array member can be NULL. no more than FOG_V3_OTA_MAX_COMPONENT_NUM.
    char *customize_str;            //user customize string, can be NULL
} FOG_V3_OTA_UPLOAD_LOG_INFO_DEVICE_ID_S;

typedef struct _FOG_V3_OTA_UPLOAD_LOG_INFO_PRODUCT_ID_S
{
    bool is_ota_success;            //is ota success(must)
    char *fog_v3_product_id;        //fog v3 product id(must)
    char *fog_v3_device_sn;         //fog v3 device sn(must)
    int32_t ota_task_id;            //fog v3 ota task id(must)
    char *component_name_array[FOG_V3_OTA_MAX_COMPONENT_NUM]; //component name array, array member can be NULL. no more than FOG_V3_OTA_MAX_COMPONENT_NUM.
    char *customize_str;            //user customize string, can be NULL
} FOG_V3_OTA_UPLOAD_LOG_INFO_PRODUCT_ID_S;

/**
 * @brief  fog v3 ota check with device id and device password.
 * @param  host_name[in]: server domain name.
 * @param  is_ssl[in]: is use ssl.
 * @param  fog_v3_device_id[in]: fog v3 device id.
 * @param  fog_v3_device_password[in]: fog v3 device password.
 * @param  ota_component_name[in]: fog v3 ota component name in ota task, can be NULL! if ota_component_name is null, all the component information will be return.
 * @param  ota_response[in/out]: the response of ota check, max check number is FOG_V3_OTA_MAX_MEMBER_NUM.
 * @retval kNoErr is returned on success, otherwise, kXXXErr is returned.
 */
int fog_v3_ota_check_with_device_id(const char *host_name, bool is_ssl, const char *fog_v3_device_id, const char *fog_v3_device_password, const char *ota_component_name, FOG_V3_OTA_CHECK_RESPONSE_S *ota_response);

/**
 * @brief  fog v3 ota check with product id.
 * @param  host_name[in]: server domain name, if host_name is NULL, will use default domain name.
 * @param  is_ssl[in]: is use ssl.
 * @param  fog_v3_product_id[in]: fog v3 product id.
 * @param  fog_v3_device_sn[in]: fog v3 device sn, can be NULL. fog server will remember which device check the ota information.
 * @param  ota_component_name[in]: fog v3 ota component name in ota task, can be NULL. if ota_member_name is null, all the member ota info will be return.
 * @param  version[in]: the ota member name version. if the ota_member_name is NULL, the version is not used.
 * @param  ota_response[in/out]: the response of ota check, max check number is FOG_V3_OTA_MAX_MEMBER_NUM.
 * @retval kNoErr is returned on success, otherwise, kXXXErr is returned.
 */
int fog_v3_ota_check_with_product_id(const char *host_name, bool is_ssl, const char *fog_v3_product_id, const char *fog_v3_device_sn, const char *ota_component_name, FOG_V3_OTA_CHECK_RESPONSE_S *ota_response);

/**
 * @brief  fog v3 ota upload log with device id and device password.
 * @param  host_name[in]: server domain name, if host_name is NULL, will use default domain name.
 * @param  is_ssl[in]: is use ssl.
 * @param  FOG_V3_OTA_UPLOAD_LOG_INFO_S[in]: fog v3 ota upload log info struct.
 * @retval kNoErr is returned on success, otherwise, kXXXErr is returned.
 */
int fog_v3_ota_upload_log_with_device_id( const char *host_name, bool is_ssl, FOG_V3_OTA_UPLOAD_LOG_INFO_DEVICE_ID_S *fog_v3_ota_upload_log_info_p );

/**
 * @brief  fog v3 ota upload log with product id and device sn
 * @param  host_name[in]: server domain name, if host_name is NULL, will use default domain name.
 * @param  is_ssl[in]: is use ssl.
 * @param  FOG_V3_OTA_UPLOAD_LOG_INFO_S[in]: fog v3 ota upload log info struct.
 * @retval kNoErr is returned on success, otherwise, kXXXErr is returned.
 */
int fog_v3_ota_upload_log_with_product_id( const char *host_name, bool is_ssl, FOG_V3_OTA_UPLOAD_LOG_INFO_PRODUCT_ID_S *fog_v3_ota_upload_log_info_p );
#endif
