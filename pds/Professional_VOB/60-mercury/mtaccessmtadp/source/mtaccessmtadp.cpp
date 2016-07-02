#include "osp.h"
#include "mcustruct.h"
#include "mcuver.h"
#include "bindmtadp.h"
#include "bindmp.h"
#include "dataswitch.h"
//#include "kdvsys.h"
#include "eqplib.h"
#include "boardagent.h"
//#include <stdio.h>

// [pengjie 2010/2/27] CRI2 pxy 支持
#include "firewallproxyserverlib.h" 
// [pengjie 2010/6/12] CRI2 pxy库整合
#include "protocolnipinterface.h"

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
#include "cri_agent.h"

BOOL ReadBoardConfig(u8 &byBoardId, BOOL &bAttached, u32 &dwMcuIp, u16 &wMcuPort, u32 &dwRegisterIp, u32 &dwMcuIpB,u16 &wMcuPortB,u8 &byBrdEthChoice )
{
	printf( "[ReadBoardConfig] mtadp Now come to ReadBoardConfig! \n" );

#ifndef _VXWORKS_
	printf( "[BrdInit]\n" );
	if( ERROR == BrdInit() )
	{
		printf("[InitAgent] call BrdInit fail.\n");
		//return FALSE;
	}
#endif	

	s8    achProfileName[32] = {0};
    BOOL  bResult;
    s8    achDefStr[] = "Cannot find the section or key";
    s8    achReturn[MAX_VALUE_LEN + 1];
    s32   sdwValue;
    s32   dwFlag;

	sprintf( achProfileName, "%s/%s", DIR_CONFIG, FILE_BRDCFG_INI);

	// MpcIp
    bResult = GetRegKeyString( achProfileName, SECTION_BoardSystem, KEY_MpcIpAddr, 
						achDefStr, achReturn, MAX_VALUE_LEN + 1 );
    if( bResult == FALSE )  
	{
		printf( "[BoardAgent] Wrong profile while reading %s!\n", KEY_MpcIpAddr );		
		return FALSE;
	}
	dwMcuIp =  ntohl( INET_ADDR( achReturn ) );
	if( 0xffffffff == dwMcuIp )
	{
		dwMcuIp = 0;
	}
    if( 0 == dwMcuIp )
    {
        printf( "[BoardAgent] The A Mpc's Ip is 0.\n");
        return FALSE;
    }
    
	//MPC Ip 2 // [5/26/2010 xliang] 
	bResult = GetRegKeyString( achProfileName, SECTION_BoardSystem, KEY_MpcIpAddrB, 
		achDefStr, achReturn, MAX_VALUE_LEN + 1 );
    if( bResult == FALSE )  
	{
		printf( "[BoardAgent] Wrong profile while reading %s!\n", KEY_MpcIpAddrB );		
		//return FALSE;
	}
	dwMcuIpB =  ntohl( INET_ADDR( achReturn ) );
	if( 0xffffffff == dwMcuIpB )
	{
		dwMcuIpB = 0;
	}
    if( 0 == dwMcuIpB )
    {
        printf( "[BoardAgent] The B Mpc's Ip is 0.\n");
        //return FALSE;
    }

	// MpcPort
	bResult = GetRegKeyInt( achProfileName, SECTION_BoardSystem, KEY_MpcPort, MCU_LISTEN_PORT, &sdwValue );
	if( bResult == FALSE )  
	{
		printf( "[BoardAgent] Wrong profile while reading %s!\n", KEY_MpcPort );
		sdwValue = MCU_LISTEN_PORT;
	}
	wMcuPort = ( u16 )sdwValue;

	// MpcPort2
	bResult = GetRegKeyInt( achProfileName, SECTION_BoardSystem, KEY_MpcPortB, MCU_LISTEN_PORT, &sdwValue );
	if( bResult == FALSE )  
	{
		printf( "[BoardAgent] Wrong profile while reading %s!\n", KEY_MpcPortB );
		sdwValue = MCU_LISTEN_PORT;
	}
	wMcuPortB = ( u16 )sdwValue;



	bResult = GetRegKeyInt(achProfileName, SECTION_IsFront, KEY_Flag, 1, &dwFlag);
    if(FALSE == bResult)
    {
        printf( "[BoardAgent] Wrong profile while reading %s!\n", "IsFront" );
        dwFlag = 1;
    }
    byBrdEthChoice = (u8)dwFlag;   // 0: front , 1: back


	TBrdEthParam tBrdEthParam;       
	memset(&tBrdEthParam,0,sizeof(tBrdEthParam));
    STATUS nRet = BrdGetEthParam(byBrdEthChoice, &tBrdEthParam);// 取单板Ip
	if ( ERROR == nRet )
	{
		printf( "555[BoardAgent][GetBoardInfo] BrdGetEthParam error!\n");
		//return FALSE;
		nRet =BrdGetEthParam( byBrdEthChoice == 1 ? 0 : 1,&tBrdEthParam );
		if ( ERROR == nRet )
		{
			printf( "666[BoardAgent][GetBoardInfo] BrdGetEthParam error!\n");
		}
	}
	u32 dwBrdIpAddr = ntohl(tBrdEthParam.dwIpAdrs);


	TBrdPosition tBrdPosition; 
    nRet = BrdQueryPosition( &tBrdPosition );

	byBoardId = (tBrdPosition.byBrdLayer<<4) | (tBrdPosition.byBrdSlot+1);
		

	s8 achIP[20];
	sprintf(achIP, "%d.%d.%d.%d", dwBrdIpAddr >> 24 & 0xff,
		dwBrdIpAddr >> 16 & 0xff,
		dwBrdIpAddr >> 8 & 0xff,
		dwBrdIpAddr & 0xff);

	printf("\n[ReadModuleConfig]  ip.%s ethchoise.%d %d.%d byBoardId.%d\n", achIP,byBrdEthChoice,
		tBrdPosition.byBrdLayer,tBrdPosition.byBrdSlot,
		byBoardId );
	

	
	bAttached = FALSE;
	dwRegisterIp = dwBrdIpAddr;
	
	return TRUE;
}



BOOL ReadModuleConfig(BOOL &bStartMtadp)
{	
	printf("[ReadModuleConfig] come in! \n");
    int nTemp;	   

    s8 pFileName[KDV_MAX_PATH] = {0};
    sprintf(pFileName, "%s/modcfg.ini", DIR_CONFIG);
	

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
#endif

	if(!OspInit(TRUE, BRD_TELNET_PORT+1))
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
	u8 byBrdEthChoice = 0;
	if(!ReadBoardConfig(byBoardId, bAttached, dwMcuIp, wMcuPort, dwRegisterIp, dwMcuIpB, wMcuPortB, byBrdEthChoice))
	{	
		OspPrintf(1, 1, "Configuration error. Exit!\n");
		return;
	}
    
	//是否存在两个mcu地址需要双层建链支持	
	if (0 != dwMcuIpB && dwMcuIp != dwMcuIpB)
	{
		bDoubleLink = TRUE;
	}
    

	//读取模块配置
	ReadModuleConfig( bStartMtadp );
	
	u32 dwDstMcuNode  = 0;//BrdGetDstMcuNode();
	u32 dwDstMcuNodeB = 0;//BrdGetDstMcuNodeB();
	printf("Starting Mtadp.%d.\n",bStartMtadp);
	
	if(bStartMtadp) //如果启动Mtadp
	{

		TMtAdpConnectParam tConnectParam;
		TMtAdpConnectParam tConnectParamB;
		TMtAdp tMtadp;
	
		tMtadp.SetMtadpId( byBoardId );
		tMtadp.SetAttachMode( bAttached ? 1 : 255 );
		tMtadp.SetIpAddr(dwRegisterIp);
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
		
		//起接入功能的单板需要上报MAC地址[5/16/2012 chendaiwei]
		TBrdEthParam tEthPara;
		memset(&tEthPara,0,sizeof(tEthPara));
		STATUS nRet =BrdGetEthParam( byBrdEthChoice,&tEthPara );
		if ( ERROR == nRet )
		{
			printf( "11111[BoardAgent][GetBoardInfo] BrdGetEthParam error!\n");
			
			nRet =BrdGetEthParam( byBrdEthChoice == 1 ? 0 : 1,&tEthPara );
			if ( ERROR == nRet )
			{
				printf( "2222[BoardAgent][GetBoardInfo] BrdGetEthParam error!\n");
			}

			//return FALSE;
		}

		if ( !MtAdpStart(&tConnectParam, &tMtadp, (bDoubleLink ? &tConnectParamB : NULL),(u8*)tEthPara.byMacAdrs) )
		{

#if defined _LINUX_ && defined _LINUX12_
			//[chendaiwei 2010/11/18] 接入业务启动，Cri2板关灯
			BrdLedStatusSet(LED_FUN_ID1,BRD_LED_OFF);
			BrdLedStatusSet(LED_FUN_ID7,BRD_LED_OFF);
#endif
			printf("Starting Mtadp failed.\n");
			OspQuit();
			return;
		}		
		else
		{
			OspSetPrompt("Mtadp");
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

			printf("mtadp Starting Mtadp OK!\n");
#endif
		}
	}
}

#ifndef _IS21APP_
API void userquit()
{
	MtAdpStop();	
#if defined _LINUX_ && defined _LINUX12_
	//[chendaiwei 2010/11/18] 接入业务启动，Cri2板关灯
	BrdLedStatusSet(LED_FUN_ID1,BRD_LED_OFF);
	BrdLedStatusSet(LED_FUN_ID7,BRD_LED_OFF);
#endif
	OspQuit();
}
#endif // _IS21APP_

#ifndef _IS21APP_
API void version()
{
	mtadpver();
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

    if (argc > 0)
    {
        g_szAppName = argv[0];
    }
    else
    {
        g_szAppName = APPNAME;
    }
    printf("[AppEntry]mtadp Starting: %s\n", g_szAppName);
    printf( "[AppEntry] mtadp userinit will begain! \n" );

	userinit();

	printf( "[AppEntry] mtadp userinit over! \n" );

    //注册退出清除函数
	OspRegQuitFunc( OspDemoQuitFunc );
    g_bQuit = FALSE;

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
}


#endif // _LINUX

/////////////////////////////////////////////////////异常捕获处理
