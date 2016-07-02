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
#ifndef _CODECWRAPPER_HD3_H_
#define _CODECWRAPPER_HD3_H_

#include "codecwrapperdef_hd.h"


class CVideoEncoder;
class CAudioEncoder;
class CVideoDecoder;
class CAudioDecoder;

class CKdvVidEnc
{
public:
	CKdvVidEnc();
	~CKdvVidEnc();
public:
	u16    CreateEncoder(TEncoder *pEncoder);                 /*HD1等使用*/
    u16    Create(TVidEncInit* pEnc, void* pReserve = NULL);  /*二代高清终端请使用这个接口*/
	u16    CreateHD3(TVidEncHD3RD* pEnc);							/*三代高清使用这个接口*/
    u16    GetEncoderStatus(TKdvEncStatus &tKdvEncStatus);/*获取编码器状态*/ 		
    u16    GetEncoderStatis(TKdvEncStatis &tKdvEncStatis);/*获取编码器的统计信息*/
	u16    SetVidDataCallback(FRAMECALLBACK fVidData, void* pContext); /*设置视频数据回调*/
    u16    GetCodecType(u32& dwType);  /*编码器工作具体位置 VID_CODECTYPE_FPGA VID_CODECTYPE_DSP*/
	//     追加编码清晰度接口，TRUE为提高清晰度，FALSE为关闭清晰度。会有性能损失
	u16    SetEncQualityEnhance(BOOL32 bEnhance);
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
       u16 StartAddLogoInEncStream(TFullLogoParam &tLogoParm);
	u16    StopAddLogoInEncStream();

	//取得当前编码状态下，横幅台标的参考底图宽高
	void   GetBannerImageSrc(u32& dwImagewidth, u32&     dwImageHeight);
	//开始横幅融入码流
	u16 StartAddBannerInEncStream(u8 *pbyBmp, u32 dwBmpLen,	const TAddBannerParam& tAddBannerParam);
       u16 StartAddBannerInEncStream(TFullBannerParam &tBannerParam,BOOL32 bSpaceArea=TRUE/*图片前后是否带有空白区*/);
    //停止横幅融入码流
	u16 StopAddBannerInEncStream();
	//1080I视频源按1080P编码
	void  Set1080IEnc1080P(BOOL32     bSet,BOOL32 bDeinterlace = FALSE);
    
    //采集图像与编码参数间比例不一致时，处理策略：0：加黑边；1：裁边；
    u16  SetVidEncResizeMode(s32 nMode);
    u16    SetVidPlayPolicy(u32 dwTimeout = 5, u32 dwPlayType = EN_PLAY_LAST);
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
	u16    CreateEncoder(TEncoder *pEncoder, u32 dwDevVersion=en_H600_Board);

	u16    SetAudDataCallback(FRAMECALLBACK fAudData, void* pContext);/*设置音频数据回调*/
	/*获取输入音频功率*/
	u32    GetInputAudioPower();
	
    u16    GetEncoderStatus(TKdvEncStatus &tKdvEncStatus);/*获取编码器状态*/ 		
    u16    GetEncoderStatis(TKdvEncStatis &tKdvEncStatis );/*获取编码器的统计信息*/
    
	
    u16    StartAudioCap();/*开始语音采集*/
    u16    StopAudioCap(); /*停止语音采集*/

    u16    SetAudioEncParam(u8 byMediaMode, 
                            u8 byMediaType=MEDIA_TYPE_PCMU,
                            u16 wAudioDuration = 30); /*设置语音压缩参数*/
    u16    GetAudioEncParam(u8 &byAudioEncParam, 
                            u8 *pbyMediaType = NULL,
                            u16 *pwAudioDuration = NULL);/*得到语音压缩参数*/

	u16    StartAudioEnc();/*开始压缩语音*/
    u16    StopAudioEnc(); /*停止压缩语音*/

    u16    SetAudioMute( BOOL32 bMute );     //设置是否编静音码流

    u16    SetAudioVolume(u8 byVolume ); /*设置采集音量*/	
	u16	   SetSingleAudInputVol(u32 dwAudPort, u32 dwVol);//设置单独音频接口音量,只能设置一路

    u16    GetAudioVolume(u8 &byVolume );/*获取采集音量*/
    
    u16    SetAudInPort(u32 dwAudPort, u32 dwAudMasterMic);  /*设置音频采集端口 HDMI or C（包括主麦，即所有音频口）；第二个参数只设置主麦，用于AEC处理，其他接口不作AEC处理*/	
	
    u16    StartAec();/*开始回声抵消*/
    u16    StopAec(); /*停止回声抵消*/
	u16	   AecDebug(u32 dwDebug);/*范围4-64*/

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
	u16	   CreateDecoder(TDecoder *pDecoer);   /*HDU, HD1等使用*/
	u16    Create(TVidDecInit* pDec, void* pReserve = NULL); /*二代高清终端请使用这个接口*/
	u16    CreateHD3(TVidDecHD3RD* pDec);
	u16    Create();
	u16    StartDec();/*开始图像解码*/
	u16    StopDec(); /*停止图像解码*/
	u16    GetDecoderStatus(TKdvDecStatus &tKdvDecStatus,u32 dwChnl = 0);/*获取解码器状态*/
    u16    GetDecoderStatis(TKdvDecStatis &tKdvDecStatis,u32 dwChnl = 0);/*获取解码器的统计信息*/	
    u16    GetCodecType(u32& dwType);  /*解码器工作具体位置 VID_CODECTYPE_FPGA VID_CODECTYPE_DSP*/

	u16    SetData(const TFrameHeader& tFrameInfo,u32 dwChnl = 0);        /*设置视频码流数据*/

	/*设置视频丢包处理策略*/
	u16    SetVidDropPolicy(u8 byDropPolicy);

    u16    SetDisplayType(u8 byDisplayType);/*设置画面显示类型(VIDEO VGA)*/
    
    u16    SetGetVideoScaleCallBack(TDecodeVideoScaleInfo tDecodeVideoScaleInfo);
	u16    SetVideoDecParam(TVideoDecParam *ptVidDecParam);

    /*设置视频画中画*/
    u16 SetVidPIPParam(enVideoPIPMode enMode, enVideoPIPIndex enIndex[3]);
    
    /*函数名：   SetPlayScale
    功    能： 设置播放设备的显示比例。
    参    数： u16 wWidth  :显示宽比例，0为自动
               u16 wHeight :显示高比例，0为自动。
    返 回 值： u16  
    说    明： 例如：输入(0,0)（默认）为保持播放制式比例输出，(16,9)表示播放设备显示比例为16:9*/
    u16    SetPlayScale(u16 wWidth, u16 wHeight);
    
    u16    SetVidPlyPortType(u32 dwType);
    u16    SetVideoPlyInfo(u32 dwOutType = VID_OUTTYPE_ORIGIN);//设置视频输出信号制式。
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
	u16	   SetDecTreshold(const u32 dwThresh = 352*288);
	u16	   SetVidOutCallBack(VIDOUTTYPECALLBACK fVidOutType);
	u16	   SetUse720P60(BOOL32 bUse720P60 = FALSE);
	u16    SetStreamPlayTypeCb(VIDDECPLAYTYPECALLBACK fVidDecPlayType);
    u32    GetOutVP(); //二代高清，表示输出VP口，用于SetVideoOutFromVidDec的第一个参数。
	u16	   TestImage(BOOL32 bSet);//色带测试
	u16    SnapShot(s8* pFileName);//快照
	u16    SnapShotView(s8* pFileName);//快照浏览
	u16    SetVidPlayPolicy(u32 dwTimeout = 5, u32 dwPlayType = EN_PLAY_LAST);
	u16     SetDecRate(u32 dwDecRate = 30);//解码流控
	u16     SetCutPixel(u32 dwCutPixel);
	u16		OnOutToCPort(BOOL32 bPlayOsd=FALSE);//C端子是否播放OSD
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
	u16	   CreateDecoder(TDecoder *pDecoer, u32 dwDevVersion=en_H600_Board);
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
	/*三代高清中dwDspID填0，pPlayStd填主输出的播放制式.
	需要注意的是解码器一定要先创建好，才能调用该接口，不然osd会失败
	*/
	u16   Create(u32 dwDspID,TVidSrcInfo *pPlayStd=NULL);
	u16   Destroy();
	u16   OsdShow(u8 *pbyBmp, u32 dwBmpLen, 
		u32 dwXPos, u32 dwYPos,	u32 dwWidth, u32 dwHeight, 
		u32 dwBgdColor, u32 dwClarity, BOOL32 bAutoCenter = TRUE);
	
	/*三代高清osd使用以下接口，旧接口不生效*/
	u16   OsdShow(u8 *pbyBmp, u32 dwBmpLen, 
		u32 dwXPos, u32 dwYPos,	u32 dwWidth, u32 dwHeight);
	
	//关闭Osd, 成功返回OSD_OK; 失败返回OSD_ERROR.
    u16 OsdClose(u32 dwXPos, u32 dwYPos, u32 dwWidth, u32 dwHeight, BOOL32 bAutoCenter = TRUE);	

    /*函数名： OsdDefaultClarity
    功    能： 设置默认的透明度，未设置则为不透明
    参    数： u8 byClarity：透明度(0-255,255为不透明)。
    返 回 值： u16 
    说    明： */
    u16 OsdDefaultClarity(u8 byClarity);
    /*函数名： OsdDefaultBGDColor
    功    能： 设置默认的透明色，未设置为白色
    参    数： u32 dwRed
               u32 dwGreen
               u32 dwBlue
    返 回 值： u16 
    说    明： */
	u16 OsdDefaultBGDColor(u32 dwRed, u32 dwGreen, u32 dwBlue);
	
    //Osd滚动短消息
    //返回值:Codec_Error_Param, 	Codec_Success
    u16  StartRunCaption(
		u8 *pbyBmp,        //滚动图片数据
		u32 dwBmpLen,      //滚动图片长度
		u32 dwBgXPos,      //滚动框起点x
		u32 dwBgYPos,      //滚动框起点y
		u32 dwBgWidth,     //滚动框宽度
		u32 dwBgHeigh,     //滚动框高度
		u32 dwBgColor,     //滚动框的颜色,填rgb:0x0RGB
		u32 dwClarity,     // 透明度
		u32 dwTimes = 1,   //循环滚动的次数(默认1)
		u32 dwSpeed = RUNSPEED_NORM,   //滚动速度(见codecwapperdef_hd宏定义，默认RUNSPEED_NORM) 
		u32 dwRunMode = RUNMODE_LEFTRIGHT);  //滚动方式(见codecwapperdef_hd宏定义，默认RUNMODE_LEFTRIGHT)
    //停止Osd滚动短消息
    void  StopRunCaption(); 
    // 三代高清中，可支持多个滚动短消息，所以在关闭时需要指定范围 
    void  StopRunCaption(u32 dwBgXPos,      //滚动框起点x
		u32 dwBgYPos,      //滚动框起点y
		u32 dwBgWidth,     //滚动框宽度
		u32 dwBgHeigh     //滚动框高度
		); 
    // 设置osd图片的相应制式，三代高清中有720p和1080p两组,参数分别为1280x720,1920x1080
    void  SetOsdStd(u32 dwWidth, u32 dwHeight);
protected:
private:
	u32	m_dwOsdWidth;
	u32	m_dwOsdHeight;
	TTranspColor m_tTranColor;
	//CLogoAdd  *m_pLogoAdd;
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
u16 HardEnvInitial(u32 dwHWtype);

s32    ScaleVideoCap(u32 dwVidSrcPort,u8 byType, u8 byValue); /* 视频采集调节 */

//////////////////////////////////////////////////////////////////////////
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
//////////////////////////////////////////////////////////////////////////
    /*函数名： SetVideoOutFromVidDec
    功    能： 切换解码器的输出内容到其他接口。
    参    数： u32 dwFromDecVP：作为输入的解码器的VPID (0 , 1, 2)
                             ***HD2的输入需要详细指定，可通过
               u32 dwVidOut   ：输出接口类型VIDIO_HDMI0等，可指定多个接口
               TVidSrcInfo* ptOutInfo：输出视频格式，不能为NULL
    返 回 值： u16 
    说    明： */
u16 SetVideoOutFromVidDec(u32 dwFromDecVP, u32 dwVidOut, TVidSrcInfo* ptOutInfo = NULL);
/*=============================================================================
  函 数 名： GetVGAPhaseClock
  功    能： 获取VGA源相位时钟和分辨率
  算法实现： 
  全局变量： 
  参    数： u32 dwVGAPort
             TVidSrcInfo* ptVGAInfo
             u32& dwPhaseValue
  返 回 值： u16 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2010/6/17     1.0	        王雪宁                   创建
=============================================================================*/
u16 GetVGAPhaseClock(u32 dwVGAPort, TVidSrcInfo* ptVGAInfo, u32& dwPhaseValue);
/*=============================================================================
  函 数 名： SetVGAPhaseClock
  功    能： 设置VGA源相位时钟
  算法实现： 
  全局变量： 
  参    数： u32 dwVGAPort
             u32 dwPhaseValue
  返 回 值： u16 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2010/6/17     1.0	        王雪宁                   创建
=============================================================================*/
u16 SetVGAPhaseClock(u32 dwVGAPort, u32 dwPhaseValue);
/*=============================================================================
  函 数 名： GetVGAFiltStatus
  功    能： 获取VGA源滤波状态
  算法实现： 
  全局变量： 
  参    数： u32 dwVGAPort
             TVidSrcInfo* ptVGAInfo
             u32& dwFiltSet
  返 回 值： u16 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2010/6/17     1.0	        王雪宁                   创建
=============================================================================*/
u16 GetVGAFiltStatus(u32 dwVGAPort, TVidSrcInfo* ptVGAInfo, u32& dwFiltSet);
/*=============================================================================
  函 数 名： SetVGAFiltStatus
  功    能： 设置VGA源滤波
  算法实现： 
  全局变量： 
  参    数： u32 dwVGAPort
             u32 dwFiltSet
  返 回 值： u16 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2010/6/17     1.0	        王雪宁                   创建
=============================================================================*/
u16 SetVGAFiltStatus(u32 dwVGAPort, u32 dwFiltSet);

/*=============================================================================
  函 数 名： SetVDecTosMaxBufNum
  功    能： 设置TOS解码在流控状态时最大堆积待解码帧数
  算法实现： 
  全局变量： 
  参    数： u32 dwBufferMax 
             s32 nIndex
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2010/8/28     1.0	        王雪宁                   创建
=============================================================================*/
void SetVDecTosMaxBufNum(u32 dwBufferMax, s32 nIndex);

/*=============================================================================
  函 数 名： GetVidPortInfo
  功    能： 获取视频源端口和制式
  算法实现： 
  全局变量： 
  参    数： u32 dwVidPortType:查询接口，如VIDIO_DVI0等
             TVidPortInfo* ptVidPortInfo:返回实际端口和制式
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2011/4/27     1.0	        王雪宁                   创建
=============================================================================*/
void GetVidPortInfo(u32 dwVidPortType, TVidPortInfo* ptVidPortInfo);

void CloseTaos(u32 nDevId);

/*函数名： SetMicVol
功    能： 卡农麦克风增益，范围0-50
参    数： u32 dwAudPort
           s32 nVol
返 回 值： API void 
说    明： */
API void SetMicVol(u32 dwAudPort, s32 nVol);

API s32 GetMicVol();

//设置h600摄像头制式
API void SetVidInPortStd(TVidSrcInfo* ptCamInfo);

//获取输出DVI接口上实际的物理接口类型
void GetVidOutPortType(u32 dwDVIId, u32 &dwRealPortType);
/*函数名： SetCanonPowerOn
功    能： 卡农麦克风输入接口48v供电开关接口实现
参    数： BOOL32 bEnable=TRUE
返 回 值： 
说    明： */
void SetCanonPowerOn(u32 dwAudPort, BOOL32 bEnable=TRUE);

//获取数字麦插入状态
API BOOL32 GetDigitalMicStatus(u32 dwAudPort);

//获取3.5口插入状态  bIsAudIn = TRUE:采集口  FALSE:播放口
API BOOL32 Get35InOrOutStatus(BOOL32 bIsAudIn);

API BOOL32 GetTRS635Status(u32 dwAudPort);

//获取HDMI音频口状态
API BOOL32 GetAudHdmiStatus(u32 dwAudPort);

/*获取SDI接口类型 SDITYPE_ININ SDITYPE_INOUT*/
API u32 GetHDMtSDIIntType();

/*设置播放分辨率改变消息通知*/
API void SetNtscPalChangeCallBack(CHANGECALLBACK pChangeProc);

//设置芯片死机通知回调
API void SetCpuResetNotify(CpuResetNotify fNotify);
//无视频源或无码流时，超时播放图像:EN_PLAY_LAST/EN_PLAY_BLUE
u16    SetVidPlayPolicy(u32 dwTimeout = 5, u32 dwPlayType = EN_PLAY_LAST);

//是否启用全屏OSD
API void UseFullOSD(BOOL32 bFullOsd = TRUE);
//HDMI口输出HDMI、DVI信号选择
API void HDMIDVISwitch(BOOL32 bHDMI = TRUE);
//设置SD信号输出制式
API void SetSDOutType(u32 dwOutType = VID_OUTTYPE_ORIGIN);	   //参数参加codecwrapperdef_hd
//重启编解码设备
API void RebootCodecDev(u32 dwDevID = CODECDEV_VIDENC);	   //参数参加codecwrapperdef_hd
//判断2批D版或之前版本
API void SetHardWareVer(BOOL32 bBefore2ndD = FALSE);
s32 HardMPUInit(u32 dwHWType_Mode); //指定开发板类型以及工作模式:codecwrapperdef_hd:enBoardType
s32 HardMPUInitDev(u32 nEncTos, u32 nDecTos, u32 nDspNum);
s32 HardMPUDestroy();
//高清终端初始化
s32 HardMTInit(u32 nMode);
s32 HardMTDestroy();
//关闭与恢复视频显示
void SetVideoShow(BOOL32 bShow);
//设置降噪等级 参见 codecwrapperdef_hd.h： NF_MODE_DISABLE etc，仅对第一路有效
void SetNfMode(u32 dwMode);
//设置降噪开关 推荐使用 TRUE
void SetNfEnable(BOOL32 bOpen);
#endif
