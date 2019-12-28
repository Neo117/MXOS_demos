/**
*********************************************************************************************************
*               Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     simple_ble_service.c
* @brief    simple BLE profile source file.
* @details  Demonstration of how to implement a self-definition profile.
* @author
* @date
* @version
*********************************************************************************************************
*/

#include <string.h>
#include "bt_config_service.h"
#include <gap.h>
#include "breeze_hal_ble.h"
#include "mxos.h"

#define app_log(format, ...)  custom_log("bt", format, ##__VA_ARGS__)

T_SERVER_ID bt_config_service_id;
/**<  Function pointer used to send event to application from ble config wifi profile. Initiated in bt_config_service_add_service. */
static P_FUN_SERVER_GENERAL_CB pfn_bt_config_service_cb = NULL;

extern ais_bt_init_t * bt_init_info;

/**< @brief  profile/service definition.  */
/**should changed according BT team **/
T_ATTRIB_APPL bt_config_service_tbl[] =
{
    /*----------------- uart data Service -------------------*/
    /* Attribute index 0 */
    /* <<Primary Service>>, .. */
    {
        (ATTRIB_FLAG_VALUE_INCL | ATTRIB_FLAG_LE),   /* wFlags     */
        {   /* bTypeValue */
            LO_WORD(GATT_UUID_PRIMARY_SERVICE),
            HI_WORD(GATT_UUID_PRIMARY_SERVICE),
            LO_WORD(BLE_UUID_AIS_SERVICE),              /* service UUID */
            HI_WORD(BLE_UUID_AIS_SERVICE)
        },
        UUID_16BIT_SIZE,                            /* bValueLen     */
        NULL,                                       /* pValueContext */
        GATT_PERM_READ                              /* wPermissions  */
    },

    /* Attribute index 1 */
    /* Data TX Characteristic */
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* wFlags */
        {   /* bTypeValue */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            GATT_CHAR_PROP_READ, /* characteristic properties */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* wPermissions */
    },
#if 1
    /* Attribute index 2 */
    /* Data TX Characteristic value  */
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* wFlags */
        {   /* bTypeValue */
            LO_WORD(BLE_UUID_AIS_RC),
            HI_WORD(BLE_UUID_AIS_RC),
        },
        0,                                          /* variable size */
        NULL,
        GATT_PERM_READ | 16 			/* wPermissions    */
    },
    /* Attribute index 3 */
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* wFlags */
        {   /* bTypeValue */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            GATT_CHAR_PROP_READ | GATT_CHAR_PROP_WRITE, /* characteristic properties */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* wPermissions */
    },
    /* Attribute index 4 */
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* wFlags */
        {   /* bTypeValue */
            LO_WORD(BLE_UUID_AIS_WC),
            HI_WORD(BLE_UUID_AIS_WC),
        },
        0,                                          /* variable size */
        NULL,
        GATT_PERM_READ | GATT_PERM_WRITE          /* wPermissions    */
    },
    /* <<Characteristic>> demo for indicate */
    /* Attribute index 5 */
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            (GATT_CHAR_PROP_READ | GATT_CHAR_PROP_INDICATE)               /* characteristic properties */
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* permissions */
    },
    /* Attribute index 6 */
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(BLE_UUID_AIS_IC),
            HI_WORD(BLE_UUID_AIS_IC)
        },
        0,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* permissions */
    },
    /* client characteristic configuration */
    {
        ATTRIB_FLAG_VALUE_INCL | ATTRIB_FLAG_CCCD_APPL,                 /* flags */
        {                                         /* type_value */
            LO_WORD(GATT_UUID_CHAR_CLIENT_CONFIG),
            HI_WORD(GATT_UUID_CHAR_CLIENT_CONFIG),
            /* NOTE: this value has an instantiation for each client, a write to */
            /* this attribute does not modify this default value:                */
            LO_WORD(GATT_CLIENT_CHAR_CONFIG_DEFAULT), /* client char. config. bit field */
            HI_WORD(GATT_CLIENT_CHAR_CONFIG_DEFAULT)
        },
        2,                                          /* bValueLen */
        NULL,
        (GATT_PERM_READ | GATT_PERM_WRITE)          /* permissions */
    },
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* wFlags */
        {   /* bTypeValue */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            GATT_CHAR_PROP_READ | GATT_CHAR_PROP_WRITE_NO_RSP, /* characteristic properties */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* wPermissions */
    },
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* wFlags */
        {   /* bTypeValue */
            LO_WORD(BLE_UUID_AIS_WWNRC),
            HI_WORD(BLE_UUID_AIS_WWNRC),
        },
        0,                                          /* variable size */
        NULL,
        GATT_PERM_READ          /* wPermissions    */
    },
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* wFlags */
        {   /* bTypeValue */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            GATT_CHAR_PROP_READ | GATT_CHAR_PROP_NOTIFY, /* characteristic properties */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* wPermissions */
    },
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* wFlags */
        {   /* bTypeValue */
            LO_WORD(BLE_UUID_AIS_NC),
            HI_WORD(BLE_UUID_AIS_NC),
        },
        0,                                          /* variable size */
        NULL,
        GATT_PERM_READ          /* wPermissions    */
    },
    /* client characteristic configuration */
    {
        ATTRIB_FLAG_VALUE_INCL | ATTRIB_FLAG_CCCD_APPL,                 /* flags */
        {                                          /* type_value */
            LO_WORD(GATT_UUID_CHAR_CLIENT_CONFIG),
            HI_WORD(GATT_UUID_CHAR_CLIENT_CONFIG),
            /* NOTE: this value has an instantiation for each client, a write to */
            /* this attribute does not modify this default value:                */
            LO_WORD(GATT_CLIENT_CHAR_CONFIG_DEFAULT), /* client char. config. bit field */
            HI_WORD(GATT_CLIENT_CHAR_CONFIG_DEFAULT)
        },
        2,                                          /* bValueLen */
        NULL,
        (GATT_PERM_READ | GATT_PERM_WRITE)          /* permissions */
    },
#endif
};


/**
  * @brief  Set service related data from application.
  *
  * @param[in] param_type            parameter type to set.
  * @param[in] len                   value length to be set.
  * @param[in] p_value             value to set.
  * @return parameter set result.
  * @retval 0 false
  * @retval 1 true
  */
bool bt_config_service_set_parameter(T_BTCONFIG_PARAM_TYPE param_type, uint16_t len, void *p_value)
{
	bool ret = true;

	app_log("bt_config_service_set_parameter");

	switch (param_type) {
		default:
			ret = false;
		break;
	}

	if (!ret) {
	    app_log("bt_config_service_set_parameter failed");
	}

	return ret;
}

void dump_raw_data(uint8_t *data, uint8_t length)
{
    printf("\r\n");
    for(int i=0;i<length;i++)
        printf("%02X ", data[i]);
    printf("\r\n");
}

bool bt_config_service_send_notify(uint8_t conn_id, uint8_t *p_data, uint16_t length)
{
//    dump_raw_data(p_data, length);
    return server_send_data(conn_id, bt_config_service_id, 11, p_data,
                            length, GATT_PDU_TYPE_NOTIFICATION);
}

bool bt_config_service_send_ind(uint8_t conn_id, uint8_t *p_data, uint16_t length)
{
//    dump_raw_data(p_data, length);
    return server_send_data(conn_id, bt_config_service_id, 6, p_data,
                            length, GATT_PDU_TYPE_INDICATION);
}


/**
 * @brief read characteristic data from service.
 *
 * @param service_id          ServiceID of characteristic data.
 * @param attrib_index        Attribute index of getting characteristic data.
 * @param offset              Used for Blob Read.
 * @param p_length            length of getting characteristic data.
 * @param pp_value            data got from service.
 * @return Profile procedure result
*/
T_APP_RESULT  bt_config_service_attr_read_cb(uint8_t conn_id, T_SERVER_ID service_id,
                                            uint16_t attrib_index, uint16_t offset, uint16_t *p_length, uint8_t **pp_value)
{
    T_APP_RESULT  cause  = APP_RESULT_SUCCESS;
    app_log("bt_config_service_attr_read_cb");

    switch (attrib_index)
    {
    default:
        app_log("bt_config_service_attr_read_cb, Attr not found, index %d", attrib_index);
        cause = APP_RESULT_ATTR_NOT_FOUND;
        break;
    }

    return (cause);
}

/**
 * @brief write characteristic data from service.
 *
 * @param conn_id
 * @param service_id        ServiceID to be written.
 * @param attrib_index      Attribute index of characteristic.
 * @param length            length of value to be written.
 * @param p_value           value to be written.
 * @return Profile procedure result
*/
T_APP_RESULT bt_config_service_attr_write_cb(uint8_t conn_id, T_SERVER_ID service_id,
                                            uint16_t attrib_index, T_WRITE_TYPE write_type, uint16_t length, uint8_t *p_value,
                                            P_FUN_WRITE_IND_POST_PROC *p_write_ind_post_proc)
{
	TBTCONFIG_CALLBACK_DATA callback_data;
	T_APP_RESULT  cause = APP_RESULT_SUCCESS;
//	app_log("bt_config_service_attr_write_cb write_type = 0x%x, index = %d", write_type, attrib_index);
	*p_write_ind_post_proc = NULL;

	if ( attrib_index == BT_CONFIG_WC_RW_INDEX )
    {
        if ( p_value == NULL )
        {
            cause = APP_RESULT_INVALID_VALUE_SIZE;
        } else
        {
            bt_init_info->wc.on_write(p_value, length);
        }
    }else if(attrib_index == BT_CONFIG_WWNRC_RW_INDEX)
    {
        if ( p_value == NULL )
            {
                cause = APP_RESULT_INVALID_VALUE_SIZE;
            } else
            {
                bt_init_info->wwnrc.on_write(p_value, length);
            }
    }

	return cause;
}

void bt_config_cccd_update_cb(uint8_t conn_id, T_SERVER_ID service_id,uint16_t attrib_index, uint16_t ccc_bits)
{
//    app_log( "bt_config_cccd_update_cb: index = %d, cccbits 0x%x", attrib_index, ccc_bits );
    switch ( attrib_index )
    {
        case BT_CONFIG_IC_CCCD_INDEX:
            bt_init_info->ic.on_ccc_change(ccc_bits);
            break;
        case BT_CONFIG_NC_CCCD_INDEX:
            bt_init_info->nc.on_ccc_change(ccc_bits);
            break;
        default:
            break;
    }
}

/**
 * @brief Simple ble Service Callbacks.
*/
const T_FUN_GATT_SERVICE_CBS bt_config_service_cbs =
{
    bt_config_service_attr_read_cb,  // Read callback function pointer
    bt_config_service_attr_write_cb, // Write callback function pointer
    bt_config_cccd_update_cb // CCCD update callback function pointer
};

/**
  * @brief Add simple BLE service to the BLE stack database.
  *
  * @param[in]   p_func  Callback when service attribute was read, write or cccd update.
  * @return Service id generated by the BLE stack: @ref T_SERVER_ID.
  * @retval 0xFF Operation failure.
  * @retval others Service id assigned by stack.
  *
  */
T_SERVER_ID bt_config_service_add_service(void *p_func)
{
    if (false == server_add_service(&bt_config_service_id,
                                    (uint8_t *)bt_config_service_tbl,
                                    sizeof(bt_config_service_tbl),
                                    bt_config_service_cbs))
    {
   // 	 DBG_8195A("bt_config_service_add_service: fail\n\r");
        app_log("bt_config_service_add_service: fail");
        bt_config_service_id = 0xff;
        return bt_config_service_id;
    }

    pfn_bt_config_service_cb = (P_FUN_SERVER_GENERAL_CB)p_func;
    return bt_config_service_id;
}
