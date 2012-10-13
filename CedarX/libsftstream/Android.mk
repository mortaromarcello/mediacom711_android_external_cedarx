LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

include $(LOCAL_PATH)/../../Config.mk

LOCAL_SRC_FILES:=            \
	sft_http_stream.cpp


LOCAL_C_INCLUDES:= \
	$(JNI_H_INCLUDE) \
	$(TOP)/frameworks/${AV_BASE_PATH}/include \
    $(TOP)/frameworks/${AV_BASE_PATH}/media/libstagefright/include \
    ${CEDARX_TOP}/include \
    ${CEDARX_TOP}/include/include_demux \
    ${CEDARX_TOP}/include/include_stream \
    ${CEDARX_TOP}/include/include_omx \
    ${CEDARX_TOP}/include/include_cedarv
    
LOCAL_SHARED_LIBRARIES += \
        libbinder         \
        libmedia          \
        libutils          \
        libcutils         \
        libstagefright_foundation \
        libstagefright \
        libicuuc \
		libswdrm
		        
LOCAL_CFLAGS += $(CEDARX_EXT_CFLAGS)

LOCAL_MODULE_TAGS := optional
LOCAL_PRELINK_MODULE := false

LOCAL_MODULE:= libcedarxsftstream

ifeq ($(TARGET_ARCH),arm)
    LOCAL_CFLAGS += -Wno-psabi
endif

include $(BUILD_SHARED_LIBRARY)