#include "mico.h"
#include "mico_wlan.h"

typedef struct _Notify_list{
  void  *function;
  void  *arg;
  struct _Notify_list *next;
  void  *contex;
} _Notify_list_t;

/* for scan notify list*/
static _Notify_list_t* Notify_list[2] = {NULL};
static int _adv_scan=0, _is_scan_reg=0;


OSStatus micoWlanStart(network_InitTypeDef_st* para)
{
    mwifi_ip_attr_t ip;
    merr_t ret;
    
    if (para->wifi_mode == Station) {
        if (para->dhcpMode == DHCP_Client) {
            ret = mwifi_connect(para->wifi_ssid, para->wifi_key, 
                strlen(para->wifi_key), NULL, NULL);
        } else {
            memset(&ip, 0, sizeof(ip));
            memcpy(ip.dnserver, para->dnsServer_ip_addr, 16);
            memcpy(ip.gateway, para->gateway_ip_addr, 16);
            memcpy(ip.netmask, para->net_mask, 16);
            memcpy(ip.localip, para->local_ip_addr, 16);
            ret = mwifi_connect(para->wifi_ssid, para->wifi_key, 
                strlen(para->wifi_key), NULL, &ip);
        }
    } else {
        memset(&ip, 0, sizeof(ip));
        memcpy(ip.dnserver, para->dnsServer_ip_addr, 16);
        memcpy(ip.gateway, para->gateway_ip_addr, 16);
        memcpy(ip.netmask, para->net_mask, 16);
        memcpy(ip.localip, para->local_ip_addr, 16);
        ret = mwifi_softap_start(para->wifi_ssid, para->wifi_key, 0, &ip);
    }

    return ret;
}

OSStatus micoWlanStartAdv(network_InitTypeDef_adv_st* para)
{
    mwifi_ip_attr_t ip;
    mwifi_connect_attr_t attr;
    merr_t ret;
    
    memcpy(attr.bssid, para->ap_info.bssid, 6);
    attr.channel = para->ap_info.channel;
    attr.security = para->ap_info.security;
    
    if (para->dhcpMode == DHCP_Client) {
        ret = mwifi_connect(para->ap_info.ssid, para->key, 
            para->key_len, &attr, NULL);
    } else {
        memset(&ip, 0, sizeof(ip));
        memcpy(ip.dnserver, para->dnsServer_ip_addr, 16);
        memcpy(ip.gateway, para->gateway_ip_addr, 16);
        memcpy(ip.netmask, para->net_mask, 16);
        memcpy(ip.localip, para->local_ip_addr, 16);
        ret = mwifi_connect(para->ap_info.ssid, para->key, 
            para->key_len, &attr, &ip);
    }

    return ret;
}

OSStatus micoWlanGetIPStatus(IPStatusTypedef *outNetpara, WiFi_Interface inInterface)
{
    mwifi_ip_attr_t attr; 
    mwifi_if_t iface;
    int ret;
    uint8_t mac[6];
    
    if (inInterface == Station) {
        ret = mwifi_get_ip(&attr, STATION_INTERFACE);
        
    } else {
        ret = mwifi_get_ip(&attr, SOFTAP_INTERFACE);
    }
    if (ret != 0)
        return ret;
    memcpy(outNetpara->ip, attr.localip, 16);
    memcpy(outNetpara->mask, attr.netmask, 16);
    memcpy(outNetpara->gate, attr.gateway, 16);
    memcpy(outNetpara->dns, attr.dnserver, 16);
    memcpy(outNetpara->broadcastip, "255.255.255.255", 16);
    mwifi_get_mac(mac);
    sprintf(outNetpara->mac, "%02x%02x%02x%02x%02x%02x", mac[0],
		mac[1], mac[2], mac[3], mac[4], mac[5]);

    return ret;
}

OSStatus micoWlanGetLinkStatus(LinkStatusTypeDef *outStatus)
{
    mwifi_link_info_t info;
    int ret;
    
    ret = mwifi_get_link_info(&info);
    outStatus->is_connected = info.is_connected;
    memcpy(outStatus->bssid, info.bssid, 6);
    memcpy(outStatus->ssid, info.ssid, 32);
    outStatus->channel = info.channel;
    outStatus->rssi = info.rssi;
    
    return ret;
}

typedef void (*mico_notify_WIFI_SCAN_COMPLETE_function)           ( ScanResult *pApList, void * inContext );
typedef void (*mico_notify_WIFI_SCAN_ADV_COMPLETE_function)       ( ScanResult_adv *pApAdvList, void * inContext );

OSStatus _system_notify_register( mico_notify_types_t notify_type, void* functionAddress, void* arg )
{
  OSStatus err = kNoErr;
  _Notify_list_t *temp;
  _Notify_list_t *notify = (_Notify_list_t *)malloc(sizeof(_Notify_list_t));

  if (notify_type == mico_notify_WIFI_SCAN_COMPLETED)
    notify_type = 0;
  else
    notify_type = 1;
  temp =  Notify_list[notify_type];
  
  require_action(notify, exit, err = kNoMemoryErr);
  notify->function = functionAddress;
  notify->arg = arg;
  notify->next = NULL;
  if(Notify_list[notify_type] == NULL){
    Notify_list[notify_type] = notify;
    notify->next = NULL;
  }else{
    if(temp->function == functionAddress)
        return kNoErr;   //Nodify already exist
    while(temp->next!=NULL){
      temp = temp->next;
      if(temp->function == functionAddress)
        return kNoErr;   //Nodify already exist
    }
    temp->next = notify;
  }
exit:
  return err;
}

OSStatus _system_notify_remove( mico_notify_types_t notify_type, void *functionAddress )
{
  OSStatus err = kNoErr;
  _Notify_list_t *temp, *temp2;

  if (notify_type == mico_notify_WIFI_SCAN_COMPLETED)
    notify_type = 0;
  else
    notify_type = 1;
  
  temp = Notify_list[notify_type];
  temp2 = temp;
  do{
    if(temp->function == functionAddress){
      if(temp == Notify_list[notify_type]){  //first element
        Notify_list[notify_type] = Notify_list[notify_type]->next;
        free(temp);
      }else{
        temp2->next = temp->next;
        free(temp);
      }
       break;
    }
    require_action(temp->next!=NULL, exit, err = kNotFoundErr);
    temp2 = temp;
    temp = temp->next;
  }while(temp != NULL);

exit:
  return err;
}

OSStatus _system_notify_remove_all( mico_notify_types_t notify_type)
{
    _Notify_list_t *temp;

    if (notify_type == mico_notify_WIFI_SCAN_COMPLETED)
        notify_type = 0;
    else
        notify_type = 1;
    temp = Notify_list[notify_type];
    while(temp) {
        Notify_list[notify_type] = Notify_list[notify_type]->next;
        free(temp);
        temp = Notify_list[notify_type];
    }

    return kNoErr;
}

static void scan_adv_complete_cb(int num, mwifi_ap_info_t *ap_list)
{
    int i = 0;
    _Notify_list_t *temp =  Notify_list[1];
      if(temp == NULL)
        return;
      if (_adv_scan) {
        ScanResult_adv pApList;

        pApList.ApNum = num;
        pApList.ApList = malloc(num * sizeof(*pApList.ApList));
        if (pApList.ApList == NULL)
            return;
        for (i = 0; i < num; i++)
        {
            strcpy(pApList.ApList[i].ssid, ap_list[i].ssid);
            pApList.ApList[i].rssi = ap_list[i].rssi;
            memcpy(pApList.ApList[i].bssid, ap_list[i].bssid, 6);
            pApList.ApList[i].security = ap_list[i].security;
            pApList.ApList[i].channel = ap_list[i].channel;
        }
        do{
          ((mico_notify_WIFI_SCAN_COMPLETE_function)(temp->function))(&pApList, temp->arg);
          temp = temp->next;
        }while(temp!=NULL);
        free(pApList.ApList);
      } else {
        ScanResult pApList;

        pApList.ApNum = num;
        pApList.ApList = malloc(num * sizeof(*pApList.ApList));
        if (pApList.ApList == NULL)
            return;
        for (i = 0; i < num; i++)
        {
            strcpy(pApList.ApList[i].ssid, ap_list[i].ssid);
            pApList.ApList[i].rssi = ap_list[i].rssi;
        }
        do{
          ((mico_notify_WIFI_SCAN_COMPLETE_function)(temp->function))(&pApList, temp->arg);
          temp = temp->next;
        }while(temp!=NULL);
        free(pApList.ApList);
      }
}

void micoWlanStartScan(void)
{
    _adv_scan=0;
    if (_is_scan_reg==0) {
        mxos_system_notify_register(mxos_notify_WIFI_SCAN_COMPLETED, scan_adv_complete_cb, NULL);
        _is_scan_reg = 1;
    }
    mwifi_scan(NULL);
}

void micoWlanStartScanAdv(void)
{
    _adv_scan=1;
    if (_is_scan_reg==0) {
        mxos_system_notify_register(mxos_notify_WIFI_SCAN_COMPLETED, scan_adv_complete_cb, NULL);
        _is_scan_reg = 1;
    }
    mwifi_scan(NULL);
}

int mxchip_active_scan(char*ssid, int is_adv)
{
    _adv_scan=is_adv;
    if (_is_scan_reg==0) {
        mxos_system_notify_register(mxos_notify_WIFI_SCAN_COMPLETED, scan_adv_complete_cb, NULL);
        _is_scan_reg = 1;
    }
    mwifi_scan(ssid);
}


OSStatus micoWlanPowerOff(void)
{
    return mwifi_off();
}

OSStatus micoWlanPowerOn(void)
{
    return mwifi_on();
}

OSStatus micoWlanSuspend(void)
{
    mwifi_disconnect();
    mwifi_softap_stop();
    return 0;
}

OSStatus micoWlanSuspendStation(void)
{
    return mwifi_disconnect();
}

OSStatus micoWlanSuspendSoftAP(void)
{
    return mwifi_softap_stop();
}

void micoWlanEnablePowerSave(void)
{
    mwifi_ps_on();
}

void micoWlanDisablePowerSave(void)
{
    mwifi_ps_off();
}

int mico_wlan_monitor_rx_type(int type)
{
    return 0;
}

int mico_wlan_start_monitor(void)
{
    return mwifi_monitor_start();
}

int mico_wlan_stop_monitor(void)
{
    return mwifi_monitor_stop();
}

OSStatus mico_wlan_monitor_set_channel( uint8_t channel )
{
    return mwifi_monitor_set_channel(channel);
}

OSStatus mico_wlan_monitor_get_channel( uint8_t *channel )
{
    *channel = mwifi_monitor_get_channel();
    return 0;
}

void mico_wlan_register_monitor_cb(monitor_cb_t fn)
{
    mwifi_monitor_reg_cb(fn);
}

/** @brief  Send management frame
 */
OSStatus mico_wlan_send_mgnt(uint8_t *buffer, uint32_t length)
{
    return mwifi_monitor_send_frame(buffer, length);
}

OSStatus mico_wlan_custom_ie_add(wlan_if_t wlan_if, uint8_t *custom_ie, uint32_t len)
{
    mwifi_if_t iface;

    if(wlan_if == Station)
        iface = STATION_INTERFACE;
    else
        iface = SOFTAP_INTERFACE;
    return mwifi_custom_ie_add(iface,custom_ie,len);
}


OSStatus mico_wlan_custom_ie_delete(wlan_if_t wlan_if, custom_ie_delete_op_t op, uint8_t *option_data, uint32_t len)
{
    mwifi_if_t iface;

    if(wlan_if == Station)
        iface = STATION_INTERFACE;
    else
        iface = SOFTAP_INTERFACE;
    
    mwifi_custom_ie_remove(iface);
}

void mico_wlan_get_mac_address_by_interface( wlan_if_t wlan_if, uint8_t *mac )
{
    mwifi_get_mac(mac);
}

OSStatus mico_wlan_register_recv_mgnt(monitor_cb_t fn)
{
    return mwifi_reg_mgnt_cb(fn);
}

int mico_wlan_monitor_no_easylink(void)
{
}

