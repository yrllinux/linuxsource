/*****************************************************************************
模块名      : 高清解码单元
文件名      : hdu2inst.h
相关文件    : 
文件实现功能: hdu2实例头文件
作者        : 田志勇
版本        : 4.7  Copyright(C) 2011-2013 KDV, All rights reserved.
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
11/10/31    4.7         田志勇      创建
******************************************************************************/
#ifndef _HDU2_INST_H_
#define _HDU2_INST_H_
#include "osp.h"
#include "mcustruct.h"
#include "eqpcfg.h"
#include "mcuconst.h"
#include "kdvmedianet.h"
#include "./dm816x/codecwrapper_hd.h"
#include "./dm816x/codecwrapperdef_hd.h"
#include <fstream>
#include "hdu2agent.h"
#include "evmcueqp.h"
#include "mcuver.h"

#define MAX_VIDEO_FRAME_SIZE        (512 * 1024)      //最大视频帧大小
#define MAX_AUDIO_FRAME_SIZE        (8 * 1024)        //最大音频帧大小

#define HDU_CONNETC_TIMEOUT         (u16)3*1000       // 连接超时值3s
#define HDU_REGISTER_TIMEOUT        (u16)5*1000       // 注册超时值5s
#define CHECK_IFRAME_INTERVAL       (u16)100          // 检测关键帧时间间隔(ms)

// Hdu2Print等级定义
#define		HDU_LVL_ERROR			(u8)1			  // 程序运行错误(逻辑或业务出错),必须输出
#define		HDU_LVL_WARNING			(u8)2			  // 告警信息, 可能正确, 也可能错误
#define		HDU_LVL_KEYSTATUS		(u8)3			  // 程序运行到一个关键状态时的信息输出
#define		HDU_LVL_DETAIL			(u8)4			  // 普通信息
#define		HDU_LVL_FRAME			(u8)5			  // 关键帧检测信息输出
#define		HDU_LVL_GENERAL			(u8)6			  // 一般信息，主要输出HDU2接收到的消息及消息的发送，设最高级别

class CHdu2ChnMgrGrp;

// 接收回调结构  
class CRcvData
{
public:
    CHdu2ChnMgrGrp* m_pcHdu2ChnMgrGrp;  //回调的Hdu2
    u32            m_byHduSubChnId;	    //回调的子通道号 
	CRcvData()
	{
		m_pcHdu2ChnMgrGrp = NULL;
		m_byHduSubChnId = 0;
	}
}
#ifndef WIN32
__attribute__ ((packed)) 
#endif
;

class CHdu2ChnMgrGrp
{
private:
	CKdvMediaRcv     	m_acVidMediaRcv[HDU_MODEFOUR_MAX_SUBCHNNUM];        //一个视频接收
	CKdvMediaRcv     	m_cAudMediaRcv;        								//一个音频接收(HDUVMP暂时不支持音频，未扩展)
	CKdvVidDec    	    m_cVidDecoder;										//视频解码器
    CKdvAudDec          m_cAudDecoder;										//音频解码器
	u8					m_abyMode[HDU_MODEFOUR_MAX_SUBCHNNUM];				//记录当前带有子通道解码模式
	TMediaEncrypt       m_tMediaDec;										//解密参数
	TDoublePayload      m_atVidDp[HDU_MODEFOUR_MAX_SUBCHNNUM];              //视频动态载荷
	TDoublePayload      m_tAudDp;											//音频动态载荷
	u8                  m_tIdleChnBackGround;								//空闲通道显示策略(HDUVMP使用HDU通道策略，未扩展)
	TLocalNetParam      m_atVidLocalNetParam[HDU_MODEFOUR_MAX_SUBCHNNUM];    //视频网络相关端口信息
	TLocalNetParam      m_tAudLocalNetParam;    //音频网络相关端口信息
	BOOL32              m_bIsNeedRs;			//是否启用网络丢包重传功能(HDUVMP使用HDU通道策略，未扩展)
	BOOL32              m_bIsMute;              //是否静音(HDUVMP使用HDU通道策略，未扩展)
	u8                  m_byVolume;             //音量(HDUVMP使用HDU通道策略，未扩展)
	u8                  m_byAudChnNum;          //声道数量(HDUVMP使用HDU通道策略，未扩展)
//	u8					m_byOneChnlDecCount;								// 统计HDU通道解码器的使用次数
	u8					m_abyHduSubChnId[HDU_MODEFOUR_MAX_SUBCHNNUM];		// Hdu多画面子通道
	CRcvData			m_acRcvCB[HDU_MODEFOUR_MAX_SUBCHNNUM];				//接收回调
public:
	CHdu2ChnMgrGrp()
	{
		memset(m_abyMode, 0, sizeof(m_abyMode));
		m_tIdleChnBackGround = 0;
		m_bIsNeedRs = FALSE;
		m_byVolume = HDU_VOLUME_DEFAULT;
	}
	~CHdu2ChnMgrGrp()
	{
	}
	void      SetMode( u8 byMode, u8 bySubChnId = 0 ){ m_abyMode[bySubChnId] = byMode; }
	u8        GetMode(u8 bySubChnId = 0){ return m_abyMode[bySubChnId]; }
	void      SetVidDoublePayload(TDoublePayload tVidDp, u8 byHduSubChnId)
	{
		memcpy( &m_atVidDp[byHduSubChnId], &tVidDp, sizeof(TDoublePayload) );
	}
	TDoublePayload   GetVidDoublePayload( u8 byHduSubChnId )
	{
		return m_atVidDp[byHduSubChnId];
	}
	void      SetAudDoublePayload(TDoublePayload tAudDp)
	{
		memcpy( &m_tAudDp, &tAudDp, sizeof(TDoublePayload) );
	}
	TDoublePayload      GetAudDoublePayload(void)
	{
		return m_tAudDp;
	}
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
	void      SetAudChnNum( u8	byAudChnNum ){ m_byAudChnNum = byAudChnNum; }
	u8        GetAudChnNum(void){ return m_byAudChnNum; }
	//创建
    BOOL32 Create(u16 wChnId,BOOL32 bIsHdu2_s);

	//开始解码
	BOOL32 StartDecode(u8 byMode = MODE_BOTH);

	//停止解码
    BOOL32 StopDecode(u8 byMode = MODE_BOTH );
	
	//开始接收
	BOOL32 StartNetRecv( u8 byMode = MODE_BOTH, u8 byHduSubChnId = 0);
	
	//停止接收
	BOOL32 StopNetRecv(u8 byMode = MODE_BOTH, u8 byHduSubChnId = 0);

	//设置视频接收地址参数(进行底层套结子的创建，绑定端口等动作)
	BOOL32  SetVidLocalNetParam(TLocalNetParam *ptVidLocalNetParam = NULL, u8 byHduSubChnId = 0);

	//设置音频接收地址参数(进行底层套结子的创建，绑定端口等动作)
	BOOL32  SetAudLocalNetParam(TLocalNetParam *ptAudLocalNetParam = NULL);

	//设置网络接收重传参数
	BOOL32 SetNetRecvRsParam(u8 byMode, BOOL32 bRepeatSnd, u8 byHduSubChnId = 0);

	//设置动态载荷值
	BOOL32 SetActivePT(u8 byMode , BOOL32 bIsVidAutoAjust = TRUE, u8 byHduSubChnId = 0);
	
	//设置解密参数
	BOOL32 SetDecryptKey(u8 byMode = MODE_BOTH, u8 byHduSubChnId = 0);

	//设置视频播放端口类型 VGA or DVI
	BOOL32 SetVidPlyPortType(u32 dwType);
	
	//设置视频输出信号制式，NULL表示自动调整
    BOOL32 SetVideoPlyInfo(TVidSrcInfo* ptInfo);
	
	//设置缩放比例
	BOOL32 SetPlayScales( u16 wWidth, u16 wHeigh );

	//设置显示策略
	BOOL32  SetVidDecResizeMode(u16 nMode);

	//设置视频画中画
	BOOL32 SetVidPIPParam(const u8 byHduVmpMode);

	//为解码器设置数据
	BOOL32 SetData(u8 byMode ,TFrameHeader tFrameHdr, u8 byHduSubChnId = 0);

	//获取视频解码器状态
	BOOL32 GetDecoderStatus(u8 byMode ,TKdvDecStatus &tKdvDecStatus, u8 byHduSubChnId);

    //获取视频解码器的统计信息
	BOOL32 GetDecoderStatis(u8 byMode ,TKdvDecStatis &tKdvDecStatis, u8 byHduSubChnId);	

    //设置音频播放端口类型 HDMI or C
    BOOL32 SetAudOutPort(u32 dwAudPort);
	
	//设置输出声音音量
	BOOL32  SetVolume(u8 byVolume = HDU_VOLUME_DEFAULT); 
	
	//得到输出声音音量
    u8  GetVolume();
	
	//设置是否静音
    BOOL32  SetIsMute(BOOL32 bMute);

	//获取是否静音
    BOOL32  GetIsMute();

	//设置空闲通道背景显示策略
	BOOL32 SetNoStreamBak(u8 byShowMode = VMP_SHOW_BLACK_MODE);//默认显示黑屏

	// 根据HDU输出接口和输出制式得到视频源信息
	BOOL32 GetVidSrcInfoByTypeAndMode(u8 byOutPortType, u8 byOutModeType, TVidSrcInfo *ptVidSrcInfo,BOOL32 bIsHdu2);

	//显示通道信息,将两路编码都打印出来
	void ShowInfo(u8 byChnId);

	//设置视频输出接口类型
	BOOL32 SetVidPlayPolicy(u8 byShowMode);
	// [2013/03/11 chenbing] 
	void SetHduSubChnId( u8 byHduSubChnId )
	{
		m_abyHduSubChnId[byHduSubChnId] = byHduSubChnId;
	}
	
	u8 GetHduSubChnId( u8 byHduSubChnId )
	{
		return m_abyHduSubChnId[byHduSubChnId];
	}
}
#ifndef WIN32
__attribute__ ((packed)) 
#endif
;

class CHdu2Instance: public CInstance
{
private:
	CHdu2ChnMgrGrp     m_cHdu2ChnMgrGrp;                                 //复合解码器指针
	CConfId            m_cChnConfId;
	enum EHDU2CHNSTATE
	{
		emIDLE,
		emINIT,
		emREADY,
		emRUNNING,
	};
public:
	CHdu2Instance();
	virtual ~CHdu2Instance();
private:

	/* --------- 管理实例消息处理函数 -------------*/
	void  DaemonInstanceEntry( CMessage* const pMsg, CApp* pcApp );     //管理实例消息主入口
	void  DaemonProcPowerOn( CMessage* const pMsg);       //上电初始化处理
	void  DaemonProcConnectTimeOut( BOOL32 bIsConnectA );               //向MCU建链消息处理
	void  DaemonProcOspDisconnect( CMessage* const pMsg); //断链处理处理
	void  DaemonProcRegisterTimeOut( BOOL32 bIsRegiterA );              //向MCU注册消息处理    
	void  DaemonProcMcuRegAck(CMessage* const pMsg);      //处理注册ACK回应
    void  DaemonProcMcuRegNack(CMessage* const pMsg);                   //处理注册NACK回应
	void  DaemonProcHduStartPlay( CMessage* const pMsg );               //开始播放
    void  DaemonProcHduStopPlay( CMessage* const pMsg );                //停止播放
	// [2013/03/11 chenbing]  
	void  DaemonProcHduChgVmpMode( CMessage* const pMsg );				//HDU通道模式切换
	void  DaemonProcHduSetAudioPayLoad(CMessage* const pMsg);			//设置音频载荷
	void  DaemonProcModePortChangeNotif( CMessage* const pMsg );        //制式改变通知
	void  DeamonProcGetMsStatusRsp( CMessage* const pMsg );             //取主备倒换状态
    void  DaemonProcHduChangeVolumeReq( CMessage* const pMsg );         //设置音量
	void  DaemonProcChangeModeCmd( CMessage* const pMsg );              //改变模式
	void  DaemonProcChangePlayPolicy( CMessage* const pMsg );           //通道空闲实现策略改变通知

	/* --------- 普通实例消息处理函数 -------------*/
	void  InstanceEntry( CMessage* const pMsg );                        //普通实例消息主入口
	void  ProcHduChnGrpCreate(void);                                    //处理创建通道管理组消息
	void  ProcInitChnCfg(void);										    //初始化hdu通道配置
	void  ProcStartPlayReq( CServMsg &cServMsg );  						//开始播放
	void  ProcChangeHduVmpMode( CServMsg &cServMsg );					//HDU通道模式切换
	void  ProcHduSetAudioPayLoad(CServMsg &cServMsg );					//设置音频载荷
	void  ProcStopPlayReq( CServMsg &cServMsg );   		                //停止播放
	void  ProcTimerNeedIFrame(void);									//Hdu请求Mcu发关键帧
	void  ProcChangeChnCfg( CMessage* const pMsg );                     //修改通道配置
    void  ProcHduChangeVolumeReq( CServMsg &cServMsg );                 //设置音量或静音
	void  ProcChangeModeCmd( );											//变更模式
	void  ProcChangePlayPolicy( CMessage* const pMsg );                 //通道空闲实现策略改变通知
	void  ProcDisConnect(void);											//HDU断链处理
	void  ProcShowMode(void);											//查寻所有通道模式
	void  ProcClearMode(void);											//清除通道模式

	/* ---------------- 功能函数 ------------------*/
	void   SendMsgToMcu( u16 wEvent, CServMsg& cServMsg );              //向MCU发送消息通用函数
	void   SendChnNotify( BOOL32 bFirstNotif = FALSE, u8 byHduSubChnId = 0 );//通道状态通知 
	void   StatusShow( void );                                          //状态显示，显示复合解码器状态
	void   StatusNotify(void);                                          //HDU状态通知，管理实例中调用
	BOOL32 Hdu2ChnStartPlay( u8 byMode = MODE_BOTH, u8 byHduSubChnId = 0);			//开始通道播放
	BOOL32 Hdu2ChnStopPlay( u8 byMode = MODE_BOTH, u8 byHduSubChnId = 0);	//停止通道播放
	BOOL32 ChangeHduVmpMode(const u8 byHduChnId, const u8 byHduVmpMode);		//切换风格

	u8 GetOldVersionEqpTypeBySubType ( u8 byHduEqpSubType ); //从新定义的HDU2SUBtype中获取老版本（4.7.1及其以前版本）EQPTYPE表示
}
#ifndef WIN32
__attribute__ ((packed)) 
#endif
;


class CHdu2Data
{
public:
	CHdu2Data();
	virtual ~CHdu2Data();
public:
	void SetNull(void)
	{
		memset(this, 0, sizeof(CHdu2Data));
		// [2013/03/12 chenbing] 
		memset(m_abyHduChnMode, HDUCHN_MODE_ONE, sizeof(m_abyHduChnMode));
	}
    void FreeStatusDataA(void)
	{
		m_dwMcuNode = INVALID_NODE;
		m_dwMcuIId  = INVALID_INS;
	}
    void FreeStatusDataB(void)
	{
		m_dwMcuNodeB = INVALID_NODE;
		m_dwMcuIIdB  = INVALID_INS;
	}
	void SetMcuNode(u32 NodeId){	m_dwMcuNode = NodeId;	};
	u32  GetMcuNode(){	return m_dwMcuNode;	};
	void SetMcuNodeB(u32 NodeId){	m_dwMcuNodeB = NodeId;	};
	u32  GetMcuNodeB(){	return m_dwMcuNodeB;	};
	void SetMcuIId(u32 InstId){	m_dwMcuIId = InstId;	};
	u32  GetMcuIId(){	return m_dwMcuIId;	};
	void SetMcuIIdB(u32 InstId){	m_dwMcuIIdB = InstId;	};
	u32  GetMcuIIdB(){	return m_dwMcuIIdB;	};
	void SetMcuSSrc(u32 dwSSrc){	m_dwMcuSSrc = dwSSrc;	};
	u32  GetMcuSSrc(){	return m_dwMcuSSrc;	};
	void SetHduCfg(THduCfg tHduCfg){	m_tHduCfg = tHduCfg;	};
	THduCfg  GetHduCfg(){	return m_tHduCfg;	};
	void SetPrsTimeSpan(TPrsTimeSpan tPrsTimeSpan){	m_tPrsTimeSpan = tPrsTimeSpan;	};
	TPrsTimeSpan  GetPrsTimeSpan(){	return m_tPrsTimeSpan;	};

	// [2013/03/11 chenbing] 
	void SetHduChnMode(u8 byHduChnId, u8 byHduChnMode, BOOL32 bIsOtherChnVmpMode = FALSE)
	{
		if (bIsOtherChnVmpMode)
		{
			for (u8 byIndex=0; byIndex<MAXNUM_HDU_CHANNEL; byIndex++)
			{
				if ( byIndex != byHduChnId )
				{
					m_abyHduChnMode[byIndex] = byHduChnMode;
				}
			}
		}
		else
		{
			m_abyHduChnMode[byHduChnId] = byHduChnMode;
		}
	}
	
	u8 GetHduChnMode(u8 byHduChnId, BOOL32 bIsOtherChnVmpMode = FALSE)
	{
		if (bIsOtherChnVmpMode)
		{
			for (u8 byIndex=0; byIndex<MAXNUM_HDU_CHANNEL; byIndex++)
			{
				if (   byIndex != byHduChnId
					&& HDUCHN_MODE_FOUR == m_abyHduChnMode[byIndex]
					)
				{
					return m_abyHduChnMode[byIndex];
				}
				
			}
			return HDUCHN_MODE_ONE;
		}
		else
		{
			return m_abyHduChnMode[byHduChnId];
		}
	}

	
	// [2013/03/12 chenbing] 判断风格是否合法
	BOOL32 IsValidHduVmpMode(u8 byHduVmpMode)
	{
		if (byHduVmpMode == HDUCHN_MODE_ONE)
		{
			return TRUE;
		}
		else if (byHduVmpMode == HDUCHN_MODE_FOUR)
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}

private:
	u32              	m_dwMcuNode;          	        // 与MCU.A通信节点号
	u32             	m_dwMcuNodeB;         	        // 与MCU.B通信节点号
	u32              	m_dwMcuIId;           	        // 通信MCU.A表示
	u32              	m_dwMcuIIdB;           	        // 通信MCU.B表示
	u32              	m_dwMcuSSrc;           	        // 业务侧会话校验值
	THduCfg         	m_tHduCfg;                      // HDU2配置
	TPrsTimeSpan     	m_tPrsTimeSpan;                 // 重传时间跨度

	// [2013/03/11 chenbing]  
	u8					m_abyHduChnMode[MAXNUM_HDU_CHANNEL];	//【新增字段】记录HDU通道模式
}
#ifndef WIN32
__attribute__ ((packed)) 
#endif
;

void Hdu2Print( const u8 byLevel, const s8* pszFmt, ...);
void hdu2log( const u8 byLevel, const u16 wModule, const s8* pszFmt, ...);
u32 GetVidPlyPortTypeByChnAndType(u16 wChnIdx,u32 dwType);
BOOL32 CheckChnCfgParam(THduModePort &tChnCfg,BOOL32 bIsHdu2_l);
s8 *IpToStr( u32 dwIP );
typedef zTemplate< CHdu2Instance, MAXNUM_HDU_CHANNEL, CHdu2Data > CHdu2App;
extern CHdu2App g_cHdu2App;
#endif //end _HDU2_INST_H_


