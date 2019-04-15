#ifndef _HAL_GPIO_IF_H_
#define _HAL_GPIO_IF_H_

/* ===========================================================================
//
//  Begin C in a C++ environment
//
// ======================================================================== */
#ifdef __cplusplus
extern "C" {
#endif

#include "hal_common_if.h"

#include "hal_common_if.h"

typedef signed16 HAL_GPIO_HANDLE;

typedef signed16 HAL_VBUS_HANDLE;

typedef signed16 HAL_GPIO_PORT;

typedef unsigned16 HAL_ADC_CHANNEL;

#define HAL_GPIO_PORT_NONE (-1)

typedef enum
{
	GPIO,
	PWM,
	ADC,
	VBUS,
	TYPES_NUM

}HAL_IO_TYPES;

typedef enum
{
    HAL_GPIO_DIRECTION_INPUT,
    HAL_GPIO_DIRECTION_OUTPUT
    
} HAL_GPIO_DIRECTION;

typedef enum
{
    HAL_GPIO_STATE_UNKNOWN,
    HAL_GPIO_STATE_LOW,
    HAL_GPIO_STATE_HIGH,
    HAL_GPIO_STATE_HIZ
    
} HAL_GPIO_STATE;


#include "hal_config.h"

/* Function to be called from hal-init process only */
extern void hal_gpio_init(void);

/* Functions to be called by application processes */
/* ===========================================================================
//
//  Name:           hal_gpio_open
//
//  Description:    Opens a hal gpio handle for the port number input, and also 
//	set direction, debounce time, callback function pointer.
//
// ======================================================================== */
extern HAL_STATUS hal_gpio_open(HAL_GPIO_HANDLE *handle, HAL_GPIO_PORT port, HAL_GPIO_DIRECTION direction, unsigned32 debounce_time_ms, void (*state_change_callback)(HAL_GPIO_HANDLE handle, HAL_GPIO_STATE state, bool is_stable));

/* ===========================================================================
//
//  Name:           hal_gpio_close
//
//  Description:    Closes a hal gpio handle. Also it will remove the gpio files
//	in the file system.
//
// ======================================================================== */
extern HAL_STATUS hal_gpio_close(HAL_GPIO_HANDLE handle);

/* ===========================================================================
//
//  Name:           hal_gpio_read
//
//  Description:    Reads the state of a gpio port using a hal gpio handle.
//	Result will be written in "state". If debounce is using, is_stable will 
//	feedback whether the value is stable.
//
// ======================================================================== */
extern HAL_STATUS hal_gpio_read(HAL_GPIO_HANDLE handle, HAL_GPIO_STATE* state, bool* is_stable);

/* ===========================================================================
//
//  Name:           hal_gpio_write
//
//  Description:    Changes the state of a gpio port using a hal gpio handle.
//
// ======================================================================== */
extern HAL_STATUS hal_gpio_write(HAL_GPIO_HANDLE handle, HAL_GPIO_STATE desired_port_state);

/* ===========================================================================
//
//  Name:           hal_gpio_cha_adc_open
//
//  Description:    Open a ADC device and give a handle. Channel should use 
//	Macro set in hal_config_a1.h.
//
// ======================================================================== */
extern HAL_STATUS hal_adc_open(HAL_GPIO_HANDLE * handle, HAL_GPIO_PORT channel);

/* ===========================================================================
//
//  Name:           hal_gpio_cha_adc_read
//
//  Description:    Reads the state of a gpio port using a hal gpio handle. The accuracy is 10-bits.
//
// ======================================================================== */
extern HAL_STATUS hal_adc_read(HAL_GPIO_HANDLE handle, short *value);

/* ===========================================================================
//
//  Name:           hal_gpio_cha_adc_close
//
//  Description:    Closes a hal ADC handle. 
//
// ======================================================================== */
extern HAL_STATUS hal_adc_close(HAL_GPIO_HANDLE handle);
/* ===========================================================================
//
//  Name:           hal_gpio_cha_adc_open
//
//  Description:    Open a ADC device and give a handle. Channel should use 
//	Macro set in hal_config_a1.h.
//
// ======================================================================== */
extern HAL_STATUS hal_gpio_cha_adc_open(HAL_GPIO_HANDLE * handle, HAL_GPIO_PORT channel);

/* ===========================================================================
//
//  Name:           hal_gpio_cha_adc_read
//
//  Description:    Reads the state of a gpio port using a hal gpio handle. The accuracy is 10-bits.
//
// ======================================================================== */
extern HAL_STATUS hal_gpio_cha_adc_read(HAL_GPIO_HANDLE handle, short *value);

/* ===========================================================================
//
//  Name:           hal_gpio_cha_adc_close
//
//  Description:    Closes a hal ADC handle. 
//
// ======================================================================== */
extern HAL_STATUS hal_gpio_cha_adc_close(HAL_GPIO_HANDLE handle);

/* ===========================================================================
//
//  Name:           hal_pwminput_open
//
//  Description:    Open a PWM device and give a handle. Please refer to hal_config_a1.h 
//	for configuration table. Then decide the device_id.
//
// ======================================================================== */
extern HAL_STATUS hal_pwminput_open(HAL_GPIO_HANDLE *handle, unsigned16 device_id);

/* ===========================================================================
//
//  Name:           hal_pwminput_read_frequency_Hz
//
//  Description:    Reads the frequency of a PWM using a hal pwm handle.
//
// ======================================================================== */
extern HAL_STATUS hal_pwminput_read_frequency_Hz(HAL_GPIO_HANDLE handle, unsigned32 *frequency);

/* ===========================================================================
//
//  Name:           hal_pwminput_read_duty_cycle
//
//  Description:    Reads the duty cycle of a PWM using a hal pwm handle.
//
// ======================================================================== */
extern HAL_STATUS hal_pwminput_read_duty_cycle(HAL_GPIO_HANDLE handle, unsigned32 *duty_cycle);

/* ===========================================================================
//
//  Name:           hal_pwminput_read_pulse_counter
//
//  Description:    TBD. This feature is not supported by ST.
//
// ======================================================================== */
extern HAL_STATUS hal_pwminput_read_pulse_counter(HAL_GPIO_HANDLE handle, unsigned32 *counts);

/* ===========================================================================
//
//  Name:           hal_pwminput_close
//
//  Description:    Closes a hal PWM handle. 
//
// ======================================================================== */
extern HAL_STATUS hal_pwminput_close(HAL_GPIO_HANDLE handle);

/* ===========================================================================
//
//  Name:           hal_vbus_open
//
//  Description:    Open a vbus device and give a handle. Please refer to hal_config_a1.h 
//	for configuration table. Then decide the device_id.
//
// ======================================================================== */
extern HAL_STATUS hal_vbus_open(HAL_VBUS_HANDLE *handle, unsigned16 device_id);

/* ===========================================================================
//
//  Name:           hal_vbus_read
//
//  Description:    Reads the vbus state(enable or disable) using a hal pwm handle.
//
// ======================================================================== */
extern HAL_STATUS hal_vbus_read( HAL_VBUS_HANDLE handle, char *read_buffer );

/* ===========================================================================
//
//  Name:           hal_vbus_write
//
//  Description:    Changes the state of a vbus using a hal gpio handle.
//
// ======================================================================== */
extern HAL_STATUS hal_vbus_write(HAL_VBUS_HANDLE handle, const char *write_buffer);

/* ===========================================================================
//
//  Name:           hal_vbus_close
//
//  Description:    Closes a hal vbus handle. 
//
// ======================================================================== */
extern HAL_STATUS hal_vbus_close(HAL_VBUS_HANDLE handle);

/* ===========================================================================
//
//  Name:           hal_gpio_debug_enable
//
//  Description:    enable debug statement for a client
//
// ======================================================================== */
extern void hal_gpio_debug_enable(void);

extern void hal_gpio_debug_disable(void);

#ifdef __cplusplus
}
#endif


#endif
