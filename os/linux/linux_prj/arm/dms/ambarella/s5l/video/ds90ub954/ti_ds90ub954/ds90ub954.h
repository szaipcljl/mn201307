/*
 * Filename : ds90ub954.h
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

#ifndef __DS90UB954_H__
#define __DS90UB954_H__

#define DS90UB954_LINK_LOCK_TO	1000

#define brg_info(str, args...) 	vin_info(str, ##args)
#define brg_error(str, args...) 	vin_error(str, ##args)
#define brg_debug(str, args...) 	vin_debug(str, ##args)

#define DS90UB954_MAX_NUM				2
#define DS90UB954_MAX_CHAN_NUM		2
#define DS90UB954_INPUT_CLK			25000000
#define DS90UB954_VOUT_DATA_RATE 		1000000000

#define IDC_ADDR_DS90UB954_0	0x60
#define IDC_ADDR_DS90UB954_1	0x7A

#define IDC_ADDR_SER0_LINK0	0x80
#define IDC_ADDR_SER0_LINK1	0x82

#define IDC_ADDR_SEN_BASE		0x90
#define IDC_ADDR_SEN_ALL		0x10

typedef enum {
    DS90UB954_GPIO_0 = 0,
    DS90UB954_GPIO_1,
    DS90UB954_GPIO_2,
    DS90UB954_GPIO_3,
    DS90UB954_GPIO_4,
    DS90UB954_GPIO_5,
    DS90UB954_GPIO_6,

    DS90UB954_GPIO_MAX,
} ds90ub954_gpio_t;

typedef enum {
    DS90UB954_FPD_MODE_CS = 0,
    DS90UB954_FPD_MODE_RAW12LF,
    DS90UB954_FPD_MODE_RAW12HF,
    DS90UB954_FPD_MODE_RAW10,

    DS90UB954_FPD_MODE_AUTO = 0xFF,
} ds90ub954_fpd_mode_t;

typedef union {
    u8  data;
    struct {
        u8  out_en : 1;
        u8  out_val : 1;
        u8  out_src : 3;
        u8  out_sel : 3;
    } bits;
} ds90ub954_gpio_reg_u;

struct ds90ub954_addr_t {
	u16 des;
	u16 ser_chan0;
	u16 ser_chan1;
	u16 sen_all;
};

struct ds90ub954_i2c_ctrl {
	struct i2c_adapter *adapter;
	struct ds90ub954_addr_t addr;
};

struct ds90ub954_link_info {
	bool ser_is_953;
};

#endif /* __DS90UB954_H__ */
