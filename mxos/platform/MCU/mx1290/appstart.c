#include "mxos.h"

int mxos_debug_enabled;
mos_mutex_id_t stdio_tx_mutex;

int main(void);
static int is_mfg_mode(void);
void mxos_system_qc_test(void);
int is_force_enter_qc_mode(void);

void mxos_app_main(void)
{
    mxos_debug_enabled = 1;
    stdio_tx_mutex = mos_mutex_new();

    mhal_uart_config_t config = {
        .baudrate = MXOS_LOG_UART_BAUDRATE,
        .data_width = DATA_WIDTH_8BIT,
        .parity = NO_PARITY,
        .stop_bits = STOP_BITS_1,
        .flow_control = FLOW_CONTROL_DISABLED,
        .buffersize = 1024,
    };

    mhal_uart_pinmux_t pinmux = {
        .tx = MXOS_LOG_UART_TXD,
        .rx = MXOS_LOG_UART_RXD,
        .rts = MODULE_PIN_NC,
        .cts = MODULE_PIN_NC,
    };

    mhal_uart_open(MXOS_LOG_UART, &config, &pinmux);

    if (is_mfg_mode())
    {
        mxos_system_qc_test();
        mos_thread_delete(NULL);
    }

    mos_worker_thread_new(MOS_WORKER_THREAD, MOS_APPLICATION_PRIORITY, 2048, 16);

    main();
}

char *mxos_get_bootloader_ver(void)
{
    return "unknow";
}

static merr_t write_boot_table(boot_table_t *boot_table)
{
    merr_t err = kNoErr;
    uint32_t offset;
    boot_table_t boot_table_dump;

    mhal_flash_erase(MODULE_PARTITION_INFO, 0x0, sizeof(boot_table_t));
    offset = 0x0;
    mhal_flash_write(MODULE_PARTITION_INFO, &offset, (uint8_t *)boot_table, sizeof(boot_table_t));
    offset = 0x0;
    mhal_flash_read(MODULE_PARTITION_INFO, &offset, (uint8_t *)&boot_table_dump, sizeof(boot_table_t));
    require_action(memcmp(boot_table, &boot_table_dump, sizeof(boot_table_t)) == 0, exit, err = kWriteErr);

exit:
    return err;
}

merr_t mxos_ota_switch(uint32_t ota_len, uint16_t ota_crc)
{
    merr_t err;
    boot_table_t boot_table;

    memset(&boot_table, 0, sizeof(boot_table_t));
    boot_table.length = ota_len;
    boot_table.start_address = mhal_flash_get_info(MODULE_PARTITION_OTA_TEMP)->partition_start_addr;
    boot_table.type = 'A';
    boot_table.upgrade_type = 'U';
    boot_table.crc = ota_crc;
    return write_boot_table(&boot_table);
}

static int is_mfg_mode(void)
{
    return is_force_enter_qc_mode();
}
