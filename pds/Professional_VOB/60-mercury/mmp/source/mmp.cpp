#include "osp.h"
//#include "kdvtype.h"
#include "kdvsys.h"
#include "eqplib.h"
//#include "mcuconst.h"
#include "mcuver.h"
#include "boardagent.h"
#include "drawinterface.h"

#ifdef _LINUX_
#include "codeccommon.h"
#endif

#ifdef MCUEQPMMP
#include "mmp_agent.h"
#endif
#ifdef MCUEQPAPU
#include "imt_agent.h"
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


#ifdef MCUEQPMMP
#define APPNAME     (LPCSTR)"/usr/bin/kdvvmp"
#endif
#ifdef MCUEQPAPU
#define APPNAME     (LPCSTR)"/usr/bin/kdvapu"
#endif
LPCSTR g_szAppName;

// 开始捕捉入口函数
// Ret - 0 正常返回
s32 StartCaptureException();

#endif //_LINUX_

/////////////////////////////////////////////////////异常捕获处理


//
//
//
API void mmpstart()
{
    TAudioMixerCfg tAudioMixerCfg;
    TVmpCfg tVmpCfg, tMpwCfg;
    TEqpCfg tBasEqpCfg;
    TEqpCfg tTvWallCfg;
    TPrsCfg tPrsCfg;
    u8 buf[MAXNUM_MAP];

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


    memset(&tVmpCfg, 0, sizeof(TVmpCfg));
    memset(&tMpwCfg, 0, sizeof(tMpwCfg));
    memset(&tBasEqpCfg, 0, sizeof(tBasEqpCfg));
    memset(&tAudioMixerCfg, 0, sizeof(TAudioMixerCfg));
    memset(&tTvWallCfg, 0, sizeof(tTvWallCfg));
    memset(&tPrsCfg, 0, sizeof(TPrsCfg));
		
    //need to activate kdvsyslib.a and DataSwitch.a on VxWorks
#ifdef _VXWORKS_    
    Trim("Kdv Mcu 4.0");
#endif

    ::OspInit(TRUE, BRD_TELNET_PORT);

    //Osp telnet 初始授权 [11/28/2006-zbq]
    OspTelAuthor( MCU_TEL_USRNAME, MCU_TEL_PWD );

#ifdef _LINUX_
	  CodecWrapperPreInit();
#endif	  
			
#ifdef MCUEQPMMP 
    // painthelp();  
     
    if (!InitMmpAgent())
    {
        printf("Init MMP Agent Failed!!!!!!\n");
        return;
    }
    else
    {
        printf("Init MMP Agent Succeeded!\n");
    }
    
    BrdGetDstMcuNode();
    
    // start up vmp 
    if (g_cBrdAgentApp.IsRunVMP())
    {
        /**/
        s32 nNum = g_cBrdAgentApp.GetVMPMAPId(buf, MAXNUM_MAP);
        g_cBrdAgentApp.GetMAPInfo(buf[0],
                                  (u32*)&tVmpCfg.m_atMap[0].dwCoreSpd,
                                  (u32*)&tVmpCfg.m_atMap[0].dwMemSpd,
                                  (u32*)&tVmpCfg.m_atMap[0].dwMemSize,
                                  (u32*)&tVmpCfg.m_atMap[0].dwPort);
        tVmpCfg.wMAPCount     = nNum;
        tVmpCfg.dwConnectIP   = g_cBrdAgentApp.GetMpcIpA();
        tVmpCfg.wConnectPort  = g_cBrdAgentApp.GetMpcPortA();
        tVmpCfg.dwConnectIpB  = g_cBrdAgentApp.GetMpcIpB();
        tVmpCfg.wConnectPortB = g_cBrdAgentApp.GetMpcPortB();
        
        tVmpCfg.byEqpType     = g_cBrdAgentApp.GetVMPType();
        tVmpCfg.byEqpId       = g_cBrdAgentApp.GetVMPId();
        tVmpCfg.dwLocalIP     = g_cBrdAgentApp.GetVMPIpAddr();
        tVmpCfg.wRcvStartPort = g_cBrdAgentApp.GetVMPRecvStartPort();
        tVmpCfg.wMcuId        = g_cBrdAgentApp.GetMcuId();
        tVmpCfg.byDbVid       = g_cBrdAgentApp.GetVMPDecodeNumber();
        g_cBrdAgentApp.GetVMPAlias(tVmpCfg.achAlias, MAXLEN_ALIAS);
        tVmpCfg.achAlias[MAXLEN_ALIAS] = '\0';
        printf(" Starting  VMP  ......\n");
        vmpinit(&tVmpCfg);
        //vmpinit(&tVmpCfg, g_cBrdAgentApp.GetVMPDecodeNumber());
    }

    if(g_cBrdAgentApp.IsRunMpw())
    {
        s32 nNum = g_cBrdAgentApp.GetMpwMAPId(buf, MAXNUM_MAP);
        g_cBrdAgentApp.GetMAPInfo(buf[0],
                                  (u32*)&tMpwCfg.m_atMap[0].dwCoreSpd,
                                  (u32*)&tMpwCfg.m_atMap[0].dwMemSpd,
                                  (u32*)&tMpwCfg.m_atMap[0].dwMemSize,
                                  (u32*)&tMpwCfg.m_atMap[0].dwPort);
        tMpwCfg.wMAPCount     = nNum;
        tMpwCfg.dwConnectIP   = g_cBrdAgentApp.GetMpcIpA();
        tMpwCfg.wConnectPort  = g_cBrdAgentApp.GetMpcPortA();
        tMpwCfg.dwConnectIpB  = g_cBrdAgentApp.GetMpcIpB();
        tMpwCfg.wConnectPortB = g_cBrdAgentApp.GetMpcPortB();
        
        tMpwCfg.byEqpType     = g_cBrdAgentApp.GetMpwType();
        tMpwCfg.byEqpId       = g_cBrdAgentApp.GetMpwId();
        tMpwCfg.dwLocalIP     = g_cBrdAgentApp.GetMpwIpAddr();
        tMpwCfg.wRcvStartPort = g_cBrdAgentApp.GetMpwRecvStartPort();
        tMpwCfg.wMcuId        = g_cBrdAgentApp.GetMcuId();
        g_cBrdAgentApp.GetMpwAlias(tMpwCfg.achAlias, MAXLEN_ALIAS);
        tMpwCfg.achAlias[MAXLEN_ALIAS] = '\0';
        printf(" Starting  MPW  ......\n");
        MpwInit(tMpwCfg);
    }

#endif

#ifdef MCUEQPAPU  		
    if (!InitImtAgent())
    {
        printf("Init IMT Agent Failed!!!!!!\n");
        return;
    }
    
    BrdGetDstMcuNode();
    
#endif

    // start up audio mixer 
    if (g_cBrdAgentApp.IsRunMixer())
    {
        s32 nNum = g_cBrdAgentApp.GetMixerMAPId(buf, MAXNUM_MAP);
        tAudioMixerCfg.wMAPCount = (u16)nNum;
        for(s32 nLp=0;nLp<nNum; nLp++)
        {
            tAudioMixerCfg.m_atMap[nLp].byMapId = buf[nLp];
            g_cBrdAgentApp.GetMAPInfo(buf[nLp],
                                     (u32*)&tAudioMixerCfg.m_atMap[nLp].dwCoreSpd,
                                     (u32*)&tAudioMixerCfg.m_atMap[nLp].dwMemSpd,
                                     (u32*)&tAudioMixerCfg.m_atMap[nLp].dwMemSize,
                                     (u32*)&tAudioMixerCfg.m_atMap[nLp].dwPort);
        }
        tAudioMixerCfg.dwConnectIP = g_cBrdAgentApp.GetMpcIpA();
        tAudioMixerCfg.wConnectPort = g_cBrdAgentApp.GetMpcPortB();
        tAudioMixerCfg.dwConnectIpB = g_cBrdAgentApp.GetMpcIpB();
        tAudioMixerCfg.wConnectPortB = g_cBrdAgentApp.GetMpcPortB();
        
        tAudioMixerCfg.byEqpType = g_cBrdAgentApp.GetMixerType();
        tAudioMixerCfg.byEqpId = g_cBrdAgentApp.GetMixerId();
        tAudioMixerCfg.dwLocalIP = g_cBrdAgentApp.GetMixerIpAddr();
        tAudioMixerCfg.wRcvStartPort = g_cBrdAgentApp.GetMixerRecvStartPort();
        tAudioMixerCfg.wMcuId = g_cBrdAgentApp.GetMcuId();
        tAudioMixerCfg.byMaxMixGroupCount = (u8)nNum;
        tAudioMixerCfg.byMaxChannelInGroup = g_cBrdAgentApp.GetMixerMaxChannelInGrp();
        g_cBrdAgentApp.GetMixerAlias(tAudioMixerCfg.achAlias,MAXLEN_ALIAS );
        tAudioMixerCfg.achAlias[MAXLEN_ALIAS] = '\0';
        printf(" Starting Audio Mixer ......\n");
        if (!mixInit(&tAudioMixerCfg))
        {
            printf("Init audio mixer failed!\r");
        }
        else
        {
            printf("Starting audio mixer success !!!\n");
        }
    }

    // start up bitstream adapter server
    if (g_cBrdAgentApp.IsRunBas())
    {
        s32 nNum = g_cBrdAgentApp.GetBasMAPId(buf,MAXNUM_MAP);
        for(s32 nLp=0; nLp<MAXNUM_MAP; nLp++)
        {
            tBasEqpCfg.m_atMap[nLp].byMapId = buf[nLp];
            g_cBrdAgentApp.GetMAPInfo(buf[nLp],
                                      (u32*)&tBasEqpCfg.m_atMap[nLp].dwCoreSpd,
                                      (u32*)&tBasEqpCfg.m_atMap[nLp].dwMemSpd,
                                      (u32*)&tBasEqpCfg.m_atMap[nLp].dwMemSize,
                                      (u32*)&tBasEqpCfg.m_atMap[nLp].dwPort);
        }
        tBasEqpCfg.dwConnectIP = g_cBrdAgentApp.GetMpcIpA();
        tBasEqpCfg.wConnectPort = g_cBrdAgentApp.GetMpcPortA();
        tBasEqpCfg.dwConnectIpB = g_cBrdAgentApp.GetMpcIpB();
        tBasEqpCfg.wConnectPortB = g_cBrdAgentApp.GetMpcPortB();

        tBasEqpCfg.byEqpType = g_cBrdAgentApp.GetBasType();
        tBasEqpCfg.byEqpId = g_cBrdAgentApp.GetBasId();
        tBasEqpCfg.dwLocalIP = g_cBrdAgentApp.GetBasIpAddr();
        tBasEqpCfg.wRcvStartPort = g_cBrdAgentApp.GetBasRecvStartPort();
        tBasEqpCfg.wMcuId = g_cBrdAgentApp.GetMcuId();
        tBasEqpCfg.wMAPCount = (u8)nNum;
        g_cBrdAgentApp.GetBasAlias(tBasEqpCfg.achAlias,MAXLEN_ALIAS );
        tBasEqpCfg.achAlias[MAXLEN_ALIAS] = '\0';
        printf("Starting BAS ......\n");
        if (!basInit(&tBasEqpCfg))
        {
            printf("Init BAS failed!\r");
        }
        else
        {
            printf("Starting BAS success !!!\n");
        }
    }

    // start up TV wall
    if (g_cBrdAgentApp.IsRunTVWall())
    {
        s32 nNum = g_cBrdAgentApp.GetTVWallMAPId(buf, MAXNUM_MAP);
        for(s32 nLp=0;nLp<nNum;nLp++)
        {
            tTvWallCfg.m_atMap[nLp].byMapId = buf[nLp];
            g_cBrdAgentApp.GetMAPInfo(buf[nLp],
                                      (u32*)&tTvWallCfg.m_atMap[nLp].dwCoreSpd,
                                      (u32*)&tTvWallCfg.m_atMap[nLp].dwMemSpd,
                                      (u32*)&tBasEqpCfg.m_atMap[nLp].dwMemSize,
                                      (u32*)&tTvWallCfg.m_atMap[nLp].dwPort);
        }
        tTvWallCfg.wMAPCount     = (u8)nNum;
        tTvWallCfg.dwConnectIP   = g_cBrdAgentApp.GetMpcIpA();
        tTvWallCfg.wConnectPort  = g_cBrdAgentApp.GetMpcPortA();
        tTvWallCfg.dwConnectIpB   = g_cBrdAgentApp.GetMpcIpB();
        tTvWallCfg.wConnectPortB  = g_cBrdAgentApp.GetMpcPortB();

        tTvWallCfg.byEqpType     = g_cBrdAgentApp.GetTWType();
        tTvWallCfg.byEqpId       = g_cBrdAgentApp.GetTWId();
        tTvWallCfg.dwLocalIP     = g_cBrdAgentApp.GetTWIpAddr();
        tTvWallCfg.wRcvStartPort = g_cBrdAgentApp.GetTWRecvStartPort();
        tTvWallCfg.wMcuId        = g_cBrdAgentApp.GetMcuId();
        g_cBrdAgentApp.GetTWAlias(tTvWallCfg.achAlias,MAXLEN_ALIAS );
        tTvWallCfg.achAlias[MAXLEN_ALIAS] = '\0';
        printf(" Starting  TV Wall  ......\n");
        if (!twInit(&tTvWallCfg))
        {
            while(1)
            {
                printf("Init TV Wall failed!\r");
                OspTaskDelay(60);
            }
        }
        printf("Starting  TV Wall success !!!\n");
    }

    if (g_cBrdAgentApp.IsRunPrs())
    {
        tPrsCfg.wMcuId        = g_cBrdAgentApp.GetMcuId();
        tPrsCfg.byEqpId       = g_cBrdAgentApp.GetPrsId();
        tPrsCfg.byEqpType     = g_cBrdAgentApp.GetPrsType();
        tPrsCfg.dwLocalIP     = g_cBrdAgentApp.GetPrsIpAddr();
        tPrsCfg.wRcvStartPort = g_cBrdAgentApp.GetPrsRecvStartPort();
        tPrsCfg.dwConnectIP   = g_cBrdAgentApp.GetMpcIpA();
        tPrsCfg.wConnectPort  = g_cBrdAgentApp.GetMpcPortA();
        tPrsCfg.dwConnectIpB = g_cBrdAgentApp.GetMpcIpB();
        tPrsCfg.wConnectPortB = g_cBrdAgentApp.GetMpcPortB();
        
        g_cBrdAgentApp.GetPrsAlias(tPrsCfg.achAlias, MAXLEN_ALIAS);
        tPrsCfg.achAlias[MAXLEN_ALIAS] = '\0';
        g_cBrdAgentApp.GetPrsRetransPara(&tPrsCfg.m_wFirstTimeSpan,
                                         &tPrsCfg.m_wSecondTimeSpan,
                                         &tPrsCfg.m_wThirdTimeSpan,
                                         &tPrsCfg.m_wRejectTimeSpan);
        printf("Starting  PRS  ......\n");
        prsinit(&tPrsCfg);
    }
}

API void mmpver()
{
    OspPrintf(TRUE, TRUE, "mmp version:%s\t",VER_MMP);
    OspPrintf(TRUE, TRUE, "compile time:%s:%s\n", __DATE__, __TIME__);
#ifdef MCUEQPAPU
    imtagtver();
#endif
#ifdef MCUEQPMMP
    mmpagtver();
#endif
    twver();
    basver();
    mixerver();
    vmpver();
    //mapver(0);
    ospver();
}

API void mmphelp()
{
    vmphelp();
}


///////////////////////////////////////////////////////////////////////////////////
//
// call CKdvMediaRcv to load it's lib
//
#include "kdvmedianet.h"
//static void TvWallLoadLib()
//{
//    CKdvMediaRcv HereInstallMediaNetLib;
//}


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

	mmpstart();

    //注册退出清除函数
	OspRegQuitFunc( OspDemoQuitFunc );
    g_bQuit = FALSE;
		
	while(!g_bQuit)
	{
	    OspDelay(1000);
	}   
    
    printf("mmp quit normally!\n");
	return 0;
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
	int* regs = NULL;
	regs = (int*)ct->uc_mcontext.regs;
	
	if (regs == NULL)
	{
		return 0;
	}
	//lint -save -e413
	int  epc = regs[32];
	int* stack = (int*)regs[1];
	int  nLink = regs[36];/* link register */
	int* presp = (int *)(*stack);
	//lint -restore

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
#endif
}


#endif // _LINUX

/////////////////////////////////////////////////////异常捕获处理
