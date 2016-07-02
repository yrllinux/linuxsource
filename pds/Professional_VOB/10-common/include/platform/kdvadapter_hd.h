/*****************************************************************************
  模块名      : Codecwrapper.a
  文件名      : codecwrapper_hd.h
  相关文件    : codecwrapperdef_hd.h
  文件实现功能: 
  作者        : 朱允荣
  版本        : V4.0  Copyright(C) 2007-2008 KDC, All rights reserved.
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2007/11/28  4.0         朱允荣      创建
******************************************************************************/


#ifdef _MEDIACTRL_DM816x_

#include "dm816x/kdvadapter_hd.h"

#else

#ifndef _CODEADAPTER_HD_H_
#define _CODEADAPTER_HD_H_
#include "codecwrapperdef_hd.h"

class CAdapterChannel;

class CKdvAdapterGroup
{
public:
    CKdvAdapterGroup ();
    virtual ~ CKdvAdapterGroup ();

public:
    u16 CreateGroup(u32 dwGroupID);// 创建适配组,参数见codecwrapperdef_hd.h
    u16 StartGroup(void);//开始适配		   
   	u16 StopGroup(void);//停止适配	
    u16 DestroyGroup(void);//退出组

	u16 SetFpgaErrReboot(u32 dwErrCount);

    u16 AddVideoChannel(const TAdapterChannel * ptAdpChannel, u8& byChnNo); //添加图象通道
    u16 RemoveVideoChannel(u8 byChnNo); //删除图象通道

    u16 GetGroupStatus(TAdapterGroupStatus &tAdapterGroupStatus); //得到组的状态
    u16 GetVideoChannelStatis(u8 byChnNo, TAdapterChannelStatis &tAdapterChannelStatis); //得到视频通道的统计信息
	
	    /*函数名： GetVideoCodecType
    功    能： 
    参    数： u8 byChnNo
               u32 dwID  : 0:第一编码(FPGA)，1第二编码(FPGA or DSP)，2解码(FPGA)
               u32& dwType :VID_CODECTYPE_FPGA 或 VID_CODECTYPE_DSP
    返 回 值： u16 
    说    明： */
    u16 GetVideoCodecType(u8 byChnNo, u32 dwID, u32& dwType);

    u16 ChangeVideoEncParam(u8 byChnNo, TVideoEncParam  * ptVidEncParam, u32 dwEncNum = 1);     //改变视频适配编码参数
    u16 ChangeVideoDecParam(u8 byChnNo, TVideoDecParam  * ptVidDecParam);     //改变视频适配解码参数

    u16 SetVidDataCallback(u8 byChnNo, u32 dwID, FRAMECALLBACK fVidData, void* pContext); /*设置视频数据回调，编码器*/
    u16 SetData(u32 chnNo, const TFrameHeader& tFrameInfo);        /*设置视频码流数据，解码器*/
    
    void  ShowChnInfo(u32 chnNo, BOOL32 bVidChn);
	
    u16 SetFastUpdata(u32 dwID);
    
    u16 SetVidEncResizeMode(s32 nMode);

    u16 ClearVideo(); /*编码进入刷黑屏状态*/
	u16 SetResizeMode(u32 dwMode);//模式0:加黑边，1:裁边，2:非等比拉伸，默认0
	
    u16 SetVidDropPolicy(u8 byDropPolicy);  //设定丢包处理策略

protected:
    CAdapterChannel* m_pCodec;
};

u16 HardInitAdapter(TAdapterInit* pInit);
u16 HardDestroyAdapter();
#endif //_MEDIACTRL_DM816x_
#endif
