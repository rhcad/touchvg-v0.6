LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := skiaview
LOCAL_SRC_FILES := skiaview.cpp

include $(BUILD_SHARED_LIBRARY)
