/*****************************************************************************
   模块名      : Board Agent
   文件名      : dscagent.h
   相关文件    : 
   文件实现功能: 单板启动及相关普通函数定义
   作者        : 李 博
   版本        : V3.6  Copyright(C) 2005-2006 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   2005/04/25  3.6         李 博         创建
   2005/11/23  40          john          增加模块、灯告警告警处理函数
******************************************************************************/
#ifndef _DSCAGENT_H_
#define _DSCAGENT_H_

#include "osp.h"
#include "dscconfig.h"

class CBoardAgent:public CInstance
{
	enum 
	{ 
		STATE_IDLE,			//空闲
		STATE_INIT,			//初始化
		STATE_NORMAL,		//工作
	};

public:
	CBoardAgent();
	~CBoardAgent();

protected:
	//实例消息处理入口函数，必须override
	void InstanceEntry(CMessage* const pMsg);
	void InstanceExit();

	//OSP断链
	void ProcOspDisconnect( CMessage* const pcMsg );

	//以下是和MANAGER的消息处理
	//注册应答消息
	void ProcBoardRegAck( CMessage* const pcMsg );	
	//注册否定应答消息
	void ProcBoardRegNAck( CMessage* const pcMsg );
	//取配置信息消息定时器到期处理
	void ProcGetConfigTimeOut( CMessage* const pcMsg );
	//配置应答消息
	void ProcBoardGetConfigAck( CMessage* const pcMsg );
	//配置否定应答消息
	void ProcBoardGetConfigNAck( CMessage* const pcMsg );
    //新配置信息通知
    void ProcSetDscInfoReq( CMessage* const pcMsg );
	// DSC的GK info更新请求
	void ProcBoardGkInfoUpdateCmd( CMessage* const pcMsg );
	// DSC的LoginInfo更新
	void ProcBoardLoginInfoUpdateCmd( CMessage* const pcMsg );
    
	//告警同步请求
	void ProcAlarmSyncReq( CMessage* const pcMsg );
	//启动消息
	void ProcBoardPowerOn( CMessage* const pcMsg );
	//连接超时
	void ProcBoardConnectManagerTimeOut( BOOL32 bIsConnectA );
	//等待注册超时
	void ProcBoardRegisterTimeOut( BOOL32 bIsConnectA );
	
    void ProcBoardModuleStatus( CMessage* const pcMsg );
    void ProcBoardLedStatus( CMessage* const pcMsg );
    void ProcGetModuleInfoReq( CMessage* const pcMsg );
	// reboot the board
	void ProcRebootCmd(CMessage* const pcMsg);
	void ProcBoardSelfTestCmd(CMessage* const pcMsg);
	void ProcBitErrorTestCmd(CMessage* const pcMsg );
	void ProcTimeSyncCmd(CMessage* const pcMsg );
	void ProcUpdateSoftwareCmd(CMessage* const pcMsg );
	void ProcStartDscUpdateSoftwareReq( CMessage* const pcMsg );
	void ProcUpdateFileReq( CMessage* const pcMsg );
	void ProcUpdateSoftwareWaittimer( CMessage* const pcMsg );

protected:
	//取单板的要连接的MPC的配置信息
	BOOL GetBoardCfgInfo();
	//连接管理程序
	BOOL ConnectManager(u32& dwMcuNode, u32 dwMpcIp, u16 wMpcPort);
	//发送消息给管理程序
	BOOL PostMsgToManager( u16  wEvent, u8 * const pbyContent, u16  wLen );

	void FreeDataA(void);
    void FreeDataB(void);
    void RegisterToMcuAgent(u32 dwDstNode);
    void MsgRegAck(CMessage* const pcMsg );
    void MsgGetConfAck(CMessage* const pcMsg);
    void MsgDisconnectInfo(CMessage* const pcMsg);

	void   NackUpdateFileReq(void);

private:
	TDscUpdateRsp m_tCurUpdateReq;
	s8 m_achCurUpdateFileName[MAXNUM_FILE_UPDATE][MAXLEN_MCU_FILEPATH];
	u8 m_abyUpdateResult[MAXNUM_FILE_UPDATE];
	u8 m_byUpdateFileNum;
	u8 m_byWaitTimes;
	u8 m_byErrTimes;
	u8 m_byBrdIdx;
	u8 m_byMcsSsnIdx;
	FILE *m_hUpdateFile;
	u8 *m_pbyFileBuf;
	u32 m_dwBufUsedSize;

private:
	TBrdPosition	m_tBoardPosition;    //单板位置，包括层、槽、类型
	u32	m_dwDstNode;    //节点号
	u32	m_dwDstIId;		//目的实例号

    u32 m_dwDstNodeB;
    u32 m_dwDstIIdB;

private:
    u32 m_dwBoardIpAddr;

#ifndef WIN32
	TBrdCRILedStateDesc m_tLedState;
#endif
	// 测试用
	BOOL    m_bIsTest;
	u16 	m_wLastEvent;
	u8	m_abyLastEventPara[20];
};

typedef zTemplate< CBoardAgent, BOARD_AGENT_MAX_INSTANCE, CDscConfig, 0 > CBrdAgentApp;

extern CBrdAgentApp	g_cBrdAgentApp;	//单板代理应用实例

API BOOL InitDscAgent();

API u8 GetBoardSlot();

void DscAgentAPIEnableInLinux();

#endif  /* DSCAGENT_H */
