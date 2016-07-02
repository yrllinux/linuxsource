
//#include "boardconfigbasic.h"
#include "mpuautotest.h"
#include "mpuvmphwtest.h"
#include "mpuvmphwtest_evid.h"
#include "mcuver.h"
#include "boardagentbasic.h"

// #ifdef _LINUX_
// #include "nipwrapper.h"
// #include "nipwrapperdef.h"
// #include "brdwrapperdef.h"
// #endif

// #ifdef _LINUX12_
// #include "brdwrapper.h"
// #endif

// #define  SERIAL_FILE_LEN                100
// #define  CONNECT_TIMER_LENGTH           (10*1000)
// #define  VERSION_LEN                    32

CMpuVmpTestClientApp g_cMpuVmpTestApp;


CMpuVmpTestClient::CMpuVmpTestClient()
{
    m_dwMpuTestDstNode = INVALID_NODE;
    m_dwMpuTestDstInst = INVALID_INS;
}

CMpuVmpTestClient::~CMpuVmpTestClient()
{
	
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
	09/06/03	v1.0		薛亮			create
====================================================================*/
void CMpuVmpTestClient::Init( CMessage *const pMsg )
{
    printf("MpuTest Initiate!!!\n");
    
	memcpy( &m_tCfg, pMsg->content, sizeof(TEqpCfg) );
	
	printf("[MpuTest Init] m_tCfg.wRcvStartPort is: %d\n", m_tCfg.wRcvStartPort);

	s32 nRet = HardMPUInit( (u32)INITMODE_All_VMP );
    if ( HARD_MULPIC_OK == nRet )
    {
        mpulog( MPU_INFO, "[Init] Init Success!\n" );
    }
    else
    {
        mpulog( MPU_CRIT, "[Init] Fail to inital the Hard Multi-picture.(Init). errcode.%d\n", nRet );
        printf("[Init] Fail to inital the Hard Multi-picture.(Init). errcode.%d\n", nRet );
		return;			
    }

    TMulCreate tMulCreate;
    tMulCreate.m_dwDSPNUM = 4;
    tMulCreate.m_dwStartDspID = 0;
    nRet = m_cHardMulPic.Create( tMulCreate );
    if ( HARD_MULPIC_OK == nRet )
    {
        mpulog( MPU_INFO, "[Init] Create HardMulPic Success!\n" );
		printf("[Init] Create HardMulPic Success!\n");
    }
    else
    {
        mpulog( MPU_CRIT, "[Init] Fail to Create. errcode.%d\n", nRet );
		printf("[Init] Fail to Create. errcode.%d\n", nRet );
        return;
    }

    u16 wRet = KdvSocketStartup();
    
    if ( MEDIANET_NO_ERROR != wRet )
    {
        mpulog( MPU_CRIT, "[Init] KdvSocketStartup failed, error: %d\n", wRet );
		printf("[Init] KdvSocketStartup failed, error: %d\n", wRet);
        return ;
    }
	
	printf("KdvSocketStartup ok!!!\n");

    u8 byLoop = 0;
    for ( ; byLoop < MAXNUM_MPUSVMP_MEMBER; byLoop++  )
    {
        m_pcMediaRcv[byLoop] = new(CKdvMediaRcv);
        if ( NULL == m_pcMediaRcv[byLoop] )
        {
            mpulog( MPU_CRIT, "new(CKdvMediaRcv) fail!\n" );
            return;
        }
		if (!InitMediaRcv(byLoop))
		{
            mpulog( MPU_CRIT, "InitMediaRcv %d fail!\n", byLoop );			
			return;
		}
    }
	
	ConnectServ();


		
    return;
}

BOOL32 CMpuVmpTestClient::ConnectServ()
{
	printf("Begin to connect Test Server!\n");

/*	BOOL bRet = TRUE;
	s8    achProfileName[64] = {0};
    memset((void*)achProfileName, 0x0, sizeof(achProfileName));
	sprintf(achProfileName, "%s/%s", DIR_CONFIG, FILE_BRDCFGDEBUG_INI);
	
	s8 achServeIpAddr[16] = {0};
	bRet = GetRegKeyString( achProfileName, SECTION_BoardConfig, KEY_ServerIpAddr, "0.0.0.0", achServeIpAddr, sizeof(achServeIpAddr) );
*/
	KillTimer(EV_C_CONNECT_TIMER);
	u32 dwServerIpAddr = inet_addr(achServIpAddr);
	m_dwMpuTestDstNode = OspConnectTcpNode(dwServerIpAddr, 60000);
	
	if( !OspIsValidTcpNode(m_dwMpuTestDstNode) )
	{
		printf("[ConnectServ] connect to Server %s failed!\n",achServIpAddr);
		mpulog(MPU_CRIT, "[ConnectServ] connect to Server %s failed!\n",achServIpAddr);
		SetTimer(EV_C_CONNECT_TIMER,MPU_CONNETC_TIMEOUT);
		return FALSE;
	}
	m_dwMcuRcvIp = dwServerIpAddr; //Feign Mcu Ip
	::OspNodeDiscCBRegQ( m_dwMpuTestDstNode, GetAppID(), GetInsID() );
	printf("Connect Server(%d) ok, DstNode is %d!\n", m_dwMcuRcvIp, m_dwMpuTestDstNode);
	SetTimer(EV_C_REGISTER_TIMER,MPU_REGISTER_TIMEOUT);
	return TRUE;
}

void CMpuVmpTestClient::RegisterSrv(void)
{

	printf("Enter RegisterSrv!\n");
	//发送登陆消息: ev_MPUTest_REQ 消息体：
	//	u8 测试的模式（VMP/BAS） + 
	//	u8 *31  空
	//	+ 用户信息 + 设备信息
    u8 abyTemp[sizeof(u8)*32 + sizeof(CLoginRequest) + sizeof(CDeviceInfo)];
    memset(abyTemp, 0, sizeof(abyTemp));
    
	abyTemp[0] = TYPE_MPUSVMP;
    CLoginRequest cLoginReq;
    cLoginReq.Empty();
    cLoginReq.SetName("admin");
    cLoginReq.SetPassword("admin");
    memcpy(abyTemp + sizeof(u8)*32, &cLoginReq, sizeof(CLoginRequest));
    
    CDeviceInfo cDeviceinfo;
	memset( &cDeviceinfo, 0x0, sizeof( cDeviceinfo ) );
#ifdef _LINUX_
    SetMpuDeviceInfo(cDeviceinfo);
#endif
    memcpy(abyTemp + sizeof(u8)*32 + sizeof(CLoginRequest), &cDeviceinfo, sizeof(CDeviceInfo));
    post( MAKEIID( MPU_SERVER_APPID, 1 ), 
		C_S_MPULOGIN_REQ, 
		abyTemp, 
        sizeof(u8) * 32  + sizeof(CLoginRequest) + sizeof(CDeviceInfo),
        m_dwMpuTestDstNode
		);

}

#ifdef _LINUX_
/*====================================================================
  函 数 名： SetMpuDeviceInfo
  功    能： 设置设备信息
  算法实现： 
  全局变量： 
  参    数： CDeviceInfo &cDeviceInfo
  返 回 值： 
  --------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  6/3/2009		1.0			薛亮					create
======================================================================*/
void CMpuVmpTestClient::SetMpuDeviceInfo(CDeviceInfo &cDeviceInfo)
{
	s8 gs_VersionBuf[128] = {0};
    {
        strcpy(gs_VersionBuf, KDV_MCU_PREFIX);
		
        s8 achMon[16] = {0};
        u32 byDay = 0;
        u32 byMonth = 0;
        u32 wYear = 0;
        s8 achFullDate[24] = {0};
		
        s8 achDate[32] = {0};
        sprintf(achDate, "%s", __DATE__);
        StrUpper(achDate);
		
        sscanf(achDate, "%s %d %d", achMon, &byDay, &wYear );
		
        if ( 0 == strcmp( achMon, "JAN") )		 
            byMonth = 1;
        else if ( 0 == strcmp( achMon, "FEB") )
            byMonth = 2;
        else if ( 0 == strcmp( achMon, "MAR") )
            byMonth = 3;
        else if ( 0 == strcmp( achMon, "APR") )		 
            byMonth = 4;
        else if ( 0 == strcmp( achMon, "MAY") )
            byMonth = 5;
        else if ( 0 == strcmp( achMon, "JUN") )
            byMonth = 6;
        else if ( 0 == strcmp( achMon, "JUL") )
            byMonth = 7;
        else if ( 0 == strcmp( achMon, "AUG") )
            byMonth = 8;
        else if ( 0 == strcmp( achMon, "SEP") )		 
            byMonth = 9;
        else if ( 0 == strcmp( achMon, "OCT") )
            byMonth = 10;
        else if ( 0 == strcmp( achMon, "NOV") )
            byMonth = 11;
        else if ( 0 == strcmp( achMon, "DEC") )
            byMonth = 12;
        else
            byMonth = 0;
		
        if ( byMonth != 0 )
        {
            sprintf(achFullDate, "%04d%02d%02d", wYear, byMonth, byDay);
            sprintf(gs_VersionBuf, "%s%s", KDV_MCU_PREFIX, achFullDate);        
        }
        else
        {
            // for debug information
            sprintf(gs_VersionBuf, "%s%s", KDV_MCU_PREFIX, achFullDate);        
        }
    }
	
    s8* pSoftVer = (s8*)gs_VersionBuf;
// 	s8* pSoftVer = (s8*)VER_MPU;
	cDeviceInfo.setSoftVersion( pSoftVer );

    //MAC, IP, SubMask
	s8 achMac[VERSION_LEN];
	memset( achMac, 0x0, sizeof(achMac) );
	u8 byEthId = 0;
	u8 byState = 0;
	//判断用前(外/0)网口还是后(内/1)网口，默认用外网口
	for(; byEthId < 2; byEthId ++)
	{
		if( ERROR == BrdGetEthLinkStat(byEthId, &byState) )
		{
			printf("[SetMpuDeviceInfo] Call BrdGetEthLinkStat(%u) ERROR!\n", byEthId);
			break;
		}
		if( byState == 1 )
		{
			break;
		}
	}
	
	TBrdEthParam tBrdEthParam;
	memset( &tBrdEthParam, 0x0, sizeof(tBrdEthParam) );
    BrdGetEthParam( byEthId, &tBrdEthParam );
    
	u32 dwIp = tBrdEthParam.dwIpAdrs;
    u32 dwMask = tBrdEthParam.dwIpMask; 
	sprintf(achMac,
        "%02X-%02X-%02X-%02X-%02X-%02X",
        tBrdEthParam.byMacAdrs[0],
        tBrdEthParam.byMacAdrs[1],
        tBrdEthParam.byMacAdrs[2],
        tBrdEthParam.byMacAdrs[3],
        tBrdEthParam.byMacAdrs[4],
        tBrdEthParam.byMacAdrs[5]
        ); 
	cDeviceInfo.setMac( achMac );
    cDeviceInfo.setIp( dwIp );
    cDeviceInfo.setSubMask( dwMask ); 
	
	// hw version, serialNo
	TBrdE2PromInfo tBrdE2PromInfo;
	s8 achSerial[12] = { 0 };
	s8 achHWversion[VERSION_LEN];
	
	memset( achHWversion, 0x0, sizeof(achHWversion) );
	memset( &tBrdE2PromInfo, 0x0, sizeof(tBrdE2PromInfo) );
    BrdGetE2PromInfo( &tBrdE2PromInfo );
	
    sprintf( achHWversion, "%d", tBrdE2PromInfo.dwHardwareVersion );
    memcpy( achSerial, tBrdE2PromInfo.chDeviceSerial, sizeof(tBrdE2PromInfo.chDeviceSerial) );
	cDeviceInfo.setHardVersion( achHWversion );
    cDeviceInfo.setSerial( achSerial ); 
	

	return;
}
#endif

void CMpuVmpTestClient::ProcRegAck(CMessage* const pMsg)
{
	KillTimer(EV_C_REGISTER_TIMER);
	m_dwMpuTestDstInst = pMsg->srcid;

	//--------------准备编码/解码参数-------------------
	
	m_wMcuRcvStartPort = 39000;			//编码输出至的目的端口
	
	u16 wVideoWidth[]		= { 1920, 1280, 352, 704 };
	u16 wVideoHeight[]		= { 1088, 720, 288, 576 };
	u16 wMaxKeyFrameInterval[]	= {3000, 3000, 500, 1000 };

    /*lint -save -e661*/
	u8 byOutLoop = 0;
	for (;byOutLoop < 4; byOutLoop ++)
	{
		m_tMpuVmpCfg.m_tVideoEncParam[byOutLoop].m_wVideoHeight = wVideoHeight[byOutLoop]; 
		m_tMpuVmpCfg.m_tVideoEncParam[byOutLoop].m_wVideoWidth  = wVideoWidth[byOutLoop];
		m_tMpuVmpCfg.m_tVideoEncParam[byOutLoop].m_byEncType    = MEDIA_TYPE_H264;
		m_tMpuVmpCfg.m_tVideoEncParam[byOutLoop].m_wBitRate     = 2000;
		m_tMpuVmpCfg.m_tVideoEncParam[byOutLoop].m_byMaxKeyFrameInterval = wMaxKeyFrameInterval[byOutLoop];
		m_tMpuVmpCfg.m_tVideoEncParam[byOutLoop].m_byMaxQuant	= 50;
		m_tMpuVmpCfg.m_tVideoEncParam[byOutLoop].m_byMinQuant	= 15;
		m_tMpuVmpCfg.m_tVideoEncParam[byOutLoop].m_byFrameRate  = 30;
		m_tMpuVmpCfg.m_adwMaxSendBand[byOutLoop] = (u32)( m_tMpuVmpCfg.m_tVideoEncParam[byOutLoop].m_wBitRate * 2 + 100 );
		
		mpulog( MPU_INFO, "Follows are the params we used to start the vmp:\n\n" );
		mpulog( MPU_INFO, "/*----------  video channel.%d  -----------*/\n", byOutLoop );
		m_tMpuVmpCfg.PrintEncParam( byOutLoop );
		mpulog( MPU_INFO, "/*----------------------------------------*/\n\n" );

	}
	/*lint -restore*/
	//风格
	m_tParam[0].m_byVMPStyle = VMP_STYLE_TWENTY;
	m_byHardStyle = ConvertVcStyle2HardStyle(m_tParam[0].m_byVMPStyle);
	m_cHardMulPic.SetMulPicType(m_byHardStyle);
	
	m_tParam[0].m_byMemberNum	= 20;
	m_tParam[0].m_byEncType		= MEDIA_TYPE_H264;

	u8 byInLoop =0;
	for(; byInLoop < MAXNUM_MPUSVMP_MEMBER; byInLoop ++)
	{
		m_tParam[0].m_tDoublePayload[byInLoop].SetRealPayLoad(106);
		m_tParam[0].m_tDoublePayload[byInLoop].SetActivePayload(106);

		m_tParam[0].m_atMtMember[byInLoop].SetMcuId(LOCAL_MCUIDX);
		m_tParam[0].m_atMtMember[byInLoop].SetMtId(byInLoop+1);
	}

    u8 byNeedPrs = 0;

	// 添加合成成员
	if ( StartNetRcvSnd(byNeedPrs) )
	{
		// 开始合成
		s32 nRet = m_cHardMulPic.StartMerge( &m_tMpuVmpCfg.m_tVideoEncParam[0], MAXNUM_MPUSVMP_CHANNEL );
		if ( -1/*HARD_MULPIC_ERROR*/ == nRet )
		{
			printf(" startmerge correct, Error code: %u\n", nRet);
			mpulog( MPU_CRIT, " startmerge correct, Error code: %u\n", nRet );
		}
		else
		{
			printf("StartMerge succeed!\n");
			mpulog( MPU_INFO, "StartMerge succeed!\n" );
		}
        
		//zbq [03/14/2009] Merge后再尝试remove一次
		u8 byMaxChnNum = m_tMpuVmpCfg.GetVmpChlNumByStyle(m_tParam[0].m_byVMPStyle);
		u8 byChnNo = 0;
		for ( byChnNo = 0; byChnNo < byMaxChnNum; byChnNo++ )
		{
			if ( m_tParam[0].m_atMtMember[byChnNo].IsNull() )
			{
				nRet = m_cHardMulPic.RemoveChannel( byChnNo );
				if ( HARD_MULPIC_OK == nRet)
				{
					m_bAddVmpChannel[byChnNo] = FALSE;
				}
				mpulog( MPU_INFO, "[MsgStartVidMixProc] Remove channel-%u again, ret.%u(%u is correct)!\n",  byChnNo, nRet, HARD_MULPIC_OK );
			}
            else
            {
                nRet = m_cHardMulPic.AddChannel( byChnNo ); 
                if ( HARD_MULPIC_OK != nRet )
                {
                    mpulog(MPU_CRIT, "[MsgStartVidMixProc] add Chn.%d failed, ret=%u!\n", byChnNo, nRet);
                }
                else
                {
                    m_bAddVmpChannel[byChnNo] = TRUE;
                }
                mpulog( MPU_INFO, "Add channel-%u return %u(%u is correct)!\n", byChnNo, nRet, HARD_MULPIC_OK );
                mpulog( MPU_INFO, "RealPayLoad: %d, ActivePayload: %d\n",
                    m_tParam[0].m_tDoublePayload[byChnNo].GetRealPayLoad(),
                    m_tParam[0].m_tDoublePayload[byChnNo].GetActivePayload() );
            }
		}
	}
	else
	{
		printf("StargMerge failed!\n");
		return;
	}

	//通知测试服务器mpu准备就绪
	post( MAKEIID( MPU_SERVER_APPID, 1 ), 
		C_S_MPUREADY_NOTIF, 
		NULL, 0, m_dwMpuTestDstNode
		);
}


void CMpuVmpTestClient::ProDisconnect(CMessage* const pMsg)
{
	
	if ( NULL == pMsg )  
    {
        printf( "[ProDisconnect] message's pointer is Null\n" );
        return;
    }

	u32 dwNode = *(u32*)pMsg->content;

	if( dwNode != m_dwMpuTestDstNode)
	{
		printf( "[ProDisconnect] the Node.%d isn't equal to m_dwMpuTestDstNode.%d\n",dwNode, m_dwMpuTestDstNode );
		return;
	}

	m_dwMpuTestDstInst = INVALID_INS;	
    m_bConnected = FALSE;
    
    if(INVALID_NODE == m_dwMpuTestDstNode)
	{
		OspPrintf( TRUE, FALSE, "[CMpuVmpTestClient] ProDisconnect: m_dwHduAutoTestDstNode is invalid!\n" );
        return;   
	}
	
    OspDisconnectTcpNode(m_dwMpuTestDstNode);
	
    m_dwMpuTestDstNode = INVALID_NODE;
	
	// stop merge
	s32 nRet = m_cHardMulPic.StopMerge();
	if (HARD_MULPIC_OK != nRet )
	{
		printf( "[ProDisconnect] stop merge return failed!\n");
	}
	
	memset( m_bAddVmpChannel, 0, sizeof(m_bAddVmpChannel) );
	
	//重连
	SetTimer( EV_C_CONNECT_TIMER, MPU_CONNETC_TIMEOUT );

    return;
}

void CMpuVmpTestClient::ChangeTestMode(u8 byMode)
{
	if( byMode != TYPE_MPUSVMP && byMode != TYPE_MPUBAS)
	{
		printf("[ChangeTestMode] Unexpected Mode.%d!\n", byMode);
		return;
	}
	printf("Change to Test Mode: %u(Vmp:11  --- bas:13)\n wait for rebooting...\n", byMode);
	
	//转模式测试
	s8   achProfileName[64] = {0};
    memset((void*)achProfileName, 0x0, sizeof(achProfileName));
    sprintf(achProfileName, "%s/%s", DIR_CONFIG, FILE_BRDCFGDEBUG_INI);
	
	BOOL32 bRet;
    s32  sdwTest = byMode;	
    bRet = SetRegKeyInt( achProfileName, SECTION_BoardDebug, KEY_TestMode, sdwTest );
    if( !bRet )  
	{
		printf( "[RestoreDefault] Wrong profile while reading %s%s!\n", SECTION_BoardDebug, KEY_TestMode );
		return;
	}
	//reset
#ifndef WIN32
	OspDelay(2000);
	BrdHwReset();
#endif
    return;

}


void CMpuVmpTestClient::RestoreDefault(void)
{
#ifndef WIN32

	for( u8 bylp = 0; bylp < 3; bylp ++)
	{

	}
	BOOL32 bRet = FALSE;
    bRet = BrdClearE2promTestFlag();	//清测试标志位
	u8 bySucessRestore = bRet?0:1;
	s32 sdwRet;
    sdwRet = post(m_dwMpuTestDstInst, C_S_MPURESTORE_NOTIF, &bySucessRestore, sizeof(u8), m_dwMpuTestDstNode);
    if ( sdwRet != OSP_OK )
    {
        printf( "[RestoreDefault] post failed!\n" );
    }
	else
	{
		printf( "[RestoreDefault] post sucessful!\n" );
	}
	//reset
	OspDelay(2000);
	BrdHwReset();
#endif
    return;
}
/*lint -save -e715*/
void CMpuVmpTestClient::DaemonInstanceEntry(CMessage* const pMsg, CApp* pcApp )
{
	switch (pMsg->event)
	{
	case EV_C_CHANGEAUTOTEST_CMD:
		DaemonProcChangeAutoTestCmd( pMsg );
		break;

	default:
		printf("DeamonInstanceEntry: wrong message type %u!\n", pMsg->event );
		break;
	}
}
/*lint -restore*/

/*====================================================================
  函 数 名： InstanceEntry
  功    能： 普通实例入口
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void
  --------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2009/06/03    4.6		    薛亮                  创建
======================================================================*/
void CMpuVmpTestClient::InstanceEntry( CMessage* const pMsg )
{
	printf("enter InstanceEntry! message type %d\n",pMsg->event );

    switch ( pMsg->event )
    {
	case EV_C_INIT:
		Init( pMsg );		
		break;

	case EV_C_CONNECT_TIMER:
		ConnectServ();
		break;

	case EV_C_REGISTER_TIMER:
		RegisterSrv();
        break;

	case S_C_MPULOGIN_ACK:
		ProcRegAck(pMsg);		
		break;

	case S_C_MPULOGIN_NACK:
		break;

	case S_C_MPURESTORE_CMD:
		RestoreDefault();
		break;

	case S_C_FASTUPDATEPIC_CMD:
		MsgFastUpdatePicProc(pMsg);
		break;

	case OSP_DISCONNECT:
		ProDisconnect(pMsg);
		break;

	case S_C_CHANGETESTMODE_CMD:
		{
			CServMsg cServMsg( pMsg->content, pMsg->length );
			u8 byMode = *cServMsg.GetMsgBody();
			ChangeTestMode(byMode);
			break;
		}
		
	default:
		printf("wrong message type %u!\n", pMsg->event );
		break;

	}

    return;
}

void CMpuVmpTestClient::DaemonProcChangeAutoTestCmd( CMessage* const pMsg )
{
	if ( NULL == pMsg)
	{
		OspPrintf( TRUE, FALSE, "[DaemonProcChangeAutoTestCmd] pMsg is Null!\n" );
		return;
	}
#ifdef _LINUX_
	

	s32 sdwAutoTest = *(s32*)pMsg->content;
	if( sdwAutoTest != 0)
	{
		BrdSetE2promTestFlag();
		OspPrintf( TRUE, FALSE, "[DaemonProcChangeAutoTestCmd] Set e2prom test flag 1\n" );
	}
	else 
	{
		BrdClearE2promTestFlag();
		OspPrintf( TRUE, FALSE, "[DaemonProcChangeAutoTestCmd] Set e2prom test flag 0\n" );
	}
#endif
	
	// 	BrdHwReset();
}