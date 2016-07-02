 /*****************************************************************************
   模块名      : hduhardwate auto test client
   文件名      : hduhwautotest.h
   相关文件    : hduhwautotest.cpp
   文件实现功能: HDU硬件自动化测试
   作者        : 
   版本        : V4.6.2  Copyright(C) 2009-2015 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   2009/04/21  1.0         江乐斌        创建 
******************************************************************************/

#ifndef MTHARDWARE_AUTO_TEST_H
#define MTHARDWARE_AUTO_TEST_H

#include "hduinst.h"
#include "osp.h"
#include "kdvtype.h"

#ifdef _LINUX_ 


#include "nipwrapper.h"
#include "nipwrapperdef.h"
#include "brdwrapperdef.h"
#endif

#ifdef _LINUX12_
#include "brdwrapper.h"
#endif

#define MAX_SERIAL_LENGTH	    32
#define MAX_MAC_LENGTH		    32
#define MAX_VERSION_LENGTH	    32


class CDeviceInfo;

class CHduAutoTestClient : public CHduInstance
{
public:
    CHduAutoTestClient();
    virtual ~CHduAutoTestClient();

	void DaemonInstanceEntry(CMessage* const pMsg, CApp* pcApp );
    void InstanceEntry( CMessage* const pMsg );
	// 初始化
	void DaemonInit();
    // 建链
    void DaemonProcConnectServerCmd();
    // 注册回应ack
	void DaemonProcRegAckRsp();
	// 注册回应nack
	void DaemonProcRegNackRsp();
    // 断链处理
	void DaemonDisconnect();
    // 开始交换
	void DaemonProcStartSwitchReq( CMessage* const pMsg );
	// 停止交换
	void DaemonProcStopSwitchReq( CMessage* const pMsg );
	// 处理恢复默认值请求消息
#ifndef WIN32
    void DaemonRestoreDefault(CMessage* const pMsg);
	// 改变自动测试标志
	void DaemonProcChangeAutoTestCmd( CMessage* const pMsg );
#endif
    // 开始交换
	void ProcStartSwitchReq( CMessage* const pMsg );
	// 停止交换
	void ProcStopSwitchReq();
	// 初始化hdu通道
	void ProcInitChnl();

 
private:

    //设置设备信息
    void SetHduDeviceInfo(CDeviceInfo &cDeviceInfo);
	BOOL ConnectAndLoginTestServer(void);

private:
	u32  m_dwHduAutoTestDstNode;
	u32  m_dwHduAutoTestDstInst;
	BOOL m_bConnected;

};

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

    void setSubMask(u32 dwIp)
    {
        dwSubMask = dwIp;
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

// 创建app类  
typedef zTemplate<CHduAutoTestClient, MAXNUM_HDU_CHANNEL> CHduAutoTestClientApp;

extern CHduAutoTestClientApp g_cHduAutoTestApp;

//autotest 调试接口
void AutoTestLog(s8 *, ...);

extern BOOL b_gPAutoTestMsg;

#endif

//END OF FILE

