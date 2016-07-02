#include "apu2chnnl.h"

/*====================================================================
函数名      ：SetAudioMode
功能        ：设置音频格式
算法实现    ：
引用全局变量：
输入参数说明：PFRAMEHDR pFrmHdr
返回值说明  ：
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2013/03/29	2.0			国大卫		  创建
====================================================================*/
static void SetAudioMode(PFRAMEHDR pFrmHdr)
{
	if (pFrmHdr->m_byMediaType == MEDIA_TYPE_AACLC)
    {
        pFrmHdr->m_byAudioMode = AUDIO_MODE_AACLC_32_M;
    }
    else if (pFrmHdr->m_byMediaType == MEDIA_TYPE_AACLD)
    {
        pFrmHdr->m_byAudioMode = AUDIO_MODE_AACLD_32_M;
    }
}
/*====================================================================
函数名      : MixEncFrameCallBack
功能        ：编码回调
算法实现    ：
引用全局变量：
输入参数说明：TKdvMixerChnlInfoe*	ptChnlInfo
			  PTFrameHeader			pFrameInfo
			  void*					pContext
返回值说明  ：
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2012/02/14  1.0         周嘉麟        创建
2012/11/14	1.0			国大卫		  参数更改
====================================================================*/
static void MixEncFrameCallBack(TKdvMixerChnlInfo* ptChnlInfo, PFRAMEHDR pFrmHdr, void* pContext)
{
	PTR_NULL_VOID(ptChnlInfo);
	PTR_NULL_VOID(pContext);

	CApu2Chnnl *pcChnnl = (CApu2Chnnl*)pContext;

	SetAudioMode(pFrmHdr);

    u16 wRet = 0 ;
    CKdvMediaSnd * pcMediaSnd =  NULL;

    pcMediaSnd = pcChnnl->GetMediaSnd(u16(ptChnlInfo->dwChID), ptChnlInfo->bNMode);
	PTR_NULL_VOID(pcMediaSnd);

    wRet = pcMediaSnd->Send(pFrmHdr);
    if (wRet != MEDIANET_NO_ERROR)
    {
        LogPrint(LOG_LVL_ERROR, MID_MCU_MIXER, "[MixEncFrameCallBack] Mixer.%d Send failed because.%d ChnId.%d bDMode.%d MediaType.%d\n",
												pcChnnl->GetInsID(), wRet, 
												ptChnlInfo->dwChID, ptChnlInfo->bNMode, pFrmHdr->m_byMediaType);
    }

    return;
}

/*====================================================================
函数名      : MixVolActCallback
功能        ：激励回调
算法实现    ：
引用全局变量：
输入参数说明：TKdvMixerChnlInfo*	ptChnlInfo
			  void*					pContext
返回值说明  ：
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2012/02/14  1.0         周嘉麟        创建
2012/11/15	1.0			国大卫		  参数更改
====================================================================*/
static void MixVolActCallback(TKdvMixerChnlInfo* ptChnlInfo, void* pContext)
{
	PTR_NULL_VOID(ptChnlInfo);
	PTR_NULL_VOID(pContext);

	CApu2Chnnl *pChnl = (CApu2Chnnl*)pContext;
    if (0xffffffff == ptChnlInfo->dwChID)
    {
		LogPrint(LOG_LVL_ERROR, MID_MCU_MIXER, "[MixVolActCallback] Mixer.%d's ptChnlInfo->dwChID is 0xffffffff!\n",
												pChnl->GetInsID());
        return;
    }
	pChnl->ProcActiveMemChange((u8)ptChnlInfo->dwChID);

    return;
}

/*=============================================================================
函 数 名：SaveDataToFile
功    能：保存到文件
算法实现： 
全局变量： 
参    数： const s8* szFileName			  文件名
		   const void* pData			  保存数据
		   u32 nLen，			          pData长度
		   BOOL32 bOver，				  是否结束保存文件
		   FILE **pFile, FILE **pLenFile, 两个文件句柄，防止同时有多路码流保存

返 回 值： BOOL: TRUE,成功返回,FALSE,本次保存失败 
-----------------------------------------------------------------------------
修改记录：
日  期        版本        修改人        走读人    修改内容
2012/02/14    1.0         周嘉麟         创建
=============================================================================*/
static BOOL32 SaveDataToFile(const s8* szFileName, const void* pData, u32 nLen, BOOL32 bEnd, FILE **pFile, FILE **pLenFile)
{
    //首次打开文件
    if((NULL == *pFile) && (NULL == *pLenFile) && (NULL != szFileName))
    {
        s8 dataname[64] = {0};
        strcpy(dataname,szFileName);
        *pFile = fopen(dataname,"wb");
        if(NULL == *pFile)
        {
            printf("[SaveDataToFile]: open data file failed\n");
            return FALSE;
        }
        strcat(dataname, ".txt");
        *pLenFile = fopen(dataname,"wb");
        if(NULL == *pLenFile)
        {
            printf("[SaveDataToFile]: open datalen file failed\n");
            fclose(*pFile);
            *pFile = NULL;
            return FALSE;
        }
    }
	
    if(NULL == *pFile || NULL == *pLenFile)
    {
        printf("[SaveDataToFile]: save file(%s) error, file(%p, %p)\n", szFileName, *pFile, *pLenFile);
        if(*pFile != NULL)
        {
            fclose(*pFile);
            *pFile = NULL;
        }
        if(*pLenFile != NULL)
        {
            fclose(*pLenFile);
            *pLenFile = NULL;
        }
        return FALSE;
    }
	
    //写文件
    if (NULL != pData && nLen > 0)
    {
        //写码流
        u8* pDst = (u8*)pData;
        fwrite(pDst, nLen, 1, *pFile);
        //写长度
        fprintf(*pLenFile, "%d\n", nLen);
    }
    //是否关闭文件
    if(bEnd)
    {
        //关闭码流文件
        fclose(*pFile);
        *pFile = NULL;
        printf("[SaveDataToFile]: %s over!.\n",szFileName);
        //关闭长度文件
        fclose(*pLenFile);
        *pLenFile = NULL;
        printf("[SaveDataToFile]: %s.txt over!.\n",szFileName);
    }
    return TRUE;
}
/*====================================================================
函数名      : MixNetRcvCallBack
功能        ：接收对象回调
算法实现    ：
引用全局变量：
输入参数说明：PFRAMEHDR pFrmHdr
			  u32       dwContext
返回值说明  ：
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2012/02/14  1.0         周嘉麟          创建
====================================================================*/
extern u32 g_dwSaveNum;
extern u16 g_wChnIdx;
static void MixNetRcvCallBack(PFRAMEHDR pFrmHdr, u32 dwContext)
{
	static FILE* s_fFile = NULL;
    static FILE* s_fLen = NULL;
	CRcvCallBack *pcCallBack = (CRcvCallBack*)dwContext;
	
	SetAudioMode(pFrmHdr);

    if(g_dwSaveNum > 0 && g_wChnIdx == pcCallBack->m_wChnId)
    {
        BOOL32 bNext = (g_dwSaveNum> 1) ? TRUE : FALSE;
        BOOL32 bRet = SaveDataToFile("/ramdisk/mcudecdata", pFrmHdr->m_pData, pFrmHdr->m_dwDataSize, bNext, &s_fFile, &s_fLen);
        if(bRet)
        {
            g_dwSaveNum--;
        }
    }
	
	/*lint -save -esym(429, pcKdvMixer)*/
    CKdvMixer* pcKdvMixer = pcCallBack->m_pcMixerChn->GetKdvMixer();
	u16 wRet = pcKdvMixer->SetAudDecData(pcCallBack->m_wChnId, pFrmHdr);
    if (wRet != (u16)Codec_Success)
    {
        LogPrint(LOG_LVL_ERROR, MID_MCU_MIXER, "[MixNetRcvCallBack] Mixer.%d SetAudDecData failed because.%d ChnId.%d datasize.%d!\n",
												pcCallBack->m_pcMixerChn->GetInsID(), wRet, 
												pcCallBack->m_wChnId, pFrmHdr->m_dwDataSize);
    }

    LogPrint(LOG_LVL_DETAIL, MID_MCU_MIXER, "[MixNetRcvCallBack] Mixer.%d SetAudDecData Chnid.%d MediaType.%d AudioMode.%d datasize.%d!\n",
												pcCallBack->m_pcMixerChn->GetInsID(),
												pcCallBack->m_wChnId, pFrmHdr->m_byMediaType,
												pFrmHdr->m_byAudioMode, pFrmHdr->m_dwDataSize);
	/*lint -restore*/
}
/*====================================================================
函数名      : CApu2Chnnl
功能        ：CApu2Chnnl构造
算法实现    ：
引用全局变量：
输入参数说明：const CEqpBase* const pEqp        绑定回调外设
              CEqpCfg* const        pcCfg       绑定配置
			  const u16             wChnIdx     初始通道索引
返回值说明  ：
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2012/02/14  1.0         周嘉麟          创建
2013/01/28  1.0			国大卫			增加声道数
2013/01/30  1.0			国大卫			增加采样率
====================================================================*/
CApu2Chnnl::CApu2Chnnl(const CEqpBase* const pEqp, CEqpCfg* const pcCfg, const u16 wChnIdx)
			:CEqpChnnlBase(pEqp, new CApu2ChnStatus), 
			 m_pcAudioRcv(NULL),
			 m_pcAudioSnd(NULL),
			 m_pcRcvCallBack(NULL),
			 m_wAudRcvNum(0),
			 m_wAudSndNum(0),
			 m_byIsNeedPrs(0),
			 m_byDepth(0),
			 m_byQualityLvl(0),
			 m_pcMixerCfg((CApu2MixerCfg*)pcCfg)
{
	m_cConfId.SetNull();
	
	memset(m_abyMtId, 0, sizeof(m_abyMtId));
	InitAudChnlInfo();
	memset(&m_tMediaEncrypt, 0, sizeof(m_tMediaEncrypt));
	memset(&m_tCapSupportEx, 0, sizeof(m_tCapSupportEx));
	u8 abyLed[APU2_TEST_LEDNUM] = {11, 22, 33, 44, 55, 66};//LED_SYS_ALARM,LED_SYS_LINK
	memcpy(m_abyLed, abyLed, sizeof(m_abyLed));
	SetState((u8)CApu2ChnStatus::IDLE);

	//设置通道索引
	CEqpChnnlBase::SetChnIdx(wChnIdx);
	
	PTR_NULL_VOID(m_pcMixerCfg);

	//设置音频质量等级
	m_byQualityLvl = m_pcMixerCfg->m_byQualityLvl;

	//实例化音频接收对象
	if (NULL == m_pcAudioRcv && 0 != m_pcMixerCfg->m_byMemberNum)
	{
		m_pcAudioRcv = new CKdvMediaRcv[m_pcMixerCfg->m_byMemberNum];
		PTR_NULL_VOID(m_pcAudioRcv);
		m_wAudRcvNum = m_pcMixerCfg->m_byMemberNum;

		//创建回调对象
		if (NULL == m_pcRcvCallBack)
		{
			m_pcRcvCallBack = new CRcvCallBack[m_pcMixerCfg->m_byMemberNum];
			if (NULL == m_pcRcvCallBack)
			{
				SAFE_DELETE_ARRAY(m_pcAudioRcv);
				m_wAudRcvNum = 0;
				LogPrint(LOG_LVL_ERROR, MID_MCU_MIXER, "[CApu2Chnnl::CApu2Chnnl] m_pcRcvCallBack new failed!\n");
				return;
			}
		}
	}

	//实例化音频发送对象
	if (NULL == m_pcAudioSnd && 0 != m_pcMixerCfg->m_byMemberNum)
	{
		m_pcAudioSnd = new CKdvMediaSnd[m_pcMixerCfg->m_byMemberNum + MAXNUM_NMODECHN];
		if (NULL == m_pcAudioSnd)
		{
			SAFE_DELETE_ARRAY(m_pcAudioRcv);
			m_wAudRcvNum = 0;
			SAFE_DELETE_ARRAY(m_pcRcvCallBack);		
			LogPrint(LOG_LVL_ERROR, MID_MCU_MIXER, "[CApu2Chnnl::CApu2Chnnl] m_pcAudioSnd new failed!\n");
			return;
		}
		m_wAudSndNum = m_pcMixerCfg->m_byMemberNum + MAXNUM_NMODECHN;
	}
}


/*====================================================================
函数名      : CApu2Chnnl
功能        ：CApu2Chnnl析构
算法实现    ：
引用全局变量：
输入参数说明：
返回值说明  ：
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2012/02/14  1.0         周嘉麟          创建
====================================================================*/
/*lint -save -e1551*/
/*lint -save -e1579*/
CApu2Chnnl::~CApu2Chnnl()
{
	CApu2Chnnl::Destroy();
}
/*lint -restore*/
/*lint -restore*/
/*====================================================================
函数名      : OnMessage
功能        ：实例消息入口
算法实现    ：
引用全局变量：
输入参数说明：CMessage* const pcMsg
返回值说明  ：TRUE OR FALSE
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2012/02/14  1.0         周嘉麟          创建
====================================================================*/
BOOL32 CApu2Chnnl::OnMessage(CMessage* const pcMsg)
{
	PTR_NULL_RETURN(pcMsg, FALSE);
	if (NULL == m_pcMixerCfg)
	{
		CServMsg cServMsg(pcMsg->content, pcMsg->length);
		cServMsg.SetErrorCode(ERR_BAS_GETCFGFAILED);
		cServMsg.SetEventId(pcMsg->event + 2);
		PostMsgToMcu(cServMsg);
		return TRUE;
	}

	//[20130131 guodawei]
	LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_MIXER, 
		"MIXER.%d (CApu2Chnnl) Received msg [%u](%s).\n", GetInsID(), pcMsg->event, ::OspEventDesc(pcMsg->event));
	
	//生产测试指示灯
#ifndef _8KI_//8KI不支持点灯测试
	if (pcMsg->event >= TIMER_TEST_LED && pcMsg->event < TIMER_TEST_LED + APU2_TEST_LEDNUM)
	{		
		ProcTimerLed(pcMsg);
		return TRUE;
	}
#endif

	switch (pcMsg->event)
	{
	case MCU_MIXER_STARTMIX_REQ:
		ProcStartMix(pcMsg);	
		 break;

	case MCU_MIXER_STOPMIX_REQ:
		ProcStopMix(pcMsg);
		 break;

	case MCU_MIXER_ADDMEMBER_REQ:
		ProcAddMixMember(pcMsg);
		 break;

	case MCU_MIXER_REMOVEMEMBER_REQ:
		ProcRemoveMixMember(pcMsg);
		break;

	case MCU_MIXER_STARTVAC_CMD:
		ProcStartVac(pcMsg);
		 break;

	case MCU_MIXER_STOPVAC_CMD:
		ProcStopVac(pcMsg);
		 break;

	case MCU_MIXER_FORCEACTIVE_REQ:
		ProcForceActive(pcMsg);
		 break;

	case MCU_MIXER_CANCELFORCEACTIVE_REQ:
		ProcCancelForceActive(pcMsg);
		 break;

	case MCU_MIXER_SEND_NOTIFY:
		ProcMixSendNtf(pcMsg);
		 break;

	case MCU_MIXER_SETMIXDEPTH_REQ:	
		 break;

	case MCU_MIXER_VACKEEPTIME_CMD:
		ProcSetVacKeepTime(pcMsg);
		 break;

	case TEST_S_C_START_REG:
		ProcTestStartMix(pcMsg);
		 break;

	case TEST_S_C_STOP_REG:
		ProcTestStopMix();
		 break;
	
	case TEST_S_C_RESTORE:
		ProcTestRestore();
		 break;
#ifndef _8KI_//8KI不支持点灯测试
	case S_C_TEST_LIGHT_CMD:
		ProcTestLed();
		 break;
#endif
	default:
		break;
	}
	return TRUE;
}

/*====================================================================
函数名      : Destroy
功能        ：Apu2通道销毁
算法实现    ：
引用全局变量：
输入参数说明：
返回值说明  ：TRUE OR FALSE
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2012/02/14  1.0         周嘉麟          创建
====================================================================*/
BOOL32 CApu2Chnnl::Destroy()
{
	SAFE_DELETE_ARRAY(m_pcAudioRcv);
	SAFE_DELETE_ARRAY(m_pcAudioSnd);
	SAFE_DELETE_ARRAY(m_pcRcvCallBack);

	m_wAudRcvNum  = 0;
	m_wAudSndNum  = 0;
	m_byIsNeedPrs = 0;
	m_cConfId.SetNull();
	memset(&m_tMediaEncrypt, 0, sizeof(m_tMediaEncrypt));
	memset(&m_tCapSupportEx, 0, sizeof(m_tCapSupportEx));
	InitAudChnlInfo();
	memset(m_abyMtId, 0, sizeof(m_abyMtId));
	return TRUE;
}
/*====================================================================
函数名      : ProcStartMix
功能        ：开启混音处理
算法实现    ：
引用全局变量：
输入参数说明：CMessage* const pcMsg
返回值说明  ：
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2012/02/14  1.0         周嘉麟          创建
====================================================================*/
void CApu2Chnnl::ProcStartMix(CMessage* const pcMsg)
{
	PTR_NULL_VOID(pcMsg);
	CServMsg cServMsg(pcMsg->content, pcMsg->length);

	u16 wMsgLen = 0;
	s8 *lpMsgBody = (s8 *)cServMsg.GetMsgBody();
	if (cServMsg.GetMsgBodyLen() > wMsgLen)
	{
		TMixerStart tMixerStart = *(TMixerStart *)lpMsgBody;
		m_tMediaEncrypt  = tMixerStart.GetAudioEncrypt();
		m_byIsNeedPrs    = tMixerStart.IsNeedByPrs();
		u8 byDepth       = tMixerStart.GetMixDepth();
		m_byDepth        = min(byDepth, MIXER_MAX_MIXDEPTH);

		lpMsgBody += sizeof(TMixerStart);
		wMsgLen += sizeof(TMixerStart);
	}
	if (cServMsg.GetMsgBodyLen() > wMsgLen)
	{
		lpMsgBody += sizeof(TDoublePayload);
		wMsgLen += sizeof(TDoublePayload);
	}
	if (cServMsg.GetMsgBodyLen() > wMsgLen)
	{
		m_tCapSupportEx  = *(TCapSupportEx *)lpMsgBody;

		lpMsgBody += sizeof(TCapSupportEx);
		wMsgLen += sizeof(TCapSupportEx);
	}

	u8 byNChnlNum = 0;
	if (cServMsg.GetMsgBodyLen() > wMsgLen)
	{
		byNChnlNum = *(u8 *)lpMsgBody;
		lpMsgBody += sizeof(u8);
		wMsgLen += sizeof(u8);
	}
	TAudioCapInfo tAudChInfo;
	memset(&tAudChInfo, 0, sizeof(tAudChInfo));
	if ( byNChnlNum > 0 )
	{
		for (u8 byFormatNum = 0; byFormatNum < byNChnlNum && cServMsg.GetMsgBodyLen() > wMsgLen; byFormatNum++)
		{
			//N路编码通道
			tAudChInfo = *(TAudioCapInfo *)lpMsgBody;
			SetMixEncInfo(&tAudChInfo, m_pcMixerCfg->m_byMemberNum + byFormatNum);

			lpMsgBody += sizeof(TAudioCapInfo);
			wMsgLen += sizeof(TAudioCapInfo);
		}
	}

	//打印
	ProcMsgPrint();

	switch(GetState())
    {
    case CApu2ChnStatus::IDLE:     
		{
			LogPrint(LOG_LVL_ERROR, MID_MCU_MIXER, "MIXER: [ProcStartMix] MIXER.%d STATE-IDLE StartMix Failed!\n", GetInsID());
			cServMsg.SetErrorCode(ERR_MIXER_STATUIDLE);
			break;
		}
    case CApu2ChnStatus::MIXING:
		{
			LogPrint(LOG_LVL_ERROR, MID_MCU_MIXER, "MIXER: [ProcStartMix] MIXER.%d STATE-MIXING StartMix Failed!\n", GetInsID());
			cServMsg.SetErrorCode(ERR_MIXER_MIXING);         
            break;
		}
    case CApu2ChnStatus::READY:
        {
			/*设置全混发送对象*/
			for (u8 byIdx = 0; byIdx < byNChnlNum; byIdx++)
			{
				SetSndObjectPara(m_pcMixerCfg->m_dwMcuRcvIp, m_pcMixerCfg->m_wMcuRcvStartPort + (m_pcMixerCfg->m_byIsSimuApu? 0 :m_pcMixerCfg->m_byMemberNum + byIdx) * PORTSPAN + 2, m_pcMixerCfg->m_byMemberNum + byIdx);
			}

			if (!StartMixing())
			{
				break;
			}

			SetState((u8)CApu2ChnStatus::MIXING);
            m_cConfId = cServMsg.GetConfId();			
			cServMsg.SetEventId(pcMsg->event + 1);
            break;
        }		
    default:
        LogPrint(LOG_LVL_ERROR, MID_MCU_MIXER, "[ProcStartMix] Mixer.%d unexpected state (%d).\n", GetInsID() , GetState());
        break;
    }

	PostMsgToMcu(cServMsg);
	return;
}

/*====================================================================
函数名      : ProcStopMix
功能        ：停止混音处理
算法实现    ：
引用全局变量：
输入参数说明：CMessage* const pcMsg
返回值说明  ：
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2012/02/14  1.0         周嘉麟          创建
====================================================================*/
void CApu2Chnnl::ProcStopMix(CMessage* const pcMsg)
{
	PTR_NULL_VOID(pcMsg);
	if (FALSE == CheckConfId(pcMsg))
	{
		return;
	}

	CServMsg cServMsg(pcMsg->content, pcMsg->length);
	cServMsg.SetEventId(pcMsg->event + 2);

    switch(GetState())
    {
    case CApu2ChnStatus::IDLE:
		{	
			cServMsg.SetErrorCode(ERR_MIXER_STATUIDLE);
			LogPrint(LOG_LVL_ERROR, MID_MCU_MIXER, "[ProcStopMix] Mixer.%d's State is Idle!\n", GetInsID());
			break;
		}
    case CApu2ChnStatus::READY:
		{
			cServMsg.SetErrorCode(ERR_MIXER_NOTMIXING);
			LogPrint(LOG_LVL_ERROR, MID_MCU_MIXER,"[ProcStopMix] Mixer.%d's State is Ready!\n", GetInsID());
			break;
		}	
    case CApu2ChnStatus::MIXING:
        {
			if (!StopMixing())
			{
				LogPrint(LOG_LVL_ERROR, MID_MCU_MIXER,"[ProcStopMix] Mixer.%d's StopMixing failed!\n", GetInsID());
				break;
			}			

			m_cConfId.SetNull(); 
			InitAudChnlInfo();
			SetState((u8)CApu2ChnStatus::READY);
			cServMsg.SetEventId(pcMsg->event + 1);
		}
		break;
    default:
        LogPrint(LOG_LVL_ERROR, MID_MCU_MIXER, "[ProcStopMix] Mixer.%d unexpected State.%d\n", GetInsID() - 1 , GetState());
		break;
    }

	PostMsgToMcu(cServMsg);
	return;
}

/*====================================================================
函数名      : ProcAddMixMember
功能        ：增加混音成员处理
算法实现    ：
引用全局变量：
输入参数说明：CMessage* const pcMsg
返回值说明  ：
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2012/02/14  1.0         周嘉麟          创建
====================================================================*/
void CApu2Chnnl::ProcAddMixMember(CMessage* const pcMsg)
{
	PTR_NULL_VOID(pcMsg);
	if (FALSE == CheckConfId(pcMsg))
	{
		return;
	}
	CServMsg cServMsg(pcMsg->content, pcMsg->length);

	u8 wResMsgLen = 0;
	u8 byChnlIdx = 0;
	s8 *pbyMsgBody = ReadMsgBody(cServMsg, wResMsgLen, byChnlIdx);
	u16 wMsgLen = 0;

	//消息体的第一个u8没用了但留着兼容旧版本
	pbyMsgBody += sizeof(u8);
	wMsgLen += sizeof(u8);

    TMixMember *ptMixMmb = (TMixMember*)pbyMsgBody;
	pbyMsgBody += sizeof(TMixMember);
	wMsgLen+= sizeof(TMixMember);

	//音频类型暂时不记录
	pbyMsgBody += sizeof(u8);
	wMsgLen += sizeof(u8);

	//解码采样率
	if (wResMsgLen > wMsgLen)
	{
		m_atAudChnlInfo[byChnlIdx].bySampleRate = *(u8 *)pbyMsgBody;
		pbyMsgBody += sizeof(u8);
		wMsgLen += sizeof(u8);
	}
	TAudioCapInfo tChnlInfo;
	//解码
	if (wResMsgLen > wMsgLen)
	{
		memset(&tChnlInfo, 0, sizeof(tChnlInfo));
		tChnlInfo = *(TAudioCapInfo *)pbyMsgBody;
		SetMixDecInfo(&tChnlInfo, byChnlIdx);

		pbyMsgBody += sizeof(TAudioCapInfo); 
		wMsgLen += sizeof(TAudioCapInfo);
	}
	//编码
	if (wResMsgLen > wMsgLen)
	{
		memset(&tChnlInfo, 0, sizeof(tChnlInfo));
		tChnlInfo = *(TAudioCapInfo *)pbyMsgBody;
		SetMixEncInfo(&tChnlInfo, byChnlIdx);

		pbyMsgBody += sizeof(TAudioCapInfo); 
		wMsgLen += sizeof(TAudioCapInfo);
	}
	if (MEDIA_TYPE_NULL == m_atAudChnlInfo[byChnlIdx].byDecAudType)
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_MIXER, "[ProcAddMixMember] Mixer.%d Channel.%d's byDecAudType Is %d So AddMember Failed\n", GetInsID(), byChnlIdx, MEDIA_TYPE_NULL);
		return;
	}
	LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_MIXER, "[ProcAddMixMember] AddMixMember Channel : %d, MtId : %d!\n", byChnlIdx, ptMixMmb->m_tMember.GetMtId());

	cServMsg.SetEventId(pcMsg->event + 1);
	if (!AddMixMem(*ptMixMmb, byChnlIdx))
	{
		cServMsg.SetEventId(pcMsg->event + 2);
		LogPrint(LOG_LVL_ERROR, MID_MCU_MIXER, "[ProcAddMixMember] Mixer.%d AddMixMem failed!\n", GetInsID());
	}
    PostMsgToMcu(cServMsg);	
	return;
}

/*====================================================================
函数名      : ProcRemoveMixMember
功能        ：删除混音成员处理
算法实现    ：
引用全局变量：
输入参数说明：CMessage* const pcMsg
返回值说明  ：
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2012/02/14  1.0         周嘉麟          创建
====================================================================*/
void CApu2Chnnl::ProcRemoveMixMember(CMessage* const pcMsg)
{
	PTR_NULL_VOID(pcMsg);
	if (FALSE == CheckConfId(pcMsg))
	{
		return;
	}

	CServMsg cServMsg(pcMsg->content,pcMsg->length);

	//待移除成员是否在混音
    TMixMember *ptMixMmb;
    ptMixMmb =(TMixMember*)(cServMsg.GetMsgBody() + sizeof(u8));

	cServMsg.SetEventId(pcMsg->event + 1);
	if(NULL == ptMixMmb)
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_MIXER, "[ProcRemoveMixMember] Mixer.%d ptMixMmb is null!\n", GetInsID());
		cServMsg.SetEventId(pcMsg->event + 2);
		PostMsgToMcu(cServMsg);
		return;
	}

	if(!RemoveMixMem(ptMixMmb->m_tMember.GetMtId()))
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_MIXER, "[ProcRemoveMixMember] Mixer.%d RemoveMixMem failed!\n", GetInsID());
		cServMsg.SetEventId(pcMsg->event + 2);
	}
    PostMsgToMcu(cServMsg);
	return;
}

/*====================================================================
函数名      : ProcStartVac
功能        ：开启语音激励处理
算法实现    ：
引用全局变量：
输入参数说明：CMessage* const pcMsg
返回值说明  ：
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2012/02/14  1.0         周嘉麟          创建
====================================================================*/
void CApu2Chnnl::ProcStartVac(CMessage* const pcMsg)
{
	PTR_NULL_VOID(pcMsg);
	CServMsg cServMsg(pcMsg->content, pcMsg->length);
	u8 byState = 0;

    switch(GetState())
    {
    case CApu2ChnStatus::IDLE:
		{
			LogPrint(LOG_LVL_ERROR, MID_MCU_MIXER, "[ProcStartVac] Mixer.%d is in  IDLE state!\n", GetInsID());
			return;
		}       
    case CApu2ChnStatus::READY: 
        {
			u16 wMsgLen = 0;
			s8 *lpMsgBody = (s8 *)cServMsg.GetMsgBody();

			if (cServMsg.GetMsgBodyLen() > wMsgLen)
			{
				TMixerStart tMixerStart = *(TMixerStart *)lpMsgBody;
				m_tMediaEncrypt = tMixerStart.GetAudioEncrypt();
				m_byIsNeedPrs = tMixerStart.IsNeedByPrs();
				u8 byDepth = tMixerStart.GetMixDepth();
				m_byDepth = min(byDepth, MAXNUM_EMIXER_DEPTH);
				lpMsgBody += sizeof(TMixerStart);
				wMsgLen += sizeof(TMixerStart);
			}
			if (cServMsg.GetMsgBodyLen() > wMsgLen)
			{
				lpMsgBody += sizeof(TDoublePayload);
				wMsgLen += sizeof(TDoublePayload);
			}
			if (cServMsg.GetMsgBodyLen() > wMsgLen)
			{
				m_tCapSupportEx = *(TCapSupportEx *)lpMsgBody;
				lpMsgBody += sizeof(TCapSupportEx);
				wMsgLen += sizeof(TCapSupportEx);
			}

			u8 byNChnlNum = 0;
			if (cServMsg.GetMsgBodyLen() > wMsgLen)
			{
				byNChnlNum = *(u8 *)lpMsgBody;
				lpMsgBody += sizeof(u8);
				wMsgLen += sizeof(u8);
			}
			TAudioCapInfo tAudChInfo;
			memset(&tAudChInfo, 0, sizeof(tAudChInfo));
			if ( byNChnlNum > 0 )
			{
				for (u8 byFormatNum = 0; byFormatNum < byNChnlNum && cServMsg.GetMsgBodyLen() > wMsgLen; byFormatNum++)
				{
					//编码通道
					tAudChInfo = *(TAudioCapInfo *)lpMsgBody;
					SetMixEncInfo(&tAudChInfo, m_pcMixerCfg->m_byMemberNum + byFormatNum);

					lpMsgBody += sizeof(TAudioCapInfo);
					wMsgLen += sizeof(TAudioCapInfo);
				}
			}
			
			//打印
			ProcMsgPrint();

			byState = (u8)CApu2ChnStatus::VAC;
			break;
        }
    case CApu2ChnStatus::MIXING:  
		{
			if (FALSE == CheckConfId(pcMsg))
			{
				return;
			}
			byState = (u8)CApu2ChnStatus::MIXING_VAC;
			break;
		}
    default:
        break;
    }

	u16 wRet = m_cMixer.StartSpeechActive();
	if(wRet != (u16)Codec_Success)
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_MIXER, "[ProcStartVac] Mixer.%d StartVolActive failed because.%d!\n",GetInsID(),wRet);
		return;
	}
	LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_MIXER, "[ProcStartVac] Mixer.%d StartVolActive success!\n",GetInsID());
	m_cConfId = cServMsg.GetConfId();
	SetState(byState);

	return;
}

/*====================================================================
函数名      : ProcStopVac
功能        ：停止语音激励处理
算法实现    ：
引用全局变量：
输入参数说明：CMessage* const pcMsg
返回值说明  ：
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2012/02/14  1.0         周嘉麟          创建
====================================================================*/
void CApu2Chnnl::ProcStopVac(CMessage* const pcMsg)
{
	PTR_NULL_VOID(pcMsg);
	CServMsg cServMsg(pcMsg->content, pcMsg->length);

    switch(GetState())
    {
    case CApu2ChnStatus::IDLE:   
    case CApu2ChnStatus::READY: 
		{
			LogPrint(LOG_LVL_ERROR, MID_MCU_MIXER,"[ProcStopVac] Mixer.%d  error state.%d!\n", GetInsID(), GetState());
			return;
		}
	case CApu2ChnStatus::VAC:
	case CApu2ChnStatus::MIXING_VAC:
		{
			if (FALSE == CheckConfId(pcMsg))
			{
				return;
			}

			u16 wRet = m_cMixer.StopSpeechActive();
			if(wRet != (u16)Codec_Success)
			{
				LogPrint(LOG_LVL_ERROR, MID_MCU_MIXER, "[ProcStopVac] Mixer.%d StopVolActive failed because.%d!\n", 
														GetInsID(), wRet);
				return;
			}

			LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_MIXER, "[ProcStopVac] Mixer.%d StopVolActive successed!\n", GetInsID());
			
			if ((u8)CApu2ChnStatus::VAC == GetState())
			{
				RemoveAllMixMem();
				m_cConfId.SetNull();
				InitAudChnlInfo();
				SetState((u8)CApu2ChnStatus::READY);
			}
			else
			{
				SetState((u8)CApu2ChnStatus::MIXING);
			}	
			break;
		}
    default:
        LogPrint(LOG_LVL_ERROR, MID_MCU_MIXER, "[ProcStopVac] Mixer.%d error state.%d\n", GetInsID(), GetState());
        break;
    }
	return;
}

/*====================================================================
函数名      : ProcForceActive
功能        ：开启强制混音处理
算法实现    ：
引用全局变量：
输入参数说明：CMessage* const pcMsg
返回值说明  ：
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2012/02/14  1.0         周嘉麟          创建
====================================================================*/
void CApu2Chnnl::ProcForceActive(CMessage* const pcMsg)
{
	PTR_NULL_VOID(pcMsg);

	CServMsg cServMsg(pcMsg->content, pcMsg->length);

	TMixMember *ptMixMmb = (TMixMember*)(cServMsg.GetMsgBody() + sizeof(u8));
	u8 byChnIdx = IsMtInMix(ptMixMmb->m_tMember.GetMtId());
	if (NULL == ptMixMmb || 0xff == byChnIdx)
	{
		cServMsg.SetEventId(pcMsg->event + 2);
		PostMsgToMcu(cServMsg);
		return;
	}
	
	cServMsg.SetErrorCode(0);
	cServMsg.SetEventId(pcMsg->event + 1);
    u16 wRet = m_cMixer.SetForceChannel(byChnIdx);
    if ((u16)CODEC_NO_ERROR != wRet)
    {
        LogPrint(LOG_LVL_ERROR, MID_MCU_MIXER, "[ProcForceActive] Mixer.%d SetForceChannel Index.%d failed because.%d\n",
														GetInsID(), byChnIdx, wRet);
        cServMsg.SetErrorCode(ERR_MIXER_CALLDEVICE);
		cServMsg.SetEventId(pcMsg->event + 2);
    }

    LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_MIXER, "[ProcForceActive] Mixer.%d SetForceChannel Index.%d successed!\n",
												GetInsID(), byChnIdx);
    PostMsgToMcu(cServMsg);
	return;
}

/*====================================================================
函数名      : ProcCancelForceActive
功能        ：停止强制混音处理
算法实现    ：
引用全局变量：
输入参数说明：CMessage* const pcMsg
返回值说明  ：
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2012/02/14  1.0         周嘉麟          创建
====================================================================*/
void CApu2Chnnl::ProcCancelForceActive(CMessage* const pcMsg)
{
	PTR_NULL_VOID(pcMsg);

	CServMsg cServMsg(pcMsg->content, pcMsg->length);
	cServMsg.SetErrorCode(0);
	cServMsg.SetEventId(pcMsg->event + 1);

    u16 wRet = m_cMixer.SetForceChannel(255);
    if ((u16)CODEC_NO_ERROR != wRet)
    {
        LogPrint(LOG_LVL_ERROR, MID_MCU_MIXER, "[ProcCancelForceActive] Mixer.%d SetForceChannel failed because.%d\n", GetInsID(), wRet);
        cServMsg.SetErrorCode(ERR_MIXER_CALLDEVICE);
		cServMsg.SetEventId(pcMsg->event + 2);    
    }
    LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_MIXER, "[ProcCancelForceActive] Mixer.%d SetForceChannel successed!\n", GetInsID());
    PostMsgToMcu(cServMsg);
    return;   
}

/*====================================================================
函数名      : ProcMixSendNtf
功能        ：开始编码发送
算法实现    ：
引用全局变量：
输入参数说明：CMessage* const pcMsg
返回值说明  ：
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2012/02/14  1.0         周嘉麟          创建
====================================================================*/
void CApu2Chnnl::ProcMixSendNtf(CMessage* const pcMsg)
{
	PTR_NULL_VOID(pcMsg);

	CServMsg cServMsg(pcMsg->content, pcMsg->length);
    u8 bySend    = *(cServMsg.GetMsgBody() + sizeof(u8));
   
	LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_MIXER, "[ProcMixSendNtf] Mixer.%d Snd.%d!\n", GetInsID(), bySend);

    u16 wRet = 0;
    if ((u8)CApu2ChnStatus::VAC == GetState() && bySend)
    {
		for (u8 byLoop = 0; byLoop < MAXNUM_NMODECHN; byLoop++)
		{
			SetMediaEncParam(byLoop, TRUE);
			SetSndObjectPara(m_pcMixerCfg->m_dwMcuRcvIp, m_pcMixerCfg->m_wMcuRcvStartPort + (m_pcMixerCfg->m_byIsSimuApu? 0 :m_pcMixerCfg->m_byMemberNum + byLoop) * PORTSPAN + 2, m_pcMixerCfg->m_byMemberNum + byLoop);
		}

        wRet = m_cMixer.StartAudMix();
        if ( wRet != (u16)Codec_Success )
        {
            LogPrint(LOG_LVL_ERROR, MID_MCU_MIXER, "[ProcMixSendNtf] Mixer.%d StartAudMix failed because.%d!\n",
													   GetInsID(), wRet);
            return;
        }
        LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_MIXER, "[ProcMixSendNtf] Mixer.%d StartAudMix() successed!\n", GetInsID());
		
		SetState((u8)CApu2ChnStatus::MIXING_VAC);
	}

    if ((u8)CApu2ChnStatus::MIXING_VAC == GetState() && !bySend)
    { 
		wRet = m_cMixer.StopAudMix();
        if (wRet != (u16)Codec_Success)
        {
            LogPrint(LOG_LVL_ERROR, MID_MCU_MIXER, "[ProcMixSendNtf] Mixer.%d StopAudMix failed because.%d!\n",
													GetInsID(), wRet);
            return;
        }
		LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_MIXER,"[ProcMixSendNtf] Mixer.%d StopAudMix() successed!\n", GetInsID());

		SetState((u8)CApu2ChnStatus::VAC);
    }
    return;
}

/*====================================================================
函数名      : ProcSetVacKeepTime
功能        ：设置语音激励保护时间
算法实现    ：
引用全局变量：
输入参数说明：CMessage* const pcMsg
返回值说明  ：
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2012/02/14  1.0         周嘉麟          创建
====================================================================*/
void CApu2Chnnl::ProcSetVacKeepTime(CMessage* const pcMsg)
{
	PTR_NULL_VOID(pcMsg);

	u16 wRet = 0;
    CServMsg cServMsg(pcMsg->content,pcMsg->length);
    u32 dwKeepTime = ntohl(*(u32*)(cServMsg.GetMsgBody() + sizeof(u8)));
    wRet = m_cMixer.SetSpeechActiveKeepTime(dwKeepTime * 1000);
    if ((u16)Codec_Success != wRet)
    {
        LogPrint(LOG_LVL_ERROR, MID_MCU_MIXER, "[ProcSetVacKeepTime] Mixer.%d SetVolActKeepTime failed because.%d!\n",
												GetInsID(), dwKeepTime);
    }
    LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_MIXER,"[ProcSetVacKeepTime] Mixer.%d SetVolActKeepTime.%d successed!\n",
												GetInsID(), dwKeepTime);
	return;
}

/*====================================================================
函数名      : ProcActiveMemChange
功能        ：激励成员改变
算法实现    ：
引用全局变量：
输入参数说明：const u8 byActiveChnId 被激励通道索引
返回值说明  ：
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2012/02/14  1.0         周嘉麟          创建
====================================================================*/
void CApu2Chnnl::ProcActiveMemChange(const u8 byActiveChnId)
{
    u8 byMtId = GetMtIdFromChnId(byActiveChnId);

	if (0xff == byMtId)
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_MIXER, "[ProcActiveMemChange] byMtId is 0xff!\n");
		return;
	}
	u32 adwChnId[MAXNUM_MIXER_DEPTH] = {0};
	u32 dwChnNum = 0;
	m_cMixer.GetMixerChID(adwChnId, dwChnNum);
	
	LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_MIXER, "[ProcActiveMemChange] byMtId %d change to be ActiveMember!\n", byMtId);

	CServMsg cServMsg;
    cServMsg.SetConfId(m_cConfId);
    cServMsg.SetErrorCode(0);
    cServMsg.SetMsgBody((u8*)adwChnId, MAXNUM_MIXER_DEPTH);
    cServMsg.CatMsgBody(&byMtId, sizeof(u8));
	cServMsg.SetEventId(MIXER_MCU_ACTIVEMMBCHANGE_NOTIF);
    PostMsgToMcu(cServMsg);

	return;
}

/*====================================================================
函数名      : ProcTestStartMix
功能        ：生产测试开启混音
算法实现    ：
引用全局变量：
输入参数说明：CMessage* const pcMsg
返回值说明  ：
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2012/02/14  1.0         周嘉麟        创建
=====================================================================*/
void CApu2Chnnl::ProcTestStartMix(CMessage* const pcMsg)
{
	PTR_NULL_VOID(pcMsg);
	PTR_NULL_VOID(m_pcMixerCfg);

	if ((u8)CApu2ChnStatus::READY != GetState())
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_MIXER, "[ProcTestStartMix] Error state.%d!\n", GetState());
		return;
	}

	CServMsg cMsg;
	TEapuAutoTestMsg ptAutoTestMsg = *(TEapuAutoTestMsg*)(pcMsg->content);
	EmEAPUTestItem emTestType = ptAutoTestMsg.GetAutoTestType();
	u8 byDspID = (u8)emTestType;
	//[20130131 guodawei] 生产测试模拟音频G.711 A
	m_atAudChnlInfo[MAXNUM_MIXER_CHNNL].byEncAudType = MEDIA_TYPE_PCMA;

	//1. 根据DspId重新创建媒控对象
	u16 wRet = (u16)Codec_Success;
	TKdvMixerInitParam initParam;
	memset(&initParam, 0, sizeof(initParam));
	initParam.dwAudVersion		= 4701;
    initParam.dwAudMixDepth		= MIXER_MAX_MIXDEPTH ;
    initParam.dwAudDecMaxNum	= MAXNUM_MIXER_CHNNL;

    wRet = m_cMixer.Create(initParam);	
    if ( wRet != (u16)Codec_Success)
    {
        LogPrint(LOG_LVL_ERROR, MID_MCU_MIXER, "[ProcTestStartMix] ReCreate failed because.%d!\n", wRet);
		SendStartError(cMsg, wRet);
        return;
    }
	LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_EQP,"[ProcTestStartMix]Create Mixer Success!\n");
	//[20130131 guodawei]
	printf("[PRODUCTTEST][ProcTestStartMix] 1. Create Mixer Success!\n");

	//2. 设置媒控编码回调
    wRet = m_cMixer.SetAudEncDataCallBack(MixEncFrameCallBack, this);
    if ( wRet != (u16)Codec_Success)
    {       
        LogPrint(LOG_LVL_ERROR, MID_MCU_MIXER, "[ProcTestStartMix] SetAudEncDataCallback failed because.%d!\n", wRet);
      
		m_cMixer.Destroy();
		SendStartError(cMsg, wRet);
		return;
    }

	//第二次赋值N发送模块
	if (m_pcAudioSnd != NULL)
	{
		m_pcAudioSnd[MAXNUM_MIXER_CHNNL].Create(MAX_AUDIO_FRAME_SIZE, 64<<10, 25, m_atAudChnlInfo[MAXNUM_MIXER_CHNNL].byEncAudType);
	}

	//3. 开启混音
	if (!StartMixing())
	{
		SendStartError(cMsg);
		return;
	}
	LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_EQP,"[ProcTestStartMix]StartMixing success!\n");
	//[20130131 guodawei]
	printf("[PRODUCTTEST][ProcTestStartMix] 2. Start Mixing Success!\n");

	//4. 绑64路接收地址
	u16 wIndex = 0;
    for ( ;wIndex < MAXNUM_MIXER_CHNNL ; wIndex++ )
    {
		TLocalNetParam tlocalNetParm;
		memset(&tlocalNetParm, 0, sizeof(TLocalNetParam));
		tlocalNetParm.m_tLocalNet.m_dwRTPAddr = 0;
		tlocalNetParm.m_tLocalNet.m_wRTPPort  = EAPU_LOCALRCVPORT + wIndex * PORTSPAN + 2;
		wRet = m_pcAudioRcv[wIndex].SetNetRcvLocalParam(tlocalNetParm);
		if (MEDIANET_NO_ERROR != wRet)
		{
			LogPrint(LOG_LVL_ERROR, MID_MCU_MIXER, "[ProcTestStartMix] Index.%d SetNetRcvLocalParam <RtpPort : %d> failed because.%d!\n",
													wIndex, tlocalNetParm.m_tLocalNet.m_wRTPPort, wRet);
			SendStartError(cMsg, wRet);			
			return;
		}
		
		wRet = m_pcAudioRcv[wIndex].StopRcv();
		if (MEDIANET_NO_ERROR != wRet)
		{
			LogPrint(LOG_LVL_ERROR, MID_MCU_MIXER, "[ProcTestStartMix] Index.%d StopRcv failed because.%d!\n",
				wIndex, wRet);
			SendStartError(cMsg, wRet);	
			return;
		}
    }
	
	//绑定1路发送地址
	TNetSndParam tNetSndPar;
	memset( &tNetSndPar, 0x00, sizeof(tNetSndPar) );
	tNetSndPar.m_byNum  = 1;
	tNetSndPar.m_tLocalNet.m_wRTPPort       = EAPU_LOCALSNDPORT + 2 ;
	tNetSndPar.m_tLocalNet.m_wRTCPPort      = EAPU_LOCALSNDPORT + 1;
	tNetSndPar.m_tRemoteNet[0].m_dwRTPAddr  = htonl(m_pcMixerCfg->dwConnectIP);
	tNetSndPar.m_tRemoteNet[0].m_wRTPPort   = SERVER_RCVPORT + 2;
	wRet = m_pcAudioSnd[m_pcMixerCfg->m_byMemberNum].SetNetSndParam(tNetSndPar);
	
	if (MEDIANET_NO_ERROR != wRet)
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_MIXER, "[ProcTestStartMix] SetNetSndParam <LocalSndPort.%d, LocalRtcpPort.%d, ServerIp.%s, ServerRcvPort.%d> failed because.%d!\n",
			tNetSndPar.m_tLocalNet.m_wRTPPort,
			tNetSndPar.m_tLocalNet.m_wRTCPPort,
			StrOfIP(tNetSndPar.m_tRemoteNet[0].m_dwRTPAddr),
			tNetSndPar.m_tRemoteNet[0].m_wRTPPort, wRet);
		SendStartError(cMsg, wRet);
		return;
	}

	TKdvMixerChnlInfo tChnlInfo;
	memset(&tChnlInfo, 0, sizeof(tChnlInfo));
	TKdvMixerTypeParam tTypeParam;
	memset(&tTypeParam, 0, sizeof(tTypeParam));
	tChnlInfo.bNMode = FALSE;
	tTypeParam.dwAudType = MEDIA_TYPE_PCMA;
	tTypeParam.dwAudMode = AUDIO_MODE_PCMA;

	wIndex = 0;
	for ( wIndex = 0; wIndex < MAXNUM_MIXER_CHNNL ; wIndex++ )
    {
		//[20130131 guodawei] //第二次赋值N发送模块
		m_pcAudioSnd[wIndex].Create(MAX_AUDIO_FRAME_SIZE, 64<<10, 25, MEDIA_TYPE_PCMA);
		wRet = m_cMixer.AddChannel(wIndex);
		if(wRet != (u16)Codec_Success)
		{
			LogPrint(LOG_LVL_ERROR, MID_MCU_MIXER, "[ProcTestStartMix] AddChannel failed because.%d!", wRet);
		
			SendStartError(cMsg, wRet);
			return;
		}
		
		tChnlInfo.dwChID = wIndex;
		wRet = m_cMixer.SetAudEncParam(tChnlInfo, tTypeParam);
		if(wRet != (u16)Codec_Success)
		{
			LogPrint(LOG_LVL_ERROR, MID_MCU_MIXER, "[ProcTestStartMix] SetAudEncParam failed because.%d!", wRet);
			SendStartError(cMsg, wRet);;
			return;
		}
		m_pcAudioRcv[wIndex].StartRcv();
    }

	LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_EQP,"[ProcTestStartMix]ProcTestStartMix() success!\n");
	//[20130131 guodawei]
	printf("[PRODUCTTEST][ProcTestStartMix] 3. Add 64 Channel Success!\n");

	SetState((u8)CApu2ChnStatus::MIXING);
	cMsg.SetEventId(TEST_C_S_START_ACK);
	PostMsgToServer(cMsg);
	
	return;
}

/*====================================================================
函数名      : ProcTestStopMix
功能        ：生产测试停止混音
算法实现    ：
引用全局变量：
输入参数说明：
返回值说明  ：
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2012/02/14  1.0         周嘉麟          创建
====================================================================*/
void CApu2Chnnl::ProcTestStopMix()
{
	StopMixing();
	SetState((u8)CApu2ChnStatus::READY);
}

/*====================================================================
函数名      : ProcTestStopMix
功能        ：生产测试复位
算法实现    ：
引用全局变量：
输入参数说明：
返回值说明  ：
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2012/02/14  1.0         周嘉麟          创建
====================================================================*/
void CApu2Chnnl::ProcTestRestore()
{
#ifndef WIN32	

	BOOL32 bRet = FALSE;
    bRet = BrdClearE2promTestFlag();
	u8 bySucessRestore = bRet? 0 : 1;
	CServMsg cMsg;
	cMsg.SetMsgBody((u8*)&bySucessRestore, sizeof(bySucessRestore));
	cMsg.SetEventId(C_S_EAPURESTORE_NOTIF);
	PostMsgToServer(cMsg);
  
	OspDelay(2000);
	BrdHwReset();

#endif
    return;
}
#ifndef _8KI_//8KI不支持点灯测试
/*====================================================================
函数名      : ProcTestLed
功能        ：生产测试点灯
算法实现    ：
引用全局变量：
输入参数说明：
返回值说明  ：
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2012/02/14  1.0         周嘉麟          创建
====================================================================*/
void CApu2Chnnl::ProcTestLed()
{
	for (u8 byLoop =0; byLoop < APU2_TEST_LEDNUM ; byLoop++ )
	{
		s32 nRet = BrdLedStatusSet( m_abyLed[byLoop], BRD_LED_OFF );
		if ( OK == nRet )
		{
			LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_MIXER, "[ProcTestLed] BrdLedStatusSet .%d off success!\n", m_abyLed[byLoop]);
		}
		else
		{
			LogPrint(LOG_LVL_ERROR, MID_MCU_MIXER, "[ProcTestLed] BrdLedStatusSet .%d off failed!\n", m_abyLed[byLoop]);
		}		
	}

	u8 byBrdLedIdx = 0;
	SetTimer(TIMER_TEST_LED, TEST_LED_TIMER_INTERVAL, byBrdLedIdx);
}


/*====================================================================
函数名      : ProcTimerLed
功能        ：生产测试点灯定时处理
算法实现    ：
引用全局变量：
输入参数说明：CMessage* const pcMsg
返回值说明  ：
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2012/02/14  1.0         周嘉麟          创建
====================================================================*/
void CApu2Chnnl::ProcTimerLed(CMessage* const pcMsg)
{
	PTR_NULL_VOID(pcMsg);
	
	KillTimer(TIMER_TEST_LED); 
	
	u8 byLedIdx = *(u8*)pcMsg->content;

	if ( byLedIdx > APU2_TEST_LEDNUM )
	{
		LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_MIXER, "[ProcTimerLed] byLedIdx.%d > APU2_TEST_LEDNUM!\n", byLedIdx);	
		return;
	}
	
	//1、点灭前一个灯
	s32 nRet = 0;
	if ( byLedIdx > 0 ) 
	{
		nRet = BrdLedStatusSet( m_abyLed[byLedIdx -1 ], BRD_LED_OFF);
		if ( OK == nRet )
		{
			LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_MIXER, "[ProcTimerLed]Set Led:%d OFF success!\n", m_abyLed[byLedIdx -1]);
		}
		else
		{
			LogPrint(LOG_LVL_ERROR, MID_MCU_MIXER, "[ProcTimerLed]Set Led:%d OFF failed!\n", m_abyLed[byLedIdx -1]);
		}
	}

	if ( byLedIdx == APU2_TEST_LEDNUM  )
	{
		LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_MIXER, "[ProcTimerLed] All Leds are off now!\n");
		return;
	}

	//2. 点灯
	nRet = BrdLedStatusSet( m_abyLed[byLedIdx], BRD_LED_ON );
	if ( OK == nRet )
	{
		LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_MIXER, "[ProcTimerLed] Set Led:%d On success\n", m_abyLed[byLedIdx]);
	}
	else
	{
		LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_MIXER, "[ProcTimerLed] Set Led:%d On failed\n", m_abyLed[byLedIdx]);
	}

	//3、点下一盏灯
	byLedIdx ++;
	SetTimer(TIMER_TEST_LED, TEST_LED_TIMER_INTERVAL, byLedIdx);
	return;
}
#endif
/*====================================================================
函数名      : StartMixing
功能        ：开启混音功能接口
算法实现    ：
引用全局变量：
输入参数说明：
返回值说明  ：TRUE OR FALSE
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2012/02/14  1.0         周嘉麟          创建
====================================================================*/
BOOL32 CApu2Chnnl::StartMixing()
{
	PTR_NULL_RETURN(m_pcMixerCfg, FALSE);
	u16 wRet = (u16)Codec_Success;

	if((u8)CApu2ChnStatus::READY != GetState())
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_MIXER, "[StartMixing] Mixer.%d Error State.%d!\n", GetInsID(), GetState());
		return FALSE;
	}

	//1.开启混音
	wRet = m_cMixer.StartAudMix();
	if ( wRet != (u16)Codec_Success )
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_MIXER, 
			"[StartMixing] Mixer.%d StartAudMix Failed! because.%d\n", GetInsID(), wRet);
		return FALSE;
	} 

	//模拟版
	if (m_pcMixerCfg->m_byIsSimuApu)
	{
		//开启语音激励
		wRet = m_cMixer.StartSpeechActive();
		if ( wRet != (u16)Codec_Success )
		{
			LogPrint(LOG_LVL_ERROR, MID_MCU_MIXER, 
				"[StartMixing] Mixer.%d StartAudMix Failed! because.%d\n", GetInsID(),wRet);
			return FALSE;
		}
	}
	
	//2.设置全混编码参数
	for (u8 byLoop = 0; byLoop < MAXNUM_NMODECHN; byLoop++)
	{
		SetMediaEncParam(byLoop, TRUE);
	}

	return TRUE;
}

/*====================================================================
函数名      : StopMixing
功能        ：停止混音功能接口
算法实现    ：
引用全局变量：
输入参数说明：
返回值说明  ：TRUE OR FALSE
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2012/02/14  1.0         周嘉麟          创建
====================================================================*/
BOOL32 CApu2Chnnl::StopMixing()
{
	if ((u8)CApu2ChnStatus::MIXING != GetState())
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_MIXER, "[StopMixing] Mixer.%d Error State.%d!\n", GetInsID(), GetState());
		return FALSE;
	}

	//1.停止混音
	u16 wRet = (u16)Codec_Success;
	wRet = m_cMixer.StopAudMix();
	if (wRet != (u16)Codec_Success)
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_MIXER, "[StopMixing] Mixer.%d StopAudMix Failed because.%d!\n", GetInsID(), wRet);
		return FALSE;
	}
	
	LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_MIXER,"[StopMixing] Mixer.%d StopAudMix Successed!\n", GetInsID());
	
	//3.移除所有混音成员 
	if (!RemoveAllMixMem())
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_MIXER,"[StopMixing] Mixer.%d RemoveAllMixMem Failed!\n", GetInsID());
		return FALSE;
	}	

	return TRUE;
}

/*====================================================================
函数名      : StopVac
功能        ：停止语音激励
算法实现    ：
引用全局变量：
输入参数说明：
返回值说明  ：TRUE OR FALSE
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2012/02/14  1.0         周嘉麟          创建
====================================================================*/
BOOL32 CApu2Chnnl::StopVac()
{
	u16 wRet = m_cMixer.StopSpeechActive();
	if ( wRet != (u16)Codec_Success )
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_MIXER, "[StopVac] Mixer.%d StopVolActive failed because.%d!\n", GetInsID(), wRet);
		return FALSE;
	}
	RemoveAllMixMem();
	return TRUE;
}
/*====================================================================
函数名      : GetMtIdFromChnId
功能        ：根据通道索引获取成员ID
算法实现    ：
引用全局变量：
输入参数说明：CMessage* const pcMsg
返回值说明  ：u8    终端ID
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2012/02/14  1.0         周嘉麟          创建
====================================================================*/
u8 CApu2Chnnl::GetMtIdFromChnId(const u8 byChnId)
{
	if (byChnId >= MAXNUM_MIXER_CHNNL)
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_MIXER, "[GetMtIdFromChnId] Mixer.%d byChnId.%s is larger than MAXNUM_MIXER_CHNNL!\n",
												GetInsID(), byChnId, MAXNUM_MIXER_CHNNL);
		return 0xff;
	}

	return m_abyMtId[byChnId];
}
/*====================================================================
函数名      : SetSndObjectPara
功能        ：设置发送对象参数
算法实现    ：
引用全局变量：
输入参数说明：u32 dwDstIp     目的IP
			  u16 wDstPort    目的端口
			  u8  byChnIdx    通道索引    

返回值说明  ：TRUE OR FALSE
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2012/02/14  1.0         周嘉麟          创建
====================================================================*/
BOOL32 CApu2Chnnl::SetSndObjectPara(u32 dwDstIp, u16 wDstPort, u8 byChnIdx)
{
	PTR_NULL_RETURN(m_pcAudioSnd, FALSE);
	PTR_NULL_RETURN(m_pcMixerCfg, FALSE);

	//根据音频格式创建发送对象
	u16 wRet = 0;
    wRet = m_pcAudioSnd[byChnIdx].Create(MAX_AUDIO_FRAME_SIZE, 64<<10, 25, m_atAudChnlInfo[byChnIdx].byEncAudType);
    if(wRet != MEDIANET_NO_ERROR)
    {
		if (m_atAudChnlInfo[byChnIdx].byEncAudType == MEDIA_TYPE_NULL)
		{
			LogPrint(LOG_LVL_WARNING, MID_MCU_BAS, 
				"MIXER: [SetSndObjectPara][warning] MIXER.%d m_pcAudSnd[%d] Not Create! REASON: EncAudType--MEDIA_TYPE_NULL!\n", 
				GetInsID(), byChnIdx);
		}
		else
		{
			LogPrint(LOG_LVL_ERROR, MID_MCU_MIXER, 
				"MIXER: [SetSndObjectPara][error] MIXER.%d Created m_pcAudSnd[%d] of AudType:[%d] Failed! REASON: %d!\n", 
				GetInsID(), byChnIdx, m_atAudChnlInfo[byChnIdx].byEncAudType, wRet);
		}
		return FALSE;
    }

    LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_MIXER, 
		"MIXER: [SetSndObjectPara] MIXER.%d Created m_pcAudSnd[%d] of AudType:[%d] Successed.\n", 
		GetInsID(), byChnIdx, m_atAudChnlInfo[byChnIdx].byEncAudType);
	
    
    TNetSndParam tNetSndPar;
    memset( &tNetSndPar, 0, sizeof(tNetSndPar));
    tNetSndPar.m_byNum  = 1;
#ifdef _8KI_
	tNetSndPar.m_tLocalNet.m_wRTPPort = m_pcMixerCfg->wRcvStartPort + byChnIdx * PORTSPAN + 4;
	tNetSndPar.m_tLocalNet.m_wRTCPPort= m_pcMixerCfg->wRcvStartPort + byChnIdx * PORTSPAN + 5;
#else
	//本地RTP及RTCP发送端口
	tNetSndPar.m_tLocalNet.m_wRTPPort       = BRD_APU2_SNDPORT + (m_pcMixerCfg->m_bySndOff +byChnIdx)* PORTSPAN + 2;
    tNetSndPar.m_tLocalNet.m_wRTCPPort      = BRD_APU2_SNDPORT + (m_pcMixerCfg->m_bySndOff +byChnIdx)* PORTSPAN + 2 + 1;
#endif
	//远端RTP地址及端口
    tNetSndPar.m_tRemoteNet[0].m_dwRTPAddr  = dwDstIp;
    tNetSndPar.m_tRemoteNet[0].m_wRTPPort   = wDstPort;

    m_pcAudioSnd[byChnIdx].SetNetSndParam(tNetSndPar);

	//设置发送重传时间间隔
    wRet = m_pcAudioSnd[byChnIdx].ResetRSFlag( 2000, TRUE );
    if(wRet != MEDIANET_NO_ERROR)
    {
        LogPrint(LOG_LVL_ERROR, MID_MCU_MIXER, "ERROR: [SetSndObjectPara] MIXER.%d ResetRSFlag Failed! REASON: %d!\n", GetInsID(), wRet);
    }
    else
    {
        LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_MIXER, "SUCCESS: [SetSndObjectPara] MIXER.%d ResetRSFlag Successed.\n", GetInsID());
    }

	//设置冗余发送参数
	SetAudResend(byChnIdx);

    u8  abyKeyBuf[MAXLEN_KEY];
    memset(abyKeyBuf, 0, MAXLEN_KEY );
    s32 nKeyLen = 0;
    u8 byEncryptMode = m_tMediaEncrypt.GetEncryptMode();
    if ( CONF_ENCRYPTMODE_NONE == byEncryptMode )
    {
		m_pcAudioSnd[byChnIdx].SetEncryptKey( NULL, 0, 0 );
        LogPrint(LOG_LVL_DETAIL, MID_MCU_MIXER,  "[SetSndObjectPara] MIXER.%d set key to NULL!\n", GetInsID());      
    }
    else if ( CONF_ENCRYPTMODE_DES == byEncryptMode || CONF_ENCRYPTMODE_AES == byEncryptMode )
    {
        if ( CONF_ENCRYPTMODE_DES == byEncryptMode )
        {
            byEncryptMode = DES_ENCRYPT_MODE;
        }
        else
        {
            byEncryptMode = AES_ENCRYPT_MODE;
        }
        m_tMediaEncrypt.GetEncryptKey( abyKeyBuf, &nKeyLen );
		
        m_pcAudioSnd[byChnIdx].SetEncryptKey( (s8*)abyKeyBuf, (u16)nKeyLen, byEncryptMode );
    }
    m_pcAudioSnd[byChnIdx].SetActivePT(m_atAudChnlInfo[byChnIdx].tEncPayload.GetRealPayLoad());
 	
    LogPrint(LOG_LVL_DETAIL, MID_MCU_MIXER,  "[SetSndObjectPara] Mode: %u KeyLen: %u PT: %u \n",
												m_tMediaEncrypt.GetEncryptMode(), 
												nKeyLen, m_atAudChnlInfo[byChnIdx].tEncPayload.GetRealPayLoad());

	return TRUE;
}

/*====================================================================
函数名      ：SetRcvObjectPara
功能        ：设置接收对象参数
算法实现    ：
引用全局变量：
输入参数说明：const u8 byChnIdx 通道索引
返回值说明  ：TRUE OR FALSE
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2012/02/14  1.0         周嘉麟          创建
====================================================================*/
BOOL32 CApu2Chnnl::SetRcvObjectPara(const u8 byChnIdx)
{
	PTR_NULL_RETURN(m_pcAudioRcv, FALSE);
	PTR_NULL_RETURN(m_pcMixerCfg, FALSE);

    u16 wRet = 0;
	s32 byKenLen = 0;

    u8 abyKeyBuf[MAXLEN_KEY];  
    memset(abyKeyBuf, 0, MAXLEN_KEY);

    u8 byEncryptMode = m_tMediaEncrypt.GetEncryptMode();
    if ( CONF_ENCRYPTMODE_NONE == byEncryptMode )
    {
        m_pcAudioRcv[byChnIdx].SetDecryptKey( NULL, 0, 0 );
		LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_MIXER, "[SetRcvObjectPara] Mixer.%d Index.%d Set key to NULL!\n", GetInsID(), byChnIdx);
    }
    else if ( CONF_ENCRYPTMODE_DES == byEncryptMode || CONF_ENCRYPTMODE_AES == byEncryptMode )
    {
        if ( CONF_ENCRYPTMODE_DES == byEncryptMode )
        {
            byEncryptMode = DES_ENCRYPT_MODE;
        }
        else
        {
            byEncryptMode = AES_ENCRYPT_MODE;
        }
        m_tMediaEncrypt.GetEncryptKey(abyKeyBuf, &byKenLen);
        m_pcAudioRcv[byChnIdx].SetDecryptKey( (s8*)abyKeyBuf, (u16)byKenLen, byEncryptMode );
    }

    u8 byRealPayload   = m_atAudChnlInfo[byChnIdx].tDecPayload.GetRealPayLoad();
    u8 byActivePayload = m_atAudChnlInfo[byChnIdx].tDecPayload.GetActivePayload();
    m_pcAudioRcv[byChnIdx].SetActivePT( byActivePayload, byRealPayload );

    LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_MIXER, "[SetRcvObjectPara] Mixer.%d Index.%d Mode: %u KeyLen: %u PT: %d AT: %d KeyBuf [%s]\n",
												GetInsID(), byChnIdx,
												m_tMediaEncrypt.GetEncryptMode(), 
												byKenLen, 
												byRealPayload ,byActivePayload,abyKeyBuf);

	TRSParam tNetRSParam ;
    if (m_byIsNeedPrs)
    {
        tNetRSParam.m_wFirstTimeSpan  = m_pcMixerCfg->m_tPrsTimeSpan.m_wFirstTimeSpan;
        tNetRSParam.m_wSecondTimeSpan = m_pcMixerCfg->m_tPrsTimeSpan.m_wSecondTimeSpan;
        tNetRSParam.m_wThirdTimeSpan  = m_pcMixerCfg->m_tPrsTimeSpan.m_wThirdTimeSpan;
        tNetRSParam.m_wRejectTimeSpan = m_pcMixerCfg->m_tPrsTimeSpan.m_wRejectTimeSpan;
    }
    else
    {
        TRSParam tNetRSParam ;
        tNetRSParam.m_wFirstTimeSpan  = 0;
        tNetRSParam.m_wSecondTimeSpan = 0;
        tNetRSParam.m_wThirdTimeSpan  = 0;
        tNetRSParam.m_wRejectTimeSpan = 0;
	}
	LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_MIXER, "[SetRcvObjectPara]Mixer.%d Index.%d m_wFirstTimeSpan:%d, m_wSecondTimeSpan:%d, m_wThirdTimeSpan:%d, m_wRejectTimeSpan:%d!\n",
		GetInsID(), byChnIdx, tNetRSParam.m_wFirstTimeSpan, tNetRSParam.m_wSecondTimeSpan, tNetRSParam.m_wThirdTimeSpan, tNetRSParam.m_wRejectTimeSpan);

	//设置接收重传时间间隔
    wRet = m_pcAudioRcv[byChnIdx].ResetRSFlag(tNetRSParam, TRUE);
    if(wRet != MEDIANET_NO_ERROR)
    {
		LogPrint(LOG_LVL_ERROR, MID_MCU_MIXER,"[SetRcvObjectPara]Mixer.%d Index.%d ResetRSFlag Failed because.%d!\n", GetInsID(), byChnIdx, wRet);
	}
	else
	{
		LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_MIXER,"[SetRcvObjectPara]Mixer.%d Index.%d ResetRSFlag Successed!\n", GetInsID(), byChnIdx);
	}

    wRet = m_pcAudioRcv[byChnIdx].StartRcv();
    if (MEDIANET_NO_ERROR != wRet)
    {
        LogPrint(LOG_LVL_ERROR, MID_MCU_MIXER,"[SetRcvObjectPara]Mixer.%d Index.%d StartRcv failed because.%d!\n", GetInsID(), byChnIdx, wRet);
    }
    else
    {
        LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_MIXER,"[SetRcvObjectPara]Mixer.%d Index.%d StartRcv() Successed!!\n", GetInsID(), byChnIdx);
    }
    return TRUE;
}
/*====================================================================
函数名        RemoveAllMixMem
功能        ：移除所有混音成员
算法实现    ：
引用全局变量：
输入参数说明：
返回值说明  ：TRUE OR FALSE
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2012/02/14  1.0         周嘉麟          创建
====================================================================*/
BOOL32 CApu2Chnnl::RemoveAllMixMem()
{
	PTR_NULL_RETURN(m_pcMixerCfg, FALSE);

    for (u16 wIndex = 0; wIndex < m_pcMixerCfg->m_byMemberNum ; wIndex++)
    {
		RemoveMixMem(m_abyMtId[wIndex]); 
    }

    return TRUE;
}

/*====================================================================
函数名      ：GetAudioMode
功能        ：获取音频模式
算法实现    ：
引用全局变量：
输入参数说明：const u8 byAudioType 音频类型
返回值说明  ：u8
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2012/02/14  1.0         周嘉麟          创建
====================================================================*/
u8 CApu2Chnnl::GetAudioMode(const u8 byAudioType, u8 byAudioTrackNum)
{
    u8 byAudMode = 0;
    switch (byAudioType)
    {
    case MEDIA_TYPE_G7221C://G.722.1.C Polycom Siren14
        byAudMode = AUDIO_MODE_G7221;//采样率
        break;
    case MEDIA_TYPE_PCMA://G.711a
        byAudMode = AUDIO_MODE_PCMA;
        break;
    case MEDIA_TYPE_PCMU://G.711u
        byAudMode = AUDIO_MODE_PCMU;
        break;
    case MEDIA_TYPE_G722://G.722
        byAudMode = AUDIO_MODE_G722;
        break;
    case MEDIA_TYPE_G728://G.728
        byAudMode = AUDIO_MODE_G728;
        break;
    case MEDIA_TYPE_G729://G.729
        byAudMode = AUDIO_MODE_G729;
        break;
    case MEDIA_TYPE_G719://G.719
        byAudMode = AUDIO_MODE_G719;
        break;
    case MEDIA_TYPE_MP3://MP3
        byAudMode = AUDIO_MODE_MP3;
        break;
    case MEDIA_TYPE_AACLC://MPEG4 AAL-LC
		if (1 == byAudioTrackNum)
		{
			byAudMode = AUDIO_MODE_AACLC_32_M;
		}
        else if (2 == byAudioTrackNum)
		{
			byAudMode = AUDIO_MODE_AACLC_32;
		}
        break;
    case MEDIA_TYPE_AACLD://MPEG4 AAL-LD
		if (1 == byAudioTrackNum)
		{
			byAudMode = AUDIO_MODE_AACLD_32_M;
		}
		else if (2 == byAudioTrackNum)
		{
			byAudMode = AUDIO_MODE_AACLD_32;
		}
        break;
    default:
        LogPrint(LOG_LVL_ERROR, MID_MCU_MIXER, "GetAudioMode MediaType.%d not surportted!!\n", byAudioType);
        return 0;
    }
    return byAudMode;
}

/*====================================================================
函数名      ：AddMixMem
功能        ：增加混音成员
算法实现    ：
引用全局变量：
输入参数说明：const TMixMember tMixMember 混音成员
返回值说明  ：TRUE OR FALSE
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2012/02/14  1.0         周嘉麟          创建
====================================================================*/
BOOL32 CApu2Chnnl::AddMixMem(TMixMember tMixMember, u8 byChIdx)
{
	PTR_NULL_RETURN(m_pcMixerCfg, FALSE);

	//通道索引号由MCU主控侧决定
	m_abyMtId[byChIdx] = tMixMember.m_tMember.GetMtId();

	LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_MIXER, "[AddMixMem] Mixer.%d Get  RemoteAddr: %s  RemotePort: %d\n", GetInsID(), 
		StrOfIP(tMixMember.m_tAddr.GetIpAddr()), tMixMember.m_tAddr.GetPort());

	//2. 设置发送对象地址
	SetSndObjectPara(tMixMember.m_tAddr.GetIpAddr(), tMixMember.m_tAddr.GetPort(), byChIdx);
	
	//3. 设置接收对象地址
	SetLocNetParam(&tMixMember, byChIdx);
	
	//4. 添加混音通道
	u16 wRet = m_cMixer.AddChannel(byChIdx);
    if(wRet != (u16)Codec_Success)
    {
        LogPrint(LOG_LVL_ERROR, MID_MCU_MIXER, "[AddMixMem] Mixer.%d AddChannel.%d failed because.%d\n",GetInsID(), byChIdx, wRet);
        return FALSE;
    }
	LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_MIXER, "[AddMixMem] Mixer.%d AddChannel.%d success!\n", GetInsID(), byChIdx);
		
	//5. 设置通道编码参数
	SetMediaEncParam(byChIdx);

	//6. 设置通道解码参数(针对AACLC,AACLD)
	SetMediaDecParam(byChIdx);

	//7. 设置接收对象参数
    SetRcvObjectPara(byChIdx);
	
	LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_MIXER,"-------------------------------------------------------\n");
	return TRUE;
}

/*====================================================================
函数名        RemoveMixMem
功能        ：移除混音成员
算法实现    ：
引用全局变量：
输入参数说明：const u8 byMtId 终端ID
返回值说明  ：TRUE OR FALSE
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2012/02/14  1.0         周嘉麟          创建
====================================================================*/
BOOL32 CApu2Chnnl::RemoveMixMem(const u8 byMtId)
{	
	//1.是否是混音成员
	u8 byChnIdx = IsMtInMix(byMtId);
	if (0xff == byChnIdx)
	{
		return FALSE;
	}

	PTR_NULL_RETURN(m_pcAudioRcv, FALSE);
	PTR_NULL_RETURN(m_pcAudioSnd, FALSE);
	
	//2.停止接收
    u16 wRet = (u16)Codec_Success; 
	wRet = m_pcAudioRcv[byChnIdx].StopRcv();
    if(wRet != MEDIANET_NO_ERROR)
    {
        LogPrint(LOG_LVL_ERROR, MID_MCU_MIXER, "[RemoveMixMem] Mixer.%d StopRcv Index.%d failed because.%d\n", 
												GetInsID(), byChnIdx, wRet);
        return FALSE;
    }
	
	LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_MIXER, "[RemoveMixMem] Mixer.%d StopRcv Index.%d successed!\n",					
												GetInsID(), byChnIdx);
    
	//3.删除通道
	wRet = m_cMixer.DelChannel(byChnIdx);
    if ((u16)Codec_Success != wRet)
    {
        LogPrint(LOG_LVL_ERROR, MID_MCU_MIXER, "[RemoveMixMem] Mixer.%d DelChannel Index.%d failed because.%d!\n", 
												GetInsID(), byChnIdx, wRet);
        return FALSE;
    }
	
    LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_MIXER, "[RemoveMixMem] Mixer.%d Call DelChannel Index.%d successed()!\n", 
												GetInsID(), byChnIdx);
	
	//4. 移除发送参数
	wRet = m_pcAudioSnd[byChnIdx].RemoveNetSndLocalParam();
    if (MEDIANET_NO_ERROR != wRet)
    {
        LogPrint(LOG_LVL_ERROR, MID_MCU_MIXER, "[RemoveMixMem] Mixer.%d RemoveNetSndLocalParam Index.%d failed because.%d!\n",
												GetInsID() , byChnIdx, wRet);
        return FALSE;
    }
	
    LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_MIXER,"[RemoveMixMem] Mixer.%d RemoveNetSndLocalParam Index.%d Successed!\n",
												GetInsID() ,byChnIdx);

	//5. 移除成员
	for (u8 byIdx = 0; byIdx < MAXNUM_MIXER_CHNNL; byIdx++)
	{
		if (0 != m_abyMtId[byIdx] && byMtId == m_abyMtId[byIdx])
		{
			m_abyMtId[byIdx] = 0;
			return TRUE;
		}
	}
	return FALSE;
}
/*====================================================================
函数名      ：IsMtInMix
功能        ：终端是否在混音
算法实现    ：
引用全局变量：
输入参数说明：u8 byMtId	终端ID
返回值说明  ：u8        终端所在通道索引
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2012/02/14  1.0         周嘉麟          创建
====================================================================*/
u8 CApu2Chnnl::IsMtInMix(const u8 byMtId)
{
	if (0 == byMtId)
	{
		return 0xff;
	}

	u8 byChnIdx = 0xff;
	for (u8 byIdx = 0; byIdx < MAXNUM_MIXER_CHNNL; byIdx++)
	{
		if (0 != m_abyMtId[byIdx] && byMtId == m_abyMtId[byIdx])
		{
			byChnIdx = byIdx;
			break;
		}
	}

	return byChnIdx;
}

/*====================================================================
函数名      ：Stop
功能        ：停止通道工作
算法实现    ：
引用全局变量：
输入参数说明：
返回值说明  ：TRUE OR FALSE
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2012/02/14  1.0         周嘉麟          创建
====================================================================*/
BOOL32 CApu2Chnnl::Stop(const u8 byMode)
{
	if (MODE_BOTH != byMode && MODE_AUDIO != byMode)
	{
		return FALSE;
	}

	//停混音
	StopMixing();

	//停语音激励
	StopVac();

	SetState((u8)CApu2ChnStatus::IDLE);
	InitAudChnlInfo();
    m_cConfId.SetNull();
	return TRUE;
}

/*====================================================================
函数名      ：GetMediaSnd
功能        ：获取发送对象
算法实现    ：
引用全局变量：
输入参数说明：const u16 wIndex 通道索引
返回值说明  ：CKdvMediaSnd*
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2012/02/14  1.0         周嘉麟          创建
====================================================================*/
CKdvMediaSnd* CApu2Chnnl::GetMediaSnd(const u16 wIndex, BOOL32 bDMode)
{
	if (wIndex >= m_wAudSndNum)
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_MIXER, "[GetMediaSnd] Mixer.%d wIndex.%d >= m_wAudSndNum.%d!\n",
												GetInsID(), wIndex, m_wAudSndNum);
		return NULL;
	}

	PTR_NULL_RETURN(m_pcMixerCfg, NULL);
	PTR_NULL_RETURN(m_pcAudioSnd, NULL);

	if (bDMode)
	{
		return &m_pcAudioSnd[m_pcMixerCfg->m_byMemberNum + wIndex];
	}
	return &m_pcAudioSnd[wIndex];
}

/*====================================================================
函数名          GetKdvMixer
功能        ：获取媒体控制对象
算法实现    ：
引用全局变量：
输入参数说明：
返回值说明  ：CKdvMixer*
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2012/02/14  1.0         周嘉麟          创建
====================================================================*/
CKdvMixer* CApu2Chnnl::GetKdvMixer() 
{
	/*lint -save -e1536*/
	return &m_cMixer;
	/*lint -restore*/
}

/*====================================================================
函数名      ：TransAudType
功能        ：转换音频类型
算法实现    ：
引用全局变量：
输入参数说明：
返回值说明  ：CKdvMixer*
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2013/03/02  2.0         国大卫         创建
====================================================================*/
u8 CApu2Chnnl::TransSimRate(const u8 bySimpleRate)
{
	u8 byTransRate = 0;
	switch (bySimpleRate)
	{
		case AAC_SAMPLE_FRQ_8:
			byTransRate = SAMPLE_RATE8000;
			break;
		case AAC_SAMPLE_FRQ_11:
			byTransRate = SAMPLE_RATE11025;
			break;
		case AAC_SAMPLE_FRQ_12:
			byTransRate = SAMPLE_RATE12000;
			break;
		case AAC_SAMPLE_FRQ_16:
			byTransRate = SAMPLE_RATE16000;
			break;
		case AAC_SAMPLE_FRQ_22:
			byTransRate = SAMPLE_RATE22050;
			break;
		case AAC_SAMPLE_FRQ_24:
			byTransRate = SAMPLE_RATE24000;
			break;
		case AAC_SAMPLE_FRQ_32:
			byTransRate = SAMPLE_RATE32000;
			break;
		case AAC_SAMPLE_FRQ_44:
			byTransRate = SAMPLE_RATE44100;
			break;
		case AAC_SAMPLE_FRQ_48:
			byTransRate = SAMPLE_RATE48000;
			break;
		case AAC_SAMPLE_FRQ_64:
			byTransRate = SAMPLE_RATE64000;
			break;
		case AAC_SAMPLE_FRQ_88:
			byTransRate = SAMPLE_RATE88200;
			break;
		case AAC_SAMPLE_FRQ_96:
			byTransRate = SAMPLE_RATE96000;
			break;
		default:
			byTransRate = 0;
			break;
	}
	return byTransRate;
}

/*====================================================================
函数名      : Init
功能        ：Apu2通道初始化
算法实现    ：
引用全局变量：
输入参数说明：
返回值说明  ：TRUE OR FALSE
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2012/02/14  1.0         周嘉麟          创建
====================================================================*/
BOOL32 CApu2Chnnl::Init()
{
	//通道索引
	u16 wChnIdx = CEqpChnnlBase::GetChnIdx();
	if (INVALID_CHNLIDX == wChnIdx)
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_MIXER, "[CApu2Chnnl::Init] GetChnIdx failed!\n");
		return FALSE;
	}

	PTR_NULL_RETURN(m_pcMixerCfg, FALSE);
	PTR_NULL_RETURN(m_pcAudioRcv, FALSE);
	PTR_NULL_RETURN(m_pcAudioSnd, FALSE);

	TKdvMixerInitParam tInitParam;
	memset(&tInitParam, 0, sizeof(tInitParam));

    tInitParam.dwAudVersion		= 4701;
    tInitParam.dwAudMixDepth	= MIXER_MAX_MIXDEPTH ;
    tInitParam.dwAudDecMaxNum	= m_pcMixerCfg->m_byMemberNum;

	//创建混音器对象
	u16 wRet = m_cMixer.Create(tInitParam);
	if (wRet != (u16)Codec_Success)
    {
		printf("ERROR: [Init] MIXER.%d Created Failed! REASON:%d!\n", GetInsID(), wRet);
        LogPrint(LOG_LVL_ERROR, MID_MCU_MIXER, "ERROR: [Init] MIXER.%d Created Failed! REASON:%d!\n", GetInsID(), wRet);
        return FALSE;
    }
	printf("SUCCESS: [Init] MIXER.%d Created Success!\n", GetInsID());
	LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_MIXER, "SUCCESS: [Init] MIXER.%d Created Success!\n", GetInsID());


	//编码回调
	wRet = m_cMixer.SetAudEncDataCallBack(MixEncFrameCallBack, this);
    if (wRet != (u16)Codec_Success)
    {
        m_cMixer.Destroy();
		printf("ERROR: [Init] MIXER.%d SetAudEncDataCallBack Failed! REASON:%d!\n", GetInsID(), wRet);
        LogPrint(LOG_LVL_ERROR, MID_MCU_MIXER, "ERROR: [Init] MIXER.%d SetAudEncDataCallBack Failed! REASON:%d!\n", GetInsID(), wRet);
        return FALSE;
    }
	printf("SUCCESS: [Init] MIXER.%d SetAudEncDataCallBack Success!\n", GetInsID());
	LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_MIXER, "SUCCESS: [Init] MIXER.%d SetAudEncDataCallBack Success!\n", GetInsID());


	//激励回调
	wRet = m_cMixer.SetSpeechActiveCallBack(MixVolActCallback, this);
    if(wRet != (u16)Codec_Success)
    {
        m_cMixer.Destroy();
		printf("ERROR: [Init] MIXER.%d SetSpeechActiveCallBack Failed! REASON:%d!\n", GetInsID(), wRet);
		LogPrint(LOG_LVL_ERROR, MID_MCU_MIXER, "ERROR: [Init] MIXER.%d SetSpeechActiveCallBack Failed! REASON:%d!\n", GetInsID(), wRet);
        return FALSE;
    }
	printf("SUCCESS: [Init] MIXER.%d SetSpeechActiveCallBack Success!\n", GetInsID());
	LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_MIXER, "SUCCESS: [Init] MIXER.%d SetSpeechActiveCallBack Success!\n", GetInsID());


	//创建收发N-1对象
	u8 byIdx = 0;
	for ( ; byIdx < m_wAudRcvNum; byIdx++)
	{
		if (NULL != m_pcAudioRcv && NULL != m_pcRcvCallBack)
		{
			m_pcRcvCallBack[byIdx].m_pcMixerChn = this;
			m_pcRcvCallBack[byIdx].m_wChnId     = byIdx;
			wRet = m_pcAudioRcv[byIdx].Create(MAX_AUDIO_FRAME_SIZE, MixNetRcvCallBack, (u32)&m_pcRcvCallBack[byIdx]);
			if(wRet != MEDIANET_NO_ERROR)
			{
				printf("ERROR: [Init] m_pcAudioRcv.[%d] Created Failed! REASON:%d!\n", byIdx, wRet);
				LogPrint(LOG_LVL_ERROR, MID_MCU_MIXER, "ERROR: [Init] m_pcAudioRcv.[%d] Created Failed! REASON:%d!\n", byIdx, wRet);
				return FALSE;
			}

			if (NULL != m_pcAudioSnd)
			{
				wRet = m_pcAudioSnd[byIdx].Create(MAX_AUDIO_FRAME_SIZE, 64<<10, 25, MEDIA_TYPE_G7221C);
				if(wRet != MEDIANET_NO_ERROR)
				{
					printf("ERROR: [Init] m_pcAudioSnd.[%d] Created Failed! REASON:%d\n", byIdx, wRet);
					LogPrint(LOG_LVL_ERROR, MID_MCU_MIXER, "ERROR: [Init] m_pcAudioSnd.[%d] Created Failed! REASON:%d\n", byIdx, wRet);
					return FALSE;
				}
			}
		}

		/*设置冗余参数*/
		if (NULL != m_pcAudioSnd)
		{
			SetAudResend(byIdx);
		}
	
		if(wRet != MEDIANET_NO_ERROR)
        {
			printf("[Init]m_pcAudioSnd[byIdx].SetAudioResend() failed!\n");
            LogPrint(LOG_LVL_ERROR, MID_MCU_MIXER, "[CApu2Chnnl::Init] m_pcAudioSnd[%d] set N-1 mode AudioResend failed because .%d!\n", byIdx, wRet);
            return FALSE;
        }
	}

	//创建4路全混发送对象
	if (NULL != m_pcAudioSnd)
	{
		for (u8 byLoop = 0; byLoop < MAXNUM_NMODECHN; byLoop++)
		{
			wRet = m_pcAudioSnd[m_wAudRcvNum + byLoop].Create(MAX_AUDIO_FRAME_SIZE, 64<<10, 25, MEDIA_TYPE_G7221C);
			if(wRet != MEDIANET_NO_ERROR)
			{
				printf("m_pcAudioSnd[m_wAudRcvNum].Create() failed!\n");
				LogPrint(LOG_LVL_ERROR, MID_MCU_MIXER, "[CApu2Chnnl::Init] m_pcAudioSnd[%d] create N failed because .%d!\n",byIdx, wRet);
				return FALSE;
			}
			
			/*设置冗余参数*/
			SetAudResend(m_wAudRcvNum + byLoop);
		}
	}

    return TRUE;
}

/*====================================================================
函数名      : Print
功能        ：Apu2通道打印
算法实现    ：
引用全局变量：
输入参数说明：
返回值说明  ：
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2012/02/14  1.0         周嘉麟          创建
====================================================================*/
void CApu2Chnnl::Print()
{
	StaticLog("\nMixer : %d", GetInsID());

	//State
	switch (GetState())
	{
	case CApu2ChnStatus::IDLE:
		StaticLog("\n\t State : %s", "IDLE");
		break;
	case CApu2ChnStatus::READY:
		StaticLog("\n\t State : %s", "READY");
		break;
	case CApu2ChnStatus::MIXING:
		StaticLog("\n\t State : %s", "MIXING");
		break;
	case CApu2ChnStatus::VAC:
		StaticLog("\n\t State : %s", "VAC");
		break;
	case CApu2ChnStatus::MIXING_VAC:
		StaticLog("\n\t State : %s", "MIXING_VAC");
		break;
	default:
		StaticLog("\n\t State : %s.%d", "unexpected State", GetState());
		break;
	}
	
	//m_wAudRcvNum && m_wAudSndNum
	StaticLog("\n\t m_wAudRcvNum : %d", m_wAudRcvNum);
	StaticLog("\n\t m_wAudSndNum : %d", m_wAudSndNum);
	
	//m_tMediaEncrypt
	u8  abyKeyBuf[MAXLEN_KEY] = {0};
	s32 nKeyLen = 0;
	m_tMediaEncrypt.GetEncryptKey(abyKeyBuf, &nKeyLen);	
	StaticLog("\n\t m_tMediaEncrypt: \n\t\t Mode	: %u\n\t\t Key	: %s", 
											m_tMediaEncrypt.GetEncryptMode(), abyKeyBuf);

	StaticLog("\n\t m_abyAudType :");
	u8 byIdx = 0;
	for (; byIdx < MAXNUM_NMODECHN; byIdx++)
	{
		StaticLog("\n\t\t %d. %d", byIdx, m_atAudChnlInfo[m_pcMixerCfg->m_byMemberNum + byIdx].byEncAudType);
	} 
	StaticLog("\n\t m_tCapSupportEx: \n\t\t VideoFECType	: %d\n\t\t AudioFECType	: %d\n\t\t DVideoFECType	: %d \
									   \n\t\t TVideoStreamCap: \n\t\t\t MediaType	: %d\n\t\t\t\t Res	: %d\n\t\t\t\t FR	: %d\n\t\t\t\t BR	: %d",
										m_tCapSupportEx.GetVideoFECType(), m_tCapSupportEx.GetAudioFECType(),
										m_tCapSupportEx.GetDVideoFECType(), m_tCapSupportEx.GetSecDSType(),
										m_tCapSupportEx.GetSecDSRes(), m_tCapSupportEx.GetSecDSFrmRate(),
										m_tCapSupportEx.GetSecDSBitRate());

	StaticLog("\n\t m_byIsNeedPrs	: %d", m_byIsNeedPrs);
	StaticLog("\n\t m_cConfId	:\n\t\t");
	m_cConfId.Print();
	StaticLog("\n");
	
	return;
}

/*====================================================================
函数名      : PrintChnlInfo
功能        ：通道信息打印
算法实现    ：
引用全局变量：
输入参数说明：
返回值说明  ：
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2013/03/02  2.0         国大卫          创建
====================================================================*/
void CApu2Chnnl::PrintChnlInfo()
{
	u8 byIdx = 0;
	
	TKdvRcvStatus tKdvRcvStatus;
	memset(&tKdvRcvStatus, 0, sizeof(tKdvRcvStatus));
	TNetSndParam tNetSndParam;
	memset(&tNetSndParam, 0, sizeof(tNetSndParam));
	
	
	//打印发送对象和接收对象信息
	for (byIdx = 0; byIdx < m_pcMixerCfg->m_byMemberNum + 1; byIdx++)
	{
		//N-1模式通道
		if (0 != m_abyMtId[byIdx] && byIdx < m_pcMixerCfg->m_byMemberNum && m_pcAudioRcv != NULL && m_pcAudioSnd != NULL)
		{
			m_pcAudioRcv[byIdx].GetStatus(tKdvRcvStatus);
			m_pcAudioSnd[byIdx].GetNetSndParam(&tNetSndParam);
			
			StaticLog("\nN-1 CHNL's ID[%d] : MT's ID[%d] | RTP-RcvPort[%d]  RTCP-RcvPort[%d]  RTCP-BackAddr[%s]  RTCP-BackPort[%d] | \
				\n\t\t\t  | RTP-SndPort[%d]  RTCP-SndPort[%d]  RemoteAddr[%s]  RemotePort[%d] |\n", 
				byIdx, m_abyMtId[byIdx], tKdvRcvStatus.m_tRcvAddr.m_tLocalNet.m_wRTPPort, 
				tKdvRcvStatus.m_tRcvAddr.m_tLocalNet.m_wRTCPPort, StrOfIP(tKdvRcvStatus.m_tRcvAddr.m_dwRtcpBackAddr), tKdvRcvStatus.m_tRcvAddr.m_wRtcpBackPort, 
				tNetSndParam.m_tLocalNet.m_wRTPPort, tNetSndParam.m_tLocalNet.m_wRTCPPort, 
				StrOfIP(tNetSndParam.m_tRemoteNet[0].m_dwRTPAddr), tNetSndParam.m_tRemoteNet[0].m_wRTPPort);
		}
		
		//N模式通道
		if (byIdx == m_pcMixerCfg->m_byMemberNum && m_pcAudioSnd != NULL &&
			(CApu2ChnStatus::MIXING == GetState() || CApu2ChnStatus::MIXING_VAC == GetState()))
		{
			m_pcAudioSnd[byIdx].GetNetSndParam(&tNetSndParam);
			
			StaticLog("\nN CHNL's ID[%d] : | RTP-SndPort[%d]  RTCP-SndPort[%d]  RemoteAddr[%s]  RemotePort[%d] |\n", 
				byIdx - m_pcMixerCfg->m_byMemberNum, tNetSndParam.m_tLocalNet.m_wRTPPort, tNetSndParam.m_tLocalNet.m_wRTCPPort, 
				StrOfIP(tNetSndParam.m_tRemoteNet[0].m_dwRTPAddr), tNetSndParam.m_tRemoteNet[0].m_wRTPPort);	
		}
	}

	return;
}

/*====================================================================
函数名      : PrintStatus
功能        ：混音编解码信息打印
算法实现    ：
引用全局变量：
输入参数说明：
返回值说明  ：
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2013/03/02  2.0         国大卫          创建
====================================================================*/
void CApu2Chnnl::PrintMixState()
{
	u8 byVolume = 0 ;
	TKdvMixerChnlInfo tChnlInfo;
	//解码信息
	TKdvMixerChStatus tDecChnlStatus;
	TKdvDecStatis tDecChnlStatis;
	//编码信息
	TKdvMixerChStatus tEncChnlStatus;
	TKdvEncStatis tEncChnlStatis;
	u16 wRet = 0;

	for (u16 wIndex = 0; wIndex < m_pcMixerCfg->m_byMemberNum; wIndex++)
	{
		if (0 == m_abyMtId[wIndex])
		{
			continue;
		}
		memset(&tChnlInfo, 0, sizeof(tChnlInfo));
		memset(&tDecChnlStatus, 0, sizeof(tDecChnlStatus));
		memset(&tDecChnlStatis, 0, sizeof(tDecChnlStatis));
		memset(&tEncChnlStatus, 0, sizeof(tEncChnlStatus));
		memset(&tEncChnlStatis, 0, sizeof(tEncChnlStatis));
		
		tChnlInfo.bNMode = 0;
		tChnlInfo.dwChID = wIndex;
		tChnlInfo.dwMixerId = GetInsID() - 1;

		//取得混音器解码通道当前状态
		wRet = m_cMixer.GetMixerDecChnlStatus(tChnlInfo, tDecChnlStatus);
		if ((u16)Codec_Success != wRet)
		{
			StaticLog("[PrintStatus] Error: Mixer.%d GetMixerDecChnlStatus failed! NMode:%d  ChID:%d  MixerId:%d!\n", 
				GetInsID(), tChnlInfo.bNMode, tChnlInfo.dwChID, tChnlInfo.dwMixerId);
		}

		//取得混音器编码通道当前状态
		wRet = m_cMixer.GetMixerEncChnlStatus(tChnlInfo, tEncChnlStatus);
		if ((u16)Codec_Success != wRet)
		{
			StaticLog("[PrintStatus] Error: Mixer.%d GetMixerEncChnlStatus failed! NMode:%d  ChID:%d  MixerId:%d!\n", 
				GetInsID(), tChnlInfo.bNMode, tChnlInfo.dwChID, tChnlInfo.dwMixerId);
		}

		//取得混音器解码通道号当前统计
		wRet = m_cMixer.GetMixerDecChStatis(tChnlInfo, tDecChnlStatis);
		if ((u16)Codec_Success != wRet)
		{
			StaticLog("[PrintStatus] Error: Mixer.%d GetMixerDecChStatis failed! NMode:%d  ChID:%d  MixerId:%d!\n", 
				GetInsID(), tChnlInfo.bNMode, tChnlInfo.dwChID, tChnlInfo.dwMixerId);
		}

		//取得混音器编码通道号当前统计
		wRet = m_cMixer.GetMixerEncChStatis(tChnlInfo, tEncChnlStatis);
		if ((u16)Codec_Success != wRet)
		{
			StaticLog("[PrintStatus] Error: Mixer.%d GetMixerEncChStatis failed! NMode:%d  ChID:%d  MixerId:%d!\n", 
				GetInsID(), tChnlInfo.bNMode, tChnlInfo.dwChID, tChnlInfo.dwMixerId);
		}

		OspPrintf(TRUE, FALSE, "\n\n");
		OspPrintf(TRUE, FALSE, "    __ChannelInfo__        __________EncStatis________            _________DecStatis__________      \n");
		OspPrintf(TRUE, FALSE, "   |      |        |      |         |        |        |          |         |        |         |     \n");
		OspPrintf(TRUE, FALSE, " ChID  EndType  DecType FrameRate BitRate PackLose PackError FrameRate RecvFrame LoseFrame LoseRatio\n");
		OspPrintf(TRUE, FALSE, "  %2d    %4d      %4d   %4d    %3d   %5d%8d      %d%8d%8d       %d						\n",
			tChnlInfo.dwChID, tEncChnlStatus.tMixChnlType.dwAudType, tDecChnlStatus.tMixChnlType.dwAudType, 
			tEncChnlStatis.m_dwFrameRate, tEncChnlStatis.m_dwBitRate, tEncChnlStatis.m_dwPackLose, tEncChnlStatis.m_dwPackError, 
			tDecChnlStatis.m_wFrameRate, tDecChnlStatis.m_dwRecvFrame, tDecChnlStatis.m_dwLoseFrame, tDecChnlStatis.m_wLoseRatio);
	}
}
/*====================================================================
函数名      ：InitChnlInfo
功能        ：初始化通道编解码信息
算法实现    ：
引用全局变量：
输入参数说明：
返回值说明  ：
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2013/03/02  2.0         国大卫          创建
====================================================================*/
void CApu2Chnnl::InitAudChnlInfo()
{
	for (u8 byLoop = 0; byLoop < MAXNUM_MIXER_CHNNL + MAXNUM_NMODECHN; byLoop++)
	{
		m_atAudChnlInfo[byLoop].byDecAudType = MEDIA_TYPE_NULL;
		m_atAudChnlInfo[byLoop].byDecSoundChnlNum = 0;
		memset(&m_atAudChnlInfo[byLoop].tDecPayload, 0, sizeof(m_atAudChnlInfo[byLoop].tDecPayload));
		m_atAudChnlInfo[byLoop].byEncAudType = MEDIA_TYPE_NULL;
		m_atAudChnlInfo[byLoop].byEncSoundChnlNum = 0;
		memset(&m_atAudChnlInfo[byLoop].tEncPayload, 0, sizeof(m_atAudChnlInfo[byLoop].tEncPayload));
		m_atAudChnlInfo[byLoop].bySampleRate = 0;
	}
}
/*====================================================================
函数名      ：GetChnlInfo
功能        ：获取通道编解码信息
算法实现    ：
引用全局变量：
输入参数说明：
返回值说明  ：
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2013/03/02  2.0         国大卫          创建
====================================================================*/
TAudDecEncInfo*	CApu2Chnnl::GetChnlInfo(const u8 byIdx)
{
	return &m_atAudChnlInfo[byIdx];
}

/*====================================================================
函数名      ：SetMixEncInfo
功能        ：设置编码信息
算法实现    ：
引用全局变量：
输入参数说明：
返回值说明  ：
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2013/03/02  2.0         国大卫          创建
====================================================================*/
BOOL32 CApu2Chnnl::SetMixEncInfo(TAudioCapInfo *ptCapInfo, u8 bySndIdx)
{
	m_atAudChnlInfo[bySndIdx].byEncAudType = ptCapInfo->GetAudioMediaType();
	m_atAudChnlInfo[bySndIdx].byEncSoundChnlNum = ptCapInfo->GetAudioTrackNum();
	m_atAudChnlInfo[bySndIdx].tEncPayload.SetActivePayload(ptCapInfo->GetActivePayLoad());
	m_atAudChnlInfo[bySndIdx].tEncPayload.SetRealPayLoad(ptCapInfo->GetAudioMediaType());
	return TRUE;
}

/*====================================================================
函数名      ：SetMixDecInfo
功能        ：设置解码信息
算法实现    ：
引用全局变量：
输入参数说明：
返回值说明  ：
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2013/03/02  2.0         国大卫          创建
====================================================================*/
BOOL32 CApu2Chnnl::SetMixDecInfo(TAudioCapInfo *ptCapInfo, u8 byRcvIdx)
{
	m_atAudChnlInfo[byRcvIdx].byDecAudType = ptCapInfo->GetAudioMediaType();
	m_atAudChnlInfo[byRcvIdx].byDecSoundChnlNum = ptCapInfo->GetAudioTrackNum();
	m_atAudChnlInfo[byRcvIdx].tDecPayload.SetActivePayload(ptCapInfo->GetActivePayLoad());
	m_atAudChnlInfo[byRcvIdx].tDecPayload.SetRealPayLoad(ptCapInfo->GetAudioMediaType());
	return TRUE;
}

/*====================================================================
函数名      ：SetMediaEncParam
功能        ：设置底层编码参数
算法实现    ：
引用全局变量：
输入参数说明：
返回值说明  ：
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2013/03/02  2.0         国大卫          创建
====================================================================*/
void CApu2Chnnl::SetMediaEncParam(u8 byChIdx, BOOL32 bNMode)
{
	TKdvMixerChnlInfo tChnlInfo;
	tChnlInfo.dwChID = byChIdx;
	tChnlInfo.bNMode = bNMode;

	byChIdx = (bNMode == TRUE) ? m_pcMixerCfg->m_byMemberNum + byChIdx : byChIdx;
	TAudDecEncInfo* ptAudEncInfo = GetChnlInfo(byChIdx);
	TKdvMixerTypeParam tTypeParam;
	tTypeParam.dwAudType = ptAudEncInfo->byEncAudType;
	tTypeParam.dwAudMode = GetAudioMode(ptAudEncInfo->byEncAudType, ptAudEncInfo->byEncSoundChnlNum);

	if (tTypeParam.dwAudType == MEDIA_TYPE_NULL)
	{
		return;
	}
	
	LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_MIXER,
		"[SetMediaEncParam] SetAudEncParam: ChnlID:%d  NMode:%d  AudType:%d  AudMode:%d\n", 
		tChnlInfo.dwChID, tChnlInfo.bNMode, tTypeParam.dwAudType, tTypeParam.dwAudMode);
	
	u16 wRet = m_cMixer.SetAudEncParam(tChnlInfo, tTypeParam);
    if(wRet != (u16)Codec_Success)
    {
        LogPrint(LOG_LVL_ERROR, MID_MCU_MIXER, 
			"[AddMixMem] Mixer.%d SetAudEncParam Index.%d failed because.%d\n", GetInsID(), byChIdx, wRet);
    }
}
/*====================================================================
函数名      ：SetMediaDecParam
功能        ：设置底层解码参数
算法实现    ：
引用全局变量：
输入参数说明：
返回值说明  ：
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2013/03/02  2.0         国大卫          创建
====================================================================*/
void CApu2Chnnl::SetMediaDecParam(u8 byChIdx)
{
	TAudDecEncInfo* ptAudDecInfo = GetChnlInfo(byChIdx);
	if (ptAudDecInfo->byDecAudType == MEDIA_TYPE_AACLC || 
		ptAudDecInfo->byDecAudType == MEDIA_TYPE_AACLD)
	{
		TAudioDecParam tAudParam;
		memset(&tAudParam, 0, sizeof(tAudParam));
		tAudParam.m_dwSamRate = TransSimRate(ptAudDecInfo->bySampleRate);
		tAudParam.m_dwMediaType = ptAudDecInfo->byDecAudType;
		tAudParam.m_dwChannelNum = ptAudDecInfo->byDecSoundChnlNum;
		
		u16 wRet = m_cMixer.SetAudioDecParam((u32)byChIdx, tAudParam);
		LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_MIXER,
			"[AddMixMem] --AACLC/LD--channelid:%d,samrate:%d,mediatype:%d,audiotracknum:%d!\n", 
			byChIdx, tAudParam.m_dwSamRate, tAudParam.m_dwMediaType, tAudParam.m_dwChannelNum);

		if (wRet != (u16)Codec_Success)
		{
			LogPrint(LOG_LVL_ERROR, MID_MCU_MIXER,
				"[AddMixMem] Mixer.%d SetAudioDecParam Index.%d failed because.%d\n",
				GetInsID(), byChIdx, wRet);
		}
	}
}
/*====================================================================
函数名      ：SetNetRcvLocalParam
功能        ：设置接收地址参数
算法实现    ：
引用全局变量：
输入参数说明：
返回值说明  ：
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2013/03/02  2.0         国大卫          创建
====================================================================*/
void CApu2Chnnl::SetLocNetParam(TMixMember *ptMixMember, u8 byChIdx)
{
	TLocalNetParam tLocalNetParam;
	memset(&tLocalNetParam, 0, sizeof(TLocalNetParam));
	tLocalNetParam.m_dwRtcpBackAddr		   = ptMixMember->m_tRtcpBackAddr.GetIpAddr();	//RTCP回发地址
	tLocalNetParam.m_wRtcpBackPort         = ptMixMember->m_tRtcpBackAddr.GetPort();		//RTCP回发端口
	tLocalNetParam.m_tLocalNet.m_dwRTPAddr = 0;		//RTP接收地址(本地为0)
    tLocalNetParam.m_tLocalNet.m_wRTPPort  = m_pcMixerCfg->wRcvStartPort + byChIdx * PORTSPAN + 2;	//RTP接收端口
#ifdef _8KI_//由于8KI混音器与MCU在一台机器上，防止端口被占，8KI对齐8KH端口策略
	tLocalNetParam.m_tLocalNet.m_wRTCPPort = m_pcMixerCfg->wRcvStartPort + byChIdx * PORTSPAN + 6;
#else
	tLocalNetParam.m_tLocalNet.m_wRTCPPort = m_pcMixerCfg->wRcvStartPort + byChIdx * PORTSPAN + 2 + 1;
#endif
	
	LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_MIXER,  "[SetLocNetParam] SetNetRcvLocalParam Index.%d NetParam<m_wRTPPort:%d, m_wRTCPPort:%d, RtcpbackIp:%s, RtcpbackPort:%d>\n", 
		byChIdx,
		tLocalNetParam.m_tLocalNet.m_wRTPPort,
		tLocalNetParam.m_tLocalNet.m_wRTCPPort,
		StrOfIP(tLocalNetParam.m_dwRtcpBackAddr),
		tLocalNetParam.m_wRtcpBackPort);
	
	u16 wRet = m_pcAudioRcv[byChIdx].SetNetRcvLocalParam(tLocalNetParam);
	if(wRet != MEDIANET_NO_ERROR)
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_MIXER, "[SetLocNetParam] Mixer.%d SetNetRcvLocalParam Index.%d Failed because.%d]\n", GetInsID(), wRet);
    }
}

/*====================================================================
函数名      ：SetAudResend
功能        ：设置底层冗余发送参数
算法实现    ：
引用全局变量：
输入参数说明：
返回值说明  ：
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2013/03/29	2.0			国大卫		  创建
====================================================================*/
void CApu2Chnnl::SetAudResend(u8 bySndChlIdx)
{
	/*冗余发送*/
	s32 nSendTimes = 0;
	s32 nSendSpan  = 0;
	switch (m_byQualityLvl)
	{
	case 0:
		nSendTimes = 0;
		nSendSpan = 0;
		break;
	case 1:
		nSendTimes = 2;
		nSendSpan= 1;
		break;
	default:
		nSendTimes = 1;
		nSendSpan = 1;
		break;
	}

	u16 wRet = m_pcAudioSnd[bySndChlIdx].SetAudioResend(nSendTimes, nSendSpan);
	if(wRet != MEDIANET_NO_ERROR)
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_MIXER, 
			"[SetAudResend][error] MIXER.%d m_pcAudioSnd[%d] set AudioResend failed! REASON: %d!\n", GetInsID(), bySndChlIdx, wRet);
		return;
	}
	LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_MIXER, 
		"[SetAudResend] MIXER.%d m_pcAudioSnd[%d] SetAudioResend Successed.\n", GetInsID(), bySndChlIdx);

}
/*====================================================================
函数名      ：CheckConfId
功能        ：会议ID校验
算法实现    ：
引用全局变量：
输入参数说明：
返回值说明  ：
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2013/03/02  2.0         国大卫          创建
====================================================================*/
BOOL32 CApu2Chnnl::CheckConfId(CMessage* pcMsg)
{
	CServMsg cServMsg(pcMsg->content,pcMsg->length);
	if (!(m_cConfId == cServMsg.GetConfId()))
    {
        s8 achCurConfId[64] ={0};
        s8 achStopConfId[64] ={0};
        m_cConfId.GetConfIdString(achCurConfId, sizeof(achCurConfId));
        cServMsg.GetConfId().GetConfIdString(achStopConfId, sizeof(achStopConfId));
        cServMsg.SetErrorCode(ERR_MIXER_ERRCONFID);
		cServMsg.SetEventId(pcMsg->event + 2);
        PostMsgToMcu(cServMsg);
        LogPrint(LOG_LVL_ERROR, MID_MCU_MIXER, 
			"[CheckConfId] Mixer.%d's ConfId %s is not equal Mcu's ConfId(%s) stop req.\n", 
			GetInsID(), achCurConfId, achStopConfId);
		return FALSE;
    }
	return TRUE;
}

/*====================================================================
函数名      ：ReadMsgBody
功能        ：读取消息体
算法实现    ：
引用全局变量：
输入参数说明：
返回值说明  ：
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2013/06/08	2.0			国大卫		  创建
====================================================================*/
s8* CApu2Chnnl::ReadMsgBody(CServMsg &cServMsg,  u8 &wMsgLen, u8 &byChIdx)
{
	s8 *lpMsgBody = (s8 *)cServMsg.GetMsgBody();
	wMsgLen = cServMsg.GetMsgBodyLen() - sizeof(u8);
	byChIdx = *(u8 *)(lpMsgBody + wMsgLen);

	return lpMsgBody;
}

/*====================================================================
函数名      ：SndErrToServer
功能        ：错误发送
算法实现    ：
引用全局变量：
输入参数说明：
返回值说明  ：
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2013/03/02  2.0         国大卫          创建
====================================================================*/
void CApu2Chnnl::SendStartError(CServMsg &cMsg, u16 wRet)
{
	cMsg.SetErrorCode(wRet);
	cMsg.SetEventId(TEST_C_S_START_NACK);
	PostMsgToServer(cMsg);
	return;
}

/*====================================================================
函数名      ：ProcMsgPrint
功能        ：消息处理打印
算法实现    ：
引用全局变量：
输入参数说明：
返回值说明  ：
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2013/06/08	2.0			国大卫		  创建
====================================================================*/
void CApu2Chnnl::ProcMsgPrint(void)
{
	/*加密参数*/
	u8  abyKeyBuf[MAXLEN_KEY] = {0};
	s32 nKeyLen = 0;
	m_tMediaEncrypt.GetEncryptKey(abyKeyBuf, &nKeyLen);	

	LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_MIXER, 
		"*************** MIXER.%d Param As Follows ***************\n", GetInsID());
	LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_MIXER, 
		"\n m_tMediaEncrypt: \n\t Mode	: %u\n\t Key	: %s", m_tMediaEncrypt.GetEncryptMode(), abyKeyBuf);
		
	LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_MIXER, 
		"\n m_tCapSupportEx: \n\t VideoFECType	: %d\n\t AudioFECType	: %d\n\t DVideoFECType	: %d \n\t TVideoStreamCap: \n\t\t MediaType	: %d\n\t\t\t Res	: %d\n\t\t\t FR	: %d\n\t\t\t BR	: %d",
			m_tCapSupportEx.GetVideoFECType(), m_tCapSupportEx.GetAudioFECType(),
			m_tCapSupportEx.GetDVideoFECType(), m_tCapSupportEx.GetSecDSType(),
			m_tCapSupportEx.GetSecDSRes(), m_tCapSupportEx.GetSecDSFrmRate(), m_tCapSupportEx.GetSecDSBitRate());
	
	for (u8 byLoop = m_pcMixerCfg->m_byMemberNum; byLoop < m_pcMixerCfg->m_byMemberNum + MAXNUM_NMODECHN; byLoop++)
	{
		LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_MIXER,"\n m_atAudChnlInfo[%d].byEncAudType : %d", 
				byLoop, m_atAudChnlInfo[byLoop].byEncAudType);
		LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_MIXER,"\n m_atAudChnlInfo[%d].byEncSoundChnlNum : %d", 
				byLoop, m_atAudChnlInfo[byLoop].byEncSoundChnlNum);
		LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_MIXER,"\n m_atAudChnlInfo[%d].tEncPayload : %d\n", 
				byLoop, m_atAudChnlInfo[byLoop].tEncPayload.GetActivePayload());
	}
	
	LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_MIXER,
		"\n m_bySampleRate	: %d(converted:%d)", m_atAudChnlInfo[m_pcMixerCfg->m_byMemberNum].bySampleRate, ((u8)12 - m_atAudChnlInfo[m_pcMixerCfg->m_byMemberNum].bySampleRate));
	LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_MIXER,
		"\n m_Depth	: %d", m_byDepth);
	LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_MIXER,
		"\n m_byIsNeedPrs	: %d", m_byIsNeedPrs);
	LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_MIXER, 
		"\n*******************************************************************************\n", GetInsID());
}
/*====================================================================
函数名      : CApu2ChnStatus
功能        ：CApu2ChnStatus构造
算法实现    ：
引用全局变量：
输入参数说明：
返回值说明  ：
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2012/02/14  1.0         周嘉麟          创建
====================================================================*/
CApu2ChnStatus::CApu2ChnStatus()
{

}


/*====================================================================
函数名      : CApu2ChnStatus
功能        ：CApu2ChnStatus析构
算法实现    ：
引用全局变量：
输入参数说明：
返回值说明  ：
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
2012/02/14  1.0         周嘉麟          创建
====================================================================*/
CApu2ChnStatus::~CApu2ChnStatus()
{

}






