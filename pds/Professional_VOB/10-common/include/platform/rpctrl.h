/*****************************************************************************
模块名      : RPCtrl
文件名      : RPCtrl.h
相关文件    : RPCtrl.cpp
文件实现功能: 录放像库提供给上层的头文件
作者        : 王小月、方辉
版本        : V4.0  Copyright(C) 2003-2005 KDC, All rights reserved.
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2005/06/29  4.0         王小月      创建
******************************************************************************/

/****************************************************************************************************************
								一些规则

一	路径名以 "/"结尾，如 "d:/asf/", 不要给"d:/asf"
	只有在开始文件下载接收参数中，文件名为实际的文件名(如a.asf)，其他情况下应为文件前缀名(如a)

二	开始录像,开始时间必须为绝对时间(秒为单位),录小文件时不能为0，录大文件可以为0;
			 如果是小文件录象，时间间隔为[MIN_TIME_ITVL,MAX_TIME_ITVL];
			 路径和文件名不能为空;
			 录小文件时指定的目录不允许存在，录大文件时指定的目录可以存在;

三	开始放像,开始时间必须为绝对时间(秒为单位),放小文件时不能为0，放大文件时可以为0;
			 结束时间必须为绝对时间(秒为单位),必须大于开始时间,可以为0，表示播放完整个录象;
			 路径和文件名不能为空;

四  开始文件下载发送,开始时间必须为绝对时间(秒为单位),不能为0;
			 结束时间必须为绝对时间(秒为单位),必须大于开始时间,可以为0，表示播放完整个录象;
			 路径和文件名不能为空;

五	删除文件以目录为单元,目录下所有文件都删除,最后删除目录;
  
六  为了简便，TNetAddr中的m_wPort是指rtp端口，程序内部将rtcp端口设置为m_wPort + 1, 所以两个m_wPort间至少相差2

七  放像进度原为百分比，调整之后改为绝对时间（如为大文件，则为文件内相对时间），可以调用RPGetPlayerStatis
			得到播放的即时时间和总时间，上层计算百分比;

****************************************************************************************************************/

#ifndef _RPCTRL_H_
#define _RPCTRL_H_

#include "kdvtype.h"
#include "kdvdef.h"
#include "osp.h"
#include "kdvmedianet.h"

//版本号
#define  VER_PRCTRL								( const char * )"rpctrl 40.20.00.43.13021"

//错误码定义
//RPCtrl操作成功
#define RP_OK                 					(u16)0
//RPCtrl操作失败
#define RPERR_BASE								(u16)2000

//RPCtrl已经初始化
#define RPERR_RP_ALREADY_INITIAL				(u16)(RPERR_BASE + 1)
//RPCtrl未初始化
#define RPERR_RP_NOT_INITIAL					(u16)(RPERR_BASE + 2)
//内存不足
#define RPERR_NO_MEMORY							(u16)(RPERR_BASE + 3)
//指针为空
#define RPERR_NULL_POINT						(u16)(RPERR_BASE + 4)
//分区容量已满(255个)
#define RPERR_PARTION_NUM						(u16)(RPERR_BASE + 5)

#define RPERR_PARAM								(u16)(RPERR_BASE + 6)
// rpctrl 创建线程间同步信号量失败
#define RPERR_CREATE_SEM_FAILED					(u16)(RPERR_BASE + 7)

//指定录像机不存在
#define RPERR_REC_NOT_EXIST						(u16)(RPERR_BASE + 10)
//不能在该录像机状态做该操作
#define RPERR_REC_STATUS_ERROR					(u16)(RPERR_BASE + 11)
//找不到空闲的录像机
#define RPERR_NO_FREE_REC  						(u16)(RPERR_BASE + 12)
//录像机路数超过容量限制
#define RPERR_REC_OVER_CAPACITY					(u16)(RPERR_BASE + 13)
//不允许媒体类型,宽高改变
#define RPERR_MEDIATYPE_CHANGE					(u16)(RPERR_BASE + 14)
//等待关键帧
#define RPERR_WAIT_KEYFRAME						(u16)(RPERR_BASE + 15)
//时间戳错误
#define RPERR_TIMESTAMP_ERR						(u16)(RPERR_BASE + 16)
//预录时间为0或过长,超过MAX_PREREC_TIME
#define RPERR_PREREC_TIME_INVALID				(u16)(RPERR_BASE + 17)
//小文件时间间隔不符合规范,应处于[MIN_TIME_ITVL,MAX_TIME_ITVL]范围
#define RPERR_SMALL_FILE_INTERVAL_INVALID		(u16)(RPERR_BASE + 18)
//不能录象,已经处于二级告警
#define RPERR_ALREADY_ALARM_TWO					(u16)(RPERR_BASE + 19)

//指定放像机不存在
#define RPERR_PLAY_NOT_EXIST					(u16)(RPERR_BASE + 21)
//不能在该放像机状态做该操作
#define RPERR_PLAY_STATUS_ERROR					(u16)(RPERR_BASE + 22)
//找不到空闲的放像机
#define RPERR_NO_FREE_PLY  						(u16)(RPERR_BASE + 23)
//放像机路数超过容量限制
#define RPERR_PLAY_OVER_CAPACITY				(u16)(RPERR_BASE + 24)
//拖放时间必须在开始时间和结束时间之间
#define RPERR_PLAY_DROG_TIME_INVALID			(u16)(RPERR_BASE + 25)
//播放粒度不符合规范,应处于[MIN_READ_CALLBACK_GRAN,MAX_READ_CALLBACK_GRAN]范围
#define RPERR_PLAY_GRAN_INVALID					(u16)(RPERR_BASE + 26)
//录象文件长度为0,应是录象时长时间无码流导致
#define RPERR_PLAY_RECORD_LENGTH_ZERO			(u16)(RPERR_BASE + 27)
//已到结束时间
#define RPERR_PLAY_REACH_STOP_TIME				(u16)(RPERR_BASE + 28)

//创建循环缓冲出错
#define RPERR_RPBUF_PARAM						(u16)(RPERR_BASE + 31)
//循环缓冲空
#define RPERR_RPBUF_NULL						(u16)(RPERR_BASE + 32)
//循环缓冲混乱
#define RPERR_RPBUF_INVALID						(u16)(RPERR_BASE + 33)
//循环缓冲满
#define RPERR_RPBUF_FULL						(u16)(RPERR_BASE + 34)
//循环缓冲数据错误
#define RPERR_RPBUF_DATA_ERR					(u16)(RPERR_BASE + 35)

//媒体类型不支持
#define RPERR_MEDIATYPE_UNSUPPORT				(u16)(RPERR_BASE + 41)
//流序号不正确
#define RPERR_STREAM_IDX_INVALID				(u16)(RPERR_BASE + 42)
//分区名过短(为0)或过长(超过PARTION_NAME_LEN)
#define RPERR_PARTION_NAME_INVALID				(u16)(RPERR_BASE + 43)
//告警临界值错误,为0或一级告警值小于二级告警值
#define RPERR_ALARM_VALUE_INVALID				(u16)(RPERR_BASE + 44)
//开始时间必须是绝对时间(至少大于0),如果结束时间不为0还必须小于结束时间
#define RPERR_START_TIME_INVALID				(u16)(RPERR_BASE + 45)

//目录已存在
#define RPERR_PATH_ALREADY_EXIST				(u16)(RPERR_BASE + 51)
//目录不存在
#define RPERR_PATH_NOT_EXIST					(u16)(RPERR_BASE + 52)
//目录创建失败
#define RPERR_PATH_CREATE_FAIL					(u16)(RPERR_BASE + 53)
//目录删除失败
#define RPERR_PATH_DELETE_FAIL					(u16)(RPERR_BASE + 54)
//文件夹名过短(为0)或过长(超过MAX_FILE_PATH_LEN)或不是以'/'结尾
#define RPERR_FILE_PATH_INVALID					(u16)(RPERR_BASE + 55)

//文件已存在
#define RPERR_FILE_ALREADY_EXIST				(u16)(RPERR_BASE + 61)
//文件不存在
#define RPERR_FILE_NOT_EXIST					(u16)(RPERR_BASE + 62)
//文件删除失败
#define RPERR_FILE_DELETE_FAIL					(u16)(RPERR_BASE + 63)
//文件改名失败
#define RPERR_FILE_RENAME_FAIL					(u16)(RPERR_BASE + 64)
//文件名过短(为0)或过长(超过MAX_FILE_NAME_LEN)
#define RPERR_FILE_NAME_INVALID					(u16)(RPERR_BASE + 65)

//reclog.txt文件创建失败
#define RPERR_RECLOG_FILE_CREATE_FAIL			(u16)(RPERR_BASE + 71)
//reclog.txt文件不存在
#define RPERR_RECLOG_FILE_NOT_EXIST				(u16)(RPERR_BASE + 72)
//reclog.txt文件操作失败
#define RPERR_RECLOG_FILE_OP_FAIL				(u16)(RPERR_BASE + 73)
//预录文件创建失败
#define RPERR_PREREC_FILE_CREATE_FAIL			(u16)(RPERR_BASE + 74)

//找不到空闲的下载接收通道
#define RPERR_NO_FREE_FTPRCV  					(u16)(RPERR_BASE + 81)
//指定下载接收通道不存在
#define RPERR_FTPRCV_NOT_EXIST  				(u16)(RPERR_BASE + 82)
//不能在该下载接收状态做该操作
#define RPERR_FTPRCV_STATUS_ERROR  				(u16)(RPERR_BASE + 83)
//下载接收无Socket在监听
#define RPERR_FTPRCV_LISTEN_SOCKET_ERROR		(u16)(RPERR_BASE + 84)
//监听超时
#define RPERR_FTPRCV_LISTEN_TIMEOUT				(u16)(RPERR_BASE + 85)
//监听地址和端口不能为0
#define RPERR_IP_OR_PORT_MUST_NO_ZERO			(u16)(RPERR_BASE + 86)
//监听错误
#define RPERR_FTPRCV_LISTEN_ERROR  				(u16)(RPERR_BASE + 87)
//接收客户端连接错误
#define RPERR_FTPRCV_ACCEPT_ERROR  				(u16)(RPERR_BASE + 88)
//客户端连接不匹配
#define RPERR_FTPRCV_CLIENT_ERROR  				(u16)(RPERR_BASE + 89)

//找不到空闲的下载发送通道
#define RPERR_NO_FREE_FTPSND  					(u16)(RPERR_BASE + 91)
//指定下载发送通道不存在
#define RPERR_FTPSND_NOT_EXIST  				(u16)(RPERR_BASE + 92)
//不能在该下载发送状态做该操作
#define RPERR_FTPSND_STATUS_ERROR  				(u16)(RPERR_BASE + 93)
//申请下载发送Socket失败
#define RPERR_FTPSND_SOCKET_ERROR  				(u16)(RPERR_BASE + 94)
//连接下载接收方失败
#define RPERR_FTPSND_CONNECT_ERROR  			(u16)(RPERR_BASE + 95)


//add zhx ftp udp下载有关
#define		RP_ERR_FTP_PARAM					(u16)(RPERR_BASE+101)		//参数错误
#define		RP_ERR_FTP_SOCKET					(u16)(RPERR_BASE+102)		//socket错误
#define		RP_ERR_FTP_TASK_CREATE				(u16)(RPERR_BASE+103)		//生成task错误
#define		RP_ERR_SND_SERVICE_NOT_START		(u16)(RPERR_BASE+104)		//没有创建snd task
#define		RP_ERR_FTP_NEW_OBJECT				(u16)(RPERR_BASE+105)		//创建对象错误
#define		RP_ERR_FTP_MAX_CAPACITY				(u16)(RPERR_BASE+106)		//超过容量
#define		RP_ERR_FTP_DATA_SCARE				(u16)(RPERR_BASE+107)		//数据缺乏
#define		RP_ERR_FTP_PACK_NO_INDEX			(u16)(RPERR_BASE+108)		//包乱序
#define		RP_ERR_OBJECT_NOT_EXIST				(u16)(RPERR_BASE+109)		//
#define		RP_ERR_FTP_STATUS_ERR				(u16)(RPERR_BASE+110)		//状态错误
#define		RP_ERR_FTP_TOOMUCH_PROXY_DATA		(u16)(RPERR_BASE+111)		//状态错误
//end add


//add zhx 06-04-18 文件预录有关
#define RP_ERR_FILE_BUF_FILENULL				(u16)(RPERR_BASE + 120)//参数错误
#define RP_ERR_FILE_BUF_PARAM					(u16)(RPERR_BASE + 121)//参数错误
#define RP_ERR_FILE_BUF_FOPEN					(u16)(RPERR_BASE + 122)//fopen错误
#define RP_ERR_FILE_BUF_NODATA					(u16)(RPERR_BASE + 123)//没有数据错误
#define RP_ERR_FILE_BUF_FREAD					(u16)(RPERR_BASE + 124)//fread错误
#define RP_ERR_FILE_BUF_FWRITE					(u16)(RPERR_BASE + 125)//fwrite错误
#define RP_ERR_FILE_BUF_FSEEK					(u16)(RPERR_BASE + 126)//fseek错误
#define RP_ERR_FILE_BUF_DATAERR					(u16)(RPERR_BASE + 127)//数据混乱错误
#define RP_ERR_FILE_BUF_NEW						(u16)(RPERR_BASE + 131)//分配缓冲错误
//end add








//其它定义
//可检测的最大分区数
#define		MAX_PARTION_NUM		255
//磁盘空间恢复正常(告警恢复)
#define	RP_DISK_RESUME_NORMAL					(u8)0
//一级磁盘空间不足告警，不停止录像
#define	RP_DISK_ALARM_LEVEL_ONE					(u8)1
//二级磁盘空间不足告警，停止所有录像
#define	RP_DISK_ALARM_LEVEL_TWO					(u8)2
//录像过程中写文件出错,录像停止
#define RP_WRITE_FILE_ERROR						(u8)3

//单个录像机录像(放像机放像)时，网络接收（发送）的最大路数，即流的个数
#define MAX_STREAM_NUM							(u8)5
//单个录像机最大视频流个数
#define MAX_VIDEOSTREAM_NUM						(u8)3
//单个录像机最大音频流个数
#define MAX_AUDIOSTREAM_NUM						(u8)2

//无效的录像机编号，开始录像失败时返回该编号
#define NULL_REC_NO								(u32)~0
//无效的放像机编号，开始放像失败时返回该编号
#define NULL_PLY_NO								(u32)~0
//无效的下载接收编号，开始录像下载接收时返回该编号
#define NULL_FTPRCV_NO							(u8)255
//无效的下载发送编号，开始录象下载发送时返回该编号
#define NULL_FTPSND_NO							(u8)255
//8秒接收不到下载连接，超时
#define MAX_FTPRCV_LISTEN_TIMEOUT				(u32)8

//录像机最大容量
#define MAX_RECORDER_CAPACITY					(u32)1024
//放像机最大容量
#define MAX_PLAYER_CAPACITY						(u32)1024

//最多允许4路文件传输同时工作
#define MAX_FTP_CHNS							(u8)4
//分驱名最大长度
#define  MAX_PARTION_NAME_LEN					(u8)32
//文件路径最大长度
#define	MAX_FILE_PATH_LEN						(u8)255
//文件名最大长度
#define	MAX_FILE_NAME_LEN						(u8)255

//最大预录时间（秒）
#define MAX_PREREC_TIME							(u32)30
//最大预录缓冲大小
#define MAX_PREREC_SIZE							(u32)((MAX_PREREC_TIME * 25 * 8) << 10)
//最大预录帧数
#define MAX_PREREC_FRMN_NUM						(u32)10000

//最大快放倍数
#define MAX_QUICKPLAY_TIMES						(u8)4
//最大慢放倍数
#define MAX_SLOWPLAY_TIMES						(u8)4

//文件列表最多存放的文件数
#define MAX_FILE_NUMBER							(u32)1000

//最小的读文件回调粒度(秒)
#define MIN_READ_CALLBACK_GRAN					(u8)1
//默认的读文件回调粒度(秒)
#define DEFAULT_READCB_GRAN						(u8)5
//最大的读文件回调粒度(秒)
#define MAX_READ_CALLBACK_GRAN					(u8)60

//最小的小文件时间间隔(秒)
#define MIN_TIME_ITVL							(u32)10
//默认的小文件时间间隔(秒)
#define DEFAULT_TIME_ITVL						(u32)60
//最大的小文件时间间隔(秒)
#define MAX_TIME_ITVL							(u32)600

//环行缓冲个数
#define MAX_BUF_NUM								(s32)16
#define BUF_STARTREAD_NUM						(s32)1
#define BUF_FASTREAD_NUM						(s32)8

#define MAX_PLAY_TIME							(u32)-1

//音频媒体数目
#define MAX_AUDIO_TYPE_NUM	(u8)16

//某种媒体类型的帧数据大小
struct TFrameSizeInfo
{
	u8 m_byMediaType;
	u32 m_dwFrameSize;
};

//  media callback 
enum
{
	RP_STREAM_PROPERTY = 0 ,
	RP_STREAM_DATA 	
} ;

typedef struct tagRpMediaStream
{
	u32		m_dwFrameSize;
	u8*		m_pbFrame;
	u32		m_dwTimeStamp;
	u32		m_dwDuration;
	BOOL32	m_bKeyFrame ;
	u8		m_byStreamId;
} TRpMediaStream ;
	

#if __Win32__ || WIN32
#pragma pack(push, 1)				//	用一个字节的结构对齐，防止不同的编译器导致错误
#endif	//WIN32

// ASF Audio Media Type
typedef struct tagRpAudioMediaInfo{
	u16		wCodecID_FormatTag;				// 音频Codec标识
	u16		wNumberOfChannels;				// 音频通道数				
	u32		dwSamplesPerSecond;				// 采样率
	u32		dwAverageNumberOfBytesPerSecond;// 平均码率 Bytes / Second
	u16		wBlockAlignment;				// 块对齐字节数 
	u16		wBitsPerSample;					// 每个采样的比特数常为8或者16
	u16		wCodecSpecificDataSize;			// 音频Codec标识
	u8*		pbyCodecSpecificData;			// 音频Codec标识
} TRpAudioMediaInfo
#if defined(_VXWORKS_) || defined(_LINUX_)
__attribute__ ( (packed) ) 
#endif
;							// 相当于 WAVEFORMATEX 结构

// ASF Video Media Type
typedef struct tagRpFormatDataStruct{
	u32		dwFormatDataSize;				// 视频属性数据大小
	s32		dwImageWidth;					// 视频画面宽度
	s32		dwImageHeight;					// 视频画面高度
	u16		wReserved;						// 保留数据
	u16		wBitsPerPixelCount;				// 每象素的比特数
	u32		dwCompressionID;				// 视频压缩Codec标识
	u32		dwImageSize;					// 画面尺寸
	s32		nHorizontalPixelsPerMeter;		// 水平分辨率
	s32		nVerticalPixelsPerMeter;		// 垂直方向分辨率
	u32		dwColorsUsedCount;				// 调色板
	u32		dwImportantColorsCount;			// 
	u8*		pbyCodecSpecificData;			// Codec 扩展信息
} TRpFormatData
#if defined(_VXWORKS_) || defined(_LINUX_)
__attribute__ ( (packed) ) 
#endif
;								// 相当于 BITMAPINFOHEADER 结构

typedef struct tagRpVideoMediaInfo{
	u32		dwEncodedImageWidth;			// 视频画面宽度
	u32		dwEncodedImageHeight;			// 视频画面宽
	u16		wFormatDataSize;				// 视频属性数据大小
	TRpFormatData 	tFormatData;			
} TRpVideoMediaInfo
#if defined(_VXWORKS_) || defined(_LINUX_)
__attribute__ ( (packed) ) 
#endif
;

typedef struct tarRpOtherMediaInfo{
	u32		dwInfoLength;					// 其它媒体数据长度
	u8		*pbyInfoData;					// 其它媒体数据指针
} TRpOtherMediaInfo
#if defined(_VXWORKS_) || defined(_LINUX_)
__attribute__ ( (packed) ) 
#endif
;


//  流属性结构 
typedef struct tagRpStreamProperty{
	u16		wStreamType;					// 流类型
	u8		byStreamNumber;					// 流序号
	u16		wErrorCorrectType;				// 纠错数据类型	
	u32		dwErrorCorrectDataLength;		// 纠错数据类型	
	u8		*pbyErrorCorrectData;			// 纠错数据类型	
	BOOL32	bIsEncryptedContent;			// 是否对内容采用了熵编码
	u32		dwAverageBitrate;				// 平均比特率  bits/s
	
	union{
		TRpAudioMediaInfo	 tAudioMediaInfo;   // 音频数据属性
		TRpVideoMediaInfo	 tVideoMediaInfo;	// 视频数据属性
		TRpOtherMediaInfo	 tOtherMediaInfo;	// 其它媒体数据属性
	};
} TRpStreamProperty
#if defined(_VXWORKS_) || defined(_LINUX_)
__attribute__ ( (packed) ) 
#endif
;

#if __Win32__ || WIN32
#pragma pack(pop)
#endif	//WIN32

typedef void (*MediaStreamCallBack)(u32 dwRecorderId, u8 DataType, u8 * pBuf, u32 dwLength, u32 dwContext);


//剩余存储空间告警回调函数,只有当二级告警,停止录像时,录像机号才有意义,其他一般设置为255
typedef void (*FreeSpaceAlarmCallBack)( u8 byAlarmLevel, s8* pchPartionName, u32 dwRecorderId, u32 dwContext );

//文件下载进度回调函数
//dwCurrentFtpTime,以秒为单位(绝对时间)
typedef void (*FtpFileRateCallBack)( u8 byFtpSndID, u32 dwCurrentFtpTime, u32 dwContext );

//录制小文件结束回调(不要与大文件切换回调混淆,不然达不到所要求的功能)
typedef void (*RecFileEndCallBack)( u32 dwRecorderID, u32 dwStoptime, u32 dwContext );

//大文件切换回调,s8* pchFileName:切换后文件名指针(不要与小文件结束回调混淆,不然达不到所要求的功能)
typedef void (*RecBigFileSwCallBack)( u32 dwRecorderID, s8* pchFileName, u32 dwContext );

//读文件进度回调函数
//dwCurrentPlayTime,以秒为单位(绝对时间),不管大小文件，统一以MAX_PLAY_TIME作为播放结束标志
typedef void (*ReadFileRateCallBack)( u32 dwPlayerID, u32 dwCurrentPlayTime, u32 dwContext );

//网络地址结构
typedef  struct  tagTRPNetAddr 
{
	//地址
	u32  m_dwIp;
	//端口号
	u16  m_wPort;
}TNetAddr;

//网络回传地址结构
typedef  struct  tagTNetBackAddr
{
	//地址
	u32  m_dwIp;
	//rtcp端口号
	u16  m_wRtcpPort;
}TNetBackAddr;

//网络接收地址信息结构
typedef  struct  tagTRPNetRcvParam
{
	//实际的地址数
	u8        m_byNum;
	//地址数组，0：第一路音频；1：第一路视频；2：第二路视频（双流）；3：第二路音频；4：第三路视频，目前最大支持5路
	TNetAddr  m_atAddr[MAX_STREAM_NUM];
	//回传地址数组
	TNetBackAddr m_atBackAddr[MAX_STREAM_NUM];
}TRPNetRcvParam;

//网络发送地址信息结构
typedef  struct  tagTRPNetSndParam
{
	//实际的地址数
	u8        m_byNum;
	//本地地址数组，0：第一路音频；1：第一路视频；2：第二路视频（双流）；3：第二路音频；4：第三路视频，目前最大支持5路
	TNetAddr  m_atLocalAddr[MAX_STREAM_NUM ];
	//远端地址数组
	TNetAddr  m_atRemoteAddr[MAX_STREAM_NUM ];
	//如果使用raw socket,则本字段为src ip/port
	TNetSession m_atAddrForRawSock[MAX_STREAM_NUM];
}TRPNetSndParam;

//文件列表
typedef struct tagTListInfo
{
	u32 m_dwNum;
	s8	m_astrFileName[MAX_FILE_NUMBER][MAX_FILE_NAME_LEN];
}TListInfo;

//载荷信息
typedef struct tagPTInfo
{
	u8 m_byRmtActivePT;
	u8 m_byRealPT;
}TPTInfo;

//载荷信息结构
typedef struct tagRemotePTParam
{
	//载荷值数组
	TPTInfo   m_atPTInfo[MAX_STREAM_NUM];
}TRemotePTParam;

//本地载荷信息结构
typedef struct tagPTParam
{
	//本地载荷数组
	u8		  m_abyLocalPt[MAX_STREAM_NUM];
}TLocalPTParam;

//加密（解密）信息
typedef struct tagEncDecInfo
{
	//加密（解密）key字串
	s8 *m_pszKeyBuf;
	//加密（解密）key字串大小
	u16 m_wKeySize;
	//加密（解密）模式
	u8 m_byMode;
}TEncDecInfo;

//加密（解密）信息结构
typedef struct tagTEncDecParam
{
	//实际的地址数
	u8	m_byNum;
	//加密（解密）信息数组
	TEncDecInfo m_atEncDecInfo[MAX_STREAM_NUM];
}TEncDecParam;

//媒体统计
typedef struct tagTMediaStatis
{
	//媒体类型
	u8  m_byMediaType;
	//是否为音频
	BOOL32 m_bAud;
	//接收（发送）总帧数
	u32 m_dwNetFrmNum;
	//接收（发送）帧率
	u32 m_dwNetFrmRate;
	//缓冲满导致的丢包数
	u32 m_dwBufFullLostFrmNum;
	//错帧导致的丢包数
	u32 m_dwErrFrmLostFrmNum;
	//缓冲空的次数
	u32 m_dwBufEmptyNum;
	//最后一帧的序号
	u32 m_dwLastFrameId;
	//是否在等待关键帧
	BOOL32 m_bWaitForKeyFrame;
	//持续时间
	u32 m_dwDuration;
	//时间戳
	u32 m_dwTimeStamp;
	//2秒内的asf文件的读（写）帧数
	u32 m_dwAsfFrmNum;
	//2秒内的asf文件的读（写）帧率
	u32 m_dwAsfFrmRate;
	//视频宽度(音频时,把这个字段用作保存模式)
	u32 m_dwWidth;
	//视频高度
	u32 m_dwHeight;
	//是否263+
	BOOL32 m_bH263Plus;
	//平均波特率06-05-18
	u32 m_dwAverageBitrate;
	//是否播放，针对上层不给网络地址，则认为那路流是不播放的
	BOOL32 m_bPlayOut;
	BOOL32 m_bHighProfile;//是否是hp码流

	u8 m_byAudioTrackNum;
	
	tagTMediaStatis()
	{
		memset(this, 0, sizeof(tagTMediaStatis));
	}
}TMediaStatis;

//录放像库初始化参数结构
typedef struct tagRPInitParam
{
	//需要的录像机最大路数（不大于最大容量）
	u32 m_dwInitialRecorderNum;
	//需要的放像机最大路数（不大于最大容量）
	u32 m_dwInitialPlayerNum;
	//一级告警的最小剩余存储空间（不停止录像）
	u32 m_dwRemainSpaceAlarmLevelOne;
	//二级告警的最小剩余存储空间（停止所有录像）
	u32	m_dwRemainSpaceAlarmLevelTwo;
	//剩余存储空间告警
	FreeSpaceAlarmCallBack	m_pCallback;
	//告警上下文
	u32 m_dwContext;	
}TRPInitParam;

//下载接收状态
enum EMFtpRcvStatus{
	//停止下载接收
	FTP_RCV_STOP,
	//正在下载接收
	FTP_RCV_RUNNING
};

//录象下载接收参数
/*
录象下载，接收方等待发送方连接请求
并需判断连接上来的请求是否是发送方的
录象下载只录大文件
*/
typedef struct tagFtpRcvParam
{
	//下载发送方地址
	TNetAddr m_tFtpSndAddr;
	//下载接收方地址
	TNetAddr m_tFtpListenAddr;
	//保存录像文件的路径
	s8 m_achRecFilePath[MAX_FILE_PATH_LEN+1];
	//保存录像文件的文件名
    s8 m_achRecFileName[MAX_FILE_NAME_LEN+1];
}TFtpRcvParam;

//下载发送状态
enum EMFtpSndStatus{
	//停止下载发送
	FTP_SND_STOP,
	//正在下载发送
	FTP_SND_RUNNING
};

typedef struct tagFtpSndParam
{
	//下载发送方地址，需连接接收地址
	TNetAddr m_tFtpSndAddr;
	//下载接收方地址
	TNetAddr m_tFtpRcvAddr;
	//开始时间
	u32 m_dwStartTime;
	//结束时间	
	u32 m_dwStopTime;
	//待放像文件的路径
    s8 m_achPlayFilePath[MAX_FILE_PATH_LEN+1];
	//待放像文件的文件名
    s8 m_achPlayFileName[MAX_FILE_NAME_LEN+1];
	//读文件进度回调
	FtpFileRateCallBack m_pCallBack;
	//读文件进度粒度(以秒为单位,应处于[MIN_READ_CALLBACK_GRAN,MAX_READ_CALLBACK_GRAN]范围)
	u8 m_byRateGran;
	//回调上下文
	u32 m_dwContext;
}TFtpSndParam;

//录像像状态
enum EMRecorderStatus{
	//停止录像
	REC_STOP,
	//正在录像
	REC_RUNNING,
	//暂停录像
	REC_PAUSE
};

//录像参数结构
/*	录放像库以 m_dwStartTime 开始命名文件， 设m_dwStartTime 为 123000，如果 m_achRecFileName 为 NULL，
	则文件名命名为 123000.asf。 如果m_achRecFileName 不为 NULL（如"a"），则文件名命名为 a123000.asf。
*/
typedef struct tagTRecParam
{
	//1：本地录像；本地录像要在codecwrapper中调用
	//0：网络录像
	BOOL32 m_bLocalRec;
	//保存录像文件的路径
    s8 m_achRecFilePath[MAX_FILE_PATH_LEN+1];
	//保存录像文件的文件名
    s8 m_achRecFileName[MAX_FILE_NAME_LEN+1];
	//第一个文件的起始时间
	u32 m_dwStartTime;
	//第一个文件以视频关键帧开始存
	BOOL32 m_bVideoFirst;
	//1：保存为小文件；
	//0：保存为大文件
	BOOL32 m_bSmallFile;
	//保存为小文件时的时间间隔（单位秒）
	u32 m_dwTimeInterval;
	union
	{
		//录制小文件结束回调
		RecFileEndCallBack m_pRecEndCallBack;
		//大文件切换回调
		RecBigFileSwCallBack m_pRecBigFileSwCB;
	};
	
	//回调上下文
	u32 m_dwContext;
}TRecParam;

//设置录像的视频流码率
typedef struct tagStreamAverageBitrate
{
	//流对应的平均码率，其中0为音频，1，为视频，2为视频
	u32 m_adwStreamAverageBitrate[MAX_STREAM_NUM];
}TStreamAverageBitrate;


//录像机运行状态
typedef struct tagTRPRecStatus
{
	//预录是否开始
	BOOL32 m_bPreRecStart;
	//播放状态
	EMRecorderStatus m_tRecState;
}TRPRecStatus;

//录像机状态
typedef struct tagTRecorderStatus
{
	//录像状态
	TRPRecStatus  m_tRecStatus;
	//录像参数
	TRecParam  m_tRecParam;
	//网络接收参数
	TRPNetRcvParam	m_tNetparam;
	//是否只录关键帧
	BOOL32 m_bOnlyRecIFrm;
	//录关键帧的间隔
	u32	m_dwFrmItvl;
}TRecorderStatus;

//录像机统计
typedef struct tagTRecorderStatis
{
	//流的个数
	u8	m_byNum;
	//录像时间(秒)
	u32 m_dwRecTime;
	//录像媒体统计数组
	TMediaStatis m_atMediaRecStatis[MAX_STREAM_NUM];
}TRecorderStatis;

//放像状态
enum EmPlayerStatus
{
	//停止放像
	PLAYER_PLAY_STOP,
	//正在放像
	PLAYER_PLAY_START,
	//暂停放像
	PLAYER_PLAY_PAUSE,
	//快放
	PLAYER_PLAY_QUICK,
	//慢放
	PLAYER_PLAY_SLOW
};

//放像参数结构
/*	开始给绝对时间,不能为0;
	结束时间给绝对秒数,可以为0，表示播放到结束;
*/ 
typedef struct tagTPlayParam	
{
	//待放像文件的路径
    s8 m_achPlayFilePath[MAX_FILE_PATH_LEN+1];
	//待放像文件的文件名
    s8 m_achPlayFileName[MAX_FILE_NAME_LEN+1];
	//开始时间(绝对时间，不能为0)
	u32 m_dwStartTime;
	//结束时间(可以为0，不为0时给绝对时间)
	u32 m_dwStopTime;
	//1：录像文件为小文件；0：录像文件为大文件
	BOOL32 m_bSmallFile;
	//读文件进度回调
	ReadFileRateCallBack m_pCallBack;
	//读文件进度粒度(以秒为单位,应处于[MIN_READ_CALLBACK_GRAN,MAX_READ_CALLBACK_GRAN]范围)
	u8 m_byRateGran;
	//回调上下文
	u32 m_dwContext;
	//网络发送地址
	TRPNetSndParam	m_tNetSndParam;
	//是否只放关键桢
	BOOL32 m_bPlyOnlyIFrm;
}TPlayParam;

//放像机状态
typedef struct tagTPlayerStatus
{
	//播放状态
	EmPlayerStatus m_tPlyStatus;
	//播放参数
	TPlayParam m_tPlayParam;
}TPlayerStatus;

//放像机统计
typedef struct tagTPlayerStatis
{
	//流的个数
	u8	m_byNum;
	//放像时间(秒)
	u32 m_dwPlyTime;
	//文件总共时间(秒)
	u32 m_dwTotalPlyTime;
	//放像媒体统计数组
	TMediaStatis m_atMediaPlyStatis[MAX_STREAM_NUM];
	//流序号数组，因为一个文件中有2个流，序号可能为2，3, 不是1，2,
	u8  m_abyStreamNoArr[MAX_STREAM_NUM];
}TPlayerStatis;

//文件信息
typedef struct tagTFileInfo
{
	//文件大小
	u32 m_dwSize;
	//持续时间
	u32 m_dwDuration;
}TFileInfo;

#define MAX_KEY_FRAME_HDR_LEN	(2048)  

typedef struct tagTKeyFrameHdr{
	u8	m_abyHdr[MAX_STREAM_NUM][MAX_KEY_FRAME_HDR_LEN];
	u32 m_adwHdrLen[MAX_STREAM_NUM];
}TKeyFrameHdr;

//文件中的流信息
typedef struct tagTMediaInfo
{
	//流的个数
	u8 m_byNum;
	//流的类型
	u8 m_abyMediaType[MAX_STREAM_NUM];
	u16	m_awWidth[MAX_STREAM_NUM];
	u16 m_awHeight[MAX_STREAM_NUM];
}TMediaInfo;

typedef struct tagTCBContext 
{
	u8 m_byIdx;
	u32 m_dwAddr;
}TCBContext; 

typedef struct tagTRPRecRSParam
{
	TRSParam m_atRsParam[MAX_STREAM_NUM];
	BOOL32	 m_abRepeatSnd[MAX_STREAM_NUM];
}TRPRecRSParam;

typedef struct tagTRPPlyRSParam
{
	u16		 m_awBufTimeSpan[MAX_STREAM_NUM];
	BOOL32	 m_abRepeatSnd[MAX_STREAM_NUM];
}TRPPlyRSParam;


//add zhx ftp udp下载有关
//下载方同时下载数(现在最好设定为1)
#define			MAX_RP_FTP_RCV_NUM			32
//ftp发送服务最大的服务数
#define			MAX_RP_FTP_SND_NUM			32


#define			MAX_FTP_RCV_USER_ID_LEN		32

typedef enum
{
		SND_STATUS_START = 1,					//开始1
		SND_STATUS_STATUS_PROBE,				//探测2
		SND_STATUS_WAITING_RCV_REQ,				//等待ack3
		SND_STATUS_WAITED_RCV_REQ,				//收到ack4
		SND_STATUS_SEND_STREAM_PROPERTY,		//流属性5
		SND_STATUS_WAIT_PROPERTY_ACK,			//等待流属性ack6
		SND_STATUS_SEND_FRAME_DATA,				//帧数据7
		SND_STATUS_WAIT_FRAME_DATA_ACK,			//等待帧数据ack8
		SND_STATUS_STOP
}TSndStatus;

//下载状态,重点对有(!!!及0)的进行处理
typedef enum
{
		RCV_STATUS_START = 1,					//1开始下载
		RCV_STATUS_ADD_STREAM_ERROR,			//2写流错误
		RCV_STASTU_ADD_FRAME_ERROR,				//3写帧错误
		RCV_STATUS_SND_REQ,						//4发送下载请求
		RCV_STATUS_WAIT_STREAM_PROPERTY,		//5等待流属性
		RCV_STATUS_SND_PROPERTY_ACK,			//6发送流属性回馈
		RCV_STATUS_WAIT_FRAME_DATA,				//7等待帧数据
		RCV_STATUS_SND_FRAME_DATA_ACK,			//8发送帧回馈
		RCV_STATUS_TIMEOUT,						//!!!9超时
		RCV_STATUS_CLOSE,						//10关闭
		Rcv_STATUS_RCV_END,						//!!!11下载结束
		RCV_STATUS_REQ_INVALID,					//!!!12检查下载参数(发送方认为下载请求参数可能不合法)
		RCV_STATUS_PROBE						//
}TRcvStatus;


typedef void(*RpFtpRcvCallBack)(u32 dwStatusCode, u32 dwSSid, u32 dwCurTime, u32 dwUserdata);

//ftp发送服务端参数结构
typedef struct tagFtpSndServParam
{
	TNetAddr	m_tLocalAddr;			//本地地址
}TFtpSndServParam;

//ftp发送
typedef struct tagFtpUdpSndParam
{
	u32 m_dwStartTime;										//开始时间
	u32 m_dwStopTime;										//结束时间
	s8 m_abyFilePath[MAX_FILE_PATH_LEN];					//文件路径
	s8 m_abyFileName[MAX_FILE_NAME_LEN];					//文件名
	BOOL32 m_bSmallFile;									//是否小文件
}TFtpUdpSndParam;

typedef struct tagFtpSndAllParam
{
	u32				m_dwSSid;			//客户端的ssid
	TSndStatus		m_emSndStatus;		//ftp发送端状态
	TNetAddr	m_tRemoteAddr;			//客户端ip/port
	s8	m_abyUserId[MAX_FTP_RCV_USER_ID_LEN+1];//用户id
}TFtpSndAllParam;


#define		MAX_PROXY_USER_DATA		16

//ftp客户端参数结构
typedef struct tagFtpUdpRcvParam
{
	s8	m_abyUserId[MAX_FTP_RCV_USER_ID_LEN+1];//用户id
	TNetAddr m_tRcvLocalAddr;			//本地地址
	TNetAddr m_tRcvRemoteAddr;			//下载服务地址
	RpFtpRcvCallBack m_pRcvCallBack;	//回调
	u32 m_dwUserData;					//上下文
	s8 m_abyLocalFilePath[MAX_FILE_PATH_LEN];			//本地存储路径
	s8 m_abyLocalFileName[MAX_FILE_NAME_LEN];			//本地存储文件名
	u32 m_dwGran;						//回调粒度
	u32 m_dwStartTime;					//下载开始时间
	u32 m_dwStopTime;					//下载结束时间
	s8 m_abyRequestFilePath[MAX_FILE_PATH_LEN];			//要下载的文件路径
	s8 m_abyRequestFileName[MAX_FILE_NAME_LEN];			//要下载的文件名
	BOOL32 m_bSmallFile;				//是否小文件
	u8	m_abyProxyUserData[MAX_PROXY_USER_DATA];//代理用户数据
	u32 m_dwProxyUserDataLen;//代理用户数据长度
	tagFtpUdpRcvParam()
	{
		m_dwProxyUserDataLen = 0;
	}
}TFtpUdpRcvParam;

//ftp发送服务端查询结构
typedef struct tagFtpSndQuery
{
	u32 dwClientNum;					//客户端数,也即下列数组元素个数
	TFtpSndAllParam m_atFtpSndAllParam[MAX_RP_FTP_SND_NUM];//客户端请求下载的相应参数
	TFtpUdpSndParam m_tFtpUdpSndParam[MAX_RP_FTP_SND_NUM];//客户端请求下载的相应参数
}TFtpSndQuery;

//ftp客户端查询结构
typedef struct tagFtpRcvQuery
{
	u32 dwStauts;						//目前状态
}TFtpRcvQuery;
//end add

/*====================================================================
函数名      :RPSetMaxAsfFileLen
功能        :设置录像机的最大录像文件大小（超过则会切文件）
算法实现    :
引用全局变量:
输入参数说明:u32 dwMaxFileLen 最大文件大小
返回值说明  :
----------------------------------------------------------------------
修改记录    :
日  期      版本        修改人        修改内容
20100926	4.6			pengjie		   create
====================================================================*/
void RPSetMaxAsfFileLen( u32 dwMaxFileLen );

/*====================================================================
函数名      :RPInit
功能        :录放像库初始化
算法实现    :
引用全局变量:
输入参数说明:const TRPInitParam *ptRPInitParam,录放像库初始化参数结构
返回值说明  :操作成功返回RP_OK,失败返回错误码
----------------------------------------------------------------------
修改记录    :
日  期      版本        修改人        修改内容
05/06/29	4.0			gie			  创建
====================================================================*/
u16 RPInit( const TRPInitParam *ptRPInitParam, BOOL32 bUsedRawSocket = FALSE);

/*====================================================================
函数名      :RPRelease
功能        :释放录放像库
算法实现    :
引用全局变量:
输入参数说明:无
返回值说明  :操作成功返回RP_OK,失败返回错误码
----------------------------------------------------------------------
修改记录    :
日  期      版本        修改人        修改内容
05/06/29	4.0			gie			  创建
====================================================================*/
u16 RPRelease( );

/*====================================================================
函数名      :  RPSetCheckPartion
功能        :  设置磁盘告警监控的分区
算法实现    :可多次使用,如果是相同的分区,则覆盖前一次的告警值.最多分区数为MAX_PARTION_NUM
引用全局变量:
输入参数说明:  s8 *pchPartionName， 分区名
			   u32  dwAlarmLvlOne, 一级告警值
			   u32  dwAlarmLvlTwo, 二级告警值
返回值说明  :操作成功返回RP_OK,失败返回错误码
----------------------------------------------------------------------
修改记录    :
日  期      版本        修改人        修改内容
06/01/24	4.0			fang hui        创建
====================================================================*/
u16 RPSetCheckPartion( s8 *pchPartionName , u32 dwAlarmLvlOne, u32 dwAlarmLvlTwo );

//设置磁盘检测间隔，单位：s ,内部默认为10s
u16 RPSetDiskCheckInterval( u32 dwInterval );

/*====================================================================
删除分区检测
s8* pchPartionName: 分区名,要与RPSetCheckPartion函数传递的分区名完全匹配
正确返回rp_Ok
====================================================================*/
u16  RPDeleteCheckPartion( s8 *pchPartionName );

/*====================================================================
函数名      :RPCreateRec
功能        :创建录像机，得到可用的录像机编号
算法实现    :
引用全局变量:
输入参数说明:u8 *pbyRecoderID, 指向录像机编号，返回给用户			 
返回值说明  :操作成功返回RP_OK,失败返回错误码
----------------------------------------------------------------------
修改记录    :
日  期      版本        修改人        修改内容
05/06/29	4.0			gie			  创建
====================================================================*/		
u16 RPCreateRec( u32 *pdwRecoderID );

/*====================================================================
函数名      :RPReleaseRec
功能        :释放录像机
算法实现    :
引用全局变量:
输入参数说明:u8 byRecoderID, 录像机编号	 
返回值说明  :操作成功返回RP_OK,失败返回错误码
----------------------------------------------------------------------
修改记录    :
日  期      版本        修改人        修改内容
05/06/29	4.0			gie			  创建
====================================================================*/		
u16 RPReleaseRec( u32 dwRecoderID );

/*====================================================================
函数名      :RPSetRecNetRcvParam
功能        :设置录像机的网络接收参数
算法实现    :
引用全局变量:
输入参数说明: u8 byRecorderID, 录像机编号
			  TRPNetRcvParam *ptNetRcvParam, 网络参数	 

返回值说明  :操作成功返回RP_OK,失败返回错误码
----------------------------------------------------------------------
修改记录    :
日  期      版本        修改人        修改内容
05/07/10	4.0			fang hui    	  创建
====================================================================*/
u16 RPSetRecNetRcvParam( u32 dwRecorderID, const TRPNetRcvParam *ptNetRcvParam );

//设置码流平均码率，正确返回rp_ok
u16 RPSetAverageBitrate(u32 dwRecorderID, const TStreamAverageBitrate& tStreamAverageBitrate);

/*====================================================================
函数名      :RPSetRemotePT
功能        :设置接收码流 的 Playload值
算法实现    :
引用全局变量:
输入参数说明: u8 byRecoderID, 录像机编号
			  TRemotePTParam *ptPTParam, 载荷结构	 

返回值说明  :操作成功返回RP_OK,失败返回错误码
----------------------------------------------------------------------
修改记录    :
日  期      版本        修改人        修改内容
05/07/10	4.0			fang hui    	  创建
====================================================================*/
u16 RPSetRemotePT( u32 dwRecoderID, const TRemotePTParam *ptPTParam );

/*====================================================================
函数名      :RPSetDecryptKey
功能        :设置 解码key字串 以及 解密模式 Aes or Des
算法实现    :
引用全局变量:
输入参数说明: u8 byRecorderID, 录像机编号
			  TEncDecParam *ptDecryptParam, 解密信息结构	 

返回值说明  :操作成功返回RP_OK,失败返回错误码
----------------------------------------------------------------------
修改记录    :
日  期      版本        修改人        修改内容
05/07/19	4.0			fang hui    	  创建
====================================================================*/
u16 RPSetDecryptKey( u32 dwRecorderID, const TEncDecParam *ptDecryptParam );

/*====================================================================
函数名      :RPGetFileCreateTime
功能        :获取文件创建时间
算法实现    :
引用全局变量:
输入参数说明:u32 dwPlayerID,放像机编号
输出参数说明:u64 qwCreateDate,文件创建时间 采用FILETIME格式
返回值说明  :操作成功返回RP_OK,失败返回错误码
----------------------------------------------------------------------
修改记录    :
日  期      版本        修改人        修改内容
13/08/27	4.0			党剑锋		  创建
====================================================================*/
u16 RPGetFileCreateTime(u32 dwPlayerID, u64 *qwCreateDate, s8 * achOrignalFullName);

/*====================================================================
函数名      :RPStartRec
功能        :使用指定录像机开始录像
算法实现    :
引用全局变量:
输入参数说明:u8  byRecorderID,录像机编号
			 const TRecParam * ptRecParam,录像参数结构
返回值说明  :操作成功返回RP_OK,失败返回错误码
----------------------------------------------------------------------
修改记录    :
日  期      版本        修改人        修改内容
05/06/29	4.0			gie			  创建
====================================================================*/
u16 RPStartRec( u32 dwRecorderID , const TRecParam *ptRecParam );

/*====================================================================
函数名      :RPSetRecStreamSave
功能        :设置录像机录哪路流
算法实现    :
引用全局变量:
输入参数说明:u8  byRecorderID,录像机编号
			 u8 byStreamId, 流序号, 1-音频，2-第一路视频，3-第二路视频
			 BOOL32 bSave, 是否存储
返回值说明  :操作成功返回RP_OK,失败返回错误码
----------------------------------------------------------------------
修改记录    :
日  期      版本        修改人        修改内容
05/12/21	4.0			fang hui			  创建
====================================================================*/
u16 RPSetRecStreamSave( u32 dwRecorderID, u8 byStreamId, BOOL32 bSave );

/*====================================================================
函数名      :RPPauseRec
功能        :暂停录像
算法实现    :
引用全局变量:
输入参数说明:u8  byRecorderID,录像机编号			 
返回值说明  :操作成功返回RP_OK,失败返回错误码
----------------------------------------------------------------------
修改记录    :
日  期      版本        修改人        修改内容
05/07/19	4.0			fang hui			  创建
====================================================================*/
u16 RPPauseRec( u32  dwRecorderID );

/*====================================================================
函数名      :RPResumeRec
功能        :继续录像
算法实现    :
引用全局变量:
输入参数说明:u8  byRecorderID,录像机编号			 
返回值说明  :操作成功返回RP_OK,失败返回错误码
----------------------------------------------------------------------
修改记录    :
日  期      版本        修改人        修改内容
05/07/19	4.0			fang hui			  创建
====================================================================*/
u16 RPResumeRec( u32 dwRecorderID );

/*====================================================================
函数名      :RPSetRecorderSaveMethod
功能        :设置录像器录制方法：抽帧录像或者正常录制,针对视频帧而言
算法实现    :
引用全局变量:
输入参数说明:u8  byRecorderID,录像机编号	
			 BOOL32 bOnlySaveKeyFrame, 是否抽关键帧
			 u32 dwSaveFrameInterval，每 dwSaveFrameInterval 个关键帧存一帧
返回值说明  :操作成功返回RP_OK,失败返回错误码
----------------------------------------------------------------------
修改记录    :
日  期      版本        修改人        修改内容
05/07/19	4.0			fang hui			  创建
====================================================================*/
u16 RPSetRecorderSaveMethod( u32 dwRecorderID, BOOL32 bOnlySaveKeyFrame = FALSE, u32 dwSaveFrameInterval = 1 );

/*====================================================================
函数名      :RPStopRec
功能        :停止指定录像机的录像
算法实现    :
引用全局变量:
输入参数说明:u8  byRecorderID,录像机编号
返回值说明  :操作成功返回RP_OK,失败返回错误码
----------------------------------------------------------------------
修改记录    :
日  期      版本        修改人        修改内容
05/06/29	4.0			gie			  创建
====================================================================*/
u16 RPStopRec( u32 dwRecorderID, u32 dwStopTime = 0 );

/*====================================================================
函数名      :RPSetRecRsParam
功能        :设置录像机的重传处理
算法实现    :
引用全局变量:
输入参数说明:u8  byRecorderID,录像机编号
			 TRPRecRSParam *ptRecRSParam，录像重传处理参数
返回值说明  :操作成功返回RP_OK,失败返回错误码
----------------------------------------------------------------------
修改记录    :
日  期      版本        修改人        修改内容
05/12/09	4.0			fang hui			  创建
====================================================================*/
u16 RPSetRecRsParam( u32 dwRecorderID, TRPRecRSParam *ptRecRSParam );

/*====================================================================
函数名      :RPStartPreRec
功能        :使用指定的录像机开始预录
算法实现    :要使用文件预录,则必须使用小文件形式;正式录像之后不能再调用预录;
			停止正式录像的同时也停止预录,
引用全局变量:
输入参数说明:u8 byRecorderID,录像机编号
			 BOOL32 bPreRecInMem,是否内存预录
			 u32 dwPreRecTime，录像机预录时间长度（单位秒）
			 ptRecParam: 文件预录参数,使用与录像方法一致,路径和文件名也作为正式录像的参数,开始时间作为预录的开始时间,
			 间隔为小文件的间隔,录像时小文件的文件名将一次递增,上层应用时必须清楚知道什么时候是正式开始录像时间;
			 
返回值说明  :操作成功返回RP_OK,失败返回错误码
----------------------------------------------------------------------
修改记录    :
日  期      版本        修改人        修改内容
05/06/29	4.0			gie			  创建
====================================================================*/
u16 RPStartPreRec( u32 dwRecorderID , BOOL32 bPreRecInMem, u32 dwPreRecTimeInterval, const TRecParam *ptRecParam );

/*====================================================================
函数名      :RPStopPreRec
功能        :停止指定的录像机的预录
算法实现    :
引用全局变量:
输入参数说明:u8 byRecorderID,录像机编号
返回值说明  :操作成功返回RP_OK,失败返回错误码
----------------------------------------------------------------------
修改记录    :
日  期      版本        修改人        修改内容
05/06/30	4.0			gie			  创建
====================================================================*/
u16 RPStopPreRec( u32 dwRecorderID );

/*====================================================================
函数名      :RPGetPreRecTime
功能        :得到当前预录总时间
算法实现    :
引用全局变量: 
输入参数说明:u8 byRecorderID,录像机编号
			 u32& dwPreRecTime,预录总时间（单位秒）
返回值说明  :操作成功返回RP_OK,失败返回错误码
----------------------------------------------------------------------
修改记录    :
日  期      版本        修改人        修改内容
05/12/31	4.0			gie			  创建
====================================================================*/
u16 RPGetPreRecTime( u32 dwRecorderID , u32& dwPreRecTime );

/*====================================================================
函数名      :RPGetRecordTotalLength
功能        :得到当前写入录象数据长度
算法实现    :
引用全局变量: 
输入参数说明:u8 byRecorderID,录像机编号
			 u64& qwRecordTotalLength（单位Bytes）
返回值说明  :操作成功返回RP_OK,失败返回错误码
----------------------------------------------------------------------
修改记录    :
日  期      版本        修改人        修改内容
05/12/31	4.0			gie			  创建
====================================================================*/
u16 RPGetRecordTotalLength( u32 dwRecorderID , u64& qwRecordTotalLength );

/*====================================================================
函数名      :RPSetRecDebugInfo
功能        :设置debug信息
算法实现    :
引用全局变量:
输入参数说明: u32 dwRecorderID, 录像机设备号
			  BOOL32 bIsWriteAsfWhenLoseFrame,丢帧时是否写文件
			  u8 byAudioBuffSize,音频buff大小
			  u8 byVideoBuffSize,视频buff大小
			  u8 byDsBuffSize, 双流buff大小
			  u8 byLeftBuffSize，最大剩余buff大小
返回值说明  :
----------------------------------------------------------------------
修改记录    :
日  期      版本        修改人        修改内容
2012/06/17	4.7			zhangli		  create
====================================================================*/
u16 RPSetRecDebugInfo(u32 dwRecorderID, BOOL32 bIsWriteAsfWhenLoseFrame, u8 byAudioBuffSize,
			u8 byVideoBuffSize, u8 byDsBuffSize, u8 byLeftBuffSize);
/*====================================================================
函数名      :RPCreatePlay
功能        :创建放像机，得到可用的放像机编号
算法实现    :
引用全局变量:
输入参数说明:u8 *pbyPlayerID, 指向放像机编号，返回给用户			 
返回值说明  :操作成功返回RP_OK,失败返回错误码
----------------------------------------------------------------------
修改记录    :
日  期      版本        修改人        修改内容
05/06/29	4.0			gie			  创建
====================================================================*/		
u16 RPCreatePlay( u32 *pdwPlayerID );

/*====================================================================
函数名      :RPReleasePlay
功能        :释放放像机
算法实现    :
引用全局变量:
输入参数说明:u8 byPlayerID, 放像机编号	 
返回值说明  :操作成功返回RP_OK,失败返回错误码
----------------------------------------------------------------------
修改记录    :
日  期      版本        修改人        修改内容
05/06/29	4.0			gie			  创建
====================================================================*/		
u16 RPReleasePlay( u32 dwPlayerID );

/*====================================================================
函数名      :RPSetReadCallBackGran
功能        :设置文件进度回调的粒度
算法实现    :
引用全局变量:
输入参数说明:u8 byPlayerID, 放像机编号
			 u8 byGran, 导致回调的读文件粒度(以秒为单位,应处于[MIN_READ_CALLBACK_GRAN,MAX_READ_CALLBACK_GRAN]范围)
返回值说明  :操作成功返回RP_OK,失败返回错误码
----------------------------------------------------------------------
修改记录    :
日  期      版本        修改人        修改内容
05/06/29	4.0			gie			  创建
====================================================================*/	
u16 RPSetReadCallBackGran( u32 dwPlayerID, u8 byGran );

/*====================================================================
函数名      :RPSetLocalPT
功能        :设置本地码流 的 Playload值
算法实现    :
引用全局变量:
输入参数说明: u8 byPlayerID, 放像机编号
			  TLocalPTParam *ptPTParam, 载荷结构	 

返回值说明  :操作成功返回RP_OK,失败返回错误码
----------------------------------------------------------------------
修改记录    :
日  期      版本        修改人        修改内容
05/07/10	4.0			fang hui    	  创建
====================================================================*/
u16 RPSetLocalPT (u32 dwPlayerID, const TLocalPTParam *ptPTParam );

/*====================================================================
函数名      :RPSetEncryptKey
功能        :设置 加密key字串 以及 加密模式 Aes or Des
算法实现    :
引用全局变量:
输入参数说明: u8 byPlayerID, 放像机编号
			  TEncDecParam *ptEncryptParam, 加密信息结构	 

返回值说明  :操作成功返回RP_OK,失败返回错误码
----------------------------------------------------------------------
修改记录    :
日  期      版本        修改人        修改内容
05/07/19	4.0			fang hui    	  创建
====================================================================*/
u16 RPSetEncryptKey( u32 dwPlayerID, TEncDecParam *ptEncryptParam );

/*====================================================================
函数名      :RPGetFileInfo
功能        :得到指定文件中的信息
算法实现    :
引用全局变量:
输入参数说明:u8 byPlayerID,放像机编号
			 s8 *pchFullFileNam,文件全名
			 TMediaInfo *ptMediaInfo, 媒体信息
返回值说明  :操作成功返回RP_OK,失败返回错误码
----------------------------------------------------------------------
修改记录    :
日  期      版本        修改人        修改内容
09/03/27	1.0			ZYQ			  创建
====================================================================*/
u16 RPGetFileInfo( u32 dwPlayerID, s8 *pchFullFileName, TFileInfo *ptFileInfo );
/*====================================================================
函数名      :RPGetFileMediaInfo
功能        :得到指定文件中的流信息
算法实现    :
引用全局变量:
输入参数说明:u8 byPlayerID,放像机编号
			 s8 *pchFullFileNam,文件全名
			 TMediaInfo *ptMediaInfo, 媒体信息
返回值说明  :操作成功返回RP_OK,失败返回错误码
----------------------------------------------------------------------
修改记录    :
日  期      版本        修改人        修改内容
05/08/11	4.0			fang hui			  创建
====================================================================*/
u16 RPGetFileMediaInfo( u32 dwPlayerID, s8 *pchFullFileName, TMediaInfo *ptMediaInfo );

/*====================================================================
函数名      :RPGetFileHpAttrib
功能        :得到指定文件中的各路适配的hp/bp属性信息
算法实现    :
引用全局变量:
输入参数说明:u8 byPlayerID,放像机编号
			 s8 *pchFullFileNam,文件全名
返回值说明  :操作成功返回RP_OK,失败返回错误码
----------------------------------------------------------------------
修改记录    :
日  期      版本        修改人        修改内容
12/02/21	4.7			zhouyiliang		  创建
====================================================================*/
u16 RPGetFileHpAttrib( u32 dwPlayerID ,s8 *pchFullFileName, BOOL32& bFirstVidHpAttri, BOOL32& bSecVidHpAttri );

/*====================================================================
函数名      :RPGetAudioChnlNum
功能        :取音频声道数
算法实现    :
引用全局变量:
输入参数说明:pchFullFileName,文件全路径
			 u8 *pChnlNum,返回声道数，是个数组指针，数组大小为音频流的个数
返回值说明  :成功，返回RP_OK；失败，返回错误码
----------------------------------------------------------------------
修改记录    :
日  期      版本        修改人        修改内容
2012/11/21	4.7			zhangli 	   create
====================================================================*/
u16 RPGetAudioChnlNum(u32 dwPlayerID ,s8 *pchFullFileName, u8 *pChnlNum);

/*====================================================================
函数名      :RPSetPlayNetParam
功能        :设置指定放相机的指定码流的发送地址参数
算法实现    :
引用全局变量:
输入参数说明:u8 byPlayerID,放像机编号
			 u8 byIndex，码流索引编号
			 const TNetSndParam& tSndPar,放像发送地址参数结构
返回值说明  :操作成功返回RP_OK,失败返回错误码
----------------------------------------------------------------------
修改记录    :
日  期      版本        修改人        修改内容
09/03/31	1.0			zyq			  创建
====================================================================*/
u16 RPSetPlayNetParam( u32 dwPlayerID , u8 byIndex, const TNetSndParam& tSndPar );

/*====================================================================
函数名      :RPStartPlay
功能        :使用指定的放像机进行网络放像
算法实现    :
引用全局变量:
输入参数说明:u8 byPlayerID,放像机编号
			 const TPlayParam *ptPlayParam,放像参数结构
返回值说明  :操作成功返回RP_OK,失败返回错误码
----------------------------------------------------------------------
修改记录    :
日  期      版本        修改人        修改内容
05/06/30	4.0			gie			  创建
====================================================================*/
u16 RPStartPlay( u32 dwPlayerID , TPlayParam *ptPlayParam );

/*====================================================================
函数名      :RPGetFstKeyFrmHdr
功能        :使用指定的放像机获取播放的文件码流第一个关键帧头,
			必须在调用GetFileMediaInfo之后调用
算法实现    :
引用全局变量:
输入参数说明:u8 byPlayerID,放像机编号
			 ptKeyFrameHdr,关键帧头数据结构
返回值说明  :操作成功返回RP_OK,失败返回错误码
----------------------------------------------------------------------
修改记录    :
日  期      版本        修改人        修改内容
05/06/30	4.0			gie			  创建
====================================================================*/
u16 RPGetFstKeyFrmHdr(u32 dwPlayerID ,TKeyFrameHdr* ptKeyFrameHdr);

/*====================================================================
函数名      :RPRestartPlay
功能        :使用指定的放像机重新开始放相
算法实现    :
引用全局变量:
输入参数说明:u8 byPlayerID,放像机编号
			 
返回值说明  :操作成功返回RP_OK,失败返回错误码
----------------------------------------------------------------------
修改记录    :
日  期      版本        修改人        修改内容
05/06/30	4.0			gie			  创建
====================================================================*/
u16 RPRestartPlay( u32 dwPlayerID );
/*====================================================================
函数名      :RPPausePlay
功能        :暂停指定放像机的网络放像
算法实现    :
引用全局变量:
输入参数说明:u8 byPlayerID,放像机编号
返回值说明  :操作成功返回RP_OK,失败返回错误码
----------------------------------------------------------------------
修改记录    :
日  期      版本        修改人        修改内容
05/06/30	4.0			gie			  创建
====================================================================*/
u16 RPPausePlay( u32 dwPlayerID );


/*====================================================================
函数名      :RPSetSwitchFileNamePattern
功能        :设置大文件切换时的文件名模板
算法实现    :
引用全局变量:
输入参数说明:u8  byPlayerID，放像机编号
s8 * pchPattern，文件名模板
返回值说明  :操作成功返回RP_OK,失败返回错误码
----------------------------------------------------------------------
修改记录    :
日  期      版本        修改人        修改内容
07/07/10	4.0			baihao			  创建
====================================================================*/
u16 RPSetSwitchFileNamePattern( u8 byRecorderID, s8 * pchPattern );

/*====================================================================
函数名      :RPSetAudioTypeDesc
功能        :设置音频格式的通道等信息
算法实现    :
引用全局变量:
输入参数说明:u8  byRecorderID,录像机编号
TAudioTypeDesc &tAudioTypeDesc 音频类型信息
返回值说明  :操作成功返回RP_OK,失败返回错误码
----------------------------------------------------------------------
修改记录    :
日  期      版本        修改人        修改内容
2012/11/01	4.7			zhangli		   create
====================================================================*/
u16 RPSetAudioTypeDesc(u8 byRecorderID, u8 byChnlNum=0, u32 dwSampleRate=0);

/*====================================================================
函数名      :RPResumePlay
功能        :恢复指定放像机的网络放像
算法实现    :
引用全局变量:
输入参数说明:u8 byPlayerID,放像机编号
返回值说明  :操作成功返回RP_OK,失败返回错误码
----------------------------------------------------------------------
修改记录    :
日  期      版本        修改人        修改内容
05/06/30	4.0			gie			  创建
====================================================================*/
u16 RPResumePlay( u32 dwPlayerID );

/*====================================================================
函数名      :RPStopPlay
功能        :停止指定放像机的网络放像
算法实现    :
引用全局变量:
输入参数说明:u8 byPlayerID,放像机编号
返回值说明  :操作成功返回RP_OK,失败返回错误码
----------------------------------------------------------------------
修改记录    :
日  期      版本        修改人        修改内容
05/06/30	4.0			gie			  创建
====================================================================*/
u16 RPStopPlay( u32 dwPlayerID );

/*====================================================================
函数名      :RPSetPlyRsParam
功能        :设置放像机的重传处理
算法实现    :
引用全局变量:
输入参数说明:u8  byPlayerID,放像机编号
			 TRPPlyRSParam *ptPlyRSParam，放像重传处理参数
返回值说明  :操作成功返回RP_OK,失败返回错误码
----------------------------------------------------------------------
修改记录    :
日  期      版本        修改人        修改内容
05/12/09	4.0			fang hui			  创建
====================================================================*/
u16 RPSetPlyRsParam( u32 dwPlayerID, TRPPlyRSParam *ptPlyRSParam );

/*====================================================================
函数名      :RPQuickPlay
功能        :提高指定放像机的网络放像速度
算法实现    :
引用全局变量:
输入参数说明:u8 byPlayerID,放像机编号
			 u8 byQuickTimes,提高放像的速度(2倍、4倍等)
返回值说明  :操作成功返回RP_OK,失败返回错误码
----------------------------------------------------------------------
修改记录    :
日  期      版本        修改人        修改内容
05/06/30	4.0			gie			  创建
====================================================================*/
u16 RPQuickPlay( u32 dwPlayerID , u8 byQuickTimes );

/*====================================================================
函数名      :RPSlowPlay
功能        :降低指定放像机的网络放像速度
算法实现    :
引用全局变量:
输入参数说明:u8 byPlayerID,放像机编号
			 u8 bySlowTimes,降低放像的速度(=2--1/2倍、=4--1/4倍等)
返回值说明  :操作成功返回RP_OK,失败返回错误码
----------------------------------------------------------------------
修改记录    :
日  期      版本        修改人        修改内容
05/06/30	4.0			gie			  创建
====================================================================*/
u16 RPSlowPlay( u32 dwPlayerID , u8 bySlowTimes );

/*====================================================================
函数名      :RPDragDropPlay
功能        :立刻使用指定放像机从指定时间开始放像
算法实现    :
引用全局变量:
输入参数说明:u8 byPlayerID,放像机编号
			 u32 dwDragDropTime，拖放目标时间（单位秒）
返回值说明  :操作成功返回RP_OK,失败返回错误码
----------------------------------------------------------------------
修改记录    :
日  期      版本        修改人        修改内容
05/06/30	4.0			gie			  创建
====================================================================*/
u16 RPDragDropPlay( u32 dwPlayerID , u32 dwDragDropTime );

/*====================================================================
函数名      :RPGetRecorderStatus
功能        :查询指定录像机的录像状态
算法实现    :
引用全局变量:
输入参数说明:u8 byRecorderID，录像机编号
			 TRecorderStatus *ptRecorderStatus，录像机状态
返回值说明  :操作成功返回RP_OK,失败返回错误码
----------------------------------------------------------------------
修改记录    :
日  期      版本        修改人        修改内容
05/06/30	4.0			gie			  创建
====================================================================*/
u16 RPGetRecorderStatus( u32 dwRecorderID, TRecorderStatus *ptRecorderStatus );

/*====================================================================
函数名      :RPGetPlayerStatus
功能        :查询指定放像机的放像状态
算法实现    :
引用全局变量:
输入参数说明:u8  byPlayerID，放像机编号
			 TPlayerStatus *ptPlayerStatus，放像机状态
返回值说明  :操作成功返回RP_OK,失败返回错误码
----------------------------------------------------------------------
修改记录    :
日  期      版本        修改人        修改内容
05/06/30	4.0			gie			  创建
====================================================================*/
u16 RPGetPlayerStatus( u32 dwPlayerID, TPlayerStatus *ptPlayerStatus );

/*====================================================================
函数名      :RPGetRecorderStatis
功能        :查询指定录像机的录像统计
算法实现    :
引用全局变量:
输入参数说明:u8 byRecorderID，录像机编号
			 TRecorderStatis *ptRecorderStatis，录像机统计
返回值说明  :操作成功返回RP_OK,失败返回错误码
----------------------------------------------------------------------
修改记录    :
日  期      版本        修改人        修改内容
05/06/30	4.0			gie			  创建
====================================================================*/
u16 RPGetRecorderStatis( u32 dwRecorderID, TRecorderStatis *ptRecorderStatis );

/*====================================================================
函数名      :RPGetPlayerStatis
功能        :查询指定放像机的放像统计
算法实现    :
引用全局变量:
输入参数说明:u8  byPlayerID，放像机编号
			 TPlayerStatis *ptPlayerStatis，放像机统计
返回值说明  :操作成功返回RP_OK,失败返回错误码
----------------------------------------------------------------------
修改记录    :
日  期      版本        修改人        修改内容
05/06/30	4.0			gie			  创建
====================================================================*/
u16 RPGetPlayerStatis( u32 dwPlayerID, TPlayerStatis *ptPlayerStatis );

/*====================================================================
函数名      :RPSetFrameSizeInfo
功能        :设置帧长数据信息
算法实现    :
引用全局变量:
输入参数说明: u32 dwPlayerID,放像机编号
			  TFrameSizeInfo *ptFrameSizeInfo,媒体类型和对应帧长信息
返回值说明  :TRUE or FLASE
----------------------------------------------------------------------
修改记录    :
日  期      版本        修改人        修改内容
2012/06/17	4.7			zhangli		  create
====================================================================*/
u16 RPSetFrameSizeInfo(u32 dwPlayerID, TFrameSizeInfo *ptFrameSizeInfo);
/*====================================================================
函数名      :recstatus
功能        :打印指定录像机的录像状态
算法实现    :
引用全局变量:
输入参数说明:u8 byRecorderID，录像机编号
返回值说明  :操作成功返回RP_OK,失败返回错误码
----------------------------------------------------------------------
修改记录    :
日  期      版本        修改人        修改内容
05/06/30	4.0			gie			  创建
====================================================================*/
API u16 recstatus( u32 dwRecorderID );

/*====================================================================
函数名      :plystatus
功能        :打印指定放像机的放像状态
算法实现    :
引用全局变量:
输入参数说明:u8 byPlayerID，放像机编号
返回值说明  :操作成功返回RP_OK,失败返回错误码
----------------------------------------------------------------------
修改记录    :
日  期      版本        修改人        修改内容
05/06/30	4.0			gie			  创建
====================================================================*/
API u16 plystatus( u32 dwPlayerID );

/*====================================================================
函数名      :recstatis
功能        :打印指定录像机的媒体统计
算法实现    :
引用全局变量:
输入参数说明:u8 byRecorderID，录像机编号
返回值说明  :操作成功返回RP_OK,失败返回错误码
----------------------------------------------------------------------
修改记录    :
日  期      版本        修改人        修改内容
05/06/30	4.0			gie			  创建
====================================================================*/
API u16 recstatis( u32 dwRecorderID );

/*====================================================================
函数名      :plystatis
功能        :打印指定放像机的放像统计
算法实现    :
引用全局变量:
输入参数说明:u8 byPlayerID，放像机编号
返回值说明  :操作成功返回RP_OK,失败返回错误码
----------------------------------------------------------------------
修改记录    :
日  期      版本        修改人        修改内容
05/06/30	4.0			gie			  创建
====================================================================*/
API u16 plystatis( u32 dwPlayerID );

/*====================================================================
函数名      :RPDumpFreeRP
功能        :打印可用的录放像机的编号
算法实现    :
引用全局变量:
输入参数说明:
返回值说明  :操作成功返回RP_OK,失败返回错误码
----------------------------------------------------------------------
修改记录    :
日  期      版本        修改人        修改内容
05/07/05	4.0		   fang hui 		创建
====================================================================*/
API u16 freerp();

/*====================================================================
函数名      :RPVersion
功能        :打印录放像库版本信息
算法实现    :
引用全局变量:
输入参数说明:
返回值说明  :操作成功返回RP_OK,失败返回错误码
----------------------------------------------------------------------
修改记录    :
日  期      版本        修改人        修改内容
05/06/30	4.0			gie			  创建
====================================================================*/
API u16 rpver();

/*====================================================================
函数名      :RPHelp
功能        :录放像库帮助
算法实现    :
引用全局变量:
输入参数说明:
返回值说明  :操作成功返回RP_OK,失败返回错误码
----------------------------------------------------------------------
修改记录    :
日  期      版本        修改人        修改内容
05/06/30	4.0			gie			  创建
====================================================================*/
API u16 rphelp();

/*====================================================================
函数名      :recinfo
功能        :录象全局统计
算法实现    :
引用全局变量:
输入参数说明:
返回值说明  :操作成功返回RP_OK,失败返回错误码
----------------------------------------------------------------------
修改记录    :
日  期      版本        修改人        修改内容
06/07/20	4.0			gie			  创建
====================================================================*/
API u16 recinfo();

/*====================================================================
函数名      :print1080i
功能        :开启/关闭录像的1080i帧消息
算法实现    :
引用全局变量:
输入参数说明:bEnable 
返回值说明  :操作成功返回RP_OK,失败返回错误码
----------------------------------------------------------------------
修改记录    :
日  期      版本        修改人        修改内容
06/07/20	4.0			gie			  创建
====================================================================*/
API void print1080i(BOOL bEnable);

/*====================================================================
函数名      :savestream
功能        :存录像码流
算法实现    :
引用全局变量:
输入参数说明: u8 byRecorderID: 录像机编号
			  u8 byStreamId：第几路码流
			  u32 dwFrmNum： 要存的帧数
返回值说明  :操作成功返回RP_OK,失败返回错误码
----------------------------------------------------------------------
修改记录    :
日  期      版本        修改人        修改内容
05/07/12	4.0			fang hui		  创建
====================================================================*/
API u16 savestream( u32 dwRecorderID, u8 byStreamId, u32 dwFrmNum );

/*====================================================================
函数名      :RPListFile
功能        :列举指定目录下的文件
算法实现    :
引用全局变量:
输入参数说明: s8 *pchPath, 指定目录
			  TListInfo *ptListInfo， 文件列表
返回值说明  :操作成功返回RP_OK,失败返回错误码
----------------------------------------------------------------------
修改记录    :
日  期      版本        修改人        修改内容
05/07/19	4.0			fang hui		  创建
====================================================================*/
u16 RPListFile( s8 *pchPath, TListInfo *ptListInfo );

//得到小文件文件夹播放长度
//pchpath：小文件路径
//dwplayduration：播放长度(单位:s)
//!!!只能用于获取小文件文件夹播放长度
u16 RPGetPlayDuration(s8* pchPath, u32& dwPlayDuration );

/*====================================================================
函数名      :RPDeleteFile
功能        :删除录象文件
算法实现    :
引用全局变量:
输入参数说明: s8 *pchPath, 文件目录
				s8* pchName = NUL, 文件名,带.asf后缀
				BOOL32 bBigFile = FALSE, 是否大文件,如果是大文件,要求给出路径和文件名
				如果是小文件,只要给出路径
返回值说明  :操作成功返回RP_OK,失败返回错误码
----------------------------------------------------------------------
修改记录    :
日  期      版本        修改人        修改内容
06/02/16	4.0			gie		  创建
====================================================================*/
u16 RPDeleteFile( s8 *pchPath, s8* pchName = NULL, BOOL32 bBigFile = FALSE);

/*====================================================================
函数名      :RPRenameFile
功能        :重命名指定文件
算法实现    :
引用全局变量:
输入参数说明: s8 *pchOldName, 旧文件名
			  s8 *pchNewName，新文件名 
返回值说明  :操作成功返回RP_OK,失败返回错误码
----------------------------------------------------------------------
修改记录    :
日  期      版本        修改人        修改内容
05/09/26	4.0			fang hui		  创建
====================================================================*/
u16 RPRenameFile(s8 *pchOldName, s8 *pchNewName);

/*====================================================================
函数名      :SetRPMediaTOS
功能        :设置媒体TOS值 nType类型0:全部 1:音频 2:视频 3: 数据
算法实现    :
引用全局变量:
输入参数说明:	int nTOS, tos值
				int nType，类型	
返回值说明  :无
----------------------------------------------------------------------
修改记录    :
日  期      版本        修改人        修改内容
05/11/09	4.0			fang hui		  创建
====================================================================*/
int SetRPMediaTOS( int nTOS, int nType );

/*====================================================================
函数名      :GetRPMediaTOS
功能        :得到媒体TOS值 nType类型0:全部 1:音频 2:视频 3: 数据
算法实现    :
引用全局变量:
输入参数说明:	int nType，类型	
返回值说明  :无
----------------------------------------------------------------------
修改记录    :
日  期      版本        修改人        修改内容
05/11/09	4.0			fang hui		  创建
====================================================================*/
int GetRPMediaTOS( int nType );

/*====================================================================
函数名      :RPCreateFtpRcv
功能        :创建录象下载，得到可用的下载接收编号
算法实现    :
引用全局变量:
输入参数说明:u8 *pbyFtpRcvID, 指向下载接收编号，返回给用户			 
返回值说明  :操作成功返回RP_OK,失败返回错误码
----------------------------------------------------------------------
修改记录    :
日  期      版本        修改人        修改内容
05/12/27	4.0			gie			  创建
====================================================================*/		
u16 RPCreateFtpRcv( u8 *pbyFtpRcvID );

/*====================================================================
函数名      :RPReleaseFtpRcv
功能        :释放录象下载
算法实现    :
引用全局变量:
输入参数说明:u8 byFtpRcvID, 下载接收编号
返回值说明  :操作成功返回RP_OK,失败返回错误码
----------------------------------------------------------------------
修改记录    :
日  期      版本        修改人        修改内容
05/12/27	4.0			gie			  创建
====================================================================*/		
u16 RPReleaseFtpRcv( u8 byFtpRcvID );

/*====================================================================
函数名      :RPStartFtpRcv
功能        :开始下载接收
算法实现    :
引用全局变量:
输入参数说明:u8  byFtpRcvID,下载接收编号
			 TFtpRcvParam *ptFtpRcvParam,录像下载接收参数
返回值说明  :操作成功返回RP_OK,失败返回错误码
----------------------------------------------------------------------
修改记录    :
日  期      版本        修改人        修改内容
05/12/27	4.0			gie			  创建
====================================================================*/
u16 RPStartFtpRcv( u8 byFtpRcvID , TFtpRcvParam *ptFtpRcvParam );

/*====================================================================
函数名      :RPStopFtpRcv
功能        :停止下载接收
算法实现    :
引用全局变量:
输入参数说明:u8  byFtpRcvID,下载接收编号
返回值说明  :操作成功返回RP_OK,失败返回错误码
----------------------------------------------------------------------
修改记录    :
日  期      版本        修改人        修改内容
05/12/27	4.0			gie			  创建
====================================================================*/
u16 RPStopFtpRcv( u8 byFtpRcvID );

/*====================================================================
函数名      :RPCreateFtpSnd
功能        :创建录象下载发送，得到可用的发送编号
算法实现    :
引用全局变量:
输入参数说明:u8 *pbyFtpSndID, 指向下载发送编号，返回给用户			 
返回值说明  :操作成功返回RP_OK,失败返回错误码
----------------------------------------------------------------------
修改记录    :
日  期      版本        修改人        修改内容
05/12/27	4.0			gie			  创建
====================================================================*/		
u16 RPCreateFtpSnd( u8 *pbyFtpSndID );

/*====================================================================
函数名      :RPReleaseFtpSnd
功能        :释放录象下载发送
算法实现    :
引用全局变量:
输入参数说明:u8 byFtpSndID, 下载发送编号
返回值说明  :操作成功返回RP_OK,失败返回错误码
----------------------------------------------------------------------
修改记录    :
日  期      版本        修改人        修改内容
05/12/27	4.0			gie			  创建
====================================================================*/		
u16 RPReleaseFtpSnd( u8 byFtpSndID );

/*====================================================================
函数名      :RPSetFtpReadCallBackGran
功能        :设置文件下载进度回调的粒度
算法实现    :
引用全局变量:
输入参数说明:u8 byFtpSndID, 下载发送编号
			 u8 byGran, 导致回调的读文件粒度(以秒为单位,应处于[MIN_READ_CALLBACK_GRAN,MAX_READ_CALLBACK_GRAN]范围)
返回值说明  :操作成功返回RP_OK,失败返回错误码
----------------------------------------------------------------------
修改记录    :
日  期      版本        修改人        修改内容
05/06/29	4.0			gie			  创建
====================================================================*/	
u16 RPSetFtpReadCallBackGran( u8 byFtpSndID, u8 byGran );

/*====================================================================
函数名      :RPStartFtpSnd
功能        :开始下载发送
算法实现    :
引用全局变量:
输入参数说明:u8  byFtpSndID,下载发送编号
			 TFtpSndParam *ptFtpSndParam,录像下载发送参数
返回值说明  :操作成功返回RP_OK,失败返回错误码
----------------------------------------------------------------------
修改记录    :
日  期      版本        修改人        修改内容
05/12/27	4.0			gie			  创建
====================================================================*/
u16 RPStartFtpSnd( u8 byFtpSndID , TFtpSndParam *ptFtpSndParam );

/*====================================================================
函数名      :RPStopFtpSnd
功能        :停止下载发送
算法实现    :
引用全局变量:
输入参数说明:u8  byFtpSndID,下载发送编号
返回值说明  :操作成功返回RP_OK,失败返回错误码
----------------------------------------------------------------------
修改记录    :
日  期      版本        修改人        修改内容
05/12/27	4.0			gie			  创建
====================================================================*/
u16 RPStopFtpSnd( u8 byFtpSndID );

/* 注意：下面接口仅供codecwrapper调用，其它模块不要调用 !!! */

/*====================================================================
函数名      :RecDataPrc
功能        :录像数据处理
算法实现    :透明调用网络回调,提供给codecwrapper用
引用全局变量:
输入参数说明:	PFRAMEHDR pFrmHdr, 帧头
				dwContext， 上下文	
返回值说明  :无
----------------------------------------------------------------------
修改记录    :
日  期      版本        修改人        修改内容
05/10/19	4.0			fang hui		  创建
====================================================================*/
void RecDataPrc( PFRAMEHDR pFrmHdr, u32 dwContext );

/*====================================================================
函数名      :GetRecAddr
功能        :得到录像机的地址
算法实现    :
引用全局变量:
输入参数说明:	u8 byRecId, 录像机编号
				u32 *pdwAddr, 指向地址的指针
返回值说明  :操作成功返回RP_OK,失败返回错误码
----------------------------------------------------------------------
修改记录    :
日  期      版本        修改人        修改内容
05/10/19	4.0			fang hui		  创建
====================================================================*/
u16 GetRecAddr( u32 dwRecId, u32 *pdwAddr );

/*====================
以下为ftp udp相关接口
对于ftp服务端: 先调用RpFtpStartSndService启动ftp发送服务,再调用RpFtpSndProbePack发送
探测包
add zhx 06-04-17
=====================*/
/*===================================================================
//启动ftp 服务端服务
TFtpSndServParam : 本地绑定网络地址
正确返回: rp_ok
====================================================================*/
u16 RpFtpStartSndService(TFtpSndServParam tFtpSndServParam);

/*===================================================================
//停止ftp服务端服务
正确返回: rp_ok
====================================================================*/
u16 RpFtpStopSndService();

/*===================================================================
//ftp服务端发送探测包接口
u8*: 要发送数据缓冲
u32: 要发送数据长度
TNetAddr: 要发送的对端地址
u32 dwNatIntvl = 0: 发送探测包的频率(以s为单位)，=0时按上层的缓冲发送，且只发送一次；
					〉0时为底层是否自动发送的标志，且忽略上层的缓冲内容
正确返回: rp_ok
====================================================================*/
u16 RpFtpSndProbePack(u8* pBuf,	u32 dwBufLen, TNetAddr tRemoteAddr, u32 dwNatIntvl = 0);

/*===================================================================
//ftp服务端查询
正确返回: rp_ok
====================================================================*/
u16 RpFtpSndQuery(TFtpSndQuery& tFtpSndQuery);


/*
客户端使用：先调用RpFtpRcvCreate创建下载对象，在调用RpFtpRcvSndProbe发送
探测包（不过nat，则可省略），然后调用RpFtpStartRcv开始下载
*/
/*====================================================================
//创建下载客户端
tlocaladdr: 本地绑定网络地址
pdwssid：标识号
正确返回：rp_ok
====================================================================*/
u16 RpFtpRcvCreate(TNetAddr& tLocalAddr, u32* pdwSsid );

/*====================================================================
//客户端发送探测包,  !!!!!每调用一次，只发送一次,如果要穿代理，则上层也要把
代理数据放到pbuf里，注意加上代理数据长度，rpctrl只是透明转发
dwssid: 客户端标识号
tremoteaddr:对端网络地址
pbuf: 用户缓冲
dwbuflen: 用户缓冲长度
正确返回:rp_ok
====================================================================*/
u16 RpFtpRcvSndProbe(u32 dwSsid, TNetAddr& tRemoteAddr, u8* pBuf, u32 dwBufLen );

/*===================================================================
//ftp客户端启动下载接口
TFtpUdpRcvParam: 下载参数
u32*: 下载ssid标示号
u32 dwNatIntvl = 0: 发送探测包的频率(以s为单位)，0时为不穿nat，〉0时为穿nat
!!!!!!!只自动发送探测包10次，如果没有回应，下载失败
正确返回: rp_ok
====================================================================*/
u16 RpFtpStartRcv(TFtpUdpRcvParam tFtpRcvParam, u32 dwSsid, u32 dwNatIntvl/*=0*/);

/*===================================================================
//ftp客户端结束下载接口
dwSsid: 下载标示号
正确返回: rp_ok
====================================================================*/
u16 RpFtpStopRcv(u32 dwSsid);

/*===================================================================
//ftp客户端查询
u32: 标示号
TFtpRcvQuery: 
正确返回: rp_ok
====================================================================*/
u16 RpFtpRcvQuery(u32 dwSsid, TFtpRcvQuery& tFtpRcvQuery);

u16 RpSetMediaStreamCallback(u32 dwRecorderId, MediaStreamCallBack pfCB, u32 dwContext, BOOL32 bRec);

API void rpftpdebug(BOOL32 bDebug);

u16 RPSetFECEnable(u32 dwPlayerID, BOOL32 bEnable, u8 byStreamId);

u16 RPSetFECPackLEn(u32 dwPlayerID, u16 wLen, u8 byStreamId);

u16 RPSetFECInterFrame(u32 dwPlayerID, BOOL32 bInterFrame, u8 byStreamId);

u16	RPSetFECModeu32(u32 dwPlayerID, u8 byMode, u8 byStreamId);

u16 RPSetFECXY(u32 dwPlayerID, s32 nDataPackNum, s32 nCRCPackNum, u8 byStreamId);

#endif  //_RPCtrl_H_
