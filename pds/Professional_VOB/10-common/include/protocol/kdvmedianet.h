/*****************************************************************************
模块名      : KdvMediaNet
文件名      : KdvMediaNet.h
相关文件    : KdvMediaNet.cpp
文件实现功能: KdvMediaSnd KdvMediaRcv类定义
作者        : 魏治兵 胡昌威
版本        : V2.0  Copyright(C) 2003-2005 KDC, All rights reserved.
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2003/06/03  2.0         魏治兵      Create
2003/06/03  2.0         魏治兵      Add RTP Option
******************************************************************************/

#ifndef _KDVMEDIANET_0603_H_
#define _KDVMEDIANET_0603_H_

// #ifndef _VXWORKS_
// #include "fcnet.h"
// #endif


//模块版本信息和编译时间 及 依赖的库的版本和编译时间
API void kdvmedianetver();
API void KdvGetMedianetVer(char** ppVerionString);

//模块帮助信息 及 依赖的库的帮助信息
API void kdvmedianethelp();


#define  MEDIANET_NO_ERROR                  (u16)0//成功返回值  
#define  ERROR_MEDIA_NET_BASE               (u16)16000
#define  ERROR_SND_PARAM                    (ERROR_MEDIA_NET_BASE+1)//设置发送模块参数出错 
#define  ERROR_SND_NOCREATE	                (ERROR_MEDIA_NET_BASE+2)//发送模块没有创建
#define  ERROR_SND_MEMORY	                (ERROR_MEDIA_NET_BASE+3)//发送模块内存操作出错
#define  ERROR_SND_CREATE_SOCK	            (ERROR_MEDIA_NET_BASE+4)//发送模块创建socket
#define  ERROR_RTP_SSRC                     (ERROR_MEDIA_NET_BASE+5)//RTP同步源错误.
#define  ERROR_LOOPBUF_CREATE               (ERROR_MEDIA_NET_BASE+6)//环状缓冲创建错误  
#define  ERROR_RTP_NO_INIT                  (ERROR_MEDIA_NET_BASE+7)//RTP类有些对象没创建
#define  ERROR_RTCP_NO_INIT                 (ERROR_MEDIA_NET_BASE+8)//RTP类有些对象没创建
#define  ERROR_RTCP_SET_TIMER               (ERROR_MEDIA_NET_BASE+9)//RTCP设置定时器出错
#define  ERROR_RTP_SSRC_COLLISION           (ERROR_MEDIA_NET_BASE+10)//RTP 同步源出错
#define  ERROR_SOCK_INIT                    (ERROR_MEDIA_NET_BASE+11)//socket 没有初始化
#define  ERROR_H261_PACK_NUM                (ERROR_MEDIA_NET_BASE+12)//H261的包数不合理
#define  ERROR_PACK_TOO_LEN                 (ERROR_MEDIA_NET_BASE+13)//G.711的数举包太长
#define  ERROR_H263_PACK_NUM                (ERROR_MEDIA_NET_BASE+14)//H263的包数不合理
#define  ERROR_H263_PACK_TOOMUCH            (ERROR_MEDIA_NET_BASE+15)//H263的数据包太长

#define  ERROR_SND_INVALID_SOCK	            (ERROR_MEDIA_NET_BASE+16)//发送模块无效socket
#define  ERROR_SND_SEND_UDP	                (ERROR_MEDIA_NET_BASE+17)//发送模块数据包投递失败（目标可能不可达）
#define  ERROR_SND_FRAME                     (ERROR_MEDIA_NET_BASE+18)//发送模块帧拆包错误

#define  ERROR_NET_RCV_PARAM                (ERROR_MEDIA_NET_BASE+100)//设置接收模块参数出错
#define  ERROR_NET_RCV_NOCREATE             (ERROR_MEDIA_NET_BASE+101)//接收模块没有创建
#define  ERROR_NET_RCV_MEMORY               (ERROR_MEDIA_NET_BASE+102)//接收模块内存操作出错
#define  ERROR_RCV_RTP_CREATE               (ERROR_MEDIA_NET_BASE+103)//接收模块RTP创建失败
#define  ERROR_RCV_RTP_CALLBACK             (ERROR_MEDIA_NET_BASE+104)//接收模块设置RTP回调函数失败
#define  ERROR_RCV_RTP_SETREMOTEADDR        (ERROR_MEDIA_NET_BASE+105)//接收模块设置RTP远端地址失败
#define  ERROR_CREATE_LOOP_BUF              (ERROR_MEDIA_NET_BASE+106)//创建环状缓冲失败
#define  ERROR_RCV_NO_CREATE                (ERROR_MEDIA_NET_BASE+107)//接收模块接收对象没有创建

#define  ERROR_WSA_STARTUP                  (ERROR_MEDIA_NET_BASE+200)//wsastartup error
#define  ERROR_CREATE_SEMAPORE              (ERROR_MEDIA_NET_BASE+201)// create semapore error
#define  ERROR_SOCKET_CALL                  (ERROR_MEDIA_NET_BASE+202)//调用socket() 函数出错
#define  ERROR_BIND_SOCKET                  (ERROR_MEDIA_NET_BASE+203)//socket 绑定出错
#define  ERROR_CREATE_THREAD                (ERROR_MEDIA_NET_BASE+204)//创建线程出错 

#define  ERROR_LOOPBUF_FULL                 (ERROR_MEDIA_NET_BASE+205)//循环缓冲满
#define  ERROR_NULL_POINT                 (ERROR_MEDIA_NET_BASE+206)//空指针

#define  ERROR_SET_DECRYPTKEY               (ERROR_MEDIA_NET_BASE+210)//设置解密key失败
#define  ERROR_DECRYPT_FRAME                (ERROR_MEDIA_NET_BASE+212)//解密一帧失败
#define  ERROR_SET_ENCRYPTKEY               (ERROR_MEDIA_NET_BASE+213)//设置加密key失败
#define  ERROR_ENCRYPT_FRAME                (ERROR_MEDIA_NET_BASE+215)//加密一帧失败
#define  ERROR_SET_USERDATA                 (ERROR_MEDIA_NET_BASE+216)//加密一帧失败
#define  ERROR_SENDER_FEC_CRC	                 (ERROR_MEDIA_NET_BASE+217)//发送fec计算错误
#define  ERROR_RECEIVER_FEC_CRC	                 (ERROR_MEDIA_NET_BASE+218)//接收fec计算错误
#define  ERROR_RECEIVER_FEC_MODE_INVALID         (ERROR_MEDIA_NET_BASE+219)//接收fec模式错误
#define  ERROR_SENDER_FEC_INORDER         (ERROR_MEDIA_NET_BASE+220)//发送fec乱序
#define  ERROR_SENDER_FEC_INVALID_PARAM         (ERROR_MEDIA_NET_BASE+221)//发送fec错误参数

#define  ERROR_NET_BIND_ADDR_SOCKET					(ERROR_MEDIA_NET_BASE + 222) //设置网络参数IP绑定错误
#define  ERROR_NET_BIND_PORT_SOCKET					(ERROR_MEDIA_NET_BASE + 223) //设置网络参数端口绑定错误
#define  ERROR_NET_OTHER_BIND_ERROR					(ERROR_MEDIA_NET_BASE + 224) //设置网络其它绑定错误

#define  ERROR_NET_CREATE_ERROR_SOCKET			(ERROR_MEDIA_NET_BASE + 225) //设置网络参数时其他错误

// define the byte order type [20120816]
#define  BYTE_ORDER_UNKNOWN   (u8)0
#define  BYTE_ORDER_BIG       (u8)1
#define  BYTE_ORDER_LITTLE    (u8)2


#define MIN_PRE_BUF_SIZE                    (s32)28// G.711需再加一字节

#define MAX_H261_HEADER_LEN      (s32)388//
#define MAX_H263_HEADER_LEN      (s32)3076//
#define MAX_H263PLUS_HEADER_LEN  (s32)3076//

#ifndef DES_ENCRYPT_MODE
#define DES_ENCRYPT_MODE         (u8)0      //DES加密模式
#define AES_ENCRYPT_MODE         (u8)1      //AES加密模式
#define ENCRYPT_KEY_SIZE         (u8)32     //密钥长度 取其中的较大值
#define DES_ENCRYPT_KEY_SIZE     (u8)8      //DES密钥长度
#define AES_ENCRYPT_KEY_SIZE_MODE_A (u8)16  //AES Mode-A 密钥长度
#define AES_ENCRYPT_KEY_SIZE_MODE_B (u8)24  //AES Mode-B 密钥长度
#define AES_ENCRYPT_KEY_SIZE_MODE_C (u8)32  //AES Mode-C 密钥长度
#endif

//fec raid5算法
#define FEC_MODE_RAID5	0
//fec raid6算法
#define FEC_MODE_RAID6	1	
//fec raid5 interlace
#define FEC_MODE_RAID5_INTERLEAVE	2
//fec raid6 interlace
#define FEC_MODE_RAID6_INTERLEAVE	3

//Frame Header Structure
typedef struct tagFrameHdr
{
    u8     m_byMediaType; //媒体类型
    u8    *m_pData;       //数据缓冲
    u32    m_dwPreBufSize;//m_pData缓冲前预留了多少空间，用于加
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
                   BOOL32    m_bHighProfile; // H264 HP标识 - 2012/03/01
              }m_tVideoParam;
        u8    m_byAudioMode;//音频模式
    };
	
}FRAMEHDR,*PFRAMEHDR;



#ifndef  MAX_NETSND_DEST_NUM
#define  MAX_NETSND_DEST_NUM   5
#endif

#ifndef TNETSTRUCT//防止地址结构重定义
#define TNETSTRUCT

#define MAX_USERDATA_LEN    16

/*网络参数*/
typedef struct tagNetSession
{
    tagNetSession(){m_dwRTPAddr=0; m_wRTPPort=0; m_dwRTCPAddr=0; m_wRTCPPort=0; \
                    m_dwRtpUserDataLen = 0; m_dwRtcpUserDataLen = 0;}
    
	u32   m_dwRTPAddr;  //RTP地址(网络序)
	u16   m_wRTPPort;   //RTP端口(本机序)
	u32   m_dwRTCPAddr; //RTCP地址(网络序)
	u16   m_wRTCPPort;  //RTCP端口(本机序)
    u32   m_dwRtpUserDataLen;   //Rtp用户数据长度,用于在代理情况下,需要在每个udp头前添加固定头数据的情况
    u8    m_abyRtpUserData[MAX_USERDATA_LEN]; //用户数据指针
    u32   m_dwRtcpUserDataLen;   //Rtcp用户数据长度,用于在代理情况下,需要在每个udp头前添加固定头数据的情况
    u8    m_abyRtcpUserData[MAX_USERDATA_LEN]; //用户数据指针
}TNetSession;

/*本地网络参数*/
/*当本地在pxy后面, 本地地址中的m_wUserLen表示接收时需要去除的头长度*/
typedef struct tagLocalNetParam
{
	TNetSession m_tLocalNet;
	u32         m_dwRtcpBackAddr;//RTCP回发地址(网络序)
	u16         m_wRtcpBackPort; //RTCP回发端口（本机序）
    //u32         m_dwRtcpBackUserDataLen;   //RtcpBack用户数据长度
    //u8          m_abyRtcpBackUserData[MAX_USERDATA_LEN]; //用户数据指针
}TLocalNetParam;

/*网络发送参数*/
typedef struct tagNetSndParam
{
	u8          m_byNum;	//实际地址对数
	TNetSession m_tLocalNet;//当地地址对
	TNetSession m_tRemoteNet[MAX_NETSND_DEST_NUM];//远端地址对
}TNetSndParam;

#endif //end TNETSTRUCT

typedef BOOL32 (*ChangeRate)(u32 dwContext  , u32 dwBand);

typedef struct tagFecParam
{
	BOOL32		m_bUseFec;
	BOOL32		m_bIntraFrame;
	u8			m_nFecMode;
	s32			m_nFecPacklen;
	s32			m_nFecDataPackNum;
	s32			m_nFecCrcPackNum;
}TFecParam;

/*发送模块状态信息*/
typedef struct tagKdvSndStatus
{
	u8           m_byMediaType;   //媒体类型
	u32          m_dwMaxFrameSize;//最大的帧大小
	u32  	     m_dwNetBand;     //发送带宽
	u32          m_dwFrameID;     //数据帧标识
	u8           m_byFrameRate;   //发送频率
	TNetSndParam m_tSendAddr;     //发送地址
	TFecParam	 m_tFecParam;
	BOOL32				m_bReverseG7221c;
}TKdvSndStatus;

/*发送模块统计信息*/
typedef struct tagKdvSndStatistics
{
	u32	m_dwPackSendNum; //已发送的包数
	u32	m_dwFrameNum;	 //已发送的帧数
	u32	m_dwFrameLoseNum;//由于缓冲满等原因造成的发送的丢帧数
}TKdvSndStatistics;

/*发送scoket信息*/
typedef struct tagKdvSndSocketInfo
{
    BOOL32 m_bUseRawSocket;
    u32    m_dwSrcIP;
    u32    m_wPort;
}TKdvSndSocketInfo;

/*接收模块状态信息*/
typedef struct tagKdvRcvStatus
{
	BOOL32	       m_bRcvStart;  //是否开始接收
	u32            m_dwFrameID;  //数据帧ID
	TLocalNetParam m_tRcvAddr;   //接收当地地址
	u32            m_dwFecFrameId;
	BOOL32         m_bReverseG7221c;
	BOOL32         m_bH264DependMark; //H264组帧时是否依赖于MARK标志位
 }TKdvRcvStatus;

/*解码器统计信息*/
typedef struct tagKdvRcvStatistics
{
	u32    m_dwPackNum;       //已接收的包数
	u32    m_dwPackLose;      //丟包数(为网卡上没有收到而导致的丢包数)
	u32    m_dwPackIndexError;//包乱序数
	u32    m_dwFrameNum;      //已接收的帧数
	u32		m_dwPhysicalPackLose;//真正的丢包数
	//	u32    m_dwFrameLoseNum;  //由于数据源切换清空队列等原因造成的接收的丢帧数

	u32 m_dwFecPackNum;//所收到的fec包数,  未启用
	u32 m_dwFecPackLose;//fec丢包总数，包括不可恢复的和可恢复的
	u32 m_dwFecRealFrameNum;//fec的帧数（即所收到的fec组数）
	u32 m_dwFecNeedRecovery;
	u32 m_dwFecRecoSucc;
	u32 m_dwFecRecoFail;
	u32 m_dwFecGroupFail;
}TKdvRcvStatistics;

/*=============================================================================
	函数名		：KdvSocketStartUp
	功能		：kdv socket 初始化,在使用收发类之前需调用此函数。
	算法实现	：（可选项）
	引用全局变量：
	输入参数说明：无
	              

	返回值说明：参见错误码定义
=============================================================================*/
u16 KdvSocketStartup();

/*=============================================================================
	函数名		：KdvSocketCleanUp
	功能		：和KdvSocketStartup相对应的清除函数
	算法实现	：（可选项）
	引用全局变量：无
	输入参数说明：无
	              

	返回值说明：无
=============================================================================*/
u16 KdvSocketCleanup();


/*============================================================================
API BOOL32 KdvSetUdpMaxLen(u32 dwMaxUdpPackLen):设置udp发送的最大包长
dwMaxUdpPackLen:最大包长,其值必须小于等于1450,大于等于1000
因为老版本有每帧最多96包的限制,又有每帧128*1024的限制,如果有128k的帧的话,
那最小udp包长必须比1370大
============================================================================*/
API BOOL32 KdvSetUdpMaxLen(s32 dwMaxUdpPackLen);

API void setUseRawSend(BOOL32 bUsed);

/*============================================================================
开启平滑发送的API
============================================================================*/
API s32 SetSendUseTimer(s32 nMode);


/*=============================================================================
	函数名		：SetH264FrameHPFlag
	功能		：解析H264SPS信息，填入帧头结构的HighProfile标识字段。
	输入参数说明  ： PFRAMEHDR pFrmHdr， 指向H264帧头结构的指针
	返回值说明    ： MEDIANET_NO_ERROR为成功，其它为失败，请参考错误码定义
=============================================================================*/
API u16  SetH264FrameHPFlag( PFRAMEHDR pFrmHdr );



typedef struct tagRtpPack
{
    u8	 m_byMark;			//是否帧边界1表示最后一包
    u8   m_byExtence;		//是否有扩展信息
    u8   m_byPadNum;		//可能的padding长度
    u8   m_byPayload;		//载荷
    u32  m_dwSSRC;			//同步源
    u16  m_wSequence;		//序列号
    u32  m_dwTimeStamp;		//时间戳
    u8  *m_pExData;         //扩展数据
    s32  m_nExSize;			//扩展大小：sizeof(u32)的倍数；
    u8  *m_pRealData;		//媒体数据 
    s32  m_nRealSize;		//数据大小 
    s32  m_nPreBufSize;		//m_pRealData前预分配的空间;
}TRtpPack;

typedef struct tagRSParam
{
    u16  m_wFirstTimeSpan;   //第一个重传检测点
    u16  m_wSecondTimeSpan;  //第二个重传检测点
    u16  m_wThirdTimeSpan;   //第三个重传检测点
    u16  m_wRejectTimeSpan;  //过期丢弃的时间跨度
} TRSParam;

//发送端高级设置参数
typedef struct tagAdvancedSndInfo
{
    s32      m_nMaxSendNum;      // 根据带块计算的最大发送次数;	
    BOOL32   m_bRepeatSend;      // 对于 (mp4) 是否重发
    u16      m_wBufTimeSpan;
    BOOL32   m_bEncryption;      // 对于 (mp4/H.26X/Audio) 是否设置加密
    u8       m_byEncryptMode;    // 加密模式 Aes 或者 Des
    u16      m_wKeySize;         // 加密密钥长度
    s8       m_szKeyBuf[ENCRYPT_KEY_SIZE+1];   // 加密密钥缓冲
    u8       m_byLocalActivePT;  // 动态载荷PT值
    s32      m_nResendTimes;  //重发次数
    s32      m_nResendSpan;   //重发间隔
	BOOL32	 m_bHdFlag;//是否用于高清
	BOOL32	m_bFregmentSlice;//是否切片
	BOOL32  m_bSmoothSnd;//是否启用了平滑发送
	
	u32		m_nRLBUnitNum;      //重传缓存块数
	u32		m_dwPrintFlag;
	u32		m_dwSSRC;
} TAdvancedSndInfo;

//接收端高级设置参数
typedef struct tagAdvancedRcvInfo
{
    BOOL32    m_bConfuedAdjust;  // 对于 (mp3) 是否做乱序调整	
    BOOL32    m_bRepeatSend;     // 对于 (mp4) 是否重发
    TRSParam  m_tRSParam;
    BOOL32    m_bDecryption;     // 对于 (mp4/H.26X/Audio) 是否设置解密
    u8        m_byDecryptMode;   // 解密模式 Aes 或者 Des
    u16       m_wKeySize;        // 解密密钥长度
    s8        m_szKeyBuf[ENCRYPT_KEY_SIZE+1];   // 解密密钥缓冲
    u8        m_byRmtActivePT;   // 接收到的动态载荷的Playload值
    u8        m_byRealPT;        // 该动态载荷实际代表的的Playload类型，同于发送时的PT约定
	u8			m_byLastFrameType;
	BOOL32	 m_bHdFlag;//是否用于高清
	BOOL32   m_bSmoothSend;

	s32		  m_nPacketBuffNum;
	u32		  m_dwBufBlockSize;
	u32		  m_dwBufTotalNum;
	u32			m_dwPrintFlag;
	u32			m_dwSSRC;
} TAdvancedRcvInfo;

//Frame Rcv CallBack Function
typedef   void (*PFRAMEPROC )(PFRAMEHDR pFrmHdr, u32 dwContext);
//RTP PACK Call back
typedef   void (*PRTPCALLBACK)(TRtpPack *pRtpPack, u32 dwContext); 


class CKdvNetSnd;
class CKdvMediaSnd
{
public:
    CKdvMediaSnd();
    ~CKdvMediaSnd();
public:
	//创建发送模块
    u16 Create( u32 dwMaxFrameSize, u32 dwNetBand, 
                u8 ucFrameRate, u8 ucMediaType, u32 dwSSRC = 0);

	//设置是否高清的标志(高清h264帧结构与其他的不一样，在调用create后务必调用这个函数)
	u16	SetHDFlag( BOOL32 bHdFlag );

	//是否切片,当数据加rtp头超过(1456+12)时，是否切片标志，只对h264有效
	u16 SetFregmentSlice( BOOL32 bFregmentSlice );
	
	//设置网络发送参数(进行底层套结子的创建，绑定端口,以及发送目标地址的设定等动作)
    u16 SetNetSndParam( TNetSndParam tNetSndParam , BOOL32 bReuseAddr = FALSE);

	//移除网络发送本地地址参数(进行底层套结子的删除，释放端口等动作)
    u16 RemoveNetSndLocalParam();

	//设置 动态载荷的 Playload值, byLocalActivePT设置为0-表示清空 本端动态载荷标记
    u16 SetActivePT( u8 byLocalActivePT );

	//设置加密key字串及加密码流的动态载荷PT值, pszKeyBuf设置为NULL-表示不加密
    u16 SetEncryptKey( s8 *pszKeyBuf, u16 wKeySize, u8 byEncryptMode=DES_ENCRYPT_MODE );	

	//重置帧ID
    u16 ResetFrameId();
	//重置同步源SSRC
    u16 ResetSSRC(u32 dwSSRC = 0);

	//重置发送端重传处理的开关,关闭后，将不对已经发送的数据包进行缓存
    u16 ResetRSFlag(u16 wBufTimeSpan, BOOL32 bRepeatSnd = FALSE);

	//设置发送选项
    u16 SetSndInfo(u32 dwNetBand, u8 ucFrameRate);

	//重置发送选项
	u16 ResetSndInfo(void);

	//发送数据包
    u16 Send ( PFRAMEHDR pFrmHdr, BOOL32 bAvalid=TRUE );
	//用于转发H261或H263数据包
    u16 Send (TRtpPack *pRtpPack, BOOL32 bTrans = TRUE, BOOL32 bAvalid = TRUE);
 
	//得到状态
    u16 GetStatus ( TKdvSndStatus &tKdvSndStatus );
	//得到统计
    u16 GetStatistics ( TKdvSndStatistics &tKdvSndStatistics );	
	//得到发送端高级设置参数（重传等）
    u16 GetAdvancedSndInfo(TAdvancedSndInfo &tAdvancedSndInfo);
	
	//发送自测试
    u16 SelfTestSend (int nFrameLen, int nSndNum, int nSpan);

	//rtcp定时rtcp包上报, 内部使用，外部请勿调用
    u16 DealRtcpTimer();

    //设置源地址，用于IP伪装，只能在使用raw socket的时候才有效
    u16 SetSrcAddr(TNetSession tSrcNet);

    //得到socket相关的信息
    u16 GetSndSocketInfo(TKdvSndSocketInfo &tRtpSockInfo, TKdvSndSocketInfo &tRtcpSockInfo);

    u16 GetNetSndParam(TNetSndParam* ptNetSndParam);

    //设置音频重发nSendTimes：重发次数，nSendSpan：每次重发间隔，需要把发送重传打开
    u16 SetAudioResend(s32 nSendTimes, s32 nSendSpan);
	
	//调用顺序，可以只调用SetFecEnable，则默认raid5, 5+1, 切包988字节
	//SetFecMode,SetFecIntraFrame,SetFecXY应该在SetFecEnable之前调用,
	//如果在SetFecEnable之后调用，则这三个函数的任何一个都会导致前面ssrc的重置，后果是丢帧丢包
	//SetFecEnable函数的参数的true和false的每次切换都会导致丢帧丢包
	//SetFecPackLen可随时调用	
	//使能fec 发送
	u16 SetFecEnable( BOOL32 bEnableFec );
	//设置fec的切包长度
	u16 SetFecPackLen( u16 wPackLen );
	//是否帧内fec
	u16 SetFecIntraFrame( BOOL32 bIntraFrame );
	//设置fec算法 FEC_MODE_RAID5
	u16 SetFecMode( u8 byAlgorithm );
	//设置fec的x包数据包 + y包冗余包
	u16 SetFecXY(s32 nDataPackNum, s32 nCrcPackNum);

	u16 SetFecParam(const TFecParam& tPara);

	//设置反转g7221c
	u16 SetReverseG7221c( BOOL32 bReverse );

	// 设置字节序 [20120816]
	u16 SetCurByteOrderH261( u8 byOrder );

	//打印信息
	void SetPrintFlag(u32 dwFlag);
	void AddPrintFlag(u32 dwFlag);
	void SaveSendData(u32 dwFrameNum);
	u16	RsFecSndStart(u32 dwContext = 0 , ChangeRate pFunc = NULL, u8 byDataNum = 20);
	u16 RsFecSndStop();

	void RsReSetPacketNum(int nNewPacketNum);
public:

	u16 SendRtcpStatisReq(u8* pBuf, u16 wLen, u32 dwTargetIp, u16 wTargetPort);	

private:
    CKdvNetSnd *m_pcNetSnd;
    void*       m_hSndSynSem;  //用于对象的单线程操作的同步量
	TNetSndParam	m_tNetSndParam;
};

class CKdvNetRcv;
class CKdvMediaRcv
{
public:
    CKdvMediaRcv();
    ~CKdvMediaRcv();
public:
	//创建接收模块
    u16 Create( u32 dwMaxFrameSize, 
                PFRAMEPROC pFrameCallBackProc, 
                u32 dwContext,
                u32 dwSSRC = 0);
    u16 Create( u32 dwMaxFrameSize,
                PRTPCALLBACK PRtpCallBackProc,
                u32 dwContext,
                u32 dwSSRC = 0);

	//设置是否高清的标志(高清h264回调帧的格式与其他不同，调用create后务必调用这个函数)	
	/*
	 *	帧结构，最大512个nal，每2字节代表nal的长度，这2字节为主机序，当长度为0时代表nal结束
	 *	在实际的数据之前预留2*512=1024字节
	 *	---------------------------------------------------------------------------
	 *	| Len0 | Len1 | Len2 | ... | Len511 | NALU0 | NALU1 | NALU2 | ... | NALUx |
	 *	---------------------------------------------------------------------------  
	 *	framehdr结构的m_pData指向Len0处，m_dwDataSize包含前面的1024字节
	 */
	u16	SetHDFlag( BOOL32 bHdFlag );
	
	//设置接收地址参数(进行底层套结子的创建，绑定端口等动作)
    u16 SetNetRcvLocalParam ( TLocalNetParam tLocalNetParam , BOOL32 bReuseAddr = FALSE);
	
	//移除接收地址参数(进行底层套结子的删除，释放端口等动作)
    u16 RemoveNetRcvLocalParam();

	//设置 动态载荷的 Playload值, byRmtActivePT设置为0-表示清空 本端动态载荷标记
    u16 SetActivePT( u8 byRmtActivePT, u8 byRealPT );

	//重置接收端对于mpeg4或者H.264采用的重传处理的开关,关闭后，将不发送重传请求
    u16 ResetRSFlag(TRSParam tRSParam, BOOL32 bRepeatSnd = FALSE);

	//重置接收端对于 (mp3) 是否采用乱序调整处理的开关, 关闭后，将不调整
    u16 ResetCAFlag(BOOL32 bConfuedAdjust = TRUE);
	
	//重置RTP回调接口信息
    u16 ResetRtpCalllback(PRTPCALLBACK pRtpCallBackProc, u32 dwContext);

	//设置接收解密key字串, pszKeyBuf设置为NULL-表示不加密
    u16 SetDecryptKey(s8 *pszKeyBuf, u16 wKeySize, u8 byDecryptMode=DES_ENCRYPT_MODE);
		
	//开始接收
    u16 StartRcv();
	//停止接收
    u16 StopRcv();
	//得到状态
    u16 GetStatus ( TKdvRcvStatus &tKdvRcvStatus );
	//得到统计
    u16 GetStatistics ( TKdvRcvStatistics &tKdvRcvStatistics );
	//得到接收端高级设置参数（重传、乱序重排等）
    u16 GetAdvancedRcvInfo(TAdvancedRcvInfo &tAdvancedRcvInfo);
	//得到当前接收到的最大帧尺寸
    u16 GetMaxFrameSize(u32 &dwMaxFrameSize);
	//得到当前接收到的媒体类型
    u16 GetLocalMediaType(u8 &byMediaType);
	
	//rtcp定时rtcp包上报, 内部使用，外部请勿调用
    u16 DealRtcpTimer();

    u16 RelayRtpStart(u32 dwIP, u16 wPort);
    u16 RelayRtpStop();

	//重传过nat时，设置本机的rtp接收端口对应的公网地址,目的为使重传时不用广播
	//u32 dwRtpPublicIp:本地接收rtp对应的公网ip
	//u16 wRtpPublicPort:本地接收rtp对应的公网port
	u16 SetRtpPublicAddr(u32 dwRtpPublicIp, u16 wRtpPublicPort);

	//设置反转g7221c
	u16 SetReverseG7221c( BOOL32 bReverse );
	//记录帧信息，测试用
	u16 SaveSendData(u32 dwFrameNum);

    //设置H264组帧时是否依赖于MARK标志位, FALSE表示不依赖MARK, TRUE表示依赖, 默认值为TRUE.
    u16 SetRcvH264DependInMark( BOOL32 bH264DependMark );


	//打印信息
	void SetPrintFlag(u32 dwFlag);
	void AddPrintFlag(u32 dwFlag);
	u16	RsFecRcvStart();
	u16 RsFecRcvStop();
	void RsSetRepeatAndFecOn(BOOL32 bRepeat , BOOL32 bFecOn);
private:
    CKdvNetRcv *m_pcNetRcv;
    void*       m_hRcvSynSem;  //用于对象的单线程操作的同步量
};


//设置媒体TOS值 nType类型0:全部 1:音频 2:视频 3: 数据
API s32 kdvSetMediaTOS(s32 nTOS, s32 nType);
API s32 kdvGetMediaTOS(s32 nType);
API BOOL32 Rs_Fec_Test_1();
// 
// #ifndef _VXWORKS_
// 
// /*流控相关*/
// /*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// 注：
// 流控涉及的所有ip 均为网络序
// port为主机序	
// 所有码率:单位bit/s
// 所有时间单位:millisecond
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
// 
// /*===============================
// typedef void (*NETBDNOTICECB)(u32 dwIp, u32 dwBandWidth, u32 dwContext);
// //流控动态带宽通知回调
// u32 dwip: 目的ip，标识通道组
// u32 dwBd：通道组的建议带宽
// u32 dwContext：用户数据
// ================================*/
// typedef void (*NETBDNOTICECB)(u32 dwIp, u32 dwBandWidth, u32 dwContext);
// 
// /*===============================
// typedef void(*NETBDDETECTCB)(u32 dwTargetIp, u32 dwBandWidth, u32 dwContext);
// //带宽检测通知回调
// u32 dwip: 目的ip，标识通道组
// u32 dwBd：通道组的建议带宽
// u32 dwContext：用户数据
// ================================*/
// typedef void(*NETBDDETECTCB)(u32 dwTargetIp, u32 dwBandWidth, u32 dwContext);
// 
// /*==================================
// u16 KdvInitFcNet(u16 wCtrlPort, u16 wStartDataPort, u16 wDataPortNum, u32 dwTotalGrpNum=0 );
// //初始化
// //必须在创建mediasnd之前调用
// 将根据指定起始端口号和端口数，在后续可能的检测发起中选取指定范围内可用的端口进行检测流量接收(数据)；
// u16 wCtrlPort:控制信令监听port，此为tcp连接;带宽检测请求侦听端口
// u16 wStartDataPort：检测的数据port,此为udp;带宽检测数据接收端口可用范围起始值
// u16 wDataPortNum：同时可能有多个检测，此为连续可用的数据port数;带宽检测数据接收端口额外可用数量，0表示只有
//                 起始值可用，1表示可用端口范围为[start_data_port, start_data_port+1],依此类推
// u16 wInnerStartPort:模块内部使用的端口可用范围起始值
// u16 wInnerPortNum:模块内部使用端口额外可用数量，0表示只有起始值可用，
// 						1表示可用端口范围为[start_inner_port, start_inner_port+1]，依此类推
// u32 dwTotalGrpNum:通道组数，=0则取默认值:INTERFCNET_DEFAULT_CHANNEL_NUM;
// ====================================*/
// u16 KdvInitFcNet(u16 wCtrlPort, u16 wStartDataPort, u16 wDataPortNum, u16 wInnerStartPort, u16 wInnerPortNum, u32 dwTotalGrpNum=0 );
// u16 KdvQuitFcNet();
// 
// /*=================================
// u16 KdvRegChnlGrp(u32 dwChnlGrpIp, NETBDNOTICECB pNetBdNoticeC, u32 dwContext, u32 dwFcTimeInterval=0);
// //按指定时间间隔对去往指定对端的通道组启动/重置自动流量控制
// u32 dwIp：目的ip
// u32 dwFcTimeInterval：流控时间间隔,=0则取默认值INTERFCNET_DEFAULT_TIMEOUT_MILLIS;
// 以毫秒计，合理取值范围为<100, 600000>，为较精确地跟踪码流实际收发情况，
// 该值建议设置范围为<500, 5000>
// NETBDNOTICECB pNetBdNoticeC：带宽通知回调函数
// u32 dwContext：用户数据
// ==================================*/
// u16 KdvRegChnlGrp(u32 dwChnlGrpIp, NETBDNOTICECB pNetBdNoticeCb, u32 dwContext, u32 dwFcTimeInterval=0);
// //关闭对去往指定对端的通道组的流量控制
// u16 KdvUnregChnlGrp(u32 dwChnlGrpIp);
// 
// 
// /*========================================================
// u16 KdvDcbDetect(u32 dwTargetIp, u32 dwInitBandWidth, u32 dwContext, NETBDDETECTCB pNetBdDetectCb);
// //检测从本端到指定地址的全程可用带宽
// u32 dwTargetIp:目标ip
// u16 wPort:对端检测侦听端口
// u32 dwInitBandWidth:检测时初始建议带宽,作用为有利于减少检测次数
// u32 dwContext:用于数据
// NETBDDETECTCB pNetBdDetectCb:带宽检测通知回调
// =========================================================*/
// u16 KdvDcbDetect(u32 dwTargetIp, u16 wPort, u32 dwInitBandWidth, u32 dwContext, NETBDDETECTCB pNetBdDetectCb);
// 
// #endif	//




#ifndef _VXWORKS_
//此处的函数全部为空函数，仅为了终端能够编译通过；
//如果想实现这些功能请使用头文件#include "fcnet.h"中对应的函数 

typedef void (*NETBDNOTICECB)(u32 dwIp, u32 dwBandWidth, u32 dwContext);
typedef void(*NETBDDETECTCB)(u32 dwTargetIp, u32 dwBandWidth, u32 dwContext);

u16 KdvInitFcNet(u16 wCtrlPort, u16 wStartDataPort, u16 wDataPortNum, u16 wInnerStartPort, u16 wInnerPortNum, u32 dwTotalGrpNum=0 );
u16 KdvQuitFcNet();
u16 KdvRegChnlGrp(u32 dwChnlGrpIp, NETBDNOTICECB pNetBdNoticeCb, u32 dwContext, u32 dwFcTimeInterval=0);
u16 KdvUnregChnlGrp(u32 dwChnlGrpIp);
u16 KdvDcbDetect(u32 dwTargetIp, u16 wPort, u32 dwInitBandWidth, u32 dwContext, NETBDDETECTCB pNetBdDetectCb);

#endif	//


#endif	//_KDVMEDIANET_0603_H_



