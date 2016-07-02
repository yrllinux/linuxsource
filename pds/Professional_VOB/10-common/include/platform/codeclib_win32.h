/* CodecLib.h - Codec API Header */ 

/* Copyright(C) 2001-2002 KDC, All rights reserved. 
说明:CODEC上层API封装函数
作者:胡昌威 魏治兵
*/

#ifndef __CODECLIB_H_
#define __CODECLIB_H_

#include "KdvDef.h"
#include "CodecCommon.h"

/*=======================================常量、宏与结构的定义=================================*/

#ifdef SOFT_CODEC

//模块版本信息和编译时间 及 依赖的库的版本和编译时间
API void kdvencoderver();
API void kdvdecoderver();

//模块帮助信息 及 依赖的库的帮助信息
API void kdvencoderhelp();
API void kdvdecoderhelp();

#endif


enum
{
	MC_CODERC_NULL = 0,
	MC_CODERCDEC_1,
	MC_CODERCDEC_2,
	MC_CODERCDEC_3,
	MC_CODERCDEC_4,
	MC_CODERCDEC_5,
	MC_CODERC_LOCAL,
	MC_CODERC_END
};

enum
{
	MC_PICMEGER_TYPE_ONE = 0,
	MC_PICMEGER_TYPE_TWO,
	MC_PICMEGER_TYPE_THREE,
	MC_PICMEGER_TYPE_FOUR,
	MC_PICMEGER_TYPE_FOUR_BIG,
	MC_PICMEGER_TYPE_SIX,
	MC_PICMEGER_TYPE_TWO_BIG
};

/*编码比特率(Kbps)*/
#define  BIT_RATE_ZERO           (u16)0 

/*网络发送带宽(单位:Kbps,1K=1024)*/   
//#define  SND_NET_BAND          (u8)100<<10

/*I帧间最大P帧数目*/
#define  MAX_P_FARME_INTERNAL_I  (u8)75   /*I帧间最大P帧为75*/

/*量化参数(1-31)*/
#define  MAX_QUANT_PARAM         (u8)31   /*最大量化参数*/
#define  MIN_QUANT_PARAM         (u8)1    /*最小量化参数*/
#define  QUANT_PARAM_31          (u8)31   /*最大量化参数*/
#define  QUANT_PARAM_1           (u8)1    /*最小量化参数*/

/*图像大小*/
#define  VIDEO_WIDTH_720         (u16)720  /*图像高度*/
#define  VIDEO_HEIGHT_576        (u16)576  /*图像宽度*/
#define  VIDEO_WIDTH_352         (u16)352  /*图像高度*/
#define  VIDEO_HEIGHT_288        (u16)288  /*图像宽度*/
#define  VIDEO_WIDTH_DEFAULT     (u16)640  /*图像默认高度*/
#define  VIDEO_HEIGHT_DEFAULT    (u16)480  /*图像默认宽度*/

/*帧率(default:25)*/
#define  FRAME_RATE_ONE          (u8)1    /*默认帧率*/
#define  FRAME_RATE_DEFAULT      (u8)25   /*默认帧率*/

/*图象压缩质量(1-5)*/
#define  IMAGE_ENCODE_QUALITY_TWO  (u8)2   /*压缩质量2*/
#define  IMAGE_ENCODE_QUALITY_FOUR (u8)4   /*压缩质量4*/     

/*声音模式 mp3*/
#define	 AUDIO_MODE_WORST        (u8)0/*最差*/
#define	 AUDIO_MODE_BAD          (u8)1/*差  */
#define	 AUDIO_MODE_NORMAL       (u8)2/*一般*/
#define	 AUDIO_MODE_FINE         (u8)3/*好  */
#define	 AUDIO_MODE_BEST         (u8)4/*最好*/

/*声音音量*/
#define  AUDIO_VOLUME_2          (u8)2  
#define  AUDIO_VOLUME_25         (u8)25  

#define MAX_AUDIO_DEV_NAME_LEN    255     //声卡最大长度255字节
#define MAX_AUDIO_DEV_NUMBER      31      //最多支持31块声卡
//G.711
#define  AUDIO_MODE_PCMA         (u8)5//a law
#define  AUDIO_MODE_PCMU         (u8)6//u law 
//G.723
#define AUDIO_MODE_G723_6        (u8)7 
#define AUDIO_MODE_G723_5        (u8)8
//G.728
#define AUDIO_MODE_G728		 (u8)9 
//G.722
#define AUDIO_MODE_G722		 (u8)10 
//G.729
#define AUDIO_MODE_G729		 (u8)11 
//G.7221
#define AUDIO_MODE_G7221		 (u8)12 
//adpcm
#define AUDIO_MODE_ADPCM		 (u8)13 

#define AUDIO_MODE_AACLC_32      (u8)14

#define AUDIO_MODE_AACLD_32      (u8)15

#define AUDIO_MODE_AACLC_32_SINGLE (u8)16

//定义视频会议产品线的产品版本
#define T2_V1R2 (u32)1
#define HD      (u32)2


#define  MAX_LOCAL_IPADDR_LEN    (u8)30 /*最大的ip地址长度*/

#ifndef  MAX_NETSND_DEST_NUM
#define  MAX_NETSND_DEST_NUM     (u8)5  /*最大发送的目的地数*/
#endif

#ifndef MEDIA_TYPE
#define MEDIA_TYPE
/*图像编码类型
#define  MEDIA_TYPE_MP4	         (u8)97//MPEG-4 
#define  MEDIA_TYPE_H261		 (u8)31//H.261  
#define  MEDIA_TYPE_H263		 (u8)34//H.263  reservation
//语音编码类型
#define	 MEDIA_TYPE_MP3	         (u8)96//mp3 mode 0-4
#define  MEDIA_TYPE_PCMA		 (u8)8//G.711 Alaw  mode 5
#define  MEDIA_TYPE_PCMU		 (u8)0//G.711 ulaw  mode 6
#define	 MEDIA_TYPE_G7231		 (u8)4//G.7231 reservation
#define	 MEDIA_TYPE_G728		 (u8)15//G.728 reservation
#define	 MEDIA_TYPE_G729		 (u8)18//G.729 reservation
*/
#endif//MEDIA_TYPE

#ifdef SOFT_CODEC
#define  CAP_AUDIO_ONLY          (u8)0x01 //音频            数据捕获
#define  CAP_CAMERA_ONLY         (u8)0x02 //      CAMERA视频数据捕获
#define  CAP_AC_BOTH             (u8)0x03 //音频＋CAMERA视频数据均进行捕获
#define  CAP_DESKTOP_ONLY        (u8)0x04 //	    DESKTOP桌面视频数据捕获
#define  CAP_AD_BOTH             (u8)0x05 //音频＋DESKTOP桌面视频数据均进行捕获
#define  CAP_FILE_AV_BOTH		 (u8)0x08 //以指定文件（目前支持kdc录像机产生的asf文件）的
                                            //音频＋视频压缩流作为捕获源

#define  CTRL_AUDIO_ONLY          (u8)0x01 //音频(包括采集到的音频以及文件的音频流)
#define  CTRL_VIDEO_ONLY          (u8)0x02 //视频(包括采集到的视频以及文件的视频流)
#define  CTRL_AV_BOTH			  (u8)0x03 //（音频＋视频）

#define  MAX_DECODER_NUM		 (u8)20   //最大解码器数目
#define  MAX_ENCODER_NUM		 (u8)20	  //最大编码器数目

#define  CAP_FRAME_BMP           (u8)0 //RGB24位图;
#define  CAP_FRAME_UYVY          (u8)1 //yuv4:2:2格式
#define  CAP_FRAME_I420          (u8)2 //yuv4:2:0格式
#define  CAP_FRAME_ERROR		 (u8)3 //不支持格式
#define  CAP_FRAME_YUY2			 (u8)4 //YUY2 (and YUNV and V422 and YUYV)

#define  SOFT_DEC_ONLY           (u8)0x01
#define  HARD_DEC_ONLY           (u8)0x02
#define  SOFTHARD_DEC_BOTH       (u8)0x03 
#define  AUDIO_MODE_ONLYDEC      (u8)0x01
#define  AUDIO_MODE_ONLYRCV      (u8)0x02
#define  AUDIO_MODE_PLAY         (u8)0x00                
#else

/*图象显示参数*/
#define MAX_DISPLAY_SATURATION   (u8)99  /*最大饱和度*/
#define MAX_DISPLAY_CONTRAST     (u8)99  /*最大对比度*/
#define MAX_DISPLAY_BRIGHTNESS   (u8)99  /*最大亮度*/

/*mpv4编码参数*/
#define FRAME_FORMAT_MERGEENC 				(u8)5      //用于图像合成编码
#define FRAME_FORMAT_FOURCIF 				(u8)4      //固定为FOURCIF编码
#define FRAME_FORMAT_TWOCIF 				(u8)3      //固定为TWOCIF编码
#define FRAME_FORMAT_CIF 					(u8)2		//固定为CIF编码
#define FRAME_FORMAT_HALFCIF 				(u8)1		//固定为HALFCIF编码
#define FRAME_FORMAT_AUTO	 				(u8)0		//不固定分辨率

/*mpv4解码参数*/
#define POST_PROCESS_LEVEL_0				(u8)0		//不作后处理
#define POST_PROCESS_LEVEL_1				(u8)1		//不加dering滤波
#define POST_PROCESS_LEVEL_2				(u8)2		//Y分量加dering滤波
#define POST_PROCESS_LEVEL_3				(u8)3		//YUV加dering滤波 

#define MOVEMONITORMATRIX					(u32)1584

#define ZOOM_LVL_ORIGN						(u32)0	//原始大小
#define ZOOM_LVL_QUATOR						(u32)1	//1/4大小
#define ZOOM_LVL_NINTH						(u32)2	//1/9大小
#define ZOOM_LVL_SIXTEENTH					(u32)3	//1/16大小
#define ZOOM_LVL_QUIT						(u32)4	//退出双流组
#define ZOOM_LVL_HIDE						(u32)5	//隐藏

typedef void ( *TDecodeVideoScaleInfo)(u16 wVideoWidth, u16 wVideoHeight);
typedef void (*REBOOTCALLBACK)(u8 byMapId);
typedef void (*CHANGECALLBACK)();
#endif

/*
#ifndef DES_ENCRYPT_MODE
#define DES_ENCRYPT_MODE         (u8)0      //DES加密模式
#define AES_ENCRYPT_MODE         (u8)1      //AES加密模式
#define ENCRYPT_KEY_SIZE         (u8)32     //密钥长度 取其中的较大值
#define DES_ENCRYPT_KEY_SIZE     (u8)8      //DES密钥长度
#define AES_ENCRYPT_KEY_SIZE_MODE_A (u8)16  //AES Mode-A 密钥长度
#define AES_ENCRYPT_KEY_SIZE_MODE_B (u8)24  //AES Mode-B 密钥长度
#define AES_ENCRYPT_KEY_SIZE_MODE_C (u8)32  //AES Mode-C 密钥长度
#endif
*/

#ifndef TNETSTRUCT
#define TNETSTRUCT

#define MAX_USERDATA_LEN    16

/*网络参数*/
typedef struct tagNetSession
{
    tagNetSession(){m_dwRTPAddr=0; m_wRTPPort=0; m_dwRTCPAddr=0; m_wRTCPPort=0; \
                    m_dwRtpUserDataLen = 0; m_dwRtcpUserDataLen = 0;}
    
    u32   m_dwRTPAddr;  //RTP地址(网络序)
    u16   m_wRTPPort;   //RTP端口(本机序)
    u32   m_dwRTCPAddr; //RTCP地址(网络序)
    u16   m_wRTCPPort;  //RTCP端口(本机序)
    u32   m_dwRtpUserDataLen;   //Rtp用户数据长度,用于在代理情况下,需要在每个udp头前添加固定头数据的情况
    u8    m_abyRtpUserData[MAX_USERDATA_LEN]; //用户数据指针
    u32   m_dwRtcpUserDataLen;   //Rtcp用户数据长度,用于在代理情况下,需要在每个udp头前添加固定头数据的情况
    u8    m_abyRtcpUserData[MAX_USERDATA_LEN]; //用户数据指针
}TNetSession;

/*本地网络参数*/
typedef struct tagLocalNetParam
{
	TNetSession  m_tLocalNet;
	u32        m_dwRtcpBackAddr;/*RTCP回发地址*/
	u16         m_wRtcpBackPort; /*RTCP回发端口*/        
}TLocalNetParam;

/*网络发送参数*/
typedef struct tagNetSndParam
{
	u8 m_byNum;
	TNetSession m_tLocalNet;
	TNetSession m_tRemoteNet[MAX_NETSND_DEST_NUM];
}TNetSndParam;

#endif

/*编码器参数*/
typedef struct Encoder
{  
	u32   m_dwCpuId;/*CPU标识*/
#ifdef SOFT_CODEC
	s32   m_nPriority;
	HWND    m_hwVideoPreview;
#else
	u32   m_dwCoreSpd;/*CPU速度*/
	u32   m_dwMemSpd; /*内存速度*/
	u32   m_dwMemSize;/*内存大小*/
	u8    m_byPort;   /*Map端口号*/
#endif
}TEncoder;

/*视频解码器参数*/
typedef struct VideoDecoder
{
	s32     m_nPriority;/*视频处理线程优先级*/
	u32     m_dwCpuId;  /*视频处理的CPU标识*/
	s32     m_nVideoPlayBufNum;     /*视频回放缓冲个数*/
	s32     m_nVideoStartPlayBufNum;/*视频开始回放缓冲个数*/
#ifdef SOFT_CODEC
	HWND    m_hwPlayVideo;
	BOOL32  m_bOverlay;//是否Overlay;
	u8      m_byMask;  //0x01软件解码,0x02硬件解码,0x03两者同时解码
    u16     m_wDecTimer;  //定时控制播放的时间间隔，单位ms；default 0，自适应，不定时。
	//硬件解码部分 
	s32 	m_nBsp15Freq;//MHZ
	s32 	m_nMemFreq;  //MHZ
	u8      m_byChipType;
	u8	    m_byBoardId;//index from 0 
	u8	    m_byBufNum; //queue length, default 5 
	TCHAR	m_achDemoFilePath[MAX_PATH];
#else
	u32   m_dwCoreSpd;/*CPU速度*/
	u32   m_dwMemSpd; /*内存速度*/
	u32   m_dwMemSize;/*内存大小*/
	u8    m_byPort;   /*Map端口号*/
#endif
	
	u32	  m_dwPostPrcLvl;  /*mpv4后处理的级别*/	
    BOOL32    m_bHDFlag; //是否高清码流
}TVideoDecoder;

/*音频解码器参数*/
typedef struct AudioDecoder
{
	s32   m_nPriority;       /*音频处理线程优先级*/
	u32   m_dwCpuId;         /*音频处理的CPU标识*/
	s32   m_nAudioPlayBufNum;/*音频播放缓冲数*/
	s32   m_nAudioStartPlayBufNum;/*音频开始播放缓冲数*/
#ifdef SOFT_CODEC
    u8    m_byAudioMode;
	HWND    m_hwPlayAudio;
#else
	u32   m_dwCoreSpd; /*CPU速度*/
	u32   m_dwMemSpd;  /*内存速度*/
	u32   m_dwMemSize; /*内存大小*/
	u8    m_byPort;    /*Map端口号*/
#endif
}TAudioDecoder;

#ifdef SOFT_CODEC

#define FRMRATE_CAL_BY_FPS  0 
#define FRMRATE_CAL_BY_SPF  1

typedef struct VideoCapParam
{
	u16	m_wVideoWidth; //视频捕获图像宽度(default:640)
	u16	m_wVideoHeight;//视频捕获图像高度(default:480)
	u8  m_byFrmRateLittleThanOne; // 帧率是否小于1 ?  
	u8	m_byFrameRate; //视频捕获帧率(default:25)   若帧率>=1，则 m_byFrameRate ＝ fps, 若帧率<1，则 m_byFrameRate ＝ spf  
	u8  m_byFOURCC;    //视频捕获帧格式(default:CAP_FRAME_BMP)
}TVideoCapParam;
#else

#endif

/*编码器状态*/
typedef struct KdvEncStatus
{
	BOOL32 				m_bVideoSignal;			/*是否有视频信号*/
	BOOL32	        m_bVideoCapStart; /*是否开始视频捕获*/
	BOOL32			m_bAudioCapStart; /*是否开始音频捕获*/
	BOOL32			m_bVideoEncStart; /*是否开始视频编码*/
	BOOL32			m_bAudioEncStart; /*是否开始音频编码*/
	BOOL32			m_bNetSndVideoStart;/*是否开始视频发送*/
	BOOL32			m_bNetSndAudioStart;/*是否开始音频发送*/
	u32           m_dwVideoBufLen;  /*视频缓冲长度*/
	u32           m_dwAudioBufLen;  /*音频缓冲长度*/ 
	TVideoEncParam	m_tVideoEncParam; /*视频编码参数*/
	u8            m_byAudioEncParam;/*音频编码模式*/
	u8			  m_byAudEncMediaType; /*音频编码媒体类型*/	
	TNetSndParam    m_tVideoSendAddr; /*视频发送地址*/
	TNetSndParam    m_tAudioSendAddr; /*音频发送地址*/
#ifdef SOFT_CODEC
	u32           m_dwSetEncParam; //设置硬件编码参数次数
	u32           m_dwVideoSend;   //已发送的视频包数
	u32           m_dwAudioSend;   //已发送的音频包数
	u32           m_dwHardFrameNum;//硬件回调帧数
	u32           m_dwPrevFrameNum;//预览帧数
#else
	u32  m_dwVideoMapHeartBeat;
	u32  m_dwAudioMapHeartBeat;  
#endif
	
}TKdvEncStatus;

typedef struct MediaSDKEnvirVar
{
    u32 m_dwProductVer;     //MediaSDK运行环境的产品的版本信息，默认为V1R2环境(T2_V1R2),可选HD
    u32 m_dwProductInfo;    //MediaSDK运行环境的特殊版本信息
    u32 m_dwOSVer;          //操作系统版本号
    u32 m_dwRservered;      //保留字段

}TMediaSDKEnvirVar;

/*编码器统计信息*/
typedef struct KdvEncStatis
{
    u8   m_byVideoFrameRate;/*视频帧率*/
    u32  m_dwVideoBitRate;  /*视频码流速度*/
    u32  m_dwAudioBitRate;  /*语音码流速度*/
    u32  m_dwVideoPackLose; /*视频丢帧数*/
    u32  m_dwVideoPackError;/*视频错帧数*/
    u32  m_dwAudioPackLose; /*语音丢帧数*/
    u32  m_dwAudioPackError;/*语音错帧数*/
	u32  m_wAvrVideoBitRate;   /*1分钟内视频编码平均码率*/
	u32	 m_wAvrAudioBitRate;   /*1分钟内语音编码平均码率*/
	u32  m_dwEncdHeight;/*码流高*/
	u32  m_dwEncdWidth;/*码流宽*/
}TKdvEncStatis;


/*解码器状态*/
typedef struct KdvDecStatus
{
	BOOL32	m_bVideoDecStart;/*是否开始视频解码*/
	BOOL32	m_bAudioDecStart;/*是否开始音频解码*/
	BOOL32	m_bRcvVideoStart;/*是否开始视频接收*/
	BOOL32	m_bRcvAudioStart;/*是否开始音频接收*/
	u32	m_dwAudioBufLen; /*音频播放缓冲长度*/
	u32	m_dwVideoBufLen; /*视频播放缓冲长度*/
	TLocalNetParam   m_tAudioRcvAddr;/*音频当地地址*/
	TLocalNetParam   m_tVideoRcvAddr;/*视频当地地址*/
#ifdef SOFT_CODEC
	BOOL32	m_bAudioOutThread;/*是否声音播放线程存活*/
	BOOL32	m_bMp4CodecThread;/*是否Mp4解码线程存活*/
	BOOL32  m_bDecoderCapability;	//显卡解码能力
	BOOL32  m_bDispCardDec;			//是否是显卡解码
#else      
	u32   m_dwVideoMapHeartBeat;//图像解码Map心跳
	u32   m_dwAudioMapHeartBeat;//语音解码Map心跳
	TVideoDisplayParam m_tVideoDisplayParam;//图像显示参数  
#endif
	u32   m_dwAudioDecType;     //音频解码类型
	u32   m_dwVideoDecType;     //视频解码类型
}TKdvDecStatus;

/*解码器统计信息   remove to codeccommon.h*/

//硬件采集芯片类型
#ifdef SOFT_CODEC
#define ENC_CHIP_TYPE_7111		1//7111
#define ENC_CHIP_TYPE_7114		2//7114 
#endif

/* 滚动横幅(从左往右滚)参数 */
typedef struct
{
	u32 dwXPos;
    u32 dwYPos;//显示区域的Y坐标
    u32 dwWidth;//显示区域的宽
    u32 dwHeight;//显示区域的高
    u32 dwBMPWight;//BMP图像宽
    u32 dwBMPHeight;//BMP图像高
    u32 dwBannerCharHeight;//字体高度－－主要用于上下滚动时，确定停顿的位置
    TBackBGDColor tBackBGDColor;//背景色 
    u32 dwBGDClarity;//具体颜色透明度
    u32 dwPicClarity;//整幅图片透明度
    u32 dwRunMode;//滚动模式：上下or左右or静止（宏定义如上）
    u32 dwRunSpeed;//滚动速度 四个等级（宏定义如上）
    u32 dwRunTimes;//滚动次数 0为无限制滚动
	u32 bRunForever;//无限制滚动
	u32 dwHaltTime;//停顿时间(秒)，上下滚动时，每滚完一行字的停顿间隔：0始终停留（与静止等同） 
}TAddBannerParam;

#define BANNER_SIZE          600000           /*横幅尺寸，上层传入的图像，象素宽高之积不得大于此限制*/

/*========================================类的声明与定义======================================*/

/*视频丢包处理策略*/
#define VID_DROP_PREFERRED_QUALITY      0    //质量优先，为等I帧策略
#define VID_DROP_PREFERRED_FAST_SPEED   1    //速度优先快档，自恢复策略中丢包后只解不播的帧数为1秒钟
#define VID_DROP_PREFERRED_MID_SPEED    2    //速度优先中档，自恢复策略中丢包后只解不播的帧数为2秒钟
#define VID_DROP_PREFERRED_SLOW_SPEED   3    //速度优先慢档，自恢复策略中丢包后只解不播的帧数为4秒钟



#ifdef SOFT_CODEC
struct TEncoderRef;
struct TEncoderStatus;
class  CSoftEncoder;
class  CDrawWnd;
typedef void (*PAUDIOCALLBACK)(u8 buAudioMode, u8 *pBuf, s32 nSize, u32 dwContext);
typedef void (*PVOICECHECK)(CHAR cMode, u32 dwContext);
typedef void (*PDRAWCALLBACK)(u32 dwParam);
typedef void (*PSTATICPICCALLBACK)(u8 *pBuf, u32 dwSize, BITMAPINFOHEADER* pHead, u32 dwParam);


typedef struct tagVoiceCheck
{
	s32			m_nBGTime;
	s32			m_nThreshold;
	PVOICECHECK	m_pCallBack;
	u32         m_dwContext;
}TVoiceCheck;

typedef void (*PGETAUDIOPOWER)(u8 byAudioPower, u32 dwContext);

#define PIC_ENCODE_JPEG   (u8)1//
#define PIC_ENCODE_BMP    (u8)2//

#define MAX_LOGO_NAME_LEN (u8)32//

//台标融入码流的相关信息设置
typedef struct tagLogoSetting
{
	u32 m_dwXPos;    //台标位置
	u32 m_dwYPos;    //台标位置
	u8  m_byClarity; //台标透明度(0-255)
	u8  m_byLogoName[MAX_LOGO_NAME_LEN+3]; //终端别名
}TLogoSetting;

#else
class CVideoEncoder;
class CAudioEncoder;
typedef void ( *TDebugCallBack )(char *pbyBuf, int dwBufLen);
#endif

/*文件编码器*/
class   CFileEncoder;

class CKdvEncoder/**/
{
public:
    CKdvEncoder();
    virtual ~CKdvEncoder();
public:
	
#ifdef SOFT_CODEC      
    //初始化软件编码器
    u16    Create( HWND previewHwnd, 
		           s32 nPriority = THREAD_PRIORITY_NORMAL,
		           u8 byBindCpuId = 0, u8 byCapType = CAP_AC_BOTH );

    u16    SetVoiceCheck(BOOL32 bCheck,const TVoiceCheck *ptVoiceCheck);/*语音功率监测*/	
	
	u16    StartCap(u8 byCapType = CTRL_AV_BOTH);//开始捕获
    u16    StopCap(u8 byCapType =  CTRL_AV_BOTH); //停止捕获
    u16    SetPreviewHwnd( HWND hPrevWnd );
    u16    SelectVideoSource(const TCHAR *szFileName = NULL);//选择视频源
    u16    SetVideoIcon( BOOL32 bEnable );//bEnable,TRUE为设置，FALSE不设。
    u16    SetVideoCapParam(const TVideoCapParam  *ptVideoCapParam );//设置图像捕获参数
    u16    SetAudioCapParam( u8 byMode );
    u16    GrabPic(CDrawWnd *pDrawWnd);//当pDrawWnd无效时，重调接口，参数为空.
    u16    SaveAsPic(LPCTSTR lpszFileName, u8 byEncodeMode);
	u16    SetAudioCallBack(PAUDIOCALLBACK pAudioCallBack, u32 dwContext);

	// 设置 获取输入音频功率 的回调
	u16    SetInputAudioPowerCB(PGETAUDIOPOWER pGetInputAudioPowerCB, u32 dwContext);

	// 设置 台标融入码流 的开关
	u16    SetAddLogoIntoEncStream(TLogoSetting *ptLogoSetting);
	
	// 设置 编码器的本地预览 的开关
	u16    ShowPreview(BOOL32 bPreview);

    u16    GetAudioMute( BOOL32& bMute );     // 是否静音 [zhuyr][4/20/2005]
    
	//设置音频的网络重传参数  [zhuyr][12/5/2005]
	//u16    SetNetFeedbackAudioParam(u16 wBufTimeSpan, BOOL32 bRepeatSnd = FALSE);
#else // vxworks
	u16    CreateEncoder(const TEncoder *ptVideoEncoder, 
							const TEncoder *ptAudioEncoder);/*初始化编码器*/

	/*视频编码器加入单视频源双码流组*/
    u16    AddVidEncToSingleCapGroup(u16  wAddtoChnNum);
	
	/*发送静态图片*/
	u16    SetSendStaticPic(BOOL32  bSendStaticPic);

	/*存取快照抓拍图片*/
	u16    SetSnapshotMaxSize(u32  dwSnapshotMaxSize);
	u16    Snapshot();
	u32    GetSnapshotSaveSize();
	u32    GetSnapshotSavePicNum();

	/*获取输入音频功率*/
	u32    GetInputAudioPower();

	u16    ScaleVideoCap(u8 byType, u8 byValue); /* 视频采集调节 */

	u16	   SetMpv4FrmPara(const TMpv4FrmParam *ptMpv4FrmPara);	/*设置mpv4帧参数*/ 
//	u16    SetVgaEncPara(TVgaEncParam *ptVgaEncPara);  /*设置VGA编码参数*/

	/* 调用录像、放像库的接口 */
	u16 BindRecorder(u8 byRecorderId);
	
		//监控终端用
	//设置双采集单码流模式
	u16   SetDblCapOneStreamParameter(u16 dwInOut, u16 dwZoom, u16 dwXPos, u16 dwYPos);
	u16	   SetEncMoveMonitor(TMoveDetectParam *ptMoveDetectParam,u32 dwAreaNum,u32 dwPort);
	void   SetEncMotionCallBack(TMotionAlarmInfo tMotionAlarmInfo,u32 dwContext){m_ptMotionAlarmInfo = tMotionAlarmInfo;m_dwMotionContext = dwContext;}	
	TMotionAlarmInfo	m_ptMotionAlarmInfo;
	u32					m_dwMotionContext; //移动侦测回调上下文	
	
#endif

	u16    GetVgaMode(u32 *pdwWidth, u32 *pdwHeight, u32 *pdwRefreshRate);	//得到vga的分辨率
    u16    GetEncoderStatus(TKdvEncStatus &tKdvEncStatus);/*获取编码器状态*/ 		
    u16    GetEncoderStatis(TKdvEncStatis &tKdvEncStatis );/*获取编码器的统计信息*/
//#ifndef WIN32
    u16    StartVideoCap(u8 byCapType = VIDCAP_CAPVIDEO);/*开始采集图像*/
    u16    StopVideoCap(); /*停止采集图像*/
//#endif
    u16    SetVideoEncParam(const TVideoEncParam *ptVideoEncParam );/*设置视频编码参数*/
    u16    GetVideoEncParam(TVideoEncParam &tVideoEncParam );       /*得到视频编码参数*/
    u16    StartVideoEnc();/*开始压缩图像*/
    u16    StopVideoEnc(); /*停止压缩图像*/
	
//#ifndef WIN32
    u16    StartAudioCap(BOOL32 bAinMic = FALSE);/*开始语音采集*/
    u16    StopAudioCap(); /*停止语音采集*/
//#endif
    u16    SetAudioEncParam(u8 byAudioEncParam, 
                            u8 byMediaType=MEDIA_TYPE_MP3,
                            u16 wAudioDuration = 30); /*设置语音压缩参数*/
    u16    GetAudioEncParam(u8 &byAudioEncParam, 
                            u8 *pbyMediaType = NULL,
                            u16 *pwAudioDuration = NULL);/*得到语音压缩参数*/

	u16    StartAudioEnc();/*开始压缩语音*/
    u16    StopAudioEnc(); /*停止压缩语音*/
	u16 SetEncoderAec(BOOL32 bSetAec);
    u16    SetAudioMute( BOOL32 bMute );     //设置是否静音
    u16    SetAudioVolume(u8 byVolume ); /*设置采集音量*/	
    u16    GetAudioVolume(u8 &byVolume );/*获取采集音量*/	
	
	/*设置图像的网络重传参数*/
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
	//前向纠错
	//调用顺序，可以只调用SetFecEnable，则默认raid5, 5+1, 切包988字节
	//SetFecMode,SetFecIntraFrame,SetFecXY应该在SetFecEnable之前调用,
	//如果在SetFecEnable之后调用，则这三个函数的任何一个都会导致前面ssrc的重置，后果是丢帧丢包
	//SetFecEnable函数的参数的true和false的每次切换都会导致丢帧丢包
	//SetFecPackLen可随时调用	

	//视频是否使用前向纠错
	u16    SetVidFecEnable(BOOL32 bEnableFec);
	//设置fec的切包长度
	u16    SetVidFecPackLen(u16 wPackLen);
	//是否帧内fec
	u16    SetVidFecIntraFrame(BOOL32 bIntraFrame);
	//设置fec算法 FEC_MODE_RAID5
	u16    SetVidFecMode(u8 byAlgorithm);
	//设置fec的x包数据包 + y包冗余包
	u16    SetVidFecXY(s32 nDataPackNum, s32 nCrcPackNum);

	//音频是否使用前向纠错
	u16    SetAudFecEnable(BOOL32 bEnableFec);
	//设置fec的切包长度
	u16    SetAudFecPackLen(u16 wPackLen);
	//是否帧内fec
	u16    SetAudFecIntraFrame(BOOL32 bIntraFrame);
	//设置fec算法 FEC_MODE_RAID5
	u16    SetAudFecMode(u8 byAlgorithm);
	//设置fec的x包数据包 + y包冗余包
	u16    SetAudFecXY(s32 nDataPackNum, s32 nCrcPackNum);
	
    u16    StartAec(u8 byType = 1);/*开始回声抵消*/
    u16    StopAec(); /*停止回声抵消*/

	u16	   StartAgcSmp(); /*开始smp增益*/
	u16	   StopAgcSmp(); /*停止smp增益*/
	u16	   StartAgcSin(); /*开始sin增益*/
	u16	   StopAgcSin(); /*停止sin增益*/
	
	u16    SetFastUpata(BOOL32 bIsNeedProtect = TRUE);     /*设置FastUpdata，MediaCtrl在75帧内编一个I帧发送*/
	
	//视频动态载荷的PT值的设定
	u16 SetAudioActivePT(u8 byLocalActivePT);
	
	//音频动态载荷的PT值的设定
	u16 SetVideoActivePT(u8 byLocalActivePT );
	
	//设置音频编码加密key字串、加密的载荷PT值 以及 解密模式 Aes or Des
	u16    SetAudEncryptKey( s8 *pszKeyBuf, u16 wKeySize, u8 byEncryptMode );
	
	//设置视频编码加密key字串、加密的载荷PT值 以及 解密模式 Aes or Des
	u16    SetVidEncryptKey( s8 *pszKeyBuf, u16 wKeySize, u8 byEncryptMode ); 
	u16    SendStaticPicture(BOOL32 bSend, TCHAR* pszFileName);
 	u16    GetFirstWaveInAudioDevice(TCHAR * pszDeviceName); // 返回值ERROR_NO_AUDIO_DEV表示无声卡
	u16    GetNextWaveInAudioDevice(s32 nDeviceID, TCHAR * pszDeviceName);  // 返回值ERROR_NO_AUDIO_DEV表示无声卡
	u16    SetWaveInAudioDevice(s32 nDeviceID); // nDeviceID可以是ERROR_NO_AUDIO_DEV, 表示无声卡。
	u16    SetReverseG7221c(BOOL32 bReverse);
	

#ifdef SOFT_CODEC
    VOID    GetLastError( u32 &dwErrorId );
#endif
	
private:
	s64    m_qwLastFastupdataTime;     //最近一次FastUpdata的时间

protected:

#ifdef SOFT_CODEC
    CSoftEncoder     *m_pcKdvInterEnc;
	s32    m_nAudioDevCount; //声卡个数
	TCHAR   m_pszAudioDevList[MAX_AUDIO_DEV_NUMBER][MAX_AUDIO_DEV_NAME_LEN];
    TEncoderRef      *m_ptRef;
    TEncoderStatus   *m_ptStatus;	
#else
	u32               m_dwMagicNum;
    CVideoEncoder    *pVideoEncoder;
    CAudioEncoder    *pAudioEncoder;
#endif
};

#ifdef SOFT_CODEC
class CKdvVideoDec;
class CKdvAudioDec;
struct TDecoderRef;
struct TDecoderStatus;
#else
class CVideoDecoder;
class CAudioDecoder;
#endif

class CKdvDecoder
{
public:
    CKdvDecoder();
    virtual ~CKdvDecoder();
	
public:
	BOOL32 SetDispCardDecode(BOOL32 bDispCardDecode);//设置显卡解码,1:尝试显卡解码,0:cpu解码
	BOOL32 GetDispCardDecodeStatus();//查询显卡解码状态,1:显卡解码,0:cpu解码
	static	BOOL32 GetDecodeCapability();//查询显卡解码能力,1:可以硬解码,0:不可以
    static u16 SetEnvironmentVar(const TMediaSDKEnvirVar *ptEnvirVar);/*设置MediaSDK运行环境参数, 必须在调用初始化解码器前初始化*/
    u16    CreateDecoder(const TVideoDecoder *ptVideoDecoder,const TAudioDecoder *ptAudioDecoder);/*初始化解码器*/	
    u16    GetDecoderStatus(TKdvDecStatus &tKdvDecStatus );/*获取解码器状态*/
    u16    GetDecoderStatis(TKdvDecStatis &tKdvDecStatis );/*获取解码器的统计信息*/	

	u16    SetVideoDecType(u8 byType); /*设置图像解码器类型, 只有在开始解码前调用才有效*/
    u16    StartVideoDec();/*开始图像解码*/
    u16    StopVideoDec(); /*停止图像解码*/

#ifndef SOFT_CODEC
	u16    StartVideoPlay(); /* 开始图像播放 */
	u16    StopVideoPlay(); /* 停止图像播放 */
#endif

	u16	   SetVideoPlayFormat(BOOL32 bFormat); /* 设置显示格式 TRUE: 16:9, FALSE: 4:3 */
	
	u16    SetAudioDecType(u8 byType,u32 dwAudioMode); /*设置声音解码器类型, 只有在开始解码前调用才有效*/
    u16    StartAudioDec();/*开始声音解码*/	
    u16    StopAudioDec(); /*停止声音解码*/	
	//设置AEC的开关
	//当编码器开启AEC时，对应的一个解码器开启必须开启AEC
	u16 SetDecoderAec(BOOL32 bSetAec);
	//设置AEC的对齐样点数
	u16 SetAecDelayBufSize(u32 u32AecDelayBufSize);
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


    u16    ClearWindow();  /*清屏*/
    u16    FreezeVideo();  /*视频冻结*/
    u16    UnFreezeVideo();/*取消视频冻结*/

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

#ifdef SOFT_CODEC
	//  [zhuyr][12/5/2005]设置音频的网络重传参数
	//u16    SetNetFeedbackAudioParam(TNetRSParam tNetRSParam, BOOL32 bRepeatSnd = FALSE);

    //移动侦测  [zhuyr][8/11/2005]
    u16    SetWatchMoveRect(LPRECT pRect, u8 bySize);
    u16    SetWatchColor(COLORREF crWatch);

    //是否根据网络抖动来调整播放速度（定时播放有效）  [zhuyr][5/24/2005]
    u16    SetNetDithering(BOOL32 bUser);

    //监控用  [zhuyr][5/18/2005]
    u16    RegStaticPicCallBack(PSTATICPICCALLBACK pCallBack, u32 dwParam);

    //设置解码作图后调用的回调函数  [zhuyr][4/25/2005]
    u16    SetDrawCallBack(PDRAWCALLBACK pFun, u32 dwParam);

    //得到静音状态  [zhuyr][4/20/2005]
    u16    GetAudioMute(BOOL32& bMute);

    //得到Wrapper的版本和编译信息 bysize最好不小于128  [zhuyr][2/23/2005]
    u16    GetWrapperVer(TCHAR *pszVerBuffer, u8 bySize);
    
    //bAuto为TRUE时，前四个参数表示四个方向上被裁的宽度是已CIF时的象素数;
    //会随解码图象大小改变。
    //bAuto为FALSe时，前四个参数设定不会随解码图象大小改变。[zhuyr][3/17/2005]
    u16    SetShowMargins(s32 nLeft, s32 nTop, s32 nRight, s32 nBottom, BOOL32 bAuto = TRUE);
	u16    SetPlayHwnd( HWND hPlayWnd, BOOL32 bSaveLastFrame = FALSE);
	u16    RedrawLastFrame();/*窗口重画*/
	
	/*当pDrawWnd无效时，重调此接口，参数为空.*/
	u16    GrabPic(CDrawWnd *pDrawWnd);//抓取图像
    u16    SaveAsPic(LPCTSTR lpszFileName, u8 byEncodeMode);//图像保存

	//设置是否只解关键帧 TRUE为设置，FALSE为取消
	u16    SetOnlyKeyFrame(BOOL32 bKeyFrame);

	u16    SetAudioCallBack(PAUDIOCALLBACK pAudioCallBack, u32 dwContext);
	// 设置 获取输出音频功率 的回调
	u16    SetOutputAudioPowerCB(PGETAUDIOPOWER pGetOutputAudioPowerCB, u32 dwContext);

//	u16    EnumWaveOutAudioDevice(OUT CStringArray &strAryAudioDevice);
//	u16    SetWaveOutAudioDevice(CString strContainText);
 	u16    GetFirstWaveOutAudioDevice(TCHAR * pszDeviceName); // 返回值ERROR_NO_AUDIO_DEV表示无声卡
	u16    GetNextWaveOutAudioDevice(s32 nDeviceID, TCHAR * pszDeviceName);  // 返回值ERROR_NO_AUDIO_DEV表示无声卡
	u16    SetWaveOutAudioDevice(s32 nDeviceID); // nDeviceID可以是ERROR_NO_AUDIO_DEV, 表示无声卡。

	u16    SetAudioDecParam(u32 dwSamplePerSecond, u32 dwChannels);//音频解码参数设置
	u16    SetReverseG7221c(BOOL32 bReverse );//设置g7221c反转

    VOID   GetLastError( u32 &dwErrorId );
#else
	u16    SetDisplayType(u8 byDisplayType);/*设置画面显示类型(PAL VGA)*/
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
	
	/*存取快照抓拍图片*/
	u16    SetSnapshotMaxSize(u32  dwSnapshotMaxSize);
	u16    Snapshot();
	u32    GetSnapshotSaveSize();
	u32    GetSnapshotSavePicNum();
	
	/*获取输出音频功率*/
	u32    GetOutputAudioPower();
	u16    SetAudioRevBuf(u32 dwStartBufs, u32 dwFasterBufs);

	//监控终端用
	u16	   SetDecMoveMonitor(TMoveDetectParam *ptMoveDetectParam,u32 dwAreaNum);
	void   SetDecMotionCallBack(TMotionAlarmInfo tMotionAlarmInfo,u32 dwContext){m_ptMotionAlarmInfo = tMotionAlarmInfo;m_dwMotionContext = dwContext;}	
	TMotionAlarmInfo	m_ptMotionAlarmInfo;
	u32					m_dwMotionContext; //移动侦测回调上下文	

#endif

protected:
#ifdef SOFT_CODEC
public:
    CKdvVideoDec       *m_pcKdvVideoDec;//保留
    CKdvAudioDec       *m_pcKdvAudioDec;//保留
    TDecoderRef	       *m_ptRef;   //保留
    TDecoderStatus     *m_ptStatus;//保留
    static TMediaSDKEnvirVar  m_tMediaSDKEnvirVar;//记录MediaSDK运行环境信息        
protected:
    u8		       m_byFlag;
	s32    m_nAudioDevCount; //声卡个数
	TCHAR   m_pszAudioDevList[MAX_AUDIO_DEV_NUMBER][MAX_AUDIO_DEV_NAME_LEN];
#else
	u32               m_dwMagicNum;
    CVideoDecoder       *pVideoDecoder;
    CAudioDecoder       *pAudioDecoder;
#endif

public:      /*文件编码器功能*/
    u16    CreateFileEncoder(char * pFielName);
	u16    DestroyFileEncoder();
	u16    StartFileEncoder();
	u16    StopFileEncoder();
	u16    PauseFileEncoder();
	u16    ContinueFileEncoder();

	u16 SendVidFrameToMap(u16 dwStreamType, u16 dwFrameSeq, u16 dwWidth,
		                   u16 dwHeight, u8 *pbyData, u16 dwDataLen);
	u16 SendAudFrameToMap(u16 dwStreamType, u16 dwFrameSeq,  u8 byAudioMode, 
		                   u8 *pbyData, u16 dwDataLen);
	
protected:
	CFileEncoder *  m_pcFileEncoder;
	BOOL32            m_bFileEcncoderInit;
	BOOL32            m_bFileEncoderStart;
	BOOL32            m_bFileEncoderPause;
	
};


#ifdef SOFT_CODEC
class CDrawWnd : public CStatic
{
	DECLARE_DYNAMIC(CDrawWnd)
public:
	CDrawWnd(BOOL32 bAlloc = FALSE/*是否预分配背景进缓冲*/);
	
	// Attributes
public:
	
    /*
    SetFullScreen
    参    数： u32 nMax    :屏幕被分割成几份
               u32 nXIndex :水平方向索引，从1开始
               u32 nYIndex :垂直方向索引，从1开始
               u32 nCombine:合并边长（1～nMax）
    */
    BOOL32 SetFullScreen();   //设置全屏,
    BOOL32 SetFullScreen(u32 nMax, u32 nXIndex, u32 nYIndex);
    BOOL32 SetFullScreen(u32 nMax, u32 nXIndex, u32 nYIndex, u32 nCombine);
    BOOL32 CancelFullScreen();//取消全屏
/*注意：
     调用这两个函数后要重新调用编解码器设置画图窗口函数
	 解码器的是SetPlayHwnd,编码器是SetPreviewHwnd.
*/
    BOOL32 SetBackBitmap(u8 *pBuf, s32 nSize, PBITMAPINFOHEADER pbmiHeader);
    BOOL32 SetFrontBmp(u8 * pFrontBuf);
    BOOL32 GetPicInfo(PBITMAPINFOHEADER pbih);
    BOOL32 SaveAsPic(LPCTSTR lpszFileName, u8 byEncodeMode);//图像保存
    BOOL32 PrintfPic();
public:
    RECT m_rtPos;
	HWND m_hParentWnd;
	BOOL32 m_bFullScreen;
	
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDrawWnd)
public:
	virtual BOOL32 Create(LPCTSTR lpszWindowName, UINT dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID=0xffff, CCreateContext* pContext = NULL);
protected:
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL
	
private:
    void *m_pPriData;
public:
	virtual ~CDrawWnd();
	
	// Generated message map functions
protected:
	//{{AFX_MSG(CDrawWnd)
	// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	
	DECLARE_MESSAGE_MAP()
};

class CAVCapabilityList;
class CAVCapCheck  
{
public:
    CAVCapCheck();
    ~CAVCapCheck();
public:
	BOOL32 CheckAudInCap();
	BOOL32 CheckVidInCap();
	BOOL32 CheckAudOutCap();
	BOOL32 CheckVidOutCap();
	BOOL32 CheckAudEncCap(u8 byMediaType);
	BOOL32 CheckVidEncCap(u8 byMediaType);
	BOOL32 CheckAudDecCap(u8 byMediaType);
	BOOL32 CheckVidDecCap(u8 byMediaType);	

private:
	CAVCapabilityList *m_ptCAVCapList;
};

#endif

/* 硬件编解码器初始化函数相关定义 */
typedef struct 
{
	u32 dwCoreSpd;            /*内核速度*/
	u32 dwMemSpd;             /*内存速度*/
	u32 dwMemSize;            /*内存大小*/
	u32 dwMaxVidEncNum;       /*最大视频编码路数*/
	u32 dwMaxVidDecNum;       /*最大视频解码路数*/
	u32 dwMaxAudEncNum;       /*最大音频编码路数*/
	u32 dwMaxAudDecNum;       /*最大音频解码路数*/
	BOOL32  bVgaOut;              /*使用VGA作为图象输出？该字段仅当字段dwMaxVidDecNum>0时有效*/
	u16  wRefreshRate;         /*VGA扫描频率(HZ)*/
}THardCodecInitParm;

/*Osd信息纪录*/
typedef struct 
{
	u8    bHasData;      //记录MapHalt前有操作
	u8 *  pbyBmp;        //以下为Osd信息
	u32   dwBmpLen; 
	u32   dwXPos; 
	u32   dwYPos; 
	u32   dwWidth; 
	u32   dwHeight; 
	u8    byBgdColor; 
	u32   dwClarity; 
}TOsdRecordInfo;

typedef struct 
{
	u8    bHasData;      //记录MapHalt前有操作
	u8 *  pbyBmp;        //以下为Osd信息
	u32   dwBmpLen; 
	u32   dwXPos; 
	u32   dwYPos; 
	u32   dwWidth; 
	u32   dwHeight; 
	TBackBGDColor    tBackBGDColor; 
	u32   dwClarity; 
}TOsdEncAndLocRecordInfo;//当台标融入码流及本地显示台标时，改变背景色为结构TBackBGDColor

typedef struct 
{
	u8    bHasData;      //记录MapHalt前有操作
	u8 *  pbyBmp;        //以下为Osd信息
	u32   dwBmpLen; 
	u32   dwChnNum;
	TAddBannerParam tAddBannerParam;
}TOsdEncAndLocBannerRecordInfo;//本地显示及编码融入横幅，终端休眠唤醒后，恢复横幅结构


//#include "EqtMap.h"

#define OSD_BUF_LEN       (u32)(800<<10)   //(u32)(1<<20)

//#define OSD_TIMER

#ifdef OSD_TIMER
#ifdef _VXWORKS_
#include "timers.h"
#include "time.h"
#endif
#endif

#define MAX_OSD_NUM          100
#define OSD_OK               0
#define OSD_ERROR            0xFFFF

#define RUNSPEED_VERYSLOW    0
#define RUNSPEED_SLOW        1
#define RUNSPEED_NORMAL      2
#define RUNSPEED_FAST        3
#define RUNSPEED_VERYFAST    4

#define RUNMODE_DOWN2UP      0
#define RUNMODE_LEFT2RIGHT   1
#define RUNMODE_STILL        2



typedef struct 
{
	BOOL32  bValid;
	u8  byType; //类型：0--静态Osd, 1--滚动Osd.
	u32 dwXPos;
	u32 dwYPos;
	u32 dwWidth;
	u32 dwHeight;
}TOsdDesc;

/*Osd参数*/
typedef struct OsdParam
{
	u16 byOsdType;         /*Osd 类型*/
	u8 *pbyHdrBuf;        /*头Buffer*/
	u32 dwHdrLen;         /*头长度*/
	u8 *pbyDataBuf;       /*数据Buffer*/
	u32 dwDataLen;        /*数据长度*/
	u32 dwXPos;           /*X坐标*/
    u32 dwYPos;           /*Y坐标*/
    u32 dwHeight;         /*高度*/
    u32 dwWidth;          /*宽度*/
	u32 dwTansparentValue; /*透明度*/
	u32 dwOtherParam1;     /*其它参数1*/
	u32 dwOtherParam2;     /*其它参数2*/
}TOsdParam;

class CHardCodec;

class COsdMap
{
public:
    COsdMap();
    virtual ~COsdMap();
	
public:
	//初始化
    u16 Initialize(u32 dwMapID);

	//显示OSD，成功返回本Osd的ID号；失败返回OSD_ERROR.
	int OsdShow(u8 *pbyBmp, u32 dwBmpLen, u32 dwXPos, u32 dwYPos, u32 dwWidth, u32 dwHeight, 
				 u8  byBgdColor, u32 dwClarity, BOOL32  bSlide=FALSE,u32 dwSlideTimes=1,
				 u32 dwSlideStride=10);

	//关闭Osd, 成功返回OSD_OK; 失败返回OSD_ERROR.
    int OsdClose(u32 dwXPos, u32 dwYPos, u32 dwWidth, u32 dwHeight, BOOL32 bSlide=FALSE);	
	
#ifdef SOFT_CODEC
	//开始台标融入码流
	int StartAddLogoInEncStream(u8 *pbyBmp, u32 dwBmpLen, u32 dwXPos, u32 dwYPos, u32 dwWidth, 
								u32 dwHeight, TBackBGDColor tBackBGDColor , u32 dwClarity, u32 dwChnNum);
	//停止台标融入码流
	int StopAddLogoInEncStream(u32 dwChnNum);
#else	
	//开始台标融入码流
	int StartAddLogoInEncStream(u8 *pbyBmp, u32 dwBmpLen, u32 dwXPos, u32 dwYPos, u32 dwWidth, 
								u32 dwHeight, TBackBGDColor tBackBGDColor , u32 dwClarity, u32 dwChnNum, u32 dwCapPrt = 2);
	//停止台标融入码流
	int StopAddLogoInEncStream(u32 dwChnNum, u32 dwCapPrt = 2);
#endif

	//开始本地图像加入台标
	int StartAddIconInLocal(u8 *pbyBmp, u32 dwBmpLen, u32 dwXPos, u32 dwYPos, u32 dwWidth, 
							u32 dwHeight, TBackBGDColor tBackBGDColor , u32 dwClarity, u32 dwChnNum);
	//停止本地图像加入台标
	int StopAddIconInLocal(u32 dwChnNum);
	
	//开始本地显示滚动字幕(从下往上滚)
	int StartRunCaption(u8 *pbyBmp, u32 dwBmpLen, u32 dwTimes, u32 dwXPos, u32 dwSpeed, u32 dwWidth, 
		                u32 dwHeight, u8  byBgdColor, u32 dwClarity, u32 dwChnNum);

	//停止本地显示滚动字幕
	int StopRunCaption(u32 dwChnNum);

	//开始横幅融入码流
	int StartAddBannerInEncStream(u8 *pbyBmp/*图像指针*/, u32 dwBmpLen/*图像大小*/, u32 dwChnNum/*通道号*/,	TAddBannerParam tAddBannerParam/*滚动横幅结构*/);

    //停止横幅融入码流
	int StopAddBannerInEncStream(u32 dwChnNum);

	//开始横幅融入本地
	int StartAddBannerInLocal(u8 *pbyBmp/*图像指针*/, u32 dwBmpLen/*图像大小*/, u32 dwChnNum/*通道号*/,	TAddBannerParam tAddBannerParam/*滚动横幅结构*/);

    //停止横幅融入本地
	int StopAddBannerInLocal(u32 dwChnNum);

	//获取OsdBuf地址
	u16 GetOsdBufAddr();
	
#ifdef OSD_TIMER

#ifdef SOFT_CODEC
	int  StartTimeIconShow(u32 dwXPos, u32 dwYPos, u32 dwWidth, u32 dwHeight, 
						  TBackBGDColor tBackBGDColor, u32 dwClarity, u32 dwChnNum);
	int  StopTimeIconShow(u32 dwChnNum);
#else

	int  StartTimeIconShow(u32 dwXPos, u32 dwYPos, u32 dwWidth, u32 dwHeight, 
	            		  TBackBGDColor tBackBGDColor, u32 dwClarity, u32 dwChnNum, u32 dwCapPrt = 2);
	int	 AlarmTimeIconShow(u32 dwChnNum,u32 dwCapPrt,u32 dwAlarmTime);
	int  StopTimeIconShow(u32 dwChnNum, u32 dwCapPrt = 2);
#endif	
	int  CheckTImeIconProc();
	//时间台标时钟处理函数
#ifdef _VXWORKS_
    friend void TimeIconTimerProcess(timer_t timerid, int Param);   
#endif

#endif

	/*Map重启后恢复Osd信息*/
	void   RecoverOsdShow();
	void   RecoverAddLogo();
	void   RecoverAddIcon();
	void   RecoverAddBanner();
#ifdef OSD_TIMER
	void   RecoverTimeIcon();
#endif
	
private:
    int  CheckOsdBuf(u32 dwBufNo);

	TOsdRecordInfo  m_tOsdShowInfo;       //显示静态OSD信息
	TOsdEncAndLocRecordInfo  m_tAddLogoInfo;       //台标融入码流信息
	TOsdEncAndLocRecordInfo  m_tAddIconInfo;       //本地图像加入台标信息
	TOsdEncAndLocRecordInfo  m_tTimeIconInfo;      //时间台标信息
	TOsdEncAndLocBannerRecordInfo m_tBannerLocInfo;   //本地横幅
	TOsdEncAndLocBannerRecordInfo m_tBannerEncInfo;   //横幅融入码流信息
private:
	TOsdDesc m_atOsdDesc[MAX_OSD_NUM];
    volatile u32 m_dwMapBuffer[2];  //Osd Buffer
    u8 m_byCurBufNum;      //当前Buffer编号
	int nWaitTime;           //等待次数
	
#ifdef OSD_TIMER
	//时钟ID
#ifdef _VXWORKS_
	timer_t m_timerId;      
#endif

#endif

	u32   m_dwAddLogoChn;
	u32   m_dwDrawOsdFailTimes; 
	CHardCodec *m_pcCodecMap;	
};


#ifdef HARD_CODEC


/* 硬件编解码器初始化函数, 必须在使用硬件编解码库中其他函数前调用. 成功返回CODEC_NO_ERROR, 失败参见错误码 */
extern "C" int HardCodecInit(int dwCpuId, THardCodecInitParm *ptCodecInitParam, BOOL32  bKdv8010 = TRUE);

/* 硬件编解码器退出函数，调用之后，用户再不能对相应编解码器进行任何操作 */
extern "C" void HardCodecQuit(int dwCpuId);

/* 清除显示缓冲函数，用户在调用HardCodecQuit（）函数之前先调用本函数以解决reboot画面错乱问题 */
extern "C" void ClearPlayBuf();


API void codecwrapperver(u32 dwMapId);

/*   设置发送色带测试或显示色带测试
     dwChnNum -- 发送通道号，=255时表示所有编码通道都发送色带测试图片
     dwTestPicType -- 色带测试图片号，0--分辨率测试，1--色度测试，2--灰度测试，255--取消设置*/
API BOOL32  SetSendTestPic(u32 dwMapId, u32 dwChnNum, u32 dwTestPicType);
API BOOL32  SetPlayTestPic(u32 dwMapId, u32 dwTestPicType);

/*删除所有快照存储的文件*/
API BOOL32  ClearSnapShotFile();

/*设置Map待机*/
API BOOL32  SetMapSleep(u32 dwMapId);
/*设置Map苏醒*/
API BOOL32  SetMapWakeup(u32 dwMapId);

/*设置Map重启的回调*/
API void SetMapRebootCallBack(REBOOTCALLBACK pRebootProc);
/*设置PAL，NTSC制式改变消息通知*/
API void SetNtscPalChangeCallBack(CHANGECALLBACK pChangeProc);
API int McSwitch(BOOL32 bEnterMc);	//切换mc状态
API int SetMcPicMergeParam(u8 abyMergeParam[]);	//设置mc状态时的图像合成参数
API int SetMcAudDecType(BOOL bMixer, u8 byMcAudDecMask);	//设置mc状态时音频解码路数
API int SetVideoSendPacketLen(s32 dwMaxLen);

#ifndef TOSFUNC
#define TOSFUNC
//设置媒体TOS值 nType类型0:全部 1:音频 2:视频 3: 数据
API	int SetMediaTOS(int nTOS, int nType);
API	int GetMediaTOS(int nType);
#endif

#else

//设置媒体TOS值 nType类型0:全部 1:音频 2:视频 3: 数据
API	int SetMediaTOS(int nTOS, int nType);
API	int GetMediaTOS(int nType);

API void SetResendDrop(u32 dwInterval, u32 dwRatio);

#endif /* HARD_CODEC */

#endif /*__CODECLIB_H*/
















