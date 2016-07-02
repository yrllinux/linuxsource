/*****************************************************************************
  模块名      : Codecwrapper.a
  文件名      : codecwrapper_ti.h
  相关文件    : codecwrapperdef_ti.h
  文件实现功能: 
  作者        : 朱允荣
  版本        : V1.0  Copyright(C) 1997-2006 KDC, All rights reserved.
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2006/12/13  1.0         朱允荣      创建
******************************************************************************/
#ifndef _CODECWRAPPER_TI_H_
#define _CODECWRAPPER_TI_H_

#include "codecwrapperdef_ti.h"


class CVideoEncProcess;
class CAudioEncProcess;
class CVideoDecProcess;
class CAudioDecProcess;

class CAVDecoder;

class CKdvEncoder
{
public:
	CKdvEncoder();
	~CKdvEncoder();
public:
	u16    CreateEncoder(TEncoder *pVidEncoder, TEncoder *pAudEncoder);
	u16    SetEncoderActive(u32 dwCpuMask, BOOL32 bVideo);
	
	/*视频编码器加入单视频源双码流组*/
    u16    AddVidEncToSingleCapGroup(u16  wAddtoChnNum);
	
	/*发送静态图片*/
	u16    SetSendStaticPic(BOOL32  bSendStaticPic);

	/*获取输入音频功率*/
	u32    GetInputAudioPower();

	u16    ScaleVideoCap(u8 byType, u8 byValue); /* 视频采集调节 */

	/* 调用录像、放像库的接口 */
	u16 BindRecorder(u8 byRecorderId);
	
		//监控终端用
	//设置双采集单码流模式
	u16    SetDblCapOneStreamParameter(u16 dwInOut, u16 dwZoom, u16 dwXPos, u16 dwYPos);
	u16	   SetEncMoveMonitor(TMoveDetectParam *ptMoveDetectParam, u32 dwAreaNum);
	u16   SetEncMotionCallBack(TMotionAlarmInfo tMotionAlarmInfo, u32 dwContext);	
	
    u16    GetEncoderStatus(TKdvEncStatus &tKdvEncStatus);/*获取编码器状态*/ 		
    u16    GetEncoderStatis(TKdvEncStatis &tKdvEncStatis );/*获取编码器的统计信息*/

    u16    StartVideoCap(u8 byCapType = VIDCAP_CAPVIDEO);/*开始采集图像*/
    u16    StopVideoCap(); /*停止采集图像*/

    u16    SetVideoEncParam(const TVideoEncParam *ptVideoEncParam );/*设置视频编码参数*/
    u16    GetVideoEncParam(TVideoEncParam &tVideoEncParam );       /*得到视频编码参数*/
    u16    StartVideoEnc();/*开始压缩图像*/
    u16    StopVideoEnc(); /*停止压缩图像*/
	
    u16    StartAudioCap(BOOL32 bAinMic = FALSE);/*开始语音采集*/
    u16    StopAudioCap(); /*停止语音采集*/

    u16    SetAudioEncParam(u8 byAudioEncParam, 
                            u8 byMediaType=MEDIA_TYPE_PCMU,
                            u16 wAudioDuration = 30); /*设置语音压缩参数*/
    u16    GetAudioEncParam(u8 &byAudioEncParam, 
                            u8 *pbyMediaType = NULL,
                            u16 *pwAudioDuration = NULL);/*得到语音压缩参数*/

	u16    StartAudioEnc();/*开始压缩语音*/
    u16    StopAudioEnc(); /*停止压缩语音*/
    u16    SetAudioMute( BOOL32 bMute );     //设置是否静音
    u16    SetAudioVolume(u8 byVolume ); /*设置采集音量*/	
    u16    GetAudioVolume(u8 &byVolume );/*获取采集音量*/	
	
	/*设置网络重传参数*/
	u16    SetNetFeedbackVideoParam(u16 wBufTimeSpan, BOOL32 bRepeatSnd = FALSE);
	u16    SetNetFeedbackAudioParam(u16 wBufTimeSpan, BOOL32 bRepeatSnd = FALSE);
	//设置音频重发nSendTimes：重发次数，nSendSpan：每次重发间隔。(0,0)关闭。
	//该功能需要把发送重传打开, 如果没打开，将自动打开。
	u16    SetNetAudioResend(s32 nSendTimes, s32 nSendSpan);

    u16    SetNetSndVideoParam(const TNetSndParam *ptNetSndParam);/*设置图像的网络传送参数*/	
    u16    SetNetSndAudioParam(const TNetSndParam *ptNetSndParam);/*设置语音的网络传送参数*/
    u16    StartSendVideo(int  dwSSRC = 0);/*开始发送图像*/	
    u16    StopSendVideo(); /*停止发送图像*/
    u16    StartSendAudio(int  dwSSRC = 0);/*开始发送语音*/	
    u16    StopSendAudio(); /*停止发送语音*/	
	u16	   SetAudioDumb(BOOL32 bDumb); /*设置是否哑音*/	 
	
    u16    StartAec(u8 byType = 1);/*开始回声抵消*/
    u16    StopAec(); /*停止回声抵消*/

	u16	   StartAgcSmp(); /*开始smp增益*/
	u16	   StopAgcSmp(); /*停止smp增益*/
	u16	   StartAgcSin(); /*开始sin增益*/
	u16	   StopAgcSin(); /*停止sin增益*/
	
	u16    SetFastUpata(BOOL32 bIsNeedProtect = TRUE);     /*设置FastUpdata，MediaCtrl在75帧内编一个I帧发送*/


	//视频是否使用前向纠错
	u16    SetVidFecEnable(BOOL32 bEnableFec);
	//设置fec的切包长度
	u16    SetVidFecPackLen(u16 wPackLen );
	//是否帧内fec
	u16    SetVidFecIntraFrame(BOOL32 bIntraFrame);
	//设置fec算法 FEC_MODE_RAID5
	u16    SetVidFecMode(u8 byAlgorithm);
	//设置fec的x包数据包 + y包冗余包
	u16    SetVidFecXY(s32 nDataPackNum, s32 nCrcPackNum);
	
	//音频是否使用前向纠错
	u16    SetAudFecEnable(BOOL32 bEnableFec);
	//设置fec的切包长度
	u16    SetAudFecPackLen(u16 wPackLen );
	//是否帧内fec
	u16    SetAudFecIntraFrame(BOOL32 bIntraFrame);
	//设置fec算法 FEC_MODE_RAID5
	u16    SetAudFecMode(u8 byAlgorithm);
	//设置fec的x包数据包 + y包冗余包
	u16    SetAudFecXY(s32 nDataPackNum, s32 nCrcPackNum);

	//设置反转g7221c
	u16     SetReverseG7221c( BOOL32 bReverse );

	//视频动态载荷的PT值的设定
	u16 SetAudioActivePT(u8 byLocalActivePT);
	
	//音频动态载荷的PT值的设定
	u16 SetVideoActivePT(u8 byLocalActivePT );
	
	//设置音频编码加密key字串、加密的载荷PT值 以及 解密模式 Aes or Des
	u16    SetAudEncryptKey( s8 *pszKeyBuf, u16 wKeySize, u8 byEncryptMode );
	
	//设置视频编码加密key字串、加密的载荷PT值 以及 解密模式 Aes or Des
	u16    SetVidEncryptKey( s8 *pszKeyBuf, u16 wKeySize, u8 byEncryptMode ); 
public://add by flying
		//开始台标融入码流
	int StartAddLogoInEncStream(u8 *pbyBmp, u32 dwBmpLen, u32 dwXPos, u32 dwYPos, u32 dwWidth, 
								u32 dwHeight, TBackBGDColor tBackBGDColor , u32 dwClarity, u32 dwChnNum, u32 dwCapPrt = 2);
	//停止台标融入码流
	int StopAddLogoInEncStream(u32 dwChnNum, u32 dwCapPrt = 2);

	//开始横幅融入码流
	int StartAddBannerInEncStream(u8 *pbyBmp/*图像指针*/, u32 dwBmpLen/*图像大小*/, u32 dwChnNum/*通道号*/,	TAddBannerParam tAddBannerParam/*滚动横幅结构*/);
    //停止横幅融入码流
	int StopAddBannerInEncStream(u32 dwChnNum);
	
/*   设置发送色带测试或显示色带测试
     dwTestPicType -- 色带测试图片号，0--分辨率测试，1--色度测试，2--灰度测试，255--取消设置*/
	u16 SetSendTestPic(u32 dwTestPicType);
public://add by flying
	int SetVideoChanel(u32 dwChannel);
	int GetVideoChannel();
protected:
private:
	CVideoEncProcess *m_pVideoCodec;
	CAudioEncProcess *m_pAudioCodec;
};

class CKdvDecoder
{
public:
	CKdvDecoder();
	~CKdvDecoder();
public:
	u16	   CreateDecoder(TDecoder *pVidEncoder, TDecoder *pAudEncoder);
	u16    SetDecoderActive(u32 dwCpuMask, BOOL32 bVideo);
	u16    GetDecoderStatus(TKdvDecStatus &tKdvDecStatus );/*获取解码器状态*/
    u16    GetDecoderStatis(TKdvDecStatis &tKdvDecStatis );/*获取解码器的统计信息*/	

	u16    SetVideoDecType(u8 byType); /*设置图像解码器类型, 只有在开始解码前调用才有效*/
    u16    StartVideoDec();/*开始图像解码*/
    u16    StopVideoDec(); /*停止图像解码*/
	u16	   SetVideoPlayFormat(BOOL32 bFormat); /* 设置显示格式 TRUE: 16:9, FALSE: 4:3 */
	
	u16    SetAudioDecType(u8 byType); /*设置声音解码器类型, 只有在开始解码前调用才有效*/
    u16    StartAudioDec();/*开始声音解码*/	
    u16    StopAudioDec(); /*停止声音解码*/	
    u16    SetAudioVolume(u8 byVolume );  /*设置输出声音音量*/	
    u16    GetAudioVolume(u8 &pbyVolume );/*得到输出声音音量*/	
    u16    SetAudioMute(BOOL32 bMute);/*设置静音*/
	
    u16    StartRcvVideo();/*开始网络接收图像*/
    u16    StopRcvVideo(); /*停止网络接收图像*/
    u16    StartRcvAudio();/*开始网络接收声音*/	
    u16    StopRcvAudio(); /*停止网络接收声音*/	
    u16    SetVideoNetRcvParam(const TLocalNetParam *ptLocalNetParam );/*设置图像的网络接收参数*/	
    u16    SetAudioNetRcvParam(const TLocalNetParam *ptLocalNetParam );/*设置语音的网络接收参数*/
	
	/*设置图像的网络重传参数*/
	u16    SetNetFeedbackVideoParam(TNetRSParam tNetRSParam, BOOL32 bRepeatSnd = FALSE);
	u16    SetNetFeedbackAudioParam(TNetRSParam tNetRSParam, BOOL32 bRepeatSnd = FALSE);

	/*解码器发送两路码流，一路发往Map或者win32解码绘制，一路发往网络, 参数NULL,则关闭后一路，并作相应套节子释放*/
    u16    SetVidDecDblSend(const TNetSndParam *ptNetSndVideoParam, u16 wBitRate = 4<<10);
    u16    SetAudDecDblSend(const TNetSndParam *ptNetSndAudioParam); 



	//设置 H.263+/H.264 等动态视频载荷的 Playload值
	u16    SetVideoActivePT( u8 byRmtActivePT, u8 byRealPT );

	//设置 视频解码key字串 以及 解密模式 Aes or Des
    u16    SetVidDecryptKey(s8 *pszKeyBuf, u16 wKeySize, u8 byDecryptMode); 

	//设置 动态音频载荷的 Playload值
	u16    SetAudioActivePT( u8 byRmtActivePT, u8 byRealPT );

	//设置 音频解码key字串 以及 解密模式 Aes or Des
    u16    SetAudDecryptKey(s8 *pszKeyBuf, u16 wKeySize, u8 byDecryptMode); 

	/*设置视频丢包处理策略*/
	u16    SetVidDropPolicy(u8 byDropPolicy);

	//设置反转g7221c
	u16     SetReverseG7221c( BOOL32 bReverse );
	
    u16    ClearWindow();  /*清屏*/
    u16    FreezeVideo();  /*视频冻结*/
    u16    UnFreezeVideo();/*取消视频冻结*/

	u16    SetDisplayType(u8 byDisplayType);/*设置画面显示类型(VIDEO VGA)*/
    u16    SetDisplayMode(u8 byDisplayMode);     /*设置画中画显示方式*/
    u16    SetDisplaySaturation(u8 bySaturation);/*设置显示饱和度*/
    u16    SetDisplayContrast(u8 byContrast);    /*设置显示对比度*/
    u16    SetDisplayBrightness(u8 byBrightness);/*设置显示亮度*/	
    u16    SetLittlePicDisplayParam(u16 dwXPos,u16 dwYPos,u16 dwWidth,u16 dwHeight);/*设置小画面显示位置与尺寸*/
    u16    SetGetVideoScaleCallBack(TDecodeVideoScaleInfo tDecodeVideoScaleInfo);
    u16    GetAlarmInfo(TAlarmInfo &tAlarmInfo);
	u16    SetVideoDecParam(TVideoDecParam *ptVidDecParam);
	
	/*设置双视频流参数*/
	u16    SetDoubleVidStreamParam(u32  dwZoomLevel, u32  dwXPos, u32  dwYPos);
	
	/*获取输出音频功率*/
	u32    GetOutputAudioPower();

	//监控终端用
	u16	   SetDecMoveMonitor(TMoveDetectParam *ptMoveDetectParam,u32 dwAreaNum);
	u16   SetDecMotionCallBack(TMotionAlarmInfo tMotionAlarmInfo,u32 dwContext);
	
	//播放铃声
	u16   PlayRing(s8* pData = NULL);
	// 设置播放SVGA [7/17/2007]
	u16   SetPlaySVGA(BOOL32 bPlay);
public: // add by flying
	//开始本地图像加入台标
	int StartAddIconInLocal(u8 *pbyBmp, u32 dwBmpLen, u32 dwXPos, u32 dwYPos, u32 dwWidth, 
							u32 dwHeight, TBackBGDColor tBackBGDColor , u32 dwClarity, u32 dwChnNum);
	//停止本地图像加入台标
	int StopAddIconInLocal(u32 dwChnNum);
	//开始横幅融入本地
	int StartAddBannerInLocal(u8 *pbyBmp/*图像指针*/, u32 dwBmpLen/*图像大小*/, u32 dwChnNum/*通道号*/,	TAddBannerParam tAddBannerParam/*滚动横幅结构*/);
    //停止横幅融入本地
	int StopAddBannerInLocal(u32 dwChnNum);

/*   设置发送色带测试或显示色带测试
     dwTestPicType -- 色带测试图片号，0--分辨率测试，1--色度测试，2--灰度测试，255--取消设置*/
	u16 SetPlayTestPic(u32 dwMapId, u32 dwTestPicType);
protected:
private:
	CVideoDecProcess *m_pVideoCodec;
	CAudioDecProcess *m_pAudioCodec;
};

u16 HardCodecInitSlave(TSlaveInit* pInit);
u16 HardCodecInitMaster(TMasterInit* pInit);
u16 HardCodecDestroyMaster();
void InitScreen();
BOOL32 SetVideoSrcIndex(s32 nIndex);

API void SetMicVol(s32 dwVol);
API s32 GetMicVol();

u16  SendMTMsgToSlave(u32 dwID, void* pBuf, u32 dwBufSize);
void SetMTMsgFromSlaveNotify(MsgFromSlave fNotify);

//MC
API void McSwitch(BOOL32 bEnterMC);         //切换mc状态
API void SetMCParam(u8 abyMergeParam[]);    //设置mc状态时的图像合成参数
API int SetMcAudDecType(BOOL bMixer, u32 dwMcAudDecMask);	//设置mc状态时音频解码路数

/*设置PAL，NTSC制式改变消息通知*/
API void SetNtscPalChangeCallBack(CHANGECALLBACK pChangeProc);
/*   设置发送色带测试或显示色带测试
     dwChnNum -- 发送通道号，=255时表示所有编码通道都发送色带测试图片
     dwTestPicType -- 色带测试图片号，0--分辨率测试，1--色度测试，2--灰度测试，255--取消设置*/
API BOOL32  SetSendTestPic(u32 dwMapId, u32 dwChnNum, u32 dwTestPicType);
API BOOL32  SetPlayTestPic(u32 dwMapId, u32 dwTestPicType);

//设置媒体TOS值 nType类型0:全部 1:音频 2:视频 3: 数据
API	int SetMediaTOS(int nTOS, int nType);
API	int GetMediaTOS(int nType);

//设置丢包失效比
API void SetResendDrop(u32 dwInterval, u32 dwRatio);
//设置芯片死机通知回调
API void SetCpuResetNotify(CpuResetNotify fNotify);
#endif
