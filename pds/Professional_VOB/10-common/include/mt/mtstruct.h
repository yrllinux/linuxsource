/*******************************************************************************
 *  模块名   : MT                                                              *
 *  文件名   : mtstruct.h                                                      *
 *  相关文件 :                                                                 *
 *  实现功能 : 终端常用数据结构定义                                            *
 *  作者     : 张明义                                                          *
 *  版本     : V3.0  Copyright(c) 1997-2005 KDC, All rights reserved.          *
 *                                                                             *
 *  说明:                                                                      *
 *      本文件定义不同节点、应用间通信用的数据结构，其结构定义必须用一下模式   *
 *    type struct tag结构名 {                                                  *
 *        数据类型  变量名;                                                    *
 *        数据类型  变量名;                                                    *
 *     }结构名;                                                                *
 *                                                                             *
 *    =======================================================================  *
 *  修改记录:                                                                  *
 *    日  期      版本        修改人      修改内容                             *
 *  2005/8/6      4.0         张明义      创建                                 *
 *                                                                             *
 *******************************************************************************/
#ifndef _MT_STRUCT_H_
#define _MT_STRUCT_H_

#include "kdvtype.h"
#include "mtmacro.h"
#include "mttype.h"
#include "osp.h"
#include "vccommon.h"




/************************************************************************/
/* 配置相关信息数据结构定义开始                                         */
/************************************************************************/


/************************************************************************/
/* H323信息数据结构定义                                                 */
/************************************************************************/
typedef struct tagTH323Cfg
{
    s8   achMtAlias[MT_MAX_H323ALIAS_LEN+1]; //别名
    s8   achE164[MT_MAX_E164NUM_LEN+1];      //E164号码
    BOOL bIsUseGk;                  // 是否使用GK
	s8   achGKPwd[MT_MAX_PASSWORD_LEN+1];//GK密码
    u32_ip  dwGkIp;                     // GK 地址
    s8   achGkIpName[MT_MAX_H323GKSERVER_NAME_LEN]; //Gk域名地址
    BOOL bIsH239Enable;                  // H239方式
    BOOL bIsEnctyptEnable;          // 是否使用加密
    EmEncryptArithmetic emEncrptMode;      //加密方式
    u16  wRoundTrip;                //链路检测时间
public:
	tagTH323Cfg(){memset(this , 0 ,sizeof( struct tagTH323Cfg) );	}
}TH323Cfg,*PTH323Cfg;
    

/************************************************************************/
/* 引导信息数据结构定义                                               */
/************************************************************************/
typedef struct tagTGuideCfg
{
    EmLanguage  emLanguage;         //语言选择
    BOOL  bIsDHCPEnable;            //是否使用DHCP
    u32_ip  dwIp;                      //终端IP地址    
    u32_ip  dwMask;                     //子网掩码
	u32_ip  dwGateWay;                  //网关地址
    BOOL bH323Enable;              //是否使用H323
	BOOL bH320Enable;              //是否使用H320
	BOOL bSIPEnable;                //是否使用SIP
    TH323Cfg tH323Info;             //H323设置
    BOOL bUserCfgPwdEnable;         //是否设置配置密码
    s8  achUserCfgPwd[MT_MAX_PASSWORD_LEN+1]; //是否设置配置密码
    BOOL bNetCfgPwdEnable;         //是否设置网络配置密码
    s8  achNetCfgPwd[MT_MAX_PASSWORD_LEN+1]; //网络设置密码
}TGuideCfg, *PTGuideCfg;

// TsymboPoint台标位置结构
typedef struct tagTSymboPoint
{
	u16 X;
	u16 Y;
public:
	tagTSymboPoint(){ memset( this ,0 ,sizeof( struct  tagTSymboPoint ) );}
}TSymboPoint;

/************************************************************************/
/* 显示设置信息数据结构定义                                               */
/************************************************************************/
typedef struct tagTOsdCfg
{
    BOOL  bIsAutoPip;            //是否启用自动画中画
    BOOL  bIsShowConfLapse;      //是否显示会议时间
    BOOL  bIsShowSysTime;        //是否显示系统时间
    BOOL  bIsShowState;          //是否显示状态标志
    EmLabelType  emLabelType;    //台标类型
	TSymboPoint  tLableCoordinate; // 台标坐标
    EmDualVideoShowMode emDualMode;//双流显示方式
	EmDisplayRatio  emDisplayRatio;//显示比例模式
}TOsdCfg, *PTOsdCfg;

//时间结构重新定义
typedef struct tagTMtKdvTime
{
    u16 		m_wYear;//年
    u8  		m_byMonth;//月
    u8  		m_byMDay;//日
    u8  		m_byHour;//时
	u8  		m_byMinute;//分
	u8  		m_bySecond;//秒
public:
    tagTMtKdvTime(){ memset ( this ,0 ,sizeof( struct tagTMtKdvTime) );}
}TMtKdvTime, *PTMtKdvTime;

/************************************************************************/
/* 用户设置信息数据结构定义                                               */
/************************************************************************/
typedef struct tagTUserCfg
{
    EmTripMode emAnswerMode;   //应答方式
    BOOL  bIsAutoCallEnable;    //是否启用自动呼叫
    s8  achTerminal[MT_MAX_H323ALIAS_LEN+1]; //自动呼叫的终端名
    u16  wCallRate;               //自动呼叫码率
    BOOL  bIsRmtCtrlEnbale;      //是否允许远端控制
    BOOL  bIsSleepEnable;        //是否开启待机功能
    u16  wSleepTime;            //待机时间
	BOOL  bDisableTelephone;       // 是否禁用电话功能
}TUserCfg, *PTUserCfg;

/************************************************************************/
/*8010c配置vga输出参数                                               */
/************************************************************************/
//[xujinxing]
typedef struct tagTVgaOutCfg
{
	BOOL bVgaOut; //8010c是否在vga上输出
	u16  wRefreshRate; //vga的刷新率
}TVgaOutCfg, *PTVgaOutCfg;

//[xujinxing-2006-05-19]
typedef struct tagTMtEquipmentCapset
{
    BOOL bMicAdjustSupport;
	BOOL bMcSupport;
}TMtEquipmentCapset, *PTMtEquipmentCapset;


/************************************************************************/
/*内嵌mc配置                                                            */
/************************************************************************/
//[xujinxing-2006-10-17]
typedef struct tagTInnerMcCfg
{
	BOOL bUseMc;
	EmMCMode emMode;
	BOOL bAutoVMP;
	tagTInnerMcCfg()
	{
		bUseMc = FALSE;
		emMode = emMcModeEnd;
		bAutoVMP = FALSE;
	}
}TInnerMcCfg,*PTInnerMcCfg;



/************************************************************************/
/* 以太网信息数据结构定义                                               */
/************************************************************************/
typedef struct tagTEthnetInfo
{
    BOOL bEnable;
    u32_ip  dwIP;
    u32_ip  dwMask;
    BOOL bIsUserAssignBandWidth;
    u32  dwUpBandWidth;
    u32  dwDownBandWidth;
}TEthnetInfo, *PTEthnetInfo;



typedef struct tagTE1Unit                                                                                     
{                                                                                                              
    BOOL bUsed;        //使用标志,置1使用，0不使用  
    u32   dwE1TSMask;        // E1时隙分配说明 
    EmDLProtocol emProtocol; //接口协议封装类型,如果是单E1连接可以指定PPP/HDLC，
    //如果是多E1捆绑连接必须是PPP协议
    u32 dwEchoInterval; // 对应serial同步接口的echo请求时间间隔，秒为单位，有效范围0-256，默认填2
    u32 dwEchoMaxRetry;// 对应serial同步接口的echo最大重发次数，有效范围0-256，默认填2 
}TE1Unit,*PTE1Unit;

/************************************************************************/
/* E1信息数据结构定义                                               */
/************************************************************************/
typedef struct tagTE1Config
{                                                                                                            
// Single & Multi Chan param.                                                                                  

	u8  byUnitNum;                 //使用E1单元个数
    BOOL  bIsBind;                  //  是否绑定                                                                    
    u32_ip  dwIP;                      //  ip地址                                                                          
    u32_ip  dwIPMask;                  //  掩码，                                                                          
	// 多通道参数
    EmAuthenticationType emAuthenticationType;//PPP链接的验证方法PAP/CHAP,默认填emPAP 
    u32  dwFragMinPackageLen;       //  最小分片包长，字节为单位，范围20~1500，默认填20                                 
    s8   achSvrUsrName[MT_MAX_E1_NAME_LEN+1]; 	/* 服务端用户名称，用来验证对端的数据 */                                       
    s8   achSvrUsrPwd[MT_MAX_E1_NAME_LEN+1];   	/* 服务端用户密码，用来验证对端的数据 */                               
    s8   achSentUsrName[MT_MAX_E1_NAME_LEN+1];	/* 客户端用户名称，用来被对端验证 */                                           
    s8   achSentUsrPwd[MT_MAX_E1_NAME_LEN+1]; 	/* 客户端用户密码，用来被对端验证 */                                           
	// E1 Module Param.                                                                                            
    TE1Unit atE1Unit[ MT_MAX_E1UNIT_NUM ];//E1_SINGLE_LINK_CHAN_MT_MAX_NUM];/* 每个被捆绑的serial同步接口参数 */
	
}TE1Config, *PTE1Config;



/************************************************************************/
/* 串口信息数据结构定义                                               */
/************************************************************************/
typedef struct tagTSerialCfg
{
    u32  dwBaudRate; //波特率单位kbps
    u8   byByteSize;   //数据位长度 5,6,7,8
    EmParityCheck emCheck; //校验算法
    EmStopBits emStopBits; //停止位
}TSerialCfg, *PTSerialCfg;


/************************************************************************/
/* 路由信息数据结构定义                                               */
/************************************************************************/
typedef struct tagTRouteCfg
{
    BOOL bUsed;
    u32_ip dwDstNet; //目的子网
    u32_ip dwDstNetMask; //目的子网掩码
    u32_ip dwNextIP; //下一跳IP地址  
    u32 dwPri;  //路由优先级
}TRouteCfg, *PTRouteCfg;

/************************************************************************/
/* PPPOE信息数据结构定义                                               */
/************************************************************************/

/*初始化PPPOE模块时使用的结构体*/
typedef struct  tagTPPPOECfg
{
	BOOL    bUsed;   //是否使用PPPoE
    s8      abyUserName [MT_PPPoE_MAX_USENAME_LEN];/*拨号时使用的用户名,必填项*/
    s8      abyPassword [MT_PPPoE_MAX_PASSWORD_LEN]; /*拨号时使用的密码,必填项*/
    s8      abyServerName[MT_PPPoE_MAX_SERVERNAME_LEN];/*指定拨号服务商的名字*/
    BOOL  bAutoDialEnable;/*是否允许断链后自动拨号,默认不自动拨号*/
    BOOL  bDefaultRouteEnable;/*是否允许将对端的地址设为默认网关，默认不设为网关*/
    BOOL  bDebug;/*是否以debug模式运行pppoe，默认不运行在debug模式*/
    u32     dwAutoDialInterval;/*断链后再次自动拨号需要等带的时间（允许设置成0s）*/
    u16     wDialRetryTimes;/*拨号尝试的次数，如果达到该次数还没有响应则拨号失败*/
    u16     wLcpEchoSendInterval;/*链接建立后，发送LCP-ECHO包的时间间隔*/
    u16     wLcpEchoRetryTimes;/* 发送几次LCP-ECHO包没有得到相应则认为链接断开*/
    u8      byEthID;/*PPPOE服务需要绑定的以太网的ID号（从0开始）*/
}TPPPOECfg, *PTPPPOECfg;


/*有关PPPOE模块各种统计信息的结构体*/
typedef struct tagTPPPoEStates
{
    BOOL    bPppoeRunning;/*PPPOE是否运行，TRUR—运行；FALSE—没有运行*/
    u32     dwLocalIpAddr;/*本机IP地址,网络序*/
    u32     dwPeerIpAddr;/*对端IP地址,网络序*/
    s8      abyServerName[MT_PPPoE_MAX_SERVERNAME_LEN];/*ISP Server的名字*/
    u32     dwLinkKeepTimes;/*链路建立的时间*/
    u32     dwLinkDownTimes;/*链路断链的次数统计*/
    u32     dwPktsSend;/*总共发送的数据包数--包括各类协议包以及上层业务程序的数据包*/
    u32     dwPktsRecv;/*总共接收的数据包数*/
    u32     dwBytesSend;/*总共发送的字节数*/
    u32     dwBytesRecv;/*总共接收的字节数*/
}TPPPoEStates,*PTPPPoEStates;

/*调用上层业务程序注册的回调函数发送认证失败时消息的内容结构体*/
typedef struct tagTPPPoEAuthFailed
{
    s8      abyUserName [MT_PPPoE_MAX_USENAME_LEN];/*业务程序给的拨号时使用的用户名*/
    s8      abyPassword [MT_PPPoE_MAX_PASSWORD_LEN]; /*业务程序给的拨号时使用的密码 */
}TPPPoEAuthFailed,*PTPPPoEAuthFailed;

/*调用上层业务程序注册的回调函数发送建链通知消息的结构体*/
typedef struct tagTPPPoEConnNotify
{
    u32     dwOurIpAddr;/*本断获得的IP地址,网络序*/
    u32     dwPeerIpAddr;/*对端的IP地址*/
    u32     dwDnsServer1;/*DNS Server1,网络序*/
    u32     dwDnsServer2;/*DNS Server1,网络序*/    
}TPPPoEConnNotify,*PTPPPoEConnNotify;

/*通知业务程序找不到指定的ISP Server的消息结构体*/
typedef struct tagTPPPoENoServer
{
    s8      abyServerName[MT_PPPoE_MAX_SERVERNAME_LEN];/*ISP Server的名字*/
}TPPPoENoServer,*PTPPPoENoServer;

/*存储PPPOE版本信息的结构体*/
typedef struct tagTPPPoEVersion
{
    s8      abyVersion[MT_PPPoE_MAX_VERSION_LEN];
}TPPPoEVersion,*PTPPPoEVersion;

/************************************************************************/
/* SNMP信息数据结构定义                                               */
/************************************************************************/
typedef struct tagTSNMPCfg
{
    s8    achUserName[MT_MAX_COMM_LEN+1];//共同体名
    u32_ip   dwTrapServerIp;//告警主机地址
}TSNMPCfg, *PTSNMPCfg;


/************************************************************************/
/* QOS信息数据结构定义                                               */
/************************************************************************/
typedef struct tagTIPQoS
{
	//QoS服务类型
    EmQoS    emType;
	//信令QOS值 范围
    u8       abySignalling[2];
	//数据信道（用于摄像头远摇）QOS值 范围
    u8       abyData[2];
	//语音QOS值 范围
    u8       abyAudio[2];
	//视频QOS值 范围
    u8       abyVideo[2];
	//质量保证类型
    EmTOS    emTOS;
}TIPQoS, *PTIPQoS;




/************************************************************************/
/* 流媒体信息数据结构定义                                               */
/************************************************************************/
typedef struct tagTStreamMedia
{
    BOOL bUse;
    BOOL bForwardLocalVideo; //是否转发本地视频
    u32_ip  dwMCIP;    //组播IP地址
    u16  wPort;     //端口号
	u8   byTTL;     //TTL值
    s8   achChannelName[MT_MAX_CHANNEL_NAME_LEN+1];//频道名称
    s8   achPassword[MT_MAX_PASSWORD_LEN+1];//密码
}TStreamMedia, *PTStreamMedia;


/************************************************************************/
/*视频参数体信息数据结构定义                                               */
/************************************************************************/
typedef struct tagTVideoParam 
{
    BOOL  bUseFixedFrame;    //使用固定帧 
    u8 byFrameRate;	///帧率
    EmFrameUnitType  emFrameUnit;	//帧率单位
    u8 byQualMaxValue;	//最小量化参数
    u8 byQualMinValue;	//最大量化参数
    u16 wIKeyRate;	//关键帧间隔
    EmEncodeMode emEncodeMode;     //编码方式
    EmLostPackageRestore emRestoreType; //丢包恢复方式
    u16 wH264IKeyRate;		//H264关键帧间隔
    u8  byH264QualMaxValue;	//H264最大量化参数
    u8  byH264QualMinValue;	//H264最小量化参数
//以下参数只有在windows下才有用
//添加采集图像宽高
	u16	wWidth; //视频捕获图像宽度 仅Windows有效(default:640)
	u16	wHeight;//视频捕获图像高度 仅Windows有效(default:480)
	EmMediaType   emCapType;    //采集类型   (default:emMediaAV)
	EmPCCapFormat emPCCapFormat;//视频捕获帧格式 仅Windows有效(default:emCapPCFrameBMP)
//以下参数，针对高清终端, 
	u8 by720pFrameRate; //720p分辨率下帧率
	u8 byVgaFrameRate;  //双流vga下帧率
	//xjx_080612
	u8 byD1FrameRate; //D1分辨率下帧率
	
}TVideoParam, *PTVideoParam;


/************************************************************************/
/*优选协议信息数据结构定义                                               */
/************************************************************************/
typedef struct tagTAVPriorStrategy 
{
    EmVideoFormat  emVideoFormat;  //视频优选协议
    EmAudioFormat  emAudioFormat;    //音频优选协议
    EmVideoResolution emVideoResolution; //分辨率优选协议
}TAVPriorStrategy, *PTAVPriorStrategy;
/************************************************************************/
/* 码流重传信息数据结构定义                                               */
/************************************************************************/
typedef struct tagTLostPackResend
{
	BOOL bUse;             //是否启用
	EmNetType emNetType;   //网络类型
	u8   byResendLevel;    //重传等级 0<低,重传一次> 1<中,重传两次> 2<高,重传三次>
	u16  wFirstTimeSpan;   //第一个重传检测点   default 40
	u16  wSecondTimeSpan;  //第二个重传检测点   default 80
	u16  wThirdTimeSpan;   //第三个重传检测点   default 160
	u16  wRejectTimeSpan;  //过期丢弃的时间跨度 default 200
	u16  wSendBufTimeSpan; //发送缓存           default 1000
	BOOL bUseSmoothSend;   //启用平滑发送

}TLostPackResend,*PTLostPackResend;


/************************************************************************/
/*摄像机信息数据结构定义                                               */
/************************************************************************/


//摄像头能力类型
typedef struct tagTCameraTypeInfo
{
	s8 achName[MT_MAXLEN_CAMERA_TYPE_NAME];
	u8 byMaxAddr; //摄像头最大地址值
	u8 byMaxSpeedLevel;//摄像头最大速度级别 

}TCameraTypeInfo ,*PTCameraTypeInfo;


//摄像头配置
typedef struct tagTCameraCfg
{
    BOOL  bUse;
    s8  achCamera[MT_MAX_CAMERA_NAME_LEN+1];//摄像头名
    u8  byAddr;  //摄像头地址
    u8  bySpeedLevel; //摄像头控制速度
    EmSerialType emSerialType;
    u32_ip dwSerialServerIP; //串口服务器IP ,当该值为0时表示使用本地串口
    u16 wSerialServerPort; //串口服务器端口号    
	
}TCameraCfg, *PTCameraCfg;

typedef struct tagTVideoStandard
{
	EmVideoType     emVideoType;
	BOOL            bIsInPort;    //是否为输入端口
	EmVideoStandard emStandard;
}TVideoStandard ,*PTVideoStandard;

/************************************************************************/
/*视频源信息数据结构定义                                               */
/************************************************************************/
typedef struct tagTVideoSource
{
    EmVideoInterface emMainCam;
    u8 byCPortIndex ;//C端子输入时表明输入端口号，0基址
    EmVideoInterface emVideoOut;
    u8  byFrequency; //VGA时为刷新率
}TVideoSource, *PTVideoSource;


//快照信息
typedef struct tagTSnapInfo
{
	BOOL bResult;		// 获取结果(成功TRUE /失败FALSE)
	u32  dwMaxSize;		// 最大空间大小(字节)
	u32  dwUsedSize;		// 已用空间大小(字节)
	u32	 dwPicNum;	// 快照图片个数(个)
} TSnapInfo,*PTSnapInfo;

typedef struct tagTBannerInfo
{
	BOOL        bEnable;			// 使能(MT_ENABLE/MT_DISABLE)
	u32		    dwTextClr;			// 文字颜色
	u32		    dwBkgClr;			// 背景色
	EmTextAlign	emTextAlign;	// 文字对齐模式(TEXT_ALIGN_LEFT/TEXT_ALIGN_CENTER/TEXT_ALIGN_RIGHT)
	EmRollMode	emRollMode;		// 滚动模式(ROLL_NOT_ROLL/ROLL_RIGHT_LEFT/ROLL_DOWN_UP)
	u8 	        byTransParent;		// 透明度
	EmRollSpeed	emRollSpeed;		// 滚动速度(ROLL_SPEED_SLOW/ROLL_SPEED_NORMAL/ROLL_SPEED_FAST/ROLL_SPEED_FASTER)
	u8		    byRollNum;			// 滚动次数(0:表示无限制滚动,1~255:表示用户指定的滚动次数)
	u8		    byStayTime;			// 停留时间(0:表示不停留,1~255:表示用户指定的时间,单位秒)
	u32		    dwStartXPos;		// 字幕显示的起始X坐标
	u32		    dwStartYPos;		// 字幕显示的起始Y坐标
	u32		    dwDisplayWidth;		// 字幕显示区域的宽(以象素为单位)
	u32		    dwDisplayHeight;	// 字幕显示区域的高(指行高,以象素为单位)
	s8	        achWinInfo[ 256 ];	// 用于存放Windows的相关信息(以\0结束)
} TBannerInfo, *PTBannerInfo;


// codec网络统计信息
typedef struct tagTCodecStatistic
{
	u16 awVideoBitRate[2];		// 视频解码码率
	u16 awAudioBitRate[2];		// 音频解码码率
	u32 adwVideoRecvFrm[2];		// 收到的视频帧数
	u32 adwAudioRecvFrm[2];		// 收到的音频帧数
	u16 awVideoLoseRatio[2]; 	// 视频丢包率
	u16 awAudioLoseRatio[2];	    // 音频丢包率
	u32 adwVideoLoseFrm[2];		// 视频总丢报数
	u32 adwAudioLoseFrm[2];		// 音频总丢报数
} TCodecStatistic, *PTCodecStatistic;

// 音频功率
typedef struct tagTAudioPower
{
	u8	byDecoderId;
	u32 dwOutputPower;
	u32 dwInputPower;
} TAudioPower, *PTAudioPower;

/************************************************************************/
/*     矩阵配置                                                         */
/************************************************************************/

/************************************************************************/
/* 配置相关信息数据结构定义结束                                         */
/************************************************************************/



/************************************************************************/
/* 会议相关信息数据结构定义开始                                         */
/************************************************************************/

//终端编号
typedef struct tagTMtId
{
	u8 byMcuNo;
	u8 byTerNo;
	
public:
	tagTMtId(){ memset( this ,0 ,sizeof( struct tagTMtId ) );	}
	
}TMtId ,*PTMtId;

//基本会议信息
typedef struct tagTConfBaseInfo
{
	s8	 achConfId[MT_MAX_CONF_ID_LEN + 1 ];			// 会议ID
	s8	 achConfName[MT_MAX_CONF_NAME_LEN + 1 ];		// 会议名
	s8	 achConfNumber[ MT_MAX_CONF_E164_LEN + 1 ];	    // 会议号码
	s8	 achConfPwd[ MT_MAXLEN_PASSWORD + 1 ];			// 会议密码
	BOOL bNeedPassword;                         //是否需要密码
	u16  wConfDuration;							// 会议持续时间		
	EmVideoFormat   emVideoFormat;							// 会议视频格式(VIDEO_H261等)
	EmAudioFormat   emAudioFormat;							// 会议音频格式(AUDIO_MP3等)
	EmVideoResolution  emResolution;							// 会议视频分辨率(VIDEO_CIF等)
	BOOL  bIsAutoVMP;								// 是否自动画面合成
	BOOL  bIsMix;	    							// 是否混音

public:
	tagTConfBaseInfo(){ memset( this ,0 ,sizeof( struct tagTConfBaseInfo ) );	}
}TConfBaseInfo ,*PTConfBaseInfo;

//会议列表信息
typedef struct tagTConfListInfo
{
	u8			  byConfNum;
	TConfBaseInfo atConfInfo[MT_MAX_CONF_NUM];
public:
	tagTConfListInfo(){ memset( this ,0 ,sizeof( struct tagTConfListInfo ) );	}
}TConfListInfo ,*PTConfListInfo;

//终端信息
typedef struct tagTMtInfo
{
	TMtId tLabel;
	s8     achAlias[MT_MAX_NAME_LEN+1];

public:
	tagTMtInfo(){memset( this ,0 ,sizeof( struct tagTMtInfo) );}
}TMtInfo ,*PTMtInfo;

//视频编解码状态
typedef struct tagTMtVideoCodecStatus
{
	BOOL                bRuning;           //是否在工作
	EmVideoFormat       emFormat;          //视频编码格式
	EmVideoResolution   emRes;             //视频编码格式
	u16                 wBitrate;          //视频编码码率(单位:kbps)
	u16                 wAverageBitrate;   //主视频编平均码码率(单位:kbps)
	BOOL                IsEncrypt;         //主视频编码是否加密
	EmEncryptArithmetic emArithmetic;	   //主视编码使用的加密算法	

public:
	tagTMtVideoCodecStatus()
	{ 
		memset ( this ,0 ,sizeof( struct tagTMtVideoCodecStatus) );
	}
}TMtVideoCodecStatus ,*PTMtVideoCodecStatus;

//终端状态
typedef struct tagTTerStatus
{
	EmMtModel  emMtModel;				    //终端型号
	EmFileSys  emFileSys;                   //文件系统类型

	u8		byEncVol;					//当前编码音量(单位:等级)
	u8		byDecVol;					//当前解码音量(单位:等级)
	BOOL	bIsMute;					//是否哑音	
	BOOL	bIsQuiet;					//是否静音			
	BOOL	bIsLoopBack;				//是否自环
	BOOL    bIsInMixing;                //是否参加混音
	BOOL    bRegGkStat;			    	//GK注册结果	1 成功 0 失败
	EmAction   emPollStat;				//轮询状态		emStart ,emPause ,emStop /开始/暂停/停止
	EmSite	emCamCtrlSrc;				    //当前控制摄像头源类型 emLocal, emRemote
	u8		byLocalCamCtrlNo;			    //当前控制的本地摄像头号(1-6号摄像头)
	u8		byRemoteCamCtrlNo;			    //当前控制的远端摄像头号(1-6号摄像头)
	u8      byE1UnitNum ;                   //终端E1模块的的E1个数
	BOOL	bFECCEnalbe;				//是否允许远遥
	BOOL    bLocalIsVAG;                //本地第二路码流输出到是否VGA
	BOOL    bPVHasVideo;                //第一路是否有视频源
	BOOL    bSVHasVideo;                //第二路是否有视频源
//一下部分不建议使用
	BOOL	bIsAudioPowerTest;			//是否在测试音频	
	BOOL	bIsLocalImageTest;			//是否本地图像测试
	BOOL	bIsRemoteImageTest;			//是否远端图像测试 
//	///////会议状态///////
	BOOL       bIsInConf;				//是否在会议中	
	EmConfMode emConfMode;				//会议模式		1 两点 2 多点		
    BOOL       bCallByGK;				//是否通过GK呼叫 TRUE GK路由 FALSE直接呼叫
//	
//	///////设备状态///////
	BOOL	bMatrixStatus;				    //外置矩阵状态		(TRUE ok FALSE err)
	u8		byCamNum;					    //摄像头个数	(0~6)
	BOOL	bCamStatus[MT_MAX_CAMERA_NUM];	//摄像头状态 (TRUE ok FALSE err)
	EmMtVideoPort emVideoSrc;					//当前本地视频源(0:S端子,1-6:C端子)

	BOOL    bIsEncrypt;
//	//编解码器状态
	EmAudioFormat	    emADecFormat;	//音频解码格式
	EmAudioFormat	    emAEncFormat;	//音频编码格式
	TMtVideoCodecStatus  tPVEncStatus;   //主视频编码器
	TMtVideoCodecStatus  tSVEncStatus;   //辅视频编码器
	TMtVideoCodecStatus  tPVDecStatus;   //主视频解码器
	TMtVideoCodecStatus  tSVDecStatus;   //辅视频解码器
//	
	u16		wSysSleep;					//待机时间(0x0表示不待机)
//VOD states
	BOOL bIsInVOD;
	EmUIPosion   byVodUser;
	//////配置状态///////
public:
   tagTTerStatus(){ memset ( this ,0 ,sizeof( struct tagTTerStatus) );}
}TTerStatus ,*PTTerStatus;



//画面合成参数
typedef struct tagTMtVMPParam
{
	BOOL    bIsCustomVMP;  //是否自定义画面合成
	BOOL    bIsAutoVMP;    //是否自动画面合成
	BOOL    bIsBroadcast;
	EmVMPStyle emStyle;       //画面合成风格
	TMtId   atMt[MT_MAXNUM_VMP_MEMBER]; //画面合成成员
	EmVMPMmbType atemMmbType[MT_MAXNUM_VMP_MEMBER];//个成员的类型

public:
   tagTMtVMPParam(){ memset ( this ,0 ,sizeof( struct tagTMtVMPParam) );}
}TMtVMPParam ,*PTMtVMPParam;
// 扩展画面合成参数
typedef struct tagTMtVmpItem
{
	TMtId        tMt;
	EmVMPMmbType emMmbType;
	u32          adwReserved[8];
public:
   tagTMtVmpItem(){ memset ( this ,0 ,sizeof( struct tagTMtVmpItem) );}
}TMtVmpItem, *PTMtVmpItem;



//轮询信息
typedef struct tagTMtPollInfo
{
	EmMediaType emMode;      //轮询模式 emMediaVide /emMediaAV 
	EmAction    emStat;      //    emStart/  emStop/  emPause 轮询状态
	u16         wKeepTime;   //终端轮询的保留时间 单位:秒(s)
	u8			byMtNum;			// 参加轮询的终端个数,0表示所有与会终端(单位个,最多CTRL_POLL_MAXNUM)
	TMtInfo	    atMtInfo[ MT_MAX_POLL_NUM ]; // 参加轮询的终端<m,t>
public:
	tagTMtPollInfo(){ memset ( this ,0 ,sizeof( struct tagTMtPollInfo) );}
}TMtPollInfo ,*PTMtPollInfo;

 
typedef struct tagTMtSimpConfInfo 
{
	TMtId tSpeaker;
	TMtId tChairMan;
	BOOL  bIsVAC;
	BOOL  bIsDisc;
	BOOL  bIsAutoVMP;
	BOOL  bIsCustomVMP;
public:
	tagTMtSimpConfInfo(){ memset( this, 0, sizeof( struct tagTMtSimpConfInfo ) ); }
}TMtSimpConfInfo, *PTMtSimpConfInfo;
  
//定义会议信息
typedef struct tagTMtConfInfo
{
	s8          chConfId[MT_MAXLEN_CONFGUID+1];
	TMtKdvTime    tStartTime;//开始时间，控制台填0为立即开始
	u16           wDuration;   //持续时间(分钟)，0表示不自动停止
    u16           wBitRate;        //会议码率(单位:Kbps,1K=1024)
    u16           wSecBitRate;     //双速会议的第2码率(单位:Kbps,为0表示是单速会议)
    EmVideoResolution emMainVideoResolution;  //主视频格式
    EmVideoResolution emSecondVideoResolution; //辅视频格式
    EmVideoResolution emDoubleVideoResolution;   //第二路视频格式,
    u8            byTalkHoldTime;                 //最小发言持续时间(单位:秒)

	s8            achConfPwd[MT_MAXLEN_PASSWORD+1];    //会议密码
    s8            achConfName[MT_MAX_CONF_NAME_LEN+1]; //会议名
    s8            achConfE164[MT_MAX_CONF_E164_LEN+1]; //会议的E164号码
	
	BOOL          bIsAudioPowerSel;   //是否语音激励
	BOOL          bIsDiscussMode;     //是否讨论模式
    BOOL          bIsAutoVMP;            //是否自动多画面合成
	BOOL          bIsCustomVMP;        //是否自定义多画面合成
	BOOL          bIsForceBroadcast;//强制广播

    TMtId 	      tChairman;	   //主席终端，MCU号为0表示无主席
    TMtId		  tSpeaker;		  //发言终端，MCU号为0表示无发言人
    TMtPollInfo   tPollInfo;        //会议轮询参数,仅轮询时有较
    TMtVMPParam   tVMPParam;        //当前视频复合参数，仅视频复合时有效
  
public:
	tagTMtConfInfo(){ memset ( this ,0 ,sizeof( struct tagTMtConfInfo) );}
}TMtConfInfo ,*PTMtConfInfo;





// 编解码统计信息
typedef struct tagTMtCodecStat
{
	BOOL bWorking;               //是否开始工作
	u16 wBitrate;
	u32 dwTransPacket;            //收发包数
	u32 dwLostPackets;            //丢包数 
	u16 wLostRatio;               //丢包率
	EmAudioFormat  emAudioFormat; //格式
	EmVideoFormat  emVideoFormat;
	EmVideoResolution emVideoResolution; //视频分辨率
	BOOL      bIsEncrypt;   //是否加密
public:
	tagTMtCodecStat(){ memset( this ,0 ,sizeof( struct tagTMtCodecStat));}


}TMtCodecStat ,*PTMtCodecStat;

//IP传输地址
typedef struct tagTIPTransAddr
{
	u32_ip dwIP ;
	u16 wPort;
public:
	tagTIPTransAddr(){ memset( this, 0, sizeof( struct tagTIPTransAddr));}
}TIPTransAddr,*PTIPTransAddr;

//终端地址
typedef struct tagTMtAddr
{
	EmMtAddrType emType;							//地址类型
	u32_ip			 dwIP;						//终端IP地址
	s8			 achAlias[MT_MAX_H323ALIAS_LEN+1];		//(别名)
public:
	tagTMtAddr(){memset( this ,0 ,sizeof( struct tagTMtAddr));	}
}TMtAddr,*PTMtAddr;




//会议标识
typedef struct tagTConfId
{
	u8  byGuid[MT_MAXLEN_CONFGUID];//会议Id
	TMtAddr   tAlias;    //会议别名
public:
	tagTConfId(){ memset( this ,0 ,sizeof( struct tagTConfId)) ;	}
}TConfId ,PTConfId ;

//呼叫参数
typedef struct tagTDialParam
{
	EmCallMode   emCallType;			// 呼叫类型:JOIN|CREATE|INVITE
	EmConfProtocol emProtocol;          // 通信协议协议
	u16			 wCallRate;			    // 呼叫速率(kbps)
	TMtAddr	     tCallingAddr;		    // 主叫地址(主叫时可不填)
	TMtAddr	     tCalledAddr;			// 被叫地址

	//////////////////////////////////////////////////////////////////////////
	//以下仅用于创建会议
	BOOL		bCreateConf;		//是否是创建会议
	TConfBaseInfo   tConfInfo;			//会议信息
	u8			byTerNum;			//被邀终端个数
	TMtAddr 	atList[MT_MAXNUM_INVITEMT];	//被邀终端列表
public:
	tagTDialParam(){ memset( this ,0 ,sizeof( struct tagTDialParam ) );}
}TDialParam,*PTDialParam;

// 呼叫链路状态结构
typedef struct tagTLinkState
{
	EmCallState emCallState; //会话状态
	u32_ip			dwIpAddr;					  // 对端IP地址(网络序)
	s8  		achAlias[ MT_MAX_NAME_LEN + 1 ];  // 对端别名
	BOOL        bCalling;                         // TRUE = 主叫  FALSE=被叫
	EmCallDisconnectReason	emReason;   // 呼叫挂断原因
	BOOL        bGetChairToken;                   //获得主席令牌
	BOOL        bSeenByAll;                       //被广播
	u16         wCallRate;                        //呼叫码率
	EmMtModel   emPeerMtModel;                    //add by xujinxing,07/07/26,对端型号
public:
	tagTLinkState(){ memset( this ,0 ,sizeof( struct tagTLinkState ) );}
}TLinkState,*PTLinkState;


//TEmbedFwNatProxy 内嵌防火墙代理配置结构
typedef struct tagTEmbedFwNatProxy
{
    BOOL   bUsed;
    u32_ip dwFwNatProxyServIP;
	s8     achPrxySrvName[MT_MAX_PROXYSERVER_NAME_LEN];
    u16    wFwNatProxyServListenPort;
    u16    wStreamBasePort;//媒体流传输起始端口号
public:
	tagTEmbedFwNatProxy(){ memset( this ,0 ,sizeof( struct tagTEmbedFwNatProxy ) );}
}TEmbedFwNatProxy,*PTEmbedFwNatProxy;


//静态NAT映射地址
typedef struct tagTNATMapAddr
{
	BOOL bUsed; //是否启用静态映射地址
	u32_ip  dwIP;  //映射地址
public:
	tagTNATMapAddr(){ memset( this ,0 ,sizeof( struct  tagTNATMapAddr ) );}
}TNATMapAddr,*PTNATMapAddr;

//GK配置
typedef struct tagTGKCfg
{
	BOOL bUsed; //是否使用GK
	u32_ip  dwGKIP; //GK IP地址
    s8   achGkIpName[MT_MAX_H323GKSERVER_NAME_LEN]; //Gk域名地址
	s8   achGKPasswd[MT_MAX_PASSWORD_LEN + 1]; //Gk密码
public:
	tagTGKCfg(){ memset( this ,0 ,sizeof( struct  tagTGKCfg ) );}
}TGKCfg,*PTGKCfg;

/************************************************************************/
/* 内置GK信息数据结构定义                                               */
/************************************************************************/
typedef struct tagTInnerGKCfg
{
    BOOL bGKUsed; //是否启用内置GK
    u32_ip dwIp; //ip地址
    u32_ip dwMask; //子网掩码
    u32_ip dwGateWay; //网关
public:
    tagTInnerGKCfg(){ memset( this, 0, sizeof(struct tagTInnerGKCfg ) ); }   
}TInnerGKCfg, *PTInnerGKCfg;

/************************************************************************/
/* 内置Proxy信息数据结构定义                                               */
/************************************************************************/
typedef struct tagTInnerProxyCfg
{
    BOOL bInnerProxyUsed; //是否启用代理服务器
    u32_ip dwIp; //ip地址
    u32_ip dwMask; //子网掩码
    u32_ip dwGateWay; //网关
public:
    tagTInnerProxyCfg(){ memset( this, 0, sizeof(struct tagTInnerProxyCfg ) ); }  
}TInnerProxyCfg, *PTInnerProxyCfg;

/************************************************************************/
/* WiFi无线网络信息数据结构定义                                         */
/************************************************************************/
typedef struct tagTWiFiNetCfg
{
    BOOL   bWiFiUsed;  //是否启用WiFi 
    BOOL   bDHCP;      //是否启用DHCP
    u32_ip dwIp;       //ip地址
    u32_ip dwMask;     //子网掩码
    u32_ip dwGateWay;  //网关
public:
    tagTWiFiNetCfg(){ memset( this, 0, sizeof(struct tagTWiFiNetCfg ) ); }  
}TWiFiNetCfg, *PTWiFiNetCfg;

// TMonitorService（监控切换业务数据结构） 
typedef struct tagTMonitorService
{
	BOOL bSwitchEnable;
	//空闲时切换到监控业务的时间间隔
	u32   dwSwitchTimeout;
public:
	tagTMonitorService(){ memset( this ,0 ,sizeof( struct  tagTMonitorService ) );}
}TMonitorService,*PTMonitorService;

//矩形结构定义
typedef struct  tagTRect
{
    u16  wLeft;
    u16  wTop;
    u16  wWidth;
    u16  wHeight;

}TRect;

//点
typedef struct tagTPoint
{
	u16 wX;
	u16 wY;
}TPoint;

//双流
typedef struct TDualStream
{
    EmDualSrcType	emDualSrcType;  // 双流源类型
	EmAction        emAction;		// [REQ/RSP]	开关(MT_START/MT_STOP)
	BOOL            bResult;		// [RSP]		操作结果 TRUE= 成功 FALSE=失败
	EmMtVideoPort   emVideoSrc;     // 视频源
	TMtAddr         tRecvAddr;		// [RSP]		终端接收地址(仅对PC源)
	u8		        byReason;			// [RSP]		双流操作失败的原因
}TDualStream,*PTDualStream;


//双流码率比例配置
typedef struct tagTDualRation
{
    BOOL bSecondSendRateType;   //TRUE auto
    BOOL bSecondRecvRateType;   //接收码率类型
    u8 bySecondSendRate;  //如果不是自动时双流发送码率的比例值
    u8 bySecondRecvRate;  //双流接收码率的比例值
    u8 byPrimaryRecvRate; //单流接收码率的比例值
    tagTDualRation(){ memset( this ,0 ,sizeof( struct  tagTDualRation ) );}
}TDualRation, *PTDualRation;

//	图像调整参数
typedef struct tagTImageAdjustParam
{	 
	u8  wBrightness;//亮度  	 
	u8  wContrast;   //对比度  	
	u8  wSaturation; //色彩饱和度 
public:
	tagTImageAdjustParam(){ memset( this ,0 ,sizeof( struct  tagTImageAdjustParam ) );}
	
}TImageAdjustParam,*PTImageAdjustParam;


//视频编码参数
typedef struct tagTVideoEncodeParameter    
{
	EmVideoFormat      emVideoFormat; 
	u16                byMaxKeyFrameInterval; // I帧间最大P帧数目
    u8                 byMaxQuant;  //最大量化参数
	u8                 byMinQuant;  //最小量化参数
    BOOL               bUseFixedFrame; //是否使用固定帧率
	u8	               byFrameRate;  //帧率
	BOOL               bFramePerSecond;  //帧率单位 TRUE＝ 帧/秒  FALSE＝秒/帧 

    EmEncodeMode emEncodeMode;     //编码方式
	EmVideoResolution  emVideoResolution;
	u32                wChanMaxBitrate; // 单位 Kbps
//以下数据只有在windows平台下有用
//添加采集图像宽高
	u16	m_wWidth; //视频捕获图像宽度 仅Windows有效(default:640)
	u16	m_wHeight;//视频捕获图像高度 仅Windows有效(default:480)
	EmMediaType   emCapType;    //采集类型   (default:emMediaAV)
	EmPCCapFormat emPCCapFormat;//视频捕获帧格式 仅Windows有效(default:emCapPCFrameBMP)
public:
	tagTVideoEncodeParameter(){ memset( this ,0 ,sizeof( struct  tagTVideoEncodeParameter ) );}	
}TVideoEncodeParameter ,*PTVideoEncodeParameter;

typedef struct tagTEncryptKey
{
	u8 byLen;   //密钥长度
	u8 byKey[MT_MAXLEN_ENCRYPTKEY]; //密钥长度
public:
	tagTEncryptKey(){ memset( this ,0 ,sizeof( struct  tagTEncryptKey ) );}	
}TEncryptKey,*PTEncryptKey;

//视频通道参数
typedef struct tagTVideoChanParam
{
	EmVideoFormat     emVideoFormat;
	EmVideoResolution emVideoResolution;
	u32                wChanMaxBitrate; // 单位 Kbps
	u8                 byPayload;       //动态载荷
	TEncryptKey        tKey;            //若tKey.byLen=0 表示不加密
	BOOL               bIsH239;
	u8                 byFrameRate;          //帧率
 // 	u8                 byFrameRate;     2005-12-19 张明义委托张飞添加
public:
	tagTVideoChanParam()
	{ 
		emVideoFormat = emVEnd;
		emVideoResolution = emVResEnd;
		wChanMaxBitrate   = 0;
		byPayload         = 0;
		memset( &tKey ,0 ,sizeof(tKey) );
		bIsH239  = FALSE;
		byFrameRate = 25;
	}	
}TVideoChanParam ,*PTVideoChanParam;

//视频通道参数
typedef struct tagTAudioChanParam
{
	EmAudioFormat   emAudioFormat;
  
	u8               byPayload;     //动态载荷
	TEncryptKey      tKey;          //若tKey.byLen=0 表示不加密
public:
	tagTAudioChanParam()
	{
		emAudioFormat = emAEnd;
		byPayload     = 0;
		memset( &tKey ,0 ,sizeof(tKey));
	}
}TAudioChanParam ,*PtagTAudioChanParam;

//***************************** VOD *************************************
//
//登陆VOD服务器用户信息
//
typedef struct tagTVODUserInfo
{
	TIPTransAddr m_tIp;  //VOD服务器地址
	s8   m_achUserName[VOD_MAX_USER_NAME_LENGTH + 1];//用户名
	s8   m_achUserPass[VOD_MAX_PASSWORD_LENGTH + 1];//用户密码
	BOOL m_bRecordNamePass; //下次登录是否提示用户名和密码
public:
	tagTVODUserInfo()
	{
		memset(this,0,sizeof(tagTVODUserInfo));
	}
}TVODUserInfo,*PTVODUserInfo;


//
// vod 文件信息 本结构取代 TVODFileNameInfo+TVODFileInfo 
//
typedef struct tagTVODFile
{
	s8   achName[ MT_MAX_NAME_LEN + 1 ]; // 文件名
    BOOL bIsFile;     // TRUE =file FALSE= folder
    u32  dwFileSize;  // 文件长度
	//-- 文件信息--
	BOOL bIsInfoValid;       //文件信息是否有效
	u32 dwRecordTime;        //录制时间，
    u32 dwDuration;          //播放总时长，毫秒
    EmAudioFormat emAudio;          //音频， emAEnd表示没有该路码流 
    EmVideoFormat emPrimoVideo;     //视频1，emVEnd表示没有该路码流 
    EmVideoFormat emSecondVideo;    //视频2，emVEnd表示没有该路码流

}TVODFile;
//
// Files from VODServer Directory 
//
typedef struct tagTVODFileNameInfo
{
    s8  m_achFileName[ MT_MAX_NAME_LEN + 1 ]; // 文件名
    u8  m_byType; // 0 目录  1 文件 
    u32 m_dwFileLen; // 文件长度
} TVODFileNameInfo, *PTVODFileNameInfo;
 

//
// File Info from VODServer
//
typedef struct tagTVODFileInfo
{
    u32 m_dwRecordTime;        //录制时间，
    u32 m_dwDuration;          //播放总时长，毫秒
    EmAudioFormat m_emAudio;          //音频， emAEnd表示没有该路码流 // MEDIA_TYPE_PCMA ==> emAG711a( mtmp )
    EmVideoFormat m_emPrimoVideo;         //视频1，emVEnd表示没有该路码流 // kdvdef.h
    EmVideoFormat m_emSecondVideo;         //视频2，emVEnd表示没有该路码流
} TVODFileInfo, *PTVODFileInfo;

//
// Request Play Info from UI
//
typedef struct tagTRequestPalyInfo {
	s8  m_achFileName[ MT_MAX_NAME_LEN + 1 ]; // 文件名
	u8  m_bStart; // ( TRUE / 是, FALSE ) 是否立即开始播放
    EmAudioFormat m_emAudio;          //音频， emAEnd表示没有该路码流 // MEDIA_TYPE_PCMA ==> emAG711a( mtmp )
    EmVideoFormat m_emPrimoVideo;         //视频1，emVEnd表示没有该路码流 // kdvdef.h
    EmVideoFormat m_emSecondVideo;         //视频2，emVEnd表示没有该路码流
} TRequestPalyInfo, *PTRequestPalyInfo;
//****************************************** VOD ********************************************

typedef struct tagTVideoSourceInfo
{
	EmMtVideoPort  emVideoPort;
	s8  achPortName[MT_MAX_PORTNAME_LEN +1];
}TVideoSourceInfo,*PTVideoSourceInfo;


//外部矩阵信息
typedef struct tagTExternalMatrixInfo 
{
	s8 TypeName[ MT_MAX_AVMATRIX_MODEL_NAME ]; //矩阵型号
	u8 OutPort; //指定的输出断口号
	u8 InPortTotal; //输入端口总数 
public:
	tagTExternalMatrixInfo( ) { memset( this, 0, sizeof ( *this ) );	}

} TExternalMatrixInfo, *PTExternalMatrixInfo;


//内部矩阵方案结构
typedef struct tagTAVInnerMatrixScheme
{
	  EmMatrixType  emType; //矩阵类型
		s8 achName[ MT_MAXLEN_AVMATRIX_SCHEMENAME + 1 ]; 
		u8 byIndex; 
		//
		// 输出端口号( 1 - 6 )
		// 各个输出端口连接的输入端口
		//
		u8 abyOutPort2InPort[ 6 ]; 
		//
		// 端口( 1- 6 )名字
		//
		s8 achOutPortName[6][ MT_MAXLEN_AVMATRIX_SCHEMENAME + 1 ];
		s8 achInPortName [6][ MT_MAXLEN_AVMATRIX_SCHEMENAME + 1 ];
public:
	tagTAVInnerMatrixScheme( )
	{
		memset( this, 0, sizeof ( struct tagTAVInnerMatrixScheme ) );
	}
}TAVInnerMatrixScheme,*PTAVInnerMatrixScheme;


//短消息
typedef struct tagTMtSMS
{
	s8    abyText[MT_MAXLEN_SMSTEXT+1];
	EmSMSType emType;
	TMtId tMtSrc;
	u8    byDstNum;
	TMtId atMtDst[MT_MAXNUM_SMSDST];
	u8    bySpeed; //速度 (1-5)
	u8    byRollTimes; //滚动次数
//
public:
	tagTMtSMS(){memset(this ,0 ,sizeof( struct tagTMtSMS));	}
}TMtSMS ,*PTMtSMS;



//
//    配置通知数据结构
//

//用户设置
typedef struct tagTUserCfgNotify
{
	EmLanguage  emLanguage;   //!语言指示 
    TOsdCfg     tOsdCfg;      //!界面指示 
    TUserCfg    tUserCfg;     //!用户配置
public:
	tagTUserCfgNotify(){memset(this ,0 ,sizeof( struct tagTUserCfgNotify));	}
}TUserCfgNotify ,*PTUserCfgNotify;



//网络配置指示
typedef struct tagTNetConfigNotify
{
	TEthnetInfo  tEth0;    //!以太网0指示
	TEthnetInfo  tEth1;    //!以太网1指示
    TPPPOECfg    tPPOE;    //!PPPOE配置
    TSerialCfg   tRS232;   //!232串口配置指示
	TSerialCfg   tRS422;   //!422串口配置指示 
	TSerialCfg   tRS485;   //!485串口配置指示
    TE1Config    tE1;      //!E1指示
    TRouteCfg    atRouteCfg[MT_MAX_ROUTE_NUM];  //!路由
    TEmbedFwNatProxy tEmbedPrxy;        //!防火墙指示
    TNATMapAddr  tNatMapAddr;           //!静态NAT映射地址指示
    //TRetransCfg                //!码流重传
    TSNMPCfg     tSnmp;             //!网管配置
    TIPQoS       tQos;              //!Qos指示 
public:
	tagTNetConfigNotify(){memset(this ,0 ,sizeof( tagTNetConfigNotify));	}
}TNetConfigNotify ,*PTNetConfigNotify;


//音视频设置
typedef struct tagTAVConfigNotify
{
	TAVPriorStrategy  tAVPriorStrategy;        //!编码优选协议指示
	TVideoEncodeParameter tPVEncParam;  //!主视频编码参数
	TVideoEncodeParameter tSVEncParam;  //!辅视频编码参数
	EmMtVideoPort         emVideoSrc;    //!视频源指示
	TImageAdjustParam     tImgAdjustParam;   //!视频调整参数
	EmVideoStandard    emPriomVideoStandard;       //!主频制式指示  MTCTRL->UI
	EmVideoStandard    emSecondVideoStandard;      //!视频制式指示  MTCTRL->UI
	BOOL      bIsAEC;                //!AEC指示     MTCTRL->UI 
	BOOL      bIsAGC;                //!AGC指示
	u8        abyInVolume;                //!输入声音指示
	u8        abyOutVolume;                      //!输出声音指示
	TStreamMedia     tStreamMedia;         //!流媒体

public:
	tagTAVConfigNotify(){memset(this ,0 ,sizeof( struct tagTAVConfigNotify));	}

}TAVConfigNotify ,*PTAVConfigNotify;
/************************************************************************/
/* 会议相关信息数据结构定义结束                                         */
/************************************************************************/

//////////////////////////////////////////////////////////////////////////
//丢包时自动降低码率配置
typedef struct tagTBitRateAdjust
{
	BOOL bEnable;   //是否启动
	u16  wInterval;  //时间间隔
	u16  wLostTimes; //丢包次数
	u16  wDescRatio; //降速比率
	u16  wLowDescRatio; //慢速降速码率
	u16  wDelayInterval;  //延迟时间
}TBitRateAdjust,*PTBitRateAdjust;

//////////////////////////////////////////
//编码器统计信息
typedef struct tagTCodecPackStat
{
	u16    wBitrate;       //实时码率
	u16    wAvrBitrate;    //平均码率
	u32    dwLostPacks;    //丢包数
	u32    dwTotalPacks;   //收发包数
	u32    dwFrames;       //收发帧数
	u8     byLostFrameRate;//丢帧率(2秒)%d     
}TCodecPackStat,*PTCodecPackStat;

//视频统计信息
typedef struct tagTVideoStatistics
{
	BOOL             bH239;	
	EmVideoFormat     emFormat;      //格式
	EmVideoResolution emRes;
	TCodecPackStat    tPack;
}TVideoStatistics ,*PTVideoStatistics;

//视频统计信息
typedef struct tagTAudioStatistics
{
	EmAudioFormat     emFormat;      //格式
	TCodecPackStat    tPack;
}TAudioStatistics ,*PTAudioStatistics;

/************************************************************************/
/* PCMT视频源信息 Add by FangTao                                                       */
/************************************************************************/
typedef struct tagTCapType
{
	s8 m_achCapFileName[MT_MAX_FULLFILENAME_LEN+1];
	EmCapType emCapType;
public:
	tagTCapType(){ memset(this ,0 ,sizeof( tagTCapType)); }
}TCapType ,*PTCapType;

//呼叫统计信息
typedef struct tagTCallInfoStatistics
{
	u16               wCallBitrate;//呼叫码率
	EmEncryptArithmetic emRecvEncrypt; //会议接收加密类型
	EmEncryptArithmetic emSendEncrypt; //会议发送加密类型
	TAudioStatistics  tRecvAudio;//音频接收格式
	TAudioStatistics  tSendAudio;//音频发送格式

	TVideoStatistics tPrimoRecvVideo;  //主视频接收统计信息 
	TVideoStatistics tPrimoSendVideo;  //主视频发送统计信息 
	TVideoStatistics tSencodRecvVideo; //第二路视频接收统计信息 
	TVideoStatistics tSecondSendVideo; //第二路视频发送统计信息 
public:
	tagTCallInfoStatistics()
	{
		wCallBitrate = 0;
		emRecvEncrypt = emEncryptNone;
		emSendEncrypt = emEncryptNone;
		tRecvAudio.emFormat = emAEnd;
		memset(&tRecvAudio.tPack,0,sizeof(tRecvAudio.tPack));
		memcpy(&tSendAudio,&tRecvAudio,sizeof(tSendAudio));

		tPrimoRecvVideo.bH239 = FALSE;
		tPrimoRecvVideo.emFormat = emVEnd;
		tPrimoRecvVideo.emRes = emVResEnd;
		memset(&tPrimoRecvVideo.tPack,0,sizeof(tPrimoRecvVideo.tPack));

		memcpy(&tPrimoSendVideo,&tPrimoRecvVideo,sizeof(tPrimoRecvVideo));
		memcpy(&tSencodRecvVideo,&tPrimoRecvVideo,sizeof(tPrimoRecvVideo));
		memcpy(&tSecondSendVideo,&tPrimoRecvVideo,sizeof(tPrimoRecvVideo));
	}

}TCallInfoStatistics,*PTCallInfoStatistics;


/************************************************************************/
/* 双流盒信息                                                           */
/************************************************************************/
typedef struct tagTDualStreamBoxInfo
{
	BOOL   m_bUsed;
	u32_ip m_dwIp;
	u16    m_wPort;
}TDualStreamBoxInfo,*PTDualStreamBoxInfo;


/************************************************************************/
/* 编码器统计信息                                                       */
/************************************************************************/



/************************************************************************/
/* 文件传输模块开始
/************************************************************************/

// 版本号
#define		MTC_MT_FILE_VER		0x0100		// Ver1.0
#define     MTB_MT_FILE_VER     0x0100      // Ver1.0 add wangliang 2007/01/08

// 文件片编号，循环使用
#define		MTC_MT_MSG_BGN		0			// 请求开始消息的协议编号
#define     MTB_MT_MSG_BGN      0           // 请求开始消息的协议编号 add wangliang 2007/01/08


// 文件类型类型
#define		MT_OSD_LABEL		0			// 终端台标
#define		MT_OSD_RUNMSG		1			// 终端滚动消息
#define		MT_OSD_TITLE		2			// 终端字幕

// 拒绝原因
#define		NACK_REASON_OK			0		// 传输成功
#define		NACK_REASON_LOSTPACK	1		// 丢包
#define		NACK_REASON_TIMEOUT		2		// 超时
#define		NACK_REASON_TYPEERR		3		// 类型不匹配
#define		NACK_REASON_LENERR		4		// 消息头长度出错
#define		NACK_REASON_VERERR		5		// 版本号不匹配
#define		NACK_REASON_PROTOCOL	6		// 协议编号出错
#define		NACK_REASON_RECVING		7		// 正在传输
#define		NACK_REASON_FILEERR		8		// 文件不正确
#define     NACK_REASON_UNKNOWTYPE  9       // 未定义文件类型
#define     NACK_REASON_SAVEFILEERROR 10    //保存文件出错
#define     NACK_REASON_MTSTATUSERROR 11    //终端状态不正确



typedef  struct tagTFileTransfer 
{
public:
	//版本号		0x0100
	u16 wProtocolIdentifer;

	// 文件传输当前分片编号，0开始编号，循环标号
	u16 wFileSliceNo;


	//文件类型		台标,滚动消息,字幕
	u8 byType;

	//原因
	u8 byRspInfo;
	
	//文件标识符
	u16 wFileIdentifer;

	//消息包最大长度
	u16 wPakageMaxLen;

	//用户数据长度，值=消息头+文件切片长度
	u16 wDataLen;

}TFileTransfer,*PTFileTransfer;

/************************************************************************/
/* 双流盒结构定义
/************************************************************************/
typedef struct tagTDVBNetSession
{
	 //[xujinxing]
	 u32_ip   m_dwRTPAddr; /*RTP地址*/
	 u16    m_wRTPPort;  /*RTP端口*/
	 u32_ip   m_dwRTCPAddr;/*RTCP地址*/
	 u16    m_wRTCPPort; /*RTCP端口*/
}TDVBNetSession,*PTDVBNetSession;

typedef struct tagTDVBNetSndParam
{
	 u8 m_byNum;
	 TDVBNetSession m_tLocalNet;
	 TDVBNetSession m_tRemoteNet[MT_MAX_NETSND_DEST_NUM];
}TDVBNetSndParam,*PTDVBNetSndParam;

typedef struct tagTDVBLocalNetParam
{
	 TDVBNetSession  m_tLocalNet;
	 u32_ip        m_dwRtcpBackAddr;/*RTCP回发地址*/
	 u16         m_wRtcpBackPort; /*RTCP回发端口*/        
}TDVBLocalNetParam,*PTDVBLocalNetParam;

typedef struct tagTDVBVideoEncParam
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

	u8    m_byPalNtsc;    /*图像制式(PAL或NTSC)*/	
	u8    m_byCapPort;    /*采集端口号: 1~7有效。在KDV8010上，S端子端口号为7；C端子端口号缺省为2*/
	u8	m_byFrameRate;  /*帧率(default:25)*/	
	u8    m_byImageQulity;/*图象压缩质量,0:速度优先;1:质量优先*/
	u8    m_byReserved6;  /*保留*/
	u16	m_wVideoWidth;  /*图像宽度(default:640)*/
	u16	m_wVideoHeight; /*图像高度(default:480)*/


	u8  m_byFrameFmt; // 分辨率 0:不固定类型, 1: half cif, 2: 1 cif, 3: 2 cif,  4: 4 cif,  5: 用于图像合成编码  
	u8	m_byFrmRateCanSet; //帧率是否由外界设定 ?
	u8  m_byFrmRateLittleThanOne; // 帧率是否小于1 ?
}TDVBVideoEncParam,*PTDVBVideoEncParam;

typedef struct tagTSitecallInformation
{
	BOOL m_bIsCustomConf;
	s8 m_achConfName[MT_MAX_CONF_NAME_LEN+1];
	s8 m_achConfPwd[MT_MAXLEN_PASSWORD+1];
	BOOL m_bLocalPay;	
	s8 m_achConfCard[MT_MAX_CONF_NAME_LEN+1];
	s8 m_achConfCardPwd[MT_MAXLEN_PASSWORD+1];
	u8 m_byMultiPicCount;
	EmCallRate        m_emRate;
	EmVideoFormat     m_emVideoType;
	EmVideoResolution m_emVideoFormat;
	EmAudioFormat     m_emAudioFormat;
	u8   m_byTerninalCount;
	TMtAddr m_atTerminalE164Info[MT_MAX_CALLSITE_TERNINAL];
	TMtAddr m_atTerminalH323Info[MT_MAX_CALLSITE_TERNINAL];
public:
	tagTSitecallInformation()
	{
		memset(this,0,sizeof(tagTSitecallInformation));
		m_emRate = emRaten64;
		m_emVideoType = emVH263;
		m_emVideoFormat = emVCIF;
		m_emAudioFormat = emAG711a;
		m_bLocalPay = TRUE;
	}
}TSitecallInformation,*PTSitecallInformation;

typedef struct tagTTransparentSerial
{
    EmSerialType  m_emComType;
	u32           m_nDataLen;
    s8            m_achData[128];
}TTransparentSerial,*PTTransparentSerial;

typedef struct tagTPeerCapabilityInfo
{
    BOOL m_bMix;                        //混音
    BOOL m_bVAC;                        //语音激励
    BOOL m_bCustomVMP;                  //定制画面合成
    BOOL m_bAutoVMP;                    //自动画面合成
    BOOL m_bEndConf;                    //结束会议
    BOOL m_bInvateMt;                   //邀请终端
    BOOL m_bDropMt;                     //删除终端
    BOOL m_bSelSpeaker;                 //指定/取消发言人
    BOOL m_bSelChair;                   //指定/放弃主席
    BOOL m_bSelSource;                  //选看终端
    BOOL m_bFECC;                       //远程摄像头遥控
    BOOL m_bQuiet;                      //远端静音
    BOOL m_bMute;                       //远端哑音
    BOOL m_bConfReq;                    //会议申请处理
    BOOL m_bOnlineList;                 //在线终端列表
    BOOL m_bOfflineList;                //离线终端列表
	BOOL m_bPicSwitch;                  //画面切换
	BOOL m_bSelSpeakingUser;            //点名 
	BOOL m_bForceBroadcast;             //强制广播
}TPeerCapabilityInfo,*PTPeerCapabilityInfo;


/************************************************************************/
/*              快捷键结构体                                            */
/************************************************************************/
typedef struct tagTRapidKey 
{
	EmMtVideoPort emMainVidSrcBtn;
	EmHotKeyType emRedBtn;
	EmHotKeyType emYellowBtn;
	EmHotKeyType emGreenBtn;
}TRapidKey, *PTRapidKey;

/************************************************************************/
/*              终端在线升级结构体                                      */
/************************************************************************/
//升级版本中的文件信息
typedef struct tagTVerFileInfo
{
    EmMtOSType   m_emFileType;       //文件类型(Linux,Vxworks)
    u32          m_dwFileSize;       //文件大小(按字节计算)
    s8           m_szFileName[MT_MAX_FILESRV_FILENAME_LEN];//文件名
}TVerFileInfo, *PTVerFileInfo;

//升级版本的信息
typedef struct tagTVerInfo
{ 
    u8 m_byFileNum;
    s8 m_achBugReport[MT_MAX_FILESRV_BUG_REPORT_LEN];
    s8 m_achSoftVer[MT_MAX_FILESRV_SOFTWARE_VER_LEN];
    TVerFileInfo m_atVerFile[MT_MAX_FILESRV_DEVFILE_NUM];
}TVerInfo,*PTVerInfo;


// FEC配置信息结构
typedef struct tagTFecInfo
{
	BOOL   m_bEnableFec;   //视频是否使用前向纠错
	u8     m_byAlgorithm;  //设置fec算法 0:NONE 1:RAID5 2:RAID6
	u16    m_wPackLen;     //设置fec的切包长度
	BOOL   m_bIntraFrame;  //是否帧内fec
	u32    m_dwDataPackNum; //设置fec的x包数据包
	u32    m_dwCrcPackNum;  //设置fec的y包冗余包
public:
	tagTFecInfo(){ memset( this, 0, sizeof( struct tagTFecInfo ) ); }
}TFecInfo,*PTFecInfo;


//xjx_071221, 高清的内置矩阵的保存结构
//用宏来区分输出口的个数吧。
//内部矩阵方案结构
typedef struct tagTHDAVInnerMatrixScheme
{
	    EmMatrixType  emType; //矩阵类型
		s8 achName[ MT_MAXLEN_AVMATRIX_SCHEMENAME + 1 ]; 
		u8 byIndex; 
		BOOL bUseVgaIn;   //YPrPb2与vga输入端口复用，使用用vga
		BOOL bUseVgaOut;  //YPrPb2与vga输出端口复用，是否使用vga
		// 输出端口号( 1 - 4 ),	各个输出端口上连接的输入端口
		u8 abyOutPort2InPort[ MT_HD_MAX_AV_OUTPORT_NUM ]; 
		// 端口( 1-4 )名字, 对应背板上的名字
		s8 achOutPortName[MT_HD_MAX_AV_OUTPORT_NUM][ MT_MAXLEN_AVMATRIX_SCHEMENAME + 1 ];
		s8 achInPortName [MT_HD_MAX_AV_OUTPORT_NUM][ MT_MAXLEN_AVMATRIX_SCHEMENAME + 1 ];
public:
	tagTHDAVInnerMatrixScheme( )
	{
		memset( this, 0, sizeof ( struct tagTHDAVInnerMatrixScheme ) );
	}
}THDAVInnerMatrixScheme,*PTHDAVInnerMatrixScheme;



template < class T >
T SAFE_CAST( T &t, u8 *p )
{
	if (NULL == p) return t;
	memcpy( &t, p, sizeof( T ) );
	return t;
}



char* GetStructVersion();

/************************************************************************/
/* 终端硬件自动化测试 wl 2007/02/06
/************************************************************************/


// 测试消息头结构
typedef struct tagTMtAutoTestMsg
{	
    // 版本号 0x0100
    u16  m_wProtocolIdentifer;
    // 测试类别
    EmAutoTestType m_emAutoTestType;
    // 开始测试标志位：TRUE，开始测试；FALSE，停止测试
    BOOL m_bAutoTestStart;
    // 视频类型
    EmVideoType m_emVideoType;
    // 视频端口
    EmMtVideoPort m_emVideoPort;
    // 测试结果: 0，失败; 1，成功。
    BOOL m_bAutoTestReult; 
    // 测试失败原因:1, 硬件测试结果失败; 2, 测试正在进行;  
    // 3, 不支持该项测试; 4,终端状态错误;5, 测试软件版本号错误; 
    // 其它附加返回信息
    u8 m_byErrorReason;
    //电话号码
    s8 m_achPhoneNumber[32+1]; 
    //分机号码
    s8 m_achSubPhoneNumber[32+1];
    //E1 本机地址
    u32_ip m_dwLocalIp;
    //E1 ping地址
    u32_ip m_dwDstIp;
}TMtAutoTestMsg, *PTMtAutoTestMsg;

// 地址薄条目信息
typedef struct tagTAddrEntry
{
    s8      m_achEntryName[ MT_MAX_NAME_LEN + 1 ];		// 条目名字
    s8      m_achIpaddr[ MT_MAX_H323ALIAS_LEN + 1 ];	// 呼叫地址
    u32     m_dwCallTime;                               // 呼叫时间
    u16	    m_wCallRate;								// 呼叫速率
	BOOL    m_bTeleCall;
}TAddrEntry,*PTAddrEntry;


/************************************************************************/
/* 内置代理，多网段接入的配置                                           */
/************************************************************************/
typedef struct tagTPxyIPCfg
{
    BOOL bUsed;
    u32_ip dwIP;         //IP地址
    u32_ip dwMask;       //子网掩码
    u32_ip dwGateway;    //网关地址
}TPxyIPCfg, *PTPxyIPCfg;

typedef struct tagTWifiLinkInfo
{
    s8       achSSID[MT_MAX_NAME_LEN];            /* WiFi network name */
	s8       achBssMacAddr[MT_MAX_NAME_LEN];      /* Bss Mac Address*/
    u32      dwSignalLevel;                         /*Signal level : 0-100*/
    u32      dwLinkRate;				 	        /* Rate, unit: mpbs */
    u32      dwChannel;                             /* Wireless channel number*/
    u8       byAuthType;                            /*Authentication method*/
    u8       byEncrytoType;                         /*Encryto method*/
	u8       byNetType;                             /*ad-hoc or infra*/
}TWifiLinkInfo, *PTWifiLinkInfo;

typedef struct tagTWifiScanResult
{
    u32 dwBssNum;   /*Number of Link info, 0 -- WIFI_MAX_BSS_NUM*/
    TWifiLinkInfo tLinkInfo[WIFI_MAX_BSS_NUM];
} TWifiScanResult, *PTWifiScanResult;

typedef struct tagTWifiLinkCfgItem
{
	u32  dwId;
    s8   achCfgName[WIFI_MAX_NAME_LEN]; /*Configure name, shall not be empty when need save */
    TWifiLinkInfo tWifiLinkInfo;           /* Wifi link basic info*/

    s8   achWepKey1[WIFI_MAX_KEY_LEN];     /*Web key, shall not be null when byAuthType is Web*/
    s8   achWepKey2[WIFI_MAX_KEY_LEN];
    s8   achWepKey3[WIFI_MAX_KEY_LEN];
    s8   achWepKey4[WIFI_MAX_KEY_LEN];
    s8   achWpaPasswd[WIFI_MAX_KEY_LEN];  /*Wpa key, shall not be null when byAuthType is wpa or wpa2*/
}TWifiLinkCfgItem, *PTWifiLinkCfgItem;


typedef struct tagTWifiCfgInfo
{
    u32 dwCfgNum;
    TWifiLinkCfgItem tCfgInfo[WIFI_MAX_CFG_NUM];
} TWifiCfgInfo, * PTWifiCfgInfo;

typedef struct tagTWifiLinkStat
{
    EmWiFiLinkStat emState;
    TWifiLinkCfgItem tItem;  /*the state corresponds to this link*/
} TWifiLinkStat, *PTWifiLinkStat;

//扫描SSID信息
typedef struct tagTWiFiBSSIDInfo
{
    u8                      byPSKOFF;                              //PSK or enterprise
    EmWiFiNetType           emNetType;                             //网络类型
    EmWiFiEncryptType       emEncryptType;                         //加密类型
    EmWiFiEncryptArithmetic emArithType;                           //加密算法
    BOOL                    bEncrypt;                              //是否加密
    u32                     dwChannel;                             //Wireless channel number
    u32                     dwBeaconInterval;                      //BCN（省电参数）
    u32                     dwLinkRate;				 	           //Rate, unit: mpbs 
    u32                     dwSignalLevel;                         //信号强度
    s8                      achBSSMacAddr[MT_WIFI_MAX_NAME_LEN];   // Bss Mac Address
    s8                      achSSID[MT_WIFI_MAX_NAME_LEN];         //WiFi network name
    s8                      achPasswd[MT_WIFI_MAX_PASSWORD_LEN];   //加密密码
public:
    tagTWiFiBSSIDInfo()
    {
        memset( this, 0, sizeof( struct tagTWiFiBSSIDInfo ) );
    }
}TWiFiBSSIDInfo, *PTWiFiBSSIDInfo;

//配置SSID信息,是否需要保存其他的配置信息如省电模式,待确定?
typedef struct tagTWiFiBSSIDCfg
{
    u8                      byPSKOFF;                              //PSK or enterprise
    EmWiFiNetType           emNetType;                             //网络类型
    EmWiFiEncryptType       emEncryptType;                         //加密类型
    EmWiFiEncryptArithmetic emArithType;                           //加密算法
    BOOL                    bEncrypt;                              //是否加密
	u32                     dwCfgPri;                              //网络连接优先级
    u32                     dwChannel;                             //Wireless channel number
    s8                      achSSID[MT_WIFI_MAX_NAME_LEN];           //WiFi network name
    s8                      achPassword[MT_WIFI_MAX_PASSWORD_LEN];   //加密密码
    //u8                      byCfgType;                             //扫描项，配置项
    //s8                      achBSSMacAddr[MT_WIFI_MAX_NAME_LEN];   // Bss Mac Address
    //u32                     dwBeaconInterval;                      //BCN（省电参数）
    //u32                     dwLinkRate;				 	           //Rate, unit: mpbs
    //u32                     dwSignalLevel;                         //信号强度
public:
    tagTWiFiBSSIDCfg()
    {
        memset( this, 0, sizeof( struct tagTWiFiBSSIDCfg ) );
    }
}TWiFiBSSIDCfg, *PTWiFiBSSIDCfg;

typedef struct tagTGkRegInfo
{
	BOOL bRegisted;
	EmGKRegFailedReason emReason;
}TGkRegInfo, *PTGkRegInfo;

typedef struct tagTSiteCallResult
{
	BOOL bRegisted;
	EmGKRegFailedReason emReason;
}TSiteCallResult, *PTSiteCallResult;


typedef struct tagTHdAudioPort
{
	EmHDAudPort emHdAudPortIn;
	EmHDAudPort emHdAudPortOut;
}THdAudioPort, *PTHdAudioPort;

typedef struct tagTMiscCfg
{
	BOOL bUseSlice; //启用切包传输
}TMiscCfg, *PTMiscCfg;

typedef struct tagTVideoDisplayFrm
{
    u32 m_dw1080PFrm;
    u32 m_dw1080IFrm;
    u32 m_dwVGAFrm;
    u32 m_dw720PFrm;
    u32 m_dwSDFrm;
public:
		tagTVideoDisplayFrm()
		{
			memset( this, 0, sizeof( struct tagTVideoDisplayFrm ) );
    }
}TVideoDisplayFrm,*PTVideoDisplayFrm;

#endif //!_MT_STRUCT_H_
