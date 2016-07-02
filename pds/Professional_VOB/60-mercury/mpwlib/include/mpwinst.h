/*****************************************************************************
   模块名      : MpwLib多画面复合电视墙
   文件名      : mpwinst.h
   相关文件    : 
   文件实现功能: 定义消息类
   作者        : john
   版本        : V4.0  Copyright( C) 2001-2005 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   2005/09/25  1.0         john        创建
   2003/03/21  4.0         张宝卿      代码优化
******************************************************************************/
#ifndef _VIDEO_MPWINST_H_
#define _VIDEO_MPWINST_H_

#include "evmcueqp.h"
#include "mpwcfg.h"
#include "eqpcfg.h"

#define MPW_ID              (u8)9 // Mpw ID

class CMpwInst : public CInstance
{
	enum //实例状态
	{
		IDLE   = 0,
		NORMAL = 1,
		RUNNING = 2
	};
public:
	CMpwInst(  );
	virtual ~CMpwInst(  );

private:
	BOOL32        m_bDbVid;         // 是否双视频	
    TNetAddress   m_tRtcpAddr;      // Rtcp 地址
	CKDVVMPParam  m_tParam;
    CConfId       m_cConfId;
    
	// 自身状态
	CHardMulPic   m_cHardMulPic;    // 画面复合器的封装
    BOOL32        m_bAddVmpChannel[MAXNUM_VMP_MEMBER];
    u32           m_dwLastFUPTick;  //最后收到关键帧请求的Tick数
    TNetAddress   m_tSndAddr[2];    // 合成后输出地址
    TCapSupportEx m_tCapSupportEx;

private:
	// 消息响应
	void InstanceEntry( CMessage *const pcMsg );                // 消息入口    
	void MsgRegAckProc( CMessage* const pcMsg );                // 确认
	void MsgRegNackProc( CMessage* const pcMsg );               // 拒绝
	void MsgStartVidMixProc( CMessage* const pcMsg );           // 开始复合
	void MsgStopVidMixProc( CMessage* const pcMsg );            // 停止复合
	void MsgChangeMixParamProc( CMessage* const pcMsg );        // 改变复合参数
	void MsgGetMixParam( CMessage* const pcMsg );               // 查询复合参数
	void MsgSetBitRate( CMessage* const pcMsg );
	void MsgGetMpwData( CMessage* const pcMsg );
    void MsgNeedIFrameProc( void );                             // 是否需要关键帧
    void MsgFastUpdatePic( void );                              // 定时编一个关键帧

private:
	// 功能处理
    void Init( CMessage* const pcMsg );                         // 硬件初始化
    void ProcConnectTimeOut( BOOL32 bIsConnectA );
    void ProcRegisterTimeOut( BOOL32 bIsRegisterA );
	void ProcDisconnect( CMessage *const pcMsg );               // 断口连接
	void ProcGetMsStatusRsp( CMessage* const pcMsg );           // 取主备倒换状态

	BOOL32 ConnectToMcu( BOOL32 bIsConnectA, u32& dwMcuNode );  // 连接MCU
	void   RegisterToMcu( BOOL32 bIsRegisterA, u32 dwMcuNode ); // 向MCU注册
	void   StopAndClearInstStatus( void );                      // 停止并清除实例状态
	BOOL32 StartHardwareMix( CKDVVMPParam& tParam );            // 开始硬件合成
	BOOL32 AdjustMpwStyle( CKDVVMPParam& tParam );              // 改变合成风格
	BOOL32 ChangeMpwChannel( CKDVVMPParam& tParam );            // 改变逻辑通道

	u8   ConvertVc2Hard(  u8 byOldstyle  );                     // 业务到底层的合成模式转换
	u8   GetFavirateStyle( u32 dwTotalmap, u8 &byChannel );     // 根据MAP数目来决定合成类型
    u8   GetMpwNumOfStyle( u8 byMpwStyle );                     // 由合成风格取画面数
    u8   GetActivePayload( u8 byRealPayloadType );              // 取动态载荷值

    void UpdataEncryptParam( CMessage * const pcMsg );          // 修改加密参数
    void SetEncryptParam( u8 byChnNo, TMediaEncrypt* ptVideoEncrypt, TDoublePayload* ptDoublePayload );
    void SetDecryptParam( u8 byChnNo, TMediaEncrypt* ptVideoEncrypt, TDoublePayload* ptDoublePayload );
    
    void MpwStatusShow( void );                                 // 显示所有的状态和统计信息
    void SendStatusMsg( u8 online, u8 state, u8 style );        // 报告状态
	void ProcSetQosInfo( CMessage* const pcMsg );               // 设置Qos信息
    void ComplexQos( u8& byValue, u8 byPrior );                                    
    void mpwlog( s8* pszFmt, ...);
	BOOL32 SendMsgToMcu( u16  wEvent, CServMsg* const pcServMsg ); // 发送消息给MCU
    
    u16 GetSendRtcpPort(u8 byChanNo);
};

class CMpwConfig
{
public:
    CMpwConfig();
    ~CMpwConfig();
public:
    u8            m_byChannelnum;   // 通道数 
    CMpwCfg       m_tCfg;           // 配置参数
    u32           m_dwMcuNode;      // MCU.A 的节点号
    u32           m_dwMcuNodeB;     // MCU.B 的节点号
    u32           m_dwMcuIId;       // 与实例通讯的MCU.A 的全局ID
    u32           m_dwMcuIIdB;      // 与实例通讯的MCU.B 的全局ID
    BOOL32        m_bEmbed;         // 内嵌在A板 
    BOOL32        m_bEmbedB;        // 内嵌在B板
    u8            m_byRegAckNum;    // Reg Ack num
	u32           m_dwMpcSSrc;      // guzh [6/12/2007] 业务侧会话校验值 
public:
    void FreeStatusDataA();
    void FreeStatusDataB();
};

typedef zTemplate< CMpwInst, 1, CMpwConfig > CMpwApp;
extern CMpwApp g_cMpwApp;

#endif 
