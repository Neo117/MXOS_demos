#include "mxos.h"

#define OTA_HEARD_LEN   (0x1000)

int mxos_debug_enabled;
mos_mutex_id_t stdio_tx_mutex;
static int is_mfg_mode(void);

int main(void);

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

    sys_log_uart_off();

    mhal_uart_open(MXOS_LOG_UART, &config, &pinmux);
    if (is_mfg_mode())
    {
        mxos_system_qc_test();
        mos_thread_delete(NULL);
    }

    mos_worker_thread_new(MOS_WORKER_THREAD, MOS_APPLICATION_PRIORITY, 2048, 16);

    main();
}
static int is_mfg_mode(void)
{
    mhal_gpio_open(MXKIT_ELINK_PIN, INPUT_PULL_UP);

    return (mhal_gpio_value(MXKIT_ELINK_PIN) == 0);
}
char *mxos_get_bootloader_ver(void)
{
    return "unknow";
}

merr_t mxos_ota_switch(int ota_data_len, uint16_t ota_data_crc)
{
    uint8_t cur_id = 0;
    uint32_t ota_offset = 0;

    cur_id = get_cur_fw_idx();

    mhal_flash_erase(MODULE_PARTITION_OTA_HEARD,ota_offset,OTA_HEARD_LEN);

     if(cur_id == 1)
     {
        cur_id = 2;
        mhal_flash_write(MODULE_PARTITION_OTA_HEARD, &ota_offset, &cur_id,sizeof(cur_id));
        
     }else if(cur_id == 2)
     {
        cur_id = 1;
        mhal_flash_write(MODULE_PARTITION_OTA_HEARD, &ota_offset, &cur_id,sizeof(cur_id));
     }

    return kNoErr;

}
