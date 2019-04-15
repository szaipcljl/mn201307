#ifndef HAL_CONFIG_A1_HH
#define HAL_CONFIG_A1_HH
#include "hal_common_if.h"


void       debug_vmf_trace_init(void);
unsigned8  debug_get_vmf_trace_id(void); 
	
/* Debugging definitions */
#define HAL_GPIO_TRACE(...) 	printf("HAL_GPIO:");printf(__VA_ARGS__);printf("line is %d.", __LINE__);printf("func is %s.", __FUNCTION__);printf("\n");
#define HAL_I2C_TRACE(...)  		printf("HAL_I2C:");printf(__VA_ARGS__);printf("line is %d.", __LINE__);printf("func is %s.", __FUNCTION__);printf("\n"); 
#define HAL_SPI_TRACE(...)  		printf("HAL_SPI:");printf(__VA_ARGS__);printf("line is %d.", __LINE__);printf("func is %s.", __FUNCTION__);printf("\n"); 
#define HAL_UART_TRACE(...)  	printf("HAL_UART:");printf(__VA_ARGS__);printf("line is %d.", __LINE__);printf("func is %s.", __FUNCTION__);printf("\n"); 



#if defined(HAL_DEBUG)
	#define HAL_GPIO_DEBUG(...) if(TRUE == process_debug_enable){printf("HAL_GPIO:");printf(__VA_ARGS__);printf("line is %d.", __LINE__);printf("func is %s.", __FUNCTION__);printf("\n");}

	#define HAL_ADC_DEBUG(...)  if(TRUE == process_debug_enable){printf("HAL_ADC:");printf(__VA_ARGS__);printf("line is %d.", __LINE__);printf("func is %s.", __FUNCTION__);printf("\n");}
		
	#define HAL_PWM_DEBUG(...)  if(TRUE == process_debug_enable){printf("HAL_PWM:");printf(__VA_ARGS__);printf("line is %d.", __LINE__);printf("func is %s.", __FUNCTION__);printf("\n");}
		
	#define HAL_VBUS_DEBUG(...) if(TRUE == process_debug_enable){printf("HAL_VBUS:");printf(__VA_ARGS__);printf("line is %d.", __LINE__);printf("func is %s.", __FUNCTION__);printf("\n");}

	#define HAL_UART_DEBUG(...) if(TRUE == process_debug_enable){printf("HAL_UART:");printf(__VA_ARGS__);printf("line is %d.", __LINE__);printf("func is %s.", __FUNCTION__);printf("\n");}

#else
	#define HAL_GPIO_DEBUG(...)

	#define HAL_ADC_DEBUG(...) 
		
	#define HAL_PWM_DEBUG(...) 
		
	#define HAL_VBUS_DEBUG(...)

	#define HAL_UART_DEBUG(...) 

#endif
//#define HAL_VMF_DEBUG		


#if 0
#define DEBUG_INIT()	debug_vmf_trace_init(); /* setbuf(stdout, NULL)  No buffering, to avoid out-of-sync debug output from different procs */
#define DEBUG_GPIO_ERR(_x_)	strcpy(_func, __FUNCTION__); _line=__LINE__; VMF_TRACE_PRINTF(hal_gpio_shared->handles[i].vmf_trace_id, VMF_TRACE_SEV_STD_ERROR,   STR(DBG_TASK_ID), ": ERROR",   "%s line %d:", _func, _line); 
        
#define DEBUG_GPIO_WRN(_x_)	strcpy(_func, __FUNCTION__); _line=__LINE__; VMF_TRACE_PRINTF(hal_gpio_shared->handles[i].vmf_trace_id, VMF_TRACE_SEV_STD_WARNING,  STR(DBG_TASK_ID), ": WARNING", "%s line %d:", _func, _line); 

//#define debug_nw_gw(SEVERITY, TASK, MSG, ...) 
//        DEBUG_NW_PRINTF(vgw_trace_id, (SEVERITY), TASK, (char *)__PRETTY_FUNCTION__, MSG, ##__VA_ARGS__)
#endif


/*****************************************************************************************
 * HAL GPIO
 */

/* Port range for this micro */
#define HAL_GPIO_MIN_PORT 			0
#define HAL_GPIO_MAX_PORT 			159
#define HAL_GPIO_NULL_PORT			160
#define GPIO_MIN_HANDLE 			1

/* 0 value handle is reserved */
#define RESERVE_HANDLE				0

/* The maximum number of GPIO handles that can be opened */
#define HAL_GPIO_NUMBER_OF_HANDLES	160

/* The maximum number of ADC handles that can be opened */
#define ADC_MIN_HANDLE 				1
#define HAL_ADC_NUMBER_OF_HANDLES	10

/* The maximum number of PWM handles that can be opened */
#define PWM_MIN_HANDLE 				1
#define HAL_PWM_NUMBER_OF_HANDLES	10

/* The maximum number of VBUS handles that can be opened */
#define VBUS_MIN_HANDLE				1
#define HAL_VBUS_NUMBER_OF_HANDLES	5

/* The polling period for debounced GPIO handles */
#define HAL_GPIO_POLLING_PERIOD_MS 	20

/* Get definitions needed */
#include "hal_gpio_if.h"

/* GPIOs allocated for testing purposes */
#define HAL_GPIO_PORT_already_allocated_by_bsp  ((HAL_GPIO_PORT)36)     /* Should be same as i2c-2 */
#define HAL_GPIO_PORT_not_connected             ((HAL_GPIO_PORT)127)     /* Should not be connected to anything */
#define HAL_GPIO_PORT_pulled_high                HAL_GPIO_PORT_not_connected  /* NOT IDEAL - need something that is pulled high */
#define HAL_GPIO_PORT_eject_cd                  ((HAL_GPIO_PORT)113) 

/* GPIOs allocated for NGI */
#define HAL_GPIO_PORT_CD_RESET                  ((HAL_GPIO_PORT)61)
#define BT_RESET_SIGNAL							((HAL_GPIO_PORT)7)
#define IAP_RESET								((HAL_GPIO_PORT)39)
#define IPCL_CS									((HAL_GPIO_PORT)83)
#define DSP_RESET								((HAL_GPIO_PORT)148)


/*AD convert for USB*/
#define HAL_GPIO_PORT_CHANNEL0		(1)
#define HAL_GPIO_PORT_CHANNEL1		(1<<1)
#define HAL_GPIO_PORT_CHANNEL2		(1<<2)
#define HAL_GPIO_PORT_CHANNEL3		(1<<3)
#define HAL_GPIO_PORT_CHANNEL4		(1<<4)
#define HAL_GPIO_PORT_CHANNEL5		(1<<5)
#define HAL_GPIO_PORT_CHANNEL6		(1<<6)
#define HAL_GPIO_PORT_CHANNEL7		(1<<7)
#define HAL_GPIO_NULL_CHANNEL		0
#define HAL_ADC_CHANNEL_NUM			8
#define HAL_ADC_CHANNEL_MIN			1
#define HAL_ADC_CHANNEL_MAX			(HAL_GPIO_PORT_CHANNEL0|HAL_GPIO_PORT_CHANNEL1 \
									|HAL_GPIO_PORT_CHANNEL2|HAL_GPIO_PORT_CHANNEL3 \
									|HAL_GPIO_PORT_CHANNEL4|HAL_GPIO_PORT_CHANNEL5 \
									|HAL_GPIO_PORT_CHANNEL6|HAL_GPIO_PORT_CHANNEL7)

/* PWM devices */
#define NUMBER_OF_PWM_DEVICES		1

typedef enum
{
	HAL_PWM_DEVICE_SPEED = 0,
	HAL_PWM_MAX_NUM,
} HAL_PWM_DEVICE_ID;

typedef struct
{
	unsigned16 deviceID;
}HAL_PWM_CONFIG_ENTRY;

extern HAL_PWM_CONFIG_ENTRY hal_pwm_config[NUMBER_OF_PWM_DEVICES];

/* VBUS devices */
/* There are 4 states between VBUS high and low */
#define VBUS_HIGH					"3"
#define VBUS_LOW					"0"
#define NUMBER_OF_VBUS_DEVICES		1
typedef unsigned16 HAL_VBUS_BUS;

typedef enum
{
	HAL_VBUS_DEVICE_BOTH = 0,
	HAL_VBUS_MAX_NUM,
}HAL_VBUS_DEVICE_ID;

typedef struct
{
	unsigned16 deviceID;
}HAL_VBUS_CONFIG_ENTRY;

extern HAL_VBUS_CONFIG_ENTRY hal_vbus_config[NUMBER_OF_VBUS_DEVICES];
/*****************************************************************************************
 * HAL I2C
 */

#define NUMBER_OF_I2C_DEVICES 		3
#define HAL_I2C_DEVICEID_VIP		(0x30)
#define HAL_I2C_DEVICEID_CD				
#define HAL_I2C_DEVICEID_TUNER
#define HAL_I2C_DEVICEID_AUDIO
#define HAL_I2C_DEVICEID_AMP		(0xD4)
#define HAL_I2C_DEVICEID_CODEC  (0x20)
#define HAL_I2C_DEVICEID_DSP		(0x18)
#define HAL_I2C_DEVICEID_IPOD	(0x11)


typedef struct
{
	unsigned16 deviceID;
	unsigned16 i2cBus;
	unsigned32 baud;    
}HAL_I2C_CONFIG_ENTRY;
 
 
/*****************************************************************************************
 * HAL SPI
 */

#define SS_NOTUSE	(-2)

/* SPI devices */
typedef enum
{
	HAL_SPI_DEVICE_ID_VIP = 0,
	HAL_SPI_DEVICE_ID_IPOD,
	HAL_SPI_DEVICE_ID_IAP,
	NUMBER_OF_SPI_DEVICES,

} HAL_SPI_DEVICE_ID;
  
/* Types */
typedef unsigned16 HAL_SPI_BUS;

/* Config table structure */
typedef struct
{
    HAL_SPI_DEVICE_ID deviceID;
    HAL_GPIO_PORT chip_select;
    HAL_SPI_BUS spi_bus;
} HAL_SPI_CONFIG_ENTRY;

extern HAL_SPI_CONFIG_ENTRY hal_spi_config[NUMBER_OF_SPI_DEVICES];

/* The maximum number of SPI handles that can be opened */
#define HAL_SPI_NUMBER_OF_HANDLES 160

/*****************************************************************************************
 * HAL UART
 */

/* UART devices */
typedef enum
{
	HAL_UART_DEVICE_ID_PRINTF = 0,
	HAL_UART_DEVICE_ID_DEVIPC,    
    NUMBER_OF_UART_DEVICES
    
} HAL_UART_DEVICE_ID;

/* Types */
typedef unsigned16 HAL_UART_BUS;
   
/* Config table structure */
typedef struct
{
    HAL_UART_DEVICE_ID  deviceID;
    HAL_UART_BUS        uart_bus;  // NOTDONEYET: Update hal_document to reflect this
    HAL_GPIO_PORT       rts;
    HAL_GPIO_PORT       cts;
} HAL_UART_CONFIG_ENTRY;

extern HAL_UART_CONFIG_ENTRY hal_uart_config[NUMBER_OF_UART_DEVICES];

/* The maximum number of UART handles that can be opened */
#define HAL_UART_NUMBER_OF_HANDLES	160

#define HAL_UART_MIN_HANDLE			1

extern char *uart_dev_path[NUMBER_OF_UART_DEVICES];
 


#endif

