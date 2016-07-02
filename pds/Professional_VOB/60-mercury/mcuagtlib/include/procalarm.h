/*****************************************************************************
   模块名      : Mcu Agent
   文件名      : ProcAlarm.h
   相关文件    : ProcAlarm.cpp
   文件实现功能: 告警表的维护
   作者        : liuhuiyun
   版本        : V0.9  Copyright(C) 2001-2005 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   2005/08/17  1.0         liuhuiyun       创建
******************************************************************************/
#ifndef  _AGENT_ALARMPROC_H
#define  _AGENT_ALARMPROC_H
#include "agentcommon.h"
#include "mcuagtstruct.h"
#include "agtcomm.h"

#ifdef WIN32
#pragma  once
#endif

class  CAlarmProc 
{
public:
    CAlarmProc();
    virtual ~CAlarmProc();

public:
    
    BOOL32 AddAlarm( u32 dwAlarmCode, u8 byObjType, u8 abyObject[], TMcupfmAlarmEntry * ptData );
    BOOL32 FindAlarm( u32 dwAlarmCode, u8 byObjType, u8 abyObject[], TMcupfmAlarmEntry * ptData );
    BOOL32 DeleteAlarm( u32 dwSerialNo );
    BOOL32 GetAlarmStamp(u16& wAlarmStamp);
    BOOL32 GetAlarmTableIndex(u16 wTableIndex, TMcupfmAlarmEntry& tAlarmEntry);
	void   ProcReadError( u8 byErrorId );   //处理错误信息
	BOOL32 GetErrorString(s8* pszErrStr, u16 wLen);
	void   ShowErrorString(void);
	void   ShowCurAlarm( void );		    //显示当前告警
    void   ShowSemHandle( void );           //显示信号量值
	void   InitAlarmTable(void);		    //初始化告警表
	void   Quit(void);
private:

	TMcupfmAlarmTable	m_atAlarmTable[MAXNUM_MCU_ALARM];	
	TMcuPerformance		m_tMcuPfm;
    u32                 m_dwNowAlarmNo;		// 当前告警号
	u16                 m_wErrorNum;		// 读配置错误数
	SEMHANDLE	        m_hMcuAlarmTable;   // 告警表操作信号量
	s8                  m_achCfgErr[MAX_ERROR_NUM][MAX_ERROR_STR_LEN];		// 错误字符串
};

#endif // _AGENT_ALARMPROC_H
