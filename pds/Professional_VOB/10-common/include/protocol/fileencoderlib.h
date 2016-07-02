
/*****************************************************************************
模块名      : FileEncoderLib
文件名      : FileEncoderLib.h
相关文件    : FileEncoderLib.cpp
文件实现功能: FEPlayer Class Define
作者        : 万春雷
版本        : V2.0  Copyright(C) 2003-2005 KDC, All rights reserved.
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2004/01/14  2.0         万春雷      Create
2004/11/02  2.0         万春雷      增加获取文件所包含的媒体类型信息的接口
******************************************************************************/


#ifndef _FILEENCODERLIB_0601_H_
#define _FILEENCODERLIB_0601_H_



//模块版本信息和编译时间 及 依赖的库的版本和编译时间
extern "C" void fileencoderver();


//模块帮助信息 及 依赖的库的帮助信息
extern "C" void fileencoderhelp();



#define  FE_OK                      (u16)0		//FileEncoderLib库操作成功 
#define  FEERR_BASE_CODE            (u16)20000	//错误码基准值

//没有初始化fe库
#define  FEERR_NOT_INIT_LIB         (u16)(FEERR_BASE_CODE + 1)
//内存分配错误
#define  FEERR_OUTOFMEMORY          (u16)(FEERR_BASE_CODE + 2)
//无效的编码器，对象指针为空
#define  FEERR_INVAILD_ENCODER      (u16)(FEERR_BASE_CODE + 3)
//创建同步信号量失败
#define  FEERR_CREATE_SEMAPORE      (u16)(FEERR_BASE_CODE + 4)
//无效的文件  
#define  FEERR_INVAILD_FILE			(u16)(FEERR_BASE_CODE + 5)
//定时器参数不正确    
#define FEERR_TIMER_SET_PARAM       (u16)(FEERR_BASE_CODE + 6)
//定时器注册的线程满
#define FEERR_TIMER_FULL_THREAD     (u16)(FEERR_BASE_CODE + 7)
//播放线程空
#define FEERR_ENCORDE_THREAD_NULL   (u16)(FEERR_BASE_CODE + 8)


#define AUDIO_FILE				    (u8)0//
#define VIDEO_FILE				    (u8)1//

#define MAX_FESND_DEST_NUM		    (u8)5
#define FE_MAX_ENCODER_NUMBER	    (u8)28

#define DEFAULT_FE_REPEAT_NUM       (u8)0    //循环重播
#define DEFAULT_FE_MEDIA_TYPE       (u8)255  //由文件自身信息决定
#define DEFAULT_FE_AUDIO_MODE       (u8)255  //由文件自身信息决定
#define DEFAULT_FE_FRAME_RATE       (u8)0    //由文件自身信息决定

//Frame Header Structure
typedef struct tagFEFrameHdr
{
    u8     m_byMediaType; //媒体类型
    u8    *m_pData;       //数据缓冲
    u32    m_dwPreBufSize;// m_pData缓冲前预留了多少空间，用于加
						  // RTP option的时候偏移指针一般为12+4+12
						  // (FIXED HEADER + Extence option + Extence bit)
    u32    m_dwDataSize;  //m_pData指向的实际缓冲大小缓冲大小
    u8     m_byFrameRate; //发送帧率,用于接收端
    u32    m_dwFrameID;   //帧标识，用于接收端
    u32    m_dwTimeStamp; //时间戳, 用于接收端
    u32    m_dwSSRC;      //同步源, 用于接收端
    union
    {
        struct{
			BOOL32    m_bKeyFrame;    //频帧类型（I or P）
			u16       m_wVideoWidth;  //视频帧宽
			u16       m_wVideoHeight; //视频帧宽
		}m_tVideoParam;
        u8    m_byAudioMode;//音频模式
    };
	
}FEFRAMEHDR,*PFEFRAMEHDR;

//网络参数
typedef struct tagFENetSession
{
	u32   m_dwRTPAddr;	//RTP地址(网络序)
	u16   m_wRTPPort;		//RTP端口(本机序)
	u32   m_dwRTCPAddr;	//RTCP地址(网络序)
	u16   m_wRTCPPort;	//RTCP端口(本机序)
}TFENetSession;

// 状态
typedef enum {
	FE_CLOSE = 0,   //文件编码器已关闭
	FE_OPEN  = 1,   //文件编码器已打开
	FE_START = 2,   //文件编码器播放中。。。
	FE_STOP  = 3,   //文件编码器停止播放，播放位置定位到文件头部
	FE_PAUSE = 4,   //文件编码器暂停，保留当前播放位置
	FE_END   = 5      //文件编码器由于播放完毕或者播放出错，结束播放，播放位置定位到文件头部
}emFERunStatus;

// 文件编码器状态
typedef struct tagFEStatusParam
{
	u32            m_dwFrameNum; //已经播放的数据帧数目
	emFERunStatus  m_emRunStatus;//播放状态，可为运行、暂停、停止、打开、关闭
}TFEStatusParam, *PTFEStatusParam;


//文件编码器文件的 文件源参数
typedef struct tagFEFileInfoParam
{
	s8   *m_pszFileName;
	u32	  m_dwRepeatNum;	//重复播放的次数，DEFAULT_FE_REPEAT_NUM , 循环重播
    u8    m_byMediaType;	//数据帧的媒体类型，DEFAULT_FE_MEDIA_TYPE 由文件决定
	u8    m_byFrameRate;	//视频帧的发送帧率，DEFAULT_FE_FRAME_RATE 由文件决定
	u8    m_byAudioMode;	//音频帧的模式，DEFAULT_FE_AUDIO_MODE 由文件决定
}TFEFileInfoParam;

//文件编码器的 网络发送参数 // - 由库内层完成实际的网络发送，
typedef struct tagFENetSndParam
{
	u8            m_byNum;		//实际地址对数
	TFENetSession m_tLocalNet;	//当地地址对
	TFENetSession m_tRemoteNet[MAX_FESND_DEST_NUM];//远端地址对
}TFENetSndParam;


//Frame Rcv CallBack Function
typedef void ( *PFEFrameCallBack )(PFEFRAMEHDR pFrmHdr, u32 dwContext);

//Notify Info CallBack Function
typedef void ( *PFENotifyCallBack )(PTFEStatusParam ptFEStatusParam, u32 dwContext);



//文件编码器文件的 信息回调参数 // - (包括文件播放状态的回调以及读取到的数据帧的回调)
typedef struct tagFEFileCBParam
{
	u32	              m_dwContext;
	PFEFrameCallBack  m_fpFrameCBFunc;  // - 由库获取文件数据帧，回调给外层处理
    PFENotifyCallBack m_fpNotifyCBFunc; // - 由库获取文件播放状态，回调给外层处理
}TFECBParam;

//文件编码器文件的类型信息
typedef struct tagFileHeaderInfo  
{
	u8   m_byFileType;		  //文件类型（0 - 音频， 1 - 视频）
    u8   m_byMediaType;	      //媒体类型
	u8   m_byRatio;		      //发送帧率(ms)
	u32  m_dwFrameNum;		  //数据总帧数
	u16  m_wVideoWidth;       //视频帧宽
	u16  m_wVideoHeight;      //视频帧高
	u8   m_byAudioMode;       //音频模式
}TFileHeaderInfo;


/*=============================================================================
函数名		 feInit
功能		初始化文件编码库,重载之一， 外部使用
算法实现	：（可选项）
输入参数说明：
返回值说明： 成功返回FE_OK,  失败返回错误码
=============================================================================*/
u16 feInit();


/*=============================================================================
函数名		 feRelease
功能		反化文件编码库,减少引用参数计数,直至释放库，重载之一， 外部使用
算法实现	：（可选项）
输入参数说明：无
返回值说明： 成功返回FE_OK,  失败返回错误码
=============================================================================*/
u16 feRelease();



//----------------------------------------------------------------------

class CFEPlayer;
class CFileEncoder
{
public:
    CFileEncoder();
    ~CFileEncoder();

public:

	//打开文件编码器，并根据指定参数，检测文件的媒体类型，确定文件数据帧的读取频率
	u16 OpenEncoder(TFEFileInfoParam tFEFileInfoParam);
	//关闭文件编码器
	u16 CloseEncoder();

	//获取文件所包含的媒体类型信息
	u16 GetFileInfo(TFileHeaderInfo &tFileInfo);

	//设置 读取到的数据帧的外层通知处理方式：两种方式可同时有效
	//1.可由库内层完成实际的网络发送，参数指针为NULL,则释放原有的套结字,不发送
	//2.可由库获取文件信息帧，回调给外层处理，参数指针为NULL,不回调
	u16 SetFrameDealParam(TFENetSndParam	*ptFENetSndParam, TFECBParam *ptFECBParam);

	//开始文件编码
	u16 StartEncode();
	//停止文件编码
	u16 StopEncode();

	//暂停文件编码
	u16 PauseEncode();
	//继续文件编码
	u16 ContinueEncode();
	
private:
	CFEPlayer *m_pcFEPlayer;	
	void*      m_hSynSem;   //用于对象的单线程操作的同步量
};

//----------------------------------------------------------------------


#endif /////_FILEENCODERLIB_0601_H_
