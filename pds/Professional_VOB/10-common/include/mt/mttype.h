

#ifndef _MT_TYPE_H_
#define _MT_TYPE_H_
#include "osp.h"


enum EmWiFiLinkStat
{
	emWiFiIdle = 0,
	emWiFiConnecting,
    emWiFiConnectFailed,
    emWiFiConnected,
    emWiFiDisconnecting,
    emWiFiDisconnected
};


// enum EmWiFiLinkStatus
// {
// 	emWiFiIdle = 0,
//     emWiFiConnected,
//     emWiFiConnecting,
//     emWiFiConnectFailed,
//     emWiFiDisconnected,
//     emWiFiDisconnectFailed,
//     emWiFiDevicePullIn,
//     emWiFiDevicePullOut
// };

enum EmWiFiSelType
{
    emWiFiCfgSel = 0,
    emWiFiScanSel
};

enum EmWiFiNetType
{
    emWiFiAdHoc = 0,
    emWiFiInfra
};

enum EmWiFiEncryptType
{
    emWiFiEncryptNone = 0,
    emWiFiEncryptWep,
    emWiFiEncryptWpa,
    emWiFiEncryptWpa2
};

enum EmWiFiEncryptArithmetic
{
	emWiFiArithNone = 0,
	emWiFiArithWep40,
	emWiFiArithTkip ,
	emWiFiArithWrap,
	emWiFiArithAes,						/* aes */
	emWiFiArithWep104	
};

// VOD REJ Code 
//------------------------消息的REJ码-------------------------------------------
enum EmVODRejCode 
{	
	emVODNoReason = 0, // 未知错误
	emVODFileNotFound = 1, // 文件未找到
	emVODPalyNotStarted = 2, //播放未开始
	emVODMaxUser = 3, // 在线用户已达到最大允许数量
	emVODUserNotExist = 4, // 用户名不存在	
	emVODUserLogined = 5, // 该用户已经登录
	emVODInvalidedPassword = 6, // 密码错误
	emVODMEDIAVODInvalidedFileNameType = 7, // Client <-- Server ,文件名为空或不是ASF文件
	emVODPlayBusy = 9, // Client <-- Server ,正在发送上一次的码流请求
	emConnectFailed = 10,
	emDisconnect = 11 ,// 连接中断
	emVODNoError      //操作成功
};

//VOD 状态
enum EmVODState
{
	emVODIdle         ,//空闲
	emVODLogin        ,//正在登陆
	emVODFileListQuery,//正在获取文件列表
	emVODFileInfoQuery ,//正在查询播放文件信息
	emVODPlay  ,//正在播放
	emVODPause    ,//暂停
	emVODStop  ,   //停止
	emVODSeek  ,   //指定位置播放
	emVODActiveEnd
};

//横幅
enum EmTextAlign
{
	emAlignLeft = 0,//居左
	emAlignCenter,//居中
	emAlignRight//居右
};

enum EmRollMode
{
	emStatic = 0,
	emRight2Left,
	emDown2Up
};

enum EmRollSpeed
{
	emSlower = 0,
	emSlow,
	emNormal,
	emFast,
	emFaster
};

//串口停止位类型
enum EmStopBits
{
    em1StopBit =0,
    em1HalfStopBits=1,
    em2StopBits = 2
};



//串口校验位类型
enum EmParityCheck
{
    emNoCheck = 0 ,//无校验
    emOddCheck =1 ,//奇校验
    emEvenCheck =2,//偶校验
};

//串口类型
enum EmSerialType
{
    emRS232 = 0,
    emRS422 = 1,
    emRS485 = 2,
	emSerialTcpip =3 
};

//语言类型
enum EmLanguage
{
    emEnglish  = 0,
    emChineseSB = 1,
	emLangeEnd
};




//协议类型
enum EmConfProtocol
{
	emH323  = 0, 
	emH320  = 1,
	emSIP   = 2,
	emInvalidConfProtocel 
};

//编码方式
enum EmEncodeMode
{
    emQualityFirst  = 0, 
    emSpeedFirst    = 1,
    
};

//丢包恢复类型
enum EmLostPackageRestore
{
    emLPQualityFirst  = 0, 
    emLPSpeedSlow     = 1,
    emLPSpeedNormal   = 2,
    emLPSpeedFast     = 3,
};

//矩阵类型
enum EmMatrixType
{
	emMatrixInner   = 0, 
	emMatrixOuter   = 1,
	emMatrixVas     = 2,
};

//双流显示模式
enum EmDualVideoShowMode
{
    emDualVideoOnSigleScreen  =0,//单屏双显
    emEachVideoOwnScreen      =1//双屏双显
};

//显示比例
enum EmDisplayRatio
{
	emDR4to3      = 0, //4:3
	emDR16to9     = 1, //16:9
};

//视频类型
enum EmVideoType
{
   emPriomVideo = 0 ,//主视频
   emSecondVideo = 1//第二路视频
};


//视频制式类型
enum EmVideoStandard
{
    emPAL  =0 ,
    emNTSC = 1 ,
//    emSECAM = 2
};

//视频源类型
enum EmVideoInterface
{
    emVGA    = 0, 
    emSVideo = 1,
    emCVideo = 2,
};

//终端视频端口
enum EmMtVideoPort
{

	emMtVGA   =0 , //VGA 
	emMtSVid    , //S 端子
	emMtPC      , //PC
	emMtC1Vid   ,  //C1端子
	emMtC2Vid   ,  //C2端子
	emMtC3Vid   ,  //C3端子
	emMtC4Vid   ,  //C4端子
	emMtC5Vid   ,  //C5端子
	emMtC6Vid   ,    //C6端子
	emMtExternalVid = 10,//外置矩阵映射基始值
	emMtVideoMaxCount = 74, //最多支持视频源个数
	emMtVidInvalid = 255 
};

//应答方式类型
enum EmTripMode
{
    emTripModeAuto      =0,  //自动
    emTripModeManu      =1,  //手动 
	emTripModeNegative  =2,   //拒绝、关闭
};

//台标类型
enum EmLabelType
{
    emLabelAuto    =0,   //自动
    emLabelUserDef =1,  //自定义
    emLabelOff     =2  //关闭
};

//E1链路检测类型
enum EmDLProtocol
{
    emPPP   = 0, 
    emHDLC  = 1,
    emPPPOE = 2,
    emMP    = 3
};

//E1验证类型
enum EmAuthenticationType
{
    emPAP  = 0,
    emCHAP = 1
};

//帧率单位类型
enum EmFrameUnitType
{
    emFrameSecond =0,
    emSecondFrame =1
};

// 媒体类型
enum EmMediaType
{
	emMediaVideo = 1, //视频
	emMediaAudio = 2, //音频
	emMediaAV    = 3, //音频和视频
};


//视频协议类型
enum EmVideoFormat
{
    emVH261     = 0,
    emVH262     = 1,//MPEG2
    emVH263     = 2,
    emVH263plus = 3,
    emVH264     = 4,
    emVMPEG4    = 5,

	emVEnd		
};

//音频协议类型
enum EmAudioFormat
{
    emAG711a  = 0,
    emAG711u  = 1,
    emAG722   = 2,
    emAG7231  = 3,
    emAG728   = 4,
    emAG729   = 5,
    emAMP3    = 6,
	emAG721   = 7,
	emAG7221  = 8,

	emAMpegAACLC =9, 	//xjx_080315, 高清支持的音频
	emAMpegAACLD = 10,
	emAG719   = 11, //ruiyigen 20091012

	emAEnd	 
};


//分辨率类型
enum EmVideoResolution
{
    emVResolutionAuto = 0,//自动
    emVSQCIF = 1,
	emVQCIF  = 2,
    emVCIF   = 3,
    emV2CIF  = 4,
    emV4CIF  = 5,
    emV16CIF = 6,
		
	emVGA352x240,
	emVGA704x480,
	emVGA640x480,
	emVGA800x600,
	emVGA1024x768,
	emVGA1280x1024,   //新加, SXGA
	emVGA1600x1200,   //UXGA
	
	emVSQCIF112x96,
	emVSQCIF96x80,
	
	emVHD720p1280x720,  //高清
	emVHD1080p1920x1080, 
	emVHD1080i1920x1080,
	
	//xjx_080412, 新加
	emVHD480i720x480,
	emVHD480p720x480,
	emVHD576i720x576,
	emVHD576p720x576,	

	emVResEnd,

	//sfqian_081216 非标
	emV320x192,
	emV432x240,
	emV480x272,
	emV640x368,
	emV864x480,
	emV960x544,
	emV1440x816,
};

//QOS类型服务
enum EmQoS
{
   emDiffServ  = 0,      //区分服务
   emIPPrecedence  = 1  //IP优先
};

//TOS 类型
enum EmTOS
{
	emTOSNone        = 0,//不使用
	emTOSMinCost     = 1,//最小开销
	emTOSMaxReliable = 2,//最高可靠性
	emTOSMaxThruput  = 3,//最大吞吐量
	emTOSMinDelay    = 4,//最小延迟
};


//位置
enum EmSite
{
    emLocal = 0,  //本地
    emRemote    //远端
};

//共计10个通道
enum EmChanType
{
	emChanSendAudio,
	emChanSendPrimoVideo,
	emChanSendSecondVideo,
	emChanSendFecc,
	emChanSendT120,

	//接收通道
	emChanRecvAudio,
	emChanRecvPrimoVideo,
	emChanRecvSecondVideo,
	emChanRecvFecc,
	emChanRecvT120,

	emChanTypeEnd	
};

//编解码器各个部件
enum EmCodecComponent
{
	emPriomVideoEncoder  =0,
	emPriomVideoDecoder  ,

	emSecondVideoEncoder ,
	emSecondVideoDecoder ,

	emAudioEncoder ,
	emAudioDecoder ,
	emCodecComponentEnd,
};

//终端型号
enum EmMtModel
{
	emUnknownMtModel=0,
	emPCMT  =1    ,//桌面终端
	em8010      ,
	em8010A     ,
	em8010Aplus ,//8010A+
	em8010C     ,
	em8010C1    ,//8010C1
	emIMT       ,
	em8220A     ,
	em8220B     ,
	em8220C     ,
	em8620A     ,
	emTS6610E   ,
	emTS6210    ,
	em8010A_2   ,
	em8010A_4   ,
	em8010A_8   ,
	em7210      ,
	em7610      ,
	emTS5610    ,
	emTS6610    ,
	em7810      ,
	em7910      ,
	em7620_A    ,
	em7620_B    ,
	em7820_A	,
	em7820_B	,
	em7920_A	,
	em7920_B	

};
// << 文件系统类型 >>
enum EmFileSys
{
	emRAWFS=1,//raw file system [ramdisk + rawblock + tffs]
	emTFFS  //tffs
};
//加密算法
enum EmEncryptArithmetic
{
	emEncryptNone = 0,
	emDES         = 1,
	emAES         = 2,
	emEncryptAuto = 3,

	emEncryptEnd
	
};

//操作
enum EmAction
{
    emStart,
    emStop,
    emPause,
    emResume,
};

//终端使能操作
enum EmOptRet
{
	emEnable,
	emDisable,
};


//会议模式
enum EmConfMode
{
	emP2PConf = 0,//点对点会议
	emMCCConf = 1 //多点会议
};


//地址类型
enum EmMtAddrType
{
	emIPAddr =0,
	emE164   ,
	emH323id ,
	emDialNum,
	emSipAddr	
};

//呼叫模式
enum EmCallMode
{
	emJoin,
	emCreate,
	emInvite
};


//控制方向
enum EmDirection
{
	emUP   =0,
	emDown   ,
	emLeft   ,
	emRight 
};

//呼叫状态
enum EmCallState
{
	emCS_Idle  ,
	emCS_Calling ,      //正在发起呼叫
	emCS_P2P ,      //点对点会议
	emCS_MCC ,      //多点会议
	emCS_Hanup        //挂断
};


//呼叫状态
enum EmCallSiteState
{
	emCSS_IDLE,
	emCSS_Init ,
	emCSS_Waiting ,      //正在发起呼叫
	emCSS_Connected ,      //点对点会议
	emCSS_Failed
};

//传输模式
enum EmTransMode
{
	emUnicast,  //单播
	emBroadcast, //广播
	emMulticast //组播
};

//双流源类型
enum EmDualSrcType
{
    emDualVGA = 0,
	emDualVideo = 1,
	emDualPC = 2
};



enum EmMtInstType
{
	emMtService = 0 ,
	emMtUI          ,
	emMtConsole     ,
	emMtH323Service ,
	emMtH320Service ,
	emMtSipService  ,
	emMtH323StackIn ,
	emMtH323StackOut,
	emMtMP          ,
	emMtDevice      ,
	emMtAgent       
};

//画面合成风格定义
enum EmVMPStyle
{
    emVMPDynamic       =   0 ,  //动态分屏(仅自动合成时有效)
    emVMPOne         =   1 ,  //一画面
    emVMPVTwo        =   2 ,  //两画面：左右分 
    emVMPHTwo        =   3 ,  //两画面: 一大一小
    emVMPThree       =   4 ,  //三画面
    emVMPFour        =   5 ,  //四画面
    emVMPSix         =   6 , //六画面 
    emVMPEight       =   7 ,  //八画面
    emVMPNine        =   8 ,  //九画面
    emVMPTen         =   9 ,  //十画面
    emVMPThirteen    =   10,  //十三画面
    emVMPSixteen     =   11,  //十六画面
    emVMPSpecFour    =   12,  //特殊四画面 
    emVMPSeven       =   13,  //七画面
	emVMPSpecThirteen  =   14,  //特殊十三画面（用于华为MCU）
	emVMPTwenty      =   15,  //高清MPU的二十画面（只用于高清MPU）
};


//画面合成成员类型定义
enum EmVMPMmbType
{
    emVMPMmbMCSspec    = 1,  //会控指定 
    emVMPMmbSpeaker    = 2,  //发言人跟随
    emVMPMmbChairman   = 3,  //主席跟随
    emVMPMmbPoll       = 4,  //轮询视频跟随
    emVMPMmbVAC        = 5,	//语音激励(会控不要用此类型)
};

//短消息业务类型
enum EmSMSType//MS
{
	emSMSSingleLine  = 0,  //短消息
	emSMSPageTitle   = 1,  // 翻页字幕
	emSMSRollTitle   = 2,  // 滚动字幕
	emSMSStaticTitle = 3,  // 静态字幕
};


enum EmMtFile
{
    emMtFileBegin  = 0,//终端文件类型开始
	emMtConfigFile   =1,//配置文件
	emMtOldConfigBakFile     ,//旧配置文件备份
	emMtDebugConfiFile, //调试配置文件
	emMtCameraCommandFile,//摄像头命令文件
	emMtCameraPresetPostionFile,//摄像头预置位保存文件
	emMtExternMaxtrixCommandFile,//外置矩阵命令问文件
	emMtExternMaxtrixPortNameFile,//外置矩阵端口名文件
	emMtInnerMaxtrixSchemeFile,//内置矩阵方案文件
	emMtStreamMediaHtmlFile,//流媒体网页文件
	emMtResourceFile,//终端资源文件
	emMtZipResourceFile,//压缩终端资源文件
	emMtSymbolFile,//终端台标图片文件
	emMtBannerFile,//终端横幅文件
	emMtStaticPicFile,//终端静态图片文件
	emMtResInBin,//Bin文件中的资源文件
	emMtAddressBookEx,//扩展地址薄文件
	emMtUpdateSysFile,//版本升级文件
	emMtMinitorFile, //监视文件
	emMtZipFile,//应用程序
	emMtHintFile,//界面显示资源文件
	emMtKeyFile, //mclicense文件
    emMtInnerPxyFile,//内置代理服务器配置文件
	emMtFileEnd //终端文件类型结束

};
typedef u32   u32_ip;

enum EmCallRate
{
	emRaten64 = 0,							/*(0)64kbps*/
	emRaten2m64,							/*(1)2×64kbps*/
	emRaten3m64,							/*(2)3×64kbps*/
	emRaten4m64,							/*(3)4×64kbps*/
	emRaten5m64,							/*(4)5×64kbps*/
	emRaten6m64,							/*(5)6×64kbps*/
	emRater384,							/*(6)384kbps*/
	emRater1536,							/*(7)1536kbps*/
	emRater1920,							/*(8)1920kbps*/
	emRater128,							/*(9)128kbps*/
	emRater192,							/*(10)192kbps*/
	emRater256,							/*(11)256kbps*/
	emRater320,							/*(12)320kbps*/
	emRater512,							/*(13)512kbps*/
	emRater768,							/*(14)768kbps*/
	emRater1152,							/*(15)1152kbps*/
	emRater1472,							/*(16)1472kbps*/
	/*(注：V2.0版本中新增3M、4M、6M、8M速率)*/
	emRater3M,							/*(17)3M(2880kbps)*/
	emRater4M,							/*(18)4M(3840kbps)*/
	emRater6M,							/*(19)6M(5760kbps)*/
	emRater8M,							/*(20)8M(7680kbps)*/
	emRaterLastOne
};


//PC windows 采集格式
enum EmPCCapFormat
{
	emCapPCFrameBMP  = 0,//RGB24位图;
	emCapPCFrameUYUY = 1,//yuv4:2:2格式
	emCapPCFrameI420 = 2,//yuv4:2:0格式
};

//画中画显示模式
enum EmPiPMode
{
	emPiPClose   = 0,//画中画关闭
	emPiPRightBottom ,//画中画小画面显示在右下角
	emPiPLeftBottom  ,//画中画小画面显示在左下角
	emPiPLeftTop     ,//画中画小画面显示在左上角
	emPiPRightTop    ,//画中画小画面显示在右上角
};
// DVB Component
enum EmDVBComponent
{
	emDVBEncoder = 0,
	emDVBPriomVideoDecoder,
	emDVBSecondVideoDecoder
};
enum EmUIPosion
{
	emMT = 0,
	emMTC = 1
};
//呼叫挂断原因
enum EmCallDisconnectReason
{
	emDisconnect_Busy = 1   ,//对端忙
	emDisconnect_Normal     ,//正常挂断
	emDisconnect_Rejected   ,//对端拒绝
	emDisconnect_Unreachable ,//对端不可达
	emDisconnect_Local       ,//本地原因
	emDisconnect_Nnknown      //未知原因
};


//MC 的模式
enum EmMCMode
{
	emMcSpeech  ,//演讲模式
	emMcDiscuss ,//讨论模式
	emMcModeEnd
};


//内嵌mc的画面合成风格
enum EmInnerVMPStyle
{
    emInnerVMPAuto = 0,
	emInnerVMPOne,
	emInnerVMPTwo,
	emInnerVMPThree,
	emInnerVMPFour,
	emInnerVMPFive,
	emInnerVMPSix,
	emInnerVMPTwoBigLittle,
	emInnerVMPThreeBigLittle,
	emInnerVMPFourBigLittle,
	emInnerVMPEnd
};

//内嵌mc的解码器组件号
enum EmVMPComponent
{
	emVMPDecNull = 0,
	emVMPDec1th,
	emVMPDec2th,
	emVMPDec3th,
	emVMPDec4th,
	emVMPDec5th,
	emVMPDecLocal,
	emVMPEnd
};


//pcmt视频源类型
enum EmCapType
{
   emAudioOnly = 1, //
   emCameraOnly,
   emACBoth,
   emDesktopOnly,
   emADBoth,
   emFileAVBoth  
};
//pppoe状态
enum EmPPPoEState
{
	emFree,
	emLinking,
	emLinkup,
	emLinkdown,
	emUserError,
	emTimeOut,
	emAgentError
};

enum EmDhcpState
{
	emDhcpFree,
	emDhcpLinkup,
	emDhcpLinkdown,
};

enum EmAddrGroupOptType
{
    emCallIn,
	emCallOut,
	emUserDef,
	emTemplate,
	emSiteCall,
	emMissed,
	emUserDefExt0,
	emUserDefExt1,
	emUserDefExt2,
	emUserDefExt3,
	emUserDefExt4,
	emUserDefExt5,
	emInvalid
};

enum EmHotKeyType
{
	emApplySpeak,
	emApplyChair,
	emPrevPage,
	emNextPage,
	emBack,
	emLoop,
	emHotkeyEnd,
	emAEC,
	emDial,
};

enum EmMtOSType
{
	emWindows,
    emVxworks,
	emLinux
};

enum EmFxoState
{
    emFxoIdle,
    emFxoCalling, 
    emFxoConnect,
};

// 扬声器音量大小类型
enum EmMtLoudspeakerVal
{
	emLoudspeakerLow = 0,
	emLoudspeakerMiddle,
	emLoudspeakerHigh,
};

// 重传时所选择的网络类型
enum EmNetType
{
	emInternet = 0,
	emSpecial,
	emVPN,
};

//T2终端系统运行状态标志
enum EmSysRunSuccessFlag
{	
	emSysNoUpdateOperation = 0,
	emSysUpdateVersionRollBack,
	emSysUpdateVersionSuccess,
	emSysSetUpdateFlagFailed,
};

//mcu通知终端的信息类型  
enum EmMcuNtfMsgType
{
	emMsgNoneNtf =0,
	emMsgBePolledNextNtf,
	
};




struct TDATAINFO
{
	//数据类型
	int type ;
	//数据类型大小
	int size ;
	//数据个数
	int arraysize;
	//数据地址偏移量
	int offset;
	//成员变量名称
	char* member;
};

struct TENUMINFO
{
	char* descrip;
	int   val;
};

// 终端硬件自动化测试类型 add by wangliang 2007/02/05
enum EmAutoTestType 
{
    emAutoTestTypeBegin,            //测试类型开始
        
    emAutoTestFXOFunc,              //电话接口测试，FXO环路测试
    emAutoTestSerialFunc,           //串口功能测试
    emAutoTestEthFunc,              //网口功能测试
    emAutoTestUSBFunc,              //USB功能测试
    emAutoTestAudioLoopBackFunc,    //音频环回功能测试
    emAutoTestVideoLoopBackFunc,    //视频环回功能测试 - C端子  
    emAutoTestLoudspeakerFunc,      //扬声器功能测试
    emAutoTestMICFunc,              //MIC功能测试
    emAutoTestCameraRotaFunc,       //摄像头转动测试
    emAutoTestRTCFunc,              //RTC功能测试
    emAutoTestLightFunc,            //指示灯测试
    emAutoTestRemoteCtrlFunc,       //红外接口功能测试
    emAutoTestOtherFunc,            //其他功能测试
    emAutoTestVideoLoopBackVgaFunc, //视频环回功能测试 - VGA
    emAutoTestSetDefault,           //恢复测试标志默认值
    emAutoTestLoadFile,             //单片机文件加载
    
    emAutoTestE1Func,               //E1功能测试

    emAutoTestSDIInSDIOutFunc,      //高清输入输出测试：SDI输入，SDI输出
    emAutoTestSDIInHDMIOutFunc,     //高清输入输出测试：SDI输入，HDMI输出
    emAutoTestSDIInYprPb1OutFunc,   //高清输入输出测试：SDI输入，YprPb1输出
    emAutoTestSDIInYprPb2OutFunc,   //高清输入输出测试：SDI输入，YprPb2输出
    
    emAutoTestHDMIInSDIOutFunc,      //高清输入输出测试：HDMI输入，SDI输出
    emAutoTestHDMIInHDMIOutFunc,     //高清输入输出测试：HDMI输入，HDMI输出
    emAutoTestHDMIInYprPb1OutFunc,   //高清输入输出测试：HDMI输入，YprPb1输出
    emAutoTestHDMIInYprPb2OutFunc,   //高清输入输出测试：HDMI输入，YprPb2输出

    emAutoTestYprPb1InSDIOutFunc,    //高清输入输出测试：YprPb1输入，SDI输出
    emAutoTestYprPb1InHDMIOutFunc,   //高清输入输出测试：YprPb1输入，HDMI输出
    emAutoTestYprPb1InYprPb1OutFunc, //高清输入输出测试：YprPb1输入，YprPb1输出
    emAutoTestYprPb1InYprPb2OutFunc, //高清输入输出测试：YprPb1输入，YprPb2输出
    
    emAutoTestYprPb2InSDIOutFunc,    //高清输入输出测试：YprPb2输入，SDI输出
    emAutoTestYprPb2InHDMIOutFunc,   //高清输入输出测试：YprPb2输入，HDMI输出
    emAutoTestYprPb2InYprPb1OutFunc, //高清输入输出测试：YprPb2输入，YprPb1输出
    emAutoTestYprPb2InYprPb2OutFunc, //高清输入输出测试：YprPb2输入，YprPb2输出
    
    emAutoTestVGAInVGAOutFunc,       //VGA功能测试

    emAutoTestAudioCPortInCPortOut, //高清音频输入输出功能测试:c端子输入,c端子输出
    emAutoTestAudioCPortInHDMIOut,  //高清音频输入输出功能测试:c端子输入,HDMI输出
    emAutoTestAudioHDMIInCPortOut,  //高清音频输入输出功能测试:HDMI输入,c端子输出
    emAutoTestAudioHDMIInHDMIOut,   //高清音频输入输出功能测试:HDMI输入,HDMI输出
    emAutoTestTypeEnd               //测试类型结束
};

enum EmGKRegFailedReason
{
	emNone,
	emGKUnReachable,
	emInvalidAliase,
	emDupAlias,
	emInvalidCallAddress,
	emResourceUnavailable,
	emUnknown
};

enum HD_PORT_MASK
{
	//PORT_HDMI =  0x01,
	//PORT_SDI =   0x02,
	PORT_SDI =  0x01,
	PORT_HDMI =   0x02,	
	PORT_YPrPb1 = 0x04,
	PORT_YPrPb2 = 0x08,	
	PORT_VGA = 0x10,
	PORT_C0 = 0x20,
};

enum EmHDAudPort
{
	emAudHDMI0 = 0,
	emAudC0
};

//////////////////////////////////////////////////////////////////////////

#endif

