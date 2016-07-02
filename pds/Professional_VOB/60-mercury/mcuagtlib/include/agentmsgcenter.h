/*****************************************************************************
   模块名      : Mcu Agent
   文件名      : Agentmsgcenter.h
   相关文件    : 
   文件实现功能: 消息中转
   作者        : 
   版本        : V4.0  Copyright( C) 2006-2008 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人        修改内容
   2005/08/17  4.0         liuhuiyun     创建
******************************************************************************/
#ifndef _MCUAGENT_MSGDEAL_H
#define _MCUAGENT_MSGDEAL_H

#include "evagtsvc.h"
#include "osp.h"
#include "kdvsys.h"
#include "mcuconst.h"
#include "mcuagtstruct.h"
#include "agtcomm.h"
#include "agentsnmp.h"
#include "snmpadp.h"
#include "procalarm.h"

#include "ftpc_api.h"

#ifdef WIN32
#pragma  once
#endif

// 性能统计定时上报时长 设计规格最后定为600s(10分钟) [miaoqingsong 20111219 modify]
#define UPDATE_PFMINFO_TIME_OUT			600000

// 定时扫描调用回调定时器时长 mqs 20101118 add
#define OPENORCLOSE_MPCLED_TIME_OUT     35000

class CMsgCenterInst : public CInstance
{
	/*lint -save -esym(1551, CMsgCenterInst::~CMsgCenterInst)*/
	/*lint -save -sem(CMsgCenterInst::Quit,cleanup)*/

public:
    CMsgCenterInst();
    virtual ~CMsgCenterInst();
	void InstanceEntry(CMessage* const pcMsg);

protected: 
    BOOL32 Initialize(void);
    BOOL32 Quit(void);
    BOOL32 SetBoardLedState( u8 byLayer, u8 bySlot, u8 byType, s8 * pchLedState );

protected:
	void ProcSDHStatusChange( CMessage * const pcMsg );         // 处理SDH状态
	void ProcBoardStatusChange( CMessage * const pcMsg );       // 处理单板状态
	void ProcLinkStatusChange( CMessage * const pcMsg );        // 处理链路状态
	void ProcNetSyncStatusChange( CMessage * const pcMsg );     // 处理网同步状态
	void ProcSoftwareStatusChange( CMessage * const pcMsg );    // 处理更新软件
	void ProcPowerOnOffStatusChange( CMessage * const pcMsg );  // 处理待机
	void ProcPowerStatusChange( CMessage * const pcMsg );       // 处理上电
	void ProcModuleStatusChange( CMessage * const pcMsg );      // 处理模块状态
	void ProcBoardQueryAlarm( CMessage * const pcMsg );         // 处理单板查询告警
	void ProcPowerFanStatusChange( CMessage * const pcMsg );    // 处理电源风扇状态
	void ProcBoxFanStatusChange( CMessage * const pcMsg );      // 处理机箱风扇状态, mqs, 2010/12/14
	void ProcMPCCpuStatusChange( CMessage * const pcMsg );      // 处理MPC板Cpu状态, mqs, 2010/12/15
	void ProcMPCMemoryStatusChange( CMessage * const pcMsg);    // 处理MPC板Memory状态, mqs, 2010/12/15
	void ProcPowerTempStatusChange( CMessage * const pcMsg);    // 处理电源板温度异常状态，mqs, 2010/12/16
	void ProcMpc2TempStatusChange( CMessage * const pcMsg);     // 处理MPC2板温度异常状态，mqs,2011/01/13
	void ProcBrdFanStatusChange( CMessage * const pcMsg );      // 处理单板风扇
	void ProcConfStatusChange( CMessage * const pcMsg );        // 处理会议状态, mqs, 2010/12/22, 上级MCU触发
    void ProcLedStatusChange( CMessage * const pcMsg );         // 处理Led状态（MPC板）
    /*void ProcLedAlarm( CMessage * const pcMsg );*/                
    void ProcBoardStatusAlarm( CMessage * const pcMsg );        // 处理外设单板告警（Led/网口状态） [2012/05/04 liaokang]
    void ProcMcuLedAlarm( CMessage * const pcMsg );             // 处理备份mcu的Led告警
	void ProcCpuFanStatusChange( CMessage * const pcMsg );		// 处理cpu风扇告警
	void ProcCpuTempStatusChange( CMessage * const pcMsg );		// 处理cpu温度告警

    void ProcMcsUpdateBrdCmd( CMessage * const pcMsg );         // mcu通知代理更新单板版本
#ifdef _MINIMCU_
	void ProcMcsUpdateDscMsg( CMessage * const pcMsg );			// mcu更新DSC版本统一处理函数, zgc, 20070820
	void ProcSetDscInfoReq( CMessage * const pcMsg );			// 设置DSC info, zgc, 2007-07-17
	void ProcSetDscInfoRsp( CMessage * const pcMsg );			// 设置DSC info的回应处理, zgc, 2007-07-17
	void ProcDscRegSucceedNotif( CMessage * const pcMsg );			// 通知MCU DSC板已经注册成功, zgc, 2007-07-31
#endif	
	
    void ProcNMSUpdateBrdCmd( CMessage * const pcMsg );         // 处理网管更新MPC版本
	void ProcSmUpdateBrdCmd( void );          // 处理sm更新MPC版本
    void ProcGetConfigError();                                  // 处理单板取配置错误
    void ProcPowerOffAlarm();                                   // 关机告警
    void ProcColdStartdAlarm();                                 // 处理冷启动
    void ProcSnmpInfo();                                        // 取Snmp信息
	void ProcMsgToBrdManager( CMessage * const pcMsg );				// 转发MCU发往单板代理的消息, zgc, 2007-08-28

    u8   GetBoardLedStatus(u8 byBrdType, TEqpBrdCfgEntry& tBrdCfg, u8 * pszLedStr );         // 取相应单板的灯状态
    u16  SendPrmAlarmTrap( CNodes * pcNodes, TMcupfmAlarmEntry tAlarmEntry, u8 byTrapType ); // 发对应的告警trap
	void ProcUpdateMcuPfmInfoTimer( void );
	void ProcOpenorcloseMpcLedTimer( void );    // 处理MPC板NMS灯状态，mqs 20101118 add
	// 单板温度状态改变，目前支持DRI2板 [10/25/2011 chendaiwei]
	void ProcBoardTempStatusChange( CMessage * const pcMsg );
	//  [10/26/2011 chendaiwei] 单板CPU状态改变，目前支持DRI，DRI2板
	void ProcBoardCpuStatusChange(CMessage * const pcMsg);
	/*void ProcBoardSoftwareVersionNotif(CMessage * const pcMsg );*/

private:
    CNodes*			m_pcSnmpNodes;
    CAgentSnmp*		m_pcAgentSnmp;
   
};

//处理MCU系统信息的SNMP回调
u16 ProcCallBackMcuSystem(u32 dwNodeName, BOOL32 bFlag, void *pBuf, u16* pwBufLen);
u16 ProcCallBackMcuLocal (u32 dwNodeName, BOOL32 bFlag, void *pBuf, u16* pwBufLen);

//处理MCU网络信息的SNMP回调
u16 ProcCallBackMcuNet(u32 dwNodeName, BOOL32 bFlag, void *pBuf, u16* pwBufLen);
u16 ProcCallBackMcuNetQos(u32 dwNodeName, BOOL32 bFlag, void *pBuf, u16* pwBufLen);
u16 ProcCallBackMcuNetSnmpCfg(u32 dwNodeName, BOOL32 bFlag, void *pBuf, u16* pwBufLen);
u16 ProcCallBackMcuNetBoardCfg(u32 dwNodeName, BOOL32 bFlag, void *pBuf, u16* pwBufLen);

//处理MCU外设信息的SNMP回调
u16 ProcCallBackMcuEqpBas(u32 dwNodeName, BOOL32 bFlag, void *pBuf, u16* pwBufLen);
u16 ProcCallBackMcuEqpVMP(u32 dwNodeName, BOOL32 bFlag, void *pBuf, u16* pwBufLen);
u16 ProcCallBackMcuEqpPrs(u32 dwNodeName, BOOL32 bFlag, void *pBuf, u16* pwBufLen);
u16 ProcCallBackMcuEqpDcs(u32 dwNodeName, BOOL32 bFlag, void *pBuf, u16* pwBufLen);
u16 ProcCallBackMcuEqpMixer(u32 dwNodeName, BOOL32 bFlag, void *pBuf, u16* pwBufLen);
u16 ProcCallBackMcuEqpTVWall(u32 dwNodeName, BOOL32 bFlag, void *pBuf, u16* pwBufLen);
u16 ProcCallBackMcuEqpNetSync(u32 dwNodeName, BOOL32 bFlag, void *pBuf, u16* pwBufLen);
u16 ProcCallBackMcuEqpRecorder(u32 dwNodeName, BOOL32 bFlag, void *pBuf, u16* pwBufLen);
u16 ProcCallBackMcuPfmInfo(u32 dwNodeName, BOOL32 bFlag, void *pBuf, u16* pwBufLen);

//处理MCU告警和通知SNMP回调
u16 ProcCallBackMcuPfmAlarm(u32 dwNodeName, BOOL32 bFlag, void *pBuf, u16* pwBufLen);
u16 ProcCallBackMcuNotifications(u32 dwNodeName, BOOL32 bFlag, void *pBuf, u16* pwBufLen);

u16 McuMgtGetUpdateFiles(void *pBuf, u16 *pwBufLen);
u16 McuMgtSetUpdateFiles(void *pBuf, u16 *pwBufLen);

u16 McuSmGetUpdateFiles(void *pBuf, u16 *pwBufLen);
u16 McuSmSetUpdateFiles( void *pBuf, u16 *pwBufLen );

BOOL32 ChangeNumBufToStrBuf(u8 * pNumBuf, u8 byBufLen, u8 * pStrBuf);

void ProcCbFtpcCallbackFun(TFtpcCallback *PTParam, void *pCbd);

//[5/16/2013 liaokang] 放于此,等待后续snmpadp支持上去
// Snmp是否为UTF8编码
void   SetSnmpIsUtf8(BOOL32 bIsUtf8);
BOOL32 GetSnmpIsUtf8();
//end
BOOL32 TransEncodingForNmsData(const s8 *pSrc, s8 *pDst, u32 dwLen, BOOL32 bSet = FALSE);

typedef zTemplate<CMsgCenterInst, 1 > CMsgCenterApp;
extern CMsgCenterApp g_cMsgProcApp;

#endif // _MCUAGENT_MSGDEAL_H

