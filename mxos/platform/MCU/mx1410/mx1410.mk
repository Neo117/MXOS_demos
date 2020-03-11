NAME := MX1410

# Host architecture is Cortex-M4
HOST_ARCH := Cortex-M33F

$(NAME)_PREBUILT_LIBRARY := \
image/objs.a \
image/lib_wlan.a \
image/lib_cmsis_dsp.a \
image/btgap.a

DEFAULT_LINK_SCRIPT := rlx8721d_img2_ns.ld
$(NAME)_SOURCES := ../../../MXOS/net/LwIP/mxos/mxos_network.c ../../../MXOS/RTOS/mxos_rtos_common.c \
				   appstart.c ../../GCC/time_newlib.c 
				   
$(NAME)_INCLUDES := ../../../MXOS/RTOS/FreeRTOS/mxos

GLOBAL_CFLAGS	+= -ffunction-sections -march=armv8-m.main+dsp -mthumb -mcmse -mfloat-abi=hard -mfpu=fpv5-sp-d16 -g -gdwarf-3 -nostartfiles -nodefaultlibs -nostdlib -O2 -D__FPU_PRESENT -gdwarf-3 -fstack-usage -fdata-sections -nostartfiles -nostdlib -Wall -Wpointer-arith -Wno-write-strings -Wno-maybe-uninitialized -Wextra -Wno-implicit-fallthrough
GLOBAL_CXXFLAGS	+= $(CPU_CFLAGS)
GLOBAL_ASMFLAGS	+= $(CPU_CFLAGS)
GLOBAL_LDFLAGS	+= -march=armv8-m.main+dsp -mthumb -mcmse -mfloat-abi=hard -mfpu=fpv5-sp-d16 -nostartfiles -specs nosys.specs -Wl,--gc-sections -Wl,--warn-section-align -Wl,--build-id=none -Wl,--no-enum-size-warning -Wl,--warn-common -lm -lstdc++
GLOBAL_LDFLAGS	+= -L mxos/platform/MCU/mx1410

GLOBAL_INCLUDES := . ../include ../../ ../../../MXOS/RTOS/

BTDIR = sdk/component/common/bluetooth/realtek
# board-specific
GLOBAL_INCLUDES += \
   $(BTDIR)/sdk \
   $(BTDIR)/sdk/inc \
   $(BTDIR)/sdk/inc/app \
   $(BTDIR)/sdk/inc/bluetooth/gap \
   $(BTDIR)/sdk/inc/bluetooth/gap/gap_lib \
   $(BTDIR)/sdk/inc/os \
   $(BTDIR)/sdk/inc/platform \
   $(BTDIR)/sdk/inc/bluetooth/profile \
   $(BTDIR)/sdk/inc/bluetooth/profile/client \
   $(BTDIR)/sdk/inc/bluetooth/profile/server \
   $(BTDIR)/sdk/inc/stack \
   $(BTDIR)/sdk/board/amebad/src \
   $(BTDIR)/sdk/board/amebad/src/hci \
   $(BTDIR)/sdk/board/amebad/src/vendor_cmd \
   $(BTDIR)/sdk/board/amebad/lib \
   $(BTDIR)/sdk/board/common/inc 

                   
EXTRA_TARGET_MAKEFILES +=  \
mxos/platform/MCU/mx1410/build_helper.mk \
mxos/platform/MCU/mx1410/flash_alg.mk

GLOBAL_DEFINES += FreeRTOS_VERSION=\"V8.1.2\"

GLOBAL_LDFLAGS  += \
-Wl,-wrap,_malloc_r \
-Wl,-wrap,_free_r \
-Wl,-wrap,_realloc_r \
-Wl,-wrap,_calloc_r
