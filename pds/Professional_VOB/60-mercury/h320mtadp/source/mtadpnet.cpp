/*****************************************************************************
模块名      : h320终端适配模块
文件名      : MtAdpNet.cpp
相关文件    : MtAdpNet.h
文件实现功能: 码流收发处理模块
作者        : 许世林
版本        : V3.6  Copyright(C) 1997-2003 KDC, All rights reserved.
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2005/06/07  3.6         许世林      创建
******************************************************************************/
#ifdef WIN32
#define TYPE_SCODE
#endif

#include "mtadpnet.h"
#include "mtadpinst.h"
#include "h261addrtpheader.h"
#include "h263decrtpreform.h"

/*=============================================================================
  函 数 名： ProcVidNetRcv
  功    能： 网络接收视频数据回调
  算法实现： 将接收到的视频码流通过协议栈接口发送到终端
  全局变量： 
  参    数： PFEFRAMEHDR pFrmHdr
             u32 dwContext
  返 回 值： void 
=============================================================================*/
void ProcVidNetRcv(PFRAMEHDR pFrmHdr, u32 dwContext)
{
    CMtAdpInst *pcIns = (CMtAdpInst *)dwContext;
    if(NULL != pcIns)
    {       
        u16 wRet = pcIns->m_pcH320Stack->SendData(pFrmHdr->m_pData, 
                                                  pFrmHdr->m_dwDataSize, 
                                                  dtype_video);
        if(KDVH320_OK != wRet)
        {
            pcIns->MtAdpLog(LOG_WARNING, "[ProcVidNetRcv] send video data to mt failed. err no :%d\n", wRet);
        }
    }        
  
    return;
}

/*=============================================================================
  函 数 名： ProcAudNetRcv
  功    能： 网络接收音频数据回调
  算法实现： 将接收到的音频码流通过协议栈接口发送到终端
  全局变量： 
  参    数： PFRAMEHDR pFrmHdr
             u32 dwContext
  返 回 值： void 
=============================================================================*/
void ProcAudNetRcv(PFRAMEHDR pFrmHdr, u32 dwContext)
{
    CMtAdpInst *pcIns = (CMtAdpInst *)dwContext;
    if(NULL != pcIns)
    {       
        u16 wRet = pcIns->m_pcH320Stack->SendData(pFrmHdr->m_pData, 
                                                  pFrmHdr->m_dwDataSize, 
                                                  dtype_audio);
        if(KDVH320_OK != wRet)
        {
            pcIns->MtAdpLog(LOG_WARNING, "[ProcAudNetRcv] send audio data to mt failed. err no :%d\n", wRet);
        }
    }    
    
    return;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMtAdpNet::CMtAdpNet(CMtAdpInst * const pcIns)
{
    m_pcMtAdpIns = pcIns;

    m_pcAudNetRcv = NULL;
    m_pcVidNetRcv = NULL;
    m_pcAudNetSnd = NULL;
    m_pcVidNetSnd = NULL;
   
    m_byAudSndMediaType = MEDIA_TYPE_NULL;
    m_byVidSndMediaType = MEDIA_TYPE_NULL;

    m_byRtpPackBuf = new u8[BUFLEN_RTPPACK];

#ifdef WIN32
    KdvSocketStartup();
#endif

    OspSemBCreate(&m_semAudSnd);
    OspSemBCreate(&m_semVidSnd);   
}

CMtAdpNet::~CMtAdpNet()
{
    SAFE_DELETE(m_pcAudNetRcv);
    SAFE_DELETE(m_pcVidNetRcv);
    SAFE_DELETE(m_pcAudNetSnd);
    SAFE_DELETE(m_pcVidNetSnd);

    SAFE_DELETE(m_byRtpPackBuf);
 
#ifdef WIN32
    KdvSocketCleanup();        
#endif

    OspSemDelete(m_semAudSnd);
    OspSemDelete(m_semVidSnd);
}

/*=============================================================================
  函 数 名： VidNetRcvInit
  功    能： 视频接收对象初始化
  算法实现： 
  全局变量： 
  参    数： TLocalNetParam &tNetParam
  返 回 值： BOOL32 
=============================================================================*/
BOOL32 CMtAdpNet::VidNetRcvInit()
{    
    if(NULL == m_pcMtAdpIns)
    {
        return FALSE;
    }

    TChannel *ptChan = &m_pcMtAdpIns->m_atChannel[VID_RCV_INDEX];
    if(CHAN_CONNECTED != ptChan->m_byState)
    {
        MtAdpLog(LOG_CRITICAL, "[VidNetRcvInit] video rcv chan state is invalid\n");
        return FALSE;
    }

    u16 wRet = 0;

    // xsl [10/26/2006] 仅第一次创建
    if (NULL == m_pcVidNetRcv)
    {
        m_pcVidNetRcv = new CKdvMediaRcv;
        if(NULL == m_pcVidNetRcv)
        {
            MtAdpLog(LOG_CRITICAL, "[VidNetRcvInit] mem allocation failed!\n");
            return FALSE;
        }
        
        wRet = m_pcVidNetRcv->Create(DEF_VID_FRAMESIZE, ProcVidNetRcv, (u32)m_pcMtAdpIns);
        if(MEDIANET_NO_ERROR != wRet)
        {
            MtAdpLog(LOG_CRITICAL, "[VidNetRcvInit] create failed. err no :%u\n", wRet);
            SAFE_DELETE(m_pcVidNetRcv);
            return FALSE;
        }
    }    

    TLocalNetParam tNetParam;
    memset(&tNetParam, 0, sizeof(tNetParam));
    tNetParam.m_tLocalNet.m_dwRTPAddr = ptChan->m_tLogicChan.m_tRcvMediaChannel.GetNetSeqIpAddr();
    tNetParam.m_tLocalNet.m_wRTPPort = ptChan->m_tLogicChan.m_tRcvMediaChannel.GetPort();
    tNetParam.m_tLocalNet.m_dwRTCPAddr = ptChan->m_tLogicChan.m_tRcvMediaCtrlChannel.GetNetSeqIpAddr();
    tNetParam.m_tLocalNet.m_wRTCPPort = ptChan->m_tLogicChan.m_tRcvMediaCtrlChannel.GetPort();
    tNetParam.m_dwRtcpBackAddr = ptChan->m_tLogicChan.m_tSndMediaCtrlChannel.GetNetSeqIpAddr();
    tNetParam.m_wRtcpBackPort = ptChan->m_tLogicChan.m_tSndMediaCtrlChannel.GetPort();
    wRet = m_pcVidNetRcv->SetNetRcvLocalParam(tNetParam);
    if(MEDIANET_NO_ERROR != wRet)
    {
        MtAdpLog(LOG_CRITICAL, "[VidNetRcvInit] SetNetRcvLocalParam failed. err no :%u\n", wRet);
        VidNetRcvQuit();
        return FALSE;
    }

    wRet = m_pcVidNetRcv->StartRcv();
    if(MEDIANET_NO_ERROR != wRet)
    {
        MtAdpLog(LOG_CRITICAL, "[VidNetRcvInit] StartRcv failed. err no :%u\n", wRet);
        VidNetRcvQuit();
        return FALSE;
    }   

    return TRUE;
}

/*=============================================================================
  函 数 名： VidNetRcvQuit
  功    能： 视频接收对象退出
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： BOOL32 
=============================================================================*/
BOOL32 CMtAdpNet::VidNetRcvQuit()
{
    if(NULL == m_pcVidNetRcv)
    {
        return FALSE;
    }
    
    u16 wRet = 0;
    wRet = m_pcVidNetRcv->StopRcv();
    if(MEDIANET_NO_ERROR != wRet)
    {
        MtAdpLog(LOG_CRITICAL, "[VidNetRcvQuit] StopRcv failed. err no :%u\n", wRet);
    }

    wRet = m_pcVidNetRcv->RemoveNetRcvLocalParam();
    if(MEDIANET_NO_ERROR != wRet)
    {
        MtAdpLog(LOG_CRITICAL, "[VidNetRcvQuit] RemoveNetRcvLocalParam failed. err no :%u\n", wRet);
    }

    return TRUE;
}

/*=============================================================================
  函 数 名： AudNetRcvInit
  功    能： 音频接收对象初始化
  算法实现： 
  全局变量： 
  参    数： TLocalNetParam &tNetParam
  返 回 值： BOOL32 
=============================================================================*/
BOOL32 CMtAdpNet::AudNetRcvInit()
{
    if(NULL == m_pcMtAdpIns)
    {
        return FALSE;
    }

    TChannel *ptChan = &m_pcMtAdpIns->m_atChannel[AUD_RCV_INDEX];
    if(CHAN_CONNECTED != ptChan->m_byState)
    {
        MtAdpLog(LOG_CRITICAL, "[AudNetRcvInit] audio rcv chan state is invalid\n");
        return FALSE;
    }

    u16 wRet = 0;

    // xsl [10/26/2006] 仅第一次创建对象
    if (NULL == m_pcAudNetRcv)
    {
        m_pcAudNetRcv = new CKdvMediaRcv;
        if(NULL == m_pcAudNetRcv)
        {
            MtAdpLog(LOG_CRITICAL, "[AudNetRcvInit] mem allocation failed!\n");
            return FALSE;
        }
    
        wRet = m_pcAudNetRcv->Create(DEF_AUD_FRAMESIZE, ProcAudNetRcv, (u32)m_pcMtAdpIns);
        if(MEDIANET_NO_ERROR != wRet)
        {
            MtAdpLog(LOG_CRITICAL, "[AudNetRcvInit] create failed. err no :%u\n", wRet);
            SAFE_DELETE(m_pcAudNetRcv);
            return FALSE;
        }
    }    
    
    TLocalNetParam tNetParam;
    memset(&tNetParam, 0, sizeof(tNetParam));
    tNetParam.m_tLocalNet.m_dwRTPAddr = ptChan->m_tLogicChan.m_tRcvMediaChannel.GetNetSeqIpAddr();
    tNetParam.m_tLocalNet.m_wRTPPort = ptChan->m_tLogicChan.m_tRcvMediaChannel.GetPort();
    tNetParam.m_tLocalNet.m_dwRTCPAddr = ptChan->m_tLogicChan.m_tRcvMediaCtrlChannel.GetNetSeqIpAddr();
    tNetParam.m_tLocalNet.m_wRTCPPort = ptChan->m_tLogicChan.m_tRcvMediaCtrlChannel.GetPort();
    tNetParam.m_dwRtcpBackAddr = ptChan->m_tLogicChan.m_tSndMediaCtrlChannel.GetNetSeqIpAddr();
    tNetParam.m_wRtcpBackPort = ptChan->m_tLogicChan.m_tSndMediaCtrlChannel.GetPort();
    wRet = m_pcAudNetRcv->SetNetRcvLocalParam(tNetParam);
    if(MEDIANET_NO_ERROR != wRet)
    {
        MtAdpLog(LOG_CRITICAL, "[AudNetRcvInit] SetNetRcvLocalParam failed. err no :%u\n", wRet);
        AudNetRcvQuit();
        return FALSE;
    }

    wRet = m_pcAudNetRcv->StartRcv();
    if(MEDIANET_NO_ERROR != wRet)
    {
        MtAdpLog(LOG_CRITICAL, "[AudNetRcvInit] StartRcv failed. err no :%u\n", wRet);
        AudNetRcvQuit();
        return FALSE;
    }

    return TRUE;
}

/*=============================================================================
  函 数 名： AudNetRcvQuit
  功    能： 音频接收对象退出
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void 
=============================================================================*/
BOOL32 CMtAdpNet::AudNetRcvQuit()
{
    if(NULL == m_pcAudNetRcv)
    {
        return FALSE;
    }
    
    u16 wRet = 0;
    wRet = m_pcAudNetRcv->StopRcv();
    if(MEDIANET_NO_ERROR != wRet)
    {
        MtAdpLog(LOG_CRITICAL, "[AudNetRcvQuit] StopRcv failed. err no :%u\n", wRet);
    }

    wRet = m_pcAudNetRcv->RemoveNetRcvLocalParam();
    if(MEDIANET_NO_ERROR != wRet)
    {
        MtAdpLog(LOG_CRITICAL, "[AudNetRcvQuit] RemoveNetRcvLocalParam failed. err no :%u\n", wRet);
    }    
    
    return TRUE;
}

/*=============================================================================
  函 数 名： VidNetSndInit
  功    能： 视频发送对象初始化
  算法实现： 
  全局变量： 
  参    数： TNetSndParam &tNetParam
  返 回 值： BOOL32 
=============================================================================*/
BOOL32 CMtAdpNet::VidNetSndInit()
{
    ENTER(m_semVidSnd);

    if(NULL == m_pcMtAdpIns)
    {
        return FALSE;
    }

    TChannel *ptChan = &m_pcMtAdpIns->m_atChannel[VID_SND_INDEX];
    if(CHAN_CONNECTED != ptChan->m_byState)
    {
        MtAdpLog(LOG_CRITICAL, "[VidNetSndInit] vid snd chan state is invalid\n");
        return FALSE;
    }

    u16 wRet = 0;   
    
     //保存视频发送媒体类型
    m_byVidSndMediaType = ptChan->m_tLogicChan.GetChannelType();

    // xsl [10/26/2006] 仅第一次创建
    if (NULL == m_pcVidNetSnd)
    {
        m_pcVidNetSnd = new CKdvMediaSnd;
        if(NULL == m_pcVidNetSnd)
        {
            MtAdpLog(LOG_CRITICAL, "[VidNetSndInit] mem allocation failed!\n");
            return FALSE;
        }

        wRet = m_pcVidNetSnd->Create(DEF_VID_FRAMESIZE, 
                                 DEF_VID_NETBAND, 
                                 DEF_FRAME_RATE, 
                                 m_byVidSndMediaType);
        if(MEDIANET_NO_ERROR != wRet)
        {
            MtAdpLog(LOG_CRITICAL, "[VidNetSndInit] create failed. err no :%u\n", wRet);
            SAFE_DELETE(m_pcVidNetSnd);
            return FALSE;
        }
    }    
   
    TNetSndParam tSndParam;
    memset(&tSndParam, 0, sizeof(tSndParam));
    tSndParam.m_byNum = 1;
    tSndParam.m_tLocalNet.m_dwRTPAddr = ptChan->m_tSndMediaChannel.GetNetSeqIpAddr();
    tSndParam.m_tLocalNet.m_wRTPPort = ptChan->m_tSndMediaChannel.GetPort();
    tSndParam.m_tLocalNet.m_dwRTCPAddr = ptChan->m_tLogicChan.m_tSndMediaCtrlChannel.GetNetSeqIpAddr();
    tSndParam.m_tLocalNet.m_wRTCPPort = ptChan->m_tLogicChan.m_tSndMediaCtrlChannel.GetPort();
    tSndParam.m_tRemoteNet[0].m_dwRTPAddr = ptChan->m_tLogicChan.m_tRcvMediaChannel.GetNetSeqIpAddr();
    tSndParam.m_tRemoteNet[0].m_wRTPPort = ptChan->m_tLogicChan.m_tRcvMediaChannel.GetPort();
    tSndParam.m_tRemoteNet[0].m_dwRTCPAddr = ptChan->m_tLogicChan.m_tRcvMediaCtrlChannel.GetNetSeqIpAddr();
    tSndParam.m_tRemoteNet[0].m_wRTCPPort = ptChan->m_tLogicChan.m_tRcvMediaCtrlChannel.GetPort();
    wRet = m_pcVidNetSnd->SetNetSndParam(tSndParam);
    if(MEDIANET_NO_ERROR != wRet)
    {
        MtAdpLog(LOG_CRITICAL, "[VidNetSndInit] SetNetSndParam failed. err no :%u\n", wRet);
        return FALSE;
    }

    return TRUE;
}

/*=============================================================================
  函 数 名： VidNetSndQuit
  功    能： 视频发送退出
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： BOOL32 
=============================================================================*/
BOOL32 CMtAdpNet::VidNetSndQuit()
{
    ENTER(m_semVidSnd);

    if(NULL == m_pcVidNetSnd)
    {
        return FALSE;
    }

    u16 wRet = 0;
    wRet = m_pcVidNetSnd->RemoveNetSndLocalParam();
    if(MEDIANET_NO_ERROR != wRet)
    {
        MtAdpLog(LOG_CRITICAL, "[VidNetSndQuit] RemoveNetSndLocalParam failed. err no :%u\n", wRet);
        return FALSE;
    }
    
    return TRUE;
}

/*=============================================================================
  函 数 名： AudNetSndInit
  功    能： 音频发送对象初始化
  算法实现： 
  全局变量： 
  参    数： TNetSndParam &tNetParam
  返 回 值： BOOL32 
=============================================================================*/
BOOL32 CMtAdpNet::AudNetSndInit()
{
    ENTER(m_semAudSnd);

    if(NULL == m_pcMtAdpIns)
    {       
        return FALSE;
    }

    TChannel *ptChan = &m_pcMtAdpIns->m_atChannel[AUD_SND_INDEX];
    if(CHAN_CONNECTED != ptChan->m_byState)
    {
        MtAdpLog(LOG_CRITICAL, "[AudNetSndInit] aud snd chan state is invalid\n");
        return FALSE;
    }
    
    u16 wRet = 0;

    //保存音频发送媒体类型
    m_byAudSndMediaType = ptChan->m_tLogicChan.GetChannelType();  

    // xsl [10/26/2006] 仅第一次创建
    if (NULL == m_pcAudNetSnd)
    {
        m_pcAudNetSnd = new CKdvMediaSnd;
        if(NULL == m_pcAudNetSnd)
        {
            MtAdpLog(LOG_CRITICAL, "[AudNetSndInit] mem allocation failed!\n");
            return FALSE;
        }

        wRet = m_pcAudNetSnd->Create(DEF_AUD_FRAMESIZE, 
                                DEF_AUD_NETBAND, 
                                DEF_FRAME_RATE, 
                                m_byAudSndMediaType);
        if(MEDIANET_NO_ERROR != wRet)
        {
            MtAdpLog(LOG_CRITICAL, "[AudNetSndInit] create failed. err no :%u\n", wRet);
            SAFE_DELETE(m_pcAudNetSnd);
            return FALSE;
        }
    }    
        
    TNetSndParam tSndParam;
    memset(&tSndParam, 0, sizeof(tSndParam));
    tSndParam.m_byNum = 1;
    tSndParam.m_tLocalNet.m_dwRTPAddr = ptChan->m_tSndMediaChannel.GetNetSeqIpAddr();
    tSndParam.m_tLocalNet.m_wRTPPort = ptChan->m_tSndMediaChannel.GetPort();
    tSndParam.m_tLocalNet.m_dwRTCPAddr = ptChan->m_tLogicChan.m_tSndMediaCtrlChannel.GetNetSeqIpAddr();
    tSndParam.m_tLocalNet.m_wRTCPPort = ptChan->m_tLogicChan.m_tSndMediaCtrlChannel.GetPort();    
    tSndParam.m_tRemoteNet[0].m_dwRTPAddr = ptChan->m_tLogicChan.m_tRcvMediaChannel.GetNetSeqIpAddr();
    tSndParam.m_tRemoteNet[0].m_wRTPPort = ptChan->m_tLogicChan.m_tRcvMediaChannel.GetPort();
    tSndParam.m_tRemoteNet[0].m_dwRTCPAddr = ptChan->m_tLogicChan.m_tRcvMediaCtrlChannel.GetNetSeqIpAddr();
    tSndParam.m_tRemoteNet[0].m_wRTCPPort = ptChan->m_tLogicChan.m_tRcvMediaCtrlChannel.GetPort();
    wRet = m_pcAudNetSnd->SetNetSndParam(tSndParam);
    if(MEDIANET_NO_ERROR != wRet)
    {
        MtAdpLog(LOG_CRITICAL, "[AudNetSndInit] SetNetSndParam failed. err no :%u\n", wRet);
        return FALSE;
    }
    
    return TRUE;
}

/*=============================================================================
  函 数 名： AudNetSndQuit
  功    能： 音频发送退出
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： BOOL32 
=============================================================================*/
BOOL32 CMtAdpNet::AudNetSndQuit()
{
    ENTER(m_semAudSnd);

    if(NULL == m_pcAudNetSnd)
    {
        return FALSE;
    }
    
    u16 wRet = 0;
    wRet = m_pcAudNetSnd->RemoveNetSndLocalParam();
    if(MEDIANET_NO_ERROR != wRet)
    {
        MtAdpLog(LOG_CRITICAL, "[AudNetSndQuit] RemoveNetSndLocalParam failed. err no :%u\n", wRet);
        return FALSE;
    }    
    
    return TRUE;
}

//for test
static u8  g_byNetVideo = 1;
static u8  g_byRTPVideo = 1;
API void netvideo(u8 by)
{
    g_byNetVideo = by;
}

API void rtpvideo(u8 by)
{
    g_byRTPVideo = by;
}
/*=============================================================================
  函 数 名： VidNetSnd
  功    能： 视频码流发送
  算法实现： 
  全局变量： 
  参    数： PFRAMEHDR pFrmHdr
             BOOL32 bAvalid 
  返 回 值： BOOL32 
=============================================================================*/
BOOL32 CMtAdpNet::VidNetSnd(u8 *pbyData, u32 dwLen)
{
    ENTER(m_semVidSnd);

    if(NULL == m_pcVidNetSnd)
    {
        return FALSE;
    }
 
    if(0 == g_byRTPVideo)
    {
        return TRUE;
    }

    //rtp切包处理
    s32 nDataLen = 0;
	if(MEDIA_TYPE_H261 == m_byVidSndMediaType)
	{
		if(ERR_OK != H261AddRTPHeader(pbyData, dwLen, m_byRtpPackBuf, 
#ifdef WIN32
            (long *)&nDataLen
#elif defined _LINUX_
            (long *)&nDataLen
#elif defined _VXWORKS_
            &nDataLen
#endif
            ))
		{
			MtAdpLog(LOG_CRITICAL, "[VidNetSnd] H261AddRTPHeader failed!\n");
			return FALSE;
		}
	}
	else if(MEDIA_TYPE_H263 == m_byVidSndMediaType)
	{
		if(DECRTPERR_OK != H263AddRtpHdr(pbyData, dwLen, m_byRtpPackBuf, &nDataLen))
		{
			MtAdpLog(LOG_CRITICAL, "[VidNetSnd] H263AddRtpHdr failed!\n");
			return FALSE;
		}
	}

    if(0 == g_byNetVideo)
    {
        return TRUE;
    }

    //调用net发送码流
    FRAMEHDR hdr;
    memset(&hdr, 0, sizeof(hdr));
    hdr.m_byFrameRate = DEF_FRAME_RATE;
    hdr.m_byMediaType = m_byVidSndMediaType;
    hdr.m_pData = m_byRtpPackBuf;
    hdr.m_dwDataSize = nDataLen;

    u16 wRet = 0;
    wRet = m_pcVidNetSnd->Send(&hdr);
    if(MEDIANET_NO_ERROR != wRet)
    {
        MtAdpLog(LOG_CRITICAL, "[VidNetSnd] send vid data failed. err no :%u\n", wRet);
        return FALSE;
    }

    return TRUE;
}

/*=============================================================================
  函 数 名： AudNetSnd
  功    能： 音频码流发送
  算法实现： 
  全局变量： 
  参    数： PFRAMEHDR pFrmHdr
             BOOL32 bAvalid 
  返 回 值： BOOL32 
=============================================================================*/
BOOL32 CMtAdpNet::AudNetSnd(u8 *pbyData, u32 dwLen)
{
    ENTER(m_semAudSnd);

    if(NULL == m_pcAudNetSnd)
    {
        return FALSE;
    }

    //调用net发送码流
    FRAMEHDR hdr;
    memset(&hdr, 0, sizeof(hdr));
    hdr.m_byMediaType = m_byAudSndMediaType;
    hdr.m_pData = pbyData;
    hdr.m_dwDataSize = dwLen;

    u16 wRet = 0;
    wRet = m_pcAudNetSnd->Send(&hdr);
    if(MEDIANET_NO_ERROR != wRet)
    {
        MtAdpLog(LOG_CRITICAL, "[AudNetSnd] send aud data failed. err no :%u\n", wRet);
        return FALSE;
    }

    return TRUE;
}

/*=============================================================================
  函 数 名： MtAdpLog
  功    能： 调试信息打印输出
  算法实现： 
  全局变量： 
  参    数： u8 byLevel
             s8 * pInfo
             ...
  返 回 值： void 
=============================================================================*/
void CMtAdpNet::MtAdpLog(u8 byLevel, s8 * pInfo, ...)
{
    if(byLevel > g_cMtAdpApp.m_byLogLevel)
    {
        return;
    }

    s8 szBuf[512] = {0};
    s32 nIndex = sprintf(szBuf, "[MtAdpInst %u]", m_pcMtAdpIns->GetInsID());	

    va_list arg;
    va_start(arg, pInfo);
    vsprintf(szBuf+nIndex, pInfo, arg);
    OspPrintf(TRUE, FALSE, szBuf);
    va_end(arg);
}
