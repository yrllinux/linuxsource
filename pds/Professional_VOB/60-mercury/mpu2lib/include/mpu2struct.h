/*****************************************************************************
   模块名      : mpu2lib
   文件名      : mpu2struct.h
   相关文件    : 
   文件实现功能: mpu2lib结构和回调声明 头文件
   作者        : 倪志俊
   版本        : V4.7  Copyright(C) 2012-2015 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期       版本        修改人      修改内容
   2012/07/26  4.7         倪志俊       注释
******************************************************************************/
#ifndef _MPUSTRUCT_H_
#define _MPUSTRUCT_H_

#include "mcustruct.h"
#include "eqpcfg.h"
#include "./dm816x/kdvmulpic_hd.h"
#include "kdvmedianet.h"
#include "./dm816x/kdvadapter_hd.h"

extern s8	MPULIB_CFG_FILE[KDV_MAX_PATH];

void CBMpu2BasRecvFrame( PFRAMEHDR pFrmHdr, u32 dwContext );
void CBMpu2BasSendFrame( PTFrameHeader pFrmHdr, void* dwContext );
void CBMpuDVmpRecvFrame( PFRAMEHDR pFrmHdr, u32 dwContext );
void CBMpuSVmpRecvFrame( PFRAMEHDR pFrmHdr, u32 dwContext );
void CBMpuEVpuRecvFrame( PFRAMEHDR pFrmHdr, u32 dwContext );
void VMPCALLBACK( PTFrameHeader pFrameInfo, void* pContext );
void CBMpuBasAudRecvRtpPk(TRtpPack *pRtpPack, u32 dwContext);

#define MIN_BITRATE_OUTPUT             (u16)128
#define MPU_CONNETC_TIMEOUT            (u16)3*1000     // 连接超时值3s
#define MPU_REGISTER_TIMEOUT           (u16)5*1000     // 注册超时值5s
#define MPU_SECOND_REGISTER_TIMEOUT    (u16)7*1000     // 第二块外设注册超时值7s
#define CHECK_IFRAME_INTERVAL          (u16)1000       // 检测关键帧时间间隔(ms)
#define FIRST_REGACK                   (u8)1           // 第一次收到注册Ack
#define PORT_SNDBIND                   20000
#define VMP_SMOOTH_OPEN	               (u8)1
#define	MAX_BAS_NUM						(u8)2

#ifdef WIN32//songkun,20110628,加入MPU WIN32 B版编译
    #define MAXNUM_MPUSVMP_CHANNEL		 (u8)4
#else
    #ifndef _MPUB_256_
        #define MAXNUM_MPUSVMP_CHANNEL		 (u8)3
    #else
        #define MAXNUM_MPUSVMP_CHANNEL		 (u8)4
    #endif
#endif

#define MAXNUM_MPU2VMP_CHANNEL         (u8)9

//TODO:暂时定义灯的个数为5，等实际板子来了再确定
#ifdef _8KI_
#define MPU2_LED_NUM                   (u8)3//TODO:need确切定义
#else
#define MPU2_LED_NUM                   (u8)2
#endif

#define TEST_LED_TIMER_INTERVAL         (long)2*1000

#define DEFAULT_BMP__PATH        (LPCSTR)"/usr/etc/config/dft_16to9.bmp"
#define USR_BMP_PATH				(LPCSTR)"/usr/etc/config/usr_16to9.bmp"


#define  MAXNUM_VMPDEFINE_PIC MAXNUM_MPU2VMP_MEMBER

// 画面合成成员别名 [7/3/2013 liaokang]
struct TVmpMbAlias
{
public:  
    BOOL32  SetMbAlias( u8 byAliasLen, const s8 *pAlias ) 
    { 
        if( NULL == pAlias )
        {
            return FALSE;
        }
        memset(m_achMbAlias, 0, sizeof(m_achMbAlias));
        memcpy(m_achMbAlias, pAlias, min(byAliasLen, MAXLEN_ALIAS-1));
        m_achMbAlias[MAXLEN_ALIAS-1] = '\0';
        return TRUE; 
    }
    const s8* GetMbAlias(void) { return m_achMbAlias; }
    
protected:
    s8   m_achMbAlias[MAXLEN_ALIAS];
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

struct TVmpMemPicRoute
{
private:
    s8 m_szVmpMemAlias[/*VALIDLEN_ALIAS+1*/MAXLEN_ALIAS]; // 扩容 [7/3/2013 liaokang]
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
	BOOL32			m_bEnableSmoothSend;	//是否开启平滑发送
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

	void   SetEnableSmoothSend( BOOL32 bEnable )
	{
		m_bEnableSmoothSend = bEnable;
	}
	BOOL32 IsEnableSmoothSend() const
	{
		return m_bEnableSmoothSend;
	}

    u16    GetDecRateDebug( u16 wOriginRate ) const
    {	
		if ( wOriginRate == 0 )
		{
			return wOriginRate;
		}

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
		OspPrintf(TRUE,FALSE,"IsEnableSmoothSend: %d\n",m_bEnableSmoothSend);
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

struct TMpu2BasCfg
{
	u32				m_dwMcuIId;				//主MCU的通信实例号
	u32				m_dwMcuIIdB;			//备MCU的通信实例号 
	u32				m_dwMcuRcvIp;			//转发板IP
	u16				m_wMcuRcvStartPort;		//转发板起始端口
	u8				m_byRegAckNum;			//注册成功计数
	u32				m_dwSSrc;				//会话校验值
	TEqp			m_tEqp;					//外设
	TPrsTimeSpan	m_tPrsTimeSpan;			//重传参数
	TEqpBasicCfg			m_tCfg;					//外设配置
	TMpu2BasCfg()
	{
		ClearDaTa();
	}
	
	~TMpu2BasCfg()
	{
		ClearDaTa();
	}
	
	void ClearDaTa()
	{
		m_dwMcuIId = 0;
		m_dwMcuIIdB = 0;
		m_dwMcuRcvIp =0;
		m_wMcuRcvStartPort = 0;
		m_byRegAckNum = 0;
		m_dwSSrc = 0;
	}
	
	void PrintfBasCfg()
	{
		OspPrintf(TRUE,FALSE,"BasEqp: %d\n",m_tEqp.GetEqpId());
		OspPrintf(TRUE,FALSE,"m_dwMcuIId: %d\n",m_dwMcuIId);
		OspPrintf(TRUE,FALSE,"m_dwMcuIIdB: %d\n",m_dwMcuIIdB);
		OspPrintf(TRUE,FALSE,"m_dwMcuRcvIp: 0x%x\n",m_dwMcuRcvIp);
		OspPrintf(TRUE,FALSE,"m_wMcuRcvStartPort: %d\n",m_wMcuRcvStartPort);
		OspPrintf(TRUE,FALSE,"m_byRegAckNum: %d\n",m_byRegAckNum);
		OspPrintf(TRUE,FALSE,"m_dwSSrc: %d\n",m_dwSSrc);
	}

};

//BAS全局数据区
class CMpu2BasData
{
private:
    u32              m_dwMcuNode;					 // 与MCU.A通信节点号
    u32              m_dwMcuNodeB;				 	 // 与MCU.B通信节点号
	u8               m_byWorkMode;					 // 避免使用全局变量,在此保存WorkMode
	u16				 m_wMTUSize;					 // MTU大小
	u8				 m_byBasNum;					 // BAS外设个数
	TMpu2BasCfg	     m_atMpu2BasCfg[MAX_BAS_NUM];	 // 外设对应配置
    TDebugVal        m_tDebugVal;					 // 作弊值	
	CKdvVideoAdapter m_cAptGrp[MAX_BAS_NUM];		 // 适配器媒控对象,enchance可以起两个BASIC模式
	
public:
    CMpu2BasData(){}
    virtual ~CMpu2BasData(){}
	

	CKdvVideoAdapter*	GetVideoAdapter(u8 byIdx)
	{
		if ( (m_byBasNum-1) < byIdx)
		{
			return NULL;
		}
		
		return &m_cAptGrp[byIdx];
	}

	//适配媒控层封装
	u16	 InitVideoAdapter(u8 byBasIdx);									//初始化BAS适配器
	u16  DestroyVideoAdapter(u8 byBasIdx);								//退出BAS适配器
	u16  StartVideoAdapter(u8 byBasIdx,u8 byChnId);						//开启某个BAS通道适配
	u16  StopVideoAdapter(u8 byBasIdx,u8 byChnId);						//停止某个BAS通道适配
	u16	 AddBasChnnl(u8 byBasIdx,const TAdapterChannel *ptAdpChannel);	//添加一个BAS通道初始化
	u16	 AddNewEncParam(u8 byBasIdx,u8 byChnId,u8 byOutIdx,const TVideoEncParam &tVidEncParam);
	u16	 RemoveBasChnnl(u8 byBasIdx,u8 byChnId);						//删除一个BAS通道
	u16	 GetBasChnDecStatis(u8 byBasIdx,u8 byChnId,TKdvDecStatis &tVidDecStatis);//获得某个通道的解码状态，用于请求关键帧
	u16	 ChangeBasVideoEncParam(u8 byBasIdx,u8 byChnId, TAdapterEncParam &tVidEncParam);	//BAS编码下参使用
	u16	 SetBasVidDataCallback(u8 byBasIdx,u8 byChnId, u32 dwEncoderId, FRAMECALLBACK fVidData, void* pContext);//设置视频数据回调，编码器
	u16	 SetFrameData(u8 byBasIdx,u8 byChnId, const TFrameHeader& tFrameInfo);	//设置帧数据
	u16	 SetEncFastUpdata(u8 byBasIdx,u8 byChnId);								//设置编码关键帧
	u16	 SetEncResizeMode(u8 byBasIdx,u8 byChnId,u32 dwMode);					//设置黑边剪裁
	void ShowBasChnStatus(u8 byBasIdx,u8 byChnId,BOOL32 bIsEnc = TRUE);			//显示BAS状态

	void FreeStatusDataA( void );
	void FreeStatusDataB( void );
	

	//外设相关配置数据
	void SetBasWorkMode(u8 byWorkMode)
	{
		m_byWorkMode = byWorkMode;
	}

	u8   GetBasWorkMode()
	{
		return m_byWorkMode;
	}

	void SetMcuANode(u32 dwMcuNode)
	{
		m_dwMcuNode = dwMcuNode;
	}

	u32  GetMcuANode()
	{
		return m_dwMcuNode;
	}

	void SetMcuBNode(u32 dwMcuNode)
	{
		m_dwMcuNodeB = dwMcuNode;
	}

	u32  GetMcuBNode()
	{
		return m_dwMcuNodeB;
	}

	void SetMTUSize(u16 wMTUSize)
	{
		m_wMTUSize = wMTUSize;
	}
	
	u16  GetMTUSize()
	{
		return m_wMTUSize;
	}
	
	
	void SetBasNum(u8 byBasNum)
	{
		m_byBasNum = byBasNum;
	}

	u8	 GetBasNum()
	{
		return m_byBasNum;
	}

	void SetBasData(TMpu2Cfg *pMpu2Cfg);		//设置BAS外设数据配置
	
	void	SetMpu2BasCfg(u8 byIdx, const TMpu2BasCfg &tMpu2BasCfg);	//设置BAS外设配置
	BOOL32  GetMpu2BasCfg(u8 byIdx, TMpu2BasCfg &tMpu2BasCfg);	

	//作弊文件值
    void ReadDebugValues();
	TDebugVal GetDebugVal();

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

	u32   GetIframeInterval(void) 
	{ 
		return m_tDebugVal.GetBasIframeInterval(); 
	}

	BOOL32 IsEnableSmoothSend()
	{
		return m_tDebugVal.IsEnableSmoothSend();
	}

	void ShowBasCfgData()
	{
		for ( u8 byIdx=0; byIdx<m_byBasNum;byIdx++ )
		{
			m_atMpu2BasCfg[byIdx].PrintfBasCfg();
		}
	}

};

class CMpu2BasAdpGroup // 可能根据底层接口调整而调整
{
private:
	BOOL32           m_bIsStart;			//标志位，表示是否已经开启适配
	u8				 m_byOutChnlNum;		//通道对应编码输出路数
	u32              m_dwNetBand;			//带宽	
	u16				 m_wLoclRcvStartPort;	//本地接收码流的RTP起始端口
	u32				 m_dwDestIp;			//发送目的的地址
	u16				 m_wDestStartPort;		//发送目的的起始端口，第二路发送端口为第一路发送目的端口 + 10
	
	u8				 m_byWorkMode;
	u8				 m_byChnId;			    //BAS通道ID，如果是两个外设，也还是各自起始为0
	u8				 m_byBasIdx;			//BAS外设ID

	u8               m_byAudDecPT;			//中转音频的载荷，唇音同步
// 	TDoublePayload   m_tDbPayload;			//解码载荷
// 	TMediaEncrypt    m_tMediaDec;			//解密参数

	u16				 m_awRawBiteRate[MAXNUM_BASOUTCHN];		//原始码率，用于平滑发送设置
	u16				 m_awPreRawBiteRate[MAXNUM_BASOUTCHN];	//原始码率，用于比较是否设置
    TAdapterEncParam m_atVidEncParam[MAXNUM_BASOUTCHN];		//编码参数
	TMediaEncrypt    m_atMediaEnc[MAXNUM_BASOUTCHN];			//编码加密参数

	//丢包重传相关
	BOOL32           m_bNeedRS;									//是否需要重传
	TRSParam         m_tRsParam;								//重传参数，用于解码
	u16              m_awBufTime[MAXNUM_BASOUTCHN];	//重传参数，发送缓存时间，用于编码

	//网络对象
	CKdvMediaRcv*    m_pcMediaVidRcv;							  //视频码流接收对象
	CKdvMediaRcv*    m_pcMediaAudRcv;							  //音频码流接收接收
	CKdvMediaSnd*    m_pcMediaVidSnd[MAXNUM_BASOUTCHN];  //视频码流发送对象数组
	CKdvMediaSnd*    m_pcMediaAudSnd[MAXNUM_BASOUTCHN];  //音频码流发送对象数组
		
public:	
    CMpu2BasAdpGroup() : m_dwNetBand(0), m_bIsStart(FALSE), m_byAudDecPT(MEDIA_TYPE_NULL)
	{
		memset(	m_awRawBiteRate,0,sizeof(m_awRawBiteRate) );
		memset(	m_awPreRawBiteRate,0,sizeof(m_awPreRawBiteRate) );
	}
	~CMpu2BasAdpGroup()
	{
		u8 byOutNum = m_byOutChnlNum;
		StopAdapter();
		//释放网络收发对象
		if ( m_pcMediaVidRcv != NULL )
		{
			delete m_pcMediaVidRcv;
			m_pcMediaVidRcv = NULL;
		}
		
		if ( m_pcMediaAudRcv != NULL )
		{
			delete m_pcMediaAudRcv;
			m_pcMediaAudRcv = NULL;
		}
		
		for (u8 byIdx = 0; byIdx < byOutNum; byIdx++)
		{
			if ( m_pcMediaVidSnd[byIdx]!= NULL )
			{
				delete m_pcMediaVidSnd[byIdx];
				m_pcMediaVidSnd[byIdx] = NULL;
			}
			
			if ( m_pcMediaAudSnd[byIdx]!= NULL )
			{
				delete m_pcMediaAudSnd[byIdx];
				m_pcMediaAudSnd[byIdx] = NULL;
			}
		}		
	}

	BOOL Init(	 u16 wLocalRcvStartPort,    //rtp码流接收端口，本地rtcp端口为rtp端口+1
				 u16 wRtcpBackPort,         //rtcp回发端口(转发板端口)，目前策略是使用RTP接收端口+1		   
			     u32 dwDestIp,				//发送目的Ip地址，
	             u16 dwDestPort,			//发送目的起始端口
			   	 u32 dwContext,					
                 u8  byChnId, 
				 u8	 byOutChnlNum,			//编码发送路数
				 u8	 byWorkMode,
				 u8  byBasIdx
				 );	

    //开始适配
	BOOL StartAdapter( u8 byBasIdx, BOOL32 bStartMediaRcv = FALSE );

	//停止适配
	BOOL StopAdapter( BOOL32 bStopMediaRcv = TRUE );

	//设置视频编码参数
    BOOL SetVideoEncParam( u32 dwDecMediaType,TAdapterEncParam * aptVidEncParam, THDAdaptParam *aptBasParam,THDAdaptParam *aptSpecialBasParam,u8 byVailedChnNum, BOOL32 bSetNetParam = TRUE,BOOL32 bChangeParam =FALSE );
	
	//设置视频解码时的载荷和解密参数，以及编码时加密参数和载荷
	void SetKeyAndPT( u8* abyKeyBuf, 
		u8 byKenLen, 
		u8 byEncryptMode, 
		u8 *pbySndActive, 
		u8 byVailedOutNum, 
		u8 byActivePT = 0, 
		u8 byRealPT = 0
		);

    //设置音频码流接收参数
    BOOL SetAudioParam( u8 byAudDecType);
	
	/*设置码流接收重传参数*/
	BOOL  SetNetRecvFeedbackVideoParam( TRSParam tNetRSParam, BOOL32 bRepeatSnd = FALSE );
	
	/*设置码流发送重传参数*/
	BOOL  SetNetSendFeedbackVideoParam( u16 wBufTimeSpan, u8 byVailedOutNum, BOOL32 bRepeatSnd = FALSE );
  
	//设置（清除）本适配对象的平滑发送规则, FALSE:清除, TRUE:设置
	void SetSmoothSendRule( BOOL32 bNeedSet );	
	
    //请求关键帧
	BOOL SetFastUpdata();

	// 为适配组设置网络发送IP地址
	void ModNetSndIpAddr(u32 dwSndIp);
	
   	//得到视频通道的统计信息
	BOOL GetVideoChannelStatis(TKdvDecStatis &tAdapterDecStatis );

	void ShowChnInfo();

	inline void SetStart(BOOL32 bIsStart)
	{
		m_bIsStart = bIsStart;
		return;
	}
	
	inline BOOL IsStart( void )
	{
		return m_bIsStart;
	}

	u16 SetResizeMode(u32 dwResizeMode);
	u16 SetAdpGrpData(const TFrameHeader& tFrameInfo);

	//音频发送时采用友元来访问音频发送对象
	friend void CBMpuBasAudRecvRtpPk(TRtpPack *pRtpPack, u32 dwContext);
private:

	BOOL SetVideoEncSend( u8  byIdx,            //发送对象索引
						  u32 dwNetBand,
		                  u16 wLoacalStartPort, //为本地接收的rtp端口，rtcp端口为rtp端口+1
			              u32 dwDestIp, 
                          u16 wDestPort );      //第一路发送目的端口，第二路发送端口为第一路发送目的端口 + 10)
    
    BOOL SetAudioSend( u8  byIdx,				//发送对象索引
					   u32 dwNetBand,
                       u16 wLocalStartPort,     //为本地接收的rtp端口，rtcp端口为rtp端口+1
                       u32 dwDestIp,            //发送目的Ip地址，
                       u16 wDestPort);          //第一路音频发送目的端口，视频对应端口＋2，第二路发送端口为第一路发送目的端口 + 10)
  
	BOOL AddBasChn(u32 dwDecMediaType,TAdapterEncParam * ptVidEncParam, u8 byVailedChnNum);

	BOOL AddNewEncParam(const TVideoEncParam &tVidEncParam, u8 byEncIdx);

	BOOL  IsParamChange(u8 byOutIdx, const TVideoEncParam &tOldParam, const TVideoEncParam &tNewParam);

	BOOL  IsAddNewParam(const TVideoEncParam &tOldParam, const TVideoEncParam &tNewParam);

	void  ConverToRealParam2(u8 byOutIdx, TAdapterEncParam &tVidEncParam, THDAdaptParam &tBasParam);

	BOOL32 IsNeedConvertToSpecialParam(THDAdaptParam &tRealParam,THDAdaptParam &tSpecialParam);

	void ConverToRealParam3(u8 byOutIdx,THDAdaptParam &tRealParam,THDAdaptParam &tSpecialParam,TAdapterEncParam &tVidEncParam);
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
	
	u8 GetOutChnlVidTpye(u8 byOutChnIdx)
	{
		return m_tChnStatus.GetOutputVidParam(byOutChnIdx)->GetVidType();
	}
	
	THDAdaptParam* GetBasOutPutVidParam(u8 byOutChnIdx)
	{
		return m_tChnStatus.GetOutputVidParam(byOutChnIdx);
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
    u32  m_adwMaxSendBand[MAXNUM_MPU2VMP_CHANNEL];        //最大网络发送带宽
    
public:    
    // 图像编码参数
	TVideoEncParam m_tVideoEncParam[MAXNUM_MPU2VMP_CHANNEL];
    TDebugVal m_tDebugVal;           //作弊值
	u8 m_aVMPStyleChnlRes[VMPSTYLE_NUMBER][MAXNUM_MPU2VMP_MEMBER];

public:
    TMpuVmpCfg()
	{
		InitVmpStyleChnlRes();
	}
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

	//初始化vmp各个style对应的各个通道的分辨率
	void InitVmpStyleChnlRes( );

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
    TRtpPkQueue()
    {
        memset(this, 0, sizeof(TRtpPkQueue));
    }

	~TRtpPkQueue()
	{
		Quit();
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

