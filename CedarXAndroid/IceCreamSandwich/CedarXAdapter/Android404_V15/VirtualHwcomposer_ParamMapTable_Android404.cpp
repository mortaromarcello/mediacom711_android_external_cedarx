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
* File   : VirtualHwcomposer_ParamMapTable_Android404.cpp
* Version: V1.0
* By     : 
* Date   : 2012-9-18
* Description:
********************************************************************************
*/
#include "CedarXAdapterAndroid404_s.h"
#include "VirtualHwcomposer_ParamMapTable_Android404.h"

namespace android {

HWC3DSrcModeMap   HWC3DSrcMode_MapTable[] =
{
    {HWC_3D_SRC_MODE_TB  , VIRTUAL_HWC_3D_SRC_MODE_TB   },
    {HWC_3D_SRC_MODE_FP  , VIRTUAL_HWC_3D_SRC_MODE_FP   },
    {HWC_3D_SRC_MODE_SSF , VIRTUAL_HWC_3D_SRC_MODE_SSF  },
    {HWC_3D_SRC_MODE_SSH , VIRTUAL_HWC_3D_SRC_MODE_SSH  },
    {HWC_3D_SRC_MODE_LI  , VIRTUAL_HWC_3D_SRC_MODE_LI   },

    {HWC_3D_SRC_MODE_NORMAL, VIRTUAL_HWC_3D_SRC_MODE_NORMAL  },

    {HWC_3D_SRC_MODE_NORMAL, VIRTUAL_HWC_3D_SRC_MODE_UNKNOWN },
};
int MapHWC3DSrcMode_Virtual2Arch(VirtualHWC3DSrcMode nHWC3DSrcModeVirtual)
{
    __s32   i;
    for(i=0; ; i++)
    {
        if(HWC3DSrcMode_MapTable[i].nHWC3DSrcModeVirtual == VIRTUAL_HWC_3D_SRC_MODE_UNKNOWN)
        {
            //reach the tail of the index table
            LOGW("can't find map index, nHWC3DSrcModeVirtual[%x]\n", nHWC3DSrcModeVirtual);
            break;
        }
        if(HWC3DSrcMode_MapTable[i].nHWC3DSrcModeVirtual == nHWC3DSrcModeVirtual)
        {
            //find the media  file type
            break;
        }
    }
    return HWC3DSrcMode_MapTable[i].nHWC3DSrcMode;
}


HWCDisplayModeMap   HWCDisplayMode_MapTable[] =
{
    {HWC_3D_OUT_MODE_2D 		       ,    VIRTUAL_HWC_3D_OUT_MODE_2D 		        },
    {HWC_3D_OUT_MODE_HDMI_3D_1080P24_FP, 	VIRTUAL_HWC_3D_OUT_MODE_HDMI_3D_1080P24_FP 	},
    {HWC_3D_OUT_MODE_ANAGLAGH 	       , 	VIRTUAL_HWC_3D_OUT_MODE_ANAGLAGH 	        },
    {HWC_3D_OUT_MODE_ORIGINAL 	       , 	VIRTUAL_HWC_3D_OUT_MODE_ORIGINAL 	        },

    {HWC_3D_OUT_MODE_LI                , 	VIRTUAL_HWC_3D_OUT_MODE_LI                  },
    {HWC_3D_OUT_MODE_CI_1              , 	VIRTUAL_HWC_3D_OUT_MODE_CI_1                },
    {HWC_3D_OUT_MODE_CI_2              , 	VIRTUAL_HWC_3D_OUT_MODE_CI_2                },
    {HWC_3D_OUT_MODE_CI_3              , 	VIRTUAL_HWC_3D_OUT_MODE_CI_3                },
    {HWC_3D_OUT_MODE_CI_4              , 	VIRTUAL_HWC_3D_OUT_MODE_CI_4                },

    {HWC_3D_OUT_MODE_HDMI_3D_720P50_FP , 	VIRTUAL_HWC_3D_OUT_MODE_HDMI_3D_720P50_FP   },
    {HWC_3D_OUT_MODE_HDMI_3D_720P60_FP , 	VIRTUAL_HWC_3D_OUT_MODE_HDMI_3D_720P60_FP   },
    {HWC_3D_OUT_MODE_HDMI_3D_720P60_FP , 	VIRTUAL_HWC_3D_OUT_MODE_UNKNOWN   },
};
int MapHWCDisplayMode_Virtual2Arch(VirtualHWCDisplayMode nHWCDisplayModeVirtual)
{
    __s32   i;
    for(i=0; ; i++)
    {
        if(HWCDisplayMode_MapTable[i].nHWCDisplayModeVirtual == VIRTUAL_HWC_3D_OUT_MODE_UNKNOWN)
        {
            //reach the tail of the index table
            LOGW("can't find map index, nHWCDisplayModeVirtual[%x]\n", nHWCDisplayModeVirtual);
            break;
        }
        if(HWCDisplayMode_MapTable[i].nHWCDisplayModeVirtual == nHWCDisplayModeVirtual)
        {
            //find the media  file type
            break;
        }
    }
    return HWCDisplayMode_MapTable[i].nHWCDisplayMode;
}

}

