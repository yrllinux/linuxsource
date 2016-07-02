/*****************************************************************************
   模块名      : mpu2lib
   文件名      : mpu2struct.cpp
   相关文件    : 
   文件实现功能: CMpu2BasData CMpu2BasAdpGroup 结构和回调实现
   作者        : 倪志俊
   版本        : V4.7  
-----------------------------------------------------------------------------
   修改记录:
   日  期       版本        修改人      修改内容
   2012/05/10    4.7         倪志俊      创建
******************************************************************************/
#include "mpu2struct.h"
#include "mpu2inst.h"



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
void CBMpu2BasRecvFrame( PFRAMEHDR pFrmHdr, u32 dwContext )
{
	CMpu2BasInst * pInst = (CMpu2BasInst*)dwContext;
    if ( pFrmHdr == NULL || pInst == NULL )
    {
        mpulog( MPU_CRIT, "[CBMpu2BasRecvFrame] PTFrameHeader 0x%x, dwContext 0x%x.\n", pFrmHdr, dwContext);
        return ;
    }

	CMpu2BasAdpGroup *pAdpGroup = pInst->GetBasAdpGroup();
	if ( pAdpGroup == NULL )
	{
		mpulog( MPU_CRIT, "[CBMpu2BasRecvFrame] pAdpGroup 0x%x is null.\n", pAdpGroup);
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

    mpulogall( "[CBMpu2BasRecvFrame] MediaType:%u, FrameID:%u, KF:%d, W*H:%d*%d, SSRC:%u, DataSize:%u\n",
              tFrameHdr.m_dwMediaType,
              tFrameHdr.m_dwFrameID,
              tFrameHdr.m_tVideoParam.m_bKeyFrame,
              tFrameHdr.m_tVideoParam.m_wVideoWidth,
              tFrameHdr.m_tVideoParam.m_wVideoHeight,              
              tFrameHdr.m_dwSSRC,
              tFrameHdr.m_dwDataSize );

	u16 wRet = pAdpGroup->SetAdpGrpData(tFrameHdr);

	if (  wRet != (u16)Codec_Success )
	{
		ErrorLog("[CBMpu2BasRecvFrame]SetAdpGrpData tFrameHdr error:%d\n",wRet);
	}


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
void CBMpu2BasSendFrame( PTFrameHeader pFrmHdr, void* dwContext )
{
    if ( pFrmHdr == NULL || dwContext == NULL )
    {
        mpulog( MPU_CRIT, "[CBMpu2BasSendFrame] PTFrameHeader 0x%x, dwContext 0x%x\n", pFrmHdr, dwContext );
        return ;
    }

    if (g_bPauseSend)
    {
        mpulogall("[CBMpu2BasSendFrame] pause send due to debug!\n");
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

	mpulogall( "[CBMpu2BasSendFrame] MediaType:%u, FrameID:%u, KF:%d, W*H:%d*%d, m_dwDataSize:%u\n",
	    	   tTempFrmHdr.m_byMediaType,
		       tTempFrmHdr.m_dwFrameID,
    		   tTempFrmHdr.m_tVideoParam.m_bKeyFrame,
               tTempFrmHdr.m_tVideoParam.m_wVideoWidth,
	    	   tTempFrmHdr.m_tVideoParam.m_wVideoHeight,	    	   
		       tTempFrmHdr.m_dwDataSize );
    

	u16 wRet = pMediaSnd->Send( &tTempFrmHdr );
    if (wRet != MEDIANET_NO_ERROR)
    {
        mpulog(MPU_CRIT, "[CBMpu2BasSendFrame] pMediaSnd->Send failed, ret.%d!\n", wRet);
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
    CMpu2BasInst * pInst = (CMpu2BasInst*)dwContext;
    if ( pRtpPack == NULL || pInst == NULL )
    {
        mpulog( MPU_CRIT, "[CBAudRecvFrame] pRtpPack 0x%x, dwContext 0x%x.\n", pRtpPack, dwContext);
        return ;
    }
    
    CMpu2BasAdpGroup *pAdpGroup = pInst->GetBasAdpGroup();
    if ( pAdpGroup == NULL )
    {
        mpulog( MPU_CRIT, "[CBAudRecvFrame] pAdpGroup 0x%x is null.\n", pAdpGroup);
        return ;
    }

    TRtpPkQueue *ptQueue = pInst->GetRtpPkQueue();
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
  函 数 名： Init
  功    能： 初始化
  算法实现： 
  全局变量： 
  参    数： void
  返 回 值： void 
=============================================================================*/
BOOL CMpu2BasAdpGroup::Init(  u16 wLocalRcvStartPort,    //rtp码流接收端口，本地rtcp端口为rtp端口+1
							  u16 wRtcpBackPort,         //rtcp回发端口(转发板端口)，目前策略是使用RTP接收端口+1		   
							  u32 dwDestIp,				//发送目的Ip地址，
							  u16 dwDestPort,			//发送目的起始端口
							  u32 dwContext,			//实例指针
							  u8  byChnId,				//通道ID //适配通道Id ( 0,1,2,3,4,5,6... ),不区分外设
							  u8  byOutChnlNum,		//编码发送路数
							  u8  byWorkMode,
							  u8  byBasIdx
							)
{
	m_byWorkMode	   = byWorkMode;
	m_dwNetBand        = 8000000;
	m_byOutChnlNum	   = byOutChnlNum;	
	m_wLoclRcvStartPort= wLocalRcvStartPort;
	m_dwDestIp		   = dwDestIp;		
	m_wDestStartPort   = dwDestPort;
	m_byChnId		   = byChnId;
	m_byBasIdx		   = byBasIdx;

    DetailLog("[CMpu2BasAdpGroup::Init] m_dwNetBand:%d m_wLoclRcvStartPort:%d, m_dwDestIp:%s, m_wDestPort:%d,m_byOutChnlNum:%d\n",
           m_dwNetBand,
           wLocalRcvStartPort,
           StrOfIP(dwDestIp),
           dwDestPort,
		   m_byOutChnlNum
           );

	//创建视频码流接收对象
    m_pcMediaVidRcv    = new CKdvMediaRcv;
	if ( NULL == m_pcMediaVidRcv )
	{
		ErrorLog("CMpu2BasAdpGroup::init--> new CKdvMediaRcv failed!\n" );
		printf( "CMpu2BasAdpGroup::init--> new CKdvMediaRcv failed!\n" );
		return FALSE;
	}

	//创建音频码流接收对象
    m_pcMediaAudRcv = new CKdvMediaRcv;
	if ( NULL == m_pcMediaAudRcv )
	{
		ErrorLog("CMpu2BasAdpGroup::init--> new CKdvMediaRcv failed!\n" );
		printf( "CMpu2BasAdpGroup::init--> new CKdvMediaRcv failed!\n" );
		return FALSE;
	}
	
	//创建视频与音频码流发送对象
	u8 byIdx = 0;	
	for (byIdx = 0; byIdx < m_byOutChnlNum; byIdx++)
	{
		m_pcMediaVidSnd[byIdx] = new CKdvMediaSnd;
		m_pcMediaAudSnd[byIdx] = new CKdvMediaSnd;
		
		if ((NULL == m_pcMediaVidSnd[byIdx]) || (NULL == m_pcMediaAudSnd[byIdx]))
		{
			for(u8 byIndex = 0; byIndex < byIdx+1; byIndex++)
			{
				delete m_pcMediaVidSnd[byIndex];
				m_pcMediaVidSnd[byIndex] = NULL;
				delete m_pcMediaAudSnd[byIndex];
				m_pcMediaAudSnd[byIndex] = NULL;
			}
			ErrorLog("CMpu2BasAdpGroup::Create--> new CKdvMediaSnd[%d] failed!\n", byIdx);
			printf( "CMpu2BasAdpGroup::Create--> new CKdvMediaSnd[%d] failed!\n", byIdx );
			return FALSE;
		}				
	}
  
	//媒控适配组创建，并将媒控与网络通过回调函数关联
	u16 wRet = 0;
	do {
// 		//编码参数和解码类型此时确定不了
// 		TAdapterChannel tBasChnl;
// 		tBasChnl.m_dwAdapterChnlId = m_byChnId;
// 		tBasChnl.m_dwEncNum = m_byOutChnlNum;
// 		tBasChnl.m_dwDecMediaType = MEDIA_TYPE_H264;
// 		wRet = g_cMpu2BasApp.AddBasChnnl(m_byBasIdx,&tBasChnl);
// 		if ( (u16)Codec_Success != wRet )
// 		{
// 			ErrorLog( "[CMpu2AdpGroup]AddBasChnnl:%d byBasIdx:%d  errno: %d \n",m_byChnId,byBasIdx,wRet );
//             printf( "[CMpu2AdpGroup]AddBasChnnl:%d byBasIdx:%d  errno: %d \n", m_byChnId,byBasIdx,wRet );
// 			return FALSE;
// 		}
// 		else
// 		{
// 			KeyLog("[CMpu2AdpGroup]AddBasChnnl:%d success byBasIdx:%d\n",m_byChnId,byBasIdx);
// 			printf( "CMpu2AdpGroup::AddBasChnnl:%d success  byBasIdx:%d\n",m_byChnId,byBasIdx);
// 		}


		printf("before create!\n");
		wRet = m_pcMediaVidRcv->Create(MAX_VIDEO_FRAME_SIZE,
			                        CBMpu2BasRecvFrame,   
				    			    (u32)dwContext );
		KeyLog("[CMpu2BasAdpGroup::Init]Create CBMpu2BasRecvFrame for basidx:%d baschnid:%d success\n",m_byBasIdx,m_byChnId);
		printf("after create!---wRet :%d\n",wRet);
        if( MEDIANET_NO_ERROR != wRet )
		{
//			printf("before destroy!--wRet:%d\n",wRet);
			//m_cAptGrp.Destroy(); 
//			printf("after destroy!\n");
			ErrorLog("CMpu2AdpGroup::Create-->m_pcMediaVidRcv->Create fail, Error code is: %d \n", wRet );
//            printf( "CMpu2AdpGroup::Create-->m_pcMediaVidRcv->Create fail, Error code is: %d \n", wRet );
			break;
		}

		wRet = m_pcMediaAudRcv->Create(MAX_AUDIO_FRAME_SIZE,
			                           CBMpuBasAudRecvRtpPk,   
				    			       (u32)dwContext );

        if( MEDIANET_NO_ERROR != wRet )
		{
			//m_cAptGrp.Destroy(); 
			ErrorLog("CMpuAdpGroup::Create-->m_pcMediaAudRcv->Create fail, Error code is: %d \n", wRet );
//            printf( "CMpuAdpGroup::Create-->m_pcMediaAudRcv->Create fail, Error code is: %d \n", wRet );
			break;
		}
		
		TLocalNetParam tlocalNetParm;
		//视频接收参数设置
		memset( &tlocalNetParm, 0, sizeof(TLocalNetParam) );
        tlocalNetParm.m_tLocalNet.m_wRTPPort  = m_wLoclRcvStartPort;
		//8KI避免与转发端口冲突，故本地绑定的RTCP音频端口+3
#ifdef _8KI_
		tlocalNetParm.m_tLocalNet.m_wRTCPPort = m_wLoclRcvStartPort + 3 +1;	    //46004
#else
        tlocalNetParm.m_tLocalNet.m_wRTCPPort = m_wLoclRcvStartPort + 1;		//46001
#endif
        tlocalNetParm.m_dwRtcpBackAddr        = htonl(m_dwDestIp);
        tlocalNetParm.m_wRtcpBackPort         = wRtcpBackPort;
		
		KeyLog("before SetRcv!\n");
        wRet = m_pcMediaVidRcv->SetNetRcvLocalParam( tlocalNetParm );
		KeyLog("after SetRcv! --ret: %d\n",wRet);
        if( MEDIANET_NO_ERROR !=  wRet )
        {
            //m_cAptGrp.Destroy();
            ErrorLog("CMpuAdpGroup::Create-->m_pcMediaVidRcv->SetNetRcvLocalParam fail, Error code is: %d \n", wRet );
//            printf( "CMpuAdpGroup::Create-->m_pcMediaVidRcv->SetNetRcvLocalParam fail, Error code is: %d \n", wRet );
            break;
		}

		//音频接收参数设置
	
        memset( &tlocalNetParm, 0, sizeof(TLocalNetParam) );
        tlocalNetParm.m_tLocalNet.m_wRTPPort  = m_wLoclRcvStartPort + 2;
		//8KI避免与转发端口冲突，故本地绑定的RTCP音频端口+3
#ifdef _8KI_
		tlocalNetParm.m_tLocalNet.m_wRTCPPort = m_wLoclRcvStartPort + 3 +2;	    //46005
#else
		tlocalNetParm.m_tLocalNet.m_wRTCPPort = m_wLoclRcvStartPort + 1 + 2;	//46003
#endif
        tlocalNetParm.m_dwRtcpBackAddr        = htonl(m_dwDestIp);
        tlocalNetParm.m_wRtcpBackPort         = wRtcpBackPort + 2;

		KeyLog("before SetRcv!\n");
        wRet = m_pcMediaAudRcv->SetNetRcvLocalParam( tlocalNetParm );
		KeyLog("after SetRcv! --ret: %d\n",wRet);
        if( MEDIANET_NO_ERROR !=  wRet )
        {
            //m_cAptGrp.Destroy();
            ErrorLog("CMpuAdpGroup::Create-->m_pcMediaAudRcv->SetNetRcvLocalParam fail, Error code is: %d \n", wRet );
//            printf( "CMpuAdpGroup::Create-->m_pcMediaAudRcv->SetNetRcvLocalParam fail, Error code is: %d \n", wRet );
            break;
		}


		//mpu2:medianet接收对象不要调用SetHdFlag接口或者设置参数为false。
		//如果设置为TRUE，net组帧时会加一个2K的头，这个头是不需要的，只会增加负担。
        m_pcMediaVidRcv->SetHDFlag( FALSE );
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
  参    数： u8 byBasIdx 表示第几个外设
  返 回 值： void 
=============================================================================*/
BOOL CMpu2BasAdpGroup::StartAdapter( u8 byBasIdx, BOOL32 bStartMediaRcv )
{
	DetailLog("CMpu2BasAdpGroup::StartAdapter\n" );
	u16 wRet = 0;

	if ( IsStart() )
	{
    	DetailLog("CMpu2BasAdpGroup::StartAdapter success\n" );
		return TRUE;
	}	
	wRet = g_cMpu2BasApp.StartVideoAdapter(byBasIdx,m_byChnId);

	if ( bStartMediaRcv )
	{
		m_pcMediaVidRcv->StartRcv();
        m_pcMediaAudRcv->StartRcv();
	}

	if( (u16)Codec_Success != wRet )
	{
		m_pcMediaVidRcv->StopRcv();
		m_pcMediaAudRcv->StopRcv();
        ErrorLog("CMpu2BasAdpGroup::StartAdapter-->m_cAptGrp.StartGroup fail, Error code is: %d \n", wRet );
		return FALSE;
	}

	SetStart(TRUE);

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
BOOL CMpu2BasAdpGroup::SetVideoEncSend( u8 byIdx, u32 dwNetBand,  u16 wLoacalStartPort,  u32 dwDestIp,  u16 dwDestPort )
{
	DetailLog("[SetVideoEncSend] wLoacalStartPort:%u, dest:%s:%u\n", wLoacalStartPort, StrOfIP(dwDestIp), dwDestPort );
	
    u16 wRet = 0;

	do
	{
        u8 byFrameRate = (u8)m_atVidEncParam[byIdx].m_tVideoEncParam.m_byFrameRate;
    	u8 byMediaType = (u8)m_atVidEncParam[byIdx].m_tVideoEncParam.m_byEncType;
    	wRet = m_pcMediaVidSnd[byIdx]->Create( MAX_VIDEO_FRAME_SIZE,
		    	                        dwNetBand,
			                            byFrameRate,
			    				        byMediaType );

        if ( MEDIANET_NO_ERROR != wRet )
        {
            g_cMpu2BasApp.StopVideoAdapter(m_byBasIdx,m_byChnId);
            ErrorLog("CMpuBasAdpGroup::SetVideoEncSend-->m_pcMediaVidSnd[%d].Create fail, Error code is:%d, NetBand.%d, FrmRate.%d, MediaType.%d\n", byIdx, wRet, dwNetBand, byFrameRate, byMediaType );
//            printf( "CMpuBasAdpGroup::SetVideoEncSend-->m_pcMediaVidSnd[%d].Create fail, Error code is: %d \n", byIdx,  wRet );
    	   	break;
        }
        else
        {
            DetailLog("CMpuBasAdpGroup::SetVideoEncSend-->m_pcMediaVidSnd[%d].0x%x.Create succeed, NetBand.%d, FrmRate.%d, MediaType.%d\n", byIdx, m_pcMediaVidSnd[byIdx], dwNetBand, byFrameRate, byMediaType );
//            printf( "CMpuBasAdpGroup::SetVideoEncSend-->m_pcMediaVidSnd[%d].Create succeed!\n", byIdx);
        }
		
		//选看通道最后1路非高清H264,主流和双流最后2两路非高清H264
		if ( m_byOutChnlNum == MPU2_BAS_ENHACNED_SELOUTPUT ||  m_byOutChnlNum == MPU2_BAS_BASIC_SELOUTPUT)
		{
			if ( (byIdx ==(MPU2_BAS_ENHACNED_SELOUTPUT-1)) ||
				 (byIdx ==(MPU2_BAS_BASIC_SELOUTPUT-1))
				)
			{
				m_pcMediaVidSnd[byIdx]->SetHDFlag( FALSE );
			}
			else
			{
				m_pcMediaVidSnd[byIdx]->SetHDFlag( TRUE );
			}
		}
		else
		{
			u8 byEnd = m_byOutChnlNum - 2 -1;	//主流和双流最后2出是标清
			if ( byIdx > byEnd )
			{
				m_pcMediaVidSnd[byIdx]->SetHDFlag( FALSE );
			}
			else
			{
				m_pcMediaVidSnd[byIdx]->SetHDFlag( TRUE );
			}
		}

		//设置发送参数
		TNetSndParam tNetSndPar;
		memset( &tNetSndPar, 0x00, sizeof(tNetSndPar) );
		
		tNetSndPar.m_byNum  = 1;
		tNetSndPar.m_tLocalNet.m_wRTPPort       = wLoacalStartPort;
		tNetSndPar.m_tLocalNet.m_wRTCPPort      = wLoacalStartPort + 1;
		tNetSndPar.m_tRemoteNet[0].m_dwRTPAddr  = htonl(dwDestIp);
		tNetSndPar.m_tRemoteNet[0].m_wRTPPort   = dwDestPort;
		tNetSndPar.m_tRemoteNet[0].m_dwRTCPAddr = htonl(dwDestIp);
		tNetSndPar.m_tRemoteNet[0].m_wRTCPPort  = dwDestPort + 1;
		
		wRet = m_pcMediaVidSnd[byIdx]->SetNetSndParam( tNetSndPar );
		
		if ( MEDIANET_NO_ERROR != wRet )
		{
			g_cMpu2BasApp.StopVideoAdapter(m_byBasIdx,m_byChnId);
			ErrorLog( "CMpubasAdpGroup::Great-->m_cMediaSnd[%d].SetNetSndParam fail, Error code is: %d \n", byIdx, wRet );
//			printf( "CMpuBasAdpGroup::Great-->m_cMediaSnd[%d].SetNetSndParam fail, Error code is: %d \n", byIdx, wRet );
			break;
		}
		return TRUE;

	} while ( 0 );

	return FALSE;
}

/*=============================================================================
  函 数 名： SetAudioSend
  功    能： 音频空回调发送 支持
  算法实现： 
  全局变量： 
  参    数： void
  返 回 值： void 
=============================================================================*/
BOOL CMpu2BasAdpGroup::SetAudioSend(u8 byIdx, u32 dwNetBand, u16 wLocalStartPort, u32 dwDestIp, u16 wDestPort)
{
	mpulog( MPU_INFO, "[SetAudioSend] wLoacalStartPort:%u, dest:%s:%u\n", 
                       wLocalStartPort, StrOfIP(dwDestIp), wDestPort );

	u16 wRet = 0;

	do
	{        
        u8 byFrameRate = 50;
    	wRet = m_pcMediaAudSnd[byIdx]->Create( MAX_AUDIO_FRAME_SIZE,
		    	                        dwNetBand,
			                            byFrameRate,
			    				        m_byAudDecPT );

        if ( MEDIANET_NO_ERROR != wRet )
		{
         	g_cMpu2BasApp.StopVideoAdapter(m_byBasIdx,m_byChnId);
    		ErrorLog("SetAudioSend::Great-->m_pcMediaAudSnd[%d].Create fail, Error is:%d, NetBand.%d, FrmRate.%d, MediaType.%d \n", 
						byIdx, wRet, dwNetBand, byFrameRate, m_byAudDecPT );
//            printf( "SetAudioSend::Great-->m_pcMediaAudSnd[0].Create fail, Error is: %d \n", wRet );
    	   	break;
		}
        else
        {
            DetailLog("SetAudioSend::Great-->m_pcMediaAudSnd[%d].0x%x.Create succeed, NetBand.%d, FrmRate.%d, MediaType.%d\n",
						byIdx, m_pcMediaVidSnd[1], dwNetBand, byFrameRate, m_byAudDecPT );
//            printf( "SetAudioSend::Great-->m_pcMediaAudSnd[%d].Create succeed!\n" , byIdx);
        }

    	//设置发送参数
    	TNetSndParam tNetSndPar;
    	memset( &tNetSndPar, 0x00, sizeof(tNetSndPar) );

    	tNetSndPar.m_byNum  = 1;
    	tNetSndPar.m_tLocalNet.m_wRTPPort       = wLocalStartPort;
     	tNetSndPar.m_tLocalNet.m_wRTCPPort      = wLocalStartPort + 1;
    	tNetSndPar.m_tRemoteNet[0].m_dwRTPAddr  = htonl(dwDestIp);
    	tNetSndPar.m_tRemoteNet[0].m_wRTPPort   = wDestPort;
    	tNetSndPar.m_tRemoteNet[0].m_dwRTCPAddr = htonl(dwDestIp);
    	tNetSndPar.m_tRemoteNet[0].m_wRTCPPort  = wDestPort + 1;

    	wRet = m_pcMediaAudSnd[byIdx]->SetNetSndParam( tNetSndPar );

    	if ( MEDIANET_NO_ERROR != wRet )
		{
    		g_cMpu2BasApp.StopVideoAdapter(m_byBasIdx,m_byChnId);
    		ErrorLog("SetAudioSend::SetNetSndParam-->m_cMediaSnd[%d].SetNetSndParam fail, Error is: %d \n", 
					  byIdx, wRet );
//            printf( "SetAudioSend::SetNetSndParam-->m_cMediaSnd[%d].SetNetSndParam fail, Error is: %d \n", 
//					byIdx, wRet );
    		break;
		}

		return TRUE;

	} while ( 0 );

	return FALSE;
}

/*=============================================================================
  函 数 名： AddBasChn
  功    能： 添加编码通道
  算法实现： 
  全局变量： 
  参    数： void
  返 回 值： void 
=============================================================================*/
BOOL CMpu2BasAdpGroup::AddBasChn(u32 dwDecMediaType,TAdapterEncParam * ptVidEncParam, u8 byVailedChnNum)
{
	//添加通道
	TAdapterChannel tBasChnl;
	memset( &tBasChnl,0,sizeof(TAdapterChannel) );
	tBasChnl.m_dwAdapterChnlId = m_byChnId;
	tBasChnl.m_dwEncNum = byVailedChnNum;
	tBasChnl.m_dwDecMediaType = dwDecMediaType;
	tBasChnl.m_ptEncParam = ptVidEncParam;
	u16 wRet = 0;
	wRet = g_cMpu2BasApp.AddBasChnnl(m_byBasIdx,&tBasChnl);
	if ( (u16)Codec_Success != wRet )
	{
		ErrorLog( "[CMpu2BasAdpGroup::AddBasChn]AddBasChnnl:%d-%d fail, Error code is: %d \n", m_byBasIdx,m_byChnId,wRet );
		return FALSE;
	}
	else
	{
		KeyLog("[CMpu2BasAdpGroup::AddBasChn]AddBasChnnl:%d-%d success\n",m_byBasIdx,m_byChnId);
	}
	
	
	// 设置编码回调
	for (u8 byOutIdx = 0; byOutIdx < byVailedChnNum; byOutIdx++)
	{
		wRet = g_cMpu2BasApp.SetBasVidDataCallback(m_byBasIdx,m_byChnId,byOutIdx,CBMpu2BasSendFrame,m_pcMediaVidSnd[byOutIdx]);
		if ( (u16)Codec_Success != wRet )
		{
			ErrorLog("[CMpu2BasAdpGroup::AddBasChn]m_cAptGrp.SetBasVidDataCallback(%d ,%d,%d ) fail, Error code is: %d \n", m_byBasIdx, m_byChnId, byOutIdx, wRet );
//			printf("[CMpu2BasAdpGroup::AddBasChn]m_cAptGrp.SetBasVidDataCallback(%d ,%d,%d ) fail, Error code is: %d \n", m_byBasIdx, m_byChnId, byOutIdx,wRet);
			return FALSE;
		}
	}	

	//保存新参数
	memcpy( m_atVidEncParam, ptVidEncParam, sizeof(TAdapterEncParam) * byVailedChnNum );
	return TRUE;
}

/*=============================================================================
  函 数 名： AddNewEncParam
  功    能： 添加编码通道
  算法实现： 
  全局变量： 
  参    数： void
  返 回 值： void 
=============================================================================*/
BOOL CMpu2BasAdpGroup::AddNewEncParam(const TVideoEncParam &tVidEncParam, u8 byEncIdx)
{
	//添加新参数
	u16 wRet =0;
	wRet = g_cMpu2BasApp.AddNewEncParam(m_byBasIdx,m_byChnId,byEncIdx,tVidEncParam);
	if ( (u16)Codec_Success != wRet )
	{
		ErrorLog( "[CMpu2BasAdpGroup::AddNewEncParam]AddNewEncParam:%d-%d-%d fail, Error code is: %d \n", m_byBasIdx,m_byChnId,byEncIdx,wRet );
		return FALSE;
	}

	// 设置编码回调
	wRet = g_cMpu2BasApp.SetBasVidDataCallback(m_byBasIdx,m_byChnId,byEncIdx,CBMpu2BasSendFrame,m_pcMediaVidSnd[byEncIdx]);
	if ( (u16)Codec_Success != wRet )
	{
		ErrorLog("[CMpu2BasAdpGroup::AddNewEncParam]m_cAptGrp.SetBasVidDataCallback(%d ,%d,%d ) fail, Error code is: %d \n", m_byBasIdx, m_byChnId, byEncIdx, wRet );
//		printf("[CMpu2BasAdpGroup::AddNewEncParam]m_cAptGrp.SetBasVidDataCallback(%d ,%d,%d ) fail, Error code is: %d \n", m_byBasIdx, m_byChnId, byEncIdx,wRet);
		return FALSE;
	}

	//设置发送
	u16	wLocalSendPort =0;
#ifdef _8KI_
	TMpu2BasCfg tMpu2BasCfg;
	if ( g_cMpu2BasApp.GetMpu2BasCfg(0,tMpu2BasCfg) )
	{
		u8 byPos = 0;
		byPos = (m_wDestStartPort-tMpu2BasCfg.m_wMcuRcvStartPort)/PORTSPAN;
		wLocalSendPort = BAS_8KE_LOCALSND_PORT +  PORTSPAN *(byPos+byEncIdx);
	}
	else
	{
		ErrorLog("[CMpu2BasAdpGroup::AddNewEncParam]GetMpu2BasCfg error\n!");
	}
#else
	wLocalSendPort = m_wDestStartPort+ PORTSPAN * byEncIdx;
#endif
	
	//保存新参数
	m_atVidEncParam->m_dwEncoderId = byEncIdx;
	memcpy( &(m_atVidEncParam[byEncIdx].m_tVideoEncParam), &tVidEncParam, sizeof(TVideoEncParam) );

	if( !SetVideoEncSend(byEncIdx, m_dwNetBand, wLocalSendPort, m_dwDestIp, m_wDestStartPort+ PORTSPAN * byEncIdx) )
	{
		ErrorLog("[CMpu2BasAdpGroup::AddNewEncParam]SetVideoEncSend(%d ,%d,%d ) fail, Error code is: %d \n", m_byBasIdx, m_byChnId, byEncIdx,wRet);
//		printf("[CMpu2BasAdpGroup::AddNewEncParam]SetVideoEncSend(%d ,%d,%d ) fail, Error code is: %d \n", m_byBasIdx, m_byChnId, byEncIdx,wRet);
		return FALSE;
	}
	KeyLog("[CMpu2BasAdpGroup::AddNewEncParam]SetVideoEncSend(%d,%d,%d), LocalStartPort:%u, DestPort:%d\n", 
			m_byBasIdx,
			m_byChnId,
			byEncIdx,	
			wLocalSendPort,
			m_wDestStartPort+ PORTSPAN * byEncIdx
			);

	KeyLog("[CMpu2BasAdpGroup::AddNewEncParam]AddNewEncParam:%d-%d-%d success\n",m_byBasIdx,m_byChnId,byEncIdx);
	return TRUE;
}


/*=============================================================================
  函 数 名： IsAddNewParam
  功    能： 编码是否改变
  算法实现： 
  全局变量： 
  参    数： void
  返 回 值： void 
=============================================================================*/
BOOL CMpu2BasAdpGroup::IsAddNewParam(const TVideoEncParam &tOldParam, const TVideoEncParam &tNewParam)
{
	if ( TYPE_MPUBAS== g_cMpu2BasApp.GetBasWorkMode() ||
		TYPE_MPUBAS_H== g_cMpu2BasApp.GetBasWorkMode()
		)
	{
		if (  (tOldParam.m_byEncType != MEDIA_TYPE_NULL &&  tOldParam.m_byEncType != MEDIA_TYPE_NULL) &&
			(tNewParam.m_byEncType != MEDIA_TYPE_NULL &&  tNewParam.m_byEncType!=0 && tNewParam.m_byEncType != tOldParam.m_byEncType )
			)
		{
			return TRUE;	
		}
	}

	if (  (tOldParam.m_byEncType == MEDIA_TYPE_NULL || tOldParam.m_byEncType == 0) &&
		  (tNewParam.m_byEncType != MEDIA_TYPE_NULL&&tNewParam.m_byEncType!=0)
	   )
	{
		return TRUE;
	}

	return FALSE;
}

/*=============================================================================
  函 数 名： ConverToRealParam2
  功    能： 编码参数转换
  算法实现： 
  全局变量： 
  参    数： void
  返 回 值： void 
=============================================================================*/
void CMpu2BasAdpGroup::ConverToRealParam2(u8 byOutIdx, TAdapterEncParam &tVidEncParam, THDAdaptParam &tBasParam)
{
	if ( byOutIdx >= MAXNUM_BASOUTCHN )
	{
		ErrorLog("[ConverToRealParam2]byOutNum:%d is error!\n",byOutIdx);
		return;
	}

	ConverToRealParam(m_byBasIdx,m_byChnId,byOutIdx,tBasParam);
	tVidEncParam.m_tVideoEncParam.m_byFrameRate = tBasParam.GetFrameRate();
	tVidEncParam.m_tVideoEncParam.m_wVideoWidth = tBasParam.GetWidth();
	tVidEncParam.m_tVideoEncParam.m_wVideoHeight = tBasParam.GetHeight();
	return;
}

/*=============================================================================
  函 数 名： ConverToRealParam3
  功    能： 编码参数转换
  算法实现： 
  全局变量： 
  参    数： void
  返 回 值： void 
=============================================================================*/
void CMpu2BasAdpGroup::ConverToRealParam3(u8 byOutIdx,THDAdaptParam &tRealParam,THDAdaptParam &tSpecialParam,TAdapterEncParam &tVidEncParam)
{
	if ( byOutIdx >= MAXNUM_BASOUTCHN )
	{
		ErrorLog("[ConverToRealParam3]byOutNum:%d is error!\n",byOutIdx);
		return;
	}

	//[nizhijun 2012/09/25] 比较帧率和分辨率是否需要调整
	if ( IsNeedConvertToSpecialParam(tRealParam,tSpecialParam) )
	{
		KeyLog("[ConverToRealParam3]BasIdx:%d,ChnId:%d,outIdx:%d former param %d-%d-%d\n",
			m_byBasIdx,m_byChnId,byOutIdx,
			tVidEncParam.m_tVideoEncParam.m_byFrameRate,
			tVidEncParam.m_tVideoEncParam.m_wVideoWidth,
			tVidEncParam.m_tVideoEncParam.m_wVideoHeight	
			);	
		ConverToRealParam2(byOutIdx,tVidEncParam,tSpecialParam);
		KeyLog("[ConverToRealParam3]BasIdx:%d,ChnId:%d,outIdx:%d change to special param %d-%d-%d\n",
			m_byBasIdx,m_byChnId,byOutIdx,
			tVidEncParam.m_tVideoEncParam.m_byFrameRate,
			tVidEncParam.m_tVideoEncParam.m_wVideoWidth,
			tVidEncParam.m_tVideoEncParam.m_wVideoHeight	
			);
	}
	else
	{
		ConverToRealParam2(byOutIdx,tVidEncParam,tRealParam);
	}
	
	tVidEncParam.m_tVideoEncParam.m_wBitRate = tSpecialParam.GetBitrate();
	if ( g_cMpu2BasApp.IsEnableCheat())
	{
		u16 wBitrate = 	tVidEncParam.m_tVideoEncParam.m_wBitRate;
		wBitrate = g_cMpu2BasApp.GetDecRateDebug( wBitrate );
		tVidEncParam.m_tVideoEncParam.m_wBitRate = wBitrate;
	}
	return;
}

/*=============================================================================
  函 数 名： IsNeedConvertToSpecialParam
  功    能： 编码参数转换
  算法实现： 
  全局变量： 
  参    数： void
  返 回 值： void 
=============================================================================*/
BOOL32 CMpu2BasAdpGroup::IsNeedConvertToSpecialParam(THDAdaptParam &tRealParam,THDAdaptParam &tSpecialParam)
{
	//比较帧率
	if ( tSpecialParam.IsNull() )
	{
		return FALSE;
	}

	if ( tRealParam.GetFrameRate()> tSpecialParam.GetFrameRate()  )
	{
		return TRUE;
	}
				
	//比较分辨率乘积是否需要调小
	if ( (tRealParam.GetWidth()) *(tRealParam.GetHeight()) > (tSpecialParam.GetWidth())*(tSpecialParam.GetHeight())  )
	{
		return TRUE;
	}
	return FALSE;
}


/*=============================================================================
  函 数 名： IsParamChange
  功    能： 编码是否改变
  算法实现： 
  全局变量： 
  参    数： void
  返 回 值： void 
=============================================================================*/
BOOL CMpu2BasAdpGroup::IsParamChange(u8 byOutIdx,const TVideoEncParam &tOldParam, const TVideoEncParam &tNewParam)
{
	if ( tOldParam.m_byEncType != tNewParam.m_byEncType )
	{
		KeyLog("[IsParamChange]outIdx:%d m_byEncType old:%d,new:%d\n",byOutIdx,tOldParam.m_byEncType,tNewParam.m_byEncType);
		return TRUE;
	}

	if ( tOldParam.m_wVideoHeight != tNewParam.m_wVideoHeight )
	{
		KeyLog("[IsParamChange]outIdx:%d m_wVideoHeight old:%d,new:%d\n",byOutIdx,tOldParam.m_wVideoHeight,tNewParam.m_wVideoHeight);
		return TRUE;
	}

	if ( tOldParam.m_wVideoWidth != tNewParam.m_wVideoWidth )
	{
		KeyLog("[IsParamChange]outIdx:%d m_wVideoWidth old:%d,new:%d\n",byOutIdx,tOldParam.m_wVideoWidth,tNewParam.m_wVideoWidth);
		return TRUE;
	}

	if ( tOldParam.m_dwProfile != tNewParam.m_dwProfile )
	{
		KeyLog("[IsParamChange]outIdx:%d m_dwProfile old:%d,new:%d\n",byOutIdx,tOldParam.m_dwProfile,tNewParam.m_dwProfile );
		return TRUE;
	}

	if ( tOldParam.m_wBitRate != tNewParam.m_wBitRate )
	{
		KeyLog("[IsParamChange]outIdx:%d m_wBitRate old:%d,new:%d\n",byOutIdx,tOldParam.m_wBitRate,tNewParam.m_wBitRate);
		return TRUE;
	}

	if ( tOldParam.m_byFrameRate != tNewParam.m_byFrameRate )
	{
		KeyLog("[IsParamChange]outIdx:%d m_byFrameRate old:%d,new:%d\n",byOutIdx,tOldParam.m_byFrameRate,tNewParam.m_byFrameRate);
		return TRUE;
	}

	return FALSE;
}

/*=============================================================================
  函 数 名： SetNetRecvFeedbackVideoParam
  功    能： 设置网络接收重传参数
  算法实现： 
  全局变量： 
  参    数： void
  返 回 值： void 
=============================================================================*/
BOOL  CMpu2BasAdpGroup::SetNetRecvFeedbackVideoParam( TRSParam tNetRSParam, BOOL32 bRepeatSnd/* = FALSE */ )
{
	u16 wRet = 0;

	//保存参数
    m_bNeedRS = bRepeatSnd;
	memcpy( &m_tRsParam, &tNetRSParam, sizeof(m_tRsParam) );

    wRet = m_pcMediaVidRcv->ResetRSFlag( tNetRSParam, bRepeatSnd );
	if ( MEDIANET_NO_ERROR != wRet )
	{
		ErrorLog("CMpuBasAdpGroup::SetNetRecvFeedbackVideoParam-->m_pcMediaRcv->ResetRSFlag fail,Error code is:%d\n", wRet );
		return FALSE;
	}

	//nzj:添加音频接收 2010/9/6
	wRet = m_pcMediaAudRcv->ResetRSFlag( tNetRSParam, bRepeatSnd );
	if ( MEDIANET_NO_ERROR != wRet )
	{
		ErrorLog("CMpuBasAdpGroup::SetNetRecvFeedbackVideoParam-->m_pcMediaAudRcv->ResetRSFlag fail,Error code is:%d\n", wRet );
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
BOOL  CMpu2BasAdpGroup::SetNetSendFeedbackVideoParam( u16 wBufTimeSpan, u8 byVailedOutNum, BOOL32 bRepeatSnd/*= FALSE*/ )
{
	u16 wRet = 0;
	if (0 == byVailedOutNum || byVailedOutNum > m_byOutChnlNum)
	{
		WarningLog("[SetNetSendFeedbackVideoParam] byVailedOutNum is %d!\n", byVailedOutNum);
		return FALSE;
	}
	//保存参数
	for (u8 byIdx = 0; byIdx < byVailedOutNum; byIdx++)
	{
		//未创建发送对象的不设重传参数
		if (0 == m_atVidEncParam[byIdx].m_tVideoEncParam.m_wBitRate 
			||  MEDIA_TYPE_NULL == m_atVidEncParam[byIdx].m_tVideoEncParam.m_byEncType
			||  0 == m_atVidEncParam[byIdx].m_tVideoEncParam.m_byFrameRate)
		{
			continue;
		}

		m_awBufTime[byIdx] = wBufTimeSpan;
		
		wRet = m_pcMediaVidSnd[byIdx]->ResetRSFlag( wBufTimeSpan, bRepeatSnd );
		if ( MEDIANET_NO_ERROR != wRet )
		{
			ErrorLog("CMpu2BasAdpGroup::SetNetSendFeedbackVideoParam-->m_pcMediaVidSnd[%d].ResetRSFlag fail,Error code is:%d\n", byIdx, wRet );
			return FALSE;
		}		
		//nzj:添加音频发送 2010/9/6
		wRet = m_pcMediaAudSnd[byIdx]->ResetRSFlag( wBufTimeSpan, bRepeatSnd );
		if ( MEDIANET_NO_ERROR != wRet )
		{
			ErrorLog("CMpu2BasAdpGroup::SetNetSendFeedbackVideoParam-->m_pcMediaAudSnd[%d].ResetRSFlag fail,Error code is:%d\n", byIdx, wRet );
			return FALSE;
		}

	}
	return TRUE;
}

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
void CMpu2BasAdpGroup::SetSmoothSendRule( BOOL32 bNeedSet )
{
	DetailLog("[SetSmoothSendRule] IsSet.%d!\n", bNeedSet );
	
#ifdef _LINUX_
	s32 nRet = 0;
	if( bNeedSet )
	{
		for( u8 byOutIdx = 0; byOutIdx < m_byOutChnlNum; byOutIdx++ )
		{
			// 			u16 wBitrate = m_atVidEncParam[byOutIdx].m_tVideoEncParam.m_wBitRate;
			// 			// 还原打折前码率，平滑参数不能按照编码码率设置
			// 			wBitrate = g_cMpu2BasApp.GetOrigRate(wBitrate);
			//如果先前设置的码率大于0并且等于当前需要设置的码率，那么则不需要再次设置
			if ( m_awPreRawBiteRate[byOutIdx] > 0  )
			{
				if (  m_awPreRawBiteRate[byOutIdx] == m_awRawBiteRate[byOutIdx] )
				{
					continue;	
				}
			}
			u16 wBitrate = m_awRawBiteRate[byOutIdx];
			if ( wBitrate>0 )
			{
				// 转换成Byte
				wBitrate = wBitrate >> 3;
				// 峰值再增加20%
				u32 dwPeak = wBitrate + wBitrate / 5;
				
				DetailLog("[SetSmoothSendRule] Setting OutIdx:%d ssrule for 0x%x:%u, rate/peak=%u/%u KByte.\n",
					byOutIdx, m_dwDestIp, m_wDestStartPort + PORTSPAN * byOutIdx, wBitrate, dwPeak );
				
				nRet = BatchAddSSRule( m_dwDestIp, m_wDestStartPort + PORTSPAN * byOutIdx, wBitrate, dwPeak, 2 );   // default 2 second
				
				if ( 0 == nRet )
				{
					ErrorLog("[SetSmoothSendRule] Set OutIdx:%d rule failed. SSErrno=%d\n", byOutIdx, nRet );
				}
				else
				{
					m_awPreRawBiteRate[byOutIdx]= m_awRawBiteRate[byOutIdx];
				}
			}
		}
		nRet = BatchExecSSRule();
		if (0 == nRet)
		{
			ErrorLog( "[SetSmoothSendRule]BatchExecSSRule failed. SSErrno=%d. \n", SSErrno);
		}
	}
	else
	{
		for( u8 byOutIdx = 0; byOutIdx < m_byOutChnlNum; byOutIdx++ )
		{
			nRet = BatchAddUnsetSSRule( m_dwDestIp, m_wDestStartPort + PORTSPAN * byOutIdx );
			
			if ( 0 == nRet )
			{
				ErrorLog("[ClearSmoothSendRule] Clear OutIdx:%d rule failed. SSErrno=%d\n", byOutIdx, nRet );
			}
		}
		nRet = BatchExecUnsetSSRule();
		if (0 == nRet)
		{
			ErrorLog( "[SetSmoothSendRule]BatchExecUnsetSSRule failed. SSErrno=%d. \n", SSErrno);
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
BOOL CMpu2BasAdpGroup::StopAdapter( BOOL32 bStopMediaRcv )
{
	u16 wRet = 0;
	DetailLog("CMpuBasAdpGroup::StopAdapter\n" );
	if(!IsStart())
	{
    	DetailLog("CMpuBasAdpGroup::StopAdapter success\n" );
		return TRUE;
	}
	if ( bStopMediaRcv )
	{
		m_pcMediaVidRcv->StopRcv();
		m_pcMediaAudRcv->StopRcv();
	}
	
	wRet = 	g_cMpu2BasApp.StopVideoAdapter(m_byBasIdx,m_byChnId);
	
	if ( (u16)Codec_Success != wRet )
	{
        ErrorLog("CMpuBasAdpGroup::StopAdapter-->m_cAptGrp.StopGroup fail, Error code is: %d \n", wRet );
		return FALSE;
	}

	wRet = 	g_cMpu2BasApp.RemoveBasChnnl(m_byBasIdx,m_byChnId);
	
	if ( (u16)Codec_Success != wRet )
	{
        ErrorLog("CMpuBasAdpGroup::StopAdapter-->m_cAptGrp.RemoveBasChnnl fail, Error code is: %d \n", wRet );
		return FALSE;
	}

	SetStart(FALSE);

	memset(	m_awRawBiteRate,0,sizeof(m_awRawBiteRate) );
	memset(	m_awPreRawBiteRate,0,sizeof(m_awPreRawBiteRate) );
	memset(m_atVidEncParam,0,sizeof(m_atVidEncParam));
   	DetailLog("CMpuBasAdpGroup::StopAdapter success\n" );	
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
BOOL CMpu2BasAdpGroup::SetFastUpdata()
{
	DetailLog("CMpu2BasAdpGroup::SetFastUpdata(basidx:%d, chnid:%d)!\n",m_byBasIdx,m_byChnId );

    u16 dwErrorcode = CODEC_NO_ERROR;
    dwErrorcode = g_cMpu2BasApp.SetEncFastUpdata(m_byBasIdx,m_byChnId);
    if (dwErrorcode != CODEC_NO_ERROR)
    {
        ErrorLog( "[SetFastUpdata] For m_byChnId.%d fail(errcode:%d)\n", m_byChnId, dwErrorcode);
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
BOOL CMpu2BasAdpGroup::GetVideoChannelStatis(TKdvDecStatis &tAdapterDecStatis )
{
	u16 wRet = 0;
	wRet = g_cMpu2BasApp.GetBasChnDecStatis(m_byBasIdx,m_byChnId,tAdapterDecStatis);

	if ( (u16)Codec_Success != wRet )
	{
		ErrorLog( "CMpu2BasAdpGroup::GetVideoChannelStatis-->m_cAptGrp.GetVideoChannelStatis fail! Error code is:%d\n", wRet );
		return FALSE;
	}
	else
	{
		DetailLog("CMpu2BasAdpGroup::GetVideoChannelStatis-->m_cAptGrp.GetVideoChannelStatis %d-%d-%d\n", m_byBasIdx,m_byChnId,tAdapterDecStatis.m_bVidCompellingIFrm);
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
void CMpu2BasAdpGroup::ShowChnInfo( )
{
    StaticLog("\n----------------Adapter Group Info-------------------------\n");
    
    //基本信息
    StaticLog("\n*************Basic info*************\n");
    StaticLog("\t Is Start:%d\n", m_bIsStart);	
    if ( !m_bIsStart) 
    {
        return;
    }

	StaticLog("\t Is m_byWorkMode:%d\n", m_byWorkMode);
	StaticLog("\t Is m_byBasIdx:%d\n", m_byBasIdx);
	StaticLog("\t Is m_byChnId:%d\n", m_byChnId);
	StaticLog("\t Is m_byOutChnlNum:%d\n", m_byOutChnlNum);
    StaticLog("\t Is m_wLoclRcvStartPort:%d\n", m_wLoclRcvStartPort);
	StaticLog("\t Is m_dwDestIp:%d\n", m_dwDestIp);
	StaticLog("\t Is m_wDestStartPort:%d\n", m_wDestStartPort);

    //编码参数
    StaticLog("\n*************ENC Parmater***********\n");
    
    for(u32 dwIdx = 0; dwIdx < m_byOutChnlNum; dwIdx++)
    {
		StaticLog("-----\t EncOutIdx:%u--------\n", dwIdx);
		StaticLog("\t EncType:%u\n", m_atVidEncParam[dwIdx].m_tVideoEncParam.m_byEncType);
        StaticLog("\t VideoWidth:%u\n", m_atVidEncParam[dwIdx].m_tVideoEncParam.m_wVideoWidth);
        StaticLog("\t VideoHeight:%u\n", m_atVidEncParam[dwIdx].m_tVideoEncParam.m_wVideoHeight);
        StaticLog("\t ProfileType:%d\n", m_atVidEncParam[dwIdx].m_tVideoEncParam.m_dwProfile);
        StaticLog("\t BitRate:%u\n",m_atVidEncParam[dwIdx].m_tVideoEncParam.m_wBitRate);
		StaticLog("\t FrameRate:%u\n", m_atVidEncParam[dwIdx].m_tVideoEncParam.m_byFrameRate);
        StaticLog("\t MaxKeyFrameInterval:%u\n", m_atVidEncParam[dwIdx].m_tVideoEncParam.m_dwMaxKeyFrameInterval);
        StaticLog("\t RcMode:%u\n", m_atVidEncParam[dwIdx].m_tVideoEncParam.m_byRcMode);
        StaticLog("\t MaxQuant:%u\n", m_atVidEncParam[dwIdx].m_tVideoEncParam.m_byMaxQuant);
        StaticLog("\t MinQuant:%u\n", m_atVidEncParam[dwIdx].m_tVideoEncParam.m_byMinQuant);
        StaticLog("\t FrameFmt:%u\n", m_atVidEncParam[dwIdx].m_tVideoEncParam.m_byFrameFmt);
    }    
	g_cMpu2BasApp.ShowBasChnStatus(m_byBasIdx,m_byChnId,TRUE);
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
BOOL CMpu2BasAdpGroup::SetAudioParam(u8 byAudDecType)
{	
	if (MEDIA_TYPE_NULL == byAudDecType)
	{
		ErrorLog( "CMpuBasAdpGroup::SetAudioParam-->m_byAudDecPT is NULL!\n" );
		return FALSE;
	}

    if (m_byAudDecPT == byAudDecType)
    {
        return TRUE;
    }

    m_byAudDecPT = byAudDecType;

	for (u8 byOutIdx = 0; byOutIdx < m_byOutChnlNum; byOutIdx++)
	{
		u16	wLocalSendPort =0;
#ifdef _8KI_
		TMpu2BasCfg tMpu2BasCfg;
		if ( g_cMpu2BasApp.GetMpu2BasCfg(0,tMpu2BasCfg) )
		{
			u8 byPos = 0;
			byPos = (m_wDestStartPort-tMpu2BasCfg.m_wMcuRcvStartPort)/PORTSPAN;
			wLocalSendPort = BAS_8KE_LOCALSND_PORT +  PORTSPAN *(byPos+byOutIdx) + 4;//55604
		}
		else
		{
			ErrorLog("[CMpu2BasAdpGroup::SetAudioParam]GetMpu2BasCfg error\n!");
		}
#else
		wLocalSendPort = m_wDestStartPort + PORTSPAN * byOutIdx+4;//53004
#endif
		if (!SetAudioSend(  byOutIdx,
							m_dwNetBand,
							wLocalSendPort,
							m_dwDestIp,
							m_wDestStartPort + PORTSPAN * byOutIdx + 2) )
		{
			ErrorLog("CMpuBasAdpGroup::SetAudioParam-->SetAudioSend fail\n" );
			return FALSE;
		}
	}
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
BOOL CMpu2BasAdpGroup::SetVideoEncParam(  u32 dwDecMediaType,
										 TAdapterEncParam * aptVidEncParam,
										 THDAdaptParam	*aptBasParam,
										 THDAdaptParam *aptSpecialBasParam,
										 u8				 byVailedChnNum,
									     BOOL32		     bSetNetParam,
										 BOOL32 bChangeParam/* =FALSE*/)

{
	KeyLog( "[SetVideoEncParam]dwDecMediaType:%d,byVailedChnNum:%d,bSetNetParam:%d,bChangeParam:%d\n",
				dwDecMediaType, byVailedChnNum, bSetNetParam, bChangeParam
			 );
	if ( NULL == aptVidEncParam )
	{
		ErrorLog("[SetVideoEncParam]aptVidEncParam is NULL!\n" );
		return FALSE;
	}

	if ( NULL == aptBasParam )
	{
		ErrorLog("[SetVideoEncParam]aptBasParam is NULL!\n" );
		return FALSE;
	}
	
	if ( NULL == aptSpecialBasParam )
	{
		ErrorLog("[SetVideoEncParam]aptSpecialBasParam is NULL!\n" );
		return FALSE;
	}
	
	for ( u8 byOutIdx =0;  byOutIdx<byVailedChnNum; byOutIdx++)
	{
		m_awRawBiteRate[byOutIdx] = aptBasParam[byOutIdx].GetBitrate();
	}

	BOOL32 bIsRetOk = FALSE;
	u16 wRet = 0;
	do
	{
		if ( !bChangeParam )
		{
			//先设置参数AddBasChn，最后再根据原先码率来调整分辨率，故此处先用临时变量添加通道
			TAdapterEncParam tTempVidEncParam[MAXNUM_BASOUTCHN];
			memset(tTempVidEncParam, 0, sizeof(TAdapterEncParam)*MAXNUM_BASOUTCHN);
			memcpy(tTempVidEncParam, aptVidEncParam, sizeof(TAdapterEncParam)*byVailedChnNum);
			u8 byIdx = 0;
			if ( g_cMpu2BasApp.IsEnableCheat())
			{
				for ( byIdx = 0; byIdx < byVailedChnNum; byIdx++)
				{
					u16 wBitrate = tTempVidEncParam[byIdx].m_tVideoEncParam.m_wBitRate;
					wBitrate = g_cMpu2BasApp.GetDecRateDebug( wBitrate );
					tTempVidEncParam[byIdx].m_tVideoEncParam.m_wBitRate = wBitrate;
				}
			}
		
			if ( !AddBasChn(dwDecMediaType,tTempVidEncParam,byVailedChnNum) )
			{
				ErrorLog("[CMpu2BasAdpGroup::AddBasChn] failed!\n" );
				break;
			}

			//添加完通道后，再进行转换
			if ( NULL == aptBasParam )
			{
				ErrorLog("[SetVideoEncParam]aptBasParam is NULL!\n" );
				break;
			}

			for ( byIdx = 0; byIdx < byVailedChnNum; byIdx++)
			{		
				ConverToRealParam3(byIdx,aptBasParam[byIdx],aptSpecialBasParam[byIdx],aptVidEncParam[byIdx]);

				if ( IsParamChange( byIdx, m_atVidEncParam[byIdx].m_tVideoEncParam, aptVidEncParam[byIdx].m_tVideoEncParam) )
				{
					wRet = g_cMpu2BasApp.ChangeBasVideoEncParam(m_byBasIdx,m_byChnId,aptVidEncParam[byIdx]);
					if ( (u16)Codec_Success != wRet )
					{
						ErrorLog( "[CMpu2BasAdpGroup::SetVideoEncParam]after AddBasChn m_cAptGrp.ChangeVideoEncParam:%d-%d-%d fail, Error code is: %d \n", 
							m_byBasIdx,m_byChnId,byIdx,wRet );
						break;
					}
					else
					{
						memcpy( &(m_atVidEncParam[byIdx].m_tVideoEncParam), &(aptVidEncParam[byIdx].m_tVideoEncParam), sizeof(TVideoEncParam));
						KeyLog("[CMpu2BasAdpGroup::SetVideoEncParam]after AddBasChn Change:%d-%d-%d param success\n",m_byBasIdx,m_byChnId,byIdx);
					}
				}
			}
			
		}
		else
		{
			for ( u8 byLoop = 0; byLoop<byVailedChnNum; byLoop++ )
			{
				if ( IsAddNewParam(m_atVidEncParam[byLoop].m_tVideoEncParam,aptVidEncParam[byLoop].m_tVideoEncParam) )
				{
					KeyLog("[CMpu2BasAdpGroup::SetVideoEncParam]new param:out:%d,type:%d,width:%d,height:%d,profietype:%d,framerate:%d,biterate:%d\n",
							byLoop,
							aptVidEncParam[byLoop].m_tVideoEncParam.m_byEncType,
							aptVidEncParam[byLoop].m_tVideoEncParam.m_wVideoWidth,
							aptVidEncParam[byLoop].m_tVideoEncParam.m_wVideoHeight,
							aptVidEncParam[byLoop].m_tVideoEncParam.m_dwProfile,
							aptVidEncParam[byLoop].m_tVideoEncParam.m_byFrameRate,
							aptVidEncParam[byLoop].m_tVideoEncParam.m_wBitRate
						  );
					
					TAdapterEncParam tTempNewVidEncParam =aptVidEncParam[byLoop] ;
					u16 wBitrate = tTempNewVidEncParam.m_tVideoEncParam.m_wBitRate;
					wBitrate = g_cMpu2BasApp.GetDecRateDebug( wBitrate );
					tTempNewVidEncParam.m_tVideoEncParam.m_wBitRate = wBitrate;
					if ( !AddNewEncParam( tTempNewVidEncParam.m_tVideoEncParam,byLoop ) )
					{
						ErrorLog("[CMpu2BasAdpGroup::SetVideoEncParam]basidx:%d--chnid:%d--outidx:%d AddNewEncParam fail\n",
								m_byBasIdx,m_byChnId,byLoop
								);
						return FALSE;
					}

					ConverToRealParam3(byLoop,aptBasParam[byLoop],aptSpecialBasParam[byLoop],aptVidEncParam[byLoop]);
					
					if ( IsParamChange( byLoop, m_atVidEncParam[byLoop].m_tVideoEncParam, aptVidEncParam[byLoop].m_tVideoEncParam) )
					{
						wRet = g_cMpu2BasApp.ChangeBasVideoEncParam(m_byBasIdx,m_byChnId,aptVidEncParam[byLoop]);
						if ( (u16)Codec_Success != wRet )
						{
							ErrorLog( "[CMpu2BasAdpGroup::SetVideoEncParam]after AddBasChn m_cAptGrp.ChangeVideoEncParam:%d-%d-%d fail, Error code is: %d \n", 
								m_byBasIdx,m_byChnId,byLoop,wRet );
							break;
						}
						else
						{
							memcpy( &(m_atVidEncParam[byLoop].m_tVideoEncParam), &(aptVidEncParam[byLoop].m_tVideoEncParam), sizeof(TVideoEncParam));
							KeyLog("[CMpu2BasAdpGroup::SetVideoEncParam]after AddBasChn Change:%d-%d-%d param success\n",m_byBasIdx,m_byChnId,byLoop);
						}
					}

					//添加新编码参数成功后，就直接返回
					return	TRUE;
				}
				else
				{
					ConverToRealParam3(byLoop,aptBasParam[byLoop],aptSpecialBasParam[byLoop],aptVidEncParam[byLoop]);
					
					if ( IsParamChange( byLoop, m_atVidEncParam[byLoop].m_tVideoEncParam, aptVidEncParam[byLoop].m_tVideoEncParam) )
					{
						wRet = g_cMpu2BasApp.ChangeBasVideoEncParam(m_byBasIdx,m_byChnId,aptVidEncParam[byLoop]);
						if ( (u16)Codec_Success != wRet )
						{
							ErrorLog( "[CMpu2BasAdpGroup::SetVideoEncParam]m_cAptGrp.ChangeVideoEncParam:%d-%d-%d fail, Error code is: %d \n", 
								m_byBasIdx,m_byChnId,byLoop,wRet );
							break;
						}
						else
						{
							memcpy( &(m_atVidEncParam[byLoop].m_tVideoEncParam), &(aptVidEncParam[byLoop].m_tVideoEncParam), sizeof(TVideoEncParam));
							KeyLog("[CMpu2BasAdpGroup::SetVideoEncParam]Change:%d-%d-%d param success\n",m_byBasIdx,m_byChnId,byLoop);
						}
					}
				}
			}		
		}
	
		//网络参数在开启的时候作单次设置就可以了
		if ( bSetNetParam)
		{
			for (u8 byIdx = 0; byIdx < m_byOutChnlNum; byIdx++)
			{
				if ( MEDIA_TYPE_NULL == m_atVidEncParam[byIdx].m_tVideoEncParam.m_byEncType
					||  0 == m_atVidEncParam[byIdx].m_tVideoEncParam.m_byFrameRate)
				{
					KeyLog("[CMpu2BasAdpGroup::SetVideoEncParam]basidx:%d,chnid:%d,outidx:%d,m_wBitRate:%d,m_byEncType:%d,m_byFrameRate:%d continue\n",
								m_byBasIdx,m_byChnId,byIdx,
								m_atVidEncParam[byIdx].m_tVideoEncParam.m_wBitRate,
								m_atVidEncParam[byIdx].m_tVideoEncParam.m_byEncType,
								m_atVidEncParam[byIdx].m_tVideoEncParam.m_byFrameRate
								);
					continue;
				}
				//BAS本地发送端口8000A的话与转发板目的端口相同,但8KI不能相同，故此处重新计算
				u16	wLocalSendPort =0;
#ifdef _8KI_
				TMpu2BasCfg tMpu2BasCfg;
				if ( g_cMpu2BasApp.GetMpu2BasCfg(0,tMpu2BasCfg) )
				{
					u8 byPos = 0;
					byPos = (m_wDestStartPort-tMpu2BasCfg.m_wMcuRcvStartPort)/PORTSPAN;
					wLocalSendPort = BAS_8KE_LOCALSND_PORT +  PORTSPAN *(byPos+byIdx);
				}
				else
				{
					ErrorLog("[CMpu2BasAdpGroup::SetAudioParam]GetMpu2BasCfg error\n!");
				}
#else
				wLocalSendPort = m_wDestStartPort+ PORTSPAN * byIdx;
#endif
				if( !SetVideoEncSend(byIdx, m_dwNetBand, wLocalSendPort, m_dwDestIp, m_wDestStartPort+ PORTSPAN * byIdx) )
				{
					ErrorLog("CMpuBasAdpGroup::ChangeVideoEncParam-->SetVideoEncSend <OutIdx:%d>fail\n", byIdx);
					break;
				}
				KeyLog("[SetVideoEncParam]SetVideoEncSend(%d-%d-%d), LocalStartPort:%u, DestPort:%d\n", 
							m_byBasIdx,
							m_byChnId,
							byIdx,	
							wLocalSendPort,
							m_wDestStartPort+ PORTSPAN * byIdx
						);
			}
		}

		bIsRetOk = TRUE;
		break;

	} while ( 0 );
	
	return bIsRetOk;
}

/*=============================================================================
  函 数 名： SetKeyAndPT
  功    能： 
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void 
=============================================================================*/
void CMpu2BasAdpGroup::SetKeyAndPT( u8* abyKeyBuf,			 //加密字符串
								    u8 byKenLen,		 	 //密码字符串长度
									u8 byEncryptMode,		 //加密模式
									u8 *pbySndActive,		 //发送码流的载荷
									u8 byVailedOutNum,		 //发送码流路数
									u8 byActivePT /* = 0 */,  //接收码流的载荷
									u8 byRealPT /* = 0 */     //接收码流的真实载荷
								  )
{
	if (0 == byVailedOutNum)
	{
		WarningLog("[SetKeyandPT] byVailedOutNum is 0\n");
		return;
	}
	// 解码
	m_pcMediaVidRcv->SetActivePT( byActivePT, byRealPT );
	m_pcMediaVidRcv->SetDecryptKey( (s8*)abyKeyBuf, byKenLen, byEncryptMode );
	KeyLog("[CMpu2BasAdpGroup::SetKeyAndPT]basidx:%d bychnid:%d byActivePT:%d,byRealPT:%d\n",
			m_byBasIdx,m_byChnId,byActivePT,byRealPT
			);
	// 编码
	for (u8 byIdx = 0; byIdx < byVailedOutNum; byIdx++)
	{		
		m_pcMediaVidSnd[byIdx]->SetActivePT(pbySndActive[byIdx]);
		m_pcMediaVidSnd[byIdx]->SetEncryptKey( (s8*)abyKeyBuf, byKenLen, byEncryptMode );
	}
}

u16 CMpu2BasAdpGroup::SetResizeMode(u32 dwResizeMode)
{
	return g_cMpu2BasApp.SetEncResizeMode(m_byBasIdx,m_byChnId,dwResizeMode);
}

u16 CMpu2BasAdpGroup::SetAdpGrpData(const TFrameHeader& tFrameInfo)
{
	DetailLog("[SetAdpGrpData]m_byBasIdx:%d,m_byChnId:%d\n",m_byBasIdx,m_byChnId);
	return g_cMpu2BasApp.SetFrameData(m_byBasIdx,m_byChnId,tFrameInfo);
}

/*=============================================================================
  函 数 名： ModNetSndIpAddr
  功    能： 
  算法实现： 
  全局变量： 
  参    数： void
  返 回 值： void 
=============================================================================*/
void CMpu2BasAdpGroup::ModNetSndIpAddr(u32 dwSndIp)
{	
	m_dwDestIp = dwSndIp;
	TNetSndParam tSndParam;
	for (u8 byIdx = 0; byIdx < m_byOutChnlNum; byIdx++)
	{
		if (m_pcMediaVidSnd[byIdx] != NULL)
		{
			m_pcMediaVidSnd[byIdx]->GetNetSndParam(&tSndParam);
			tSndParam.m_tRemoteNet[0].m_dwRTPAddr  = htonl(dwSndIp);
			tSndParam.m_tRemoteNet[0].m_dwRTCPAddr = htonl(dwSndIp);
			m_pcMediaVidSnd[byIdx]->SetNetSndParam(tSndParam);
		}

		if (m_pcMediaAudSnd[byIdx] != NULL)
		{
			m_pcMediaAudSnd[byIdx]->GetNetSndParam(&tSndParam);
			tSndParam.m_tRemoteNet[0].m_dwRTPAddr  = htonl(dwSndIp);
			tSndParam.m_tRemoteNet[0].m_dwRTCPAddr = htonl(dwSndIp);
			m_pcMediaAudSnd[byIdx]->SetNetSndParam(tSndParam);
		}
	}
}

/*=============================================================================
函 数 名： SetBasData
功    能： 设置BAS外设配置数据
算法实现： 
全局变量： 
参    数： TMpu2Cfg *pMpu2Cfg 外设配置指针
返 回 值： void 
=============================================================================*/
void CMpu2BasData::SetBasData(TMpu2Cfg *pMpu2Cfg)
{
	if ( NULL == pMpu2Cfg )
	{
		return;
	}
	SetBasWorkMode(pMpu2Cfg->GetMpu2WorkMode());
	SetBasNum(pMpu2Cfg->GetMpu2EqpNum());

	for ( u8 byIdx = 0; byIdx < pMpu2Cfg->GetMpu2EqpNum(); byIdx++ )
	{
		m_atMpu2BasCfg[byIdx].m_byRegAckNum = 0;
		m_atMpu2BasCfg[byIdx].m_tCfg = pMpu2Cfg->GetMpu2EqpCfg(byIdx);
		//memcpy(&(m_atMpu2BasCfg[byIdx].m_tCfg), &(pMpu2Cfg->GetMpu2EqpCfg(byIdx)), sizeof(TEqpCfg));
	}
	
    return;
}
/*=============================================================================
函 数 名： GetMpu2BasCfg
功    能： 根据idx获得BAS外设配置，通过引用返回
算法实现： 
全局变量： 
参    数： 
返 回 值： BOOL32  获得失败
=============================================================================*/
void CMpu2BasData::SetMpu2BasCfg(u8 byIdx, const TMpu2BasCfg &tMpu2BasCfg)
{
	memcpy( &m_atMpu2BasCfg[byIdx], &(tMpu2BasCfg), sizeof(TMpu2BasCfg) );
	return;
}


/*=============================================================================
函 数 名： InitVideoAdapter
功    能： 初始化BAS适配器
算法实现： 
全局变量： 
参    数：u8 byIdx 表示第几个BAS外设 
返 回 值：u16 
=============================================================================*/
u16	CMpu2BasData::InitVideoAdapter(u8 byBasIdx)
{
	if ( byBasIdx >=MAX_BAS_NUM )
	{
		return 0XFFFF;
	}
	u32 dwAdapterMode;
	if ( TYPE_MPU2BAS_BASIC == m_byWorkMode )
	{
		dwAdapterMode = BAS_BASIC;
	}
	else if ( TYPE_MPU2BAS_ENHANCED == m_byWorkMode )
	{
		dwAdapterMode = BAS_ENHANCED;
	}
	else if ( TYPE_MPUBAS == m_byWorkMode )				//存在mpu2模拟mpu bap4时使用BAS_BASIC模式
	{
		dwAdapterMode = BAS_BASIC;
	}
	else if( TYPE_MPUBAS_H == m_byWorkMode )
	{
		//todo 存在mpu2模拟mpu bap2模式时，4.7封闭使用MPU2_SIMULATE_MPU_BASIC模式
		//4.7暂时还是用BAS_BASIC，凡超会优化
		dwAdapterMode = BAS_BASIC;		
	}
	else
	{
		printf( "[InitVideoAdapter]m_byWorkMode:%d is wrong\n",m_byWorkMode);
		return (u16)Codec_Error_Base;
	}

	TVidAdapterCreate tAdapteCreate;
	tAdapteCreate.m_dwDevVersion = (u32)en_MPU2_Board;
	tAdapteCreate.m_dwMode = dwAdapterMode;
	printf( "[InitVideoAdapter]tAdapteCreate.m_dwMode:%d\n",dwAdapterMode);
	return m_cAptGrp[byBasIdx].Create(tAdapteCreate);
}

/*=============================================================================
函 数 名： DestroyVideoAdapter
功    能： 退出
算法实现： 
全局变量： 
参    数：u8 byIdx 表示第几个BAS外设 
返 回 值：
=============================================================================*/
u16 CMpu2BasData::DestroyVideoAdapter(u8 byBasIdx)
{
	if ( byBasIdx >=MAX_BAS_NUM )
	{
		return 0XFFFF;
	}
	return m_cAptGrp[byBasIdx].Destroy();	
}

/*=============================================================================
函 数 名： StartVideoAdapter
功    能： 开始适配
算法实现： 
全局变量： 
参    数：u8 byIdx 表示第几个BAS外设 
		  u8 byChnId 表示通道ID
返 回 值：
=============================================================================*/
u16 CMpu2BasData::StartVideoAdapter(u8 byBasIdx,u8 byChnId)
{
	if ( byBasIdx >=MAX_BAS_NUM )
	{
		return 0XFFFF;
	}

	return m_cAptGrp[byBasIdx].Start(byChnId);	
}

/*=============================================================================
函 数 名： StopVideoAdapter
功    能： 开始适配
算法实现： 
全局变量： 
参    数：u8 byIdx 表示第几个BAS外设 
		  u8 byChnId 表示通道ID
返 回 值：
=============================================================================*/
u16 CMpu2BasData::StopVideoAdapter(u8 byBasIdx,u8 byChnId)
{
	if ( byBasIdx >=MAX_BAS_NUM )
	{
		return 0XFFFF;
	}
	DetailLog("[CMpu2BasData::StopVideoAdapter]stop byBasIdx:%d,byChnId:%d\n",byBasIdx,byChnId);
	return m_cAptGrp[byBasIdx].Stop(byChnId);	
}

/*=============================================================================
函 数 名： AddBasChnnl
功    能： 添加一个BAS通道
算法实现： 
全局变量： 
参    数：u8 byIdx 表示第几个BAS外设 
		  u8 byChnId 表示通道ID
返 回 值：
=============================================================================*/
u16 CMpu2BasData::AddBasChnnl(u8 byBasIdx,const TAdapterChannel *ptAdpChannel)
{
	if ( byBasIdx >=MAX_BAS_NUM )
	{
		return 0XFFFF;
	}
	DetailLog("[AddBasChnnl]add byBasIdx:%d,byChnId:%d,byOutNum:%d\n",byBasIdx,ptAdpChannel->m_dwAdapterChnlId,ptAdpChannel->m_dwEncNum);
	return m_cAptGrp[byBasIdx].AddVideoChannel(ptAdpChannel);
}

/*=============================================================================
函 数 名： AddNewEncParam
功    能： 添加一个BAS通道
算法实现： 
全局变量： 
参    数：u8 byIdx 表示第几个BAS外设 
		  u8 byChnId 表示通道ID
返 回 值：
=============================================================================*/
u16 CMpu2BasData::AddNewEncParam(u8 byBasIdx,u8 byChnId,u8 byOutIdx,const TVideoEncParam &tVidEncParam)
{
	if ( byBasIdx >=MAX_BAS_NUM )
	{
		return 0XFFFF;
	}
	DetailLog("[AddNewEncParam]add byBasIdx:%d,byChnId:%d,byOutIdx:%d\n",byBasIdx,byChnId,byOutIdx);
	TVideoEncParam tTempVidEncParam;
	memcpy(&tTempVidEncParam,&tVidEncParam,sizeof(TVideoEncParam));
	return m_cAptGrp[byBasIdx].AddVideoEncParam(byChnId,&tTempVidEncParam,byOutIdx);
}

/*=============================================================================
函 数 名： RemoveBasChnnl
功    能： 删除一个BAS通道
算法实现： 
全局变量： 
参    数：u8 byIdx 表示第几个BAS外设 
		  u8 byChnId 表示通道ID
返 回 值：
=============================================================================*/
u16 CMpu2BasData::RemoveBasChnnl(u8 byBasIdx,u8 byChnId)
{
	if ( byBasIdx >=MAX_BAS_NUM )
	{
		return 0XFFFF;
	}
	DetailLog("[CMpu2BasData::RemoveBasChnnl]remove byBasIdx:%d,byChnId:%d\n",byBasIdx,byChnId);
	return m_cAptGrp[byBasIdx].RemoveVideoChannel(byChnId);
}

/*=============================================================================
函 数 名： GetBasChnDecStatis
功    能： 获得某个通道的解码状态，用于请求关键帧
算法实现： 
全局变量： 
参    数：u8 byIdx 表示第几个BAS外设 
		  u8 byChnId 表示通道ID
		  TKdvDecStatis &tVidDecStatis 返回的解码状态
返 回 值：
=============================================================================*/
u16 CMpu2BasData::GetBasChnDecStatis(u8 byBasIdx,u8 byChnId,TKdvDecStatis &tVidDecStatis)
{
	if ( byBasIdx >=MAX_BAS_NUM )
	{
		return 0XFFFF;
	}
	
	return m_cAptGrp[byBasIdx].GetVidDecStatis(byChnId,tVidDecStatis);
}

/*=============================================================================
函 数 名： ChangeBasVideoEncParam
功    能： //BAS编码下参使用
算法实现： 
全局变量： 
参    数：u8 byIdx 表示第几个BAS外设 
		  u8 byChnId 表示通道ID
		  TVideoEncParam* ptVidEncParam 编码参数
		  u32 dwEncNum 编码路数
返 回 值：
=============================================================================*/
u16 CMpu2BasData::ChangeBasVideoEncParam(u8 byBasIdx,u8 byChnId, TAdapterEncParam &tVidEncParam)
{
	if ( byBasIdx >=MAX_BAS_NUM )
	{
		return 0XFFFF;
	}
	KeyLog("[ChangeBasVideoEncParam]byBasIdx:%d,byChnId:%d,outidx:%d,tVidEncParam:type:%d-framerate:%d-height:%d-width:%d-biterate:%d-profiletype:%d\n",
			byBasIdx,byChnId,tVidEncParam.m_dwEncoderId,
			tVidEncParam.m_tVideoEncParam.m_byEncType,
			tVidEncParam.m_tVideoEncParam.m_byFrameRate,
			tVidEncParam.m_tVideoEncParam.m_wVideoHeight,
			tVidEncParam.m_tVideoEncParam.m_wVideoWidth,
			tVidEncParam.m_tVideoEncParam.m_wBitRate,
			tVidEncParam.m_tVideoEncParam.m_dwProfile
			);
	return m_cAptGrp[byBasIdx].ChangeVideoEncParam(byChnId,&(tVidEncParam.m_tVideoEncParam), tVidEncParam.m_dwEncoderId);
}

/*=============================================================================
函 数 名： SetBasVidDataCallback
功    能： 设置视频数据回调，编码器
算法实现： 
全局变量： 
参    数：u8 byIdx 表示第几个BAS外设 
		  u8 byChnId 表示通道ID
		  u32 dwEncoderId 编码输出ID
		  FRAMECALLBACK fVidData 回调函数
		  void* pContext 发送对象指针
返 回 值：
=============================================================================*/
u16 CMpu2BasData::SetBasVidDataCallback(u8 byBasIdx,u8 byChnId, u32 dwEncoderId, FRAMECALLBACK fVidData, void* pContext)
{
	if ( byBasIdx >=MAX_BAS_NUM )
	{
		return 0XFFFF;
	}
	
	return m_cAptGrp[byBasIdx].SetVidDataCallback(byChnId,dwEncoderId,fVidData,pContext);
}

/*=============================================================================
函 数 名： SetFrameData
功    能： 设置帧数据
算法实现： 
全局变量： 
参    数：u8 byIdx 表示第几个BAS外设 
		  u8 byChnId 表示通道ID
		  const TFrameHeader& tFrameInfo 帧数据
返 回 值：
=============================================================================*/
u16 CMpu2BasData::SetFrameData(u8 byBasIdx,u8 byChnId, const TFrameHeader& tFrameInfo)
{
	if ( byBasIdx >=MAX_BAS_NUM )
	{
		return 0XFFFF;
	}
	
	return m_cAptGrp[byBasIdx].SetData(byChnId,tFrameInfo);
}

/*=============================================================================
函 数 名： SetEncFastUpdata
功    能： 设置编码关键帧
算法实现： 
全局变量： 
参    数：u8 byIdx 表示第几个BAS外设 
		  u8 byChnId 表示通道ID
返 回 值：
=============================================================================*/
u16 CMpu2BasData::SetEncFastUpdata(u8 byBasIdx,u8 byChnId)
{
	if ( byBasIdx >=MAX_BAS_NUM )
	{
		return 0XFFFF;
	}
	
	return m_cAptGrp[byBasIdx].SetFastUpdata(byChnId,0,TRUE);
}

/*=============================================================================
函 数 名： SetEncResizeMode
功    能： 设置黑边剪裁
算法实现： 
全局变量： 
参    数：u8 byIdx 表示第几个BAS外设 
		  u8 byChnId 表示通道ID
		  u32 dwMode //模式0:加黑边，1:裁边，默认0
返 回 值：
=============================================================================*/
u16 CMpu2BasData::SetEncResizeMode(u8 byBasIdx,u8 byChnId,u32 dwMode)
{
	if ( byBasIdx >=MAX_BAS_NUM )
	{
		return 0XFFFF;
	}
	
	//todo 通道ID
	return m_cAptGrp[byBasIdx].SetVidEncResizeMode(byChnId,dwMode);
}

/*=============================================================================
函 数 名： ShowBasChnStatus
功    能： 显示状态
算法实现： 
全局变量： 
参    数：u8 byIdx 表示第几个BAS外设 
		  u8 byChnId 表示通道ID
返 回 值：
=============================================================================*/
void CMpu2BasData::ShowBasChnStatus(u8 byBasIdx,u8 byChnId,BOOL32 bIsEnc/* = TRUE*/)
{
	if ( byBasIdx >=MAX_BAS_NUM )
	{
		return ;
	}
	
	m_cAptGrp[byBasIdx].ShowChnInfo(byChnId,bIsEnc);
}

/*=============================================================================
函 数 名： GetMpu2BasCfg
功    能： 根据idx获得BAS外设配置，通过引用返回
算法实现： 
全局变量： 
参    数： 
返 回 值： BOOL32  获得失败
=============================================================================*/
BOOL32 CMpu2BasData::GetMpu2BasCfg(u8 byIdx, TMpu2BasCfg &tMpu2BasCfg)
{
	if ( (m_byBasNum-1) < byIdx)
	{
		return FALSE;
	}
	
	memcpy( &tMpu2BasCfg, &(m_atMpu2BasCfg[byIdx]), sizeof(TMpu2BasCfg) );
	
	return TRUE;
}

/*=============================================================================
  函 数 名： ReadDebugValues
  功    能： 从配置文件读取码率作弊值
  算法实现： 
  全局变量： 
  参    数： void
  返 回 值： void 
=============================================================================*/
void CMpu2BasData::ReadDebugValues()
{
    s32 nValue;
	sprintf( MPULIB_CFG_FILE, "%s/mcueqp.ini", DIR_CONFIG );
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

	// 读是否打开平滑发送
	if (!::GetRegKeyInt(MPULIB_CFG_FILE, SECTION_MPUDEBUG, "IsUseSmoothSend", 1, &nValue))
	{
		m_tDebugVal.SetEnableSmoothSend(TRUE);
	}
	else
	{
		m_tDebugVal.SetEnableSmoothSend( (nValue==0) ? FALSE : TRUE );
	}

    return;
}

/*=============================================================================
  函 数 名： GetDebugVal
  功    能： 获得作弊参数
  算法实现： 
  全局变量： 
  参    数： void
  返 回 值： void 
=============================================================================*/
TDebugVal CMpu2BasData::GetDebugVal()
{
	return m_tDebugVal;
}


/*=============================================================================
  函 数 名： FreeStatusDataA
  功    能： 清除状态参数
  算法实现： 
  全局变量： 
  参    数： void
  返 回 值： void 
=============================================================================*/
void CMpu2BasData::FreeStatusDataA( void )
{
    m_dwMcuNode = INVALID_NODE;
	for ( u8 byIdx = 0; byIdx < m_byBasNum; byIdx++)
	{
		TMpu2BasCfg tMpu2BasCfg;
		if ( GetMpu2BasCfg(byIdx, tMpu2BasCfg) )
		{
			tMpu2BasCfg.m_dwMcuIId = INVALID_INS;
		}
	}

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
void CMpu2BasData::FreeStatusDataB( void )
{
    m_dwMcuNodeB = INVALID_NODE;
	for ( u8 byIdx = 0; byIdx < m_byBasNum; byIdx++)
	{
		TMpu2BasCfg tMpu2BasCfg;
		if ( GetMpu2BasCfg(byIdx, tMpu2BasCfg) )
		{
			tMpu2BasCfg.m_dwMcuIIdB = INVALID_INS;
		}
	}
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
	TMpu2VmpRcvCB* pRcvCB = (TMpu2VmpRcvCB*)dwContext;
    if ( pFrmHdr == NULL || pRcvCB == NULL )
    {
        ErrorLog( "[mpu][error] PTFrameHeader 0x%x, dwContext 0x%x.\n", pFrmHdr, dwContext );
        return;
    }

	CHardMulPic* pHardMulPic = pRcvCB->m_pHardMulPic;
	if ( pHardMulPic == NULL )
	{
		ErrorLog( "[mpu][error] pAdpGroup 0x%x is null.\n", pHardMulPic );
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

    DetailLog( "MediaType:%u, FrameID:%u, KF:%d, W*H:%d*%d, SSRC:%u, DataSize:%u\n",
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
        ErrorLog( "[CBSendFrame] PTFrameHeader 0x%x, dwContext 0x%x\n", pFrameInfo, pContext );
        return ;
    }

    if (g_bPauseSend)
    {
        ErrorLog("[VMPCALLBACK] pause send due to debug!\n");
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

	DetailLog( "[CBSendFrame] MediaType:%u, FrameID:%u, KF:%d, W*H:%d*%d, m_dwDataSize:%u\n",
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

    if ( m_tDebugVal.IsEnableBitrateCheat() )
	{  
		::GetRegKeyInt( MPULIB_CFG_FILE, SECTION_MPUDEBUG, "DefaultTargetRatePct", 75, &nValue );
		m_tDebugVal.SetDefaultRate( (u16)nValue );
	}
	
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

		StaticLog( "\n\tEncType: \t%u \n\tRcMode: \t%u \n\tMaxKeyFrameInterval: \t%d \n\tMaxQuant: \t%u \n\tMinQuant: \t%u\n",
                               m_tVideoEncParam[byVideoIdx].m_byEncType,
							   m_tVideoEncParam[byVideoIdx].m_byRcMode,
                               m_tVideoEncParam[byVideoIdx].m_dwMaxKeyFrameInterval,
							   m_tVideoEncParam[byVideoIdx].m_byMaxQuant,
                               m_tVideoEncParam[byVideoIdx].m_byMinQuant);

		StaticLog( "\n\tBitRate: \t%u \n\tSndNetBand: \t%u \n\tFrameRate: \t%u \n\tImageQulity: \t%u \n\tVideoWidth: \t%d \n\tVideoHeight: \t%d\n\tProfileType:%d\n",
                               m_tVideoEncParam[byVideoIdx].m_wBitRate,
							   m_adwMaxSendBand[byVideoIdx],
                               m_tVideoEncParam[byVideoIdx].m_byFrameRate,
							   m_tVideoEncParam[byVideoIdx].m_byImageQulity,
                               m_tVideoEncParam[byVideoIdx].m_wVideoWidth,
							   m_tVideoEncParam[byVideoIdx].m_wVideoHeight,
							   m_tVideoEncParam[byVideoIdx].m_dwProfile);

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
void CMpu2BasData::GetDefaultParam(u8 byEnctype, TVideoEncParam& tEncparam)
{
#if 1
    s32 nValue;
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
            tEncparam.m_dwMaxKeyFrameInterval = EnVidFmt[0][2];
        }
        else
        {
            tEncparam.m_dwMaxKeyFrameInterval = nValue;
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
            tEncparam.m_dwMaxKeyFrameInterval = EnVidFmt[1][2];
        }
        else
        {
            tEncparam.m_dwMaxKeyFrameInterval = nValue;
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
            tEncparam.m_dwMaxKeyFrameInterval = EnVidFmt[2][2];
        }
        else
        {
            tEncparam.m_dwMaxKeyFrameInterval = nValue;
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
            tEncparam.m_dwMaxKeyFrameInterval = EnVidFmt[3][2];
        }
        else
        {
            tEncparam.m_dwMaxKeyFrameInterval = nValue;
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
            tEncparam.m_dwMaxKeyFrameInterval = EnVidFmt[4][2];
        }
        else
        {
            tEncparam.m_dwMaxKeyFrameInterval = nValue;
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
            tEncparam.m_dwMaxKeyFrameInterval = EnVidFmt[5][2];
        }
        else
        {
            tEncparam.m_dwMaxKeyFrameInterval = nValue;
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
            tEncparam.m_dwMaxKeyFrameInterval = EnVidFmt[0][2];
        }
        else
        {
            tEncparam.m_dwMaxKeyFrameInterval = nValue;
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
            tEncparam.m_dwMaxKeyFrameInterval = EnVidFmt[1][2];
        }
        else
        {
            tEncparam.m_dwMaxKeyFrameInterval = nValue;
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
            tEncparam.m_dwMaxKeyFrameInterval = EnVidFmt[2][2];
        }
        else
        {
            tEncparam.m_dwMaxKeyFrameInterval = nValue;
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
            tEncparam.m_dwMaxKeyFrameInterval = EnVidFmt[3][2];
        }
        else
        {
            tEncparam.m_dwMaxKeyFrameInterval = nValue;
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
            tEncparam.m_dwMaxKeyFrameInterval = EnVidFmt[4][2];
        }
        else
        {
            tEncparam.m_dwMaxKeyFrameInterval = nValue;
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
            tEncparam.m_dwMaxKeyFrameInterval = EnVidFmt[5][2];
        }
        else
        {
            tEncparam.m_dwMaxKeyFrameInterval = nValue;
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
	case VMP_STYLE_TEN_M:
        byMaxMemNum = 10;
        break;
    case VMP_STYLE_THIRTEEN:
	case VMP_STYLE_THIRTEEN_M:
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
	case VMP_STYLE_THREE_2BOTTOMRIGHT:
	case VMP_STYLE_THREE_2BOTTOM2SIDE:
	case VMP_STYLE_THREE_2BOTTOMLEFT:
	case VMP_STYLE_THREE_VERTICAL:
		byMaxMemNum = 3;
        break;
	case VMP_STYLE_FOUR_3BOTTOMMIDDLE:
		byMaxMemNum = 4;
        break;
	case VMP_STYLE_TWENTYFIVE:
		byMaxMemNum = 25;
        break;
    default:
        break;
    }
    
    return byMaxMemNum;
}


void TMpuVmpCfg::InitVmpStyleChnlRes()
{
	u8 byStyle = 0;
	u8 byLoop = 0;
     memset( m_aVMPStyleChnlRes, 0, sizeof( m_aVMPStyleChnlRes ) );

    byStyle = VMP_STYLE_ONE;        //1画面
    m_aVMPStyleChnlRes[byStyle][0] = VIDEO_FORMAT_HD1080;


    byStyle = VMP_STYLE_VTWO;       //左右2画面
    for(byLoop = 0; byLoop < 2;byLoop ++)
    {
         m_aVMPStyleChnlRes[byStyle][byLoop] = VIDEO_FORMAT_720_960x544;
    }


    byStyle = VMP_STYLE_HTWO;       //一大一小2画面
    m_aVMPStyleChnlRes[byStyle][0] = VIDEO_FORMAT_HD1080;
    m_aVMPStyleChnlRes[byStyle][1] = VIDEO_FORMAT_640x368;


    byStyle = VMP_STYLE_THREE;      //3画面
    for(byLoop = 0; byLoop < 3; byLoop ++)
    {
         m_aVMPStyleChnlRes[byStyle][byLoop] = VIDEO_FORMAT_960x544;
    }


    byStyle = VMP_STYLE_FOUR;       //4画面
    for(byLoop = 0; byLoop < 4; byLoop ++)
    {
          m_aVMPStyleChnlRes[byStyle][byLoop] = VIDEO_FORMAT_960x544;
    }


    byStyle = VMP_STYLE_SIX;        //6画面
    m_aVMPStyleChnlRes[byStyle][0] = VIDEO_FORMAT_HD720;


    for(byLoop = 1; byLoop < 6; byLoop ++)
    {
         m_aVMPStyleChnlRes[byStyle][byLoop] = VIDEO_FORMAT_640x368;
    }


    byStyle = VMP_STYLE_EIGHT;      //8画面
    //m_aVMPStyleChnlRes[byStyle][0] = VIDEO_FORMAT_1440x816;
    m_aVMPStyleChnlRes[byStyle][0] = VIDEO_FORMAT_HD720;


    for(byLoop = 1; byLoop < 8; byLoop ++)
    {
          m_aVMPStyleChnlRes[byStyle][byLoop] = VIDEO_FORMAT_480x272;
    }


    byStyle = VMP_STYLE_NINE;       //9画面
    for(byLoop = 0; byLoop < 9; byLoop ++)
    {
          m_aVMPStyleChnlRes[byStyle][byLoop] = VIDEO_FORMAT_640x368;
    }


    byStyle = VMP_STYLE_TEN;        //10画面
    m_aVMPStyleChnlRes[byStyle][0] = VIDEO_FORMAT_960x544;
    m_aVMPStyleChnlRes[byStyle][5] = VIDEO_FORMAT_960x544;


    for(byLoop = 1; byLoop < 5; byLoop ++)
    {
          m_aVMPStyleChnlRes[byStyle][byLoop] = VIDEO_FORMAT_480x272;
    }
    for(byLoop = 6; byLoop < 10; byLoop ++)
    {
          m_aVMPStyleChnlRes[byStyle][byLoop] = VIDEO_FORMAT_480x272;
    }

    byStyle = VMP_STYLE_THIRTEEN;//13画面
    m_aVMPStyleChnlRes[byStyle][0] = VIDEO_FORMAT_960x544;


    for(byLoop = 1; byLoop < 13; byLoop ++)
    {
          m_aVMPStyleChnlRes[byStyle][byLoop] = VIDEO_FORMAT_480x272;
    }


    byStyle = VMP_STYLE_SIXTEEN;//16画面
    for(byLoop = 0; byLoop < 16; byLoop ++)
    {
          m_aVMPStyleChnlRes[byStyle][byLoop] = VIDEO_FORMAT_480x272;
    }


    byStyle = VMP_STYLE_SPECFOUR;//特殊4画面
    m_aVMPStyleChnlRes[byStyle][0] = VIDEO_FORMAT_HD720;


    for(byLoop = 1; byLoop < 4; byLoop ++)
    {
          m_aVMPStyleChnlRes[byStyle][byLoop] = VIDEO_FORMAT_640x368;
    }


    byStyle = VMP_STYLE_SEVEN;      //7画面
    for(byLoop = 0; byLoop < 3; byLoop ++)
    {
          m_aVMPStyleChnlRes[byStyle][byLoop] = VIDEO_FORMAT_960x544;
    }
    for(byLoop = 3; byLoop < 7; byLoop ++)
    {
          m_aVMPStyleChnlRes[byStyle][byLoop] = VIDEO_FORMAT_480x272;
    }


    byStyle = VMP_STYLE_TWENTY; //20画面
    for(byLoop = 0; byLoop < 20; byLoop ++)
    {
          m_aVMPStyleChnlRes[byStyle][byLoop] = VIDEO_FORMAT_480x272;
    }

	byStyle = VMP_STYLE_TEN_M; //10画面，上下各4个中间两个
    for(byLoop = 0; byLoop < 2; byLoop ++)//中间两个大的
    {
		m_aVMPStyleChnlRes[byStyle][byLoop] = VIDEO_FORMAT_960x544;
		
    }
	for(byLoop = 2; byLoop < 10; byLoop ++)//下面4个
    {
		m_aVMPStyleChnlRes[byStyle][byLoop] = VIDEO_FORMAT_480x272;
		
    }

	byStyle = VMP_STYLE_THIRTEEN_M; //13画面，中间一个大
    for(byLoop = 0; byLoop < 13; byLoop ++)
    {
		if ( byLoop == 0)//中间那个
		{
			m_aVMPStyleChnlRes[byStyle][byLoop] = VIDEO_FORMAT_960x544;
			continue;
		}
		m_aVMPStyleChnlRes[byStyle][byLoop] = VIDEO_FORMAT_480x272;
    }
	byStyle = VMP_STYLE_TWENTYFIVE;
	//TODO:25风格定义，最大的成员数，mpu2用自己的宏
   for(byLoop = 0; byLoop < 25; byLoop ++)
   {
	   m_aVMPStyleChnlRes[byStyle][byLoop] = VIDEO_FORMAT_CIF;
   }
	
}
//END OF FILE
