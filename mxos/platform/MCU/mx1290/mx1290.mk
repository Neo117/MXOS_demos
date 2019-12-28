#
#  UNPUBLISHED PROPRIETARY SOURCE CODE
#  Copyright (c) 2016 MXCHIP Inc.
#
#  The contents of this file may not be disclosed to third parties, copied or
#  duplicated in any form, in whole or in part, without the prior written
#  permission of MXCHIP Corporation.
#


NAME = platform_mx1290

# Host architecture is ARM Cortex M4
HOST_ARCH := Cortex-M4F

# Host MCU alias for OpenOCD
HOST_OPENOCD := mx1290

GLOBAL_INCLUDES += \
. \
.. \
../include \
../.. \
../../include \
../../$(TOOLCHAIN_NAME) \
../../$(HOST_ARCH) \
../../$(HOST_ARCH)/CMSIS \
peripherals

# Same with vendor
GLOBAL_CFLAGS   += -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -g2 -O2 -Wno-pointer-sign -fno-common -fmessage-length=0  -ffunction-sections -fdata-sections -fomit-frame-pointer -fno-short-enums -std=gnu99 -fsigned-char
GLOBAL_CXXFLAGS += -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -g2 -O2 -Wno-pointer-sign -fno-common -fmessage-length=0  -ffunction-sections -fdata-sections -fomit-frame-pointer -fno-short-enums -std=gnu99 -fsigned-char
GLOBAL_ASMFLAGS += -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -g2 -O2 -Wno-pointer-sign -fno-common -fmessage-length=0  -ffunction-sections -fdata-sections -fomit-frame-pointer -fno-short-enums -std=gnu99 -fsigned-char
GLOBAL_LDFLAGS  += -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -g --specs=nano.specs -nostartfiles -Os -Wl,--gc-sections -Wl,--cref -Wl,--entry=Reset_Handler -Wl,--no-enum-size-warning -Wl,--no-wchar-size-warning -Wl,-wrap,malloc -Wl,-wrap,free -Wl,-wrap,realloc

# defined by mxchip
GLOBAL_LDFLAGS  += -L mxos/platform/MCU/mx1290/linkscripts
GLOBAL_LDFLAGS 	+= -specs=nosys.specs -u _printf_float

# Source files
$(NAME)_SOURCES := \
appstart.c \
libc_stub.c \
../../../MXOS/RTOS/mxos_rtos_common.c \
../../../MXOS/net/LwIP/mxos/mxos_network.c

$(NAME)_INCLUDES := \
../../../MXOS/RTOS/FreeRTOS/mxos

# Libraries
$(NAME)_PREBUILT_LIBRARY := \
libraries/lib_mxos.a \
libraries/lib_platform.a \
libraries/lib_wlan.a \
libraries/lib_wps.a \
libraries/lib_p2p.a \
libraries/lib_rtlstd.a

ifneq ($(MXOS_DISABLE_WOLFSSL), 1)
$(NAME)_COMPONENTS += wolfSSL
endif

DEFAULT_LINK_SCRIPT := linkscripts/rlx8711B-symbol-v02-img2_xip1.ld

EXTRA_TARGET_MAKEFILES +=  \
mxos/platform/MCU/mx1290/flash_alg.mk \
mxos/platform/MCU/mx1290/tools/image.mk

GLOBAL_DEFINES += CONFIG_MX1290 FreeRTOS_VERSION=\"V8.2.0\"


GLOBAL_LDFLAGS  += \
-Wl,-wrap,_malloc_r \
-Wl,-wrap,_free_r \
-Wl,-wrap,_realloc_r \
-Wl,-wrap,_calloc_r

