//-----------------------------------------------------------------------------
//
// Copyright (c) 2009 MStar Semiconductor, Inc.  All rights reserved.
//
//-----------------------------------------------------------------------------
// FILE
//      gpio_control.h
//
// DESCRIPTION
//      GPIO control. Provide the same interface to access GPIO.
//      The reason why don't to use GPIO programming interface in kernel
//      is that we share this module in kernel and u-boot.
//
// HISTORY
//      2009.07.09       Jimmy Hsu          Initial Version.
//      2009.08.19       Jimmy Hsu          Refine API to follow GPIO programming
//                                          interface in kernel.
//
//-----------------------------------------------------------------------------
#ifndef GPIO_CONTROL_H
#define GPIO_CONTROL_H

//-----------------------------------------------------------------------------
//    Include Files
//-----------------------------------------------------------------------------
#ifdef __UBOOT__
    #include <linux/types.h>
#else
    #include <linux/kernel.h>
#endif

//-----------------------------------------------------------------------------
//    EXTERN definition
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//    Constant
//-----------------------------------------------------------------------------
#define GPIO_OK                        0
#define GPIO_ERROR_INVALID            -1
#define GPIO_ERROR_REREGISTER         -2
#define GPIO_ERROR_NONREGISTER        -3
#define GPIO_ERROR_INPUT_INVALID      -4
#define GPIO_ERROR_OUTPUT_INVALID     -5

#define GPIO_NR_IRQ_PINS              13 // Only 13 bits for GPIO IRQ.

//-----------------------------------------------------------------------------
//    Data Type
//-----------------------------------------------------------------------------
typedef enum gpio_pin_e
{
    GPIO_PIN_INVALID = -1,
    GPIO_PIN_G00 = 0, // GPIO_PIN_G series must as first.
    GPIO_PIN_G01,
    GPIO_PIN_G02,
    GPIO_PIN_G03,
    GPIO_PIN_G04,
    GPIO_PIN_G05,
    GPIO_PIN_G06,
    GPIO_PIN_G07,
    GPIO_PIN_G08,
    GPIO_PIN_G09,
    GPIO_PIN_G10,
    GPIO_PIN_G11,
    GPIO_PIN_G12,
    GPIO_PIN_G13,
    GPIO_PIN_G14,
    GPIO_PIN_G15,
    GPIO_NR_GPINS,
    GPIO_PIN_U00,
    GPIO_PIN_U01,
    GPIO_PIN_U02,
    GPIO_PIN_U03,
    GPIO_PIN_U04,
    GPIO_PIN_U05,
    GPIO_PIN_U06,
    GPIO_PIN_U07,
    GPIO_NR_UPINS,
    GPIO_PIN_A00, // Share with I2S/PCM/SPDIF
    GPIO_PIN_A01,
    GPIO_PIN_A02,
    GPIO_PIN_A03,
    GPIO_PIN_A04,
    GPIO_PIN_A05,
    GPIO_PIN_A06,
    GPIO_PIN_A07,
    GPIO_PIN_A08,
    GPIO_PIN_A09, // Share with UART
    GPIO_PIN_A10,
    GPIO_PIN_A11, // Share with JTAG
    GPIO_PIN_A12,
    GPIO_PIN_A13,
    GPIO_PIN_A14,
    GPIO_PIN_A15,
    GPIO_NR_APINS,
    // You can add GPIO_S/C/N/P...series below.
    GPIO_NR_PINS,
} gpio_pin_et;

#define GPIO_PIN_G16             GPIO_PIN_U00
#define GPIO_PIN_G17             GPIO_PIN_U01
#define GPIO_PIN_G18             GPIO_PIN_U02
#define GPIO_PIN_G19             GPIO_PIN_U03
#define GPIO_PIN_G20             GPIO_PIN_U04
#define GPIO_PIN_G21             GPIO_PIN_U05
#define GPIO_PIN_G22             GPIO_PIN_U06
#define GPIO_PIN_G23             GPIO_PIN_U07

typedef enum gpio_status_e
{
    GPIO_STATUS_INVALID = -1,
    GPIO_STATUS_NONE,
    GPIO_STATUS_REQUESTED,
    GPIO_STATUS_IS_OUT,
    GPIO_STATUS_IRQ,
} gpio_status_et;

// The callback function to handle the GPIO IRQ.
typedef s32 (*gpio_irq_handler_pfn)(s32 irq, void *data);

typedef struct gpio_desc_s
{
    const char *label;
    gpio_irq_handler_pfn irq_handler;
    gpio_status_et status;
} gpio_desc_st;

//-----------------------------------------------------------------------------
//    Variable
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//    Macro
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//    Global Function
//-----------------------------------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

//-----------------------------------------------------------------------------
//  Function: gpio_is_valid
//
//  Purpose:
//      Check GPIO is valid, but don't check GPIO has registered.
//
//  Parameters:
//      pin: [in] GPIO pin.
// 
//  Return Values:
//      0 indicates success,
//      negative number indicates failure.
//-----------------------------------------------------------------------------
    s32 gpio_is_valid(gpio_pin_et pin);

//-----------------------------------------------------------------------------
//  Function: gpio_request
//
//  Purpose:
//      Request the GPIO to use.
//
//  Parameters:
//      pin:   [in] GPIO pin.
//      label: [in[ GPIO label description.
// 
//  Return Values:
//      0 indicates success,
//      negative number indicates failure.
//-----------------------------------------------------------------------------
    s32 gpio_request(gpio_pin_et pin, const char *label);

//-----------------------------------------------------------------------------
//  Function: gpio_irq_request
//
//  Purpose:
//      Request the GPIO IRQ to use.
//
//  Parameters:
//      pin:     [in] GPIO pin.
//      label:   [in] GPIO label description.
//      handler: [in] IRQ hander function.
// 
//  Return Values:
//      0 indicates success,
//      negative number indicates failure.
//-----------------------------------------------------------------------------
    s32 gpio_irq_request(gpio_pin_et pin, const char *label, gpio_irq_handler_pfn handler);

//-----------------------------------------------------------------------------
//  Function: gpio_free
//
//  Purpose:
//      Free the GPIO/GPIO IRQ.
//
//  Parameters:
//      pin: [in] GPIO pin.
// 
//  Return Values:
//      None.
//-----------------------------------------------------------------------------
    void gpio_free(gpio_pin_et pin);

//-----------------------------------------------------------------------------
//  Function: gpio_direction_input
//
//  Purpose:
//      GPIO input enable.
//
//  Parameters:
//      pin: [in] GPIO pin.
// 
//  Return Values:
//      0 indicates success,
//      negative number indicates failure.
//-----------------------------------------------------------------------------
    s32 gpio_direction_input(gpio_pin_et pin);

//-----------------------------------------------------------------------------
//  Function: gpio_direction_output
//
//  Purpose:
//      GPIO output enable.
//
//  Parameters:
//      pin:   [in] GPIO pin.
//      value: [in] 1 for high, 0 for low.
// 
//  Return Values:
//      0 indicates success,
//      negative number indicates failure.
//-----------------------------------------------------------------------------
    s32 gpio_direction_output(gpio_pin_et pin, s32 value);

//-----------------------------------------------------------------------------
//  Function: gpio_get_value
//
//  Purpose:
//      Get GPIO value.
//
//  Parameters:
//      pin: [in] GPIO pin.
// 
//  Return Values:
//      0 indicates high, 1 indicates low,
//      negative number indicates failure.
//-----------------------------------------------------------------------------
    s32 gpio_get_value(gpio_pin_et pin);

//-----------------------------------------------------------------------------
//  Function: gpio_set_value
//
//  Purpose:
//      Set GPIO value.
//
//  Parameters:
//      pin:   [in] GPIO pin.
//      value: [in] 1 for high, 0 for low.
// 
//  Return Values:
//      0 indicates success,
//      negative number indicates failure.
//-----------------------------------------------------------------------------
    s32 gpio_set_value(gpio_pin_et pin, s32 value);

//-----------------------------------------------------------------------------
//  Function: gpio_show
//
//  Purpose:
//      Show every GPIO status/lable.
//
//  Parameters:
//      None.
// 
//  Return Values:
//      None.
//-----------------------------------------------------------------------------
    void gpio_show(void);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // GPIO_CONTROL_H

