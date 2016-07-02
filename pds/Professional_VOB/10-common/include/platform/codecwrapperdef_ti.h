/*****************************************************************************
  模块名      : Codecwrapper.a
  文件名      : codecwrapperdef_ti.h
  相关文件    : codecwrapper_ti.h
  文件实现功能: 
  作者        : 朱允荣
  版本        : V1.0  Copyright(C) 1997-2006 KDC, All rights reserved.
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2006/12/13  1.0         朱允荣      创建
******************************************************************************/
#ifndef _CODECWRAPPER_DEF_TI_H_
#define _CODECWRAPPER_DEF_TI_H_

//CPU标识
#define CWTI_CPUMASK_MAIN   0x00000001
#define CWTI_CPUMASK_SUB    0x00000002
#define CWTI_CPUMASK_ALL    0x00000003

//错误码
#define ERROR_CODE_BASE             30000             
#define CODEC_NO_ERROR              0//无错

#include "kdvtype.h"
#include "osp.h"
#include "kdvmedianet.h"

enum enCodecErr
{
	Codec_Success = CODEC_NO_ERROR,

	Codec_Error_Base = ERROR_CODE_BASE,
	Codec_Error_Param,
	Codec_Error_CPUMask,
	Codec_Error_Video_NO_Create,
	Codec_Error_Audio_NO_Create,
	Codec_Error_Audio_CapInstance,
	Codec_Error_Audio_PlyInstance,
	Codec_Error_Video_CapInstance,
	Codec_Error_Video_PlyInstance,
	Codec_Error_Audio_CapInit,
	Codec_Error_Audio_PlyInit,
	Codec_Error_Video_CapInit,
	Codec_Error_Video_PlyInit,
	Codec_Error_GetCMem,
	Codec_Error_GetMem,
	Codec_Error_Not_Start_Send
};

#define CODECMAXCHANNELNUM         10
#define MAX_VIDENC_CHANNEL         CODECMAXCHANNELNUM
#define MAX_VIDDEC_CHANNEL         CODECMAXCHANNELNUM
#define MAX_AUDENC_CHANNEL         CODECMAXCHANNELNUM
#define MAX_AUDDEC_CHANNEL         CODECMAXCHANNELNUM

//采集类型
#define  VIDCAP_CAPVIDEO			(u8)1	//采集video
#define  VIDCAP_CAPVGA				(u8)2	//采集vga

/*画面显示方式*/
#define DISPLAY_TYPE_PAL      (u8)0x1 /*PAL video*/
#define DISPLAY_TYPE_NTSC     (u8)0x2 /*NTSC video*/
#define DISPLAY_TYPE_VGA      (u8)0x3 /*VGA*/

/*图象制式*/
#define	 VID_ENC_USEPAL		 (u8)0x21     /*PAL制式*/
#define	 VID_ENC_USENTSC	 (u8)0x22     /*NTSC制式*/

/*画中画显示方式*/
#define	PIP_LOCAL_IN_REMOTE	     (u8)0x0 /*本地为小画面，远端为大画面*/
#define	PIP_REMOTE_IN_LOCAL	     (u8)0x1 /*远端为小画面，本地为大画面*/
#define	PIP_LOCAL_ONLY		     (u8)0x2 /*仅显示本地画面*/
#define	PIP_REMOTE_ONLY		     (u8)0x3 /*仅显示远端画面*/
#define	PIP_LITTLE_PIC	         (u8)0x4 /*显示开机小画面*/
#define	PIP_LEFT_AND_RIGHT       (u8)0x5 /*左右两画面，一解码，一本地*/
#define	PIP_THREE_PIC            (u8)0x6 /*三画面*/
#define	PIP_SECOND_IN_MAIN	     (u8)0x7 /*本地为小画面，远端为大画面*/
#define	PIP_MAIN_IN_SECOND	     (u8)0x8 /*远端为小画面，本地为大画面*/
#define	PIP_LEFT_AND_RIGHT_DB    (u8)0x9 /*左右两画面 双流时，两解码器*/
//MC 解码器编号
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

//画面合成类型
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


//Audio mode
enum
{
	AUDIO_MODE_PCMA = 5,
	AUDIO_MODE_PCMU,
	AUDIO_MODE_G723_6,
	AUDIO_MODE_G723_5,
	AUDIO_MODE_G728,
	AUDIO_MODE_G722,
	AUDIO_MODE_G729,
	AUDIO_MODE_G7221,
	AUDIO_MODE_ADPCM 
};


/* 丢包处理策略*/
enum
{
	VID_DROP_PREFERRED_QUALITY   =   0, 
	VID_DROP_PREFERRED_FAST_SPEED,   
	VID_DROP_PREFERRED_MID_SPEED,
	VID_DROP_PREFERRED_SLOW_SPEED
};

const u32 con_horizontal = 0;
const u32 con_vertical   = 1;

/*图象参数*/
enum
{
	VIDCAP_SCALE_BRIGHTNESS = 0 ,
	VIDCAP_SCALE_CONTRAST  ,
	VIDCAP_SCALE_SATURATION
};

enum
{
	CPUTYPE_MASTER_ARM = 0,
	CPUTYPE_MASTER_DSP = 1,
	CPUTYPE_SLAVE_ARM  = 2,
	CPUTYPE_SLAVE_DSP  = 3
};

//接口结构
typedef struct tagCodecInit
{
	u32 m_dwCpuMask;
	u32 m_dwVlynqChannel;  //只能使用在Slave初始化中配置的ID好（0～Max－1）
	s32 m_bUserMC;         //是否使用MC，只对视频编码器有效

	BOOL32 m_bMasterUseAec;       //音频创建时指定主设备是否使用Aec
}
TEncoder, TDecoder;

typedef struct tagSlaveInit
{
	u32  m_wVidEncNum;    //从机视频编码器个数
	u32  m_wVidDecNum;    //从机视频解码器个数
	u32  m_wAudEncNum;    //从机音频编码器个数
	u32  m_wAudDecNum;    //从机音频解码器个数
	BOOL32 m_bUseOsd;     //从机上是否显示OSD
	BOOL32 m_bUseMC;      //是否有终端内嵌MC功能
	BOOL32 m_bHasFXO;     //是否使用FXO
}
TSlaveInit;

typedef struct tagTMasterInit
{
	u32  m_wVidEncNum;    //主机视频编码器个数
	u32  m_wVidDecNum;    //主机视频解码器个数
	BOOL m_bD1Splite;

	BOOL m_bAudMaster;   //主机上存在音频编解码
}TMasterInit,*PTMasterInit;

typedef struct
{
	u32 dwFrmType;				//0:不固定类型, 1: half cif, 2: 1 cif, 3: 2 cif,  4: 4 cif,  5: 用于图像合成编码
	u32 dwFrameRate;				//帧率
	u32 dwIFrmInterval;				//I帧间隔
	u32 dwBitRate;					//码率
	BOOL32 bFrmRateLittleThanOne;	//编码帧率小于1 ?
	BOOL32 bFramRateCanSet;			//编码帧率是否由外部设定? 1:是，0：否
	BOOL32 bAutoDelFrame;			//编码器是否自动丢帧1：是，0：否				
}TMpv4FrmParam;

//监控终端用运动侦测
//侦测矩形区域定义
typedef struct tagDetectRect
{
	u32  dwXPos;//侦测区域顶点横坐标
	u32  dwYPos;//侦测区域顶点纵坐标
	u32  dwWidth;//侦测区域宽度
	u32  dwHeigth;//侦测区域高度
}TDetectRect;

//移动侦测参数结构
typedef struct tagMoveDetectParam
{
	BOOL32		bDetectEnable;//侦测使能
	TDetectRect tDetectRect;  // 侦测区域
	u8			byAlarmRate;//侦测区域告警产生百分比（大于此临界值告警,0-100）
	u8			byResumeRate;//产生告警后运动范围百分比小于该值则恢复告警(该值小于byAlarmRate,0-100)
}TMoveDetectParam;

typedef struct tagMosaic
{
    BOOL32  bEnable;        //是否使用马赛克
	u32     dwXPos;//马赛克区域 4cif计算顶点横坐标
	u32     dwYPos;//顶点纵坐标
	u32     dwWidth;//宽度
	u32     dwHeigth;//高度
    u32     dwGranularity;  //马赛克颗粒
} TMosaic;

//移动回调参数结构
typedef struct tagMoveCBParam
{
	BOOL32		bMoveAlarm;//告警或恢复
	//TDetectRect tDetectRect;//侦测区域
    u32			dwAlarmRectNum;
}TMoveCBParam;

/*视频编码参数*/
typedef struct VideoEncParam
{
	u8    m_byEncType;   /*图像编码类型*/
    u8    m_byRcMode;    /*图像压缩码率控制参数*/    
	u8    m_byMaxQuant;  /*最大量化参数(1-31)*/
    u8    m_byMinQuant;  /*最小量化参数(1-31)*/

    u16   m_byMaxKeyFrameInterval;/*I帧间最大P帧数目*/
    u16   m_wBitRate;    /*编码比特率(Kbps)*/

	u32   m_dwSndNetBand;/*网络发送带宽(单位:Kbps,1K=1024)*/   

	u8    m_byPalNtsc;    /*图像制式(PAL或NTSC)*/	
	u8    m_byCapPort;    /*采集端口号: 1~7有效。在KDV8010上，S端子端口号为7；C端子端口号缺省为2*/
	u8	  m_byFrameRate;  /*帧率(default:25)*/	
	u8    m_byImageQulity;/*图象压缩质量,0:速度优先;1:一般;2:质量优先*/

	u16   m_wVideoWidth;  /*图像宽度(default:352)*/
	u16	  m_wVideoHeight; /*图像高度(default:288)*/

	u8  m_byFrameFmt; // 分辨率 0:不固定类型, 1: half cif, 2: 1 cif, 3: 2 cif,  4: 4 cif,  5: 用于图像合成编码  
	u8	m_byFrmRateCanSet; //帧率是否由外界设定 ?
	u8  m_byFrmRateLittleThanOne; // 帧率是否小于1 ?
	u8  m_byReserved1;  /*保留*/
//	u8  m_byAutoDelFrm;			//编码器是否自动丢帧 ?不起作用，质量只由Quality决定
}TVideoEncParam;

/*编码器状态*/
typedef struct KdvEncStatus
{
	BOOL32 			m_bVideoSignal;			/*是否有视频信号*/
	BOOL32	        m_bVideoCapStart; /*是否开始视频捕获*/
	BOOL32			m_bAudioCapStart; /*是否开始音频捕获*/
	BOOL32			m_bVideoEncStart; /*是否开始视频编码*/
	BOOL32			m_bAudioEncStart; /*是否开始音频编码*/
	BOOL32			m_bNetSndVideoStart;/*是否开始视频发送*/
	BOOL32			m_bNetSndAudioStart;/*是否开始音频发送*/
	u32				m_dwVideoBufLen;  /*视频缓冲长度*/
	u32				m_dwAudioBufLen;  /*音频缓冲长度*/ 
	TVideoEncParam	m_tVideoEncParam; /*视频编码参数*/
	u8				m_byAudioEncParam;/*音频编码模式*/
	u8				m_byAudEncMediaType; /*音频编码媒体类型*/	
	TNetSndParam    m_tVideoSendAddr; /*视频发送地址*/
	TNetSndParam    m_tAudioSendAddr; /*音频发送地址*/
	u32				m_dwVideoMapHeartBeat;
	u32				m_dwAudioMapHeartBeat;  
	
}TKdvEncStatus;

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
}TKdvEncStatis;

typedef struct VideoDisplayParam
{
	u8    m_byPIPMode;   /*画中画显示方式*/
	u8    m_bySaturation;/*显示饱和度*/
	u8    m_byContrast;  /*显示对比度*/
	u8    m_byBrightness;/*显示亮度*/
	u16   m_dwPIPXPos;   /*画中画X坐标*/
	u16   m_dwPIPYPos;   /*画中画Y坐标*/
	u16   m_dwPIPWidth;  /*画中画宽度*/
	u16   m_dwPIPHeight; /*画中画高度*/    
	u32   m_dwPlayMode;   //显示模式，1-PAL显示，2-NTSC显示，3-VGA显示
}TVideoDisplayParam;


/*解码器状态*/
typedef struct KdvDecStatus
{
	BOOL32	m_bVideoDecStart;/*是否开始视频解码*/
	BOOL32	m_bAudioDecStart;/*是否开始音频解码*/
	BOOL32	m_bRcvVideoStart;/*是否开始视频接收*/
	BOOL32	m_bRcvAudioStart;/*是否开始音频接收*/
	u32		m_dwAudioBufLen; /*音频播放缓冲长度*/
	u32		m_dwVideoBufLen; /*视频播放缓冲长度*/
	TLocalNetParam   m_tAudioRcvAddr;/*音频当地地址*/
	TLocalNetParam   m_tVideoRcvAddr;/*视频当地地址*/
    
	u32   m_dwVideoMapHeartBeat;//图像解码Map心跳
	u32   m_dwAudioMapHeartBeat;//语音解码Map心跳
	TVideoDisplayParam m_tVideoDisplayParam;//图像显示参数  
	
	u32   m_dwAudioDecType;     //音频解码类型
	u32   m_dwVideoDecType;     //视频解码类型
}TKdvDecStatus;

/*解码器统计信息*/
typedef struct KdvDecStatis
{
	u8	  m_byVideoFrameRate;/*视频解码帧率*/
	u16	  m_wVideoBitRate;   /*视频解码码率*/
	u16	  m_wAudioBitRate;   /*语音解码码率*/
	u32   m_dwVideoRecvFrame;/*收到的视频帧数*/
	u32   m_dwAudioRecvFrame;/*收到的语音帧数*/
	u32   m_dwVideoLoseFrame;/*丢失的视频帧数*/
	u32   m_dwAudioLoseFrame;/*丢失的语音帧数*/
    u16   m_wVideoLoseRatio;//视频丢失率,单位是% 	
	u16   m_wAudioLoseRatio;//音频丢失率,单位是% 	
	u32   m_dwPackError;/*乱帧数*/ 
	u32   m_dwIndexLose;/*序号丢帧*/
	u32   m_dwSizeLose; /*大小丢帧*/
	u32   m_dwFullLose; /*满丢帧*/	
	u32   m_wAvrVideoBitRate;   /*1分钟内视频解码平均码率*/
	u32	  m_wAvrAudioBitRate;   /*1分钟内语音解码平均码率*/
	BOOL32 m_bVidCompellingIFrm;  /*视频强制请求I帧*/								  
	u32   m_dwDecdWidth;	/*码流的宽*/
	u32   m_dwDecdHeight;   /*码流的高*/
}TKdvDecStatis;

/* 图像解码参数结构 */
typedef struct
{
    u32 dwStartRecvBufs;         // 开始接收buffer数
    u32 dwFastRecvBufs;         // 快收buffer数
	u32 dwPostPrcLvl;			// mpv4后处理的级别		
}TVideoDecParam;


/*告警信息*/
typedef struct AlarmInfo
{
	BOOL32 m_bMap0Halt;
	BOOL32 m_bMap1Halt;
	BOOL32 m_bMap2Halt;
	u32    m_dwMap0HaltTimes;         //Map0重启次数
	u32    m_dwMap1HaltTimes;         //Map1重启次数
	u32    m_dwMap2HaltTimes;         //Map2重启次数
	BOOL32 m_bNoVideoInput;
	BOOL32 m_bRecVideoNoBitStream;
	BOOL32 m_bRecAudioNoBitStream;
    BOOL32 m_bSendVideoLoseFrame;
    BOOL32 m_bSendAudioLoseFrame;
}TAlarmInfo;

typedef struct tagNetRSParam
{
    u16  m_wFirstTimeSpan;	  //第一个重传检测点
	u16  m_wSecondTimeSpan;  //第二个重传检测点
	u16  m_wThirdTimeSpan;   //第三个重传检测点
	u16  m_wRejectTimeSpan;  //过期丢弃的时间跨度
} TNetRSParam;

typedef struct BackBGDColor
{
	u8 RColor;    //R背景
	u8 GColor;    //G背景
	u8 BColor;    //B背景	
}TBackBGDColor;

typedef TBackBGDColor TBGColor;

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
	u32 dwHaltTime;//停顿时间(秒)，上下滚动时，每滚完一行字的停顿间隔：0始终停留（与静止等同） 
}TAddBannerParam;

typedef TAddBannerParam TDynimicPicInfo;

#define SPEED_FAST 8
#define SPEED_HIGH 6
#define SPEED_NORM 4
#define SPEED_LOW  2

#define UPDWON     1
#define LEFTRIGHT  2
#define STATIC     3

const u32 dwUpDownStep    = 16;
const u32 dwLeftRightStep = 16;


const u8  byOsdProcess_Priority  = 5;
const u32 dwOsdProcess_Stacksize = 0;
const u16  wOsdProcess_Flag      = 0;

const u8  byAllTransparent = 0x00; 
const u8  byNoneTransparent = 0xFF;
const u32 dwOsdDelayTimes = 100;

const u32 dwOsdBackgroudSize = 720 * 288 * 2;

typedef void ( *TMotionAlarmInfo)(u32 dwPort,TMoveCBParam *ptMoveCBParam,u32 dwAreaNum,u32 dwContext);
typedef void ( *TDecodeVideoScaleInfo)(u16 wVideoWidth, u16 wVideoHeight);
typedef void ( *TDebugCallBack )(char *pbyBuf, int dwBufLen);
typedef void ( *CHANGECALLBACK)();
typedef void ( *CpuResetNotify)(s32 nCpuType);  //CPUTYPE_MASTER_ARM 等
typedef void ( *MsgFromSlave)(u32 dwID, void* pBuf, u32 dwBufSize); 
#endif
