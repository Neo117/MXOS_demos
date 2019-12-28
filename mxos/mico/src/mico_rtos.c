#include "mico.h"

OSStatus mico_rtos_create_thread( mico_thread_t* thread, uint8_t priority, const char* name, mico_thread_function_t function, uint32_t stack_size, uint32_t arg )
{
    mos_thread_id_t ret;
    
    ret = mos_thread_new(priority, name, function, stack_size, arg );
    if (ret == NULL)
        return kGeneralErr;

    if (thread) {
        *thread = ret;
    }
    return kNoErr;
}

OSStatus mico_rtos_delete_thread( mico_thread_t* thread )
{
    if ( thread == NULL )
    {
        mos_thread_delete( NULL );
    }
    else 
    {
        mos_thread_delete( *thread );
    }
    return kNoErr;
}

void mico_rtos_thread_yield( void )
{
    mos_thread_yield();
}

OSStatus mico_rtos_thread_join( mico_thread_t* thread )
{
    mico_thread_t tmp = *thread;

    if ( (thread == NULL) || (tmp == NULL) )
        return kNoErr;

    mos_thread_join(tmp);
    
    return kNoErr;
}

bool mico_rtos_is_current_thread( mico_thread_t* thread )
{
    if ( mos_thread_get_id( ) == *thread )
    {
        return true;
    }
    else
    {
        return false;
    }
}

mico_thread_t mico_rtos_get_current_thread( void )
{
    return (mico_thread_t)mos_thread_get_id();
}


OSStatus mico_rtos_check_stack( void )
{
    return kNoErr;
}

OSStatus mico_rtos_thread_force_awake( mico_thread_t* thread )
{
    mos_thread_awake(*thread);

    return kNoErr;
}


OSStatus mico_rtos_init_semaphore( mico_semaphore_t* semaphore, int count )
{
    *semaphore = mos_semphr_new(count);

    return ( *semaphore != NULL ) ? kNoErr : kGeneralErr;
}

OSStatus mico_rtos_get_semaphore( mico_semaphore_t* semaphore, uint32_t timeout_ms )
{
    return mos_semphr_acquire(*semaphore, timeout_ms);
}

int mico_rtos_set_semaphore( mico_semaphore_t* semaphore )
{
    return mos_semphr_release(*semaphore);
}

OSStatus mico_rtos_deinit_semaphore( mico_semaphore_t* semaphore )
{
    if (semaphore != NULL)
    {
        mos_semphr_delete(*semaphore);
        *semaphore = NULL;
    }
    return kNoErr;
}

OSStatus mico_rtos_init_mutex( mico_mutex_t* mutex )
{
    *mutex = mos_mutex_new();
    return ( *mutex != NULL ) ? kNoErr : kGeneralErr;
}

OSStatus mico_rtos_lock_mutex( mico_mutex_t* mutex )
{
    mos_mutex_lock(*mutex);

    return kNoErr;
}


OSStatus mico_rtos_unlock_mutex( mico_mutex_t* mutex )
{
    mos_mutex_unlock(*mutex);

    return kNoErr;
}

OSStatus mico_rtos_deinit_mutex( mico_mutex_t* mutex )
{
    if (mutex == NULL)
        return kNoErr;

    mos_mutex_delete(*mutex);
    *mutex = NULL;
    return kNoErr;
}

OSStatus mico_rtos_init_queue( mico_queue_t* queue, const char* name, uint32_t message_size, uint32_t number_of_messages )
{
    UNUSED_PARAMETER(name);

    *queue = mos_queue_new(message_size, number_of_messages);

    return ( *queue != NULL ) ? kNoErr : kGeneralErr;;
}

OSStatus mico_rtos_push_to_queue( mico_queue_t* queue, void* message, uint32_t timeout_ms )
{
    return mos_queue_push(*queue, message, timeout_ms);
}


OSStatus mico_rtos_push_to_queue_front( mico_queue_t* queue, void* message, uint32_t timeout_ms )
{
    return mos_queue_push_front(queue, message, timeout_ms);
}


OSStatus mico_rtos_pop_from_queue( mico_queue_t* queue, void* message, uint32_t timeout_ms )
{
    return mos_queue_pop(*queue, message, timeout_ms);
}


OSStatus mico_rtos_deinit_queue( mico_queue_t* queue )
{
    mos_queue_delete(*queue);
    *queue = NULL;
    return kNoErr;
}

bool mico_rtos_is_queue_empty( mico_queue_t* queue )
{
    int free, total;
    
    free = mos_queue_get_free(*queue);
    total = mos_queue_get_total(*queue);
    if (free == total)
        return true;
    else
        return false;
}

bool mico_rtos_is_queue_full( mico_queue_t* queue )
{
    if (mos_queue_get_free(*queue) == 0)
        return true;
    else
        return false;
}

OSStatus mico_rtos_init_timer( mico_timer_t* timer, uint32_t time_ms, timer_handler_t function, void* arg )
{
    mos_timer_id_t ret;
    
    ret = mos_timer_new(time_ms, function, true, arg);
    if (ret == NULL)
        return kGeneralErr;
    *timer = ret;
    return kNoErr;
}

int mico_init_once_timer( mico_timer_t* timer, uint32_t time_ms, timer_handler_t function, void* arg )
{
    mos_timer_id_t ret;
    
    ret = mos_timer_new(time_ms, function, false, arg);
    if (ret == NULL)
        return kGeneralErr;
    *timer = ret;
    return kNoErr;
}


OSStatus mico_rtos_start_timer( mico_timer_t* timer )
{
    mos_timer_start(*timer);

    return kNoErr;
}

OSStatus mico_rtos_stop_timer( mico_timer_t* timer )
{
    mos_timer_stop(*timer);

    return kNoErr;
}

OSStatus mico_rtos_change_timer( mico_timer_t* timer, uint32_t ntime )
{
    mos_timer_change(*timer, ntime);
    return kNoErr;
}

OSStatus mico_rtos_reload_timer( mico_timer_t* timer )
{
    mos_timer_reset(*timer);

    return kNoErr;
}

OSStatus mico_rtos_deinit_timer( mico_timer_t* timer )
{
    mos_timer_delete(*timer);

    return kNoErr;
}


bool mico_rtos_is_timer_running( mico_timer_t* timer )
{
    return mos_timer_is_runing(*timer);
}

void mico_rtos_suspend_thread(mico_thread_t* thread)
{
    if (thread == NULL)
        mos_thread_suspend(NULL);
    else
        mos_thread_suspend(*thread);
}

void mico_rtos_resume_thread(mico_thread_t* thread)
{
    if (thread == NULL)
        vTaskResume(NULL);
    else
        vTaskResume(*thread);
}

/**
 * Gets time in milliseconds since RTOS start
 *
 * @Note: since this is only 32 bits, it will roll over every 49 days, 17 hours.
 *
 * @returns Time in milliseconds since RTOS started.
 */
mico_time_t mico_rtos_get_time( void )
{
    return mos_time();
}

/**
 * Delay for a number of milliseconds
 *
 * Processing of this function depends on the minimum sleep
 * time resolution of the RTOS.
 * The current thread sleeps for the longest period possible which
 * is less than the delay required, then makes up the difference
 * with a tight loop
 *
 * @return OSStatus : kNoErr if delay was successful
 *
 */
OSStatus mico_rtos_delay_milliseconds( uint32_t num_ms )
{
    mos_msleep(num_ms);

    return kNoErr;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void *mico_malloc( size_t xWantedSize )
{
	return malloc(xWantedSize);
}

void mico_free( void *pv )
{
	free(pv);
}

void *mico_realloc( void *pv, size_t xWantedSize )
{
	return realloc(pv, xWantedSize);
}


char *mico_current_task_name(void)
{
    return mos_thread_get_name(NULL);
}

void mico_rtos_thread_sleep(uint32_t seconds)
{
    mos_msleep(seconds*1000);
}
void mico_rtos_thread_msleep(uint32_t mseconds)
{
    mos_msleep(mseconds);
}

int mico_create_event_fd(mico_event_t handle)
{
    return mos_event_fd_new(handle);
}
int mico_delete_event_fd(int fd)
{
    return mos_event_fd_delete(fd);
}

//#endif


