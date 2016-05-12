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
#include "sensor_bf2205_front_raw_param_common.c"
#include "sensor_bf2205_front_raw_param_prv_0.c"
#include "sensor_bf2205_front_raw_param_cap_0.c"
#include "sensor_bf2205_front_raw_param_video_0.c"

/* End Include */

//#endif


/************************************************************************/


/* IspToolVersion=1.15.46.2 */


/* Capture Sizes:
	1600x1200,1600x1200
*/


/************************************************************************/


static struct sensor_raw_resolution_info_tab s_bf2205_front_trim_info=
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


static struct sensor_raw_ioctrl s_bf2205_front_ioctrl=
{
	0,
	0,
	0
};


/************************************************************************/


static struct sensor_version_info s_bf2205_front_version_info=
{
	0x00030004,
	sizeof(struct sensor_version_info),
	0x00
};


/************************************************************************/


static uint32_t s_bf2205_front_libuse_info[]=
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


static struct sensor_raw_info s_bf2205_front_mipi_raw_info=
{
	&s_bf2205_front_version_info,
	{
		{s_bf2205_front_tune_info_common, sizeof(s_bf2205_front_tune_info_common)},
		{s_bf2205_front_tune_info_prv_0, sizeof(s_bf2205_front_tune_info_prv_0)},
		{NULL, 0},
		{NULL, 0},
		{NULL, 0},
		{s_bf2205_front_tune_info_cap_0, sizeof(s_bf2205_front_tune_info_cap_0)},
		{NULL, 0},
		{NULL, 0},
		{NULL, 0},
		{s_bf2205_front_tune_info_video_0, sizeof(s_bf2205_front_tune_info_video_0)},
		{NULL, 0},
		{NULL, 0},
		{NULL, 0},

	},
	&s_bf2205_front_trim_info,
	&s_bf2205_front_ioctrl,
	(struct sensor_libuse_info *)s_bf2205_front_libuse_info,
	{
		&s_bf2205_front_fix_info_common,
		&s_bf2205_front_fix_info_prv_0,
		NULL,
		NULL,
		NULL,
		&s_bf2205_front_fix_info_cap_0,
		NULL,
		NULL,
		NULL,
		&s_bf2205_front_fix_info_video_0,
		NULL,
		NULL,
		NULL,

	},
	{
		{s_bf2205_front_common_tool_ui_input, sizeof(s_bf2205_front_common_tool_ui_input)},
		{s_bf2205_front_prv_0_tool_ui_input, sizeof(s_bf2205_front_prv_0_tool_ui_input)},
		{NULL, 0},
		{NULL, 0},
		{NULL, 0},
		{s_bf2205_front_cap_0_tool_ui_input, sizeof(s_bf2205_front_cap_0_tool_ui_input)},
		{NULL, 0},
		{NULL, 0},
		{NULL, 0},
		{s_bf2205_front_video_0_tool_ui_input, sizeof(s_bf2205_front_video_0_tool_ui_input)},
		{NULL, 0},
		{NULL, 0},
		{NULL, 0},

	}
};