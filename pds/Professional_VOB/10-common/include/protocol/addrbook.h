#ifndef ADDR_BOOK_H
#define ADDR_BOOK_H

#include "osp.h"
#include "eventid.h"
#include "kdvtype.h"
#include "lanman.h"

const u8  HOST_ORDER = 1;				// 字节序
const u8  NET_ORDER = 2;				// 网络序
#define	  MAX_NAME_LEN   48		        // 最大名字长(宏定义避免编译有parse error)
const u32 MAX_H323ALIAS_LEN = 48;		// 最大别名长度
const u32 MAX_NAME_LEN_OLD = 32;		// 老版本最大名字长
const u32 MAX_H323ALIAS_LEN_OLD = 32;	// 老版本最大别名长度
const u32 MAX_E164NUM_LEN = 16;		    // 最大号码长度
const u32 MAX_H320ID_LEN  = 48;		    // 最大H320ID长度
const u32 MAX_IPADDR_LEN  = 15;		    // 最大IP地址长度
const u32 MAX_ADDRENTRY = 5000;			// 地址薄容纳的最大地址条目
const u32 MAX_ADDRGROUP = 64;			// 地址薄容纳的最大组个数
const u32 MAXNUM_ENTRY_TABLE = 192;	    // 最大终端列表个数
const u32 MAXNUM_GROUP_TABLE = 128;	    // 最大会议组列表个数
const u32 INVALID_INDEX = 0xFFFFFFFF;   // 作为初始索引号
const u32 MAX_GROUP_USERDATA_LEN = 256; // 会议组用户数据最大长度
const u16 MAX_DELENTRY_NUM = 512;       // 一次删除条目最大个数
const u32 MULTI_ENTRY = 0xFEFEFEFE;     // 删除条目或组条目时用来表示多组

const u32 VERSION1_ADDRENTRY_LENGTH = 143;     // 版本1的地址条目长度
const u32 VERSION1_MULTISETENTRY_LENGTH = 813; // 版本1的组条目长度

const u32 VERSION_MARK = 0x00000000;         //各系统地址薄文件兼容后版本标识
const u32 VERSION_ID1   = 110825;             //addrbook.kdv版本ID,该版本地址薄呼叫码率不兼容
const u32 VERSION_ID2   = 120215;             //带有会议组条目层次结构的地址薄
const u32 VERSION_ID    = 120215/*110923*/;



#include "array.h"

#define     ADDRBOOK_PATH_LEN       256
#define		ADDRBOOK_APPID	        46	//地址簿
#define		ADDRBOOK_LOG_HINT		100
#define		ADDRBOOK_LOG_ERROR	    0


#define    ADDRBOOK_FILEHEAD           "kdvaddrbook copyright"      //文件标识(不可更改)
const u8   ADDR_FILEHEAD_LEN           = 32;                        //文件表示占字节数，其余补0
const u32  VERSION                     = 20130424;                  //文件版本号
const u16  ADDRBOOK_READFILE_BUF_LEN   = 2048;                      //一次读取文件BUF大小 

//条目类型定义
enum emEntryType
{
    emEntryType_Start,
        emEntryType_BaseEntry,
        emEntryType_GroupEntry,
        emEntryType_GroupRelation,
        emEntryType_End,
};

//编码方式，只做内部定义，不用来交互协商
enum emCodingMode
{
    emCodingModeStart,
        emCodingModeGBK,
        emCodingModeUTF8,
        emCodingModeEnd,
};

#if defined(WIN32) 
#pragma pack(push)
#pragma pack(1)
#endif

#ifndef min
#define min(a,b) (((a) < (b)) ? (a) : (b))
#endif

//属性信息(网络序)
struct TAttributeInfo
{
private:
    u8      m_byCodingMode;
    u16     m_wEntryNum;
public:
    
    TAttributeInfo()
    {
        m_byCodingMode = 0;
        m_wEntryNum = 0;
    }
    
    void SetCodingMode( u8 byCodingMode )
    {
        m_byCodingMode = byCodingMode;
    }
    u8 GetCodingMode()
    {
        return m_byCodingMode;
    }
    
    void SetEntryNum( u16 EntryNum )
    {
        m_wEntryNum = htons(EntryNum);
    }
    u16 GetEntryNum()
    {
        return ntohs(m_wEntryNum);
    }
    
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

// 地址薄条目信息
struct tagAddrEntryInfo
{
	char	achEntryName[ MAX_NAME_LEN + 1 ];		// 条目名字
	char	achMtAlias[ MAX_H323ALIAS_LEN + 1 ];	// 呼叫别名
	u32	    dwMtIpAddr;								// IP地址(网络序)
	char	achMtNumber[ MAX_E164NUM_LEN + 1 ];		// 呼叫号码
	u8      bH320Terminal;                          // 0:H323终端; 非0:H320终端
	char    achH320Id[MAX_H320ID_LEN];              // H320ID
	u16	    wCallRate;								// 呼叫速率

public:
    void print()
    {
        OspPrintf(TRUE,FALSE, "tagAddrEntryInfo:==========\n");
        if ( bH320Terminal != (u8)0 )
        {
            OspPrintf( TRUE, FALSE, "H320Id : %s\n", achH320Id);
            OspPrintf( TRUE, FALSE, "======================\n" );
            return;
        }
        
        if ( achEntryName == NULL )
            OspPrintf( TRUE, FALSE, "Name    : 0\n" );
        else
            OspPrintf( TRUE, FALSE, "Name    : %s\n", achEntryName );
        
        if ( achMtAlias == NULL )
            OspPrintf( TRUE, FALSE, "Alias	 : 0\n" );
        else
            OspPrintf( TRUE, FALSE, "Alias	 : %s\n", achMtAlias );
        
        if ( achMtNumber == NULL )
            OspPrintf( TRUE, FALSE, "Number  : 0\n" );
        else
            OspPrintf( TRUE, FALSE, "Number  : %s\n", achMtNumber );
        
        OspPrintf( TRUE, FALSE, "IP      : 0x%x\n", dwMtIpAddr );
        OspPrintf( TRUE, FALSE, "Rate    : 0x%x\n", wCallRate );
        OspPrintf( TRUE, FALSE, "======================\n" );
    }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

typedef tagAddrEntryInfo TADDRENTRYINFO;

// 会议组信息
struct tagMultiSetEntryInfo
{
    tagMultiSetEntryInfo() {achGroupName[0]='\0'; dwUserDataLen = 0;}
    char achGroupName[ MAX_NAME_LEN + 1 ];	// 组名
	u32	dwMcuIpAddr;						// MCU地址(网络序)
	u16	wConfBitRate;						// 会议速率
	u8	byOpen;								// 开放性(0:开放	1:非开放)
    u32 dwUserDataLen;                            // 用户数据长度
    u8  byUserData[MAX_GROUP_USERDATA_LEN];                          // 用户数据256字节
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;
typedef tagMultiSetEntryInfo TMULTISETENTRYINFO;

//多级会议组结构描述
struct TMultiLevel
{
    friend class CAddrBook;
private:
    u32 m_byGroupIndex;
    u32 m_byUpGroupIndex;
public:
    TMultiLevel()
    {
        Clear();
    }
    
    void Clear()
    {
        m_byGroupIndex = INVALID_INDEX;
        m_byUpGroupIndex = INVALID_INDEX;
    }

    BOOL32 IsNull()
    {
        if ( m_byGroupIndex == INVALID_INDEX ||
             m_byUpGroupIndex == INVALID_INDEX)
        {
            return TRUE;
        }
        else
        {
            return FALSE;
        }
    }

    u32 GetGroupIndex(){ return ntohl(m_byGroupIndex); }
    void SetGroupIndex( u32 dwGroupIndex ){ m_byGroupIndex = htonl(dwGroupIndex); }
    
    u32 GetUpGroupIndex(){ return ntohl(m_byUpGroupIndex); }
    void SetUpGroupIndex( u32 dwUpGroupIndex ){ m_byUpGroupIndex = htonl(dwUpGroupIndex); } 
    
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;


class CAddrBook;


// 地址薄条目
////////////////////////////////////////////////////////////////////////////////////
class CAddrEntry
{
	friend class CAddrBook;
public :
	// constructor,destructor,copy-constructor,assign
	/*
	====================================================================
    功能        ：无参构造函数
    输入参数说明：无
    返回值说明  ：无
	====================================================================
	*/
	CAddrEntry();

	/*
	====================================================================
    功能        ：带地址簿条目信息结构参数的构造函数
    输入参数说明：地址条目信息结构
    返回值说明  ：无
	====================================================================
	*/
	CAddrEntry( const TADDRENTRYINFO &tAddrEntryInfo );

	/*
	====================================================================
    功能        ：带地址簿条目对象参数的构造函数
    输入参数说明：地址条目对象
    返回值说明  ：无
	====================================================================
	*/
	CAddrEntry( const CAddrEntry &cNewAddrEntry );

	/*
	====================================================================
    功能        ：析构函数
    输入参数说明：无
    返回值说明  ：无
	====================================================================
	*/
	~CAddrEntry();

	/*
	====================================================================
    功能        ：赋值重载函数
    输入参数说明：
    返回值说明  ：无
	====================================================================
	*/
	CAddrEntry &operator = ( const CAddrEntry &cOtherAddrEntry );

	// get entry
	/*
	====================================================================
    功能        ：获取地址条目字节序
    输入参数说明：无
    返回值说明  ：当前条目的字节序
	====================================================================
	*/
	const u8 GetEntryByteOrder( void ) const
		{ return m_byByteOrder; }
	/*
	====================================================================
    功能        ：获取地址条目索引(地址条目索引用户不能修改)
    输入参数说明：无
    返回值说明  ：地址条目的物理位置索引
	====================================================================
	*/
	const u32 GetEntryIndex( void ) const
		{ return m_dwAddrEntryIndex; }

	/*
	====================================================================
    功能        ：设置地址条目索引
    输入参数说明：地址薄条目索引
    返回值说明  ：地址条目的物理位置索引
	====================================================================
	*/
    const void SetEntryIndex(u32 AddrEntryIndex)
    {
        m_dwAddrEntryIndex = AddrEntryIndex;
    }
	/*
	====================================================================
    功能        ：获取地址条目信息
    输入参数说明：存放条目信息的缓存区
    返回值说明  ：ptAddrEntryInfo -- 条目信息
	====================================================================
	*/
	void GetEntryInfo( TADDRENTRYINFO *ptAddrEntryInfo );

	/*
	====================================================================
    功能        ：获取地址条目名称
    输入参数说明：存放条目名称的缓存区 + 缓存区长度
    返回值说明  ：pchEntryName -- 条目名称
	====================================================================
	*/
	void GetEntryName( char* pchEntryName, u32 dwNameLen );

	/*
	====================================================================
    功能        ：获取地址条目的H323别名
    输入参数说明：存放H323别名的缓存区 + 缓存区长度
    返回值说明  ：pchMtAlias -- 终端别名
	====================================================================
	*/
	void GetMtAlias( char* pchMtAlias, u32 dwAliasLen );

	/*
	====================================================================
    功能        ：获取地址条目的IP地址
    输入参数说明：存放IP地址的缓存区 + 缓存区长度
    返回值说明  ：pchMtIpAddr -- 终端IP地址
	====================================================================
	*/
	 void GetMtIpAddr( char* pchMtIpAddr, u32 dwIpAddrLen );
	 u32  GetMtIpAddr() const
	{
		return this->m_tAddrEntryInfo.dwMtIpAddr;
	}
	/*
	====================================================================
    功能        ：获取地址条目的号码
    输入参数说明：存放号码的缓存区 + 缓存区长度
    返回值说明  ：pchMtNumber -- 终端号码
	====================================================================
	*/
	void GetMtNumber( char* pchMtNumber, u32 dwNumberLen );
	
	/*
	====================================================================
    功能        ：获取地址条目的呼叫速率
    输入参数说明：无
    返回值说明  ：呼叫速率（kbps）
	====================================================================
	*/
	const u16 GetCallRate( void ) 
		{ return m_tAddrEntryInfo.wCallRate; }

	/*
	====================================================================
    功能        ：获取地址条目名称长度
    输入参数说明：无
    返回值说明  ：地址条目名称长度
	====================================================================
	*/
	const u32 GetEntryNameLen( void ) 
		{ return strlen( m_tAddrEntryInfo.achEntryName ); }

	/*
	====================================================================
    功能        ：获取地址条目H323别名长度
    输入参数说明：无
    返回值说明  ：H323别名长度
	====================================================================
	*/
	const u32 GetMtAliasLen( void ) 
		{ return strlen( m_tAddrEntryInfo.achMtAlias ); }

	/*
	====================================================================
    功能        ：获取地址条目电话号码长度
    输入参数说明：无
    返回值说明  ：电话号码长度
	====================================================================
	*/
	const u32 GetMtNumberLen( void ) 
		{ return strlen( m_tAddrEntryInfo.achMtNumber ); }

	/*
	====================================================================
    功能        ：获取H320地址条目
    输入参数说明：存放H320号码的缓存区 + 缓存区长度
    返回值说明  ：pchH320MtId -- 终端号码
	====================================================================
	*/
	void GetH320MtId( char* pchH320MtId, u32 dwIdLen ); 

	
	/*
	====================================================================
    功能        ：获取H320地址条目的长度
    输入参数说明：无
    返回值说明  ：H320地址条目的长度
	====================================================================
	*/
	const u32  GetH320MtIdLen(void) 
	{ return strlen( m_tAddrEntryInfo.achH320Id); }
	
	
	/*
	====================================================================
    功能        ：获取地址条目H323别名长度
    输入参数说明：无
    返回值说明  ：H323别名长度
	====================================================================
	*/
	 bool IsH320Terminal( void )
		{ return  m_tAddrEntryInfo.bH320Terminal != (u8)0; }

	/*
	====================================================================
    功能        ：设置地址簿基本信息结构
    输入参数说明：地址簿信息结构
    返回值说明  ：无
	====================================================================
	*/
	void SetEntryInfo( const TADDRENTRYINFO &tAddrEntryInfo );

	/*
	====================================================================
    功能        ：设置条目名称
    输入参数说明：名称
    返回值说明  ：成功与否
	====================================================================
	*/
	BOOL SetEntryName( const char* pchEntryName );

	/*
	====================================================================
    功能        ：设置终端别名
    输入参数说明：别名信息
    返回值说明  ：成功与否
	====================================================================
	*/
	BOOL SetMtAlias( const char* pchMtAlias );

	/*
	====================================================================
    功能        ：设置终端IP地址
    输入参数说明：IP地址(字符串)
    返回值说明  ：成功与否
	====================================================================
	*/
	BOOL SetMtIpAddr( const char* pchMtIpAddr );

	/*
	====================================================================
    功能        ：设置终端号码
    输入参数说明：终端号码(字符串)
    返回值说明  ：成功与否
	====================================================================
	*/
	BOOL SetMtNumber( const char* pchMtNumber );

	/*
	====================================================================
    功能        ：设置终端呼叫速率
    输入参数说明：呼叫速率
    返回值说明  ：无
	====================================================================
	*/
	void SetCallRate( u16 wCallRate ) 
		{ m_tAddrEntryInfo.wCallRate = wCallRate; } 

	/*
	====================================================================
    功能        ：设置H320地址条目
    输入参数说明：H320号码
    返回值说明  ：成功与否
	====================================================================
	*/
	BOOL SetH320MtId( const char* pchH320MtId); 
	
	/*
	====================================================================
    功能        ：设置该地址条目为一个H320/H323的地址条目
    输入参数说明：Ture：设置为H320类型的终端；False:设置为H323类型的终端。
    返回值说明  ：无
	====================================================================
	*/
	void SetTerminalType( const bool bH320)
	{ (bH320 ? (m_tAddrEntryInfo.bH320Terminal = (u8)1) : (m_tAddrEntryInfo.bH320Terminal = (u8)0));}

	// print for test
	/*
	====================================================================
    功能        ：打印本地址簿条目信息(用于测试)
    输入参数说明：无
    返回值说明  ：无
	====================================================================
	*/
	void PrintEntry( )
	{
		OspPrintf( TRUE, FALSE, "======================\n" );
		OspPrintf( TRUE, FALSE, "Index   : %d\n", m_dwAddrEntryIndex );
        m_tAddrEntryInfo.print();
		
// 		if ( m_tAddrEntryInfo.bH320Terminal != (u8)0 )
// 		{
// 			OspPrintf( TRUE, FALSE, "H320Id : %s\n", m_tAddrEntryInfo.achH320Id);
// 			OspPrintf( TRUE, FALSE, "======================\n" );
// 			return;
// 		}
// 		
// 		if ( m_tAddrEntryInfo.achEntryName == NULL )
// 			OspPrintf( TRUE, FALSE, "Name    : 0\n" );
// 		else
// 			OspPrintf( TRUE, FALSE, "Name    : %s\n", m_tAddrEntryInfo.achEntryName );
// 
// 		if ( m_tAddrEntryInfo.achMtAlias == NULL )
// 			OspPrintf( TRUE, FALSE, "Alias	 : 0\n" );
// 		else
// 			OspPrintf( TRUE, FALSE, "Alias	 : %s\n", m_tAddrEntryInfo.achMtAlias );
// 
// 		if ( m_tAddrEntryInfo.achMtNumber == NULL )
// 			OspPrintf( TRUE, FALSE, "Number  : 0\n" );
// 		else
// 			OspPrintf( TRUE, FALSE, "Number  : %s\n", m_tAddrEntryInfo.achMtNumber );
// 
// 		OspPrintf( TRUE, FALSE, "IP      : 0x%x\n", m_tAddrEntryInfo.dwMtIpAddr );
// 		OspPrintf( TRUE, FALSE, "Rate    : 0x%x\n", m_tAddrEntryInfo.wCallRate );
        OspPrintf( TRUE, FALSE, "Byte Order is:%d\n", m_byByteOrder);

		OspPrintf( TRUE, FALSE, "======================\n" );

	}


	// compare
	/*
	====================================================================
    功能        ：小于重载
    输入参数说明：
    返回值说明  ：比较结果
	====================================================================
	*/
	BOOL operator < ( CAddrEntry &cAddrEntry )
	{
		int i = memcmp( &m_tAddrEntryInfo, 
						&cAddrEntry.m_tAddrEntryInfo,
						sizeof( m_tAddrEntryInfo ) );

		return ( i < 0 ) ? TRUE : FALSE; 
	}

	/*
	====================================================================
    功能        ：小于等于重载
    输入参数说明：无
    返回值说明  ：比较结果
	====================================================================
	*/
	BOOL operator <= ( CAddrEntry &cAddrEntry )
	{
		int i = memcmp( &m_tAddrEntryInfo, 
						&cAddrEntry.m_tAddrEntryInfo,
						sizeof( m_tAddrEntryInfo ) );
		return ( i <= 0 ) ? TRUE : FALSE; 
	}

	/*
	====================================================================
    功能        ：大于重载
    输入参数说明：无
    返回值说明  ：比较结果
	====================================================================
	*/
	BOOL operator > ( CAddrEntry &cAddrEntry )
	{
		int i = memcmp( &m_tAddrEntryInfo, 
						&cAddrEntry.m_tAddrEntryInfo,
						sizeof( m_tAddrEntryInfo ) );
		return ( i <= 0 ) ? FALSE : TRUE; 
	}

	/*
	====================================================================
    功能        ：大于等于重载
    输入参数说明：无
    返回值说明  ：比较结果
	====================================================================
	*/
	BOOL operator >= ( CAddrEntry &cAddrEntry )
	{
		int i = memcmp( &m_tAddrEntryInfo, 
						&cAddrEntry.m_tAddrEntryInfo,
						sizeof( m_tAddrEntryInfo ) );
		
		return ( i < 0 ) ? FALSE : TRUE; 
	}

	/*
	====================================================================
    功能        ：恒等于重载
    输入参数说明：无
    返回值说明  ：比较结果
	====================================================================
	*/ 
	BOOL operator == ( CAddrEntry &cAddrEntry )
	{
		if ( memcmp( &m_tAddrEntryInfo, &cAddrEntry.m_tAddrEntryInfo, 
										sizeof( m_tAddrEntryInfo ) ) == 0 )
			return TRUE;
		else
			return FALSE;
	}

	/*
	====================================================================
    功能        ：不等于
    输入参数说明：无
    返回值说明  ：比较结果
	====================================================================
	*/
	BOOL operator != ( CAddrEntry &cAddrEntry )
	{
		if ( memcmp( &m_tAddrEntryInfo, &cAddrEntry.m_tAddrEntryInfo,
			                     sizeof( m_tAddrEntryInfo ) ) !=0 ) 
			return TRUE;
		else
			return FALSE;
	}

	// order convert
	/*
	====================================================================
    功能        ：把当前地址簿条目转换为网络序
    输入参数说明：无
    返回值说明  ：比较结果
	====================================================================
	*/
	void OrderHton( /*BOOL32 bIsNoChangeRateOrter = TRUE*/ );

	/*
	====================================================================
    功能        ：把当前地址簿条目转换为机器序
    输入参数说明：无
    返回值说明  ：比较结果
	====================================================================
	*/
	void OrderNtoh( BOOL32 bIsNoChangeRateOrter = TRUE );

private :
	u32			m_dwAddrEntryIndex;		// 条目索引
	TADDRENTRYINFO	m_tAddrEntryInfo;		// 条目相关信息
	u8			m_byByteOrder;			// 当前字节序
	
	
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

// 会议组
//////////////////////////////////////////////////////////////////////////

class CAddrMultiSetEntry
{
	friend class CAddrEntry;
	friend class CAddrBook;

public :
	// constructor & destructor & copy-constructor & assign
	/*
	====================================================================
    功能        ：无参构造函数
    输入参数说明：无
    返回值说明  ：无
	====================================================================
	*/
	CAddrMultiSetEntry();

	/*
	====================================================================
    功能        ：带参构造函数
    输入参数说明：会议组信息结构
    返回值说明  ：无
	====================================================================
	*/
	CAddrMultiSetEntry( const TMULTISETENTRYINFO &tMultiSetEntryInfo );

	/*
	====================================================================
    功能        ：带参构造函数
    输入参数说明：会议组对象
    返回值说明  ：无
	====================================================================
	*/
	CAddrMultiSetEntry( const CAddrMultiSetEntry &cNewMultiSetEntry );

	/*
	====================================================================
    功能        ：析构函数
    输入参数说明：无
    返回值说明  ：无
	====================================================================
	*/
	~CAddrMultiSetEntry();

	/*
	====================================================================
    功能        ：赋值重载
    输入参数说明：无
    返回值说明  ：无
	====================================================================
	*/
	CAddrMultiSetEntry &operator = ( const CAddrMultiSetEntry &cOtherMultiSetEntry );


	// get
	/*
	====================================================================
    功能        ：获取会议组字节序
    输入参数说明：无
    返回值说明  ：当前会议组的字节序
	====================================================================
	*/
	const u8 GetGroupByteOrder( void ) const
		{ return m_byByteOrder; }

	/*
	====================================================================
    功能        ：获取本会议组的物理索引号
    输入参数说明：无
    返回值说明  ：会议组索引号
	====================================================================
	*/
	const u32 GetGroupIndex( void ) const
		{ return m_dwMultiSetEntryIndex; }

	/*
	====================================================================
    功能        ：获取会议组信息
    输入参数说明：存放会议组信息的结构
    返回值说明  ：ptMultiSetEntryInfo -- 会议组信息
	====================================================================
	*/
	void GetGroupInfo( TMULTISETENTRYINFO *ptMultiSetEntryInfo );

	/*
	====================================================================
    功能        ：获取会议组名称
    输入参数说明：存放会议组名称的缓存区 + 缓存区长度
    返回值说明  ：pchGroupName -- 会议组名称
	====================================================================
	*/
	void GetGroupName( char* pchGroupName, u32 dwNameLen );

	/*
	====================================================================
    功能        ：获取会议组MCU地址
    输入参数说明：存放IP地址的缓存区 + 缓存区长度
    返回值说明  ：pchMcuIpAddr -- 终端IP地址
	====================================================================
	*/
	void GetMcuIpAddr( char* pchMcuIpAddr, u32 dwIpAddrLen );

	/*
	====================================================================
    功能        ：获取会议速率
    输入参数说明：无
    返回值说明  ：会议速率
	====================================================================
	*/
	u16 GetConfBitRate( void ) 
		{ return m_wConfBitRate; }

    /*
	====================================================================
    功能        ：获取用户数据长度
    输入参数说明：无
    返回值说明  ：用户数据长度
	====================================================================
	*/
    u32 GetUserDataLen();

    /*
	====================================================================
    功能        ：获取用户数据
    输入参数说明：无
    返回值说明  ：成功TRUE
	====================================================================
	*/
	BOOL GetUserData(u8* pbyBuf, u32 dwBufLen);

	/*
	====================================================================
    功能        ：获取会议开放属性
    输入参数说明：无
    返回值说明  ：开放属性(0:不开放, 1: 开放)
	====================================================================
	*/
	u8 GetConfOpen( void ) 
		{ return m_byOpen; }

	/*
	====================================================================
    功能        ：获取会议组名称长度
    输入参数说明：无
    返回值说明  ：名称长度
	====================================================================
	*/
	u32 GetGroupNameLen( void ) 
		{ return strlen( m_achGroupName ); }

	// get mttable
	/*
	====================================================================
    功能        ：获取会议组终端列表
    输入参数说明：无
    返回值说明  ：终端列表头指针
	====================================================================
	*/
	u32* GetEntryTable( void ) 
			{ return m_adwConfEntryTable; }

	/*
	====================================================================
    功能        ：获取会议组终端个数(终端列表长度)
    输入参数说明：无
    返回值说明  ：列表长度
	====================================================================
	*/
	const u8 GetEntryCount( void );
	
	// Set
	/*
	====================================================================
    功能        ：设置会议组信息
    输入参数说明：会议组信息结构
    返回值说明  ：无
	====================================================================
	*/
	void SetGroupInfo( const TMULTISETENTRYINFO &tMultiSetEntryInfo );

	/*
	====================================================================
    功能        ：设置会议名称
    输入参数说明：会议组名称
    返回值说明  ：成功与否
	====================================================================
	*/
	BOOL SetGroupName( const char* pchGroupName );
	/*
	====================================================================
    功能        ：设置MCU IP地址
    输入参数说明：IP地址
    返回值说明  ：成功与否
	====================================================================
	*/
	BOOL SetMcuIpAddr( const char* pchIpAddr );


	/*
	====================================================================
    功能        ：设置会议速率
    输入参数说明：会议速率
    返回值说明  ：无
	====================================================================
	*/
	void SetConfBitRate( const u16 wConfBitRate ) 
		{ m_wConfBitRate = wConfBitRate; }

	/*
	====================================================================
    功能        ：设置会议的开放属性
    输入参数说明：开放属性(0: 不开放,1: 开放)
    返回值说明  ：无
	====================================================================
	*/
	void SetConfOpen( const u8 byOpen )
		{ m_byOpen = byOpen; }

    /*
	====================================================================
    功能        ：设置用户数据
    输入参数说明：无
    返回值说明  ：会议速率
	====================================================================
	*/
	BOOL SetUserData(u8* pbyBuf, u32 dwBufLen);

	/*
	====================================================================
    功能        ：向会议组的终端列表中加入终端
    输入参数说明：终端索引
    返回值说明  ：成功与否
	====================================================================
	*/
	BOOL AddEntryToMultiSetList( const u32 dwEntryIndex );


	// compare
	/*
	====================================================================
    功能        ：小于重载
    输入参数说明：无
    返回值说明  ：比较结果
	====================================================================
	*/
	BOOL operator < ( CAddrMultiSetEntry &cAddrMultiSetEntry )
	{
        int i = memcmp(m_achGroupName, 
                       cAddrMultiSetEntry.m_achGroupName, 
                       sizeof( m_achGroupName ));

		return ( i < 0 ) ? TRUE : FALSE; 
	}

	/*
	====================================================================
    功能        ：小于等于重载
    输入参数说明：无
    返回值说明  ：比较结果
	====================================================================
	*/
	BOOL operator <= ( CAddrMultiSetEntry &cAddrMultiSetEntry )
	{
        int i = memcmp(m_achGroupName, 
                       cAddrMultiSetEntry.m_achGroupName, 
                       sizeof( m_achGroupName ));
        
        return ( i <= 0 ) ? TRUE : FALSE; 
	}

	/*
	====================================================================
    功能        ：大于重载
    输入参数说明：无
    返回值说明  ：比较结果
	====================================================================
	*/
	BOOL operator > ( CAddrMultiSetEntry &cAddrMultiSetEntry )
	{
        int i = memcmp(m_achGroupName, 
                       cAddrMultiSetEntry.m_achGroupName, 
                       sizeof( m_achGroupName ));
        
        return ( i > 0 ) ? TRUE : FALSE; 
    }
    
	/*
	====================================================================
    功能        ：大于等于
    输入参数说明：无
    返回值说明  ：比较结果
	====================================================================
	*/
	BOOL operator >= ( CAddrMultiSetEntry &cAddrMultiSetEntry )
	{
        int i = memcmp(m_achGroupName, 
            cAddrMultiSetEntry.m_achGroupName, 
            sizeof(m_achGroupName));
        
        return ( i >= 0 ) ? TRUE : FALSE; 
    }
    
	/*
	====================================================================
    功能        ：恒等于重载
    输入参数说明：无
    返回值说明  ：比较结果
	====================================================================
	*/ 
	BOOL operator == ( CAddrMultiSetEntry &cAddrMultiSetEntry )
	{
        if ( memcmp(m_achGroupName, 
					cAddrMultiSetEntry.m_achGroupName, 
					sizeof(m_achGroupName) ) == 0 )
			return TRUE;
		else
			return FALSE;
	}

	/*
	====================================================================
    功能        ：不等于重载
    输入参数说明：无
    返回值说明  ：比较结果
	====================================================================
	*/
	BOOL operator != ( CAddrMultiSetEntry &cAddrMultiSetEntry )
	{
        if (memcmp(m_achGroupName, 
                   cAddrMultiSetEntry.m_achGroupName, 
				   sizeof(m_achGroupName)) != 0 )
			return TRUE;
		else
			return FALSE;
	}

	/*
	====================================================================
    功能        ：打印本会议组信息(用于测试)
    输入参数说明：无
    返回值说明  ：无
	====================================================================
	*/
	void PrintGroup( void )
	{
		OspPrintf( TRUE, FALSE, "index  : %d\n", m_dwMultiSetEntryIndex );

		if ( m_achGroupName != NULL )
			OspPrintf( TRUE, FALSE, "Name   : %s\n", m_achGroupName );
		else
			OspPrintf( TRUE, FALSE, "Name   : \n" );

		OspPrintf( TRUE, FALSE, "McuIp  : 0x%x\n", m_dwMcuIpAddr );
		OspPrintf( TRUE, FALSE, "Rate   : %d\n", m_wConfBitRate );
		OspPrintf( TRUE, FALSE, "Open   : %d\n", m_byOpen );
        OspPrintf( TRUE, FALSE, "Userlen: %d\n", m_dwUserDataLen);
		OspPrintf( TRUE, FALSE, "Entry Table, size = %d\n", GetEntryCount() );
		for ( int i = 0; i < GetEntryCount(); i++ )
			OspPrintf(TRUE, FALSE,  "%d ,", m_adwConfEntryTable[ i ] );
		OspPrintf( TRUE, FALSE, "\n" );
	}

	// order convert
	/*
	====================================================================
    功能        ：把当前会议组转换为网络序
    输入参数说明：无
    返回值说明  ：比较结果
	====================================================================
	*/
	void OrderHton( void );

	/*
	====================================================================
    功能        ：把当前会议组转换为机器序
    输入参数说明：无
    返回值说明  ：比较结果
	====================================================================
	*/
	void OrderNtoh( void );

private :
	u32					m_dwMultiSetEntryIndex;						// 会议组索引
	//TMULTISETENTRYINFO	m_tMultiSetEntryInfo;						// 会议组信息
    //组信息
    char m_achGroupName[ MAX_NAME_LEN + 1 ];	// 组名
    u32	m_dwMcuIpAddr;						// MCU地址(网络序)
    u16	m_wConfBitRate;						// 会议速率
    u8	m_byOpen;						    // 开放性(0:开放	1:非开放)

	u32	m_adwConfEntryTable[ MAXNUM_ENTRY_TABLE ];	// 终端列表
	u8	m_byByteOrder;								// 当前字节序

    u32 m_dwUserDataLen;                            // 用户数据长度
    u8  m_byUserData[MAX_GROUP_USERDATA_LEN];                          // 用户数据256字节
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

struct tagAddrEntry
{
private :
	u32 dwEntryIndex;				// 条目索引
	TADDRENTRYINFO tAddrEntryInfo;	// 条目信息

public :
	tagAddrEntry() : dwEntryIndex( 0xFF )
	{
		memset( &tAddrEntryInfo, 0, sizeof( tAddrEntryInfo ) );
	}
    void print()
    {
        OspPrintf( TRUE, FALSE, "======================\n" );
        OspPrintf( TRUE, FALSE, "Index   : %d\n", dwEntryIndex );
        
        tAddrEntryInfo.print();
            
        OspPrintf( TRUE, FALSE, "======================\n" );
    }

    /*
    ====================================================================
    功能        ：获取条目索引
    输入参数说明：无
    返回值说明  ：条目索引
    ====================================================================
    */	
    u32 GetEntryIndex()
    { return ntohl( dwEntryIndex ); }
    
    /*
    ====================================================================
    功能        ：设置条目索引
    输入参数说明：无
    返回值说明  ：无
    ====================================================================
    */
    void SetEntryIndex( u32 dwIndex )
    { dwEntryIndex = htonl( dwIndex ); }

	/*
	====================================================================
    功能        ：获取条目信息
    输入参数说明：无
    返回值说明  ：条目信息结构
	====================================================================
	*/
	TADDRENTRYINFO GetEntryInfo()
	{
		TADDRENTRYINFO tTempAddrEntryInfo;
		memset( &tTempAddrEntryInfo, 0, sizeof( tTempAddrEntryInfo ) );
		memcpy( &tTempAddrEntryInfo, &tAddrEntryInfo, sizeof( tTempAddrEntryInfo ) );
		tTempAddrEntryInfo.wCallRate = ntohs( tTempAddrEntryInfo.wCallRate );

		return tTempAddrEntryInfo;
	}

	/*
	====================================================================
    功能        ：设置条目信息
    输入参数说明：无
    返回值说明  ：无
	====================================================================
	*/
	void SetEntryInfo( TADDRENTRYINFO *ptAddrEntryInfo )
	{
		if ( ptAddrEntryInfo == NULL )
			return;

		memset( &tAddrEntryInfo, 0, sizeof( tAddrEntryInfo ) );
		memcpy( &tAddrEntryInfo, ptAddrEntryInfo, sizeof( tAddrEntryInfo ) );
		tAddrEntryInfo.wCallRate = htons( tAddrEntryInfo.wCallRate );

	}

    //下面获取条目信息不做字节序转换，内存中保存的都为网络序
    //上面接口保留，因为有可能其他模块使用
    TADDRENTRYINFO GetEntry()
    {
        TADDRENTRYINFO tTempAddrEntryInfo;
        memset( &tTempAddrEntryInfo, 0, sizeof( tTempAddrEntryInfo ) );
        memcpy( &tTempAddrEntryInfo, &tAddrEntryInfo, sizeof( tTempAddrEntryInfo ) );
        
        return tTempAddrEntryInfo;
	}

    void SetEntry( TADDRENTRYINFO *ptAddrEntryInfo )
    {
        if ( ptAddrEntryInfo == NULL )
            return;
        
        memset( &tAddrEntryInfo, 0, sizeof( tAddrEntryInfo ) );
        memcpy( &tAddrEntryInfo, ptAddrEntryInfo, sizeof( tAddrEntryInfo ) );
	}
	
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;
typedef tagAddrEntry TADDRENTRY;
typedef TADDRENTRY *PTADDRENTRY;


struct tagAddrMultiSetEntry
{
private :
	u32 dwEntryIndex;								// 条目索引
	TMULTISETENTRYINFO tAddrMultiSetEntryInfo;		// 条目信息
	u32 dwListEntryNum;							// 与会终端个数
	u32 adwListEntryIndex[ MAXNUM_ENTRY_TABLE ];	// 与会终端索引

public :
	tagAddrMultiSetEntry():dwEntryIndex(0xFF),dwListEntryNum(0xFF)
	{
		memset( &tAddrMultiSetEntryInfo, 0, sizeof( tAddrMultiSetEntryInfo ) );
		memset( adwListEntryIndex, 0xFF, MAXNUM_ENTRY_TABLE * sizeof( u32 ) );
	}
	/*
	====================================================================
    功能        ：获取条目索引
    输入参数说明：无
    返回值说明  ：条目条目索引
	====================================================================
	*/
	u32 GetEntryIndex()
		{ return ntohl( dwEntryIndex ); }

	/*
	====================================================================
    功能        ：设置条目索引
    输入参数说明：无
    返回值说明  ：无
	====================================================================
	*/
	void SetEntryIndex( u32 dwIndex )
		{ dwEntryIndex = htonl( dwIndex ); }

	/*
	====================================================================
    功能        ：获取条目信息
    输入参数说明：无
    返回值说明  ：条目信息结构
	====================================================================
	*/
	TMULTISETENTRYINFO GetEntryInfo()
	{
		TMULTISETENTRYINFO tTempAddrMultiSetEntryInfo;
		memset( &tTempAddrMultiSetEntryInfo, 0, sizeof( tTempAddrMultiSetEntryInfo ) );
		memcpy( &tTempAddrMultiSetEntryInfo, &tAddrMultiSetEntryInfo, 
					sizeof( tTempAddrMultiSetEntryInfo ) );
		tTempAddrMultiSetEntryInfo.wConfBitRate = ntohs( tTempAddrMultiSetEntryInfo.wConfBitRate );
        tTempAddrMultiSetEntryInfo.dwUserDataLen = ntohl( tTempAddrMultiSetEntryInfo.dwUserDataLen );

		return tTempAddrMultiSetEntryInfo;
	}

	/*
	====================================================================
    功能        ：设置条目信息
    输入参数说明：无
    返回值说明  ：无
	====================================================================
	*/
	void SetEntryInfo( TMULTISETENTRYINFO *ptAddrMultiSetEntryInfo )
	{
		if ( ptAddrMultiSetEntryInfo == NULL )
			return;

		memset( &tAddrMultiSetEntryInfo, 0, sizeof( tAddrMultiSetEntryInfo ) );
		memcpy( &tAddrMultiSetEntryInfo, ptAddrMultiSetEntryInfo, sizeof( tAddrMultiSetEntryInfo ) );
		tAddrMultiSetEntryInfo.wConfBitRate = htons( tAddrMultiSetEntryInfo.wConfBitRate );
        tAddrMultiSetEntryInfo.dwUserDataLen = htonl( tAddrMultiSetEntryInfo.dwUserDataLen );
	}

	/*
	====================================================================
    功能        ：获取与会列表终端个数
    输入参数说明：无
    返回值说明  ：终端个数
	====================================================================
	*/
	u32 GetListEntryNum()
		{ return ntohl( dwListEntryNum ); }

	/*
	====================================================================
    功能        ：设置与会列表终端个数
    输入参数说明：无
    返回值说明  ：无
	====================================================================
	*/
	void SetListEntryNum( u32 dwNum )
	{ 
        //guzh 2008/06/06
        if ( dwNum > MAXNUM_ENTRY_TABLE )
			dwNum = MAXNUM_ENTRY_TABLE;

        dwListEntryNum = htonl( dwNum ); 
    }

	/*
	====================================================================
    功能        ：获取与会列表
    输入参数说明：存放与会列表的缓冲区 + 缓冲区长度
    返回值说明  ：无
	====================================================================
	*/
	void GetListEntryIndex( u32* padwListEntryIndex, u32 dwNum )
	{
		if ( dwNum > MAXNUM_ENTRY_TABLE )
			dwNum = MAXNUM_ENTRY_TABLE;

		for ( int i = 0; i < (int)dwNum; i++ )
			padwListEntryIndex[ i ] = ntohl( adwListEntryIndex[ i ] );
	}

	/*
	====================================================================
    功能        ：设置与会列表
    输入参数说明：存放与会列表的缓冲区 + 与会终端个数
    返回值说明  ：无
	====================================================================
	*/	
	void SetListEntryIndex( u32* padwListEntryIndex, u32 dwNum )
	{
		int i;
        for ( i = 0; i < MAXNUM_ENTRY_TABLE; i++ )
			adwListEntryIndex[ i ] = INVALID_INDEX;

		if ( dwNum > MAXNUM_ENTRY_TABLE )
			dwNum = MAXNUM_ENTRY_TABLE;

		for ( i = 0; i < (int)dwNum; i++ )
		{
			adwListEntryIndex[ i ] = htonl( *( padwListEntryIndex + i ) );
		}
	}
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;
typedef tagAddrMultiSetEntry TADDRMULTISETENTRY;
typedef TADDRMULTISETENTRY *PTADDRMULTISETENTRY;

// 条目类型 + 条目个数 + 操作类型 + 条目索引 + 消息长度 + 消息体
const u16 ADDRBOOK_MSGHEAD_LEN					= 12;
const u16 ADDRBOOK_MSGHEAD_ENTRYTYPE_OFFSET	= 0;
const u16 ADDRBOOK_MSGHEAD_ENTRYTYPE_LEN		= 1;
const u16 ADDRBOOK_MSGHEAD_NUM_OFFSET			= ADDRBOOK_MSGHEAD_ENTRYTYPE_OFFSET +  ADDRBOOK_MSGHEAD_ENTRYTYPE_LEN;
const u16 ADDRBOOK_MSGHEAD_NUM_LEN				= 4;
const u16 ADDRBOOK_MSGHEAD_OPTTYPE_OFFSET		= ADDRBOOK_MSGHEAD_NUM_OFFSET + ADDRBOOK_MSGHEAD_NUM_LEN;
const u16 ADDRBOOK_MSGHEAD_OPTTYPE_LEN			= 1;
const u16 ADDRBOOK_MSGHEAD_ENTRYINDEX_OFFSET	= ADDRBOOK_MSGHEAD_OPTTYPE_OFFSET + ADDRBOOK_MSGHEAD_OPTTYPE_LEN;
const u16 ADDRBOOK_MSGHEAD_ENTRYINDEX_LEN		= 4;
const u16 ADDRBOOK_MSGHEAD_MSGLEN_OFFSET		= ADDRBOOK_MSGHEAD_ENTRYINDEX_OFFSET + ADDRBOOK_MSGHEAD_ENTRYINDEX_LEN;
const u16 ADDRBOOK_MSGHEAD_MSGLEN_LEN			= 2;
const u16 ADDRBOOK_MSGBODY_OFFSET				= ADDRBOOK_MSGHEAD_LEN;

class CAddrBookMsg
{
protected :
	u8	m_abyBuffer[ 0x1800 ];			// buff
	
private :

public :
	CAddrBookMsg()
	{
		memset( m_abyBuffer, 0, sizeof( m_abyBuffer ) );
		u16 wLen = ADDRBOOK_MSGHEAD_LEN;
		wLen = htons( wLen );
		memcpy( m_abyBuffer + ADDRBOOK_MSGHEAD_MSGLEN_OFFSET, &wLen,
					ADDRBOOK_MSGHEAD_MSGLEN_LEN );
	}
	
	// get & set
	/*
	====================================================================
    功能        ：获取条目类型字段
    输入参数说明：无
    返回值说明  ：条目类型
	====================================================================
	*/
	u8 GetEntryType() const 
	{ 	
		return ( *( u8* )( m_abyBuffer + ADDRBOOK_MSGHEAD_ENTRYTYPE_OFFSET ) ); 
	}

	/*
	====================================================================
    功能        ：设置条目类型字段
    输入参数说明: 条目类型
    返回值说明  ：无
	====================================================================
	*/
	void SetEntryType( u8 byEntryType )
	{
		*( m_abyBuffer + ADDRBOOK_MSGHEAD_ENTRYTYPE_OFFSET ) = byEntryType;
	}
	

	/*
	====================================================================
    功能        ：获取条目个数字段
    输入参数说明：无
    返回值说明  ：条目个数
	====================================================================
	*/
	u32 GetEntryNum() const
	{ 
		u32 dwEntryNum = 0;
		
		memcpy( &dwEntryNum, m_abyBuffer + ADDRBOOK_MSGHEAD_NUM_OFFSET, 
					ADDRBOOK_MSGHEAD_NUM_LEN );
		return ntohl( dwEntryNum ); 
	}

	/*
	====================================================================
    功能        ：设置条目个数字段
    输入参数说明：条目个数
    返回值说明  ：无
	====================================================================
	*/
	void SetEntryNum( u32 dwEntryNum )
	{ 
		dwEntryNum = htonl( dwEntryNum ); 
		memcpy( m_abyBuffer + ADDRBOOK_MSGHEAD_NUM_OFFSET, &dwEntryNum,
				 ADDRBOOK_MSGHEAD_NUM_LEN );
	}

	/*
	====================================================================
    功能        ：获取条目操作类型字段
    输入参数说明：无
    返回值说明  ：条目操作类型
	====================================================================
	*/
	u8 GetEntryOptType() const
	{ 	
		return ( *( u8* )( m_abyBuffer + ADDRBOOK_MSGHEAD_OPTTYPE_OFFSET ) ); 
	}

	/*
	====================================================================
    功能        ：设置条目操作类型字段
    输入参数说明：操作类型(增加/删除)
    返回值说明  ：无
	====================================================================
	*/
	void SetEntryOptType( u8 byOptType )
	{
		*( m_abyBuffer + ADDRBOOK_MSGHEAD_OPTTYPE_OFFSET ) = byOptType;
	}

	/*
	====================================================================
    功能        ：获取条目索引字段
    输入参数说明：无
    返回值说明  ：条目索引
	====================================================================
	*/
	u32 GetEntryIndex() const
	{ 
		u32 dwEntryIndex = 0;
		
		memcpy( &dwEntryIndex, m_abyBuffer + ADDRBOOK_MSGHEAD_ENTRYINDEX_OFFSET, 
					ADDRBOOK_MSGHEAD_ENTRYINDEX_LEN );
		return ntohl( dwEntryIndex ); 
	}

	/*
	====================================================================
    功能        ：设置条目索引字段
    输入参数说明：条目索引
    返回值说明  ：无
	====================================================================
	*/
	void SetEntryIndex( u32 dwIndex )
	{ 
		dwIndex = htonl( dwIndex ); 
		memcpy( m_abyBuffer + ADDRBOOK_MSGHEAD_ENTRYINDEX_OFFSET, &dwIndex,
				 ADDRBOOK_MSGHEAD_ENTRYINDEX_LEN );
	}

	/*
	====================================================================
    功能        ：获取整个通信消息的长短
    输入参数说明：无
    返回值说明  ：消息长度
	====================================================================
	*/
	u16 GetMsgLen() const 
	{
		u16 wMsgLen = 0;

		memcpy( &wMsgLen, m_abyBuffer + ADDRBOOK_MSGHEAD_MSGLEN_OFFSET,
				ADDRBOOK_MSGHEAD_MSGLEN_LEN	);
		
		return ntohs( wMsgLen );
	}

    /*
	====================================================================
    功能        ：获取消息体长度
    输入参数说明：无
    返回值说明  ：消息体长度
	====================================================================
	*/
    u16 GetMsgBodyLen() const
    {        
		return GetMsgLen() - ADDRBOOK_MSGHEAD_LEN;
    }

	/*
	====================================================================
    功能        ：设置通信消息体
    输入参数说明：消息体缓冲区 + 缓冲区长度
    返回值说明  ：无
	====================================================================
	*/
	void SetMsgBody( const u8 *pbyMsgBody = NULL, u16 wLen = 0 )
	{
		wLen = (u16)min( wLen, sizeof( m_abyBuffer ) - ADDRBOOK_MSGHEAD_LEN );
		memcpy( m_abyBuffer + ADDRBOOK_MSGBODY_OFFSET, pbyMsgBody, wLen );

		wLen = htons( wLen + ADDRBOOK_MSGHEAD_LEN );
		memcpy( m_abyBuffer + ADDRBOOK_MSGHEAD_MSGLEN_OFFSET, &wLen,
					ADDRBOOK_MSGHEAD_MSGLEN_LEN );
	}

	/*
	====================================================================
    功能        ：获取通信消息体
    输入参数说明：无
    返回值说明  ：通信消息体头指针
	====================================================================
	*/
	u8 *GetMsgBody( void ) const
	{
		return ( ( u8* )( m_abyBuffer + ADDRBOOK_MSGBODY_OFFSET ) );
	}

	/*
	====================================================================
    功能        ：获取整个通信消息缓冲区
    输入参数说明：无
    返回值说明  ：缓冲区头指针
	====================================================================
	*/
	u8 *GetAddrBookMsg( void ) const
	{
		return ( ( u8* )m_abyBuffer );
	}

	/*
	====================================================================
    功能        ：设置整个通信消息的缓冲区
    输入参数说明：缓冲区 + 缓冲区长度
    返回值说明  ：无
	====================================================================
	*/
	void SetAddrBookMsg( const u8 *pbyMsg, u16 wMsgLen )
	{
		if( wMsgLen < ADDRBOOK_MSGHEAD_LEN )
		{
//			OspPrintf( TRUE, FALSE, "CAddrBookMsg: Exception -- invalid addrbook message!\n" );
			return;
		}

		wMsgLen = min( wMsgLen, sizeof( m_abyBuffer ) );
		memcpy( m_abyBuffer, pbyMsg, wMsgLen );
// 		memcpy( m_abyBuffer + ADDRBOOK_MSGHEAD_MSGLEN_OFFSET, &wMsgLen,
// 					ADDRBOOK_MSGHEAD_MSGLEN_LEN );
	}
	
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

// 地址薄
//////////////////////////////////////////////////////////////////////////////////////
class CAddrBook
{	
public :
/*
	enum EntryType
	{
		ADDR_ENTRY,
		MULTISET_ENTRY
	};
*/


	// constructor,destructor,copy-constructor, assign
	/*
	====================================================================
    功能        ：带参构造函数
    输入参数说明：最大地址簿条目个数 + 最大会议组个数 + 编码方式
    返回值说明  ：无
	====================================================================
	*/
	CAddrBook( u32 dwAddrEntrySize = MAX_ADDRENTRY,
			   u32 dwAddrGroupSize = MAX_ADDRGROUP, 
               u8 byCodingMode = emCodingModeGBK);

	/*
	====================================================================
    功能        ：带参构造函数
    输入参数说明：地址簿对象
    返回值说明  ：无
	====================================================================
	*/
	CAddrBook( const CAddrBook &cNewAddrBook );

	/*
	====================================================================
    功能        ：赋值重载
    输入参数说明：无
    返回值说明  ：无
	====================================================================
	*/
	CAddrBook &operator = ( const CAddrBook &cOtherAddrBook );

	/*
	====================================================================
    功能        ：析构函数
    输入参数说明：无
    返回值说明  ：无
	====================================================================
	*/
	~CAddrBook();

	// 获取地址簿相关属性
	//////////////////////////////////////////////////////////////////////////
	/*
	====================================================================
    功能        ：获取最后一次错误码
    输入参数说明：无
    返回值说明  ：错误码
	====================================================================
	*/
	const u32 GetLastError( void )
			{	return m_dwLastError; }

	/*
	====================================================================
    功能        ：获取地址簿条目的最大个数
    输入参数说明：无
    返回值说明  ：条目最大个数
	====================================================================
	*/
	const u32 GetAddrEntryTotalSize( void )
			{	return m_cAddrBook.GetTotalSize(); }

	/*
	====================================================================
    功能        ：获取当前使用的地址簿条目个数
    输入参数说明：无
    返回值说明  ：使用的地址簿条目个数
	====================================================================
	*/
	const u32 GetAddrEntryUsedSize( void )
			{ return m_cAddrBook.GetUsedSize(); }

	/*
	====================================================================
    功能        ：获取地址簿会议组最大个数
    输入参数说明：无
    返回值说明  ：会议组最大个数
	====================================================================
	*/
	const u32 GetAddrGroupTotalSize( void )
			{ return m_cAddrMultiSetEntry.GetTotalSize(); }

	/*
	====================================================================
    功能        ：获取当前使用的地址簿会议组个数
    输入参数说明：无
    返回值说明  ：使用的会议组个数
	====================================================================
	*/
	const u32 GetAddrGroupUsedSize( void )
			{ return m_cAddrMultiSetEntry.GetUsedSize(); }


	// 有序获取地址簿条目或地址簿会议组
	/////////////////////////////////////////////////////////////////////////////
	/*
	====================================================================
    功能        ：获取有序表中第一个地址条目(有序表)
    输入参数说明：存放地址簿条目的缓存区
    返回值说明  ：pcAddrEntry -- 第一个地址簿条目
				  成功 -- 返回条目索引
				  失败 -- 返回非法索引INVALID_INDEX
	====================================================================
	*/
	u32 GetFirstAddrEntry( CAddrEntry* pcAddrEntry );

	/*
	====================================================================
    功能        ：获取有序表中第一个会议组(有序表)
    输入参数说明：存放地址簿会议组的缓存区
    返回值说明  ：pcAddrMultiSetEntry -- 第一个会议组
				  成功 -- 返回会议组索引
				  失败 -- 返回非法索引INVALID_INDEX
	====================================================================
	*/
	u32 GetFirstAddrMultiSetEntry( CAddrMultiSetEntry* pcAddrMultiSetEntry );

	/*
	====================================================================
    功能        ：获取有序表中当前条目后继的多个地址簿条目(有序表)
    输入参数说明：当前条目索引 + 存放后继地址条目列表缓存区 + 列表最大长度 
    返回值说明  ：pcEntryTable -- 实际获取的条目列表
				  成功 -- 返回实际获取的条目个数
				  失败 -- 返回0(包括不合法和实际条目没有的情况)
	====================================================================
	*/
	u32 GetNextAddrEntry( u32 dwCurEntryIndex, CAddrEntry* pcEntryTable, 
							u32 dwEntryNum = 1 );

	/*
	====================================================================
    功能        ：获取有序表中当前会议组后继的多个会议组(有序表)
    输入参数说明：当前会议组索引 + 存放后继会议组列表缓存区 + 列表最大长度
    返回值说明  ：pcGroupTable -- 实际获取的会议组列表
				  成功 -- 返回实际获取的会议组个数
				  失败 -- 返回0(包括不合法和实际条目没有的情况)
	====================================================================
	*/
	u32 GetNextAddrMultiSetEntry( u32 dwCurGroupIndex, CAddrMultiSetEntry* pcGroupTable,
									u32 dwGroupNum = 1 );
	
	bool IsAddrEntryValid( u32 dwAddrEntryIndex);
	bool IsAddrMultiSetEntryValid( u32 dwAddrMultiSetEntryIndex);
		
	// 搜索完全匹配的条目或会议组
	//////////////////////////////////////////////////////////////////////////
	/*
	====================================================================
    功能        ：搜索完全匹配的条目
    输入参数说明：地址簿条目信息(包括所有信息)
    返回值说明  ：u32  条目索引
				  成功 -- 返回查找条目的索引值
				  失败 -- INVALID_INDEX
	====================================================================
	*/
	u32 IsAddrEntryExist( TADDRENTRYINFO tAddrEntryInfo );

	/*
	====================================================================
    功能        ：	搜索完全匹配的会议组
    输入参数说明：	地址簿会议组信息(包括所有信息)
    返回值说明  ：	u32  会议组索引
					成功 -- 返回查找会议组的索引值
					失败 -- INVALID_INDEX
	====================================================================
	*/
	u32 IsAddrMultiSetEntryExist( TMULTISETENTRYINFO tMultiSetEntryInfo );
	
	// 获取指定地址簿条目或或会议组
	//////////////////////////////////////////////////////////////////////////
	/*
	====================================================================
    功能        ：通过索引获取指定的地址簿条目
    输入参数说明：存放地址簿条目的缓存区 + 索引(默认0)
    返回值说明  ：pcAddrEntry -- 指定的地址簿条目
				  成功 -- 返回条目索引
				  失败 -- 返回非法索引INVALID_INDEX
	====================================================================
	*/
	u32  GetAddrEntry( CAddrEntry *pcAddrEntry, u32 dwEntryIndex = 0 );

	/*
	====================================================================
    功能        ：通过索引获取指定的地址簿会议组
    输入参数说明：存放地址簿会议组的缓存区 + 索引( 默认0 )
    返回值说明  ：pcMultiSetEntry -- 指定的会议组
				  成功 -- 返回会议组索引
				  失败 -- 返回非法索引INVALID_INDEX
	====================================================================
	*/
	u32  GetAddrMultiSetEntry( CAddrMultiSetEntry* pcMultiSetEntry, 
									u32 dwGroupIndex = 0 );



	// 在地址簿中查找与此匹配的所有地址簿条目或会议组
	//////////////////////////////////////////////////////////////////////////
	/*
	====================================================================
    功能        ：根据条目名称查找所有匹配条目
    输入参数说明：条目名称 + 存放匹配条目列表的缓存区 + 列表最大长度
    返回值说明  ：pcEntryTable -- 实际条目列表
				  成功 -- 返回实际匹配的条目个数
				  失败 -- 返回0(包括不合法和实际条目没有的情况)
	====================================================================
	*/
	u32 GetAddrEntryByName( char* pchEntryName, CAddrEntry* pcEntryTable, 
						u32 dwEntryNum = 1 );

	/*
	====================================================================
    功能        ：根据终端H323别名查找所有匹配的条目
    输入参数说明：终端H323别名 + 存放匹配条目列表的缓存区 + 列表最大长度
    返回值说明  ：pcEntryTable -- 实际条目列表
				  成功 -- 返回实际匹配的条目个数
				  失败 -- 返回0(包括不合法和实际条目没有的情况)
	====================================================================
	*/
	u32 GetAddrEntryByAlias( char* pchMtAlias, CAddrEntry* pcEntryTable,
						u32 dwEntryNum = 1 );

	/*
	====================================================================
    功能        ：根据终端地址查找所有匹配的条目
    输入参数说明：终端地址 + 存放匹配条目列表的缓存区 + 列表最大长度
    返回值说明  ：pcEntryTable -- 实际条目列表
				  成功 -- 返回实际匹配的条目个数
				  失败 -- 返回0(包括不合法和实际条目没有的情况)
	====================================================================
	*/
	u32 GetAddrEntryByAddr( char* pchIpAddr, CAddrEntry* pcEntryTable,
						u32 dwEntryNum = 1 );

	/*
	====================================================================
    功能        ：根据终端号码查找所有匹配的条目
    输入参数说明：终端号码 + 存放匹配条目列表的缓存区 + 列表的最大长度
    返回值说明  ：pcEntryTable -- 实际条目列表
				  成功 -- 返回实际匹配的条目个数
				  失败 -- 返回0(包括不合法和实际条目没有的情况)
	====================================================================
	*/
	u32 GetAddrEntryByNumber( char* pchMtNumber, CAddrEntry* pcEntryTable,
						u32 dwEntryNum = 1 );

	/*
	====================================================================
    功能        ：根据会议组名称查找所有匹配的条目
    输入参数说明：会议组名称 + 存放匹配会议组列表的缓存区 + 列表最大长度
    返回值说明  ：pcGroupTable -- 实际会议组列表
				  成功 -- 返回实际匹配的会议组个数
				  失败 -- 返回0(包括不合法和实际条目没有的情况)
	====================================================================
	*/
	u32 GetAddrMultiSetEntry( char* pchGroupName, CAddrMultiSetEntry* pcGroupTable,
								u32 dwGroupNum = 1 );


	// 向地址簿中加入条目或会议组
	//////////////////////////////////////////////////////////////////////////////
	/*
	====================================================================
    功能        ：向地址簿中加入一个条目
    输入参数说明：条目名称 + 终端别名 + 终端地址 + 终端号码 + 终端速率
    返回值说明  ：成功 -- 返回该条目的索引
				  失败 -- 返回非法索引INVALID_INDEX
	====================================================================
	*/
	u32 AddAddrEntry( const char* pchEntryName,
						const char* pchMtAlias = NULL,
						const char* pchMtIpAddr = NULL,
						const char* pchMtNumber = NULL,
						const u16  wCallRate = 0 );

	/*
	====================================================================
    功能        ：向地址簿中加入一个会议组
    输入参数说明：会议组名称 + 会议速率 + 会议开放属性
    返回值说明  ：成功 -- 返回该会议组的索引
				  失败 -- 返回非法索引INVALID_INDEX
	====================================================================
	*/
	u32 AddAddrMultiSetEntry( const char* pchGroupName,
								const char* pchMcuIpAddr = NULL,
								const u16 wBitRate = 0,
								const u8 byOpen = 0 );

	/*
	====================================================================
    功能        ：向地址簿中加入一个条目
    输入参数说明：条目对象
    返回值说明  ：成功 -- 返回加入条目的索引
				  失败 -- 返回非法索引INVALID_INDEX
	====================================================================
	*/
	u32 AddAddrEntry( /*const*/ CAddrEntry &cAddrEntry );

    /*
	====================================================================
    功能        ：将从地址薄文件中读取的条目加入地址簿对象中
    输入参数说明：条目对象
    返回值说明  ：成功 -- 返回加入条目的索引
				  失败 -- 返回非法索引INVALID_INDEX
	====================================================================
	*/
    u32 AddAddrEntryFromFile( /*const*/ CAddrEntry &cAddrEntry );

	/*
	====================================================================
    功能        ：向地址簿中加入一个会议组
    输入参数说明：会议组对象
    返回值说明  ：成功 -- 返回加入会议组的索引
				  失败 -- 返回非法索引INVALID_INDEX
	====================================================================
	*/
	u32 AddAddrMultiSetEntry( /*const*/ CAddrMultiSetEntry &cAddrMultiSetEntry );
	
	/*
	====================================================================
    功能        ：向地址簿中加入一个条目
    输入参数说明：条目信息
    返回值说明  ：成功 -- 返回加入条目的索引
				  失败 -- 返回非法索引INVALID_INDEX
	====================================================================
	*/
	u32 AddAddrEntry( const TADDRENTRYINFO &tAddrEntryInfo, 
						u32 dwIndex = INVALID_INDEX );

	/*
	====================================================================
    功能        ：向地址簿中加入一个会议组
    输入参数说明：会议组信息
    返回值说明  ：成功 -- 返回加入会议组的索引
				  失败 -- 返回非法索引INVALID_INDEX
	====================================================================
	*/
	u32 AddAddrMultiSetEntry( const TMULTISETENTRYINFO &tMultiSetEntryInfo, 
								u32 dwIndex = INVALID_INDEX );

	// 向会议组中加入地址簿条目
	/////////////////////////////////////////////////////////////////////////////
	/*
	====================================================================
    功能        ：向指定的会议组中加入一个已存在的条目
    输入参数说明：会议组索引 + 条目索引
    返回值说明  ：成功 -- 返回加入条目的索引
				  失败 -- 返回非法索引INVALID_INDEX
	====================================================================
	*/
	u32 AddAddrEntry( u32 dwGroupIndex, u32 dwEntryIndex );

	/*
	====================================================================
    功能        ：向指定的会议组中加入一个条目
    输入参数说明：会议组索引 + 条目实体
    返回值说明  ：成功 -- 返回加入条目的索引
				  失败 -- 返回非法索引INVALID_INDEX
	====================================================================
	*/
	u32 AddAddrEntry( u32 dwGroupIndex, /*const*/ CAddrEntry &cAddrEntry );

	/*
	====================================================================
    功能        ：向指定的会议组中加入一个条目
    输入参数说明：会议组索引 + 条目信息
    返回值说明  ：成功 -- 返回加入条目的索引
				  失败 -- 返回非法索引INVALID_INDEX
	====================================================================
	*/
	u32 AddAddrEntry( u32 dwGroupIndex, const TADDRENTRYINFO &tAddrEntryInfo );


	// 从地址簿中删除条目或会议组
	//////////////////////////////////////////////////////////////////////////
	/*
	====================================================================
    功能        ：清空整个地址簿
    输入参数说明：无
    返回值说明  ：成功与否
	====================================================================
	*/
	BOOL ClearAddrBook( void );

	/*
	====================================================================
    功能        ：从地址簿中删除指定的地址簿条目
    输入参数说明：条目索引
    返回值说明  ：成功与否
	====================================================================
	*/
	BOOL DelAddrEntry( u32 dwEntryIndex, u32* pdwGroupIndexTable = NULL, u16* dwGroupNum = NULL );

	/*
	====================================================================
    功能        ：从地址簿中删除指定的会议组
    输入参数说明：会议组索引
    返回值说明  ：成功与否
	====================================================================
	*/
	BOOL DelAddrMultiSetEntry( u32 dwGroupIndex );

	// 从会议组中删除条目
	////////////////////////////////////////////////////////////////////////
	/*
	====================================================================
    功能        ：从会议组中删除指定的条目(并不从地址簿中删除)
    输入参数说明：会议组索引 + 条目索引
    返回值说明  ：成功与否
	====================================================================
	*/
	BOOL DelAddrEntry( u32 dwGroupIndex, u32 dwEntryIndex );

	// 修改地址簿条目或会议组
	////////////////////////////////////////////////////////////////////////
	/*
	====================================================================
    功能        ：修改指定的地址簿条目
    输入参数说明：条目实体
    返回值说明  ：成功与否
	====================================================================
	*/
	BOOL ModifyAddrEntry( const CAddrEntry &cAddrEntry, BOOL bNeelSort = TRUE );

	/*
	====================================================================
    功能        ：修改指定的会议组
    输入参数说明：会议组实体
    返回值说明  ：成功与否
	====================================================================
	*/
	BOOL ModifyAddrGroup( const CAddrMultiSetEntry &cAddrMultiSetEntry, BOOL bNeedSort = TRUE );

	/*
	====================================================================
    功能        ：修改指定的地址簿条目
    输入参数说明：条目索引 + 条目信息
    返回值说明  ：成功与否
	====================================================================
	*/
	BOOL ModifyAddrEntry( u32 dwEntryIndex, const TADDRENTRYINFO &tAddrEntryInfo );

	/*
	====================================================================
    功能        ：修改指定的会议组的信息,不修改终端列表
    输入参数说明：会议组索引 + 会议组信息
    返回值说明  ：成功与否
	====================================================================
	*/
	BOOL ModifyAddrGroupInfo( u32 dwGroupIndex, 
							  const TMULTISETENTRYINFO &tMultiSetEntryInfo );

	/*
	====================================================================
    功能        ：修改指定的会议组,包括终端列表
    输入参数说明：会议组索引 + 会议组信息 + 列表长度 + 列表数组
    返回值说明  ：成功与否
	====================================================================
	*/
	BOOL ModifyAddrGroup( u32 dwGroupIndex, 
						  const TMULTISETENTRYINFO &tMultiSetEntryInfo,
						  u8 byMtListLen = 0,
						  u32 *pdwMtList = NULL);

	// 保存或载入地址簿
	////////////////////////////////////////////////////////////////////////
	/*
	====================================================================
    功能        ：从文件载入地址簿
    输入参数说明：无
    返回值说明  ：成功与否
	====================================================================
	*/
	BOOL LoadAddrBook( const char* pchLoadPath = NULL/*, BOOL32 bKeepRateNetOrder = TRUE*/ );

	/*
	====================================================================
    功能        ：保存地址簿到文件
    输入参数说明：无
    返回值说明  ：成功与否
	====================================================================
	*/
	BOOL SaveAddrBook( const char* pchSavePath = NULL );    

    /*
	====================================================================
    功能        ：从文件载入地址簿(老版本文件，在LoadAddrBook里面调用)
    输入参数说明：无
    返回值说明  ：成功与否
	====================================================================
	*/
    BOOL LoadOldFile(const char* pchLoadPath = NULL, BOOL32 bKeepRateNetOrder = TRUE);

    //TEST
    BOOL AddrEntryOld2New(CAddrEntry& cAddrEntry , const u8 * pbyBuf);
    BOOL MultiSetEntryOld2New(CAddrMultiSetEntry& cAddrEntry , const u8 * pbyBuf);
	
	// test 
	/*
	====================================================================
    功能        ：打印地址簿信息(用于测试)
    输入参数说明：无
    返回值说明  ：无
	====================================================================
	*/
	void PrintAddrBook( void );
    
    /*
	====================================================================
    功能        ：获取会议组多级关系表
    输入参数说明：TMultiLevel *tMultiLevel:返回表内容
    返回值说明  ：成功与否
	====================================================================
	*/
    BOOL32 GetMultiLevelTable( TMultiLevel *tMultiLevel);

    /*
	====================================================================
    功能        ：将某一会议作为另一会议的下级会议
    输入参数说明：u32 dwGroupIndex：下级会议组索引 
                  u32 dwUpGroupIndex：上级会议组索引
    返回值说明  ：成功与否
	====================================================================
	*/
    BOOL32 AddGroupToGroup( const u32 dwGroupIndex, const u32 dwUpGroupIndex );

    /*
	====================================================================
    功能        ：删除一会议作为另一会议的下级关系
    输入参数说明：u32 dwGroupIndex：下级会议组索引 
                  u32 dwUpGroupIndex：上级会议组索引
    返回值说明  ：成功与否
	====================================================================
	*/
    BOOL32 DeleteGroupFromGroup( u32 dwGroupIndex, u32 dwUpGroupIndex ); 

     /*
	====================================================================
    功能        ：找到第一级的所有会议组
    输入参数说明：u32 *dwOneLevelGroup：输出第一级会议索引表，
                  要有足够空间（最大组数）
    返回值说明  ：u32：第一级会议组个数
	====================================================================
	*/
    u32 FindLevelOneGroup( u32 *dwOneLevelGroup );

    /*
	====================================================================
    功能        ：找到某一会议组的上级会议
    输入参数说明：u32 dwGroupIndex：会议索引
    返回值说明  ：u32：上级会议索引
	====================================================================
	*/
    u32 FindUpGroupOfGroupByIndex( u32 dwGroupIndex );

    /*
	====================================================================
    功能        ：找到会议组的所有下级会议组的索引列表
    输入参数说明：u32 dwUpGroupIndex：上级会议组索引
                  u32 *dwLowGroupIndex：返回下级会议组索引列表
    返回值说明  ：u32：下级会议组个数,错误时返回INVALID_INDEX
	====================================================================
	*/
    u32 FindLowGroupOfGroupByIndex( u32 dwUpGroupIndex, u32 *dwLowGroupIndex );
    
    /*
	====================================================================
    功能        ：会议组直接层级关系打印
    输入参数说明：void
    返回值说明  ：void
	====================================================================
	*/
    void PrintGroupRelation();

    /*
	====================================================================
    功能        ：获取会议组下所有条目
    输入参数说明：u32 dwGroupIndext：会议组索引
                  u32* pdwEntrySetIndext：条目索引指针
    返回值说明  ：u32:会议组下条目数量
	====================================================================
	*/
    u32 GetGroupAllEntrySet( u32 dwGroupIndext, u32* pdwEntrySetIndext );
	
    /*
	====================================================================
    功能        ：获得当前有多少组间关系
    输入参数说明：
    返回值说明  ：组间关系数
	====================================================================
	*/
    u16 GetMultiLevelUsedSize();

    /*
	====================================================================
    功能        ：获得部分组关系表
    输入参数说明：u32 dwStartIndex:起始位置
                  u32 dwGetNum:期望获得的数目
    返回值说明  ：TMultiLevel* tMultiLevel(组间关系表)
                  u32 实际获得的数目
	====================================================================
	*/
    u32 GetMultiLevelPartByIndex(u32 dwStartIndex, u32 dwGetNum, TMultiLevel* tMultiLevel);

protected :

    /*
    ====================================================================
    功能        ：获取会议组多级关系指针
    输入参数说明：
    返回值说明  ：TMultiLevel *tMultiLevel:返回表内容
    ====================================================================
    */
    TMultiLevel* GetMultiLevel();

    /*
	====================================================================
    功能        ：保存会议组多级关系表
    输入参数说明：TMultiLevel *tMultiLevel:表内容
    返回值说明  ：成功与否
	====================================================================
	*/
    BOOL32 SaveMultiLevelTable( TMultiLevel *tMultiLevel, u16 wMultiLevelSize);

    /*
	====================================================================
    功能        ：清除会议组与其他会议组的层级关系
    输入参数说明：u32 dwGroupIndex:会议组索引
    返回值说明  ：void
	====================================================================
	*/
    void ClearGroupRelation( u32 dwGroupIndex );

    /*
	====================================================================
    功能        ：两个组之间是否为上下级关系，包括直接和间接上下级关系
    输入参数说明：u32 dwGroupIndext：会议组索引
                  u32 dwUpGroupIndext：上级组索引
    返回值说明  ：BOOL32:是否是上级
	====================================================================
	*/
    BOOL32 IsUpLevelGroup( u32 dwGroupIndext, u32 dwUpGroupIndext );

    /*
	====================================================================
    功能        ：设置当前有多少组间关系
    输入参数说明：
    返回值说明  ：组间关系数
	====================================================================
	*/
    void SetMultiLevelUsedSize( u16 wSize);

    /*
	====================================================================
    功能        ：整理组间关系表，去掉不存在的会议
    输入参数说明：
    返回值说明  ：TMultiLevel* tMultiLevel(组间关系表)
                  u16 wUsedSize(表大小)
	====================================================================
	*/
    u16 PackMultiLevelTable(TMultiLevel* tMultiLevel, u16 wUsedSize);

    /*
	====================================================================
    功能        ：获得字符编码方式
    输入参数说明：void
    返回值说明  ：u8
	====================================================================
	*/
    u8 GetCodingMode()
    {
        return m_byCodingMode;
    }

    /*
	====================================================================
    功能        ：将条目编码由GBK转为UTF8
    输入参数说明：cEntry(输入GBK，输出UTF8)
    返回值说明  ：BOOL32是否发生错误
	====================================================================
	*/
    BOOL32 AddrEntryGbk2Utf8( CAddrEntry& cEntry );

    /*
	====================================================================
    功能        ：将条目编码由UTF8转为GBK
    输入参数说明：cEntry(输入UTF8，输出GBK)
    返回值说明  ：BOOL32是否发生错误
	====================================================================
	*/
    BOOL32 AddrEntryUtf82Gbk( CAddrEntry& cEntry );

    /*
	====================================================================
    功能        ：将组条目编码由GBK转为UTF8
    输入参数说明：cGroupEntry(输入GBK，输出UTF8)
    返回值说明  ：BOOL32是否发生错误
	====================================================================
	*/
    BOOL32 AddrGroupGbk2Utf8( CAddrMultiSetEntry& cGroupEntry );

    /*
	====================================================================
    功能        ：将组条目编码由UTF8转为GBK
    输入参数说明：cGroupEntry(输入UTF8，输出GBK)
    返回值说明  ：BOOL32是否发生错误
	====================================================================
	*/
    BOOL32 AddrGroupUtf82Gbk( CAddrMultiSetEntry& cGroupEntry );

    /*
	====================================================================
    功能        ：将地址薄编码由UTF8转为GBK
    输入参数说明：无
    返回值说明  ：void
	====================================================================
	*/
    void AddrBookUtf82Gbk();

    /*
	====================================================================
    功能        ：将地址薄编码由Gbk转为Utf-8
    输入参数说明：无
    返回值说明  ：void
	====================================================================
	*/
    void AddrBookGbk2Utf8();

    /*
	====================================================================
    功能        ：得到某类型所有成员大小
    输入参数说明：dwEntryType(IN:类型)+dwMemberNum(OUT:成员数)+pdwMemberSize(OUT:成员大小列表)
    返回值说明  ：BOOL
	====================================================================
	*/
    BOOL GetEntryMemberSize( const u16 dwEntryType, u16& dwMemberNum, u16* pdwMemberSize );

    /*
	====================================================================
    功能        ：保存条目头信息到文件
    输入参数说明：wEntryType(IN:类型)
    返回值说明  ：BOOL
	====================================================================
	*/
    BOOL SaveEntryHeadInfo(const u16 wEntryType);

    /*
	====================================================================
    功能        ：按类型读取条目
    输入参数说明：wEntryType(IN:类型)
    返回值说明  ：BOOL
	====================================================================
	*/
    BOOL ReadEntry(const u16 wEntryType);

    /*
	====================================================================
    功能        ：跳过不认识的类型
    输入参数说明：void
    返回值说明  ：BOOL
	====================================================================
	*/
    BOOL SkipUnknowType();

    /*
	====================================================================
    功能        ：全路径名中获取名字和路径
    输入参数说明：pchWholePath(IN:全路径) + pchFileName(OUT:文件名) + pchPath(OUT:路径)
    返回值说明  ：BOOL
	====================================================================
	*/
    BOOL GainNameAndPath(  const s8* pchWholePath, s8 * pchFileName, s8 * pchPath );

private :

	FILE*	m_fpAddrBook;

	// 地址薄条目数组对象
	CAddArray< CAddrEntry >			m_cAddrBook;

	// 地址簿会议组数组对象
	CAddArray< CAddrMultiSetEntry >		m_cAddrMultiSetEntry;

	u32	m_dwAddrEntryTotalSize;			// 地址条目最大个数
	u32	m_dwAddrGroupTotalSize;			// 会议组最大个数
	u32	m_dwLastError;
    TMultiLevel* m_tMultiLevelTable;    //组间关系表
    u16 m_wMultiLevelUsedSize;          //组间关系表长度

    u8 m_byCodingMode;                  //编码方式		
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;





BOOL InitAddressBook( u32 dwEntryNum = MAX_ADDRENTRY, 
					  u32 dwMultiSetNum = MAX_ADDRGROUP,
                      const char* pchAddrBookPath = NULL,
                      u8 byCodingMode = emCodingModeGBK);


API void addrhelp();
API void addrbookver();
API void prtaddrbook();
API void addrlogon();
API void addrlogoff();

#if defined(WIN32) 
#pragma pack(pop)
#endif

#endif // ADDR_BOOK_H


