/****************************************************************************
  模块名   ：穿越防火墙&NAT代理Client端                        
  文件名   ：FirewallProxyClientLib.h
  相关文件 ：FirewallProxyClientLib.cpp
  文件功能 ：定义穿越防火墙&NAT
             代理客户端Lib的使用头文件
  作者	   ：Tony
  版本	   ：1.0
----------------------------------------------------------------------------
  修改记录:
  日  期		版本		修改人		走读人    修改内容
  2004/12/09    1.0         
******************************************************************************/
#ifndef FIREWALLPROXYCLIENTLIB_H_
#define FIREWALLPROXYCLIENTLIB_H_

#include "osp.h"

//add by yj for ipv6
//#include "h323adapter.h"
#ifdef RV_42
#include "h323adapter4_2.h"
#else
#include "h323adapter.h"
#endif
//end

//=================================================================================
//              Protocol type
//=================================================================================
#ifndef PROTOCOL_H323STACK
#define PROTOCOL_H323STACK            0
#define PROTOCOL_OSPSTACK             1
#endif

//add by gzj.071030
#define VER_PXYC (const char *)"PxyClient v40.00.01.00.100608(CBB)"

#ifdef WIN32
#define PXY_CLINET_CALLBACK __cdecl
#else
#define PXY_CLINET_CALLBACK
#endif

typedef s32 (PXY_CLINET_CALLBACK* CBPxyClientState)( const BOOL32 bIsPxyClientRunning );



//回调函数指针结构
typedef struct tagPxyCallBackFunc
{
    CBPxyClientState	m_fpPxyClientState;

	tagPxyCallBackFunc()
	{
		m_fpPxyClientState = NULL;

	}
}TPxyCallBackFunc, *LPTPxyCallBackFunc;
typedef struct tagClientCfgStruc
{
public:
	// Client Cfg param.
	u32 m_dwSrvIpAddr;                  // PxyServer的Ip地址
	u16 m_wSrvPort;                     // PxyServer的Osp端口
	u32 m_dwRegisterIpAddr;             // PxyClient的本端地址
    u16 m_dwRegisterIpPort;             // 暂时没有使用
	u16 m_wStartMediaPort;              // Chan端口过NAT起始端口(UDP)
	u16 m_wTerminalNum;                 // 终端最大数目
	u8  m_chClientName[32];             // Client Name
	u16 m_wTelenetPort;                 // PxyClient 的TelenetPort
    u16 m_wRtpPort;                     // RTP端口
    u16 m_wRtcpPort;                    // RTCP端口

    //add 2007.05.30
    s32				      m_nProductIdSize;
	u8				      m_abyProductId[LEN_256];				//产品号
	s32				      m_nVerIdSize;
	u8				      m_abyVersionId[LEN_256];				//版本号
    
public:
	tagClientCfgStruc()
	{
		memset( this, 0, sizeof(tagClientCfgStruc) );
	}
    
    
    void SetClientName(s8 *pszName)
    {
        if (pszName == NULL)
        {
            return;
        }

        u8 byLen = strlen(pszName);
        if (byLen > sizeof(m_chClientName)-1)
        {
            byLen = sizeof(m_chClientName)-1;
        }
        memcpy(m_chClientName, pszName, byLen);
        m_chClientName[byLen] = '\0';
        return;
    }

	//设置产品号
	void SetProductId(u8 *pbyId, s32 nLen)
	{
		if(pbyId == NULL|| nLen <= 0 )
		{
			return ;
		}
		memset( m_abyProductId, 0, sizeof(m_abyProductId) );
		s32 nReal = min(max(LEN_1, nLen), LEN_256);
		m_nProductIdSize = nReal;
        if(nLen >= LEN_1)
		{
			memcpy(m_abyProductId, pbyId, nReal);
		}
	}
	//获取产品号长度
	s32 GetProductIdSize()
	{
		return m_nProductIdSize;
	}
	//获取产品号
	s32 GetProductId(u8 *pbyId, s32 nLen)
	{
		if(pbyId == NULL|| nLen <= 0 )
		{
			return 0;
		}
		s32 nReal = min(nLen, m_nProductIdSize); 
		memcpy(pbyId, m_abyProductId, nReal);
		return nReal;
	}
	//设置版本号	
	void SetVersionId(u8 *pbyId, s32 nLen)
	{
		if(pbyId == NULL|| nLen <= 0 )
		{
			return ;
		}
		memset( m_abyVersionId, 0, sizeof(m_abyVersionId) );
		s32 nReal = min(max(LEN_1, nLen), LEN_256);
		m_nVerIdSize = nReal;
        if(nLen >= LEN_1)
		{
			memcpy(m_abyVersionId, pbyId, nReal);
		}
	}
	//获取版本号长度
	s32 GetVersionIdSize()
	{
		return m_nVerIdSize;
	}
	//获取版本号
	s32 GetVersionId(u8 *pbyId, s32 nLen)
	{
		if(pbyId == NULL|| nLen <= 0 )
		{
			return 0;
		}
		s32 nReal = min(nLen, m_nVerIdSize); 
		memcpy(pbyId, m_abyVersionId, nReal);
		return nReal;
	}
	
}TClientCfgStruc, *PTClientCfgStruc;

// 初始化PxyClient模块
extern u32 InitPxyClient(TClientCfgStruc &gClientCfg, u8 byProtocol );
// 反初始化PxyClient模块
void DestroyPxyClient();

API u32 UpdatePxyClientLocalIP(u32 dwNewLocalIP);


/*====================================================================
函	 数   名: SetPxyClientCallBack
功	      能: 注册应用回调函数
算 法 实  现: 
引用全局变量: 无
输入参数说明: lpCallBackFunc - 回调函数结构指针
返   回   值: 成功 - TRUE
			  失败 - FALSE
====================================================================*/
s32 SetPxyClientCallBack( LPTPxyCallBackFunc lpCallBackFunc);


#endif //FIREWALLPROXYCLIENTLIB_H_
