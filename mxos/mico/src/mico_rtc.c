#include "mico.h"

void MicoRtcInitialize(void)
{
    mhal_rtc_open();
}

OSStatus MicoRtcGetTime(mico_rtc_time_t* time)
{
    time_t t;
    
    t = mhal_rtc_get();
}

OSStatus MicoRtcSetTime(mico_rtc_time_t* time)
{
    time_t t;
    
    mhal_rtc_set(t);
}

