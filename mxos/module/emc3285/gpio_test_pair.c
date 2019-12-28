/**
 ******************************************************************************
 * @file    gpio_pair_3070.c
 * @author  William Xu
 * @version V1.0.0
 * @date    18-Dec-2016
 * @brief   This file provide the definition of GPIO pairs used for GPIO QC test.
 ******************************************************************************
 *
 *  UNPUBLISHED PROPRIETARY SOURCE CODE
 *  Copyright (c) 2016 MXCHIP Inc.
 *
 *  The contents of this file may not be disclosed to third parties, copied or
 *  duplicated in any form, in whole or in part, without the prior written
 *  permission of MXCHIP Corporation.
 ******************************************************************************
 */

#include "module.h"
#include "internal/qc_test_internal.h"

const qc_test_gpio_pair_t qc_test_gpio_pairs[] =
{
    {PB_2,   PB_1},
    {PA_12,  PA_13},
    {PA_28,  PA_25},
    {PA_26,  PA_30},
    {PB_20,  PB_21},
    {PB_3,   PA_27},
};

const int qc_test_gpio_pairs_num = sizeof(qc_test_gpio_pairs) / sizeof(qc_test_gpio_pair_t);
