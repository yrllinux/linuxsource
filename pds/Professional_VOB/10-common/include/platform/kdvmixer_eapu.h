/*****************************************************************************
  模块名      : Codecwrapper_eapu.a
  文件名      : kdvmixer_eapu.h
  相关文件    : codecwrapperdef_t3.h
  文件实现功能: 
  作者        : 刘辉
  版本        : V4.0  Copyright(C) 2007-2008 KDC, All rights reserved.
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2009/11/01  1.0         刘辉     创建
******************************************************************************/
#ifndef _KDVMIXER_EAPU_H_
#define _KDVMIXER_EAPU_H_

#include "codecwrapperdef_hd.h"


//混音格式
typedef struct tagKdvMixerInitParam
{
	u32 dwMaxChNum;      //混音最大通道数
	u32 dwAudDecDspID;   //解码所在dsp
	u32 dwAudEncDspID;	 //编码所在dsp
	u32 dwDModeChMum; //DMode输出路数
	u32 dwReserve;
}TKdvMixerInitParam;

//混音格式
typedef struct tagKdvMixerType
{
	u32 dwChID;      //混音通道
	BOOL32 bDMode;  //额外DMode输出标志
	u8 byAudType;	//音频格式
	u8 byAudMode;   //音频网络模式
	u16 wReserve;
}TKdvMixerType;

//混音器通道状态
typedef struct tagKdvMixChStatus
{
	BOOL32 bAddMixer; //是否加入混音器
	u32 dwChID;      //混音通道
	u32 dwMixChId; //实际所在混音组通道号
	u8 byAudEncType;	//音频编码格式
	u8 byAudEncMode;   //音频编码网络模式
	u8 byAudDecType;	//音频解码格式
	u8 byAudDecMode;   //音频解码网络模式
}TKdvMixerChStatus;

//编码器统计信息计算中间值
typedef struct tagSingleEncStatis
{
	u32   dwRealEncCount;   //实际编码帧数
	u32	  dwFrameCount;		// 1分钟编码帧数
	u32   dwLoseRatio;	    //丢帧率,单位是% 	
	u32   dwAvrBitCount;	// 1分钟内码平均码率
	s32	  dwNextTimer;
	struct timeval tLastval;
}TSingleEncStatis;

//编码器统计信息计算中间值
typedef struct tagSingleDecStatis
{
	u32	  dwFrameCount;		//编码帧率
	u32   dwAvrBitCount;		// 1分钟内码平均码率
	u32   dwBitCount;		   // 2s内码平均码率
	u32   dwLastFrameID;
	u32   dwLastSSRC;
	u32   dwSSRCChangeProted;
	s32	  dwNextTimer;
	struct timeval tLastval;
}TSingleDecStatis;


//混音器通道统计
typedef struct tagKdvMixerChStatis
{
	u32 dwChID;      //通道ID
	TKdvEncStatis tAudEncStatis;   //音频编码统计
	TSingleEncStatis tAudEncSingleData;  //编码统计中间信息
	TKdvDecStatis tAudDecStatis;   //音频解码统计
	TSingleDecStatis tAudDecSingleData;  //解码统计中间信息
}TKdvMixerChStatis;


//混音编码帧回调
typedef void ( *MIXENCFRAMECALLBACK)(TKdvMixerType* ptType, PTFrameHeader pFrameInfo, void* pContext);
//语音激励回调
typedef void ( *VOLACTIVECALLBACK)(TKdvMixerType* ptType, void* pContext);

class CAudioMixer;

class CKdvMixer
{
public:
	CKdvMixer();
	~CKdvMixer();
public:
	//创建混音器
	u16 Create(const TKdvMixerInitParam& tInitParam);
	//销毁混音器
	u16 Destroy();
	//重新创建混音器
	//当混音器所使用dsp改变时，需重新创建
	u16 ReCreate(const TKdvMixerInitParam& tInitParam);
	//启停混音
	u16 StartAudMix();
	u16 StopAudMix();
	//启停语音激励
	u16 StartVolActive();
	u16 StopVolActive();
	//通道加入混音器
	//u32 dwChID，通道号为0 ~ dwMaxChNum-1
	u16 AddChannel(u32 dwChID);
	//通道退出混音器
	//u32 dwChID，通道号为0 ~ dwMaxChNum-1
	u16 DelChannel(u32 dwChID);
	//设置在混音器中音频编码参数
	//DMode通道不需要先addchannel，其余通道应先addchannel，再设置编码参数
	u16 SetAudEncParam(const TKdvMixerType& tType);
	//DMode通道设置编码参数
	u16 SetDModeChannel(const TKdvMixerType& tType);
	//DMode通道删除
	u16 DelDModeChannel(u32 dwDModeChID);
	//设置音频解码码流
	u16 SetAudDecData(u32 dwChID, const TFrameHeader& tFrameInfo);
	//设置音频编码回调
	//FRAMECALLBACK fAudData，回调函数指针；void* pContext，参数
	u16 SetAudEncDataCallback(MIXENCFRAMECALLBACK fAudData, void* pContext);
	//设置语音激励回调
	//VOLACTIVECALLBACK fVolAct，回调函数指针；void* pContext，参数
	u16 SetVolActiveCallback(VOLACTIVECALLBACK fVolAct, void* pContext);
	//强制混音通道(强制混音通道同时只有一个)
	//u32 dwChID,当dwChID未加入当前混音器中时，即为取消强制通道
	u16 SetForceChannel(u32 dwChID);
	//设置混音深度
	//u8 byDepth,混音深度目前最大为8，超过8时底层强制为8
	u16 SetMixDepth(u8 byDepth);
	//设置语音激励的保护时间
	//u32 dwKeepTime，保护时间，单位ms
	u16 SetVolActKeepTime(u32 dwKeepTime);
	//设置混音通道音量
	u16 SetMixerChVol(BOOL32 bDMode, u32 dwChID, u8 byVolume);
	// 取得混音通道音量
	u16 GetMixerChVol(BOOL32 bDMode, u32 dwChID, u8& byVolume);
	//取得混音器通道当前状态
	u16 GetMixerChStatus(BOOL32 bDMode, u32 dwChID, TKdvMixerChStatus& tStatus);
	//取得混音器道号当前统计
	u16 GetMixerChStatis(BOOL32 bDMode, u32 dwChID, TKdvMixerChStatis& tStatis);
	//设置混音延时(ms)
	u16 SetMixerDelay(u32 dwTime);
	//设置开始, 快收缓冲数
	u16 SetMixerRcvBufs(u32 dwStartRcvBufs, u32 dwFastRcvBufs);
	//取得实际参与混音所有通道号
	//pdwChId，数组空间上层开辟，
	//dwChNum入口表示数组大小；返回表示参与混音所有通道数
	u16 GetActiveChID(u32* pdwChId, u32& dwChNum);
protected:
private:
	CAudioMixer*  m_pcMixer;
};


//设定当前*.bin,*.out路径，默认/usr/bin
void SetCodecAppDir(s8* szPath);
//注册媒体控制层OSP调试命令
void RegsterCommands();

#endif
