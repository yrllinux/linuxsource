/*****************************************************************************
  模块名      : 
  文件名      : kdvadapter_hd.h
  相关文件    : 
  文件实现功能: 
  作者        : 王雪宁
  版本        : V1.0  Copyright(C) 2008-2009 KDC, All rights reserved.
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2012/4/24   1.0         王雪宁      创建
******************************************************************************/
#ifndef _CODEADAPTER_HD3_H_
#define _CODEADAPTER_HD3_H_

#include "codecwrapperdef_hd.h"
//////////////////////////////////////////////////////////////////////////
#define MAX_BAS_SESSION					4
#define BAS_BASIC_ADAPTER_MAX_NUM		4
#define BAS_ENHANCED_ADAPTER_MAX_NUM	7
//////////////////////////////////////////////////////////////////////////
//错误码定义

#define HARD_BAS_OK         0
#define HARD_BAS_BASE       5000

enum enBASErr
{
    Bas_Success = HARD_BAS_OK,
		
	Bas_Error_Base = HARD_BAS_BASE,
	Bas_Error_NoMemory,
	Bas_Error_NoDevice,
	Bas_Error_NoCreate,
	Bas_Error_Param,
	Bas_Error_Process
};
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//adapter structs
/* 适配器编码参数 */
typedef struct tagVidAdapterCreate
{
    u32  m_dwDevVersion;	//enBoardType产品版本号
    u32  m_dwMode;			//业务模式，如BAS_BASIC
}TVidAdapterCreate;

typedef struct tagAdapterEncParam
{
	u32 m_dwEncoderId;		//编码器ID,0、1、2、3……;每个适配通道都是从0开始
    TVideoEncParam m_tVideoEncParam;	//编码器参数
}TAdapterEncParam;

//适配通道
typedef struct tagAdapterChannel
{
	u32 m_dwAdapterChnlId;	//适配器通道号,0、1、2、3……
    u32	m_dwDecMediaType;	//解码码流类型,解码时节省重新创建解码器的时间;不确定时填个默认值
	u32 m_dwEncNum;			//编码器个数
    TAdapterEncParam* m_ptEncParam; //编码参数
}TAdapterChannel;
//////////////////////////////////////////////////////////////////////////
class CAdapterChannel;

class CKdvVideoAdapter
{
public:
    CKdvVideoAdapter();
    ~CKdvVideoAdapter();

public:
    u16 Create(const TVidAdapterCreate &tInit);	//创建适配组,参数见codecwrapperdef_hd.h
	u16 Destroy(void);	//退出

    u16 Start(u32 dwAdapterChnlId);	//开始某一路适配		   
   	u16 Stop(u32 dwAdapterChnlId);	//停止某一路适配	
    
    u16 AddVideoChannel(const TAdapterChannel *ptAdpChannel);	//添加适配通道
    u16 RemoveVideoChannel(u32 dwAdapterChnlId);				//删除适配通道

	//dwAdapterChnlId适配器通道号,dwEncoderId编码器编号
    u16 GetVidDecStatus(u32 dwAdapterChnlId, TKdvDecStatus &tVidDecStatus); //得到解码状态信息
    u16 GetVidDecStatis(u32 dwAdapterChnlId, TKdvDecStatis &tVidDecStatis); //得到解码统计信息

	u16 GetVidEncStatus(u32 dwAdapterChnlId, u32 dwEncoderId, TKdvEncStatus &tVidEncStatus);
	u16 GetVidEncStatis(u32 dwAdapterChnlId, u32 dwEncoderId, TKdvEncStatis &tVidEncStatis);

    u16 ChangeVideoEncParam(u32 dwAdapterChnlId, TVideoEncParam* ptVidEncParam, u32 dwEncNum = 1);	//改变一个视频适配通道所有的编码参数
    u16 ChangeVideoDecParam(u32 dwAdapterChnlId, TVideoDecParam* ptVidDecParam);					//改变一个视频适配通道的解码参数

    u16 AddVideoEncParam(u32 dwAdapterChnlId, TVideoEncParam* ptVidEncParam, u32 dwEncId);
    u16 SetVidDataCallback(u32 dwAdapterChnlId, u32 dwEncoderId, FRAMECALLBACK fVidData, void* pContext); /*设置视频数据回调，编码器*/
    u16 SetData(u32 dwAdapterChnlId, const TFrameHeader& tFrameInfo);        /*设置视频码流数据，解码器*/
    
    void  ShowChnInfo(u32 dwAdapterChnlId, BOOL32 bEnc);
	
    u16 SetFastUpdata(u32 dwAdapterChnlId,u32 dwEncoderId,BOOL32 bAllUpdate=FALSE);
    
    u16 SetVidDecResizeMode(u32 dwAdapterChnlId, u32 dwMode);
	u16 SetVidEncResizeMode(u32 dwAdapterChnlId, u32 dwMode);//模式0:加黑边，1:裁边，默认0

    u16 ClearVideo(); /*清理视频缓存，必须在停止收到码流后调用*/
protected:
    CAdapterChannel* m_pCodec;
};

#endif
