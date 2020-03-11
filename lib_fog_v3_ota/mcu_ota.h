#ifndef _MCU_OTA_H_
#define _MCU_OTA_H_

#ifndef MCU_OTA_DEBUG_ENABLE
#define MCU_OTA_DEBUG_ENABLE                        (1)
#endif

#define MCU_V3_OTA_BIN_BUFF_LEN                     (2048)
#define MCU_OTA_MD5_MAX_LEN                         (64)

typedef enum {
    FOG_V3_OTA_STATE_START,
    FOG_V3_OTA_STATE_LOADING,
    FOG_V3_OTA_STATE_FAILED,
    FOG_V3_OTA_STATE_FAILED_AND_RETRY,
    FOG_V3_OTA_STATE_FILE_DOWNLOAD_SUCCESS,
    FOG_V3_OTA_STATE_MD5_SUCCESS,
    FOG_V3_OTA_STATE_MD5_FAILED,
    FOG_V3_OTA_STATE_MAX,
    FOG_V3_OTA_STATE_NONE
}FOG_V3_OTA_STATE_E; //ota state

typedef void (*FOG_V3_OTA_STATE_CB) (FOG_V3_OTA_STATE_E state, uint32_t sys_args);

/**
 * @brief fog v3 module ota function
 * @param  ota_url[in]: module ota url,sopport http and https.
 * @param  ota_file_md5[in]: ota file md5.
 * @param  fog_v3_ota_state_callback[in]: fog v3 ota state callback
 * @param  is_block[in]: is block until file download finish
 * @retval kNoErr is returned on success, otherwise, kXXXErr is returned.
 */
int fog_v3_module_ota( const char *ota_url, const char *ota_file_md5, FOG_V3_OTA_STATE_CB fog_v3_ota_state_callback, bool is_block);

#endif
