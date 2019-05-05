
ifeq ($(BUILD_CMR_FAULT_DETECTOR), y)
LOCAL_PATH	:= $(call my-dir)

include $(CLEAR_VARS)

#LIB_PATH := $(FAKEROOT_DIR)/usr/lib


APP_PATH := $(FAKEROOT_DIR)/usr/bin

LOCAL_TARGET	:= cmr_guarder_test

LOCAL_SRCS		:= $(LOCAL_PATH)/vin_cap.c \
					$(LOCAL_PATH)/cmr_guarder.c \
					$(LOCAL_PATH)/cmr_guarder_test.c

LOCAL_CFLAGS	:= -I$(LOCAL_PATH)/../include/ \
					-I$(LOCAL_PATH)/../../include

LOCAL_LIBS  := libfault_detector.so
LOCAL_LDFLAGS	:= -lpthread


include $(BUILD_APP)

.PHONY: $(LOCAL_TARGET)

$(LOCAL_TARGET): $(LOCAL_MODULE)
	@mkdir -p $(APP_PATH)/
	@cp -dpRf $< $(APP_PATH)/
	@echo "Build $@ Done."

$(call add-target-into-build, $(LOCAL_TARGET))

##############################################

LIB_PATH := $(FAKEROOT_DIR)/usr/lib

PLATFORM :=s5l

LOCAL_SRCS		:= $(LOCAL_PATH)/vin_cap.c \
					$(LOCAL_PATH)/cmr_guarder.c


LOCAL_CFLAGS	:= -I$(LOCAL_PATH)/../include/ \
					-I$(LOCAL_PATH)/../../include

LOCAL_LDFLAGS	:= -lpthread -lrt

LOCAL_TARGET	:= libcmr_guarder.so
LOCAL_SO_NAME = libcmr_guarder.so

include $(BUILD_APP)

.PHONY: $(LOCAL_TARGET)

$(LOCAL_TARGET): $(LOCAL_MODULE)
	@cp -dpRf $< $(LIB_PATH)/
	$(call prebuild-links)
	@echo "Build $@ Done."

$(call add-target-into-build, $(LOCAL_TARGET))


endif
