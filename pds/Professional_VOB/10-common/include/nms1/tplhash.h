/*=============================================================================
模   块   名: 网管公用模块库
文   件   名: tplhash.h
相 关  文 件: 无
文件实现功能: 管理map型数据的模板类
作        者: 李洪强
版        本: V4.0  Copyright(C) 2003-2006 KDC, All rights reserved.
说        明: 1.设计的目的在于快速映射osp消息及其处理函数。
              2.本类适用的数据类型为简单的数据，即不需要另外申请空间，可以用memcpy实现赋值的功能。
              3.索引元素(TKey)必须是可用<和==比较大小的, 建议使用s32、s16、s8、u32、u16、u8
              4.本模板类不支持嵌套, 不能和TplArray混用
              5.不需要MFC支持
-------------------------------------------------------------------------------
修改记录:
日      期  版本    修改人      修改内容
2005/01/27  3.6     李洪强      创建
2005/11/10  4.0     王昊        功能实现
2005/12/08  4.0     王昊        分离出单独文件
=============================================================================*/

#ifndef _TPLHASH_20051208_H_
#define _TPLHASH_20051208_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define DEFAULT_HASH_SIZE       (int)100    //  默认初始分配空间大小

////////////////////////////////////////////////////////////////////////
// 
//  TplHash     哈希表的模板类
// 
// 哈希表的结构设计如下
//              单个元素
//      [----------------------|-------------------]
//      [ TKey                 |     TItem         ]
// 节点按照从小到大的顺序排列
//
//  插入节点过程
//                             TKey
//                               |
//                           Hash value
//                               |
//                 ------------------------------
//                 |                            |
//          value exist in table            value not exist
//                 |                            |
//          replace value          find appropriate place to save the value
//
//
//   查询过程
//                             TKey
//                               |
//                           Hash value
//                               |
//                    Search in hash entry list
//                               |
//               ---------------------------------
//              |                                 |
//      Find match TKey, return TRUE      No match, return FALSE
//

template<class TKey, class TItem>
class TplHash
{
public:
    //单个节点结构
    typedef struct tagHashNode
    {
        tagHashNode()
        {
            memset( &m_tKey, 1, sizeof (m_tKey) );
        }
        tagHashNode(const TKey &tKey, const TItem &tItem) : m_tKey(tKey)
        {
            m_tItem = tItem;
        }
        TKey    m_tKey;
        TItem   m_tItem;
    } THashNode;

public:
    /*=============================================================================
    函 数 名:TplHash
    功    能:构造函数
    参    数:int nInitSize                  [in]    初始空间大小
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/11/10  4.0     王昊    创建
    =============================================================================*/
    TplHash(int nInitSize = DEFAULT_HASH_SIZE);

    /*=============================================================================
    函 数 名:TplHash
    功    能:拷贝构造函数
    参    数:const TplArray &tplHash        [in]    源对象
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/11/10  4.0     王昊    创建
    =============================================================================*/
    TplHash(const TplHash &tplHash);

    /*=============================================================================
    函 数 名:~TplHash
    功    能:析构函数
    参    数:无
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/11/10  4.0     王昊    创建
    =============================================================================*/
    ~TplHash();

    /*=============================================================================
    函 数 名:operator=
    功    能:赋值
    参    数:const TplArray &tplHash        [in]    源对象
    注    意:无
    返 回 值:自身
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/11/09  4.0     王昊    创建
    =============================================================================*/
    inline TplHash& operator=(const TplHash &tplHash);

    /*=============================================================================
    函 数 名:IsExist
    功    能:判断键值是否存在
    参    数:const TKey &tKey               [in]    键值
    注    意:无
    返 回 值:存在:TRUE; 不存在: FALSE
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/11/09  4.0     王昊    创建
    =============================================================================*/
    inline BOOL IsExist(const TKey &tKey) const { return Find( tKey ) != -1; }

    /*=============================================================================
    函 数 名:SetAt
    功    能:将一个键值与一个数据项注册在表中
    参    数:const TKey &tKey               [in]    键值
             const TItem &tItem             [in]    数据项
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/01/26	3.6		李洪强	创建
    2005/11/09  4.0     王昊    设计调整
    =============================================================================*/
    void SetAt(const TKey &tKey, const TItem &tItem);

    /*=============================================================================
    函 数 名:GetAt
    功    能:获取键值
    参    数:const TKey &tKey               [in]    键值
             TItem &tItem                   [out]    数据项
    注    意:无
    返 回 值:找到返回TRUE，没找到返回FALSE
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/01/26	3.6		李洪强	创建
    2005/11/09  4.0     王昊    设计调整
    =============================================================================*/
    inline BOOL GetAt(const TKey &tKey, TItem &tItem) const;

    /*=============================================================================
    函 数 名:Delete
    功    能:删除键值
    参    数:const TKey &tKey               [in]    键值
    注    意:无
    返 回 值:找到返回TRUE，没找到返回FALSE
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/11/09  4.0     王昊    创建
    =============================================================================*/
    inline BOOL Delete(const TKey &tKey);

    /*=============================================================================
    函 数 名:GetEntryNum
    功    能:获取节点总数
    参    数:无
    注    意:无
    返 回 值:节点总数
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/11/09  4.0     王昊    创建
    =============================================================================*/
    inline int GetEntryNum(void) const { return m_nUsedSize; }

    /*=============================================================================
    函 数 名:Clear
    功    能:清空
    参    数:无
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/11/09  4.0     王昊    创建
    =============================================================================*/
    inline void Clear(void);

private:
    /*=============================================================================
    函 数 名:Find
    功    能:查找键值
    参    数:const TKey &tKey               [in]    键值
    注    意:无
    返 回 值:返回键值在m_ptHashTable的索引号, -1为没找到
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/11/09  4.0     王昊    创建
    =============================================================================*/
    inline int Find(const TKey &tKey) const;

private:
    THashNode   *m_ptHashTable; //保存结点信息的表
    int         m_nTableSize;   //分配空间大小
    int         m_nUsedSize;    //使用大小
};

/*=============================================================================
函 数 名:TplHash
功    能:构造函数
参    数:int nInitSize                  [in]    初始空间大小
注    意:无
返 回 值:无
-------------------------------------------------------------------------------
修改纪录:
日      期  版本    修改人  修改内容
2005/11/10  4.0     王昊    创建
=============================================================================*/
template<class TKey, class TItem>
TplHash<TKey, TItem>::TplHash(int nInitSize /* = DEFAULT_HASH_SIZE */)
    : m_nUsedSize(0), m_nTableSize(nInitSize), m_ptHashTable(NULL)
{
    m_ptHashTable = (nInitSize > 0 ? new THashNode [nInitSize] : NULL);
}

/*=============================================================================
函 数 名:TplHash
功    能:拷贝构造函数
参    数:const TplArray &tplHash        [in]    源对象
注    意:无
返 回 值:无
-------------------------------------------------------------------------------
修改纪录:
日      期  版本    修改人  修改内容
2005/11/10  4.0     王昊    创建
=============================================================================*/
template<class TKey, class TItem>
TplHash<TKey, TItem>::TplHash(const TplHash &tplHash)
    : m_nTableSize(tplHash.m_nUsedSize), m_nUsedSize(tplHash.m_nUsedSize),
      m_ptHashTable(NULL)
{
    if ( this != &tplHash && m_nUsedSize > 0 )
    {
        m_ptHashTable = new THashNode [m_nUsedSize];
        ASSERT( m_ptHashTable != NULL );
        memcpy( m_ptHashTable, tplHash.m_ptHashTable,
                sizeof (THashNode) * m_nUsedSize );
    }
}

/*=============================================================================
函 数 名:~TplHash
功    能:析构函数
参    数:无
注    意:无
返 回 值:无
-------------------------------------------------------------------------------
修改纪录:
日      期  版本    修改人  修改内容
2005/11/10  4.0     王昊    创建
=============================================================================*/
template<class TKey, class TItem>
TplHash<TKey, TItem>::~TplHash()
{
    if ( m_ptHashTable != NULL )
    {
        delete [] m_ptHashTable;
        m_ptHashTable = NULL;
    }
}

/*=============================================================================
函 数 名:operator=
功    能:赋值
参    数:const TplArray &tplHash        [in]    源对象
注    意:无
返 回 值:自身
-------------------------------------------------------------------------------
修改纪录:
日      期  版本    修改人  修改内容
2005/11/09  4.0     王昊    创建
=============================================================================*/
template<class TKey, class TItem>
inline TplHash<TKey, TItem>& TplHash<TKey, TItem>::operator=(const TplHash &tplHash)
{
    if ( this != &tplHash )
    {
        m_nUsedSize = tplHash.m_nUsedSize;
        m_nTableSize = m_nUsedSize;

        if ( m_ptHashTable != NULL )
        {
            delete [] m_ptHashTable;
            m_ptHashTable = NULL;
        }

        if ( m_nUsedSize > 0 )
        {
            m_ptHashTable = new THashNode [m_nUsedSize];
            ASSERT( m_ptHashTable != NULL );
            memcpy( m_ptHashTable, tplHash.m_ptHashTable,
                    sizeof (THashNode) * m_nUsedSize );
        }
    }

    return (*this);
}

/*=============================================================================
函 数 名:Find
功    能:查找键值
参    数:const TKey &tKey               [in]    键值
注    意:无
返 回 值:返回键值在m_ptHashTable的索引号, -1为没找到
-------------------------------------------------------------------------------
修改纪录:
日      期  版本    修改人  修改内容
2005/11/09  4.0     王昊    创建
=============================================================================*/
template<class TKey, class TItem>
inline int TplHash<TKey, TItem>::Find(const TKey &tKey) const
{
    int nRight  = m_nUsedSize - 1;
    int nLeft   = 0;
    int nMid    = nRight / 2;

    if ( m_nUsedSize == 0 )
        return -1;

    while ( nRight >= nLeft )
    {
        if ( m_ptHashTable[nMid].m_tKey == tKey )
        {
            return nMid;
        }
        else if ( m_ptHashTable[nMid].m_tKey < tKey )
        {
            nLeft = nMid + 1;
        }
        else
        {
            nRight = nMid - 1;
        }

        nMid = (nRight + nLeft + 1) / 2;
    }

    return -1;
}

/*=============================================================================
函 数 名:SetAt
功    能:将一个键值与一个数据项注册在表中
参    数:const TKey &tKey               [in]    键值
         const TItem &tItem             [in]    数据项
注    意:无
返 回 值:无
-------------------------------------------------------------------------------
修改纪录:
日      期  版本    修改人  修改内容
2005/01/26	3.6		李洪强	创建
2005/11/09  4.0     王昊    设计调整
=============================================================================*/
template<class TKey, class TItem>
void TplHash<TKey, TItem>::SetAt(const TKey &tKey, const TItem &tItem)
{
    int nRight  = m_nUsedSize - 1;
    int nLeft   = 0;
    int nMid    = nRight / 2;
    THashNode tHashNode( tKey, tItem );

    if ( m_nUsedSize == 0 )  //  当前没有元素，直接添加
    {
        if ( m_nTableSize == 0 )
        {
            m_ptHashTable = new THashNode( tKey, tItem );
            ASSERT( m_ptHashTable != NULL );
            m_nTableSize = 1;
        }
        else
        {
            m_ptHashTable[0] = tHashNode;
        }
        m_nUsedSize = 1;
        return;
    }

    while ( nRight >= nLeft )
    {
        if ( m_ptHashTable[nMid].m_tKey == tKey )  //  元素已存在，刷新
        {
            m_ptHashTable[nMid] = tHashNode;
            return;
        }
        else if ( m_ptHashTable[nMid].m_tKey < tKey )
        {
            nLeft = nMid + 1;
        }
        else
        {
            nRight = nMid - 1;
        }

        nMid = (nRight + nLeft + 1) / 2;
    }

    if ( m_nTableSize > m_nUsedSize )   //  还有空间
    {
        memmove( m_ptHashTable + nMid + 1, m_ptHashTable + nMid,
                 (m_nUsedSize - nMid) * sizeof (THashNode) );
    }
    else    //  需要另外分配空间
    {
        THashNode *ptOldTable = m_ptHashTable;
        m_ptHashTable = new THashNode [m_nUsedSize + 1];
        ASSERT( m_ptHashTable != NULL );
        memcpy( m_ptHashTable, ptOldTable, nMid * sizeof (THashNode) );
        memcpy( m_ptHashTable + nMid + 1, ptOldTable + nMid,
                (m_nUsedSize - nMid) * sizeof (THashNode) );
        m_nTableSize++;
        delete [] ptOldTable;
    }
    m_ptHashTable[nMid] = tHashNode;
    m_nUsedSize++;

    return;
}

/*=============================================================================
函 数 名:GetAt
功    能:获取键值
参    数:const TKey &tKey               [in]    键值
         TItem &tItem                   [out]    数据项
注    意:无
返 回 值:找到返回TRUE，没找到返回FALSE
-------------------------------------------------------------------------------
修改纪录:
日      期  版本    修改人  修改内容
2005/01/26	3.6		李洪强	创建
2005/11/09  4.0     王昊    设计调整
=============================================================================*/
template<class TKey, class TItem>
inline BOOL TplHash<TKey, TItem>::GetAt(const TKey &tKey, TItem &tItem) const
{
    int nIndex = Find( tKey );
    if ( nIndex != -1 )
    {
        tItem = m_ptHashTable[nIndex].m_tItem;
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/*=============================================================================
函 数 名:Delete
功    能:删除键值
参    数:const TKey &tKey               [in]    键值
注    意:无
返 回 值:找到返回TRUE，没找到返回FALSE
-------------------------------------------------------------------------------
修改纪录:
日      期  版本    修改人  修改内容
2005/11/09  4.0     王昊    创建
=============================================================================*/
template<class TKey, class TItem>
inline BOOL TplHash<TKey, TItem>::Delete(const TKey &tKey)
{
    int nIndex = Find( tKey );

    if ( nIndex != -1 )
    {
        memmove( m_ptHashTable + nIndex, m_ptHashTable + nIndex + 1,
                 (--m_nUsedSize - nIndex) * sizeof (THashNode) );
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/*=============================================================================
函 数 名:Clear
功    能:清空
参    数:无
注    意:无
返 回 值:无
-------------------------------------------------------------------------------
修改纪录:
日      期  版本    修改人  修改内容
2005/11/09  4.0     王昊    创建
=============================================================================*/
template<class TKey, class TItem>
inline void TplHash<TKey, TItem>::Clear(void)
{
    m_nUsedSize = 0;
    // 顾振华@2005.12.28  这里要清空所有状态
    m_nTableSize = 0;
    if ( m_ptHashTable != NULL )
    {
        delete [] m_ptHashTable;
        m_ptHashTable = NULL;
    }
}

#endif  //  _TPLHASH_20051208_H_