#include "kdvtype.h"
#include "osp.h"
#include "evmcueqp.h"
#include "mcustruct.h"
#include "mcuconst.h"
#include "mcueqpsim.h"

#ifndef _HD_
#include "kdvmulpic.h"
#include "codeclib.h"
#else
#include "codecwrapperdef_hd.h"
#include "kdvmulpic_hd.h"
#endif

NULL_CLASS_DEF(CVideoDecoder);
NULL_CLASS_DEF(CVideoEncoder);

#ifndef _HD_
CHardMulPic::CHardMulPic()
{
}
CHardMulPic::~CHardMulPic()
{
	SAFE_DEL_PTR(m_pVidEncPlayer);
	SAFE_DEL_PTR(m_pVideoEncoder);
	SAFE_DEL_PTR(m_pSecondVideoEncoder);
	
	for (int nLoop = 0; nLoop < MULPIC_MAX_CHNS; nLoop++)
	{
		SAFE_DEL_PTR(m_apVideoDecoder[nLoop]);
	}
}
	
int CHardMulPic::Initialize(TMulPicParam tMulPicParam, BOOL32 bDbVid)
{
	return HARD_MULPIC_OK;
}
int CHardMulPic::Quit()
{
	return HARD_MULPIC_OK;
}

int CHardMulPic::AddChannel(u8 byChnNo, TNetAddress  tNetRcvAddr,TNetAddress tRtcpNetRcvAddr)
{
    OspPrintf(TRUE, FALSE, "[Debug09.13][CHardMulPic::AddChannel] byChnl.%d, RecvAddr: 0x%x:%d, Rtcp: 0x%x:%d\n", 
              byChnNo, 
              ntohl(tNetRcvAddr.dwIp),
              ntohs(tNetRcvAddr.wPort),
              ntohl(tRtcpNetRcvAddr.dwIp),
              ntohs(tRtcpNetRcvAddr.wPort) );
	return HARD_MULPIC_OK;
}	

int CHardMulPic::RemoveChannel(u8 byChnNo)
{
	return HARD_MULPIC_OK;
}
	
int CHardMulPic::StartMerge(TVideoEncParam tVideoEncParam[2], TNetAddress tNetSndAddr[2])
{
	return HARD_MULPIC_OK;
}
int CHardMulPic::StopMerge(void)
{
	return HARD_MULPIC_OK;
}
	
int CHardMulPic::GetStatus(TMulPicStatus &tMulPicStatus)
{
	return HARD_MULPIC_OK;
}
int CHardMulPic::GetStatis(TMulPicStatis &tMulPicStatis)
{
	return HARD_MULPIC_OK;
}
	
int CHardMulPic::SetMulPicType(u8 byNewType)
{
	return HARD_MULPIC_OK;
}
	
int CHardMulPic::ChangeBitRate(BOOL32 bSecondType, u16  wBitRate/*kbps*/)
{
	return HARD_MULPIC_OK;
}
	
int CHardMulPic::SendVidFrameToMap(u8 byChn, u32 dwStreamType, u32 dwFrameSeq, u32 dwWidth,
		u32 dwHeight, u8 *pbyData, u32 dwDataLen)
{
	return HARD_MULPIC_OK;
}
	
int CHardMulPic::InitOneDsp(TMulPicParam tMulPicParam)
{
	return HARD_MULPIC_OK;
}
int CHardMulPic::InitTwoDsp(TMulPicParam tMulPicParam)
{
	return HARD_MULPIC_OK;
}
int CHardMulPic::InitThreeDsp(TMulPicParam tMulPicParam)
{
	return HARD_MULPIC_OK;
}
int CHardMulPic::InitFourDsp(TMulPicParam tMulPicParam)
{
	return HARD_MULPIC_OK;
}
int CHardMulPic::InitFiveDsp(TMulPicParam tMulPicParam)
{
	return HARD_MULPIC_OK;
}

int CHardMulPic::StartAddIcon(u8 byChnNo, u8 *pbyBmp, u32 dwBmpLen, u32 dwXPos, u32 dwYPos, u32 dwWidth,
								 u32 dwHeight, TBackBGDColor tBackBGDColor, u32 dwClarity)
{
	return HARD_MULPIC_OK;
}

int CHardMulPic::StopAddIcon(u8 byChnNum)
{
	return HARD_MULPIC_OK;
}
u16 CHardMulPic::SetDisplayType(u8 byDisplayType)
{
	return HARD_MULPIC_OK;
}

int CHardMulPic::OnInitMapOpr(u8 byMapNo, TMulPicParam tMulPicParam, u32 dwTotalMapNo, 
	u32 dwVidEncNum, u32 dwVidDecNum, u32 dwStartChn, u32 dwEndChn, BOOL32 bDbVid)
{
	return HARD_MULPIC_OK;
}
	
u32 CHardMulPic::GetPicMergeChn(u8  byPicMergeType)
{
	return 1;
}
u8  CHardMulPic::GetRealChnNo(u8  byChnNo)
{
	return 1;
}
u8  CHardMulPic::Get3MapRealChnNum(u8  byChnNo)
{
	return 1;
}
u8  CHardMulPic::Get4MapRealChnNum(u8  byChnNo)
{
	return 1;
}
u8  CHardMulPic::Get5MapRealChnNum(u8  byChnNo)
{
	return 1;
}
u16 CHardMulPic::SetFastUpdata(void)
{
    return 1;
}
u16 CHardMulPic::SetBackBoardOut(BOOL32 bOut)
{
    return 1;
}

/*获取通道接收统计信息*/
int CHardMulPic::GetVidRecvStatis(u8 byChn, TKdvDecStatis &tKdvDecStatis)
{
	return 1;
}

/*设置图像的网络接收重传参数*/
u16  CHardMulPic::SetNetRecvFeedbackVideoParam(TNetRSParam tNetRSParam, BOOL32 bRepeatSnd,u8 byTotalNum)
{
	return 1;
}
/*设置图像的网络发送重传参数*/
u16  CHardMulPic::SetNetSendFeedbackVideoParam(u16 wBufTimeSpan, BOOL32 bRepeatSnd)
{
	return 1;
}

//设置视频编码载荷
	u16  CHardMulPic::SetVidEncryptPT(u8 byChnNo, u8 byEncryptPT)
{
	return 1;
} 

//设置视频编码加密key字串、以及 解密模式 Aes or Des
u16  CHardMulPic::SetVidEncryptKey(u8 byChnNo, s8 *pszKeyBuf, u16 wKeySize, u8 byEncryptMode )
{
    return 1;
}  

//设置 H.263+/H.264 等动态视频载荷的 Playload值
u16  CHardMulPic::SetVideoActivePT(u8 byChnNo, u8 byRmtActivePT, u8 byRealPT )
{
    return 1;
}
//设置 视频解码key字串 以及 解密模式 Aes or Des
u16  CHardMulPic::SetVidDecryptKey(u8 byChnNo, s8 *pszKeyBuf, u16 wKeySize, u8 byDecryptMode)
{
    return 1;
}

int CHardMulPic::SetBGDAndSidelineColor(TMulPicColor tBGDColor, TMulPicColor atSlidelineColor[MULPIC_MAX_CHNS], BOOL32 bRimEnabled )
{
    return 0;
}

int CHardMulPic::SetNetSndLocalAddr(TNetAddress atNetSndLocalAddr[2])
{
    return 0;
}

API int SetMediaTOS(int nTOS, int nType)
{
    return 0;
}
API int GetMediaTOS(int nType)
{
    return 0;
}
/*
//设置视频编码加密key字串、加密的载荷PT值 以及 解密模式 Aes or Des
u16  CHardMulPic::SetVidEncryptKey( s8 *pszKeyBuf, u16 wKeySize, u8 byEncryptPT, u8 byEncryptMode )
{
	return 1;
}

//设置 H.263+/H.264 等动态视频载荷的 Playload值
u16  CHardMulPic::SetVideoActivePT( u8 byRmtActivePT, u8 byRealPT )
{
	return 1;
}
//设置 视频解码key字串 以及 解密模式 Aes or Des
u16  CHardMulPic::SetVidDecryptKey(s8 *pszKeyBuf, u16 wKeySize, u8 byDecryptMode)
{
	return 1;
}
*/

u16 CHardMulPic::SetVidFecMode(unsigned char, unsigned char)
{
    return 0;
}

u16 CHardMulPic::SetVidFecEnable(unsigned char, int)
{
    return 0;
}

#else

int CHardMulPic::SetBGDAndSidelineColor(TMulPicColor tBGDColor, TMulPicColor atSlidelineColor[MULPIC_MAX_CHNS], BOOL32 bRimEnabled )
{
    return 0;
}

int CHardMulPic::StopMerge( void )
{
    return 0;
}

int CHardMulPic::ChangeBitRate( int, unsigned short )
{
    return 0;
}

// int CHardMulPic::AddChannel( int )
// {
//     return 0;
// }

NULL_CLASS_DEF(CMulPic);

CHardMulPic::CHardMulPic( void )
{
    return;
}

CHardMulPic::~CHardMulPic( void )
{
	SAFE_DEL_PTR(m_pMulPic);
}

// int CHardMulPic::StartMerge( struct tagVideoEncParam *, int )
// {
//     return 0;
// }

// int CHardMulPic::SetMulPicType( int )
// {
//     return 0;
// }

// int HardMPUInit( unsigned long )
// {
//     return 0;
// }

// int CHardMulPic::SetFastUpdata( int )
// {
//     return 0;
// }

int CHardMulPic::GetDecStatis( int, struct tagMulPicChnStatis & )
{
    return 0;
}


int CHardMulPic::SetVidDataCallback( int, void (__cdecl*)(struct tagFrameHeader *,void *), void * )
{
    return 0;// int CHardMulPic::RemoveChannel( int )
// {
//     return 0;
// }

}

int CHardMulPic::SetData( int, struct tagFrameHeader const & )
{
    return 0;
}

int CHardMulPic::Create( struct tagMulCreate const & )
{
    return 0;
}

void RegsterCommands()
{
    return;
}


s32 CHardMulPic::GetStatus(s32 nChnNo,TMulPicStatus &tMulPicStatus)
{
    return 0;
}

u16 CHardMulPic::SetEncResizeMode(u32 dwMode)
{
	return 0;
}
#endif

//END OF FILE

