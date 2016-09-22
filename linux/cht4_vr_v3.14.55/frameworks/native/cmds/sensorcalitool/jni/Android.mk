LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES:= \
	SensorCaliJNI.cpp \
	calibrate.cpp \
	mySensorManager.cpp

LOCAL_SHARED_LIBRARIES := \
	libcutils \
	libutils \
	libbinder \
	liblog \
    libandroid_runtime \
	libgui \
	libui

LOCAL_C_INCLUDES += \
	frameworks/base/nitive/include \
	
LOCAL_MODULE:= libsensorcalitool

LOCAL_MODULE_TAGS := optional
include $(BUILD_SHARED_LIBRARY)
