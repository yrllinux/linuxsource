/*****************************************************************************
模块名      : 高清解码单元
文件名      : hdu2test
相关文件    : 
文件实现功能: hdu2生产测试
作者        : 田志勇
版本        : 4.7  Copyright(C) 2011-2013 KDV, All rights reserved.
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
11/10/31    4.7         田志勇      创建
******************************************************************************/
#ifndef MTHARDWARE_AUTO_TEST_H
#define MTHARDWARE_AUTO_TEST_H

#include "hdu2inst.h"
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
#define CONNECT_TIMER_LENGTH    (3*1000)
#define VERSION_LEN             32
const s8 Host_IpAddr[] = "10.1.1.1";
class CDeviceInfo;
class CHdu2TestClient : public CInstance
{
public:
    CHdu2TestClient();
    virtual ~CHdu2TestClient();
private:

	/* --------- 管理实例消息处理函数 -------------*/
	void  DaemonInstanceEntry(CMessage* const pMsg, CApp* pcApp );    //管理实例消息主入口
	void  DaemonProcPowerOn();                  //上电初始化
	void  DaemonProcConnectTimerOut();          //向MCU建链消息处理
	void  DaemonProcOspDisconnect();            //断链处理处理
    void DaemonRestoreDefault(CMessage* const pMsg);                  //处理恢复默认值请求消息

	/* --------- 普通实例消息处理函数 -------------*/
	void  InstanceEntry( CMessage* const pMsg );                      //普通实例消息主入口
	void  ProcHduChnGrpCreate(CMessage* const pMsg);                  //处理创建通道管理组消息;
	void  ProcStartSwitchReq( CMessage* const pMsg );                 //开始解码及码流接收
	void ProcStopSwitchReq( CMessage* const pMsg );                   //停止解码及码流接收

	/* ---------------- 功能函数 ------------------*/
    void SetHduDeviceInfo(CDeviceInfo &cDeviceInfo);                  //设置设备信息
	BOOL Connect(void);                                               //连接生产测试服务器
	void LoginTestServer();                                        //向生产测试服务器进行注册
	void StatusShow(void);                                            //显示通道信息

private:
	CHdu2ChnMgrGrp  m_cHdu2ChnMgrGrp;
	u32  m_dwHduAutoTestDstNode;
	THduModePort    m_tHduModePort;
	enum EHDU2CHNSTATE
	{
		emIDLE,
		emINIT,
		emREADY,
		emRUNNING,
	};
};

class CDeviceInfo
{
private:
    s8  achSerial[MAX_SERIAL_LENGTH];             //序列号
    s8  achMac[MAX_MAC_LENGTH];                   //mac地址
    s8  m_achSoftVersion[MAX_VERSION_LENGTH];     //软件版本
    s8  m_achHardVersion[MAX_VERSION_LENGTH];     //硬件版本
    u32 m_dwIp;                                   //设备ip
    u32 m_dwSubMask;                              //子网掩码
public:
    void setSubMask(u32 dwIp)
    {
        m_dwSubMask = dwIp;
    }
    void setSerial(s8 * buf)
    {
        if(buf == NULL) return;
        memset(achSerial,0,MAX_SERIAL_LENGTH);
        int length = (strlen(buf) >= MAX_SERIAL_LENGTH -1 ? MAX_SERIAL_LENGTH - 1: strlen(buf));
        memcpy(achSerial,buf,length);
        achSerial[length] = '\0';
    }
    void setIp(u32 IP)
    {
        m_dwIp = IP;
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
        memset(m_achSoftVersion, 0, MAX_VERSION_LENGTH);
        int length = (strlen(buf) >= MAX_VERSION_LENGTH -1 ? MAX_VERSION_LENGTH - 1 : strlen(buf));
        memcpy(m_achSoftVersion, buf, length);
        m_achSoftVersion[length] = '\0';
    }

    void setHardVersion(s8 * buf)
    {
        if(buf == NULL) return;
        memset(m_achHardVersion, 0, MAX_VERSION_LENGTH);
        int length = (strlen(buf) >= MAX_VERSION_LENGTH -1 ? MAX_VERSION_LENGTH - 1 : strlen(buf));
        memcpy(m_achHardVersion, buf, length);
        m_achHardVersion[length] = '\0';
    }    
};
typedef zTemplate<CHdu2TestClient, MAXNUM_HDU_CHANNEL> CHdu2TestClientApp;
extern CHdu2TestClientApp g_cHdu2TestApp;
#endif
//END OF FILE

