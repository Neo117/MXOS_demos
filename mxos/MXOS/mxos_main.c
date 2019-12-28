/**
 ******************************************************************************
 * @file    mxos_main.c
 * @author  William Xu
 * @version V1.0.0
 * @date    22-Aug-2016
 * @brief   MXOS initialize before main application
 ******************************************************************************
 *
 *  UNPUBLISHED PROPRIETARY SOURCE CODE
 *  Copyright (c) 2017 MXCHIP Inc.
 *
 *  The contents of this file may not be disclosed to third parties, copied or
 *  duplicated in any form, in whole or in part, without the prior written
 *  permission of MXCHIP Corporation.
 *
 ******************************************************************************
 */

/** @file
 *
 */

#include <string.h>
#include <stdlib.h>
#include "mxos.h"
#include "module.h"
#include "mos_worker.h"
#if MXOS_QUALITY_CONTROL_ENABLE
#include "qc_test.h"
#endif

/******************************************************
 *                      Macros
 ******************************************************/

/******************************************************
 *                    Constants
 ******************************************************/

#ifndef STDIO_BUFFER_SIZE
#define STDIO_BUFFER_SIZE   64
#endif

/******************************************************
 *                   Enumerations
 ******************************************************/

/******************************************************
 *                 Type Definitions
 ******************************************************/

/******************************************************
 *                    Structures
 ******************************************************/

/******************************************************
 *               Static Function Declarations
 ******************************************************/

/******************************************************
 *               Variable Definitions
 ******************************************************/


#ifndef MXOS_DISABLE_STDIO
mos_mutex_id_t MXOS_WEAK stdio_tx_mutex = NULL;
#endif /* #ifndef MXOS_DISABLE_STDIO */

extern int mxos_debug_enabled;

/******************************************************
 *               Function Definitions
 ******************************************************/
extern merr_t mxos_rtos_init( void );

/* mxos_main is executed after rtos is start up and before real main*/
void mxos_main( void )
{
#if MXOS_APPLICATION

#ifdef _MXOS_DEBUG_
    mxos_debug_enabled = 1;
#else
    mxos_debug_enabled = 0;
#endif

    /* Customized board configuration. */
    mxos_board_init( );

#ifndef MXOS_DISABLE_STDIO
    if( stdio_tx_mutex == NULL )
        stdio_tx_mutex = mos_mutex_new( );
#endif

    mxos_rtos_init( );

#if MXOS_QUALITY_CONTROL_ENABLE
#ifndef RTOS_mocOS
    if ( mxos_should_enter_mfg_mode( ) ) {
        mxos_system_qc_test( );
        mos_thread_delete(NULL);
        mos_thread_yield();
    }
#endif
#endif

#endif
}

