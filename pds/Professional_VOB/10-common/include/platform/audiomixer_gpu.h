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
#ifndef _AUDIO_MIXER_GPU_H_
#define _AUDIO_MIXER_GPU_H_

#include "osp.h"
#include "kdvmedianet.h"
#include "codecwrapper_common.h"


/*编码器统计信息*/
typedef struct tagKdvEncStatis
{
    u32  m_dwFrameRate;  /*帧率*/
    u32  m_dwFrameNum;	//发送帧数
    u32  m_dwBitRate;    /*码流速度*/
    u32  m_dwPackLose;   /*丢帧数*/
    u32  m_dwPackError;  /*错帧数*/
	u32  m_wAvrBitRate;  /*1分钟内视频编码平均码率*/
}TKdvEncStatis;



#define AUDMIX_MAX_CHANNEL_USR 64          //最大混音路数
#define AUDMIX_MAX_CHANNEL     64          //最大混音路数

#define MAX_MIXER_DEPTH     8 //混音深度
#define MAX_MIX_NUM  2

#define AUDIO_ADAPTOR_MAX_CHNL		4		//音频适配器通道数
#define AUDIO_ADAPTOR_MAX_ENCNUM	2		//音频适配通道最大编码数

#define AUDIO_NMODE_MAX_CHNL		3			//混音器N模式最大的路上
enum emErrorCode
{
	Codec_Success = 0,
	Codec_ERROR_NOMEM = 30000,
	Codec_ERROR_NOCREATE,
	Codec_ERROR_RECREATE,
	Codec_ERROR_Param,
	Codec_ERROR_AEncCreate,
	
};
enum
{
	AUDIO_MODE_MP3 = 4,
	AUDIO_MODE_PCMA = 5,
	AUDIO_MODE_PCMU,
	AUDIO_MODE_G723_6,
	AUDIO_MODE_G723_5,
	AUDIO_MODE_G728,
	AUDIO_MODE_G722,
	AUDIO_MODE_G729,
	AUDIO_MODE_G719,
	AUDIO_MODE_G7221,
	AUDIO_MODE_ADPCM,
    AUDIO_MODE_AACLC_32,     //32K 双声道
    AUDIO_MODE_AACLC_32_M,   //32K 单声道
    AUDIO_MODE_AACLC_48,     //48K 双声道
    AUDIO_MODE_AACLC_48_M,   //48K 单声道
    AUDIO_MODE_AACLD_32,
    AUDIO_MODE_AACLD_32_M,
    AUDIO_MODE_AACLD_48,
    AUDIO_MODE_AACLD_48_M
};

//混音组通道信息
typedef struct tagMixChnInfo
{ 
     BOOL32   m_bForceChannel;   //是否是强制混音通道
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
    BOOL32   m_bMixerStart;      //混音是否开始
    u8     m_byCurChannelNum;  //混音组的通道数
    TNetAddress m_tNModeDstAddr[AUDIO_NMODE_MAX_CHNL];    //N模式的输出地址
    TMixerChannel m_atChannel[AUDMIX_MAX_CHANNEL_USR];  //组中的通道	
}TMixerGroupStatus;

//通道统计信息
typedef struct tagMixerChannelStatis
{
    u32  m_dwRecvBitRate;        //接收码率
    u32  m_dwRecvPackNum;        //收到的包数
    u32  m_dwRecvLosePackNum;    //网络接收缺少的包数
    u32  m_dwRecvFps;		//帧率
    u32  m_dwSendBitRate;        //发送码率
    u32  m_dwSendPackNum;        //发送的包数
    u32  m_dwMixerDiscardPackNum;//混音丢掉的包数
}TMixerChannelStatis;

typedef struct tagAudType
{
	u32 dwAudType;
	u32 dwAudMode;
}TAudType;
//适配通道状态
typedef struct tagAudAdapterStatus
{
	BOOL32		bAdpUsed;	//当前通道是否使用
	TAudType	tAudType;	//音频格式
}TAudAdapterStatus;

typedef struct tagAudAdapterStatis
{
	u32  m_dwRecvBitRate;        //接收码率
	u32  m_dwRecvPackNum;        //收到的包数
	u32  m_dwRecvLosePackNum;    //网络接收缺少的包数
	
}TAudAdapterStatis;
typedef struct tagMixerChnlInfo
{
	u32 dwMixerId;	//混音器ID
	u32 dwChID;		//通道号
	BOOL32 bNMode;	//是否N模式，用于混音器
}TMixChnlInfo;

//语音激励回调，byExciteChn表示当前激励的通道号, 为0表示没有激励出通道
typedef void (*TMixerCallBack)(unsigned char byExciteChn, unsigned int dwContext);
//适配器回调,dwChID:适配的ID,  dwEncID:编码器ID
typedef void (*AudAdapterCallBack)(u32 dwChID, u32 dwEncID,  PFRAMEHDR pFrmHdr, void* pContext);

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
    函数名    : SetNModeOutput
    功能      ：支持多格式的N模式发送信息设置
    参数说明  ：
    ptLocalAddr             [I] 本地IP,端口，用于发送
    ptDstNetAddr           [I] 远端IP,端口，用于发送
    dwNetAddrNum		 [I] 指定的网络地址的个数
    wTimeSan                [I] 重传时间
	bRepeat                [I] 是否重传标志
    
    返回值说明：成功返回TRUE，失败返回FALSE
    **************************************************************/	 
    BOOL32 SetNModeOutput(TNetAddress* ptLocalAddr, TNetAddress* ptDstNetAddr,u32 dwNetAddrNum, u16 wTimeSan, BOOL32 bRepeat = FALSE); 
     /*************************************************************
    函数名    : SetAudEncParam
    功能      ：设置编码参数
    参数说明  ：    [I]byChnNo      通道号       
    bNmode                     [I]是否为指定N模式的格式
    dwEncType                [I] 编码类型
    dwEncMode                [I] 编码模式
    
    返回值说明：成功返回TRUE，失败返回FALSE
    **************************************************************/
    BOOL32 SetAudEncParam(u8 byChnNo, u32 dwEncType, u32 dwEncMode,BOOL32 bNMode=FALSE);

     /*************************************************************
    函数名    : SetAudioDecParam
    功能      ：设置解码参数---仅AACLC/AACLD有效
    参数说明  ：    [I]byChnNo      通道号       
    dwDecType                 [I]解码类型
    dwDecMode                [I] 解码模式
    
    返回值说明：成功返回TRUE，失败返回FALSE
    **************************************************************/
    BOOL32 SetAudioDecParam(u8 byChnNo, u32 dwDecType, u32 dwDecMode);	 
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
    BOOL32 SetEncryptKey(u8 byChnNo, s8 *pszKeyBuf, u16 wKeySize, u8 byEncryptMode=DES_ENCRYPT_MODE, BOOL32 bNMode=FALSE);

    /*************************************************************
    函数名    : SetAudioResend
    功能      ：设置音频重发nSendTimes：重发次数，nSendSpan：每次重发间隔，需要把发送重传打开
    参数说明  ：byChnNo          [I] 通道编号
                nSendTimes       [I]
                nSendSpan        [I]

    返回值说明：TRUE:为启动  FALSE:为停止    
    **************************************************************/
    BOOL32 SetAudioResend(u8 byChnNo, s32 nSendTimes, s32 nSendSpan,BOOL32 bNMode=FALSE);

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
    函数名    : GetNMixStatis
    功能      ：获得N 模式的编码统计
    参数说明		dwChnId    通道号
    					TMixerChannelStatis    统计值
    返回值说明：
    **************************************************************/
	void   GetNMixStatis(u32 dwChnId,TMixerChannelStatis* tStatis );

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
	BOOL32 SetSndFeedbackParam(u32 dwId, u16 wTimeSan, BOOL32 bRepeat,BOOL32 bNMode=FALSE);

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
	
	BOOL32 SetActivePT(u8 dwChnlId, u8 byMediaType,BOOL32 bNMode=FALSE);

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
    u32	m_dwNmodeNum;	//实际N模式的输出路输出
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
class CAudioAdaptorPro;
class CODECWRAPPER_API CAudioAdaptor
{
public:
	CAudioAdaptor();
	~CAudioAdaptor();

	//创建
	u16 Create();
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
	u16 SetAudEncParam(u32 dwChID, u32 dwEncIndex, u32 dwEncType, u32 dwEncMode);
	
	//设置音频解码码流
	u16 SetAudDecData(u32 dwChID, const PFRAMEHDR pFrmHdr);
	
	//音频解码参数，AAC_LC/LD有效
	u16 SetAudioDecParam(u32 dwChID, u32 dwDecType, u32 dwDecMode);
	
	//设置音频编码回调
	//AUDADAPTENCFRAMECALLBACK fAudEncCB，回调函数指针；void* pContext，参数
	u16 SetAudEncDataCallBack(AudAdapterCallBack fAudAdaptEncCB, void* pContext);

	//设置快收缓冲数
	u16 SetMixerRcvBufs(u32 dwFastRcvBufs);

	//取得适配器解码通道当前状态
	u16 GetAdaptChnlStatus(u32 dwChID, TAudAdapterStatus& tDecChnlStatus,s8 ** pchTypeName=NULL, s8 **pchModeName=NULL);
	//取得适配器编码通道当前状态
	//dwChID表示第几路适配器0 ~ AUDIO_ADAPTOR_MAX_CHNL-1，dwEncIndex表示适配后的编码路数0~AUDIO_ADAPTOR_MAX_ENCNUM-1
	u16 GetAdaptEncChnlStatus(u32 dwChID, u32 dwEncIndex, TAudAdapterStatus& tEncChnlStatus,s8 **pTypeName=NULL, s8 **pModeName=NULL);
	
	//取得适配器解码通道号当前统计
	u16 GetAdaptChnlStatis(u32 dwChID, TKdvDecStatis& tDecChnlStatis);
	//取得适配器编码通道号当前统计
	//dwChID表示第几路适配器0 ~ AUDIO_ADAPTOR_MAX_CHNL-1，dwEncIndex表示适配后的编码路数0~AUDIO_ADAPTOR_MAX_ENCNUM-1
	u16 GetAdaptEncChStatis(u32 dwChID, u32 dwEncIndex, TKdvEncStatis& tEncChnlStatis);

	BOOL32 SaveDecIn(l32 l32ChnNo, l32 l32FrameNum);

	BOOL32 SaveDecOut(l32 l32ChnNo, l32 l32FrameNum);

	BOOL32 SaveEncIn(l32 l32ChnNo, l32 l32EncId, l32 l32FrameNum);

	BOOL32 SaveEncOut(l32 l32ChnNo, l32 l32EncId, l32 l32FrameNum);
private:
	CAudioAdaptorPro *m_pAudioAdpPro;	//音频适配处理
};
#endif /*end of _AUDIO_MIXER_GPU_H_*/
