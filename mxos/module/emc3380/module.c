#include "mxos.h"

enum
{
  PHY_PARTITION_BOOTLOADER = 0, // first 24K
  PHY_PARTITION_APPLICATION1,
  PHY_PARTITION_APPLICATION2,
  PHY_PARTITION_KVRO,
  PHY_PARTITION_BT_FTL,
  PHY_PARTITION_KV,
  PHY_PARTITION_USER,
  PHY_PARTITION_MAX,
  PHY_PARTITION_NONE,
};

/* Logic partition on flash devices */
const mxos_logic_partition_t mxos_partitions[] = {
    [PHY_PARTITION_BOOTLOADER] = {
        .partition_description = "Bootloader",
        .partition_start_addr = 0x00000000,
        .partition_length = 0x6000, //24k bytes
        .partition_options = PAR_OPT_READ_EN | PAR_OPT_WRITE_DIS,
    },
    [PHY_PARTITION_APPLICATION1] = {
        .partition_description = "Application1",
        .partition_start_addr = 0x00006000,
        .partition_length = 0x178000, //1504k bytes
        .partition_options = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
    [PHY_PARTITION_APPLICATION2] = {
        .partition_description = "Application2",
        .partition_start_addr = 0x00188000,
        .partition_length = 0x178000, // 1504k bytes
        .partition_options = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
    [PHY_PARTITION_KVRO] = {
        .partition_description = "KVRO",
        .partition_start_addr = 0x0017E000,
        .partition_length = 0x4000, //16k bytes
        .partition_options = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
    [PHY_PARTITION_BT_FTL] = {
        .partition_description = "BT_FTL",
        .partition_start_addr = 0x00182000,
        .partition_length = 0x3000, //12k bytes
        .partition_options = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },

    [PHY_PARTITION_USER] = {
        .partition_description = "USER",
        .partition_start_addr = 0x00300000,
        .partition_length = 0x100000 - 0x4000, //1024K - 16K bytes
        .partition_options = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
    [PHY_PARTITION_KV] = {
        .partition_description = "KV",
        .partition_start_addr = 0x3FC000,
        .partition_length = 0x4000, //16 bytes
        .partition_options = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN,
    },
    [PHY_PARTITION_NONE] = {
        .partition_description = "NONE",
        .partition_start_addr = 0,
        .partition_length = 0, //1024K bytes
        .partition_options = 0,
    },
};

#define OTA_INDEX_1			0
#define OTA_INDEX_2			1

uint32_t ota_get_cur_index(void);

const mxos_logic_partition_t *paltform_flash_get_info(int inPartition)
{
  switch (inPartition)
  {
  case MODULE_PARTITION_BOOTLOADER:
    return &mxos_partitions[PHY_PARTITION_BOOTLOADER];
  case MODULE_PARTITION_KVRO:
    return &mxos_partitions[PHY_PARTITION_KVRO];
  case MODULE_PARTITION_KV:
    return &mxos_partitions[PHY_PARTITION_KV];    
  case MODULE_PARTITION_USER:
    return &mxos_partitions[PHY_PARTITION_USER];

  case MODULE_PARTITION_APPLICATION:
    if (ota_get_cur_index() == OTA_INDEX_2)
      return &mxos_partitions[PHY_PARTITION_APPLICATION1];
    else
      return &mxos_partitions[PHY_PARTITION_APPLICATION2];
  case MODULE_PARTITION_OTA_TEMP:
    if (ota_get_cur_index() == OTA_INDEX_2)
      return &mxos_partitions[PHY_PARTITION_APPLICATION2];
    else
      return &mxos_partitions[PHY_PARTITION_APPLICATION1];
  default:
    return &mxos_partitions[PHY_PARTITION_NONE];
  }
}

void sys_clear_ota_signature(void);

merr_t mxos_ota_switch(uint32_t ota_len, uint16_t ota_crc)
{
  sys_clear_ota_signature();

  return kNoErr;
}
