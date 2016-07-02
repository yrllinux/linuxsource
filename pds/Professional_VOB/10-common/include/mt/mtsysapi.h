/*******************************************************************************
 *  模块名   : 终端业务                                                        *
 *  文件名   : mtsysapi.h                                                      *
 *  相关文件 :                                                                 *
 *  实现功能 : 提供终端业务系基本接口函数                                      *
 *  作者     : 张明义                                                          *
 *  版本     : V3.0  Copyright(c) 1997-2005 KDC, All rights reserved.          *
 *                                                                             *
 *    =======================================================================  *
 *  修改记录:                                                                  *
 *    日  期      版本        修改人      修改内容                             *
 *  2005/7/20     4.0         张明义      创建                                 *
 *                                                                             *
 *******************************************************************************/

#include "osp.h"
#include "mttype.h"
#include "mtstruct.h"
#include "mthint.h"
#include "mtlogctrl.h"

#ifndef _MT_SYS_API_H_
#define _MT_SYS_API_H_

#ifdef WIN32
#define MTAPI   extern "C"			__declspec(dllexport)
#else //#ifdef ( _VXWORKS_ || _LINUX_ )
#define MTAPI   extern "C"		
#endif

typedef void ( *FIPCONFLICT )( u8*pbyMacAddr, u32 dwIpAdrs);

typedef void (*FIPCONFLICT_d6446)(const u8* mac_addr, const struct in_addr* addr);


//终端打印函数
extern "C" void mtPrintf( char *szFormat, ... );

typedef struct tagTPingRtn
{
	s32 nTimeOut;
    s32 nReachable;  //是否到达
    s32 nTTL;
    s32 nTripTime;
}TPingRtn;

//系统升级状态
enum EmMtSysUpdateStatus
{
	emSysUpdateInit = 0,
	emSysUpdateProcessing,
	emSysUpdateSucceed,
	emSysUpdateFail	
};

MTAPI BOOL  InstallSystemPath(); //加载系统路径信息
MTAPI void  UninstallSystemPath();//卸载系统路径信息

//设置配置文件路径，该函数必须在系统启动时调用，否则失败
MTAPI BOOL mtSetConfPath( s8* path );

MTAPI EmMtModel GetMtModel();//获取终端型号

//[xujinxing]
MTAPI void GetTVgaOutCfg(TVgaOutCfg &tVgaOutCfg);//获取vga输出配置
API void SetTVgaOutCfg(TVgaOutCfg &tVgaOutCfg); //设置vga输出配置

MTAPI BOOL mtIsSupportDVB();

MTAPI EmFileSys GetFileSys();//获取终端文件系统
MTAPI BOOL mtIsFileExist( char *pcFileName );

void UnstallMtFile();	
void InstallMtFile();
s8* mtGetFileName(EmMtFile emFile );

#ifdef WIN32
void mtSetWindowWorkPath(s8* path );
#endif

MTAPI BOOL HardwareInit();//硬件初始化



#if defined(_VXWORKS_) || defined(_LINUX_)

#ifdef _d6446_
API BOOL InitDSP( BOOL bMcEnable );
#else
API BOOL InitDSP();
#endif

API u32  mtGetDspMemSize( u16 wDspId );
API u32  mtGetDspSpeed( u16 wDspId );
API u32  mtGetDspMemSpeed( u16 wDspId );
API void mtPing( s8* pchHost, s32 nPacketSize, s32 nTTL, s32 nTimeout, TPingRtn* ptReturn );

#endif//

    /*********************************************/
    /*                板级配置函数               */
    /*********************************************/

s32 mtGetOsTime();

//获取系统时间
BOOL mtBrdGetTime( struct tm& time );

//设置系统时间
BOOL mtBrdSetTime( struct tm& time );

//获取置E1模块上的E1个数
u8 mtBrdGetE1UnitNum();

//查询是否发生过IP冲突
BOOL mtBrdIpOnceConflicted();

//注册IP冲突回调函数
void mtRegIPConflictCallBack( FIPCONFLICT pfFunc );
//davinci下的ip冲突检测
void mtRegIPConflictCallBack_d6446( FIPCONFLICT_d6446 pfFunc );

	
//设置界面通信句柄
MTAPI BOOL  mtSetUIHandle( u32 hWnd);
MTAPI u32   mtGetUIHandle();

//设置界面事件函数
BOOL mtSetUIEventCb( void* callback );

//发送消息到界面
BOOL mtPostMsgToUI( u16 wEventId, void* lpData = NULL , int nSize = 0 );


BOOL mtSetUIInstallFunction( void* f );
BOOL mtInstallUI();
BOOL mtUninstallUI();

//键盘
BOOL  mtSetKeyboardNotifyFunction(void* pf);
void* mtGetKeyboardNotifyFunction();


API void mtstart();
API void mtquit();

//根据视频源端口获取摄像头索引
u8   mtGetCameraIndex( EmMtVideoPort emPort );

//根据摄像头索引视频源端口获取
EmMtVideoPort mtGetVideoPort( u8 byCamIndex );

u32_ip mtGetHostIP();//获取机器IP地址



//终端端口

struct TMtIPPortRange
{
	//UDP

	u16 wMtBasePort;
	u16 wMtEndPort;
	u16 wMcBasePort;
	u16 wMcEndPort;
	u16 wInnerBasePort;
	u16 wInnerEndPort;

	//TCP
	u16 wMtcPort;
	u16 wOtherTCPBase;
	u16 wOtherTCPEnd;

	//not use ,only reserve
	u16 wMtH245Port;
	u16 wMcH245BasePort;
	u16 wMcH245EndPort;
};
//结算终端端口
void mtCalcMtIPort( u16 wUDPPort ,u16 wTCPPort ,TMtIPPortRange& tRange ,u16 wMtcPort=60000);

//网络键盘帧听端口
u16  mtGetKeyboardPort();
u16 mtGetMaxCallNum();


/*
   
	 端口管理   


*/

//设置终端通信端口号
void InstallCommPort( u16 wMTCPort ,u16 wTCPBasePort ,u16 wUDPBasePort );


//获取TCP H245通信端口范围
void mtGetH245PortRange(u16& wBase ,u16& wEnd);
//获取终端控制台侦听端口号
u16 mtGetMtcListenPort();

//获取终端TCP通信起始端口号
u16 mtGetCommPort();

//根据编解码器组建类型获得本地RTP端口号
u16	mtGetLocalRtpPort( EmCodecComponent emComp   );


// 根据编解码器组建类型获得本地RTCP端口号
u16	mtGetLocalRtcpPort( EmCodecComponent emComp   );


//根据逻辑通道类型获得本地RTP端口号
u16	mtGetLocalRtpPort( EmChanType emChan   );


//根据编解码器组建类型获得本地RTCP端口号
u16	mtGetLocalRtcpPort( EmChanType emChan   );

//获取终端控制台发起双流时的转发RTP、RTCP本地接受端口号 
void mtGetMtcVideoForwardLocalPort(u16& wRtp  ,u16& wRtcp );


//音频解码转发
void mtGetAudioDecForwardLocalPort(u16& wRtp  ,u16& wRtcp );

// 获取双流盒发起双流时本地接受RTP、RTCP端口号
void	mtGetDVBVideoForwardLocalPort(u16& wRtp  ,u16& wRtcp );

//获取解码器1的本地RTP、RTCP端口号
void mtGet1thVidDecForwardLocalPort( u16& wRtp ,u16& wRtcp );

//获取解码器3的本地RTP、RTCP端口号
void mtGet2thVidDecForwardLocalPort( u16& wRtp ,u16& wRtcp );

//获取解码器3的本地RTP、RTCP端口号
void mtGet3thVidDecForwardLocalPort( u16& wRtp ,u16& wRtcp );

//获取解码器3的本地RTP、RTCP端口号
void mtGet4thVidDecForwardLocalPort( u16& wRtp ,u16& wRtcp );

//获取音频自环接收的RTP、RTCP端口
void mtGetAudioLoopbackLocalPort( u16& wRtp ,u16& wRtcp );


//视频自环接收端口号
void	mtGetVideoLoopbackLocalPort( u16& wRtp ,u16& wRtcp   );

//获取混音器端口
void  mtGetMixerLocalPort( u8 byIndex ,u16& wRtp ,u16& wRtcp   );

//VMP 接收端口
void  mtGetVMPLocalPort( u8 byIndex ,u16& wRtp ,u16& wRtcp   );

//MC 接收N通道混音端口
void  mtGetMcMixerNChanRcvPort(u16& wRtp ,u16& wRtcp   );
//MC 接收VMP 合成数据端口
void  mtGetMcVMPRcvPort(u16& wRtp ,u16& wRtcp   );

void  mtGetMcMixerChanRcvPort( u8 byIndex ,u16& wRtp ,u16& wRtcp   );
//根据终端ID获取本地接收RTP端口
u16 mtGetMCLocalRtpPort( u8 byMtId , EmChanType emType );

//根据终端ID获取本地接收RTCP端口
u16 mtGetMCLocalRtcpPort( u8 byMtId , EmChanType emType );


//根据编解码器组件获取MC时端口
void mtGetActiveMcLocalPort( EmCodecComponent emType ,u16& wRtp ,u16& wRtcp ,u16& wBackRtcp );

//根据通道获取端口获取MC时端口
void mtGetActiveMcLocalPort( EmChanType emType ,u16& wRtp ,u16& wRtcp ,u16& wBackRtcp );

//获取H245端口个数
u16 mtGetH245PortSize();

//获取防火墙代理起始端口
u16 mtGetProxyUDPBasePort();

//获取流控的起始端口
u16 mtGetFcnetUDPBasePort();

//xjx_080103, 获取netbuf的起始端口
u16 mtGetNetBufBasePort();


//类型转换

u8 mtMapVideoType(EmVideoFormat format);//Mt定义视频格式转换为KdvDef中的视频类型
u8 mtMapAudioType(EmAudioFormat format );//Mt定义音频格式转换为KdvDef中的音频类型
EmAudioFormat mtMapAudioFormat(u8 type);//KdvDef中的视频类型转换为Mt定义视频格式
EmVideoFormat mtMapVideoFormat(u8 type);//KdvDef中的音频类型转换为Mt定义音频格式

const char *mtGetHintMsgDesc( u16 wId, EmLanguage emLanguage );


//事件处理安装
BOOL mtInstallEvent();
//获取事件描述
s8*   mtGetEventDescription(u16 wEvent );

#include "mttypeex.h"
//数据类型辅助函数
char* GetEnumDescr(int type , int val);
#define GetEnumDescript( type ,val )   GetEnumDescr((int)em_##type ,val )




void mtPrintData(char* title,u8* buf ,EmDataType type ,u8 level =0 ,int step= 0 );
#define MTPRINT( data ,type)\
	mtPrintData(#data ,(u8*)data ,em_##type,0,0)

#define MtPrint( level , title , data ,type   )\
	mtPrintData( title ,(u8*)data ,em_##type,level,0)

//获取指定类型Marshal后数据大小
int  mtGetMarshalSize(EmDataType type);
#define _marshal_size( type) mtGetMarshalSize(em_##type )

BOOL mtMarshal(u8*const pbyDstBuf ,
			   const u8* const pbySrcBuf ,
			   EmDataType type ,
			   int nMmbCount ,
			   int& nDstMarshalSize ,
			   int& nSrcMarshalSize );

inline static BOOL mtMarshal(u8*const pbyDstBuf ,
						const u8* const pbySrcBuf ,
						EmDataType type,
						int nMmbCount = 1)
{
	int nDstMarshalSize;
	int nSrcMarshalSize;
	return mtMarshal( pbyDstBuf ,pbySrcBuf ,type ,nMmbCount ,nDstMarshalSize,nSrcMarshalSize);
}

#define _marshal( pDst ,pSrc ,type ) mtMarshal( (u8*)pDst,(u8*)pSrc ,em_##type ,1)			   
			   
BOOL mtUnmarshal(u8*const pbyDstBuf ,
			   const u8* const pbySrcBuf ,
			   EmDataType type ,
			   int nMmbCount ,
			   int& nDstMarshlSize ,
			   int& nSrcMarshalSize );

inline static BOOL mtUnmarshal(u8*const pbyDstBuf ,
						const u8* const pbySrcBuf ,
						EmDataType type,
						int nMmbCount = 1)
{
	int nDstMarshalSize;
	int nSrcMarshalSize;
	return mtUnmarshal( pbyDstBuf ,pbySrcBuf ,type ,nMmbCount ,nDstMarshalSize,nSrcMarshalSize);
}
#define _unmarshal( pDst ,pSrc ,type ) mtUnmarshal( (u8*)pDst,(u8*)pSrc ,em_##type ,1)

s8* mtGetAppName(u16 byAppId );

//mtsysapi初始化
BOOL mtSysApiInit();
void mtSysApiExit();

//加载MC信号量
void mcInstallSemphore();

//卸载MC信号量
void mcUninstallSemphore();

//设置MC是否激活
void mcSetActive(BOOL bActive );

//判断MC是否激活
BOOL mcIsActive();

//判断是否允许扩展会议
BOOL mcIsAdHocPermit();

//设置扩展会议是否允许
void mcSetAdHocPermit(BOOL bPermit );

u8  mtGetFecCapacityType();
void mtSetFecCapacityType( u8 byFecCapType );

//是否包含MC模块
BOOL mtIsMcValid();
//获取最大呼叫数
u16 mtGetMaxCallNum();

//记录当前呼叫码率是否适合扩展会议的判断结果
void mcSetCallRateValid( BOOL bValid );
//获取当前呼叫码率是否适合扩展会议的判断结果,只在点对点
BOOL mcIsCallRateVaild();

//根据vmp的风格，获取画面数
u8 mcGetVMPNum( EmInnerVMPStyle emVMPStyle);

extern BOOL32 MtCheckLicense(void);  // New license api
extern void MtCheckMacAddr(void);

#if 0  // Deleted by mengxiaofeng
void mcGenEncodeData(u8 *pOutBuf, u16 wInBufLen, u8 *pInBuf, BOOL32 bDirect);
BOOL mcGetLicenseDataFromFile();
BOOL mcGetLicenseFile();
BOOL mcCheckLicenseData();
#endif

s8 * mtGetMtModelName();

//流控模块的封装
BOOL mtInitFcNet();
BOOL mtQuitFcNet();

//T2终端的在线升级的api函数
BOOL mtMSSystemSwitch();
BOOL mtSetSysRunSuccess();
EmSysRunSuccessFlag mtGetRunSuccessFlag();

//设置Led灯状态
void mtSetLedStatus( u8 byLedType );
//xjx_080221,设置LNK灯的状态
void mtSetLNKLedStatus( BOOL bLedOn);
//指示灯状态设置
void mtHDSetLedStatus( u8 byLedId, u8 byState );

MTAPI BOOL IsT2Version();
MTAPI BOOL IsHDVersion();
MTAPI BOOL Is8010A();
MTAPI BOOL IsHangyeMT();
MTAPI BOOL IsQiyeMT();


//根据音频类型获取编码码率
u16 mtGetBitrate( EmAudioFormat emFormat );

#define MAXCALL_MT         16
#define INVALID_CALLID 0xff //无效的CallId
BOOL  InitCallIdManager();
u8    AllocateCallId(s8* holder );
void  FreeCallId(u8 byCallId );

//动态载荷
u8 mtGetDynamicPayload( EmVideoFormat emFormat );

//VOD 状态判断函数
BOOL mtVODStatePermit( EmVODState emFrom ,EmVODState emTo);

//写文件到裸分区
BOOL WriteFileToDisk(char* const pFileName, char* const pchOsName, char* const pchAppName);

//文件操作
BOOL mtAttribFile(const s8 *  filename ,BOOL bReadOnly =FALSE);
BOOL mtRenameFile(const s8 * oldpath,const s8 * newpath);
BOOL mtRemoveFile(const s8 *  filename );
BOOL mtMakeDir(const s8* path );
//
s8*  mtGetOsVer();
s8*  mtGetHardwareVer();
s8*  mtGetSoftwareVer();
s8*  mtGetSoftwareVer_Upgrade();

BOOL mtHasInnerMatrix();


BOOL mtBrdMicAdjustIsSupport();

u8    mtBrdMicVolumeGet( );
void  mtBrdMicVolumeSet(u8 byPlus );

//初始化DHCP
BOOL mtInitDHCP();
BOOL mtOpenDHCP();
BOOL mtCloseDHCP();
BOOL mtUpdateDHCPdue();
s8* mtGetMacAddr();

enum EmPathList{
	emFtpPath = 0,
	emRamdiskConfPath,
	emConfPath,
	emUserInfoPath,
	emWebfilesPath,
	emWebDocPath,
	emDataPath,
	emResPath,
	emPathListEnd
};
char* GetSysPath(EmPathList emPath);

void InitSysUpdateSem();
void DelSysUpdateSem();
void mtSetUpdateStatus(EmMtSysUpdateStatus emVal);//设置系统文件升级状态变量
void mtGetUpdateStatus(EmMtSysUpdateStatus &emVal);//获取系统文件升级状态变量

//系统重启切换VGA/Video时提示
void mtSysHint(EmHintMsgNo emHintMsgNo ,u16 wAppId , u16 wInstId = 1 );
const char* mtGetEventCheckString();
const char* mtPcdvGetEventCheckString();

void mtStartSpeaker( BOOL bOpen ); // 开启关闭扬声器
u8   mtInterpretCallDisconnectReason( EmCallDisconnectReason emReason );

API void MsgFromSlave_d6446(u32 dwID, void* pBuf, u32 dwBufSize);
BOOL IsLegalIpAddr(char* pchString );
EmMtAddrType GetAddrType( const char* pchString, u32 dwLen );

//WiFi接口, add by wangliang 2007/12/03
s32 mtWiFiGetIf( s8* pchIfName );   
s32 mtWiFiScanBss( s8* pchIfName, void** pBssList );
s32 mtWiFiFreeBss( void* pBssList );    
s32 mtWiFiGetStat( s8* pchIfName, void* pStatus ); 
s32 mtWiFiGetMib( s8* pchIfName, void* pMib ); 
s32 mtWiFiJoin( s8* pchIfName, void* pItem ); 
s32 mtWiFiUnJoin( s8* pchIfName ); 

//8010终端的喂狗问题, xjx_080310
BOOL mtOpenWdGuard(u32 dwNoticeTimeout);
BOOL mtCloseWdGuard(void);
BOOL mtNoticeWdGuard(void);
BOOL mtWdGuardIsOpened(void);

void MtmpStartInstallSemphore();
void MtmpStartUninstallSemphore();

// 用于高清1.2.1版本的多套分辨率台标、横幅等
#define HD_MULTIRES_VER            "Hd.multi.res.ver"
extern void mtSysGetPriomDecSrcRes( u16& w, u16& h );
extern void mtSysGetPriomEncScale( u16& w, u16& h );
extern void mtSysGetBannerImageSrc( u32& w, u32& h);

enum EmHdMultiResFile
{
	emHdSymbolFile1 = 0,
	emHdSymbolFile2,
	emHdSymbolFile3,
	emHdBannerFile1,
	emHdBannerFile2,
	emHdBannerFile3,
	emHdMultiResFileEnd,
};
s8* mtGetHdMultiResFileName( EmHdMultiResFile emFileType );
u16 mtArchSwapLE16( u16 x);
u32 mtArchSwapLE32( u32 x);
u16 mtArchSwapBE16( u16 x);
u32 mtArchSwapBE32( u32 x);

API void mtSetHDBVersion( BOOL bBVersion);
API BOOL mtHDIsBVersion();

#endif/*!_MT_SYS_API_H_*/
