/*
 * Filename : ds90ub954.c
 *
 * History:
 *    2018/12/03 - [Ning Ma] Create
 *
 * Copyright (c) 2018 Roadefend, Inc.
 *
 * This file and its contents ("Software") are protected by intellectual
 * property rights including, without limitation, U.S. and/or foreign
 * copyrights. This Software is also the confidential and proprietary
 * information of Ambarella, Inc. and its licensors. You may not use, reproduce,
 * disclose, distribute, modify, or otherwise prepare derivative works of this
 * Software or any portion thereof except pursuant to a signed license agreement
 * or nondisclosure agreement with Ambarella, Inc. or its authorized affiliates.
 * In the absence of such an agreement, you agree to promptly notify and return
 * this Software to Ambarella, Inc.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 * MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include <linux/module.h>
#include <linux/ambpriv_device.h>
#include <linux/interrupt.h>
#include <linux/of_gpio.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/i2c.h>
#include <plat/iav_helper.h>
#include <plat/spi.h>
#include <iav_utils.h>
#include <vin_api.h>
#include "ds90ub954.h"

static int id = 0x1;
module_param(id, int, 0644);
MODULE_PARM_DESC(id, "channel ID 0x01~0x0F, bit map");

static int interlace = 1;
module_param(interlace, int, 0644);
MODULE_PARM_DESC(interlace, "line interleaving mode");

static int vinc_id = 0x10;
module_param(vinc_id, int, 0644);
MODULE_PARM_DESC(vinc_id, "indicate which vin controller is bind to");
/**********************************************************************/
static struct ds90ub954_id_t id_table[] = {
	/* DES */
	{DES_UB954A_ID, "ds90ub954a"},
	{DES_UB954B_ID, "ds90ub954b"},
	/* SER */
	{SER_UB953A_ID, "ds90ub953a"},
	{SER_UB953B_ID, "ds90ub953b"},
	{SER_UB913A_ID, "ds90ub913a"},
	{SER_UB913B_ID, "ds90ub913b"},
	/* TODO: add serializer id here */
};

static u32 ds90ub954_addr_table[DS90UB954_MAX_NUM] = {
	IDC_ADDR_DS90UB954_0,
	IDC_ADDR_DS90UB954_1
};

static u32 ds90ub954_ser_addr_table[DS90UB954_MAX_NUM][DS90UB954_MAX_CHAN_NUM] = {
	{IDC_ADDR_SER0_LINKA, IDC_ADDR_SER0_LINKB},
	{IDC_ADDR_SER1_LINKA, IDC_ADDR_SER1_LINKB}
};

struct ds90ub954_priv {
	struct ds90ub954_i2c_ctrl i2c_ctrl;
	bool fsync;
};

/**********************************************************************/
static int ds90ub954_write_reg(struct vin_brg_device *ambrg, u32 slave_addr, u32 subaddr, u32 data)
{
	struct ds90ub954_priv *pinfo = (struct ds90ub954_priv *)ambrg->priv;
	struct i2c_msg msgs[1];
	u8 pbuf[3];
	int rval = 0;

	brg_debug("w [0x%x]0x%x, 0x%x\n", slave_addr, subaddr, data);

	pbuf[0] = (subaddr & 0xff00) >> 8;
	pbuf[1] = subaddr & 0xff;
	pbuf[2] = data;

	msgs[0].len = 3;
	msgs[0].flags = 0;
	msgs[0].addr = slave_addr >> 1;
	msgs[0].buf = pbuf;

	rval = i2c_transfer(pinfo->i2c_ctrl.adapter, msgs, 1);
	if (rval < 0) {
		brg_error("failed(%d): [0x%x:0x%x]\n", rval, subaddr, data);
		return rval;
	}

	return 0;
}

static int ds90ub954_read_reg(struct vin_brg_device *ambrg, u32 slave_addr, u32 subaddr, u32 *data)
{
	struct ds90ub954_priv *pinfo = (struct ds90ub954_priv *)ambrg->priv;
	struct i2c_msg msgs[2];
	u8 buf0[2], buf1;
	int rval = 0;

	brg_debug("r [0x%x]0x%x\n", slave_addr, subaddr);

	buf0[0] = (subaddr & 0xff00) >> 8;
	buf0[1] = subaddr & 0xff;

	msgs[0].addr = slave_addr >> 1;
	msgs[0].flags = 0;
	msgs[0].buf = buf0;
	msgs[0].len = 2;

	msgs[1].addr = slave_addr >> 1;
	msgs[1].flags = I2C_M_RD;
	msgs[1].buf = &buf1;
	msgs[1].len = 1;

	rval = i2c_transfer(pinfo->i2c_ctrl.adapter, msgs, 2);
	if (rval < 0) {
		vin_error("failed(%d): [0x%x]\n", rval, subaddr);
		return rval;
	}

	*data = buf1;

	return 0;
}

static int ds90ub954_write_reg_8(struct vin_brg_device *ambrg, u32 slave_addr, u32 subaddr, u32 data)
{
	struct ds90ub954_priv *pinfo = (struct ds90ub954_priv *)ambrg->priv;
	struct i2c_msg msgs[1];
	u8 pbuf[3];
	int rval = 0;

	brg_debug("w8 [0x%x]0x%x, 0x%x\n", slave_addr, subaddr, data);

	pbuf[0] = subaddr & 0xff;
	pbuf[1] = data;

	msgs[0].len = 2;
	msgs[0].flags = 0;
	msgs[0].addr = slave_addr >> 1;
	msgs[0].buf = pbuf;

	rval = i2c_transfer(pinfo->i2c_ctrl.adapter, msgs, 1);
	if (rval < 0) {
		brg_error("failed(%d): [0x%x:0x%x]\n", rval, subaddr, data);
		return rval;
	}

	return 0;
}

static int ds90ub954_read_reg_8(struct vin_brg_device *ambrg, u32 slave_addr, u32 subaddr, u32 *data)
{
	struct ds90ub954_priv *pinfo = (struct ds90ub954_priv *)ambrg->priv;
	struct i2c_msg msgs[2];
	u8 buf0, buf1;
	int rval = 0;

	brg_debug("r8 [0x%x]0x%x\n", slave_addr, subaddr);

	buf0 = subaddr & 0xff;

	msgs[0].addr = slave_addr >> 1;
	msgs[0].flags = 0;
	msgs[0].buf = &buf0;
	msgs[0].len = 1;

	msgs[1].addr = slave_addr >> 1;
	msgs[1].flags = I2C_M_RD;
	msgs[1].buf = &buf1;
	msgs[1].len = 1;

	rval = i2c_transfer(pinfo->i2c_ctrl.adapter, msgs, 2);
	if (rval < 0) {
		vin_error("failed(%d): [0x%x]\n", rval, subaddr);
		return rval;
	}

	*data = buf1;

	return 0;
}

static int ds90ub954_read_chan_reg(struct vin_brg_device *ambrg, u32 chip_id, u32 addr, u32 *data)
{
	struct ds90ub954_priv *pinfo = (struct ds90ub954_priv *)ambrg->priv;
	struct i2c_msg msgs[2];
	//u8 slave_addr = 0;
	u8 slave_addr = ambrg->sensor_ctrl.dev_addr;
	u8 tx_buf[3], rx_buf[2];
	int i;
	int rval = 0;

	*data = 0;

	/* we can only read one channel */
	if (chip_id != 0x1 && chip_id != 0x2)
		return -EPERM;

	for (i = 0; i < DS90UB954_MAX_CHAN_NUM; i++) {
		if (chip_id & (1 << i)) {
			break;
		}
	}

	/* Fill the data buffer */
	for (i = 0; i < ambrg->sensor_ctrl.reg_addr_w_byte; i++)
		tx_buf[i] = addr >> ((ambrg->sensor_ctrl.reg_addr_w_byte - i - 1) << 3);

	msgs[0].addr = slave_addr >> 1;
	msgs[0].flags = 0;
	msgs[0].buf = tx_buf;
	msgs[0].len = ambrg->sensor_ctrl.reg_addr_w_byte;

	msgs[1].addr = slave_addr >> 1;
	msgs[1].flags = I2C_M_RD;
	msgs[1].buf = rx_buf;
	msgs[1].len = ambrg->sensor_ctrl.reg_data_w_byte;

	rval = i2c_transfer(pinfo->i2c_ctrl.adapter, msgs, 2);
	if (rval < 0) {
		vin_error("failed(%d): [0x%x]\n", rval, addr);
		return rval;
	}

	for (i = 0; i < ambrg->sensor_ctrl.reg_data_w_byte; i++)
		*data |= rx_buf[i] << ((ambrg->sensor_ctrl.reg_data_w_byte - i  - 1 ) << 3);

	brg_debug("sen r [0x%x]0x%x, data:0x%x\n", slave_addr, addr, *data);

	return 0;
}

static int ds90ub954_write_chan_reg(struct vin_brg_device *ambrg, u32 chip_id, u32 addr, u32 data, u32 size)
{
	struct ds90ub954_priv *pinfo = (struct ds90ub954_priv *)ambrg->priv;
	struct i2c_msg msgs[1];
	u8 slave_addr = ambrg->sensor_ctrl.dev_addr;
	u8 buf_size;
	u8 tx_buf[5];
	int i, j;
	int rval = 0;


	/* write by channel */
	if ((chip_id != ambrg->ch_id.active_id) || (ambrg->ch_num == 1)) {
		for (i = 0; i < DS90UB954_MAX_CHAN_NUM; i++) {
			if (chip_id & (1 << i)) {
				//slave_addr = IDC_ADDR_SEN_BASE + (((ambrg->id << 1) + (1 << i)) << 1);
				break;
			}
		}
	}

	buf_size = ambrg->sensor_ctrl.reg_addr_w_byte + size * ambrg->sensor_ctrl.reg_data_w_byte;

	/* Fill the data buffer */
	for (i = 0; i < ambrg->sensor_ctrl.reg_addr_w_byte; i++)
		tx_buf[i] = addr >> ((ambrg->sensor_ctrl.reg_addr_w_byte - i - 1) << 3);

	for (j = 0; j < ambrg->sensor_ctrl.reg_data_w_byte * size; j++) {
		if (ambrg->sensor_ctrl.reg_data_w_byte == 1)
			tx_buf[i + j] = data >> (j << 3);
		else
			tx_buf[i + j] = data >> ((ambrg->sensor_ctrl.reg_data_w_byte - j - 1) << 3);
	}

	brg_debug("sen w [0x%x]0x%x, 0x%x\n", slave_addr, addr, data);

	msgs[0].len = buf_size;
	msgs[0].flags = 0;
	msgs[0].addr = slave_addr >> 1;
	msgs[0].buf = tx_buf;

	rval = i2c_transfer(pinfo->i2c_ctrl.adapter, msgs, 1);
	if (rval < 0) {
		brg_error("failed(%d): [0x%x:0x%x]\n", rval, addr, data);
		return rval;
	}

	return rval;
}

static int ds90ub954_set_gpio(struct vin_brg_device *ambrg, u32 chip_id, ds90ub954_gpio_t gpio_id, u8 level)
{
#if 0
	struct ds90ub954_priv *pinfo = (struct ds90ub954_priv *)ambrg->priv;
	ds90ub954_gpio_reg_s gpio_reg = {0};
	u16 chan_addr, reg_addr;

	brg_debug("[0x%d]gpio %d->%d\n", chip_id, gpio_id, level);

	if (chip_id == 0) {
		reg_addr = 0x2b0 + (gpio_id * sizeof(ds90ub954_gpio_reg_s));
		chan_addr = pinfo->i2c_ctrl.addr.des;
	} else if (chip_id == 0x1) {
		reg_addr = 0x2be + (gpio_id * sizeof(ds90ub954_gpio_reg_s));
		chan_addr = pinfo->i2c_ctrl.addr.ser_chan0;
	} else if (chip_id == 0x2) {
		reg_addr = 0x2be + (gpio_id * sizeof(ds90ub954_gpio_reg_s));
		chan_addr = pinfo->i2c_ctrl.addr.ser_chan1;
	} else {
		brg_error("unknow chip id:0x%x\n", chip_id);
		return -EPERM;
	}

	if (gpio_id >= DS90UB954_GPIO_MAX) {
		brg_error("gpio id is out of range:%d\n", gpio_id);
		return -EPERM;
	}

	ds90ub954_read_reg(ambrg, chan_addr, reg_addr, (u32 *)&gpio_reg.gpio_a.data);
	ds90ub954_read_reg(ambrg, chan_addr, reg_addr + 1, (u32 *)&gpio_reg.gpio_b.data);

	gpio_reg.gpio_b.bits.gpio_tx_id = gpio_id;
	gpio_reg.gpio_b.bits.out_type = 1;
	gpio_reg.gpio_b.bits.pull_updn_sel = 0;

	gpio_reg.gpio_a.bits.gpio_out_ids = 0;
	gpio_reg.gpio_a.bits.gpio_tx_en = 0;
	gpio_reg.gpio_a.bits.gpio_rx_en = 0;
	gpio_reg.gpio_a.bits.gpio_out = (!level) ? 0 : 1;
	gpio_reg.gpio_a.bits.tx_comp_en = 0;
	gpio_reg.gpio_a.bits.tx_prio = 0;
	gpio_reg.gpio_a.bits.res_cfg = 1;

	ds90ub954_write_reg(ambrg, chan_addr, reg_addr, gpio_reg.gpio_a.data);
	ds90ub954_write_reg(ambrg, chan_addr, reg_addr + 1, gpio_reg.gpio_b.data);

#endif
	return 0;
}

static int ds90ub954_wait_lock_status(struct vin_brg_device *ambrg, u32 to_ms)
{
	struct ds90ub954_priv *pinfo = (struct ds90ub954_priv *)ambrg->priv;
	unsigned long t_end;
	u32 data;
	int rval = 0;

	/* wait for link locked */
	t_end = jiffies + msecs_to_jiffies(to_ms);
	do {
		if (time_after(jiffies, t_end)) {
			brg_error("Link locked fail!\n");
			rval = -EIO;
			break;
		}
		msleep(1);
		ds90ub954_read_reg_8(ambrg, pinfo->i2c_ctrl.addr.des, UB954_DEVICE_STS_REG, &data);
	} while (!(data & (1 << 2)));

	return rval;
}

static int ds90ub954_hw_init(struct vin_brg_device *ambrg, u32 chip_id)
{
	struct ds90ub954_priv *pinfo = (struct ds90ub954_priv *)ambrg->priv;
	u32 dev_id, addr, data;
	u16 des_addr = pinfo->i2c_ctrl.addr.des;
	int broken_id = 0;
	int i, j;
	int rval;

	brg_debug("%s\n", __func__);

	ds90ub954_write_reg_8(ambrg, des_addr, UB954_FPD3_PORT_SEL_REG, 0x01);
	ds90ub954_write_reg_8(ambrg, des_addr, UB954_RX_PORT_CTL_REG, 0x81);
	ds90ub954_write_reg_8(ambrg, des_addr, UB954_BCC_CONFIG_REG, 0x58);

	/* check DES ID */
	ds90ub954_read_reg_8(ambrg, des_addr, 0x00, &dev_id);

	for (i = 0; i < ARRAY_SIZE(id_table); i++) {
		if (dev_id == id_table[i].id) {
			brg_info("[%d]deserializer %s, id:0x%x\n", ambrg->id, id_table[i].name, dev_id);
			break;
		}
	}

	rval = ds90ub954_wait_lock_status(ambrg, DS90UB954_LINK_LOCK_TO);
	if (rval < 0) {
		brg_error("[%d]No serializer link!\n", ambrg->id);
		return rval;
	}

#if 0
	/* mask other ds90ub954's remote link */
	for (i = 0; i < DS90UB954_MAX_NUM; i++) {
		mask_addr = ds90ub954_addr_table[i];
		/* set DIS_REM_CC for other ds90ub954's remote link */
		ds90ub954_read_reg(ambrg, mask_addr, 0x0001, &data);
		data = (des_addr != mask_addr) ? (data | (1 << 4)) : (data & ~(1 << 4));
		ds90ub954_write_reg(ambrg, mask_addr, 0x0001, data);
	}
#endif

	/*
	 * check SER ID, reset each link
	 * chip_id: 0x1, ser_chan0?
	 * chip_id: 0x2, ser_chan1?
	 */
	for (i = 0; i < DS90UB954_MAX_CHAN_NUM; i++) {
		if (chip_id & (1 << i)) {
			addr = (i == 0) ? pinfo->i2c_ctrl.addr.ser_chan0 : pinfo->i2c_ctrl.addr.ser_chan1;

			/* Reset links */
			brg_debug("[%d:%d]reset link %s\n", ambrg->id, 1 << i, (i == 0) ? "A" : "B");
			//ds90ub954_write_reg(ambrg, des_addr, 0x0010, 0x20 | (i + 1));

			rval = ds90ub954_wait_lock_status(ambrg, DS90UB954_LINK_LOCK_TO);
			if (rval < 0) {
				brg_error("[%d:%d]Link lock timeout!\n", ambrg->id, 1 << i);
				return rval;
			}

			/* remap serializer's address */
			ds90ub954_write_reg_8(ambrg, des_addr, UB954_SER_ALIAS_ID_REG, IDC_ADDR_SER0_LINKA);

			dev_id = 0;
			ds90ub954_read_reg_8(ambrg, des_addr, UB954_SER_ALIAS_ID_REG, &dev_id);
			for (j = 0; j < ARRAY_SIZE(id_table); j++) {
				if (dev_id == id_table[j].id) {
					brg_info("[%d:%d]serializer %s, id:0x%x\n", ambrg->id, 1 << i, id_table[j].name, dev_id);
					break;
				}
			}

			ds90ub954_read_reg_8(ambrg, addr, UB913_GPIO0_1_CONFIG, &rval);
			ds90ub954_write_reg_8(ambrg, addr, UB913_GPIO0_1_CONFIG, rval & 0xF5 );
			printk("#mn-1: addr=0x%x, rval = 0x%x\n", addr, rval & 0xF5);
			msleep(2);
			//ds913 gpio0:default 0 reset; set to high after reset.
			ds90ub954_write_reg_8(ambrg, addr, UB913_GPIO0_1_CONFIG, rval & (~(1 << 2)) | (1<<3 | 1<<0) );
			msleep(10);

			/* sensor i2c address mapping */
			ds90ub954_write_reg_8(ambrg, des_addr, UB954_SLAVE_ID0_REG, ambrg->sensor_ctrl.dev_addr);
			ds90ub954_write_reg_8(ambrg, des_addr, UB954_SLAVE_ALIAS_ID0_REG, ambrg->sensor_ctrl.dev_addr);
		}
	}

	return broken_id;
}

static void ds90ub954_sensor_pwr_rst(struct vin_brg_device *ambrg, u32 chip_id)
{
#if 0
	/* sensor power on */
	ds90ub954_set_gpio(ambrg, chip_id, DS90UB954_GPIO_5, 1);
	/* sensor reset */
	ds90ub954_set_gpio(ambrg, chip_id, DS90UB954_GPIO_4, 0);
	msleep(5);
	ds90ub954_set_gpio(ambrg, chip_id, DS90UB954_GPIO_4, 1);
#endif
}

static void ds90ub954_set_clk_si(struct vin_brg_device *ambrg, u32 chip_id, u32 clk_si)
{
	struct ds90ub954_priv *pinfo = (struct ds90ub954_priv *)ambrg->priv;
	int addr, i;

	brg_debug("%s, clk_si:%d\n", __func__, clk_si);

#if 0 //max9296 for reference
	for (i = 0; i <DS90UB954_MAX_CHAN_NUM; i++) {
		if (chip_id & (1 << i)) {
			addr = (i == 0) ? pinfo->i2c_ctrl.addr.ser_chan0 : pinfo->i2c_ctrl.addr.ser_chan1;
			switch(clk_si) {
			case 24000000:
				/* pre-defined clock setting is disabled */
				ds90ub954_write_reg(ambrg, addr, 0x03F1, 0x05); /* MFP2 to output RCLKOUT */
				ds90ub954_write_reg(ambrg, addr, 0x03F0, 0x12);
				ds90ub954_write_reg(ambrg, addr, 0x03F4, 0x0A);
				ds90ub954_write_reg(ambrg, addr, 0x03F5, 0x07);
				ds90ub954_write_reg(ambrg, addr, 0x03F0, 0x10);
				ds90ub954_write_reg(ambrg, addr, 0x1A03, 0x12);
				ds90ub954_write_reg(ambrg, addr, 0x1A07, 0x04);
				ds90ub954_write_reg(ambrg, addr, 0x1A08, 0x3D);
				ds90ub954_write_reg(ambrg, addr, 0x1A09, 0x40);
				ds90ub954_write_reg(ambrg, addr, 0x1A0A, 0xC0);
				ds90ub954_write_reg(ambrg, addr, 0x1A0B, 0x7F);
				ds90ub954_write_reg(ambrg, addr, 0x03F0, 0x11);
				break;
			case 27000000:
				/* pre-defined clock setting is enabled */
				ds90ub954_write_reg(ambrg, addr, 0x0003, 0x07); /* MFP2 to output RCLKOUT */
				ds90ub954_write_reg(ambrg, addr, 0x0006, 0xbf);
				ds90ub954_write_reg(ambrg, addr, 0x03f0, 0x53);
				ds90ub954_write_reg(ambrg, addr, 0x03f0, 0x51);
				break;
			case 37125000:
				/* pre-defined clock setting is enabled */
				ds90ub954_write_reg(ambrg, addr, 0x0003, 0x07); /* MFP2 to output RCLKOUT */
				ds90ub954_write_reg(ambrg, addr, 0x0006, 0xbf);
				ds90ub954_write_reg(ambrg, addr, 0x03f0, 0x63);
				ds90ub954_write_reg(ambrg, addr, 0x03f0, 0x61);
				break;
			default:
				brg_error("clock %d hasn't been supported yet\n", clk_si);
				break;
			}

			ds90ub954_sensor_pwr_rst(ambrg, 1 << i);
		}
	}
#endif
}

static int ds90ub954_dbg_write(struct vin_brg_device *ambrg, u32 chip_id, u32 subaddr, u32 data)
{
	u8 chan_id;

	if (subaddr == 0xFFFF) {
		ambrg->ch_id.dbg_id = data;
		switch (data & 0xF0) {
		case 0x10:
			//brg_info("max20087\n");
			break;
		case 0x20:
			brg_info("gpio\n");
			break;
		case 0x30:
			//brg_info("icm20648\n");
			break;
		case 0xF0:
			brg_info("sensor\n");
			break;
		default:
			break;
		}
	} else {
		chan_id = ambrg->ch_id.dbg_id & 0xF;
		switch (ambrg->ch_id.dbg_id & 0xF0) {
		case 0x10:
			//max20087_write_reg(ambrg, IDC_ADDR_MAX20087, subaddr, data);
			break;
		case 0x20:
			ds90ub954_set_gpio(ambrg, chan_id, subaddr, data);
			break;
		case 0x30:
			//icm20648_write_reg(ambrg, 0x30, subaddr, data);
			break;
		case 0xF0:
			ds90ub954_write_chan_reg(ambrg, chan_id, subaddr, data, 1);
			break;
		default:
			ds90ub954_write_reg(ambrg, ambrg->ch_id.dbg_id, subaddr, data);
			break;
		}
	}

	return 0;
}

static int ds90ub954_dbg_read(struct vin_brg_device *ambrg, u32 chip_id, u32 subaddr, u32 *data)
{
	struct ds90ub954_priv *pinfo = (struct ds90ub954_priv *)ambrg->priv;
	u8 chan_id;

	if (subaddr == 0xFFFF) {
		brg_info("ds90ub954 I2C address mapping\n" \
				"DES:0x%x\n" \
				"SER_A:0x%x\n" \
				"SER_B:0x%x\n" ,
				pinfo->i2c_ctrl.addr.des,
				pinfo->i2c_ctrl.addr.ser_chan0,
				pinfo->i2c_ctrl.addr.ser_chan1);
	} else {
		chan_id = ambrg->ch_id.dbg_id & 0xF;
		switch (ambrg->ch_id.dbg_id & 0xF0) {
		case 0x10:
			//max20087_read_reg(ambrg, IDC_ADDR_MAX20087, subaddr, data);
			break;
		case 0x30:
			//icm20648_read_reg(ambrg, 0x30, subaddr, data);
			break;
		case 0xF0:
			ds90ub954_read_chan_reg(ambrg, chan_id, subaddr, data);
			break;
		default:
			ds90ub954_read_reg(ambrg, ambrg->ch_id.dbg_id, subaddr, data);
			break;
		}
	}

	return 0;
}

static void ds90ub954_vin_config(struct vin_brg_device *ambrg, u32 chip_id)
{
	struct ds90ub954_priv *pinfo = (struct ds90ub954_priv *)ambrg->priv;
	u8 des_addr = pinfo->i2c_ctrl.addr.des;
	u8 ser_addr;
	u32 data;

	if (chip_id & 0x1) {
		/*
		 * [LINK A only or in reserve splitter mode]
		 * SER: MIPI Port B -> Pipe X (TX Stream ID=1)
		 * DES: Pipe Y (RX Stream ID=1) -> Controller 1
		 */
		//ser_addr = pinfo->i2c_ctrl.addr.ser_chan0;

		/*  Video Pipes setup */

	}

	if (chip_id & 0x2) {
#if 0
		ser_addr = pinfo->i2c_ctrl.addr.ser_chan1;

		/* Video Pipes setup */
		if (chip_id == 0x2) {
			/*
			 * [LINK B only]
			 * SER: MIPI Port B -> Pipe X (TX Stream ID=1)
			 * DES: Pipe Y (RX Stream ID=1) -> Controller 1
			 */

			/* Video Pipe X from Port B */
			/* Stream ID for packets from Pipe X is 1 */

		} else /* if (chip_id == 0x3) */ {
			/*
			 * [LINK B in reverse splitter mode]
			 * SER: MIPI Port B -> Pipe X (TX Stream ID=2)
			 * DES: Pipe Z (RX Stream ID=2) -> Controller 2
			 */
			/* Video Pipe X from Port B */
			/* Stream ID for packets from Pipe X is 2 */

			/* Concatenation register for sync mode settings (Wx4H mode), set stream 1 as master */
		}

		/* XHS and XVS GPIO forwarding setup */
		/* XVS signal from SoC, DES_MFP5 --> SER_MFP7 */
		/* XHS signal from SoC, DES_MFP0 --> SER_MFP0 */

		/* Serializer MIPI Setup */
		/* Set MIPI_RX registers number of lanes 1x4 mode */
		// select number of data lanes for Port B.
#endif
	}
}

static void ds90ub954_vout_config(struct vin_brg_device *ambrg, u32 chip_id, struct vin_video_format *format)
{
	struct ds90ub954_priv *pinfo = (struct ds90ub954_priv *)ambrg->priv;
	u8 des_addr = pinfo->i2c_ctrl.addr.des;
	u8 data;

	ambrg->vout_info.lane_num = SENSOR_4_LANE;
	ambrg->vout_info.interface_type = SENSOR_MIPI;
	ambrg->vout_info.data_rate = SENSOR_MIPI_BIT_RATE_H;

	/* RX0 VC=0 */
	ds90ub954_write_reg_8(ambrg, des_addr, UB954_FPD3_PORT_SEL_REG, 0x01); //RX0
	ds90ub954_write_reg_8(ambrg, des_addr, UB954_RAW12_ID_REG, 0xEC) ; //Map Sensor A VC0 to CSI-Tx VC0

	/* CSI_EN */
	//CSI_EN & CSI0 4L; Enable CSI continuous clock mode.
	ds90ub954_write_reg_8(ambrg, des_addr, UB954_CSI_CTL_REG, 0x3) ;

	/* Basic_FWD */
	ds90ub954_write_reg_8(ambrg, des_addr, UB954_FWD_CTL2_REG, 0x14) ; //Synchronized Basic_FWD
	/* FWD_PORT all RX to CSI0 */
	ds90ub954_write_reg_8(ambrg, des_addr, UB954_FWD_CTL1_REG, 0x20) ; //forwarding of all RX to CSI0

	/* CSI Transmitter Speed select, default 0x20: 800 Mbps serial rate */
	//Fixme: affect i2c
	//ds90ub954_write_reg(ambrg, des_addr, UB954_CSI_PLL_CTL_REG, 0x02);
}

static struct vin_brg_ops ds90ub954_ops = {
	.brg_write_reg			= ds90ub954_dbg_write,
	.brg_read_reg			= ds90ub954_dbg_read,
	.brg_write_chan_reg	= ds90ub954_write_chan_reg,
	.brg_read_chan_reg	= ds90ub954_read_chan_reg,
	.brg_hw_init_pre		= ds90ub954_hw_init,
#ifdef BRG_TO_SNR_CLK
	.brg_set_clk_si		= ds90ub954_set_clk_si,
#endif
	.brg_vin_config_pre	= ds90ub954_vin_config,
	.brg_vout_config		= ds90ub954_vout_config,
};

/* ========================================================================== */
static int ds90ub954_drv_probe(u8 id, u32 chan_id)
{
	struct ds90ub954_priv *pinfo;
	struct i2c_adapter *adap;
	struct vin_brg_device *ambrg;
	static u8 probe_num = 0;
	u32 intf_id;
	int rval;

	intf_id = (vinc_id >> (probe_num << 2)) & 0xF;

	ambrg = kzalloc(sizeof(struct vin_brg_device) + sizeof(struct ds90ub954_priv), GFP_KERNEL);
	if (!ambrg)
		return -ENOMEM;

	ambrg->sensor_ctrl.ops = kzalloc(sizeof(struct vin_ops), GFP_KERNEL);
	if (!ambrg->sensor_ctrl.ops) {
		brg_error("No memory: sensor ops!\n");
		rval = -ENOMEM;
		goto ds90ub954_drv_probe_exit2;
	}

	ambrg->vin_cfg = kzalloc(sizeof(struct vin_device_config), GFP_KERNEL);
	if (!ambrg->vin_cfg) {
		brg_error("No memory: vin cfg!\n");
		rval = -ENOMEM;
		goto ds90ub954_drv_probe_exit1;
	}

	ambrg->name = "ds90ub954";
	ambrg->id = id;
	ambrg->intf_id = intf_id;
	ambrg->ch_id.load_id = chan_id;
	ambrg->ch_id.active_id = ambrg->ch_id.load_id;
	ambrg->ch_id.broken_id = 0x00;
	ambrg->ch_id.dbg_id = 0x90;
	ambrg->master_clk = DS90UB954_INPUT_CLK;
	ambrg->use_sen_clk = true;
	ambrg->use_vin_crop = true;
	ambrg->vout_info.output_format = interlace;

	pinfo = (struct ds90ub954_priv *)ambrg->priv;

	rval = amba_register_vin_brg(ambrg, &ds90ub954_ops);
	if (rval < 0) {
		rval = -EPERM;
		goto ds90ub954_drv_probe_exit1;
	}

	adap = i2c_get_adapter(ambrg->bus_addr >> 16);
	if (!adap) {
		brg_error("No such i2c controller: %d\n", ambrg->bus_addr >> 16);
		rval = -ENODEV;
		goto ds90ub954_drv_probe_exit1;
	}

	pinfo->i2c_ctrl.adapter = adap;
	pinfo->i2c_ctrl.addr.des  = ds90ub954_addr_table[ambrg->id];
	pinfo->i2c_ctrl.addr.ser_def  = IDC_ADDR_SER_DEFAULT;
	pinfo->i2c_ctrl.addr.ser_chan0  = ds90ub954_ser_addr_table[ambrg->id][0];
	pinfo->i2c_ctrl.addr.ser_chan1  = ds90ub954_ser_addr_table[ambrg->id][1];
	pinfo->fsync = (ambrg->ch_num > 1) ? true : false;

	probe_num++;

	return 0;
ds90ub954_drv_probe_exit1:
	if (ambrg->vin_cfg)
		kfree(ambrg->vin_cfg);
	kfree(ambrg->sensor_ctrl.ops);
ds90ub954_drv_probe_exit2:
	kfree(ambrg);
	return rval;
}

static int __init ds90ub954_init(void)
{
	int rval = 0;
	u32 chan_id;
	int i;

	for (i = 0; i < DS90UB954_MAX_NUM; i++) {
		chan_id = (id >> (i << 3)) & 0xFF;
		if (chan_id) { //chan_id > 0
			rval = ds90ub954_drv_probe(i, chan_id);
			if (rval < 0)
				break;
		}
	}

	return rval;
}

static void __exit ds90ub954_exit(void)
{
	return;
}

module_init(ds90ub954_init);
module_exit(ds90ub954_exit);

MODULE_DESCRIPTION("TI DS90UB954 DUAL GMSL2/GMSL1 SERDES bridge");
MODULE_AUTHOR("Ning Ma, <ning.ma@roadefend.com>");
MODULE_LICENSE("Proprietary");
