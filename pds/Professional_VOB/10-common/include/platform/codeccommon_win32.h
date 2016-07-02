/*****************************************************************************
   模块名      : 
   文件名      : CodecCommon.h
   相关文件    : 
   文件实现功能: 通用定义。
   作者        : 李雪峰
   版本        : V3.0  Copyright(C) 2001-2002 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人         修改内容
   2004/02/07  3.0         李雪峰           创建
******************************************************************************/
#ifndef __COMMON_H_
#define __COMMON_H_

#define CODECVERSION               "CodecWrapper 40.10.05.74.080102"

typedef struct BackBGDColor
{
	u8 RColor;    //R背景
	u8 GColor;    //G背景
	u8 BColor;    //B背景	
}TBackBGDColor;

typedef struct tagNetRSParam
{
    u16  m_wFirstTimeSpan;	  //第一个重传检测点
	u16  m_wSecondTimeSpan;  //第二个重传检测点
	u16  m_wThirdTimeSpan;   //第三个重传检测点
	u16  m_wRejectTimeSpan;  //过期丢弃的时间跨度
} TNetRSParam;

/*==================================错误码=======================*/

#ifndef DES_ENCRYPT_MODE
#define DES_ENCRYPT_MODE         (u8)0      //DES加密模式
#define AES_ENCRYPT_MODE         (u8)1      //AES加密模式
#define ENCRYPT_KEY_SIZE         (u8)32     //密钥长度 取其中的较大值
#define DES_ENCRYPT_KEY_SIZE     (u8)8      //DES密钥长度
#define AES_ENCRYPT_KEY_SIZE_MODE_A (u8)16  //AES Mode-A 密钥长度
#define AES_ENCRYPT_KEY_SIZE_MODE_B (u8)24  //AES Mode-B 密钥长度
#define AES_ENCRYPT_KEY_SIZE_MODE_C (u8)32  //AES Mode-C 密钥长度
#endif

#ifdef SOFT_CODEC
#define CODEC_NO_ERROR              (u16)0//无错
#define CODEC_ERROR_BASE            (u16)10000//
#define ERROR_CODEC_UNKNOWN	        (u16)10000//不知名的编解码器错误 
#define ERROR_CODEC_PARAM           (u16)(CODEC_ERROR_BASE + 1)//参数错误;  
#define ERROR_DEC_DRAW_HDC	        (u16)(CODEC_ERROR_BASE + 2)//解码器画图窗口DC错误
#define ERROR_DEC_DRAW_DIBOPEN      (u16)(CODEC_ERROR_BASE + 3)//解码器画图对象打开错误
#define ERROR_DEC_DRAW_DIBBEGIN     (u16)(CODEC_ERROR_BASE + 4)//解码器画图格式错误
#define ERROR_DEC_DRAW_CLEARWINDOW  (u16)(CODEC_ERROR_BASE + 5)//解码器清屏错误
#define ERROR_DEC_ICOPEN            (u16)(CODEC_ERROR_BASE + 6)//解码器打开解码驱动错误
#define ERROR_DEC_DECOMPRESSBEGIN   (u16)(CODEC_ERROR_BASE + 7)//解码器设置解码驱动格式错误
#define ERROR_DEC_SETTIMER          (u16)(CODEC_ERROR_BASE + 8)//解码器设置解码定时器错误
#define ERROR_DEC_NO_VIDEO          (u16)(CODEC_ERROR_BASE + 9)//解码器没有创建视频
#define ERROR_DEC_SOCKET_CALLBACK   (u16)(CODEC_ERROR_BASE + 10)//解码器设置网络事件回调错误
#define ERROR_DEC_SOCKET_CREATE     (u16)(CODEC_ERROR_BASE + 11)//解码器创建网络套接字错误
#define ERROR_DEC_NO_AUDIO          (u16)(CODEC_ERROR_BASE + 12)//解码器没有创建音频 
#define ERROR_DEC_WAVEOPEN          (u16)(CODEC_ERROR_BASE + 13)//解码器不能打开声卡*
#define ERROR_DEC_MIXEROPEN         (u16)(CODEC_ERROR_BASE + 14)//解码器不能打开声卡音量控制
#define ERROR_DEC_WAVEPREHEADER     (u16)(CODEC_ERROR_BASE + 15)//解码器不能设置音频播放头
#define ERROR_DEC_WAVEWRITE         (u16)(CODEC_ERROR_BASE + 16)//解码器不能向声卡写音频数据
#define ERROR_DEC_WAVE_ENCOPEN      (u16)(CODEC_ERROR_BASE + 17)//解码器不能打开音频解码器*
#define ERROR_DEC_WAVE_ENCPREHEADER (u16)(CODEC_ERROR_BASE + 18)//解码器不能设置音频解码头
#define ERROR_DEC_WAVE_ENCCONVERT   (u16)(CODEC_ERROR_BASE + 19)//解码器音频解码错误
#define ERROR_DEC_WAVESTART         (u16)(CODEC_ERROR_BASE + 20)//解码器开始音频播放错误
#define ERROR_DEC_WAVESTOP          (u16)(CODEC_ERROR_BASE + 21)//解码器停止音频播放错误
#define ERROR_DEC_MIXER_SETVOLUME   (u16)(CODEC_ERROR_BASE + 22)//解码器设置音量出错
#define ERROR_DEC_MIXER_GETVOLUME   (u16)(CODEC_ERROR_BASE + 23)//解码器获取音量出错
#define ERROR_DEC_MIXERMUTE         (u16)(CODEC_ERROR_BASE + 24)//解码器设置静音出错
#define ERROR_DEC_AUDIOOUT_EXIT     (u16)(CODEC_ERROR_BASE + 25)//解码器音频线程退出出错
#define ERROR_DEC_HARDCREATE        (u16)(CODEC_ERROR_BASE + 26)//解码器创建硬件解码器失败
#define ERROR_DEC_CREATE            (u16)(CODEC_ERROR_BASE + 27)//解码器创建出错 
#define ERROR_DEC_NEW               (u16)(CODEC_ERROR_BASE + 28)//对象生成出错
#define ERROR_DEC_SETAUDIO_ADDR     (u16)(CODEC_ERROR_BASE + 29)//解码器设置音频地址错误  
#define ERROR_DEC_AUDIO_LEN         (u16)(CODEC_ERROR_BASE + 30)//音频编解码的数据长度不正确
#define ERROR_DEC_NO_MASK           (u16)(CODEC_ERROR_BASE + 31)//既不是软件解码也不是硬件解码
#define ERROR_SOCKOBJ_NULL          (u16)(CODEC_ERROR_BASE + 32)//网络接收对象不存在
#define ERROR_DEC_NET_INVALIDPARAM  (u16)(CODEC_ERROR_BASE + 33)//无效的网络接收参数 
#define EROOR_DEC_NO_IMPLEMENT      (u16)(CODEC_ERROR_BASE + 34)//此功能还不支持 
#define ERROR_DEC_SETSTATE          (u16)(CODEC_ERROR_BASE + 35)//解码器设置解码驱动状态错误
#define ERROR_NO_AUDIO_DEV          (u16)(CODEC_ERROR_BASE + 36)//没有指定音频设备

#define ERROR_ENC_CAPWIN            (u16)(CODEC_ERROR_BASE + 100)//编码器创建捕获窗口出错
#define ERROR_ENC_CAPDRIVER         (u16)(CODEC_ERROR_BASE + 101)//编码器没有发现捕获卡
#define ERROR_ENC_DIBOPEN           (u16)(CODEC_ERROR_BASE + 102)//编码器画图对象打开错误
#define ERROR_ENC_DRAWHDC           (u16)(CODEC_ERROR_BASE + 103)//编码器画图窗口打不开
#define ERROR_ENC_ICOPEN            (u16)(CODEC_ERROR_BASE + 104)//编码器编码对象打开出错
#define ERROR_ENC_DRAWPOS           (u16)(CODEC_ERROR_BASE + 105)//编码器得到画图位置出错
#define ERROR_ENC_DIBBEGIN          (u16)(CODEC_ERROR_BASE + 106)//编码器设置画图格式出错
#define ERROR_ENC_GET_VIDEOBMP      (u16)(CODEC_ERROR_BASE + 107)//编码器得到图像格式出错
#define ERROR_ENC_SET_VIDEOBMP      (u16)(CODEC_ERROR_BASE + 108)//编码器设置图像格式出错
#define ERROR_ENC_GET_CAPSET        (u16)(CODEC_ERROR_BASE + 109)//编码器得到捕获设置出错
#define ERROR_ENC_SET_CAPSET        (u16)(CODEC_ERROR_BASE + 110)//编码器设置捕获设置出错
#define ERROR_ENC_SET_AUDIOFORMATE  (u16)(CODEC_ERROR_BASE + 111)//编码器设置音频捕获格式出错
#define ERROR_ENC_STREAMOPEN        (u16)(CODEC_ERROR_BASE + 112)//编码器打开音频编码对象出错
#define ERROR_ENC_PREHEADER         (u16)(CODEC_ERROR_BASE + 113)//编码器准备音频编码头出错
#define ERROR_ENC_MIXEROPEN         (u16)(CODEC_ERROR_BASE + 114)//编码器创建控制音量对象出错
#define ERROR_ENC_MIXER_SETVOLUME   (u16)(CODEC_ERROR_BASE + 115)//编码器设置音量出错
#define ERROR_ENC_MIXER_GETVOLUME   (u16)(CODEC_ERROR_BASE + 116)//编码器得到音量出错
#define ERROR_ENC_ALREADY           (u16)(CODEC_ERROR_BASE + 117)//编码器状态已经存在
#define ERROR_ENC_STOPCAP           (u16)(CODEC_ERROR_BASE + 118)//编码器无法停止捕获
#define ERROR_ENC_SETSTATE          (u16)(CODEC_ERROR_BASE + 119)//编码器设置编码参数出错
#define ERROR_ENC_ENCBEGIN          (u16)(CODEC_ERROR_BASE + 120)//编码器设置编码帧格式
#define ERROR_ENC_SETTIMER          (u16)(CODEC_ERROR_BASE + 121)//编码器设置定时器出错
#define ERROR_ENC_STOPVIDEOENC      (u16)(CODEC_ERROR_BASE + 122)//编码起停止视频编码出错
#define ERROR_ENC_NO_SETPARAM       (u16)(CODEC_ERROR_BASE + 123)//编码器没有设置参数
#define ERROR_ENC_STOPAUDIOENC      (u16)(CODEC_ERROR_BASE + 124)//编码器音频编码出错
#define ERROR_ENC_CREATESOCK        (u16)(CODEC_ERROR_BASE + 125)//编码器设置当地地址出错
#define ERROR_ENC_STOPSENDVIDEO     (u16)(CODEC_ERROR_BASE + 126)//编码器停止发送视频出错
#define ERROR_ENC_STOPSENDAUDIO     (u16)(CODEC_ERROR_BASE + 127)//编码器停止发送音频出错
#define ERROR_ENC_AUDIOENC          (u16)(CODEC_ERROR_BASE + 128)//编码器音频转换错误
#define ERROR_ENC_NO_CREATE         (u16)(CODEC_ERROR_BASE + 129)//编码器创建不成功  
#define ERROR_ENC_SOCK_INIT         (u16)(CODEC_ERROR_BASE + 130)//编码器网络套接字不是2.2版
#define ERROR_ENC_STARTCAP          (u16)(CODEC_ERROR_BASE + 131)//编码器无法开始捕获
#define ERROR_ENC_CAP_WIDTH         (u16)(CODEC_ERROR_BASE + 132)//编码器捕获帧太宽
#define ERROR_DRAW_FORMATE          (u16)(CODEC_ERROR_BASE + 133)//画图格式不支持
#define ERROR_HARDENC_FORMATE       (u16)(CODEC_ERROR_BASE + 134)//硬件不支持的画图格式
#define ERROR_ENC_CAP_FORMATE       (u16)(CODEC_ERROR_BASE + 135)//采集格式和编码格式不匹配
#define ERROR_ENC_NEW               (u16)(CODEC_ERROR_BASE + 136)//对象生成出错
#define ERROR_ENC_INVALID_PARAM     (u16)(CODEC_ERROR_BASE + 137)//无效的参数
#define ERROR_ENC_CREATE_EVENT      (u16)(CODEC_ERROR_BASE + 138)//不能创建事件 
#define EROOR_ENC_NO_IMPLEMENT      (u16)(CODEC_ERROR_BASE + 139)//此功能还不支持 
#define ERROR_ENC_GETSTATE          (u16)(CODEC_ERROR_BASE + 140)//编码器得到编码参数出错
#define ERROR_DECENC_CREATE_THREAD  (u16)(CODEC_ERROR_BASE + 141)//不能创建线程

#define ERROR_ENC_CAP_DESKTOP_FORMAT    (u16)(CODEC_ERROR_BASE + 142)//要求的编码器桌面捕获格式还不支持
#define ERROR_ENC_CREATE_DESKTOP_MEMDC  (u16)(CODEC_ERROR_BASE + 143)//创建桌面内存dc失败
#define ERROR_ENC_CAP_DESKTOP_WIDTH     (u16)(CODEC_ERROR_BASE + 144)//编码器捕获桌面分辨率高于当前桌面实际分辨率
#define ERROR_ENC_CAP_DESKTOP_BITCOUNT  (u16)(CODEC_ERROR_BASE + 145)//编码器捕获桌面位深高于当前桌面实际位深
#define ERROR_ENC_INVILAD_FILE      (u16)(CODEC_ERROR_BASE + 146)//提供的文件无效
#define ERROR_ENC_OPERATE_FILE     (u16)(CODEC_ERROR_BASE + 147)//文件编码器对指定的文件操作失败
#endif

#ifdef HARD_CODEC
#define ERROR_CODE_BASE             30000             
#define CODEC_NO_ERROR              (u16)0//无错
#define MAP_OPERATE_SUCCESS         (u16)0 /*MAP操作成功*/

#define MAP_OPEN_FAILURE            (u16)(ERROR_CODE_BASE+1)/*MAP打开失败*/
#define MAP_DOWNFILE_FAILURE        (u16)(ERROR_CODE_BASE+2)/*MAP程序下载失败*/
#define MAP_RUNAPP_FAILURE          (u16)(ERROR_CODE_BASE+3)/*MAP运行程序失败*/
#define MAP_OPENMSG_FAILURE         (u16)(ERROR_CODE_BASE+4)/*打开消息通道失败*/
#define MAP_CCI_FAILURE             (u16)(ERROR_CODE_BASE+5)/*CCI通讯失败*/
#define MAP_CCI_PEER_CLOSED         (u16)(ERROR_CODE_BASE+6)/*CCI读取失败*/
#define MAP_CCI_NOT_CONNECTED       (u16)(ERROR_CODE_BASE+7)/*CCI未连接*/
#define MAP_CCI_SMEM_CORRUPT        (u16)(ERROR_CODE_BASE+8)/*CCI内存损坏*/
#define MAP_CCI_LENGTH_ERROR        (u16)(ERROR_CODE_BASE+9)/*CCI长度错误*/
#define MAP_CCI_QUEUE_FULL          (u16)(ERROR_CODE_BASE+10)/*CCI队列满*/
#define MAP_CCI_MSG_LOST            (u16)(ERROR_CODE_BASE+11)/*CCI消息丢失*/
#define MAP_CCI_NOT_INIT            (u16)(ERROR_CODE_BASE+12)/*CCI未初始化*/
#define MAP_CCI_UNKNOWN_ERROR       (u16)(ERROR_CODE_BASE+13)/*CCI未知错误*/
#define MAP_NOT_READY               (u16)(ERROR_CODE_BASE+14)/*MAP未准备好*/
#define MAP_READ_SDRAM_FAIL         (u16)(ERROR_CODE_BASE+15)/*读MAP内存失败*/
#define MAP_WRITE_SDRAM_FAIL        (u16)(ERROR_CODE_BASE+16)/*写MAP内存失败*/

#define DECODER_OPERATE_SUCCESS     (u16)0/*解码成功操作*/
#define DECODER_ALL_NOT_INIT        (u16)(ERROR_CODE_BASE+1001)/*解码器未初始化*/
#define DECODER_VIDEO_NOT_INIT      (u16)(ERROR_CODE_BASE+1002)/*视频解码器未初始化*/
#define DECODER_AUDIO_NOT_INIT      (u16)(ERROR_CODE_BASE+1003)/*语音解码器未初始化*/
#define DECODER_VIDEO_TYPE_UNSPT    (u16)(ERROR_CODE_BASE+1004)/*视频解码器类型不支持*/
#define DECODER_AUDIO_TYPE_UNSPT    (u16)(ERROR_CODE_BASE+1005)/*音频解码器类型不支持*/
#define DECODER_ILLEGAL_MAPID       (u16)(ERROR_CODE_BASE+1006)/*MapId非法*/
#define DECODER_VIDEO_INIT_FAIL     (u16)(ERROR_CODE_BASE+1007)/*视频解码器初始化失败*/
#define DECODER_AUDIO_INIT_FAIL     (u16)(ERROR_CODE_BASE+1008)/*音频解码器初始化失败*/
#define DECODER_SNAPSHOT_FAIL_H261  (u16)(ERROR_CODE_BASE+1009)/*h261快照不支持*/

#define ENCODER_OPERATE_SUCCESS     (u16)0/*编码成功操作*/
#define ENCODER_ALL_NOT_INIT        (u16)(ERROR_CODE_BASE+2001)/*编码器未初始化*/
#define ENCODER_VIDEO_NOT_INIT      (u16)(ERROR_CODE_BASE+2002)/*视频编码器未初始化*/
#define ENCODER_AUDIO_NOT_INIT      (u16)(ERROR_CODE_BASE+2003)/*语音编码器未初始化*/
#define ENCODER_VIDEO_TYPE_UNSPT    (u16)(ERROR_CODE_BASE+2004)/*视频解码器类型不支持*/
#define ENCODER_AUDIO_TYPE_UNSPT    (u16)(ERROR_CODE_BASE+2005)/*音频解码器类型不支持*/
#define ENCODER_VIDEO_FRMTYPE_UNSPT (u16)(ERROR_CODE_BASE+2006)/*视频帧格式不支持*/
#define ENCODER_RTPHEADLEN_ERROR    (u16)(ERROR_CODE_BASE+2007)/*RTP头长度错*/
#define ENCODER_ILLEGAL_MAPID       (u16)(ERROR_CODE_BASE+2008)/*MapId非法*/
#define ENCODER_VIDEO_INIT_FAIL     (u16)(ERROR_CODE_BASE+2009)/*视频编码器初始化失败*/
#define ENCODER_AUDIO_INIT_FAIL     (u16)(ERROR_CODE_BASE+2010)/*音频编码器初始化失败*/
#define ENCODER_OTHER_LIB_ERR		(u16)(ERROR_CODE_BASE+2011)/*其它库的错误*/ 
#define ENCODER_SNAPSHOT_FAIL_H261  (u16)(ERROR_CODE_BASE+2012)/*h261快照不支持*/

#define SENDER_NOT_INIT             (u16)(ERROR_CODE_BASE+3003)/*发送模块未初始化*/
#define RECEIVER_NOT_INIT           (u16)(ERROR_CODE_BASE+3004)/*接收模块未初始化*/
#define PARAMETER_ERROR             (u16)(ERROR_CODE_BASE+3005)/*参数错误*/
#define HARDCODEC_NOT_INIT          (u16)(ERROR_CODE_BASE+3006)/*编解码器未初始化*/
#define HARDCODEC_INIT_FAIL         (u16)(ERROR_CODE_BASE+3007)/*编解码器初始化失败*/
#define MAGIC_NUMBER_ERROR          (u16)(ERROR_CODE_BASE+3008)/*类未构造*/

#define MIXER_OPERATE_SUCCESS       (u16)0         /*智能混音器操作成功*/

#define MIXER_NOT_INIT              (u16)(ERROR_CODE_BASE+4001)/*语音解码器未初始化*/
#define MIXER_CHANNEL_INUSE         (u16)(ERROR_CODE_BASE+4002)/*通道已使用*/
#define MIXER_CHANNEL_INIDLE        (u16)(ERROR_CODE_BASE+4003)/*通道还未使用*/
#define MIXER_SENDER_NOT_INIT       (u16)(ERROR_CODE_BASE+4004)/*发送模块未初始化*/
#define MIXER_RECEIVER_NOT_INIT     (u16)(ERROR_CODE_BASE+4005)/*接收模块未初始化*/
#define MIXER_START_NO_CHANNEL      (u16)(ERROR_CODE_BASE+4006)/*没有通道就开始混音*/
#define MIXER_GROUP_FULL            (u16)(ERROR_CODE_BASE+4007)/*组满*/ 
#define MIXER_CHANNEL_INVALID       (u16)(ERROR_CODE_BASE+4008)/*无效的组号*/ 
#define MIXER_NO_FORCEMIX_CHANNEL   (u16)(ERROR_CODE_BASE+4009)/*没有强制混音的通道*/
#define MIXER_PARAM_ERROR			(u16)(ERROR_CODE_BASE+4010)/*参数错*/

#define ADAPTER_NOT_INIT            (u16)(ERROR_CODE_BASE+5001)/*适配器未构造*/

#define PICMERGER_NOT_INIT          (u16)(ERROR_CODE_BASE+6001)/*图像合成器未构造*/

#define OSD_OPERATE_SUCCESS              (u16)(ERROR_CODE_BASE+7001)/*成功操作*/
#define OSD_INVALID_MENUID               (u16)(ERROR_CODE_BASE+7002)/*无效的菜单ID*/
#define OSD_GETBUFFER_FAILURE            (u16)(ERROR_CODE_BASE+7003)/*无法得到OSD内存地址*/
#define OSD_WRITEDATA_FAILURE            (u16)(ERROR_CODE_BASE+7004)/*OSD写数据失败*/
#define OSD_MAPBUFFER_NOTREADY           (u16)(ERROR_CODE_BASE+7005)/*OSD内存未就绪*/
#define OSD_INVALID_VOLUME               (u16)(ERROR_CODE_BASE+7006)/*无效的音量值*/
#define OSD_INVALID_TRANSPARENT          (u16)(ERROR_CODE_BASE+7007)/*无效的透明度*/
#define OSD_INVALID_RUNSPEED             (u16)(ERROR_CODE_BASE+7008)/*无效的滚动速度*/  
#define OSD_INVALID_MAPID                (u16)(ERROR_CODE_BASE+7009)/*无效的MapId*/
#define OSD_MAP_NOT_INIT                 (u16)(ERROR_CODE_BASE+70010)/*Map未初始化*/
#define OSD_REGIST_FAIL                  (u16)(ERROR_CODE_BASE+70011)/*向Map注册OSD失败*/
#define OSD_CLOSE_FAIL                   (u16)(ERROR_CODE_BASE+70012)/*Osd关闭失败*/

#endif

#define  SNAPSHOT_DIRECTORY		"snapshot"	
#define  MAX_SNAPSHOT_FILENAME_LEN	50
#define  MAPX_SNAPSHOT_SAVE_SIZE    (5<<20)

#define  VIDCAP_SCALE_BRIGHTNESS    (u8)0	//亮度
#define  VIDCAP_SCALE_CONTRAST		(u8)1	//对比度	
#define  VIDCAP_SCALE_SATURATION    (u8)2   //饱和度 

#define  VIDCAP_CAPVIDEO			(u8)1	//采集video
#define  VIDCAP_CAPVGA				(u8)2	//采集vga

//====================结构定义=====================================//
/*图像压缩码率控制参数*/
#define	 RC_MODE_CONST_BITRATE	 (u8)0x0 /*恒速率*/
#define  RC_MODE_VAR_BITRATE	 (u8)0x1 /*变速率*/

/*图像压缩优化控制参数*/
#define	 QC_MODE_QUALITY	 (u8)0x1 /*质量优先*/
#define  QC_MODE_SPEED		 (u8)0x0 /*速度优先*/

/*图象制式*/
#define	 VID_ENC_USEPAL		 (u8)0x21     /*PAL制式*/
#define	 VID_ENC_USENTSC	 (u8)0x22     /*NTSC制式*/

/*画面显示方式*/
#define	DISPLAY_TYPE_PAL	     (u8)0x1 /*PAL video显示*/
#define	DISPLAY_TYPE_NTSC	     (u8)0x2 /*NTSC video显示*/
#define	DISPLAY_TYPE_VGA	     (u8)0x3 /*VGA 显示*/
/*画中画显示方式*/
#define	PIP_LOCAL_IN_REMOTE	     (u8)0x0 /*本地为小画面，远端为大画面*/
#define	PIP_REMOTE_IN_LOCAL	     (u8)0x1 /*远端为小画面，本地为大画面*/
#define	PIP_LOCAL_ONLY		     (u8)0x2 /*仅显示本地画面*/
#define	PIP_REMOTE_ONLY		     (u8)0x3 /*仅显示远端画面*/
#define	PIP_LITTLE_PIC	         (u8)0x4 /*显示开机小画面*/
#define	PIP_LEFT_AND_RIGHT       (u8)0x5 /*左右两画面*/

#define  ASF_AUD_IDX			 (u8)0 /* asf文件中的音频序号 */
#define  ASF_VID_IDX			 (u8)1 /* asf文件中的视频序号 */

/*视频编码参数*/
typedef struct VideoEncParam
{
	u8  m_byEncType;   /*图像编码类型*/
    u8  m_byRcMode;    /*图像压缩码率控制参数*/
    u16  m_byMaxKeyFrameInterval;/*I帧间最大P帧数目*/
    u8  m_byMaxQuant;  /*最大量化参数(1-31)*/
    u8  m_byMinQuant;  /*最小量化参数(1-31)*/
    u8  m_byReserved1; /*保留*/
    u8  m_byReserved2; /*保留*/
    u8  m_byReserved3; /*保留*/
    u16  m_wBitRate;    /*编码比特率(Kbps)*/
    u16  m_wReserved4;  /*保留*/		
	u32 m_dwSndNetBand;/*网络发送带宽(单位:Kbps,1K=1024)*/   
    u32 m_dwReserved5; /*保留*/
#ifdef SOFT_CODEC
	u16 m_EncVideoWidth;//编码设置图像宽
	u16 m_EncVideoHeight;//编码设置图像高
	u8  m_byQcMode; /*图像质量码率控制参数,质量优先或者速度优先*/
    u8  m_byQI;
    u8  m_byQP;
#else
	u8    m_byPalNtsc;    /*图像制式(PAL或NTSC)*/	
	u8    m_byCapPort;    /*采集端口号: 1~7有效。在KDV8010上，S端子端口号为7；C端子端口号缺省为2*/
	u8	m_byFrameRate;  /*帧率(default:25)*/	
	u8    m_byImageQulity;/*图象压缩质量,0:速度优先;1:质量优先*/
	u8    m_byReserved6;  /*保留*/
	u16	m_wVideoWidth;  /*图像宽度(default:640)*/
	u16	m_wVideoHeight; /*图像高度(default:480)*/
#endif

	u8  m_byFrameFmt; // 分辨率 0:不固定类型, 1: half cif, 2: 1 cif, 3: 2 cif,  4: 4 cif,  5: 用于图像合成编码  
	u8	m_byFrmRateCanSet; //帧率是否由外界设定 ?
	u8  m_byFrmRateLittleThanOne; // 帧率是否小于1 ?
//	u8  m_byAutoDelFrm;			//编码器是否自动丢帧 ?不起作用，质量只由Quality决定
}TVideoEncParam;

/*vga编码参数*/
typedef struct VgaEncParam
{
	u8	m_byEncType;   /*图像编码类型*/
	u16 m_wBitRate;    /*编码比特率(Kbps)*/
	u32	m_dwFrameRate;  /*帧率(default:25)*/	
    u32	m_dwMaxKeyFrameInterval;/*I帧间最大P帧数目*/
	u8	m_byImageQulity; /*图象压缩质量(1-5)*/
    u8	m_byMaxQuant;  /*最大量化参数(1-31)*/
    u8	m_byMinQuant;  /*最小量化参数(1-31)*/
	u32 m_dwSndNetBand; /*网络发送带宽(单位:Kbps,1K=1024)*/   
	u8	m_byReserved1; /*保留*/
    u8	m_byReserved2; /*保留*/
    u8	m_byReserved3; /*保留*/
	u8	m_byReserved4; /*保留*/	
}TVgaEncParam;

/*vga编码状态*/
typedef struct VgaEncStatus
{
	u8 m_byVgaMode;		/* vga模式， 适配成功为 0--7之间， 不成功为 255 */
	TVgaEncParam m_tVgaEncParam;
}TVgaEncStatus;

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
	u16   LastVideoBitRate; //上次统计的码率
    u32   GetDecoderStatisnum;//码率不变则该值递增
	u32	  m_dwVidPackLoseNum;//视频丢包数，080905
	u16   m_wVidPackLoseRatio;//视频丢包率
}TKdvDecStatis; 

/* 图像解码参数结构 */
typedef struct
{
    u32 dwStartRecvBufs;         // 开始接收buffer数
    u32 dwFastRecvBufs;         // 快收buffer数
	u32 dwPostPrcLvl;			// mpv4后处理的级别		
}TVideoDecParam;

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

#ifdef HARD_CODEC
typedef struct VideoDisplayParam
{
	u8    m_byPIPMode;   /*画中画显示方式*/
	u8    m_bySaturation;/*显示饱和度*/
	u8    m_byContrast;  /*显示对比度*/
	u8    m_byBrightness;/*显示亮度*/
	u16    m_dwPIPXPos;   /*画中画X坐标*/
	u16    m_dwPIPYPos;   /*画中画Y坐标*/
	u16    m_dwPIPWidth;  /*画中画宽度*/
	u16    m_dwPIPHeight; /*画中画高度*/    
	u32  m_dwPlayMode;   //显示模式，1-PAL显示，2-NTSC显示，3-VGA显示
}TVideoDisplayParam;

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

typedef struct
{
	BOOL32  bSnapShot;  /*存快照 ?*/
	u32  dwSnapshotMaxSize;	/*快照存储的最大空间*/
    u32  dwSnapshotSaveSiz; /*已存快照的大小(byte)*/
    u32  dwSnapshotSavePicNum;/*已存快照的数目*/	
}TSnapShot;

typedef struct 
{
    BOOL32 bSnaping;    /* 是否正在快照 */
    s8 achSnapFileName[128];   /* 要保存的快照文件名 */
}TSnaptoFile;

typedef struct
{
    u32   dwIp;     //Ip地址(网络序) 
	u16    wPort;    //端口号(网络序) 
}TNetAddress;

/* 混音参数结构 */
typedef struct
{
	u32  dwType;          // 本通道类型：
    u32  dwVolume;       //音量
	u32  dwOutFequency;     // 声音输出频率: 32000/44100/48000
	u32  dwOutPower;   // 声音输出功率
	u32  dwOutMode; // 本通道的输出模式：0-立体声，1-单声道
	u32  dwOutBitRate; //　输出码率(kbps):　有效值为(32, 40, 48, 56, 64, 80, 96, 112, 128, 160, 192, 224, 256, 320)
}TMixerParam;

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

typedef void ( *TMotionAlarmInfo)(u32 dwPort,TMoveCBParam *ptMoveCBParam,u32 dwAreaNum,u32 dwContext);
typedef void ( *TDecodeVideoScaleInfo)(u16 wVideoWidth, u16 wVideoHeight);
typedef void ( *TDebugCallBack )(char *pbyBuf, int dwBufLen);

API void mapver(u32 dwMapId);

typedef struct 
{
	u32 dwCoreSpd; 
	u32 dwMemSpd;  
	u32 dwMemSize;  
	u32 dwMaxVidEncNum;  
	u32 dwMaxVidDecNum; 
	u32 dwMaxAudEncNum;
	u32 dwMaxAudDecNum; 
	BOOL32  bVgaOut; 
	u16  wRefreshRate; 
	u32  dwMapId;
}TMapCheckParam;

/* 视频采集调节 */ 
typedef struct
{
	u8 byBrt;			//亮度    默认值：128 
	u8 byCst;			//对比度  默认值：68 	
	u8 byStn;			//饱和度   默认值：64 
	u8 byHue;			//色度     默认值：0 
}TVideoCapScale;


#ifdef _VXWORKS_
void  GetDiskInfo();
#endif

#ifdef _LINUX_
API void CodecWrapperPreInit(void); /* 预初始化codecwrapper，应用起来后先完成BrdInit、ospInit，
                                       之后必须紧接着调用该函数以完成开机画面启动、调试函数的注册 */

#endif

#endif

#endif





















