/******************************************************************************
模块名	    ：img_common_public
文件名	    ：img_common_public.h            
相关文件	：
文件实现功能：定义图像库公共参数结构体
作者		：zwenyi
版本		：1.0
-------------------------------------------------------------------------------
修改记录:
日  期			版本			修改人		走读人    修改内容
2007/09/14		1.0				zwenyi                  创建
******************************************************************************/
#ifndef _IMG_COMMON_PUBLIC_H_
#define _IMG_COMMON_PUBLIC_H_

#include "videodefines.h"

#ifdef __cplusplus
extern "C"
{
#endif

//图像信息的结构体
typedef struct 
{
	u8 *pu8Y;            //图像的Y分量
	u8 *pu8U;            //图像的U分量
	u8 *pu8V;            //图像的V分量(U、V分量可以不赋值，直接赋以NULL，但注意这两者必须同时填或不填)
	l32 l32Width;        //图像的宽度
	l32 l32Height;       //图像的高度
	l32 l32YStride;      //图像的Y分量步长
	l32 l32UVStride;     //图像的UV分量步长
	l32 l32IsInterlace;  //图像的类型(帧格式或者场格式)
	l32 l32YUVType;      //图标图像格式(YUV422或者YUV420)
}TImageInfo;

#define MAX_BUF_GROUPS  64

typedef struct
{
    s16 s16Width;                   //本组BUF宽度
    s16 s16Height;                  //本组BUF高度
    l32 l32BufNums;                 //本组BUF数目
    u8 au8Idx[MAX_BUF_GROUPS];      //对应本组包含的解码器ID号列表
}TBufGroupInfo;

//输入参数保留字段扩展类型
typedef enum
{
    VidReserved_DebugAddress = 1, //调试类型
    VidReserved_Unknown
}TEnumVidInputReservedType;

//调试类型参数结构体
typedef struct 
{
    TEnumVidInputReservedType tVidReservedType; //输入参数扩展类型
    u32 u32DebugAddress;                        
}TVidInputReservedDebugAddress;

//版本信息输入结构
typedef struct
{
    void* pvVer;
    l32 l32BufLen; 
    u32 u32CodecType;   //编解码器（CODECTYPE），图像库（ImageProcessType）
}TVerInfoInput;

//版本信息输出结构
typedef struct
{
    l32 l32VerLen;
    u32 u32Reserve;    
}TVerInfoOutput;

//错误码解析信息输入结构
typedef struct
{
    l32 l32ErrCode;
    l32 l32BufLen; 
    u32 u32Reserved;
}TErrCodeInfoInput;

//错误码解析信息输出结构
typedef struct
{
    void* pvErrCodeInfo;
    u32 u32Reserved;    
}TErrCodeInfoOutput;

//调试信息输入结构
typedef struct
{
    s8 as8DebugInfo[256]; 
    u32 u32Reserved;
}TDebugInfoInput;


#ifdef __cplusplus
}
#endif

#endif //_IMG_IMAGELIB_H_
