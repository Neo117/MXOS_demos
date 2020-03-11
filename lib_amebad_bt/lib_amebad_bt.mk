
NAME := Lib_amebad_bt

$(NAME)_SOURCES += bt.c 

ifneq ($(LIB_AMEBABT_DISABLE_SIMPLE_PROFILE),1)
$(NAME)_SOURCES += profile/src/client/ancs_client.c \
				   profile/src/client/bas_client.c \
				   profile/src/client/gaps_client.c \
				   profile/src/client/gcs_client.c \
				   profile/src/client/simple_ble_client.c \
				   profile/src/client/hids_client.c \
				   profile/src/server/dis.c \
				   profile/src/server/hids.c \
				   profile/src/server/simple_ble_service.c 
endif

GLOBAL_INCLUDES  += ./ \
					./gap \
					./profile/inc \
					./stack 
					
ifneq ($(LIB_AMEBABT_DISABLE_SIMPLE_PROFILE),1)
GLOBAL_INCLUDES  += ./profile/inc/client \
					./profile/inc/server
endif