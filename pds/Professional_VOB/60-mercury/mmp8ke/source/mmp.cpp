/*****************************************************************************
模块名      : MMP容器
文件名      : mmp.cpp
创建时间    : 2009年 02月 10日
实现功能    : 
作者        : 周广程
版本        : 
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2009/02/10  1.0         周广程        创建
******************************************************************************/
#include "osp.h"
#include "kdvsys.h"
#include "mcuconst.h"
//#include "mcuver.h"
#include "mmp.h"
#include "eqpcfg.h"
#include "eqplib.h"
//#include "commonlib.h"
#include "watchdog.h"
#include "mmpcommon.h"
#if defined(_8KE_) || defined(_8KH_)
#include "eqpgvari.h"
#else
#include "multpic_gpu.h"
#include "audiomixer_gpu.h"
#endif

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
const s8 g_szAppName[] = "/opt/mcu/mmp_8000h";
#elif defined(_8KE_)
const s8 g_szAppName[] = "/opt/mcu/mmp_8000e";
#elif defined(_8KI_)
const s8 g_szAppName[] = "/opt/mcu/mmp_8000i";
#endif


// 开始捕捉入口函数
// Ret - 0 正常返回
s32 StartCaptureException();

#endif //_LINUX_

FILE * g_pMmpLog = NULL;

CMulPic  * g_pcHardMulPic;                              //画面复合器的封装
//CCodecAdapter * g_apcAdapter[MAXNUM_BAS_CHNNL];         //适配器封装
CAudioMixer   *g_apcAMixer[MAXNUM_MIXER_GROUP];        //混音组实例

TResBrLimitTable g_tResBrLimitTable;

//extern void FileLog(FILE * pLogFile, s8 * pszFmt, ...);
//extern BOOL32 CreatFile(const s8 * lpszFileFullPath);
/////////////////////////////////////////////////////异常捕获处理


//
//
//
API void mmpstart()
{
    T8keAudioMixerCfg tAudioMixerCfg;
    T8keVmpCfg tVmpCfg;
    T8keBasCfg tBasCfg;

	// 设置进程异常捕获回调
#ifdef _LINUX_
    s32 nRet = StartCaptureException();
	if (0 != nRet)
	{
		printf("Mmp: StartCaptureException Error(%d), Ignored.\n", nRet);
	}
    else
    {
        printf("Mmp: StartCaptureException Succeeded\n");
    }    
#endif

#ifdef WIN32
    OspRegistModule("codecwrapper.dll");
    OspRegistModule("ospdll.dll");
#endif

    memset(&tVmpCfg, 0, sizeof(tVmpCfg));
    memset(&tBasCfg, 0, sizeof(tBasCfg));
    memset(&tAudioMixerCfg, 0, sizeof(tAudioMixerCfg));

    //Osp telnet 初始授权 [11/28/2006-zbq]
#ifdef NDEBUG
    ::OspInit(FALSE, BRD_TELNET_PORT,"MMP");
    OspTelAuthor( MCU_TEL_USRNAME, MCU_TEL_PWD );
#else
    ::OspInit(TRUE, BRD_TELNET_PORT,"MMP");
#endif

    OspSetPrompt("mmp");
	
    BOOL bInitCodecOk = InitCodecWrapper();
    if (!bInitCodecOk)
    {
        printf("InitCodecWrapper failed, exit!!!%d\n",bInitCodecOk);
        return;
    }

    //提高进程优先级
#ifdef WIN32
    SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);     
#endif


	logsetport( 5676,1 );
	u16 wLogRet = LogInit(LOGFILE_8KH_MMPPREIEQP);
	if( LOG_ERR_NO_ERR != wLogRet )
	{
		printf( "KdvLog Init failed,error:%d\n",wLogRet);
	}		
	

    TMmpStartMod tMmpStartMod;
    GetStartMod( tMmpStartMod );
    
    TConnectInfo tConnectInfo;
    GetConnectInfo( tConnectInfo );

	//读分辨率码率限制信息表
	if (tMmpStartMod.IsRunBas() || tMmpStartMod.IsRunVmp())
	{
		BOOL32 bReadTable = g_tResBrLimitTable.ReadResBrLimitTable();
		if ( !bReadTable )
		{
			printf("g_tResBrLimitTable.ReadResBrLimitTable() error,no configfile or defaulttable\n");
			
		}
	}


    // start up vmp 
    if (tMmpStartMod.IsRunVmp())
    {     
        tVmpCfg.SetEqpIp( tConnectInfo.GetLocalIp() );
        tVmpCfg.SetConnectIp( tConnectInfo.GetMcuIp() );
        tVmpCfg.SetConnectPort( tConnectInfo.GetMcuPort() );        
        tVmpCfg.SetEqpType( EQP_TYPE_VMP );
        tVmpCfg.SetMcuId( LOCAL_MCUID );
        tVmpCfg.SetIsEmbed( FALSE );
		tVmpCfg.SetAlias("VMP");
		tVmpCfg.SetIframeInterval(tMmpStartMod.GetVmpIframeInterval());
        printf(" Starting  VMP  ......\n");
		logenablemod(MID_MCU_VMP);
        vmpinit(&tVmpCfg);
    }

    // start up BAS 
    if (tMmpStartMod.IsRunBas())
    {     
        tBasCfg.SetEqpIp( tConnectInfo.GetLocalIp() );
        tBasCfg.SetConnectIp( tConnectInfo.GetMcuIp() );
        tBasCfg.SetConnectPort( tConnectInfo.GetMcuPort() );        
        tBasCfg.SetEqpType( EQP_TYPE_BAS );
        tBasCfg.SetMcuId( LOCAL_MCUID );
        tBasCfg.SetIsEmbed( FALSE );
		tBasCfg.SetIframeInterval(tMmpStartMod.GetBasIframeInterval());
        
        printf(" Starting  BAS  ......\n");
		logenablemod(MID_MCU_BAS);
        basInit(&tBasCfg);
    }

    // start up aud
    if (tMmpStartMod.IsRunMixer())
    {     
        tAudioMixerCfg.SetEqpIp( tConnectInfo.GetLocalIp() );
        tAudioMixerCfg.SetConnectIp( tConnectInfo.GetMcuIp() );
        tAudioMixerCfg.SetConnectPort( tConnectInfo.GetMcuPort() );        
        tAudioMixerCfg.SetEqpType( EQP_TYPE_MIXER );
        tAudioMixerCfg.SetMcuId( LOCAL_MCUID );
        tAudioMixerCfg.SetIsEmbed( FALSE );
        tAudioMixerCfg.SetMaxMixGrpCount(MAXNUM_MIXER_GROUP_8KE);    //guzh: FIXME: 先填写1，目前一个混音器就支持1个混音组
        tAudioMixerCfg.SetMaxChnnlInGrp(MAXNUM_MIXER_CHNNL);
        tAudioMixerCfg.SetAlias("Mixer");
        printf(" Starting  Mixer  ......\n");
		logenablemod(MID_MCU_MIXER);
        mixInit(&tAudioMixerCfg);
    }
}


BOOL32 g_bQuit = FALSE;

#ifdef _LINUX_
void OspDemoQuitFunc( int nSignalNum )
{
    //g_bQuit = TRUE;
    MmpFileLog( MMPLOGFILENAME,"[Mmp] Mmp quit normally:%d! g_bQuit not set true\n",nSignalNum);
}
#endif

/*lint -save -e715*/
int main(int argc, char** argv  )
{
//add by zhanghb 090313
#ifdef  NDEBUG
// 	if(argc <= 1)
// 	{
// 		return 0;
// 	}
// 	if (0 != (strcmp("guard",argv[1])))
// 	{
// 		return 0;
// 	}
	//启动watchdogclient
//	WDStart(em_MMP);
#endif
// end add 
    // 增加启动日志打印, zgc, 2009-04-23
    MmpFileLog( MMPLOGFILENAME, "[Mmp] Mmp start!\n" );

	mmpstart();

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
  
#ifndef _8KH_
    OspQuit();
#endif

	if ( NULL != g_pMmpLog )
	{
		fclose( g_pMmpLog );
	}

	return 0;
}	


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

/*lint -save -e716*/
/* parse cpu context information is architecture depent, 
   encapsulate as helper function can be easy */
int parse_x86_context(void* tmp)
{
	struct ucontext* ct = (struct ucontext*)tmp;	
	int* regs = (int*)ct->uc_mcontext.gregs;
	int  epc = regs[14];
	int *presp = (int *)regs[6];
	char achInfo[255] = {0};
	int nLen = 0;

	// 将打印写入log文件
	printf("\nexception instruction address: 0x%x\n", epc);
	nLen = sprintf(achInfo, "\nexception instruction address: 0x%x\n", epc);
	writeexclog(achInfo, nLen);
	
	printf("\n===================CPU registers==============================\n");
	nLen = sprintf(achInfo, "\n===================CPU registers==============================\n");
	writeexclog(achInfo, nLen);

	printf("cpu register value:\n");
	nLen = sprintf(achInfo, "cpu register value:\n");
	writeexclog(achInfo, nLen);

	printf("GS:0x%x\t FS:0x%x\t ES:0x%x\t DS:0x%x\n", 
	       regs[0], regs[1], regs[2], regs[3]);
	nLen = sprintf(achInfo, "GS:0x%x\t FS:0x%x\t ES:0x%x\t DS:0x%x\n", 
	       regs[0], regs[1], regs[2], regs[3]);
	writeexclog(achInfo, nLen);

	printf("CS:EIP:   0x%x:0x%x\nSS:UESP:  0x%x:0x%x\n", 
	       regs[15],regs[14], regs[18], regs[17]);
	nLen = sprintf(achInfo, "CS:EIP:   0x%x:0x%x\nSS:UESP:  0x%x:0x%x\n", 
	       regs[15],regs[14], regs[18], regs[17]);
	writeexclog(achInfo, nLen);

	printf("EDI:0x%x\t ESI:0x%x\t EBP:0x%x\n",regs[4], regs[5], regs[6]);
	nLen = sprintf(achInfo, "EDI:0x%x\t ESI:0x%x\t EBP:0x%x\n",regs[4], regs[5], regs[6]);
	writeexclog(achInfo, nLen);

	printf("ESP:0x%x\t EBX:0x%x\t EDX:0x%x\n",regs[7], regs[8], regs[9]);
	nLen = sprintf(achInfo, "ESP:0x%x\t EBX:0x%x\t EDX:0x%x\n",regs[7], regs[8], regs[9]);
	writeexclog(achInfo, nLen);

	printf("ECX:0x%x\t\t EAX:0x%x\n",regs[10], regs[11]);
	nLen = sprintf(achInfo, "ECX:0x%x\t\t EAX:0x%x\n",regs[10], regs[11]);
	writeexclog(achInfo, nLen);

	printf("TRAPNO:0x%x\t ERR:0x%x\t EFL:0x%x\n",regs[12], regs[13], regs[16]);
	nLen = sprintf(achInfo, "TRAPNO:0x%x\t ERR:0x%x\t EFL:0x%x\n",regs[12], regs[13], regs[16]);
	writeexclog(achInfo, nLen);

	printf("\n=================== call trace ==============================\n");
	nLen = sprintf(achInfo, "\n=================== call trace ==============================\n");
	writeexclog(achInfo, nLen);


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
			{
				printf("programs's user stack error !!\n");
				nLen = sprintf(achInfo, "programs's user stack error !!\n");
				writeexclog(achInfo, nLen);
			}
			break;
		}
		presp = (int *)(*(presp));/* pre function's base sp */		
	}
	
#endif			
	free(sym);
	free(strtab);
	return 0;
}



void parse_context(void* uc)
{
	parse_x86_context(uc);
}


int printf_symbl_name(int pc)
{
    char achInfo[255] = {0};
	int nLen = 0;
	Elf32_Sym *syml = get_func_symb_by_addr(sym,symsize, pc);
	if (syml) {
		if (int(syml->st_name) >= strsize)
		{
			printf("st_name if big then strtable size\r\n");
			nLen = sprintf(achInfo, "st_name if big then strtable size\r\n");
			writeexclog(achInfo, nLen);
			return -1;
		}
		//return (strtab+syml->st_name);
		printf("pc:%x %s\r\n", pc, (strtab+syml->st_name));

        // xsl [8/16/2006] write to log
        nLen = sprintf(achInfo, "pc:%x %s\r\n", pc, (strtab+syml->st_name));
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
    
    if ( NULL != g_pcHardMulPic )
    {
        delete g_pcHardMulPic;
        g_pcHardMulPic = NULL;
    }
    
    /*for ( u8 byBasIdx = 0; byBasIdx < MAXNUM_BAS_CHNNL; byBasIdx++ )
    {
        if ( NULL != g_apcAdapter[byBasIdx] )
        {
            delete g_apcAdapter[byBasIdx];
            g_apcAdapter[byBasIdx] = NULL;
        }
    }*/
    
    for ( u8 byMixIdx = 0; byMixIdx < MAXNUM_MIXER_GROUP; byMixIdx++ )
    {
        if ( NULL != g_apcAMixer[byMixIdx] )
        {
            delete g_apcAMixer[byMixIdx];
            g_apcAMixer[byMixIdx] = NULL;
        }
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
		printf("Mmp: Open file %s error: %s\n\n", g_szAppName, strerror(errno));
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
void CreatModCfg( void )
{    
    s8 achProfileName[MAX_VALUE_LEN + 1];
    memset( achProfileName, 0, sizeof(achProfileName) );
    sprintf( achProfileName, "%s/%s", DIR_CONFIG, MODCFG_INI);

    FILE *stream = fopen( achProfileName, "r" );
    if( stream != NULL )
    {
        fclose(stream);
        return;
    }
    
    BOOL32 bCreat = CreatFile( achProfileName );
    if ( !bCreat )
    {
        OspPrintf( TRUE, FALSE, "[CreatModCfg] Creat modcfg.ini failed!\n");
        return;
    }

    SetRegKeyInt( achProfileName, "App", "IsRunVmp", 1 ); 
    SetRegKeyInt( achProfileName, "App", "IsRunMixer", 1 );
    SetRegKeyInt( achProfileName, "App", "IsRunBas", 1 );

    return;
}

void GetStartMod( TMmpStartMod &tMmpStartMod )
{

    CreatModCfg();

    s8 achProfileName[MAX_VALUE_LEN + 1];
    memset( achProfileName, 0, sizeof(achProfileName) );
    sprintf( achProfileName, "%s/%s", DIR_CONFIG, MODCFG_INI);
    
    s32 nValue = 0;
    BOOL32 bResult = TRUE;
    //s8    achDefStr[] = "Cannot find the section or key";
    //s8    achReturn[MAX_VALUE_LEN + 1] = {0};
    
    // VMP
    bResult = GetRegKeyInt( achProfileName, "App", "IsRunVmp", 0, &nValue ); 
    if( FALSE == bResult)  
    {
        printf("[GetStartMod] Wrong profile while reading %s!\n", "IsRunVmp" );
        tMmpStartMod.SetIsRunVmp( TRUE );
    }
    else
    {
        tMmpStartMod.SetIsRunVmp( (nValue == 1) ? TRUE : FALSE );
    }


    // MIXER  
    bResult = GetRegKeyInt( achProfileName, "App", "IsRunMixer", 0, &nValue );
    if( FALSE == bResult)  
    {
        printf("[BoardAgent] Wrong profile while reading %s!\n", "IsRunMixer" );
        tMmpStartMod.SetIsRunMixer( TRUE );
    }
    else
    {
        tMmpStartMod.SetIsRunMixer( (nValue == 1) ? TRUE : FALSE );
    }

    
    // BAS
    bResult = GetRegKeyInt( achProfileName, "App", "IsRunBas", 0, &nValue );
    if( FALSE == bResult)  
    {
        printf("[BoardAgent] Wrong profile while reading %s!\n", "IsRunBas" );
        tMmpStartMod.SetIsRunBas( TRUE );
    }
    else
    {
        tMmpStartMod.SetIsRunBas( (nValue == 1) ? TRUE : FALSE );
    }

	//VMP IFAME interval
	bResult = GetRegKeyInt( achProfileName, "VMP8KE", "VMPIframeInterval", 1000, &nValue );
    if( FALSE == bResult)  
    {
        printf("[BoardAgent] Wrong profile while reading %s!\n", "VMPIframeInterval" );
        tMmpStartMod.SetVmpIframeInterval(1000);
    }
    else
    {
        tMmpStartMod.SetVmpIframeInterval(nValue);
    }
	
	//BAS IFAME  interval
	bResult = GetRegKeyInt( achProfileName, "BAS8KE", "BASIframeInterval", 1000, &nValue );
    if( FALSE == bResult)  
    {
        printf("[BoardAgent] Wrong profile while reading %s!\n", "BASIframeInterval" );
        tMmpStartMod.SetBasIframeInterval( 1000 );
    }
    else
    {
        tMmpStartMod.SetBasIframeInterval(nValue);
    }
}

void CreatConnectCfg( void )
{
    s8 achProfileName[MAX_VALUE_LEN + 1];
    memset( achProfileName, 0, sizeof(achProfileName) );
    sprintf( achProfileName, "%s/%s", DIR_CONFIG, CONNECT_INI);
    
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
    
    //guzh 不写默认文件
    /*
    SetRegKeyString( achProfileName, "McuInfo", "McuIpAddr", "127.0.0.1" );
    SetRegKeyInt( achProfileName, "McuInfo", "McuPort", 60000 );
    SetRegKeyInt( achProfileName, "McuInfo", "HeartBeatTime", DEF_OSPDISC_HBTIME );
    SetRegKeyInt( achProfileName, "McuInfo", "HeartBeatNum", DEF_OSPDISC_HBNUM );
    */
    
    return;
}

void GetConnectInfo( TConnectInfo &tConnectInfo )
{
    CreatConnectCfg();

    s8    achProfileName[32];
    BOOL32 bResult = FALSE;
    s8    achDefStr[] = "Cannot find the section or key";
    s8    achReturn[MAX_VALUE_LEN + 1] = {0};
    s32   nValue = 0;

	sprintf( achProfileName, "%s/%s", DIR_CONFIG, CONNECT_INI);

    u32 tdwIPList[5];  //列举到的当前设置的所有有效ip
    u16 dwIPNum = 0;   //列举到ip 数目
    
    u32 dwLocalIp = 0;
	bResult = GetRegKeyString(achProfileName, "McuInfo", "McuIpAddr", achDefStr, achReturn, MAX_VALUE_LEN+1);
	if ( bResult)
	{
		// 只对一体机有效，因为用的是同一IP [pengguofeng 8/21/2012]
		dwLocalIp = ntohl(INET_ADDR(achReturn));
		printf("[DaemonProcConnetTimerOut]IP(%s)from %s will be used first\n", achReturn, achProfileName);
	}

	if ( 0 == dwLocalIp )
	{		
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

	LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_EQP, "[GetConnectInfo] localIp:%x\n", dwLocalIp);
    tConnectInfo.SetLocalIp( dwLocalIp );

    bResult = GetRegKeyString( achProfileName, "McuInfo", "McuIpAddr",achDefStr, achReturn, MAX_VALUE_LEN + 1 );
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
        printf("[GetConnectInfo] Wrong profile while reading %s!\n", "McuIpAddr" );

        tConnectInfo.SetMcuIp( dwLocalIp );

    }
    

	bResult = GetRegKeyInt( achProfileName, "McuInfo", "McuPort", MCU_LISTEN_PORT, &nValue );
	if( !bResult || nValue == 0)  
	{
		printf("[BoardAgent] Wrong profile while reading %s!\n", "MpcPort" );
		tConnectInfo.SetMcuPort( MCU_LISTEN_PORT );
	}
	else
    {
        tConnectInfo.SetMcuPort( (u16)nValue );
    }

    //读取osp断链检测参数
    bResult = GetRegKeyInt( achProfileName, "McuInfo", "HeartBeatTime", DEF_OSPDISC_HBTIME, &nValue );
    if ( !bResult )
    {
        tConnectInfo.SetDiscHeartBeatTime( DEF_OSPDISC_HBTIME );
    }
    else
    {
        if ( nValue >= 3 )
        {
            tConnectInfo.SetDiscHeartBeatTime( (u16)nValue );
        }
        else
        {
            tConnectInfo.SetDiscHeartBeatTime( DEF_OSPDISC_HBTIME );
        }
    }
    
    GetRegKeyInt( achProfileName, "McuInfo", "HeartBeatNum", DEF_OSPDISC_HBNUM, &nValue );
    if ( !bResult )
    {
        tConnectInfo.SetDiscHeartBeatNum( DEF_OSPDISC_HBNUM );
    }
    else
    {
        if ( nValue >= 1 )
        {
            tConnectInfo.SetDiscHeartBeatNum( (u8)nValue );
        }
        else
        {
            tConnectInfo.SetDiscHeartBeatNum( DEF_OSPDISC_HBNUM );
        }
    }

	return;
}   


/*=============================================================================
函 数 名： MmpFileLog
功    能： 日志文件输出
算法实现： 
全局变量： 
参    数：  const s8 * pchLogFile
           s8 * pszFmt
           ...
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2009/6/3   4.0			周广程                  创建
=============================================================================*/
/*lint -save -esym(526, __builtin_va_start)*/
/*lint -save -esym(628, __builtin_va_start)*/
/*lint -save -e438 -e530*/
void MmpFileLog( const s8 * pchLogFile, s8 * pszFmt, ... )
{
	if ( NULL == pchLogFile )
	{
		return;
	}

	va_list argptr;
	s8 achPrintBuf[255] = {0};

	va_start( argptr, pszFmt );    
    vsprintf( achPrintBuf, pszFmt, argptr );   
    va_end(argptr); 

	if ( NULL == g_pMmpLog )
	{
		g_pMmpLog = fopen( pchLogFile, "ab" );
		if ( NULL == g_pMmpLog )
		{
			return;
		}
	}

	FileLog( g_pMmpLog, achPrintBuf );
	return;
}
/*lint -restore*/

/////////////////////////////////////////////////////异常捕获处理
