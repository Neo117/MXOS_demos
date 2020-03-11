#
#  UNPUBLISHED PROPRIETARY SOURCE CODE
#  Copyright (c) 2016 MXCHIP Inc.
#
#  The contents of this file may not be disclosed to third parties, copied or
#  duplicated in any form, in whole or in part, without the prior written
#  permission of MXCHIP Corporation.
#

NAME := Lib_jsmn

$(NAME)_SOURCES :=	jsmn.c \
					json_escape_str.c \
					json_generator.c \
					json_wrappers.c \
					json_utils.c

GLOBAL_INCLUDES += 	.

ifeq ($(BUILD_MXOS), 1)
GLOBAL_DEFINES +=	JSMN_USE_MXOS=1
else
GLOBAL_DEFINES +=	JSMN_USE_MICO=1
endif