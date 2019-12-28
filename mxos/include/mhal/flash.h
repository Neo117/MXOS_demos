/**
 ******************************************************************************
 * @file    flash.h
 * @author  snow yang
 * @mail    yangsw@mxchip.com
 * @version v1.0.0
 * @date    2019/06/21
 * @brief   This file provides mxchip FLASH APIs.
 ******************************************************************************
 */

#ifndef __FLASH_H__
#define __FLASH_H__

#include "stdint.h"
#include "merr.h"

/** 
* @addtogroup hal
* @{
*/

/**
* @defgroup flash FLASH
* @brief FLASH driver APIs
* @{
*/

#define PAR_OPT_READ_POS (0)  /**< partition read pos */
#define PAR_OPT_WRITE_POS (1) /**< partition write pos */

#define PAR_OPT_READ_MASK (0x1u << PAR_OPT_READ_POS)   /**< partition read mask */
#define PAR_OPT_WRITE_MASK (0x1u << PAR_OPT_WRITE_POS) /**< partition write mask */

#define PAR_OPT_READ_DIS (0x0u << PAR_OPT_READ_POS)   /**< partition read disable */
#define PAR_OPT_READ_EN (0x1u << PAR_OPT_READ_POS)    /**< partition read enable */
#define PAR_OPT_WRITE_DIS (0x0u << PAR_OPT_WRITE_POS) /**< partition write disable  */
#define PAR_OPT_WRITE_EN (0x1u << PAR_OPT_WRITE_POS)  /**< partition write enable */

/**
 * @brief flash logic_partition_t
 */
typedef struct
{
    int32_t partition_owner;           /**< partition owner */
    const char *partition_description; /**< partition description */
    uint32_t partition_start_addr;     /**< partition start_addr */
    uint32_t partition_length;         /**< partition length */
    uint32_t partition_options;        /**< partition options */
} mxos_logic_partition_t;

/**
 * @brief A name string of a Flash drive
 */
extern const char *flash_name[];

/**
 * @brief   Get flash partition information
 * @param   par     Partition name
 * 
 * @return  Partition structure pointer 
 */
mxos_logic_partition_t *mhal_flash_get_info(int par);

/**@brief   Erase an area of a flash partition
 * @param   par     Partition name
 * @param   addr    Start address of the erased flash area
 * @param   n       Size of the erased flash area
 *
 * @retval  kNoErr  Success
 * @retval  others  Failure
 */
merr_t mhal_flash_erase(int par, uint32_t addr, uint32_t n);

/**@brief   Write data to an area of a flash partition
 * @param   par     Partition name
 * @param   paddr   Point to the start address that the data is written to, and
 *                  point to the last unwritten address after this function is 
 *                  returned, so you can call this function serval times without
 *                  update this start address.
 * @param   buf     Point to the data buffer that will be written to flash
 * @param   n       The length of the buffer
 *
 * @retval  kNoErr  Success
 * @retval  others  Failure
 */
merr_t mhal_flash_write(int par, uint32_t *paddr, uint8_t *buf, uint32_t n);

/**@brief   Read data from an area of a flash partition
 * @param   par     Partition name
 * @param   paddr   Point to the start address that the data is read, and
 *                  point to the last unread address after this function is 
 *                  returned, so you can call this function serval times without
 *                  update this start address.
 * @param   buf     Point to the data buffer that stores the data read from flash
 * @param   n       The length of the buffer
 *
 * @retval  kNoErr  Success
 * @retval  others  Failure
 */
merr_t mhal_flash_read(int par, uint32_t *paddr, uint8_t *buf, uint32_t n);

/** @} */
/** @} */
#endif