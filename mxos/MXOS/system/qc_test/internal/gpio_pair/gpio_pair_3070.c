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


#include "qc_test_internal.h"
#include "platform.h"

const qc_test_gpio_pair_t qc_test_gpio_pairs[] =
{
    {MODULE_PIN_3,  MODULE_PIN_4},
    {MODULE_PIN_5,  MODULE_PIN_6},
    {MODULE_PIN_7,  MODULE_PIN_8},
    {MODULE_PIN_9,  MODULE_PIN_10},
    {MODULE_PIN_12,  MODULE_PIN_13},
    {MODULE_PIN_13,  MODULE_PIN_14},
    {MODULE_PIN_19,  MODULE_PIN_20},
    {MODULE_PIN_21,  MODULE_PIN_22},
    {MODULE_PIN_22,  MODULE_PIN_23},
};

const int qc_test_gpio_pairs_num = sizeof(qc_test_gpio_pairs) / sizeof(qc_test_gpio_pair_t);
