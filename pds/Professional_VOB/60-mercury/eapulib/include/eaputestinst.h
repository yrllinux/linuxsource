/*==============================================================================
类名    :  CEapuTestClient
功能    :  EAPU自动测试客户端类
主要接口:  
备注    :  
-------------------------------------------------------------------------------
修改记录:  
日  期     版本          修改人          走读人          修改记录						
==============================================================================*/
#ifdef _LINUX_
#include "nipwrapper.h"
#include "nipwrapperdef.h"
#include "brdwrapperdef.h"
#endif

#ifdef _LINUX12_
#include "brdwrapper.h"
#endif

#include "eapuautotest.h"
#include "kdvmixer_eapu.h"
#include "ummessagestruct.h"
#include "mcustruct.h"
#include "kdvmedianet.h"
#include "mcuver.h"
#define MAX_AUDIO_FRAME_SIZE            (u32)8 * 1024       //接受大小
#define EV_C_BASE                       60000

#define EV_C_CONNECT_TIMER                 EV_C_BASE + 1    // 建链
#define EV_C_REGISTER_TIMER				   EV_C_BASE + 2
#define EV_C_INIT                          EV_C_BASE + 3    // 初始化

#define EV_C_CHANGEAUTOTEST_CMD            EV_C_BASE + 6    // 改变自动测试标志

#define MIN_BITRATE_OUTPUT              (u16)128
#define EAPU_CONNETC_TIMEOUT            (u16)3*1000     // 连接超时值3s
#define EAPU_REGISTER_TIMEOUT           (u16)5*1000     // 注册超时值5s
#define PORT_SNDBIND                    20000

//#define ERR_EAPU_ERRDSPID                  (u16)1002
/*EV_EAPUAUTOTEST_BGN + 1       //外设向服务器进行注册请求。包括用户名和用户密码信息。以便服务器端进行身份验证。
TEST_S_C_LOGIN_ACK      服务器对外设注册请求的确认。
TEST_S_C_LOGIN_NACK     服务器对外设注册请求的拒绝。附带拒绝原因
TEST_S_C_START_REG      服务器要求外设开启测试，并附带要求测试的DSP号（即混音器号）。以及音频格式等信息。
TEST_C_S_START_ACK     外设对服务器的开始测试请求的确认。
TEST_C_S_START_NACK    外设对服务器的开始测试请求的拒绝。附带拒绝原因
TEST_S_C_STOP_REG       服务器要求外设停止当前测试。
TEST_C_S_STOP_ACK       外设对服务器的停止当前测试请求的确认。
TEST_C_S_STOP_NACK     外设对服务器的停止当前测试请求的拒绝。附带拒绝原因*/

//const s8 achServIpAddr[] = "10.1.1.1";
const s8 achServIpAddr[] = "172.16.160.10";

#define MAX_SERIAL_LENGTH	    32
#define MAX_MAC_LENGTH		    32
#define MAX_VERSION_LENGTH	    32
class CDeviceInfo
{
private:
    s8          achSerial[MAX_SERIAL_LENGTH];           //序列号
    s8          achMac[MAX_MAC_LENGTH];                 //mac地址
    s8          achSoftVersion[MAX_VERSION_LENGTH];     //软件版本
    s8          achHardVersion[MAX_VERSION_LENGTH];     //硬件版本
    u32         dwIp;                                   //设备ip
    u32         dwSubMask;                              //子网掩码
public:
    u32 getSubMask()
    {
        return dwSubMask;
    }
	
    void setSubMask(u32 dwMask)
    {
        dwSubMask = dwMask;
    }
	
    void setSerial(s8 * buf)
    {
        if(buf == NULL) return;
        memset(achSerial,0,MAX_SERIAL_LENGTH);
        int length = (strlen(buf) >= MAX_SERIAL_LENGTH -1 ? MAX_SERIAL_LENGTH - 1: strlen(buf));
        memcpy(achSerial,buf,length);
        achSerial[length] = '\0';
    }    
    s8* getSerial()
    {
        return achSerial;
    }
    
    void setIp(u32 IP)
    {
        dwIp = IP;
    }
    
    u32 getIp()
    {
        return dwIp;
    }
	
    s8* getMac()
    {
        return achMac;
    } 
    void setMac(s8* pchbuf)
    {
        if(pchbuf == NULL) return;
        memset(achMac,0,MAX_VERSION_LENGTH);
        int length = (strlen(pchbuf) >= MAX_MAC_LENGTH -1 ? MAX_MAC_LENGTH - 1: strlen(pchbuf));
        memcpy(achMac, pchbuf, length);
    }
	
    void setSoftVersion(s8* buf)
    {
        if(buf == NULL) return;
        memset(achSoftVersion, 0, MAX_VERSION_LENGTH);
        int length = (strlen(buf) >= MAX_VERSION_LENGTH -1 ? MAX_VERSION_LENGTH - 1 : strlen(buf));
        memcpy(achSoftVersion, buf, length);
        achSoftVersion[length] = '\0';
    }
    s8* getSoftVersion()
    {
        return achSoftVersion;
    }
    void setHardVersion(s8 * buf)
    {
        if(buf == NULL) return;
        memset(achHardVersion, 0, MAX_VERSION_LENGTH);
        int length = (strlen(buf) >= MAX_VERSION_LENGTH -1 ? MAX_VERSION_LENGTH - 1 : strlen(buf));
        memcpy(achHardVersion, buf, length);
        achHardVersion[length] = '\0';
    }    
    s8* getHardVersion()
    {
        return achHardVersion;
    }
};

class CEapuMixTestInst;
class CRcvTestCallBack
{
public:
    CEapuMixTestInst* m_pcMixerInst;    //回调的混音器
	u32           m_byChnnlId;      //回调的通道号 
};
class CEapuMixTestInst : public CInstance
{
public:
    CEapuMixTestInst();
    virtual ~CEapuMixTestInst();

	void DaemonInstanceEntry(CMessage* const pMsg, CApp* pcApp );
    void InstanceEntry( CMessage* const pMsg );
	void Init();                             //硬件初始化
    BOOL ConnectServ();												//connect server 
	void ProcStartTestReq(CMessage* const pMsg);
	void ProcStopTestReq(CMessage* const pMsg);
	void RegisterSrv();
	void ProcRegAck( CMessage * const pcMsg ); 
	void ProDisconnect( CMessage * const pcMsg );
	void RestoreDefault(void);
	void DaemonProcChangeAutoTestCmd( CMessage* const pMsg );		//改变生产测试flag
private:
	
    //设置设备信息
	BOOL32 SendMsgToMcu( u16 wEvent, CServMsg const &cServMsg );    //向主备MCU发送信息
    void SetEapuDeviceInfo(CDeviceInfo &cDeviceInfo);
public:
	CKdvMixer m_cMixer;  
	u8               m_byAudType;
	CRcvTestCallBack m_acRcvCB[MAXNUM_MIXER_CHNNL];
	CKdvMediaRcv*    m_pcAudRcv[MAXNUM_MIXER_CHNNL];                             //通道对应媒体接受码流
	//BOOL32           m_bIsChnUsed[MAXNUM_MIXER_CHNNL];                           //该通道是否使用
	CKdvMediaSnd*    m_pcAudSnd[MAXNUM_MIXER_CHNNL + 2 * MAXNUM_MIXER_AUDTYPE]; 
private:
	u32  m_dwEapuTestDstNode;
	u32  m_dwEapuTestDstInst;
	BOOL m_bConnected;
	u32  m_dwMcuRcvIp;
	u8   m_byCurTestDspId;
};
typedef zTemplate< CEapuMixTestInst, 1 > CEapuMixerTestApp;
extern CEapuMixerTestApp g_cEapuMixerTestApp;
