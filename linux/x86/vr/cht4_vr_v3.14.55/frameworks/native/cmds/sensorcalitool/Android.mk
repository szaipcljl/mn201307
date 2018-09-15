LOCAL_PATH:= $(call my-dir)

#
# libsensorcalitool - sensorhub apk library
#
include $(CLEAR_VARS)

include external/stlport/libstlport.mk

LOCAL_SRC_FILES:= \
	jni/SensorCaliJNI.cpp \
	calibrate.cpp \
	circ_buf.cpp\
	sensorthread.cpp

LOCAL_SHARED_LIBRARIES := \
	libcutils \
	libutils \
	libbinder \
	liblog \
	libgui \
	libui \
	libstlport\
	libandroid_runtime

LOCAL_C_INCLUDES += \
	frameworks/base/nitive/include \
	$(call include-path-for, stlport) \
	$(call include-path-for, stlport)/stl \
	$(call include-path-for, stlport)/using/h/

LOCAL_MODULE:= libsensorcalitool

LOCAL_MODULE_TAGS := optional
include $(BUILD_SHARED_LIBRARY)

#
# sensorcalitool - build exe for debug: use adb shell and print in terminal
#
BUILD_EXE_FOR_DEBUG := true

ifdef BUILD_EXE_FOR_DEBUG
include $(CLEAR_VARS)

include external/stlport/libstlport.mk
LOCAL_SRC_FILES:= \
	calibrate.cpp \
	circ_buf.cpp\
	sensorthread.cpp

LOCAL_SHARED_LIBRARIES := \
	libcutils \
	libutils \
	libbinder \
	liblog \
	libgui \
	libstlport\
	libui

LOCAL_C_INCLUDES += \
	frameworks/base/nitive/include \

LOCAL_MODULE_TAGS := optional

LOCAL_MODULE:= sensorcalitool
LOCAL_CFLAGS += -DDEBUG_USE_ADB
include $(BUILD_EXECUTABLE)
endif
