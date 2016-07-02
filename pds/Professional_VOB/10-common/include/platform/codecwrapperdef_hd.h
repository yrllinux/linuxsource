/*****************************************************************************
  模块名      : Codecwrapper_HD.a
  文件名      : codecwrapperdef_hd.h
  相关文件    : codecwrapper_hd.h
  文件实现功能: 
  作者        : 朱允荣
  版本        : V4.0  Copyright(C) 2007-2008 KDC, All rights reserved.
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2007/11/29  4.0         朱允荣      创建
******************************************************************************/
#ifndef _CODECWRAPPER_DEF_HD_H_
#define _CODECWRAPPER_DEF_HD_H_

#ifdef _MEDIACTRL_DM816x_

#include "dm816x/codecwrapperdef_hd.h"

#else
//CPU标识
#define CWTI_CPUMASK_MAIN   0x00000001
#define CWTI_CPUMASK_SUB    0x00000002
#define CWTI_CPUMASK_ALL    0x00000003

//错误码
#define ERROR_CODE_BASE             30000             
#define CODEC_NO_ERROR              0//无错

#include "kdvtype.h"
#include "osp.h"

//初始化时指定开发板类型
enum enBoardType
{
	en_H600_Board = 1,
	en_H700_Board,
	en_H900_Board,
	en_HDU2_Board,
	en_MPU2_Board,
	en_APU2_Board,
	en_HDU2_Board_S,
	en_Invalid_Board,
};

enum enCodecErr
{
	Codec_Success = CODEC_NO_ERROR,

	Codec_Error_Base = ERROR_CODE_BASE,
	Codec_Error_Param,
	Codec_Error_CPUID,
	Codec_Error_Codec_NO_Create,
	Codec_Error_Audio_CapInstance,
	Codec_Error_Audio_PlyInstance,
	Codec_Error_CreateThread,
	Codec_Error_FastUpdateTooFast,
	Codec_Error_WriteToDSP,
	Codec_Error_CreateHPIChannel,
	Codec_Error_GetMem,
	Codec_Error_GetBMP
};

#define CODECMAXCHANNELNUM         4
#define MAX_VIDENC_CHANNEL         CODECMAXCHANNELNUM
#define MAX_VIDDEC_CHANNEL         CODECMAXCHANNELNUM
#define MAX_AUDENC_CHANNEL         CODECMAXCHANNELNUM
#define MAX_AUDDEC_CHANNEL         CODECMAXCHANNELNUM

#define MAX_NUMLOGO_NUM        4
#define MAX_NOMALLOGO_NUM      16
#define MAX_BMP_NUM            32
#define MIN_BMP_LOGO_ID        1

//Audio mode
enum
{
	AUDIO_MODE_MP3 = 4,
	AUDIO_MODE_PCMA = 5,
	AUDIO_MODE_PCMU,
	AUDIO_MODE_G723_6,
	AUDIO_MODE_G723_5,
	AUDIO_MODE_G728,
	AUDIO_MODE_G722,
	AUDIO_MODE_G729,
	AUDIO_MODE_G719,
	AUDIO_MODE_G7221,
	AUDIO_MODE_ADPCM,
    AUDIO_MODE_AACLC_32,     //32K 双声道
    AUDIO_MODE_AACLC_32_M,   //32K 单声道
    AUDIO_MODE_AACLC_48,     //48K 双声道
    AUDIO_MODE_AACLC_48_M,   //48K 单声道
    AUDIO_MODE_AACLD_32,
    AUDIO_MODE_AACLD_32_M,
    AUDIO_MODE_AACLD_48,
    AUDIO_MODE_AACLD_48_M
};

//音频采样率索引
typedef enum
{
    SAMPLE_RATE96000 = 0, //采样率为96kHz的索引是0
    SAMPLE_RATE88200,     //采样率为88.2kHz的索引是1
    SAMPLE_RATE64000,     //采样率为64kHz的索引是2
    SAMPLE_RATE48000,     //采样率为48kHz的索引是3
    SAMPLE_RATE44100,     //采样率为44.1kHz的索引是4
    SAMPLE_RATE32000,     //采样率为32kHz的索引是5
    SAMPLE_RATE24000,     //采样率为24kHz的索引是6  
    SAMPLE_RATE22050,     //采样率为22.05kHz的索引是7
    SAMPLE_RATE16000,     //采样率为16kHz的索引是8
    SAMPLE_RATE12000,     //采样率为12kHz的索引是9
    SAMPLE_RATE11025,     //采样率为11.025kHz的索引是10 
    SAMPLE_RATE8000       //采样率为8kHz的索引是11
};

/* 丢包处理策略*/
enum
{
	VID_DROP_PREFERRED_QUALITY   =   0, 
	VID_DROP_PREFERRED_FAST_SPEED,   
	VID_DROP_PREFERRED_MID_SPEED,
	VID_DROP_PREFERRED_SLOW_SPEED
};

/*编码器工作具体位置*/
enum
{
    VID_CODECTYPE_FPGA = 0,
    VID_CODECTYPE_DSP,
    VID_CODECTYPE_UNKNOW
};

/* 视频输入输出接口宏定义 */
#define VIDIO_HDMI0                 0x00000001
#define VIDIO_HDMO1					0x00000002
#define VIDIO_VGA0                  0x00000010
#define VIDIO_YPbPr0                0x00000100
#define VIDIO_YPbPr1                0x00000200  /* 在KDV7180上该接口和VIDIN_VGA0冲突，只能2选1 */
#define VIDIO_SDI0                  0x00001000
#define VIDIO_C0                    0x00010000


/*图象参数*/
enum
{
    VIDCAP_SCALE_BRIGHTNESS = 0, /* 设置亮度，  C | YPbPr */
    VIDCAP_SCALE_CONTRAST,       /* 设置对比度，C | YPbPr */
    VIDCAP_SCALE_HUE,            /* 设置色度，  C */
    VIDCAP_SCALE_SATURATION      /* 设置饱和度，C | YPbPr */
};

/*SD信号固定输出格式*/
enum
{
	SDOUTTYPE_576I = 0,     //固定按576i输出
	SDOUTTYPE_720P,         //固定按720p输出
	SDOUTTYPE_1080I         //固定按1080i输出
};

/* 重启编解码设备ID*/
enum
{
	CODECDEV_VIDENC = 0,     //视频编码设备
	CODECDEV_VIDDEC,         //视频解码设备
	CODECDEV_AUDDEV          //音频编解码设备
};

/* VGA、1080I、1080P输出频率选择*/
#define  VIDPLYFREQ_AUTO             0     //默认自动调整
#define  VIDPLYFREQ_24FPS            24    //用于1080P
#define  VIDPLYFREQ_25FPS            25    //用于1080P
#define  VIDPLYFREQ_30FPS            30    //用于1080P
#define  VIDPLYFREQ_50FPS            50    //用于1080I
#define  VIDPLYFREQ_60FPS            60	   //用于1080I、VGA
#define  VIDPLYFREQ_75FPS            75	   //用于VGA
#define  VIDPLYFREQ_1080POUT1080I    0xFF  //用于1080P按1080I输出

/*适配器GroupID*/
/*注:A、B模式通道不可交叉创建*/
enum
{
	ADAPTER_MODEA_NOR = 0,     //A模式的普通适配通道
	ADAPTER_MODEA_VGA,         //A模式的VGA适配通道
	ADAPTER_MODEB_H264ToH263,		   //B模式的H264ToH263通道
	ADAPTER_MODEB_H263ToH264           //B模式的H263ToH264通道
};

//设备初始化模式
enum enInitMode
{
    INITMODE_All_VMP = 0,           //高清单VMP模式，相当于调用 HardMPUInitDev(1,2,4)
    INITMODE_HALF_VMP,              //高清双VMP模式，相当于调用 HardMPUInitDev(2,2,4)
    INITMODE_EVMP,                  //标清单VMP模式，相当于调用 HardMPUInitDev(1,2,4)
    INITMODE_HDBAS,                 //高清适配模式，相当于调用  HardMPUInitDev(2,2,4)
	INITMODE_HD2BAS,				//兼容高清2终端1080p60高清适配模式，同INITMODE_HDBAS
    INITMODE_EBAS,                  //标清适配模式，相当于调用  HardMPUInitDev(2,0,4)
    INITMODE_HDU,                   //高清电视墙模式，相当于调用  HardMPUInitDev(0,1,1)
	INITMODE_HDU_D,					//高清电视墙模式，HDU_D板
    INITMODE_NUM
};

//HDU工作模式，hdu-1 or hdu-2
enum enHduWorkMode
{
    HDU_WORKMODE_INVALID,
    HDU_WORKMODE_1_CHNL,            //只有一路音/视频输出
    HDU_WORKMODE_2_CHNL                 //两路音/视频输出，常见模式
};

/*无码流时背景*/
enum enNoStreamBack
{
	NoStrm_PlyBlk = 0,		//黑色背景(默认)
	NoStrm_PlyLst = 1,		//播放上一帧
	NoStrm_PlyBmp_dft,		//默认图片
	NoStrm_PlyBmp_usr,		//用户定义图片
	NoStrm_Total
};
#define FILE_BMP_DFT_VGA		"/usr/etc/config/dft_vga.bmp"
#define FILE_BMP_DFT_16To9		"/usr/etc/config/dft_16to9.bmp"
#define FILE_BMP_USR_VGA		"/usr/etc/config/usr_vga.bmp"
#define FILE_BMP_USR_16To9		"/usr/etc/config/usr_16to9.bmp"

/* 音频输入输出接口宏定义 */
#define AUDIO_HDMI0                 0x00000001
#define AUDIO_HDMI1					0x00000002
#define AUDIO_LINE0					0x00000010
#define AUDIO_LINE1					0x00000020
#define AUDIO_C0                    0x00010000

/* 短消息及横幅滚动方式宏定义 */
//滚动方式
#define RUNMODE_STATIC     0    //短消息不支持
#define RUNMODE_LEFTRIGHT  1    
#define RUNMODE_UPDWON     2    
//滚动速度
#define RUNSPEED_FAST      4
#define RUNSPEED_HIGH      3
#define RUNSPEED_NORM      2
#define SPEED_LOW          1

/*组合主、辅视频输出VP*/
#define MAKEVPID(nMain,nSub)          (u32)( ((u32)nMain&0x000000FF) | (((u32)(nSub+1)&0x000000FF)<<8) )

//宏块编码质量设置
#define VIDENC_MBSIZE_ALL         (u32)0xFF0FFFFF
#define VIDENC_MBSIZE_P8x8UP      (u32)0xFF0FE0FF
#define VIDENC_MBSIZE_DEFAULT     0  //目前为0xFF0FFFFF

#define VIDENC_MBSIZE_ENABLE      VIDENC_MBSIZE_ALL
#define VIDENC_MBSIZE_DISABLE     VIDENC_MBSIZE_P8x8UP
//////////////////////////////////////////////////////////////////////////
//adpter 宏定义
#define MAX_VIDEO_ADAPT_CHANNUM       1
#define MAX_AUDIO_ADAPT_CHANNUM       3
#define MAX_ADAPT_CHNNUM              (MAX_VIDEO_ADAPT_CHANNUM+MAX_AUDIO_ADAPT_CHANNUM)

#define MAX_TRANS_VIDEO_ADAPT_CHANNUM       20
#define MAX_TRANS_AUDIO_ADAPT_CHANNUM       20

enum {
	ADAPTER_CODECID_HD1080,
	ADAPTER_CODECID_HD720P,
	ADAPTER_CODECID_SD4CIF,
	ADAPTER_CODECID_SDCIF,
	ADAPTER_CODECID_OTHER1,
	ADAPTER_CODECID_OTHER2
};


#define ADAPTER_CODECID_ENCHD               0
#define ADAPTER_CODECID_ENCSD               1
#define ADAPTER_CODECID_DEC                 10
//////////////////////////////////////////////////////////////////////////

//接口结构
typedef struct tagDSPInit
{
	u32     m_dwDspID;          //初始化的DSP编号
	u32     m_dwLogoNum;        //是(1)否(0)需要在DSP上加logo，banner，OSD等
	u32     m_dwNumLogoNum;     //暂不使用
	u32     m_dwAudEncNum;      //音频编码路数
	u32     m_dwAudDecNum;      //音频解码路数
	BOOL32  m_bInitAec;         //是否使用AEC
    BOOL32  m_bOutD1;           //是否固定输出D1
    u32     m_bDspDDRFreq;      //Dsp DDR频率(135、162)
}TDSPInit;

typedef struct tagCodecInit
{
	u32     dwDspID;   //Dspid kdv7810(0:videnc, 1:viddec, 2:audio)
	u32     dwChnID;   // 0,1....
	u32     dwVPID;    // 0:不通过DSP直接采集， 1:通过DSP环回后采集
                       /* 解码器多VP输出： 占用的VP口，一个chnl最多同时从4个VP口输出。
                       32位拆分为4个Byte,每个byte对应一个视频输出口:
                       Bit[ 7: 0]为主视频输出: 范围0~FPGA_DEC_VP_MAX_NUM-1,分别对应VP0/1/2/3；
                       Bit[15: 8]为辅视频输出1；
                       Bit[23:16]为辅视频输出2；
                       Bit[31:24]为辅视频输出3；
                       3个辅视频输出段采用[VP号+1]来指示对应的VP号，
                       0 表示对应辅视频输出关闭。
                       可使用MAKEVPID(nMain,nSub),来生成，nMain为主VPID， nSub为辅VPID*/

	u32     dwCapPort; //采集端口
}
TEncoder, TDecoder;

typedef struct tagVidSrcInfo
{
    u16	    m_wWidth;                     /* 视频宽，像素为单位,为0表示无信号 */
    u16	    m_wHeight;                    /* 视频高，行为单位,为0表示无信号 */
    BOOL32  m_bProgressive;               /* 逐行或隔行，TRUE=逐行；FALSE=隔行 */
    u32     m_dwFrameRate;                /* 帧率，逐行时=场频，隔行时=场频/2，即60i=30P,为0表示无信号 */
}
TVidSrcInfo;

typedef struct tagVidFrameInfo
{
	BOOL32    m_bKeyFrame;    //频帧类型（I or P）
	u16       m_wVideoWidth;  //视频帧宽
	u16       m_wVideoHeight; //视频帧宽
}
TVidFrameInfo;

typedef struct tagFrameHeader
{
	u32     m_dwMediaType;    //码流类型
	u32     m_dwFrameID;      //帧标识，用于接收端
	u32     m_dwSSRC;         //同步源信息，用于接收端
	u8*     m_pData;          //音视频数据
	u32     m_dwDataSize;     //数据长度
    union
    {
		TVidFrameInfo m_tVideoParam;
        u32           m_dwAudioMode;//音频模式
    };
}
TFrameHeader, *PTFrameHeader;

//音频解码参数,只有AAC_LC有效
typedef struct 
{
    u32 m_dwMediaType;            //音频媒体类型 MEDIA_TYPE_AACLC
    u32 m_dwSamRate;              //音频采样率, SAMPLE_RATE32000等
    u32 m_dwChannelNum;           //音频声道数, 1、2
}
TAudioDecParam;

typedef struct
{
	u32 dwFrmType;				    //0:不固定类型, 1: half cif, 2: 1 cif, 3: 2 cif,  4: 4 cif,  5: 用于图像合成编码
	u32 dwFrameRate;				//帧率
	u32 dwIFrmInterval;				//I帧间隔
	u32 dwBitRate;					//码率
	BOOL32 bFrmRateLittleThanOne;	//编码帧率小于1 ?
	BOOL32 bFramRateCanSet;			//编码帧率是否由外部设定? 1:是，0：否
	BOOL32 bAutoDelFrame;			//编码器是否自动丢帧1：是，0：否				
}TMpv4FrmParam;


typedef struct 
{
	u32 m_dwReserve;
}
TVideoDecParam;

/*视频编码参数*/
typedef struct tagVideoEncParam
{
	u16   m_wVideoWidth;            /*图像宽度(default:1280)*/
	u16	  m_wVideoHeight;           /*图像高度(default:720)*/
    u16   m_byMaxKeyFrameInterval;  /*I帧间最大P帧数目*/
    u16   m_wBitRate;               /*编码比特率(Kbps)*/

	u8    m_byEncType;              /*图像编码类型*/
    u8    m_byRcMode;               /*图像压缩码率控制参数*/    
	u8    m_byMaxQuant;             /*最大量化参数(1-51)*/
    u8    m_byMinQuant;             /*最小量化参数(1-51)*/

	u8	  m_byFrameRate;            /*帧率(default:60)*/	 
	u8	  m_byFrmRateCanSet;        /*帧率是否由外界设定 ?, 无效*/
	u8    m_byFrmRateLittleThanOne; /* 帧率是否小于1 ?, 无效*/
	u8    m_byImageQulity;          /*图象压缩质量,0:速度优先;1:质量优先, 无效*/

	u8  m_byFrameFmt;               /*分辨率*/ 
	u8  m_byReserved1;              /*保留*/
}TVideoEncParam;

/*编码器状态*/
typedef struct tagKdvEncStatus
{
	BOOL32 			m_bVideoSignal;			/*是否有视频信号*/
	BOOL32			m_bAudioCapStart;       /*是否开始音频捕获*/
	BOOL32			m_bEncStart;            /*是否开始编码*/	
}TKdvEncStatus;

/*编码器统计信息*/
typedef struct tagKdvEncStatis
{
    u32  m_dwFrameRate;  /*帧率*/
    u32  m_dwBitRate;    /*码流速度*/
    u32  m_dwPackLose;   /*丢帧数*/
    u32  m_dwPackError;  /*错帧数*/
	u32  m_wAvrBitRate;  /*1分钟内视频编码平均码率*/
}TKdvEncStatis;



/*解码器状态*/
typedef struct tagKdvDecStatus
{
	BOOL32	m_bDecStart;         /*是否开始解码*/	
	u32     m_dwAudioDecType;    /*解码类型*/
    u32     m_dwHeart;           /*解码线程心跳*/
	u32     m_dwPos;             /*解码线程位置*/
    u32     m_dwDecType;
}TKdvDecStatus;

/*解码器统计信息*/
typedef struct tagKdvDecStatis
{
	u16	   m_wFrameRate;           /*解码帧率*/
    u16    m_wLoseRatio;           /*丢失率,单位是%*/ 
	u32    m_dwRecvFrame;          /*收到的帧数*/
	u32    m_dwLoseFrame;          /*丢失的帧数*/		
	u32    m_dwPackError;          /*乱帧数*/ 
	u32    m_dwIndexLose;          /*序号丢帧*/
	u32    m_dwSizeLose;           /*大小丢帧*/
	u32    m_dwFullLose;           /*满丢帧*/	
	u16	   m_wBitRate;             /*视频解码码率*/
	u16    m_wAvrVideoBitRate;     /*1分钟内解码平均码率*/
	BOOL32 m_bVidCompellingIFrm;   /*视频强制请求I帧*/								  
	u32    m_dwDecdWidth;	       /*码流的宽*/
	u32    m_dwDecdHeight;         /*码流的高*/
}TKdvDecStatis;

typedef struct tagTranspDColor
{
	u8 RColor;          //R背景
	u8 GColor;          //G背景
	u8 BColor;          //B背景	
	u8 u8Transparency;  //透明度(0表示全透明，255表示不透明，1~254表示部分透明)
}TTranspColor;

typedef struct BackBGDColor
{
    u8 RColor;    //R背景
    u8 GColor;    //G背景
    u8 BColor;    //B背景	
}TBackBGDColor;       //仅为终端兼容使用

/* 滚动横幅参数 */
typedef struct
{
	u32 dwXPos;
    u32 dwYPos;//显示区域的Y坐标
    u32 dwWidth;//显示区域的宽
    u32 dwHeight;//显示区域的高
    u32 dwBMPWight;//BMP图像宽
    u32 dwBMPHeight;//BMP图像高
    u32 dwBannerCharHeight;//字体高度－－主要用于上下滚动时，确定停顿的位置
    TTranspColor tBackBGDColor;//背景色 
    u32 dwPicClarity;          //整幅图片透明度
    u32 dwRunMode;//滚动模式：上下or左右or静止（宏定义如上）
    u32 dwRunSpeed;//滚动速度 四个等级（宏定义如上）
    u32 dwRunTimes;//滚动次数 0为无限制滚动
 	u32 dwHaltTime;//停顿时间(秒)，上下滚动时，每滚完一行字的停顿间隔：0始终停留（与静止等同） 
}TAddBannerParam;


//////////////////////////////////////////////////////////////////////////
//adapter structs

/* 音频编码参数 */
typedef struct
{
    u8  byAudioEncMode; /*声音编码模式*/
    u8  byAudioDuration;/*时长*/
}TAdapterAudioEncParam;

/* 适配器编码参数 */
typedef union
{
    TVideoEncParam          tVideoEncParam[6];
    TAdapterAudioEncParam   tAudioEncParam;
}TAdapterEncParam;

//适配通道
typedef struct
{
    u8 byChnNo;//通道号
    u8 byMediaType;//目标码流类型(必须指定)
    TAdapterEncParam  tAdapterEncParam; // 编码参数
}TAdapterChannel;

//适配组状态
typedef struct
{
    BOOL32     bAdapterStart;      //适配是否开始
    u8     byCurChnNum;  //目前正在适配的通道数目
    TAdapterChannel atChannel[6];  //组中的通道
}TAdapterGroupStatus;

//通道统计信息
typedef struct	
{
    u32  dwRecvBitRate;        //接收码率
    u32  dwRecvPackNum;        //收到的包数
    u32  dwRecvLosePackNum;    //网络接收丢包数
    u32  dwSendBitRate;        //发送码率
    u32  dwSendPackNum;        //发送的包数
    u32  dwSendLosePackNum;    //发送丢包数
    BOOL32 m_bVidCompellingIFrm;	//是否要强制关键帧
#ifdef MAU_VMP
    u32  dwDecChnl;                 //*****新
#endif
}TAdapterChannelStatis;

typedef struct
{
    u32 dwChannelNum;
}TAdapterInit;

////////////////////////////////////////////////////////////////////////////
//

typedef void ( *FRAMECALLBACK)(PTFrameHeader pFrameInfo, void* pContext);
typedef void ( *TDecodeVideoScaleInfo)(u16 wVideoWidth, u16 wVideoHeight);
typedef void ( *TDebugCallBack )(char *pbyBuf, int dwBufLen);
typedef void ( *CHANGECALLBACK)(void* pContext);
typedef void ( *CpuResetNotify)(s32 nCpuType);  //nCpuType：DSP ID，0，1，2 等
typedef void ( *VIDENCCALLBACK)(TVideoEncParam* pFrameInfo, void* pContext);//gaoden:MainVidEncParam设置完成之回调(2009-7-8)

#endif
#endif





















