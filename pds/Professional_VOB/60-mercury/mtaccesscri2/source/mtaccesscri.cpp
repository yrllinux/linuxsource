#include "osp.h"
#include "mcustruct.h"
#include "mcuver.h"
#include "cri_agent.h"
#include "bindmtadp.h"
#include "bindmp.h"

#ifdef _LINUX_
	#include "dataswitch_v6.h"
#endif

#ifdef WIN32
	#include "dataswitch.h"
#endif

//#include "kdvsys.h"
#include "eqplib.h"
#include "boardagent.h"
//#include <stdio.h>

// [pengjie 2010/2/27] CRI2 pxy 支持
#include "firewallproxyserverlib.h" 
// [pengjie 2010/6/12] CRI2 pxy库整合
#include "protocolnipinterface.h"

//#include "kdvlog.h"
//#include "loguserdef.h"


#if defined(WIN32) || defined(_WIN32)
#define CRI_KDVLOG_CONF_FILE	(s8*)"./conf/kdvlog_cri.ini"
#else
#define CRI_KDVLOG_CONF_FILE	(s8*)"/usr/etc/config/conf/kdvlog_cri.ini"
#endif


/////////////////////////////////////////////////////Linux 下异常捕获处理
#ifdef _LINUX_
 
//#include <stdlib.h>
#include <elf.h>
//#include <sys/types.h>
//#include <unistd.h>
//#include <signal.h>
#include <ucontext.h>
//#include <errno.h>
//#include <string.h>
#include "nipdebug.h"

Elf32_Sym *get_symtab(Elf32_Sym *symtab, int tabsize, int addr);
Elf32_Shdr * read_elf_section(FILE *file, Elf32_Ehdr *ehdr);
void *read_symtable(FILE *file, Elf32_Shdr *shdr, Elf32_Ehdr *ehdr,   int *size);
void *read_strtable(FILE *file, Elf32_Shdr *shdr, Elf32_Ehdr *ehdr,  int *size);
int  read_elf_head(FILE *file, Elf32_Ehdr *ehdr);
Elf32_Sym *get_func_symb_by_addr(Elf32_Sym *symtab, int tabsize, int addr);
int printf_symbl_name(int pc);

Elf32_Sym *sym;
char *strtab;
int symsize, strsize;

#define SECTION_HEADER_INDEX(I)				\
  ((I) < SHN_LORESERVE					\
   ? (I)						\
   : ((I) <= SHN_HIRESERVE				\
      ? 0						\
      : (I) - (SHN_HIRESERVE + 1 - SHN_LORESERVE)))

#define APPNAME     (LPCSTR)"/usr/bin/kdvcri"
LPCSTR g_szAppName;

// 开始捕捉入口函数
// Ret - 0 正常返回
s32 StartCaptureException();

#endif //_LINUX_

/////////////////////////////////////////////////////异常捕获处理

// #include "cri_agent.h"
// #include "cristruct.h"

BOOL ReadBoardConfig(u8 &byBoardId, BOOL &bAttached, u32 &dwMcuIp, u16 &wMcuPort, u32 &dwRegisterIp)
{
	printf( "[ReadBoardConfig] Now come to ReadBoardConfig! \n" );
	if(!InitCriAgent())
	{
		OspPrintf(1, 0, "Starting up board agent failed. Exit!\n");	
		printf("Starting up board agent failed. Exit!\n");
		//cdw
		return FALSE;
	}

	if( g_cBrdAgentApp.GetBoardId() == MCU_BOARD_MPC ||
        g_cBrdAgentApp.GetBoardId() == MCU_BOARD_MPCD )//running on mpc
	{
		bAttached = TRUE;
	}
	else //running on dri
	{
		bAttached = FALSE;
		//byBoardId = g_cBrdAgentApp.GetBoardId() % 16;
		byBoardId = g_cBrdAgentApp.GetBoardId();
		dwMcuIp = g_cBrdAgentApp.GetMpcIpA();
		wMcuPort = g_cBrdAgentApp.GetMpcPortA();
		dwRegisterIp = g_cBrdAgentApp.GetBrdIpAddr();
	}
	return TRUE;
}

BOOL GetMpcBInfo(u32& dwMpcIp, u16& wMpcPort)  // get mpcB's info
{
    dwMpcIp = g_cBrdAgentApp.GetMpcIpB();
    wMpcPort = g_cBrdAgentApp.GetMpcPortB();
    return TRUE;
}

BOOL ReadModuleConfig(BOOL &bStartMtadp, BOOL &bStartMp, BOOL &bStartPrs, BOOL &bStartPxy)
{	
	printf("[ReadModuleConfig] come in! \n");
    int nTemp;	   

    s8 pFileName[KDV_MAX_PATH] = {0};
    sprintf(pFileName, "%s/modcfg.ini", DIR_CONFIG);
	
	//是否启动prs
	bStartPrs = g_cBrdAgentApp.IsRunPrs();

	FILE *tmpFp = fopen(pFileName, "r");
	if(tmpFp == NULL)
	{
		OspPrintf(TRUE, TRUE, "%s not found.\n", pFileName);
		return FALSE;
	}
	else
    {
		fclose(tmpFp);
    }
		
	//是否启动MtAdp
	GetRegKeyInt( pFileName, "Modules", "mtadp" , 0 , &nTemp);
	bStartMtadp = ((u8)nTemp == 1);
	
	//是否启动mp
	GetRegKeyInt( pFileName, "Modules", "mp" , 1 , &nTemp);
	bStartMp = ((u8)nTemp == 1);

	//是否启动Pxy
	GetRegKeyInt( pFileName, "Modules", "pxy" , 0 , &nTemp);
	bStartPxy = ((u8)nTemp == 1);
	printf("[ReadModuleConfig] IsStartPxy = %d \n", bStartPxy);

	return TRUE;

}


BOOL BrdIsHdi()
{
#ifdef _MTACCESS_HDI_
    return TRUE;
#else
    return FALSE;
#endif
}

// [pengjie 2011/2/16] is2.2 支持
void CopyCfgFile()
{
#ifdef _LINUX_
	// 1、如果没有目录创建目录
	mkdir(DIR_CONFIG,0777);
	chmod(DIR_CONFIG,0777);

	// 2、将解压出来的配置文件拷贝到指定目录下
	s32 nReturn = 0;
	s8 achCommand[MAX_PATH];
	memset( achCommand, 0, sizeof(achCommand) ); 
	sprintf( achCommand, "%s %s/%s %s/%s", "cp", DIR_BIN, FILE_MODCFG_INI, DIR_CONFIG, FILE_MODCFG_INI );
	nReturn = system( achCommand );
	printf( "CopyCfgFile modcfg.ini return: %d \n", nReturn );

	memset( achCommand, 0, sizeof(achCommand) );
	sprintf( achCommand, "%s %s/%s %s/%s", "cp", DIR_BIN, FILE_MTADPDEBUG_INI, DIR_CONFIG, FILE_MTADPDEBUG_INI );
	nReturn = system( achCommand );
	printf( "CopyCfgFile mtadpdebug.ini return: %d \n", nReturn );

	// 3、清除之前解压的临时文件
	memset( achCommand, 0, sizeof(achCommand) );
	sprintf( achCommand, "%s %s/%s", "rm -f", DIR_BIN, FILE_MODCFG_INI );
	nReturn = system( achCommand );
	printf( "RemoveCfgFile modcfg.ini return: %d \n", nReturn );

	memset( achCommand, 0, sizeof(achCommand) );
	sprintf( achCommand, "%s %s/%s", "rm -f", DIR_BIN, FILE_MTADPDEBUG_INI );
	nReturn = system( achCommand );
	printf( "RemoveCfgFile mtadpdebug.ini return: %d \n", nReturn );

#endif
	return;
}

// 返回值说明 [pengguofeng 5/17/2012]
/*
 *	ret: 0 - OK, but without Ipv6 address
		-1 - fopen error or invalid parameter
		 1 - OK, and find an Ipv6 address
 */
int get_linklocal_ipv6addr(char *address, s16 &swScopeId, char *iface)
{
/*lint -save -e527*/
/*lint -save -e578*/
#ifdef WIN32
	// win32 just for test[pengguofeng 5/17/2012]
	char * pbyV6Ip = "2001:da8:207::9403";
	memcpy(address, pbyV6Ip, strlen(pbyV6Ip));
	swScopeId = 0x0;
	return 1;
#endif

	if (NULL == iface)
	{
		return 0;
	}
	
	if (NULL == address)
	{
		return 0;
	}
#define IF_INET6 "/proc/net/if_inet6"
	
	char str[128] = {0};
	char *addr, *index, *prefix, *scope, *flags, *name;
	char *delim = " \t\n", *p, *q;
	FILE *fp;
	int count;
	
	if (!address || !iface) {
		printf("address and iface can't be NULL!\n");
		return -1;
	}
	
	if (NULL == (fp = fopen(IF_INET6, "r"))) {
		printf("fopen error");
		//perror("fopen error");
		return -1;
	}
	
	BOOL32 bFind = FALSE;
#define IPV6_ADDR_LINKLOCAL 0x0000U
	while (fgets(str, sizeof(str), fp)) {
		printf("str:%s", str);
		addr = strtok(str, delim);
		index = strtok(NULL, delim);
		prefix = strtok(NULL, delim);
		scope = strtok(NULL, delim);
		flags = strtok(NULL, delim);
		name = strtok(NULL, delim);
		printf("addr:%s, index:0x%s, prefix:0x%s, scope:0x%s, flags:0x%s, name:%s\n",
			addr, index, prefix, scope, flags, name);
		
		if (NULL == scope)
		{
			continue;
		}

		if (NULL == name)
		{
			continue;
		}

		if (strcmp(name, iface))
			continue;
		
		/* Just get IPv6 linklocal address */
		if (IPV6_ADDR_LINKLOCAL != (unsigned int)strtoul(scope, NULL, 16))
//			continue;
		
		if (NULL != address)
		{
			memset(address, 0x00, sizeof(address));
		}		
		p = addr;
		q = address;
		count = 0;

		if (NULL == p)
		{
			continue;
		}
		if (NULL == q)
		{
			continue;
		}
		while (*p != ' ') {
			if (count == 4 ) {
				count = 0;
				*q++ = ':';
			}
			*q++ = *p++;
			count++;
		}

		if (NULL != address)
		{
			address[strlen(address)-1] = '\0';
		}
		
		if (NULL != index)
		{
			swScopeId = atoi(index);
		}
		
		printf("find out %s's linklocal IPv6 address: %s, and dwSocpeId.%d\n", iface, address, swScopeId);
		bFind = TRUE; // 区分返回值 [pengguofeng 5/17/2012]
		break;
	}
#undef IPV6_ADDR_LINKLOCAL
	
	fclose(fp);
	if ( !bFind )
	{
		return 0;
	}
	else
	{
		return 1;
	}

}


/*====================================================================
函数名      ：ReadMtIpMapTab
功能        ：读取终端Ip映射配置
算法实现    ：
引用全局变量：
输入参数说明：TMtIpMapTab &tIpMapTab
返回值说明  ：BOOL32
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2012/05/10  1.0         周嘉麟          创建
====================================================================*/
BOOL32 ReadMtIpMapTab(TMtIpMapTab &tIpMapTab)
{	
	s8   achFullName[64] = {0};
	s32  nEntryNum = 0;
	s8   chSeps[] = " \t"; //空格、者分隔符
	s8*  pchToken = NULL;
	s8** lpszTable;
	u8   byLoop = 0;
	
	sprintf(achFullName, "%s/%s", DIR_CONFIG, "ipmapcfg.ini");
    
	//获取总的条数
	BOOL32 bRet = GetRegKeyInt( achFullName, "MtIpMapTable", "EntryNum", 0, &nEntryNum );
	if( FALSE == bRet ) 
	{
		return bRet;
	}
	if( nEntryNum < 0 )
	{
		OspPrintf( TRUE, FALSE, "[ReadMtIpMapTab] Read MtIpMapTable failed because nEntryNum < %d!\n");
		return FALSE;
	}
	
	if( nEntryNum > MAXNUM_CONF_MT )
	{
		nEntryNum = MAXNUM_CONF_MT;
	}
	
	//分配内存
	lpszTable = (s8 **)malloc( nEntryNum * sizeof( s8* ));
	if (NULL == lpszTable)
	{
		printf("[ReadMtIpMapTab] malloc nEntryNum failed!\n");
		return FALSE;
	}

	for( byLoop = 0; byLoop < (u32)nEntryNum; byLoop++ )
	{
		lpszTable[byLoop] = (s8 *)malloc(MAX_VALUE_LEN + 1);
	}
	
	//读取表
	bRet = GetRegKeyStringTable( achFullName, "MtIpMapTable", "fail", lpszTable, (u32*)&nEntryNum, (MAX_VALUE_LEN+1) );
	if( FALSE == bRet ) 
	{
		OspPrintf( TRUE, FALSE, "[ReadMtIpMapTab] Read MtIpMapTable failed!\n");
		printf( "[ReadMtIpMapTab] Read MtIpMapTable failed!\n");

		//释放内存
		for( byLoop = 0; byLoop < nEntryNum; byLoop++ )
		{
			if( NULL != lpszTable[byLoop] )
			{
				free( lpszTable[byLoop] );
			}
		}
	    free( lpszTable );

		return bRet;
	}
	

	for( byLoop = 0; byLoop < nEntryNum; byLoop++ )
	{
		pchToken = strtok( lpszTable[byLoop], chSeps );
		
		//读取IpV6		
		if( NULL == pchToken )
		{
			printf( "[ReadMtIpMapTab] MtIpMapTable Err while reading %s entryID.%d!\n", "IpV6", byLoop );
			OspPrintf( TRUE, FALSE, "[ReadMtIpMapTab] MtIpMapTable Err while reading %s entryID.%d!\n", "IpV6", byLoop );
			continue;
		}
		else
		{
			tIpMapTab.m_atIpMap[byLoop].SetV6IP(pchToken);
		}
		
		//读取IpV4
		pchToken = strtok( NULL, chSeps );
		if( NULL == pchToken )
		{
			printf( "[ReadMtIpMapTab]MtIpMapTable Err while reading %s entryID.%d!\n", "IpV4", byLoop );
			OspPrintf( TRUE, FALSE, "[ReadMtIpMapTab]MtIpMapTable Err while reading %s entryID.%d!\n", "IpV4", byLoop );
			continue;
		}
		else
		{	
			tIpMapTab.m_atIpMap[byLoop].SetV4IP(ntohl(INET_ADDR(pchToken)));		
		}
		
	}
	
	
	//释放内存
	for( byLoop = 0; byLoop < nEntryNum; byLoop++ )
	{
		if( NULL != lpszTable[byLoop] )
		{
			free( lpszTable[byLoop] );
		}
	}
	free( lpszTable );
	
	return TRUE;
}

/*====================================================================
函数名      ：ReadCriIpMapTab
功能        ：读取接入板Ip映射配置
算法实现    ：
引用全局变量：
输入参数说明：TCriIpMapTab &tCriIpMapTab
返回值说明  ：BOOL32
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2012/05/10  1.0         周嘉麟          创建
====================================================================*/
BOOL32 ReadCriIpMapTab(TCriIpMapTab &tCriIpMapTab)
{	
	s8   achFullName[64] = {0};
	s32  nEntryNum = 0;
	s8   chSeps[] = " \t"; //空格、者分隔符
	s8*  pchToken;
	s8** lpszTable;
	u8   byLoop = 0;
	
	sprintf(achFullName, "%s/%s", DIR_CONFIG, "ipmapcfg.ini");
    
	//获取总的条数
	BOOL32 bRet = GetRegKeyInt( achFullName, "CriIpMapTable", "EntryNum", 0, &nEntryNum );
	if( FALSE == bRet ) 
	{
		return bRet;
	}
	if( nEntryNum < 0 )
	{
		OspPrintf( TRUE, FALSE, "[ReadCriIpMapTab] Read CriIpMapTable failed because nEntryNum < 0 !\n");
		return FALSE;
	}
	
	if( nEntryNum > MAXNUM_DRI )
	{
		nEntryNum = MAXNUM_DRI;
	}
	
	//分配内存
	lpszTable = (s8 **)malloc( nEntryNum * sizeof( s8* ) );

	if (NULL == lpszTable)
	{
		printf("[ReadCriIpMapTab] malloc nEntryNum failed!\n");
		return  FALSE;
	}
	for( byLoop = 0; byLoop < (u32)nEntryNum; byLoop++ )
	{
		lpszTable[byLoop] = (s8 *)malloc(MAX_VALUE_LEN + 1);
	}
	
	//读取表
	bRet = GetRegKeyStringTable( achFullName, "CriIpMapTable", "fail", lpszTable, (u32*)&nEntryNum, (MAX_VALUE_LEN+1) );
	if( FALSE == bRet ) 
	{
		OspPrintf( TRUE, FALSE, "[ReadCriIpMapTab] Read CriIpMapTable failed!\n");
		printf( "[ReadCriIpMapTab] Read CriIpMapTable failed!\n");

		for( byLoop = 0; byLoop < nEntryNum; byLoop++ )
		{
			if( NULL != lpszTable[byLoop] )
			{
				free( lpszTable[byLoop] );
			}
		}
		free( lpszTable );
		return bRet;
	}
	
	for( byLoop = 0; byLoop < nEntryNum; byLoop++ )
	{
		pchToken = strtok( lpszTable[byLoop], chSeps );
		
		//读取IpV6		
		if( NULL == pchToken )
		{
			printf( "[ReadCriIpMapTab] CriIpMapTable Err while reading %s entryID.%d!\n", "IpV6", byLoop );
			OspPrintf( TRUE, FALSE, "[ReadCriIpMapTab] CriIpMapTable Err while reading %s entryID.%d!\n", "IpV6", byLoop );
			continue;
		}
		else
		{
			tCriIpMapTab.m_atIpMap[byLoop].SetV6IP(pchToken);
		}
		
		//读取IpV4
		pchToken = strtok( NULL, chSeps );
		if( NULL == pchToken )
		{
			printf( "[ReadCriIpMapTab] CriIpMapTable Err while reading %s entryID.%d!\n", "IpV4", byLoop );
			OspPrintf( TRUE, FALSE, "[ReadCriIpMapTab] CriIpMapTable Err while reading %s entryID.%d!\n", "IpV4", byLoop );
			continue;
		}
		else
		{	
			tCriIpMapTab.m_atIpMap[byLoop].SetV4IP(ntohl(INET_ADDR(pchToken)));
		}
	}
	
	
	//释放内存
	for( byLoop = 0; byLoop < nEntryNum; byLoop++ )
	{
		if( NULL != lpszTable[byLoop] )
		{
			free( lpszTable[byLoop] );
		}
	}
	free( lpszTable );
	
	return TRUE;
}

/*====================================================================
函数名      ：ReadGkIpMapTab
功能        ：读取GkIp映射配置
算法实现    ：
引用全局变量：
输入参数说明：TGKIpMapTab &tGkIpMapTab
返回值说明  ：BOOL32
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2012/05/10  1.0         周嘉麟          创建
====================================================================*/
BOOL32 ReadGkIpMapTab(TGKIpMapTab &tGkIpMapTab)
{	
	s8   achFullName[64] = {0};
	s32  nEntryNum = 0;
	s8   chSeps[] = " \t"; //空格、者分隔符
	s8*  pchToken;
	s8** lpszTable;
	u8   byLoop = 0;
	
	sprintf(achFullName, "%s/%s", DIR_CONFIG, "ipmapcfg.ini");
    
	//获取总的条数
	BOOL32 bRet = GetRegKeyInt( achFullName, "GKIpMapTable", "EntryNum", 0, &nEntryNum );
	if( FALSE == bRet ) 
	{
		return bRet;
	}
	if( nEntryNum < 0 )
	{
		OspPrintf( TRUE, FALSE, "[ReadGkIpMapTab]Read GKIpMapTable failed because nEntryNum < 0\n!");
		return FALSE;
	}
	
	if( nEntryNum > MAXNUM_CONFIG_GK )
	{
		nEntryNum = MAXNUM_CONFIG_GK;
	}
	
	//分配内存
	lpszTable = (s8 **)malloc( nEntryNum * sizeof( s8* ) );
	if (NULL == lpszTable)
	{
		printf("[ReadGkIpMapTab] malloc nEntryNum failed!\n");
		return FALSE;
	}
	for( byLoop = 0; byLoop < (u32)nEntryNum; byLoop++ )
	{
		lpszTable[byLoop] = (s8 *)malloc( MAX_VALUE_LEN+1 );
	}
	
	//读取表
	bRet = GetRegKeyStringTable( achFullName, "GKIpMapTable", "fail", lpszTable, (u32*)&nEntryNum, (MAX_VALUE_LEN+1) );
	if( FALSE == bRet ) 
	{
		printf("[ReadGkIpMapTab] Read GKIpMapTable failed!\n");
		OspPrintf(TRUE, FALSE, "[ReadGkIpMapTab] Read GKIpMapTable failed!\n");

		//释放内存
		for( byLoop = 0; byLoop < nEntryNum; byLoop++ )
		{
			if( NULL != lpszTable[byLoop] )
			{
				free( lpszTable[byLoop] );
			}
		}
		free( lpszTable );
		return bRet;
	}
	
	for( byLoop = 0; byLoop < nEntryNum; byLoop++ )
	{
		pchToken = strtok( lpszTable[byLoop], chSeps );
		
		//读取IpV6		
		if( NULL == pchToken )
		{
			OspPrintf(TRUE, FALSE, "[ReadGkIpMapTab] GKIpMapTable Err while reading %s entryID.%d!\n", "IpV6", byLoop );
			printf( "[ReadGkIpMapTab] GKIpMapTable Err while reading %s entryID.%d!\n", "IpV6", byLoop );
			continue;
		}
		else
		{
			tGkIpMapTab.m_atIpMap[byLoop].SetV6IP(pchToken);
		}
		
		//读取IpV4
		pchToken = strtok( NULL, chSeps );
		if( NULL == pchToken )
		{
			OspPrintf(TRUE, FALSE, "[ReadGkIpMapTab] GKIpMapTable Err while reading %s entryID.%d!\n", "IpV4", byLoop );
			printf("[ReadGkIpMapTab] GKIpMapTable Err while reading %s entryID.%d!\n", "IpV4", byLoop );
			continue;
		}
		else
		{	
			tGkIpMapTab.m_atIpMap[byLoop].SetV4IP(ntohl(INET_ADDR(pchToken)));
		}
	}
	
	
	//释放内存
	for( byLoop = 0; byLoop < nEntryNum; byLoop++ )
	{
		if( NULL != lpszTable[byLoop] )
		{
			free( lpszTable[byLoop] );
		}
	}
	free( lpszTable );
	
	return TRUE;
}


API void userinit()
{	
	printf( "[userinit] Now try to userinit! \n" );

    Err_t err = LogInit(CRI_KDVLOG_CONF_FILE);
    if(LOG_ERR_NO_ERR != err)
    {
        printf("KdvLog Init failed, err is %d\n", err);
	}

	BOOL bAttached = TRUE;
	u8   byBoardId = 1;
	u32  dwMcuIp = 0;
	u16  wMcuPort = MCU_LISTEN_PORT;
	u32  dwRegisterIp = 0;
	
	u32  dwMcuIpB = 0;
	u16  wMcuPortB = 0;
	bool bDoubleLink = FALSE; //是否支持双层建链

	BOOL bStartMtadp = FALSE;
	BOOL bStartMp = TRUE;
	BOOL bStartPrs = FALSE;
	BOOL bStartPxy = FALSE;

	// 设置进程异常捕获回调
#ifdef _LINUX_
    s32 nRet = StartCaptureException();
	if (0 != nRet)
	{
		printf("CRI: StartCaptureException Error(%d), Ignored.\n", nRet);
	}
    else
    {
        printf("CRI: StartCaptureException Succeeded\n");
    } 
	CopyCfgFile();
#endif

	if(!OspInit(TRUE, BRD_TELNET_PORT))
	{
		OspPrintf(1, 0, "Starting up OSP failed. Exit!\n");
		OspQuit();
		return ;
	}
    //Osp telnet 初始授权 [11/28/2006-zbq]
#ifndef WIN32
    OspTelAuthor( MCU_TEL_USRNAME, MCU_TEL_PWD );
#endif

#ifdef OSPEVENT
#undef OSPEVENT
#endif

#define OSPEVENT( x, y ) OspAddEventDesc( #x, y )

#ifdef _EV_MCUMT_H_
	#undef _EV_MCUMT_H_
	#include "evmcumt.h"
#else
	#include "evmcumt.h"
#endif
/*lint -save -esym(529, EV_MCUMT_SAT_END)*/	
#ifdef _EV_MCU_H_
	#undef _EV_MCU_H_
	#include "evmcu.h" 
#else
	#include "evmcu.h"
#endif
/*lint -restore*/
#ifdef _EV_MCUGK_H_
#undef _EV_MCUGK_H_
#include "evmcugk.h"
#else
#include "evmcugk.h"
#endif
	
    //need to activate kdvsyslib.a and DataSwitch.a on VxWorks
#ifdef _VXWORKS_
    Trim("Kdv Mcu 4.0");
#endif
   	
	//读取单板配置
	if(!ReadBoardConfig(byBoardId, bAttached, dwMcuIp, wMcuPort, dwRegisterIp))
	{	
		OspPrintf(1, 1, "Configuration error. Exit!\n");
		return;
	}
    
	// 读取终端IP映射表
	TMtIpMapTab tMtIpMapTab;
	if(!ReadMtIpMapTab(tMtIpMapTab))
	{	
		OspPrintf(TRUE, FALSE, "[userinit] ReadMtIpMapTab failed!\n");
	}

	// 读取接入板IP映射表
	
	TCriIpMapTab tCriIpMapTab;
	if (!ReadCriIpMapTab(tCriIpMapTab))
	{
		OspPrintf(TRUE, FALSE, "[userinit] ReadCriIpMapTab failed!\n");
	}

	// 读取接入板IP映射表
	TGKIpMapTab tGKIpMapTab;
	if (!ReadGkIpMapTab(tGKIpMapTab))
	{
		OspPrintf(TRUE, FALSE, "[userinit] ReadGkIpMapTab failed!\n");
	}

	//获取V6Ip
	s8  achV6Ip[64] = {0};
	s16 swScopeId = 0;
	s8  *pbyV6Ip = NULL;
	// add [pengguofeng 5/17/2012]
	u8 byEthId = 1; //from eth1
	int nRetIpv6 =  get_linklocal_ipv6addr(achV6Ip, swScopeId, "eth1");
	if ( nRetIpv6 != 1)
	{
		byEthId = 0;
		nRetIpv6 = get_linklocal_ipv6addr(achV6Ip, swScopeId, "eth0");
		if ( nRetIpv6 != 1)
		{
			printf("[userinit]without any available IpV6 address!\n");
		}
	}

	if (1 != nRetIpv6)
	{
		pbyV6Ip = NULL;
	}
	else
	{
		pbyV6Ip = achV6Ip;
	}
	
	printf("[userinit]get eth%d ipv6 addr:%s scopeId:%d\n", byEthId, pbyV6Ip, swScopeId);

	//是否存在两个mcu地址需要双层建链支持
	GetMpcBInfo(dwMcuIpB, wMcuPortB);
	if (0 != dwMcuIpB && dwMcuIp != dwMcuIpB)
	{
		bDoubleLink = TRUE;
	}
    
#ifndef _IS21APP_
	//读取模块配置
	ReadModuleConfig(bStartMtadp, bStartMp, bStartPrs, bStartPxy);
	
	u32 dwDstMcuNode  = BrdGetDstMcuNode();
	u32 dwDstMcuNodeB = BrdGetDstMcuNodeB();
	
	if(bStartMtadp) //如果启动Mtadp
	{
		TMtAdpConnectParam tConnectParam;
		TMtAdpConnectParam tConnectParamB;
		TMtAdp tMtadp;
	
		tMtadp.SetMtadpId( byBoardId );
		tMtadp.SetAttachMode( bAttached ? 1 : 255 );
		tMtadp.SetIpAddr( ntohl(dwRegisterIp) );
		tConnectParam.m_dwMcuTcpNode = dwDstMcuNode;
		tConnectParam.m_wMcuPort     = wMcuPort;
		tConnectParam.SetMcuIpAddr( ntohl(dwMcuIp) );	
        tConnectParam.m_bSupportHD = BrdIsHdi();
		
		if (bDoubleLink)
		{
			tConnectParamB.m_dwMcuTcpNode = dwDstMcuNodeB;
			tConnectParamB.m_wMcuPort     = wMcuPortB;
			tConnectParamB.SetMcuIpAddr( ntohl(dwMcuIpB) );
            tConnectParamB.m_bSupportHD = BrdIsHdi();
		}
		
//		if ( !MtAdpStart(&tConnectParam, &tMtadp, (bDoubleLink ? &tConnectParamB : NULL)) )
 		if ( !MtAdpStart(&tMtIpMapTab, &tConnectParam, &tMtadp,
			&tCriIpMapTab, &tGKIpMapTab, pbyV6Ip, swScopeId,
			(bDoubleLink ? &tConnectParamB : NULL)) )
		{

#if defined _LINUX_ && defined _LINUX12_
			//[chendaiwei 2010/11/18] 接入业务启动，Cri2板关灯
			BrdLedStatusSet(LED_FUN_ID1,BRD_LED_OFF);
			BrdLedStatusSet(LED_FUN_ID7,BRD_LED_OFF);
#endif
			OspPrintf(TRUE, TRUE, "Starting Mtadp failed.\n");
			OspQuit();
			return;
		}		
		else
		{
#if defined _LINUX_ && defined _LINUX12_
			//[chendaiwei 2010/11/18] 接入业务启动，Cri2板点灯
			//HDI板点灯
			if(BrdIsHdi())
			{
				BrdLedStatusSet(LED_FUN_ID7,BRD_LED_ON);				
			}
			else
			{
				BrdLedStatusSet(LED_FUN_ID1,BRD_LED_ON);
			}

			OspPrintf(TRUE, TRUE, "Starting Mtadp OK!\n");
#endif
		}
	}
	
	if(bStartMp) //如果启动Mp
	{		
		TMp tMp;
		tMp.SetIpAddr(/*ntohl*/(dwRegisterIp));

		tMp.SetMpId(byBoardId);
		tMp.SetAttachMode(bAttached ? 1 : 255);
		TStartMpParam tMpParamSlave;
		tMpParamSlave.dwMcuIp = dwMcuIpB;
		tMpParamSlave.wMcuPort = wMcuPortB;

		printf("[userinit] mpstart!\n");
		if(!mpstart(&tMtIpMapTab, pbyV6Ip, dwDstMcuNode, dwMcuIp, wMcuPort, &tMp, &tMpParamSlave))
		{
#if defined _LINUX_ && defined _LINUX12_
			//[chendaiwei 2010/11/18] 转发业务启动失败，Cri2板关灯
			BrdLedStatusSet(LED_FUN_ID2,BRD_LED_OFF);
#endif
			OspPrintf(1, 1, "Starting mp failed.\n");
			OspQuit();
			return;
		}
		else
		{
#if defined _LINUX_ && defined _LINUX12_
			//[chendaiwei 2010/11/18] 转发业务启动成功，Cri2板点灯
			BrdLedStatusSet(LED_FUN_ID2,BRD_LED_ON);
#endif
			OspPrintf(1, 1, "Starting mp OK!\n");
		}	
	}
	
	if(bStartPrs) //如果启动Prs
	{	
		TPrsCfg tPrsCfg;
		memset(&tPrsCfg, 0, sizeof(tPrsCfg));
		g_cBrdAgentApp.GetPrsCfg(&tPrsCfg);
		
		if( !prsinit( &tPrsCfg ) )
		{
#if defined _LINUX_ && defined _LINUX12_
			//[chendaiwei 2010/11/18] 丢包重传业务启动失败，Cri2板关灯
			BrdLedStatusSet(LED_FUN_ID3,BRD_LED_OFF);
#endif
			OspPrintf(1, 1, "Starting prs failed\n" );
		}
		else
		{
#if defined _LINUX_ && defined _LINUX12_
			//[chendaiwei 2010/11/18] 丢包重传业务启动成功，Cri2板点灯
			BrdLedStatusSet(LED_FUN_ID3,BRD_LED_ON);
#endif
			OspPrintf(1, 1, "Starting prs OK!\n" );
		}
	}		
	// [11/18/2010 xliang] pxy is not include at present.
	/*
	// [pengjie 2010/2/27] CRI2 Pxy 支持
#ifdef _LINUX12_
	printf("[userinit] IsStartPxy = %d \n", bStartPxy);
	if( bStartPxy )
	{
		// [pengjie 2010/6/12] cri2 pxy整合 先初始化代理服务器和GK封装的NIP接口
		InitPxyNipFun();
	    // [pengjie 2010/6/12] 再初始化pxy
		InitPxy(); // 初始化
	}
#endif
	*/
#if defined _LINUX_ && defined _LINUX12_
	//[chendaiwei 2010/11/29] CRI2业务成功启动，点亮M/S灯
    BrdLedStatusSet(LED_CRI2_MS,BRD_LED_ON);
#endif

#endif // _IS21APP_
}

#ifndef _IS21APP_
API void userquit()
{
	MtAdpStop();
	mpstop();
#if defined _LINUX_ && defined _LINUX12_
			//[chendaiwei 2010/11/18] 转发业务启动失败，Cri2板关灯
			BrdLedStatusSet(LED_FUN_ID2,BRD_LED_OFF);
			//[chendaiwei 2010/11/18] 丢包重传业务启动失败，Cri2板关灯
			BrdLedStatusSet(LED_FUN_ID3,BRD_LED_OFF);
			//[chendaiwei 2010/11/18] 接入业务启动，Cri2板关灯
			BrdLedStatusSet(LED_FUN_ID1,BRD_LED_OFF);
			BrdLedStatusSet(LED_FUN_ID7,BRD_LED_OFF);
			//[chendaiwei 2010/11/29] 停CRI2业务，熄灭M/S灯
			BrdLedStatusSet(LED_CRI2_MS,BRD_LED_OFF);
#endif
	OspQuit();
}
#endif // _IS21APP_

#ifndef _IS21APP_
API void version()
{
	//mtadpver();
	mpver();	
// 	dsver();
}
#endif // _IS21APP_

#ifdef _LINUX_
BOOL32 g_bQuit = FALSE;
void OspDemoQuitFunc( int nSignalNum )
{
	g_bQuit = TRUE;
}

int main( int argc, char** argv )
{
// [pengjie 2010/12/9] 对于IS2.1目前不跑业务，但升级时需要业务程序设置升级成功标志位
#ifdef _IS21APP_
 	printf("=============IS2.1 APP Start !==================\n");
// 	if( ERROR == BrdInit() )
// 	{
// 		printf("Call BrdInit() fail !\n");
// 		return 0;
// 	}
// 	
// 	u8 byRes = BrdSetSysRunSuccess();
// 	if( UPDATE_VERSION_SUCCESS == byRes )
// 	{
// 		printf("[BrdSetSysRunSuccess] succeed to update !\n");
// 	}
// 	else
// 	{
// 		printf( "[BrdSetSysRunSuccess] update filed Ret.%d !\n", byRes );
// 	}
// 	printf("=============IS2.1 APP quit normally !==========\n");
// 	return 0;
#endif
// End

    if (argc > 0)
    {
        g_szAppName = argv[0];
    }
    else
    {
        g_szAppName = APPNAME;
    }
    printf("[AppEntry]Starting: %s\n", g_szAppName);
    printf( "[AppEntry] userinit will begain! \n" );

	//zjj20120107 升级成功设置放到注册mcu成功之前做设置
#ifdef _LINUX12_
	s8 byRet = BrdSetSysRunSuccess();
	switch (byRet)
	{
	case NO_UPDATE_OPERATION:
		printf("[BrdSetSysRunSuccess]start cri normally\n");
		break;
	case UPDATE_VERSION_ROLLBACK:
		printf("[BrdSetSysRunSuccess]fail to update, rollback version\n");
		break;
	case UPDATE_VERSION_SUCCESS:
		printf("[BrdSetSysRunSuccess]succeed to update\n");
		break;		
	case SET_UPDATE_FLAG_FAILED:
		printf("[BrdSetSysRunSuccess]fail to set run flag\n");
		break;
	default:
		printf("[BrdSetSysRunSuccess]bad callback param\n");
		break;
	}
#endif //_LINUX12_

	userinit();

	printf( "[AppEntry] userinit over! \n" );

    //注册退出清除函数
	OspRegQuitFunc( OspDemoQuitFunc );
    g_bQuit = FALSE;

/*#ifdef _LINUX12_
	s8 byRet = BrdSetSysRunSuccess();
	switch (byRet)
	{
	case NO_UPDATE_OPERATION:
		printf("[BrdSetSysRunSuccess]start cri normally\n");
		break;
	case UPDATE_VERSION_ROLLBACK:
		printf("[BrdSetSysRunSuccess]fail to update, rollback version\n");
		break;
	case UPDATE_VERSION_SUCCESS:
		printf("[BrdSetSysRunSuccess]succeed to update\n");
		break;		
	case SET_UPDATE_FLAG_FAILED:
		printf("[BrdSetSysRunSuccess]fail to set run flag\n");
		break;
	default:
		printf("[BrdSetSysRunSuccess]bad callback param\n");
		break;
	}
#endif //_LINUX12_
*/

    while (!g_bQuit)
    {
        OspDelay(1000);
    }	

    printf("cri quit normally!\n");
    return 0;
}
#endif


#if defined( WIN32 )
void main(void)
{
	userinit();
	
	while (true)
    {
        Sleep(INFINITE);
    }
}
#endif


/////////////////////////////////////////////////////Linux 下异常捕获处理

#ifdef _LINUX_


#ifdef _X86_

/* parse cpu context information is architecture depent, 
   encapsulate as helper function can be easy */
int parse_x86_context(void* tmp)
{
	struct ucontext* ct = (struct ucontext*)tmp;	
	int* regs = (int*)ct->uc_mcontext.gregs;
	int  epc = regs[14];
	int *presp = (int *)regs[6];

	printf("\nexception instruction address: 0x%x\n", epc);
	
	printf("\n===================CPU registers==============================\n");

	printf("cpu register value:\n");
	printf("GS:0x%x\t FS:0x%x\t ES:0x%x\t DS:0x%x\n", 
	       regs[0], regs[1], regs[2], regs[3]);

	printf("CS:EIP:   0x%x:0x%x\nSS:UESP:  0x%x:0x%x\n", 
	       regs[15],regs[14], regs[18], regs[17]);

	printf("EDI:0x%x\t ESI:0x%x\t EBP:0x%x\n",regs[4], regs[5], regs[6]);
	printf("ESP:0x%x\t EBX:0x%x\t EDX:0x%x\n",regs[7], regs[8], regs[9]);
	printf("ECX:0x%x\t\t EAX:0x%x\n",regs[10], regs[11]);
	printf("TRAPNO:0x%x\t ERR:0x%x\t EFL:0x%x\n",regs[12], regs[13], regs[16]);

	printf("\n=================== call trace ==============================\n");


	/* ebp will record frame-pointer */
#if 0	
	*(stack-1) = epc;/* put the exception addr in the stack */
	for(i = -1;; i++) 
		if (printf_symbl_name(*(stack+i)) == -1)
			break;
#else
	printf_symbl_name(epc);/*fault addr*/
	while(1)
	{
		int ret = printf_symbl_name(*(presp + 1));
		if (ret == -1 || !ret)
		{
			if (!ret)
				printf("programs's user stack error !!\n");
			break;
		}
		presp = (int *)(*(presp));/* pre function's base sp */		
	}
	
#endif			
	free(sym);
	free(strtab);
	return 0;
}

#else   // Power PC 

void writeexclog(const s8 *pachBuf, s32 nBufLen)
{
	if (NULL == pachBuf || nBufLen <= 0)
    {
        printf("invalid param in writeexclog. pachBuf.%d nBufLen.%d\n",
                (int)pachBuf, nBufLen);
        return;
    }		
	
	FILE *hLogFile = fopen(DIR_EXCLOG, "r+b");
    if (NULL == hLogFile)
    {
        printf("exc.log not exist and create it\n");
        hLogFile = fopen(DIR_EXCLOG, "w+b");
        if (NULL == hLogFile)
        {
            printf("create exc.log failed for %s\n", strerror(errno));     
            return;
        }
    }

    fseek(hLogFile, 0, SEEK_END);
	s32 nLen = fwrite(pachBuf, nBufLen, 1, hLogFile);
	if (0 == nLen)
    {
        printf("write to exc.log failed\n");
        return;
    }
	
	fclose(hLogFile);
	return;
}

/* parse cpu context information is architecture depent, 
   encapsulate as helper function can be easy */
/*lint -save -e530*/
int parse_ppc_context(void* tmp)
{
	struct ucontext* ct = (struct ucontext*)tmp;
	int* regs = (int*)ct->uc_mcontext.regs;
	int  epc = regs[32];
	int* stack = (int*)regs[1];
	int  nLink = regs[36];/* link register */
	int* presp = (int *)(*stack);

	printf("\nexception instruction address: 0x%x\n", epc);

	printf("\n=================== call trace ==============================\n");

    // xsl [8/16/2006] write to log
    char achInfo[255];
    time_t tiCurTime = ::time(NULL);             
    int nLen = sprintf(achInfo, "\nsytem time %s\n", ctime(&tiCurTime));
    writeexclog(achInfo, nLen);
    nLen = sprintf(achInfo, "exception instruction address: 0x%x\n", epc);
    writeexclog(achInfo, nLen);
    nLen = sprintf(achInfo, "\n=========== call trace ============\n");
    writeexclog(achInfo, nLen);
	
	/* ebp will record frame-pointer */

	int func_size ;
	func_size = printf_symbl_name(epc);/*fault addr*/
	if (func_size > 0)/* link register and faule addr may be in the same function so do follows */
	{
		if ((epc - nLink < 0) || (epc - nLink >= func_size))
			printf_symbl_name(nLink);
	}
	
	while(1)
	{
		int ret = printf_symbl_name(*(presp + 1));
		if (ret == -1 || !ret)
		{
			if (!ret)
				printf("programs's user stack error !!\n");
			break;
		}
		presp = (int *)(*(presp));/* pre function's base sp */		
	}
	
	free(sym);
	free(strtab);
	return 0;
}
/*lint -restore*/
#endif


void parse_context(void* uc)
{
#ifdef _X86_
	parse_x86_context(uc);
#else   // Power PC
	parse_ppc_context(uc);
#endif
}


int printf_symbl_name(int pc)
{
	Elf32_Sym *syml = get_func_symb_by_addr(sym,symsize, pc);
	if (syml) {
		if ((s32)syml->st_name >= strsize)
		{
			printf("st_name if big then strtable size\r\n");
			return -1;
		}
		//return (strtab+syml->st_name);
		printf("pc:%x %s\r\n", pc, (strtab+syml->st_name));

        // xsl [8/16/2006] write to log
        char achInfo[255];
        int nLen = sprintf(achInfo, "pc:%x %s\r\n", pc, (strtab+syml->st_name));
        writeexclog(achInfo, nLen);
		
		if (!strcmp((strtab+syml->st_name), "main"))		
			return -1;
			
		return syml->st_value;	
	}	
	return 0;
}
void my_sigaction(int signo, siginfo_t* info, void* ct)
{
	if(signo == SIGSEGV) {
		printf("\r\ncatch segment fault! pid = %d\n", getpid());
	}
	if(info->si_code == SI_KERNEL)
		printf("signal is send by Kernel\n");
		
	printf("caused by:\t");

	if(info->si_code == SEGV_MAPERR)
		printf("access a fault address: %p\n", info->si_addr);

	else if(info->si_code == SEGV_ACCERR)
		printf("access a no permission address: %p\n", info->si_addr);
	else
		printf("unknow reason\n");
	if (ct)
		parse_context(ct);
	else
		printf("process context is NULL\n");	

	/* sigment fault is critical fault, we should exit right now */
	exit(1);
}

void install_sigaction()
{
	struct sigaction sigact;

	sigemptyset (&sigact.sa_mask);
	//memset(&sigact, 0, sizeof (struct sigaction));
	sigact.sa_flags = (s32)(SA_ONESHOT | SA_SIGINFO);
	sigact.sa_sigaction = my_sigaction;
	sigaction(SIGSEGV , &sigact, NULL);	
}



/* get the function'symbol that the "addr" is in the function or the same of the function */
Elf32_Sym *get_func_symb_by_addr(Elf32_Sym *symtab, int tabsize, int addr)
{	
	for (;tabsize > 0; tabsize -= sizeof(Elf32_Sym), symtab ++)
	{		
#if 1
		if (ELF32_ST_TYPE(symtab->st_info) != STT_FUNC || symtab->st_shndx == 0)/* we only find the function symbol and not a und  */	
			continue;
		if (!symtab->st_value)
			continue;
		if (addr >= (s32)symtab->st_value && addr < (s32)(symtab->st_size+symtab->st_value))
			return symtab;	
#else
		if (ELF32_ST_TYPE(symtab->st_info) == STT_FUNC)/* we only find the function symbol */	
			printf("name %s addr %x size %x\r\n", (strtab+symtab->st_name), symtab->st_value, symtab->st_size);
#endif			
	}
	return NULL;
}	


Elf32_Sym *get_symbol(Elf32_Sym *symtab, int tabsize, int addr)
{
	for (;tabsize > 0; tabsize -= sizeof(Elf32_Sym), symtab ++)
		if (symtab->st_value == addr)
			return symtab;

	return NULL;		
}
Elf32_Shdr * read_elf_section(FILE *file, Elf32_Ehdr *ehdr)
{
	Elf32_Shdr *pshdr;	
	int s_size = ehdr->e_shentsize * ehdr->e_shnum;
	
	pshdr = (Elf32_Shdr *)malloc(s_size);
	if (pshdr == NULL) 
	{
		printf("malloc shdr error\r\n");
		return NULL;
	}	

	if (fseek (file, ehdr->e_shoff, SEEK_SET))
	{
        free(pshdr);
		printf("fseek error\r\n");
		return NULL;
	}	
	
	if (fread(pshdr, s_size, 1, file) != 1)
	{
        free(pshdr);
		printf("read file error in func read_elf_section\r\n");
		return NULL;
	}

	return pshdr;
}


void *read_symtable(FILE *file, Elf32_Shdr *shdr, Elf32_Ehdr *ehdr, int *size)
{
	int sec_num;
	int tb_size;
	void *pSym;

	
	for (sec_num = 0; sec_num < ehdr->e_shnum; sec_num ++, shdr ++)
	{
		if (shdr->sh_type == SHT_SYMTAB)
			break;
	}	
	
	if (sec_num == ehdr->e_shnum)
	{
		printf("No symbol table\n");
		return NULL;
	}
	*size = tb_size = shdr->sh_size;
	
	pSym = (void *)malloc(tb_size);
	if (pSym ==NULL)
	{	
		printf("malloc error in func read_symtable\r\n");
		return NULL;
	}
	if (fseek (file, shdr->sh_offset, SEEK_SET))
	{
        free(pSym);
		printf("fseek error\r\n");
		return NULL;
	}	

	if (fread(pSym, tb_size, 1, file) != 1)
	{
        free(pSym);
		printf("read file error in func read_symtable\r\n");
		return NULL;
	}	
	return pSym;
}

void *read_strtable(FILE *file, Elf32_Shdr *shdr, Elf32_Ehdr *ehdr,  int *size)
{
	int sec_num;
	int tb_size;
	void *pSym;
	Elf32_Shdr *lshdr = shdr;
	
	for (sec_num = 0; sec_num < ehdr->e_shnum; sec_num ++, shdr ++)
	{
		if (shdr->sh_type == SHT_SYMTAB)
			break;
	}	

	shdr  = lshdr + SECTION_HEADER_INDEX(shdr->sh_link);
	
	*size = tb_size = shdr->sh_size;
	
	pSym = (void *)malloc(tb_size);
	if (pSym ==NULL)
	{	
		printf("malloc error in func read_strtable\r\n");
		return NULL;
	}
	if (fseek (file, shdr->sh_offset, SEEK_SET))
	{
        free(pSym);
		printf("fseek error\r\n");
		return NULL;
	}	

	if (fread(pSym, tb_size, 1, file) != 1)
	{
        free(pSym);
		printf("read file error in func read_strtable\r\n");
		return NULL;
	}	

	return pSym;
}

int  read_elf_head(FILE *file, Elf32_Ehdr *ehdr)
{
	if (fread(ehdr, sizeof(Elf32_Ehdr), 1, file) != 1)
		return -1;

	return 0;
}


/*=============================================================================
    函 数 名： StartCaptureException
    功    能： 设置进程异常捕获回调
    算法实现： 
    全局变量： 
    参    数： 
    返 回 值： s32 0-成功，非0-错误码
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2006/7/11   1.0			万春雷                  创建
=============================================================================*/
s32 StartCaptureException()
{
#ifndef _X86_
    return init_nipdebug(0, NULL, DIR_EXCLOG);
#else
	Elf32_Ehdr ehdr;
	Elf32_Shdr *shdr;
	FILE *file;

	install_sigaction();
	
	file = fopen(g_szAppName, "rb");
	if (file == NULL)
	{
		printf("CRI: Open file %s error: %s\n\n", g_szAppName, strerror(errno));
		return errno;
	}

	/* read the elf file's head */
	if (read_elf_head(file, &ehdr) == -1)
		return -1;
	
	/* read all sections of the elf file */
	shdr = read_elf_section(file, &ehdr);
	if (shdr == NULL)
		return -3;

	/* read static symbol table through symbol section*/
	sym = (Elf32_Sym *)read_symtable(file, shdr, &ehdr,  &symsize);
	if (sym == NULL)
		return -4;
	
	/* read string table through string section */
	strtab = (char *)read_strtable(file, shdr, &ehdr, &strsize);
	if (strtab == NULL)
		return -5;

	free(shdr);

    fclose(file);

	return 0;
#endif
}

#endif // _LINUX

/////////////////////////////////////////////////////异常捕获处理
