/*
********************************************************************************
*                    Module
*                   SubModule
*                    SubSystem: Android
*                     moduleName: CedarX
*
*          (c) Copyright 2010-2012.
*               All Rights Reserved
*
* File   : CedarXAdapterAndroid404.cpp
* Version: V1.0
* By     : 
* Date   : 2012-9-18
* Description:
********************************************************************************
*/
//#define LOG_NDEBUG 0  //#include <CDX_LogNDebug.h>
#define LOG_TAG "CedarXAdapterAndroid404_V14"
#include <utils/Log.h>  //#include <CDX_Debug.h>

#include "CedarXAdapterAndroid404_s.h" 

namespace android {

/*******************************************************************************
Function name: android.CedarXPlayerAdapter.CedarXPlayerAdapterIoCtrl
Description: 
    
Parameters: 
    cmd:CedarXPlayerAdapterCmd
Return: 
    ref to frameworks/base/include/utils/Errors.h.
    OK;
    UNKNOWN_ERROR;
Time: 2012/9/18
*******************************************************************************/
int CedarXPlayerAdapter::CedarXPlayerAdapterIoCtrl(int cmd, int aux, void *pbuffer)
{
    int   ret = OK;
    switch(cmd)
    {
        case CEDARXPLAYERADAPTER_CMD_SETSCREEN_SPECIALPROCESS:  //from android version 4.0.4, don't need special process.
        {
            ret = pCedarXPlayer->mVideoRenderer->control(VIDEORENDER_CMD_SETSCREEN, aux);   //mScreenID = screen;
            //ret = pCedarXPlayer->stop();
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



CedarXPlayerAdapter::CedarXPlayerAdapter(struct CedarXPlayer *player) : pCedarXPlayer(player)
{
	LOGV("CedarXPlayerAdapter Construction. Function[%s],File[%s],Line[%d]", __FUNCTION__, __FILE__, __LINE__);
}
CedarXPlayerAdapter::~CedarXPlayerAdapter()
{
	LOGV("CedarXPlayerAdapter Destruction. Function[%s],File[%s],Line[%d]", __FUNCTION__, __FILE__, __LINE__);
}

} // namespace android

