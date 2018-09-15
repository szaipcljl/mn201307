
#ifndef __MAIN_H
#define __MAIN_H

#include <string.h> /* strlen */
#include <stdio.h>  /* sprintf */
#include <math.h>   /* trunc */

#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_conf.h"
#include "stm32f4xx_hal_def.h"

#include "i2c.h"
#include "spi.h"
#include "usart.h"
#include "gpio.h"

#include "bma2x2.h"
#include "bmg160.h"
#include "bstdr_comm_support.h"
#include "ak09911.h"
#include "AlgorithmManager.h"
#include "sensor_protocol.h"
//#include "Dsh_ipc.h"

#include "transport.h"
#include "protocol.h"

#define ONE_KHZ_TIME 1
#define ONE_HUN_HZ_TIME 10 

#endif /* __MAIN_H */
