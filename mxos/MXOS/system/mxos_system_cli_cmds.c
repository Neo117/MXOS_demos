/* ******************************
 * Author : Snow Yang
 * Date   : 2018-12-04
 * Mail   : yangsw@mxchip.com
 * ******************************/

#include "mxos.h"
#include "tftp_ota/tftp.h"

static void task_Command(int argc, char **argv);

void wifistate_Command(int argc, char *argv[]);
void wifidebug_Command(int argc, char *argv[]);
void wifiscan_Command(int argc, char *argv[]);
void ifconfig_Command(int argc, char *argv[]);
void arp_Command(int argc, char *argv[]);
void ping_Command(int argc, char *argv[]);
void dns_Command(int argc, char *argv[]);
void socket_show_Command(int argc, char *argv[]);
void memory_show_Command(int argc, char *argv[]);
void memory_dump_Command(int argc, char *argv[]);
void memory_set_Command(int argc, char *argv[]);
void memp_dump_Command(int argc, char *argv[]);
void driver_state_Command(int argc, char *argv[]);
void tftp_ota(void);
void handle_kv_cmd(int argc, char **argv);
merr_t mxos_rtos_print_thread_status(void);

const char *snowyang =
    "                                              ,---------,\r\n"
    "                                            ,\"        ,\"|\r\n"
    "       ,-------------------------+        ,\"        ,\"  |\r\n"
    "      +-----------------------+  |      ,\"        ,\"    |\r\n"
    "      |  .-----------------.  |  |     +---------+      |\r\n"
    "      |  |                 |  |  |     | -==----'|      |\r\n"
    "      |  |  I LOVE MXOS!   |  |  |     |         |      |\r\n"
    "      |  |                 |  |  |/----|`---=    |      |\r\n"
    "      |  |  $ snowyang     |  |  |   ,/|==== ooo |      ;\r\n"
    "      |  |                 |  |  |  // |(((( [33]|    ,\"\r\n"
    "      |  `-----------------'  |,\" .;'| |((((     |  ,\"\r\n"
    "      +-----------------------+  ;;  | |         |,\"\r\n"
    "         /_)______________(_/  //'   | +---------+\r\n"
    "    ___________________________/___  `,\r\n"
    "   /  oooooooooooooooo  .o.  oooo /,   \\,\"-----------\r\n"
    "  / ==ooooooooooooooo==.o.  ooo= //   ,`\\--{)B     ,\"\r\n"
    " /_==__==========__==_ooo__ooo=_/'   /___________,\"\r\n"
    "\r\n";

static char *ssid;
static char *passphrase;

static void connect_ap(void *arg)
{
  system_context_t *sys_ctx;

  if ((sys_ctx = system_context()) != NULL)
  {
    mos_mutex_lock(sys_ctx->flashContentInRam_mutex);
    memcpy(sys_ctx->flashContentInRam.mxos_config.ssid, ssid, maxSsidLen);
    memset(sys_ctx->flashContentInRam.mxos_config.bssid, 0x0, 6);
    memcpy(sys_ctx->flashContentInRam.mxos_config.user_key, passphrase, maxKeyLen);
    sys_ctx->flashContentInRam.mxos_config.user_keyLength = strlen(passphrase);
    memcpy(sys_ctx->flashContentInRam.mxos_config.key, passphrase, maxKeyLen);
    sys_ctx->flashContentInRam.mxos_config.keyLength = strlen(passphrase);
    sys_ctx->flashContentInRam.mxos_config.dhcpEnable = true;
    mos_mutex_unlock(sys_ctx->flashContentInRam_mutex);

    sys_ctx->flashContentInRam.mxos_config.configured = allConfigured;
    mxos_system_context_update(&sys_ctx->flashContentInRam); //Update Flash content
  }

  mwifi_connect(ssid, passphrase, strlen(passphrase), NULL, NULL);

  mos_thread_delete(NULL);
}

static void wifi_connect_command(int argc, char *argv[])
{
  if (argc < 2)
  {
    mcli_printf("Missing argument\r\n");
    return;
  }

  ssid = argv[1];
  passphrase = argc == 2 ? "" : argv[2];

  mos_thread_new(MOS_APPLICATION_PRIORITY, "connect AP", connect_ap, 2048, NULL);
}

static void start_ap(void *arg)
{
  mwifi_softap_start(ssid, passphrase, 6, NULL);

  mos_thread_delete(NULL);
}

static void wifi_start_ap_command(int argc, char *argv[])
{
  if (argc < 2)
  {
    mcli_printf("Missing argument\r\n");
    return;
  }

  ssid = argv[1];
  passphrase = argc == 2 ? "" : argv[2];

  mos_thread_new(MOS_APPLICATION_PRIORITY, "start AP", start_ap, 2048, NULL);
}

static void task_Command(int argc, char **argv)
{
  mxos_rtos_print_thread_status();
}

static void snowyang_command(int argc, char *argv[])
{
  int n = strlen(snowyang);
  for (int i = 0; i < n; i++)
  {
    mcli_putc(snowyang[i]);
  }
}

static void tftp_Command(int argc, char **argv)
{
  tftp_file_info_t cmdinfo;
  int tftpcmd;
  uint32_t ip;
  int parttype;
  mxos_logic_partition_t *partition;

  if (argc != 7)
  {
    goto WRONGCMD;
  }
  if (strcmp(argv[2], "put") == 0)
  {
    tftpcmd = 0;
  }
  else if (strcmp(argv[2], "get") == 0)
  {
    tftpcmd = 1;
  }
  else
  {
    goto WRONGCMD;
  }

  ip = inet_addr(argv[1]);
  parttype = (int)atoi(argv[4]);

  partition = mhal_flash_get_info(parttype);
  if (partition)
  {
    cmdinfo.flashtype = parttype;
  }
  else
  {
    goto WRONGCMD;
  }

  cmdinfo.flashaddr = strtoul(argv[5], NULL, 0);
  strncpy(cmdinfo.filename, argv[3], 32);

  cmdinfo.filelen = strtoul(argv[6], NULL, 0);
  if (tftpcmd == 0)
  { // put
    mcli_printf("tftp put to %s, filenmae %s. from %s flash, address 0x%lx, len %ld\r\n", argv[1], cmdinfo.filename,
                partition->partition_description, cmdinfo.flashaddr, cmdinfo.filelen);
    tsend(&cmdinfo, ip);
  }
  else
  { // get
    mcli_printf("tftp get from %s, filenmae %s. to %s flash, address 0x%lx, len %ld\r\n", argv[1], cmdinfo.filename,
                partition->partition_description, cmdinfo.flashaddr, cmdinfo.filelen);
    tget(&cmdinfo, ip);
  }
  return;

WRONGCMD:
  mcli_printf("Usage: tftp <ip> put <filename> <partition type> <flashaddr> <flashlen>\r\n"
              "       tftp <ip> get <filenmae> <partition type> <flashaddr> <flashlen>\r\n");
}

static void partShow_Command(int argc, char **argv)
{
  int i;
  mxos_logic_partition_t *partition;

  for (i = MODULE_PARTITION_BOOTLOADER; i <= MODULE_PARTITION_MAX; i++)
  {
    partition = mhal_flash_get_info(i);
    if (partition == NULL)
      continue;
    mcli_printf("%4d | %11s |  Dev:%ld  | 0x%08lx | 0x%08lx |\r\n", i,
                partition->partition_description, partition->partition_owner,
                partition->partition_start_addr, partition->partition_length);
  };
}

static void uptime_Command(int argc, char **argv)
{
  mcli_printf("UP time %ldms\r\n", mos_time());
}

void tftp_ota_thread(void *arg)
{
  tftp_ota();
  mos_thread_delete(NULL);
}

static void get_version(int argc, char **argv)
{
  char ver[128];
  int ret;

  mcli_printf("Product module: %s\r\n", MODEL);
  mcli_printf("Hardware version: %s\r\n", HARDWARE_REVISION);
  mcli_printf("Manufacture: %s\r\n", MANUFACTURER);
  mcli_printf("Kernel version: %s\r\n", mxos_system_lib_version());

  mcli_printf("MXOS version: %s\r\n", MXOS_OS_VERSION);
  mcli_printf("Firmware version: %s\r\n", FIRMWARE_REVISION);
  mcli_printf("Application info: %s\r\n", APP_INFO);
  mcli_printf("Bootloader version: %s\r\n", mxos_get_bootloader_ver());

  memset(ver, 0, sizeof(ver));
  ret = mxos_wlan_driver_version(ver, sizeof(ver));
  if (ret == 0)
    mcli_printf("WIFI version: %s\r\n", ver);
  else
    mcli_printf("Can't get WIFI version, return %d\r\n", ret);
}

static void reboot(int argc, char **argv)
{
  mxos_sys_reboot();
}

#ifdef CONFIG_MXOS_AWS
void aws_log_set(int enable);
static void aws_handler(int argc, char **argv)
{
  if (argc == 1)
  {
    mcli_printf("Usage: awsdebug on/off. \r\n");
    return;
  }

  if (!strcasecmp(argv[1], "on"))
  {
    mcli_printf("Enable AWS debug\r\n");
    aws_log_set(1);
  }
  else if (!strcasecmp(argv[1], "off"))
  {
    mcli_printf("Disable AWS debug\r\n");
    aws_log_set(0);
  }
}
#endif

static const mcli_cmd_t mxos_system_cli_cmds[] = {
    {"snowyang", "Just try it", snowyang_command},
    {"version", "", get_version},

    // WIFI
    {"scan", "scan ap", wifiscan_Command},
    {"connect", "connect <SSID> [passphrase]", wifi_connect_command},
    {"startap", "startap <SSID> [passphrase]", wifi_start_ap_command},
    {"wifistate", "Show wifi state", wifistate_Command},
    {"wifidebug", "wifidebug on/off", wifidebug_Command},
#ifdef CONFIG_MXOS_AWS
    {"awsdebug", "enable aws debug info", aws_handler},
#endif

    // network
    {"ifconfig", "Show IP address", ifconfig_Command},
    {"arp", "arp show/clean", arp_Command},
    {"ping", "ping <ip>", ping_Command},
    {"dns", "show/clean/<domain>", dns_Command},
    {"sockshow", "Show all sockets", socket_show_Command},
    // os
    {"tasklist", "list all thread name status", task_Command},

    // others
    {"memshow", "print memory information", memory_show_Command},
    {"memdump", "<addr> <length>", memory_dump_Command},
    {"memset", "<addr> <value 1> [<value 2> ... <value n>]", memory_set_Command},
    {"memp", "print memp list", memp_dump_Command},
    {"wifidriver", "show wifi driver status", driver_state_Command}, // bus credite, flow control...
    {"reboot", "reboot MXOS system", reboot},
    {"tftp", "tftp", tftp_Command},
    {"time", "system time", uptime_Command},
    {"flash", "Flash memory map", partShow_Command},
    {"kv", "kv [set key value | get key | del key | list]", handle_kv_cmd},
};

void mxos_system_cli_cmds_add(void)
{
  mcli_cmds_add((mcli_cmd_t *)mxos_system_cli_cmds, sizeof(mxos_system_cli_cmds) / sizeof(mcli_cmd_t));
}