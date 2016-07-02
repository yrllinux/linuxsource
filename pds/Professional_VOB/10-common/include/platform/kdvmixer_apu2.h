/*****************************************************************************
  模块名      : 
  文件名      : kdvmixer_apu2.h
  相关文件    : 
  文件实现功能: 
  作者        : 王雪宁
  版本        : V1.0  Copyright(C) 2008-2009 KDC, All rights reserved.
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2012/11/1   1.0         王雪宁      创建
******************************************************************************/

#ifndef _KDVMIXER_APU2_H_
#define _KDVMIXER_APU2_H_

#include "codecwrapperdef_hd.h"
#include "kdvmedianet.h"

//初始化参数
typedef struct tagKdvMixerInitParam
{
	u32 dwAudVersion;	//版本号
	u32 dwAudDecMaxNum;	//最大加入混音组路数
	u32 dwAudMixDepth;	//混音深度
}TKdvMixerInitParam;

//通道信息
typedef struct tagKdvMixerChnlInfo
{
	u32 dwMixerId;	//混音器ID
	u32 dwChID;		//通道号
	BOOL32 bNMode;	//是否N模式，用于混音器
}TKdvMixerChnlInfo;

//编码参数
typedef struct tagKdvMixerTypeParam
{
	u32 dwAudType;	//音频格式
	u32 dwAudMode;	//音频网络模式
}TKdvMixerTypeParam;

//混音器通道状态
typedef struct tagKdvMixChStatus
{
	BOOL32 bAddMixer;	//是否加入混音器
	u32 dwMixChId;		//实际所在混音组通道号
	TKdvMixerTypeParam tMixChnlType;
}TKdvMixerChStatus;

typedef TKdvMixerTypeParam TKdvAdaptTypeParam;
typedef struct tagKdvAdaptChStatus
{
	BOOL32 bAddChnl;	//通道是否添加
	TKdvAdaptTypeParam tMixChnlType;
}TKdvAdaptChStatus;

//////////////////////////////////////////////////////////////////////////
#define MIXER_MAX_GROUP			2		//最大混音组个数
#define	MIXER_MAX_MIXDEPTH		8		//混音深度
#define	MIXER_MAX_DECCHNL		64		//最大解码通道数
#define	MIXER_MAX_NMODE_ENCNUM	4		//最大N模式编码个数
//////////////////////////////////////////////////////////////////////////
//音频错误码
enum enAudioErrorCode
{
	Codec_Error_Audio_Create = (ERROR_CODE_BASE + 400),
	Codec_Error_Audio_Close,
	Codec_Error_Audio_Param,
	Codec_Error_Audio_Process,
	Codec_Error_Audio_GetMem,
	Codec_Error_Audio_GetData
};
//////////////////////////////////////////////////////////////////////////
//混音编码帧回调
typedef void (*MIXENCFRAMECALLBACK)(TKdvMixerChnlInfo* ptChnlInfo, PFRAMEHDR pFrmHdr, void* pContext);
//语音激励回调
typedef void (*SPEECHACTIVECALLBACK)(TKdvMixerChnlInfo* ptChnlInfo, void* pContext);
//////////////////////////////////////////////////////////////////////////
class CAudioMixProc;

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

	//启停混音
	u16 StartAudMix();
	u16 StopAudMix();

	//启停语音激励
	u16 StartSpeechActive();
	u16 StopSpeechActive();

	//通道加入混音器
	//u32 dwChID，通道号为0 ~ MIXER_MAX_DECCHNL-1
	u16 AddChannel(u32 dwChID);

	//通道退出混音器
	//u32 dwChID，通道号为0 ~ MIXER_MAX_DECCHNL-1
	u16 DelChannel(u32 dwChID);

	//设置在混音器中音频编码参数
	//先addchannel，再设置编码参数
	//发送通道号，0 ~ MIXER_MAX_DECCHNL-1
	//n模式通道号，0 ~ MIXER_MAX_NMODE_ENCNUM-1
	u16 SetAudEncParam(const TKdvMixerChnlInfo& tChnlInfo, const TKdvMixerTypeParam& tEncParam);

	//设置音频解码码流
	u16 SetAudDecData(u32 dwChID, const PFRAMEHDR pFrmHdr);

	//音频解码参数，AAC_LC/LD有效
	u16 SetAudioDecParam(u32 dwChID, const TAudioDecParam& tAudParam);

	//设置音频编码回调
	//MIXENCFRAMECALLBACK fAudEncCB，回调函数指针；void* pContext，参数
	u16 SetAudEncDataCallBack(MIXENCFRAMECALLBACK fAudEncCB, void* pContext);

	//设置语音激励回调
	//SPEECHACTIVECALLBACK fSpeechActiveCB，回调函数指针；void* pContext，参数
	u16 SetSpeechActiveCallBack(SPEECHACTIVECALLBACK fSpeechActiveCB, void* pContext);

	//强制混音通道(强制混音通道同时只有一个)
	//u32 dwChID,当dwChID未加入当前混音器中时，即为取消强制通道
	//先AddChannel，再设置强制混音通道
	u16 SetForceChannel(u32 dwChID);

	//设置语音激励的保护时间
	//u32 dwKeepTime，保护时间，单位ms
	u16 SetSpeechActiveKeepTime(u32 dwKeepTime);

	//设置混音解码通道音量
	u16 SetMixerDecChnlVol(u32 dwChID, u8 byVolume);

	// 取得混音解码通道音量
	u16 GetMixerDecChnlVol(u32 dwChID, u8& byVolume);

	//设置混音主动延时(ms)，用以唇音同步
	u16 SetMixerDelay(u32 dwDelayTime=0);
	
	//设置快收缓冲数
	u16 SetMixerRcvBufs(u32 dwFastRcvBufs);

	//取得混音器解码通道当前状态
	u16 GetMixerDecChnlStatus(const TKdvMixerChnlInfo& tDecChnlInfo, TKdvMixerChStatus& tDecChnlStatus);
	//取得混音器编码通道当前状态
	u16 GetMixerEncChnlStatus(const TKdvMixerChnlInfo& tEncChnlInfo, TKdvMixerChStatus& tEncChnlStatus);

	//取得混音器解码通道号当前统计
	u16 GetMixerDecChStatis(const TKdvMixerChnlInfo& tDecChnlInfo, TKdvDecStatis& tDecChnlStatis);
	//取得混音器编码通道号当前统计
	u16 GetMixerEncChStatis(const TKdvMixerChnlInfo& tEncChnlInfo, TKdvEncStatis& tEncChnlStatis);

	//取得实际参与混音所有解码通道号
	//pdwChId，数组空间上层开辟，最大混音深度
	//dwChNum入口表示数组大小；返回表示参与混音所有通道数
	u16 GetMixerChID(u32* pdwChId, u32& dwChNum);
private:
	CAudioMixProc*  m_pcMixProc;
};
//////////////////////////////////////////////////////////////////////////
#define AUDIO_ADAPTOR_MAX_CHNL		7		//音频适配最大通道数
#define	AUDIO_ADAPTOR_MAX_ENCNUM	3		//最大适配编码个数
//////////////////////////////////////////////////////////////////////////
typedef TKdvMixerTypeParam TKdvAudioAdaptorTypeParam;

//混音编码帧回调
typedef void (*AUDADAPTENCFRAMECALLBACK)(u32* pdwChID, PFRAMEHDR pFrmHdr, void* pContext);
//////////////////////////////////////////////////////////////////////////
class CAudioAdaptProc;

class CKdvAudAdaptor
{
public:
	CKdvAudAdaptor();
	~CKdvAudAdaptor();

	//创建
	u16 Create(u32 dwAudVersion=en_APU2_Board);
	//销毁
	u16 Destroy();
	
	u16 StartAdaptor(BOOL32 bStart=TRUE);
	
	//通道加入适配器
	//u32 dwChID，通道号为0 ~ AUDIO_ADAPTOR_MAX_CHNL-1
	u16 AddChannel(u32 dwChID);
	
	//通道退出适配器
	//u32 dwChID，通道号为0 ~ AUDIO_ADAPTOR_MAX_CHNL-1
	u16 DelChannel(u32 dwChID);
	
	//设置在音频适配器中音频编码参数
	//先addchannel，再设置编码参数
	//发送通道号，0 ~ AUDIO_ADAPTOR_MAX_CHNL-1
	//编码参数，0~AUDIO_ADAPTOR_MAX_ENCNUM-1
	//dwChID表示第几路适配器0 ~ AUDIO_ADAPTOR_MAX_CHNL-1，dwEncIndex表示适配后的编码路数0~AUDIO_ADAPTOR_MAX_ENCNUM-1
	u16 SetAudEncParam(const u32 dwChID, const u32 dwEncIndex, const TKdvAudioAdaptorTypeParam* ptEncParam);
	
	//设置音频解码码流
	u16 SetAudDecData(u32 dwChID, const PFRAMEHDR pFrmHdr);
	
	//音频解码参数，AAC_LC/LD有效
	u16 SetAudioDecParam(u32 dwChID, const TAudioDecParam& tAudParam);
	
	//设置音频编码回调
	//AUDADAPTENCFRAMECALLBACK fAudEncCB，回调函数指针；void* pContext，参数
	u16 SetAudEncDataCallBack(AUDADAPTENCFRAMECALLBACK fAudAdaptEncCB, void* pContext);

	//设置快收缓冲数
	u16 SetMixerRcvBufs(u32 dwFastRcvBufs);

	//取得适配器解码通道当前状态
	u16 GetAdaptDecChnlStatus(const u32 dwChID, TKdvAdaptChStatus& tDecChnlStatus);
	//取得适配器编码通道当前状态
	//dwChID表示第几路适配器0 ~ AUDIO_ADAPTOR_MAX_CHNL-1，dwEncIndex表示适配后的编码路数0~AUDIO_ADAPTOR_MAX_ENCNUM-1
	u16 GetAdaptEncChnlStatus(const u32 dwChID, const u32 dwEncIndex, TKdvAdaptChStatus& tEncChnlStatus);
	
	//取得适配器解码通道号当前统计
	u16 GetAdaptDecChStatis(const u32 dwChID, TKdvDecStatis& tDecChnlStatis);
	//取得适配器编码通道号当前统计
	//dwChID表示第几路适配器0 ~ AUDIO_ADAPTOR_MAX_CHNL-1，dwEncIndex表示适配后的编码路数0~AUDIO_ADAPTOR_MAX_ENCNUM-1
	u16 GetAdaptEncChStatis(const u32 dwChID, const u32 dwEncIndex, TKdvEncStatis& tEncChnlStatis);
private:
	CAudioAdaptProc* m_pcAdaptorProc;
};
//////////////////////////////////////////////////////////////////////////
#endif
