/*****************************************************************************
   模块名      : mcu
   文件名      : mcuguard.cpp
   相关文件    : mcuguard.h
   文件实现功能: MCU守卫应用类定义
   作者        : 李屹
   版本        : V0.9  Copyright(C) 2001-2002 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   2002/07/25  0.9         李屹        创建
******************************************************************************/
#include "commonlib.h"
#include "evmcu.h"
#include "mcuvc.h"
#include "mcuguard.h"
#include "kdvencrypt.h"
#include "readlicense.h"
#include "usbkeyconst.h"
#include "licensekeyconst.h"

#ifdef _VXWORKS_
    #include "memlib.h"
    #include "tasklib.h"
    #include "brddrvlib.h"
#endif

// MPC2 支持
#ifdef _LINUX_
    #ifdef _LINUX12_
        #include "brdwrapper.h"
        #include "brdwrapperdef.h"
        #include "nipwrapper.h"
    #else
        #include "boardwrapper.h"
    #endif
#endif

CMcuGuardApp	g_cMcuGuardApp;	//MCU守卫应用实例
BOOL32  g_bEnableMcuGuard = TRUE;

#ifdef _LINUX_
void writeexclog(const s8 *pachBuf, s32 nBufLen);
#endif

// [11/15/2011 liuxu] 
CGuardCheck g_cMcuVcGuardCheck;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMcuGuardInst::CMcuGuardInst()
{
	m_dwGuardTestSeriNo = 0;
	m_byCurSyncSourceState = 5; //将初始状态设为SYNCSOURCE_NORMAL，SYNCSOURCE_ABNORMAL以外的值
	m_bySyncSourceMode  = 0;
	m_byTaskState      = 0;
    m_bRebootMcu = FALSE;
	memset( m_abyTaskBusyTimes, 0, sizeof( m_abyTaskBusyTimes ) );
}

CMcuGuardInst::~CMcuGuardInst()
{

}

/*====================================================================
    函数名      ：InstanceEntry
    功能        ：实例消息处理入口函数，必须override
    算法实现    ：
    引用全局变量：
    输入参数说明：CMessage * const pcMsg, 传入的消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    2003-4-16					杨皞昀
====================================================================*/
void CMcuGuardInst::InstanceEntry( CMessage * const pcMsg )
{
	if( NULL == pcMsg )
	{
		printf( "CMcuGuardInst: The received msg's pointer in the msg entry is NULL!");
		LogPrint( LOG_LVL_ERROR, MID_MCU_GUARD, "[CMcuGuardInst] ERROR: The received msg's pointer is NULL!" );
		return;
	}
	//printf("CMcuGuardInst: instid(%d) recv msg(%d)!\n",GetInsID(), pcMsg->event);
	LogPrint( LOG_LVL_DETAIL, MID_MCU_GUARD, "[CMcuGuardInst] INFO: instid(%d) recv msg(%d)!\n",GetInsID(), pcMsg->event );

	switch( pcMsg->event )
	{
		case OSP_POWERON:				//上电消息
			ProcReoPowerOn( );
			break;
		case MCUGD_SCHEDULED_CHECK_TIMER:
			ProcTaskCheck();
			ProcMemCheck();
			ProcFileSystemCheck();
			ProcCpuStatusCheck();
			ProcSyncSrcStateCheck();
			//Send Test packet 			
			PingEveryTask();
			//restart the check timer
			SetTimer( MCUGD_SCHEDULED_CHECK_TIMER, TIMESPACE_GUARD_ONGOING_CHECK/6 );
			break;
		case MCUGD_FEEDDOG_TIMER:
			ProcTimerFeedDog();
			break;
		case MCU_APPTASKTEST_ACK:
			ProcTaskTestAck( pcMsg );
			break;
		case AGT_SVC_REBOOT:
			ProcPowerOff();
			break;
		case MCU_STOPGUARDTIMER_CMD:
			KillTimer( MCUGD_SCHEDULED_CHECK_TIMER );
			break;
		case MCU_STARTGUARDTIMER_CMD:
			SetTimer( MCUGD_SCHEDULED_CHECK_TIMER, TIMESPACE_GUARD_ONGOING_CHECK/6 );
			break;		
		// get license data when mcu power on 
		case MCUGD_GETLICENSE_DATA:
			printf("begin to read license as linux!\n");
			ProcGetLicenseDataFromFile();
			break;

		default:
			break;
	}
	return;
}

/*====================================================================
    函数名      ：ProcReoPowerOn
    功能        ：上电处理
    算法实现    ：
    引用全局变量：
    输入参数说明：无
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    2003-4-16					杨皞昀
====================================================================*/
void CMcuGuardInst::ProcReoPowerOn( )
{
	g_cMcuVcGuardCheck.Init(TIMESPACE_MAX_FAILED_TIMES);

	//set reboot msg
    g_cMcuAgent.SetRunningMsgDst( AGT_SVC_POWERON, MAKEIID(AID_MCU_GUARD, 1) );
	g_cMcuAgent.SetRebootMsgDst( AGT_SVC_REBOOT, MAKEIID(AID_MCU_GUARD, 1) );
	g_cMcuAgent.SetPowerOffMsgDst( AGT_SVC_POWEROFF, MAKEIID(AID_MCU_GUARD, 1) );

#if defined(_8KE_) || defined(_8KH_) || defined(_8KI_)
	// 8ke don't execute the following watchdog and don't ping app action as these things will cause crash in x86 system
	// FIXME: ospquit will cause exception   
	return;
#endif

	//Set the sync source mode
	TNetSyncInfo tNetSyncInfo;
	g_cMcuAgent.GetNetSyncInfo(&tNetSyncInfo);
	m_bySyncSourceMode = tNetSyncInfo.GetMode();
	ProcSyncSrcStateCheck();
	//start the check timer
	PingEveryTask();
	SetTimer( MCUGD_SCHEDULED_CHECK_TIMER, TIMESPACE_GUARD_ONGOING_CHECK/6 );

	// change feed dog mode

#ifndef WIN32
	if (g_cMcuVcApp.IsWatchDogEnable())
	{

#if defined(_VXWORKS_)
        // 硬件喂狗，业务不管理
		BrdMpcWatchdogMode( WATCHDOG_USE_CLK );
#elif defined(_LINUX_)

        // 启动系统 喂狗检测, 时间单位为秒
        SysOpenWdGuard( TIMESPACE_FEEDDOG * 3 / 1000);
        // 开始喂狗
        SetTimer( MCUGD_FEEDDOG_TIMER, TIMESPACE_FEEDDOG);	
	
#endif
	}
	else
	{
		SysRebootDisable();
	}
#endif

// #ifdef _LINUX12_
// 	
// 	 	s8 *pszName = NULL;
// 	 	#ifdef _8KI_
// 	 		pszName = "mcu_8000i";
// 	 	#else
// 	 		pszName = "mcu";
// 	 	#endif
	
// 	if(!NotfiyGuardUpdateSuccess(pszName))
// 	{
// 		LogPrint(LOG_LVL_WARNING,MID_MCU_GUARD,"[ProcReoPowerOn] NotfiyGuardUpdateSuccess failed!\n");
// 	}
/*#endif*/
	
	return;
}

/*====================================================================
    函数名      ：ProcMemCheck
    功能        ：内存检查
    算法实现    ：
    引用全局变量：
    输入参数说明：无
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    2003-4-16					杨皞昀
====================================================================*/
void CMcuGuardInst::ProcMemCheck(void)
{
#ifdef _VXWORKS_
	MEM_PART_STATS  tMemPartInfo;

	memPartInfoGet(memSysPartId, &tMemPartInfo);
	u32 dwMemTop = (u32)sysPhysMemTop();
	//u32 dwMemAllocRate = tMemPartInfo.numBytesAlloc /(dwMemTop/100);

    u32 dwMemSpare = dwMemTop - tMemPartInfo.numBytesAlloc;

    if (dwMemSpare < MEMSPACE_MIN_SIZE)
    {
		LogPrint( LOG_LVL_WARNING, MID_MCU_GUARD, "[CMcuGuardInst] WARNING: The Free system memory is not enough(%d<3M)!\n", dwMemSpare );
    }
/*  else
    {
        LogPrint(LOG_LVL_ERROR, MID_MCU_GUARD,  "CMcuGuardInst: dwMemTop is %d!\n", dwMemTop);
        LogPrint(LOG_LVL_ERROR, MID_MCU_GUARD, "CMcuGuardInst: tMemPartInfo.numBytesAlloc is %d!\n", tMemPartInfo.numBytesAlloc);
        LogPrint(LOG_LVL_ERROR, MID_MCU_GUARD, "CMcuGuardInst: The Free system memory is %d!\n", dwMemSpare);
    }*/
    
/*
	if( dwMemAllocRate > STATUS_ALLFULL )
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_GUARD, "CMcuGuardInst: The Free system memory is not enough(>%95)!\n");
	}
	else if( dwMemAllocRate > STATUS_CONGESTION )
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_GUARD, "CMcuGuardInst: The use of system memory is congestion(>%85)!\n");
	}
	else if( dwMemAllocRate > STATUS_BUSY )
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_GUARD, "CMcuGuardInst: The use of system memory is busy(>%75)!\n");
	}
	else
	{

	}
*/
#endif

	return;
}

/*====================================================================
    函数名      ：ProcFileSystemCheck
    功能        ：文件系统检查
    算法实现    ：
    引用全局变量：
    输入参数说明：无
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    2003-4-16					杨皞昀
====================================================================*/
void CMcuGuardInst::ProcFileSystemCheck(void)
{
	u8 byTimes = 5;

	for (u8 bycounter=0; bycounter<byTimes; bycounter++)
	{//creat a file and continue to open it five times
		
	}

	return;
}

/*====================================================================
    函数名      ：ProcTaskCheck
    功能        ：MCU任务检查
    算法实现    ：
    引用全局变量：
    输入参数说明：无
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    2003-4-16					杨皞昀
====================================================================*/
void CMcuGuardInst::ProcTaskCheck(void)
{
	//Check whether every App's Ack came.
	for (u8 byLoop = 0; byLoop < SHIFT_END_CHECK; byLoop++)
	{
		ProcEachTaskCheck( byLoop );
	}

	return;
}

/*====================================================================
    函数名      ：ProcTaskTestAck
    功能        ：MCU任务检查确认处理
    算法实现    ：
    引用全局变量：
    输入参数说明：CMessage * pcMsg ；各被检查任务反馈消息
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    2003-4-16				杨皞昀
====================================================================*/
void CMcuGuardInst::ProcTaskTestAck( const CMessage * pcMsg )
{
	u16  wAppId;
	char achName[32];
	u32  dwMsgSeriNo = *(u32 *)pcMsg->content;
	
	if (dwMsgSeriNo != m_dwGuardTestSeriNo)
	{
		return;
	}

	wAppId = GETAPP(pcMsg->srcid);
	switch ( wAppId )
	{
		case AID_MCU_MCSSN:
			m_byTaskState |= ( u8 )1 << SHIFT_MCSSN_CHECK;
			strncpy( achName, "McSsn", sizeof( achAppName ) );
			achName[ sizeof( achName ) - 1 ] = 0;
			break;
		case AID_MCU_PERIEQPSSN:
			m_byTaskState |= ( u8 )1 << SHIFT_PERIEQPSSN_CHECK;
			strncpy( achName, "EqpSsn", sizeof( achAppName ) );
			achName[ sizeof( achName ) - 1 ] = 0;
			break;
		case AID_MCU_VC:
			m_byTaskState |= ( u8 )1 << SHIFT_MCUVC_CHECK; 
			strncpy( achName, "McuVc", sizeof( achAppName ) );
			achName[ sizeof( achName ) - 1 ] = 0;
			break;
		case AID_MCU_CONFIG:
			m_byTaskState |= ( u8 )1 << SHIFT_MCUCONFIG_CHECK;
			strncpy( achName, "McuConfig", sizeof( achAppName ) );
			achName[ sizeof( achName ) - 1 ] = 0;
			break;
		case AID_MCU_MPSSN:
			m_byTaskState |= ( u8 )1 << SHIFT_MPSSN_CHECK;
			strncpy( achName, "MpSsn", sizeof( achAppName ) );
			achName[ sizeof( achName ) - 1 ] = 0;
			break;
		case AID_MCU_MTADPSSN:
			m_byTaskState |= ( u8 )1 << SHIFT_MTADPSSN_CHECK;
			strncpy( achName, "MtadpSsn", sizeof( achAppName ) );
			achName[ sizeof( achName ) - 1 ] = 0;
			break;
        case AID_MCU_DCSSSN:
            m_byTaskState |= ( u8 )1 << SHIFT_DCSSSN_CHECK;
            strncpy( achName, "DcsSsn", sizeof( achAppName ) );
            achName[ sizeof( achName ) - 1 ] = 0;
            break;
		default:
			LogPrint(LOG_LVL_ERROR, MID_MCU_GUARD, "CMcuGuardInst: receive the wrong task ack msg\n");
			break;
	}

	LogPrint(LOG_LVL_DETAIL, MID_MCU_GUARD, "[ProcTaskTestAck] Ping app %s ack received!\n", achName);	
	return;
}

/*====================================================================
    函数名      ：ProcCpuStatusCheck
    功能        ：CPU检查
    算法实现    ：
    引用全局变量：
    输入参数说明：无
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    2003-4-16					杨皞昀
====================================================================*/
void CMcuGuardInst::ProcCpuStatusCheck(void)
{
	return;
}

/*====================================================================
    函数名      ：ProcTimerSyncSourceCheck(void)
    功能        ：同步源状态检测
    算法实现    ：
    引用全局变量：
    输入参数说明：无
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    2003-7-2				JQL
====================================================================*/
void CMcuGuardInst::ProcSyncSrcStateCheck( void )
{
	//定时检测
	u8	bySyncCurState = GuardGetBrdNetSyncLock();
	CSyncSourceAlarm cSyncSourceAlarm;
	
	//Only Post Message to Agent when Source Sync changed!
	if( bySyncCurState != m_byCurSyncSourceState )
	{
		m_byCurSyncSourceState = bySyncCurState;
		memset( &cSyncSourceAlarm, 0, sizeof( CSyncSourceAlarm ) );		
		cSyncSourceAlarm.masterstatus = m_byCurSyncSourceState;
		post( MAKEIID( AID_MCU_AGENT, 1),  MCU_AGT_SYNCSOURCE_STATUSCHANGE, &cSyncSourceAlarm, sizeof( CSyncSourceAlarm ) );	
	}
}


/*====================================================================
    函数名      ：ProcPowerOff
    功能        ：下电消息处理
    算法实现    ：
    引用全局变量：
    输入参数说明：无
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    2003-4-16					杨皞昀
====================================================================*/
void CMcuGuardInst::ProcPowerOff(void)
{
	g_cMcuVcGuardCheck.Destroy();

    //重新启动MCU，记下当前MCU运行状况。
#if ( defined _VXWORKS_ ) && ( !defined _MINIMCU_ )
    FILE  *hFilestream = NULL;
    char   achFullName[] = { "/log/mcudump.log" } ;
    time_t currenttime;
    
    hFilestream = fopen( achFullName, "a+" ); 
    if (NULL != hFilestream)
    {
        MEM_PART_STATS  tMemPartInfo;			
        memPartInfoGet(memSysPartId, &tMemPartInfo);
        u32 dwMemAlloc = tMemPartInfo.numBytesAlloc;
        time( &currenttime );			
        
        fprintf( hFilestream, "Mcu Reboot, Current Time %s.\n", ctime( &currenttime ) );
        fprintf( hFilestream, "System Memory Info: %u Memory Alloced.\n", dwMemAlloc );
        
        for (u8 byIndex = 0; byIndex <= SHIFT_MTADPSSN_CHECK; byIndex++)
        {
            if (m_abyTaskBusyTimes[byIndex] > 0)
            {
                fprintf( hFilestream, "App shift %d, No Ack for %d Times.\n", byIndex, m_abyTaskBusyTimes[byIndex] );
            }		
        }
        
        int taskId = taskNameToId( achAppName );
        if (ERROR != taskId)
        {
            TASK_DESC taskDesc;
            if (ERROR != taskInfoGet( taskId, &taskDesc ))
            {
                fprintf( hFilestream, "Task Name:%s,  StackSize:%d,	 StackMargin:%d,  ErrorCode:%d.\n", 
                    taskDesc.td_name, taskDesc.td_stackSize, taskDesc.td_stackMargin, taskDesc.td_errorStatus );
            }
        }
        fprintf( hFilestream, "\n\n" );
        fclose( hFilestream );								
    }
#endif    

	//send the system start msg to agent
	McuAgentQuit(FALSE);
	
	OspDelay( 1000 );
	
#if !defined(_8KH_)
	OspQuit();
	
	OspDelay( 1000 );
#endif
    printf("Quiting MCU...\n");
	
#if (!defined(_8KE_) && !defined(_8KH_) && !defined(_8KI_)) && (!defined WIN32)
	BrdHwReset();
	printf("[ProcPowerOff] reboot\n");		
#endif

	return;
}

void CMcuGuardInst::ProcGuardTest(void)
{
	return;
}

/*====================================================================
    函数名      :GuardGetBrdNetSyncLock
    功能        ：获取数据板同步状况
    算法实现    ：
    引用全局变量：
    输入参数说明：无
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    2003-7-4				JQL
====================================================================*/
u8 CMcuGuardInst::GuardGetBrdNetSyncLock()
{
	return SYNCSOURCE_NORMAL;
}

/*====================================================================
    函数名      : PingEveryTask
    功能        ：向MCU的每一个应用发送测试包
    算法实现    ：
    引用全局变量：
    输入参数说明：无
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    2003-7-4				JQL
====================================================================*/
void CMcuGuardInst::PingEveryTask()
{
	//IncompletePro: 存在主备同步造成各App锁定时暂不作特殊处理

	//测试之前，初始化状态变量
	m_byTaskState = 0;
	m_dwGuardTestSeriNo++;

	//发送数据包
	post( MAKEIID( AID_MCU_MCSSN, CInstance::DAEMON ), MCU_APPTASKTEST_REQ, 
		  (u8 *)&m_dwGuardTestSeriNo, sizeof(m_dwGuardTestSeriNo) );
	post( MAKEIID( AID_MCU_PERIEQPSSN, CInstance::DAEMON ), MCU_APPTASKTEST_REQ, 
		  (u8 *)&m_dwGuardTestSeriNo, sizeof(m_dwGuardTestSeriNo) );
	post( MAKEIID( AID_MCU_VC, CInstance::DAEMON ), MCU_APPTASKTEST_REQ, 
		  (u8 *)&m_dwGuardTestSeriNo, sizeof(m_dwGuardTestSeriNo) );
	post( MAKEIID( AID_MCU_CONFIG, CInstance::DAEMON ), MCU_APPTASKTEST_REQ, 
		  (u8 *)&m_dwGuardTestSeriNo, sizeof(m_dwGuardTestSeriNo) );
	post( MAKEIID( AID_MCU_MPSSN, CInstance::DAEMON ), MCU_APPTASKTEST_REQ, 
		  (u8 *)&m_dwGuardTestSeriNo, sizeof(m_dwGuardTestSeriNo) );
	post( MAKEIID( AID_MCU_MTADPSSN, CInstance::DAEMON ), MCU_APPTASKTEST_REQ, 
		  (u8 *)&m_dwGuardTestSeriNo, sizeof(m_dwGuardTestSeriNo) );
    post( MAKEIID( AID_MCU_DCSSSN, CInstance::DAEMON ), MCU_APPTASKTEST_REQ, 
        (u8 *)&m_dwGuardTestSeriNo, sizeof(m_dwGuardTestSeriNo) );

	return;
}

/*====================================================================
    函数名      : ProcEachTaskCheck
    功能        ：检测每一个任务状态
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 AppId 应用号；u8 Shift 在TaskBusyTimes中的位置
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    2003-7-17				JQL
====================================================================*/
void CMcuGuardInst::ProcEachTaskCheck( u8 byShift )
{
	u8 byAppId;
	
	//依据偏移量得到应用号，应用名
	switch ( byShift ) 
	{
	case SHIFT_MCSSN_CHECK:
		byAppId = AID_MCU_MCSSN;
		strncpy( achAppName, "McSsn", sizeof( achAppName ) );
		achAppName[ sizeof( achAppName ) - 1 ] = 0;
		break;
	case SHIFT_PERIEQPSSN_CHECK:
		byAppId = AID_MCU_PERIEQPSSN;
		strncpy( achAppName, "EqpSsn", sizeof( achAppName ) );
		achAppName[ sizeof( achAppName ) - 1 ] = 0;
		break;
	case SHIFT_MCUVC_CHECK:
		byAppId = AID_MCU_VC;
		strncpy( achAppName, "McuVc", sizeof( achAppName ) );
		achAppName[ sizeof( achAppName ) - 1 ] = 0;
		break;
	case SHIFT_MCUCONFIG_CHECK:
		byAppId = AID_MCU_CONFIG;
		strncpy( achAppName, "McuConfig", sizeof( achAppName ) );
		achAppName[ sizeof( achAppName ) - 1 ] = 0;
		break;
	case SHIFT_MPSSN_CHECK:
		byAppId = AID_MCU_MPSSN;
		strncpy( achAppName, "MpSsn", sizeof( achAppName ) );
		achAppName[ sizeof( achAppName ) - 1 ] = 0;
		break;
	case SHIFT_MTADPSSN_CHECK:
		byAppId = AID_MCU_MTADPSSN;
		strncpy( achAppName, "MtadpSsn", sizeof( achAppName ) );
		achAppName[ sizeof( achAppName ) - 1 ] = 0;
		break;
    case SHIFT_DCSSSN_CHECK:
        byAppId = AID_MCU_DCSSSN;
        strncpy( achAppName, "DcsSsn", sizeof( achAppName ) );
        achAppName[ sizeof( achAppName ) - 1 ] = 0;
        break;
	default:
		return;
	}
		
	if ( 0 == (m_byTaskState & (u8)((unsigned int)1 << byShift)) )
	{
		printf( "McuGuard: The %s Task maybe is busy or dead!\n", achAppName );
		LogPrint( LOG_LVL_KEYSTATUS, MID_MCU_GUARD, "[ProcEachTaskCheck] The %s Task maybe busy or dead!\n", achAppName );
		
		//连续三次应用没有返回应答，向代理发告警
		if (TIMESPACE_MAX_FAILED_TIMES == m_abyTaskBusyTimes[byShift])
		{
			
			if ( SHIFT_MCUVC_CHECK == byShift )
			{
				g_cMcuVcGuardCheck.GuardCheck();
				if( !g_cMcuVcGuardCheck.IsClientUnRsp())
				{
					printf("The McuVc Thread is busy\n");
					StaticLog("The McuVc Thread is busy\n");
					return;
				}
			}

			CTaskStatus taskStatus;
			taskStatus.appid = byAppId;
			taskStatus.status = TASK_BUSY;
			post( MAKEIID( AID_MCU_AGENT, 1 ), SVC_AGT_TASK_STATUS, &taskStatus, sizeof( CTaskStatus ) );
			LogPrint( LOG_LVL_ERROR, MID_MCU_GUARD, "[ProcEachTaskCheck] The %s Task is dead, usptimes is %d!\n", achAppName, m_abyTaskBusyTimes[byShift] );

			//重启MCU
#ifndef _DEBUG
			m_bRebootMcu = TRUE;
			LogPrint( LOG_LVL_KEYSTATUS, MID_MCU_GUARD, "[ProcEachTaskCheck] Task %s has no response, The System will Reboot!\n", achAppName );

            //  xsl [6/16/2006] 采用上层喂狗方式可能造成误判，因为底层超时时间为1.6秒，所以vx下暂时采用硬件喂狗的方式
            if (g_bEnableMcuGuard)
            {
                ProcPowerOff();
            }            
#endif		
		}
		else
		{
			
			if( SHIFT_MCUVC_CHECK == byShift)
			{
				g_cMcuVcGuardCheck.GuardCheck();
			}

			m_abyTaskBusyTimes[byShift]++;
		}
	}
	else
	{
		//如果任务状态正常，向代理发送恢复告警
		if (TIMESPACE_MAX_FAILED_TIMES == m_abyTaskBusyTimes[byShift])
		{
			CTaskStatus taskStatus;
			taskStatus.appid = byAppId;
			taskStatus.status = TASK_NORMAL;
			post( MAKEIID( AID_MCU_AGENT, 1 ), SVC_AGT_TASK_STATUS, &taskStatus, sizeof( CTaskStatus ) );
		}

		if( SHIFT_MCUVC_CHECK == byShift)
		{
			g_cMcuVcGuardCheck.GuardCheck();
		}

		m_abyTaskBusyTimes[byShift] = 0;
	}

	return;
}

/*====================================================================
    函数名      : ProcTimerFeedDog
    功能        ：喂狗定时器到
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    2003-10-22				JQL
====================================================================*/
void CMcuGuardInst::ProcTimerFeedDog()
{
#ifdef _LINUX_
    
	LogPrint(LOG_LVL_DETAIL, MID_MCU_GUARD, "[ProcTimerFeedDog] BrdQueryNipState will be calling!\n");
    if (OK != BrdQueryNipState())
    {
        m_bRebootMcu = TRUE;
              
        // xsl [11/1/2006] 写入日志文件
        s8 achInfo[255];
        time_t tiCurTime = ::time(NULL);             
        s32 nLen = sprintf(achInfo, "\nSytem time %s\n%s\n\n", 
                            ctime(&tiCurTime), "BrdQueryNipState is failed, system will stop feed dog.");
        writeexclog(achInfo, nLen);
        LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_GUARD, achInfo);
    }
#endif

	//如果不需重启MCU，继续喂狗
	if (!m_bRebootMcu)
	{
#if defined(_LINUX_)
        // 调用Linux系统 feed
		SysNoticeWdGuard();
        SetTimer( MCUGD_FEEDDOG_TIMER, TIMESPACE_FEEDDOG );

        LogPrint(LOG_LVL_DETAIL, MID_MCU_GUARD, "[ProcTimerFeedDog] BrdMpcFeedDog() and settimer, curTicks:%d\n", OspTickGet());
#elif defined(_VXWORKS_)
        // 暂时不做操作。硬件喂狗
#endif	                
	}

    return;
}

/*=============================================================================
函 数 名： ProcGetLicenseDataFromFile
功    能： 加载license key
算法实现： 
全局变量： 
参    数： const CMessage *pcMsg
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2006/10/20  4.0			许世林                  创建
2007/01/23  4.0			周广程				  增加对新版本License的读写功能，
											  同时保持对旧版本的支持
=============================================================================*/
void CMcuGuardInst::ProcGetLicenseDataFromFile( void )
{
//    u8 achEncText[LEN_KEYFILE_CONT] = {0};
	
    // guzh [4/25/2007] 首先检查产品类型
    if ( !g_cMcuAgent.IsMcuPdtBrdMatch( GetMcuPdtType() ) )
    {
        printf("[Mcu License] Illegale Running Board! IsMcuPdtBrdMatch False! GetMcuPdtType is:%d\n", GetMcuPdtType());
        g_cMcuVcApp.SetLicenseNum( 0 );
        g_cMcuGuardApp.SetLicenseErrorCode( 9 );
		return;        
    }

    s8 achKeyFileName[KDV_MAX_PATH] = {0};
    sprintf(achKeyFileName, "%s/%s", DIR_DATA, KEY_FILENAME);

	BOOL32 bType8ke = ( GetMcuPdtType() == MCU_TYPE_KDV8000E );
	BOOL32 bRet = TRUE;
	//与旧版本兼容, zgc, 2007/01/23
	FILE *pHandler = fopen( achKeyFileName, "rb" );
	if( NULL == pHandler )
	{
		printf("[Mcu License] fail to open key file.\n");
		g_cMcuVcApp.SetLicenseNum( 0 );
        g_cMcuGuardApp.SetLicenseErrorCode( 1 );
		bRet = FALSE;

	}    
	else
	{
		fseek(pHandler, 0, SEEK_SET);
		
		s8 achcopyright[128];
		memset( achcopyright, 0 ,sizeof(achcopyright) );
		
		fread( achcopyright, strlen(KEDA_COPYRIGHT), 1, pHandler );
		fclose( pHandler );
		if( 0 == memcmp( achcopyright, KEDA_COPYRIGHT, strlen(KEDA_COPYRIGHT) ) )
		{
			//读旧版本License, zgc, 2007/01/23
			printf("read old license!\n");
			GetOldLicenseDataFromFile( achKeyFileName );
		}
		else
		{
			//读新版本License, zgc, 2007/01/23
			printf("read new license!\n");
			bRet = GetCurrentLicenseDataFromFile( achKeyFileName );
		}
	}

	if(!bRet && bType8ke)
	{
		u8 byDefLicenseNum = 0;
		u8 byDefLicenseHdMtNum = 0;
		u8 byDefLicensePcMtNum = 0;
#ifdef _NOUSBKEY_// [3/15/2011 xliang] 编译自测版本加该宏
		byDefLicenseNum = 16;
		byDefLicenseHdMtNum = 16;
		byDefLicensePcMtNum = 16;
#endif
		printf( "readlicense error, so set default value(%d,%d,%d) for testing!\n", 
			byDefLicenseNum, byDefLicenseHdMtNum, byDefLicensePcMtNum );
		LogPrint( LOG_LVL_KEYSTATUS, MID_MCU_GUARD, "[ProcGetLicenseDataFromFile] : readlicense error, so set default value(%d,%d,%d) for testing!\n", 
			byDefLicenseNum, byDefLicenseHdMtNum, byDefLicensePcMtNum );

		g_cMcuVcApp.SetLicenseNum(byDefLicenseNum);
		g_cMcuVcApp.SetLicenseHDMtNum(byDefLicenseHdMtNum);
		g_cMcuVcApp.SetLicensePCMtNum(byDefLicensePcMtNum);
	}

	// [miaoqingsong 20111202] 向MCU代理设置Lincse数目
	u16 wLicenseNum = htons(g_cMcuVcApp.GetLicenseNum());
#if defined(_8KE_) || defined(_8KH_) || defined(_8KI_)
	g_cMcuAgent.SetAuthMTTotal(ntohs(wLicenseNum));
#else
	g_cMcuAgent.SetAuthMTTotal(wLicenseNum);
#endif

	//单板搜索基本信息写入配置文件[4/19/2013 chendaiwei]

#ifdef _LINUX_
	s8 achMcuType[64] ={0};
	s8 achMcuAlias[64] = {0};
	switch( GetMcuPdtType())
	{
	case MCU_TYPE_KDV8000E:
		{
			strcpy(achMcuType,"8000G");
			strcpy(achMcuAlias,"8000G");
		}
		break;
	case MCU_TYPE_KDV8000H:
		{
			strcpy(achMcuType,"8000H");
			strcpy(achMcuAlias,"8000H");
		}
		break;
	case MCU_TYPE_KDV8000H_M:
		{
			strcpy(achMcuType,"8000H-M");
			strcpy(achMcuAlias,"8000H-M");
		}
		break;
	case MCU_TYPE_KDV800L:
		{
			strcpy(achMcuType,"800L");
			strcpy(achMcuAlias,"800L");
		}
		break;
	case MCU_TYPE_KDV8000I:
		{
			strcpy(achMcuType,"8000I");
			strcpy(achMcuAlias,"8000I");
		}
		break;
	default:
		return;
		break;
	}

	//方案二：
    s8 achConfigFilePath[64]={0};
	sprintf(achConfigFilePath,"%s/%s",DIR_CONFIG,"kdvipdt.ini");
	
	FILE *hFile = fopen(achConfigFilePath,"a+");
	if( hFile == NULL)
	{
		printf("[GuardInst]open or create kdvipdt.ini failed!\n");
		
		return;
	}
	else
	{
		fclose(hFile);
	}
	
	if(!SetRegKeyString(achConfigFilePath,"Dev","TypeAlias",achMcuType))
	{
		printf("[GuardInst]set type alis failed!\n");
		
		return;
	}
	
	if(!SetRegKeyString(achConfigFilePath,"Dev","Alias",achMcuAlias))
	{
		printf("[WriteBoardInfoToDetecteeFile]set device alis failed!\n");
		
		return;
	}
#endif
//end:方案二

	return;
}

void CMcuGuardInst::GetBoardSequenceNum( u8 *pBuf, s8* achSep )
{
#ifndef WIN32
	TBrdEthParam tBrdEthParam;
	u8 byEthId = g_cMcuAgent.GetInterface(); // 前后网口选择

    u8 byRet = BrdGetEthParam( byEthId, &tBrdEthParam );
	
	s8 achMacBuf[64];
	memset( achMacBuf, 0, sizeof(achMacBuf) );
    s8 szTmp[3] = {0};
	u8 byLp = 0;
	while(byLp < 6)
	{
        sprintf(szTmp, "%.2X", tBrdEthParam.byMacAdrs[byLp] );
        strcat( achMacBuf, szTmp);
        if (byLp < 5)
        {
            // guzh [7/3/2007] 自定义分割符
            strcat(achMacBuf, achSep);
        }
        byLp ++;
	}
	
	memcpy( pBuf, achMacBuf, strlen(achMacBuf) );
#endif

	return;
}

void CMcuGuardInst::GenEncodeData(u8 *pOutBuf, u16 wInBufLen, u8 *pInBuf, BOOL32 bDirect)
{
    s8 achIV[MAX_IV_SIZE] = {0};
	
    s8 achKey[32] = {0};
	//strncpy( achKey, KEDA_AES_KEY, sizeof(KEDA_AES_KEY) );// 保持key为16的倍数
	strncpy( achKey, KEDA_AES_KEY, strlen(KEDA_AES_KEY)+1 );// 保持key为16的倍数
    u16 wKeyLen = strlen(achKey);

	if( bDirect ) // eccrypt 
	{
		KdvAES( achKey, wKeyLen, MODE_CBC, DIR_ENCRYPT, achIV,
					   pInBuf, wInBufLen, pOutBuf );
	}
	else  // decrypt
	{
		KdvAES( achKey, wKeyLen, MODE_CBC, DIR_DECRYPT, achIV,
					   pInBuf, wInBufLen, pOutBuf );
	}

	return;
}

/*=============================================================================
函 数 名： GetOldLicenseDataFromFile
功    能： 加载旧版本license key
算法实现： 
全局变量： 
参    数： s8* pchPath
返 回 值： BOOL32 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/01/23  4.0			周广程                 创建
=============================================================================*/
BOOL32 CMcuGuardInst::GetOldLicenseDataFromFile( s8* pchPath )
{
/*lint -save -e438*/

	u8 achEncText[LEN_KEYFILE_CONT] = {0};

	FILE *pHandler = fopen( pchPath, "rb" );
	if( NULL == pHandler )
	{
		printf("[Mcu License] fail to open key file.\n");
		g_cMcuVcApp.SetLicenseNum( 0 );
        g_cMcuGuardApp.SetLicenseErrorCode( 1 );
		return FALSE;
	}
	
	fseek(pHandler, 0, SEEK_SET);

	s8 achcopyright[128];
	memset( achcopyright, 0 ,sizeof(achcopyright) );
	
	fread( achcopyright, strlen(KEDA_COPYRIGHT), 1, pHandler );
	
	u16 wEncTextLen = 0;
	fread( &wEncTextLen, sizeof(wEncTextLen), 1, pHandler );    // 这里读出来应该是 LEN_KEYFILE_CONT
    wEncTextLen = ntohs(wEncTextLen);
    
	if (wEncTextLen > LEN_KEYFILE_CONT)
    {
        // 如果大于LEN_KEYFILE_CONT，忽略后面的内容
        wEncTextLen = LEN_KEYFILE_CONT;
    }

	fread( achEncText, wEncTextLen, 1, pHandler );

	fclose( pHandler );
	pHandler = NULL;
    
    u8 achDecText[LEN_KEYFILE_CONT] = {0};

	GenEncodeData( achDecText, wEncTextLen, achEncText, FALSE );

    u8 achSequenceStr[64] = {0};
	GetBoardSequenceNum( achSequenceStr, ":" );  //本板的ID
	
    // 1:用户名,不检验
	s8  achSeps[] = "|"; 

	s8 *pchToken = strtok( (s8*)achDecText, achSeps );
	if( NULL == pchToken )
	{
		printf("Error key file, Get UserID Failed!\n");
		g_cMcuVcApp.SetLicenseNum( 0 );
		return FALSE;
	}

    // 2: DeviceID
    pchToken = strtok( NULL, achSeps );
	if( NULL == pchToken )
	{
		printf("Device id null!\n");
		g_cMcuVcApp.SetLicenseNum( 0 );
		g_cMcuGuardApp.SetLicenseErrorCode( 5 );
		return FALSE;
	}
    
	//u16 wSequeLen = strlen((s8*)achSequenceStr);
	StrUpper( (s8*)achSequenceStr );
	StrUpper( pchToken );
	if( 0 != strcmp( pchToken, (s8*)achSequenceStr ) )
	{
		printf("Error key file for this board, bad Device ID!\n");
		g_cMcuVcApp.SetLicenseNum( 0 );
		g_cMcuGuardApp.SetLicenseErrorCode( 5 );
		return FALSE;
	}

    // 3, 授权数量
	pchToken = strtok( NULL, achSeps );
	if( NULL == pchToken )
	{
		printf("Error key file, Get License Failed!\n");
		g_cMcuVcApp.SetLicenseNum( 0 );
		g_cMcuGuardApp.SetLicenseErrorCode( 7 );
		return FALSE;
	}

	g_cMcuVcApp.SetLicenseNum( atoi(pchToken) );        
    // 不能大于容量值
    if (g_cMcuVcApp.GetLicenseNum() > MAXNUM_MCU_MT)
    {
        g_cMcuVcApp.SetLicenseNum(MAXNUM_MCU_MT);
    }

    // 4: Expire Date
    pchToken = strtok( NULL, achSeps );
    if ( NULL != pchToken )
    {
		TKdvTime tDate;
        tDate.SetYear(atoi(pchToken));        
        pchToken = strtok( NULL, achSeps );
        if ( NULL != pchToken )
        {
            tDate.SetMonth(atoi(pchToken));
            pchToken = strtok( NULL, achSeps );
            if (NULL != pchToken)
            {
                tDate.SetDay(atoi(pchToken));
            }            
        } 
        // guzh [5/8/2007] 如果配置超过2030年，认为不限制
        if ( tDate.GetYear() >= 2030 )
        {
            memset( &tDate, 0, sizeof(tDate) );
        }
        g_cMcuVcApp.SetMcuExpireDate(tDate);
    }

	return TRUE;
}

/*=============================================================================
函 数 名： GetCurrentLicenseDataFromFile
功    能： 加载当前版本license key
算法实现： 
全局变量： 
参    数： s8* pchPath
返 回 值： BOOL32 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/01/23  4.0			周广程                 创建
=============================================================================*/
BOOL32 CMcuGuardInst::GetCurrentLicenseDataFromFile( s8* pchPath )
{
//#ifndef _8KE_
	
#ifndef WIN32    
	u16 wRet = 0; 
	wRet = ReadLicenseFile( pchPath );
	if(SUCCESS_LICENSE != wRet) 
	{
		printf("Read License file failed! wRet = %d!\n", wRet);
		g_cMcuVcApp.SetLicenseNum( 0 );
		g_cMcuGuardApp.SetLicenseErrorCode( 1 );
		return FALSE;
	}
	
	s8 achGetValue[50];
	s32 nValuelen = 50;

#if !defined(_8KE_) && !defined(_8KH_) && !defined(_8KI_)
	//读许可证版本号
	nValuelen = 50;
	memset( achGetValue, 0, sizeof(achGetValue) );
	if( SUCCESS_LICENSE != ( wRet = GetValue(KEY_LICENSE_VERSION, achGetValue, nValuelen) ) )
	{
		printf("Read license version failed!\n");
		g_cMcuVcApp.SetLicenseNum( 0 );
		g_cMcuGuardApp.SetLicenseErrorCode( 2 ); 
		return FALSE;
	}
	printf("License version: %s\n",achGetValue);
#endif
    // License 版本号是用于区分不同版本的License，本身不做校验
    /*
	if( 0 != memcmp( LICENSE_VERSION, achGetValue, strlen(LICENSE_VERSION) ) )
	{
		printf("License version is not correct!\n");
		g_cMcuVcApp.SetLicenseNum( 0 );
		g_cMcuGuardApp.SetLicenseErrorCode( 2 );
		return FALSE;
	}
	//printf("License version: %s\n",achGetValue);
    */

	//读许可证类型
	nValuelen = 50;
	memset( achGetValue, 0, sizeof(achGetValue) );
	if( SUCCESS_LICENSE != ( wRet = GetValue(KEY_LICENSE_TYPE, achGetValue, nValuelen) ) )
	{
		printf("Read license type failed!\n");
		g_cMcuVcApp.SetLicenseNum( 0 );
		g_cMcuGuardApp.SetLicenseErrorCode( 3 );
		return FALSE;
	}
	if( 0 != memcmp( LICENSE_TYPE, achGetValue, strlen(LICENSE_TYPE) ) )
	{
		printf("License type is not correct!\n");
		printf("License type: %s\n",achGetValue);
		g_cMcuVcApp.SetLicenseNum( 0 );
		g_cMcuGuardApp.SetLicenseErrorCode( 3 );
		return FALSE;
	}
	printf("License type: %s\n",achGetValue);
	
	//读License Key
	nValuelen = 50;
	memset( achGetValue, 0, sizeof(achGetValue) );
	if( SUCCESS_LICENSE != ( wRet = GetValue( (s8*)KEY_LICENSE_KEY, achGetValue, nValuelen ) ) )
	{
		printf("Read LicenseKey failed");
		g_cMcuVcApp.SetLicenseNum( 0 );
		g_cMcuGuardApp.SetLicenseErrorCode( 4);
		return FALSE;
	}
    g_cMcuGuardApp.SetLicenseSN(achGetValue);
	printf("License key: %s\n",achGetValue);

	//读设备ID
    nValuelen = 50;
	memset( achGetValue, 0, sizeof(achGetValue) );
	if( SUCCESS_LICENSE != ( wRet = GetValue(KEY_MCU_DEVICE_ID, achGetValue, nValuelen) ) )
	{
		printf("Read device id failed!\n");
		g_cMcuVcApp.SetLicenseNum( 0 );
		g_cMcuGuardApp.SetLicenseErrorCode( 5 );
		return FALSE;
	}
    StrUpper( achGetValue );

	// [6/3/2010 xliang] 对于license生成时容易在mac地址末尾无意添加了空格这种情况，这里进行容错。
	printf("Device id from license: [%s]\n",achGetValue);
	if( achGetValue[17] == ' ')
	{
		achGetValue[17] = '\0';
	}
	
	printf("Device id from license modified: [%s]\n",achGetValue);

#if defined(_8KE_) || defined(_8KH_) || defined(_8KI_)
	BOOL32 bMacMatch = FALSE;
	s8 achMacAddr[MAXNUM_ETH_INTERFACE][32] = {0};

#if defined(_8KE_) || defined(_8KH_)
	TMcu8KECfg tMcu8KeCfg;
	g_cMcuAgent.GetMcuEqpCfg(&tMcu8KeCfg);
#else
	g_cMcuAgent.GetMacByEthIdx( 0,achMacAddr[0],32 );
	g_cMcuAgent.GetMacByEthIdx( 1,achMacAddr[1],32 );
	g_cMcuAgent.GetMacByEthIdx( 2,achMacAddr[2],32 );
	
#endif
	

	for( u8 byIdx = 0; byIdx < MAXNUM_ETH_INTERFACE; byIdx ++)
	{
#if defined(_8KE_) || defined(_8KH_)
		TNetAdaptInfo *ptNetAdapt = tMcu8KeCfg.m_tLinkNetAdap.GetValue(byIdx); //当前启用的eth，无论0或1均从结点0开始累计
		if( ptNetAdapt != NULL )
		{
			u8 *pbyMacAddr = ptNetAdapt->GetMacAddress();
			if( pbyMacAddr == NULL )
			{
				continue;
			}
			GetMacStrSegmentedbySep(pbyMacAddr, achMacAddr[byIdx], ":");
#endif
			
			StrUpper(achMacAddr[byIdx]);
			printf("Device id from ethIdx.%d: %s\n", byIdx, achMacAddr[byIdx]);
			s32 nCmpRet = 0;
			nCmpRet = strcmp( achMacAddr[byIdx], achGetValue );
			printf("strcmp(%s, %s) result is %d!\n", achMacAddr[byIdx], achGetValue, nCmpRet);
			/*			if( 0 == strcmp( achMacAddr[byIdx], achGetValue ))*/
			if( 0 == nCmpRet )
			{
				bMacMatch = TRUE;
				break;
			}
			else
			{
				memset(achMacAddr[byIdx], 0, sizeof(achMacAddr[byIdx]));
#if defined(_8KE_) || defined(_8KH_)
				GetMacStrSegmentedbySep(pbyMacAddr, achMacAddr[byIdx], "-"); 
#endif
				StrUpper( achMacAddr[byIdx] );	
				nCmpRet = 0;
				nCmpRet = strcmp( achMacAddr[byIdx], achGetValue );
				printf("strcmp(%s, %s) result is %d!\n", achMacAddr[byIdx], achGetValue, nCmpRet);
				if( 0 == nCmpRet )
				{
					bMacMatch = TRUE;
					break;
				}
				else
				{
					continue;
				}
			}
#if defined(_8KE_) || defined(_8KH_)
		}
#endif
	}
	if( !bMacMatch )
	{
		printf("Device id is incorrect for this board!\n");
		g_cMcuVcApp.SetLicenseNum( 0 );
		g_cMcuGuardApp.SetLicenseErrorCode( 5 );
		return FALSE;
	}
#else	
    // guzh [7/3/2007] 分别尝试2种分隔符
    u8 achSequenceStr[64] = {0};    
	GetBoardSequenceNum( achSequenceStr, ":" );
	StrUpper( (s8*)achSequenceStr );	
	if( 0 != strcmp( (s8*)achSequenceStr, achGetValue ) )
	{
        memset(achSequenceStr, 0, sizeof(achSequenceStr));
        GetBoardSequenceNum( achSequenceStr, "-" ); 
        StrUpper( (s8*)achSequenceStr );	
        if( 0 != strcmp( (s8*)achSequenceStr, achGetValue ) )
		{
			printf("Device id is incorrect for this board!\n");
			g_cMcuVcApp.SetLicenseNum( 0 );
			g_cMcuGuardApp.SetLicenseErrorCode( 5 );
			return FALSE;
        }
	}
	//printf("Device id: %s\n",achGetValue);
#endif
	//读MCU类型
	nValuelen = 50;
	memset( achGetValue, 0, sizeof(achGetValue) );
	if( SUCCESS_LICENSE != ( wRet = GetValue(KEY_MCU_TYPE, achGetValue, nValuelen) ) )
	{
		printf("Read Mcu type failed!\n");
		g_cMcuVcApp.SetLicenseNum( 0 );
		g_cMcuGuardApp.SetLicenseErrorCode( 6 );
		return FALSE;
	}
	else
	{

		// 对MCU类型进行校验, zgc, 2007-03-09
		u8 byMcuType = GetMcuPdtType();
		switch( byMcuType ) {
		case MCU_TYPE_KDV8000B:
		case MCU_TYPE_KDV8000B_HD:
			if( 0 != strcmp( "8000B", achGetValue ) )
			{
				printf( "Mcu type(%s) is error!\n", achGetValue);
				g_cMcuVcApp.SetLicenseNum( 0 );
                g_cMcuGuardApp.SetLicenseErrorCode( 6 );
				return FALSE;
			}
			break;
		case MCU_TYPE_KDV8000C:
			if( 0 != strcmp( "8000C", achGetValue ) )
			{
				printf( "Mcu type(%s) is error!\n", achGetValue);
				g_cMcuVcApp.SetLicenseNum( 0 );
                g_cMcuGuardApp.SetLicenseErrorCode( 6 );
				return FALSE;
			}
			break;
        case MCU_TYPE_KDV8000:
            if( 0 != strcmp( "8000", achGetValue ) )
            {
                printf( "Mcu type(%s) is error!\n", achGetValue);
                g_cMcuVcApp.SetLicenseNum( 0 );
                g_cMcuGuardApp.SetLicenseErrorCode( 6 );
                return FALSE;
            }
            break;
		case MCU_TYPE_WIN32:
			break;
		case MCU_TYPE_KDV8000E:		
			if( 0 != strcmp( "8000G", achGetValue ) )
            {
                printf( "Mcu type(%s) is error!\n", achGetValue);
                g_cMcuVcApp.SetLicenseNum( 0 );
                g_cMcuGuardApp.SetLicenseErrorCode( 6 );
                return FALSE;
            }
            break;
		case MCU_TYPE_KDV8000H:
			if( 0 != strcmp( "8000H", achGetValue ) )
            {
                printf( "Mcu type(%s) is error!\n", achGetValue);
                g_cMcuVcApp.SetLicenseNum( 0 );
                g_cMcuGuardApp.SetLicenseErrorCode( 6 );
                return FALSE;
            }
			break;
		case MCU_TYPE_KDV8000I:
			if( 0 != strcmp( "8000I", achGetValue ) )
            {
                printf( "Mcu type(%s) is error!\n", achGetValue);
                g_cMcuVcApp.SetLicenseNum( 0 );
                g_cMcuGuardApp.SetLicenseErrorCode( 6 );
                return FALSE;
            }
			break;
		case MCU_TYPE_KDV800L:
#ifdef _800L_
			g_cMcuAgent.SetMcuType800L();
			OspSetPrompt("mcu_800l");
#endif
			if( 0 != strcmp( "800L", achGetValue ) )
			{
                printf( "Mcu type(%s) is error!\n", achGetValue);
                g_cMcuVcApp.SetLicenseNum( 0 );
                g_cMcuGuardApp.SetLicenseErrorCode( 6 );
                return FALSE;
			}
			break;
		case MCU_TYPE_KDV8000H_M:
#ifdef _8KH_M_
			g_cMcuAgent.SetMcuType8000Hm();
			OspSetPrompt("mcu8000H-M");
#endif
			if( 0 != strcmp("8000H-M",achGetValue))
			{
                printf( "Mcu type(%s) is error!\n", achGetValue);
                g_cMcuVcApp.SetLicenseNum( 0 );
                g_cMcuGuardApp.SetLicenseErrorCode( 6 );
                return FALSE;
			}
			break;
		default:
			printf( "Mcu type(%s) is error!\n", achGetValue);
            g_cMcuGuardApp.SetLicenseErrorCode( 6 );
			g_cMcuVcApp.SetLicenseNum( 0 );
			return FALSE;
		}
	} 
	
	//读授权接入数量
	nValuelen = 50;
	memset( achGetValue, 0, sizeof(achGetValue) );
	if( SUCCESS_LICENSE != ( wRet = GetValue(KEY_MCU_ACCESS_NUM, achGetValue, nValuelen) ) )
	{
		printf("Read Mcu access num failed!\n");
		g_cMcuVcApp.SetLicenseNum( 0 );
		g_cMcuGuardApp.SetLicenseErrorCode( 7 );
		return FALSE;
	}
	g_cMcuVcApp.SetLicenseNum( atoi(achGetValue) );        
    // 不能大于容量值
    if (g_cMcuVcApp.GetLicenseNum() > MAXNUM_MCU_MT)
    {
        g_cMcuVcApp.SetLicenseNum(MAXNUM_MCU_MT);
    }
	//printf("Access num: %s\n",achGetValue);
	
#if defined(_8KE_) || defined(_8KH_)  || defined(_8KI_)// [3/10/2010 xliang] 8000E 额外读1个授权数字段
	//读高清接入数 ( 分基础型和增强型 )
	nValuelen = 50;
	memset( achGetValue, 0, sizeof(achGetValue) );
	if( SUCCESS_LICENSE != ( wRet = GetValue(KEY_MCU_ACCESS_HD_MT_NUM, achGetValue, nValuelen) ) )
	{
		printf("It's enhanced product, set value equal to license num above.\n");
		g_cMcuVcApp.SetLicenseHDMtNum( g_cMcuVcApp.GetLicenseNum() );
	}
	else
	{
		g_cMcuVcApp.SetLicenseHDMtNum( atoi(achGetValue) );        
		// 不能大于容量值
		if (g_cMcuVcApp.GetLicenseHDMtNum() > MAXNUM_MCU_MT)
		{
			g_cMcuVcApp.SetLicenseHDMtNum(MAXNUM_MCU_MT);
		}
	}
#endif	

#if defined(_8KH_)
	if( g_cMcuAgent.Is8000HmMcu() )
	{
		nValuelen = 50;
		if( SUCCESS_LICENSE != ( wRet = GetValue(KEY_MCU_ACCESS_AUDIO_NUM, achGetValue, nValuelen) ) )
		{
			printf("Get MCU AUDMTADP FAILED!\n");
			LogPrint(LOG_LVL_WARNING, MID_PUB_ALWAYS, "Get MCU AUDMTADP FAILED!\n");
		}
		else
		{
			g_cMcuVcApp.SetAudMtLicenseNum(atoi(achGetValue));		
		}
	}
	else
	{
		g_cMcuVcApp.SetAudMtLicenseNum(MAXNUM_MCU_MT);	
	}
#else
	g_cMcuVcApp.SetAudMtLicenseNum(MAXNUM_MCU_MT);	
#endif

	//读pcmt接入数
	nValuelen = 50;
	memset( achGetValue, 0, sizeof(achGetValue) );
	if( SUCCESS_LICENSE != ( wRet = GetValue(KEY_MCU_ACCESS_PCMT_NUM, achGetValue, nValuelen) ) )
	{
		printf("Read Mcu access pcmt num failed!\n");
		g_cMcuVcApp.SetLicensePCMtNum( 0 );
		//[2011/01/28 zhushz]PCmt error add
		g_cMcuGuardApp.SetLicenseErrorCode( 11 );
		//return FALSE;
	}
	g_cMcuVcApp.SetLicensePCMtNum( atoi(achGetValue) );        
    // 不能大于容量值
    if (g_cMcuVcApp.GetLicensePCMtNum() > MAXNUM_MCU_MT)
    {
        g_cMcuVcApp.SetLicensePCMtNum(MAXNUM_MCU_MT);
    }



#if !defined(_8KE_) && !defined(_8KH_) && !defined(_8KI_)
	//读失效日期
	nValuelen = 50;
	memset( achGetValue, 0, sizeof(achGetValue) );
	if( SUCCESS_LICENSE != ( wRet = GetValue(KEY_MCU_EXP_DATE, achGetValue, nValuelen) ) )
	{
		printf("Read Mcu expire time failed!\n");
		g_cMcuVcApp.SetLicenseNum( 0 );
		g_cMcuGuardApp.SetLicenseErrorCode( 8 );
		return FALSE;
	}
	TKdvTime tDate;
	s8 * pchToken = strtok( achGetValue, "-" );
	if( NULL != pchToken)
	{
		tDate.SetYear( atoi( pchToken ) );
		pchToken = strtok( NULL, "-" );
		if( NULL != pchToken )
		{
			tDate.SetMonth( atoi( pchToken ) );
			pchToken = strtok( NULL, "-" );
			if( NULL != pchToken )
			{
				tDate.SetDay( atoi( pchToken ) );
			}
		}
	}
	if( 0 == tDate.GetYear() || 0 == tDate.GetMonth() || 0 == tDate.GetDay() )
	{
		printf("Read Mcu expire time failed!\n");
		g_cMcuVcApp.SetLicenseNum( 0 );
		g_cMcuGuardApp.SetLicenseErrorCode( 8 );
		return FALSE;
	}
    // guzh [5/8/2007] 如果配置超过2030年，认为不限制
    if ( tDate.GetYear() >= 2030 )
    {
        memset( &tDate, 0, sizeof(tDate) );
    }
	g_cMcuVcApp.SetMcuExpireDate(tDate);
#endif
	//读VCS授权接入能力
	nValuelen = 50;
	memset( achGetValue, 0, sizeof(achGetValue) );
	if( SUCCESS_LICENSE != ( wRet = GetValue(KEY_MCU_VCS_ACCESS_NUM, achGetValue, nValuelen) ) )
	{
		printf("Read vcs access num failed!\n");
		g_cMcuVcApp.SetVCSAccessNum( 0 );
		g_cMcuGuardApp.SetLicenseErrorCode( 10 );
		return FALSE;
	}
	g_cMcuVcApp.SetVCSAccessNum(atoi(achGetValue));        
    // 不能大于最大值
    if (g_cMcuVcApp.GetVCSAccessNum() > MAXNUM_MCU_VC)
    {
        g_cMcuVcApp.SetVCSAccessNum(MAXNUM_MCU_VC);
    }
#endif
//#endif
	return TRUE;
}

CMcuGuardData::CMcuGuardData()
{
	m_byLicenseErrorCode = 0;
    memset(m_szLicenseSN, 0, sizeof(m_szLicenseSN));
}
CMcuGuardData::~CMcuGuardData()
{
}
/*=============================================================================
函 数 名： GetLicenseErrorCode
功    能： 获得错误码
算法实现： 
全局变量： 
参    数： 无
返 回 值： u8 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/01/26  4.0			周广程                 创建
=============================================================================*/
u8 CMcuGuardData::GetLicenseErrorCode()
{
	return m_byLicenseErrorCode;
}

/*=============================================================================
函 数 名： SetLicenseErrorCode
功    能： 设置错误码
算法实现： 
全局变量： 
参    数： u8
返 回 值： 无 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/01/26  4.0			周广程                 创建
=============================================================================*/
void CMcuGuardData::SetLicenseErrorCode( u8 byErrorCode )
{
	m_byLicenseErrorCode = byErrorCode;
}

/*=============================================================================
函 数 名： SetLicenseSN
功    能： 设置License序列号
算法实现： 
全局变量： 
参    数： u8
返 回 值： 无 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/06/08  4.0			顾振华                 创建
=============================================================================*/
void CMcuGuardData::SetLicenseSN(const s8* szSN)
{
    strncpy(m_szLicenseSN, szSN, sizeof(m_szLicenseSN)-1);
}

/*=============================================================================
函 数 名： GetLicenseSN
功    能： 获取License序列号
算法实现： 
全局变量： 
参    数： u8
返 回 值： 无 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/06/08  4.0			顾振华                 创建
=============================================================================*/
const s8* CMcuGuardData::GetLicenseSN(void)
{
    return m_szLicenseSN;
}


/*=============================================================================
函 数 名： Init
功    能： 初始化CGuardCheck
算法实现： 
全局变量： 
参    数： u8
返 回 值： true, 成功初始化; false, 初始化失败
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2011/11/14  4.6 	    刘旭                  创建
=============================================================================*/
BOOL32	CGuardCheck::Init( const u8 byMaxUnRsp )
{
	if ( byMaxUnRsp == 0 )
	{
		LogPrint( LOG_LVL_WARNING, MID_MCU_GUARD, "[CGuardCheck::Init] Param invalid\n");
		return FALSE;
	}

	// 先销毁
	Destroy();

	// 创建信号量
	if (!OspSemBCreate(&m_hSem))
	{
		LogPrint( LOG_LVL_WARNING, MID_MCU_GUARD, "[CGuardCheck::Init]Sem create failed\n");
		return FALSE;
	}

	m_byMaxUnRspTimes = byMaxUnRsp;
	m_byUnRspTimes = 0;
	m_byFlag = 0;

	return TRUE;
}


/*=============================================================================
函 数 名： Destroy
功    能： 销毁
算法实现： 
全局变量： 
参    数： 
返 回 值： 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2011/11/14  4.6 	    刘旭                  创建
=============================================================================*/
void CGuardCheck::Destroy()
{
	if (m_hSem)
	{
		OspSemGive(m_hSem);
		OspSemDelete(m_hSem);
		m_hSem = NULL;
	}

	m_byMaxUnRspTimes = 0;
	m_byUnRspTimes = 0;
	m_byFlag = 0;
}


/*=============================================================================
函 数 名： ClientSet
功    能： 被检测线程设置自己的标记位
算法实现： 
全局变量： 
参    数： 
返 回 值： 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2011/11/14  4.6 	    刘旭                  创建
=============================================================================*/
void	CGuardCheck::ClientSet()
{
	if (m_hSem)
	{
		OspSemTake(m_hSem);
	}

	m_byFlag |=  (1 << E_GC_FLAG_CHECK);

	if (m_hSem)
	{
		OspSemGive(m_hSem);
	}
}

/*=============================================================================
函 数 名： GuardCheck
功    能： Guard做检查
算法实现： 
全局变量： 
参    数： 
返 回 值： 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2011/11/14  4.6 	    刘旭                  创建
=============================================================================*/
BOOL32	CGuardCheck::GuardCheck()
{
	if (m_hSem)
	{
		OspSemTake(m_hSem);
	}
	
	BOOL32 bChecked = (m_byFlag & (1 << E_GC_FLAG_CHECK)) ? TRUE : FALSE;
	if (!bChecked)
	{
		StaticLog("GuardCheck: unrsp %d times\n", ++m_byUnRspTimes);
		printf("GuardCheck: unrsp %d times\n", m_byUnRspTimes);
	}else
	{
		m_byUnRspTimes = 0;
	}

	// 重置标志位 
	m_byFlag &=  ~(1 << E_GC_FLAG_CHECK);
	
	if (m_hSem)
	{
		OspSemGive(m_hSem);
	}
	
	return bChecked;
}

API void mcuguarddisable(void)
{
    g_bEnableMcuGuard = FALSE;
}

API void mcuguardenable(void)
{
    g_bEnableMcuGuard = TRUE;
}

API void showlicense(void)
{
    switch( g_cMcuGuardApp.GetLicenseErrorCode() ) 
	{
	case 0:
        {
            StaticLog("License SN(Key)       : %s\n", g_cMcuGuardApp.GetLicenseSN() );
            StaticLog( "Licensed Access Number: %d\n", g_cMcuVcApp.GetLicenseNum() );
            StaticLog( "VCS Access Number: %d\n", g_cMcuVcApp.GetVCSAccessNum() );
			StaticLog( "Aud Access Number: %d\n", g_cMcuVcApp.GetAudMtLicenseNum() );
            TKdvTime tExpireDate = g_cMcuVcApp.GetMcuExpireDate(); 
            StaticLog( "License Expired Date  : %.4d-%.2d-%.2d\n", 
                       tExpireDate.GetYear(), tExpireDate.GetMonth(), tExpireDate.GetDay() );

            break;
        }
	case 1:
		StaticLog( "\nLicense Error: %s\n", READ_LICENSE_FILE_ERROR );
		break;
	case 2:
		StaticLog( "\nLicense Error: %s\n", READ_LICENSE_VERSION_ERROR );
		break;
	case 3:
		StaticLog( "\nLicense Error: %s\n", READ_LICENSE_TYPE_ERROR );
		break;
	case 4:
		StaticLog( "\nLicense Error: %s\n", READ_LICENSE_KEY_ERROR );
		break;
	case 5:
		StaticLog( "\nLicense Error: %s\n", READ_LICENSE_MCUDEVICEID_ERROR );
		break;
	case 6:
		StaticLog( "\nLicense Error: %s\n", READ_LICENSE_MCUTYPE_ERROR );
		break;
	case 7:
		StaticLog( "\nLicense Error: %s\n", READ_LICENSE_MCUACCESSNUM_ERROR );
		break;
	case 8:
		StaticLog( "\nLicense Error: %s\n", READ_LICENSE_MCUEXPIREDATA_ERROR );
		break;
    case 9:
        StaticLog( "\nBoard Error: %s\n",   READ_LICENSE_ILLEGALEBOARD_ERROR );
        break;
	case 10:
        StaticLog( "\nLicense Error: %s\n", READ_LICENSE_VCSACCESSNUM_ERROR );
		break;		
	case 11://[2011/01/28 zhushz]PCmt error add
		StaticLog( "\nLicense Error: %s\n", READ_LICENSE_PCMTACCESSNUM_ERROR );
		break;	
	default:
		StaticLog( "\nLicense Error: %s\n", READ_LICENSE_UNDEFINED_ERROR );
		break;
	}
}

// END OF FILE
