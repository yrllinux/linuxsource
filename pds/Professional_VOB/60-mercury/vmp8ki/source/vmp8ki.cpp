/*****************************************************************************
模块名      : vmp8ki
文件名      : vmp8ki.cpp
创建时间    : 2012年 05月 02日
实现功能    : 
作者        : 周翼亮
版本        : 
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2012/05/02  1.0         周翼亮        创建
******************************************************************************/
#include "osp.h"
#include "kdvsys.h"
#include "mcuconst.h"
#include "mcuver.h"
#include "vmp8ki.h"
#include "eqpcfg.h"
#include "eqplib.h"
#include "commonlib.h"
#include "watchdog.h"

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

LPCSTR g_szAppName = "/usr/bin/vmp8ki";

// 开始捕捉入口函数
// Ret - 0 正常返回
s32 StartCaptureException();

#endif //_LINUX_




/*=============================================================================
函 数 名： APIEnableInLinux
功    能： 注册linux调试接口
算法实现： 
全局变量： 
参    数： void
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2012/5/2  4.7			周翼亮                  创建
=============================================================================*/
/*lint -save -e611*/
void VmpAPIEnableInLinux( void )
{
#ifdef _LINUX_     
    OspRegCommand("vmpstart",          (void*)vmpstart,           "vmpstart");
	
#endif
}
/*lint -restore*/

/////////////////////////////////////////////////////异常捕获处理



//启动vmp8ki业务
API void vmpstart()
{

	TMpu2Cfg tMpu2Vmpcfg;
	BOOL32 bAutoTest = FALSE;
	tMpu2Vmpcfg.SetMpu2EqpNum( 1 );//只起一个vmp
#ifdef _LINUX_
	bAutoTest = BrdGetE2promTestFlag();
#endif
	tMpu2Vmpcfg.SetIsAutoTest( bAutoTest );
	TEqpBasicCfg tEqpBasicCfg;
	char szAlias[]="VMP";
	memcpy(tEqpBasicCfg.achAlias,szAlias,sizeof(szAlias));
	tEqpBasicCfg.byEqpType=EQP_TYPE_VMP;
	//localIP,用mmp的实现方式
	u32 tdwIPList[5];  //列举到的当前设置的所有有效ip
	u16 dwIPNum;       //列举到ip 数目
   
	u32 dwLocalIp = 0;
	dwIPNum = OspAddrListGet(tdwIPList, 5);
	if( 0 == dwIPNum )
	{
		OspPrintf( TRUE, FALSE, "[vmpstart] LocalIP is invalid.\n");
		LogPrint(LOG_LVL_ERROR,MID_PUB_ALWAYS,"[VMP8KI:vmpstart]LocalIP is invalid.\n");
		   
		 dwLocalIp = ntohl(INET_ADDR("127.0.0.1"));
	}
	else
	{
		dwLocalIp = ntohl(tdwIPList[0]);
    }
	tEqpBasicCfg.dwLocalIP = dwLocalIp;
	tEqpBasicCfg.dwConnectIP = dwLocalIp;
	tEqpBasicCfg.wConnectPort = MCU_LISTEN_PORT;
	tEqpBasicCfg.wMcuId = LOCAL_MCUID;
	
	tMpu2Vmpcfg.SetMpu2EqpCfg(0,tEqpBasicCfg);
	u8 byWorkMode = TYPE_MPU2VMP_BASIC;
	if( bAutoTest )
	{
		//分为插子卡和不插子卡两种
		BOOL32 bHasEcard = FALSE;
#ifdef _LINUX_
		if ( BrdMpu2HasEcard() == 1)
		{
			bHasEcard = TRUE;
		}
		
#endif
		if ( bHasEcard ) //插子卡
		{
			byWorkMode = TYPE_MPU2VMP_ENHANCED;		//插子卡，则起ENHANCE模式
		}
		else
		{
			byWorkMode = TYPE_MPU2VMP_BASIC;		//不插子卡，测BASIC模式
		}
	}
	tMpu2Vmpcfg.SetMpu2WorkMode( byWorkMode );
	Mpu2Init(&tMpu2Vmpcfg);

#ifdef _LINUX_
	if ( NotfiyGuardUpdateSuccess("vmp_8000i") )
	{
		printf("Vmp8ki: NotfiyGuardUpdateSuccess success\n");
		LogPrint(  LOG_LVL_KEYSTATUS,MID_PUB_ALWAYS,"Vmp8ki: NotfiyGuardUpdateSuccess success\n");
	}
	else
	{
		printf("VMP8ki: NotfiyGuardUpdateSuccess failed\n");
		LogPrint(  LOG_LVL_ERROR,MID_PUB_ALWAYS,"Vmp8ki: NotfiyGuardUpdateSuccess failed\n" );
	}
#endif

}


BOOL32 g_bQuit = FALSE;

#ifdef _LINUX_
void OspDemoQuitFunc( int nSignalNum )
{
    g_bQuit = TRUE;
    //MmpFileLog( MMPLOGFILENAME,"[Mmp] Mmp quit normally!\n");
	LogPrint(LOG_LVL_WARNING,MID_PUB_ALWAYS,"[VMP8KI]vmp quit normally!\n");
}
#endif

int main(int argc, char** argv  )
{

	// 设置进程异常捕获回调
#ifdef _LINUX_
    s32 nRet = StartCaptureException();
	if (0 != nRet)
	{
		printf("VMP: StartCaptureException Error(%d), Ignored.\n", nRet);
	}
    else
    {
        printf("VMP: StartCaptureException Succeeded\n");
    }    
#endif
	
#ifdef WIN32
    OspRegistModule("codecwrapper.dll");
    OspRegistModule("ospdll.dll");
#endif
	
    //Osp telnet 初始授权 [11/28/2006-zbq]
#ifdef NDEBUG
    ::OspInit(FALSE, BRD_TELNET_PORT);
    OspTelAuthor( MCU_TEL_USRNAME, MCU_TEL_PWD );
#else
    ::OspInit(TRUE, BRD_TELNET_PORT);
#endif
	
    OspSetPrompt("vmp8ki");
	
	
    //提高进程优先级
#ifdef WIN32
    SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);     
#endif
	
	
	logsetport( 5676,1 );
	u16 wLogRet = LogInit(LOGFILE_VMP8KI);
	if( LOG_ERR_NO_ERR != wLogRet )
	{
		printf( "KdvLog Init failed,error:%d\n",wLogRet);
	}		

	//注册调试函数
	VmpAPIEnableInLinux();

	//启动8kivmp
	vmpstart();

    //注册退出清除函数
#ifdef _LINUX_
	OspRegQuitFunc( OspDemoQuitFunc );
#endif

    g_bQuit = FALSE;
		
	while(!g_bQuit)
	{
	    OspDelay(1000);
	}   
    
#ifdef _LINUX_
    OspDisconnectAllTcpNode();
#endif
    
    OspQuit();



	return 0;
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

#endif

/* parse cpu context information is architecture depent, 
   encapsulate as helper function can be easy */

int parse_ppc_context(void* tmp)
{
/*lint -save -esym(530, regs)*/
/*lint -save -e578*/
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
    int nLen = sprintf(achInfo, "\nSytem time %s\n", ctime(&tiCurTime));
    writeexclog(achInfo, nLen);
    nLen = sprintf(achInfo, "Exception instruction address: 0x%x\n", epc);
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

/*lint -restore*/
/*lint -restore*/
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
	void *pSym = NULL;

	
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
		free(pSym);
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
	void *pSym = NULL;
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
	Elf32_Ehdr ehdr;
	Elf32_Shdr *shdr;
	FILE *file;
            
    signal(SIGPIPE, SIG_IGN);

	install_sigaction();
	
	file = fopen(g_szAppName, "rb");
	if (file == NULL)
	{
		printf("vmp8ki: Open file %s error: %s\n\n", g_szAppName, strerror(errno));
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



/////////////////////////////////////////////////////异常捕获处理
