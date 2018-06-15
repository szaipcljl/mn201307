#include <linux/clk.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/i2c.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/of_device.h>
#include <linux/of_gpio.h>
#include <linux/pinctrl/consumer.h>
#include <linux/regulator/consumer.h>
#include <media/v4l2-chip-ident.h>
#include "v4l2-int-device.h"
#include "mxc_v4l2_capture.h"

#include "xc7027_ar0144.h"

/***********************************
** Architecture:
** I2C bus: I2C1
** ADDR:    0x60   0x5d    0x1b      0x20
** i.MX6 -> 914 -> 913 -> xc7027 -> ar0144
***********************************/

#define DS90UB914_ADDR  0x60
#define DS90UB913_ADDR  0x5D  //write it into 914 reg addr 0x07
#define XC7027_ADDR     0x1B
#define AR0144_ADDR     0x10  //not sure

static struct sensor_data rda601_data;
static int ds90ub914_pwn_gpio, isp_pwn_gpio;

//add for xc7027 ROI
static struct mutex xc7027_mutex;

struct roi_rec {
	unsigned int x;  //start x
	unsigned int y;  //start y
	unsigned int w;  //width
	unsigned int h;	 //height
};

#define CMD_GET_REC   0x55
#define CMD_SET_REC   0xAA
#define ISP_ROI_BASE  0x1F00

static struct cdev xc7027_cdev;
static dev_t xc7027_dev_id;
static struct class *xc7027_cls;
struct device *pxc7027_device = NULL;

static int xc7027_status = 0; // 0-off 1-on

/*******************************************
** I2C slave device read and write function
*******************************************/
/* ti_wrtie_reg for 914 913*/
static s32 ti_wrtie_reg(u8 slave_addr, u8 reg, u8 val)
{
	s32 rc = 0;
	u8 au8Buf[2] = {0};
	u16 slave_addr_temp = 0;

	au8Buf[0] = reg;
	au8Buf[1] = val;

	slave_addr_temp = rda601_data.i2c_client->addr;
	rda601_data.i2c_client->addr = slave_addr;

	if (2 != i2c_master_send(rda601_data.i2c_client, au8Buf, 2)) {
		pr_err("%s error: addr=0x%x reg=0x%x,val=0x%x\n", __func__, slave_addr, reg, val);
		rc =  -1;
	}

	rda601_data.i2c_client->addr = slave_addr_temp;
	return rc;
}

/* ti_read_reg for 914 913*/
static s32 ti_read_reg(u8 slave_addr, u8 reg, u8 *val)
{
	s32 rc = 0;
	u8 au8RegBuf[1] = {0};
	u8 u8RdVal = 0;
	u16 slave_addr_temp = 0;

	au8RegBuf[0] = reg;

	slave_addr_temp = rda601_data.i2c_client->addr;
	rda601_data.i2c_client->addr = slave_addr;

	if (1 != i2c_master_send(rda601_data.i2c_client, au8RegBuf, 1)) {
		pr_err("%s error: addr=0x%x reg=0x%x\n", __func__, slave_addr, reg);
		rc = -1;
	}

	if (1 != i2c_master_recv(rda601_data.i2c_client, &u8RdVal, 1)) {
		pr_err("%s error: addr=0x%x reg=0x%x,val=0x%x\n", __func__, slave_addr, reg, u8RdVal);
		rc = -1;
	}

	rda601_data.i2c_client->addr = slave_addr_temp;
	*val = u8RdVal;
	return rc;
}

/* ar_wrtie_reg for ar0144 16bit reg addr and 16bit reg value*/
static s32 ar_wrtie_reg(u8 slave_addr, u16 reg, u16 val)
{
	s32 rc = 0;
	u8 au8Buf[4] = {0};
	u16 slave_addr_temp = 0;

	au8Buf[0] = reg >> 8;
	au8Buf[1] = reg & 0xff;
	au8Buf[2] = val >> 8;
	au8Buf[3] = val & 0xff;

	slave_addr_temp = rda601_data.i2c_client->addr;
	rda601_data.i2c_client->addr = slave_addr;

	if (4 != i2c_master_send(rda601_data.i2c_client, au8Buf, 4)) {
		pr_err("%s error: addr=0x%x reg=0x%x,val=0x%x\n", __func__, slave_addr, reg, val);
		rc =  -1;
	}

	rda601_data.i2c_client->addr = slave_addr_temp;
	return rc;
}

/* ar_read_reg for ar0144*/
static s32 ar_read_reg(u8 slave_addr, u16 reg, u16 *val)
{
	s32 rc = 0;
	u8 au8RegBuf[2] = {0};
	u8 au8RdVal[2] = {0};
	u16 slave_addr_temp = 0;

	au8RegBuf[0] = reg >> 8;
	au8RegBuf[1] = reg & 0xff;

	slave_addr_temp = rda601_data.i2c_client->addr;
	rda601_data.i2c_client->addr = slave_addr;

	if (2 != i2c_master_send(rda601_data.i2c_client, au8RegBuf, 2)) {
		pr_err("%s error: addr=0x%x reg=0x%x\n", __func__, slave_addr, reg);
		rc = -1;
	}

	if (2 != i2c_master_recv(rda601_data.i2c_client, au8RdVal, 2)) {
		pr_err("%s error: addr=0x%x reg=0x%x,val=0x%x\n", __func__, slave_addr, reg, au8RdVal[0] << 8 | au8RdVal[1]);
		rc = -1;
	}

	rda601_data.i2c_client->addr = slave_addr_temp;
	*val = (u16)(au8RdVal[0] << 8 | au8RdVal[1]);
	return rc;
}

static s32 ar_write_regs(struct ar_regval *pRegSetting, s32 ArySize)
{
	s32 i = 0;
	s32 retval = 0;

	for(i=0; i<ArySize; ++i, ++pRegSetting) {
		if( pRegSetting->u16RegAddr == 0xffff && pRegSetting->u16Val != 0) {
			mdelay(pRegSetting->u16Val);
		} else {
			retval = ar_wrtie_reg(AR0144_ADDR, pRegSetting->u16RegAddr, pRegSetting->u16Val);
			if (retval < 0) {
				printk("ar_write_regs error regaddr=0x%x val=0x%x\n", pRegSetting->u16RegAddr, pRegSetting->u16Val);
				return retval;
			}
		}
	}
	return retval;
}

/* xc_write_reg for xc7027 16bit addr 8bit vale */
static s32 xc_write_reg(u16 reg, u8 val)
{
	s32 rc = 0;
	u8 au8Buf[3] = {0};
	u8 i = 0;

	au8Buf[0] = reg >> 8;
	au8Buf[1] = reg & 0xff;
	au8Buf[2] = val;

	if (3 != i2c_master_send(rda601_data.i2c_client, au8Buf, 3)) {
		pr_err("%s error: addr=0x%x reg=0x%x,val=0x%x\n", __func__, rda601_data.i2c_client->addr, reg, val);

		for(i=0; i<3; i++) {
			mdelay(100*(i+5));
			if (3 != i2c_master_send(rda601_data.i2c_client, au8Buf, 3)) {
				pr_err("%s error: addr=0x%x reg=0x%x,val=0x%x\n", __func__, rda601_data.i2c_client->addr, reg, val);
				rc = -1;
			} else {
				pr_err("%s success: addr=0x%x reg=0x%x,val=0x%x\n", __func__, rda601_data.i2c_client->addr, reg, val);
				rc = 0;
				break;
			}
		}
	}
	else
		rc = 0;

	return rc;
}

static s32 xc_write_regs(struct xc_regval *pRegSetting, s32 ArySize)
{
	s32 i = 0;
	s32 retval = 0;

	for(i=0; i<ArySize; ++i, ++pRegSetting) {
		retval = xc_write_reg(pRegSetting->u16RegAddr, pRegSetting->u8Val);
		if (retval < 0) {
			printk("xc_write_regs error, regaddr=0x%x val=0x%x\n", pRegSetting->u16RegAddr, pRegSetting->u8Val);
			return retval;
		}
	}
	return retval;
}

/* xc_read_reg for xc7027 16bit addr 8bit vale */
static s32 xc_read_reg(u16 reg, u8 *val)
{
	u8 au8RegBuf[2] = {0};
	u8 u8RdVal = 0;

	au8RegBuf[0] = reg >> 8;
	au8RegBuf[1] = reg & 0xff;

	if (2 != i2c_master_send(rda601_data.i2c_client, au8RegBuf, 2)) {
		pr_err("%s error: addr=0x%x reg=0x%x\n", __func__, rda601_data.i2c_client->addr, reg);
		return -1;
	}

	if (1 != i2c_master_recv(rda601_data.i2c_client, &u8RdVal, 1)) {
		pr_err("%s error: addr=0x%x reg=0x%x,val=0x%x\n", __func__, rda601_data.i2c_client->addr, reg, u8RdVal);
		return -1;
	}

	*val = u8RdVal;
	return 0;
}

static inline void rda601_reset(void)
{
	//isp and camera and 913 power off
	gpio_set_value(isp_pwn_gpio, 0);
	mdelay(5);
	//isp and camera and 913 power on
	gpio_set_value(isp_pwn_gpio, 1);
	mdelay(20);
}

static int ioctl_dev_init(struct v4l2_int_device *s)
{
	u16 regval = 0;

	//config 914 regs
	ti_wrtie_reg(DS90UB914_ADDR, 0x07,  DS90UB913_ADDR << 1); //913 slave addr
 	ti_wrtie_reg(DS90UB914_ADDR, 0x08,  0x36); //xc7027 slave addr
	ti_wrtie_reg(DS90UB914_ADDR, 0x010, 0x36); //xc7027 slave addr
 	ti_wrtie_reg(DS90UB914_ADDR, 0x09,  0x20); //ar0144 slave addr
	ti_wrtie_reg(DS90UB914_ADDR, 0x011, 0x20); //ar0144 slave addr
	mdelay(1);

	//set xc7027 default values
	xc_write_regs(XC7027_default_regs, ARRAY_SIZE(XC7027_default_regs));

	//xc7027 bypass on for access ar0144
	xc_write_regs(bypass_on_regs, ARRAY_SIZE(bypass_on_regs));

	ar_write_regs(ar0144_default_regs, ARRAY_SIZE(ar0144_default_regs));

	ar_read_reg(AR0144_ADDR, 0x3270, &regval);
	ar_wrtie_reg(AR0144_ADDR, 0x3270, regval | 1 << 8);

	//xc7027 bypass off for access ar0144
	xc_write_regs(bypass_off_regs, ARRAY_SIZE(bypass_off_regs));

	return 0;
}

static int ioctl_dev_exit(struct v4l2_int_device *s)
{
	return 0;
}


static int ioctl_s_power(struct v4l2_int_device *s, int on)
{
	struct sensor_data *sensor = s->priv;

	if (on && !sensor->on) {
		//enable ds90ub914
		gpio_set_value(ds90ub914_pwn_gpio, 1);
		//enable ds90ub913 xc7027 ar0144 power
		gpio_set_value(isp_pwn_gpio, 1);
		//the delay must be 20ms, or read/write 913 reg failed
		mdelay(20);
	} else if (!on && sensor->on) {
		//disable ds90ub913 xc7027 ar0144 power
		gpio_set_value(isp_pwn_gpio, 0);
		//disable ds90ub914
		gpio_set_value(ds90ub914_pwn_gpio, 0);
	}

	sensor->on = on;
	xc7027_status = on;
	return 0;
}

static int ioctl_g_ifparm(struct v4l2_int_device *s, struct v4l2_ifparm *p)
{
	if (s == NULL) {
		pr_err("ERROR!! no slave device set!\n");
		return -1;
	}

	memset(p, 0, sizeof(*p));
	p->u.bt656.clock_curr = rda601_data.mclk;
	p->if_type = V4L2_IF_TYPE_BT656;
	p->u.bt656.mode = V4L2_IF_TYPE_BT656_MODE_NOBT_8BIT;
	p->u.bt656.bt_sync_correct = 1;  /* Indicate external vsync */

	return 0;
}

static int ioctl_init(struct v4l2_int_device *s)
{
	return 0;
}


static int ioctl_enum_fmt_cap(struct v4l2_int_device *s,
			      struct v4l2_fmtdesc *fmt)
{
	fmt->pixelformat = rda601_data.pix.pixelformat;
	return 0;
}

static int ioctl_g_fmt_cap(struct v4l2_int_device *s, struct v4l2_format *f)
{
	struct sensor_data *sensor = s->priv;

	f->fmt.pix = sensor->pix;
	return 0;
}

static int ioctl_g_parm(struct v4l2_int_device *s, struct v4l2_streamparm *a)
{
	struct sensor_data *sensor = s->priv;
	struct v4l2_captureparm *cparm = &a->parm.capture;
	int ret = 0;

	switch (a->type) {
	/* This is the only case currently handled. */
	case V4L2_BUF_TYPE_VIDEO_CAPTURE:
		memset(a, 0, sizeof(*a));
		a->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		cparm->capability = sensor->streamcap.capability;
		cparm->timeperframe = sensor->streamcap.timeperframe;
		cparm->capturemode = sensor->streamcap.capturemode;
		ret = 0;
		break;

	/* These are all the possible cases. */
	case V4L2_BUF_TYPE_VIDEO_OUTPUT:
	case V4L2_BUF_TYPE_VIDEO_OVERLAY:
	case V4L2_BUF_TYPE_VBI_CAPTURE:
	case V4L2_BUF_TYPE_VBI_OUTPUT:
	case V4L2_BUF_TYPE_SLICED_VBI_CAPTURE:
	case V4L2_BUF_TYPE_SLICED_VBI_OUTPUT:
		ret = -EINVAL;
		break;

	default:
		pr_debug("type is unknown - %d\n", a->type);
		ret = -EINVAL;
		break;
	}

	return ret;
}

static int ioctl_s_parm(struct v4l2_int_device *s, struct v4l2_streamparm *a)
{
	int ret = 0;

	switch (a->type) {
	/* This is the only case currently handled. */
	case V4L2_BUF_TYPE_VIDEO_CAPTURE:
		break;

	/* These are all the possible cases. */
	case V4L2_BUF_TYPE_VIDEO_OUTPUT:
	case V4L2_BUF_TYPE_VIDEO_OVERLAY:
	case V4L2_BUF_TYPE_VBI_CAPTURE:
	case V4L2_BUF_TYPE_VBI_OUTPUT:
	case V4L2_BUF_TYPE_SLICED_VBI_CAPTURE:
	case V4L2_BUF_TYPE_SLICED_VBI_OUTPUT:
		pr_debug("type is not " \
			"V4L2_BUF_TYPE_VIDEO_CAPTURE but %d\n",
			a->type);
		ret = -EINVAL;
		break;

	default:
		pr_debug("type is unknown - %d\n", a->type);
		ret = -EINVAL;
		break;
	}

	return ret;
}

static int ioctl_g_ctrl(struct v4l2_int_device *s, struct v4l2_control *vc)
{
	return 0;
}

static int ioctl_s_ctrl(struct v4l2_int_device *s, struct v4l2_control *vc)
{
	int retval = 0;

	switch (vc->id) {
	case V4L2_CID_BRIGHTNESS:
		break;
	case V4L2_CID_CONTRAST:
		break;
	case V4L2_CID_SATURATION:
		break;
	case V4L2_CID_HUE:
		break;
	case V4L2_CID_AUTO_WHITE_BALANCE:
		break;
	case V4L2_CID_DO_WHITE_BALANCE:
		break;
	case V4L2_CID_RED_BALANCE:
		break;
	case V4L2_CID_BLUE_BALANCE:
		break;
	case V4L2_CID_GAMMA:
		break;
	case V4L2_CID_EXPOSURE:
		break;
	case V4L2_CID_AUTOGAIN:
		break;
	case V4L2_CID_GAIN:
		break;
	case V4L2_CID_HFLIP:
		break;
	case V4L2_CID_VFLIP:
		break;
	default:
		retval = -EPERM;
		break;
	}

	return retval;
}

static int ioctl_enum_framesizes(struct v4l2_int_device *s,
				 struct v4l2_frmsizeenum *fsize)
{
	if (fsize->index >= 1)
		return -EINVAL;

	fsize->pixel_format = rda601_data.pix.pixelformat;
	fsize->discrete.width = 1280;
	fsize->discrete.height = 720;
	return 0;
}

static int ioctl_enum_frameintervals(struct v4l2_int_device *s,
					 struct v4l2_frmivalenum *fival)
{
	return 0;
}

static int ioctl_g_chip_ident(struct v4l2_int_device *s, int *id)
{
	((struct v4l2_dbg_chip_ident *)id)->match.type = V4L2_CHIP_MATCH_I2C_DRIVER;
	strcpy(((struct v4l2_dbg_chip_ident *)id)->match.name, "rda601_camera");

	return 0;
}

static struct v4l2_int_ioctl_desc rda601_ioctl_desc[] = {
	{ vidioc_int_dev_init_num,
	  (v4l2_int_ioctl_func *)ioctl_dev_init },
	{ vidioc_int_dev_exit_num,
	  ioctl_dev_exit},
	{ vidioc_int_s_power_num,
	  (v4l2_int_ioctl_func *)ioctl_s_power },
	{ vidioc_int_g_ifparm_num,
	  (v4l2_int_ioctl_func *)ioctl_g_ifparm },
	{ vidioc_int_init_num,
	  (v4l2_int_ioctl_func *)ioctl_init },
	{ vidioc_int_enum_fmt_cap_num,
	  (v4l2_int_ioctl_func *)ioctl_enum_fmt_cap },
	{ vidioc_int_g_fmt_cap_num,
	  (v4l2_int_ioctl_func *)ioctl_g_fmt_cap },
	{ vidioc_int_g_parm_num,
	  (v4l2_int_ioctl_func *)ioctl_g_parm },
	{ vidioc_int_s_parm_num,
	  (v4l2_int_ioctl_func *)ioctl_s_parm },
	{ vidioc_int_g_ctrl_num,
	  (v4l2_int_ioctl_func *)ioctl_g_ctrl },
	{ vidioc_int_s_ctrl_num,
	  (v4l2_int_ioctl_func *)ioctl_s_ctrl },
	{ vidioc_int_enum_framesizes_num,
	  (v4l2_int_ioctl_func *)ioctl_enum_framesizes },
	{ vidioc_int_enum_frameintervals_num,
	  (v4l2_int_ioctl_func *)ioctl_enum_frameintervals },
	{ vidioc_int_g_chip_ident_num,
	  (v4l2_int_ioctl_func *)ioctl_g_chip_ident },
};

static struct v4l2_int_slave rda601_slave = {
	.ioctls = rda601_ioctl_desc,
	.num_ioctls = ARRAY_SIZE(rda601_ioctl_desc),
};

static struct v4l2_int_device rda601_int_device = {
	.module = THIS_MODULE,
	.name = "rda601_camera",
	.type = v4l2_int_type_slave,
	.u = {
		.slave = &rda601_slave,
	},
};

//add for xc7027 ROI
static int xc7027_open(struct inode *inode, struct file *file)
{
	return nonseekable_open(inode, file);
}

static int xc7027_get_roi_rec(struct roi_rec *p)
{
	u8 reg_val[8] = {0};
	u8 i = 0;
	int ret = 0;

	//set xc7027 reg pages
	xc_write_reg(0xfffd, 0x80);
	xc_write_reg(0xfffe, 0x30);

	for(i=0; i< 8; i++) {
		ret = xc_read_reg(ISP_ROI_BASE + i, &reg_val[i]);
		if(ret < 0) {
			printk("CMD_GET_REC 0x%04x failed\n", ISP_ROI_BASE+i);
			return ret;
		}
	}

	p->x = reg_val[0]<<8 | reg_val[1];
	p->y = reg_val[2]<<8 | reg_val[3];
	p->w = reg_val[4]<<8 | reg_val[5];
	p->h = reg_val[6]<<8 | reg_val[7];

	return 0;
}

static int xc7027_set_roi_rec(struct roi_rec xc7027_roi_rec)
{
	int ret = 0;
	u8  reg_val[8] = {0};
	u8  i = 0;

	if(    xc7027_roi_rec.x > 1280 || xc7027_roi_rec.y > 720  \
		|| xc7027_roi_rec.w > 1280 || xc7027_roi_rec.h > 720  \
		|| (xc7027_roi_rec.x + xc7027_roi_rec.w) > 1280       \
		|| (xc7027_roi_rec.y + xc7027_roi_rec.h) > 720) {
		printk("CMD_SET_REC parameters are invalid, (%d,%d) -- (%d,%d)\n", \
		xc7027_roi_rec.x, xc7027_roi_rec.y, 	\
		xc7027_roi_rec.x + xc7027_roi_rec.w, 	\
		xc7027_roi_rec.y + xc7027_roi_rec.h);

		return -1;
	}

	reg_val[0] = xc7027_roi_rec.x >> 8;
	reg_val[1] = xc7027_roi_rec.x & 0xFF;
	reg_val[2] = xc7027_roi_rec.y >> 8;
	reg_val[3] = xc7027_roi_rec.y & 0xFF;
	reg_val[4] = xc7027_roi_rec.w >> 8;
	reg_val[5] = xc7027_roi_rec.w & 0xFF;
	reg_val[6] = xc7027_roi_rec.h >> 8;
	reg_val[7] = xc7027_roi_rec.h & 0xFF;

	//set xc7027 reg pages
	xc_write_reg(0xfffd, 0x80);
	xc_write_reg(0xfffe, 0x30);

	for(i=0; i< 8; i++) {
		ret = xc_write_reg(ISP_ROI_BASE + i, reg_val[i]);
		if(ret < 0) {
			printk("CMD_SET_REC 0x%04x failed\n", ISP_ROI_BASE+i);
			return ret;
		}
	}

	return 0;
}

static long xc7027_unlocked_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	int ret = 0;
	struct roi_rec xc7027_roi_rec;

	if(!xc7027_status) { //xc7027 off
		printk("ISP xc7027 is power off\n");
		return -1;
	}

	switch(cmd) {
		case CMD_GET_REC:
			memset(&xc7027_roi_rec, 0, sizeof(struct roi_rec));

			mutex_lock(&xc7027_mutex);
			ret = xc7027_get_roi_rec(&xc7027_roi_rec);
			mutex_unlock(&xc7027_mutex);
			if(ret < 0) {
				printk("xc7027_get_roi_rec failed\n");
				return ret;
			}

			if (copy_to_user((struct roi_rec *)arg, &xc7027_roi_rec, sizeof(struct roi_rec)))
				return -EFAULT;
		break;

		case CMD_SET_REC:
			if (copy_from_user(&xc7027_roi_rec, (struct roi_rec *)arg, sizeof(struct roi_rec)))
				return -EFAULT;

			mutex_lock(&xc7027_mutex);
			ret = xc7027_set_roi_rec(xc7027_roi_rec);
			mutex_unlock(&xc7027_mutex);
			if(ret < 0) {
				printk("xc7027_set_roi_rec failed\n");
				return ret;
			}
		break;

		default:
			printk("xc7027_roi_rec invalid case parameters\n");
			return -1;
		break;
	}

	return ret;
}

static struct file_operations xc7027_fops = {
	.owner = THIS_MODULE,
	.open  = xc7027_open,
	.unlocked_ioctl = xc7027_unlocked_ioctl,
};

//add for xc7027 roi rec sysfs get/set
static ssize_t xc7027_roi_rec_show(struct device *dev,
            struct device_attribute *attr, char *buf)
{
	int ret = 0;
    struct roi_rec xc7027_roi_rec;

	if(!xc7027_status) { //xc7027 off
		printk("ISP xc7027 is power off\n");
		return -1;
	}

	memset(&xc7027_roi_rec, 0, sizeof(struct roi_rec));

	mutex_lock(&xc7027_mutex);
	ret = xc7027_get_roi_rec(&xc7027_roi_rec);
	mutex_unlock(&xc7027_mutex);
	if(ret < 0) {
		printk("xc7027_get_roi_rec failed\n");
		return ret;
	}

    return sprintf(buf, "x=%d y=%d w=%d h=%d\n", xc7027_roi_rec.x ,xc7027_roi_rec.y ,xc7027_roi_rec.w ,xc7027_roi_rec.h);
}

static int xc7027_roi_parse_buf(const char *buf,  struct roi_rec *rec)
{
	u8 i = 0, j = 0, k = 0;
	u8 buf_len = 0;
	unsigned int data[8] = {0};

	buf_len = strlen(buf);

	if(buf_len < 8) {
		printk("The input parameters are invalid\n");
		printk("Please input as 0 0 320 180\n");
		return -1;
	}

	for(i=0; i<buf_len-1; i++) {
		if(buf[i] == ' ')
			continue;

		if(buf[i] < 0x30 || buf[i] > 0x39) {
			printk("The Char of input parameters are invalid\n");
			return -2;
		}

		for(j=0; j<8; j++) {
			data[k] = data[k] * 10 + buf[i] - 0x30;
			i++;
			if(buf[i] == ' ' || i == buf_len-1) { //at the end of buf
				k++;
				break;
			}
		}
	}

	if(k != 4) {
		printk("The Number of input parameters are invalid\n");
		return -3;
	}

	rec->x = data[0];
	rec->y = data[1];
	rec->w = data[2];
	rec->h = data[3];

	return 0;
}

static ssize_t xc7027_roi_rec_set(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
	int ret = 0;
	struct roi_rec xc7027_roi_rec;

	if(!xc7027_status) { //xc7027 off
		printk("ISP xc7027 is power off\n");
		return -1;
	}

	ret = xc7027_roi_parse_buf(buf ,&xc7027_roi_rec);
	if(ret < 0)
		return ret;

	mutex_lock(&xc7027_mutex);
	ret = xc7027_set_roi_rec(xc7027_roi_rec);
	mutex_unlock(&xc7027_mutex);
	if(ret < 0) {
		printk("xc7027_set_roi_rec failed\n");
		return ret;
	}

	return count;
}

static DEVICE_ATTR(roi, S_IRUGO | S_IWUSR, xc7027_roi_rec_show, xc7027_roi_rec_set);

static s32 xc7027_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	struct pinctrl *pinctrl;
	struct device *dev = &client->dev;
	s32 retval;

	/* xc7027 pinctrl */
	pinctrl = devm_pinctrl_get_select_default(dev);
	if (IS_ERR(pinctrl)) {
		dev_err(dev, "setup pinctrl failed\n");
		return PTR_ERR(pinctrl);
	}

	/* request ds90ub914 power down pin */
	ds90ub914_pwn_gpio = of_get_named_gpio(dev->of_node, "ds90ub914-pwn-gpios", 0);
	if (!gpio_is_valid(ds90ub914_pwn_gpio)) {
		dev_err(dev, "no ds90ub914 pwdn pin available\n");
		return -ENODEV;
	}

	retval = devm_gpio_request_one(dev, ds90ub914_pwn_gpio, GPIOF_OUT_INIT_LOW, "ds90ub914_pwdn");
	if (retval < 0) {
		dev_err(dev, "gpio request ds90ub914_pwdn failed\n");
		return retval;
	}

	/* request xc7027 and ar0144 power enable pin */
	isp_pwn_gpio = of_get_named_gpio(dev->of_node, "isp-pwn-gpios", 0);
	if (!gpio_is_valid(isp_pwn_gpio)) {
		dev_err(dev, "no isp and camera pwdn pin available\n");
		return -ENODEV;
	}

	retval = devm_gpio_request_one(dev, isp_pwn_gpio, GPIOF_OUT_INIT_LOW, "isp_pwdn");
	if (retval < 0) {
		dev_err(dev, "gpio request isp_pwdn failed\n");
		return retval;
	}

	memset(&rda601_data, 0, sizeof(rda601_data));
	rda601_data.i2c_client = client;

	retval = of_property_read_u32(dev->of_node, "mclk",&rda601_data.mclk);
	if (retval < 0) {
		dev_err(dev, "mclk frequency is invalid\n");
		return retval;
	}

	retval = of_property_read_u32(dev->of_node, "mclk_source", (u32 *) &(rda601_data.mclk_source));
	if (retval < 0) {
		dev_err(dev, "mclk_source invalid\n");
		return retval;
	}

	retval = of_property_read_u32(dev->of_node, "csi_id", &(rda601_data.csi));
	if (retval < 0) {
		dev_err(dev, "csi_id invalid\n");
		return retval;
	}

	rda601_data.io_init = rda601_reset;
	rda601_data.i2c_client = client;
	rda601_data.pix.pixelformat = V4L2_PIX_FMT_GREY; //V4L2_PIX_FMT_YUYV
	rda601_data.pix.width = 1280;
	rda601_data.pix.height = 720;
	rda601_data.streamcap.capability = V4L2_MODE_HIGHQUALITY | V4L2_CAP_TIMEPERFRAME;
	rda601_data.streamcap.capturemode = 0;
	rda601_data.streamcap.timeperframe.denominator = 20;
	rda601_data.streamcap.timeperframe.numerator = 1;

	rda601_int_device.priv = &rda601_data;
	retval = v4l2_int_device_register(&rda601_int_device);
	if (retval < 0) {
		dev_err(dev, "v4l2 int device register failed\n");
		return retval;
	}

	//add for xc7027 ROI
	alloc_chrdev_region(&xc7027_dev_id, 0, 1, "xc7027_roi");
	cdev_init(&xc7027_cdev, &xc7027_fops);
	cdev_add(&xc7027_cdev, xc7027_dev_id, 1);
	xc7027_cls = class_create(THIS_MODULE, "xc7027_roi");
	pxc7027_device = device_create(xc7027_cls, NULL, xc7027_dev_id, NULL, "xc7027_roi");

	//add for xc7027 roi rec sysfs get/set
	retval = device_create_file(pxc7027_device, &dev_attr_roi);
	if(retval < 0) {
		dev_err(dev, "device create file failed\n");
		return retval;
	}

	mutex_init(&xc7027_mutex);

	printk("rda601 camera module is found\n");

	return retval;
}

static int xc7027_remove(struct i2c_client *client)
{
	v4l2_int_device_unregister(&rda601_int_device);

	device_remove_file(pxc7027_device, &dev_attr_roi);
	cdev_del(&xc7027_cdev);
	unregister_chrdev_region(xc7027_dev_id, 1);
	device_destroy(xc7027_cls, xc7027_dev_id);
	class_destroy(xc7027_cls);
	return 0;
}

static const struct i2c_device_id xc7027_id[] = {
	{ "xc7027", 0 },
	{ }
};
MODULE_DEVICE_TABLE(i2c, xc7027_id);

static const struct of_device_id xc7027_dt_ids[] = {
	{ .compatible = "xchip,xc7027" },
	{ }
};
MODULE_DEVICE_TABLE(of, xc7027_dt_ids);

static struct i2c_driver xc7027_driver = {
	.driver = {
		.name = "xc7027",
		.of_match_table	= xc7027_dt_ids,
	},
	.id_table = xc7027_id,
	.probe	  = xc7027_probe,
	.remove   = xc7027_remove,
};

module_i2c_driver(xc7027_driver);

MODULE_AUTHOR("Freescale Semiconductor, Inc.");
MODULE_DESCRIPTION("xc7027 driver");
MODULE_LICENSE("GPL");
