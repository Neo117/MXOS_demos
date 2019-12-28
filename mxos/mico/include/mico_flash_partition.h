#ifndef _MICO_FLASH_PARTITION_H_
#define _MICO_FLASH_PARTITION_H_

#include "mhal/flash.h"
#include "mico_result.h"

typedef enum
{
    MICO_PARTITION_ERROR = -1,
    MICO_PARTITION_BOOTLOADER = 0,
    MICO_PARTITION_APPLICATION,
    MICO_PARTITION_ATE,
    MICO_PARTITION_OTA_TEMP,
    MICO_PARTITION_RF_FIRMWARE,
    MICO_PARTITION_PARAMETER_1,
    MICO_PARTITION_PARAMETER_2,
    MICO_PARTITION_BT_FIRMWARE,
    MICO_PARTITION_MAX,
    MICO_PARTITION_NONE,
} mico_partition_t;

enum
{
  MICO_FLASH_SPI,
  MICO_FLASH_MAX,
  MICO_FLASH_NONE,
} ;

typedef mxos_logic_partition_t  mico_logic_partition_t;

/**@brief   Get the infomation of the specified flash area 
 *
 * @param   inPartition:  The target flash logical partition which should be erased
 *
 * @return  mico_logi_partition struct
 */
mico_logic_partition_t* MicoFlashGetInfo( mico_partition_t inPartition );


/**@brief   Erase an area on a Flash logical partition
 *
 * @note    Erase on an address will erase all data on a sector that the 
 *          address is belonged to, this function does not save data that
 *          beyond the address area but in the affected sector, the data
 *          will be lost.
 *
 * @param  inPartition     : The target flash logical partition which should be erased
 * @param  off_set         : Start address of the erased flash area
 * @param  size    	   : Size of the erased flash area
 *
 * @return  kNoErr        : On success.
 * @return  kGeneralErr   : If an error occurred with any step
 */
OSStatus MicoFlashErase(mico_partition_t inPartition, uint32_t off_set, uint32_t size);

/**@brief  Write data to an area on a Flash logical partition
 *
 * @param  inPartition    : The target flash logical partition which should be read which should be written
 * @param  off_set        : Point to the start address that the data is written to, and
 *                          point to the last unwritten address after this function is 
 *                          returned, so you can call this function serval times without
 *                          update this start address.
 * @param  inBuffer       : point to the data buffer that will be written to flash
 * @param  inBufferLength : The length of the buffer
 *
 * @return  kNoErr        : On success.
 * @return  kGeneralErr   : If an error occurred with any step
 */
OSStatus MicoFlashWrite( mico_partition_t inPartition, volatile uint32_t* off_set, uint8_t* inBuffer ,uint32_t inBufferLength);

/**@brief    Read data from an area on a Flash to data buffer in RAM
 *
 * @param    inPartition    : The target flash logical partition which should be read
 * @param    off_set        : Point to the start address that the data is read, and
 *                          point to the last unread address after this function is 
 *                          returned, so you can call this function serval times without
 *                          update this start address.
 * @param    outBuffer      : Point to the data buffer that stores the data read from flash
 * @param    inBufferLength : The length of the buffer
 *
 * @return    kNoErr        : On success.
 * @return    kGeneralErr   : If an error occurred with any step
 */
OSStatus MicoFlashRead( mico_partition_t inPartition, volatile uint32_t* off_set, uint8_t* outBuffer, uint32_t inBufferLength);


#endif

