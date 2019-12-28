/**
 ******************************************************************************
 * @file    Debug.h
 * @author  William Xu
 * @version V1.0.0
 * @date    05-May-2014
 * @brief   This header contains defines, macros, and functions to aid in
 *          debugging the MICO project.
 ******************************************************************************
 *
 *  The MIT License
 *  Copyright (c) 2014 MXCHIP Inc.
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is furnished
 *  to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 *  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR
 *  IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 ******************************************************************************
 */

#ifndef __mico_Debug_h__
#define __mico_Debug_h__

#include "mdebug.h"
#include "cli/mcli.h"

struct cli_command {
	/** The name of the CLI command */
	const char *name;
	/** The help text associated with the command */
	const char *help;
	/** The function that should be invoked for this command. */
	void (*function) (char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv);
};

typedef struct
{
    int num_of_chunks; /**< number of free chunks*/
    int total_memory; /**< maximum total allocated space*/
    int allocted_memory; /**< total allocated space*/
    int free_memory; /**< total free space*/
} micoMemInfo_t;

#define MicoGetMemoryInfo           mico_memory_info

/**
 * @brief  Get memory usage information
 *
 * @param  None
 *
 * @return Point to structure of memory usage information in heap
 */
micoMemInfo_t* MicoGetMemoryInfo( void );

#define cmd_printf mcli_printf

#endif // __Debug_h__

