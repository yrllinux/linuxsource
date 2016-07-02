  /*****************************************************************************
   模块名      : KDVNMS
   文件名      : NmsSvrDef.h
   相关文件    : NmsSvrDef.cpp
   文件实现功能: 公共数据结构的定义
   作者		   : 薛新文
   版本 	   : V0.9  Copyright(C) 2001-2002 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本 	修改人  	修改内容
   2002/03/05  0.9  	薛新文  	创建
******************************************************************************/
#ifndef _NMS_SVR_COMMON_H
#define _NMS_SVR_COMMON_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "nmsmacro.h"
#include <list>
#include <afxmt.h>

using namespace std;

#define OPER_UNIMPLEMENTED				FALSE

/***************************************************************************/

//各个模块的定时器系数
#define SESS_MODULAR		1
#define CM_MODULAR			0.85
#define FM_MODULAR			0.85

//前后台配置是否一致
#define	CONFIG_SAME			0
#define	CONFIG_DIFFERENT	1

//前后台告警是否一致
#define	ALARM_SAME			0
#define	ALARM_DIFFERENT		1

//服务器内部消息
#define EV_SVR_START					EV_SVR_BGN + 0	//系统启动消息
#define	EV_SVR_CM_NORMAL				EV_SVR_BGN + 1	//CM模块正常消息
#define	EV_SVR_FM_NORMAL				EV_SVR_BGN + 2	//FM模块正常消息
#define EV_SVR_GETVERSION_SUC			EV_SVR_BGN + 3	//获取版本信息成功消息
#define EV_SVR_GETVERSION_FAIL			EV_SVR_BGN + 4	//获取版本信息失败消息
#define EV_SVR_CFG_CHANGE				EV_SVR_BGN + 7	//配置改变消息
#define EV_SVR_LINK_CHANGE				EV_SVR_BGN + 8	//链路改变消息
#define EV_AGT_RESTART					EV_SVR_BGN + 9	//Agt重新启动消息
#define EV_SVR_NEED_FMSYNC				EV_SVR_BGN + 10	//设备需要同步告警消息
#define EV_SVR_GETALARMSTAMP_SUC		EV_SVR_BGN + 11	//获取告警信息成功消息
#define EV_SVR_GETALARMSTAMP_FAIL		EV_SVR_BGN + 12	//获取告警信息失败消息
#define EV_SVR_GETALARM_SUC				EV_SVR_BGN + 13	//获取告警成功消息
#define EV_SVR_GETALARM_FAIL			EV_SVR_BGN + 14	//获取告警失败消息
#define EV_SVR_ALARM					EV_SVR_BGN + 15	//告警上报消息
#define EV_SVR_SHUTDOWN					EV_SVR_BGN + 16	//告警上报消息
#define EV_SVR_FTP_PROC					EV_SVR_BGN + 21 //Ftp进展
#define EV_SVR_FTP_THREAD_EXIT			EV_SVR_BGN + 22 //Ftp线程退出
#define EV_SVR_FTP_PUT_SUC				EV_SVR_BGN + 23 //FTP Put 成功
#define EV_SVR_FIND_DEVICE				EV_SVR_BGN + 24 //发现新设备
#define EV_SVR_GETDEVICEINFO_SUC		EV_SVR_BGN + 25 //获取设备信息成功
#define EV_SVR_GETDEVICEINFO_FAIL		EV_SVR_BGN + 26 //获取设备信息失败
#define EV_SVR_ADD_BOARD				EV_SVR_BGN + 27 //添加板卡
#define EV_SVR_BOARD_STATUS				EV_SVR_BGN + 29 //板卡状态改变
#define EV_SVR_QUERY_ALL_BOARD_SUC		EV_SVR_BGN + 31 //查询板卡成功
#define EV_SVR_QUERY_ALL_BOARD_FAILED	EV_SVR_BGN + 32 //查询板卡失败
#define EV_SVR_MCU_CONFSTATE            EV_SVR_BGN + 33 //Mcu会议状态
#define EV_SVR_MT_CONFSTATE             EV_SVR_BGN + 34 //Mt会议状态
#define EV_SVR_FILE_UPDATE              EV_SVR_BGN + 35 //文件升级

//系统中的定时器定义
//For CM
#define CM_TIMER_FOR_TIMER				EV_SVR_BGN + 80 //开始设置配置轮询定时
#define CM_TIMER_FOR_POLL				EV_SVR_BGN + 81 //定时配置轮询
#define CM_TIMER_FOR_VER				EV_SVR_BGN + 82 //配置轮询超时
//#define CM_TIMER_FOR_QUERY_BOARD		EV_SVR_BGN + 83 //开始查询板卡定时
#define CM_TIMER_FOR_OPER				EV_SVR_BGN + 84 //操作超时
#define CM_TIMER_FOR_DEL_DEVICE			EV_SVR_BGN + 85
#define CM_TIMER_FOR_NEXTPOLL           EV_SVR_BGN + 87

//系统定时器定时到发出的消息定义
//For CM
#define EV_CM_TIMER_FOR_TIMER			EV_SVR_BGN + 80 //开始设置配置轮询定时事件
#define EV_CM_TIMER_FOR_POLL			EV_SVR_BGN + 81 //定时配置轮询事件
#define EV_CM_TIMER_FOR_VER				EV_SVR_BGN + 82 //配置轮询超时事件
//#define EV_CM_TIMER_FOR_QUERY_BOARD		EV_SVR_BGN + 83 //开始查询板卡定时事件
#define EV_CM_TIMER_FOR_OPER			EV_SVR_BGN + 84 //操作超时
#define EV_CM_TIMER_FOR_DEL_DEVICE		EV_SVR_BGN + 85 //操作超时
#define EV_CM_TIMER_QUERY_UPDATE        EV_SVR_BGN + 86 //查询文件升级状态
#define EV_CM_TIMER_FOR_NEXTPOLL        EV_SVR_BGN + 87



//系统中的定时器定义
//For FM
#define FM_TIMER_FOR_POLL				EV_SVR_BGN + 100 //告警轮询定时
#define FM_TIMER_FOR_ALARMSTAMP			EV_SVR_BGN + 101 //获取告警时间戳定时
#define FM_TIMER_FOR_ALARM				EV_SVR_BGN + 102 //获取告警定时
#define FM_TIMER_FOR_TIMER				EV_SVR_BGN + 103 //进行设定定时告警论询定时
#define FM_TIMER_FOR_DEVICEINFO			EV_SVR_BGN + 104 //获取设备信息定时
#define FM_TIMER_FOR_NEXTPOLL           EV_SVR_BGN + 105 //获取下一次轮询消息定时

//系统定时器定时到发出的消息定义
//For FM
#define EV_FM_TIMER_FOR_POLL			EV_SVR_BGN + 100 //告警轮询定时事件
#define EV_FM_TIMER_FOR_ALARMSTAMP		EV_SVR_BGN + 101 //获取告警时间戳定时事件
#define EV_FM_TIMER_FOR_ALARM			EV_SVR_BGN + 102 //获取告警事件定时
#define EV_FM_TIMER_FOR_TIMER			EV_SVR_BGN + 103 //进行设定定时告警论询事件
#define EV_FM_TIMER_FOR_DEVICEINFO		EV_SVR_BGN + 104 //获取设备信息定时事件
#define EV_FM_TIMER_FOR_NEXTPOLL        EV_SVR_BGN + 105 //下一次轮询开始 事件

//操作ID
#define OPER_GETVERSION_ID			1		//获取设备配置文件版本
#define OPER_GETBOARD_ID			2		//查询板卡
#define OPER_SYNCFILE_ID			3		//向前台发送的Set配置文件名SNMP操作
#define OPER_SYNCTIME_ID			4		//向前台发送的Set时间同步SNMP操作
#define OPER_DEVICERESTART_ID		5		//向前台发送重启动命令
#define OPER_SETPERISTATUS_ID		6		//设置设备状态
#define	OPER_SETPERISYNCTIME_ID		7		//同步设备时间
#define OPER_SETPERIBITERRTEST_ID	8		//设置设备误码测试
#define OPER_SETPERISELFTEST_ID		9		//设置设备自检
#define OPER_PERIE1LINKCFG_ID		10		//同步设备E1链路状态
#define OPER_GETALARMSTAMP_ID		11		//Snmp Get告警时间戳
#define OPER_GETALARM_ID			12		//Snmp Get告警
#define OPER_GETDEVICEINFO_ID		13		//获取设备信息
#define OPER_GETSINGLEBOARD_ID		14		//查询单块板卡


//系统中的定时器定义
//For Session mamage
#define SM_TIMER_FOR_OPER				EV_SVR_BGN + 120

//系统定时器定时到发出的消息定义
//For Session mamage
#define EV_SM_TIMER_FOR_OPER			EV_SVR_BGN + 120

const BYTE SVR_STATUS_IDL = 0x0;
const BYTE SVR_STATUS_CCMAPP_NORMAL = 0x1;
const BYTE SVR_STATUS_CFMAPP_NORMAL = 0x2;
const BYTE SVR_STATUS_CSMAPP_NORMAL = 0x4;
const BYTE SVR_STATUS_SERVICE = 0x07;

//服务器的端口号
#define	SERVER_PORT			2000

//别名长度
#define ALIAS_MAX_LEN		(s32)32

//函数名最长长度
#define MAX_PROC_NAME_LEN		(u8)32
//实例名最长长度
#define MAX_INS_NAME_LEN		MAX_PROC_NAME_LEN

//版本信息字符串的长度
#define	VERINFO_LEN			(s32)20

//文件名的长度
#define FILE_LEN			(s32)100
//Database Name
#define DBNAME_LEN			(s32)17
//文件路径长度
#define PATH_LEN			(s32)256
//Entry名的长度
#define	ENTRY_LEN			(s32)20
//Community长度
#define COMMUNITY_LEN		(s32)50
//OID Name长度
#define OID_NAME_LEN		(s32)100

//Snmp默认端口
#define SNMP_DEFAULT_PORT	(u16)161
//接收告警端口
#define TRAP_DEFAULT_PORT   (u16)162

//数据库中和Mcu/Mt相关的表的数目
#define MCU_TABLE_NUM		9
#define MT_TABLE_NUM		5

//系统实例限制
#define MAX_INSTACE_NUM			16448
#define NORMAL_INSTACE_NUM		10
#define MAX_SESSION_NUM			8

//根据InstanceID判断全局数据中的Index
#define GET_INDEX(x) x != CInstance::DAEMON ? x - 1 : NORMAL_INSTACE_NUM - 1

//一次同步最多告警数目
#define	MAX_ALARM_NUM		200

inline void STRNCPY(TCHAR * const pszDst, LPCTSTR pszSrc, size_t nCount)
{	
    _tcsncpy( pszDst, pszSrc, nCount - 1 );
    pszDst[nCount - 1] = 0;
}

//定义服务器的配置文件
#define CFG_SECTION_SYSTEM      (LPCSTR)"System"
#define CFG_ITEM_SVR_IP         (LPCSTR)"Server"
#define CFG_ITEM_TRAPPORT       (LPCSTR)"TrapPort"
#define CFG_ITEM_OSP_PORT       (LPCSTR)"OspConnPort"
#define CFG_ITEM_TELNET_PORT    (LPCSTR)"TelnetPort"
#define CFG_ITEM_CM_PERIOD      (LPCSTR)"CmSyncPeriod"
#define CFG_ITEM_FM_PERIOD      (LPCSTR)"FmSyncPeriod"
#define CFG_ITEM_SNMP_TIMEOUT   (LPCSTR)"SnmpTimeout"
#define CFG_ITEM_READ_COMMUNITY (LPCSTR)"ReadCommunity"
#define CFG_ITEM_WRITE_COMMUNITY    (LPCSTR)"WriteCommunity"

#define CFG_SECTION_DB          (LPCSTR)"Database"
#define CFG_ITEM_DB_SERVER      (LPCSTR)"DBServer"
#define CFG_ITEM_DB_NAME        (LPCSTR)"DBName"
#define CFG_ITEM_DB_USER        (LPCSTR)"DBUser"
#define CFG_ITEM_DB_PWD         (LPCSTR)"DBPwd"

#define CFG_SECTION_FTP             (LPCSTR)"Ftp"
#define CFG_ITEM_REMOTE_FTP_USER    (LPCSTR)"FtpRemoteUser"
#define CFG_ITEM_REMOTE_FTP_PWD     (LPCSTR)"FtpRemotePwd"
#define CFG_ITEM_LOCAL_FTP_USER     (LPCSTR)"FtpLocalUser"
#define CFG_ITEM_LOCAL_FTP_PWD      (LPCSTR)"FtpLocalPwd"
#define CFG_ITEM_FTP_LOCAL_PATH     (LPCSTR)"FtpLocalPath"

#define CFG_SECTION_LOG             (LPCSTR)"Log"
#define CFG_ITEM_LOG_PRINTSCREEN    (LPCSTR)"PrintScreenLevel"
#define CFG_ITEM_LOG_PRINTFILE      (LPCSTR)"PrintFileLevel"
#define CFG_ITEM_LOG_FILE           (LPCSTR)"LogFile"

#define CFG_SECTION_LANGUAGE        (LPCSTR)"Language"
#define CFG_ITEM_LANGUAGE_TYPE      (LPCSTR)"Type"


typedef std::list<u32> TAlarmFilterList;

//临界区定义
class CSection
{
public:
	CSection(CRITICAL_SECTION *pSection, CEvent *pcEvent = NULL,
             BOOL32 bIsSection = TRUE)
	{
        if ( bIsSection )
        {
		    m_pSection = pSection;
		    m_pcEvent = NULL;
		    EnterCriticalSection( m_pSection );
        }
        else
        {
            m_pSection = NULL;
		    m_pcEvent = pcEvent;
		    m_pcEvent->ResetEvent();
        }
	}

	~CSection()
	{
		if ( m_pSection != NULL )
			LeaveCriticalSection( m_pSection );

		if ( m_pcEvent != NULL )
			m_pcEvent->SetEvent();
	}

private:
    CSection() {}   //  wanghao 2005/11/07 不允许调用默认构造函数

private:
	CRITICAL_SECTION    *m_pSection;
	CEvent              *m_pcEvent;
};

#endif
