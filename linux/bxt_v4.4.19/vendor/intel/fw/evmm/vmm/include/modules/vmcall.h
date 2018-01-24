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

#ifndef _VMCALL_H_
#define _VMCALL_H_

#ifndef MODULE_VMCALL
#error "MODULE_VMCALL is not defined"
#endif

#include "vmm_objects.h"

typedef enum {
	TRUSTY_VMCALL_SMC = 0x74727500,
#ifdef ENABLE_SGUEST_SMP
	TRUSTY_VMCALL_RESCHEDULE = 0x74727501,
#endif
	TRUSTY_VMCALL_PUTS = 0x74727502,
	TRUSTY_VMCALL_TIMER = 0x74727503,
	TRUSTY_VMCALL_DEBUG_BUFFER = 0x74727504,
	TRUSTY_VMCALL_PENDING_INTR = 0x74727505,
	TRUSTY_VMCALL_IRQ_DONE = 0x74727506,
	TRUSTY_VMCALL_DUMP_INIT = 0x74727507
}vmcall_id_t;


typedef void (*vmcall_handler_t) (guest_cpu_handle_t gcpu);

void vmcall_register(guest_id_t guest_id, uint32_t vmcall_id,
			 vmcall_handler_t handler);

#endif                          /* _VMCALL_H_ */
