/*****************************************************************************
    模块名      : EqpAgt
    文件名      : eqpagtutility.h
    相关文件    : 
    文件实现功能: EqpAgt业务内部使用结构和类
    作者        : liaokang
    版本        : V4r7  Copyright(C) 2011 KDC, All rights reserved.
-----------------------------------------------------------------------------
    修改记录:
    日  期      版本        修改人          修改内容
    2012/06/18  4.7         liaokang        创建
******************************************************************************/
#ifndef _EQPAGT_UTILITY_H_
#define _EQPAGT_UTILITY_H_

#include "snmpadp.h"
#include "kdvsys.h"
#include "eventid.h"
#include "kdvlog.h"
#include "loguserdef.h"

// 版本号
#define  VER_EQPAGTCOMMBASE         ( LPCSTR )"eqpagtcommbase4.7.1.1.1.120727"

#ifdef WIN32
#ifndef vsnprintf
#define vsnprintf   _vsnprintf
#endif
#endif

// 字符串最大长度
#define  EQPAGT_MAX_LENGTH          (u16)256
#define  EQPAGT_MAX_PATH_LEN        (u16)128  // 路径最大长度
#define  EQPAGT_MAX_NAME_LEN        (u16)128  // 文件名最大长度

// log打印等级定义，对应kdvlog
#define	 LOG_OFF				    (u8)0	// 关闭打印
#define	 LOG_ERROR				    (u8)1	// 程序运行错误(逻辑或业务出错),必须输出
#define	 LOG_WARNING			    (u8)2	// 告警信息, 可能正确, 也可能错误
#define	 LOG_KEYSTATUS			    (u8)3	// 程序运行到一个关键状态时的信息输出
#define	 LOG_DETAIL				    (u8)4	// 普通信息, 最好不要写进log文件中

// 状态
#define  STATUS_NORMAL	            (u8)0   // 状态正常
#define  STATUS_ABNORMAL            (u8)1   // 状态异常

// 告警返回值
#define  NO_ALARM                   (u8)0   // 无告警
#define  ON_ALARM                   (u8)1   // 打开告警
#define  OFF_ALARM                  (u8)2   // 关闭告警

#define  MAXNUM_ALARM_NODES         (u8)5   // 最大支持5个节点同时告警

// 函数返回值
#define  NO_TRAP                    (u16)0  // 无Trap产生
#define  GENERATE_TRAP              (u16)1  // 产生Trap

// 由于Linux在inet_addr时保护 "0"认为是错误地址，所以需要保护一下
// 建议所有Linux相关的地址转换使用本宏。
// 本宏要求字符串以\0结尾
#define INET_ADDR( szAddr )   ( strncmp(szAddr, "0", strlen(szAddr)) == 0 ? 0 : inet_addr(szAddr) )

// 释放指定表的内存
#define FREE_TABLE_MEM(pp, v)	                            \
    do{	                                                    \
    if(NULL == pp) break;	                                \
    for (s32 nLoop = 0; nLoop < (s32)v; nLoop++ )	        \
    {														\
        if( NULL != pp[nLoop] )	                            \
            delete []pp[nLoop];	                            \
            pp[nLoop] = NULL;	                            \
    }	                                                    \
    delete []pp;	                                        \
    pp = NULL;	                                            \
} while (0)	

// 信号量 g_hEqpAgtCfg 操作eqpagtcfg.ini配置文件
BOOL32 CreateSemHandle( SEMHANDLE &hSem );
BOOL32 FreeSemHandle( SEMHANDLE &hSem );
/*lint -save -esym(1788, cSemOpt)*/
#define  ENTER( hSem)               CCfgSemOpt cSemOpt( hSem );
/*lint -restore*/
class CCfgSemOpt{
public:
    CCfgSemOpt( SEMHANDLE& hSem )
    {
        m_hSem = hSem;
        if( OspSemTakeByTime( m_hSem, WAIT_SEM_TIMEOUT ) != TRUE )
        {
            OspPrintf( TRUE, FALSE, "[CCfgSemOpt] semTake error accord to handle<0x%x>\n", hSem );
        }
    }
    ~CCfgSemOpt( )
    {
        OspSemGive( m_hSem );
    }
private:
    SEMHANDLE m_hSem;
};

/************************************************************************/
/*                      支持函数注册  begin                             */
/************************************************************************/
// Get/Set回调函数定义
//（之所以仍然采用dwNodeName，而不采用dwNodeValue，是因为某些节点信息的获
// 取是采用遍历信息表（逐行获取，如u8  byRow = GET_NODE_ROW(dwNodeName)））
typedef u16 (*TEqpAgtCBFunc)(u32 dwNodeName, void * pBuf, u16 * pwBufLen);
// Trap函数定义
typedef u16 (*TEqpAgtTrapFunc)( CNodes& cNodes );
// 生成EqpAgtCfg默认配置文件项定义
typedef void (*TEqpAgtCfgDefValue)( LPCSTR lpszFileName );

// Get操作支持
void EqpAgtGetOper(u32 dwNodeValue, TEqpAgtCBFunc pfFunc );
// Set操作支持
void EqpAgtSetOper(u32 dwNodeValue, TEqpAgtCBFunc pfFunc );
// Trap操作支持
void EqpAgtTrapOper(u32 dwNodeValue, TEqpAgtTrapFunc pfFunc, u32 dwScanTimeSpan, BOOL32 bEnable = TRUE );
// 生成EqpAgtCfg默认配置文件项操作支持
void EqpAgtCfgDefValueOper( TEqpAgtCfgDefValue pfFunc );
/************************************************************************/
/*                      支持函数注册  end                               */
/************************************************************************/


// 由U32得到Ip字符串
void GetIpFromU32( LPSTR lpszDstStr, u32 dwIpInfo );

void EqpAgtLog( u8 byPrintLvl, s8* pszFmt, ... );
void StaticLog( LPCSTR lpszFmt, ...);

/*=============================================================================
模板类：
类名： Link
作用： 链表节点
说明： 如有特别初始化需要，类T应自行提供相应的无参数构造函数
=============================================================================*/
template <class T>
class ArrayNode
{
private:
    T m_Data;                                   // 用于保存结点元素的内容
    u32 m_dwNextId;                             // 指向后继结点的指针

public:      
    ArrayNode() { m_dwNextId = 0; }    
    ArrayNode(const T &Data, u32 dwNextId = 0)
    {
        SetData( Data );
        SetNext( dwNextId );
    }
    
    T * GetData( void ) { return &m_Data; }    
    void SetData( const T &Data ) { m_Data = Data; }    
    u32 GetNext( void ) { return ntohl(m_dwNextId); }
    void SetNext( u32 dwNextId ) { m_dwNextId = htonl(dwNextId); }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

/*=============================================================================
模板类：
类名： LinkArray
作用： 构造一个单向链表
说明： 1、如有特别初始化需要，类T应自行提供相应的无参数构造函数
       2、元素T应支持重载运算符 == : 该要求取消
       3、2009-02-26：取消 IsExist 和 GetPos 两函数，因为这两个函数需要 类T 重载
          运算符 == ，通用性不强
=============================================================================*/
template <class T, u32 dwArrayLen=1>
class LinkArray
{
private: 
    ArrayNode<T> m_atLinkArray[dwArrayLen];
    u32     m_dwHeadId;
    u32     m_dwTailId;                                  // 单链表的头、尾ID号, 索引号等于ID号减1

private: 
    u32     GetIdByLink( ArrayNode<T> * pCur )
    {
        u32 dwArraySize = sizeof(m_atLinkArray)/sizeof(m_atLinkArray[0]);
        u32 dwIdx = 0;
        for ( dwIdx = 0; dwIdx < dwArraySize; dwIdx++ )
        {
            if ( pCur == &(m_atLinkArray[dwIdx]) )
            {
                return (dwIdx+1);
            }
        } 
        return 0;
    }
    ArrayNode<T> * GetLinkById( u32 dwId )
    {
        if ( 0 == dwId || dwId > sizeof(m_atLinkArray)/sizeof(m_atLinkArray[0]) )
        {
            return NULL;
        }
        ArrayNode<T> * pCur = &(m_atLinkArray[dwId-1]);
        return pCur;
    }

    u32     GetHeadId( void ) const { return ntohl(m_dwHeadId); }
    void    SetHeadId( u32 dwHeadId ) { m_dwHeadId = htonl(dwHeadId); }

    u32     GetTailId( void ) const { return ntohl(m_dwTailId); }
    void    SetTailId( u32 dwTailId ) { m_dwTailId = htonl(dwTailId); }

    // 获得空闲存储空间ID
    u32 GetIdleId(void)
    {
        u32 dwArraySize = sizeof(m_atLinkArray)/sizeof(m_atLinkArray[0]);
        u32 dwIdx = 0;
        for ( dwIdx = 0; dwIdx < dwArraySize; dwIdx++ )
        {
            if ( NULL == m_atLinkArray[dwIdx].GetNext() &&
                GetTailId() != (dwIdx+1) )
            {
                return (dwIdx+1);
            }
        }
        return 0;
    }
     // 返回线性表指向第p个元素的指针值
    ArrayNode<T> * GetLink(const u32 &dwPos)        
    {
        if ( dwPos < 0 || dwPos >= Length() )
        {
            return NULL;
        }
        
        u32 dwCount = 0;
        ArrayNode<T> *pCur = GetLinkById( GetHeadId() );
        while ( pCur != NULL && dwCount < dwPos )
        {
            u32 dwNextId = pCur->GetNext();
            pCur = GetLinkById(dwNextId);
            dwCount++;
        }       
        return pCur;
    }   

public: 
    LinkArray() { Clear(); }         // 构造函数
    ~LinkArray() { Clear(); }        // 析构函数
    // 判断链表是否为空
    BOOL32 IsEmpty() const { return ( 0 == m_dwHeadId || 0 == m_dwTailId ); }
    // 将链表存储的内容清除，成为空表
    void   Clear()                                    
    {
        m_dwHeadId = 0;
        m_dwTailId = 0;
        u32 dwArraySize = sizeof(m_atLinkArray)/sizeof(m_atLinkArray[0]);
        u32 dwIdx = 0;
        for ( dwIdx = 0; dwIdx < dwArraySize; dwIdx++ )
        {
            m_atLinkArray[dwIdx].SetNext( 0 );
        }
    }
    // 返回此顺序表的当前实际长度
    u32 Length() 
    {
        ArrayNode<T> *pCur = GetLinkById( GetHeadId() );
        u32 dwCount = 0;
        
        while ( pCur != NULL ) 
        {
            u32 dwNextId = pCur->GetNext();
            pCur = GetLinkById(dwNextId);
            dwCount++;
        }
        return dwCount;
    }    
    // 在表尾添加一个元素value，表的长度增1
    BOOL32 Append(const T &Value)                      
    {
        u32 dwIdleId = GetIdleId();
        ArrayNode<T> *pCur = GetLinkById( dwIdleId );
        if ( NULL == pCur )
        {
            return FALSE;
        }       
        pCur->SetData( Value );
        pCur->SetNext( 0 );
        
        u32 dwOldTailId = GetTailId();
        SetTailId(dwIdleId);

        ArrayNode<T> * pTail = GetLinkById( dwOldTailId );
        if ( NULL != pTail )
        {
            pTail->SetNext( dwIdleId );
        }
        else
        {
            SetHeadId(dwIdleId);
        }        
        return TRUE;
    }
    // 在第dwPos个位置插入数据内容为Value的新结点
    BOOL32 Insert(const u32 &dwPos, const T &Value)    
    {
        ArrayNode<T> *pCur = NULL;
        ArrayNode<T> *pNext = NULL;
        
        u32 dwIdleId = GetIdleId();
        pNext = GetLinkById( dwIdleId ); 
        if ( NULL == pNext )
        {
            return FALSE;
        }
        
        if ( dwPos == 0 )
        {
            pNext->SetData( Value );
            pNext->SetNext( GetHeadId() );
            SetHeadId( dwIdleId );
            if ( 0 == GetTailId() )
            {
                // 原来的表为空
                SetTailId( dwIdleId );
            }
        }
        else 
        {
            if ( (pCur = GetLink(dwPos-1) ) == NULL) 
            {                                                     
                return FALSE;
            }
            pNext->SetData( Value );
            pNext->SetNext( pCur->GetNext() );
            pCur->SetNext( dwIdleId );
            u32 dwPreId = GetIdByLink(pCur);
            if ( dwPreId == GetTailId() )                                          
            {
                SetTailId( dwIdleId );
            }
        }    
        return TRUE;
    }
    // 删除位置i上的元素，表的长度减 1
    BOOL32 Delete(const u32 &dwPos)                    
    {
        ArrayNode<T> *pCur = NULL;
        ArrayNode<T> *pPre = NULL; 
        
        pCur = GetLink( dwPos );
        if ( NULL == pCur )
        {
            return FALSE;
        }
        if ( dwPos > 0 )
        {
            pPre = GetLink( dwPos-1 );
            if ( NULL == pPre )
            {
                return FALSE;
            }
        }

        u32 dwNext = pCur->GetNext();
        pCur->SetNext( 0 );
        if ( NULL == pPre )
        {                           
            SetHeadId( dwNext );                  
        }
        else
        {
            pPre->SetNext( dwNext );         
        }
        if ( GetTailId() == GetIdByLink(pCur) )
        {
            if ( dwNext != 0 )
            {
                SetTailId( dwNext );
            }
            else
            {
                SetTailId( GetIdByLink(pPre) );
            }
        } 
        return TRUE;
    }

    // 返回位置i的元素值 
    T * GetValue(const u32 &dwPos)            
    {
        ArrayNode<T> *p = GetLink(dwPos);
        if ( p == NULL )
        {
            return NULL;
        }
        else
        {
            return p->GetData();
        }
    }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

#endif  // _EQPAGT_UTILITY_H_