/*****************************************************************************
   模块名      : KDV system
   文件名      : kdvmcugk.h
   相关文件    : 
   文件实现功能: KDVRAS信令相关结构
   作者        : 
   版本        : V0.9  Copyright(C) 2002-2003 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   2003/05/06  1.0         zhangsh        
******************************************************************************/
#include "mcustruct.h"
#include "gkmmcuerr.h"
#include "mcuconst.h"
#include "evmcugk.h"
//#include "gkconst.h"
///////////////////////////////////////////
//userage: 
//	this header file is depended on the header file listed
//		osp.h
//		mcustruct.h
//			
///////////////////////////////////////////

#ifndef _KDV_RAS_H
#define _KDV_RAS_H

#ifdef WIN32
#pragma pack( push )
#pragma pack ( 1 )
#endif

///////////////////////////////////////////
//常量定义
///////////////////////////////////////////
const u16 MAX_REG_ALIAS_BYTELEN = 512;	//最大的别名长度，512个字节
const u8  MAX_REG_CONFNUM = 65; //可注册的最大别名数,MCU别名一个,64个会议.
//
// class CKDVAlias
// 封装协议栈定义的cmAlias结构,由于我们自定了类型,请参考mcustruct.h定义的类型
//
// 由于使用的是固定长度的char数组，使用时注意alias长度不能超过
// MAX_REG_ALIAS_BYTELEN
//
// 
//
class CKDVAlias
{
public:
	CKDVAlias()
	{
		Reset();
	}

	////////////////////////////////////////
	// 清空
	void Reset()
	{
 		memset(&m_alias.string,0,sizeof(m_alias.string));
		memset(m_aliasString,0,sizeof(m_aliasString));
		m_alias.string = m_aliasString;
		m_alias.length = 0;
	}

	//////////////////////////////////////////
	// 设置Alias(除cmAliasTypeTransportAddress和cmAliasTypePartyNumber之外)
	// 返回值：成功true, 否则false
	BOOL SetAlias(const char * buf, int len, cmAliasType type)
		{
		assert(type!=cmAliasTypeTransportAddress && type!=cmAliasTypePartyNumber);
		if ( len > MAX_REG_ALIAS_BYTELEN )
			return FALSE;

		Reset();
		m_alias.type = type;
		memcpy(m_aliasString, buf, len);
		m_alias.length = len;

		return TRUE;
		}

	//////////////////////////////////////////
	// 设置cmAliasTypeTransportAddress的Alias
	BOOL SetAlias(const cmTransportAddress & tpaddr)
		{
		if ( tpaddr.type != cmTransportTypeIP )
			return FALSE;

		Reset();
		m_alias.type = cmAliasTypeTransportAddress;
		m_alias.transport.type = tpaddr.type;
		m_alias.transport.ip = tpaddr.ip;
		m_alias.transport.port = tpaddr.port;
		m_alias.transport.distribution = tpaddr.distribution;

		return TRUE;
		}

	//////////////////////////////////////////
	// 设置cmAliasTypeTransportAddress的Alias
	// @ 只能是cmTransportTypeIP类型的transportaddress
	// unsigned long ip : ip
	// unsigned short port : port
	BOOL SetAlias(unsigned long ip, unsigned short port, cmDistribution dsb)
		{
		Reset();
		m_alias.type = cmAliasTypeTransportAddress;
		m_alias.transport.type = cmTransportTypeIP;
		m_alias.transport.ip = (UINT32)ip;
		m_alias.transport.port = (UINT16)port;
		m_alias.transport.distribution = dsb;

		return TRUE;
		}

	///////////////////////////////////////////
	// 设置cmAliasTypeTransportAddress的Alias
	// @ 只能是cmTransportTypeIP类型的transportaddress
	// const sockaddr_in & addr : IP address
	BOOL SetAlias(const sockaddr_in & addr, cmDistribution dsb)
		{
		Reset();
		m_alias.type = cmAliasTypeTransportAddress;
		m_alias.transport.type = cmTransportTypeIP;
		m_alias.transport.ip = (UINT32)addr.sin_addr.s_addr;
		m_alias.transport.port = (UINT16)addr.sin_port;
		m_alias.transport.distribution = dsb;

		return TRUE;
		}

	///////////////////////////////////////////
	// 设置cmAliasTypePartyNumber的Alias
	// const char * buf : buffer
	// int len : 长度
	// cmPartyNumberType type : type
	BOOL SetAlias(const char * buf, int len, cmPartyNumberType type)
		{
		if ( len > MAX_REG_ALIAS_BYTELEN )
			return FALSE;

		Reset();
		m_alias.type = cmAliasTypePartyNumber;
		m_alias.pnType = type;
		memcpy(m_aliasString, buf, len);
		m_alias.length = len;

		return TRUE;
		}
	
	///////////////////////////////////////
	// 测试是否有alias
	// 返回值：如果有alias返回true, 否则false
	BOOL HasAlias() const
		{
		if ( m_alias.length > 0 ) // 非cmAliasTypeTransportAddress
			return TRUE;

		return ( m_alias.type == cmAliasTypeTransportAddress
			&& m_alias.transport.ip != 0 );
		}
	///////////////////////////////////////
	// 测试是否有alias
	// 返回值：如果有alias返回true, 否则false
	BOOL IsEmpty() const
		{
		if ( m_alias.length == 0 ) // 非cmAliasTypeTransportAddress
			return TRUE;

		return ( m_alias.type == cmAliasTypeTransportAddress
			&& m_alias.transport.ip == 0 );
		}

	///////////////////////////////////////
	// 得到cmAlias
	// 返回值：Alias的const引用
	const cmAlias & GetcmAlias() const { return m_alias; }

	////////////////////////////////////////////
	// 得到TransportAddress
	// @ 使用时要注意Alias的类型一定要是cmAliasTypeTransportAddress
	// 返回值： cmTransportAddress的const引用
	const cmTransportAddress & GetTransportAdress() const
		{
		assert(m_alias.type==cmAliasTypeTransportAddress);
		return m_alias.transport;
		}
	
	///////////////////////////////////////
	// 得到可写的cmAlias
	// 返回值：Alias的引用
	cmAlias & GetWriteablecmAlias() { return m_alias; }
	
	////////////////////////////////////////////
	// 得到可写的TransportAddress
	// @ 使用时要注意Alias的类型一定要是cmAliasTypeTransportAddress
	// 返回值： cmTransportAddress的引用
	cmTransportAddress & GetWriteableTransportAdress()
		{
		assert(m_alias.type==cmAliasTypeTransportAddress);
		return m_alias.transport;
		}
	
	//////////////////////////////////////
	// 得到Alias的类型
	// 返回值：Alias类型cmAliasType
	cmAliasType GetType() const { return m_alias.type; }

	//////////////////////////////////////
	// 得到PartyNumber类型Alias的类型
	// 使用时要注意Alias的类型必须是cmAliasTypePartyNumber
	// 返回值：PartyNumber类型Alias的类型
	cmPartyNumberType GetPartyNumberType() const
		{
		assert(m_alias.type==cmAliasTypePartyNumber);
		return m_alias.pnType;
		}

	//////////////////////////////////////
	// 将alias转换成字符串
	// @ 如果是cmAliasTypeTransportAddress类型，返回形如100.3.1.4:1719的字符串
	//    否则返回所记录的字符串
	// [out] char *buf : 指向转换后的字符串
	// [in] int len : buf长度。如果是cmAliasTypeTransportAddress类型，长度应该大于等于22
	void ToString(char *buf, int len)
	{
		if (buf == NULL)
		{
			return;
		}

		// cmAliasTypeTransportAddress
		if ( m_alias.type == cmAliasTypeTransportAddress )
			{
			if ( len < 22 ) //" 255.255.255.255:65536" 
				{
				buf[0] = '\0';
				return;
				}
			
			in_addr Addr;
			Addr.s_addr = m_alias.transport.ip;
#ifdef WIN32
			char * psAddr = inet_ntoa(Addr);
			strcpy(buf, psAddr);
#else
#ifdef _VXWORKS_
			inet_ntoa_b(Addr,buf);
#else
#error "This version only surpport WIN32 and VXWORKS!"
#endif // _VXWORKS_
#endif // WIN32

			char sPort[16];
			sprintf(sPort,"%d",m_alias.transport.port);
			strcat(buf, sPort);
			
			return;
			}

		// other type...
		int stringLen = (len < m_alias.length) ? len : m_alias.length;
		int actLen = 0;
		int i;
		char *pPtr = m_aliasString;

		for(i=0; i<stringLen; i++, pPtr++)
		{
			if(((*pPtr)>=0x21)&&((*pPtr)<= 0x7e))
			{
				actLen += sprintf(buf+actLen, "%1c", *pPtr);
			}
			else
			{
				actLen += sprintf(buf+actLen, ".");
			} 
		}  				

		if(actLen < len)
		{
			buf[actLen] = '\0';
		}
		else
		{
			buf[len-1] = '\0';
		}
	}

	////////////////////////////////////////
	// 赋值
	const CKDVAlias & operator = (const CKDVAlias & alias)
	{
		if ( this != &alias )
			{
			memcpy(&m_alias,&(alias.m_alias),sizeof(m_alias));
			m_alias.string = m_aliasString;
			if ( m_alias.type != cmAliasTypeTransportAddress ) // 其它的字符串缓冲才有用
				memcpy(&m_aliasString,&(alias.m_aliasString),sizeof(m_aliasString));
			}
		
		return *this;
	}

	/////////////////////////////////////////
	// == 
	BOOL operator==( const CKDVAlias & another ) const
		{
		if ( this == &another )
			return TRUE;
		
		if ( m_alias.type != another.m_alias.type )
			return FALSE;
		
		switch ( m_alias.type )
			{
			case cmAliasTypePartyNumber:
			case cmAliasTypeE164:
			case cmAliasTypeH323ID:
			case cmAliasTypeEndpointID:
			case cmAliasTypeGatekeeperID:
			case cmAliasTypeURLID:
			case cmAliasTypeEMailID:
				if ( m_alias.type == cmAliasTypePartyNumber
					&& m_alias.pnType != another.m_alias.pnType )
					return FALSE;
				if ( m_alias.length != another.m_alias.length )
					return FALSE;
				return !memcmp(m_aliasString, another.m_aliasString, m_alias.length);
				break;
			case cmAliasTypeTransportAddress:
				return ( m_alias.transport.type == another.m_alias.transport.type
					&&  m_alias.transport.ip == another.m_alias.transport.ip
					&&  m_alias.transport.port == another.m_alias.transport.port
					&&  m_alias.transport.distribution == another.m_alias.transport.distribution
					);
				break;
			default:
				assert(false);
				return FALSE;
				break;
			}

		}
	/////////////////////////////////////////////
	// !=
	BOOL operator!=( const CKDVAlias & another ) const
		{
		if ( this == &another )
			return FALSE;
		
		if ( m_alias.type != another.m_alias.type )
			return TRUE;
		
		switch ( m_alias.type )
			{
			case cmAliasTypePartyNumber:
			case cmAliasTypeE164:
			case cmAliasTypeH323ID:
			case cmAliasTypeEndpointID:
			case cmAliasTypeGatekeeperID:
			case cmAliasTypeURLID:
			case cmAliasTypeEMailID:
				if ( m_alias.type == cmAliasTypePartyNumber
					&& m_alias.pnType != another.m_alias.pnType )
					return TRUE;
				if ( m_alias.length != another.m_alias.length )
					return TRUE;
				return memcmp(m_aliasString, another.m_aliasString, m_alias.length);
				break;
			case cmAliasTypeTransportAddress:
				return ( m_alias.transport.type != another.m_alias.transport.type
					||  m_alias.transport.ip != another.m_alias.transport.ip
					||  m_alias.transport.port != another.m_alias.transport.port
					||  m_alias.transport.distribution != another.m_alias.transport.distribution
					);
				break;
			default:
				assert(false);
				return FALSE;
				break;
			}
		}

public:
	cmAlias m_alias; //协议栈Alias结构
	char	m_aliasString[MAX_REG_ALIAS_BYTELEN];//别名地址内存区
}
#ifdef _VXWORKS_
__attribute__ ( (packed) ) 
#endif // _VXWORKS_
;

//
// class  CRegistrationRequest
// 注册会议的请求
//
class  CRegistrationRequest
{
public:
	CRegistrationRequest() 
	{
		memset(&m_addrCallAddr,0,sizeof(m_addrCallAddr));
	}
public:
	TMtAlias				m_aliasRegConf;	//注册会议别名
	TTransportAddr  		m_addrCallAddr;	//GKM的传输地址
}
#ifdef _VXWORKS_
__attribute__ ( (packed) ) 
#endif // _VXWORKS_
;
//
// class CAdmissonRequest
// 初始带宽和地址解析请求
//
class CAdmissonRequest  //: public CRASMsg
{
public:
	CAdmissonRequest() : m_hsCall(NULL),// m_callType(cmCallTypeP2P),
					m_BandWidth(0)
	{
		memset(&m_tDstAlias,0,sizeof(m_tDstAlias));
	}
public:
	HCALL					m_hsCall;//协议栈呼叫句柄
	TMtAlias				m_tSrcAlias; //主叫的会议别名
	TMtAlias				m_tDstAlias;//被叫别名地址(destinationInfo)
	u32						m_BandWidth; // BandWidth
}
#ifdef _VXWORKS_
__attribute__ ( (packed) ) 
#endif // _VXWORKS_
;
//
// class CUnregistrationRequest
// 注销已经注册的会议
//
class CUnregistrationRequest
{
public:
	CUnregistrationRequest() 
	{
		memset(&m_addrCallAddr,0,sizeof(m_addrCallAddr));
	}
public:
	TMtAlias				m_aliasRegConf;//会议别名
	TTransportAddr 			m_addrCallAddr;//会议传输地址
}
#ifdef _VXWORKS_
__attribute__ ( (packed) ) 
#endif // _VXWORKS_
;
//
// class CDisengageRequest
// 带宽释放请求
//
class CDisengageRequest  
{
public:
	CDisengageRequest() : m_hsCall(NULL)
	{
	}
public:
	HCALL					m_hsCall;//释放带宽针对的呼叫
}
#ifdef _VXWORKS_
__attribute__ ( (packed) ) 
#endif // _VXWORKS_
;
//
// class CBandWidthRequest
// 带宽改变请求
//
class CBandWidthRequest
{
public:
	CBandWidthRequest() : m_hCall(NULL),m_BandWidth(0)
		{
		}

public:
	HCALL					m_hCall;	 //带宽改变针对的呼叫
	u32						m_BandWidth; // 带宽
}
#ifdef _VXWORKS_
__attribute__ ( (packed) ) 
#endif // _VXWORKS_
;
//
// class CLocationRequest
// 定位请求,目前版本中未用
//
class CLocationRequest //: public CRASMsg
{
public:
	CLocationRequest()
		{
		memset(&m_aliasHasKnown,0,sizeof(m_aliasHasKnown));
		}

public:
	TMtAlias				m_aliasHasKnown; //已经知道的被叫别名
}
#ifdef _VXWORKS_
__attribute__ ( (packed) ) 
#endif // _VXWORKS_
;
//
// class CLocationConfirm
// 定位确认,目前版本中未用
//
class CLocationConfirm
{
public:
	CLocationConfirm() 
		{
		memset(&m_addrGetbyAlias,0,sizeof(m_addrGetbyAlias));
		}
	

public:
	TTransportAddr     	m_addrGetbyAlias; //网守解析出的被叫传输地址
}
#ifdef _VXWORKS_
__attribute__ ( (packed) ) 
#endif // _VXWORKS_
;

//
// class CErrorCode
// ErrorCode
//
class CErrorCode
{
public:
	u32		m_dwErrCode; //错误编号
}
#ifdef _VXWORKS_
__attribute__ ( (packed) ) 
#endif // _VXWORKS_
;

#ifdef WIN32
#pragma pack( pop )
#endif

#endif // _KDV_RAS_H
