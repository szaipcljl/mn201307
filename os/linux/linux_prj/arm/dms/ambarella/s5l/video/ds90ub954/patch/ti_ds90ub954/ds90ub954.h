/*
 * Filename : ds90ub954.h
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

#ifndef __DS90UB954_H__
#define __DS90UB954_H__

//#define BRG_TO_SNR_CLK //clock output to sensor from brg

#define DS90UB954_LINK_LOCK_TO	100

#define DES_UB954A_ID	0x7A
#define DES_UB954B_ID	0x60

#define SER_UB953A_ID	0x9E
#define SER_UB953B_ID	0x9E
#define SER_UB913A_ID	0xB0
#define SER_UB913B_ID	0xB0

#define brg_info(str, args...) 	vin_info(str, ##args)
#define brg_error(str, args...) 	vin_error(str, ##args)
#define brg_debug(str, args...) 	vin_debug(str, ##args)

#define DS90UB954_MAX_NUM			2
#define DS90UB954_MAX_CHAN_NUM		2
#define DS90UB954_INPUT_CLK			24000000
#define DS90UB954_VOUT_DATA_RATE 	1000000000

#define IDC_ADDR_MAX20087		0x50
#define IDC_ADDR_ICM20648		0xD0

#define IDC_ADDR_DS90UB954_0	0x7A
#define IDC_ADDR_DS90UB954_1	0x60

#define IDC_ADDR_SER_DEFAULT	0xBA //ds90ub913
#define IDC_ADDR_SER0_LINKA	0xBA //RIN0+/-
#define IDC_ADDR_SER0_LINKB	0x84
#define IDC_ADDR_SER1_LINKA	0x86
#define IDC_ADDR_SER1_LINKB	0x88

typedef enum {
    DS90UB954_GPIO_0 = 0,
    DS90UB954_GPIO_1,
    DS90UB954_GPIO_2,
    DS90UB954_GPIO_3,
    DS90UB954_GPIO_4,
    DS90UB954_GPIO_5,
    DS90UB954_GPIO_6,
    DS90UB954_GPIO_7,
    DS90UB954_GPIO_8,
    DS90UB954_GPIO_9,
    DS90UB954_GPIO_10,

    DS90UB954_GPIO_MAX,
} ds90ub954_gpio_t;

typedef union {
    u8  data;
    struct {
        u8  gpio_out_ids : 1;	/* [0] 1 = disable GPIO output driver */
        u8  gpio_tx_en : 1;	/* [1] 1 = enable GPIO TX source for GMSL2 transmission */
        u8  gpio_rx_en : 1;	/* [2] 1 = enable GPIO Out source for GMSL2 reception */
        u8  gpio_in : 1;		/* [3] GPIO pin input value */
        u8  gpio_out : 1;		/* [4] GPIO pin output drive value when GpioRxEn=0 */
        u8  tx_comp_en : 1;	/* [5] Jitter minimization compesation enable */
        u8  tx_prio : 1;		/* [6] Priority for GPIO scheduling. 0 - Low priority, 1 - High priority */
        u8  res_cfg : 1;		/* [7] Resistor pull-up/pull-down strength. 0 - 40kohm, 1 - 1Mohm  */
    } bits;
} ds90ub954_gpio_reg_a_u;

typedef union {
    u8  data;
    struct {
        u8  gpio_tx_id : 5;	/* [4:0] GPIO ID for pin while transmiting */
        u8  out_type : 1;		/* [5] Driver type selection. 0 - Open-drain, 1 - Push-pull */
        u8  pull_updn_sel : 2;	/* [7:6] Buffer pull up/down configuration. 0 - None, 1 - Pull-up, 2 - Pull-down, 3 - Reserved */
    } bits;
} ds90ub954_gpio_reg_b_u;

typedef union {
    u8  data;
    struct {
        u8  gpio_rx_id : 5;	/* [4:0] GPIO ID for pin while receiving */
        u8  reserved : 2;		/* [6:5] */
        u8  ovr_res_cfg : 1;	/* [7] Override non-GPIO port function IO setting */
    } bits;
} ds90ub954_gpio_reg_c_u;

typedef struct {
    ds90ub954_gpio_reg_a_u gpio_a;
    ds90ub954_gpio_reg_b_u gpio_b;
    ds90ub954_gpio_reg_c_u gpio_c;
} ds90ub954_gpio_reg_s;

struct ds90ub954_id_t {
	u8 id;
	char name[12];
};

struct ds90ub954_addr_t {
	u16 des;
	u16 ser_def;
	u16 ser_chan0;
	u16 ser_chan1;
};

struct ds90ub954_i2c_ctrl {
	struct i2c_adapter *adapter;
	struct ds90ub954_addr_t addr;
};

//ds90ub954 register maps
#define UB954_FPD3_PORT_SEL_REG 0x4c
#define UB954_RX_PORT_CTL_REG 0x0C
#define UB954_BCC_CONFIG_REG 0x58
#define UB954_SER_ALIAS_ID_REG 0x5C
#define UB954_SLAVE_ID0_REG 0x5D
#define UB954_SLAVE_ALIAS_ID0_REG 0x65
#define UB954_I2C_DEVICE_ID_REG 0x00

#define UB954_DEVICE_STS_REG 0x04

//CSI control
#define UB954_CSI_PLL_CTL_REG 0x1F
#define UB954_FWD_CTL1_REG 0x20
#define UB954_FWD_CTL2_REG 0x21
#define UB954_CSI_CTL_REG 0x33
#define UB954_PORT_CONFIG_REG 0x6D
#define UB954_PORT_CONFIG2_REG 0x7C
#define UB954_RAW10_ID_REG 0x70   //RAW10 input mode
#define UB954_RAW12_ID_REG 0x71   //RAW12 input mode
#define UB954_CSI_VC_MAP_REG 0x72 //CSI-2 input mode


//ds90ub913 register maps
#define UB913_GPIO0_1_CONFIG 0x0D

#endif /* __DS90UB954_H__ */
