/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include "mxos.h"
#include <breeze_hal_os.h>

int _os_timer_new(os_timer_t *timer, os_timer_cb_t cb, void *arg, int ms)
{
    mos_timer_id_t mtimer;
    mtimer = mos_timer_new(ms, (mos_timer_handler_t)cb, 0, arg);
    timer->hdl = mtimer;
    return 0;
}

int _os_timer_start(os_timer_t *timer)
{
    mos_timer_start((mos_timer_id_t)timer->hdl);
    return 0;
}

int _os_timer_stop(os_timer_t *timer)
{
    mos_timer_stop((mos_timer_id_t)timer->hdl);
    return 0;
}

void _os_timer_free(os_timer_t *timer)
{
    mos_timer_delete(timer->hdl);
}

void os_reboot()
{
    //TODO
}

void os_msleep(int ms)
{
    mos_msleep(ms);
}

long long os_now_ms()
{
    return mos_time();
}

int os_kv_set(const char *key, const void *value, int len, int sync)
{
    return 0;
}

int os_kv_get(const char *key, void *buffer, int *buffer_len)
{
    return 0;
}

int os_kv_del(const char *key)
{
    return 0;
}

int os_rand(void)
{
    return 0;
}
