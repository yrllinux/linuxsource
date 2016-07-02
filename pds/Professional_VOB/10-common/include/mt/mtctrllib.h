/*****************************************************************************
   模块名      : MTCONTROL
   文件名      : MtCtrlLib.h
   相关文件    : 
   文件实现功能: MT业务控制3.0接口文件
   作者        : 邓辉
   版本        : V3.0  Copyright(C) 2003-2005 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   2003/08/20  3.0         邓辉        创建
******************************************************************************/
#ifndef _MT_CORE_H
#define _MT_CORE_H

#include "kdvtype.h"
//#include "kdvdef.h"
#include "mteventdef.h"
#include "mtconstdef.h"
#include "vccommon.h"
#include "time.h"


//返回消息的类型
enum ReturnMsgType
{
	Ret_DevInit,
	Ret_CamCtrl,
	Ret_MatrixCtrl,
	Ret_GkRegister,
	Ret_MakeCall,
	Ret_AnswerCall,
	Ret_HangUp,
	Ret_DropTer,
	Ret_ApplySpeak,
	Ret_BroadCastTer,
	Ret_ViewThisTer,
	Ret_StartPoll,
	Ret_StopPoll,
	Ret_MakeMeChair,
	Ret_ReleaseChair,
	Ret_ConfEnd,
	Ret_SelfTest,
};

typedef enum
{
	ITT_MT_ConfState = 1,		//会议状态			| IT_CONFSTAT
	ITT_MT_PollState,			//轮循状态			| IT_POLLSTAT
	ITT_MT_EncodeStat,			//编码状态			| IT_ENCSTAT
	ITT_MT_DecodeStat,			//解码状态			| IT_DECSTAT
	ITT_MT_CameraStat,			//摄像头状态		| IT_CAMERASTAT
	ITT_MT_MatrixStat,			//矩阵状态			| IT_MATRIXSTAT
	ITT_MT_TerCfg,				//基本信息配置		| _TTERCFG
	ITT_MT_NetCfg,				//网络配置			| _TNETCFG
	ITT_MT_CallCfg,				//呼叫配置			| _TCALLCFG
	ITT_MT_AudioCfg,			//音频配置			| _TAENCPARAM + _TADECPARAM
	ITT_MT_VideoCfg,			//视频配置			| _TVENCPARAM + _TVDECPARAM
	ITT_MT_CameraCfg,			//摄像头配置		| _TDEVCFG+设备号(1u8:1-4)
	ITT_MT_MatrixCfg,			//矩阵配置			| _TDEVCFG
	ITT_MT_OsdCfg,				//显示配置			| _TOSDCFG	
	ITT_MT_LastSms,				//最后一次短消息内容| 字符串
	ITT_MT_CODECSTATE,			//Codec状态			| IT_CODECSTATE
	ITT_MT_Reboot,				//重启动终端
	ITT_MT_ADDRBOOK_NUM,		//地址簿条目和组的个数	| IT_ADDRBOOK
	ITT_MT_ADDRBOOK_FIND_ENTRY,	//查找地址簿条目		| IT_ADDRBOOK + TADDRENTRY	
	ITT_MT_ADDRBOOK_FIND_GROUP,	//查找地址簿组			| IT_ADDRBOOK + TADDRMULTISETENTRY

		
	ITT_MC_GetState,			//获取MC状态				|无
	ITT_MC_IsInConf,			//判断是某终端否在会议中	|u32(ip) 
	ITT_MC_IsLableInConf,		//指定的终端编号是否在会议中
	ITT_MC_GetFloorer,			//获取当前发言人
	ITT_MC_GetChair,			//获取当前主席
	IIT_MC_GetConfInfo,			//获取当前会议信息
	ITT_MC_GetConfMode,			//获得当前会议模式
	ITT_MC_GetApplyingChair,	//获得正在申请主席的终端
	ITT_MC_GetApplyingFloorer,	//获得正在申请发言的终端
	ITT_MC_GetToBeFloorer,		//获得将成为发言人的终端
	ITT_MC_GetSrcLabel,			//获得视频源终端编号

}InterTestType;

enum NodeType
{
	node_all,
	node_ui,
	node_mtc,
	node_mcu,
};

enum AddrType
{
	addr_ip = 1,
	addr_e164,
	addr_h323id,
	addr_email
};

enum AddrMissType
{
	miss_ipandmask = 1,
	miss_mac = 2,
	miss_all = 3,
};
//==================================================================================
//									函数接口
//==================================================================================


API void mtctrlhelp();
API void mtctrlver();

extern "C" 
{

/*====================================================================
    函数名      : InitMt
    功能        : 初始化MtCtrlLib
    输入参数说明: wGuiAppId		- 上层GUI APP号
				  dwCodecAppId	- Codec APP号
				  configDir		- 配置文件路径(最后必须以"/"结束!)
    返回值说明  : 0 - 成功 1 - 配置文件初始化失败 2 - 创建MTC监听节点失败				
====================================================================*/
s32 InitMt(u16 wGuiAppId,u32 dwCodecAppId,const char* configDir = "./conf/",
		   const u16 listenPort = MT_CONNECT_PORT, const u16 wCallingPort = 1720 );

/*====================================================================
    函数名      : CloseMt
    功能        : 释放MtCtrlLib
    输入参数说明: 无
    返回值说明  : 无
====================================================================*/
void CloseMt();

#ifdef WIN32
#define KDV8010_BOARD               100    /* KDV8010终端 */
#define KDV8018_BOARD               101    /* KDV8018终端 */
#define KDV8010A_BOARD              102    /* KDV8010A终端 */
#define KDV8010B_BOARD              103    /* KDV8010B终端 */
#define KDV8010C_BOARD              104    /* KDV8010C终端 */
#define KDV8010D_BOARD              105    /* KDV8010D终端 */
#define KDV8010E_BOARD              106    /* KDV8010E终端 */
#define DSL8000_BRD_IMT				7       /* IMT板 */

u8 BrdGetBoardID();

#endif // WIN32

BOOL GetMtFilePath(const s8* sFileName/*IN*/,s8* sPath /*OUT*/);

BOOL AddrIsMca(u32 dwIp);
BOOL AddrIsBca(u32 dwIp);
}

//==================================================================================
//									消息结构定义
//==================================================================================
#ifdef WIN32
#pragma pack(1)
#define PACKED 
#else
#define PACKED __attribute__((__packed__))	// 取消编译器的优化对齐
#endif // WIN32

//协议地址
typedef struct tagIpAddr
{
	u32 ip;					//网络序
	u16 port;				
	u8  type;
}PACKED TIPADDR,*PTIPADDR;

//基本会议信息
typedef struct tagConferenceInfo
{
	s8	 achConfId[CONF_ID_LEN + 1 ];			// 会议ID
	s8	 achConfName[CONF_NAME_LEN + 1 ];		// 会议名
	s8	 achConfNumber[ CONF_E164_LEN + 1 ];	// 会议号码
	s8	 achConfPwd[ MAX_PWD_LEN + 1 ];			// 会议密码
	u16  wConfDuration;							// 会议持续时间		
	u8   byVideoFormat;							// 会议视频格式(VIDEO_H261等)
	u8	 byAudioFormat;							// 会议音频格式(AUDIO_MP3等)
	u8   byResolution;							// 会议视频分辨率(VIDEO_CIF等)
	u8   byAutoVMP;								// 是否自动画面合成(MT_ENABLE/MT_DISABLE)
	u8   byAutoMix;								// 是否混音(MT_ENABLE/MT_DISABLE)
}PACKED _TCONFINFO,*_PTCONFINFO;


//本地信息配置
typedef struct  tagTerCfg
{
	s8	achTerName[MT_NAME_LEN+1];			//终端别名
	s8	achE164Name[MT_E164_LEN+1];			//终端E164号码
	u8	byProType;							//终端协议类型
}PACKED _TTERCFG,*_PTTERCFG;

//Ethernet地址
typedef struct tagLanAddr
{
	u8  byIndex;	//网卡号(win32填0,vxWorks填0-3)
	u32 dwIP;		//IP地址(网络序)
	u32 dwMask;		//网络掩码(网络序)
	u8  byMacAddr[6];//MAC地址
}PACKED _TLANADDR,*_PTLANADDR;

// Qos
typedef struct tagQosInfo
{
	u8  byQosType;   //Service or IP first
	// Service first param.
	u8  bySrvAudio;
	u8  bySrvVideo;
	u8  bySrvData;
	u8  bySrvSignaling;
	// Ip first param.
	u8  byIpAudio;
	u8  byIpVideo;
	u8  byIpData;
	u8  byIpSignaling;
	u8  byTos;
}PACKED TQosInfo,*PTQosInfo;

//网络信息配置
typedef struct  tagNetCfg
{
	_TLANADDR	tLocalAddr;	//本地地址 
	u32		dwGWAddr;			//网关地址
	u16		wSendInitPort;		//发送起始端口
	u16		wRecvInitPort;		//接收起始端口
	u8		byNetType;			//网络类型(局域网,广域网,ADSL,ISDN)
	u32		dwUpBandWidth;		//上行带宽(kbps)
	u32		dwDownBandWidth;	//下行带宽(kbps)
	TQosInfo tQosInfo;
}PACKED _TNETCFG,*_PTNETCFG;

// 网络管理信息配置
typedef struct tagSnmpCfg
{
	u32 dwTrapAddr;		// 接收TRAP的地址(网络序)
	s8  achCommunity[ MT_COMM_LEN + 1 ];// 共同体名
}PACKED _TSNMPCFG,*_PTSNMPCFG;

//设备信息配置
typedef struct tagDeviceCfg
{
	u8  byDevNo;	//设备号(最高BIT(no.7)1:不使用,0:使用,剩余BIT为设备ID)
	u8  byType;		//设备类型
	u32 dwIpAddr;	//设备IP(网络序)
	u16 wPort;		//设备端口(如为串口设备,表示串口号)
	u8  byCameraAddr;	// 摄像头地址
	u8  byCameraSpeed;	// 摄像头速度
}PACKED _TDEVCFG,*_PTDEVCFG;

//串口信息配置
typedef struct tagSeriesCfg
{
	u8  byType;  // 串口协议
	u32 dwBaudRate;	//波特率
	u8  byByteSize;	//数据位
	u8  byParity;		//奇偶检验
	u8  byStopBits;	//停止位
}PACKED _TSERCFG, *_PTSERCFG;

//显示信息配置
typedef struct tagOsdCfg
{
	u8	byShowLabel;		// 是否显示台标		MT_ENABLE:开启    MT_DISABLE: 关闭
	u8  byLabelType;		// 台标类型 :		MT_LABEL_DEF:系统默认  MT_LABEL_USER: 用户自定义
	u8	byEncLabel;			// 是否显示状态		MT_ENABLE:开启	  MT_DISABLE: 关闭
	u8	byPIPMode;			// 画中画模式		MT_AUTO:自动开启  MT_MANUAL: 手动开启
}PACKED _TOSDCFG,*_PTOSDCFG;

//呼叫信息配置
typedef struct tagCallCfg
{
	u8	byAnswerMode;		//应答方式(MT_AUTO/MT_MANUAL/MT_REJECT)
	u8	byCallMode;			//呼叫方式(MT_AUTO/MT_MANUAL)
	u8	byAdHocMode;		//多点会议扩展方式(MT_AUTO/MT_MANUAL/MT_REJECT)
	u16	wTtl;				//roundtrip时间间隔
	u8  byUseGk;			//是否使用GK(MT_ENABLE/MT_DISABLE)
	u32	dwGKAddr;			//网守地址(网络序)
	u32	dwAutoCallIpAddr;	//自动呼叫的IP(网络序)	
	u16 wAutoCallRate;		//自动呼叫速率
	u8  byIsEncrypt;		//是否加密(ENCRYPT_NONE/ENCRYPT_AUTO)
	u8	byEncryptMode;		//加密模式(ENCRYPT_AUTO/ENCRYPT_DES/ENCRYPT_AES)
	u8  byIsH239;			//是否使用H239(DUAL_H239_NONE/DUAL_H239_AUTO)
}PACKED _TCALLCFG,*_PTCALLCFG;

// 视频输出
typedef struct tagVideoOut
{
	u8	byOutType;		//输出类型(VIDEO_S|VIDEO_VGA)
	u16 wVgaFs;			//VGA频率
	u8	byDvSource;		//双流输出源(MT_LOCAL:本地|MT_REMOTE:网络)
}PACKED _TVOUT,*_PTVOUT;

//视频编码参数
typedef struct tagVideoEncParam
{
	u8	vSource;			//视频源(VIDEO_COMP3等)
	u8	vFormat;			//视频格式(VIDEO_H261等)
	u8	vSystem;			//视频制式(VIDEO_PAL/VIDEO_NTSC)
	u8	vResolution;		//视频分辨率(VIDEO_CIF等)
	u8  vAutoFrame;			//是否是自动帧率(MT_ENALBE/MT_DISABLE)
	u8	vFrameRate;			//视频帧率
	u8	vFrameRateUnit;		//帧率单位(VIDEO_FPS:帧/每秒 VIDEO_SPF:秒/每帧)
	u16	vBitRate;			//视频码率(kbps)
	u16 vBandWidth;			//最大网络带宽(kbps)
	u8	vCompress;			//图像压缩质量(IMAGE_QUALITY_PRI/IMAGE_SPEED_PRI)
	u16	vIKeyRate;			//最大关键帧间隔
	u8	vQualMaxValue;		//图像压缩最大量化参数
	u8	vQualMinValue;		//图像压缩最小量化参数
	u16 wH264IKeyRate;		//h264的关键帧间隔
	u8  byH264QualMaxValue;	//h264最大量化参数
	u8  byH264QualMinValue;	//h264最小量化参数
}PACKED _TVENCPARAM,*_PTVENCPARAM;

//音频编码参数
typedef struct tagAudioEncParam
{
	u8	aSource;		//音频源(LINE/MIC)
	u8  aFormat;		//音频格式(g711-56k/g711-64k/g723.1/g728/g729/mp3)
	u16 aBitRate;		//音频码率(kbps)
	u8  aSampleRate;	//音频采样率(kbps)
	u8	aVolumn;		//编码音量
}PACKED _TAENCPARAM,*_PTAENCPARAM;

//视频解码参数
typedef struct tatVideoDecParam
{
	u8  byDecompress;		//图像解码质量(IMAGE_QUALITY_BETTER等)
	u8	vFormat;		//视频格式
	u8	vOverlay;		//是否使能Overlay
	u8	vMpeg4BPV;		//Mpeg4后处理值(0-4)
	u16 vBitRate;		//视频码率(kbps)
}PACKED _TVDECPARAM,*_PTVDECPARAM;

//音频解码参数
typedef struct tagAudioDecParam
{
	u8  aFormat;		//音频格式
	u8	aVolumn;		//解码音量
}PACKED _TADECPARAM,*_PTADECPARAM;

//AEC参数
typedef struct tagAec
{
	u8	bAecEnable;		//是否使用AEC(MT_ENABLE|MT_DISABLE)
	u8	bAGC;			//是否自动增益(MT_ENABLE|MT_DISABLE)
	u8	byPolicy;		//方案(1或2)
}PACKED _TAEC,*_PTAEC;

// 图像质量参数
typedef struct tagImageQuality
{
	u8 byType;			// 图像质量类型(饱和度,亮度,对比度,所有)
	u8 byBright;		// 亮度参数值(0~255)
	u8 byContrast;		// 对比度参数值(0~127)
	u8 bySaturation;	// 饱和度参数值(0~127)
}PACKED _TIMAGEQUAL,*_PTIMAGEQUAL;

//发送重传配置
typedef struct tagSendRtCfg
{
	u8	bRetransmit;	//是否重传(MT_ENABLE|MT_DISABLE)
	u16 wTimeOut;		//缓冲时间(ms)
}PACKED _TSENDRTCFG,*_PTSENDRTCFG;

//接收重传配置
typedef struct tagRecvCfg
{
	u8	bRetransmit;	//是否重传(MT_ENABLE|MT_DISABLE)
	u16 wCheckPoint1;	//检测点1(ms)
	u16 wCheckPoint2;	//检测点2(ms)
	u16 wCheckPoint3;	//检测点3(ms)
	u16 wTimeOut;		//丢包超时时间(ms)	
}PACKED _TRECVRTCFG,*_PTRECVRTCFG;

//矩阵端口信息
typedef struct tagMatrixPort
{
	u8 byPort;							//端口号
	u8 byImageId;						//图片编号
	s8 achName[MATRIX_PORTNAME_LEN+1];	//端口名称
}PACKED _TMATRIXPORT,*_PTMATRIXPORT;

//矩阵信息
typedef struct tagMatrixInfo
{
	u8  byPortNum;								//端口个数
	_TMATRIXPORT atInPort[MATRIX_MAX_PORT];		//输入端口
	_TMATRIXPORT atOutPort[MATRIX_MAX_PORT];	//输出端口
	u8  abyConfig[MATRIX_MAX_PORT];				//矩阵配置关系(索引为输出端口)
}PACKED _TMATRIXINFO,*_PTMATRIXINFO;

//矩阵配置方案
typedef struct tagMtMatrixScheme
{
	u8	byType;										// 矩阵类型	
	u8	bySchemeId;									// 方案号
	s8	achVideoSchName[ MATRIX_SCHNAME_LEN + 1 ];	// 视频矩阵方案名称
	_TMATRIXINFO atVideo;							// 视频矩阵方案配置
	s8  achAudioSchName[ MATRIX_SCHNAME_LEN + 1 ];	// 音频矩阵方案名称
	_TMATRIXINFO atAudio;							// 音频矩阵方案配置
}PACKED _TMATRIXSCHEME,*_PTMATRIXSCHEME;

// 快照信息
typedef struct tagSnapInfo
{
	u8	byState;		// 获取结果(成功MT_OK/失败MT_ERR)
	u32 dwMaxSize;		// 最大空间大小(字节)
	u32 dwUsedSize;		// 已用空间大小(字节)
	u32	dwSnapPicNum;	// 快照图片个数(个)
}PACKED _TSNAPINFO,*_PTSNAPINFO;


//终端地址
typedef struct tagTerAddr
{
	u8			type;							//地址类型
	u32			int_addr;						//(IP,网络序)
	s8			str_addr[MT_NAME_LEN+1];		//(别名)
}PACKED _TERADDR,*_PTERADDR;

//呼叫参数
typedef struct tagDialParam
{
	u8			byCallType;			//呼叫类型:JOIN|CREATE|INVITE
	u8			byWorkMode;			//工作模式(单播/多播,H320)
	u16			wCallRate;			//呼叫速率(kbps)
	_TERADDR	callingAddr;		//主叫地址(主叫时可不填)
	_TERADDR	calledAddr;			//被叫地址

	//////////////////////////////////////////////////////////////////////////
	//以下仅用于创建会议
	u8			byCreateConf;		//是否是创建会议(MT_ENABLE/MT_DISABLE)
	_TCONFINFO  tConfInfo;			//会议信息
	u8			byTerNum;			//被邀终端个数
	_TERADDR	atList[CONF_MT_CAPACITY/4];	//被邀终端列表,目前最大为32	
}PACKED _TDIALPARAM,*_PTDIALPARAM;

// 终端编解码码率
typedef struct tagBitRate
{
	u8 byId;
	// 实时编解码码率
	u16 wSendBitRate;
	u16 wRecvBitRate;
	// 一分钟之内的平均码率
	u16 wSendAVBitRate;
	u16 wRecvAVBitRate;
}PACKED _TBITRATE,*_PTBITRATE;

// codec网络统计信息
typedef struct tagCodecStatistic
{
	u8  byDecoderId;
	u16 wVideoBitRate;		// 视频解码码率
	u16 wAudioBitRate;		// 音频解码码率
	u32 dwVideoRecvFrm;		// 收到的视频帧数
	u32 dwAudioRecvFrm;		// 收到的音频帧数
	u16 wVideoLoseRatio;	// 视频丢包率
	u16 wAudioLoseRatio;	// 音频丢包率
	u32 dwVideoLoseFrm;		// 视频总丢报数
	u32 dwAudioLoseFrm;		// 音频总丢报数
}PACKED _TCODECSTATISTIC,*_PTCODECSTATISTIC;

// codec解码信息
typedef struct tagDecodeInfo
{
	u8 byDecoderId;			// 解码器ID
	u8 byVideoType;			// 视频解码类型
	u8 byVideoRes;			// 视频解码分辨率
	u8 byAudioType;			// 音频解码类型
}PACKED _TDECODEINFO,*_PTDECODEINFO;

// 音频功率
typedef struct tagAudioPower
{
	u8	byDecoderId;
	u32 dwOutputPower;
	u32 dwInputPower;
}PACKED _TAUDIOPOWER,*_PTAUDIOPOWER;

// MAP状态
typedef struct tagMapStatus
{
	u8	byId;		// MAP No.
	u8	byStatus;	// 0(正常)/1(异常)
	u16	wDownNum;	// 异常次数
}PACKED _TMAPSTATUS,*_PTMAPSTATUS;

// 图像测试
typedef struct tagImageTest
{
	u8 bySrc;		// 图像源(本地MT_LOCAL/远端MT_REMOTE)
	u8 byType;		// 测试类型(分辨率RESOLUTION_TEST/色带COLOR_TEST/灰度GRAY_TEST)
	u8 byOnOff;		// 开关(MT_ENABLE/MT_DISABLE)
}PACKED _TIMAGETEST,*_PTIMAGETEST;

// 监控模式切换
typedef struct tagMonitorSwitch
{
	u8  byOnOff;		// 开关(MT_ENABLE/MT_DISABLE)
	u16 wTimeOut;		// 切换时间(单位:分钟)
}PACKED _TMONITORSWITCH,*_PTMONITORSWITCH;

// 动态载荷设置结构
typedef struct tagDynamicPT
{
	u8 byMediaType;				// 媒体类型(MT_AUDIO/MT_VIDEO)
	u8 byPayloadType;			// 媒体格式
	u8 byDynamicPayloadType;	// 动态载荷值
	u8 byDecoderId;				// 解码器ID
}PACKED _TDYNAMICPT,*_PTDYNAMICPT;

// 加密/解密结构
typedef struct tagEncrypt
{
	u8	byIndex;					// 设备ID
	u8	byMediaType;				// 媒体类型(MT_AUDIO/MT_VIDEO)
	u8  byDynamicPayloadType;		// 动态载荷值
	u8	byMode;						// 加密模式(ENCRYPT_DES/ENCRYPT_AES)
	u8  abyKey[ MAX_H235KEY_LEN ];	// 密钥内容
	int nKeyLen;					// 密钥长度(以字节为单位)
}PACKED _TENDECRYPT,*_PTENDECRYPT;

// 双流操作结构
typedef struct tagDualStream
{
	u8		bySource;		// [REQ]		双流源(DUAL_VGA/DUAL_PC)
	u8		byOnOff;		// [REQ/RSP]	开关(MT_START/MT_STOP)
	u8		byResult;		// [RSP]		操作结果(MT_OK/MT_ERR)
	TIPADDR tRecvAddr;		// [RSP]		终端接收地址(仅对PC源)
	u8		byReason;		// [RSP]		双流操作失败的原因
}PACKED _TDUALSTREAM,*_PTDUALSTREAM;

// 终端状态
typedef struct tagTerStatus
{
	///////会议状态///////
	u8		bOnline;					//是否在会议中	1 yes 0 no
	u8		byConfMode;					//会议模式		1 两点 2 多点		
	u8		byPollStat;					//轮询状态		1 开始 2 暂停 3 停止
	u8		byCallMode;					//呼叫方式		1 GK路由 0 直接呼叫
	u8		bRegGkStat;					//GK注册结果	1 成功 0 失败
	u8		byEncryptConf;				//加密状态		1 加密 0 不加密(只是表示本端发送是否加密)
	
	///////设备状态///////
	u8		byMtBoardType;				//终端板卡类型	0: WIN,1:8010,2:8010a,3:8018,4:IMT,5:8010C
	u8		bMatrixStatus;				//矩阵状态		(1 ok 0 err)
	u8		byCamCtrlSrc;				//当前控制摄像头源类型(0:本地,1:远端)
	u8		byLocalCamCtrlNo;			//当前控制的本地摄像头号(1-6号摄像头)
	u8		byRemoteCamCtrlNo;			//当前控制的远端摄像头号(1-6号摄像头)
	u8		byCamNum;					//摄像头个数	(0~6)
	u8		bCamStatus[CAMERA_MAX_NUM];	//摄像头状态 (1 ok 0 err)
	u8		byVideoSrc;					//当前本地视频源(0:S端子,1-6:C端子)
	u8		bRemoteCtrl;				//是否允许远遥	1 yes 0 no

	//////编解码状态////////
	u8		byAEncFormat;				//音频编码格式(AUDIO_G711A,AUDIO_MP3...)
	u8		byVEncFormat;				//视频编码格式(VIDEO_H261,VIDEO_H263...)
	u8		byVEncRes;					//视频编码分辨率(VIDEO_QCIF,VIDEO_CIF...)
	u8      byEncryptMode;				//编码使用的加密算法(ENCRYPT_AES/ENCRYPT_DES/ENCRYPT_NONE)
	u8		byADecFormat;				//音频解码格式(AUDIO_G711A,AUDIO_MP3...)
	u8		byVDecFormat;				//视频解码格式(VIDEO_H261,VIDEO_H263...)
	u8		byVDecRes;					//视频解码分辨率(VIDEO_QCIF,VIDEO_CIF...)
	u8		byDecryptMode;				//解码使用的加密算法(ENCRYPT_AES/ENCRYPT_DES/ENCRYPT_NONE)
	u16		wSendBitRate[2];			//视频编码码率(单位:kbps)
	u16		wRecvBitRate[2];			//视频解码码率(单位:kbps)
	u16		wSendAVBitRate;				//平均编码码率(单位:kbps)
	u16		wRecvAVBitRate;				//平均解码码率(单位:kbps)
	u8		byEncVol;					//当前编码音量(单位:等级)
	u8		bMute;						//是否哑音			1 yes 0 no
	u8		byDecVol;					//当前解码音量(单位:等级)
	u8		bQuiet;						//是否静音			1 yes 0 no
	u8		bLocalLoop;					//是否自环			1 yes 0 no
	u8		byAudioPower;				//是否在测试音频	1 yes 0 no
	u8		byLocalImageTest;			//是否本地图像测试1 yes 0 no
	u8		byRmtImageTest;				//是否远端图像测试 1 yes 0 no
	u16		wSysSleep;					//待机时间(0xFFFF表示不待机)

	//////配置状态///////

}PACKED _TTERSTATUS,*_PTTERSTATUS;

//UI操作提示
typedef struct tagMtNote
{
	u8	byMsgId;			//消息ID (ReturnMsgType)
	s8	achNote[64];		//描述
}PACKED _TMTNOTE,*_PTMTNOTE;


//broadcast panel config
typedef struct tagPanelCfg
{
	u8	 byEnable;			// 启动开关MT_ENABLE/MT_DISABLE
	char achName[ MT_PATH_LEN ];	// 频道名称
	u32	 dwIp;				// 组播地址（网络序）
	u16  wPort;				// 组播端口号
	char achPwd[ MT_PATH_LEN ];		// 流媒体密码
}PACKED _TPANELCFG,*_PTPANELCFG;

// 地址结构
typedef struct tagProtocolAddr 
{
	TIPADDR  tLocalAddr;	//本端地址
	TIPADDR  tRemoteAddr;	//对端地址
}PACKED TPROADDR,*PTPROADDR;

// 自环结构
typedef struct tagSelfTest
{
	u8	byMediaType;	// 媒体类型(MT_VIDEO/MT_AUDIO/MT_AV)
	u8	byOnOff;		// 开关(MT_START/MT_STOP)
	u32 dwIp;			// 默认0:表示127.0.0.1,否则指定IP
}PACKED _TSELFTEST,*_PTSELFTEST;

// 字幕结构
typedef struct tagTitleCfg
{
	u8		byEnable;			// 使能(MT_ENABLE/MT_DISABLE)
	u32		dwTextClr;			// 文字颜色
	u32		dwBkgClr;			// 背景色
	u8		byTextAlign;		// 文字对齐模式(TEXT_ALIGN_LEFT/TEXT_ALIGN_CENTER/TEXT_ALIGN_RIGHT)
	u8		byRollMode;			// 滚动模式(ROLL_NOT_ROLL/ROLL_RIGHT_LEFT/ROLL_DOWN_UP)
	u8		byTransParent;		// 是否透明显示(MT_ENABLE/MT_DISABLE)
	u8		byRollSpeed;		// 滚动速度(ROLL_SPEED_SLOW/ROLL_SPEED_NORMAL/ROLL_SPEED_FAST/ROLL_SPEED_FASTER)
	u8		byRollNum;			// 滚动次数(0:表示无限制滚动,1~255:表示用户指定的滚动次数)
	u8		byStayTime;			// 停留时间(0:表示不停留,1~255:表示用户指定的时间,单位秒)
	u32		dwStartXPos;		// 字幕显示的起始X坐标
	u32		dwStartYPos;		// 字幕显示的起始Y坐标
	u32		dwDisplayWidth;		// 字幕显示区域的宽(以象素为单位)
	u32		dwDisplayHeight;	// 字幕显示区域的高(指行高,以象素为单位)
	char	achWinInfo[ 256 ];	// 用于存放Windows的相关信息(以\0结束)
}PACKED _TTITLECFG,*_PTTITLECFG;

// 轮询结构
typedef struct tagMtPoll
{
	u8			byMediaType;		// 轮询媒体格式( MT_AUDIO/MT_VIDEO/MT_AV )
	u8			byIntTime;			// 轮询间隔时间(单位秒)
	u8			byMtNum;			// 参加轮询的终端个数,0表示所有与会终端(单位个,最多CTRL_POLL_MAXNUM)
	TMTLABEL	atMtLabel[ CTRL_POLL_MAXNUM ]; // 参加轮询的终端<m,t>
}PACKED _TMTPOLL,*_PTMTPOLL;

// 呼叫链路状态结构
typedef struct tagLinkState
{
	u8			byLinkState;					// 链路状态(CALL_LINKING,CALL_LINKED,CALL_UNLINK)
	u32			dwIpAddr;						// 对端IP地址(网络序)
	char		achAlias[ MT_DISPLAY_LEN + 1 ]; // 对端别名
	u8			byOtherInfo;					// 其他信息:
												// 对于LINKING表示主叫还是被叫(MT_CALLING/MT_CALLED
												// 对于UNLINK,表示断链原因(DISCONNECT_REASON_BUSY/..)
}PACKED _TLINKSTATE,*_PTLINKSTATE;

// 终端分区信息结构
typedef struct tagMtDiskInfo
{
	char		achSysName[ MT_PATH_LEN + 1 ];		// 系统分区名称
	char		achDataName[ MT_PATH_LEN + 1 ];		// 数据分区名称
}PACKED _TMTDISKINFO,*_PTMTDISKINFO;

// 解码器转发结构
typedef struct tagStreamForward
{
	u8			byDecoderId;			// 解码器ID
	u8			byMediaType;			// 媒体类型(MT_VIDEO/MT_AUDIO)
	u16			wBitRate;				// 转发编码码率
	TPROADDR	tRtpAddr;				// 转发目的RTP地址
	TPROADDR	tRtcpAddr;				// 转发目的RTCP地址
}PACKED _TSTREAMFWD,*_PTSTREAMFWD;

// 当前登陆用户信息
typedef struct tagLoginUserInfo
{
	char		achUserName[ MT_COMM_LEN + 1 ];	// 登陆用户名
	u32			dwUserIp;						// 登陆用户IP(网络序)
}PACKED _TLOGINUSERINFO,*_PTLOGINUSERINFO;

/////////////////////Tony For Net Manager//////////////////////
typedef struct tagNetModule
{
	u8  byEthNum;
	u8  byE1Num;
}PACKED TNetModuleInfo, *PTNetModuleInfo;

/* 路由参数结构 */
typedef struct tagIpRouteParam{
	u8  byUsed;          // 该条目是否使用
    u32 dwDesIpNet;      /* 目的子网	*/
    u32 dwDesIpMask;     /* 掩码		*/
    u32 dwGwIpAdrs;      /* 网关ip地址 */
    u32 dwRoutePri;		 /* 路由优先级，0为默认 */
}PACKED TIpRouteParam, *PTIpRouteParam;

typedef struct tagAllRouteInfo{
    u32 dwIpRouteNum;    /* 得到的实际路由个数 */
    TIpRouteParam tIpRouteParam[ROUTE_MAX_NUM];//IP_ROUTE_MAX_NUM];
}PACKED TAllIpRouteInfo, *PTAllIpRouteInfo;

// E1模块参数
typedef struct tagE1Module
{
  u32 dwUsedFlag;        /*使用标志,置1使用，0不使用 */
  u32 dwE1TSMask;        /* E1时隙分配说明 */
  u32 dwProtocolType;    /* 接口协议封装类型,如果是单E1连接可以指定PPP/HDLC，如果是多E1捆绑连接必须是PPP协议 */
  u32 dwEchoInterval;    /* 对应serial同步接口的echo请求时间间隔，秒为单位，有效范围0-256，默认填2 */
  u32 dwEchoMaxRetry;    /* 对应serial同步接口的echo最大重发次数，有效范围0-256，默认填2 */
}PACKED TE1ModuleInfo, *PTE1ModuleInfo;

// E1通道参数
typedef struct tagE1ChanInfo
{

	// Single & Multi Chan param.
    u8  byNumModuleUsed;           //  E1 Num Use By User.
	u8  byIsBind;                  //  是否绑定
    u32 dwIpAdrs;                  //  ip地址
    u32 dwIpMask;                  //  掩码，

	// Multi Chan param.
    u32 dwAuthenticationType;      //  PPP链接的验证方法PAP/CHAP,默认填MP_AUTHENTICATION_PAP */
    u32 dwFragMinPackageLen;       //  最小分片包长，字节为单位，范围20~1500，默认填20 
    u8   chSvrUsrName[E1_NAME_MAXLEN]; 	/* 服务端用户名称，用来验证对端的数据 */
    u8   chSvrUsrPwd[E1_NAME_MAXLEN];   	/* 服务端用户密码，用来验证对端的数据 */
    u8   chSentUsrName[E1_NAME_MAXLEN];	/* 客户端用户名称，用来被对端验证 */
    u8   chSentUsrPwd[E1_NAME_MAXLEN]; 	/* 客户端用户密码，用来被对端验证 */

	// E1 Module Param.
    TE1ModuleInfo tE1ModuleInfo[ E1_MODULE_MAXNUM ];//E1_SINGLE_LINK_CHAN_MAX_NUM];/* 每个被捆绑的serial同步接口参数 */

}PACKED TE1ChanInfo, *PTE1ChanInfo;

// Embeded Pxy Sys
typedef struct tagPxyInfo
{
	 u8   byPxyUsed;         //  防火墙使用标志
     u32  dwPxySrvIp;        //  代理服务端Ip
     u16  wPxySrvPort;       //  代理服务端Port
     u16  wMediaPort;        //  码流端口
     u8   byPPPoeUsed;       //  PPPoe启用标志

}PACKED TPxyInfo, *PTPxyInfo;

typedef struct tagVideoParam // notify video param.
{
	u8	bIsEncrypt;		  //是否加密
	u8	byVLocalMode;	  //视频加密算法(发送)
	u8  byVLocalKeyLen;   //
	u8  byVLocalKey[LEN_H235KEY_MAX];
	u8	byVRemoteMode;	  //视频解密算法(接收)
	u8  byVRemoteKeyLen;  //
	u8  byVRemoteKey[LEN_H235KEY_MAX];

	u8	byALocalMode;	  //视频加密算法(发送)
	u8  byALocalKeyLen;   //
	u8  byALocalKey[LEN_H235KEY_MAX];
	u8	byARemoteMode;	  //视频解密算法(接收)
	u8  byARemoteKeyLen;  //
	u8  byARemoteKey[LEN_H235KEY_MAX];
	
	u8  byVLocalType;     //本端视频类型 
	u8  byVLocalRealType; //本端视频实际类型
	u8  byVRemoteType;    //远端视频类型
	u8  byVRemoteRealType;//远端视频实际类型

}PACKED TVideoParam, *PTVideoParam;

/////////////////////just use for windows////////////////////

//windows下的采集参数
typedef struct
{
	u8	byCaptureType;	//采集类型
	u8	byCaptFormat;	//采集格式
	u8	bOverlay;		//是否使用overlay MT_ENABLE | MT_DISABLE
	s8	sCaptFileName[MT_PATH_LEN+1];
}_TPCCODEC,*_PTPCCODEC;

typedef struct {
	s8	name[MT_NAME_LEN + 1];		//用户名
	s8	password[MT_PWD_LEN+1];	//密码
}_TUSERREG,*_PTUSERREG;

#ifdef WIN32
#pragma pack()
#endif


//==================================================================================
//									类定义
//==================================================================================
//句柄结构
typedef struct tagHandle
{
	u32		dwHsCall;				//呼叫句柄
	u32		dwHsChanOrRas;			//信道或者RAS句柄
	void*	pAppCall;				//应用呼叫指针
	void*	pAppChanOrRas;			//应用信道或RAS指针
	tagHandle():dwHsCall(NULL),dwHsChanOrRas(NULL),pAppCall(NULL),pAppChanOrRas(NULL) {}
}PACKED THANDLE,*PTHANDLE;


//业务消息结构
typedef struct tagMtMsg 
{
	u32 dwHeader;				//消息头标识
	u16	wMsgLen;				//整个消息长度
	u16 wBodyLen;				//内容长度
	u8  byMsgType;				//消息类型
	u8  bySubType;				//子类型
	u8	byDirection;			//消息方向
	u8  byNodeType;				//用于指定发向某个类型的UI(node_all,node_ui,node_mtc)
	u16 wSrcId;					//消息源ID
	u16	wMsgId;					//消息ID
	THANDLE tHandle;				//内部消息使用
	s8	achBody[MTMSG_CONTENT_LEN];	//消息内容
}PACKED TMTMSG,*PTMTMSG;



////////////////终端业务消息///////////////////
class CMtMsg
{
public:
	CMtMsg()
	{ 
		memset(&m_tMsg,0,sizeof(TMTMSG)); 
		m_tMsg.dwHeader = htonl(MTMSG_HEADER);
		m_tMsg.wMsgLen  = htons(MTMSG_HEADER_LEN);
	}
	CMtMsg(PTMTMSG ptMsg,BOOL bNetOrder)
	{
		memset(&m_tMsg,0,sizeof(TMTMSG));
		if (ptMsg != NULL)
		{
			if (bNetOrder)
			{
				//长度先转换为主机序
				u16 wMsgLen;
				wMsgLen = ntohs(ptMsg->wMsgLen);
				if (wMsgLen < sizeof(TMTMSG))
					memcpy(&m_tMsg,ptMsg,wMsgLen);
			}
			else
			{
				memcpy(&m_tMsg,ptMsg,ptMsg->wMsgLen);
				HostToNet();
			}
		}
	}
	//填写内部消息句柄
	void SetHandle(PTHANDLE ptHandle)
	{
		if (ptHandle != NULL)
		{
			m_tMsg.tHandle.dwHsCall = htonl(ptHandle->dwHsCall);
			m_tMsg.tHandle.dwHsChanOrRas = htonl(ptHandle->dwHsChanOrRas);
			m_tMsg.tHandle.pAppCall = (void*)htonl((u32)ptHandle->pAppCall);
			m_tMsg.tHandle.pAppChanOrRas = (void*)htonl((u32)ptHandle->pAppChanOrRas);
		}
		//memcpy(&m_tMsg.tHandle,ptHandle,sizeof(THANDLE));
	}
	//填写消息内容
	void SetBody(const void* pBody,u16 wLen)
	{
		if (pBody != NULL && wLen <= MTMSG_CONTENT_LEN)	
		{
			memcpy(m_tMsg.achBody,pBody,wLen);
			m_tMsg.wBodyLen = htons(wLen);
			m_tMsg.wMsgLen = htons(MTMSG_HEADER_LEN + wLen);
		}
	}
	void SetDirection(u8 byDirect) { m_tMsg.byDirection = byDirect; }
	void SetNodeType( NodeType node ) { m_tMsg.byNodeType = node; }
	void SetEvent(u16 wEvent)
	{
		//系统消息
		if (wEvent > EV_MTSYS_BGN && wEvent < EV_MTSYS_END)
		{
			m_tMsg.byMsgType = MT_SYS;
		}
		//内部消息
		if (wEvent > EV_INNER_BGN && wEvent < EV_INNER_END)
		{
			m_tMsg.byMsgType = MT_INNER;
			if (wEvent > INNER_CALL_ENTRY && wEvent < INNER_CHAN_ENTRY)	//呼叫
				m_tMsg.bySubType = INNER_CALL;
			if (wEvent > INNER_CHAN_ENTRY && wEvent < INNER_CTRL_ENTRY) //信道
				m_tMsg.bySubType = INNER_CHAN;
			if (wEvent > INNER_CTRL_ENTRY && wEvent < INNER_RAS_ENTRY)  //会议控制
				m_tMsg.bySubType = INNER_CTRL;
			if (wEvent > INNER_RAS_ENTRY && wEvent < INNER_MISC_ENTRY)  //RAS
				m_tMsg.bySubType = INNER_RAS;
			if (wEvent > INNER_MISC_ENTRY && wEvent < INNER_MC_ENTRY)	//内部MISC消息
				m_tMsg.bySubType = INNER_MISC;
			if (wEvent > INNER_MC_ENTRY && wEvent < INNER_H320_ENTRY)	//内部MC消息
				m_tMsg.bySubType = INNER_MC;
			if ( wEvent > INNER_H320_ENTRY &&wEvent < EV_INNER_END )	//H320消息
				m_tMsg.bySubType = INNER_H320;

		}
		//外部消息
		if (wEvent > EV_OUTER_BGN && wEvent < EV_OUTER_END)
		{
			m_tMsg.byMsgType = MT_OUTER;
			if (wEvent > OUTER_CONF_ENTRY && wEvent < OUTER_MATR_ENTRY) //会议控制消息
				m_tMsg.bySubType = OUTER_CONFCTRL;
			if (wEvent > OUTER_MATR_ENTRY && wEvent < OUTER_CAM_ENTRY)	//视频切换矩阵
				m_tMsg.bySubType = OUTER_MATRIX;
			if (wEvent > OUTER_CAM_ENTRY && wEvent < OUTER_CODEC_ENTRY)	//摄像头
				m_tMsg.bySubType = OUTER_CAMERA;
			if (wEvent > OUTER_CODEC_ENTRY && wEvent < OUTER_CFG_ENTRY)	//编解码
				m_tMsg.bySubType = OUTER_CODEC;
			if (wEvent > OUTER_CFG_ENTRY && wEvent < OUTER_MISC_ENTRY)	//OSD
				m_tMsg.bySubType = OUTER_CFG;
			if (wEvent > OUTER_MISC_ENTRY && wEvent < EV_OUTER_END)		//外部MISC消息
				m_tMsg.bySubType = OUTER_MISC;
		}
		m_tMsg.wMsgId = htons(wEvent);
	}
	//获得消息ID
	u16	GetMsgId()	{ 	return ntohs(m_tMsg.wMsgId); 	}	
	//获得消息方向
	u8	GetMsgDirect()	{	return m_tMsg.byDirection;	}
	//获得消息类型
	u8	GetMsgType()	{   return m_tMsg.byMsgType;	}
	//获得消息子类型
	u8	GetMsgSubType() {   return m_tMsg.bySubType;	}
	// 获取消息来源
	NodeType GetNodeType() { return (NodeType)m_tMsg.byNodeType; }
	//获得整个消息(网络序)
	PTMTMSG GetMsg() 
	{
		return &m_tMsg; 
	}
	//获得整个消息(主机序)
	void FetchHostMsg(TMTMSG &tMsg)
	{
		memset(&tMsg,0,sizeof(tMsg));
		u16 wMsgLen = ntohs(m_tMsg.wMsgLen);
		memcpy(&tMsg,&m_tMsg,sizeof(tMsg));
		tMsg.wMsgLen = wMsgLen;
		tMsg.dwHeader = ntohl(tMsg.dwHeader);
		tMsg.wMsgId = ntohs(tMsg.wMsgId);
		tMsg.wBodyLen = ntohs(tMsg.wBodyLen);
		tMsg.wSrcId = ntohs(tMsg.wSrcId);
		tMsg.tHandle.dwHsCall = ntohl(tMsg.tHandle.dwHsCall);
		tMsg.tHandle.dwHsChanOrRas = ntohl(tMsg.tHandle.dwHsChanOrRas);
		tMsg.tHandle.pAppCall = (void*)ntohl((u32)tMsg.tHandle.pAppCall);
		tMsg.tHandle.pAppChanOrRas = (void*)ntohl((u32)tMsg.tHandle.pAppChanOrRas);
	}

	//获得整个消息长度
	u16    GetMsgLen() 
	{ 
		return ntohs(m_tMsg.wMsgLen); 
	}
	//获得消息体
	s8*	GetBody()	
	{ 
		return m_tMsg.achBody;
	}
	//获得消息体长度
	u16	GetBodyLen() 
	{ 
		return ntohs(m_tMsg.wBodyLen); 
	}

	//消息头序列转换为主机序
	void    NetToHost()
	{
		m_tMsg.dwHeader = ntohl(m_tMsg.dwHeader);
		m_tMsg.wMsgId = ntohs(m_tMsg.wMsgId);
		m_tMsg.wMsgLen = ntohs(m_tMsg.wMsgLen);
		m_tMsg.wBodyLen = ntohs(m_tMsg.wBodyLen);
		m_tMsg.wSrcId = ntohs(m_tMsg.wSrcId);
		m_tMsg.tHandle.dwHsCall = ntohl(m_tMsg.tHandle.dwHsCall);
		m_tMsg.tHandle.dwHsChanOrRas = ntohl(m_tMsg.tHandle.dwHsChanOrRas);
		m_tMsg.tHandle.pAppCall = (void*)ntohl((u32)m_tMsg.tHandle.pAppCall);
		m_tMsg.tHandle.pAppChanOrRas = (void*)ntohl((u32)m_tMsg.tHandle.pAppChanOrRas);
	}
	//消息头序列转换为网络序
	void	HostToNet()
	{
		m_tMsg.dwHeader = htonl(m_tMsg.dwHeader);
		m_tMsg.wMsgId = htons(m_tMsg.wMsgId);
		m_tMsg.wMsgLen = htons(m_tMsg.wMsgLen);
		m_tMsg.wBodyLen = htons(m_tMsg.wBodyLen);
		m_tMsg.wSrcId = htons(m_tMsg.wSrcId);
		m_tMsg.tHandle.dwHsCall = htonl(m_tMsg.tHandle.dwHsCall);
		m_tMsg.tHandle.dwHsChanOrRas = htonl(m_tMsg.tHandle.dwHsChanOrRas);
		m_tMsg.tHandle.pAppCall = (void*)htonl((u32)m_tMsg.tHandle.pAppCall);
		m_tMsg.tHandle.pAppChanOrRas = (void*)htonl((u32)m_tMsg.tHandle.pAppChanOrRas);
	}
protected:
	TMTMSG	m_tMsg;			//消息内容
};

//字节序列转换结构,当MTMSG的消息内容为结构类型时,CStructAdapter提供字节
//序列的转换功能
class CStructAdapter
{
public:
	enum StructType
	{
		ST_NONE,		//无效结构类型
		ST_TERCFG,		//本地信息配置
		ST_NETCFG,		//网络配置
		ST_DEVCFG,		//设备配置
		ST_OSDCFG,		//显示配置
		ST_CALLCFG,		//呼叫配置
		ST_VIDEOENC,	//视频编码	
		ST_AUDIOENC,	//音频编码
		ST_VIDEODEC,	//视频解码
		ST_AUDIODEC,	//音频解码	
		ST_DIALPARAM,	//呼叫参数
		ST_TERSTATUS,	//终端状态
		ST_NETADDR,		//网络地址
		ST_VIDEOOUT,	//视频输出	
		ST_SENDRT,		//发送重传
		ST_RECVRT,		//接收重传
		ST_SNAPINFO,	//快照信息
		ST_BITRATE,		//编解码码率上报
		ST_STATISTIC,	//编解码统计上报
		ST_PANEL,		//流媒体
		ST_MNTSWT,		//监控切换模式
		ST_AUDIOPOWER,	//音频功率上报
		ST_SERIAL,		// 串口
		ST_DUALSTREAM,	// 双流
		ST_TITLE,		// 字幕

		ST_ROUTE,       // 单Route 
		ST_ROUTE_TABLE, // 整个Route表
		ST_E1,
		ST_PXY,
		ST_VIDEOPARAM

	};
public:
	CStructAdapter() 
	{
		m_pStruct = NULL;
		m_eType = ST_NONE;
	}
	CStructAdapter(void* pStruct,StructType eType) 
	{
		m_pStruct = pStruct;
		m_eType = eType;
	}

	//设置入口参数
	void SetStruct(void* pStruct,StructType eType)
	{
		m_pStruct = pStruct;
		m_eType = eType;
	}
	//主机序->网络序
	void HostToNet()
	{
		if (m_pStruct!=NULL && m_eType!=ST_NONE)
		{
			switch(m_eType) 
			{
			case ST_CALLCFG:		//呼叫配置
				{
					_PTCALLCFG ptCall = (_PTCALLCFG)m_pStruct;
					ptCall->wTtl = htons(ptCall->wTtl);
					ptCall->wAutoCallRate = htons( ptCall->wAutoCallRate );
				}
				break;

			case ST_NETCFG:			//网络配置
				{
					_PTNETCFG ptNet = (_PTNETCFG)m_pStruct;
					ptNet->wSendInitPort = htons(ptNet->wSendInitPort);
					ptNet->wRecvInitPort = htons(ptNet->wRecvInitPort);
					ptNet->dwUpBandWidth = htonl(ptNet->dwUpBandWidth);
					ptNet->dwDownBandWidth = htonl(ptNet->dwDownBandWidth);
				}
				break;

			case ST_DEVCFG:			//设备配置
				{
					_PTDEVCFG ptDev = (_PTDEVCFG)m_pStruct;
					//ptDev->dwIpAddr = htonl(ptDev->dwIpAddr);
					ptDev->wPort = htons(ptDev->wPort);
				}
				break;
			
			case ST_VIDEOENC:		//视频编码
				{
					_PTVENCPARAM ptVEnc = (_PTVENCPARAM)m_pStruct;
					ptVEnc->vBitRate = htons(ptVEnc->vBitRate);
					ptVEnc->vBandWidth = htons(ptVEnc->vBandWidth);
					ptVEnc->vIKeyRate = htons( ptVEnc->vIKeyRate );
					ptVEnc->wH264IKeyRate = htons( ptVEnc->wH264IKeyRate );
				}
				break;

			case ST_AUDIOENC:		//音频编码
				{
					_PTAENCPARAM ptAEnc = (_PTAENCPARAM)m_pStruct;
					ptAEnc->aBitRate = htons(ptAEnc->aBitRate );
				}
				break;

			case ST_VIDEODEC :		// 视频解码
				{
					_PTVDECPARAM ptVDec = ( _PTVDECPARAM )m_pStruct;
					ptVDec->vBitRate = htons( ptVDec->vBitRate );
				}
				break;

			case ST_TERSTATUS:
				{
					_PTTERSTATUS ptStatus = (_PTTERSTATUS)m_pStruct;
					ptStatus->wSendBitRate[0] = htons(ptStatus->wSendBitRate[0]);
					ptStatus->wRecvBitRate[0] = htons(ptStatus->wRecvBitRate[0]);
					ptStatus->wSendBitRate[1] = htons(ptStatus->wSendBitRate[1]);
					ptStatus->wRecvBitRate[1] = htons(ptStatus->wRecvBitRate[1]);
					ptStatus->wSendAVBitRate = htons( ptStatus->wSendAVBitRate );
					ptStatus->wRecvAVBitRate = htons( ptStatus->wRecvAVBitRate );
					ptStatus->wSysSleep = htons( ptStatus->wSysSleep );
				}
				break;

			case ST_NETADDR:		//网络地址
				{
					PTIPADDR pAddr = (PTIPADDR)m_pStruct;
					pAddr->port = htons(pAddr->port);
				}
				break;

			case ST_VIDEOOUT:		//视频输出
				{
					_PTVOUT ptVout = (_PTVOUT)m_pStruct;
					ptVout->wVgaFs = htons(ptVout->wVgaFs);
				}
				break;

			case ST_SENDRT:		//发送重传	
				{
					_PTSENDRTCFG ptSrt = (_PTSENDRTCFG)m_pStruct;
					ptSrt->wTimeOut = htons(ptSrt->wTimeOut);
				}
				break;

			case ST_RECVRT:		//接收重传
				{
					_PTRECVRTCFG ptRrt = (_PTRECVRTCFG)m_pStruct;
					ptRrt->wCheckPoint1 = htons(ptRrt->wCheckPoint1);
					ptRrt->wCheckPoint2 = htons(ptRrt->wCheckPoint2);
					ptRrt->wCheckPoint3 = htons(ptRrt->wCheckPoint3);
					ptRrt->wTimeOut = htons(ptRrt->wTimeOut);
				}
				break;

			case ST_DIALPARAM :	// 呼叫参数
				{
					_PTDIALPARAM ptDialParam = ( _PTDIALPARAM )m_pStruct;
					ptDialParam->wCallRate = htons( ptDialParam->wCallRate );
					ptDialParam->tConfInfo.wConfDuration = htons( ptDialParam->tConfInfo.wConfDuration );
				}
				break;

			case ST_SNAPINFO : // 快照信息
				{
					_PTSNAPINFO ptSnapInfo = ( _PTSNAPINFO )m_pStruct;
					ptSnapInfo->dwMaxSize = htonl( ptSnapInfo->dwMaxSize );
					ptSnapInfo->dwUsedSize = htonl( ptSnapInfo->dwUsedSize );
					ptSnapInfo->dwSnapPicNum = htonl( ptSnapInfo->dwSnapPicNum );
				}
				break;

			case ST_BITRATE : // 编解码码率上报
				{
					_PTBITRATE ptBitRate = ( _PTBITRATE )m_pStruct;
					ptBitRate->wSendBitRate = htons( ptBitRate->wSendBitRate );
					ptBitRate->wRecvBitRate = htons( ptBitRate->wRecvBitRate );
					ptBitRate->wSendAVBitRate = htons( ptBitRate->wSendAVBitRate );
					ptBitRate->wRecvAVBitRate = htons( ptBitRate->wRecvAVBitRate );
				}
				break;

			case ST_PANEL :		// 流媒体
				{
					_PTPANELCFG ptPanelCfg = ( _PTPANELCFG )m_pStruct;
					ptPanelCfg->wPort = htons( ptPanelCfg->wPort );

				}
				break;

			case ST_STATISTIC :			// codec统计信息上报
				{
					_PTCODECSTATISTIC ptCodecStat = ( _PTCODECSTATISTIC )m_pStruct;
					ptCodecStat->wVideoBitRate = htons( ptCodecStat->wVideoBitRate );
					ptCodecStat->wAudioBitRate = htons( ptCodecStat->wAudioBitRate );
					ptCodecStat->wVideoLoseRatio = htons( ptCodecStat->wVideoLoseRatio );
					ptCodecStat->wAudioLoseRatio = htons( ptCodecStat->wAudioLoseRatio );
					ptCodecStat->dwVideoRecvFrm = htonl( ptCodecStat->dwVideoRecvFrm );
					ptCodecStat->dwAudioRecvFrm = htonl( ptCodecStat->dwAudioRecvFrm );
					ptCodecStat->dwVideoLoseFrm = htonl( ptCodecStat->dwVideoLoseFrm );
					ptCodecStat->dwAudioLoseFrm = htonl( ptCodecStat->dwAudioLoseFrm );
				}
				break;

			case ST_AUDIOPOWER :		// 音频功率上报
				{
					_PTAUDIOPOWER ptAudioPwr = ( _PTAUDIOPOWER )m_pStruct;
					ptAudioPwr->dwOutputPower = htonl( ptAudioPwr->dwOutputPower );
					ptAudioPwr->dwInputPower = htonl( ptAudioPwr->dwInputPower );
				}
				break;

			case ST_MNTSWT :	// 监控切换模式
				{
					_PTMONITORSWITCH ptMntSwt = ( _PTMONITORSWITCH )m_pStruct;
					ptMntSwt->wTimeOut = htons( ptMntSwt->wTimeOut );
				}
				break;

			case ST_SERIAL :		// 串口
				{
					_PTSERCFG ptSerialCfg = ( _PTSERCFG )m_pStruct;
					ptSerialCfg->dwBaudRate = htonl( ptSerialCfg->dwBaudRate );
				}
				break;

			case ST_DUALSTREAM :	// 双流设置
				{
					_PTDUALSTREAM ptDualStr = ( _PTDUALSTREAM )m_pStruct;
					ptDualStr->tRecvAddr.port = htons( ptDualStr->tRecvAddr.port );
				}
				break;

			case ST_TITLE :		// 字幕
				{
					_PTTITLECFG ptTitleCfg = ( _PTTITLECFG )m_pStruct;
					ptTitleCfg->dwBkgClr = htonl( ptTitleCfg->dwBkgClr );
					ptTitleCfg->dwTextClr = htonl( ptTitleCfg->dwTextClr );
					ptTitleCfg->dwStartXPos = htonl( ptTitleCfg->dwStartXPos );
					ptTitleCfg->dwStartYPos = htonl( ptTitleCfg->dwStartYPos );
					ptTitleCfg->dwDisplayHeight = htonl( ptTitleCfg->dwDisplayHeight );
					ptTitleCfg->dwDisplayWidth = htonl( ptTitleCfg->dwDisplayWidth );
				}
				break;

			case ST_ROUTE:
				{
					PTIpRouteParam ptIpRoute = (PTIpRouteParam)m_pStruct;
					ptIpRoute->dwDesIpMask = htonl(ptIpRoute->dwDesIpMask);
					ptIpRoute->dwDesIpNet  = htonl(ptIpRoute->dwDesIpNet);
					ptIpRoute->dwGwIpAdrs  = htonl(ptIpRoute->dwGwIpAdrs);
					ptIpRoute->dwRoutePri  = htonl(ptIpRoute->dwRoutePri);

				}
				break;

			case ST_ROUTE_TABLE:
				{
					PTAllIpRouteInfo ptRoute = ( PTAllIpRouteInfo )m_pStruct;
					ptRoute->dwIpRouteNum = htonl(ptRoute->dwIpRouteNum);

					PTIpRouteParam ptRouteParam = NULL;
					for ( u8 byIndex = 0; byIndex < ROUTE_MAX_NUM; byIndex++ )
					{
						ptRouteParam = &ptRoute->tIpRouteParam[byIndex];
						if (ptRouteParam->byUsed)
						{
							ptRouteParam->dwDesIpMask = htonl(ptRouteParam->dwDesIpMask);
							ptRouteParam->dwDesIpNet  = htonl(ptRouteParam->dwDesIpNet);
							ptRouteParam->dwGwIpAdrs  = htonl(ptRouteParam->dwGwIpAdrs);
							ptRouteParam->dwRoutePri  = htonl(ptRouteParam->dwRoutePri);
						}
					}
				}

				break;
			case ST_E1:
				{
					PTE1ChanInfo ptE1Chan = (PTE1ChanInfo)m_pStruct;
					ptE1Chan->dwAuthenticationType = htonl(ptE1Chan->dwAuthenticationType);
					ptE1Chan->dwFragMinPackageLen  = htonl(ptE1Chan->dwFragMinPackageLen);
					ptE1Chan->dwIpAdrs             = htonl(ptE1Chan->dwIpAdrs);
					ptE1Chan->dwIpMask             = htonl(ptE1Chan->dwIpMask);

					PTE1ModuleInfo ptE1Module = NULL;
					for ( u8 byIndex = 0; byIndex < E1_MODULE_MAXNUM; byIndex++)
					{
						ptE1Module = &ptE1Chan->tE1ModuleInfo[byIndex];
						ptE1Module->dwE1TSMask = htonl(ptE1Module->dwE1TSMask);
						ptE1Module->dwEchoInterval = htonl(ptE1Module->dwEchoInterval);
						ptE1Module->dwEchoMaxRetry = htonl(ptE1Module->dwEchoMaxRetry);
						ptE1Module->dwProtocolType = htonl(ptE1Module->dwProtocolType);
						ptE1Module->dwUsedFlag = htonl(ptE1Module->dwUsedFlag);
					}

				}
				break;

			case ST_PXY:
				{
					PTPxyInfo ptPxy = (PTPxyInfo)m_pStruct;
					ptPxy->dwPxySrvIp = htonl(ptPxy->dwPxySrvIp);
					ptPxy->wPxySrvPort= htons(ptPxy->wPxySrvPort);
					ptPxy->wMediaPort = htons(ptPxy->wMediaPort);
				}
			default:
				break;
			}
		}
		m_eType = ST_NONE;
		m_pStruct = NULL;
	}

	//网络序->主机序
	void NetToHost()
	{
		if (m_pStruct!=NULL && m_eType!=ST_NONE)
		{
			switch(m_eType) 
			{
			case ST_CALLCFG:		//呼叫配置
				{
					_PTCALLCFG ptCall = (_PTCALLCFG)m_pStruct;
					ptCall->wTtl = ntohs(ptCall->wTtl);
					ptCall->wAutoCallRate = ntohs( ptCall->wAutoCallRate );
				}
				break;

			case ST_NETCFG:			//网络配置
				{
					_PTNETCFG ptNet = (_PTNETCFG)m_pStruct;
					ptNet->wSendInitPort = ntohs(ptNet->wSendInitPort);
					ptNet->wRecvInitPort = ntohs(ptNet->wRecvInitPort);
					ptNet->dwUpBandWidth = ntohl(ptNet->dwUpBandWidth);
					ptNet->dwDownBandWidth = ntohl(ptNet->dwDownBandWidth);
				}
				break;

			case ST_DEVCFG:			//设备配置
				{
					_PTDEVCFG ptDev = (_PTDEVCFG)m_pStruct;
					//ptDev->dwIpAddr = ntohl(ptDev->dwIpAddr);
					ptDev->wPort = ntohs(ptDev->wPort);
				}
				break;


			case ST_VIDEOENC:		//视频编码
				{
					_PTVENCPARAM ptVEnc = (_PTVENCPARAM)m_pStruct;
					ptVEnc->vBitRate = ntohs(ptVEnc->vBitRate);
					ptVEnc->vBandWidth = ntohs(ptVEnc->vBandWidth);
					ptVEnc->vIKeyRate = ntohs( ptVEnc->vIKeyRate );
					ptVEnc->wH264IKeyRate = ntohs( ptVEnc->wH264IKeyRate );
				}
				break;

			case ST_AUDIOENC:		//音频编码
				{
					_PTAENCPARAM ptAEnc = (_PTAENCPARAM)m_pStruct;
					ptAEnc->aBitRate = ntohs(ptAEnc->aBitRate );
				}
				break;

			case ST_VIDEODEC :		// 视频解码
				{
					_PTVDECPARAM ptVDec = ( _PTVDECPARAM )m_pStruct;
					ptVDec->vBitRate = ntohs( ptVDec->vBitRate );
				}
				break;


			case ST_TERSTATUS:
				{
					_PTTERSTATUS ptStatus = (_PTTERSTATUS)m_pStruct;
					ptStatus->wSendBitRate[0] = ntohs(ptStatus->wSendBitRate[0]);
					ptStatus->wRecvBitRate[0] = ntohs(ptStatus->wRecvBitRate[0]);
					ptStatus->wSendBitRate[1] = ntohs(ptStatus->wSendBitRate[1]);
					ptStatus->wRecvBitRate[1] = ntohs(ptStatus->wRecvBitRate[1]);
					ptStatus->wSendAVBitRate = ntohs( ptStatus->wSendAVBitRate );
					ptStatus->wRecvAVBitRate = ntohs( ptStatus->wRecvAVBitRate );
					ptStatus->wSysSleep = ntohs( ptStatus->wSysSleep );
				}
				break;

			case ST_NETADDR:		//网络地址
				{
					PTIPADDR pAddr = (PTIPADDR)m_pStruct;
					pAddr->port = ntohs(pAddr->port);
				}
				break;
				
			case ST_VIDEOOUT:		//视频输出
				{
					_PTVOUT ptVout = (_PTVOUT)m_pStruct;
					ptVout->wVgaFs = ntohs(ptVout->wVgaFs);
				}
				break;

			case ST_SENDRT:		//发送重传	
				{
					_PTSENDRTCFG ptSrt = (_PTSENDRTCFG)m_pStruct;
					ptSrt->wTimeOut = ntohs(ptSrt->wTimeOut);
				}
				break;

			case ST_RECVRT:		//接收重传
				{
					_PTRECVRTCFG ptRrt = (_PTRECVRTCFG)m_pStruct;
					ptRrt->wCheckPoint1 = ntohs(ptRrt->wCheckPoint1);
					ptRrt->wCheckPoint2 = ntohs(ptRrt->wCheckPoint2);
					ptRrt->wCheckPoint3 = ntohs(ptRrt->wCheckPoint3);
					ptRrt->wTimeOut = ntohs(ptRrt->wTimeOut);
				}
				break;

			case ST_DIALPARAM :	// 呼叫参数
				{
					_PTDIALPARAM ptDialParam = ( _PTDIALPARAM )m_pStruct;
					ptDialParam->wCallRate = ntohs( ptDialParam->wCallRate );
					ptDialParam->tConfInfo.wConfDuration = ntohs( ptDialParam->tConfInfo.wConfDuration );
				}
				break;

			case ST_SNAPINFO : // 快照信息
				{
					_PTSNAPINFO ptSnapInfo = ( _PTSNAPINFO )m_pStruct;
					ptSnapInfo->dwMaxSize = ntohl( ptSnapInfo->dwMaxSize );
					ptSnapInfo->dwUsedSize = ntohl( ptSnapInfo->dwUsedSize );
					ptSnapInfo->dwSnapPicNum = ntohl( ptSnapInfo->dwSnapPicNum );
				}
				break;

			case ST_BITRATE : // 编解码码率上报
				{
					_PTBITRATE ptBitRate = ( _PTBITRATE )m_pStruct;
					ptBitRate->wSendBitRate = ntohs( ptBitRate->wSendBitRate );
					ptBitRate->wRecvBitRate = ntohs( ptBitRate->wRecvBitRate );
					ptBitRate->wSendAVBitRate = ntohs( ptBitRate->wSendAVBitRate );
					ptBitRate->wRecvAVBitRate = ntohs( ptBitRate->wRecvAVBitRate );
				}
				break;

			case ST_PANEL :		// 流媒体
				{
					_PTPANELCFG ptPanelCfg = ( _PTPANELCFG )m_pStruct;
					ptPanelCfg->wPort = ntohs( ptPanelCfg->wPort );
				}
				break;

			case ST_STATISTIC :			// codec统计信息上报
				{
					_PTCODECSTATISTIC ptCodecStat = ( _PTCODECSTATISTIC )m_pStruct;
					ptCodecStat->wVideoBitRate = ntohs( ptCodecStat->wVideoBitRate );
					ptCodecStat->wAudioBitRate = ntohs( ptCodecStat->wAudioBitRate );
					ptCodecStat->wVideoLoseRatio = ntohs( ptCodecStat->wVideoLoseRatio );
					ptCodecStat->wAudioLoseRatio = ntohs( ptCodecStat->wAudioLoseRatio );
					ptCodecStat->dwVideoRecvFrm = ntohl( ptCodecStat->dwVideoRecvFrm );
					ptCodecStat->dwAudioRecvFrm = ntohl( ptCodecStat->dwAudioRecvFrm );
					ptCodecStat->dwVideoLoseFrm = ntohl( ptCodecStat->dwVideoLoseFrm );
					ptCodecStat->dwAudioLoseFrm = ntohl( ptCodecStat->dwAudioLoseFrm );
				}
				break;
				
			case ST_AUDIOPOWER :		// 音频功率上报
				{
					_PTAUDIOPOWER ptAudioPwr = ( _PTAUDIOPOWER )m_pStruct;
					ptAudioPwr->dwOutputPower = ntohl( ptAudioPwr->dwOutputPower );
					ptAudioPwr->dwInputPower = ntohl( ptAudioPwr->dwInputPower );
				}
				break;

			case ST_MNTSWT :	// 监控切换模式
				{
					_PTMONITORSWITCH ptMntSwt = ( _PTMONITORSWITCH )m_pStruct;
					ptMntSwt->wTimeOut = ntohs( ptMntSwt->wTimeOut );
				}
				break;

			case ST_SERIAL :	// 串口
				{
					_PTSERCFG ptSerialCfg = ( _PTSERCFG )m_pStruct;
					ptSerialCfg->dwBaudRate = ntohl( ptSerialCfg->dwBaudRate );
				}
				break;

			case ST_DUALSTREAM :	// 双流设置
				{
					_PTDUALSTREAM ptDualStr = ( _PTDUALSTREAM )m_pStruct;
					ptDualStr->tRecvAddr.port = ntohs( ptDualStr->tRecvAddr.port );
				}
				break;

			case ST_TITLE :			// 字幕
				{
					_PTTITLECFG ptTitleCfg = ( _PTTITLECFG )m_pStruct;
					ptTitleCfg->dwBkgClr = ntohl( ptTitleCfg->dwBkgClr );
					ptTitleCfg->dwTextClr = ntohl( ptTitleCfg->dwTextClr );
					ptTitleCfg->dwStartXPos = ntohl( ptTitleCfg->dwStartXPos );
					ptTitleCfg->dwStartYPos = ntohl( ptTitleCfg->dwStartYPos );
					ptTitleCfg->dwDisplayHeight = ntohl( ptTitleCfg->dwDisplayHeight );
					ptTitleCfg->dwDisplayWidth = ntohl( ptTitleCfg->dwDisplayWidth );
				}
				break;

			case ST_E1:
				{
					PTE1ChanInfo ptE1Chan = (PTE1ChanInfo)m_pStruct;
					ptE1Chan->dwAuthenticationType = ntohl(ptE1Chan->dwAuthenticationType);
					ptE1Chan->dwFragMinPackageLen  = ntohl(ptE1Chan->dwFragMinPackageLen);
					ptE1Chan->dwIpAdrs             = ntohl(ptE1Chan->dwIpAdrs);
					ptE1Chan->dwIpMask             = ntohl(ptE1Chan->dwIpMask);

					PTE1ModuleInfo ptE1Module = NULL;
					for ( u8 byIndex = 0; byIndex < E1_MODULE_MAXNUM; byIndex++)
					{
						ptE1Module = &ptE1Chan->tE1ModuleInfo[byIndex];
						ptE1Module->dwE1TSMask = ntohl(ptE1Module->dwE1TSMask);
						ptE1Module->dwEchoInterval = ntohl(ptE1Module->dwEchoInterval);
						ptE1Module->dwEchoMaxRetry = ntohl(ptE1Module->dwEchoMaxRetry);
						ptE1Module->dwProtocolType = ntohl(ptE1Module->dwProtocolType);
						ptE1Module->dwUsedFlag = ntohl(ptE1Module->dwUsedFlag);
					}
					
				}
				break;
			case ST_ROUTE:
				{
					PTIpRouteParam ptRouteParam = ( PTIpRouteParam )m_pStruct;
					ptRouteParam->dwDesIpMask = ntohl(ptRouteParam->dwDesIpMask);
					ptRouteParam->dwDesIpNet  = ntohl(ptRouteParam->dwDesIpNet);
					ptRouteParam->dwGwIpAdrs  = ntohl(ptRouteParam->dwGwIpAdrs);
					ptRouteParam->dwRoutePri  = ntohl(ptRouteParam->dwRoutePri);
				}
				break;

			case ST_ROUTE_TABLE:
				{
					PTAllIpRouteInfo ptRoute = ( PTAllIpRouteInfo )m_pStruct;
					ptRoute->dwIpRouteNum = ntohl(ptRoute->dwIpRouteNum);

					PTIpRouteParam ptRouteParam = NULL;
					for ( u8 byIndex = 0; byIndex < ROUTE_MAX_NUM; byIndex++ )
					{
						ptRouteParam = &ptRoute->tIpRouteParam[byIndex];
						if (ptRouteParam->byUsed)
						{
							ptRouteParam->dwDesIpMask = ntohl(ptRouteParam->dwDesIpMask);
							ptRouteParam->dwDesIpNet  = ntohl(ptRouteParam->dwDesIpNet);
							ptRouteParam->dwGwIpAdrs  = ntohl(ptRouteParam->dwGwIpAdrs);
							ptRouteParam->dwRoutePri  = ntohl(ptRouteParam->dwRoutePri);
						}
					}
				}
				break;

			case ST_PXY:
				{
					PTPxyInfo ptPxy = (PTPxyInfo)m_pStruct;
					ptPxy->dwPxySrvIp = ntohl(ptPxy->dwPxySrvIp);
					ptPxy->wPxySrvPort= ntohs(ptPxy->wPxySrvPort);
					ptPxy->wMediaPort = ntohs(ptPxy->wMediaPort);
				}
			default:
				break;
			}
		}
		m_eType = ST_NONE;
		m_pStruct = NULL;
	}

protected:
	void*		m_pStruct;
	StructType  m_eType;
};

//////////////////////////////////////////////////////////////////////////
// 大型文件传输协议消息头定义
typedef struct tagFileTransfer
{
public :
	// 设置版本号
	void SetVersion( u16 wVer ) { wVersion = htons( wVer ) ; }
	u16	 GetVersion()	{ return ntohs( wVersion ); }
	
	// 设置协议编号
	void SetProtocolNo( u16 wNo ) { wProtocolNo = htons( wNo ); }
	u16  GetProtocolNo()	{ return ntohs( wProtocolNo ); }
	
	// 设置消息类型
	void SetMsgType( u8 byMsgType ) { byType = byMsgType; }
	u8	 GetMsgType()	{ return byType; }

	// 设置原因
	void SetReason( u8 byReason ) { byNackReason = byReason; }
	u8   GetReason() { return byNackReason; }

	// 设置用户数据区长度
	void SetDataLen( u16 wLen ) { wDataLen = htons( wLen ); }
	u16  GetDataLen() { return ntohs( wDataLen ); }

	void Reset()
	{
		SetVersion( MTC_MT_FILE_VER );
		SetProtocolNo( MTC_MT_MSG_BGN );
		byType = 0;
		byNackReason = 0;
		wDataLen = 0;
	}

private :
	u16		wVersion;			// 版本号		0x0100
	u16		wProtocolNo;		// 协议编号		0开始编号
	u8		byType;				// 消息类型		台标,滚动消息,字幕
	u8		byNackReason;		// 原因
	u16		wDataLen;			// 用户数据长度
}PACKED _TFILETRANSFER,*_PTFILETRANSFER;

// MT打印
API void MtPrintf( BOOL bScreen, BOOL bFile, char* format, ... );

//==================================================================================
//									调试宏定义
//==================================================================================
#define TRACE_CALLIN( lev, fun, str ) \
	if ( lev > 0 ) \
	MtPrintf( TRUE, FALSE, "[TRACE]Calling \"%s\" in file \"%s\"on Ln%d(%s)\n", fun, __FILE__, __LINE__, str );

#define TRACE_CALLOUT( lev, fun, str ) \
	if ( lev > 0 ) \
	MtPrintf( TRUE, FALSE, "[TRACE]Leaving \"%s\" in file \"%s\"on Ln%d(%s)\n", fun, __FILE__, __LINE__, str );

// 指针判断
#define MT_ASSERT( p ) if ( p == NULL ) { MtPrintf( TRUE, FALSE, "[SYSTEM]Invalid Pointer(%s:(%d))!\n", __FILE__, __LINE__ ); return; }
#define MT_ASSERT_RET( p, ret ) if ( p == NULL ) { MtPrintf( TRUE, FALSE, "[SYSTEM]Invalid Pointer(%s:(%d))!\n", __FILE__, __LINE__ ); return ret; }

// 释放内存
#define MT_SAFE_DELETE( p ) if ( p != NULL ) { delete p; p = NULL; }
#define MT_SAFE_DELETE_ARRAY( p ) if ( p != NULL ) { delete []p; p = NULL; }

// 有效性判断
#define MT_MAXVALUE_INVALID( val, maxVal ) if ( val > maxVal ) { MtPrintf( TRUE, FALSE, "[SYSTEM]Invalid value(%s:(%d))!\n", __FILE__, __LINE__ ); return; } 
#define MT_MAXVALUE_INVALID_RET( val, maxVal, ret ) if ( val > maxVal ) { MtPrintf( TRUE, FALSE, "[SYSTEM]Invalid value(%s:(%d))!\n", __FILE__, __LINE__ ); return ret; } 

#define MT_MINVALUE_INVALID( val, minVal ) if ( val < minVal ) { MtPrintf( TRUE, FALSE, "[SYSTEM]Invalid value(%s:(%d))!\n", __FILE__, __LINE__ ); return; } 
#define MT_MINVALUE_INVALID_RET( val, minVal, ret ) if ( val < minVal ) { MtPrintf( TRUE, FALSE, "[SYSTEM]Invalid value(%s:(%d))!\n", __FILE__, __LINE__ ); return ret; } 

// length check(用于消息体中长度的检查)
#define MT_LEN_CHECK( actLen, expLen ) if ( actLen != expLen ) { MtPrintf( TRUE, FALSE, "[SYSTEM]Len Mismatch(%s:(%d))!\n", __FILE__, __LINE__ ); return; }

// 错误信息打印
#define MT_ERROR_PRT( actVal, expVal, hint, errorno ) if ( actVal != expVal ) { MtPrintf( TRUE, FALSE, hint, errorno ); return FALSE; }




#endif