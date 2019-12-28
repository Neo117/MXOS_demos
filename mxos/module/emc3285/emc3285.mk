NAME := EMC3285

MODULE := emc3285
HOST_MCU_FAMILY := mx1410

$(NAME)_COMPONENTS += platform/MCU/$(HOST_MCU_FAMILY)
$(NAME)_SOURCES := module.c gpio_test_pair.c
# Global includes
GLOBAL_INCLUDES  := .
GLOBAL_DEFINES := QC_TEST_BLE_SCAN

JTAG := jlink_swd
