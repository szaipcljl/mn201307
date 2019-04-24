#ifndef _CMR_DEV_INFO_H
#define _CMR_DEV_INFO_H

#define I2C_BUS_DEV0 "/dev/i2c-0"
#define I2C_BUS_DEV1 "/dev/i2c-1"

#define AR0144_SENSOR

#ifdef AR0144_SENSOR
// dev related
#define AR0144_ADAPT_NODE			I2C_BUS_DEV1
#define AR0144_I2C_ADDR				0x49 //7bit, remaped by ds90ub934
#define AR0144_BUS_ID				1

// reg related
#define AR0144_FLASH_CTL1			0x3270

#define AR0144_TEMP_CTL				0x30b4
#define AR0144_TEMP_EN_VAL			0x11
#define AR0144_TEMP_REG				0x30b2
#define AR0144_FLASH_ON_MASK		(0x0100)

//
#define ADAPT_NODE					AR0144_ADAPT_NODE
#define I2C_ADDR					AR0144_I2C_ADDR
#define BUS_ID						AR0144_BUS_ID

#define FLASH_CTL					AR0144_FLASH_CTL1
#define FLASH_ON_MASK				AR0144_FLASH_ON_MASK

#define TEMP_CTL					AR0144_TEMP_CTL
#define TEMP_EN_VAL					AR0144_TEMP_EN_VAL
#define TEMP_REG					AR0144_TEMP_REG

#else

#define ADAPT_NODE
#define I2C_ADDR
#define BUS_ID

#define FLASH_CTL
#define TEMP_CTL
#define TEMP_EN_VAL
#define TEMP_REG


#endif

// dev related
#define CMR_DEV_ADAPT_NODE		ADAPT_NODE
#define CMR_DEV_I2C_ADDR		I2C_ADDR
#define CMR_BUS_ID				BUS_ID

#define CMR_FLASH_CTL			FLASH_CTL
#define CMR_FLASH_ON_MASk		FLASH_ON_MASK

#define CMR_TEMP_CTL			TEMP_CTL
#define CMR_TEMP_EN_VAL			TEMP_EN_VAL
#define CMR_TEMP_REG			TEMP_REG

#define cmr_ir_led_status(val)		(!!(val & CMR_FLASH_ON_MASk))

#endif
