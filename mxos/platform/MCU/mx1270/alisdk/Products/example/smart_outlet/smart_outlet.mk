NAME := smart_outlet

$(NAME)_SOURCES := \
app_entry.c \
vendor.c \
smart_outlet_main.c \
device_state_manger.c \
factory.c \
../../../Service/timer_service/ccronexpr.c \
../../../Service/timer_service/timer_service.c \
../../../Service/version/main.c \
../../../Service/version/version.c

$(NAME)_PREBUILT_LIBRARY := \
../../../prebuild/lib/cjson.a

GLOBAL_DEFINES += \
RUN_ALIOS_APP=1