/*****************************************************************************
   模块名      : MP
   文件名      : mpdata.cpp
   相关文件    : mp.h
   文件实现功能: MP的全局数据实现
   作者        : 胡昌威
   版本        : V4.0  Copyright(C) 2006-2008 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期       版本        修改人          修改内容
   2007/09/24   4.0         张宝卿          创建
******************************************************************************/
#include "mp.h"

CMpApp g_cMpApp;

// 构造
CMpData::CMpData() : m_nInitUdpHandle(0),
					 m_dwMcuNode(0),
					 m_dwMcuIId(0),
                     m_dwMcuNodeB(0),           
                     m_dwMcuIIdB(0),
                     m_dwMcuIpAddr(0),
					 m_wMcuPort(0),
                     m_dwMcuIpAddrB(0),                    
                     m_wMcuPortB(0),
                     m_bEmbedA(FALSE),
                     m_bEmbedB(FALSE),
					 m_dwMpcSSrc(0),
                     m_byAttachMode(0),
                     m_byMpId(0),
                     m_byMcuId(0),
                     m_dwIpAddr(0),
                     m_byRegAckNum(0),
                     m_byConnectNum(0),
                     m_bIsRestrictFlux(FALSE),
                     m_bIsSendOverrunNtf(TRUE),
                     m_bIsWaitingIFrame(FALSE),
					 m_byWIFrameSeqInt(SWITCH_SRC_SEQ_INTERVAL),
                     m_bCancelWaitIFrameNoUni(FALSE),
					 m_bySeqNumRangeLow(MIN_SEQNUM_INTERVAL),
					 m_bySeqNumRangeUp(MAX_SEQNUM_INTERVAL),
					 m_bySensSeqNum(SWITCH_SRC_SEQ_INTERVAL),
					 m_byRtcpBackTimerInterval(RTCPBACK_INTERVAL),
                     m_dwPinHoleInterval(5)	 
					 
                     
{
    memset(m_abyConfMode, 0, sizeof(m_abyConfMode));
    memset(m_byMtNumNeedT, 0, sizeof(m_byMtNumNeedT));
    memset(m_abyMpAlias, 0, sizeof(m_abyMpAlias));
    memset(m_awChgDelayTick, 0, sizeof(m_awChgDelayTick));
    memset(m_abyPtState, 0, sizeof(m_abyPtState));
	memset(m_achV6Ip, 0, sizeof(m_achV6Ip));
	memset((void*)&m_tMtIpMapTab,0, sizeof(m_tMtIpMapTab));
	memset(m_atPinHoleTable, 0, sizeof(m_atPinHoleTable));
}

// 析构
CMpData::~CMpData()
{
    if(NULL != m_pptFilterParam)
    {
        delete[] m_pptFilterParam;
        m_pptFilterParam = NULL;
    }

    if(NULL != m_pptFilterData)
    {
        delete[] m_pptFilterData;
        m_pptFilterData = NULL;
    }

    if(NULL != m_pptWaitIFrmData)
    {
        delete[] m_pptWaitIFrmData;
        m_pptWaitIFrmData = NULL;
    }

    if( NULL != m_pptSwitchChannel )
    {
        delete[] m_pptSwitchChannel;
        m_pptSwitchChannel = NULL;
    }

    if( NULL != m_hPtInfo )
    {
        OspSemDelete( m_hPtInfo );
        m_hPtInfo = NULL;
    }

    if( NULL != m_hPtTick )
    {
        OspSemDelete( m_hPtTick );
        m_hPtTick = NULL;
    }
    
    if( NULL != m_hMtNum )
    {
        OspSemDelete( m_hMtNum );
        m_hMtNum = NULL;
    }
}

/*=============================================================================
  函 数 名： Init
  功    能： 归一重整参数信息保存表内存分配
  算法实现： 
  全局变量： 
  参    数： void
  返 回 值： void
  ----------------------------------------------------------------------
  修改记录    ：
  日  期		版本		修改人		走读人    修改内容
  2007/04/18  4.0			张宝卿                增加对中转点的信号量保护
=============================================================================*/
BOOL32 CMpData::Init()
{
	printf("[Init] new TSwitchChannelV6!\n");
    m_pptSwitchChannel = new TSwitchChannelV6[MAX_CONFIDX][MAX_SWITCH_CHANNEL];
    if( NULL == m_pptSwitchChannel )  
    {
        MpLog( LOG_CRIT, "[Init] allocate memory for SwitchChnnl failed !\n");
        return FALSE;
    }
	printf("[Init] new TSwitchChannelV6 success!\n");
    ClearSwitchTable( TRUE );

	printf("[Init] new TSendFilterParam!\n");
    m_pptFilterParam = new TSendFilterParam[MAX_CONFIDX][MAX_SWITCH_CHANNEL];
    if( NULL == m_pptFilterParam )  return FALSE;
    
	printf("[Init] new TSendFilterAppData!\n");
    m_pptFilterData = new TSendFilterAppData[MAX_CONFIDX][MAX_SWITCH_CHANNEL];
    if( NULL == m_pptFilterData ) return FALSE;

	printf("[Init] new TWaitIFrameData!\n");
    m_pptWaitIFrmData = new TWaitIFrameData[MAX_CONFIDX][MAXNUM_CONF_MT+POINT_NUM];
    if( NULL == m_pptWaitIFrmData ) return FALSE;


    BOOL32 bRet = TRUE;
    bRet = OspSemBCreate( &m_hPtInfo );
    if ( !bRet ) 
    {
        OspSemDelete( m_hPtInfo );
        m_hPtInfo = NULL;
        MpLog( LOG_CRIT, "[Init] create semhandle for PtInfo failed !\n" );
		printf("[Init]create semhandle for PtInfo failed !\n");
        return FALSE;
    }

    bRet = OspSemBCreate( &m_hPtTick );
    if ( !bRet ) 
    {
        OspSemDelete( m_hPtTick );
        m_hPtTick = NULL;
        MpLog( LOG_CRIT, "[Init] create semhandle for PtTick failed !\n" );
		printf("[Init] create semhandle for PtTick failed !\n");
        return FALSE;
    }

    bRet = OspSemBCreate( &m_hMtNum );
    if ( !bRet ) 
    {
        OspSemDelete( m_hMtNum );
        m_hMtNum = NULL;
        MpLog( LOG_CRIT, "[Init] create semhandle for MtNum failed !\n" );
		printf("[Init] create semhandle for MtNum failed  !\n");
        return FALSE;
    }
    
    return TRUE;
}

/*=============================================================================
  函 数 名： FreeStatusA
  功    能： 清空A板状态
  算法实现： 
  全局变量： 
  参    数： void
  返 回 值： void 
=============================================================================*/
void CMpData::FreeStatusA(void)
{
    m_dwMcuNode = INVALID_NODE;
    m_dwMcuIId  = INVALID_INS;
    return;
}

/*=============================================================================
  函 数 名： FreeStatusB
  功    能： 清空B板状态
  算法实现： 
  全局变量： 
  参    数： void
  返 回 值： void 
=============================================================================*/
void CMpData::FreeStatusB(void)
{
    m_dwMcuNodeB = INVALID_NODE;
    m_dwMcuIIdB  = INVALID_INS;
    return;
}

/*=============================================================================
  函 数 名： CreateDS
  功    能： 初始化DataSwitch
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： BOOL32 
=============================================================================*/
BOOL32 CMpData::CreateDS(u32 dwMpIp)
{
#if defined(_8KE_) || defined(_8KH_) || defined(_8KI_)
	if( DSOK != dsSetCapacity( 512,512 ) )
	{
		printf("[CreateDS] dsSetCapacity 512 failed !\n");
	}
#endif
	
    //u32 adwIpAddr[] = { htonl(m_dwIpAddr), 0 };
	TDSNetAddr atDsNetAddr[2];
	u32 byIpNum = 0;
	
	 
	s8 achRevIp[18] = {0};
	strofip(dwMpIp, achRevIp); 
	OspPrintf(1, 0, "[CreateDS] IpV4.%s!\n", achRevIp);
	printf("[CreateDS] IpV4.%s!\n", achRevIp);
	atDsNetAddr[0].SetIPStr(DS_TYPE_IPV4, achRevIp);
	byIpNum++;

	if (0 != strlen(m_achV6Ip))
	{
		OspPrintf(1, 0, "[CreateDS] IpV6.%x!\n", m_achV6Ip);
		printf("[CreateDS] IpV6.%s!\n", m_achV6Ip);
		atDsNetAddr[1].SetIPStr(DS_TYPE_IPV6, m_achV6Ip);
		byIpNum++;
	}
	
    m_nInitUdpHandle = dsCreate(byIpNum, atDsNetAddr);
    

#ifndef WIN32    

    if(INVALID_DSID == m_nInitUdpHandle)
    {
        MpLog( LOG_CRIT, "[CreateDS] Call dsCreate Failure !\n");
        return FALSE;
    }
#endif
    return TRUE;
}

/*=============================================================================
  函 数 名： DestroyDS
  功    能： 销毁dataswitch
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void 
=============================================================================*/
void CMpData::DestroyDS()
{
    dsDestroy(m_nInitUdpHandle);
    return;
}


/*=============================================================================
  函 数 名： MpSendFilterParamInit
  功    能： 
  算法实现： 
  全局变量： 
  参    数： void
  返 回 值： void 
  ----------------------------------------------------------------------
  修改记录    ：
  日  期		版本		修改人		走读人    修改内容
  2007/04/18    4.0			张宝卿                代码移植
=============================================================================*/
void CMpData::MpSendFilterParamInit(void)
{
    if( NULL == m_pptFilterParam   || 
        NULL == m_pptFilterData ||
        NULL == m_pptWaitIFrmData )
    {
        MpLog( LOG_CRIT, "[MpSendFilterParamInit] memory allocate failed !\n");
        return;
    }

    u8 byConfIdx = 0;
    for( ; byConfIdx < MAX_CONFIDX; byConfIdx++ )
    {
        MpResetSendFilterParam( byConfIdx );
    }
    return;
}

/*=============================================================================
  函 数 名： MpResetSendFilterParam
  功    能： 
  算法实现： 
  全局变量： 
  参    数： u8 byConfNo
  返 回 值： void 
  ----------------------------------------------------------------------
  修改记录    ：
  日  期		版本		修改人		走读人    修改内容
  2007/04/18    4.0			张宝卿                代码移植
=============================================================================*/
void CMpData::MpResetSendFilterParam(u8 byConfNo)
{
    if( NULL == m_pptFilterParam   || 
        NULL == m_pptFilterData ||
        NULL == m_pptWaitIFrmData )
    {
        MpLog( LOG_CRIT, "[MpResetSendFilterParam] memory allocate failed !\n");
        return;
    }
    
    for(u16 wChannelIdx = 0; wChannelIdx < MAX_SWITCH_CHANNEL; wChannelIdx++)
    {
        m_pptFilterParam[byConfNo][wChannelIdx].nSeqNumInterval     = 0;
        m_pptFilterParam[byConfNo][wChannelIdx].nTimeStampInterval  = 0;
        m_pptFilterParam[byConfNo][wChannelIdx].wModifySeqNum       = 0;
        m_pptFilterParam[byConfNo][wChannelIdx].dwModifyTimeStamp   = 0;
        m_pptFilterParam[byConfNo][wChannelIdx].dwModifySSRC        = 0;
        m_pptFilterParam[byConfNo][wChannelIdx].wLastSeqNum         = 0;
        m_pptFilterParam[byConfNo][wChannelIdx].dwLastTimeStamp     = 0;
        m_pptFilterParam[byConfNo][wChannelIdx].dwLastSSRC          = 0;

        m_pptFilterData[byConfNo][wChannelIdx].byConfNo   = 0xFF;
        m_pptFilterData[byConfNo][wChannelIdx].wChannelNo = 0xFFFF;
		memset(m_pptFilterData[byConfNo][wChannelIdx].achDstIp, 0, sizeof(m_pptFilterData[byConfNo][wChannelIdx].achDstIp));

        m_pptFilterData[byConfNo][wChannelIdx].wDstPort   = 0;
        m_pptFilterData[byConfNo][wChannelIdx].dwAddTime  = 0;
        m_pptFilterData[byConfNo][wChannelIdx].bRemoved   = FALSE;
        m_pptFilterData[byConfNo][wChannelIdx].bUniform   = FALSE;
        m_pptFilterData[byConfNo][wChannelIdx].byPayload  = INVALID_PAYLOAD;
    }

    for(u16 wMtIdx = 0; wMtIdx < MAXNUM_CONF_MT+POINT_NUM; wMtIdx++)
    {
        m_pptWaitIFrmData[byConfNo][wMtIdx].byConfNo = 0xff;
        m_pptWaitIFrmData[byConfNo][wMtIdx].byMtId   = 0xff;
		memset(m_pptWaitIFrmData[byConfNo][wMtIdx].achSrcIp, 0, sizeof(m_pptWaitIFrmData[byConfNo][wMtIdx].achSrcIp));

        m_pptWaitIFrmData[byConfNo][wMtIdx].wSrcPort = 0;
        m_pptWaitIFrmData[byConfNo][wMtIdx].bWaitingIFrame = FALSE;
        m_pptWaitIFrmData[byConfNo][wMtIdx].dwFstTimeStamp = 0;
    }
}


/*=============================================================================
  函 数 名： AddSwitchTable
  功    能： 增加交换表
  算法实现： 
  全局变量： 
  参    数： u8              byConfIdx
             TSwitchChannel* ptSwitchChannel
             u8              byType：1.Add 1 to 1 switch, 2.Add Many to 1 switch
  返 回 值： BOOL32 
  ----------------------------------------------------------------------
  修改记录    ：
  日  期		版本		修改人		走读人    修改内容
  2007/04/20    4.0			张宝卿                代码移植
  2007/07/19    4.0         张宝卿                表项增加策略 修正
  2012/05/12	4.7			周嘉麟				  交换表V6支持
=============================================================================*/
BOOL32 CMpData::AddSwitchTable( u8 byConfIdx, TSwitchChannelV6* ptSwitchChannel, u8 byType )
{
	if (NULL == ptSwitchChannel)
	{
		OspPrintf(TRUE, FALSE, "[AddSwitchTable] ptSwitchChannel is null!\n");
		return FALSE;
	}

    TSwitchChannelV6 tSwitchChannel = *ptSwitchChannel;

	s8* pchRcvIp = tSwitchChannel.GetRcvIp();
	s8* pchDstIp = tSwitchChannel.GetDstIp();

	if (NULL == pchRcvIp || NULL == pchDstIp)
	{
		return FALSE;
	}

    u16 wRcvPort = tSwitchChannel.GetRcvPort();
	u16 wDstPort = tSwitchChannel.GetDstPort();

    u16 wIdx = 0;
    u16 wFirstNullIdx = INVALID_SWITCH_CHANNEL;

    if( byConfIdx >= MAX_CONFIDX )
    {
        MpLog( LOG_CRIT, "[AddSwitchTable] ConfIdx.%d error !\n", byConfIdx + 1 );
        return FALSE;
    }

    if( NULL == m_pptSwitchChannel )
    {
        MpLog( LOG_CRIT, "[AddSwitchTable] err: NULL == m_pptSwitchChannel\n" );
        return FALSE;
    }

    BOOL32 bIsExist = FALSE;
	u16 wTableIdx = 0;
    if( SWITCH_ONE2ONE == byType )
    {
        while( wTableIdx < MAX_SWITCH_CHANNEL )
        {
			u32 dwDstIpLen = strlen(pchDstIp);
			u32 dwLen      = strlen(m_pptSwitchChannel[byConfIdx][wTableIdx].GetDstIp());
            if(0 == memcmp(m_pptSwitchChannel[byConfIdx][wTableIdx].GetDstIp(), 
							pchDstIp, max(dwLen, dwDstIpLen)) &&  												
				m_pptSwitchChannel[byConfIdx][wTableIdx].GetDstPort() == wDstPort )
            {
                wIdx = wTableIdx;
                bIsExist = TRUE;
                break;
            }
            if( 0 == strlen(m_pptSwitchChannel[byConfIdx][wTableIdx].GetDstIp()) )
            {
                // zbq [07/19/2007] One2One的交换从249依次往下，排大头
                wFirstNullIdx = wTableIdx;
            }
            wTableIdx++;
        }
    }
    else
    {
        while( wTableIdx < MAX_SWITCH_CHANNEL )
        {
			u32 dwRcvIpLen    = strlen(pchRcvIp);
			u32 dwTabRcvIpLen = strlen(m_pptSwitchChannel[byConfIdx][wTableIdx].GetRcvIp());

			u32 dwDstIpLen    = strlen(pchDstIp);
			u32 dwTabDstIpLen = strlen(m_pptSwitchChannel[byConfIdx][wTableIdx].GetDstIp());

            //zbq [07/19/2007] Mul2One交换的增加也需要过滤重复
            if (0 == memcmp(m_pptSwitchChannel[byConfIdx][wTableIdx].GetRcvIp(), pchRcvIp, 
							max(dwTabRcvIpLen, dwRcvIpLen)) &&  
                 m_pptSwitchChannel[byConfIdx][wTableIdx].GetRcvPort() == wRcvPort &&

				0 == memcmp(m_pptSwitchChannel[byConfIdx][wTableIdx].GetDstIp(), pchDstIp, 
					    	max(dwTabDstIpLen, dwDstIpLen)) && 
                 m_pptSwitchChannel[byConfIdx][wTableIdx].GetDstPort() == wDstPort )
            {
                wIdx = wTableIdx;
                bIsExist = TRUE;
                break;                
            }
            if( 0 == strlen(m_pptSwitchChannel[byConfIdx][wTableIdx].GetDstIp())) // 找一个空位置
            {
                //zbq [07/19/2007] Mul2One的RTP交换也排大头，RTCP交换排小头
                if ( 0 == wRcvPort%2 )
                {
                    wFirstNullIdx = wTableIdx;
                }
                else
                {
                    if ( INVALID_SWITCH_CHANNEL == wFirstNullIdx )
                    {
                        wFirstNullIdx = wTableIdx;
                    }
                }
            }
            wTableIdx++;
        }
    }

    if( !bIsExist ) // 不存在
    {
        wIdx = wFirstNullIdx;
    }

    if( wIdx < MAX_SWITCH_CHANNEL )
    {
        m_pptSwitchChannel[byConfIdx][wIdx] = tSwitchChannel;

        MpLog( LOG_INFO, "[AddSwitchTable] Add <Index.%d, %s@%d>-><%s@%d>!\n", 
                          wIdx, pchRcvIp, wRcvPort, pchDstIp, tSwitchChannel.GetDstPort());
        return TRUE;
    }
    MpLog( LOG_CRIT, "[AddSwitchTable] Add Fail, exceeded.%d !\n!", wIdx );

    return FALSE;
}


/*=============================================================================
  函 数 名： RemoveSwitchTable
  功    能： 移除交换表
  算法实现： 
  全局变量： 
  参    数： u8     byConfIdx
             TSwitchChannel *ptSwitchChannel
             u8     byType
             BOOL32 bStop
  返 回 值： BOOL32 
  ----------------------------------------------------------------------
  修改记录    ：
  日  期		版本		修改人		走读人    修改内容
  2007/04/18    4.0			张宝卿                代码移植
  2007/07/19    4.0         张宝卿                删除表项策略 修正
=============================================================================*/
BOOL32 CMpData::RemoveSwitchTable(u8 byConfIdx, TSwitchChannelV6 *ptSwitchChannel, u8 byType, BOOL32 bStop)
{
    if ( NULL == ptSwitchChannel || NULL == m_pptSwitchChannel ) 
    {
        MpLog( LOG_CRIT, "[RemoveSwitchTable] param err: ptSwitchChannel.0x%x, m_pptSwitchChannel.0x%x !\n",
                         ptSwitchChannel, m_pptSwitchChannel );
        return FALSE;
    }

    u8 byFstIdx   = INVALID_SWITCH_CHANNEL;
    u16 wTableIdx = 0;
    BOOL32 bFlag = TRUE;

    TSwitchChannelV6 tSwitchChannel = *ptSwitchChannel;

	s8* pchRcvIp  = tSwitchChannel.GetRcvIp();
    s8* pchDstIp  = tSwitchChannel.GetDstIp();
	if (NULL == pchRcvIp || NULL == pchDstIp)
	{
		return FALSE;
	}
    
    u16 wRcvPort = tSwitchChannel.GetRcvPort();
	u16 wDstPort = tSwitchChannel.GetDstPort();

    if( byConfIdx >= MAX_CONFIDX )
    {
        MpLog( LOG_CRIT, "[RemoveSwitchTable] ConfIdx:%d error !\n", byConfIdx+1 );
        return FALSE;
    }

    while(wTableIdx < MAX_SWITCH_CHANNEL)
    {
        // zbq [07/19/2007] Mul2One的交换要Rcv和Dst一起标识; 若删除所有，则需依次删除之
        if ( SWITCH_ONE2ONE == byType )
        {
			u8 dwDstIpLen    = strlen(pchDstIp);
			u8 dwTabDstIpLen = strlen(m_pptSwitchChannel[byConfIdx][wTableIdx].GetDstIp());

            if(0 == memcmp(m_pptSwitchChannel[byConfIdx][wTableIdx].GetDstIp(), pchDstIp, max(dwTabDstIpLen, dwDstIpLen)) &&
                m_pptSwitchChannel[byConfIdx][wTableIdx].GetDstPort() == wDstPort )
            {
                break;
            }
        }
        else
        {
            if ( !bStop )
            {
				u32 dwDstIpLen    = strlen(pchDstIp);
		    	u32 dwTabDstIpLen = strlen(m_pptSwitchChannel[byConfIdx][wTableIdx].GetDstIp());

				u32 dwRcvIpLen    = strlen(pchRcvIp);
				u32 dwTabRcvIpLen = strlen(m_pptSwitchChannel[byConfIdx][wTableIdx].GetRcvIp());

                if(0 == memcmp(m_pptSwitchChannel[byConfIdx][wTableIdx].GetDstIp(), pchDstIp, 
						max(dwTabDstIpLen, dwDstIpLen)) &&
                    m_pptSwitchChannel[byConfIdx][wTableIdx].GetDstPort() == wDstPort &&

                   0 == memcmp(m_pptSwitchChannel[byConfIdx][wTableIdx].GetRcvIp(), pchRcvIp, 
						max(dwTabRcvIpLen, dwRcvIpLen)) &&
                    m_pptSwitchChannel[byConfIdx][wTableIdx].GetRcvPort() == wRcvPort )
                {
                    break;
                }
            }
            else
            {
				u32 dwDstIpLen    = strlen(pchDstIp);
		    	u32 dwTabDstIpLen = strlen(m_pptSwitchChannel[byConfIdx][wTableIdx].GetDstIp());
                if(0 == memcmp(m_pptSwitchChannel[byConfIdx][wTableIdx].GetDstIp(), pchDstIp, 
						max(dwTabDstIpLen, dwDstIpLen)) &&
					m_pptSwitchChannel[byConfIdx][wTableIdx].GetDstPort() == wDstPort)
                {
                    if ( INVALID_SWITCH_CHANNEL == byFstIdx )
                    {
                        byFstIdx = wTableIdx;
                    }

                    MpLog( LOG_INFO, "[RemoveSwitchTable] Remove <%s@%d>-><%s@%d>, bStop \n",
                                      pchRcvIp, wRcvPort, pchDstIp, wDstPort );
                    
                    memset(&m_pptSwitchChannel[byConfIdx][wTableIdx], 0, sizeof(TSwitchChannelV6));
                }
            }
        }
        wTableIdx++;
    }

    if ( !bStop )
    {
        if( wTableIdx < MAX_SWITCH_CHANNEL )
        {
            memset(&m_pptSwitchChannel[byConfIdx][wTableIdx], 0, sizeof(TSwitchChannelV6));
            
            MpLog( LOG_INFO, "[RemoveSwitchTable] Remove <%s@%d>-><%s@%d> \n",
                              pchRcvIp, wRcvPort, pchDstIp, wDstPort );
        }
        else
        {
            bFlag = FALSE;
            MpLog( LOG_ERR, "[RemoveSwitchTable] Entry Failure. Entry = %s:%d.\n", pchDstIp, wDstPort);
        }        
    }
    else
    {
        if ( INVALID_SWITCH_CHANNEL == byFstIdx )
        {
            bFlag = FALSE;
            MpLog( LOG_ERR, "[RemoveSwitchTable] Entry Failure. Entry = %s:%d. bStop\n", pchDstIp, wDstPort);
        }
    }
    return bFlag;
}

/*=============================================================================
  函 数 名： RemoveSwitchTableSrc
  功    能： 移除交换表: 根据源地址和端口
  算法实现： 
  全局变量： 
  参    数： u8 byConfIdx
             TSwitchChannel *ptSwitchChannel
  返 回 值： BOOL32 
  ----------------------------------------------------------------------
  修改记录    ：
  日  期		版本		修改人		走读人    修改内容
  2007/04/20    4.0			张宝卿                代码移植
=============================================================================*/
BOOL32 CMpData::RemoveSwitchTableSrc( u8 byConfIdx, TSwitchChannelV6 *ptSwitchChannel )
{
    if ( NULL == ptSwitchChannel || NULL == m_pptSwitchChannel ) 
    {
        MpLog( LOG_CRIT, "[RemoveSwitchTableSrc] param err: ptSwitchChannel.0x%x, m_pptSwitchChannel.0x%x !\n",
                          ptSwitchChannel, m_pptSwitchChannel );
        return FALSE;
    }

    
    BOOL32 bFlag = FALSE;
    
    TSwitchChannelV6 tSwitchChannel = *ptSwitchChannel;

    s8* pchSrcIp  = tSwitchChannel.GetSrcIp();
	if (NULL == pchSrcIp)
	{
		return FALSE;
	}

    u16 wSrcPort = tSwitchChannel.GetRcvPort();
    u16 wRcvPort = tSwitchChannel.GetRcvPort();
    
    if( byConfIdx >= MAX_CONFIDX )
    {
        MpLog( LOG_CRIT, "[RemoveSwitchTableSrc] ConfIdx:%d error !\n", byConfIdx+1 );
        return FALSE;
    }
    
	u16 wTableIdx = 0;
    while(wTableIdx < MAX_SWITCH_CHANNEL)
    {
		u32 dwSrcIpLen    = strlen(pchSrcIp);
		u32 dwTabSrcIpLen = strlen(m_pptSwitchChannel[byConfIdx][wTableIdx].GetSrcIp());

        if(0 == memcmp(m_pptSwitchChannel[byConfIdx][wTableIdx].GetSrcIp(), pchSrcIp, 
						max(dwTabSrcIpLen, dwSrcIpLen)) &&
            m_pptSwitchChannel[byConfIdx][wTableIdx].GetRcvPort() == wSrcPort )
        {
            // zbq [05/25/2007] 严格匹配源交换
            if ( g_cMpApp.IsPointSrc(m_pptSwitchChannel[byConfIdx][wTableIdx].GetDstPort()) )
            {
                break;
            }
        }
        wTableIdx++;
    }
    
    if(wTableIdx < MAX_SWITCH_CHANNEL)
    {       
        MpLog( LOG_INFO, "[RemoveSwitchTable] Remove <%s@%d>-><%s@%d>-><%s@%d> by SRC\n",
                          pchSrcIp, wRcvPort, tSwitchChannel.GetRcvIp(), wRcvPort, tSwitchChannel.GetDstIp(),
                          m_pptSwitchChannel[byConfIdx][wTableIdx].GetDstPort());

        memset(&m_pptSwitchChannel[byConfIdx][wTableIdx], 0, sizeof(TSwitchChannelV6));
        bFlag = TRUE;
    }
    else
    {
        MpLog( LOG_CRIT, "[RemoveSwitchTableSrc] Entry Failure. Entry = %s:%d by Src\n", pchSrcIp, wSrcPort);
        bFlag = FALSE;
    }
    return bFlag;
}

/*=============================================================================
  函 数 名： ClearSwitchTable
  功    能： 清空交换表
  算法实现： 
  全局变量： 
  参    数： BOOL32 bClearAll
             u8 byConfIdx = 0
  返 回 值： void 
  ----------------------------------------------------------------------
  修改记录    ：
  日  期		版本		修改人		走读人    修改内容
  2007/04/20    4.0			张宝卿                代码移植
=============================================================================*/
void CMpData::ClearSwitchTable( BOOL32 bClearAll, u8 byConfIdx )
{    
    if( NULL == m_pptSwitchChannel )
    {
        MpLog( LOG_CRIT, "[ClearSwitchTable] NULL == m_pptSwitchChannel\n");
        return;
    }

    if ( bClearAll ) 
    {
        for(u8 byConfID = 0; byConfID < MAX_CONFIDX; byConfID++)
        {
            u16 wTableIdx = 0;
            while(wTableIdx < MAX_SWITCH_CHANNEL)
            {
                memset(&m_pptSwitchChannel[byConfID][wTableIdx], 0, sizeof(TSwitchChannelV6));
                wTableIdx++;
            }
        }
    }
    else
    {
        u16 wTableIdx = 0;
        while(wTableIdx < MAX_SWITCH_CHANNEL)
        {
            memset(&m_pptSwitchChannel[byConfIdx][wTableIdx], 0, sizeof(TSwitchChannelV6));
            wTableIdx++;
        }
    }

    return;
}

/*=============================================================================
  函 数 名： SearchSwitchTable
  功    能： 查找交换表
  算法实现： 
  全局变量： 
  参    数： u8 byConfIdx
             u32 dwDstIp
             u16 wDstPort
             TSwitchChannel &tSwitchChannel
  返 回 值： BOOL32 
  ----------------------------------------------------------------------
  修改记录    ：
  日  期		版本		修改人		走读人    修改内容
  2007/04/20    4.0			张宝卿                代码移植
=============================================================================*/
BOOL32 CMpData::SearchSwitchTable(u8 byConfIdx, s8* pchDstIp, u16 wDstPort, TSwitchChannelV6 &tSwitchChannel )
{    
    if( byConfIdx > MAX_CONFIDX )
    {
        MpLog( LOG_CRIT, "[SearchSwitchTable] ConfIdx:%d error !\n", byConfIdx + 1 );
        return FALSE;
    }

    if( NULL == m_pptSwitchChannel )
    {
        MpLog( LOG_CRIT, "[SearchSwitchTable] NULL == m_pptSwitchChannel\n");
        return FALSE;
    }

	if (NULL == pchDstIp)
	{
		return FALSE;
	}

    BOOL32 bFlag = FALSE;
    u16 wTableIdx = 0;

    while(wTableIdx < MAX_SWITCH_CHANNEL)
    {
		u32 dwDstIpLen    = strlen(pchDstIp);
		u32 dwTabDstIpLen = strlen(m_pptSwitchChannel[byConfIdx][wTableIdx].GetDstIp());
        if(0 == memcmp(m_pptSwitchChannel[byConfIdx][wTableIdx].GetDstIp(), pchDstIp, 
						max(dwTabDstIpLen, dwDstIpLen)) &&
            m_pptSwitchChannel[byConfIdx][wTableIdx].GetDstPort() == wDstPort )
        {
            tSwitchChannel = m_pptSwitchChannel[byConfIdx][wTableIdx];
            bFlag = TRUE;
            break;
        }
        wTableIdx++;
    }

    return bFlag;
}

/*=============================================================================
  函 数 名： SearchBridgeSwitch
  功    能： 查找交换表 里的桥交换
  算法实现： 
  全局变量： 
  参    数： u8 byConfIdx
             u16 wRcvDstPort
             TSwitchChannel &tSwitchChannel
  返 回 值： BOOL32 
  ----------------------------------------------------------------------
  修改记录    ：
  日  期		版本		修改人		走读人    修改内容
  2007/04/29    4.0			张宝卿                创建
=============================================================================*/
// BOOL32 CMpData::SearchBridgeSwitch( u8 byConfIdx, u16 wRcvDstPort, TSwitchChannelV6 &tSwitchChannel )
// {
//     if( byConfIdx > MAX_CONFIDX )
//     {
//         MpLog( LOG_CRIT, "[SearchSwitchTable] ConfIdx:%d error !\n", byConfIdx + 1 );
//         return FALSE;
//     }
//     
//     if( NULL == m_pptSwitchChannel )
//     {
//         MpLog( LOG_CRIT, "[SearchSwitchTable] NULL == m_pptSwitchChannel\n");
//         return FALSE;
//     }
//     
//     BOOL32 bFlag = FALSE;
//     u16 wTableIdx = 0;
//     
//     while(wTableIdx < MAX_SWITCH_CHANNEL)
//     {
//         if( m_pptSwitchChannel[byConfIdx][wTableIdx].GetRcvPort() == wRcvDstPort &&
//             m_pptSwitchChannel[byConfIdx][wTableIdx].GetDstPort() == wRcvDstPort )
//         {
//             tSwitchChannel = m_pptSwitchChannel[byConfIdx][wTableIdx];
//             bFlag = TRUE;
//             break;
//         }
//         wTableIdx++;
//     }
//     
//     return bFlag;
// }

/*=============================================================================
  函 数 名： IsSrcSwitchExist
  功    能： 查找交换表 里的源交换
  算法实现： 
  全局变量： 
  参    数： u8 byConfIdx
             TSwitchChannel *ptSrcSwitch
  返 回 值： BOOL32 
  ----------------------------------------------------------------------
  修改记录    ：
  日  期		版本		修改人		走读人    修改内容
  2007/06/13    4.0			张宝卿                创建
=============================================================================*/
BOOL32 CMpData::IsSrcSwitchExist( u8 byConfIdx, TSwitchChannelV6 *ptSrcSwitch )
{
    if( byConfIdx > MAX_CONFIDX )
    {
        MpLog( LOG_CRIT, "[IsSrcSwitchExist] ConfIdx:%d error !\n", byConfIdx + 1 );
        return FALSE;
    }
    
    if( NULL == m_pptSwitchChannel || NULL == ptSrcSwitch )
    {
        MpLog( LOG_CRIT, "[IsSrcSwitchExist] m_pptSwitch.0x%x, ptSrcSwitch.0x%x\n", m_pptSwitchChannel, ptSrcSwitch);
        return FALSE;
    }

    BOOL32 bGotSrcNow = FALSE;
    u8 bySwitchedPt = 0;
    if ( PS_SWITCHED == g_cMpApp.GetPtState(byConfIdx, 1) )
    {
        bySwitchedPt = 1;
    }
    else if ( PS_SWITCHED == g_cMpApp.GetPtState(byConfIdx, 2) )
    {
        bySwitchedPt = 2;
    }

    if ( bySwitchedPt != 0 )
    {
        bGotSrcNow = TRUE;
    }

    TSwitchChannelV6 tCurSrcSwitch;
    u16 wDstPort = byConfIdx * PORTSPAN + CONFBRD_STARTPORT + bySwitchedPt * POINT_NUM;

    BOOL32 bExist = FALSE;
    u16 wTableIdx = 0;

    if ( bGotSrcNow )
    {
        wTableIdx = 0;
        while (wTableIdx < MAX_SWITCH_CHANNEL)
        {
			u32 dwSrcIpLen     = strlen(ptSrcSwitch->GetSrcIp());
			u32 dwTabSrcIpLen = strlen(m_pptSwitchChannel[byConfIdx][wTableIdx].GetSrcIp());
			u32 dwRcvIpLen    = strlen(ptSrcSwitch->GetRcvIp());
			u32 dwTabRcvIpLen = strlen(m_pptSwitchChannel[byConfIdx][wTableIdx].GetRcvIp());
            
			if(0 == memcmp(m_pptSwitchChannel[byConfIdx][wTableIdx].GetSrcIp(), ptSrcSwitch->GetSrcIp(), 
						max(dwTabSrcIpLen, dwSrcIpLen)) &&
			   0 == memcmp(m_pptSwitchChannel[byConfIdx][wTableIdx].GetRcvIp(), ptSrcSwitch->GetRcvIp(), 
						max(dwTabRcvIpLen, dwRcvIpLen)) &&     
                m_pptSwitchChannel[byConfIdx][wTableIdx].GetRcvPort() == ptSrcSwitch->GetRcvPort() &&
                m_pptSwitchChannel[byConfIdx][wTableIdx].GetDstPort() == wDstPort )
            {
                bExist = TRUE;
                break;
            }
            wTableIdx++;
        }
    }

    return bExist;
}

/*=============================================================================
  函 数 名： GetSwitchChan
  功    能： 获得交换表数据区
  算法实现： 
  全局变量： 
  参    数： u8 byConfIdx:
             u16 wIndex  : 数据区位置
  返 回 值： BOOL32 
  ----------------------------------------------------------------------
  修改记录    ：
  日  期		版本		修改人		走读人    修改内容
  2007/04/20    4.0			张宝卿                创建
=============================================================================*/
BOOL32 CMpData::GetSwitchChan( u8 byConfIdx, u16 wIndex, TSwitchChannelV6 *ptSwitchChannel )
{
    if ( NULL == ptSwitchChannel || byConfIdx >= MAX_CONFIDX || wIndex >= MAX_SWITCH_CHANNEL ) 
    {
        MpLog( LOG_CRIT, "[GetSwitchTable] param err !\n" );
        return FALSE;
    }
    *ptSwitchChannel = m_pptSwitchChannel[byConfIdx][wIndex];
    return TRUE;
}


/*=============================================================================
  函 数 名： ProceedSrcChange
  功    能： 源切换通知：删除上一个源相关交换
  算法实现： 
  全局变量： 
  参    数： u8 byConfIdx
             u8 byPID
             BOOL32 bAnotherPt
  返 回 值： void 
  ----------------------------------------------------------------------
  修改记录    ：
  日  期      版本        修改人        修改内容
  07/04/11    4.0         张宝卿        创建
=============================================================================*/
void CMpData::ProceedSrcChange( u8 byConfIdx, u8 bySrcPID, BOOL32 bAnotherPt )
{
    if ( byConfIdx >= MAX_CONFIDX || bySrcPID > POINT_NUM || bySrcPID == 0 )
    {
        MpLog( LOG_CRIT, "[ProceedSrcChange] param err: ConfIdx.%d, byPID.%d \n", 
                                                           byConfIdx, bySrcPID );
        return;
    }

    u8  byDealPt = bySrcPID;
    if ( bAnotherPt )
    {
        byDealPt = bySrcPID == 1 ? 2 : 1; 
    }

	s8 achDstIp[46] = {0};	
	strofip(g_cMpApp.m_dwIpAddr, achDstIp); 

    u16 wDstPort = CONFBRD_STARTPORT + byConfIdx * PORTSPAN + byDealPt * POINT_NUM;

    TSwitchChannelV6 tSwitchM2Src;
    if ( g_cMpApp.SearchSwitchTable(byConfIdx, achDstIp, wDstPort, tSwitchM2Src) )
    {
        u32 dwRet1 = DSOK;
        u32 dwRet2 = DSOK;

		TDSNetAddr tSndAddr;
		tSndAddr.SetPort(wDstPort);
		tSndAddr.SetIPStr(tSwitchM2Src.GetSndSockType(), tSwitchM2Src.GetDstIp());

        // 1. 拆 MT->M->S1/S2 的交换
        dwRet1 = ::dsRemove( g_cMpApp.m_nInitUdpHandle, &tSndAddr);
        if ( DSOK == dwRet1 ) 
        {
            // 通知MCU删除旧的源交换
            NtfMcuSrcSwitchChg( byConfIdx, tSwitchM2Src );

            if ( !g_cMpApp.RemoveSwitchTable(byConfIdx, &tSwitchM2Src, SWITCH_ONE2ONE) ) 
            {
                MpLog( LOG_CRIT, "[ProceedSrcChange] remove <%s@%d> from table failed!\n", 
                                                                        achDstIp, wDstPort );
            }
        }
        else
        {
            MpLog( LOG_CRIT, "[ProceedSrcChange] dsRemove <%s@%d> failed, Ret=%d!\n", 
                                                achDstIp, wDstPort, dwRet1 );
        }

        
        // 2. 移除 M的dump
		TDSNetAddr tRcvAddr;
		tRcvAddr.SetIPStr(tSwitchM2Src.GetRcvSockType(), tSwitchM2Src.GetRcvIp());
		tRcvAddr.SetPort(tSwitchM2Src.GetRcvPort());

        dwRet2 = ::dsRemoveDump( g_cMpApp.m_nInitUdpHandle, &tRcvAddr);
        if ( DSOK != dwRet2 )
        {
            // FIXME: 这个基于桥交换的dump为什么会移除失败
			// to FIXME: ip应填成网络序
            MpLog( LOG_ERR, "[ProceedSrcChange] dsRemoveDump <%s@%d> failed !\n", 
                              tSwitchM2Src.GetRcvIp(), tSwitchM2Src.GetRcvPort() );
        }

        // zbq[06/15/2007] 此处可能会误拆仍然在使用的桥 ！若仍有残留，则依赖MCU加强判断处理。
        /*
        // 3. 移除 M的桥交换，保护考虑. 此保护MCU和MP独立 zbq [04/29/2007]
        TSwitchChannel tBridgeSwitch;
        u16 wRcvDstPort = tSwitchM2Src.GetRcvPort();
        if ( g_cMpApp.SearchBridgeSwitch( byConfIdx, wRcvDstPort, tBridgeSwitch ) )
        {
            u32 dwRet3 = DSOK;
            dwRet3 = ::dsRemove( g_cMpApp.m_nInitUdpHandle,
                                 tBridgeSwitch.GetDstIP(), wRcvDstPort );
            if ( DSOK == dwRet3 )
            {
                if ( !g_cMpApp.RemoveSwitchTable( byConfIdx, &tBridgeSwitch, SWITCH_ONE2ONE ) )
                {
                    MpLog( LOG_CRIT, "[ProceedSrcChange] remove brg<0x%x@%d>-><0x%x@%d> frome table failed !\n",
                                      tBridgeSwitch.GetRcvIP(), wRcvDstPort, tSwitchM2Src.GetDstIP(), wRcvDstPort );
                }
            }
            else
            {
                MpLog( LOG_CRIT, "[ProceedSrcChange] dsRemove brg<0x%x@%d>-><0x%x@%d> failed !\n",
                                  tBridgeSwitch.GetRcvIP(), wRcvDstPort, tSwitchM2Src.GetDstIP(), wRcvDstPort );
            }
        }
		*/
    }
    else
    {
        MpLog( LOG_CRIT, "[ProceedSrcChange] search for dst<%d: %s@%d> failed!\n",
                                                     byConfIdx, achDstIp, wDstPort );
    }

    // 4. 拆 M->S1/S2->T 的交换
    TSwitchChannelV6 tSwitchSrc2T;
	tSwitchSrc2T.SetRcvSockType(DS_TYPE_IPV4);
    tSwitchSrc2T.SetRcvIp(achDstIp);
    tSwitchSrc2T.SetRcvPort(wDstPort);
	tSwitchSrc2T.SetSndSockType(DS_TYPE_IPV4);
    tSwitchSrc2T.SetDstIp(achDstIp);
    tSwitchSrc2T.SetDstPort(CONFBRD_STARTPORT + byConfIdx * PORTSPAN);
	
	TDSNetAddr tManyAddr;
	tManyAddr.SetPort(tSwitchSrc2T.GetRcvPort());
	tManyAddr.SetIPStr(DS_TYPE_IPV4, achDstIp);

	TDSNetAddr tOneAddr;
	tOneAddr.SetPort(tSwitchSrc2T.GetDstPort());
	tOneAddr.SetIPStr(DS_TYPE_IPV4, achDstIp);

    u32 dwRet3 = DSOK;
	dwRet3 = ::dsRemoveManyToOne( g_cMpApp.m_nInitUdpHandle, &tManyAddr, &tOneAddr);
    if ( DSOK == dwRet3 )
    {
        if ( !g_cMpApp.RemoveSwitchTable(byConfIdx, &tSwitchSrc2T, SWITCH_MUL2ONE) ) 
        {
            MpLog( LOG_CRIT, "[ProceedSrcChange] remove <%s@%d>-><%s@%d> from table failed!\n", 
                              achDstIp, wDstPort, achDstIp, tSwitchSrc2T.GetDstPort() );
        }
    }
    else
    {
        MpLog( LOG_CRIT, "[ProceedSrcChange] RemoveM2One <%s@%d>-><%s@%d> failed, Ret=%d!\n", 
                                 achDstIp, wDstPort, achDstIp, tSwitchSrc2T.GetDstPort(), dwRet3 );
    }

    return;
}


/*=============================================================================
  函 数 名： NtfMcuSrcSwitchChg
  功    能： 通知MCU交换源变更
  算法实现： 
  全局变量： 
  参    数： u8              byConfIdx:
             TSwitchChannel  &tSwitch :
  返 回 值： BOOL32 
  ----------------------------------------------------------------------
  修改记录    ：
  日  期      版本        修改人        修改内容
  07/05/08    4.0         张宝卿        创建
=============================================================================*/
void CMpData::NtfMcuSrcSwitchChg( u8 byConfIdx, TSwitchChannelV6 &tSwitchSrc )
{
    if ( byConfIdx >= MAX_CONFIDX )
    {
        MpLog( LOG_CRIT, "[NtfMcuSrcSwitchChg] param err: confIdx.%d !\n", byConfIdx );
        return;
    }

    CServMsg cServMsg;
    cServMsg.SetConfIdx( byConfIdx + 1 );
    cServMsg.SetSrcDriId( g_cMpApp.m_byMpId );
    cServMsg.SetEventId( MP_MCU_BRDSRCSWITCHCHANGE_NTF );
    cServMsg.SetMsgBody( (u8*)&tSwitchSrc, sizeof(TMt) );
    
    SendMsg2Mcu( cServMsg.GetEventId(), cServMsg );

    return;
}

/*=============================================================================
  函 数 名： ShowWholeSwitch
  功    能： 显示所有交换信息
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void
  ----------------------------------------------------------------------
  修改记录    ：
  日  期		版本		修改人		走读人    修改内容
  2007/06/13    4.0			张宝卿                创建
=============================================================================*/
void CMpData::ShowWholeSwitch()
{
    u16 wRevPort = 0;
	u16 wDstPort = 0;;
    TSwitchChannelV6 tSwitchChannel;
    
    OspPrintf(TRUE, FALSE, "\n---------------- Total switch channel info --------------------\n");
    
    if(NULL == m_pptSwitchChannel)
    {
        MpLog( LOG_CRIT, "[ShowSwitch] NULL == m_pptSwitchChannel\n");
        return;
    }
    
	s8* pchSrcIp = NULL;
	s8* pchDisIp = NULL;
	s8* pchRcvIp = NULL;
	s8* pchDstIp = NULL;

	s8 achSrcIp[18] = {0};
	s8 achRcvIp[18] = {0};
	s8 achDstIp[18] = {0};
	
    for(int nLp = 0; nLp < MAX_CONFIDX; nLp++)
    {
        for(int nLp1 = 0; nLp1 < MAX_SWITCH_CHANNEL; nLp1++)
        {
            tSwitchChannel = m_pptSwitchChannel[nLp][nLp1];
            if(!tSwitchChannel.IsNull() || !tSwitchChannel.IsSrcNull() || !tSwitchChannel.IsRcvNull())
            {
                u8 byConfIdx = 0;
                u8 byPointId = 0;
                		
				memset(achSrcIp, 0, sizeof(achSrcIp));
				memset(achRcvIp, 0, sizeof(achRcvIp));
				memset(achDstIp, 0, sizeof(achDstIp));

                pchSrcIp = tSwitchChannel.GetSrcIp();
                pchDisIp = tSwitchChannel.GetDisIp();
                pchRcvIp = tSwitchChannel.GetRcvIp();
                pchDstIp = tSwitchChannel.GetDstIp();
                
                wRevPort = tSwitchChannel.GetRcvPort();
                wDstPort = tSwitchChannel.GetDstPort();
                
                u16 wSrcPort  = 0;
                
                if ( g_cMpApp.IsPointT(wRevPort) )
                {
                    byConfIdx = (wRevPort - CONFBRD_STARTPORT) / PORTSPAN;
                    
                    for( u8 byPId = 1; byPId <= POINT_NUM; byPId ++ )
                    {
                        if ( PS_SWITCHED == g_cMpApp.GetPtState(byConfIdx, byPId) ) 
                        {
                            wSrcPort = CONFBRD_STARTPORT + byConfIdx * PORTSPAN + byPId * POINT_NUM;
                            break;
                        }                        
                    }

					u8 byIndex = 0;
					if (IsV6MtIp(pchSrcIp, byIndex))
					{
						u32 dwV4Ip = GetV4IpAcd2Tab(byIndex);
						strofip(dwV4Ip, achSrcIp);
						pchSrcIp = achSrcIp;
					}  
					OspPrintf(TRUE, FALSE, "\nConf%d %d  %s:%d", nLp+1, nLp1, pchSrcIp, wSrcPort);					
                }
                else if ( g_cMpApp.IsPointSrc(wRevPort, &byConfIdx, &byPointId) )
                {
                    TSwitchChannelV6 tSwitchSrc;
                    g_cMpApp.SearchSwitchTable( byConfIdx, pchDstIp, wRevPort, tSwitchSrc );
                    wSrcPort = tSwitchSrc.GetRcvPort();
                    
					u8 byIndex = 0;
					if (IsV6MtIp(pchSrcIp, byIndex))
					{
						u32 dwV4Ip = GetV4IpAcd2Tab(byIndex);
						strofip(dwV4Ip, achSrcIp);
						pchSrcIp = achSrcIp;
					}  
                    OspPrintf(TRUE, FALSE, "\nConf%d %d  %s:%d", nLp+1, nLp1, pchSrcIp, wSrcPort);
                }
                else
                {
					u8 byIndex = 0;
					if (IsV6MtIp(pchSrcIp, byIndex))
					{
						u32 dwV4Ip = GetV4IpAcd2Tab(byIndex);
						strofip(dwV4Ip, achSrcIp);
						pchSrcIp = achSrcIp;
					}  
                    OspPrintf(TRUE, FALSE, "\nConf%d %d  %s:%d", nLp+1, nLp1, pchSrcIp, wRevPort);
                }
                
                if(0 != strlen(pchDisIp))
                {
                    OspPrintf(TRUE, FALSE, "--> %s:%d", pchDisIp, wRevPort);
                }
				u8 byIndex = 0;
				if (IsV6MtIp(pchRcvIp, byIndex))
				{
					u32 dwV4Ip = GetV4IpAcd2Tab(byIndex);
					strofip(dwV4Ip, achRcvIp);
					pchRcvIp = achRcvIp;
				}  
                OspPrintf(TRUE, FALSE, "--> %s:%d", pchRcvIp, wRevPort);

				byIndex = 0;
				if (IsV6MtIp(pchDstIp, byIndex))
				{
					u32 dwV4Ip = GetV4IpAcd2Tab(byIndex);
					strofip(dwV4Ip, achDstIp);
					pchDstIp = achDstIp;
				}  
                OspPrintf(TRUE, FALSE, "--> %s:%d", pchDstIp, wDstPort);
            }
        }
    }
    OspPrintf( TRUE, FALSE, "\n" );
    
    return;
}

/*=============================================================================
  函 数 名： ShowSwitchUniformInfo
  功    能： 
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void
  ----------------------------------------------------------------------
  修改记录    ：
  日  期		版本		修改人		走读人    修改内容
  2007/04/20    4.0			张宝卿                代码整理、移植
=============================================================================*/
void CMpData::ShowSwitchUniformInfo()
{
	u16 wDstPort;
	u16 wLoop1, wLoop2;
    TSendFilterAppData * ptSendAppData;

    OspPrintf(TRUE,FALSE,"\n----------------------switch channel uniform info--------------------\n");
    OspPrintf(TRUE,FALSE,"\nConfId ChnlNo         destaddr        IsUniform(0:no uniform 1:uniform)\n");

    if(NULL == g_cMpApp.m_pptFilterData)
    {
        return;
    }

	s8* pchDstIp = NULL;
    for (wLoop1=0; wLoop1 < MAX_CONFIDX; wLoop1++)
	{
        if ( CONF_UNIFORMMODE_VALID == g_cMpApp.GetConfUniMode((u8)wLoop1) )
        {
		    for (wLoop2=0; wLoop2<MAX_SWITCH_CHANNEL; wLoop2++)
		    {
			    ptSendAppData = &g_cMpApp.m_pptFilterData[wLoop1][wLoop2];
			    if (ptSendAppData->byConfNo != 0xFF && ptSendAppData->wChannelNo != 0xFFFF)
			    {
				    pchDstIp  = ptSendAppData->achDstIp;
				    wDstPort = ptSendAppData->wDstPort;
                    if (ptSendAppData->bUniform)
                    {
                    }

				    OspPrintf(TRUE, FALSE, "   %d      %d       %s:%d          %d\n",
                              wLoop1+1, wLoop2, pchDstIp, wDstPort, ptSendAppData->bUniform);
			    }
		    }
        }
        else if ( CONF_UNIFORMMODE_NONE == g_cMpApp.GetConfUniMode((u8)wLoop1) )
        {
            OspPrintf(TRUE, FALSE, "   %d      Encrypt conf isn't uniform packet\n", wLoop1+1);
        }
	}
    OspPrintf(TRUE,FALSE,"\n----------------------switch channel uniform end---------------------\n");
}

/*=============================================================================
  函 数 名： IsPointSrc
  功    能： 判断某端口是否为终端节点 S1或S2
  算法实现： 
  全局变量： 
  参    数： u16 wPort
             u8 *pbyConfIdx
             u8 *pbyPointId
  返 回 值： BOOL32
  ----------------------------------------------------------------------
  修改记录    ：
  日  期		版本		修改人		走读人    修改内容
  2007/04/10    4.0			张宝卿                创建
=============================================================================*/
BOOL32 CMpData::IsPointSrc( u16 wPort, u8 *pbyConfIdx, u8 *pbyPointId )
{
    if ( wPort > CONFBRD_STARTPORT &&
         wPort < CONFBRD_STARTPORT + MAX_CONFIDX * PORTSPAN ) 
    {
        //只滤 S1或S2
        if ( 0 != (wPort - CONFBRD_STARTPORT) % PORTSPAN ) 
        {
            // 会议索引，非会议号
            u8 byConfIdx = (wPort - CONFBRD_STARTPORT) / PORTSPAN;
            u8 byPointId = (wPort - CONFBRD_STARTPORT - PORTSPAN * byConfIdx) / POINT_NUM; 
			
            if ( 1 == byPointId || 2 == byPointId ) 
            {
                if ( NULL != pbyConfIdx ) 
                {
                    *pbyConfIdx = byConfIdx;
                }
                if ( NULL != pbyPointId ) 
                {
                    *pbyPointId = byPointId;
                }
                return TRUE;
            }
        }
    }
    return FALSE;
}

/*=============================================================================
  函 数 名： IsPointMt
  功    能： 判断某端口是否为终端端口
  算法实现： 
  全局变量： 
  参    数： u16 wPort
  返 回 值： BOOL32
  ----------------------------------------------------------------------
  修改记录    ：
  日  期		版本		修改人		走读人    修改内容
  2007/04/10    4.0			张宝卿                创建
=============================================================================*/
BOOL32 CMpData::IsPointMt( u16 wPort )
{
    BOOL32 bRet = FALSE;
    if ( wPort >= MT_MCU_STARTPORT &&
         wPort <= MT_MCU_STARTPORT + MAXNUM_MCU_MT * PORTSPAN ) 
    {
        bRet = 0 == wPort % PORTSPAN ? TRUE : FALSE;
    }
    return bRet;
}

/*=============================================================================
  函 数 名： IsPointT
  功    能： 判断某端口是否为T节点
  算法实现： 
  全局变量： 
  参    数： u16 wPort
  返 回 值： BOOL32
  ----------------------------------------------------------------------
  修改记录    ：
  日  期		版本		修改人		走读人    修改内容
  2007/04/10    4.0			张宝卿                创建
=============================================================================*/
BOOL32 CMpData::IsPointT( u16 wPort )
{
    BOOL32 bRet = FALSE;
    if ( wPort >= CONFBRD_STARTPORT &&
         wPort <= CONFBRD_STARTPORT + MAX_CONFIDX * PORTSPAN ) 
    {
        bRet = 0 == wPort % PORTSPAN ? TRUE : FALSE;
    }
    return bRet;
}

/*=============================================================================
    函 数 名： GetWaitIFrameDataBySrc
    功    能： 根据源信息获取等待关键帧信息
    算法实现： 
    全局变量： 
    参    数： u32 dwSrcIp
               u16 wSrcPort
               TWaitIFrameData *ptWaitData
    返 回 值： BOOL 
    ----------------------------------------------------------------------
    修改记录    ：
    日  期		版本		修改人		走读人    修改内容
    2006/5/11  4.0			许世林                  创建
=============================================================================*/
BOOL CMpData::GetWaitIFrameDataBySrc(s8* pchSrcIp, u16 wSrcPort, TWaitIFrameData *ptWaitData)
{
    for(u8 byConfNo = 0; byConfNo < MAXNUM_MCU_CONF; byConfNo++)
    {
        for(u8 byMtIdx = 0; byMtIdx < MAXNUM_CONF_MT+POINT_NUM; byMtIdx++)
        {          
			u32 dwSrcIpLen    = strlen(pchSrcIp);
			u32 dwTabSrcIpLen = strlen(m_pptWaitIFrmData[byConfNo][byMtIdx].achSrcIp);

            if (0 == memcmp(m_pptWaitIFrmData[byConfNo][byMtIdx].achSrcIp, pchSrcIp,
						max(dwTabSrcIpLen, dwSrcIpLen)) && 

                m_pptWaitIFrmData[byConfNo][byMtIdx].wSrcPort == wSrcPort)
            {
                *ptWaitData = m_pptWaitIFrmData[byConfNo][byMtIdx];
                return TRUE;
            }
        }
    }
    
    return FALSE;
}

/*=============================================================================
    函 数 名： SetConfUniMode
    功    能： 会议是否归一重整
    算法实现： 
    全局变量： 
    参    数： u8 byConfIdx, u8 byMode
    返 回 值： BOOL 
    ----------------------------------------------------------------------
    修改记录    ：
    日  期		版本		修改人		走读人    修改内容
    2006/5/11  4.0			许世林                  创建
=============================================================================*/
void CMpData::SetConfUniMode( u8 byConfIdx, u8 byMode )
{
    m_abyConfMode[byConfIdx] = byMode;
    return;
}

/*=============================================================================
    函 数 名： GetConfUniMode
    功    能： 会议是否归一重整
    算法实现： 
    全局变量： 
    参    数： u8 byConfIdx
    返 回 值： BOOL 
    ----------------------------------------------------------------------
    修改记录    ：
    日  期		版本		修改人		走读人    修改内容
    2006/5/11  4.0			许世林                  创建
=============================================================================*/
u8 CMpData::GetConfUniMode( u8 byConfIdx )
{
    return m_abyConfMode[byConfIdx];
}

/*=============================================================================
    函 数 名： GetPtIdle
    功    能： 获得空闲的视频源中转点
    算法实现： 
    全局变量： 
    参    数： u8 byConfIdx
    返 回 值： u8 : 返回 1 或 2：成功
                    返回 0     ：失败
    ----------------------------------------------------------------------
    修改记录    ：
    日  期		版本		修改人		走读人    修改内容
    2007/03/29  4.0			张宝卿                  创建
=============================================================================*/
u8 CMpData::GetPtIdle( u8 byConfIdx )
{
    u8 byPoint = 1;
    for( ; byPoint <= POINT_NUM; byPoint ++ )
    {
        if ( PS_IDLE == m_abyPtState[byConfIdx][byPoint-1] )
        {
            return byPoint;
        }
    }
    MpLog( LOG_ERR, "[GetPtIdle] ConfIdx.%d get Idle point failed! CurStatus<%d, %d>\n",
                      byConfIdx, m_abyPtState[byConfIdx][0], m_abyPtState[byConfIdx][1]);
    return 0;
}

/*=============================================================================
    函 数 名： GetPtState
    功    能： 获取 某中转节点 某视频端口 当前状态
    算法实现： 
    全局变量： 
    参    数： 
    返 回 值： u8 失败：PS_UNEXIST
    ----------------------------------------------------------------------
    修改记录    ：
    日  期		版本		修改人		走读人    修改内容
    2007/03/29  4.0			张宝卿                  创建
=============================================================================*/
u8 CMpData::GetPtState( u8 byConfIdx, u8 byPoint, BOOL32 bAnother )
{
    if ( byConfIdx >= MAX_CONFIDX || 0 == byPoint ) 
    {
        MpLog( LOG_CRIT, "[GetPtState] param err: ConfIdx.%d, byPoint.%d !\n",
                          byConfIdx, byPoint );
        return PS_UNEXIST;
    }
    if ( !bAnother )
    {
        return m_abyPtState[byConfIdx][byPoint-1];
    }
    else
    {
        u8 byAnotherPt = 0;
        if ( byPoint == 1 )
        {
            byAnotherPt = 2;
        }
        else if ( byPoint == 2 )
        {
            byAnotherPt = 1;
        }
        else
        {
            MpLog( LOG_CRIT, "[GetPtState] Cur Point.%d unexist!\n", byPoint );
            return PS_UNEXIST;
        }

        return m_abyPtState[byConfIdx][byAnotherPt-1];
    }
}

/*=============================================================================
    函 数 名： SetPtState
    功    能： 标注 某中转节点 某视频端口 当前状态
    算法实现： 
    全局变量： 
    参    数： 
    返 回 值： void
    ----------------------------------------------------------------------
    修改记录    ：
    日  期		版本		修改人		走读人    修改内容
    2007/03/29  4.0			张宝卿                  创建
=============================================================================*/
void CMpData::SetPtState( u8 byConfIdx, u8 byPoint, u8 byState, BOOL32 bAnother )
{
	/*lint -save -e1788*/
    ENTER( m_hPtInfo )
	
    if ( !bAnother )
    {
        if ( PS_IDLE != m_abyPtState[byConfIdx][byPoint-1] )
        {
            MpLog( LOG_INFO, "[SetPtState] ConfIdx.%d: byPoint.%d state change from.%d to %d !\n", 
                              byConfIdx, byPoint, m_abyPtState[byConfIdx][byPoint-1], byState );
        }
        m_abyPtState[byConfIdx][byPoint-1] = byState;
    }
    else
    {
        u8 byAnotherPt = 0;
        if ( byPoint == 1 ) 
        {
            byAnotherPt = 2;
        }
        else if ( byPoint == 2 )
        {
            byAnotherPt = 1;
        }
        else
        {
            MpLog( LOG_CRIT, "[SetPtState] cur point.%d unexist !\n", byPoint );
            return;
        }
        if ( PS_IDLE != m_abyPtState[byConfIdx][byAnotherPt-1] )
        {
            MpLog( LOG_INFO, "[SetPtState] ConfIdx.%d: byPoint.%d state change from.%d to %d !\n", 
                        byConfIdx, byAnotherPt, m_abyPtState[byConfIdx][byAnotherPt-1], byState );
        }
        m_abyPtState[byConfIdx][byAnotherPt-1] = byState;
    }
    return;
	/*lint -restore*/
}

/*=============================================================================
    函 数 名： SetChgDelay
    功    能： 设置 某会议是否强制切换的超时时长 单位 tick
    算法实现： 
    全局变量： 
    参    数： u8  byConfIdx
               u16 wDelayTime
    返 回 值： void
    ----------------------------------------------------------------------
    修改记录    ：
    日  期		版本		修改人		走读人    修改内容
    2007/04/13  4.0			张宝卿                  创建
=============================================================================*/
void CMpData::SetChgDelay( u8 byConfIdx, u16 wDelayTick )
{
	/*lint -save -e1788*/
    ENTER( m_hPtTick )
    m_awChgDelayTick[byConfIdx] = wDelayTick;
    return;
	/*lint -restore*/
}

/*=============================================================================
    函 数 名： GetChgDelay
    功    能： 获取 某会议是否强制切换的超时时长 单位 tick
    算法实现： 
    全局变量： 
    参    数： u8 byConfIdx 
    返 回 值： u16
    ----------------------------------------------------------------------
    修改记录    ：
    日  期		版本		修改人		走读人    修改内容
    2007/04/13  4.0			张宝卿                  创建
=============================================================================*/
u16 CMpData::GetChgDelay( u8 byConfIdx )
{
    return m_awChgDelayTick[byConfIdx];
}

/*=============================================================================
    函 数 名： ClearChgDelay
    功    能： 设置 某会议是否强制切换的超时时长 单位 tick
    算法实现： 
    全局变量： 
    参    数： u8  byConfIdx
    返 回 值： void
    ----------------------------------------------------------------------
    修改记录    ：
    日  期		版本		修改人		走读人    修改内容
    2007/04/19  4.0			张宝卿                  创建
=============================================================================*/
void CMpData::ClearChgDelay( u8 byConfIdx )
{
	/*lint -save -e1788*/
    ENTER( m_hPtTick )
    m_awChgDelayTick[byConfIdx] = 0;
    return;
	/*lint -restore*/
}

/*=============================================================================
  函 数 名： GetChanNoByDst
  功    能： 
  算法实现： 
  全局变量： 
  参    数： u8  byConfIdx:
             u32 dwDstIp  :
             u16 wDstPort :
             u8  byGetType: 0-set, 1-romove
             u16 wRcvPort : 目前只针对 M->S1/S2->T 的区分处理
  返 回 值： u16 
=============================================================================*/
u16  CMpData::GetChanNoByDst(u8 byConfIdx, s8* pchDstIp, u16 wDstPort, u8 byGetType, u16 wRcvPort)
{
    u16 wChanNo = 0;

    if( NULL == g_cMpApp.m_pptFilterData )
    {
        MpLog( LOG_CRIT, "[GetChanNoByDst] err: NULL == m_pptFilterData\n");
        return wChanNo;
    }

	if (NULL == pchDstIp)
	{
		MpLog( LOG_CRIT, "[GetChanNoByDst]pchDstIp is null!\n");
		return 0xffff;
	}
    // 
    while ( wChanNo < MAX_SWITCH_CHANNEL )
    {
		u32 dwDstIpLen    = strlen(pchDstIp);
		u32 dwTabDstIpLen = strlen(m_pptFilterData[byConfIdx][wChanNo].achDstIp);

        if (0 == memcmp(m_pptFilterData[byConfIdx][wChanNo].achDstIp, pchDstIp,
						max(dwTabDstIpLen, dwDstIpLen)) &&

            m_pptFilterData[byConfIdx][wChanNo].wDstPort == wDstPort)
        {
            if ( 0 != wRcvPort )
            {
                // 区分 S1->T 和 S2->T
                if ( IsPortSrcToT(byConfIdx, wRcvPort) &&
                     m_pptFilterData[byConfIdx][wChanNo].wRcvPort == wRcvPort )
                {
                    return wChanNo;
                }
            }
            else
            {
                return wChanNo;
            }
        }
		wChanNo++;
    }

    if (wChanNo >= MAX_SWITCH_CHANNEL && byGetType == 1)
    {
        return MAX_SWITCH_CHANNEL;
    }

    wChanNo = 0;
    while (wChanNo < MAX_SWITCH_CHANNEL) // 找一个空的位置
    {
        if (0 == strlen(m_pptFilterData[byConfIdx][wChanNo].achDstIp))
        {
            u32 dwNow = time(NULL);

			u32 dwDstIpLen    = strlen(pchDstIp)+1;
			u32 dwTabDstIpLen = sizeof(m_pptFilterData[byConfIdx][wChanNo].achDstIp);

			memcpy(m_pptFilterData[byConfIdx][wChanNo].achDstIp, pchDstIp,
					min(dwTabDstIpLen, dwDstIpLen));

            m_pptFilterData[byConfIdx][wChanNo].wDstPort = wDstPort;

            if ( IsPortSrcToT(byConfIdx, wRcvPort) )
            {
                m_pptFilterData[byConfIdx][wChanNo].wRcvPort = wRcvPort;
            }           

            srand(dwNow);
            m_pptFilterData[byConfIdx][wChanNo].wSeqNum     = rand();
            m_pptFilterData[byConfIdx][wChanNo].dwTimeStamp = rand();
            m_pptFilterData[byConfIdx][wChanNo].dwSSRC      = rand();
            
            m_pptFilterData[byConfIdx][wChanNo].dwAddTime   = dwNow;
            m_pptFilterData[byConfIdx][wChanNo].bRemoved    = FALSE;
            return wChanNo;
        }
		
		wChanNo++;
    }

    wChanNo = 0;
    u16 wTempChannelNo = 1;
    u16 wSelectChannelNo = MAX_SWITCH_CHANNEL;

    while (wChanNo < MAX_SWITCH_CHANNEL) // 寻找合适位置(已经移除了的)
    {
        if (m_pptFilterData[byConfIdx][wChanNo].bRemoved)
        {
            wTempChannelNo = wChanNo + 1;
            while (wTempChannelNo < MAX_SWITCH_CHANNEL) // 寻找最早加入的位置
            {
                if (m_pptFilterData[byConfIdx][wTempChannelNo].bRemoved)
                {
                    if (m_pptFilterData[byConfIdx][wChanNo].dwAddTime >
                        m_pptFilterData[byConfIdx][wTempChannelNo].dwAddTime)
                    {
                        wSelectChannelNo = wTempChannelNo;
                        wChanNo = wTempChannelNo;
                    }
                }
                wTempChannelNo++;
            }
            wSelectChannelNo = wChanNo;
            break;
        }
        wChanNo++;
    }

    if (wSelectChannelNo < MAX_SWITCH_CHANNEL)
    {
        u32 dwNow = time(NULL);
		u32 dwDstIpLen = strlen(pchDstIp)+1;
		u32 dwTabIpLen = sizeof(m_pptFilterData[byConfIdx][wSelectChannelNo].achDstIp);
		memcpy(m_pptFilterData[byConfIdx][wSelectChannelNo].achDstIp, pchDstIp, 
				min(dwTabIpLen, dwDstIpLen));

        m_pptFilterData[byConfIdx][wSelectChannelNo].wDstPort = wDstPort;
        
        if ( IsPortSrcToT(byConfIdx, wRcvPort) )
        {
            m_pptFilterData[byConfIdx][wSelectChannelNo].wRcvPort = wRcvPort;
        } 

        srand(dwNow);
        m_pptFilterData[byConfIdx][wSelectChannelNo].wSeqNum     = rand();
        m_pptFilterData[byConfIdx][wSelectChannelNo].dwTimeStamp = rand();
        m_pptFilterData[byConfIdx][wSelectChannelNo].dwSSRC      = rand();
    
        m_pptFilterData[byConfIdx][wSelectChannelNo].dwAddTime   = dwNow;
        m_pptFilterData[byConfIdx][wSelectChannelNo].bRemoved    = FALSE;
    }

    return wSelectChannelNo;
}

/*=============================================================================
  函 数 名： IsPortSrcToT
  功    能： 是否为中转节点的合法PORT
  算法实现： 
  全局变量： 
  参    数： u16 wRcvPort
  返 回 值： BOOL32
  ----------------------------------------------------------------------
  修改记录    ：
  日  期	  版本		    修改人		走读人    修改内容
  2007/04/04  4.0			张宝卿                  创建
=============================================================================*/
BOOL32 CMpData::IsPortSrcToT( u8 byConfIdx, u16 wRcvPort )
{
    if ( wRcvPort >= CONFBRD_STARTPORT ||
         wRcvPort <= CONFBRD_STARTPORT + MAX_CONFIDX * PORTSPAN )
    {
        u16 wPortRem = CONFBRD_STARTPORT + byConfIdx * PORTSPAN;
        if ( ( 0 == (wRcvPort - wPortRem) % POINT_NUM) &&
              ( 1 == (wRcvPort - wPortRem) / POINT_NUM ||
                2 == (wRcvPort - wPortRem) / POINT_NUM ) )
        {
            MpLog( LOG_INFO, "[IsPortSrcToT] Legal port.%d, ConfIdx.%d !\n", wRcvPort, byConfIdx );
            return TRUE;
        }
    }
    MpLog( LOG_ERR, "[IsPortSrcToT] illegal port.%d, ConfIdx.%d !\n", wRcvPort, byConfIdx );
    return FALSE;
}

/*=============================================================================
  函 数 名： IsPortNeedChkIFrm
  功    能： 是否为需要检测关键帧的接收端口
  算法实现： 
  全局变量： 
  参    数： u16 wRcvPort
  返 回 值： BOOL32
  ----------------------------------------------------------------------
  修改记录    ：
  日  期		版本		修改人		走读人    修改内容
  2007/04/20    4.0			张宝卿                  创建
=============================================================================*/
BOOL32 CMpData::IsPortNeedChkIFrm( u16 wRcvPort )
{
    return IsPointSrc(wRcvPort) || IsPointT(wRcvPort) || IsPointMt(wRcvPort);
}

/*=============================================================================
  函 数 名： IsTInUseStill
  功    能： 除了本通道外，本会议是否还有其他终端接收T端口的码流
  算法实现： 
  全局变量： 
  参    数： u8 byConfIdx
  返 回 值： BOOL32
  ----------------------------------------------------------------------
  修改记录    ：
  日  期		版本		修改人		走读人    修改内容
  2007/04/27    4.0			张宝卿                创建
=============================================================================*/
BOOL32 CMpData::IsTInUseStill( u8 byConfIdx )
{
    BOOL32 bRet = FALSE;

    if ( byConfIdx >= MAX_CONFIDX )
    {
        MpLog( LOG_CRIT, "[IsTInUseStill] param err: byConfIdx=%d !\n", byConfIdx );
        return FALSE;
    }

    // 需要从T端取数据的终端数
    u8 byMtDataFromT = 0;
    
	s8 achMpIp[IPV6_NAME_LEN] = {0};
	strofip(g_cMpApp.m_dwIpAddr, achMpIp); 

    for( u16 wChnIdx = 0; wChnIdx < MAX_SWITCH_CHANNEL; wChnIdx ++ )
    {
        if ( 0 != strlen(m_pptSwitchChannel[byConfIdx][wChnIdx].GetDstIp()) &&
             0 != m_pptSwitchChannel[byConfIdx][wChnIdx].GetDstPort() )
        {
			u32 dwMpIpLen     = sizeof(achMpIp);
			u32 dwTabRcvIpLen = strlen(m_pptSwitchChannel[byConfIdx][wChnIdx].GetRcvIp())+1;

            if (memcmp(achMpIp, m_pptSwitchChannel[byConfIdx][wChnIdx].GetRcvIp(),
						max(dwMpIpLen, dwTabRcvIpLen)) &&
                 
				CONFBRD_STARTPORT+byConfIdx*PORTSPAN == m_pptSwitchChannel[byConfIdx][wChnIdx].GetRcvPort() )
            {
                byMtDataFromT ++;

                MpLog( LOG_INFO, "[IsTInUseStill] Conf.%d: Rcv.%s@%d --> Dst.%s@%d !\n",
                                  byConfIdx,
                                  m_pptSwitchChannel[byConfIdx][wChnIdx].GetRcvIp(),
                                  m_pptSwitchChannel[byConfIdx][wChnIdx].GetRcvPort(),
                                  m_pptSwitchChannel[byConfIdx][wChnIdx].GetDstIp(),
                                  m_pptSwitchChannel[byConfIdx][wChnIdx].GetDstPort());
            }
        }
    }

    if ( 0 != byMtDataFromT )
    {
        bRet = TRUE;
        MpLog( LOG_INFO, "[IsTInUseStill] switchNum.%d in use of T !\n", byMtDataFromT );
    }

    return bRet;
}

/*=============================================================================
  函 数 名： GetMtNumNeedT
  功    能： 获取仍可能从T节点转出码流的终端数
  算法实现： 
  全局变量： 
  参    数： u8 byConfIdx
  返 回 值： u8
  ----------------------------------------------------------------------
  修改记录    ：
  日  期		版本		修改人		走读人    修改内容
  2007/05/13    4.0			张宝卿                创建
=============================================================================*/
u8 CMpData::GetMtNumNeedT( u8 byConfIdx )
{
    if ( byConfIdx >= MAX_CONFIDX )
    {
        MpLog( LOG_CRIT, "[GetMtNumNeedT] param err: confIdx.%d \n", byConfIdx );
        return 0;
    }
    return m_byMtNumNeedT[byConfIdx];
}

/*=============================================================================
  函 数 名： SetMtNumNeedT
  功    能： 设置 仍可能从T节点转出码流的终端数
  算法实现： 
  全局变量： 
  参    数： u8 byConfIdx
             u8 byMtNum
  返 回 值： void
  ----------------------------------------------------------------------
  修改记录    ：
  日  期		版本		修改人		走读人    修改内容
  2007/05/13    4.0			张宝卿                创建
=============================================================================*/
void CMpData::SetMtNumNeedT( u8 byConfIdx, u8 byMtNum )
{
	/*lint -save -e1788*/
    ENTER( m_hMtNum )

    if ( byConfIdx >= MAX_CONFIDX )
    {
        MpLog( LOG_CRIT, "[SetMtNumNeedT] param err: confIdx.%d \n", byConfIdx );
        return;
    }
    m_byMtNumNeedT[byConfIdx] = byMtNum;

    return;
	/*lint -restore*/
}

/*=============================================================================
  函 数 名： SendMsg2Mcu
  功    能： 发消息给Mcu
  算法实现： 
  全局变量： 
  参    数： u16      wEvent
             CServMsg &cServMsg
  返 回 值： void 
=============================================================================*/
void CMpData::SendMsg2Mcu( u16 wEvent, CServMsg &cServMsg )
{
    if( OspIsValidTcpNode(m_dwMcuNode) || m_bEmbedA )
    {
        OspPost( m_dwMcuIId, wEvent, cServMsg.GetServMsg(), cServMsg.GetServMsgLen(), m_dwMcuNode );
    }
    
    if( OspIsValidTcpNode(m_dwMcuNodeB)  || m_bEmbedB )
    {
        OspPost( m_dwMcuIIdB, wEvent, cServMsg.GetServMsg(), cServMsg.GetServMsgLen(), m_dwMcuNodeB );
    }
    
    return;
}

/*=============================================================================
  函 数 名： IsPortSrcToT
  功    能： 是否为中转节点的合法PORT
  算法实现： 
  全局变量： 
  参    数： u16 wRcvPort
  返 回 值： BOOL32
  ----------------------------------------------------------------------
  修改记录    ：
  日  期		版本		修改人		走读人    修改内容
  2007/04/18    4.0			张宝卿                  创建
=============================================================================*/
void CMpData::ShowHandle()
{
    OspPrintf( TRUE, FALSE, "PtStateHandle.0x%x \n", m_hPtInfo );
    OspPrintf( TRUE, FALSE, "PtTickHandle.0x%x \n",  m_hPtTick );
    OspPrintf( TRUE, FALSE, "PtMtNumHandle.0x%x \n", m_hMtNum );
}

/*=============================================================================
  函 数 名： IsV6MtIp
  功    能： 是否V6终端
  算法实现： 
  全局变量： 
  参    数： u32 dwMtIp V4Ip
			 u8& byIdx

  返 回 值： BOOL32
  ----------------------------------------------------------------------
  修改记录    ：
  日  期		版本		修改人		走读人    修改内容
  2012/05/11    4.0			周嘉麟                  创建
=============================================================================*/
BOOL32 CMpData::IsV6MtIp(u32 dwMtIp, u8& byIndex)
{
	if (0 == strlen(g_cMpApp.m_achV6Ip))
	{
		return FALSE;
	}

	if (0 == dwMtIp)
	{
		OspPrintf(TRUE, FALSE, "[IsV6MtIp] dwMtIp = 0!\n");
		return FALSE;
	}

	for (u8 byIdx = 0; byIdx < MAXNUM_CONF_MT; byIdx++)
	{
		if (dwMtIp == m_tMtIpMapTab.m_atIpMap[byIdx].GetV4IP())
		{				
			byIndex = byIdx;
			return TRUE;
		}
	}
	return FALSE;

}


/*=============================================================================
  函 数 名： IsV6MtIp
  功    能： 是否V6终端
  算法实现： 
  全局变量： 
  参    数： s8* pchMtIp
			 u8& byIndex

  返 回 值： BOOL32
  ----------------------------------------------------------------------
  修改记录    ：
  日  期		版本		修改人		走读人    修改内容
  2012/05/11    4.0			周嘉麟                  创建
=============================================================================*/
BOOL32 CMpData::IsV6MtIp(s8* pchMtIp, u8& byIndex)
{
	if (0 == strlen(g_cMpApp.m_achV6Ip))
	{
		return FALSE;
	}

	if (NULL == pchMtIp)
	{
		OspPrintf(TRUE, FALSE, "[IsV6MtIp2] pchMtIp is null!\n");
		return FALSE;
	}
	

	for (u8 byIdx = 0; byIdx < MAXNUM_CONF_MT; byIdx++)
	{
		u32 dwMapTabIpLen = strlen(m_tMtIpMapTab.m_atIpMap[byIdx].GetV6IP());
		u32 dwMtIpLen     = strlen(pchMtIp);
		if (0 == memcmp(pchMtIp, m_tMtIpMapTab.m_atIpMap[byIdx].GetV6IP(),
					max(dwMtIpLen, dwMapTabIpLen)))
		{				
			byIndex = byIdx;
			return TRUE;
		}
	}
	return FALSE;
}

/*=============================================================================
  函 数 名： GetV6IpAcd2Tab
  功    能： 获取指定的V6IP
  算法实现： 
  全局变量： 
  参    数： u8 byIndex

  返 回 值： s8*
  ----------------------------------------------------------------------
  修改记录    ：
  日  期		版本		修改人		走读人    修改内容
  2012/05/11    4.0			周嘉麟                  创建
=============================================================================*/
s8* CMpData::GetV6IpAcd2Tab(u8 byIndex)
{
	if (byIndex >= MAXNUM_CONF_MT)
	{
		return NULL;
	}

	return m_tMtIpMapTab.m_atIpMap[byIndex].GetV6IP();
}

/*=============================================================================
  函 数 名： GetV4IpAcd2Tab
  功    能： 获取指定的V4IP
  算法实现： 
  全局变量： 
  参    数： u8 byIndex

  返 回 值： s8*
  ----------------------------------------------------------------------
  修改记录    ：
  日  期		版本		修改人		走读人    修改内容
  2012/05/11    4.7			周嘉麟                  创建
=============================================================================*/
u32 CMpData::GetV4IpAcd2Tab(u8 byIndex)
{
	if (byIndex >= MAXNUM_CONF_MT)
	{
		return NULL;
	}

	return m_tMtIpMapTab.m_atIpMap[byIndex].GetV4IP();
}

//END OF FILE
