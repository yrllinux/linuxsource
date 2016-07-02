/*****************************************************************************
   模块名      : mpulib
   文件名      : mpustruct.h
   相关文件    : 
   文件实现功能: mpulib结构和回调声明 头文件
   作者        : 周文
   版本        : V4.6  Copyright(C) 2008-2010 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期       版本        修改人      修改内容
   2009/3/14    4.6         张宝卿      注释
******************************************************************************/
#ifndef _MPUSTRUCT_H_
#define _MPUSTRUCT_H_

#include "mcustruct.h"
#include "eqpcfg.h"
#include "kdvmulpic_hd.h"
#include "kdvmedianet.h"
#include "kdvadapter_hd.h"

extern s8	MPULIB_CFG_FILE[KDV_MAX_PATH];

void CBMpuBasRecvFrame( PFRAMEHDR pFrmHdr, u32 dwContext );
void CBMpuBasSendFrame( PTFrameHeader pFrmHdr, void* dwContext );
void CBMpuDVmpRecvFrame( PFRAMEHDR pFrmHdr, u32 dwContext );
void CBMpuSVmpRecvFrame( PFRAMEHDR pFrmHdr, u32 dwContext );
void CBMpuEVpuRecvFrame( PFRAMEHDR pFrmHdr, u32 dwContext );
void VMPCALLBACK( PTFrameHeader pFrameInfo, void* pContext );
void CBMpuBasAudRecvRtpPk(TRtpPack *pRtpPack, u32 dwContext);

#define MIN_BITRATE_OUTPUT             (u16)128
#define MPU_CONNETC_TIMEOUT            (u16)3*1000     // 连接超时值3s
#define MPU_REGISTER_TIMEOUT           (u16)5*1000     // 注册超时值5s
#define CHECK_IFRAME_INTERVAL          (u16)1000       // 检测关键帧时间间隔(ms)
#define FIRST_REGACK                   (u8)1           // 第一次收到注册Ack
#define PORT_SNDBIND                   20000
#define VMP_SMOOTH_OPEN	               (u8)1

#ifdef WIN32//songkun,20110628,加入MPU WIN32 B版编译
    #define MAXNUM_MPUSVMP_CHANNEL		 (u8)4
#else
    #ifndef _MPUB_256_
        #define MAXNUM_MPUSVMP_CHANNEL		 (u8)3
    #else
        #define MAXNUM_MPUSVMP_CHANNEL		 (u8)4
    #endif
#endif

#define  MAXNUM_VMPDEFINE_PIC MAXNUM_MPUSVMP_MEMBER

struct TVmpMemPicRoute
{
private:
	s8 m_szVmpMemAlias[VALIDLEN_ALIAS+1];
	s8 m_szVmpMemPicRoute[255];
public:
	TVmpMemPicRoute()
	{
		memset(m_szVmpMemAlias, 0, sizeof(m_szVmpMemAlias));
		memset(m_szVmpMemPicRoute, 0, sizeof(m_szVmpMemPicRoute));
	}	
	
	void SetRoute(const s8* pszRoute)
    {
        memset( m_szVmpMemPicRoute, 0, sizeof(m_szVmpMemPicRoute));
        strncpy( m_szVmpMemPicRoute, pszRoute,  sizeof(m_szVmpMemPicRoute));
		m_szVmpMemPicRoute[sizeof(m_szVmpMemPicRoute)-1] = '\0';
    }
	
	const s8* GetRoute() const
    {
        return m_szVmpMemPicRoute;
    }
	
	void SetVmpMemAlias(const s8* pszMemAlias)
    {
        memset( m_szVmpMemAlias, 0, sizeof(m_szVmpMemAlias));
        strncpy( m_szVmpMemAlias, pszMemAlias,  sizeof(m_szVmpMemAlias));
		m_szVmpMemAlias[sizeof(m_szVmpMemAlias)-1] = '\0';
    }
	
	const s8* GetVmpMemAlias() const
    {
        return m_szVmpMemAlias;
    }
};

struct TDebugVal
{
protected:
    BOOL32          m_bEnableBitrateCheat;   // 是否启用码率输出作弊
    u16             m_wDefault;              // 缺省下调的百分比
    u32             m_dwAudioDeferNum;       // 音频延迟的帧数
	s32             m_nVidEncHWMode;        //  [11/9/2009 pengjie] BAS 图像宽高不匹配时处理，1：裁边，0：加黑边
	s32				m_nVidSDMode;	    	//[nizhijun 2010/11/9] VMP//模式0:加黑边，1:裁边，2:非等比拉伸，默认0
	u32				m_dwVmpIframeInterval;		 //[nizhijun 2011/03/21] VMP定时检测关键帧请求时间(单位：毫秒)
	u32				m_dwBasIframeInterval;		 //[nizhijun 2011/03/21] BAS定时检测关键帧请求时间(单位：毫秒)
	TVmpMemPicRoute	m_atVmpMemPic[MAXNUM_VMPDEFINE_PIC];
public:
    void   SetEnableBitrateCheat( BOOL32 bEnable )
    {
        m_bEnableBitrateCheat = bEnable;
    }
    BOOL32 IsEnableBitrateCheat() const
    {
        return m_bEnableBitrateCheat;
    }

    u16    GetDecRateDebug( u16 wOriginRate ) const
    {
        // 没有则返回缺省值
        wOriginRate = wOriginRate * m_wDefault / 100;

        if ( wOriginRate > MIN_BITRATE_OUTPUT )
        {
            return wOriginRate;
        }
        else
        {
            return MIN_BITRATE_OUTPUT;
        }
    }
    void   SetDefaultRate( u16 wPctRate )
    {
        m_wDefault = wPctRate;
    }

	u16    GetOrigRate(u16 wDbgRate) const
    {
		if( m_wDefault == 0 )
		{
			return wDbgRate;
		}
        return (wDbgRate * 100 / m_wDefault);
    }

    void   SetAudioDeferNum(u32 dwDeferNum)
    {
        //缓冲区不能为0
        if (0 == dwDeferNum)
        {
            dwDeferNum = 1;
        }
        m_dwAudioDeferNum = htonl(dwDeferNum);
    }

    u32    GetAudioDeferNum(void)
    {
        return ntohl(m_dwAudioDeferNum);
    }

	void   SetVidEncHWMode(s32 nMode)
    {
        m_nVidEncHWMode = htonl(nMode);
    }

	u32    GetVidEncHWMode(void)
    {
        return ntohl( m_nVidEncHWMode );
    }

	void	SetVidSDMode(s32 nMode)
	{
		m_nVidSDMode = htonl(nMode);
	}
	
	u32		GetVidSDMode(void)
	{
		return ntohl(m_nVidSDMode);
	}
	
	void  SetVmpIframeInterval(u32 dwIframeInterval) { m_dwVmpIframeInterval = dwIframeInterval; }
    u32   GetVmpIframeInterval(void) { return m_dwVmpIframeInterval; }
	
	void  SetBasIframeInterval(u32 dwIframeInterval) { m_dwBasIframeInterval = dwIframeInterval; }
    u32   GetBasIframeInterval(void) { return m_dwBasIframeInterval; }

	void SetRoute(u8 byIndex, const s8* pszRoute)
    {
		if (byIndex >= MAXNUM_VMPDEFINE_PIC || NULL == pszRoute)
		{
			return;
		}
        m_atVmpMemPic[byIndex].SetRoute(pszRoute);
    }
	
	const s8* GetVmpMemRoute(u8 byIndex) const
    {
        return m_atVmpMemPic[byIndex].GetRoute();
    }
	
	void SetVmpMemAlias(u8 byIndex, const s8* pszMemAlias)
    {
		if (byIndex >= MAXNUM_VMPDEFINE_PIC || NULL == pszMemAlias)
		{
			return;
		}
		m_atVmpMemPic[byIndex].SetVmpMemAlias(pszMemAlias);
    }
	
	const s8* GetVmpMemAlias(u8 byIndex) const
    {
        return m_atVmpMemPic[byIndex].GetVmpMemAlias();
    }


	void Print()
	{
		OspPrintf(TRUE,FALSE,"IsEnableBitrateCheat: %d\n",m_bEnableBitrateCheat);
		OspPrintf(TRUE,FALSE,"BiteRates default adjust to: %d\n",m_wDefault);
		OspPrintf(TRUE,FALSE,"BAS:AudioDeferNum: %d\n",GetAudioDeferNum());
		OspPrintf(TRUE,FALSE,"BAS:VidEncHWMode: %d\n",GetVidEncHWMode());
		OspPrintf(TRUE,FALSE,"VMP:VidSDMode: %d\n",GetVidSDMode());
		OspPrintf(TRUE,FALSE,"VMP:IframeInterval: %d\n",GetVmpIframeInterval());
		OspPrintf(TRUE,FALSE,"Bas:IframeInterval: %d\n",GetVmpIframeInterval());
		OspPrintf(TRUE,  FALSE, "VmpPic as follows:\n");
		for (u8 byIndex = 0; byIndex < MAXNUM_VMPDEFINE_PIC; byIndex++)
		{
			OspPrintf(TRUE, FALSE, "Index.%d -> Alias.%s -> Route.%s\n", 
				byIndex,
				m_atVmpMemPic[byIndex].GetVmpMemAlias(),
				m_atVmpMemPic[byIndex].GetRoute());
		}
	}

};

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

// 以下CMpuBasCfg，CMpuBasAdpGroup和TMpuBasChannelInfo，为bas外设结构，暂时被EBap，Bas共用
class CMpuBasCfg
{
public:
	CMpuBasCfg()
	{
		m_byRegAckNum = 0;
	}

    virtual ~CMpuBasCfg(){}
public:
    u32              m_dwMcuIId;                 // 通信MCU.A表示
    u32              m_dwMcuIIdB;                // 通信MCU.B表示
    u32              m_dwMcuNode;                // 与MCU.A通信节点号
    u32              m_dwMcuNodeB;               // 与MCU.B通信节点号

    BOOL32           m_bEmbedMcu;                //是否内嵌在MCU.A中
    BOOL32           m_bEmbedMcuB;               //是否内嵌在MCU.B中

    u32              m_dwMcuRcvIp;               // MCU.接收地址
    u16              m_wMcuRcvStartPort;         // MCU.接收起始端口号

    u8               m_byRegAckNum;              // 第一次注册成功
	u32              m_dwMpcSSrc;                // 业务侧会话校验值 

	u16				 m_wMTUSize;				 // MTU大小
    TEqpCfg          m_tCfg;                     // 适配器配置
    TEqp             m_tEqp;                     // 本设备
    TPrsTimeSpan     m_tPrsTimeSpan;             // 重传时间跨度
    TDebugVal        m_tDebugVal;                // 作弊值	
	BOOL32           m_bIsUseSmoothSend;         // 是否打开平滑发送的开关 
    
    u8               m_byWorkMode;               // 避免使用全局变量,在此保存WorkMode

public:
    void FreeStatusDataA( void );
    void FreeStatusDataB( void );

    void ReadDebugValues();
    BOOL32 IsEnableCheat() const
    {
        return m_tDebugVal.IsEnableBitrateCheat();
    }
    u16 GetDecRateDebug( u16 wOriginRate ) const
    {
        return m_tDebugVal.GetDecRateDebug( wOriginRate );
    }

	u16 GetOrigRate(u16 wDbgRate) const
    {
        return m_tDebugVal.GetOrigRate(wDbgRate);
    }

    u32 GetAudioDeferNum( void )
    {
        return m_tDebugVal.GetAudioDeferNum();
    }

	s32 GetVidEncHWMode( void )
    {
        return m_tDebugVal.GetVidEncHWMode();
    }

    void GetDefaultParam(u8 byEnctype,TVideoEncParam& TEncparam);

	u32   GetIframeInterval(void) { return m_tDebugVal.GetBasIframeInterval(); }
};

class CMpuBasAdpGroup // 可能根据底层接口调整而调整
{
private:
	BOOL32           m_bIsStart;
	u32              m_dwNetBand;
	u32              m_dwRtcpBackAddr;   //rtcp回发地址
	u16              m_wRtcpBackPort;    //rtcp回发端口
    u16              m_wLocalStartPort;  //为本地接收的rtp端口，rtcp端口为rtp端口+1
    u32              m_dwDestAddr;       //发送的目的地址
	u16              m_wDestPort;        //第一路发送目的端口，第二路发送端口为第一路发送目的端口 + 10
    	
    TVideoEncParam   m_atVidEncParam[MAXNUM_MPU_H_VOUTPUT]; //编码参数
	TMediaEncrypt    m_tMediaDec;        //解密参数
	TMediaEncrypt    m_atMediaEnc[MAXNUM_MPU_H_VOUTPUT];    //加密参数,与解密参数一致，不会用到？
	TDoublePayload   m_tDbPayload;       //载荷
	BOOL32           m_bNeedRS;          //是否需要重传
	TRSParam         m_tRsParam;         //重传参数
	u16              m_awBufTime[MAXNUM_MPU_H_VOUTPUT];     //发送缓存时间

    u8               m_byAudDecPT;       //中转音频的载荷
    u8               m_byAdpMode;        //码流切换标识，可能用到

	//FIXME：临时放开，稍后提供接口
public:
	CKdvMediaRcv*    m_pcMediaRcv;							 //一个接受
	CKdvMediaSnd*    m_pcMediaSnd[MAXNUM_MPU_H_VOUTPUT];     //最多6路发送(mpu为2路，mpu_h为6路)
	CKdvAdapterGroup m_cAptGrp;								 //一组适配
	
public:
	CKdvMediaRcv*    m_pcMediaAudRcv;     //增加一路音频接收		
	CKdvMediaSnd*    m_pcMediaAudSnd[MAXNUM_MPU_H_VOUTPUT];  //增加一路音频发送

public:
	void   SetAdpMode( u8 byType ) { m_byAdpMode = byType; }         
	u8     GetAdpMode( void ) const  { return m_byAdpMode; } 
	
    CMpuBasAdpGroup() : m_dwNetBand(0), m_bIsStart(FALSE), m_byAudDecPT(MEDIA_TYPE_NULL)
	{
	}
	~CMpuBasAdpGroup()
	{
		StopAdapter();
		m_cAptGrp.DestroyGroup();
	}
	BOOL IsStart( void )
	{
		return m_bIsStart;
	}
	BOOL Create( /*u32 dwMaxRcvFrameSize,*/ 
				 u16 wRtcpBackPort,    //rtcp回发端口
			     u16 wLocalStartPort,  //rtp端口，rtcp端口为rtp端口+1
			     u32 dwDestIp,         //发送目的Ip地址，
	             u16 dwDestPort,
			   	 u32 dwContext,
                 u8  byChnId );      //第一路发送目的端口，第二路发送端口为第一路发送目的端口 + 10
	void ResetDestPort(u16 wNewDstPort)
	{
		m_wDestPort = wNewDstPort;
	}

    //开始适配
	BOOL StartAdapter( BOOL32 bStartMediaRcv = FALSE );

	//停止适配
	BOOL StopAdapter( BOOL32 bStopMediaRcv = TRUE );

	//设置视频编码参数
    BOOL SetVideoEncParam( TVideoEncParam * ptVidEncParam, u8 byChnIdx, u8 byVailedChnNum, BOOL32 bSetNetParam = TRUE );

    //设置音频接收参数
    BOOL SetAudioParam( u8 byAudDecType, u8 byChnIdx);

    //设置视频编码加密key字串以及 加密模式 Aes or Des
    BOOL SetVidEncryptKey( const TMediaEncrypt& tMediaEnc , u8 byVailedChnNum )
	{
		if (0 == byVailedChnNum ||byVailedChnNum > MAXNUM_MPU_H_VOUTPUT )
		{
			OspPrintf(TRUE, FALSE, "[SetVidEncryptKey] byVailedChnNum is unexpected.%d!\n", byVailedChnNum);
		}

		u8  aEncKey[MAXLEN_KEY];
		s32 iLen = 0;
		const_cast<TMediaEncrypt&>(tMediaEnc).GetEncryptKey( aEncKey, &iLen );
		for (u8 byIdx = 0; byIdx < byVailedChnNum; byIdx++)
		{
			m_pcMediaSnd[byIdx]->SetEncryptKey( (s8*)aEncKey, iLen, const_cast<TMediaEncrypt&>(tMediaEnc).GetEncryptMode() );
		}
		//m_pcMediaSnd[0]->SetEncryptKey( (s8*)aEncKey, iLen, const_cast<TMediaEncrypt&>(tMediaEnc).GetEncryptMode() );
		return TRUE;
	}

    //设置第二路视频编码加密key字串以及 加密模式 Aes or DesBOOL SetSecVidEncryptKey(const TMediaEncrypt& tMediaEnc)
    BOOL SetSecVidEncryptKey( const TMediaEncrypt& tMediaEnc )    
	{
		u8  aEncKey[MAXLEN_KEY];
		s32 iLen = 0;
		const_cast<TMediaEncrypt&>(tMediaEnc).GetEncryptKey( aEncKey, &iLen );
		m_pcMediaSnd[1]->SetEncryptKey( (s8*)aEncKey, iLen, const_cast<TMediaEncrypt&>(tMediaEnc).GetEncryptMode() );
		return TRUE;
	}

	//设置 H.263+/H.264 等动态视频载荷的 Playload值
    BOOL SetVideoActivePT( u8 byRmtActivePT, u8 byRealPT )
	{
        m_pcMediaRcv->SetActivePT( byRmtActivePT, byRealPT );
		return TRUE;
	}

    void SetKeyandPT( u8* abyKeyBuf, u8 byKenLen, u8 byEncryptMode, u8 *pbySndActive, u8 byVailedOutNum, u8 byActivePT = 0, u8 byRealPT = 0/*u8 bySndActivePT0 = 0, u8 bySndActivePT1 = 0*/ )
    {
		if (0 == byVailedOutNum)
		{
			OspPrintf(TRUE, FALSE, "[SetKeyandPT] byVailedOutNum is 0\n");
			return;
		}
        // 进
        m_pcMediaRcv->SetActivePT( byActivePT, byRealPT );
        m_pcMediaRcv->SetDecryptKey( (s8*)abyKeyBuf, byKenLen, byEncryptMode );
        // 出
		for (u8 byIdx = 0; byIdx < byVailedOutNum; byIdx++)
		{		
			 m_pcMediaSnd[byIdx]->SetActivePT(pbySndActive[byIdx]);
			 m_pcMediaSnd[byIdx]->SetEncryptKey( (s8*)abyKeyBuf, byKenLen, byEncryptMode );
		}
//         m_pcMediaSnd[0]->SetActivePT( bySndActivePT0 );
//         m_pcMediaSnd[1]->SetActivePT( bySndActivePT1 );
//         m_pcMediaSnd[0]->SetEncryptKey( (s8*)abyKeyBuf, byKenLen, byEncryptMode );
//         m_pcMediaSnd[1]->SetEncryptKey( (s8*)abyKeyBuf, byKenLen, byEncryptMode );
    }

	/*设置图像的网络接收重传参数*/
	BOOL  SetNetRecvFeedbackVideoParam( TRSParam tNetRSParam, BOOL32 bRepeatSnd = FALSE );

	/*设置图像的网络发送重传参数*/
	BOOL  SetNetSendFeedbackVideoParam( u16 wBufTimeSpan, u8 byVailedOutNum, BOOL32 bRepeatSnd = FALSE );

	/*设置第二路适配图像的网络发送重传参数*/
	BOOL  SetNetSecSendFeedbackVideoParam( u16 wBufTimeSpan, BOOL32 bRepeatSnd = FALSE );

    // 平滑发送支持
// 	void SetSmoothSendRule( void );
// 	void ClearSmoothSendRule( void );
	//  void StopSmoothSend();	
	
	// [pengjie 2010/12/30] 设置（清除）本适配对象的平滑发送规则, FALSE:清除, TRUE:设置
	void SetSmoothSendRule( BOOL32 bNeedSet );	

    //请求关键帧
	BOOL SetFastUpdata( u8 byChnIdx );

	//得到视频通道的统计信息
	BOOL GetVideoChannelStatis( u8 byChnNo, TAdapterChannelStatis &tAdapterChannelStatis );

    void ShowChnInfo( void );
	
	// 为适配组设置网络发送IP地址
	void ModNetSndIpAddr(u32 dwSndIp);

	//根据bas启动模式获取输出通道数
	BOOL32 GetOutputChnNumAcd2WorkMode(u8 byWorkMode, u8 &byOutputChnNum);

private:
	BOOL SetVideoEncSend( u8  byIdx,            //发送对象索引
						  u32 dwNetBand,
		                  u16 wLoacalStartPort, //为本地接收的rtp端口，rtcp端口为rtp端口+1
			              u32 dwDestIp, 
                          u16 wDestPort );      //第一路发送目的端口，第二路发送端口为第一路发送目的端口 + 10)

// 	BOOL SetSecVideoEncSend( u32 dwNetBand,
// 		                     u16 wLoacalStartPort, //为本地接收的rtp端口，rtcp端口为rtp端口+1
// 			                 u32 dwDestIp,         //发送目的Ip地址，
// 	                         u16 dwDestPort );     //第一路发送目的端口，第二路发送端口为第一路发送目的端口 + 10)
    
    BOOL SetAudioSend( u8  byIdx,				//发送对象索引
					   u32 dwNetBand,
                       u16 wLocalStartPort,     //为本地接收的rtp端口，rtcp端口为rtp端口+1
                       u32 dwDestIp,            //发送目的Ip地址，
                       u16 wDestPort);          //第一路音频发送目的端口，视频对应端口＋2，第二路发送端口为第一路发送目的端口 + 10)

//     BOOL SetSecAudioSend( u32 dwNetBand,
//                           u16 wLocalStartPort,  //为本地接收的rtp端口，rtcp端口为rtp端口+1
//                           u32 dwDestIp,         //发送目的Ip地址，
//                           u16 wDestPort);       //第一路音频发送目的端口，视频对应端口＋2，第二路发送端口为第一路发送目的端口 + 10)
};

struct TMpuBasChannelInfo
{
    CConfId             m_cChnConfId;
    u8                  m_byChnConfType;    //服务于高清还是标清会议,可能会用到
    u8                  m_byCurrentState;   //闲置；预约；已经使用。可能会用到

    u32                 m_dwLastFUPTick;    //最后收到关键帧请求的Tick数

    THDBasVidChnStatus  m_tChnStatus;       //是否危险？这个结构体将作为ChnnlNotify的消息体

public:
    TMpuBasChannelInfo()
    {
        m_dwLastFUPTick = 0;
    }
};

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

/***********************************************************************************************************************************/
/*  MEDIA_TYPE_H262,          0,            300,       31,      2,       2048,       0,       0,     25,    5,       576,     720, */
/***********************************************************************************************************************************/
const u16 EnVidFmt[6][12]=
{
//类型               压缩码率控制参数 最大关键帧 最大量化 最小量化 编码比特率K 图像制式 采集端口 帧率       图像质量         视频高度 视频宽度
//mpeg4
 MEDIA_TYPE_MP4,           0,            25,        31,      2,       1200,       0,       0,     25,    1/*QC_MODE_QUALITY*/,       576,     720,
//261
 MEDIA_TYPE_H261,          0,            25,        31,      2,       1200,       0,       0,     25,    1/*QC_MODE_QUALITY*/,       576,     720,
//262(mpeg-2)
 MEDIA_TYPE_H262,          0,            25,        31,      2,       2048,       0,       0,     25,    1/*QC_MODE_QUALITY*/,       576,     720,
//263
 MEDIA_TYPE_H263,          1,            25,        31,      2,       1200,       0,       0,     25,    1/*QC_MODE_QUALITY*/,       576,     720,
 //h263+
 MEDIA_TYPE_H263PLUS,      0,            300,       31,      2,       1200,       0,       0,     25,    1/*QC_MODE_QUALITY*/,       576,     720,
//h264
 MEDIA_TYPE_H264,          0,            300,       51,      2,       2048,       0,       0,     25,    1/*QC_MODE_QUALITY*/,       576,     720
};

// 以下为vmp外设结构
class TMpuVmpCfg
{
public:
	//TMulPicParam m_tMulPicParam;   //图像参数设置
    u32  m_adwMaxSendBand[MAXNUM_MPUSVMP_CHANNEL];        //最大网络发送带宽
    
public:    
    // 图像编码参数
	TVideoEncParam m_tVideoEncParam[MAXNUM_MPUSVMP_CHANNEL];
    TDebugVal m_tDebugVal;           //作弊值

public:
    TMpuVmpCfg(){}
    ~TMpuVmpCfg(){}
	void PrintEncParam( u8 byVideoIdx ) const;
	void GetDefaultParam( u8 byEnctype, TVideoEncParam& TEncparam, u32& byNetBand );
    void ReadDebugValues();
    u8   GetVmpChlNumByStyle(u8 byStyle);

    BOOL32 IsEnableBitrateCheat() const
    {
        return m_tDebugVal.IsEnableBitrateCheat();
    }
    u16 GetDecRateDebug( u16 wOriginRate ) const
    {
        return m_tDebugVal.GetDecRateDebug( wOriginRate );
    }
    u32 GetAudioDeferNum( void )
    {
        return m_tDebugVal.GetAudioDeferNum();
    }
	
	u16 GetOrigRate(u16 wDbgRate) const
    {
        return m_tDebugVal.GetOrigRate(wDbgRate);
    }

	u32 GetVidSDMode(void) 
	{
		return m_tDebugVal.GetVidSDMode();
	}

	u32 GetIframeInterval(void){ return m_tDebugVal.GetVmpIframeInterval(); }

	const s8* GetVmpMemAlias(u8 byIndex) const
    {
        return m_tDebugVal.GetVmpMemAlias(byIndex);
    }
	
	const s8* GetVmpMemRoute(u8 byIndex) const
    {
        return m_tDebugVal.GetVmpMemRoute(byIndex);
    }
	
	BOOL32 TableMemoryFree( void **ppMem, u32 dwEntryNum );

	void Print()
	{
		m_tDebugVal.Print();
	}
};

/************************************************************************/
/*                                                                      */
/*                       唇音同步相关数据结构                           */
/*                                                                      */
/************************************************************************/

#define MAXNUM_BUF_LEN                  (u16)(1024 * 2)
#define MAXNUM_BUF_FRM                  (u16)(100)

//2k的静态buf
struct TBuf
{
public:
    void SetNull(void)
    {
        memset(&m_abyBuf, 0, sizeof(m_abyBuf));
        m_dwLen = 0;
    }
public:
    u32 m_dwLen;
    u8  m_abyBuf[MAXNUM_BUF_LEN];
};

//某交换缓冲队列
struct TRtpPkQueue
{
public:
    TRtpPkQueue::TRtpPkQueue()
    {
        memset(this, 0, sizeof(TRtpPkQueue));
    }

    //  申请内存
	//  pengjie[8/7/2009] 修改了申请内存失败的处理
    void Init(u32 dwLen)
    {
        if ( 0 == dwLen ) 
        {
            OspPrintf( TRUE, FALSE, "[TRtpPkQueue::Init] err: Len.%d, Init failed !\n", dwLen );
            return;
        }

        m_ptBuf   = new TBuf[dwLen];
        m_ptRtpPk = new TRtpPack[dwLen];
		if((NULL != m_ptBuf) && (NULL != m_ptRtpPk))
		{
			memset(m_ptBuf, 0, sizeof(TBuf) * dwLen);
			memset(m_ptRtpPk, 0, sizeof(TRtpPack) * dwLen);
			m_dwLen = dwLen;
		}
		else
		{
            delete [] m_ptBuf; 
			delete [] m_ptRtpPk;
            m_ptBuf = NULL;  
            m_ptRtpPk = NULL;

            OspPrintf( TRUE, FALSE, "[TRtpPkQueue::Init] Init failed !\n" );
			return;
		}
		return;
    }

    //释放内存
    void Quit(void)
    {
        if ( NULL != m_ptBuf ) 
        {
            delete [] m_ptBuf;
            m_ptBuf = NULL;
        }
        if ( NULL != m_ptRtpPk)
        {
            delete [] m_ptRtpPk;
            m_ptRtpPk = NULL;
        }
        m_dwLen = 0;
        m_dwHead = 0;
        m_dwTail = 0;
        return;
    }

    //增加buf到队尾
    BOOL32 IncBuf( u8 *pUdpData, u32 dwUdpLen, TRtpPack &tRtpPk )
    {
        /*
        //为了提高回调处理效率，不影响功能，注释之
        if ( IsQueueFull() )
        {
            OspPrintf(TRUE, FALSE, "[TRtpPkQueue::IncBuf] Buffer Full!\n");
            return FALSE;
        }*/

        if (!IsQueueNull())
        {
            //新码流到来，清空当前队列
            if (tRtpPk.m_dwSSRC != m_ptRtpPk[m_dwHead].m_dwSSRC)
            {
                Clear();
                OspPrintf(TRUE, FALSE, "[TRtpPkQueue][IncBuf] new SSRC.%d !eq HeadSSRC.%d, clear queue!\n",
                                        tRtpPk.m_dwSSRC,
                                        m_ptRtpPk[m_dwHead].m_dwSSRC);
            }
        }

        if (dwUdpLen >= MAXNUM_BUF_LEN)
        {
            OspPrintf(TRUE, FALSE, "[TRtpPkQueue::IncBuf] new Udp data len.%d is unexpected!\n", dwUdpLen);
            return FALSE;
        }

        m_ptBuf[m_dwTail].m_dwLen = min(dwUdpLen, MAXNUM_BUF_LEN);
        memcpy(&m_ptBuf[m_dwTail].m_abyBuf, pUdpData, min(dwUdpLen, MAXNUM_BUF_LEN));
        m_ptRtpPk[m_dwTail] = tRtpPk;

        m_dwTail = (m_dwTail + 1) % m_dwLen;

        //OspPrintf(TRUE, FALSE, "[TRtpPkQueue::IncBuf] head.%d, tail.%d\n", m_dwHead, m_dwTail);
        return TRUE;
    }

    //队列满，提取并删除队头的buf
    BOOL32 DecBuf( u8 *pUdpData, u32 &dwUdpLen, TRtpPack &tRtpPk )
    {
        if (IsQueueNull())
        {
            OspPrintf(TRUE, FALSE, "[TRtpPkQueue::DecBuf] no udp data in exist, ignore it!\n");
            return FALSE;
        }
        /*
        if (dwUdpLen == 0 || dwUdpLen < m_ptBuf[m_dwHead].m_dwLen)
        {
            OspPrintf(TRUE, FALSE, "[TRtpPkQueue::DecBuf] udp buf len.%d is less than.%d, ignore it!\n",
                                    dwUdpLen, m_ptBuf[m_dwHead].m_dwLen);
            return FALSE;
        }*/

        dwUdpLen = m_ptBuf[m_dwHead].m_dwLen;
        memcpy(pUdpData, m_ptBuf[m_dwHead].m_abyBuf, dwUdpLen);
        tRtpPk = m_ptRtpPk[m_dwHead];

        m_ptBuf[m_dwHead].SetNull();

        m_dwHead = (m_dwHead + 1) % m_dwLen;
        
        //OspPrintf(TRUE, FALSE, "[TRtpPkQueue::DecBuf] head.%d been remove, tail.%d\n", m_dwHead, m_dwTail);
        
        return TRUE;
    }

    BOOL32 IsQueueFull(void)
    {
        BOOL32 bFull = m_ptBuf[m_dwTail].m_dwLen != 0;
        /*
        OspPrintf(TRUE, FALSE, "[TRtpPkQueue::IsQueueFull] head.%d, tail.%d, len.%d, full.%d!\n",
                                m_dwHead, m_dwTail, m_ptBuf[m_dwTail].m_dwLen, bFull);
                                */
        return bFull;
    }
    BOOL32 IsQueueNull(void)
    {
        return m_ptBuf[m_dwHead].m_dwLen == 0;
    }
    u32 GetPackLen(void)
    {
        return m_dwLen;
    }

    void Clear()
    {
        m_dwHead = 0;
        m_dwTail = 0;
        for(u8 byIdx = 0; byIdx < m_dwLen; byIdx++)
        {
            m_ptBuf[byIdx].SetNull();
        }
    }

    void Print()
    {
        OspPrintf(TRUE, FALSE, "\nTFrmQueue <H.%d, T.%d, L.%d>:\n\n", m_dwHead, m_dwTail, m_dwLen);

        for (u8 byIdx = 0; byIdx < m_dwLen; byIdx ++)
        {
            if (byIdx == m_dwHead && byIdx == m_dwTail)
            {
                OspPrintf(TRUE, FALSE, "\t[%d] m_pBufLen.%d, Type.%d, Seq.%d  <-- HEAD/TAIL\n", 
                    byIdx,
                    m_ptBuf[byIdx].m_dwLen,
                    m_ptRtpPk[byIdx].m_byPayload,
                    m_ptRtpPk[byIdx].m_wSequence );
            }
            else if (byIdx == m_dwHead)
            {
                OspPrintf(TRUE, FALSE, "\t[%d] m_pBufLen.%d, Type.%d, Seq.%d <-- HEAD\n", 
                    byIdx,
                    m_ptBuf[byIdx].m_dwLen,
                    m_ptRtpPk[byIdx].m_byPayload,
                    m_ptRtpPk[byIdx].m_wSequence );
            }
            else if (byIdx == m_dwTail)
            {
                OspPrintf(TRUE, FALSE, "\t[%d] m_pBufLen.%d, Type.%d, Seq.%d  <-- TAIL\n", 
                    byIdx,
                    m_ptBuf[byIdx].m_dwLen,
                    m_ptRtpPk[byIdx].m_byPayload,
                    m_ptRtpPk[byIdx].m_wSequence );
            }
            else
            {
                OspPrintf(TRUE, FALSE, "\t[%d] m_pBufLen.%d, Type.%d, Seq.%d \n", 
                    byIdx,
                    m_ptBuf[byIdx].m_dwLen,
                    m_ptRtpPk[byIdx].m_byPayload,
                    m_ptRtpPk[byIdx].m_wSequence );
            }
        }
    }
private:

    u32 m_dwLen;
    u32 m_dwHead;
    u32 m_dwTail;
    TBuf *m_ptBuf;
    TRtpPack *m_ptRtpPk;
};

#endif // !_MPUSTRUCT_H_

//END OF FILE

