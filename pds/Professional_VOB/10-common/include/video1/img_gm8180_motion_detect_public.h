/******************************************************************************
模块名	    ：img_gm8180_motion_detect_public
文件名	    ：img_gm8180_motion_detect_public.h            
相关文件	：
文件实现功能：定义GM8180图像运动侦测参数结构体
作者		：zouwenyi
版本		：1.0
-------------------------------------------------------------------------------
修改记录:
日  期			版本			修改人		走读人    修改内容 
2008/11/25		1.0				邹文艺                 创建
******************************************************************************/
#ifndef _IMG_GM8180_MOTION_DETECT_PUBLIC_H_
#define _IMG_GM8180_MOTION_DETECT_PUBLIC_H_

#include "videodefines.h"

#ifdef __cplusplus
extern "C"
{
#endif

//GM8180运动侦测参数结构体
typedef struct  
{
    l32 l32MBWidth;       //输入图像宏块宽度
    l32 l32MBHeight;       //输入图像宏块高度
    l32 l32IsUseSad;      //use sad info flag(目前GM8180不支持，只能置0)
    l32 l32InterlaceMode; //interlace mode (1: interlaced; 0: non-interlaced)
    l32 l32MvThrehold;    //是否运动的阈值
    l32 l32MDInterval;    //进行运动侦测的间隔帧数
}TImgGM8180MotionDetectParam;

//GM8180运动侦测状态参数结构体
#define TImgGM8180MotionDetectStatusParam TImgGM8180MotionDetectParam
    
//GM8180运动侦测参数结构体
typedef struct  
{
    u8 *pu8MBInfo;     //模块信息指针(GM8180硬件提供的信息)
    u8 *pu8MotionInfo; //输出运动搜索信息(每个字节代表一个宏块是否运动的标志，
                       //0：静止，1：运动,按从左到右，从上到下顺序进行排列)
}TImgGM8180MotionDetectInput;

//GM8180运动侦测输出参数结构体
typedef struct
{
    u32 u32Reserved; //保留参数
}TImgGM8180MotionDetectOutput;

#ifdef __cplusplus
}
#endif

#endif //_IMG_GM8180_MOTION_DETECT_PUBLIC_H_
