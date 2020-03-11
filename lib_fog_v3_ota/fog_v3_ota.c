#include "mxos.h"
#include "fog_v3_ota.h"
#include "jsmn_parser.h"
#include "json_generator.h"

#if (FOG_V3_OTA_DEBUG_ENABLE == 1)
#define app_log(M, ...)             custom_log("FOG_V3_OTA", M, ##__VA_ARGS__)
#elif (FOG_V3_OTA_DEBUG_ENABLE == 0)
#define app_log(M, ...)
#else
#error "FOG_V3_OTA_DEBUG_ENABLE not define!"
#endif

/**
 * @brief  process ota upload log response
 * @param  http_response[in]: json response
 * @retval kNoErr is returned on success, otherwise, kXXXErr is returned.
 */
static int process_ota_upload_log_response( const char *http_response)
{
    int err = kGeneralErr;
    jsontok_t json_tokens[FOG_V3_OTA_UPLOAD_RES_TOKENS];    // Define an array of JSON tokens
    jobj_t jobj;
    int code = -1;

    require_string( http_response != NULL, exit, "body is NULL!!!" );

    require_string( ((*http_response == '{') && (*(http_response + strlen( http_response ) - 1) == '}')), exit, "http body JSON format error" );

    // Initialise the JSON parser and parse the given string
    err = json_init( &jobj, json_tokens, FOG_V3_OTA_UPLOAD_RES_TOKENS, (char *) http_response, strlen( http_response ) );
    require_action_string( err == WM_SUCCESS, exit, err = kGeneralErr, "parse error!" );

    err = json_get_composite_object( &jobj, "meta" );
    require_action_string( err == WM_SUCCESS, exit, err = kGeneralErr, "get meta error!" );

    //get code
    err = json_get_val_int( &jobj, "code", &code );
    require_action_string( err == WM_SUCCESS, exit, err = kGeneralErr, "get code error!" );

    err = json_release_composite_object( &jobj );
    require_action_string( err == WM_SUCCESS, exit, err = kGeneralErr, "release meta error!" );

    require_action_string( code == 0, exit, err = kGeneralErr, "get ota upload log response code error!" );

    exit:
    if ( err != kNoErr )
    {
        app_log("http ota upload log response err, code = %d, response : %s", code, http_response);
    }

    return err;
}

/**
 * @brief  process ota json response
 * @param  http_response[in]: json response
 * @param  key_buff[in/out]: user key buff
 * @param  key_buff_length[in]: user key buff length
 * @retval kNoErr is returned on success, otherwise, kXXXErr is returned.
 */
static int process_ota_response( const char *http_response, FOG_V3_OTA_CHECK_RESPONSE_S *ota_response)
{
    int err = kGeneralErr;
    jsontok_t json_tokens[FOG_V3_OTA_NUM_TOKENS];    // Define an array of JSON tokens
    jobj_t jobj;
    int code = -1;
    int32_t component_num = 0;
    uint8_t i = 0;

    require_string( http_response != NULL && ota_response != NULL, exit, "param error" );

    require_string( ((*http_response == '{') && (*(http_response + strlen( http_response ) - 1) == '}')), exit, "http body JSON format error" );

    memset(ota_response, 0, sizeof(FOG_V3_OTA_CHECK_RESPONSE_S));

    // Initialise the JSON parser and parse the given string
    err = json_init( &jobj, json_tokens, FOG_V3_OTA_NUM_TOKENS, (char *) http_response, strlen( http_response ) );
    require_action_string( err == WM_SUCCESS, exit, err = kGeneralErr, "parse error!" );

    err = json_get_composite_object( &jobj, "meta" );
    require_action_string( err == WM_SUCCESS, exit, err = kGeneralErr, "get meta error!" );

    //get code
    err = json_get_val_int( &jobj, "code", &code );
    require_action_string( err == WM_SUCCESS, exit, err = kGeneralErr, "get code error!" );

    err = json_release_composite_object( &jobj );
    require_action_string( err == WM_SUCCESS, exit, err = kGeneralErr, "release meta error!" );

    require_action_string( code == 0, exit, err = kGeneralErr, "get ota response code error!" );

    err = json_get_composite_object( &jobj, "data" );
    require_action_string( err == WM_SUCCESS, exit, err = kGeneralErr, "get data error!" );

    err = json_get_val_int(&jobj, "otid", (int *)&(ota_response->ota_task_id));
    require_action_string(err == WM_SUCCESS, exit, err = kGeneralErr, "get ota task id error!");

    require_action_string( ota_response->ota_task_id > 0, exit, err = kNoErr, "ota task id error! may don't have ota task!" );

    err = json_get_array_object(&jobj, "files", (int *)&component_num);
    require_action_string(err == WM_SUCCESS, exit, err = kGeneralErr, "get files error!");

    require_action_string( component_num > 0 && component_num <= FOG_V3_OTA_MAX_COMPONENT_NUM, exit, err = kGeneralErr, "ota task id error!" );

    ota_response->ota_component_num = component_num; //set component_num

    for(i = 0; i < component_num; i ++)
    {
        err = json_array_get_composite_object(&jobj, i);
        require_action_string(err == WM_SUCCESS, exit, err = kGeneralErr, "get array error!");

        err = json_get_val_str( &jobj, "file_url",  ota_response->ota_component[i].file_url, FOG_V3_OTA_FILE_URL_MAX_LEN);
        require_action_string( err == WM_SUCCESS, exit, err = kGeneralErr, "get file url error!" );

        err = json_get_val_str( &jobj, "component",  ota_response->ota_component[i].component_name, FOG_V3_OTA_COMPONENT_NAME_MAX_LEN);
        require_action_string( err == WM_SUCCESS, exit, err = kGeneralErr, "get file url error!" );

        err = json_get_val_str( &jobj, "md5",  ota_response->ota_component[i].file_md5, FOG_V3_OTA_COMPONENT_MD5_MAX_LEN);
        require_action_string( err == WM_SUCCESS, exit, err = kGeneralErr, "get file url error!" );

        err = json_get_val_str( &jobj, "version",  ota_response->ota_component[i].ota_version, FOG_V3_OTA_COMPONENT_VERSION_MAX_LEN);
        require_action_string( err == WM_SUCCESS, exit, err = kGeneralErr, "get file url error!" );

        err = json_get_val_str( &jobj, "customize",  ota_response->ota_component[i].custom_string, FOG_V3_OTA_CUSTOM_STRING_MAX_LEN);
        require_action_string( err == WM_SUCCESS, exit, err = kGeneralErr, "get file url error!" );

        err = json_array_release_composite_object(&jobj);
        require_action_string(err == WM_SUCCESS, exit, err = kGeneralErr, "get array error!");

        //app_log("[%d]url:%s", i, ota_response->ota_component[i].file_url);
        //app_log("[%d]name:%s", i, ota_response->ota_component[i].component_name);
        //app_log("[%d]md5:%s", i, ota_response->ota_component[i].file_md5);
        //app_log("[%d]version:%s", i, ota_response->ota_component[i].ota_version);
        //app_log("[%d]custom_string:%s", i, ota_response->ota_component[i].custom_string);
    }

    err = json_release_composite_object( &jobj );
    require_action_string( err == WM_SUCCESS, exit, err = kGeneralErr, "release jobj error!" );

    exit:
    if ( err != kNoErr )
    {
        app_log("http get time response err, code = %d, response : %s", code, http_response);
    }

    return err;
}

/**
 * @brief  fog v3 ota check with device id and device password.
 * @param  host_name[in]: server domain name, if host_name is NULL, will use default domain name.
 * @param  is_ssl[in]: is use ssl.
 * @param  fog_v3_device_id[in]: fog v3 device id.
 * @param  fog_v3_device_password[in]: fog v3 device password.
 * @param  ota_component_name[in]: fog v3 ota component name in ota task, can be NULL! if ota_component_name is null, all the component information will be return.
 * @param  ota_response[in/out]: the response of ota check, max check number is FOG_V3_OTA_MAX_MEMBER_NUM.
 * @retval kNoErr is returned on success, otherwise, kXXXErr is returned.
 */
int fog_v3_ota_check_with_device_id(const char *host_name, bool is_ssl, const char *fog_v3_device_id, const char *fog_v3_device_password, const char *ota_component_name, FOG_V3_OTA_CHECK_RESPONSE_S *ota_response)
{
    int err = kGeneralErr;
    HTTP_REQ_S https_req = HTTP_REQ_INIT_PARAM;
    const char *http_body_str_has_component = "{\"deviceid\":\"%s\",\"password\":\"%s\",\"component\":\"%s\"}";
    const char *http_body_str_no_component = "{\"deviceid\":\"%s\",\"password\":\"%s\"}";
    char http_body_buff[256] = { 0 };

    require_action(fog_v3_device_id != NULL && fog_v3_device_password != NULL && ota_response != NULL, exit, err = kParamErr);
    if(host_name != NULL)
    {
        require_action(strlen(host_name) < FOG_V3_OTA_DOMAIN_MAX_LEN, exit, err = kParamErr);
    }

    memset( &https_req, 0, sizeof(https_req) );

    https_req.domain_name = ((host_name == NULL) ? FOG_V3_DEVICE_ID_OTA_DOMAIN : (char *)host_name);
    https_req.port = ((is_ssl == true) ? FOG_V3_DEVICE_ID_OTA_PORT: FOG_V3_DEVICE_ID_OTA_PORT_NOSSL);
    https_req.is_success = false;
    https_req.timeout_ms = FOG_V3_DEVICE_ID_OTA_TIMEOUT;

    https_req.http_req = malloc( FOG_V3_OTA_REQ_BUFF_LEN );
    require_string( https_req.http_req != NULL, exit, "malloc error" );
    memset( https_req.http_req, 0, FOG_V3_OTA_REQ_BUFF_LEN );

    https_req.http_res = malloc( FOG_V3_OTA_RES_BUFF_LEN );
    require_string( https_req.http_res != NULL, exit, "malloc error" );
    memset( https_req.http_res, 0, FOG_V3_OTA_RES_BUFF_LEN );
    https_req.res_len = FOG_V3_OTA_RES_BUFF_LEN;

    if(ota_component_name != NULL)
    {
        sprintf( http_body_buff, http_body_str_has_component, fog_v3_device_id, fog_v3_device_password, ota_component_name );
    }else
    {
        sprintf( http_body_buff, http_body_str_no_component, fog_v3_device_id, fog_v3_device_password );
    }

    if(host_name == NULL)
    {
        sprintf( https_req.http_req, FOG_V3_DEVICE_ID_OTA_STR, FOG_V3_DEVICE_ID_OTA_URI, FOG_V3_DEVICE_ID_OTA_DOMAIN, strlen(http_body_buff), http_body_buff );
    }else
    {
        sprintf( https_req.http_req, FOG_V3_DEVICE_ID_OTA_STR, FOG_V3_DEVICE_ID_OTA_URI, host_name, strlen(http_body_buff), http_body_buff );
    }

    https_req.req_len = strlen( https_req.http_req );

    app_log("http request:\r\n%s", https_req.http_req);

    if(is_ssl == true)
    {
        err = http_short_connection_ssl( &https_req );
        require_noerr_string( err, exit, "https error!" );
    }else
    {
        err = http_short_connection_nossl( &https_req );
        require_noerr_string( err, exit, "http error!" );
    }


    require_action_string(https_req.is_success == true, exit, err = kGeneralErr, "https request error!");

    app_log("[device id ota check]http response:%s", https_req.http_res);

    err = process_ota_response(https_req.http_res, ota_response);
    require_noerr_string( err, exit, "ota check with device id error!" );

    exit:
    //free request buff
    if ( https_req.http_req != NULL )
    {
        free( https_req.http_req );
        https_req.http_req = NULL;
    }

    //free response buff
    if ( https_req.http_res != NULL )
    {
        free( https_req.http_res );
        https_req.http_res = NULL;
    }

    return err;
}

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
int fog_v3_ota_check_with_product_id(const char *host_name, bool is_ssl, const char *fog_v3_product_id, const char *fog_v3_device_sn, const char *ota_component_name, FOG_V3_OTA_CHECK_RESPONSE_S *ota_response)
{
    int err = kGeneralErr;
    HTTP_REQ_S https_req = HTTP_REQ_INIT_PARAM;
    char http_body_buff[256] = { 0 };
    struct json_str jstr;

    require_action(fog_v3_product_id != NULL && ota_response != NULL, exit, err = kParamErr);
    if(host_name != NULL)
    {
        require_action(strlen(host_name) < FOG_V3_OTA_DOMAIN_MAX_LEN, exit, err = kParamErr);
    }

    memset( &https_req, 0, sizeof(https_req) );

    https_req.domain_name = ((host_name == NULL) ? FOG_V3_PRODUCT_ID_OTA_DOMAIN : (char *)host_name);
    https_req.port = (is_ssl == true) ? FOG_V3_PRODUCT_ID_OTA_PORT: FOG_V3_PRODUCT_ID_OTA_PORT_NOSSL;
    https_req.is_success = false;
    https_req.timeout_ms = FOG_V3_PRODUCT_ID_OTA_TIMEOUT;

    https_req.http_req = malloc( FOG_V3_OTA_REQ_BUFF_LEN );
    require_string( https_req.http_req != NULL, exit, "malloc error" );
    memset( https_req.http_req, 0, FOG_V3_OTA_REQ_BUFF_LEN );

    https_req.http_res = malloc( FOG_V3_OTA_RES_BUFF_LEN );
    require_string( https_req.http_res != NULL, exit, "malloc error" );
    memset( https_req.http_res, 0, FOG_V3_OTA_RES_BUFF_LEN );
    https_req.res_len = FOG_V3_OTA_RES_BUFF_LEN;

    //generate json str...
    json_str_init(&jstr, http_body_buff, sizeof(http_body_buff));

    err = json_start_object(&jstr);
    require_string(err == WM_SUCCESS, exit, "start object error!");

    err = json_set_val_str(&jstr, "productid", fog_v3_product_id);
    require_string(err == WM_SUCCESS, exit, "add productid error!");

    if(fog_v3_device_sn != NULL)
    {
        err = json_set_val_str(&jstr, "dsn", fog_v3_device_sn);
        require_string(err == WM_SUCCESS, exit, "add dsn error!");
    }

    if ( ota_component_name != NULL )
    {
        err = json_set_val_str( &jstr, "component", ota_component_name );
        require_string( err == WM_SUCCESS, exit, "add component error!" );
    }

    err = json_close_object(&jstr);
    require_string(err == WM_SUCCESS, exit, "close json obj error!");

    if ( host_name == NULL )
    {
        sprintf( https_req.http_req, FOG_V3_PRODUCT_ID_OTA_STR, FOG_V3_PRODUCT_ID_OTA_URI, FOG_V3_PRODUCT_ID_OTA_DOMAIN, strlen(http_body_buff), http_body_buff );
    }else
    {
        sprintf( https_req.http_req, FOG_V3_PRODUCT_ID_OTA_STR, FOG_V3_PRODUCT_ID_OTA_URI, host_name, strlen(http_body_buff), http_body_buff );
    }

    https_req.req_len = strlen( https_req.http_req );

    app_log("http request:\r\n%s", https_req.http_req);

    if ( is_ssl == true )
    {
        err = http_short_connection_ssl( &https_req );
        require_noerr_string( err, exit, "https error!" );
    }else
    {
        err = http_short_connection_nossl( &https_req );
        require_noerr_string( err, exit, "http error!" );
    }

    require_action_string(https_req.is_success == true, exit, err = kGeneralErr, "https request error!");

    app_log("[product id ota check]http response:%s", https_req.http_res);

    err = process_ota_response(https_req.http_res, ota_response);
    require_noerr_string( err, exit, "ota check with product id error!" );

    exit:
    //free request buff
    if ( https_req.http_req != NULL )
    {
        free( https_req.http_req );
        https_req.http_req = NULL;
    }

    //free response buff
    if ( https_req.http_res != NULL )
    {
        free( https_req.http_res );
        https_req.http_res = NULL;
    }

    return err;
}

/**
 * @brief  fog v3 ota upload log with device id and device password.
 * @param  host_name[in]: server domain name, if host_name is NULL, will use default domain name.
 * @param  is_ssl[in]: is use ssl.
 * @param  FOG_V3_OTA_UPLOAD_LOG_INFO_S[in]: fog v3 ota upload log info struct.
 * @retval kNoErr is returned on success, otherwise, kXXXErr is returned.
 */
int fog_v3_ota_upload_log_with_device_id( const char *host_name, bool is_ssl, FOG_V3_OTA_UPLOAD_LOG_INFO_DEVICE_ID_S *fog_v3_ota_upload_log_info_p )
{
    int err = kGeneralErr;
    HTTP_REQ_S https_req = HTTP_REQ_INIT_PARAM;
    char http_body_buff[512] = { 0 };
    struct json_str jstr;
    uint32_t i = 0;

    require_action(fog_v3_ota_upload_log_info_p != NULL, exit, err = kParamErr);
    require_action(fog_v3_ota_upload_log_info_p->fog_v3_device_id != NULL, exit, err = kParamErr);
    require_action(fog_v3_ota_upload_log_info_p->fog_v3_device_password != NULL, exit, err = kParamErr);
    require_action(fog_v3_ota_upload_log_info_p->ota_task_id > 0, exit, err = kParamErr);
    if(host_name != NULL)
    {
        require_action(strlen(host_name) < FOG_V3_OTA_DOMAIN_MAX_LEN, exit, err = kParamErr);
    }

    memset( &https_req, 0, sizeof(https_req) );

    https_req.domain_name = ((host_name == NULL) ? FOG_V3_DEVICE_ID_OTA_LOG_DOMAIN : (char *)host_name);
    https_req.port = ((is_ssl == true) ? FOG_V3_DEVICE_ID_OTA_LOG_PORT : FOG_V3_DEVICE_ID_OTA_LOG_PORT_NOSSL);
    https_req.is_success = false;
    https_req.timeout_ms = FOG_V3_DEVICE_ID_OTA_LOG_TIMEOUT;

    https_req.http_req = malloc( FOG_V3_OTA_REQ_BUFF_LEN );
    require_string( https_req.http_req != NULL, exit, "malloc error" );
    memset( https_req.http_req, 0, FOG_V3_OTA_REQ_BUFF_LEN );

    https_req.http_res = malloc( FOG_V3_OTA_RES_BUFF_LEN );
    require_string( https_req.http_res != NULL, exit, "malloc error" );
    memset( https_req.http_res, 0, FOG_V3_OTA_RES_BUFF_LEN );
    https_req.res_len = FOG_V3_OTA_RES_BUFF_LEN;

    //generate json str...
    json_str_init(&jstr, http_body_buff, sizeof(http_body_buff));

    err = json_start_object(&jstr);
    require_string(err == WM_SUCCESS, exit, "start object error!");

    err = json_set_val_str(&jstr, "deviceid", fog_v3_ota_upload_log_info_p->fog_v3_device_id);
    require_string(err == WM_SUCCESS, exit, "add deviceid error!");

    err = json_set_val_str(&jstr, "password", fog_v3_ota_upload_log_info_p->fog_v3_device_password);
    require_string(err == WM_SUCCESS, exit, "add password error!");

    err = json_set_val_int(&jstr, "otid", fog_v3_ota_upload_log_info_p->ota_task_id);
    require_string(err == WM_SUCCESS, exit, "add otid error!");

    err = json_set_val_bool(&jstr, "isfinished", fog_v3_ota_upload_log_info_p->is_ota_success);
    require_string(err == WM_SUCCESS, exit, "add otid error!");

    if ( fog_v3_ota_upload_log_info_p->customize_str != NULL )
    {
        err = json_set_val_str( &jstr, "customize", fog_v3_ota_upload_log_info_p->customize_str );
        require_string( err == WM_SUCCESS, exit, "add customize error!" );
    }

    json_push_array_object(&jstr, "components");

    for(i = 0; i < FOG_V3_OTA_MAX_COMPONENT_NUM; i ++)
    {
        if(fog_v3_ota_upload_log_info_p->component_name_array[i] != NULL)
        {
            if(strlen(fog_v3_ota_upload_log_info_p->component_name_array[i]) < FOG_V3_OTA_COMPONENT_NAME_MAX_LEN )
            {
                json_set_array_str(&jstr, fog_v3_ota_upload_log_info_p->component_name_array[i]);
            }else
            {
                app_log("[error] index:%lu, component name error!", i);
            }
        }
    }

    json_pop_array_object(&jstr);

    err = json_close_object(&jstr);
    require_string(err == WM_SUCCESS, exit, "close json obj error!");

    if ( host_name == NULL )
    {
        sprintf( https_req.http_req, FOG_V3_DEVICE_ID_OTA_LOG_STR, FOG_V3_DEVICE_ID_OTA_LOG_URI, FOG_V3_DEVICE_ID_OTA_LOG_DOMAIN, strlen(http_body_buff), http_body_buff );
    }else
    {
        sprintf( https_req.http_req, FOG_V3_DEVICE_ID_OTA_LOG_STR, FOG_V3_DEVICE_ID_OTA_LOG_URI, host_name, strlen(http_body_buff), http_body_buff );
    }

    https_req.req_len = strlen( https_req.http_req );

    app_log("http request:\r\n%s", https_req.http_req);

    if ( is_ssl == true )
    {
        err = http_short_connection_ssl( &https_req );
        require_noerr_string( err, exit, "https error!" );
    }else
    {
        err = http_short_connection_nossl( &https_req );
        require_noerr_string( err, exit, "https error!" );
    }

    require_action_string(https_req.is_success == true, exit, err = kGeneralErr, "https request error!");

    err = process_ota_upload_log_response(https_req.http_res);
    require_noerr(err, exit);

    app_log("[device id ota upload]http response:%s", https_req.http_res);

    exit:
    //free request buff
    if ( https_req.http_req != NULL )
    {
        free( https_req.http_req );
        https_req.http_req = NULL;
    }

    //free response buff
    if ( https_req.http_res != NULL )
    {
        free( https_req.http_res );
        https_req.http_res = NULL;
    }

    return err;
}

/**
 * @brief  fog v3 ota upload log with product id and device sn
 * @param  host_name[in]: server domain name, if host_name is NULL, will use default domain name.
 * @param  is_ssl[in]: is use ssl.
 * @param  FOG_V3_OTA_UPLOAD_LOG_INFO_S[in]: fog v3 ota upload log info struct.
 * @retval kNoErr is returned on success, otherwise, kXXXErr is returned.
 */
int fog_v3_ota_upload_log_with_product_id( const char *host_name, bool is_ssl, FOG_V3_OTA_UPLOAD_LOG_INFO_PRODUCT_ID_S *fog_v3_ota_upload_log_info_p )
{
    int err = kGeneralErr;
    HTTP_REQ_S https_req = HTTP_REQ_INIT_PARAM;
    char http_body_buff[512] = { 0 };
    struct json_str jstr;
    uint32_t i = 0;

    require_action(fog_v3_ota_upload_log_info_p != NULL, exit, err = kParamErr);
    require_action(fog_v3_ota_upload_log_info_p->fog_v3_product_id != NULL, exit, err = kParamErr);
    require_action(fog_v3_ota_upload_log_info_p->fog_v3_device_sn != NULL, exit, err = kParamErr);
    require_action(fog_v3_ota_upload_log_info_p->ota_task_id > 0, exit, err = kParamErr);
    if(host_name != NULL)
    {
        require_action(strlen(host_name) < FOG_V3_OTA_DOMAIN_MAX_LEN, exit, err = kParamErr);
    }

    memset( &https_req, 0, sizeof(https_req) );

    https_req.domain_name = ((host_name == NULL) ? FOG_V3_PRODUCT_ID_OTA_LOG_DOMAIN : (char *)host_name);
    https_req.port = ((is_ssl == true) ? FOG_V3_PRODUCT_ID_OTA_LOG_PORT : FOG_V3_PRODUCT_ID_OTA_LOG_PORT_NOSSL);
    https_req.is_success = false;
    https_req.timeout_ms = FOG_V3_PRODUCT_ID_OTA_LOG_TIMEOUT;

    https_req.http_req = malloc( FOG_V3_OTA_REQ_BUFF_LEN );
    require_string( https_req.http_req != NULL, exit, "malloc error" );
    memset( https_req.http_req, 0, FOG_V3_OTA_REQ_BUFF_LEN );

    https_req.http_res = malloc( FOG_V3_OTA_RES_BUFF_LEN );
    require_string( https_req.http_res != NULL, exit, "malloc error" );
    memset( https_req.http_res, 0, FOG_V3_OTA_RES_BUFF_LEN );
    https_req.res_len = FOG_V3_OTA_RES_BUFF_LEN;

    //generate json str...
    json_str_init(&jstr, http_body_buff, sizeof(http_body_buff));

    err = json_start_object(&jstr);
    require_string(err == WM_SUCCESS, exit, "start object error!");

    err = json_set_val_str(&jstr, "productid", fog_v3_ota_upload_log_info_p->fog_v3_product_id);
    require_string(err == WM_SUCCESS, exit, "add productid error!");

    err = json_set_val_str(&jstr, "dsn", fog_v3_ota_upload_log_info_p->fog_v3_device_sn);
    require_string(err == WM_SUCCESS, exit, "add dsn error!");

    err = json_set_val_int(&jstr, "otid", fog_v3_ota_upload_log_info_p->ota_task_id);
    require_string(err == WM_SUCCESS, exit, "add otid error!");

    err = json_set_val_bool(&jstr, "isfinished", fog_v3_ota_upload_log_info_p->is_ota_success);
    require_string(err == WM_SUCCESS, exit, "add otid error!");

    if ( fog_v3_ota_upload_log_info_p->customize_str != NULL )
    {
        err = json_set_val_str( &jstr, "customize", fog_v3_ota_upload_log_info_p->customize_str );
        require_string( err == WM_SUCCESS, exit, "add customize error!" );
    }

    json_push_array_object(&jstr, "components");

    for(i = 0; i < FOG_V3_OTA_MAX_COMPONENT_NUM; i ++)
    {
        if(fog_v3_ota_upload_log_info_p->component_name_array[i] != NULL)
        {
            if(strlen(fog_v3_ota_upload_log_info_p->component_name_array[i]) < FOG_V3_OTA_COMPONENT_NAME_MAX_LEN )
            {
                json_set_array_str(&jstr, fog_v3_ota_upload_log_info_p->component_name_array[i]);
            }else
            {
                app_log("[error] index:%lu, component name error!", i);
            }
        }
    }

    json_pop_array_object(&jstr);

    err = json_close_object(&jstr);
    require_string(err == WM_SUCCESS, exit, "close json obj error!");

    if(host_name == NULL)
    {
        sprintf( https_req.http_req, FOG_V3_PRODUCT_ID_OTA_LOG_STR, FOG_V3_PRODUCT_ID_OTA_LOG_URI, FOG_V3_PRODUCT_ID_OTA_LOG_DOMAIN, strlen(http_body_buff), http_body_buff );
    }else
    {
        sprintf( https_req.http_req, FOG_V3_PRODUCT_ID_OTA_LOG_STR, FOG_V3_PRODUCT_ID_OTA_LOG_URI, host_name, strlen(http_body_buff), http_body_buff );
    }

    https_req.req_len = strlen( https_req.http_req );

    app_log("http request:\r\n%s", https_req.http_req);

    if(is_ssl == true)
    {
        err = http_short_connection_ssl( &https_req );
        require_noerr_string( err, exit, "https error!" );
    }else
    {
        err = http_short_connection_nossl( &https_req );
        require_noerr_string( err, exit, "http error!" );
    }

    require_action_string(https_req.is_success == true, exit, err = kGeneralErr, "https request error!");

    err = process_ota_upload_log_response(https_req.http_res);
    require_noerr(err, exit);

    app_log("[product id ota upload]http response:%s", https_req.http_res);

    exit:
    //free request buff
    if ( https_req.http_req != NULL )
    {
        free( https_req.http_req );
        https_req.http_req = NULL;
    }

    //free response buff
    if ( https_req.http_res != NULL )
    {
        free( https_req.http_res );
        https_req.http_res = NULL;
    }

    return err;
}
