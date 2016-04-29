/*
 * Copyright (C) 2012 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * V4.0
 */

#include <utils/Log.h>
#include "sensor.h"
#include "jpeg_exif_header.h"
#include "sensor_drv_u.h"
#include "sensor_raw.h"

#if defined(CONFIG_CAMERA_ISP_VERSION_V3) || defined(CONFIG_CAMERA_ISP_VERSION_V4)
#include "sensor_bf2205_front_raw_param_main.c"
#else
#include "sensor_bf2205_front_raw_param.c"
#endif

#ifndef CONFIG_CAMERA_AUTOFOCUS_NOT_SUPPORT
#include "../af_dw9714.h"
#endif

#define SENSOR_NAME			"bf2205_front"
#define I2C_SLAVE_ADDR			0x60

#define bf2205_front_PID_ADDR			0x0A
#define bf2205_front_PID_VALUE			0x02
#define bf2205_front_VER_ADDR			0x0B
#define bf2205_front_VER_VALUE			0x05

/* sensor parameters begin */
#define SNAPSHOT_WIDTH			1600
#define SNAPSHOT_HEIGHT		1200
#define PREVIEW_WIDTH			1600
#define PREVIEW_HEIGHT			1200

/*Mipi output*/
#define LANE_NUM			1
#define RAW_BITS				10

#define SNAPSHOT_MIPI_PER_LANE_BPS	384
#define PREVIEW_MIPI_PER_LANE_BPS		384

/*line time unit: 0.1us*/
#define SNAPSHOT_LINE_TIME		470
#define PREVIEW_LINE_TIME		470

/* frame length*/
#define SNAPSHOT_FRAME_LENGTH		1418
#define PREVIEW_FRAME_LENGTH		1418

/* please ref your spec */
#define FRAME_OFFSET			3
#define SENSOR_MAX_GAIN		0x200  
#define SENSOR_BASE_GAIN		0x10
#define SENSOR_MIN_SHUTTER		1 
//////////////////////////////////////////////
#define bf2205_front_MIRROR_EN    0
#define bf2205_front_FLIP_EN         0

/* please ref your spec
 * 1 : average binning
 * 2 : sum-average binning
 * 4 : sum binning
 */
#define BINNING_FACTOR			1
#define bf2205_front_USE_VERTICAL_BINNING_EN  1

/* please ref spec
 * 1: sensor auto caculate
 * 0: driver caculate
 */
#define SUPPORT_AUTO_FRAME_LENGTH	0
/* sensor parameters end */

/* isp parameters, please don't change it*/
#if defined(CONFIG_CAMERA_ISP_VERSION_V3) || defined(CONFIG_CAMERA_ISP_VERSION_V4)
#define ISP_BASE_GAIN			0x80
#else
#define ISP_BASE_GAIN			0x10
#endif
/* please don't change it */
#define EX_MCLK				24

struct hdr_info_t {
	uint32_t capture_max_shutter;
	uint32_t capture_shutter;
	uint32_t capture_gain;
};

struct sensor_ev_info_t {
	uint16_t preview_shutter;
	uint16_t preview_gain;
};

/*==============================================================================
 * Description:
 * global variable
 *============================================================================*/
static struct hdr_info_t s_hdr_info;
static uint32_t s_current_default_frame_length;
struct sensor_ev_info_t s_sensor_ev_info;

//#define FEATURE_OTP    /*OTP function switch*/

#ifdef  FEATURE_OTP 			
#define MODULE_ID_NULL			0x0000
#define MODULE_ID_bf2205_front_yyy		0x0001    //bf2205_front: sensor P/N;  yyy: module vendor
#define MODULE_ID_END			0xFFFF
#define LSC_PARAM_QTY 240

struct otp_info_t {
	uint16_t flag;
	uint16_t module_id;
	uint16_t lens_id;
	uint16_t vcm_id;
	uint16_t vcm_driver_id;
	uint16_t year;
	uint16_t month;
	uint16_t day;
	uint16_t rg_ratio_current;
	uint16_t bg_ratio_current;
	uint16_t rg_ratio_typical;
	uint16_t bg_ratio_typical;
	uint16_t r_current;
	uint16_t g_current;
	uint16_t b_current;
	uint16_t r_typical;
	uint16_t g_typical;
	uint16_t b_typical;
	uint16_t vcm_dac_start;
	uint16_t vcm_dac_inifity;
	uint16_t vcm_dac_macro;
	uint16_t lsc_param[LSC_PARAM_QTY];
};


#include "sensor_bf2205_front_yyy_otp.c"

	struct raw_param_info_tab s_bf2205_front_raw_param_tab[] = {
		{MODULE_ID_bf2205_front_yyy, &s_bf2205_front_mipi_raw_info, bf2205_front_yyy_identify_otp, bf2205_front_yyy_update_otp},
		{MODULE_ID_END, PNULL, PNULL, PNULL}
	};

#else

#define bf2205_front_RAW_PARAM_COM		0x0000

		struct raw_param_info_tab s_bf2205_front_raw_param_tab[] = {
			{bf2205_front_RAW_PARAM_COM, &s_bf2205_front_mipi_raw_info, PNULL, PNULL},
			{RAW_INFO_END_ID, PNULL, PNULL, PNULL}
		};

#endif

static SENSOR_IOCTL_FUNC_TAB_T s_bf2205_front_ioctl_func_tab;
struct sensor_raw_info *s_bf2205_front_mipi_raw_info_ptr = &s_bf2205_front_mipi_raw_info;

static const SENSOR_REG_T bf2205_front_init_setting[] = {
		{0x12,(0x40 | (bf2205_front_MIRROR_EN << 5) |(bf2205_front_FLIP_EN << 4))},
	{0x00,0x1f}, // add
	{0x01,0x40}, // add
	{0x02,0x04}, // add
	{0x0D,0xF0},
		{0x1F,0x00},
		{0x0E,0x1D},
		{0x0F,0x09},
		{0x10,0x20},
		{0x11,0x80},
	{0x20,0x04},
		{0x21,0x07},
	{0x22,0x92},
		{0x23,0x05},
		{0x24,0x40},
		{0x25,0xB0},
		{0x26,0x46},
	{0x27,0xBC},
	{0x28,0x0E}, // 0x0D
		{0x29,0x00},
		{0x2A,0xAF},
		{0x2B,0x50},
		{0x2C,0x03},
		{0x2D,0x00},
		{0x2E,0x30},
		{0x2F,0x04},
		{0x1D,0x00},
		{0x1E,0x00},
		{0x6C,0x68},
		{0x70,0x49},
		{0x72,0x58},
		{0x73,0x33},
		{0x74,0x78},
		{0x75,0x2B},
		{0x76,0xD0},
		{0x77,0x07},
		{0x78,0x15},
		{0x6D,0xAA},
		{0x6E,0x9D},
		{0x6F,0x9D},
		{0x60,0x47},
		{0x61,0x80},
		{0x68,0x04},
		{0x13,0x87},
		{0x14,0x80},
		{0x16,0xC0},
		{0x17,0x40},
		{0x18,0xD5},
		{0x19,0x80},
		{0x37,0x2E},
		{0x38,0xFF},
		{0x39,0x70},
		{0x4A,0x03},
		{0x49,0x10},
		{0x48,0x20},
		
};

static const SENSOR_REG_T bf2205_front_preview_setting[] = {
		{0x12,(0x40 | (bf2205_front_MIRROR_EN << 5) |(bf2205_front_FLIP_EN << 4))},
			{0x00,0x1f}, // add
	{0x01,0x40}, // add
	{0x02,0x04}, // add
	{0x0D,0xF0},
		{0x1F,0x00},
		{0x0E,0x1D},
		{0x0F,0x09},
		{0x10,0x20},
		{0x11,0x80},
	{0x20,0x04},
		{0x21,0x07},
	{0x22,0x92},
		{0x23,0x05},
		{0x24,0x40},
		{0x25,0xB0},
		{0x26,0x46},
	{0x27,0xBC},
	{0x28,0x0E}, // 0x0D
		{0x29,0x00},
		{0x2A,0xAF},
		{0x2B,0x50},
		{0x2C,0x03},
		{0x2D,0x00},
		{0x2E,0x30},
		{0x2F,0x04},
		{0x1D,0x00},
		{0x1E,0x00},
		{0x6C,0x68},
		{0x70,0x49},
		{0x72,0x58},
		{0x73,0x33},
		{0x74,0x78},
		{0x75,0x2B},
		{0x76,0xD0},
		{0x77,0x07},
		{0x78,0x15},
		{0x6D,0xAA},
		{0x6E,0x9D},
		{0x6F,0x9D},
		{0x60,0x47},
		{0x61,0x80},
		{0x68,0x04},
		{0x13,0x87},
		{0x14,0x80},
		{0x16,0xC0},
		{0x17,0x40},
		{0x18,0xD5},
		{0x19,0x80},
		{0x37,0x2E},
		{0x38,0xFF},
		{0x39,0x70},
		{0x4A,0x03},
		{0x49,0x10},
		{0x48,0x20},
	
};

static const SENSOR_REG_T bf2205_front_snapshot_setting[] = {
		{0x12,(0x40 | (bf2205_front_MIRROR_EN << 5) |(bf2205_front_FLIP_EN << 4))},
		{0x00,0x1f}, // add
	{0x01,0x40}, // add
	{0x02,0x04}, // add
	{0x0D,0xF0},
		{0x1F,0x00},
		{0x0E,0x1D},
		{0x0F,0x09},
		{0x10,0x20},
		{0x11,0x80},
	{0x20,0x04},
		{0x21,0x07},
	{0x22,0x92},
		{0x23,0x05},
		{0x24,0x40},
		{0x25,0xB0},
		{0x26,0x46},
	{0x27,0xBC},
	{0x28,0x0E}, // 0x0D
		{0x29,0x00},
		{0x2A,0xAF},
		{0x2B,0x50},
		{0x2C,0x03},
		{0x2D,0x00},
		{0x2E,0x30},
		{0x2F,0x04},
		{0x1D,0x00},
		{0x1E,0x00},
		{0x6C,0x68},
		{0x70,0x49},
		{0x72,0x58},
		{0x73,0x33},
		{0x74,0x78},
		{0x75,0x2B},
		{0x76,0xD0},
		{0x77,0x07},
		{0x78,0x15},
		{0x6D,0xAA},
		{0x6E,0x9D},
		{0x6F,0x9D},
		{0x60,0x47},
		{0x61,0x80},
		{0x68,0x04},
		{0x13,0x87},
		{0x14,0x80},
		{0x16,0xC0},
		{0x17,0x40},
		{0x18,0xD5},
		{0x19,0x80},
		{0x37,0x2E},
		{0x38,0xFF},
		{0x39,0x70},
		{0x4A,0x03},
		{0x49,0x10},
		{0x48,0x20},
	
};

static SENSOR_REG_TAB_INFO_T s_bf2205_front_resolution_tab_raw[SENSOR_MODE_MAX] = {
	{ADDR_AND_LEN_OF_ARRAY(bf2205_front_init_setting), 0, 0, EX_MCLK,
	 SENSOR_IMAGE_FORMAT_RAW},	
	{ADDR_AND_LEN_OF_ARRAY(bf2205_front_preview_setting), 
	PREVIEW_WIDTH, PREVIEW_HEIGHT, EX_MCLK, 
	SENSOR_IMAGE_FORMAT_RAW},
	{ADDR_AND_LEN_OF_ARRAY(bf2205_front_snapshot_setting),
	SNAPSHOT_WIDTH, SNAPSHOT_HEIGHT, EX_MCLK,
	SENSOR_IMAGE_FORMAT_RAW}, 	 
	//{ADDR_AND_LEN_OF_ARRAY(bf2205_front_snapshot_setting),SNAPSHOT_WIDTH, SNAPSHOT_HEIGHT, EX_MCLK,SENSOR_IMAGE_FORMAT_RAW}, 	 

};

static SENSOR_TRIM_T s_bf2205_front_resolution_trim_tab[SENSOR_MODE_MAX] = {
	{0, 0, 0, 0, 0, 0, 0, {0, 0, 0, 0}},
	{0, 0, PREVIEW_WIDTH, PREVIEW_HEIGHT, PREVIEW_LINE_TIME, PREVIEW_MIPI_PER_LANE_BPS, PREVIEW_FRAME_LENGTH, {0, 0, PREVIEW_WIDTH, PREVIEW_HEIGHT}},
	{0, 0, SNAPSHOT_WIDTH, SNAPSHOT_HEIGHT, SNAPSHOT_LINE_TIME, SNAPSHOT_MIPI_PER_LANE_BPS, SNAPSHOT_FRAME_LENGTH,{0, 0, SNAPSHOT_WIDTH, SNAPSHOT_HEIGHT}},
	//{0, 0, SNAPSHOT_WIDTH, SNAPSHOT_HEIGHT, SNAPSHOT_LINE_TIME, SNAPSHOT_MIPI_PER_LANE_BPS, SNAPSHOT_FRAME_LENGTH,{0, 0, SNAPSHOT_WIDTH, SNAPSHOT_HEIGHT}},

};
/*******
static const SENSOR_REG_T s_bf2205_front_1600X1200_video_tab[SENSOR_VIDEO_MODE_MAX][1] = {

	{
	 {0xffff, 0xff}
	 },

	{
	 {0xffff, 0xff}
	 },

	{
	 {0xffff, 0xff}
	 },

	{
	 {0xffff, 0xff}
	 }
};
*******/
static const SENSOR_REG_T s_bf2205_front_1600X1200_video_tab[SENSOR_VIDEO_MODE_MAX][1] = {
	/*video mode 0: ?fps */
	{
	 {0xffff, 0xff}
	 },
	/* video mode 1:?fps */
	{
	 {0xffff, 0xff}
	 },
	/* video mode 2:?fps */
	{
	 {0xffff, 0xff}
	 },
	/* video mode 3:?fps */
	{
	 {0xffff, 0xff}
	 }
};

static SENSOR_VIDEO_INFO_T s_bf2205_front_video_info[SENSOR_MODE_MAX] = {
	{{{0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}, PNULL},
	{{{30, 30, 470, 90}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
	 (SENSOR_REG_T **) s_bf2205_front_1600X1200_video_tab},
	{{{15, 15, 470, 1000}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
	 (SENSOR_REG_T **) s_bf2205_front_1600X1200_video_tab},
};

/*==============================================================================
 * Description:
 * set video mode
 *
 *============================================================================*/
static uint32_t bf2205_front_set_video_mode(uint32_t param)
{
	SENSOR_REG_T_PTR sensor_reg_ptr;
	uint16_t i = 0x00;
	uint32_t mode;

	if (param >= SENSOR_VIDEO_MODE_MAX)
		return 0;

	if (SENSOR_SUCCESS != Sensor_GetMode(&mode)) {
		SENSOR_PRINT("fail.");
		return SENSOR_FAIL;
	}

	if (PNULL == s_bf2205_front_video_info[mode].setting_ptr) {
		SENSOR_PRINT("fail.");
		return SENSOR_FAIL;
	}

	sensor_reg_ptr = (SENSOR_REG_T_PTR) & s_bf2205_front_video_info[mode].setting_ptr[param];
	if (PNULL == sensor_reg_ptr) {
		SENSOR_PRINT("fail.");
		return SENSOR_FAIL;
	}

	for (i = 0x00; (0xffff != sensor_reg_ptr[i].reg_addr)
	     || (0xff != sensor_reg_ptr[i].reg_value); i++) {
		Sensor_WriteReg(sensor_reg_ptr[i].reg_addr, sensor_reg_ptr[i].reg_value);
	}

	return 0;
}

/*==============================================================================
 * Description:
 * sensor all info
 * please modify this variable acording your spec
 *============================================================================*/
SENSOR_INFO_T g_BF2205_front_mipi_raw_info = {
	/* salve i2c write address */
	(I2C_SLAVE_ADDR >> 1),
	/* salve i2c read address */
	(I2C_SLAVE_ADDR >> 1),
	/*bit0: 0: i2c register value is 8 bit, 1: i2c register value is 16 bit */
	SENSOR_I2C_REG_8BIT | SENSOR_I2C_VAL_8BIT | SENSOR_I2C_FREQ_400,
	/* bit2: 0:negative; 1:positive -> polarily of horizontal synchronization signal
	 * bit4: 0:negative; 1:positive -> polarily of vertical synchronization signal
	 * other bit: reseved
	 */
	SENSOR_HW_SIGNAL_PCLK_N| SENSOR_HW_SIGNAL_VSYNC_N | SENSOR_HW_SIGNAL_HSYNC_P,
	/* preview mode */
	SENSOR_ENVIROMENT_NORMAL | SENSOR_ENVIROMENT_NIGHT,
	/* image effect */
	SENSOR_IMAGE_EFFECT_NORMAL |
	    SENSOR_IMAGE_EFFECT_BLACKWHITE |
	    SENSOR_IMAGE_EFFECT_RED |
	    SENSOR_IMAGE_EFFECT_GREEN | SENSOR_IMAGE_EFFECT_BLUE | SENSOR_IMAGE_EFFECT_YELLOW |
	    SENSOR_IMAGE_EFFECT_NEGATIVE | SENSOR_IMAGE_EFFECT_CANVAS,

	/* while balance mode */
	0,
	/* bit[0:7]: count of step in brightness, contrast, sharpness, saturation
	 * bit[8:31] reseved
	 */
	7,
	/* reset pulse level */
	SENSOR_LOW_PULSE_RESET,
	/* reset pulse width(ms) */
	50,
	/* 1: high level valid; 0: low level valid */
	SENSOR_HIGH_LEVEL_PWDN,
	/* count of identify code */
	1,
	/* supply two code to identify sensor.
	 * for Example: index = 0-> Device id, index = 1 -> version id
	 * customer could ignore it.
	 */
	{{bf2205_front_PID_ADDR, bf2205_front_PID_VALUE}
	 ,
	 {bf2205_front_VER_ADDR, bf2205_front_VER_VALUE}
	 }
	,
	/* voltage of avdd */
	SENSOR_AVDD_2800MV,
	/* max width of source image */
	SNAPSHOT_WIDTH,
	/* max height of source image */
	SNAPSHOT_HEIGHT,
	/* name of sensor */
	SENSOR_NAME,
	/* define in SENSOR_IMAGE_FORMAT_E enum,SENSOR_IMAGE_FORMAT_MAX
	 * if set to SENSOR_IMAGE_FORMAT_MAX here,
	 * image format depent on SENSOR_REG_TAB_INFO_T
	 */
	SENSOR_IMAGE_FORMAT_RAW,
	/*  pattern of input image form sensor */
	SENSOR_IMAGE_PATTERN_RAWRGB_R,
	/* point to resolution table information structure */
	s_bf2205_front_resolution_tab_raw,
	/* point to ioctl function table */
	&s_bf2205_front_ioctl_func_tab,
	/* information and table about Rawrgb sensor */
	&s_bf2205_front_mipi_raw_info_ptr,
	/* extend information about sensor
	 * like &g_bf2205_front_ext_info
	 */
	NULL,
	/* voltage of iovdd */
	SENSOR_AVDD_1800MV,
	/* voltage of dvdd */
	SENSOR_AVDD_2800MV,
	/* skip frame num before preview */
	2, //1
	/* skip frame num before capture */
	2, //1
	/* deci frame num during preview */
	0,
	/* deci frame num during video preview */
	0,
	0,
	0,
	0,
	0,
	0,
	{SENSOR_INTERFACE_TYPE_CSI2, LANE_NUM, RAW_BITS, 0}
	,
	0,
	/* skip frame num while change setting */
	2, //1
	/* horizontal  view angle*/
	65,
	/* vertical view angle*/
	60
};

#if defined(CONFIG_CAMERA_ISP_VERSION_V3) || defined(CONFIG_CAMERA_ISP_VERSION_V4)

#define param_update(x1,x2) sprintf(name,"/data/bf2205_front_%s.bin",x1);\
				if(0==access(name,R_OK))\
				{\
					FILE* fp = NULL;\
					SENSOR_PRINT("param file %s exists",name);\
					if( NULL!=(fp=fopen(name,"rb")) ){\
						fread((void*)x2,1,sizeof(x2),fp);\
						fclose(fp);\
					}else{\
						SENSOR_PRINT("param open %s failure",name);\
					}\
				}\
				memset(name,0,sizeof(name))

static uint32_t bf2205_front_InitRawTuneInfo(void)
{
	uint32_t rtn=0x00;

	isp_raw_para_update_from_file(&g_BF2205_front_mipi_raw_info,0);

	struct sensor_raw_info* raw_sensor_ptr=s_bf2205_front_mipi_raw_info_ptr;
	struct isp_mode_param* mode_common_ptr = raw_sensor_ptr->mode_ptr[0].addr;
	int i;
	char name[100] = {'\0'};

	for (i=0; i<mode_common_ptr->block_num; i++) {
		struct isp_block_header* header = &(mode_common_ptr->block_header[i]);
		uint8_t* data = (uint8_t*)mode_common_ptr + header->offset;
		switch (header->block_id)
		{
		case	ISP_BLK_PRE_WAVELET_V1: {
				/* modify block data */
				struct sensor_pwd_param* block = (struct sensor_pwd_param*)data;

				static struct sensor_pwd_level pwd_param[SENSOR_SMART_LEVEL_NUM] = {
					#include "NR/pwd_param.h"
				};

				param_update("pwd_param",pwd_param);

				block->param_ptr = pwd_param;
			}
			break;

		case	ISP_BLK_BPC_V1: {
				/* modify block data */
				struct sensor_bpc_param_v1* block = (struct sensor_bpc_param_v1*)data;

				static struct sensor_bpc_level bpc_param[SENSOR_SMART_LEVEL_NUM] = {
					#include "NR/bpc_param.h"
				};

				param_update("bpc_param",bpc_param);

				block->param_ptr = bpc_param;
			}
			break;

		case	ISP_BLK_BL_NR_V1: {
				/* modify block data */
				struct sensor_bdn_param* block = (struct sensor_bdn_param*)data;

				static struct sensor_bdn_level bdn_param[SENSOR_SMART_LEVEL_NUM] = {
					#include "NR/bdn_param.h"
				};

				param_update("bdn_param",bdn_param);

				block->param_ptr = bdn_param;
			}
			break;

		case	ISP_BLK_GRGB_V1: {
				/* modify block data */
				struct sensor_grgb_v1_param* block = (struct sensor_grgb_v1_param*)data;
				static struct sensor_grgb_v1_level grgb_param[SENSOR_SMART_LEVEL_NUM] = {
					#include "NR/grgb_param.h"
				};

				param_update("grgb_param",grgb_param);

				block->param_ptr = grgb_param;

			}
			break;

		case	ISP_BLK_NLM: {
				/* modify block data */
				struct sensor_nlm_param* block = (struct sensor_nlm_param*)data;

				static struct sensor_nlm_level nlm_param[32] = {
					#include "NR/nlm_param.h"
				};

				param_update("nlm_param",nlm_param);

				static struct sensor_vst_level vst_param[32] = {
					#include "NR/vst_param.h"
				};

				param_update("vst_param",vst_param);

				static struct sensor_ivst_level ivst_param[32] = {
					#include "NR/ivst_param.h"
				};

				param_update("ivst_param",ivst_param);

				static struct sensor_flat_offset_level flat_offset_param[32] = {
					#include "NR/flat_offset_param.h"
				};

				param_update("flat_offset_param",flat_offset_param);

				block->param_nlm_ptr = nlm_param;
				block->param_vst_ptr = vst_param;
				block->param_ivst_ptr = ivst_param;
				block->param_flat_offset_ptr = flat_offset_param;
			}
			break;

		case	ISP_BLK_CFA_V1: {
				/* modify block data */
				struct sensor_cfa_param_v1* block = (struct sensor_cfa_param_v1*)data;
				static struct sensor_cfae_level cfae_param[SENSOR_SMART_LEVEL_NUM] = {
					#include "NR/cfae_param.h"
				};

				param_update("cfae_param",cfae_param);

				block->param_ptr = cfae_param;
			}
			break;

		case	ISP_BLK_RGB_PRECDN: {
				/* modify block data */
				struct sensor_rgb_precdn_param* block = (struct sensor_rgb_precdn_param*)data;

				static struct sensor_rgb_precdn_level precdn_param[SENSOR_SMART_LEVEL_NUM] = {
					#include "NR/rgb_precdn_param.h"
				};

				param_update("rgb_precdn_param",precdn_param);

				block->param_ptr = precdn_param;
			}
			break;

		case	ISP_BLK_YUV_PRECDN: {
				/* modify block data */
				struct sensor_yuv_precdn_param* block = (struct sensor_yuv_precdn_param*)data;

				static struct sensor_yuv_precdn_level yuv_precdn_param[SENSOR_SMART_LEVEL_NUM] = {
					#include "NR/yuv_precdn_param.h"
				};

				param_update("yuv_precdn_param",yuv_precdn_param);

				block->param_ptr = yuv_precdn_param;
			}
			break;

		case	ISP_BLK_PREF_V1: {
				/* modify block data */
				struct sensor_prfy_param* block = (struct sensor_prfy_param*)data;

				static struct sensor_prfy_level prfy_param[SENSOR_SMART_LEVEL_NUM] = {
					#include "NR/prfy_param.h"
				};

				param_update("prfy_param",prfy_param);

				block->param_ptr = prfy_param;
			}
			break;

		case	ISP_BLK_UV_CDN: {
				/* modify block data */
				struct sensor_uv_cdn_param* block = (struct sensor_uv_cdn_param*)data;

				static struct sensor_uv_cdn_level uv_cdn_param[SENSOR_SMART_LEVEL_NUM] = {
					#include "NR/yuv_cdn_param.h"
				};

				param_update("yuv_cdn_param",uv_cdn_param);

				block->param_ptr = uv_cdn_param;
			}
			break;

		case	ISP_BLK_EDGE_V1: {
				/* modify block data */
				struct sensor_ee_param* block = (struct sensor_ee_param*)data;

				static struct sensor_ee_level edge_param[SENSOR_SMART_LEVEL_NUM] = {
					#include "NR/edge_param.h"
				};

				param_update("edge_param",edge_param);

				block->param_ptr = edge_param;
			}
			break;

		case	ISP_BLK_UV_POSTCDN: {
				/* modify block data */
				struct sensor_uv_postcdn_param* block = (struct sensor_uv_postcdn_param*)data;

				static struct sensor_uv_postcdn_level uv_postcdn_param[SENSOR_SMART_LEVEL_NUM] = {
					#include "NR/yuv_postcdn_param.h"
				};

				param_update("yuv_postcdn_param",uv_postcdn_param);

				block->param_ptr = uv_postcdn_param;
			}
			break;

		case	ISP_BLK_IIRCNR_IIR: {
				/* modify block data */
				struct sensor_iircnr_param* block = (struct sensor_iircnr_param*)data;

				static struct sensor_iircnr_level iir_cnr_param[SENSOR_SMART_LEVEL_NUM] = {
					#include "NR/iircnr_param.h"
				};

				param_update("iircnr_param",iir_cnr_param);

				block->param_ptr = iir_cnr_param;
			}
			break;

		case	ISP_BLK_IIRCNR_YRANDOM: {
				/* modify block data */
				struct sensor_iircnr_yrandom_param* block = (struct sensor_iircnr_yrandom_param*)data;
				static struct sensor_iircnr_yrandom_level iir_yrandom_param[SENSOR_SMART_LEVEL_NUM] = {
					#include "NR/iir_yrandom_param.h"
				};

				param_update("iir_yrandom_param",iir_yrandom_param);

				block->param_ptr = iir_yrandom_param;
			}
			break;

		case  ISP_BLK_UVDIV_V1: {
				/* modify block data */
				struct sensor_cce_uvdiv_param_v1* block = (struct sensor_cce_uvdiv_param_v1*)data;

				static struct sensor_cce_uvdiv_level cce_uvdiv_param[SENSOR_SMART_LEVEL_NUM] = {
					#include "NR/cce_uv_param.h"
				};

				param_update("cce_uv_param",cce_uvdiv_param);

				block->param_ptr = cce_uvdiv_param;
			}
			break;
		case ISP_BLK_YIQ_AFM:{
			/* modify block data */
			struct sensor_y_afm_param *block = (struct sensor_y_afm_param*)data;

			static struct sensor_y_afm_level y_afm_param[SENSOR_SMART_LEVEL_NUM] = {
					#include "NR/y_afm_param.h"
				};

				param_update("y_afm_param",y_afm_param);

				block->param_ptr = y_afm_param;
			}
			break;

		default:
			break;
		}
	}


	return rtn;
}
#endif

/*==============================================================================
 * Description:
 * get default frame length
 *
 *============================================================================*/
static uint32_t bf2205_front_get_default_frame_length(uint32_t mode)
{
	return s_bf2205_front_resolution_trim_tab[mode].frame_line;
}

/*==============================================================================
 * Description:
 * write group-hold on to sensor registers
 * please modify this function acording your spec
 *============================================================================*/
static void bf2205_front_group_hold_on(void)
{
	SENSOR_PRINT("E");
	#if 0
	uint32 val;
	Sensor_ReadReg(val,0x12)
	Sensor_WriteReg(0x12, val);
	#endif
}

/*==============================================================================
 * Description:
 * write group-hold off to sensor registers
 * please modify this function acording your spec
 *============================================================================*/
static void bf2205_front_group_hold_off(void)
{
	SENSOR_PRINT("E");
	#if 0
	uint32 val;
	Sensor_ReadReg(val , 0x12)
	Sensor_WriteReg(0x12, val | 0x08);
	#endif
}


/*==============================================================================
 * Description:
 * read gain from sensor registers
 * please modify this function acording your spec
 *============================================================================*/
static uint16_t bf2205_front_read_gain(void)
{
	uint16_t gain_R=0;
	gain_R = Sensor_ReadReg(0x00);
	return gain_R;
	
	/*	
	uint16_t gain_h = 0;
	uint16_t gain_l = 0;

	gain_h = Sensor_ReadReg(0xYYYY) & 0xff;
	gain_l = Sensor_ReadReg(0xYYYY) & 0xff;

	return ((gain_h << 8) | gain_l);
	*/
	
}

/*==============================================================================
 * Description:
 * write gain to sensor registers
 * please modify this function acording your spec
 *============================================================================*/
static void bf2205_front_write_gain(uint32_t gain16)
{

// write gain, 16 = 1x
	uint16_t gain_buff=0;
	uint16_t iReg,temp,val;
	uint16_t gainMSB, gainLSB;
	//param : 1x = 16
	if (16*16 < gain16)
		gain16 = 255;
	if (1*16 > gain16)
		gain16 = 16;	  

	if(8*16 <= gain16) {
		gainMSB = 7;
	} else if (4*16 <= gain16) {
		gainMSB = 3;
	} else if (2*16 <= gain16){
		gainMSB = 1;
	} else {
		gainMSB = 0;
	}
	
	gainLSB = gain16 / (gainMSB + 1) - 16;
	if (gainLSB > 15) 
		gainLSB = 15;
		
////////////////group latch write gain ///////////////

		gain_buff = ((gainMSB << 4) + gainLSB );
		val = Sensor_ReadReg(0x12);
		if (val & 0x08) {
			SENSOR_PRINT_HIGH("SENSOR_bf2205_front: ============ write gain reg[0x12][3] not clear!! (0x%x)", val);
			Sensor_WriteReg(0x12 , 0xf7 & val );
			SENSOR_PRINT_HIGH("SENSOR_bf2205_front:== 0x%x",val);
		}
		Sensor_WriteReg(0xc0, 0x00); 
		Sensor_WriteReg(0xc1, gain_buff); 
		Sensor_WriteReg(0x12, (val | 0x08));	
		
/////////////////////////////////////////////////
		SENSOR_PRINT_HIGH("SENSOR_bf2205_front: _bf2205_front_set_gain16 gain16 = 0x%x, gainMSB,LSB = 0x%x,0x%x ", gain16,gainMSB,gainLSB);
	
}

/*==============================================================================
 * Description:
 * read frame length from sensor registers
 * please modify this function acording your spec
 *============================================================================*/
static uint16_t bf2205_front_read_frame_length(void)
{
	uint16_t frame_len_h = 0;
	uint16_t frame_len_l = 0;

	frame_len_h = Sensor_ReadReg(0x23) & 0xff;
	frame_len_l = Sensor_ReadReg(0x22) & 0xff;

	return ((frame_len_h << 8) | frame_len_l);
}

/*==============================================================================
 * Description:
 * write frame length to sensor registers
 * please modify this function acording your spec
 *============================================================================*/
static void bf2205_front_write_frame_length(uint32_t frame_len)
{
	Sensor_WriteReg(0x23, (frame_len >> 8) & 0xff);
	Sensor_WriteReg(0x22, frame_len & 0xff);
}

/*==============================================================================
 * Description:
 * read shutter from sensor registers
 * please modify this function acording your spec
 *============================================================================*/
static uint32_t bf2205_front_read_shutter(void)
{
	uint16_t shutter_h = 0;
	uint16_t shutter_l = 0;

	shutter_h = Sensor_ReadReg(0x02) & 0xff;
	shutter_l = Sensor_ReadReg(0x01) & 0xff;

	return (shutter_h << 8) | shutter_l;
}

/*==============================================================================
 * Description:
 * write shutter to sensor registers
 * please pay attention to the frame length
 * please modify this function acording your spec
 *============================================================================*/
static void bf2205_front_write_shutter(uint32_t shutter)
{
	Sensor_WriteReg(0x02, (shutter >> 8) & 0xff);
	Sensor_WriteReg(0x01, shutter & 0xff);
}

/*==============================================================================
 * Description:
 * write exposure to sensor registers and get current shutter
 * please pay attention to the frame length
 * please don't change this function if it's necessary
 *============================================================================*/
static uint16_t bf2205_front_update_exposure(uint32_t shutter,uint32_t dummy_line)
{
	uint32_t dest_fr_len = 0;
	uint32_t cur_fr_len = 0;
	uint32_t fr_len = s_current_default_frame_length;

      bf2205_front_group_hold_on();
	if (1 == SUPPORT_AUTO_FRAME_LENGTH)
		goto write_sensor_shutter;

	dest_fr_len = ((shutter + dummy_line+FRAME_OFFSET) > fr_len) ? (shutter +dummy_line+ FRAME_OFFSET) : fr_len;

	cur_fr_len = bf2205_front_read_frame_length();

	if (shutter < SENSOR_MIN_SHUTTER)
		shutter = SENSOR_MIN_SHUTTER;

	if (dest_fr_len != cur_fr_len)
		bf2205_front_write_frame_length(dest_fr_len);
write_sensor_shutter:
	/* write shutter to sensor registers */
	bf2205_front_write_shutter(shutter);
	return shutter;
}

/*==============================================================================
 * Description:
 * sensor power on
 * please modify this function acording your spec
 *============================================================================*/
static uint32_t bf2205_front_power_on(uint32_t power_on)
{
	SENSOR_AVDD_VAL_E dvdd_val = g_BF2205_front_mipi_raw_info.dvdd_val;
	SENSOR_AVDD_VAL_E avdd_val = g_BF2205_front_mipi_raw_info.avdd_val;
	SENSOR_AVDD_VAL_E iovdd_val = g_BF2205_front_mipi_raw_info.iovdd_val;
	BOOLEAN power_down = g_BF2205_front_mipi_raw_info.power_down_level;
	BOOLEAN reset_level = g_BF2205_front_mipi_raw_info.reset_pulse_level;

	if (SENSOR_TRUE == power_on) {
		Sensor_PowerDown(power_down);
		usleep(12 * 1000);
		Sensor_SetMonitorVoltage(SENSOR_AVDD_2800MV);
		Sensor_SetVoltage(dvdd_val, avdd_val, iovdd_val);
		usleep(20 * 1000);
	
		dw9714_init(2);		

		Sensor_SetMCLK(SENSOR_DEFALUT_MCLK);
		usleep(10 * 1000);
		Sensor_SetAvddVoltage(avdd_val);
		Sensor_SetDvddVoltage(dvdd_val);
		Sensor_SetIovddVoltage(iovdd_val);
		Sensor_PowerDown(!power_down);
		Sensor_SetResetLevel(!reset_level);
		usleep(10 * 1000);
		Sensor_SetMCLK(SENSOR_DEFALUT_MCLK);

		#ifndef CONFIG_CAMERA_AUTOFOCUS_NOT_SUPPORT
		Sensor_SetMonitorVoltage(SENSOR_AVDD_2800MV);
		usleep(5 * 1000);
		dw9714_init(2);
		#endif

	} else {

		#ifndef CONFIG_CAMERA_AUTOFOCUS_NOT_SUPPORT
		dw9714_deinit(2);
		Sensor_SetMonitorVoltage(SENSOR_AVDD_CLOSED);
		#endif

		Sensor_SetMCLK(SENSOR_DISABLE_MCLK);
		Sensor_SetResetLevel(reset_level);
		Sensor_PowerDown(power_down);
		Sensor_SetAvddVoltage(SENSOR_AVDD_CLOSED);
		Sensor_SetDvddVoltage(SENSOR_AVDD_CLOSED);
		Sensor_SetIovddVoltage(SENSOR_AVDD_CLOSED);
	}
	SENSOR_PRINT("(1:on, 0:off): %d", power_on);
	return SENSOR_SUCCESS;
}

#ifdef FEATURE_OTP

/*==============================================================================
 * Description:
 * get  parameters from otp
 * please modify this function acording your spec
 *============================================================================*/
static int bf2205_front_get_otp_info(struct otp_info_t *otp_info)
{
	uint32_t ret = SENSOR_FAIL;
	uint32_t i = 0x00;

	//identify otp information
	for (i = 0; i < NUMBER_OF_ARRAY(s_bf2205_front_raw_param_tab); i++) {
		SENSOR_PRINT("identify module_id=0x%x",s_bf2205_front_raw_param_tab[i].param_id);

		if(PNULL!=s_bf2205_front_raw_param_tab[i].identify_otp){
			//set default value;
			memset(otp_info, 0x00, sizeof(struct otp_info_t));

			if(SENSOR_SUCCESS==s_bf2205_front_raw_param_tab[i].identify_otp(otp_info)){
				if (s_bf2205_front_raw_param_tab[i].param_id== otp_info->module_id) {
					SENSOR_PRINT("identify otp sucess! module_id=0x%x",s_bf2205_front_raw_param_tab[i].param_id);
					ret = SENSOR_SUCCESS;
					break;
				}
				else{
					SENSOR_PRINT("identify module_id failed! table module_id=0x%x, otp module_id=0x%x",s_bf2205_front_raw_param_tab[i].param_id,otp_info->module_id);
				}
			}
			else{
				SENSOR_PRINT("identify_otp failed!");
			}
		}
		else{
			SENSOR_PRINT("no identify_otp function!");
		}
	}

	if (SENSOR_SUCCESS == ret)
		return i;
	else
		return -1;
}

/*==============================================================================
 * Description:
 * apply otp parameters to sensor register
 * please modify this function acording your spec
 *============================================================================*/
static uint32_t bf2205_front_apply_otp(struct otp_info_t *otp_info, int id)
{
	uint32_t ret = SENSOR_FAIL;
	//apply otp parameters
	SENSOR_PRINT("otp_table_id = %d", id);
	if (PNULL != s_bf2205_front_raw_param_tab[id].cfg_otp) {

		if(SENSOR_SUCCESS==s_bf2205_front_raw_param_tab[id].cfg_otp(otp_info)){
			SENSOR_PRINT("apply otp parameters sucess! module_id=0x%x",s_bf2205_front_raw_param_tab[id].param_id);
			ret = SENSOR_SUCCESS;
		}
		else{
			SENSOR_PRINT("update_otp failed!");
		}
	}else{
		SENSOR_PRINT("no update_otp function!");
	}

	return ret;
}

/*==============================================================================
 * Description:
 * cfg otp setting
 * please modify this function acording your spec
 *============================================================================*/
static uint32_t bf2205_front_cfg_otp(uint32_t param)
{
	uint32_t ret = SENSOR_FAIL;
	struct otp_info_t otp_info={0x00};
	int table_id = 0;

	table_id = bf2205_front_get_otp_info(&otp_info);
	if (-1 != table_id)
		ret = bf2205_front_apply_otp(&otp_info, table_id);

	//checking OTP apply result
	if (SENSOR_SUCCESS != ret) {//disable lsc
		Sensor_WriteReg(0xYYYY,0xYY);
	}
	else{//enable lsc
		Sensor_WriteReg(0xYYYY,0xYY);
	}

	return ret;
}
#endif

/*==============================================================================
 * Description:
 * identify sensor id
 * please modify this function acording your spec
 *============================================================================*/
static uint32_t bf2205_front_identify(uint32_t param)
{
	uint16_t pid_value = 0x00;
	uint16_t ver_value = 0x00;
	uint32_t ret_value = SENSOR_FAIL;

	SENSOR_PRINT("mipi raw identify");

	pid_value = Sensor_ReadReg(bf2205_front_PID_ADDR);

	if (bf2205_front_PID_VALUE == pid_value) {
		ver_value = Sensor_ReadReg(bf2205_front_VER_ADDR);
		SENSOR_PRINT("Identify: PID = %x, VER = %x", pid_value, ver_value);
		if (bf2205_front_VER_VALUE == ver_value) {
			#if defined(CONFIG_CAMERA_ISP_VERSION_V3) || defined(CONFIG_CAMERA_ISP_VERSION_V4)
			bf2205_front_InitRawTuneInfo();
			#endif
			ret_value = SENSOR_SUCCESS;
			SENSOR_PRINT_HIGH("this is bf2205_front sensor");
		} else {
			SENSOR_PRINT_HIGH("Identify this is %x%x sensor", pid_value, ver_value);
		}
	} else {
		SENSOR_PRINT_HIGH("identify fail, pid_value = %x", pid_value);
	}

	return ret_value;
}

/*==============================================================================
 * Description:
 * get resolution trim
 *
 *============================================================================*/
static unsigned long bf2205_front_get_resolution_trim_tab(uint32_t param)
{
	return (unsigned long) s_bf2205_front_resolution_trim_tab;
}

/*==============================================================================
 * Description:
 * before snapshot
 * you can change this function if it's necessary
 *============================================================================*/
static uint32_t bf2205_front_before_snapshot(uint32_t param)
{
	uint32_t cap_shutter = 0;
	uint32_t prv_shutter = 0;
	uint32_t gain = 0;
	uint32_t cap_gain = 0;
	uint32_t capture_mode = param & 0xffff;
	uint32_t preview_mode = (param >> 0x10) & 0xffff;

	uint32_t prv_linetime = s_bf2205_front_resolution_trim_tab[preview_mode].line_time;
	uint32_t cap_linetime = s_bf2205_front_resolution_trim_tab[capture_mode].line_time;

	s_current_default_frame_length = bf2205_front_get_default_frame_length(capture_mode);
	SENSOR_PRINT("capture_mode = %d", capture_mode);

	if (preview_mode == capture_mode) {
		cap_shutter =s_sensor_ev_info.preview_shutter;  //bf2205_front_read_shutter();
		cap_gain = s_sensor_ev_info.preview_gain;
		goto snapshot_info;
	}

	prv_shutter = s_sensor_ev_info.preview_shutter;	//bf2205_front_read_shutter();
	gain = s_sensor_ev_info.preview_gain;	//bf2205_front_read_gain();
//////////////////////////////Read register preview////////////////////////////////
{
		uint16_t i, j;
		uint16_t val[16];
		SENSOR_PRINT_HIGH("SENSOR_bf2205_front: before_snapshot sensor reg dump");
		SENSOR_PRINT_HIGH("SENSOR_bf2205_front:    : 00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f");
		SENSOR_PRINT_HIGH("SENSOR_bf2205_front:    -------------------------------------------------");
		for (i = 0; i < 16; i++) {
			for (j = 0; j < 16; j++) {			
				val[j] = Sensor_ReadReg(i*16+j);
			}
			SENSOR_PRINT_HIGH("SENSOR_bf2205_front: %02x : %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x",
			i*16,val[0],val[1],val[2],val[3],val[4],val[5],val[6],val[7],val[8],val[9],val[10],val[11],val[12],val[13],val[14],val[15]);
		}
	}
	SENSOR_PRINT_HIGH("preview gain = 0x%x ",prv_shutter);
//////////////////////////////////////////////////////////

	Sensor_SetMode(capture_mode);
	Sensor_SetMode_WaitDone();

	cap_shutter = prv_shutter * prv_linetime / cap_linetime * BINNING_FACTOR;

	while (gain >= (2 * SENSOR_BASE_GAIN)) {
		if (cap_shutter * 2 > s_current_default_frame_length)
			break;
		cap_shutter = cap_shutter * 2;
		gain = gain/2;
	}

	cap_shutter = bf2205_front_update_exposure(cap_shutter,0);
	cap_gain = gain;
	bf2205_front_write_gain(cap_gain);
	SENSOR_PRINT_HIGH("preview_shutter = 0x%x, preview_gain = 0x%x",
		     s_sensor_ev_info.preview_shutter, s_sensor_ev_info.preview_gain);

	SENSOR_PRINT("capture_shutter = 0x%x, capture_gain = 0x%x", cap_shutter, cap_gain);
snapshot_info:
	s_hdr_info.capture_shutter = cap_shutter; //bf2205_front_read_shutter();
	s_hdr_info.capture_gain = cap_gain; //bf2205_front_read_gain();
	/* limit HDR capture min fps to 10;
	 * MaxFrameTime = 1000000*0.1us;
	 */
	s_hdr_info.capture_max_shutter = 1000000 / cap_linetime;

	Sensor_SetSensorExifInfo(SENSOR_EXIF_CTRL_EXPOSURETIME, cap_shutter);
////////////////////////////Read register capture/////////////////////////////////
{
		uint16_t i, j;
		uint16_t val[16];
		SENSOR_PRINT_HIGH("SENSOR_bf2205_front: after_snapshot sensor reg dump");
		SENSOR_PRINT_HIGH("SENSOR_bf2205_front:    : 00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f");
		SENSOR_PRINT_HIGH("SENSOR_bf2205_front:    -------------------------------------------------");
		for (i = 0; i < 16; i++) {
			for (j = 0; j < 16; j++) {			
				val[j] = Sensor_ReadReg(i*16+j);
			}
			SENSOR_PRINT_HIGH("SENSOR_bf2205_front: %02x : %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x",
			i*16,val[0],val[1],val[2],val[3],val[4],val[5],val[6],val[7],val[8],val[9],val[10],val[11],val[12],val[13],val[14],val[15]);
		}
	}
////////////////////////////////////////////////////////////////////////////////
	return SENSOR_SUCCESS;
}

/*==============================================================================
 * Description:
 * get the shutter from isp
 * please don't change this function unless it's necessary
 *============================================================================*/
static uint32_t bf2205_front_write_exposure(uint32_t param)
{
	uint32_t ret_value = SENSOR_SUCCESS;
	uint16_t exposure_line = 0x00;
	uint16_t dummy_line = 0x00;
	uint16_t mode = 0x00;

	exposure_line = param & 0xffff;
	dummy_line = (param >> 0x10) & 0xfff; /*for cits frame rate test*/
	mode = (param >> 0x1c) & 0x0f;

	SENSOR_PRINT("current mode = %d, exposure_line = %d, dummy_line=%d", mode, exposure_line,dummy_line);
	s_current_default_frame_length = bf2205_front_get_default_frame_length(mode);

	s_sensor_ev_info.preview_shutter = bf2205_front_update_exposure(exposure_line,dummy_line);

	return ret_value;
}

/*==============================================================================
 * Description:
 * get the parameter from isp to real gain
 * you mustn't change the funcion !
 *============================================================================*/
static uint32_t isp_to_real_gain(uint32_t param)
{
	uint32_t real_gain = 0;

	
#if  defined(CONFIG_CAMERA_ISP_VERSION_V3) || defined(CONFIG_CAMERA_ISP_VERSION_V4)
	real_gain=param;
#else
	real_gain = ((param & 0xf) + 16) * (((param >> 4) & 0x01) + 1);
	real_gain = real_gain * (((param >> 5) & 0x01) + 1) * (((param >> 6) & 0x01) + 1);
	real_gain = real_gain * (((param >> 7) & 0x01) + 1) * (((param >> 8) & 0x01) + 1);
	real_gain = real_gain * (((param >> 9) & 0x01) + 1) * (((param >> 10) & 0x01) + 1);
	real_gain = real_gain * (((param >> 11) & 0x01) + 1);
#endif

	return real_gain;
}

/*==============================================================================
 * Description:
 * write gain value to sensor
 * you can change this function if it's necessary
 *============================================================================*/
static uint32_t bf2205_front_write_gain_value(uint32_t param)
{
	uint32_t ret_value = SENSOR_SUCCESS;
	uint32_t real_gain = 0;

	real_gain = isp_to_real_gain(param);

	real_gain = real_gain * SENSOR_BASE_GAIN / ISP_BASE_GAIN;

	SENSOR_PRINT("real_gain = 0x%x", real_gain);

	s_sensor_ev_info.preview_gain = real_gain;
	bf2205_front_write_gain(real_gain);

	return ret_value;
}

#ifndef CONFIG_CAMERA_AUTOFOCUS_NOT_SUPPORT
/*==============================================================================
 * Description:
 * write parameter to vcm
 * please add your VCM function to this function
 *============================================================================*/
static uint32_t bf2205_front_write_af(uint32_t param)
{
	return dw9714_write_af(param);
}
#endif

/*==============================================================================
 * Description:
 * increase gain or shutter for hdr
 *
 *============================================================================*/
static void bf2205_front_increase_hdr_exposure(uint8_t ev_multiplier)
{
	uint32_t shutter_multiply = s_hdr_info.capture_max_shutter / s_hdr_info.capture_shutter;
	uint32_t gain = 0;

	if (0 == shutter_multiply)
		shutter_multiply = 1;

	if (shutter_multiply >= ev_multiplier) {
		bf2205_front_update_exposure(s_hdr_info.capture_shutter * ev_multiplier,0);
		bf2205_front_write_gain(s_hdr_info.capture_gain);
	} else {
		gain = s_hdr_info.capture_gain * ev_multiplier / shutter_multiply;
		if (SENSOR_MAX_GAIN < gain)
			gain = SENSOR_MAX_GAIN;

		bf2205_front_update_exposure(s_hdr_info.capture_shutter * shutter_multiply,0);
		bf2205_front_write_gain(gain);
	}
}

/*==============================================================================
 * Description:
 * decrease gain or shutter for hdr
 *
 *============================================================================*/
static void bf2205_front_decrease_hdr_exposure(uint8_t ev_divisor)
{
	uint16_t gain_multiply = 0;
	uint32_t shutter = 0;
	gain_multiply = s_hdr_info.capture_gain / SENSOR_BASE_GAIN;

	if (gain_multiply >= ev_divisor) {
		bf2205_front_update_exposure(s_hdr_info.capture_shutter,0);
		bf2205_front_write_gain(s_hdr_info.capture_gain / ev_divisor);
	} else {
		shutter = s_hdr_info.capture_shutter * gain_multiply / ev_divisor;
        bf2205_front_update_exposure(shutter,0);
		bf2205_front_write_gain(s_hdr_info.capture_gain / gain_multiply);
		
	}
}

/*==============================================================================
 * Description:
 * set hdr ev
 * you can change this function if it's necessary
 *============================================================================*/
static uint32_t bf2205_front_set_hdr_ev(unsigned long param)
{
	uint32_t ret = SENSOR_SUCCESS;
	SENSOR_EXT_FUN_PARAM_T_PTR ext_ptr = (SENSOR_EXT_FUN_PARAM_T_PTR) param;

	uint32_t ev = ext_ptr->param;
	uint8_t ev_divisor, ev_multiplier;

	switch (ev) {
	case SENSOR_HDR_EV_LEVE_0:
		ev_divisor = 2;
		bf2205_front_decrease_hdr_exposure(ev_divisor);
		break;
	case SENSOR_HDR_EV_LEVE_1:
		ev_multiplier = 1;
		bf2205_front_increase_hdr_exposure(ev_multiplier);
		break;
	case SENSOR_HDR_EV_LEVE_2:
		ev_multiplier = 2;
		bf2205_front_increase_hdr_exposure(ev_multiplier);
		break;
	default:
		break;
	}
	return ret;
}

/*==============================================================================
 * Description:
 * extra functoin
 * you can add functions reference SENSOR_EXT_FUNC_CMD_E which from sensor_drv_u.h
 *============================================================================*/
static uint32_t bf2205_front_ext_func(unsigned long param)
{
	uint32_t rtn = SENSOR_SUCCESS;
	SENSOR_EXT_FUN_PARAM_T_PTR ext_ptr = (SENSOR_EXT_FUN_PARAM_T_PTR) param;

	SENSOR_PRINT("ext_ptr->cmd: %d", ext_ptr->cmd);
	switch (ext_ptr->cmd) {
	case SENSOR_EXT_EV:
		rtn = bf2205_front_set_hdr_ev(param);
		break;
	default:
		break;
	}

	return rtn;
}

/*==============================================================================
 * Description:
 * mipi stream on
 * please modify this function acording your spec
 *============================================================================*/
static uint32_t bf2205_front_stream_on(uint32_t param)
{
	int val;
	SENSOR_PRINT("SENSOR_bf2205_front: StreamOn");
	val = Sensor_ReadReg(0x12);
	val &= ~(0x40);

	Sensor_WriteReg(0x12, val);

	return 0;
}

/*==============================================================================
 * Description:
 * mipi stream off
 * please modify this function acording your spec
 *============================================================================*/
static uint32_t bf2205_front_stream_off(uint32_t param)
{
	int val;
	SENSOR_PRINT("SENSOR_bf2205_front: StreamOff");
	val = Sensor_ReadReg(0x12);
	val |= 0x40;
	Sensor_WriteReg(0x12, val);
	usleep(100*1000);
	return 0;
}

/*==============================================================================
 * Description:
 * all ioctl functoins
 * you can add functions reference SENSOR_IOCTL_FUNC_TAB_T from sensor_drv_u.h
 *
 * add ioctl functions like this:
 * .power = bf2205_front_power_on,
 *============================================================================*/
static SENSOR_IOCTL_FUNC_TAB_T s_bf2205_front_ioctl_func_tab = {
	.power = bf2205_front_power_on,
	.identify = bf2205_front_identify,
	.get_trim = bf2205_front_get_resolution_trim_tab,
	.before_snapshort = bf2205_front_before_snapshot,
	.write_ae_value = bf2205_front_write_exposure,
	.write_gain_value = bf2205_front_write_gain_value,
	#ifndef CONFIG_CAMERA_AUTOFOCUS_NOT_SUPPORT
	.af_enable = bf2205_front_write_af,
	#endif
	.set_focus = bf2205_front_ext_func,
	//.set_video_mode = bf2205_front_set_video_mode,
	.stream_on = bf2205_front_stream_on,
	.stream_off = bf2205_front_stream_off,
	#ifdef FEATURE_OTP
	.cfg_otp=bf2205_front_cfg_otp,
	#endif	

	//.group_hold_on = bf2205_front_group_hold_on,
	//.group_hold_of = bf2205_front_group_hold_off,
};
