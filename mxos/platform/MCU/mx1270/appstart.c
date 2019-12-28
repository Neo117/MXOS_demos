#include <stdint.h>

#include "module.h"
#include "mxkit.h"
#include "mxos_opt.h"
#include "mhal/gpio.h"
#include "mhal/uart.h"
#include "mhal/flash.h"
#include "mos.h"
#include "mos_worker.h"
#include "qc_test.h"

#define REG_RD(addr) (*(volatile uint32_t *)(addr))
#define REG_WR(addr, value) (*(volatile uint32_t *)(addr)) = (value)

#define ALWAYS_ON_REGFILE 0x40000A00
#define RETENTION_RAM_ADDR 0x40008000
#define POWERON_FLAG_RETENTION_RAM_ADDR (RETENTION_RAM_ADDR + 0x10)
#define AON_RST_CHECK_REG (ALWAYS_ON_REGFILE + 0x10)
#define AON_RST_CHECK_FLAG 0x00000002
#define LEGA_AON_POWER_ON_FLAG 0x12345678

#define PWR_ON_RST 0
#define HARDWARE_PIN_RST 1
#define SOFTWARE_RST 2 //including NVIC_SystemReset and WDG RST

#define STAT_PIN 11
#define BOOT_PIN 12
#define EASL_PIN 13

int mxos_debug_enabled;
mos_mutex_id_t stdio_tx_mutex;

int32_t vfs_init(void);
void ulog_init(const char host_name[8]);
void vfs_device_init(void);
void aos_loop_init(void);
void aos_loop_run(void);
int main(int argc, const char *argv[]);

uint8_t mxos_get_last_reset_reason(void)
{
    uint32_t flag1 = REG_RD(POWERON_FLAG_RETENTION_RAM_ADDR);
    uint32_t flag2 = REG_RD(AON_RST_CHECK_REG);
    if ((LEGA_AON_POWER_ON_FLAG == flag1) && (AON_RST_CHECK_FLAG == (flag2 & AON_RST_CHECK_FLAG)))
    {
        return SOFTWARE_RST;
    }
    else if ((LEGA_AON_POWER_ON_FLAG == flag1) && (0 == flag2))
    {
        REG_WR(AON_RST_CHECK_REG, AON_RST_CHECK_FLAG);
        return HARDWARE_PIN_RST;
    }
    else
    {
        REG_WR(AON_RST_CHECK_REG, AON_RST_CHECK_FLAG);
        REG_WR(POWERON_FLAG_RETENTION_RAM_ADDR, LEGA_AON_POWER_ON_FLAG);
        return PWR_ON_RST;
    }
}

static void usrapp_thread(void *arg)
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

    mhal_gpio_open(BOOT_PIN, INPUT_PULL_UP);
    if (mhal_gpio_value(BOOT_PIN) == false)
    {
        mhal_gpio_open(STAT_PIN, INPUT_PULL_UP);
        if (mhal_gpio_value(STAT_PIN) == false)
        {
            mxos_system_qc_test();
            mos_thread_delete(NULL);
        }
    }

    mos_worker_thread_new(MOS_WORKER_THREAD, MOS_APPLICATION_PRIORITY, 2048, 16);

    main(0, NULL);

    mos_thread_delete(NULL);
}

void mxos_app_init(void)
{
    vfs_init();
    ulog_init("A");
    vfs_device_init();
    aos_loop_init();

    mos_thread_new(32, "user app thread", usrapp_thread, MXOS_DEFAULT_APPLICATION_STACK_SIZE, NULL);

    aos_loop_run();
}

char *mxos_get_bootloader_ver(void)
{
    return "bootloader";
}

typedef struct
{
    uint32_t dst_adr;
    uint32_t src_adr;
    uint32_t siz;
    uint16_t crc;
} ota_hdr_t;

typedef struct {
    unsigned short crc;
} ota_crc16_ctx;

/*CRC16*/
static unsigned short update_crc16(unsigned short crcIn, unsigned char byte)
{
    unsigned int crc = crcIn;
    unsigned int in = byte | 0x100;

    do {
        crc <<= 1;
        in <<= 1;
        if (in & 0x100) {
            ++crc;
        }
        if (crc & 0x10000) {
            crc ^= 0x1021;
        }
    } while (!(in & 0x10000));
    return crc & 0xffffu;
}

void ota_crc16_init(ota_crc16_ctx *inCtx)
{
    inCtx->crc = 0;
}

void ota_crc16_update(ota_crc16_ctx *inCtx, const void *inSrc, size_t inLen)
{
    const unsigned char *src = (const unsigned char *) inSrc;
    const unsigned char *srcEnd = src + inLen;
    while ( src < srcEnd ) {
        inCtx->crc = update_crc16(inCtx->crc, *src++);
    }
}

void ota_crc16_final(ota_crc16_ctx *inCtx, unsigned short *outResult )
{
    inCtx->crc = update_crc16(inCtx->crc, 0);
    inCtx->crc = update_crc16(inCtx->crc, 0);
    *outResult = inCtx->crc & 0xffffu;
}

int ota_image_crc(uint32_t image_size)
{
    uint16_t crcout;
    uint32_t filelen, flashaddr, len = 0, left;
    ota_crc16_ctx ctx = {0};
    uint8_t* tmpbuf;

    tmpbuf = (uint8_t*)malloc(1024);

    filelen = image_size;

    ota_crc16_init(&ctx);
    
    flashaddr = 0;
    left = filelen;

    while (left > 0) {
        if (left > 1024) {
            len = 1024;
        } else {
            len = left;
        }
        left -= len;
        mhal_flash_read(MODULE_PARTITION_OTA_TEMP, &flashaddr, (uint8_t*)tmpbuf, len);
        ota_crc16_update(&ctx, tmpbuf, len);
    }

    ota_crc16_final(&ctx, &crcout);

    return crcout;
}

merr_t mxos_ota_switch(uint32_t ota_len, uint16_t ota_crc)
{
    uint32_t addr = 0;
    
    if(ota_len <= 16)
        return kGeneralErr;
    ota_len -=16;

    ota_crc = ota_image_crc(ota_len);
    
    ota_hdr_t ota_hdr =
        {
            .dst_adr = 0xA000,
            .src_adr = 0x100000,
            .siz = ota_len,
            .crc = ota_crc,
        };


    mhal_flash_erase(MODULE_PARTITION_INFO, addr, sizeof(ota_hdr));

    mhal_flash_write(MODULE_PARTITION_INFO, &addr, (uint8_t *)&ota_hdr, sizeof(ota_hdr));

    return kNoErr;
}