 /*****************************************************************************
   模块名      : HardAdapter
   文件名      : KdvAdapter.h
   相关文件    : 
   文件实现功能: 硬件码流适配器上层API 封装
   作者        : 向飞
   版本        : V3.0  Copyright(C) 2001-2002 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人         修改内容
   2003/12/03  3.0         向飞           创建
******************************************************************************/
#ifndef _KDVADAPTER_H_
#define _KDVADAPTER_H_

#include "codeccommon.h"

//宏定义
#define MAX_VIDEO_ADAPT_CHANNUM       1
#define MAX_AUDIO_ADAPT_CHANNUM       3
#define MAX_ADAPT_CHNNUM              (MAX_VIDEO_ADAPT_CHANNUM+MAX_AUDIO_ADAPT_CHANNUM)

#define MAX_TRANS_VIDEO_ADAPT_CHANNUM       20
#define MAX_TRANS_AUDIO_ADAPT_CHANNUM       20

/*图像编码类型
#define  MEDIA_TYPE_MP4	     (u8)97//MPEG-4
#define  MEDIA_TYPE_H261	 (u8)31//H.261
#define  MEDIA_TYPE_H263	 (u8)34//H.263
//语音编码类型
#define	 MEDIA_TYPE_MP3	         (u8)96//mp3 mode 0-4
#define  MEDIA_TYPE_PCMA		 (u8)8//G.711 Alaw  mode 5
#define  MEDIA_TYPE_PCMU		 (u8)0//G.711 ulaw  mode 6
#define	 MEDIA_TYPE_G7231		 (u8)4//G.7231 reservation
#define	 MEDIA_TYPE_G728		 (u8)15//G.728 reservation
#define	 MEDIA_TYPE_G729		 (u8)18//G.729 reservation
*/

//Map 参数
typedef struct
{
	u8    byMapId;   //Map号
    u32   dwCoreSpd; //Map内核速度
    u32   dwMemSpd;  //内存速度
    u32   dwMemSize; //内存大小
}TAdapterMap;

//网络地址
/*typedef struct
{
    u32   dwIp;     //Ip地址(网络序) 
	u16    wPort;    //端口号(网络序) 
}TNetAddress;
*/

/*视频编码参数*/
typedef struct
{
    u8  byRcMode;    /*图像压缩码率控制参数*/
    u8  byMaxKeyFrameInterval;/*I帧间最大P帧数目*/
    u8  byMaxQuant;  /*最大量化参数(2-31)*/
    u8  byMinQuant;  /*最小量化参数(2-31)*/
    u16  wBitRate;    /*编码比特率(Kbps)*/
	u32 dwSndNetBand;/*网络发送带宽(单位:Kbps,1K=1024)*/   
	u8  byFrameRate;  /*帧率(default:25)*/	
	u8  byImageQulity;/*图象压缩质量(1-5)*/
	u16  wVideoWidth;  /*图像宽度(default:640)*/
	u16  wVideoHeight; /*图像高度(default:480)*/
}TAdapterVideoEncParam;

/* 音频编码参数 */
typedef struct
{
	u8  byAudioEncMode; /*声音编码模式*/
	u8  byAudioDuration;/*时长*/
}TAdapterAudioEncParam;

/* 适配器编码参数 */
typedef union
{
//   TAdapterVideoEncParam tVideoEncParam;
	TVideoEncParam          tVideoEncParam;
    TAdapterAudioEncParam   tAudioEncParam;
}TAdapterEncParam;

//适配通道
typedef struct
{
	u8 byChnNo;//通道号
	u8 byMediaType;//目标码流类型(必须指定)
	TAdapterEncParam  tAdapterEncParam; // 编码参数
    TNetAddress tSrcNetAddr;   //通道源地址
	TNetAddress tRtcpBackAddr;
	TNetAddress tDstNetAddr;   //通道目的地址
	TNetAddress tBindAddr;     //捆绑地址
}TAdapterChannel;

//适配组状态
typedef struct
{
    BOOL32     bAdapterStart;      //适配是否开始
	u8     byCurChnNum;  //目前正在适配的通道数目
    TAdapterChannel atChannel[MAX_ADAPT_CHNNUM];  //组中的通道
}TAdapterGroupStatus;

//通道统计信息
typedef struct	
{
	u32  dwRecvBitRate;        //接收码率
    u32  dwRecvPackNum;        //收到的包数
	u32  dwRecvLosePackNum;    //网络接收丢包数
    u32  dwSendBitRate;        //发送码率
	u32  dwSendPackNum;        //发送的包数
	u32  dwSendLosePackNum;    //发送丢包数
	BOOL32 m_bVidCompellingIFrm;	//是否要强制关键帧
}TAdapterChannelStatis;

typedef struct
{
	u32   dwDspNo;
	u32   dwCoreSpd; 
	u32   dwMemSpd;  
	u32   dwMemSize; 
	u32   dwChnNum;
}TAdapterCheckParam;

class CVideoEncoder;
class CAudioEncoder;
class CVideoDecoder;
class CAudioDecoder;

class CKdvAdapterGroup
{
public:
    CKdvAdapterGroup ();
    virtual ~ CKdvAdapterGroup ();

public:
	u16 CreateGroup(const TAdapterMap *ptAdapterMap, u8 byVideoChnNum, u8 byAudioChnNum);// 创建适配组
	u16 StartGroup(void);//开始适配		   
   	u16 StopGroup(void);//停止适配	
    u16 DestroyGroup(void);//退出组

	u16 AddVideoChannel(const TAdapterChannel * ptAdpChannel); //添加图象通道
	u16 RemoveVideoChannel(u8 byChnNo); //删除图象通道
	u16 AddAudioChannel(const TAdapterChannel * ptAdpChannel); //添加声音通道
	u16 RemoveAudioChannel(u8 byChnNo); //删除声音通道

    u16 GetGroupStatus(TAdapterGroupStatus &tAdapterGroupStatus); //得到组的状态
    u16 GetAudioChannelStatis(u8 byChnNo, TAdapterChannelStatis &tAdapterChannelStatis); //得到音频通道的统计信息
	u16 GetVideoChannelStatis(u8 byChnNo, TAdapterChannelStatis &tAdapterChannelStatis); //得到视频通道的统计信息

	u16 ChangeVideoEncParam(u8 byChnNo, TVideoEncParam  * ptVidEncParam);     //改变视频适配编码参数
	u16 ChangeAudioEncParam(u8 byChnNo, u8 byMediaType, TAdapterAudioEncParam * ptAudEncParam);     //改变音频适配编码参数 
	u16 ChangeVideoDecParam(u8 byChnNo, TVideoDecParam  * ptVidDecParam);     //改变视频适配解码参数

	/*设置图像的网络接收重传参数*/
	u16  SetNetRecvFeedbackVideoParam(TNetRSParam tNetRSParam, BOOL32 bRepeatSnd = FALSE);
	/*设置图像的网络发送重传参数*/
	u16  SetNetSendFeedbackVideoParam(u16 wBufTimeSpan, BOOL32 bRepeatSnd = FALSE);
	/*设置声音的网络接收重传参数*/
	u16  SetNetRecvFeedbackAudioParam(TNetRSParam tNetRSParam, BOOL32 bRepeatSnd = FALSE);
	/*设置声音的网络发送重传参数*/
	u16  SetNetSendFeedbackAudioParam(u16 wBufTimeSpan, BOOL32 bRepeatSnd = FALSE);
	
	//设置音频编码载荷PT值
	u16  SetAudEncryptPT(u32 dwChnNo, u8 byEncryptPT);
	//设置音频编码加密key字串以及 加密模式 Aes or Des
	u16  SetAudEncryptKey(u32 dwChnNo, s8 *pszKeyBuf, u16 wKeySize, u8 byEncryptMode );

	//设置音频编码载荷PT值
	u16  SetVidEncryptPT(u32 dwChnNo, u8 byEncryptPT);
	//设置视频编码加密key字串以及 加密模式 Aes or Des
	u16  SetVidEncryptKey(u32 dwChnNo, s8 *pszKeyBuf, u16 wKeySize, u8 byEncryptMode );  
	
	//设置 H.263+/H.264 等动态视频载荷的 Playload值
	u16  SetVideoActivePT(u32 dwChnNo, u8 byRmtActivePT, u8 byRealPT );
	//设置 视频解码key字串 以及 解密模式 Aes or Des
    u16  SetVidDecryptKey(u32 dwChnNo, s8 *pszKeyBuf, u16 wKeySize, u8 byDecryptMode); 
	
	//设置 动态音频载荷的 Playload值
	u16  SetAudioActivePT(u32 dwChnNo, u8 byRmtActivePT, u8 byRealPT );
	//设置 音频解码key字串 以及 解密模式 Aes or Des
    u16  SetAudDecryptKey(u32 dwChnNo, s8 *pszKeyBuf, u16 wKeySize, u8 byDecryptMode); 
	
	//视频是否使用前向纠错
	u16    SetVidFecEnable(u32 dwChnNo, BOOL32 bEnableFec);
	//设置fec的切包长度
	u16    SetVidFecPackLen(u32 dwChnNo, u16 wPackLen);
	//是否帧内fec
	u16    SetVidFecIntraFrame(u32 dwChnNo, BOOL32 bIntraFrame);
	//设置fec算法 FEC_MODE_RAID5
	u16    SetVidFecMode(u32 dwChnNo, u8 byAlgorithm);
	//设置fec的x包数据包 + y包冗余包
	u16    SetVidFecXY(u32 dwChnNo, s32 nDataPackNum, s32 nCrcPackNum);
	

	void  ShowChnInfo(u32 chnNo, BOOL32 bVidChn);
	
	u16 SetFastUpdata();
	u16 VpuInitProc(const TAdapterMap *ptAdapterMap);

protected:
	CVideoEncoder    *m_apVideoEncoder[MAX_VIDEO_ADAPT_CHANNUM];
	CVideoDecoder    *m_apVideoDecoder[MAX_VIDEO_ADAPT_CHANNUM];
	CAudioEncoder    *m_apAudioEncoder[MAX_AUDIO_ADAPT_CHANNUM];
	CAudioDecoder    *m_apAudioDecoder[MAX_AUDIO_ADAPT_CHANNUM];

	CVideoEncoder    *m_apTransVideoEncoder[MAX_TRANS_VIDEO_ADAPT_CHANNUM];
	CVideoDecoder    *m_apTransVideoDecoder[MAX_TRANS_VIDEO_ADAPT_CHANNUM];
	CAudioEncoder    *m_apTransAudioEncoder[MAX_TRANS_AUDIO_ADAPT_CHANNUM];
	CAudioDecoder    *m_apTransAudioDecoder[MAX_TRANS_AUDIO_ADAPT_CHANNUM];

	u32 m_bAdapterStart;
	u32 m_dwMaxVidChnNum;
	u32 m_dwMaxAudChnNum;
	u32 m_dwNoDsp;
	s64 m_qwLastFastupdataTime;

	TAdapterChannel  m_tVidAdtGrpStat[MAX_VIDEO_ADAPT_CHANNUM];
	TAdapterChannel  m_tAudAdtGrpStat[MAX_AUDIO_ADAPT_CHANNUM];

	TAdapterChannel  m_tTransVidAdtGrpStat[MAX_TRANS_VIDEO_ADAPT_CHANNUM];
	TAdapterChannel  m_tTransAudAdtGrpStat[MAX_TRANS_AUDIO_ADAPT_CHANNUM];
};

API void adapterwrapperver(u32 dwMapId);

API void showchn(u32 chnNo, BOOL32 bVidChn);

#ifndef TOSFUNC
#define TOSFUNC
//设置媒体TOS值 nType类型0:全部 1:音频 2:视频 3: 数据
API int SetMediaTOS(int nTOS, int nType);
API int GetMediaTOS(int nType);
#endif

#endif /* _KDVADAPTER_H_ */