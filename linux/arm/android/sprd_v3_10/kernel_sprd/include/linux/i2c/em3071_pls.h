#ifndef  __EM3071_PLS_H__
#define  __EM3071_PLS_H__

#define EM3071_ALS_DEVICE	"em3071_als"
#define EM3071_PLS_DEVICE 	"em3071_pls"
#define EM3071_PLS_INPUT_DEV	"proximity"
#define EM3071_ALS_INPUT_DEV    "lightsensor"
#define EM3071_PLS_ADDRESS		0x24
#define EM3071_PLS_RETRY_COUNT	3

#define I2C_BUS_NUM_STATIC_ALLOC 1 
#define I2C_STATIC_BUS_NUM (2) 
#include <linux/types.h>
#include <linux/ioctl.h>

#define LTR_IOCTL_MAGIC 				0x1C
#define LTR_IOCTL_GET_PFLAG  			_IOR(LTR_IOCTL_MAGIC, 1, int)
#define LTR_IOCTL_GET_LFLAG  			_IOR(LTR_IOCTL_MAGIC, 2, int)
#define LTR_IOCTL_SET_PFLAG  			_IOW(LTR_IOCTL_MAGIC, 3, int)
#define LTR_IOCTL_SET_LFLAG  			_IOW(LTR_IOCTL_MAGIC, 4, int)
#define LTR_IOCTL_GET_DATA  			_IOW(LTR_IOCTL_MAGIC, 5, unsigned char)
#define LTR_IOCTL_GET_CHIPINFO              _IOR(LTR_IOCTL_MAGIC, 6, char)


//REG
#define EM3071_PLS_REG_PID				0x00
#define EM3071_PLS_REG_CONFIG			0x01
#define EM3071_PLS_REG_INT_STATUS		0x02
#define EM3071_PLS_REG_LOW_THD			0x03
#define EM3071_PLS_REG_HIGHT_THD		0x04
#define EM3071_PLS_REG_ALS_THD1			0x05
#define EM3071_PLS_REG_ALS_THD2			0x06
#define EM3071_PLS_REG_ALS_THD3			0x07
#define EM3071_PLS_REG_PS_DATA			0x08
#define EM3071_PLS_REG_ALS_L_DATA		0x09
#define EM3071_PLS_REG_ALS_H_DATA		0x0A

#define EM3071_PLS_REG_OFFSET			0x0F

#define EM3071_ENABLE_PS				0xB8
#define EM3071_ENABLE_ALS				0x06
#define EM3071_ENABLE_BOTH				0xBE

#define EM3071_PS_L_THD					0X70
#define EM3071_PS_H_THD					0X80

//#define EM3071_PS_CLOSE_THD				120
//#define EM3071_PS_FAR_THD				90

static int prox_value = 0;
static atomic_t system_ready;
#define DEFAULT_DELAY_TIME 				200

typedef enum _SENSOR_TYPE{
	EM3071_PLS_ALPS=0,
	EM3071_PLS_PXY,
	EM3071_PLS_BOTH
}SENSOR_TYPE;


//-------------- Define Sensor Data Struct ---------------//
typedef struct EM3071_data {
	struct i2c_client *client;
	struct mutex update_lock;
	struct mutex data_mutex;
	struct work_struct work;//prox_work;
	struct delayed_work delay_work;
	struct input_dev *input;
	struct workqueue_struct *em_work_queue;
	struct early_suspend em_early_suspend;

	atomic_t enable;
	atomic_t delay;
	atomic_t last;
	atomic_t last_status;

}EM3071_data;

struct em3071_pls_platform_data {
        int irq_gpio_number;
};

#endif

