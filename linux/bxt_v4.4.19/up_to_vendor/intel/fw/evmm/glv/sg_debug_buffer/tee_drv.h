/*******************************************************************************
* Copyright (c) 2015 Intel Corporation
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
*******************************************************************************/
#ifndef __TEE_DRV_H__
#define __TEE_DRV_H__

#define DEVICE_NAME "tee_drv"

#define VMM_NATIVE_VMCALL_SIGNATURE                                           \
    (  ((unsigned int)'$' << 24)                                              \
    |  ((unsigned int)'i' << 16)                                              \
    |  ((unsigned int)'M' << 8)                                               \
    |  ((unsigned int)'@' << 0)                                               \
    )

#define TEE_COMM_SUCCESS 0
#define TEE_COMM_FAIL 1

#define TRUSTY_VMCALL_DEBUG_BUFFER 0x74727504 // "tru" is 0x747275
#define DEBUG_BUF_SIZE          (16 * 4096)
#define MAX_CPU_SUPPORTED       80

/* Proc file name "/proc/tee_messages" */
#define PROCFS_NAME             "tee_messages"

typedef unsigned int UINT32;
typedef unsigned long long UINT64;

typedef struct _s_msg {
	UINT32 msg_size;
	UINT32 packed_1;			/* alignment required by TMSL */
	UINT64 data;
} s_msg;

#endif
