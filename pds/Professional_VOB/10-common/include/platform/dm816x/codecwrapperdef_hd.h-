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
#ifndef _CODECWRAPPER_DEF_HD3_H_
#define _CODECWRAPPER_DEF_HD3_H_

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
	en_HDU2_Board_VMP_BAS,//hdu2作画面合成或适配
    en_HDU2_Board_S,
	en_Invalid_Board,
};
//MPU2模式设置
#define VMP_BASIC				0
#define VMP_ENHANCED			1
#define BAS_BASIC				2
#define BAS_ENHANCED			3
//模式切换:前适配通道为1080p30能力
//仅支持VMP_BASIC或VMP_ENHANCED与VMP_ADP_1080P30DEC切换,不可在三者之间互切
//不可在创建时指定该模式
#define VMP_ADP_1080P30DEC	4

//HDU2 一个通道解码多路，且输出合成图像
#define HDU2_VMP				5

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
	Codec_Error_GetMem
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
}enSampleRate;

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
#define VIDIO_HDMI1                 0x00000002
#define VIDIO_HDMI2                 0x00000004
#define VIDIO_HDMI3                 0x00000008
#define VIDIO_VGA0                  0x00000010
#define VIDIO_VGA1                  0x00000020
#define VIDIO_VGA2                  0x00000040
#define VIDIO_YPbPr0                0x00000100
#define VIDIO_YPbPr1                0x00000200  /* 在KDV7180上该接口和VIDIN_VGA0冲突，只能2选1 */
#define VIDIO_YPbPr2                0x00000400
#define VIDIO_SDI0                  0x00001000
#define VIDIO_SDI1                  0x00002000
#define VIDIO_C0                    0x00010000
#define VIDIO_C1                    0x00020000
#define VIDIO_S0                    0x00100000
#define VIDIO_DVI0                  0x01000000
#define VIDIO_DVI1                  0x02000000
#define VIDIO_DVI2                  0x04000000
#define VIDIO_CAMERA0               0x10000000


/*图象参数*/
enum
{
    VIDCAP_SCALE_BRIGHTNESS = 0, /* 设置亮度，  C | YPbPr */
    VIDCAP_SCALE_CONTRAST,       /* 设置对比度，C | YPbPr */
    VIDCAP_SCALE_HUE,            /* 设置色度，  C */
    VIDCAP_SCALE_SATURATION,      /* 设置饱和度，C | YPbPr */
	VIDCAP_SCALE_SHAPNESS,		//设置锐度的值，类型为int * 型，取值范围2 》= pArgs 》= 0， 0锐度最强，1次之，2最弱
	VIDCAP_SCALE_NOISE,			//设置去噪的值，类型为 int * 型，取值范围3 》= pArgs 》= 0，3表示关闭去噪，0去噪最强，1次之，2最弱
	VIDCAP_SCALE_BACKLIGHT
};

/*SD信号固定输出格式*/
enum
{
	VID_OUTTYPE_ORIGIN = 0,	//原始比例播放
	VID_OUTTYPE_480I60,     //固定按 480i30输出
	VID_OUTTYPE_576I,		//固定按 576i25输出
	VID_OUTTYPE_576P,
	VID_OUTTYPE_720P50,		//固定按 720p50输出
	VID_OUTTYPE_720P60,		//固定按 720p60输出
	VID_OUTTYPE_1080I50,	//固定按1080i25输出
	VID_OUTTYPE_1080I60,	//固定按1080i30输出
	VID_OUTTYPE_1080P24,	//固定按1080p24输出
	VID_OUTTYPE_1080P25,	//固定按1080p25输出
	VID_OUTTYPE_1080P30,	//固定按1080p30输出
    //二代高清支持
	VID_OUTTYPE_1080P50,	//固定按1080p50输出
	VID_OUTTYPE_1080P60,	//固定按1080p60输出
	VID_OUTTYPE_1080P23,	//固定按1080p23输出
	VID_OUTTYPE_1080P29,	//固定按1080p29输出
	VID_OUTTYPE_1080P59,	//固定按1080p59输出
    //VGA制式
	VID_OUTTYPE_VGA60,	   //固定按VGA 60HZ输出 640x480
	VID_OUTTYPE_VGA75,	   //固定按VGA 75HZ输出
	VID_OUTTYPE_SVGA60,	   //固定按SVGA 60HZ输出 800x600
	VID_OUTTYPE_SVGA75,	   //固定按SVGA 75HZ输出
	VID_OUTTYPE_XGA60,	  //固定按XGA 60HZ输出 1024x768
	VID_OUTTYPE_XGA75,	  //固定按XGA 75HZ输出
	VID_OUTTYPE_SXGA60,	  //固定按SXGA 60HZ输出  1280x1024
	VID_OUTTYPE_SXGA75,   //固定按SXGA 75HZ输出  1280x1024
	VID_OUTTYPE_UXGA60,	  //固定按UXGA 60HZ输出 1600x1200
	VID_OUTTYPE_WXGA1280x768_60,	  //固定按WXGA 60HZ输出 1280x768
	VID_OUTTYPE_WXGA1280x768_75,	  //固定按WXGA 75HZ输出 1280x768
	VID_OUTTYPE_WXGA1280x800_60,	  //固定按WXGA 60HZ输出 1280x800
	VID_OUTTYPE_WXGA1280x800_75,	  //固定按WXGA 75HZ输出 1280x800
	VID_OUTTYPE_WXGA1360x768_60,	  //固定按WXGA 60HZ输出 1360x768
	VID_OUTTYPE_WXGA1360x768_75,	  //固定按WXGA 75HZ输出 1360x768
	VID_OUTTYPE_WSXGA60,	  //固定按WSXGA 60HZ输出 1440x900
	VID_OUTTYPE_WSXGAPLUS60	  //固定按SXGA+ 60HZ输出 1680x1050
};

//PIP mode
enum  enVideoPIPMode
{
	VIDEO_PIPMODE_ONE = 0,              //单画面
	VIDEO_PIPMODE_TWO_LEFTRIGHT,         //两画面,左右
	VIDEO_PIPMODE_TWO_LEFTUP,            //两画面,左上
	VIDEO_PIPMODE_TWO_RIGHTUP,		     //两画面,右上
	VIDEO_PIPMODE_TWO_RIGHTDOWN,   	     //两画面,右下
	VIDEO_PIPMODE_TWO_LEFTDOWN,		     //两画面,左下
	VIDEO_PIPMODE_THREE,                 //三画面，品字形
	VIDEO_PIPMODE_THREE_ONEBIG,          //三画面，一大二小
	VIDEO_PIPMODE_FOUR,				//  4画面， hdu2用
    VIDEO_PIPMODE_INVALID
};

//画中画位置索引
enum enVideoPIPIndex
{
	VIDEO_PIPINDEX_NULL = 0,              //空画面
	VIDEO_PIPINDEX_LOCAL,			      //本地
	VIDEO_PIPINDEX_MAINVIDDEC,            //主解码
	VIDEO_PIPINDEX_SUBVIDDEC,             //副解码
	VIDEO_PIPINDEX_SUBLOCAL,              //副编码
    VIDEO_PIPINDEX_INVALID
};

/* 重启编解码设备ID*/
enum
{
	CODECDEV_VIDENC = 0,     //视频编码设备
	CODECDEV_VIDDEC,         //视频解码设备
	CODECDEV_AUDDEV          //音频编解码设备
};

/* SDI接口类型*/
enum
{
	SDITYPE_NONE = 0,     //没有插SDI模块
	SDITYPE_ININ,         //两个输入模块
	SDITYPE_INOUT         //一个输入，一个输出模块
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
    INITMODE_EBAS,                  //标清适配模式，相当于调用  HardMPUInitDev(2,0,4)
    INITMODE_HDU,                   //高清电视墙模式，相当于调用  HardMPUInitDev(0,1,1)
    INITMODE_NUM
};

/* 音频输入输出接口宏定义 */
#define AUDIO_HDMI0					0x00000001
#define AUDIO_HDMI1                 0x00000002
#define AUDIO_HDMI2                 0x00000004
#define AUDIO_HDMI3                 0x00000008
#define AUDIO_C0					0x00000030//c端
#define AUDIO_C_LEFT                0x00000010//c端左声道
#define AUDIO_C_RIGHT				0x00000020//c端右声道
#define AUDIO_MIC0                  0x00000100//canon麦克0输入
#define AUDIO_MIC1                  0x00000200//canon麦克1输入
#define AUDIO_SPEAKER0              0x00001000//扬声器
#define AUDIO_DIGIT0				0x00010000//数字麦克0输入
#define AUDIO_DIGIT1				0x00020000//数字麦克1输入
#define AUDIO_35mm0					0x00100000//3.5mm0
#define AUDIO_35mm1					0x00200000//3.5mm1
//#define AUDIO_635mm					0x01000000//6.35mm
#define AUDIO_635mm					0x03000000//6.35mm
#define AUDIO_635mm_LEFT			0x01000000//6.35mm
#define AUDIO_635mm_RIGHT			0x02000000//6.35mm

#define AUDIO_SDI0			        0x10000000//SDI0
#define AUDIO_SDI1			        0x20000000//SDI1
/* 短消息及横幅滚动方式宏定义 */
//滚动方式
#define RUNMODE_STATIC     0    //短消息不支持
#define RUNMODE_LEFTRIGHT  1    
#define RUNMODE_UPDWON     2    
#define RUNMODE_UPDWONPAGE 3    //横幅翻页滚动
//滚动速度
#define RUNSPEED_FAST      4
#define RUNSPEED_HIGH      3
#define RUNSPEED_NORM      2
#define SPEED_LOW          1

//无视频输出时播放策略
enum enplayMode
{
	EN_PLAY_LAST = 0,
	EN_PLAY_BLUE,
	EN_PLAY_BMP,
	EN_PLAY_BLACK,
	EN_PLAY_BMP_USR,
	EN_PLAY_ERROR
};
enum enZoomMode
{
    EN_ZOOM_FILLBLACK = 0,//加黑边
    EN_ZOOM_CUT,//裁边
    EN_ZOOM_SCALE//不等比拉伸
};
/*组合主、辅视频输出VP*/
#define MAKEVPID(nMain,nSub)          (u32)( ((u32)nMain&0x000000FF) | (((u32)(nSub+1)&0x000000FF)<<8) )

//宏块编码质量设置
#define VIDENC_MBSIZE_ALL         (u32)0xFF0FFFFF
#define VIDENC_MBSIZE_P8x8UP      (u32)0xFF0FE0FF
#define VIDENC_MBSIZE_DEFAULT     0  //目前为0xFF0FFFFF

#define VIDENC_MBSIZE_ENABLE      VIDENC_MBSIZE_ALL
#define VIDENC_MBSIZE_DISABLE     VIDENC_MBSIZE_P8x8UP
//////////////////////////////////////////////////////////////////////////
/*
//adpter 宏定义
#define MAX_VIDEO_ADAPT_CHANNUM       1
#define MAX_AUDIO_ADAPT_CHANNUM       3
#define MAX_ADAPT_CHNNUM              (MAX_VIDEO_ADAPT_CHANNUM+MAX_AUDIO_ADAPT_CHANNUM)

#define MAX_TRANS_VIDEO_ADAPT_CHANNUM       20
#define MAX_TRANS_AUDIO_ADAPT_CHANNUM       20

#define ADAPTER_CODECID_ENCHD               0
#define ADAPTER_CODECID_ENCSD               1
#define ADAPTER_CODECID_DEC                 2
*/
//////////////////////////////////////////////////////////////////////////
#define NF_MODE_DISABLE               0
#define NF_MODE_LOW                   1
#define NF_MODE_MED					  2
#define NF_MODE_HIGH                  3

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

typedef struct tagVidEncInit 
{
    u32 dwChnID;
    u32 dwCapPort;
    u32 dwEncTaosID;
    u32 dwTaosVPID;
    u32 dwBeforeDSP;
    u32 dwDspLinkToTaosVP;
    u32 dwEncDspID;
    u32 dwDspInVPID;
    u32 dwPlyLocalDspID;
    u32 dwPlyLocalVP;
	BOOL32 bFpsInStream;
}
TVidEncInit;

typedef struct tagVidDecInit 
{
    u32 dwChnID;
    u32 dwPlyPort;
    u32 dwDecTaosID;
    u32 dwTaosVPID;
    u32 dwAfterDSP;
    u32 dwDspLinkToTaosVP;
    u32 dwDecDspID;
    u32 dwDspOutVPID;
    u32 dwSubOutDsp;
    u32 dwSubOutVP;
    u32 dwPlyStd;   //默认 VID_OUTTYPE_1080P50
}
TVidDecInit;

//HD3 结构体
typedef struct tagVidEncIntit3RD
{
    u32 dwChnID;
    u32 dwCapPort;
    u32 dwPlyStd;   //默认 VID_OUTTYPE_1080P50
	u32 dwReserved;
}TVidEncHD3RD;
typedef struct tagVidDecBufInfo
{
	u32 dwPlayBufferCnt; //上层可以设定播放用缓冲数量(1~5) 默认为0 ，底层按照最大分配
	u32 dwDecBufferCnt; //上层可配的解码后处理buffer数量(1~5)，默认为0，底层按照最大分配
}TVidDecBufInfo;

typedef struct tagVidDecIntit3RD
{
    u32 dwChnID;
    u32 dwPlyPort;
    u32 dwPlyStd;   //默认 VID_OUTTYPE_1080P50
	BOOL32 bCPortOutputEnable;// c端子随主输出 默认关闭
	//以下参数可用户配置也可使用默认，buffer过多会造成延时，过少会引起抖动
	TVidDecBufInfo* pExtraBufInfo;//if NULL - use defult num
	u32 dwReserved;	//  HUD2 通道要输出画面合成时，赋值:HDU2_VMP
}TVidDecHD3RD;

typedef struct tagVidSrcInfo
{
    u16	    m_wWidth;                     /* 视频宽，像素为单位,为0表示无信号 */
    u16	    m_wHeight;                    /* 视频高，行为单位,为0表示无信号 */
    BOOL32  m_bProgressive;               /* 逐行或隔行，TRUE=逐行；FALSE=隔行 */
    u32     m_dwFrameRate;                /* 帧率，逐行时=场频，隔行时=场频/2，即60i=30P,为0表示无信号 */
}
TVidSrcInfo;

typedef struct tagVidPortInfo
{
    TVidSrcInfo m_tVidSrcInfo;	//视频制式
	u32			m_dwVidPort;	//视频端口			
}
TVidPortInfo;

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
	u32		m_dwFPS;		  //帧率
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
	u16 m_wVideoWidth;				/*图像宽度(default:1280)*/
	u16	m_wVideoHeight;				/*图像高度(default:720)*/
	u8  m_byEncType;				/*图像编码类型*/
 	u8  m_byImageQulity;			/*图象压缩质量,0:速度优先;1:质量优先, 无效*/
	u16 m_wBitRate;					/*编码比特率(Kbps)*/
	
	u8  m_byFrameFmt;				/*分辨率*/ 
    u8  m_byRcMode;					/*图像压缩码率控制参数*/    
	u8  m_byMaxQuant;				/*最大量化参数(1-51)*/
    u8  m_byMinQuant;				/*最小量化参数(1-51)*/
	u32 m_dwAvgQpI;					/* 平均qp 默认0- 28*/
	u32 m_dwAvgQpP;
	u8	m_byFrameRate;				/*帧率(default:60)*/	 
	u8	m_byFrmRateCanSet;			/*帧率是否由外界设定 ?, 无效*/
	u8  m_byFrmRateLittleThanOne;	/* 帧率是否小于1 ?, 无效*/
	u8  m_byLoopFilterMask;		    /*开启环路滤波 默认为0 不开启*/
    u32	m_dwMaxKeyFrameInterval;	/*I帧间最大P帧数目*/
	
	
	u32  m_dwProfile;				// 0 BaseLine | 1 HighProfile
	u32  m_dwReserved;				/*保留*/
	//追加编码器参数
	u32  m_dwCabacDisable;			/*是否开启CABAC*/
	u32  m_dwSilceMbCont;           /*slice的宏块数量，设零则为单slice*/
	u32  m_dwRateControlAlg;        /*码率控制算法，仅对m_byRcMode == 1 时有效*/
	
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

typedef struct
{
    u32 dwWidth;    //图片所对应的视频源制式宽度
    u32 dwHeight;  //图片所对应的视频源制式高度
    u8 *pBMP;
    u32 dwBmpLen;
}TBmpInfo;
typedef struct
{
    TBmpInfo tBmpInfo[3];
    TAddBannerParam tBannerParam[3];
    u32 Reserver;
}TFullBannerParam;

typedef struct {
    u32 dwXPos;
    u32 dwYPos;
    u32 dwLogoWidth;//logo宽
    u32 dwLogoHeight;//logo高
    TTranspColor tBackBGDColor;    
}TAddLogoParam;

typedef struct
{
    TBmpInfo tBmpInfo[3];
    TAddLogoParam tLogoParam[3];
}TFullLogoParam;

//////////////////////////////////////////////////////////////////////////
//adapter structs
#if 0
/* 音频编码参数 */
typedef struct
{
    u8  byAudioEncMode; /*声音编码模式*/
    u8  byAudioDuration;/*时长*/
}TAdapterAudioEncParam;

/* 适配器编码参数 */
typedef union
{
    TVideoEncParam          tVideoEncParam[2];
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
    TAdapterChannel atChannel[MAX_ADAPT_CHNNUM];  //组中的通道
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
#endif
////////////////////////////////////////////////////////////////////////////
//

typedef void ( *FRAMECALLBACK)(PTFrameHeader pFrameInfo, void* pContext);
typedef void ( *TDecodeVideoScaleInfo)(u16 wVideoWidth, u16 wVideoHeight);
typedef void ( *TDebugCallBack )(char *pbyBuf, int dwBufLen);
typedef void ( *CHANGECALLBACK)(void* pContext);
typedef void ( *CpuResetNotify)(s32 nCpuType);  //nCpuType：DSP ID，0，1，2 等
typedef void ( *VIDENCCALLBACK)(TVideoEncParam* pFrameInfo, void* pContext);	//编码参数设置
typedef void ( *VIDOUTTYPECALLBACK)(TVidSrcInfo* pSetVidOutInfo, TVidSrcInfo* pUseVidOutInfo);	//播放制式保护
typedef void ( *VIDDECPLAYTYPECALLBACK)(BOOL32 bVGAStream, BOOL32 bVGAPlayPort);	//双流解码制式和播放接口不匹配时
#endif





















