LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := optional
LOCAL_MODULE:= ftmsensor
LOCAL_SRC_FILES:= sensor.cpp
LOCAL_SHARED_LIBRARIES := libcutils libhardware
include $(BUILD_EXECUTABLE)
