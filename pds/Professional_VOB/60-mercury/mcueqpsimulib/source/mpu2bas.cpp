#include "kdvtype.h"
#include "osp.h"
#include "mcueqpsim.h"
#include "./dm816x/kdvadapter_hd.h"

CKdvVideoAdapter::CKdvVideoAdapter ()
{
}

CKdvVideoAdapter::~ CKdvVideoAdapter ()
{

}

u16 CKdvVideoAdapter::Create(const TVidAdapterCreate &tInit)// 创建适配组
{
    return CODEC_NO_ERROR;
}

u16 CKdvVideoAdapter::Destroy()
{
	return CODEC_NO_ERROR;
}

u16 CKdvVideoAdapter::Start(u32 dwAdapterChnlId)
{
    return CODEC_NO_ERROR;
}

u16 CKdvVideoAdapter::Stop(u32 dwAdapterChnlId)
{
    return CODEC_NO_ERROR;
}

u16 CKdvVideoAdapter::AddVideoChannel(const TAdapterChannel *ptAdpChannel)
{
    return CODEC_NO_ERROR;
}

u16 CKdvVideoAdapter::AddVideoEncParam(u32 dwAdapterChnlId, TVideoEncParam* ptVidEncParam, u32 dwEncId)
{
    return CODEC_NO_ERROR;
}

u16 CKdvVideoAdapter::RemoveVideoChannel(u32 dwAdapterChnlId)
{
    return CODEC_NO_ERROR;
}

u16 CKdvVideoAdapter::GetVidDecStatus(u32 dwAdapterChnlId, TKdvDecStatus &tVidDecStatus)
{
    return CODEC_NO_ERROR;
}

u16 CKdvVideoAdapter::GetVidDecStatis(u32 dwAdapterChnlId, TKdvDecStatis &tVidDecStatis)
{
    return CODEC_NO_ERROR;
}

u16 CKdvVideoAdapter::GetVidEncStatus(u32 dwAdapterChnlId, u32 dwEncoderId,TKdvEncStatus &tVidDecStatus)
{
    return CODEC_NO_ERROR;
}

u16 CKdvVideoAdapter::GetVidEncStatis(u32 dwAdapterChnlId, u32 dwEncoderId, TKdvEncStatis &tVidEncStatis)
{
    return CODEC_NO_ERROR;
}

u16 CKdvVideoAdapter::ChangeVideoEncParam(u32 dwAdapterChnlId, TVideoEncParam* ptVidEncParam, u32 dwEncNum /*= 1*/)
{
    return CODEC_NO_ERROR;
}

u16 CKdvVideoAdapter::ChangeVideoDecParam(u32 dwAdapterChnlId, TVideoDecParam* ptVidDecParam)
{
    return CODEC_NO_ERROR;
}

u16 CKdvVideoAdapter::SetVidDataCallback(u32 dwAdapterChnlId, u32 dwEncoderId, FRAMECALLBACK fVidData, void* pContext) /*设置视频数据回调，编码器*/
{
    return CODEC_NO_ERROR;
}

u16 CKdvVideoAdapter::SetData(u32 dwAdapterChnlId, const TFrameHeader& tFrameInfo)        /*设置视频码流数据，解码器*/
{
    return CODEC_NO_ERROR;
}

void CKdvVideoAdapter::ShowChnInfo(u32 dwAdapterChnlId, BOOL32 bEnc)
{
    return;
}

u16 CKdvVideoAdapter::SetFastUpdata(u32 dwAdapterChnlId,u32 dwEncoderId,BOOL32 bAllUpdate/*=FALSE*/)
{
    return CODEC_NO_ERROR;
}

u16 CKdvVideoAdapter::ClearVideo()
{
    return CODEC_NO_ERROR;
}

u16 CKdvVideoAdapter::SetVidDecResizeMode(u32 dwAdapterChnlId, u32 dwMode)
{
	 return CODEC_NO_ERROR;
}
u16 CKdvVideoAdapter::SetVidEncResizeMode(u32 dwAdapterChnlId, u32 dwMode)
{
	 return CODEC_NO_ERROR;
}