/*
********************************************************************************
*                    Module
*                   SubModule
*                    SubSystem
*                     moduleName
*
*          (c) Copyright 2010-2012.
*               All Rights Reserved
*
* File   : CedarXNativeRendererAdapterAndroid404.cpp
* Version: V1.0
* By     : 
* Date   : 2012-9-18
* Description:
********************************************************************************
*/
//#define LOG_NDEBUG 0  //#include <CDX_LogNDebug.h>
#define LOG_TAG "CedarXNativeRendererAdapterAndroid404_V14"
#include <utils/Log.h>  //#include <CDX_Debug.h>

#include "CedarXAdapterAndroid404_s.h" 

namespace android {

static int convertVirtualVideo3DInfo2video3Dinfo_t(video3Dinfo_t *pDes, VirtualVideo3DInfo *pSrc)
{
    memset(pDes, 0, sizeof(video3Dinfo_t));
    pDes->width = pSrc->width;
	pDes->height = pSrc->height;
	pDes->format = pSrc->format;
	pDes->_3d_mode = MapHWC3DSrcMode_Virtual2Arch(pSrc->src_mode);
	pDes->display_mode = MapHWCDisplayMode_Virtual2Arch(pSrc->display_mode);

    pDes->is_mode_changed = 1;
    return OK;
}


int CedarXNativeRendererAdapter::CedarXNativeRendererAdapterIoCtrl(int cmd, int para, void *pbuffer)
{
    //cmd = enum VIDEORENDER_CMD
    int result;
    switch(cmd)
    {
	    case VIDEORENDER_CMD_ROTATION_DEG    :  //no use
	        return pCedarXNativeRenderer->mNativeWindow->perform(pCedarXNativeRenderer->mNativeWindow.get(), NATIVE_WINDOW_SETPARAMETER, HWC_LAYER_ROTATION_DEG, para);
    	case VIDEORENDER_CMD_DITHER          :  //no use
    	    return pCedarXNativeRenderer->mNativeWindow->perform(pCedarXNativeRenderer->mNativeWindow.get(), NATIVE_WINDOW_SETPARAMETER, HWC_LAYER_DITHER, para);
    	case VIDEORENDER_CMD_SETINITPARA     :  //no use
    	    return pCedarXNativeRenderer->mNativeWindow->perform(pCedarXNativeRenderer->mNativeWindow.get(), NATIVE_WINDOW_SETPARAMETER, HWC_LAYER_SETINITPARA, para);
    	case VIDEORENDER_CMD_SETVIDEOPARA    :  //no use
    	    return pCedarXNativeRenderer->mNativeWindow->perform(pCedarXNativeRenderer->mNativeWindow.get(), NATIVE_WINDOW_SETPARAMETER, HWC_LAYER_SETVIDEOPARA, para);
    	case VIDEORENDER_CMD_SETFRAMEPARA    :  //no use
            return pCedarXNativeRenderer->mNativeWindow->perform(pCedarXNativeRenderer->mNativeWindow.get(), NATIVE_WINDOW_SETPARAMETER, HWC_LAYER_SETFRAMEPARA, para);
    	case VIDEORENDER_CMD_GETCURFRAMEPARA :  //cur use
            return pCedarXNativeRenderer->mNativeWindow->perform(pCedarXNativeRenderer->mNativeWindow.get(), NATIVE_WINDOW_SETPARAMETER, HWC_LAYER_GETCURFRAMEPARA, para);
    	case VIDEORENDER_CMD_QUERYVBI        :  //no use
            return pCedarXNativeRenderer->mNativeWindow->perform(pCedarXNativeRenderer->mNativeWindow.get(), NATIVE_WINDOW_SETPARAMETER, HWC_LAYER_QUERYVBI, para);
    	case VIDEORENDER_CMD_SETSCREEN       :  //no use
            return pCedarXNativeRenderer->mNativeWindow->perform(pCedarXNativeRenderer->mNativeWindow.get(), NATIVE_WINDOW_SETPARAMETER, HWC_LAYER_SETSCREEN, para);
    	case VIDEORENDER_CMD_SHOW            :  //cur use
            return pCedarXNativeRenderer->mNativeWindow->perform(pCedarXNativeRenderer->mNativeWindow.get(), NATIVE_WINDOW_SETPARAMETER, HWC_LAYER_SHOW, para);
    	case VIDEORENDER_CMD_RELEASE         :  //no use
            return pCedarXNativeRenderer->mNativeWindow->perform(pCedarXNativeRenderer->mNativeWindow.get(), NATIVE_WINDOW_SETPARAMETER, HWC_LAYER_RELEASE, para);
    	case VIDEORENDER_CMD_SET3DMODE       :  //cur use
    	{
            //para = &VirtualVideo3DInfo
            video3Dinfo_t _3d_info;
            convertVirtualVideo3DInfo2video3Dinfo_t(&_3d_info, (VirtualVideo3DInfo*)para);
    	    return pCedarXNativeRenderer->mNativeWindow->perform(pCedarXNativeRenderer->mNativeWindow.get(), NATIVE_WINDOW_SETPARAMETER, HWC_LAYER_SET3DMODE, &_3d_info);
    	}
    	case VIDEORENDER_CMD_SETFORMAT       :  //no use
    	    return pCedarXNativeRenderer->mNativeWindow->perform(pCedarXNativeRenderer->mNativeWindow.get(), NATIVE_WINDOW_SETPARAMETER, HWC_LAYER_SETFORMAT, para);
    	case VIDEORENDER_CMD_VPPON           :  //cur use
    	    return pCedarXNativeRenderer->mNativeWindow->perform(pCedarXNativeRenderer->mNativeWindow.get(), NATIVE_WINDOW_SETPARAMETER, HWC_LAYER_VPPON, para);
    	case VIDEORENDER_CMD_VPPGETON        :  //no use
    	    return pCedarXNativeRenderer->mNativeWindow->perform(pCedarXNativeRenderer->mNativeWindow.get(), NATIVE_WINDOW_SETPARAMETER, HWC_LAYER_VPPGETON, para);
    	case VIDEORENDER_CMD_SETLUMASHARP    :  //cur use
    	    return pCedarXNativeRenderer->mNativeWindow->perform(pCedarXNativeRenderer->mNativeWindow.get(), NATIVE_WINDOW_SETPARAMETER, HWC_LAYER_SETLUMASHARP, para);
    	case VIDEORENDER_CMD_GETLUMASHARP    :  //no use
    	    return pCedarXNativeRenderer->mNativeWindow->perform(pCedarXNativeRenderer->mNativeWindow.get(), NATIVE_WINDOW_SETPARAMETER, HWC_LAYER_GETLUMASHARP, para);
    	case VIDEORENDER_CMD_SETCHROMASHARP  :  //cur use
    	    return pCedarXNativeRenderer->mNativeWindow->perform(pCedarXNativeRenderer->mNativeWindow.get(), NATIVE_WINDOW_SETPARAMETER, HWC_LAYER_SETCHROMASHARP, para);
    	case VIDEORENDER_CMD_GETCHROMASHARP  :  //no use
    	    return pCedarXNativeRenderer->mNativeWindow->perform(pCedarXNativeRenderer->mNativeWindow.get(), NATIVE_WINDOW_SETPARAMETER, HWC_LAYER_GETCHROMASHARP, para);
    	//case VIDEORENDER_CMD_SETWHITEEXTEN   :
    	//case VIDEORENDER_CMD_GETWHITEEXTEN   :
    	case VIDEORENDER_CMD_SETBLACKEXTEN   :  //cur use
            return pCedarXNativeRenderer->mNativeWindow->perform(pCedarXNativeRenderer->mNativeWindow.get(), NATIVE_WINDOW_SETPARAMETER, HWC_LAYER_SETBLACKEXTEN, para);
    	case VIDEORENDER_CMD_GETBLACKEXTEN   :  //no use
    		return pCedarXNativeRenderer->mNativeWindow->perform(pCedarXNativeRenderer->mNativeWindow.get(), NATIVE_WINDOW_SETPARAMETER, HWC_LAYER_GETBLACKEXTEN, para);
    	case VIDEORENDER_CMD_SET_CROP		:   //cur use, para = &android_native_rect_t
    		return pCedarXNativeRenderer->mNativeWindow->perform(pCedarXNativeRenderer->mNativeWindow.get(), NATIVE_WINDOW_SET_CROP,para);
    	default:
    		LOGW("undefined command[%x]!", cmd);
    		break;
	}

    return 0;
    
}


CedarXNativeRendererAdapter::CedarXNativeRendererAdapter(CedarXNativeRenderer *vRender) : pCedarXNativeRenderer(vRender)
{
    LOGD("CedarXNativeRendererAdapter constuct!Function[%s]File[%s]Line[%d]", __FUNCTION__, __FILE__, __LINE__);
}
CedarXNativeRendererAdapter::~CedarXNativeRendererAdapter()
{
    LOGD("CedarXNativeRendererAdapter constuct!Function[%s]File[%s]Line[%d]", __FUNCTION__, __FILE__, __LINE__);
}

}

