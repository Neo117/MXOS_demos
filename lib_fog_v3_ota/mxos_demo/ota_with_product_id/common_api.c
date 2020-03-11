#include "mxos.h"

merr_t user_get_device_mac( char *mac_buff, uint32_t mac_buff_len )
{
    merr_t err = kGeneralErr;
    uint8_t mac[10] = {0};

    require((mac_buff != NULL) && (mac_buff_len >= 16), exit);

    mwifi_get_mac(mac);

    memset(mac_buff, 0, mac_buff_len);
    sprintf(mac_buff, "%02X%02X%02X%02X%02X%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

    err = kNoErr;
    exit:
    return err;
}
