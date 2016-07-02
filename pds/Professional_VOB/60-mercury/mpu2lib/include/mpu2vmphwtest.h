 /*****************************************************************************
   模块名      : mpuhardware (vmp/bas) auto test client 
   文件名      : mpuvmphwtest.h, 
   相关文件    : mpuvmphwtest.cpp, mpubashwtest.cpp
   文件实现功能: MPU自动化测试
   作者        : 
   版本        : V4.6.2  Copyright(C) 2009-2015 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   2009/05/27  1.0         薛亮        创建 
******************************************************************************/

#ifndef MPU2VMPHWTEST_H
#define MPU2VMPHWTEST_H

#include "mpu2struct.h"
#include "eqpcfg.h"
#include "mpu2inst.h"
#include "mpu2utility.h"
#ifdef _LINUX_
#include "libsswrap.h"

#endif


#define MAXNUM_MPUBAS_CHANNEL        (u8)4

#define MAX_SERIAL_LENGTH	    32
#define MAX_MAC_LENGTH		    32
#define MAX_VERSION_LENGTH	    32

#define MAX_IPSTRING_LEN           64

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

/*==============================================================================
类名    :  CMpu2VmpTestClient
功能    :  MPU-VMP 自动测试客户端类
主要接口:  
备注    :  
-------------------------------------------------------------------------------
修改记录:  
日  期     版本          修改人          走读人          修改记录
2009-7-16				 薛亮							 create							
==============================================================================*/
class CMpu2VmpTestClient : public CInstance
{
public:
    CMpu2VmpTestClient();
    virtual ~CMpu2VmpTestClient()
	{
		Clear();
	}

	void DaemonInstanceEntry(CMessage* const pMsg, CApp* pcApp );
    void InstanceEntry( CMessage* const pMsg );
	void Init( CMessage * const pcMsg );                             //硬件初始化
	void MsgRegAckProc( CMessage * const pcMsg );                    //确认
	void MsgRegNackProc( CMessage * const pcMsg );                   //拒绝
    BOOL ConnectServ();												//connect server 
	void RegisterSrv();
	void ProcRegAck( CMessage * const pcMsg ); 
	void ProDisconnect( CMessage * const pcMsg );
	void RestoreDefault(void);
	void ChangeTestMode(u8 byMode);									//转模式测试（含重启）
	void DaemonProcChangeAutoTestCmd( CMessage* const pMsg );		//改变生产测试flag
	
	void Clear();
	BOOL32 InitMediaRcv(u8 byChnnl);
	void StartNetRcvSnd();
	void InitMediaSnd( u8 byChnNo );
	BOOL32 SetMediaSndNetParam(u8 byChnNo);
	BOOL32 SetMediaRcvNetParam( u8 byChnNo);
	void MsgFastUpdatePicProc( CMessage * const pMsg );
	void ProcTestLedCmd();           //处理收到服务器发过来的测试灯消息
	void TimerTestLed(CMessage* const pMsg);
	void Proc8KIStartVmpTestReq(CMessage* const pMsg);
	void PrepareToTest();
private:
#ifdef _LINUX_
    //设置设备信息
    void SetMpuDeviceInfo(CDeviceInfo &cDeviceInfo);
#endif	
public:
		CHardMulPic   m_cHardMulPic; 

private:
	u32  m_dwMpuTestDstNode;
	u32  m_dwMpuTestDstInst;
	TEqpBasicCfg       m_tCfg;            // 基本配置

	CKdvMediaRcv* m_pcMediaRcv[MAXNUM_MPU2VMP_MEMBER];   // 25个接受
	CKdvMediaSnd* m_pcMediaSnd[MAXNUM_MPU2VMP_CHANNEL];  // 最多9路发送 
	TMpuVmpCfg    m_tMpuVmpCfg;
	u32           m_dwServerRcvIp;      // Server接收地址
    u16           m_wServerRcvStartPort;// Server接收起始端口号
	BOOL32        m_bAddVmpChannel[MAXNUM_MPU2VMP_MEMBER];// 记录各通道是否已经添加
	u8            m_byHardStyle;// 画面合成风格（底层用的）
	u32           m_adwLastFUPTick[MAXNUM_MPU2VMP_CHANNEL];   // 上一次收到关键帧请求的Tick数
	u8            m_abyLedId[MPU2_LED_NUM]; //mpu2板的灯ID
	u8            m_by8KICurTestCard;        //8kI生产测试当前正在测的板子
};


class CMpu2VmpTestCfg:public CMpu2VmpCfg
{
public:
	CMpu2VmpTestCfg() 
	{
		memset(m_achServerIp,0,sizeof(m_achServerIp));
		memcpy(m_achServerIp,achServIpAddr,min(sizeof(achServIpAddr),sizeof(m_achServerIp)) );
	}
	virtual ~CMpu2VmpTestCfg() {}
	
public:
		s8 m_achServerIp[MAX_IPSTRING_LEN];//保存serverip，可以让用户调用api配置serverip
	
};

// 创建app类  
typedef zTemplate<CMpu2VmpTestClient, 1,CMpu2VmpTestCfg> CMpu2VmpTestClientApp;

extern CMpu2VmpTestClientApp g_cMpuVmpTestApp;

//
///*==============================================================================
//类名    :  CMpuBasTestClient
//功能    :  MPU-BAS 自动测试客户端类
//主要接口:  
//备注    :  
//-------------------------------------------------------------------------------
//修改记录:  
//日  期     版本          修改人          走读人          修改记录
//2009-7-16				 薛亮								create
//==============================================================================*/
//class CMpuBasTestClient :public CMpuBasInst
//{
//public:
//    CMpuBasTestClient();
//    virtual ~CMpuBasTestClient();
//
//	void DaemonInstanceEntry(CMessage* const pMsg, CApp* pcApp );
//    void InstanceEntry( CMessage* const pMsg );
//
//	void DaemonInit( CMessage * const pcMsg );                      //硬件初始化
//	void DaemonProcConnectTimeOut(void);							//建链
//	void DaemonRegisterSrv();
//	void DaemonProcRegAck( CMessage * const pcMsg ); 
//	void DaemonProcOspDisconnect( CMessage* const pMsg );
//#ifdef _LINUX_	
//	void DaemonProcChangeAutoTestCmd( CMessage* const pMsg );		//改变生产测试flag
//#endif 	
//public:
//	BOOL ConnectServ(void);											//connect server 
//	void ProcInitBas(void);
//	void ProcStartAdpReq(void);
//	void RestoreDefault(void);										//恢复生产测试前默认值
//	void ChangeTestMode(u8 byMode);									//转模式测试（含重启）
//private:
//	
//    //设置设备信息
//#ifdef _LINUX_
//    void SetMpuDeviceInfo(CDeviceInfo &cDeviceInfo);
//#endif
//// private:
//// 	u32  m_dwMpuTestDstNode;
//// 	u32  m_dwMpuTestDstInst;
//// 	BOOL m_bConnected;
//	
//};
//
//// 创建app类  
//typedef zTemplate<CMpuBasTestClient, MAXNUM_MPUBAS_CHANNEL, CMpuBasCfg> CMpuBasTestClientApp;
//
//extern CMpuBasTestClientApp g_cMpuBasTestApp;

#endif //MPU2VMPHWTEST_H

//END OF FILE