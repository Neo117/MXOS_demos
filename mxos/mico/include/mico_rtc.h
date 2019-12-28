#ifndef __MICO_RTC__H_
#define __MICO_RTC__H_

/**
 * RTC time
 */
typedef struct
{
    uint8_t sec;
    uint8_t min;
    uint8_t hr;
    uint8_t weekday;/* 1-sunday... 7-saturday */
    uint8_t date;
    uint8_t month;
    uint8_t year;
} mico_rtc_time_t;

/**@brief This function will initialize the on board CPU real time clock
 *
 * @note  This function should be called by MICO system when initializing clocks, so
 *        It is not needed to be called by application
 *
 * @return    kNoErr        : on success.
 * @return    kGeneralErr   : if an error occurred with any step
 */
void MicoRtcInitialize(void);

/**@brief This function will return the value of time read from the on board CPU real time clock. Time value must be given in the format of
 * the structure mico_rtc_time_t
 *
 * @param time        : pointer to a time structure
 *
 * @return    kNoErr        : on success.
 * @return    kGeneralErr   : if an error occurred with any step
 */
OSStatus MicoRtcGetTime(mico_rtc_time_t* time);

/**@brief This function will set MCU RTC time to a new value. Time value must be given in the format of
 * the structure mico_rtc_time_t
 *
 * @param time        : pointer to a time structure
 *
 * @return    kNoErr        : on success.
 * @return    kGeneralErr   : if an error occurred with any step
 */
OSStatus MicoRtcSetTime(mico_rtc_time_t* time);

#endif
