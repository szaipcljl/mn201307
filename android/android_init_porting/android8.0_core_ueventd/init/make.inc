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

# If building on Linux, then build unit test for the host.
ifeq ($(HOST_OS),linux)
include $(CLEAR_VARS)
LOCAL_CPPFLAGS := $(init_cflags)
LOCAL_SRC_FILES:= \
    parser/tokenizer.cpp \

LOCAL_MODULE := libinit_parser
LOCAL_CLANG := true
include $(BUILD_HOST_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := init_parser_tests
LOCAL_SRC_FILES := \
    parser/tokenizer_test.cpp \

LOCAL_STATIC_LIBRARIES := libinit_parser
LOCAL_CLANG := true
include $(BUILD_HOST_NATIVE_TEST)
endif


###

LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_CFLAGS := $(init_cflags)
LOCAL_SRCS:= \
   $(LOCAL_PATH)/action.cpp \
   $(LOCAL_PATH)/capabilities.cpp \
   $(LOCAL_PATH)/descriptors.cpp \
   $(LOCAL_PATH)/import_parser.cpp \
   $(LOCAL_PATH)/init_parser.cpp \
   $(LOCAL_PATH)/log.cpp \
   $(LOCAL_PATH)/parser.cpp \
   $(LOCAL_PATH)/service.cpp \
   $(LOCAL_PATH)/util.cpp \

#LOCAL_LIBS	:= libbase.a libselinux.a liblog.a libprocessgroup.a libcap.a
LOCAL_LIBS	:= libbase.a libselinux.a liblog.a libprocessgroup.a libcap.a
LOCAL_TARGET := libinit.a
#LOCAL_SANITIZE := integer
#LOCAL_CLANG := true
include $(BUILD_APP)

.PHONY: $(LOCAL_TARGET)

$(LOCAL_TARGET): $(LOCAL_MODULE)
	@echo "Build $@ Done."

$(call add-target-into-build, $(LOCAL_TARGET))

###############

include $(CLEAR_VARS)
LOCAL_CFLAGS := $(init_cflags)
LOCAL_SRCS := \
   $(LOCAL_PATH)/bootchart.cpp \
   $(LOCAL_PATH)/builtins.cpp \
   $(LOCAL_PATH)/devices.cpp \
   $(LOCAL_PATH)/init.cpp \
   $(LOCAL_PATH)/init_first_stage.cpp \
   $(LOCAL_PATH)/keychords.cpp \
   $(LOCAL_PATH)/property_service.cpp \
   $(LOCAL_PATH)/reboot.cpp \
   $(LOCAL_PATH)/signal_handler.cpp \
   $(LOCAL_PATH)/ueventd.cpp \
   $(LOCAL_PATH)/ueventd_parser.cpp \
   $(LOCAL_PATH)/watchdogd.cpp \

LOCAL_TARGET:= init
LOCAL_C_INCLUDES += \
    system/core/mkbootimg

LOCAL_LIBS := \
    libinit.a \
    libbootloader_message.a \
    libfs_mgr.a \
    libfec.a \
    libfec_rs.a \
    libsquashfs_utils.a \
    liblogwrap.a \
    libext4_utils.a \
    libcutils.a \
    libbase.a \
    libc.a \
    libselinux.a \
    liblog.a \
    libcrypto_utils.a \
    libcrypto.a \
    libc++_static.a \
    libdl.a \
    libsparse.a \
    libz.a \
    libprocessgroup.a \
    libavb.a


include $(BUILD_APP)

.PHONY: $(LOCAL_TARGET)

$(LOCAL_TARGET): $(LOCAL_MODULE)
	@echo "Build $@ Done."

$(call add-target-into-build, $(LOCAL_TARGET))



#=================================
include $(CLEAR_VARS)
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
   $(LOCAL_PATH)/../libcutils/uevent.c \
   $(LOCAL_PATH)/init.cpp


LOCAL_TARGET := sinit

LOCAL_LIBS :=

include $(BUILD_APP)

.PHONY: $(LOCAL_TARGET)

$(LOCAL_TARGET): $(LOCAL_MODULE)
	@echo "Build $@ Done."

$(call add-target-into-build, $(LOCAL_TARGET))

