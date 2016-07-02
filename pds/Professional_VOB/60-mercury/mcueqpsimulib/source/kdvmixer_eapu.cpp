#include "kdvmixer_eapu.h"
#include "mcueqpsim.h"


//NULL_CLASS_DEF(CKdvMixer);
class CAudioMixer
{
	
};

CKdvMixer::CKdvMixer()
{

}

CKdvMixer::~CKdvMixer()
{
	if (m_pcMixer)
	{
		delete m_pcMixer;
		m_pcMixer = NULL;
	}
}
u16 CKdvMixer::Create(const TKdvMixerInitParam& tInitParam)
{
	printf("µ×²ã´´½¨»ìÒôÆ÷\n");
	return 0;
}
u16 CKdvMixer::Destroy()
{
	return 0;
}
u16 CKdvMixer::ReCreate(const TKdvMixerInitParam& tInitParam)
{
	return 0;
}
u16 CKdvMixer::StartAudMix()
{
	return 0;
}
u16 CKdvMixer::StopAudMix()
{
	return 0;
}
u16 CKdvMixer::StartVolActive()
{
	return 0;
}
u16 CKdvMixer::StopVolActive()
{
	return 0;
}
u16 CKdvMixer::AddChannel(u32 dwChID)
{
	return 0;
}
u16 CKdvMixer::DelChannel(u32 dwChID)
{
	return 0;
}
u16 CKdvMixer::SetAudEncParam(const TKdvMixerType& tType)
{
	return 0;
}
u16 CKdvMixer::SetDModeChannel(const TKdvMixerType& tType)
{
	return 0;
}
u16 CKdvMixer::DelDModeChannel(u32 dwDModeChID)
{
	return 0;
}
u16 CKdvMixer::SetAudDecData(u32 dwChID, const TFrameHeader& tFrameInfo)
{
	return 0;
}
u16 CKdvMixer::SetAudEncDataCallback(MIXENCFRAMECALLBACK fAudData, void* pContext)
{
	return 0;
}
u16 CKdvMixer::SetVolActiveCallback(VOLACTIVECALLBACK fVolAct, void* pContext)
{
	return 0;
}
u16 CKdvMixer::SetForceChannel(u32 dwChID)
{
	return 0;
}
u16 CKdvMixer::SetMixDepth(u8 byDepth)
{
	return 0;
}
u16 CKdvMixer::SetVolActKeepTime(u32 dwKeepTime)
{
	return 0;
}
u16 CKdvMixer::SetMixerChVol(BOOL32 bDMode, u32 dwChID, u8 byVolume)
{
	return 0;
}
u16 CKdvMixer::GetMixerChVol(BOOL32 bDMode, u32 dwChID, u8& byVolume)
{
	return 0;
}
u16 CKdvMixer::GetMixerChStatus(BOOL32 bDMode, u32 dwChID, TKdvMixerChStatus& tStatus)
{
	return 0;
}
u16 CKdvMixer::GetMixerChStatis(BOOL32 bDMode, u32 dwChID, TKdvMixerChStatis& tStatis)
{
	return 0;
}
u16 CKdvMixer::SetMixerDelay(u32 dwTime)
{
	return 0;
}
u16 CKdvMixer::SetMixerRcvBufs(u32 dwStartRcvBufs, u32 dwFastRcvBufs)
{
	return 0;
}
u16 CKdvMixer::GetActiveChID(u32*pdwChId, u32& dwChNum)
{
	return 0;
}