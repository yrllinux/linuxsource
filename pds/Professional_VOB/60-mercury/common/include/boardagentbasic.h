/*****************************************************************************
   模块名      : Board Agent Basic
   文件名      : boardagentbasic.h
   相关文件    : 
   文件实现功能: 单板启动及相关普通函数定义
   作者        : 周广程
   版本        : V1.0  Copyright(C) 2001-2007 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   2007/08/20  4.0         周广程       创建
******************************************************************************/
#ifndef BOARDAGENTBASIC_H
#define BOARDAGENTBASIC_H

#include "osp.h"
#include "agentcommon.h"
#include "boardconfigbasic.h"

#define ALARM_MCU_OBJECT_LENGTH 2
#define ALARM_TABLE_OBJECT_LENGTH 5

// 告警结构
typedef struct
{
	u32	dwBoardAlarmSerialNo;			//告警序列号
    u32   dwBoardAlarmCode;				//告警码
    u8   byBoardAlarmObjType;			//告警对象类型
    u8	byBoardAlarmObject[ALARM_TABLE_OBJECT_LENGTH];		    //告警对象
	TBoardAlarmMsgInfo tBoardAlarm;         //状态消息和结构
    BOOL  	bExist;						    //该告警是否有效
}TBoardAlarm
;

class CBBoardAgent:public CInstance
{
protected:
	enum 
	{ 
		STATE_IDLE,			//空闲
		STATE_INIT,			//初始化
		STATE_NORMAL,		//工作
	};

public:
	CBBoardAgent();
	virtual ~CBBoardAgent();

//protected:
	//实例消息处理入口函数，必须override
	void InstanceEntry(CMessage* const pMsg);
	void InstanceExit();
	//启动消息
	void ProcBoardPowerOn( CMessage* const pcMsg );
	//OSP断链
	void ProcOspDisconnect( CMessage* const pcMsg );
	//单板复位消息处理
	void ProcBoardResetCmd( CMessage* const pcMsg );
	//以下是和MANAGER的消息处理
	//连接超时
//	void ProcBoardConnectManagerTimeOut( BOOL32 IsConnectA );
	void ProcBoardConnectManagerTimeOut( void );			// [6/1/2010 xliang] 
	BOOL32 ProcConnect();									// [6/1/2010 xliang] 
	//等待注册超时
	void ProcBoardRegisterTimeOut( BOOL32 bIsConnectA );
	//注册应答消息
	void ProcBoardRegAck( CMessage* const pcMsg );	
	//注册否定应答消息
	void ProcBoardRegNAck( CMessage* const pcMsg );
	//取配置信息消息定时器到期处理
	void ProcGetConfigTimeOut( CMessage* const pcMsg );
	//配置应答消息
	virtual void ProcBoardGetConfigAck( CMessage* const pcMsg ) = 0;
	//配置否定应答消息
	virtual void ProcBoardGetConfigNAck( CMessage* const pcMsg ) = 0;
	//告警同步请求
	void ProcAlarmSyncReq( CMessage* const pcMsg );
	//取单板软件版本消息处理
	void ProcGetVersionReq( CMessage* const pcMsg );
	//取单板的模块信息消息处理
	void ProcGetModuleInfoReq( CMessage* const pcMsg );
	//单板时间同步消息处理
	void ProcTimeSyncCmd( CMessage* const pcMsg );
	//单板自测消息处理
	void ProcBoardSelfTestCmd( CMessage* const pcMsg );
	//单板误码测试消息处理
	void ProcBitErrorTestCmd( CMessage* const pcMsg );
	//软件版本更新消息
	void ProcUpdateSoftwareCmd( CMessage* const pcMsg );
	//取单板的统计信息消息处理
	void ProcGetStatisticsReq( CMessage* const pcMsg );
	//以下是状态消息处理
	//内存状态
	void ProcBoardMemeryStatus( CMessage* const pcMsg );
	//文件系统状态
	void ProcBoardFileSystemStatus( CMessage* const pcMsg );
	//模块信息处理
	void ProcBoardModuleStatus( CMessage* const pcMsg );

	//测试用消息
	void ProcBoardGetCfgTest( CMessage* const pcMsg );
	void ProcBoardGetAlarm( CMessage* const pcMsg );
	void ProcBoardGetLastManagerCmd( CMessage* const pcMsg );
	void TestingProcess( CMessage* const pcMsg );

//protected:
	//连接管理程序
//	BOOL32 ConnectManager(u32& dwMcuNode, u32 dwMpcIp, u16 wMpcPort);
	BOOL32 ConnectManager( BOOL32 bIsNodeA );
	//判断是否需要连接的MPC A/B
	BOOL32 IsNeedConnectNode( BOOL32 bIsNodeA );

	//发送消息给管理程序
	BOOL32 PostMsgToManager( u16  wEvent, u8 * const pbyContent = NULL, u16  wLen = 0 );
	//告警列表中增加相应的告警信息
    BOOL32 AddAlarm( u32 dwAlarmCode, u8 byObjType, u8 byObject[], TBoardAlarm *ptData );
	//告警列表中删除相应的告警信息
    BOOL32 DeleteAlarm( u32 dwSerialNo );	
	//告警列表中获取相应的告警信息
    BOOL32 FindAlarm( u32 dwAlarmCode, u8 byObjType, u8 byObject[], TBoardAlarm *ptData );
	//设置引起该告警的消息信息
	BOOL32 SetAlarmMsgInfo( u32 dwSerialNo, TBoardAlarmMsgInfo* const ptMsg );
		
    void FreeDataA(void);
    void FreeDataB(void);
    void RegisterToMcuAgent(u32 dwDstNode);
    void MsgRegAck(CMessage* const pcMsg );
	void brdagtlog(char * fmt, ...);

	void UpdateIpPort(u32 dwIp, u16 wPort, BOOL32 bUpdateA);

	//单板温度异常（正常）状态通知
	void ProcBoardTempStatusNotif(CMessage* const pcMsg);
	//单板CPU占用率状态通知
	void ProcBoardCpuStatusNotif(CMessage* const pcMsg);

private:
	CBBoardConfig *m_pBBoardConfig;

	TBoardAlarm  m_atBoardAlarmTable[ MAXNUM_BOARD_ALARM ];  /*告警表*/
	u32		 m_dwBoardAlarmTableMaxNo;  /* 告警表中告警的最大编号*/
public:
	u8       m_byIsMpu2SimuMpu;			//是否MPU2模拟MPU
	u8		 m_byIsAnyBrdRegSuccess;	//值为0表示未注册成功，1表示双链或单链有一个注册成功
	u8       m_byRegBrdTypeFlagA;		//注册到MPCA板，值为0以BRDID A注册，值为1以BRDID B注册，兼容新老MCU的BRD TYPE定义不同
	//目前支持MPU2模拟MPU
	u8       m_byRegBrdTypeFlagB;		//注册到MPCB板，值为0以BRDID A注册，值为1以BRDID B注册，兼容新老MCU的BRD TYPE定义不同
										//目前支持MPU2模拟MPU

};

void AddBrdAgtEventStr();            // 添加单板代理消息字符串 [liaokang 2012/05/05]

#endif  /* BOARDAGENTBASIC_H */
