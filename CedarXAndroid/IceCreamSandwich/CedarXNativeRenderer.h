/*
 * Copyright (C) 2009 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef CEDARXNATIVE_RENDERER_H_

#define CEDARXNATIVE_RENDERER_H_


#include <utils/RefBase.h>
#if (CEDARX_ANDROID_VERSION < 7)
#include <ui/android_native_buffer.h>
#endif
#include <hardware/hwcomposer.h>

namespace android {

typedef enum tag_VirtualHWC3DSrcMode  
{
    VIRTUAL_HWC_3D_SRC_MODE_TB  = 0x0,//top bottom, 
    VIRTUAL_HWC_3D_SRC_MODE_FP  = 0x1,//frame packing
    VIRTUAL_HWC_3D_SRC_MODE_SSF = 0x2,//side by side full
    VIRTUAL_HWC_3D_SRC_MODE_SSH = 0x3,//side by side half
    VIRTUAL_HWC_3D_SRC_MODE_LI  = 0x4,//line interleaved

    VIRTUAL_HWC_3D_SRC_MODE_NORMAL = 0xFF, //2d
    VIRTUAL_HWC_3D_SRC_MODE_UNKNOWN = -1,
}VirtualHWC3DSrcMode;   //virtual to HWC_3D_SRC_MODE_TB

typedef enum tag_VirtualHWCDisplayMode  
{
    VIRTUAL_HWC_3D_OUT_MODE_2D 		            = 0x0,//left picture
    VIRTUAL_HWC_3D_OUT_MODE_HDMI_3D_1080P24_FP 	= 0x1,
    VIRTUAL_HWC_3D_OUT_MODE_ANAGLAGH 	        = 0x2,//
    VIRTUAL_HWC_3D_OUT_MODE_ORIGINAL 	        = 0x3,//original pixture

    VIRTUAL_HWC_3D_OUT_MODE_LI                  = 0x4,//line interleaved
    VIRTUAL_HWC_3D_OUT_MODE_CI_1                = 0x5,//column interlaved 1
    VIRTUAL_HWC_3D_OUT_MODE_CI_2                = 0x6,//column interlaved 2
    VIRTUAL_HWC_3D_OUT_MODE_CI_3                = 0x7,//column interlaved 3
    VIRTUAL_HWC_3D_OUT_MODE_CI_4                = 0x8,//column interlaved 4

    VIRTUAL_HWC_3D_OUT_MODE_HDMI_3D_720P50_FP   = 0x9,
    VIRTUAL_HWC_3D_OUT_MODE_HDMI_3D_720P60_FP   = 0xa,
    VIRTUAL_HWC_3D_OUT_MODE_UNKNOWN     = -1,
}VirtualHWCDisplayMode;   //virtual to HWC_3D_OUT_MODE_ANAGLAGH

typedef struct tag_VirtualVideo3DInfo
{
	unsigned int        width;
	unsigned int        height;
	unsigned int        format; // HWC_FORMAT_MBYUV422(hwcomposer.h) or HAL_PIXEL_FORMAT_YV12(graphics.h).
	VirtualHWC3DSrcMode     src_mode;   //virtual to HWC_3D_SRC_MODE_TB
	VirtualHWCDisplayMode   display_mode;   //virtual to HWC_3D_OUT_MODE_ANAGLAGH(hwcomposer.h)
}VirtualVideo3DInfo;    //virtual to video3Dinfo_t

enum VIDEORENDER_CMD
{
	VIDEORENDER_CMD_UNKOWN = 0,
    VIDEORENDER_CMD_ROTATION_DEG    = 1, //HWC_LAYER_ROTATION_DEG    ,
    VIDEORENDER_CMD_DITHER          = 3, //HWC_LAYER_DITHER          ,
    VIDEORENDER_CMD_SETINITPARA     = 4, //HWC_LAYER_SETINITPARA     ,
    VIDEORENDER_CMD_SETVIDEOPARA    = 5, //HWC_LAYER_SETVIDEOPARA    ,
    VIDEORENDER_CMD_SETFRAMEPARA    = 6, //HWC_LAYER_SETFRAMEPARA    ,
    VIDEORENDER_CMD_GETCURFRAMEPARA = 7, //HWC_LAYER_GETCURFRAMEPARA ,
    VIDEORENDER_CMD_QUERYVBI        = 8, //HWC_LAYER_QUERYVBI        ,
    VIDEORENDER_CMD_SETSCREEN       = 9, //HWC_LAYER_SETMODE       ,
    VIDEORENDER_CMD_SHOW            = 0xa, //HWC_LAYER_SHOW            ,
    VIDEORENDER_CMD_RELEASE         = 0xb, //HWC_LAYER_RELEASE         ,
    VIDEORENDER_CMD_SET3DMODE       = 0xc, //HWC_LAYER_SET3DMODE       ,
    VIDEORENDER_CMD_SETFORMAT       = 0xd, //HWC_LAYER_SETFORMAT       ,
    VIDEORENDER_CMD_VPPON           = 0xe, //HWC_LAYER_VPPON           ,
    VIDEORENDER_CMD_VPPGETON        = 0xf, //HWC_LAYER_VPPGETON        ,
    VIDEORENDER_CMD_SETLUMASHARP    = 0x10,//HWC_LAYER_SETLUMASHARP    ,
    VIDEORENDER_CMD_GETLUMASHARP    = 0x11, //HWC_LAYER_GETLUMASHARP    ,
    VIDEORENDER_CMD_SETCHROMASHARP  = 0x12, //HWC_LAYER_SETCHROMASHARP  ,
    VIDEORENDER_CMD_GETCHROMASHARP  = 0x13,//HWC_LAYER_GETCHROMASHARP  ,
    VIDEORENDER_CMD_SETWHITEEXTEN   = 0x14, //HWC_LAYER_SETWHITEEXTEN   ,
    VIDEORENDER_CMD_GETWHITEEXTEN   = 0x15, //HWC_LAYER_GETWHITEEXTEN   ,
    VIDEORENDER_CMD_SETBLACKEXTEN   = 0x16, //HWC_LAYER_SETBLACKEXTEN   ,
    VIDEORENDER_CMD_GETBLACKEXTEN   = 0x17, //HWC_LAYER_GETBLACKEXTEN   ,
    VIDEORENDER_CMD_SET_CROP		= 0x1000         ,
};

struct MetaData;
class CedarXNativeRendererAdapter;

class CedarXNativeRenderer {
public:
    CedarXNativeRenderer(
            const sp<ANativeWindow> &nativeWindow, const sp<MetaData> &meta);

    ~CedarXNativeRenderer();

    void render(
            const void *data, size_t size, void *platformPrivate);

    int control(int cmd, int para);

private:
    //OMX_COLOR_FORMATTYPE mColorFormat;
    sp<ANativeWindow> mNativeWindow;
    int32_t mWidth, mHeight;
    int32_t mCropLeft, mCropTop, mCropRight, mCropBottom;
    int32_t mCropWidth, mCropHeight;

    CedarXNativeRenderer(const CedarXNativeRenderer &);
    CedarXNativeRenderer &operator=(const CedarXNativeRenderer &);
    
    friend class CedarXNativeRendererAdapter;
    CedarXNativeRendererAdapter *pCedarXNativeRendererAdapter;
};


class CedarXNativeRendererAdapter  //base adapter, its implementation is put in different AdapterDirectories, selected to compile by makefile according to android version.
{
public:
    CedarXNativeRendererAdapter(CedarXNativeRenderer *vRender);
    virtual ~CedarXNativeRendererAdapter();
    int CedarXNativeRendererAdapterIoCtrl(int cmd, int aux, void *pbuffer);  //cmd = CedarXNativeRendererAdapterCmd
    
private:
    CedarXNativeRenderer* const pCedarXNativeRenderer; //CedarXNativeRenderer pointer
};

}  // namespace android

#endif  // SOFTWARE_RENDERER_H_
