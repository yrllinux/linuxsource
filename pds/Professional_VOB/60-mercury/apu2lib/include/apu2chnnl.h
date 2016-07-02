/*****************************************************************************
  模块名      : apu2lib
  文件名      : apu2chnnl.h
  相关文件    : 
  文件实现功能: apu2通道对象拓展
  作者        : 周嘉麟
  版本        : V1.0  Copyright(C) 2009-2010 KDC, All rights reserved.
******************************************************************************/
#ifndef _APU2_CHNNL_H_
#define _APU2_CHNNL_H_

#include "eqpbase.h"
#include "apu2cfg.h"
#include "kdvmixer_apu2.h"
#include "evapu2.h"
#include "kdvmedianet.h"
#include "mcuinnerstruct.h"

#ifdef WIN32
	#include "winbrdwrapper.h"
#endif

//编解码信息
typedef struct tAudDecEncInfo
{
	//解码信息
	u8 byDecAudType;
	u8 byDecSoundChnlNum;
	u8 bySampleRate;
	TDoublePayload tDecPayload;
	//编码信息
	u8 byEncAudType;
	u8 byEncSoundChnlNum;
	TDoublePayload tEncPayload;
}TAudDecEncInfo;

	/************************************************************************/
	/*							CApu2Chnnl类定义							*/
	/************************************************************************/
class CRcvCallBack;

class CApu2Chnnl:public CEqpChnnlBase
{
public:
	CApu2Chnnl(const CEqpBase* const pEqp, CEqpCfg* const pcCfg, const u16 wChnIdx);
	~CApu2Chnnl();
public:
	//通道初始化
	BOOL32 Init();
	//通道销毁
	BOOL32 Destroy();

	/************************消息入口*************************/
	BOOL32 OnMessage(CMessage* const pcMsg);

	/************************消息接口*************************/
	//开启混音处理
	void ProcStartMix(CMessage* const pcMsg);
	//停止混音处理
	void ProcStopMix(CMessage* const pcMsg);
	//增加混音成员处理
	void ProcAddMixMember(CMessage* const pcMsg);
	//删除混音成员处理
	void ProcRemoveMixMember(CMessage* const pcMsg);
	//开启语音激励处理
	void ProcStartVac(CMessage* const pcMsg);
	//停止语音激励处理
	void ProcStopVac(CMessage* const pcMsg);
	//开启强制混音处理
	void ProcForceActive(CMessage* const pcMsg);
	//停止强制混音处理
	void ProcCancelForceActive(CMessage* const pcMsg);
	//开始编码发送
	void ProcMixSendNtf(CMessage* const pcMsg);
	//设置混音深度
	void ProcSetMixDepth(CMessage* const pcMsg);
	//设置语音激励保护时间
	void ProcSetVacKeepTime(CMessage* const pcMsg);	
	//激励成员改变
	void ProcActiveMemChange(const u8 byActiveChnId);
	//生产测试开启混音
	void ProcTestStartMix(CMessage* const pcMsg);
	//生产测试停止混音
	void ProcTestStopMix();
	//生产测试复位
	void ProcTestRestore();
#ifndef _8KI_//8KI不支持点灯
	//生产测试点灯
	void ProcTestLed();
	//生产测试点灯定时处理
	void ProcTimerLed(CMessage* const pcMsg);
#endif

	/************************功能接口*************************/
	//开启混音
	BOOL32  StartMixing();
	//停止混音
	BOOL32  StopMixing();
	//停止语音激励
	BOOL32	StopVac();
	//增加单个混音成员
	BOOL32  AddMixMem(TMixMember tMixMember, u8 byChIdx);
	//移除单个混音成员
	BOOL32  RemoveMixMem(const u8 byMtId);
	//移除所有混音成员
	BOOL32  RemoveAllMixMem();
	//设置发送对象参数
	BOOL32  SetSndObjectPara(u32 dwDstIp, u16 wDstPort, u8 byChnIdx);
	//设置接收对象参数
	BOOL32  SetRcvObjectPara(const u8 byChnIdx);
	//终端是否在混音
	u8      IsMtInMix(const u8 byMtId);
	//根据通道索引获取成员ID
	u8      GetMtIdFromChnId(const u8 byChnId);
	//获取音频模式
	u8      GetAudioMode(const u8 byAudioType, u8 byAudioTrackNum = 1);	
	//获取网络发送对象
	CKdvMediaSnd* GetMediaSnd(const u16 wIndex, BOOL32 bDMode);
	//获取媒体控制对象
    CKdvMixer* GetKdvMixer();
	//转换音频类型
	u8		TransSimRate(const u8 bySimpleRate);
	//停止通道工作
	BOOL32  Stop(const u8 byMode = MODE_BOTH);
	//混音器信息打印
	void    Print();
	//通道信息打印
	void	PrintChnlInfo();
	//混音编解码信息打印
	void	PrintMixState();
public:
	void InitAudChnlInfo();
	TAudDecEncInfo*	GetChnlInfo(const u8 byIdx);
	BOOL32 SetMixEncInfo(TAudioCapInfo *ptCapInfo, u8 bySndIdx);
	BOOL32 SetMixDecInfo(TAudioCapInfo *ptCapInfo, u8 byRcvIdx);
public:
	void SetMediaEncParam(u8 byChIdx, BOOL32 bNMode = FALSE);	/*设置底层编码参数*/
	void SetMediaDecParam(u8 byChIdx);							/*设置底层解码参数*/
	void SetLocNetParam(TMixMember *ptMixMember, u8 byChIdx);	/*设置接收地址参数*/
	void SetAudResend(u8 bySndChlIdx);							/*设置底层冗余发送参数*/
public:
	BOOL32 CheckConfId(CMessage* pcMsg);
	s8 *ReadMsgBody(CServMsg &cServMsg,  u8 &wMsgLen, u8 &byChIdx);
	void SendStartError(CServMsg &cMsg, u16 wRet = 0);
	void ProcMsgPrint(void);

private:
	//网络收发及媒体控制	
	CKdvMixer	   m_cMixer;							//媒体控制对象
	CKdvMediaRcv * m_pcAudioRcv;						//音频接收对象
	CKdvMediaSnd * m_pcAudioSnd;						//音频发送对象
	CRcvCallBack * m_pcRcvCallBack;						//接收回调对象
	u16			   m_wAudRcvNum;						//音频接收对象数
	u16			   m_wAudSndNum;						//音频发送对象数
	/*参数*/
	TMediaEncrypt  m_tMediaEncrypt;
	TCapSupportEx  m_tCapSupportEx;
	u8			   m_byIsNeedPrs;
	u8			   m_byDepth;
	u8			   m_byQualityLvl;						// 保存音质等级
	TAudDecEncInfo m_atAudChnlInfo[MAXNUM_MIXER_CHNNL + MAXNUM_NMODECHN];	//各通道编解码信息
	//会议关联
	CConfId        m_cConfId;
	u8			   m_abyMtId[MAXNUM_MIXER_CHNNL];
	//配置指针
	CApu2MixerCfg*const m_pcMixerCfg;
	//生产测试点灯
	u8             m_abyLed[APU2_TEST_LEDNUM];
};

	/************************************************************************/
	/*							CApu2Chnnl类定义							*/
	/************************************************************************/
class CApu2ChnStatus:public CChnnlStatusBase
{
public:
	enum EnMixState
	{
		IDLE = 0,
			READY,	
			MIXING,			//定制混音或智能混音
			VAC,            //语音激励
			MIXING_VAC,	    //智能混音 + 语音激励
	};
public:
	CApu2ChnStatus();
	virtual~CApu2ChnStatus();
};

	/************************************************************************/
	/*							CRcvCallBack类定义						    */
	/************************************************************************/
class CRcvCallBack
{
public:
    CApu2Chnnl*   m_pcMixerChn;    //回调的混音器
    u16           m_wChnId;        //回调的通道号 
	CRcvCallBack()
	{
		m_pcMixerChn = NULL;
		m_wChnId     = 0;
	}
	virtual~CRcvCallBack(){};
};

#endif