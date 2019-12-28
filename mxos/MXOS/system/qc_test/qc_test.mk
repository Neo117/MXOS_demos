#
#  UNPUBLISHED PROPRIETARY SOURCE CODE
#  Copyright (c) 2016 MXCHIP Inc.
#
#  The contents of this file may not be disclosed to third parties, copied or
#  duplicated in any form, in whole or in part, without the prior written
#  permission of MXCHIP Corporation.
#

NAME = Lib_MXOS_System_QC


$(NAME)_SOURCES := qc_test.c \
                   internal/qc_test_tcpip.c \
                   internal/qc_test_wlan.c \
                   internal/qc_test_ble.c \
                   internal/qc_test_cli.c \
                   internal/qc_test_gpio.c 
                   
GLOBAL_INCLUDES := . 
$(NAME)_INCLUDES := internal



