NAME := alisdk

# Libraries
$(NAME)_PREBUILT_LIBRARY := \
prebuild/lib/board_mx1270.a \
prebuild/lib/asr5501.a \
prebuild/lib/vcall.a \
prebuild/lib/kernel_init.a \
prebuild/lib/auto_component.a \
prebuild/lib/libiot_sdk.a \
prebuild/lib/iotx-hal.a \
prebuild/lib/netmgr.a \
prebuild/lib/ota.a \
prebuild/lib/cli.a \
prebuild/lib/ota_hal.a \
prebuild/lib/arch_armv7m.a \
prebuild/lib/newlib_stub.a \
prebuild/lib/rhino.a \
prebuild/lib/digest_algorithm.a \
prebuild/lib/net.a \
prebuild/lib/log.a \
prebuild/lib/activation.a \
prebuild/lib/chip_code.a \
prebuild/lib/imbedtls.a \
prebuild/lib/kv.a \
prebuild/lib/yloop.a \
prebuild/lib/hal.a \
prebuild/lib/ota_transport.a \
prebuild/lib/ota_download.a \
prebuild/lib/ota_verify.a \
prebuild/lib/alicrypto.a \
prebuild/lib/vfs.a \
prebuild/lib/vfs_device.a \
prebuild/lib/awss_security.a \
prebuild/lib/libaiotss.a \
prebuild/lib/libasr_wifi.a \
prebuild/lib/mxos.a

GLOBAL_INCLUDES := \
Service/version \
Service/timer_service \
prebuild/include \
prebuild/include/exports \
prebuild/include/imports \
prebuild/include/kernel/vfs/include \
prebuild/include/kernel/rhino/core/include \
prebuild/include/framework/netmgr/include \
prebuild/include/platform/arch/arm/armv7m/gcc/m4 \
prebuild/include/board/mx1270

GLOBAL_DEFINES := \
ALCS_ENABLED \
WITH_LWIP \
CONFIG_AOS_CLI \
LOG_SIMPLE \
CONFIG_SDK_THREAD_COST=1 \
CONFIG_YWSS \
BUILD_AOS \
AWSS_SUPPORT_STATIS \
DEV_ERRCODE_ENABLE \
DEV_OFFLINE_OTA_ENABLE \
REGION_SINGAPORE \
ONLINE \
AOS_TIMER_SERVICE \
ENABLE_COUNTDOWN_LIST \
ENABLE_LOCALTIMER \
GIT_BRANCH=\"rel_1.3.0\" \
GIT_HASH=\"e3fe74097d268f9c4addaf2b95754e6ce2a12399\" \
COMPILE_HOST=\"parallels\" \
REGION_ENV_STRING=\""SINGAPORE-ONLINE"\" \
APP_NAME=\"smart_outlet\" \
SYSINFO_APP_VERSION=\"app-1.3.0-20200214.130134\" \
CONFIG_BLDTIME_MUTE_DBGLOG=1 \
MQTT_COMM_ENABLED \
ALCS_ENABLED \
DEVICE_MODEL_ENABLED \
DEV_BIND_ENABLED \
WIFI_PROVISION_ENABLED \
AWSS_SUPPORT_SMARTCONFIG \
AWSS_SUPPORT_ZEROCONFIG \
AWSS_SUPPORT_SMARTCONFIG \
AWSS_SUPPORT_ZEROCONFIG \
AWSS_SUPPORT_DEV_AP \
OTA_ENABLED \
MQTT_AUTO_SUBSCRIBE \
MQTT_PREAUTH_SUPPORT_HTTPS_CDN \
MX1270 \
SYSINFO_PRODUCT_MODEL=\"ALI_AOS_LEGAWIFI\" \
SYSINFO_DEVICE_NAME=\"5501A0V240A\" 