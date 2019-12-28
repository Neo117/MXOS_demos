NAME := lib_mico_for_mxos

$(NAME)_SOURCES += src/mico_rtos.c src/mico_system.c src/mico_tls.c \
	src/mico_wlan.c src/mico_rtc.c src/mico_flash_partition.c


GLOBAL_DEFINES += _MICO_INCLUDE_
GLOBAL_INCLUDES += ./include

