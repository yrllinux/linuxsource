#include "osp.h"
#include "kdvtype.h"
#include "kdvsys.h"
#include "eqplib.h"
#include "mcuconst.h"
#include "mcuver.h"
#include "boardagent.h"
#include "mmp_agent.h"
#include "boardagentbasic.h"



void main(void)
{
    TAudioMixerCfg tAudioMixerCfg;
    TVmpCfg tVmpCfg, tMpwCfg;
    TEqpCfg tBasEqpCfg;
    TEqpCfg tTvWallCfg;
    TPrsCfg tPrsCfg;
	TEqpCfg tHduCfg;     //4.6

    u8 buf[MAXNUM_MAP];

	memset(&tHduCfg, 0, sizeof(tHduCfg));   //4.6
    memset(&tVmpCfg, 0, sizeof(TVmpCfg));
    memset(&tMpwCfg, 0, sizeof(tMpwCfg));

    memset(&tBasEqpCfg, 0, sizeof(tBasEqpCfg));
    memset(&tAudioMixerCfg, 0, sizeof(TAudioMixerCfg));
    memset(&tTvWallCfg, 0, sizeof(tTvWallCfg));
    memset(&tPrsCfg, 0, sizeof(TPrsCfg));

#ifdef _DEBUG
    OspInit(TRUE, 2500);
#else
    OspInit();
#endif
    //Osp telnet ≥ı º ⁄»® [11/28/2006-zbq]
    //OspTelAuthor( MCU_TEL_USRNAME, MCU_TEL_PWD );

    if (!InitMmpAgent())
    {
        printf("Init Mmp Agent Failed!\n");
        return;
    }
/*	if (!InitBrdAgent())     //4.6  jlb
	{
		printf("Init Hdu Agent Failed!\n");
        return;
    }*/

    if (g_cBrdAgentApp.IsRunVMP())
    {
        /**/
        int num = g_cBrdAgentApp.GetVMPMAPId(buf, MAXNUM_MAP);
        g_cBrdAgentApp.GetMAPInfo(buf[0],
                                  (u32*)&tVmpCfg.m_atMap[0].dwCoreSpd,
                                  (u32*)&tVmpCfg.m_atMap[0].dwMemSpd,
                                  (u32*)&tVmpCfg.m_atMap[0].dwMemSize,
                                  (u32*)&tVmpCfg.m_atMap[0].dwPort);
        tVmpCfg.wMAPCount     = num;
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
        int nNum = g_cBrdAgentApp.GetMpwMAPId(buf, MAXNUM_MAP);
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
        printf(" Starting  mpw  ......\n");
        MpwInit(tMpwCfg);
    }

    // start up audio mixer 
    if (g_cBrdAgentApp.IsRunMixer())
    {
        printf(" Starting audio mixer ......\n");
		int num = g_cBrdAgentApp.GetMixerMAPId(buf, MAXNUM_MAP);
		tAudioMixerCfg.wMAPCount = (u16)num;
		for(int i=0;i<num; i++)
		{
			tAudioMixerCfg.m_atMap[i].byMapId = buf[i];
	        g_cBrdAgentApp.GetMAPInfo(buf[i],
                                 (u32*)&tAudioMixerCfg.m_atMap[i].dwCoreSpd,
                                 (u32*)&tAudioMixerCfg.m_atMap[i].dwMemSpd,
                                 (u32*)&tAudioMixerCfg.m_atMap[i].dwMemSize,
                                 (u32*)&tAudioMixerCfg.m_atMap[i].dwPort);
		}
		tAudioMixerCfg.dwConnectIP = g_cBrdAgentApp.GetMpcIpA();
	    tAudioMixerCfg.wConnectPort = g_cBrdAgentApp.GetMpcPortA();
		tAudioMixerCfg.dwConnectIpB = g_cBrdAgentApp.GetMpcIpB();
		tAudioMixerCfg.wConnectPortB = g_cBrdAgentApp.GetMpcPortB();
    
	    tAudioMixerCfg.byEqpType = g_cBrdAgentApp.GetMixerType();
		tAudioMixerCfg.byEqpId = g_cBrdAgentApp.GetMixerId();
		tAudioMixerCfg.dwLocalIP = g_cBrdAgentApp.GetMixerIpAddr();
	    tAudioMixerCfg.wRcvStartPort = g_cBrdAgentApp.GetMixerRecvStartPort();
		tAudioMixerCfg.wMcuId = g_cBrdAgentApp.GetMcuId();
		tAudioMixerCfg.byMaxMixGroupCount = (u8)num;
	    tAudioMixerCfg.byMaxChannelInGroup = g_cBrdAgentApp.GetMixerMaxChannelInGrp();
		g_cBrdAgentApp.GetMixerAlias(tAudioMixerCfg.achAlias,MAXLEN_ALIAS );
		tAudioMixerCfg.achAlias[MAXLEN_ALIAS] = '\0';
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
        int num = g_cBrdAgentApp.GetBasMAPId(buf,MAXNUM_MAP);
        for(int i=0; i<MAXNUM_MAP; i++)
        {
            tBasEqpCfg.m_atMap[i].byMapId = buf[i];
            g_cBrdAgentApp.GetMAPInfo(buf[i],
                                      (u32*)&tBasEqpCfg.m_atMap[i].dwCoreSpd,
                                      (u32*)&tBasEqpCfg.m_atMap[i].dwMemSpd,
                                      (u32*)&tBasEqpCfg.m_atMap[i].dwMemSize,
                                      (u32*)&tBasEqpCfg.m_atMap[i].dwPort);
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
        tBasEqpCfg.wMAPCount = (u8)num;
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
        int num = g_cBrdAgentApp.GetTVWallMAPId(buf, MAXNUM_MAP);
        for(int i=0;i<num;i++)
        {
            tTvWallCfg.m_atMap[i].byMapId = buf[i];
            g_cBrdAgentApp.GetMAPInfo(buf[i],
                                      (u32*)&tTvWallCfg.m_atMap[i].dwCoreSpd,
                                      (u32*)&tTvWallCfg.m_atMap[i].dwMemSpd,
                                      (u32*)&tBasEqpCfg.m_atMap[i].dwMemSize,
                                      (u32*)&tTvWallCfg.m_atMap[i].dwPort);
        }
        tTvWallCfg.wMAPCount     = (u8)num;
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
                OspDelay( 60);
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

    while (true)
        Sleep(0xFFFFFFFF);
}

API void mmpver()
{
    OspPrintf(TRUE, TRUE, "mmp version:%s\t",VER_MMP);
    OspPrintf(TRUE, TRUE, "compile time:%s:%s\n", __DATE__, __TIME__);

    mmpagtver();
    twver();
    basver();
    mixerver();
    vmpver();
    ospver();
}

API void mmphelp()
{
    vmphelp();
}



