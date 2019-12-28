/**
 ******************************************************************************
 * @file    gpio.h
 * @author  snow yang
 * @mail    yangsw@mxchip.com
 * @version v1.0.0
 * @date    2019/06/21
 * @brief   This file provides mxchip GPIO APIs.
 * 
 ******************************************************************************
 */

#ifndef __GPIO_H__
#define __GPIO_H__

#include <stdbool.h>
#include "stdint.h"
#include "merr.h"

/** 
* @addtogroup hal
* @{
*/

/**
* @defgroup gpio GPIO
* @brief GPIO driver APIs
* @{
*/

/**
 * @brief Pin mode
 */
typedef enum
{
    INPUT_PULL_UP,             /**< Input with an internal pull-up resistor - use with devices that actively drive the signal low - e.g. button connected to ground */
    INPUT_PULL_DOWN,           /**< Input with an internal pull-down resistor - use with devices that actively drive the signal high - e.g. button connected to a power rail */
    INPUT_HIGH_IMPEDANCE,      /**< Input - must always be driven, either actively or by an external pullup resistor */
    OUTPUT_PUSH_PULL,          /**< Output actively driven high and actively driven low - must not be connected to other active outputs - e.g. LED output */
    OUTPUT_OPEN_DRAIN_NO_PULL, /**< Output actively driven low but is high-impedance when set high - can be connected to other open-drain/open-collector outputs. Needs an external pull-up resistor */
    OUTPUT_OPEN_DRAIN_PULL_UP, /**< Output actively driven low and is pulled high with an internal resistor when set high - can be connected to other open-drain/open-collector outputs. */
} mhal_gpio_mode_t;

/**
 * @brief GPIO interrupt mode
 */
typedef enum
{
    IRQ_TRIGGER_RISING_EDGE = 0x1,                                               /**< Interrupt modeed at input signal's rising edge */
    IRQ_TRIGGER_FALLING_EDGE = 0x2,                                              /**< Interrupt modeed at input signal's falling edge */
    IRQ_TRIGGER_BOTH_EDGES = IRQ_TRIGGER_RISING_EDGE | IRQ_TRIGGER_FALLING_EDGE, /**< Interrupt modeed at input signal's falling edge or signal's rising edge*/
} mhal_gpio_irq_mode_t;

/**
 * @brief GPIO interrupt callback handler
 */
typedef void (*mhal_gpio_irq_handler_t)(void *arg);

/**
 * @brief   Open an GPIO device
 * 
 * @param   gpio    GPIO name
 * @param   mode    GPIO mode
 *
 * @retval  kNoErr  Success
 * @retval  others  Failure
 */
merr_t mhal_gpio_open(int gpio, mhal_gpio_mode_t mode);

/**
 * @brief   Close an GPIO device
 * 
 * @param   gpio    GPIO name
 */
void mhal_gpio_close(int gpio);

/**
 * @brief   Set a GPIO output high
 * 
 * @param   gpio    GPIO name
 */
void mhal_gpio_high(int gpio);

/**
 * @brief   Set a GPIO output low
 * 
 * @param   gpio    GPIO name
 */
void mhal_gpio_low(int gpio);

/**
 * @brief   Toggle a GPIO output
 * 
 * @param   gpio    GPIO name
 */
void mhal_gpio_toggle(int gpio);

/**
 * @brief   Read a GPIO input value
 * 
 * @param   gpio    GPIO name
 * 
 * @retval  true    High
 * @retval  fasle   Low
 */
bool mhal_gpio_value(int gpio);

/**
 * @brief   Enable interrupt for a GPIO pin
 * 
 * @param   gpio    GPIO name
 * @param   mode    The type of mode (rising/falling edge)
 * @param   handler A function pointer to the interrupt handler
 * @param   arg     An argument that will be passed to the interrupt handler
 */
void mhal_gpio_int_on(int gpio, mhal_gpio_irq_mode_t mode, mhal_gpio_irq_handler_t handler, void *arg);

/**
 * @brief   Disable interrupt for a GPIO pin
 * 
 * @param   gpio    GPIO name
 */
void mhal_gpio_int_off(int gpio);

/** 
 * @} 
 */
/** 
 * @} 
 */

#endif
