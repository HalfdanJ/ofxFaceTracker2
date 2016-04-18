LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := dlib
APP_OPTIM := release

LOCAL_C_INCLUDE := $(LOCAL_PATH)/dlib
LOCAL_CFLAGS := -DDLIB_NO_GUI_SUPPORT -fexceptions -frtti  -fpic -ffunction-sections -funwind-tables -fno-strict-aliasing 
LOCAL_CFLAGS += -DNDEBUG
LOCAL_CFLAGS += -O2

FILE_LIST := $(wildcard $(LOCAL_PATH)/../dlib/all/source.cpp)

LOCAL_SRC_FILES := $(FILE_LIST:$(LOCAL_PATH)/%=%)

LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)
include $(BUILD_STATIC_LIBRARY)
