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
* File   : CedarXAdapterAndroid411_s.h
* Version: V1.0
* By     : 
* Date   : 2012-9-18
* Description:
********************************************************************************
*/
#ifndef _CEDARXADAPTERANDROID411_S_H_
#define _CEDARXADAPTERANDROID411_S_H_
//extern header
#include "CedarXPlayer.h"
#include "CedarXNativeRenderer.h"

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

//api

//internal shared header
#include "VirtualHwcomposer_ParamMapTable_Android411.h"

#endif  /* _CEDARXADAPTERANDROID411_S_H_ */

