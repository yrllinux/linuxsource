/************************************************************
//  文 件 名：  asflib.h
//  相关文件：  asflib.cpp
//	作    者：	赵红显
//	创建时间：	2005-08-29
//	修改时间：	
************************************************************/

#ifndef __ASF_LIB__
#define __ASF_LIB__

#if __Win32__ || WIN32
#pragma pack(push, 1)				//	用一个字节的结构对齐，防止不同的编译器导致错误
#endif	//WIN32


/************************************************************
*************************************************************

//定义ASF文件读写接口的数据结构

*************************************************************
*************************************************************/
#include "kdvtype.h"
#include "osp.h"

#define OUT		// 表示函数的参数是输入参数
#define IN		// 表示函数的参数是输出参数

#define MAX_ASF_READER					400		//最多可播放路数
#define MAX_ASF_PATH_LENTGTH			128		//asf文件路径最大长度,调试用,上层不用考虑

#define COPY_DWORD(achByte, dwInt)          do{*(achByte) =   (u8)(((u32)dwInt)& 0xff) ; \
                                               *(achByte+1) = (u8)(((u32)dwInt >> 8)& 0xff) ; \
                                               *(achByte+2) = (u8)(((u32)dwInt >> 16)& 0xff); \
                                               *(achByte+3) = (u8)(((u32)dwInt >> 24)& 0xff); \
                                               } while(0);

#define COPY_WORD(achByte, wInt)             do{*(achByte) =   (u8)(((u16)wInt)& 0xff) ; \
                                                *(achByte+1) = (u8)(((u16)wInt >> 8)& 0xff); \
                                               } while(0);

typedef void* HTimer;

#ifndef PTR_CHECK_RET
#define PTR_CHECK_RET(p, r)  do{ if(!(p)) { OspPrintf(1, 0, "Point %s is null\n", #p); return (r); } }while(0)
#endif

#define CHECKSTREAMNUMBER(a)	(((a) >= 1 && (a) <= 127) ? TRUE : FALSE)

//编码器对应到媒体类型时用到的结构
typedef struct tagAudioTypeInfo 
{
	u8 byAudioMediaType;//音频媒体类型mp3,pcma,pcmu,g728,g729...
	u8 byAudioMode;//音频模式
}TAudioTypeInfo
#if defined(_VXWORKS_) || defined(_LINUX_)
__attribute__ ( (packed) ) 
#endif
;

typedef union tagMediaDetailTypeIfo 
{
	u8 byVideoMediaType;//视频媒体类型h261,h263,h264,mp4...
	TAudioTypeInfo tAudioTypeInfo;
}UnionMediaDetailTypeInfo
#if defined(_VXWORKS_) || defined(_LINUX_)
__attribute__ ( (packed) ) 
#endif
;

//  文件属性相关结构 
typedef struct tagASFFileProperty{
	u64		qwTotalFileSize;				// 文件大小
	u64		qwCreateDate;					// 文件创建时间，采用FILETIME格式
	u64		qwDataPacketsCount;				// 包含的数据包个数
	u64		qwPlayDuration;					// 文件播放的总时间长度(100nano-second)
	u64		qwSendDuration;					// 文件发送的总时间长度(100nano-second)
	u64		qwPreroll;					// 文件起始的偏移时间
	BOOL32		bIsBroadCast;					// 文件/现场流的标识
	u32		dwDataPacketSize;				// 包含数据包的大小
	u32		dwMaximumBitrate;				// 最大比特率 bits per second for entire file.
	u16		wAudioStreamCount;				// 音频流的数目
	u16		wVideoStreamCount;				// 视频流的数目
} TASFFileProperty
#if defined(_VXWORKS_) || defined(_LINUX_)
__attribute__ ( (packed) ) 
#endif
;

//  内容描述属性结构 
typedef struct tagContentDescriptionProperty{
	u16		wTitleLength;					// 内容标题长度
	u16		wAuthorLength;					// 内容作者长度
	u16		wCopyrightLength;				// 内容版权长度
	u16		wDescriptionLength;				// 内容描述长度
	u16*	pwTitle;						// Unicode字符串表示的内容标题
	u16*	pwAuthor;						// Unicode字符串表示的内容作者
	u16*	pwCopyright;					// Unicode字符串表示的内容版权
	u16*	pwDescription;					// Unicode字符串表示的内容描述信息
} TContentDescriptionProperty
#if defined(_VXWORKS_) || defined(_LINUX_)
__attribute__ ( (packed) ) 
#endif
;

// ASF Audio Media Type
typedef struct tagAudioMediaInfo{
	u16		wCodecID_FormatTag;				// 音频Codec标识
	u16		wNumberOfChannels;				// 音频通道数				
	u32		dwSamplesPerSecond;				// 采样率
	u32		dwAverageNumberOfBytesPerSecond;// 平均码率 Bytes / Second
	u16		wBlockAlignment;				// 块对齐字节数 
	u16		wBitsPerSample;					// 每个采样的比特数常为8或者16
	u16		wCodecSpecificDataSize;			// 音频Codec标识
	u8*		pbyCodecSpecificData;			// 音频Codec标识
} TAudioMediaInfo
#if defined(_VXWORKS_) || defined(_LINUX_)
__attribute__ ( (packed) ) 
#endif
;							// 相当于 WAVEFORMATEX 结构

// Spread Audio
typedef struct tagSpreadAudio{
	u8		bySpan;							// 音频数据包的序号跨度
	u16		wVirtualPacketLength;			// 音频数据包的载荷长度
	u16		wVirtualChunkLength;			// 音频数据包的载荷长度
	u16		wSilenceDataLength;				// 静音数据长度
	u8*		pbySilenceData;					// 静音数据
} TSpreadAudio
#if defined(_VXWORKS_) || defined(_LINUX_)
__attribute__ ( (packed) ) 
#endif
;

// ASF Video Media Type
typedef struct tagFormatDataStruct{
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
} TFormatData
#if defined(_VXWORKS_) || defined(_LINUX_)
__attribute__ ( (packed) ) 
#endif
;								// 相当于 BITMAPINFOHEADER 结构

typedef struct tagVideoMediaInfo{
	u32		dwEncodedImageWidth;			// 视频画面宽度
	u32		dwEncodedImageHeight;			// 视频画面宽
	u16		wFormatDataSize;				// 视频属性数据大小
	TFormatData 	tFormatData;			
} TVideoMediaInfo
#if defined(_VXWORKS_) || defined(_LINUX_)
__attribute__ ( (packed) ) 
#endif
;

typedef struct tarOtherMediaInfo{
	u32		dwInfoLength;					// 其它媒体数据长度
	u8		*pbyInfoData;					// 其它媒体数据指针
} TOtherMediaInfo
#if defined(_VXWORKS_) || defined(_LINUX_)
__attribute__ ( (packed) ) 
#endif
;


//  流属性结构 
typedef struct tagStreamProperty{
	u16		wStreamType;					// 流类型
	u8		byStreamNumber;					// 流序号
	u16		wErrorCorrectType;				// 纠错数据类型	
	u32		dwErrorCorrectDataLength;		// 纠错数据类型	
	u8		*pbyErrorCorrectData;			// 纠错数据类型	
	BOOL32	bIsEncryptedContent;			// 是否对内容采用了熵编码
	u32		dwAverageBitrate;				// 平均比特率  bits/s

	union{
		TAudioMediaInfo	 tAudioMediaInfo;   // 音频数据属性
		TVideoMediaInfo	 tVideoMediaInfo;	// 视频数据属性
		TOtherMediaInfo	 tOtherMediaInfo;	// 其它媒体数据属性
	};
} TStreamProperty
#if defined(_VXWORKS_) || defined(_LINUX_)
__attribute__ ( (packed) ) 
#endif
;


// 流对象的属性链表结构
struct TStreamListEntry {		
	TStreamProperty		tStreamProperty;	// 流对象的属性
	TStreamListEntry	*pNextStream;		// 下一个流对象的指针
	u8 					byStreamID;			// 流对象的顺序号
	u8					byAlignedField;		//对齐字节,无意义
	u16					wAlignedField;		//对齐字节,无意义

} 
#if defined(_VXWORKS_) || defined(_LINUX_)
__attribute__ ( (packed) ) 
#endif
;

//  载荷数据
typedef struct tagStreamDataPayload
{
	u8		byStreamNumber;					// 流序号			
	u32		dwMediaObjectNumber;			// 帧序号
	BOOL32	bIsKeyFrame;					// 是否为关键帧
	u32		dwPresentationTime;				// 帧时间信息（时间戳）
	u32		dwPayloadDataSize;				// 数据长度
	u8		*pbyPayloadData;				// 有效数据
	
}TStreamDataPayload
#if defined(_VXWORKS_) || defined(_LINUX_)
__attribute__ ( (packed) ) 
#endif
;

//  标志数据
typedef struct tagMarkers{
	u64		qwOffset;						// 标志在文件中的位置偏移
	u64		qwPresentationTime;				// 标志显示的时间
	u32		dwMarkerDescriptionLength;		// 标志的描述长度
	u8		*pbyMarkerDescription;			// 标志的描述信息
} TMarkerStruct
#if defined(_VXWORKS_) || defined(_LINUX_)
__attribute__ ( (packed) ) 
#endif
;


//  命令脚本数据
typedef struct tagCommands{
	u32		dwPresentationTime;				// 脚本执行的时间
	u16		wCommandsTypeLength;			// 脚本的类型长度
	u16		*pwCommandsType;				// 脚本的类型
	u16		wCommandNameLength;				// 脚本的名称长度
	u16		*pwCommandName;				// 脚本的名称
} TCommandStruct
#if defined(_VXWORKS_) || defined(_LINUX_)
__attribute__ ( (packed) ) 
#endif
;

// 读到帧数据后的回调函数
typedef void (* ASFFrameProcCallBack)(u32 dwUser, u32 dwStatus, u32 dwNextFrameTimeStamp, TStreamDataPayload *piFrameData);

//  流的类型
enum {
		Audio_Media_Type				= 0,
		Video_Media_Type				= 1,
		Command_Media_Type				= 2,
		JFIF_Media_Type					= 3,
		Degradable_JPEG_Media_Type		= 4,
		File_Transfer_Media_Type		= 5,
		Binary_Media_Type				= 6,
		Unknown_Media_Type				= 7
};

typedef u16 ASF_MEDIA_TYPE;


//播放模式，正常，快放，慢放
//05.7.12
enum {
    NORMAL_PLAY         = 0,
    FAST_PLAY           = 1,
    SLOW_PLAY           = 2
};

//播放倍数
enum{
	NORMAL_SPEED		= 1,			//正常速度
	TWICE_SPEED			= 2,			//两倍速
	FOUR_TIMES_SPEED	= 4				//四倍速
};

//读或播放时的状态
enum{
		ASF_Read_Status_Wait			= 0,
		ASF_Read_Status_Playing			= 1,
		ASF_Read_Status_EOF				= 2,
		ASF_Read_Status_Closed			= 3,
		ASF_Read_Status_SINGLE_FRAME	= 4	//单帧调用05.8.11
};
typedef u16	ASF_READ_STATUS;


//  纠错数据类型 
enum {
		No_Error_Correction_Type 		= 0,
		Audio_Spread_Type 				= 1,
		Unknown_Error_Type 				= 2
};
typedef u16 ERROR_CORRECTION_TYPE;


//  函数返回的错误码
#ifndef __ASF_ERROR_CODE__
#define __ASF_ERROR_CODE__
enum
{
		ASF_Error_NoError				= 0,   // 没有错误，正确返回
		ASF_Error_FileNotFound			= 1,   // 文件没有找到
		ASF_Error_InvalidASFFile		= 2,	// 无效的ASF文件
		ASF_Error_ObjectNotFound 		= 3,	// 对象没有找到
		ASF_Error_BadIndex				= 4,	// 错误的对象索引
		ASF_Error_ValueNotFound			= 5,	// 没有找到相应的值
		ASF_Error_BadArgument			= 6,	// 错误的参数
		ASF_Error_ReadOnly				= 7,	// 文件只读错误
		ASF_Error_NotEnoughSpace		= 8,	// 存储空间不足错误
		ASF_Error_InternalError  		= 9,	// 无法申请内存的内部错误
		ASF_Error_ReadFileError  		= 10,	// 读文件内部错误
		ASF_Error_WriteFileError  		= 11,	// 写文件内部错误
		ASF_Error_SeekFileError  		= 12,	// 查找文件错误
		ASF_Error_CreateFileError  		= 13,	// 创建文件错误
		ASF_Error_CreateThreadError		= 14,	// 创建文件错误
		ASF_Error_ObjectNULLError		= 15,	// 参数指针为空的错误
		ASF_Error_TooMuchVideoStrream	= 16,	//
		ASF_Error_NotSupportedCodec		= 17,	//
		ASF_Error_FileEnd				= 18,	//
		ASF_Error_UnknownError			= 19,	// 无法确定的错误		
		ASF_Error_WriteMemError			= 20	// [8/10/2011 liuxu] 写内存错误
};
typedef u16 Error_Code;

static char * sASF_ErrorInfo[] = 
{
	/* 0  */ "No Error, return OK!" ,
	/* 1  */ "Open the file FAILED for ASF file does not exist or the file cannot be open with \"rb\" mode!" ,
	/* 2  */ "The opened file is invalid, it maybe not keep to ASF Specification, or occur some Error when parsing the file !"  ,
	/* 3  */ "Cannot find Object by the given index!"   ,
	/* 4  */ "It is a wrong index of Object!"  ,
	/* 5  */ "Cannot get value by the given index!" ,
	/* 6  */ "The input parameter is illegal,for example pointer is NULL or parameter is not in the range!"  ,
	/* 7  */ "The file is only for reading!" ,
	/* 8  */ "There is not enough space for write or new!" ,
	/* 9  */ "Failed in newing space for pointer!" ,
	/* 10 */ "Read file ERROR for the get bytes is not equal to the one that should get!"  ,
	/* 11 */ "Write file ERROR for the put bytes is not equal to the one that should put!"  ,
	/* 12 */ "Seek file ERROR !" ,
	/* 13 */ "Create file ERROR !" ,
	/* 14 */ "Create thread ERROR!" ,
	/* 15 */ "Input pointer is NULL!" ,
	/* 16 */ "there is too much stream or video stream!",
	/* 17 */ "not supported codec format id, cannot play the asf file!",
	/* 18 */ "to file end!",
	/* 19 */ "There is an unknown Error!" 	
};

#endif  // __ASF_ERROR_CODE__

// 文件对象的ID，供设置调试信息和对象标识
// 标识码由3位数字组成
// 1 × ×  与文件头信息相关的头部对象
// 2 × ×  与数据对象相关的对象 
// 3 × ×  与索引信息相关的对象
// 4 × ×  综合性的对象
// 9 × ×  无效的对象
enum
{
		// ASF TOP-LEVEL HEADER OBJECT
		ASF_ObjectID_Header_Object                       = 100,
		ASF_ObjectID_File_Properties_Object              = 101,
		ASF_ObjectID_Stream_Properties_Object            = 102,
		ASF_ObjectID_Header_Extension_Object             = 103,
		ASF_ObjectID_Codec_List_Object                   = 104,
		ASF_ObjectID_Script_Command_Object               = 105,
		ASF_ObjectID_Marker_Object                       = 106,
		ASF_ObjectID_Bitrate_Mutual_Exclusion_Object     = 107,
		ASF_ObjectID_Error_Correction_Object             = 108,
		ASF_ObjectID_Content_Description_Object          = 109,
		ASF_ObjectID_Extended_Content_Description_Object = 110,
		ASF_ObjectID_Content_Branding_Object             = 111,
		ASF_ObjectID_Stream_Bitrate_Properties_Object    = 112,
		ASF_ObjectID_Content_Encryption_Object           = 113,
		ASF_ObjectID_Extended_Content_Encryption_Object  = 114,
		ASF_ObjectID_Alt_Extended_Content_Encryption_Object = 115,
		ASF_ObjectID_Digital_Signature_Object            = 116,
		ASF_ObjectID_Padding_Object                      = 117, 
		
		// ASF TOP-LEVEL DATA OBJECT
		ASF_ObjectID_Data_Object						= 200,
		ASF_ObjectID_Packet_Object						= 201,        
		ASF_ObjectID_DataErrorCorrection_Object			= 202,
		ASF_ObjectID_DataParseInfomation_Object			= 203,
		ASF_ObjectID_DataPadding_Object					= 204,
		ASF_ObjectID_DataPayload_Object					= 205,
		
		// ASF TOP-LEVEL INDEX OBJECTS       
		ASF_ObjectID_Simple_Index_Object                 = 300,
		ASF_ObjectID_Index_Object                        = 301,
		ASF_ObjectID_Media_Object_Index_Object           = 302,
		ASF_ObjectID_Timecode_Index_Object               = 303,
		
		// ASF ALL OBJECT		
		ASF_ObjectID_ALL_Object							 = 400,
		ASF_ObjectID_ALL_Header_Object					 = 401,
		ASF_ObjectID_ALL_Frame_Object					 = 402,

		// Present Nothing 
		ASF_ObjectID_Nothing_Object						 = 900
		
};
typedef u16 ASFObjectID;


class CKdvASFFile;
/*********************************************************
**********************************************************

定义ASF文件的写接口类

**********************************************************
**********************************************************/


class CKdvASFFileWriter
{
public:
	//
	// Constructors and destructor.
	CKdvASFFileWriter(void);
	~CKdvASFFileWriter(void);

	//  写ASF文件的初始化操作
	u16	Open(IN const char * pFileName);

	//  设置ASF文件的属性信息
	u16	SetASFFileProperty(IN TASFFileProperty * pFileProperty);
	
	//  设置ASF文件的内容版权信息
	u16	SetContentProperty(IN TContentDescriptionProperty * pContentDescriptionProperty);

	//设置是否生成索引对象
	u16  SetGenerateSimpleIndexObject(IN BOOL32 isGenarate = false);
		
	//  将设置好的流添加到文件头中
	u16  AddStream(IN TStreamProperty * pStreamProperty);

	//  开始写ASF文件的数据部分
	u16  BeginWriting(void);

	//  按照不同的流序号写帧数据
	u16  WriteSample(IN u8 byStreamNumber, IN u8 * pDataBuffer, IN u32 dwBufferSize,
					 IN BOOL32 bIsKeyFrame, IN u32 dwTimeStamp, IN u16 wDuration, OUT u32* pdwFileSize = NULL);
	
	//  结束写帧数据
	u16  EndWriting(void);
	
	//  关闭文件，释放资源空间
	u16  Close(void);


	//  设置是否需要在屏幕输出调试信息
	u16  SetDebugInfo(IN u16 wObjectID);
	//  根据函数的返回值获取详细的错误信息
	char    *GetErrorInfo(IN u16 wErrorIndex);
	//  专用于显示二进制数据、将每个字节的值用两个十六进制的数字显示出来
	void	ShowDataWithHex(IN u8 * pData, IN u32 dwDataLength);	
	
protected:

	CKdvASFFile			*m_pASFFile;				//  ASF文件的类变量指针
	
	u16					m_wObjectIDForDebug;		//	表示要调试的目标对象ID
	
};



/********************************************************
*********************************************************

文件定义ASF文件的读接口类

*********************************************************
*********************************************************/


class CKdvASFFileReader
{
public:
	//
	// Constructors and destructor.
	CKdvASFFileReader(void);
	~CKdvASFFileReader(void);

	//  读ASF文件的初始化操作，完成判断是否为ASF文件，并读取ASF文件的头信息
	u16	Open(IN const char * pFileName, IN ASFFrameProcCallBack inCallback = NULL , IN u32 dwContext = NULL);

	//  设置处理帧数据的回调函数
	u16	SetCallback(IN u32 dwUser, IN ASFFrameProcCallBack inCallback);

	//  获取ASF文件所有头对象的个数
	u16	GetHeaderObjectTotal(OUT u32 * dwHeadObjectCount);
	
	//  获取ASF文件的属性信息
	u16	GetASFFileProPerty(OUT TASFFileProperty * pFileProperty);

	//05.7.26得到对应字段的长度
	u16	GetContentDescriptionPropertyFieldLength(OUT u16 *poTitleLength,OUT u16 *poAuthorLength, OUT u16 *poCopyrightLength,OUT u16 *poDescriptionLength);
	
	//  获取ASF文件的内容版权信息,参数需设置好长度，分配好内存
	u16	GetContentDescriptionProperty(IN OUT TContentDescriptionProperty * pioContentDescriptionProperty);

	//  获取ASF文件所有流的个数
	u16	GetStreamTotal(OUT u16 *wStreamCount);
	
	//得到对应字段的长度
	u16 GetStreamPropertyRelativeFieldLenth(IN u8 byinStreamIndex, OUT u16 *poStreamType, OUT u32 *poErrorCorrectDataLength, OUT u16 *poAudVidOthCodecSpecificLength);

	//  根据索引获取ASF文件的每个流的信息,参数需设置好长度，分配好内存
	u16	GetStreamPropertyByIndex(IN u8 byStreamIndex,IN OUT TStreamProperty * pioStreamProperty);
	
	//  获取ASF文件的纠错类型
	u16	GetErrorCorrectionType(OUT u16 *pErrorType);
	//  获取ASF文件的纠错数据长度
	u16	GetErrorCorrectionDataLength(OUT u32 *pErrCorrDataLength);
	//  获取ASF文件的纠错数据
	u16	GetErrorCorrectionData(OUT u8 *pErrCorrData, IN u32 dwDataLength);	
	
	//  设置是否播放音频流
	u16	SetPlayAudioStream(IN u8 byIndex, IN BOOL32 isPlayStream = TRUE);
//	u16	SetPlayAudioStream(IN BOOL32 isPlay = true );
	//  设置是否播放视频流（所有流中至少要播放一个流，设置错误系统有策略强制播放一个流）
	u16	SetPlayVideoStream(IN u8 byIndex, IN BOOL32 isPlayStream = TRUE);
//	u16	SetPlayVideoStream(IN BOOL32 isPlayFirstStream = true, IN BOOL32 isPlaySecondStream = true );//05.7.26

	//设置快放慢放和倍数
    u16 SetPlayRate(IN u16 wPlayMode, IN u32 wPlayRate);

	//根据编解码器ID，得出相应的媒体类型
	u16	CodecFormatId2MediaType(IN u8 byiStreamType,IN u32 dwiCodecFormatID,OUT UnionMediaDetailTypeInfo& uoMediaDetailTypeInfo);

	//  根据输入的起始时间，异步读取ASF文件的帧数据
	//bIsAutoCallBack，是否自动回调，自动回调和单帧回调可切换
	//不管是自动回调还是单帧回调，都必须设置回调函数，必须调用此函数。
	u16	Start(IN u32 dwStartTime = 0, BOOL32 bIsAutoCallBack = true);
	
	//单帧获取函数，没调用一次
	u16	GetNextFrame( u32& dwStatus, TStreamDataPayload **ppoFrameData);

	//  暂停异步读取ASF文件的帧数据
	u16	Pause(void);
	//  由暂停的位置重新异步读取ASF文件的帧数据,或由单帧回调模式返回到自动回调模式
	u16	Resume(void);
	//  停止异步读取ASF文件的帧数据
	u16	Stop(void);
	//  关闭ASF文件读取，并释放资源
	u16	Close(void);	


	//  设置是否需要在屏幕输出调试信息
	u16		SetDebugInfo(IN u16 wObjectID);
	//  输出整个ASF文件的内容对象索引表信息（Table Of Content）
	u16		DumpObjectTOC(void);
	//  根据函数的返回值获取详细的错误信息
	char    *GetErrorInfo(IN u16 wErrorIndex);
	//  专用于显示二进制数据、将每个字节的值用两个十六进制的数字显示出来
	void	ShowDataWithHex(IN u8 * pData, IN u32 dwDataLength);	
	
public:

private:
	CKdvASFFile*	m_pASFFile;				//  ASF文件的类变量指针

	u16				m_wObjectIDForDebug;		//	表示要调试的目标对象ID
	
};


/***************************************************************
****************************************************************

全局函数定义

****************************************************************
****************************************************************/

#ifndef API
#define API extern "C"
#endif


/*==============================================================
 AsfStartUp():	定时器线程创建函数,与AsfCleanUp（）对称调用，在调用读接口之前调用
				如果只用到写接口，这两个函数可以不调用。在调用读接口之前需要调用AsfStartUp(),
				调用AsfStartUp（）之后可以new多个asfreader对象或实例，	
				由于里面定时器的关系,目前在一个进程里面最多支持new 32个asfreader对象,
				asfwriter不受限制。	Delete所有 asfreader之后调用AsfCleanUp()。
 ===============================================================*/
void AsfStartUp();

/*===============================================================
AsfCleanUp(): Asflib的定时器线程退出函数,与AsfStartUp（）对称调用
================================================================*/
void AsfCleanUp();

/*================================================================
AsfWriteCleanUp:在只用到asfwriter的情况下,如果没有调用asfstartup和asfcleanup,
则在退出时要调用这个函数释放内存,否则有内存泄漏.
如果调用了asfstartup和asfcleanup,则不用调用这个函数
=================================================================*/
void AsfWriteCleanUp();

/*================================================================
 asflibversion(): asflib版本信息
=================================================================*/
API void asflibversion();

/*================================================================
 asflibhelp(): asflib帮助函数
=================================================================*/
API void asflibhelp();

/*================================================================
 asffileinfo()：接口打开的文件索引号和文件名信息
=================================================================*/
API void asffileinfo();

/*================================================================
 sfdebuginfo()：asflib实时信息打印
=================================================================*/
API void asfdebuginfo(s32 nIndex, s32 nLevel);

#if __Win32__ || WIN32
#pragma pack(pop)
#endif	//WIN32

#endif  //__ASF_LIB__
