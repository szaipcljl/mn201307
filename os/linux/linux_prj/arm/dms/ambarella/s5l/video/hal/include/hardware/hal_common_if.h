#ifndef _HAL_COMMON_IF_H_
#define _HAL_COMMON_IF_H_




#ifndef _UNSIGNED_32
   #define _UNSIGNED_32
   typedef unsigned long int     unsigned32;
#endif


#ifndef _SIGNED_32
   #define _SIGNED_32
   typedef long int     signed32;
#endif


#ifndef _UNSIGNED_16
   #define _UNSIGNED_16
   typedef unsigned short     unsigned16;
#endif


#ifndef _SIGNED_16
   #define _SIGNED_16
   typedef short int    signed16;
#endif


#ifndef _UNSIGNED_8
   #define _UNSIGNED_8
   typedef unsigned char      unsigned8;
#endif



#ifndef _SIGNED_8
   #define _SIGNED_8
   typedef signed char      signed8;
#endif


#ifndef _UNSIGNED_64
   #define _UNSIGNED_64
   typedef unsigned long long    unsigned64;
#endif

#ifndef _SIGNED_64
   #define _SIGNED_64
   typedef long long    signed64;
#endif

#ifndef _BOOLEAN
   #define _BOOLEAN
    /* For C++  - NOTDONEYET: There is a clash of symbols in the code - resolve properly */
    #ifndef __cplusplus
      typedef short bool;
    #endif
#endif


#ifndef true
	#define true	(1)
#endif

#ifndef TRUE
	#define TRUE 	(1)
#endif 

#ifndef  false
	#define false	(0)
#endif

#ifndef  FALSE
	#define FALSE	(0)
#endif



/* Assertions */
#define ASSERT(c) { if(!(c)) { printf("ASSERT() failed at file %s, line %d\n", __FILE__, __LINE__); }}


/* Common hal status values */
typedef enum
{
    HAL_STATUS_OK,
    
    HAL_STATUS_UNKNOWN_SYSTEM_ERROR,
    
    HAL_STATUS_NO_AVAILABLE_HANDLES,
    
    HAL_STATUS_TIMEOUT,
    
    HAL_STATUS_INVALID_HANDLE,
    
    HAL_STATUS_IO_PIN_ALREADY_ALLOCATED,
    
    HAL_STATUS_DEVICE_NOT_PRESENT,

    HAL_STATUS_DEVICE_NOT_CONFIGURED,

    HAL_STATUS_DEVICE_ALREADY_OPEN,
    
    HAL_STATUS_CANNOT_WRITE,

	HAL_STATUS_WRITE_INVALID_VALUE,
    
    HAL_STATUS_INVALID_STATE,

    HAL_STATUS_COMMS_FAILURE,

    HAL_STATUS_DEVICE_ILLEGAL_CONFIGURATION,

    HAL_STATUS_UART_NULL_BUFFER,

    HAL_STATUS_UART_ERROR_OVERRUN,

    HAL_STATUS_UART_ERROR_PARITY,

    HAL_STATUS_UART_ERROR_FRAME,

    HAL_STATUS_UART_ERROR_BREAK

} HAL_STATUS;

/* Shared memory keys - put here so we can make sure they are unique */
//#define UNIQUE_KEY_FOR_HAL_GPIO_SHARED 	1234
#define UNIQUE_KEY_FOR_HAL_IO_SHARED 	1234
#define UNIQUE_KEY_FOR_HAL_I2C_SHARED  	1235
#define UNIQUE_KEY_FOR_HAL_SPI_SHARED  	1236
#define UNIQUE_KEY_FOR_HAL_UART_SHARED 	1237
//#define UNIQUE_KEY_FOR_HAL_ADC_SHARED  	1238
//#define UNIQUE_KEY_FOR_HAL_PWM_SHARED  	1239
//#define UNIQUE_KEY_FOR_HAL_VBUS_SHARED	1240


#endif
