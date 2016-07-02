#include "kdvmixer_apu2.h"
#include "mcueqpsim.h"


/*ªÏ“Ù∆˜*/
class CAudioMixProc
{
	
};
u16 CKdvMixer::StartSpeechActive()
{
	return 0;
}
u16 CKdvMixer::StopSpeechActive()
{
	return 0;
}
u16 CKdvMixer::SetAudEncParam(const TKdvMixerChnlInfo& tChnlInfo, const TKdvMixerTypeParam& tEncParam)
{
	return 0;
}
u16 CKdvMixer::SetAudDecData(u32 dwChID, const PFRAMEHDR pFrmHdr)
{
	return 0;
}
u16 CKdvMixer::SetAudEncDataCallBack(MIXENCFRAMECALLBACK fAudEncCB, void* pContext)
{
	return 0;
}
u16 CKdvMixer::SetSpeechActiveCallBack(SPEECHACTIVECALLBACK fSpeechActiveCB, void* pContext)
{
	return 0;
}
u16 CKdvMixer::SetSpeechActiveKeepTime(u32 dwKeepTime)
{
	return 0;
}
u16 CKdvMixer::SetMixerDecChnlVol(u32 dwChID, u8 byVolume)
{
	return 0;
}
u16 CKdvMixer::GetMixerDecChnlVol(u32 dwChID, u8& byVolume)
{
	return 0;
}
u16 CKdvMixer::GetMixerDecChnlStatus(const TKdvMixerChnlInfo& tDecChnlInfo, TKdvMixerChStatus& tDecChnlStatus)
{
	return 0;
}
u16 CKdvMixer::GetMixerEncChnlStatus(const TKdvMixerChnlInfo& tEncChnlInfo, TKdvMixerChStatus& tEncChnlStatus)
{
	return 0;
}
u16 CKdvMixer::GetMixerDecChStatis(const TKdvMixerChnlInfo& tDecChnlInfo, TKdvDecStatis& tDecChnlStatis)
{
	return 0;
}
u16 CKdvMixer::GetMixerEncChStatis(const TKdvMixerChnlInfo& tEncChnlInfo, TKdvEncStatis& tEncChnlStatis)
{
	return 0;
}
u16 CKdvMixer::SetAudioDecParam(u32 dwChID, const TAudioDecParam& tAudParam)
{
	return 0;
}
u16 CKdvMixer::SetMixerRcvBufs(u32 dwFastRcvBufs)
{
	return 0;
}
u16 CKdvMixer::GetMixerChID(u32* pdwChId, u32& dwChNum)
{
	return 0;
}


/*  ≈‰∆˜*/
class CAudioAdaptProc
{

};
CKdvAudAdaptor::CKdvAudAdaptor()
{

}
CKdvAudAdaptor::~CKdvAudAdaptor()
{
	if (m_pcAdaptorProc)
	{
		delete m_pcAdaptorProc;
		m_pcAdaptorProc = NULL;
	}
}
u16 CKdvAudAdaptor::Create(u32 dwAudVersion)
{
	return 0;
}
u16 CKdvAudAdaptor::Destroy()
{
	return 0;
}
u16 CKdvAudAdaptor::StartAdaptor(BOOL32 bStart)
{
	return 0;
}
u16 CKdvAudAdaptor::AddChannel(u32 dwChID)
{
	return 0;
}
u16 CKdvAudAdaptor::DelChannel(u32 dwChID)
{
	return 0;
}
u16 CKdvAudAdaptor::SetAudEncParam(const u32 dwChID, const u32 dwEncNum, const TKdvAudioAdaptorTypeParam* ptEncParam)
{
	return 0;
}
u16 CKdvAudAdaptor::SetAudDecData(u32 dwChID, const PFRAMEHDR pFrmHdr)
{
	return 0;
}
u16 CKdvAudAdaptor::SetAudioDecParam(u32 dwChID, const TAudioDecParam& tAudParam)
{
	return 0;
}
u16 CKdvAudAdaptor::SetAudEncDataCallBack(AUDADAPTENCFRAMECALLBACK fAudAdaptEncCB, void* pContext)
{
	return 0;
}
//new
u16 CKdvAudAdaptor::SetMixerRcvBufs(u32 dwFastRcvBufs)
{
	return 0;
}
u16 CKdvAudAdaptor::GetAdaptDecChnlStatus(const u32 dwChID, TKdvAdaptChStatus& tDecChnlStatus)
{
	return 0;
}
u16 CKdvAudAdaptor::GetAdaptEncChnlStatus(const u32 dwChID, const u32 dwEncNum, TKdvAdaptChStatus& tEncChnlStatus)
{
	return 0;
}	
u16 CKdvAudAdaptor::GetAdaptDecChStatis(const u32 dwChID, TKdvDecStatis& tDecChnlStatis)
{
	return 0;
}
u16 CKdvAudAdaptor::GetAdaptEncChStatis(const u32 dwChID, const u32 dwEncNum, TKdvEncStatis& tEncChnlStatis)
{
	return 0;
}