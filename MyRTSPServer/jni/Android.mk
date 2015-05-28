LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := librtspjni

LOCAL_SRC_FILES := rtspjni.cpp \
					common/BufferQueue.cpp \

LOCAL_SHARED_LIBRARIES :=       \
	libdl 						\

LOCAL_LDLIBS :=-llog

LOCAL_STATIC_LIBRARIES += liblive555 \
							libavutil

include $(BUILD_SHARED_LIBRARY)

include $(call all-makefiles-under,$(LOCAL_PATH))
