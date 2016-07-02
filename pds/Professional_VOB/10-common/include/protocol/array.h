#ifndef ARRAY_H_
#define ARRAY_H_

#include "osp.h"
#include "eventid.h"
#include "kdvtype.h"


// type define


// 消息定义(详见<<地址簿设计参考文档>>)
//////////////////////////////////////////////////////////////////////////

// 地址簿初始化
OSPEVENT( ADDRBOOK_INIT,						EV_ADDRBOOK_BGN );

// 地址簿注册/注销
OSPEVENT( ADDRBOOK_REGISTER_REQ,				EV_ADDRBOOK_BGN + 1 );
OSPEVENT( ADDRBOOK_REGISTER_ACK,				EV_ADDRBOOK_BGN + 2 );
OSPEVENT( ADDRBOOK_REGISTER_NACK,				EV_ADDRBOOK_BGN + 3 );
OSPEVENT( ADDRBOOK_UNREG_CMD,					EV_ADDRBOOK_BGN + 4 );

// 地址簿保存应答消息
OSPEVENT( ADDRBOOK_SAVE_ACK,					EV_ADDRBOOK_BGN + 5 );
OSPEVENT( ADDRBOOK_SAVE_NACK,					EV_ADDRBOOK_BGN + 6 );

// 向地址簿中增加条目请求和应答
OSPEVENT( ADDRBOOK_ADD_ENTRY_REQ,				EV_ADDRBOOK_BGN + 7 );
OSPEVENT( ADDRBOOK_ADD_ENTRY_ACK,				EV_ADDRBOOK_BGN + 8 );
OSPEVENT( ADDRBOOK_ADD_ENTRY_NACK,				EV_ADDRBOOK_BGN + 9 );

// 向地址簿中增加会议组请求和应答
OSPEVENT( ADDRBOOK_ADD_MULSETENTRY_REQ,			EV_ADDRBOOK_BGN + 10 );
OSPEVENT( ADDRBOOK_ADD_MULSETENTRY_ACK,			EV_ADDRBOOK_BGN + 11 );
OSPEVENT( ADDRBOOK_ADD_MULSETENTRY_NACK,		EV_ADDRBOOK_BGN + 12 );

// 从地址簿中删除条目请求和应答
OSPEVENT( ADDRBOOK_DEL_ENTRY_REQ,				EV_ADDRBOOK_BGN + 13 );
OSPEVENT( ADDRBOOK_DEL_ENTRY_ACK,				EV_ADDRBOOK_BGN + 14 );
OSPEVENT( ADDRBOOK_DEL_ENTRY_NACK,				EV_ADDRBOOK_BGN + 15);

// 从地址簿中删除会议组请求和应答
OSPEVENT( ADDRBOOK_DEL_MULSETENTRY_REQ,			EV_ADDRBOOK_BGN + 16 );
OSPEVENT( ADDRBOOK_DEL_MULSETENTRY_ACK,			EV_ADDRBOOK_BGN + 17 );
OSPEVENT( ADDRBOOK_DEL_MULSETENTRY_NACK,		EV_ADDRBOOK_BGN + 18 );

// 向会议组终端列表中加入终端请求和应答
OSPEVENT( ADDRBOOK_ADD_ENTRYTOMULSET_REQ,		EV_ADDRBOOK_BGN + 19 );
OSPEVENT( ADDRBOOK_ADD_ENTRYTOMULSET_ACK,		EV_ADDRBOOK_BGN + 20 );
OSPEVENT( ADDRBOOK_ADD_ENTRYTOMULSET_NACK,		EV_ADDRBOOK_BGN + 21 );

// 从会议组终端列表中删除终端请求和应答
OSPEVENT( ADDRBOOK_DEL_ENTRYFRMULSET_REQ,		EV_ADDRBOOK_BGN + 22 );
OSPEVENT( ADDRBOOK_DEL_ENTRYFRMULSET_ACK,		EV_ADDRBOOK_BGN + 23 );
OSPEVENT( ADDRBOOK_DEL_ENTRYFRMULSET_NACK,		EV_ADDRBOOK_BGN + 24);

// 修改条目请求和应答
OSPEVENT( ADDRBOOK_MODIFYENTRY_REQ,				EV_ADDRBOOK_BGN + 25 );
OSPEVENT( ADDRBOOK_MODIFYENTRY_ACK,				EV_ADDRBOOK_BGN + 26 );
OSPEVENT( ADDRBOOK_MODIFYENTRY_NACK,			EV_ADDRBOOK_BGN + 27 );

// 修改会议组请求和应答
OSPEVENT( ADDRBOOK_MODIFYMULSETENTRY_REQ,		EV_ADDRBOOK_BGN + 28 );
OSPEVENT( ADDRBOOK_MODIFYMULSETENTRY_ACK,		EV_ADDRBOOK_BGN + 29 );
OSPEVENT( ADDRBOOK_MODIFYMULSETENTRY_NACK,		EV_ADDRBOOK_BGN + 30 );

// 获取地址簿中条目的总个数请求和应答
OSPEVENT( ADDRBOOK_GETENTRYNUM_REQ,				EV_ADDRBOOK_BGN + 31 );
OSPEVENT( ADDRBOOK_GETENTRYNUM_ACK,				EV_ADDRBOOK_BGN + 32 );
OSPEVENT( ADDRBOOK_GETENTRYNUM_NACK,			EV_ADDRBOOK_BGN + 33 );

// 获取地址簿中所有条目信息的请求和应答
OSPEVENT( ADDRBOOK_GETENTRY_REQ,				EV_ADDRBOOK_BGN + 34 );
OSPEVENT( ADDRBOOK_GETENTRY_ACK,				EV_ADDRBOOK_BGN + 35 );
OSPEVENT( ADDRBOOK_GETENTRY_NACK,				EV_ADDRBOOK_BGN + 36 );

// 获取地址簿中会议组总个数的请求和应答
OSPEVENT( ADDRBOOK_GETMULSETENTRYNUM_REQ,		EV_ADDRBOOK_BGN + 37 );
OSPEVENT( ADDRBOOK_GETMULSETENTRYNUM_ACK,		EV_ADDRBOOK_BGN + 38 );
OSPEVENT( ADDRBOOK_GETMULSETENTRYNUM_NACK,		EV_ADDRBOOK_BGN + 39 );

// 获取地址簿中所有会议组信息的请求和应答
OSPEVENT( ADDRBOOK_GETMULSETENTRY_REQ,			EV_ADDRBOOK_BGN + 40 );
OSPEVENT( ADDRBOOK_GETMULSETENTRY_ACK,			EV_ADDRBOOK_BGN + 41 );
OSPEVENT( ADDRBOOK_GETMULSETENTRY_NACK,			EV_ADDRBOOK_BGN + 42 );

// 更新或删除条目或会议组的指示
OSPEVENT( ADDRBOOK_REFRESHENTRY_NOTIF,			EV_ADDRBOOK_BGN + 43 );
OSPEVENT( ADDRBOOK_DELENTRY_NOTIF,				EV_ADDRBOOK_BGN + 44 );
OSPEVENT( ADDRBOOK_REFRESHMULSETENTRY_NOTIF,	EV_ADDRBOOK_BGN + 45 );
OSPEVENT( ADDRBOOK_DELMULSETENTRY_NOTIF,		EV_ADDRBOOK_BGN + 46 );

// 更新最近呼入/呼出列表(向地址簿模块发送)
OSPEVENT ( ADDRBOOK_UPDATEINMT_CMD				, EV_ADDRBOOK_BGN + 47 );
OSPEVENT ( ADDRBOOK_UPDATEOUTMT_CMD				, EV_ADDRBOOK_BGN + 48 );

// 清空地址簿命令
OSPEVENT ( ADDRBOOK_CLEARALL_CMD				, EV_ADDRBOOK_BGN + 49 );

//注销命令和回应
OSPEVENT( ADDRBOOK_UNREG_REQ,					EV_ADDRBOOK_BGN + 50 );
OSPEVENT( ADDRBOOK_UNREG_ACK,					EV_ADDRBOOK_BGN + 51 );
//返回错误码
OSPEVENT( ADDRBOOK_UNREG_NACK,					EV_ADDRBOOK_BGN + 52 );

/*地址薄多级支持*/
// 获取地址簿中会议组关系总个数的请求和应答
OSPEVENT( ADDRBOOK_GETMULSETRELATIONNUM_REQ,		EV_ADDRBOOK_BGN + 53 );
OSPEVENT( ADDRBOOK_GETMULSETRELATIONNUM_ACK,		EV_ADDRBOOK_BGN + 54 );
OSPEVENT( ADDRBOOK_GETMULSETRELATIONNUM_NACK,		EV_ADDRBOOK_BGN + 55 );

// 获取地址簿中所有会议组关系的请求和应答
OSPEVENT( ADDRBOOK_GETMULSETRELATION_REQ,			EV_ADDRBOOK_BGN + 56 );
OSPEVENT( ADDRBOOK_GETMULSETRELATION_ACK,			EV_ADDRBOOK_BGN + 57 );
OSPEVENT( ADDRBOOK_GETMULSETRELATION_NACK,			EV_ADDRBOOK_BGN + 58 );

// 向地址簿中增加会议组关系请求和应答
OSPEVENT( ADDRBOOK_ADD_MULSETTOMULSET_REQ,			EV_ADDRBOOK_BGN + 59 );
OSPEVENT( ADDRBOOK_ADD_MULSETTOMULSET_ACK,			EV_ADDRBOOK_BGN + 60 );
OSPEVENT( ADDRBOOK_ADD_MULSETTOMULSET_NACK,		    EV_ADDRBOOK_BGN + 61 );

// 从地址簿中删除会议组关系请求和应答
OSPEVENT( ADDRBOOK_DEL_MULSETFROMMULSET_REQ,		EV_ADDRBOOK_BGN + 62 );
OSPEVENT( ADDRBOOK_DEL_MULSETFROMMULSET_ACK,		EV_ADDRBOOK_BGN + 63 );
OSPEVENT( ADDRBOOK_DEL_MULSETFROMMULSET_NACK,		EV_ADDRBOOK_BGN + 64 );

OSPEVENT( ADDRBOOK_REFRESHMULSETRELATION_NOTIF,		EV_ADDRBOOK_BGN + 65 );


// error id
#define ADDRBOOK_NO_ERROR				(u32)25000
#define ADDRBOOK_MEMORY_ERROR			(u32)( ADDRBOOK_NO_ERROR + 1 )
#define ADDRBOOK_INVALID_POINTER		(u32)( ADDRBOOK_NO_ERROR + 2 )
#define ADDRBOOK_NO_ELEMENT				(u32)( ADDRBOOK_NO_ERROR + 3 )
#define ADDRBOOK_OUTOF_RANGE			(u32)( ADDRBOOK_NO_ERROR + 4 )
#define ADDRBOOK_SIZE_ZERO				(u32)( ADDRBOOK_NO_ERROR + 5 )
#define ADDRBOOK_NO_FREEPOS				(u32)( ADDRBOOK_NO_ERROR + 6 )
#define ADDRBOOK_FILEOPT_ERROR			(u32)( ADDRBOOK_NO_ERROR + 7 )
#define ADDRBOOK_EXIST_ELEMENT			(u32)( ADDRBOOK_NO_ERROR + 8 )
#define ADDRBOOK_REGISTER_ERROR			(u32)( ADDRBOOK_NO_ERROR + 9 )

// 定义的Buff长度
#ifndef RES_H_
#define MAX_BUFFER_LEN  1024
#endif 

// 最大获取地址簿条目个数
const u32 MAX_GETENTRY_NUM = 25;
// 最大获取地址簿会议组个数
const u32 MAX_GETGROUP_NUM = 5;
// 最大获取地址簿会议组关系个数
const u32 MAX_GETGROUPRELATION_NUM = 40;


// 地址簿条目操作类型
#define ADD_ENTRY		0x01
#define MODIFY_ENTRY	0x02
#define DEL_ENTRY		0x03
#define SEARCH_ENTRY     0x04

// 地址簿条目类型
#define ADDR_ENTRY			0x1
#define MULTISET_ENTRY		0x2
#define MULTISET_RELATION   0X3     //地址薄组关系

#if defined(WIN32) 
#pragma pack(push)
#pragma pack(1)
#endif


// abstract array
//////////////////////////////////////////////////////////////////////////
template< class T >
class CAbArray
{
public :
	enum 
	{ 
		MAX_ARRAY_SIZE = 0xFFFFFFFF
	};
	// order type
	enum ArrayOrder
	{
		NO_ORDER,
		IN_ORDER,
		IN_REVERSE
	};

public :
	// attribute 
	u32 GetTotalSize( void ) { return m_dwArrTotalSize; }
	u32 GetUsedSize( void ) { return m_dwArrUsedSize; }
	ArrayOrder GetArrayOrder() { return m_OrderOfArr; }
	void ResetArrayOrder() { m_OrderOfArr = NO_ORDER; }
	
	// operation
	virtual BOOL ExpandArray( u32 dwNewSize ) = 0;
	virtual int StoreElement( T &Element, u32 dwIndex, BOOL bNeedSort = TRUE ) = 0;
	virtual int ReCallElement( T &Element, u32 dwIndex ) = 0;
	virtual void SwapElement( u32 dwIndex1, u32 dwIndex2 ) = 0;

	// get element by order,return physical index of element
	virtual int GetFirstElement( T &Element );
	virtual int GetNextElement( u32 dwIndex, T &Element );

	// sort
	int SortByShell();
	int ReOrder();

	// search

	// 在存放元素数组中查找空位置
	int SearchFreePos( void );

	// 根据元素物理位置索引,查找元素有序位置索引
	int SearchPosByLine( u32 dwPhyIndex );
	// 在有序数组中查找指定元素
	int SearchElemByBinary( T& KeyElement, BOOL32 bNeedSort = TRUE);

	// 向有序列表中加入新增元素的物理索引号
	int AddToOrderTable( u32 dwElementIndex );

	// 从有序列表中驱除已删除元素的物理索引号
	int DelFromOrderTable( u32 dwElementIndex );

	u32 GetLastError( void ) { return m_dwLastError; }

protected :
	u32		m_dwArrTotalSize;		// max size of array
	u32		m_dwArrUsedSize;		// current used size of array
	u32*		m_pdwOrderPos;			// order table
	ArrayOrder	m_OrderOfArr;			// order of array
	T			m_TempBuff1;
	T			m_TempBuff2;
	u32		m_dwLastError;			// 错误码


}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;


// 向有序列表中加入一个新增元素的物理索引号,返回在有序列表中的位置
template< class T >
int CAbArray< T >::AddToOrderTable( u32 dwElementIndex )
{
	if ( m_pdwOrderPos == NULL )
	{
		m_dwLastError = ADDRBOOK_INVALID_POINTER;
		return -1;
	}

	// 有序数组为空,放在第一个有序位置
	if ( m_dwArrUsedSize == 0 )
	{
		m_pdwOrderPos[ 0 ] = dwElementIndex;
		m_OrderOfArr = IN_ORDER;
		m_dwArrUsedSize++;
		return 0;
	}
	// 若元素已满,不能再插入
	else if ( m_dwArrUsedSize == m_dwArrTotalSize )
	{
		m_dwLastError = ADDRBOOK_OUTOF_RANGE;
		return -1;
	}
	// 一般情况,插入在最后一个位置,然后排序
	else
	{
		m_pdwOrderPos[ m_dwArrUsedSize ] = dwElementIndex;
		m_dwArrUsedSize++;
		if ( m_OrderOfArr == IN_ORDER )
		{
			m_OrderOfArr = NO_ORDER;
			if ( SortByShell() == -1 )
				return -1;
		}
		else if ( m_OrderOfArr == IN_REVERSE )
		{
			m_OrderOfArr = NO_ORDER;
			if ( SortByShell() == -1 )
				return -1;
			if ( ReOrder() == -1 )
				return -1;
		}
		else
		{
			m_OrderOfArr = NO_ORDER;
			if ( SortByShell() == -1 )
				return -1;
		}
	}

	return 0;


}

// 从有序列表中驱除已删除元素的物理索引号
template< class T >
int CAbArray< T >::DelFromOrderTable( u32 dwElementIndex )
{
	if ( m_pdwOrderPos == NULL )
	{
		m_dwLastError = ADDRBOOK_INVALID_POINTER;
		return -1;
	}

	if ( m_dwArrUsedSize == 0 )
	{
		m_dwLastError = ADDRBOOK_SIZE_ZERO;
		return -1;
	}

	int nIndex = SearchPosByLine( dwElementIndex );
	if ( nIndex == -1 )
		return -1;
	else
	{
		for( int i = nIndex; i < ( int )m_dwArrUsedSize - 1; i++ )
		{
			m_pdwOrderPos[ i ] = m_pdwOrderPos[ i + 1 ];			
		}
        m_pdwOrderPos[m_dwArrUsedSize-1] = INVALID_INDEX;
		m_dwArrUsedSize--;
	}

	return 0;
}

// 从有序列表中获取第一个元素,返回该元素的物理位置索引
template< class T >
int CAbArray< T >::GetFirstElement( T &Element )
{
	if ( m_pdwOrderPos == NULL )
	{
		m_dwLastError = ADDRBOOK_INVALID_POINTER;
		return -1;
	}
	
	if ( m_dwArrUsedSize == 0 )
	{
		m_dwLastError = ADDRBOOK_SIZE_ZERO;
		return -1;
	}

	int nIndex = ReCallElement( Element, m_pdwOrderPos[ 0 ] );
	if ( nIndex == -1 )
		return -1;

	return m_pdwOrderPos[ 0 ];
}

// 给定当前元素的物理位置索引,从有序列表中获取下一个元素,返回下一个元素及其物理位置索引
template< class T >
int CAbArray< T >::GetNextElement( u32 dwIndex, T &Element )
{
	if ( m_pdwOrderPos == NULL )
	{
		m_dwLastError = ADDRBOOK_INVALID_POINTER;
		return -1;
	}

	if ( dwIndex > m_dwArrTotalSize )
	{
		m_dwLastError = ADDRBOOK_OUTOF_RANGE;
		return -1;
	}

	int nOrderIndex = SearchPosByLine( dwIndex );
    //guzh[2008/05/04] 访问越界保护
	if ( nOrderIndex == -1 || nOrderIndex+1 >= (int)m_dwArrTotalSize)
		return -1;

	int nPhyIndex = ReCallElement( Element, m_pdwOrderPos[ nOrderIndex + 1 ]);
	if ( nPhyIndex == -1 )
		return -1;

	return m_pdwOrderPos[ nOrderIndex + 1 ];
}

// 希尔法排序
template< class T >
int CAbArray< T >::SortByShell()
{
	if ( m_pdwOrderPos == NULL )
	{
		m_dwLastError = ADDRBOOK_INVALID_POINTER;
		return -1;
	}
	
	if ( m_OrderOfArr == IN_ORDER )
		return 0;

	if ( m_OrderOfArr == IN_REVERSE )
	{
		if ( ReOrder() == -1 )
			return -1;
		return 0;
	}

	u32 dwOffSet = m_dwArrUsedSize;
	u32 dwTemp;
	while( dwOffSet > 1 )
	{
		dwOffSet = dwOffSet / 2;
		do 
		{
			m_OrderOfArr = IN_ORDER;
			// 从有序列表中获取比较元素的物理索引位置,
			// 根据物理索引位置获取元素内容,比较之
			// 在有序列表中交换相互的物理索引值
			// 实现不移动元素的位置情况下,由有序列表来维护数组元素的有序性
			for( int j = 0, i = dwOffSet; j < (int)( m_dwArrUsedSize - dwOffSet ); j++, i++ )
			{
				if ( ReCallElement( m_TempBuff1, m_pdwOrderPos[ j ] ) == -1 )
					return -1;
				if ( ReCallElement( m_TempBuff2, m_pdwOrderPos[ i ] ) == -1 )
					return -1;
				
				if ( m_TempBuff1 > m_TempBuff2 )
				{
					dwTemp = m_pdwOrderPos[ j ];
					m_pdwOrderPos[ j ] = m_pdwOrderPos[ i ];
					m_pdwOrderPos[ i ] = dwTemp;
					m_OrderOfArr = NO_ORDER;
				}

			}
			
		} while( m_OrderOfArr != IN_ORDER );
	}

	return 0;
}

// 反序
template< class T >	
int CAbArray< T >::ReOrder()
{
	return 0;
}

// 在物理位置查找空位置,返回空位置索引号
template< class T >
int CAbArray< T >::SearchFreePos()
{
	u32	dwIndex = 0;
	BOOL	bFound = FALSE;
	T		UnUsedBuff;		// 未使用的地址簿条目
		
	while ( ( dwIndex < m_dwArrTotalSize ) && ( !bFound ) )
	{
		int nIndex = ReCallElement( m_TempBuff1, dwIndex );
		if ( nIndex == -1 )
			return -1;

		if ( m_TempBuff1 != UnUsedBuff )
			dwIndex++;
		else
			bFound = TRUE;
	}
	
	if ( bFound )
		return dwIndex;
	else
	{
		m_dwLastError = ADDRBOOK_NO_FREEPOS;
		return -1;		
	}
	
}

// 根据元素的物理位置索引,查找其在有序列表中的位置
template< class T >
int CAbArray< T >::SearchPosByLine( u32 dwPhyIndex )
{
	u32	dwIndex = 0;
	BOOL	bFound = FALSE;
		
	// 有序数组存在性检查
	if ( m_pdwOrderPos == NULL )
	{
		m_dwLastError = ADDRBOOK_INVALID_POINTER;
		return -1;
	}

	// 在有序列表中查找指定元素
	while ( ( dwIndex < m_dwArrUsedSize ) && ( !bFound ) )
	{
		if ( dwPhyIndex != m_pdwOrderPos[ dwIndex ] )
			dwIndex++;
		else
			bFound = TRUE;
	}
	
	if ( bFound )
		return dwIndex;
	else
	{
		m_dwLastError = ADDRBOOK_NO_ELEMENT;
		return -1;		
	}
	
}

// 在有序位置中二分查找指定元素,返回该元素的物理位置索引
template< class T >
int CAbArray< T >::SearchElemByBinary( T& KeyElement, BOOL bNeedSort/* = TRUE*/ )
{
	int nLowPos = 0;
	int nHighPos = m_dwArrUsedSize - 1;
	int nMidPos;
	BOOL bFound = FALSE;

	if ( m_pdwOrderPos == NULL )
	{
		m_dwLastError = ADDRBOOK_INVALID_POINTER;
		return -1;
	}

	switch( m_OrderOfArr )
	{
	case NO_ORDER :
		if ( bNeedSort && SortByShell() == -1 )
			return -1;
		break;

	case IN_REVERSE :
		if ( ReOrder() == -1 )
			return -1;
		break;
	}

	while( ( nLowPos <= nHighPos ) && !bFound )
	{
		nMidPos = ( nLowPos + nHighPos ) / 2;
		if ( ReCallElement( m_TempBuff1, m_pdwOrderPos[ nMidPos ] ) == -1 )
			return -1;
		if ( KeyElement < m_TempBuff1 )
			nHighPos = nMidPos - 1;
		else if ( KeyElement > m_TempBuff1 )
			nLowPos = nMidPos + 1;
		else
			bFound = TRUE;
	}
	
	if ( bFound )
		return m_pdwOrderPos[ nMidPos ];
	else
	{
		m_dwLastError = ADDRBOOK_NO_ELEMENT;
		return -1;
	}

}



// array
//////////////////////////////////////////////////////////////////////////
template< class T >
class CAddArray : public CAbArray< T >
{
public :

	// constructor & destructor
	CAddArray( u32 dwArrSize = 0 );
	CAddArray( const CAddArray< T >& NewArray ) { ArrayCopy( NewArray ); }
	~CAddArray() 
	{ 
		this->m_dwArrTotalSize = 0;
		this->m_dwArrUsedSize = 0;
		this->m_OrderOfArr = this->NO_ORDER;
		if ( m_pArr != NULL ) 
		{
			delete []m_pArr; 
			m_pArr = NULL;
		}
		
		if ( this->m_pdwOrderPos != NULL ) 
		{
			delete [] this->m_pdwOrderPos;
			this->m_pdwOrderPos = NULL;
		}
	}
	
	// basic operation
	BOOL ExpandArray( u32 dwNewSize );
	int StoreElement( T &Element, u32 dwIndex, BOOL bNeedSort = TRUE );
    int StoreElementFromFile( T &Element, u32 dwIndex );
	int ReCallElement( T &Element, u32 dwIndex );
	void SwapElement( u32 dwIndex1, u32 dwIndex2 );

	// add & del
	int InsertElement( T &Element, u32 dwIndex = INVALID_INDEX );
    int InsertElementFromFile( T &Element, u32 dwIndex = INVALID_INDEX );
//	void DeletesElement( T &Element );
	int DeletesElement( u32 dwIndex );
//    BOOL32 IsIndexExisted( u32 dwIndex );

	// override
	CAddArray< T > &operator = ( const CAddArray< T > &NewArray )
	{
		ArrayCopy( NewArray );
		return *this;
	}

	T &operator[]( const u32 dwIndex ) { return m_pArr[ dwIndex ]; }
	

protected :
	T*	m_pArr;
	
private :

	// copy
	CAddArray< T > & ArrayCopy( const CAddArray< T >& NewArray );


}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;


// 构造函数,并传递数组长度
template< class T >
CAddArray< T >::CAddArray( u32 dwArrSize /* = 0 */ )
{
	this->m_dwArrTotalSize = 0;
	this->m_dwArrUsedSize = 0;
	this->m_OrderOfArr = this->NO_ORDER;
	m_pArr = NULL;
	this->m_pdwOrderPos = NULL;
	this->m_dwLastError = ADDRBOOK_NO_ERROR;

	if ( dwArrSize > 0 )
	{
		// 分配物理位置
		if ( m_pArr != NULL )
		{
			delete[] m_pArr;
			m_pArr = NULL;
		}

		m_pArr = new T[ dwArrSize ];
		if ( m_pArr != NULL )
			this->m_dwArrTotalSize = dwArrSize;
		else
			this->m_dwLastError = ADDRBOOK_MEMORY_ERROR;

		// 创建有序映射表
		if ( this->m_pdwOrderPos != NULL )
		{
			delete[] this->m_pdwOrderPos;
			this->m_pdwOrderPos = NULL;
		}
		this->m_pdwOrderPos = new u32[ dwArrSize ];
		if ( this->m_pdwOrderPos == NULL)
			this->m_dwLastError = ADDRBOOK_MEMORY_ERROR;
		else
		{
			for ( int i = 0; i < (int)dwArrSize; i++ )
				this->m_pdwOrderPos[ i ] = INVALID_INDEX;
		}
	}
	else
		this->m_dwLastError = ADDRBOOK_SIZE_ZERO;
}

// 拷贝函数
template< class T >
CAddArray< T > &CAddArray< T >::ArrayCopy( const CAddArray< T >& NewArray )
{
	if ( m_pArr != NULL )
	{
		delete []m_pArr;
		m_pArr = NULL;
	}
	
	if ( this->m_pdwOrderPos != NULL )
	{
		delete [] this->m_pdwOrderPos;
		this->m_pdwOrderPos = NULL;
	}
	
	this->m_dwArrTotalSize = 0;
	this->m_dwArrUsedSize = 0;
	this->m_OrderOfArr = NewArray.m_OrderOfArr;
	m_pArr = new T[ NewArray.m_dwArrTotalSize ];
	this->m_pdwOrderPos = new u32[ NewArray.m_dwArrTotalSize ];
	if ( ( m_pArr == NULL ) || ( this->m_pdwOrderPos == NULL ) )
		this->m_dwLastError = ADDRBOOK_MEMORY_ERROR;
	else
	{
		this->m_dwArrTotalSize = NewArray.m_dwArrTotalSize;
		this->m_dwArrUsedSize = NewArray.m_dwArrUsedSize;
		memmove( m_pArr, NewArray.m_pArr, this->m_dwArrTotalSize * sizeof( T ) );
		memmove( this->m_pdwOrderPos, NewArray.m_pdwOrderPos, this->m_dwArrTotalSize * sizeof( u32 ) );
	}



	return *this;

}

// 数组扩容
template< class T >
BOOL CAddArray< T >::ExpandArray( u32 dwNewSize )
{
	if ( m_pArr == NULL )
	{
		return FALSE;
	}
	return TRUE;
}

// 将给定元素写入下标为dwIndex的元素,成功返回实际物理索引,否则-1
template< class T >
int CAddArray< T >::StoreElement( T &Element, u32 dwIndex, BOOL bNeedSort /*= TRUE*/ )
{
	if ( m_pArr == NULL )
	{
		this->m_dwLastError = ADDRBOOK_INVALID_POINTER;
		return -1;
	}

	if ( dwIndex >= this->m_dwArrTotalSize )
	{
		this->m_dwLastError = ADDRBOOK_OUTOF_RANGE;
		return -1;
	}
	
	// 是否加入一个完全相同的地址条目到不同的位置
	int nIndex = SearchElemByBinary( Element, bNeedSort );
	if ( ( nIndex != -1 ) && ( nIndex != (int)dwIndex ) )
	{		
		if ( ReCallElement( this->m_TempBuff1, nIndex ) == -1 )
			return -1;

		if ( this->m_TempBuff1 == Element )
		{
			this->m_dwLastError = ADDRBOOK_EXIST_ELEMENT;
			return -1;
		}
	}

	m_pArr[ dwIndex ] = Element;
	this->m_OrderOfArr = this->NO_ORDER;
	
	return dwIndex;
}

template< class T >
int CAddArray< T >::StoreElementFromFile( T &Element, u32 dwIndex )
{
    if ( m_pArr == NULL )
    {
        this->m_dwLastError = ADDRBOOK_INVALID_POINTER;
        return -1;
    }
    
    if ( dwIndex >= this->m_dwArrTotalSize )
    {
        this->m_dwLastError = ADDRBOOK_OUTOF_RANGE;
        return -1;
    }
    
    // 是否加入一个完全相同的地址条目到不同的位置
    //	int nIndex = SearchElemByBinary( Element );
    // 	if ( ( nIndex != -1 ) && ( nIndex != (int)dwIndex ) )
    // 	{		
    // 		if ( ReCallElement( this->m_TempBuff1, nIndex ) == -1 )
    // 			return -1;
    // 
    // 		if ( this->m_TempBuff1 == Element )
    // 		{
    // 			this->m_dwLastError = ADDRBOOK_EXIST_ELEMENT;
    // 			return -1;
    // 		}
    // 	}
    
    m_pArr[ dwIndex ] = Element;
    this->m_OrderOfArr = this->NO_ORDER;
    
    return dwIndex;
}

// 将下标为dwIndex的数组元素取出给当前元素,成功返回元素的物理索引号,否则-1
template< class T >
int CAddArray< T >::ReCallElement( T &Element, u32 dwIndex )
{
	if ( m_pArr == NULL )
	{
		this->m_dwLastError = ADDRBOOK_INVALID_POINTER;
		return -1;
	}

	if ( dwIndex >= this->m_dwArrTotalSize )
	{
		this->m_dwLastError = ADDRBOOK_OUTOF_RANGE;
		return -1;
	}

	Element = m_pArr[ dwIndex ];
	
	return dwIndex;
}

// 交换两个元素
template< class T >
void CAddArray< T >::SwapElement( u32 dwIndex1, u32 dwIndex2 )
{
	return;
}

// 往数组中插入一个元素,返回物理位置索引
template< class T >
int CAddArray< T >::InsertElement( T &Element, 
								   u32 dwIndex /*= INVALID_INDEX*/ )
{
	if ( m_pArr == NULL )
	{
		this->m_dwLastError = ADDRBOOK_INVALID_POINTER;
		return -1;
	}

	if ( this->m_dwArrUsedSize == this->m_dwArrTotalSize )
	{
		this->m_dwLastError = ADDRBOOK_OUTOF_RANGE;
		return -1;
	}

	// 如果当前新增元素没有指定索引号,则将新增元素放入第一个空的物理位置中
	if ( dwIndex == INVALID_INDEX )
	{
		dwIndex = this->SearchFreePos();
		if ( dwIndex == -1 )
			return -1;
	}

	if ( StoreElement( Element, dwIndex ) != -1 )
	{
		// 将新增元素加入有序队列中并排序
		if ( this->AddToOrderTable( dwIndex ) == -1 )
			return -1;

	}
	else
		return -1;

	return dwIndex;
}

template< class T >
int CAddArray< T >::InsertElementFromFile( T &Element, 
                                           u32 dwIndex /*= INVALID_INDEX*/ )
{
    if ( m_pArr == NULL )
    {
        this->m_dwLastError = ADDRBOOK_INVALID_POINTER;
        return -1;
    }
    
    if ( this->m_dwArrUsedSize == this->m_dwArrTotalSize )
    {
        this->m_dwLastError = ADDRBOOK_OUTOF_RANGE;
        return -1;
    }
    
    // 如果当前新增元素没有指定索引号,则将新增元素放入第一个空的物理位置中
    if ( dwIndex == INVALID_INDEX )
    {
        dwIndex = this->SearchFreePos();
        if ( dwIndex == -1 )
            return -1;
    }
    
    if ( StoreElementFromFile( Element, dwIndex ) != -1 )
    {
        // 将新增元素加入有序队列中并排序
        // 		if ( this->AddToOrderTable( dwIndex ) == -1 )
        // 			return -1;
        this->m_pdwOrderPos[ this->m_dwArrUsedSize ] = dwIndex;
        this->m_dwArrUsedSize++;
    }
    else
        return -1;
    
    return dwIndex;
}
// 删除下标为dwIndex的元素,成功返回实际删除的元素索引,否则-1
template< class T >
int CAddArray< T >::DeletesElement( u32 dwIndex )
{
	if ( m_pArr == NULL )
	{
		this->m_dwLastError = ADDRBOOK_INVALID_POINTER;
		return -1;
	}

	if ( dwIndex >= this->m_dwArrTotalSize )
	{
		this->m_dwLastError = ADDRBOOK_OUTOF_RANGE;
		return -1;
	}

	T UnUsedBuff;
	m_pArr[ dwIndex ] = UnUsedBuff;		// 将相应物理位置置为空
	
	// 从有序队列中删除
	if ( this->DelFromOrderTable( dwIndex ) == -1 )
		return -1;

	return dwIndex;
}

// 索引是否已经分配
//template< class T >
// BOOL32 CAddArray< T >::IsIndexExisted( u32 dwIndex )
// {
//     if ( this->m_pdwOrderPos == NULL )
//     {
//         this->m_dwLastError = ADDRBOOK_INVALID_POINTER;
//         return -1;
//     }
//     
//     if ( dwIndex >= this->m_dwArrTotalSize )
//     {
//         this->m_dwLastError = ADDRBOOK_OUTOF_RANGE;
//         return -1;
//     }
//     
// //     T UnUsedBuff;
// //     m_pArr[ dwIndex ] = UnUsedBuff;		// 将相应物理位置置为空
// //     
// //     // 从有序队列中删除
// //     if ( this->DelFromOrderTable( dwIndex ) == -1 )
// //         return -1;
//     for (u32 i = 0 ; i < this->m_dwArrUsedSize; i++)
//     {
//         if (dwIndex == this->m_pdwOrderPos[i])
//         {
//             return TRUE;
//         }
//         else
//         {
//             continue;
//         }
//     }
//     
//     return FALSE;
// }

#if defined(WIN32) 
#pragma pack(pop)
#endif

#endif // ARRAY_H_
