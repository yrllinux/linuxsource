/*****************************************************************************
   模块名      : Mcu Agent
   文件名      : ProcAlarm.cpp
   相关文件    : ProcAlarm.h
   文件实现功能: 告警表的维护
   作者        : liuhuiyun
   版本        : V4.0  Copyright( C) 2006-2008 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   2005/08/17  4.0         liuhuiyun       创建
   2006/04/30  4.0		   liuhuiyun	   优化
******************************************************************************/
//lint -sem(CAlarmProc::Quit,cleanup)
//lint -sem(CAlarmProc::InitAlarmTable,initializer)
/*lint -save -e1565*/
#include "procalarm.h"
#include "configureagent.h"

CAlarmProc g_cAlarmProc;

CAlarmProc::CAlarmProc() : m_tMcuPfm(), m_dwNowAlarmNo(0), m_wErrorNum(0)
{
    m_dwNowAlarmNo = 1;
	m_tMcuPfm.m_wAlarmStamp = 0;
	memset( m_achCfgErr, '\0',sizeof(m_achCfgErr) );
	memset(m_achCfgErr, 0, sizeof(m_achCfgErr));

	InitAlarmTable();
}

CAlarmProc::~CAlarmProc()
{
	Quit();
}

/*=============================================================================
  函 数 名： InitAlarmTable
  功    能： 初始化告警表
  算法实现： 
  全局变量： 
  参    数： void
  返 回 值： void 
=============================================================================*/

void CAlarmProc::InitAlarmTable( void )
{
    memset( m_atAlarmTable, 0, sizeof(m_atAlarmTable) );
    	
	if( !OspSemBCreate(&m_hMcuAlarmTable) )  // 告警表操作信号量
	{
		OspSemDelete( m_hMcuAlarmTable );
		Agtlog(LOG_ERROR, "[Agent] create m_hMcuAlarmTable failed!\n" );
	}
    return;
}

/*=============================================================================
  函 数 名： Quit
  功    能： 
  算法实现： 
  全局变量： 
  参    数： void
  返 回 值： void 
=============================================================================*/
/*lint -save -e1551*/
void CAlarmProc::Quit( void )
{
	if( NULL != m_hMcuAlarmTable )
	{
		OspSemDelete( m_hMcuAlarmTable );
		m_hMcuAlarmTable = NULL;
	}
}

/*=============================================================================
  函 数 名： AddAlarm
  功    能： 增加告警
  算法实现： 
  全局变量： 
  参    数：  u32 dwAlarmCode
             u8 byObjType
             u8 byObject[]
             TMcupfmAlarmEntry * ptData
  返 回 值： BOOL 
=============================================================================*/
BOOL32 CAlarmProc::AddAlarm( u32 dwAlarmCode, u8 byObjType, u8 abyObject[], TMcupfmAlarmEntry * ptData )
{
    ENTER( m_hMcuAlarmTable )

    u32     dwLoop;
    time_t  tCurTime;
    struct tm   *ptTime;

    // 找到一个空表项
    for(dwLoop = 0; dwLoop < MAXNUM_MCU_ALARM; dwLoop++ )
    {
        if( !m_atAlarmTable[dwLoop].m_bExist )
        {
            break;
        }
    }
    
    if( dwLoop >= sizeof( m_atAlarmTable ) / sizeof( m_atAlarmTable[0] ) )
    {
        return FALSE;
    }

	// 填写该告警表项
    m_atAlarmTable[dwLoop].m_dwAlarmCode = dwAlarmCode;
    m_atAlarmTable[dwLoop].m_byObjType = byObjType;
    memcpy( m_atAlarmTable[dwLoop].m_achObject, abyObject, sizeof(m_atAlarmTable[0].m_achObject) );
    
    tCurTime = time( 0 );
    ptTime = localtime( &tCurTime );
    sprintf( m_atAlarmTable[dwLoop].m_achTime, "%4.4u-%2.2u-%2.2u %2.2u:%2.2u:%2.2u", ptTime->tm_year + 1900, 
             ptTime->tm_mon + 1, ptTime->tm_mday, ptTime->tm_hour, ptTime->tm_min, ptTime->tm_sec );
    
    m_atAlarmTable[dwLoop].m_bExist = TRUE;

    // 返回值
    ptData->m_dwSerialNo = dwLoop + 1;  // 从一开始
    ptData->m_dwAlarmCode = m_atAlarmTable[dwLoop].m_dwAlarmCode;
    ptData->m_byObjType = m_atAlarmTable[dwLoop].m_byObjType;
    memcpy( ptData->m_achObject, m_atAlarmTable[dwLoop].m_achObject, sizeof( ptData->m_achObject ) );
    memcpy( ptData->m_achTime, m_atAlarmTable[dwLoop].m_achTime, sizeof(ptData->m_achTime) );
    
    m_dwNowAlarmNo = m_dwNowAlarmNo < (dwLoop+1) ? (dwLoop+1) : m_dwNowAlarmNo;    // 目前的告警号
    m_tMcuPfm.m_wAlarmStamp++;

	Agtlog(LOG_VERBOSE, "[ProcAlarm] m_tMcuPfm.m_wAlarmStamp = %d.\n",m_tMcuPfm.m_wAlarmStamp);
	Agtlog(LOG_VERBOSE, "[ProcAlarm] add alarm wLoop= %d, m_dwNowAlarmNo= %d.\n", dwLoop, m_dwNowAlarmNo);	
	Agtlog(LOG_VERBOSE, "[ProcAlarm] l= %d, s= %d, t= %d.\n", abyObject[0], abyObject[1], abyObject[2]);
    
	return TRUE;
}

/*=============================================================================
  函 数 名： FindAlarm
  功    能： 查找告警
  算法实现： 
  全局变量： 
  参    数：  u32 dwAlarmCode
             u8 byObjType
             u8 byObject[]
             TMcupfmAlarmEntry *ptData
  返 回 值： BOOL 
=============================================================================*/
BOOL32 CAlarmProc::FindAlarm( u32 dwAlarmCode, u8 byObjType, u8 abyObject[], TMcupfmAlarmEntry *ptData )
{
    ENTER( m_hMcuAlarmTable )

    BOOL32    bFind = FALSE;
	u32 dwLoop = 0;
	// 告警表中的每一项
    for( dwLoop = 0; dwLoop < m_dwNowAlarmNo; dwLoop++ )
    {
        if( !m_atAlarmTable[dwLoop].m_bExist )   // 该告警是否存在
        {
            continue;
        }

		// 码和告警对象类型要一致
        if( dwAlarmCode == m_atAlarmTable[dwLoop].m_dwAlarmCode 
            && byObjType == m_atAlarmTable[dwLoop].m_byObjType )
        {
			// 对象要一致
            switch( byObjType )
            {
            case ALARMOBJ_MCU_BOARD:
                if( abyObject[0] == m_atAlarmTable[dwLoop].m_achObject[0] &&
				    abyObject[1] == m_atAlarmTable[dwLoop].m_achObject[1] &&
				    abyObject[2] == m_atAlarmTable[dwLoop].m_achObject[2] )
                {
                    bFind = TRUE;
                }
                break;
            case ALARMOBJ_MCU_LINK:
                if( abyObject[0] == m_atAlarmTable[dwLoop].m_achObject[0] &&
                    abyObject[1] == m_atAlarmTable[dwLoop].m_achObject[1] &&
					abyObject[2] == m_atAlarmTable[dwLoop].m_achObject[2] &&
					abyObject[3] == m_atAlarmTable[dwLoop].m_achObject[3] )
                {
                    bFind = TRUE;
                }
                break;
            case ALARMOBJ_MCU:
			case ALARMOBJ_MCU_SDH:
                bFind = TRUE;
                break;

			case ALARMOBJ_MCU_SOFTWARE:
				if( abyObject[0] == m_atAlarmTable[dwLoop].m_achObject[0] &&
                    abyObject[1] == m_atAlarmTable[dwLoop].m_achObject[1] &&
					abyObject[2] == m_atAlarmTable[dwLoop].m_achObject[2] )
				{
					bFind = TRUE;
				}
				break;

			case ALARMOBJ_MCU_TASK:
				if( abyObject[0] == m_atAlarmTable[dwLoop].m_achObject[0] &&
                    abyObject[1] == m_atAlarmTable[dwLoop].m_achObject[1] &&
					abyObject[2] == m_atAlarmTable[dwLoop].m_achObject[2] &&
					*(u16*)&abyObject[3] == *(u16*)&m_atAlarmTable[dwLoop].m_achObject[3] )
				{
					bFind = TRUE;
				}
				break;

			case ALARMOBJ_MCU_POWER:
				if( abyObject[0] == m_atAlarmTable[dwLoop].m_achObject[0] &&
					abyObject[1] == m_atAlarmTable[dwLoop].m_achObject[1] )
				{
					bFind = TRUE;
				}
				break;

			case ALARMOBJ_MCU_MODULE:
				if( abyObject[0] == m_atAlarmTable[dwLoop].m_achObject[0] && 
					abyObject[1] == m_atAlarmTable[dwLoop].m_achObject[1] &&
					abyObject[2] == m_atAlarmTable[dwLoop].m_achObject[2] &&
					abyObject[3] == m_atAlarmTable[dwLoop].m_achObject[3] )
				{
					bFind = TRUE;
				}
				break;
				
			case ALARMOBJ_MCU_BRD_FAN:
				if( abyObject[0] == m_atAlarmTable[dwLoop].m_achObject[0] &&
					abyObject[1] == m_atAlarmTable[dwLoop].m_achObject[1] &&
					abyObject[2] == m_atAlarmTable[dwLoop].m_achObject[2] &&
					abyObject[3] == m_atAlarmTable[dwLoop].m_achObject[3] )
				{
					bFind = TRUE;
				}
				break;
			
			// [2010/12/15 miaoqingsong add] NMS5.0新功能添加
			case ALARMOBJ_MCU_BOX_FAN:                                               
				if ( abyObject[0] == m_atAlarmTable[dwLoop].m_achObject[0] &&
					 abyObject[1] == m_atAlarmTable[dwLoop].m_achObject[1] )
				{
					bFind = TRUE;
				}
				break;

			case ALARMOBJ_MCU_MPC_CPU:                                               
				if ( abyObject[0] == m_atAlarmTable[dwLoop].m_achObject[0] )
				{
					bFind = TRUE;
				}
				break;

			case ALARMOBJ_MCU_MPC_MEMORY:                                            
				if ( abyObject[0] == m_atAlarmTable[dwLoop].m_achObject[0] )
				{
					bFind = TRUE;
				}
				break;

			case ALARMOBJ_MCU_POWER_TEMP:                                             
				if ( abyObject[0] == m_atAlarmTable[dwLoop].m_achObject[0] )
				{
					bFind = TRUE;
				}
				break;
				
			case ALARMOBJ_MCU_MPC_TEMP:                                               
				if ( abyObject[0] == m_atAlarmTable[dwLoop].m_achObject[0] )
				{
					bFind = TRUE;
				}
				break;
			// [2010/12/15 miaoqingsong add end]
			case ALARMOBJ_MCU_CPU_FAN:
				if ( abyObject[0] == m_atAlarmTable[dwLoop].m_achObject[0] )
				{
					bFind = TRUE;
				}
				break;

            default:
                Agtlog( LOG_VERBOSE, "[FindAlarm]: FindAlarm(): Wrong AlarmObjType!\n");
                break;
            }
        
            if( bFind )
            {
                break;
            }
        }   // end if
    }

    if( bFind )
    {
        // 返回值
        ptData->m_dwSerialNo = dwLoop + 1;
        ptData->m_dwAlarmCode = m_atAlarmTable[dwLoop].m_dwAlarmCode;
        ptData->m_byObjType = m_atAlarmTable[dwLoop].m_byObjType;

		//lukunpeng [2/26/2010]
		//如果找到了，应该更新此时的Object,因为Object中可能有设备的State,不更新的话，会出现发给nms的state错误
		memcpy( m_atAlarmTable[dwLoop].m_achObject, abyObject, sizeof(m_atAlarmTable[dwLoop].m_achObject) );
        memcpy( ptData->m_achObject, 
                m_atAlarmTable[dwLoop].m_achObject, sizeof( ptData->m_achObject ) );

        time_t tCurTime = time( 0 );
		struct tm   *ptTime = localtime( &tCurTime );
		sprintf( ptData->m_achTime, "%4.4u-%2.2u-%2.2u %2.2u:%2.2u:%2.2u", ptTime->tm_year + 1900, 
					ptTime->tm_mon + 1, ptTime->tm_mday, ptTime->tm_hour, ptTime->tm_min, ptTime->tm_sec );

    }
    return bFind;
}

/*=============================================================================
  函 数 名： DeleteAlarm
  功    能： 删除告警
  算法实现： 
  全局变量： 
  参    数： u32 dwSerialNo
  返 回 值： BOOL 
=============================================================================*/
BOOL32 CAlarmProc::DeleteAlarm( u32 dwSerialNo )
{
    ENTER( m_hMcuAlarmTable )
	
    if( dwSerialNo > m_dwNowAlarmNo )
    {
        Agtlog(LOG_ERROR, "[DeleteAlarm] The Alarm No is no exist\n");
        return FALSE;
    }
	
    if( 0 != dwSerialNo)    // 数组越界
    {
        m_atAlarmTable[dwSerialNo - 1].m_bExist = FALSE;
    }
    else
    {
        m_atAlarmTable[dwSerialNo].m_bExist = FALSE;
    }
    
    if( dwSerialNo == m_dwNowAlarmNo && m_dwNowAlarmNo != 0 )     // 最后一个
    {
        m_dwNowAlarmNo--;   // 减少一个
        if(0 == m_dwNowAlarmNo)
        {
            m_dwNowAlarmNo = 1; // 从一开始
        }
    }

    // 告警戳，增加、删除告警时加1 
    m_tMcuPfm.m_wAlarmStamp++;

    return TRUE;
}
/*=============================================================================
  函 数 名： GetAlarmStamp
  功    能： 取告警戳
  算法实现： 
  全局变量： 
  参    数： u16& wAlarmStamp
  返 回 值： BOOL32 
=============================================================================*/
BOOL32 CAlarmProc::GetAlarmStamp(u16& wAlarmStamp)
{
    wAlarmStamp = m_tMcuPfm.m_wAlarmStamp;
    return TRUE;
}

/*=============================================================================
  函 数 名： GetAlarmTableIndex
  功    能： 取告警表信息
  算法实现： 
  全局变量： 
  参    数： u16 wTableIndex
  返 回 值： BOOL32 
=============================================================================*/
BOOL32 CAlarmProc::GetAlarmTableIndex(u16 wTableIndex, TMcupfmAlarmEntry& tAlarmEntry)
{
    BOOL32 bRet = TRUE;

    if ( wTableIndex >= MAXNUM_MCU_ALARM )
    {
        Agtlog( LOG_ERROR, "[GetAlarmTableIndex] param err: wIndex = %d !\n", wTableIndex );
        return FALSE;
    }
    Agtlog(LOG_VERBOSE, "[GetAlarmTableIndex] wTableIndex= %d, m_dwNowAlarmNo= %d, m_bExist= %d\n", 
                        wTableIndex, m_dwNowAlarmNo, m_atAlarmTable[wTableIndex].m_bExist);

	// 扫描告警表
	if( wTableIndex < m_dwNowAlarmNo && TRUE == m_atAlarmTable[wTableIndex].m_bExist )
	{
        tAlarmEntry.m_dwSerialNo = wTableIndex;
        tAlarmEntry.m_byObjType = m_atAlarmTable[wTableIndex].m_byObjType;
        tAlarmEntry.m_dwAlarmCode = m_atAlarmTable[wTableIndex].m_dwAlarmCode;
        memcpy( tAlarmEntry.m_achTime, m_atAlarmTable[wTableIndex].m_achTime, sizeof(tAlarmEntry.m_achTime));
        memcpy( tAlarmEntry.m_achObject, m_atAlarmTable[wTableIndex].m_achObject, sizeof(tAlarmEntry.m_achObject) );
	}
    else
    {
        bRet = FALSE;
        Agtlog(LOG_VERBOSE, "Get Alarm Info Fail.\n");
    }

    return bRet;
}

/*=============================================================================
  函 数 名： GetErrorString
  功    能： 取错误信息
  算法实现： 
  全局变量： 
  参    数： s8* pszErrStr
             u16 wLen
  返 回 值： void 
=============================================================================*/
BOOL32 CAlarmProc::GetErrorString(s8* pszErrStr, u16 wLen)
{
    if( 0 == m_wErrorNum )
	{
        Agtlog(LOG_WARN, "[GetErrorString] m_wErrorNum == 0, no error string !\n");
		return FALSE;
	}
	
	u16 wLength = 0;
    u16 wSrcLen = wLen; // 保留原有值

    s8  aszErrStr[MAX_ERROR_NUM * MAX_ERROR_STR_LEN];
    memset(aszErrStr, '\0', sizeof(aszErrStr));

    for(u8 byLop = 0; byLop < m_wErrorNum; byLop++)
    {
        wLength = strlen(m_achCfgErr[byLop]);
        if(wLen < wLength)
        {
            return FALSE;
        }
        wLen = wLen - wLength;
        sprintf(aszErrStr, "%s\n%s", aszErrStr, m_achCfgErr[byLop]);
    }
	
	if( 0 != m_wErrorNum )
    {
		strncpy(pszErrStr, aszErrStr, wSrcLen);
	}
    return TRUE;
}
/*=============================================================================
  函 数 名： ProcReadError
  功    能： 读配置文件错误
  算法实现： 
  全局变量： 
  参    数： u8 byErrorId
  返 回 值： void 
=============================================================================*/
void CAlarmProc::ProcReadError(u8 byErrorId)
{
	if( MAX_ERROR_NUM < m_wErrorNum ) // 越界保护
	{
		m_wErrorNum = 0; 
	}

    switch( byErrorId )
	{
    case ERR_AGENT_READSYSCFG:
        strncpy(m_achCfgErr[m_wErrorNum], ERR_STRING_SYS, strlen(ERR_STRING_SYS)+1);
        m_wErrorNum++;
        break;

    case ERR_AGENT_READNETCFG:
        strncpy(m_achCfgErr[m_wErrorNum], ERR_STRING_NETWORK, strlen(ERR_STRING_NETWORK)+1);
        m_wErrorNum++;
        break;

    case ERR_AGENT_READLOCALCFG:
        strncpy(m_achCfgErr[m_wErrorNum], ERR_STRING_LOACL, strlen(ERR_STRING_LOACL)+1);
        m_wErrorNum++;
        break;

    case ERR_AGENT_READTRAPCFG:
        strncpy(m_achCfgErr[m_wErrorNum], ERR_STRING_TRAP, strlen(ERR_STRING_TRAP)+1);
        m_wErrorNum++;
        break;

    case ERR_AGENT_READBOARDCFG:
        strncpy(m_achCfgErr[m_wErrorNum], ERR_STRING_BOARD, strlen(ERR_STRING_BOARD)+1);
        m_wErrorNum++;
        break;

    case ERR_AGENT_READMIXCFG:
        strncpy(m_achCfgErr[m_wErrorNum], ERR_STRING_MIX, strlen(ERR_STRING_MIX)+1);
        m_wErrorNum++;
        break;

    case ERR_AGENT_READTVCFG:
        strncpy(m_achCfgErr[m_wErrorNum], ERR_STRING_TVWALL, strlen(ERR_STRING_TVWALL)+1);
        m_wErrorNum++;
        break;
    case ERR_AGENT_READRECORDERCFG:
        strncpy(m_achCfgErr[m_wErrorNum], ERR_STRING_RECORDER, strlen(ERR_STRING_RECORDER)+1);
        m_wErrorNum++;
        break;

    case ERR_AGENT_READBASCFG:
        strncpy(m_achCfgErr[m_wErrorNum], ERR_STRING_BAS, strlen(ERR_STRING_BAS)+1);
        m_wErrorNum++;
        break;

    case ERR_AGENT_READBASHDCFG:
        strncpy(m_achCfgErr[m_wErrorNum], ERR_STRING_BASHD, strlen(ERR_STRING_BASHD)+1);
        m_wErrorNum++;
        break;

    case ERR_AGENT_READVMPCFG:
        strncpy(m_achCfgErr[m_wErrorNum], ERR_STRING_VMP, strlen(ERR_STRING_VMP)+1);
        m_wErrorNum++;
        break;

    case ERR_AGENT_READMPWCFG:
        strncpy(m_achCfgErr[m_wErrorNum], ERR_STRING_MPW, strlen(ERR_STRING_MPW)+1);
        m_wErrorNum++;
        break;

    case ERR_AGENT_READVMPATTACHCFG:
        strncpy(m_achCfgErr[m_wErrorNum], ERR_STRING_VMPATTACH, strlen(ERR_STRING_VMPATTACH)+1);
        m_wErrorNum++;
        break;

    case ERR_AGENT_READPRSCFG:
        strncpy(m_achCfgErr[m_wErrorNum], ERR_STRING_PRS, strlen(ERR_STRING_PRS)+1);
        m_wErrorNum++;
        break;

    case ERR_AGENT_READQOSCFG:
        strncpy(m_achCfgErr[m_wErrorNum], ERR_STRING_QOS, strlen(ERR_STRING_QOS)+1);
        m_wErrorNum++;
        break;

    case ERR_AGENT_READNETSYSCCFG:
        strncpy(m_achCfgErr[m_wErrorNum], ERR_STRING_NETSYSC, strlen(ERR_STRING_NETSYSC)+1);
        m_wErrorNum++;
        break;

    case ERR_AGENT_READDATACONFCFG:
        strncpy(m_achCfgErr[m_wErrorNum], ERR_STRING_DATACONF, strlen(ERR_STRING_DATACONF)+1);
        m_wErrorNum++;
        break;
        
    default:
        break;
    }
    return;
}

/*=============================================================================
  函 数 名： ShowErrorString
  功    能： 
  算法实现： 
  全局变量： 
  参    数： void
  返 回 值： void 
=============================================================================*/
void CAlarmProc::ShowErrorString(void)
{
	OspPrintf(TRUE, FALSE, "\t\t The Error String\n");
    for(u8 byLoop = 0; byLoop < MAX_ERROR_NUM; byLoop++)
    {
        OspPrintf(TRUE, FALSE, "[Agent] %s\n", m_achCfgErr[byLoop]);
    }
	return;
}

/*=============================================================================
  函 数 名： ShowCurAlarm
  功    能： 显示当前告警
  算法实现： 
  全局变量： 
  参    数： void
  返 回 值： void 
=============================================================================*/
void CAlarmProc::ShowCurAlarm( void )
{
	OspPrintf(TRUE, FALSE, "[ShowCurAlarm] m_tMcuPfm.m_wAlarmStamp = %d.\n", m_tMcuPfm.m_wAlarmStamp);
	OspPrintf(TRUE, FALSE, "[ShowCurAlarm] m_dwNowAlarmNo = %d.\n", m_dwNowAlarmNo );
	
	for( u8 byLoop = 0; byLoop < m_dwNowAlarmNo; byLoop++ )
	{
		if( m_atAlarmTable[byLoop].m_bExist )
		{
			OspPrintf(TRUE, FALSE, "No= %d, Type= %d, Code= %d, Object= %d,%d,%d, time= %s\n", 
                                    byLoop, 
			                        m_atAlarmTable[byLoop].m_byObjType,
			                        m_atAlarmTable[byLoop].m_dwAlarmCode,
			                        m_atAlarmTable[byLoop].m_achObject[0],
			                        m_atAlarmTable[byLoop].m_achObject[1],
			                        m_atAlarmTable[byLoop].m_achObject[2],
			                        m_atAlarmTable[byLoop].m_achTime);
		}
	}
    return;
}

/*=============================================================================
  函 数 名： ShowSemHandle
  功    能： 显示信号量值
  算法实现： 
  全局变量： 
  参    数： void
  返 回 值： void 
=============================================================================*/
void CAlarmProc::ShowSemHandle(void)
{
    OspPrintf( TRUE, FALSE, "Brd Manager Handle: 0x%x \n", m_hMcuAlarmTable );
    return;
}
/*lint -restore*/
// END OF FILE
