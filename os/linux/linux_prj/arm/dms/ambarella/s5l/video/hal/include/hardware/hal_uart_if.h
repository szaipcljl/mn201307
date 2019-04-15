#ifndef _HAL_UART_IF_H_
#define _HAL_UART_IF_H_

/* ===========================================================================
//
//  Begin C in a C++ environment
//
// ======================================================================== */
#ifdef __cplusplus
extern "C" {
#endif

#include "hal_common_if.h"
#include "hal_config_a1.h"

typedef signed16 HAL_UART_HANDLE;


typedef enum
{
	HAL_UART_PARITY_NONE = 0,
	HAL_UART_PARITY_ODD,
	HAL_UART_PARITY_EVEN,
	HAL_UART_PARITY_MARK,
	HAL_UART_PARITY_SPACE
}HAL_UART_PARITY;

typedef enum
{
    HAL_UART_FLOW_CONTROL_OFF = 0,
    HAL_UART_FLOW_CONTROL_XON_XOFF,
    HAL_UART_FLOW_CONTROL_RTS_CTS
    
} HAL_UART_FLOW_CONTROL;

typedef enum
{
	TIMEOUT_DISABLE = 0,
	TIMEOUT_ENABLE = 1
}TIMEOUT;

typedef enum
{
	HAL_TIMEOUT_IMMEDIATE = 0,
	HAL_TIMEOUT_FOREVER = 0xffffffff,
}HAL_TIMEOUT;



/* Function to be called from hal-init process only */
extern void hal_uart_init(void);
/**************************************************************************************
 *
 * Function Name: hal_uart_open
 *
 * Argumetns: handle, device_id, *buffer, buffer_size, baud, data_bits, stop_bits, parity, flow_control
 *
 * Return:  HAL_STATUS 
 *
 * Note: This function used to intialize the UART PORT for reading and writing. UART is initialized based on the basic settings 
 which are passed as arguments.   
 *
 **************************************************************************************/
extern HAL_STATUS hal_uart_open(HAL_UART_HANDLE *handle, HAL_UART_DEVICE_ID device_id,  unsigned32 baud, unsigned8 data_bits, unsigned8 stop_bits, HAL_UART_PARITY parity, HAL_UART_FLOW_CONTROL flow_control );
/**************************************************************************************
 *
 * Function Name: hal_uart_write
 *
 * Argumetns: HAL_UART_HANDLE handle, unsigned8 *buffer, unsigned32 buffer_size, unsigned32 timeout_ms)
 *
 * Return:  HAL_STATUS 
 *
 * Note: This function used to used to write the UART port. buffer data will be transmitted in UART.
 * 	 If the data is not transmited according to expected time, timeout error will be thrown.
 *
 **************************************************************************************/
extern HAL_STATUS hal_uart_write(HAL_UART_HANDLE handle, const unsigned8 *write_buffer, unsigned32 buffer_size);
/**************************************************************************************
 *
 * Function Name: hal_uart_read
 *
 * Argumetns: HAL_UART_HANDLE handle, unsigned8 *read_buffer, unsigned32 read_length
 *
 * Return:  HAL_STATUS 
 *
 * Note: This function used to used to read the UART port. characters will be read from the port
 * 	 If the character is not available as per expected time, timeout error will be thrown.
 *
 * Important Note: As of now this is iplemented to read single character from UART port and
 * time out to be implement in future.
 **************************************************************************************/
extern HAL_STATUS hal_uart_read( HAL_UART_HANDLE handle, unsigned8 *read_buffer, unsigned32 timeout );


#ifdef __cplusplus
}
#endif

#endif

