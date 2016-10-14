LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES:= \
	jni/SensorCaliJNI.cpp \
	calibrate.cpp \
	sensorthread.cpp

LOCAL_SHARED_LIBRARIES := \
	libcutils \
	libutils \
	libbinder \
	liblog \
	libgui \
	libui \
	libandroid_runtime

LOCAL_C_INCLUDES += \
	frameworks/base/nitive/include \

LOCAL_MODULE:= libsensorcalitool

LOCAL_MODULE_TAGS := optional
include $(BUILD_SHARED_LIBRARY)

#used for adb debug
#include $(BUILD_EXECUTABLE)
