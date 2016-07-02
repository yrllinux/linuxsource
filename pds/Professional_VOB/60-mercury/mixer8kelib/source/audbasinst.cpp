/*****************************************************************************
   模块名      : 混音器
   文件名      : audbasinst.cpp
   创建时间    : 2013年 9月 12日
   实现功能    : 
   作者        : 周晶晶
   版本        : 
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   20130912		1.0			周晶晶		新增
   
******************************************************************************/      
#ifdef _8KI_

#include "audbasinst.h"
#include "audmixinst.h"
#include "evmcueqp.h"
#include "boardagent.h"
#include "mcuver.h"

C8KIAudBasApp g_c8KIAudBasApp;

static void BasEncFrameCallBack(u32 dwChID, u32 dwEncID,  PFRAMEHDR pFrmHdr, void* pContext)
{
	/*媒控通道信息*/
	//u8 byRcvIdx = *pdwChID / AUDIO_ADAPTOR_MAX_ENCNUM;
	//u8 bySndIdx = *pdwChID % AUDIO_ADAPTOR_MAX_ENCNUM;
	C8KIAudBasInst *pIns = (C8KIAudBasInst*)pContext;

	if( pIns->GetInsID() - 1 != (u8)dwChID )
	{
		LogPrint( LOG_LVL_ERROR, MID_MCU_BAS, 
			"[BasEncFrameCallBack] baschlid.%d != paramchlid.%d \n", 
			pIns->GetInsID() - 1, dwChID );
		return;
	}

	//255编码类型不发送
	if( pIns->GetEncAudType( (u8)dwEncID ) == MEDIA_TYPE_NULL)
	{
		LogPrint( LOG_LVL_ERROR, MID_MCU_BAS, "[BasEncFrameCallBack] mediatype null\n" );
		return;
	}

	/*设置默认音频类型*/
	if (pFrmHdr->m_byMediaType == MEDIA_TYPE_AACLC)
    {
        pFrmHdr->m_byAudioMode = AUDIO_MODE_AACLC_32_M;
    }
    else if (pFrmHdr->m_byMediaType == MEDIA_TYPE_AACLD)
    {
        pFrmHdr->m_byAudioMode = AUDIO_MODE_AACLD_32_M;
    }
		
	CKdvMediaSnd *pcMediaSnd = pIns->GetMediaSndObj( (u8)dwEncID );
    if( NULL == pcMediaSnd )
	{
		LogPrint( LOG_LVL_ERROR, MID_MCU_BAS, "[BasEncFrameCallBack] pcMediaSnd null\n" );
		return;
	}
	
	/*开始发送*/
    u16 wRet = pcMediaSnd->Send(pFrmHdr);
    if (wRet != MEDIANET_NO_ERROR)
    {
		LogPrint(LOG_LVL_ERROR, MID_MCU_BAS, 
			"[BasEncFrameCallBack] BAS.%d  SndChnId.%d ERROR.%d\n",pIns->GetInsID()-1, dwEncID, wRet );
    }

    return;
}

static void BasNetRcvCallBack(PFRAMEHDR pFrmHdr, u32 dwContext)
{
	/*解析回调信息*/
	//CBasRcvCallBack *pcCallBack = (CBasRcvCallBack*)dwContext;
	C8KIAudBasInst *pIns = (C8KIAudBasInst*)dwContext;


	/*设置默认音频类型*/
	if (pFrmHdr->m_byMediaType == MEDIA_TYPE_AACLC)
    {
        pFrmHdr->m_byAudioMode = AUDIO_MODE_AACLC_32_M;
    }
    else if (pFrmHdr->m_byMediaType == MEDIA_TYPE_AACLD)
    {
        pFrmHdr->m_byAudioMode = AUDIO_MODE_AACLD_32_M;
    }
	
	/*设置音频解码码流*/
    CAudioAdaptor* pcKdvBas = pIns->GetAdaptObj();

	if( NULL == pcKdvBas )
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_BAS, "[MixNetRcvCallBack] pcKdvBas is null\n" );
		return;
	}

	/*设置解码信息*/
	u16 wRet = pcKdvBas->SetAudDecData( pIns->GetInsID()-1, pFrmHdr );
    if (wRet != (u16)Codec_Success)
    {
        LogPrint(LOG_LVL_ERROR, MID_MCU_BAS, "[MixNetRcvCallBack] SetAudDecData failed because.%d ChnId.%d datasize.%d!\n",
												wRet, pIns->GetInsID()-1, pFrmHdr->m_dwDataSize );
	}
}

C8KIAudBasInst::C8KIAudBasInst()
{	
	m_bIsNeedPrs = 0;
	m_tMediaEncrypt.Reset();
	m_tAudioCapInfo.Clear();
	m_bySampleRate = 0;
}

C8KIAudBasInst::~C8KIAudBasInst()
{
	m_bIsNeedPrs = 0;
	m_tMediaEncrypt.Reset();
	m_tAudioCapInfo.Clear();
	m_bySampleRate = 0;
}

/*====================================================================
	函数  : SendMsgToMcu
	功能  : InstanceEntry实例消息入口
	输入  : 无
	输出  : 无
	返回  : 无
	注    :
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
====================================================================*/
CAudioAdaptor* C8KIAudBasInst::GetAdaptObj( void )
{
	u8 byChlIdx = GetInsID() - 1;
	if( byChlIdx >= 0 && byChlIdx < MAXNUM_8KI_AUD_BAS )
	{
		return &m_cBas;
	}
	return NULL;
}

/*====================================================================
	函数  : SendMsgToMcu
	功能  : InstanceEntry实例消息入口
	输入  : 无
	输出  : 无
	返回  : 无
	注    :
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
====================================================================*/
CKdvMediaSnd* C8KIAudBasInst::GetMediaSndObj( u8 byEndIdx )
{
	if( byEndIdx >= MAXNUM_8KIAUDBAS_ENCNUM )
		return NULL;

	return &m_cAudioSnd[byEndIdx];
}

/*====================================================================
	函数  : SendMsgToMcu
	功能  : InstanceEntry实例消息入口
	输入  : 无
	输出  : 无
	返回  : 无
	注    :
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
====================================================================*/
u8 C8KIAudBasInst::GetEncAudType( u8 byEndIdx )
{
	if( byEndIdx >= MAXNUM_8KIAUDBAS_ENCNUM )
		return MEDIA_TYPE_NULL;

	return m_atAudAdpParam[byEndIdx].GetAudCodeType();
}

/*====================================================================
	函数  : SendMsgToMcu
	功能  : InstanceEntry实例消息入口
	输入  : 无
	输出  : 无
	返回  : 无
	注    :
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
====================================================================*/
BOOL C8KIAudBasInst::SendMsgToMcu(u16 wEvent, CServMsg* const pcServMsg)
{
    LogPrint(LOG_LVL_DETAIL, MID_MCU_BAS, "[Send] audbas Message %u(%s).\n", wEvent, ::OspEventDesc(wEvent));

    if( OspIsValidTcpNode(g_c8KIAudBasApp.m_dwMcuNode))
    {
        post(g_c8KIAudBasApp.m_dwMcuIId, wEvent, pcServMsg->GetServMsg(), pcServMsg->GetServMsgLen(), g_c8KIAudBasApp.m_dwMcuNode);
    }

    return TRUE;
}

/*====================================================================
	函数  : ProcInit
	功能  : InstanceEntry实例消息入口
	输入  : 无
	输出  : 无
	返回  : 无
	注    :
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
====================================================================*/
void C8KIAudBasInst::ProcInit(CMessage * const pcMsg)
{
	if( NULL == pcMsg )
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_BAS, "[ProcInit] pMsg is null \n" );
		return;
	}

	if( NORMAL == CurState() )
	{
		if (OspIsValidTcpNode( g_c8KIAudBasApp.m_dwMcuNode ) )
		{
			::OspNodeDiscCBRegQ( g_c8KIAudBasApp.m_dwMcuNode, GetAppID(), GetInsID() );
		}

		SendStatusChangeMsg(TRUE , u8(TBasBaseChnStatus::READY));

		LogPrint( LOG_LVL_WARNING, MID_MCU_BAS, "[ProcInit] Current state is NORMAL.\n" );

		return;
	}

	if ( IDLE != CurState() )
    {
        LogPrint(LOG_LVL_ERROR, MID_MCU_BAS, "[ProcRegAck] Current state is not IDLE! Error!\n" );
		printf("[ProcRegAck] Current state is not IDLE! Error!\n" );
        return;
    }
	
	u16 wRet = m_cBas.Create();
	if (wRet != (u16)Codec_Success)
    {
		printf("[ProcInit] BAS.%d Created Failed! REASON:%d!\n", GetInsID(), wRet);
        LogPrint(LOG_LVL_ERROR, MID_MCU_BAS, 
			"BAS: [ProcInit] BAS.%d Created Failed! REASON:%d!\n", GetInsID(), wRet);
        return ;
    }

	wRet = m_cBas.SetAudEncDataCallBack(BasEncFrameCallBack, this);
    if (wRet != (u16)Codec_Success)
    {
        m_cBas.Destroy();
		printf("[ProcInit] BAS.%d SetAudEncDataCallBack Failed! REASON:%d!\n", GetInsID(), wRet);
        LogPrint(LOG_LVL_ERROR, MID_MCU_BAS, 
			"[ProcInit] BAS.%d SetAudEncDataCallBack Failed! REASON:%d!\n", GetInsID(), wRet);
        return ;
    }
		
	//创建BAS接收对象
	wRet = m_cAudioRcv.Create(MAX_AUDIO_FRAME_SIZE, BasNetRcvCallBack, (u32)this);
	if( wRet != MEDIANET_NO_ERROR )
	{
		m_cBas.Destroy();
		printf("ERROR: [Init] BAS.%d m_pcAudioRcv Created Failed! REASON:%d!\n", GetInsID(),  wRet);
		LogPrint(LOG_LVL_ERROR, MID_MCU_BAS, 
			"BAS: [ProcInit] BAS.%d m_pcAudioRcv Created Failed! REASON:%d!\n", GetInsID(),  wRet);
		return ;
	}
	else
	{
		//每个BAS通道对应2路发送
		for (u8 byIdx = 0; byIdx < MAXNUM_8KIAUDBAS_ENCNUM; byIdx++)
		{			
			wRet = m_cAudioSnd[byIdx].Create(MAX_AUDIO_FRAME_SIZE, 64<<10, 25, MEDIA_TYPE_G7221C);
			if(wRet != MEDIANET_NO_ERROR)
			{
				m_cBas.Destroy();
				printf("ERROR: [Init] BAS.%d m_pcAudioSnd.[%d] Created Failed! REASON:%d\n", GetInsID(), byIdx, wRet);
				LogPrint(LOG_LVL_ERROR, MID_MCU_BAS, 
					"BAS: [CApu2BasChnnl::Init][error] BAS.%d m_pcAudioSnd.[%d] Created Failed! REASON:%d\n", 
					GetInsID(), byIdx, wRet);
				return ;
			}			
		}
	}



	if (OspIsValidTcpNode( g_c8KIAudBasApp.m_dwMcuNode ) )
    {
		::OspNodeDiscCBRegQ( g_c8KIAudBasApp.m_dwMcuNode, GetAppID(), GetInsID() );
	}


	SendStatusChangeMsg(TRUE , u8(TBasBaseChnStatus::READY));
	

    NEXTSTATE(NORMAL);

    return;
}


/*====================================================================
	函数  : SendStatusChangeMsg
	功能  : 状态改变后通知MCU
	输入  : 无
	输出  : 无
	返回  : 无
	注    :
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
	03/12/4		v1.0		zhangsh			create
====================================================================*/
void C8KIAudBasInst::SendStatusChangeMsg( u8 byOnline, u8 byState)
{
    CServMsg cServMsg;
    TPeriEqpStatus tBasStatus;
    memset(&tBasStatus,0,sizeof(TPeriEqpStatus));

    tBasStatus.m_byOnline = byOnline;
    tBasStatus.SetMcuEqp( g_c8KIAudBasApp.GetMcuId(), g_c8KIAudBasApp.GetEqpId(), g_c8KIAudBasApp.GetEqpType());
	//tAudBasChnlStatus.ClrOutputAudParam(

	TAudBasChnStatus *ptAudBasChnlStatus = tBasStatus.m_tStatus.tAudBas.m_tAudBasStaus.m_t8KIAudBasStatus.GetAudChnStatus( GetInsID() - 1 );
	if( NULL != ptAudBasChnlStatus )
	{	
		ptAudBasChnlStatus->ClrOutputAudParam();
		ptAudBasChnlStatus->SetStatus(byState);
		tBasStatus.m_tStatus.tAudBas.m_tAudBasStaus.m_t8KIAudBasStatus.SetAudChnStatus( *ptAudBasChnlStatus,GetInsID() - 1 );
	}
    tBasStatus.SetAlias( g_c8KIAudBasApp.GetAlias() );
    
	u8 byChnType = BAS_8KIAUDCHN;
	u8 byChlIdx = GetInsID() - 1;
    cServMsg.SetMsgBody((u8*)&(tBasStatus), sizeof(TPeriEqpStatus));
	cServMsg.CatMsgBody((u8*)&byChnType,sizeof(byChnType));
	cServMsg.CatMsgBody((u8*)&byChlIdx,sizeof(byChlIdx));
    SendMsgToMcu(BAS_MCU_BASSTATUS_NOTIF, &cServMsg);
    return;
}

/*====================================================================
	函数  : SetAudResend
	功能  : InstanceEntry实例消息入口
	输入  : 无
	输出  : 无
	返回  : 无
	注    :
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
====================================================================*/
BOOL32 C8KIAudBasInst::SetAudResend(u8 byEncIdx)
{
	/*冗余发送*/
	s32 nSendTimes = 0;
	s32 nSendSpan  = 0;
	switch (g_c8KIAudBasApp.GetQualityLvl())
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

	u16 wRet = m_cAudioSnd[byEncIdx].SetAudioResend(nSendTimes, nSendSpan);
	if(wRet != MEDIANET_NO_ERROR)
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_BAS, 
			"[CApu2BasChnnl::SetSndObjectPara][error] BAS.%d m_pcAudioSnd[%d] set AudioResend failed! REASON: %d!\n", GetInsID(), byEncIdx, wRet);
		return FALSE;
	}
	LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_BAS, 
		"[CApu2BasChnnl::SetSndObjectPara] BAS.%d m_pcAudioSnd[%d] SetAudioResend Successed.\n", GetInsID(), byEncIdx);

	return TRUE;

}

/*====================================================================
	函数  : SetSndObjectPara
	功能  : InstanceEntry实例消息入口
	输入  : 无
	输出  : 无
	返回  : 无
	注    :
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
====================================================================*/
BOOL32 C8KIAudBasInst::SetSndObjectPara( u32 dwDstIp,u16 wDstPort,u8 byEncIdx )
{
	
    u16 wRet = m_cAudioSnd[byEncIdx].Create( MAX_AUDIO_FRAME_SIZE, 64<<10, 25, m_atAudAdpParam[byEncIdx].GetAudCodeType() );

    if(wRet != MEDIANET_NO_ERROR)
    {
		/*编码类型255*/
		if( m_atAudAdpParam[byEncIdx].GetAudCodeType() == MEDIA_TYPE_NULL)
		{
			LogPrint(LOG_LVL_WARNING, MID_MCU_BAS, "[SetSndObjectPara][warning] BAS.%d m_cAudioSnd[%d] Not Create! REASON: EncAudType--MEDIA_TYPE_NULL!\n", 
				GetInsID(), byEncIdx);
		}
		/*对象创建异常*/
        else
		{
			LogPrint(LOG_LVL_ERROR, MID_MCU_BAS, "[SetSndObjectPara][error] BAS.%d Created m_cAudioSnd[%d] of AudType:[%d] Failed! REASON: %d!\n",
				GetInsID(), byEncIdx, m_atAudAdpParam[byEncIdx].GetAudCodeType(), wRet);
		}
		return FALSE;
    }

    LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_BAS, "[SetSndObjectPara] BAS.%d Created m_cAudioSnd[%d] of AudType:[%d] Successed.\n",
		GetInsID(), byEncIdx, m_atAudAdpParam[byEncIdx].GetAudCodeType() );
	
    
    TNetSndParam tNetSndPar;
    memset( &tNetSndPar, 0, sizeof(tNetSndPar));
    tNetSndPar.m_byNum  = 1;
	/*远端RTP地址及端口*/
    tNetSndPar.m_tRemoteNet[0].m_dwRTPAddr  = dwDstIp;
    tNetSndPar.m_tRemoteNet[0].m_wRTPPort   = wDstPort;
	/*本地RTP及RTCP发送端口*/
	//tNetSndPar.m_tLocalNet.m_wRTPPort       = wDstPort;
    //tNetSndPar.m_tLocalNet.m_wRTCPPort      = wDstPort + 1;
	u8 byChlIdx = GetInsID() - 1;
	tNetSndPar.m_tLocalNet.m_wRTPPort		  = BAS_8KE_LOCALSND_PORT + (g_c8KIAudBasApp.GetEqpId() - BASID_MIN) * BAS_8KH_PORTSPAN 
													+ ( byChlIdx * MAXNUM_8KIAUDBAS_ENCNUM + byEncIdx ) * PORTSPAN + 2;
	tNetSndPar.m_tLocalNet.m_wRTCPPort        = BAS_8KE_LOCALSND_PORT + (g_c8KIAudBasApp.GetEqpId() - BASID_MIN) * BAS_8KH_PORTSPAN 
													+ ( byChlIdx * MAXNUM_8KIAUDBAS_ENCNUM + byEncIdx ) * PORTSPAN + 2 + 1;

	LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_BAS, 
		"[SetSndObjectPara] BAS.%d m_cAudioSnd[%d] LocalSnd<RtpPort.%d, RtcpPort.%d>, RemoteRcv<IP.%s, Port.%d>.\n", 
		GetInsID(), byEncIdx, tNetSndPar.m_tLocalNet.m_wRTPPort, tNetSndPar.m_tLocalNet.m_wRTCPPort,
		strofip(tNetSndPar.m_tRemoteNet[0].m_dwRTPAddr), tNetSndPar.m_tRemoteNet[0].m_wRTPPort);

	/*设置发送对象参数*/
    wRet = m_cAudioSnd[byEncIdx].SetNetSndParam(tNetSndPar);
    if(wRet != MEDIANET_NO_ERROR)
    {
        LogPrint(LOG_LVL_ERROR, MID_MCU_BAS, "[SetSndObjectPara][error] BAS.%d m_cAudioSnd[%d] SetNetSndParam Failed! REASON: %d!\n",
			GetInsID(), byEncIdx, wRet);
		return FALSE;
    }
    LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_BAS, "[SetSndObjectPara] BAS.%d m_cAudioSnd[%d] SetNetSndParam Success.\n", 
		GetInsID(), byEncIdx);

	/*设置重传间隔*/
    wRet = m_cAudioSnd[byEncIdx].ResetRSFlag( 2000, TRUE );
    if(wRet != MEDIANET_NO_ERROR)
    {
        LogPrint(LOG_LVL_ERROR, MID_MCU_BAS, "[SetSndObjectPara][error] BAS.%d m_cAudioSnd[%d] ResetRSFlag Failed! REASON: %d!\n", 
			GetInsID(), byEncIdx, wRet);
		return FALSE;
    }
    LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_BAS, "[SetSndObjectPara] BAS.%d m_cAudioSnd[%d] ResetRSFlag Successed.\n", 
		GetInsID(), byEncIdx);

	/*冗余发送设置*/
	SetAudResend(byEncIdx);

	/*加密参数*/
    u8  abyKeyBuf[MAXLEN_KEY];
    memset(abyKeyBuf, 0, MAXLEN_KEY );
    s32 nKeyLen = 0;
    u8 byEncryptMode = m_tMediaEncrypt.GetEncryptMode();
    if ( CONF_ENCRYPTMODE_NONE == byEncryptMode )
    {
		m_cAudioSnd[byEncIdx].SetEncryptKey( NULL, 0, 0 );
        LogPrint(LOG_LVL_DETAIL, MID_MCU_BAS, "[CApu2BasChnnl] BAS.%d set key to NULL!\n", GetInsID());      
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
		
        m_cAudioSnd[byEncIdx].SetEncryptKey( (s8*)abyKeyBuf, (u16)nKeyLen, byEncryptMode );
    }
    m_cAudioSnd[byEncIdx].SetActivePT(m_atAudAdpParam[byEncIdx].GetAudCodeType());

	return TRUE;
}

/*====================================================================
	函数  : SetMediaEncParam
	功能  : InstanceEntry实例消息入口
	输入  : 无
	输出  : 无
	返回  : 无
	注    :
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
====================================================================*/
BOOL32 C8KIAudBasInst::SetMediaDecParam( void )
{
	u8 byChlIdx = GetInsID() - 1;

	//TAudBasInfo *ptAudBasInfo = GetChnlInfo(byChIdx);
	if (m_tAudioCapInfo.GetAudioMediaType() == MEDIA_TYPE_AACLC || 
		m_tAudioCapInfo.GetAudioMediaType() == MEDIA_TYPE_AACLD)
	{		
		u8 byAudioMode = GetAudioMode(m_tAudioCapInfo.GetAudioMediaType(), m_tAudioCapInfo.GetAudioTrackNum());
		
		u16 wRet = m_cBas.SetAudioDecParam( byChlIdx, m_tAudioCapInfo.GetAudioMediaType(), byAudioMode );

		LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_BAS, 
			" [SetMediaDecParam] BAS.%d SetAudioDecParam: byChIdx:[%d] m_dwMediaType[%d] m_dwChannelNum[%d] \n", 
			GetInsID(), byChlIdx, m_tAudioCapInfo.GetAudioMediaType(), m_tAudioCapInfo.GetAudioTrackNum() );
		
		if (wRet != (u16)Codec_Success)
		{
			LogPrint(LOG_LVL_ERROR, MID_MCU_BAS, 
				"[SetMediaDecParam][error] BAS.%d SetAudioDecParam.%d Failed! Because.%d!\n", 
				GetInsID(), byChlIdx, wRet);
			return FALSE;
		}
	}
	return TRUE;
}

/*====================================================================
	函数  : SetMediaEncParam
	功能  : InstanceEntry实例消息入口
	输入  : 无
	输出  : 无
	返回  : 无
	注    :
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
====================================================================*/
BOOL32 C8KIAudBasInst::SetMediaEncParam( void )
{
	
	u8 byChlIdx = GetInsID() - 1;
	
	u8 byAudioMode = 0;
	for( u8 byLoop = 0; byLoop < MAXNUM_8KIAUDBAS_ENCNUM; byLoop++ )
	{
		if( MEDIA_TYPE_NULL == m_atAudAdpParam[byLoop].GetAudCodeType() )
		{
			continue;
		}
		SetSndObjectPara( ntohl(g_c8KIAudBasApp.GetMcuIp()), 
			g_c8KIAudBasApp.GetMcuStartPort() + (byChlIdx * MAXNUM_8KIAUDBAS_ENCNUM + byLoop) * PORTSPAN + 2, 
			byLoop);		
		
		byAudioMode = GetAudioMode(m_atAudAdpParam[byLoop].GetAudCodeType(), m_atAudAdpParam[byLoop].GetTrackNum());

		LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_BAS, 
			"[SetMediaEncParam] BAS.%d SetAudEncParam: byChIdx:[%d][%d] dwAudMode[%d] dwAudType[%d]\n", 
			GetInsID(), byChlIdx, byLoop, byAudioMode, m_atAudAdpParam[byLoop].GetAudCodeType());
		
		u16 wRet = m_cBas.SetAudEncParam(byChlIdx, byLoop, 
								m_atAudAdpParam[byLoop].GetAudCodeType(), 
								byAudioMode);

		if (wRet != (u16)Codec_Success)
		{
			LogPrint(LOG_LVL_ERROR, MID_MCU_BAS, 
				"[SetMediaEncParam] BAS.%d Chnl.%d SetAudEncParam.%d Failed! Because.%d!\n", 
				GetInsID(), byChlIdx, byLoop, wRet);
			return FALSE;
		}
	}
	return TRUE;
}


/*====================================================================
	函数  : SetRcvNetParam
	功能  : InstanceEntry实例消息入口
	输入  : 无
	输出  : 无
	返回  : 无
	注    :
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
====================================================================*/
BOOL32 C8KIAudBasInst::SetRcvObjectPara( void )
{
	u8 byChlIdx = GetInsID() - 1;
	u16 wRet = 0;
	s32 byKenLen = 0;

    u8 abyKeyBuf[MAXLEN_KEY];  
    memset(abyKeyBuf, 0, MAXLEN_KEY);

	/*设置加密参数*/
    u8 byEncryptMode = m_tMediaEncrypt.GetEncryptMode();
    if ( CONF_ENCRYPTMODE_NONE == byEncryptMode )
    {
        m_cAudioRcv.SetDecryptKey( NULL, 0, 0 );
		LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_BAS, 
			"[SetRcvObjectPara] BAS.%d Set key to NULL!\n", GetInsID());
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
        m_cAudioRcv.SetDecryptKey( (s8*)abyKeyBuf, (u16)byKenLen, byEncryptMode );
    }
	else
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_BAS, 
			"[SetRcvObjectPara] BAS.%d Wrong byEncryptMode.%d \n", GetInsID(), byEncryptMode );
		return FALSE;
	}

	/*设置载荷信息*/
    u8 byRealPayload   = m_tAudioCapInfo.GetAudioMediaType();
    u8 byActivePayload = m_tAudioCapInfo.GetActivePayLoad();
    m_cAudioRcv.SetActivePT( byActivePayload, byRealPayload );

    LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_BAS, 
		"SetRcvObjectPara BAS.%d Mode: %u KeyLen: %u PT: %d AT: %d KeyBuf [%s]\n", 
		GetInsID(), m_tMediaEncrypt.GetEncryptMode(), byKenLen, byRealPayload ,byActivePayload,abyKeyBuf);


	TRSParam tNetRSParam;

    if (m_bIsNeedPrs)
    {
		TPrsTimeSpan tPrsTimeSpan = g_c8KIAudBasApp.GetPrsTimeSpan();
        tNetRSParam.m_wFirstTimeSpan  = tPrsTimeSpan.m_wFirstTimeSpan;
        tNetRSParam.m_wSecondTimeSpan = tPrsTimeSpan.m_wSecondTimeSpan;
        tNetRSParam.m_wThirdTimeSpan  = tPrsTimeSpan.m_wThirdTimeSpan;
        tNetRSParam.m_wRejectTimeSpan = tPrsTimeSpan.m_wRejectTimeSpan;
    }
    else
    {
        tNetRSParam.m_wFirstTimeSpan  = 0;
        tNetRSParam.m_wSecondTimeSpan = 0;
        tNetRSParam.m_wThirdTimeSpan  = 0;
        tNetRSParam.m_wRejectTimeSpan = 0;
    }

	LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_BAS, 
		"[SetRcvObjectPara] BAS.%d m_wFirstTimeSpan:%d, m_wSecondTimeSpan:%d, m_wThirdTimeSpan:%d, m_wRejectTimeSpan:%d!\n", 
		GetInsID(), tNetRSParam.m_wFirstTimeSpan, tNetRSParam.m_wSecondTimeSpan,  tNetRSParam.m_wThirdTimeSpan, tNetRSParam.m_wRejectTimeSpan);
	
	/*重传间隔*/
	wRet = m_cAudioRcv.ResetRSFlag( tNetRSParam, TRUE );
	if(wRet != MEDIANET_NO_ERROR)
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_BAS, 
			"[SetRcvObjectPara] BAS.%d ResetRSFlag Failed! REASON: %d!\n", GetInsID(), wRet);
		return FALSE;
	}	

	/*启动接收*/
    wRet = m_cAudioRcv.StartRcv();
    if (MEDIANET_NO_ERROR != wRet)
    {
        LogPrint(LOG_LVL_ERROR, MID_MCU_BAS, 
			"[SetRcvObjectPara] BAS.%d StartRcv Failed! REASON: %d!\n", GetInsID(), wRet);
		return FALSE;
    }
    
	return TRUE;
	
}

/*====================================================================
	函数  : SetRcvNetParam
	功能  : InstanceEntry实例消息入口
	输入  : 无
	输出  : 无
	返回  : 无
	注    :
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
====================================================================*/
BOOL32 C8KIAudBasInst::SetRcvNetParam( void )
{
	u8 byChlIdx = GetInsID() - 1;
		
	TLocalNetParam tLocalNetParam;
	memset(&tLocalNetParam, 0, sizeof(TLocalNetParam));
	//RTCP源
	tLocalNetParam.m_dwRtcpBackAddr = ntohl(g_c8KIAudBasApp.GetMcuIp());
	tLocalNetParam.m_wRtcpBackPort = g_c8KIAudBasApp.GetEqpStartPort() + byChlIdx * MAXNUM_8KIAUDBAS_ENCNUM * PORTSPAN + 2 + 1;
	//RTP接收
	tLocalNetParam.m_tLocalNet.m_dwRTPAddr = 0;
    tLocalNetParam.m_tLocalNet.m_wRTPPort = g_c8KIAudBasApp.GetEqpStartPort() + byChlIdx * MAXNUM_8KIAUDBAS_ENCNUM * PORTSPAN + 2;
	tLocalNetParam.m_tLocalNet.m_wRTCPPort = g_c8KIAudBasApp.GetEqpStartPort() + byChlIdx * MAXNUM_8KIAUDBAS_ENCNUM * PORTSPAN + 2 + 3;
	
	//参数打印
	LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_BAS, 
		"[SetRcvNetParam] BAS:%d SetRcvNetParam: RtcpBackAddr[%s]  RtcpBackPort[%d]  LocalRtpPort[%d]  LocalPrtcpPort[%d]\n", 
		GetInsID(), strofip(tLocalNetParam.m_dwRtcpBackAddr), tLocalNetParam.m_wRtcpBackPort, 
		tLocalNetParam.m_tLocalNet.m_wRTPPort, tLocalNetParam.m_tLocalNet.m_wRTCPPort);
	
	u16 wRet = m_cAudioRcv.SetNetRcvLocalParam(tLocalNetParam);
	if(wRet != MEDIANET_NO_ERROR)
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_BAS, 
			"[SetRcvNetParam][error] BAS:%d SetRcvNetParam Failed! Because.%d!\n", GetInsID(), wRet);
		return FALSE;
    }

	return TRUE;
}

/*====================================================================
	函数  : InstanceEntry
	功能  : InstanceEntry实例消息入口
	输入  : 无
	输出  : 无
	返回  : 无
	注    :
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
====================================================================*/
void C8KIAudBasInst::ProcStartAdaptReq( CMessage * const pcMsg )
{
	if( NULL == pcMsg )
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_BAS, "[ProcStartAdaptReq] pMsg is null \n" );
		return;
	}

	
	if ( NORMAL != CurState() )
    {
        LogPrint(LOG_LVL_ERROR, MID_MCU_BAS, "[ProcStartAdaptReq] Current state is not NORMAL! Error!\n" );
		printf("[ProcStartAdaptReq] Current state is not NORMAL! Error!\n" );
        return;
    }

	CServMsg cServMsg(pcMsg->content,pcMsg->length);

	u8 *pbyMsgBody = cServMsg.GetMsgBody();

	

	for( u8 byIdx=0;byIdx < MAXNUM_8KIAUDBAS_ENCNUM;++byIdx )
	{
		m_atAudAdpParam[byIdx] = *(TAudAdaptParam*)pbyMsgBody;
		pbyMsgBody += sizeof(TAudAdaptParam);
	}

	m_bIsNeedPrs = m_atAudAdpParam[0].IsNeedbyPrs();

	m_tMediaEncrypt = *(TMediaEncrypt*)pbyMsgBody;
	pbyMsgBody += sizeof(TMediaEncrypt);

	m_tAudioCapInfo = *(TAudioCapInfo* )pbyMsgBody;
	pbyMsgBody += sizeof(TAudioCapInfo);

	m_bySampleRate = *(u8*)pbyMsgBody;

	TEqp tBas;
	tBas.SetMcuEqp(g_c8KIAudBasApp.GetMcuId(), g_c8KIAudBasApp.GetEqpId(), g_c8KIAudBasApp.GetEqpType());
	cServMsg.SetMsgBody((u8*)&tBas, sizeof(tBas));
	
#if !defined(WIN32)
	u8 byChlIdx = GetInsID() - 1;
	/*占用通道*/
	u16 wRet = m_cBas.AddChannel( byChlIdx );
    if(wRet != (u16)Codec_Success)
    {
        LogPrint(LOG_LVL_ERROR, MID_MCU_BAS, 
			"[AddBasMember] BAS.%d OccupyChnl.%d Failed! Because.%d!\n", GetInsID(), byChlIdx, wRet);
		SendMsgToMcu( pcMsg->event + 2,&cServMsg );
        return;
    }

	/*设置接收对象地址*/
	if( !SetRcvNetParam() )
	{
		SendMsgToMcu( pcMsg->event + 2,&cServMsg );
        return;
	}

	/*设置接收对象*/
	if( !SetRcvObjectPara() )
	{
		SendMsgToMcu( pcMsg->event + 2,&cServMsg );
        return;
	}

	/*设置编码参数*/
	if( !SetMediaEncParam() )
	{
		SendMsgToMcu( pcMsg->event + 2,&cServMsg );
        return;
	}

	/*设置解码参数*/
	if( !SetMediaDecParam() )
	{
		SendMsgToMcu( pcMsg->event + 2,&cServMsg );
        return;
	}

	
	wRet = m_cBas.StartAdaptor(TRUE);
	if (wRet != (u16)Codec_Success)
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_BAS, 
			"BAS: [CApu2BasChnnl::OnStartAdaptReq][error] BAS.%d StartAdapt Failed! Because.%d!\n", GetInsID(), wRet);
		SendMsgToMcu( pcMsg->event + 2,&cServMsg );
        return;
	}
#endif

	NEXTSTATE(RUNNING);

	SendMsgToMcu( pcMsg->event + 1,&cServMsg );


}

/*====================================================================
	函数  : ProcChgAdaptReq
	功能  : InstanceEntry实例消息入口
	输入  : 无
	输出  : 无
	返回  : 无
	注    :
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
====================================================================*/
void C8KIAudBasInst::ProcChgAdaptReq(CMessage * const pcMsg)
{
	if( NULL == pcMsg )
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_BAS, "[ProcChgAdaptReq] pMsg is null \n" );
		return;
	}

	
	if ( RUNNING != CurState() )
    {
        LogPrint(LOG_LVL_ERROR, MID_MCU_BAS, "[ProcChgAdaptReq] Current state is not NORMAL! Error!\n" );
		printf("[ProcChgAdaptReq] Current state is not NORMAL! Error!\n" );
        return;
    }

	CServMsg cServMsg(pcMsg->content,pcMsg->length);

	u8 *pbyMsgBody = cServMsg.GetMsgBody();

	for( u8 byIdx=0;byIdx < MAXNUM_8KIAUDBAS_ENCNUM;++byIdx )
	{
		m_atAudAdpParam[byIdx] = *(TAudAdaptParam*)pbyMsgBody;
		pbyMsgBody += sizeof(TAudAdaptParam);
	}

	m_bIsNeedPrs = m_atAudAdpParam[0].IsNeedbyPrs();

	m_tMediaEncrypt = *(TMediaEncrypt*)pbyMsgBody;
	pbyMsgBody += sizeof(TMediaEncrypt);

	m_tAudioCapInfo = *(TAudioCapInfo* )pbyMsgBody;
	pbyMsgBody += sizeof(TAudioCapInfo);

	m_bySampleRate = *(u8*)pbyMsgBody;

	TEqp tBas;
	tBas.SetMcuEqp(g_c8KIAudBasApp.GetMcuId(), g_c8KIAudBasApp.GetEqpId(), g_c8KIAudBasApp.GetEqpType());
	cServMsg.SetMsgBody((u8*)&tBas, sizeof(tBas));

#if !defined(WIN32)
	/*设置编码参数*/
	if( !SetMediaEncParam() )
	{
		SendMsgToMcu( pcMsg->event + 2,&cServMsg );
		return;
	}
	
	/*设置解码参数*/
	if( !SetMediaDecParam() )
	{
		SendMsgToMcu( pcMsg->event + 2,&cServMsg );
		return;
	}
#endif
	
	SendMsgToMcu( pcMsg->event + 1,&cServMsg );
}

/*====================================================================
	函数  : ProcStopAdaptReq
	功能  : InstanceEntry实例消息入口
	输入  : 无
	输出  : 无
	返回  : 无
	注    :
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
====================================================================*/
void C8KIAudBasInst::ProcStopAdaptReq( CMessage * const pcMsg )
{
	if( NULL == pcMsg )
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_BAS, "[ProcStopAdaptReq] pMsg is null \n" );
		return;
	}

	
	if ( RUNNING != CurState() )
    {
        LogPrint(LOG_LVL_ERROR, MID_MCU_BAS, "[ProcStopAdaptReq] Current state is not NORMAL! Error!\n" );
		printf("[ProcChgAdaptReq] Current state is not NORMAL! Error!\n" );
        return;
    }

	CServMsg cServMsg( pcMsg->content,pcMsg->length );

	TEqp tBas;
	tBas.SetMcuEqp(g_c8KIAudBasApp.GetMcuId(), g_c8KIAudBasApp.GetEqpId(), g_c8KIAudBasApp.GetEqpType());
	cServMsg.SetMsgBody((u8*)&tBas, sizeof(tBas));
	
#if !defined(WIN32)
	/*关闭适配*/
	u16 wRet = m_cBas.StartAdaptor(FALSE);
	if (wRet != (u16)Codec_Success)
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_BAS, 
			"[ProcStopAdaptReq] BAS.%d StopAdapt Failed! Because.%d!\n", GetInsID(), wRet);
		SendMsgToMcu( pcMsg->event + 2,&cServMsg );
		return;
	}
#endif
	SendMsgToMcu( pcMsg->event + 1,&cServMsg );

	NEXTSTATE(NORMAL);

}

/*====================================================================
	函数  : InstanceEntry
	功能  : InstanceEntry实例消息入口
	输入  : 无
	输出  : 无
	返回  : 无
	注    :
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
====================================================================*/
void C8KIAudBasInst::ProcDisconnect( CMessage * const pcMsg )
{
	ProcStopAdaptReq( pcMsg );

	NEXTSTATE(NORMAL);
}

/*====================================================================
	函数  : InstanceEntry
	功能  : InstanceEntry实例消息入口
	输入  : 无
	输出  : 无
	返回  : 无
	注    :
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
====================================================================*/
void C8KIAudBasInst::InstanceEntry( CMessage * const pcMsg )
{
	if( NULL == pcMsg )
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_BAS, "[InstanceEntry] pMsg is null \n" );
		return;
	}

	LogPrint(LOG_LVL_DETAIL, MID_MCU_BAS,"[InstanceEntry]Daemon:Recv msg %u(%s).\n", pcMsg->event, ::OspEventDesc(pcMsg->event));


	switch(pcMsg->event)
    {
	case EV_BAS_INI:
		ProcInit( pcMsg );
		break;

	case MCU_BAS_STARTADAPT_REQ:
		ProcStartAdaptReq(pcMsg);
		break;

	case MCU_BAS_CHANGEAUDPARAM_REQ:
		ProcChgAdaptReq(pcMsg);
		break;

	case MCU_BAS_STOPADAPT_REQ:
		ProcStopAdaptReq(pcMsg);
		break;

	case OSP_DISCONNECT:
		ProcDisconnect( pcMsg );
		break;

	default:
		LogPrint(LOG_LVL_DETAIL, MID_MCU_BAS,"[DaemonInstanceEntry] unknow msg.%d\n",pcMsg->event );
		break;
	}
	
}

/*====================================================================
	函数  : DaemonProcInit
	功能  : DaemonProcInit
	输入  : 无
	输出  : 无
	返回  : 无
	注    :
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
====================================================================*/
void C8KIAudBasInst::DaemonProcInit(CMessage * const pcMsg)
{
	if( NULL == pcMsg )
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_BAS, "[DaemonInstanceEntry] pMsg is null \n" );
		return;
	}

	//初始化socket
	u16 wRet = KdvSocketStartup();
    if ( MEDIANET_NO_ERROR != wRet )
    {
        LogPrint(LOG_LVL_ERROR, MID_MCU_BAS, "BAS: [DaemonProcInit] KdvSocketStartup Failed!\n");
        return;
    }

	T8keEqpCfg *ptEqpCfg = (T8keEqpCfg*)pcMsg->content;
	
	g_c8KIAudBasApp.SetConnectIp( ptEqpCfg->GetConnectIp() );
	g_c8KIAudBasApp.SetConnectPort( ptEqpCfg->GetConnectPort() );
	g_c8KIAudBasApp.SetMcuId( LOCAL_MCUID );
    g_c8KIAudBasApp.SetEqpType( EQP_TYPE_BAS );    
    g_c8KIAudBasApp.SetAlias( "8KIAudBas" );

	s32 sdwQuaLvlDefault = 0;
	s32 sdwReturnValue = 0;
	s8  achProfileName[64] = {0};
	memset((void*)achProfileName, 0x0, sizeof(achProfileName));
	sprintf(achProfileName, "%s/%s", DIR_CONFIG, "mcueqp.ini");
	BOOL32 bRet = TRUE;// 读取模拟版

	// 读取音频质量等级
	bRet = GetRegKeyInt( achProfileName, SECTION_EqpMixer , KEY_QualityLvl, sdwQuaLvlDefault, &sdwReturnValue );
	g_c8KIAudBasApp.SetQualityLvl((u8)sdwReturnValue);




	NEXTSTATE(IDLE);


	SetTimer(EV_BAS_CONNECT_TIMER, BAS_CONNETC_TIMEOUT);
	LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_BAS,"[DaemonProcInit]set EV_BAS_CONNECT_TIMER!\n" );	
}

/*=============================================================================
  函 数 名： ConnectMcu
  功    能： 
  算法实现： 
  全局变量： 
  参    数： 
             u32& dwMcuNode
  返 回 值： BOOL32 
=============================================================================*/
BOOL32 C8KIAudBasInst::ConnectMcu( u32& dwMcuNode )
{
    BOOL32 bRet = TRUE;

    if( !OspIsValidTcpNode(dwMcuNode))
    {
        dwMcuNode = OspConnectTcpNode(htonl(g_c8KIAudBasApp.GetConnectIp()), g_c8KIAudBasApp.GetConnectPort(), 10, 0, 100 );  

        if (OspIsValidTcpNode(dwMcuNode))
        {
            ::OspNodeDiscCBRegQ( dwMcuNode, GetAppID(), GetInsID() );
            LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_BAS,"[ConnectMcu] Connect to Mcu Success,node is %u!\n", dwMcuNode);
			printf("[ConnectMcu] Connect to Mcu Success,node is %u!\n", dwMcuNode);
            // 记录本地IP
            g_c8KIAudBasApp.SetEqpIp( ntohl( OspNodeLocalIpGet(dwMcuNode) ) );
        }
        else
        {
            //建链失败
            LogPrint(LOG_LVL_ERROR, MID_MCU_BAS, "[ConnectMcu] Failed to Connect Mcu %s:%d>\n", 
                                    strofip(g_c8KIAudBasApp.GetConnectIp(), FALSE ), g_c8KIAudBasApp.GetConnectPort());
			printf("[ConnectMcu] Failed to Connect Mcu %s:%d>\n", 
                                    strofip(g_c8KIAudBasApp.GetConnectIp(), FALSE ), g_c8KIAudBasApp.GetConnectPort());
            bRet = FALSE;
        }
    }
    return bRet;
}

/*====================================================================
	函数  : DaemonProcConnectTimeOut
	功能  : connect消息入口
	输入  : 无
	输出  : 无
	返回  : 无
	注    :
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
====================================================================*/
void C8KIAudBasInst::DaemonProcConnectTimeOut( void )
{	
    switch( CurState() )
    {
    case IDLE:
        {
            BOOL32 bRet = FALSE;
            bRet = ConnectMcu(g_c8KIAudBasApp.m_dwMcuNode);
            if( bRet )
            { 
                SetTimer(EV_BAS_REGISTER_TIMER, BAS_REGISTER_TIMEOUT); 
            }
            else
            {
                SetTimer(EV_BAS_CONNECT_TIMER, BAS_CONNETC_TIMEOUT);
            }
        }
        break;
    default:
        LogPrint(LOG_LVL_ERROR, MID_MCU_BAS, "[DaemonProcConnectTimeOut]: EV_BAS_CONNECT_TIMER received in wrong state %u!\n", CurState());
        break;
    }
 
    return;
}

void C8KIAudBasInst::DaemonProcRegisterTimeOut( void )
{	
    switch( CurState() )
	{
	case IDLE:
		{
			LogPrint( LOG_LVL_KEYSTATUS, MID_MCU_BAS,"[DaemonProcRegisterTimeOut] App:%d, Ins:%d \n", GetAppID(), GetInsID());
			CServMsg cSvrMsg;
			TEqpRegReq tReg;
			memset(&tReg, 0, sizeof(tReg));
			
			
			tReg.SetEqpIpAddr( g_c8KIAudBasApp.GetEqpIp() );
			tReg.SetEqpType(g_c8KIAudBasApp.GetEqpType());
			tReg.SetEqpAlias(g_c8KIAudBasApp.GetAlias());
			tReg.SetVersion( DEVVER_BAS );
			tReg.SetMcuId( g_c8KIAudBasApp.GetMcuId() );
			
			cSvrMsg.SetMsgBody((u8*)&tReg, sizeof(tReg));
			u8 byIsAudBas = 1;
			cSvrMsg.CatMsgBody((u8*)&byIsAudBas, sizeof(byIsAudBas));
			
			post( MAKEIID( AID_MCU_PERIEQPSSN, CInstance::DAEMON ), BAS_MCU_REG_REQ,
						cSvrMsg.GetServMsg(), cSvrMsg.GetServMsgLen(), g_c8KIAudBasApp.m_dwMcuNode);

			SetTimer(EV_BAS_REGISTER_TIMER, BAS_REGISTER_TIMEOUT); 
			break;
		}
	default:
		LogPrint( LOG_LVL_ERROR, MID_MCU_BAS,"[CBASInst::ProRegister]: EV_BAS_REGISTER_TIMER received in wrong state %u!\n", CurState());
        break;
	}

}
/*====================================================================
	函数  : DaemonInstanceEntry
	功能  : Daemon实例消息入口
	输入  : 无
	输出  : 无
	返回  : 无
	注    :
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
====================================================================*/
void C8KIAudBasInst::DaemonProcRegisterAck( CMessage * const pcMsg )
{
	if( NULL == pcMsg )
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_BAS, "[DaemonProcRegisterAck] pMsg is null \n" );
		return;
	}

	if ( IDLE != CurState() )
    {
        LogPrint(LOG_LVL_ERROR, MID_MCU_BAS, "[ProcRegAck] Current state is not IDLE! Error!\n" );
		printf("[ProcRegAck] Current state is not IDLE! Error!\n" );
        return;
    }

    NEXTSTATE(NORMAL);

	CServMsg cServMsg(pcMsg->content, pcMsg->length);
    TEqpRegAck tRegAck = *(TEqpRegAck*)cServMsg.GetMsgBody();

	g_c8KIAudBasApp.m_dwMcuIId = pcMsg->srcid;

	KillTimer(EV_BAS_REGISTER_TIMER);
    LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_BAS, "register to mcu.A succeed !\n");
	printf("register to mcu.A succeed !\n");

	g_c8KIAudBasApp.SetEqpId( tRegAck.GetEqpId() );
    g_c8KIAudBasApp.SetMcuStartPort( tRegAck.GetMcuStartPort() );
    g_c8KIAudBasApp.SetEqpStartPort( tRegAck.GetEqpStartPort() );
	g_c8KIAudBasApp.SetMcuIp( tRegAck.GetMcuIpAddr() );

	TPrsTimeSpan tPrsTimeSpan = *(TPrsTimeSpan*)(cServMsg.GetMsgBody() + sizeof(TEqpRegAck));
	tPrsTimeSpan.m_wFirstTimeSpan  = ntohs(tPrsTimeSpan.m_wFirstTimeSpan);
	tPrsTimeSpan.m_wSecondTimeSpan = ntohs(tPrsTimeSpan.m_wSecondTimeSpan);
	tPrsTimeSpan.m_wThirdTimeSpan  = ntohs(tPrsTimeSpan.m_wThirdTimeSpan);
	tPrsTimeSpan.m_wRejectTimeSpan = ntohs(tPrsTimeSpan.m_wRejectTimeSpan);
	// xsl [8/15/2006] 限制一下最大值，防止底层内存申请失败
	if(tPrsTimeSpan.m_wRejectTimeSpan > DEF_LOSETIMESPAN_PRS)
	{
		tPrsTimeSpan.m_wRejectTimeSpan = DEF_LOSETIMESPAN_PRS;
	}
	g_c8KIAudBasApp.SetPrsTimeSpan( tPrsTimeSpan );

	
	u16 wMTUSize = *(u16*)( cServMsg.GetMsgBody() + sizeof(TEqpRegAck) + sizeof(TPrsTimeSpan) );
	wMTUSize = ntohs( wMTUSize );
	g_c8KIAudBasApp.SetMTUSize( wMTUSize );

	for( u8 byLoop = 1;byLoop <= MAXNUM_8KI_AUD_BAS; ++byLoop )
	{
		post(MAKEIID(GetAppID(), byLoop), EV_BAS_INI, NULL, 0);	
	}
	
}

void C8KIAudBasInst::DaemonProcRegisterNack( void )
{
	if ( IDLE != CurState() )
    {
        LogPrint(LOG_LVL_ERROR, MID_MCU_BAS, "[ProcRegAck] Current state is not IDLE! Error!\n" );
		printf("[ProcRegAck] Current state is not IDLE! Error!\n" );
        return;
    }

	LogPrint( LOG_LVL_ERROR, MID_MCU_BAS, "[Info] bas Register to MCU be refused .retry...\n");

}

/*====================================================================
	函数  : DaemonInstanceEntry
	功能  : Daemon实例消息入口
	输入  : 无
	输出  : 无
	返回  : 无
	注    :
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
====================================================================*/
void C8KIAudBasInst::DaemonProcAdpatMsg( CMessage * const pcMsg )
{
	if( NULL == pcMsg )
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_BAS, "[DaemonInstanceEntry] pMsg is null \n" );
		return;
	}

	if ( NORMAL != CurState() )
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_BAS, "[DaemonProcAdpatMsg] Current state is not NORMAL! Error!\n" );
		printf("[DaemonProcAdpatMsg] Current state is not NORMAL! Error!\n" );
        return;
	}

	CServMsg cServMsg( pcMsg->content,pcMsg->length );

	LogPrint(LOG_LVL_ERROR, MID_MCU_BAS,"[DaemonProcAdpatMsg] rcv Msg%d(%s) chl.%d\n",pcMsg->event,
		OspEventDesc(pcMsg->event),cServMsg.GetChnIndex() );	

	if( cServMsg.GetChnIndex() >= 0 && cServMsg.GetChnIndex() < MAXNUM_8KI_AUD_BAS )
	{
		post( MAKEIID(GetAppID(), cServMsg.GetChnIndex()+1 ), pcMsg->event, pcMsg->content, pcMsg->length );
	}
	else
	{
		SendMsgToMcu(pcMsg->event+2, &cServMsg);
	}
		

}

/*====================================================================
	函数  : DaemonInstanceEntry
	功能  : Daemon实例消息入口
	输入  : 无
	输出  : 无
	返回  : 无
	注    :
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
====================================================================*/
void C8KIAudBasInst::DaemonProcDisconnect( CMessage* const pMsg )
{

	SetTimer(EV_BAS_CONNECT_TIMER, BAS_CONNETC_TIMEOUT);

	LogPrint(LOG_LVL_KEYSTATUS, MID_MCU_BAS,"[DaemonProcDisconnect]set EV_BAS_CONNECT_TIMER!\n" );	

	NEXTSTATE(IDLE);

}

/*====================================================================
	函数  : DaemonInstanceEntry
	功能  : Daemon实例消息入口
	输入  : 无
	输出  : 无
	返回  : 无
	注    :
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
====================================================================*/
void C8KIAudBasInst::DaemonInstanceEntry(CMessage* const pMsg, CApp* pcApp)
{
	if( NULL == pMsg )
	{
		LogPrint(LOG_LVL_ERROR, MID_MCU_BAS, "[DaemonInstanceEntry] pMsg is null \n" );
		return;
	}

	LogPrint(LOG_LVL_DETAIL, MID_MCU_BAS,"[DaemonInstanceEntry]Daemon:Recv msg %u(%s).\n", pMsg->event, ::OspEventDesc(pMsg->event));
	
	switch(pMsg->event)
    {
	case EV_BAS_INI:
		DaemonProcInit( pMsg );
		break;

	case EV_BAS_CONNECT_TIMER:
		DaemonProcConnectTimeOut();
		break;

	case EV_BAS_REGISTER_TIMER:
        DaemonProcRegisterTimeOut();
        break; 

	case MCU_BAS_REG_ACK:
		DaemonProcRegisterAck( pMsg );
		break;

	case MCU_BAS_REG_NACK:
		DaemonProcRegisterNack();
		break;

	case MCU_BAS_STARTADAPT_REQ:
	case MCU_BAS_CHANGEAUDPARAM_REQ:
	case MCU_BAS_STOPADAPT_REQ:
		DaemonProcAdpatMsg(pMsg);
		break;

	case OSP_DISCONNECT:
		DaemonProcDisconnect( pMsg );
		break;

	default:
		LogPrint(LOG_LVL_DETAIL, MID_MCU_BAS,"[DaemonInstanceEntry] unknow msg.%d\n",pMsg->event );
		break;
	}
}

/*====================================================================
	函数  : GetAudioMode
	功能  : Daemon实例消息入口
	输入  : 无
	输出  : 无
	返回  : 无
	注    :
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
====================================================================*/
u8 C8KIAudBasInst::GetAudioMode(const u8 byAudioType, u8 byAudioTrackNum)
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
	case MEDIA_TYPE_NULL:
		break;
    default:
        LogPrint(LOG_LVL_ERROR, MID_MCU_BAS, 
			"BAS: [CApu2BasChnnl::GetAudioMode][error] BAS.%d GetAudioMode MediaType.%d not surportted!!\n", GetInsID()-1, byAudioType);
        return 0;
    }
    return byAudMode;
}

void C8KIBASCfg::Clear( void )
{
	m_dwMcuNode = INVALID_NODE;
	m_dwMcuIId  = 0;
	m_dwConnectIP = 0;
	m_wConnectPort = 0;
	m_wEqpStartPort = 0;
	m_wEqpStartPort = 0;
	m_wMcuStartPort = 0;
	m_wMcuStartPort = 0;
	m_EqpId = 0;
	m_byEqpType = 0;
	m_byEqpType = 0;
	m_MTUSize = 0;
	m_byQualityLvl = 0;
	m_dwLocalIP = 0;
	m_byMcuId = 0;
	memset( &m_achAlias,0,sizeof(m_achAlias) );
}

C8KIBASCfg::C8KIBASCfg()
{
	Clear();
}

C8KIBASCfg::~C8KIBASCfg()
{
	Clear();
}

#endif//#ifdef _8KI_