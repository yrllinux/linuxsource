/******************************************************************************
模块名  ： simbrdwrapper
文件名  ： BoardWrapper.h
相关文件：
文件实现功能：模拟BoardWrapper模块对外提供的主要功能
作者    ：john
版本    ：1.0.0.0.0
---------------------------------------------------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
7/0/2006    0.1         john         创建
******************************************************************************/
#include <time.h>
#include <sys/sysinfo.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>

#include "simbrdwrapper.h"
pid_t g_tpid = 0;

STATUS BrdInit(void)
{
	g_tpid = getpid();
	return OK;
}

STATUS BrdQueryPosition( TBrdPosition *ptBrdPosition )
{
	if( NULL == ptBrdPosition )
	{
		return ERROR;
	}

	s32 nvalue = 0;

	::GetRegKeyInt( profilename.data(),
					boardposition.data(),
					boardlayer.data(),
					0, &nvalue );
	
	ptBrdPosition->byBrdLayer = (u8)nvalue;
	
	::GetRegKeyInt( profilename.data(),
					boardposition.data(),
					boardslot.data(),
					0, &nvalue );
	
	ptBrdPosition->byBrdSlot = (u8)nvalue;

	::GetRegKeyInt( profilename.data(),
					boardposition.data(),
					boardtype.data(),
					0, &nvalue );

	ptBrdPosition->byBrdID = (u8)nvalue;
	
	return OK;	
}

u8  BrdGetBoardID(void)
{
	s32 nvalue = 0;	
	::GetRegKeyInt( profilename.data(),
					boardposition.data(),
					boardtype.data(),
					0, &nvalue );

	return (u8)nvalue;
}

u8  BrdQueryHWVersion (void)
{
	return (u8)boardhwver;
}

u8  BrdQueryFPGAVersion (void)
{
	return (u8)boardfpgaver;
}

s32  BrdQueryNipState (void)
{
    return OK;
}
void  BrdQueryOsVer(s8 *pchVer, u32 dwBufLen,u32 *pdwVerLen)
{
	u32 length = osversion.length() < dwBufLen ? osversion.length() : dwBufLen ;

	memcpy( pchVer, osversion.data(), length );

	*pdwVerLen = length;
	return;
}

u8 BrdGetBSP15Speed( u8 byDevID )
{
	u8 byretspeed = 0;
	if( byDevID < (u8)5 )
	{
		byretspeed = bsp15speed;
	}

	return byretspeed;
}

STATUS BrdUpdateAppImage (s8* pImageFile)
{
    return OK;
}

STATUS BrdUpdateAppFile (s8* pchSrcFile,s8* pchDstFile)
{
    return OK;
}

void BrdEthPrintEnable(BOOL32 bEnable)
{
    return ;
}
u8 BrdGetBSP15SdramSize(u8 byDevID)
{
	u8 bydramsize = 0;
	if( byDevID < 5 )
	{
		bydramsize = bsp15speed;
	}

	return bydramsize;
}

s32 BrdOpenSerial(u8 byPort)
{
	s32 nret = 0;
	switch( byPort )
	{
	case BRD_SERIAL_RS232:
		nret = 0;
		break;
	case BRD_SERIAL_RS422:
		nret = 1;
		break;
	case BRD_SERIAL_RS485:
		nret = 2;
		break;
	case BRD_SERIAL_INFRARED:
		nret = 3;
		break;
	default:
		nret = 5;
		break;
	}
	return nret;
}

s32 BrdCloseSerial(s32 nFd)
{
	return OK;
}

s32  BrdReadSerial(s32 nFd, s8  *pbyBuf, s32 nMaxbytes)
{
	return OK;
}

s32 BrdWriteSerial (s32 nFd, s8  *pbyBuf, s32 nBytes)
{
	return NULL == pbyBuf ? ERROR: OK ;
}

s32 BrdIoctlSerial (s32 nFd, s32 nFunction, s32 nArg)
{
	return OK;
}

s32  BrdRs485QueryData (s32 nFd, TRS485InParam *ptRS485InParam,TRS485RtnData *ptRS485RtnData)
{
	if( NULL == ptRS485InParam || NULL == ptRS485RtnData )
	{
		return ERROR;
	}
	return OK;
}

s32  BrdRs485TransSnd (s32 nFd, u8 *pbyMsg, u32 dwMsgLen)
{
	return OK;
}

s32  BrdLedStatusSet(u8 byLedID, u8 byState)
{
	return OK;
}

s32  BrdQueryLedState(TBrdLedState *ptBrdLedState)
{
	if( NULL == ptBrdLedState )
	{
		return ERROR;
	}
	return OK;
}

void  SysRebootEnable (void)
{
	
}

void  SysRebootDisable(void)
{
	
}

STATUS  BrdWakeup(void)
{
	return OK;
}

STATUS  BrdSleep (void)
{
	return OK;
}

void  BrdHwReset(void)
{
	pid_t tpid = getpid();
	printf("[] kill pid= %d.\n", tpid);
	// int nret = kill(tpid, 9);
	int nret = kill(g_tpid, 9);
	if( 0 != nret )
	{
		printf("[] error = %d.\n", errno );
	}
	return;
}

STATUS SysRebootHookAdd(VOIDFUNCPTR ptRebootHookFunc)
{
	return OK;
}

STATUS SysOpenWdGuard(u32 dwNoticeTimeout)
{
	return OK;
}

STATUS SysCloseWdGuard(void)
{
	return OK;
}

STATUS SysNoticeWdGuard(void)
{
	return OK;
}

BOOL32 SysWdGuardIsOpened(void)
{
	return FALSE;
}

void   BrdSetWatchdogMode(u8 byMode)
{
	switch( byMode )
	{
	case WATCHDOG_USE_CLK:
		break;
	case WATCHDOG_USE_SOFT:
		break;
	case WATCHDOG_STOP:
		break;
	default:
		break;
	}
	return;
}

void   BrdFeedDog(void)
{

}

STATUS BrdAlarmStateScan(TBrdAlarmState *ptBrdAlarmState)
{
	if( NULL == ptBrdAlarmState )
	{
		return ERROR;
	}
	return OK;
}

STATUS BrdSetFanState(u8 byRunState)
{
	if( BRD_FAN_RUN == byRunState
		|| BRD_FAN_STOP == byRunState )
	{
		return OK;
	}

	return ERROR;
}

BOOL32   BrdFanIsStopped(u8 byFanId)
{
	return FALSE;
}

u8   BrdExtModuleIdentify(void)
{
	return 0xd; // mdsc;
}

void BrdVideoMatrixSet(u8 byInSelect, u8 byOutSelect)
{
	
}

STATUS BrdTimeGet( struct tm* ptGettm )
{
	time_t time_c;
	time( &time_c );
	*ptGettm = *localtime( &time_c );
}

STATUS BrdTimeSet( struct tm* ptGettm )
{
	time_t time_c = mktime( ptGettm );
#ifdef _LINUX_
	s32 nret = stime( &time_c );
#endif
}

STATUS BrdGetAlarmInput(u8 byPort, u8* pbyState)
{
	if( NULL == pbyState )
	{
		return ERROR;
	}
	return OK;
}

STATUS BrdSetAlarmOutput(u8 byPort, u8  byState)
{
	return OK;
}

u32 BrdGetSdramMaxSize(void)
{
	struct sysinfo info;
	s32 nret = sysinfo( &info );
	return info.totalram;
}

STATUS BrdGetAllDiskInfo(TBrdAllDiskInfo *ptBrdAllDiskInfo)
{
	if( NULL == ptBrdAllDiskInfo )
	{ return ERROR; }
	return OK;
}

u32 BrdGetFullFileName(u8 byPutDiskId, s8 *pchInFileName, s8 *pchRtnFileName)
{
	if( pchInFileName == pchRtnFileName )
	{
		return 0;
	}
	return OK;
}

STATUS BrdCopyFile (s8 *pchSrcFile, s8 *pchDesFile)
{
	
}

STATUS BrdBackupFile (s8 *pchSrcFile, s8 *pchDesFile)
{
}

STATUS BrdRestoreFile(s8 *pchSrcFile, s8 *pchDesFile)
{
}

STATUS SysSetUsrAppParam(s8 *pbyFileName, u8 byFileType, s8 *pAutoRunFunc)
{
	return OK;
}

STATUS SysSetMultiAppParam(TAppLoadInf *ptAppLoadInf)
{
	return OK;
}

STATUS BrdGetMemInfo(u32 *pdwByteFree, u32 *pdwByteAlloc)
{
	struct sysinfo info;
	s32 nret = sysinfo( &info );
	*pdwByteFree = info.freeram/(1024*1024);
	*pdwByteAlloc = (info.totalram - info.freeram)/(1024*1024);

	return (0 == nret) ? OK : ERROR;
}

u8 SysGetIdlePercent(void)
{
	return 50;  // ?? how to
}

STATUS BrdGetDeviceInfo (TBrdDeviceInfo* ptBrdDeviceInfo)
{
	if( NULL == ptBrdDeviceInfo )
		return ERROR;
	return OK;
}

STATUS BrdAddUser (s8* pchUserName,s8* pchPassword, TUserType tUserType)
{
	if( NULL == pchUserName )
		return ERROR;
	return OK;
}

STATUS BrdDelUser (s8* pchUserName,s8* pchPassword, TUserType tUserType)
{
	if( NULL == pchUserName )
		return ERROR;
	return OK;
}


u8  BrdGetBSP15CapturePort(u8 byBSP15ID)
{
	return 0;
}

u8  BrdGetSAA7114OutPort(u8 byBSP15ID)
{
	return 0;
}

u8  BrdGetCaptureChipType(u8 byBSP15ID)
{
	return 0;
}

u8  BrdGetAudCapChipType(u8 byBSP15ID)
{
	return 0;
}

u32 BrdBsp15GpdpIsUsed(void)
{
	return 0;
}

u8  BrdGetBSP15VGAConf(u8 byDevId)
{
	return 0;
}

void BrdStopVGACap(void)
{
	
}

void BrdStartVGACap(u8 byMode)
{
}

void BrdSetVGACapMode (u8 byMode)
{
}

void BrdCloseVGA(void)
{
}

void BrdOpenVGA(void)
{
}

void BrdSetBsp15GPIOMode(u8 byMode)
{
}

void BrdMapDevOpenPreInit(u8 byBSP15ID)
{
}

void BrdMapDevClosePreInit(u8 byBSP15ID)
{
}

void  BrdAudioMatrixSet(u8 byAudioInSelect, u8 byAdioOutSelect)
{
}

u8 SysGetOsVer(void)
{
	return 0;
}

u8 BrdGetMatrixType(void)
{
	return 0;
}

s32 BrdGetPowerVoltage(u32 dwSampleTimes)
{
	return 0;
}

STATUS BrdShutoffPower(void)
{
	return OK;
}


u8 BrdGetSwitchSel(void)
{
	return 0;
}

STATUS BrdGetEthParam(u8 byEthId, TBrdEthParam *ptBrdEthParam)
{
	if( NULL == ptBrdEthParam )
		return ERROR;

	char achipaddr[64];
	memset( achipaddr, 0, sizeof(achipaddr) );

	::GetRegKeyString( profilename.data(), host.data(), hostip.data(), "0.0.0.0", achipaddr, 64 );
	ptBrdEthParam->dwIpAdrs = inet_addr( achipaddr );

	memset( achipaddr, 0, sizeof(achipaddr) );
	::GetRegKeyString( profilename.data(), host.data(), hostmask.data(), "0.0.0.0", achipaddr, 64 );
	ptBrdEthParam->dwIpMask = htonl(inet_addr( achipaddr ));
	
	return OK;
}


STATUS BrdGetEthParamAll(u8 byEthId, TBrdEthParamAll *ptBrdEthParamAll)
{
    if( NULL == ptBrdEthParamAll )
        return ERROR;
    
    char achipaddr[64];
    memset( achipaddr, 0, sizeof(achipaddr) );
    
    ptBrdEthParamAll->dwIpNum = 1;

    ::GetRegKeyString( profilename.data(), host.data(), hostip.data(), "0.0.0.0", achipaddr, 64 );    
    ptBrdEthParamAll->atBrdEthParam[0].dwIpAdrs = inet_addr( achipaddr );
    
    memset( achipaddr, 0, sizeof(achipaddr) );
    ::GetRegKeyString( profilename.data(), host.data(), hostmask.data(), "0.0.0.0", achipaddr, 64 );
    ptBrdEthParamAll->atBrdEthParam[0].dwIpMask = inet_addr( achipaddr );
    
	return OK;
}

STATUS BrdSetEthParam(u8 byEthId, u8 byIpOrMac, TBrdEthParam *ptBrdEthParam)
{
	return OK;
}

STATUS BrdDelEthParam(u8 byEthId)
{
	return OK;
}

STATUS BrdSetDefGateway(u32 dwIpAdrs)
{
	return OK;
}

u32 BrdGetDefGateway(void)
{
	char achipaddr[64];
	memset( achipaddr, 0, sizeof(achipaddr) );

	::GetRegKeyString( profilename.data(), host.data(), hostgateway.data(), "0.0.0.0", achipaddr, 64 );
	u32 dwIpAdrs = htonl( inet_addr( achipaddr ) );
	return dwIpAdrs;
}

STATUS BrdDelDefGateway(void)
{
	return OK;
}

u32 BrdGetNextHopIpAddr(u32 dwDstIpAddr,u32 dwDstMask)
{
	return 0;
}

u32 BrdGetRouteWayBandwidth(void)
{
	return 1;
}

STATUS BrdAddOneIpRoute(TBrdIpRouteParam *ptBrdIpRouteParam)
{
	return OK;
}

STATUS BrdDelOneIpRoute(TBrdIpRouteParam *ptBrdIpRouteParam)
{
	return OK;
}

STATUS BrdGetAllIpRoute(TBrdAllIpRouteInfo *ptBrdAllIpRouteInfo)
{
	return OK;
}

u32 BrdChkOneIpStatus(u32 dwIpAdrs)
{
	return OK;
}

STATUS BrdSaveNipConfig(void)
{
	return OK;
}

STATUS BrdIpConflictCallBackReg (TIpConflictCallBack  ptFunc)
{
	return OK;
}

BOOL32 BrdIpOnceConflicted (void)
{
	return OK;
}

BOOL32 BrdPing(s8* pchDestIP,TPingOpt* ptPingOpt,s32 nUserID,TPingCallBack ptCallBackFunc)
{
	return OK;
}

u8 BrdGetEthActLnkStat(u8 byEthID)
{
	return OK;
}


STATUS BrdSetV35Param(u32 dwDevId, u32 dwIpAdrs, u32 dwIpMask)
{
return OK;
}

STATUS BrdGetV35Param(u32 dwDevId, u32 *pdwIpAdrs, u32 *pdwIpMask)
{
	return OK;
}

STATUS BrdDelV35Param(u32 dwDevId)
{
	return OK;
}

BOOL32   BrdLineIsLoop(u32 *pdwIpAddr)
{
	return OK;
}

u8 BrdGetE1MaxNum(void)
{
	return OK;
}

STATUS BrdSetE1SyncClkOutputState (u8 byMode)
{
	return OK;
}

STATUS BrdSetE1RelayLoopMode(u8 byE1ID,u8 byMode)
{
	return OK;
}

STATUS BrdSelectE1NetSyncClk(u8 byE1ID)
{
	return OK;
}

u8  BrdQueryE1Imp(u8 byE1ID)
{
	return OK;
}

u8  BrdGetE1AlmState(u8 byE1Id)
{
	return OK;
}

u8 E1TransGetMaxE1Num(void)
{
	return OK;
}

s32 E1TransChanOpen( u8 byE1ChanID,TE1TransChanParam *ptChanParam,TE1MsgCallBack  ptE1MsgCallBack )
{
	return OK;
}

s32 E1TransChanClose( u8 byE1ChanID)
{
	return OK;
}

s32 E1TransChanMsgSnd(u8 byE1ChanID, TE1TransMsgParam *ptMsgParam)
{
	return OK;
}

s32 E1TransTxPacketNumGet(u8 byE1ChanID)
{
	return OK;
}

s32 E1TransBufQHeadPtrInc(u8 byE1ChanID, u32 dwOffset)
{
	return OK;
}

s32 E1TransBufMsgCopy(u8 byE1ChanID, u8 *pbyDstBuf, u32 dwSize)
{
	return OK;
}

s32 E1TransBufMsgLenGet(u8 byE1ChanID)
{
	return OK;
}

s32 E1TransBufMsgByteRead(u8 byE1ChanID, u32 dwOffset, u8 *pbyRtnByte)
{
	return OK;
}

s32 E1TransChanLocalLoopSet(u8 byE1ChanID, BOOL32 bIsLoop)
{
	return OK;
}

s32 E1TransChanInfoGet(u8 byE1ChanID, TE1TransChanStat *ptInfo)
{
	return OK;
}

STATUS BrdOpenE1SingleLinkChan(u32 dwChanID, TBrdE1SingleLinkChanInfo *ptChanParam)
{
	return OK;
}

STATUS BrdCloseE1SingleLinkChan(u32 dwChanID)
{
	return OK;
}

STATUS BrdGetE1SingleLinkChanInfo(u32 dwChanID, TBrdE1SingleLinkChanInfo *ptChanInfo)
{
	return OK;
}

STATUS BrdOpenE1MultiLinkChan(u32 dwChanID, TBrdE1MultiLinkChanInfo *ptChanParam)
{
	return OK;
}

STATUS BrdCloseE1MultiLinkChan(u32 dwChanID)
{
	return OK;
}

STATUS BrdGetE1MultiLinkChanInfo(u32 dwChanID, TBrdE1MultiLinkChanInfo *ptChanInfo)
{
	return OK;
}

BOOL32   BrdRawFlashIsUsed(void)
{
	return OK;
}

u32 BrdGetFullRamDiskFileName(s8 *pInFileName, s8 *pRtnFileName)
{
	return OK;
}

STATUS BrdFpUnzipFile(s8 *pUnzipFileName, s8 *pOuputFileName)
{
	return OK;
}

BOOL32   BrdFpUnzipFileIsExist(s8 *pUnzipFileName)
{
	return OK;
}

STATUS BrdFpPartition(TFlPartnParam *ptParam)
{
	return OK;
}

STATUS BrdFpGetFPtnInfo(TFlPartnParam *ptParam)
{
	return OK;
}

s32  BrdFpGetExecDataInfo(u32 *pdwExecAdrs, u32 *pdwExeCodeType)
{
	return OK;
}

s32  BrdFpReadExecData(u8 *pbyDesBuf, u32 dwLen)
{
	return OK;
}

STATUS BrdFpUpdateExecData(s8* pFile)
{
	return OK;
}

STATUS BrdFpEraseExeCode(void)
{
	return OK;
}

STATUS BrdFpUpdateAuxData(s8* pFile)
{
	return OK;
}

u8  BrdFpGetUsrFpnNum (void)
{
	return OK;
}

STATUS BrdFpWriteDataToUsrFpn(u8 byIndex, u8 *pbyData, u32 dwLen)
{
	return OK;
}

s32  BrdFpReadDataFromUsrFpn(u8 bySection, u8 *pbyDesBuf, u32 dwLen)
{
	return OK;
}


u8 BrdMPCQueryAnotherMPCState(void)
{
	return BRD_MPC_IN_POSITION;
}

u8 BrdMPCQueryLocalMSState (void)
{
	return BRD_MPC_RUN_MASTER;
}

/*================================
函数名      : BrdMPCSetLocalMSState
功能        ：设置当前主处理机板主备用状态
算法实现    ：（可选项）
引用全局变量：
输入参数说明：byState:
              #define BRD_MPC_RUN_MASTER          0      // 当前主处理机板主用
              #define BRD_MPC_RUN_SLAVE           1      // 当前主处理机板备用 
返回值说明  ：OK/ERROR
==================================*/
STATUS BrdMPCSetLocalMSState (u8 byState)
{
	return OK;
}

/*================================
函数名      : BrdMPCQuerySDHType
功能        ：光模块种类查询
算法实现    ：（可选项）
引用全局变量：
输入参数说明：无
返回值说明  ：宏定义：
		#define BRD_MPC_SDHMODULE_NONE      ((u8)0x07)   // 当前主处理机板没有插模块 
        #define BRD_MPC_SDHMODULE_SOI1      ((u8)0x00)   // 当前主处理机板插模块SOI1 
        #define BRD_MPC_SDHMODULE_SOI4      ((u8)0x01)   // 当前主处理机板插模块SOI4  
==================================*/
u8 BrdMPCQuerySDHType (void)
{
	return BRD_MPC_SDHMODULE_NONE;
}

/*================================
函数名      : BrdMPCQueryNetSyncMode
功能        ：锁相环工作模式查询
算法实现    ：（可选项）
引用全局变量：
输入参数说明：无
返回值说明  ：宏定义：
		#define SYNC_MODE_FREERUN           ((u8)0)   // 自由振荡，对于MCU应设置为该模式，所有下级线路时钟与该MCU同步
        #define SYNC_MODE_TRACK_SDH8K       ((u8)1)   // 跟踪模式，作为下级MCU应设置为该模式，跟踪SDH来的8K时钟
        #define SYNC_MODE_TRACK_SDH2M       ((u8)2)   // 跟踪模式，作为下级MCU应设置为该模式，跟踪SDH来的2M时钟 
        #define SYNC_MODE_TRACK_DT2M        ((u8)4)   // 跟踪模式，作为下级MCU应设置为该模式，跟踪DT来的2M时钟 
        #define SYNC_MODE_UNKNOWN           ((u8)0xff)// 未知或错误的模式 
==================================*/
u8 BrdMPCQueryNetSyncMode (void)
{
	return SYNC_MODE_FREERUN;
}

/*================================
函数名      : BrdMPCSetNetSyncMode
功能        ：锁相环网同步模式选择设置，不支持SDH部分
算法实现    ：（可选项）
引用全局变量：
输入参数说明：byMode：锁相环网同步模式，有如下定义：
        #define SYNC_MODE_FREERUN           ((u8)0)   // 自由振荡，对于主MCU应设置为该模式，所有下级线路时钟与该MCU同步 
        #define SYNC_MODE_TRACK_SDH8K       ((u8)1)   //跟踪模式，作为下级MCU应设置为该模式，跟踪SDH来的8K时钟 
        #define SYNC_MODE_TRACK_SDH2M       ((u8)2)   // 跟踪模式，作为下级MCU应设置为该模式，跟踪SDH来的2M时钟 
        #define SYNC_MODE_TRACK_DT2M        ((u8)4)   // 跟踪模式，作为下级MCU应设置为该模式，跟踪DT来的2M时钟
返回值说明  ：错误返回ERROR；成功返回OK
==================================*/
STATUS BrdMPCSetNetSyncMode(u8 byMode)
{
	return OK;
}

/*================================
函数名      : BrdMPCSetAllNetSyncMode
功能        ：锁相环网同步模式选择设置,增加sdh部分
算法实现    ：（可选项）
引用全局变量：
输入参数说明：byMode：锁相环网同步模式，有如下定义：
        #define SYNC_MODE_FREERUN           ((u8)0)   // 自由振荡，对于主MCU应设置为该模式，所有下级线路时钟与该MCU同步 
        #define SYNC_MODE_TRACK_SDH8K       ((u8)1)   // 跟踪模式，作为下级MCU应设置为该模式，跟踪SDH来的8K时钟 
        #define SYNC_MODE_TRACK_SDH2M       ((u8)2)   // 跟踪模式，作为下级MCU应设置为该模式，跟踪SDH来的2M时钟 
        #define SYNC_MODE_TRACK_DT2M        ((u8)4)   // 跟踪模式，作为下级MCU应设置为该模式，跟踪DT来的2M时钟
        dwSdhE1Id：对于同步模式为SYNC_MODE_TRACK_SDH8K和SYNC_MODE_TRACK_SDH2M时必须指定sdh对应的e1号，如果是SOI-1模
        块范围为241-301；如果是SIO-4模块时范围为241-484
返回值说明  ：错误返回ERROR；成功返回OK
==================================*/
STATUS BrdMPCSetAllNetSyncMode(u8 byMode, u32 dwSdhE1Id)
{
	return OK;
}

STATUS BrdMPCResetSDH (void)
{
	return OK;
}

STATUS BrdMPCReSetAnotherMPC (void)
{
	return OK;
}

STATUS BrdMPCOppReSetDisable (void)
{
	return OK;
}

STATUS BrdMPCOppReSetEnable (void)
{
	return OK;
}


STATUS BrdMPCLedBoardSpeakerSet(u8 byState)
{
	return OK;
}

STATUS BrdSetSecondEthParam(u8 byEthId, u8 byIpOrMac, TBrdEthParam *ptBrdEthParam)
{
    return OK;
}






