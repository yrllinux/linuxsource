/*****************************************************************************
  模块名      : apu2lib
  文件名      : apu2baschnnl.h
  相关文件    : 
  文件实现功能: apu2通道对象拓展
  作者        : 国大卫
  版本        : V1.0  Copyright(C) 2009-2010 KDC, All rights reserved.
******************************************************************************/
#ifndef _APU2BASCHNNL_H_
#define _APU2BASCHNNL_H_

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
typedef struct tAudBasInfo
{
	//解码信息
	u8 byDecAudType;
	u8 byDecSoundChnlNum;
	u8 bySampleRate;
	TDoublePayload tDecPayload;
	//编码信息
	u8 abyEncAudType[MAXNUM_APU2BAS_OUTCHN];
	u8 abyEncSoundChnlNum[MAXNUM_APU2BAS_OUTCHN];
	TDoublePayload atEncPayload[MAXNUM_APU2BAS_OUTCHN];
}TAudBasInfo;

	/************************************************************************/
	/*							CApu2BasChnnl类定义							*/
	/************************************************************************/
class CBasRcvCallBack;

class CApu2BasChnnl:public CEqpChnnlBase
{
public:
	CApu2BasChnnl(const CEqpBase* const pEqp, CEqpCfg* const pcCfg, const u16 wChnIdx);
	~CApu2BasChnnl();
public:
	//基本
	BOOL32 Init();
	BOOL32 Destroy();
	BOOL32 OnMessage(CMessage* const pcMsg);

	//消息处理
	void ProcStartAdaptReq(CMessage* const pcMsg);		/*处理开启适配请求*/
	void ProcChgAdaptReq(CMessage* const pcMsg);		/*处理修改参数命令*/
	void ProcStopAdaptReq(CMessage* const pcMsg);		/*处理停止适配请求*/

	//功能接口
	u16		OnStartAdaptReq(void);						/*开适配*/
	u16		OnChgAdaptReq(const u8 byChIdx, u8 *abyEncChg);				/*改参数*/
	BOOL32 	OnStopAdaptReq(void);						/*关适配*/
	u16		AddBasMember(const u8 byChIdx);				/*加通道*/
	u16		RemoveBasMember(const u8 byChIdx);			/*删通道*/
	u8		GetAudioMode(const u8 byAudioType, u8 byAudioTrackNum = 1);		/*获取音频模式*/
	CKdvAudAdaptor* GetKdvBas();						/*获取媒体对象*/
	u8		TransSimRate(const u8 bySimpleRate);		/*转换解码率*/
	BOOL32  Stop(const u8 byMode = MODE_BOTH);			/*通道停止*/
	void    Print();
	void	PrintChnlInfo();

public:
	/*m_atAudBasInfo*/
	void	InitChnlInfo(TAudBasInfo &tAudChnlInfo);	/*初始化通道信息*/
	tAudBasInfo* GetChnlInfo(const u8 byIdx);			/*获取通道信息*/
	BOOL32	SetBasEncInfo(TAudAdaptParam *pAptParam, u8 byRcvIdx, u8 bySndIdx);	/*设置通道编码信息*/
	BOOL32	SetBasDecInfo(TAudioCapInfo *pAudCapInfo, u8 byRcvIdx);	/*设置通道解码信息*/

	/*m_acConfId*/
	CConfId GetConfId(const u16 wIndex);				/*获取通道所属会议*/

	/*m_abChnlOccupy*/
	BOOL32	GetChnlOccupy(u8 byChIdx);					/*获取通道占用情况*/

	/*m_pcAudioRcv*/
	void CreateAudioRcv(u16 wNum);		/*实例化接收对象*/

	/*m_pcAudioSnd*/
	void CreateAydioSnd(u16 wNum);		/*实例化发送对象*/
	CKdvMediaSnd* GetMediaSnd(const u32 wIndex);		/*获取发送对象*/

public:
	void SetMediaEncParam(u8 byChIdx);	/*设置底层编码参数*/
	void SetMediaDecParam(u8 byChIdx);	/*设置底层解码参数*/
	void SetAudResend(u8 bySndChlIdx);	/*设置底层冗余发送参数*/
	BOOL32  SetSndObjectPara(u32 dwDstIp, u16 wDstPort, u8 bySndChnIdx);	/*设置发送对象*/
	BOOL32  SetRcvObjectPara(const u8 byChnIdx);							/*设置接收对象*/
	BOOL32	SetRcvNetParam(const u8 byChnIdx);								/*设置地址参数*/

public:
	void SendMsgToMcu(CServMsg &cServMsg, u16 wRet = ERR_BAS_NOERROR, u8 byChIdx = 0);
	s8	*ReadMsgBody(CServMsg &cServMsg,  u8 &wMsgLen, u8 &byChIdx);
	void VariablesClear(u8 byClearLevel = 0);
	void ProcMsgPrint(u8 byChIdx);

private:
	CKdvAudAdaptor m_cBas;								/*媒体控制对象*/
	CKdvMediaRcv * m_pcAudioRcv;						/*音频接收对象*/
	CKdvMediaSnd * m_pcAudioSnd;						/*音频发送对象*/
	CBasRcvCallBack *m_pcRcvCallBack;					/*接收回调对象*/
	/*适配参数*/
	TMediaEncrypt  m_tMediaEncrypt;						/*加密参数*/
	BOOL32		   m_bIsNeedPrs;						/*是否重传*/

	u8			   m_byQualityLvl;						/*音质等级*/
	TAudBasInfo	   m_atAudBasInfo[MAXNUM_APU2_BASCHN];	/*各通道编解码信息*/
	CConfId        m_acConfId[MAXNUM_APU2_BASCHN];		/*通道所属会议*/
	BOOL32		   m_abChnlOccupy[MAXNUM_APU2_BASCHN];	/*占用通道的终端索引*/

	/*配置指针*/
	CApu2BasCfg*const m_pcBasCfg;
};

	/************************************************************************/
	/*							CApu2BasChnStatus类定义						*/
	/************************************************************************/
class CApu2BasChnStatus : public CChnnlStatusBase
{
public:
	enum EnMixState
	{
		IDLE = 0,
		READY,	
		RUNNING,	
	};
public:
	CApu2BasChnStatus();
	virtual~CApu2BasChnStatus();
};

	/************************************************************************/
	/*							CBasRcvCallBack类定义						*/
	/************************************************************************/
class CBasRcvCallBack
{
public:
    CApu2BasChnnl*		m_pcBasChn;    //回调的适配器
    u16					m_wChnId;      //回调的通道号 
	CBasRcvCallBack()
	{
		m_pcBasChn = NULL;
		m_wChnId     = 0;
	}
	virtual~CBasRcvCallBack(){};
};

#endif