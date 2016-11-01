LOCAL_PATH:= $(call my-dir)

#
# libsensorcalitool - sensorhub apk library
#
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

#
# sensorcalitool - build exe for debug: use adb shell and print in terminal
#
#BUILD_EXE_FOR_DEBUG := true

ifdef BUILD_EXE_FOR_DEBUG
include $(CLEAR_VARS)

LOCAL_SRC_FILES:= \
	calibrate.cpp \
	sensorthread.cpp

LOCAL_SHARED_LIBRARIES := \
	libcutils \
	libutils \
	libbinder \
	liblog \
	libgui \
	libui

LOCAL_C_INCLUDES += \
	frameworks/base/nitive/include \

LOCAL_MODULE_TAGS := optional

LOCAL_MODULE:= sensorcalitool
LOCAL_CFLAGS += -DDEBUG_USE_ADB
include $(BUILD_EXECUTABLE)
endif
