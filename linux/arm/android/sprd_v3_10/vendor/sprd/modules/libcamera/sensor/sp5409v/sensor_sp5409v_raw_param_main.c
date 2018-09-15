/*
 * Copyright (C) 2012 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


/************************************************************************/


//#ifdef WIN32

#include "sensor_raw.h"

/* Begin Include */
#include "sensor_sp5409v_raw_param_common.c"
#include "sensor_sp5409v_raw_param_prv_0.c"
#include "sensor_sp5409v_raw_param_prv_1.c"
#include "sensor_sp5409v_raw_param_cap_0.c"
#include "sensor_sp5409v_raw_param_cap_1.c"
#include "sensor_sp5409v_raw_param_video_0.c"

/* End Include */

//#endif


/************************************************************************/


/* IspToolVersion=R1.15.4802_P6 */


/* Capture Sizes:
	1280x960,2592x1944
*/


/************************************************************************/


static struct sensor_raw_resolution_info_tab s_sp5409v_trim_info=
{
	0x00,
	{
		{0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
		{0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
		{0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
		{0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
		{0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
		{0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
		{0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
		{0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
		{0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
		{0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
	}
};


/************************************************************************/


static struct sensor_raw_ioctrl s_sp5409v_ioctrl=
{
	0,
	0,
	0
};


/************************************************************************/


static struct sensor_version_info s_sp5409v_version_info=
{
	0x00030004,
	sizeof(struct sensor_version_info),
	0x00
};


/************************************************************************/


static uint32_t s_sp5409v_libuse_info[]=
{
    0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
    0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
    0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
    0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
    0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
    0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
    0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
    0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000
};


/************************************************************************/


static struct sensor_raw_info s_sp5409v_mipi_raw_info=
{
	&s_sp5409v_version_info,
	{
		{s_sp5409v_tune_info_common, sizeof(s_sp5409v_tune_info_common)},
		{s_sp5409v_tune_info_prv_0, sizeof(s_sp5409v_tune_info_prv_0)},
		{s_sp5409v_tune_info_prv_1, sizeof(s_sp5409v_tune_info_prv_1)},
		{NULL, 0},
		{NULL, 0},
		{s_sp5409v_tune_info_cap_0, sizeof(s_sp5409v_tune_info_cap_0)},
		{s_sp5409v_tune_info_cap_1, sizeof(s_sp5409v_tune_info_cap_1)},
		{NULL, 0},
		{NULL, 0},
		{s_sp5409v_tune_info_video_0, sizeof(s_sp5409v_tune_info_video_0)},
		{NULL, 0},
		{NULL, 0},
		{NULL, 0},

	},
	&s_sp5409v_trim_info,
	&s_sp5409v_ioctrl,
	(struct sensor_libuse_info *)s_sp5409v_libuse_info,
	{
		&s_sp5409v_fix_info_common,
		&s_sp5409v_fix_info_prv_0,
		&s_sp5409v_fix_info_prv_1,
		NULL,
		NULL,
		&s_sp5409v_fix_info_cap_0,
		&s_sp5409v_fix_info_cap_1,
		NULL,
		NULL,
		&s_sp5409v_fix_info_video_0,
		NULL,
		NULL,
		NULL,

	},
	{
		{s_sp5409v_common_tool_ui_input, sizeof(s_sp5409v_common_tool_ui_input)},
		{s_sp5409v_prv_0_tool_ui_input, sizeof(s_sp5409v_prv_0_tool_ui_input)},
		{s_sp5409v_prv_1_tool_ui_input, sizeof(s_sp5409v_prv_1_tool_ui_input)},
		{NULL, 0},
		{NULL, 0},
		{s_sp5409v_cap_0_tool_ui_input, sizeof(s_sp5409v_cap_0_tool_ui_input)},
		{s_sp5409v_cap_1_tool_ui_input, sizeof(s_sp5409v_cap_1_tool_ui_input)},
		{NULL, 0},
		{NULL, 0},
		{s_sp5409v_video_0_tool_ui_input, sizeof(s_sp5409v_video_0_tool_ui_input)},
		{NULL, 0},
		{NULL, 0},
		{NULL, 0},

	}
};