/*****************************************************************************
   模块名      : BAS
   文件名      : basInst.cpp
   相关文件    : basInst.h
   文件实现功能: BAS基类实现
   作者        : zsh
   版本        : V1.0  Copyright(C) 2001-2002 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   2003/11/10  2.0         zsh         创建
   2005/01/12  3.6         libo        增加加密
   2006/03/21  4.0         张宝卿      代码优化
******************************************************************************/
#include "basinst.h"
#include "evmcueqp.h"
#include "evbas.h"
#include "baserr.h"
#include "boardagent.h"
#include "mcuver.h"

s8	BASCFG_FILE[KDV_MAX_PATH] = {0};
s8	SECTION_BASSYS[] = "EQPBAS";
s8    SECTION_BASDEBUG[] = "EQPDEBUG";

u8      g_byDstTypeA = MEDIA_TYPE_NULL;
u8      g_byDstTypeV = MEDIA_TYPE_NULL;
s32     g_nbaslog    = 0;

CBasApp g_cBasApp;

/*lint -save -e438 -e530*/
void baslog( s8* pszFmt, ...)
{
    if(g_nbaslog == 1)
    {
		s8 achPrintBuf[255] = {0};
		s32  nBufLen = 0;
		va_list argptr;
        nBufLen = sprintf(achPrintBuf, "[Bas]: ");
        va_start(argptr, pszFmt);
        vsprintf(achPrintBuf + nBufLen, pszFmt, argptr);
        va_end(argptr); 
        OspPrintf(TRUE, FALSE, achPrintBuf);
    }
}
/*lint -restore*/

/*=============================================================================
  函 数 名： InitalData
  功    能： 
  算法实现： 
  全局变量： 
  参    数： void
  返 回 值： void 
=============================================================================*/
void CBasInst::InitalData(void)
{
    u8 byLop = 0;
    for(byLop = 0; byLop < MAXNUM_BASGRP; byLop++)
    {
        m_atAdpInfo[byLop].byGrpIdx = 255; ;
	    m_atAdpInfo[byLop].m_wLocalBindPort = 0;
	    m_atAdpInfo[byLop].m_wLocalRcvPort  = 0;
	  
        m_atAdpInfo[byLop].byNextChnNum = 0;
        m_atAdpInfo[byLop].byCanUseChnNum = 0;   
        m_atAdpInfo[byLop].byAudioOrVideoGrp = 0;
        memset(m_atAdpInfo[byLop].abyChnInfo, 0, sizeof(m_atAdpInfo[byLop].abyChnInfo));
    }

    for(byLop = 0; byLop < MAX_CHANNELS_BAS; byLop++)
    {
        memset(&m_atChnInfo[byLop], 0, sizeof(m_atChnInfo[byLop]) );
        m_atChnInfo[byLop].m_tChnStatus.SetStatus(u8(TBasChnStatus::IDLE));
	    m_atChnInfo[byLop].m_tChnStatus.SetAudType(u8(TAdaptParam::aNONE));
	    m_atChnInfo[byLop].m_tChnStatus.SetVidType(u8(TAdaptParam::vNONE));
        m_atChnInfo[byLop].bIsUsed = FALSE;
    }      
    m_byAudioSN = 0;
    m_byVideoSN = 0;
    
    m_byNextChnIdx = 0;
    m_byAudioChnNum = 0;
    m_byNextAudioChnIdx = 0;
    m_byAudioStartChnIdx = 0;
    m_byAudioGrpNum = 0;
    m_byAudioGrpStartIdx = 0;
    m_byNextAudioGrpIdx = 0;
    
    m_byVideoChnNum = 0;
    m_byVideoStartChnIdx = 0;
    m_byNextVideoChnIdx = 0;
    m_byVideoGrpNum = 0;
    m_byVideoGrpStartIdx = 0;
    m_byNextVideoGrpIdx = 0;
    
    m_byAllGrpNum = 0;
    m_byAllChnNum = 0;

    sprintf(BASCFG_FILE, "%s/mcueqp.ini", DIR_CONFIG);

	//
	m_appId = INVALID_APP;
	m_instId = INVALID_INS;
	memset(m_alias, 0, sizeof(m_alias));
	m_aliasLen = 0;
	m_maxAliasLen = 0;
	m_curState = 0;
	m_lastState = 0;
	m_tSemTimerList = 0;
	memset(&m_timerInfoListHead, 0, sizeof(TInstTimerInfo));
}

//构造
CBasInst::CBasInst()
{	
    InitalData();
}

CBasInst::~CBasInst()
{
}


/*====================================================================
	函数  : DaemonInstanceEntry
	功能  : Daemon实例消息入口
	输入  : 无
	输出  : 无
	返回  : 无
	注    :
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
====================================================================*/
void CBasInst::DaemonInstanceEntry(CMessage* const pMsg, CApp* pcApp)
{
    //CBasInst* pcInst = NULL;
    //u8* pbyMsgBody = NULL;

    CServMsg cServMsg(pMsg->content, pMsg->length);
    //pbyMsgBody = (u8*)cServMsg.GetMsgBody();

    u8 byChnIdx = cServMsg.GetChnIndex();
    
    switch(pMsg->event)
    {
    //断链检测:
    case MCU_EQP_ROUNDTRIP_REQ:
        SendMsgToMcu(EQP_MCU_ROUNDTRIP_ACK, cServMsg);
        break;

	//MCU 通知会话 重联MPCB 相应实例, zgc, 2007/04/30
	case MCU_EQP_RECONNECTB_CMD:
		//DaemonProcReconnectBCmd( pMsg, pcApp );
		break;
        
    //适配器上电初始化
    case EV_BAS_INI:
		DaemonProcInit(pMsg, pcApp);
		break;

    // 建链
    case EV_BAS_CONNECT:
        ProcConnectTimeOut(TRUE);
        break;
	// 连接Mcu
    case EV_BAS_CONNECTB:
        ProcConnectTimeOut(FALSE);
        break;

    // 注册消息
    case EV_BAS_REGISTER:  	
        ProcRegisterTimeOut(TRUE);
        break;
	// 注册
    case EV_BAS_REGISTERB:  	
        ProcRegisterTimeOut(FALSE);
        break;

    // MCU 注册应答消息
    case MCU_BAS_REG_ACK:
        MsgRegAckProc(pMsg, pcApp);
        break;

    // MCU拒绝本混音器注册
    case MCU_BAS_REG_NACK:
        MsgRegNackProc(pMsg);
        break;

    // OSP 断链消息
    case OSP_DISCONNECT:
        MsgDisconnectProc(pMsg, pcApp);
        break;

    // 开始适配
    case MCU_BAS_STARTADAPT_REQ:
        MsgStartAdptProc(byChnIdx, pMsg);
        break;

    // 停止适配
    case MCU_BAS_STOPADAPT_REQ:
        MsgStopAdptProc(byChnIdx, pMsg);
        break;

    // 设置适配参数
    case MCU_BAS_SETADAPTPARAM_CMD:
        MsgAdjustAdptProc(byChnIdx, pMsg);
        break;

    // 请求编一个关键帧
    case MCU_BAS_FASTUPDATEPIC_CMD:
        MsgFastUpdatePicProc(byChnIdx);
        break;
        
    // 设置Qos值
    case MCU_EQP_SETQOS_CMD:
        ProcSetQosMsg(pMsg);
        break;
		
	// 请求关键帧
    case EV_BAS_NEEDIFRAME_TIMER:
        MsgTimerNeedIFrameProc();
        break;

	// 取主备倒换状态
	case EV_BAS_GETMSSTATUS:
	case MCU_EQP_GETMSSTATUS_ACK:
		DeamonProcGetMsStatusRsp(pMsg);
		break;

	// 显示适配状态    
    case EV_BAS_SHOW:
        StatusShow(pcApp);
        break;

	// 显示适配通道信息
	case EV_BAS_SHOWINFO:
		BasInfo();
		break;

    default:
        break;
    }
    return;
}

/*====================================================================
	函数  : InstanceEntry
	功能  : 普通实例消息入口
	输入  : 无
	输出  : 无
	返回  : 无
	注    :
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
====================================================================*/
void CBasInst::InstanceEntry(CMessage* const pMsg)
{
    return;
}

/*=============================================================================
  函 数 名： DaemonProcInit
  功    能： 初始化处理
  算法实现： 
  全局变量： 
  参    数： CMessage* const pMsg
             CApp* pcApp
  返 回 值： void 
=============================================================================*/
void CBasInst::DaemonProcInit(CMessage* const pMsg, CApp* pcApp)
{
	if (!Init(pMsg, pcApp)) // 具体处理初始化
    {
        OspPrintf(TRUE, FALSE, "[Bas]: BAS inital failed!\n");
        return;
    }

    // guzh [4/9/2007] 获取码率输出作弊
    g_cBasApp.ReadDebugValues();

    ProcEmbed();
	return;
}

/*====================================================================
	函数  : Init
	功能  : 初始化
	输入  : 无
	输出  : 无
	返回  : 无
	注    :
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
====================================================================*/
int CBasInst::Init(CMessage* const pMsg, CApp* pcApp)
{
    u16 wRet = MAP_OPERATE_SUCCESS;
    TAdapterMap tMap;
    memset(&g_cBasApp.m_tCfg, 0, sizeof(g_cBasApp.m_tCfg));
    memcpy(&g_cBasApp.m_tCfg, pMsg->content, sizeof(g_cBasApp.m_tCfg));

    u8 byLop = 0;
	u8 byLop2 = 0;//zhouguangcheng 10-26 add for creat audio group

	g_cBasApp.m_tEqp.SetMcuEqp((u8)g_cBasApp.m_tCfg.wMcuId, g_cBasApp.m_tCfg.byEqpId, g_cBasApp.m_tCfg.byEqpType);	// 保存Mcu信息
	g_cBasApp.m_bEmbedMcu  = g_cBasApp.m_tCfg.dwConnectIP  == g_cBasApp.m_tCfg.dwLocalIP ? TRUE : FALSE;  // 是否嵌入在MCU.A
    g_cBasApp.m_bEmbedMcuB = g_cBasApp.m_tCfg.dwConnectIpB == g_cBasApp.m_tCfg.dwLocalIP ? TRUE : FALSE;  // 是否嵌入在MCU.B

    m_byAudioGrpNum = 1;    // 目前只支持一个音频适配组
    m_byAudioGrpStartIdx = 0;
    m_byNextAudioGrpIdx = 1;
    m_byNextAudioChnIdx = 1;
    m_byAudioChnNum = 3;    // 音频通道数3个
    
	baslog("The map num is %d\n", g_cBasApp.m_tCfg.wMAPCount);
	
    for(byLop = 0; byLop < m_byAudioGrpNum; byLop++) // 音频组
    {
        // 音频适配组
	    tMap.byMapId   = g_cBasApp.m_tCfg.m_atMap[byLop].byMapId;
	    tMap.dwCoreSpd = g_cBasApp.m_tCfg.m_atMap[byLop].dwCoreSpd;
	    tMap.dwMemSize = g_cBasApp.m_tCfg.m_atMap[byLop].dwMemSize;
	    tMap.dwMemSpd  = g_cBasApp.m_tCfg.m_atMap[byLop].dwMemSpd;
	    
        if (INVALID_MAP_ID > tMap.byMapId)     // 255 非法的Map，表示不需要音频适配
        {            
            wRet = g_cBasApp.m_cAptGrp[byLop].CreateGroup(&tMap, 0, m_byAudioChnNum);
            if (MAP_OPERATE_SUCCESS != wRet)
	        {
		        baslog("Create Adpt Grp.%d failed (%d)!.\n", byLop, wRet);
		        return FALSE;
	        }		
            baslog("Create Aud Grp %d at Map.%d with CoreSpd=%d, MemSize=%d, MemSpd=%d\n",
		                 byLop, tMap.byMapId, tMap.dwCoreSpd, tMap.dwMemSize, tMap.dwMemSpd);
            
            m_atAdpInfo[byLop].bIsUsed = FALSE;
            m_atAdpInfo[byLop].byNextChnNum = 0;

            // 每个音频组可以支持三个音频适配通道, 从0 开始
            m_atAdpInfo[byLop].AddOneChannel(0);
            m_atAdpInfo[byLop].AddOneChannel(1);
            m_atAdpInfo[byLop].AddOneChannel(2);
            m_atAdpInfo[byLop].byCanUseChnNum = m_byAudioChnNum;  
            
            m_atAdpInfo[byLop].byAudioOrVideoGrp = 1; // 音频(冗余信息)
            m_atAdpInfo[byLop].byGrpIdx = byLop;    
            m_atAdpInfo[byLop].byGrpType = TYPE_AUDIO;
            
			// 初始化每个通道状态
            for(byLop2 = m_byNextChnIdx; byLop2 < m_byAudioChnNum; byLop2++) 
            {
                m_atChnInfo[byLop2].m_tChnStatus.SetStatus(u8(TBasChnStatus::IDLE));
                m_atChnInfo[byLop2].m_tChnStatus.SetVidType(MEDIA_TYPE_NULL);
                m_atChnInfo[byLop2].m_tChnStatus.SetAudType(MEDIA_TYPE_NULL);
                m_atChnInfo[byLop2].m_tChnStatus.SetChannelType(BAS_CHAN_AUDIO);
                m_atChnInfo[byLop2].byChnType = TYPE_AUDIO;

                baslog("The Chn %d, Audio: %d, video: %d Type: %d\n", byLop2, 
                        m_atChnInfo[byLop2].m_tChnStatus.GetAudType(), 
                        m_atChnInfo[byLop2].m_tChnStatus.GetVidType(),
                        m_atChnInfo[byLop2].byChnType );                    
            }
            m_byNextChnIdx += m_byAudioChnNum;
        }
        else
        {
            m_byAudioGrpNum = 0;
            m_byAudioChnNum = 0;
			g_cBasApp.m_tCfg.wMAPCount = g_cBasApp.m_tCfg.wMAPCount - 1;  // Map 数减一

        }
    }

    baslog("The map num is %d\n", g_cBasApp.m_tCfg.wMAPCount);

    m_byVideoGrpNum = (u8)(g_cBasApp.m_tCfg.wMAPCount - m_byAudioGrpNum) / 2 ;    // 视频适配组数目(一个视频适配组需要二块Map)
    m_byVideoGrpStartIdx = m_byAudioGrpNum;               // 视频适配组起始序号  
    m_byVideoStartChnIdx = m_byAudioChnNum;               // 视频通道在音频通道后                      
    m_byNextVideoChnIdx  = m_byVideoStartChnIdx;
    m_byAllGrpNum = m_byVideoGrpNum + m_byAudioGrpNum;

	// 视频适配组

    for( byLop = m_byAudioGrpNum; byLop < m_byAllGrpNum; byLop++)
    {
        // 视频
        u8 byIndex = byLop;
		tMap.byMapId   = g_cBasApp.m_tCfg.m_atMap[byIndex].byMapId; // Map信息
        if( byIndex > 1	|| INVALID_MAP_ID == tMap.byMapId ) // 每个视频适配组需要两个Map, Map数跳1. 
        {													// MapId 为255时,不开启音适配, 跳过
            byIndex = byIndex + 1;
        }

		tMap.byMapId   = g_cBasApp.m_tCfg.m_atMap[byIndex].byMapId; // Map信息
	    tMap.dwCoreSpd = g_cBasApp.m_tCfg.m_atMap[byIndex].dwCoreSpd;
	    tMap.dwMemSize = g_cBasApp.m_tCfg.m_atMap[byIndex].dwMemSize;
	    tMap.dwMemSpd  = g_cBasApp.m_tCfg.m_atMap[byIndex].dwMemSpd;

	    
	    wRet = g_cBasApp.m_cAptGrp[byLop].CreateGroup(&tMap, MAX_VIDEO_ADAPT_CHANNUM, 0); //创建适配组
	    if (MAP_OPERATE_SUCCESS != wRet)
	    {
		    baslog("Create Adpt Grp.%d failed (%d)!.\n", byLop, wRet);
		    return FALSE;
	    }
	    baslog("Create Vid Grp %d at Map.%d with CoreSpd=%d ,MemSize=%d ,MemSpd=%d\n",
		        byLop, tMap.byMapId, tMap.dwCoreSpd, tMap.dwMemSize, tMap.dwMemSpd);

        m_byVideoChnNum++;
        m_atAdpInfo[byLop].bIsUsed = FALSE;
        m_atAdpInfo[byLop].byGrpType = TYPE_VIDEO;
        baslog("the video start channel is: %d\n", m_byNextChnIdx);
		m_atAdpInfo[byIndex].AddOneChannel( m_byNextChnIdx );

        m_atChnInfo[m_byNextChnIdx].bIsUsed = FALSE;
        m_atChnInfo[m_byNextChnIdx].m_tChnStatus.SetStatus(u8(TBasChnStatus::IDLE));
        m_atChnInfo[m_byNextChnIdx].m_tChnStatus.SetAudType(MEDIA_TYPE_NULL);
        m_atChnInfo[m_byNextChnIdx].m_tChnStatus.SetVidType(MEDIA_TYPE_NULL);
        m_atChnInfo[m_byNextChnIdx].m_tChnStatus.SetChannelType(BAS_CHAN_VIDEO);
        m_atChnInfo[m_byNextChnIdx].byChnType = TYPE_VIDEO;

        baslog("The Chn %d, Audio: %d, video: %d, type: %d\n", m_byNextChnIdx, 
                m_atChnInfo[m_byNextChnIdx].m_tChnStatus.GetAudType(), 
                m_atChnInfo[m_byNextChnIdx].m_tChnStatus.GetVidType(),
                m_atChnInfo[m_byNextChnIdx].byChnType);
        
        m_atAdpInfo[byLop].byCanUseChnNum++;
        m_atAdpInfo[byLop].byGrpIdx = byLop;
  
        m_atAdpInfo[byLop].byAudioOrVideoGrp = 2; // 视频适配组
        m_atAdpInfo[byLop].byGrpType = TYPE_VIDEO;

        m_byNextChnIdx ++;
    }

    m_byAllGrpNum = m_byAudioGrpNum + m_byVideoGrpNum;  // 适配组数
    m_byAllChnNum = m_byAudioChnNum + m_byVideoChnNum;  // 通道数

    baslog("The total adp group num: %d\n", m_byAllGrpNum);
    baslog("audio: %d, video: %d, total channel num: %d\n", m_byAudioChnNum, m_byVideoChnNum, m_byAllChnNum);
	return TRUE;

}

/*=============================================================================
  函 数 名： ProcEmbed
  功    能： 处理是否嵌入在Mcu中
  算法实现： 
  全局变量： 
  参    数： void
  返 回 值： void 
=============================================================================*/
void CBasInst::ProcEmbed(void)
{
    if (TRUE == g_cBasApp.m_bEmbedMcu)
    {
        baslog( "the bas embed in the mcu\n");
        g_cBasApp.m_dwMcuNode = 0;                          //内嵌时，将节点号置为0就可以
        SetTimer(EV_BAS_REGISTER, BAS_REGISTER_TIMEOUT );   //直接注册
    }

    if( TRUE == g_cBasApp.m_bEmbedMcuB)
    {
        baslog("the bas embed in Mcu B\n");
        g_cBasApp.m_dwMcuNodeB = 0;
        SetTimer(EV_BAS_REGISTERB, BAS_REGISTER_TIMEOUT );  //直接注册
    } 

    if(0 != g_cBasApp.m_tCfg.dwConnectIP && FALSE == g_cBasApp.m_bEmbedMcu)
    {
        SetTimer(EV_BAS_CONNECT, BAS_CONNETC_TIMEOUT );     //3s后再建链
    }
    if(0 != g_cBasApp.m_tCfg.dwConnectIpB && FALSE == g_cBasApp.m_bEmbedMcuB)
    {
        SetTimer(EV_BAS_CONNECTB, BAS_CONNETC_TIMEOUT );    //3s后再建链
    }

    return;
}

/*=============================================================================
  函 数 名： ProcConnectTimeOut
  功    能： 处理注册超时
  算法实现： 
  全局变量： 
  参    数： BOOL32 bIsConnectA
  返 回 值： void 
=============================================================================*/
void CBasInst::ProcConnectTimeOut(BOOL32 bIsConnectA)
{
    BOOL32 bRet = FALSE;
    if(TRUE == bIsConnectA)
    {
        bRet = ConnectMcu(bIsConnectA, g_cBasApp.m_dwMcuNode);
        if(TRUE == bRet)
        {  
            SetTimer(EV_BAS_REGISTER, BAS_REGISTER_TIMEOUT ); 
        }
        else
        {
            SetTimer(EV_BAS_CONNECT, BAS_CONNETC_TIMEOUT);   //3s后再建链
        }
    }
    else
    {
        bRet = ConnectMcu(bIsConnectA, g_cBasApp.m_dwMcuNodeB);
        if(TRUE == bRet)
        { 
            SetTimer(EV_BAS_REGISTERB, BAS_REGISTER_TIMEOUT ); 
        }
        else
        {
            SetTimer(EV_BAS_CONNECTB, BAS_CONNETC_TIMEOUT);   //3s后再建链
        }
    }
    return;
}
/*====================================================================
	函数  : ConnectMcu
	功能  : 与MCU建链
	输入  : 无
	输出  : 无
	返回  : 无
	注    :
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
====================================================================*/
BOOL32 CBasInst::ConnectMcu(BOOL32 bIsConnectA, u32& dwMcuNode)
{
    BOOL32 bRet = TRUE;
    if(!OspIsValidTcpNode(dwMcuNode)) // 过滤不合法节点
    {
        if(TRUE == bIsConnectA)
        {  
            dwMcuNode = BrdGetDstMcuNode();  
        }
        else
        {  
            dwMcuNode = BrdGetDstMcuNodeB(); 
        }

	    if (::OspIsValidTcpNode(dwMcuNode))
	    {
		    baslog("Connect Mcu%d Success, node: %d!\n", 
                                    g_cBasApp.m_tCfg.wMcuId, dwMcuNode);
		    ::OspNodeDiscCBRegQ(dwMcuNode, GetAppID(), CInstance::DAEMON);// 断链消息处理
	    }
	    else 
	    {
		    baslog("Connect to Mcu failed, retry after 10s! (A)\n");
            bRet = FALSE;
	    }
    }
    return bRet;
}

/*=============================================================================
  函 数 名： ProcRegisterTimeOut
  功    能： 处理注册超时
  算法实现： 
  全局变量： 
  参    数： BOOL32 bIsRegiterA
  返 回 值： void 
=============================================================================*/
void CBasInst::ProcRegisterTimeOut(BOOL32 bIsRegiterA)
{
    if(TRUE == bIsRegiterA)
    {
        Register(bIsRegiterA, g_cBasApp.m_dwMcuNode);
        SetTimer(EV_BAS_REGISTER, BAS_REGISTER_TIMEOUT);  //直接注册
    }
    else
    {
        Register(bIsRegiterA, g_cBasApp.m_dwMcuNodeB);
        SetTimer(EV_BAS_REGISTERB, BAS_REGISTER_TIMEOUT);  //直接注册
    }
    return;
}
/*====================================================================
	函数  : Register
	功能  : 向MCU注册
	输入  : 无
	输出  : 无
	返回  : 无
	注    :
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
====================================================================*/
void CBasInst::Register(BOOL32 bIsRegiterA, u32 dwMcuNode)
{
    CServMsg       cSvrMsg;
    TPeriEqpRegReq tReg;

    tReg.SetEqpAlias(g_cBasApp.m_tCfg.achAlias);
    tReg.SetChnnlNum(g_cBasApp.m_byChnNum);
    tReg.SetPeriEqpIpAddr(g_cBasApp.m_tCfg.dwLocalIP);
    tReg.SetStartPort(g_cBasApp.m_tCfg.wRcvStartPort);
    tReg.SetVersion(DEVVER_BAS);

    if(TRUE == bIsRegiterA)
    {
        tReg.SetMcuEqp((u8)g_cBasApp.m_tCfg.wMcuId, g_cBasApp.m_tCfg.byEqpId, g_cBasApp.m_tCfg.byEqpType);// 以后两个McuId有可能不一致
    }
    else
    {
        tReg.SetMcuEqp((u8)g_cBasApp.m_tCfg.wMcuId, g_cBasApp.m_tCfg.byEqpId, g_cBasApp.m_tCfg.byEqpType); // Mcu Id 
    }

    cSvrMsg.SetMsgBody((u8*)&tReg, sizeof(tReg));
        
    post(MAKEIID(AID_MCU_PERIEQPSSN, g_cBasApp.m_tCfg.byEqpId),
            BAS_MCU_REG_REQ,
            cSvrMsg.GetServMsg(),
            cSvrMsg.GetServMsgLen(),
            dwMcuNode);
	return;
}

/*====================================================================
	函数  : MsgRegAckProc 
	功能  : MCU应答混音器注册消息处理函数
	输入  : 无
	输出  : 无
	返回  : 无
	注    :
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
====================================================================*/
void CBasInst::MsgRegAckProc(CMessage* const pMsg, CApp* pcApp)
{
	if(NULL == pMsg)
    {
        baslog("Recv Reg Ack Msg, but the msg's pointer is Null\n");
        return;
    }

    u8 byChnIdx = 0;
	TPeriEqpStatus tEqpStatus;
	TPeriEqpRegAck* ptRegAck = NULL;

	CServMsg cServMsg(pMsg->content, pMsg->length);
	ptRegAck =(TPeriEqpRegAck*)cServMsg.GetMsgBody();
    TPrsTimeSpan tPrsTimeSpan = *(TPrsTimeSpan*)(cServMsg.GetMsgBody() + sizeof(TPeriEqpRegAck));
	// 获得MTU size, zgc, 2007-04-02
	u16 wMTUSize = *(u16*)( cServMsg.GetMsgBody() + sizeof(TPeriEqpRegAck) + sizeof(TPrsTimeSpan) );
	wMTUSize = ntohs( wMTUSize );
	
    if(pMsg->srcnode == g_cBasApp.m_dwMcuNode) // 注册应答
    {
        g_cBasApp.m_dwMcuRcvIp       = ptRegAck->GetMcuIpAddr();
	    g_cBasApp.m_wMcuRcvStartPort = ptRegAck->GetMcuStartPort();
	    g_cBasApp.m_dwMcuIId = pMsg->srcid;
        g_cBasApp.m_byRegAckNum++;
        KillTimer(EV_BAS_REGISTER);// 关闭定时器
        baslog("we register success to mcu master\n");

        tEqpStatus.SetMcuEqp((u8)g_cBasApp.m_tCfg.wMcuId, g_cBasApp.m_tCfg.byEqpId, g_cBasApp.m_tCfg.byEqpType);	

    }
    else if(pMsg->srcnode == g_cBasApp.m_dwMcuNodeB)
    {
        g_cBasApp.m_dwMcuRcvIpB = ptRegAck->GetMcuIpAddr();
        g_cBasApp.m_wMcuRcvStartPortB = ptRegAck->GetMcuStartPort();
        g_cBasApp.m_dwMcuIIdB = pMsg->srcid;
        g_cBasApp.m_byRegAckNum++;
        baslog("we regitster success to mcu slave\n");
        KillTimer(EV_BAS_REGISTERB);
        tEqpStatus.SetMcuEqp((u8)g_cBasApp.m_tCfg.wMcuId, g_cBasApp.m_tCfg.byEqpId, g_cBasApp.m_tCfg.byEqpType);	

    }

	// guzh [6/12/2007] 校验会话参数
    if ( g_cBasApp.m_dwMpcSSrc == 0 )
    {
        g_cBasApp.m_dwMpcSSrc = ptRegAck->GetMSSsrc();
    }
    else
    {
        // 异常情况，断开两个节点
        if ( g_cBasApp.m_dwMpcSSrc != ptRegAck->GetMSSsrc() )
        {
            OspPrintf(TRUE, FALSE, "[MsgRegAckProc] MPC SSRC ERROR(%u<-->%u), Disconnect Both Nodes!\n", 
                      g_cBasApp.m_dwMpcSSrc, ptRegAck->GetMSSsrc());
            if ( OspIsValidTcpNode(g_cBasApp.m_dwMcuNode) )
            {
                OspDisconnectTcpNode(g_cBasApp.m_dwMcuNode);
            }
            if ( OspIsValidTcpNode(g_cBasApp.m_dwMcuNodeB) )
            {
                OspDisconnectTcpNode(g_cBasApp.m_dwMcuNodeB);
            }      
            return;
        }
    }

	// 上报通道状态
	for (byChnIdx = 0; byChnIdx <= m_byAllChnNum; byChnIdx++)
	{
		tEqpStatus.m_tStatus.tBas.tChnnl[byChnIdx] = m_atChnInfo[byChnIdx].m_tChnStatus;
	}

	tEqpStatus.m_tStatus.tBas.byChnNum = m_byAllChnNum;
	tEqpStatus.m_byOnline = (u8)TRUE;	
	tEqpStatus.SetAlias(g_cBasApp.m_tCfg.achAlias);

    if (INVALID_MAP_ID > g_cBasApp.m_tCfg.m_atMap[0].byMapId) // 是否支持音频适配
    {
        tEqpStatus.m_tStatus.tBas.tBasCapSet.SetAudioCapSet(TRUE);
    }
    else
    {
        tEqpStatus.m_tStatus.tBas.tBasCapSet.SetAudioCapSet(FALSE);
    }

	// 默认支持视频适配
    tEqpStatus.m_tStatus.tBas.tBasCapSet.SetVideoCapSet(TRUE);

    if( m_byVideoChnNum >= 2)   // 有两个视频通道时才支持码率适配
    {
        tEqpStatus.m_tStatus.tBas.tBasCapSet.SetBitrateCapSet(TRUE);
    }
    else
    {
        tEqpStatus.m_tStatus.tBas.tBasCapSet.SetBitrateCapSet(FALSE);
    }

	cServMsg.SetMsgBody((u8*)&tEqpStatus, sizeof(tEqpStatus));
	SendMsgToMcu(BAS_MCU_BASSTATUS_NOTIF, cServMsg);

    if(FIRST_REGACK == g_cBasApp.m_byRegAckNum) // 第一次收到注册成功消息
    {
        g_cBasApp.m_tPrsTimeSpan = tPrsTimeSpan;
        baslog("The Bas's Prs para: first : %d, second: %d, three: %d, reject: %d", 
                g_cBasApp.m_tPrsTimeSpan.m_wFirstTimeSpan,
                g_cBasApp.m_tPrsTimeSpan.m_wSecondTimeSpan,
                g_cBasApp.m_tPrsTimeSpan.m_wThirdTimeSpan,
                g_cBasApp.m_tPrsTimeSpan.m_wRejectTimeSpan);
		
		// 设置MTU size, zgc, 2007-04-02
		g_cBasApp.m_wMTUSize = wMTUSize;
		baslog( "[RegAck]The net MTU is : %d\n", g_cBasApp.m_wMTUSize );
		//SetVideoSendPacketLen( (s32)g_cBasApp.m_wMTUSize );

        for(byChnIdx = 0; byChnIdx < m_byAllChnNum; byChnIdx++)         
        {
            m_atChnInfo[byChnIdx].m_tChnStatus.SetStatus(u8(TBasChnStatus::READY)); // 置为Ready
        }
        SetTimer(EV_BAS_NEEDIFRAME_TIMER, CHECK_IFRAME_INTERVAL);   // 定时查询适配组是否需要关键帧
        NEXTSTATE(u32(NORMAL)); // DAEMON 实例进入 NORMAL 状态

    }
    for(byChnIdx = 0; byChnIdx < m_byAllChnNum; byChnIdx++)         
    {
        SendChnNotif(byChnIdx);
    }
    
	baslog("Recv Reg Ack Msg, Register Mcu%d  success !\n", g_cBasApp.m_tCfg.wMcuId);
    return;
}

/*====================================================================
	函数  : MsgRegNackProc
	功能  : MCU 拒绝混音器注册消息处理函数(目前的策略是被拒后继续注册)
	输入  : 无
	输出  : 无
	返回  : 无
	注    :
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
====================================================================*/
void CBasInst::MsgRegNackProc(CMessage* const pMsg)
{
	if( pMsg->srcnode == g_cBasApp.m_dwMcuNodeB)
    {
        baslog("Bas registe be refused by B's Mcu%d .\n", g_cBasApp.m_tCfg.wMcuId);
    }
    if( pMsg->srcnode == g_cBasApp.m_dwMcuNode)    
    {
        baslog("Bas registe be refused by A's Mcu%d .\n", g_cBasApp.m_tCfg.wMcuId);
    }
    return;
}

/*====================================================================
	函数  : MsgDisconnectProc
	功能  : Osp断链消息处理函数
	输入  : 无
	输出  : 无
	返回  : 无
	注    :
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
	05/04/11	4.0			liuhuiyun	  创建
====================================================================*/
void CBasInst::MsgDisconnectProc(CMessage* const pMsg, CApp* pcApp)
{

    CServMsg cServMsg;
    if( 0 == m_byAllChnNum || NULL == pMsg || NULL == pcApp)    // 没有适配组
    {
        baslog("No Adapt Group or teh message's pointer is Null\n");
        return;
    }

    u32 dwNode = *(u32*)pMsg->content;

    if (INVALID_NODE != dwNode)
    {
        OspDisconnectTcpNode(dwNode);
    } 
    if(dwNode == g_cBasApp.m_dwMcuNode) // 断链
    {
        g_cBasApp.FreeStatusDataA();
        SetTimer(EV_BAS_CONNECT, BAS_CONNETC_TIMEOUT);
    }
    else if(dwNode == g_cBasApp.m_dwMcuNodeB)
    {
        g_cBasApp.FreeStatusDataB();
        SetTimer(EV_BAS_CONNECTB, BAS_CONNETC_TIMEOUT);
    }
	// 与其中一个断链后，向Mcu取主备倒换状态，判断是否成功
	if (INVALID_NODE != g_cBasApp.m_dwMcuNode || INVALID_NODE != g_cBasApp.m_dwMcuNodeB)
	{
		if (OspIsValidTcpNode(g_cBasApp.m_dwMcuNode))
		{
			post( g_cBasApp.m_dwMcuIId, EQP_MCU_GETMSSTATUS_REQ, NULL, 0, g_cBasApp.m_dwMcuNode );           
			baslog("[MsgDisconnectProc] GetMsStatusReq. McuNode.A\n");
		}
		else if (OspIsValidTcpNode(g_cBasApp.m_dwMcuNodeB))
		{
			post( g_cBasApp.m_dwMcuIIdB, EQP_MCU_GETMSSTATUS_REQ, NULL, 0, g_cBasApp.m_dwMcuNodeB );        
			baslog("[DaemonProcMcuDisconnect] GetMsStatusReq. McuNode.B\n");
		}

		SetTimer(EV_BAS_GETMSSTATUS, WAITING_MSSTATUS_TIMEOUT);
		return;
	}
	// 两个节点都断，清空状态信息
    if(INVALID_NODE == g_cBasApp.m_dwMcuNode && INVALID_NODE == g_cBasApp.m_dwMcuNodeB)
    {
        ClearInstStatus();
    }
    return;
}

/*=============================================================================
函 数 名： DaemonProcReconnectBCmd
功    能： 
算法实现： 
全局变量： 
参    数：  CMessage* const pMsg
           CApp* pcApp
返 回 值： void  
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/4/30   4.0			周广程                  创建
=============================================================================*/
void  CBasInst::DaemonProcReconnectBCmd( CMessage* const pMsg, CApp* pcApp )
{
	if( pMsg == NULL )
	{
		return;
	}
	CServMsg cServMsg( pMsg->content, pMsg->length );
	u32 dwMpcBIp = *(u32*)cServMsg.GetMsgBody();
	dwMpcBIp = ntohl(dwMpcBIp);
	g_cBasApp.m_tCfg.dwConnectIpB = dwMpcBIp;
	
	// 如果已连接, 先断开
	if( OspIsValidTcpNode( g_cBasApp.m_dwMcuNodeB ) )
	{
		OspDisconnectTcpNode( g_cBasApp.m_dwMcuNodeB );
	}

	g_cBasApp.FreeStatusDataB();
    if( 0 != g_cBasApp.m_tCfg.dwConnectIpB && !g_cBasApp.m_bEmbedMcuB )
    {
        SetTimer(EV_BAS_CONNECTB, BAS_CONNETC_TIMEOUT );    //3s后再建链
    }
	else
	{
		SetTimer(EV_BAS_REGISTERB, BAS_REGISTER_TIMEOUT );
	}
}

/*=============================================================================
  函 数 名： DeamonProcGetMsStatus
  功    能： 处理取主备倒换状态
  算法实现： 
  全局变量： 
  参    数： CMessage* const pMsg
  返 回 值： void 
=============================================================================*/
void CBasInst::DeamonProcGetMsStatusRsp(CMessage* const pMsg)
{
	BOOL bSwitchOk = FALSE;
	CServMsg cServMsg(pMsg->content, pMsg->length);
	if( MCU_EQP_GETMSSTATUS_ACK == pMsg->event )
	{
		TMcuMsStatus *ptMsStatus = (TMcuMsStatus *)cServMsg.GetMsgBody();
        
        KillTimer(EV_BAS_GETMSSTATUS);
        baslog("[DeamonProcGetMsStatus]. receive msg MCU_EQP_GETMSSTATUS_ACK. IsMsSwitchOK :%d\n", 
                ptMsStatus->IsMsSwitchOK());

        if(ptMsStatus->IsMsSwitchOK()) // 倒换成功
        {
            bSwitchOk = TRUE;
        }
	}

	// 倒换失败或者超时
	if( !bSwitchOk )
	{
		//20110914 zjl 超时后先清状态 再断链 再连接
		ClearInstStatus();
		baslog("[DeamonProcGetMsStatusRsp] ClearInstStatus!\n");

		if ( OspIsValidTcpNode(g_cBasApp.m_dwMcuNode ))
		{
			baslog("[DeamonProcGetMsStatusRsp] OspDisconnectTcpNode A!\n");
			OspDisconnectTcpNode(g_cBasApp.m_dwMcuNode );
		}
		if ( OspIsValidTcpNode(g_cBasApp.m_dwMcuNodeB))
		{
			baslog("[DeamonProcGetMsStatusRsp] OspDisconnectTcpNode B!\n");
			OspDisconnectTcpNode(g_cBasApp.m_dwMcuNodeB);
		}

		if( INVALID_NODE == g_cBasApp.m_dwMcuNode)// 有可能产生重连，在Connect里面过滤
		{
			baslog("[DeamonProcGetMsStatusRsp] EV_BAS_CONNECT!\n");
			SetTimer(EV_BAS_CONNECT, BAS_CONNETC_TIMEOUT);
		}
		if( INVALID_NODE == g_cBasApp.m_dwMcuNodeB)
		{
			baslog("[DeamonProcGetMsStatusRsp] EV_BAS_CONNECTB!\n");
			SetTimer(EV_BAS_CONNECTB, BAS_CONNETC_TIMEOUT);
		}
	}
	return;
}

/*=============================================================================
  函 数 名： ClearInstStatus
  功    能： 清空本实例保存的状态信息
  算法实现： 
  全局变量： 
  参    数： void
  返 回 值： void 
=============================================================================*/
void CBasInst::ClearInstStatus(void)
{
	CServMsg cServMsg;
	// 先停止适配
    for(u8 byChnIdx = 0; byChnIdx < m_byAllChnNum; byChnIdx++)	//向各通道发送停止消息
	{		
        cServMsg.SetConfId( m_atChnInfo[byChnIdx].m_cChnConfId );
		cServMsg.SetChnIndex( byChnIdx );
        post(MAKEIID(GetAppID(), CInstance::DAEMON), 
			 MCU_BAS_STOPADAPT_REQ, cServMsg.GetServMsg(), cServMsg.GetServMsgLen());
	}

    KillTimer(EV_BAS_NEEDIFRAME_TIMER);
	NEXTSTATE(u32(IDLE));   // DAEMON 实例进入空闲状态
    g_cBasApp.m_byRegAckNum = 0;
	g_cBasApp.m_dwMpcSSrc = 0;
	return;
}

/*====================================================================
	函数  : SendMsgToMcu
	功能  : 向MCU发送消息
	输入  : wEvent - 发送事件
	        cServMsg - 业务消息
	输出  : 无
	返回  : 成功返回TRUE否则FALSE
	注    : 
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
====================================================================*/
s32 CBasInst::SendMsgToMcu(u16 wEvent, CServMsg const &cServMsg)
{
    // 外设消息需同时发给两个Mcu
	BOOL32 bRet = TRUE;
    if (g_cBasApp.m_bEmbedMcu || OspIsValidTcpNode(g_cBasApp.m_dwMcuNode)) 
	{
		post(g_cBasApp.m_dwMcuIId, wEvent, cServMsg.GetServMsg(), cServMsg.GetServMsgLen(), g_cBasApp.m_dwMcuNode);
		baslog("Send Message %u(%s) to Mcu%d\n",
                            wEvent, ::OspEventDesc(wEvent), g_cBasApp.m_tCfg.wMcuId);
	}
	else
	{
		//baslog("Send Message failed %u(%s), since disconnected with MCU%d.\n",
        //                       wEvent, ::OspEventDesc(wEvent), g_cBasApp.m_tCfg.wMcuId);
		bRet = FALSE;
	}

    if(g_cBasApp.m_bEmbedMcuB || OspIsValidTcpNode(g_cBasApp.m_dwMcuNodeB))
    {
	    post(g_cBasApp.m_dwMcuIIdB, wEvent, cServMsg.GetServMsg(), cServMsg.GetServMsgLen(), g_cBasApp.m_dwMcuNodeB);
		baslog("Send Message %u(%s) to Mcu%d\n",
                            wEvent, ::OspEventDesc(wEvent), g_cBasApp.m_tCfg.wMcuId);
    }
    else
    {
        //baslog("Send Message failed %u(%s), since disconnected with MCU%d.\n",
        //                       wEvent, ::OspEventDesc(wEvent), g_cBasApp.m_tCfg.wMcuId);
		bRet = FALSE;
    }

    return bRet;
}

/*====================================================================
	函数  : SendChnNotif
	功能  : 向MCU发送通道状态通知
	输入  : 
	输出  : 无
	返回  : 成功返回TRUE否则FALSE
	注    : 
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
====================================================================*/
void CBasInst::SendChnNotif(u8 byChnIdx )
{
	u8 byEqpId = (u8)g_cBasApp.m_tCfg.byEqpId;
	
    CServMsg cServMsg;
    cServMsg.SetChnIndex( byChnIdx );
	cServMsg.SetConfId(m_atChnInfo[byChnIdx].m_cChnConfId);
	cServMsg.SetMsgBody(&byEqpId, sizeof(u8));
	cServMsg.CatMsgBody((u8*)&m_atChnInfo[byChnIdx].m_tChnStatus, sizeof(m_atChnInfo[byChnIdx].m_tChnStatus) );

    baslog("The chn: %d, Audio:%d, video: %d, type: %d (in send chanl notify)\n", byChnIdx,
            m_atChnInfo[byChnIdx].m_tChnStatus.GetAudType(), 
            m_atChnInfo[byChnIdx].m_tChnStatus.GetVidType(),
            m_atChnInfo[byChnIdx].byChnType );
	SendMsgToMcu(BAS_MCU_CHNNLSTATUS_NOTIF, cServMsg);
    return;
}

/*====================================================================
	函数  : SetEncryptParam
	功能  : 设置加密参数
	输入  : 
	输出  : 无
	返回  : 无
	注    : 
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
    05/01/12    3.6         libo          create
	05/04/11	4.0		    liuhuiyun	  调整
====================================================================*/
void CBasInst::SetEncryptParam(u8 byChnIdx, TMediaEncrypt*  ptMediaEncryptVideo,
                               TDoublePayload* ptDoublePayloadVideo,
                               TMediaEncrypt*  ptMediaEncryptAudio,
                               TDoublePayload* ptDoublePayloadAudio,
                               u8 byVidEncryptPT, u8 byAudEncryptPT, u8 byRealEncodePT)
{
    /**/
    baslog("********************************************************\n");
    baslog("ptDoublePayloadVideo->GetActivePayload() = %d\n", ptDoublePayloadVideo->GetActivePayload());
    baslog("ptDoublePayloadVideo->GetRealPayLoad()   = %d\n", ptDoublePayloadVideo->GetRealPayLoad());
    baslog("ptDoublePayloadAudio->GetActivePayload() = %d\n", ptDoublePayloadAudio->GetActivePayload());
    baslog("ptDoublePayloadAudio->GetRealPayLoad()   = %d\n", ptDoublePayloadAudio->GetRealPayLoad());

    baslog("byVidEncryptPT = %d\n", byVidEncryptPT);

    s32 len;
    u8 key[20];
    ptMediaEncryptVideo->GetEncryptKey(key, &len);
    baslog("ptMediaEncryptVideo->GetEncryptMode() = %d\n", ptMediaEncryptVideo->GetEncryptMode());
    baslog("Video key = %s, len = %d\n", key, len);
    ptMediaEncryptAudio->GetEncryptKey(key, &len);
    baslog("ptMediaEncryptAudio->GetEncryptMode() = %d\n", ptMediaEncryptAudio->GetEncryptMode());
    baslog("Audio key = %s, len = %d\n", key, len);
    baslog("********************************************************\n");

    u16 wRet;
    u8  abyKeyBuf[MAXLEN_KEY];
    s32 wKeySize;
    u8  byEncryptMode;

    u32 dwChnNo = byChnIdx;
    u8  byGrpNo = GetGrpNoFromChnIdx(byChnIdx);
    if( TYPE_AUDIO == m_atChnInfo[byChnIdx].byChnType && TRUE == m_atChnInfo[byChnIdx].bIsUsed)
    {
        byEncryptMode = ptMediaEncryptAudio->GetEncryptMode();
        if (byEncryptMode == CONF_ENCRYPTMODE_NONE) // 不需要加密
        {
            // zbq [10/13/2007] FEC支持
            u8 byPayload = 0;
            if ( MEDIA_TYPE_FEC == ptDoublePayloadAudio->GetActivePayload() )
            {
                byPayload = MEDIA_TYPE_FEC;
            }

            // 编码
            wRet = g_cBasApp.m_cAptGrp[byGrpNo].SetAudEncryptPT(dwChnNo, byPayload);
            if (CODEC_NO_ERROR != wRet)
            {
                baslog("Grp.%d call \"SetAudEncryptPT\" failed.\n", GetInsID());
                return;
            }
			// 加密Key
            wRet = g_cBasApp.m_cAptGrp[byGrpNo].SetAudEncryptKey(dwChnNo, (s8*)NULL, (u16)0, byEncryptMode);
            if (CODEC_NO_ERROR != wRet)
            {
                baslog("SetAudEncryptKey failed err=%d!\n", wRet);
                return;
            }

            // 解码
            wRet = g_cBasApp.m_cAptGrp[byGrpNo].SetAudioActivePT(dwChnNo, byPayload, byPayload);
            if (CODEC_NO_ERROR != wRet)
            {
                baslog("Grp.%d call \"SetAudioActivePT\" failed.\n", GetInsID());
                return;
            }

            wRet = g_cBasApp.m_cAptGrp[byGrpNo].SetAudDecryptKey(dwChnNo, (s8*)NULL, (u16)0, byEncryptMode);
            if (CODEC_NO_ERROR != wRet)
            {
                baslog("SetAudDecryptKey failed err=%d!\n", wRet);
                return;
            }
        }
        else if (byEncryptMode == CONF_ENCRYPTMODE_DES || byEncryptMode == CONF_ENCRYPTMODE_AES) // 需要加密
        {
            baslog("编码-音频\n");
            if (byEncryptMode == CONF_ENCRYPTMODE_DES)
            {
                byEncryptMode = DES_ENCRYPT_MODE;
            }
            else if (byEncryptMode == CONF_ENCRYPTMODE_AES)
            {
                byEncryptMode = AES_ENCRYPT_MODE;
            }

            // zbq [10/13/2007] FEC支持
            u8 byRealPayload = ptDoublePayloadAudio->GetRealPayLoad();
            u8 byActivePayload = ptDoublePayloadAudio->GetActivePayload();
            
            if ( MEDIA_TYPE_FEC == byActivePayload )
            {
                byRealPayload = MEDIA_TYPE_FEC;
            }

            wRet = g_cBasApp.m_cAptGrp[byGrpNo].SetAudEncryptPT(dwChnNo, byAudEncryptPT); // 音频编码载荷
            if (CODEC_NO_ERROR != wRet)
            {
                baslog("Grp.%d call \"SetAudEncryptPT\" failed.\n", GetInsID());
                return;
            }

            ptMediaEncryptAudio->GetEncryptKey(abyKeyBuf, &wKeySize); // 加密Key
            wRet = g_cBasApp.m_cAptGrp[byGrpNo].SetAudEncryptKey(dwChnNo, (s8*)abyKeyBuf, (u16)wKeySize, byEncryptMode);
            if (CODEC_NO_ERROR != wRet)
            {
                baslog("SetAudEncryptKey failed err=%d!\n", wRet);
                return;
            }

            baslog("解码-音频\n");
            wRet = g_cBasApp.m_cAptGrp[byGrpNo].SetAudioActivePT(dwChnNo, 
                                                                 byActivePayload,
                                                                 byRealPayload );
            if (CODEC_NO_ERROR != wRet)
            {
                baslog("Grp.%d call \"SetAudEncryptPT\" failed.\n", GetInsID());
                return;
            }

            ptMediaEncryptAudio->GetEncryptKey(abyKeyBuf, &wKeySize);
            wRet = g_cBasApp.m_cAptGrp[byGrpNo].SetAudDecryptKey(dwChnNo, (s8*)abyKeyBuf, (u16)wKeySize, byEncryptMode);
            if (CODEC_NO_ERROR != wRet)
            {
                baslog("SetAudDecryptKey failed err=%d!\n", wRet); 
                return;
            }
        }
    }

    if( TYPE_VIDEO == m_atChnInfo[byChnIdx].byChnType && TRUE == m_atChnInfo[byChnIdx].bIsUsed)
    {
        dwChnNo = 0;        // 视频通道号只能为0，因为一个视频适配组只有一个适配通道
        // 视频
        byEncryptMode = ptMediaEncryptVideo->GetEncryptMode();
        if (byEncryptMode == CONF_ENCRYPTMODE_NONE)
        {
            // 解码
            u8 byPayLoadValue;

            // zbq [10/13/2007] FEC支持
            if ( ptDoublePayloadVideo->GetActivePayload() == MEDIA_TYPE_FEC )
            {
                byPayLoadValue = MEDIA_TYPE_FEC;
            }
            else if (ptDoublePayloadVideo->GetRealPayLoad() == MEDIA_TYPE_H264)
            {
                byPayLoadValue = MEDIA_TYPE_H264;
            }
            else if (ptDoublePayloadVideo->GetRealPayLoad() == MEDIA_TYPE_H263PLUS)
            {
                byPayLoadValue = MEDIA_TYPE_H263PLUS;
            }
            else
            {
                byPayLoadValue = 0;
            }

            // 编码
            wRet = g_cBasApp.m_cAptGrp[byGrpNo].SetVidEncryptPT(dwChnNo, byRealEncodePT);
            if (CODEC_NO_ERROR != wRet)
            {
                baslog("Grp.%d call \"SetVidEncryptPT\" failed.\n", GetInsID());
                return;
            }

            wRet = g_cBasApp.m_cAptGrp[byGrpNo].SetVidEncryptKey(dwChnNo, (s8*)NULL, (u16)0, byEncryptMode);
            if (CODEC_NO_ERROR != wRet)
            {
                baslog("SetVidEncryptKey failed err=%d!\n", wRet); // changed
                return;
            }

            log(LOGLVL_EXCEPTION, "[bas] SetVideoActivePT =%d!\n", byPayLoadValue);
            wRet = g_cBasApp.m_cAptGrp[byGrpNo].SetVideoActivePT(dwChnNo, byPayLoadValue, byPayLoadValue);
            if (CODEC_NO_ERROR != wRet)
            {
                baslog("Grp.%d call \"SetVideoActivePT\" failed.\n", GetInsID()); //changed
                return;
            }
            wRet = g_cBasApp.m_cAptGrp[byGrpNo].SetVidDecryptKey(dwChnNo, (s8*)NULL, (u16)0, byEncryptMode);
            if (CODEC_NO_ERROR != wRet)
            {
                baslog("SetVidDecryptKey failed err=%d!\n", wRet);
                return;
            }
        }
        else if (byEncryptMode == CONF_ENCRYPTMODE_DES || byEncryptMode == CONF_ENCRYPTMODE_AES)
        {
            log(LOGLVL_DEBUG2, "[bas]编码-视频\n");
            //上层与下层的编解码模式不一致
            if (byEncryptMode == CONF_ENCRYPTMODE_DES)
            {
                byEncryptMode = DES_ENCRYPT_MODE;
            }
            else if (byEncryptMode == CONF_ENCRYPTMODE_AES)
            {
                byEncryptMode = AES_ENCRYPT_MODE;
            }

            // zbq [10/13/2007] FEC支持
            u8 byRealPayload = ptDoublePayloadVideo->GetRealPayLoad();
            u8 byActivePayload = ptDoublePayloadVideo->GetActivePayload();
            
            if ( MEDIA_TYPE_FEC == byActivePayload )
            {
                byRealPayload = MEDIA_TYPE_FEC;
            }

            wRet = g_cBasApp.m_cAptGrp[byGrpNo].SetVidEncryptPT(dwChnNo, byVidEncryptPT);
            if (CODEC_NO_ERROR != wRet)
            {
                baslog("Grp.%d call \"SetVidEncryptPT\" failed.\n", GetInsID()); // changed
                return;
            }

            ptMediaEncryptVideo->GetEncryptKey(abyKeyBuf, &wKeySize);
            wRet = g_cBasApp.m_cAptGrp[byGrpNo].SetVidEncryptKey(dwChnNo, (s8*)abyKeyBuf, (u16)wKeySize, byEncryptMode);
            if (CODEC_NO_ERROR != wRet)
            {
                baslog("SetVidEncryptKey failed err=%d!\n", wRet); // changed
                return;
            }

            log(LOGLVL_DEBUG2, "[bas]解码-视频\n");
            wRet = g_cBasApp.m_cAptGrp[byGrpNo].SetVideoActivePT(dwChnNo,
                                                                 byActivePayload,
                                                                 byRealPayload);
            if (CODEC_NO_ERROR != wRet)
            {
                baslog("Grp.%d call \"SetVideoEncryptPT\" failed.\n", GetInsID());
                return;
            }

            ptMediaEncryptVideo->GetEncryptKey(abyKeyBuf, &wKeySize);
            wRet = g_cBasApp.m_cAptGrp[byGrpNo].SetVidDecryptKey(dwChnNo, (s8*)abyKeyBuf, (u16)wKeySize, byEncryptMode);
            if (CODEC_NO_ERROR != wRet)
            {
                baslog("SetVidEncryptKey failed err=%d!\n", wRet); // changed
                return;
            }
        }
    }

    return;
}

/*====================================================================
	函数  : MsgStartAdptProc
	功能  : 开始适配处理
	输入  : 
	输出  : 无
	返回  : 无
	注    : 
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
	05/04/11	3.6		    liuhuiyun	  注释整理
====================================================================*/
void CBasInst::MsgStartAdptProc(u8 byChnIdx, CMessage* const pMsg)
{    
    CServMsg cServMsg(pMsg->content, pMsg->length);
    BOOL bIsUsed = m_atChnInfo[byChnIdx].bIsUsed;

    if( m_byAllChnNum < byChnIdx || NULL == pMsg ||TRUE == bIsUsed ) // 不正确的通道号，通道已被使用
    {
        cServMsg.SetErrorCode(ERR_BAS_CHNOTREAD);
		cServMsg.SetMsgBody((u8*)&g_cBasApp.m_tEqp, sizeof(g_cBasApp.m_tEqp));
		SendMsgToMcu(BAS_MCU_STARTADAPT_NACK, cServMsg);
		baslog("the chn.%d in unknown state !\n", GetInsID()-1);
    }

	baslog("Start adp, The channel is %d\n", byChnIdx);

	s8 achDstTypeV[LENGTH_TYPE] = {0};
	s8 achDstTypeA[LENGTH_TYPE] = {0};

    TMediaEncrypt  tMediaEncryptVideo;
	TDoublePayload tDoublePayloadVideo;

    TMediaEncrypt  tMediaEncryptAudio;
	TDoublePayload tDoublePayloadAudio;		

	TAdaptParam  tParm = *(TAdaptParam*)cServMsg.GetMsgBody();	
    u16 wBitrate = tParm.GetBitrate();
    // guzh [4/9/2007] 支持码率作弊
    if ( g_cBasApp.IsEnableCheat() )
    {
        wBitrate = g_cBasApp.GetDecRateDebug( wBitrate );
    }
    tParm.SetBitRate(wBitrate);

    tMediaEncryptVideo = *(TMediaEncrypt*)(cServMsg.GetMsgBody()  + sizeof(TAdaptParam));
    tDoublePayloadVideo= *(TDoublePayload*)(cServMsg.GetMsgBody() + sizeof(TAdaptParam) + sizeof(TMediaEncrypt));
	tMediaEncryptAudio = *(TMediaEncrypt*)(cServMsg.GetMsgBody()  + sizeof(TAdaptParam) + sizeof(TMediaEncrypt) + sizeof(TDoublePayload));
    tDoublePayloadAudio= *(TDoublePayload*)(cServMsg.GetMsgBody() + sizeof(TAdaptParam) + 2 * sizeof(TMediaEncrypt) + sizeof(TDoublePayload));
	
	// MCU前向纠错, zgc, 2007-09-28
	TCapSupportEx tCapSupportEx = *(TCapSupportEx*)(cServMsg.GetMsgBody() + sizeof(TAdaptParam) + 2 * sizeof(TMediaEncrypt) + 2 * sizeof(TDoublePayload));
	if( g_nbaslog == 1 )
	{
		tCapSupportEx.Print();
	}

    u8 byGrpNo = GetGrpNoFromChnIdx(byChnIdx);
	switch( m_atChnInfo[byChnIdx].m_tChnStatus.GetStatus() )
	{
	case TBasChnStatus::READY: // 状态保护
		{
            u16 wRet = 0;
            /*
            SetEncryptParam(byChnIdx, &tMediaEncryptVideo, &tDoublePayloadVideo,
                            &tMediaEncryptAudio, &tDoublePayloadAudio,
                            tParm.GetVidActiveType(), tParm.GetAudActiveType());*/

            // Rog [05/11/2005]
			g_byDstTypeV = tParm.GetVidActiveType();
			g_byDstTypeA = tParm.GetAudActiveType();
			GetMediaType( g_byDstTypeV, achDstTypeV );
            GetMediaType( g_byDstTypeA, achDstTypeA );			
			baslog("Video:%d, Audio:%d\n", g_byDstTypeV, g_byDstTypeA);
            baslog("Video:%s, Audio:%s\n", achDstTypeV, achDstTypeA);            
            baslog("the channele is: %d, type: %d\n", byChnIdx, m_atChnInfo[byChnIdx].byChnType);
            // Rog
            if( TYPE_AUDIO == m_atChnInfo[byChnIdx].byChnType && FALSE == m_atChnInfo[byChnIdx].bIsUsed ) // 音频
            {
                baslog("The channel %d is Audio\n", byChnIdx);
                wRet = AddAudioChannel(byChnIdx, tParm);
                if( 0 != wRet )
                {
                    cServMsg.SetErrorCode(ERR_BAS_OPMAP);
				    baslog("EqpId=%u, EqpType=%u\n", g_cBasApp.m_tEqp.GetEqpId(), g_cBasApp.m_tEqp.GetEqpType());
				
				    cServMsg.SetMsgBody((u8*)&g_cBasApp.m_tEqp,sizeof(g_cBasApp.m_tEqp));
				    SendMsgToMcu(BAS_MCU_STARTADAPT_NACK, cServMsg);
				    log(LOGLVL_EXCEPTION, "[bas] the chn.%d modify param failed!\n", GetInsID()-1);
				    return;
                }
                else
                {
                    baslog("Add Audio channel success the chnl: %d\n", byChnIdx);
                }
            }
            else if( TYPE_VIDEO == m_atChnInfo[byChnIdx].byChnType && FALSE == m_atChnInfo[byChnIdx].bIsUsed ) // 视频
            {
                baslog( "The channel %d is Video\n", byChnIdx);
                wRet = AddVideoChannel(byChnIdx, tParm);
                if( 0 != wRet )
                {
                    cServMsg.SetErrorCode(ERR_BAS_OPMAP);
				    baslog("EqpId=%u, EqpType=%u\n", g_cBasApp.m_tEqp.GetEqpId(), g_cBasApp.m_tEqp.GetEqpType());
				
				    cServMsg.SetMsgBody((u8*)&g_cBasApp.m_tEqp,sizeof(g_cBasApp.m_tEqp));
				    SendMsgToMcu(BAS_MCU_STARTADAPT_NACK, cServMsg);
				    baslog("the chn.%d modify param failed!\n", GetInsID()-1);
				    return;
                }
                else
                {
                    baslog("Add Video channel success the chnl: %d\n", byChnIdx);
                }                
            }
            else
            {
                baslog("the channel %d type is %d error,or Now is being used(%d)\n", 
                                    byChnIdx, m_atChnInfo[byChnIdx].byChnType, m_atChnInfo[byChnIdx].bIsUsed);
                cServMsg.SetMsgBody((u8*)&g_cBasApp.m_tEqp,sizeof(g_cBasApp.m_tEqp));
			    SendMsgToMcu(BAS_MCU_STARTADAPT_NACK, cServMsg);
                return;
            }

            // zbq [10/15/2007] FEC前向纠错 支持（目前不考虑双流）
            BOOL32 bFECEnable = FECTYPE_NONE != tCapSupportEx.GetVideoFECType() ? TRUE : FALSE;
            g_cBasApp.m_cAptGrp[byGrpNo].SetVidFecEnable(byChnIdx, bFECEnable);
            baslog("[MsgStartAdptProc] Fec Enable.%d, FecMode.%d\n", bFECEnable, tCapSupportEx.GetVideoFECType());

			// 记录通道状态
			m_atChnInfo[byChnIdx].m_tChnStatus.SetAudType(tParm.GetAudType());
			m_atChnInfo[byChnIdx].m_tChnStatus.SetVidType(tParm.GetVidType());
			m_atChnInfo[byChnIdx].m_tChnStatus.SetBitRate(tParm.GetBitrate());
			m_atChnInfo[byChnIdx].m_tChnStatus.SetResolution(tParm.GetWidth(), tParm.GetHeight());
			m_atChnInfo[byChnIdx].m_tChnStatus.SetStatus(u8(TBasChnStatus::RUNING));
            
            m_atChnInfo[byChnIdx].bIsUsed = TRUE;

            //zbq[01/03/2008]开启适配的编码参数调整
            SetEncryptParam(byChnIdx, &tMediaEncryptVideo, &tDoublePayloadVideo,
                            &tMediaEncryptAudio, &tDoublePayloadAudio,
                            tParm.GetVidActiveType(), tParm.GetAudActiveType(), tParm.GetVidType());

			baslog("Success EqpId=%u, EqpType=%u\n", g_cBasApp.m_tEqp.GetEqpId(), g_cBasApp.m_tEqp.GetEqpType());
			m_atChnInfo[byChnIdx].m_cChnConfId = cServMsg.GetConfId();

			cServMsg.SetMsgBody((u8*)&g_cBasApp.m_tEqp, sizeof(g_cBasApp.m_tEqp));
			SendMsgToMcu(BAS_MCU_STARTADAPT_ACK, cServMsg);

			SendChnNotif(byChnIdx);
            //u8 byGrpNo = GetGrpNoFromChnIdx(byChnIdx);  // 取通道对应的适配组

			// 设丢包重传参数
			TNetRSParam tNetRSParam;
            if (tParm.IsNeedbyPrs())		//是否需要包重传？ 1: 重传, 0: 不重传
			{		
				baslog("\nBas is needed by Prs!\n");
                tNetRSParam.m_wFirstTimeSpan  = g_cBasApp.m_tPrsTimeSpan.m_wFirstTimeSpan;
				tNetRSParam.m_wSecondTimeSpan = g_cBasApp.m_tPrsTimeSpan.m_wSecondTimeSpan;
				tNetRSParam.m_wThirdTimeSpan  = g_cBasApp.m_tPrsTimeSpan.m_wThirdTimeSpan;
				tNetRSParam.m_wRejectTimeSpan = g_cBasApp.m_tPrsTimeSpan.m_wRejectTimeSpan;
				g_cBasApp.m_cAptGrp[byGrpNo].SetNetSendFeedbackVideoParam(2000, TRUE);
				g_cBasApp.m_cAptGrp[byGrpNo].SetNetRecvFeedbackVideoParam(tNetRSParam, TRUE);
			}
			else
			{
				baslog("\nBas is not needed by Prs!\n");
                tNetRSParam.m_wFirstTimeSpan  = 0;
				tNetRSParam.m_wSecondTimeSpan = 0;
				tNetRSParam.m_wThirdTimeSpan  = 0;
				tNetRSParam.m_wRejectTimeSpan = 0;
				g_cBasApp.m_cAptGrp[byGrpNo].SetNetSendFeedbackVideoParam(2000, FALSE);
				g_cBasApp.m_cAptGrp[byGrpNo].SetNetRecvFeedbackVideoParam(tNetRSParam, FALSE);
			}
		}
		break;
	case TBasChnStatus::RUNING:
		cServMsg.SetErrorCode(ERR_BAS_CHRUNING);
		cServMsg.SetMsgBody((u8*)&g_cBasApp.m_tEqp,sizeof(g_cBasApp.m_tEqp));
		SendMsgToMcu(BAS_MCU_STARTADAPT_NACK, cServMsg);
		baslog("the chn.%d is runing ,refuse start adap req\n", byChnIdx);		
		break;

	default:
		cServMsg.SetErrorCode(ERR_BAS_CHNOTREAD);
		cServMsg.SetMsgBody((u8*)&g_cBasApp.m_tEqp, sizeof(g_cBasApp.m_tEqp));
		SendMsgToMcu(BAS_MCU_STARTADAPT_NACK, cServMsg);
		baslog("the chn.%d in unknown state !\n", byChnIdx);
		break;
	}
    return;
}

/*=============================================================================
  函 数 名： AddAudioChannel
  功    能： 开启一个音频通道
  算法实现： 
  全局变量： 
  参    数：  u8 byChnIdx
             TAdaptParam& tParm
  返 回 值： u16 
=============================================================================*/
u16 CBasInst::AddAudioChannel( u8 byChnIdx, TAdaptParam& tParm )
{
    u16 wReturn = 0;
	s8 achDstType[LENGTH_TYPE] = {0};
	
	TAdapterChannel tAdpChn;
	memset(&tAdpChn, 0, sizeof(TAdapterChannel));

    u8 byGrpNo = GetGrpNoFromChnIdx(byChnIdx);
    if( 0 == m_atAdpInfo[byGrpNo].byCanUseChnNum )
    {
        baslog("[Bas]there is not enough channels to use,  the adp group %d \n", byGrpNo);
        return ERR_BAS_NOENOUGHCHLS;
    }
    
	// 启动音频
    if (0 != m_byAudioChnNum)
    {
	    tAdpChn.byChnNo = byChnIdx;
	    tAdpChn.byMediaType = tParm.GetAudType();

	    baslog("Channel-%u, type: %d\n", byChnIdx, m_atChnInfo[byChnIdx].byChnType );
	    tAdpChn.tAdapterEncParam.tAudioEncParam.byAudioDuration = 36; // 默认
	    tAdpChn.tAdapterEncParam.tAudioEncParam.byAudioEncMode  = 4;  // 默认
	    tAdpChn.tDstNetAddr.dwIp  = g_cBasApp.m_dwMcuRcvIp;           // 发送的目的端
	    tAdpChn.tDstNetAddr.wPort = g_cBasApp.m_wMcuRcvStartPort + byChnIdx * PORTSPAN + 2;

        tAdpChn.tRtcpBackAddr.dwIp  = g_cBasApp.m_dwMcuRcvIp;     // Bas侧发现丢包后的回馈地址
        tAdpChn.tRtcpBackAddr.wPort = g_cBasApp.m_tCfg.wRcvStartPort + byChnIdx * PORTSPAN + 3;

		// ip置为0, zgc, 2007-09-06
	    //tAdpChn.tSrcNetAddr.dwIp  = g_cBasApp.m_tCfg.dwLocalIP;       // 从本端指定端口发送出去
		tAdpChn.tSrcNetAddr.dwIp  = 0;
	    tAdpChn.tSrcNetAddr.wPort = g_cBasApp.m_tCfg.wRcvStartPort + byChnIdx * PORTSPAN + 2;
	    
	    tAdpChn.tBindAddr.dwIp  = g_cBasApp.m_tCfg.dwLocalIP;         // 绑定端口()
	    tAdpChn.tBindAddr.wPort = LOCALBINDSTARTPORT + byChnIdx * PORTSPAN + 2;
                     
	    GetMediaType(tAdpChn.byMediaType, achDstType); 
	    baslog("Add Audio Channel : %s, Dst[%s@%d] Src[%s@%d] Rtcp[%s@%d]\n",
		                   achDstType,
		                   ipStr(tAdpChn.tDstNetAddr.dwIp), tAdpChn.tDstNetAddr.wPort,
		                   ipStr(tAdpChn.tSrcNetAddr.dwIp), tAdpChn.tSrcNetAddr.wPort,
                           ipStr(tAdpChn.tRtcpBackAddr.dwIp), tAdpChn.tRtcpBackAddr.wPort);
				    
	    u16 wRet = g_cBasApp.m_cAptGrp[byGrpNo].AddAudioChannel(&tAdpChn);
        if (MAP_OPERATE_SUCCESS != wRet)
	    {
		    baslog("Add audio channel failed(%d) when start adpt Grp.%dChn.%d \n", wRet, byGrpNo, byChnIdx);		    
            wReturn = wRet;
	    }
        else
        {
            m_byAudioSN++;
            m_atAdpInfo[byGrpNo].bIsUsed = (TRUE != m_atAdpInfo[byGrpNo].bIsUsed);
            m_atAdpInfo[byGrpNo].byCanUseChnNum--; // 可用通道数减一

	        baslog("Add Audio channel success Inst.%d Grp.%d ChnNo.%d\n", GetInsID(), byGrpNo, tAdpChn.byChnNo);
        }
    }
    else
    {
        baslog("There is not audio channel to use, the total audio chnls: %d\n", m_byAudioChnNum);
        wReturn = 255;
    }
    return wReturn;
}

/*=============================================================================
  函 数 名： AddVideoChannel
  功    能： 增加视频通道
  算法实现： 
  全局变量： 
  参    数：  u8 byChnIdx
             TAdaptParam& tParm
  返 回 值： u16 
=============================================================================*/
u16 CBasInst::AddVideoChannel( u8 byChnIdx, TAdaptParam& tParm )
{
    
    u16 wBitrate = tParm.GetBitrate();
    u16 wResult = 0;

    TAdapterChannel tAdpChn;
	TVideoDecParam  tDecParam;
	memset(&tAdpChn, 0, sizeof(TAdapterChannel));
	memset(&tDecParam, 0, sizeof(TVideoDecParam));
    
    u8 byGrpNo = GetGrpNoFromChnIdx(byChnIdx);
    
	// 启动视频
	tAdpChn.byChnNo     = 0;
	tAdpChn.byMediaType = tParm.GetVidType();
	g_cBasApp.GetDefaultParam(tParm.GetVidType(), tAdpChn.tAdapterEncParam.tVideoEncParam);
	
	if (tAdpChn.tAdapterEncParam.tVideoEncParam.m_dwSndNetBand == 0)
    {
		tAdpChn.tAdapterEncParam.tVideoEncParam.m_dwSndNetBand = wBitrate * 2 + 100;
    }
	
	tAdpChn.tAdapterEncParam.tVideoEncParam.m_byEncType    = tAdpChn.byMediaType;
	tAdpChn.tAdapterEncParam.tVideoEncParam.m_wBitRate     = wBitrate;
	tAdpChn.tAdapterEncParam.tVideoEncParam.m_wVideoHeight = tParm.GetHeight();
	tAdpChn.tAdapterEncParam.tVideoEncParam.m_wVideoWidth  = tParm.GetWidth();


	tAdpChn.tDstNetAddr.dwIp  = g_cBasApp.m_dwMcuRcvIp; // 同音频通道处理
	tAdpChn.tDstNetAddr.wPort = g_cBasApp.m_wMcuRcvStartPort + byChnIdx * PORTSPAN;

	// ip置为0, zgc, 2007-09-06
	//tAdpChn.tSrcNetAddr.dwIp  = g_cBasApp.m_tCfg.dwLocalIP;
	tAdpChn.tSrcNetAddr.dwIp  = 0;
	tAdpChn.tSrcNetAddr.wPort = g_cBasApp.m_tCfg.wRcvStartPort + byChnIdx * PORTSPAN;

    tAdpChn.tRtcpBackAddr.dwIp  = g_cBasApp.m_dwMcuRcvIp;
    tAdpChn.tRtcpBackAddr.wPort = g_cBasApp.m_tCfg.wRcvStartPort + byChnIdx * PORTSPAN + 1;

	tAdpChn.tBindAddr.dwIp  = g_cBasApp.m_tCfg.dwLocalIP;
	tAdpChn.tBindAddr.wPort = LOCALBINDSTARTPORT + byChnIdx * PORTSPAN;
	tAdpChn.byMediaType = tParm.GetVidType();
    u8 abyDstType[LENGTH_TYPE];
    memset(abyDstType, '\0', sizeof(abyDstType));

	GetMediaType(tAdpChn.byMediaType, (s8*)abyDstType);

	baslog("Add Video Channel : %s(%u), %dKbps %dx%d ,Dst[%s@%d] Src[%s@%d] Rtcp[%s@%d], LittlThanOne-%u\n",
		                   abyDstType,tAdpChn.byMediaType,
		                   tAdpChn.tAdapterEncParam.tVideoEncParam.m_wBitRate,
		                   tAdpChn.tAdapterEncParam.tVideoEncParam.m_wVideoHeight,
		                   tAdpChn.tAdapterEncParam.tVideoEncParam.m_wVideoWidth,
		                   ipStr(tAdpChn.tDstNetAddr.dwIp),
                           tAdpChn.tDstNetAddr.wPort,
		                   ipStr(tAdpChn.tSrcNetAddr.dwIp),
                           tAdpChn.tSrcNetAddr.wPort,
                           ipStr(tAdpChn.tRtcpBackAddr.dwIp),
                           tAdpChn.tRtcpBackAddr.wPort,
                           tAdpChn.tAdapterEncParam.tVideoEncParam.m_byFrmRateLittleThanOne);

    baslog("The Adp Group is %d, channle %d\n", byGrpNo, byChnIdx);
	u16 wRet = g_cBasApp.m_cAptGrp[byGrpNo].AddVideoChannel(&tAdpChn);
    if (MAP_OPERATE_SUCCESS != wRet)
	{
		baslog("Add video channel failed(%d) when start adpt Inst.%d %d\n", wRet, GetInsID(), byGrpNo);
		wResult = 255;
        return wResult;
	}
    else
    {
        baslog("Add Video channel %d success\n", tAdpChn.byChnNo);
    }

	g_cBasApp.GetDefaultDecParam(tParm.GetVidType(), tDecParam); // 根据视频类型取编码参数
	wRet = g_cBasApp.m_cAptGrp[byGrpNo].ChangeVideoDecParam(tAdpChn.byChnNo, &tDecParam);
	baslog("Change Video Dec Channel : FastRecvBufs=%u, PostPrcLvl=%u, StartRecvBufs=%u\n",
		    tDecParam.dwFastRecvBufs,
		    tDecParam.dwPostPrcLvl,
		    tDecParam.dwStartRecvBufs);
    
    if (MAP_OPERATE_SUCCESS != wRet)
	{
		baslog("Change video channel failed(%d)\n", wRet);
		wResult = 255;
        return wResult;				
	}

	if ( g_cBasApp.m_bIsUseSmoothSend )
    {
    // guzh [3/12/2007] 平滑发送支持
    SetSmoothSendRule( g_cBasApp.m_dwMcuRcvIp, 
                       g_cBasApp.m_wMcuRcvStartPort + byChnIdx * PORTSPAN,
                       tAdpChn.tAdapterEncParam.tVideoEncParam.m_wBitRate );
		baslog("Set smooth send rule, bitrate: %d\n", tAdpChn.tAdapterEncParam.tVideoEncParam.m_wBitRate);   
	}

	m_byVideoSN++;
    m_atAdpInfo[byChnIdx].byCanUseChnNum --;
    m_atAdpInfo[byChnIdx].bIsUsed = (TRUE != m_atAdpInfo[byChnIdx].bIsUsed);

	baslog("[Add video channel success Inst.%d Grp.%d ChnNo.%d\n", GetInsID(), byGrpNo, tAdpChn.byChnNo);

	return wResult;
}

/*====================================================================
	函数  : MsgAdjustAdptProc
	功能  : 调整适配参数
	输入  : 
	输出  : 无
	返回  : 无
	注    : 
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
====================================================================*/
void CBasInst::MsgAdjustAdptProc(u8 byChnIdx, CMessage* const pMsg)
{
	u16 wRet;
	s8 achDstTypeV[LENGTH_TYPE] = {0};
	s8 achDstTypeA[LENGTH_TYPE] = {0};

    TMediaEncrypt tMediaEncryptVideo;
	TDoublePayload tDoublePayloadVideo;

    TMediaEncrypt tMediaEncryptAudio;
	TDoublePayload tDoublePayloadAudio;

    baslog("Adjust adp param, The channel is: %d\n", byChnIdx);

	CServMsg cServMsg(pMsg->content, pMsg->length);
    if(NULL == pMsg)
    {
        baslog("[MsgAdjustAdptProc] pcMsg pointer should not be NULL !\n");
        return;
    }
    
    u8 byGrpNo = GetGrpNoFromChnIdx(byChnIdx);

	TAdaptParam* ptParm = (TAdaptParam*)cServMsg.GetMsgBody();
	
    tMediaEncryptVideo = *(TMediaEncrypt*)(cServMsg.GetMsgBody()  + sizeof(TAdaptParam));
    tDoublePayloadVideo= *(TDoublePayload*)(cServMsg.GetMsgBody() + sizeof(TAdaptParam) + sizeof(TMediaEncrypt));
	tMediaEncryptAudio = *(TMediaEncrypt*)(cServMsg.GetMsgBody()  + sizeof(TAdaptParam) + sizeof(TMediaEncrypt) + sizeof(TDoublePayload));
    tDoublePayloadAudio= *(TDoublePayload*)(cServMsg.GetMsgBody() + sizeof(TAdaptParam) + 2 * sizeof(TMediaEncrypt) + sizeof(TDoublePayload));
    
    u16 wBitrate = ptParm->GetBitrate();
    // guzh [4/9/2007] 支持码率作弊
    if ( g_cBasApp.IsEnableCheat() )
    {
        wBitrate = g_cBasApp.GetDecRateDebug( wBitrate );
    }
    ptParm->SetBitRate(wBitrate);

	switch( m_atChnInfo[byChnIdx].m_tChnStatus.GetStatus() ) 
	{
	case TBasChnStatus::RUNING:
		{
			/*
            SetEncryptParam(byChnIdx, &tMediaEncryptVideo, &tDoublePayloadVideo,
                            &tMediaEncryptAudio, &tDoublePayloadAudio,
                            ptParm->GetVidActiveType(), ptParm->GetAudActiveType());*/

            TVideoEncParam tVidParam;
            memset(&tVidParam, 0, sizeof(TVideoEncParam));
            // guzh [5/17/2007] 采用配置文件值
            tVidParam.m_byEncType     = ptParm->GetVidType();

            SetEncryptParam(byChnIdx, &tMediaEncryptVideo, &tDoublePayloadVideo,
							&tMediaEncryptAudio, &tDoublePayloadAudio,
							ptParm->GetVidActiveType(), ptParm->GetAudActiveType(), tVidParam.m_byEncType);

            g_cBasApp.GetDefaultParam(tVidParam.m_byEncType, tVidParam);

            tVidParam.m_wVideoHeight  = ptParm->GetHeight();
            tVidParam.m_wVideoWidth   = ptParm->GetWidth();
            tVidParam.m_wBitRate      = ptParm->GetBitrate();            
            if (tVidParam.m_dwSndNetBand == 0)
            {
                tVidParam.m_dwSndNetBand  = tVidParam.m_wBitRate * 2 + 100;
            }            

            /*			
			tVidParam.m_byCapPort     = 0;
			tVidParam.m_byFrameRate   = 25;
			tVidParam.m_byImageQulity = QC_MODE_QUALITY;
			tVidParam.m_wVideoHeight  = ptParm->GetHeight();
			tVidParam.m_wVideoWidth   = ptParm->GetWidth();
			tVidParam.m_byMaxKeyFrameInterval = 75;
			tVidParam.m_byMaxQuant    = 31;
			tVidParam.m_byMinQuant    = 3;
			tVidParam.m_byRcMode      = 1;
			tVidParam.m_dwSndNetBand  = 100 * 1000; //100M
			tVidParam.m_wBitRate      = ptParm->GetBitrate();
			tVidParam.m_byEncType     = ptParm->GetVidType();
            */

            baslog("Change Video Enc Param: %u, %dx%d, Bitrate.%d SndNetBand.%d Kbps\n",
                    tVidParam.m_byEncType,                
                    tVidParam.m_wVideoHeight,
                    tVidParam.m_wVideoWidth,
                    tVidParam.m_wBitRate,
                    tVidParam.m_dwSndNetBand);

            if( TYPE_VIDEO == m_atChnInfo[byChnIdx].byChnType && TRUE == m_atChnInfo[byChnIdx].bIsUsed )
            {
                wRet = g_cBasApp.m_cAptGrp[byGrpNo].ChangeVideoEncParam(0, &tVidParam);  // 视频通道号为0
                if (MAP_OPERATE_SUCCESS != wRet)
			    {
				    baslog("[Bas]Change Video Enc Param failed!(%d)\n", wRet);
			    }
			    else
			    {
				    m_atChnInfo[byChnIdx].m_tChnStatus.SetVidType(ptParm->GetVidType());
				    m_atChnInfo[byChnIdx].m_tChnStatus.SetBitRate(ptParm->GetBitrate());
				    m_atChnInfo[byChnIdx].m_tChnStatus.SetResolution(ptParm->GetWidth(), ptParm->GetHeight());
                    baslog("The Input Param is: video: %d, BitRate: %d, Width: %d, Height: %d\n",
                            ptParm->GetVidType(), ptParm->GetBitrate(), ptParm->GetWidth(), ptParm->GetHeight());
			    }
				if ( g_cBasApp.m_bIsUseSmoothSend )
				{
				// 先停止再重新设置, zgc, 2007-09-21
				ClearSmoothSendRule( g_cBasApp.m_dwMcuRcvIp, g_cBasApp.m_wMcuRcvStartPort + byChnIdx * PORTSPAN );
				// 重新设置平滑发送规则, zgc, 2007-09-19
				SetSmoothSendRule( g_cBasApp.m_dwMcuRcvIp, 
                       g_cBasApp.m_wMcuRcvStartPort + byChnIdx * PORTSPAN,
                       tVidParam.m_wBitRate );
				baslog("The smoothsendrule bitrate is : %d\n", tVidParam.m_wBitRate);
				}
            }
            else if( TYPE_AUDIO == m_atChnInfo[byChnIdx].byChnType && TRUE == m_atChnInfo[byChnIdx].bIsUsed) // 音频
            {
                TAdapterAudioEncParam tAudParam;
			    tAudParam.byAudioDuration = 36;
                tAudParam.byAudioEncMode = 4;
                
			    wRet = g_cBasApp.m_cAptGrp[byGrpNo].ChangeAudioEncParam(byChnIdx, ptParm->GetAudType(), &tAudParam);// 修改通道参数
                if (MAP_OPERATE_SUCCESS != wRet)
			    {
				    baslog("Change Audio Enc Param failed!(%d)\n", wRet);
			    }
			    else 
			    {
				    m_atChnInfo[byChnIdx].m_tChnStatus.SetAudType(ptParm->GetAudType());
                    baslog("Change Audio(%d) Adapt Success\n", ptParm->GetAudType());
			    }
            }    

			cServMsg.SetMsgBody(&g_cBasApp.m_tCfg.byEqpId, sizeof(g_cBasApp.m_tCfg.byEqpId));
			cServMsg.CatMsgBody((u8*)&m_atChnInfo[byChnIdx].m_tChnStatus, sizeof(m_atChnInfo[byChnIdx].m_tChnStatus));
			SendMsgToMcu( BAS_MCU_CHNNLSTATUS_NOTIF ,cServMsg);

			GetMediaType(m_atChnInfo[byChnIdx].m_tChnStatus.GetVidType(), achDstTypeV);
			GetMediaType(m_atChnInfo[byChnIdx].m_tChnStatus.GetAudType(), achDstTypeA);

			baslog("Change to %s %dx%d %s !\n",
				    achDstTypeV,
				    m_atChnInfo[byChnIdx].m_tChnStatus.GetWidth(),
				    m_atChnInfo[byChnIdx].m_tChnStatus.GetHeight(),
				    achDstTypeA);
		}
		break;
	default:
		break;
	}
    return;
}

/*====================================================================
	函数  : MsgStopAdptProc
	功能  : 停止适配
	输入  : 
	输出  : 无
	返回  : 无
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
====================================================================*/
void CBasInst::MsgStopAdptProc(u8 byChnIdx, CMessage* const pMsg)
{
	u16 wRet;
	CServMsg cServMsg(pMsg->content, pMsg->length);
    CConfId    m_cTmpConfId = cServMsg.GetConfId();

	if (!(m_atChnInfo[byChnIdx].m_cChnConfId == m_cTmpConfId))
	{
		cServMsg.SetErrorCode(ERR_BAS_OPMAP);
		SendMsgToMcu(BAS_MCU_STOPADAPT_NACK, cServMsg);

		baslog("channel.%d unmatch conf id in stop adpt req\n", byChnIdx);
		return;

	}

    baslog("The channel is: %d, type: %d\n", byChnIdx, m_atChnInfo[byChnIdx].byChnType );

	switch( m_atChnInfo[byChnIdx].m_tChnStatus.GetStatus() )
	{
	case TBasChnStatus::READY: // 状态保护
		{
			cServMsg.SetErrorCode(ERR_BAS_CHREADY);
			cServMsg.SetMsgBody((u8*)&g_cBasApp.m_tEqp,sizeof(g_cBasApp.m_tEqp));
			SendMsgToMcu(BAS_MCU_STOPADAPT_NACK, cServMsg);
		}
		break;
	case TBasChnStatus::RUNING:
        wRet = ProcFreeConfChns(byChnIdx, m_cTmpConfId);
        if( 0 != wRet )
        {
            if (1 == wRet)
		    {	
			    cServMsg.SetErrorCode(ERR_BAS_OPMAP);
			    SendMsgToMcu(BAS_MCU_STOPADAPT_NACK, cServMsg);
			    baslog("Remove audio channel failed(%d) when start adpt Inst.%d %d.%d \n",
                        wRet, GetInsID());
			    return;				
		    }
            if(2 == wRet)
            {
                cServMsg.SetErrorCode(ERR_BAS_OPMAP);
			    cServMsg.SetMsgBody((u8*)&g_cBasApp.m_tEqp, sizeof(g_cBasApp.m_tEqp));
			    SendMsgToMcu(BAS_MCU_STOPADAPT_NACK, cServMsg);
			    baslog("Remove video channel failed(%d) when start adpt Inst.%d %d.%d \n",
                        wRet, GetInsID(), 0);
			    return ;
            }
            
        }
        
//		TNetRSParam tNetRSParam;
//		tNetRSParam.m_wFirstTimeSpan  = g_cBasApp.m_tPrsTimeSpan.m_wFirstTimeSpan;
//		tNetRSParam.m_wSecondTimeSpan = g_cBasApp.m_tPrsTimeSpan.m_wSecondTimeSpan;
//		tNetRSParam.m_wThirdTimeSpan  = g_cBasApp.m_tPrsTimeSpan.m_wThirdTimeSpan;
//		tNetRSParam.m_wRejectTimeSpan = g_cBasApp.m_tPrsTimeSpan.m_wRejectTimeSpan;
//		g_cBasApp.m_cAptGrp[byGrpIdx].SetNetSendFeedbackVideoParam(2000, FALSE);
//		g_cBasApp.m_cAptGrp[byGrpIdx].SetNetRecvFeedbackVideoParam(tNetRSParam, FALSE);
		
		cServMsg.SetMsgBody((u8*)&g_cBasApp.m_tEqp, sizeof(g_cBasApp.m_tEqp));
		SendMsgToMcu(BAS_MCU_STOPADAPT_ACK, cServMsg);
		SendChnNotif(byChnIdx);

		g_byDstTypeA = MEDIA_TYPE_NULL;
		g_byDstTypeV = MEDIA_TYPE_NULL;

		break;
	default:
		cServMsg.SetErrorCode(ERR_BAS_CHNOTREAD);
		cServMsg.SetMsgBody((u8*)&g_cBasApp.m_tEqp, sizeof(g_cBasApp.m_tEqp));
		SendMsgToMcu(BAS_MCU_STARTADAPT_NACK, cServMsg);
		break;
	}
}

/*=============================================================================
  函 数 名： AddChannel
  功    能： 开启一个通道
  算法实现： 
  全局变量： 
  参    数： u8 byGrpIdx
             TAdaptParam* ptParm
  返 回 值： int 
=============================================================================*/
s32 CBasInst::AddChannel(u8 byChnIdx, TAdaptParam* ptParm)
{
	return 0;
    
}

/*=============================================================================
  函 数 名： ProcFreeConfChns
  功    能： 释放指定会议的所有适配通道
  算法实现： 
  全局变量： 
  参    数： CConfId&  cConfId
  返 回 值： u8 
=============================================================================*/
u8 CBasInst::ProcFreeConfChns(u8 byChnIdx, CConfId&  cConfId)
{
    u16 wRet = 0;
    u8  byGrpNo = 0;

    byGrpNo = GetGrpNoFromChnIdx(byChnIdx);
    if( m_atChnInfo[byChnIdx].m_cChnConfId == cConfId )
    {
        if( TYPE_AUDIO == m_atChnInfo[byChnIdx].byChnType)
        {                             
            wRet = g_cBasApp.m_cAptGrp[byGrpNo].RemoveAudioChannel(byChnIdx);
            if(MAP_OPERATE_SUCCESS != wRet)
            {
                baslog("Remove Audio channel %d fail\n", byChnIdx);
                return  1;  // 1：移除音频通道失败
            }
            else
            {
                baslog("Remove Audio channel %d Success\n", byChnIdx);
                m_atChnInfo[byChnIdx].bIsUsed = FALSE;
                m_atChnInfo[byChnIdx].m_tChnStatus.SetAudType(MEDIA_TYPE_NULL);
                m_atChnInfo[byChnIdx].m_tChnStatus.SetStatus(u8(TBasChnStatus::READY));
                m_atChnInfo[byChnIdx].m_tChnStatus.SetBitRate(0);
                m_atAdpInfo[byGrpNo].byCanUseChnNum++;
            }
        }
        else if(TYPE_VIDEO == m_atChnInfo[byChnIdx].byChnType )
        {
            
            wRet = g_cBasApp.m_cAptGrp[byGrpNo].RemoveVideoChannel(0);
            if(MAP_OPERATE_SUCCESS != wRet)
            {
                baslog("Remove Video channel %d fail\n", byChnIdx);
                return  2;  // 2：移除视频通道失败
            }
            else
            {
                baslog("Remove Video channel %d Success\n", byChnIdx);

                // guzh [3/12/2007] 平滑发送支持
                ClearSmoothSendRule( g_cBasApp.m_dwMcuRcvIp,
                                     g_cBasApp.m_wMcuRcvStartPort + byChnIdx * PORTSPAN);

                m_atChnInfo[byChnIdx].bIsUsed = FALSE;
                m_atChnInfo[byChnIdx].m_tChnStatus.SetVidType(MEDIA_TYPE_NULL);                
                m_atChnInfo[byChnIdx].m_tChnStatus.SetStatus(u8(TBasChnStatus::READY));
                m_atChnInfo[byChnIdx].m_tChnStatus.SetBitRate(0);
                m_atChnInfo[byChnIdx].m_tChnStatus.SetResolution(0, 0);
                m_atAdpInfo[byGrpNo].byCanUseChnNum++;
            }
        }           
    }    
    return 0;   // 成功
}

/*=============================================================================
  函 数 名： GetGrpNoFromChnIdx
  功    能： 由通道号得到适配组号
  算法实现： 
  全局变量： 
  参    数： u8 byChnIdx
  返 回 值： u8  
=============================================================================*/
u8  CBasInst::GetGrpNoFromChnIdx(u8 byChnIdx)
{
    if( 0 == m_byAudioGrpNum)   // 没有音频通道时，通道号对应适配组号
    {
        return byChnIdx;
    }
    
    // 顾振华 [5/25/2006]  Group0 支持3个通道的音频适配
    // 所以0-2代表音频通道，3-6代表视频通道
    if( byChnIdx <= 2 )
    {
        return 0;   // audio adp group
    }

    u8 byReturn = byChnIdx - 2;
    return byReturn;
}

/*====================================================================
	函数  : MsgTimerNeedIFrameProc
	功能  : 
	输入  : 无
	输出  : 无
	返回  : 无
	注    :
----------------------------------------------------------------------
	修改记录    ：
    日  期         版本        修改人       修改内容
    05/08/22       v3.6        libo         create
====================================================================*/
void CBasInst::MsgTimerNeedIFrameProc()
{
    CServMsg cServMsg;    
    TAdapterChannelStatis tAdapterChannelStatis;    
    memset(&tAdapterChannelStatis, 0, sizeof(TAdapterChannelStatis));
    static u8 byPrintCount = 1;
    if( byPrintCount > 20)
    {
        baslog("The Video group start index is: %d\n", m_byVideoGrpStartIdx);
        byPrintCount = 1;
    }
    for(u8 byLop = m_byVideoGrpStartIdx; byLop < (m_byVideoGrpStartIdx + m_byVideoGrpNum); byLop++)
    {
        u8 byIndex = byLop;
        if(1 < byIndex)  // 跳跃一块Map
        {
            byIndex++;
        }
        
        g_cBasApp.m_cAptGrp[byLop].GetVideoChannelStatis(0, tAdapterChannelStatis);// 视频通道固定为0
        u8 byChnNo = m_atAdpInfo[byIndex].abyChnInfo[0];
        if (tAdapterChannelStatis.m_bVidCompellingIFrm)
        {
            cServMsg.SetConfId( m_atChnInfo[byChnNo].m_cChnConfId );
            cServMsg.SetChnIndex( byChnNo );
            SendMsgToMcu(BAS_MCU_NEEDIFRAME_CMD, cServMsg);
            baslog("[MsgTimerNeedIFrameProc]Bas Grp %d channel:%d request iframe!!\n",byLop, byChnNo);
        }
    }
	        
	SetTimer(EV_BAS_NEEDIFRAME_TIMER, CHECK_IFRAME_INTERVAL); //定时检测
    return;
}

/*====================================================================
	函数  : MsgFastUpdatePicProc
	功能  : 请求Bas编一个关键帧
	输入  : 无
	输出  : 无
	返回  : 无
	注    :
----------------------------------------------------------------------
	修改记录    ：
    日  期         版本        修改人       修改内容
    05/08/22       v3.6        libo         create
====================================================================*/
void CBasInst::MsgFastUpdatePicProc(u8 byChnIdx)
{
    baslog("[MsgFastUpdatePicProc]Enter!\n");
    
    u8  byGrpNo = GetGrpNoFromChnIdx(byChnIdx);
    u32 dwTimeInterval = 60;
    u32 dwLastFUPTick = m_atChnInfo[byChnIdx].m_dwLastFUPTick;

    u32 dwCurTick = OspTickGet();	
    if (dwCurTick - dwLastFUPTick > dwTimeInterval)
    {
        m_atChnInfo[byChnIdx].m_dwLastFUPTick = dwCurTick;
        g_cBasApp.m_cAptGrp[byGrpNo].SetFastUpdata();
        baslog( "[MsgFastUpdatePicProc]MsgFastUpdatePicProc!\n");
    }
    return;
}

/*====================================================================
	函数  : StatusShow
	功能  : 显示适配器状态
	输入  : 
	输出  : 无
	返回  : 无
	注    : 
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
====================================================================*/
void CBasInst::StatusShow(CApp* pcApp)
{
    
	s8 STS[][10] = {"IDLE", "READY", "RUNING", "UNKNOW"};
	
	s8 achDstTypeV[LENGTH_TYPE] = {0};
	s8 achDstTypeA[LENGTH_TYPE] = {0};

	// 打印MTU, zgc, 2007-04-06
	OspPrintf(TRUE, FALSE, "The MTU Size is : %d\n", g_cBasApp.m_wMTUSize );

	OspPrintf( TRUE, FALSE, "\n====== Bitstream Adapter Server Status (%2d channels) =====\n", g_cBasApp.m_byChnNum );
	OspPrintf( TRUE, FALSE, "通道 状态   语音   视频    宽   高   码率K  接收  发送\n" ); 

	for ( s32 nLop = 0; nLop < m_byAllChnNum; nLop++ )
	{
		GetMediaType( m_atChnInfo[nLop].m_tChnStatus.GetAudType(), achDstTypeA );
		GetMediaType( m_atChnInfo[nLop].m_tChnStatus.GetVidType(), achDstTypeV );
        
		OspPrintf( TRUE, FALSE, "%2d)  %-6s %-5s %-5s    %-4d %-4d %-7d %-5d %-5d\n",
                                nLop,
                                STS[m_atChnInfo[nLop].m_tChnStatus.GetStatus()],
                                achDstTypeA,
                                achDstTypeV,
                                m_atChnInfo[nLop].m_tChnStatus.GetWidth(), m_atChnInfo[nLop].m_tChnStatus.GetHeight(),
                                m_atChnInfo[nLop].m_tChnStatus.GetBitrate(),
                                nLop * PORTSPAN + g_cBasApp.m_tCfg.wRcvStartPort,
                                g_cBasApp.m_wMcuRcvStartPort + nLop * PORTSPAN );
	}
    return;
}

/*====================================================================
	函数  : BasInfo
	功能  : 显示适配器状态(内部调试用)
	输入  : 
	输出  : 无
	返回  : 无
	注    : 
----------------------------------------------------------------------
	修改记录    ：
	日  期         版本        修改人        修改内容
    05/08/22       v3.6        libo           create
====================================================================*/
void CBasInst::BasInfo()
{
    u16 wRet;
    TAdapterGroupStatus tAdapterGroupStatus;
    TAdapterChannelStatis tAdapterChannelStatis;

	s8   pDstType[LENGTH_TYPE] = {0};
	s8   pSrcType[LENGTH_TYPE] = {0};
	s32  nType;
	s32  nHeight;
	s32  nWidth;
	s32  nSndBit;
	BOOL bFlag = FALSE;  
    u8   byChnIdx = 0;

    u16 wAptGrpIdx = 0;
    for(wAptGrpIdx = 0; wAptGrpIdx < m_byAllGrpNum; wAptGrpIdx++)
    {
        memset(&tAdapterGroupStatus, 0, sizeof(TAdapterGroupStatus));
        wRet = g_cBasApp.m_cAptGrp[wAptGrpIdx].GetGroupStatus(tAdapterGroupStatus);
        
		memset(pDstType, 0, LENGTH_TYPE);

        switch(wAptGrpIdx)
        {

        case 0:
            OspPrintf( TRUE, FALSE, "\n=============================音频适配组==============================\n");
			GetMediaType(g_byDstTypeA, pDstType);
			break;
        case 1:
        case 2:
            OspPrintf( TRUE, FALSE, "\n=============================视频适配组==============================\n");
            byChnIdx =  m_atAdpInfo[wAptGrpIdx].abyChnInfo[0];  
        	OspPrintf( TRUE, FALSE, "Video:%d\n", m_atChnInfo[byChnIdx].m_tChnStatus.GetVidType());
			GetMediaType(g_byDstTypeV, pDstType);
            bFlag = TRUE;
			break;
        default:
            break;
        }

		s32 nCount = tAdapterGroupStatus.byCurChnNum - 1;
		nType = tAdapterGroupStatus.atChannel[nCount].byMediaType;
		if( nType == 255 )
		{
			tAdapterGroupStatus.bAdapterStart = 0;
			tAdapterGroupStatus.byCurChnNum   = 0;
		}

        OspPrintf( TRUE, FALSE, "\n适配是否开始 = %d   该适配组创建的适配通道数目 = %d\n\n",
                                tAdapterGroupStatus.bAdapterStart,
                                tAdapterGroupStatus.byCurChnNum);
		
        OspPrintf( TRUE, FALSE, "通道号 源码流  目标码流    通道源地址(Port)     通道目的地址(Port)       捆绑地址\n");
        u16 wLoop = 0;
        for(wLoop = 0; wLoop < tAdapterGroupStatus.byCurChnNum; wLoop++)
        {
            nType = tAdapterGroupStatus.atChannel[wLoop].byMediaType;
			GetMediaType( nType, pSrcType);
			
            // 已经停止适配
			if(nType == 255)
			{
				break;
			}
			OspPrintf( TRUE, FALSE, "  %2d    %s    %s     %s(%d)     %s(%d)      %s(%d)\n",
                                    tAdapterGroupStatus.atChannel[wLoop].byChnNo,
                                    pSrcType, pDstType,
                                    ipStr(tAdapterGroupStatus.atChannel[wLoop].tSrcNetAddr.dwIp),
									tAdapterGroupStatus.atChannel[wLoop].tSrcNetAddr.wPort,

                                    ipStr(tAdapterGroupStatus.atChannel[wLoop].tDstNetAddr.dwIp),
									tAdapterGroupStatus.atChannel[wLoop].tDstNetAddr.wPort,
									
                                    ipStr(tAdapterGroupStatus.atChannel[wLoop].tBindAddr.dwIp),
									tAdapterGroupStatus.atChannel[wLoop].tBindAddr.wPort
                                    );
        }

		if( bFlag == TRUE)
        {
            OspPrintf( TRUE, FALSE, "\t图像高度\t图像宽度\t发送码率\n");
		    for(wLoop = 0; wLoop<tAdapterGroupStatus.byCurChnNum; wLoop++)
		    {
		    	
		    	nHeight = tAdapterGroupStatus.atChannel[wLoop].tAdapterEncParam.tVideoEncParam.m_wVideoHeight;
		    	nWidth = tAdapterGroupStatus.atChannel[wLoop].tAdapterEncParam.tVideoEncParam.m_wVideoWidth;
		    	nSndBit = tAdapterGroupStatus.atChannel[wLoop].tAdapterEncParam.tVideoEncParam.m_wBitRate;
		    	OspPrintf( TRUE, FALSE, "\t%d\t       %d\t       %d\n",nHeight, nWidth, nSndBit);

		    }
        }

        OspPrintf( TRUE, FALSE, "接收码率  收到的包数  网络接收丢包数  发送码率  发送的包数  发送丢包数\n");
        for(wLoop = 0; wLoop < tAdapterGroupStatus.byCurChnNum; wLoop++)
        {
            memset(&tAdapterChannelStatis, 0, sizeof(TAdapterChannelStatis));
            switch(wAptGrpIdx)
            {
            case 0:
                wRet = g_cBasApp.m_cAptGrp[wAptGrpIdx].GetAudioChannelStatis((u8)wLoop, tAdapterChannelStatis);
                if ( MAP_OPERATE_SUCCESS != wRet )
                {
                    OspPrintf( TRUE, FALSE, "[Bas]: get audio channel statis failed !\n");
                }
        	    break;
            case 1:
            case 2:
                wRet = g_cBasApp.m_cAptGrp[wAptGrpIdx].GetVideoChannelStatis((u8)wLoop, tAdapterChannelStatis);
                if ( MAP_OPERATE_SUCCESS != wRet )
                {
                    OspPrintf( TRUE, FALSE, "[Bas]: get video channel statis failed !\n");
                }
        	    break;
            }
            OspPrintf( TRUE, FALSE, "  %-4d     %-8d      %-8d       %-4d     %-8d    %-8d\n",
                                    tAdapterChannelStatis.dwRecvBitRate,
                                    tAdapterChannelStatis.dwRecvPackNum,
                                    tAdapterChannelStatis.dwRecvLosePackNum,
                                    tAdapterChannelStatis.dwSendBitRate,
                                    tAdapterChannelStatis.dwSendPackNum,
                                    tAdapterChannelStatis.dwSendLosePackNum);
        }

        OspPrintf( TRUE, FALSE, "\n==================================================================\n");
    }
}

/*=============================================================================
  函 数 名： ProcSetQosMsg
  功    能： 
  算法实现： 
  全局变量： 
  参    数： CMessage* const pMsg
  返 回 值： void 
=============================================================================*/
void CBasInst::ProcSetQosMsg(CMessage* const pMsg)
{
    if(NULL == pMsg)
    {
        baslog("The Bas's Qos infomation is Null\n");
        return;
    }
    TMcuQosCfgInfo  tQosInfo;
    CServMsg cServMsg(pMsg->content, pMsg->length);
    tQosInfo = *(TMcuQosCfgInfo*)cServMsg.GetMsgBody();

    u8 byQosType    = tQosInfo.GetQosType();
    u8 byAudioValue = tQosInfo.GetAudLevel();
    u8 byVideoValue = tQosInfo.GetVidLevel();
    u8 byDataValue  = tQosInfo.GetDataLevel();
    u8 byIpPriorValue = tQosInfo.GetIpServiceType();
	baslog("Type: %d, Aud= %d, Vid= %d, Data= %d, IpPrior= %d\n", 
		    byQosType, byAudioValue, byVideoValue, byDataValue, byIpPriorValue);

    if(QOSTYPE_IP_PRIORITY == byQosType)
    {
        byAudioValue = (byAudioValue << 5);
        byVideoValue = (byVideoValue << 5);
        byDataValue  = (byDataValue << 5);
        ComplexQos(byAudioValue, byIpPriorValue);
        ComplexQos(byVideoValue, byIpPriorValue);
        ComplexQos(byDataValue,  byIpPriorValue);
    }
    else
    {
        byAudioValue = (byAudioValue << 2);
        byVideoValue = (byVideoValue << 2);
        byDataValue  = (byDataValue  << 2);
    }

	baslog("\nAud= %d, Vid= %d, Data= %d\n", 
					byAudioValue, byVideoValue, byDataValue);
    
    SetMediaTOS((s32)byAudioValue, QOS_AUDIO);
    SetMediaTOS((s32)byVideoValue, QOS_VIDEO);
    SetMediaTOS((s32)byDataValue,  QOS_DATA );
    
    return;
}

/////////////////////////////////////////////////////////////////
/////////// Qos值组合规则
/////////////////////////////////////////////////////////////////
/*
(u8)nTOS的各位含义：
0为最高位

IP优先方式：

   0     1     2     3     4     5     6     7
+-----+-----+-----+-----+-----+-----+-----+-----+
|                 |     |     |     |     |     |
|   PRECEDENCE    |  D  |  T  |  R  |  0  |  0  |
|                 |     |     |     |     |     |
+-----+-----+-----+-----+-----+-----+-----+-----+

Bits 0-2: Precedence.
Bit  3:   0 = Normal Delay, 1 = Low Delay.
Bits 4:   0 = Normal Throughput, 1 = High Throughput.
Bits 5:   0 = Normal Relibility, 1 = High Relibility.
Bit 6-7:  Reserved for Future Use.

Precedence:
111 - Network Control
110 - Internetwork Control
101 - CRITIC/ECP
100 - Flash Override
011 - Flash
010 - Immediate
001 - Priority
000 - Routine


0为最高位

区别服务方式：

   0     1     2     3     4     5     6     7
+-----+-----+-----+-----+-----+-----+-----+-----+
|                                   |     |     |
|   	区分服务      	            |  0  |  0  |
|                                   |     |     |
+-----+-----+-----+-----+-----+-----+-----+-----+

区分服务:0-63
//////////////////////////////////////////////////////////////////////////////
*/

/*=============================================================================
  函 数 名： ComplexQos
  功    能： 组合Qos值
  算法实现： 
  全局变量： 
  参    数： u8& byValue
             u8 byPrior
  返 回 值： void 
=============================================================================*/
void CBasInst::ComplexQos(u8& byValue, u8 byPrior)
{
    u8 byLBit = ((byPrior & 0x08) >> 3);
    u8 byRBit = ((byPrior & 0x04) >> 1);
    u8 byTBit = ((byPrior & 0x02) << 1);
    u8 byDBit = ((byPrior & 0x01) << 3);
    
    byValue = byValue + ((byDBit + byTBit + byRBit + byLBit)*2);
    return;
}

/*====================================================================
	函数  : GetMediaType
	功能  : 取得媒体类型
	输入  : nType: 媒体类型
	输出  : 无
	返回  : 媒体类型对应的字符串
	注    : 
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
====================================================================*/
BOOL32 CBasInst::GetMediaType(u8 byType, s8* pDstType)
{    
    if ( NULL == pDstType )
    {
        baslog("[GetMediaType] the pointer should not be NULL !\n");
        return FALSE;
    }

    memset(pDstType, 0, LENGTH_TYPE);

	switch(byType)
	{
    //audio
	case MEDIA_TYPE_MP3:                //mp3 mode 0-4
		sprintf(pDstType, "mp3\0");
		break;
		
	case MEDIA_TYPE_PCMA:               //G.711 Alaw  mode 5
		sprintf(pDstType, "G.711A\0");
		break;
		
	case MEDIA_TYPE_PCMU:               //G.711 ulaw  mode 6
		sprintf(pDstType, "G.711U\0");
		break;

	case MEDIA_TYPE_G721:               //G.721
		sprintf(pDstType, "G.721\0");
		break;
	case MEDIA_TYPE_G722:               //G.722
		sprintf(pDstType, "G.722\0");
		break;
	case MEDIA_TYPE_G7231:              //G.7231
		sprintf(pDstType, "G.7231\0");
		break;
	case MEDIA_TYPE_G728:               //G.728
		sprintf(pDstType, "G.728\0");
		break;
	case MEDIA_TYPE_G729:               //G.729
		sprintf(pDstType, "G.729\0");
		break;
	case MEDIA_TYPE_G7221C:              //G.7221
		sprintf(pDstType, "G.7221\0");
		break;
    case MEDIA_TYPE_AACLC:
        sprintf( pDstType, "AAC-LC\0" );  //AAC-LC
        break;
    case MEDIA_TYPE_AACLD:
        sprintf( pDstType, "AAC-LD\0" );  //AAC-LD
        break;		
    //video
	case MEDIA_TYPE_MP4:                //MPEG-4
		sprintf(pDstType, "MPG4\0");
		break;
	case MEDIA_TYPE_H261:               //H.261
		sprintf(pDstType, "H.261\0");
		break;
	case MEDIA_TYPE_H262:               //H.262(MPEG-2)
		sprintf(pDstType, "MPG2\0");
		break;
	case MEDIA_TYPE_H263:               //H.263
		sprintf(pDstType, "H.263\0");
		break;
	case MEDIA_TYPE_H264:               //H.264
		sprintf(pDstType, "H.264\0");
		break;
    case MEDIA_TYPE_NULL:
		sprintf(pDstType, "NULL\0");
        break;
	default:
		sprintf(pDstType, "Unknown(%d)\0", byType); // 未定义
		break;
	}
	return TRUE;
}

/*====================================================================
	函数  : ipStr
	功能  : 取得IP地址字符串
	输入  : dwIp: IP地址
	输出  : 无
	返回  : IP地址对应的字符串
	注    : 
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
====================================================================*/
s8* CBasInst::ipStr(u32 dwIP)
{
	static s8 achIPStr[4][32]={0};
	static s8 byIdx = 0;
    struct in_addr in;

	byIdx++;
	s8 bySize = sizeof(achIPStr)/sizeof(achIPStr[0]);
	byIdx %= bySize;
    in.s_addr = dwIP;
	memset(achIPStr[byIdx], 0, sizeof(achIPStr[0]));
	
#ifdef _MSC_VER
	strncpy(achIPStr[byIdx],inet_ntoa(in),sizeof(achIPStr[0]));
#endif //_MSC_VER

#ifdef _LINUX_
	strncpy(achIPStr[byIdx],inet_ntoa(in),sizeof(achIPStr[0]));
#endif //_LINUX_
	
#ifdef _VXWORKS_
	inet_ntoa_b(in, achIPStr[byIdx]);
#endif //_VXWORKS_
	
	achIPStr[byIdx][31] = '\0'; // 字符串结束标志
	return achIPStr[byIdx];
}


/*=============================================================================
函 数 名： SetSmoothSendRule
功    能： 设置平滑发送规则
算法实现： 
全局变量： SSErrno
参    数： 均为主机序
返 回 值： void
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/03/12  4.0			顾振华                  创建
=============================================================================*/
void CBasInst::SetSmoothSendRule(u32 dwDestIp, u16 wPort,u32 dwRate)
{
    dwRate = dwRate >> 3;   // Bit 2 Byte;
    u32 dwPeak = dwRate + dwRate / 5;   // add 20% 
    baslog( "[SetSmoothSendRule] Setting ssrule for 0x%x:%d, rate/peak=%u/%u KByte.\n",
        dwDestIp, wPort, dwRate, dwPeak );

#ifdef _LINUX_
    s32 nRet = SetSSRule(dwDestIp, wPort, dwRate, dwPeak, 2);   // default 2 second
    if ( 0 == nRet )
    {
        baslog( "[SetSmoothSendRule] Set rule failed. SSErrno=%d\n", SSErrno );
    }
#endif

}

/*=============================================================================
函 数 名： ClearSmoothSendRule
功    能： 取消设置平滑发送规则
算法实现： 
全局变量： SSErrno
参    数： 均为主机序
返 回 值： void
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/03/12  4.0			顾振华                  创建
=============================================================================*/
void CBasInst::ClearSmoothSendRule(u32 dwDestIp, u16 wPort)
{
    baslog( "[ClearSmoothSendRule] Unsetting ssrule for 0x%x:%d.\n",
        dwDestIp, wPort );

#ifdef _LINUX_
    s32 nRet = UnsetSSRule(dwDestIp, wPort);
    if ( 0 == nRet )
    {
        baslog( "[ClearSmoothSendRule] Unset rule failed. SSErrno=%d\n", SSErrno );
    }
#endif

}

/*=============================================================================
函 数 名： StopSmoothSend
功    能： 停止平滑发送
算法实现： 
全局变量： SSErrno
参    数： 
返 回 值： void
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/03/12  4.0			顾振华                  创建
=============================================================================*/
void CBasInst::StopSmoothSend()
{
    baslog( "[StopSmoothSend] Stopping SmoothSend\n" );

#ifdef _LINUX_
    s32 nRet = CloseSmoothSending();
    if ( 0 == nRet )
    {
        baslog( "[ClearSmoothSendRule] CloseSmoothSending failed. SSErrno=%d\n", SSErrno );
    }
#endif 

}

CBasCfg::CBasCfg()
{
    m_dwMcuRcvIp  = 0;
    m_dwMcuRcvIpB = 0;
    m_wMcuRcvStartPort  = 0;
    m_wMcuRcvStartPortB = 0;
    m_bEmbedMcu  = 0;
    m_bEmbedMcuB = 0;
    m_byChnNum   = 0;
    m_byRegAckNum = 0;
	m_dwMpcSSrc  = 0;
	m_dwMcuNode = INVALID_NODE;
    m_dwMcuIId  = INVALID_INS;
	m_dwMcuNodeB = INVALID_NODE;
    m_dwMcuIIdB  = INVALID_INS;
    memset( &m_tEqp, 0, sizeof(m_tEqp) );
    memset( &m_tCfg, 0, sizeof(m_tCfg) );
    memset( &m_tPrsTimeSpan, 0, sizeof(m_tPrsTimeSpan) );

	m_bIsUseSmoothSend = FALSE;
    
    for( u8 byLop = 0; byLop < MAXNUM_BASGRP + 1; byLop++)
    {
        m_cAptGrp[byLop].~ CKdvAdapterGroup() ;
    }
}

CBasCfg::~CBasCfg()
{
}

/*=============================================================================
  函 数 名： FreeStatusDataA
  功    能： 清除状态参数
  算法实现： 
  全局变量： 
  参    数： void
  返 回 值： void 
=============================================================================*/
void CBasCfg::FreeStatusDataA(void)
{
    m_dwMcuNode = INVALID_NODE;
    m_dwMcuIId  = INVALID_INS;
    return;
}

/*=============================================================================
  函 数 名： FreeStatusDataB
  功    能： 清除状态参数
  算法实现： 
  全局变量： 
  参    数： void
  返 回 值： void 
=============================================================================*/
void CBasCfg::FreeStatusDataB(void)
{
    m_dwMcuNodeB = INVALID_NODE;
    m_dwMcuIIdB  = INVALID_INS;
    return;
}

/*=============================================================================
  函 数 名： ReadDebugValues
  功    能： 从配置文件读取码率作弊值
  算法实现： 
  全局变量： 
  参    数： void
  返 回 值： void 
=============================================================================*/
void CBasCfg::ReadDebugValues()
{
    s32 nValue;
    ::GetRegKeyInt( BASCFG_FILE, SECTION_BASDEBUG, "IsEnableRateAutoDec", 1, &nValue );
    m_tDebugVal.SetEnableBitrateCheat( (nValue==0) ? FALSE : TRUE );

    if ( !m_tDebugVal.IsEnableBitrateCheat() )
        return;

    ::GetRegKeyInt( BASCFG_FILE, SECTION_BASDEBUG, "DefaultTargetRatePct", 75, &nValue );
    m_tDebugVal.SetDefaultRate((u16)nValue);

    return;
}


/*====================================================================
	函数  : GetDefaultParam
	功能  : 根据媒体类型取相应编码的参数
	输入  : byEnctype: 媒体类型 [In]
			tEncparam: 保存参数 [Out]
	输出  : 无
	返回  : 无
	注    : 
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
    07/05/17    4.0         顾振华        整理
====================================================================*/
void CBasCfg::GetDefaultParam(u8 byEnctype, TVideoEncParam& tEncparam)
{
    s32 nValue;
	// 读是否打开平滑发送, zgc, 2008-03-03
	if (!::GetRegKeyInt(BASCFG_FILE, SECTION_BASDEBUG, "IsUseSmoothSend", 1, &nValue))
	{
		m_bIsUseSmoothSend = TRUE;
	}
	else
	{
		m_bIsUseSmoothSend = ( 1 == nValue ) ? TRUE : FALSE;
	}

    switch( byEnctype )
    {
    case MEDIA_TYPE_MP4:
        tEncparam.m_byEncType = (u8)EnVidFmt[0][0];

        if (!::GetRegKeyInt(BASCFG_FILE, SECTION_BASSYS, "MPEG4BitrateControlMode", 0, &nValue))
        {
            tEncparam.m_byRcMode = (u8)EnVidFmt[0][1];
        }
        else
        {
            tEncparam.m_byRcMode = (u8)nValue;
        }

        if (!::GetRegKeyInt(BASCFG_FILE, SECTION_BASSYS, "MPEG4VideoEcdIKeyRate", 0, &nValue))
        {
            tEncparam.m_byMaxKeyFrameInterval = EnVidFmt[0][2];
        }
        else
        {
            tEncparam.m_byMaxKeyFrameInterval = nValue;
        }

        if (!::GetRegKeyInt(BASCFG_FILE, SECTION_BASSYS, "MPEG4VideoEcdQualMaxValu", 0, &nValue))
        {
            tEncparam.m_byMaxQuant = (u8)EnVidFmt[0][3];
        }
        else
        {
            tEncparam.m_byMaxQuant = (u8)nValue;
        }

        if (!::GetRegKeyInt(BASCFG_FILE, SECTION_BASSYS, "MPEG4VideoEcdQualMinValu", 0, &nValue))
        {
            tEncparam.m_byMinQuant = (u8)EnVidFmt[0][4];
        }
        else
        {
            tEncparam.m_byMinQuant = (u8)nValue;
        }

        if (!::GetRegKeyInt(BASCFG_FILE, SECTION_BASSYS, "MPEG4EncodeBitRate", 0, &nValue))
        {
            tEncparam.m_wBitRate = EnVidFmt[0][5];
        }
        else
        {
            tEncparam.m_wBitRate = (u16)nValue;
        }

        if (!::GetRegKeyInt(BASCFG_FILE, SECTION_BASSYS, "MPEG4BandWidth", 0, &nValue))
        {
            tEncparam.m_dwSndNetBand = 0;
        }
        else
        {
            tEncparam.m_dwSndNetBand = (u32)nValue;
        }
/*
        if (!::GetRegKeyInt(BASCFG_FILE, SECTION_BASSYS, "MPEG4VideoEncLostFrameOnOff", 0, &nValue))
        {
            encparam.m_byAutoDelFrm = 2;
        }
        else
        {
            encparam.m_byAutoDelFrm = (u32)nValue;
        }
*/
    #ifdef SOFT_CODEC
        tEncparam.m_byQcMode = 0;
        tEncparam.m_byQI     = 5;
        tEncparam.m_byQP     = 13;
    #else
        tEncparam.m_byPalNtsc = (u8)EnVidFmt[0][6];
        tEncparam.m_byCapPort = (u8)EnVidFmt[0][7];
    
        if (!::GetRegKeyInt(BASCFG_FILE, SECTION_BASSYS, "MPEG4FrameRate", 0, &nValue))
        {
            tEncparam.m_byFrameRate = (u8)EnVidFmt[0][8];
        }
        else
        {
            tEncparam.m_byFrameRate = (u8)nValue;
        }

        if (!::GetRegKeyInt(BASCFG_FILE, SECTION_BASSYS, "MPEG4VideoImageQuality", 0, &nValue))
        {
            tEncparam.m_byImageQulity = (u8)EnVidFmt[0][9];
        }
        else
        {
            tEncparam.m_byImageQulity = (u8)nValue;
        }

        if (!::GetRegKeyInt(BASCFG_FILE, SECTION_BASSYS, "MPEG4VideoHeight", 0, &nValue))
        {
            tEncparam.m_wVideoHeight	= EnVidFmt[0][10];
        }
        else
        {
            tEncparam.m_wVideoHeight = (u16)nValue;
        }

        if (!::GetRegKeyInt(BASCFG_FILE, SECTION_BASSYS, "MPEG4VideoWidth", 0, &nValue))
        {
            tEncparam.m_wVideoWidth = EnVidFmt[0][11];
        }
        else
        {
            tEncparam.m_wVideoWidth = (u16)nValue;
        }
    #endif
        break;

    case MEDIA_TYPE_H261: //261
        // OspPrintf(TRUE, FALSE, "[Vmp Encode]The Encode format: H261, %d\n", MEDIA_TYPE_H261);
        tEncparam.m_byEncType = (u8)EnVidFmt[1][0];
        if (!::GetRegKeyInt(BASCFG_FILE, SECTION_BASSYS, "261BitrateControlMode", 0, &nValue))
        {
            tEncparam.m_byRcMode = (u8)EnVidFmt[1][1];
        }
        else
        {
            tEncparam.m_byRcMode = (u8)nValue;
        }

        if (!::GetRegKeyInt(BASCFG_FILE, SECTION_BASSYS, "261VideoEcdIKeyRate", 0, &nValue))
        {
            tEncparam.m_byMaxKeyFrameInterval = EnVidFmt[1][2];
        }
        else
        {
            tEncparam.m_byMaxKeyFrameInterval = nValue;
        }

        if (!::GetRegKeyInt(BASCFG_FILE, SECTION_BASSYS, "261VideoEcdQualMaxValu", 0, &nValue))
        {
            tEncparam.m_byMaxQuant = (u8)EnVidFmt[1][3];
        }
        else
        {
            tEncparam.m_byMaxQuant = (u8)nValue;
        }

        if (!::GetRegKeyInt(BASCFG_FILE, SECTION_BASSYS, "261VideoEcdQualMinValu", 0, &nValue))
        {
            tEncparam.m_byMinQuant = (u8)EnVidFmt[1][4];
        }
        else
        {
            tEncparam.m_byMinQuant = (u8)nValue;
        }

        if (!::GetRegKeyInt(BASCFG_FILE, SECTION_BASSYS, "261EncodeBitRate", 0, &nValue))
        {
            tEncparam.m_wBitRate = EnVidFmt[1][5];
        }
        else
        {
            tEncparam.m_wBitRate = (u16)nValue;
        }

        if (!::GetRegKeyInt(BASCFG_FILE, SECTION_BASSYS, "261BandWidth", 0, &nValue))
        {
            tEncparam.m_dwSndNetBand = 0;
        }
        else
        {
            tEncparam.m_dwSndNetBand = (u32)nValue;
        }

    #ifdef SOFT_CODEC
        tEncparam.m_byQcMode = 0;
        tEncparam.m_byQI     = 5;
        tEncparam.m_byQP     = 13;
    #else
        tEncparam.m_byPalNtsc = (u8)EnVidFmt[1][6];
        tEncparam.m_byCapPort = (u8)EnVidFmt[1][7];
        if (!::GetRegKeyInt(BASCFG_FILE, SECTION_BASSYS, "261FrameRate", 0, &nValue))
        {
            tEncparam.m_byFrameRate = (u8)EnVidFmt[1][8];
        }
        else
        {
            tEncparam.m_byFrameRate = (u8)nValue;
        }

        if (!::GetRegKeyInt(BASCFG_FILE, SECTION_BASSYS, "261VideoImageQuality", 0, &nValue))
        {
            tEncparam.m_byImageQulity = (u8)EnVidFmt[1][9];
        }
        else
        {
            tEncparam.m_byImageQulity = (u8)nValue;
        }

        if (!::GetRegKeyInt(BASCFG_FILE, SECTION_BASSYS, "261VideoHeight", 0, &nValue))
        {
            tEncparam.m_wVideoHeight	= EnVidFmt[1][10];
        }
        else
        {
            tEncparam.m_wVideoHeight = (u16)nValue;
        }

        if (!::GetRegKeyInt(BASCFG_FILE, SECTION_BASSYS, "261VideoWidth", 0, &nValue))
        {
            tEncparam.m_wVideoWidth = EnVidFmt[1][11];
        }
        else
        {
            tEncparam.m_wVideoWidth = (u16)nValue;
        }
    #endif
        break;

    case MEDIA_TYPE_H262:   //262(mepg2)
        // OspPrintf(TRUE, FALSE, "[Vmp Encode]The Encode format: H262, %d\n", MEDIA_TYPE_H262);
        tEncparam.m_byEncType = (u8)EnVidFmt[2][0];
        if (!::GetRegKeyInt(BASCFG_FILE, SECTION_BASSYS, "262BitrateControlMode", 0, &nValue))
        {
            tEncparam.m_byRcMode = (u8)EnVidFmt[2][1];
        }
        else
        {
            tEncparam.m_byRcMode = (u8)nValue;
        }

        if (!::GetRegKeyInt(BASCFG_FILE, SECTION_BASSYS, "262VideoEcdIKeyRate", 0, &nValue))
        {
            tEncparam.m_byMaxKeyFrameInterval = EnVidFmt[2][2];
        }
        else
        {
            tEncparam.m_byMaxKeyFrameInterval = nValue;
        }

        if (!::GetRegKeyInt(BASCFG_FILE, SECTION_BASSYS, "262VideoEcdQualMaxValu", 0, &nValue))
        {
            tEncparam.m_byMaxQuant = (u8)EnVidFmt[2][3];
        }
        else
        {
            tEncparam.m_byMaxQuant = (u8)nValue;
        }

        if (!::GetRegKeyInt(BASCFG_FILE, SECTION_BASSYS, "262VideoEcdQualMinValu", 0, &nValue))
        {
            tEncparam.m_byMinQuant = (u8)EnVidFmt[2][4];
        }
        else
        {
            tEncparam.m_byMinQuant = (u8)nValue;
        }

        if (!::GetRegKeyInt(BASCFG_FILE, SECTION_BASSYS, "262EncodeBitRate", 0, &nValue))
        {
            tEncparam.m_wBitRate = EnVidFmt[2][5];
        }
        else
        {
            tEncparam.m_wBitRate = (u16)nValue;
        }

        if (!::GetRegKeyInt( BASCFG_FILE, SECTION_BASSYS, "262BandWidth", 0, &nValue))
        {
            tEncparam.m_dwSndNetBand = 0;
        }
        else
        {
            tEncparam.m_dwSndNetBand = (u32)nValue;
        }

    #ifdef SOFT_CODEC
        tEncparam.m_byQcMode = 0;
        tEncparam.m_byQI     = 5;
        tEncparam.m_byQP     = 13;
    #else
        tEncparam.m_byPalNtsc = (u8)EnVidFmt[2][6];
        tEncparam.m_byCapPort = (u8)EnVidFmt[2][7];
        if (!::GetRegKeyInt(BASCFG_FILE, SECTION_BASSYS, "262FrameRate", 0, &nValue))
        {
            tEncparam.m_byFrameRate = (u8)EnVidFmt[2][8];
        }
        else
        {
            tEncparam.m_byFrameRate = (u8)nValue;
        }

        if (!::GetRegKeyInt(BASCFG_FILE, SECTION_BASSYS, "262VideoImageQuality", 0, &nValue))
        {
            tEncparam.m_byImageQulity = (u8) EnVidFmt[2][9];
        }
        else
        {
            tEncparam.m_byImageQulity = (u8)nValue;
        }

        if (!::GetRegKeyInt( BASCFG_FILE, SECTION_BASSYS, "262VideoHeight", 0, &nValue))
        {
            tEncparam.m_wVideoHeight	= EnVidFmt[2][10];
        }
        else
        {
            tEncparam.m_wVideoHeight = (u16)nValue;
        }

        if (!::GetRegKeyInt(BASCFG_FILE, SECTION_BASSYS, "262VideoWidth", 0, &nValue))
        {
            tEncparam.m_wVideoWidth = EnVidFmt[2][11];
        }
        else
        {
            tEncparam.m_wVideoWidth = (u16)nValue;
        }
    #endif
        break;

    case MEDIA_TYPE_H263:  //263
        // OspPrintf(TRUE, FALSE, "[Vmp Encode]The Encode format: H263, %d\n", MEDIA_TYPE_H263);
        tEncparam.m_byEncType = (u8)EnVidFmt[3][0];
        if (!::GetRegKeyInt(BASCFG_FILE, SECTION_BASSYS, "263BitrateControlMode", 0, &nValue))
        {
            tEncparam.m_byRcMode = (u8)EnVidFmt[3][1];
        }
        else
        {
            tEncparam.m_byRcMode = (u8)nValue;
        }

        if (!::GetRegKeyInt(BASCFG_FILE, SECTION_BASSYS, "263VideoEcdIKeyRate", 0, &nValue))
        {
            tEncparam.m_byMaxKeyFrameInterval = EnVidFmt[3][2];
        }
        else
        {
            tEncparam.m_byMaxKeyFrameInterval = nValue;
        }

        if (!::GetRegKeyInt(BASCFG_FILE, SECTION_BASSYS, "263VideoEcdQualMaxValu", 0, &nValue))
        {
            tEncparam.m_byMaxQuant = (u8)EnVidFmt[3][3];
        }
        else
        {
            tEncparam.m_byMaxQuant = (u8)nValue;
        }

        if (!::GetRegKeyInt(BASCFG_FILE, SECTION_BASSYS, "263VideoEcdQualMinValu", 0, &nValue))
        {
            tEncparam.m_byMinQuant = (u8)EnVidFmt[3][4];
        }
        else
        {
            tEncparam.m_byMinQuant = (u8)nValue;
        }

        if (!::GetRegKeyInt(BASCFG_FILE, SECTION_BASSYS, "263EncodeBitRate", 0, &nValue))
        {
            tEncparam.m_wBitRate = EnVidFmt[3][5];
        }
        else
        {
            tEncparam.m_wBitRate = (u16)nValue;
        }

        if (!::GetRegKeyInt( BASCFG_FILE, SECTION_BASSYS, "263BandWidth", 0, &nValue))
        {
            tEncparam.m_dwSndNetBand = 0;
        }
        else
        {
            tEncparam.m_dwSndNetBand = (u32)nValue;
        }

    #ifdef SOFT_CODEC
        tEncparam.m_byQcMode = 0;
        tEncparam.m_byQI     = 5;
        tEncparam.m_byQP     = 13;
    #else
        tEncparam.m_byPalNtsc = (u8)EnVidFmt[3][6];
        tEncparam.m_byCapPort = (u8)EnVidFmt[3][7];
        if (!::GetRegKeyInt(BASCFG_FILE, SECTION_BASSYS, "263FrameRate", 0, &nValue))
        {
            tEncparam.m_byFrameRate = (u8)EnVidFmt[3][8];
        }
        else
        {
            tEncparam.m_byFrameRate = (u8)nValue;
        }

        if (!::GetRegKeyInt(BASCFG_FILE, SECTION_BASSYS, "263VideoImageQuality", 0, &nValue))
        {
            tEncparam.m_byImageQulity = (u8) EnVidFmt[3][9];
        }
        else
        {
            tEncparam.m_byImageQulity = (u8)nValue;
        }

        if (!::GetRegKeyInt( BASCFG_FILE, SECTION_BASSYS, "263VideoHeight", 0, &nValue))
        {
            tEncparam.m_wVideoHeight	= EnVidFmt[3][10];
        }
        else
        {
            tEncparam.m_wVideoHeight = (u16)nValue;
        }

        if (!::GetRegKeyInt(BASCFG_FILE, SECTION_BASSYS, "263VideoWidth", 0, &nValue))
        {
            tEncparam.m_wVideoWidth = EnVidFmt[3][11];
        }
        else
        {
            tEncparam.m_wVideoWidth = (u16)nValue;
        }
    #endif
        break;

    case MEDIA_TYPE_H263PLUS:  //263+
        // OspPrintf(TRUE, FALSE, "[Vmp Encode]The Encode format: H263+, %d\n", MEDIA_TYPE_H263PLUS);
        tEncparam.m_byEncType = (u8)EnVidFmt[4][0];
        if (!::GetRegKeyInt(BASCFG_FILE, SECTION_BASSYS, "263PLUSBitrateControlMode", 0, &nValue))
        {
            tEncparam.m_byRcMode = (u8)EnVidFmt[4][1];
        }
        else
        {
            tEncparam.m_byRcMode = (u8)nValue;
        }

        if (!::GetRegKeyInt(BASCFG_FILE, SECTION_BASSYS, "263PLUSVideoEcdIKeyRate", 0, &nValue))
        {
            tEncparam.m_byMaxKeyFrameInterval = EnVidFmt[4][2];
        }
        else
        {
            tEncparam.m_byMaxKeyFrameInterval = nValue;
        }

        if (!::GetRegKeyInt(BASCFG_FILE, SECTION_BASSYS, "263PLUSVideoEcdQualMaxValu", 0, &nValue))
        {
            tEncparam.m_byMaxQuant = (u8)EnVidFmt[4][3];
        }
        else
        {
            tEncparam.m_byMaxQuant = (u8)nValue;
        }

        if (!::GetRegKeyInt(BASCFG_FILE, SECTION_BASSYS, "263PLUSVideoEcdQualMinValu", 0, &nValue))
        {
            tEncparam.m_byMinQuant = (u8)EnVidFmt[4][4];
        }
        else
        {
            tEncparam.m_byMinQuant = (u8)nValue;
        }

        if (!::GetRegKeyInt(BASCFG_FILE, SECTION_BASSYS, "263PLUSEncodeBitRate", 0, &nValue))
        {
            tEncparam.m_wBitRate = EnVidFmt[4][5];
        }
        else
        {
            tEncparam.m_wBitRate = (u16)nValue;
        }

        if (!::GetRegKeyInt( BASCFG_FILE, SECTION_BASSYS, "263PLUSBandWidth", 0, &nValue))
        {
            tEncparam.m_dwSndNetBand = 0;
        }
        else
        {
            tEncparam.m_dwSndNetBand = (u32)nValue;
        }

    #ifdef SOFT_CODEC
        tEncparam.m_byQcMode = 0;
        tEncparam.m_byQI     = 5;
        tEncparam.m_byQP     = 13;
    #else
        tEncparam.m_byPalNtsc = (u8)EnVidFmt[4][6];
        tEncparam.m_byCapPort = (u8)EnVidFmt[4][7];
        if (!::GetRegKeyInt(BASCFG_FILE, SECTION_BASSYS, "263PLUSFrameRate", 0, &nValue))
        {
            tEncparam.m_byFrameRate = (u8)EnVidFmt[4][8];
        }
        else
        {
            tEncparam.m_byFrameRate = (u8)nValue;
        }

        if (!::GetRegKeyInt(BASCFG_FILE, SECTION_BASSYS, "263PLUSVideoImageQuality", 0, &nValue))
        {
            tEncparam.m_byImageQulity = (u8) EnVidFmt[4][9];
        }
        else
        {
            tEncparam.m_byImageQulity = (u8)nValue;
        }

        if (!::GetRegKeyInt( BASCFG_FILE, SECTION_BASSYS, "263PLUSVideoHeight", 0, &nValue))
        {
            tEncparam.m_wVideoHeight	= EnVidFmt[4][10];
        }
        else
        {
            tEncparam.m_wVideoHeight = (u16)nValue;
        }

        if (!::GetRegKeyInt(BASCFG_FILE, SECTION_BASSYS, "263PLUSVideoWidth", 0, &nValue))
        {
            tEncparam.m_wVideoWidth = EnVidFmt[4][11];
        }
        else
        {
            tEncparam.m_wVideoWidth = (u16)nValue;
        }
    #endif
        break;

    case MEDIA_TYPE_H264:  //264
        // OspPrintf(TRUE, FALSE, "[Vmp Encode]The Encode format: H264, %d\n", MEDIA_TYPE_H264);
        tEncparam.m_byEncType = (u8)EnVidFmt[5][0];
        if (!::GetRegKeyInt(BASCFG_FILE, SECTION_BASSYS, "264SBitrateControlMode", 0, &nValue))
        {
            tEncparam.m_byRcMode = (u8)EnVidFmt[5][1];
        }
        else
        {
            tEncparam.m_byRcMode = (u8)nValue;
        }

        if (!::GetRegKeyInt(BASCFG_FILE, SECTION_BASSYS, "264VideoEcdIKeyRate", 0, &nValue))
        {
            tEncparam.m_byMaxKeyFrameInterval = EnVidFmt[5][2];
        }
        else
        {
            tEncparam.m_byMaxKeyFrameInterval = nValue;
        }

        if (!::GetRegKeyInt(BASCFG_FILE, SECTION_BASSYS, "264VideoEcdQualMaxValu", 0, &nValue))
        {
            tEncparam.m_byMaxQuant = (u8)EnVidFmt[5][3];
        }
        else
        {
            tEncparam.m_byMaxQuant = (u8)nValue;
        }

        if (!::GetRegKeyInt(BASCFG_FILE, SECTION_BASSYS, "264VideoEcdQualMinValu", 0, &nValue))
        {
            tEncparam.m_byMinQuant = (u8)EnVidFmt[5][4];
        }
        else
        {
            tEncparam.m_byMinQuant = (u8)nValue;
        }

        if (!::GetRegKeyInt(BASCFG_FILE, SECTION_BASSYS, "264EncodeBitRate", 0, &nValue))
        {
            tEncparam.m_wBitRate = EnVidFmt[5][5];
        }
        else
        {
            tEncparam.m_wBitRate = (u16)nValue;
        }

        if (!::GetRegKeyInt( BASCFG_FILE, SECTION_BASSYS, "264BandWidth", 0, &nValue))
        {
            tEncparam.m_dwSndNetBand = 0;
        }
        else
        {
            tEncparam.m_dwSndNetBand = (u32)nValue;
        }

    #ifdef SOFT_CODEC
        tEncparam.m_byQcMode = 0;
        tEncparam.m_byQI     = 5;
        tEncparam.m_byQP     = 13;
    #else
        tEncparam.m_byPalNtsc = (u8)EnVidFmt[5][6];
        tEncparam.m_byCapPort = (u8)EnVidFmt[5][7];
        if (!::GetRegKeyInt(BASCFG_FILE, SECTION_BASSYS, "264FrameRate", 0, &nValue))
        {
            tEncparam.m_byFrameRate = (u8)EnVidFmt[5][8];
        }
        else
        {
            tEncparam.m_byFrameRate = (u8)nValue;
        }

        if (!::GetRegKeyInt(BASCFG_FILE, SECTION_BASSYS, "264SVideoImageQuality", 0, &nValue))
        {
            tEncparam.m_byImageQulity = 0; // (u8) EnVidFmt[5][9];  // 目前h264只支持速度优先
        }
        else
        {
            tEncparam.m_byImageQulity = (u8)nValue;
        }

        if (!::GetRegKeyInt( BASCFG_FILE, SECTION_BASSYS, "264VideoHeight", 0, &nValue))
        {
            tEncparam.m_wVideoHeight	= EnVidFmt[5][10];
        }
        else
        {
            tEncparam.m_wVideoHeight = (u16)nValue;
        }

        if (!::GetRegKeyInt(BASCFG_FILE, SECTION_BASSYS, "264VideoWidth", 0, &nValue))
        {
            tEncparam.m_wVideoWidth = EnVidFmt[5][11];
        }
        else
        {
            tEncparam.m_wVideoWidth = (u16)nValue;
        }
    #endif
        break;

    default:
        break;
    }

    return;
}

/*====================================================================
	函数  : GetDefaultDecParam
	功能  : 根据媒体类型取相应解码的参数
	输入  : mediatype: 媒体类型, 
			tParam   : 保存参数
	输出  : 无
	返回  : 无
	注    : 
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
	05/04/11	3.6		    liuhuiyun	  注释整理
    07/05/17    4.0         顾振华        整理
====================================================================*/
void CBasCfg::GetDefaultDecParam( u8 byMediatype, TVideoDecParam& tParam )
{
	s32 nValue;
	switch( byMediatype )
	{
		case MEDIA_TYPE_MP4:
		    if ( !::GetRegKeyInt( BASCFG_FILE, SECTION_BASSYS, "MPEG4StartBuf", 0, &nValue ) )
            {	tParam.dwStartRecvBufs = 1; }
		    else
            {	tParam.dwStartRecvBufs   = (u32)nValue; }
			    
		    if ( !::GetRegKeyInt( BASCFG_FILE, SECTION_BASSYS, "MPEG4FastBuf", 0, &nValue ) )
            {	tParam.dwFastRecvBufs = 8; }
		    else
            {	tParam.dwFastRecvBufs   = (u32)nValue; }
			    
		    if ( !::GetRegKeyInt( BASCFG_FILE, SECTION_BASSYS, "MPEG4PostPrcLvl", 0, &nValue ) )
            {	tParam.dwPostPrcLvl   = 1; }
		    else
            {	tParam.dwPostPrcLvl   = (u32)nValue;  }
		  break;
          
		case MEDIA_TYPE_H261: //261
			if ( !::GetRegKeyInt( BASCFG_FILE, SECTION_BASSYS, "261StartBuf", 0, &nValue ) )
            {	tParam.dwStartRecvBufs = 1; }
			else
            {	tParam.dwStartRecvBufs   = (u32)nValue; }
			
			if ( !::GetRegKeyInt( BASCFG_FILE, SECTION_BASSYS, "261FastBuf", 0, &nValue ) )
            {	tParam.dwFastRecvBufs = 8; }
			else
            {	tParam.dwFastRecvBufs   = (u32)nValue; }
			
			if ( !::GetRegKeyInt( BASCFG_FILE, SECTION_BASSYS, "261PostPrcLvl", 0, &nValue ) )
            {	tParam.dwPostPrcLvl   = 1; }
			else
            {	tParam.dwPostPrcLvl   = (u32)nValue; }
			break;

		case MEDIA_TYPE_H262:   //262
			if ( !::GetRegKeyInt( BASCFG_FILE, SECTION_BASSYS, "262StartBuf", 0, &nValue ) )
            {	tParam.dwStartRecvBufs = 1; }
			else
            {	tParam.dwStartRecvBufs   = (u32)nValue; }
			
			if ( !::GetRegKeyInt( BASCFG_FILE, SECTION_BASSYS, "262FastBuf", 0, &nValue ) )
            {	tParam.dwFastRecvBufs = 8; }
			else
            {	tParam.dwFastRecvBufs   = (u32)nValue; }
			
			if ( !::GetRegKeyInt( BASCFG_FILE, SECTION_BASSYS, "262PostPrcLvl", 0, &nValue ) )
            {	tParam.dwPostPrcLvl   = 1; }
			else
            {	tParam.dwPostPrcLvl   = (u32)nValue; }
			break;

		case MEDIA_TYPE_H263://263
			if ( !::GetRegKeyInt( BASCFG_FILE, SECTION_BASSYS, "263StartBuf", 0, &nValue ) )
            {	tParam.dwStartRecvBufs = 1; }
			else
            {	tParam.dwStartRecvBufs   = (u32)nValue; }
			
			if ( !::GetRegKeyInt( BASCFG_FILE, SECTION_BASSYS, "263FastBuf", 0, &nValue ) )
            {	tParam.dwFastRecvBufs = 8; }
			else
            {	tParam.dwFastRecvBufs   = (u32)nValue; }
			
			if ( !::GetRegKeyInt( BASCFG_FILE, SECTION_BASSYS, "263PostPrcLvl", 0, &nValue ) )
            {	tParam.dwPostPrcLvl   = 1; }
			else
            {	tParam.dwPostPrcLvl   = (u32)nValue; }
			break;

		case MEDIA_TYPE_H263PLUS:   //263+
			if ( !::GetRegKeyInt( BASCFG_FILE, SECTION_BASSYS, "263PLUSStartBuf", 0, &nValue ) )
            {	tParam.dwStartRecvBufs = 1; }
			else
            {	tParam.dwStartRecvBufs   = (u32)nValue; }
			
			if ( !::GetRegKeyInt( BASCFG_FILE, SECTION_BASSYS, "263PLUSFastBuf", 0, &nValue ) )
            {	tParam.dwFastRecvBufs = 8; }
			else
            {	tParam.dwFastRecvBufs   = (u32)nValue; }
			
			if ( !::GetRegKeyInt( BASCFG_FILE, SECTION_BASSYS, "263PLUSPostPrcLvl", 0, &nValue ) )
            {	tParam.dwPostPrcLvl   = 1; }
			else
            {	tParam.dwPostPrcLvl   = (u32)nValue; }
			break;
		case MEDIA_TYPE_H264:   //264
			if ( !::GetRegKeyInt( BASCFG_FILE, SECTION_BASSYS, "264StartBuf", 0, &nValue ) )
            {	tParam.dwStartRecvBufs = 1; }
			else
            {	tParam.dwStartRecvBufs   = (u32)nValue; }
			
			if ( !::GetRegKeyInt( BASCFG_FILE, SECTION_BASSYS, "264FastBuf", 0, &nValue ) )
            {	tParam.dwFastRecvBufs = 8; }
			else
            {	tParam.dwFastRecvBufs   = (u32)nValue; }
			
			if ( !::GetRegKeyInt( BASCFG_FILE, SECTION_BASSYS, "264PostPrcLvl", 0, &nValue ) )
            {	tParam.dwPostPrcLvl   = 1; }
			else
            {	tParam.dwPostPrcLvl   = (u32)nValue; }
			break;

		default:
			break;
	}
	return;
}

// END OF FILE
