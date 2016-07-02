/* CodecLib.h - Codec API Header */ 

/* Copyright(C) 2001-2002 KDC, All rights reserved. 
说明:CODEC上层API封装函数
作者:胡昌威 魏治兵
*/
#include "kdvtype.h"
#include "osp.h"
#include "codeclib.h"
#include "mcueqpsim.h"
//模块版本信息和编译时间 及 依赖的库的版本和编译时间
void kdvencoderver()
{
}
void kdvdecoderver()
{
}

//模块帮助信息 及 依赖的库的帮助信息
void kdvencoderhelp()
{
}
void kdvdecoderhelp()
{
}

#ifdef SOFT_CODEC
class CSoftEncoder{};
class TEncoderRef{};
class TEncoderStatus{};
#else
class CVideoEncoder{};
class CAudioEncoder{};
#endif

CKdvEncoder::CKdvEncoder()
{
}
CKdvEncoder::~CKdvEncoder()
{

#ifdef SOFT_CODEC
    SAFE_DEL_PTR(m_pcKdvInterEnc);
    SAFE_DEL_PTR(m_ptRef);
    SAFE_DEL_PTR(m_ptStatus);	
#else
    SAFE_DEL_PTR(pVideoEncoder);
    SAFE_DEL_PTR(pAudioEncoder);
#endif
}

	
#ifdef SOFT_CODEC      
    //初始化软件编码器
u16 CKdvEncoder::Create( HWND previewHwnd, s32 nPriority ,u8 byBindCpuId, u8 byCapType)
{
	return CODEC_NO_ERROR;
}
u16 CKdvEncoder::SetVoiceCheck(BOOL32 bCheck,const TVoiceCheck *ptVoiceCheck)/*语音功率监测*/	
{
	return CODEC_NO_ERROR;
}
u16 CKdvEncoder::StartCap(u8 byCapType)//开始捕获
{
	return CODEC_NO_ERROR;
}
u16 CKdvEncoder::StopCap(u8 byCapType) //停止捕获
{
	return CODEC_NO_ERROR;
}
u16 CKdvEncoder::SetPreviewHwnd( HWND hPrevWnd )
{
	return CODEC_NO_ERROR;
}
u16 CKdvEncoder::SelectVideoSource(const char *szFileName)//选择视频源
{
	return CODEC_NO_ERROR;
}
u16 CKdvEncoder::SetVideoIcon( BOOL32 bEnable )//bEnable,TRUE为设置，FALSE不设。
{
	return CODEC_NO_ERROR;
}
u16 CKdvEncoder::SetVideoCapParam(const TVideoCapParam  *ptVideoCapParam )//设置图像捕获参数
{
	return CODEC_NO_ERROR;
}
u16 CKdvEncoder::SetAudioCapParam( u8 byMode )
{
	return CODEC_NO_ERROR;
}
u16 CKdvEncoder::GrabPic(CDrawWnd *pDrawWnd)//当pDrawWnd无效时，重调接口，参数为空.
{
	return CODEC_NO_ERROR;
}
u16 CKdvEncoder::SaveAsPic(LPCSTR lpszFileName, u8 byEncodeMode)
{
	return CODEC_NO_ERROR;
}
u16 CKdvEncoder::SetAudioCallBack(PAUDIOCALLBACK pAudioCallBack, u32 dwContext)
{
	return CODEC_NO_ERROR;
}

	// 设置 获取输入音频功率 的回调
u16 CKdvEncoder::SetInputAudioPowerCB(PGETAUDIOPOWER pGetInputAudioPowerCB, u32 dwContext)
{
	return CODEC_NO_ERROR;
}

#else // vxworks
u16 CKdvEncoder::CreateEncoder(const TEncoder *ptVideoEncoder, 
							const TEncoder *ptAudioEncoder)/*初始化编码器*/
{
	return CODEC_NO_ERROR;
}
	/*视频编码器加入单视频源双码流组*/
u16 CKdvEncoder::AddVidEncToSingleCapGroup(u16  wAddtoChnNum)
{
	return CODEC_NO_ERROR;
}
	
	/*发送静态图片*/
u16 CKdvEncoder::SetSendStaticPic(BOOL32  bSendStaticPic)
{
	return CODEC_NO_ERROR;
}
	/*存取快照抓拍图片*/
u16 CKdvEncoder::SetSnapshotMaxSize(u32  dwSnapshotMaxSize)
{
	return CODEC_NO_ERROR;
}
u16 CKdvEncoder::Snapshot()
{
	return CODEC_NO_ERROR;
}
u32 CKdvEncoder::GetSnapshotSaveSize()
{
	return CODEC_NO_ERROR;
}
u32 CKdvEncoder::GetSnapshotSavePicNum()
{
	return CODEC_NO_ERROR;
}

	/*获取输入音频功率*/
u32 CKdvEncoder::GetInputAudioPower()
{
	return 2;
}

u16 CKdvEncoder::ScaleVideoCap(u8 byType, u8 byValue) /* 视频采集调节 */
{
	return CODEC_NO_ERROR;
}
#endif

u16 CKdvEncoder::GetEncoderStatus(TKdvEncStatus &tKdvEncStatus)/*获取编码器状态*/ 
{
	return CODEC_NO_ERROR;
}
u16 CKdvEncoder::GetEncoderStatis(TKdvEncStatis &tKdvEncStatis )/*获取编码器的统计信息*/
{
	return CODEC_NO_ERROR;
}
//#ifndef WIN32
u16 CKdvEncoder::StartVideoCap( u8 byCapType )/*开始采集图像*/
{
	return CODEC_NO_ERROR;
}
u16 CKdvEncoder::StopVideoCap() /*停止采集图像*/
{
	return CODEC_NO_ERROR;
}
//#endif
u16 CKdvEncoder::SetVideoEncParam(const TVideoEncParam *ptVideoEncParam )/*设置视频编码参数*/
{
	return CODEC_NO_ERROR;
}
u16 CKdvEncoder::GetVideoEncParam(TVideoEncParam &tVideoEncParam )       /*得到视频编码参数*/
{
	return CODEC_NO_ERROR;
}
u16 CKdvEncoder::StartVideoEnc()/*开始压缩图像*/
{
	return CODEC_NO_ERROR;
}
u16 CKdvEncoder::StopVideoEnc() /*停止压缩图像*/
{
	return CODEC_NO_ERROR;
}
u16	CKdvEncoder::SetMpv4FrmPara(const TMpv4FrmParam *ptMpv4FrmPara)	/*设置mpv4帧参数*/ 
{
	return CODEC_NO_ERROR;
}
	
//#ifndef WIN32
u16 CKdvEncoder::StartAudioCap(BOOL32 bAinMic)/*开始语音采集*/
{
	return CODEC_NO_ERROR;
}
u16 CKdvEncoder::StopAudioCap() /*停止语音采集*/
{
	return CODEC_NO_ERROR;
}
//#endif
u16 CKdvEncoder::SetAudioEncParam(u8 byAudioEncParam, 
                            u8 byMediaType,
                            u16 wAudioDuration )/*设置语音压缩参数*/
{
	return CODEC_NO_ERROR;
}
u16 CKdvEncoder::GetAudioEncParam(u8 &byAudioEncParam, 
                            u8 *pbyMediaType,
                            u16 *pwAudioDuration )/*得到语音压缩参数*/
{
	return CODEC_NO_ERROR;
}

u16 CKdvEncoder::StartAudioEnc()/*开始压缩语音*/
{
	return CODEC_NO_ERROR;
}
u16 CKdvEncoder::StopAudioEnc() /*停止压缩语音*/
{
	return CODEC_NO_ERROR;	
}
u16 CKdvEncoder::SetAudioMute( BOOL32 bMute )     //设置是否静音
{
	return CODEC_NO_ERROR;
}
u16 CKdvEncoder::SetAudioVolume(u8 byVolume ) /*设置采集音量*/	
{
	return CODEC_NO_ERROR;
}
u16 CKdvEncoder::GetAudioVolume(u8 &byVolume )/*获取采集音量*/	
{
	return CODEC_NO_ERROR;
}
	
	/*设置图像的网络重传参数*/
u16 CKdvEncoder::SetNetFeedbackVideoParam(u16 wBufTimeSpan, BOOL32 bRepeatSnd)
{
	return CODEC_NO_ERROR;
}
u16 CKdvEncoder::SetNetSndVideoParam(const TNetSndParam *ptNetSndParam)/*设置图像的网络传送参数*/	
{
	return CODEC_NO_ERROR;
}
u16 CKdvEncoder::SetNetSndAudioParam(const TNetSndParam *ptNetSndParam)/*设置语音的网络传送参数*/
{
	return CODEC_NO_ERROR;
}
u16 CKdvEncoder::StartSendVideo(int  dwSSRC)/*开始发送图像*/	
{
	return CODEC_NO_ERROR;
}
u16 CKdvEncoder::StopSendVideo() /*停止发送图像*/
{
	return CODEC_NO_ERROR;
}
u16 CKdvEncoder::StartSendAudio(int  dwSSRC)/*开始发送语音*/	
{
	return CODEC_NO_ERROR;
}
u16 CKdvEncoder::StopSendAudio() /*停止发送语音*/	
{
	return CODEC_NO_ERROR;
}
u16	CKdvEncoder::SetAudioDumb(BOOL32 bDumb) /*设置是否哑音*/	 
{
	return CODEC_NO_ERROR;
}
	
u16 CKdvEncoder::StartAec( u8 byType )/*开始回声抵消*/
{
	return CODEC_NO_ERROR;
}
u16 CKdvEncoder::StopAec() /*停止回声抵消*/
{
	return CODEC_NO_ERROR;
}

u16	CKdvEncoder::StartAgcSmp() /*开始smp增益*/
{
	return CODEC_NO_ERROR;
}
u16	CKdvEncoder::StopAgcSmp() /*停止smp增益*/
{
	return CODEC_NO_ERROR;
}
u16	CKdvEncoder::StartAgcSin() /*开始sin增益*/
{
	return CODEC_NO_ERROR;
}
u16	CKdvEncoder::StopAgcSin() /*停止sin增益*/
{
	return CODEC_NO_ERROR;
}	
u16 CKdvEncoder::SetFastUpata(BOOL32 bIsNeedProtect)     /*设置FastUpdata，MediaCtrl在75帧内编一个I帧发送*/
{
	return CODEC_NO_ERROR;
}
#ifdef SOFT_CODEC
VOID CKdvEncoder::GetLastError( u32 &dwErrorId )
{
	return 0;
}
#endif

NULL_CLASS_DEF(CFileEncoder);
NULL_CLASS_DEF(CKdvVideoDec);
NULL_CLASS_DEF(CKdvAudioDec);
NULL_CLASS_DEF(TDecoderRef);
NULL_CLASS_DEF(TDecoderStatus);
NULL_CLASS_DEF(CVideoDecoder);
NULL_CLASS_DEF(CAudioDecoder);

CKdvDecoder::CKdvDecoder()
{
}
CKdvDecoder::~CKdvDecoder()
{
	SAFE_DEL_PTR(m_pcFileEncoder);
#ifdef SOFT_CODEC
	SAFE_DEL_PTR(m_pcKdvVideoDec);
	SAFE_DEL_PTR(m_pcKdvAudioDec);
	SAFE_DEL_PTR(m_ptRef);
	SAFE_DEL_PTR(m_ptStatus);
#else
	SAFE_DEL_PTR(pVideoDecoder);
	SAFE_DEL_PTR(pAudioDecoder);
#endif
}
	

u16  CKdvDecoder::CreateDecoder(const TVideoDecoder *ptVideoDecoder,const TAudioDecoder *ptAudioDecoder)/*初始化解码器*/	
{
	return CODEC_NO_ERROR;
}
u16 CKdvDecoder::GetDecoderStatus(TKdvDecStatus &tKdvDecStatus )/*获取解码器状态*/
{
	return CODEC_NO_ERROR;
}
u16    CKdvDecoder::GetDecoderStatis(TKdvDecStatis &tKdvDecStatis )/*获取解码器的统计信息*/	
{
	return CODEC_NO_ERROR;
}

u16    CKdvDecoder::SetVideoDecType(u8 byType) /*设置图像解码器类型, 只有在开始解码前调用才有效*/
{
	return CODEC_NO_ERROR;
}
u16    CKdvDecoder::StartVideoDec()/*开始图像解码*/
{
	return CODEC_NO_ERROR;
}
    u16    CKdvDecoder::StopVideoDec() /*停止图像解码*/
	{
		return CODEC_NO_ERROR;
	}
	u16	   CKdvDecoder::SetVideoPlayFormat(BOOL32 bFormat) /* 设置显示格式 TRUE: 16:9, FALSE: 4:3 */
	{
		return CODEC_NO_ERROR;
	}
	
	u16    CKdvDecoder::SetAudioDecType(u8 byType) /*设置声音解码器类型, 只有在开始解码前调用才有效*/
	{
		return CODEC_NO_ERROR;
	}
    u16    CKdvDecoder::StartAudioDec()/*开始声音解码*/	
	{
		return CODEC_NO_ERROR;
	}
    u16    CKdvDecoder::StopAudioDec() /*停止声音解码*/	
	{
		return CODEC_NO_ERROR;
	}
    u16    CKdvDecoder::SetAudioVolume(u8 byVolume )/*设置输出声音音量*/	
	{
		return CODEC_NO_ERROR;
	}
    u16    CKdvDecoder::GetAudioVolume(u8 &pbyVolume )/*得到输出声音音量*/
	{
		return CODEC_NO_ERROR;
	}	
    u16    CKdvDecoder::SetAudioMute(BOOL32 bMute)/*设置静音*/
	{
		return CODEC_NO_ERROR;
		
	}
	
    u16    CKdvDecoder::StartRcvVideo()/*开始网络接收图像*/
	{
		return CODEC_NO_ERROR;

	}
    u16    CKdvDecoder::StopRcvVideo() /*停止网络接收图像*/
	{
		return CODEC_NO_ERROR;

	}
    u16    CKdvDecoder::StartRcvAudio()/*开始网络接收声音*/	
	{
		return CODEC_NO_ERROR;

	}
    u16    CKdvDecoder::StopRcvAudio() /*停止网络接收声音*/	
	{
		return CODEC_NO_ERROR;
	}		
    u16    CKdvDecoder::SetVideoNetRcvParam(const TLocalNetParam *ptLocalNetParam )/*设置图像的网络接收参数*/	
	{
		return CODEC_NO_ERROR;
	}		
    u16    CKdvDecoder::SetAudioNetRcvParam(const TLocalNetParam *ptLocalNetParam )/*设置语音的网络接收参数*/
	{
		return CODEC_NO_ERROR;
		
	}
	
	/*设置图像的网络重传参数*/
	u16    CKdvDecoder::SetNetFeedbackVideoParam(TNetRSParam tNetRSParam, BOOL32 bRepeatSnd)
	{
		return CODEC_NO_ERROR;
		
	}

	/*解码器发送两路码流，一路发往Map或者win32解码绘制，一路发往网络, 参数NULL,则关闭后一路，并作相应套节子释放*/
    u16    CKdvDecoder::SetVidDecDblSend(const TNetSndParam *ptNetSndVideoParam, u16 wBitRate/* = 4<<10*/)
	{
		return CODEC_NO_ERROR;
	}		
    u16    CKdvDecoder::SetAudDecDblSend(const TNetSndParam *ptNetSndAudioParam)
	{
		return CODEC_NO_ERROR;
		
	}


    u16    CKdvDecoder::ClearWindow() /*清屏*/
	{
		return CODEC_NO_ERROR;
	}		
    u16   CKdvDecoder::FreezeVideo() /*视频冻结*/
	{
		return CODEC_NO_ERROR;
	}		
    u16    CKdvDecoder::UnFreezeVideo()/*取消视频冻结*/
	{
		return CODEC_NO_ERROR;
		
	}


    u16    CKdvDecoder::SetDisplayMode(u8 byDisplayMode)     /*设置画中画显示方式*/
	{
		return CODEC_NO_ERROR;
	}		
    u16    CKdvDecoder::SetDisplaySaturation(u8 bySaturation)/*设置显示饱和度*/
	{
		return CODEC_NO_ERROR;

	}
    u16    CKdvDecoder::SetDisplayContrast(u8 byContrast)    /*设置显示对比度*/
	{
		return CODEC_NO_ERROR;
	}		
    u16    CKdvDecoder::SetDisplayBrightness(u8 byBrightness)/*设置显示亮度*/	
	{
		return CODEC_NO_ERROR;
	}		
    u16    CKdvDecoder::SetLittlePicDisplayParam(u16 dwXPos,u16 dwYPos,u16 dwWidth,u16 dwHeight)/*设置小画面显示位置与尺寸*/
	{
		return CODEC_NO_ERROR;
	}		
    u16    CKdvDecoder::SetGetVideoScaleCallBack(TDecodeVideoScaleInfo tDecodeVideoScaleInfo)
	{
		return CODEC_NO_ERROR;
	}		
    u16    CKdvDecoder::GetAlarmInfo(TAlarmInfo &tAlarmInfo)
	{
		return CODEC_NO_ERROR;
		
	}
	u16    CKdvDecoder::SetVideoDecParam(TVideoDecParam *ptVidDecParam)
	{
		return CODEC_NO_ERROR;
		
	}
	
	/*设置双视频流参数*/
	u16    CKdvDecoder::SetDoubleVidStreamParam(u32  dwZoomLevel, u32  dwXPos, u32  dwYPos)
	{
		return CODEC_NO_ERROR;
		
	}
	
	/*存取快照抓拍图片*/
	u16    CKdvDecoder::SetSnapshotMaxSize(u32  dwSnapshotMaxSize)
	{
		return CODEC_NO_ERROR;
		
	}
	u16    CKdvDecoder::Snapshot()
	{
		return CODEC_NO_ERROR;
		
	}
	u32    CKdvDecoder::GetSnapshotSaveSize()
	{
		return CODEC_NO_ERROR;
		
	}
	u32    CKdvDecoder::GetSnapshotSavePicNum()
	{
		return CODEC_NO_ERROR;
		
	}
	
	/*获取输出音频功率*/
	u32    CKdvDecoder::GetOutputAudioPower()
	{
		return CODEC_NO_ERROR;
		
	}


	//设置 远端动态视频载荷的 Playload值
	u16    CKdvDecoder::SetVideoActivePT( u8 byRmtActivePT, u8 byRealPT )
	{
		return CODEC_NO_ERROR;
		
	}
	//设置 视频解码key字串 以及 解密模式 Aes or Des
    u16    CKdvDecoder::SetVidDecryptKey(s8 *pszKeyBuf, u16 wKeySize, u8 byDecryptMode) 
	{
		return CODEC_NO_ERROR;
		
	}
	//设置 远端动态音频载荷的 Playload值
	u16    CKdvDecoder::SetAudioActivePT( u8 byRmtActivePT, u8 byRealPT )
	{
		return CODEC_NO_ERROR;
		
	}
	//设置 音频解码key字串 以及 解密模式 Aes or Des
    u16    CKdvDecoder::SetAudDecryptKey(s8 *pszKeyBuf, u16 wKeySize, u8 byDecryptMode) 
	{
		return CODEC_NO_ERROR;
		
	}


      /*文件编码器功能*/
    u16    CreateFileEncoder(char * pFielName)
	{
		return CODEC_NO_ERROR;
		
	}

	u16    DestroyFileEncoder()
	{
		return CODEC_NO_ERROR;
		
	}
	u16    StartFileEncoder()
	{
		return CODEC_NO_ERROR;
		
	}
	u16    StopFileEncoder()
	{
		return CODEC_NO_ERROR;
		
	}
	u16  PauseFileEncoder()
	{
		return CODEC_NO_ERROR;
		
	}
	u16    ContinueFileEncoder()
	{		return CODEC_NO_ERROR;
	
	}

	u16 SendVidFrameToMap(u16 dwStreamType, u16 dwFrameSeq, u16 dwWidth,
		                   u16 dwHeight, u8 *pbyData, u16 dwDataLen)
	{
		return CODEC_NO_ERROR;
		
	}
	u16 SendAudFrameToMap(u16 dwStreamType, u16 dwFrameSeq,  u8 byAudioMode, 
		                   u8 *pbyData, u16 dwDataLen)
	{
		return CODEC_NO_ERROR;
		
	}
	

	NULL_CLASS_DEF(CHardCodec);

	COsdMap::COsdMap()
	{
	}
    COsdMap::~COsdMap()
	{
		SAFE_DEL_PTR(m_pcCodecMap);
	}
	

	//初始化
    u16 COsdMap::Initialize(u32 dwMapID)
	{
		return CODEC_NO_ERROR;
	}

	//显示OSD，成功返回本Osd的ID号；失败返回OSD_ERROR.
	int COsdMap::OsdShow(u8 *pbyBmp, 
		         u32 dwBmpLen, 
				 u32 dwXPos, 
				 u32 dwYPos, 
				 u32 dwWidth, 
				 u32 dwHeight, 
				 u8  byBgdColor, 
				 u32 dwClarity, 
				 BOOL32  bSlide,
				 u32 dwSlideTimes,
				 u32 dwSlideStride)
	{
		return CODEC_NO_ERROR;
	}

	//关闭Osd, 成功返回OSD_OK; 失败返回OSD_ERROR.
    int COsdMap::OsdClose(u32 dwXPos, u32 dwYPos, u32 dwWidth, u32 dwHeight, BOOL32 bSlide)
	{
		return CODEC_NO_ERROR;
	}
  
	//开始台标融入码流
	int COsdMap::StartAddLogoInEncStream(u8 *pbyBmp, u32 dwBmpLen, u32 dwXPos, u32 dwYPos, u32 dwWidth, 
								u32 dwHeight, TBackBGDColor tBackBGDColor , u32 dwClarity, u32 dwChnNum, u32 dwCapPrt)
	{
		return CODEC_NO_ERROR;
	}
	//停止台标融入码流
	int COsdMap::StopAddLogoInEncStream(u32 dwChnNum, u32 dwCapPrt)
	{
		return CODEC_NO_ERROR;
	}

	//开始本地图像加入台标
	int COsdMap::StartAddIconInLocal(u8 *pbyBmp, u32 dwBmpLen, u32 dwXPos, u32 dwYPos, u32 dwWidth, 
		u32 dwHeight, TBackBGDColor tBackBGDColor , u32 dwClarity, u32 dwChnNum)
	{
		return CODEC_NO_ERROR;
	}
	//停止本地图像加入台标
	int COsdMap::StopAddIconInLocal(u32 dwChnNum)
	{
		return CODEC_NO_ERROR;
	}
	
	//开始本地显示滚动字幕(从下往上滚)
	int COsdMap::StartRunCaption(u8 *pbyBmp, u32 dwBmpLen, u32 dwTimes, u32 dwXPos, u32 dwSpeed, u32 dwWidth, 
		u32 dwHeight, u8  byBgdColor, u32 dwClarity, u32 dwChnNum)
	{
		return CODEC_NO_ERROR;
	}

	//停止本地显示滚动字幕
	int COsdMap::StopRunCaption(u32 dwChnNum)
	{
		return CODEC_NO_ERROR;
	}

	//获取OsdBuf地址
	u16 COsdMap::GetOsdBufAddr()
	{
		return CODEC_NO_ERROR;
	}
	
 
#ifdef HARD_CODEC


/* 硬件编解码器初始化函数, 必须在使用硬件编解码库中其他函数前调用. 成功返回CODEC_NO_ERROR, 失败参见错误码 */
int HardCodecInit(int dwCpuId, THardCodecInitParm *ptCodecInitParam, BOOL32  bKdv8010)
{
	return CODEC_NO_ERROR;
}
/* 硬件编解码器退出函数，调用之后，用户再不能对相应编解码器进行任何操作 */
void HardCodecQuit(int dwCpuId)
{
	return ;
}
/* 清除显示缓冲函数，用户在调用HardCodecQuit（）函数之前先调用本函数以解决reboot画面错乱问题 */
void ClearPlayBuf()
{
	return ;
}

void codecwrapperver(u32 dwMapId)
{
	return ;
}
/*   设置发送色带测试或显示色带测试
     dwChnNum -- 发送通道号，=255时表示所有编码通道都发送色带测试图片
     dwTestPicType -- 色带测试图片号，0--分辨率测试，1--色度测试，2--灰度测试，255--取消设置*/
BOOL32  SetSendTestPic(u32 dwMapId, u32 dwChnNum, u32 dwTestPicType)
{
	return TRUE;
}
BOOL32  SetPlayTestPic(u32 dwMapId, u32 dwTestPicType)
{
	return TRUE;
}


#endif














