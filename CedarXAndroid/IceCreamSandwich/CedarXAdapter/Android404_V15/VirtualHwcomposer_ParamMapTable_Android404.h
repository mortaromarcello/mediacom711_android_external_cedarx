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
* File   : VirtualHwcomposer_ParamMapTable_Android404.h
* Version: V1.0
* By     : 
* Date   : 2012-9-18
* Description:
********************************************************************************
*/
#ifndef _VIRTUALHWCOMPOSER_PARAMMAPTABLE_ANDROID404_H_
#define _VIRTUALHWCOMPOSER_PARAMMAPTABLE_ANDROID404_H_

namespace android {

typedef struct tag_HWC3DSrcModeMap
{
    int                     nHWC3DSrcMode;      // type=HWC_3D_SRC_MODE_FP, hardware\libhardware\include\hardware\hwcomposer.h
    VirtualHWC3DSrcMode     nHWC3DSrcModeVirtual;
}HWC3DSrcModeMap;
extern HWC3DSrcModeMap   HWC3DSrcMode_MapTable[];
extern int MapHWC3DSrcMode_Virtual2Arch(VirtualHWC3DSrcMode nHWC3DSrcModeVirtual);


typedef struct tag_HWCDisplayModeMap
{
    int                     nHWCDisplayMode;      // type=HWC_3D_OUT_MODE_2D, hardware\libhardware\include\hardware\hwcomposer.h
    VirtualHWCDisplayMode   nHWCDisplayModeVirtual;
}HWCDisplayModeMap;
extern HWCDisplayModeMap   HWCDisplayMode_MapTable[];
extern int MapHWCDisplayMode_Virtual2Arch(VirtualHWCDisplayMode nHWCDisplayModeVirtual);

}
#endif  /* _VIRTUALHWCOMPOSER_PARAMMAPTABLE_ANDROID404_H_ */

