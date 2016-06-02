/*
 * File:         em3071_pls.c
 * Based on:
 * Author:       Yunlong Wang <Yunlong.Wang @spreadtrum.com>
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
    *
 */


#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/jiffies.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <linux/miscdevice.h>
#include <linux/mutex.h>
#include <linux/mm.h>
#include <linux/device.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <linux/sysctl.h>
#include <linux/input.h>
#include <linux/gpio.h>
#include <linux/irq.h>
#include <linux/earlysuspend.h>
#include <linux/platform_device.h>
#include <asm/uaccess.h>
#include <linux/slab.h>
#include <linux/wakelock.h>
#include <linux/i2c/em3071_pls.h>
#include <linux/of_gpio.h>

#define PLS_DEBUG 1
#define DEBUG_I2C_DATA 0

#define EM3071_PLS_ADC_LEVEL9

#if PLS_DEBUG
#define PLS_DBG(format, ...)	\
		printk(KERN_INFO "em3071 " format "\n", ## __VA_ARGS__)
#else
#define PLS_DBG(format, ...)
#endif

extern int sprd_3rdparty_gpio_pls_irq;

static atomic_t p_flag;
static atomic_t l_flag;

static struct i2c_client *this_client;
//add
static struct EM3071_data *this_data;
static struct EM3071_data *this_data_p;
//end
static int em3071_pls_opened=0;
/* Attribute */
static int em3071_pls_irq;
static unsigned char suspend_flag=0; //0: sleep out; 1: sleep in


static ssize_t em3071_pls_show_suspend(struct device* cd,struct device_attribute *attr, char* buf);
static ssize_t em3071_pls_store_suspend(struct device* cd, struct device_attribute *attr,const char* buf, size_t len);
static ssize_t em3071_pls_show_ps(struct device* cd,struct device_attribute *attr, char* buf);
static ssize_t em3071_pls_show_als(struct device* cd,struct device_attribute *attr, char* buf);


static ssize_t em3071_pls_show_version(struct device* cd,struct device_attribute *attr, char* buf);
static void em3071_pls_early_suspend(struct early_suspend *handler);
static void em3071_pls_early_resume(struct early_suspend *handler);
static int em3071_pls_write_data(unsigned char addr, unsigned char data);
static int em3071_pls_read_data(unsigned char addr, unsigned char *data);
static int em3071_pls_enable(SENSOR_TYPE type);
static int em3071_pls_disable(SENSOR_TYPE type);
static void em3071_pls_report_init(void);
static void em3071_als_work(struct work_struct *work);
static int g_raw_dat;


static struct wake_lock pls_delayed_work_wake_lock;
static DECLARE_DELAYED_WORK(report_polling_work, em3071_als_work);
static DEVICE_ATTR(suspend, S_IRUGO | S_IWUSR, em3071_pls_show_suspend, em3071_pls_store_suspend);
static DEVICE_ATTR(version, S_IRUGO | S_IWUSR, em3071_pls_show_version, NULL);
static DEVICE_ATTR(ps, S_IRUGO | S_IWUSR, em3071_pls_show_ps, NULL);
static DEVICE_ATTR(als, S_IRUGO | S_IWUSR, em3071_pls_show_als, NULL);


static ssize_t em3071_pls_show_ps(struct device* cd,struct device_attribute *attr, char* buf)
{
	int pdata = 0;
	pdata = i2c_smbus_read_word_data(this_client, EM3071_PLS_REG_PS_DATA);

	return snprintf(buf, PAGE_SIZE, "%x\n", pdata);
}
static ssize_t em3071_pls_show_als(struct device* cd,struct device_attribute *attr, char* buf)
{
	int ldata ,hdata,luxValue;

	ldata = i2c_smbus_read_word_data(this_client, EM3071_PLS_REG_ALS_L_DATA);
	hdata = i2c_smbus_read_word_data(this_client, EM3071_PLS_REG_ALS_H_DATA);
    luxValue = (((hdata&0x0F)<<8)&0xF00)+(ldata&0xFF);

	return snprintf(buf, PAGE_SIZE, "%x\n", luxValue);
}



static ssize_t em3071_pls_show_suspend(struct device* cd,
				     struct device_attribute *attr, char* buf)
{
	ssize_t ret = 0;

	if(suspend_flag==1)
		sprintf(buf, "em3071 Resume\n");
	else
		sprintf(buf, "em3071 Suspend\n");

	ret = strlen(buf) + 1;

	return ret;
}

static ssize_t em3071_pls_store_suspend(struct device* cd, struct device_attribute *attr,
		       const char* buf, size_t len)
{
	unsigned long on_off = simple_strtoul(buf, NULL, 10);
	suspend_flag = on_off;

	if(on_off==1)
	{
		printk("em3071 Entry Resume\n");
		em3071_pls_enable(EM3071_PLS_BOTH);

	}
	else
	{
		printk("em3071 Entry Suspend\n");
		em3071_pls_disable(EM3071_PLS_BOTH);

	}

	return len;
}

static ssize_t em3071_pls_show_version(struct device* cd,
				     struct device_attribute *attr, char* buf)
{
	ssize_t ret = 0;

	sprintf(buf, "em3071");
	ret = strlen(buf) + 1;

	return ret;
}

static int em3071_pls_create_sysfs(struct i2c_client *client)
{
	int err;
	struct device *dev = &(client->dev);

	PLS_DBG("%s", __func__);
	err = device_create_file(dev, &dev_attr_ps);
	err = device_create_file(dev, &dev_attr_als);
	err = device_create_file(dev, &dev_attr_suspend);
	err = device_create_file(dev, &dev_attr_version);

	return err;
}

#define ALS_DELAY			1000
static int em3071_pls_enable(SENSOR_TYPE type)
{
	unsigned char config;
	
	em3071_pls_read_data(EM3071_PLS_REG_CONFIG, &config);
	PLS_DBG("%s: type=%d; config=%x", __func__, type, config);

	wake_lock(&pls_delayed_work_wake_lock);

	PLS_DBG("%s: after type=%d  delay_work=%x\n",__func__, type,this_data->delay_work);

	switch(type) {
		case  EM3071_PLS_ALPS:   //初始化ALS
			cancel_delayed_work(&report_polling_work);
			config |= EM3071_ENABLE_ALS;
			i2c_smbus_write_byte_data(this_client, EM3071_PLS_REG_ALS_THD1, 0x00);
			i2c_smbus_write_byte_data(this_client, EM3071_PLS_REG_ALS_THD2, 0xF0);
			i2c_smbus_write_byte_data(this_client, EM3071_PLS_REG_ALS_THD3, 0xFF);
			i2c_smbus_write_byte_data(this_client, EM3071_PLS_REG_CONFIG, config);
			queue_delayed_work(this_data->em_work_queue, &report_polling_work,msecs_to_jiffies(ALS_DELAY));
			break;
		case EM3071_PLS_PXY:    //初始化PS
			config |= EM3071_ENABLE_PS;
			i2c_smbus_write_byte_data(this_client, EM3071_PLS_REG_LOW_THD, EM3071_PS_L_THD);
			i2c_smbus_write_byte_data(this_client, EM3071_PLS_REG_HIGHT_THD, EM3071_PS_H_THD);
			i2c_smbus_write_byte_data(this_client, EM3071_PLS_REG_CONFIG, config); 
			wake_lock(&pls_delayed_work_wake_lock);
			em3071_pls_report_init();

			break;
		case EM3071_PLS_BOTH:  //初始化ALS+PS			
			i2c_smbus_write_byte_data(this_client, EM3071_PLS_REG_INT_STATUS, 0x00);
			i2c_smbus_write_byte_data(this_client, EM3071_PLS_REG_LOW_THD, EM3071_PS_L_THD);
			i2c_smbus_write_byte_data(this_client, EM3071_PLS_REG_HIGHT_THD, EM3071_PS_H_THD);
			i2c_smbus_write_byte_data(this_client, EM3071_PLS_REG_ALS_THD1, 0x00);
			i2c_smbus_write_byte_data(this_client, EM3071_PLS_REG_ALS_THD2, 0xF0);
			i2c_smbus_write_byte_data(this_client, EM3071_PLS_REG_ALS_THD3, 0xFF);
			i2c_smbus_write_byte_data(this_client, EM3071_PLS_REG_CONFIG, EM3071_ENABLE_BOTH);			
			em3071_pls_report_init();

			break;
		default:
			break;
	}

	return 0;
}

static int em3071_pls_disable(SENSOR_TYPE type)
{
	int pxy_flag, apls_flag;
	unsigned char config;

	em3071_pls_read_data(EM3071_PLS_REG_CONFIG, &config);	
	PLS_DBG("%s: type=%d; config=%x", __func__, type, config);	
	switch(type) {
		case EM3071_PLS_ALPS:
				config &= 0xB8;
				em3071_pls_write_data(EM3071_PLS_REG_CONFIG, config);
				wake_unlock(&pls_delayed_work_wake_lock);
			//cancel_delayed_work_sync(&this_data->delay_work);
			break;
		case EM3071_PLS_PXY:				
				config &= 0x06;
				PLS_DBG("%s: Only Light Sensor alive", __func__);
				em3071_pls_write_data(EM3071_PLS_REG_CONFIG, config);
				wake_unlock(&pls_delayed_work_wake_lock);
			break;
		case EM3071_PLS_BOTH:			
				config = 0X00;			
				PLS_DBG("%s: Disable both sensors", __func__);
				em3071_pls_write_data(EM3071_PLS_REG_CONFIG, config);
				wake_unlock(&pls_delayed_work_wake_lock);
			break;
		default:
			break;
	}

	return 0;
}

static int em3071_pls_open(struct inode *inode, struct file *file)
{
	PLS_DBG("%s\n", __func__);
	if (em3071_pls_opened)
		return -EBUSY;
	em3071_pls_opened = 1;
	return 0;
}

static int em3071_pls_release(struct inode *inode, struct file *file)
{
	PLS_DBG("%s", __func__);
	em3071_pls_opened = 0;
	return em3071_pls_disable(EM3071_PLS_BOTH);
}

static int em3071_read_chip_info(struct i2c_client *client, char *buf)
{
	if(NULL == buf) {
		return -1;
	}
	if(NULL == client) {
		*buf = 0;
		return -2;
	}

	sprintf(buf, "em3071");
	printk("[em3071] em3071_read_chip_info %s\n",buf);
	return 0;
}

static long em3071_pls_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{

    int value;
    int flag;
	int err;
	char strbuf[100];
    unsigned char *buf;
    void __user *argp = (void __user *)arg;

    PLS_DBG("ps io ctrl cmd %d\n", _IOC_NR(cmd));

    //ioctl message handle must define by android sensor library (case by case)
    switch(cmd)
    {
        case LTR_IOCTL_GET_PFLAG:        // _1_

            PLS_DBG("elan Proximity Sensor IOCTL get pflag \n");
            flag = atomic_read(&p_flag);
            if (copy_to_user(argp, &flag, sizeof(flag)))
                return -EFAULT;

            PLS_DBG("elan Proximity Sensor get pflag %d\n",flag);
            break;

        case LTR_IOCTL_SET_PFLAG:         //_3_
            PLS_DBG("elan Proximity IOCTL Sensor set pflag \n");
            if (copy_from_user(&flag, argp, sizeof(flag)))
                return -EFAULT;
			if (flag < 0 || flag > 1) {
				return -EINVAL;
			}
			atomic_set(&p_flag, flag);

			//em_sensor_restart_work();

			if(flag==1){
				em3071_pls_enable(EM3071_PLS_PXY);
			}
			else if(flag==0) {
				em3071_pls_disable(EM3071_PLS_PXY);
			}

            //elan_sensor_restart_work();

            PLS_DBG("elan Proximity Sensor set pflag %d\n",flag);
            break;

		case LTR_IOCTL_GET_LFLAG:            //_2_

            PLS_DBG("elan ambient-light IOCTL Sensor get lflag \n");
			flag = atomic_read(&l_flag);
            if (copy_to_user(argp, &flag, sizeof(flag)))
                return -EFAULT;

            PLS_DBG("elan ambient-light Sensor get lflag %d\n",flag);
            break;

        case LTR_IOCTL_SET_LFLAG:                      //_4_

            PLS_DBG("elan ambient-light IOCTL Sensor set lflag \n");
			if (copy_from_user(&flag, argp, sizeof(flag))) {
				return -EFAULT;
			}
			if (flag < 0 || flag > 1) {
				return -EINVAL;
			}

			atomic_set(&l_flag, flag);
			if(flag==1){
				em3071_pls_enable(EM3071_PLS_ALPS);
			}
			else if(flag==0) {
				em3071_pls_disable(EM3071_PLS_ALPS);
			}

			PLS_DBG("elan ambient-light Sensor set lflag %d\n",flag);

            break;

        case LTR_IOCTL_GET_DATA:                     //_5_
            buf = (unsigned char *)&g_raw_dat;
            if(copy_to_user(argp, &buf , 2))
                return -EFAULT;
            break;

		case LTR_IOCTL_GET_CHIPINFO: // _6_ 开机只会probe一个设备,也只会获得一个chipinfo信息
			err = em3071_read_chip_info(this_client, strbuf);
			if(err < 0)
				return -EFAULT;
			if(copy_to_user(argp, strbuf, strlen(strbuf)+1))
				return -EFAULT;
			break;

        default:
            pr_err("%s: invalid cmd %d\n", __func__, _IOC_NR(cmd));
            return -EINVAL;
    }




	return 0;

}



static struct file_operations em3071_pls_fops = {
	.owner				= THIS_MODULE,
	.open				= em3071_pls_open,
	.release			= em3071_pls_release,
	.unlocked_ioctl		= em3071_pls_ioctl,
};
static struct miscdevice em3071_pls_device = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = EM3071_PLS_DEVICE,
	.fops = &em3071_pls_fops,
};



//-------------- Global functions ---------------//

//ID: device id
void EM3071_show_devid(void)
{
	struct EM3071_data *data = i2c_get_clientdata(this_client);
	int id;
	mutex_lock(&data->update_lock);
	id = i2c_smbus_read_byte_data(this_client, EM3071_PLS_REG_PID);
	mutex_unlock(&data->update_lock);

	printk("[EM3071]++++EM3071_show_devid: %x\n", id);
	return 0;

}
/*******************************************************************************
* Function    : em3071_pls_config_pins
* Description :  alloc gpio irqs
* Parameters  :  void
* Return      :    int irq_num;
*******************************************************************************/
#if 0
static int em3071_pls_config_pins(void)
{
	gpio_request(sprd_3rdparty_gpio_pls_irq,"ALS_PS_INT");//iVIZM

	gpio_direction_input(sprd_3rdparty_gpio_pls_irq);
	em3071_pls_irq=sprd_alloc_gpio_irq(sprd_3rdparty_gpio_pls_irq);

	msleep(10); //wait for stable

	return em3071_pls_irq;
}
#endif

/*******************************************************************************
* Function    :  em3071_pls_early_suspend
* Description :  cancel the delayed work and put ts to shutdown mode
* Parameters  :  handler
* Return      :    none
*******************************************************************************/
static void em3071_pls_early_suspend(struct early_suspend *handler)
{
	PLS_DBG("%s\n", __func__);
#if 0
	em3071_pls_disable(EM3071_PLS_BOTH);
#endif
}

/*******************************************************************************
* Function    :  em3071_pls_early_resume
* Description :  ts re-entry the normal mode and schedule the work, there need to be  a litte time
                      for ts ready
* Parameters  :  handler
* Return      :    none
*******************************************************************************/
static void em3071_pls_early_resume(struct early_suspend *handler)
{
	PLS_DBG("%s\n", __func__);
#if 0
	em3071_pls_enable(EM3071_PLS_ALPS);
//em3071_pls_enable(EM3071_PLS_ALPS);
#endif
}

static void em3071_pls_report_init(void)
{
	EM3071_data *em3071_pls = (EM3071_data *)i2c_get_clientdata(this_client);
	PLS_DBG("%s\n",__func__);
	input_report_abs(em3071_pls->input, ABS_DISTANCE, 0x01); //report far
	input_sync(em3071_pls->input);
}

static void em3071_als_work(struct work_struct *work)
{
	struct EM3071_data *data = container_of((struct delayed_work *)work, struct EM3071_data, work);
	int delay = DEFAULT_DELAY_TIME;
	int ldata, hdata;
	int luxValue=0;

	cancel_delayed_work(&report_polling_work);

	ldata = i2c_smbus_read_word_data(this_client, EM3071_PLS_REG_ALS_L_DATA);
	hdata = i2c_smbus_read_word_data(this_client, EM3071_PLS_REG_ALS_H_DATA);
	printk("cdata = %d, irdata = %d\n", ldata&0xFF, hdata&0xFF);


	luxValue = (((hdata&0x0F)<<8)&0xF00)+(ldata&0xFF);
	g_raw_dat = luxValue;
	printk("luxValue = %d\n", luxValue);

	input_report_abs(this_data_p->input, ABS_MISC, luxValue );
	input_sync(this_data_p->input);
	atomic_set(&data->last, luxValue);
	//schedule_delayed_work(&data->delay_work, msecs_to_jiffies(delay)+1);
	//hjt schedule_work(&data->work);
	queue_delayed_work(this_data->em_work_queue, &report_polling_work,msecs_to_jiffies(ALS_DELAY));
}

static void em3071_pls_work(struct work_struct *work)
{
	unsigned char int_status, enable;
	unsigned short als,ps,als_thre;
	int err;
	EM3071_data* data = container_of(work, EM3071_data, work);   // #define INIT_WORK(_work, _func, _data)，  //_data   

	int pdata;

	printk(KERN_INFO "[em3071] : prox_work_func\n");
	pdata = i2c_smbus_read_word_data(this_client, EM3071_PLS_REG_PS_DATA);
	printk(KERN_INFO "[em3071] : pdata:%d\n",pdata&0xFF);

	i2c_smbus_write_byte_data(this_client, EM3071_PLS_REG_INT_STATUS, 0x00);
	if((pdata&0xFF) > EM3071_PS_H_THD)
	{
			prox_value =0x00; //this_data	
	}
	if((pdata&0xFF) < EM3071_PS_L_THD)
	{
			prox_value =0x05;
	}
	input_report_abs(data->input, ABS_DISTANCE, prox_value);
	input_sync(data->input);
	atomic_set(&data->last, prox_value);

	enable_irq(data->client->irq);

}

/*******************************************************************************
* Function    : em3071_pls_irq_handler
* Description :  handle ts irq
* Parameters  :  handler
* Return      :    none
*******************************************************************************/
static irqreturn_t em3071_pls_irq_handler(int irq, void *dev_id)
{
	EM3071_data *pls = (EM3071_data *)dev_id;

	disable_irq_nosync(pls->client->irq);
	queue_work(pls->em_work_queue,&pls->work);
	return IRQ_HANDLED;
}

struct em3071_pls_platform_data private_dat = {140}; // mod int gpio

static int em3071_pls_probe(struct i2c_client *client, const struct i2c_device_id *id)
{

	int err = 0;
	int ret;
	struct input_dev *input_dev;
//	struct input_dev * input_dev_a;
	EM3071_data *em3071_pls;

	EM3071_data *em3071_als;

	printk("*****init****");
	struct em3071_pls_platform_data *pdata = &private_dat;

    ret = gpio_request(pdata->irq_gpio_number, "ALS_PS_INT");
    if(ret) {
            printk("gpio_request failed!\n");
            //goto exit_gpio_request_failed;
    }
    gpio_direction_input(pdata->irq_gpio_number);
    em3071_pls_irq = gpio_to_irq(pdata->irq_gpio_number);

	wake_lock_init(&pls_delayed_work_wake_lock, WAKE_LOCK_SUSPEND, "prox_delayed_work");

      //判定I2C功能
	if (!i2c_check_functionality(client->adapter, I2C_FUNC_I2C)) {
		printk("%s: functionality check failed\n", __func__);
		err = -ENODEV;
		goto exit_check_functionality_failed;
	}
     //申请内存
	em3071_pls = kzalloc(sizeof(EM3071_data), GFP_KERNEL);
	if (!em3071_pls)
	{
		printk("%s: request memory failed\n", __func__);
		err= -ENOMEM;
		goto exit_request_memory_failed;
	}

	em3071_als = kzalloc(sizeof(EM3071_data), GFP_KERNEL);
	if (!em3071_als)
	{
		printk("%s: request memory failed\n", __func__);
		err= -ENOMEM;
		goto exit_request_memory_failed;
	}

	i2c_set_clientdata(client, em3071_pls);
	em3071_pls->client = client;
	this_client = client; //全局变量

	this_data = em3071_als;
	this_data_p = em3071_pls;


	atomic_set(&em3071_pls->delay, DEFAULT_DELAY_TIME);
	atomic_set(&em3071_als->enable, 0);
	atomic_set(&em3071_als->delay, DEFAULT_DELAY_TIME);

	//printk("I2c addr = 0x%x", client->addr);
	PLS_DBG("I2C addr=0x%x", client->addr);

	ret = i2c_smbus_write_byte_data(client, EM3071_PLS_REG_INT_STATUS, 0x00);
	if (ret < 0) {
        printk("[em3071] : em3071_set_enable Fail");
		goto exit_device_init_failed;
	}
	ret = i2c_smbus_write_byte_data(client, EM3071_PLS_REG_CONFIG, 0x00);
	if (ret < 0) {
        printk("[em3071] : em3071_set_enable Fail");
		goto exit_device_init_failed;
	}
	ret = i2c_smbus_write_byte_data(client, EM3071_PLS_REG_OFFSET, 0X00);
	if (ret < 0) {
        printk("[em3071] : em3071_set_enable Fail");
		goto exit_device_init_failed;
	}
	printk("[em3071] : i2c_smbus_write_byte_data OK");
	//register device //字符杂项注册设备
	err = misc_register(&em3071_pls_device);
	if (err) {
		printk("%s: em3071_pls_device register failed\n", __func__);
		goto exit_device_register_failed;
	}
	// register input device for proximity //注册输入设备
	input_dev = input_allocate_device(); //申请内存并初始化
	if (!input_dev)
	{
		printk("%s: input allocate device failed\n", __func__);
		err = -ENOMEM;
		goto exit_input_dev_allocate_failed;
	}

	em3071_pls->input = input_dev;

	input_dev->name = EM3071_PLS_INPUT_DEV; //和Hal要相同
	input_dev->phys  = EM3071_PLS_INPUT_DEV;
	input_dev->id.bustype = BUS_I2C;
	input_dev->dev.parent = &client->dev;
	input_dev->id.vendor = 0x0001;
	input_dev->id.product = 0x0001;
	input_dev->id.version = 0x0010;

	__set_bit(EV_ABS, input_dev->evbit);
	//for proximity
	input_set_abs_params(input_dev, ABS_DISTANCE, 0, 1, 0, 0);//设定PS范围
	//for lightsensor
	input_set_abs_params(input_dev, ABS_MISC, 0, 0xFFF, 0, 0);//设定ALS范围
	err = input_register_device(input_dev); //注册输入设备到内核
	if (err < 0)
	{
	    printk("%s: input device regist failed\n", __func__);
	    goto exit_input_register_failed;
	}
	//EM3071_show_devid();
	//hjt INIT_WORK(&em3071_als->work, em3071_als_work);
	em3071_als->em_work_queue= create_singlethread_workqueue(EM3071_ALS_DEVICE);	
	//create work queue
	INIT_WORK(&em3071_pls->work,em3071_pls_work);   //INT PS        // #define INIT_WORK(_work, _func, _data)，_data 在em3071_pls_work中
	em3071_pls->em_work_queue= create_singlethread_workqueue(EM3071_PLS_DEVICE);	 

	//register early suspend
	em3071_pls->em_early_suspend.suspend = em3071_pls_early_suspend;
	em3071_pls->em_early_suspend.resume =em3071_pls_early_resume;
	register_early_suspend(&em3071_pls->em_early_suspend);

	//register irq
	client->irq = em3071_pls_irq;
	PLS_DBG("IRQ number is %d", client->irq);

	if(client->irq > 0)
	{
		err =  request_irq(client->irq, em3071_pls_irq_handler, /*IRQF_TRIGGER_FALLING|IRQF_NO_SUSPEND*/IRQF_TRIGGER_LOW, client->name,em3071_pls);
		if (err <0)
		{
			printk("%s: IRQ setup failed %d\n", __func__, err);
			goto irq_request_err;
		}
	}
	//create attribute files
	em3071_pls_create_sysfs(client);

	printk("%s: Probe Success!\n",__func__);

	//em3071_pls_enable(EM3071_PLS_BOTH); //maning add for debug

	return 0;

irq_request_err:
	destroy_workqueue(em3071_pls->em_work_queue); 
	destroy_workqueue(em3071_als->em_work_queue); 
; 
exit_input_register_failed:
	input_free_device(input_dev);
	misc_deregister(&em3071_pls_device);
exit_device_register_failed:
exit_input_dev_allocate_failed:
exit_device_init_failed:
	kfree(em3071_pls);
	kfree(em3071_als);
exit_request_memory_failed:
exit_check_functionality_failed:
	//sprd_free_gpio_irq(em3071_pls_irq);
	gpio_free(irq_to_gpio(em3071_pls_irq));
	wake_lock_destroy(&pls_delayed_work_wake_lock);
	printk("%s: Probe Fail!\n",__func__);
	return err;

}

static int em3071_pls_remove(struct i2c_client *client)
{
	EM3071_data *em3071_pls = i2c_get_clientdata(client);

	printk("%s\n", __func__);

	//remove queue
	
	flush_workqueue(em3071_pls->em_work_queue);
	destroy_workqueue(em3071_pls->em_work_queue); 
	flush_workqueue(this_data->em_work_queue);
	destroy_workqueue(this_data->em_work_queue);
	
	//free suspend
	unregister_early_suspend(&em3071_pls->em_early_suspend);
	misc_deregister(&em3071_pls_device);
	//free input
	input_unregister_device(em3071_pls->input);
	input_free_device(em3071_pls->input);
	//free irq
	free_irq(em3071_pls->client->irq,em3071_pls);
	//sprd_free_gpio_irq(em3071_pls->client->irq);
	 gpio_free(irq_to_gpio(em3071_pls->client->irq));
	//free malloc
	kfree(em3071_pls);

	wake_lock_destroy(&pls_delayed_work_wake_lock);

	return 0;
}

static const struct i2c_device_id em3071_pls_id[] = {
	{ EM3071_PLS_DEVICE, 1 },
	{ }
};

#ifdef I2C_BUS_NUM_STATIC_ALLOC
static struct i2c_board_info  em3071_i2c_boardinfo = {
     I2C_BOARD_INFO(EM3071_PLS_DEVICE, EM3071_PLS_ADDRESS),

};
#endif

#ifdef I2C_BUS_NUM_STATIC_ALLOC
int i2c_static_add_device_em3071(struct i2c_board_info *info)
{
    struct i2c_adapter *adapter;
    struct i2c_client *client;
    int err;

    adapter = i2c_get_adapter(I2C_STATIC_BUS_NUM);
    if (!adapter) {
       pr_err("%s: can't get i2c adapter\n", __FUNCTION__);
       err = -ENODEV;
       goto i2c_err;
    }

    client = i2c_new_device(adapter, info);
    if (!client) {
       pr_err("%s:  can't add i2c device at 0x%x\n",
             __FUNCTION__, (unsigned int)info->addr);
       err = -ENODEV;
       goto i2c_err;
    }

    i2c_put_adapter(adapter);
    return 0;

i2c_err:
   return err;
}
#endif /*I2C_BUS_NUM_STATIC_ALLOC*/
/*----------------------------------------------------------------------------*/

static const struct of_device_id em3071_of_match[] = {
       { .compatible = "EM,em3071_pls", },
        {}
};
MODULE_DEVICE_TABLE(of, em3071_of_match);
static struct i2c_driver em3071_pls_driver = {
    .driver = {
        .owner = THIS_MODULE,
        .name  = EM3071_PLS_DEVICE,
         .of_match_table = em3071_of_match,
    },
	.probe      = em3071_pls_probe,
	.remove     = em3071_pls_remove,
	.id_table = em3071_pls_id,
};
/*----------------------------------------------------------------------------*/

static int __init em3071_pls_init(void)
{
	int ret = 0;
	printk("%s\n", __func__);
	//em3071_pls_config_pins();

#ifdef I2C_BUS_NUM_STATIC_ALLOC
    ret = i2c_static_add_device_em3071(&em3071_i2c_boardinfo);
    if (ret < 0) {
         pr_err("%s: add i2c device error %d\n", __FUNCTION__, ret);
         goto init_err;
    }
#endif

	return i2c_add_driver(&em3071_pls_driver);

#ifdef I2C_BUS_NUM_STATIC_ALLOC

init_err:
	return ret;
#endif
}

static void __exit em3071_pls_exit(void)
{
	printk("%s\n", __func__);
	i2c_del_driver(&em3071_pls_driver);
}



#if 1// em3071
static int em3071_pls_rx_data(char *buf, int len)
{
	uint8_t i;
#if DEBUG_I2C_DATA
	char addr = buf[0];
#endif
	struct i2c_msg msgs[] = {
		{
			.addr	= this_client->addr,
			.flags	= 0,
			.len	= 1,
			.buf	= buf,
		},
		{
			.addr	= this_client->addr,
			.flags	= I2C_M_RD,
			.len	= len,
			.buf	= buf,
		}
	};

	for (i = 0; i < EM3071_PLS_RETRY_COUNT; i++) {
		if (i2c_transfer(this_client->adapter, msgs, 2) >= 0) {
			break;
		}
		mdelay(10);
	}

	if (i >= EM3071_PLS_RETRY_COUNT) {
		return -EIO;
	}


#if DEBUG_I2C_DATA
 	printk("ts %s receive to address[0x%x] data_len[%d] retry_times[%d]: \n", __func__, addr, len,i);
	for(i=0; i<len; i++)
		printk("%x, ",buf[i]);
	printk("\n");
#endif


	return 0;
}

static int em3071_pls_tx_data(char *buf, int len)
{
	uint8_t i;
#if DEBUG_I2C_DATA
	char addr=buf[0];
#endif
	struct i2c_msg msg[] = {
		{
			.addr	= this_client->addr,
			.flags	= 0,
			.len	= len,
			.buf	= buf,
		}
	};

	for (i = 0; i < EM3071_PLS_RETRY_COUNT; i++) {
		if (i2c_transfer(this_client->adapter, msg, 1) >= 0) {
			break;
		}
		mdelay(10);
	}

	if (i >= EM3071_PLS_RETRY_COUNT) {
		return -EIO;
	}

#if DEBUG_I2C_DATA
	printk("ts %s send to address[0x%x] data_len[%d] retry_times[%d]: \n", __func__, addr, len,i);
	for(i=0; i<len-1; i++)
		printk("%x, ",buf[i+1]);
	printk("\n");
#endif


	return 0;
}

/*******************************************************************************
* Function    :  em3071_pls_write_data
* Description :  write data to IC
* Parameters  :  addr: register address, data: register data
* Return      :  none
*******************************************************************************/
static int em3071_pls_write_data(unsigned char addr, unsigned char data)
{
	unsigned char buf[2];
	buf[0]=addr;
	buf[1]=data;
	return em3071_pls_tx_data(buf, 2);
}

/*******************************************************************************
* Function    :  em3071_pls_read_data
* Description :  read data from IC
* Parameters  :  addr: register address, data: read data
* Return      :    status
*******************************************************************************/
static int em3071_pls_read_data(unsigned char addr, unsigned char *data)
{
	int ret;
	unsigned char buf;

	buf=addr;
	ret = em3071_pls_rx_data(&buf, 1);
	*data = buf;

	return ret;;
}
#endif


module_init(em3071_pls_init);
module_exit(em3071_pls_exit);

MODULE_AUTHOR("BINGO_CHEN");
MODULE_DESCRIPTION("Proximity&Light Sensor em3071 DRIVER");
MODULE_LICENSE("GPL");

