/*****************************************************************************
模块名      : h320终端适配模块
文件名      : MtAdp.cpp
相关文件    : MtAdp.h
文件实现功能: 全局入口模块
作者        : 许世林
版本        : V3.6  Copyright(C) 1997-2003 KDC, All rights reserved.
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2005/06/07  3.6         许世林      创建
******************************************************************************/

#include "osp.h"
#include "dri_agent.h"
#include "mtadpinst.h"
#include "evmcumt.h"
#include "mcuver.h"

/////////////////////////////////////////////////////Linux 下异常捕获处理
#ifdef _LINUX_

#include <stdio.h>
#include <stdlib.h>
#include <elf.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <ucontext.h>
#include <errno.h>
#include <string.h>
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

#define APPNAME     (LPCSTR)"h320mtadp"
LPCSTR g_szAppName;

// 开始捕捉入口函数
// Ret - 0 正常返回
s32 StartCaptureException();

#endif //_LINUX_

/////////////////////////////////////////////////////异常捕获处理


CMtAdpApp  g_cMtAdpApp;

//版本信息
#define VER_H320MTADP          (LPCSTR)"h320mtadp40.01.02.32.061027"

void APIEnableInLinux();

API void userinit()
{
	// 设置进程异常捕获回调
#ifdef _LINUX_
    s32 nRet = StartCaptureException();
	if (0 != nRet)
	{
		printf("MtAdp: StartCaptureException Error(%d), Ignored.\n", nRet);
	}
    else
    {
        printf("MtAdp: StartCaptureException Succeeded\n");
    }     
#endif

#ifdef WIN32
    OspInit(TRUE);
#else
    OspInit(TRUE, BRD_TELNET_PORT);  
#endif
    
    //Osp telnet 初始授权 [11/28/2006-zbq]
    OspTelAuthor( MCU_TEL_USRNAME, MCU_TEL_PWD );
    
    //for msg desc
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

    //初始化代理，读取mcu信息
    if( !InitDriAgent() )
    {
        printf("[Userinit] InitDriAgent failed!\n");
        OspQuit();
        return;
    }

    //获取dri板id
    g_cMtAdpApp.m_byDriId = g_cBrdAgentApp.GetBoardId();

    //获取本地IP地址
    g_cMtAdpApp.m_dwLocalIP = g_cBrdAgentApp.GetBrdIpAddr();
   
    OspPrintf(TRUE, FALSE, "[userinit] local Ip is %s\n", StrIPv4(g_cMtAdpApp.m_dwLocalIP));
        
    //别名
    sprintf(g_cMtAdpApp.m_achMtAdpAlias, "H320MtAdp%d", g_cMtAdpApp.m_byDriId);

    //创建应用
    g_cMtAdpApp.CreateApp("H320MtAdp", AID_MCU_MTADP, APPPRI_MTADP/*Pri*/, 2000/*Quene*/, 2<<20/*stacksize*/);

    u32 dwMcuAIp = g_cBrdAgentApp.GetMpcIpA();
    u32 dwMcuBIp = g_cBrdAgentApp.GetMpcIpB();
    if ( dwMcuAIp != 0 && dwMcuBIp != 0 &&
         dwMcuAIp != dwMcuBIp )
    {
        g_cMtAdpApp.m_bDoubleLink = TRUE;
    }
    else
    {
        g_cMtAdpApp.m_bDoubleLink = FALSE;
    }
    //power on
    OspPost( MAKEIID(AID_MCU_MTADP, CInstance::DAEMON), OSP_POWERON );

    return;
}

API void userquit()
{
#ifdef WIN32
    OspQuit();
#endif
}

API void Reboot()
{
    userquit();

#ifndef WIN32
    BrdHwReset();
#endif
}

#ifdef WIN32
void main( void )
{
    userinit();

    Sleep(INFINITE);
}
#endif

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
    printf("[AppEntry]Starting: %s\n", g_szAppName);
    
    userinit();

    //注册退出清除函数
	OspRegQuitFunc( OspDemoQuitFunc );
    g_bQuit = FALSE;

    APIEnableInLinux();
    while(!g_bQuit)
    {
        OspDelay(1000);
    }   	 

    printf("mtadp quit normally!\n");

    return 0;
}	
#endif

/*=============================================================================
  函 数 名： madebug
  功    能： 设置屏幕消息打印级别(0,1,2,3)
  算法实现： 
  全局变量： 
  参    数： u8 byLevel
  返 回 值： API void 
=============================================================================*/
API void madebug(u8 byLevel)
{
    g_cMtAdpApp.m_byLogLevel = byLevel;
}

/*=============================================================================
  函 数 名： maver
  功    能： 版本信息打印
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： API void 
=============================================================================*/
API void mtadpver()
{
	static s8 gs_VersionBuf[128] = {0};
	
    strcpy(gs_VersionBuf, KDV_MCU_PREFIX);
    
    s8 achMon[16] = {0};
    u32 byDay = 0;
    u32 byMonth = 0;
    u32 wYear = 0;
    static s8 achFullDate[24] = {0};
    
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
	
	OspPrintf( TRUE, FALSE, "H320Mtadp Version: %s\n", gs_VersionBuf  );
	OspPrintf(TRUE, FALSE, "H320MtAdp Module Version: %s. Compile Time: %s, %s\n",
		VER_H320MTADP, __TIME__, __DATE__);
    
    h320stackver();
}

/*=============================================================================
  函 数 名： mahelp
  功    能： 帮助信息打印
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： API void 
=============================================================================*/
API void mtadphelp()
{
    mtadpver();
    OspPrintf(TRUE, FALSE, "\nFollowing are all debug commands in h320MtAdp module:\n");
    OspPrintf(TRUE, FALSE, "mtadpver:            print version information.\n");
    OspPrintf(TRUE, FALSE, "madebug:             set mtadp log print level(Param: 0 - 3).\n");
    OspPrintf(TRUE, FALSE, "showswitch:          show switch infomation(Param: E1 channel id).\n");
    OspPrintf(TRUE, FALSE, "h320stackhelp:       h320 stack help.\n");
    OspPrintf(TRUE, FALSE, "matau(UsrName, Pwd): mtadp telnet author\n");
}

/*====================================================================
    函数名      : mcutau
    功能        ：mcu telnet 授权
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：无
====================================================================*/
API void matau( LPCSTR lpszUsrName = NULL, LPCSTR lpszPwd = NULL )
{
    OspTelAuthor( lpszUsrName, lpszPwd );
}

/*=============================================================================
  函 数 名： showswitch
  功    能： 打印码流交换信息
  算法实现： 
  全局变量： 
  参    数： u8 byChanId  [in] E1 通道号
  返 回 值： API void 
=============================================================================*/
API void showswitch(u8 byChanId)
{
    if(byChanId >= MAXNUM_CHAN)
    {
        OspPrintf(TRUE, FALSE, "Invalid E1 channel id. should < %d.\n", MAXNUM_CHAN);
        return;
    }

    OspPrintf(TRUE, FALSE, "E1 channel<%d> switch info:\n", byChanId);

    OspPost(MAKEIID(AID_MCU_MTADP, byChanId+1), EV_MTADP_SHOWSWITCH);
}

/*=============================================================================
  函 数 名： nobchvideoloopback
  功    能： for test
  算法实现： 
  全局变量： 
  参    数： u8 byChan
             u8 byLoop
  返 回 值： API void  
=============================================================================*/
API void nobchvideoloopback(u8 byChan, u8 byLoop)
{
    if(byChan >= MAXNUM_CHAN)
    {
        OspPrintf(TRUE, FALSE, "Invalid channel id. should < %d.\n", MAXNUM_CHAN);
        return;
    }

    OspPost(MAKEIID(AID_MCU_MTADP, byChan+1), EV_MTADP_VIDLOOPBACK, &byLoop, 1);
}

/*=============================================================================
  函 数 名： bchvideoloopback
  功    能： for test
  算法实现： 
  全局变量： 
  参    数： u8 byChan
             u8 byLoop
  返 回 值： API void   
=============================================================================*/
API void bchvideoloopback(u8 byChan, u8 byLoop)
{
    if(byChan >= MAXNUM_CHAN)
    {
        OspPrintf(TRUE, FALSE, "Invalid channel id. should < %d.\n", MAXNUM_CHAN);
        return;
    }
    
    OspPost(MAKEIID(AID_MCU_MTADP, byChan+1), EV_MTADP_BCHVIDLOOPBACK, &byLoop, 1);
}

void APIEnableInLinux()
{
#ifdef _LINUX_
    OspRegCommand("mtadphelp",  (void*)mtadphelp,   "mtadp help command");
    OspRegCommand("mtadpver",   (void*)mtadpver,    "mtadp version command");
    OspRegCommand("madebug",    (void*)madebug,     "mtadp debug command");
    OspRegCommand("showswitch", (void*)showswitch,  "mtadp switch info command");
    OspRegCommand("matau",      (void*)matau,       "mtadp telnet author command");
#endif
}



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
	int  link = regs[36];/* link register */
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
		if ((epc - link < 0) || (epc - link >= func_size))
			printf_symbl_name(link);
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
		if (syml->st_name >= strsize)
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
	sigact.sa_flags = SA_ONESHOT | SA_SIGINFO;
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
		if (addr >= symtab->st_value && addr < (symtab->st_size+symtab->st_value))
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
		printf("fseek error\r\n");
		return NULL;
	}	
	
	if (fread(pshdr, s_size, 1, file) != 1)
	{
		printf("read file error in func %s\r\n", __func__);
		return NULL;
	}

	return pshdr;
}


void *read_symtable(FILE *file, Elf32_Shdr *shdr, Elf32_Ehdr *ehdr, int *size)
{
	int sec_num;
	int tb_size;
	void *sym;

	
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
	
	sym = (void *)malloc(tb_size);
	if (sym ==NULL)
	{	
		printf("malloc error in func %s\r\n", __func__);
		return NULL;
	}
	if (fseek (file, shdr->sh_offset, SEEK_SET))
	{
		printf("fseek error\r\n");
		return NULL;
	}	

	if (fread(sym, tb_size, 1, file) != 1)
	{
		printf("read file error in func %s\r\n", __func__);
		return NULL;
	}	
	return sym;
}

void *read_strtable(FILE *file, Elf32_Shdr *shdr, Elf32_Ehdr *ehdr,  int *size)
{
	int sec_num;
	int tb_size;
	void *sym;
	Elf32_Shdr *lshdr = shdr;
	
	for (sec_num = 0; sec_num < ehdr->e_shnum; sec_num ++, shdr ++)
	{
		if (shdr->sh_type == SHT_SYMTAB)
			break;
	}	

	shdr  = lshdr + SECTION_HEADER_INDEX(shdr->sh_link);
	
	*size = tb_size = shdr->sh_size;
	
	sym = (void *)malloc(tb_size);
	if (sym ==NULL)
	{	
		printf("malloc error in func %s\r\n", __func__);
		return NULL;
	}
	if (fseek (file, shdr->sh_offset, SEEK_SET))
	{
		printf("fseek error\r\n");
		return NULL;
	}	

	if (fread(sym, tb_size, 1, file) != 1)
	{
		printf("read file error in func %s\r\n", __func__);
		return NULL;
	}	

	return sym;
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
		printf("Mtadp: Open file %s error: %s\n\n", g_szAppName, strerror(errno));
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
