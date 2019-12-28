#ifndef __MBT_H__
#define __MBT_H__

typedef void (*P_FUN_BT_ON_CB)(void);
typedef void (*P_FUN_BT_OFF_CB)(void);

typedef struct
{
    P_FUN_BT_ON_CB        bt_on_cb;
    P_FUN_BT_OFF_CB       bt_off_cb;
} MBT_STATE_CBS;

/**@brief   Register callbacks for bt on/off event
 * @param   cbs     Point to the callback functions
 */
void mbt_callback_init( MBT_STATE_CBS *cbs );

/**@brief   Initialize bt and stack
 * @param   link_num  Max link number
 * @retval  kNoErr  Success
 * @retval  others  Failure
 */
merr_t mbt_init( uint8_t link_num );

/**@brief   Deinitialize bt and stack
 * @retval  kNoErr  Success
 * @retval  others  Failure
 */
merr_t mbt_deinit( void );

/**@brief   Start ble advertisement
 * @retval  kNoErr  Success
 * @retval  others  Failure
 */
merr_t mbt_le_adv_start( void );

/**@brief   Stop ble advertisement
 * @retval  kNoErr  Success
 * @retval  others  Failure
 */
merr_t mbt_le_adv_stop( void );

/**@brief   Create a connection to a connectable advertiser
 * @param[in] remote_bd The Peer's Public Device Address, Random (static) Device Address, Non-Resolvable Private Address, or
                        Resolvable Private Address depending on the Peer_Address_Type parameter.
 * @param[in] remote_bd_type The type of address used in the connectable advertisement sent by the peer.
 * @param[in] scan_timeout  Scan timeout value.
 * @retval  kNoErr  Success
 * @retval  others  Failure
 */
merr_t mbt_le_conn( uint8_t *addr, T_GAP_REMOTE_ADDR_TYPE type, uint16_t scan_timeout );

/**@brief   Terminates the existing connection.
 * @param   conn_id  connection ID to be disconnected.
 * @retval  kNoErr  Success
 * @retval  others  Failure
 */
merr_t mbt_le_disconnect( uint8_t conn_id );

/**@brief   Start a device discovery scan.
 * @retval  kNoErr  Success
 * @retval  others  Failure
 */
merr_t mbt_scan_start( void );

/**@brief   Stop a device discovery scan.
 * @retval  kNoErr  Success
 * @retval  others  Failure
 */
merr_t mbt_scan_stop( void );

#endif
