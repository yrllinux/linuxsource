/*****************************************************************************
   模块名      : 多画面显示
   文件名      : twinst.h
   相关文件    : 
   文件实现功能: 多画面显示实例头文件
   作者        : 李屹
   版本        : V0.9  Copyright(C) 2001-2002 KDC, All rights reserved.
 -----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   2003/11/20     			   zhangsh		MCC
   2006/03/21  4.0         张宝卿      代码优化
******************************************************************************/
#ifndef _TV_WALL_INST_H_
#define _TV_WALL_INST_H_

#include "osp.h"
#include "mcustruct.h"
#define  MEDIA_TYPE
#include "codeccommon.h"
#include "codeclib.h"
#include "eqpcfg.h"

#define MAXNUM_TVWALL_CHNNL			(u8)5				//最大通道数
#define TVWALL_MODEL_NEW			(u8)3				//电视墙单板模式
#define FIRST_REGACK				(u8)1				//第一次收到注册Ack
#define TV_CONNETC_TIMEOUT			(u16)(3 * 1000)	    //connect time
#define TV_REGISTER_TIMEOUT			(u16)(6 * 1000)	    //register time
#define TVWALL_PRI                  (u16)60

#define PCHECK( x )  \
    if( NULL == x ) \
    {\
	OspPrintf( TRUE, FALSE, "The pointer cannot be Null\n"  );\
	return;\
    }\

class CTWInst : public CInstance
{
protected:
    enum ESTATE
    {
        eIDLE,
        eNORMAL,
        eERROR
    };
    
    u8            m_byTvNum;	    // 电视墙画面数
    TTvWallStatus m_tTWStatus;	    // 电视墙状态
    TEqpCfg       m_tCfg;		    // 电视墙配置
    u8            m_byTvModel;	    // 电视墙输出模式
    CKdvDecoder   m_acDecoder[MAXNUM_TVWALL_CHNNL];
    TPrsTimeSpan  m_tPrsTimeSpan;  
public:
    CTWInst();
    ~CTWInst();
	
	//消息入口
	void InstanceEntry( CMessage * const pMsg );

    void ProcConnectTimeOut( BOOL32 bIsConnectA );				// 连接总入口
    void ProcRegisterTimeOut( BOOL32 bIsRegisterA );			// 注册总入口
	BOOL32  ConnectMcu( BOOL32 bIsConnectA, u32& dwMcuNode );	// 具体连接Mcu
	void    Register( BOOL32 bIsRegisterA, u32 dwMcuNode );		// 向MCU注册

	void MsgDisconnectProc( CMessage * const pMsg );			// 断链处理
    void MsgRegAckProc( CMessage * const pMsg );				// 注册成功
   	void MsgRegNackProc( CMessage * const pMsg );				// 注册失败
    void ProcStartPlayMsg( CMessage * const pcMsg );			// 开始播放
	void ProcStopPlayMsg( CMessage * const pcMsg );				// 停止播放
	void ProcGetMsStatusMsg( CMessage * const pcMsg );			// 取主备状态
    void SendNotify();											// 状态上报
	BOOL SendMsgToMcu( u16 wEvent, CServMsg const &cServMsg );

	void ProcReconnectBCmd( CMessage * const pcMsg );	//MCU 通知会话 重联MPCB 相应实例, zgc, 2007/04/30

	BOOL Init( CMessage * const pMsg );
	BOOL InitDecoder();								// 初始化解码器
	BOOL Play( s32 nChnnl );						// 指定通道开始播放
	BOOL Stop( s32 nChnnl );						// 停止指定通道
	void StopAllChannels( void );					// 停止所有通道

	void ProcSetQosInfo( CMessage * const pMsg );	// 设置Qos信息
    void ComplexQos( u8& byValue, u8 byPrior );

	// 设置视频编码参数
    void SetEncryptParam( u8 byChnIdx, TMediaEncrypt *ptMediaEncryptVideo, TDoublePayload *ptDoublePayloadVideo );
	
	// 设置音频编码参数
    BOOL32 SetAudEnctypt( u8 byChnIdx, TMediaEncrypt *ptMediaEncryptVideo, TDoublePayload *ptDoublePayloadVideo );
	void ConfigShow();
	void StatusShow();
};

class CTvWallCfg
{
public:
    CTvWallCfg();
    virtual ~CTvWallCfg();
public:
    u32           m_dwMcuIId ;	 // mcu.A实例ID
    u32			  m_dwMcuIIdB;	 // mcu.B实例ID
    u32           m_dwMcuNode;	 // 与MCU.A通信节点号
    u32			  m_dwMcuNodeB;	 // 与MCU.B通信节点号
    BOOL32        m_bEmbed;      // 内嵌于A板
    BOOL32		  m_bEmbedB;     // 内嵌于B板
    u8            m_byRegAckNum; // 第一次收到注册

	u32           m_dwMpcSSrc;      // guzh [6/12/2007] 业务侧会话校验值

public:
    void FreeStatusDataA();
    void FreeStatusDataB();
	u32 GetCfgWait2BlueTimeSpan();
};

typedef zTemplate< CTWInst, 1, CTvWallCfg > CTvWallApp;
extern CTvWallApp g_cTvWallApp;

#endif //_TV_WALL_INST_H_
