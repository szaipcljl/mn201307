##
## History:
##    2018/08/16 - [Ning Ma] Created file
##
##    Copyright (c) 2018 Roadefend, Inc.
##

LOCAL_PATH := $(call my-dir)

# --
ifneq (,$(filter userdebug eng,$(TARGET_BUILD_VARIANT)))
init_options += \
    -DALLOW_LOCAL_PROP_OVERRIDE=1 \
    -DALLOW_PERMISSIVE_SELINUX=1 \
    -DREBOOT_BOOTLOADER_ON_PANIC=1 \
    -DDUMP_ON_UMOUNT_FAILURE=1
else
init_options += \
    -DALLOW_LOCAL_PROP_OVERRIDE=0 \
    -DALLOW_PERMISSIVE_SELINUX=0 \
    -DREBOOT_BOOTLOADER_ON_PANIC=0 \
    -DDUMP_ON_UMOUNT_FAILURE=0
endif

ifneq (,$(filter eng,$(TARGET_BUILD_VARIANT)))
init_options += \
    -DSHUTDOWN_ZERO_TIMEOUT=1
else
init_options += \
    -DSHUTDOWN_ZERO_TIMEOUT=0
endif

init_options += -DLOG_UEVENTS=0

init_cflags += \
    $(init_options) \
    -Wall -Wextra \
    -Wno-unused-parameter \
	-Wno-error=unused-variable \
    -Werror \
    -std=gnu++14 \
	-I$(LOCAL_PATH)/../base/include \
	-I$(LOCAL_PATH)/../libcutils/include

# --

include $(CLEAR_VARS)
UEVENTD_SEPARATED_BIN := y
LOCAL_CFLAGS := $(init_cflags)
LOCAL_SRCS := \
   $(LOCAL_PATH)/ueventd.cpp \
   $(LOCAL_PATH)/ueventd_parser.cpp \
   $(LOCAL_PATH)/devices.cpp \
   $(LOCAL_PATH)/util.cpp \
   $(LOCAL_PATH)/parser.cpp \
   $(LOCAL_PATH)/watchdogd.cpp \
   $(LOCAL_PATH)/../base/stringprintf.cpp \
   $(LOCAL_PATH)/../base/file.cpp \
   $(LOCAL_PATH)/../libcutils/uevent.c


ifeq ($(UEVENTD_SEPARATED_BIN), y)
# "UEVENTD_SEPARATED_BIN defined, build ueventd"
LOCAL_CFLAGS += -DUEVENTD_SEPARATED_BIN=1
LOCAL_TARGET := ueventd
else
# "UEVENTD_SEPARATED_BIN not defined, build init"
LOCAL_SRCS += $(LOCAL_PATH)/init.cpp
LOCAL_TARGET := init
endif



LOCAL_LIBS :=

include $(BUILD_APP)

.PHONY: $(LOCAL_TARGET)

$(LOCAL_TARGET): $(LOCAL_MODULE)
	@echo "Build $@ Done."

$(call add-target-into-build, $(LOCAL_TARGET))

