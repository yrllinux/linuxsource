/*****************************************************************************
   模块名      : mcu_new
   文件名      : mcuvcnms.cpp
   相关文件    : mcuvc.h
   文件实现功能: MCU业务网管交换函数
   作者        : zhangsh
   版本        : V2.0  Copyright(C) 2001-2002 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   2005/02/02  2.0         zhangsh        创建
******************************************************************************/


#ifdef WIN32
#else
//#include <vxWorks.h>
//#include <usrLib.h>
#endif

#include "mcuvc.h"
#include "evmcu.h"
#include "evnms.h"
#include "evmcumt.h"
#include "udpnms.h"
#include "evmcumcs.h"
#include "evmodem.h"

extern CMcuVcApp	g_cMcuVcApp;	    //MCU业务应用实例
extern CMpManager   g_cMpManager;       //MP管理类全局变量
extern u8           g_byMcuModemNum;


extern TMcuTopo	    g_atMcuTopo[MAXNUM_TOPO_MCU];
extern u8		    g_wMcuTopoNum;
extern TMtTopo		g_atMtTopo[MAXNUM_TOPO_MT];	//保存全网中MT拓扑信息
extern u16		    g_wMtTopoNum;		        //保存全网中MT数目


/*====================================================================
    函数名      :ProcApplyFreqTimeOut
    功能        :超时和申请失败处理
    算法实现    
    引用全局变量：
    输入参数说明：
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/12/19   1.0          zhangsh       创建
====================================================================*/
void CMcuVcInst::ProcApplyFreqTimeOut( const CMessage*pcMsg )
{
	//CServMsg cServMsg;
	//cServMsg.SetSrcSsnId( m_byMcSsnId );
	//cServMsg.SetSerialNO( m_dwMcSerialNo );
	//cServMsg.SetSrcMcu( LOCAL_MCUID /*g_cMcuAgent.GetId()*/ );

	switch( CurState() )
	{
	case STATE_WAITAUTH:
		//cServMsg.SetErrorCode(ERR_MCU_NOTGETFREQUENCE);
		//SendReplyBack( cServMsg, MCU_MCS_CREATECONF_NACK );

#ifndef WIN32
		NotifyMcsAlarmInfo(0, ERR_MCU_NOTGETFREQUENCE);
        ConfLog(FALSE, "[NMS]Apply Freq Time Out!\n");
        
		ReleaseConf( TRUE );
		NEXTSTATE(STATE_IDLE);
#endif
		break;
	default:
		log( LOGLVL_EXCEPTION, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
}

/*====================================================================
  函数名      ProcGetFreqFromNMS
  功能        网管申请回应处理
  算法实现    ：
  引用全局变量：
  输入参数说明：const CMessage * pcMsg, 传入的消息
  返回值说明  ：
  ----------------------------------------------------------------------
  修改记录    ：
  日  期      版本          修改人        修改内容
  03/01/03    1.0           zhangsh        创建
  09/08/28    4.6           张宝卿        从卫星版本移植过来
====================================================================*/
void CMcuVcInst::ProcGetFreqFromNMS( const CMessage * pcMsg )
{
	TFreqGetMsg tAck;

    memset(&tAck, 0, sizeof(tAck));

    if (pcMsg->event == NMS_MCU_APPLYCONFFREQ_NOTIF)
    {
        tAck = *(TFreqGetMsg*)pcMsg->content;
    }
	CServMsg cServMsg;
	
	u8 byNum;
	u8 byLoop;
	switch( CurState() )
	{
	case STATE_WAITAUTH:
	case STATE_WAITEQP:

	//FIXME: 稍后严格 保障状态机
	case STATE_ONGOING:

		KillTimer( MCUVC_APPLYFREQUENCE_CHECK_TIMER );

        if (pcMsg->event == MCUVC_APPLYFREQUENCE_CHECK_TIMER)
        {
#ifndef WIN32
            NotifyMcsAlarmInfo(0, ERR_MCU_NOTGETFREQUENCE);
            ReleaseConf( TRUE );
			NEXTSTATE(STATE_IDLE);
            ConfLog( FALSE, "[ProcGetFreqFromNMS] get freq from nms time out!\n" );
#endif
            return;
        }

		if ( tAck.GetBrdFreq() == 0 || tAck.GetNum() == 0 )
		{
			NotifyMcsAlarmInfo(0, ERR_MCU_NOTGETFREQUENCE);
			ReleaseConf( TRUE );
			NEXTSTATE(STATE_IDLE);
			ConfLog( FALSE, "[ProcGetFreqFromNMS] get freq from nms failed with BrdFreq.%d, FreqNum.%d)\n",
                       tAck.GetBrdFreq(), tAck.GetNum() );
			return;
		}
		
		g_cMcuVcApp.SetConfFreq( m_byConfIdx, tAck.GetBrdFreq() + 3000000, 0 ,0 );
        SatLog( "Get Brd Freq - %u, Rcv Freq Num - %u\n",tAck.GetBrdFreq(), tAck.GetNum() );

		g_cMcuVcApp.SetConfGetSendFreq( m_byConfIdx, TRUE );
        if ( NMSConvertFreq( tAck.GetBrdFreq() ) == 0 ) 
        {
            ConfLog( FALSE, "BAD Brd Freq - %u\n",tAck.GetBrdFreq() );

            NotifyMcsAlarmInfo(0, ERR_MCU_NOTGETFREQUENCE);
            ReleaseConf( TRUE );
			NEXTSTATE(STATE_IDLE);
            ConfLog( FALSE, "[ProcGetFreqFromNMS] Conf had to be release due to BAD Brd Freq - %u\n",tAck.GetBrdFreq() );

			return;
        }
		

        //FIXME: 最大5条回传上星
		byNum = m_tConf.GetConfAttrbEx().GetSatDCastChnlNum();
		if ( byNum > 5 )
        {
			byNum = 5;
        }

		g_cMcuVcApp.SetConfRcvNum( m_byConfIdx, byNum );
		for ( byLoop = 0 ; byLoop < 5; byLoop ++ )
		{

			//if (byLoop != 1)
			{
				g_cMcuVcApp.SetConfFreq( m_byConfIdx,
					                     0,
										 byLoop,
										 tAck.GetContent(byLoop+1) + 3000000 + 1000000 + 1000000 * byLoop );
				
				SatLog( "[ProcGetFreqFromNMS] Get Receive Num - %u Freq - %u\n",
							byLoop + 1,
							tAck.GetContent(byLoop+1) + 3000000 + 1000000 + 1000000 * byLoop );
			}
			/*
			else
			{
				g_cMcuVcApp.SetConfFreq( m_byConfIdx,
					                     0,
										 byLoop,
										 tAck.GetContent(byLoop+1) + 4000000 + 2000000 + 1000000 + 1000000 * byLoop );

				SatLog( "[ProcGetFreqFromNMS] Get Receive Num - %u Freq - %u\n",
							byLoop + 1,
							tAck.GetContent(byLoop+1) + 4000000 + 2000000 + 1000000 + 1000000 * byLoop );
				
			}
			*/

			g_cMcuVcApp.SetConfGetReceiveFreq( m_byConfIdx, TRUE );

            if ( byLoop < byNum && NMSConvertFreq( tAck.GetContent( byLoop+1 ) ) == 0 ) 
            {
                NotifyMcsAlarmInfo(0, ERR_MCU_NOTGETFREQUENCE);
                ReleaseConf( TRUE );
				NEXTSTATE(STATE_IDLE);
                ConfLog( FALSE, "[ProcGetFreqFromNMS] Conf had to be release due to BAD Rcv Freq - %u\n",tAck.GetContent( byLoop+1 ) );
                
                return;
            }
		}

        if (STATE_ONGOING == CurState())
        {
            StartConfModem(byNum);
        }

        //FIXME: 卫星会议下 及时会议和预约会议的区分处理
		//if ( m_bSchedStart )
		{
			OspPost(MAKEIID(AID_MCU_VC,GetInsID()), MT_MCU_CREATECONF_REQ,cServMsg.GetServMsg(),cServMsg.GetServMsgLen() );
		}
        /*
        else
        {
			OspPost(MAKEIID(AID_MCU_VC,GetInsID()),MCS_MCU_CREATECONF_REQ,cServMsg.GetServMsg(),cServMsg.GetServMsgLen() );
		}*/
		break;
	default:
		ConfLog( FALSE, "[ProcGetFreqFromNMS] Wrong message %u(%s) received in state %u!\n", 
			            pcMsg->event,
                        ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
	return;
}

/*====================================================================
函数名      :SendCmdToNms
功能        :组合消息通过DAEMON发送
算法实现    
引用全局变量：
输入参数说明：
返回值说明  ：
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
03/12/19   1.0          zhangsh         创建
09/08/28   4.6          张宝卿          从卫星版本移植过来
====================================================================*/
u32 CMcuVcInst::SendCmdToNms( u16   wCmd,
                              u8    byBrdNum,
                              u32   dwBrdBit,
                              u16   wNum,
                              u32   dwBitRate,
                              u32   dwFreq[5], u32 dwBrdFreq)
{
    
    CConfId cConfId;
    cConfId.SetNull();
    u8 byLoop;
    CServMsg cServMsg;
    cConfId = m_tConf.GetConfId();
    u32 dwSn;
    dwSn = GetSerialId();
    switch( wCmd ) 
    {
    case MCU_NMS_APPLYCONFFREQ_REQ:
        {
            TMcuApplyFreq tReq;
            memset( &tReq, 0, sizeof(TMcuApplyFreq) );
            tReq.SetMessageType( MCU_NMS_APPLYCONFFREQ_REQ );
            tReq.SetBrdBitRate( dwBrdBit );
            tReq.SetRcvNum( wNum );
            tReq.SetRcvBitRate( dwBitRate );
            tReq.SetConfId( cConfId );
            tReq.SetSN( dwSn );
            cServMsg.SetMsgBody( (u8*)&tReq, sizeof( TMcuApplyFreq ) );
            cServMsg.SetConfIdx( m_byConfIdx );
            g_cMcuVcApp.SendMsgToDaemonConf( MCU_NMS_SENDNMSMSG_CMD, cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );
        }
        break;
    case MCU_NMS_RELEASECONFFREQ_REQ:
        {
            TMcuReleaseFreq  tReqFree;
            memset( &tReqFree, 0, sizeof(TMcuReleaseFreq) );
            tReqFree.SetCmd( MCU_NMS_RELEASECONFFREQ_REQ );
            tReqFree.SetBrdBitRate( dwBrdBit );
            tReqFree.SetBrdFreq( dwBrdFreq );
            SatLog("Release Conf BrdBit -%u BrdFrq - %u \n", dwBrdBit, dwBrdFreq );
            tReqFree.SetRcvBitRate( dwBitRate );
            tReqFree.SetNum( wNum );
            SatLog("Release Conf UnicastBit -%u Num - %u \n", dwBitRate, wNum );
            tReqFree.SetSN( dwSn );
            for ( byLoop = 0; byLoop < wNum; byLoop ++ )
            {
                SatLog("Release Conf Freq -%u \n", dwFreq[byLoop]);
                tReqFree.SetFreeFreq( byLoop+1, dwFreq[byLoop] );
            }
            cServMsg.SetMsgBody( (u8*)&tReqFree, sizeof( TMcuReleaseFreq ) );
            cServMsg.SetConfIdx( m_byConfIdx );
            g_cMcuVcApp.SendMsgToDaemonConf( MCU_NMS_SENDNMSMSG_CMD, cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );
        }
        break;
    default:
        return 0;
    }
    return dwSn;
}

/*====================================================================
 函数名      ReleaseConfFreq 
 功能        释放网管的频率
 算法实现    ：
 引用全局变量：
 输入参数说明：const CMessage * pcMsg, 传入的消息
 返回值说明  ：
 ----------------------------------------------------------------------
 修改记录    ：
 日  期      版本        修改人        修改内容
 03/01/03    1.0          zhangsh       创建
 09/08/28    4.6          张宝卿        从卫星版本移植过来s
====================================================================*/
void CMcuVcInst::ReleaseConfFreq( )
{
	u32 dwBrdBitRate;
	u32 dwRcvBitRate;
	u32 dwBrdFreq;
	u32 dwFreq[5];
	TMcuReleaseFreq  tReqFree;
	CServMsg cServMsg;
	u32 dwSn;
	dwSn = GetSerialId();
	switch( CurState() )
	{
	case STATE_IDLE:
    case STATE_ONGOING:
    case STATE_WAITEQP:

		memset( &tReqFree, 0, sizeof(TMcuReleaseFreq) );
		tReqFree.SetCmd( MCU_NMS_RELEASECONFFREQ_REQ );

		//广播的
		if ( g_cMcuVcApp.IsConfGetSendFreq( m_byConfIdx ) )
		{
			dwBrdBitRate = g_cMcuVcApp.GetConfSndBitRate( m_byConfIdx );
			dwBrdFreq = g_cMcuVcApp.GetConfSndFreq( m_byConfIdx );
			g_cMcuVcApp.SetConfGetSendFreq( m_byConfIdx, FALSE );
		}
		tReqFree.SetBrdBitRate( dwBrdBitRate );
		tReqFree.SetBrdFreq( dwBrdFreq );
		
        ConfLog( FALSE, "释放:广播码率 - %u 广播频点 - %u\n",dwBrdBitRate, dwBrdFreq );
		
        //接收的
		if ( g_cMcuVcApp.IsConfGetReceiveFreq( m_byConfIdx ) )
		{
			dwRcvBitRate = g_cMcuVcApp.GetConfRcvBitRate( m_byConfIdx );
			for ( u8 byNum = 0; byNum < m_tConf.GetConfAttrbEx().GetSatDCastChnlNum(); byNum ++)
			{
				dwFreq[byNum] = g_cMcuVcApp.GetConfRcvFreq( m_byConfIdx, byNum );
                
                ConfLog( FALSE, "释放:单播频点 - %u\n",dwFreq[byNum] );

				tReqFree.SetFreeFreq( byNum+1, dwFreq[byNum] );
			}
			g_cMcuVcApp.SetConfGetReceiveFreq( m_byConfIdx, FALSE );
		}
		g_cMcuVcApp.ReleaseConfInfo( m_byConfIdx );
		

		tReqFree.SetRcvBitRate( dwRcvBitRate );
		tReqFree.SetNum( m_tConf.GetConfAttrbEx().GetSatDCastChnlNum() );
		tReqFree.SetSN( dwSn );
		cServMsg.SetMsgBody( (u8*)&tReqFree, sizeof( TMcuReleaseFreq ) );
		cServMsg.SetConfIdx( m_byConfIdx );
		g_cMcuVcApp.SendMsgToDaemonConf( MCU_NMS_SENDNMSMSG_CMD, cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );
		
		//SendCmdToNms( MCU_NMS_RELEASECONFFREQ_REQ, 1, dwBrdBitRate,m_tConf.GetConfAttrb().GetSatDCastChnlNum(), dwRcvBitRate, dwFreq,dwBrdFreq );
		break;
	default:
		log( LOGLVL_EXCEPTION, "CMcuVcInst: Wrong release freq message received in state %u!\n", CurState() );
		break;
	}
	return;
}

/*====================================================================
    函数名      :ProcDelayTimeOut
    功能        :会议的时间更改超时
    算法实现    
    引用全局变量：
    输入参数说明：
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    05/02/02   1.0          zhangsh       创建
====================================================================*/
void CMcuVcInst::ProcDelayTimeOut( const CMessage* pcMsg )
{
	CServMsg cServMsg;
	//cServMsg.SetSrcSsnId( m_byMcSsnId );
	//cServMsg.SetSerialNO( m_dwMcSerialNo );
	//cServMsg.SetSrcMcu( LOCAL_MCUID /*g_cMcuAgent.GetId()*/ );
	cServMsg.SetConfId( m_tConf.GetConfId() );
	
	switch( CurState() )
	{
	case STATE_SCHEDULED:
		
        //延长会议,添加终端
		KillTimer(TIMER_DELAYCONF);
		
        if ( !m_bSchedStart )
		{
			cServMsg.SetMsgBody( ( u8 * const )&m_tConf, sizeof( m_tConf ) );
			cServMsg.SetConfId( m_tConf.GetConfId() );
			
            /*
			//Notification
			SendMsgToAllMcs( MCU_MCS_CONF_NOTIF, 
				                             cServMsg.GetServMsg(),
                                             cServMsg.GetServMsgLen() );*/
            ConfLog( FALSE,"[NMS]Modify time timeout!\n");
		}
		else
		{
			//要修改
			cServMsg.SetErrorCode( ERR_MCU_SCHEDTIMEROUT );
			SendMsgToMcs( cServMsg.GetSrcSsnId(),
                          MCU_MCS_MODIFYCONF_NACK, cServMsg );
            
            ConfLog( FALSE, "[NMS]Modify time timeout!\n");
		}
		break;
	case STATE_MASTER:

        //延长会议,添加终端
		KillTimer(TIMER_DELAYCONF);
		cServMsg.SetErrorCode( ERR_MCU_NMSTIMEROUT );
		SendReplyBack( cServMsg, MCU_MCS_DELAYCONF_NACK );
        ConfLog( FALSE, "[NMS]Modify time timeout!\n");
		break;

    default:
		log( LOGLVL_EXCEPTION, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}

}
/*====================================================================
    函数名      :ProcNmsMcuReshedconfNotif
    功能        :会议的时间更改
    算法实现    
    引用全局变量：
    输入参数说明：
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    05/02/02   1.0          zhangsh       创建
====================================================================*/
void CMcuVcInst::ProcNmsMcuReshedconfNotif(  const CMessage*pcMsg )
{
	TReSchConfNotif tSchConfNotif = *(TReSchConfNotif*)pcMsg->content;
	CServMsg cServMsg;
	//cServMsg.SetSrcSsnId( m_byMcSsnId );
	//cServMsg.SetSerialNO( m_dwMcSerialNo );
	//cServMsg.SetSrcMcu( LOCAL_MCUID /*g_cMcuAgent.GetId()*/ );
	cServMsg.SetConfId( m_tConf.GetConfId() );
	//提示用户
	switch( CurState() )
	{
	case STATE_SCHEDULED://重新安排时间,手工立即开始
		
        if ( !m_bSchedStart )
		{
			KillTimer(TIMER_DELAYCONF);
			if ( tSchConfNotif.GetMtNum() == 0 )
			{
				//Ack
                SatLog( "[NMS]Modify time ok!\n");

                //FIXME: 预约会议通过网管修改时间


                /*
				SendMsgToMcs( cServMsg.GetSrcSsnId(), MCU_MCS_MODIFYCONF_ACK, cServMsg );
				m_tConf.SetStartTime( m_dwOldStart );
				m_tConf.SetDuration( m_dwOldDuration );
				cServMsg.SetMsgBody( ( u8*)&m_tConf, sizeof( m_tConf ) );
				cServMsg.SetConfId( m_tConf.GetConfId() );
				g_cMcuVcApp.SendMsgToConf( m_tConf.GetConfId(), MCU_MCS_CONF_NOTIF, cServMsg.GetServMsg(), 
					cServMsg.GetServMsgLen() );
				SendMsgToAllMcs( MCU_MCS_CONF_NOTIF, cServMsg );
				::AddConfInfoToFile( m_tConf );*/
			}
			else
			{
                ConfLog( FALSE, "[NMS]Modify time nak!\n");
				cServMsg.SetErrorCode( ERR_MCU_NMSNACK );
				SendMsgToMcs( cServMsg.GetSrcSsnId(), MCU_MCS_MODIFYCONF_NACK, cServMsg );
			}
		}
		else
		{
			KillTimer(TIMER_RESCHED);
			if ( tSchConfNotif.GetMtNum() == 0 )
			{
                SatLog( "[NMS]begin now!\n");
				m_tConf.SetStartTime( time(NULL) );
				m_tConf.SetDuration( m_dwOldDuration );
				SendMsgToMcs( cServMsg.GetSrcSsnId(), MCU_MCS_MODIFYCONF_ACK, cServMsg );
				cServMsg.SetSrcSsnId( 0 );		//do not need reply
				cServMsg.SetMsgBody( ( u8 * const )&m_tConf, sizeof( m_tConf ) );
				
                //FIXME: 暂时放弃面向网管的状态机保护
                //NEXTSTATE( STATE_WAITAUTH );
				post( MAKEIID( AID_MCU_VC,GetInsID() ), MCU_SCHEDULE_CONF_START, cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );
			}
			else
			{
				m_bSchedStart = FALSE;

                SatLog( "[NMS]Not allow begin now!\n");
				cServMsg.SetErrorCode( ERR_MCU_NMSNACK );
				SendMsgToMcs( cServMsg.GetSrcSsnId(), MCU_MCS_MODIFYCONF_NACK, cServMsg );
			}
		}
		break;
	case STATE_MASTER://只有延长会议才到此处
		
        KillTimer(TIMER_DELAYCONF);
		if ( tSchConfNotif.GetMtNum() == 0 )
		{
            SatLog( "[NMS]conf delay ok!\n");
			m_dwOldStart = m_tConf.GetDuration() + ntohs( m_dwOldStart );
			m_tConf.SetDuration( m_dwOldStart );
			SendReplyBack( cServMsg, MCU_MCS_DELAYCONF_ACK );
			cServMsg.SetMsgBody( ( u8 * const )&m_tConf, sizeof( m_tConf ) );
			cServMsg.SetConfId( m_tConf.GetConfId() );
			
            //Notification
			//SendMsgToAllMcs( MCU_MCS_CONF_NOTIF, cServMsg );
		}
		else
		{
            ConfLog( FALSE, "[NMS]conf delay nak!\n");
			cServMsg.SetErrorCode( ERR_MCU_NMSNACK );
			SendReplyBack( cServMsg, MCU_MCS_DELAYCONF_NACK );			
		}
		break;
	default:
		log( LOGLVL_EXCEPTION, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
}
/*====================================================================
    函数名      :ProcNmsAddMt
    功能        :增加终端
    算法实现    
    引用全局变量：
    输入参数说明：
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    05/02/02   1.0          zhangsh       创建
====================================================================*/
void CMcuVcInst::ProcNmsAddMt( const CMessage*pcMsg )
{
	TMtOperateNotif tMtOperateNotif = *(TMtOperateNotif*)pcMsg->content;
	CServMsg cServMsg;
	//cServMsg.SetSrcSsnId( m_byMcSsnId );
	//cServMsg.SetSerialNO( m_dwMcSerialNo );
	//cServMsg.SetSrcMcu( LOCAL_MCUID /*g_cMcuAgent.GetId()*/ );
	cServMsg.SetConfId( m_tConf.GetConfId() );
	TMt tMt;
	u32* pNackIp;
	u32  dwMtIp;

	//提示用户
	switch( CurState() )
	{
	case STATE_SCHEDULED:
		KillTimer(TIMER_ADDMT);
		SendReplyBack( cServMsg,MCU_MCS_MODIFYCONF_ACK );

		//将终端删除
		if ( tMtOperateNotif.GetMtNum() != 0 )
		{
			pNackIp = (u32*)(pcMsg->content+sizeof(TSchConfNotif));
			for ( u16 wDelMt = 0 ; wDelMt < tMtOperateNotif.GetMtNum() ; wDelMt ++ )
			{
				for ( u16 byMtLoop = 1; byMtLoop < MAXNUM_MCU_MT; byMtLoop ++ )
				{
					if ( m_tConfAllMtInfo.MtInConf( byMtLoop ) )
					{
						dwMtIp = ::topoGetMtInfo( LOCAL_MCUID /*g_cMcuAgent.GetId()*/,
                                                  byMtLoop,
                                                  g_atMtTopo,
                                                  g_wMtTopoNum ).GetIpAddr();

						dwMtIp = htonl( dwMtIp );
						if ( *pNackIp == dwMtIp )
						{
                            TMt tMt = m_ptMtTable->GetMt(byMtLoop);
							RemoveMt( tMt, TRUE );
						}
					}
				}
				pNackIp ++;
			}			
		}
		cServMsg.SetMsgBody( ( u8 * const )&m_tConf, sizeof( m_tConf ) );
		cServMsg.SetConfId( m_tConf.GetConfId() );
		
        //Notification
		//SendMsgToAllMcs( MCU_MCS_CONF_NOTIF, cServMsg );
        //::AddConfInfoToFile(m_tConf);
		break;

	case STATE_MASTER:

		//即时会议中根据返回将终端加入,每次只有一个终端
		KillTimer(TIMER_ADDMT);
		if ( tMtOperateNotif.GetMtNum() == 0 )
		{
			tMt.SetMt( LOCAL_MCUID /*g_cMcuAgent.GetId()*/, m_wOpMtId );
			cServMsg.SetEventId( MCS_MCU_ADDMT_REQ );
			cServMsg.SetMsgBody( (u8*)&tMt, sizeof(TMt) );
			SendReplyBack( cServMsg, MCU_MCS_ADDMT_ACK );
			
            //Invite it
			InviteUnjoinedMt( cServMsg, &tMt );
			SetTimer( TIMER_INVITE_UNJOINEDMT, 1000 );
		}
		else
		{
			cServMsg.SetErrorCode( ERR_MCU_NMSNACK );
			SendReplyBack( cServMsg, MCU_MCS_ADDMT_NACK );

            TMt tMt = m_ptMtTable->GetMt(m_wOpMtId);
			RemoveMt( tMt, TRUE );
		}
		m_wOpMtId = 0;
		break;

	default:
		log( LOGLVL_EXCEPTION, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}	
}
/*====================================================================
    函数名      :ProcNmsDelMt
    功能        :增加终端
    算法实现    
    引用全局变量：
    输入参数说明：
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    05/02/02   1.0          zhangsh       创建
====================================================================*/
void CMcuVcInst::ProcNmsDelMt( const CMessage*pcMsg )
{
	TMtOperateNotif tMtOperateNotif = *(TMtOperateNotif*)pcMsg->content;
	CServMsg cServMsg;
	//cServMsg.SetSrcSsnId( m_byMcSsnId );
	//cServMsg.SetSerialNO( m_dwMcSerialNo );
	//cServMsg.SetSrcMcu( LOCAL_MCUID /*g_cMcuAgent.GetId()*/ );
	cServMsg.SetConfId( m_tConf.GetConfId() );
	TMt tMt = *(TMt*)cServMsg.GetMsgBody();
	
    //提示用户
	switch( CurState() )
	{
	case STATE_SCHEDULED:
		
        KillTimer(TIMER_DELMT);
		if ( tMtOperateNotif.GetMtNum() == 0 )
		{
			//Ack
			SendReplyBack( cServMsg, MCU_MCS_MODIFYCONF_ACK );
		}
		cServMsg.SetMsgBody( ( u8 * const )&m_tConf, sizeof( m_tConf ) );
		cServMsg.SetConfId( m_tConf.GetConfId() );
		
        //Notification
		//SendMsgToAllMcs( MCU_MCS_CONF_NOTIF, cServMsg );
		//::AddConfInfoToFile(m_tConf);
		break;

	case STATE_MASTER:
		//即时会议肯定可以删除的
		KillTimer(TIMER_DELMT);
		break;
	default:
		log( LOGLVL_EXCEPTION, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}	
}
/*====================================================================
    函数名      :ProcNmsAddMt
    功能        :增加终端
    算法实现    
    引用全局变量：
    输入参数说明：
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    05/02/02   1.0          zhangsh       创建
====================================================================*/
void CMcuVcInst::ProcNmsAddMtOut( const CMessage* pcMsg )
{
	CServMsg cServMsg;
	cServMsg.SetErrorCode(ERR_MCU_NMSTIMEROUT);
	//cServMsg.SetSrcSsnId( m_byMcSsnId );
	//cServMsg.SetSerialNO( m_dwMcSerialNo );
	//cServMsg.SetSrcMcu( LOCAL_MCUID /*g_cMcuAgent.GetId()*/ );
	cServMsg.SetConfId( m_tConf.GetConfId() );
	switch( CurState() )
	{
	case STATE_SCHEDULED:
		KillTimer( TIMER_ADDMT );
		SendReplyBack( cServMsg, MCU_MCS_MODIFYCONF_NACK );
		//专门安排一条消息来反应超时
		break;
	case STATE_MASTER:
		KillTimer( TIMER_ADDMT );
		m_wOpMtId = 0;
		SendReplyBack( cServMsg, MCU_MCS_ADDMT_NACK );
		break;
	default:
		log( LOGLVL_EXCEPTION, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}

}
/*====================================================================
    函数名      :ProcNmsDelMtOut
    功能        :增加终端
    算法实现    
    引用全局变量：
    输入参数说明：
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    05/02/02   1.0          zhangsh       创建
====================================================================*/
void CMcuVcInst::ProcNmsDelMtOut( const CMessage* pcMsg )
{
	CServMsg cServMsg;
	//cServMsg.SetSrcSsnId( m_byMcSsnId );
	//cServMsg.SetSerialNO( m_dwMcSerialNo );
	//cServMsg.SetConfId( m_tConf.GetConfId() );
	//cServMsg.SetSrcMcu( LOCAL_MCUID /*g_cMcuAgent.GetId()*/ );
	switch( CurState() )
	{
	case STATE_SCHEDULED:
		KillTimer( TIMER_DELMT );
		break;
	case STATE_MASTER:
		//早就删除了
		KillTimer( TIMER_DELMT );
		break;
	default:
		log( LOGLVL_EXCEPTION, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
}
/*====================================================================
    函数名      : ProcSchedConfTimeOut
    功能        :
    算法实现    
    引用全局变量：
    输入参数说明：
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    05/02/02   1.0          zhangsh       创建
====================================================================*/
void CMcuVcInst::ProcSchedConfTimeOut( const CMessage * pcMsg )
{
	CServMsg cServMsg;
	//cServMsg.SetSerialNO( m_dwMcSerialNo );
	//cServMsg.SetSrcSsnId( m_byMcSsnId );
	//cServMsg.SetSrcMcu( LOCAL_MCUID /*g_cMcuAgent.GetId()*/ );

	switch( CurState() )
	{
	case STATE_SCHEDULED:
    case STATE_WAITAUTH:

    //FIXME：后续考虑卫星会议的状态严格翻转串行处理
    case STATE_ONGOING:
		
        KillTimer( TIMER_SCHEDCONF );
		//cServMsg.SetErrorCode( ERR_MCU_SCHEDTIMEROUT );
		//SendMsgToMcs( cServMsg.GetSrcSsnId(), MCU_MCS_CREATECONF_NACK, cServMsg );
		
        //ClearVcInst();

#ifndef WIN32
		NotifyMcsAlarmInfo( 0, ERR_MCU_SCHEDTIMEROUT );
        ReleaseConf( TRUE );	
		NEXTSTATE(STATE_IDLE);
		
        ConfLog(FALSE, "[ProcSchedConfTimeOut] release conf!\n");
		NEXTSTATE( STATE_IDLE );
#endif
		break;

    default:
		ConfLog(FALSE, "[ProcSchedConfTimeOut] Wrong message %u(%s) received in state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
}
/*====================================================================
    函数名      :ProcSchedStartNow
    功能        :
    算法实现    
    引用全局变量：
    输入参数说明：
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    05/02/02   1.0          zhangsh       创建
====================================================================*/
void CMcuVcInst::ProcSchedStartNow( const CMessage* pcMsg )
{
	CServMsg cServMsg;
	//cServMsg.SetSerialNO( m_dwMcSerialNo );
	//cServMsg.SetSrcSsnId( m_byMcSsnId );
	//cServMsg.SetSrcMcu( LOCAL_MCUID /*g_cMcuAgent.GetId()*/ );
	
	switch( CurState() )
	{
	case STATE_SCHEDULED:
		
        KillTimer( TIMER_RESCHED );
		
        NotifyMcsAlarmInfo( 0, ERR_MCU_SCHEDTIMEROUT );
        ReleaseConf( TRUE );	
		NEXTSTATE(STATE_IDLE);
        ConfLog(FALSE, "[ProcSchedStartNow] release conf!\n");

        NEXTSTATE( STATE_IDLE );
        break;
	default:
		log( LOGLVL_EXCEPTION, "CMcuVcInst: Wrong message %u(%s) received in state %u!\n", 
			                    pcMsg->event, 
                                ::OspEventDesc( pcMsg->event ),
                                CurState() );
		break;
	}
}



/*====================================================================
    函数名      StartConfModem
    功能        MCU侧MODEM设置
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/11/11   1.0          zhangsh       创建
====================================================================*/
BOOL CMcuVcInst::StartConfModem( u8 byNum)
{
	CServMsg cServMsg;
	cServMsg.SetConfId( m_tConf.GetConfId() );
	cServMsg.SetConfIdx( m_byConfIdx );
	u8 byModemId;
	TMt tMt;
	u8 byLoop;
	u32 RecMode;
	u32 SendMode;
	u32 dwSendFreq, dwRevFreq;
	u32 dwSendBitRate, dwRevBitRate;
	u8 byMode;
	if ( byNum == 0 )
    {
        ConfLog(FALSE, "[StartConfModem] ModemNum.%d unexpected, ignore it!\n", byNum);
		return TRUE;
    }
	
	memset( (u8*)&tMt, 0, sizeof(TMt) );
	
    //得到一个可以发送的MODEM
	byModemId = g_cMcuVcApp.GetIdleMcuModemForSnd();
	m_byConfSPortNum = g_cMcuVcApp.GetModemSportNum(byModemId);
	
    //接收从
	SendMode = DEM_CLKSRC_INTER;
	cServMsg.SetMsgBody( (u8*)&tMt, sizeof(TMt) );
	cServMsg.CatMsgBody( (u8*)&SendMode, sizeof(u32) );
	
    //发送从
	RecMode = MOD_CLKSRC_INTER;
	cServMsg.CatMsgBody( (u8*)&RecMode, sizeof(u32) );
	g_cModemSsnApp.SendMsgToModemSsn( byModemId, MCU_MODEM_SETSRC, cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );
	
    //发送频率
	dwSendFreq = g_cMcuVcApp.GetConfSndFreq( m_byConfIdx );
	if ( dwSendFreq == 0 )
	{
		ConfLog(FALSE, "[StartConfModem] Cant get free Freq!\n" );
		return FALSE;
	}

	//发送带宽：双格式会议发送带宽加倍
	//这个 统一在频率申请在做处理
// 	if (MEDIA_TYPE_NULL != m_tConf.GetSecVideoMediaType())
// 	{
// 		dwSendBitRate = g_cMcuVcApp.GetConfSndBitRate( m_byConfIdx ) * 2;
// 	}
// 	else
// 	{
		dwSendBitRate = g_cMcuVcApp.GetConfSndBitRate( m_byConfIdx );
//	}

	//设置比特率
	cServMsg.SetMsgBody( (u8*)&tMt, sizeof(TMt) );
	cServMsg.CatMsgBody( (u8*)&dwSendBitRate, sizeof(u32) );
	byMode = 1;
	cServMsg.CatMsgBody( (u8*)&byMode, sizeof(u8) );
	g_cModemSsnApp.SendMsgToModemSsn( byModemId, MCU_MODEM_SETBITRATE, cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );
	
    //进入发送
	cServMsg.SetMsgBody( (u8*)&tMt, sizeof(TMt) );
	cServMsg.CatMsgBody( (u8*)&dwSendFreq,sizeof(u32) );
	g_cModemSsnApp.SendMsgToModemSsn( byModemId, MCU_MODEM_SEND, cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );
	
    //保存数据
	g_cMcuVcApp.SetMcuModemSndData( byModemId, m_byConfIdx, dwSendFreq, dwSendBitRate);

//#if 0
	//设置比特率
	dwRevBitRate = g_cMcuVcApp.GetConfRcvBitRate( m_byConfIdx );
	cServMsg.SetMsgBody( (u8*)&tMt, sizeof(TMt) );
	cServMsg.CatMsgBody( (u8*)&dwRevBitRate, sizeof(u32) );
	byMode = 2;
	cServMsg.CatMsgBody( (u8*)&byMode, sizeof(u8) );
	g_cModemSsnApp.SendMsgToModemSsn( byModemId, MCU_MODEM_SETBITRATE, cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );
	
    //进入接收
	dwRevFreq = g_cMcuVcApp.GetConfRcvFreq( m_byConfIdx, 0 );
	cServMsg.SetMsgBody( (u8*)&tMt, sizeof(TMt) );
	cServMsg.CatMsgBody( (u8*)&dwRevFreq, sizeof(u32) );
	g_cModemSsnApp.SendMsgToModemSsn( byModemId, MCU_MODEM_RECEIVE, cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );
	//g_cMcuVcApp.SetMcuModemData( byModemId, m_byConfIdx, dwSendFreq, dwRevFreq, dwSendBitRate, dwRevBitRate );
	
    //剩余的收
	byNum = g_cMcuVcApp.GetConfRcvNum( m_byConfIdx ) - 1;
//#endif

    dwRevBitRate = g_cMcuVcApp.GetConfRcvBitRate( m_byConfIdx );
	byNum = g_cMcuVcApp.GetConfRcvNum( m_byConfIdx );
	for ( byLoop = 1 ; byLoop <= byNum && byLoop <= g_byMcuModemNum ; byLoop ++ )
	{
		byModemId = g_cMcuVcApp.GetIdleMcuModemForRcv();
		
		//接收从
		SendMode = DEM_CLKSRC_INTER;
		cServMsg.SetMsgBody( (u8*)&tMt, sizeof(TMt) );
		cServMsg.CatMsgBody( (u8*)&SendMode, sizeof(u32) );
		
		//发送从
		SendMode = MOD_CLKSRC_INTER;
		cServMsg.CatMsgBody( (u8*)&SendMode, sizeof(u32) );
		g_cModemSsnApp.SendMsgToModemSsn( byModemId, MCU_MODEM_SETSRC, cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );
		
		//设置比特率
		cServMsg.SetMsgBody( (u8*)&tMt, sizeof(TMt) );
		cServMsg.CatMsgBody( (u8*)&dwRevBitRate, sizeof(u32) );
		byMode = 2;
		cServMsg.CatMsgBody( (u8*)&byMode, sizeof(u8) );
		g_cModemSsnApp.SendMsgToModemSsn( byModemId, MCU_MODEM_SETBITRATE, cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );
		
		//进入接收
		dwRevFreq = g_cMcuVcApp.GetConfRcvFreq( m_byConfIdx, byLoop-1 );
		cServMsg.SetMsgBody( (u8*)&tMt, sizeof(TMt) );
		cServMsg.CatMsgBody( (u8*)&dwRevFreq, sizeof(u32) );
		g_cModemSsnApp.SendMsgToModemSsn( byModemId, MCU_MODEM_RECEIVE, cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );
		g_cMcuVcApp.SetMcuModemRcvData( byModemId, m_byConfIdx, dwRevFreq, dwRevBitRate );
	}
	return TRUE;
}

/*====================================================================
函数名      StopConfModem
功能        MCU侧MODEM释放
算法实现    ：
引用全局变量：
输入参数说明：
返回值说明  ：
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
03/11/11   1.0          zhangsh       创建
====================================================================*/
BOOL CMcuVcInst::StopConfModem( u8 byNum )
{
	CServMsg cServMsg;
	cServMsg.SetConfId( m_tConf.GetConfId() );
	cServMsg.SetConfIdx( m_byConfIdx );
	u8 byModemId;
	
	if ( byNum == 0 )
		return TRUE;
	
	for ( u8 byLoop = 1 ; byLoop <= 17 ; byLoop ++ )
	{
		byModemId = g_cMcuVcApp.GetConfRcvModem( m_byConfIdx );
		if ( byModemId != 0 )
		{	
			ConfLog(FALSE, "Now we Release Mcu Receive Modem-%u\n", byModemId );
			g_cModemSsnApp.SendMsgToModemSsn( byModemId, MCU_MODEM_RELEASE, cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );
			g_cMcuVcApp.ReleaseRcvMcuModem( byModemId );
		}

		byModemId = g_cMcuVcApp.GetConfSndModem( m_byConfIdx );
		if ( byModemId != 0 )
		{	
			ConfLog( FALSE, "Now we Release Mcu Send Modem-%u\n", byModemId );
			g_cModemSsnApp.SendMsgToModemSsn( byModemId, MCU_MODEM_RELEASE, cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );
			g_cMcuVcApp.ReleaseSndMcuModem( byModemId );
		}
	}	
	return TRUE;
}

//END OF FILE

