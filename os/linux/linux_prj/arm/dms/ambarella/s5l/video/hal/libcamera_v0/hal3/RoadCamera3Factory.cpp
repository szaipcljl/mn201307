/* Copyright (c) 2018-2020, The Linux Foundataion. All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are
* met:
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above
*       copyright notice, this list of conditions and the following
*       disclaimer in the documentation and/or other materials provided
*       with the distribution.
*     * Neither the name of The Linux Foundation nor the names of its
*       contributors may be used to endorse or promote products derived
*       from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
* WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
* ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
* BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
* BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
* WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
* OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
* IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*/
#define LOG_TAG "RoadCamera3Factory"
//#define LOG_NDEBUG 0

#include <stdlib.h>
#include <utils/Log.h>
#include <utils/Errors.h>
#include <hardware/camera3.h>

#include "RoadCamera3Factory.h"

using namespace android;

namespace roadcamera {

RoadCamera3Factory gRoadCamera3Factory;

/*===========================================================================
 * FUNCTION   : RoadCamera3Factory
 *
 * DESCRIPTION: default constructor of RoadCamera3Factory
 *
 * PARAMETERS : none
 *
 * RETURN     : None
 *==========================================================================*/
RoadCamera3Factory::RoadCamera3Factory()
{
	camera_info info;
	//mNumOfCameras = RoadCamera3Setting::getNumberOfCameras();
	mStaticMetadata = NULL;
}


/*===========================================================================
 * FUNCTION   : ~RoadCamera3Factory
 *
 * DESCRIPTION: deconstructor of QCamera2Factory
 *
 * PARAMETERS : none
 *
 * RETURN     : None
 *==========================================================================*/
RoadCamera3Factory::~RoadCamera3Factory()
{
}

/*===========================================================================
 * FUNCTION   : get_number_of_cameras
 *
 * DESCRIPTION: static function to query number of cameras detected
 *
 * PARAMETERS : none
 *
 * RETURN     : number of cameras detected
 *==========================================================================*/
int RoadCamera3Factory::get_number_of_cameras()
{
	return gRoadCamera3Factory.getNumberOfCameras();
}

/*===========================================================================
 * FUNCTION   : get_camera_info
 *
 * DESCRIPTION: static function to query camera information with its ID
 *
 * PARAMETERS :
 *   @camera_id : camera ID
 *   @info      : ptr to camera info struct
 *
 * RETURN     : int32_t type of status
 *              NO_ERROR  -- success
 *              none-zero failure code
 *==========================================================================*/
int RoadCamera3Factory::get_camera_info(int camera_id, struct camera_info *info)
{
	return gRoadCamera3Factory.getCameraInfo(camera_id, info);
}

/*===========================================================================
 * FUNCTION   : getNumberOfCameras
 *
 * DESCRIPTION: query number of cameras detected
 *
 * PARAMETERS : none
 *
 * RETURN     : number of cameras detected
 *==========================================================================*/
int RoadCamera3Factory::getNumberOfCameras()
{
	return mNumOfCameras;
}

/*===========================================================================
 * FUNCTION   : getCameraInfo
 *
 * DESCRIPTION: query camera information with its ID
 *
 * PARAMETERS :
 *   @camera_id : camera ID
 *   @info      : ptr to camera info struct
 *
 * RETURN     : int32_t type of status
 *              NO_ERROR  -- success
 *              none-zero failure code
 *==========================================================================*/
int RoadCamera3Factory::getCameraInfo(int camera_id, struct camera_info *info)
{
	int rc;

	return rc;
}

int RoadCamera3Factory::set_callbacks(const camera_module_callbacks_t *callbacks)
{
    return 0;
}

void RoadCamera3Factory::get_vendor_tag_ops(vendor_tag_ops_t* ops)
{
}

/*===========================================================================
 * FUNCTION   : cameraDeviceOpen
 *
 * DESCRIPTION: open a camera device with its ID
 *
 * PARAMETERS :
 *   @camera_id : camera ID
 *   @hw_device : ptr to struct storing camera hardware device info
 *
 * RETURN     : int32_t type of status
 *              NO_ERROR  -- success
 *              none-zero failure code
 *==========================================================================*/
int RoadCamera3Factory::cameraDeviceOpen(int camera_id,
                    struct hw_device_t **hw_device)
{
	int rc = NO_ERROR;
	return rc;
}

/*===========================================================================
 * FUNCTION   : camera_device_open
 *
 * DESCRIPTION: static function to open a camera device by its ID
 *
 * PARAMETERS :
 *   @camera_id : camera ID
 *   @hw_device : ptr to struct storing camera hardware device info
 *
 * RETURN     : int32_t type of status
 *              NO_ERROR  -- success
 *              none-zero failure code
 *==========================================================================*/
int RoadCamera3Factory::camera_device_open(
		const struct hw_module_t *module, const char *id,
		struct hw_device_t **hw_device)
{
}

struct hw_module_methods_t RoadCamera3Factory::mModuleMethods = {
	open: RoadCamera3Factory::camera_device_open,
};

}; // namespace roadcamera
