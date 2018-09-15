LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES:= \
	sensor_profiler.cpp

LOCAL_SHARED_LIBRARIES := \
	libcutils \
	libutils \
	libbinder \
	liblog \
	libgui \
	libui

LOCAL_C_INCLUDES += \
	frameworks/base/nitive/include \

LOCAL_MODULE:= sensor_profiler

LOCAL_MODULE_TAGS := optional

include $(BUILD_EXECUTABLE)
