/******************************************************************************
模块名	    ：img_motionsurveil_public
文件名	    ：img_motionsurveil_public.h   
相关文件	：
文件实现功能：定义图像库参数结构体
作者		：zhaobo
版本		：1.0
-------------------------------------------------------------------------------
修改记录:
日  期	            版本		修改人   走读人     修改内容
2007/09/13		1.0	    zhaobo                创建
2009/03/12              1.1         王建                增加帧场类型参数
2009/06/06              1.2        邹文艺               修改移动侦测接口参数
******************************************************************************/
#ifndef _IMG_MOTIONSURVEIL_PUBLIC_H_
#define _IMG_MOTIONSURVEIL_PUBLIC_H_

#include "videodefines.h"

#ifdef __cplusplus
extern "C"
{
#endif

//移动侦测区域参数
typedef struct
{
    s16 s16Top;     //侦测区域垂直位置偏移（16的整倍数）
    s16 s16Left;    //侦测区域水平位置偏移（16的整倍数）
    s16 s16Width;   //侦测区域宽度（16的整倍数）
    s16 s16Height;  //侦测区域高度（16的整倍数）
}TImageMotionSurveilArea;

//运动侦侧的参数
typedef struct  
{
    l32 l32Width;		//图像的宽度
    l32 l32Height;		//图像的高度
    l32 l32YUVType;		//图像的图像类型
    l32 l32Interlace;           //图像的帧场类型(FRAME_FORMAT:帧格式；FIELD_FORMAT:场格式)
    l32 l32FineTuneLevel;       //移动侦测级别微调参数(范围为-10～+10,从更不灵敏到更灵敏变化,0表示移动侦测不进行微调)
    s16 s16SurveilLevel;        //侦测级别(数值越大越灵敏)
    s16 s16SurveilNum;          //侦测区域个数
    TImageMotionSurveilArea atMotionSurveilArea[MAX_MOTION_SURVEIL_NUM]; //移动侦测区域
    u32 u32Reserved;
}TImageMotionSurveilParam;

//运动侦测的状态结构体
#define TImageMotionSurveilStatusParam TImageMotionSurveilParam

//运动侦侧的输入参数
typedef struct
{
    u8 *pu8InCurImage;        //当前帧源图像
    u8 *pu8OutMotionInfo;     //输出运动信息的内存块地址
    u32 u32Reserved;
}TImageMotionSurveilInput;

//运动侦侧的输出参数
typedef struct
{
	u32 u32Reserved;		//保留参数
}TImageMotionSurveilOutput;

#ifdef __cplusplus
}
#endif

#endif //_IMG_MOTIONSURVEIL_PUBLIC_H_


