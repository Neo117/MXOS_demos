
NAME := Lib_amebad_bt

$(NAME)_SOURCES += bt.c \
				   profile/src/client/ancs_client.c \
				   profile/src/client/bas_client.c \
				   profile/src/client/gaps_client.c \
				   profile/src/client/gcs_client.c \
				   profile/src/client/simple_ble_client.c \
				   profile/src/client/hids_client.c \
				   profile/src/server/dis.c \
				   profile/src/server/hids.c \
				   profile/src/server/simple_ble_service.c 
					#    profile/src/server/bas.c \
					#    profile/src/server/hids_kb.c \
					#    profile/src/server/hids_rmc.c \

GLOBAL_INCLUDES  := ./ ./gap ./profile/inc ./stack ./profile/inc/client ./profile/inc/server

