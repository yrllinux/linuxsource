/*****************************************************************************
  模块名      : Codecwrapper.a
  文件名      : codecwrapper_hd.h
  相关文件    : codecwrapperdef_hd.h
  文件实现功能: 
  作者        : 朱允荣
  版本        : V4.0  Copyright(C) 2007-2008 KDC, All rights reserved.
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2007/11/28  4.0         朱允荣      创建
******************************************************************************/
#ifndef _CODECWRAPPER_HD_H_
#define _CODECWRAPPER_HD_H_

#ifdef _MEDIACTRL_DM816x_

#include "dm816x/codecwrapper_hd.h"

#else

#include "codecwrapperdef_hd.h"


class CVideoEncoder;
class CAudioEncoder;
class CVideoDecoder;
class CAudioDecoder;
class CLogoAdd;

class CKdvVidEnc
{
public:
	CKdvVidEnc();
	~CKdvVidEnc();
public:
	u16    CreateEncoder(TEncoder *pEncoder);
    u16    GetEncoderStatus(TKdvEncStatus &tKdvEncStatus);/*获取编码器状态*/ 		
    u16    GetEncoderStatis(TKdvEncStatis &tKdvEncStatis);/*获取编码器的统计信息*/
	u16    SetVidDataCallback(FRAMECALLBACK fVidData, void* pContext); /*设置视频数据回调*/
    u16    GetCodecType(u32& dwType);  /*编码器工作具体位置 VID_CODECTYPE_FPGA VID_CODECTYPE_DSP*/

//    u16    StartCap();/*开始采集图像*/
//    u16    StopCap(); /*停止采集图像*/

	u16    SetVidCapPortType(u32 dwType);
	u16    SetVidCapVPID(u32 dwID);  //0:不通过DSP直接采集， 1:通过DSP环回后采集

    u16    SetVideoEncParam(const TVideoEncParam *ptVideoEncParam); /*设置视频编码参数*/
    u16    GetVideoEncParam(TVideoEncParam &tVideoEncParam );       /*得到视频编码参数*/
    u16	   SetVidEncParamCallback(VIDENCCALLBACK fVidEncParamCb, void* pbContext);		/*gaoden:EncParam设置完成之回调(2009-7-8)*/
    u16    StartEnc();/*开始压缩图像*/
    u16    StopEnc(); /*停止压缩图像*/
	
	u16    SetVideoEncSlieceLen(u32 nLen);   /*默认1400，0时表示单Slice编码。当编码分辩率1080i及以上时，至少8192。*/
    u16    GetVideoSrcInfo(TVidSrcInfo& tInfo); /*获取输入视频信号制式*/
    u16    SetVideoSrcInfo(TVidSrcInfo* ptInfo); /*禁用，已作废。 强制设置当前输入信号制式，仅限YPbPr,VGA接口*/

    /*函数名：   SetVideoEncMBQulity
    功    能： 设置宏块编码等级
    参    数： u32 dwQulityMask：宏块编码等级掩码，VIDENC_MBSIZE_ENABLE 或 VIDENC_MBSIZE_DISABLE
    返 回 值： u16  
    说    明： 任意编码中调用后，所有编码器都生效*/
    u16    SetVideoEncMBQulity(u32 dwQulityMask); 
    
    /*函数名：   ScaleVideoCap
    功    能： 调节视频采集参数,仅限C和分量端子。
    参    数： u32 dwVidSrcPort :视频采集端子 VIDIO_C0, VIDIO_YPbPr0, VIDIO_YPbPr1
               u8 byType        :调节类型 VIDCAP_SCALE_BRIGHTNESS 等
               u8 byValue       :调节数值 0~255; 0x80为默认值
    返 回 值： u16  
    说    明： */
    u16    ScaleVideoCap(u32 dwVidSrcPort,u8 byType, u8 byValue); /* 视频采集调节 */

	u16    SetFastUpata(BOOL32 bIsNeedProtect = TRUE);     /*设置FastUpdata，MediaCtrl在75帧内编一个I帧发送*/
	/*发送静态图片*/
	u16    SetSendStaticPic(BOOL32  bSendStaticPic);

	u16    StartAddLogoInEncStream(u8 *pbyBmp, u32 dwBmpLen, u32 dwXPos, u32 dwYPos,
		u32 dwWidth, u32 dwHeight, TTranspColor tBackBGDColor);	
	u16    StopAddLogoInEncStream();

	//取得当前编码状态下，横幅台标的参考底图宽高
	void   GetBannerImageSrc(u32& dwImagewidth, u32&     dwImageHeight);
	//开始横幅融入码流
	u16 StartAddBannerInEncStream(u8 *pbyBmp, u32 dwBmpLen,	const TAddBannerParam& tAddBannerParam);
    //停止横幅融入码流
	u16 StopAddBannerInEncStream();
	//1080I视频源按1080P编码
	void  Set1080IEnc1080P(BOOL32     bSet);
    
    //采集图像与编码参数间比例不一致时，处理策略：0：加黑边；1：裁边；
    u16  SetVidEncResizeMode(s32 nMode);
protected:
private:
	CVideoEncoder *m_pCodec;
};

class CKdvAudEnc
{
public:
	CKdvAudEnc();
	~CKdvAudEnc();
public:
	u16    CreateEncoder(TEncoder *pEncoder);

	u16    SetAudDataCallback(FRAMECALLBACK fAudData, void* pContext);/*设置音频数据回调*/
	/*获取输入音频功率*/
	u32    GetInputAudioPower();
	
    u16    GetEncoderStatus(TKdvEncStatus &tKdvEncStatus);/*获取编码器状态*/ 		
    u16    GetEncoderStatis(TKdvEncStatis &tKdvEncStatis );/*获取编码器的统计信息*/
    
	
    u16    StartAudioCap();/*开始语音采集*/
    u16    StopAudioCap(); /*停止语音采集*/

    u16    SetAudioEncParam(u8 byAudioEncParam, 
                            u8 byMediaType=MEDIA_TYPE_PCMU,
                            u16 wAudioDuration = 30); /*设置语音压缩参数*/
    u16    GetAudioEncParam(u8 &byAudioEncParam, 
                            u8 *pbyMediaType = NULL,
                            u16 *pwAudioDuration = NULL);/*得到语音压缩参数*/

	u16    StartAudioEnc();/*开始压缩语音*/
    u16    StopAudioEnc(); /*停止压缩语音*/

    u16    SetAudioMute( BOOL32 bMute );     //设置是否编静音码流

    u16    SetAudioVolume(u8 byVolume ); /*设置采集音量*/	
    u16    GetAudioVolume(u8 &byVolume );/*获取采集音量*/
    
    u16    SetAudInPort(u32 dwAudPort);  /*设置音频采集端口 HDMI or C*/	
	
    u16    StartAec();/*开始回声抵消*/
    u16    StopAec(); /*停止回声抵消*/

	u16	   StartAgcSmp(); /*开始smp增益*/
	u16	   StopAgcSmp(); /*停止smp增益*/
	u16	   StartAgcSin(); /*开始sin增益*/
	u16	   StopAgcSin(); /*停止sin增益*/		
protected:
private:
	CAudioEncoder *m_pCodec;
};

class CKdvVidDec
{
public:
	CKdvVidDec();
	~CKdvVidDec();
public:
	u16	   CreateDecoder(TDecoder *pDecoer);
    u16    StartDec();/*开始图像解码*/
    u16    StopDec(); /*停止图像解码*/
	u16    GetDecoderStatus(TKdvDecStatus &tKdvDecStatus);/*获取解码器状态*/
    u16    GetDecoderStatis(TKdvDecStatis &tKdvDecStatis);/*获取解码器的统计信息*/	
    u16    GetCodecType(u32& dwType);  /*解码器工作具体位置 VID_CODECTYPE_FPGA VID_CODECTYPE_DSP*/

	u16    SetData(const TFrameHeader& tFrameInfo);        /*设置视频码流数据*/

	/*设置视频丢包处理策略*/
	u16    SetVidDropPolicy(u8 byDropPolicy);

    u16    SetDisplayType(u8 byDisplayType);/*设置画面显示类型(VIDEO VGA)*/
    u32    SetWorkMode(u32 dwWorkMode);     /*设置hdu工作模式，指hdu-1、hdu-2模式，hdu和hdu-d用于区分hdu启动模式*/
    
    u16    SetGetVideoScaleCallBack(TDecodeVideoScaleInfo tDecodeVideoScaleInfo);
	u16    SetVideoDecParam(TVideoDecParam *ptVidDecParam);
    
    /*函数名：   SetPlayScale
    功    能： 设置播放设备的显示比例。
    参    数： u16 wWidth  :显示宽比例，0为自动
               u16 wHeight :显示高比例，0为自动。
    返 回 值： u16  
    说    明： 例如：输入(0,0)（默认）为保持播放制式比例输出，(16,9)表示播放设备显示比例为16:9*/
    u16    SetPlayScale(u16 wWidth, u16 wHeight);
    
    u16    SetVidPlyPortType(u32 dwType);
    u16    SetVideoPlyInfo(TVidSrcInfo* ptInfo);  /*设置视频输出信号制式，NULL表示自动调整。*/
    u16    GetVideoSrcInfo(TVidSrcInfo& tInfo);   /*获取输出视频信号制式*/
    //u16    SetVidPlyDefault(BOOL32 bNtsc = FALSE); /*是否默认采用 30/60播放*/
    //u16    SetVidPlyVgaDefault(u32 dwFreq = 60); /*VGA播放频率默认60Hz，其他75,85等*/
    //u16    SetAutoCheckPlayInfo(BOOL32 bAuto);    /*是否自动调整播放帧率*/
	//设置VGA、1080I、1080P的输出频率(参数参加codecwrapperdef_hd)
	void   SetVidOutFreq(u8 byVGAFreq, u8 by1080IFreq, u8 by1080PFreq);
    u16    ClearWindow();  /*清屏*/
    u16    FreezeVideo();  /*视频冻结*/
    u16    UnFreezeVideo();/*取消视频冻结*/	
	void   SetCheckFpga(BOOL32 bIsCheck);	//设置是否检查FPGA异常
	void   SetFpgaErrReboot(u32 dwErrCount);//设置fpga重启的错误次数
	u16	   SetVidDecResizeMode(s32 nMode);//处理策略：0：加黑边；1：裁边；
	u32    GetRunCaptionTimes();
	u16    SetDspPlayCallBack(CHANGECALLBACK fDspPlay,void *pContext);
	u16	   SetNoStreamBak(u32 dwBakType, u8* pbyBmp = NULL, u32 dwBmpLen = 0);
protected:
private:
	CVideoDecoder *m_pCodec;
};

class CKdvAudDec
{
public:
	CKdvAudDec();
	~CKdvAudDec();
public:
	u16	   CreateDecoder(TDecoder *pDecoer);
	u16    GetDecoderStatus(TKdvDecStatus &tKdvDecStatus);/*获取解码器状态*/
    u16    GetDecoderStatis(TKdvDecStatis &tKdvDecStatis);/*获取解码器的统计信息*/	

    u16    SetData(const TFrameHeader& tFrameInfo); /*设置音频码流数据*/
    u16    SetAudioDecParam(const TAudioDecParam& tAudParam);/*音频解码参数，AAC_LC有效*/ 
    
    u16    SetAudOutPort(u32 dwAudPort);  /*设置音频播放端口 HDMI or C*/

    u16    StartDec();/*开始声音解码*/	
    u16    StopDec(); /*停止声音解码*/	
    u16    SetAudioVolume(u8 byVolume );  /*设置输出声音音量*/	
    u16    GetAudioVolume(u8 &pbyVolume );/*得到输出声音音量*/	
    u16    SetAudioMute(BOOL32 bMute);/*设置静音*/
		
	/*获取输出音频功率*/
	u32    GetOutputAudioPower();
	
	//播放铃声 ？？？
	u16   PlayRing(s8* pData = NULL);
protected:
private:
	CAudioDecoder *m_pCodec;
};
// 
// class COsdMap
// {
// public:
// 	COsdMap();
// 	~COsdMap();
// public:
// 	u16   Create(u32 dwDspID);
// 	u16   Destroy();
// 	//上传图片
// 	u16   LoadBmp(u32 dwBmpID, u8 *pbyBmp, u32 dwBmpLen, TTranspColor tBackBGDColor);
// 	//设置台标的图片，位置
// 	u16   SetLogoInfo(u32 dwLogoID, u32 dwBmpID, u32 dwXPos, u32 dwYPos);
// 	//显示台标
// 	u16   ShowLogo(u32 dwLogoID, BOOL32 bShow = TRUE);
// 	//清除台标的图片和位置信息。SetLogoInfo的反操作
// 	u16   ClearLogoInfo(u32 dwLogoID);
// 	//清理图片。LoadBmp的反操作
// 	u16   ClearBmp(u32 dwBmpID);
// 
// 	//上传数字图片。dwSumOfNumPic：图片中数字小图片的个数，图片必须纵向排列
// 	u16   LoadNumBmp(u8 *pbyBmp, u32 dwBmpLen, TTranspColor tBackBGDColor, u32 dwSumOfNumPic);
// 	//设置显示图片的内容，位置
// 	u16   SetNumLogoInfo(u32 dwNumLogoID, u8 *pbyLogoInfo, u32 dwLogoNum, u32 dwXPos, u32 dwYPos);
// 	//显示数字图片
// 	u16   ShowNumLogo(u32 dwNumLogoID, BOOL32 bShow = TRUE);
// protected:
// private:
// 	CLogoAdd  *m_pLogoAdd;
// };

class COsdMap
{
public:
	COsdMap();
	~COsdMap();
public:
	u16   Create(u32 dwDspID);
	u16   Destroy();
	u16   OsdShow(u8 *pbyBmp, u32 dwBmpLen, 
		u32 dwXPos, u32 dwYPos,	u32 dwWidth, u32 dwHeight, 
		u8  byBgdColor, u32 dwClarity, BOOL32 bAutoCenter = TRUE);

	//关闭Osd, 成功返回OSD_OK; 失败返回OSD_ERROR.
    u16 OsdClose(u32 dwXPos, u32 dwYPos, u32 dwWidth, u32 dwHeight, BOOL32 bAutoCenter = TRUE);	

    /*函数名： OsdDefaultClarity
    功    能： 设置默认的透明度，未设置则为不透明
    参    数： u8 byClarity：透明度，有效位只有三位(xxxx 000x),x取值不会影响透明度。
                             从不透明到透明为(0, 2, 4, 6, 8, 10, 12, 14)。
    返 回 值： u16 
    说    明： */
    u16 OsdDefaultClarity(u8 byClarity);
	
    //Osd滚动短消息
    //返回值:Codec_Error_Param, 	Codec_Success
    u16  StartRunCaption(
		u8 *pbyBmp,        //滚动图片数据
		u32 dwBmpLen,      //滚动图片长度
		u32 dwBgXPos,      //滚动框起点x
		u32 dwBgYPos,      //滚动框起点y
		u32 dwBgWidth,     //滚动框宽度
		u32 dwBgHeigh,     //滚动框高度
		u8  byBgColor,     //滚动框透明色
		u32 dwClarity,     // 透明度
		u32 dwTimes = 1,   //循环滚动的次数(默认1)
		u32 dwSpeed = RUNSPEED_NORM,   //滚动速度(见codecwapperdef_hd宏定义，默认RUNSPEED_NORM) 
		u32 dwRunMode = RUNMODE_LEFTRIGHT);  //滚动方式(见codecwapperdef_hd宏定义，默认RUNMODE_LEFTRIGHT)
    //停止Osd滚动短消息
    void  StopRunCaption();  
protected:
private:
	CLogoAdd  *m_pLogoAdd;
};

//设定当前*.bin,*.out路径，默认/usr/bin
void SetCodecAppDir(s8* szPath);
//注册OSP命令
void RegsterCommands();

//初始化DSP芯片
u16 HardCodecInitDSP(TDSPInit* pInit);
u16 HardCodecDestroyDSP(u32 dwDSPID);
//初始化FPGA
u16 HardCodecDevInit(u32 nDecDevNum, u32 nEncDevNum);
u16 HardCodecDestroy();

//接口FPGA设置
    /*函数名： SetVideoOutFromVidIn
    功    能： 切换输入口视频到指定输出接口输出。
    参    数： u32 dwFromVidIn：输入接口类型VIDIO_HDMI0等，只能指定一个接口
               u32 dwVidOut   ：输出接口类型VIDIO_HDMI0等，可指定多个接口
                                如（VIDIO_YPbPr0|VIDIO_HDMI0）
               TVidSrcInfo* ptOutInfo:输出视频格式，可为NULL
    返 回 值： u16 :0 成功，非零 失败。
    说    明： 可一对多，不能多对多或多对一*/
u16 SetVideoOutFromVidIn(u32 dwFromVidIn, u32 dwVidOut, TVidSrcInfo* ptOutInfo = NULL);
    /*函数名： SetVideoOutFromVidDec
    功    能： 切换解码器的输出内容到其他接口。
    参    数： u32 dwFromDecVP：作为输入的解码器的VPID (0 , 1, 2)
               u32 dwVidOut   ：输出接口类型VIDIO_HDMI0等，可指定多个接口
               TVidSrcInfo* ptOutInfo：输出视频格式，不能为NULL
    返 回 值： u16 
    说    明： */
u16 SetVideoOutFromVidDec(u32 dwFromDecVP, u32 dwVidOut, TVidSrcInfo* ptOutInfo);

API void SetMicVol(s32 dwVol);
API s32 GetMicVol();

/*设置播放分辨率改变消息通知*/
API void SetNtscPalChangeCallBack(CHANGECALLBACK pChangeProc);

//设置芯片死机通知回调
API void SetCpuResetNotify(CpuResetNotify fNotify);

//是否启用全屏OSD
API void UseFullOSD(BOOL32 bFullOsd = TRUE);
//HDMI口输出HDMI、DVI信号选择
API void HDMIDVISwitch(BOOL32 bHDMI = TRUE);
//设置SD信号输出制式
API void SetSDOutType(u32 dwOutType = SDOUTTYPE_576I);	   //参数参加codecwrapperdef_hd
//重启编解码设备
API void RebootCodecDev(u32 dwDevID = CODECDEV_VIDENC);	   //参数参加codecwrapperdef_hd


s32 HardMPUInit(u32 nMode); //nMode:enInitMode
s32 HardMPUInitDev(u32 nEncTos, u32 nDecTos, u32 nDspNum);
s32 HardMPUDestroy();
#endif //MEDIACTRL_DM816x
#endif
