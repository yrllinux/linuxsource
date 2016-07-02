#ifndef _KDVSIPIF_H_
#define _KDVSIPIF_H_

#include "osp.h"
#include "KdvType.h"

#define HAPPCALL        void*
#define HCALL           void*
#define LPHCALL         HCALL*   //void**
#define LPHAPPCALL      HAPPCALL*  //void**

#define HAPPREGCLIENT   void*
#define HREGCLIENT      void*
#define LPHREGCLIENT    HREGCLIENT*


#ifdef WIN32
#define KDVCALLBACK CALLBACK
#else
#define KDVCALLBACK
#endif

#define VIDEO_RCV_PORT      5000
#define AUDIO_RCV_PORT      5100
#define VIDEO_SND_PORT      7200
#define AUDIO_SND_PORT      7300 
#define DATA_SND_PORT      12000
#define DATA_RCV_PORT      13000


//#define VIDEO_RCV_PORT      6000
//#define AUDIO_RCV_PORT      6100
//#define VIDEO_SND_PORT      8200
//#define AUDIO_SND_PORT      8300 
//#define DATA_SND_PORT      12005
//#define DATA_RCV_PORT      13005






//----------------------------------- Media 相关-----------------------------------
#define SIP_MAX_MEDIA_FORMAT_NUM    20

typedef enum
{
    PCMU  =  0,
    G7231 =  4,
    PCMA  =  8,
    G722  =  9,
    G728  = 15,
    G729  = 18,
    H261  = 31,
    H263  = 34

} ESipPayload;

typedef enum
{
    SipAudio = 0,
    SipVideo,
    SipT120,
    SipMeidaOther = -1
} ESipMediaType;


typedef enum
{
    SipMediaRTP = 0,
    SipMediaTCP
} ESipMediaProtocol;


class CSipMediaFormat
{
public:
    CSipMediaFormat();
    CSipMediaFormat(ESipPayload ePayLoad);

    ESipPayload      GetPayload() { return m_ePayload; };
    s32              GetClockRate() { return m_nClockRate; };
    s32              GetNext() { return m_nNext; };

    ESipMediaType   GetMediaType();

    const s8* GetPayloadName();


    friend class CSipMedia;
    friend class CSipMedias;

private:
    void    SetFormat(ESipPayload ePayload, s32 nClockRate);

    ESipPayload      m_ePayload;
    s32              m_nClockRate;
    s32              m_nNext;
    BOOL             m_bIsUsed;
};


class CSipMedia
{
public:
    CSipMedia();

    ESipMediaType getMediaType(){ return m_eMediaType; };

    friend class CSipMedias;

protected:
    void    SetPort(u16 wPort);
    u16     GetPort();

    void    SetEncrypt(BOOL bIsEncrypt = TRUE);
    BOOL    GetEncrypt();

    void    SetProtocol(ESipMediaProtocol eProtocol);
    
    ESipMediaProtocol GetProtocol();

    //BOOL GetMediaStr();
    
    BOOL    m_bIsEncrypt;
    u16     m_wPort;

    ESipMediaProtocol   m_eProtocol;
    ESipMediaType       m_eMediaType;

    s32     m_nFormat;
};


class CSipAudio : public CSipMedia
{
public:
    CSipAudio();
};


class CSipVideo : public CSipMedia
{
public:
    CSipVideo();
};

class CSipT120 : public CSipMedia
{
public:

};



class CSipMedias
{
public:
    CSipMedias();

    BOOL    SetAudio(u16 wPort, ESipMediaProtocol eProtocol = SipMediaRTP,
                     BOOL bIsEncrypt = FALSE);

    BOOL    SetVideo(u16 wPort, ESipMediaProtocol eProtocol = SipMediaRTP,
                     BOOL bIsEncrypt = FALSE);

    BOOL    GetAudio(u16* pwPort, ESipMediaProtocol* peProtocol = NULL,
                     BOOL* pbIsEncrypt = NULL);

    BOOL    GetVideo(u16* pwPort, ESipMediaProtocol* peProtocol = NULL,
                     BOOL* pbIsEncrypt = NULL);

    BOOL    IsFormatExist(ESipPayload ePayload);

    BOOL    AddFormat(ESipPayload ePayload, s32 nClockRate);
    BOOL    DelFormat(ESipPayload ePayload);
    
    void    GetCommon(CSipMedias& cOtherMedias);
    
    CSipMediaFormat* GetFormat(s32 nFormat);

    CSipMediaFormat* GetAudioFormat();
    CSipMediaFormat* GetVideoFormat();


    //BOOL    AddFormat(CSipMediaFormat* pcFormat);
    //BOOL    DelFormat(CSipMediaFormat* pcFormat);

private:
    BOOL    AddFormatToList(s32* pnFormatHead, s32 nNew);
    BOOL    DelFormatFromList(s32* pnFormatHead, s32 nDelete);

    CSipAudio m_cAudio;
    CSipVideo m_cVideo;
    CSipT120  m_cT120;


    CSipMediaFormat m_acFormat[SIP_MAX_MEDIA_FORMAT_NUM];
};



//--------------------------------- Config --------------------------------------
typedef enum
{
    Sip_Terminal_Mt = 0,
    Sip_Terminal_Proxy
} ESipTerminalType;

class CKdvSipConfig
{
public:
    CKdvSipConfig();
    ~CKdvSipConfig();

    BOOL    SetLocalAddr(const s8* pszLocalAddr, u16 wPort);
    BOOL    GetLoaclAddr(s8* achStrBuff, u32 dwBuffLen, u16* pwPort);

    BOOL    SetProxyAddr(const s8* pszProxyAddr, u16 wPort);
    BOOL    GetProxyAddr(s8* achStrBuff, u32 dwBuffLen, u16* pwPort);

    BOOL    SetRegistrarAddr(const s8* pszRegistrarAddr, u16 wPort);
    BOOL    GetRegistrarAddr(s8* achStrBuff, u32 dwBuffLen, u16* pwPort);

    BOOL    SetUserName(const s8* pszUserName);
    BOOL    GetUserName(s8* achStrBuff, u32 dwBuffLen);

    BOOL    SetAuthInfo(const s8* pszUserName, const s8* pszPassword);

    BOOL    SetTerminalType(ESipTerminalType eTerminalType);
    ESipTerminalType    GetTerminalType();

    s8      m_achLocalAddr[64];         //本端地址
    s8      m_achProxyServer[64];       //Proxy服务器地址
    s8      m_achRegistrarServer[64];   //注册服务器地址

    u16     m_wLocalPort;           //本地端口号
    u16     m_wProxyPort;           //proxy服务器端口号
    u16     m_wRegistrarPort;       //注册服务器端口号
    BOOL    m_bUseProxy;            //是否使用代理服务器

    s8      m_achUserName[32];      //终端用户名

    s8      m_achAuthUser[64];      //认证用户名
    s8      m_achAuthPassword[64];  //认证密码

    ESipTerminalType     m_eTerminalType;   //终端类型  0－MT，1－Proxy server 

//辅助函数，查询函数

//protected:
    u32     m_dwLocalIP;           //本端地址

	s32		m_nMaxCalls;		   //最大呼叫数
	s32		m_nMaxChannels;		   //每个呼叫打开的逻辑通道数
};


typedef struct tagSipSessionInfo
{
    s8      m_achSessionName[64];
    //origin
    s8      m_achOriginSID[64];         //SessionId
    s8      m_achOriginUserName[64];    //User name
    s8      m_achOriginVer[64];          //session Version
    s8      m_achOriginAddress[128];    //Address-url


    u32     m_dwBandWidth;
    u32     m_dwStartTime;
    u32     m_dwEndTime;
    u32     m_dwIP;
    
	tagSipSessionInfo(){ memset(this, 0, sizeof(tagSipSessionInfo));};
} TSipSessionInfo;


typedef struct tagSipSdp
{
    TSipSessionInfo m_tSessionInfo;
    CSipMedias m_cMedias;
} TSipSdp;


//创建新的呼叫
typedef s32 (KDVCALLBACK* NotifyNewCallT)(HCALL hCall, LPHAPPCALL lphaCall);

//有呼叫进来
typedef s32 (KDVCALLBACK* NotifyCallIncomingT)(HAPPCALL haCall, HCALL hCall,
                                               TSipSdp* ptRemoteSipSdp);
//呼叫连接建立
typedef s32 (KDVCALLBACK* NotifyCallConnectedT)(HAPPCALL haCall, HCALL hCall,
												TSipSdp* ptRemoteSipSdp);
//呼叫挂断
typedef s32 (KDVCALLBACK* NotifyCallDropT)(HAPPCALL haCall, HCALL hCall);



typedef enum
{
    REGISTERFAIL = -1,
    REGISTERSUCCESS = 0
} ERegisterResult;


typedef s32 (KDVCALLBACK* NotifyRegisterResultT)(HAPPREGCLIENT haRegClient,
                                                 HREGCLIENT hsRegClient,
                                                 ERegisterResult eResult);

typedef void (KDVCALLBACK* NotifyPictureFastUpdateT)(HAPPCALL haCall, HCALL hsCall); 


typedef s32 (KDVCALLBACK* NotifyRegisterRequestT)(s8* pszRegisterAddr, s8* pszDestAddr);



typedef struct 
{
    NotifyNewCallT		        fpNewCall;          //创建了新的呼叫
    
    NotifyCallIncomingT         fpCallIncoming;     //呼叫进来      
    NotifyCallConnectedT        fpCallConnected;    //呼叫建立
    NotifyCallDropT             fpCallDrop;         //呼叫挂断

    NotifyRegisterResultT       fpRegisterResult;       //注册结果回调

    NotifyPictureFastUpdateT    fpPictureFastUpdate;    //图像快速更新回调（对方请求关键帧）

    NotifyRegisterRequestT      fpRegisterRequest;      //注册请求回调

    //NotifyCallCtrlMsgT       fpNotifyCallMsg;    //呼叫过程中的其他消息
	
    //NotifyDeleteCallT        fpDeleteCall;       //删除呼叫
} SIPEVENT, *LPSIPEVENT;


typedef struct tagSipNodeInfo
{
    s8 achUserName[64];
    s8 achFrom[128];
    BOOL bIsOrigin;
} TSipNodeInfo;

//------------------------------------- Adapter -------------------------------------

//Sip Adapter抽象类
class CKdvSipAdapter
{
public:
    //创建SipAdapter对象
    static CKdvSipAdapter* CreateSipAdapter();

    //删除SipAdapter对象
    static void DestroySipAdapter(CKdvSipAdapter* pcSipAdapter); 

    //初始化
    virtual s32 Initialize(const CKdvSipConfig& cConfig) = NULL;
    
    //关闭
    virtual s32 Close() = NULL;

    //创建新的呼叫
    virtual s32 CreateNewCall(HAPPCALL haCall, LPHCALL lphsCall) = NULL;

    //进行呼叫
    virtual s32 MakeCall(HCALL hCall, const s8* pszTo, const CKdvSipConfig& cConfig) = NULL;
    
    //挂断呼叫
	virtual s32 DropCall(HCALL hCall) = NULL;

    //设置本地SDP
    virtual s32 SetSDP(TSipSdp& tSipSdp) = NULL;

    //设置APP回调
	virtual s32 SetAppCallback(SIPEVENT& tAppEvent) = NULL;

    //进行消息循环
    virtual void CheckMessage(BOOL32 bUseMultiThread) = NULL;

    //注册
    virtual s32 Register(HAPPREGCLIENT haRegClient, LPHREGCLIENT lphsRegClient,
                 const CKdvSipConfig& cKdvConfig, const s8* pszRegServer = NULL) = NULL;
    //取消注册
    virtual s32 UnRegister(HAPPREGCLIENT haRegClient, LPHREGCLIENT lphsRegClient,
                   const CKdvSipConfig& cKdvConfig, const s8* pszRegServer = NULL) = NULL;

    //发送图像快速更新命令
    virtual s32 SendPictureFastUpdate(HCALL hCall) = NULL;

//辅助函数
    //得到对端信息
    virtual s32 GetPeerInfo(HCALL hsCall, TSipNodeInfo* ptPeerInfo) = NULL;

    //打印消息
    //virtual static void AppPrintMessage(IN RvSipMsgHandle hMsg) = NULL;
    
    //打印注册表
    virtual void DispRegTable() = NULL;
    
    //打印Transaction表
    virtual void DispTranscTable() = NULL;

protected://接口虚拟类，不允许示例化
    //构造函数
    CKdvSipAdapter();
    //析构函数
    ~CKdvSipAdapter();
};

//日志级别定义
typedef enum
{
    LOG_EXP	    =       (s32)0,		//异常
    LOG_IMT	    =       (s32)1,		//重要日志
    LOG_DEBUG	=       (s32)2,		//一级调试信息
    LOG_ALL     =       (s32)3      //所有调试信息
};

static s32 m_nLogLevel = 3; 
void AdapterLog(s32 nLevel, s8 * pchFormat,...);


#endif