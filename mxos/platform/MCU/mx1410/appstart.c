#include "mxos.h"

int mxos_debug_enabled;
mos_mutex_id_t stdio_tx_mutex;

static int is_mfg_mode(void);
int main(void);
void mxos_system_qc_test(void);

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

    mos_worker_thread_new(MOS_WORKER_THREAD, MOS_APPLICATION_PRIORITY, MXOS_WORKER_THREAD_STACK_SIZE, 16);

    main();
}

char *mxos_get_bootloader_ver(void)
{
    return "Bootloader";
}

static int is_mfg_mode(void)
{
    mhal_gpio_open(MXOS_BOOT_PIN, INPUT_PULL_UP);
    mhal_gpio_open(MXOS_STATUS_PIN, INPUT_PULL_UP);

    return mhal_gpio_value(MXOS_BOOT_PIN) == 0 && mhal_gpio_value(MXOS_STATUS_PIN) == 0;
}
