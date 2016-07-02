#ifndef _KDVMEDIAVODINSTANCE_H_
#define _KDVMEDIAVODINSTANCE_H_

#include "kdvmediavod.h"
#include "asflib.h"
#define  KDVCALLBACK

#define CONFIG_FILE_NAME        "./config.ini"



API s32 vodstart();
API void vodend();

void VodLog(s32 nLevel, s8 * pchFormat,...);

#define LOG_EXP			        (s32)0		//异常
#define LOG_IMT			        (s32)1		//重要日志
#define LOG_DEBUG		        (s32)2		//一级调试信息
#define LOG_ALL                 (s32)3      //所有调试信息 

#define INVALID_PALYID          (u32)-1


#define VOD_STATE_LOGOUT            0
#define VOD_STATE_LOGIN             1
#define VOD_STATE_PLAYSTART         2
#define VOD_STATE_PLAYPAUSE         3
#define VOD_STATE_DISCONECT         4

//调试
OSPEVENT(MEDIAVOD_QUIT, MEDIAVOD_MESSAGE_START + 34);

typedef struct 
{
    u16             m_wVodListenPort;

    u32             m_dwLocalIP;
    u16             m_wSendPort;

	u16				m_wMTUSize; // MTU大小, zgc, 2007-04-02

    s8              m_achRootDir[MEDIAVOD_MAX_PATHFILENAME];
} TVodConfig;


typedef struct 
{
    u32 m_dwNodeId;
    u32 m_dwClientId;
    time_t m_LoginTime;
    
    s8  m_achUserName[MEDIAVOD_MAX_USERNAME];      //用户名
    s8  m_achPassword[MEDIAVOD_MAX_PASSWORD];      //用户密码
} TVodClientInfo;


typedef struct tagVodPlayInfo
{

    s8  m_achFileName[MEDIAVOD_MAX_PATHFILENAME];    //包含路径的文件名
    u8  m_byStart;             //是否立即开始播放
    u16 m_wAudioPort;          //0表示不使用
    u16 m_wVideo1Port;         //0表示不使用
    u16 m_wVideo2Port;         //0表示不使用
    u32 m_dwIP;                //播放的地址

    u8  m_byAudioType;          //音频， 255表示没有该路码流
    u8  m_byVideo1Type;         //视频1，255表示没有该路码流
    u8  m_byVideo2Type;         //视频2，255表示没有该路码流
} TPlayInfo;


extern TVodConfig g_tVodConfig;

class Ckdvmediavodinstance : public CInstance  
{
public:
	u16 GetFileInfo(s8* pszFileName, TVodFileInfoAck* ptVodFileInfoAck, u8 &byChnlNum, u32 &dwSamplesPerSecond);
	u16 AppiontPlay(TVodPlayGotoReq* ptVodPlayGotoReq);
	u16 StartAgain();
	u16 PausePlay();
	u16 StopPlay();
	u16 StartPlay(TVodPlayReq*);
	u16 PlayInfo(TVodPlayInfoAck* tVodPlayInfoAck);
	Ckdvmediavodinstance();
	virtual ~Ckdvmediavodinstance(){};
	void DaemonInstanceEntry(CMessage* const pcMsg, CApp* pcApp);
	//事件处理
	void InstanceEntry(CMessage * const pcMsg);
    void InstanceDump(u32 dwParam);

    void InstClear();

    void OnPowerOn(CMessage * const pcMsg);
    void OnPowerOff(CMessage * const pcMsg);
    void DaemonQuit();

    void OnClientReg(CMessage * const pcMsg, CApp* pcApp);

    //子事件处理
    void OnClientUp(CMessage * const pcMsg);

	void OnDisconnect(CMessage * const pcMsg);

    void OnFileListReq(CMessage * const pcMsg);

    void OnFileInfoReq(CMessage * const pcMsg);

    void OnPlayReq(CMessage* const pcMsg);

    void OnStopReq(CMessage * const pcMsg);

	void OnPauseReq(CMessage * const pcMsg);

    void OnResumeReq(CMessage * const pcMsg);

    void OnPalyGotoReq(CMessage * const pcMsg);

    void OnPlayInfoReq(CMessage * const pcMsg);

    void OnUnRegReq(CMessage * const pcMsg);

    void OnNotifyPlayInfo(CMessage * const pcMsg);
    
    u16 IsAutherUser(s8* pszUserName, s8* pszPassword);
private:
    TVodClientInfo m_tClientInfo;

    //u32 m_dwClientId;   //客户端节点Id
    //u32 m_dwClientNode;//客户端节点号
	//u8  m_achClientUserName[MEDIAVOD_MAX_USERNAME];//客户端用户名
	//u8  m_byState;  //具体状态，比如播放，暂停等
    //u8  m_instanceState;//BUSY OR IDLE
    u16 m_wPlayCheckValue;  //play的检查值（用于回调时核对）

	/* [2013/5/10 zhangli]查看过文件信息后，退出时程序会崩溃，原因是退出时调用了OspQuit，CKdvASFFileReader析构里面有
	OspFreeMem的操作，在系统自动析构CKdvASFFileReader时因为osp已经quite导致崩溃。这里改为CKdvASFFileReader *，退出时
	通过delete m_pcKdvASFFileReader自己控制析构*/
	CKdvASFFileReader *m_pcKdvASFFileReader;
public:
    u32 m_dwPlayID;
    TPlayInfo m_tPlayInfo;
};

typedef zTemplate< Ckdvmediavodinstance, MEDIAVOD_MAX_CLIENT> CMediavodAPP;
extern CMediavodAPP g_cMediaVodApp;	
extern SEMHANDLE g_hSemQuit;


typedef struct
{
    u16 wCheckValue;
    u8  byRate;
} TPlayRateMsg;

s32 MediaVodInit(TVodConfig* ptVodConfig);


BOOL32 IsValidPlayFile(s8* pszFileName);
BOOL32 IsVideoType(u8 byMediaType);
BOOL32 isAsfFile(const s8 *pFileNme);

#endif // _KDVMEDIAVODINSTANCE_H_