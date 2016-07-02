/*****************************************************************************
   模块名      : DataSwitch
   文件名      : dataswitch.h
   相关文件    : 
   文件实现功能: DataSwitch模块接口函数定义
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   2005/03/18  4.0         靳志业        创建
   2005/08/10  4.0         靳志业        实现3.6中新增的功能
******************************************************************************/

#ifndef DATASWITCH_H
#define DATASWITCH_H


#include "osp.h"

#ifdef _MSC_VER
#include <winsock2.h>
#include <ws2tcpip.h>
#endif

#ifdef _LINUX_
#include <netinet/in.h>
#include <netinet/ip6.h>
#include <netinet/udp.h>
#endif


#define DSVERSION       "Dataswitch_socket(IPv6) 40.20.01.03.120525"

/* DataSwitch 句柄 */
#define DSID    u32
	
/*DataSwitch 返回值*/
#define DSOK    1
#define DSERROR 0

#define INVALID_DSID  0xffffffff   /*无效的句柄值*/


// DataSwitch网络地址类型定义
#define DS_SUPPORT_NONE 0x00
#define DS_SUPPORT_IPV4 0x01
#define DS_SUPPORT_IPV6 0x02

#define DS_CFLAG_IPV6

#if defined(DS_CFLAG_IPV6)
  #undef  DS_SUPPORT_TYPE
  #define DS_SUPPORT_TYPE   DS_SUPPORT_IPV6
#else
  #undef  DS_SUPPORT_TYPE
  #define DS_SUPPORT_TYPE   DS_SUPPORT_IPV4
#endif


#ifdef _LINUX_
  #define IN_ADDR    in_addr
  #define IN6_ADDR   in6_addr
#endif

#ifdef _MSC_VER
  #ifndef snprintf
    #define snprintf   _snprintf   
  #endif
  #ifndef vsnprintf
    #define vsnprintf  _vsnprintf
  #endif
#endif

#ifndef _MSC_VER
	#define SOCKHANDLE			 int

	#ifndef SOCKADDR
		#define SOCKADDR		 sockaddr
	#endif

	#define SOCKADDR_IN 		 sockaddr_in
	#define SOCKADDR_IN6 		 sockaddr_in6
#endif

#if (DS_SUPPORT_TYPE & DS_SUPPORT_IPV6)
  #define DS_SOCKET_SOCKADDR_SIZE sizeof(struct sockaddr_in6)
#else
  #define DS_SOCKET_SOCKADDR_SIZE sizeof(struct sockaddr_in)
#endif




#define DS_TYPE_NONE           (u8)0         //未定义地址类型
#define DS_TYPE_IPV4           (u8)1         //IPv4地址类型
#define DS_TYPE_IPV6           (u8)2         //IPv6地址类型

#define DS_IPV6_ADDRSIZE   16
#define DS_MAX_IPSTR_SIZE  64

/**
* 地址类型结构, 可支持IPv4和IPv6
*/
typedef struct tagDSNetAddr
{
	u8  byIPType;
	u16 wPort;
	u32 dwScopeID;
	union
	{
		u32 dwV4IP;                      // IPv4地址, 网络序
		u8  abyV6IP[DS_IPV6_ADDRSIZE];   // IPv6地址, 网络序		
	};

	tagDSNetAddr()
	{
		Clear();
	}

	void Clear()
	{
		memset( this, 0, sizeof(tagDSNetAddr) );
	}

	void SetV4IPAddress( u32 dwV4Addr )
	{
		byIPType = DS_TYPE_IPV4;
		dwV4IP = dwV4Addr;
	}
	
	void SetV6IPAddress( u8 *pbyV6Addr )
	{
		byIPType = DS_TYPE_IPV6;
		memcpy( abyV6IP, pbyV6Addr, DS_IPV6_ADDRSIZE );
	}

	u32 GetV4IPAddress()
	{
		return dwV4IP;
	}
	
	u8 *GetV6IPAddress()
	{
		return abyV6IP;
	}

	void SetType(u8 byType)
	{
		byIPType = byType;
	}
	
	u8 GetType( )
	{
		return byIPType;
	}

	void SetPort( u16 port )
	{
		wPort = port;
	}

	u16 GetPort(  )
	{
		return wPort;
	}

	void SetScopeID( u32 scopeid )
	{
		dwScopeID = scopeid;
	}

	u32 GetScopeID(  )
	{
		return dwScopeID;
	}


	BOOL32 SetIPStr( u8 byType, s8* pchIPStr )
	{
		BOOL32 bRet = FALSE; 
		
		if ( byType == DS_TYPE_IPV4 )
		{
			bRet = SetV4IPStr( pchIPStr );
		} 
		else if  ( byType == DS_TYPE_IPV6 )
		{
			bRet = SetV6IPStr( pchIPStr );
		}
		
		return bRet;
	}
	
	const s8 *GetIPStr( s8* pchIPStr, u8 bySize )
	{
		if ( byIPType == DS_TYPE_IPV4 )
		{
			return GetV4IPStr( pchIPStr, bySize );
		} 
		if ( byIPType == DS_TYPE_IPV6 )
		{
			return GetV6IPStr( pchIPStr, bySize );
		}
		else
		{
			return NULL;
		}
	}
	
	BOOL32 SetV4IPStr( s8* pchIPStr )
	{
		BOOL32 bRet = inet_pton( AF_INET, pchIPStr, &dwV4IP );
		if ( TRUE == bRet ) 
		{
			byIPType = DS_TYPE_IPV4;
			return TRUE;
		} 
		else
		{
			return FALSE;
		}
	}
	
	BOOL32 SetV6IPStr( s8* pchIPStr )
	{
		BOOL32 bRet = inet_pton( AF_INET6, pchIPStr, abyV6IP );
		if ( TRUE == bRet ) 
		{
			byIPType = DS_TYPE_IPV6;
			return TRUE;
		} 
		else
		{
			return FALSE;
		}
	}
	
	const s8 *GetV4IPStr( s8* pchIPStr, u8 bySize )
	{
		return inet_ntop( AF_INET, &(dwV4IP), pchIPStr, bySize );
	}
	
	const s8 *GetV6IPStr( s8* pchIPStr, u8 bySize )
	{
		return inet_ntop( AF_INET6, abyV6IP, pchIPStr, bySize );
	}


}TDSNetAddr;
 
 /*
 * @func FilterFunc
 * @brief 接收过滤函数
 *
 * 每个接收结点有一个过滤函数，当此结点接收到UDP数据包时执行此函数。
 * 并根据函数的返回值，来动态地决定是否对此数据包转发。
 *
 * @param ptRecvAddr     - 接收地址
 * @param ptSrcAddr      - 源地址
 * @param pData          - [in, out]数据包；可以修改；
 * @param uLen           - [in, out]数据包长度；修改后的长度不可超过最大值
 * @return 0, 该数据包有效； 其他值，该数据包无效；
 */	
typedef u32 (*FilterFunc)(TDSNetAddr* ptRecvAddr, TDSNetAddr* ptSrcAddr, u8* pData, u32 uLen);


#define SENDMEM_MAX_MODLEN        (u8)32        // 发送时允许的最大修改长度
    
/**
 * 数据包转发目标：指明具体的地址对
 * DS所控制的多个IP，对于数据包的转发应该不产生影响。也就是说，如果
 * 有两个IP都可以将一个数据包正确的地转发出去，则使用哪一个IP是没有
 * 关系的。所以，转发输出接口IP是不需要的。
 */
typedef struct tagNetSndMember
{
	TDSNetAddr tDstAddr; /*转发目的地址*/
    long       lIdx;     /* 接口索引 */
    u32        errNum;   /* 出错次数*/
    long       errNo;    /* 错误号 */
    void *     lpuser;   /* user info */
    /* 为实现发送时修改数据而增加的部分 */
    u16  wOffset;        /* 修改部分的起始位置; 目前未使用 */
    u16  wLen;		/* 修改部分的长度，必须为4的倍数；为零表示不修改 */
    char            pNewData[SENDMEM_MAX_MODLEN];
    /* 要修改的数据 */
    void *          pAppData;       /* 用户自定义的回调函数的参数 */

}TNetSndMember;


typedef void (*SendCallback)(TDSNetAddr* ptRecvAddr,
                             TDSNetAddr* ptSrcAddr,
                             TNetSndMember *ptSends,            // 转发目标列表
                             u16* pwSendNum,                    // 转发目标个数 
                             u8* pUdpData, 
							 u32 dwUdpLen);


/************************************************************
 * 函数名：dsCreate
 *
 * 功能：  运行DataSwitch模块
 *
 * 实现说明： 
 * Dataswitch支持“多用户”；每次调用dsCreate就会返回一个
 * 用户标识。添加、删除时均要匹配用户标志，转发时则不考虑。
 *
 * 返回值： 
 * @return INVALID_DSID: 表示失败 ; 否则成功
 ************************************************************/
API DSID dsCreate( u32 dwIfNum,  TDSNetAddr atIP[] );

/************************************************************
 * 函数名：dsDestroy
 *
 * 功能： 终止DataSwitch的运行
 *
 * 实现说明：
 * 一次删除一个用户，直到所有用户都删除，所有资源才全释放。
 *
 * 参数说明：
 * @param dsId  - 用户标识
 * 
 * 返回值：
 ************************************************************/
API void dsDestroy( DSID dsId );
 
/************************************************************
 * 函数名： dsAdd
 *
 * 功能： 增加转发目标唯一的转发规则
 *
 * 实现说明：
 * “目前唯一”的含义是：如果已经存在的交换规则中转发目标
 * 与当前规则的相同，则先删除这些规则。
 *
 * 参数说明：
 * @param dsId          - 用户标识
 * @param ptRecvAddr    - 接收地址，即接收数据包的目的地址
 * @param ptInLocalIP   - 接收数据包的网络接口
 * @param ptSendtoAddr  - 转发目的地址
 * @param ptOutLocalIP  - 转发数据包所用本地IP
 *
 * 返回值：
 * @return DSOK: 表示成功 ; DSERROR: 表示失败 ;
 ************************************************************/
API u32 dsAdd(DSID dsId,
			  TDSNetAddr* ptRecvAddr,
			  TDSNetAddr* ptInLocalIP,
			  TDSNetAddr* ptSendtoAddr,
              TDSNetAddr* ptOutLocalIP = NULL);

/************************************************************
 * 函数名：dsRemove
 *
 * 功能： 删除转发目标为指定地址的转发规则
 * 
 * 实现说明：
 *
 * 参数说明：
 * @param dsId          - 用户标识
 * @param ptSendtoAddr  - 转发目的地址
 *
 * 返回值：
 * @return DSOK: 表示成功 ; DSERROR: 表示失败 ;
 ************************************************************/
API u32 dsRemove(DSID dsId, TDSNetAddr* ptSendtoAddr);

/************************************************************
 * 函数名：dsAddDump
 *
 * 功能： 增加Dump规则
 * DUMP规则：从指定地址接收的数据包不被转发。
 * 如果一个接收地址只有DUMP规则，则接收的数据包不被转发；
 * 如果还有其他转发规则，则按照其他规则转发。
 *
 * 实现说明：
 * DUMP规则允许被创建多次，因为它与其他规则是可以并存的，
 * 包括它自身；
 *
 * 参数说明：
 * @param dsId          - 用户标识
 * @param ptRecvAddr    - 接收地址，即接收数据包的目的地址
 * @param ptInLocalIP   - 接收数据包的网络接口
 *
 * 返回值：
 * @return DSOK: 表示成功 ; DSERROR: 表示失败 ;
 ************************************************************/
API u32 dsAddDump(DSID dsId, TDSNetAddr* ptRecvAddr, TDSNetAddr* ptInLocalIP);

/************************************************************
 * 函数名：dsRemoveDump
 *
 * 功能： 删除Dump规则
 *
 * 实现说明：
 * 
 * 参数说明：
 * @param dsId          - 用户标识
 * @param ptRecvAddr    - 接收地址，即接收数据包的目的地址
 *
 * 返回值：
 * @return DSOK: 表示成功 ; DSERROR: 表示失败 ;
 ************************************************************/
API u32 dsRemoveDump(DSID dsId, TDSNetAddr* ptRecvAddr);

/************************************************************
 * 函数名：dsAddManyToOne
 * 
 * 功能： 增加多对一的转发规则
 * 注意，它与dsAdd的语义正好相反，两者混合使用时要小心。
 * 
 * 实现说明：
 *
 * 参数说明：
 * @param dsId           - 用户标识
 * @param ptRecvAddr     - 接收地址，即接收数据包的目的地址
 * @param ptInLocalIP    - 接收数据包的网络接口
 * @param ptSendtoAddr   - 转发目的地址
 * @param ptOutLocalIP   - 转发数据包所用本地IP（必须与转发目的地址同协议族）
 *
 * 返回值：
 * @return DSOK: 表示成功 ; DSERROR: 表示失败 ;
 ************************************************************/
API u32 dsAddManyToOne(DSID dsId ,
					   TDSNetAddr* ptRecvAddr,
					   TDSNetAddr* ptInLocalIP,
					   TDSNetAddr* ptSendtoAddr,
                       TDSNetAddr* ptOutLocalIP = NULL);

/************************************************************
 * 函数名：dsRemoveAllManyToOne
 * 
 * 功能： 删除所有转发目标为指定地址的多对一规则
 * 
 * 注意：它的语义与dsRemove完全相同（可以从接口参数上证明）
 * 提供这个接口是为了不让两类不同语义的接口混合使用。一个
 * 程序中，应当始终使用某一类接口。
 *
 * 实现说明：
 * 
 * 参数说明：
 * @param dsId           - 用户标识
 * @param ptSendtoAddr   - 转发目的地址
 *
 * 返回值：
 * @return DSOK: 表示成功 ; DSERROR: 表示失败 ;
 ************************************************************/
API u32 dsRemoveAllManyToOne(DSID dsId, TDSNetAddr* ptSendtoAddr );

/************************************************************
 * 函数名：dsRemoveManyToOne
 *
 * 功能： 删除指定的多对一规则
 *
 * 实现说明：
 * 
 * 参数说明：
 * @param dsId           - 用户标识
 * @param ptRecvAddr     - 接收地址，即接收数据包的目的地址
 * @param ptSendtoAddr   - 转发目的地址
 *
 * 返回值：
 * @return DSOK: 表示成功 ; DSERROR: 表示失败 ;
 ************************************************************/
API u32 dsRemoveManyToOne(DSID dsId ,
                          TDSNetAddr* ptRecvAddr,
                          TDSNetAddr* ptSendtoAddr );

/************************************************************
 * 函数名：dsAddSrcSwitch
 *
 * 功能： 增加按源转发的规则
 * 
 * 按源转发：使用此规则时，接收数据包的源地址必须与规则中的源
 * 地址相等。每次转发时，都要先根据数据包中的源地址查找转发规
 * 则，如果找不到，则使用默认源地址（即0@0）的转发规则。
 * 注意：该接口支持多点到一点。如果tSrcAddr中SrcIP和SrcPort都为零，
 * 它就完全等同于dsAddManyToOne。
 *
 * 实现说明：
 *
 * 参数说明：
 * @param dsId           - 用户标识
 * @param ptRecvAddr     - 接收地址，即接收数据包的目的地址
 *                        如果源地址为IPv6地址，则该地址必须为IPv6地址，
 *                        建议将该地址统一设置为IPv6地址
 * @param ptInLocalIP    - 接收数据包的网络接口
 * @param ptSrcAddr      - 接收数据包的源地址
 * @param ptSendtoAddr   - 转发目的地址
 * @param ptOutLocalIP   - 转发数据包所用本地IP（必须与目的地址同协议族）
 *
 * 返回值：
 * @return DSOK: 表示成功 ; DSERROR: 表示失败 ;
 ************************************************************/      
API u32 dsAddSrcSwitch(DSID dsId ,
                       TDSNetAddr* ptRecvAddr,
                       TDSNetAddr* ptInLocalIP,
					   TDSNetAddr* ptSrcAddr,
					   TDSNetAddr* ptSendtoAddr,
                       TDSNetAddr* ptOutLocalIP = NULL);

/************************************************************
 * 函数名：dsRemoveAllSrcSwitch
 *
 * 功能： 删除所有指定的按源转发规则
 *
 * 实现说明：
 * 
 * 参数说明：
 * @param dsId           - 用户标识
 * @param ptRecvAddr     - 接收地址，即接收数据包的目的地址
 * @param ptSrcAddr      - 接收数据包的源地址
 *
 * 返回值：
 * @return DSOK: 表示成功 ; DSERROR: 表示失败 ;
 ************************************************************/      
API u32 dsRemoveAllSrcSwitch(DSID dsId,
                             TDSNetAddr* ptRecvAddr,
                             TDSNetAddr* ptSrcAddr );

/************************************************************
 * 函数名：dsRemoveSrcSwitch
 * 
 * 功能： 删除指定的按源转发的规则
 *
 * 实现说明：
 * 
 * 参数说明：
 * @param dsId          - 用户标识
 * @param ptRecvAddr     - 接收地址，即接收数据包的目的地址
 * @param ptSrcAddr      - 接收数据包的源地址
 * @param ptSendtoAddr   - 转发目的地址
 *
 * 返回值：
 * @return DSOK: 表示成功 ; DSERROR: 表示失败 ;
 ************************************************************/      
API u32 dsRemoveSrcSwitch(DSID dsId,
                          TDSNetAddr* ptRecvAddr,
                          TDSNetAddr* ptSrcAddr,
                          TDSNetAddr* ptSendtoAddr );

/************************************************************
 * 函数名：dsSetFilterFunc
 * 
 * 功能： 设置过滤函数
 *
 * 注意：函数指针可以为空，表示清除原有的设置；另外，由于
 * DataSwitch是在另外的线程中调用此函数的，要设置的函数必
 * 须是全局函数；设置的参数，也必须是全局有效的。
 *
 * 实现说明：
 * 
 * 参数说明：
 * @param dsId          - 用户标识
 * @param ptRecvAddr    - 接收地址，即接收数据包的目的地址
 * @param ptFunc        - 过滤函数指针
 * @param FuncParam     - 为过滤函数传递的参数
 *
 * 返回值：
 * @return DSOK: 表示成功 ; DSERROR: 表示失败 ;
 ************************************************************/
API u32 dsSetFilterFunc(DSID dsId,
						TDSNetAddr* ptRecvAddr,
                        FilterFunc ptFunc);
 
/************************************************************
 * 函数名：dsRemoveAll
 * 
 * 功能： 删除所有的转发规则
 * 
 * 实现说明：
 * 
 * 参数说明：
 * @param dsId          - 用户标识
 *
 * 返回值：
 * @return DSOK: 表示成功 ; DSERROR: 表示失败 ;
 ************************************************************/      
API u32 dsRemoveAll( DSID dsId );

/**********************************************************
 * 函数: dsSpecifyFwdSrc                                  *
 * 功能: 为指定接收地址设置转发数据包所填充的源地址(IPv4) *
 * 输入:
 * @param dsId           - DSID
 * @param ptOrigAddr     - 原始地址
 * @param ptV4MappedAddr - IPv4映射地址
 * 输出: 无                                               *
 * 返回: 成功返回DSOK 否则DSERROR                         *
 **********************************************************/
API u32 dsSpecifyFwdSrc(DSID dsId, 
						TDSNetAddr* ptOrigAddr,
						TDSNetAddr* ptV4MappedAddr );

/**********************************************************
 * 函数: dsSpecifyFwdSrcV6                                 *
 * 功能: 为指定接收地址设置转发数据包所填充的源地址(IPv6)  *
 * 输入:
 * @param dsId           - DSID
 * @param ptOrigAddr     - 原始地址
 * @param ptMappedAddrV6 - IPv6映射地址
 * 输出: 无                                               *
 * 返回: 成功返回DSOK 否则DSERROR                         *
 **********************************************************/
API u32 dsSpecifyFwdSrcV6(DSID dsId, 
						TDSNetAddr* ptOrigAddr,
						TDSNetAddr* ptMappedAddrV6 );

/**********************************************************
 * 函数: dsResetFwdSrc                                    *
 * 功能: 恢复指定地址转发数据包的源地址
 * 输入:
 * @param dsId          - DSID
 * @param ptOrigAddr    - 原始IP
 * 输出: 无                                               *
 * 返回: 成功返回DSOK 否则DSERROR                         *
 **********************************************************/
API u32 dsResetFwdSrc(DSID dsId, TDSNetAddr* ptOrigAddr );

/**********************************************************
 * 函数: dsResetFwdSrcV6                                    *
 * 功能: 恢复指定地址转发数据包的源地址(IPv6)
 * 输入:
 * @param dsId          - DSID
 * @param ptOrigAddr    - 原始IP
 * 输出: 无                                               *
 * 返回: 成功返回DSOK 否则DSERROR                         *
 **********************************************************/
API u32 dsResetFwdSrcV6(DSID dsId, TDSNetAddr* ptOrigAddr );


 /**********************************************************
 * 函数: dsSetSendCallback                                *
 * 功能: 设置发送回调函数                                 *
 *       取消时，设置函数指针为NULL即可
 * 输入: dsID           - 创建交换模块时的句柄            *
 *       ptRcvAddr      - 本地接收地址                    *
 *       ptSrcAddr      - 转发目的地址                    *
 *       pfCallback     - 回调函数
 * 输出: 无                                               *
 * 返回: 无                                               *
 **********************************************************/    
API u32 dsSetSendCallback( DSID dsId, 
						   TDSNetAddr* ptRcvAddr,
						   TDSNetAddr* ptSrcAddr, 
						   SendCallback pfCallback);

 /**********************************************************
 * 函数: dsSetAppDataForSend                              *
 * 功能: 为发送目标设置一个自定义的指针                   *
 *       取消时，设置函数指针为NULL即可
 * 输入: dsID           - 创建交换模块时的句柄            *
 *       ptRcvAddr      - 本地接收地址                    *
 *       ptSrcAddr      - 源地址                          *
 *       ptDstAddr      - 转发目的地址                    *
 *       pAppData       - 自定义指针
 * 输出: 无                                               *
 * 返回: 
 *     DSOK:成功 DSERROR:失败                             *
 **********************************************************/    
API u32 dsSetAppDataForSend( DSID dsId, 
							 TDSNetAddr* ptRcvAddr, 
							 TDSNetAddr* ptSrcAddr, 
							 TDSNetAddr* ptDstAddr, 
							 void * pAppData);
                
/**********************************************************
 * 函数: dsGetRecvPktCount                                *
 * 功能: 查询接收总包数                                   *
 * 输入: dsID            - 创建交换模块时的句柄           *
 *       ptRcvAddr       - 本地接收地址                   *
 *       ptSrcAddr       - 源地址                         *
 *       dwRecvPktCount  - 接收总包数                     *
 * 输出: 无                                               *
 * 返回: 成功返回DSOK 否则DSERROR                         *
 **********************************************************/    
API u32 dsGetRecvPktCount( DSID dsId , 
						   TDSNetAddr* ptRcvAddr,
						   TDSNetAddr* ptSrcAddr,
						   u32& dwRecvPktCount );

/**********************************************************
 * 函数: dsGetSendPktCount                                *
 * 功能: 查询发送总包数                                   *
 * 输入: dsID            - 创建交换模块时的句柄           *
 *       ptRcvAddr       - 本地接收地址                   *
 *       ptSrcAddr       - 源地址                         *
 *       ptSendtoAddr    - 转发目的地址                   *
 *       dwSendPktCount  - 发送总包数                     *
 * 输出: 无                                               *
 * 返回: 成功返回DSOK 否则DSERROR                         *
 **********************************************************/    
API u32 dsGetSendPktCount( DSID dsId , 
						   TDSNetAddr* ptRcvAddr, 
						   TDSNetAddr* ptSrcAddr, 
						   TDSNetAddr* ptSendtoAddr,
						   u32& dwSendPktCount );
						  
/**********************************************************
 * 函数: dsGetRecvBytesCount	                          *
 * 功能: 查询接收总字节数                                 *
 * 输入:												  *
 * 输出: 无                                               *
 * 返回: 返回即时接收字节数								  *
 **********************************************************/
API s64 dsGetRecvBytesCount( );

/**********************************************************
 * 函数: dsGetSendBytesCount	                          *
 * 功能: 查询发送总字节数                                 *
 * 输入:												  *
 * 输出: 无                                               *
 * 返回: 返回即时字节数									  *
 **********************************************************/
API s64 dsGetSendBytesCount( );

/************************************************************
 * 函数： dsinfo
 * 功能： 显示所有的转发规则，及所有正在监听的端口 
 * 输入：
 * 输出：
 * 返回：
 ************************************************************/      
API void dsinfo();

/************************************************************
 * 函数： dsver
 * 功能： 显示Dataswitch的版本信息
 * 输入：
 * 输出：
 * 返回：
 ************************************************************/      
API void dsver();

/************************************************************
 * 函数： dshelp
 * 功能： 显示Dataswitch所提供的命令的帮助信息
 * 输入：
 * 输出：
 * 返回：
 ************************************************************/      
API void dshelp();

/************************************************************
 * 函数： dedebug
 * 功能： 打开/关闭调试信息
 * 
 * 输入：
 * @param op            - 指明是设置还是清除：set, clear
 *                        (仅在WIN32有效)
 * 输出：
 * 返回：
 ************************************************************/      
API void dsdebug(char* op = NULL);

/************************************************************
 * 函数： dedebug
 * 功能： 打开/关闭更高一级调试信息
 * 单独设置此函数是因为调试信息会影响整体性能 
 * 
 * 输入：
 * @param op            - 指明是设置还是清除：set, clear
 *                        (仅在WIN32有效)
 * 输出：
 * 返回：
 ************************************************************/      
API void dsdebug2();

#endif
