
#include "kdvtype.h"
#include "osp.h"
#include "kdvmixer.h"




CKdvMixerGroup::CKdvMixerGroup()
{
}
CKdvMixerGroup:: ~CKdvMixerGroup()
{
}
u16 CKdvMixerGroup::StartMixerGroup(u8 bAllMix)
{
	return MIXER_OPERATE_SUCCESS;
}

u16    CKdvMixerGroup::SetMixChnStatus(u8  bChnStatus[192])  //设置手工参与混音模式下，各通道是否参与混音
{
	return MIXER_OPERATE_SUCCESS;
}

	u16 CKdvMixerGroup::CreateMixerGroup(const TMixerMap* tMixerMap,u8 byChannelNum)//创建混音组
	{
		return MIXER_OPERATE_SUCCESS;
	}
	u16 CKdvMixerGroup::DestroyMixerGroup( )//删除混音组
	{
		return MIXER_OPERATE_SUCCESS;
	}
	u16 CKdvMixerGroup::StopMixerGroup()//停止混音组
	{
		return MIXER_OPERATE_SUCCESS;
	}
	u16 CKdvMixerGroup::AddMixerChannel(const TMixerChannel* tMixerChanel)//增加组的混音通道
	{
		return MIXER_OPERATE_SUCCESS;
	}
	u16 CKdvMixerGroup::DeleteMixerChannel(u8 byChannelIndex)//删除组的混音通道
	{
		return MIXER_OPERATE_SUCCESS;
	}
    
	u16 CKdvMixerGroup::SetNModeOutput(const TNetAddress* tLocalAddr, const TNetAddress* tDstNetAddr)//开始N模式的输出
	{
		return MIXER_OPERATE_SUCCESS;
	}

    u16 CKdvMixerGroup::SetNModeNetFeedbackParam(BOOL32 bRepeatSnd, u16 wBufTimeSpan)
    {
        return MIXER_OPERATE_SUCCESS;
    }
    u16 CKdvMixerGroup::SetMixerDelay(u32 dwTime)
    {
        return CODEC_NO_ERROR;
    }
	u16 CKdvMixerGroup::SetMixerNetFeedbackParam(u8 byChnIndex,  BOOL32 bFeedBackSnd, TNetRSParam tNetRSParam, BOOL32 bRepeatSnd, u16 wBufTimeSpan)
    {
        return MIXER_OPERATE_SUCCESS;
    }

    u16 CKdvMixerGroup::SetForceAudMix(u8 byChnNo) // 强制混音
	{
		return MIXER_OPERATE_SUCCESS;
	}
    u16 CKdvMixerGroup::CancelForceAudMix() // 取消强制
	{
		return MIXER_OPERATE_SUCCESS;
	}
    u16 CKdvMixerGroup::SetMixerDepth(u8 byDepth)//设置混音深度
	{
		return MIXER_OPERATE_SUCCESS;
	}
    u16 CKdvMixerGroup::SetMixerCallBack(TMixerCallBack tMixerCallBack, int dwContext)//语音激励回调
	{
		return MIXER_OPERATE_SUCCESS;
	}
    u16 CKdvMixerGroup::GetActiveChns(u8 abyActiveChnsArr[], u8 byArrNum, u8 *pbyActiveNum) //获得当前参与混音的通道
	{
		return MIXER_OPERATE_SUCCESS;
	}
	
	u16 CKdvMixerGroup::SetMixerChannelVolume(u8 byChannelIndex,u8 byVolume)//设置通道音量
	{
		return MIXER_OPERATE_SUCCESS;
	}
	u16 CKdvMixerGroup::GetMixerGroupStatus(TMixerGroupStatus &tMixerGroupStatus)//得到组的状态
	{
		return MIXER_OPERATE_SUCCESS;
	}
	u16 CKdvMixerGroup::GetMixerChannelStatis(u8 byChannelIndex,TMixerChannelStatis &tMixerChannelStatis)//得到通道的统计信息
	{
		return MIXER_OPERATE_SUCCESS;
	}
	
	u16 CKdvMixerGroup::SetMixerParam(TMixerParam  *ptMixPara)//设置智能混音器参数
	{
		return MIXER_OPERATE_SUCCESS;
	}

	u16 CKdvMixerGroup::SetMixerRcvBufs(u32 dwStartRcvBufs, u32 dwFastRcvBufs)
	{
		return MIXER_OPERATE_SUCCESS;
	}
	
	u16 CKdvMixerGroup::StopMixerSnd(BOOL32 bStopSnd)	//TRUE: 停止所有发送, FALSE: 开始所有发送
	{
		return MIXER_OPERATE_SUCCESS;
	}
	u16 CKdvMixerGroup::SetExciteKeepTime(u32 dwKeepTime)  //设置语音激励的延时保护, 单位：秒
	{
		return MIXER_OPERATE_SUCCESS;
	}
	
	void CKdvMixerGroup::MixerCallBackProc(u8 *pbyMediaMsg, u32 dwMediaMsgLen)
	{
	}
	
	u16 CKdvMixerGroup::SendMsgToMap(u32 dwCciQueNo, u8 *pbyMsgHead, u32 dwMsgHeadLen, u8 *pbyMsgBody,
		u32 dwMsgBodyLen)		//测试用
	{
		return MIXER_OPERATE_SUCCESS;
	}
	
	u16 CKdvMixerGroup::H2MCmdSend(u32 type, u32 event, u32 channel, u8 *msg, u32 msgLen) //测试用
	{
		return MIXER_OPERATE_SUCCESS;
	}
    //设置音频编码载荷PT值
	u16  CKdvMixerGroup::SetAudEncryptPT(u8 byChnNo, u8 byEncryptPT)
	{
		return MIXER_OPERATE_SUCCESS;
	}
	//设置音频编码加密key字串以及 解密模式 Aes or Des
	u16  CKdvMixerGroup::SetAudEncryptKey(u8 byChnNo, s8 *pszKeyBuf, u16 wKeySize, u8 byEncryptMode )
	{
		return MIXER_OPERATE_SUCCESS;
	}
	
	
	//设置 动态音频载荷的 Playload值
	u16  CKdvMixerGroup::SetAudioActivePT(u8 byChnNo, u8 byRmtActivePT, u8 byRealPT )
	{
		return MIXER_OPERATE_SUCCESS;
	}
	//设置 音频解码key字串 以及 解密模式 Aes or Des
    u16  CKdvMixerGroup::SetAudDecryptKey(u8 byChnNo, s8 *pszKeyBuf, u16 wKeySize, u8 byDecryptMode)
	{
		return MIXER_OPERATE_SUCCESS;
	}
/*
	//设置音频编码加密key字串、加密的载荷PT值 以及 解密模式 Aes or Des
	u16  CKdvMixerGroup::SetAudEncryptKey( s8 *pszKeyBuf, u16 wKeySize, u8 byEncryptPT, u8 byEncryptMode )
	{
		return MIXER_OPERATE_SUCCESS;
	}
	//设置 动态音频载荷的 Playload值
	u16  CKdvMixerGroup::SetAudioActivePT( u8 byRmtActivePT, u8 byRealPT )
	{
		return MIXER_OPERATE_SUCCESS;
	}
	//设置 音频解码key字串 以及 解密模式 Aes or Des
    u16  CKdvMixerGroup::SetAudDecryptKey(s8 *pszKeyBuf, u16 wKeySize, u8 byDecryptMode)
	{
		return MIXER_OPERATE_SUCCESS;
	}
*/

    u16 CKdvMixerGroup::SetNetAudioResend(u8 byChnNo, s32 nSendTimes, s32 nSendSpan)
    {
        return MIXER_OPERATE_SUCCESS;         
    }