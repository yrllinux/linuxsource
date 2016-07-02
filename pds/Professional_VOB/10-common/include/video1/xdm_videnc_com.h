/*****************************************************************************
模块名      : 
文件名      : xdm_videnc_com.h
相关文件    : 
文件实现功能: 
作者        : 王建
版本        : V1.0  Copyright(C) 2006-2007 KDC, All rights reserved.
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2007/2/26   1.0         王建        创建
******************************************************************************/
#ifndef  XDM_VIDENC_COM_H_
#define  XDM_VIDENC_COM_H_

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef CHAR_BIT
   #define CHAR_BIT 8
#endif

/* : IVIDENC_Cmd */
enum VidEncCmdId
{
    VID_ENC_SET_PARAMS = 257,
    VID_ENC_GET_STATIS,
    VID_ENC_GET_STATUS,
    VID_ENC_PARSE_ERR_CODE,
    VID_ENC_GET_VER,
    VID_ENC_DEBUG		
};

/* :public IVIDENC_Params*/
typedef struct
{
    IVIDENC_Params videnc_Params;
    XDAS_UInt32 u32VidUnitEncParamAddr;    //VieoUnit编码参数结构地址，由上层传下参数结构物理地址
}TVidEncoderParams;

/* :public IVIDENC_InArgs*/
typedef struct
{
    IVIDENC_InArgs videnc_Inargs;
    TEncInput tEncInput;             //VideoUnit编码输入
}TVideoEncoderInputParam;

/* : public IVIDENC_OutArgs*/
typedef struct
{
	IVIDENC_OutArgs videnc_Outargs;
	TEncOutput tEncOutput;          //VideoUnit编码输出
}TVideoEncoderOutputParam;

/* :public IVIDENC_DynamicParams*/
typedef struct
{
    IVIDENC_DynamicParams videnc_DynamicParams;
    XDAS_UInt32 u32VidUnitEncDynParamAddr;    //VieoUnit编码动态参数结构地址，由上层传下参数结构物理地址
    XDAS_UInt32 u32VidUnitEncStatAddr;        //VieoUnit编码状态参数结构地址，由上层传下参数结构物理地址
}TVidEncoderDynParams;

/* :public IVIDENC_Status*/
typedef struct
{
    IVIDENC_Status videnc_Status;
}TVidEncoderStat;

#ifdef __cplusplus
}
#endif

#endif
