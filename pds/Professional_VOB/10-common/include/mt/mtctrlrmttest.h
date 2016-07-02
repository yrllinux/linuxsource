        #ifndef _MT_INTEGRATION_TEST_H
#define _MT_INTEGRATION_TEST_H

#include "kdvdef.h"
#include "mtctrllib.h"
#include "ummessagestruct.h"
#include "evusermanage.h"
#include "addrbook.h"

#define VER_MT_TESTLIB "testlib36.10.01.04.00.050412"
API void testlibver();

#pragma pack(1)

//终端编号
typedef struct tagTerLabel
{
	u8	byMcuNo;
	u8	byTerNo;
}IT_TERLABEL,*IT_PTERLABEL;

//终端信息
typedef struct tagTerInfo
{
	IT_TERLABEL label;	//编号
	s8	name[32];		//名字
}IT_TERINFO,*IT_PTERINFO;

// 通道状态结构(0:关闭,1:打开)
typedef struct tagChannelState
{
	u8		byVideoSend;	// 视频发送通道
	u8		byVideoRecv;	// 视频接收通道
	u8		byAudioSend;	// 音频发送通道
	u8		byAudioRecv;	// 音频接收通道
	u8		byH224Send;		// H224发送通道
	u8		byH224Recv;		// H224接收通道
}IT_CHANSTATE,*IT_PCHANSTATE;

// 时间标准格式
typedef struct SysTime
{
	u8 bySec;	/* seconds after the minute	- [0, 59] */
	u8 byMin;	/* minutes after the hour	- [0, 59] */
	u8 byHour;	/* hours after midnight		- [0, 23] */
	u8 byDay;	/* day of the month		- [1, 31] */
	u8 byMon;	/* months since January		- [0, 11] */
	u8 byYear;	/* years since 1900	*/
}IT_SYSTIME,*IT_PSYSTIME;

//会议状态
typedef struct tagConfState
{
	u8				byMtBoardType;	//终端板卡类型(0:win,1:8010,2:8010a,3:8018,4:imt,5:8010c)
	u8				byMtStat;		//终端状态
	u8				byConfMode;		//会议模式,默认为P2P
	u8				bRasReg;		//是否在GK上注册成功
	u8				bLoopTest;		//是否在自环
	u16				wConfRate;		//会议速率
	u16				wTerNum;		//与会终端个数
	IT_TERINFO		atTerList[ CONF_MT_CAPACITY ];	//终端列表	
	IT_TERLABEL		tLocal;			//本端编号
	IT_TERLABEL		tChair;			//主席
	IT_TERLABEL		tBroadcaster;	//发言人
	IT_TERLABEL		tViewing;		//本端正在观看的终端
	u8				bChimeIn;		//是否在插话
	u8				bVac;			//是否在语音激励
	u8				bDual;			//双流状态,包括发起者和接收者
	u8				byDualSrc;		//双流源(1:VGA,2:PC,3:Video)
	u8				byDualProtocol;	//双流使用的协议(1:h245,2:h239)
	u8				byDualMode;		//双流显示模式(1:单屏双显,2:双屏双显)
	u8				bMtcConnect;	//终控连接状态
	u8				byEncryptMode;	//加密类型(1:不加密,2:AES,3:DES)
	CLoginRequest	cLoginUser;		//登陆用户信息
	IT_SYSTIME		tSysTime;		//系统时间
	IT_CHANSTATE	tChanState;		// 通道状态
}IT_CONFSTAT,*IT_PCONFSTAT;

//轮循状态
typedef struct  tagPollState
{
	u8		bPolling;		//轮询状态
	u8		byPollMedia;	//轮循媒体模式
	u8		byPollIntvl;	//轮询间隔
	u8		byPollNum;		//轮询的终端数
	IT_TERLABEL	atPollList[CTRL_POLL_MAXNUM];	//轮询列表
}IT_POLLSTAT,*IT_PPOLLSTAT;

//编码状态
typedef struct tagEncState
{
	u8	byDevId;		//编码器ID
	u8	byVideoFormat;	//视频格式
	u8	byAudioFormat;	//音频格式
	u8	byEncVol;		//编码音量
	u8	bMute;			//是否哑音
	u8	bAudioEncoding;	//是否音频编码
	u8	bVideoEncoding;	//是否视频编码
	u8	bAudioSending;	//是否发送音频
	u8	bVideoSending;	//是否发送视频
	u16	wBitRate;		//编码码率(网络序)
}IT_ENCSTAT,*IT_PENCSTAT;

//解码状态
typedef struct tagDecState
{
	u8	 byDevId;			//解码器ID
	u8	 byVideoFormat;		//视频格式
	u8	 byAudioFormat;		//音频格式
	u8	 byDecVol;			//解码音量
	u8	 bQuiet;			//是否静音
	u8	 bAudioDecoding;	//是否音频解码
	u8	 bVideoDecoding;	//是否视频解码
	u8	 bAudioRecving;		//是否接收音频
	u8	 bVideoRecving;		//是否接收视频
	u8	 bAecEnable;		//是否开启回声抵消
}IT_DECSTAT,*IT_PDECSTAT;

//摄像头状态
typedef struct tagCameraState
{
	struct CamStat
	{
		u8	byNo;			//摄像头号(1-4)
		u8	bStat;			//是否连接 1:连接 0:未连接
	};
	u8		byCtrlSrc;		//控制源(本地|远端)
	u8		byLocalCtrlNo;	//当前控制的本地摄像头(1-6)
	u8		byRemoteCtrlNo;	//当前控制的远端摄像头(1-6)
	u8		byCamNum;		//配置的摄像头个数
	u8		bFecc;			//是否允许远遥 1:yes 0:no
	CamStat	atCamList[4];	//摄像头状态
}IT_CAMERASTAT,*IT_PCAMERASTAT;

//矩阵状态
typedef struct tagMatrixState
{
	u8		bStat;			//是否连接 1:连接 0:未连接
	u8		byVideoSrc;		//当前视频源
	u8		byAudioSrc;		//当前音频源
	_TMATRIXSCHEME	tCurrentScheme;	//当前使用的方案
}IT_MATRIXSTAT,*IT_PMATRIXSTAT;

// CODECMANAGER状态
typedef struct tagCodecState
{
	u32 dwAudioOutputPower;	//	音频输出功率 
	u32 dwAudioInputPower;	//	音频输入功率

}IT_CODECSTATE,*IT_PCODECSTATE;

// 地址簿状态
typedef struct tagAddrBookState
{
	u32		dwAddrEntryNum;			// 当前地址簿中条目的个数(网络序)
	u32		dwAddrGroupNum;			// 当前地址簿中会议组的个数(网络序)
	u8		bAddrEntryIsExist;		// 查询某个条目是否存在(1:存在,0:不存在)
	u8		bAddrGroupIsExist;		// 查询某个会议组是否存在(1:存在,0:不存在)
	u8      abyEntryInfo[ 1024 ];	// 用于返回查询目标条目的信息(网络序)
}IT_ADDRBOOK,*IT_PADDRBOOK;


#pragma pack()

class CMtCtrlRmtTest
{
public:
	CMtCtrlRmtTest() : m_dwTesterNode( INVALID_NODE ) {}
	virtual ~CMtCtrlRmtTest() {}
	/////////////////////////////链路建立和撤销////////////////////////////////
	BOOL LinkInstance( u32 dwIp, u16 wPort = PORT_MT );
	void DisLinkInstance( void );
	BOOL IsLinked();
	void SetTesterNode( u32 dwNodeId ) { m_dwTesterNode = dwNodeId; }

	/////////////////////////////详细查询接口//////////////////////////////////

	// 终端配置
	//////////////////////////////////////////////////////////////////////////
	//终端信息配置
	BOOL	TerInfoCfgQuery( _PTTERCFG ptTerCfg );
	//网络配置
	BOOL	NetCfgQuery( _PTNETCFG ptNetCfg );
	//呼叫配置
	BOOL	CallCfgQuery( _PTCALLCFG ptCallCfg );
	//视频配置
	BOOL	VideoCfgQuery( _PTVENCPARAM ptVEncCfg, _PTVDECPARAM ptVDecCfg, u8 byId = 0 );
	//音频配置
	BOOL	AudioCfgQuery( _PTAENCPARAM ptAEncCfg, _PTADECPARAM ptADecCfg );
	//摄像头配置
	BOOL	CameraCfgQuery( _PTDEVCFG ptCameraCfg, u8 byNo );
	//矩阵配置
	BOOL	MatrixCfgQuery( _PTDEVCFG ptMatrixCfg );
	//显示配置
	BOOL	OsdCfgQuery( _PTTERCFG ptOsdCfg );

	// 终端基本状态
	//////////////////////////////////////////////////////////////////////////
	//会议状态
	BOOL		ConfStatQuery( IT_PCONFSTAT ptConfState );		
	//编码状态
	BOOL	EncodeStatQuery( IT_PENCSTAT ptEncState, u8 byId = 0 );
	//解码状态
	BOOL	DecodeStatQuery( IT_PDECSTAT ptDecState, u8 byId = 0 );
	//摄像头状态
	BOOL	CameraStatQuery( IT_PCAMERASTAT ptCamState );
	//矩阵状态
	BOOL	MatrixStatQuery( IT_PMATRIXSTAT ptMatrixState );
	//轮循状态
	BOOL	PollStatQuery( IT_PPOLLSTAT ptPollState );
	//短消息查询
	BOOL	LastSmsQuery( s8* sms,const u16 wBufLen );

	// Codec状态查询
	BOOL	CodecStateQuery( IT_PCODECSTATE ptCodecState );
	

	//////////////////////////////////简要查询接口////////////////////////////////////

	// 会议状态
	//////////////////////////////////////////////////////////////////////////
	// 获取通道状态
	BOOL GetChanState( IT_CHANSTATE *ptChanState = NULL );
	// 是否空闲
	BOOL IsIdle();
	// 是否在呼叫进行中
	BOOL IsCalling();
	// 是否在公共过程中
	BOOL IsInCommon();
	// 是否在会议中(包括点对点和多点会议)
	BOOL IsInConf();
	// 是否在断链过程中
	BOOL IsUnLinking();
	// 是否在待机
	BOOL IsInSleep();
	// 是否在多点会议中
	BOOL IsInMultConf();
	// 是否在p2p
	BOOL IsInP2PConf();
	// GK是否注册成功
	BOOL IsGKRegSuccess();
	// 是否在自环过程中
	BOOL IsInSelfTest();
	//获取当前会议码率
	BOOL GetConfRate( u16 *pwRate = NULL );
	// 获取终端列表,返回-1表示失败
	int GetTerList( IT_TERINFO* ptTerList = NULL, u16 wListLen = 0 );
	// 获取本终端mt号
	BOOL GetLocal( IT_TERINFO *ptTerInfo = NULL );
	// 获取发言人
	BOOL GetSpeaker( IT_TERINFO *ptTerInfo = NULL );
	// 获取主席
	BOOL GetChairman( IT_TERINFO *ptTerInfo = NULL );
	// 获取当前视频源(当前终端看到的图像来源)
	BOOL GetVideoSrc( IT_TERINFO *ptTerInfo = NULL );
	// 是否发言人
	BOOL IsSpeaker();
	// 是否主席
	BOOL IsChairman();
	// 是否插话
	BOOL IsChimeIn();
	// 是否在语音激励
	BOOL IsVac();
	
	// 是否双流
	BOOL IsDual();
	// 获取双流源类型
	BOOL GetDualSrcType( u8 *pbyType = NULL );
	// 获取双流协议类型
	BOOL GetDualProtocolType( u8 *pbyType = NULL );
	// 获取双流显示模式
	BOOL GetDualMode( u8 *pbyType = NULL );

	// 是否是加密会议
	BOOL IsEncryptConf();
	// 加密模式
	BOOL GetEncryptMode( u8 *pbyType = NULL );
	// 获取终端板卡类型
	BOOL GetMtBoardType( u8* pbyType = NULL );

	// 获取当前第一路视频编码格式
	BOOL GetMainVideoEncType( u8 *pbyType = NULL );
	// 获取当前第二路视频编码格式
	BOOL GetSecVideoEncType( u8 *pbyType = NULL );
	// 获取当前第一路视频解码格式
	BOOL GetMainVideoDecType( u8 *pbyType = NULL );
	// 获取当前第二路视频解码格式
	BOOL GetSecVideoDecType( u8 *pbyType = NULL );
	// 获取当前第一路音频编码格式
	BOOL GetMainAudioEncType( u8 *pbyType = NULL );
	// 获取当前第一路音频解码格式
	BOOL GetMainAudioDecType( u8 *pbyType = NULL );

	//是否静音 
	BOOL IsLocalSpeakerMute();
	//是否哑音
	BOOL IsLocalMicMute();
	//获得扬声器音量
	BOOL GetLocalSpeakerVolume(s32 *pnVolNum);
	//获得麦克风音量
	BOOL GetLocalMicVolume(s32 *pnVolNum);
	
	//查询终控是否已连上终端
	BOOL IsMtcConnectedMt();
	//获得当前登陆用户
	BOOL GetConnectedUser(CLoginRequest *pcLoginuser);
	//获得地址簿条目总数
	BOOL GetEntriesCount(s32 *pnCount);
	//查询一条目是否存在
	BOOL  IsEntryExist(CAddrEntry *pcEntry);
	//获得组总数
	BOOL GetGroupCount(s32 *pnCount);	
	//查询一组是否存在
	BOOL IsGroupExist(CAddrMultiSetEntry *pcAddrMentry);

	// 获取系统时间
	BOOL GetSysTime( IT_SYSTIME* ptSysTime );
	
protected:
	void	PostMsg(u16 wEvent,void* pContent = NULL,u16 wLen =0);
	BOOL	IntTestQuery(u16  wEvent,
						 void* buf,
						 u16  wBufLen,
						 u16* pwRetLen,
						 const void* pParam = NULL,
						 u16  wParamLen = 0,
						 u16  wAppId = AID_MT_CTRL );
protected:
	u32	m_dwTesterNode;

};

#endif 