/*****************************************************************************
   模块名      : Configure Agent
   文件名      : ConfAgent.cpp
   相关文件    : ConfAgent.h reccfg.ini
   文件实现功能: 读取配置信息功能
   作者        : 
   版本        : V3.5  Copyright(C) 2004-2005 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
******************************************************************************/
#include "ConfAgent.h"

#define LOCAL_IP	"127.0.0.1"

BOOL    bPrintLog = TRUE;
u32     g_adwDevice[MAXNUM_RECORDER_CHNNL+1] = {(u32)0};

TCfgAgent   g_tCfg;
CDBOperate  g_cTDBOperate;       // DB 操作

u32	g_adwRecDuration[MAXNUM_RECORDER_CHNNL+1] = {(u32)0};

u16 GetRecChnIndex( u32 dwRecId )
{
    for(u8 byLop = 0; byLop < MAXNUM_RECORDER_CHNNL+1; byLop++ )
    {
        if( g_adwDevice[byLop] == dwRecId )
        {
            return byLop;
        }
    }
    return (u16)NOT_FIND_RECID; // not find the rec id
}


/*====================================================================
    函数名      ：GetExecuteProgramPath
    功能        ：获得执行程序的路径
    引用全局变量：
    输入参数说明：path - 指向存放路径的指针
    返回值说明  ：TRUE/FALSE
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    04/12/06    3.6         libo           创建
    06/08/21    4.0         顾振华        
====================================================================*/
void GetExecuteProgramPath(s8 * path)
{
	TCHAR achExeName[KDV_MAX_PATH] = _T("");

	GetModuleFileName(AfxGetInstanceHandle(), achExeName, sizeof (achExeName));

	CString cstrExePath = achExeName;
	cstrExePath = cstrExePath.Left( cstrExePath.ReverseFind('\\') );    // 完整路径
    strcpy(path, cstrExePath);
}

/*====================================================================
    函数名	     ：ReadConfigFromFile
	功能		 ：从配置文件中读取配置参数
	引用全局变量 ：无
    输入参数说明 ：无
	返回值说明   ：成功返回 TRUE，否则返回FALSE
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
    04/12/28    3.6         libo          修改配置文件的路径
    06/08/28    4.0         顾振华        调整
====================================================================*/
BOOL TCfgAgent::ReadConfigFromFile()
{
	s8   achBuf[256];
	s8   achHead[256];
	s32 nValue;
	
	BOOL bFlag = TRUE;

    m_bInited = FALSE;
	
	// 增加英语提示, zgc, 2007-03-23
	m_tLangId = GetSystemDefaultLangID();
	// Os不是简体中文版
    if( LANGUAGE_CHINESE != m_tLangId )
    {
        m_bOSEn = TRUE;
    }

	if( m_bOSEn )
	{
		sprintf(achHead, "The current configuration file: %s\nSection: %s\n",
		    m_szCfgFilename, SECTION_RECSYS);
	}
	else
	{
		sprintf(achHead, "当前配置文件 : %s\nSection: %s\n",
		    m_szCfgFilename, SECTION_RECSYS);
	}

	//ID
	if ( !::GetRegKeyInt( m_szCfgFilename, SECTION_RECSYS, KEY_RECSYS_PERIEQPID, 0, &nValue ) )
	{
		if( m_bOSEn )
		{
			sprintf( achBuf, "%sKey    :%s\nRead the peripheral ID number failed.\n", achHead, KEY_RECSYS_PERIEQPID);
		}
		else
		{
			sprintf( achBuf, "%sKey    :%s\n读取外设ID号失败\n", achHead, KEY_RECSYS_PERIEQPID);
		}
		AfxMessageBox( achBuf );
		return FALSE;
	}
	
	m_byEqpId   = (u8)nValue;

	//Type
	/*if ( !::GetRegKeyInt( RECCFG_FILE, SECTION_RECSYS, KEY_RECSYS_PERIEQPTYPE, 
						  0, &nValue ) )
	{
		sprintf( achBuf ,"%sKey    :%s\n读取外设类型号失败",achHead,KEY_RECSYS_PERIEQPTYPE);
		AfxMessageBox( achBuf );
		return FALSE;
	}*/

	m_byEqpType   = EQP_TYPE_RECORDER;//(u8)nValue;

	//Alias
	if ( !::GetRegKeyString( m_szCfgFilename, SECTION_RECSYS, KEY_RECSYS_PERIEQPALIAS, 
							 NULL, m_szAlias, sizeof( m_szAlias ) ) )
	{
		if( m_bOSEn )
		{
			sprintf( achBuf ,"%sKey    :%s\nRead the alias failed.",achHead,KEY_RECSYS_PERIEQPALIAS);
		}
		else
		{
			sprintf( achBuf ,"%sKey    :%s\n读取别名失败",achHead,KEY_RECSYS_PERIEQPALIAS);
		}
			
		AfxMessageBox( achBuf );
		return FALSE;
	}
	
    // 机器名
    WORD wVersionRequested;
    WSADATA wsaData;
    wVersionRequested = MAKEWORD( 2, 2 );
    int err = WSAStartup( wVersionRequested, &wsaData );

    if( !::GetRegKeyString(m_szCfgFilename, SECTION_RECSYS, KEY_RECSYS_MACHINENAME, "\0", m_szMachineName, sizeof(m_szMachineName)))
    {
        /*
		if( m_bOSEn )
        {
			sprintf(m_szMachineName, "Please set the recorder's name.");
		}
		else
		{
			sprintf(m_szMachineName, "请设置机器名");
		}
        */
        gethostname(m_szMachineName, 32);
        
        // AfxMessageBox(szMachineName);
        // return FALSE;
    }
    else
    {
        if ( strlen(m_szMachineName) == 0 || strcmp(m_szMachineName, "KEDACOM") == 0 )
        {
            gethostname(m_szMachineName, 32);
        }
    }

	//录像IP地址
	if ( !::GetRegKeyString( m_szCfgFilename, SECTION_RECSYS, KEY_RECSYS_IPADDR, 
						  0, m_achLocalIP ,sizeof(m_achLocalIP) ) )
	{
		if( m_bOSEn )
		{
			sprintf( achBuf ,"%sKey    :%s\nRead the local host's IP failed.",achHead,KEY_RECSYS_IPADDR);
		}
		else
		{
			sprintf( achBuf ,"%sKey    :%s\n读取本机IP地址失败",achHead,KEY_RECSYS_IPADDR);
		}
		AfxMessageBox( achBuf );
		return FALSE;
	}


	bFlag = IsLegality( m_achLocalIP );
	if(bFlag)
	{
		m_dwRecIpAddr = inet_addr(m_achLocalIP);
	}
	else
	{
		m_dwRecIpAddr = inet_addr(LOCAL_IP);
	}

    if ( m_dwRecIpAddr == inet_addr(LOCAL_IP) )
    {
        hostent *tHost = gethostbyname(m_szMachineName);
        if (tHost != NULL)
        {
            if ( tHost->h_addr_list[0] )
            {                 
                memcpy (&m_dwRecIpAddr, tHost->h_addr_list[0],tHost->h_length);
            }
        }            
    }

	//录像机接收起始端口号
	if (!::GetRegKeyInt( m_szCfgFilename, SECTION_RECSYS, KEY_RECSYS_RCVPORT, REC_EQP_STARTPORT, &nValue))
	{
		if( m_bOSEn )
		{
			sprintf( achBuf ,"%sKey    :%s\nRead the receive port failed.",achHead,KEY_RECSYS_RCVPORT);
		}
		else
		{
			sprintf( achBuf ,"%sKey    :%s\n读取录像接收起始端口号失败",achHead,KEY_RECSYS_RCVPORT);
		}
		
		AfxMessageBox( achBuf );
		return FALSE;
	}
	
	bFlag = IsLegality(nValue);
	if(bFlag)
	{
		m_wRecStartPort = (u16)nValue;
	}
	else
	{
		m_wRecStartPort = REC_EQP_STARTPORT;
		//ErrorMsg( 1 );
	}
	
	//放像通道起始端口号
	if (!::GetRegKeyInt(m_szCfgFilename, SECTION_RECSYS, KEY_RECSYS_PLAYPORT, REC_EQP_STARTPORT, &nValue))
	{
		if( m_bOSEn )
		{
			sprintf(achBuf, "%sKey    :%s\nRead the play port failed.", achHead, KEY_RECSYS_PLAYPORT);
		}
		else
		{
			sprintf(achBuf, "%sKey    :%s\n读取放像起始端口号失败", achHead, KEY_RECSYS_PLAYPORT);
		}
		
		AfxMessageBox(achBuf);
		return FALSE;
	}

	bFlag = IsLegality(nValue);
	if(bFlag == TRUE)
	{
		m_wPlayStartPort = (u16)nValue;

	}
	else
	{
		m_wPlayStartPort = REC_EQP_STARTPORT;
	}

	// rec path
	if ( !::GetRegKeyString( m_szCfgFilename, SECTION_RECSYS, KEY_RECSYS_RECFILEPATH, 
						 "\0", m_szRecordPath, sizeof( m_szRecordPath ) ) )
	{
		if( m_bOSEn )
		{
			sprintf( achBuf ,"%sKey    :%s\nRead the path of saving record file failed.",achHead, KEY_RECSYS_RECFILEPATH);
		}
		else
		{
			sprintf( achBuf ,"%sKey    :%s\n读取录像存储路径失败",achHead, KEY_RECSYS_RECFILEPATH);
		}
		
		AfxMessageBox( achBuf );
		return FALSE;
	}

	bFlag = IsLegality( m_szRecordPath );
    if (bFlag)
    {
        // 尝试创建本目录
        ::CreateDirectory( m_szRecordPath, NULL );
    }
	
	// MCU ID
    /*
	if ( !::GetRegKeyInt( m_szCfgFilename, SECTION_RECSYS, KEY_RECSYS_MCUID, 
						  0, &nValue ) )
	{
		if( m_bOSEn )
		{
			sprintf( achBuf ,"%sKey    :%s\nRead the MCU ID failed.",achHead,KEY_RECSYS_MCUID);
		}
		else
		{
			sprintf( achBuf ,"%sKey    :%s\n读取MCU ID失败",achHead,KEY_RECSYS_MCUID);
		}
		
		AfxMessageBox( achBuf );
		return FALSE;
	}
    */
	m_wMcuId = LOCAL_MCUID;

    /*
	bFlag = IsLegality( nValue );
	if(bFlag == FALSE)
	{
		m_wMcuId = LOCAL_MCUID;
	}
    */

	// MCU IP 地址
	if ( !::GetRegKeyString( m_szCfgFilename, SECTION_RECSYS, KEY_RECSYS_MCUIPADDR, 
						  "\0", achBuf,sizeof(achBuf) ) )
	{
		if( m_bOSEn )
		{
			sprintf( achBuf ,"%sKey    :%s\nRead the MCU IP failed.",achHead,KEY_RECSYS_MCUIPADDR);
		}
		else
		{
			sprintf( achBuf ,"%sKey    :%s\n读取MCU IP地址失败",achHead,KEY_RECSYS_MCUIPADDR);
		}
		AfxMessageBox( achBuf );
		return FALSE;
	}
	m_dwMcuIpAddr = inet_addr(achBuf);

	bFlag = IsLegality( achBuf );	// 如果Mcu Ip为空，则认为是本地地址
	if(!bFlag)
	{
		m_dwMcuIpAddr = inet_addr( LOCAL_IP );
	}

	// MCU IP Connect Port
	if ( !::GetRegKeyInt( m_szCfgFilename, SECTION_RECSYS, KEY_RECSYS_MCUCONNPORT, 
						  0, &nValue ) )
	{
		if( m_bOSEn )
		{
			sprintf( achBuf ,"%sKey    :%s\nRead the MCU connecting port failed.",achHead,KEY_RECSYS_MCUCONNPORT);
		}
		else
		{
			sprintf( achBuf ,"%sKey    :%s\n读取MCU 连接端口号失败",achHead,KEY_RECSYS_MCUCONNPORT);
		}
		
		nValue = MCU_LISTEN_PORT;
		return FALSE;
	}
	m_wMcuConnPort = (u16)nValue;
	
	bFlag = IsLegality(nValue);
	if(!bFlag)
	{
		m_wMcuConnPort = MCU_LISTEN_PORT;
	}
	
/*
    // MCU ID B
	if ( !::GetRegKeyInt( m_szCfgFilename, SECTION_RECSYS, KEY_RECSYS_MCUIDB, 
						  0, &nValue ) )
	{
		sprintf( achBuf ,"%sKey    :%s\n读取Mcu ID失败",achHead,KEY_RECSYS_MCUIDB);
	}
	wMcuIdB = (u8)nValue;

	bFlag = IsLegality( nValue );
	if(bFlag == FALSE)
	{
		wMcuIdB = LOCAL_MCUID;
	}

	
	// MCUB IP 地址
	if ( !::GetRegKeyString( m_szCfgFilename, SECTION_RECSYS, KEY_RECSYS_MCUIPADDRB, 
						  "\0", achBuf,sizeof(achBuf) ) )
	{
		sprintf( achBuf ,"%sKey    :%s\n读取Mcu IP地址失败",achHead,KEY_RECSYS_MCUIPADDRB);
        dwMcuIpAddrB = 0;
	}
    else
    {
        dwMcuIpAddrB = inet_addr(achBuf);
    }

	// MCUB Connect Port
	if ( !::GetRegKeyInt( m_szCfgFilename, SECTION_RECSYS, KEY_RECSYS_MCUCONNPORTB, 
						  0, &nValue ) )
	{
		sprintf( achBuf ,"%sKey    :%s\n读取Mcu 连接端口号失败",achHead,KEY_RECSYS_MCUCONNPORTB);
        nValue = MCU_LISTEN_PORT;
	}
	wMcuConnPortB = (u16)nValue;
	
    if(dwMcuIpAddr == dwMcuIpAddrB) // 保护
    {
        dwMcuIpAddrB = 0;
        wMcuConnPortB = 0;
    }
	 
    if(0 == dwMcuIpAddr && 0 == dwMcuIpAddrB)
    {
        // AfxMessageBox("The McuA and McuB's Ip are all 0, refuse\n");
        // return FALSE;
    }*/ 

	// Play channels
	if ( !::GetRegKeyInt( m_szCfgFilename, SECTION_RECSYS, KEY_RECSYS_PLAYCHNUM, 
						  0, &nValue ) )
	{
		if( m_bOSEn )
		{
			sprintf( achBuf ,"%sKey    :%s\nRead the play channel number failed.",achHead,KEY_RECSYS_PLAYCHNUM);
		}
		else
		{
			sprintf( achBuf ,"%sKey    :%s\n读取放像通道个数失败",achHead,KEY_RECSYS_PLAYCHNUM);
		}
		AfxMessageBox( achBuf );
		return FALSE;
	}
	m_byPlayChnNum = (u8)nValue;
	bFlag = IsLegality(nValue);
	if(bFlag == FALSE)
	{
		m_byPlayChnNum = 0;
	}

	// Recorder channels
	if (!::GetRegKeyInt(m_szCfgFilename, SECTION_RECSYS, KEY_RECSYS_RECCHNUM, 0, &nValue))
	{
		if( m_bOSEn )
		{
			sprintf( achBuf ,"%sKey    :%s\nRead the record channel number failed.",achHead,KEY_RECSYS_RECCHNUM);
		}
		else
		{
			sprintf( achBuf ,"%sKey    :%s\n读取录像通道个数失败",achHead,KEY_RECSYS_RECCHNUM);
		}
		
		AfxMessageBox( achBuf );
		return FALSE;
	}
	m_byRecChnNum = (u8)nValue;
	bFlag = IsLegality(nValue);
	if(bFlag == FALSE)
	{
		m_byRecChnNum = 0;
	}

    // 检测端口之间是否有足够的间隔
    BOOL32 bEnough = TRUE;
    if(m_wPlayStartPort < m_wRecStartPort)
    {
        bEnough = ( (m_wRecStartPort - m_wPlayStartPort) >= (m_byPlayChnNum*PORTSPAN) ) ;
        if (!bEnough)
        {
            // 后退到足够空间
            m_wPlayStartPort = m_wRecStartPort - m_byPlayChnNum*PORTSPAN;
        }
    }
    else
    {
        bEnough = ( (m_wPlayStartPort - m_wRecStartPort) >= (m_byRecChnNum*PORTSPAN) );
        if (!bEnough)
        {
            // 后退到足够空间
            m_wRecStartPort = m_wPlayStartPort - m_byRecChnNum*PORTSPAN;
        }
    }


	m_byTotalChnNum = m_byRecChnNum + m_byPlayChnNum;
	if ((m_byTotalChnNum ==0) || ( m_byTotalChnNum >MAXNUM_RECORDER_CHNNL))
	{
		if( m_bOSEn )
		{
			sprintf(achBuf,"The largest number of the play and record channels of the recorder is %d。\nThe configuration requre the %d play channels, the %d record channels。\nWill start recorder as the minist configuration, please modify the configuration.",
			MAXNUM_RECORDER_CHNNL, m_byPlayChnNum, m_byRecChnNum );
		}
		else
		{
			sprintf(achBuf,"本录像机放像和录像通道总数最大为%d。\n配置要求放像通道%d ,录像通道%d。\n将按照最小配置启动，请修改配置。",
			MAXNUM_RECORDER_CHNNL, m_byPlayChnNum, m_byRecChnNum );
		}
		AfxMessageBox( achBuf );

        // 默认配置启动
		m_byRecChnNum = 3;
        m_byPlayChnNum = 3;
        m_byTotalChnNum = m_byRecChnNum + m_byPlayChnNum;
	}

	memset(m_achMSInfo, 0, sizeof(m_achMSInfo));

	// 是否支持发布
	if (!::GetRegKeyInt(m_szCfgFilename, SECTION_RECSYS, KEY_RECSYS_ISPUBLISH, 0, &nValue))
	{
		if( m_bOSEn )
		{
			sprintf( achBuf ,"%sKey    :%s\nRead wether support the publishing failed.",achHead,KEY_RECSYS_RECCHNUM);
		}
		else
		{
			sprintf( achBuf ,"%sKey    :%s\n读取是否支持发布失败",achHead,KEY_RECSYS_RECCHNUM);
		}
		AfxMessageBox( achBuf );
		return FALSE;
	}
	m_bMediaServer = (BOOL)nValue;
	
	// 数据库IP地址
	if ( !::GetRegKeyString(m_szCfgFilename, SECTION_RECSYS, KEY_RECSYS_DBHOSTIP, 
		"\0", m_achDBHostIP, sizeof(m_achDBHostIP)))
	{
		if( m_bOSEn )
		{
			strcat(m_achMSInfo, "Get the database's IP failed. Key=\"DBHostIP\"\n");
		}
		else
		{
			strcat(m_achMSInfo, "获取数据库IP地址失败 Key=\"DBHostIP\"\n");
		}
		//bMediaServer = FALSE;
		AfxMessageBox(m_achMSInfo);
		return FALSE;
	}
	if(m_achDBHostIP[0] >= '0' && m_achDBHostIP[0] <= '9')
	{
		m_dwDBHostIP = inet_addr(m_achDBHostIP);
	}
	else
	{
		m_dwDBHostIP= 0;
	}
	
	bFlag = IsLegality(m_achDBHostIP);
	if((bFlag == FALSE) || (m_dwDBHostIP == 0))
	{
		if( m_bMediaServer )
		{
			if( m_bOSEn )
				AfxMessageBox("The DB Ip can not be Null");
			else
				AfxMessageBox("数据库IP地址不能为空！");
			
		}
	}
	
	//数据库用户名
	if (!::GetRegKeyString(m_szCfgFilename, SECTION_RECSYS, KEY_RECSYS_USERNAME,
		 "\0", m_achDBUsername, sizeof(m_achDBUsername)))
	{
		if( m_bOSEn )
		{
			strcat(m_achMSInfo ,"Get username failed. Key=\"DBUserName\"\n");
		}
		else
		{
			strcat(m_achMSInfo ,"获取用户名失败 Key=\"DBUserName\"\n");
		}
		AfxMessageBox( achBuf );
		return FALSE;
	}
	
	
	//数据库用户密码
	if (!::GetRegKeyString(m_szCfgFilename, SECTION_RECSYS, KEY_RECSYS_PASSWORD, 
		 "\0", m_achDBPassword, sizeof(m_achDBPassword)))
	{
		if( m_bOSEn )
		{
			strcat(m_achMSInfo ,"Get the database user password failed. Key=\"DBPassWord\"\n");
		}
		else
		{
			strcat(m_achMSInfo ,"获取数据库用户密码失败 Key=\"DBPassWord\"\n");
		}
		AfxMessageBox( achBuf );
		return FALSE;
	}		
	
	//FTP 路径
	if (!::GetRegKeyString(m_szCfgFilename, SECTION_RECSYS, KEY_RECSYS_VODMMSPATH, 
		 "\0", m_achVodFtpPatch, sizeof(m_achVodFtpPatch)))
	{
		if( m_bOSEn )
		{
			strcat(m_achMSInfo, "Get the FTP path failed. Key=\"VodMMSPath\"\n");
		}
		else
		{
			strcat(m_achMSInfo, "FTP 路径失败 Key=\"VodMMSPath\"\n");
		}
		AfxMessageBox(achBuf);
		return FALSE;
	}
	
	//FTP用户名
	if (!::GetRegKeyString(m_szCfgFilename, SECTION_RECSYS, KEY_RECSYS_MMSUSERNAME, 
		 "\0", m_achFTPUsername, sizeof(m_achFTPUsername)))
	{
		if( m_bOSEn )
		{
			strcat(m_achMSInfo, "Get the FTP username failed. Key=\"MMSUserName\"\n");
		}
		else
		{
			strcat(m_achMSInfo, "获取FTP用户名失败 Key=\"MMSUserName\"\n");
		}	
	}
	
	//FTP密码
	if (!::GetRegKeyString(m_szCfgFilename, SECTION_RECSYS, KEY_RECSYS_MMSPASSWORD, 
		 "\0", m_achFTPPassword, sizeof(m_achFTPUsername)))
	{
		if( m_bOSEn )
		{
			strcat(m_achMSInfo, "Get the FTP password failed. Key=\"MMSPassWord\"\n");
		}
		else
		{
			strcat(m_achMSInfo, "获取FTP密码失败 Key=\"MMSPassWord\"\n");
		}
	}


    // 是否允许运行多个，默认允许[06/07/27]
    if (!::GetRegKeyInt( m_szCfgFilename, SECTION_RECSYS, KEY_RECSYS_MULTIINSTANCE,
		                 1, &nValue ))
    {
        nValue = 1;
    }
    m_bMultiInst = nValue;

// [pengjie 2010/9/26] 切大文件配置化支持
    // 设置录像机的最大录像文件大小（超过则会切文件）单位：M
    if (!::GetRegKeyInt( m_szCfgFilename, SECTION_RECSYS, "MaxRecFileLen",
		1200, &nValue ))
    {
		::SetRegKeyInt( m_szCfgFilename, SECTION_RECSYS, "MaxRecFileLen", 1200);
        nValue = 1200;
    }
    m_dwMaxRecFileLen = nValue * 1024 * 1024;
// End

	s8 achProfileName [KDV_MAX_PATH] = {0};

    GetExecuteProgramPath(achProfileName);
    strcat(achProfileName, RECDBGCFG_FILE);

#ifdef REC_CPPUNIT_TEST
	//Telnet Port
	if ( !::GetRegKeyInt( achProfileName, SECTION_RECSYS,
		                  KEY_RECSYS_TELNETPORT, 0, &nValue ) )
	{
		m_wTelnetPort = (u16)nValue;	//配置文件中没有Telnet port项设置为0
	}

	m_bCppUnitTest = TRUE;
	//读取 CPP UNIT 测试时测试服务IP地址
	if ( !::GetRegKeyString( achProfileName,
		                     "RecorderDebugTestSystem", 
		                     "CPPUnitTestServerIpAddr",
		                     "", achBuf, sizeof(achBuf) ) )
	{
		m_bCppUnitTest = FALSE;
	}
	else
	{
		m_dwCppUnitIpAddr = inet_addr(achBuf);
	}

	//读取 CPP UNIT 测试时测试服务端口
	if ( !::GetRegKeyInt( achProfileName, 
		                  "RecorderDebugTestSystem", 
						  "CPPUnitTestServerPort" , 
						  0, &nValue ) )
	{
		m_bCppUnitTest = FALSE;
	}
	else
	{
		m_wCppUnitPort = (u16)nValue;
	}
#endif

	//[6/1/2012 zhangli]  丢帧时是否写文件
	if (!::GetRegKeyInt(achProfileName, SECTION_RECORDERDEBUGINFO, "IsWriteAsfWhenLoseFrame", 0, &nValue))
    {
		nValue = 0;
    }
    m_bIsWriteAsfWhenLoseFrame = nValue;
    
	//[6/1/2012 zhangli]  丢帧时是否请求关键帧
	if (!::GetRegKeyInt(achProfileName, SECTION_RECORDERDEBUGINFO, "IsNeedKeyFrameWhenLoseFrame", 0, &nValue))
    {
		nValue = 0;
    }
    m_bIsNeedKeyFrameWhenLoseFrame = nValue;
	
	//[2012/3/19 zhangli] 帧长数据信息
	::GetRegKeyInt(achProfileName, SECTION_RECORDERDEBUGINFO, "FrameSizeInfoNum", 0, &nValue);
	
	s8 achSection[32] = {0};
	s8    achReturn[MAX_VALUE_LEN + 1];
	s8    *pchToken = NULL;
	s8 achSeps[] = " \t";
	TFrameSizeInfo tFrameSizeInfo;
	
	for (u8 byLoop = 0; byLoop < nValue; ++byLoop)
	{
		memset(achSection, 0, sizeof(achSection));
		
		sprintf(achSection, "FrameSizeInfo%d", byLoop);
		
		::GetRegKeyString(achProfileName, SECTION_RECORDERDEBUGINFO, achSection, "", achReturn, MAX_VALUE_LEN+1);
		
		//media type
		pchToken = strtok(achReturn, achSeps);
        if (NULL == pchToken)
        {
            LogPrint(LOG_LVL_ERROR, MID_RECEQP_REC, "Wrong profile while reading %s!\n",achSection);
            continue;
        }
        else
        {
            tFrameSizeInfo.m_byMediaType = atoi(pchToken);
        }
		
		//format
		pchToken = strtok(NULL, achSeps);
        if (NULL == pchToken)
        {
            LogPrint(LOG_LVL_ERROR, MID_RECEQP_REC, "Wrong profile while reading %s!\n",achSection);
            continue;
        }
        else
        {
            tFrameSizeInfo.m_dwFrameSize = atoi(pchToken);
        }
		
		SetFrameSizeInfo(tFrameSizeInfo);
	}

	//[6/1/2012 zhangli]  音频buff大小，默认为16
	if (!::GetRegKeyInt(achProfileName, SECTION_RECORDERDEBUGINFO, "audioBuffSize", MAX_BUF_NUM, &nValue))
    {
		nValue = MAX_BUF_NUM;
    }
    m_byAudioBuffSize = nValue;

	//[6/1/2012 zhangli]  视频buff大小，默认为16
	if (!::GetRegKeyInt(achProfileName, SECTION_RECORDERDEBUGINFO, "videoBuffSize", MAX_BUF_NUM, &nValue))
    {
		nValue = MAX_BUF_NUM;
    }
    m_byVideoBuffSize = nValue;

	//[6/1/2012 zhangli]  双流buff大小，默认为16
	if (!::GetRegKeyInt(achProfileName, SECTION_RECORDERDEBUGINFO, "dsBuffSize", MAX_BUF_NUM, &nValue))
    {
		nValue = MAX_BUF_NUM;
    }
    m_byDsBuffSize = nValue;

	//[6/1/2012 zhangli]  最大剩余buff大小，默认为10
	if (!::GetRegKeyInt(achProfileName, SECTION_RECORDERDEBUGINFO, "leftBuffSize", 10, &nValue))
    {
		nValue = 10;
    }
    m_byLeftBuffSize = nValue;

	if (!::GetRegKeyInt(achProfileName, SECTION_RECORDERDEBUGINFO, "minSizeForBroadcast", 512, &nValue))
    {
		nValue = 512;
    }
	
	m_wMinSizeForBroadcast = nValue;

    m_bInited = TRUE;
	return m_bInited;

}

/*====================================================================
函数名      :SetFrameSizeInfo
功能        :设置帧长数据信息
算法实现    :
引用全局变量:
输入参数说明: u8 byMediaType 媒体类型
u32 dwFrameSize帧长
返回值说明  :TRUE or FLASE
----------------------------------------------------------------------
修改记录    :
日  期      版本        修改人        修改内容
2012/06/17	4.7			zhangli		  create
====================================================================*/
BOOL32 TCfgAgent::SetFrameSizeInfo(TFrameSizeInfo &tFrameSizeInfo)
{
	if (tFrameSizeInfo.m_byMediaType == 0 || tFrameSizeInfo.m_dwFrameSize == 0)
	{
		LogPrint(LOG_LVL_ERROR, MID_RECEQP_RPCTRL, "SetFrameSizeInfo parma error, byMediaType:%d, dwFrameSize:%d\n",
			tFrameSizeInfo.m_byMediaType, tFrameSizeInfo.m_dwFrameSize);
		
		return FALSE;
	}
	
	for (u8 byLoop = 0; byLoop < MAX_AUDIO_TYPE_NUM; ++byLoop)
	{
		if (m_atFrameSize[byLoop].m_byMediaType == 0 || m_atFrameSize[byLoop].m_dwFrameSize == 0)
		{
			memcpy(&m_atFrameSize[byLoop], &tFrameSizeInfo, sizeof(TFrameSizeInfo));
			return TRUE;
		}
	}
	return FALSE;
}

/*====================================================================
    函数名	     ：Write
	功能		 ：把配置参数写入配置文件
	引用全局变量 ：无
    输入参数说明 ：无
	返回值说明   ：成功返回 TRUE，否则返回FALSE
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
    04/12/31    3.6         libo          create
	06/01/18	4.0			张宝卿		  adapt to read only config file
====================================================================*/
BOOL TCfgAgent::WriteConfigIntoFile()
{
	s8   achBuf[256];
	s8   achHead[256];
	s32 nValue;
    struct in_addr in;

	if( m_bOSEn )
	{
		sprintf(achHead, "The current configuration file : %s\nSection: %s\n",
		    m_szCfgFilename, SECTION_RECSYS);
	}
	else
	{
		sprintf(achHead, "当前配置文件 : %s\nSection: %s\n",
		    m_szCfgFilename, SECTION_RECSYS);
	}

   
	
	//改变配置文件的属性为Normal, 以保存修改后的参数. 
	CFile* pFile = NULL;
	CFileStatus cFileOrignalStatus;
	memset( &cFileOrignalStatus, 0, sizeof(CFileStatus) );
	CFileStatus cFileChangedStatus;
	memset( &cFileChangedStatus, 0, sizeof(CFileStatus) );
	pFile->GetStatus( m_szCfgFilename, cFileOrignalStatus );
	pFile->GetStatus( m_szCfgFilename, cFileChangedStatus );

	if ( 0x00 != cFileOrignalStatus.m_attribute ) 
	{
		cFileChangedStatus.m_attribute = 0x00;
		pFile->SetStatus(m_szCfgFilename, cFileChangedStatus);
	}
	
	//录像IP地址
    in.s_addr = m_dwRecIpAddr;
    sprintf(m_achLocalIP, "%s", inet_ntoa(in));
	if (!::SetRegKeyString(m_szCfgFilename, SECTION_RECSYS, KEY_RECSYS_IPADDR, m_achLocalIP))
	{
		if( m_bOSEn )
		{
			sprintf(achBuf ,"%sKey    :%s\nWrite the record IP failed.", achHead, KEY_RECSYS_IPADDR);
		}
		else
		{
			sprintf(achBuf ,"%sKey    :%s\n写录像IP地址失败", achHead, KEY_RECSYS_IPADDR);
		}
		AfxMessageBox(achBuf);
		return FALSE;
	}

    //外设别名
	if (!::SetRegKeyString(m_szCfgFilename, SECTION_RECSYS, KEY_RECSYS_PERIEQPALIAS, m_szAlias))
	{
		if( m_bOSEn )
		{
			sprintf(achBuf, "%sKey    :%s\nWrite the alias failed.",achHead,KEY_RECSYS_PERIEQPALIAS);
		}
		else
		{
			sprintf(achBuf, "%sKey    :%s\n写别名失败",achHead,KEY_RECSYS_PERIEQPALIAS);
		}
		AfxMessageBox(achBuf);
		return FALSE;
	}
	
	//外设ID
    nValue = (s32)m_byEqpId;
	if (!::SetRegKeyInt(m_szCfgFilename, SECTION_RECSYS, KEY_RECSYS_PERIEQPID, nValue))
	{
		if( m_bOSEn )
		{
			sprintf(achBuf, "%sKey    :%s\nWrite the peripheral ID failed.\n", achHead, KEY_RECSYS_PERIEQPID);
		}
		else
		{
			sprintf(achBuf, "%sKey    :%s\n写外设ID号失败\n", achHead, KEY_RECSYS_PERIEQPID);
		}
		AfxMessageBox(achBuf);
		return FALSE;
	}

	//外设类型---固定值
	// m_byEqpType = EQP_TYPE_RECORDER;	


	//录像机接收起始端口号
    nValue = (s32)m_wRecStartPort;
	if (!::SetRegKeyInt( m_szCfgFilename, SECTION_RECSYS, KEY_RECSYS_RCVPORT, nValue))
	{
		if( m_bOSEn )
		{
			sprintf( achBuf ,"%sKey    :%s\nWrite the receive port failed.",achHead,KEY_RECSYS_RCVPORT);
		}
		else
		{
			sprintf( achBuf ,"%sKey    :%s\n写录像接收起始端口号失败",achHead,KEY_RECSYS_RCVPORT);
		}
		AfxMessageBox(achBuf);
		return FALSE;
	}	
    // 放像起始端口
    nValue = m_wPlayStartPort;
    if (!::SetRegKeyInt( m_szCfgFilename, SECTION_RECSYS, KEY_RECSYS_PLAYPORT, nValue))
	{
		if( m_bOSEn )
		{
			sprintf( achBuf ,"%sKey    :%s\nWrite the play port failed.",achHead, KEY_RECSYS_PLAYPORT);
		}
		else
		{
			sprintf( achBuf ,"%sKey    :%s\n写放像接收起始端口号失败",achHead, KEY_RECSYS_PLAYPORT);
		}		
		AfxMessageBox(achBuf);
		return FALSE;
	}

	//rec path
	if (!::SetRegKeyString(m_szCfgFilename, SECTION_RECSYS, KEY_RECSYS_RECFILEPATH, m_szRecordPath))
	{
		if( m_bOSEn )
		{
			sprintf( achBuf ,"%sKey    :%s\nWrite the path of saving record file failed.",achHead,KEY_RECSYS_IPADDR);
		}
		else
		{
			sprintf( achBuf ,"%sKey    :%s\n写录像存储路径失败",achHead,KEY_RECSYS_IPADDR);
		}
		AfxMessageBox(achBuf);
		return FALSE;
	}

    /*
	//MCU ID
    nValue = (s32)m_wMcuId;
	if (!::SetRegKeyInt(m_szCfgFilename, SECTION_RECSYS, KEY_RECSYS_MCUID, nValue))
	{
		if( m_bOSEn )
		{
			sprintf(achBuf ,"%sKey    :%s\nWrite the MCU ID failed.",achHead,KEY_RECSYS_MCUID);
		}
		else
		{
			sprintf(achBuf ,"%sKey    :%s\n写MCU ID失败",achHead,KEY_RECSYS_MCUID);
		}
		AfxMessageBox(achBuf);
		return FALSE;
	}
    */
    
	/*
    if(dwMcuIpAddr == dwMcuIpAddrB) // 保护
    {
        dwMcuIpAddrB = 0;
        wMcuConnPortB = 0;
    }
    
    if(0 == dwMcuIpAddr && 0 == dwMcuIpAddrB)
    {
        AfxMessageBox("The McuA and McuB's Ip are all 0, refuse\n");
        return FALSE;
    }*/

	//MCU IP 地址
    in.s_addr = m_dwMcuIpAddr;
    sprintf(achBuf, "%s", inet_ntoa(in));
	if (!::SetRegKeyString(m_szCfgFilename, SECTION_RECSYS, KEY_RECSYS_MCUIPADDR, achBuf))
	{
		if( m_bOSEn )
		{
			sprintf(achBuf, "%sKey    :%s\nWrite the MCU IP failed.", achHead, KEY_RECSYS_MCUIPADDR);
		}
		else
		{
			sprintf(achBuf, "%sKey    :%s\n写MCU IP地址失败", achHead, KEY_RECSYS_MCUIPADDR);
		}
		AfxMessageBox(achBuf);
		return FALSE;
	}

	//MCU IP Connect Port
    nValue = (s32)m_wMcuConnPort;
	if (!::SetRegKeyInt(m_szCfgFilename, SECTION_RECSYS, KEY_RECSYS_MCUCONNPORT, nValue))
	{
		if( m_bOSEn )
		{
			sprintf(achBuf, "%sKey    :%s\nWrite the MCU connecting port failed.", achHead, KEY_RECSYS_MCUCONNPORT);
		}
		else
		{
			sprintf(achBuf, "%sKey    :%s\n写MCU 连接端口号失败", achHead, KEY_RECSYS_MCUCONNPORT);
		}
		AfxMessageBox(achBuf);
		return FALSE;
	}

    /*
    //MCU ID
    nValue = (s32)wMcuIdB;
	if (!::SetRegKeyInt(m_szCfgFilename, SECTION_RECSYS, KEY_RECSYS_MCUIDB, nValue))
	{
		sprintf(achBuf ,"%sKey    :%s\n写MCU ID失败",achHead, KEY_RECSYS_MCUIDB);
		AfxMessageBox(achBuf);
		return FALSE;
	}
	
	//MCU IP 地址
    if( 0 != dwMcuIpAddrB)
    {
        in.s_addr = dwMcuIpAddrB;
        sprintf(achBuf, "%s", inet_ntoa(in));
    }
    else
    {
        memcpy(achBuf, "0", sizeof("0"));
    }

	if (!::SetRegKeyString(m_szCfgFilename, SECTION_RECSYS, KEY_RECSYS_MCUIPADDRB, achBuf))
	{
		sprintf(achBuf, "%sKey    :%s\n写MCU IP地址失败", achHead, KEY_RECSYS_MCUIPADDRB);
		AfxMessageBox(achBuf);
		// return FALSE;
	}

	//MCU IP Connect Port
    nValue = (s32)wMcuConnPortB;
	if (!::SetRegKeyInt(m_szCfgFilename, SECTION_RECSYS, KEY_RECSYS_MCUCONNPORTB, nValue))
	{
		sprintf(achBuf, "%sKey    :%s\n写MCU 连接端口号失败", achHead, KEY_RECSYS_MCUCONNPORTB);
		AfxMessageBox(achBuf);
		// return FALSE;
	}
	*/
	

	m_byTotalChnNum = m_byRecChnNum + m_byPlayChnNum;
	if ((m_byTotalChnNum == 0) || (m_byTotalChnNum > MAXNUM_RECORDER_CHNNL))
	{
		if( m_bOSEn )
		{
			sprintf(achBuf,"The largest number of the play and record channels of the recorder is %d。\nThe configuration requre the %d play channels, the %d record channels。\nWill start recorder as the minist configuration, please modify the configuration.",
			MAXNUM_RECORDER_CHNNL, m_byPlayChnNum, m_byRecChnNum );
		}
		else
		{
			sprintf(achBuf,"本录像机放像和录像通道总数最大为%d。\n配置要求放像通道%d ,录像通道%d。\n将按照最小配置启动，请修改配置。",
			MAXNUM_RECORDER_CHNNL, m_byPlayChnNum, m_byRecChnNum );
		}
		AfxMessageBox( achBuf );

        // 默认配置启动
		m_byRecChnNum = 3;
        m_byPlayChnNum = 3;
        m_byTotalChnNum = m_byRecChnNum + m_byPlayChnNum;
	}
    
	//Play channels
    nValue = (s32)m_byPlayChnNum;
	if (!::SetRegKeyInt(m_szCfgFilename, SECTION_RECSYS, KEY_RECSYS_PLAYCHNUM, nValue))
	{
		if( m_bOSEn )
		{
			sprintf(achBuf, "%sKey    :%s\nWrite the play channel number failed.", achHead, KEY_RECSYS_PLAYCHNUM);
		}
		else
		{
			sprintf(achBuf, "%sKey    :%s\n写放像通道个数失败", achHead, KEY_RECSYS_PLAYCHNUM);
		}
		AfxMessageBox(achBuf);
		return FALSE;
	}

	//Recorder channels
    nValue = (s32)m_byRecChnNum;
	if (!::SetRegKeyInt(m_szCfgFilename, SECTION_RECSYS, KEY_RECSYS_RECCHNUM, nValue))
	{
		if( m_bOSEn )
		{
			sprintf(achBuf, "%sKey    :%s\nWrite the record channel number failed.", achHead, KEY_RECSYS_RECCHNUM);
		}
		else
		{
			sprintf(achBuf, "%sKey    :%s\n写录像通道个数失败", achHead, KEY_RECSYS_RECCHNUM);
		}
		AfxMessageBox(achBuf);
		return FALSE;
	}

    // 机器名
    if(!::SetRegKeyString(m_szCfgFilename, SECTION_RECSYS, KEY_RECSYS_MACHINENAME, m_szMachineName))
    {
		if( m_bOSEn )
		{
			sprintf(m_szMachineName, "Write the local recorder name failed.");
		}
		else
		{
			sprintf(m_szMachineName, "写本地机器名失败");
		}
        AfxMessageBox(m_szMachineName);
        // return FALSE;
    }
    
    //是否支持发布功能
    nValue = (s32)m_bMediaServer;
    if (!::SetRegKeyInt(m_szCfgFilename, SECTION_RECSYS, KEY_RECSYS_ISPUBLISH, nValue))
	{
		if( m_bOSEn )
		{
			sprintf( achBuf ,"%sKey    :%s\nWrite wether support the publishing failed.", achHead, KEY_RECSYS_ISPUBLISH);
		}
		else
		{
			sprintf( achBuf ,"%sKey    :%s\n写是否支持发布失败", achHead, KEY_RECSYS_ISPUBLISH);
		}
		AfxMessageBox( achBuf );
		return FALSE;
	}

	//数据库IP地址
    in.s_addr = m_dwDBHostIP;
    sprintf(m_achDBHostIP, "%s", inet_ntoa(in));
	if (!::SetRegKeyString(m_szCfgFilename, SECTION_RECSYS, KEY_RECSYS_DBHOSTIP, m_achDBHostIP))
	{
		if( m_bOSEn )
		{
			strcat(m_achMSInfo, "Write the database IP failed. Key=\"DBHostIP\"\n");
		}
		else
		{
			strcat(m_achMSInfo, "写数据库IP地址失败 Key=\"DBHostIP\"\n");
		}
		AfxMessageBox(m_achMSInfo);
		return FALSE;
	}	    

	//数据库用户名
	if (!::SetRegKeyString(m_szCfgFilename, SECTION_RECSYS, KEY_RECSYS_USERNAME, m_achDBUsername))
	{
		if( m_bOSEn )
		{
			strcat(m_achMSInfo ,"Write the username failed. Key=\"DBUserName\"\n");
		}
		else
		{
			strcat(m_achMSInfo ,"写用户名失败 Key=\"DBUserName\"\n");
		}
		AfxMessageBox(m_achMSInfo);
		return FALSE;
	}

	//数据库用户密码
	if (!::SetRegKeyString(m_szCfgFilename, SECTION_RECSYS, KEY_RECSYS_PASSWORD, m_achDBPassword))
	{
		if( m_bOSEn )
		{
			strcat(m_achMSInfo ,"Write the database user password failed. Key=\"DBPassWord\"\n");
		}
		else
		{
			strcat(m_achMSInfo ,"写数据库用户密码失败 Key=\"DBPassWord\"\n");
		}
		AfxMessageBox(m_achMSInfo);
		return FALSE;
	}		
	
	//FTP 路径
	if (!::SetRegKeyString(m_szCfgFilename, SECTION_RECSYS, KEY_RECSYS_VODMMSPATH, m_achVodFtpPatch))
	{
		if( m_bOSEn )
		{
			strcat(m_achMSInfo, "Write the FTP path failed. Key=\"VodFtpPath\"\n");
		}
		else
		{
			strcat(m_achMSInfo, "写FTP 路径失败 Key=\"VodFtpPath\"\n");
		}
		AfxMessageBox(m_achMSInfo);
		return FALSE;
	}
    
	//FTP用户名
	if (!::SetRegKeyString(m_szCfgFilename, SECTION_RECSYS, KEY_RECSYS_MMSUSERNAME, m_achFTPUsername))
	{
		if( m_bOSEn )
		{
			strcat(m_achMSInfo, "Write the FTP username failed. Key=\"FTPUserName\"\n");
		}
		else
		{
			strcat(m_achMSInfo, "写FTP用户名失败 Key=\"FTPUserName\"\n");
		}
		AfxMessageBox(m_achMSInfo);
		return FALSE;
	}
	
	//FTP密码
	if (!::SetRegKeyString(m_szCfgFilename, SECTION_RECSYS, KEY_RECSYS_MMSPASSWORD, m_achFTPPassword))
	{
		if( m_bOSEn )
		{
			strcat(m_achMSInfo, "Write the FTP password failed. Key=\"FTPPassWord\"\n");
		}
		else
		{
			strcat(m_achMSInfo, "写FTP密码失败 Key=\"FTPPassWord\"\n");
		}
		AfxMessageBox(m_achMSInfo);
		return FALSE;
	}

    /*
	//恢复原来的文件属性
	if ( 0x00 != cFileOrignalStatus.m_attribute )
	{
		pFile->SetStatus( m_szCfgFilename, cFileOrignalStatus );
	}
    */
    
	return TRUE;
}

// 判断参数是否合法
BOOL TCfgAgent::IsLegality(s32 nNum )
{
	if( nNum < 0 )
	{
		ErrorMsg( 1 );
		return FALSE;

	}
	else
	{
		ErrorMsg( 0 );
		return TRUE;
	}

}

BOOL TCfgAgent::IsLegality(s8* aChar)
{
	s32 nLeng = -1;
	nLeng = strlen(aChar);
	if(nLeng == 0)
	{
		ErrorMsg( 4 );
		return FALSE;
	}
	else
	{
		//ErrorMsg(0);
		return TRUE;
	}
}

void TCfgAgent::ErrorMsg(int nNum)
{
	switch(nNum)
	{
	case -1:
		break;

	case 0:
		// RecLog("Success!");
		break;

	case 1:
		RecLog("The Parameter can not be negative");
		break;

	case 2:
		RecLog("The Parameter is more small");
		break;

	case 3:
		RecLog("The Parameter is more Big");
		break;

	case 4:
		RecLog( "The String can not be Null");
		break;

	case 5:
		RecLog("Read the config file Fail");
		break;

	case 6:
		RecLog("Write the config file Fail");
		break;

	case 7:
		RecLog("Call the interface Fail");
		break;

	case 8:
		break;
	case 9:
		break;
	case 10:
		break;
	default:
		break;
	}
}

/*=============================================================================
  函 数 名： IsLegalityPort
  功    能： 判断两个端口是否有足够的间隔
  算法实现： 
  全局变量： 
  参    数： u16 wPort1
             u16 wPort2
  返 回 值： BOOL32 
=============================================================================*/
BOOL32 TCfgAgent::IsLegalityPort(u16 wPort1, u16 wPort2)
{
    return TRUE;
}

void TCfgAgent::RecLog(char * fmt,...)
{
	char PrintBuf[255];
    int BufLen = 0;
    va_list argptr;
	if( bPrintLog )
	{		  
		BufLen = sprintf(PrintBuf,"[Rec]:"); 
		va_start(argptr, fmt);
		BufLen += vsprintf(PrintBuf+BufLen,fmt,argptr); 
		va_end(argptr); 
		BufLen += sprintf(PrintBuf+BufLen,"\n"); 
		OspPrintf(1,0,PrintBuf); 
	}
}