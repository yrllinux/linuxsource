/*****************************************************************************
模块名      : 
文件名      : xdm_imgproc_com.h
相关文件    : 
文件实现功能: 
作者        : 王建
版本        : V1.0  Copyright(C) 2006-2007 KDC, All rights reserved.
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2007/2/28   1.0          王建         创建
******************************************************************************/
#ifndef  XDM_IMGPROC_COM_H_
#define  XDM_IMGPROC_COM_H_

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef CHAR_BIT
   #define CHAR_BIT 8
#endif

/* : IIMGDEC_Cmd */
enum ImgProcCmdId
{
    IMG_PROC_SET_PARAMS = 271,
    IMG_PROC_GET_STATIS,
    IMG_PROC_GET_STATUS,
    IMG_PROC_PARSE_ERR_CODE,
    IMG_PROC_GET_VER,
    IMG_PROC_DEBUG		
};

/*pub;ic:IIMGDEC_Params*/
typedef struct
{
	IVIDDEC_Params img_Params;
	XDAS_UInt32	u32ImgProcType; //图像处理类型
	XDAS_UInt32 u32ImgParamAddr;//图像处理参数结构地址，由上层传下参数结构物理地址
	XDAS_UInt32 u32ImgParamSize;//图像处理参数结构大小
	XDAS_UInt32 u32ImgRetVal;   //错误码信息
}TImgProcParam;

/* :public IIMGDEC_InArgs*/
typedef struct
{
	IVIDDEC_InArgs img_Inargs;
	XDAS_UInt32 u32ImgInputAddr;//图像处理输入结构地址，由上层传下输入结构物理地址
    XDAS_UInt32 u32ImgOutputAddr;//图像处理输出结构地址，由上层传下输出结构物理地址
}TImgInputParam;

/* : public IIMGDEC_OutArgs*/
typedef struct
{
	IVIDDEC_OutArgs img_Outargs;
}TImgOutputParam;

/* :public IIMGDEC_DynamicParams*/
typedef struct
{
    IVIDDEC_DynamicParams img_DynamicParams;
    XDAS_UInt32 u32ImgDynParamAddr;    //图像处理动态参数结构地址，由上层传下参数结构物理地址
    XDAS_UInt32 u32ImgDynParamSize;    //图像处理动态参数结构大小
    XDAS_UInt32 u32ImgStatAddr;        //图像处理状态参数结构地址，由上层传下参数结构物理地址
}TImgProcDynParams;

/* :public IIMGDEC_Status*/
typedef struct
{
    IVIDDEC_Status img_Status;
}TImgProcStat;

#ifdef __cplusplus
}
#endif

#endif
