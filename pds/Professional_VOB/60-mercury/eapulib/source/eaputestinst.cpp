#include "eaputestinst.h"
//打印级别定义
#define MIXTESTLOG_LEVER_CRIT                (u8)0       //关键点/错误/异常
#define MIXTESTLOG_LEVER_WARN                (u8)1       //警告
#define MIXTESTLOG_LEVER_INFO                (u8)2       //流程
#define MIXTESTLOG_LEVER_RCVDATA             (u8)3       //接收数据
#define MIXTESTLOG_LEVER_SNDDATA             (u8)4       //发送数据
extern s32  g_nPrintElog;
#define SECTION_BoardDebug			(s8*)"BoardDebug"
#define KEY_ServerIpAddr			(s8*)"ServerIpAddr"
#ifdef _LINUX_
#define  VERSION_LEN                    32
#endif
/*====================================================================
    函数名      ：mixtestlog
    功能        ：日志函数
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    2010/01/20  1.0         田志勇          创建
====================================================================*/
/*lint -save -e438 -e530  -e843*/
static void mixtestlog( u8 byLevel, s8* pszFmt, ... )
{
    static const s8 * pchLevelName[] = {(s8 *)"[CRIT]", (s8 *)"[WARN]", (s8 *)"[INFO]",(s8 *)"[DATA]", (s8 *)"[DETAIL]", ""};

    s8 achPrintBuf[512];
    s32  nBufLen = 0;

    if ( g_nPrintElog >= byLevel )
    {
		va_list argptr;
        nBufLen = sprintf( achPrintBuf, "%s: ", pchLevelName[byLevel] );         
        va_start( argptr, pszFmt );
        vsprintf( achPrintBuf + nBufLen, pszFmt, argptr ); 
        va_end( argptr ); 
        OspPrintf( TRUE, FALSE, achPrintBuf ); 
    }

    return;
}
/*lint -restore*/

/*====================================================================
    函数名      ：StrOfIP
    功能        ：
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    2010/01/20  1.0         田志勇          创建
====================================================================*/
static char * StrOfTestIP( u32 dwIP )
{
    dwIP = htonl(dwIP);
    static char strIP[17];  
    u8 *p = (u8 *)&dwIP;
    sprintf(strIP,"%d.%d.%d.%d%c",p[0],p[1],p[2],p[3],0);
    return strIP;
}
/*====================================================================
    函数名      ：MixNetRcvCallBack
    功能        ：接收编码回调，将接收到的数据传给混音器
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    2010/01/20  1.0         田志勇          创建
====================================================================*/
static void MixNetRcvTestCallBack(PFRAMEHDR pFrmHdr, u32 dwContext)
{
    CRcvTestCallBack *pcManaPara = (CRcvTestCallBack *)dwContext;
    TFrameHeader tFrame = {0};
    tFrame.m_dwMediaType = pFrmHdr->m_byMediaType;
    tFrame.m_dwAudioMode = pFrmHdr->m_byAudioMode;
    tFrame.m_dwDataSize  = pFrmHdr->m_dwDataSize;
    tFrame.m_pData       = pFrmHdr->m_pData;
    tFrame.m_dwFrameID   = pFrmHdr->m_dwFrameID;
    tFrame.m_dwSSRC      = pFrmHdr->m_dwSSRC;
    //将接收到的混音成员帧数据传给对应的混音器组。
    u16 wRet = pcManaPara->m_pcMixerInst->m_cMixer.SetAudDecData(pcManaPara->m_byChnnlId, tFrame);
    if (wRet != (u16)Codec_Success)
    {
        mixtestlog(MIXTESTLOG_LEVER_CRIT,"Mixer[%d]call SetAudDecData() Failed! ret=[%d]  chnid(%d), datasize(%d)\n",
                pcManaPara->m_pcMixerInst->GetInsID(), wRet, pcManaPara->m_byChnnlId, tFrame.m_dwDataSize);
    }
	if (MIXTESTLOG_LEVER_RCVDATA == g_nPrintElog)
	{
		mixtestlog(MIXTESTLOG_LEVER_RCVDATA,"Mixer[%d]RcvCall SetAudDecData() chid(%d), datasize(%d)\n",pcManaPara->m_pcMixerInst->GetInsID(), pcManaPara->m_byChnnlId, tFrame.m_dwDataSize);
	}
}
/*====================================================================
    函数名      ：MixEncFrameCallBack
    功能        ：混音器编码回调，将混音器编好的混音码流通过发送对象发送出去
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    2010/01/20  1.0         田志勇          创建
====================================================================*/
static void MixTestEncFrameCallBack(TKdvMixerType* ptType, PTFrameHeader pFrameInfo, void* pContext)
{
	u16 wRet = 0 ;
    CEapuMixTestInst *pInst = (CEapuMixTestInst*)pContext;
    FRAMEHDR tFrmHdr = {0};
    tFrmHdr.m_byAudioMode = (u8)pFrameInfo->m_dwAudioMode;
    tFrmHdr.m_dwDataSize = pFrameInfo->m_dwDataSize;
    tFrmHdr.m_dwFrameID = pFrameInfo->m_dwFrameID;
    tFrmHdr.m_byMediaType = (u8)pFrameInfo->m_dwMediaType;
    tFrmHdr.m_dwSSRC = pFrameInfo->m_dwSSRC;
    tFrmHdr.m_pData = pFrameInfo->m_pData;
	//将混音器编好的混音码流通过发送对象发送出去
	if ( ptType->bDMode ) 
	{
		wRet = pInst->m_pcAudSnd[0]->Send(&tFrmHdr);
		if (wRet != MEDIANET_NO_ERROR)
		{
			mixtestlog(MIXTESTLOG_LEVER_CRIT,"Mixer[%d]call Send() wRet = [%d] chid[%d]\n",
                pInst->GetInsID(),wRet, ptType->dwChID);
		}
		if (MIXTESTLOG_LEVER_SNDDATA == g_nPrintElog)
		{
			mixtestlog(MIXTESTLOG_LEVER_SNDDATA,"Mixer[%d]call Send()  chid[%d]\n",
                pInst->GetInsID(), ptType->dwChID);
		}
	}
}
CEapuMixTestInst::CEapuMixTestInst()
{
	m_byCurTestDspId = 0;
    m_dwEapuTestDstNode = INVALID_NODE;
    m_dwEapuTestDstInst = INVALID_INS;
}

CEapuMixTestInst::~CEapuMixTestInst()
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
void CEapuMixTestInst::Init()
{
	KdvSocketStartup();
	u16 wRet = 0;
	u32 dwIndex = 0 ;
    for ( dwIndex = 0; dwIndex < MAXNUM_MIXER_CHNNL ; dwIndex++ )
    {
        m_pcAudRcv[dwIndex] = new CKdvMediaRcv;
        if (m_pcAudRcv[dwIndex] == NULL)//保护措施
        {
            mixtestlog(MIXTESTLOG_LEVER_INFO,"new(CKdvMediaRcv) fail!\n" );//是否做清除处理
            return;
        }
		m_acRcvCB[dwIndex].m_byChnnlId = dwIndex;
		m_acRcvCB[dwIndex].m_pcMixerInst = this;
		wRet = m_pcAudRcv[dwIndex]->Create(MAX_AUDIO_FRAME_SIZE, MixNetRcvTestCallBack, (u32)&m_acRcvCB[dwIndex]);
		if(wRet != MEDIANET_NO_ERROR)
		{
			mixtestlog(MIXTESTLOG_LEVER_CRIT,"Mixer[%d]RCVOBJECT Call Create() Failed! wRet=[%d]\n", GetInsID() - 1, wRet);
			return;
		}
		mixtestlog(MIXTESTLOG_LEVER_INFO,"Mixer[%d]RCVOBJECT Call Create() Successed!\n", GetInsID() - 1);
		TLocalNetParam tlocalNetParm;
		memset(&tlocalNetParm, 0, sizeof(TLocalNetParam));
		tlocalNetParm.m_tLocalNet.m_dwRTPAddr = 0;
		tlocalNetParm.m_tLocalNet.m_wRTPPort  = EAPU_LOCALRCVPORT + (u16)(dwIndex) * PORTSPAN + 2;//本地接收端口
		mixtestlog(MIXTESTLOG_LEVER_INFO,"info:chn[%d]LocRcvPort[%d]\n",dwIndex,tlocalNetParm.m_tLocalNet.m_wRTPPort);
		wRet = m_pcAudRcv[dwIndex]->SetNetRcvLocalParam(tlocalNetParm);
        OspPrintf(TRUE,FALSE,"new CKdvMediaRcv[%d]",dwIndex);
    }
	m_pcAudSnd[0] = new CKdvMediaSnd;
    if (m_pcAudSnd[0] == NULL)//保护措施
    {
		mixtestlog(MIXTESTLOG_LEVER_INFO," m_pcAudSndnew(CKdvMediaSnd) fail!\n" );//是否做清除处理
        return;
    }
	mixtestlog(MIXTESTLOG_LEVER_INFO," m_pcAudSndnew(CKdvMediaSnd) Successed!\n" );
	wRet = m_pcAudSnd[0]->Create(8*1024, 64<<10, 25, MEDIA_TYPE_PCMA);
	if(wRet != MEDIANET_NO_ERROR)
    {
        mixtestlog(MIXTESTLOG_LEVER_CRIT,"Mixer[%d]Call Create() Failed!wAudType=[%d]dwChnIdx=[%d]\n",
				GetInsID(),MEDIA_TYPE_PCMA,0);
        return;
    }
	mixtestlog(MIXTESTLOG_LEVER_CRIT,"Mixer[%d]Call Create() Successed!wAudType=[%d]dwChnIdx=[%d]\n",
				GetInsID(),MEDIA_TYPE_PCMA,0);
	TNetSndParam tNetSndPar;
	memset( &tNetSndPar, 0x00, sizeof(tNetSndPar) );
	tNetSndPar.m_byNum  = 1;
	tNetSndPar.m_tLocalNet.m_wRTPPort       = EAPU_LOCALSNDPORT + 2 ;//发送端口
	tNetSndPar.m_tLocalNet.m_wRTCPPort      = EAPU_LOCALSNDPORT + 1;//m_tMixerCfg.wRcvStartPort + dwChnIdx * PORTSPAN + 2 + 1;//远端未收到时回馈到本地的端口
	tNetSndPar.m_tRemoteNet[0].m_dwRTPAddr  = htonl(m_dwMcuRcvIp);
	tNetSndPar.m_tRemoteNet[0].m_wRTPPort   = SERVER_RCVPORT + 2;//远端接收端口
	wRet = m_pcAudSnd[0]->SetNetSndParam(tNetSndPar);
	mixtestlog(MIXTESTLOG_LEVER_INFO,"LocRTPSndPort：[%d] LocRTCPSndPort：[%d] RemoteRcvIP：[%s] RemoteRcvPort：[%d]\n",
        tNetSndPar.m_tLocalNet.m_wRTPPort,tNetSndPar.m_tLocalNet.m_wRTCPPort,
        StrOfTestIP(tNetSndPar.m_tRemoteNet[0].m_dwRTPAddr),tNetSndPar.m_tRemoteNet[0].m_wRTPPort);



	TKdvMixerInitParam initParam = { 0 } ;
	initParam.dwMaxChNum = 64;
    initParam.dwAudDecDspID = 0 ;
    initParam.dwAudEncDspID = 0 ;
	initParam.dwDModeChMum  = 1 ;
    wRet = m_cMixer.Create(initParam);
    if ( wRet != (u16)Codec_Success)//创建混音器
    {
        //创建失败，则销毁
        mixtestlog(MIXTESTLOG_LEVER_INFO,"Mixer[%d]Call Create() Failed! wRet=[%d]\n", GetInsID() - 1, wRet);
        return;
    }
	mixtestlog(MIXTESTLOG_LEVER_INFO,"Mixer[%d]Call Create() Successed!\n",GetInsID() - 1);
    wRet =m_cMixer.SetAudEncDataCallback(MixTestEncFrameCallBack, this);//设置编码回调
    if ( wRet != (u16)Codec_Success)//创建混音器
    {
        m_cMixer.Destroy();
        mixtestlog(MIXTESTLOG_LEVER_INFO,"Mixer[%d]Call SetAudEncDataCallback() Failed! wRet=[%d]\n", GetInsID() - 1, wRet);
        return;
    }
	mixtestlog(MIXTESTLOG_LEVER_INFO,"Mixer[%d]Call SetAudEncDataCallback() Successed!\n",GetInsID() - 1);
	wRet = m_cMixer.SetMixDepth(10);
    if ((u16)Codec_Success != wRet)
    {
        mixtestlog(MIXTESTLOG_LEVER_INFO,"Mixer Call SetMixDepth() Failed! wRet = [%d]\n", wRet);
        return;
    }

	if(!ConnectServ())
	{
		mixtestlog(MIXTESTLOG_LEVER_INFO,"ConnectServ\n");
		SetTimer( EV_C_CONNECT_TIMER, EAPU_CONNETC_TIMEOUT );
	}
	else
	{
		::OspNodeDiscCBRegQ( m_dwEapuTestDstNode, GetAppID(), GetInsID() );
		KillTimer(EAPU_CONNETC_TIMEOUT);
		mixtestlog(MIXTESTLOG_LEVER_INFO,"EV_C_REGISTER_TIMER\n");
	}

    return;
}
BOOL32 CEapuMixTestInst::ConnectServ()
{
	OspDelay(1*1000);
	printf("Begin to connect Test Server!\n");
    BOOL  bRet;
    s8    achDefStr[] = "10.1.1.1";
    s8    achReturn[MAX_VALUE_LEN + 1]  = {0};
	s8    achProfileName[64] = {0};
    memset((void*)achProfileName, 0x0, sizeof(achProfileName));
 	sprintf(achProfileName, "%s/%s", DIR_CONFIG, FILE_BRDCFGDEBUG_INI);
	u32 dwServerIpAddr = 0;
	//20100706_tzy  该接口在LINUX下读文件失败时，返回的默认值有问题，在WIN32下正常
	bRet = GetRegKeyString( achProfileName, SECTION_BoardDebug, KEY_ServerIpAddr, 
						achDefStr, achReturn, MAX_VALUE_LEN + 1 );
    if( bRet )  
	{
		dwServerIpAddr = inet_addr(achReturn);
		mixtestlog(MIXTESTLOG_LEVER_INFO, "[ConnectServ] Success profile while reading %s**%s**%s***[%s]!\n", achProfileName,SECTION_BoardDebug, KEY_ServerIpAddr,achReturn );
	}
	else
	{
		mixtestlog(MIXTESTLOG_LEVER_INFO,"[ConnectServ] Wrong profile while reading %s**%s**%s!\n", achProfileName,SECTION_BoardDebug, KEY_ServerIpAddr );
		dwServerIpAddr = inet_addr( "10.1.1.1" );
	}

	m_dwEapuTestDstNode = OspConnectTcpNode(dwServerIpAddr, 60000);
	
	if(INVALID_NODE == m_dwEapuTestDstNode)
	{
		printf("[ConnectServ] connect to Server %s failed!\n",achReturn);
		mixtestlog(MIXTESTLOG_LEVER_INFO, "[ConnectServ] connect to Server %s failed!\n",achReturn);
		
		return FALSE;
	}
	m_dwMcuRcvIp = dwServerIpAddr; //Feign Mcu Ip
	TNetSndParam tNetSndPar;
	memset( &tNetSndPar, 0x00, sizeof(tNetSndPar) );
	tNetSndPar.m_byNum  = 1;
	tNetSndPar.m_tLocalNet.m_wRTPPort       = SERVER_RCVPORT + 2 ;//发送端口
	tNetSndPar.m_tLocalNet.m_wRTCPPort      = SERVER_RCVPORT + 3;//m_tMixerCfg.wRcvStartPort + dwChnIdx * PORTSPAN + 2 + 1;//远端未收到时回馈到本地的端口
	tNetSndPar.m_tRemoteNet[0].m_dwRTPAddr  = htonl(m_dwMcuRcvIp);
	tNetSndPar.m_tRemoteNet[0].m_wRTPPort   = SERVER_RCVPORT + 2;//远端接收端口
	m_pcAudSnd[0]->SetNetSndParam(tNetSndPar);
	mixtestlog(MIXTESTLOG_LEVER_INFO,"LocRTPSndPort：[%d] LocRTCPSndPort：[%d] RemoteRcvIP：[%s] RemoteRcvPort：[%d]\n",
        tNetSndPar.m_tLocalNet.m_wRTPPort,tNetSndPar.m_tLocalNet.m_wRTCPPort,
        StrOfTestIP(tNetSndPar.m_tRemoteNet[0].m_dwRTPAddr),tNetSndPar.m_tRemoteNet[0].m_wRTPPort);
	printf("Connect Server(%d) ok, DstNode is %d!\n", m_dwMcuRcvIp, m_dwEapuTestDstNode);
	RegisterSrv();
	return TRUE;
}

void CEapuMixTestInst::RegisterSrv(void)
{
	mixtestlog(MIXTESTLOG_LEVER_INFO,"Enter RegisterSrv\n");
	printf("Enter RegisterSrv!\n");
    u8 abyTemp[sizeof(u8)*32 + sizeof(CLoginRequest) + sizeof(CDeviceInfo)];
    memset(abyTemp, 0, sizeof(abyTemp));
    CLoginRequest cLoginReq;
    cLoginReq.Empty();
    cLoginReq.SetName("admin");
    cLoginReq.SetPassword("admin");
    memcpy(abyTemp + sizeof(u8)*32, &cLoginReq, sizeof(CLoginRequest));
    
    CDeviceInfo cDeviceinfo;
	memset( &cDeviceinfo, 0x0, sizeof( cDeviceinfo ) );
    SetEapuDeviceInfo(cDeviceinfo);
    memcpy(abyTemp + sizeof(u8)*32 + sizeof(CLoginRequest), &cDeviceinfo, sizeof(CDeviceInfo));
	
    post( MAKEIID( EAPU_SERVER_APPID, 1 ), 
		TEST_C_S_LOGIN_REG, 
		abyTemp, 
        sizeof(u8) * 32  + sizeof(CLoginRequest) + sizeof(CDeviceInfo),
        m_dwEapuTestDstNode);
	SetTimer( EV_C_REGISTER_TIMER, EAPU_REGISTER_TIMEOUT);
}

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
void CEapuMixTestInst::SetEapuDeviceInfo(CDeviceInfo &cDeviceInfo)
{
#ifndef WIN32
    
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
	mixtestlog(MIXTESTLOG_LEVER_INFO,"achMac=[%s]achHWversion=[%s]\n",achMac,achHWversion);

#endif
	mixtestlog(MIXTESTLOG_LEVER_INFO,"achSoftVersion=[%s]\n",cDeviceInfo.getSoftVersion());
	return;
}

void CEapuMixTestInst::ProcRegAck(CMessage* const pMsg)
{
	KillTimer(EV_C_REGISTER_TIMER);
	m_dwEapuTestDstInst = pMsg->srcid;
	//通知测试服务器mpu准备就绪
//	post( MAKEIID( MPU_SERVER_APPID, 1 ),C_S_MPUREADY_NOTIF, NULL, 0, m_dwEapuTestDstNode);
}
void CEapuMixTestInst::ProcStartTestReq(CMessage* const pMsg)
{
	mixtestlog(MIXTESTLOG_LEVER_INFO,"Rcv Message TEST_S_C_START_REG\n");
	printf("Rcv Message TEST_S_C_START_REG\n");

	CServMsg cServMsg(pMsg->content, pMsg->length);
	u8 byDspId = 0 ;
	TEapuAutoTestMsg tEapuAutoTestMsg = *(TEapuAutoTestMsg*)(pMsg->content);

	/*lint -save -e685*/
	/*Warning -- Relational operator '<=' always evaluates to 'true'*/
	if ( tEapuAutoTestMsg.GetAutoTestType() >= emTestDspID0 && 
		 tEapuAutoTestMsg.GetAutoTestType() <= emTestDspID3 )
	{
		byDspId = (u8)tEapuAutoTestMsg.GetAutoTestType() ;
		//byAudioType = *((u8*)cSmsg.GetMsgBody() + sizeof(u8)) ;
		if (m_byCurTestDspId == byDspId && m_byCurTestDspId != 0)
		{ 
			mixtestlog(MIXTESTLOG_LEVER_INFO,"Msg:m_byCurTestDspId = [%d]  byDspId = [%d]",m_byCurTestDspId,byDspId);
			printf("Msg:m_byCurTestDspId = [%d]  byDspId = [%d]",m_byCurTestDspId,byDspId);
			return;
		}
		m_byCurTestDspId = byDspId;
		mixtestlog(MIXTESTLOG_LEVER_INFO,"Msg:DspID[%d]**",byDspId);
	}
	else
	{
		cServMsg.SetErrorCode(ERR_EAPUTEST_ERRDSPID);
        SendMsgToMcu(TEST_C_S_START_NACK, cServMsg);
		printf("Msg:TEST_C_S_START_NACK\n");
		return;
	}
	/*lint -restore*/

	m_byAudType = MEDIA_TYPE_PCMA;
	mixtestlog(MIXTESTLOG_LEVER_INFO,"Msg:DspID[%d]AudType[%d]",byDspId,m_byAudType);
	u16 wRet = 0;
	u32 dwIndex = 0;
	for ( dwIndex = 0; dwIndex < MAXNUM_MIXER_CHNNL ; dwIndex++ )
    {
        wRet = m_pcAudRcv[dwIndex]->StopRcv();
		if ( wRet != (u16)Codec_Success)
		{
			mixtestlog(MIXTESTLOG_LEVER_WARN,"DSPID[%d] call StopRcv(%d) Failed!\n", byDspId,dwIndex);
		}
		else
		{
			mixtestlog(MIXTESTLOG_LEVER_INFO,"DSPID[%d] call StopRcv(%d) successed!\n", byDspId,dwIndex);
		}
	}

	TKdvMixerInitParam initParam = { 0 } ;
	initParam.dwMaxChNum = MAXNUM_MIXER_CHNNL;
    initParam.dwAudDecDspID = byDspId ;
    initParam.dwAudEncDspID = byDspId ;
	initParam.dwDModeChMum  = 1 ;
    wRet = m_cMixer.ReCreate(initParam);	
    if ( wRet != (u16)Codec_Success)//创建混音器
    {
        //创建失败，则销毁
        mixtestlog(MIXTESTLOG_LEVER_INFO,"Mixer[%d]Calll ReCreate() Failed! wRet=[%d]\n", GetInsID() - 1, wRet);
		printf("Mixer[%d]Calll ReCreate() Failed! wRet=[%d]\n", GetInsID() - 1, wRet);
        return;
    }
	mixtestlog(MIXTESTLOG_LEVER_INFO,"Mixer[%d]Call ReCreate() Successed!\n",GetInsID() - 1);
    wRet =m_cMixer.SetAudEncDataCallback(MixTestEncFrameCallBack, this);//设置编码回调
    if ( wRet != (u16)Codec_Success)//创建混音器
    {
        m_cMixer.Destroy();
        printf("Mixer[%d]Call SetAudEncDataCallback() Failed! wRet=[%d]\n", GetInsID() - 1, wRet);
        return;
    }
	mixtestlog(MIXTESTLOG_LEVER_INFO,"Mixer[%d]Call SetAudEncDataCallback() Successed!\n",GetInsID() - 1);
	wRet = m_cMixer.SetMixDepth(10);
    if ((u16)Codec_Success != wRet)
    {
        mixtestlog(MIXTESTLOG_LEVER_INFO,"Mixer Call SetMixDepth() Failed! wRet = [%d]\n", wRet);
    }
	TKdvMixerType tType;
	tType.dwChID = 0;//多格式要修改
	tType.bDMode = TRUE;
	tType.byAudType = MEDIA_TYPE_PCMA;
	tType.byAudMode = AUDIO_MODE_PCMA ;
	wRet = m_cMixer.SetDModeChannel(tType);
	if(wRet != (u16)Codec_Success)
	{
		cServMsg.SetErrorCode(wRet);         
		SendMsgToMcu(TEST_C_S_START_NACK, cServMsg);
		mixtestlog(MIXTESTLOG_LEVER_CRIT,"Mixer[%d]CreateDModeSnd () Call SetDModeChannel() failed(%d)\n",wRet);
		printf("Mixer[%d]CreateDModeSnd () Call SetDModeChannel() failed(%d)\n",wRet);
		return;
	}
	mixtestlog(MIXTESTLOG_LEVER_INFO,"Mixer[%d]Call SetDModeChannel() Successed!\n",GetInsID() - 1);
	wRet = m_cMixer.StartAudMix();
	if ( wRet != (u16)Codec_Success )
	{
		cServMsg.SetErrorCode(wRet);         
		SendMsgToMcu(TEST_C_S_START_NACK, cServMsg);
		mixtestlog(MIXTESTLOG_LEVER_CRIT,"Mixer[%d]at READY state Call StartMix() Failed! wRet = [%d]\n", GetInsID() - 1,wRet);
		printf("Mixer[%d]at READY state Call StartMix() Failed! wRet = [%d]\n", GetInsID() - 1,wRet);
		return;
	} 
	mixtestlog(MIXTESTLOG_LEVER_INFO,"Mixer[%d]Call StartAudMix() Successed!\n",GetInsID() - 1);
	for ( dwIndex = 0; dwIndex < MAXNUM_MIXER_CHNNL ; dwIndex++ )
    {
		TKdvMixerType tMixerType;
		tMixerType.dwChID = dwIndex;
		tMixerType.bDMode = FALSE;
		tMixerType.byAudType = m_byAudType;
		tMixerType.byAudMode = AUDIO_MODE_PCMA ;
        m_pcAudRcv[dwIndex]->StartRcv();
		wRet = m_cMixer.AddChannel(dwIndex);//混音器通道号从0开始
		if(wRet != (u16)Codec_Success)
		{
			mixtestlog(MIXTESTLOG_LEVER_CRIT,"Mixer[%d]AddChannel(%d) failed(%d)\n",GetInsID() - 1, dwIndex, wRet);
			printf("Mixer[%d]AddChannel(%d) failed(%d)\n",GetInsID() - 1, dwIndex, wRet);
			return;
		}
		mixtestlog(MIXTESTLOG_LEVER_INFO,"Mixer[%d]Call AddChannel(%d) Successed!\n",GetInsID() - 1,dwIndex);
		wRet = m_cMixer.SetAudEncParam(tMixerType);
		if(wRet != (u16)Codec_Success)
		{
			cServMsg.SetErrorCode(ERR_EAPUTEST_ERRDEVICE);
	        SendMsgToMcu(TEST_C_S_START_NACK, cServMsg);
		    mixtestlog(MIXTESTLOG_LEVER_CRIT,"MixerCall SetAudEncParam(%d) failed(%d)\n", dwIndex, wRet);
			printf("MixerCall SetAudEncParam(%d) failed(%d)\n", dwIndex, wRet);
			return;
		}
		mixtestlog(MIXTESTLOG_LEVER_INFO,"Mixer[%d]Call SetAudEncParam(%d) Successed!\n",GetInsID() - 1,dwIndex);
    }
	cServMsg.SetErrorCode(0);         
	SendMsgToMcu(TEST_C_S_START_ACK, cServMsg);
	mixtestlog(MIXTESTLOG_LEVER_INFO,"Send Message TEST_C_S_START_ACK to A's Mcu%d\n");
}

BOOL32 CEapuMixTestInst::SendMsgToMcu(u16 wEvent, CServMsg const &cServMsg)
{
    BOOL bRet = TRUE;
    if (OspIsValidTcpNode( m_dwEapuTestDstNode ))
    {
        post(m_dwEapuTestDstInst, wEvent, cServMsg.GetServMsg(), 
			cServMsg.GetServMsgLen(), m_dwEapuTestDstNode);
        mixtestlog(MIXTESTLOG_LEVER_INFO,"Send Message %u(%s) to A's Mcu%d\n", wEvent, ::OspEventDesc(wEvent),
			LOCAL_MCUID);
    }
    else
    {
        mixtestlog(MIXTESTLOG_LEVER_INFO,"Send Message failed %u(%s) ,since disconnected with AMCU%d .\n",
            wEvent, ::OspEventDesc(wEvent), 192);
        bRet = FALSE;
    }
    return bRet;
}
void CEapuMixTestInst::ProcStopTestReq(CMessage* const pMsg)
{
	CServMsg cServMsg(pMsg->content, pMsg->length);

	for (  u32 dwIndex = 0; dwIndex < MAXNUM_MIXER_CHNNL ; dwIndex++ )
    {
        u16 wRet = m_pcAudRcv[dwIndex]->StopRcv();
		if ( wRet != (u16)Codec_Success)
		{
			mixtestlog(MIXTESTLOG_LEVER_WARN,"Mixer[%d] call StopAudMix() Failed!\n", GetInsID() - 1);
		}
		else
		{
			mixtestlog(MIXTESTLOG_LEVER_INFO,"Mixer[%d] call StopAudMix() successed!\n", GetInsID() - 1);
		}
	}
	mixtestlog(MIXTESTLOG_LEVER_INFO,"Mixer[%d] call StopAudMix() successed!\n", GetInsID() - 1);
	cServMsg.SetErrorCode(0);         
	SendMsgToMcu(TEST_C_S_STOP_ACK, cServMsg);
	mixtestlog(MIXTESTLOG_LEVER_INFO,"Send Message TEST_C_S_STOP_ACK to A's Mcu%d\n");
}
void CEapuMixTestInst::ProDisconnect(CMessage* const pMsg)
{
	mixtestlog(MIXTESTLOG_LEVER_INFO,"ProDisconnect!\n");
	if ( NULL == pMsg )  
    {
        printf( "[ProDisconnect] message's pointer is Null\n" );
        return;
    }

	u32 dwNode = *(u32*)pMsg->content;

	if( dwNode != m_dwEapuTestDstNode)
	{
		printf( "[ProDisconnect] the Node.%d isn't equal to m_dwEapuTestDstNode.%d\n",dwNode, m_dwEapuTestDstNode );
		return;
	}

	m_dwEapuTestDstInst = INVALID_INS;	
    m_bConnected = FALSE;
    
    if(INVALID_NODE == m_dwEapuTestDstNode)
	{
		OspPrintf( TRUE, FALSE, "[CEapuMixTestInst] ProDisconnect: m_dwHduAutoTestDstNode is invalid!\n" );
        return;   
	}
	
    OspDisconnectTcpNode(m_dwEapuTestDstNode);
	
    m_dwEapuTestDstNode = INVALID_NODE;

	//恢复到初始状态[7/11/2013 chendaiwei]
	u16 wRet = 0;
	u32 dwIndex = 0;
	for ( dwIndex = 0; dwIndex < MAXNUM_MIXER_CHNNL ; dwIndex++ )
    {
        wRet = m_pcAudRcv[dwIndex]->StopRcv();
		if ( wRet != (u16)Codec_Success)
		{
			mixtestlog(MIXTESTLOG_LEVER_WARN,"m_byCurTestDspId[%d] call StopRcv(%d) Failed!\n", m_byCurTestDspId,dwIndex);
		}
		else
		{
			mixtestlog(MIXTESTLOG_LEVER_INFO,"m_byCurTestDspId[%d] call StopRcv(%d) successed!\n", m_byCurTestDspId,dwIndex);
		}
	}

	m_cMixer.Destroy();
	m_byCurTestDspId = 0;
	printf("[ProDisconnect]Destroy and restasrt again\n");

	TKdvMixerInitParam initParam = { 0 } ;
	initParam.dwMaxChNum = 64;
    initParam.dwAudDecDspID = 0 ;
    initParam.dwAudEncDspID = 0 ;
	initParam.dwDModeChMum  = 1 ;
    wRet = m_cMixer.Create(initParam);
    if ( wRet != (u16)Codec_Success)//创建混音器
    {
        //创建失败，则销毁
        mixtestlog(MIXTESTLOG_LEVER_INFO,"Mixer[%d]Call Create() Failed! wRet=[%d]\n", GetInsID() - 1, wRet);
        return;
    }
	mixtestlog(MIXTESTLOG_LEVER_INFO,"Mixer[%d]Call Create() Successed!\n",GetInsID() - 1);
    wRet =m_cMixer.SetAudEncDataCallback(MixTestEncFrameCallBack, this);//设置编码回调
    if ( wRet != (u16)Codec_Success)//创建混音器
    {
        m_cMixer.Destroy();
        mixtestlog(MIXTESTLOG_LEVER_INFO,"Mixer[%d]Call SetAudEncDataCallback() Failed! wRet=[%d]\n", GetInsID() - 1, wRet);
        return;
    }
	mixtestlog(MIXTESTLOG_LEVER_INFO,"Mixer[%d]Call SetAudEncDataCallback() Successed!\n",GetInsID() - 1);
	wRet = m_cMixer.SetMixDepth(10);
    if ((u16)Codec_Success != wRet)
    {
        mixtestlog(MIXTESTLOG_LEVER_INFO,"Mixer Call SetMixDepth() Failed! wRet = [%d]\n", wRet);
        return;
    }

	//重连
	SetTimer( EV_C_CONNECT_TIMER, EAPU_CONNETC_TIMEOUT );

    return;
}


void CEapuMixTestInst::RestoreDefault(void)
{
#ifndef WIN32

	BOOL32 bRet = FALSE;
    bRet = BrdClearE2promTestFlag();	//清测试标志位
	u8 bySucessRestore = bRet?0:1;
	s32 sdwRet;
    sdwRet = post(m_dwEapuTestDstInst, C_S_EAPURESTORE_NOTIF, &bySucessRestore, sizeof(u8), m_dwEapuTestDstNode);
    if ( sdwRet != OSP_OK )
    {
        printf( "[RestoreDefault] post failed!\n" );
    }
	else
	{
		printf( "[RestoreDefault] post sucessful!\n" );
	}
	OspDelay(2000);
	BrdHwReset();
#endif
    return;
}

void CEapuMixTestInst::DaemonInstanceEntry(CMessage* const pMsg, CApp* pcApp )
{
	mixtestlog(MIXTESTLOG_LEVER_INFO,"DaemonInstanceEntry msg [%u](%s)(appid:%d).\n", pMsg->event, ::OspEventDesc(pMsg->event),pcApp != NULL ? pcApp->appId : 0);
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
void CEapuMixTestInst::InstanceEntry( CMessage* const pMsg )
{
	if( pMsg == NULL )
	{
		mixtestlog(MIXTESTLOG_LEVER_WARN,"pMsg == NULL\n");
		return;
	}

    switch ( pMsg->event )
    {
	case EV_C_INIT:
		Init();		
		break;

	case EV_C_CONNECT_TIMER:
		KillTimer(EV_C_CONNECT_TIMER);
		mixtestlog(MIXTESTLOG_LEVER_INFO, "EV_C_CONNECT_TIMER!\n");
		if (!ConnectServ())
		{
			SetTimer( EV_C_CONNECT_TIMER, EAPU_CONNETC_TIMEOUT );
		}
		else
		{
			::OspNodeDiscCBRegQ( m_dwEapuTestDstNode, GetAppID(), GetInsID() );
		}
		break;

	case EV_C_REGISTER_TIMER:
		RegisterSrv();
        break;

	case TEST_S_C_LOGIN_ACK:
		mixtestlog(MIXTESTLOG_LEVER_INFO,"Recv msg TEST_S_C_LOGIN_ACK.\n");
		ProcRegAck(pMsg);		
		break;

	case TEST_S_C_LOGIN_NACK:
		break;

	case TEST_S_C_START_REG:
		ProcStartTestReq(pMsg);		
		break;

	case TEST_S_C_STOP_REG:
		break;
	case TEST_S_C_RESTORE:
		RestoreDefault();
		break;

	case OSP_DISCONNECT:
		ProDisconnect(pMsg);
		break;
		
	default:
		printf("wrong message type %u!\n", pMsg->event );
		break;

	}

    return;
}

void CEapuMixTestInst::DaemonProcChangeAutoTestCmd( CMessage* const pMsg )
{
#ifdef _LINUX_
	
	if ( NULL == pMsg)
	{
		OspPrintf( TRUE, FALSE, "[DaemonProcChangeAutoTestCmd] pMsg is Null!\n" );
		return;
	}
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
	OspPrintf( TRUE, FALSE, "[DaemonProcChangeAutoTestCmd] MsgSrcNode(%d)\n" ,pMsg != NULL ? pMsg->srcnode : 0);
	// 	BrdHwReset();
}