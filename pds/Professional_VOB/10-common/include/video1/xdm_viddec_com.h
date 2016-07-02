/*****************************************************************************
模块名      : 
文件名      : xdm_viddec_com.h
相关文件    : 
文件实现功能: 
作者        : 王建
版本        : V1.0  Copyright(C) 2006-2007 KDC, All rights reserved.
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2007/2/26   1.0         王建        创建
******************************************************************************/
#ifndef  XDM_VIDDEC_COM_H_
#define  XDM_VIDDEC_COM_H_

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef CHAR_BIT
   #define CHAR_BIT 8
#endif

/* : IVIDDEC_Cmd */
enum VidDecCmdId
{
    VID_DEC_SET_PARAMS = 264,
    VID_DEC_GET_STATIS,
    VID_DEC_GET_STATUS,
    VID_DEC_PARSE_ERR_CODE,
    VID_DEC_GET_VER,
    VID_DEC_DEBUG		
};

/* :public IVIDDEC_Params*/
typedef struct
{
	IVIDDEC_Params viddec_Params;
	XDAS_UInt32 u32VidUnitDecParamAddr;  //VieoUnit解码参数结构地址，由上层传下参数结构物理地址
}TVidDecoderParams;

/* :public IVIDDEC_InArgs*/
typedef struct
{
	IVIDDEC_InArgs viddec_Inargs;
	TDecInput tDecInput;             //VideoUnit解码输入
}TVideoDecoderInputParam;

/* : public IVIDDEC_OutArgs*/
typedef struct
{
	IVIDDEC_OutArgs viddec_Outargs;
	TDecOutput tDecOutput;           //VideoUnit解码输出
}TVideoDecoderOutputParam;

/* :public IVIDDEC_DynamicParams*/
typedef struct
{
    IVIDDEC_DynamicParams viddec_DynamicParams;
    XDAS_UInt32 u32VidUnitDecDynParamAddr;    //VieoUnit解码动态参数结构地址，由上层传下参数结构物理地址
    XDAS_UInt32 u32VidUnitDecStatAddr;        //VieoUnit解码状态参数结构地址，由上层传下参数结构物理地址
}TVidDecoderDynParams;

/* :public IVIDDEC_Status*/
typedef struct
{
    IVIDDEC_Status viddec_Status;
}TVidDecoderStat;

#ifdef __cplusplus
}
#endif

#endif
