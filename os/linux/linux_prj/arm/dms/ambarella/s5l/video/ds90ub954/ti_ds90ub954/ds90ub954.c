/*
 * Filename : ds90ub954.c
 *
 * History:
 *    2018/12/25 - [Long Zhao] Create
 *
 * Copyright (c) 2018 Ambarella, Inc.
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

static int vinc_id = 0x10;
module_param(vinc_id, int, 0644);
MODULE_PARM_DESC(vinc_id, "indicate which vin controller is bind to");

static int mode = DS90UB954_FPD_MODE_AUTO;
module_param(mode, int, 0644);
MODULE_PARM_DESC(mode, "FPD-Linke input mode 0: CSI mode 1: RAW 12 LF 2: RAW 12 HF 3: RAW10 0xFF: From MODE pin");

static bool test = 0;
MODULE_PARM_DESC(test, "test pattern enable");
module_param(test, bool, 0644);
/**********************************************************************/
static u32 ds90ub954_addr_table[DS90UB954_MAX_NUM] = {
	IDC_ADDR_DS90UB954_0,
	IDC_ADDR_DS90UB954_1
};

static ds90ub954_gpio_t ds90ub954_poc_ctrl[DS90UB954_MAX_CHAN_NUM] = {
	DS90UB954_GPIO_1,
	DS90UB954_GPIO_0
};

struct ds90ub954_priv {
	struct ds90ub954_i2c_ctrl i2c_ctrl;
	struct ds90ub954_link_info link_info[DS90UB954_MAX_CHAN_NUM];
	ds90ub954_fpd_mode_t link_mode;
};

static struct vin_reg_8_8 ds90ub954_test_pattern_regs[] = {
	/* 720p YUV422 8bits */
	{0x70, 0x1e},	{0xB0, 0x00},	{0xB1, 0x01},	{0xB2, 0x01},	{0xB1, 0x02},
	{0xB2, 0x83},	{0xB1, 0x03},	{0xB2, 0x1e},	{0xB1, 0x04},	{0xB2, 0x0F},
	{0xB1, 0x05},	{0xB2, 0x00},	{0xB1, 0x06},	{0xB2, 0x01},	{0xB1, 0x07},
	{0xB2, 0xE0},	{0xB1, 0x08},	{0xB2, 0x02},	{0xB1, 0x09},	{0xB2, 0xD0},
	{0xB1, 0x0A},	{0xB2, 0x04},	{0xB1, 0x0B},	{0xB2, 0x1A},	{0xB1, 0x0C},
	{0xB2, 0x0C},	{0xB1, 0x0D},	{0xB2, 0x67},	{0xB1, 0x0E},	{0xB2, 0x21},
	{0xB1, 0x0F},	{0xB2, 0x0A},
};
/**********************************************************************/
static int ds90ub954_write_reg(struct vin_brg_device *ambrg, u32 slave_addr, u32 subaddr, u32 data)
{
	struct ds90ub954_priv *pinfo = (struct ds90ub954_priv *)ambrg->priv;
	struct i2c_msg msgs[1];
	u8 pbuf[3];
	int rval = 0;

	brg_debug("w [0x%x]0x%x, 0x%x\n", slave_addr, subaddr, data);

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

static int ds90ub954_read_reg(struct vin_brg_device *ambrg, u32 slave_addr, u32 subaddr, u32 *data)
{
	struct ds90ub954_priv *pinfo = (struct ds90ub954_priv *)ambrg->priv;
	struct i2c_msg msgs[2];
	u8 buf0, buf1;
	int rval = 0;

	brg_debug("r [0x%x]0x%x\n", slave_addr, subaddr);

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
	u8 slave_addr = 0;
	u8 tx_buf[3], rx_buf[2];
	int i;
	int rval = 0;

	*data = 0;

	/* we can only read one channel */
	if (chip_id != 0x1 && chip_id != 0x2)
		return -EPERM;

	for (i = 0; i < DS90UB954_MAX_CHAN_NUM; i++) {
		if (chip_id & (1 << i)) {
			slave_addr = IDC_ADDR_SEN_BASE + (((ambrg->id << 1) + (1 << i)) << 1);
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
	u8 slave_addr = pinfo->i2c_ctrl.addr.sen_all;
	u8 buf_size;
	u8 tx_buf[5];
	int i, j;
	int rval = 0;

	/* write by channel */
	if (chip_id != ambrg->ch_id.active_id) {
		for (i = 0; i < DS90UB954_MAX_CHAN_NUM; i++) {
			if (chip_id & (1 << i)) {
				slave_addr = IDC_ADDR_SEN_BASE + (((ambrg->id << 1) + (1 << i)) << 1);
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
		ds90ub954_read_reg(ambrg, pinfo->i2c_ctrl.addr.des, 0x4, &data);
	} while (!(data & BIT(2)));

	return rval;
}

static void ds90ub954_set_fixed_pattern(struct vin_brg_device *ambrg)
{
	struct ds90ub954_priv *pinfo = (struct ds90ub954_priv *)ambrg->priv;
	struct vin_reg_8_8 *regs;
	u8 des_addr = pinfo->i2c_ctrl.addr.des;
	int i;

	regs = ds90ub954_test_pattern_regs;
	for (i = 0; i < ARRAY_SIZE(ds90ub954_test_pattern_regs); i++)
		ds90ub954_write_reg(ambrg, des_addr, regs[i].addr, regs[i].data);
}

static int ds90ub954_mode_to_string(ds90ub954_fpd_mode_t mode, char *string)
{
	if (mode == DS90UB954_FPD_MODE_CS)
		strcpy(string, "CSI");
	else if (mode == DS90UB954_FPD_MODE_RAW12LF)
		strcpy(string, "RAW 12 LF");
	else if (mode == DS90UB954_FPD_MODE_RAW12HF)
		strcpy(string, "RAW 12 HF");
	else if (mode == DS90UB954_FPD_MODE_RAW10)
		strcpy(string, "RAW 10");
	else
		return -EPERM;

	return 0;
}

static int ds90ub954_set_gpio(struct vin_brg_device *ambrg, u32 chip_id, ds90ub954_gpio_t gpio_id, u8 level)
{
	struct ds90ub954_priv *pinfo = (struct ds90ub954_priv *)ambrg->priv;
	ds90ub954_gpio_reg_u gpio_reg = {0};
	u16 reg_addr;
	u32 data;

	brg_debug("[0x%d]gpio %d->%d\n", chip_id, gpio_id, level);

	if (gpio_id >= DS90UB954_GPIO_MAX) {
		brg_error("gpio id is out of range:%d\n", gpio_id);
		return -EPERM;
	}

	reg_addr = 0x10 + gpio_id * sizeof(ds90ub954_gpio_reg_u);

	/* clear GPIO input enable */
	ds90ub954_read_reg(ambrg, pinfo->i2c_ctrl.addr.des, 0xF, &data);
	data &= ~BIT(gpio_id);
	ds90ub954_write_reg(ambrg, pinfo->i2c_ctrl.addr.des, 0xF, data);

	gpio_reg.bits.out_src = 0x4;
	gpio_reg.bits.out_sel = 0;
	gpio_reg.bits.out_en = 1;
	gpio_reg.bits.out_val = !!level;
	ds90ub954_write_reg(ambrg, pinfo->i2c_ctrl.addr.des, reg_addr, gpio_reg.data);

	return 0;
}

static void ds90ub954_poc_pwr_rst(struct vin_brg_device *ambrg, u32 chip_id)
{
	int i;

	/* reset poc power(GPIO0/1) */
	for (i = 0; i < DS90UB954_MAX_CHAN_NUM; i++) {
		if (chip_id & (1 << i)) {
			ds90ub954_set_gpio(ambrg, 1 << i, ds90ub954_poc_ctrl[i], 0);
			msleep(100);
			ds90ub954_set_gpio(ambrg, 1 << i, ds90ub954_poc_ctrl[i], 1);
		}
	}
}

static void ds90ub954_initialize_des(struct vin_brg_device *ambrg, u32 chip_id)
{
	struct ds90ub954_priv *pinfo = (struct ds90ub954_priv *)ambrg->priv;
	char input_mode[10] = {0};
	u8 des_addr = pinfo->i2c_ctrl.addr.des;
	u8 ser_addr;
	u8 bc_freq;
	u32 data;
	int i;

	if (chip_id == 0x1) {
		ds90ub954_write_reg(ambrg, des_addr, 0x0C, 0x81);
	} else if (chip_id == 0x2) {
		ds90ub954_write_reg(ambrg, des_addr, 0x0C, 0x96);
	} else if (chip_id == 0x3) {
		ds90ub954_write_reg(ambrg, des_addr, 0x0C, 0xBF);
	}

	for (i = 0; i < DS90UB954_MAX_CHAN_NUM; i++) {
		if (chip_id & (1 << i)) {
			ser_addr = (i == 0) ? pinfo->i2c_ctrl.addr.ser_chan0 : pinfo->i2c_ctrl.addr.ser_chan1;

			/* RX port W/R selection */
			data = (i == 0) ? 0x1 : 0x12;
			ds90ub954_write_reg(ambrg, des_addr, 0x4C, data);

			/* set I2C_PASS_THROUGH */
			ds90ub954_read_reg(ambrg, des_addr, 0x58, &data);
			ds90ub954_write_reg(ambrg, des_addr, 0x58, data | BIT(6));

			/* set SER_ALIAS_ID */
			ds90ub954_write_reg(ambrg, des_addr, 0x5C, ser_addr);

			/* sensor i2c address mapping for each channel */
			ds90ub954_write_reg(ambrg, des_addr, 0x5D, ambrg->sensor_ctrl.dev_addr);
			ds90ub954_write_reg(ambrg, des_addr, 0x65, IDC_ADDR_SEN_BASE + (((ambrg->id << 1) + (1 << i)) << 1));

			/* sensor i2c broadcasting address mapping */
			ds90ub954_write_reg(ambrg, des_addr, 0x5E, ambrg->sensor_ctrl.dev_addr);
			ds90ub954_write_reg(ambrg, des_addr, 0x66, pinfo->i2c_ctrl.addr.sen_all);

			ds90ub954_read_reg(ambrg, des_addr, 0x6D, &data);
			pinfo->link_mode = data & 0x3;
			ds90ub954_mode_to_string(pinfo->link_mode, input_mode);
			brg_info("[0x%x]FPD-Link mode(Mode pin): %s!\n", 1 << i, input_mode);

			/* change link mode if necessary */
			if (mode != DS90UB954_FPD_MODE_AUTO && mode != pinfo->link_mode) {
				pinfo->link_mode = mode;
				data &= ~(BIT(0) | BIT(1));
				data |= pinfo->link_mode;
				ds90ub954_write_reg(ambrg, des_addr, 0x6D, data);

				/* TODO: add specified bc freq here */
				bc_freq = (pinfo->link_mode == DS90UB954_FPD_MODE_CS) ? 0x6 : 0x0;

				/* set I2C_PASS_THROUGH again because bc_freq may change for different mode */
				ds90ub954_read_reg(ambrg, des_addr, 0x58, &data);
				data &= ~0x7;
				data |= bc_freq;
				ds90ub954_write_reg(ambrg, des_addr, 0x58, data);
				ds90ub954_mode_to_string(pinfo->link_mode, input_mode);
				brg_info("[0x%x]FPD-Link mode(SW): %s!\n", 1 << i, input_mode);
			}
		}
	}

	/* Turn on poc power */
	ds90ub954_poc_pwr_rst(ambrg, chip_id);
}

static void ds90ub954_initialize_ser(struct vin_brg_device *ambrg, u32 chip_id)
{
	struct ds90ub954_priv *pinfo = (struct ds90ub954_priv *)ambrg->priv;
	unsigned char chipid[6] = {0};
	u8 des_addr = pinfo->i2c_ctrl.addr.des;
	u8 ser_addr;
	u32 data;
	int i, j;

	for (i = 0; i < DS90UB954_MAX_CHAN_NUM; i++) {
		if (chip_id & (1 << i)) {
			ser_addr = (i == 0) ? pinfo->i2c_ctrl.addr.ser_chan0 : pinfo->i2c_ctrl.addr.ser_chan1;

			/* RX port W/R selection */
			data = (i == 0) ? 0x1 : 0x12;
			ds90ub954_write_reg(ambrg, des_addr, 0x4C, data);

			/* Read serializer i2c address */
			ds90ub954_read_reg(ambrg, des_addr, 0x5B, &data);
			brg_info("[0x%x]Serializer found, addr:0x%x!\n", 1 << i, data);

			/* check SER ID */
			for (j = 0; j < 5; j++)
				ds90ub954_read_reg(ambrg, ser_addr, 0xF1 + j, (u32 *)&(chipid[j]));

			if (!strncmp(chipid, "UB953", 5)) {
				pinfo->link_info[i].ser_is_953 = true;
				brg_info("[0x%x]Serializer is ds90ub953, id:%s!\n", 1 << i, chipid);
			}
		}
	}
}

static void ds90ub954_sen_pwr_rst(struct vin_brg_device *ambrg, u32 chip_id)
{
	struct ds90ub954_priv *pinfo = (struct ds90ub954_priv *)ambrg->priv;
	u8 ser_addr;
	u32 data;
	int i;

	for (i = 0; i < DS90UB954_MAX_CHAN_NUM; i++) {
		if (chip_id & (1 << i)) {
			ser_addr = (i == 0) ? pinfo->i2c_ctrl.addr.ser_chan0 : pinfo->i2c_ctrl.addr.ser_chan1;

			if (pinfo->link_info[i].ser_is_953) {
				/* TODO: set 953's GPIO here */
			} else {
				/* reset GPO0 to enable sensor module */
				ds90ub954_read_reg(ambrg, ser_addr, 0xD, &data);
				data |= BIT(0);
				data &= ~(BIT(3) | BIT(2));
				ds90ub954_write_reg(ambrg, ser_addr, 0xD, data);
				msleep(10);
				data |= BIT(3);
				ds90ub954_write_reg(ambrg, ser_addr, 0xD, data);
			}
		}
	}
}

static void ds90ub954_set_clk_si(struct vin_brg_device *ambrg, u32 chip_id, u32 clk_si)
{
	struct ds90ub954_priv *pinfo = (struct ds90ub954_priv *)ambrg->priv;
	u8 ser_addr;
	int i;

	brg_debug("%s, clk_si:%d\n", __func__, clk_si);

	if (test)
		return;

	for (i = 0; i < DS90UB954_MAX_CHAN_NUM; i++) {
		if (chip_id & (1 << i)) {
			ser_addr = (i == 0) ? pinfo->i2c_ctrl.addr.ser_chan0 : pinfo->i2c_ctrl.addr.ser_chan1;

			if (!pinfo->link_info[i].ser_is_953)
				continue;

			switch(clk_si) {
			case 24000000:
				ds90ub954_write_reg(ambrg, ser_addr, 0x06, 0x43); /* CLK_DIV=4, M=3 */
				ds90ub954_write_reg(ambrg, ser_addr, 0x07, 0x7D); /* N=125 */
				break;
			case 25000000:
				ds90ub954_write_reg(ambrg, ser_addr, 0x06, 0x41); /* CLK_DIV=4, M=1 */
				ds90ub954_write_reg(ambrg, ser_addr, 0x07, 0x28); /* N=40 */
				break;
			default:
				brg_error("clock %d hasn't been supported yet\n", clk_si);
				break;
			}
		}
	}

	ds90ub954_sen_pwr_rst(ambrg, chip_id);
}

static int ds90ub954_hw_init(struct vin_brg_device *ambrg, u32 chip_id)
{
	struct ds90ub954_priv *pinfo = (struct ds90ub954_priv *)ambrg->priv;
	unsigned char chipid[6] = {0};
	int rval, i, broken_id = 0;

	brg_debug("%s\n", __func__);

	if (chip_id != 0x1 && chip_id != 0x2 && chip_id != 0x3) {
		brg_error("ds90ub954 can only support max 2 links!\n");
		return -ENODEV;
	}

	/* check DES ID */
	for (i = 0; i < 5; i++)
		ds90ub954_read_reg(ambrg, pinfo->i2c_ctrl.addr.des, 0xF1 + i, (u32 *)&(chipid[i]));

	if (!strncmp(chipid, "UB954", 5))
		brg_info("[%d]deserializer: ds90ub954, id:%s!\n", ambrg->id, chipid);
	else
		return -ENODEV;

	/* Initialize deserializer */
	ds90ub954_initialize_des(ambrg, chip_id);

	if (test) {
		ds90ub954_set_fixed_pattern(ambrg);
	} else {
		rval = ds90ub954_wait_lock_status(ambrg, DS90UB954_LINK_LOCK_TO);
		if (rval < 0) {
			brg_error("[%d]No serializer link!\n", ambrg->id);
			return rval;
		}

		ds90ub954_initialize_ser(ambrg, chip_id);
	}

	return broken_id;
}

static int ds90ub954_dbg_write(struct vin_brg_device *ambrg, u32 chip_id, u32 subaddr, u32 data)
{
	u8 chan_id;

	if (subaddr == 0xFFFF) {
		ambrg->ch_id.dbg_id = data;
		switch (data & 0xF0) {
		case 0xF0:
			brg_info("sensor\n");
			break;
		default:
			break;
		}
	} else {
		chan_id = ambrg->ch_id.dbg_id & 0xF;
		switch (ambrg->ch_id.dbg_id & 0xF0) {
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
			"SER_B:0x%x\n" \
			"SEN base:0x%x\n",
			pinfo->i2c_ctrl.addr.des,
			pinfo->i2c_ctrl.addr.ser_chan0,
			pinfo->i2c_ctrl.addr.ser_chan1,
			IDC_ADDR_SEN_BASE);
	} else {
		chan_id = ambrg->ch_id.dbg_id & 0xF;
		switch (ambrg->ch_id.dbg_id & 0xF0) {
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
	u8 ser_addr;
	u8 mipi_clk_mode, mipi_lane;
	u32 data;
	int i;

	for (i = 0; i < DS90UB954_MAX_CHAN_NUM; i++) {
		if (chip_id & (1 << i)) {
			ser_addr = (i == 0) ? pinfo->i2c_ctrl.addr.ser_chan0 : pinfo->i2c_ctrl.addr.ser_chan1;

			if (pinfo->link_info[i].ser_is_953) {
				if (ambrg->vin_cfg->interface_type != SENSOR_MIPI) {
					brg_error("DS90UB953 can only support MIPI sensor!\n");
				} else {
					/* set clock mode/data lanes */
					mipi_clk_mode = (ambrg->vin_cfg->mipi_cfg.clk_mode == SENSOR_MIPI_CLK_NON_CONTINUE) ? 0 : 1;
					mipi_lane = ambrg->vin_cfg->mipi_cfg.lane_number;
					ds90ub954_read_reg(ambrg, ser_addr, 0x02, &data);
					data &= ~(BIT(4) | BIT(5) | BIT(6));
					data |= (mipi_clk_mode << 6) | ((mipi_lane - 1) << 4);
					ds90ub954_write_reg(ambrg, ser_addr, 0x02, data);
				}
			} else {
				/* TODO: set SER vin settings here */
			}
		}
	}
}

static void ds90ub954_vout_config(struct vin_brg_device *ambrg, u32 chip_id, struct vin_video_format *format)
{
	struct ds90ub954_priv *pinfo = (struct ds90ub954_priv *)ambrg->priv;
	u8 des_addr = pinfo->i2c_ctrl.addr.des;
	u8 fwd_port_mask = 0x3;
	int i;

	ambrg->vout_info.lane_num = SENSOR_4_LANE;
	ambrg->vout_info.interface_type = SENSOR_MIPI;
	ambrg->vout_info.data_rate = SENSOR_MIPI_BIT_RATE_H;

	if (ambrg->vin_cfg->input_mode == SENSOR_YUV_1PIX ||
		ambrg->vin_cfg->input_mode == SENSOR_YUV_2PIX) {
		ds90ub954_write_reg(ambrg, des_addr, 0xBC, 0x00); /* FRAME_VALID_MIN */
		ds90ub954_write_reg(ambrg, des_addr, 0x4B, 0x1E); /* DT: YUV422 8-bit */
		ds90ub954_write_reg(ambrg, des_addr, 0x7C,
			(pinfo->link_mode == DS90UB954_FPD_MODE_RAW10) ? 0xE0 : 0xA0);
		ds90ub954_write_reg(ambrg, des_addr, 0x70, 0x1E); /* set DT RAW10 */
	}

	/* continues mode, csi_en */
	ds90ub954_write_reg(ambrg, des_addr, 0x33, 0x03);
	/* synchronous forwarding with line interleaving */
	ds90ub954_write_reg(ambrg, des_addr, 0x21, 0x08);

	for (i = 0; i < DS90UB954_MAX_CHAN_NUM; i++) {
		if (chip_id & (1 << i)) {
			fwd_port_mask &= ~(1 << i);
		}
	}
	/* fwd_port to csi0 */
	ds90ub954_write_reg(ambrg, des_addr, 0x20, fwd_port_mask << 4);
}

static struct vin_brg_ops ds90ub954_ops = {
	.brg_write_reg			= ds90ub954_dbg_write,
	.brg_read_reg			= ds90ub954_dbg_read,
	.brg_write_chan_reg	= ds90ub954_write_chan_reg,
	.brg_read_chan_reg	= ds90ub954_read_chan_reg,
	.brg_hw_init_pre		= ds90ub954_hw_init,
	.brg_set_clk_si		= ds90ub954_set_clk_si,
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
	ambrg->ch_id.dbg_id = ds90ub954_addr_table[ambrg->id];
	ambrg->master_clk = DS90UB954_INPUT_CLK;
	ambrg->use_sen_clk = true;
	ambrg->use_vin_crop = true;
	ambrg->vout_info.output_format = 1;

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
	pinfo->i2c_ctrl.addr.ser_chan0  = IDC_ADDR_SER0_LINK0;
	pinfo->i2c_ctrl.addr.ser_chan1  = IDC_ADDR_SER0_LINK1;
	pinfo->i2c_ctrl.addr.sen_all = IDC_ADDR_SEN_ALL;

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
		if (chan_id) {
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

MODULE_DESCRIPTION("TI DS90UB954 DUAL FPD-LINK SERDES bridge");
MODULE_AUTHOR("Long Zhao, <longzhao@ambarella.com>");
MODULE_LICENSE("Proprietary");
