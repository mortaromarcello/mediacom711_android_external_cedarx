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
* File   : CedarXRecorderAdapterAndroid411.cpp
* Version: V1.0
* By     : 
* Date   : 2012-9-19
* Description:
********************************************************************************
*/
//#define LOG_NDEBUG 0  //#include <CDX_LogNDebug.h>
#define LOG_TAG "CedarXRecorderAdapterAndroid411.cpp"
#include <utils/Log.h>  //#include <CDX_Debug.h>

#include "CedarXAdapterAndroid411_s.h"
#include "CedarXRecorder.h"
#include <camera/ICamera.h>

#undef LOGV 
#undef LOGD 
#undef LOGI 
#undef LOGW 
#undef LOGE 

#define LOGV ALOGV
#define LOGD ALOGD
#define LOGI ALOGI
#define LOGW ALOGW
#define LOGE ALOGE

namespace android {

CedarXRecorderAdapter::CedarXRecorderAdapter(CedarXRecorder *recorder) : pCedarXRecorder(recorder)
{
    LOGV("Function[%s],File[%s],Line[%d]", __FUNCTION__, __FILE__, __LINE__);
}
CedarXRecorderAdapter::~CedarXRecorderAdapter()
{
    LOGV("Function[%s],File[%s],Line[%d]", __FUNCTION__, __FILE__, __LINE__);
}
int CedarXRecorderAdapter::CedarXRecorderAdapterIoCtrl(int cmd, int aux, void *pbuffer)  //cmd = CedarXRecorderAdapterCmd
{
    __s32   ret = OK;
    switch(cmd)
    {
        case CEDARXRECORDERADAPTER_CMD_NOTIFY_CAMERA_CEDARX_ENCODE:  //from android version 4.0.4, camera change its command.
        {
            ret = pCedarXRecorder->mCamera->sendCommand(CAMERA_CMD_SET_CEDARX_RECORDER, 0, 0);
            break;
        }
        default:
        {
            LOGW("unknown cmd[%d], Function[%s],File[%s],Line[%d]\n", cmd, __FUNCTION__, __FILE__, __LINE__);
            ret = UNKNOWN_ERROR;
            break;
        }
    }
    return ret;
}

}
