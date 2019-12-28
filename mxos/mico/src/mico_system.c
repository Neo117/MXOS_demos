#include "mico.h"
#include "mkv.h"


OSStatus mico_system_notify_register( mico_notify_types_t notify_type, void* functionAddress, void* arg )
{
  if (notify_type == mico_notify_DHCP_COMPLETED)
    return 0;
  if (notify_type == mico_notify_WIFI_SCAN_COMPLETED) {
    _system_notify_register(notify_type, functionAddress, arg);
  } else if (notify_type == mico_notify_WIFI_SCAN_ADV_COMPLETED) {
    _system_notify_register(notify_type, functionAddress, arg);
  } else {
    return mxos_system_notify_register(notify_type, functionAddress, arg);
  }
}

OSStatus mico_system_notify_remove( mico_notify_types_t notify_type, void *functionAddress )
{
  if (notify_type == mico_notify_DHCP_COMPLETED)
    return 0;
  if (notify_type == mico_notify_WIFI_SCAN_COMPLETED) {
    _system_notify_remove(notify_type, functionAddress);
  } else if (notify_type == mico_notify_WIFI_SCAN_ADV_COMPLETED) {
    _system_notify_remove(notify_type, functionAddress);
  } else {
    return mxos_system_notify_remove(notify_type, functionAddress);
  }
}

OSStatus mico_system_notify_remove_all( mico_notify_types_t notify_type)
{
  if (notify_type == mico_notify_DHCP_COMPLETED)
    return 0;
  if (notify_type == mico_notify_WIFI_SCAN_COMPLETED) {
    _system_notify_remove_all(notify_type);
  } else if (notify_type == mico_notify_WIFI_SCAN_ADV_COMPLETED) {
    _system_notify_remove_all(notify_type);
  } else {
    return mxos_system_notify_remove_all(notify_type);
  }
}

micoMemInfo_t* MicoGetMemoryInfo( void )
{
    static micoMemInfo_t info;

    mos_mallinfo_t *p = mos_mallinfo();
    
    info.allocted_memory = p->total - p->free;
    info.free_memory = p->free;
    info.num_of_chunks = p->chunks;
    info.total_memory = p->total;
    
    return &info;
}

void MicoSystemReboot( void )
{
    mxos_sys_reboot();
}

WEAK int main(void)
{
	return application_start();
}

static int user_config_data_size = 0;
static void* user_config_data = NULL;

void* mico_system_context_init( uint32_t size_of_user_data )
{
    char kv_name[16];
    int len, i, err, blocknum, left;
    uint8_t *p;

    mxos_system_init();
    if (size_of_user_data != 0) {
        user_config_data = malloc(size_of_user_data);
        user_config_data_size = size_of_user_data;
        p = user_config_data;
        blocknum = (user_config_data_size+KV_MAX_VAL_LEN-1)/KV_MAX_VAL_LEN;
        left =user_config_data_size;
        memset(p, 0xFF, user_config_data_size);
        for(i=0; i<blocknum; i++) {
            if (left > KV_MAX_VAL_LEN) {
                len = KV_MAX_VAL_LEN;
            } else {
                len = left;
            }
            left -= len;
            sprintf(kv_name, "_mico_kv%d_", i);
            printf("kv get %s, p %p, len %d \r\n", kv_name, p, len);
            err = mkv_item_get(kv_name, p, &len);
            printf("kv get %s, p %p, len %d, err %d\r\n", kv_name, p, len, err);
            p += len;
        }
    }
    return mxos_system_context_get();
}

OSStatus mico_system_init( mico_Context_t* in_context )
{
    // mxos_system_init is done in mico_system_context_init  
    return 0;
}

mico_Context_t* mico_system_context_get( void )
{
    return mxos_system_context_get();
}

void* mico_system_context_get_user_data( mico_Context_t* const in_context )
{
    return user_config_data;
}

OSStatus mico_system_context_restore( mico_Context_t* const in_context )
{
    mxos_system_context_restore(in_context);
    appRestoreDefault_callback(user_config_data, user_config_data_size);
}


OSStatus mico_system_context_update( mico_Context_t* const in_context )
{
    char kv_name[16];
    int len, i, err, blocknum, left;
    uint8_t *p;
    
    mxos_system_context_update(in_context);
    if (user_config_data != NULL) {
        p = user_config_data;
        blocknum = (user_config_data_size+KV_MAX_VAL_LEN-1)/KV_MAX_VAL_LEN;
        left =user_config_data_size;
        for(i=0; i<blocknum; i++) {
            if (left > KV_MAX_VAL_LEN) {
                len = KV_MAX_VAL_LEN;
            } else {
                len = left;
            }
            left -= len;
            sprintf(kv_name, "_mico_kv%d_", i);
            printf("kv set %s, len %d\r\n", kv_name, len);
            err = mkv_item_set(kv_name, p, len);
            printf("kv set %s, return %d\r\n", kv_name, err);
            p += len;
        }
    }
}

