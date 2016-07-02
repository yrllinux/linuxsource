/*****************************************************************************
  模块名      : 混音模块
  文件名      : audiomixer.h
  相关文件    : 
  文件实现功能: 音频混音
  作者        : 胡小鹏
  版本        : V1.0  Copyright(C) 1997-2009 KDC, All rights reserved.
  -----------------------------------------------------------------------------
  修改记录:
  日  期      版本        修改人      修改内容
  2009/03/24  V1.0        胡小鹏       创建
******************************************************************************/
#ifndef _AUDIO_MIXER_H_
#define _AUDIO_MIXER_H_

#include "codecwrapper_common.h"

#define AUDMIX_MAX_CHANNEL_USR 64          //最大混音路数
#define AUDMIX_MAX_CHANNEL     64          //最大混音路数

#define MAX_MIXER_DEPTH     10 //混音深度
#define MAX_MIX_NUM  2
//混音组通道信息
typedef struct tagMixChnInfo
{ 
     BOOL   m_bForceChannel;   //是否是强制混音通道
     u32     m_dwChannelId;    //     通道号
} TMixChnInfo;

//混音通道
typedef struct
{    
    TNetAddress m_tSrcNetAddr;     //通道源地址(接收对象)
    //通道源RTCP地址等于通道源地址的端口加一(接收对象)
    TNetAddress m_tRtcpBackAddr;   //通道RTCP回馈地址(接收对象)
	TNetAddress m_tRtcpLocalAddr;
    TNetAddress m_tLocalNetAddr;   //通道本地地址(发送对象)
    //通道本地RTCP地址等于通道本地地址的端口加一(发送对象)
    TNetAddress m_tDstNetAddr;     //通道目的地址(发送对象)
}TMixerChannelParam;

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

typedef struct tagMixerGroupStatus
{
    BOOL   m_bMixerStart;      //混音是否开始
    u8     m_byCurChannelNum;  //混音组的通道数
    TNetAddress m_tNModeDstAddr;    //N模式的输出地址
    TMixerChannel m_atChannel[AUDMIX_MAX_CHANNEL_USR];  //组中的通道	
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


//语音激励回调，byExciteChn表示当前激励的通道号, 为0表示没有激励出通道
typedef void (*TMixerCallBack)(unsigned char byExciteChn, unsigned int dwContext);

class CODECWRAPPER_API CAudioMixer
{
public:
    CAudioMixer(void);
    ~CAudioMixer();
 /*************************************************************
    函数名    : Init
    功能      ：混音器初始化
    参数说明  ：   byMixNum [I]此 混音器总的通道数，混音器  总的最大能力64路
                                    若创建2个混音器则此值为32, 4个混音器为64/4=16

    返回值说明：成功返回TRUE，失败返回FALSE    
    **************************************************************/
   BOOL32 Init(u8 byMixNum);
    
    /*************************************************************
    函数名    : StartMixer
    功能      ：开始混音
    参数说明  ：无            

    返回值说明：成功返回TRUE，失败返回FALSE    
    **************************************************************/
    BOOL32 StartMixer();

    /*************************************************************
    函数名    : StopMixerSnd
    功能      ：停止混音
    参数说明  ：            

    返回值说明：成功返回TRUE，失败返回FALSE    
    **************************************************************/
    BOOL32 StopMixer();
    
     /*************************************************************
    函数名    : SetNModeOutput
    功能      ：N模式发送信息设置
    参数说明  ：ptLocalAddr             [I] 本地IP,端口，用于发送
    ptDstNetAddr            [I] 远端IP,端口，用于发送
    wTimeSan                [I] 重传时间
	bRepeat                 [I] 是否重传标志
    
    返回值说明：成功返回TRUE，失败返回FALSE
    **************************************************************/
    BOOL32 SetNModeOutput(TNetAddress* ptLocalAddr, TNetAddress* ptDstNetAddr, u16 wTimeSan, BOOL32 bRepeat = FALSE);

    /*************************************************************
    函数名    : AddMixerChannel
    功能      ：设置接收解码
    参数说明  ：byChnNo             [I] 通道编号
                ptMixerChannelParam [I] 参数指针
    
    返回值说明：成功返回TRUE，失败返回FALSE     
    **************************************************************/
    BOOL32 AddMixerChannel(u8 byChnNo, TMixerChannelParam* ptMixerChannelParam);

    /*************************************************************
    函数名    : DeleteMixerChannel
    功能      ：删除组的混音通道
    参数说明  ：byChnNo      [I] 通道编号            

    返回值说明：成功返回TRUE，失败返回FALSE   
    **************************************************************/
    BOOL32 DeleteMixerChannel(u8 byChnNo);
    
    /*************************************************************
    函数名    : SetMixerCallBack
    功能      ：语音激励回调
    参数说明  ：tMixerCallBack    [I] 回调函数
                dwContext         [I] 回调函数参数

    返回值说明：成功返回TRUE，失败返回FALSE     
    **************************************************************/
    BOOL32 SetMixerCallBack(TMixerCallBack tMixerCallBack, int dwContext);
    
    /*************************************************************
    函数名    : SetMixerCallBack
    功能      ：设置混音深度
    参数说明  ：byDepth    [I] 混音深度           

    返回值说明：成功返回TRUE，失败返回FALSE     
    **************************************************************/
    BOOL32 SetMixerDepth(u8 byDepth);     
    
    /*************************************************************
    函数名    : SetMixerCallBack
    功能      ：设置语音激励的延时保护, 单位：秒
    参数说明  ：dwKeepTime    [I] 语音激励的延时保护, 单位：秒          

    返回值说明：成功返回TRUE，失败返回FALSE 
    --------------------------------------------------------------
    修改记录  ：
    日  期      版本        修改人      走读人        修改记录
    2009/04/14  V1.0        胡小鹏                      创建
    **************************************************************/
    BOOL32 SetExciteKeepTime(u32 dwKeepTime);  

    /*************************************************************
    函数名    : SetMixerDelay
    功能      ：设置混音延时(ms)
    参数说明  ：dwTime    [I] 混音延时(ms)

    返回值说明：成功返回TRUE，失败返回FALSE  
    **************************************************************/
    BOOL32 SetMixerDelay(u32 dwTime);      
    
    /*************************************************************
    函数名    : SetForceAudMix
    功能      ：强制混音
    参数说明  ：byChnNo    [I] 强制混音通道号，从编号从０开始

    返回值说明：成功返回TRUE，失败返回FALSE  
    **************************************************************/
    BOOL32 SetForceAudMix(u8 byChnNo);     //强制混音

    /*************************************************************
    函数名    : CancelForceAudMix
    功能      ：取消所有强制
    参数说明  ：无

    返回值说明：TRUE:为启动  FALSE:为停止
    **************************************************************/
    BOOL32 CancelForceAudMix();            //取消强制

    /*************************************************************
    函数名    : StopMixerSnd
    功能      ：
    参数说明  ：bStopSnd      [I] TRUE: 停止所有发送, FALSE: 开始所有发送            

    返回值说明：成功返回TRUE，失败返回FALSE    
    **************************************************************/
    BOOL32 StopMixerSnd(BOOL32 bStopSnd);	//TRUE: 停止所有发送, FALSE: 开始所有发送

    /*************************************************************
    函数名    : GetActiveStatus
    功能      ：取启动/停止 状态
    参数说明  ：无

    返回值说明：TRUE:为启动  FALSE:为停止    
    **************************************************************/
    BOOL32 GetActiveStatus(void);    
	
    /*************************************************************
    函数名    : SetDecryptKey
    功能      ：设置接收解密key字串, pszKeyBuf设置为NULL-表示不加密
    参数说明  ：byChnNo        [I] 接收解码通道编号
                pszKeyBuf      [I]
                wKeySize       [I]
                byDecryptMode  [I]

    返回值说明：TRUE:为启动  FALSE:为停止    
    **************************************************************/
    BOOL32 SetDecryptKey(u8 byChnNo, s8 *pszKeyBuf, u16 wKeySize, u8 byDecryptMode=DES_ENCRYPT_MODE); 
    
    /*************************************************************
    函数名    : SetAudioResend
    功能      ：设置加密key字串及加密码流的动态载荷PT值, pszKeyBuf设置为NULL-表示不加密
    参数说明  ：byChnNo          [I] 编码发送通道编号
                pszKeyBuf        [I]
                wKeySize         [I]
                byEncryptMode    [I]

    返回值说明：TRUE:为启动  FALSE:为停止    
    **************************************************************/
    BOOL32 SetEncryptKey(u8 byChnNo, s8 *pszKeyBuf, u16 wKeySize, u8 byEncryptMode=DES_ENCRYPT_MODE);

    /*************************************************************
    函数名    : SetAudioResend
    功能      ：设置音频重发nSendTimes：重发次数，nSendSpan：每次重发间隔，需要把发送重传打开
    参数说明  ：byChnNo          [I] 通道编号
                nSendTimes       [I]
                nSendSpan        [I]

    返回值说明：TRUE:为启动  FALSE:为停止    
    **************************************************************/
    BOOL32 SetAudioResend(u8 byChnNo, s32 nSendTimes, s32 nSendSpan);

    /*************************************************************
    函数名    : SetAudioActivePT
    功能      ：设置 动态载荷的 Playload值, byRmtActivePT设置为0-表示清空 本端动态载荷标记
    参数说明  ：byChnNo       [I] 接收解码通道编号
                byRmtActivePT [I] 动态载荷参数
                byRealPT      [I] 动态载荷参数

    返回值说明：TRUE:为启动  FALSE:为停止
    **************************************************************/
    BOOL32 SetAudioActivePT(u8 byChnNo, u8 byRmtActivePT, u8 byRealPT);  
    
    /*************************************************************
    函数名    : SaveDecIn
    功能      ：保存解码前码流
    参数说明  ：l32ChnNo         解码器通道号
                l32FrameNum      要保存的帧数
    
    返回值说明：成功返回TRUE，失败返回FALSE    
    **************************************************************/
    BOOL32 SaveDecIn(l32 l32ChnNo, l32 l32FrameNum);

    /*************************************************************
    函数名    : SaveDecOut
    功能      ：保存解码后pcm
    参数说明  ：l32ChnNo         解码器通道号
                l32FrameNum      要保存的帧数
    
    返回值说明：成功返回TRUE，失败返回FALSE    
    **************************************************************/
    BOOL32 SaveDecOut(l32 l32ChnNo, l32 l32FrameNum);

    /*************************************************************
    函数名    : SaveEncIn
    功能      ：保存编码前pcm
    参数说明  ：l32ChnNo         编码器通道号
                l32FrameNum      要保存的帧数
    
    返回值说明：成功返回TRUE，失败返回FALSE    
    **************************************************************/
    BOOL32 SaveEncIn(l32 l32ChnNo, l32 l32FrameNum);

    /*************************************************************
    函数名    : SaveEncOut
    功能      ：保存编码后码流
    参数说明  ：l32ChnNo         解码器通道号
                l32FrameNum      要保存的帧数
    
    返回值说明：成功返回TRUE，失败返回FALSE    
    **************************************************************/
    BOOL32 SaveEncOut(l32 l32ChnNo, l32 l32FrameNum);

    /*************************************************************
    函数名    : GetMixStatus
    功能      ：获得混音组状态信息
    参数说明  TMixerGroupStatus  结构体
    
    返回值说明：
    **************************************************************/
	void   GetMixStatus(TMixerGroupStatus* tStatus);

    /*************************************************************
    函数名    : GetMixStatis
    功能      ：获得混音组统计信息
    参数说明  TMixerGroupStatus  结构体
    
    返回值说明：
    **************************************************************/
	void   GetMixStatis(u32 dwChnID, TMixerChannelStatis* tStatis);

	/*************************************************************
    函数名    : SetFeedBackParam
    功能      ：设置重传控制参数
    参数说明	channelID
				TNetRSParam  结构体
    
    返回值说明：
    **************************************************************/
	BOOL32 SetFeedbackParam(u32 dwId, TNetRSParam tParam, BOOL32 bRepeat);

	/*************************************************************
    函数名    : SetSndFeedBackParam
    功能      ：设置重传控制参数
    参数说明	channelID
				TNetRSParam  结构体
    
    返回值说明：
    **************************************************************/
	BOOL32 SetSndFeedbackParam(u32 dwId, u16 wTimeSan, BOOL32 bRepeat);

	/*************************************************************
    函数名    : SetAACChannel
    功能      ：设置AAC音频声道数
    参数说明	dwChannel    声道数
   
    返回值说明：
    **************************************************************/
	BOOL32 SetAACChannel(l32 dwChannel);

	    /*************************************************************
    函数名    : SetActivePT
    功能      ：设置 动态载荷的 Playload值
    参数说明  ：byChnNo       [I] 编码通道编号
	            byMediaType   [I] 动态载荷参数

    返回值说明：TRUE:为启动  FALSE:为停止
    **************************************************************/
	
	BOOL32 SetActivePT(u8 byMediaType);

	void Free();

	void* m_pcMixer;                  //混音

    void* m_pListSphExcit;            //语音激励列表
    void* m_pListEnc;                 //接收编码列表
    void* m_pListDec;                 //解码发送列表 
	void* m_pListDecBackBuf;          //解码备份Buf列表；用于不同时长音频混音
    
    //编号列表
    void* m_pListSelChnNo;            //选中的编号列表
    void* m_pListForceMixChnNo;       //要强制加入混音的列表
    void* m_pListMissMixerChnNo;      //没有进入混音的列表
    void* m_pListComMixChnNo;         //普通通道列表，包括进入混音和没有进入
    
    void* m_ptLockProcess;            //线程锁
    void* m_ptLock;                   //线程锁   

    void* m_ptCond; 
    l32 m_l32Period; 
    l32 m_l32Delay;
    BOOL32 m_bNmode;
    u8 m_u8Depth;
    u8 m_u8SphChn;
    u8 m_u8SphChnBak;

	u8 m_byMixNum;

//
    TMixChnInfo     m_tMixChId[MAX_MIXER_DEPTH];  //记录混音码流通道id
    TMixChnInfo     m_tSaveMixChId[MAX_MIXER_DEPTH];  //上次记录混音通道id
    u32     m_dwChSlientTime[MAX_MIXER_DEPTH]; //混音通道记时
    u32    m_dwKeepTime;  //通道保护时间
    u32     m_dwMixChNum;  //当前混音深度
    u32     m_dwSaveMixChNum;  //上次当前混音深度
    u32    m_pdwChEXite[AUDMIX_MAX_CHANNEL_USR];
    BOOL32 GetOneFreeMixCh(u32& dwMixChId,BOOL32 bLast = FALSE);   //取得一路空闲的混音组通道, 返回混音组通道id
    BOOL32  ChIsInMixer(u32 dwChid, u32& dwMixChId, BOOL32 bLast = FALSE) ;   //查询通道号是否已经在混音组中
   void  PreMixProc(); //混音前处理
   BOOL32 MixerIsChanged();
    TMixerCallBack m_tMixerCallBack;
    u32 m_dwContext;
	u32 m_TimerID;
    BOOL32 m_bActive;    
    
    void* ClearNumList(void* ptParam);
    void* RemoveNumListNode(void* ptParam, u8 u8Data);
	TMixerGroupStatus m_tMixStatus;
    u8 m_dwId;
    u32 m_dwBitRate;
	u32 m_dwDecType;
	l32 m_l32Channel;
};


#endif /*end of _AUDIO_MIXER_H_*/
