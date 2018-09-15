#ifndef __GF_SPI_H
#define __GF_SPI_H

#include <linux/types.h>

/********************GF Mapping**********************/
#define GF_BASE             (0x8000)
#define GF_OFFSET(x)        (GF_BASE + x)

#define GF_VERSION	        GF_OFFSET(0)
#define GF_CONFIG_DATA      GF_OFFSET(0x40)
#define GF_CFG_ADDR	        GF_OFFSET(0x47)
#define GF_MODE_STATUS      GF_OFFSET(0x043)
//#define GF_MIXER_DATA     GF_OFFSET(0x140)
#define GF_BUFFER_STATUS	GF_OFFSET(0x140)
#define GF_KEY_DATA         GF_OFFSET(0x142)
#define GF_NOISE_DATA       GF_OFFSET(0x144)
#define GF_LONG_PRESS_STDP  GF_OFFSET(0x146)
#define GF_BUFFER_DATA      GF_OFFSET(0x141)


#define GF_BUF_STA_MASK     (0x1<<7)
#define	GF_BUF_STA_READY	(0x1<<7)
#define	GF_BUF_STA_BUSY     (0x0<<7)

#define	GF_IMAGE_MASK       (0x1<<6)
#define	GF_IMAGE_ENABLE     (0x1)
#define	GF_IMAGE_DISABLE	(0x0)

#define	GF_KEY_MASK	        (GF_HOME_KEY_MASK | \
                                 GF_MENU_KEY_MASK | \
                                 GF_BACK_KEY_MASK )

//home key
#define	GF_HOME_KEY_MASK	(0x1<<5)
#define	GF_HOME_KEY_ENABL   (0x1)
#define	GF_HOME_KEY_DISABLE (0x0)

#define	GF_HOME_KEY_STA     (0x1<<4)
//menu key
#define	GF_MENU_KEY_MASK    (0x1<<3)
#define	GF_MENU_KEY_ENABLE	(0x1)
#define	GF_MENU_KEY_DISABLE	(0x0)

#define	GF_MENU_KEY_STA	(0x1<<2)
//back key
#define	GF_BACK_KEY_MASK    (0x1<<1)
#define	GF_BACK_KEY_ENABLE  (0x1)
#define	GF_BACK_KEY_DISABLE (0x0)

#define	GF_BACK_KEY_STA     (0x1<<0)


#define	GF_IMAGE_MODE       (0x00)
#define	GF_KEY_MODE	        (0x01)
#define GF_SLEEP_MODE       (0x02)
#define GF_FF_MODE		(0x03)
#define GF_DEBUG_MODE       (0x56)

/**********************GF ops****************************/
#define GF_W                0xF0
#define GF_R                0xF1
#define GF_WDATA_OFFSET         (0x3)
#define GF_RDATA_OFFSET         (0x5)
#define GF_CFG_LEN                  (249)   /*config data length*/

/**********************************************************/

#define GF_FASYNC 		1//If support fasync mechanism.
#define PROCESSOR_64_BIT  1
//#undef GF_FASYNC

/*************************************************************/
struct gf_dev {
	dev_t			devt;
	spinlock_t		spi_lock;
	struct spi_device	*spi;
	struct list_head	device_entry;

	struct input_dev        *input;

	struct workqueue_struct *spi_wq;
	struct work_struct     spi_work;
	struct work_struct	    gf318m_irq_work;
	/* buffer is NULL unless this device is open (users > 0) */
	struct mutex buf_lock;
	unsigned		users;
	u8			*buffer;		
	u8  		buf_status;
	u8 mode;
	struct timer_list   gf_timer;
#ifdef GF_FASYNC
	struct  fasync_struct *async;
#endif
};

/**********************IO Magic**********************/
#define  GF_IOC_MAGIC    'g'  //define magic number
struct gf_ioc_transfer {
	u8	cmd;
	u8 reserve;
	u16	addr;
	u32 len;
#if PROCESSOR_64_BIT
	u32 buf;
#else 
	u8 *buf;
#endif
};
//define commands
/*read/write GF registers*/
#define  GF_IOC_CMD	_IOWR(GF_IOC_MAGIC, 1, struct gf_ioc_transfer)
#define  GF_IOC_REINIT	_IO(GF_IOC_MAGIC, 0)
#define  GF_IOC_SETSPEED	_IOW(GF_IOC_MAGIC, 2, u32)
//#define  GF_IOC_DISABLE    _IO(GF_IOC_MAGIC, 3)
//#define  GF_IOC_ENABLE     _IO(GF_IOC_MAGIC, 4)

#define  GF_IOC_MAXNR    3

/*******************Refering to hardware platform*****************************/
#define 	GF_RST_PIN   	4 // 235
#define 	GF_IRQ_PIN   	6 //234
#define 	GF_CS_PIN   	90 //233
#define 	GF_IRQ_NUM   	gpio_to_irq(GF_IRQ_PIN)
#define		GF_MISO_PIN	92 //237

#if 0
/*Confure the IRQ pin for GF irq if necessary*/
inline static void gf_irq_cfg(void)
{
	/*Config IRQ pin, referring to platform.*/
	gpio_request(GF_IRQ_PIN, "gf_irq");
	gpio_direction_input(GF_IRQ_PIN);
	gpio_free(GF_IRQ_PIN);
}


inline static void gf_miso_pullup(void)
{
	/*Config MISO pin, referring to platform.*/
	gpio_request(GF_MISO_PIN, "gf_miso");
	gpio_direction_input(GF_MISO_PIN);
	gpio_free(GF_MISO_PIN);
}



inline static void gf_miso_backnal(void)
{
	/*Config IRQ pin, referring to platform.*/	
	//s3c_gpio_cfgpin(GF_MISO_PIN, S3C_GPIO_SFN(0x2));
	//s3c_gpio_setpull(GF_MISO_PIN, S3C_GPIO_PULL_NONE);	
}
#endif

/********************************************************************
*CPU output low level in RST pin to reset GF. This is the MUST action for GF.
*Take care of this function. IO Pin driver strength / glitch and so on.
********************************************************************/
#if 0
inline static void gf_hw_reset(void)
{		/*rst pin referring to samsung KIT.*/
	gpio_request(GF_RST_PIN, "gf_rst");
	gpio_direction_output(GF_RST_PIN,1);
//	gpio_request_one(GF_MISO_PIN, GPIOF_OUT_INIT_HIGH, "gf_miso");
//	gpio_request_one(GF_IRQ_PIN, GPIOF_OUT_INIT_LOW, "gf_irq");

	//s3c_gpio_setpull(GF_RST_PIN, S3C_GPIO_PULL_NONE);
	//s5p_gpio_set_drvstr(GF_RST_PIN, S5P_GPIO_DRVSTR_LV4);
//	s3c_gpio_setpull(GF_MISO_PIN, S3C_GPIO_PULL_NONE);
//	s3c_gpio_setpull(GF_IRQ_PIN, S3C_GPIO_PULL_NONE);

//	gpio_direction_output(GF_RST_PIN, 0);
//	gpio_set_value(GF_MISO_PIN, 1);
//	gpio_set_value(GF_RST_PIN, 0);
//	gpio_set_value(GF_IRQ_PIN, 1);
//	mdelay(10);
	
//	gpio_set_value(GF_IRQ_PIN, 0);
	gpio_set_value(GF_RST_PIN, 1);
	mdelay(1);

//	gpio_direction_output(GF_RST_PIN, 0);
//	gpio_set_value(GF_IRQ_PIN, 1);
	gpio_set_value(GF_RST_PIN, 0);
	mdelay(6);
//	gpio_direction_output(GF_MISO_PIN, 1);
//	gpio_direction_output(GF_RST_PIN, 1);
//	gpio_set_value(GF_IRQ_PIN, 0);
	gpio_set_value(GF_RST_PIN, 1);
	mdelay(50);
	
//	gpio_free(GF_IRQ_PIN);
//	gpio_free(GF_RST_PIN);
 //   gf_irq_cfg();

//	s3c_gpio_setpull(GF_MISO_PIN, S3C_GPIO_PULL_NONE);
//	s3c_gpio_cfgpin(GF_MISO_PIN, S3C_GPIO_SFN(2));
//	gpio_free(GF_MISO_PIN);
	
}

#endif

int gf_spi_read_bytes(struct gf_dev *gf_dev,
                                u16 addr, u32 data_len, u8 *rx_buf);

int gf_spi_write_bytes(struct gf_dev *gf_dev,
                                u16 addr, u32 data_len, u8 *tx_buf);
int gf_fw_update(struct gf_dev* gf_dev, unsigned char *buf, unsigned short len);

#endif
