 /*****************************************************************************
   ģ����      : Board Agent
   �ļ���      : imtagent.cpp
   ����ļ�    : 
   �ļ�ʵ�ֹ���: �������ʵ�֣���ɸ澯��������MANAGER�Ľ���
   ����        : jianghy
   �汾        : V0.9  Copyright(C) 2001-2002 KDC, All rights reserved.
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���      �޸�����
   2003/08/25  1.0         jianghy       ����
   2004/12/05  3.5         �� ��        �½ӿڵ��޸�
   2006/04/18  4.0         john         ����������������
******************************************************************************/
#include "osp.h"
#include "evagtsvc.h"
#include "imt_agent.h"
#include "eqplib.h"

#ifdef _VXWORKS_
#include "timers.h"
#include <dirent.h>
#endif


extern SEMHANDLE g_semImt;

CBrdAgentApp	g_cBrdAgentApp;	//�������Ӧ��ʵ��

// ���캯��
CBoardAgent::CBoardAgent()
{
	return;
}

//��������
CBoardAgent::~CBoardAgent()
{
	return;
}

/*====================================================================
    ������      ��InstanceExit
    ����        ��ʵ���˳�����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/08/20    1.0         jianghy       ����
====================================================================*/
void CBoardAgent::InstanceExit()
{
}

/*====================================================================
    ������      ��InstanceEntry
    ����        ��ʵ����Ϣ������ں���������override
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����CMessage * const pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/08/20    1.0         jianghy       ����
====================================================================*/
void CBoardAgent::InstanceEntry(CMessage* const pcMsg)
{
	if( NULL == pcMsg )
	{
		OspPrintf( TRUE, FALSE, "[Imt]: The msg's pointer is NULL(InstanceEntry)!");
		return;
	}

	ImtLog("%u(%s) passed!\n", pcMsg->event, ::OspEventDesc(pcMsg->event));

	switch( pcMsg->event )
	{
// 	case OSP_POWERON:            //��������
// 	case BRDAGENT_CONNECT_MANAGERA_TIMER:          //���ӹ�������ʱ
//     case BRDAGENT_CONNECT_MANAGERB_TIMER:          //���ӹ�������ʱ
// 	case BRDAGENT_REGISTERA_TIMER:		         //ע��ʱ�䳬ʱ
//     case BRDAGENT_REGISTERB_TIMER:		         //ע��ʱ�䳬ʱ
// 	case MPC_BOARD_REG_ACK:              //ע��Ӧ����Ϣ
// 	case MPC_BOARD_REG_NACK:			 //ע���Ӧ����Ϣ
// 	case BRDAGENT_GET_CONFIG_TIMER:               //�ȴ�����Ӧ����Ϣ��ʱ
// 	case MPC_BOARD_ALARM_SYNC_REQ:       //��������ĸ澯ͬ������
// 	case MPC_BOARD_BIT_ERROR_TEST_CMD:   //���������������
// 	case MPC_BOARD_TIME_SYNC_CMD:        //����ʱ��ͬ������
// 	case MPC_BOARD_SELF_TEST_CMD:        //�����Բ�����
// 	case MPC_BOARD_RESET_CMD:            //��������������
// 	case MPC_BOARD_UPDATE_SOFTWARE_CMD:  //������������
// 	case MPC_BOARD_GET_STATISTICS_REQ:   //��ȡ�����ͳ����Ϣ
// 	case MPC_BOARD_GET_VERSION_REQ:      //��ȡ����İ汾��Ϣ
// 	case OSP_DISCONNECT:
// 	//�����Ǹ澯�����������Ϣ
// 	case SVC_AGT_MEMORY_STATUS:          //�ڴ�״̬�ı�
// 	case SVC_AGT_FILESYSTEM_STATUS:      //�ļ�ϵͳ״̬�ı�
// 	//�����ǲ�������Ϣ
// 	case BOARD_MPC_CFG_TEST:
// 	case BOARD_MPC_GET_ALARM:
// 	case BOARD_MPC_MANAGERCMD_TEST:
// 	case MPC_BOARD_GET_MODULE_REQ:      //��ȡ�����ģ����Ϣ
// 		CBBoardAgent::InstanceEntry(pcMsg);
// 		if ( pcMsg->event == BOARD_AGENT_POWERON )
// 		{
// 			if ( !g_cBrdAgentApp.ReadMAPConfig() )
// 			{
// 				ImtLog("Read map config failed!\n");
// 			}
// 		}
// 		break;
	case BOARD_AGENT_POWERON:
		if ( !g_cBrdAgentApp.ReadMAPConfig() )
		{
			ImtLog("Read map config failed!\n");
		}
		break;

	case MPC_BOARD_GET_CONFIG_ACK:       //ȡ������ϢӦ����Ϣ
		ProcBoardGetConfigAck( pcMsg );
		break;
		
	case MPC_BOARD_GET_CONFIG_NACK:      //ȡ������Ϣ��Ӧ��
		ProcBoardGetConfigNAck( pcMsg );
		break;

	case MPC_BOARD_LED_STATUS_REQ:
		ProcLedStatusReq( pcMsg );
		break;

	case BOARD_LED_STATUS:
		ProcBoardLedStatus( pcMsg );
		break;

	case MCU_BRD_FAN_STATUS:
		ProcImtFanStatus( pcMsg );
		break;

	case MCU_BOARD_CONFIGMODIFY_NOTIF:
		ProcBoardConfigModify( pcMsg );
		break;

	default:
		CBBoardAgent::InstanceEntry(pcMsg);
		break;
	}
	
	return;
}

//ģ�ⵥ��BSP�ϵĺ��� ������
//#ifdef WIN32
//void BrdQueryPosition(TBrdPos* ptPosition)
//{
//	return ;
//}
//#else
//#ifndef IMT
//void BrdQueryPosition(TBrdPos* ptPosition)
//{
//	return ;
//}
//#endif
//#endif

/*====================================================================
    ������      ��ProcBoardGetConfigNAck
    ����        ������ȡ������Ϣ��Ӧ����Ϣ����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����CMessage * const pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/08/21    1.0         jianghy       ����
====================================================================*/
void CBoardAgent::ProcBoardGetConfigNAck( CMessage* const pcMsg )
{

	switch( CurState() ) 
	{
	case STATE_INIT:
    case STATE_NORMAL:

		break;
		
	default:
		OspPrintf(TRUE, FALSE, "[Imt] Wrong state %u when in (ProcBoardGetConfigNAck)!srcnode.%d\n", 
			CurState(),pcMsg!=NULL?pcMsg->srcnode:0 );
		break;
	}
	return;
}

/*====================================================================
    ������      ��ProcBoardGetConfigAck
    ����        ������ȡ������ϢӦ����Ϣ����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����CMessage * const pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/08/21    1.0         jianghy       ����
    04/11/11    3.5         �� ��         �½ӿڵ��޸�
====================================================================*/
void CBoardAgent::ProcBoardGetConfigAck( CMessage* const pcMsg )
{
    switch(CurState()) 
    {
    case STATE_INIT:
        MsgGetConfAck(pcMsg);
    	break;

    default:
        OspPrintf(TRUE, FALSE, "[Imt] Wrong state: %d when in (ProcBoardGetConfigAck)\n");
        break;
    }
    
    return;
}

/*=============================================================================
  �� �� ���� MsgGetConfAck
  ��    �ܣ� ����ȡ������ϢAck��Ϣ
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� CMessage* const pcMsg
  �� �� ֵ�� void 
=============================================================================*/
void CBoardAgent::MsgGetConfAck(CMessage* const pcMsg)
{
    u8	byPeriCount;
	u8	byPeriType;
	u16	wLoop, wIndex;
	TEqpMixerEntry*		ptMixerCfg;		//MIXER��������Ϣ
	TEqpTVWallEntry*	ptTVWallCfg;	//TVWALL��������Ϣ
	TEqpBasEntry*		ptBasCfg;		//BAS��������Ϣ
	TEqpVMPEntry*		ptVMPCfg;		//VMP��������Ϣ
	TEqpPrsEntry*		ptPrsCfg;		//Prs��������Ϣ
    TEqpMPWEntry*		ptMpwCfg;
    
    switch( CurState() ) 
	{
	case STATE_INIT:
		
		byPeriCount = pcMsg->content[0]; // �������
		wIndex = 1;
		for( wLoop = 0; wLoop < byPeriCount; wLoop++ )
		{			
			byPeriType = pcMsg->content[wIndex++];  // ��������
			switch( byPeriType )                    // ���øõ�����Ӧ��������Ϣ
			{
			case EQP_TYPE_TVWALL:
				ptTVWallCfg = (TEqpTVWallEntry*)(pcMsg->content+wIndex);
				wIndex += sizeof(TEqpTVWallEntry);
				g_cBrdAgentApp.SetTVWallConfig(ptTVWallCfg);
				break;
				
			case EQP_TYPE_MIXER:
				ptMixerCfg = (TEqpMixerEntry*)(pcMsg->content+wIndex);
				wIndex += sizeof(TEqpMixerEntry);
				g_cBrdAgentApp.SetMixerConfig(ptMixerCfg);
				break;
				
			case EQP_TYPE_BAS:
				ptBasCfg = (TEqpBasEntry*)(pcMsg->content+wIndex);
				wIndex += sizeof(TEqpBasEntry);
				g_cBrdAgentApp.SetBasConfig(ptBasCfg);
				break;
				
			case EQP_TYPE_VMP:
				ptVMPCfg = (TEqpVMPEntry*)(pcMsg->content+wIndex);
				wIndex += sizeof(TEqpVMPEntry);
				g_cBrdAgentApp.SetVMPConfig(ptVMPCfg);
				break;
                
			case EQP_TYPE_VMPTW:
                ptMpwCfg = (TEqpMPWEntry*)(pcMsg->content+wIndex);
                wIndex += sizeof(TEqpMPWEntry);
                g_cBrdAgentApp.SetMpwConfig(ptMpwCfg);
                break;

			case EQP_TYPE_PRS:
				ptPrsCfg = (TEqpPrsEntry*)(pcMsg->content+wIndex);
                ptPrsCfg->SetMcuIp(g_cBrdAgentApp.GetMpcIpA()); // Prs û��ȡ��McuIp
				wIndex += sizeof(TEqpPrsEntry);
				g_cBrdAgentApp.SetPrsConfig(ptPrsCfg);
				break;
			default:
				break;
			}
		}
		
//#ifndef WIN32
#ifdef IMT
		BrdLedStatusSet( LED_SYS_LINK, BRD_LED_ON );
#endif
//#endif
		
		if( pcMsg->content[ wIndex + 6 ] == 0 ) // �Ƿ�ʹ�ÿ��Ź�
		{
//#ifndef WIN32
			SysRebootDisable( );
//#endif
		}
		else
		{
//#ifndef WIN32
			SysRebootEnable( );
//#endif
		}

		OspSemGive( g_semImt );

		KillTimer( BRDAGENT_GET_CONFIG_TIMER );
		NEXTSTATE( STATE_NORMAL );
        break;
		
	default:
		OspPrintf(TRUE, FALSE, "BoardAgent: Wrong message %u(%s) received in current state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
    return;
}

/*====================================================================
    ������      ��ProcLedStatusReq
    ����        ��MPC���͸������ȡ���������״̬��Ϣ����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����CMessage * const pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/08/21    1.0         jianghy       ����

    04/11/11    3.5         �� ��         �½ӿڵ��޸�
====================================================================*/
void CBoardAgent::ProcLedStatusReq( CMessage* const pcMsg )
{
//#ifndef WIN32
#ifdef IMT
	u8 abyBuf[32];
	u8  byCount;
    TBrdLedState tBrdLedState;

	switch( CurState() ) 
	{
	case STATE_INIT:
	case STATE_NORMAL:
        BrdQueryLedState( &tBrdLedState );
        memcpy(&m_tLedState, &tBrdLedState.nlunion.tBrdIMTLedState, sizeof(TBrdIMTLedStateDesc));
		
		TBrdPos tBrdPos = g_cBrdAgentApp.GetBrdPosition();
		memcpy(abyBuf, &tBrdPos, sizeof(tBrdPos) );
		byCount = sizeof(m_tLedState);
		memcpy(abyBuf+sizeof(tBrdPos), &byCount, sizeof(byCount) ); 
		memcpy(abyBuf+sizeof(tBrdPos)+sizeof(byCount), &m_tLedState, sizeof(m_tLedState));
		
		PostMsgToManager( BOARD_MPC_LED_STATUS_ACK, abyBuf, 
			sizeof(tBrdPos)+sizeof(u8)+sizeof(m_tLedState) );
		break;
		
	default:
		OspPrintf(TRUE, FALSE, "BoardAgent: Wrong message %u(%s) received in current state %u!\n", 
			pcMsg->event, ::OspEventDesc( pcMsg->event ), CurState() );
		break;
	}
#endif
//#endif
	return;
}

/*====================================================================
    ������      ��ProcImtFanStatus
    ����        ��MAP����״̬�ı���Ϣ����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����CMessage * const pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/08/19    1.0         jianghy       ����
====================================================================*/
void CBoardAgent::ProcImtFanStatus( CMessage* const pcMsg )
{
	TBoardAlarm tAlarmData;
	u8 byAlarmObj[2];
	TBoardAlarmMsgInfo tAlarmMsg;
	u8 abyAlarmBuf[ sizeof(TBoardAlarmMsgInfo) + 10 ];

	memset( byAlarmObj, 0, sizeof(byAlarmObj) );
	memset( &tAlarmMsg, 0, sizeof(tAlarmMsg) );
	memset( abyAlarmBuf, 0, sizeof(abyAlarmBuf) );
	byAlarmObj[0] = pcMsg->content[0];
	byAlarmObj[1] = 0;

	TBrdPos tBrdPos = g_cBrdAgentApp.GetBrdPosition();
	if( FindAlarm( ALARM_MCU_BOARD_FAN_STOP, ALARMOBJ_MCU_BRD_FAN, byAlarmObj, &tAlarmData ) )
	{
		if(  pcMsg->content[1] == 0 )      //normal
		{
			if( DeleteAlarm( tAlarmData.dwBoardAlarmSerialNo ) == FALSE )
			{
				ImtLog( "[ProcImtFanStatus]DeleteAlarm( AlarmCode = %lu, Object = %d, %d ) failed!\n", 
				tAlarmData.dwBoardAlarmCode, tAlarmData.byBoardAlarmObject[0], 
				tAlarmData.byBoardAlarmObject[1] );
			}
			else
			{
				tAlarmMsg.wEventId = pcMsg->event;
				tAlarmMsg.abyAlarmContent[0] = tBrdPos.byBrdLayer;
				tAlarmMsg.abyAlarmContent[1] = tBrdPos.byBrdSlot;
				tAlarmMsg.abyAlarmContent[2] = tBrdPos.byBrdID;
				tAlarmMsg.abyAlarmContent[3] =  pcMsg->content[0];
				tAlarmMsg.abyAlarmContent[4] =  pcMsg->content[1];
			
				memcpy(abyAlarmBuf, &tBrdPos,sizeof(tBrdPos) );
				
				memcpy(abyAlarmBuf+sizeof(tBrdPos)+sizeof(u16),
					&tAlarmMsg, sizeof(tAlarmMsg) );

				*(u16*)( abyAlarmBuf + sizeof(tBrdPos) ) = htons(1); 

				PostMsgToManager( BOARD_MPC_ALARM_NOTIFY, abyAlarmBuf, 
					sizeof(tBrdPos)+sizeof(u16)+sizeof(tAlarmMsg) );
			}
		}
	}
	else        //no such alarm
	{
		if( pcMsg->content[1] == 1 )     //abnormal
		{
			if( AddAlarm( ALARM_MCU_BOARD_FAN_STOP, ALARMOBJ_MCU_BRD_FAN, byAlarmObj, &tAlarmData ) == FALSE )
			{
				ImtLog("[ProcImtFanStatus] AddAlarm  ALARM_BOARD_MEMORYERROR failed!\n");
			}
			else
			{
				tAlarmMsg.wEventId = pcMsg->event;
				tAlarmMsg.abyAlarmContent[0] = tBrdPos.byBrdLayer;
				tAlarmMsg.abyAlarmContent[1] = tBrdPos.byBrdSlot;
				tAlarmMsg.abyAlarmContent[2] = tBrdPos.byBrdID;
				tAlarmMsg.abyAlarmContent[3] =  pcMsg->content[0];
				tAlarmMsg.abyAlarmContent[4] =  pcMsg->content[1];

				SetAlarmMsgInfo( tAlarmData.dwBoardAlarmSerialNo, &tAlarmMsg );
			
				memcpy(abyAlarmBuf, &tBrdPos,sizeof(tBrdPos) );
				
				memcpy(abyAlarmBuf+sizeof(tBrdPos)+sizeof(u16),
					&tAlarmMsg, sizeof(tAlarmMsg) );

				*(u16*)( abyAlarmBuf + sizeof(tBrdPos) ) = htons(1); 

				PostMsgToManager( BOARD_MPC_ALARM_NOTIFY, abyAlarmBuf, 
					sizeof(tBrdPos)+sizeof(u16)+sizeof(tAlarmMsg) );
			}
		}
	}
	return;
}

/*====================================================================
    ������      ��ProcBoardLedStatus
    ����        ������״̬�ı���Ϣ����
    �㷨ʵ��    ��
    ����ȫ�ֱ�����
    �������˵����CMessage * const pcMsg, �������Ϣ
    ����ֵ˵��  ��
----------------------------------------------------------------------
    �޸ļ�¼    ��
    ��  ��      �汾        �޸���        �޸�����
    03/08/19    1.0         jianghy       ����
====================================================================*/
void CBoardAgent::ProcBoardLedStatus( CMessage* const pcMsg )
{
//#ifndef WIN32
#ifdef IMT
	u8 abyBuf[32];
	u8  byCount;

	TBrdLedState tLedState;
	memcpy( &tLedState, pcMsg->content, sizeof(TBrdLedState) );

	if( memcmp(&m_tLedState, &tLedState.nlunion.tBrdIMTLedState, sizeof(TBrdIMTLedStateDesc)) == 0 )
		return; 

	TBrdPos tBrdPos = g_cBrdAgentApp.GetBrdPosition();
	memcpy(abyBuf, &tBrdPos, sizeof(TBrdPos) );
	byCount = sizeof(TBrdIMTLedStateDesc);
	memcpy(abyBuf+sizeof(TBrdPos), &byCount, sizeof(u8) ); 
	memcpy(abyBuf+sizeof(TBrdPos)+sizeof(u8), &tLedState.nlunion.tBrdIMTLedState, sizeof(TBrdIMTLedStateDesc));
					
	PostMsgToManager( BOARD_MPC_LED_STATUS_NOTIFY, abyBuf, 
		sizeof(TBrdPos)+sizeof(u8)+sizeof(TBrdIMTLedStateDesc) );
#endif
//#endif
}

/*=============================================================================
�� �� ���� ProcBoardConfigModify
��    �ܣ� 
�㷨ʵ�֣� 
ȫ�ֱ����� 
��    ���� CMessage* const pcMsg
�� �� ֵ�� void 
----------------------------------------------------------------------
�޸ļ�¼    ��
��  ��		�汾		�޸���		�߶���    �޸�����
2007/9/19   4.0			�ܹ��                  ����
=============================================================================*/
void CBoardAgent::ProcBoardConfigModify( CMessage* const pcMsg )
{
	if ( NULL == pcMsg )
	{
		OspPrintf(TRUE, FALSE, "[ProcBoardConfigModify] Receive null msg!\n");
		return;
	}

	CServMsg cServMsg( pcMsg->content, pcMsg->length );
	u8 byLayer = 0;
	memcpy( &byLayer, cServMsg.GetMsgBody(), sizeof(u8) );
	u8 bySlot = 0;
	memcpy( &bySlot, cServMsg.GetMsgBody()+sizeof(u8), sizeof(u8) );

	if ( byLayer != g_cBrdAgentApp.GetBrdPosition().byBrdLayer
		|| bySlot != g_cBrdAgentApp.GetBrdPosition().byBrdSlot )
	{
		ImtLog( "[ProcBoardConfigModify] Receive msg is not local board msg! Layer = %d, Slot = %d\n", byLayer, bySlot );
		return;
	}

	u8	byPeriCount;
	u8	byPeriType;
	u16	wLoop, wIndex;
	TEqpMixerEntry*		ptMixerCfg;		//MIXER��������Ϣ
	TEqpTVWallEntry*	ptTVWallCfg;	//TVWALL��������Ϣ
	TEqpBasEntry*		ptBasCfg;		//BAS��������Ϣ
	TEqpPrsEntry*		ptPrsCfg;		//Prs��������Ϣ
	
	BOOL32 bIsCfgMixer = FALSE;
	BOOL32 bIsCfgTW = FALSE;
	BOOL32 bIsCfgBas = FALSE;
	BOOL32 bIsCfgPrs = FALSE;

	u8 *pMsg = cServMsg.GetMsgBody()+sizeof(u8)+sizeof(u8);

	byPeriCount = pMsg[0]; // �����������
	wIndex = 1;

	for( wLoop = 0; wLoop < byPeriCount; wLoop++ )
	{			
		byPeriType = pMsg[wIndex++]; // ������������
		
		switch( byPeriType ) // ���øõ�����Ӧ��������Ϣ
		{
		case EQP_TYPE_TVWALL:
			bIsCfgTW = TRUE;
			if ( !g_cBrdAgentApp.IsRunTVWall() )
			{
				ptTVWallCfg = (TEqpTVWallEntry*)(pMsg+wIndex);
				wIndex += sizeof(TEqpTVWallEntry);
				g_cBrdAgentApp.SetTVWallConfig(ptTVWallCfg);
				TEqpCfg tTVWallCfg;
				g_cBrdAgentApp.GetTVWallCfg(tTVWallCfg);
				if( !twInit( &tTVWallCfg ) )
				{
					ImtLog( " [ProcBoardConfigModify]Start the tvwall failed!\n");
				}	
				else
				{
					ImtLog( " [ProcBoardConfigModify]Start the tvwall OK!\n");
				}
			}
			else
			{
				ImtLog( "The board have started the tvwall module, need reboot!\n" );
				return;
			}
			break;
			
		case EQP_TYPE_MIXER:
			bIsCfgMixer = TRUE;
			if ( !g_cBrdAgentApp.IsRunMixer() )
			{
				ptMixerCfg = (TEqpMixerEntry*)(pMsg+wIndex);
				wIndex += sizeof(TEqpMixerEntry);
				g_cBrdAgentApp.SetMixerConfig(ptMixerCfg);
				TAudioMixerCfg tMixerCfg;
				g_cBrdAgentApp.GetMixerCfg(tMixerCfg);
				if( !mixInit( &tMixerCfg ) )
				{
					ImtLog( " [ProcBoardConfigModify]Start the mixer failed!\n");
				}	
				else
				{
					ImtLog( " [ProcBoardConfigModify]Start the mixer OK!\n");
				}
			}
			else
			{
				ImtLog( "The board have started the mixer module, need reboot!\n" );
				return;
			}
			break;
			
		case EQP_TYPE_BAS:
			bIsCfgBas = TRUE;
			if ( !g_cBrdAgentApp.IsRunBas() )
			{
				ptBasCfg = (TEqpBasEntry*)(pMsg+wIndex);
				wIndex += sizeof(TEqpBasEntry);
				g_cBrdAgentApp.SetBasConfig(ptBasCfg);
				TEqpCfg tBasCfg;
				g_cBrdAgentApp.GetBasCfg( tBasCfg );
				if( !basInit( &tBasCfg ) )
				{
					ImtLog( " [ProcBoardConfigModify]Start the bas failed!\n");
				}	
				else
				{
					ImtLog( " [ProcBoardConfigModify]Start the bas OK!\n");
				}
			}
			else
			{
				ImtLog( "The board have started the bas module, need reboot!\n" );
				return;
			}
			break;
			
		case EQP_TYPE_PRS:
			bIsCfgPrs = TRUE;
			if ( !g_cBrdAgentApp.IsRunPrs() )
			{
				ptPrsCfg = (TEqpPrsEntry*)(pMsg+wIndex);
				ptPrsCfg->SetMcuIp(g_cBrdAgentApp.GetMpcIpA()); // Prs û��ȡ��McuIp
				wIndex += sizeof(TEqpPrsEntry);
				TPrsCfg tPrsCfg;
				g_cBrdAgentApp.GetPrsCfg(tPrsCfg);
				if( !prsinit( &tPrsCfg ) )
				{
					ImtLog( " [ProcBoardConfigModify]Start the prs failed!\n");
				}	
				else
				{
					ImtLog( " [ProcBoardConfigModify]Start the prs OK!\n");
				}
			}
			else
			{
				ImtLog( "The Board have started the Prs module, need reboot!\n" );
				return;
			}
			break;
		default:
			break;
		}
	}

	if ( (!bIsCfgMixer && g_cBrdAgentApp.IsRunMixer()) 
		|| (!bIsCfgBas && g_cBrdAgentApp.IsRunBas())
		|| (!bIsCfgPrs && g_cBrdAgentApp.IsRunPrs())
		|| (!bIsCfgTW && g_cBrdAgentApp.IsRunTVWall()) )
	{
		ImtLog("[ProcBoardConfigModify] New cfg delete the eqp, need reboot!\n");
		return;
	}
	
	// �Ժ��ǵ���IP        
    ImtLog( "Cfg brd ip = 0x%x\n", ntohl(*(u32*)&pMsg[wIndex+2]) );

	if ( g_cBrdAgentApp.GetBrdIpAddr() != ntohl(  *(u32*)&pMsg[wIndex+2] ) )
	{
		ImtLog( "The board ip changed, need recfg!\n" );
		return;
	}
			
	if( pMsg[ wIndex + 6 ] == 0 )// �Ժ����Ƿ�ʹ�ÿ��Ź�
	{
//#ifndef WIN32
		SysRebootDisable( );
//#endif
	}
	else
	{
//#ifndef WIN32
		SysRebootEnable( );
//#endif
	}
		
	return;
}
/*lint -save -esym(530,argptr)*/
void ImtLog(char * fmt, ...)
{
	if ( g_cBrdAgentApp.m_bPrintBrdLog )
	{
		OspPrintf(TRUE, FALSE, "[Imt]");
	}
    va_list argptr;
	va_start(argptr, fmt);
	g_cBrdAgentApp.brdlog(fmt, argptr);
	va_end(argptr);
    return;
}
/*lint -restore*/

/*lint -save -e714*/
/*lint -save -e765*/
API u32 BrdGetDstMcuNode( void )
{
    return g_cBrdAgentApp.m_dwDstMcuNodeA;
}

API u32 BrdGetDstMcuNodeB(void)
{
    return g_cBrdAgentApp.m_dwDstMcuNodeB;
}

API u8 BrdGetTvwallModel(void)
{
    return g_cBrdAgentApp.m_byTvWallModel;
}

// API void noupdatesoft( void )
// {
// 	g_cBrdAgentApp.m_bUpdateSoftBoard = FALSE;
// }
// 
// API void updatesoft( void )
// {
// 	g_cBrdAgentApp.m_bUpdateSoftBoard = TRUE;
// }

API void pimtmsg(void)
{
	g_cBrdAgentApp.m_bPrintBrdLog = 1;
    return;
}

API void npimtmsg(void)
{
	g_cBrdAgentApp.m_bPrintBrdLog = 0;
	return;
}
/*lint -restore*/