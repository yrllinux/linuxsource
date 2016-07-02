/*****************************************************************************
    模块名      : EqpAgt
    文件名      : eqpagtipinfo.h
    相关文件    : 
    文件实现功能: EqpAgt IP信息（主要参考commonlib）
    作者        : liaokang
    版本        : V4r7  Copyright(C) 2011 KDC, All rights reserved.
-----------------------------------------------------------------------------
    修改记录:
    日  期      版本        修改人          修改内容
    2012/06/18  4.7         liaokang        创建
******************************************************************************/
#ifndef _EQPAGT_IPINFO_H_
#define _EQPAGT_IPINFO_H_

#include "eqpagtutility.h"

#ifdef WIN32
#include <wtypes.h>
#include <objbase.h>
#include <setupapi.h>
#pragma comment(lib,"setupapi.lib")
#endif

// IP配置文件
#define  IPCFGFILENAME             ( LPCSTR )"ipconfig.ini"

#ifdef _LINUX_
#define  ROUTE_CFGFILE              "/proc/net/route"
#define  NETWORK_CFGFILE            "/etc/sysconfig/network"
#define  NETIF_CFGFILE_DIR          "/etc/sysconfig/network-scripts/"  
#define  RC_LOCAL                   "/etc/rc.d/rc.local"

#define  GW_KEY                     (LPCSTR)"GATEWAY"
#define  IP_KEY                     (LPCSTR)"IPADDR"
#define  HWADDR_KEY                 (LPCSTR)"HWADDR"
#define  DEVICE_KEY                 (LPCSTR)"DEVICE"
#define  NETMAS_KEY                 (LPCSTR)"NETMASK"
#define  TYPE_KEY                   (LPCSTR)"Ethernet"
#define  ONBOOT_KEY                 (LPCSTR)"ONBOOT"
#define  BOOTPR_KEY                 (LPCSTR)"BOOTPROTO"
#endif // _LINUX_



// 网络适配器信息获取
#define EQPAGT_MAX_ADAPTER_DESCRIPTION_LEN      (u32)128 
#define EQPAGT_MAX_ADAPTER_NAME_LEN             (u32)256 
#define EQPAGT_MAX_ADAPTER_ADDRESS_LEN          (u32)8 

#define EQPAGT_MAX_ADAPTER_IP_NUM               (u32)16
#define EQPAGT_MAX_ADAPTER_GW_NUM               (u32)16
#define EQPAGT_MAX_ADAPTER_ROUTE_NUM            (u32)16
#define EQPAGT_MAX_ADAPTER_NUM                  (u32)16
#define EQPAGT_MAX_MULTINET_NUM					(u32)(EQPAGT_MAX_ADAPTER_IP_NUM-2)

#define EQPAGT_MIB_IF_TYPE_OTHER                1
#define EQPAGT_MIB_IF_TYPE_ETHERNET             6
#define EQPAGT_MIB_IF_TYPE_TOKENRING            9
#define EQPAGT_MIB_IF_TYPE_FDDI                 15
#define EQPAGT_MIB_IF_TYPE_PPP                  23
#define EQPAGT_MIB_IF_TYPE_LOOPBACK             24
#define EQPAGT_MIB_IF_TYPE_SLIP                 28

struct TNetParam
{
protected:   
    u32 m_dwIpAddr;		// 网络序
    u32 m_dwIpMask;		// 网络序    
public:
    TNetParam()
    {
        SetNull();
    }
    
public:
    void SetNetParam( u32 dwIp, u32 dwIpMask )
    {
        m_dwIpAddr = htonl(dwIp);
        m_dwIpMask = htonl(dwIpMask); 
    }
    
    void SetNull(void)
    {
        m_dwIpAddr = 0;
        m_dwIpMask = 0;
    }
    
    BOOL32 IsValid() const
    {
        return ( 0 == m_dwIpAddr ) ? FALSE : TRUE ;
    }
    
    // 获取地址参数
    u32  GetIpAddr(void) const { return ntohl(m_dwIpAddr); }
    u32  GetIpMask(void) const{ return ntohl(m_dwIpMask); }
    
    BOOL   operator ==( const TNetParam & tObj ) const                  //判断是否相等
    {
        if ( GetIpAddr() == tObj.GetIpAddr() 
            && GetIpMask() == tObj.GetIpMask() )
        {
            return TRUE;
        }
        return FALSE;
    }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

struct TNetAdaptInfo
{
protected:
    s8  m_achAdapterName[EQPAGT_MAX_ADAPTER_NAME_LEN + 4];
    s8  m_achDescription[EQPAGT_MAX_ADAPTER_DESCRIPTION_LEN + 4];
    u32 m_dwMacAddressLength;
    u8  m_abyMacAddress[EQPAGT_MAX_ADAPTER_ADDRESS_LEN];
    u32 m_dwIndex;
    u32 m_dwType;
	u32 m_dwDefGWIpAddr;	// [3/25/2010 xliang] 该网口默认网关ip地址
    TNetParam m_tCurrentIpAddress;

public:
    LinkArray<TNetParam, EQPAGT_MAX_ADAPTER_IP_NUM> m_tLinkIpAddr;          // 本网卡IP地址
    LinkArray<TNetParam, EQPAGT_MAX_ADAPTER_GW_NUM> m_tLinkDefaultGateway;  // 本网卡默认网关

protected:
    void SetMacAddressLength( u32 dwMacLength ) { m_dwMacAddressLength = htonl( dwMacLength ); }
public:
    void SetDefGWIpAddr(u32 dwDefIpAddr) { m_dwDefGWIpAddr = htonl(dwDefIpAddr); }
	u32  GetDefGWIpAddr() {return ntohl(m_dwDefGWIpAddr); }

public:
    
    TNetAdaptInfo(void) { Clear(); }
    ~TNetAdaptInfo(void) { Clear(); }

    void Clear( void )
    {
        memset( m_achAdapterName, '\0', sizeof(m_achAdapterName) );
        memset( m_achDescription, '\0', sizeof(m_achDescription) );
        memset( m_abyMacAddress, 0, sizeof(m_abyMacAddress) );
        m_dwMacAddressLength = 0;
        m_dwIndex = 0;
        m_dwType = 0;
        
        m_tCurrentIpAddress.SetNull();
        m_tLinkIpAddr.Clear();
        m_tLinkDefaultGateway.Clear();
    }
    void Print( void )
    {
        u32 dwIdx = 0;
        StaticLog("AdapterName: %s\n", GetAdapterName() );
		printf( "AdapterName: %s\n", GetAdapterName() );
        StaticLog("Description: %s\n", GetDescription() );
        StaticLog("MacAddress : ");
		printf("MacAddress : ");
        for ( dwIdx = 0; dwIdx < GetMacAddressLength(); dwIdx++ )
        {
            if ( dwIdx < GetMacAddressLength()-1)
            {
                StaticLog("%02x-", m_abyMacAddress[dwIdx]);
				printf("%02x-", m_abyMacAddress[dwIdx]);
            }
            else
            {
                StaticLog("%02x\n", m_abyMacAddress[dwIdx]);
				printf( "%02x\n", m_abyMacAddress[dwIdx]);
            }
        }
        StaticLog("AdaptIdx: 0x%x\n", GetAdaptIdx() );
		printf("AdaptIdx: 0x%x\n", GetAdaptIdx() );
        StaticLog("AdaptType: %d ", GetAdaptType() );
        switch ( GetAdaptType() )    //适配器类型
        {
        case EQPAGT_MIB_IF_TYPE_OTHER:
            StaticLog( "Other\n");
            break;
        case EQPAGT_MIB_IF_TYPE_ETHERNET:
            StaticLog( "Ethernet\n");
            break;
        case EQPAGT_MIB_IF_TYPE_TOKENRING:
            StaticLog( "Tokenring\n");
            break;
        case EQPAGT_MIB_IF_TYPE_FDDI:
            StaticLog( "FDDI\n");
            break;            
        case EQPAGT_MIB_IF_TYPE_PPP:
            StaticLog( "PPP\n");
            break;
        case EQPAGT_MIB_IF_TYPE_LOOPBACK:
            StaticLog( "LoopBack\n");
            break;
        case EQPAGT_MIB_IF_TYPE_SLIP:
            StaticLog( "Slip\n");
            break;    
        default:
            StaticLog( "Unknow\n");
            break;
        }
        StaticLog( "CurrentIpAddress: Ip - 0x%x, Mask - 0x%x\n", 
            m_tCurrentIpAddress.GetIpAddr(),
            m_tCurrentIpAddress.GetIpMask() );
            for ( dwIdx = 0; dwIdx < m_tLinkIpAddr.Length(); dwIdx++ )
            {
                StaticLog( "NetAddress[%d]: Ip - 0x%x, Mask - 0x%x\n", dwIdx,
                m_tLinkIpAddr.GetValue( dwIdx )->GetIpAddr(), 
                m_tLinkIpAddr.GetValue( dwIdx )->GetIpMask() );

				printf("NetAddress[%d]: Ip - 0x%x, Mask - 0x%x\n", dwIdx,
					m_tLinkIpAddr.GetValue( dwIdx )->GetIpAddr(), 
					m_tLinkIpAddr.GetValue( dwIdx )->GetIpMask() );
            }
            for ( dwIdx = 0; dwIdx < m_tLinkDefaultGateway.Length(); dwIdx++ )
            {
                StaticLog( "DefaultGW[%d]: Ip - 0x%x\n", dwIdx,
                    m_tLinkDefaultGateway.GetValue( dwIdx )->GetIpAddr() );
            }
    }

    // 网络适配器别名操作
    void SetAdapterName( s8 * pchAdapterName )
    {
        if ( NULL != pchAdapterName )
        {
            strncpy( m_achAdapterName, pchAdapterName, sizeof(m_achAdapterName)-4 );
            m_achAdapterName[sizeof(m_achAdapterName)-4] = '\0';
        }
        else
        {
            memset( m_achAdapterName, '\0', sizeof(m_achAdapterName) );
        }
        return;
    }
    s8 * GetAdapterName( void )
    {
        return m_achAdapterName; 
    }

    // 网络适配器MAC地址操作
    void SetMacAddress( u8 *pbyMacAddress, u32 dwLength )
    {
        if ( NULL != pbyMacAddress )
        {
            u8 *pbyMac = pbyMacAddress;
            u32 dwLop = 0;
            for ( dwLop = 0; dwLop < dwLength && dwLop < EQPAGT_MAX_ADAPTER_ADDRESS_LEN; dwLop++ )
            {
                m_abyMacAddress[dwLop] = *pbyMac;
                pbyMac++;
            }
            SetMacAddressLength( dwLop );
        }
        else
        {
            memset( m_abyMacAddress, 0, sizeof(m_abyMacAddress) );
            SetMacAddressLength( 0 );
        }
        return;
    }
    u8 * GetMacAddress( void ) { return m_abyMacAddress; }

    u32  GetMacAddressLength( void ) const { return ntohl(m_dwMacAddressLength); }

    // 网络适配器描述操作
    void SetDescription( s8 * pchDescription )
    {
        if ( NULL != pchDescription )
        {
            strncpy( m_achDescription, pchDescription, sizeof(m_achDescription)-4 );
            m_achDescription[sizeof(m_achDescription)-4] = '\0';
        }
        else
        {
            memset( m_achDescription, '\0', sizeof(m_achDescription) );
        }
        return;
    }
    s8 * GetDescription( void ) { return m_achDescription; }

    // 网络适配器索引号操作
    void SetAdaptIdx( u32 dwIdx ) {m_dwIndex = htonl(dwIdx);}
    u32  GetAdaptIdx( void ) const { return ntohl(m_dwIndex); }

    // 网络适配器类型
    void SetAdaptType( u32 dwType ) { m_dwType = htonl(dwType); }
    u32  GetAdaptType( void ) const { return ntohl(m_dwType); }

    void SetCurretnIpAddr( const TNetParam &tCurIp ) 
    {
        m_tCurrentIpAddress = tCurIp; 
    }
    TNetParam GetCurrentIpAddr(void) const 
    { 
        return m_tCurrentIpAddress; 
    }

    s32 GetIpAddrPos( u32 dwIpAddr )
    {
        u32 dwIpNum = m_tLinkIpAddr.Length();
        for ( u32 dwIdx = 0; dwIdx < dwIpNum; dwIdx++ )
        {
            TNetParam tLinkParam = *(m_tLinkIpAddr.GetValue(dwIdx) );
            if ( tLinkParam.GetIpAddr() == dwIpAddr )
            {
                return dwIdx;
            }
        }
        return -1;
    }

    s32 GetDefaultGWPos( u32 dwGWAddr )
    {
        u32 dwIpNum = m_tLinkDefaultGateway.Length();
        for ( u32 dwIdx = 0; dwIdx < dwGWAddr; dwIdx++ )
        {
            TNetParam tLinkParam = *(m_tLinkDefaultGateway.GetValue(dwIdx) );
            if ( tLinkParam.GetIpAddr() == dwGWAddr )
            {
                return dwIdx;
            }
        }
        return -1;
    }
    
    TNetAdaptInfo & operator = (TNetAdaptInfo tNetAdapt)
    {
        if ( this != &tNetAdapt )
        {
            this->Clear();
            this->SetAdapterName( tNetAdapt.GetAdapterName() );
            this->SetAdaptIdx( tNetAdapt.GetAdaptIdx() );
            this->SetAdaptType( tNetAdapt.GetAdaptType() );
            this->SetCurretnIpAddr( tNetAdapt.GetCurrentIpAddr() );
            this->SetDescription( tNetAdapt.GetDescription() );
            this->SetMacAddress( tNetAdapt.GetMacAddress(), tNetAdapt.GetMacAddressLength() );
			this->SetDefGWIpAddr(tNetAdapt.GetDefGWIpAddr());
            u32 dwLop = 0;
            u32 dwArrayLen = 0;
            // Ip array
            TNetParam *ptNetAddr = NULL;
            dwArrayLen = tNetAdapt.m_tLinkIpAddr.Length();
            for ( dwLop = 0; dwLop < dwArrayLen; dwLop++ )
            {
                ptNetAddr = tNetAdapt.m_tLinkIpAddr.GetValue(dwLop);
                if ( NULL == ptNetAddr )
                {
                    continue;
                }
                if ( !m_tLinkIpAddr.Append( *ptNetAddr ) )
                {
                    break;
                }
            }
            // GW
            TNetParam *ptGW = NULL;
            dwArrayLen = tNetAdapt.m_tLinkDefaultGateway.Length();
            for ( dwLop = 0; dwLop < dwArrayLen; dwLop++ )
            {
                ptGW = tNetAdapt.m_tLinkDefaultGateway.GetValue(dwLop);
                if ( NULL == ptGW )
                {
                    continue;
                }
                if ( !m_tLinkDefaultGateway.Append( *ptGW ) )
                {
                    break;
                }
            }
        }
        return *this;
    }

}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

struct TNetAdaptInfoAll
{
public:
    LinkArray<TNetAdaptInfo, EQPAGT_MAX_ADAPTER_NUM> m_tLinkNetAdapt;
    
public:
    TNetAdaptInfoAll(void) { Clear(); }
    ~TNetAdaptInfoAll(void) { Clear(); }
    
    void Clear(void)
    {
        u32 dwAdaptNum = m_tLinkNetAdapt.Length();
        for ( u32 dwIdx = 0; dwIdx < dwAdaptNum; dwIdx++ )
        {
            TNetAdaptInfo * ptAdaptInfo = m_tLinkNetAdapt.GetValue(dwIdx);
            ptAdaptInfo->Clear();
        }
        m_tLinkNetAdapt.Clear();
    }
    
    TNetAdaptInfoAll & operator = (TNetAdaptInfoAll & tNetAdaptInfoAll)
    {
        if ( this != &tNetAdaptInfoAll )
        {
            this->Clear();
            u32 dwLop = 0;
            u32 dwArrayLen = tNetAdaptInfoAll.m_tLinkNetAdapt.Length();
            TNetAdaptInfo * ptNetAdapt = NULL;
            for ( dwLop = 0; dwLop < dwArrayLen; dwLop++ )
            {
                ptNetAdapt = tNetAdaptInfoAll.m_tLinkNetAdapt.GetValue( dwLop );
                if ( NULL == ptNetAdapt )
                {
                    continue;
                }
                if ( !this->m_tLinkNetAdapt.Append(*ptNetAdapt) )
                {
                    break;
                }
            }
        }
        return *this;
    }
    
    BOOL32 IsValid(void)
    {
        u32 dwAdapterNum = m_tLinkNetAdapt.Length();
        u32 dwIdx = 0;
        TNetAdaptInfo * ptAdapter = NULL;
        for ( dwIdx = 0; dwIdx < dwAdapterNum; dwIdx++ )
        {
            ptAdapter = m_tLinkNetAdapt.GetValue( dwIdx );
            if ( ptAdapter->m_tLinkIpAddr.Length() == 0 )
            {
                return FALSE;
            }
        }
        return TRUE;
    }
    
    // 取和ETHID相等的网卡信息 
    TNetAdaptInfo *GetAdapterByEthId( u32 dwEthId) 
    { 
        TNetAdaptInfo * ptAdapter = NULL; 
        for ( u8 byLoop = 0; byLoop < m_tLinkNetAdapt.Length(); byLoop++) 
        { 
            ptAdapter = m_tLinkNetAdapt.GetValue(byLoop); 
            if ( ptAdapter != NULL && ptAdapter->GetAdaptIdx() == dwEthId) 
            { 
                return ptAdapter; 
            } 
        } 
        return NULL; 
    } 
    
    
    void Print(void)
    {
        u32 dwAdapterNum = m_tLinkNetAdapt.Length();
        u32 dwIdx = 0;
        TNetAdaptInfo * ptAdapter = NULL;
        for ( dwIdx = 0; dwIdx < dwAdapterNum; dwIdx++ )
        {
            ptAdapter = m_tLinkNetAdapt.GetValue( dwIdx );
            ptAdapter->Print();
        }
    }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

struct TEthParam{
    u32 dwIpAdrs;         /*网络字节序*/
    u32 dwIpMask;         /*网络字节序*/
    u8  byMacAdrs[6];
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;
/* 以太网参数结构 */
struct TAllEthParam{
    u32 dwIpNum;/*有效的IP地址数*/
    TEthParam atEthParam[EQPAGT_MAX_ADAPTER_NUM];/*存放IP地址等信息的数组*/
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

// 检验IP地址及掩码是否有效
BOOL32 IsValidIpV4( LPCSTR lpszIPStr );
BOOL32 IsValidIpV4( u32 dwIP);
BOOL32 IsValidIpMask( LPCSTR lpszIpMask );
BOOL32 IsValidIpMask( u32 dwIpMask );

BOOL32 GetNetAdapterInfoActive( TNetAdaptInfoAll * ptNetAdaptInfoAll );
BOOL32 GetNetAdapterInfoAll( TNetAdaptInfoAll * ptNetAdaptInfoAll );
BOOL32 GetNetAdapterInfo( TNetAdaptInfoAll * ptNetAdaptInfoAll, BOOL32 bActive = TRUE );
BOOL32 GetAdapterIp( u32 dwIfIdx, TAllEthParam* ptAllEthParm);

//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<仅win32用 begin>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#ifdef WIN32
// windows下设置/获取Ip, maskIp, Gateway地址
// set操作在WIN7/VISTA/SERVER 2008等上面必须以管理员身份运行程序才能起效。
/*=============================================================================
函 数 名： RegGetIpAdr
功    能： 返回IP字符串，以'\0'分割，以两个连续的'\0'表示结束
算法实现： 
全局变量： 
参    数： s8* lpszAdapterName
s8* pIpAddr
u32 &dwLen [IN/OUT]//IP数组长度
返 回 值： u16 成功返回ERROR_SUCCESS，如果返回ERROR_NOT_ENOUGH_MEMORY，则wlen返回实际需要的u32个数
=============================================================================*/
u32 RegGetIpAdr( LPSTR lpszAdapterName, LPSTR lpszIpAddr, u32 &dwLen );

/*=============================================================================
函 数 名： RegGetIpAdr
功    能： 获得以u32形式表示的IP地址
算法实现： 
全局变量： 
参    数： s8* lpszAdapterName
           u32* pdwIpAddr       ：缓存地址
           u32 &dwLen           ：缓存空间长度
返 回 值： u32 ：成功返回ERROR_SUCCESS，如果返回ERROR_NOT_ENOUGH_MEMORY，则wlen返回实际需要的u32个数
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2009/6/24   4.0			周广程                  创建
=============================================================================*/
u32 RegGetIpAdr( LPSTR lpszAdapterName, u32* pdwIpAddr, u32 &dwLen );

/*=============================================================================
函 数 名： RegGetNetMask
功    能： 
算法实现： 
全局变量： 
参    数： s8* lpszAdapterName
           s8* pNetMask
           u32 &dwLen  [IN/OUT] //NetMask数组长度
返 回 值： u32 成功返回ERROR_SUCCESS，如果返回ERROR_NOT_ENOUGH_MEMORY，则wlen返回实际需要的u32个数 
=============================================================================*/
u32 RegGetNetMask( LPSTR lpszAdapterName, LPSTR lpszNetMask, u32 &dwLen );

/*=============================================================================
函 数 名： RegGetNetMask
功    能： 获得以u32形式表示的掩码
算法实现： 
全局变量： 
参    数： s8* lpszAdapterName
           u32* pdwNetMask
           u32 &dwLen
返 回 值： u16 成功返回ERROR_SUCCESS，如果返回ERROR_NOT_ENOUGH_MEMORY，则dwlen返回实际需要的u32个数 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2009/6/24   4.0			周广程                  创建
=============================================================================*/
u32 RegGetNetMask( LPSTR lpszAdapterName, u32* pdwNetMask, u32 &dwLen );

/*=============================================================================
函 数 名： RegGetNetGate
功    能： 
算法实现： 
全局变量： 
参    数： s8* lpszAdapterName
           s8* pNetGate
           u32 &dwLen
返 回 值： u32 
=============================================================================*/
u32 RegGetNetGate( LPSTR lpszAdapterName, LPSTR lpszNetGate, u32 &dwLen );

/*=============================================================================
函 数 名： RegGetNetGate
功    能： 
算法实现： 
全局变量： 
参    数： s8* lpszAdapterName
           u32* pdwNetGate
           u32 &dwLen
返 回 值： u32 
=============================================================================*/
u32 RegGetNetGate( LPSTR lpszAdapterName, u32* pdwNetGate, u32 &dwLen );

/*=============================================================================
函 数 名： RegSetIpAdr
功    能： 
算法实现： 
全局变量： 
参    数： s8* pIpAddr
u32 dwLen //IP数组长度
返 回 值： u32 
=============================================================================*/
u32 RegSetIpAdr(s8* lpszAdapterName, s8* pIpAddr, u32 dwLen);

/*=============================================================================
函 数 名： RegSetIpAdr
功    能： 
算法实现： 
全局变量： 
参    数： s8* lpszAdapterName
u32 * pdwIpAddr
u32 dwLen
返 回 值： u32 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2009/6/24   4.0			周广程                  创建
=============================================================================*/
u32 RegSetIpAdr(s8* lpszAdapterName, u32 * pdwIpAddr, u32 dwLen);

/*=============================================================================
函 数 名： RegSetNetMask
功    能： 
算法实现： 
全局变量： 
参    数： s8* pNetMask
u32 dwLen   [IN] //NetMask数组长度
返 回 值： u32 
=============================================================================*/
u32 RegSetNetMask(s8* lpszAdapterName, s8* pNetMask, u32 dwLen);

/*=============================================================================
函 数 名： RegSetNetMask
功    能： 
算法实现： 
全局变量： 
参    数： s8* lpszAdapterName
u32* pdwNetMask
u32 dwLen
返 回 值： u32 
=============================================================================*/
u32 RegSetNetMask(s8* lpszAdapterName, u32* pdwNetMask, u32 dwLen);

/*=============================================================================
函 数 名： RegSetNetGate
功    能： 
算法实现： 
全局变量： 
参    数： const s8* lpszAdapterName
s8* pNetGate
u32 dwLen
返 回 值： u32 
=============================================================================*/
u32 RegSetNetGate(const s8* lpszAdapterName, s8* pNetGate, u32 dwLen);

/*=============================================================================
函 数 名： RegSetNetGate
功    能： 
算法实现： 
全局变量： 
参    数： const s8* lpszAdapterName
u32* pdwNetGate
u32 dwLen
返 回 值： u32 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2009/6/24   4.0			周广程                  创建
=============================================================================*/
u32 RegSetNetGate(const s8* lpszAdapterName, u32* pdwNetGate, u32 dwLen);

//网卡操作函数
/*=============================================================================
函 数 名： GetRegistryProperty
功    能： 
算法实现： 
全局变量： 
参    数： HDEVINFO DeviceInfoSet
           PSP_DEVINFO_DATA DeviceInfoData
           ULONG Property
           PVOID Buffer
           PULONG Length
返 回 值： u32 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2009/6/23   4.0			周广程                  创建
=============================================================================*/
u32  GetRegistryProperty( HDEVINFO hDevInfo, PSP_DEVINFO_DATA DeviceInfoData, ULONG Property, PVOID Buffer, PULONG Length );

#endif //WIN32
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<仅win32用   end>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


//<<<<<<<<<<<<<<<<<<<<<<<<仅LINUX平台上使用 begin>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#ifdef _LINUX_

#endif	// LINUX
//<<<<<<<<<<<<<<<<<<<<<<<<仅LINUX平台上使用   end>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


#endif  // _EQPAGT_IPINFO_H_