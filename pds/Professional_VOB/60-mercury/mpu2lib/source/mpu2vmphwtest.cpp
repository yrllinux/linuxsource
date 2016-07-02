/*****************************************************************************
   模块名      : mpu2lib
   文件名      : mpu2vmphwtest.cpp
   相关文件    : 
   文件实现功能: mpu2lib vmp生产测试业务实现
   作者        : 周翼亮
   版本        : V4.7  Copyright(C) 2011 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期       版本        修改人      修改内容
   2011/12/07    4.7         周翼亮      创建
******************************************************************************/

#include "mpuautotest.h"
#include "mpu2vmphwtest.h"
#include "mcuver.h"
#include "boardagentbasic.h"
#include "evmpu2.h"

/*lint -save -esym(1512, CUserFullInfo)*/
/*lint -save -esym(1512, CExUsrInfo)*/
CMpu2VmpTestClientApp g_cMpuVmpTestApp;

#ifdef _8KI_
#define _8KI_LEFTCARD      (u8)0
#define _8KI_RIGHTCARD     (u8)1
#endif

/*====================================================================
	函数  : CMpu2VmpTestClient
	功能  : 生产测试构造函数
	输入  : 
	输出  : 无
	返回  : 无
	注    :
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
   2011/12/07		v4.7		zhouyiliang			create
====================================================================*/
CMpu2VmpTestClient::CMpu2VmpTestClient()
{
    m_dwMpuTestDstNode = INVALID_NODE;
    m_dwMpuTestDstInst = INVALID_INS;
	m_by8KICurTestCard = 0xFF;
	//TODO:等实际mpu2板子过来再详细修改，先定义灯的基本框架
	//mpu2有以下define了以下5个灯，但是驱动确认只有两个灯需要点，link和alarm
// 	u8 byLedRun; 
//     u8 byLedLINK;
//     u8 byLedALM;
//     u8 byLedLINK0;	
//     u8 byLedLINK1;
	u8 abyLedId[MPU2_LED_NUM] = { LED_SYS_ALARM,LED_SYS_LINK };//应该根据实际定义
	memcpy(m_abyLedId,abyLedId,MPU2_LED_NUM);
}




/*====================================================================
	函数  : Clear
	功能  : 清空当前instance的状态
	输入  : 
	输出  : 无
	返回  : 无
	注    :
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
	11/12/1		v4.7		zhouyiliang		create
====================================================================*/
void CMpu2VmpTestClient::Clear()
{
    u8 byLoop = 0;
    for ( ; byLoop < MAXNUM_MPU2VMP_MEMBER; byLoop++ )
    {
		if( NULL != m_pcMediaRcv[byLoop] ) 
		{
			delete m_pcMediaRcv[byLoop];
			 m_pcMediaRcv[byLoop] = NULL;
		}
       
    }
    byLoop = 0;
    for (; byLoop < MAXNUM_MPU2VMP_CHANNEL; byLoop++)
    {
		if ( NULL !=  m_pcMediaSnd[byLoop] ) 
		{
			delete  m_pcMediaSnd[byLoop];
			m_pcMediaSnd[byLoop] = NULL;
		}
        
    }
    byLoop = 0;
    for ( ; byLoop < MAXNUM_MPU2VMP_MEMBER; byLoop++ )
    {
        m_bAddVmpChannel[byLoop] = FALSE;
    }
	m_cHardMulPic.Destory();
	


       
}

/*====================================================================
	函数  : Init
	功能  : 初始化函数
	输入  : 
	输出  : 无
	返回  : 无
	注    :
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
   2011/12/07		v4.7		zhouyiliang			create
====================================================================*/
void CMpu2VmpTestClient::Init( CMessage *const pMsg )
{
	//1、参数检验 
    if ( NULL == pMsg )
    {
        ErrorLog( "[CMpu2VmpTestClient::Init] Init params cannot be NULL!\n" );
        return;
    }
    KeyLog("Mpu2Test Initiate!!!\n");
    //2、解析消息体TEqpBasicCfg，直接保存到m_tCfg
	memcpy( &m_tCfg, pMsg->content, sizeof(TEqpBasicCfg) );
	m_tCfg.wRcvStartPort = 40700;//写死40700
	KeyLog("[CMpu2VmpTestClient Init] m_tCfg.wRcvStartPort is: %d\n", m_tCfg.wRcvStartPort);

	//3、媒控层初始化
	TMultiPicCreate tMPCreate;

	tMPCreate.m_dwDevVersion = (u32)en_MPU2_Board;


	tMPCreate.m_dwMode = VMP_BASIC;
	if ( g_cMpu2VmpApp.m_byWorkMode == TYPE_MPU2VMP_ENHANCED)
	{
		tMPCreate.m_dwMode = VMP_ENHANCED;
	}
	
    s32 nRet = m_cHardMulPic.Create( tMPCreate );

    if ( HARD_MULPIC_OK == nRet )
    {
        KeyLog( "[Init] Create HardMulPic Success!\n" );
		printf("[Init] Create HardMulPic Success!\n");
    }
    else
    {
        ErrorLog( "[Init] Fail to Create. errcode.%d\n", nRet );
		printf("[Init] Fail to Create. errcode.%d\n", nRet );
        return;
    }


	//4、网络层初始化
    u16 wRet = KdvSocketStartup();
    
    if ( MEDIANET_NO_ERROR != wRet )
    {
        ErrorLog( "[Init] KdvSocketStartup failed, error: %d\n", wRet );
        return ;
    }
	
	KeyLog("KdvSocketStartup ok!!!\n");

	//5、Init接收对象
    u8 byLoop = 0;
    for ( ; byLoop < MAXNUM_MPU2VMP_MEMBER; byLoop++  )
    {
        m_pcMediaRcv[byLoop] = new(CKdvMediaRcv);
        if ( NULL == m_pcMediaRcv[byLoop] )
        {
            ErrorLog( "new(CKdvMediaRcv) fail!\n" );
			Clear();
            return;
        }
		if (!InitMediaRcv(byLoop))
		{
            ErrorLog( "InitMediaRcv %d fail!\n", byLoop );			
			Clear();
			return;
		}
    }

	//6、new发送对象
	byLoop =0;
	for (;byLoop < MAXNUM_MPU2VMP_CHANNEL;byLoop++)
	{

        m_pcMediaSnd[byLoop] = new(CKdvMediaSnd); 
        if ( NULL == m_pcMediaSnd[byLoop] )
        {
			Clear();
            ErrorLog( " new(CKdvMediaSnd) fail!\n" );
            return;
        }
    
	}
	//7、开始连测试接服务器
	ConnectServ();
	

		
    return;
}
/*====================================================================
	函数  : ConnectServ
	功能  : 连接服务器函数
	输入  : 
	输出  : 无
	返回  : 无
	注    :
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
   2011/12/07		v4.7		zhouyiliang			create
====================================================================*/
BOOL32 CMpu2VmpTestClient::ConnectServ()
{
	KillTimer(EV_C_CONNECT_TIMER);

	KeyLog("Begin to connect Test Server!\n");

	//1、连接测试服务器10.1.1.1 @60000
	u32 dwServerIpAddr = inet_addr(g_cMpuVmpTestApp.m_achServerIp);
	m_dwMpuTestDstNode = OspConnectTcpNode(dwServerIpAddr, 60000);
	//2、连接测试服务器失败，起连接的timer
	if( !OspIsValidTcpNode(m_dwMpuTestDstNode) )
	{
		ErrorLog( "[ConnectServ] connect to Server %s failed!\n",g_cMpuVmpTestApp.m_achServerIp);
		SetTimer( EV_C_CONNECT_TIMER, MPU_CONNETC_TIMEOUT );
		return FALSE;
	}
	//3、连接测试服务器成功，注册断链消息，起注册的timer
	m_dwServerRcvIp = dwServerIpAddr; 
	KeyLog("Connect Server(%d) ok, DstNode is %d!\n", m_dwServerRcvIp, m_dwMpuTestDstNode);
	::OspNodeDiscCBRegQ( m_dwMpuTestDstNode, GetAppID(), GetInsID() );
	SetTimer( EV_C_REGISTER_TIMER, MPU_REGISTER_TIMEOUT);
	return TRUE;
}
/*====================================================================
	函数  : RegisterSrv
	功能  : 往服务器注册函数
	输入  : 
	输出  : 无
	返回  : 无
	注    :
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
   2011/12/07		v4.7		zhouyiliang			create
====================================================================*/
void CMpu2VmpTestClient::RegisterSrv(void)
{

	KillTimer(EV_C_REGISTER_TIMER);

	KeyLog("Enter RegisterSrv!\n");
	//发送登陆消息: ev_MPUTest_REQ 消息体：
	//	u8 测试的模式（VMP/BAS） + 
	//	u8 *31  空
	//	+ 用户信息 + 设备信息
    u8 abyTemp[sizeof(u8)*32 + sizeof(CLoginRequest) + sizeof(CDeviceInfo)];
    memset(abyTemp, 0, sizeof(abyTemp));

	abyTemp[0] = g_cMpuVmpTestApp.m_byWorkMode;
	

#ifdef _8KI_ 
	abyTemp[1] = MCU_TYPE_KDV8000I;
#endif
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
	SetTimer( EV_C_REGISTER_TIMER, MPU_REGISTER_TIMEOUT);
}

#ifdef _LINUX_
/*====================================================================
  函 数 名： SetMpuDeviceInfo
  功    能： 设置设备信息
  算法实现： 
  全局变量： 
  参    数： CDeviceInfo &cDeviceInfo
  返 回 值： 
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
   2011/12/07		v4.7		zhouyiliang			create
====================================================================*/
void CMpu2VmpTestClient::SetMpuDeviceInfo(CDeviceInfo &cDeviceInfo)
{


	s8* pSoftVer = (s8*)VER_MPU2;
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
			ErrorLog("[SetMpuDeviceInfo] Call BrdGetEthLinkStat(%u) ERROR!\n", byEthId);
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
/*====================================================================
  函 数 名： ProcRegAck
  功    能： 注册成功处理函数
  算法实现： 
  全局变量： 
  参    数： CMessage
  返 回 值： 
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
   2011/12/07		v4.7		zhouyiliang			create
====================================================================*/
void CMpu2VmpTestClient::ProcRegAck(CMessage* const pMsg)
{
	KillTimer(EV_C_REGISTER_TIMER);
	m_dwMpuTestDstInst = pMsg->srcid;

#ifdef _8KI_ //8ki的mpu2vmp生产测试流程改变，登陆成功后要等服务器发S_C_STARTVMPTEST_REQ再初始化媒控，开始收发
	return;
#endif
	PrepareToTest();

}

/*====================================================================
  函 数 名： PrepareToTest
  功    能： 为开始测试做准备，给媒控下参，准备好开始收发
  算法实现： 
  全局变量： 
  参    数： CMessage
  返 回 值： 
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
   2012/04/19		v4.7		zhouyiliang			create
====================================================================*/
void CMpu2VmpTestClient::PrepareToTest( )
{
	//1、--------------准备编码/解码参数-------------------
	
	m_wServerRcvStartPort = 39000;			//编码输出至的目的端口
	//9路：	1080p60HP,1080p60BP,1080p30HP,1080p30BP,720p60HP,720p60BP,4cif 25,cif 25,other 25
// 	u16 wVideoWidth[]		= { 1920,1920,1920,1920, 1280,1280, 704,352, 352 };
// 	u16 wVideoHeight[]		= { 1080,1080,1080,1080,  720, 720, 576,288, 288 };
// 	u8  abyFrameRate[]      = {   60,  60,  30,  30,   60,  60,  25, 25, 25  };
// 	u8  abyProfile[]        = {   1,     0,  1,   0,    1,   0,   0,  0,  0               };
// 	u16 wMaxKeyFrameInterval[]	= {3000,3000,3000,3000,3000, 3000, 1000, 500 ,500};
	
    /*lint -save -e661*/
	//u8 byOutLoop = 0;
	//根据规格，只需编一路1080p30bp的码流，所以只需下一路参数就可以了
	// 	for (;byOutLoop < MAXNUM_MPU2VMP_CHANNEL; byOutLoop ++)
	// 	{
	// 		m_tMpuVmpCfg.m_tVideoEncParam[byOutLoop].m_wVideoHeight = wVideoHeight[byOutLoop]; 
	// 		m_tMpuVmpCfg.m_tVideoEncParam[byOutLoop].m_wVideoWidth  = wVideoWidth[byOutLoop];
	// 		m_tMpuVmpCfg.m_tVideoEncParam[byOutLoop].m_byEncType    = (byOutLoop < 8)?MEDIA_TYPE_H264:MEDIA_TYPE_MP4;
	// 		m_tMpuVmpCfg.m_tVideoEncParam[byOutLoop].m_wBitRate     = 2000;
	// 		m_tMpuVmpCfg.m_tVideoEncParam[byOutLoop].m_byMaxKeyFrameInterval = wMaxKeyFrameInterval[byOutLoop];
	// 		m_tMpuVmpCfg.m_tVideoEncParam[byOutLoop].m_byMaxQuant	= 50;
	// 		m_tMpuVmpCfg.m_tVideoEncParam[byOutLoop].m_byMinQuant	= 15;
	// 		m_tMpuVmpCfg.m_tVideoEncParam[byOutLoop].m_byFrameRate  = abyFrameRate[byOutLoop];
	// 		m_tMpuVmpCfg.m_adwMaxSendBand[byOutLoop] = (u32)( m_tMpuVmpCfg.m_tVideoEncParam[byOutLoop].m_wBitRate * 2 + 100 );
	// 		m_tMpuVmpCfg.m_tVideoEncParam[byOutLoop].m_byProfile = abyProfile[byOutLoop];
	// 		
	// 		KeyLog( "Follows are the params we used to start the vmp:\n\n" );
	// 		KeyLog( "/*----------  video channel.%d  -----------*/\n", byOutLoop );
	// 		m_tMpuVmpCfg.PrintEncParam( byOutLoop );
	// 		KeyLog( "/*----------------------------------------*/\n\n" );
	// 
	// 	}
	
	m_tMpuVmpCfg.m_tVideoEncParam[0].m_wVideoHeight = 1080;//wVideoHeight[3]; 
	m_tMpuVmpCfg.m_tVideoEncParam[0].m_wVideoWidth  = 1920;//wVideoWidth[3];
	m_tMpuVmpCfg.m_tVideoEncParam[0].m_byEncType    = MEDIA_TYPE_H264;
	m_tMpuVmpCfg.m_tVideoEncParam[0].m_wBitRate     = 3072;
	m_tMpuVmpCfg.m_tVideoEncParam[0].m_dwMaxKeyFrameInterval = 3000;//wMaxKeyFrameInterval[3];
	m_tMpuVmpCfg.m_tVideoEncParam[0].m_byMaxQuant	= 50;
	m_tMpuVmpCfg.m_tVideoEncParam[0].m_byMinQuant	= 15;
	m_tMpuVmpCfg.m_tVideoEncParam[0].m_byFrameRate  = 30;//abyFrameRate[3];
	m_tMpuVmpCfg.m_adwMaxSendBand[0] = (u32)( 3072 * 2 + 100 );
	m_tMpuVmpCfg.m_tVideoEncParam[0].m_dwProfile = 0;
	
	KeyLog( "Follows are the params we used to start the vmp:\n\n" );
	KeyLog( "/*----------  video channel.%d  -----------*/\n", 0 );
	m_tMpuVmpCfg.PrintEncParam( 0 );
	KeyLog( "/*----------------------------------------*/\n\n" );
	
	/*lint -restore*/
	//2、设置风格
	//	m_tParam[0].m_byVMPStyle = VMP_STYLE_TWENTY;
	m_byHardStyle = ConvertVcStyle2HardStyle( VMP_STYLE_TWENTYFIVE );
	m_cHardMulPic.SetMulPicType(m_byHardStyle);
	
	
	
    s32 nRet = 0;
	// 3、添加合成成员
	for (u8 byChnNo = 0; byChnNo < MAXNUM_MPU2VMP_MEMBER; byChnNo++ )
	{
        nRet = m_cHardMulPic.AddChannel( byChnNo ); 
        if ( HARD_MULPIC_OK != nRet )
        {
            ErrorLog( "[MsgStartVidMixProc] add Chn.%d failed, ret=%u!\n", byChnNo, nRet);
        }
        else
        {
            m_bAddVmpChannel[byChnNo] = TRUE;
        }
        KeyLog( "Add channel-%u return %u(%u is correct)!\n", byChnNo, nRet, HARD_MULPIC_OK );
		
	}
	//4、StartMerge
	nRet = m_cHardMulPic.SetVideoEncParam(&m_tMpuVmpCfg.m_tVideoEncParam[0], 
										1);
	nRet = m_cHardMulPic.StartMerge( );
	if ( HARD_MULPIC_OK/*HARD_MULPIC_ERROR*/ != nRet )
	{
		ErrorLog( " startmerge correct, Error code: %u\n", nRet );
		Clear();
		return;
	}
	else
	{
		KeyLog( "StartMerge succeed!\n" );
//		printf("StartMerge Succeed!\n");
	}
	//5、开始收发
	StartNetRcvSnd() ;
	
	
	//6、通知测试服务器mpu2准备就绪
	post( MAKEIID( MPU_SERVER_APPID, 1 ), 
		C_S_MPUREADY_NOTIF, 
		NULL, 0, m_dwMpuTestDstNode
		);
}

/*====================================================================
  函 数 名： ProDisconnect
  功    能： 断链处理函数
  算法实现： 
  全局变量： 
  参    数： CMessage
  返 回 值： 
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
   2011/12/07		v4.7		zhouyiliang			create
====================================================================*/
void CMpu2VmpTestClient::ProDisconnect(CMessage* const pMsg)
{
	
	if ( NULL == pMsg )  
    {
        ErrorLog( "[ProDisconnect] message's pointer is Null\n" );
        return;
    }

	u32 dwNode = *(u32*)pMsg->content;

	if( dwNode != m_dwMpuTestDstNode)
	{
		ErrorLog( "[ProDisconnect] the Node.%d isn't equal to m_dwMpuTestDstNode.%d\n",dwNode, m_dwMpuTestDstNode );
		return;
	}

	m_dwMpuTestDstInst = INVALID_INS;	
    
    if(INVALID_NODE == m_dwMpuTestDstNode)
	{
		ErrorLog( "[CMpu2VmpTestClient ProDisconnect]: m_dwHduAutoTestDstNode is invalid!\n" );
        return;   
	}
	//1、断开节点
    OspDisconnectTcpNode(m_dwMpuTestDstNode);
	
    m_dwMpuTestDstNode = INVALID_NODE;
	//2、stopmerge
	s32 nRet = m_cHardMulPic.StopMerge();
	if (HARD_MULPIC_OK != nRet )
	{
		ErrorLog("[CMpu2VmpTestClient ProDisconnect] stopmerge return failed!\n");
	}

	m_by8KICurTestCard = 0xFF;
	
	memset( m_bAddVmpChannel, 0, sizeof(m_bAddVmpChannel) );
	
	//3、重连
	SetTimer( EV_C_CONNECT_TIMER, MPU_CONNETC_TIMEOUT );

    return;
}

/*====================================================================
  函 数 名： ChangeTestMode
  功    能： 改变测试模式函数（mpu2只测vmp，应该没有用了）
  算法实现： 
  全局变量： 
  参    数： byMode：要测的模式
  返 回 值： 
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
   2011/12/07		v4.7		zhouyiliang			create
====================================================================*/
void CMpu2VmpTestClient::ChangeTestMode(u8 byMode)
{
	if( byMode != TYPE_MPUSVMP && byMode != TYPE_MPUBAS)
	{
		ErrorLog("[ChangeTestMode] Unexpected Mode.%d!\n", byMode);
		return;
	}
	KeyLog("Change to Test Mode: %u\n wait for rebooting...\n", byMode);
	
	//转模式测试
	s8   achProfileName[64] = {0};
    memset((void*)achProfileName, 0x0, sizeof(achProfileName));
    sprintf(achProfileName, "%s/%s", DIR_CONFIG, FILE_BRDCFGDEBUG_INI);
	
	BOOL32 bRet;
    s32  sdwTest = byMode;	
    bRet = SetRegKeyInt( achProfileName, SECTION_BoardDebug, KEY_TestMode, sdwTest );
    if( !bRet )  
	{
		ErrorLog( "[RestoreDefault] Wrong profile while reading %s%s!\n", SECTION_BoardDebug, KEY_TestMode );
		return;
	}
	//reset
#ifndef WIN32
	OspDelay(2000);
	BrdHwReset();
#endif
    return;

}

/*====================================================================
  函 数 名： RestoreDefault
  功    能： 清除测试标志
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： 
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
   2011/12/07		v4.7		zhouyiliang			create
====================================================================*/
void CMpu2VmpTestClient::RestoreDefault(void)
{
#ifndef WIN32


	BOOL32 bRet = FALSE;
    bRet = BrdClearE2promTestFlag();	//清测试标志位
	u8 bySucessRestore = bRet?0:1;
	s32 sdwRet;
    sdwRet = post(m_dwMpuTestDstInst, C_S_MPURESTORE_NOTIF, &bySucessRestore, sizeof(u8), m_dwMpuTestDstNode);
    if ( sdwRet != OSP_OK )
    {
        ErrorLog( "[RestoreDefault] post failed!\n" );
    }
	else
	{
		KeyLog( "[RestoreDefault] post sucessful!\n" );
	}
	//reset
	OspDelay(2000);
	BrdHwReset();
#endif
    return;
}
/*lint -save -e715*/
/*====================================================================
  函 数 名： DaemonInstanceEntry
  功    能： Daemon消息分发处理
  算法实现： 
  全局变量： 
  参    数： CMessage：消息体，CApp:模块
  返 回 值： 
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
   2011/12/07		v4.7		zhouyiliang			create
====================================================================*/
void CMpu2VmpTestClient::DaemonInstanceEntry(CMessage* const pMsg, CApp* pcApp )
{
	if (NULL == pMsg || NULL == pcApp)
	{
		ErrorLog("[DaemonInstanceEntry]wrong message recived. pcMsg is Null or pApp is Null\n ");
		return;
	}
	switch (pMsg->event)
	{
	case EV_C_CHANGEAUTOTEST_CMD:
		DaemonProcChangeAutoTestCmd( pMsg );
		break;

	default:
		ErrorLog("DeamonInstanceEntry: wrong message type %u!\n", pMsg->event );
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
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
   2011/12/07		v4.7		zhouyiliang			create
====================================================================*/
void CMpu2VmpTestClient::InstanceEntry( CMessage* const pMsg )
{
	if ( NULL == pMsg )
    {
		ErrorLog("[InstanceEntry]wrong message recived. pMsg is Null \n");
        return;
    }
	KeyLog("enter InstanceEntry! message type %d\n",pMsg->event );
    switch ( pMsg->event )
    {
		//初始化消息
		case EV_C_INIT:
			Init( pMsg );		
			break;
		//连接timer
		case EV_C_CONNECT_TIMER:
			ConnectServ();
			break;
		//注册timer
		case EV_C_REGISTER_TIMER:
			RegisterSrv();
			break;
		//测试服务器注册Ack
		case S_C_MPULOGIN_ACK:
			ProcRegAck(pMsg);		
			break;
		//测试服务器注册NACK
		case S_C_MPULOGIN_NACK:
			break;
		//清除生产测试标志位
		case S_C_MPURESTORE_CMD:
			RestoreDefault();
			break;
		//服务器请求关键帧
		case S_C_FASTUPDATEPIC_CMD:
			MsgFastUpdatePicProc(pMsg);
			break;
		//OSP断链
		case OSP_DISCONNECT:
			ProDisconnect(pMsg);
			break;
		//点灯测试
		case S_C_TEST_OUS_CMD:
			ProcTestLedCmd();
			break;
		//点灯timer
		case TIMER_TEST_LED:
			TimerTestLed(pMsg);
		break;

		case  S_C_STARTVMPTEST_REQ:
			Proc8KIStartVmpTestReq(pMsg);
			break;

		
			
		default:
			ErrorLog("wrong message type %u!\n", pMsg->event );
			break;

	}

    return;
}

/*====================================================================
  函 数 名： DaemonProcChangeAutoTestCmd
  功    能： 设置生产测试标志
  算法实现： 
  全局变量： 
  参    数： CMessage
  返 回 值： void
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
   2011/12/07		v4.7		zhouyiliang			create
====================================================================*/
void CMpu2VmpTestClient::DaemonProcChangeAutoTestCmd( CMessage* const pMsg )
{
	if ( NULL == pMsg)
	{
		ErrorLog( "[DaemonProcChangeAutoTestCmd] pMsg is Null!\n" );
		return;
	}
#ifdef _LINUX_
	

	s32 sdwAutoTest = *(s32*)pMsg->content;
	if( sdwAutoTest != 0)
	{
		BrdSetE2promTestFlag();
		KeyLog( "[DaemonProcChangeAutoTestCmd] Set e2prom test flag 1\n" );
	}
	else 
	{
		BrdClearE2promTestFlag();
		KeyLog( "[DaemonProcChangeAutoTestCmd] Set e2prom test flag 0\n" );
	}
#endif
	
	// 	BrdHwReset();
}
/*====================================================================
  函 数 名： InitMediaRcv
  功    能： 初始化接收对象
  算法实现： 
  全局变量： 
  参    数： byChnNo:接收对象通道号
  返 回 值： TRUE：初始化成功，FALSE：初始化失败
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
   2011/12/07		v4.7		zhouyiliang			create
====================================================================*/
BOOL32 CMpu2VmpTestClient::InitMediaRcv(u8 byChnNo )
{
	//mpu2只测一个instance（无basic*2模式）
    g_cMpu2VmpApp.m_atMpu2VmpRcvCB[0][byChnNo].m_byChnnlId = byChnNo;
    g_cMpu2VmpApp.m_atMpu2VmpRcvCB[0][byChnNo].m_pHardMulPic  = &m_cHardMulPic;

    s32 wRet = m_pcMediaRcv[byChnNo]->Create( MAX_VIDEO_FRAME_SIZE,
			                                  CBMpuSVmpRecvFrame,   
				    		                  (u32)&g_cMpu2VmpApp.m_atMpu2VmpRcvCB[0][byChnNo] );

    if( MEDIANET_NO_ERROR !=  wRet )
	{
		ErrorLog( " m_cMediaRcv[%d].Create fail, Error code is: %d \n", byChnNo, wRet );
		return FALSE;
	}
    else
    {
        KeyLog( "m_cMediaRcv[%d].Create succeed!\n", byChnNo );
    }
	
	//媒控：medianet接收对象不要调用SetHdFlag接口或者设置参数为false。
	//如果设置为TRUE，net组帧时会加一个2K的头，这个头是不需要的，只会增加负担。
    m_pcMediaRcv[byChnNo]->SetHDFlag( FALSE );

    return TRUE;
}
/*====================================================================
  函 数 名： StartNetRcvSnd
  功    能： 开始发送和接收
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
   2011/12/07		v4.7		zhouyiliang			create
====================================================================*/
void CMpu2VmpTestClient::StartNetRcvSnd()
{
   
	//1. 编码发送相关
    u8 byChnNo = 0;
	//u8 byActPt = 0;

	// media encrypt modify
//	TMediaEncrypt tMediaEncrypt;
// 	s32 nKeyLen = 0;
// 	u8  abyKeyBuf[MAXLEN_KEY] = {0};
	
//     for ( byChnNo = 0; byChnNo < MAXNUM_MPU2VMP_CHANNEL; byChnNo++ )
//     {
//         InitMediaSnd( byChnNo );
//         m_cHardMulPic.SetVidDataCallback( byChnNo, VMPCALLBACK, m_pcMediaSnd[byChnNo] );
//     }
	//生产测试只编一路就可以了
	InitMediaSnd( 0 );
    m_cHardMulPic.SetVidDataCallback( byChnNo, VMPCALLBACK, m_pcMediaSnd[0] );

    //2. 解码接收相关
	for (byChnNo = 0; byChnNo < MAXNUM_MPU2VMP_MEMBER; byChnNo ++)
	{
        //MediaRcv 对象设置网络参数
		//设置动态载荷,加密
		m_pcMediaRcv[byChnNo]->SetDecryptKey( NULL, 0, 0 );	
		//2009-7-22 为防止后续动态载荷切换引起meidanet紊乱，统一设置载荷为 MEDIA_TYPE_H264
        u16 wRet = m_pcMediaRcv[byChnNo]->SetActivePT( MEDIA_TYPE_H264, MEDIA_TYPE_H264 );
		KeyLog( "Set key to NULL, PT(real): %u, SetActivePt result is:%d \n", MEDIA_TYPE_H264, wRet );
		SetMediaRcvNetParam( byChnNo);

		  if (NULL != m_pcMediaRcv[byChnNo]) 
		  {
			m_pcMediaRcv[byChnNo]->StartRcv();
		  }
	  
	}


}


/*=============================================================================
  函 数 名： InitMediaSnd
  功    能： 初始化发送对象
  算法实现： 
  全局变量： 
  参    数： u8 byChnNo        ：通道号
  返 回 值： void 
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
   2011/12/07		v4.7		zhouyiliang			create
====================================================================*/
void CMpu2VmpTestClient::InitMediaSnd( u8 byChnNo )
{
    u8 byFrameRate = m_tMpuVmpCfg.m_tVideoEncParam[byChnNo].m_byFrameRate;
    u8 byMediaType = m_tMpuVmpCfg.m_tVideoEncParam[byChnNo].m_byEncType;
    u32 dwNetBand  = m_tMpuVmpCfg.m_adwMaxSendBand[byChnNo];


	//1、最大网络发送带宽调整
    if ( dwNetBand * 1024 > 8000000 )
    {
        if ( m_tMpuVmpCfg.m_tVideoEncParam[byChnNo].m_wBitRate <= 8128 )
        {
			dwNetBand = m_tMpuVmpCfg.m_tVideoEncParam[byChnNo].m_wBitRate * 1024;
        }
        else
        {
            dwNetBand = 0;
        }
    }
    else
    {
        dwNetBand = dwNetBand * 1024;
    }
    KeyLog( "[InitMediaSnd]m_cMediaSnd[%d]: dwNetBand = %d\n",byChnNo, dwNetBand);

	
    //2、Create 发送对象
    s32 wRet = m_pcMediaSnd[byChnNo]->Create( MAX_VIDEO_FRAME_SIZE,
		    	                      dwNetBand,
			                          byFrameRate,
			    				      byMediaType );

    if ( MEDIANET_NO_ERROR != wRet )
    {
        ErrorLog( "[InitMediaSnd] m_cMediaSnd[%d].Create fail, Error code is:%d, NetBand.%d, FrmRate.%d, MediaType.%d\n", byChnNo, wRet, dwNetBand, byFrameRate, byMediaType );
    	return;
    }
    else
    {
        KeyLog( "[InitMediaSnd] m_cMediaSnd[%d].Create as NetBand.%d, FrmRate.%d, MediaType.%d\n", byChnNo, dwNetBand, byFrameRate, byMediaType );
    }
	//3、设置HDFlag
	BOOL32 bHDFlag = TRUE;
	if ( 7 == byChnNo || 8 == byChnNo ) 
	{
		bHDFlag = FALSE;
	}
	//4.设置payload
	m_pcMediaSnd[byChnNo]->SetActivePT(MEDIA_TYPE_H264);
	KeyLog("[InitMediaSnd] m_cMediaSnd[%d].SetActivePt:%d",byChnNo,MEDIA_TYPE_H264);
	m_pcMediaSnd[byChnNo]->SetHDFlag( bHDFlag );
	KeyLog( "[InitMediaSnd] m_cMediaSnd[%d].SetHDFlag: %d\n", byChnNo, bHDFlag );

	//4、设置发送通道的netparam
    SetMediaSndNetParam(byChnNo);

    return;
}

/*=============================================================================
  函 数 名： SetMediaSndNetParam
  功    能： 设置发送对象的网络参数
  算法实现： 
  全局变量： 
  参    数： u8 byChnNo        ：通道号
  返 回 值： TRUE:设置成功，FALSE：设置失败 
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
   2011/12/07		v4.7		zhouyiliang			create
====================================================================*/
BOOL32 CMpu2VmpTestClient::SetMediaSndNetParam(u8 byChnNo)
{
	 TNetSndParam tNetSndPar;
    memset( &tNetSndPar, 0x00, sizeof(tNetSndPar) );
	
    tNetSndPar.m_byNum  = 1;           
    tNetSndPar.m_tLocalNet.m_wRTPPort       = PORT_SNDBIND +  byChnNo * 2; 
    tNetSndPar.m_tLocalNet.m_wRTCPPort      = m_wServerRcvStartPort + byChnNo * PORTSPAN + 1;/*PORT_SNDBIND + byChnNo * 2 + 1*/
    tNetSndPar.m_tRemoteNet[0].m_dwRTPAddr  = m_dwServerRcvIp; //已经是网络序了，不需再转
    tNetSndPar.m_tRemoteNet[0].m_wRTPPort   = m_wServerRcvStartPort + byChnNo * PORTSPAN;
    tNetSndPar.m_tRemoteNet[0].m_dwRTCPAddr = m_dwServerRcvIp;//已经是网络序了，不需再转
    tNetSndPar.m_tRemoteNet[0].m_wRTCPPort  = m_wServerRcvStartPort + byChnNo * PORTSPAN + 1;

    KeyLog( "[InitMediaSnd] Snd[%d]: RTPAddr(%s)\n", byChnNo, StrOfIP(m_dwServerRcvIp));

    u16 wRet = m_pcMediaSnd[byChnNo]->SetNetSndParam( tNetSndPar );
    if ( MEDIANET_NO_ERROR != wRet )
	{
    	ErrorLog( "[SetMediaSndNetParam] m_cMediaSnd[%d].SetNetSndParam fail, Error code is: %d \n", byChnNo, wRet );
       	return FALSE;
	}
	return TRUE;
}


/*=============================================================================
  函 数 名： SetMediaRcvNetParam
  功    能： 设置Medianet Receive对象网络参数
  算法实现： 
  全局变量： 
  参    数： u8 byChnNo        ：通道号
  返 回 值： TRUE:设置成功，FALSE：设置失败
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
   2011/12/07		v4.7		zhouyiliang			create
====================================================================*/
BOOL32 CMpu2VmpTestClient::SetMediaRcvNetParam( u8 byChnNo)
{
	TLocalNetParam tlocalNetParm;
    memset(&tlocalNetParm, 0, sizeof(TLocalNetParam));
    tlocalNetParm.m_tLocalNet.m_wRTPPort  = m_tCfg.wRcvStartPort + byChnNo * PORTSPAN;
    tlocalNetParm.m_tLocalNet.m_wRTCPPort = m_tCfg.wRcvStartPort + 1 + byChnNo * PORTSPAN;
    tlocalNetParm.m_dwRtcpBackAddr        = m_dwServerRcvIp;//已经是网络序了，不需再转
	// xliang [5/6/2009] 分配原则 ChnNo		Port(该MPU绑定的MP板上rtcp端口)
	//							  0~7	===> 2~9 (即39002,...,39009)
	//							  8~15	===> 12~19 (即39012,...,39019)
	//16~23	===> 22~29	
	//24~25 ====> 32~33	
	u16 wRtcpBackPort;
	if (byChnNo >= 24)
	{
		wRtcpBackPort = m_wServerRcvStartPort+8+byChnNo;
	}
	else if (byChnNo >= 16)
	{
		wRtcpBackPort = m_wServerRcvStartPort + 6 + byChnNo;
	}
	if (byChnNo >= 8)
	{
		wRtcpBackPort = m_wServerRcvStartPort + 4 + byChnNo;
	}
	else
	{
		wRtcpBackPort = m_wServerRcvStartPort + 2 + byChnNo;
    }
	
	tlocalNetParm.m_wRtcpBackPort = wRtcpBackPort;//远端的rtcp port
	
    KeyLog( "m_cMediaRcv[%d]:\n", byChnNo );
    KeyLog( "\t tlocalNetParm.m_tLocalNet.m_wRTPPort[%d]:\n", tlocalNetParm.m_tLocalNet.m_wRTPPort );
    KeyLog( "\t tlocalNetParm.m_tLocalNet.m_wRTCPPort[%d]:\n", tlocalNetParm.m_tLocalNet.m_wRTCPPort );
    KeyLog( "\t tlocalNetParm.m_dwRtcpBackAddr[%s]:\n", StrOfIP(tlocalNetParm.m_dwRtcpBackAddr) );
    KeyLog( "\t tlocalNetParm.m_wRtcpBackPort[%d]:\n", tlocalNetParm.m_wRtcpBackPort );
	
	s32 nRet = 0;
    nRet = m_pcMediaRcv[byChnNo]->SetNetRcvLocalParam( tlocalNetParm );
    if( MEDIANET_NO_ERROR !=  nRet )
    {
        ErrorLog( " m_cMediaRcv[%d].SetNetRcvLocalParam fail, Error code is: %d \n", byChnNo, nRet );        
        return FALSE;
	}
    else
    {
        KeyLog( "m_cMediaRcv[%d].SetNetRcvLocalParam succeed!\n", byChnNo );
    }
	return TRUE;
}


/*=============================================================================
  函 数 名： MsgFastUpdatePicProc
  功    能： 请求关键帧处理函数
  算法实现： 
  全局变量： 
  参    数： CMessage
  返 回 值： void
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
   2011/12/07		v4.7		zhouyiliang			create
====================================================================*/
void CMpu2VmpTestClient::MsgFastUpdatePicProc( CMessage * const pMsg )
{
	
	//1、参数合法性校验
	if ( NULL == pMsg ) 
	{
		ErrorLog("[MsgFastUpdatePicProc]Null pointer CMessage\n");
		return;
	}
	//2、解析消息体，是请求哪个编码通道的关键帧
    CServMsg cServMsg( pMsg->content, pMsg->length );
	u8 byChnnlId = cServMsg.GetChnIndex();

	//3、1s内的重复请求过滤
	u32 dwTimeInterval = 1 * OspClkRateGet();	
    u32 dwCurTick = OspTickGet();	
    if( dwCurTick - m_adwLastFUPTick[byChnnlId] > dwTimeInterval 
		)
    {
        m_adwLastFUPTick[byChnnlId] = dwCurTick;

		m_cHardMulPic.SetFastUpdata( byChnnlId );

	
        KeyLog( "[MsgFastUpdatePicProc]m_cHardMulPic.SetFastUpdata(%u)!\n",byChnnlId );
    }

}

/*=============================================================================
  函 数 名： ProcTestOusCmd
  功    能： 测试mpu2状态灯处理函数
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
   2011/12/07		v4.7		zhouyiliang			create
====================================================================*/
void CMpu2VmpTestClient::ProcTestLedCmd()
{
	//1、先灭所有的灯	
	for (u8 byLoop =0; byLoop < MPU2_LED_NUM ; byLoop++ )
	{
#ifdef _LINUX_
		s32 nRet = BrdLedStatusSet( m_abyLedId[byLoop],BRD_LED_OFF );
		if ( OK == nRet )
		{
			KeyLog("[ProcTestOusCmd]Set Led:%d OFF success\n",m_abyLedId[byLoop]);
		}
		else
		{
			KeyLog("[ProcTestOusCmd]Set Led:%d OFF failed\n",m_abyLedId[byLoop]);
		}
#endif
	}
	//2、起timer，依次点灯
	u8 byLedIdIdx = 0;
	SetTimer(TIMER_TEST_LED,TEST_LED_TIMER_INTERVAL,byLedIdIdx);
}


/*=============================================================================
  函 数 名： TimerTestLed
  功    能： 点灯的timer处理
  算法实现： 
  全局变量： 
  参    数： CMessage
  返 回 值： void
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
   2011/12/07		v4.7		zhouyiliang			create
====================================================================*/
void CMpu2VmpTestClient::TimerTestLed(CMessage* const pMsg)
{
	if ( NULL == pMsg ) 
	{
		ErrorLog("[TimerTestLed]Null pointer CMessage\n");
		return;
	}
	KillTimer(TIMER_TEST_LED);
	u8 byLedIdIdx = *(u8 *)pMsg->content;
	if ( byLedIdIdx > MPU2_LED_NUM )
	{
		ErrorLog("[TimerTestLed]Invalid ledIdx:%d\n",byLedIdIdx);	
		return;
	}
	s32 nRet = 0;
	//1、灭掉前面一次点亮的灯
	if ( byLedIdIdx > 0 ) 
	{
#ifdef _LINUX_
		nRet = BrdLedStatusSet( m_abyLedId[byLedIdIdx -1 ],BRD_LED_OFF );
		if ( OK == nRet )
		{
			KeyLog("[TimerTestLed]Set Led:%d OFF success\n",m_abyLedId[byLedIdIdx -1]);
		}
		else
		{
			KeyLog("[TimerTestLed]Set Led:%d OFF failed\n",m_abyLedId[byLedIdIdx -1]);
		}
#endif
	
	}
	if ( byLedIdIdx == MPU2_LED_NUM  ) //最后一个灯也测试完了
	{
		//灭了最后一个灯，此时所有灯都灭了
		KeyLog("[TimerTestLed] All Leds are off now!!!\n");
		return;
	}
	//2、点亮这次要点的灯
#ifdef _LINUX_
	nRet = BrdLedStatusSet( m_abyLedId[byLedIdIdx],BRD_LED_ON );
	if ( OK == nRet )
	{
		KeyLog("[TimerTestLed]Set Led:%d ON success\n",m_abyLedId[byLedIdIdx]);
	}
	else
	{
		KeyLog("[TimerTestLed]Set Led:%d ON failed\n",m_abyLedId[byLedIdIdx]);
	}
#endif
	
	//3、继续点下一盏灯
	SetTimer(TIMER_TEST_LED,TEST_LED_TIMER_INTERVAL,byLedIdIdx + 1);

	
}

/*=============================================================================
  函 数 名： Proc8KIStartVmpTestReq
  功    能： 处理测试服务器发过来的开始vmp测试请求
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
   2012/04/19		v4.7		zhouyiliang			create
====================================================================*/
void CMpu2VmpTestClient::Proc8KIStartVmpTestReq(CMessage* const pMsg)
{
	if ( pMsg == NULL )
	{
		ErrorLog("[Proc8KIStartVmpTestReq]null pointer msg in!\n");
		return;
	}
 #ifdef _8KI_
	//1、解析消息体，消息体中带了当前要测试的是哪块card
	CServMsg cServMsg( pMsg->content, pMsg->length );
	u8* pContent = cServMsg.GetMsgBody();
	u8 byCardIdx = *pContent;
	pContent += sizeof(u8);
	if (byCardIdx != _8KI_LEFTCARD && byCardIdx != _8KI_RIGHTCARD)
	{
		post( MAKEIID( MPU_SERVER_APPID, 1 ), 
			C_S_STARTVMPTEST_NACK, 
			NULL, 0, m_dwMpuTestDstNode
			);
		ErrorLog("[Proc8KIStartVmpTestReq]Server Passed Invalid cardidx:d\n",byCardIdx);
		return;
	}
	if (m_by8KICurTestCard != 0xFF && byCardIdx == m_by8KICurTestCard )//认为已经初始化过了，直接回ack，不再做初始化操作
	{
		post( MAKEIID( MPU_SERVER_APPID, 1 ), 
			C_S_STARTVMPTEST_ACK, 
			NULL, 0, m_dwMpuTestDstNode
		);
		return;
	}
	m_cHardMulPic.StopMerge();
	//2、TODO:媒控层初始化，等媒控的接口,初始化不成功则要回nack
	
	post( MAKEIID( MPU_SERVER_APPID, 1 ), 
		C_S_STARTVMPTEST_ACK, 
		NULL, 0, m_dwMpuTestDstNode
		);
	//3、准备好开始收发
	m_by8KICurTestCard = byCardIdx;//保存当前测试的板卡id
	PrepareToTest();
 #endif
}