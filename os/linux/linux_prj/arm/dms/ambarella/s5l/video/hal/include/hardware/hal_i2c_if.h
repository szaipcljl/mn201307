/* ===========================================================================
//
//  Begin C in a C++ environment
//
// ======================================================================== */
#ifdef __cplusplus
extern "C" {
#endif

#include "hal_common_if.h"

#ifndef _I2C_H_
#define _I2C_H_

#include "hal_config.h"

typedef struct  
{
 unsigned16 device_addr;
 signed16 file;
 unsigned16 adapter_nr;
}*HAL_I2C_HANDLE,HAL_I2C;


HAL_STATUS hal_i2c_open(HAL_I2C_HANDLE *handle, unsigned16 deviceID);
HAL_STATUS hal_i2c_read(HAL_I2C_HANDLE handle, unsigned8 *value, unsigned16 length, unsigned32 timeout_ms);
HAL_STATUS hal_i2c_write(HAL_I2C_HANDLE handle, unsigned8 *value, unsigned16 length, unsigned32 timeout_ms);
HAL_STATUS hal_i2c_close(HAL_I2C_HANDLE handle);

void hal_i2c_init(void);

#ifdef __cplusplus
}
#endif

#endif
