#include "kdvtype.h"
#include "osp.h"
#include "mcueqpsim.h"
#ifndef _HD_
#include "kdvadapter.h"
#else 
#include "kdvadapter_hd.h"
#endif

#ifndef _HD_
	NULL_CLASS_DEF(CVideoEncoder);
	NULL_CLASS_DEF(CVideoDecoder);
	NULL_CLASS_DEF(CAudioEncoder);
	NULL_CLASS_DEF(CAudioDecoder);
#else
	NULL_CLASS_DEF(CAdapterChannel);
#endif

CKdvAdapterGroup::CKdvAdapterGroup ()
{
}

CKdvAdapterGroup::~ CKdvAdapterGroup ()
{
#ifndef _HD_

#define DEL_TABLE_PTR(pp, v) do { for (int nLoop = 0; pp && nLoop < v; nLoop++) { SAFE_DEL_PTR(pp[nLoop]); } }while(0)


	DEL_TABLE_PTR(m_apVideoEncoder, MAX_VIDEO_ADAPT_CHANNUM);
	DEL_TABLE_PTR(m_apVideoDecoder, MAX_VIDEO_ADAPT_CHANNUM);
	DEL_TABLE_PTR(m_apAudioEncoder, MAX_AUDIO_ADAPT_CHANNUM);
	DEL_TABLE_PTR(m_apAudioDecoder, MAX_AUDIO_ADAPT_CHANNUM);
	
	DEL_TABLE_PTR(m_apTransVideoEncoder, MAX_TRANS_VIDEO_ADAPT_CHANNUM);
	DEL_TABLE_PTR(m_apTransVideoDecoder, MAX_TRANS_VIDEO_ADAPT_CHANNUM);
	DEL_TABLE_PTR(m_apTransAudioEncoder, MAX_TRANS_AUDIO_ADAPT_CHANNUM);
	DEL_TABLE_PTR(m_apTransAudioDecoder, MAX_TRANS_AUDIO_ADAPT_CHANNUM);

#else
	SAFE_DEL_PTR(m_pCodec);
#endif
}

#ifndef _HD_
	u16 CKdvAdapterGroup::CreateGroup(const TAdapterMap *ptAdapterMap, u8 byVideoChnNum, u8 byAudioChnNum)// 创建适配组
	{
		return CODEC_NO_ERROR;
	}
	u16 CKdvAdapterGroup::StartGroup(void)//开始适配
	{
		return CODEC_NO_ERROR;
	}
	
   	u16 CKdvAdapterGroup::StopGroup(void)//停止适配	
	{
		return CODEC_NO_ERROR;
	}
	
    u16 CKdvAdapterGroup::DestroyGroup(void)//退出组
	{
		return CODEC_NO_ERROR;
	}
	
	
	u16 CKdvAdapterGroup::AddVideoChannel(const TAdapterChannel * ptAdpChannel) //添加图象通道
	{
		return CODEC_NO_ERROR;
	}
	
	u16 CKdvAdapterGroup::RemoveVideoChannel(u8 byChnNo) //删除图象通道
	{
		return CODEC_NO_ERROR;
	}
	
	u16 CKdvAdapterGroup::AddAudioChannel(const TAdapterChannel * ptAdpChannel) //添加声音通道
	{
		return CODEC_NO_ERROR;
	}
	
	u16 CKdvAdapterGroup::RemoveAudioChannel(u8 byChnNo) //删除声音通道
	{
		return CODEC_NO_ERROR;
	}
	
    u16 CKdvAdapterGroup::GetGroupStatus(TAdapterGroupStatus &tAdapterGroupStatus) //得到组的状态
	{
		return CODEC_NO_ERROR;
	}
	
	u16 CKdvAdapterGroup::GetAudioChannelStatis(u8 byChnNo, TAdapterChannelStatis &tAdapterChannelStatis) //得到音频通道的统计信息
	{
		return CODEC_NO_ERROR;
	}
	
	u16 CKdvAdapterGroup::GetVideoChannelStatis(u8 byChnNo, TAdapterChannelStatis &tAdapterChannelStatis) //得到视频通道的统计信息
	{
		return CODEC_NO_ERROR;
	}
	
	u16 CKdvAdapterGroup::ChangeVideoEncParam(u8 byChnNo, TVideoEncParam  * ptVidEncParam)     //改变视频适配编码参数
	{
		return CODEC_NO_ERROR;
	}
	
	u16 CKdvAdapterGroup::ChangeAudioEncParam(u8 byChnNo, u8 byMediaType, TAdapterAudioEncParam * ptAudEncParam)     //改变音频适配编码参数 
	{
		return CODEC_NO_ERROR;
	}
		
	u16 CKdvAdapterGroup::ChangeVideoDecParam(u8 byChnNo, TVideoDecParam  * ptVidDecParam)     //改变视频适配解码参数
	{
		return CODEC_NO_ERROR;
	}
	
	void  CKdvAdapterGroup::ShowChnInfo(u32 chnNo, BOOL32 bVidChn)
	{
	}

	/*设置图像的网络接收重传参数*/
	u16  CKdvAdapterGroup::SetNetRecvFeedbackVideoParam(TNetRSParam tNetRSParam, BOOL32 bRepeatSnd)
	{
			return CODEC_NO_ERROR;
	}
	/*设置图像的网络发送重传参数*/
	u16 CKdvAdapterGroup::SetNetSendFeedbackVideoParam(u16 wBufTimeSpan, BOOL32 bRepeatSnd )
	{
		return CODEC_NO_ERROR;
	}

    //设置音频编码载荷PT值
	u16  CKdvAdapterGroup::SetAudEncryptPT(u32 dwChnNo, u8 byEncryptPT)
	{
		return CODEC_NO_ERROR;
	}
	//设置音频编码加密key字串以及 解密模式 Aes or Des
	u16  CKdvAdapterGroup::SetAudEncryptKey(u32 dwChnNo, s8 *pszKeyBuf, u16 wKeySize, u8 byEncryptMode )
	{
		return CODEC_NO_ERROR;
	}

	//设置音频编码载荷PT值
	u16  CKdvAdapterGroup::SetVidEncryptPT(u32 dwChnNo, u8 byEncryptPT)
	{
		return CODEC_NO_ERROR;
	}
	//设置视频编码加密key字串以及 解密模式 Aes or Des
	u16  CKdvAdapterGroup::SetVidEncryptKey(u32 dwChnNo, s8 *pszKeyBuf, u16 wKeySize, u8 byEncryptMode )
	{
		return CODEC_NO_ERROR;
	} 
	
	//设置 H.263+/H.264 等动态视频载荷的 Playload值
	u16  CKdvAdapterGroup::SetVideoActivePT(u32 dwChnNo, u8 byRmtActivePT, u8 byRealPT )
	{
		return CODEC_NO_ERROR;
	}
	//设置 视频解码key字串 以及 解密模式 Aes or Des
    u16  CKdvAdapterGroup::SetVidDecryptKey(u32 dwChnNo, s8 *pszKeyBuf, u16 wKeySize, u8 byDecryptMode)
	{
		return CODEC_NO_ERROR;
	}
	
	//设置 动态音频载荷的 Playload值
	u16  CKdvAdapterGroup::SetAudioActivePT(u32 dwChnNo, u8 byRmtActivePT, u8 byRealPT )
	{
		return CODEC_NO_ERROR;
	}
	//设置 音频解码key字串 以及 解密模式 Aes or Des
    u16  CKdvAdapterGroup::SetAudDecryptKey(u32 dwChnNo, s8 *pszKeyBuf, u16 wKeySize, u8 byDecryptMode)
	{
		return CODEC_NO_ERROR;
	}
    u16 CKdvAdapterGroup::SetFastUpdata(void)
    {
        return CODEC_NO_ERROR;
    }
    


#ifndef TOSFUNC
#define TOSFUNC
//设置媒体TOS值 nType类型0:全部 1:音频 2:视频 3: 数据
API int SetMediaTOS(int nTOS, int nType);
API int GetMediaTOS(int nType);
#endif

/*
	//设置音频编码加密key字串、加密的载荷PT值 以及 解密模式 Aes or Des
	u16  CKdvAdapterGroup::SetAudEncryptKey( s8 *pszKeyBuf, u16 wKeySize, u8 byEncryptMode )
	{
		return CODEC_NO_ERROR;
	}
	//设置视频编码加密key字串、加密的载荷PT值 以及 解密模式 Aes or Des
	u16  CKdvAdapterGroup::SetVidEncryptKey( s8 *pszKeyBuf, u16 wKeySize, u8 byEncryptMode )
	{
		return CODEC_NO_ERROR;
	}
	
	//设置 H.263+/H.264 等动态视频载荷的 Playload值
	u16  CKdvAdapterGroup::SetVideoActivePT( u8 byRmtActivePT, u8 byRealPT )
	{
		return CODEC_NO_ERROR;
	}
	//设置 视频解码key字串 以及 解密模式 Aes or Des
    u16  CKdvAdapterGroup::SetVidDecryptKey(s8 *pszKeyBuf, u16 wKeySize, u8 byDecryptMode)
	{
		return CODEC_NO_ERROR;
	}
	
	//设置 动态音频载荷的 Playload值
	u16  CKdvAdapterGroup::SetAudioActivePT( u8 byRmtActivePT, u8 byRealPT )
	{
		return CODEC_NO_ERROR;
	}
	//设置 音频解码key字串 以及 解密模式 Aes or Des
    u16  CKdvAdapterGroup::SetAudDecryptKey(s8 *pszKeyBuf, u16 wKeySize, u8 byDecryptMode)
	{
		return CODEC_NO_ERROR;
	}

*/

u16 CKdvAdapterGroup::SetVidFecMode(unsigned long,unsigned char)
{
    return 0;
}

u16 CKdvAdapterGroup::SetVidFecEnable(unsigned long,int)
{
    return 0;
}

#else

u16 CKdvAdapterGroup::CreateGroup(u32 bIsVgaGroup)// 创建适配组
{
    return 0;
}

u16 CKdvAdapterGroup::StartGroup()
{
    return 0;
}

u16 CKdvAdapterGroup::StopGroup()
{
    return 0;
}

u16 CKdvAdapterGroup::DestroyGroup()
{
    return 0;
}

u16 CKdvAdapterGroup::AddVideoChannel(const TAdapterChannel * ptAdpChannel, u8& byChnNo)
{
    return 0;
}

u16 CKdvAdapterGroup::RemoveVideoChannel(u8 byChnNo)
{
    return 0;
}

u16 CKdvAdapterGroup::GetGroupStatus(TAdapterGroupStatus &tAdapterGroupStatus)
{
    return 0;
}

u16 CKdvAdapterGroup::GetVideoChannelStatis(u8 byChnNo, TAdapterChannelStatis &tAdapterChannelStatis)
{
    return 0;
}

u16 CKdvAdapterGroup::GetVideoCodecType(u8 byChnNo, u32 dwID, u32& dwType)
{
    return 0;
}

u16 CKdvAdapterGroup::ChangeVideoEncParam(u8 byChnNo, TVideoEncParam  * ptVidEncParam, u32 dwEncNum)
{
    return 0;
}

u16 CKdvAdapterGroup::ChangeVideoDecParam(u8 byChnNo, TVideoDecParam  * ptVidDecParam)
{
    return 0;
}

u16 CKdvAdapterGroup::SetVidDataCallback(u8 byChnNo, u32 dwID, FRAMECALLBACK fVidData, void* pContext) /*设置视频数据回调，编码器*/
{
    return 0;
}

u16 CKdvAdapterGroup::SetData(u32 chnNo, const TFrameHeader& tFrameInfo)        /*设置视频码流数据，解码器*/
{
    return 0;
}

void CKdvAdapterGroup::ShowChnInfo(u32 chnNo, BOOL32 bVidChn)
{
    return;
}

u16 CKdvAdapterGroup::SetFastUpdata(u32 dwID)
{
    return 0;
}

u16 CKdvAdapterGroup::ClearVideo()
{
    return CODEC_NO_ERROR;
}

u16 CKdvAdapterGroup::SetResizeMode(u32 dwMode)
{
	return 0;
}
#endif //_HD_
