/*****************************************************************************
模块名      : 高清解码单元
文件名      : hduinst.h
相关文件    : 
文件实现功能: hdu实例头文件
作者        : 江乐斌
版本        : V1.0  Copyright(C) 2008-2010 KDC, All rights reserved.
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
08/11/26    4.6         江乐斌        创建
******************************************************************************/
#ifndef _HDU_INST_H_
#define _HDU_INST_H_

#include "osp.h"
#include "mcustruct.h"
#include "eqpcfg.h"

#include "mcuconst.h"
#include "kdvmedianet.h"
#include "codecwrapper_hd.h"
#include "codecwrapperdef_hd.h"
#include <fstream>

#include "hduagent.h"

#include "evhduid.h"
#include "hdustruct.h"
#include "errhdu.h"
#include "evmcueqp.h"
#include "mcuver.h"


#define MAX_VIDEO_FRAME_SIZE (512 * 1024)

#define MAX_AUDIO_FRAME_SIZE (8 * 1024)


/*     打印级别    */
#define LOG_CRIT            (u8)0
#define LOG_ERR             (u8)1
#define LOG_WARNING            (u8)2
#define LOG_INFO            (u8)3



class CDecoderGrp
{
public:
	CKdvMediaRcv     	m_cVidMediaRcv;        	//一个视频接收
	CKdvMediaRcv     	m_cAudMediaRcv;        	//一个音频接收
	CKdvVidDec    	    m_cVidDecoder;          //视频解码器
    CKdvAudDec          m_cAudDecoder;          //音频解码器
	
private:
	BOOL32           	m_bIsStart;             //是否启动
	u8					m_byLastOutPutMode;		//记录上次输出模式
	u32              	m_dwNetBand;            //网络绑定
	u32              	m_dwRtcpBackAddr;       //rtcp回发地址
	u16              	m_wRtcpBackPort;    	//rtcp回发端口
	u16              	m_wLocalStartPort;  	//为本地接收的rtp端口，rtcp端口为rtp端口+1
	
	TMediaEncrypt       m_tMediaDec;            //解密参数
	BOOL32           	m_bIsNeedRS;         	//是否需要重传
	TRSParam         	m_tRsParam;         	//重传参数
public:
	CDecoderGrp()
	{
		m_bIsStart = 0;
		m_byLastOutPutMode = 0;
		m_dwNetBand = 0;
		m_dwRtcpBackAddr = 0;
		m_wRtcpBackPort = 0;
		m_wLocalStartPort = 0;
		m_bIsNeedRS = 0;
		memset((void*)&m_tMediaDec, 0x0, sizeof(m_tMediaDec));
        memset((void*)&m_tRsParam, 0x0, sizeof(m_tRsParam));
	}
	~CDecoderGrp()
	{
        //是否要停止销毁相应的音视频解码器
	}
public:
	BOOL32    IsStart(void){ return m_bIsStart; }
	void      SetIsStart( BOOL32 bIsStart ){ m_bIsStart = bIsStart; }

	u8        GetLastOutPutMode(void){ return m_byLastOutPutMode; }
	void      SetLastOutPutMode( u8	byLastOutPutMode ){ m_byLastOutPutMode = byLastOutPutMode; }

    u32       GetNetBand( void ){ return m_dwNetBand; }
	void      SetNetBand( u32 dwNetBand ){ m_dwNetBand = dwNetBand; }
    
	u32       GetRtcpBackAddr( void ){ return m_dwRtcpBackAddr; }
	void      SetRtcpBackAddr( u32 dwRtcpBackAddr ){ m_dwRtcpBackAddr = dwRtcpBackAddr; }
	
	u16       GetRtcpBackPort( void ){ return m_wRtcpBackPort; }
	void      SetRtcpBackPort( u16 wRtcpBackPort ){ m_wRtcpBackPort = wRtcpBackPort; }
    
	u16       GetLocalStartPort(void){ return m_wLocalStartPort; }
	void      SetLocalStartPort(u16 wLocalStartPort){ m_wLocalStartPort = wLocalStartPort; }

	BOOL32    GetMediaEncrypt(TMediaEncrypt& tMediaEncrypt)
	{
        memcpy( &tMediaEncrypt, &m_tMediaDec, sizeof(TMediaEncrypt) );
		return TRUE;
	}
	void      SetMediaEncrypt(TMediaEncrypt *ptMediaEncrypt)
	{
		memcpy( &m_tMediaDec, ptMediaEncrypt, sizeof(TMediaEncrypt) );
		return;
	}

	BOOL32    IsNeedRS(void){ return m_bIsNeedRS; }
	void      SetIsNeedRS(BOOL32 bIsNeedRS){ m_bIsNeedRS = bIsNeedRS; }

    BOOL32    GetRsParam(TRSParam& tRsParam)
	{ 
		tRsParam.m_wFirstTimeSpan  = m_tRsParam.m_wFirstTimeSpan;
		tRsParam.m_wSecondTimeSpan = m_tRsParam.m_wSecondTimeSpan;                                          
		tRsParam.m_wThirdTimeSpan  = m_tRsParam.m_wThirdTimeSpan;
		tRsParam.m_wRejectTimeSpan = m_tRsParam.m_wRejectTimeSpan;
	    return TRUE;
	}	
	void      SetRsParam(TRSParam &tRsParam)
	{ 
	    m_tRsParam.m_wFirstTimeSpan  = tRsParam.m_wFirstTimeSpan;
	    m_tRsParam.m_wSecondTimeSpan = tRsParam.m_wSecondTimeSpan;                                          
	    m_tRsParam.m_wThirdTimeSpan  = tRsParam.m_wThirdTimeSpan;
	    m_tRsParam.m_wRejectTimeSpan = tRsParam.m_wRejectTimeSpan;
	    return;
	}

	//创建KdvMediaRcv,KdvVidDec,KdvAudDec
    BOOL Create(u32    dwChnId,                  //通道ID    		 
		        u32    dwRtcpBackAddr,         //rtcp回发Ip地址，
				u16    wRtcpBackPort,          //rtcp回发端口
	 	        u16    wLocalStartPort,        //rtp端口，rtcp端口为rtp端口+1
				u32    dwContext);             //适应码流透传，传入实例指针

	//开始解码
	BOOL StartDecode(BOOL32 bStartMediaRcv = FALSE, u8 byOutPutMode = HDU_OUTPUTMODE_BOTH);

	//停止解码
    BOOL StopDecode( BOOL32 bStartMediaRcv = TRUE, u8 byOutPutMode = HDU_OUTPUTMODE_BOTH );

   	//开始视频解码
	BOOL StartVidDec(BOOL32 bStartMediaRcv = FALSE);
	
	//停止视频解码
	BOOL StopVidDec(BOOL32 bStopMediaRcv = TRUE, BOOL32 bStopDec = FALSE);
 
	//开始音频解码
	BOOL StartAudDec(BOOL32 bStartMediaRcv = FALSE);
	
	//停止音频解码
	BOOL StopAudDec(BOOL32 bStopMediaRcv = TRUE, BOOL32 bStopDec = FALSE);

	//设置图像的网络接收重传参数
	BOOL SetNetRecvFeedbackVideoParam(TRSParam tNetRSParam, BOOL32 bRepeatSnd = FALSE);

	//设置 H.263+/H.264 等动态视频载荷的 Playload值
    BOOL SetVideoActivePT(u8 byRmtActivePT, u8 byRealPT );

	//设置音频动态载荷 playload值
	BOOL SetAudActivePT(u8 byRmtActivePT, u8 byRealPT );

	//设置 视频解码key字串 以及 解密模式 Aes or Des
    BOOL SetVidDecryptKey(s8 *pszKeyBuf, u16 wKeySize, u8 byDecryptMode);

	//设置音频解密字串及解密模式
	BOOL SetAudDecryptKey(s8 *pszKeyBuf, u16 wKeySize, u8 byDecryptMode);

	//设置音频的网络接收重传参数
	BOOL SetNetRecvFeedbackAudioParam(TRSParam tNetRSParam, BOOL32 bRepeatSnd = FALSE);

	//获取视频解码器状态
	BOOL GetVidDecoderStatus(TKdvDecStatus &tKdvDecStatus);

    //获取视频解码器的统计信息
	BOOL GetVidDecoderStatis(TKdvDecStatis &tKdvDecStatis);	

	//获取音频解码器状态
	BOOL GetAudDecoderStatus(TKdvDecStatus &tKdvDecStatus);

    //获取音频解码器的统计信息
	BOOL GetAudVidDecoderStatis(TKdvDecStatis &tKdvDecStatis);	
    
	//设置视频播放端口类型 VGA or DVI
	BOOL SetVidPlyPortType(u32 dwType);

	//设置视频输出信号制式，NULL表示自动调整
    BOOL SetVideoPlyInfo(TVidSrcInfo* ptInfo);
	
	//设置缩放比例
	BOOL SetPlayScales( u16 wWidth, u16 wHeigh );

    //设置音频播放端口类型 HDMI or C
    BOOL SetAudOutPort(u32 dwAudPort);

	//设置视频接收地址参数(进行底层套结子的创建，绑定端口等动作)
    BOOL SetVidNetRcvLocalParam ( TLocalNetParam *ptLocalNetParam );

	//设置音频接收地址参数(进行底层套结子的创建，绑定端口等动作)
    BOOL SetAudNetRcvLocalParam ( TLocalNetParam *ptLocalNetParam );
	
	//设置输出声音音量
	u16  SetAudioVolume(u8 byVolume ); 
	
	//得到输出声音音量
    u16  GetAudioVolume(u8 &pbyVolume );
	
	//设置静音
    u16  SetAudioMute(BOOL32 bMute);

	//显示通道信息,将两路编码都打印出来
	void ShowChnInfo(void);
}
#ifndef WIN32
__attribute__ ((packed)) 
#endif
;

class CHduInstance: public CInstance
{
	enum ESTATE
    {
        eIDLE,
		eREADY,
		eNORMAL,
    };
public:
	TChannelInfo         m_tChnInfo;                                    //会议号
	CDecoderGrp     	*m_pDecCroup;                                   //复合解码器指针
public:
	CHduInstance();
	virtual ~CHduInstance();
	void InstanceEntry( CMessage* const pMsg );                         
	void DaemonInstanceEntry( CMessage* const pMsg, CApp* pcApp );      

	/* --------- DaemonInstance消息处理函数 -------------*/
	void  DaemonProcPowerOn( CMessage* const pMsg);       //初始化
	void  DaemonProcHduStartPlay( CMessage* const pMsg );               //开始播放
    void  DaemonProcHduStopPlay( CMessage* const pMsg );                //停止播放
	void  DaemonProcOspDisconnect( CMessage* const pMsg);				//断链处理
	void  DaemonProcConnectTimeOut( BOOL32 bIsConnectA );               //向MCU建链
	void  DaemonProcRegisterTimeOut( BOOL32 bIsRegiterA );              //向MCU注册    
	void  DaemonProcMcuRegAck(CMessage* const pMsg);					//处理注册ACK回应
    void  DaemonProcMcuRegNack(CMessage* const pMsg);                   //处理注册NACK回应
	void  DaemonProcModePortChangeNotif( CMessage* const pMsg );        //制式改变通知
	void  DeamonProcGetMsStatusRsp( CMessage* const pMsg );             //取主备倒换状态
	void  DaemonProcSetQosCmd( CMessage* const pMsg );                  //设置Qos值
    void  DaemonProcHduChangeVolumeReq( CMessage* const pMsg );         //设置音量
	void  DaemonProcChangePlayPolicy( CMessage* const pMsg );           //通道空闲实现策略改变通知
	
	/* --------- Instance消息处理函数 -------------*/
	void  ProcInitHdu(void);                                            //初始化HDU
	void  ProcInitHduChnl( CMessage* const pMsg );                      //初始化hdu通道信息
	void  ProcStopHdu( void );                                          //停止HDU设备
	void  ProcStartPlayReq( CServMsg &cServMsg );  		                //开始播放
	void  ProcStopPlayReq( CServMsg &cServMsg );   		                //停止播放
	void  ProcTimerNeedIFrame( void );                                  //Hdu请求Mcu发关键帧
	void  ProcSetMode( CMessage* const pMsg );                          //设置输入输出制式
    void  ProcHduChangeVolumeReq( CServMsg &cServMsg );                 //设置音量或静音
	void  ProcChangePlayPolicy( CMessage* const pMsg );                 //通道空闲实现策略改变通知
	
	/* ---------------- 功能函数 ------------------*/
	BOOL32 ConnectMcu( BOOL32 bIsConnectA, u32& dwMcuNode );            //连MCU
	void   Register(u32 dwMcuNode );               //向MCU注册
	BOOL32 SendMsgToMcu( u16 wEvent, CServMsg& cServMsg );              //向MCU发送消息通用函数
	void   SendChnNotif( BOOL32 bFirstNotif = FALSE );                          //通道状态通知 
	void   ClearInstStatus( void );                                     //清空本实例保存的状态信息
	void   StatusShow( void );                                          //状态显示，显示复合解码器状态
	void   StatusNotify(void);                                          //HDU状态通知                                  
	void   ComplexQos( u8& byValue, u8 byPrior );                       //组合Qos值
    BOOL32 SetEncryptParam( TMediaEncrypt *ptMediaEncryptVideo, 
		                    TDoublePayload *ptDoublePayloadVideo );     // 设置视频编码参数
	BOOL32 SetAudEnctypt( TMediaEncrypt *ptMediaEncryptVideo, 
		                  TDoublePayload *ptDoublePayloadVideo );       // 设置音频编码参数
	BOOL Play( u8 byMode );						                        // 开始播放
	BOOL Stop( void );						                            // 停止通道播放
	// 根据HDU输出接口和输出制式得到视频源信息
	BOOL GetVidSrcInfoByTypeAndMode(u8 byOutPortType, u8 byOutModeType, TVidSrcInfo *ptVidSrcInfo);

}
#ifndef WIN32
__attribute__ ((packed)) 
#endif
;

#ifndef MIN_BITRATE_OUTPUT
#define MIN_BITRATE_OUTPUT      (u16)128
#endif

struct THduDebugVal
{
protected:
    BOOL32          m_bEnableBitrateCheat;                              // 是否启用码率输出作弊
    u16             m_wDefault;                                         // 缺省下调的百分比
	s32             m_nVidDecResizeMode;                                // 0：加黑边；1：裁边；2：全屏 (初始化调用，走配置文件)
																		
public:
    void   SetEnableBitrateCheat(BOOL32 bEnable)
    {
        m_bEnableBitrateCheat = bEnable;
    }
    BOOL32 IsEnableBitrateCheat() const
    {
        return m_bEnableBitrateCheat;
    }
	
    u16    GetDecRateDebug(u16 wOriginRate) const
    {    
		//没有则返回缺省值
        wOriginRate = wOriginRate * m_wDefault/100;

        if (wOriginRate > MIN_BITRATE_OUTPUT)
            return wOriginRate;
        else
            return MIN_BITRATE_OUTPUT;
    }
    void   SetDefaultRate(u16 wPctRate)
    {
        m_wDefault = wPctRate;
    }

	void   SetVidDecResizeMode(s32 nMode)
    {
        m_nVidDecResizeMode = htonl(nMode);
    }

	u32    GetVidDecResizeMode(void)
    {
        return ntohl( m_nVidDecResizeMode );
    }	
	
}
#ifndef WIN32
__attribute__ ((packed)) 
#endif
;

class CHduData
{
public:
	CHduData();
	virtual ~CHduData();
public:
	u8                  m_bMaxChnNum;                   //接收信道数

	u8                  m_byOutPutMode;                 //输出模式0:无输出模式，1:音频，2:视频， 3:音视频

//	BOOL32              m_bMute[MAXNUM_HDU_CHANNEL];    // 保存静音状态，以便轮询后恢复    
	u32              	m_dwMcuIId;           	        // 通信MCU.A表示
	u32              	m_dwMcuIIdB;           	        // 通信MCU.B表示
	u32              	m_dwMcuNode;          	        // 与MCU.A通信节点号
	u32             	m_dwMcuNodeB;         	        // 与MCU.B通信节点号
	
    BOOL32              m_bEmbedMcu;                    // 是否内嵌在MCU.A中
    BOOL32              m_bEmbedMcuB;                   // 是否内嵌在MCU.B中

	u32              	m_dwMcuRcvIp;          	        // MCU.接收地址
//	u16              	m_wMcuRcvStartPort;             // MCU.接收起始端口号
	
	u8               	m_byRegAckNum;        	        // 第一次注册成功
	u32              	m_dwMpcSSrc;           	        // 业务侧会话校验值 
	u16  		     	m_wMTUSize;                     // 最大传输单元
	THduCfg         	m_tCfg;                         // HDU配置
	TEqp             	m_tHdu;                      	// 本设备
	TPrsTimeSpan     	m_tPrsTimeSpan;                 // 重传时间跨度
	THduDebugVal    	m_tDebugVal;                    // 作弊预留
    THduStatus          m_tHduStatus;                   // HDU状态

public:
	void SetNull(void);
    void FreeStatusDataA(void);                         // 清除A状态参数
    void FreeStatusDataB(void);                         // 清除B状态参数

	void ReadDebugValues(void);                         // 从配置文件中读取码率作弊值
    BOOL32 IsEnableCheat(void) const;                   // 获取是否作弊
    u16 GetDecRateDebug(u16 wOriginRate) const;         // 获取码率作弊值
	s32 GetVidDecResizeMode( void )
    {
        return m_tDebugVal.GetVidDecResizeMode();
    }
}
#ifndef WIN32
__attribute__ ((packed)) 
#endif
;
void hdulog( s8* pszFmt, ...);
void hdulogall( s8* pszFmt, ...);
s8*  ipStr( u32 dwIP );	

typedef zTemplate< CHduInstance, MAXNUM_HDU_CHANNEL, CHduData > CHduApp;
extern CHduApp g_cHduApp;

#endif //end _TV_HDU_INST_H_


