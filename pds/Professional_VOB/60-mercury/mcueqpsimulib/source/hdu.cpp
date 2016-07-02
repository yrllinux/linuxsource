#include "kdvtype.h"
#include "osp.h"
#include "kdvdef.h"
#include "codecwrapper_hd.h"
#include "kdvdef.h"
#include "mcueqpsim.h"

NULL_CLASS_DEF(CVideoDecoder);

CKdvVidDec::CKdvVidDec()
{

}
CKdvVidDec::~CKdvVidDec()
{
	SAFE_DEL_PTR(m_pCodec);
}

u16	   CKdvVidDec::CreateDecoder(TDecoder *pDecoer)
{
	return CODEC_NO_ERROR;
}

u16    CKdvVidDec::StartDec()/*开始图像解码*/
{
	return CODEC_NO_ERROR;
}
u16    CKdvVidDec::StopDec() /*停止图像解码*/
{
	return CODEC_NO_ERROR;
}

u16    CKdvVidDec::GetDecoderStatus(TKdvDecStatus &tKdvDecStatus)/*获取解码器状态*/
{
	return CODEC_NO_ERROR;
}

u16    CKdvVidDec::GetDecoderStatis(TKdvDecStatis &tKdvDecStatis)/*获取解码器的统计信息*/
{
	return CODEC_NO_ERROR;
}
	
u16    CKdvVidDec::GetCodecType(u32& dwType)  /*解码器工作具体位置 VID_CODECTYPE_FPGA VID_CODECTYPE_DSP*/
{
	return CODEC_NO_ERROR;
}
	
u16    CKdvVidDec::SetPlayScale(u16 wWidth, u16 wHeigh)
{
	return CODEC_NO_ERROR;
}

u16    CKdvVidDec::SetData(const TFrameHeader& tFrameInfo)        /*设置视频码流数据*/
{
	return CODEC_NO_ERROR;
}
	
	/*设置视频丢包处理策略*/
u16    CKdvVidDec::SetVidDropPolicy(u8 byDropPolicy)
{
	return CODEC_NO_ERROR;
}
	
u16    CKdvVidDec::SetDisplayType(u8 byDisplayType)/*设置画面显示类型(VIDEO VGA)*/
{
	return CODEC_NO_ERROR;
}    

u16    CKdvVidDec::SetGetVideoScaleCallBack(TDecodeVideoScaleInfo tDecodeVideoScaleInfo)
{
	return CODEC_NO_ERROR;
}

u16    CKdvVidDec::SetVideoDecParam(TVideoDecParam *ptVidDecParam)
{
	return CODEC_NO_ERROR;
}
    
u16    CKdvVidDec::SetVidPlyPortType(u32 dwType)
{
	return CODEC_NO_ERROR;
}

u16    CKdvVidDec::SetVideoPlyInfo(TVidSrcInfo* ptInfo)  /*设置视频输出信号制式，NULL表示自动调整。*/
{
	return CODEC_NO_ERROR;
}

// [6/29/2011 liuxu] Add
/*设置hdu工作模式，指hdu-1、hdu-2模式，hdu和hdu-d用于区分hdu启动模式*/
u32    CKdvVidDec::SetWorkMode(u32 dwWorkMode)
{
	return 0;
}

u16    CKdvVidDec::GetVideoSrcInfo(TVidSrcInfo& tInfo)   /*获取输出视频信号制式*/
{
	return CODEC_NO_ERROR;
}
    //u16    SetVidPlyDefault(BOOL32 bNtsc = FALSE); /*是否默认采用 30/60播放*/
    //u16    SetVidPlyVgaDefault(u32 dwFreq = 60); /*VGA播放频率默认60Hz，其他75,85等*/
    //u16    SetAutoCheckPlayInfo(BOOL32 bAuto);    /*是否自动调整播放帧率*/
	
u16    CKdvVidDec::ClearWindow()  /*清屏*/
{
	return CODEC_NO_ERROR;
}

u16    CKdvVidDec::FreezeVideo()  /*视频冻结*/
{
	return CODEC_NO_ERROR;
}

u16    CKdvVidDec::UnFreezeVideo()/*取消视频冻结*/
{
    return CODEC_NO_ERROR;
}	

u16    CKdvVidDec::SetVidDecResizeMode(s32 nMode)
{
	return CODEC_NO_ERROR;
}

u16		CKdvVidDec::SetNoStreamBak(unsigned long,unsigned char *,unsigned long)
{
	return CODEC_NO_ERROR;
}

NULL_CLASS_DEF(CAudioDecoder);
CKdvAudDec::CKdvAudDec()
{

}
CKdvAudDec::~CKdvAudDec()
{
	SAFE_DEL_PTR(m_pCodec);
}

u16	   CKdvAudDec::CreateDecoder(TDecoder *pDecoer)
{
	return CODEC_NO_ERROR;
}

u16    CKdvAudDec::GetDecoderStatus(TKdvDecStatus &tKdvDecStatus)/*获取解码器状态*/
{
	return CODEC_NO_ERROR;
}

u16    CKdvAudDec::GetDecoderStatis(TKdvDecStatis &tKdvDecStatis)/*获取解码器的统计信息*/
{
	return CODEC_NO_ERROR;
}	
	
u16    CKdvAudDec::SetData(const TFrameHeader& tFrameInfo) /*设置音频码流数据*/
{
	return CODEC_NO_ERROR;
}

u16    CKdvAudDec::SetAudioDecParam(const TAudioDecParam& tAudParam)/*音频解码参数，AAC_LC有效*/
{
	return CODEC_NO_ERROR;
} 
    
u16    CKdvAudDec::SetAudOutPort(u32 dwAudPort)  /*设置音频播放端口 HDMI or C*/
{
	return CODEC_NO_ERROR;
}
	
u16    CKdvAudDec::StartDec()/*开始声音解码*/
{
	return CODEC_NO_ERROR;
}	

u16    CKdvAudDec::StopDec() /*停止声音解码*/
{
	return CODEC_NO_ERROR;
}	

u16    CKdvAudDec::SetAudioVolume(u8 byVolume )  /*设置输出声音音量*/
{
	return CODEC_NO_ERROR;
}	

u16    CKdvAudDec::GetAudioVolume(u8 &pbyVolume )/*得到输出声音音量*/
{
	return CODEC_NO_ERROR;
}	

u16    CKdvAudDec::SetAudioMute(BOOL32 bMute)/*设置静音*/
{
	return CODEC_NO_ERROR;
}
	
	/*获取输出音频功率*/
u32    CKdvAudDec::GetOutputAudioPower()
{
	return CODEC_NO_ERROR;
}
	
	//播放铃声 ？？？
u16   CKdvAudDec::PlayRing(s8* pData /*= NULL*/)
{
	return CODEC_NO_ERROR;
}
