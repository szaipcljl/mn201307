################################################################################
# Copyright (c) 2015 Intel Corporation
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
################################################################################

CSOURCES = $(wildcard *.c)

SUBDIRS =

ifneq ($(findstring MODULE_ACPI,$(EVMM_CMPL_FLAGS)), )
SUBDIRS += acpi
endif

ifneq ($(findstring MODULE_ACPI_PM,$(EVMM_CMPL_FLAGS)), )
SUBDIRS += acpi_pm
endif

ifneq ($(findstring MODULE_VMCALL,$(EVMM_CMPL_FLAGS)), )
SUBDIRS += vmcall
endif

ifneq ($(findstring MODULE_DEADLOOP,$(EVMM_CMPL_FLAGS)), )
SUBDIRS += deadloop
endif

ifneq ($(findstring MODULE_LOCAL_APIC,$(EVMM_CMPL_FLAGS)), )
SUBDIRS += local_apic
endif

ifneq ($(findstring MODULE_IPC,$(EVMM_CMPL_FLAGS)), )
SUBDIRS += ipc
endif

ifneq ($(findstring MODULE_VMX_TIMER,$(EVMM_CMPL_FLAGS)), )
SUBDIRS += vmx_timer
endif

ifneq ($(findstring MODULE_TRUSTY_GUEST,$(EVMM_CMPL_FLAGS)), )
SUBDIRS += trusty_guest
endif

ifneq ($(findstring MODULE_VTD,$(EVMM_CMPL_FLAGS)), )
SUBDIRS += vtd
endif

ifneq ($(findstring MODULE_MSR_ISOLATION,$(EVMM_CMPL_FLAGS)), )
SUBDIRS += msr_isolation
endif

ifneq ($(findstring MODULE_DEVICE_PROT,$(EVMM_CMPL_FLAGS)), )
SUBDIRS += device_prot
endif

# since it is difficult to enable -Wall for all files now, do it in
# modules first
# "modules" folder should be the last folder in core/Makefile to avoid
# impacts to non-module folder/files
# TODO: remove below line and add -Wall in rule.linux to make -Wall
# available for all files

EVMM_CMPL_FLAGS += -Wall

include $(PROJS)/rule.linux
