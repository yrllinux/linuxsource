#include "mcustruct.h"
#include "mcuver.h"
#include "bindmp.h"
#include "dataswitch.h"
#include "eqplib.h"
#include "boardagent.h"
#include "bindmtadp.h"


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

#define APPNAME     (LPCSTR)"/usr/bin/mp8000b"
LPCSTR g_szAppName;

// 开始捕捉入口函数
// Ret - 0 正常返回
s32 StartCaptureException();

#endif //_LINUX_

/////////////////////////////////////////////////////异常捕获处理结束


#include "dscagent.h"

// 从 DSC 代理获取连接的MCU信息，以及需要启动的模块
BOOL ReadBoardConfig(u8& byBoardId, 
                     u32* pdwMcuIp, 
                     u16* pwMcuPort, 
                     u32& dwRegisterIp,
                     TDSCModuleInfo &tOtherModuleInfo,
                     BOOL& bStartPrs)
{
	BOOL  bRet = InitDscAgent();

	if(FALSE == bRet)
	{
		OspPrintf(TRUE, FALSE, "Starting dsc agent failed. Exit!\n");	
		return FALSE;
	}
	
	// byBoardId    = g_cBrdAgentApp.GetBoardId();
	*pdwMcuIp  = g_cBrdAgentApp.GetConnectMcuIpAddr();
	*pwMcuPort = g_cBrdAgentApp.GetConnectMcuPort();
	dwRegisterIp = g_cBrdAgentApp.GetBrdIpAddr();

    tOtherModuleInfo = g_cBrdAgentApp.GetRunningModuleInfo();
    bStartPrs = g_cBrdAgentApp.IsRunPrs();
	
	return TRUE;
}

// 总入口
API int userinit()
{	
    // 设置进程异常捕获回调
#ifdef _LINUX_
    s32 nRet = StartCaptureException();
    if (0 != nRet)
    {
        printf("Mp8000b: StartCaptureException Error(%d), Ignored.\n", nRet);
    }
    else
    {
        printf("Mp8000b: StartCaptureException Succeeded\n");
    }       
#endif

	u32  dwMcuIp     = 0;
	u16  wMcuPort    = MCU_LISTEN_PORT;
	
	u8   byBoardId = 1;     // 8000b onle be 1
	u32  dwRegisterIp = 0;
	
	bool bDoubleLink = FALSE; // 是否支持双层建链
	
    printf("compile time : %s  %s \n", __DATE__, __TIME__);
	    
    if(!OspInit(TRUE, BRD_TELNET_PORT))
	{
		printf("Start up OSP failed. Exit!\n");
		return 0;
	}

    OspTelAuthor( MCU_TEL_USRNAME, MCU_TEL_PWD );
	
    /* activate kdvsyslib.a and DataSwitch.a on VxWorks */    
#ifdef _VXWORKS_    
 	Trim("Kdv Mcu 4.0");
#endif	

    // 读取单板配置
    TDSCModuleInfo tInfo;
    BOOL bStartPrs = FALSE;

    BOOL32 bRet = ReadBoardConfig(byBoardId, &dwMcuIp, &wMcuPort, dwRegisterIp, tInfo, bStartPrs);
    if (!bRet)
    {	
        printf("Configuration error. Exit!\n");
        OspQuit();
        return 0;
	}
	
	//printf("MP ip: %0x, id: %d\n", ntohl(dwRegisterIp), byBoardId);
	printf("MP ip: %.32x, id: %d\n", ntohl(dwRegisterIp), byBoardId);
	
    // 如果不启动Proxy下启动Mp
	if( !tInfo.IsStartProxy() && tInfo.IsStartMp()) 
	{
        TMp tMp;
        tMp.SetIpAddr( ntohl(dwRegisterIp) );
        tMp.SetMpId(byBoardId);
        tMp.SetAttachMode(2);
	    tMp.SetMcuId(LOCAL_MCUID);

		if ( !mpstart(dwMcuIp, wMcuPort, &tMp, NULL) )
		{
			printf("Starting mp failed.\n");
			OspQuit();
			return 0;
		}
		else
		{
			printf("Starting mp OK!\n");
		}
	}
	else
	{
		printf("The Dsc is not to run mp!\n");
	}

	u32 dwDstMcuNode  = BrdGetDstMcuNode();
	// u32 dwDstMcuNodeB = BrdGetDstMcuNodeB();
	bool bAttached = FALSE;
	
	if (tInfo.IsStartMtAdp()) //如果启动Mtadp
	{
		TMtAdpConnectParam tConnectParam;
		TMtAdpConnectParam tConnectParamB;
		TMtAdp tMtadp;
	
		tMtadp.SetMtadpId( byBoardId );
		tMtadp.SetAttachMode( bAttached ? 1 : 255 );
		tMtadp.SetIpAddr( ntohl(dwRegisterIp) );
        // tMtadp.SetIpAddr( tEther.dwIpAdrs ); // 网路序？
		tConnectParam.m_dwMcuTcpNode = dwDstMcuNode;
		tConnectParam.m_wMcuPort     = wMcuPort;
		tConnectParam.SetMcuIpAddr( ntohl(dwMcuIp) );	
		if (bDoubleLink)
		{
			tConnectParamB.m_dwMcuTcpNode = INVALID_NODE;
			tConnectParamB.m_wMcuPort     = 0;
			tConnectParamB.SetMcuIpAddr( 0 );
		}
        
		if ( !MtAdpStart(&tConnectParam, &tMtadp, NULL ) )
		{
			printf("Starting Mtadp failed.\n");
			OspQuit();
			return 0;
		}		
		else
		{
			printf("Starting Mtadp OK!\n");
		}
	}
	else
	{
		printf("The Dsc is not to run mtadp!\n");
	}	
    
  if (bStartPrs)
	{	
		TPrsCfg tPrsCfg;
		g_cBrdAgentApp.GetPrsCfg(&tPrsCfg);
		
		if (!prsinit(&tPrsCfg))
		{
			printf("Starting prs failed\n" );
		}
		else
		{
			printf("Starting prs OK!\n" );
		}
	}
	else
	{
		printf("The Dsc is not to run prs!\n");
	}

	return 1;
}


API void userquit()
{
    BOOL bStartMtAdp = TRUE;
    BOOL bStartPrs = TRUE;
    
#ifndef   _LINUX_  
	BOOL bStartMp = TRUE;
    if ( bStartMp )
    {
        mpstop();
    }
#endif
 
    if ( bStartMtAdp )
    {
        MtAdpStop();
    }
    if ( bStartPrs )
    {
        // PRS QUIT
    }
	OspQuit();
}

API void version()
{
	dsver();
#ifndef _LINUX_	
	mpver();	
#endif	
}


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

    
	int nRet = userinit();
	
	if(0 == nRet)
	{
		return 0;
	}

    //注册退出清除函数
	OspRegQuitFunc( OspDemoQuitFunc );
    g_bQuit = FALSE;
			
	while(!g_bQuit)
	{
	    OspDelay(1000);
	}   	 

    printf("mp8000b quit normally!\n");
	return 0;
}	
#endif

#ifdef WIN32
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

//lint -save -e1013 -e530
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
//lint -restore
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
		printf("Mp8000b: Open file %s error: %s\n\n", g_szAppName, strerror(errno));
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
