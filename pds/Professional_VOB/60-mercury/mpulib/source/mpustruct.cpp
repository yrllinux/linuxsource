/*****************************************************************************
   模块名      : mpulib
   文件名      : mpustruct.cpp
   相关文件    : 
   文件实现功能: mpulib结构和回调实现
   作者        : 周文
   版本        : V4.5  Copyright(C) 2008-2010 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期       版本        修改人      修改内容
   2009/3/14    4.6         张宝卿      注释
******************************************************************************/
#include "mpustruct.h"
#include "mpuinst.h"
//#include "boardagent.h"
//#include "mpuutility.h"

const s8  SECTION_MPUDEBUG[]    = "EQPDEBUG";
const s8  SECTION_VMPSYS[]      = "EQPVMP";
const s8  SECTION_VMPDEFINE_PIC[] = "VMPDEFINEPIC";
s8	MPULIB_CFG_FILE[KDV_MAX_PATH] = {0};

extern BOOL32 g_bPauseSend;

/*=============================================================================
  函 数 名： CBRecvFrame
  功    能： 处理收到的帧数据
  算法实现： 
  全局变量： 
  参    数： PFRAMEHDR pFrmHdr, u32 dwContext
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录:
  日  期      版本        修改人      修改内容
  2008/08/04  4.5         张永强         创建
=============================================================================*/
void CBMpuBasRecvFrame( PFRAMEHDR pFrmHdr, u32 dwContext )
{
	CMpuBasInst * pInst = (CMpuBasInst*)dwContext;
    if ( pFrmHdr == NULL || pInst == NULL )
    {
        mpulog( MPU_CRIT, "[CBRecvFrame] PTFrameHeader 0x%x, dwContext 0x%x.\n", pFrmHdr, dwContext);
        return ;
    }

	CMpuBasAdpGroup *pAdpGroup = pInst->m_pAdpCroup;
	if ( pAdpGroup == NULL )
	{
		mpulog( MPU_CRIT, "[CBRecvFrame] pAdpGroup 0x%x is null.\n", pAdpGroup);
        return ;
    }

	TFrameHeader tFrameHdr;
	memset( &tFrameHdr, 0, sizeof(tFrameHdr) );

	tFrameHdr.m_dwMediaType = pFrmHdr->m_byMediaType;
	tFrameHdr.m_dwFrameID   = pFrmHdr->m_dwFrameID;
    tFrameHdr.m_dwSSRC      = pFrmHdr->m_dwSSRC;
	tFrameHdr.m_pData       = pFrmHdr->m_pData + pFrmHdr->m_dwPreBufSize; 
	tFrameHdr.m_dwDataSize  = pFrmHdr->m_dwDataSize;
	tFrameHdr.m_tVideoParam.m_bKeyFrame    = pFrmHdr->m_tVideoParam.m_bKeyFrame;
	tFrameHdr.m_tVideoParam.m_wVideoHeight = pFrmHdr->m_tVideoParam.m_wVideoHeight;
	tFrameHdr.m_tVideoParam.m_wVideoWidth  = pFrmHdr->m_tVideoParam.m_wVideoWidth;

    mpulogall( "[CBRecvFrame] MediaType:%u, FrameID:%u, KF:%d, W*H:%d*%d, SSRC:%u, DataSize:%u\n",
              tFrameHdr.m_dwMediaType,
              tFrameHdr.m_dwFrameID,
              tFrameHdr.m_tVideoParam.m_bKeyFrame,
              tFrameHdr.m_tVideoParam.m_wVideoWidth,
              tFrameHdr.m_tVideoParam.m_wVideoHeight,              
              tFrameHdr.m_dwSSRC,
              tFrameHdr.m_dwDataSize );

	pAdpGroup->m_cAptGrp.SetData(0, tFrameHdr);

	return;
}

/*=============================================================================
  函 数 名： CBSendFrame
  功    能： 处理适配后的帧数据
  算法实现： 
  全局变量： 
  参    数： PFRAMEHDR pFrmHdr, u32 dwContext
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录:
  日  期      版本        修改人      修改内容
  2008/08/04  4.5         张永强         创建
=============================================================================*/
void CBMpuBasSendFrame( PTFrameHeader pFrmHdr, void* dwContext )
{
    if ( pFrmHdr == NULL || dwContext == NULL )
    {
        mpulog( MPU_CRIT, "[CBSendFrame] PTFrameHeader 0x%x, dwContext 0x%x\n", pFrmHdr, dwContext );
        return ;
    }

    if (g_bPauseSend)
    {
        mpulogall("[CBMpuBasSendFrame] pause send due to debug!\n");
        return;
    }
	CKdvMediaSnd *pMediaSnd = (CKdvMediaSnd*)dwContext;

	FRAMEHDR tTempFrmHdr;
	memset(&tTempFrmHdr, 0x00, sizeof(tTempFrmHdr));

	tTempFrmHdr.m_byMediaType  = (u8)pFrmHdr->m_dwMediaType;
	tTempFrmHdr.m_dwFrameID    = pFrmHdr->m_dwFrameID;
	tTempFrmHdr.m_byFrameRate  = FRAME_RATE;                //帧率固定为25帧

//	tTempFrmHdr.m_dwSSRC       = 0;                         //接收的时候会自动设置
	tTempFrmHdr.m_pData        = pFrmHdr->m_pData;
	tTempFrmHdr.m_dwDataSize   = pFrmHdr->m_dwDataSize;
//	tTempFrmHdr.m_dwPreBufSize = 0;
//	tTempFrmHdr.m_dwTimeStamp  = 0;                         //发送的时候会自动设置
	tTempFrmHdr.m_tVideoParam.m_bKeyFrame    = pFrmHdr->m_tVideoParam.m_bKeyFrame;
	tTempFrmHdr.m_tVideoParam.m_wVideoHeight = pFrmHdr->m_tVideoParam.m_wVideoHeight;
	tTempFrmHdr.m_tVideoParam.m_wVideoWidth  = pFrmHdr->m_tVideoParam.m_wVideoWidth;

	mpulogall( "[CBSendFrame] MediaType:%u, FrameID:%u, KF:%d, W*H:%d*%d, m_dwDataSize:%u\n",
	    	   tTempFrmHdr.m_byMediaType,
		       tTempFrmHdr.m_dwFrameID,
    		   tTempFrmHdr.m_tVideoParam.m_bKeyFrame,
               tTempFrmHdr.m_tVideoParam.m_wVideoWidth,
	    	   tTempFrmHdr.m_tVideoParam.m_wVideoHeight,	    	   
		       tTempFrmHdr.m_dwDataSize );
    

	u16 wRet = pMediaSnd->Send( &tTempFrmHdr );
    if (wRet != MEDIANET_NO_ERROR)
    {
        mpulog(MPU_CRIT, "[CBMpuBasSendFrame] pMediaSnd->Send failed, ret.%d!\n", wRet);
    }

	return;
}

/*=============================================================================
  函 数 名： CBMpuBasAudRecvRtpPk
  功    能： 音频缓冲回调(rtp pack directly)
  算法实现： 
  全局变量： 
  参    数： TRtpPack *pRtpPack, u32 dwContext
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录:
  日  期      版本        修改人      修改内容
  2009/07/29  4.6         张宝卿         创建
=============================================================================*/
void CBMpuBasAudRecvRtpPk(TRtpPack *pRtpPack, u32 dwContext)
{
    CMpuBasInst * pInst = (CMpuBasInst*)dwContext;
    if ( pRtpPack == NULL || pInst == NULL )
    {
        mpulog( MPU_CRIT, "[CBAudRecvFrame] pRtpPack 0x%x, dwContext 0x%x.\n", pRtpPack, dwContext);
        return ;
    }
    
    CMpuBasAdpGroup *pAdpGroup = pInst->m_pAdpCroup;
    if ( pAdpGroup == NULL )
    {
        mpulog( MPU_CRIT, "[CBAudRecvFrame] pAdpGroup 0x%x is null.\n", pAdpGroup);
        return ;
    }

    TRtpPkQueue *ptQueue = pInst->m_ptFrmQueue;
    if ( NULL == ptQueue )
    {
        mpulog( MPU_CRIT, "[CBAudRecvFrame] ptQueue 0x%x is null.\n", ptQueue);
        return ;
    }
    
    mpulogall( "[CBAudRecvFrame] MediaType:%u, RealSize:%u, ExSize:%u, SSRC.%d, Sequence:%d, Timestamp.%d\n",
                pRtpPack->m_byPayload,
                pRtpPack->m_nRealSize,
                pRtpPack->m_nExSize,
                pRtpPack->m_dwSSRC,
                pRtpPack->m_wSequence,
                pRtpPack->m_dwTimeStamp);

    if ((u32)pRtpPack->m_nRealSize + (u32) pRtpPack->m_nExSize * sizeof(u32) > MAXNUM_BUF_LEN)
    {
        OspPrintf(TRUE, FALSE, "[CBAudRecvFrame] RtpPk<Type.%d, RealSize.%d, ExSize.%d, SSRC.%d, Seq.%d, TimeStamp.%d> unexpected, ignore it!\n",
                                pRtpPack->m_byPayload,
                                pRtpPack->m_nRealSize,
                                pRtpPack->m_nExSize,
                                pRtpPack->m_dwSSRC,
                                pRtpPack->m_wSequence,
                                pRtpPack->m_dwTimeStamp);
        return;
    }


    if (!ptQueue->IsQueueFull())
    {
        u8 abyBuf[MAXNUM_BUF_LEN];
        memset(abyBuf, 0, sizeof(abyBuf));
        
        //RealSize
        memcpy(abyBuf,
               pRtpPack->m_pRealData,
               min(pRtpPack->m_nRealSize, MAXNUM_BUF_LEN-1));
        
        //ExSize
        if (0 != pRtpPack->m_nExSize)
        {
            memcpy(&abyBuf[pRtpPack->m_nRealSize], 
                    pRtpPack->m_pExData,
                    min( (u32) pRtpPack->m_nExSize*sizeof(u32), MAXNUM_BUF_LEN-1-(u32)pRtpPack->m_nRealSize));
        }

		s32 nLen = sizeof(u32);
        ptQueue->IncBuf(abyBuf, pRtpPack->m_nRealSize + pRtpPack->m_nExSize*nLen, *pRtpPack);
        mpulogall("[CBAudRecvFrame] RtpPk<Type.%d, RealSize.%d, ExSize.%d, SSRC.%d, Seq.%d, TimeStamp.%d> IncBuf for chn.%d!\n",
                    pRtpPack->m_byPayload,
                    pRtpPack->m_nRealSize,
                    pRtpPack->m_nExSize,
                    pRtpPack->m_dwSSRC,
                    pRtpPack->m_wSequence,
                    pRtpPack->m_dwTimeStamp,
                    pInst->GetInsID()-1);
    }
    else
    {
        //记录当前包
        u8 abyBufIn[MAXNUM_BUF_LEN];
        memset(abyBufIn, 0, sizeof(abyBufIn));

        //RealSize
        if (NULL != pRtpPack->m_pRealData)
        {
            memcpy(abyBufIn,
                   pRtpPack->m_pRealData,
                   min(pRtpPack->m_nRealSize, MAXNUM_BUF_LEN-1));
        }
        else
        {
            OspPrintf(TRUE, FALSE, "[CBAudRecvFrame] RtpPk<Type.%d, RealSize.%d, ExSize.%d, SSRC.%d, Seq.%d, TimeStamp.%d> pRtpPack->m_pRealData == NULL!\n",
                                    pRtpPack->m_byPayload,
                                    pRtpPack->m_nRealSize,
                                    pRtpPack->m_nExSize,
                                    pRtpPack->m_dwSSRC,
                                    pRtpPack->m_wSequence,
                                    pRtpPack->m_dwTimeStamp);
        }


        //ExSize
        if (0 != pRtpPack->m_nExSize)
        {
            if (NULL != pRtpPack->m_pExData)
            {
                memcpy(&abyBufIn[pRtpPack->m_nRealSize], 
                        pRtpPack->m_pExData,
                        min((u32) pRtpPack->m_nExSize*sizeof(u32), MAXNUM_BUF_LEN-1-(u32)pRtpPack->m_nRealSize));
            }
            else
            {
                OspPrintf(TRUE, FALSE, "[CBAudRecvFrame] RtpPk<Type.%d, RealSize.%d, ExSize.%d, SSRC.%d, Seq.%d, TimeStamp.%d> pRtpPack->m_pExData == NULL!\n",
                                        pRtpPack->m_byPayload,
                                        pRtpPack->m_nRealSize,
                                        pRtpPack->m_nExSize,
                                        pRtpPack->m_dwSSRC,
                                        pRtpPack->m_wSequence,
                                        pRtpPack->m_dwTimeStamp);
            }
        }

        TRtpPack tRtpPkIn = *pRtpPack;

        //取队头发送
        u8 abyBufOut[MAXNUM_BUF_LEN];
        memset(abyBufOut, 0, sizeof(abyBufOut));
        u32 dwBufHdLen = MAXNUM_BUF_LEN;
        TRtpPack tRtpPkOut;
        
        ptQueue->DecBuf(abyBufOut, dwBufHdLen, tRtpPkOut);

        mpulogall("[CBAudRecvFrame] RtpPk<Type.%d, RealSize.%d, ExSize.%d, SSRC.%d, Seq.%d, TimeStamp.%d> DecBuf for chn.%d!\n",
                    pRtpPack->m_byPayload,
                    pRtpPack->m_nRealSize,
                    pRtpPack->m_nExSize,
                    pRtpPack->m_dwSSRC,
                    pRtpPack->m_wSequence,
                    pRtpPack->m_dwTimeStamp,
                    pInst->GetInsID()-1
                   );
        
        //RealData
        tRtpPkOut.m_pRealData = abyBufOut;

        //ExData
        tRtpPkOut.m_pExData = abyBufOut + tRtpPkOut.m_nRealSize;

        //保护队头帧可能是空帧
        if (0 != tRtpPkOut.m_nRealSize)
        {
            pAdpGroup->m_pcMediaAudSnd[0]->Send(&tRtpPkOut);
            pAdpGroup->m_pcMediaAudSnd[1]->Send(&tRtpPkOut);
        }

		s32 nLen = sizeof(u32);
        //保存当前帧
        ptQueue->IncBuf(abyBufIn, tRtpPkIn.m_nRealSize + tRtpPkIn.m_nExSize*nLen, tRtpPkIn);
        
        mpulogall( "[CBAudRecvFrame] RtpPk<Type.%d, RealSize.%d, ExSize.%d, SSRC.%d, Seq.%d, TimeStamp.%d> IncBuf insteadly for chn.%d!\n",
                    pRtpPack->m_byPayload,
                    pRtpPack->m_nRealSize,
                    pRtpPack->m_nExSize,
                    pRtpPack->m_dwSSRC,
                    pRtpPack->m_wSequence,
                    pRtpPack->m_dwTimeStamp,
                    pInst->GetInsID()-1
                    );
    }
    return;
}

/*=============================================================================
  函 数 名： Create
  功    能： 从配置文件读取码率作弊值
  算法实现： 
  全局变量： 
  参    数： void
  返 回 值： void 
=============================================================================*/
BOOL CMpuBasAdpGroup::Create( /*u32 dwMaxRcvFrameSize, */ 
		 		              u16 wRtcpBackPort,    //rtcp回发端口
			                  u16 wLocalStartPort,  //为本地接收的rtp端口，rtcp端口为rtp端口+1
			                  u32 dwDestIp,         //发送目的Ip地址，
	                          u16 dwDestPort,       //第一路发送目的端口，第二路发送端口为第一路发送目的端口 + 10
						      u32 dwContext,		//适应码流透传，传入实例指针
                              u8  byChnId )         //适配通道Id ( 0,1,2,3 )

{
	m_dwNetBand        = 8000000;
	m_dwRtcpBackAddr   = htonl( dwDestIp );
	m_wRtcpBackPort    = wRtcpBackPort;
	m_wLocalStartPort  = wLocalStartPort;
	m_dwDestAddr       = htonl( dwDestIp );
	m_wDestPort        = dwDestPort;

    s8 achRtcpIpStr[17] = {0};
    memcpy( achRtcpIpStr, StrOfIP(m_dwRtcpBackAddr), sizeof(achRtcpIpStr));

    mpulog( MPU_INFO, "[CHdAdpGroup::Create] m_dwNetBand:%d m_dwRtcpBackAddr:%s, m_wRtcpBackPort:%u m_wLocalStartPort:%u, m_dwDestAddr:%s m_wDestPort:%d\n",
           m_dwNetBand,
           achRtcpIpStr,
           m_wRtcpBackPort,
           m_wLocalStartPort,
           StrOfIP(m_dwDestAddr),
		   m_wDestPort );

//  pengjie[8/7/2009] 修改创建对象失败处理问题
    m_pcMediaRcv    = new CKdvMediaRcv;    
	if ( NULL == m_pcMediaRcv )
	{
		printf( "CMpu2BasAdpGroup::Create--> new CKdvMediaRcv failed!\n" );
		return FALSE;
	}
	
	//创建音频码流接收对象
    m_pcMediaAudRcv = new CKdvMediaRcv;
	if ( NULL == m_pcMediaAudRcv )
	{
		printf( "CMpu2BasAdpGroup::Create--> new CKdvMediaRcv failed!\n" );
		return FALSE;
	}

	//zjl 根据bas启动模式获取初始化输出通道数
	u8  byOutChnNum = 0;
	if (!GetOutputChnNumAcd2WorkMode(g_cMpuBasApp.m_byWorkMode, byOutChnNum))
	{
		OspPrintf(TRUE, FALSE, "[CMpuBasAdpGroup][Create] GetOutputChnNumAcd2WorkMode failed!\n");
		return FALSE;
	}

	u8 byIdx = 0;	
	for (byIdx = 0; byIdx < byOutChnNum/*MAXNUM_VOUTPUT*/; byIdx++)
	{
		m_pcMediaSnd[byIdx] = new CKdvMediaSnd;
		m_pcMediaAudSnd[byIdx] = new CKdvMediaSnd;
		
		if ((NULL == m_pcMediaSnd[byIdx]) || (NULL == m_pcMediaAudSnd[byIdx]))
		{
			delete m_pcMediaRcv;
			m_pcMediaRcv    = NULL;
			delete m_pcMediaAudRcv;
			m_pcMediaAudRcv = NULL;
			
			for(u8 byIndex = 0; byIndex < byIdx+1; byIndex++)
			{
				delete m_pcMediaSnd[byIndex];
				m_pcMediaSnd[byIndex] = NULL;
				delete m_pcMediaAudSnd[byIndex];
				m_pcMediaAudSnd[byIndex] = NULL;
			}
			printf( "CMpuBasAdpGroup::Create--> new CKdvMediaSnd[%d] failed!\n", byIdx );
			return FALSE;
		}				
	}
  
	u16 wRet = 0;
	do {
		wRet = m_cAptGrp.CreateGroup( byChnId );
		if( (u16)Codec_Success != wRet )
		{
			mpulog( MPU_CRIT, "CMpuBasAdpGroup::Create-->m_cAptGrp.CreateGroup fail, Error code is: %d \n", wRet );
            printf( "CMpuBasAdpGroup::Create-->m_cAptGrp.CreateGroup fail, Error code is: %d \n", wRet );
			break;
		}
        else
        {
            printf( "CMpuBasAdpGroup::Create-->m_cAptGrp.CreateGroup succeed!\n" );
		}


        //u8 *pbyTest = new u8[1024];
		
		printf("before create!\n");
		wRet = m_pcMediaRcv->Create(MAX_VIDEO_FRAME_SIZE,
			                        CBMpuBasRecvFrame,   
				    			    (u32)dwContext );
		printf("after create!---wRet :%d\n",wRet);
        if( MEDIANET_NO_ERROR != wRet )
		{
			printf("before destroy!--wRet:%d\n",wRet);
			m_cAptGrp.DestroyGroup(); 
			printf("after destroy!\n");
			mpulog( MPU_CRIT, "CMpuAdpGroup::Create-->m_pcMediaRcv->Create fail, Error code is: %d \n", wRet );
            printf(  "CMpuAdpGroup::Create-->m_pcMediaRcv->Create fail, Error code is: %d \n", wRet );
			break;
		}

		wRet = m_pcMediaAudRcv->Create(MAX_AUDIO_FRAME_SIZE,
			                           CBMpuBasAudRecvRtpPk,   
				    			       (u32)dwContext );

        if( MEDIANET_NO_ERROR != wRet )
		{
			m_cAptGrp.DestroyGroup(); 
			mpulog( MPU_CRIT, "CMpuAdpGroup::Create-->m_pcMediaAudRcv->Create fail, Error code is: %d \n", wRet );
            printf(  "CMpuAdpGroup::Create-->m_pcMediaAudRcv->Create fail, Error code is: %d \n", wRet );
			break;
		}
		
		//音频接收参数设置
		TLocalNetParam tlocalNetParm;
        memset( &tlocalNetParm, 0, sizeof(TLocalNetParam) );
        tlocalNetParm.m_tLocalNet.m_wRTPPort  = m_wLocalStartPort + 2;
		tlocalNetParm.m_tLocalNet.m_wRTCPPort = m_wLocalStartPort + 1 + 2;
        tlocalNetParm.m_dwRtcpBackAddr        = m_dwDestAddr;
        tlocalNetParm.m_wRtcpBackPort         = wRtcpBackPort + 2;

		printf("before SetRcv!\n");
        wRet = m_pcMediaAudRcv->SetNetRcvLocalParam( tlocalNetParm );
		printf("after SetRcv! --ret: %d\n",wRet);
        if( MEDIANET_NO_ERROR !=  wRet )
        {
            m_cAptGrp.DestroyGroup();
            mpulog( MPU_CRIT, "CMpuAdpGroup::Create-->m_pcMediaAudRcv->SetNetRcvLocalParam fail, Error code is: %d \n", wRet );
            printf( "CMpuAdpGroup::Create-->m_pcMediaAudRcv->SetNetRcvLocalParam fail, Error code is: %d \n", wRet );
            break;
		}

        memset( &tlocalNetParm, 0, sizeof(TLocalNetParam) );
        tlocalNetParm.m_tLocalNet.m_wRTPPort  = m_wLocalStartPort;
		tlocalNetParm.m_tLocalNet.m_wRTCPPort = m_wLocalStartPort + 1;
       
        tlocalNetParm.m_dwRtcpBackAddr        = m_dwDestAddr;
        tlocalNetParm.m_wRtcpBackPort         = wRtcpBackPort;

		printf("before SetRcv!\n");
        wRet = m_pcMediaRcv->SetNetRcvLocalParam( tlocalNetParm );
		printf("after SetRcv! --ret: %d\n",wRet);
        if( MEDIANET_NO_ERROR !=  wRet )
        {
            m_cAptGrp.DestroyGroup();
            mpulog( MPU_CRIT, "CMpuAdpGroup::Create-->m_pcMediaRcv->SetNetRcvLocalParam fail, Error code is: %d \n", wRet );
            printf( "CMpuAdpGroup::Create-->m_pcMediaRcv->SetNetRcvLocalParam fail, Error code is: %d \n", wRet );
            break;
		}


        // 设置编码回调
		
		if (TYPE_MPUBAS== g_cMpuBasApp.m_byWorkMode)
		{
			wRet = m_cAptGrp.SetVidDataCallback( 0, ADAPTER_CODECID_ENCHD, CBMpuBasSendFrame, m_pcMediaSnd[0] );
			if ( (u16)Codec_Success != wRet )
			{
				mpulog( MPU_CRIT, "CMpuBasAdpGroup::Great-->m_cAptGrp.SetVidDataCallback(0 ,0 ) fail, Error code is: %d \n", wRet );
				printf("CMpuBasAdpGroup::Great-->m_cAptGrp.SetVidDataCallback(0 ,0 ) fail, Error code is: %d \n", wRet);
				break;
			}
			
			wRet = m_cAptGrp.SetVidDataCallback( 0, ADAPTER_CODECID_ENCSD, CBMpuBasSendFrame, m_pcMediaSnd[1] );
			if ( (u16)Codec_Success != wRet )
			{
				mpulog( MPU_CRIT, "CMpuBasAdpGroup::Great-->m_cAptGrp.SetVidDataCallback(0 ,1 ) fail, Error is: %d \n", wRet );
				printf("CMpuBasAdpGroup::Great-->m_cAptGrp.SetVidDataCallback(0 ,1 ) fail, Error is: %d \n", wRet);
				break;
			}
		}
		else if (TYPE_MPUBAS_H == g_cMpuBasApp.m_byWorkMode)
		{
			for (u8 byOutIdx = 0; byOutIdx < MAXNUM_MPU_H_VOUTPUT; byOutIdx++)
			{
				wRet = m_cAptGrp.SetVidDataCallback( 0, ADAPTER_CODECID_HD1080 + byOutIdx, CBMpuBasSendFrame, m_pcMediaSnd[byOutIdx] );
				if ( (u16)Codec_Success != wRet )
				{
					mpulog( MPU_CRIT, "CMpuBasAdpGroup::Great-->m_cAptGrp.SetVidDataCallback(0 ,0 ) fail, Error code is: %d \n", wRet );
					printf("CMpuBasAdpGroup::Great-->m_cAptGrp.SetVidDataCallback(0 ,0 ) fail, Error code is: %d \n", wRet);
					break;
				}
			}			
		}
        
		
        // 接收HDFlag为TRUE
        m_pcMediaRcv->SetHDFlag( TRUE );

		return TRUE;
	}
	while ( 0 );

	return FALSE;
}

/*=============================================================================
  函 数 名： StartAdapter
  功    能： 
  算法实现： 
  全局变量： 
  参    数： void
  返 回 值： void 
=============================================================================*/
BOOL CMpuBasAdpGroup::StartAdapter( BOOL32 bStartMediaRcv )
{
	mpulog( MPU_INFO, "CHdAdpGroup::StartAdapter\n" );
	u16 wRet = 0;

	if ( m_bIsStart )
	{
    	mpulog(MPU_INFO, "CHdAdpGroup::StartAdapter success\n" );
		return TRUE;
	}	
	wRet = m_cAptGrp.StartGroup();

	if ( bStartMediaRcv )
	{
		m_pcMediaRcv->StartRcv();
        m_pcMediaAudRcv->StartRcv();
	}

	if( (u16)Codec_Success != wRet )
	{
		m_pcMediaRcv->StopRcv();
        mpulog( MPU_CRIT, "CHdAdpGroup::StartAdapter-->m_cAptGrp.StartGroup fail, Error code is: %d \n", wRet );
		return FALSE;
	}

	m_bIsStart = TRUE;

	return TRUE;
}


/*=============================================================================
  函 数 名： SetVideoEncSend
  功    能： 设置第一路编码码流的发送对象
  算法实现： 
  全局变量： 
  参    数： void
  返 回 值： void 
=============================================================================*/
BOOL CMpuBasAdpGroup::SetVideoEncSend( u8 byIdx, u32 dwNetBand,  u16 wLoacalStartPort,  u32 dwDestIp,  u16 dwDestPort )
{
	mpulog( MPU_INFO, "[SetVideoEncSend] wLoacalStartPort:%u, dest:%s:%u\n", wLoacalStartPort, StrOfIP(dwDestIp), dwDestPort );
	
    u16 wRet = 0;

	do
	{
        u8 byFrameRate = m_atVidEncParam[byIdx/*0*/].m_byFrameRate;
    	u8 byMediaType = m_atVidEncParam[byIdx/*0*/].m_byEncType;

        //zbq[10/10/2008] 适应MCU的群组优化调整，作参数保护
        //逻辑错误？待续
		//20100514 zjl 帧率及带宽不在此处保护，由mcu业务保障
//         if ( byFrameRate > 60 )
//         {
//             byFrameRate = 30;
//         }
//         if ( dwNetBand > 8000000 )
//         {
//             if ( m_atVidEncParam[1].m_wBitRate <= 8128 )
//             {
//                 dwNetBand = m_atVidEncParam[1].m_wBitRate * 1024;
//             }
//             else
//             {
//                 dwNetBand = 0;
//             }
//         }

    	wRet = m_pcMediaSnd[byIdx/*0*/]->Create( MAX_VIDEO_FRAME_SIZE,
		    	                        dwNetBand,
			                            byFrameRate,
			    				        byMediaType );

        if ( MEDIANET_NO_ERROR != wRet )
        {
            m_cAptGrp.DestroyGroup();
            mpulog( MPU_CRIT, "CMpuBasAdpGroup::SetVideoEncSend-->m_cMediaSnd[%d].Create fail, Error code is:%d, NetBand.%d, FrmRate.%d, MediaType.%d\n", byIdx, wRet, dwNetBand, byFrameRate, byMediaType );
            printf( "CMpuBasAdpGroup::SetVideoEncSend-->m_cMediaSnd[%d].Create fail, Error code is: %d \n", byIdx,  wRet );
    	   	break;
        }
        else
        {
            mpulog( MPU_INFO, "CMpuBasAdpGroup::SetVideoEncSend-->m_cMediaSnd[%d].0x%x.Create succeed, NetBand.%d, FrmRate.%d, MediaType.%d\n", byIdx, m_pcMediaSnd[byIdx], dwNetBand, byFrameRate, byMediaType );
            printf( "CMpuBasAdpGroup::SetVideoEncSend-->m_cMediaSnd[%d].Create succeed!\n", byIdx);
        }
		
		//根据mpu工作模式设置hdflag
		if (TYPE_MPUBAS == g_cMpuBasApp.m_byWorkMode)
		{
			//bap-4模式mpu一进两出，第一路tos为高清，第二路dsp设标清
			if (0 == byIdx)
			{
				m_pcMediaSnd[byIdx]->SetHDFlag( TRUE );
			}
			else if (1 ==  byIdx)
			{
				m_pcMediaSnd[byIdx]->SetHDFlag( FALSE );
			}
		}
		else if(TYPE_MPUBAS_H == g_cMpuBasApp.m_byWorkMode)
		{
			//bap-2模式mpu一进6出，前四路h264tos为高清，后两路otherd在dsp上为标清
			if (byIdx < 4)
			{
				m_pcMediaSnd[byIdx]->SetHDFlag( TRUE );
			}
			else
			{
				m_pcMediaSnd[byIdx]->SetHDFlag( FALSE );
			}
		}
		else
		{
			OspPrintf(TRUE, FALSE, "[SetVideoEncSend] unexpected mpu workmode:%d!\n", g_cMpuBasApp.m_byWorkMode);
			return FALSE;
		}
		

    	//设置发送参数
    	TNetSndParam tNetSndPar;
    	memset( &tNetSndPar, 0x00, sizeof(tNetSndPar) );

    	tNetSndPar.m_byNum  = 1;
    	tNetSndPar.m_tLocalNet.m_wRTPPort       = wLoacalStartPort;
     	tNetSndPar.m_tLocalNet.m_wRTCPPort      = wLoacalStartPort + 1;
    	tNetSndPar.m_tRemoteNet[0].m_dwRTPAddr  = dwDestIp;
    	tNetSndPar.m_tRemoteNet[0].m_wRTPPort   = dwDestPort;
    	tNetSndPar.m_tRemoteNet[0].m_dwRTCPAddr = dwDestIp;
    	tNetSndPar.m_tRemoteNet[0].m_wRTCPPort  = dwDestPort + 1;

    	wRet = m_pcMediaSnd[byIdx]->SetNetSndParam( tNetSndPar );

    	if ( MEDIANET_NO_ERROR != wRet )
		{
    		m_cAptGrp.DestroyGroup();
    		mpulog( MPU_CRIT, "CMpubasAdpGroup::Great-->m_cMediaSnd[%d].SetNetSndParam fail, Error code is: %d \n", byIdx, wRet );
            printf( "CMpuBasAdpGroup::Great-->m_cMediaSnd[%d].SetNetSndParam fail, Error code is: %d \n", byIdx, wRet );
    		break;
		}
	
        /*
    	//设置编码回调
    	wRet = m_cAptGrp.SetVidDataCallback( 0, 0, CBMpuBasSendFrame, &(m_pcMediaSnd[0]) );

    	if ( Codec_Success != wRet )
		{
    		mpulog( MPU_CRIT, "CMpuBasAdpGroup::Great-->m_cAptGrp.SetVidDataCallback(0 ,0 ) fail, Error code is: %d \n", wRet );
    		break;
		}*/

		return TRUE;

	} while ( 0 );

	return FALSE;
}

/*=============================================================================
  函 数 名： SetSecVideoEncSend
  功    能： 设置第二路编码码流的发送对象
  算法实现： 
  全局变量： 
  参    数： void
  返 回 值： void 
=============================================================================*/
// BOOL CMpuBasAdpGroup::SetSecVideoEncSend( u32 dwNetBand, u16 wLoacalStartPort, u32 dwDestIp, u16 dwDestPort )
// {
// 	mpulog( MPU_INFO, "[SetSecVideoEncSend] wLoacalStartPort:%u, dest:%s:%u\n", wLoacalStartPort, StrOfIP(dwDestIp), dwDestPort );
// 
// 	u16 wRet = 0;
// 
// 	do
// 	{        
//         u8 byFrameRate = m_atVidEncParam[1].m_byFrameRate;
//     	u8 byMediaType = m_atVidEncParam[1].m_byEncType;
// 
//         //zbq[10/10/2008] 适应MCU的群组优化调整，作参数保护
//         //逻辑问题？待续！
//         if ( byFrameRate > 60 )
//         {
//             byFrameRate = 5;
//         }
//         if ( dwNetBand > 8000000 )
//         {
//             if ( m_atVidEncParam[0].m_wBitRate <= 8128 )
//             {
//                 dwNetBand = m_atVidEncParam[0].m_wBitRate * 1024;
//             }
//             else
//             {
//                 dwNetBand = 0;
//             }
//         }
// 
//     	wRet = m_pcMediaSnd[1]->Create( MAX_VIDEO_FRAME_SIZE,
// 		    	                        dwNetBand,
// 			                            byFrameRate,
// 			    				        byMediaType );
// 
//         if ( MEDIANET_NO_ERROR != wRet )
// 		{
//          	m_cAptGrp.DestroyGroup();
//     		mpulog( MPU_CRIT, "CMpuBasAdpGroup::Great-->m_cMediaSnd[1].Create fail, Error is: %d \n", wRet );
//             printf( "CMpubasAdpGroup::Great-->m_cMediaSnd[1].Create fail, Error is: %d \n", wRet );
//     	   	break;
// 		}
//         else
//         {
//             mpulog( MPU_INFO, "CMpuBasAdpGroup::SetVideoEncSend-->m_cMediaSnd[1].0x%x.Create succeed, NetBand.%d, FrmRate.%d, MediaType.%d\n", m_pcMediaSnd[1], dwNetBand, byFrameRate, byMediaType );
//             printf( "CMpuBasAdpGroup::SetVideoEncSend-->m_cMediaSnd[1].Create succeed!\n" );
//         }
// 
// 		//不需要判断是否设置高清标志
// 		m_pcMediaSnd[1]->SetHDFlag( FALSE );
// 
//     	//设置发送参数
//     	TNetSndParam tNetSndPar;
//     	memset( &tNetSndPar, 0x00, sizeof(tNetSndPar) );
// 
//     	tNetSndPar.m_byNum  = 1;
//     	tNetSndPar.m_tLocalNet.m_wRTPPort       = wLoacalStartPort;
//      	tNetSndPar.m_tLocalNet.m_wRTCPPort      = wLoacalStartPort + 1;
//     	tNetSndPar.m_tRemoteNet[0].m_dwRTPAddr  = dwDestIp;
//     	tNetSndPar.m_tRemoteNet[0].m_wRTPPort   = dwDestPort;
//     	tNetSndPar.m_tRemoteNet[0].m_dwRTCPAddr = dwDestIp;
//     	tNetSndPar.m_tRemoteNet[0].m_wRTCPPort  = dwDestPort + 1;
// 
//     	wRet = m_pcMediaSnd[1]->SetNetSndParam( tNetSndPar );
// 
//     	if ( MEDIANET_NO_ERROR != wRet )
// 		{
//     		m_cAptGrp.DestroyGroup();
//     		mpulog( MPU_CRIT, "CMpuBasAdpGroup::Great-->m_cMediaSnd[0].SetNetSndParam fail, Error is: %d \n", wRet );
//             printf( "CMpuBasAdpGroup::Great-->m_cMediaSnd[0].SetNetSndParam fail, Error is: %d \n", wRet );
//     		break;
// 		}
// 	
//         /*
//     	//设置编码回调
//     	wRet = m_cAptGrp.SetVidDataCallback( 0, 1, CBMpuBasSendFrame, &(m_pcMediaSnd[1]) );
// 
//     	if ( Codec_Success != wRet )
// 		{
//     		mpulog( MPU_CRIT, "CMpuBasAdpGroup::Great-->m_cAptGrp.SetVidDataCallback(0 ,1 ) fail, Error is: %d \n", wRet );
//     		break;
// 		}*/
// 
// 		return TRUE;
// 
// 	} while ( 0 );
// 
// 	return FALSE;
// }

/*=============================================================================
  函 数 名： SetAudioSend
  功    能： 音频空回调发送 支持
  算法实现： 
  全局变量： 
  参    数： void
  返 回 值： void 
=============================================================================*/
BOOL CMpuBasAdpGroup::SetAudioSend(u8 byIdx, u32 dwNetBand, u16 wLocalStartPort, u32 dwDestIp, u16 wDestPort)
{
	mpulog( MPU_INFO, "[SetAudioSend] wLoacalStartPort:%u, dest:%s:%u\n", 
                       wLocalStartPort, StrOfIP(dwDestIp), wDestPort );

	u16 wRet = 0;

	do
	{        
        u8 byFrameRate = 50;

        //zbq[10/10/2008] 适应MCU的群组优化调整，作参数保护
        //逻辑问题？待续！
//20100514 zjl 帧率及带宽不在此处保护，由mcu业务保障
//         if ( byFrameRate > 60 )
//         {
//             byFrameRate = 5;
//         }
//         if ( dwNetBand > 8000000 )
//         {
//             dwNetBand = 96 * 1024;
//         }

    	wRet = m_pcMediaAudSnd[byIdx]->Create( MAX_AUDIO_FRAME_SIZE,
		    	                        dwNetBand,
			                            byFrameRate,
			    				        m_byAudDecPT );

        if ( MEDIANET_NO_ERROR != wRet )
		{
         	m_cAptGrp.DestroyGroup();
    		mpulog( MPU_CRIT, "CMpuBasAdpGroup::Great-->m_pcMediaAudSnd[%d].Create fail, Error is:%d, NetBand.%d, FrmRate.%d, MediaType.%d \n", byIdx, wRet, dwNetBand, byFrameRate, m_byAudDecPT );
            printf( "CMpubasAdpGroup::Great-->m_pcMediaAudSnd[0].Create fail, Error is: %d \n", wRet );
    	   	break;
		}
        else
        {
            mpulog( MPU_INFO, "CMpuBasAdpGroup::SetVideoEncSend-->m_pcMediaAudSnd[%d].0x%x.Create succeed, NetBand.%d, FrmRate.%d, MediaType.%d\n", byIdx, m_pcMediaSnd[1], dwNetBand, byFrameRate, m_byAudDecPT );
            printf( "CMpuBasAdpGroup::SetVideoEncSend-->m_pcMediaAudSnd[%d].Create succeed!\n" , byIdx);
        }

    	//设置发送参数
    	TNetSndParam tNetSndPar;
    	memset( &tNetSndPar, 0x00, sizeof(tNetSndPar) );

    	tNetSndPar.m_byNum  = 1;
    	tNetSndPar.m_tLocalNet.m_wRTPPort       = wLocalStartPort;
     	tNetSndPar.m_tLocalNet.m_wRTCPPort      = wLocalStartPort + 1;
    	tNetSndPar.m_tRemoteNet[0].m_dwRTPAddr  = dwDestIp;
    	tNetSndPar.m_tRemoteNet[0].m_wRTPPort   = wDestPort;
    	tNetSndPar.m_tRemoteNet[0].m_dwRTCPAddr = dwDestIp;
    	tNetSndPar.m_tRemoteNet[0].m_wRTCPPort  = wDestPort + 1;

    	wRet = m_pcMediaAudSnd[byIdx]->SetNetSndParam( tNetSndPar );

    	if ( MEDIANET_NO_ERROR != wRet )
		{
    		m_cAptGrp.DestroyGroup();
    		mpulog( MPU_CRIT, "CMpuBasAdpGroup::Great-->m_cMediaSnd[%d].SetNetSndParam fail, Error is: %d \n", byIdx, wRet );
            printf( "CMpuBasAdpGroup::Great-->m_cMediaSnd[%d].SetNetSndParam fail, Error is: %d \n", byIdx, wRet );
    		break;
		}

		return TRUE;

	} while ( 0 );

	return FALSE;
}
// 
// BOOL CMpuBasAdpGroup::SetSecAudioSend(u32 dwNetBand, u16 wLocalStartPort, u32 dwDestIp, u16 wDestPort)
// {
//     mpulog( MPU_INFO, "[SetAudioSend] wLoacalStartPort:%u, dest:%s:%u\n", 
//                        wLocalStartPort, StrOfIP(dwDestIp), wDestPort );
//     
//     u16 wRet = 0;
//     
//     do
//     {        
//         u8 byFrameRate = 50;
//         
//         //zbq[10/10/2008] 适应MCU的群组优化调整，作参数保护
//         //逻辑问题？待续！
//         if ( byFrameRate > 60 )
//         {
//             byFrameRate = 5;
//         }
//         if ( dwNetBand > 8000000 )
//         {
//             dwNetBand = 96 * 1024;
//         }
//         
//         wRet = m_pcMediaAudSnd[1]->Create( MAX_AUDIO_FRAME_SIZE,
//                                             dwNetBand,
//                                             byFrameRate,
//                                             m_byAudDecPT );
//         
//         if ( MEDIANET_NO_ERROR != wRet )
//         {
//             m_cAptGrp.DestroyGroup();
//             mpulog( MPU_CRIT, "CMpuBasAdpGroup::Great-->m_pcMediaAudSnd[1].Create fail, Error is:%d, NetBand.%d, FrmRate.%d, MediaType.%d\n", wRet, dwNetBand, byFrameRate, m_byAudDecPT );
//             printf( "CMpubasAdpGroup::Great-->m_pcMediaAudSnd[1].Create fail, Error is: %d \n", wRet );
//             break;
//         }
//         else
//         {
//             mpulog( MPU_INFO, "CMpuBasAdpGroup::SetVideoEncSend-->m_pcMediaAudSnd[1].0x%x.Create succeed, NetBand.%d, FrmRate.%d, MediaType.%d\n", m_pcMediaSnd[1], dwNetBand, byFrameRate, m_byAudDecPT );
//             printf( "CMpuBasAdpGroup::SetVideoEncSend-->m_pcMediaAudSnd[1].Create succeed!\n" );
//         }
//         
//         //设置发送参数
//         TNetSndParam tNetSndPar;
//         memset( &tNetSndPar, 0x00, sizeof(tNetSndPar) );
//         
//         tNetSndPar.m_byNum  = 1;
//         tNetSndPar.m_tLocalNet.m_wRTPPort       = wLocalStartPort;
//         tNetSndPar.m_tLocalNet.m_wRTCPPort      = wLocalStartPort + 1;
//         tNetSndPar.m_tRemoteNet[0].m_dwRTPAddr  = dwDestIp;
//         tNetSndPar.m_tRemoteNet[0].m_wRTPPort   = wDestPort;
//         tNetSndPar.m_tRemoteNet[0].m_dwRTCPAddr = dwDestIp;
//         tNetSndPar.m_tRemoteNet[0].m_wRTCPPort  = wDestPort + 1;
//         
//         wRet = m_pcMediaAudSnd[1]->SetNetSndParam( tNetSndPar );
//         
//         if ( MEDIANET_NO_ERROR != wRet )
//         {
//             m_cAptGrp.DestroyGroup();
//             mpulog( MPU_CRIT, "CMpuBasAdpGroup::Great-->m_cMediaSnd[1].SetNetSndParam fail, Error is: %d \n", wRet );
//             printf( "CMpuBasAdpGroup::Great-->m_cMediaSnd[1].SetNetSndParam fail, Error is: %d \n", wRet );
//             break;
//         }
//         
//         return TRUE;
//         
//     } while ( 0 );
//     
// 	return FALSE;
// }

/*=============================================================================
  函 数 名： SetNetRecvFeedbackVideoParam
  功    能： 设置网络接收重传参数
  算法实现： 
  全局变量： 
  参    数： void
  返 回 值： void 
=============================================================================*/
BOOL  CMpuBasAdpGroup::SetNetRecvFeedbackVideoParam( TRSParam tNetRSParam, BOOL32 bRepeatSnd/* = FALSE */ )
{
	u16 wRet = 0;

	//保存参数
    m_bNeedRS = bRepeatSnd;
	memcpy( &m_tRsParam, &tNetRSParam, sizeof(m_tRsParam) );

    wRet = m_pcMediaRcv->ResetRSFlag( tNetRSParam, bRepeatSnd );
	if ( MEDIANET_NO_ERROR != wRet )
	{
		mpulog( MPU_CRIT, "CMpuBasAdpGroup::SetNetRecvFeedbackVideoParam-->m_pcMediaRcv->ResetRSFlag fail,Error code is:%d\n", wRet );
		return FALSE;
	}

	//nzj:添加音频接收 2010/9/6
	wRet = m_pcMediaAudRcv->ResetRSFlag( tNetRSParam, bRepeatSnd );
	if ( MEDIANET_NO_ERROR != wRet )
	{
		mpulog( MPU_CRIT, "CMpuBasAdpGroup::SetNetRecvFeedbackVideoParam-->m_pcMediaAudRcv->ResetRSFlag fail,Error code is:%d\n", wRet );
		return FALSE;
	}

	return TRUE;
}

/*=============================================================================
  函 数 名： SetNetSendFeedbackVideoParam
  功    能： 设置图像的网络发送重传参数
  算法实现： 
  全局变量： 
  参    数： void
  返 回 值： void 
=============================================================================*/
BOOL  CMpuBasAdpGroup::SetNetSendFeedbackVideoParam( u16 wBufTimeSpan, u8 byVailedOutNum, BOOL32 bRepeatSnd/*= FALSE*/ )
{
	u16 wRet = 0;
	if (0 == byVailedOutNum || byVailedOutNum > MAXNUM_MPU_H_VOUTPUT)
	{
		OspPrintf(TRUE, FALSE, "[SetNetSendFeedbackVideoParam] byVailedOutNum is %d!\n", byVailedOutNum);
	}
	//保存参数
	for (u8 byIdx = 0; byIdx < byVailedOutNum; byIdx++)
	{
		//未创建发送对象的不设重传参数
		if (0 == m_atVidEncParam[byIdx].m_wBitRate 
			||  MEDIA_TYPE_NULL == m_atVidEncParam[byIdx].m_byEncType
			||  0 == m_atVidEncParam[byIdx].m_byFrameRate)
		{
			continue;
		}

		m_awBufTime[byIdx] = wBufTimeSpan;
		
		wRet = m_pcMediaSnd[byIdx]->ResetRSFlag( wBufTimeSpan, bRepeatSnd );
		if ( MEDIANET_NO_ERROR != wRet )
		{
			mpulog( MPU_CRIT, "CMpuBasAdpGroup::SetNetSendFeedbackVideoParam-->m_cMediaSnd[%d].ResetRSFlag fail,Error code is:%d\n", byIdx, wRet );
			return FALSE;
		}		
		//nzj:添加音频发送 2010/9/6
		wRet = m_pcMediaAudSnd[byIdx]->ResetRSFlag( wBufTimeSpan, bRepeatSnd );
		if ( MEDIANET_NO_ERROR != wRet )
		{
			mpulog( MPU_CRIT, "CMpuBasAdpGroup::SetNetSendFeedbackVideoParam-->m_pcMediaAudSnd[%d].ResetRSFlag fail,Error code is:%d\n", byIdx, wRet );
			return FALSE;
		}

	}
// 	m_awBufTime[0] = wBufTimeSpan;
// 	
//     wRet = m_pcMediaSnd[0]->ResetRSFlag( wBufTimeSpan, bRepeatSnd );
// 	if ( MEDIANET_NO_ERROR != wRet )
// 	{
// 		mpulog( MPU_CRIT, "CMpuBasAdpGroup::SetNetSendFeedbackVideoParam-->m_cMediaSnd[0].ResetRSFlag fail,Error code is:%d\n", wRet );
// 		return FALSE;
// 	}

	return TRUE;
}

/*=============================================================================
  函 数 名： SetNetSecSendFeedbackVideoParam
  功    能： 设置图像的网络发送重传参数
  算法实现： 
  全局变量： 
  参    数： void
  返 回 值： void 
=============================================================================*/
BOOL  CMpuBasAdpGroup::SetNetSecSendFeedbackVideoParam( u16 wBufTimeSpan, BOOL32 bRepeatSnd/*= FALSE*/ )
{
	u16 wRet = 0;
	
	//保存参数
   	m_awBufTime[1] = wBufTimeSpan;
    
    wRet = m_pcMediaSnd[1]->ResetRSFlag( wBufTimeSpan, bRepeatSnd );
    if ( MEDIANET_NO_ERROR != wRet )
	{
     	mpulog( MPU_CRIT, "CMpuBasAdpGroup::SetNetSendFeedbackVideoParam-->m_cMediaSnd[1].ResetRSFlag fail,Error code is:%d\n", wRet );
    	return FALSE;
	}

	return FALSE;
}

/*=============================================================================
  函 数 名： SetSmoothSendRule
  功    能： 
  算法实现： 
  全局变量： 
  参    数： void
  返 回 值： void 
=============================================================================*/
// void CMpuBasAdpGroup::SetSmoothSendRule( void )
// {
// #ifdef _LINUX_
// 	u8 byOutNum = 0;
// 	if(!GetOutputChnNumAcd2WorkMode(g_cMpuBasApp.m_byWorkMode, byOutNum))
// 	{
// 		OspPrintf(TRUE, FALSE, "[SetSmoothSendRule] GetOutputChnNumAcd2WorkMode failed!\n");
// 		return;
// 	}
// 	for (u8 byOutIdx = 0; byOutIdx < byOutNum; byOutIdx++)
// 	{
// 		if (0 == m_atVidEncParam[byOutIdx].m_wBitRate)
// 		{
// 			continue;
// 		}
// 		u16 wBitrate = m_atVidEncParam[byOutIdx].m_wBitRate;
// 		// 还原打折前码率，平滑参数不能按照编码码率设置
// 		wBitrate = g_cMpuBasApp.GetOrigRate(wBitrate);
// 		
// 		wBitrate = wBitrate >> 3;
// 		u32 dwPeak = wBitrate + wBitrate / 5;   // add 20% 
// 		
// 		mpulog( MPU_INFO, "[SetSmoothSendRule] Setting OutIdx:%d ssrule for 0x%x:%u, rate/peak=%u/%u KByte.\n",
// 				byOutIdx, m_dwDestAddr, m_wDestPort + PORTSPAN * byOutIdx, wBitrate, dwPeak );
// 		
// 		
// 		s32 nRet = SetSSRule( m_dwDestAddr, m_wDestPort + PORTSPAN * byOutIdx, wBitrate, dwPeak, 2 );   // default 2 second
// 		if ( 0 == nRet )
// 		{
// 			mpulog(MPU_CRIT, "[SetSmoothSendRule] Set OutIdx:%d rule failed. SSErrno=%d\n", byOutIdx, nRet );
// 		}
// 	}
// //   u16 wBitrate = m_atVidEncParam[0].m_wBitRate;
// // 	// 还原打折前码率，平滑参数不能按照编码码率设置
// // 	wBitrate = g_cMpuBasApp.GetOrigRate(wBitrate);
// // 
// //     wBitrate = wBitrate >> 3;
// //     u32 dwPeak = wBitrate + wBitrate / 5;   // add 20% 
// // 
// //     mpulog( MPU_INFO, "[SetSmoothSendRule] Setting ssrule for 0x%x:%u, rate/peak=%u/%u KByte.\n",
// //                        m_dwDestAddr, m_wDestPort, wBitrate, dwPeak );
// // 
// // 
// //     s32 nRet = SetSSRule( m_dwDestAddr, m_wDestPort, wBitrate, dwPeak, 2 );   // default 2 second
// //     if ( 0 == nRet )
// //     {
// //         mpulog(MPU_CRIT, "[SetSmoothSendRule] Set rule failed. SSErrno=%d\n", nRet );
// //     }
// // 
// //     wBitrate = m_atVidEncParam[1].m_wBitRate;
// // 	// 还原打折前码率，平滑参数不能按照编码码率设置
// // 	wBitrate = g_cMpuBasApp.GetOrigRate(wBitrate);
// // 
// // 	wBitrate = wBitrate >> 3;
// //     dwPeak = wBitrate + wBitrate / 5;   // add 20% 
// //     mpulog(MPU_INFO, "[SetSmoothSendRule] Setting ssrule for 0x%x:%d, rate/peak=%u/%u KByte.\n",
// //                       m_dwDestAddr, m_wDestPort + PORTSPAN, wBitrate, dwPeak );
// // 
// //     nRet = SetSSRule( m_dwDestAddr, m_wDestPort + PORTSPAN, wBitrate, dwPeak, 2 );   // default 2 second
// //     if ( 0 == nRet )
// // 	{
// //         mpulog(MPU_CRIT, "[SetSmoothSendRule] Set rule failed. SSErrno=%d\n", nRet );
// // 	}
// 
// #endif
// }

/*=============================================================================
  函 数 名： StopSmoothSend
  功    能： 
  算法实现： 
  全局变量： 
  参    数： void
  返 回 值： void 
=============================================================================*/
// void CMpuBasAdpGroup::StopSmoothSend()
// {
//     mpulog( MPU_INFO, "[StopSmoothSend] Stopping SmoothSend\n" );
// 
// #ifdef _LINUX_
//    s32 nRet = CloseSmoothSending();
//     if ( 0 == nRet )
//     {
//         mpulog(MPU_CRIT, "[ClearSmoothSendRule] CloseSmoothSending failed. SSErrno=%d\n", nRet );
//    }
// #endif 	
// }

/*=============================================================================
函 数 名： SetSmoothSendRule
功    能： 设置（清除）本适配对象的平滑发送规则
算法实现： 
全局变量： BOOL32 bIsStart 
TRUE：设置规则； FALSE：清除规则
参    数： void
返 回 值： void 
-------------------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
20101230    4.6         pengjie       create
=============================================================================*/
void CMpuBasAdpGroup::SetSmoothSendRule( BOOL32 bNeedSet )
{
	mpulog( MPU_INFO, "[SetSmoothSendRule] IsSet.%d!\n", bNeedSet );
	
#ifdef _LINUX_
	u8 byOutNum = 0;
	if(!GetOutputChnNumAcd2WorkMode(g_cMpuBasApp.m_byWorkMode, byOutNum))
	{
		OspPrintf(TRUE, FALSE, "[SetSmoothSendRule] GetOutputChnNumAcd2WorkMode failed!\n");
		return;
	}
	
	if( bNeedSet )
	{
		for( u8 byOutIdx = 0; byOutIdx < byOutNum; byOutIdx++ )
		{
			u16 wBitrate = m_atVidEncParam[byOutIdx].m_wBitRate;
			// 还原打折前码率，平滑参数不能按照编码码率设置
			wBitrate = g_cMpuBasApp.GetOrigRate(wBitrate);
			// 转换成Byte
			wBitrate = wBitrate >> 3;
			// 峰值再增加20%
			u32 dwPeak = wBitrate + wBitrate / 5;
			
			mpulog( MPU_INFO, "[SetSmoothSendRule] Setting OutIdx:%d ssrule for 0x%x:%u, rate/peak=%u/%u KByte.\n",
				byOutIdx, m_dwDestAddr, m_wDestPort + PORTSPAN * byOutIdx, wBitrate, dwPeak );
			
			s32 nRet = SetSSRule( m_dwDestAddr, m_wDestPort + PORTSPAN * byOutIdx, wBitrate, dwPeak, 2 );   // default 2 second
			
			if ( 0 == nRet )
			{
				mpulog(MPU_CRIT, "[SetSmoothSendRule] Set OutIdx:%d rule failed. SSErrno=%d\n", byOutIdx, nRet );
			}
		}
	}
	else
	{
		for( u8 byOutIdx = 0; byOutIdx < byOutNum; byOutIdx++ )
		{
			s32 nRet = UnsetSSRule( m_dwDestAddr, m_wDestPort + PORTSPAN * byOutIdx );
			
			if ( 0 == nRet )
			{
				mpulog(MPU_CRIT, "[ClearSmoothSendRule] Clear OutIdx:%d rule failed. SSErrno=%d\n", byOutIdx, nRet );
			}
		}
	}
#endif
	
	return;
}

/*=============================================================================
  函 数 名： StopAdapter
  功    能： 
  算法实现： 
  全局变量： 
  参    数： void
  返 回 值： void 
=============================================================================*/
BOOL CMpuBasAdpGroup::StopAdapter( BOOL32 bStopMediaRcv )
{
	u16 wRet = 0;
	mpulog( MPU_INFO, "CMpuBasAdpGroup::StopAdapter\n" );
	if(!m_bIsStart)
	{
    	mpulog( MPU_INFO, "CMpuBasAdpGroup::StopAdapter success\n" );
		return TRUE;
	}
	if ( bStopMediaRcv )
	{
		m_pcMediaRcv->StopRcv();
	}

	wRet = m_cAptGrp.StopGroup();

	if ( (u16)Codec_Success != wRet )
	{
        mpulog( MPU_CRIT, "CMpuBasAdpGroup::StopAdapter-->m_cAptGrp.StopGroup fail, Error code is: %d \n", wRet );
		return FALSE;
	}

	m_bIsStart = FALSE;
   	mpulog( MPU_INFO, "CMpuBasAdpGroup::StopAdapter success\n" );	
	return TRUE;
}

/*=============================================================================
  函 数 名： SetFastUpdata
  功    能： 
  算法实现： 
  全局变量： 
  参    数： u8 byChnIdx: 某通道
  返 回 值： void 
=============================================================================*/
BOOL CMpuBasAdpGroup::SetFastUpdata( u8 byChnIdx )
{
	//u32 dwType = 0;
	mpulog( MPU_INFO, "CMpuBasAdpGroup::SetFastUpdata(%d)！\n",byChnIdx );

    u16 dwErrorcode = CODEC_NO_ERROR;
    dwErrorcode = m_cAptGrp.SetFastUpdata(byChnIdx);
    if (dwErrorcode != CODEC_NO_ERROR)
    {
        mpulog( MPU_CRIT, "[SetFastUpdata] For chn.%d fail(errcode:%d)\n", byChnIdx, dwErrorcode);
        return FALSE;
	}
	return TRUE;
}

/*=============================================================================
  函 数 名： GetVideoChannelStatis
  功    能： 
  算法实现： 
  全局变量： 
  参    数： void
  返 回 值： void 
=============================================================================*/
BOOL CMpuBasAdpGroup::GetVideoChannelStatis( u8 byChnNo, TAdapterChannelStatis &tAdapterChannelStatis )
{
	u16 wRet = 0;
	wRet = m_cAptGrp.GetVideoChannelStatis( byChnNo,tAdapterChannelStatis );

	if ( (u16)Codec_Success != wRet )
	{
		mpulog( MPU_CRIT,"CMpuBasAdpGroup::GetVideoChannelStatis-->m_cAptGrp.GetVideoChannelStatis fail! Error code is:%d\n", wRet );
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}

/*=============================================================================
  函 数 名： ShowChnInfo
  功    能： 
  算法实现： 
  全局变量： 
  参    数： void
  返 回 值： void 
=============================================================================*/
void CMpuBasAdpGroup::ShowChnInfo( void )
{
    OspPrintf(TRUE, FALSE, "\n----------------Adapter Group Info-------------------------\n");
    
    //基本信息
    OspPrintf(TRUE, FALSE, "\n*************Basic info*************\n");
    //OspPrintf(TRUE, FALSE, "\t Type:%d\n",   m_byType);
    OspPrintf(TRUE, FALSE, "\t Is Start:%d\n", m_bIsStart);	
    if ( !m_bIsStart) 
    {
        return;
    }
    
    OspPrintf(TRUE, FALSE, "\t Net Band:%u\n", m_dwNetBand);
    
    //解码参数
    OspPrintf(TRUE, FALSE, "\n*************DEC Parmater***********\n");
    u8 abyDecKey[64];
    s32 iLen = 0;
    m_tMediaDec.GetEncryptKey(abyDecKey, &iLen);
    abyDecKey[iLen] = 0;
    
    OspPrintf(TRUE, FALSE, "\t DEC key:%s\n", abyDecKey);
    OspPrintf(TRUE, FALSE, "\t DEC key len:%d\n", iLen);
    OspPrintf(TRUE, FALSE, "\t DEC mode:%u\n", m_tMediaDec.GetEncryptMode());
    OspPrintf(TRUE, FALSE, "\t Active payload:%u\n", m_tDbPayload.GetActivePayload());
    OspPrintf(TRUE, FALSE, "\t Real payload:%u\n", m_tDbPayload.GetRealPayLoad());
    
    //编码参数
    OspPrintf(TRUE, FALSE, "\n*************ENC Parmater***********\n");
    
    for(u32 dwIdx = 0; dwIdx < MAXNUM_VOUTPUT; dwIdx++)
    {
        if (0 == dwIdx)
        {
            OspPrintf(TRUE, FALSE, "\nThe first:\n");
        }
        else
        {
            OspPrintf(TRUE, FALSE, "\nThe second:\n");
        }
        OspPrintf(TRUE, FALSE, "\t VideoWidth:%u\n", m_atVidEncParam[dwIdx].m_wVideoWidth);
        OspPrintf(TRUE, FALSE, "\t VideoHeight:%u\n", m_atVidEncParam[dwIdx].m_wVideoHeight);
        OspPrintf(TRUE, FALSE, "\t MaxKeyFrameInterval:%u\n", m_atVidEncParam[dwIdx].m_byMaxKeyFrameInterval);
        OspPrintf(TRUE, FALSE, "\t BitRate:%u\n",m_atVidEncParam[dwIdx].m_wBitRate);
        OspPrintf(TRUE, FALSE, "\t EncType:%u\n", m_atVidEncParam[dwIdx].m_byEncType);
        OspPrintf(TRUE, FALSE, "\t RcMode:%u\n", m_atVidEncParam[dwIdx].m_byRcMode);
        OspPrintf(TRUE, FALSE, "\t MaxQuant:%u\n", m_atVidEncParam[dwIdx].m_byMaxQuant);
        OspPrintf(TRUE, FALSE, "\t MinQuant:%u\n", m_atVidEncParam[dwIdx].m_byMinQuant);
        OspPrintf(TRUE, FALSE, "\t FrameRate:%u\n", m_atVidEncParam[dwIdx].m_byFrameRate);
        OspPrintf(TRUE, FALSE, "\t FrameFmt:%u\n", m_atVidEncParam[dwIdx].m_byFrameFmt);
        
        m_atMediaEnc[dwIdx].GetEncryptKey(abyDecKey, &iLen);
        abyDecKey[iLen] = 0;
        OspPrintf(TRUE, FALSE, "\n");
        OspPrintf(TRUE, FALSE, "\t ENC key:%s\n", abyDecKey);
        OspPrintf(TRUE, FALSE, "\t ENC key len:%d\n", iLen);
        OspPrintf(TRUE, FALSE, "\t ENC mode:%u\n", m_atMediaEnc[dwIdx].GetEncryptMode());
    }    
	m_cAptGrp.ShowChnInfo(0, TRUE);

    return;
}

/*=============================================================================
  函 数 名： SetAudioParam
  功    能： 保存需要缓冲的中转音频的载荷
  算法实现： 
  全局变量： 
  参    数： void
  返 回 值： BOOL 
=============================================================================*/
BOOL CMpuBasAdpGroup::SetAudioParam(u8 byAudDecType, u8 byChnIdx)
{
    if (m_byAudDecPT == byAudDecType)
    {
        return TRUE;
    }

    m_byAudDecPT = byAudDecType;

    if (MEDIA_TYPE_NULL == byAudDecType)
    {
        mpulog( MPU_CRIT, "CMpuBasAdpGroup::SetAudioParam-->m_byAudDecPT is NULL, ignore Aud!\n" );
        return FALSE;
    }
	u8 byOutNum = 0;
	if(!GetOutputChnNumAcd2WorkMode(g_cMpuBasApp.m_byWorkMode, byOutNum))
	{
		mpulog( MPU_CRIT, "GetOutputChnNumAcd2WorkMode failed!\n" );
		return FALSE;
	}

	for (u8 byOutIdx = 0; byOutIdx < byOutNum; byOutIdx++)
	{
		if (!SetAudioSend(  byOutIdx,
							m_dwNetBand,
							//PORT_SNDBIND + byChnIdx * PORTSPAN + 4,
							g_cMpuBasApp.m_wMcuRcvStartPort + PORTSPAN * (byChnIdx * byOutNum + byOutIdx)  + 4,
							m_dwDestAddr,
							m_wDestPort + PORTSPAN * byOutIdx + 2) )
		{
			mpulog( MPU_CRIT, "CMpuBasAdpGroup::SetAudioParam-->SetAudioSend fail\n" );
			return FALSE;
		}
	}
    
    
//     if (!SetSecAudioSend(m_dwNetBand,
//                         //PORT_SNDBIND + byChnIdx * PORTSPAN + 6,
// 						g_cMpuBasApp.m_wMcuRcvStartPort + PORTSPAN * ( byChnIdx * 2 + 1 ) + 6,
//                         m_dwDestAddr,
//                         m_wDestPort + PORTSPAN + 2))
//     {
//         mpulog( MPU_CRIT, "CMpuBasAdpGroup::SetAudioParam-->SetSecAudioSend fail\n" );
//         return FALSE;
//     }

    return TRUE;
}

/*=============================================================================
  函 数 名： SetVideoEncParam
  功    能： 
  算法实现： 
  全局变量： 
  参    数： void
  返 回 值： void 
=============================================================================*/
BOOL CMpuBasAdpGroup::SetVideoEncParam( TVideoEncParam * ptVidEncParam,
                                        u8               byChnIdx,
										u8				 byVailedChnNum,
									    BOOL32		     bSetNetParam )
{
	mpulog( MPU_INFO, "CMpuAdpGroup::SetVideoEncParam\n" );
	if ( NULL == ptVidEncParam )
	{
		mpulog( MPU_CRIT, "CMpuAdpGroup::ChangeVideoEncParam-->bad paramter!\n" );
		return FALSE;
	}

    const u8 byChnNo = 0; //被固定

	u16 wRet = 0;
	do
	{
        memcpy( m_atVidEncParam, ptVidEncParam, sizeof(TVideoEncParam) * byVailedChnNum );

		//mpu_4模式暂时用老的策略
		if (TYPE_MPUBAS == g_cMpuBasApp.m_byWorkMode)
		{
			if ( m_atVidEncParam[1].m_wVideoHeight == 0 && m_atVidEncParam[1].m_wVideoWidth ==0 )
			{
        		wRet = m_cAptGrp.ChangeVideoEncParam( byChnNo, ptVidEncParam/*m_atVidEncParam*/, 1 );
			}
			else
			{
				wRet = m_cAptGrp.ChangeVideoEncParam( byChnNo, ptVidEncParam/*m_atVidEncParam*/, 2 );
			}
		}
		//mpu_2模式下，有效通道参数可以不必理会，底层直接取6
		else if(TYPE_MPUBAS_H == g_cMpuBasApp.m_byWorkMode)
		{
			wRet = m_cAptGrp.ChangeVideoEncParam( byChnNo, ptVidEncParam, byVailedChnNum );
		}
		
    	if ( (u16)Codec_Success != wRet )
		{
            mpulog( MPU_CRIT, "CMpuBasAdpGroup::SetVideoEncParam-->m_cAptGrp.ChangeVideoEncParam fail, Error code is: %d \n", wRet );
            break;
		}

		//网络参数在开启的时候作单次设置就可以了
		if ( bSetNetParam )
		{

			//???这里的byVailedChnNum是否默认为6或者填实际有效通道数
			for (u8 byIdx = 0; byIdx < byVailedChnNum; byIdx++)
			{
				if (0 == m_atVidEncParam[byIdx].m_wBitRate 
					||  MEDIA_TYPE_NULL == m_atVidEncParam[byIdx].m_byEncType
					||  0 == m_atVidEncParam[byIdx].m_byFrameRate)
				{
					continue;
				}

				u16	wLocalSendPort =0;
				wLocalSendPort = g_cMpuBasApp.m_wMcuRcvStartPort + PORTSPAN * (byChnIdx * byVailedChnNum + byIdx);
				if( !SetVideoEncSend(byIdx, m_dwNetBand, wLocalSendPort, m_dwDestAddr, m_wDestPort + byIdx*PORTSPAN) )
				{
					mpulog( MPU_CRIT, "CMpuBasAdpGroup::ChangeVideoEncParam-->SetVideoEncSend <OutIdx:%d>fail\n", byIdx);
					break;
				}
				mpulog(MPU_INFO, "[SetVideoEncParam]SetVideoEncSend Idx:%d, LocalStartPort:%u, DestPort:%d\n", 
									byIdx,	g_cMpuBasApp.m_wMcuRcvStartPort + PORTSPAN * (byChnIdx * byVailedChnNum + byIdx),
									m_wDestPort + byIdx*PORTSPAN);
			}
// 			//if( !SetVideoEncSend(m_dwNetBand, PORT_SNDBIND + byChnIdx * PORTSPAN, m_dwDestAddr, m_wDestPort) )
// 			if( !SetVideoEncSend(m_dwNetBand, g_cMpuBasApp.m_wMcuRcvStartPort + PORTSPAN * byChnIdx * byVailedChnNum , m_dwDestAddr, m_wDestPort) )
// 			{
// 				mpulog( MPU_CRIT, "CMpuBasAdpGroup::ChangeVideoEncParam-->SetVideoEncSend fail\n" );
// 				break;
// 			}
// 			
// 
// 			//if( !SetSecVideoEncSend(m_dwNetBand, PORT_SNDBIND + byChnIdx * PORTSPAN + 2, m_dwDestAddr, m_wDestPort + PORTSPAN) )
// 			if( !SetSecVideoEncSend(m_dwNetBand, g_cMpuBasApp.m_wMcuRcvStartPort + PORTSPAN * ( byChnIdx * 2 + 1 ), m_dwDestAddr, m_wDestPort + PORTSPAN) )
// 			{
// 				mpulog( MPU_CRIT, "CMpuBasAdpGroup::ChangeVideoEncParam-->SetVideoEncSend fail\n" );
// 				break;
// 			}
		}

		return TRUE;

	} while ( 0 );
	
	return FALSE;
}
/*=============================================================================
  函 数 名： ModNetSndIpAddr
  功    能： 
  算法实现： 
  全局变量： 
  参    数： void
  返 回 值： void 
=============================================================================*/
void CMpuBasAdpGroup::ModNetSndIpAddr(u32 dwSndIp)
{
	m_dwDestAddr = htonl(dwSndIp);

	u8 byOutNum = 0;
	if(!GetOutputChnNumAcd2WorkMode(g_cMpuBasApp.m_byWorkMode, byOutNum))
	{
		OspPrintf(TRUE, FALSE, "[ModNetSndIpAddr] GetOutputChnNumAcd2WorkMode failed!\n");
	}

	TNetSndParam tSndParam;
	for (u8 byIdx = 0; byIdx < byOutNum/*2*/; byIdx++)
	{
		if (m_pcMediaSnd[byIdx] != NULL)
		{
			m_pcMediaSnd[byIdx]->GetNetSndParam(&tSndParam);
			tSndParam.m_tRemoteNet[0].m_dwRTPAddr  = m_dwDestAddr;
			tSndParam.m_tRemoteNet[0].m_dwRTCPAddr = m_dwDestAddr;
			m_pcMediaSnd[byIdx]->SetNetSndParam(tSndParam);
		}

		if (m_pcMediaAudSnd[byIdx] != NULL)
		{
			m_pcMediaAudSnd[byIdx]->GetNetSndParam(&tSndParam);
			tSndParam.m_tRemoteNet[0].m_dwRTPAddr  = m_dwDestAddr;
			tSndParam.m_tRemoteNet[0].m_dwRTCPAddr = m_dwDestAddr;
			m_pcMediaAudSnd[byIdx]->SetNetSndParam(tSndParam);
		}

	}


}

/*=============================================================================
  函 数 名： ReadDebugValues
  功    能： 从配置文件读取码率作弊值
  算法实现： 
  全局变量： 
  参    数： void
  返 回 值： void 
=============================================================================*/
void CMpuBasCfg::ReadDebugValues()
{
    s32 nValue;

    ::GetRegKeyInt( MPULIB_CFG_FILE, SECTION_MPUDEBUG, "AudioDeferNum", 12, &nValue );
    m_tDebugVal.SetAudioDeferNum((u32)nValue);

    ::GetRegKeyInt( MPULIB_CFG_FILE, SECTION_MPUDEBUG, "IsEnableRateAutoDec", 1, &nValue );
    m_tDebugVal.SetEnableBitrateCheat( (nValue==0) ? FALSE : TRUE );

    ::GetRegKeyInt( MPULIB_CFG_FILE, SECTION_MPUDEBUG, "DefaultTargetRatePct", 75, &nValue );
    m_tDebugVal.SetDefaultRate( (u16)nValue );

	//  [11/9/2009 pengjie] 是否加黑边或裁边
    ::GetRegKeyInt( MPULIB_CFG_FILE, SECTION_MPUDEBUG, "VidEncHWMode", 0, &nValue );
	if( nValue != 0 && nValue != 1 && nValue != 2)
	{
		nValue = 0;
	}
    m_tDebugVal.SetVidEncHWMode(nValue);

	
	//  [nizhijun 2011/03/21] 设置BAS定时检测关键帧时间间隔
	::GetRegKeyInt( MPULIB_CFG_FILE, SECTION_MPUDEBUG, "BASIframeInterval", 1000, &nValue );
    m_tDebugVal.SetBasIframeInterval(nValue);

    return;
}

/*=============================================================================
  函 数 名： FreeStatusDataA
  功    能： 清除状态参数
  算法实现： 
  全局变量： 
  参    数： void
  返 回 值： void 
=============================================================================*/
void CMpuBasCfg::FreeStatusDataA( void )
{
    m_dwMcuNode = INVALID_NODE;
    m_dwMcuIId  = INVALID_INS;
    return;
}

/*=============================================================================
  函 数 名： FreeStatusDataB
  功    能： 清除状态参数
  算法实现： 
  全局变量： 
  参    数： void
  返 回 值： void 
=============================================================================*/
void CMpuBasCfg::FreeStatusDataB( void )
{
    m_dwMcuNodeB = INVALID_NODE;
    m_dwMcuIIdB  = INVALID_INS;
    return;
}

////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////以下为vmp部分////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////



/*=============================================================================
  函 数 名： CBMpuSVmpRecvFrame
  功    能： 处理收到的帧数据
  算法实现： 
  全局变量： 
  参    数： PFRAMEHDR pFrmHdr, u32 dwContext
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录:
  日  期      版本        修改人      修改内容
  2008/08/04  4.5         张永强         创建
=============================================================================*/
void CBMpuSVmpRecvFrame( PFRAMEHDR pFrmHdr, u32 dwContext )
{
	TMpuSVmpRcvCB* pRcvCB = (TMpuSVmpRcvCB*)dwContext;
    if ( pFrmHdr == NULL || pRcvCB == NULL )
    {
        mpulog( MPU_CRIT, "[mpu][error] PTFrameHeader 0x%x, dwContext 0x%x.\n", pFrmHdr, dwContext );
        return;
    }

	CHardMulPic* pHardMulPic = &(pRcvCB->m_pThis->m_cHardMulPic);
	if ( pHardMulPic == NULL )
	{
		mpulog( MPU_CRIT, "[mpu][error] pAdpGroup 0x%x is null.\n", pHardMulPic );
        return;
    }

	TFrameHeader tFrameHdr;
	memset( &tFrameHdr, 0, sizeof(tFrameHdr) );

	tFrameHdr.m_dwMediaType = pFrmHdr->m_byMediaType;
	tFrameHdr.m_dwFrameID   = pFrmHdr->m_dwFrameID;
    tFrameHdr.m_dwSSRC      = pFrmHdr->m_dwSSRC;
	tFrameHdr.m_pData       = pFrmHdr->m_pData + pFrmHdr->m_dwPreBufSize; 
	tFrameHdr.m_dwDataSize  = pFrmHdr->m_dwDataSize;
	tFrameHdr.m_tVideoParam.m_bKeyFrame    = pFrmHdr->m_tVideoParam.m_bKeyFrame;
	tFrameHdr.m_tVideoParam.m_wVideoHeight = pFrmHdr->m_tVideoParam.m_wVideoHeight;
	tFrameHdr.m_tVideoParam.m_wVideoWidth  = pFrmHdr->m_tVideoParam.m_wVideoWidth;

    mpulogall( "MediaType:%u, FrameID:%u, KF:%d, W*H:%d*%d, SSRC:%u, DataSize:%u\n",
              tFrameHdr.m_dwMediaType,
              tFrameHdr.m_dwFrameID,
              tFrameHdr.m_tVideoParam.m_bKeyFrame,
              tFrameHdr.m_tVideoParam.m_wVideoWidth,
              tFrameHdr.m_tVideoParam.m_wVideoHeight,              
              tFrameHdr.m_dwSSRC,
              tFrameHdr.m_dwDataSize );

	pHardMulPic->SetData( pRcvCB->m_byChnnlId, tFrameHdr );

	return;
}



/*=============================================================================
  函 数 名： FRAMECALLBACK
  功    能： 处理适配后的帧数据
  算法实现： 
  全局变量： 
  参    数： PFRAMEHDR pFrmHdr, u32 dwContext
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录:
  日  期      版本        修改人      修改内容
  2008/08/04  4.5         张永强         创建
=============================================================================*/
void VMPCALLBACK( PTFrameHeader pFrameInfo, void* pContext )
{
    if ( pFrameInfo == NULL || pContext == NULL )
    {
        mpulog( MPU_CRIT, "[CBSendFrame] PTFrameHeader 0x%x, dwContext 0x%x\n", pFrameInfo, pContext );
        return ;
    }

    if (g_bPauseSend)
    {
        mpulogall("[VMPCALLBACK] pause send due to debug!\n");
        return;
    }

	CKdvMediaSnd *pMediaSnd = (CKdvMediaSnd*)pContext;

	FRAMEHDR tTempFrmHdr;
	memset(&tTempFrmHdr, 0x00, sizeof(tTempFrmHdr));

	tTempFrmHdr.m_byMediaType  = (u8)pFrameInfo->m_dwMediaType;
	tTempFrmHdr.m_dwFrameID    = pFrameInfo->m_dwFrameID;
	tTempFrmHdr.m_byFrameRate  = FRAME_RATE;                //帧率固定为25帧

//	tTempFrmHdr.m_dwSSRC       = 0;                         //接收的时候会自动设置
	tTempFrmHdr.m_pData        = pFrameInfo->m_pData;
	tTempFrmHdr.m_dwDataSize   = pFrameInfo->m_dwDataSize;
//	tTempFrmHdr.m_dwPreBufSize = 0;
//	tTempFrmHdr.m_dwTimeStamp  = 0;                         //发送的时候会自动设置
	tTempFrmHdr.m_tVideoParam.m_bKeyFrame    = pFrameInfo->m_tVideoParam.m_bKeyFrame;
	tTempFrmHdr.m_tVideoParam.m_wVideoHeight = pFrameInfo->m_tVideoParam.m_wVideoHeight;
	tTempFrmHdr.m_tVideoParam.m_wVideoWidth  = pFrameInfo->m_tVideoParam.m_wVideoWidth;

	mpulogall( "[CBSendFrame] MediaType:%u, FrameID:%u, KF:%d, W*H:%d*%d, m_dwDataSize:%u\n",
	    	   tTempFrmHdr.m_byMediaType,
		       tTempFrmHdr.m_dwFrameID,
    		   tTempFrmHdr.m_tVideoParam.m_bKeyFrame,
               tTempFrmHdr.m_tVideoParam.m_wVideoWidth,
	    	   tTempFrmHdr.m_tVideoParam.m_wVideoHeight,	    	   
		       tTempFrmHdr.m_dwDataSize );
    

	pMediaSnd->Send( &tTempFrmHdr );

	return;
}

/*lint -save -e429 -e438 -e737*/
/*=============================================================================
  函 数 名： ReadDebugValues
  功    能： 从配置文件读取码率作弊值
  算法实现： 
  全局变量： 
  参    数： void
  返 回 值： void 
=============================================================================*/
void TMpuVmpCfg::ReadDebugValues()
{
    s32 nValue;
    ::GetRegKeyInt( MPULIB_CFG_FILE, SECTION_MPUDEBUG, "IsEnableRateAutoDec", 1, &nValue );
    m_tDebugVal.SetEnableBitrateCheat( (nValue==0) ? FALSE : TRUE );

    if ( !m_tDebugVal.IsEnableBitrateCheat() )
        return;

    ::GetRegKeyInt( MPULIB_CFG_FILE, SECTION_MPUDEBUG, "DefaultTargetRatePct", 75, &nValue );
    m_tDebugVal.SetDefaultRate( (u16)nValue );

	
	//  [nizhijun 2010/11/9] 是否加黑边或裁边,默认是0
	::GetRegKeyInt( MPULIB_CFG_FILE, SECTION_MPUDEBUG, "VidSDMode", 0, &nValue );
	if( nValue != 0 && nValue != 1 && nValue != 2)
	{
		nValue = 0;
	}
    m_tDebugVal.SetVidSDMode(nValue);


	//  [nizhijun 2011/03/21] 设置VMP定时检测关键帧时间间隔
	::GetRegKeyInt( MPULIB_CFG_FILE, SECTION_MPUDEBUG, "VMPIframeInterval", 1000, &nValue );
    m_tDebugVal.SetVmpIframeInterval(nValue);


	s32 nMemEntryNum = 0;
	::GetRegKeyInt( MPULIB_CFG_FILE, SECTION_VMPDEFINE_PIC, "EntryNum", 0, &nMemEntryNum );
	if (0 == nMemEntryNum)
	{
		return;
	}
	
	// alloc memory
	s8** ppszTable = NULL;        
    ppszTable = new s8*[nMemEntryNum];
    if( NULL == ppszTable )
    {
        OspPrintf(1, 0,  "[AgentGetBrdCfgTable] Fail to malloc memory \n");
        return;
    }
	
	u32 dwEntryNum = nMemEntryNum;
	u32 dwLoop = 0;
    for( dwLoop = 0; dwLoop < dwEntryNum; dwLoop++ )
    {
       ppszTable[dwLoop] = new s8[MAX_VALUE_LEN+1];
       if(NULL == ppszTable[dwLoop])
       {
		   //释放内存
		   TableMemoryFree( (void**)ppszTable, nMemEntryNum );
		   return;
        }
    }
	u32 dwReadEntryNum = dwEntryNum;
	GetRegKeyStringTable( MPULIB_CFG_FILE,    
		SECTION_VMPDEFINE_PIC,      
		"fail",     
		ppszTable, 
		&dwReadEntryNum, 
		MAX_VALUE_LEN + 1);
	
	if (dwReadEntryNum != dwEntryNum)
	{
		//释放内存
		TableMemoryFree( (void**)ppszTable, nMemEntryNum );
		return;
	}
	
	s8    achSeps[] = "\t";        // 分隔符
    s8    *pchToken = NULL;
	
	for( dwLoop = 0; dwLoop < dwEntryNum; dwLoop++)
    {
		if (dwLoop + 1 == MAXNUM_VMPDEFINE_PIC)
		{
			break;
		}
		//忽略
        pchToken = strtok( ppszTable[dwLoop], achSeps );
        if (NULL == pchToken)
        {
            continue;
        }
		
		//Vmp成员别名
		pchToken = strtok( NULL, achSeps );
		if (NULL == pchToken)
        {
            continue;
        }
		m_tDebugVal.SetVmpMemAlias(dwLoop, pchToken);
		
		//Vmp成员所要显示的图片
		pchToken = strtok( NULL, achSeps );
		if (NULL == pchToken)
        {
            continue;
        }
		m_tDebugVal.SetRoute(dwLoop, pchToken);
	}
	
	TableMemoryFree( (void**)ppszTable, nMemEntryNum );

    return;
}

/*=============================================================================
  函 数 名： PrintEncParam
  功    能： 
  算法实现： 
  全局变量： 
  参    数： void
  返 回 值： void 
=============================================================================*/
void TMpuVmpCfg::PrintEncParam( u8 byVideoIdx ) const
{

		mpulog(MPU_INFO, "\n\tEncType: \t%u \n\tRcMode: \t%u \n\tMaxKeyFrameInterval: \t%d \n\tMaxQuant: \t%u \n\tMinQuant: \t%u\n",
                               m_tVideoEncParam[byVideoIdx].m_byEncType,
							   m_tVideoEncParam[byVideoIdx].m_byRcMode,
                               m_tVideoEncParam[byVideoIdx].m_byMaxKeyFrameInterval,
							   ntohs(m_tVideoEncParam[byVideoIdx].m_byMaxQuant),
                               ntohs(m_tVideoEncParam[byVideoIdx].m_byMinQuant));

		mpulog(MPU_INFO, "\n\tBitRate: \t%u \n\tSndNetBand: \t%u \n\tFrameRate: \t%u \n\tImageQulity: \t%u \n\tVideoWidth: \t%d \n\tVideoHeight: \t%d\n",
                               m_tVideoEncParam[byVideoIdx].m_wBitRate,
							   m_adwMaxSendBand[byVideoIdx],
                               m_tVideoEncParam[byVideoIdx].m_byFrameRate,
							   m_tVideoEncParam[byVideoIdx].m_byImageQulity,
                               m_tVideoEncParam[byVideoIdx].m_wVideoWidth,
							   m_tVideoEncParam[byVideoIdx].m_wVideoHeight);

}

/*=============================================================================
函 数 名： TableMemoryFree
功    能： 释放指定表的内存
算法实现： 
全局变量： 
参    数：  void **ppMem
u32 dwEntryNum
返 回 值： BOOL32 
=============================================================================*/
BOOL32 TMpuVmpCfg::TableMemoryFree( void **ppMem, u32 dwEntryNum )
{
	if( NULL == ppMem)
	{
		return FALSE;
	}
	for( u32 dwLoop = 0; dwLoop < dwEntryNum; dwLoop++ )
	{
		if( NULL != ppMem[dwLoop] )
		{
			delete [] (s8*)ppMem[dwLoop];
			ppMem[dwLoop] = NULL;
		}
	}
	delete [] (s8*)ppMem;
	ppMem = NULL;
	return TRUE;
}

/*====================================================================
	函数  : GetDefaultParam
	功能  : 根据媒体类型取相应编码的参数
	输入  : byEnctype: 媒体类型 [In]
			tEncparam: 保存参数 [Out]
	输出  : 无
	返回  : 无
	注    : 
----------------------------------------------------------------------
	修改记录    ：
	日  期      版本        修改人        修改内容
    07/05/17    4.0         顾振华        整理
====================================================================*/
void CMpuBasCfg::GetDefaultParam(u8 byEnctype, TVideoEncParam& tEncparam)
{
#if 1
    s32 nValue;
	// 读是否打开平滑发送, zgc, 2008-03-03
	if (!::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_MPUDEBUG, "IsUseSmoothSend", 1, &nValue))
	{
		m_bIsUseSmoothSend = TRUE;
	}
	else
	{
		m_bIsUseSmoothSend = ( 1 == nValue ) ? TRUE : FALSE;
	}

    switch( byEnctype )
    {
    case MEDIA_TYPE_MP4:
        tEncparam.m_byEncType = (u8)EnVidFmt[0][0];

        if (!::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_MPUDEBUG, "MPEG4BitrateControlMode", 0, &nValue))
        {
            tEncparam.m_byRcMode = (u8)EnVidFmt[0][1];
        }
        else
        {
            tEncparam.m_byRcMode = (u8)nValue;
        }

        if (!::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_MPUDEBUG, "MPEG4VideoEcdIKeyRate", 0, &nValue))
        {
            tEncparam.m_byMaxKeyFrameInterval = EnVidFmt[0][2];
        }
        else
        {
            tEncparam.m_byMaxKeyFrameInterval = nValue;
        }

        if (!::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_MPUDEBUG, "MPEG4VideoEcdQualMaxValu", 0, &nValue))
        {
            tEncparam.m_byMaxQuant = (u8)EnVidFmt[0][3];
        }
        else
        {
            tEncparam.m_byMaxQuant = (u8)nValue;
        }

        if (!::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_MPUDEBUG, "MPEG4VideoEcdQualMinValu", 0, &nValue))
        {
            tEncparam.m_byMinQuant = (u8)EnVidFmt[0][4];
        }
        else
        {
            tEncparam.m_byMinQuant = (u8)nValue;
        }

        if (!::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_MPUDEBUG, "MPEG4EncodeBitRate", 0, &nValue))
        {
            tEncparam.m_wBitRate = EnVidFmt[0][5];
        }
        else
        {
            tEncparam.m_wBitRate = (u16)nValue;
        }

        if (!::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_MPUDEBUG, "MPEG4BandWidth", 0, &nValue))
        {
//            tEncparam.m_dwSndNetBand = 0;
        }
        else
        {
//            tEncparam.m_dwSndNetBand = (u32)nValue;
        }
/*
        if (!::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_MPUDEBUG, "MPEG4VideoEncLostFrameOnOff", 0, &nValue))
        {
            encparam.m_byAutoDelFrm = 2;
        }
        else
        {
            encparam.m_byAutoDelFrm = (u32)nValue;
        }
*/
    #ifdef SOFT_CODEC
        tEncparam.m_byQcMode = 0;
        tEncparam.m_byQI     = 5;
        tEncparam.m_byQP     = 13;
    #else
//        tEncparam.m_byPalNtsc = (u8)EnVidFmt[0][6];
//        tEncparam.m_byCapPort = (u8)EnVidFmt[0][7];
    
        if (!::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_MPUDEBUG, "MPEG4FrameRate", 0, &nValue))
        {
            tEncparam.m_byFrameRate = (u8)EnVidFmt[0][8];
        }
        else
        {
            tEncparam.m_byFrameRate = (u8)nValue;
        }

        if (!::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_MPUDEBUG, "MPEG4VideoImageQuality", 0, &nValue))
        {
            tEncparam.m_byImageQulity = (u8)EnVidFmt[0][9];
        }
        else
        {
            tEncparam.m_byImageQulity = (u8)nValue;
        }

        if (!::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_MPUDEBUG, "MPEG4VideoHeight", 0, &nValue))
        {
            tEncparam.m_wVideoHeight	= EnVidFmt[0][10];
        }
        else
        {
            tEncparam.m_wVideoHeight = (u16)nValue;
        }

        if (!::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_MPUDEBUG, "MPEG4VideoWidth", 0, &nValue))
        {
            tEncparam.m_wVideoWidth = EnVidFmt[0][11];
        }
        else
        {
            tEncparam.m_wVideoWidth = (u16)nValue;
        }
    #endif
        break;

    case MEDIA_TYPE_H261: //261
        // mpulog( MPU_CRIT, "[Vmp Encode]The Encode format: H261, %d\n", MEDIA_TYPE_H261);
        tEncparam.m_byEncType = (u8)EnVidFmt[1][0];
        if (!::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_MPUDEBUG, "261BitrateControlMode", 0, &nValue))
        {
            tEncparam.m_byRcMode = (u8)EnVidFmt[1][1];
        }
        else
        {
            tEncparam.m_byRcMode = (u8)nValue;
        }

        if (!::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_MPUDEBUG, "261VideoEcdIKeyRate", 0, &nValue))
        {
            tEncparam.m_byMaxKeyFrameInterval = EnVidFmt[1][2];
        }
        else
        {
            tEncparam.m_byMaxKeyFrameInterval = nValue;
        }

        if (!::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_MPUDEBUG, "261VideoEcdQualMaxValu", 0, &nValue))
        {
            tEncparam.m_byMaxQuant = (u8)EnVidFmt[1][3];
        }
        else
        {
            tEncparam.m_byMaxQuant = (u8)nValue;
        }

        if (!::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_MPUDEBUG, "261VideoEcdQualMinValu", 0, &nValue))
        {
            tEncparam.m_byMinQuant = (u8)EnVidFmt[1][4];
        }
        else
        {
            tEncparam.m_byMinQuant = (u8)nValue;
        }

        if (!::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_MPUDEBUG, "261EncodeBitRate", 0, &nValue))
        {
            tEncparam.m_wBitRate = EnVidFmt[1][5];
        }
        else
        {
            tEncparam.m_wBitRate = (u16)nValue;
        }

        if (!::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_MPUDEBUG, "261BandWidth", 0, &nValue))
        {
//            tEncparam.m_dwSndNetBand = 0;
        }
        else
        {
 //           tEncparam.m_dwSndNetBand = (u32)nValue;
        }

    #ifdef SOFT_CODEC
        tEncparam.m_byQcMode = 0;
        tEncparam.m_byQI     = 5;
        tEncparam.m_byQP     = 13;
    #else
//        tEncparam.m_byPalNtsc = (u8)EnVidFmt[1][6];
//        tEncparam.m_byCapPort = (u8)EnVidFmt[1][7];
        if (!::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_MPUDEBUG, "261FrameRate", 0, &nValue))
        {
            tEncparam.m_byFrameRate = (u8)EnVidFmt[1][8];
        }
        else
        {
            tEncparam.m_byFrameRate = (u8)nValue;
        }

        if (!::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_MPUDEBUG, "261VideoImageQuality", 0, &nValue))
        {
            tEncparam.m_byImageQulity = (u8)EnVidFmt[1][9];
        }
        else
        {
            tEncparam.m_byImageQulity = (u8)nValue;
        }

        if (!::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_MPUDEBUG, "261VideoHeight", 0, &nValue))
        {
            tEncparam.m_wVideoHeight	= EnVidFmt[1][10];
        }
        else
        {
            tEncparam.m_wVideoHeight = (u16)nValue;
        }

        if (!::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_MPUDEBUG, "261VideoWidth", 0, &nValue))
        {
            tEncparam.m_wVideoWidth = EnVidFmt[1][11];
        }
        else
        {
            tEncparam.m_wVideoWidth = (u16)nValue;
        }
    #endif
        break;

    case MEDIA_TYPE_H262:   //262(mepg2)
        // mpulog( MPU_CRIT, "[Vmp Encode]The Encode format: H262, %d\n", MEDIA_TYPE_H262);
        tEncparam.m_byEncType = (u8)EnVidFmt[2][0];
        if (!::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_MPUDEBUG, "262BitrateControlMode", 0, &nValue))
        {
            tEncparam.m_byRcMode = (u8)EnVidFmt[2][1];
        }
        else
        {
            tEncparam.m_byRcMode = (u8)nValue;
        }

        if (!::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_MPUDEBUG, "262VideoEcdIKeyRate", 0, &nValue))
        {
            tEncparam.m_byMaxKeyFrameInterval = EnVidFmt[2][2];
        }
        else
        {
            tEncparam.m_byMaxKeyFrameInterval = nValue;
        }

        if (!::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_MPUDEBUG, "262VideoEcdQualMaxValu", 0, &nValue))
        {
            tEncparam.m_byMaxQuant = (u8)EnVidFmt[2][3];
        }
        else
        {
            tEncparam.m_byMaxQuant = (u8)nValue;
        }

        if (!::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_MPUDEBUG, "262VideoEcdQualMinValu", 0, &nValue))
        {
            tEncparam.m_byMinQuant = (u8)EnVidFmt[2][4];
        }
        else
        {
            tEncparam.m_byMinQuant = (u8)nValue;
        }

        if (!::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_MPUDEBUG, "262EncodeBitRate", 0, &nValue))
        {
            tEncparam.m_wBitRate = EnVidFmt[2][5];
        }
        else
        {
            tEncparam.m_wBitRate = (u16)nValue;
        }

        if (!::GetRegKeyInt( MPULIB_CFG_FILE, SECTION_MPUDEBUG, "262BandWidth", 0, &nValue))
        {
//            tEncparam.m_dwSndNetBand = 0;
        }
        else
        {
 //           tEncparam.m_dwSndNetBand = (u32)nValue;
        }

    #ifdef SOFT_CODEC
        tEncparam.m_byQcMode = 0;
        tEncparam.m_byQI     = 5;
        tEncparam.m_byQP     = 13;
    #else
//        tEncparam.m_byPalNtsc = (u8)EnVidFmt[2][6];
//        tEncparam.m_byCapPort = (u8)EnVidFmt[2][7];
        if (!::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_MPUDEBUG, "262FrameRate", 0, &nValue))
        {
            tEncparam.m_byFrameRate = (u8)EnVidFmt[2][8];
        }
        else
        {
            tEncparam.m_byFrameRate = (u8)nValue;
        }

        if (!::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_MPUDEBUG, "262VideoImageQuality", 0, &nValue))
        {
            tEncparam.m_byImageQulity = (u8) EnVidFmt[2][9];
        }
        else
        {
            tEncparam.m_byImageQulity = (u8)nValue;
        }

        if (!::GetRegKeyInt( MPULIB_CFG_FILE, SECTION_MPUDEBUG, "262VideoHeight", 0, &nValue))
        {
            tEncparam.m_wVideoHeight	= EnVidFmt[2][10];
        }
        else
        {
            tEncparam.m_wVideoHeight = (u16)nValue;
        }

        if (!::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_MPUDEBUG, "262VideoWidth", 0, &nValue))
        {
            tEncparam.m_wVideoWidth = EnVidFmt[2][11];
        }
        else
        {
            tEncparam.m_wVideoWidth = (u16)nValue;
        }
    #endif
        break;

    case MEDIA_TYPE_H263:  //263
        // mpulog( MPU_CRIT, "[Vmp Encode]The Encode format: H263, %d\n", MEDIA_TYPE_H263);
        tEncparam.m_byEncType = (u8)EnVidFmt[3][0];
        if (!::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_MPUDEBUG, "263BitrateControlMode", 0, &nValue))
        {
            tEncparam.m_byRcMode = (u8)EnVidFmt[3][1];
        }
        else
        {
            tEncparam.m_byRcMode = (u8)nValue;
        }

        if (!::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_MPUDEBUG, "263VideoEcdIKeyRate", 0, &nValue))
        {
            tEncparam.m_byMaxKeyFrameInterval = EnVidFmt[3][2];
        }
        else
        {
            tEncparam.m_byMaxKeyFrameInterval = nValue;
        }

        if (!::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_MPUDEBUG, "263VideoEcdQualMaxValu", 0, &nValue))
        {
            tEncparam.m_byMaxQuant = (u8)EnVidFmt[3][3];
        }
        else
        {
            tEncparam.m_byMaxQuant = (u8)nValue;
        }

        if (!::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_MPUDEBUG, "263VideoEcdQualMinValu", 0, &nValue))
        {
            tEncparam.m_byMinQuant = (u8)EnVidFmt[3][4];
        }
        else
        {
            tEncparam.m_byMinQuant = (u8)nValue;
        }

        if (!::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_MPUDEBUG, "263EncodeBitRate", 0, &nValue))
        {
            tEncparam.m_wBitRate = EnVidFmt[3][5];
        }
        else
        {
            tEncparam.m_wBitRate = (u16)nValue;
        }

        if (!::GetRegKeyInt( MPULIB_CFG_FILE, SECTION_MPUDEBUG, "263BandWidth", 0, &nValue))
        {
//            tEncparam.m_dwSndNetBand = 0;
        }
        else
        {
//            tEncparam.m_dwSndNetBand = (u32)nValue;
        }

    #ifdef SOFT_CODEC
        tEncparam.m_byQcMode = 0;
        tEncparam.m_byQI     = 5;
        tEncparam.m_byQP     = 13;
    #else
//        tEncparam.m_byPalNtsc = (u8)EnVidFmt[3][6];
 //       tEncparam.m_byCapPort = (u8)EnVidFmt[3][7];
        if (!::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_MPUDEBUG, "263FrameRate", 0, &nValue))
        {
            tEncparam.m_byFrameRate = (u8)EnVidFmt[3][8];
        }
        else
        {
            tEncparam.m_byFrameRate = (u8)nValue;
        }

        if (!::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_MPUDEBUG, "263VideoImageQuality", 0, &nValue))
        {
            tEncparam.m_byImageQulity = (u8) EnVidFmt[3][9];
        }
        else
        {
            tEncparam.m_byImageQulity = (u8)nValue;
        }

        if (!::GetRegKeyInt( MPULIB_CFG_FILE, SECTION_MPUDEBUG, "263VideoHeight", 0, &nValue))
        {
            tEncparam.m_wVideoHeight	= EnVidFmt[3][10];
        }
        else
        {
            tEncparam.m_wVideoHeight = (u16)nValue;
        }

        if (!::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_MPUDEBUG, "263VideoWidth", 0, &nValue))
        {
            tEncparam.m_wVideoWidth = EnVidFmt[3][11];
        }
        else
        {
            tEncparam.m_wVideoWidth = (u16)nValue;
        }
    #endif
        break;

    case MEDIA_TYPE_H263PLUS:  //263+
        // mpulog( MPU_CRIT, "[Vmp Encode]The Encode format: H263+, %d\n", MEDIA_TYPE_H263PLUS);
        tEncparam.m_byEncType = (u8)EnVidFmt[4][0];
        if (!::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_MPUDEBUG, "263PLUSBitrateControlMode", 0, &nValue))
        {
            tEncparam.m_byRcMode = (u8)EnVidFmt[4][1];
        }
        else
        {
            tEncparam.m_byRcMode = (u8)nValue;
        }

        if (!::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_MPUDEBUG, "263PLUSVideoEcdIKeyRate", 0, &nValue))
        {
            tEncparam.m_byMaxKeyFrameInterval = EnVidFmt[4][2];
        }
        else
        {
            tEncparam.m_byMaxKeyFrameInterval = nValue;
        }

        if (!::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_MPUDEBUG, "263PLUSVideoEcdQualMaxValu", 0, &nValue))
        {
            tEncparam.m_byMaxQuant = (u8)EnVidFmt[4][3];
        }
        else
        {
            tEncparam.m_byMaxQuant = (u8)nValue;
        }

        if (!::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_MPUDEBUG, "263PLUSVideoEcdQualMinValu", 0, &nValue))
        {
            tEncparam.m_byMinQuant = (u8)EnVidFmt[4][4];
        }
        else
        {
            tEncparam.m_byMinQuant = (u8)nValue;
        }

        if (!::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_MPUDEBUG, "263PLUSEncodeBitRate", 0, &nValue))
        {
            tEncparam.m_wBitRate = EnVidFmt[4][5];
        }
        else
        {
            tEncparam.m_wBitRate = (u16)nValue;
        }

        if (!::GetRegKeyInt( MPULIB_CFG_FILE, SECTION_MPUDEBUG, "263PLUSBandWidth", 0, &nValue))
        {
 //           tEncparam.m_dwSndNetBand = 0;
        }
        else
        {
//            tEncparam.m_dwSndNetBand = (u32)nValue;
        }

    #ifdef SOFT_CODEC
        tEncparam.m_byQcMode = 0;
        tEncparam.m_byQI     = 5;
        tEncparam.m_byQP     = 13;
    #else
 //       tEncparam.m_byPalNtsc = (u8)EnVidFmt[4][6];
 //       tEncparam.m_byCapPort = (u8)EnVidFmt[4][7];
        if (!::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_MPUDEBUG, "263PLUSFrameRate", 0, &nValue))
        {
            tEncparam.m_byFrameRate = (u8)EnVidFmt[4][8];
        }
        else
        {
            tEncparam.m_byFrameRate = (u8)nValue;
        }

        if (!::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_MPUDEBUG, "263PLUSVideoImageQuality", 0, &nValue))
        {
            tEncparam.m_byImageQulity = (u8) EnVidFmt[4][9];
        }
        else
        {
            tEncparam.m_byImageQulity = (u8)nValue;
        }

        if (!::GetRegKeyInt( MPULIB_CFG_FILE, SECTION_MPUDEBUG, "263PLUSVideoHeight", 0, &nValue))
        {
            tEncparam.m_wVideoHeight	= EnVidFmt[4][10];
        }
        else
        {
            tEncparam.m_wVideoHeight = (u16)nValue;
        }

        if (!::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_MPUDEBUG, "263PLUSVideoWidth", 0, &nValue))
        {
            tEncparam.m_wVideoWidth = EnVidFmt[4][11];
        }
        else
        {
            tEncparam.m_wVideoWidth = (u16)nValue;
        }
    #endif
        break;

    case MEDIA_TYPE_H264:  //264
        // mpulog( MPU_CRIT, "[Vmp Encode]The Encode format: H264, %d\n", MEDIA_TYPE_H264);
        tEncparam.m_byEncType = (u8)EnVidFmt[5][0];
        if (!::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_MPUDEBUG, "264SBitrateControlMode", 0, &nValue))
        {
            tEncparam.m_byRcMode = (u8)EnVidFmt[5][1];
        }
        else
        {
            tEncparam.m_byRcMode = (u8)nValue;
        }

        if (!::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_MPUDEBUG, "264VideoEcdIKeyRate", 0, &nValue))
        {
            tEncparam.m_byMaxKeyFrameInterval = EnVidFmt[5][2];
        }
        else
        {
            tEncparam.m_byMaxKeyFrameInterval = nValue;
        }

        if (!::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_MPUDEBUG, "264VideoEcdQualMaxValu", 0, &nValue))
        {
            tEncparam.m_byMaxQuant = (u8)EnVidFmt[5][3];
        }
        else
        {
            tEncparam.m_byMaxQuant = (u8)nValue;
        }

        if (!::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_MPUDEBUG, "264VideoEcdQualMinValu", 0, &nValue))
        {
            tEncparam.m_byMinQuant = (u8)EnVidFmt[5][4];
        }
        else
        {
            tEncparam.m_byMinQuant = (u8)nValue;
        }

        if (!::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_MPUDEBUG, "264EncodeBitRate", 0, &nValue))
        {
            tEncparam.m_wBitRate = EnVidFmt[5][5];
        }
        else
        {
            tEncparam.m_wBitRate = (u16)nValue;
        }

        if (!::GetRegKeyInt( MPULIB_CFG_FILE, SECTION_MPUDEBUG, "264BandWidth", 0, &nValue))
        {
//            tEncparam.m_dwSndNetBand = 0;
        }
        else
        {
//            tEncparam.m_dwSndNetBand = (u32)nValue;
        }

    #ifdef SOFT_CODEC
        tEncparam.m_byQcMode = 0;
        tEncparam.m_byQI     = 5;
        tEncparam.m_byQP     = 13;
    #else
//        tEncparam.m_byPalNtsc = (u8)EnVidFmt[5][6];
 //       tEncparam.m_byCapPort = (u8)EnVidFmt[5][7];
        if (!::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_MPUDEBUG, "264FrameRate", 0, &nValue))
        {
            tEncparam.m_byFrameRate = (u8)EnVidFmt[5][8];
        }
        else
        {
            tEncparam.m_byFrameRate = (u8)nValue;
        }

        if (!::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_MPUDEBUG, "264SVideoImageQuality", 0, &nValue))
        {
            tEncparam.m_byImageQulity = 0; // (u8) EnVidFmt[5][9];  // 目前h264只支持速度优先
        }
        else
        {
            tEncparam.m_byImageQulity = (u8)nValue;
        }

        if (!::GetRegKeyInt( MPULIB_CFG_FILE, SECTION_MPUDEBUG, "264VideoHeight", 0, &nValue))
        {
            tEncparam.m_wVideoHeight	= EnVidFmt[5][10];
        }
        else
        {
            tEncparam.m_wVideoHeight = (u16)nValue;
        }

        if (!::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_MPUDEBUG, "264VideoWidth", 0, &nValue))
        {
            tEncparam.m_wVideoWidth = EnVidFmt[5][11];
        }
        else
        {
            tEncparam.m_wVideoWidth = (u16)nValue;
        }
    #endif
        break;

    default:
        break;
    }
#endif
    return;
}

/*=============================================================================
  函 数 名： GetDefaultParam
  功    能： 从配置文件读取码率作弊值
  算法实现： 
  全局变量： 
  参    数： void
  返 回 值： void 
=============================================================================*/
void TMpuVmpCfg::GetDefaultParam( u8 byEnctype, TVideoEncParam& tEncparam, u32& byNetBand )
{
    s32 nValue;

    // 不需判断是否需要
// 	// 读是否打开平滑发送, zgc, 2007-09-28
// 	if ( !::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_VMPSYS, "IsUseSmoothSend", 1, &nValue) )
// 	{
// 		m_byIsUseSmoothSend = VMP_SMOOTH_OPEN;
// 	}
// 	else
// 	{
// 		m_byIsUseSmoothSend = (u8)nValue;
//	}
	
    switch( byEnctype )
    {
    //case 97: (mepg4)
    case MEDIA_TYPE_MP4:
        // mpulog( MPU_CRIT, "[Vmp Encode]The Encode format: MP4, %d\n", MEDIA_TYPE_MP4);
        tEncparam.m_byEncType = (u8)EnVidFmt[0][0];

        if ( !::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_VMPSYS, "MPEG4BitrateControlMode", 0, &nValue) )
        {
            tEncparam.m_byRcMode = (u8)EnVidFmt[0][1];
        }
        else
        {
            tEncparam.m_byRcMode = (u8)nValue;
        }

        if ( !::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_VMPSYS, "MPEG4VideoEcdIKeyRate", 0, &nValue) )
        {
            tEncparam.m_byMaxKeyFrameInterval = EnVidFmt[0][2];
        }
        else
        {
            tEncparam.m_byMaxKeyFrameInterval = nValue;
        }

        if ( !::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_VMPSYS, "MPEG4VideoEcdQualMaxValu", 0, &nValue) )
        {
            tEncparam.m_byMaxQuant = (u8)EnVidFmt[0][3];
        }
        else
        {
            tEncparam.m_byMaxQuant = (u8)nValue;
        }

        if ( !::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_VMPSYS, "MPEG4VideoEcdQualMinValu", 0, &nValue) )
        {
            tEncparam.m_byMinQuant = (u8)EnVidFmt[0][4];
        }
        else
        {
            tEncparam.m_byMinQuant = (u8)nValue;
        }

        if ( !::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_VMPSYS, "MPEG4EncodeBitRate", 0, &nValue) )
        {
            tEncparam.m_wBitRate = EnVidFmt[0][5];
        }
        else
        {
            tEncparam.m_wBitRate = (u16)nValue;
        }

        if ( !::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_VMPSYS, "MPEG4BandWidth", 0, &nValue) )
        {
            byNetBand = 0;
        }
        else
        {
            byNetBand = (u32)nValue;
        }
/*
        if (!::GetRegKeyInt(VMPCFG_FILE, SECTION_VMPSYS, "MPEG4VideoEncLostFrameOnOff", 0, &nValue))
        {
            encparam.m_byAutoDelFrm = 2;
        }
        else
        {
            encparam.m_byAutoDelFrm = (u32)nValue;
        }
*/
    #ifdef SOFT_CODEC
        tEncparam.m_byQcMode = 0;
        tEncparam.m_byQI     = 5;
        tEncparam.m_byQP     = 13;
    #else
//      tEncparam.m_byPalNtsc = (u8)EnVidFmt[0][6];
//      tEncparam.m_byCapPort = (u8)EnVidFmt[0][7];
    
        if ( !::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_VMPSYS, "MPEG4FrameRate", 0, &nValue) )
        {
            tEncparam.m_byFrameRate = (u8)EnVidFmt[0][8];
        }
        else
        {
            tEncparam.m_byFrameRate = (u8)nValue;
        }

        if ( !::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_VMPSYS, "MPEG4VideoImageQuality", 0, &nValue) )
        {
            tEncparam.m_byImageQulity = (u8)EnVidFmt[0][9];
        }
        else
        {
            tEncparam.m_byImageQulity = (u8)nValue;
        }

        if ( !::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_VMPSYS, "MPEG4VideoHeight", 0, &nValue) )
        {
            tEncparam.m_wVideoHeight	= EnVidFmt[0][10];
        }
        else
        {
            tEncparam.m_wVideoHeight = (u16)nValue;
        }

        if ( !::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_VMPSYS, "MPEG4VideoWidth", 0, &nValue) )
        {
            tEncparam.m_wVideoWidth = EnVidFmt[0][11];
        }
        else
        {
            tEncparam.m_wVideoWidth = (u16)nValue;
        }
    #endif
        break;

    case MEDIA_TYPE_H261: //261
        // mpulog( MPU_CRIT, "[Vmp Encode]The Encode format: H261, %d\n", MEDIA_TYPE_H261);
        tEncparam.m_byEncType = (u8)EnVidFmt[1][0];
        if ( !::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_VMPSYS, "261BitrateControlMode", 0, &nValue) )
        {
            tEncparam.m_byRcMode = (u8)EnVidFmt[1][1];
        }
        else
        {
            tEncparam.m_byRcMode = (u8)nValue;
        }

        if ( !::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_VMPSYS, "261VideoEcdIKeyRate", 0, &nValue) )
        {
            tEncparam.m_byMaxKeyFrameInterval = EnVidFmt[1][2];
        }
        else
        {
            tEncparam.m_byMaxKeyFrameInterval = nValue;
        }

        if ( !::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_VMPSYS, "261VideoEcdQualMaxValu", 0, &nValue) )
        {
            tEncparam.m_byMaxQuant = (u8)EnVidFmt[1][3];
        }
        else
        {
            tEncparam.m_byMaxQuant = (u8)nValue;
        }

        if ( !::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_VMPSYS, "261VideoEcdQualMinValu", 0, &nValue) )
        {
            tEncparam.m_byMinQuant = (u8)EnVidFmt[1][4];
        }
        else
        {
            tEncparam.m_byMinQuant = (u8)nValue;
        }

        if ( !::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_VMPSYS, "261EncodeBitRate", 0, &nValue) )
        {
            tEncparam.m_wBitRate = EnVidFmt[1][5];
        }
        else
        {
            tEncparam.m_wBitRate = (u16)nValue;
        }

        if ( !::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_VMPSYS, "261BandWidth", 0, &nValue) )
        {
            byNetBand = 0;
        }
        else
        {
            byNetBand = (u32)nValue;
        }

    #ifdef SOFT_CODEC
        tEncparam.m_byQcMode = 0;
        tEncparam.m_byQI     = 5;
        tEncparam.m_byQP     = 13;
    #else
//      tEncparam.m_byPalNtsc = (u8)EnVidFmt[1][6];
//      tEncparam.m_byCapPort = (u8)EnVidFmt[1][7];
        if ( !::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_VMPSYS, "261FrameRate", 0, &nValue) )
        {
            tEncparam.m_byFrameRate = (u8)EnVidFmt[1][8];
        }
        else
        {
            tEncparam.m_byFrameRate = (u8)nValue;
        }

        if ( !::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_VMPSYS, "261VideoImageQuality", 0, &nValue) )
        {
            tEncparam.m_byImageQulity = (u8)EnVidFmt[1][9];
        }
        else
        {
            tEncparam.m_byImageQulity = (u8)nValue;
        }

        if ( !::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_VMPSYS, "261VideoHeight", 0, &nValue) )
        {
            tEncparam.m_wVideoHeight	= EnVidFmt[1][10];
        }
        else
        {
            tEncparam.m_wVideoHeight = (u16)nValue;
        }

        if ( !::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_VMPSYS, "261VideoWidth", 0, &nValue) )
        {
            tEncparam.m_wVideoWidth = EnVidFmt[1][11];
        }
        else
        {
            tEncparam.m_wVideoWidth = (u16)nValue;
        }
    #endif
        break;

    case MEDIA_TYPE_H262:   //262(mepg2)
        // mpulog( MPU_CRIT, "[Vmp Encode]The Encode format: H262, %d\n", MEDIA_TYPE_H262);
        tEncparam.m_byEncType = (u8)EnVidFmt[2][0];
        if ( !::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_VMPSYS, "262BitrateControlMode", 0, &nValue) )
        {
            tEncparam.m_byRcMode = (u8)EnVidFmt[2][1];
        }
        else
        {
            tEncparam.m_byRcMode = (u8)nValue;
        }

        if ( !::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_VMPSYS, "262VideoEcdIKeyRate", 0, &nValue) )
        {
            tEncparam.m_byMaxKeyFrameInterval = EnVidFmt[2][2];
        }
        else
        {
            tEncparam.m_byMaxKeyFrameInterval = nValue;
        }

        if ( !::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_VMPSYS, "262VideoEcdQualMaxValu", 0, &nValue) )
        {
            tEncparam.m_byMaxQuant = (u8)EnVidFmt[2][3];
        }
        else
        {
            tEncparam.m_byMaxQuant = (u8)nValue;
        }

        if ( !::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_VMPSYS, "262VideoEcdQualMinValu", 0, &nValue) )
        {
            tEncparam.m_byMinQuant = (u8)EnVidFmt[2][4];
        }
        else
        {
            tEncparam.m_byMinQuant = (u8)nValue;
        }

        if ( !::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_VMPSYS, "262EncodeBitRate", 0, &nValue) )
        {
            tEncparam.m_wBitRate = EnVidFmt[2][5];
        }
        else
        {
            tEncparam.m_wBitRate = (u16)nValue;
        }

        if ( !::GetRegKeyInt( MPULIB_CFG_FILE, SECTION_VMPSYS, "262BandWidth", 0, &nValue) )
        {
            byNetBand = 0;
        }
        else
        {
            byNetBand = (u32)nValue;
        }

    #ifdef SOFT_CODEC
        tEncparam.m_byQcMode = 0;
        tEncparam.m_byQI     = 5;
        tEncparam.m_byQP     = 13;
    #else
//      tEncparam.m_byPalNtsc = (u8)EnVidFmt[2][6];
//      tEncparam.m_byCapPort = (u8)EnVidFmt[2][7];
        if ( !::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_VMPSYS, "262FrameRate", 0, &nValue) )
        {
            tEncparam.m_byFrameRate = (u8)EnVidFmt[2][8];
        }
        else
        {
            tEncparam.m_byFrameRate = (u8)nValue;
        }

        if ( !::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_VMPSYS, "262VideoImageQuality", 0, &nValue) )
        {
            tEncparam.m_byImageQulity = (u8) EnVidFmt[2][9];
        }
        else
        {
            tEncparam.m_byImageQulity = (u8)nValue;
        }

        if ( !::GetRegKeyInt( MPULIB_CFG_FILE, SECTION_VMPSYS, "262VideoHeight", 0, &nValue) )
        {
            tEncparam.m_wVideoHeight	= EnVidFmt[2][10];
        }
        else
        {
            tEncparam.m_wVideoHeight = (u16)nValue;
        }

        if ( !::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_VMPSYS, "262VideoWidth", 0, &nValue) )
        {
            tEncparam.m_wVideoWidth = EnVidFmt[2][11];
        }
        else
        {
            tEncparam.m_wVideoWidth = (u16)nValue;
        }
    #endif
        break;

    case MEDIA_TYPE_H263:  //263
        // mpulog( MPU_CRIT, "[Vmp Encode]The Encode format: H263, %d\n", MEDIA_TYPE_H263);
        tEncparam.m_byEncType = (u8)EnVidFmt[3][0];
        if ( !::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_VMPSYS, "263BitrateControlMode", 0, &nValue) )
        {
            tEncparam.m_byRcMode = (u8)EnVidFmt[3][1];
        }
        else
        {
            tEncparam.m_byRcMode = (u8)nValue;
        }

        if ( !::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_VMPSYS, "263VideoEcdIKeyRate", 0, &nValue) )
        {
            tEncparam.m_byMaxKeyFrameInterval = EnVidFmt[3][2];
        }
        else
        {
            tEncparam.m_byMaxKeyFrameInterval = nValue;
        }

        if ( !::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_VMPSYS, "263VideoEcdQualMaxValu", 0, &nValue) )
        {
            tEncparam.m_byMaxQuant = (u8)EnVidFmt[3][3];
        }
        else
        {
            tEncparam.m_byMaxQuant = (u8)nValue;
        }

        if ( !::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_VMPSYS, "263VideoEcdQualMinValu", 0, &nValue) )
        {
            tEncparam.m_byMinQuant = (u8)EnVidFmt[3][4];
        }
        else
        {
            tEncparam.m_byMinQuant = (u8)nValue;
        }

        if ( !::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_VMPSYS, "263EncodeBitRate", 0, &nValue) )
        {
            tEncparam.m_wBitRate = EnVidFmt[3][5];
        }
        else
        {
            tEncparam.m_wBitRate = (u16)nValue;
        }

        if ( !::GetRegKeyInt( MPULIB_CFG_FILE, SECTION_VMPSYS, "263BandWidth", 0, &nValue) )
        {
            byNetBand = 0;
        }
        else
        {
            byNetBand = (u32)nValue;
        }

    #ifdef SOFT_CODEC
        tEncparam.m_byQcMode = 0;
        tEncparam.m_byQI     = 5;
        tEncparam.m_byQP     = 13;
    #else
//      tEncparam.m_byPalNtsc = (u8)EnVidFmt[3][6];
//      tEncparam.m_byCapPort = (u8)EnVidFmt[3][7];
        if ( !::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_VMPSYS, "263FrameRate", 0, &nValue) ) 
        {
            tEncparam.m_byFrameRate = (u8)EnVidFmt[3][8];
        }
        else
        {
            tEncparam.m_byFrameRate = (u8)nValue;
        }

        if ( !::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_VMPSYS, "263VideoImageQuality", 0, &nValue) )
        {
            tEncparam.m_byImageQulity = (u8) EnVidFmt[3][9];
        }
        else
        {
            tEncparam.m_byImageQulity = (u8)nValue;
        }

        if ( !::GetRegKeyInt( MPULIB_CFG_FILE, SECTION_VMPSYS, "263VideoHeight", 0, &nValue) )
        {
            tEncparam.m_wVideoHeight	= EnVidFmt[3][10];
        }
        else
        {
            tEncparam.m_wVideoHeight = (u16)nValue;
        }

        if ( !::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_VMPSYS, "263VideoWidth", 0, &nValue) )
        {
            tEncparam.m_wVideoWidth = EnVidFmt[3][11];
        }
        else
        {
            tEncparam.m_wVideoWidth = (u16)nValue;
        }
    #endif
        break;

    case MEDIA_TYPE_H263PLUS:  //263+
        // mpulog( MPU_CRIT, "[Vmp Encode]The Encode format: H263+, %d\n", MEDIA_TYPE_H263PLUS);
        tEncparam.m_byEncType = (u8)EnVidFmt[4][0];
        if ( !::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_VMPSYS, "263PLUSBitrateControlMode", 0, &nValue) )
        {
            tEncparam.m_byRcMode = (u8)EnVidFmt[4][1];
        }
        else
        {
            tEncparam.m_byRcMode = (u8)nValue;
        }

        if ( !::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_VMPSYS, "263PLUSVideoEcdIKeyRate", 0, &nValue) )
        {
            tEncparam.m_byMaxKeyFrameInterval = EnVidFmt[4][2];
        }
        else
        {
            tEncparam.m_byMaxKeyFrameInterval = nValue;
        }

        if ( !::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_VMPSYS, "263PLUSVideoEcdQualMaxValu", 0, &nValue) )
        {
            tEncparam.m_byMaxQuant = (u8)EnVidFmt[4][3];
        }
        else
        {
            tEncparam.m_byMaxQuant = (u8)nValue;
        }

        if ( !::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_VMPSYS, "263PLUSVideoEcdQualMinValu", 0, &nValue) )
        {
            tEncparam.m_byMinQuant = (u8)EnVidFmt[4][4];
        }
        else
        {
            tEncparam.m_byMinQuant = (u8)nValue;
        }

        if ( !::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_VMPSYS, "263PLUSEncodeBitRate", 0, &nValue) )
        {
            tEncparam.m_wBitRate = EnVidFmt[4][5];
        }
        else
        {
            tEncparam.m_wBitRate = (u16)nValue;
        }

        if ( !::GetRegKeyInt( MPULIB_CFG_FILE, SECTION_VMPSYS, "263PLUSBandWidth", 0, &nValue) )
        {
            byNetBand = 0;
        }
        else
        {
            byNetBand = (u32)nValue;
        }

    #ifdef SOFT_CODEC
        tEncparam.m_byQcMode = 0;
        tEncparam.m_byQI     = 5;
        tEncparam.m_byQP     = 13;
    #else
//      tEncparam.m_byPalNtsc = (u8)EnVidFmt[4][6];
//      tEncparam.m_byCapPort = (u8)EnVidFmt[4][7];
        if ( !::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_VMPSYS, "263PLUSFrameRate", 0, &nValue) )
        {
            tEncparam.m_byFrameRate = (u8)EnVidFmt[4][8];
        }
        else
        {
            tEncparam.m_byFrameRate = (u8)nValue;
        }

        if ( !::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_VMPSYS, "263PLUSVideoImageQuality", 0, &nValue) )
        {
            tEncparam.m_byImageQulity = (u8) EnVidFmt[4][9];
        }
        else
        {
            tEncparam.m_byImageQulity = (u8)nValue;
        }

        if ( !::GetRegKeyInt( MPULIB_CFG_FILE, SECTION_VMPSYS, "263PLUSVideoHeight", 0, &nValue) )
        {
            tEncparam.m_wVideoHeight	= EnVidFmt[4][10];
        }
        else
        {
            tEncparam.m_wVideoHeight = (u16)nValue;
        }

        if ( !::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_VMPSYS, "263PLUSVideoWidth", 0, &nValue) )
        {
            tEncparam.m_wVideoWidth = EnVidFmt[4][11];
        }
        else
        {
            tEncparam.m_wVideoWidth = (u16)nValue;
        }
    #endif
        break;

    case MEDIA_TYPE_H264:  //264
        // mpulog( MPU_CRIT, "[Vmp Encode]The Encode format: H264, %d\n", MEDIA_TYPE_H264);
        tEncparam.m_byEncType = (u8)EnVidFmt[5][0];
        if ( !::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_VMPSYS, "264SBitrateControlMode", 0, &nValue) )
        {
            tEncparam.m_byRcMode = (u8)EnVidFmt[5][1];
        }
        else
        {
            tEncparam.m_byRcMode = (u8)nValue;
        }

        if ( !::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_VMPSYS, "264VideoEcdIKeyRate", 0, &nValue) )
        {
            tEncparam.m_byMaxKeyFrameInterval = EnVidFmt[5][2];
        }
        else
        {
            tEncparam.m_byMaxKeyFrameInterval = nValue;
        }

        if ( !::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_VMPSYS, "264VideoEcdQualMaxValu", 0, &nValue) )
        {
            tEncparam.m_byMaxQuant = (u8)EnVidFmt[5][3];
        }
        else
        {
            tEncparam.m_byMaxQuant = (u8)nValue;
        }

        if ( !::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_VMPSYS, "264VideoEcdQualMinValu", 0, &nValue) )
        {
            tEncparam.m_byMinQuant = (u8)EnVidFmt[5][4];
        }
        else
        {
            tEncparam.m_byMinQuant = (u8)nValue;
        }

        if ( !::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_VMPSYS, "264EncodeBitRate", 0, &nValue) )
        {
            tEncparam.m_wBitRate = EnVidFmt[5][5];
        }
        else
        {
            tEncparam.m_wBitRate = (u16)nValue;
        }

        if ( !::GetRegKeyInt( MPULIB_CFG_FILE, SECTION_VMPSYS, "264BandWidth", 0, &nValue) )
        {
            byNetBand = 0;
        }
        else
        {
            byNetBand = (u32)nValue;
        }

    #ifdef SOFT_CODEC
        tEncparam.m_byQcMode = 0;
        tEncparam.m_byQI     = 5;
        tEncparam.m_byQP     = 13;
    #else
//      tEncparam.m_byPalNtsc = (u8)EnVidFmt[5][6];
//      tEncparam.m_byCapPort = (u8)EnVidFmt[5][7];
        if ( !::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_VMPSYS, "264FrameRate", 0, &nValue) )
        {
            tEncparam.m_byFrameRate = (u8)EnVidFmt[5][8];
        }
        else
        {
            tEncparam.m_byFrameRate = (u8)nValue;
        }

        if ( !::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_VMPSYS, "264SVideoImageQuality", 0, &nValue) )
        {
            tEncparam.m_byImageQulity = 0; // (u8) EnVidFmt[5][9];  // 目前h264只支持速度优先
        }
        else
        {
            tEncparam.m_byImageQulity = (u8)nValue;
        }

        if ( !::GetRegKeyInt( MPULIB_CFG_FILE, SECTION_VMPSYS, "264VideoHeight", 0, &nValue) )
        {
            tEncparam.m_wVideoHeight	= EnVidFmt[5][10];
        }
        else
        {
            tEncparam.m_wVideoHeight = (u16)nValue;
        }

        if ( !::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_VMPSYS, "264VideoWidth", 0, &nValue) )
        {
            tEncparam.m_wVideoWidth = EnVidFmt[5][11];
        }
        else
        {
            tEncparam.m_wVideoWidth = (u16)nValue;
        }
    #endif
        break;

    default:
        break;
    }

    return;
}


/*=============================================================================
函 数 名： GetVmpChlNumByStyle
功    能： 根据画面合成风格获得合成通道数
算法实现： 
全局变量： 
参    数： u8 byVMPStyle
返 回 值： u8
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
04/03/31    3.0         胡昌威                创建
09/03/14    4.6         张宝卿                从标清以移植过来
=============================================================================*/
u8 TMpuVmpCfg::GetVmpChlNumByStyle( u8 byVMPStyle )
{
    u8   byMaxMemNum = 0;
    
    switch( byVMPStyle ) 
    {
    case VMP_STYLE_ONE:
        byMaxMemNum = 1;
        break;
    case VMP_STYLE_VTWO:
    case VMP_STYLE_HTWO:
        byMaxMemNum = 2;
        break;
    case VMP_STYLE_THREE:
        byMaxMemNum = 3;
        break;
    case VMP_STYLE_FOUR:
    case VMP_STYLE_SPECFOUR:
        byMaxMemNum = 4;
        break;
    case VMP_STYLE_SIX:
        byMaxMemNum = 6;
        break;
    case VMP_STYLE_EIGHT:
        byMaxMemNum = 8;
        break;
    case VMP_STYLE_NINE:
        byMaxMemNum = 9;
        break;
    case VMP_STYLE_TEN:
        byMaxMemNum = 10;
        break;
    case VMP_STYLE_THIRTEEN:
        byMaxMemNum = 13;
        break;
    case VMP_STYLE_SIXTEEN:
        byMaxMemNum = 16;
        break;       
    case VMP_STYLE_SEVEN:
        byMaxMemNum = 7;
        break;
    case VMP_STYLE_TWENTY:
        byMaxMemNum = 20;
        break;
    default:
        break;
    }
    
    return byMaxMemNum;
}

BOOL32 CMpuBasAdpGroup::GetOutputChnNumAcd2WorkMode(u8 byWorkMode, u8 &byOutputChnNum)
{
	byOutputChnNum = 0;
	switch (byWorkMode)
	{
	case TYPE_MPUBAS:
		byOutputChnNum = MAXNUM_VOUTPUT;
		break;
	case TYPE_MPUBAS_H:
		byOutputChnNum = MAXNUM_MPU_H_VOUTPUT;
		break;
	default:
		OspPrintf(TRUE, FALSE, "[GetOutputChnNumAcd2WorkMode] unexpected mpu workmode:%d!\n", byWorkMode);
		break;
	}
	return 0 != byOutputChnNum;
}
/*lint -restore*/
//END OF FILE
