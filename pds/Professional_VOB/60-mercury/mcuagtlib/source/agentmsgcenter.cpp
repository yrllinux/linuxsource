/*****************************************************************************
   模块名      : Mcu Agent
   文件名      : AgetnMsgCenter.cpp
   相关文件    : AgeentMsgCenter.h
   文件实现功能: 消息的中转
   作者        : liuhuiyun
   版本        : V4.0  Copyright( C) 2006-2008 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人          修改内容
   2005/08/17  1.0         liuhuiyun       创建
   2006/04/30  4.0		   liuhuiyun       优化
   2006/11/02  4.0         张宝卿          数据读取算法优化
******************************************************************************/

// Last value assigned to variable not used)
/*lint -save -e438*/
/*lint -save -e529*/		// Symbol not subsequently referenced
/*lint -save -e550*/		// Symbol not accessed)
/*lint -save -e578*/		// Declaration of symbol hides symbol 

#include <string.h>
#include <stdlib.h>
//#include <time.h>

#include "osp.h"
#include "agentcommon.h"
#include "agentmsgcenter.h"
#include "configureagent.h"

#ifdef _VXWORKS_
    #include <semLib.h>
    #include <taskLib.h>
    #include <inetlib.h>
    #define IS_DSL8000_BRD_MPC
#endif

CMsgCenterApp		    g_cMsgProcApp;
extern  SEMHANDLE       g_hSync;	            // 全局同步信号量，用于类之间同步操作
BOOL32                  g_bNmsOnline = FALSE;   // 全局变量，用于MPC板NMS灯状态控制 mqs add 20101118
BOOL32                  g_bNmsLedState = FALSE; // 全局变量，用于标记MPC板NMS灯状态 mqs add 20101118

CMsgCenterInst::CMsgCenterInst()
{
}

CMsgCenterInst::~CMsgCenterInst()
{
    Quit();
}

/*=============================================================================
  函 数 名： AgentCallBack
  功    能： Snmp回调函数
  算法实现： 
  全局变量： 
  参    数： u32 dwNodeName
             u8 byRWFlag
             void * pBuf
             u16* pwBufLen
  返 回 值： u16 
  -----------------------------------------------------------------------------
  修改记录:
  日  期      版本        修改人      修改内容
  2006/07/28  4.0         张宝卿      优化整理
=============================================================================*/
u16 AgentCallBack(u32 dwNodeName, u8 byRWFlag, void * pBuf, u16* pwBufLen)
{
    u32    dwNodeValue = GET_NODENAME(dwNodeName);
    BOOL32 bRead = ( READ_FLAG == byRWFlag ) ? TRUE : FALSE;
    u16    wResult = SNMP_SUCCESS;
   
    switch( dwNodeValue )
    {
    //Mcu Sys - system
    case NODE_MCUSYSSTATE:
    case NODE_MCUSYSTIME:
    case NODE_MCUSYSFTPFILENAME:    
    case NODE_MCUSYSCONFIGVERSION:
    case NODE_MCUSYSSOFTWAREVERSION:
    case NODE_MCUSYSFTPUSER:
    case NODE_MCUSYSFTPPASSWORD:
    case NODE_MCUSYSCONFIGERRORSTRING:
    case NODE_MCUSYSLAYER:
    case NODE_MCUSYSSLOT:
    case NODE_MCUSYSMGTSTATE:
    case NODE_MCUSYSMGTUPDATESOFTWARE:
    case NODE_MCUSYSMGTBITERRORTEST:
/*  case NODE_MCUSYSMGTTIMESYNC:*/  // [20120806 liaokang] 网管通过NODE_MCUSYSTIME直接同步所有单板时间
    case NODE_MCUSYSMGTSELFTEST:
    case NODE_MCUSYSOSTYPE:
    case NODE_MCUSYSSUBTYPE:
	case NODE_MCUSYSSELFUPDATESOFTWARE:
	case NODE_MCUCOMPILETIME:
        wResult = ProcCallBackMcuSystem(dwNodeName, bRead, pBuf, pwBufLen);
        break;

    //Mcu Sys - pfm Alarm
    case NODE_MCUPFMALARMSTAMP:
    case NODE_MCUPFMALARMSERIALNO:
    case NODE_MCUPFMALARMALARMCODE:
    case NODE_MCUPFMALARMOBJTYPE:
    case NODE_MCUPFMALARMOBJECT:
    case NODE_MCUPFMALARMTIME:
        wResult = ProcCallBackMcuPfmAlarm(dwNodeName, bRead, pBuf, pwBufLen);
        break;

    //Mcu Sys - notifications
    case NODE_MCUCONFSTATENOTIFY:
    case NODE_MCUCONFIDNOTIFY:
    case NODE_MCUCONFNAMENOTIFY:
    case NODE_MCUCONFSTARTTIMENOTIFY:
    case NODE_MTSTATENOTIFY:
    case NODE_MTALIASNOTIFY:
    case NODE_MTJOINTIMENOTIFY:
        wResult = ProcCallBackMcuNotifications(dwNodeName, bRead, pBuf, pwBufLen);
        break;

    //Mcu Sys - Local info
    case NODE_MCUID:
    case NODE_MCUALIAS:
    case NODE_MCUE164NUMBER:
	case NODE_MCULOCALINFOS:
	//新增Case语句SetLocalInfo须作相应修改[2/21/2013 chendaiwei]
        wResult = ProcCallBackMcuLocal(dwNodeName, bRead, pBuf, pwBufLen);
        break;

    //Mcu Net - Net
	case NODE_MCUNETPORTKIND:
	case NODE_MCUNETTEMPPORTKIND:
	case NODE_MCUNETIPADDR:
	case NODE_MCUNETIPMASK:
	case NODE_MCUNETGATEWAY:		
    case NODE_MCUNETGKIPADDR:
    case NODE_MCUNETMULTICASTIPADDR:
    case NODE_MCUNETMULTICASTPORT:         
    case NODE_MCUNETSTARTRECVPORT: 
    case NODE_MCUNET225245STARTPORT:   
    case NODE_MCUNET225245MAXMTNUM:
    case NODE_MCUNETUSEMPCTRANSDATA:
    case NODE_MCUNETUSEMPCSTACK:
	case NODE_MCUNETMAC:
	case NODE_MCUNETGKUSED:
	//新增Case语句SetNetWorkInfo须作相应修改[2/21/2013 chendaiwei]
        wResult = ProcCallBackMcuNet(dwNodeName, bRead, pBuf, pwBufLen);
        break;

    //Mcu Net - QOS
    case NODE_MCUNETQOSTYPE:
    case NODE_MCUNETQOSIPSERVICETYPE: 
    case NODE_MCUNETQOSAUDIOLEVEL: 
    case NODE_MCUNETQOSVIDEOLEVEL:
    case NODE_MCUNETQOSDATALEVEL: 
    case NODE_MCUNETQOSSIGNALLEVEL:
        wResult = ProcCallBackMcuNetQos(dwNodeName, bRead, pBuf, pwBufLen);
        break;
    
    //Mcu Net - brd snmp table
    case NODE_MCUNETSNMPCFGIPADDR:
    case NODE_MCUNETSNMPCFGREADCOMMUNITY: 
    case NODE_MCUNETSNMPCFGWRITECOMMUNITY:
    case NODE_MCUNETSNMPCFGGETSETPORT:
    case NODE_MCUNETSNMPCFGTRAPPORT:
    case NODE_MCUNETSNMPCFGEXIST:
        wResult = ProcCallBackMcuNetSnmpCfg(dwNodeName, bRead, pBuf, pwBufLen);
        break;

    //Mcu Net - brd cfg table
    case NODE_MCUNETBRDCFGID:
    case NODE_MCUNETBRDCFGLAYER: 
    case NODE_MCUNETBRDCFGSLOT: 
    case NODE_MCUNETBRDCFGTYPE:
    case NODE_MCUNETBRDCFGSTATUS:
    case NODE_MCUNETBRDCFGVERSION:
    case NODE_MCUNETBRDCFGIPADDR:
    case NODE_MCUNETBRDCFGMODULE:
    case NODE_MCUNETBRDCFGPANELLED:
    case NODE_MCUNETBRDCFGOSTYPE:
        wResult = ProcCallBackMcuNetBoardCfg(dwNodeName, bRead, pBuf, pwBufLen);
        break;

    // Mcu Eqp - mixer table
    case NODE_MCUEQPMIXERID: 
    case NODE_MCUEQPMIXERPORT:
    case NODE_MCUEQPMIXERSWITCHBRDID:
    case NODE_MCUEQPMIXERALIAS:
    case NODE_MCUEQPMIXERRUNNINGBRDID:
    case NODE_MCUEQPMIXERIPADDR:
    case NODE_MCUEQPMIXERMIXSTARTPORT:
    case NODE_MCUEQPMIXERMAXMIXGROUPNUM:  
    case NODE_MCUEQPMIXERCONNSTATE:
    case NODE_MCUEQPMIXEREXIST: 
        wResult = ProcCallBackMcuEqpMixer(dwNodeName, bRead, pBuf, pwBufLen);
        break;

    //Mcu Eqp - recorder table
    case NODE_MCUEQPRECORDERID: 
    case NODE_MCUEQPRECORDERSWITCHBRDID:
    case NODE_MCUEQPRECORDERPORT:
    case NODE_MCUEQPRECORDERALIAS:
    case NODE_MCUEQPRECORDERIPADDR:
    case NODE_MCUEQPRECORDERRECVSTARTPORT: 
    case NODE_MCUEQPRECORDERCONNSTATE:
    case NODE_MCUEQPRECORDEREXIST:
        wResult = ProcCallBackMcuEqpRecorder(dwNodeName, bRead, pBuf, pwBufLen);
        break;

    //Mcu Eqp - TVWall table
    case NODE_MCUEQPTVWALLID:
    case NODE_MCUEQPTVWALLALIAS: 
    case NODE_MCUEQPTVWALLRUNNINGBRDID: 
    case NODE_MCUEQPTVWALLIPADDR:  
    case NODE_MCUEQPTVWALLVIDEOSTARTRECVPORT:  
    case NODE_MCUEQPTVWALLCONNSTATE: 
    case NODE_MCUEQPTVWALLEXIST:
        wResult = ProcCallBackMcuEqpTVWall(dwNodeName, bRead, pBuf, pwBufLen);
        break;

    //Mcu Eqp - Bas table
    case NODE_MCUEQPBASID:
    case NODE_MCUEQPBASPORT:
    case NODE_MCUEQPBASSWITCHBRDID:
    case NODE_MCUEQPBASALIAS:
    case NODE_MCUEQPBASRUNNINGBRDID:
    case NODE_MCUEQPBASIPADDR:
    case NODE_MCUEQPBASSTARTRECVPORT :
    case NODE_MCUEQPBASCONNSTATE:
    case NODE_MCUEQPBASEXIST:
        wResult = ProcCallBackMcuEqpBas(dwNodeName, bRead, pBuf, pwBufLen);
        break;

    //Mcu Eqp - Vmp table
    case NODE_MCUEQPVMPID :
    case NODE_MCUEQPVMPPORT :
    case NODE_MCUEQPVMPSWITCHBRDID :
    case NODE_MCUEQPVMPALIAS:
    case NODE_MCUEQPVMPRUNNINGBRDID :
    case NODE_MCUEQPVMPIPADDR :
    case NODE_MCUEQPVMPSTARTRECVPORT : 
    case NODE_MCUEQPVMPENCODERNUM: 
        wResult = ProcCallBackMcuEqpVMP(dwNodeName, bRead, pBuf, pwBufLen);
        break;

    //Mcu Eqp - DCS Stub
    case NODE_MCUEQPDCSIPADDR :  
    case NODE_MCUEQPDCSCONNSTATE:
        wResult = ProcCallBackMcuEqpDcs(dwNodeName, bRead, pBuf, pwBufLen);
       break;

    //Mcu Eqp - PRS Stub
    case NODE_MCUEQPPRSID:   
    case NODE_MCUEQPPRSMCUSTARTPORT: 
    case NODE_MCUEQPPRSSWITCHBRDID:
    case NODE_MCUEQPPRSALIAS:
    case NODE_MCUEQPPRSRUNNINGBRDID:
    case NODE_MCUEQPPRSIPADDR:       
    case NODE_MCUEQPPRSSTARTPORT: 
    case NODE_MCUEQPPRSFIRSTTIMESPAN: 
    case NODE_MCUEQPPRSSECONDTIMESPAN: 
    case NODE_MCUEQPPRSTHIRDTIMESPAN:
    case NODE_MCUEQPPRSREJECTTIMESPAN:
        wResult = ProcCallBackMcuEqpPrs(dwNodeName, bRead, pBuf, pwBufLen);
        break;
      
    //Mcu Eqp - Net Sync Stub
    case NODE_MCUEQPNETSYNCMODE : 
    case NODE_MCUEQPNETSYNCDTSLOT:  
    case NODE_MCUEQPNETSYNCE1INDEX :  
    case NODE_MCUEQPNETSYNCSTATE:
        wResult = ProcCallBackMcuEqpNetSync(dwNodeName, bRead, pBuf, pwBufLen);
       break;

	//Mcu Performance Info
	case NODE_MCUPFMCPURATE:
	case NODE_MCUPFMMEMORY:
	case NODE_MCUPFMMTNUM:
	case NODE_MCUPFMCONFNUM:
	case NODE_MCUPFMEQPUSEDRATE:
	case NODE_MCUPFMAUTHRATE:
	case NODE_MCUPFMMACHTEMPER:
    case NODE_MCUPFMMCSCONNECTIONS:           // 新增 在线MCS的IP信息  [04/24/2012 liaokang] 
        wResult = ProcCallBackMcuPfmInfo(dwNodeName, bRead, pBuf, pwBufLen);
       break;

    default:
        Agtlog(LOG_ERROR, "[AgentCallBack] unexpected node(0x%x) received for: %d !\n", dwNodeValue, bRead);
        wResult = SNMP_GENERAL_ERROR;
        break;
    }
    return wResult;
}

/*=============================================================================
  函 数 名： ProcCallBackMcuSystem
  功    能： 处理MCU系统状态回调
  算法实现： 
  全局变量： 
  参    数： u32 dwNodeName:    构造节点名
             BOOL32  bRead:     读写模式 0-write，1-read
             void * pBuf:       输入输出数据
             u16 *pwBufLen:     数据长度
  返 回 值： u16 成功 SNMP_SUCCESS
  -----------------------------------------------------------------------------
  修改记录:
  日  期      版本        修改人      修改内容
  2006/07/28  4.0         张宝卿       创建
=============================================================================*/
u16 ProcCallBackMcuSystem(u32 dwNodeName, BOOL32 bRead, void *pBuf, u16* pwBufLen)
{
    //u8  byRow       = GET_NODE_ROW(dwNodeName);
    u32 dwNodeValue = GET_NODENAME(dwNodeName);
    u16 wResult = SNMP_SUCCESS;
    BOOL32 bSupportOpr = TRUE;
    
    TMcuSystem tMcuSysInfo;
    g_cCfgParse.GetSystemInfo( &tMcuSysInfo );
    
    switch(dwNodeValue)
    {
    case NODE_MCUSYSSTATE:

        if( bRead )
        {
            *pwBufLen = (u16)sizeof(s32);
            *(s32*)pBuf = (s32)tMcuSysInfo.GetState();
        }
        else
        {
            u32 dwOpt = *(u32*)pBuf;
            tMcuSysInfo.SetState( (u8)dwOpt );
        }
        break;

    case NODE_MCUSYSTIME:

        if( bRead )
        {
			u16 wTimeLen = strlen(tMcuSysInfo.GetTime());
            *pwBufLen = min( *pwBufLen, wTimeLen );
            strncpy( (s8*)pBuf, tMcuSysInfo.GetTime(), *pwBufLen );
        }
        else
        {
            //同步机框时间采用实时处理策略: 理由是通过结构传输会导致系统时间的拖延[12/12/2006-zbq]
            g_cCfgParse.SyncSystemTime( (const s8*)pBuf );
        }
        break;

    case NODE_MCUSYSFTPFILENAME:
        
        bSupportOpr = FALSE;
        break;
        
    case NODE_MCUSYSCONFIGVERSION:
        
        if ( bRead )
        {
			u16 wCfgVerLen = strlen(tMcuSysInfo.GetConfigVer()) ;
            *pwBufLen = min( *pwBufLen, wCfgVerLen );
            strncpy( (s8*)pBuf, tMcuSysInfo.GetConfigVer(), *pwBufLen ); 

			g_bNmsOnline = TRUE;                                 // miaoqingsong  20101118 add 点MPC板NMS灯 
        }
        else
        {
            tMcuSysInfo.SetConfigVer( (s8*)pBuf );
        }
        break;

    case NODE_MCUSYSSOFTWAREVERSION:

        if ( bRead )
        {
			// [miaoqingsong 20111122] 添加网管获取软件版本号接口GetMcuSoftVersion()获取软件版本号做到与MCS界面统一
			u16 wSoftVerLen = strlen(GetMcuSoftVersion());
			*pwBufLen = min( *pwBufLen, wSoftVerLen );
			strncpy( (s8*)pBuf, GetMcuSoftVersion(), *pwBufLen);

//             memcpy((s8*)pBuf, VER_MCU, strlen(VER_MCU));
//             *pwBufLen = strlen(VER_MCU); 
			g_bNmsOnline = TRUE;                                 // miaoqingsong  20101118 add 点MPC板NMS灯
			Agtlog( LOG_INFORM, "[ProcCallBackMcuSystem] NODE_MCUSYSSOFTWAREVERSION <%s>!\n", GetMcuSoftVersion() );  
        }
        else
        {
            bSupportOpr = FALSE;
        }
        break;
        
    case NODE_MCUSYSFTPUSER:

        if( bRead )
        {
            memcpy((s8*)pBuf, "admin", sizeof("admin"));
            *pwBufLen = sizeof("admin");
        }
        else
        {
            bSupportOpr = FALSE;
        }
        break;

    case NODE_MCUSYSFTPPASSWORD:

        if( bRead )
        {
            memcpy((s8*)pBuf, "admin", sizeof("admin"));
            *pwBufLen = sizeof("admin");
        }
        else
        {
            bSupportOpr = FALSE;
        }
        break;
        
    case NODE_MCUSYSCONFIGERRORSTRING:

        if ( bRead )
        {
            g_cAlarmProc.GetErrorString((s8*)pBuf, *pwBufLen);
        }
        else
        {
            bSupportOpr = FALSE;
        }
        break;

    case NODE_MCUSYSLAYER:

        if( bRead )
        {
            *(s32*)pBuf = (s32)g_cCfgParse.GetMcuLayerCount();
            *pwBufLen = sizeof(s32);
        }
        else
        {
            bSupportOpr = FALSE;
        }
        break;

    case NODE_MCUSYSSLOT:

        if( bRead )
        {
            *(s32*)pBuf = (s32)MCU_SLOT_NUM;
            *pwBufLen = sizeof(s32);
        }
        else
        {
            bSupportOpr = FALSE;
        }
        break;

    case NODE_MCUSYSMGTSTATE:

        if( bRead )
        {
            *(s32*)pBuf = tMcuSysInfo.GetState();
            *pwBufLen = sizeof(s32);
        }
        else
        {
			TBrdPosition  tBrdPos;
            tBrdPos.byBrdLayer = *(u8*)pBuf;
            tBrdPos.byBrdSlot  = *((u8*)pBuf+1);
			// [20110316 miaoqingsong modify] 添加网管侧到MCU侧单板类型转换
            tBrdPos.byBrdID    = g_cCfgParse.GetMcuBrdTypeFromSnmp(*((u8*)pBuf+2));
            u8 byOpt           = *((u8*)pBuf+3);
            if( RESET_BRD == byOpt )
            {
                g_cCfgParse.SnmpRebootBoard(tBrdPos);
                Agtlog(LOG_INFORM, "[ProcCallBackMcuSystem] <%d,%d:%s> reboot \n", 
                                    tBrdPos.byBrdLayer, tBrdPos.byBrdSlot, 
                                    g_cCfgParse.GetBrdTypeStr(tBrdPos.byBrdID) );       
            }
        }
        break;

    case NODE_MCUSYSMGTUPDATESOFTWARE:
        
        if ( bRead )
        {
            wResult = McuMgtGetUpdateFiles( pBuf, pwBufLen );
        }
        else
        {
            wResult = McuMgtSetUpdateFiles( pBuf, pwBufLen );
        }
        break;

		//File仅仅为系统升级文件，不包括配置文件
	case NODE_MCUSYSSELFUPDATESOFTWARE:
		{
			if ( bRead )
			{
				wResult = McuSmGetUpdateFiles( pBuf, pwBufLen );
			}
			else
			{
				wResult = McuSmSetUpdateFiles( pBuf, pwBufLen );
			}
		}
        break;

    case NODE_MCUSYSMGTBITERRORTEST:
        
        //本节点暂不区分读写
        {
            TBrdPosition  tBrdPos;
            tBrdPos.byBrdLayer = *(u8*)pBuf;
            tBrdPos.byBrdSlot  = *((u8*)pBuf+1);
            tBrdPos.byBrdID    = *((u8*)pBuf+2);
            u8 byOpt = *((u8*)pBuf+3);
            if( NMS_TEST_START == byOpt)
            {
                g_cCfgParse.BitErrorTest( tBrdPos );
            }
        }
        break;

        // [20120806 liaokang] 网管通过NODE_MCUSYSTIME直接同步所有单板时间
//     case NODE_MCUSYSMGTTIMESYNC:
// 
//         if ( bRead )
//         {
//             bSupportOpr = FALSE;
//         }
//         else
//         {
//             g_cCfgParse.SyncBoardTime((u8*)pBuf);
//         }
//         break;

    case NODE_MCUSYSMGTSELFTEST:
        
        //本节点暂不区分读写
        {
            TBrdPosition  tBrdPos;
            tBrdPos.byBrdLayer = *(u8*)pBuf;
            tBrdPos.byBrdSlot = *((u8*)pBuf+1);
            tBrdPos.byBrdID = *((u8*)pBuf+2);
            g_cCfgParse.BoardSelfTest( tBrdPos );
        }
        break;

    case NODE_MCUSYSOSTYPE:
        
        if ( bRead )
        {
        #ifdef WIN32
            *(s32*)pBuf = SNMP_OS_TYPE_WIN32;
        #elif defined _VXWORKS_
            *(s32*)pBuf = SNMP_OS_TYPE_VXWORKS;
		#elif defined(_8KE_) || defined(_8KH_) || defined(_8KI_)
            *(s32*)pBuf = SNMP_OS_TYPE_LINUX_RH;
        #else   /*_LINUX_*/
            *(s32*)pBuf = SNMP_OS_TYPE_LINUX;
        #endif
            *pwBufLen = sizeof(s32);
			g_bNmsOnline = TRUE;                                       // miaoqingsong  20101118 add 点MPC板NMS灯
        }
        else
        {
            bSupportOpr = FALSE;
        }
        break;

    case NODE_MCUSYSSUBTYPE:

        if ( bRead )
        {
            u8 byBrdId = 0;
            u8 byMcuSubType = 0;
        
        #ifndef WIN32
			#if defined(_8KE_) 
				byBrdId = SNMP_MCUSUBTYPE_8000G;
			#elif defined(_8KH_)
				if(g_cCfgParse.Is800LMcu())
				{
					byBrdId = SNMP_MCUSUBTYPE_800L;
				}
				else if(g_cCfgParse.Is8000HmMcu())
				{
					byBrdId = SNMP_MCUSUBTYPE_8000H_M;
				}
				else
				{
					byBrdId = SNMP_MCUSUBTYPE_8000H;
				}
			#elif defined(_8KI_)
				byBrdId = SNMP_MCUSUBTYPE_8000I;
			#else
				byBrdId = BrdGetBoardID();
			#endif			
        #else
            byBrdId = BRD_TYPE_MPC/*DSL8000_BRD_MPC*/;
        #endif

            byMcuSubType = g_cCfgParse.McuSubTypeAgt2Snmp(byBrdId);

            *(s32*)pBuf = byMcuSubType;
            *pwBufLen = sizeof(s32); 
			g_bNmsOnline = TRUE;                                   // miaoqingsong  20101118 add 点MPC板NMS灯
        }
        else
        {
            bSupportOpr = FALSE;
        }
        break;

	case NODE_MCUCOMPILETIME:
		{
			if ( bRead )
			{
				s8* pCompileTime = g_cCfgParse.GetMcuCompileTime(*pwBufLen);
				memcpy(pBuf, pCompileTime, *pwBufLen);
			}
			else
			{
				bSupportOpr = FALSE;
			}
		}
		break;
    default:
        wResult = SNMP_GENERAL_ERROR;
        Agtlog(LOG_VERBOSE, "[ProcCallBackMcuSystem] unexpected node(0x%x) received !\n", dwNodeValue );
        break;
    }

    // NOTE: 通过 NODE_MCUSYSMGTTIMESYNC 和 NODE_MCUSYSTIME 对 MPC板和其他单板
    //       进行时间设置可以获得网管传入的最新时间。而此处的tMcuSysInfo里所带
    //       的时间是获取该结构时的 MPC 时间，此处不能再设置，否则，同步操作将
    //       拖延系统时间，而且，如果是同步MPC板或是MCU，则会冲掉 
    //       NODE_MCUSYSMGTTIMESYNC 和 NODE_MCUSYSTIME 的处理，设入错误的时间。[11/27/2006-zbq]
    if ( !bRead && NODE_MCUSYSMGTTIMESYNC != dwNodeValue && NODE_MCUSYSTIME != dwNodeValue )
    {
        wResult = g_cCfgParse.SetSystemInfo( &tMcuSysInfo );
    }
    if ( !bSupportOpr )
    {
        Agtlog(LOG_WARN, "[CallBackMcuSys] node(0x%x) not support operate for: %d<1.read-0.write> \n", dwNodeValue, bRead);
    }
    return wResult;
}

/*=============================================================================
  函 数 名： McuMgtSetUpdateFiles
  功    能： 升级某单板或MPC板的某一个或某几个文件
  算法实现： 
  全局变量： 
  参    数： void * pBuf:       输入输出数据
             u16 *pwBufLen:     数据长度
  返 回 值： u16 成功 SNMP_SUCCESS
  -----------------------------------------------------------------------------
  修改记录:
  日  期      版本        修改人      修改内容
  2006/09/20  4.0         张宝卿       创建
=============================================================================*/
u16 McuMgtSetUpdateFiles( void *pBuf, u16 *pwBufLen )
{
    u16 wResult = SNMP_SUCCESS;
    
    u8 byNum = 0;
    u16 wLen = 0;
    TBrdPosition tBrdPos;
    u8 abyMsgBody[256];
    memset( abyMsgBody, 0, sizeof(abyMsgBody) );

    wLen = *pwBufLen;

    //u8 byUpdateType    = *(u8*)pBuf - '0';      // 0-MCU,1-单板(含MPC), 目前该字段恒为1
    tBrdPos.byBrdLayer = *((u8*)pBuf+1) - '0';  // 升级板层号
    tBrdPos.byBrdSlot  = *((u8*)pBuf+2) - '0';  // 升级板槽号
    tBrdPos.byBrdID    = g_cCfgParse.GetMcuBrdTypeFromSnmp(*((u8*)pBuf+3) - '0');  // 升级板类型
    byNum              = *((u8*)pBuf+4);        // 升级文件数
    wLen = wLen - 5;

    Agtlog(LOG_INFORM, "[McuMgtSetUpdateFiles] Brd<%d,%d:%s> update file len: %d.\n",
                        tBrdPos.byBrdLayer, tBrdPos.byBrdSlot, 
                        g_cCfgParse.GetBrdTypeStr(tBrdPos.byBrdID), wLen );    
    
    u8 *pbyMsgBody = abyMsgBody;
    memcpy( pbyMsgBody, &tBrdPos, sizeof(tBrdPos));
    pbyMsgBody += sizeof(tBrdPos);
    memcpy( pbyMsgBody, &byNum, sizeof(u8));
    pbyMsgBody += sizeof(u8);
    memcpy( pbyMsgBody, ((u8*)pBuf+5), wLen);
    
    OspPost( MAKEIID(AID_MCU_AGENT, 1), EV_MSGCENTER_NMS_UPDATEDMPC_CMD,
                         abyMsgBody, wLen + sizeof(tBrdPos)+sizeof(u8));

    return wResult;
}

/*=============================================================================
  函 数 名： McuMgtSetUpdateFiles
  功    能： 升级某单板或MPC板的某一个或某几个文件
  算法实现： 
  全局变量： 
  参    数： void * pBuf:       输入输出数据
             u16 *pwBufLen:     数据长度
  返 回 值： u16 成功 SNMP_SUCCESS
  -----------------------------------------------------------------------------
  修改记录:
  日  期      版本        修改人      修改内容
  2006/09/20  4.0         张宝卿       创建
=============================================================================*/
u16 McuSmSetUpdateFiles( void *pBuf, u16 *pwBufLen )
{
    u16 wResult = SNMP_SUCCESS;

	TMcuUpdateInfo* ptUpdateInfo = g_cCfgParse.GetMcuUpdateInfo();

	Agtlog(LOG_VERBOSE, "McuSmSetUpdateFiles: pwBufLen.%d\n",pwBufLen!=NULL?*pwBufLen:0);

	if (ptUpdateInfo == NULL)
	{
		return SNMP_GENERAL_ERROR;
	}

	if (ptUpdateInfo->GetUpdateState() != SNMP_FILE_UPDATE_INIT)
	{
		return SNMP_GENERAL_ERROR;
	}

	ptUpdateInfo->Clear();

	ptUpdateInfo->SetUpdateState(SNMP_FILE_UPDATE_INPROCESS);

	u8 byUpdateNum = 0;
	u8 byLayerID = 0;
	u8 bySlotID = 0;
	u8 byType = 0;

	u8 *pszBuff = (u8 *)pBuf;

	byUpdateNum = *(u8 *)(pszBuff + 0) ;//上传文件数
	byLayerID = *(u8 *)(pszBuff + 1); //层
	bySlotID = *(u8 *)(pszBuff + 2);//槽
	byType = *(u8 *)(pszBuff + 3);//单板类型

	pszBuff += sizeof(u32);

	//  [1/26/2011 chendaiwei]支持MPC2
	if ((byUpdateNum == 0) || (byType != BRD_TYPE_MPC/*DSL8000_BRD_MPC*/ && byType !=BRD_TYPE_MPC2 ))
	{
		ptUpdateInfo->SetUpdateState(SNMP_FILE_UPDATE_INIT);
		return SNMP_GENERAL_ERROR;
	}

	ptUpdateInfo->SetBrdPosition(byLayerID, bySlotID, byType);

	u32 dwSvrIp = 0; //ftp server Ip
	u16 wSrvPort = 0;//ftp server port
	s8  achVerServUsername[MAXLEN_PWD + 4] = {0};    //版本服务器登陆用户名
	s8  achVerServPassword[MAXLEN_PWD + 4] = {0};     //版本服务器登陆密码
	s8	aszFilePath[KDV_MAX_PATH] = {0};

	u8 byLen = 0;

	TUpdateFileInfo tUpdateFileInfo;

	u8 byIndex = 0;
	for (byIndex = 0; byIndex < byUpdateNum; ++byIndex)
	{
		memset(&tUpdateFileInfo, 0, sizeof(tUpdateFileInfo));

		dwSvrIp = *(u32 *)pszBuff;
		pszBuff += sizeof(u32);

		wSrvPort = *(u16 *)pszBuff;
		pszBuff += sizeof(u16);

		byLen = *(u8 *)pszBuff;
		pszBuff += sizeof(u8);
		memcpy(achVerServPassword, pszBuff, byLen);
		achVerServPassword[byLen] = '\0';
		pszBuff += byLen;

		byLen = *(u8 *)pszBuff;
		pszBuff += sizeof(u8);
		memcpy(achVerServUsername, pszBuff, byLen);
		achVerServUsername[byLen] = '\0';
		pszBuff += byLen;

		byLen = *(u8 *)pszBuff;
		pszBuff += sizeof(u8);
		memcpy(aszFilePath, pszBuff, byLen);
		aszFilePath[byLen] = '\0';
		pszBuff += byLen;

		Agtlog(LOG_VERBOSE, "dwSvrIp: 0x%x, wSrvPort: %d\n", dwSvrIp, wSrvPort);

		Agtlog(LOG_VERBOSE, "achVerServUsername %s, achVerServPassword: %s\n", achVerServUsername, achVerServPassword);

		Agtlog(LOG_VERBOSE, "aszFilePath %s, Len: %d\n", aszFilePath, byLen);

		tUpdateFileInfo.SetServ(dwSvrIp, wSrvPort);
		tUpdateFileInfo.setSvrUser(achVerServUsername, achVerServPassword);
		tUpdateFileInfo.SetSvrFilePath(aszFilePath);
		tUpdateFileInfo.ChgUpdateState(FTPC_TRANSPERCENT);
		ptUpdateInfo->AddUpdateInfo(tUpdateFileInfo);
	}

	Agtlog(LOG_VERBOSE, "ptUpdateInfo->FtpGet:\n");

	if (!ptUpdateInfo->FtpGet())
	{
		Agtlog(LOG_VERBOSE, "ptUpdateInfo->FtpGet: Fail\n");
		ptUpdateInfo->SetUpdateState(SNMP_FILE_UPDATE_FAILED);
		return SNMP_GENERAL_ERROR;
	}

    return wResult;
}

/*=============================================================================
  函 数 名： McuMgtGetUpdateFiles
  功    能： 读取当前所有在线单板 和 MPC板的 所有文件(目前是两个)的升级状态
  算法实现： 
  全局变量： 
  参    数： void * pBuf:       输入输出数据
             u16 *pwBufLen:     数据长度
  返 回 值： u16 成功 SNMP_SUCCESS
  -----------------------------------------------------------------------------
  修改记录:
  日  期      版本        修改人      修改内容
  2006/09/21  4.0         张宝卿       创建
=============================================================================*/
u16 McuMgtGetUpdateFiles( void *pBuf, u16 *pwBufLen )
{
    u16 wResult = SNMP_SUCCESS;
    
    u8  abyBuf[4096];
    memset(abyBuf, 0, sizeof(abyBuf));
    u8 *pbyBuf = &abyBuf[0] ;
    u8  byBrdNum = (u8)g_cCfgParse.GetBoardNum();
    u8  byBrdNumOnlineOSLinux = 0;
    
    //1. 第一个字段空出来，填在线的 linux 单板数(含MPC, 不含远端MPC)
    pbyBuf += 1;
    Agtlog(LOG_INFORM, "[McuMgtGetUpdateFiles] total brd num: %d !\n", byBrdNum);

    //2. 后续各单板按 u8(层)+u8(槽)+u8(类型)+u8(文件数)+u8(文件长度)+s8[](文件名)+u8(升级状态) ... 顺序写入
    s8 achOSFileName[32] = {0};
    s8 achImageFileName[32] = {0};
	s8 achBinFileName[32] = {0};
    memset(achOSFileName, 0, sizeof(achOSFileName));
    memcpy(achOSFileName, "/ramdisk/update.linux", sizeof("/ramdisk/update.linux"));
    
    u8 byImageFileStatus = SNMP_FILE_UPDATE_INIT;
    u8 byOSFileStatus    = SNMP_FILE_UPDATE_INIT;
	u8 byBinFileStatus	 = SNMP_FILE_UPDATE_INIT;
    BOOL32 bOSLinux = FALSE;
#ifdef _LINUX_
    bOSLinux = TRUE;
#endif
    u8 byRow = 1;
    for( ; byRow <= byBrdNum; byRow ++ )
    {
        TEqpBrdCfgEntry tBrdInfo;
        TBrdPosition    tBrdPos;
        memset(achImageFileName, 0, sizeof(achImageFileName));
        
        if ( SUCCESS_AGENT != g_cCfgParse.GetBrdInfoByRow(byRow, &tBrdInfo) )
        {
            Agtlog(LOG_WARN, "[McuMgtGetUpdateFiles] Index.%d GetBrdCfg failed !\n", byRow);
            continue;
        }
        else
        {
            tBrdPos.byBrdLayer = tBrdInfo.GetLayer();
            tBrdPos.byBrdSlot  = tBrdInfo.GetSlot();
            tBrdPos.byBrdID    = tBrdInfo.GetType();            
        }

		//  [1/21/2011 chendaiwei]支持MPC2
        TMPCInfo tMPCInfo;
        g_cCfgParse.GetMPCInfo( &tMPCInfo );
        if ( !( ( g_cBrdManagerApp.IsRegedBoard(tBrdPos) && OS_TYPE_LINUX == tBrdInfo.GetOSType() ) ||
                ( ( BRD_TYPE_MPC/*DSL8000_BRD_MPC*/ == tBrdInfo.GetType() || BRD_TYPE_MPC2 == tBrdInfo.GetType()) && 
                  tMPCInfo.GetLocalLayer() == tBrdInfo.GetLayer() && 
                  tMPCInfo.GetLocalSlot()  == tBrdInfo.GetSlot()  && bOSLinux ) ) )
        {
            Agtlog(LOG_WARN, "[McuMgtGetUpdateFiles] BrdId.%d <%d,%d:%s>, os<%d> reg<%d> is not reged linux brd or local linux mpc!\n", 
                              tBrdInfo.GetBrdId(), tBrdInfo.GetLayer(), tBrdInfo.GetSlot(),
                              g_cCfgParse.GetBrdTypeStr(tBrdInfo.GetType()),
                              tBrdInfo.GetOSType(), g_cBrdManagerApp.IsRegedBoard(tBrdPos) );
            continue;
        }

		BOOL32 bRet = g_cCfgParse.GetImageFileName(tBrdInfo.GetType(), achImageFileName);

		bRet |= g_cCfgParse.GetBinFileName(tBrdInfo.GetType(), achBinFileName);

		if (!bRet)
		{
            Agtlog(LOG_WARN, "[McuMgtGetUpdateFiles] BrdId.%d <%d,%d:%s> get image name failed!\n", 
				tBrdInfo.GetBrdId(), tBrdInfo.GetLayer(), tBrdInfo.GetSlot(),
				g_cCfgParse.GetBrdTypeStr(tBrdInfo.GetType()) );
            continue;
		}

        byImageFileStatus = g_cBrdManagerApp.GetImageFileStatus(tBrdPos) + '0';
        byOSFileStatus    = g_cBrdManagerApp.GetOSFileStatus(tBrdPos) + '0';
		byBinFileStatus	  = g_cBrdManagerApp.GetBinFileStatus(tBrdPos) + '0';
        if ( 255 == byImageFileStatus || 255 == byOSFileStatus || 255 == byBinFileStatus)
        {
            Agtlog(LOG_WARN, "[McuMgtGetUpdateFiles] BrdId.%d <%d,%d:%s> get status <I.%d,O.%d>failed!\n", 
                              tBrdInfo.GetBrdId(), tBrdInfo.GetLayer(), tBrdInfo.GetSlot(),
                              g_cCfgParse.GetBrdTypeStr(tBrdInfo.GetType()),
                              byImageFileStatus - '0', byOSFileStatus - '0' );
            continue;
        }

        // 实际发送的单板数
        byBrdNumOnlineOSLinux ++;

        //[2.1] Layer
        *pbyBuf = tBrdInfo.GetLayer() + '0';
        pbyBuf += 1;

        //[2.2] Slot
        *pbyBuf = tBrdInfo.GetSlot() + '0';
        pbyBuf += 1;

        //[2.3] Type
        *pbyBuf = g_cCfgParse.GetSnmpBrdTypeFromMcu(tBrdInfo.GetType()) + '0';
        pbyBuf += 1;

        //[2.4] 文件数
        *pbyBuf = 3;    // 目前只有三个文件Image, Linux, Bin
        pbyBuf += 1;

        //[2.5] 文件image：长度+文件名+升级状态
        *pbyBuf = strlen(achImageFileName);
        pbyBuf += 1;
        memcpy(pbyBuf, achImageFileName, sizeof(achImageFileName));
        pbyBuf += strlen(achImageFileName);
        *pbyBuf = byImageFileStatus;
        pbyBuf += 1;
        
        //[2.6] 文件os：长度+文件名+升级状态
        *pbyBuf = strlen(achOSFileName);
        pbyBuf += 1;
        memcpy(pbyBuf, achOSFileName, sizeof(achOSFileName));
        pbyBuf += strlen(achOSFileName);
        *pbyBuf = byOSFileStatus;
        pbyBuf += 1;
		
        //[2.6] 文件os：长度+文件名+升级状态
        *pbyBuf = strlen(achBinFileName);
        pbyBuf += 1;
        memcpy(pbyBuf, achBinFileName, sizeof(achBinFileName));
        pbyBuf += strlen(achBinFileName);
        *pbyBuf = byBinFileStatus;
        pbyBuf += 1;
        Agtlog(LOG_INFORM, "[McuMgtGetUpdateFiles] brd<%d,%d:%s> set %s<%d> %s<%d> and %s<%d> to buf !\n",
                            tBrdInfo.GetLayer(), tBrdInfo.GetSlot(), 
                            g_cCfgParse.GetBrdTypeStr(tBrdInfo.GetType()),
                            achImageFileName, byImageFileStatus-'0', 
							achOSFileName, byOSFileStatus-'0',
							achBinFileName, byBinFileStatus-'0'
							);
    }

    //3.所有单板查询完毕，实际发送单板确定
    abyBuf[0] = byBrdNumOnlineOSLinux;
    
    //4.返回给 snmp适配
    *pwBufLen  = strlen((s8*)abyBuf);
    memcpy(pBuf, abyBuf, *pwBufLen);
    Agtlog(LOG_INFORM, "[McuMgtGetUpdateFiles] Buf<len:%d>, Brd Num: %d !\n", 
                                          *pwBufLen, byBrdNumOnlineOSLinux );
    return wResult;
}

/*=============================================================================
  函 数 名： McuSmGetUpdateFiles
  功    能： 读取当前所有在线单板 和 MPC板的 所有文件(目前是两个)的升级状态
  算法实现： 
  全局变量： 
  参    数： void * pBuf:       输入输出数据
             u16 *pwBufLen:     数据长度
  返 回 值： u16 成功 SNMP_SUCCESS
  -----------------------------------------------------------------------------
  修改记录:
  日  期      版本        修改人      修改内容
=============================================================================*/
u16 McuSmGetUpdateFiles( void *pBuf, u16 *pwBufLen )
{
    u16 wResult = SNMP_SUCCESS;

	u8  abyBuf[4096];
    memset(abyBuf, 0, sizeof(abyBuf));
    u8 *pbyBuf = &abyBuf[0] ;
    
	TMcuUpdateInfo tUpdateInfo;
	g_cCfgParse.GetMcuUpdateInfo(&tUpdateInfo);
	
	u8 byFileNum = tUpdateInfo.GetFileNum();
	u16 wBuffSize = 0;
	TUpdateFileInfo *ptUpdateFile = NULL;
	u8 byFileLen = 0;
	u8 byState = 0;

	u8 byUpdate = tUpdateInfo.GetUpdateState();

	Agtlog(LOG_INFORM, "McuSmGetUpdateFiles FileNum:%d totalState:%d\n", byFileNum, byUpdate);

	//u8 file num
	*(u8 *)pbyBuf = byFileNum;
	wBuffSize += sizeof(u8);
	pbyBuf += sizeof(u8);
	for (u8 byIndex = 0; byIndex < byFileNum; ++byIndex)
	{
		ptUpdateFile = tUpdateInfo.GetUpdateFile(byIndex);
		
		if (ptUpdateFile == NULL)
		{
			break;
		}
		
		*(u32 *)pbyBuf = ptUpdateFile->GetSvrIp();
		pbyBuf += sizeof(u32);
		wBuffSize += sizeof(u32);
		
		*(u16 *)pbyBuf = ptUpdateFile->GetSvrPort();
		pbyBuf += sizeof(u16);
		wBuffSize += sizeof(u16);
		
		byFileLen = strlen(ptUpdateFile->GetSvrFilePath());
		*(u8 *)pbyBuf = byFileLen;
		pbyBuf += sizeof(u8);
		wBuffSize += sizeof(u8);
		
		memcpy(pbyBuf, ptUpdateFile->GetSvrFilePath(), byFileLen);
		pbyBuf += byFileLen;
		wBuffSize += byFileLen;
		
		*(u8 *)pbyBuf = (u8)ptUpdateFile->GetTransPercent();
		pbyBuf += sizeof(u8);
		wBuffSize += sizeof(u8);
		
		byState = SNMP_FILE_UPDATE_INPROCESS;
		if ((byUpdate == SNMP_FILE_UPDATE_FAILED) || (byUpdate == SNMP_FILE_UPDATE_INIT))
		{
			byState = SNMP_FILE_UPDATE_FAILED;
		}
		else if (byUpdate == SNMP_FILE_UPDATE_SUCCEED)
		{
			byState = SNMP_FILE_UPDATE_SUCCEED;
		}
		else
		{
		}

		Agtlog(LOG_INFORM, "McuSmGetUpdateFiles FileIndex:%d FilePath:%s State:%d\n", byIndex, ptUpdateFile->GetSvrFilePath(), byState);
		
		*(u8 *)pbyBuf = byState;
		pbyBuf += sizeof(u8);
		wBuffSize += sizeof(u8);
		
		*(u8 *)pbyBuf = tUpdateInfo.GetLayerID();
		pbyBuf += sizeof(u8);
		wBuffSize += sizeof(u8);
		
		*(u8 *)pbyBuf = tUpdateInfo.GetSlotID();
		pbyBuf += sizeof(u8);
		wBuffSize += sizeof(u8);
		
		*(u8 *)pbyBuf = tUpdateInfo.GetType();
		pbyBuf += sizeof(u8);
		wBuffSize += sizeof(u8);
	}

	//4.返回给 snmp适配
    *pwBufLen  = wBuffSize;
    memcpy(pBuf, abyBuf, *pwBufLen);
    Agtlog(LOG_INFORM, "[McuSmGetUpdateFiles] Buf<len:%d>, File Num: %d !\n", 
                                          *pwBufLen, byFileNum );
    return wResult;
}

/*=============================================================================
  函 数 名： ProcCallBackMcuPfmAlarm
  功    能： 处理MCU系统告警回调
  算法实现： 
  全局变量： 
  参    数： u32 dwNodeName:    构造节点名
             BOOL32  bRead:     读写模式 1-read，0-write
             void * pBuf:       输入输出数据
             u16 *pwBufLen:     数据长度
  返 回 值： u16 成功 SNMP_SUCCESS
  -----------------------------------------------------------------------------
  修改记录:
  日  期      版本        修改人      修改内容
  2006/07/28  4.0         张宝卿       创建
=============================================================================*/
u16 ProcCallBackMcuPfmAlarm(u32 dwNodeName, BOOL32 bRead, void *pBuf, u16* pwBufLen)
{
    u8  byRow       = GET_NODE_ROW(dwNodeName);
    u32 dwNodeValue = GET_NODENAME(dwNodeName);

    u16 wResult = SNMP_SUCCESS;
    //BOOL32 bSupportOpr = FALSE;

    TMcupfmAlarmEntry tAlarmEntry;
    if ( NODE_MCUPFMALARMSTAMP != dwNodeValue ) 
    {
        BOOL32 bResult = g_cAlarmProc.GetAlarmTableIndex( byRow-1, tAlarmEntry );
        if ( !bResult )
        {
            Agtlog(LOG_VERBOSE, "[CallBackPfmAlarm] GetAlarmTableIndex.%d failed !\n", byRow );
            return SNMP_GENERAL_ERROR;
        }
    }
    Agtlog(LOG_VERBOSE, "[CallBackPfmAlarm] Node(0x%x) received for: %d!\n", dwNodeValue, bRead );

    switch(dwNodeValue)
    {
    case NODE_MCUPFMALARMSTAMP:
        
        if ( bRead )
        {
            u16 wTmp16Data = 0;
            g_cAlarmProc.GetAlarmStamp(wTmp16Data);
            *pwBufLen = sizeof(s32);
            *(s32*)pBuf = wTmp16Data;
        }
        break;

    case NODE_MCUPFMALARMSERIALNO:
        
        if ( bRead )
        {
            *pwBufLen = sizeof(s32);
            *(s32*)pBuf = tAlarmEntry.m_dwSerialNo;
        }
        break;

    case NODE_MCUPFMALARMALARMCODE:
        
        if ( bRead )
        {
            *pwBufLen = sizeof(s32);
            *(s32*)pBuf = tAlarmEntry.m_dwAlarmCode;
        }
        break;

    case NODE_MCUPFMALARMOBJTYPE:
        
        if ( bRead )
        {
            *pwBufLen = sizeof(s32);
            *(s32*)pBuf = tAlarmEntry.m_byObjType;
        }       
        break;

    case NODE_MCUPFMALARMOBJECT:
        
        if ( bRead )
        {
            *pwBufLen = sizeof(tAlarmEntry.m_achObject);
            memcpy((s8*)pBuf, tAlarmEntry.m_achObject, sizeof(tAlarmEntry.m_achObject));
        }
        break;

    case NODE_MCUPFMALARMTIME:
        
        if ( bRead )
        {
            *pwBufLen = sizeof(tAlarmEntry.m_achTime);
            memcpy((s8*)pBuf, tAlarmEntry.m_achTime, sizeof(tAlarmEntry.m_achTime));
        }
        break;
        
    default:
        wResult = SNMP_GENERAL_ERROR;
        Agtlog(LOG_VERBOSE, "[ProcCallBackMcuPfmAlarm] unexpected node(0x%x) received !\n", dwNodeValue );
        break;
    }
    if ( !bRead )
    {
        wResult = SNMP_GENERAL_ERROR;
        Agtlog(LOG_WARN, "[CallBackPfmAlarm] Node(0x%x) support no write!\n", dwNodeValue );        
    }
    return wResult;
}

/*=============================================================================
  函 数 名： ProcCallBackMcuNotifications
  功    能： 处理MCU系统通知信息回调：目前没有对应处理
  算法实现： 
  全局变量： 
  参    数： u32 dwNodeName:    构造节点名
             BOOL32  bRead:     读写模式 0-write，1-read
             void * pBuf:       输入输出数据
             u16 *pwBufLen:     数据长度
  返 回 值： u16 成功 SNMP_SUCCESS
  -----------------------------------------------------------------------------
  修改记录:
  日  期      版本        修改人      修改内容
  2006/07/28  4.0         张宝卿       创建
=============================================================================*/
u16 ProcCallBackMcuNotifications(u32 dwNodeName, BOOL32 bRead, void *pBuf, u16* pwBufLen)
{
    u32 dwNodeValue = GET_NODENAME(dwNodeName);
    u16 wResult = SNMP_SUCCESS;
    switch(dwNodeValue)
    {
    case NODE_MCUCONFSTATENOTIFY:
    case NODE_MCUCONFIDNOTIFY:
    case NODE_MCUCONFNAMENOTIFY:
    case NODE_MCUCONFSTARTTIMENOTIFY:
    case NODE_MTSTATENOTIFY:
    case NODE_MTALIASNOTIFY:
    case NODE_MTJOINTIMENOTIFY:
        Agtlog(LOG_WARN, "[ProcCallBackMcuNotifications] node(0x%x) didn't support operate!\n", dwNodeValue);
        break;

    default:
        wResult = SNMP_GENERAL_ERROR;
        Agtlog(LOG_VERBOSE, "[ProcCallBackMcuNotifications] unexpected node(0x%x) received ! bRead.%d pBuf.%p *pwBufLen.%d\n", dwNodeValue,bRead,pBuf,pwBufLen!=NULL?(*pwBufLen):0 );
        break;
    }
    return wResult;
}

/*=============================================================================
  函 数 名： ProcCallBackMcuLocal
  功    能： 处理MCU系统本地信息回调
  算法实现： 
  全局变量： 
  参    数： u32 dwNodeName:    构造节点名
             BOOL32  bRead:     读写模式 0-write，1-read
             void * pBuf:       输入输出数据
             u16 *pwBufLen:     数据长度
  返 回 值： u16 成功 SNMP_SUCCESS
  -----------------------------------------------------------------------------
  修改记录:
  日  期      版本        修改人      修改内容
  2006/07/28  4.0         张宝卿       创建
=============================================================================*/
u16 ProcCallBackMcuLocal(u32 dwNodeName, BOOL32 bRead, void *pBuf, u16* pwBufLen)
{
    u8  byRow       = GET_NODE_ROW(dwNodeName);
    u32 dwNodeValue = GET_NODENAME(dwNodeName);

    TLocalInfo tLoaclInfo;
    g_cCfgParse.GetLocalInfo(&tLoaclInfo);
    Agtlog(LOG_VERBOSE, "[CallBackLocal] Node(0x%x) received for: %d!\n", dwNodeValue, bRead );

    u16 wResult = SNMP_SUCCESS;
    
    switch(dwNodeValue) 
    {
    case NODE_MCUID:
        
        if( bRead )
        {
            *(s32*)pBuf = (s32)tLoaclInfo.GetMcuId();
            *pwBufLen = sizeof(s32);
        }
        else
        {
            tLoaclInfo.SetMcuId( *(u8*)pBuf );
        }
        break;
        
    case NODE_MCUALIAS:
        {    
            //[5/16/2013 liaokang] 转码
            s8 achEncodingBuf[MAXLEN_ALIAS+1] = {0};
            u16  wLen = min(sizeof(achEncodingBuf)-1, *pwBufLen);
            if( bRead )
            {
                if( FALSE == TransEncodingForNmsData(tLoaclInfo.GetAlias(), achEncodingBuf, sizeof(achEncodingBuf)) )
                {
                    strncpy( achEncodingBuf, tLoaclInfo.GetAlias(), wLen);
                }
                *pwBufLen = strlen(achEncodingBuf)+1;
                *pwBufLen = min(*pwBufLen, wLen );
                memcpy( (s8*)pBuf, achEncodingBuf, *pwBufLen ); 
                g_bNmsOnline = TRUE;                                          // miaoqingsong 20101118 add 点MPC板NMS灯
            }
            else
            {
                if( FALSE == TransEncodingForNmsData((s8*)pBuf, achEncodingBuf, sizeof(achEncodingBuf), TRUE) )
                {
                    strncpy(achEncodingBuf, (s8*)pBuf, wLen);
                }
                tLoaclInfo.SetAlias( achEncodingBuf ); 
            }
            
        }
        break;
        
    case NODE_MCUE164NUMBER:  
        
        if( bRead )
        {
			u16 wLocalInfolen =  strlen(tLoaclInfo.GetE164Num());
            *pwBufLen = min(*pwBufLen, wLocalInfolen );
            memcpy((s8*)pBuf, tLoaclInfo.GetE164Num(), *pwBufLen);
        }
        else
        {
            tLoaclInfo.SetE164Num( (s8*)pBuf );
        }
        break;

	case NODE_MCULOCALINFOS: //字节1    这个数据包包含几个域
							 //字节2    第1个域的标识符(比如：编码方式标识符)
							 //字节3    该域值的数据长度（最大255）
							 //字节4    具体域值        （域值如果数值需要网络序，如果是字符串则/0不计算入域值的数据长度）
		
        //[5/16/2013 liaokang] 只要收到此节点就认为是支持Utf8的网管，否则仍然认为是GBK的网管（老的网管需要针对性进行转码） 
        SetSnmpIsUtf8(TRUE);

        if( bRead )
		{
			u8 *pTempBuf = (u8*)pBuf;
			u16 wBufLen = 0;

			*pTempBuf = 1; 
			wBufLen ++;
			pTempBuf++;

			*pTempBuf = (u8)emCharset;
			wBufLen ++;
			pTempBuf++;

			*pTempBuf = 1; 
			wBufLen ++;
			pTempBuf++;

#ifdef _UTF8
            //[4/8/2013 liaokang] 修改编码方式为utf8
            *pTempBuf = (u8)emenCoding_Utf8;
#else
			*pTempBuf = (u8)emenCoding_GBK; //老版本报GBK,值为2
#endif
			wBufLen ++;
			pTempBuf++;

            *pwBufLen = min(*pwBufLen, wBufLen );
		}
		else
		{
			//目前信息暂时不支持写
		}
		break;
        
    default:
        wResult = SNMP_GENERAL_ERROR;
        Agtlog(LOG_VERBOSE, "[ProcCallBackMcuLocal] unexpected node(0x%x) received !\n", dwNodeValue );
        break;
    }
    if ( !bRead )
    {
        wResult = g_cCfgParse.SetLocalInfo(&tLoaclInfo,dwNodeValue);
        if(SUCCESS_AGENT != wResult)
        {
            Agtlog(LOG_ERROR, "[CallBackLocal] SetLocalInfo failed!\n" );
            wResult = SNMP_GENERAL_ERROR;
        }
    }
    return wResult;
}

/*=============================================================================
  函 数 名： ProcCallBackMcuNet
  功    能： 处理MCU系统网络配置信息回调
  算法实现： 
  全局变量： 
  参    数： u32 dwNodeName:    构造节点名
             BOOL32  bRead:     读写模式 0-write，1-read
             void * pBuf:       输入输出数据
             u16 *pwBufLen:     数据长度
  返 回 值： u16 成功 SNMP_SUCCESS
  -----------------------------------------------------------------------------
  修改记录:
  日  期      版本        修改人      修改内容
  2006/07/28  4.0         张宝卿       创建
=============================================================================*/
u16 ProcCallBackMcuNet(u32 dwNodeName, BOOL32 bRead, void *pBuf, u16* pwBufLen)
{
    u8  byRow       = GET_NODE_ROW(dwNodeName);
    u32 dwNodeValue = GET_NODENAME(dwNodeName);
    
    u16 wResult = SNMP_SUCCESS;

	TNetWorkInfo  tNetWorkInfo;
	TMPCInfo tMPCInfo;
	g_cCfgParse.GetNetWorkInfo(&tNetWorkInfo);
	g_cCfgParse.GetMPCInfo(&tMPCInfo);

	//u8 byTempPortKind = g_cCfgParse.GetTempPortKind();
	u8 byLocalPortKind = tMPCInfo.GetLocalPortChoice();
	Agtlog(LOG_VERBOSE, "[NetCallBack] Node(0x%x) received for: %d!\n", dwNodeValue, bRead );

    switch(dwNodeValue) 
    {
	case NODE_MCUNETPORTKIND:
		{
			if( bRead )
			{
				*(s32*)pBuf = (s32)tMPCInfo.GetLocalPortChoice();
				*pwBufLen = sizeof(s32);
			}
			else
			{
				tMPCInfo.SetLocalPortChoice((u8)*(u32*)pBuf);
				g_cCfgParse.SetMPCInfo(tMPCInfo);
			}
		}
		break;
	case NODE_MCUNETTEMPPORTKIND:
		{
			if( bRead )
			{
				*(s32*)pBuf = (s32)g_cCfgParse.GetTempPortKind();
				*pwBufLen = sizeof(s32);
			}
			else
			{
				g_cCfgParse.SetTempPortKind((u8)*(u32*)pBuf);
			}
		}
		break;
	case NODE_MCUNETIPADDR:
		{
			if( bRead )
			{
				*(u32*)pBuf = htonl(g_cCfgParse.GetLocalIp());     
				*pwBufLen = sizeof(s32);
			}
			else
			{
				g_cCfgParse.SetLocalIp(*(u32*)pBuf, byLocalPortKind);
				//g_cCfgParse.SetMPCInfo(tMPCInfo);
			}
		}
		break;
	case NODE_MCUNETIPMASK:
		{
			if( bRead )
			{
				*(u32*)pBuf = htonl(g_cCfgParse.GetMpcMaskIp());
				*pwBufLen = sizeof(s32);
			}
			else
			{
				g_cCfgParse.SetMpcMaskIp(*(u32*)pBuf, byLocalPortKind);
				//g_cCfgParse.SetMPCInfo(tMPCInfo);
			}			
		}
		break;
	case NODE_MCUNETGATEWAY:
		{
			if( bRead )
			{
				*(u32*)pBuf = htonl(g_cCfgParse.GetMpcGateway());
				*pwBufLen = sizeof(s32);
			}
			else
			{
				g_cCfgParse.SetMpcGateway(*(u32*)pBuf);
			}			
		}
		break;
    case NODE_MCUNETGKIPADDR:
        
        if( bRead )
        {
#if defined(_8KE_) || defined(_8KH_) || defined(_8KI_)
			
			*(u32*)pBuf = tNetWorkInfo.GetGkIp();
			
#else
			
			*(u32*)pBuf = htonl(tNetWorkInfo.GetGkIp());
			
#endif

            *pwBufLen = sizeof(s32);
        }
        else
        {
#if defined(_8KE_) || defined(_8KH_) || defined(_8KI_)
			
			tNetWorkInfo.SetGKIp(ntohl(*(u32*)pBuf));
			
#else
			
			tNetWorkInfo.SetGKIp(*(u32*)pBuf);
			
#endif
        }
        break;

    case NODE_MCUNETMULTICASTIPADDR:

        if( bRead )
        {
            *(u32*)pBuf = htonl(tNetWorkInfo.GetCastIp());
            *pwBufLen = sizeof(s32);
        }
        else
        {
            tNetWorkInfo.SetCastIp(*(u32*)pBuf);
        }
        break;

    case NODE_MCUNETMULTICASTPORT: 
        
        if( bRead )
        {
            *(s32*)pBuf = (s32)tNetWorkInfo.GetCastPort();
            *pwBufLen = sizeof(s32);
        }
        else
        {
            tNetWorkInfo.SetCastPort((u16)*(u32*)pBuf);
        }
        break;
        
    case NODE_MCUNETSTARTRECVPORT : 
        
        if( bRead )
        {
            *(s32*)pBuf = (s32)tNetWorkInfo.GetRecvStartPort();
            *pwBufLen = sizeof(s32);
        }
        else
        {
            tNetWorkInfo.SetRecvStartPort((u16)*(u32*)pBuf);
        }
        break;

    case NODE_MCUNET225245STARTPORT :   
        
        if( bRead )
        {
            *(s32*)pBuf = (s32)tNetWorkInfo.Get225245StartPort();
            *pwBufLen = sizeof(s32);
        }
        else
        {
            tNetWorkInfo.Set225245StartPort((u16)*(u32*)pBuf);
        }
        break;

    case NODE_MCUNET225245MAXMTNUM:

        if( bRead )
        {
            *(s32*)pBuf = (s32)tNetWorkInfo.Get225245MtNum();
            *pwBufLen = sizeof(s32);
        }
        else
        {
            tNetWorkInfo.Set225245MtNum((u16)*(u32*)pBuf);
        }
        break;

    case NODE_MCUNETUSEMPCTRANSDATA: 
        
        if( bRead )
        {
            *(s32*)pBuf = (s32)tNetWorkInfo.GetMpcTransData();
            *pwBufLen = sizeof(s32);
        }
        else
        {
            tNetWorkInfo.SetMpcTransData((u8)*(u32*)pBuf);
        }
        break;

    case NODE_MCUNETUSEMPCSTACK:    
        
        if( bRead )
        {
            *(s32*)pBuf = (s32)tNetWorkInfo.GetMpcStack();
            *pwBufLen = sizeof(s32);
        }
        else
        {
            tNetWorkInfo.SetMpcStack((u8)*(u32*)pBuf);
        }
        break;

	case NODE_MCUNETMAC:
		if (bRead)
		{
			*pwBufLen = g_cCfgParse.GetMac((s8 *)pBuf);
		}
		else
		{
			Agtlog(LOG_ERROR, "[ProcCallBackMcuNet] Can not write mac !\n");
		}
		break;

	case NODE_MCUNETGKUSED:
		if (bRead)
		{
			*(s32*)pBuf = (s32)tNetWorkInfo.GetIsGKCharge();
			*pwBufLen = sizeof(s32);
		}
		else
		{
			tNetWorkInfo.SetIsGKCharge((u8)*(u32*)pBuf);
		}
        break;

    default:
        wResult = SNMP_GENERAL_ERROR;
        Agtlog(LOG_ERROR, "[ProcCallBackMcuNet] unexpected node(0x%x) received !\n", dwNodeValue );
        break;
    }
	if ( !bRead )
	{
		u16 wRet = g_cCfgParse.SetNetWorkInfo(&tNetWorkInfo,dwNodeValue);
		if(SUCCESS_AGENT != wRet)
		{
			Agtlog(LOG_ERROR, "[ProcCallBackMcuNet] SetNetWorkInfo mpctask failed !\n");
			wResult = SNMP_GENERAL_ERROR;
		}
	}
    return wResult;
}

/*=============================================================================
  函 数 名： ProcCallBackMcuNetQos
  功    能： 处理MCU系统网络QOS信息回调
  算法实现： 
  全局变量： 
  参    数： u32 dwNodeName:    构造节点名
             BOOL32  bRead:     读写模式 0-write，1-read
             void * pBuf:       输入输出数据
             u16 *pwBufLen:     数据长度
  返 回 值： u16 成功 SNMP_SUCCESS
  -----------------------------------------------------------------------------
  修改记录:
  日  期      版本        修改人      修改内容
  2006/07/28  4.0         张宝卿       创建
=============================================================================*/
u16 ProcCallBackMcuNetQos(u32 dwNodeName, BOOL32 bRead, void *pBuf, u16* pwBufLen)
{
    u8  byRow       = GET_NODE_ROW(dwNodeName);
    u32 dwNodeValue = GET_NODENAME(dwNodeName);
    u8  byTmp8Data  = 0;
    BOOL32 bResult = TRUE;
    u16 wResult = SNMP_SUCCESS;
    
    *pwBufLen = sizeof(s32);

	TQosInfo tQosInfo;
	g_cCfgParse.GetQosInfo( &tQosInfo );
	Agtlog(LOG_VERBOSE, "[NetQosCallBack] Node(0x%x) received for: %d!\n", dwNodeValue, bRead );

    switch(dwNodeValue) 
    {
    case NODE_MCUNETQOSTYPE:
        
        if( bRead )
        {
            byTmp8Data = tQosInfo.GetQosType();
            *(s32*)pBuf = (s32)byTmp8Data;
        }
        else
        {
            bResult = tQosInfo.SetQosType(*(u8*)pBuf);
        }
        break;

    case NODE_MCUNETQOSIPSERVICETYPE: 

        if( bRead )
        {
            byTmp8Data = tQosInfo.GetIpServiceType();
            *(s32*)pBuf = (s32)byTmp8Data;
        }
        else
        {
            bResult = tQosInfo.SetIpServiceType(*(u8*)pBuf);
        }
        break;

    case NODE_MCUNETQOSAUDIOLEVEL: 
        
        if( bRead )
        {
            byTmp8Data = tQosInfo.GetAudioLevel();
            *(s32*)pBuf = (s32)byTmp8Data;
        }
        else
        {
            bResult = tQosInfo.SetAudioLevel(*(u8*)pBuf);
        }
        break;

    case NODE_MCUNETQOSVIDEOLEVEL:
        
        if( bRead )
        {
            byTmp8Data = tQosInfo.GetVideoLevel();
            *(s32*)pBuf = (s32)byTmp8Data;
        }
        else
        {
            bResult = tQosInfo.SetVideoLevel(*(u8*)pBuf);
        }
        break;

    case NODE_MCUNETQOSDATALEVEL: 
        
        if( bRead )
        {
            byTmp8Data = tQosInfo.GetDataLevel();
            *(s32*)pBuf = (s32)byTmp8Data;
        }
        else
        {
            bResult = tQosInfo.SetDataLevel(*(u8*)pBuf);
        }
        break;

    case NODE_MCUNETQOSSIGNALLEVEL: 
        
        if( bRead )
        {
            byTmp8Data = tQosInfo.GetSignalLevel();
            *(s32*)pBuf = (s32)byTmp8Data;
        }
        else
        {
            bResult = tQosInfo.SetSignalLevel(*(u8*)pBuf);
        }
        break;
        
    default:
        wResult = SNMP_GENERAL_ERROR;
        Agtlog(LOG_ERROR, "[ProcCallBackMcuNetQos] unexpected node(0x%x) received !\n", dwNodeValue );
        break;
    }
	if ( !bResult )
	{
		wResult = SNMP_GENERAL_ERROR;
	}
	if ( !bRead )
	{
		if ( SUCCESS_AGENT != g_cCfgParse.SetQosInfo( &tQosInfo ) )
        {
            wResult = SNMP_GENERAL_ERROR;
		}
	}
    return wResult;
}

/*=============================================================================
  函 数 名： ProcCallBackMcuNetSnmpCfg
  功    能： 处理MCU系统网络snmp配置信息回调
  算法实现： 
  全局变量： 
  参    数： u32 dwNodeName:    构造节点名
             BOOL32  bRead:     读写模式 0-write，1-read
             void * pBuf:       输入输出数据
             u16 *pwBufLen:     数据长度
  返 回 值： u16 成功 SNMP_SUCCESS
  -----------------------------------------------------------------------------
  修改记录:
  日  期      版本        修改人      修改内容
  2006/07/28  4.0         张宝卿       创建
=============================================================================*/
u16 ProcCallBackMcuNetSnmpCfg(u32 dwNodeName, BOOL32 bRead, void *pBuf, u16* pwBufLen)
{
    u8  byRow       = GET_NODE_ROW(dwNodeName);
    u32 dwNodeValue = GET_NODENAME(dwNodeName);

    u16 wTmp16Data  = 0;
    //u32 dwTmp32Data = 0;
    s8  achTmpStr[32];
    memset( achTmpStr, '\0', sizeof(achTmpStr));
    u16 wLen = (*pwBufLen > sizeof(achTmpStr)) ? sizeof(achTmpStr) : *pwBufLen;

	u16 wResult = SNMP_SUCCESS;

	TTrapInfo tTrapInfo;
	wResult = g_cCfgParse.GetTrapInfo(byRow, &tTrapInfo);
	Agtlog(LOG_VERBOSE, "[SnmpCallBack] Node(0x%x) received for: %d!\n", dwNodeValue, bRead );
    
    switch(dwNodeValue) 
    {    
    case NODE_MCUNETSNMPCFGIPADDR:
		        
		if( bRead )
		{
			*(u32*)pBuf = tTrapInfo.GetTrapIp();
			*pwBufLen = sizeof(u32);
		}
		else
		{
			tTrapInfo.SetTrapIp( *(u32*)pBuf );
		}
        break;

    case NODE_MCUNETSNMPCFGREADCOMMUNITY: 

		if( bRead )
		{
			u16 wTrapLen = strlen(tTrapInfo.GetReadCom());
			wLen = min( sizeof(achTmpStr), wTrapLen );
			strncpy( (s8*)pBuf, tTrapInfo.GetReadCom(), wLen);
			*pwBufLen = wLen;
		 }
		 else
		 {
			 tTrapInfo.SetReadCom( (s8*)pBuf );
		 }
         break;

    case NODE_MCUNETSNMPCFGWRITECOMMUNITY:

		if( bRead )
		{
			u16 wTrapLen = strlen(tTrapInfo.GetWriteCom());
			wLen = min( *pwBufLen, wTrapLen );
			strncpy( (s8*)pBuf, tTrapInfo.GetWriteCom(), wLen);
			*pwBufLen = wLen;
		}
		else
		{
			tTrapInfo.SetWriteCom( (s8*)pBuf );
		}
        break;

    case NODE_MCUNETSNMPCFGGETSETPORT:
        
		if( bRead )
		{
			*(s32*)pBuf = (s32)tTrapInfo.GetGSPort();
			*pwBufLen = sizeof(s32);
		}
		else
		{
			tTrapInfo.SetGSPort( *(u16*)pBuf );
		}
        break;

    case NODE_MCUNETSNMPCFGTRAPPORT:

		if( bRead )
		{
			*(s32*)pBuf = (s32)tTrapInfo.GetTrapPort();
			*pwBufLen = sizeof(s32);
		}
		else
		{            
			/*tTrapInfo.SetGSPort( *(u16*)pBuf );*/     // [2012/05/04 liaokang] 
            tTrapInfo.SetTrapPort( *(u16*)pBuf );
		}
        break;

    case NODE_MCUNETSNMPCFGEXIST:

        Agtlog(LOG_WARN, "[SnmpCallBack] Node(0x%x) didn't support operate!\n", dwNodeValue);
        break;

    default:
        wResult = SNMP_GENERAL_ERROR;
        Agtlog(LOG_ERROR, "[ProcCallBackMcuNetSnmpCfg] unexpected node(0x%x) received !\n", dwNodeValue );
        break;
    }
	if ( !bRead )
	{
		wResult = g_cCfgParse.SetTrapInfo( byRow, &tTrapInfo );
	}
    return wResult;
}

/*=============================================================================
  函 数 名： ProcCallBackMcuNetBoardCfg
  功    能： 处理MCU系统网络单板配置信息回调
  算法实现： 
  全局变量： 
  参    数： u32 dwNodeName:    构造节点名
             BOOL32  bRead:     读写模式 0-write，1-read
             void * pBuf:       输入输出数据
             u16 *pwBufLen:     数据长度
  返 回 值： u16 成功 SNMP_SUCCESS
  -----------------------------------------------------------------------------
  修改记录:
  日  期      版本        修改人      修改内容
  2006/07/28  4.0         张宝卿       创建
=============================================================================*/
u16 ProcCallBackMcuNetBoardCfg(u32 dwNodeName, BOOL32 bRead, void *pBuf, u16* pwBufLen)
{
    u8  byRow       = GET_NODE_ROW(dwNodeName);
    u32 dwNodeValue = GET_NODENAME(dwNodeName);

    u16 wResult = SNMP_SUCCESS;
    BOOL32 bSupportOpr = TRUE;
    TEqpBrdCfgEntry tBrdInfo;
    memset( &tBrdInfo, 0, sizeof(tBrdInfo) );
    u16 wRet = g_cCfgParse.GetBrdInfoByRow( byRow, &tBrdInfo, TRUE );
    if ( SUCCESS_AGENT != wRet ) 
    {
		Agtlog(LOG_WARN, "[CallBackBrdCfg] Node(0x%x) Row(%d) Error\n", dwNodeValue, byRow );
        return SNMP_GENERAL_ERROR;
    }
    
    Agtlog(LOG_VERBOSE, "[CallBackBrdCfg] Node(0x%x) received for: %d!\n", dwNodeValue, bRead );
    
    switch(dwNodeValue) 
    {
    case NODE_MCUNETBRDCFGID:

        if( bRead )
        {
            *(s32*)pBuf = (s32)tBrdInfo.GetBrdId();
            *pwBufLen = sizeof(s32);
		
			Agtlog(LOG_VERBOSE, "[CallBackBrdCfg] NODE_MCUNETBRDCFGID : %d!\n", tBrdInfo.GetBrdId());
        }
        else
        {
            bSupportOpr = FALSE;
        }
        break;

    case NODE_MCUNETBRDCFGLAYER: 

        if( bRead )
        {
            *(s32*)pBuf = (s32)tBrdInfo.GetLayer();
            *pwBufLen = sizeof(s32);
			
			Agtlog(LOG_VERBOSE, "[CallBackBrdCfg] NODE_MCUNETBRDCFGLAYER : %d!\n", tBrdInfo.GetLayer());
        }
        else
        {
            tBrdInfo.SetLayer( *(u8*)pBuf );
        }
        break;

    case NODE_MCUNETBRDCFGSLOT: 
        
        if( bRead )
        {
            *(s32*)pBuf = (s32)tBrdInfo.GetSlot();
            *pwBufLen = sizeof(s32);
			
			Agtlog(LOG_VERBOSE, "[CallBackBrdCfg] NODE_MCUNETBRDCFGSLOT : %d!\n", tBrdInfo.GetSlot());
        }
        else
        {
            tBrdInfo.SetSlot(  *(u8*)pBuf );
        }
        break;

    case NODE_MCUNETBRDCFGTYPE:
        
        if( bRead )
        {
            *(s32*)pBuf = (s32)(g_cCfgParse.GetSnmpBrdTypeFromMcu(tBrdInfo.GetType()));
            *pwBufLen = sizeof(s32);
			
			Agtlog(LOG_VERBOSE, "[CallBackBrdCfg] NODE_MCUNETBRDCFGTYPE : %d!\n", *(s32*)pBuf);
        }
        else
        {
			// [20110316 miaoqingsong modify] 添加网管侧到MCU侧单板类型转换
			u8 byBrdType = g_cCfgParse.GetMcuBrdTypeFromSnmp(*(u8*)pBuf);
            tBrdInfo.SetType(byBrdType);
        }
        break;

    case NODE_MCUNETBRDCFGSTATUS:
        
        if( bRead )
        {
			// [20110316 miaoqingsong commentary] 获取单板状态：1 - 正常；2 - 断链
            *(s32*)pBuf = (s32)(g_cCfgParse.BrdStatusAgt2NMS(tBrdInfo.GetState()));
            *pwBufLen = sizeof(s32);
			
			Agtlog(LOG_VERBOSE, "[CallBackBrdCfg] NODE_MCUNETBRDCFGSTATUS : %d!\n", *(s32*)pBuf);
        }
        else
        {
            bSupportOpr = FALSE;
        }
        break;

    case NODE_MCUNETBRDCFGVERSION:
        
        if( bRead )
        {
            u16 wLen = MAX_SOFT_VER_LEN > *pwBufLen ? *pwBufLen : MAX_SOFT_VER_LEN;
            strncpy( (s8*)pBuf, tBrdInfo.GetVersion(), wLen );
            *pwBufLen = wLen;
        }
        else
        {
            tBrdInfo.SetVersion( (s8*)pBuf );
        }
        break;

    case NODE_MCUNETBRDCFGIPADDR:
        
        if( bRead )
        {
            *(u32*)pBuf = htonl(tBrdInfo.GetBrdIp());
            *pwBufLen = sizeof(u32);
        }
        else
        {
            tBrdInfo.SetBrdIp( ntohl(*(u32*)pBuf) );
        }
        break;

    case NODE_MCUNETBRDCFGMODULE:

        bSupportOpr = FALSE;
        break;

    case NODE_MCUNETBRDCFGPANELLED:

        if( bRead )
        {
            // liaokang [2012/04/05] 修改 用于状态获取（LED，网口等）
/*			u16 wBrdInfoLen = strlen(tBrdInfo.GetPanelLed());
            u16 wLen = min( wBrdInfoLen, *pwBufLen );
            *pwBufLen = (u16)wLen;
            strncpy( (s8*)pBuf, tBrdInfo.GetPanelLed(), wLen );
            
			// [20110813 miaoqingosng add] 机架图灯状态打印添加：1 亮 2 灭 3 快闪 4 慢闪 5 每隔2秒钟闪
			Agtlog( LOG_VERBOSE, "[CallBackBrdCfg] NODE_MCUNETBRDCFGPANELLED <%s>!\n", tBrdInfo.GetPanelLed() );*/
            
            u16 wCopyLen = 0;
            s8 achLedStatus[MAX_BOARD_LED_NUM + 1]={0};
            s8 achEthPortStatus[MAX_BOARD_ETHPORT_NUM + 1]={0};

            u16 wBrdLedLen = (u16)g_cCfgParse.PanelLedStatusAgt2NMS( tBrdInfo, achLedStatus);
            Agtlog( LOG_VERBOSE, "[CallBackBrdCfg] NODE_MCUNETBRDCFGPANELLED  BrdType: %d! Recombined Led: <%s>! Led Num %d!\n",
                tBrdInfo.GetType(), achLedStatus, wBrdLedLen );

            u16 wBrdEthPortLen = (u16)g_cCfgParse.EthPortStatusAgt2NMS( tBrdInfo, achEthPortStatus );
            Agtlog( LOG_VERBOSE, "[CallBackBrdCfg] NODE_MCUNETBRDCFGPANELLED  BrdType: %d! Recombined EthPort: <%s>! EthPortBufSize %d!\n",
                tBrdInfo.GetType(), achEthPortStatus, wBrdEthPortLen );
            
            wCopyLen = min( wBrdLedLen, *pwBufLen );
            if ( wCopyLen )  // 存在Led状态（非0）
            {
                strncpy( (s8*)pBuf, achLedStatus, wCopyLen );
            }
            else
            {
                switch ( tBrdInfo.GetType() )
                {
                case BRD_TYPE_IS22:
                    wBrdLedLen = 32; // IS2.2 32个led
                    wCopyLen = wBrdLedLen;
                    memset( (s8*)pBuf, 2, wCopyLen );   // IS2.2灯状态 灭
                    break;
                default:
                    break; 
                } 
            }

            if ( wBrdEthPortLen )      // 存在网口状态（非0）
            {
                wCopyLen = min( wBrdEthPortLen, (*pwBufLen) - wCopyLen );
                if ( wCopyLen )
                {
                    strncpy( (s8*)pBuf + wBrdLedLen, achEthPortStatus, wCopyLen );
                    wCopyLen = wCopyLen + wBrdLedLen;
                }
            }
            else
            {
                switch ( tBrdInfo.GetType() )
                {
                case BRD_TYPE_IS22:
                    {
                        // 分隔符'~'(s8) + '~'(s8) + 状态类型(u8) +前网口数目(u8) + 前网口状态(3个) + 后网口数目(u8) + 后网口状态(8个)
                        s8 achEthPort[16]; 
                        memset( achEthPort, 3, sizeof(achEthPort) ); // IS2.2网口状态 未知
                        achEthPort[0] = '~';        // 分隔符
                        achEthPort[1] = '~';        // 分隔符
                        achEthPort[2] = 1;          // 状态类型
                        achEthPort[3] = 3;          // 前网口数目
                        achEthPort[7] = 8;          // 后网口数目
                        wBrdEthPortLen = sizeof( achEthPort );
                        wCopyLen = min( wBrdEthPortLen, (*pwBufLen) - wCopyLen );
                        if ( wCopyLen )
                        {
                            // 分隔符
                            strncpy( (s8*)pBuf + wBrdLedLen, achEthPort, wCopyLen );
                            wCopyLen = wCopyLen + wBrdLedLen;  
                        }
                    }
                    break;
                default:
                    break; 
                } 
            }

            *pwBufLen = (u16)wCopyLen; 
            Agtlog( LOG_VERBOSE, "[CallBackBrdCfg] NODE_MCUNETBRDCFGPANELLED BrdType: %d! Buf:%s! BufSize: %d!\n",
                tBrdInfo.GetType(), pBuf, *pwBufLen );
        }   
        else
        {
            bSupportOpr = FALSE;
        }
        break;
        
    case NODE_MCUNETBRDCFGOSTYPE:

        if ( bRead )
        {
            *(s32*)pBuf = (s32)tBrdInfo.GetOSType();
            *pwBufLen = sizeof(s32);
			Agtlog(LOG_VERBOSE, "[CallBackBrdCfg] NODE_MCUNETBRDCFGOSTYPE : %d!\n", *(s32*)pBuf);
        }
        else
        {
            bSupportOpr = FALSE;
        }
        break;

    default:
        wResult = SNMP_GENERAL_ERROR;
        Agtlog(LOG_ERROR, "[ProcCallBackMcuNetBoardCfg] unexpected node(0x%x) received !\n", dwNodeValue );
        break;
    }
    if ( !bRead )
    {
        wResult = g_cCfgParse.SetBrdInfoByRow( byRow, &tBrdInfo );
    }
    if ( !bSupportOpr )
    {
        Agtlog( LOG_VERBOSE, "[CallBackBrdCfg] node(0x%x) not support operate for: %d<1.read-0.write> \n", dwNodeValue, bRead );
    }
    return wResult;
}

/*=============================================================================
  函 数 名： ProcCallBackMcuEqpMixer
  功    能： 处理MCU外设mixer信息回调
  算法实现： 
  全局变量： 
  参    数： u32 dwNodeName:    构造节点名
             BOOL32  bRead:     读写模式 0-write，1-read
             void * pBuf:       输入输出数据
             u16 *pwBufLen:     数据长度
  返 回 值： u16 成功 SNMP_SUCCESS
  -----------------------------------------------------------------------------
  修改记录:
  日  期      版本        修改人      修改内容
  2006/07/28  4.0         张宝卿       创建
=============================================================================*/
u16 ProcCallBackMcuEqpMixer(u32 dwNodeName, BOOL32 bRead, void *pBuf, u16* pwBufLen)
{
    u8  byRow       = GET_NODE_ROW(dwNodeName);
    u32 dwNodeValue = GET_NODENAME(dwNodeName);

    u8   byTmp8Data = 0;
    u16  wTmp16Data = 0;
    u32  dwTmp32Data = 0;

    TEqpMixerInfo tMixerInfo;
    u16 wRet = g_cCfgParse.GetEqpMixerCfgByRow( byRow, &tMixerInfo );
    if ( SUCCESS_AGENT != wRet )
    {
        Agtlog(LOG_ERROR, "[ProcCallBackMcuEqpMixer] get mixer info failed, ret:%d !\n", wRet);
        return SNMP_GENERAL_ERROR;
    }
    Agtlog(LOG_VERBOSE, "[CallBackMixer]Node<0x%x> received for: %d!\n", dwNodeValue, bRead );

    u16 wResult = SNMP_SUCCESS;
    BOOL32 bSupportOpr = TRUE;

    switch(dwNodeValue) 
    {
    case NODE_MCUEQPMIXERID: 

        if( bRead )
        {
           byTmp8Data = tMixerInfo.GetEqpId();
           *(s32*)pBuf = (s32)byTmp8Data; 
           *pwBufLen = sizeof(byTmp8Data);
        }
        else
        {
            bSupportOpr = FALSE;
        }        
        break;

    case NODE_MCUEQPMIXERPORT:

        if( bRead )
        {
            wTmp16Data = tMixerInfo.GetMcuRecvPort();
            *(s32*)pBuf = (s32)wTmp16Data;
            *pwBufLen = sizeof(s32);
        }
        else
        {
            tMixerInfo.SetMcuRecvPort( *(u16*)pBuf );
        }
        break;

    case NODE_MCUEQPMIXERSWITCHBRDID:

        if( bRead )
        {
            byTmp8Data = tMixerInfo.GetSwitchBrdId();
            *(s32*)pBuf = (s32)byTmp8Data;
            *pwBufLen = sizeof(s32);
        }
        else
        {
            tMixerInfo.SetSwitchBrdId( *(u8*)pBuf );
        }
        break;

    case NODE_MCUEQPMIXERALIAS:
        {            
            //[5/16/2013 liaokang] 转码
            s8 achEncodingBuf[MAXLEN_EQP_ALIAS+1] = {0};
            u16  wLen = min(sizeof(achEncodingBuf)-1, *pwBufLen);
            if( bRead )
            {                
                if( FALSE == TransEncodingForNmsData(tMixerInfo.GetAlias(), achEncodingBuf, sizeof(achEncodingBuf)) )
                {
                    strncpy( achEncodingBuf, tMixerInfo.GetAlias(), wLen);
                }
                *pwBufLen = strlen(achEncodingBuf)+1;
                *pwBufLen = min(*pwBufLen, wLen);
                strncpy( (s8*)pBuf, achEncodingBuf, *pwBufLen);
            }     
            else
            {
                if( FALSE == TransEncodingForNmsData((s8*)pBuf, achEncodingBuf, wLen, TRUE) )
                {
                    strncpy(achEncodingBuf, (s8*)pBuf, wLen);
                }
                tMixerInfo.SetAlias(achEncodingBuf);
            }
        }
        break;

    case NODE_MCUEQPMIXERRUNNINGBRDID:

        if( bRead )
        {
            byTmp8Data = tMixerInfo.GetRunBrdId();
            *(s32*)pBuf = (s32)byTmp8Data;
            *pwBufLen = sizeof(s32);
        }
        else
        {
            tMixerInfo.SetRunBrdId( *(u8*)pBuf );
        }
        break;

    case NODE_MCUEQPMIXERIPADDR:

        if( bRead )
        {
            dwTmp32Data = htonl(tMixerInfo.GetIpAddr());
            *(u32*)pBuf = dwTmp32Data;
            *pwBufLen = sizeof(dwTmp32Data);
        }
        else
        {
            bSupportOpr = FALSE;
        } 
        break;

    case NODE_MCUEQPMIXERMIXSTARTPORT:

        if( bRead )
        {
            wTmp16Data = tMixerInfo.GetEqpRecvPort();
            *(s32*)pBuf = (s32)wTmp16Data;
            *pwBufLen = sizeof(s32);
        }
        else
        {
            tMixerInfo.SetEqpRecvPort( *(u16*)pBuf );
        }
        break;

    case NODE_MCUEQPMIXERMAXMIXGROUPNUM:  

        if( bRead )
        {
             byTmp8Data = tMixerInfo.GetMaxChnInGrp();
             *(s32*)pBuf = (s32)byTmp8Data;
            *pwBufLen = sizeof(s32);
        }
        else
        {
            tMixerInfo.SetMaxChnInGrp( *(u8*)pBuf );
        }
        break;

    case NODE_MCUEQPMIXERCONNSTATE:

        bSupportOpr = FALSE;
        break;

    case NODE_MCUEQPMIXEREXIST:
        
        bSupportOpr = FALSE;
        break;

    default:
        wResult = SNMP_GENERAL_ERROR;
        Agtlog(LOG_ERROR, "[ProcCallBackMcuEqpMixer] unexpected node(0x%x) received !\n", dwNodeValue );
        break;
    }
    
    if ( !bRead )
    {
        u16 wResult = g_cCfgParse.SetEqpMixerCfgByRow(byRow, tMixerInfo);       
    }
    if ( !bSupportOpr )
    {
        Agtlog(LOG_WARN, "[ProcCallBackMcuEqpMixer] node(0x%x) not support operate for: %d<1.read-0.write> \n", dwNodeValue, bRead);
    }
    return wResult;
}

/*=============================================================================
  函 数 名： ProcCallBackMcuEqpRecorder
  功    能： 处理MCU外设recorder信息回调
  算法实现： 
  全局变量： 
  参    数： u32 dwNodeName:    构造节点名
             BOOL32  bRead:     读写模式 0-write，1-read
             void * pBuf:       输入输出数据
             u16 *pwBufLen:     数据长度
  返 回 值： u16 成功 SNMP_SUCCESS
  -----------------------------------------------------------------------------
  修改记录:
  日  期      版本        修改人      修改内容
  2006/07/28  4.0         张宝卿       创建
=============================================================================*/
u16 ProcCallBackMcuEqpRecorder(u32 dwNodeName, BOOL32 bRead, void *pBuf, u16* pwBufLen)
{
    u8  byRow       = GET_NODE_ROW(dwNodeName);
    u32 dwNodeValue = GET_NODENAME(dwNodeName);
    
    u8   byTmp8Data = 0;
    u16  wTmp16Data = 0;
    u32  dwTmp32Data = 0;
    
    TEqpRecInfo tRecInfo;
    u16 wRet = g_cCfgParse.GetEqpRecCfgByRow( byRow, &tRecInfo );
    if ( SUCCESS_AGENT != wRet )
    {
        Agtlog(LOG_ERROR, "[ProcCallBackMcuEqpRecorder] get rec info failed, ret:%d !\n", wRet);
        return SNMP_GENERAL_ERROR;
    }
    Agtlog(LOG_VERBOSE, "[CallBackRec] Node<0x%x> received for: %d!\n", dwNodeValue, bRead );

    u16 wResult = SNMP_SUCCESS;
    BOOL32 bSupportOpr = TRUE;
    
    switch(dwNodeValue) 
    {
    case NODE_MCUEQPRECORDERID: 

        if( bRead )
        {
            byTmp8Data = tRecInfo.GetEqpId();
            *(s32*)pBuf = (s32)byTmp8Data;
            *pwBufLen = sizeof(s32);
        }
        else
        {
            bSupportOpr = FALSE;
        } 
        break;

    case NODE_MCUEQPRECORDERSWITCHBRDID:
        
        if( bRead )
        {
            byTmp8Data = tRecInfo.GetSwitchBrdId();
            *(s32*)pBuf = (s32)byTmp8Data;
            *pwBufLen = sizeof(s32);
        }
        else
        {
            tRecInfo.SetSwitchBrdId( *(u8*)pBuf );
        }
        break;

    case NODE_MCUEQPRECORDERPORT:
        
		if( bRead )
		{
			wTmp16Data = tRecInfo.GetMcuRecvPort();
			*(s32*)pBuf = (s32)wTmp16Data;
			*pwBufLen = sizeof(s32);
		}
		else
		{
			tRecInfo.SetEqpRecvPort( *(u16*)pBuf );
		}
        break;

    case NODE_MCUEQPRECORDERALIAS:
        {            
            //[5/16/2013 liaokang] 转码
            s8 achEncodingBuf[MAXLEN_EQP_ALIAS+1] = {0};
            u16  wLen = min(sizeof(achEncodingBuf)-1, *pwBufLen);
            if( bRead )
            {                
                if( FALSE == TransEncodingForNmsData(tRecInfo.GetAlias(), achEncodingBuf, sizeof(achEncodingBuf)) )
                {
                    strncpy(achEncodingBuf, tRecInfo.GetAlias(), wLen);
                }
                *pwBufLen = strlen(achEncodingBuf)+1;
                *pwBufLen = min(*pwBufLen, wLen);
                strncpy( (s8*)pBuf, achEncodingBuf, *pwBufLen);
            }     
            else
            {
                if( FALSE == TransEncodingForNmsData((s8*)pBuf, achEncodingBuf, wLen, TRUE) )
                {
                    strncpy(achEncodingBuf, (s8*)pBuf, wLen);
                }
                tRecInfo.SetAlias( achEncodingBuf );
            }
        }
        break;

    case NODE_MCUEQPRECORDERIPADDR:

		if( bRead )
		{
		   dwTmp32Data = htonl(tRecInfo.GetIpAddr());
		   *(u32*)pBuf = dwTmp32Data;
		   *pwBufLen = sizeof(dwTmp32Data);
		}
		else
		{
			tRecInfo.SetIpAddr( *(u32*)pBuf );
		}
        break;

    case NODE_MCUEQPRECORDERRECVSTARTPORT: 

		if( bRead )
		{
		   wTmp16Data = tRecInfo.GetEqpRecvPort();
		   *(s32*)pBuf = (s32)wTmp16Data;
		   *pwBufLen = sizeof(s32);
		}
		else
		{
			tRecInfo.SetEqpRecvPort( *(u16*)pBuf );
		}
        break;

    case NODE_MCUEQPRECORDERCONNSTATE:
    case NODE_MCUEQPRECORDEREXIST:
        
        bSupportOpr = FALSE;
        break;

    default:
        wResult = SNMP_GENERAL_ERROR;
        Agtlog(LOG_ERROR, "[ProcCallBackMcuEqpRecorder] unexpected node(0x%x) received !\n", dwNodeValue );
        break;
    }
    if ( !bRead )
    {
        u16 wResult = g_cCfgParse.SetEqpRecCfgByRow( byRow, &tRecInfo);
        if( SUCCESS_AGENT != wResult )
        {
            wResult = SNMP_GENERAL_ERROR;
        }        
    }
    if ( !bSupportOpr )
    {
        Agtlog(LOG_WARN, "[ProcCallBackMcuEqpRecorder] node(0x%x) not support operate for: %d<1.read-0.write> \n", dwNodeValue, bRead);
    }
    return wResult;
}

/*=============================================================================
  函 数 名： ProcCallBackMcuEqpTVWall
  功    能： 处理MCU外设tvwall信息回调
  算法实现： 
  全局变量： 
  参    数： u32 dwNodeName:    构造节点名
             BOOL32  bRead:     读写模式 0-write，1-read
             void * pBuf:       输入输出数据
             u16 *pwBufLen:     数据长度
  返 回 值： u16 成功 SNMP_SUCCESS
  -----------------------------------------------------------------------------
  修改记录:
  日  期      版本        修改人      修改内容
  2006/07/28  4.0         张宝卿       创建
=============================================================================*/
u16 ProcCallBackMcuEqpTVWall(u32 dwNodeName, BOOL32 bRead, void *pBuf, u16* pwBufLen)
{
    u8  byRow       = GET_NODE_ROW(dwNodeName);
    u32 dwNodeValue = GET_NODENAME(dwNodeName);
    
    u8   byTmp8Data = 0;
    u16  wTmp16Data = 0;
    u32  dwTmp32Data = 0;

    TEqpTVWallInfo tTVWallInfo;
    u16 wRet = g_cCfgParse.GetEqpTVWallCfgByRow( byRow, &tTVWallInfo );
    if ( SUCCESS_AGENT != wRet )
    {
        Agtlog(LOG_ERROR, "[ProcCallBackMcuEqpTVWall] get rec info failed, ret:%d !\n", wRet);
        return SNMP_GENERAL_ERROR;
    }
    Agtlog(LOG_VERBOSE, "[CallBackTVWall] Node<0x%x> received for: %d!\n", dwNodeValue, bRead );

    u16 wResult = SNMP_SUCCESS;
    BOOL32 bSupportOpr = TRUE;
    
    switch(dwNodeValue) 
    {
    case NODE_MCUEQPTVWALLID:

        if( bRead )
        {
            byTmp8Data = tTVWallInfo.GetEqpId();
            *(s32*)pBuf = (s32)byTmp8Data;
            *pwBufLen = sizeof(s32);
        }
        else
        {
            bSupportOpr = FALSE;
        } 
        break;

    case NODE_MCUEQPTVWALLALIAS: 
        {            
            //[5/16/2013 liaokang] 转码
            s8 achEncodingBuf[MAXLEN_EQP_ALIAS+1] = {0};
            u16  wLen = min(sizeof(achEncodingBuf)-1, *pwBufLen);
            if( bRead )
            {                
                if( FALSE == TransEncodingForNmsData(tTVWallInfo.GetAlias(), achEncodingBuf, sizeof(achEncodingBuf)) )
                {
                    strncpy( achEncodingBuf, tTVWallInfo.GetAlias(), wLen);
                }
                *pwBufLen = strlen(achEncodingBuf)+1;
                *pwBufLen = min(*pwBufLen, wLen);
                strncpy( (s8*)pBuf, achEncodingBuf, *pwBufLen);
            }     
            else
            {
                if( FALSE == TransEncodingForNmsData((s8*)pBuf, achEncodingBuf, wLen, TRUE) )
                {
                    strncpy(achEncodingBuf, (s8*)pBuf, wLen);
                }
                tTVWallInfo.SetAlias(achEncodingBuf);
            }
        }
        break;

    case NODE_MCUEQPTVWALLRUNNINGBRDID: 
        
        if( bRead )
        {
            byTmp8Data = tTVWallInfo.GetRunBrdId();
            *(s32*)pBuf = (s32)byTmp8Data;
            *pwBufLen = sizeof(byTmp8Data);
        }
        else
        {
            tTVWallInfo.SetRunBrdId( *(u8*)pBuf );
        }
        break;

    case NODE_MCUEQPTVWALLIPADDR:  

        if( bRead )
        {
            dwTmp32Data = htonl(tTVWallInfo.GetIpAddr());
            *(u32*)pBuf = dwTmp32Data;
            *pwBufLen = sizeof(dwTmp32Data);
        }
        else
        {
            bSupportOpr = FALSE;
        }
        break;

    case NODE_MCUEQPTVWALLVIDEOSTARTRECVPORT:  

        if( bRead )
        {
            wTmp16Data = tTVWallInfo.GetEqpRecvPort();
            *(s32*)pBuf = (s32)wTmp16Data;
            *pwBufLen = sizeof(s32);
        }
        else
        {
            tTVWallInfo.SetEqpRecvPort( *(u16*)pBuf );
        }
        break;

    case NODE_MCUEQPTVWALLCONNSTATE:        
    case NODE_MCUEQPTVWALLEXIST:
        
        bSupportOpr = FALSE;
        break;

    default:
        wResult = SNMP_GENERAL_ERROR;
        Agtlog(LOG_ERROR, "[ProcCallBackMcuEqpTVWall] unexpected node(0x%x) received !\n", dwNodeValue );
        break;
    }

    if ( !bRead )
    {
        u16 wResult = g_cCfgParse.SetEqpTVWallCfgByRow( byRow, &tTVWallInfo );
        if( SUCCESS_AGENT != wResult )
        {
            wResult = SNMP_GENERAL_ERROR;
        }        
    }
    if ( !bSupportOpr )
    {
        Agtlog(LOG_WARN, "[ProcCallBackMcuEqpTVWall] node(0x%x) not support operate for: %d<1.read-0.write> \n", dwNodeValue, bRead);
    }
    return wResult;
}

/*=============================================================================
  函 数 名： ProcCallBackMcuEqpBas
  功    能： 处理MCU外设bas信息回调
  算法实现： 
  全局变量： 
  参    数： u32 dwNodeName:    构造节点名
             BOOL32  bRead:     读写模式 0-write，1-read
             void * pBuf:       输入输出数据
             u16 *pwBufLen:     数据长度
  返 回 值： u16 成功 SNMP_SUCCESS
  -----------------------------------------------------------------------------
  修改记录:
  日  期      版本        修改人      修改内容
  2006/07/28  4.0         张宝卿       创建
=============================================================================*/
u16 ProcCallBackMcuEqpBas(u32 dwNodeName, BOOL32 bRead, void *pBuf, u16* pwBufLen)
{
    u8  byRow       = GET_NODE_ROW(dwNodeName);
    u32 dwNodeValue = GET_NODENAME(dwNodeName);
    
    u8   byTmp8Data = 0;
    u16  wTmp16Data = 0;
    u32  dwTmp32Data = 0;    
   
    TEqpBasInfo tBasInfo;
    u16 wRet = g_cCfgParse.GetEqpBasCfgByRow( byRow, &tBasInfo );
    if ( SUCCESS_AGENT != wRet )
    {
        Agtlog(LOG_ERROR, "[ProcCallBackMcuEqpBas] get bas info failed, ret:%d !\n", wRet);
        return SNMP_GENERAL_ERROR;
    }
    Agtlog(LOG_VERBOSE, "[CallBackBas] Node<0x%x> received for: %d!\n", dwNodeValue, bRead );
    
    u16 wResult = SNMP_SUCCESS;
    BOOL32 bSupportOpr = TRUE;
    
    switch(dwNodeValue) 
    {
    case NODE_MCUEQPBASID:
        
        if( bRead )
        {
            byTmp8Data = tBasInfo.GetEqpId();
            *(s32*)pBuf = (s32)byTmp8Data;
            *pwBufLen = sizeof(s32);
        }
        else
        {
            bSupportOpr = FALSE;
        }
        break;

    case NODE_MCUEQPBASPORT:

        if( bRead )
        {
            wTmp16Data = tBasInfo.GetMcuRecvPort();
            *(s32*)pBuf = (s32)wTmp16Data;
            *pwBufLen = sizeof(s32);
        }
        else
        {
            tBasInfo.SetMcuRecvPort( *(u16*)pBuf );
        }
        break;

    case NODE_MCUEQPBASSWITCHBRDID:   

        if( bRead )
        {
            byTmp8Data = tBasInfo.GetSwitchBrdId();
            *(s32*)pBuf = (s32)byTmp8Data;
            *pwBufLen = sizeof(s32);
        }
        else
        {
            tBasInfo.SetSwitchBrdId( *(u8*)pBuf );
        }
        break;

    case NODE_MCUEQPBASALIAS:
        {            
            //[5/16/2013 liaokang] 转码
            s8 achEncodingBuf[MAXLEN_EQP_ALIAS+1] = {0};
            u16  wLen = min(sizeof(achEncodingBuf)-1, *pwBufLen);
            if( bRead )
            {                
                if( FALSE == TransEncodingForNmsData(tBasInfo.GetAlias(), achEncodingBuf, sizeof(achEncodingBuf)) )
                {
                    strncpy( achEncodingBuf, tBasInfo.GetAlias(), wLen);
                }
                *pwBufLen = strlen(achEncodingBuf)+1;
                *pwBufLen = min(*pwBufLen, wLen);
                strncpy( (s8*)pBuf, achEncodingBuf, *pwBufLen);
            }     
            else
            {
                if( FALSE == TransEncodingForNmsData((s8*)pBuf, achEncodingBuf, wLen, TRUE) )
                {
                    strncpy(achEncodingBuf, (s8*)pBuf, wLen);
                }
                tBasInfo.SetAlias(achEncodingBuf);
            }
        }
        break; 

    case NODE_MCUEQPBASRUNNINGBRDID:
        
        if( bRead )
        {
            byTmp8Data = tBasInfo.GetRunBrdId();
            *(s32*)pBuf = (s32)byTmp8Data;
            *pwBufLen = sizeof(s32);
        }
        else
        {
            tBasInfo.SetRunBrdId( *(u8*)pBuf );
        }
        break;

    case NODE_MCUEQPBASIPADDR:

        if( bRead )
        {
            dwTmp32Data = htonl(tBasInfo.GetIpAddr());
            *(u32*)pBuf = dwTmp32Data;
            *pwBufLen = sizeof(dwTmp32Data);
        }
        else
        {
            bSupportOpr = FALSE;
        }
        break;

    case NODE_MCUEQPBASSTARTRECVPORT :

        if( bRead )
        {
            wTmp16Data = tBasInfo.GetEqpRecvPort();
            *(s32*)pBuf = (s32)wTmp16Data;
            *pwBufLen = sizeof(s32);
        }
        else
        {
            tBasInfo.SetEqpRecvPort( *(u16*)pBuf );
        }
        break;

    case NODE_MCUEQPBASCONNSTATE:
    case NODE_MCUEQPBASEXIST:
        
        bSupportOpr = FALSE;
        break;

    default:
        wResult = SNMP_GENERAL_ERROR;
        Agtlog(LOG_ERROR, "[ProcCallBackMcuEqpBas] unexpected node(0x%x) received !\n", dwNodeValue );
        break;
    }

    if ( !bRead )
    {
        u16 wResult = g_cCfgParse.SetEqpBasCfgByRow( byRow, &tBasInfo );
        if( SUCCESS_AGENT != wResult )
        {
            wResult = SNMP_GENERAL_ERROR;
        }        
    }
    if ( !bSupportOpr )
    {
        Agtlog(LOG_WARN, "[ProcCallBackMcuEqpBas] node(0x%x) not support operate for: %d<1.read-0.write> \n", dwNodeValue, bRead);
    }
    return wResult;
}

/*=============================================================================
  函 数 名： ProcCallBackMcuEqpVMP
  功    能： 处理MCU外设VMP信息回调
  算法实现： 
  全局变量： 
  参    数： u32 dwNodeName:    构造节点名
             BOOL32  bRead:     读写模式 0-write，1-read
             void * pBuf:       输入输出数据
             u16 *pwBufLen:     数据长度
  返 回 值： u16 成功 SNMP_SUCCESS
  -----------------------------------------------------------------------------
  修改记录:
  日  期      版本        修改人      修改内容
  2006/07/28  4.0         张宝卿       创建
=============================================================================*/
u16 ProcCallBackMcuEqpVMP(u32 dwNodeName, BOOL32 bRead, void *pBuf, u16* pwBufLen)
{
    u8  byRow       = GET_NODE_ROW(dwNodeName);
    u32 dwNodeValue = GET_NODENAME(dwNodeName);
 
    u8   byTmp8Data = 0;
    u16  wTmp16Data = 0;
    u32  dwTmp32Data = 0;
   
    TEqpVMPInfo tVMPInfo;
    u16 wRet = g_cCfgParse.GetEqpVMPCfgByRow( byRow, &tVMPInfo );
    if ( SUCCESS_AGENT != wRet )
    {
        Agtlog(LOG_ERROR, "[ProcCallBackMcuEqpVMP] get vmp info failed, ret:%d !\n", wRet);
        return SNMP_GENERAL_ERROR;
    }
    Agtlog(LOG_VERBOSE, "[CallBackVMP] Node<0x%x> received for: %d!\n", dwNodeValue, bRead );
    
    u16 wResult = SNMP_SUCCESS;
    BOOL32 bSupportOpr = TRUE;

    switch(dwNodeValue) 
    {
    case NODE_MCUEQPVMPID :

        if( bRead )
        {
            byTmp8Data = tVMPInfo.GetEqpId();
            *(s32*)pBuf = (s32)byTmp8Data;
            *pwBufLen = sizeof(s32);
        }
        else
        {
            bSupportOpr = FALSE;
        }
        break;

    case NODE_MCUEQPVMPPORT :
        
        if( bRead )
        {
            wTmp16Data = tVMPInfo.GetMcuRecvPort();
            *(s32*)pBuf = (s32)wTmp16Data;
            *pwBufLen = sizeof(s32);
        }
        else
        {
            tVMPInfo.SetMcuRecvPort( *(u16*)pBuf );
        }
        break;

    case NODE_MCUEQPVMPSWITCHBRDID :

        if( bRead )
        {
            byTmp8Data = tVMPInfo.GetSwitchBrdId();
            *(s32*)pBuf = (s32)byTmp8Data;
            *pwBufLen = sizeof(s32);
        }
        else
        {
            tVMPInfo.SetSwitchBrdId( *(u8*)pBuf );
        }
        break;

    case NODE_MCUEQPVMPALIAS:
        {            
            //[5/16/2013 liaokang] 转码
            s8 achEncodingBuf[MAXLEN_EQP_ALIAS+1] = {0};
            u16  wLen = min(sizeof(achEncodingBuf)-1, *pwBufLen);
            if( bRead )
            {                
                if( FALSE == TransEncodingForNmsData(tVMPInfo.GetAlias(), achEncodingBuf, sizeof(achEncodingBuf)) )
                {
                    strncpy( achEncodingBuf, tVMPInfo.GetAlias(), wLen);
                }
                *pwBufLen = strlen(achEncodingBuf)+1;
                *pwBufLen = min(*pwBufLen, wLen);
                strncpy( (s8*)pBuf, achEncodingBuf, *pwBufLen);
            }     
            else
            {
                if( FALSE == TransEncodingForNmsData((s8*)pBuf, achEncodingBuf, wLen, TRUE) )
                {
                    strncpy(achEncodingBuf, (s8*)pBuf, wLen);
                }
                tVMPInfo.SetAlias(achEncodingBuf);
            }
        }
        break;

    case NODE_MCUEQPVMPRUNNINGBRDID :
        
        if( bRead )
        {
            byTmp8Data = tVMPInfo.GetRunBrdId();
            *(s32*)pBuf = (s32)byTmp8Data;
            *pwBufLen = sizeof(s32);
        }
        else
        {
            tVMPInfo.SetRunBrdId( *(u8*)pBuf );
        }
        break;

    case NODE_MCUEQPVMPIPADDR :

        if( bRead )
        {
            dwTmp32Data = htonl(tVMPInfo.GetIpAddr());
            *(u32*)pBuf = dwTmp32Data;
            *pwBufLen = sizeof(dwTmp32Data);
        }
        else
        {
            bSupportOpr = FALSE;
        }
        break;

    case NODE_MCUEQPVMPSTARTRECVPORT : 
        
        if( bRead )
        {
            wTmp16Data = tVMPInfo.GetEqpRecvPort();
            *(s32*)pBuf = (s32)wTmp16Data;
            *pwBufLen = sizeof(s32);
        }
        else
        {
            tVMPInfo.SetEqpRecvPort( *(u16*)pBuf );
        }
        break;

    case NODE_MCUEQPVMPENCODERNUM: 
        
        if( bRead )
        {
            byTmp8Data = tVMPInfo.GetEncodeNum();
            *(s32*)pBuf = (s32)byTmp8Data;
            *pwBufLen = sizeof(s32);
        }
        else
        {
            tVMPInfo.SetEncodeNum( *(u8*)pBuf );
        }
        break;

    default:
        wResult = SNMP_GENERAL_ERROR;
        Agtlog(LOG_ERROR, "[ProcCallBackMcuEqpVMP] unexpected node(0x%x) received !\n", dwNodeValue );
        break;
    }
    if ( !bRead )
    {
        u16 wResult = g_cCfgParse.SetEqpVMPCfgByRow( byRow, &tVMPInfo );
        if( SUCCESS_AGENT != wResult )
        {
            wResult = SNMP_GENERAL_ERROR;
        }        
    }
    if ( !bSupportOpr )
    {
        Agtlog(LOG_WARN, "[ProcCallBackMcuEqpVMP] node(0x%x) not support operate for: %d<1.read-0.write> \n", dwNodeValue, bRead);
    }
    return wResult;
}

/*=============================================================================
  函 数 名： ProcCallBackMcuEqpDcs
  功    能： 处理MCU外设DCS信息回调
  算法实现： 
  全局变量： 
  参    数： u32 dwNodeName:    构造节点名
             BOOL32  bRead:     读写模式 0-write，1-read
             void * pBuf:       输入输出数据
             u16 *pwBufLen:     数据长度
  返 回 值： u16 成功 SNMP_SUCCESS
  -----------------------------------------------------------------------------
  修改记录:
  日  期      版本        修改人      修改内容
  2006/07/28  4.0         张宝卿       创建
=============================================================================*/
u16 ProcCallBackMcuEqpDcs(u32 dwNodeName, BOOL32 bRead, void *pBuf, u16* pwBufLen)
{
    u8  byRow       = GET_NODE_ROW(dwNodeName);
    u32 dwNodeValue = GET_NODENAME(dwNodeName);
    
    u32 dwTmp32Data = 0;
    u16 wResult = SNMP_SUCCESS;
    
    switch(dwNodeValue) 
    {
    case NODE_MCUEQPDCSIPADDR :  
        
        Agtlog(LOG_VERBOSE, "NODE_MCUEQPDCSIPADDR \treceived For: %d!\n", bRead );
        if( bRead )
        {
            dwTmp32Data = g_cCfgParse.GetDcsIp();
            *(u32*)pBuf = dwTmp32Data;           
            *pwBufLen = sizeof(dwTmp32Data);
        }
        else
        {
            dwTmp32Data = *(u32*)pBuf;
            u16 wResult = g_cCfgParse.SetDcsIp( dwTmp32Data );
            if( SUCCESS_AGENT != wResult )
            {
                wResult = SNMP_GENERAL_ERROR;
            }
        }
        break;
        
    case NODE_MCUEQPDCSCONNSTATE:
        
        Agtlog(LOG_WARN, "NODE_MCUEQPDCSCONNSTATE didn't support operate!\n");
        break;
        
    default:
        wResult = SNMP_GENERAL_ERROR;
        Agtlog(LOG_ERROR, "[ProcCallBackMcuEqpDcs] unexpected node(0x%x) received !\n", dwNodeValue );
        break;
    }
    return wResult;
}


/*=============================================================================
  函 数 名： ProcCallBackMcuEqpPrs
  功    能： 处理MCU外设prs信息回调
  算法实现： 
  全局变量： 
  参    数： u32 dwNodeName:    构造节点名
             BOOL32  bRead:     读写模式 0-write，1-read
             void * pBuf:       输入输出数据
             u16 *pwBufLen:     数据长度
  返 回 值： u16 成功 SNMP_SUCCESS
  -----------------------------------------------------------------------------
  修改记录:
  日  期      版本        修改人      修改内容
  2006/07/28  4.0         张宝卿       创建
=============================================================================*/
u16 ProcCallBackMcuEqpPrs(u32 dwNodeName, BOOL32 bRead, void *pBuf, u16* pwBufLen)
{
    u8  byRow       = GET_NODE_ROW(dwNodeName);
    u32 dwNodeValue = GET_NODENAME(dwNodeName);
    
    u8   byTmp8Data = 0;
    u16  wTmp16Data = 0;
    u32  dwTmp32Data = 0;
    BOOL32 bResult = FALSE;
   
    TEqpPrsInfo tPrsInfo;
    u16 wRet = g_cCfgParse.GetEqpPrsCfgByRow( byRow, &tPrsInfo );
    if ( SUCCESS_AGENT != wRet )
    {
        Agtlog(LOG_ERROR, "[ProcCallBackMcuEqpPrs] get prs info failed, ret:%d !\n", wRet);
        return SNMP_GENERAL_ERROR;
    }
    Agtlog(LOG_VERBOSE, "[CallBackPrs] Node<0x%x> received for: %d!\n", dwNodeValue, bRead );
    
    u16 wResult = SNMP_SUCCESS;
    BOOL32 bSupportOpr = TRUE;
    
    switch(dwNodeValue) 
    {
    case NODE_MCUEQPPRSID:   
        
        if( bRead )
        {
            byTmp8Data = tPrsInfo.GetEqpId();
            *(s32*)pBuf = (s32)byTmp8Data;
            *pwBufLen = sizeof(s32);
        }
        else
        {
            bSupportOpr = FALSE;
        }
        break;

    case NODE_MCUEQPPRSMCUSTARTPORT: 
        
        if( bRead )
        {
            *(s32*)pBuf = (s32)tPrsInfo.GetMcuRecvPort();
            *pwBufLen = sizeof(s32);
        }
        else
        {
            tPrsInfo.SetMcuRecvPort( *(u16*)pBuf );
        }
        break;

    case NODE_MCUEQPPRSSWITCHBRDID :  
        
        if( bRead )
        {
            *(s32*)pBuf = (s32)tPrsInfo.GetSwitchBrdId();
            *pwBufLen = sizeof(s32);
        }
        else
        {
            tPrsInfo.SetSwitchBrdId( *(u8*)pBuf );
        }
        break;

    case NODE_MCUEQPPRSALIAS:
        {            
            //[5/16/2013 liaokang] 转码
            s8 achEncodingBuf[MAXLEN_EQP_ALIAS+1] = {0};
            u16  wLen = min(sizeof(achEncodingBuf)-1, *pwBufLen);
            if( bRead )
            {                
                if( FALSE == TransEncodingForNmsData(tPrsInfo.GetAlias(), achEncodingBuf, sizeof(achEncodingBuf)) )
                {
                    strncpy( achEncodingBuf, tPrsInfo.GetAlias(), wLen);
                }
                *pwBufLen = strlen(achEncodingBuf)+1;
                *pwBufLen = min(*pwBufLen, wLen);
                strncpy( (s8*)pBuf, achEncodingBuf, *pwBufLen);
            }     
            else
            {
                if( FALSE == TransEncodingForNmsData((s8*)pBuf, achEncodingBuf, wLen, TRUE) )
                {
                    strncpy(achEncodingBuf, (s8*)pBuf, wLen);
                }
                tPrsInfo.SetAlias(achEncodingBuf);
            }
        }
        break;

    case NODE_MCUEQPPRSRUNNINGBRDID :  
        
        if( bRead )
        {
            *(s32*)pBuf = (s32)tPrsInfo.GetRunBrdId();
            *pwBufLen = sizeof(s32);
        }
        else
        {
            tPrsInfo.SetRunBrdId( *(u8*)pBuf );
        }
        break;

    case NODE_MCUEQPPRSIPADDR :       
        
        if( bRead )
        {
            *(u32*)pBuf = htonl(tPrsInfo.GetIpAddr());
            *pwBufLen = sizeof(dwTmp32Data);
        }
        else
        {
            bSupportOpr = FALSE;
        }
        break;

    case NODE_MCUEQPPRSSTARTPORT : 
        
        if( bRead )
        {
            *(s32*)pBuf = (s32)tPrsInfo.GetEqpRecvPort();
            *pwBufLen = sizeof(s32);
        }
        else
        {
            tPrsInfo.SetEqpRecvPort( *(u16*)pBuf );
        }
        break;

    case NODE_MCUEQPPRSFIRSTTIMESPAN : 
        
        if( bRead )
        {
            *(s32*)pBuf = (s32)tPrsInfo.GetFirstTimeSpan();
            *pwBufLen = sizeof(s32);
        }
        else
        {
            tPrsInfo.SetFirstTimeSpan( *(u16*)pBuf );
        }
        break;

    case NODE_MCUEQPPRSSECONDTIMESPAN : 
        
        if( bRead )
        {
            *(s32*)pBuf = (s32)tPrsInfo.GetSecondTimeSpan();
            *pwBufLen = sizeof(s32);
        }
        else
        {
            tPrsInfo.SetSecondTimeSpan( *(u16*)pBuf );
        }
        break;

    case NODE_MCUEQPPRSTHIRDTIMESPAN :
        
        if( bRead )
        {
            *(s32*)pBuf = (s32)tPrsInfo.GetThirdTimeSpan();
            *pwBufLen = sizeof(s32);
        }
        else
        {
            tPrsInfo.SetThirdTimeSpan( *(u16*)pBuf );
        }
        break;

    case NODE_MCUEQPPRSREJECTTIMESPAN: 
        
        if( bRead )
        {
            *(s32*)pBuf = (s32)tPrsInfo.GetRejectTimeSpan();
            *pwBufLen = sizeof(s32);
        }
        else
        {
            tPrsInfo.SetRejectTimeSpan( *(u16*)pBuf );
        }
        break;

    default:
        wResult = SNMP_GENERAL_ERROR;
        Agtlog(LOG_ERROR, "[ProcCallBackMcuEqpPrs] unexpected node(0x%x) received !\n", dwNodeValue );
        break;
    }

    if ( !bRead )
    {
        u16 wResult = g_cCfgParse.SetEqpPrsCfgByRow( byRow, &tPrsInfo );
        if( SUCCESS_AGENT != wResult )
        {
            wResult = SNMP_GENERAL_ERROR;
        }        
    }
    if ( !bSupportOpr )
    {
        Agtlog(LOG_WARN, "[ProcCallBackMcuEqpPrs] node(0x%x) not support operate for: %d<1.read-0.write> \n", dwNodeValue, bRead);
    }
    return wResult;
}

/*=============================================================================
  函 数 名： ProcCallBackMcuEqpNetSync
  功    能： 处理MCU外设网同步信息回调
  算法实现： 
  全局变量： 
  参    数： u32 dwNodeName:    构造节点名
             BOOL32  bRead:     读写模式 0-write，1-read
             void * pBuf:       输入输出数据
             u16 *pwBufLen:     数据长度
  返 回 值： u16 成功 SNMP_SUCCESS
  -----------------------------------------------------------------------------
  修改记录:
  日  期      版本        修改人      修改内容
  2006/07/28  4.0         张宝卿       创建
=============================================================================*/
u16 ProcCallBackMcuEqpNetSync(u32 dwNodeName, BOOL32 bRead, void *pBuf, u16* pwBufLen)
{
    u8  byRow       = GET_NODE_ROW(dwNodeName);
    u32 dwNodeValue = GET_NODENAME(dwNodeName);
    
    u8   byTmp8Data = 0;
    BOOL32 bResult = FALSE;
    u16 wResult = SNMP_SUCCESS;
	*pwBufLen = sizeof(s32);

	TNetSyncInfo tNetSyncInfo;
	g_cCfgParse.GetNetSyncInfo( &tNetSyncInfo );
    Agtlog(LOG_VERBOSE, "[NetSyncCallBack] Node(0x%x) received for: %d!\n", dwNodeValue, bRead );

    switch(dwNodeValue) 
    {
    case NODE_MCUEQPNETSYNCMODE : 
        
        if( bRead )
        {
            *(s32*)pBuf = (s32)tNetSyncInfo.GetMode();
        }
        else
        {
            tNetSyncInfo.SetMode( *(u8*)pBuf );
        }
        break;
        
    case NODE_MCUEQPNETSYNCDTSLOT:  
        
        if( bRead )
        {
            *(s32*)pBuf = (s32)tNetSyncInfo.GetDTSlot();
        }
        else
        {
            tNetSyncInfo.SetDTSlot( byTmp8Data );
        }
        break;
        
    case NODE_MCUEQPNETSYNCE1INDEX :  
        
        if( bRead )
        {
            *(s32*)pBuf = (s32)tNetSyncInfo.GetE1Index();
        }
        else
        {
            tNetSyncInfo.SetE1Index( *(u8*)pBuf );
        }
        break;
        
    case NODE_MCUEQPNETSYNCSTATE:
		break;
		
    default:
        wResult = SNMP_GENERAL_ERROR;
        Agtlog(LOG_ERROR, "[ProcCallBackMcuEqpNetSync] unexpected node(0x%x) received !\n", dwNodeValue );
        break;
    }
	if ( !bRead ) 
	{
		wResult = g_cCfgParse.SetNetSyncInfo( &tNetSyncInfo );
	}
    return wResult;
}


/*=============================================================================
  函 数 名： ProcCallBackMcuPfmInfo
  功    能： 处理MCU性能信息回调
  算法实现： 
  全局变量： 
  参    数： u32 dwNodeName:    构造节点名
             BOOL32  bRead:     读写模式 0-write，1-read
             void * pBuf:       输入输出数据
             u16 *pwBufLen:     数据长度
  返 回 值： u16 成功 SNMP_SUCCESS
  -----------------------------------------------------------------------------
  修改记录:
  日  期      版本        修改人      修改内容
  2010/03/18  5.0         陆昆朋       创建
  2010/11/23  5.0         苗庆松       修改
=============================================================================*/
u16 ProcCallBackMcuPfmInfo(u32 dwNodeName, BOOL32 bRead, void *pBuf, u16* pwBufLen)
{
    u8  byRow       = GET_NODE_ROW(dwNodeName);
    u32 dwNodeValue = GET_NODENAME(dwNodeName);
    
    u16 wResult = SNMP_SUCCESS;
	BOOL32 bSupportOpr = TRUE;

	*pwBufLen = sizeof(s32);

	TMcuPfmInfo tMcuPfmInfo;
	memset(&tMcuPfmInfo, 0, sizeof(tMcuPfmInfo));
	g_cCfgParse.GetMcuPfmInfo( &tMcuPfmInfo );
    Agtlog(LOG_VERBOSE, "[ProcCallBackMcuPfmInfo] Node(0x%x) received for: %d!\n", dwNodeValue, bRead );

    switch(dwNodeValue) 
    {
    case NODE_MCUPFMCPURATE : 
        
        if( bRead )
        {
            *(s32*)pBuf = (s32)g_cCfgParse.GetCpuRate();
        }
        else
        {
            bSupportOpr = FALSE;
        }
        break;
        
    case NODE_MCUPFMMEMORY:  
        
        if( bRead )
        {
            *(s32*)pBuf = g_cCfgParse.GetMemoryRate();
        }
        else
        {
            bSupportOpr = FALSE;
        }
        break;
        
    case NODE_MCUPFMMTNUM :  
        
        if( bRead )
        {
            *(s32*)pBuf = (s32)tMcuPfmInfo.GetTotalMtNum();
        }
        else
        {
            bSupportOpr = FALSE;
        }
        break;
        
    case NODE_MCUPFMCONFNUM:
        if( bRead )
        {
            *(s32*)pBuf = (s32)tMcuPfmInfo.GetConfNum();
        }
        else
        {
            bSupportOpr = FALSE;
        }
		break;

	case NODE_MCUPFMEQPUSEDRATE:  
        
        if( bRead )
        {
			*(u32*)pBuf = SM_STAT_EQP_TYPE_MAX;
			Agtlog(LOG_VERBOSE, "GetEqpBuffer %ld\n", *(u32*)pBuf);

			*pwBufLen = tMcuPfmInfo.GetEqpBuffer((s8 *)pBuf);
        }
        else
        {
            bSupportOpr = FALSE;
        }
        break;

	case NODE_MCUPFMAUTHRATE:
        
        if( bRead )
        {
			u32 dwValue = MAKEDWORD(tMcuPfmInfo.GetAuthMTNum(), tMcuPfmInfo.GetAuthMTTotal());
            *(s32*)pBuf = dwValue;

			Agtlog(LOG_ERROR, "[ProcCallBackMcuEqpNetSync] NODE_MCUPFMAUTHRATE total:(%d); used:(%d)!\n", 
				tMcuPfmInfo.GetAuthMTTotal(), tMcuPfmInfo.GetAuthMTNum());
        }
        else
        {
            bSupportOpr = FALSE;
        }
        break;	
		
	case NODE_MCUPFMMACHTEMPER:  
        
        if( bRead )
        {
			//暂时底层还没有相关的接口，等底层完成后添加
			//调试使用，默认为20
            *(s32*)pBuf = 20;
        }
        else
        {
            bSupportOpr = FALSE;
        }
        break;	

    // 新增 在线MCS的IP信息  [04/24/2012 liaokang] 
    case NODE_MCUPFMMCSCONNECTIONS:
        
        if( bRead )
        {
            s8 abyBuf[256] = {0};
            abyBuf[255] = '\0';
            BOOL32 nRet;
            u16 wBufLen;
            nRet = g_cCfgParse.GetRecombinedRegedMcsIpInfo( abyBuf, wBufLen );
            if ( !nRet )
            {
                *pwBufLen = 2;
            }
            else
            {
                *pwBufLen = wBufLen;
            }
            memcpy( pBuf, abyBuf, *pwBufLen );
        }
        else
        {
            bSupportOpr = FALSE;
        }
        break;	

    default:
        wResult = SNMP_GENERAL_ERROR;
        Agtlog(LOG_ERROR, "[ProcCallBackMcuEqpNetSync] unexpected node(0x%x) received !\n", dwNodeValue );
        break;
    }

	if ( !bSupportOpr )
    {
        Agtlog(LOG_WARN, "[ProcCallBackMcuPfmInfo] node(0x%x) not support operate for: %d<1.read-0.write> \n", dwNodeValue, bRead);
    }

    return wResult;
}

/*=============================================================================
  函 数 名： InitalData
  功    能： 初始化类数据
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void 
// 注：没有考虑取信号量的失败
=============================================================================*/
BOOL32 CMsgCenterInst::Initialize(void)
{
	printf("CMsgCenterInst::Initialize 1\n");
	OspSemTake( g_hSync );

	BOOL32 bRet = TRUE;
    m_pcSnmpNodes = new(CNodes);
    if( NULL == m_pcSnmpNodes )
    {
        printf("[AgentMsgCenter] Fail to new a CNodes.\n");
        bRet = FALSE;
    }
	else
	{
		m_pcAgentSnmp = new(CAgentSnmp);
		if( NULL == m_pcAgentSnmp )
		{
			printf("[AgentMsgCenter] Fail to new CAgentSnmp.\n");
			if( NULL != m_pcSnmpNodes )
			{
				delete m_pcSnmpNodes;
				m_pcSnmpNodes = NULL;
			}
			bRet = FALSE;
		}
	}

#ifdef _LINUX_

	if(FTPC_OK != FTPCInit()) 
	{         
		printf("FTPCInit 初始化失败\n"); 
    } 

	if (FTPC_OK != FTPCRegCallback(ProcCbFtpcCallbackFun))
	{
		printf("FTPCRegCallback 初始化失败\n"); 
	}
#endif

	if( !bRet )
	{
		printf("\n\n fail to alloc memory for snmp nodes \n\n");
	}
	else
	{
		OspSemGive( g_hSync );	// 成功，释放信号量
	}

	printf("CMsgCenterInst::Initialize 2\n");

    return bRet;
}

/*=============================================================================
  函 数 名： Quit
  功    能： 释放数据
  算法实现： 
  全局变量： 
  参    数： void
  返 回 值： BOOL32 
=============================================================================*/
BOOL32 CMsgCenterInst::Quit(void)
{   
    if( NULL != m_pcSnmpNodes )
	{
        m_pcSnmpNodes->Clear();
		delete m_pcSnmpNodes;
		m_pcSnmpNodes = NULL;
	}

	if( NULL != m_pcAgentSnmp )
    {
		delete m_pcAgentSnmp;
		m_pcAgentSnmp = NULL;
	}

    return TRUE;
}

/*=============================================================================
    函数名      ：InstanceEntry
    功能        ：实例消息处理入口函数，必须override
    算法实现    ：
    引用全局变量：
    输入参数说明：CMessage * const pcMsg, 传入的消息
    返回值说明  ：
-------------------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
=============================================================================*/
void CMsgCenterInst::InstanceEntry(CMessage *const pcMsg)
{
	Agtlog(LOG_VERBOSE, "Msg %u(%s) received!\n", pcMsg->event, ::OspEventDesc( pcMsg->event ) );

    if( NULL == pcMsg )
	{
		Agtlog(LOG_ERROR, "[InstanceEntry] Error input parameter!\n");
		return;
	}

    switch( pcMsg->event )
    {
	case SVC_AGT_CONFERENCE_START:
	case SVC_AGT_CONFERENCE_STOP:
	case SVC_AGT_ADD_MT:		              
	case SVC_AGT_DEL_MT:                    
		ProcConfStatusChange( pcMsg );
		break;

	case SVC_AGT_POWER_STATUS:				  // 电源状态改变
		ProcPowerStatusChange( pcMsg );
		break;

	case SVC_AGT_POWER_FAN_STATUS:            // 电源风扇状态改变
		ProcPowerFanStatusChange( pcMsg );
		break;

	case SVC_AGT_BOX_FAN_STATUS:              // 机箱风扇状态改变  [miaoqingsong add]
        ProcBoxFanStatusChange( pcMsg );
		break;

	case SVC_AGT_CPU_STATUS:                  // MPC板CPU状态改变  [miaoqingsong add]
		ProcMPCCpuStatusChange( pcMsg );    
		break;

	case SVC_AGT_MPCMEMORY_STATUS:            // MPC板Memory状态改变 [miaoqingsong add]
		ProcMPCMemoryStatusChange( pcMsg );  
		break;

	case SVC_AGT_CPU_FAN_STATUS:
		ProcCpuFanStatusChange( pcMsg );	  // CPU风扇状态改变
		break;

	case SVC_AGT_CPUTEMP_STATUS:
		ProcCpuTempStatusChange( pcMsg );	  // CPU温度状态改变
		break;

	case SVC_AGT_POWERTEMP_STATUS:            // 电源板温度状态改变 [miaoqingsong add]
		ProcPowerTempStatusChange( pcMsg );
		break;

	case SVC_AGT_MPC2TEMP_STATUS:             // MPC2板温度过高状态改变 [miaoqingsong add]
		ProcMpc2TempStatusChange( pcMsg );
		break;

	case SVC_AGT_MODULE_STATUS:				  // 模块状态改变(DIC CRI板产生)
		ProcModuleStatusChange( pcMsg );
		break;

    case SVC_AGT_LED_STATUS:				  // 由单板守卫发过来
        ProcLedStatusChange(pcMsg);
        break;
		
	case SVC_AGT_SDH_STATUS:				 // SDH状态改变，由单板守卫发过来
		ProcSDHStatusChange( pcMsg );
		break;

    case MCU_AGT_BOARD_STATUSCHANGE:		 // 单板状态改变
    case SVC_AGT_STANDBYMPCSTATUS_NOTIFY:    // 备MPC状态改变        
		ProcBoardStatusChange( pcMsg );
        break;

    case MCU_AGT_LINK_STATUSCHANGE:			 // 链路状态改变
		ProcLinkStatusChange( pcMsg );
        break;

	case SVC_AGT_BRD_TEMPERATURE_STATUS_NOTIFY:         //单板温度变化，目前单板支持DRI2
		ProcBoardTempStatusChange(pcMsg);
		break;

	case SVC_AGT_BRD_CPU_STATUS_NOTIF:		 //单板CPU状态改变，目前支持DRI，DRI2
		ProcBoardCpuStatusChange(pcMsg);
		break;

    case MCU_AGT_SYNCSOURCE_STATUSCHANGE:    // 同步源状态改变
		ProcNetSyncStatusChange( pcMsg );
        break;

	case MCU_BRD_FAN_STATUS:
		ProcBrdFanStatusChange( pcMsg );	 // Imt风扇状态改变
		break;

    case SVC_AGT_MEMORY_STATUS:				 // 内存状态改变
    case SVC_AGT_FILESYSTEM_STATUS:			 // 文件系统状态改变
	case SVC_AGT_TASK_STATUS:				 // 任务状态改变
		ProcSoftwareStatusChange( pcMsg );
		break;

	case AGT_SVC_POWEROFF:					 // 业务发过来的待机消息
	case AGT_SVC_POWERON:					 // 业务发过来的从待机到运行消息
    case SVC_AGT_COLD_RESTART:				 // 冷启动完成
    case SVC_AGT_POWEROFF:					 // 关机
		ProcPowerOnOffStatusChange( pcMsg );
        break;

	case SVC_AGT_CONFIG_ERROR:
        ProcGetConfigError();				 // 读配置文件出错消息
        break;
#ifdef _MINIMCU_
	case SVC_AGT_SETDSCINFO_REQ:			 // 设置DSC info, zgc, 2007-07-17
		ProcSetDscInfoReq( pcMsg );
		break;

	case AGT_SVC_DSCREGSUCCEED_NOTIF:		 // 通知MCU DSC板已经注册成功, zgc, 2007-07-31
		ProcDscRegSucceedNotif( pcMsg );
		break;

	case BOARD_MPC_SETDSCINFO_ACK:
	case BOARD_MPC_SETDSCINFO_NACK:
		ProcSetDscInfoRsp( pcMsg );
		break;
#endif

    case EV_MSGCENTER_POWER_ON:
		Initialize();						 // 初始化数据
		break;

    case EV_TOMSGCENTER_INITAL_SNMPINFO:
        ProcSnmpInfo();						 // 初始化Snmp参数
        break;

    case EV_MSGCENTER_POWER_OFF:
        ProcPowerOffAlarm();				 // 关机告警(代理退出时发送)
        break;

    case EV_BOARD_LED_ALARM:                 // 单板管理发过来的Led告警
    case EV_BOARD_ETHPORT_ALARM:			 // 单板管理发过来的网口告警
        /*ProcLedAlarm( pcMsg );*/
        ProcBoardStatusAlarm( pcMsg );       // 处理外设单板告警（Led/网口状态） [05/04/2012 liaokang]
        break;

    case EV_AGENT_COLD_RESTART:
        ProcColdStartdAlarm();
        break;
        
    // xsl [8/17/2006] mcu通知代理更新单板版本（包括自己）
    case SVC_AGT_UPDATEBRDVERSION_CMD:
        ProcMcsUpdateBrdCmd(pcMsg);
        break;

#ifdef _MINIMCU_
	case SVC_AGT_STARTUPDATEDSCVERSION_REQ:
	case SVC_AGT_DSCUPDATEFILE_REQ:
		ProcMcsUpdateDscMsg(pcMsg);
		break;
#endif

	case SVC_AGT_DISCONNECTBRD_CMD:
	case SVC_AGT_BOARDCFGMODIFY_NOTIF:
#ifdef _MINIMCU_
	case SVC_AGT_DSCTELNETLOGININFO_UPDATE_CMD:
	case SVC_AGT_DSCGKINFO_UPDATE_CMD:
#endif	
		ProcMsgToBrdManager(pcMsg);
		break;

    case EV_MSGCENTER_NMS_UPDATEDMPC_CMD:
        ProcNMSUpdateBrdCmd( pcMsg );
        break;

    case EV_SM_UPDATEDMPC_CMD:
        ProcSmUpdateBrdCmd( );
        break;

	case MCUAGENT_UPDATE_MCUPFMINFO_TIMER:
		ProcUpdateMcuPfmInfoTimer();
		break;

	case MCUAGENT_OPENORCLOSE_MPCLED_TIMER:
		ProcOpenorcloseMpcLedTimer();
		break;

	/*case SVC_AGT_BRD_SOFTWARE_VERSION_NOTIFY:
		ProcBoardSoftwareVersionNotif(pcMsg);
		break;
	*/

    default:
        Agtlog(LOG_ERROR, "[InstanceEntry] unknown message %u(%d) received!\n", 
			                    pcMsg->event, ::OspEventDesc( pcMsg->event ) );
        break;
    }

    return;
}

/*=============================================================================
  函 数 名： ProcUpdateMcuPfmInfoTimer
  功    能： 处理MCU性能信息定时Trap上报给网管
  算法实现： 
  全局变量： 
  参    数： CMessage* const pcMsg
  返 回 值： void
  -----------------------------------------------------------------------------
  修改记录:
  日  期      版本        修改人           修改内容
  2010/03/18  5.0         陆昆朋             创建
  2010/11/23  5.0         苗庆松             修改
=============================================================================*/
void CMsgCenterInst::ProcUpdateMcuPfmInfoTimer( void )
{
	if (!m_pcSnmpNodes) return;

	TMcuPfmInfo tMcuPfmInfo;
	memset(&tMcuPfmInfo, 0, sizeof(tMcuPfmInfo));
	g_cCfgParse.GetMcuPfmInfo( &tMcuPfmInfo );

	u32 dwCpuRate = (u32)g_cCfgParse.GetCpuRate();          
	u32 dwMemoryRate = g_cCfgParse.GetMemoryRate();
	u32 dwMtNum = (u32)tMcuPfmInfo.GetTotalMtNum();
	u32 dwConfNum = (u32)tMcuPfmInfo.GetConfNum();

	s8 aszEqpBuff[AGENT_MAX_EQPNODEBUFF_LEN] = {0};
	u16 wBuffSize = tMcuPfmInfo.GetEqpBuffer(aszEqpBuff);
    
	u32 dwAuthMTRate = MAKEDWORD(tMcuPfmInfo.GetAuthMTNum(),tMcuPfmInfo.GetAuthMTTotal());

	time_t tTime = time(NULL);

	m_pcSnmpNodes->Clear();
	m_pcSnmpNodes->SetTrapType( TRAP_MCU_PFMINFO );

	// [20101123 miaoqingsong] 添加CPU 内存使用率和接入授权数占用率Trap信息上报
	m_pcSnmpNodes->AddNodeValue(NODE_MCUPFMCPURATE, &dwCpuRate, sizeof(u32));
	m_pcSnmpNodes->AddNodeValue(NODE_MCUPFMMEMORY, &dwMemoryRate, sizeof(u32));
	m_pcSnmpNodes->AddNodeValue(NODE_MCUPFMMTNUM, &dwMtNum, sizeof(u32));
	m_pcSnmpNodes->AddNodeValue(NODE_MCUPFMCONFNUM, &dwConfNum, sizeof(u32));
	m_pcSnmpNodes->AddNodeValue(NODE_MCUPFMEQPUSEDRATE, aszEqpBuff, wBuffSize);
	m_pcSnmpNodes->AddNodeValue(NODE_MCUPFMAUTHRATE, &dwAuthMTRate, sizeof(u32));
	m_pcSnmpNodes->AddNodeValue(NODE_MCUPFMTIME, &tTime, sizeof(u32));

	if(m_pcAgentSnmp)
		m_pcAgentSnmp->AgentSendTrap( *m_pcSnmpNodes );

    // [20101123 miaoqingsong] 设置定时器每60s以Trap方式向网管发一次设备性能信息
	SetTimer( MCUAGENT_UPDATE_MCUPFMINFO_TIMER, UPDATE_PFMINFO_TIME_OUT ); 
}

/*=============================================================================
  函 数 名：ProcOpenorcloseMpcLedTimer
  功    能：处理MPC板NMS灯状态
  算法实现：
  全局变量：BOOL32    g_bNmsLedState
            BOOL32    g_bNmsOnline
  参    数：CMessage* const pcMsg
  返 回 值：void
  ----------------------------------------------------------------------------
  修改记录：
  日   期        版本        修改人        修改内容
  2010/11/18     5.0         苗庆松          创建
=============================================================================*/
void CMsgCenterInst::ProcOpenorcloseMpcLedTimer( void )
{
	if ( TRUE == g_bNmsOnline )
	{
		if( !g_bNmsLedState)
		{
			if ( OK == BrdLedStatusSet(LED_SYS_LINK, BRD_LED_ON) )
			{
				g_bNmsLedState = TRUE;
			}
		}
	}
	else 
	{
		if ( OK == BrdLedStatusSet(LED_SYS_LINK, BRD_LED_OFF) )
		{
			g_bNmsLedState = FALSE;
		}
	}

	g_bNmsOnline = FALSE;
	
	// 定时扫描Get调用回调函数的情况
	SetTimer( MCUAGENT_OPENORCLOSE_MPCLED_TIMER, OPENORCLOSE_MPCLED_TIME_OUT );  
}

/*=============================================================================
  函 数 名： ProcSnmpInfo
  功    能： 初始化Snmp参数
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void 
=============================================================================*/
void CMsgCenterInst::ProcSnmpInfo()
{
	printf("CMsgCenterInst::ProcSnmpInfo 1\n");
	
	TSnmpAdpParam tSnmpParam;
	g_cCfgParse.GetSnmpParam( tSnmpParam );
    
	// [2010/03/10 miaoqingsong] 解决设备重启崩溃问题，重新调整回调函数的调用顺序
	if( m_pcAgentSnmp && SNMP_SUCCESS == m_pcAgentSnmp->Initialize( AgentCallBack, &tSnmpParam ) )
	{
			//m_pcAgentSnmp->SetAgentCallBack( AgentCallBack );
			printf("[ProcSnmpInfo] initialize snmp succeed!\n");
			Agtlog(LOG_ERROR, "[ProcSnmpInfo] initialize snmp succeed!\n");
	}
	else
	{
			printf("[ProcSnmpInfo] fail to initialize snmp .\n");
			Agtlog(LOG_ERROR, "[ProcSnmpInfo] fail to initialize snmp .\n");
	}	
    
	printf("CMsgCenterInst::ProcSnmpInfo 2\n");
    return;
}

/*=============================================================================
  函 数 名： ProcConfStatusChange
  功    能： MCU与会状态Trap信息上报网管
  算法实现： 
  全局变量： 
  参    数： CMessage* const pcMsg
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录:
  日  期        版本       修改人            修改内容
  2010/11/23    5.0        苗庆松              创建(MT与会状态信息改由终端自己发)
================================================================================*/
void CMsgCenterInst::ProcConfStatusChange( CMessage* const pcMsg )
{
    if(NULL == pcMsg)
    {
        Agtlog(LOG_ERROR, "[ProcConfStatusChange] The pointer can not be Null\n");
        return;
    }
    if ( !g_cCfgParse.HasSnmpNms() )
    {
        Agtlog(LOG_WARN, "[ProcConfStatusChange] There's no NMS configed !\n");
        return;
    }
    
	u8              byIndex = 0;
	u32             dwState = 0;
    s8              achConfId[32];
    time_t          tCurTime;
    struct tm       *ptTime = NULL;
	TMcuConfState   tConfState;
	TMcuMtConfState tMtConfState;
	TConfNotify     *ptConfNotify = NULL;
	TMtNotify       *ptMtNotify = NULL;
	CConfId         cConfId;
    
    memset(achConfId, 0, sizeof(achConfId));
    memset(&cConfId, 0, sizeof(CConfId));
    memset( &tConfState, 0, sizeof(tConfState) );
	memset( &tMtConfState, 0, sizeof(tMtConfState) );

	TMcuPfmInfo* ptMcuPfmInfo = g_cCfgParse.GetMcuPfmInfo();
		
	switch( pcMsg->event )
    {
	case SVC_AGT_CONFERENCE_START:
		{
			
			ptConfNotify = ( TConfNotify* )pcMsg->content;
			if (NULL == ptConfNotify) break;
			
			u8 byConfNtyconfIdLen = sizeof(ptConfNotify->m_abyConfId);
			u8 byConfStateConfIdLen = sizeof(tConfState.m_abyConfId);
			memcpy( tConfState.m_abyConfId, ptConfNotify->m_abyConfId, min(byConfNtyconfIdLen, byConfStateConfIdLen) );
			//[5/16/2013 liaokang] 转码
            //strncpy( (s8*)tConfState.m_abyConfName, (s8*)ptConfNotify->m_abyConfName, sizeof( tConfState.m_abyConfName ) );            
            //tConfState.m_abyConfName[ sizeof(tConfState.m_abyConfName) - 1 ] = '\0';
            s8 achEncodingBuf[MAXLEN_CONFNAME] = {0};
            if( FALSE == TransEncodingForNmsData((s8*)ptConfNotify->m_abyConfName, achEncodingBuf, sizeof(achEncodingBuf)) )
            {
                strncpy(achEncodingBuf, (s8*)ptConfNotify->m_abyConfName, sizeof(achEncodingBuf)- 1);
            }
            strncpy( (s8*)tConfState.m_abyConfName, achEncodingBuf, sizeof(tConfState.m_abyConfName) - 1 );            

			tCurTime = time(NULL);
			ptTime = localtime( &tCurTime );
			sprintf((s8*)tConfState.m_abyTime, "%4.4u-%2.2u-%2.2u %2.2u:%2.2u:%2.2u", ptTime->tm_year + 1900, 
				ptTime->tm_mon + 1, ptTime->tm_mday, ptTime->tm_hour, ptTime->tm_min, ptTime->tm_sec );
			tConfState.m_byState = CONF_STATE_START;
			if(m_pcSnmpNodes)
			{
				m_pcSnmpNodes->Clear();
				m_pcSnmpNodes->SetTrapType( TRAP_CONF_STATE );
			}
			dwState = tConfState.m_byState;
			cConfId.SetConfId(tConfState.m_abyConfId, sizeof(tConfState.m_abyConfId));
			cConfId.GetConfIdString(achConfId, sizeof(achConfId));
			
			if(m_pcSnmpNodes)
			{
				m_pcSnmpNodes->AddNodeValue(NODE_MCUCONFSTATENOTIFY, &dwState, sizeof(u32) );
				m_pcSnmpNodes->AddNodeValue(NODE_MCUCONFIDNOTIFY, achConfId, sizeof(achConfId) );
				m_pcSnmpNodes->AddNodeValue(NODE_MCUCONFNAMENOTIFY, tConfState.m_abyConfName, sizeof(tConfState.m_abyConfName) );
				m_pcSnmpNodes->AddNodeValue(NODE_MCUCONFSTARTTIMENOTIFY, tConfState.m_abyTime, sizeof(tConfState.m_abyTime) );
				
				if(m_pcAgentSnmp)
				{
					m_pcAgentSnmp->AgentSendTrap( *m_pcSnmpNodes );
				}
			}
			
			if (ptMcuPfmInfo && !ptMcuPfmInfo->FindConf(cConfId, byIndex))
			{
				ptMcuPfmInfo->AddConf(cConfId);
			}
		}
		
		break;
		
	case SVC_AGT_CONFERENCE_STOP:
		{
			ptConfNotify = ( TConfNotify* )pcMsg->content;
			if (NULL == ptConfNotify) break;
			
			u8 byConfNtyconfIdLen = sizeof(ptConfNotify->m_abyConfId);
			u8 byConfStateConfIdLen = sizeof(tConfState.m_abyConfId);
			memcpy( tConfState.m_abyConfId, ptConfNotify->m_abyConfId, min(byConfNtyconfIdLen, byConfStateConfIdLen) );
            //[5/16/2013 liaokang] 转码
            //strncpy( (s8*)tConfState.m_abyConfName, (s8*)ptConfNotify->m_abyConfName, sizeof( tConfState.m_abyConfName ) );            
            //tConfState.m_abyConfName[ sizeof(tConfState.m_abyConfName) - 1 ] = '\0';
            s8 achEncodingBuf[MAXLEN_CONFNAME] = {0};
            if( FALSE == TransEncodingForNmsData((s8*)ptConfNotify->m_abyConfName, achEncodingBuf, sizeof(achEncodingBuf)) )
            {
                strncpy(achEncodingBuf, (s8*)ptConfNotify->m_abyConfName, sizeof(achEncodingBuf) - 1);
            }
            strncpy( (s8*)tConfState.m_abyConfName, achEncodingBuf, sizeof(tConfState.m_abyConfName) - 1 );            

			tCurTime = time(NULL);
			ptTime = localtime( &tCurTime );
			sprintf((s8*)tConfState.m_abyTime, "%4.4u-%2.2u-%2.2u %2.2u:%2.2u:%2.2u", ptTime->tm_year + 1900, 
				ptTime->tm_mon + 1, ptTime->tm_mday, ptTime->tm_hour, ptTime->tm_min, ptTime->tm_sec );
			tConfState.m_byState = CONF_STATE_STOP;

			if(m_pcSnmpNodes)
			{
				m_pcSnmpNodes->Clear();
				m_pcSnmpNodes->SetTrapType( TRAP_CONF_STATE );
			}
			dwState = tConfState.m_byState;
			cConfId.SetConfId(tConfState.m_abyConfId, sizeof(tConfState.m_abyConfId));
			cConfId.GetConfIdString(achConfId, sizeof(achConfId));
			
			if(m_pcSnmpNodes)
			{
				m_pcSnmpNodes->AddNodeValue(NODE_MCUCONFSTATENOTIFY, &dwState, sizeof(u32) );
				m_pcSnmpNodes->AddNodeValue(NODE_MCUCONFIDNOTIFY, achConfId, sizeof(achConfId) );
				m_pcSnmpNodes->AddNodeValue(NODE_MCUCONFNAMENOTIFY, tConfState.m_abyConfName, sizeof(tConfState.m_abyConfName) );
				m_pcSnmpNodes->AddNodeValue(NODE_MCUCONFSTARTTIMENOTIFY, tConfState.m_abyTime, sizeof(tConfState.m_abyTime) );
				
				if(m_pcAgentSnmp)
				{
					m_pcAgentSnmp->AgentSendTrap( *m_pcSnmpNodes );
				}
			}
			ptMcuPfmInfo->DelConf(cConfId);
		}
		break;
    
	case SVC_AGT_ADD_MT:		
		{
			ptMtNotify = ( TMtNotify* )pcMsg->content;
			
			u8 byMtNtyconfIdLen = sizeof(ptMtNotify->m_abyConfId);
			u8 byMtConfStateConfIdLen = sizeof(tMtConfState.m_abyConfId);
			memcpy( tMtConfState.m_abyConfId, ptMtNotify->m_abyConfId, min(byMtNtyconfIdLen, byMtConfStateConfIdLen) );
			
            //[5/16/2013 liaokang] 转码
// 			strncpy( (s8*)tMtConfState.m_abyMtAlias, (s8*)ptMtNotify->m_abyMtAlias, sizeof(tMtConfState.m_abyMtAlias) );
// 			tMtConfState.m_abyMtAlias[ sizeof(tMtConfState.m_abyMtAlias) - 1 ] = '\0';
            s8 achEncodingBuf[MAXLEN_ALIAS+1] = {0};
            if( FALSE == TransEncodingForNmsData((s8*)ptMtNotify->m_abyMtAlias, achEncodingBuf, sizeof(achEncodingBuf)) )
            {
                strncpy(achEncodingBuf, (s8*)ptMtNotify->m_abyMtAlias, sizeof(achEncodingBuf) - 1);
            }
			strncpy( (s8*)tMtConfState.m_abyMtAlias, achEncodingBuf, sizeof(tMtConfState.m_abyMtAlias) - 1 );
			
			tCurTime = time( 0 );
			ptTime = localtime( &tCurTime );
			sprintf((s8*)tMtConfState.m_abyTime, "%4.4u-%2.2u-%2.2u %2.2u:%2.2u:%2.2u", ptTime->tm_year + 1900, 
				ptTime->tm_mon + 1, ptTime->tm_mday, ptTime->tm_hour, ptTime->tm_min, ptTime->tm_sec );
			
			tMtConfState.m_byState = CONF_ADD_TERMINAL;
			
			if (NULL != m_pcSnmpNodes)
			{
				m_pcSnmpNodes->Clear();
				m_pcSnmpNodes->SetTrapType(TRAP_MT_STATE);
			}
			
			
			dwState = tMtConfState.m_byState;
			cConfId.SetConfId(tMtConfState.m_abyConfId, sizeof(tMtConfState.m_abyConfId));
			cConfId.GetConfIdString(achConfId, sizeof(achConfId));
			
			if (NULL != m_pcSnmpNodes)
			{
				m_pcSnmpNodes->AddNodeValue(NODE_MCUCONFIDNOTIFY, achConfId, sizeof(achConfId) );
				m_pcSnmpNodes->AddNodeValue(NODE_MCUCONFSTARTTIMENOTIFY, tMtConfState.m_abyTime, sizeof(tMtConfState.m_abyTime) );
				m_pcSnmpNodes->AddNodeValue(NODE_MTSTATENOTIFY, &dwState, sizeof(u32) );
				m_pcSnmpNodes->AddNodeValue(NODE_MTALIASNOTIFY, tMtConfState.m_abyMtAlias, sizeof(tMtConfState.m_abyMtAlias) );
				
				if (NULL != m_pcAgentSnmp)
				{
					m_pcAgentSnmp->AgentSendTrap( *m_pcSnmpNodes );
				}
			}
			
			if (ptMcuPfmInfo->FindConf(cConfId, byIndex))
			{
				ptMcuPfmInfo->IncMtNum(byIndex);
			}
		}

		break;

	case SVC_AGT_DEL_MT:
		{
			ptMtNotify = ( TMtNotify* )pcMsg->content;
			
			u8 byMtNtyconfIdLen = sizeof(ptMtNotify->m_abyConfId);
			u8 byMtConfStateConfIdLen = sizeof(tMtConfState.m_abyConfId);
			memcpy( tMtConfState.m_abyConfId, ptMtNotify->m_abyConfId, min(byMtNtyconfIdLen, byMtConfStateConfIdLen) );
			
            //[5/16/2013 liaokang] 转码
            // 			strncpy( (s8*)tMtConfState.m_abyMtAlias, (s8*)ptMtNotify->m_abyMtAlias, sizeof(tMtConfState.m_abyMtAlias) );
            // 			tMtConfState.m_abyMtAlias[ sizeof(tMtConfState.m_abyMtAlias) - 1 ] = '\0';
            s8 achEncodingBuf[MAXLEN_ALIAS+1] = {0};
            if( FALSE == TransEncodingForNmsData((s8*)ptMtNotify->m_abyMtAlias, achEncodingBuf, sizeof(achEncodingBuf)) )
            {
                strncpy(achEncodingBuf, (s8*)ptMtNotify->m_abyMtAlias, sizeof(achEncodingBuf) - 1 );
            }
            strncpy( (s8*)tMtConfState.m_abyMtAlias, achEncodingBuf, sizeof(tMtConfState.m_abyMtAlias) - 1 );
			
			tCurTime = time( 0 );
			ptTime = localtime( &tCurTime );
			sprintf((s8*)tMtConfState.m_abyTime, "%4.4u-%2.2u-%2.2u %2.2u:%2.2u:%2.2u", ptTime->tm_year + 1900, 
				ptTime->tm_mon + 1, ptTime->tm_mday, ptTime->tm_hour, ptTime->tm_min, ptTime->tm_sec );
			
			tMtConfState.m_byState = CONF_DEL_TERMINAL;

			if (NULL != m_pcSnmpNodes)
			{
				m_pcSnmpNodes->Clear();
				m_pcSnmpNodes->SetTrapType(TRAP_MT_STATE);
			}
			
			dwState = tMtConfState.m_byState;
			cConfId.SetConfId(tMtConfState.m_abyConfId, sizeof(tMtConfState.m_abyConfId));
			cConfId.GetConfIdString(achConfId, sizeof(achConfId));
			
			if (NULL != m_pcSnmpNodes)
			{
				m_pcSnmpNodes->AddNodeValue(NODE_MCUCONFIDNOTIFY, achConfId, sizeof(achConfId) );
				m_pcSnmpNodes->AddNodeValue(NODE_MCUCONFSTARTTIMENOTIFY, tMtConfState.m_abyTime, sizeof(tMtConfState.m_abyTime) );
				m_pcSnmpNodes->AddNodeValue(NODE_MTSTATENOTIFY, &dwState, sizeof(u32) );
				m_pcSnmpNodes->AddNodeValue(NODE_MTALIASNOTIFY, tMtConfState.m_abyMtAlias, sizeof(tMtConfState.m_abyMtAlias) );

				if (NULL != m_pcAgentSnmp)
				{
					m_pcAgentSnmp->AgentSendTrap( *m_pcSnmpNodes );
				}
			}
			
			if (ptMcuPfmInfo->FindConf(cConfId, byIndex))
			{
				ptMcuPfmInfo->DecMtNum(byIndex);
			}
		}
		break;

    default:
        Agtlog(LOG_ERROR, "[ProcConfStatusChange] unexpected msg%d<%s> received!\n", 
                                          pcMsg->event, OspEventDesc(pcMsg->event));
        break;
	}

	return ;
}

/*=============================================================================
  函 数 名： ProcPowerStatusChange
  功    能： 处理上电
  算法实现： 
  全局变量： 
  参    数： CMessage * const pcMsg
  返 回 值： void 
=============================================================================*/
void CMsgCenterInst::ProcPowerStatusChange( CMessage * const pcMsg )
{
    if(NULL == pcMsg)
    {
        Agtlog(LOG_ERROR, "[ProcPowerStatusChange] The pointer1 can not be Null\n");
        return;
    }
    if ( !g_cCfgParse.HasSnmpNms() )
    {
        Agtlog(LOG_WARN, "[ProcPowerStatusChange] There's no NMS configed !\n");
        return;
    }

    u8   abyAlarmObj[MAX_ALARMOBJECT_NUM];
    memset(abyAlarmObj, 0, sizeof(abyAlarmObj));
    TMcupfmAlarmEntry    tAlarmData;
	TPowerStatus* ptPowerStatus = NULL;

	switch(pcMsg->event)
	{
	case SVC_AGT_POWER_STATUS:
		ptPowerStatus = (TPowerStatus*)pcMsg->content;
        abyAlarmObj[0] = ( u8 )ptPowerStatus->bySlot;
        abyAlarmObj[1] = ( u8 )ptPowerStatus->byType;
        abyAlarmObj[2] = 0;
        abyAlarmObj[3] = 0;
        abyAlarmObj[4] = 0;

        if( g_cAlarmProc.FindAlarm( ALARM_MCU_POWER_ABNORMAL, ALARMOBJ_MCU_POWER, abyAlarmObj, &tAlarmData ) )
        {
            if( 0 == ptPowerStatus->byStatus)      // 正常
            {
                if( !g_cAlarmProc.DeleteAlarm( tAlarmData.m_dwSerialNo ) )
                {
                    Agtlog(LOG_WARN, "[ProcPowerStatusChange] DeleteAlarm( AlarmCode = %lu, Object = %d, %d ) failed!\n", 
                                        tAlarmData.m_dwAlarmCode, 
                                        tAlarmData.m_achObject[0], 
                                        tAlarmData.m_achObject[1] );
                }
                else
                {
                    if (SNMP_SUCCESS != SendPrmAlarmTrap(m_pcSnmpNodes, tAlarmData, TRAP_ALARM_RESTORE))
                    {
                        Agtlog(LOG_ERROR, "[ProcPowerStatusChange] power status trap<%d> send failed !\n", TRAP_ALARM_RESTORE);
                    }
                    else
                    {
                        Agtlog(LOG_INFORM, "[ProcPowerStatusChange] power status trap<%d> send succeed !\n", TRAP_ALARM_RESTORE);
                    }
                }
            }
        }
        else        // 告警不存在
        {
            if( 0 != ptPowerStatus->byStatus )     // 异常
            {
                if( !g_cAlarmProc.AddAlarm( ALARM_MCU_POWER_ABNORMAL, ALARMOBJ_MCU_POWER, abyAlarmObj, &tAlarmData ) )
                {
                    Agtlog(LOG_WARN, "[ProcPowerStatusChange] McuAgent: AddAlarm( AlarmCode = %lu, Object = %d, %d ) failed!\n", 
                                        ALARM_MCU_POWER_ABNORMAL, abyAlarmObj[0], abyAlarmObj[1] );
                }
                else
                {   
                    if (SNMP_SUCCESS != SendPrmAlarmTrap(m_pcSnmpNodes, tAlarmData, TRAP_ALARM_GENERATE))
                    {
                        Agtlog(LOG_ERROR, "[ProcPowerStatusChange] power status trap<%d> send failed !\n", TRAP_ALARM_GENERATE);
                    }
                    else
                    {
                        Agtlog(LOG_INFORM, "[ProcPowerStatusChange] power status trap<%d> send succeed !\n", TRAP_ALARM_GENERATE);
                    }
                }
            }
        }
        break;

	default:
        Agtlog(LOG_ERROR, "[ProcPowerOffStatusChange] unexpected msg%d<%s> received!\n", pcMsg->event, OspEventDesc(pcMsg->event));
		break;
	}

	return ;
}

/*=============================================================================
  函 数 名： ProcModuleStatusChange
  功    能： 处理模块状态
  算法实现： 
  全局变量： 
  参    数： CMessage * const pcMsg
  返 回 值： void 
=============================================================================*/
void CMsgCenterInst::ProcModuleStatusChange( CMessage * const pcMsg )
{
    if(NULL == pcMsg)
    {
        Agtlog(LOG_ERROR, "[ProcModuleStatusChange] The pointer can not be Null\n");
        return;
    }
    if ( !g_cCfgParse.HasSnmpNms() )
    {
        Agtlog(LOG_WARN, "[ProcModuleStatusChange] There's no NMS configed !\n");
        return;
    }

    TModuleStatus *ptModuleStatus = NULL;
    u8   abyAlarmObj[MAX_ALARMOBJECT_NUM];
    TMcupfmAlarmEntry     tAlarmData;
	u16    wLoop = 0;

    u16   awModuleAlarmCode[] = { ALARM_MCU_MODULE_OFFLINE, 
                                   ALARM_MCU_MODULE_CONFLICT,
								   ALARM_MCU_MODULE_ERROR };
    u8   abyModuleAlarmBit[] = { 0x01, 0x02, 0x04 };
    memset(abyAlarmObj, 0, sizeof(abyAlarmObj));

	switch(pcMsg->event)
	{
	case SVC_AGT_MODULE_STATUS:
		ptModuleStatus = ( TModuleStatus * )pcMsg->content;
        abyAlarmObj[0] = ( u8 )ptModuleStatus->byLayer;
        abyAlarmObj[1] = ( u8 )ptModuleStatus->bySlot;
        abyAlarmObj[2] = ( u8 )ptModuleStatus->byType;
        abyAlarmObj[3] = ( u8 )ptModuleStatus->byModuleId;
        abyAlarmObj[4] = 0;

        for( wLoop = 0; wLoop < sizeof( awModuleAlarmCode ) / sizeof( awModuleAlarmCode[0] ); wLoop++ )
        {
            if( g_cAlarmProc.FindAlarm( awModuleAlarmCode[wLoop], ALARMOBJ_MCU_MODULE, abyAlarmObj, &tAlarmData ) )
            {
                if( 0 == ( ptModuleStatus->byStatus & abyModuleAlarmBit[wLoop] ))    //normal
                {
                    if( !g_cAlarmProc.DeleteAlarm( tAlarmData.m_dwSerialNo ) )
                    {
                            Agtlog(LOG_WARN, "[ProcModuleStatusChange] DeleteAlarm( AlarmCode = %lu, Object = %d, %d, %d, %d ) failed!\n", 
                                                tAlarmData.m_dwAlarmCode, tAlarmData.m_achObject[0], 
                                                tAlarmData.m_achObject[1], tAlarmData.m_achObject[2],
							                    tAlarmData.m_achObject[3]);
                    }
                    else
                    {
                        if (SNMP_SUCCESS != SendPrmAlarmTrap(m_pcSnmpNodes, tAlarmData, TRAP_ALARM_RESTORE))
                        {
                            Agtlog(LOG_ERROR, "[ProcModuleStatusChange] module status trap<%d> send failed !\n", TRAP_ALARM_RESTORE);
                        }
                        else
                        {
                            Agtlog(LOG_INFORM, "[ProcModuleStatusChange] module status trap<%d> send succeed !\n", TRAP_ALARM_RESTORE);
                        }
                    }
                }
            }
            else
            {
                if( 0 != ( ptModuleStatus->byStatus & abyModuleAlarmBit[wLoop] ) )    // abnormal
                {
                    if( !g_cAlarmProc.AddAlarm( awModuleAlarmCode[wLoop], ALARMOBJ_MCU_MODULE, abyAlarmObj, &tAlarmData ) )
                    {
                        Agtlog(LOG_ERROR, "[ProcModuleStatusChange] AddAlarm( AlarmCode = %lu, Object = %d, %d, %d, %d) failed!\n", 
                               awModuleAlarmCode[wLoop], abyAlarmObj[0], abyAlarmObj[1], abyAlarmObj[2], abyAlarmObj[3] );
                    }
                    else
                    {
                        if (SNMP_SUCCESS != SendPrmAlarmTrap(m_pcSnmpNodes, tAlarmData, TRAP_ALARM_GENERATE))
                        {
                            Agtlog(LOG_ERROR, "[ProcModuleStatusChange] module status trap<%d> send failed !\n", TRAP_ALARM_GENERATE);
                        }
                        else
                        {
                            Agtlog(LOG_INFORM, "[ProcModuleStatusChange] module status trap<%d> send succeed !\n", TRAP_ALARM_GENERATE);
                        }
                    }
                }
            }
        }
        break;
	default:
        Agtlog(LOG_ERROR, "[ProcModuleStatusChange] unexpected msg%d<%s> received!\n", pcMsg->event, OspEventDesc(pcMsg->event));
		break;
	}
	
	return ;
}

/*=============================================================================
  函 数 名： SetBoardLedState
  功    能： 设置Led状态
  算法实现： 
  全局变量： 
  参    数： u8 byLayer, 
             u8 bySlot, 
             u8 byType 
             s8 * pchLedState
  返 回 值： BOOL32 
=============================================================================*/
BOOL32 CMsgCenterInst::SetBoardLedState( u8 byLayer, u8 bySlot, u8 byType , s8* pszLedState )
{
    if( NULL == pszLedState)
    {
        Agtlog(LOG_ERROR, "[SetBoardLedState] The pointer can not be Null\n");
        return FALSE;
    }
    TBrdPosition tPos;
    tPos.byBrdLayer = byLayer;
    tPos.byBrdSlot  = bySlot;
    tPos.byBrdID    = byType;

	u8 byRow = g_cCfgParse.GetBoardRow(byLayer, bySlot, byType);
    if( ERR_AGENT_BRDNOTEXIST == byRow )
    {
        Agtlog(LOG_WARN, "[SetBoardLedState] Cannot Get Board Row by Layer.%d, Slot.%d !\n", 
			byLayer, bySlot);
        return FALSE;
    }

	TEqpBrdCfgEntry tBrdCfg;
    memset( &tBrdCfg, 0, sizeof(tBrdCfg) );

    u16 wRet = g_cCfgParse.GetBrdInfoByRow( byRow, &tBrdCfg );

    tBrdCfg.SetPanelLed( pszLedState );

    wRet |= g_cCfgParse.SetBrdInfoByRow( byRow, &tBrdCfg );

	if ( SUCCESS_AGENT != wRet )
    {
        Agtlog(LOG_ERROR, "[SetBoardLedState] operate brd.%d cfg failed !\n", tBrdCfg.GetBrdId());
        return FALSE;
    }

    if ( !g_cCfgParse.HasSnmpNms() )
    {
        Agtlog(LOG_WARN, "[SetBoardLedState] There's no NMS configed !\n");
        return TRUE;    //没有NMS，仍然应该对设置LED状态返回TRUE
    }

	u32 dwLayer = byLayer;
    u32 dwSlot = bySlot;
    u32 dwType = byType;
	if (!m_pcSnmpNodes || !m_pcAgentSnmp) 
	{
		Agtlog(LOG_WARN, "[SetBoardLedState] m_pcSnmpNodes or m_pcAgentSnmp is null!\n");
		return FALSE;
	}

    m_pcSnmpNodes->Clear();
    m_pcSnmpNodes->SetTrapType(TRAP_LED_STATE);
	
    u32 dwNodeName = MAKE_MCUTABLE_NODENAME(byRow, NODE_MCUNETBRDCFGLAYER);
    m_pcSnmpNodes->AddNodeValue(dwNodeName, &dwLayer, sizeof(dwLayer) );

    dwNodeName = MAKE_MCUTABLE_NODENAME(byRow, NODE_MCUNETBRDCFGSLOT);
    m_pcSnmpNodes->AddNodeValue(dwNodeName, &dwSlot, sizeof(dwSlot) );

    dwNodeName = MAKE_MCUTABLE_NODENAME(byRow, NODE_MCUNETBRDCFGTYPE);
    m_pcSnmpNodes->AddNodeValue(dwNodeName, &dwType, sizeof(dwType) );

    dwNodeName = MAKE_MCUTABLE_NODENAME(byRow, NODE_MCUNETBRDCFGPANELLED);
    m_pcSnmpNodes->AddNodeValue(dwNodeName, pszLedState, strlen(pszLedState) );

    u32 dwState = tBrdCfg.GetState();
    dwNodeName = MAKE_MCUTABLE_NODENAME(byRow, NODE_MCUNETBRDCFGSTATUS);
    m_pcSnmpNodes->AddNodeValue(dwNodeName, &dwState, sizeof(dwState));

    m_pcAgentSnmp->AgentSendTrap( *m_pcSnmpNodes );
    Agtlog(LOG_INFORM,  "Trap send succeed, type: %d, bMasterMcu: %d !\n", 
                         TRAP_LED_STATE, g_cCfgParse.IsMpcActive() );
    return TRUE;	
}

/*=============================================================================
  函 数 名： ProcLedStatusChange
  功    能： 处理Led状态
  算法实现： 
  全局变量： 
  参    数： CMessage * const pcMsg
  返 回 值： void 
=============================================================================*/
void CMsgCenterInst::ProcLedStatusChange( CMessage * const pcMsg )
{
//#ifndef WIN32
    TBrdLedState tBrdLedState = {0};
    
    memcpy(&tBrdLedState, pcMsg->content, pcMsg->length );
    
    TBrdPosition tPos;
    memset( &tPos, 0, sizeof(tPos) );
    
    TMPCInfo tMPCInfo;
    g_cCfgParse.GetMPCInfo( &tMPCInfo );
    tPos.byBrdLayer = tMPCInfo.GetLocalLayer();
    tPos.byBrdSlot  = tMPCInfo.GetLocalSlot();

#ifdef _MINIMCU_
	tPos.byBrdLayer = 0;
	tPos.byBrdSlot  = 0;
#endif


#ifdef _LINUX12_
	//  [1/21/2011 chendaiwei]支持MPC2
	tPos.byBrdID = BRD_TYPE_MPC2/*DSL8000_BRD_MPC2*/;
    SetBoardLedState( tPos.byBrdLayer, tPos.byBrdSlot, tPos.byBrdID,
                        (s8*)&tBrdLedState.nlunion.tBrdMPC2LedState);
#else
	tPos.byBrdID = BRD_TYPE_MPC/*DSL8000_BRD_MPC*/;
	SetBoardLedState( tPos.byBrdLayer, tPos.byBrdSlot, tPos.byBrdID,
                        (s8*)&tBrdLedState.nlunion.tBrdMPCLedState);
#endif
//#endif
    
    return;
}

/*=============================================================================
  函 数 名： ProcSDHStatusChange
  功    能： 处理SDH状态
  算法实现： 
  全局变量： 
  参    数： CMessage * const pcMsg
  返 回 值： void 
=============================================================================*/
void CMsgCenterInst::ProcSDHStatusChange( CMessage * const pcMsg )
{
    if(NULL == pcMsg)
    {
        Agtlog(LOG_ERROR, "[ProcSDHStatusChange] The pointer can not be Null\n");
        return;
    }
    if ( !g_cCfgParse.HasSnmpNms() )
    {
        Agtlog(LOG_WARN, "[ProcSDHStatusChange] There's no NMS configed !\n");
        return;
    }
    
    TSDHStatus *ptSDHStatus = NULL;
    u8   abyAlarmObj[MAX_ALARMOBJECT_NUM];
    TMcupfmAlarmEntry     tAlarmData;
	u16    wLoop;

	// SDH的告警码
    u16   awSDHAlarmCode[] = { ALARM_MCU_SDH_LOS,      ALARM_MCU_SDH_LOF,
							    ALARM_MCU_SDH_OOF,      ALARM_MCU_SDH_LOM,
							    ALARM_MCU_SDH_LOP,      ALARM_MCU_SDH_MS_RDI,
							    ALARM_MCU_SDH_MS_AIS,   ALARM_MCU_SDH_HP_RDI,
							    ALARM_MCU_SDH_HP_AIS,   ALARM_MCU_SDH_RS_TIM,
							    ALARM_MCU_SDH_HP_TIM,   ALARM_MCU_SDH_HP_UNEQ,
							    ALARM_MCU_SDH_HP_PSLM,  ALARM_MCU_SDH_TU_LOP,
							    ALARM_MCU_SDH_LP_RDI,   ALARM_MCU_SDH_LP_AIS,
							    ALARM_MCU_SDH_LP_TIM,   ALARM_MCU_SDH_LP_PSLM };
	// 各个SDH告警码对应的BIT位
    u32   adwSDHAlarmBit[] = { 0x00000001,   0x00000002,
							    0x00000004,   0x00000008,
							    0x00000010,   0x00000020,
							    0x00000040,   0x00000080,
							    0x00000100,   0x00000200,
							    0x00000400,   0x00000800,
							    0x00001000,   0x00002000,
							    0x00004000,   0x00008000,
							    0x00010000,   0x00020000 };

	memset( abyAlarmObj, 0, sizeof(abyAlarmObj) );
	switch(pcMsg->event)
	{
	case SVC_AGT_SDH_STATUS:
		ptSDHStatus = ( TSDHStatus * )pcMsg->content;
		*(u32*)abyAlarmObj = ptSDHStatus->dwStatus;  // 取SDH状态
        for( wLoop = 0; wLoop < sizeof( awSDHAlarmCode ) / sizeof( awSDHAlarmCode[0] ); wLoop++ )
        {
            if( g_cAlarmProc.FindAlarm( awSDHAlarmCode[wLoop], ALARMOBJ_MCU_SDH, abyAlarmObj, &tAlarmData ) )
            {
                if( 0 ==  ( ptSDHStatus->dwStatus & adwSDHAlarmBit[wLoop] ))    // normal
                {
                    if( !g_cAlarmProc.DeleteAlarm( tAlarmData.m_dwSerialNo ) )
                    {
                        Agtlog(LOG_ERROR, "[ProcSDHStatusChange] DeleteAlarm( AlarmCode = %lu, Object = %d, %d, %d ) failed!\n", 
                                            tAlarmData.m_dwAlarmCode, 
                                            tAlarmData.m_achObject[0], 
                                            tAlarmData.m_achObject[1], 
                                            tAlarmData.m_achObject[2]);
                    }
                    else
                    {
                        if (SNMP_SUCCESS !=SendPrmAlarmTrap(m_pcSnmpNodes, tAlarmData, TRAP_ALARM_RESTORE))
                        {
                            Agtlog(LOG_ERROR, "[ProcSDHStatusChange] SDH status trap<%d> send failed !\n", TRAP_ALARM_RESTORE);
                        }
                        else
                        {
                            Agtlog(LOG_INFORM, "[ProcSDHStatusChange] SDH status trap<%d> send succeed !\n", TRAP_ALARM_RESTORE);
                        }
                    }
                }
            }
            else        // no such alarm
            {
                if( 0 != ( ptSDHStatus->dwStatus & adwSDHAlarmBit[wLoop] ) )    // abnormal
                {
                    if( !g_cAlarmProc.AddAlarm( awSDHAlarmCode[wLoop], ALARMOBJ_MCU_SDH, abyAlarmObj, &tAlarmData ) )
                    {
                        Agtlog(LOG_ERROR, "[ProcSDHStatusChange] AddAlarm( AlarmCode = %lu, Object = %d, %d ) failed!\n", 
                                            awSDHAlarmCode[wLoop], abyAlarmObj[0], abyAlarmObj[1], abyAlarmObj[2] );
                    }
                    else
                    {
                        if (SNMP_SUCCESS !=SendPrmAlarmTrap(m_pcSnmpNodes, tAlarmData, TRAP_ALARM_GENERATE))
                        {
                            Agtlog(LOG_ERROR, "[ProcSDHStatusChange] SDH status trap<%d> send failed !\n", TRAP_ALARM_GENERATE);
                        }
                        else
                        {
                            Agtlog(LOG_INFORM, "[ProcSDHStatusChange] SDH status trap<%d> send succeed !\n", TRAP_ALARM_GENERATE);
                        }
                    }
                }
            }
        }
        break;

	default:
        Agtlog(LOG_ERROR, "[ProcSDHStatusChange] unexpected msg%d<%s> received!\n", pcMsg->event, OspEventDesc(pcMsg->event));
		break;
	}
	
	return ;
}

/*=============================================================================
  函 数 名： ProcBoardStatusChange
  功    能： 处理单板状态
  算法实现： 
  全局变量： 
  参    数： CMessage * const pcMsg
  返 回 值： void 
=============================================================================*/
void CMsgCenterInst::ProcBoardStatusChange( CMessage * const pcMsg )
{
    if(NULL == pcMsg)
    {
        Agtlog(LOG_ERROR, "[ProcBoardStatusChange] The pointer can not be Null\n");
        return;
    }   

    TBrdStatus  tBrdStatus;
    TBrdStatus* ptBrdStatus = NULL;
	u8		abyAlarmObj[MAX_ALARMOBJECT_NUM];  
    TMcupfmAlarmEntry     tAlarmData;
	u32   dwLoop;
    u16   awBoardAlarmCode[] = { ALARM_MCU_BOARD_DISCONNECT, ALARM_MCU_BOARD_CFGCONFLICT};
    u8    abyBoardAlarmBit[] = { 0x01, 0x02 };

    memset(abyAlarmObj, 0, sizeof(abyAlarmObj));

    if (pcMsg->event == SVC_AGT_STANDBYMPCSTATUS_NOTIFY)
    {
        TMPCInfo tMPCInfo;
        g_cCfgParse.GetMPCInfo( &tMPCInfo );
        
        tBrdStatus.byLayer = tMPCInfo.GetOtherMpcLayer();
        tBrdStatus.bySlot  = tMPCInfo.GetOtherMpcSlot();

		//  [1/21/2011 chendaiwei]支持MPC2
#ifdef _LINUX12_
        tBrdStatus.byType = BRD_TYPE_MPC2/*DSL8000_BRD_MPC2*/;
#else
        tBrdStatus.byType = BRD_TYPE_MPC/*DSL8000_BRD_MPC*/;
#endif

        tBrdStatus.byStatus = g_cCfgParse.BrdStatusMcuVc2Agt( *pcMsg->content );
        tBrdStatus.byOsType = tMPCInfo.GetOSType();
        
        tMPCInfo.SetOtherMpcStatus( tBrdStatus.byStatus );
        g_cCfgParse.SetMPCInfo( tMPCInfo );
        Agtlog(LOG_INFORM, "[ProcBoardStatusChange] Other MPC Status Changed to %d!\n", 
                                                                 tBrdStatus.byStatus );

        ptBrdStatus = &tBrdStatus;
    }
    else
    {
        ptBrdStatus = ( TBrdStatus * )pcMsg->content;
    }
    
    TBrdPosition tBrdPos;
    tBrdPos.byBrdLayer = ptBrdStatus->byLayer;
    tBrdPos.byBrdSlot  = ptBrdStatus->bySlot;
    tBrdPos.byBrdID    = ptBrdStatus->byType;

    TBoardInfo tBrdInfo;
    g_cCfgParse.GetBrdCfgById( g_cCfgParse.GetBrdId(tBrdPos), &tBrdInfo );

	switch(pcMsg->event)
	{
    // guzh [9/15/2006] 增加从MS业务通知MCU代理
    case SVC_AGT_STANDBYMPCSTATUS_NOTIFY:
	case MCU_AGT_BOARD_STATUSCHANGE:        
        abyAlarmObj[0] = ( u8 )ptBrdStatus->byLayer;
        abyAlarmObj[1] = ( u8 )ptBrdStatus->bySlot;
        abyAlarmObj[2] = ( u8 )ptBrdStatus->byType;
        abyAlarmObj[3] = ( u8 )ptBrdStatus->byStatus;
        abyAlarmObj[4] = 0;
        
        if (ptBrdStatus->byOsType == OS_TYPE_WIN32 || 
            ptBrdStatus->byOsType == OS_TYPE_LINUX || 
            ptBrdStatus->byOsType == OS_TYPE_VXWORKS)
        {
            tBrdInfo.SetOSType( ptBrdStatus->byOsType );			
            g_cCfgParse.SetBrdCfgById( tBrdInfo.GetBrdId(), &tBrdInfo );
        }
        else
        {
            // guzh [9/18/2006] 如果没有设置,则读取原来的
            ptBrdStatus->byOsType = tBrdInfo.GetOSType();			
        }

		//  [1/21/2011 chendaiwei]支持MPC2
        if( g_cCfgParse.IsMpcActive() || 
			( ptBrdStatus->byType == BRD_TYPE_MPC/*DSL8000_BRD_MPC*/ || ptBrdStatus->byType == BRD_TYPE_MPC2 ) )
        {
            SendMsgToMcuCfg(AGT_SVC_BOARDSTATUS_NOTIFY, (u8*)ptBrdStatus, sizeof(TBrdStatus));
        }        

        if ( !g_cCfgParse.HasSnmpNms() )
        {
            Agtlog(LOG_WARN, "[ProcBoardStatusChange] There's no NMS configed !\n");
            return;
        }

        // 如果配置了网管
		// 是否生成告警或告警恢复		
        for( dwLoop = 0; dwLoop < sizeof( awBoardAlarmCode ) / sizeof( awBoardAlarmCode[0] ); dwLoop++ )
        {
            if( g_cAlarmProc.FindAlarm( awBoardAlarmCode[dwLoop], ALARMOBJ_MCU_BOARD, abyAlarmObj, &tAlarmData ) )
            {
                if( 0 == ( ptBrdStatus->byStatus & abyBoardAlarmBit[dwLoop] ))    // normal
                {
                    if( !g_cAlarmProc.DeleteAlarm( tAlarmData.m_dwSerialNo ) )
                    {
                        Agtlog(LOG_ERROR, "[ProcBoardStatusChange] DeleteAlarm( AlarmCode = %lu, Object = %d, %d, %d ) failed!\n", 
                                            tAlarmData.m_dwAlarmCode, tAlarmData.m_achObject[0], 
                                            tAlarmData.m_achObject[1], tAlarmData.m_achObject[2]);
                    }
                    else
                    {
                        if (SNMP_SUCCESS !=SendPrmAlarmTrap(m_pcSnmpNodes, tAlarmData, TRAP_ALARM_RESTORE))
                        {
                            Agtlog(LOG_ERROR, "[ProcBoardStatusChange] Board<%d,%d:%s> status<%d> trap<%d> send failed !\n",
                                               ptBrdStatus->byLayer, ptBrdStatus->bySlot, 
                                               g_cCfgParse.GetBrdTypeStr(ptBrdStatus->byType), 
                                               ptBrdStatus->byStatus, TRAP_ALARM_RESTORE);
                        }
                        else
                        {
                            Agtlog(LOG_INFORM, "[ProcBoardStatusChange] Board<%d,%d:%s> status<%d> trap<%d> send succeed !\n",
                                                ptBrdStatus->byLayer, ptBrdStatus->bySlot, 
                                                g_cCfgParse.GetBrdTypeStr(ptBrdStatus->byType), 
                                                ptBrdStatus->byStatus, TRAP_ALARM_RESTORE);
                        }
                    }
                }
            }
            else        //no such alarm
            {
                if( 0 != ( ptBrdStatus->byStatus & abyBoardAlarmBit[dwLoop] ))    //abnormal
                {
                    if( !g_cAlarmProc.AddAlarm( awBoardAlarmCode[dwLoop], ALARMOBJ_MCU_BOARD, abyAlarmObj, &tAlarmData ) )
                    {
                        Agtlog(LOG_ERROR, "[ProcBoardStatusChange] AddAlarm( AlarmCode = %lu, Object = %d, %d ) failed!\n", 
                            awBoardAlarmCode[dwLoop], abyAlarmObj[0], abyAlarmObj[1], abyAlarmObj[2] );
                    }
                    else
                    {
                        if (SNMP_SUCCESS !=SendPrmAlarmTrap(m_pcSnmpNodes, tAlarmData, TRAP_ALARM_GENERATE))
                        {
                            Agtlog(LOG_ERROR, "[ProcBoardStatusChange] Board<%d,%d:%s> status<%d> trap<%d> send failed !\n",
                                                ptBrdStatus->byLayer, ptBrdStatus->bySlot, 
                                                g_cCfgParse.GetBrdTypeStr(ptBrdStatus->byType),
                                                ptBrdStatus->byStatus, TRAP_ALARM_GENERATE);
                        }
                        else
                        {
                            Agtlog(LOG_INFORM, "[ProcBoardStatusChange] Board<%d,%d:%s> status<%d> trap<%d> send succeed !\n",
                                                ptBrdStatus->byLayer, ptBrdStatus->bySlot, 
                                                g_cCfgParse.GetBrdTypeStr(ptBrdStatus->byType), 
                                                ptBrdStatus->byStatus, TRAP_ALARM_GENERATE);
                        }
                    }
                }
            }
        }
        break;

	default:
        Agtlog(LOG_ERROR, "[ProcBoardStatusChange] unexpected msg%d<%s> received!\n", pcMsg->event, OspEventDesc(pcMsg->event));
		break;
	}

	return ;
}

/*=============================================================================
  函 数 名： ProcLinkStatusChange
  功    能： 处理链路状态
  算法实现： 
  全局变量： 
  参    数： CMessage * const pcMsg
  返 回 值： void 
=============================================================================*/
void CMsgCenterInst::ProcLinkStatusChange( CMessage * const pcMsg )
{
    if(NULL == pcMsg)
    {
        Agtlog(LOG_ERROR, "[ProcLinkStatusChange] The pointer can not be Null\n");
        return;
    }
    if ( !g_cCfgParse.HasSnmpNms() )
    {
        Agtlog(LOG_WARN, "[ProcLinkStatusChange] There's no NMS configed !\n");
        return;
    }

    TLinkStatus *ptLinkStatus = NULL;
    u8   abyAlarmObj[MAX_ALARMOBJECT_NUM];
    TMcupfmAlarmEntry     tAlarmData;
	u16    wLoop;
    u16    awE1AlarmCode[] = { ALARM_MCU_BRD_E1_RCMF, ALARM_MCU_BRD_E1_RMF,
							   ALARM_MCU_BRD_E1_RUA1, ALARM_MCU_BRD_E1_RRA, 
							   ALARM_MCU_BRD_E1_LOC,  ALARM_MCU_BRD_E1_RLOS };
    u8   abyE1AlarmBit[] = { 0x20, 0x10, 0x08, 0x04, 0x02, 0x01 };
    BOOL32 bAlarmResult = FALSE;
    memset(abyAlarmObj, 0, sizeof(abyAlarmObj));

	switch(pcMsg->event)
	{
	case MCU_AGT_LINK_STATUSCHANGE:
		ptLinkStatus = ( TLinkStatus * )pcMsg->content;
        abyAlarmObj[0] = ( u8 )ptLinkStatus->byLayer;
        abyAlarmObj[1] = ( u8 )ptLinkStatus->bySlot;
        abyAlarmObj[2] = ( u8 )ptLinkStatus->byType;
        abyAlarmObj[3] = ( u8 )ptLinkStatus->byE1Num;
        abyAlarmObj[4] = ( u8 )ptLinkStatus->byStatus;

        for( wLoop = 0; wLoop < sizeof( awE1AlarmCode ) / sizeof( awE1AlarmCode[0] ); wLoop++ )
        {
            bAlarmResult = g_cAlarmProc.FindAlarm( awE1AlarmCode[wLoop], ALARMOBJ_MCU_LINK, abyAlarmObj, &tAlarmData );
            if( bAlarmResult )
            {
                if( 0 == ( ptLinkStatus->byStatus & abyE1AlarmBit[wLoop] ) )    //normal
                {
                    bAlarmResult = g_cAlarmProc.DeleteAlarm( tAlarmData.m_dwSerialNo );
                    if(  !bAlarmResult )
                    {
                        Agtlog(LOG_ERROR, "[ProcLinkStatusChange] DeleteAlarm( AlarmCode = %lu, Object = %d, %d, %d ) failed!\n", 
                                            tAlarmData.m_dwAlarmCode, tAlarmData.m_achObject[0], 
                                            tAlarmData.m_achObject[1], tAlarmData.m_achObject[2]);
                    }
                    else
                    {
                        if (SNMP_SUCCESS !=SendPrmAlarmTrap(m_pcSnmpNodes, tAlarmData, TRAP_ALARM_RESTORE))
                        {
                            Agtlog(LOG_ERROR, "[ProcLinkStatusChange] Link status<%d> trap<%d> send failed !\n",
                                               ptLinkStatus->byStatus, TRAP_ALARM_RESTORE);
                        }
                        else
                        {
                            Agtlog(LOG_INFORM, "[ProcLinkStatusChange] Link status<%d> trap<%d> send succeed !\n",
                                                ptLinkStatus->byStatus, TRAP_ALARM_RESTORE);
                        }
                    }
                }
            }
            else        // no such alarm
            {
                if( 0 != ( ptLinkStatus->byStatus & abyE1AlarmBit[wLoop] ))    // abnormal
                {
                    bAlarmResult = g_cAlarmProc.AddAlarm( awE1AlarmCode[wLoop], ALARMOBJ_MCU_LINK, abyAlarmObj, &tAlarmData );
                    if(  !bAlarmResult )
                    {
                        Agtlog(LOG_ERROR, "[ProcLinkStatusChange] AddAlarm( AlarmCode = %lu, Object = %d, %d ) failed!\n", 
                            awE1AlarmCode[wLoop], abyAlarmObj[0], abyAlarmObj[1], abyAlarmObj[2] );
                    }
                    else
                    {
                        if (SNMP_SUCCESS !=SendPrmAlarmTrap(m_pcSnmpNodes, tAlarmData, TRAP_ALARM_GENERATE))
                        {
                            Agtlog(LOG_ERROR, "[ProcLinkStatusChange] Link status<%d> trap<%d> send failed !\n",
                                               ptLinkStatus->byStatus, TRAP_ALARM_GENERATE);
                        }
                        else
                        {
                            Agtlog(LOG_INFORM, "[ProcLinkStatusChange] Link status<%d> trap<%d> send succeed !\n",
                                                ptLinkStatus->byStatus, TRAP_ALARM_GENERATE);
                        }
                    }
                }
            }
        }
        break;

	default:
        Agtlog(LOG_ERROR, "[ProcLinkStatusChange] unexpected msg%d<%s> received!\n", pcMsg->event, OspEventDesc(pcMsg->event));
		break;
	}
	
	return;
}

/*=============================================================================
  函 数 名： ProcNetSyncStatusChange
  功    能： 处理网同步状态
  算法实现： 
  全局变量： 
  参    数： CMessage * const pcMsg
  返 回 值： void 
=============================================================================*/
void CMsgCenterInst::ProcNetSyncStatusChange( CMessage * const pcMsg )
{
    if(NULL == pcMsg)
    {
        Agtlog(LOG_ERROR, "[ProcNetSyncStatusChange] The pointer can not be Null\n");
        return;
    }
    if ( !g_cCfgParse.HasSnmpNms() )
    {
        Agtlog(LOG_WARN, "[ProcNetSyncStatusChange] There's no NMS configed !\n");
        return;
    }
    
    u8   abyAlarmObj[MAX_ALARMOBJECT_NUM];
    TMcupfmAlarmEntry     tAlarmData;
    u8   bySyncSrcAlarm = 0;
    BOOL32    bAlarmResult = FALSE;
	memset( abyAlarmObj, 0, sizeof(abyAlarmObj) );
    u8   bySyncStatus = 0; // 默认为正常

	switch(pcMsg->event)
	{
	case MCU_AGT_SYNCSOURCE_STATUSCHANGE:
        bySyncStatus = *pcMsg->content;
        abyAlarmObj[0] = bySyncStatus;
        
        // 主用同步源异常 ALARM_SYNCSRC_ABNORMAL
        bAlarmResult = g_cAlarmProc.FindAlarm( ALARM_MCU_SYNCSRC_ABNORMAL, ALARMOBJ_MCU, abyAlarmObj, &tAlarmData );
        if( bAlarmResult )
        {
            if( 0 == bySyncStatus )  // normal
            {
                bAlarmResult = g_cAlarmProc.DeleteAlarm( tAlarmData.m_dwSerialNo );
                if(  !bAlarmResult )
                {
                    Agtlog(LOG_ERROR, "[ProcNetSyncStatusChange] DeleteAlarm( AlarmCode = %lu, Object = %d, %d ) failed!\n", 
                                       tAlarmData.m_dwAlarmCode, 0, 0 );
                }
                else
                {
                    if (SNMP_SUCCESS !=SendPrmAlarmTrap(m_pcSnmpNodes, tAlarmData, TRAP_ALARM_RESTORE))
                    {
                        Agtlog(LOG_ERROR, "[ProcNetSyncStatusChange] net sync status<%d> trap<%d> send failed !\n",
                                           bySyncStatus, TRAP_ALARM_RESTORE);
                    }
                    else
                    {
                        Agtlog(LOG_INFORM, "[ProcNetSyncStatusChange] net sync status<%d> trap<%d> send succeed !\n",
                                            bySyncStatus, TRAP_ALARM_RESTORE);
                    }
                }
            }
            else
            {
                bySyncSrcAlarm++;
            }
        }
        else
        {
            if( 1 == bySyncStatus )  // abnormal
            {
                bAlarmResult = g_cAlarmProc.AddAlarm( ALARM_MCU_SYNCSRC_ABNORMAL, ALARMOBJ_MCU, abyAlarmObj, &tAlarmData );
                if(  !bAlarmResult )
                {
                    Agtlog(LOG_ERROR, "[ProcNetSyncStatusChange] AddAlarm( AlarmCode = %lu, Object = %d, %d ) failed!\n", 
                                        ALARM_MCU_SYNCSRC_ABNORMAL, 0, 0 );
                }
                else
                {
                    if (SNMP_SUCCESS !=SendPrmAlarmTrap(m_pcSnmpNodes, tAlarmData, TRAP_ALARM_GENERATE))
                    {
                        Agtlog(LOG_ERROR, "[ProcNetSyncStatusChange] net sync status<%d> trap<%d> send failed !\n",
                                           bySyncStatus, TRAP_ALARM_GENERATE);
                    }
                    else
                    {
                        Agtlog(LOG_INFORM, "[ProcNetSyncStatusChange] net sync status<%d> trap<%d> send failed !\n",
                                            bySyncStatus, TRAP_ALARM_GENERATE);
                    }
                }   
                
                bySyncSrcAlarm++;
            }
        }
/*
        // 备用用同步源异常 ALARM_SYNCSRC_SPARE_ABNORMAL
        bAlarmResult = g_cAlarmProc.FindAlarm( ALARM_MCU_SYNCSRC_SPARE_ABNORMAL, ALARMOBJ_MCU, abyAlarmObj, &tAlarmData );
        if( bAlarmResult )
        {
            if( 0 == ( ( CSyncSourceAlarm * )pcMsg->content )->slavestatus )     //normal
            {
                bAlarmResult = g_cAlarmProc.DeleteAlarm( tAlarmData.m_dwSerialNo );
                if(  !bAlarmResult )
                {
                    OspPrintf(TRUE, FALSE,  "McuAgent: DeleteAlarm( AlarmCode = %lu, Object = %d, %d ) failed!\n", 
                        tAlarmData.m_dwAlarmCode, 0, 0 );
                }
                else
                {
                    SendPrmAlarmTrap(m_pcSnmpNodes, tAlarmData, TRAP_ALARM_RESTORE);
                }
            }
            else
            {
                bySyncSrcAlarm++;
            }
        }
        else        // no such alarm
        {
            if( 1 == ( ( CSyncSourceAlarm * )pcMsg->content )->slavestatus)     // abnormal
            {
                bAlarmResult = g_cAlarmProc.AddAlarm( ALARM_MCU_SYNCSRC_SPARE_ABNORMAL, ALARMOBJ_MCU, abyAlarmObj, &tAlarmData );
                if(  !bAlarmResult )
                {
                    OspPrintf(TRUE, FALSE,  "McuAgent: AddAlarm( AlarmCode = %lu, Object = %d, %d ) failed!\n", 
                        ALARM_MCU_SYNCSRC_SPARE_ABNORMAL, 0, 0 );
                }
                else
                {
                    SendPrmAlarmTrap(m_pcSnmpNodes, tAlarmData, TRAP_ALARM_GENERATE);
                }
                bySyncSrcAlarm++;
            }
        }

      
        // 主备用同步源同时异常 ALARM_SYNCSRC_BOTH_ABNORMAL
        bAlarmResult = g_cAlarmProc.FindAlarm( ALARM_MCU_SYNCSRC_BOTH_ABNORMAL, ALARMOBJ_MCU, abyAlarmObj, &tAlarmData );
        if( bAlarmResult )
        {
            if( bySyncSrcAlarm < 2 )        // normal
            {
                bAlarmResult = g_cAlarmProc.DeleteAlarm( tAlarmData.m_dwSerialNo );
                if(  !bAlarmResult )
                {
                    OspPrintf(TRUE, FALSE,  "McuAgent: DeleteAlarm( AlarmCode = %lu, Object = %d, %d ) failed!\n", 
                        tAlarmData.m_dwAlarmCode, 0, 0 );
                }
                else
                {
                    SendPrmAlarmTrap(m_pcSnmpNodes, tAlarmData, TRAP_ALARM_RESTORE);
                }
            }
        }
        else        // no such alarm
        {
            if( 2 == bySyncSrcAlarm)     // abnormal(两个都异常)
            {
                bAlarmResult = g_cAlarmProc.AddAlarm( ALARM_MCU_SYNCSRC_BOTH_ABNORMAL, ALARMOBJ_MCU, abyAlarmObj, &tAlarmData );
                if(  !bAlarmResult )
                {
                    OspPrintf(TRUE, FALSE,  "McuAgent: AddAlarm( AlarmCode = %lu, Object = %d, %d ) failed!\n", 
                        ALARM_MCU_SYNCSRC_BOTH_ABNORMAL, 0, 0 );
                }
                else
                {
                    SendPrmAlarmTrap(m_pcSnmpNodes, tAlarmData, TRAP_ALARM_GENERATE);
                }
            }
        }
*/
        break;

	default:
        Agtlog(LOG_ERROR, "[ProcNetSyncStatusChange] unexpected msg%d<%s> received!\n", pcMsg->event, OspEventDesc(pcMsg->event));
		break;
	}
	
	return ;
}


/*=============================================================================
  函 数 名： ProcBoardTempStatusChange
  功    能： 处理单板温度状态改变
  算法实现： 
  全局变量： 
  参    数： CMessage * const pcMsg
  返 回 值： void 
  ----------------------------------------------------------------------
  修改记录    ：
  日  期		版本		修改人		走读人    修改内容
  2011/10/27    4.0			陈代伟                  创建
=============================================================================*/
void CMsgCenterInst::ProcBoardTempStatusChange( CMessage * const pcMsg )
{
    if(NULL == pcMsg)
    {
        Agtlog(LOG_ERROR, "[ProcBoardTempStatusChange] The pointer can not be Null\n");
        return;
    }
    if ( !g_cCfgParse.HasSnmpNms() )
    {
        Agtlog(LOG_WARN, "[ProcBoardTempStatusChange] There's no NMS configed !\n");
        return;
    }

    TBrdStatus *ptBrdStatus = NULL;
    u8   abyAlarmObj[MAX_ALARMOBJECT_NUM];
    TMcupfmAlarmEntry     tAlarmData;
	u16    wLoop = 0;
    BOOL32 bAlarmResult = FALSE;
    memset(abyAlarmObj, 0, sizeof(abyAlarmObj));

	switch(pcMsg->event)
	{
	case SVC_AGT_BRD_TEMPERATURE_STATUS_NOTIFY:
		ptBrdStatus = ( TBrdStatus * )pcMsg->content;
        abyAlarmObj[0] = ( u8 )ptBrdStatus->byLayer;
        abyAlarmObj[1] = ( u8 )ptBrdStatus->bySlot;
        abyAlarmObj[2] = ( u8 )ptBrdStatus->byType;
        abyAlarmObj[3] = ( u8 )ptBrdStatus->byStatus;
        abyAlarmObj[4] = 0;

        bAlarmResult = g_cAlarmProc.FindAlarm( ALARM_MCU_BRD_TEMP_ABNORMAL, ALARMOBJ_MCU_BOARD, abyAlarmObj, &tAlarmData );
        if( bAlarmResult )
        {
            if( BRD_STATUS_NORMAL == ptBrdStatus->byStatus)    //normal
            {
                bAlarmResult = g_cAlarmProc.DeleteAlarm( tAlarmData.m_dwSerialNo );
                if(  !bAlarmResult )
                {
                    Agtlog(LOG_ERROR, "[ProcBoardTempStatusChange] DeleteAlarm( AlarmCode = %lu, Object = %d, %d, %d ) failed!\n", 
                                        tAlarmData.m_dwAlarmCode, tAlarmData.m_achObject[0], 
                                        tAlarmData.m_achObject[1], tAlarmData.m_achObject[2]);
                }
                else
                {
                    if (SNMP_SUCCESS !=SendPrmAlarmTrap(m_pcSnmpNodes, tAlarmData, TRAP_ALARM_RESTORE))
                    {
                        Agtlog(LOG_ERROR, "[ProcBoardTempStatusChange] temperature status<%d> trap<%d> send failed !\n",
                                           ptBrdStatus->byStatus, TRAP_ALARM_RESTORE);
                    }
                    else
                    {
                        Agtlog(LOG_INFORM, "[ProcBoardTempStatusChange] temperature status<%d> trap<%d> send succeed !\n",
                                            ptBrdStatus->byStatus, TRAP_ALARM_RESTORE);
                    }
                }
            }
        }
        else        // no such alarm
        {
            if( BRD_STATUS_ABNORMAL == ptBrdStatus->byStatus)    // abnormal
            {
                bAlarmResult = g_cAlarmProc.AddAlarm( ALARM_MCU_BRD_TEMP_ABNORMAL, ALARMOBJ_MCU_BOARD, abyAlarmObj, &tAlarmData );
                if(  !bAlarmResult )
                {
                    Agtlog(LOG_ERROR, "[ProcBoardTempStatusChange] AddAlarm( AlarmCode = %lu, Object = %d, %d ) failed!\n", 
                        ALARM_MCU_BRD_TEMP_ABNORMAL, abyAlarmObj[0], abyAlarmObj[1], abyAlarmObj[2] );
                }
                else
                {
                    if (SNMP_SUCCESS !=SendPrmAlarmTrap(m_pcSnmpNodes, tAlarmData, TRAP_ALARM_GENERATE))
                    {
                        Agtlog(LOG_ERROR, "[ProcBoardTempStatusChange] temperature status<%d> trap<%d> send failed !\n",
                                           ptBrdStatus->byStatus, TRAP_ALARM_GENERATE);
                    }
                    else
                    {
                        Agtlog(LOG_INFORM, "[ProcBoardTempStatusChange] temperature status<%d> trap<%d> send succeed !\n",
                                            ptBrdStatus->byStatus, TRAP_ALARM_GENERATE);
                    }
                }
            }
        }

        break;

	default:
        Agtlog(LOG_ERROR, "[ProcBoardTempStatusChange] unexpected msg%d<%s> received!\n", pcMsg->event, OspEventDesc(pcMsg->event));
		break;
	}
	
	return;
}

/*=============================================================================
  函 数 名： ProcBoardCpuStatusChange
  功    能： 处理单板CPU状态改变
  算法实现： 
  全局变量： 
  参    数： CMessage * const pcMsg
  返 回 值： void 
  ----------------------------------------------------------------------
  修改记录    ：
  日  期		版本		修改人		走读人    修改内容
  2011/10/27    4.0			陈代伟                  创建
=============================================================================*/
void CMsgCenterInst::ProcBoardCpuStatusChange(CMessage * const pcMsg )
{
    if(NULL == pcMsg)
    {
        Agtlog(LOG_ERROR, "[ProcBoardCpuStatusChange] The pointer can not be Null\n");
        return;
    }
    if ( !g_cCfgParse.HasSnmpNms() )
    {
        Agtlog(LOG_WARN, "[ProcBoardCpuStatusChange] There's no NMS configed !\n");
        return;
    }
	
    TBrdStatus *ptBrdStatus = NULL;
    u8   abyAlarmObj[MAX_ALARMOBJECT_NUM];
    TMcupfmAlarmEntry     tAlarmData;
	u16    wLoop = 0;
    BOOL32 bAlarmResult = FALSE;
    memset(abyAlarmObj, 0, sizeof(abyAlarmObj));
	
	switch(pcMsg->event)
	{
	case SVC_AGT_BRD_CPU_STATUS_NOTIF:
		ptBrdStatus = ( TBrdStatus * )pcMsg->content;
        abyAlarmObj[0] = ( u8 )ptBrdStatus->byLayer;
        abyAlarmObj[1] = ( u8 )ptBrdStatus->bySlot;
        abyAlarmObj[2] = ( u8 )ptBrdStatus->byType;
        abyAlarmObj[3] = ( u8 )ptBrdStatus->byStatus;
        abyAlarmObj[4] = 0;
		
        bAlarmResult = g_cAlarmProc.FindAlarm( ALARM_MCU_BRD_CPU_ABNORMAL, ALARMOBJ_MCU_BOARD, abyAlarmObj, &tAlarmData );
        if( bAlarmResult )
        {
            if( BRD_STATUS_NORMAL == ptBrdStatus->byStatus)    //normal
            {
                bAlarmResult = g_cAlarmProc.DeleteAlarm( tAlarmData.m_dwSerialNo );
                if(  !bAlarmResult )
                {
                    Agtlog(LOG_ERROR, "[ProcBoardCpuStatusChange] DeleteAlarm( AlarmCode = %lu, Object = %d, %d, %d ) failed!\n", 
						tAlarmData.m_dwAlarmCode, tAlarmData.m_achObject[0], 
						tAlarmData.m_achObject[1], tAlarmData.m_achObject[2]);
                }
                else
                {
                    if (SNMP_SUCCESS !=SendPrmAlarmTrap(m_pcSnmpNodes, tAlarmData, TRAP_ALARM_RESTORE))
                    {
                        Agtlog(LOG_ERROR, "[ProcBoardCpuStatusChange] cpu status<%d> trap<%d> send failed !\n",
							ptBrdStatus->byStatus, TRAP_ALARM_RESTORE);
                    }
                    else
                    {
                        Agtlog(LOG_INFORM, "[ProcBoardCpuStatusChange] cpu status<%d> trap<%d> send succeed !\n",
							ptBrdStatus->byStatus, TRAP_ALARM_RESTORE);
                    }
                }
            }
        }
        else        // no such alarm
        {
            if( BRD_STATUS_ABNORMAL == ptBrdStatus->byStatus)    // abnormal
            {
                bAlarmResult = g_cAlarmProc.AddAlarm( ALARM_MCU_BRD_CPU_ABNORMAL, ALARMOBJ_MCU_BOARD, abyAlarmObj, &tAlarmData );
                if(  !bAlarmResult )
                {
                    Agtlog(LOG_ERROR, "[ProcBoardCpuStatusChange] AddAlarm( AlarmCode = %lu, Object = %d, %d ) failed!\n", 
                        ALARM_MCU_BRD_CPU_ABNORMAL, abyAlarmObj[0], abyAlarmObj[1], abyAlarmObj[2] );
                }
                else
                {
                    if (SNMP_SUCCESS !=SendPrmAlarmTrap(m_pcSnmpNodes, tAlarmData, TRAP_ALARM_GENERATE))
                    {
                        Agtlog(LOG_ERROR, "[ProcBoardCpuStatusChange] cpu status<%d> trap<%d> send failed !\n",
							ptBrdStatus->byStatus, TRAP_ALARM_GENERATE);
                    }
                    else
                    {
                        Agtlog(LOG_INFORM, "[ProcBoardCpuStatusChange] cpu status<%d> trap<%d> send succeed !\n",
							ptBrdStatus->byStatus, TRAP_ALARM_GENERATE);
                    }
                }
            }
        }
		
        break;
		
	default:
        Agtlog(LOG_ERROR, "[ProcBoardCpuStatusChange] unexpected msg%d<%s> received!\n", pcMsg->event, OspEventDesc(pcMsg->event));
		break;
	}
	
	return;
}

/*=============================================================================
  函 数 名： ProcBoardSoftwareVersionNotif
  功    能： 处理单板CPU状态改变
  算法实现： 
  全局变量： 
  参    数： CMessage * const pcMsg
  返 回 值： void 
  ----------------------------------------------------------------------
  修改记录    ：
  日  期		版本		修改人		走读人    修改内容
  2011/12/12    4.0			陈代伟                  创建
=============================================================================*/
/*void CMsgCenterInst::ProcBoardSoftwareVersionNotif(CMessage * const pcMsg )
{
    if(NULL == pcMsg)
    {
        Agtlog(LOG_ERROR, "[ProcBoardSoftwareVersionNotif] The pointer can not be Null\n");
        return;
    }

    if ( !g_cCfgParse.HasSnmpNms() )
    {
        Agtlog(LOG_WARN, "[ProcBoardSoftwareVersionNotif] There's no NMS configed !\n");
        return;
    }

	//TODO 与网管通信相关实现[12/13/2011 chendaiwei]
	
	return;
}
*/
/*=============================================================================
  函 数 名： ProcBrdFanStatusChange
  功    能： 处理单板风扇状态
  算法实现： 
  全局变量： 
  参    数： CMessage * const pcMsg
  返 回 值： void 
=============================================================================*/
void CMsgCenterInst::ProcBrdFanStatusChange( CMessage * const pcMsg )
{
    
    if(NULL == pcMsg)
    {
        Agtlog(LOG_ERROR, "[ProcBrdFanStatusChange] The pointer can not be Null\n");
        return;
    }
    if ( !g_cCfgParse.HasSnmpNms() )
    {
        Agtlog(LOG_WARN, "[ProcBrdFanStatusChange] There's no NMS configed !\n");
        return;
    }

    u8   abyAlarmObj[MAX_ALARMOBJECT_NUM];
    BOOL32 bAlarmResult;
    TMcupfmAlarmEntry     tAlarmData;
	TBrdFanStatus* ptBrdFanStatus = NULL;	
    memset(abyAlarmObj, 0, sizeof(abyAlarmObj));

	switch(pcMsg->event)
	{
	case MCU_BRD_FAN_STATUS:
		ptBrdFanStatus = (TBrdFanStatus*)pcMsg->content;
        abyAlarmObj[0] = ptBrdFanStatus->byLayer;
        abyAlarmObj[1] = ptBrdFanStatus->bySlot;
        abyAlarmObj[2] = ptBrdFanStatus->byType;
        abyAlarmObj[3] = ptBrdFanStatus->byFanId;
        abyAlarmObj[4] = ptBrdFanStatus->byStatus;

        bAlarmResult = g_cAlarmProc.FindAlarm( ALARM_MCU_BOARD_FAN_STOP, ALARMOBJ_MCU_BRD_FAN, abyAlarmObj, &tAlarmData );
        if( bAlarmResult )
        {
            if( ptBrdFanStatus->byStatus  == 0 )      //normal
            {
                bAlarmResult = g_cAlarmProc.DeleteAlarm( tAlarmData.m_dwSerialNo );
                if( !bAlarmResult )
                {
                    Agtlog(LOG_ERROR, "[ProcBrdFanStatusChange] DeleteAlarm( AlarmCode = %lu, Object = %d, %d ) failed!\n", 
                                       tAlarmData.m_dwAlarmCode, tAlarmData.m_achObject[0], 
                                       tAlarmData.m_achObject[1] );
                }
                else
                {
                    if (SNMP_SUCCESS !=SendPrmAlarmTrap(m_pcSnmpNodes, tAlarmData, TRAP_ALARM_RESTORE))
                    {
                        Agtlog(LOG_ERROR, "[ProcBrdFanStatusChange] FAN status<%d> trap<%d> send failed !\n",
                                           ptBrdFanStatus->byStatus, TRAP_ALARM_RESTORE);
                    }
                    else
                    {
                        Agtlog(LOG_INFORM, "[ProcBrdFanStatusChange] FAN status<%d> trap<%d> send succeed !\n",
                                            ptBrdFanStatus->byStatus, TRAP_ALARM_RESTORE);
                    }
                }
            }
        }
        else        // no such alarm
        {
            if( ptBrdFanStatus->byStatus != 0 )     // abnormal
            {
                bAlarmResult = g_cAlarmProc.AddAlarm( ALARM_MCU_BOARD_FAN_STOP, ALARMOBJ_MCU_BRD_FAN, abyAlarmObj, &tAlarmData );
                if( !bAlarmResult )
                {
                    Agtlog(LOG_ERROR, "[ProcBrdFanStatusChange] AddAlarm( AlarmCode = %lu, Object = %d, %d ) failed!\n", 
                        ALARM_MCU_BOARD_FAN_STOP, abyAlarmObj[0], abyAlarmObj[1] );
                }
                else
                {
                    if (SNMP_SUCCESS !=SendPrmAlarmTrap(m_pcSnmpNodes, tAlarmData, TRAP_ALARM_GENERATE))
                    {
                        Agtlog(LOG_ERROR, "[ProcBrdFanStatusChange] FAN status<%d> trap<%d> send failed !\n",
                                           ptBrdFanStatus->byStatus, TRAP_ALARM_GENERATE);
                    }
                    else
                    {
                        Agtlog(LOG_INFORM, "[ProcBrdFanStatusChange] FAN status<%d> trap<%d> send failed !\n",
                                            ptBrdFanStatus->byStatus, TRAP_ALARM_GENERATE);
                    }
                }
            }
        }
        break;

	default:
        Agtlog(LOG_ERROR, "[ProcBrdFanStatusChange] unexpected msg%d<%s> received!\n", pcMsg->event, OspEventDesc(pcMsg->event));
		break;
	}

	return ;
}

/*=============================================================================
  函 数 名： ProcSoftwareStatusChange
  功    能： 处理更新软件
  算法实现： 
  全局变量： 
  参    数： CMessage * const pcMsg
  返 回 值： void 
=============================================================================*/
void CMsgCenterInst::ProcSoftwareStatusChange( CMessage * const pcMsg )
{
    if(NULL == pcMsg)
    {
        Agtlog(LOG_ERROR, "[ProcSoftwareStatusChange] The pointer can not be Null\n");
        return;
    }
    if ( !g_cCfgParse.HasSnmpNms() )
    {
        Agtlog(LOG_WARN, "[ProcSoftwareStatusChange] There's no NMS configed !\n");
        return;
    }

    u8   abyAlarmObj[MAX_ALARMOBJECT_NUM];
    TMcupfmAlarmEntry     tAlarmData;
	TSoftwareStatus* ptSoftStatus = NULL;
    BOOL32  bAlarmResult = FALSE;
    CTaskStatus* pcTaskStatus = NULL;

    u32 dwMemAllocRate = 0;
	memset( abyAlarmObj, 0, sizeof(abyAlarmObj) );

	switch(pcMsg->event)
	{
    case SVC_AGT_MEMORY_STATUS:     //内存状态改变

		ptSoftStatus = (TSoftwareStatus*)pcMsg->content;
        abyAlarmObj[0] = ( u8 )ptSoftStatus->byLayer;
        abyAlarmObj[1] = ( u8 )ptSoftStatus->bySlot;
        abyAlarmObj[2] = ( u8 )ptSoftStatus->byType;

// 		dwMemAllocRate = *(u32*)pcMsg->content;
//         *(u32*)&abyAlarmObj[0] = dwMemAllocRate;
        
        bAlarmResult = g_cAlarmProc.FindAlarm( ALARM_MCU_MEMORYERROR, ALARMOBJ_MCU_SOFTWARE, abyAlarmObj, &tAlarmData );
        if( bAlarmResult )
        {
            if( 0 == ptSoftStatus->byStatus )      // normal
            {
                bAlarmResult = g_cAlarmProc.DeleteAlarm( tAlarmData.m_dwSerialNo );
                if(  !bAlarmResult )
                {
					Agtlog( LOG_VERBOSE, "McuAgent: DeleteAlarm( AlarmCode = %lu, Object = %d, %d, %d ) failed!\n", 
                        tAlarmData.m_dwAlarmCode, tAlarmData.m_achObject[0], 
                        tAlarmData.m_achObject[1], tAlarmData.m_achObject[2] );
                }
                else
                {
                    SendPrmAlarmTrap(m_pcSnmpNodes, tAlarmData, TRAP_ALARM_RESTORE);
                }
            }
        }
        else        // no such alarm
        {
            if( 1 == ptSoftStatus->byStatus)     // abnormal
            {
                bAlarmResult = g_cAlarmProc.AddAlarm( ALARM_MCU_MEMORYERROR, ALARMOBJ_MCU_SOFTWARE, abyAlarmObj, &tAlarmData );
                if(  !bAlarmResult )
                {
					Agtlog(LOG_VERBOSE, "McuAgent: AddAlarm( AlarmCode = %lu, Object = %d, %d, %d ) failed!\n", 
                        ALARM_MCU_MEMORYERROR, abyAlarmObj[0], abyAlarmObj[1], abyAlarmObj[2] );
                }
                else
                {
                    SendPrmAlarmTrap(m_pcSnmpNodes, tAlarmData, TRAP_ALARM_GENERATE);
                }
            }
        }
        break;

    case SVC_AGT_FILESYSTEM_STATUS:     //文件系统状态改变
		ptSoftStatus = (TSoftwareStatus*)pcMsg->content;
        abyAlarmObj[0] = ( u8 )ptSoftStatus->byLayer;
        abyAlarmObj[1] = ( u8 )ptSoftStatus->bySlot;
        abyAlarmObj[2] = ( u8 )ptSoftStatus->byType;
        abyAlarmObj[3] = 0;
        abyAlarmObj[4] = 0;

        bAlarmResult = g_cAlarmProc.FindAlarm( ALARM_MCU_FILESYSTEMERROR, ALARMOBJ_MCU_SOFTWARE, abyAlarmObj, &tAlarmData );
        if( bAlarmResult )
        {
            if( 0 == ptSoftStatus->byStatus)      // normal
            {
                bAlarmResult = g_cAlarmProc.DeleteAlarm( tAlarmData.m_dwSerialNo );
                if(  !bAlarmResult )
                {
                    Agtlog(LOG_ERROR, "[ProcSoftwareStatusChange] DeleteAlarm( AlarmCode = %lu, Object = %d, %d, %d ) failed!\n", 
                                        tAlarmData.m_dwAlarmCode, tAlarmData.m_achObject[0], 
                                        tAlarmData.m_achObject[1], tAlarmData.m_achObject[2] );
                }
                else
                {
                    if (SNMP_SUCCESS !=SendPrmAlarmTrap(m_pcSnmpNodes, tAlarmData, TRAP_ALARM_RESTORE))
                    {
                        Agtlog(LOG_ERROR, "[ProcSoftwareStatusChange] File system status<%d> trap<%d> send failed !\n",
                                           ptSoftStatus->byStatus, TRAP_ALARM_RESTORE);
                    }
                    else
                    {
                        Agtlog(LOG_INFORM, "[ProcSoftwareStatusChange] File system status<%d> trap<%d> send succeed !\n",
                                            ptSoftStatus->byStatus, TRAP_ALARM_RESTORE);
                    }
                }
            }
        }
        else        // no such alarm
        {
            if( 1 == ptSoftStatus->byStatus)     // abnormal
            {
                bAlarmResult = g_cAlarmProc.AddAlarm( ALARM_MCU_FILESYSTEMERROR, ALARMOBJ_MCU_SOFTWARE, abyAlarmObj, &tAlarmData );
                if(  !bAlarmResult )
                {
                    Agtlog(LOG_ERROR, "[ProcSoftwareStatusChange] AddAlarm( AlarmCode = %lu, Object = %d, %d, %d ) failed!\n", 
                           ALARM_MCU_FILESYSTEMERROR, abyAlarmObj[0], abyAlarmObj[1], abyAlarmObj[1] );
                }
                else
                {
                    if (SNMP_SUCCESS !=SendPrmAlarmTrap(m_pcSnmpNodes, tAlarmData, TRAP_ALARM_GENERATE))
                    {
                        Agtlog(LOG_ERROR, "[ProcSoftwareStatusChange] File system status<%d> trap<%d> send failed !\n",
                                           ptSoftStatus->byStatus, TRAP_ALARM_GENERATE);
                    }
                    else
                    {
                        Agtlog(LOG_INFORM, "[ProcSoftwareStatusChange] File system status<%d> trap<%d> send succeed !\n",
                                            ptSoftStatus->byStatus, TRAP_ALARM_GENERATE);
                    }
                }
            }
        }
        break;

	case SVC_AGT_TASK_STATUS:       // 任务状态改变(mcu guard 发送过来)
        pcTaskStatus = ( CTaskStatus * )pcMsg->content;

        //任务改变仅用appID 一项做标识就可以了
        //abyAlarmObj[0] = ( u8 )pcTaskStatus->status;
        *(u16*)&abyAlarmObj[0] = pcTaskStatus->appid;

        bAlarmResult = g_cAlarmProc.FindAlarm( ALARM_MCU_TASK_BUSY, ALARMOBJ_MCU_TASK, abyAlarmObj, &tAlarmData );
		if( bAlarmResult )
		{
			if( 0 == pcTaskStatus->status)    //normal
			{
				bAlarmResult = g_cAlarmProc.DeleteAlarm( tAlarmData.m_dwSerialNo );
                if(  !bAlarmResult )
                {
					Agtlog(LOG_ERROR, "[ProcSoftwareStatusChange] DeleteAlarm( AlarmCode = %lu, Object = %d, %d, %d, %d, %d ) failed!\n", 
					                    tAlarmData.m_dwAlarmCode, tAlarmData.m_achObject[0], 
					                    tAlarmData.m_achObject[1], tAlarmData.m_achObject[2], 
					                    tAlarmData.m_achObject[3], tAlarmData.m_achObject[4] );
                }
				else
                {
                    if (SNMP_SUCCESS !=SendPrmAlarmTrap(m_pcSnmpNodes, tAlarmData, TRAP_ALARM_RESTORE))
                    {
                        Agtlog(LOG_ERROR, "[ProcSoftwareStatusChange] Task status<%d> trap<%d> send failed !\n",
                                           pcTaskStatus->status, TRAP_ALARM_RESTORE);
                    }
                    else
                    {
                        Agtlog(LOG_INFORM, "[ProcSoftwareStatusChange] Task status<%d> trap<%d> send succeed !\n",
                                            pcTaskStatus->status, TRAP_ALARM_RESTORE);
                    }
                }
			}
		}
		else        // no such alarm
		{
			if( 0 != pcTaskStatus->status)    // abnormal
			{
				bAlarmResult = g_cAlarmProc.AddAlarm( ALARM_MCU_TASK_BUSY, ALARMOBJ_MCU_TASK, abyAlarmObj, &tAlarmData );
                if(  !bAlarmResult )
                {
					Agtlog(LOG_ERROR, "[ProcSoftwareStatusChange] AddAlarm( AlarmCode = %lu, Object = %d, %d, %d, %d, %d ) failed!\n", 
					ALARM_MCU_TASK_BUSY, abyAlarmObj[0], abyAlarmObj[1], abyAlarmObj[2], abyAlarmObj[3], abyAlarmObj[4]);
                }
				else
                {
                    if (SNMP_SUCCESS !=SendPrmAlarmTrap(m_pcSnmpNodes, tAlarmData, TRAP_ALARM_GENERATE))
                    {
                        Agtlog(LOG_ERROR, "[ProcSoftwareStatusChange] Task status<%d> trap<%d> send failed !\n",
                                           pcTaskStatus->status, TRAP_ALARM_GENERATE);
                    }
                    else
                    {
                        Agtlog(LOG_INFORM, "[ProcSoftwareStatusChange] Task status<%d> trap<%d> send succeed !\n",
                                            pcTaskStatus->status, TRAP_ALARM_GENERATE);
                    }
                }
            }
		}
		break;
		
	default:
        Agtlog(LOG_INFORM, "[ProcSoftwareStatusChange] unexpected msg%d<%s> received!\n", pcMsg->event, OspEventDesc(pcMsg->event));
		break;
	}
	return ;
}

/*=============================================================================
  函 数 名： ProcPowerFanStatusChange
  功    能： 保留
  算法实现： 
  全局变量： 
  参    数： CMessage * const pcMsg
  返 回 值： void 
=============================================================================*/
void CMsgCenterInst::ProcPowerFanStatusChange( CMessage * const pcMsg )
{
    if(NULL == pcMsg)
    {
        Agtlog(LOG_ERROR, "[ProcPowerFanStatusChange] The pointer can not be Null\n");
        return;
    }

    if ( !g_cCfgParse.HasSnmpNms() )
    {
        Agtlog(LOG_WARN, "[ProcPowerFanStatusChange] There's no NMS configed !\n");
        return;
    }

    u8   abyAlarmObj[MAX_ALARMOBJECT_NUM];
    memset(abyAlarmObj, 0, sizeof(abyAlarmObj));
    TMcupfmAlarmEntry    tAlarmData;
	TPowerFanStatus* ptPowerFanStatus = NULL;

	switch(pcMsg->event)
	{
	case SVC_AGT_POWER_FAN_STATUS:
		ptPowerFanStatus = (TPowerFanStatus*)pcMsg->content;
        abyAlarmObj[0] = (u8)ptPowerFanStatus->byFanPos;
        abyAlarmObj[1] = (u8)ptPowerFanStatus->bySlot;
        abyAlarmObj[2] = 0;
        abyAlarmObj[3] = 0;
        abyAlarmObj[4] = 0;

        if( g_cAlarmProc.FindAlarm( ALARM_MCU_POWER_FAN_ABNORMAL, ALARMOBJ_MCU_POWER, abyAlarmObj, &tAlarmData ) )
        {
            if( 0 == ptPowerFanStatus->byStatus)      // 正常
            {
                if( !g_cAlarmProc.DeleteAlarm( tAlarmData.m_dwSerialNo ) )
                {
                    Agtlog(LOG_ERROR, "[ProcPowerFanStatusChange] DeleteAlarm( AlarmCode = %lu, Object = %d, %d ) failed!\n", 
                                       tAlarmData.m_dwAlarmCode, tAlarmData.m_achObject[0], 
                                       tAlarmData.m_achObject[1] );
                }
                else
                {
                    if (SNMP_SUCCESS !=SendPrmAlarmTrap(m_pcSnmpNodes, tAlarmData, TRAP_ALARM_RESTORE))
                    {
                        Agtlog(LOG_ERROR, "[ProcPowerFanStatusChange] Task status<%d> trap<%d> send failed !\n",
                                           ptPowerFanStatus->byStatus, TRAP_ALARM_RESTORE);
                    }
                    else
                    {
                        Agtlog(LOG_INFORM, "[ProcPowerFanStatusChange] Task status<%d> trap<%d> send succeed !\n",
                                            ptPowerFanStatus->byStatus, TRAP_ALARM_RESTORE);
                    }
                }
            }
        }
        else
        {
            if( 0 != ptPowerFanStatus->byStatus )
            {
                if( !g_cAlarmProc.AddAlarm( ALARM_MCU_POWER_FAN_ABNORMAL, ALARMOBJ_MCU_POWER, abyAlarmObj, &tAlarmData ) )
                {
                    Agtlog(LOG_ERROR, "[ProcPowerFanStatusChange] AddAlarm( AlarmCode = %lu, Object = %d, %d ) failed!\n", 
                                       ALARM_MCU_POWER_FAN_ABNORMAL, abyAlarmObj[0], abyAlarmObj[1] );
                }
                else
                {   
                    if (SNMP_SUCCESS !=SendPrmAlarmTrap(m_pcSnmpNodes, tAlarmData, TRAP_ALARM_GENERATE))
                    {
                        Agtlog(LOG_ERROR, "[ProcPowerFanStatusChange] Task status<%d> trap<%d> send failed !\n",
                                           ptPowerFanStatus->byStatus, TRAP_ALARM_GENERATE);
                    }
                    else
                    {
                        Agtlog(LOG_INFORM, "[ProcPowerFanStatusChange] Task status<%d> trap<%d> send succeed !\n",
                                            ptPowerFanStatus->byStatus, TRAP_ALARM_GENERATE);
                    }
                }
            }
        }
        break;

	default:
        Agtlog(LOG_ERROR, "[ProcPowerFanStatusChange] unexpected msg%d<%s> received!\n", 
                                              pcMsg->event, OspEventDesc(pcMsg->event));
		break;
	}

	return ;
}

/*=============================================================================
  函 数 名： ProcBoxFanStatusChange
  功    能： 机箱风扇异常上报告警处理
  算法实现： 
  全局变量： 
  参    数： CMessage * const pcMsg
  返 回 值： void 
  ----------------------------------------------------------------------------
  修改记录：
  日   期        版本        修改人        修改内容
  2010/12/14     5.0         苗庆松          创建
=============================================================================*/
void CMsgCenterInst::ProcBoxFanStatusChange( CMessage * const pcMsg )
{
    if(NULL == pcMsg)
    {
        Agtlog(LOG_ERROR, "[ProcBoxFanStatusChange] The pointer can not be Null\n");
        return;
    }
    if ( !g_cCfgParse.HasSnmpNms() )
    {
        Agtlog(LOG_WARN, "[ProcBoxFanStatusChange] There's no NMS configed !\n");
        return;
    }
	
    u8   abyAlarmObj[MAX_ALARMOBJECT_NUM];
    memset(abyAlarmObj, 0, sizeof(abyAlarmObj));
    TMcupfmAlarmEntry    tAlarmData;
	TBoxFanStatus* ptBoxFanStatus = NULL;
	
	switch(pcMsg->event)
	{
	case SVC_AGT_BOX_FAN_STATUS:
		ptBoxFanStatus = (TBoxFanStatus*)pcMsg->content;
        abyAlarmObj[0] = ( u8 )ptBoxFanStatus->byFanPos;
        abyAlarmObj[1] = ( u8 )ptBoxFanStatus->bySlot;
        abyAlarmObj[2] = 0;
        abyAlarmObj[3] = 0;
        abyAlarmObj[4] = 0;
		
        if( g_cAlarmProc.FindAlarm( ALARM_MCU_BOX_FAN_ABNORMAL, ALARMOBJ_MCU_BOX_FAN, abyAlarmObj, &tAlarmData ) )
        {
            if( 0 == ptBoxFanStatus->byStatus)      // 正常
            {
                if( !g_cAlarmProc.DeleteAlarm( tAlarmData.m_dwSerialNo ) )
                {
                    Agtlog(LOG_ERROR, "[ProcBoxFanStatusChange] DeleteAlarm( AlarmCode = %lu, Object = %d, %d ) failed!\n", 
						tAlarmData.m_dwAlarmCode, tAlarmData.m_achObject[0], 
						tAlarmData.m_achObject[1] );
                }
                else
                {
                    if (SNMP_SUCCESS !=SendPrmAlarmTrap(m_pcSnmpNodes, tAlarmData, TRAP_ALARM_RESTORE))
                    {
                        Agtlog(LOG_ERROR, "[ProcBoxFanStatusChange] Task status<%d> trap<%d> send failed !\n",
							ptBoxFanStatus->byStatus, TRAP_ALARM_RESTORE);
                    }
                    else
                    {
                        Agtlog(LOG_INFORM, "[ProcBoxFanStatusChange] Task status<%d> trap<%d> send succeed !\n",
							ptBoxFanStatus->byStatus, TRAP_ALARM_RESTORE);       
                    }
                }
            }
        }
        else
        {
            if( 0 != ptBoxFanStatus->byStatus )
            {
                if( !g_cAlarmProc.AddAlarm( ALARM_MCU_BOX_FAN_ABNORMAL, ALARMOBJ_MCU_BOX_FAN, abyAlarmObj, &tAlarmData ) )
                {
                    Agtlog(LOG_ERROR, "[ProcBoxFanStatusChange] AddAlarm( AlarmCode = %lu, Object = %d, %d ) failed!\n", 
						ALARM_MCU_BOX_FAN_ABNORMAL, abyAlarmObj[0], abyAlarmObj[1] );
                }
                else
                {   
                    if (SNMP_SUCCESS !=SendPrmAlarmTrap(m_pcSnmpNodes, tAlarmData, TRAP_ALARM_GENERATE))
                    {
                        Agtlog(LOG_ERROR, "[ProcBoxFanStatusChange] Task status<%d> trap<%d> send failed !\n",
							ptBoxFanStatus->byStatus, TRAP_ALARM_GENERATE);
                    }
                    else
                    {
                        Agtlog(LOG_INFORM, "[ProcBoxFanStatusChange] Task status<%d> trap<%d> send succeed !\n",
							ptBoxFanStatus->byStatus, TRAP_ALARM_GENERATE);
                    }
                }
            }
        }
        break;
		
	default:
        Agtlog(LOG_ERROR, "[ProcBoxFanStatusChange] unexpected msg%d<%s> received!\n", 
			pcMsg->event, OspEventDesc(pcMsg->event));
		break;
	}
	
	return ;
}

/*=============================================================================
  函 数 名： ProcMPCMemoryStatusChange
  功    能： MPC板Mermory状态异常上报告警处理
  算法实现： 
  全局变量： 
  参    数： CMessage * const pcMsg
  返 回 值： void 
  ----------------------------------------------------------------------------
  修改记录：
  日   期        版本        修改人        修改内容
  2010/12/15     5.0         苗庆松          创建
=============================================================================*/
void CMsgCenterInst::ProcMPCMemoryStatusChange( CMessage * const pcMsg)
{
	if( NULL == pcMsg)
	{
		Agtlog(LOG_ERROR, "[ProcMPCMemoryStatusChange] The pointer can not be Null!\n");
        return;
	}
    if ( !g_cCfgParse.HasSnmpNms() )
    {
        Agtlog(LOG_WARN, "[ProcMPCMemoryStatusChange] There's no NMS configed !\n");
        return;
    }
    
	u8   abyAlarmObj[MAX_ALARMOBJECT_NUM];
    memset(abyAlarmObj, 0, sizeof(abyAlarmObj));
    TMcupfmAlarmEntry    tAlarmData;
    TMPCMemoryStatus* ptMPCMemoryStatus = NULL;

	switch(pcMsg->event)
	{
	case SVC_AGT_MPCMEMORY_STATUS:
		ptMPCMemoryStatus = (TMPCMemoryStatus*)pcMsg->content;
        abyAlarmObj[0] = MEMORY_MCU_OBJECT;
        abyAlarmObj[1] = 0;
        abyAlarmObj[2] = 0;
        abyAlarmObj[3] = 0;
        abyAlarmObj[4] = 0;
		
        if( g_cAlarmProc.FindAlarm( ALARM_MCU_MPC_MEMORY_ABNORMAL, ALARMOBJ_MCU_MPC_MEMORY, abyAlarmObj, &tAlarmData ) )
        {
            if( 0 == ptMPCMemoryStatus->byMemoryStatus)      // 正常
            {
                if( !g_cAlarmProc.DeleteAlarm( tAlarmData.m_dwSerialNo ) )
                {
                    Agtlog(LOG_ERROR, "[ProcMPCMemoryStatusChange] DeleteAlarm( AlarmCode = %lu, Object = %d, %d ) failed!\n", 
						tAlarmData.m_dwAlarmCode, tAlarmData.m_achObject[0], tAlarmData.m_achObject[1] );
                }
                else
                {
                    if (SNMP_SUCCESS !=SendPrmAlarmTrap(m_pcSnmpNodes, tAlarmData, TRAP_ALARM_RESTORE))
                    {
                        Agtlog(LOG_ERROR, "[ProcMPCMemoryStatusChange] Task status<%d> trap<%d> send failed !\n",
							ptMPCMemoryStatus->byMemoryStatus, TRAP_ALARM_RESTORE);
                    }
                    else
                    {
                        Agtlog(LOG_INFORM, "[ProcMPCMemoryStatusChange] Task status<%d> trap<%d> send succeed !\n",
							ptMPCMemoryStatus->byMemoryStatus, TRAP_ALARM_RESTORE);

						OspPrintf( TRUE, FALSE, "[ProcMPCMemoryStatusChange] Alarm restore!\n");          // [miaoqingsong 自测打印]
                    }
                }
            }
        }
        else
        {
            if( 0 != ptMPCMemoryStatus->byMemoryStatus )    // 异常
            {
                if( !g_cAlarmProc.AddAlarm( ALARM_MCU_MPC_MEMORY_ABNORMAL, ALARMOBJ_MCU_MPC_MEMORY, abyAlarmObj, &tAlarmData ) )
                {
                    Agtlog(LOG_ERROR, "[ProcMPCMemoryStatusChange] AddAlarm( AlarmCode = %lu, Object = %d ) failed!\n", 
						ALARM_MCU_MPC_MEMORY_ABNORMAL, abyAlarmObj[0] );
                }
                else
                {   
                    if (SNMP_SUCCESS !=SendPrmAlarmTrap(m_pcSnmpNodes, tAlarmData, TRAP_ALARM_GENERATE))
                    {
                        Agtlog(LOG_ERROR, "[ProcMPCMemoryStatusChange] Task status<%d> trap<%d> send failed !\n",
							ptMPCMemoryStatus->byMemoryStatus, TRAP_ALARM_GENERATE);
                    }
                    else
                    {
                        Agtlog(LOG_INFORM, "[ProcMPCMemoryStatusChange] Task status<%d> trap<%d> send succeed !\n",
							ptMPCMemoryStatus->byMemoryStatus, TRAP_ALARM_GENERATE);

						OspPrintf( TRUE, FALSE, "[ProcMPCMemoryStatusChange] Alarm generate!\n");         // [miaoqingsong 自测打印]

                    }
                }
            }
        }
        break;
		
	default:
        Agtlog(LOG_ERROR, "[ProcMPCCpuStatusChange] unexpected msg%d<%s> received!\n", 
			pcMsg->event, OspEventDesc(pcMsg->event));
		break;
	}
	return ;
}

/*=============================================================================
  函 数 名： ProcMpc2TempStatusChange
  功    能： MPC2板温度异常上报告警处理
  算法实现： 
  全局变量： 
  参    数： CMessage * const pcMsg
  返 回 值： void 
  ----------------------------------------------------------------------------
  修改记录：
  日   期        版本        修改人        修改内容
  2011/01/13     5.0         苗庆松          创建
=============================================================================*/
void CMsgCenterInst::ProcMpc2TempStatusChange( CMessage * const pcMsg )
{
	if ( NULL == pcMsg )
	{
		Agtlog(LOG_ERROR, "[ProcMpc2TempStatusChange] The pointer can not be Null!\n");
        return;
	}
    
	if ( !g_cCfgParse.HasSnmpNms() )
    {
        Agtlog(LOG_WARN, "[ProcMpc2TempStatusChange] There's no NMS configed !\n");
        return;
    }
    
	u8   abyAlarmObj[MAX_ALARMOBJECT_NUM];
    memset(abyAlarmObj, 0, sizeof(abyAlarmObj));
    TMcupfmAlarmEntry    tAlarmData;
    TMPC2TempStatus* ptMPC2TemStatus = NULL;

	switch(pcMsg->event)
	{
	case SVC_AGT_MPC2TEMP_STATUS:
		ptMPC2TemStatus = (TMPC2TempStatus*)pcMsg->content;
        abyAlarmObj[0] = TEMP_MCU_OBJECT;
        abyAlarmObj[1] = 0;
        abyAlarmObj[2] = 0;
        abyAlarmObj[3] = 0;
        abyAlarmObj[4] = 0;
		
        if( g_cAlarmProc.FindAlarm( ALARM_MCU_MPC_TEMP_ABNORMAL, ALARMOBJ_MCU_MPC_TEMP, abyAlarmObj, &tAlarmData ) )
        {
            if( 0 == ptMPC2TemStatus->byMpc2TempStatus)      // 正常
            {
                if( !g_cAlarmProc.DeleteAlarm( tAlarmData.m_dwSerialNo ) )
                {
                    Agtlog(LOG_ERROR, "[ProcMpc2TempStatusChange] DeleteAlarm( AlarmCode = %lu, Object = %d ) failed!\n", 
						tAlarmData.m_dwAlarmCode, tAlarmData.m_achObject[0] );
                }
                else
                {
                    if (SNMP_SUCCESS !=SendPrmAlarmTrap(m_pcSnmpNodes, tAlarmData, TRAP_ALARM_RESTORE))
                    {
                        Agtlog(LOG_ERROR, "[ProcMpc2TempStatusChange] Task status<%d> trap<%d> send failed !\n",
							ptMPC2TemStatus->byMpc2TempStatus, TRAP_ALARM_RESTORE);
                    }
                    else
                    {
                        Agtlog(LOG_INFORM, "[ProcMpc2TempStatusChange] Task status<%d> trap<%d> send succeed !\n",
							ptMPC2TemStatus->byMpc2TempStatus, TRAP_ALARM_RESTORE);

						OspPrintf( TRUE, FALSE, "[ProcMpc2TempStatusChange] Alarm restore!\n");           // [miaoqingsong 自测打印]
                    }
                }
            }
        }
        else
        {
            if( 0 != ptMPC2TemStatus->byMpc2TempStatus )
            {
                if( !g_cAlarmProc.AddAlarm( ALARM_MCU_MPC_TEMP_ABNORMAL, ALARMOBJ_MCU_MPC_TEMP, abyAlarmObj, &tAlarmData ) )
                {
                    Agtlog(LOG_ERROR, "[ProcMpc2TempStatusChange] AddAlarm( AlarmCode = %lu, Object = %d ) failed!\n", 
						tAlarmData.m_dwAlarmCode, tAlarmData.m_achObject[0] );
                }
                else
                {   
                    if (SNMP_SUCCESS !=SendPrmAlarmTrap(m_pcSnmpNodes, tAlarmData, TRAP_ALARM_GENERATE))
                    {
                        Agtlog(LOG_ERROR, "[ProcMpc2TempStatusChange] Task status<%d> trap<%d> send failed !\n",
							ptMPC2TemStatus->byMpc2TempStatus, TRAP_ALARM_GENERATE);
                    }
                    else
                    {
                        Agtlog(LOG_INFORM, "[ProcMpc2TempStatusChange] Task status<%d> trap<%d> send succeed !\n",
							ptMPC2TemStatus->byMpc2TempStatus, TRAP_ALARM_GENERATE);

						OspPrintf( TRUE, FALSE, "[ProcMpc2TempStatusChange] Alarm generate!\n");        // [miaoqingsong 自测打印]
                    }
                }
            }
        }
        break;
		
	default:
        Agtlog(LOG_ERROR, "[ProcMpc2TempStatusChange] unexpected msg%d<%s> received!\n", 
			pcMsg->event, OspEventDesc(pcMsg->event));
		break;
	}

	return ;	
}

/*=============================================================================
  函 数 名： ProcMPCCpuStatusChange
  功    能： MPC板Cpu状态异常上报告警处理
  算法实现： 
  全局变量： 
  参    数： CMessage * const pcMsg
  返 回 值： void 
  ----------------------------------------------------------------------------
  修改记录：
  日   期        版本        修改人        修改内容
  2010/12/15     5.0         苗庆松          创建
=============================================================================*/
void CMsgCenterInst::ProcMPCCpuStatusChange( CMessage * const pcMsg )
{
	if ( NULL == pcMsg )
	{
		Agtlog(LOG_ERROR, "[ProcMPCCpuStatusChange] The pointer can not be Null!\n");
        return;
	}
    
	if ( !g_cCfgParse.HasSnmpNms() )
    {
        Agtlog(LOG_WARN, "[ProcMPCCpuStatusChange] There's no NMS configed !\n");
        return;
    }
    
	u8   abyAlarmObj[MAX_ALARMOBJECT_NUM];
    memset(abyAlarmObj, 0, sizeof(abyAlarmObj));
    TMcupfmAlarmEntry    tAlarmData;
    TMPCCpuStatus* ptMPCCpuStatus = NULL;

	switch(pcMsg->event)
	{
	case SVC_AGT_CPU_STATUS:
		ptMPCCpuStatus = (TMPCCpuStatus*)pcMsg->content;
        abyAlarmObj[0] = CPU_MCU_OBJECT;
        abyAlarmObj[1] = 0;
        abyAlarmObj[2] = 0;
        abyAlarmObj[3] = 0;
        abyAlarmObj[4] = 0;
		
        if( g_cAlarmProc.FindAlarm( ALARM_MCU_MPC_CPU_ABNORMAL, ALARMOBJ_MCU_MPC_CPU, abyAlarmObj, &tAlarmData ) )
        {
            if( 0 == ptMPCCpuStatus->byCpuStatus)      // 正常
            {
                if( !g_cAlarmProc.DeleteAlarm( tAlarmData.m_dwSerialNo ) )
                {
                    Agtlog(LOG_ERROR, "[ProcMPCCpuStatusChange] DeleteAlarm( AlarmCode = %lu, Object = %d, %d ) failed!\n", 
						tAlarmData.m_dwAlarmCode, tAlarmData.m_achObject[0], tAlarmData.m_achObject[1] );
                }
                else
                {
                    if (SNMP_SUCCESS !=SendPrmAlarmTrap(m_pcSnmpNodes, tAlarmData, TRAP_ALARM_RESTORE))
                    {
                        Agtlog(LOG_ERROR, "[ProcMPCCpuStatusChange] Task status<%d> trap<%d> send failed !\n",
							ptMPCCpuStatus->byCpuStatus, TRAP_ALARM_RESTORE);
                    }
                    else
                    {
                        Agtlog(LOG_INFORM, "[ProcMPCCpuStatusChange] Task status<%d> trap<%d> send succeed !\n",
							ptMPCCpuStatus->byCpuStatus, TRAP_ALARM_RESTORE);

						OspPrintf( TRUE, FALSE, "[ProcMPCCpuStatusChange] Alarm restore!\n");           // [miaoqingsong 自测打印]
                    }
                }
            }
        }
        else
        {
            if( 0 != ptMPCCpuStatus->byCpuStatus )
            {
                if( !g_cAlarmProc.AddAlarm( ALARM_MCU_MPC_CPU_ABNORMAL, ALARMOBJ_MCU_MPC_CPU, abyAlarmObj, &tAlarmData ) )
                {
                    Agtlog(LOG_ERROR, "[ProcMPCCpuStatusChange] AddAlarm( AlarmCode = %lu, Object = %d, %d ) failed!\n", 
						ALARM_MCU_MPC_CPU_ABNORMAL, abyAlarmObj[0], abyAlarmObj[1] );
                }
                else
                {   
                    if (SNMP_SUCCESS !=SendPrmAlarmTrap(m_pcSnmpNodes, tAlarmData, TRAP_ALARM_GENERATE))
                    {
                        Agtlog(LOG_ERROR, "[ProcMPCCpuStatusChange] Task status<%d> trap<%d> send failed !\n",
							ptMPCCpuStatus->byCpuStatus, TRAP_ALARM_GENERATE);
                    }
                    else
                    {
                        Agtlog(LOG_INFORM, "[ProcMPCCpuStatusChange] Task status<%d> trap<%d> send succeed !\n",
							ptMPCCpuStatus->byCpuStatus, TRAP_ALARM_GENERATE);

						OspPrintf( TRUE, FALSE, "[ProcMPCCpuStatusChange] Alarm generate!\n");        // [miaoqingsong 自测打印]
                    }
                }
            }
        }
        break;
		
	default:
        Agtlog(LOG_ERROR, "[ProcMPCCpuStatusChange] unexpected msg%d<%s> received!\n", 
			pcMsg->event, OspEventDesc(pcMsg->event));
		break;
	}
	return ;	
}

/*=============================================================================
  函 数 名： ProcPowerTempStatusChange
  功    能： 电源板温度状态异常上报告警处理
  算法实现： 
  全局变量： 
  参    数： CMessage * const pcMsg
  返 回 值： void 
  ----------------------------------------------------------------------------
  修改记录：
  日   期        版本        修改人        修改内容
  2010/12/16     5.0         苗庆松          创建
=============================================================================*/
void CMsgCenterInst::ProcPowerTempStatusChange( CMessage * const pcMsg )
{
	if ( NULL == pcMsg)
	{
		Agtlog(LOG_ERROR, "[ProcPowerTempStatusChange] The pointer can not be Null\n");
        return;
	}

	if ( !g_cCfgParse.HasSnmpNms() )
    {
        Agtlog(LOG_WARN, "[ProcPowerTempStatusChange] There's no NMS configed !\n");
        return;
    }

	u8   abyAlarmObj[MAX_ALARMOBJECT_NUM];
    memset(abyAlarmObj, 0, sizeof(abyAlarmObj));
    TMcupfmAlarmEntry    tAlarmData;
    TPowerBrdTempStatus* ptPowerBrdTempStatus = NULL;
	
	switch(pcMsg->event)
	{
	case SVC_AGT_POWERTEMP_STATUS:
		ptPowerBrdTempStatus = (TPowerBrdTempStatus*)pcMsg->content;
        abyAlarmObj[0] = ( u8 )ptPowerBrdTempStatus->bySlot;
        abyAlarmObj[1] = 0;
        abyAlarmObj[2] = 0;
        abyAlarmObj[3] = 0;
        abyAlarmObj[4] = 0;
		
        if( g_cAlarmProc.FindAlarm( ALARM_MCU_POWER_TEMP_ABNORMAL, ALARMOBJ_MCU_POWER_TEMP, abyAlarmObj, &tAlarmData ) )
        {
            if( 0 == ptPowerBrdTempStatus->byPowerBrdTempStatus )      // 正常
            {
                if( !g_cAlarmProc.DeleteAlarm( tAlarmData.m_dwSerialNo ) )
                {
                    Agtlog(LOG_ERROR, "[ProcPowerTempStatusChange] DeleteAlarm( AlarmCode = %lu, Object = %d, %d ) failed!\n", 
						tAlarmData.m_dwAlarmCode, tAlarmData.m_achObject[0], tAlarmData.m_achObject[1] );
                }
                else
                {
                    if (SNMP_SUCCESS !=SendPrmAlarmTrap(m_pcSnmpNodes, tAlarmData, TRAP_ALARM_RESTORE))
                    {
                        Agtlog(LOG_ERROR, "[ProcPowerTempStatusChange] Task status<%d> trap<%d> send failed !\n",
							ptPowerBrdTempStatus->byPowerBrdTempStatus, TRAP_ALARM_RESTORE);
                    }
                    else
                    {
                        Agtlog(LOG_INFORM, "[ProcPowerTempStatusChange] Task status<%d> trap<%d> send succeed !\n",
							ptPowerBrdTempStatus->byPowerBrdTempStatus, TRAP_ALARM_RESTORE);

						OspPrintf( TRUE, FALSE, "[ProcPowerTempStatusChange] Alarm restore!\n");           // [miaoqingsong 自测打印]
                    }
                }
            }
        }
        else
        {
            if( 0 != ptPowerBrdTempStatus->byPowerBrdTempStatus )
            {
                if( !g_cAlarmProc.AddAlarm( ALARM_MCU_POWER_TEMP_ABNORMAL, ALARMOBJ_MCU_POWER_TEMP, abyAlarmObj, &tAlarmData ) )
                {
                    Agtlog(LOG_ERROR, "[ProcPowerTempStatusChange] AddAlarm( AlarmCode = %lu, Object = %d, %d ) failed!\n", 
						ALARM_MCU_POWER_TEMP_ABNORMAL, abyAlarmObj[0], abyAlarmObj[1] );
                }
                else
                {   
                    if (SNMP_SUCCESS !=SendPrmAlarmTrap(m_pcSnmpNodes, tAlarmData, TRAP_ALARM_GENERATE))
                    {
                        Agtlog(LOG_ERROR, "[ProcPowerTempStatusChange] Task status<%d> trap<%d> send failed !\n",
							ptPowerBrdTempStatus->byPowerBrdTempStatus, TRAP_ALARM_GENERATE);
                    }
                    else
                    {
                        Agtlog(LOG_INFORM, "[ProcPowerTempStatusChange] Task status<%d> trap<%d> send succeed !\n",
							ptPowerBrdTempStatus->byPowerBrdTempStatus, TRAP_ALARM_GENERATE);

						OspPrintf( TRUE, FALSE, "[ProcPowerTempStatusChange] Alarm generate!\n");           // [miaoqingsong 自测打印]

                    }
                }
            }
        }
        break;
		
	default:
        Agtlog(LOG_ERROR, "[ProcPowerTempStatusChange] unexpected msg%d<%s> received!\n", 
			pcMsg->event, OspEventDesc(pcMsg->event));
		break;
	}
	
	return ;	
}

/*=============================================================================
  函 数 名： ProcCpuFanStatusChange
  功    能： 处理cpu风扇告警异常
  算法实现： 
  全局变量： 
  参    数： CMessage * const pcMsg
  返 回 值： void 
=============================================================================*/
void CMsgCenterInst::ProcCpuFanStatusChange( CMessage * const pcMsg )
{
	if ( NULL == pcMsg)
	{
		Agtlog(LOG_ERROR, "[ProcCpuFanStatusChange] The pointer can not be Null\n");
        return;
	}

	if ( !g_cCfgParse.HasSnmpNms() )
    {
        Agtlog(LOG_WARN, "[ProcCpuFanStatusChange] There's no NMS configed !\n");
        return;
    }

	u8   abyAlarmObj[MAX_ALARMOBJECT_NUM];
    memset(abyAlarmObj, 0, sizeof(abyAlarmObj));
    TMcupfmAlarmEntry    tAlarmData;
    u8* pbyCpuFanStatus = NULL;
	
	switch(pcMsg->event)
	{
	case SVC_AGT_CPU_FAN_STATUS:
		pbyCpuFanStatus = (u8*)pcMsg->content;        
		abyAlarmObj[0] = CPU_MCU_OBJECT;

        if( g_cAlarmProc.FindAlarm( ALARM_MCU_CPU_FAN_ABNORMAL, ALARMOBJ_MCU_CPU_FAN, abyAlarmObj, &tAlarmData ) )
        {
            if( 0 == *pbyCpuFanStatus )      // 正常
            {
                if( !g_cAlarmProc.DeleteAlarm( tAlarmData.m_dwSerialNo ) )
                {
                    Agtlog(LOG_ERROR, "[ProcCpuFanStatusChange] DeleteAlarm( AlarmCode = %lu, Object = %d, %d ) failed!\n", 
						tAlarmData.m_dwAlarmCode, tAlarmData.m_achObject[0], tAlarmData.m_achObject[1] );
                }
                else
                {
                    if (SNMP_SUCCESS !=SendPrmAlarmTrap(m_pcSnmpNodes, tAlarmData, TRAP_ALARM_RESTORE))
                    {
                        Agtlog(LOG_ERROR, "[ProcCpuFanStatusChange] Task status<%d> trap<%d> send failed !\n",
							*pbyCpuFanStatus, TRAP_ALARM_RESTORE);
                    }
                    else
                    {
                        Agtlog(LOG_INFORM, "[ProcCpuFanStatusChange] Task status<%d> trap<%d> send succeed !\n",
							*pbyCpuFanStatus, TRAP_ALARM_RESTORE);						
                    }
                }
            }
        }
        else
        {
            if( 0 != *pbyCpuFanStatus )
            {
                if( !g_cAlarmProc.AddAlarm( ALARM_MCU_CPU_FAN_ABNORMAL, ALARMOBJ_MCU_CPU_FAN, abyAlarmObj, &tAlarmData ) )
                {
                    Agtlog(LOG_ERROR, "[ProcCpuFanStatusChange] AddAlarm( AlarmCode = %lu, Object = %d, %d ) failed!\n", 
						ALARM_MCU_CPU_FAN_ABNORMAL, abyAlarmObj[0], abyAlarmObj[1] );
                }
                else
                {   
                    if (SNMP_SUCCESS !=SendPrmAlarmTrap(m_pcSnmpNodes, tAlarmData, TRAP_ALARM_GENERATE))
                    {
                        Agtlog(LOG_ERROR, "[ProcCpuFanStatusChange] Task status<%d> trap<%d> send failed !\n",
							*pbyCpuFanStatus, TRAP_ALARM_GENERATE);
                    }
                    else
                    {
                        Agtlog(LOG_INFORM, "[ProcCpuFanStatusChange] Task status<%d> trap<%d> send succeed !\n",
							*pbyCpuFanStatus, TRAP_ALARM_GENERATE);					

                    }
                }
            }
        }
        break;
		
	default:
        Agtlog(LOG_ERROR, "[ProcCpuFanStatusChange] unexpected msg%d<%s> received!\n", 
			pcMsg->event, OspEventDesc(pcMsg->event));
		break;
	}
	
	return ;
}

/*=============================================================================
  函 数 名： ProcCpuTempStatusChange
  功    能： 处理cpu温度告警异常
  算法实现： 
  全局变量： 
  参    数： CMessage * const pcMsg
  返 回 值： void 
=============================================================================*/
void CMsgCenterInst::ProcCpuTempStatusChange( CMessage * const pcMsg )
{
	if ( NULL == pcMsg)
	{
		Agtlog(LOG_ERROR, "[ProcCpuTempStatusChange] The pointer can not be Null\n");
        return;
	}

	if ( !g_cCfgParse.HasSnmpNms() )
    {
        Agtlog(LOG_WARN, "[ProcCpuTempStatusChange] There's no NMS configed !\n");
        return;
    }

	u8   abyAlarmObj[MAX_ALARMOBJECT_NUM];
    memset(abyAlarmObj, 0, sizeof(abyAlarmObj));
    TMcupfmAlarmEntry    tAlarmData;
    u8* pbyCpuTempStatus = NULL;
	
	switch(pcMsg->event)
	{
	case SVC_AGT_CPUTEMP_STATUS:
		pbyCpuTempStatus = (u8*)pcMsg->content;        
		abyAlarmObj[0] = CPU_MCU_OBJECT;

        if( g_cAlarmProc.FindAlarm( ALARM_MCU_CPU_TEMP_ABNORMAL, ALARMOBJ_MCU_CPU_TEMP, abyAlarmObj, &tAlarmData ) )
        {
            if( 0 == *pbyCpuTempStatus )      // 正常
            {
                if( !g_cAlarmProc.DeleteAlarm( tAlarmData.m_dwSerialNo ) )
                {
                    Agtlog(LOG_ERROR, "[ProcCpuTempStatusChange] DeleteAlarm( AlarmCode = %lu, Object = %d, %d ) failed!\n", 
						tAlarmData.m_dwAlarmCode, tAlarmData.m_achObject[0], tAlarmData.m_achObject[1] );
                }
                else
                {
                    if (SNMP_SUCCESS !=SendPrmAlarmTrap(m_pcSnmpNodes, tAlarmData, TRAP_ALARM_RESTORE))
                    {
                        Agtlog(LOG_ERROR, "[ProcCpuTempStatusChange] Task status<%d> trap<%d> send failed !\n",
							*pbyCpuTempStatus, TRAP_ALARM_RESTORE);
                    }
                    else
                    {
                        Agtlog(LOG_INFORM, "[ProcCpuTempStatusChange] Task status<%d> trap<%d> send succeed !\n",
							*pbyCpuTempStatus, TRAP_ALARM_RESTORE);						
                    }
                }
            }
        }
        else
        {
            if( 0 != *pbyCpuTempStatus )
            {
                if( !g_cAlarmProc.AddAlarm( ALARM_MCU_CPU_TEMP_ABNORMAL, ALARMOBJ_MCU_CPU_TEMP, abyAlarmObj, &tAlarmData ) )
                {
                    Agtlog(LOG_ERROR, "[ProcCpuTempStatusChange] AddAlarm( AlarmCode = %lu, Object = %d, %d ) failed!\n", 
						ALARM_MCU_CPU_TEMP_ABNORMAL, abyAlarmObj[0], abyAlarmObj[1] );
                }
                else
                {   
                    if (SNMP_SUCCESS !=SendPrmAlarmTrap(m_pcSnmpNodes, tAlarmData, TRAP_ALARM_GENERATE))
                    {
                        Agtlog(LOG_ERROR, "[ProcCpuTempStatusChange] Task status<%d> trap<%d> send failed !\n",
							*pbyCpuTempStatus, TRAP_ALARM_GENERATE);
                    }
                    else
                    {
                        Agtlog(LOG_INFORM, "[ProcCpuTempStatusChange] Task status<%d> trap<%d> send succeed !\n",
							*pbyCpuTempStatus, TRAP_ALARM_GENERATE);					

                    }
                }
            }
        }
        break;
		
	default:
        Agtlog(LOG_ERROR, "[ProcCpuTempStatusChange] unexpected msg%d<%s> received!\n", 
			pcMsg->event, OspEventDesc(pcMsg->event));
		break;
	}
	
	return ;
}

/*=============================================================================
  函 数 名： ProcPowerOnOffStatusChange
  功    能： 处理待机消息
  算法实现： 
  全局变量： 
  参    数： CMessage * const pcMsg
  返 回 值： void 
=============================================================================*/
void CMsgCenterInst::ProcPowerOnOffStatusChange( CMessage * const pcMsg )
{
    if( NULL == pcMsg )
    {
        Agtlog(LOG_ERROR, "[ProcPowerOnOffStatusChange] The pointer can not be Null\n");
        return;
    }
    //状态保存
    TMcuSystem tMcuSysInfo;
    g_cCfgParse.GetSystemInfo( &tMcuSysInfo );

    if ( AGT_SVC_POWEROFF == pcMsg->event )
    {
        tMcuSysInfo.SetState( MCU_STATE_STANDY );
    }
    else if ( AGT_SVC_POWERON == pcMsg->event )
    {
        tMcuSysInfo.SetState( MCU_STATE_RUNNING );
    }
    g_cCfgParse.SetSystemInfo( &tMcuSysInfo );

    //trap的发送
    if ( !g_cCfgParse.HasSnmpNms() )
    {
        Agtlog(LOG_WARN, "[ProcPowerOnOffStatusChange] There's no NMS configed !\n");
        return;
    }

	if(!m_pcSnmpNodes || !m_pcAgentSnmp)
	{
		Agtlog(LOG_WARN, "[ProcPowerOnOffStatusChange] m_pcSnmpNodes or m_pcAgentSnmp is null !\n");
        return;
	}

    switch(pcMsg->event)
	{
	case AGT_SVC_POWEROFF:     // 业务发过来的待机消息
        {
            u32 dwState = MCU_STATE_STANDY;
            m_pcSnmpNodes->Clear();
            m_pcSnmpNodes->SetTrapType( TRAP_POWEROFF );
            m_pcSnmpNodes->AddNodeValue( NODE_MCUSYSSTATE, &dwState, sizeof(dwState) );
            m_pcAgentSnmp->AgentSendTrap( *m_pcSnmpNodes );
        }

		break;

	case AGT_SVC_POWERON:    // 业务发过来的从待机到运行消息
        {
            u32 dwState = MCU_STATE_RUNNING;
            s8 achErrorStr[MAX_ERROR_NUM][MAX_ERROR_STR_LEN+1];
            memset(achErrorStr, '\0', sizeof(achErrorStr) );
			
			g_cAlarmProc.GetErrorString( (s8*)achErrorStr, sizeof(achErrorStr) );
            
            TMPCInfo tMPCInfo;
            g_cCfgParse.GetMPCInfo( &tMPCInfo );
            u32 dwLayer = (u32)tMPCInfo.GetLocalLayer();
            u32 dwSlot  = (u32)tMPCInfo.GetLocalSlot();
            m_pcSnmpNodes->Clear();
            m_pcSnmpNodes->SetTrapType( TRAP_COLD_RESTART );
            m_pcSnmpNodes->AddNodeValue( NODE_MCUSYSSTATE, &dwState, sizeof(dwState) );
            m_pcSnmpNodes->AddNodeValue( NODE_MCUSYSCONFIGERRORSTRING, achErrorStr, sizeof(achErrorStr) );
            m_pcAgentSnmp->AgentSendTrap( *m_pcSnmpNodes );
        }
		break;

    case SVC_AGT_COLD_RESTART:     // 冷启动完成
        {
            u32 dwState = MPC_COLD_START;
            s8 achErrorStr[MAX_ERROR_NUM][MAX_ERROR_STR_LEN+1];
            memset(achErrorStr, '\0', sizeof(achErrorStr) );

			g_cAlarmProc.GetErrorString( (s8*)achErrorStr, sizeof(achErrorStr) );
            m_pcSnmpNodes->Clear();
            m_pcSnmpNodes->SetTrapType( TRAP_COLD_RESTART );
            m_pcSnmpNodes->AddNodeValue( NODE_MCUSYSSTATE, &dwState, sizeof(dwState) );
            m_pcSnmpNodes->AddNodeValue( NODE_MCUSYSCONFIGERRORSTRING, achErrorStr, sizeof(achErrorStr) );
            m_pcAgentSnmp->AgentSendTrap( *m_pcSnmpNodes );
        }
        break;

    case SVC_AGT_POWEROFF:        // 关机
        {
            u32 dwState = MPC_POWER_OFF;
            m_pcSnmpNodes->Clear();
            m_pcSnmpNodes->SetTrapType(TRAP_POWEROFF);
            m_pcSnmpNodes->AddNodeValue( NODE_MCUSYSSTATE, &dwState, sizeof(dwState) );
            m_pcAgentSnmp->AgentSendTrap( *m_pcSnmpNodes );
        }
        break;
		
	default:
        Agtlog(LOG_ERROR, "[ProcPowerOnOffStatusChange] unexpected msg%d<%s> received!\n", 
                                                pcMsg->event, OspEventDesc(pcMsg->event));
		break;
	}	
	return ;
}

/*=============================================================================
  函 数 名： ProcGetConfigError
  功    能： 取配置信息错误
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void 
=============================================================================*/
void CMsgCenterInst::ProcGetConfigError()
{
    Agtlog(LOG_ERROR, "[ProcGetConfigError] Error to read Mcu config\n");

    SetTimer( SVC_AGT_CONFIG_ERROR, 3000 ); // 创建定时器
    return;
}

#ifdef _MINIMCU_
/*=============================================================================
函 数 名： ProcSetDscInfoReq
功    能： 
算法实现： 
全局变量： 
参    数： CMessage * const pcMsg
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/7/17   4.0			周广程                  创建
=============================================================================*/
void CMsgCenterInst::ProcSetDscInfoReq( CMessage * const pcMsg )
{
#ifdef _MINIMCU_
	CServMsg cMsg(pcMsg->content, pcMsg->length);

	u8 byLayer = * cMsg.GetMsgBody();
	u8 bySlot =  *( cMsg.GetMsgBody() + sizeof(u8) );
	TDSCModuleInfo tDscInfo = *(TDSCModuleInfo *)( cMsg.GetMsgBody() + sizeof(u8) + sizeof(u8) );
	TBrdPosition tBrdPos;
	tBrdPos.byBrdLayer = byLayer;
	tBrdPos.byBrdSlot = bySlot;
	tBrdPos.byBrdID = g_cCfgParse.GetConfigedDscType();

	u8 byDscType = 0xff;
	if ( g_cCfgParse.IsConfigedBrd(tBrdPos) && g_cCfgParse.IsDscReged(byDscType) )
	{
		u8 byDstInstId = g_cCfgParse.GetBrdInstIdByPos( tBrdPos );
		cMsg.SetMsgBody((u8*)&tDscInfo, sizeof(tDscInfo));
		post( MAKEIID(AID_MCU_BRDMGR, byDstInstId), MPC_BOARD_SETDSCINFO_REQ, cMsg.GetServMsg(), cMsg.GetServMsgLen() );
	}
	else
	{
		if ( SUCCESS_AGENT != g_cCfgParse.SetDSCInfo( &tDscInfo, TRUE ) )
		{
			Agtlog(LOG_ERROR, "[ProcSetDscInfo] Set dscinfo failed!\n");
			post( MAKEIID(AID_MCU_CONFIG, 1), AGT_SVC_SETDSCINFO_NACK, pcMsg->content, pcMsg->length );
		}
		post( MAKEIID(AID_MCU_CONFIG, 1), AGT_SVC_SETDSCINFO_ACK, pcMsg->content, pcMsg->length );
	}
#endif
	return;
}

/*=============================================================================
函 数 名： ProcDscRegSucceedNotif
功    能： 
算法实现： 
全局变量： 
参    数： CMessage * const pcMsg
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/7/31   4.0			周广程                  创建
=============================================================================*/
void CMsgCenterInst::ProcDscRegSucceedNotif( CMessage * const pcMsg )
{
	post( MAKEIID(AID_MCU_CONFIG, 1), pcMsg->event, pcMsg->content, pcMsg->length );
}

/*=============================================================================
函 数 名： ProcSetDscInfoRsp
功    能： 
算法实现： 
全局变量： 
参    数： CMessage * const pcMsg
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/7/17   4.0			周广程                  创建
=============================================================================*/
void CMsgCenterInst::ProcSetDscInfoRsp( CMessage * const pcMsg )
{
	Agtlog( LOG_INFORM, "[ProcSetDscInfoRsp]: %u(%s) passed!\n", pcMsg->event, ::OspEventDesc(pcMsg->event));
	CServMsg cServMsg( pcMsg->content, pcMsg->length );
	switch(pcMsg->event) 
	{
	case BOARD_MPC_SETDSCINFO_ACK:
		{
			TDSCModuleInfo tNewInfo = *(TDSCModuleInfo *)cServMsg.GetMsgBody();
			tNewInfo.Print();
			TDSCModuleInfo tOldInfo;
			g_cCfgParse.GetLastDscInfo( &tOldInfo );

			if ( SUCCESS_AGENT != g_cCfgParse.SetDSCInfo( &tNewInfo, TRUE ) )
			{
				Agtlog(LOG_ERROR, "[ProcSetDscInfo] Set dscinfo failed!\n");
				g_cCfgParse.SetDSCInfo( &tOldInfo, TRUE );
				post( MAKEIID(AID_MCU_CONFIG, 1), AGT_SVC_SETDSCINFO_NACK, pcMsg->content, pcMsg->length );
			}
			else
			{
				post( MAKEIID(AID_MCU_CONFIG, 1), AGT_SVC_SETDSCINFO_ACK );
			}
		}
		break;
	case BOARD_MPC_SETDSCINFO_NACK:
		post( MAKEIID(AID_MCU_CONFIG, 1), AGT_SVC_SETDSCINFO_NACK );
		break;
	default:
		break;
	}
}
#endif

/*=============================================================================
  函 数 名： ProcBoardStatusAlarm
  功    能： 处理外设单板告警（Led/网口状态）
  算法实现： 
  全局变量： 
  参    数： CMessage * const pcMsg
  返 回 值： void
  -----------------------------------------------------------------------------
  修改记录    ：
  日    期	  版本		修改人		走读人    修改内容
  2012/04/06  4.7		liaokang             修改 将网口状态附加到led状态之后
=============================================================================*/
void CMsgCenterInst::ProcBoardStatusAlarm( CMessage * const pcMsg )
{
    if( NULL == pcMsg )
    {
        Agtlog(LOG_ERROR, "[ProcBoardStatusAlarm] The pointer can not be Null\n");
        return;
    }

    if ( !g_cCfgParse.HasSnmpNms() )
    {
        Agtlog(LOG_WARN, "[ProcBoardStatusAlarm] There's no NMS configed !\n");
        return;
    }

    TEqpBrdCfgEntry* ptBrdCfg;
    ptBrdCfg = (TEqpBrdCfgEntry*)pcMsg->content;
    s8 achLedStatus[MAX_BOARD_LED_NUM + 1]={0};         // 最大单板灯数目为32个
    s8 achEthPortStatus[MAX_BOARD_ETHPORT_NUM + 1]={0}; // 支持IS2.2网口状态附加到led状态 [05/04/2012 liaokang]
    s8 achBuf[MAX_BOARD_LED_NUM + MAX_BOARD_ETHPORT_NUM + 1]={0};

    u16 wRow = g_cCfgParse.GetBoardRow(ptBrdCfg->GetLayer(),
                                       ptBrdCfg->GetSlot(),
                                       ptBrdCfg->GetType());
    if( ERR_AGENT_BRDNOTEXIST == wRow )
    {
        Agtlog(LOG_WARN, "[ProcBoardStatusAlarm] The board is not exist\n");
        return;
    }

	if (!m_pcSnmpNodes || !m_pcAgentSnmp)
	{
        Agtlog(LOG_WARN, "[ProcBoardStatusAlarm] m_pcSnmpNodes or m_pcAgentSnmp is null\n");
		return;
	}

    m_pcSnmpNodes->Clear();
    m_pcSnmpNodes->SetTrapType(TRAP_LED_STATE);

    u32 dwTmpData = ptBrdCfg->GetLayer();
    u32 dwNodeName = MAKE_MCUTABLE_NODENAME(wRow, NODE_MCUNETBRDCFGLAYER);
    m_pcSnmpNodes->AddNodeValue(dwNodeName, &dwTmpData, sizeof(dwTmpData) );

    dwTmpData = ptBrdCfg->GetSlot();
    dwNodeName = MAKE_MCUTABLE_NODENAME(wRow, NODE_MCUNETBRDCFGSLOT);
    m_pcSnmpNodes->AddNodeValue(dwNodeName, &dwTmpData, sizeof(dwTmpData) );
    
    dwTmpData = ptBrdCfg->GetType();
    dwNodeName = MAKE_MCUTABLE_NODENAME(wRow, NODE_MCUNETBRDCFGTYPE);
    m_pcSnmpNodes->AddNodeValue(dwNodeName, &dwTmpData, sizeof(dwTmpData) );
    
    // 支持IS2.2网口状态附加到led状态 [05/04/2012 liaokang]
    u8 byBufSize;
    u8 byLedBufSize = g_cCfgParse.PanelLedStatusAgt2NMS( *ptBrdCfg, achLedStatus);
    u8 byEthPortBufSize = g_cCfgParse.EthPortStatusAgt2NMS( *ptBrdCfg, achEthPortStatus ); 
    
    if ( byLedBufSize )
    {  
        memcpy( achBuf, achLedStatus, byLedBufSize );
    }
    else
    {
        switch ( ptBrdCfg->GetType() )
        {
        case BRD_TYPE_IS22:  
            byLedBufSize = 32;
            memset( achBuf, 2, byLedBufSize ); // IS2.2灯状态 灭
            break;
        default:
            break; 
        }
    }
        
    byBufSize = byLedBufSize;

    if ( byEthPortBufSize )
    {  
        memcpy( achBuf + byBufSize, achEthPortStatus, byEthPortBufSize );
        byBufSize += byEthPortBufSize;
    }
    else // IS2.2网口状态尚未获取到
    {
        switch ( ptBrdCfg->GetType() )
        {
        case BRD_TYPE_IS22:
            s8 achEthPort[16]; 
            memset( achEthPort, 3, sizeof(achEthPort) ); // IS2.2网口状态 未知
            achEthPort[0] = '~';        // 分隔符
            achEthPort[1] = '~';        // 分隔符
            achEthPort[2] = 1;          // 状态类型
            achEthPort[3] = 3;          // 前网口数目
            achEthPort[7] = 8;          // 后网口数目
            // 分隔符
            memcpy( achBuf + byBufSize, achEthPort, sizeof(achEthPort) );
            byBufSize += sizeof(achEthPort); 
            break;
        default:
            break; 
        } 
    }

    dwNodeName = MAKE_MCUTABLE_NODENAME(wRow, NODE_MCUNETBRDCFGPANELLED);
    m_pcSnmpNodes->AddNodeValue(dwNodeName, achBuf, byBufSize );


    dwTmpData = ptBrdCfg->GetState();
    dwNodeName = MAKE_MCUTABLE_NODENAME(wRow, NODE_MCUNETBRDCFGSTATUS);
    m_pcSnmpNodes->AddNodeValue(dwNodeName, &dwTmpData, sizeof(dwTmpData) );
    
    m_pcAgentSnmp->AgentSendTrap( *m_pcSnmpNodes );

    Agtlog(LOG_VERBOSE, "[ProcBoardStatusAlarm] Layer = %d slot = %d Type = %d Recombined led and netport = %s\n",
        ptBrdCfg->GetLayer(), ptBrdCfg->GetSlot(), ptBrdCfg->GetType(), achBuf);

    return;
}

/*=============================================================================
  函 数 名： ProcPowerOffAlarm
  功    能： 关机告警
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void 
=============================================================================*/
void CMsgCenterInst::ProcPowerOffAlarm()
{
    if ( !g_cCfgParse.HasSnmpNms() )
    {
        Agtlog(LOG_WARN, "[ProcPowerOffAlarm] There's no NMS configed !\n");
        return;
    }

	if (!m_pcSnmpNodes || !m_pcAgentSnmp)
	{
        printf("[ProcPowerOffAlarm] m_pcSnmpNodes or m_pcAgentSnmp is null\n");
		return;
	}

    u32 dwState = TRAP_POWEROFF; // 关机
    m_pcSnmpNodes->Clear();
    m_pcSnmpNodes->SetTrapType( TRAP_POWEROFF );
    m_pcSnmpNodes->AddNodeValue( NODE_MCUSYSSTATE, &dwState, sizeof(dwState) );

    Agtlog(LOG_INFORM, "[ProcPowerOffAlarm] Send alarm about power off\n");
    m_pcAgentSnmp->AgentSendTrap( *m_pcSnmpNodes );
    
    return;
}

/*=============================================================================
  函 数 名： ProcColStartdAlarm
  功    能： 处理冷启动
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void 
=============================================================================*/
void CMsgCenterInst::ProcColdStartdAlarm()
{
    if ( !g_cCfgParse.HasSnmpNms() )
    {
        Agtlog(LOG_WARN, "[ProcColdStartdAlarm] There's no NMS configed !\n");
		printf("[ProcColdStartdAlarm] There's no NMS configed !\n");
        return;
    }

	if (!m_pcSnmpNodes || !m_pcAgentSnmp)
	{
        printf("[ProcColdStartdAlarm] m_pcSnmpNodes or m_pcAgentSnmp is null\n");
		return;
	}
	
    s8 achErrorStr[MAX_ERROR_NUM][MAX_ERROR_STR_LEN+1];
    memset(achErrorStr, '\0', sizeof(achErrorStr) );
	
	u16 wErrorLen = sizeof(achErrorStr);

	BOOL32 bRet = g_cAlarmProc.GetErrorString( (s8*)achErrorStr, sizeof(achErrorStr) );
	
	if( !bRet )
	{
		wErrorLen = 0;
	}

    u32 dwState = MPC_COLD_START;

    m_pcSnmpNodes->Clear();
    m_pcSnmpNodes->SetTrapType( TRAP_COLD_RESTART );
    m_pcSnmpNodes->AddNodeValue( NODE_MCUSYSSTATE, &dwState, sizeof(u32) );
    m_pcSnmpNodes->AddNodeValue( NODE_MCUSYSCONFIGERRORSTRING, achErrorStr, wErrorLen );    
    m_pcAgentSnmp->AgentSendTrap( *m_pcSnmpNodes );

	SetTimer( MCUAGENT_UPDATE_MCUPFMINFO_TIMER, UPDATE_PFMINFO_TIME_OUT ); // 更新Mcu性能
    return;
}


/*=============================================================================
函 数 名： ProcMcsUpdateBrdCmd
功    能： mcu通知代理更新单板版本
算法实现： u8(单板索引) + u8(源文件名个数) + u8(文件名长度) + s8[](文件名)  //注：(文件名包含绝对路径)
全局变量： 
参    数： CMessage * const pcMsg
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2006/8/17  4.0			许世林                  创建
=============================================================================*/
void CMsgCenterInst::ProcMcsUpdateBrdCmd(CMessage * const pcMsg )
{
    CServMsg cServMsg(pcMsg->content, pcMsg->length);
    u8 byBrdIdx = *(u8*)cServMsg.GetMsgBody();
    u8 byRet = 1;
    
    TBoardInfo tBrdInfo;
    if ( SUCCESS_AGENT == g_cCfgParse.GetBrdCfgById(byBrdIdx, &tBrdInfo) )
    {
        Agtlog(LOG_INFORM, "[ProcMcsUpdateBrdCmd] byBrdIdx.%d, pos<%d,%d:%s>\n", 
                            byBrdIdx, tBrdInfo.GetLayer(), tBrdInfo.GetSlot(), 
                            g_cCfgParse.GetBrdTypeStr(tBrdInfo.GetType()) );
        TMPCInfo tMPCInfo;
        g_cCfgParse.GetMPCInfo( &tMPCInfo );
        Agtlog(LOG_INFORM, "[ProcMcsUpdateBrdCmd] mcu: local<%d,%d>, other<%d,%d>\n", 
                            tMPCInfo.GetLocalLayer(), tMPCInfo.GetLocalSlot(), 
                            tMPCInfo.GetOtherMpcLayer(), tMPCInfo.GetOtherMpcSlot() );

        // MPC板升级
		//  [1/21/2011 chendaiwei]支持MPC2
        if ( ( BRD_TYPE_MPC/*DSL8000_BRD_MPC*/ == tBrdInfo.GetType() || BRD_TYPE_MPC2 == tBrdInfo.GetType() )
		#ifndef _MINIMCU_
			 && 
             tMPCInfo.GetLocalLayer() == tBrdInfo.GetLayer() &&
             tMPCInfo.GetLocalSlot()  == tBrdInfo.GetSlot()
		#endif
		   )
        {
        #ifdef _LINUX_
            //linux下调用底层接口appupdate
            u8  byNum = *(u8*)(cServMsg.GetMsgBody()+sizeof(u8));
            s8 *lpMsgBody = (s8*)(cServMsg.GetMsgBody()+sizeof(u8)+sizeof(u8));
            u16 wMsgLen = cServMsg.GetMsgBodyLen() - 2;
            u8  abyResult[MAXNUM_FILE_UPDATE];
            memset( abyResult, 1, sizeof(abyResult) );
            Agtlog(LOG_INFORM, "[ProcMcsUpdateBrdCmd] update num: %d, len: %d !\n", byNum, wMsgLen);

            
            if ( !g_cCfgParse.UpdateMpcSoftware(lpMsgBody, wMsgLen, byNum, abyResult) )
                Agtlog( LOG_ERROR, "[ProcMcsUpdateBrdCmd] UpdateMpcSoftWare failed !\n");
            
            s32 nIndex = 0;
            for( ; nIndex < byNum; nIndex ++ )
            {
                Agtlog( LOG_INFORM, "[ProcMcsUpdateBrdCmd] Recv mpc result: %d\n", abyResult[nIndex] );
            }
            
            cServMsg.SetMsgBody(&byBrdIdx, sizeof(u8));
            cServMsg.CatMsgBody(&byNum, sizeof(u8));
            cServMsg.CatMsgBody(abyResult, byNum);
            
            u8 abyStrBuf[256];
            memset(abyStrBuf, 0, sizeof(abyStrBuf));
            ChangeNumBufToStrBuf(cServMsg.GetMsgBody(), byNum + 2, abyStrBuf);
            cServMsg.SetMsgBody(abyStrBuf, byNum + 2);
        #endif
            SendMsgToMcuCfg(AGT_SVC_UPDATEBRDVERSION_NOTIF, cServMsg.GetServMsg(), 
                                                        cServMsg.GetServMsgLen());
        }
        // 其他单板升级
        else
        {
            //[zw][09022008]主备mcu不响应升级对方的命令 
			//  [1/21/2011 chendaiwei]支持MPC2
            if ( BRD_TYPE_MPC/*DSL8000_BRD_MPC*/ == tBrdInfo.GetType() || BRD_TYPE_MPC2 == tBrdInfo.GetType())
            {
                Agtlog(LOG_INFORM, "byBrdIdx.%d, pos<%d,%d:%s> update, ignore it due to MPC\n", 
                                   byBrdIdx, tBrdInfo.GetLayer(), tBrdInfo.GetSlot(), 
                                   g_cCfgParse.GetBrdTypeStr(tBrdInfo.GetType()) );
                return;
            }

            TBrdPosition tBrdPos;
            tBrdPos.byBrdID = tBrdInfo.GetType();
            tBrdPos.byBrdLayer = tBrdInfo.GetLayer();
            tBrdPos.byBrdSlot = tBrdInfo.GetSlot();
            
            // 追加一个字段，标识升级发起者
            cServMsg.SetChnIndex(UPDATE_MCS);
            g_cCfgParse.UpdateBrdSoftware(tBrdPos, cServMsg);
        }
    }
    else
    {
        Agtlog(LOG_ERROR, "[ProcMcsUpdateBrdCmd] get TBoardInfo for byBrdIdx.%d failed.\n", byBrdIdx);
        
    #ifndef WIN32
        u8 byNum = *(cServMsg.GetMsgBody() + sizeof(u8));
        u8 abyRet[MAXNUM_FILE_UPDATE];
        memset(abyRet, 0, sizeof(abyRet));
        cServMsg.SetMsgBody(&byBrdIdx, sizeof(u8));
        cServMsg.CatMsgBody(&byNum,    sizeof(u8));
        cServMsg.CatMsgBody(abyRet, byNum);
        u8 abyStrBuf[256];
        memset(abyStrBuf, 0, sizeof(abyStrBuf));
        ChangeNumBufToStrBuf( cServMsg.GetMsgBody(), byNum + 2, abyStrBuf);
        cServMsg.SetMsgBody( abyStrBuf, byNum + 2 );
    #endif
        SendMsgToMcuCfg(AGT_SVC_UPDATEBRDVERSION_NOTIF, cServMsg.GetServMsg(), 
                                                    cServMsg.GetServMsgLen());
    }

    return;
}

#ifdef _MINIMCU_
/*=============================================================================
函 数 名： ProcMcsUpdateDscMsg
功    能： 
算法实现： 
全局变量： 
参    数： CMessage * const pcMsg
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/8/20   4.0			周广程                  创建
=============================================================================*/
void CMsgCenterInst::ProcMcsUpdateDscMsg( CMessage * const pcMsg )
{
	if ( NULL == pcMsg )
	{
		return;
	}

	CServMsg cMsg(pcMsg->content, pcMsg->length);

	u8 byBrdIdx = *cMsg.GetMsgBody();
	TBoardInfo tBrdInfo;
	TBrdPosition tBrdPos;
	memset(&tBrdPos,0,sizeof(tBrdPos));

    if ( SUCCESS_AGENT == g_cCfgParse.GetBrdCfgById(byBrdIdx, &tBrdInfo) )
	{	
        tBrdPos.byBrdID = tBrdInfo.GetType();
        tBrdPos.byBrdLayer = tBrdInfo.GetLayer();
        tBrdPos.byBrdSlot = tBrdInfo.GetSlot();
	}
	switch(pcMsg->event)
	{
	case SVC_AGT_STARTUPDATEDSCVERSION_REQ:
		g_cCfgParse.PostMsgToBrdMgr( tBrdPos, MPC_DSC_STARTUPDATE_SOFTWARE_REQ, cMsg.GetServMsg(), cMsg.GetServMsgLen() );
		break;
	case SVC_AGT_DSCUPDATEFILE_REQ:
		g_cCfgParse.PostMsgToBrdMgr( tBrdPos, MPC_DSC_UPDATEFILE_REQ, cMsg.GetServMsg(), cMsg.GetServMsgLen() );
		break;
	default:
		break;
	}
	return;
}
#endif

/*=============================================================================
函 数 名： ProcNMSUpdateBrdCmd
功    能： 处理 NMS 升级 MPC 
算法实现： u8(单板索引) + u8(源文件名个数) + u8(文件名长度) + s8[](文件名)  //注：(文件名包含绝对路径)
全局变量： 
参    数： CMessage * const pcMsg
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2006/9/22   4.0			张宝卿                  创建
=============================================================================*/
void CMsgCenterInst::ProcNMSUpdateBrdCmd( CMessage * const pcMsg )
{
    TBrdPosition tBrdPos;
    memcpy(&tBrdPos, pcMsg->content, sizeof(tBrdPos));
    u8  byNum       = *(pcMsg->content + sizeof(tBrdPos));
    u8 *pbyMsgBody  = pcMsg->content + sizeof(tBrdPos) + sizeof(u8);
    u16 wMsgBodyLen = pcMsg->length - sizeof(tBrdPos) - sizeof(u8);
    u8  byBrdIdx    = g_cCfgParse.GetBrdId(tBrdPos);

    Agtlog(LOG_INFORM, "[ProcNMSUpdateBrdCmd]brd<%d,%d:%s> to be update !\n",
                        tBrdPos.byBrdLayer, tBrdPos.byBrdSlot,
                        g_cCfgParse.GetBrdTypeStr(tBrdPos.byBrdID) );
    
    // 非MPC 升级, 由单板代理处理
	//  [1/21/2011 chendaiwei]支持MPC2
    if ( BRD_TYPE_MPC/*DSL8000_BRD_MPC*/ != tBrdPos.byBrdID && BRD_TYPE_MPC2 != tBrdPos.byBrdID ) 
    {
        CServMsg cServMsg;
        cServMsg.SetChnIndex( UPDATE_NMS );         //该字段作特殊标识，升级的发起者为网管
        cServMsg.SetMsgBody(&byBrdIdx, sizeof(u8));
        cServMsg.CatMsgBody(&byNum, sizeof(byNum));
        cServMsg.CatMsgBody(pbyMsgBody, wMsgBodyLen); 
        g_cCfgParse.UpdateBrdSoftware(tBrdPos, cServMsg);
        
        Agtlog(LOG_INFORM, "[ProcNMSUpdateBrdCmd] not mpc update!\n");

        return;
    }

    // 在本地升级MPC板,或者升级备份mcu 
    TBoardInfo tBrdInfo;
    if ( SUCCESS_AGENT == g_cCfgParse.GetBrdCfgById(byBrdIdx, &tBrdInfo) )
    {
    #ifndef _MINIMCU_
        TMPCInfo tMPCInfo;
        g_cCfgParse.GetMPCInfo( &tMPCInfo );

        Agtlog(LOG_INFORM, "[ProcNMSUpdateBrdCmd]other mcu<%d,%d>, local mcu<%d,%d>\n", tMPCInfo.GetOtherMpcLayer(), tMPCInfo.GetOtherMpcSlot(),
               tMPCInfo.GetLocalLayer(), tMPCInfo.GetLocalSlot() );
		//  [1/21/2011 chendaiwei]支持MPC2
        if ( (BRD_TYPE_MPC/*DSL8000_BRD_MPC*/ == tBrdInfo.GetType() || BRD_TYPE_MPC2 == tBrdInfo.GetType())
          && tMPCInfo.GetOtherMpcLayer() == tBrdInfo.GetLayer() 
          && tMPCInfo.GetOtherMpcSlot()  == tBrdInfo.GetSlot()
          && tMPCInfo.GetOtherMpcLayer() != 0
          && tMPCInfo.GetOtherMpcSlot()  != 0 )
        {
            OspPost( MAKEIID(AID_MCU_MSMANAGERSSN, 1), EV_AGENT_MCU_MCU_UPDATE, pcMsg->content, pcMsg->length );
            Agtlog(LOG_INFORM, "[ProcNMSUpdateBrdCmd] EV_AGENT_MCU_MCU_UPDATE post here!\n");

            return;
        }
    #endif 
    #ifndef WIN32
        // linux下调用底层接口appupdate, VX下改名
        u8  abyResult[MAXNUM_FILE_UPDATE];
        memset( abyResult, 1, sizeof(abyResult) );
        Agtlog(LOG_INFORM, "[ProcNMSUpdateBrdCmd] update num: %d, len: %d !\n", byNum, wMsgBodyLen);

        if ( !g_cCfgParse.UpdateMpcSoftware((s8*)pbyMsgBody, wMsgBodyLen, byNum, abyResult, TRUE) )
            Agtlog( LOG_ERROR, "[ProcNMSUpdateBrdCmd] UpdateMpcSoftWare failed !\n");

        s32 nIndex = 0;
        for( ; nIndex < byNum; nIndex ++ )
        {
            Agtlog( LOG_INFORM, "[ProcNMSUpdateBrdCmd] Recv mpc result: %d\n", abyResult[nIndex] );
        }           
    #endif
    }
    else
    {
        Agtlog(LOG_ERROR, "[ProcNMSUpdateBrdCmd] get TBoardInfo for byBrdIdx.%d failed.\n", byBrdIdx);
    }
    return;
}

/*=============================================================================
函 数 名： ProcSmUpdateBrdCmd
功    能： 处理SnmpManager升级 MPC 
算法实现：
全局变量： 
参    数： CMessage * const pcMsg
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2010/3/18   5.0			陆昆朋                  创建
=============================================================================*/
void CMsgCenterInst::ProcSmUpdateBrdCmd( void )
{
    TBrdPosition tBrdPos;

	TMcuUpdateInfo* ptUpdateInfo = g_cCfgParse.GetMcuUpdateInfo();

	tBrdPos.byBrdID = ptUpdateInfo->GetType();
	tBrdPos.byBrdLayer = ptUpdateInfo->GetLayerID();
	tBrdPos.byBrdSlot = ptUpdateInfo->GetSlotID();

    Agtlog(LOG_INFORM, "[ProcSmUpdateBrdCmd]brd<%d,%d:%s> to be update !\n",
                        tBrdPos.byBrdLayer, tBrdPos.byBrdSlot,
                        g_cCfgParse.GetBrdTypeStr(tBrdPos.byBrdID) );
    
    // 非MPC 升级, 由单板代理处理
	//  [1/21/2011 chendaiwei]支持MPC2
    if ( BRD_TYPE_MPC/*DSL8000_BRD_MPC*/ != tBrdPos.byBrdID && BRD_TYPE_MPC2 != tBrdPos.byBrdID ) 
    {
        Agtlog(LOG_INFORM, "[ProcSmUpdateBrdCmd] not mpc update!\n");
		ptUpdateInfo->SetUpdateState(SNMP_FILE_UPDATE_FAILED);
        return;
    }

    // 在本地升级MPC板,或者升级备份mcu
	u8  byBrdIdx    = g_cCfgParse.GetBrdId(tBrdPos);

    TBoardInfo tBrdInfo;
    if ( SUCCESS_AGENT == g_cCfgParse.GetBrdCfgById(byBrdIdx, &tBrdInfo) )
    {
    #ifndef _MINIMCU_
        TMPCInfo tMPCInfo;
        g_cCfgParse.GetMPCInfo( &tMPCInfo );

        Agtlog(LOG_INFORM, "[ProcSmUpdateBrdCmd]other mcu<%d,%d>, local mcu<%d,%d>\n", tMPCInfo.GetOtherMpcLayer(), tMPCInfo.GetOtherMpcSlot(),
               tMPCInfo.GetLocalLayer(), tMPCInfo.GetLocalSlot() );

		//  [1/21/2011 chendaiwei]支持MPC2
        if ( (BRD_TYPE_MPC/*DSL8000_BRD_MPC*/ == tBrdInfo.GetType() || BRD_TYPE_MPC2 == tBrdInfo.GetType())
          && tMPCInfo.GetOtherMpcLayer() == tBrdInfo.GetLayer() 
          && tMPCInfo.GetOtherMpcSlot()  == tBrdInfo.GetSlot()
          && tMPCInfo.GetOtherMpcLayer() != 0
          && tMPCInfo.GetOtherMpcSlot()  != 0 )
        {
			//暂时不支持备份mcu的升级
			Agtlog(LOG_WARN, "[ProcSmUpdateBrdCmd] 暂时不支持备份mcu的升级!\n");
            //OspPost( MAKEIID(AID_MCU_MSMANAGERSSN, 1), EV_AGENT_MCU_MCU_UPDATE, pcMsg->content, pcMsg->length );
            //Agtlog(LOG_INFORM, "[ProcSmUpdateBrdCmd] EV_AGENT_MCU_MCU_UPDATE post here!\n");

            return;
        }
    #endif 
    #ifndef WIN32
        // linux下调用底层接口appupdate, VX下改名
        if ( !g_cCfgParse.SmUpdateMpcSoftware() )
            Agtlog( LOG_ERROR, "[ProcSmUpdateBrdCmd] SmUpdateMpcSoftware failed !\n");
   
    #endif
    }
    else
    {
        Agtlog(LOG_ERROR, "[ProcSmUpdateBrdCmd] get TBoardInfo for byBrdIdx.%d failed.\n", byBrdIdx);
    }
    return;
}

/*=============================================================================
函 数 名： ProcMsgToBrdManager
功    能： 
算法实现： 
全局变量： 
参    数： CMessage * const pcMsg
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/8/28   4.0			周广程                  创建
=============================================================================*/
void CMsgCenterInst::ProcMsgToBrdManager( CMessage * const pcMsg )
{
	if ( NULL == pcMsg )
	{
		Agtlog(LOG_ERROR, "[ProcMsgToBrdManager] Msg is NULL!\n");
		return;
	}

	CServMsg cServMsg(pcMsg->content, pcMsg->length);
	u8 byBrdLayer = 0;
	u8 byBrdSlot = 0;
	memcpy(&byBrdLayer, cServMsg.GetMsgBody(), sizeof(u8));
	memcpy(&byBrdSlot, cServMsg.GetMsgBody()+sizeof(u8), sizeof(u8));
	TBrdPosition tBrdPos;
	tBrdPos.byBrdLayer = byBrdLayer;
	tBrdPos.byBrdSlot = byBrdSlot;
	u16 wMsgEvent = 0;
	BOOL32 IsSendMsg2BrdMgr = TRUE;
	switch(pcMsg->event)
	{
	case SVC_AGT_DISCONNECTBRD_CMD:
		{
			memcpy(&tBrdPos.byBrdID, cServMsg.GetMsgBody()+sizeof(u8)*2, sizeof(u8));
			if ( !g_cBrdManagerApp.IsRegedBoard(tBrdPos) )
			{
				IsSendMsg2BrdMgr = FALSE;
			}
			wMsgEvent = MPC_BOARD_DISCONNECT_CMD;
		}
		break;

#ifndef _MINIMCU_
	case SVC_AGT_BOARDCFGMODIFY_NOTIF:
		wMsgEvent = MCU_BOARD_CONFIGMODIFY_NOTIF;
		break;
#endif

#ifdef _MINIMCU_
	case SVC_AGT_DSCTELNETLOGININFO_UPDATE_CMD:
		wMsgEvent = MCU_BOARD_DSCTELNETLOGININFO_UPDATE_CMD;
		break;
	case SVC_AGT_DSCGKINFO_UPDATE_CMD:
		wMsgEvent = MPC_BOARD_DSCGKINFO_UPDATE_CMD;
		break;
#endif

	default:
		break;
	}
	if ( IsSendMsg2BrdMgr )
	{
		g_cCfgParse.PostMsgToBrdMgr( tBrdPos, wMsgEvent, cServMsg.GetServMsg(), cServMsg.GetServMsgLen() );
	}
}

/*=============================================================================
  函 数 名： GetBoardLedStatus
  功    能： 取相应单板灯的状态
  算法实现： 
  全局变量： 
  参    数： u8 byBrdType
             TEqpBrdCfgEntry& tBrdCfg
             u8* pszLedStr
  返 回 值： u8 
=============================================================================*/
u8 CMsgCenterInst::GetBoardLedStatus(u8 byBrdType, TEqpBrdCfgEntry& tBrdCfg, u8* pszLedStr)
{
    if(NULL == pszLedStr)
    {
        return 0;
    }
    u8 byLedCount = 0;

    switch( byBrdType )
	{
	case BRD_TYPE_MPC/*DSL8000_BRD_MPC*/:
		memcpy(pszLedStr, tBrdCfg.GetPanelLed(), 9 );     //mpc有9个面板灯
        byLedCount = 9;
	    break;
	//[2011/01/25 zhushz]IS2.x单板支持
	case BRD_TYPE_IS21:
		memcpy(pszLedStr, tBrdCfg.GetPanelLed(), 29 );     //IS2.1有29个面板灯
        byLedCount = 29;
	    break;
	case BRD_TYPE_IS22:
		memcpy(pszLedStr, tBrdCfg.GetPanelLed(), 32 );     //IS2.2有32个面板灯
        byLedCount = 32;
	    break;				
	//  [1/21/2011 chendaiwei]支持MPC2
	case BRD_TYPE_MPC2/*DSL8000_BRD_MP2C*/:
		memcpy(pszLedStr, tBrdCfg.GetPanelLed(), 8 );     //mpc2有8个面板灯
        byLedCount = 8;
	    break;
					
	case BRD_TYPE_CRI/*DSL8000_BRD_CRI*/:
		memcpy(pszLedStr, tBrdCfg.GetPanelLed(), 10 );    //cir有10个面板灯	
        byLedCount = 10;
	    break;
		
	//  [1/21/2011 chendaiwei]CRI2支持
	case BRD_TYPE_CRI2:
		memcpy(pszLedStr, tBrdCfg.GetPanelLed(), 14 );    //cri2有14个面板灯	
        byLedCount = 14;
	    break;

	//  [1/21/2011 chendaiwei]DRI2支持
	case BRD_TYPE_DRI2:
		memcpy(pszLedStr, tBrdCfg.GetPanelLed(), 12 );    //dri2有12个面板灯	
        byLedCount = 12;
	    break;
					
	case BRD_TYPE_MMP/*DSL8000_BRD_MMP*/:
	case BRD_TYPE_VPU/*DSL8000_BRD_VPU*/:
	case BRD_TYPE_DEC5/*DSL8000_BRD_DEC5*/:
	    memcpy(pszLedStr, tBrdCfg.GetPanelLed(), 10 );    //mpc有10个面板灯
        byLedCount = 10;
	    break;
					
	case BRD_TYPE_DRI/*DSL8000_BRD_DRI*/:
	case BRD_TYPE_DSI/*DSL8000_BRD_DSI*/:
		memcpy(pszLedStr, tBrdCfg.GetPanelLed(), 12 );    //mpc有12个面板灯
        byLedCount = 12;
	    break;

	case BRD_TYPE_16E1/*DSL8000_BRD_16E1*/:
		memcpy(pszLedStr, tBrdCfg.GetPanelLed(), 20 );    //DRI16有20个面板灯
        byLedCount = 20;
	    break;
		
	case BRD_TYPE_VAS/*DSL8000_BRD_VAS*/:
		memcpy(pszLedStr, tBrdCfg.GetPanelLed(), 4 );     //mpc有4个面板灯
        byLedCount = 4;
		break;
					
	case BRD_TYPE_IMT/*DSL8000_BRD_IMT*/:
	case BRD_TYPE_APU/*DSL8000_BRD_APU*/:
		memcpy(pszLedStr, tBrdCfg.GetPanelLed(), 8 );     //mpc有8个面板灯	
        byLedCount = 8;
	    break;
								
	case BRD_TYPE_DTI/*DSL8000_BRD_DTI*/:   //not use now
	case BRD_TYPE_DIC/*DSL8000_BRD_DIC*/:	//not use now
        
	default:
        byLedCount = strlen(tBrdCfg.GetPanelLed());
		memcpy(pszLedStr, tBrdCfg.GetPanelLed(), byLedCount);
	    break;
    }
    return byLedCount;
}

//发送消息给mcu配置任务
void SendMsgToMcuCfg( u16 wEvent, const u8 * pcMsg, u16 wLen )
{
    ::OspPost( MAKEIID( AID_MCU_CONFIG, 1), wEvent, pcMsg, wLen);
    return;
}

/*=============================================================================
  函 数 名： SendPrmAlarmTrap
  功    能： 发送pfm告警trap
  算法实现： 
  全局变量： 
  参    数： CNodes * pcNodes:              要发送的节点对象
             TMcupfmAlarmEntry tAlarmEntry: 告警数据包结构
             u8 byTrapType:                 trap类型
  返 回 值： u16 
 -----------------------------------------------------------------------------
  修改记录:
  日  期      版本        修改人      修改内容
  2006/07/28  4.0         张宝卿       创建
=============================================================================*/
/*lint -save -esym(429, pcNodes)*/
u16 CMsgCenterInst::SendPrmAlarmTrap( CNodes          * pcNodes, 
                                      TMcupfmAlarmEntry tAlarmEntry, 
                                      u8                byTrapType )
{
    u16 wRetVal = SNMP_GENERAL_ERROR;

    wRetVal = pcNodes->Clear();
    if (SNMP_SUCCESS != wRetVal) 
    {
        Agtlog(LOG_ERROR, "[SendPrmAlarmTrap] m_pcNodes clear failed !\n");
        return wRetVal;
    }

    wRetVal = pcNodes->SetTrapType(byTrapType);
    if (SNMP_SUCCESS != wRetVal) 
    {
        Agtlog(LOG_ERROR, "[SendPrmAlarmTrap] m_pcNodes set trap type failed !\n");
        return wRetVal;
    }

    wRetVal = pcNodes->AddNodeValue(NODE_MCUPFMALARMSERIALNO, 
                                    &tAlarmEntry.m_dwSerialNo, 
                                    sizeof(tAlarmEntry.m_dwSerialNo));
    if (SNMP_SUCCESS != wRetVal) 
    {
        Agtlog(LOG_ERROR, "[SendPrmAlarmTrap] m_pcNodes add node(0x%x) failed !\n", 
                                                         NODE_MCUPFMALARMSERIALNO);
        return wRetVal;
    }
    
    wRetVal = pcNodes->AddNodeValue(NODE_MCUPFMALARMALARMCODE, 
                                    &tAlarmEntry.m_dwAlarmCode,
                                    sizeof(tAlarmEntry.m_dwAlarmCode));
    if (SNMP_SUCCESS != wRetVal) 
    {
        Agtlog(LOG_ERROR, "[SendPrmAlarmTrap] m_pcNodes add node(0x%x) failed !\n", 
                                                        NODE_MCUPFMALARMALARMCODE);
        return wRetVal;
    }
	u32 dwObjType = tAlarmEntry.m_byObjType;
	
    wRetVal = pcNodes->AddNodeValue(NODE_MCUPFMALARMOBJTYPE, 
									&dwObjType, 
                                    sizeof(dwObjType));

    if (SNMP_SUCCESS != wRetVal) 
    {
        Agtlog(LOG_ERROR, "[SendPrmAlarmTrap] m_pcNodes add node(0x%x) failed !\n", 
                                                          NODE_MCUPFMALARMOBJTYPE);
        return wRetVal;
    }

    wRetVal = pcNodes->AddNodeValue(NODE_MCUPFMALARMOBJECT, 
                                    tAlarmEntry.m_achObject, 
                                    sizeof(tAlarmEntry.m_achObject));
    if (SNMP_SUCCESS != wRetVal) 
    {
        Agtlog(LOG_ERROR, "[SendPrmAlarmTrap] m_pcNodes add node(0x%x) failed !\n", 
                                                           NODE_MCUPFMALARMOBJECT);
        return wRetVal;
    }

    wRetVal = pcNodes->AddNodeValue(NODE_MCUPFMALARMTIME, 
                                    tAlarmEntry.m_achTime, 
                                    sizeof(tAlarmEntry.m_achTime));
    if (SNMP_SUCCESS != wRetVal) 
    {
        Agtlog(LOG_ERROR, "[SendPrmAlarmTrap] m_pcNodes add node(0x%x) failed !\n", 
                                                             NODE_MCUPFMALARMTIME);
        return wRetVal;
    }

	if (!m_pcAgentSnmp)
	{
		return SNMP_GENERAL_ERROR;
	}

    wRetVal = m_pcAgentSnmp->AgentSendTrap( *pcNodes );
    if (SNMP_SUCCESS != wRetVal) 
    {
        Agtlog(LOG_ERROR, "[SendPrmAlarmTrap] AgentSendTrap failed !\n");
        return wRetVal;
    }

    return wRetVal;
}

/*=============================================================================
  函 数 名： ChangeNumBufToStrBuf
  功    能： 发送pfm告警trap
  算法实现： 
  全局变量： 
  参    数： u8* pNumBuf:  传入的数据 buf
             u8 byBufLen:  长度
             u8* pStrBuf:  传出的字符串 buf
  返 回 值： BOOL32 
 -----------------------------------------------------------------------------
  修改记录:
  日  期      版本        修改人      修改内容
  2006/09/14  4.0         张宝卿       创建
=============================================================================*/
BOOL32 ChangeNumBufToStrBuf( u8* pNumBuf, u8 byBufLen, u8* pStrBuf )
{
    if ( NULL == pNumBuf || NULL == pStrBuf || 0 == byBufLen )
    {
        Agtlog( LOG_ERROR, "[ChangeNumBufToStrBuf] param error: pIn<0x%x>pOut<0x%x>len<%d>\n",
                            pNumBuf, pStrBuf, byBufLen );
        return FALSE;
    }
    s32 nIndex = 0;
    for( ; nIndex < byBufLen; nIndex ++ )
    {
        Agtlog(LOG_INFORM, "[ChangeNumBufToStrBuf] %d.num: %d to be change\n",
                            nIndex, pNumBuf[nIndex]);
        pStrBuf[nIndex] = pNumBuf[nIndex] + '0';
    }
    pStrBuf[nIndex] = '\0';
    
    return TRUE;
}

void ProcCbFtpcCallbackFun(TFtpcCallback *PTParam, void *pCbd)
{
	TUpdateFileInfo* ptLastUpdateFile = (TUpdateFileInfo *)pCbd;

	if (ptLastUpdateFile == NULL)
	{
		Agtlog(LOG_ERROR, "ProcCbFtpcCallbackFun 获取回调函数失败\n");
		return;
	}

	if (PTParam == NULL)
	{
		Agtlog(LOG_ERROR, "ProcCbFtpcCallbackFun 获取函数参数失败\n");
		return;
	}

	ptLastUpdateFile->ChgUpdateState(PTParam->dwMsgType);

	TMcuUpdateInfo* ptUpdateInfo = g_cCfgParse.GetMcuUpdateInfo();

	if (PTParam->dwMsgType == FTPC_FINISHED)
	{
		Agtlog(LOG_INFORM, "FtpcCallbackFun Msg: %ld, Percent:%ld, File:%s FTPC_FINISHED\n", 
		PTParam->dwMsgType, PTParam->UMsgInfo.dwTransPercent, ptLastUpdateFile->GetSvrFilePath());

		if (ptUpdateInfo->IsFtpEnd())
		{
			//通知mcu升级
			OspPost( MAKEIID(AID_MCU_AGENT, 1), EV_SM_UPDATEDMPC_CMD);
		}
		else
		{
			if (!ptUpdateInfo->FtpGet())
			{
				Agtlog(LOG_ERROR, "ProcCbFtpcCallbackFun FtpGet Fail\n");
				ptUpdateInfo->SetUpdateState(SNMP_FILE_UPDATE_FAILED);
			}
		}
	}
	else
	{
		ptLastUpdateFile->ChgTransPercent(PTParam->UMsgInfo.dwTransPercent);

		if (PTParam->dwMsgType != FTPC_TRANSPERCENT)
		{
			Agtlog(LOG_ERROR, "ProcCbFtpcCallbackFun Error\n");
			ptUpdateInfo->SetUpdateState(SNMP_FILE_UPDATE_FAILED);
		}

		Agtlog(LOG_VERBOSE, "FtpcCallbackFun Msg: %ld, Percent:%ld, File:%s\n", 
		PTParam->dwMsgType, PTParam->UMsgInfo.dwTransPercent, ptLastUpdateFile->GetSvrFilePath());
	}


}

//[5/16/2013 liaokang] 放于此,等待后续snmpadp支持上去
// Snmp是否为UTF8编码
BOOL32 g_bSnmpIsUtf8 = FALSE; // 默认为GBK
void SetSnmpIsUtf8(BOOL32 bIsUtf8)
{ 
    g_bSnmpIsUtf8 = bIsUtf8; 
}
BOOL32 GetSnmpIsUtf8() 
{ 
    return g_bSnmpIsUtf8;
}
//end

/*====================================================================
    函数名      : TransEncodingForNmsData
    功能        : 对网管数据进行针对性转码
    算法实现    :
    引用全局变量:
    输入参数说明: 
    返回值说明  : False 没有进行转码操作
                  TURE  进行了转码操作
----------------------------------------------------------------------
    修改记录    :
    日  期      版本        修改人      修改内容
    2013/05/16              liaokang      创建
====================================================================*/
/*lint -save -e715*/
BOOL32 TransEncodingForNmsData( const s8 *pSrc, s8 *pDst, u32 dwLen, BOOL32 bSet)
{
#ifndef _UTF8
    return FALSE; // 老版本返回FALSE，不进行转码操作
#else
    if ( !pSrc || !pDst )
    {
		return FALSE;
	}
   
    BOOL32 bNmsIsUtf8 = GetSnmpIsUtf8();
    if( bNmsIsUtf8 )
    {
        return FALSE;  // 不需要进行转码
    }

    if( bSet )
    {
        gb2312_to_utf8(pSrc, pDst, dwLen-1);
    }
    else
    {
        utf8_to_gb2312(pSrc, pDst, dwLen-1);
    }    
    
    return TRUE;
#endif
}
/*lint -restore*/

// END OF FILE
