 /*****************************************************************************
   模块名      : HardMixer
   文件名      : KdvMixer.h
   相关文件    : 
   文件实现功能: 智能混音器上层API 封装
   作者        : 向飞
   版本        : V3.0  Copyright(C) 2001-2002 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人         修改内容
   2003/12/03  3.0         向飞           创建
******************************************************************************/
#ifndef _MIXER_H_
#define _MIXER_H_

#include "codeccommon.h"

#define MAX_CHANNEL_NUM     192//最大通道数
#define MAX_MIXER_DEPTH     10 //混音深度

/*语音编码类型
#define	 MEDIA_TYPE_MP3	     (u8)96//mp3
#define  MEDIA_TYPE_PCMU	 (u8)0//G.711 ulaw
#define  MEDIA_TYPE_PCMA	 (u8)8//G.711 Alaw
#define	 MEDIA_TYPE_G7231	 (u8)4//G.7231
#define	 MEDIA_TYPE_G728	 (u8)15//G.728
#define	 MEDIA_TYPE_G729	 (u8)18//G.729
*/

//语音激励回调，数组abyActiveChns中前面的非0元素表示当前参与混音的通道号, byExciteChn表示当前激励的通道号, 为0表示没有激励出通道
typedef void (*TMixerCallBack)(unsigned char* abyActiveChns, unsigned char byExciteChn, unsigned int dwContext);

//Map 参数
typedef struct tagMixerMap
{         
    u32   m_dwCoreSpd; //Map内核速度
    u32   m_dwMemSpd;  //内存速度
    u32   m_dwMemSize; //内存大小
    u8    m_byMapId;   //Map编号
    u8    m_byPort;    //Map端口号
}TMixerMap;

//混音通道
typedef struct tagMixerChannel
{
	u8     m_byChannelIndex;//通道索引号
	TNetAddress m_tSrcNetAddr;     //通道源地址(接收对象)
								   //通道源RTCP地址等于通道源地址的端口加一(接收对象)
    TNetAddress m_tRtcpBackAddr;   //通道RTCP回馈地址(接收对象)
    TNetAddress m_tLocalNetAddr;   //通道本地地址(发送对象)
								   //通道本地RTCP地址等于通道本地地址的端口加一(发送对象)
    TNetAddress m_tDstNetAddr;     //通道目的地址(发送对象)
}TMixerChannel;

//混音组状态
typedef struct tagMixerGroupStatus
{
    BOOL     m_bMixerStart;      //混音是否开始
    u8     m_byCurChannelNum;  //混音组的通道数
    TNetAddress m_tNModeDstAddr;    //N模式的输出地址
    TMixerChannel m_atChannel[MAX_CHANNEL_NUM];  //组中的通道	
}TMixerGroupStatus;

//通道统计信息
typedef struct tagMixerChannelStatis
{
    u32  m_dwRecvBitRate;        //接收码率
    u32  m_dwRecvPackNum;        //收到的包数
    u32  m_dwRecvLosePackNum;    //网络接收缺少的包数
    u32  m_dwSendBitRate;        //发送码率
    u32  m_dwSendPackNum;        //发送的包数
    u32  m_dwMixerDiscardPackNum;//混音丢掉的包数
}TMixerChannelStatis;

class CAudioEncoder;
class CAudioDecoder;

class CKdvMixerGroup
{
public:
    CKdvMixerGroup();
    virtual ~CKdvMixerGroup();

public:
	u16 CreateMixerGroup(const TMixerMap* tMixerMap,u8 byChannelNum);//创建混音组
	u16 DestroyMixerGroup();//删除混音组
	u16 StartMixerGroup(u8 bAllMix=TRUE);//开始混音组		
	u16 StopMixerGroup();//停止混音组	
	u16 AddMixerChannel(const TMixerChannel* tMixerChannel);//增加组的混音通道
	u16 SetMixerNetFeedbackParam(u8 byChnIndex,  BOOL32 bFeedBackSnd, TNetRSParam tNetRSParam, BOOL32 bRepeatSnd, u16 wBufTimeSpan);//设置混音重传
	u16 DeleteMixerChannel(u8 byChannelIndex);//删除组的混音通道
	u16 SetNModeOutput(const TNetAddress* tLocalAddr, const TNetAddress* tDstNetAddr);
	u16 SetNModeNetFeedbackParam(BOOL32 bRepeatSnd, u16 wBufTimeSpan);//设置N模式混音重传
	
	u16 SetForceAudMix(u8 byChnNo); // 强制混音
    u16 CancelForceAudMix(); // 取消强制
    u16 SetMixerDepth(u8 byDepth);//设置混音深度
    u16 SetMixerCallBack(TMixerCallBack tMixerCallBack, int dwContext);//语音激励回调
    u16 GetActiveChns(u8 abyActiveChnsArr[], u8 byArrNum, u8 *pbyActiveNum); //获得当前参与混音的通道

	u16 SetMixerChannelVolume(u8 byChannelIndex,u8 byVolume);//设置通道音量
	u16 GetMixerGroupStatus(TMixerGroupStatus &tMixerGroupStatus);//得到组的状态
	u16 GetMixerChannelStatis(u8 byChannelIndex,TMixerChannelStatis &tMixerChannelStatis);//得到通道的统计信息

	u16 SetMixerParam(TMixerParam  *ptMixPara);//设置智能混音器参数

	u16 StopMixerSnd(BOOL32 bStopSnd);	//TRUE: 停止所有发送, FALSE: 开始所有发送
	u16 SetExciteKeepTime(u32 dwKeepTime);  //设置语音激励的延时保护, 单位：秒
	BOOL32 IsChnSelected(u8 byChannel, u8 byMixChnNum);	// 判断指定通道是否被选入混音组

	void MixerCallBackProc(u8 *pbyMediaMsg, u32 dwMediaMsgLen);
	u16 SetChnSelectTactic(void);		//设置混音通道的选择策略，是否智能

	u16    SetMixChnStatus(u8  bChnStatus[192]);   //设置手工参与混音模式下，各通道是否参与混音
	
	//设置音频编码载荷PT值
	u16  SetAudEncryptPT(u8 byChnNo, u8 byEncryptPT);
	//设置音频编码加密key字串以及 加密模式 Aes or Des
	u16  SetAudEncryptKey(u8 byChnNo, s8 *pszKeyBuf, u16 wKeySize, u8 byEncryptMode );
	u16  SetNetAudioResend(u8 byChnNo, s32 nSendTimes, s32 nSendSpan);
	
	//设置 动态音频载荷的 Playload值
	u16  SetAudioActivePT(u8 byChnNo, u8 byRmtActivePT, u8 byRealPT );
	//设置 音频解码key字串 以及 解密模式 Aes or Des
    u16  SetAudDecryptKey(u8 byChnNo, s8 *pszKeyBuf, u16 wKeySize, u8 byDecryptMode); 
    
    //设置混音延时(ms)
    u16 SetMixerDelay(u32 dwTime);
	
	//设置开始, 快收缓冲数
	u16 SetMixerRcvBufs(u32 dwStartRcvBufs, u32 dwFastRcvBufs);

	/////////////////     测试用    ////////////////
	u16 SendMsgToMap(u32 dwCciQueNo, u8 *pbyMsgHead, u32 dwMsgHeadLen, u8 *pbyMsgBody,
		u32 dwMsgBodyLen);	
	u16 H2MCmdSend(u32 type, u32 event, u32 channel, u8 *msg, u32 msgLen); 
	u16 SetReverseG7221c(u32 channel, BOOL32 bReverse);
	//////////////////////////////////////////////////////////////////////////

public:
	u8 m_abyMixingChns[MAX_MIXER_DEPTH];
	u8 m_byMixingChnNum;		// 当前混音的通道数
	u8 m_byChannelNum;			// 最大竞选的通道数
	u32 m_adwEncoderId[MAX_CHANNEL_NUM + 1];
	u8 m_byMixerStart;
	u8 m_byMixDepth;			// 混音深度
	u8 m_bChnStatus[192];		// 手工参与混音模式下，各通道是否参与混音
	TMixerParam m_tMixerParam;	// 混音参数

private:
    CAudioEncoder *m_apMixerEncoder[MAX_CHANNEL_NUM+1];
    CAudioDecoder *m_apMixerDecoder[MAX_CHANNEL_NUM];
    u8 m_byChannelState[MAX_CHANNEL_NUM+1];
    TMixerGroupStatus m_tMixerGroupStatus;
	u8 m_byMapNo;
	u8 m_byForceMixChn;
	TMixerCallBack  m_tMixerCallBack;
	u32           m_dwCallBackCont;
	BOOL32		  m_bStopSnd;	//是否禁止所有通道发送	
	u8 m_byEnteredChnNum;		// 已加入的通道数	
	BOOL32	m_bUseExcite;	// 是否用智能混音策略 ？
};

API void mixerwrapperver(u32 dwMapId);
API void setmixersend(u32 dwMapNo, BOOL32 bStopSend);     //停止/开始混音器发送

#ifndef TOSFUNC
#define TOSFUNC
//设置媒体TOS值 nType类型0:全部 1:音频 2:视频 3: 数据
API int SetMediaTOS(int nTOS, int nType);
API int GetMediaTOS(int nType);
#endif

#endif
