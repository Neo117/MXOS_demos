NAME := breeze_hal

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := This is an AliOS Things based implementation of Breeze HAP APIs..

$(NAME)_SOURCES := breeze_hal_ble.c breeze_hal_os.c
$(NAME)_SOURCES += peripheral_app.c bt_config_service.c

enhanced_auth ?= 1
ifeq ($(enhanced_auth),1)
$(NAME)_SOURCES += breeze_hal_sec.c
endif
