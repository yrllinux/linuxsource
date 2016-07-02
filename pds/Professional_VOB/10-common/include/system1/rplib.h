/*****************************************************************************
模块名      : RPLIB
文件名      : RPlib.h
相关文件    : RPlib.cpp
文件实现功能: 录放相库提供给上层的头文件
作者        : 魏治兵
版本        : V2.0  Copyright(C) 2003-2005 KDC, All rights reserved.
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2002/07/01  0.9         魏治兵      Create
2003/07/11  2.0         魏治兵      代码注释与优化
2003/10/14  2.0         万春雷      增加接口
******************************************************************************/

#ifndef _RPLib_H_
#define _RPLib_H_



//模块版本信息和编译时间 及 依赖的库的版本和编译时间
extern "C" __declspec( dllexport ) void rplibver();


//模块帮助信息 及 依赖的库的帮助信息
extern "C" __declspec( dllexport ) void rplibhelp();



#define  RP_MAX_RECORDER_NUMBER	            8//
#define  RP_MAX_PLAYER_NUMBER	            8//
#define  RP_MAX_RECORDER_AND_PLAYER_NUMBER  16//


#define  RECORDER_MIN_SPACE     		    (u32)200 // 录像的保留空间 


#define  RP_OK                 		        (u16)0    // RP库操作成功 
#define  RPERR_BASE_CODE       		        (u16)20000// 错误码基准值

//录像机、放像机总数超高，无法设置录像机、放像机总数
#define  RPERR_EXCEEDMAXRECORDER_PLAYERNUM  (u16)(RPERR_BASE_CODE + 1)
//内存分配错误
#define  RPERR_OUTOFMEMORY                  (u16)(RPERR_BASE_CODE + 2)
//管理器为空,可能没有初始化RP库
#define  RPERR_RPNULLPTRMANAGER             (u16)(RPERR_BASE_CODE + 3)
//超过最大的Recorder数目
#define  RPERR_EXCEEDMAXRECORDERNUM         (u16)(RPERR_BASE_CODE + 4)
//超过最大的Player数目
#define  RPERR_EXCEEDMAXPLAYERNUM           (u16)(RPERR_BASE_CODE + 5)
//不能找到RecorderID, 错误的ID或者已经释放
#define  RPERR_NOTFOUNDRECORDERID           (u16)(RPERR_BASE_CODE + 6)
//不能找到PlayerID, 错误的ID或者已经释放
#define  RPERR_NOTFOUNDPLAYERID             (u16)(RPERR_BASE_CODE + 7)
//错误的创建AVI文件
#define  RPERR_FAILURECREATEAVIFILE         (u16)(RPERR_BASE_CODE + 8)
//错误的创建视频Socket
#define  RPERR_FAILURECREATEVIDEOSOCKET     (u16)(RPERR_BASE_CODE + 9) 
//错误的创建音频Socket
#define  RPERR_FAILURECREATEAUDIOSOCKET     (u16)(RPERR_BASE_CODE + 10)
//非法的Socke句柄
#define  RPERR_INVALIDSOCKETHANDLE          (u16)(RPERR_BASE_CODE + 11)
//失败的设置Socket读事件
#define  RPERR_FAILURESETREADEVENT          (u16)(RPERR_BASE_CODE + 12)
//录象器启动失败
#define  RPERR_RECORDERSTARTFAILED          (u16)(RPERR_BASE_CODE + 13) 
//录象器暂停失败
#define  RPERR_RECORDERPAUSEFAILED          (u16)(RPERR_BASE_CODE + 14)
//录象器继续失败
#define  RPERR_RECORDERCONTINUEFAILED       (u16)(RPERR_BASE_CODE + 15)
//录象器停止失败
#define  RPERR_RECORDERSTOPFAILED           (u16)(RPERR_BASE_CODE + 16)
//录像器已经被删除从管理器中
#define  RPERR_NOFOUNDRECORDER              (u16)(RPERR_BASE_CODE + 17)
//非法的录像模式
#define  RPERR_INVAIDRECORDEREMODE          (u16)(RPERR_BASE_CODE + 18)
//坏的AVI文件  
#define  RPERR_BADAVIFILE                   (u16)(RPERR_BASE_CODE + 19)

#define RECORDER_ALREADY_EXIT               (u16)(RPERR_BASE_CODE + 20)
#define RECORDER_CANNOT_BE_FOUND            (u16)(RPERR_BASE_CODE + 21)

#define PLAYER_ALREADY_EXIT                 (u16)(RPERR_BASE_CODE + 22)
#define PLAYER_CANNOT_BE_FOUND              (u16)(RPERR_BASE_CODE + 23)
//不正确的recorder ID 
#define RPERR_INVALID_RECORDER_ID           (u16)(RPERR_BASE_CODE + 24)
//定时器参数不正确    
#define RPERR_TIMER_SET_PARAM               (u16)(RPERR_BASE_CODE + 25)
//定时器注册的线程满
#define RPERR_TIMER_FULL_THREAD             (u16)(RPERR_BASE_CODE + 26)
//socket startup error; 
#define RPERR_SOCK_INIT                     (u16)(RPERR_BASE_CODE + 27)
//录像线程为空
#define RPERR_REC_THREAD_NULL               (u16)(RPERR_BASE_CODE + 28)
//设置Mediasignal param不正确
#define RPERR_MEDIA_SIGNAL_PARAM            (u16)(RPERR_BASE_CODE + 29)
//分配内存出错
#define RPERR_NO_MEMORY                     (u16)(RPERR_BASE_CODE + 30)
//创建网络收发模块error;
#define RPERR_MEDIANET_CREATE               (u16)(RPERR_BASE_CODE + 31)
//网络收发模块设置参数出错
#define RPERR_MEDIANET_SETPARAM             (u16)(RPERR_BASE_CODE + 32)
//Mediasignal 创建出错
#define RPERR_MEDIASIGNAL_CREATE            (u16)(RPERR_BASE_CODE + 33)
//创建循环缓冲出错  
#define RPERR_RPBUF_PARAM                   (u16)(RPERR_BASE_CODE + 34)
//循环缓冲空
#define RPERR_RPBUF_NULL                    (u16)(RPERR_BASE_CODE + 35)
//循环缓冲混乱
#define RPERR_RPBUF_INVALID                 (u16)(RPERR_BASE_CODE + 36)
//循环缓冲满
#define RPERR_RPBUF_FULL                    (u16)(RPERR_BASE_CODE + 37)
//无效的播放id;
#define RPERR_INVALID_PALYER_ID             (u16)(RPERR_BASE_CODE + 38)
//播放线程空
#define RPERR_PLAY_THREAD_NULL              (u16)(RPERR_BASE_CODE + 39)
//playsignal 创建出错
#define RPERR_PLAYSIGNAL_CREATE             (u16)(RPERR_BASE_CODE + 40)
//路径空 
#define RPERR_LISTTAPE_PATH_NULL            (u16)(RPERR_BASE_CODE + 41)
//错误的路径名
#define RPERR_LISTTAPE_PATH_INVALID         (u16)(RPERR_BASE_CODE + 42)
//目前还不支持
#define RPERR_NO_SURPORT                    (u16)(RPERR_BASE_CODE + 43)
//获取磁盘剩余空间出错
#define RPERR_FREE_SPACE                    (u16)(RPERR_BASE_CODE + 44) 
//删除一个录像文件出错
#define RPERR_DELETE_ONE_FILE               (u16)(RPERR_BASE_CODE + 45) 
//设置录像器录制方法参数出错
#define RPERR_RECORD_METHOD_PARAM           (u16)(RPERR_BASE_CODE + 46) 

//设置文件发布的数据库操作参数无效
#define RPERR_OPEN_DB_PARAM		            (u16)(RPERR_BASE_CODE + 47) 
//打开数据库连接的操作失败
#define RPERR_OPEN_DB			            (u16)(RPERR_BASE_CODE + 48) 
//数据库没有进行连接操作，仍处于关闭状态
#define RPERR__DB_CLOSE_STATUS		        (u16)(RPERR_BASE_CODE + 49) 
//发布文件操作失败
#define RPERR__DB_INSERT_METHOD		        (u16)(RPERR_BASE_CODE + 50) 
//取消发布文件操作失败
#define RPERR__DB_CANCEL_METHOD		        (u16)(RPERR_BASE_CODE + 51) 
//查询文件发布状态操作失败
#define RPERR__DB_QUERY_METHOD		        (u16)(RPERR_BASE_CODE + 52) 

#define RPERR_SET_VID_ACTIVEPT              (u16)(RPERR_BASE_CODE + 53) // 设置视频收/发动态载荷失败
#define RPERR_SET_AUD_ACTIVEPT              (u16)(RPERR_BASE_CODE + 54) // 设置音频收/发动态载荷失败

#define RPERR_SET_VID_ENCRYPTKEY            (u16)(RPERR_BASE_CODE + 55) // 设置视频加密key失败
#define RPERR_SET_AUD_ENCRYPTKEY            (u16)(RPERR_BASE_CODE + 56) // 设置音频加密key失败
#define RPERR_SET_VID_DECRYPTKEY            (u16)(RPERR_BASE_CODE + 57) // 设置视频解密key失败
#define RPERR_SET_AUD_DECRYPTKEY            (u16)(RPERR_BASE_CODE + 58) // 设置音频解密key失败

// 录像器和放像器的回调通知返回值
enum EmRecordMode
{
	rVideoOnly = 1, 
	rAudioOnly = 2,
	rBoth      = 3,	
	rNothing   = 100,
};

// 状态
enum EmState
{
	rRunning		= 10,	//录像器运行
	rPausing		= 11,	//录像器暂停
	rStopping		= 12,   //录像器停止
	
	pRunning		= 20,   //放像器运行
	pPausing		= 21,   //放像器暂停 
	pStopping		= 22,   //录像器停止
	pEnding			= pStopping, 
	
	pForwarding		= 23,   //快进
	pBackwarding	= 24,   //快退
};

// 定义设置文件位置的基准点
enum EMPlayerStartPoint
{
	BEGIN_POSITION = 0,
	CURRENT_POSITION = 1,
	END_POSITION = 2,
};


enum EMNotifyType
{
	emNotify_RecorderOK		    = 1,   //录像状态改变
	emNotify_RecorderFullDisk	= 2,   //磁盘满

	emNotify_PlayerOK       	= 1000,	//放像状态改变 
	emNotify_PlayerEmptyFile	= 1001, //空文件, 异常
	emNotify_PlayerFinishTask	= 1002, //完成任务
	emNotify_PlayerPlayProcess	= 1003, //放像进程
};

typedef struct stMP4SourceInfo
{
	u8 SignalID;//从0开始编号
	//CString strSignalName;
	u16 wLocalAudioPort;
	u16 wLocalVideoPort;
	u32 dwVideoIPAddress;
	u16 wVideoPort;
	u32 dwAudioIPAddress;
	u16 wAudioPort;
	EmRecordMode emMode;
} MP4SOURCEINFO, *PMP4SOURCEINFO;

#define MAX_SIGNAL_NUMBERS 4

typedef struct stMP4Sources
{
	u8 SignalNumber;
	MP4SOURCEINFO m_MP4SourceInfo[MAX_SIGNAL_NUMBERS];
}MP4SOURCES;

/*统计信息*/
typedef struct tagRPLibKdvRcvStatistics
{
	u32      m_dwPackNum;/*已接收的包数*/
	u32      m_dwPackLose;/*丟包数*/
}TRPLibKdvRcvStatistics;

// 录像器状态
struct TRecorderStatus
{
	u16	RecorderID;
	u32	dwRecordSeconds;			//一共录的时间长度(单位秒)
	s8	chCurrentRecordFileName[MAX_PATH];	//当前正在录的文件的名字
	EmState emRecordState;			//状态枚举变量
	u32 	dwFileSize;				//file length already recorded	

	TRPLibKdvRcvStatistics tVideoRcvStatistics;
	TRPLibKdvRcvStatistics tAudioRcvStatistics;

	MP4SOURCES tRecordAddressInfo;  //音视频收发地址信息
};

// 放像器状态
struct TPlayerStatus
{
	u16	PlayerID;
	u32	dwPlaySeconds;         //一共放的时间长度(单位秒)
	s8	chCurrentPlayFileName[MAX_PATH]; //当前正在放的文件的名字
	
	u32	dwVideoPacketNum;      //视频包数目
	u32	dwAutioPacketNum;      //音频包数目
	EmState emPlayerState;     // 状态，可为运行、暂停、停止
	
	u32	dwTapeLengthInSeconds; //录像时间长度(单位秒)

	MP4SOURCES tPlayerAddressInfo; //音视频收发地址信息
};

struct TRPLibNotification
{
	EMNotifyType    emNotificationType;
	TRecorderStatus tRecorderStatus; 
	TPlayerStatus   tPlayerStatus; 
};

#ifndef DES_ENCRYPT_MODE
#define DES_ENCRYPT_MODE         (u8)0      //DES加密模式
#define AES_ENCRYPT_MODE         (u8)1      //AES加密模式
#define ENCRYPT_KEY_SIZE         (u8)32     //密钥长度 取其中的较大值
#define DES_ENCRYPT_KEY_SIZE     (u8)8      //DES密钥长度
#define AES_ENCRYPT_KEY_SIZE_MODE_A (u8)16  //AES Mode-A 密钥长度
#define AES_ENCRYPT_KEY_SIZE_MODE_B (u8)24  //AES Mode-B 密钥长度
#define AES_ENCRYPT_KEY_SIZE_MODE_C (u8)32  //AES Mode-C 密钥长度
#endif


/*
定义录放像器的回调涵数
wRecorderID 录像器的ID
emType 通知类型
*/
typedef VOID (CALLBACK* RPCallBack) ( u16 wID, TRPLibNotification* pRPNotification, 
				                      u32 pParam );

//----------------------------------------------------------------------
u16 rpInit(u16 wMaxRecorder = 0x1, u16 wMaxPlayer = 0x1); //初始化录放像库
u16 rpRelease(VOID); 					  //反化录放像库--减少引用计数器

/*
注册录像器回调涵数
pCallBack 录像器回调涵数指针
*/
u16 rpRegisterRecorderCallBack(RPCallBack pCallBack, const u32 pParam); 

/*
创建录像器
wRecorderID 录像器ID号
*/
//有外部传进ID
u16 rpCreateRecorder(u16 wRecorderID); 

/*
销毁录像器
wRecorderID 录像器ID号
*/
u16 rpDestroyRecorder(u16 wRecorderID); 


/*
开始录像
wRecorderID 录像器ID号
pFileName 录的文件路径 and it includes directory info!! if there is no directory, then it will return fales.
dwIpAddress IP地址
wPort 端口号
RecorMode Record模式
*/
u16 rpStartRecord(u16 wRecorderID, s8 *pszFileName, MP4SOURCES &tMP4Sources); 
u16 rpStartRecord(u16 wRecorderID, s8 *pszFileName, 
		          u32 dwVideoIPAddress, u16 wVideoPort, u32 dwAudioIPAddress, u16 wAudioPort, 
		          EmRecordMode emMode); 
u16 rpStartRecord(u16 wRecorderID, s8 *pszFileName, 
		          u32 dwIPAddress, u16 wPort, 
		          EmRecordMode emMode); 

/* 
设置 视频 动态视频载荷的 Playload值
wRecorderID   录像器ID号
byRmtActivePT 接收到的动态载荷的Playload值, 由对呼时约定
byRealPT      该动态载荷实际代表的的Playload类型，等同于我们发送时的PT约定
*/
u16 rpSetVidRcvActivePT( u16 wRecorderID, u8 byRmtActivePT, u8 byRealPT );

/* 
设置 音频 动态视频载荷的 Playload值
wRecorderID   录像器ID号
byRmtActivePT 接收到的动态载荷的Playload值, 由对呼时约定
byRealPT      该动态载荷实际代表的的Playload类型，等同于我们发送时的PT约定
*/
u16 rpSetAudRcvActivePT( u16 wRecorderID, u8 byRmtActivePT, u8 byRealPT );

/* 
设置 视频解码key字串 以及 解密模式 Aes or Des
wRecorderID   录像器ID号
pszKeyBuf     解密密钥缓冲指针
pszKeyBuf     解密密钥长度
byDecryptMode 解密模式
*/
u16 rpSetVidDecryptKey( u16 wRecorderID, s8 *pszKeyBuf, u16 wKeySize, u8 byDecryptMode );

/* 
设置 音频解码key字串 以及 解密模式 Aes or Des
wRecorderID   录像器ID号
pszKeyBuf     解密密钥缓冲指针
pszKeyBuf     解密密钥长度
byDecryptMode 解密模式
*/
u16 rpSetAudDecryptKey( u16 wRecorderID, s8 *pszKeyBuf, u16 wKeySize, u8 byDecryptMode );

/* 
暂停录像
wRecorderID 录像器ID号
*/
u16 rpPauseRecord(u16 wRecorderID); 

/*
继续录像
wRecorderID 录像器ID号
*/
u16 rpResumeRecord(u16 wRecorderID); 

/*
停止录像
wRecorderID 录像器ID号
*/
u16 rpStopRecord(u16 wRecorderID); 

/*
得到录像器的状态
wRecorderID 录像器ID号
tRecorderStatus 记录统计状态
*/
u16 rpGetRecorderStatus(u16 wRecorderID, TRecorderStatus &tRecorderStatus); 

/*/*查询文件所在逻辑分区空闲大小
pszFileName 文件路径
nFreeSpace  返回的空闲大小,单位Mb.
nTotalSpace  返回的总大小,单位Mb.
*/
u16 rpGetSpaceInfo(s8 *strPath, INT32 &nFreeSpace, INT32 &nTotalSpace);

/*
设置录像器录制方法：抽帧录像或者正常录制,针对视频帧而言
参数设定：
wRecorderID 录像器ID号
bOnlySaveKeyFrame   是否进行抽帧录像，只抽取关键帧进行录制
dwSaveFrameInterval 抽帧录像的关键帧间隔，例如 0 － 表示不间隔, 1 － 表示间隔1帧关键帧
*/
u16 rpSetRecorderSaveMethod(u16 wRecorderID,
			    BOOL32 bOnlySaveKeyFrame = FALSE, 
			    u32 dwSaveFrameInterval = 0);
/*
设置录像器最小剩余的空间
在下一次设定前所有的录像器均采用此设定值
参数设定：
dwMinFreeSize   最小剩余的空间大小（单位MB）,默认值 RECORDER_MIN_SPACE==200MB
*/
u16 rpSetMinFreeSpaceSize(u32 dwMinFreeSize = RECORDER_MIN_SPACE);


//录像文件的在线发布管理

/*
打开数据库连接
参数设定：
szHostIP   数据库ip
szUserName 数据库连接所需的用户名
szPwd      数据库连接所需的密码,允许为空
*/
u16 rpOpenDB(s8 *szHostIP, s8 *szUserName, s8 *szPwd);

/*
关闭数据库连接
*/
u16 rpCloseDB();

/*
发布一个录像文件
参数设定：
szFilePath  录像文件发布的mms路径（不包括文件名及扩展名,若mms不可以匿名登录，
			则还应包括mms用户名密码 格式为mms://admin:admin/100.8.1.4/VodFtpPath）
szFileName  录像文件的文件名（包括扩展名）
bSecret	    录像文件是否发布为保密文件,TRUE-是
*/
u16 rpPublishOneFile(s8 *szFilePath, s8 *szFileName, BOOL32 bSecret);

/*
取消一个已经发布的录像文件
参数设定：
szFilePath  录像文件发布的mms路径（不包括文件名及扩展名,若mms不可以匿名登录，
	    则还应包括mms用户名密码 格式为mms://admin:admin/100.8.1.4/VodFtpPath）
szFileName  录像文件的文件名（包括扩展名）
*/
u16 rpCancelOnePublishedFile(s8 *szFilePath, s8 *szFileName);

/*
查询一个录像文件的发布状态
参数设定：
bPublished   是否处于发布状态
szFilePath  录像文件发布的ftp路径（不包括文件名及扩展名,若ftp不可以匿名登录，
			则还应包括ftp用户名密码 格式为ftp://admin:admin/100.8.1.4/VodFtpPath）
szFileName  录像文件的文件名（包括扩展名）
*/
u16 rpQueryOneFileStauts(s8 *szFilePath, s8 *szFileName, BOOL32 &bPublished);

//---------------------------------------------------------------------------------


#define MAX_TAPE_NUMBER 100

struct TapeInfo
{
	s8      m_strFileNames[MAX_PATH]; // 文件名
	u32     m_dwLength; //tape length in milliseconds
	time_t  m_ttFileTime;//最后一次修改时间
	u8      m_StreamNumber;
};

struct TapeInfoList
{
	u8 	 m_byNum;
	TapeInfo m_tTapeIfo[MAX_TAPE_NUMBER];
};

/*
录像带列表
strPath is the default directory of saved AVI files
pFileName 播放的文件名
*/

u16 rpListTapes(s8* strPath, TapeInfoList &TapeCollections); 

/*
创建放像器
wPlayerID 放像器状态
*/
u16 rpCreatePlayer(u16 wPlayerID); 

/*
销毁放像器
wPlayerID 放像器ID
*/
u16 rpDestroyPlayer(u16 wPlayerID); 

/* 
设置 视频 动态视频载荷的 Playload值
wPlayerID       放像器ID
byLocalActivePT 发送的动态载荷的Playload值, 由对呼时约定
*/
u16 rpSetVidSndActivePT( u16 wPlayerID, u8 byLocalActivePT );

/* 
设置 音频 动态视频载荷的 Playload值
wPlayerID       放像器ID
byLocalActivePT 发送的动态载荷的Playload值, 由对呼时约定
*/
u16 rpSetAudSndActivePT( u16 wPlayerID, u8 byLocalActivePT );

/* 
设置 视频加密key字串 以及 解密模式 Aes or Des
wPlayerID     放像器ID
pszKeyBuf     加密密钥缓冲指针
wKeySize      加密密钥长度
byEncryptMode 加密模式
*/
u16 rpSetVidEncryptKey( u16 wPlayerID, s8 *pszKeyBuf, u16 wKeySize, u8 byEncryptMode );

/* 
设置 音频加密key字串 以及 解密模式 Aes or Des
wPlayerID     放像器ID
pszKeyBuf     加密密钥缓冲指针
wKeySize      加密密钥长度
byEncryptMode 加密模式
*/
u16 rpSetAudEncryptKey( u16 wPlayerID, s8 *pszKeyBuf, u16 wKeySize, u8 byEncryptMode );

/*
开始放像
wPlayerID 放像器ID
pFileName 播放的文件名
dwIpAddress 发送的IP地址
wPort 发送的端口号
dwFileSeconds 从文件的指定的位置开始播放
*/
u16 rpStartPlay(u16 wPlayerID, s8 *pszFileName, u16 wLocalPort, 
		u32 dwIpAddress, u16 wPort,
		EmRecordMode emMode, u32 dwFileSeconds = 0xffffffff); 
u16 rpStartPlay(u16 wPlayerID, s8 *pszFileName, 
		u16 wLocalAudioPort, u16 wLocalVideoPort, 
		u32 dwAudioIpAddress, u16 wAudioPort, 
		u32 dwVideoIpAddress, u16 wVideoPort,
		EmRecordMode emMode, u32 dwFileSeconds = 0xffffffff);
u16 rpStartPlay(u16 wPlayerID, s8 *pszFileName,
		MP4SOURCES &tMP4Sources, u32 dwFileSeconds = 0xffffffff);
				 
/*
暂停放像
wPlayerID 放像器ID
*/
u16 rpPausePlay(u16 wPlayerID); 

/*
暂停放像
wPlayerID 放像器ID
*/
u16 rpContinuePlay(u16 wPlayerID); 

/*
停止放像
wPlayerID 放像器ID
*/
u16 rpStopPlay(u16 wPlayerID); 

/*
设置播放位置
wPlayerID 放像器ID
wOffsetSeconds 偏移量
wBasePoint 基准点 -- 开始,当前,文件尾.
if the current status is End/Pause, then it will???
*/
u16 rpSetPlayPosition(const u16 wPlayerID, const u16 wOffsetSeconds = 0, 
		      const EMPlayerStartPoint emBasePoint = BEGIN_POSITION); 

/*
快速向前播放(仅仅关键帧)
wPlayerID 放像器ID
*/

u16 rpFastForward(u16 wPlayerID); 

/*
快速向后播放(仅仅关键帧)
wPlayerID 放像器ID
*/
u16 rpFastBackward(u16 wPlayerID); 

/*
得到放像器状态
wPlayerID 放像器ID
tPlayerStatus 播放器状态
*/
u16 rpGetPlayerStatus(u16 wPlayerID, TPlayerStatus &tPlayerStatus); 

/*
删除一个asf录像文件
pszFileName 要删除的文件名
*/
u16 rpDeleteOneFile(s8 *pszFileName); 


#endif  //_RPLib_H_









