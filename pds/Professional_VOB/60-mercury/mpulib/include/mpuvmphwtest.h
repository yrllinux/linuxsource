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

#ifndef MPUVMPHWTEST_H
#define MPUVMPHWTEST_H

#include "mpustruct.h"
#include "eqpcfg.h"
#include "mpuutility.h"
#include "mpuinst.h"
#ifdef _LINUX_
#include "libsswrap.h"

#endif


#define MAXNUM_MPUBAS_CHANNEL        (u8)4

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

/*==============================================================================
类名    :  CMpuVmpTestClient
功能    :  MPU-VMP 自动测试客户端类
主要接口:  
备注    :  
-------------------------------------------------------------------------------
修改记录:  
日  期     版本          修改人          走读人          修改记录
2009-7-16				 薛亮							 create							
==============================================================================*/
class CMpuVmpTestClient :public CMpuSVmpInst
{
public:
    CMpuVmpTestClient();
    virtual ~CMpuVmpTestClient();

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
private:
#ifdef _LINUX_
    //设置设备信息
    void SetMpuDeviceInfo(CDeviceInfo &cDeviceInfo);
#endif	
private:
	u32  m_dwMpuTestDstNode;
	u32  m_dwMpuTestDstInst;
	BOOL m_bConnected;

};

// 创建app类  
typedef zTemplate<CMpuVmpTestClient, 1> CMpuVmpTestClientApp;

extern CMpuVmpTestClientApp g_cMpuVmpTestApp;


/*==============================================================================
类名    :  CMpuBasTestClient
功能    :  MPU-BAS 自动测试客户端类
主要接口:  
备注    :  
-------------------------------------------------------------------------------
修改记录:  
日  期     版本          修改人          走读人          修改记录
2009-7-16				 薛亮								create
==============================================================================*/
class CMpuBasTestClient :public CMpuBasInst
{
public:
    CMpuBasTestClient();
    virtual ~CMpuBasTestClient();

	void DaemonInstanceEntry(CMessage* const pMsg, CApp* pcApp );
    void InstanceEntry( CMessage* const pMsg );

	void DaemonInit( CMessage * const pcMsg );                      //硬件初始化
	void DaemonProcConnectTimeOut(void);							//建链
	void DaemonRegisterSrv();
	void DaemonProcRegAck( CMessage * const pcMsg ); 
	void DaemonProcOspDisconnect( CMessage* const pMsg );
#ifdef _LINUX_	
	void DaemonProcChangeAutoTestCmd( CMessage* const pMsg );		//改变生产测试flag
#endif 	
public:
	BOOL ConnectServ(void);											//connect server 
	void ProcInitBas(void);
	void ProcStartAdpReq(void);
	void RestoreDefault(void);										//恢复生产测试前默认值
	void ChangeTestMode(u8 byMode);									//转模式测试（含重启）
private:
	
    //设置设备信息
#ifdef _LINUX_
    void SetMpuDeviceInfo(CDeviceInfo &cDeviceInfo);
#endif
// private:
// 	u32  m_dwMpuTestDstNode;
// 	u32  m_dwMpuTestDstInst;
// 	BOOL m_bConnected;
	
};

// 创建app类  
typedef zTemplate<CMpuBasTestClient, MAXNUM_MPUBAS_CHANNEL, CMpuBasCfg> CMpuBasTestClientApp;

extern CMpuBasTestClientApp g_cMpuBasTestApp;

#endif

//END OF FILE