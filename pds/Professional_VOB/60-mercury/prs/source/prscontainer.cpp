/*****************************************************************************
模块名      : prs容器
文件名      : prscontainer.cpp
创建时间    : 2011年 01月 25日
实现功能    : 
作者        : 倪志俊
版本        : 
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2011/01/25  1.0         倪志俊        创建
******************************************************************************/
#include "osp.h"		//..\..\..\10-common\include\platform,
#include "kdvsys.h"		//..\..\..\10-common\include\protocol,
#include "mcuconst.h"	//..\..\..\10-common\include\mcu,
//#include "mcuver.h"		//..\..\common\include,
#include "prscontainer.h" //..\include
#include "eqpcfg.h"		//..\..\..\10-common\include\mcu,
#include "eqplib.h"		//..\..\..\10-common\include\mcu,
//#include "commonlib.h"  //..\..\common\include,
#include "watchdog.h"	//..\..\..\10-common\include\mcu
//#include "kdvlog.h"
//#include "loguserdef.h"



/////////////////////////////////////////////////////Linux 下异常捕获处理
#ifdef _LINUX_

//#include <stdio.h>
//#include <stdlib.h>
#include <elf.h>
//#include <sys/types.h>
//#include <unistd.h>
//#include <signal.h>
#include <ucontext.h>
//#include <errno.h>
//#include <string.h>
#include "nipdebug.h"

//Elf32_Sym *get_symtab(Elf32_Sym *symtab, int tabsize, int addr);
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

#ifdef _8KH_
const s8 g_szAppName[] = "/opt/mcu/prs_8000h";
#elif defined(_8KE_)
const s8 g_szAppName[] = "/opt/mcu/prs_8000e";
#elif defined(_8KI_)
const s8 g_szAppName[] = "/opt/mcu/prs_8000i";
#endif
// 开始捕捉入口函数
// Ret - 0 正常返回
s32 StartCaptureException();

#endif //_LINUX_



/*lint -save -e715*/
BOOL32 g_bQuit = FALSE;
FILE * g_pPrsLog = NULL;
int main(int argc, char** argv)
{

	Prs8keFileLog( PRS8KELOGFILENAME, "[Prs8ke] Prs8ke start!\n" );
	Prs8keStart();
	
	//注册退出清除函数
#ifdef _LINUX_
	OspRegQuitFunc( Prs8keQuitFunc );
#endif


	g_bQuit = FALSE;
	while (!g_bQuit)
	{
		OspDelay(1000);
	}

#ifdef _LINUX_
    OspDisconnectAllTcpNode();
#endif

	//OspQuit();

	if ( NULL != g_pPrsLog )
	{
		fclose( g_pPrsLog );
	}

	return 0;
}

/////////////////////////////////////////////////////Linux 下异常捕获处理

#ifdef _LINUX_

/*lint -save -e716*/
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


void parse_context(void* uc)
{
	parse_x86_context(uc);
}


int printf_symbl_name(int pc)
{
	Elf32_Sym *syml = get_func_symb_by_addr(sym,symsize, pc);
	if (syml) {
		if (int(syml->st_name) >= strsize)
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
	sigact.sa_flags = int(SA_ONESHOT | SA_SIGINFO);
	sigact.sa_sigaction = my_sigaction;
	sigaction(SIGSEGV , &sigact, NULL);	
}


/*lint -save -e737*/
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
		if (addr >= int(symtab->st_value) && addr < int(symtab->st_size+symtab->st_value))
			return symtab;	
#else
		if (ELF32_ST_TYPE(symtab->st_info) == STT_FUNC)/* we only find the function symbol */	
			printf("name %s addr %x size %x\r\n", (strtab+symtab->st_name), symtab->st_value, symtab->st_size);
#endif			
	}
	return NULL;
}	


// Elf32_Sym *get_symbol(Elf32_Sym *symtab, int tabsize, int addr)
// {
// 	for (;tabsize > 0; tabsize -= sizeof(Elf32_Sym), symtab ++)
// 		if (symtab->st_value == addr)
// 			return symtab;
// 
// 	return NULL;		
// }
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
		printf("fseek error\r\n");
		free(pshdr);
		return NULL;
	}	
	
	if (fread(pshdr, s_size, 1, file) != 1)
	{
		printf("read file error in func read_elf_section\r\n");
		free(pshdr);
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
		printf("fseek error\r\n");
		free(pSym);
		return NULL;
	}	

	if (fread(pSym, tb_size, 1, file) != 1)
	{
		printf("read file error in func read_symtable\r\n");
		free(pSym);
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
		printf("fseek error\r\n");
		free(pSym);
		return NULL;
	}	

	if (fread(pSym, tb_size, 1, file) != 1)
	{
		printf("read file error in func read_strtable\r\n");
		free(pSym);
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
函 数 名： sigterm_myaction
功    能： 
算法实现： 
全局变量： 
参    数： int sig
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2009/11/3   4.0			周广程                  创建
=============================================================================*/
void sigterm_myaction( int sig )
{
    if ( sig != SIGTERM )
    {
        return;
    }
	
    return;
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
            
    signal(SIGPIPE, SIG_IGN);
    signal(SIGTERM, sigterm_myaction);

	install_sigaction();
	
	file = fopen(g_szAppName, "rb");
	if (file == NULL)
	{
		printf("Prs8ke: Open file %s error: %s\n\n", g_szAppName, strerror(errno));
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

#endif // LINUX

void CreatePrs8keConnectCfg( void )
{
    s8 achProfileName[MAX_VALUE_LEN + 1];
    memset( achProfileName, 0, sizeof(achProfileName) );
    sprintf( achProfileName, "%s/%s", DIR_CONFIG, PRS8KE_CONNECT_INI);
    
    FILE *stream = fopen( achProfileName, "r" );
    if( stream != NULL )
    {
        fclose(stream);
        return;
    }
    
    BOOL32 bCreat = CreatFile( achProfileName );
    if ( !bCreat )
    {
        OspPrintf( TRUE, FALSE, "[CreatConnectCfg] Creat connect.ini failed!\n");
        return;
    }

    return;
}

void GetPrs8keConnectInfo(TPrs8keConnectInfo &tConnectInfo)
{
	CreatePrs8keConnectCfg();

	s8    achProfileName[32];
    BOOL32 bResult = FALSE;
    s8    achDefStr[] = "Cannot find the section or key";
    s8    achReturn[MAX_VALUE_LEN + 1] = {0};
    s32   nValue = 0;
	
	sprintf( achProfileName, "%s/%s", DIR_CONFIG, PRS8KE_CONNECT_INI);
	
    u32 tdwIPList[5];  //列举到的当前设置的所有有效ip
    u16 dwIPNum;       //列举到ip 数目
    
    u32 dwLocalIp = 0;
	bResult = GetRegKeyString(achProfileName, "McuInfo", "McuIpAddr", achDefStr, achReturn, MAX_VALUE_LEN+1);
	if ( bResult)
	{
		// 只对一体机有效，因为用的是同一IP [pengguofeng 8/21/2012]
		dwLocalIp = ntohl(INET_ADDR(achReturn));
		printf("[GetPrs8keConnectInfo]IP(%s)from %s will be used first\n", achReturn, achProfileName);
	}

	if ( 0 == dwLocalIp )
	{
		//通过commonlib的接口取eth1的IP
		//TMcuBrdEthParamAll Eth1Param;
		//GetAdapterIp(1, &Eth1Param);
		//dwLocalIp = ntohl(/*INET_ADDR("127.0.0.1")*/Eth1Param.atBrdEthParam[0].dwIpAdrs);
//#ifdef WIN32
		//if ( 0 == dwLocalIp  )
		//{
		//	dwLocalIp = ntohl( INET_ADDR("127.0.0.1") );
		//}
//#endif
		dwIPNum = OspAddrListGet(tdwIPList, 5);
		if( 0 == dwIPNum )
		{
			OspPrintf( TRUE, FALSE, "[GetConnectInfo] LocalIP is invalid.\n");
        
			dwLocalIp = ntohl(INET_ADDR("127.0.0.1"));
		}
		else
		{
			dwLocalIp = ntohl(tdwIPList[0]);
		}
	}
	
	LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_EQP, "[GetPrs8keConnectInfo]localIp:%x\n", dwLocalIp);
    tConnectInfo.SetLocalIp( dwLocalIp );
	
    bResult = GetRegKeyString( achProfileName, "McuInfo", "McuIpAddr", 
        achDefStr, achReturn, MAX_VALUE_LEN + 1 );
    if( !bResult)  
	{
		tConnectInfo.SetMcuIp( 0 );
	}
    else
    {
        tConnectInfo.SetMcuIp( ntohl( INET_ADDR( achReturn ) ) );
        if (tConnectInfo.GetMcuIp() == ((u32)-1))
        {
            tConnectInfo.SetMcuIp( 0 );
        }
    }
	
    if (tConnectInfo.GetMcuIp() == 0)
    {
        printf("[GetPrs8keConnectInfo] Wrong profile while reading %s!\n", "McuIpAddr" );
		
        tConnectInfo.SetMcuIp( dwLocalIp );
		
    }
    	
	bResult = GetRegKeyInt( achProfileName, "McuInfo", "McuPort", MCU_LISTEN_PORT, &nValue );
	if( !bResult || nValue == 0)  
	{
		printf("[GetPrs8keConnectInfo] Wrong profile while reading %s!\n", "McuPort" );
		tConnectInfo.SetMcuPort( MCU_LISTEN_PORT );
	}
	else
    {
        tConnectInfo.SetMcuPort( (u16)nValue );
    }
	
	return;

}

void Prs8keStart()
{
	// 设置进程异常捕获回调
#ifdef _LINUX_
    s32 nRet = StartCaptureException();
	if (0 != nRet)
	{
		printf("Prs8ke: StartCaptureException Error(%d), Ignored.\n", nRet);
	}
    else
    {
        printf("Prs8ke: StartCaptureException Succeeded\n");
    }    
#endif

#ifdef NDEBUG
    BOOL32 bRet = OspInit(FALSE, BRD_TELNET_PORT,"PRS");
	if ( !bRet )
	{
		printf("Prs8ke: OspInit failed!\n");
		Prs8keFileLog(PRS8KELOGFILENAME,"Prs8ke: OspInit failed!\n");
		return;
	}
    OspTelAuthor( MCU_TEL_USRNAME, MCU_TEL_PWD );
#else
    ::OspInit(TRUE, BRD_TELNET_PORT,"PRS");
#endif	
    if ( !OspSetPrompt("Prs") )
    {
		printf("Prs8ke: OspSetPromptfailed!\n");
		Prs8keFileLog(PRS8KELOGFILENAME,"Prs8ke: OspSetPrompt failed!\n");
		return;
    }
  
	logsetport( 5675,1 );
	u16 wLogRet = LogInit(PRS_KDVLOG_PATH);
	if( LOG_ERR_NO_ERR != wLogRet)
	{
		printf( "Prs8ke:  KdvLog Init failed,error:%d\n",wLogRet);
		Prs8keFileLog(PRS8KELOGFILENAME,"Prs8ke:  KdvLog Init failed,error:%d\n",wLogRet);
	}
	logenablemod(MID_PRSEQP_COMMON);

	TPrs8keConnectInfo tConnectInfo;
	GetPrs8keConnectInfo(tConnectInfo);

	TPrsCfg tPrsCfg;
	tPrsCfg.byEqpId      = PRSID_MIN;
	tPrsCfg.byEqpType    = EQP_TYPE_PRS;
	tPrsCfg.dwConnectIP  = htonl(tConnectInfo.GetMcuIp());
	tPrsCfg.wConnectPort = tConnectInfo.GetMcuPort();
	tPrsCfg.dwLocalIP    = htonl(tConnectInfo.GetLocalIp());
	tPrsCfg.wMcuId       = LOCAL_MCUID;
	tPrsCfg.wMAPCount    = 0;
	
	tPrsCfg.m_wFirstTimeSpan  = 0;
	tPrsCfg.m_wSecondTimeSpan = 0;
	tPrsCfg.m_wThirdTimeSpan  = 0;
	tPrsCfg.m_wRejectTimeSpan = 0;
	tPrsCfg.wRcvStartPort     = PRS_EQP_STARTPORT;
    //memset( tPrsCfg.achAlias, 0, sizeof(tPrsCfg.achAlias));
    //memcpy( tPrsCfg.achAlias, tPrsParam.GetAlias(), MAXLEN_EQP_ALIAS );
	
	if( prsinit( &tPrsCfg ) )
	{
		printf( "Prs8ke init success\n");
		LogPrint(LOG_LVL_KEYSTATUS,MID_PRSEQP_COMMON,"Prs8ke init success\n");
	}
	else
	{
		printf( "Prs8ke init failed\n");
		LogPrint(LOG_LVL_ERROR,MID_PRSEQP_COMMON,"Prs8ke init failed\n");
	}
	
	return;
}

#ifdef _LINUX_
void Prs8keQuitFunc( int nSignalNum )
{
	g_bQuit = TRUE;
	Prs8keFileLog(PRS8KELOGFILENAME,"[Prs8KE] Prs8KE quit normally!\n");
}
#endif
/*lint -save -e438 -e530*/
/*lint -save -esym(526, __builtin_va_start)*/
/*lint -save -esym(628, __builtin_va_start)*/
void Prs8keFileLog( const s8 * pchLogFile, s8 * pszFmt, ... )
{
	if (NULL == pchLogFile)
	{
		return;
	}

	va_list argPtr;
	s8 achPrintBuf[255] = {0};

	va_start(argPtr,pszFmt);
	
	vsprintf(achPrintBuf,pszFmt,argPtr);
	va_end(argPtr); 

	if ( NULL == g_pPrsLog )
	{
		g_pPrsLog = fopen( pchLogFile, "ab" );
		if ( NULL == g_pPrsLog )
		{
			return;
		}
	}
	
	FileLog( g_pPrsLog, achPrintBuf );
}
/*lint -restore*/