#ifndef _HAL_SPI_IF_H_
#define _HAL_SPI_IF_H_

/* ===========================================================================
//
//  Begin C in a C++ environment
//
// ======================================================================== */
#ifdef __cplusplus
extern "C" {
#endif

#include "hal_common_if.h"

typedef signed16 HAL_SPI_HANDLE;

typedef enum
{
    HAL_SPI_CPOL_0,
    HAL_SPI_CPOL_1
    
} HAL_SPI_CPOL;

typedef enum
{
    HAL_SPI_CPHA_0,
    HAL_SPI_CPHA_1
    
} HAL_SPI_CPHA;

#include "hal_config.h"

/* Function to be called from hal-init process only */
extern void hal_spi_init(void);

/* Functions to be called by application processes */
extern HAL_STATUS hal_spi_open(HAL_SPI_HANDLE *handle, HAL_SPI_DEVICE_ID device_id, unsigned32 speed, HAL_SPI_CPOL cpol, HAL_SPI_CPHA cpha, unsigned32 chip_select_assert_delay_ns, unsigned32 chip_select_release_delay_ns);
extern HAL_STATUS hal_spi_close(HAL_SPI_HANDLE handle);
extern HAL_STATUS hal_spi_read_write(HAL_SPI_HANDLE handle, const unsigned8 *read_buffer, unsigned16 read_buffer_size, const unsigned8 *write_buffer, unsigned16 write_buffer_size, unsigned32 timeout_ms);
extern HAL_STATUS hal_spi_read(HAL_SPI_HANDLE handle, const unsigned8 *read_buffer, unsigned16 read_buffer_size, unsigned32 timeout_ms);
extern HAL_STATUS hal_spi_write(HAL_SPI_HANDLE handle, const unsigned8 *write_buffer, unsigned16 write_buffer_size, unsigned32 timeout_ms);

#ifdef __cplusplus
}
#endif


#endif
