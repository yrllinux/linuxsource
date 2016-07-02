/*****************************************************************************
模块名      : LdapAdapterLib
文件名      : ldapAdapter.h
相关文件    : 
文件实现功能: LDAP适配库公共常量,结构及API接口定义
作者        : 郭忠军
版本        : V1.0  Copyright(C) 2004-2005 KDC, All rights reserved.
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2007-01-17  1.0         郭忠军      创建
******************************************************************************/
#ifndef __LDAPADAPTER_H
#define __LDAPADAPTER_H

#include "osp.h"
////////////////////////////////// 宏定义 ////////////////////////////////
//版本号
#define VER_LDAPADAPTER "KDV LdapAdapter 40.01.01.00.01.070125"
#define LA_DEFAULT_SERVER_PORT      (u16)389   //目录服务器默认监听端口


#define LA_LEN_DEVID           (u8)32           //设备编号长度
#define LA_LEN_MAX_CALLNUM     (u8)16           //呼叫号码最大长度
#define LA_LEN_MAX_NICKNAME    (u8)64           //设备昵称最大长度


//LDAP适配层错误代码
#define LA_SUCCESS             (u32)0              //成功
#define LA_OPERATION_FAILED    (u32)400            //操作失败。通用的错误代码
#define LA_CONNECT_ERROR       (u32)401            //服务器无法连接或断链
#define LA_ENTRY_NOT_EXIST     (u32)402            //查找的条目不存在
#define LA_AUTH_NEEDED         (u32)403            //权限不足
#define LA_PARAM_ERROR         (u32)404            //无效或错误的API调用参数
#define LA_CANNOT_ALLOC_MEM    (u32)405            //适配器无法分配内存

//以下是LDAP协议栈错误代码，供调试时用。业务层不必关心。
/*
0 - LDAP_SUCCESS 
1- LDAP_OPERATIONS_ERROR 
2 - LDAP_PROTOCOL_ERROR 
3 - LDAP_TIMELIMIT_EXCEEDED 
4 - LDAP_SIZELIMIT_EXCEEDED 
5 - LDAP_COMPARE_FALSE 
6 - LDAP_COMPARE_TRUE 
7 - LDAP_STRONG_AUTH_NOT_SUPPORTED 
8 - LDAP_STRONG_AUTH_REQUIRED 
9 - LDAP_PARTIAL_RESULTS 
10 - LDAP_REFERRAL 
11 - LDAP_ADMINLIMIT_EXCEEDED 
12 - LDAP_UNAVAILABLE_CRITICAL_EXTENSION 
13 - LDAP_CONFIDENTIALITY_REQUIRED 
14 - LDAP_SASL_BIND_IN_PROGRESS 
15 - LDAP_NO_SUCH_ATTRIBUTE 
17 - LDAP_UNDEFINED_TYPE 
18 - LDAP_INAPPROPRIATE_MATCHING 
19 - LDAP_CONSTRAINT_VIOLATION 
20 - LDAP_TYPE_OR_VALUE_EXISTS 
21 - LDAP_INVALID_SYNTAX 
32 - LDAP_NO_SUCH_OBJECT 
33 - LDAP_ALIAS_PROBLEM 
34 - LDAP_INVALID_DN_SYNTAX 
35 - LDAP_IS_LEAF 
36 - LDAP_ALIAS_DEREF_PROBLEM 
48 - LDAP_INAPPROPRIATE_AUTH 
49 - LDAP_INVALID_CREDENTIALS 
50 - LDAP_INSUFFICIENT_ACCESS 
51 - LDAP_BUSY 
52 - LDAP_UNAVAILABLE 
53- LDAP_UNWILLING_TO_PERFORM 
54 - LDAP_LOOP_DETECT 
60 - LDAP_SORT_CONTROL_MISSING 
61 - LDAP_INDEX_RANGE_ERROR 
64 - LDAP_NAMING_VIOLATION 
65 - LDAP_OBJECT_CLASS_VIOLATION 
66 - LDAP_NOT_ALLOWED_ON_NONLEAF 
67 - LDAP_NOT_ALLOWED_ON_RDN 
68 - LDAP_ALREADY_EXISTS 
69 - LDAP_NO_OBJECT_CLASS_MODS 
70 - LDAP_RESULTS_TOO_LARGE 
71 - LDAP_AFFECTS_MULTIPLE_DSAS 
80- LDAP_OTHER 
81 - LDAP_SERVER_DOWN 
82 - LDAP_LOCAL_ERROR 
83 - LDAP_ENCODING_ERROR 
84 - LDAP_DECODING_ERROR 
85 - LDAP_TIMEOUT 
86 - LDAP_AUTH_UNKNOWN 
87 - LDAP_FILTER_ERROR 
88 - LDAP_USER_CANCELLED 
89 - LDAP_PARAM_ERROR 
90 - LDAP_NO_MEMORY 
91 - LDAP_CONNECT_ERROR 
92 - LDAP_NOT_SUPPORTED 
93 - LDAP_CONTROL_NOT_FOUND 
94 - LDAP_NO_RESULTS_RETURNED 
95 - LDAP_MORE_RESULTS_TO_RETURN 
96 - LDAP_CLIENT_LOOP 
97 - LDAP_REFERRAL_LIMIT_EXCEEDED 
*/

//回调消息类型代码
#define LA_EV_AUTH_RSP                (u32)100          //认证响应
#define LA_EV_REG_RSP                 (u32)101          //注册响应
#define LA_EV_SEARCH_BY_CALLNUM_RSP   (u32)102          //按呼叫号码查询单个条目响应
#define LA_EV_GET_LIST_RSP            (u32)103          //获取条目列表响应
#define LA_EV_UNREG_RSP               (u32)104          //注销响应
#define LA_EV_DISCON_NT               (u32)105          //断链通知 
#define LA_EV_CONNECT_NT              (u32)106          //目录服务器连接成功
#define LA_EV_MAX_RETRY_REACHED_NT    (u32)107          //达到最大重连次数仍然无法连接



//设备类型常量
enum ELADevType
{
    emldapdev_8220C =   (u8)1,  
    emldapdev_PCMT_LITE  = (u8)2,
    emldapdev_OTHER = (u8)255, 
};

//NAT类型常量
enum ELANatType
{ 
    emnattype_DEFAULT = (u8)1,  
    emnattype_OTHER = (u8)255, 
};
//////////////////////////////// 结构类型 //////////////////////////////

//设备编号结构
typedef struct tagDeviceID
{
    u8   m_abyDevId[LA_LEN_DEVID];   // 32位定长字符串

	tagDeviceID()
	{
		memset(m_abyDevId,0,sizeof(m_abyDevId));
	}

	BOOL SetDevId(u8 *pbyBuf,u8 byBufLen)
	{
		if((NULL == pbyBuf)||(byBufLen > LA_LEN_DEVID))
		{
			return FALSE;			
		}
		else
		{
			memcpy(m_abyDevId,pbyBuf,byBufLen);
			return TRUE;
		}
	}

	u8 GetDevId(u8 *pbyBuf, u8 byBufLen)
	{
		if((NULL == pbyBuf)||(byBufLen < LA_LEN_DEVID))
		{
			return 0;
		}
		else
		{
			memcpy(pbyBuf,m_abyDevId,LA_LEN_DEVID);
			return LA_LEN_DEVID;
		}
	}

	u8 GetDevLen()
	{
		return (u8)LA_LEN_DEVID;
	}	
	
}TDevId;

//呼叫号码结构
typedef struct tagDialNumber
{    
    u8  m_abyDialNum[LA_LEN_MAX_CALLNUM];   // 变长字符串。不以'\0'标示结束
	u8  m_byLen;                            // 呼叫号码长度

	tagDialNumber()
	{
		m_byLen = 0;
		memset(m_abyDialNum,0,sizeof(m_abyDialNum));
	}

	BOOL SetDialNum(u8* pbyBuf,u8 byBufLen)
	{
		if((NULL == pbyBuf)||(byBufLen > LA_LEN_MAX_CALLNUM))
		{
			return FALSE;
		}
		else
		{
			memcpy(m_abyDialNum,pbyBuf,byBufLen);
			m_byLen = byBufLen;
			return TRUE;
		}
	}

	u8 GetDialNum(u8* pbyBuf, u8 byBufLen)
	{
		if((NULL == pbyBuf)||(byBufLen < m_byLen))
		{
			return 0;
		}
		else
		{
			memcpy(pbyBuf,m_abyDialNum,byBufLen);			
			return m_byLen;
		}		
	}
	
	u8 GetDialNumLen()
	{
		return m_byLen;
	}
}TDialNum;

//设备昵称结构
typedef struct tagTermNickName
{
	u8  m_byLen;                                   // 长度
    u8  m_abyNickName[LA_LEN_MAX_NICKNAME];        // 昵称。变长字符串。不以 \0 结束。  

	tagTermNickName()
	{
		m_byLen = 0;
		memset(m_abyNickName,0,sizeof(m_abyNickName));
	}

	BOOL SetNickName(u8* pbyBuf,u8 byBufLen)
	{
		if((NULL == pbyBuf)||(byBufLen > LA_LEN_MAX_NICKNAME))
		{
			return FALSE;
		}
		else
		{
			memcpy(m_abyNickName,pbyBuf,byBufLen);
			m_byLen = byBufLen;
			return TRUE;
		}
	}

	u8 GetNickName(u8* pbyBuf, u8 byBufLen)
	{
		if((NULL == pbyBuf)||(byBufLen < m_byLen))
		{
			return 0;
		}
		else
		{
			memcpy(pbyBuf,m_abyNickName,byBufLen);			
			return m_byLen;
		}		
	}
	
	u8 GetNickNameLen()
	{
		return m_byLen;
	}
	
}TTermNickName;

// 终端认证信息
typedef struct tagTermAuthInfo
{
    TDevId    m_tDevId;           // 设备编号    
    TDialNum  m_tDialNum;         // 呼叫号码
	u8        m_byDevType;        //设备类型

	tagTermAuthInfo()
	{
		m_byDevType = emldapdev_8220C;		
	}

	BOOL SetDevId(u8 *pbyBuf, u8 byBufLen)
	{
		return m_tDevId.SetDevId(pbyBuf,byBufLen);
	}

	BOOL SetDialNum(u8 *pbyBuf, u8 byBufLen)
	{
		return m_tDialNum.SetDialNum(pbyBuf,byBufLen);
	}

	void  SetDevType(u8 byDevType)
	{
		m_byDevType = byDevType;
	}

	u8 GetDevId(u8 *pbyBuf, u8 byBufLen)
	{
		return m_tDevId.GetDevId(pbyBuf,byBufLen);
	}
	u8 GetDevIdLen()
	{
		return m_tDevId.GetDevLen();
	}
	u8 GetDialNum(u8 *pbyBuf, u8 byBufLen)
	{
		return m_tDialNum.GetDialNum(pbyBuf,byBufLen);
	}
	u8 GetDialNumLen()
	{
		return m_tDialNum.GetDialNumLen();
	}
	u8 GetDevType()
	{
		return m_byDevType;
	}
}TTermAuthInfo;

// 终端注册信息
typedef struct tagTermRegistInfo
{
    TTermNickName   m_tNickName;   // Nick name 
	TDialNum        m_tDialNum;    // 呼叫号码
	u32             m_dwIpAddr;    //IP Addr
	u16             m_wH245Port;   //H245 呼叫信令端口
	u8              m_byNATType;   // NAT 类型

	tagTermRegistInfo()
	{
		m_dwIpAddr = 0;
		m_wH245Port = htons(1720);
		m_byNATType = emnattype_DEFAULT;
	}

	BOOL SetNickName(u8 *pbyBuf, u8 byBufLen)
	{
		return m_tNickName.SetNickName(pbyBuf,byBufLen);
	}

	BOOL SetDialNum(u8 *pbyBuf, u8 byBufLen)
	{
		return m_tDialNum.SetDialNum(pbyBuf,byBufLen);
	}

	void SetIPAddr(u32 dwIP)
	{
		m_dwIpAddr = htonl(dwIP);		
	}

	void SetH245Port(u16 wH245Port)
	{
		m_wH245Port = htons(wH245Port);
	}

	void SetNatType(u8 byNatType)
	{
		m_byNATType = byNatType;
	}

	u8 GetNickNum(u8 *pbyBuf, u8 byBufLen)
	{
		return m_tNickName.GetNickName(pbyBuf,byBufLen);
	}
	u8 GetNickNameLen()
	{
		return m_tNickName.GetNickNameLen();
	}
	
	u8 GetDialNum(u8 *pbyBuf, u8 byBufLen)
	{
		return m_tDialNum.GetDialNum(pbyBuf,byBufLen);
	}
	u8 GetDialNumLen()
	{
		return m_tDialNum.GetDialNumLen();
	}

	u32 GetIPAddr()
	{
		return ntohl(m_dwIpAddr);
	}

	u16 GetH245Port()
	{
		return ntohs(m_wH245Port);
	}

	u8 GetNatType()
	{
		return m_byNATType;
	}

}TTermRegInfo;

//认证响应结构
typedef struct tagLaAuthRsp
{
	u32 m_dwRetCode;   //错误码
	
	tagLaAuthRsp()
	{
		m_dwRetCode = LA_SUCCESS;
	}

	void SetRspCode(u32 dwRetCode)
	{
		m_dwRetCode = dwRetCode;
	}
	
	u32 GetRspCode()
	{
		return m_dwRetCode;
	}
	
}TLaAuthRsp;


//注册响应结构
typedef struct tagLaRegRsp
{
	u32 m_dwRetCode;  //错误码
	
	tagLaRegRsp()
	{
		m_dwRetCode = LA_SUCCESS;
	}

	void SetRspCode(u32 dwRetCode)
	{
		m_dwRetCode = dwRetCode;
	}

	u32 GetRspCode()
	{
		return m_dwRetCode;
	}

}TLaRegRsp;

//查找单个实体响应结构
typedef struct tagLaSearchRsp
{
	u32 m_dwRetCode;             //错误码
	u32 m_dwReqSq;              //对应的搜索请求序列号
	TTermRegInfo m_tRetEntry;   //获取的实体

	tagLaSearchRsp()
	{
		m_dwReqSq = 0;
		m_dwRetCode = LA_SUCCESS;
	}
	void SetRspCode(u32 dwRetCode)
	{
		m_dwRetCode = dwRetCode;
	}
	void SetReqSequece(u32 dwSequence)
	{
		m_dwReqSq = dwSequence;
	}
	u32 GetRspCode()
	{
		return m_dwRetCode;
	}
	u32 GetReqSequence()
	{
		return m_dwReqSq;
	}
}TLaSearchRsp;

//注销响应结构
typedef struct tagLaUnregRsp
{
	u32 m_dwRetCode;	 //错误码
	tagLaUnregRsp()
	{
		m_dwRetCode = LA_SUCCESS;
	}
	void SetRspCode(u32 dwRetCode)
	{
		m_dwRetCode = dwRetCode;
	}
	u32 GetRspCode()
	{
		return m_dwRetCode;
	}
}TLaUnregRsp;


//回调函数结构定义
//三个参数依次是：回调类型/回调内容长度/回调内容指针
typedef  void (*LDAPCB) (u32 dwEventId, u32 dwContLen, void* pContent);


//////////////////////////////// 接口函数//////////////////////////
/*====================================================================
函数名： InitLdapAdapter
功能：   初始化LDAP适配库
算法实现：（可选项）
引用全局变量：
输入参数说明：          
返回值说明：成功返回0；失败返回非零值
====================================================================*/
BOOL32  InitLdapAdapter();  


/*====================================================================
函数名： SetLdapParam
功能：   设置LDAP连接参数。业务层可重复调用此函数，以查询不同的LDAP服务器。
引用全局变量：
输入参数说明：
           pchHost      域名或 x.x.x.x 形式的主机名称。
           wPort        目录服务器监听端口.默认为389    
返回值说明：成功返回TRUE；否则返回FALSE
====================================================================*/
BOOL32 SetLdapParam(const s8* pchHost, u16 wPor = LA_DEFAULT_SERVER_PORT);


/*====================================================================
函数名：  SetAdapterCallBack
功能：    设置回调函数
算法实现：（可选项）
引用全局变量：
输入参数说明：
          pfCBFunction 回调函数指针
返回值说明：成功返回TRUE；否则返回FALSE
====================================================================*/
BOOL32 SetAdapterCallBack(LDAPCB pfCBFunction);


/*====================================================================
函数名：  SendAuthReq
功能：    发送认证请求.匿名终端不需调用此接口。
          没有通过认证或者没有调用认证请求的终端将只能进行查询操作。
算法实现：（可选项）
引用全局变量：
输入参数说明：
          ptAuthInfo 认证信息指针
返回值说明：成功返回TRUE；否则返回FALSE
====================================================================*/
BOOL32  SendAuthReq(TTermAuthInfo* ptAuthInfo);


/*====================================================================
函数名：  SendRegReq
功能：    发送注册请求.匿名终端不需调用此接口。
          如果条目已存在，则做更新操作，业务层不需要感知是初次注册还是更新注册。
算法实现：（可选项）
引用全局变量：
输入参数说明：
          ptRegInfo 注册信息指针
返回值说明：成功返回0；失败返回非零值
====================================================================*/
BOOL32  SendRegReq(TTermRegInfo *ptRegInfo);


/*====================================================================
函数名：  RearchByCallNum
功能：    按呼叫号码查询终端信息。
          业务层提供请求序列号，以支持多个查询回调.
算法实现：（可选项）
引用全局变量：
输入参数说明：
          dwSequence  请求序列号
          ptCallNum   呼叫号码指针
返回值说明：成功返回0；失败返回非零值
====================================================================*/
BOOL32  SearchByCallNum(u32 dwSequece, TDialNum *ptCallNum);



/*====================================================================
函数名：  SendUnRegReg
功能：    发送注销请求.匿名终端不需调用此接口。          
算法实现：（可选项）
引用全局变量：
输入参数说明：
          ptRegInfo 注销信息指针
返回值说明：成功返回0；失败返回非零值
====================================================================*/
BOOL32 SendUnRegReg(TTermRegInfo *ptUnRegInfo);



/*====================================================================
函数名：  UnitLdapAdapter
功能：    退出LDAP适配库。          
算法实现：（可选项）
引用全局变量：
输入参数说明：
返回值说明：成功返回0；失败返回非零值
====================================================================*/
BOOL32 UnitLdapAdapter();






#endif //__LDAPADAPTER_H
