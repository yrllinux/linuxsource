#ifndef ITEM_20100726_H
#define ITEM_20100726_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <map>
#include <list>
#pragma warning( disable : 4786 )

using namespace std;
#ifndef Interface
#define Interface class
#endif


/*---------------------------------------------------------------------
* 类	名：OutPut
* 功    能：IItem的附加功能类，为其提供打印功能，采用mixin
* 特殊说明：数据类型为CMt，如有其他类型，自行添加
* 修改记录：
* 日期			版本		修改人		修改记录
* 2010/07/13	v1.0		牟兴茂		创建
----------------------------------------------------------------------*/
template <class T> class OutPut
{
public:
	OutPut()
	{
		m_pBase = (T*)(this);
	}
	void Ex_Print()
	{
		OutPutItem( m_pBase );
	}
	void OutPutItem(T* pItem)
	{
		if ( pItem->GetItemType() == T::emItem )
		{
			CMt* pMt = pItem->GetItemData();
			cout << "I'm Item, My name is " << (int)pMt->GetEqpId() << endl;
		}
		else
		{
			s32 nItem = pItem->GetItemCount();
			
			cout << "-----------------------------------" << endl;
			cout << "I'm Group, My name is  " << pItem->m_keyName << " ,I have " << nItem << " Items" << endl;		
			
			for ( s32 i = 0; i < nItem; i++ )
			{
				T* pSubItem = pItem->GetItemByIndex( i );
				OutPutItem( pSubItem );
			}
		}
	}
protected:
private:
	T* m_pBase;
};

//----------------------------------------------------------
//类    名: IItem
//功    能: 条目基类 采用Composite模式
//主要接口: 提供删除、添加、设置、等操作,
//特殊说明: DATA需要重载操作符== = 由于使用了map，KEY需要重载<操作符
//修改记录:
//日期		版本		修改人		修改记录
//2010/4/26	V1.0		muxingmao	创建
//----------------------------------------------------------
template<typename DATA,typename KEY>
Interface IItem 
{
public:
	enum EmItemType
	{
		emItem,
		emGroup
	};
public:
	IItem()
	{
		m_emItemType	= emItem;
		m_pParentItem	= NULL;
	}
	virtual ~IItem()
	{
		if ( NULL != m_ptItemData )
		{
			delete m_ptItemData;
			m_ptItemData = NULL;
		}
	}
	// 添加子项 内部会拷贝一次，所以如果在外面new的参数，请自行delete
	virtual BOOL32				AddItem( KEY key, IItem<DATA,KEY>* pItem ) = 0;
	// 获得子项
	virtual IItem<DATA,KEY>*	GetItemByKey( KEY key ) = 0;
	// 获得子项
	virtual IItem<DATA,KEY>*	GetItemByIndex( u32 nIndex ) = 0;
	// 删除子项
	virtual BOOL32				DeleteItem( KEY key, BOOL32 bDeleteChild = TRUE ) { return FALSE; }
	// 删除所有子项
	virtual BOOL32				DeleteAllItem( BOOL32 bDeleteChild = TRUE ) { return FALSE; }
	// 获得子项数据
	virtual DATA*				GetItemData() { return NULL; }
	// 设置条目数据
	virtual BOOL32				SetItemData( DATA* ptItemData ) { return FALSE; }
	// 获得条目数量
	virtual u32					GetItemCount() { return 0;}
	// 存储读取条目 有序列化需求的时候添加
	//	virtual void				Serialize( CArchive& ar ) = 0;
	// 修改条目
	virtual BOOL32				ModifyItemData( DATA* ptItemData ) = 0;
	// 修改条目
	virtual BOOL32				ModifyItemKey( KEY keyNew ) = 0;
	// 获得条目名称
	virtual KEY					GetItemName(){ return m_keyName; }
	// 获得条目类型
	virtual EmItemType			GetItemType(){ return m_emItemType; }
	// 获得父节点
	virtual IItem<DATA,KEY>*	GetParentItem(){ return m_pParentItem; }
	// 复制条目
	virtual IItem<DATA,KEY>*	Clone() = 0;
	// 条目名
	KEY						m_keyName;
	// 条目数据
	DATA					*m_ptItemData;	
	// 条目类型
	EmItemType				m_emItemType;
	// 父条目 用户修改条目
	IItem<DATA,KEY>*		m_pParentItem;
};




//----------------------------------------------------------
//类    名: CItem
//功    能：条目类
//主要接口: 同基类
//修改记录:
//日期		版本		修改人		修改记录
//2010/4/26	V1.0		muxingmao	创建
//----------------------------------------------------------
template<class DATA,typename KEY>
class CItem : public IItem<DATA,KEY> 
{
	friend class OutPut<CItem>;
public:
	CItem()
	{
		m_emItemType = emItem;
		m_ptItemData = NULL;
	}
	
	virtual BOOL32	AddItem( KEY key, IItem<DATA,KEY>* pItem ) { return FALSE; }	
	virtual BOOL32	DeleteItem( KEY key, BOOL32 bDeleteData = TRUE ) { return FALSE; }
	virtual BOOL32	DeleteAllItem( BOOL32 bDeleteData = TRUE ) { return FALSE; }
	virtual DATA*	GetItemData() { return m_ptItemData; }
	virtual u32		GetItemCount() { return 0; };	
	virtual IItem<DATA,KEY>*	GetItemByKey( KEY key ) { return NULL; }
	virtual IItem<DATA,KEY>*	GetItemByIndex( u32 nIndex ) { return NULL; }

	virtual BOOL32	SetItemData( DATA* ptItemData )
	{
		if ( ptItemData == m_ptItemData || ptItemData == NULL )
		{
			return FALSE;
		}
		if ( NULL == m_ptItemData )
		{
			m_ptItemData = new DATA;
		}
		
		*m_ptItemData = *ptItemData;
		return TRUE;
	}
	
	
	virtual BOOL32	ModifyItemData( DATA* ptItemData )
	{
		if ( NULL == m_ptItemData )
		{
			return SetItemData( ptItemData );
		}
		
		if ( m_ptItemData == ptItemData )
		{
			return FALSE;
		}
		
		*m_ptItemData = *ptItemData;
		
		return TRUE;
	}
	
	virtual BOOL32	ModifyItemKey( KEY keyNew )
	{
		if ( m_pParentItem == NULL)
		{
			return FALSE;
		}
		
		IItem<DATA,KEY>* pItem = Clone();
		
		if ( TRUE == m_pParentItem->AddItem( keyNew, pItem ) )
		{
			m_pParentItem->DeleteItem( m_keyName );
			
			return TRUE;
		}
		
		return FALSE;
	}
	
	virtual IItem<DATA,KEY>*	Clone()
	{
		IItem<DATA,KEY>* pItem = new CItem<DATA,KEY>();
		pItem->m_keyName = m_keyName;
		pItem->SetItemData( m_ptItemData );
		pItem->m_pParentItem = m_pParentItem;
		
		return pItem;
	}
};

//----------------------------------------------------------
//类    名: CItemGroup
//功    能：条目组类
//主要接口: 同基类
//修改记录: 
//日期		版本		修改人		修改记录
//2010/4/26	V1.0		muxingmao	创建
//----------------------------------------------------------
template<class DATA,typename KEY>
class CItemGroup : public IItem<DATA,KEY>
{
public:
	CItemGroup()
	{
		m_emItemType = emGroup;
		m_ptItemData = NULL;
	}
	
	~CItemGroup()
	{
		DeleteAllItem();
	}

	virtual BOOL32		ModifyItemData( DATA* ptItemData )
	{ 
		return SetItemData( ptItemData );
	}

	virtual DATA*		GetItemData() { return m_ptItemData; }
	
	virtual BOOL32		SetItemData( DATA* ptItemData )
	{ 
		if ( ptItemData == m_ptItemData || ptItemData == NULL )
		{
			return FALSE;
		}
		if ( NULL == m_ptItemData )
		{
			m_ptItemData = new DATA;
		}
		*m_ptItemData = *ptItemData;
		return TRUE; 
	}
	
	virtual u32			GetItemCount()
	{	
		return m_pList.size(); 
	}
	
	virtual BOOL32		AddItem( KEY key, IItem<DATA,KEY>* pItem )
	{
		if ( GetItemByKey( key ) == NULL)
		{		
			IItem<DATA,KEY>* pTemp = pItem->Clone();
			
			pTemp->m_pParentItem = this;
			
			pTemp->m_keyName = key;
			
			m_pList.insert( map<KEY,IItem<DATA,KEY>*>::value_type( key, pTemp ));
			
			return TRUE;
		}
		return FALSE;
	}
	
	virtual IItem<DATA,KEY>*		GetItemByKey( KEY key )
	{
		map<KEY,IItem<DATA,KEY>*>::iterator itr = m_pList.find( key );
		
		IItem<DATA,KEY>* pItem = NULL;
		if ( itr != m_pList.end() )
		{
			pItem = itr->second;
		}
		return pItem;
	}
	
	virtual IItem<DATA,KEY>*		GetItemByIndex( u32 nIndex )
	{
		IItem<DATA,KEY>* pItem = NULL;
		if ( nIndex >= m_pList.size() )
		{
			return pItem;
		}
		
		map<KEY,IItem<DATA,KEY>*>::iterator itr = m_pList.begin();
		
		while ( nIndex > 0 )
		{
			itr++;
			nIndex--;
		}
		pItem = itr->second;
		
		return pItem;
	}
	
	virtual BOOL32		DeleteItem( KEY key, BOOL32 bDeleteData = TRUE )
	{
		map<KEY,IItem<DATA,KEY>*>::iterator itr = m_pList.find( key );
		
		if ( m_pList.end() == itr )
		{
			return FALSE;
		}
		
		if ( bDeleteData == TRUE )
		{
			delete itr->second;
			
			itr->second = NULL;
		}
		
		m_pList.erase( itr );
		
		return TRUE;
	}
	
	virtual BOOL32		DeleteAllItem( BOOL32 bDeleteData = TRUE )
	{
		map<KEY,IItem<DATA,KEY>*>::iterator itr = m_pList.begin();
		
		while( itr != m_pList.end() )
		{
			itr->second->DeleteAllItem( bDeleteData );
			if ( bDeleteData == TRUE )
			{
				delete itr->second;
				itr->second = NULL;
			}			
			itr++;
		}
		
		m_pList.clear();
		
		return TRUE;
	}
	/*	有序列化需求的时候添加
	virtual void		Serialize( CArchive& ar ) 
	{
	if (ar.IsStoring())
	{
	s32 nItem = m_pList.size();
	
	  ar << nItem;
	  m_keyName.Serialize( ar );
	  
		map<KEY,IItem<DATA,KEY>*>::iterator itr = m_pList.begin();		
		for ( ; itr != m_pList.end(); itr++ )
		{
		
		  KEY key = (*itr).first;
		  key.Serialize( ar );
		  IItem<DATA,KEY> *pItem = itr->second;	
		  
			ar << (WORD)pItem->m_emItemType;
			
			  pItem->Serialize(ar);
			  }
			  }
			  else
			  {
			  s32 nItem = 0;
			  ar >> nItem;
			  m_keyName.Serialize( ar );
			  
				for (s32 i = 0; i < nItem; i++ )
				{
				IItem<DATA,KEY> *pItem;
				
				  KEY key;
				  key.Serialize( ar );
				  
					WORD wType;
					ar >> wType;
					
					  EmItemType emType = (EmItemType)wType;
					  switch (emType)
					  {
					  case emItem:
					  {
					  pItem = new CItem<DATA,KEY>;
					  }
					  break;
					  case emGroup:
					  {
					  pItem = new CItemGroup<DATA,KEY>;
					  }
					  break;
					  default:
					  {
					  pItem = new CItem<DATA,KEY>;
					  }	
					  break;
					  }
					  pItem->Serialize(ar);
					  AddItem(key,pItem);
					  }
					  }
					  }
	*/
	virtual BOOL32		ModifyItemKey( KEY keyNew )
	{		
		if ( m_pParentItem == NULL)
		{
			m_keyName = keyNew;
			return FALSE;
		}
		
		m_pParentItem->DeleteItem( m_keyName, FALSE );
		m_keyName = keyNew;
		if ( m_pParentItem != NULL )
		{			
			CItemGroup* pParentItem = (CItemGroup*)m_pParentItem;
			pParentItem->m_pList.insert( map<KEY,IItem<DATA,KEY>*>::value_type( keyNew, this ));
		}
		
		return TRUE;
	}
	
	virtual IItem<DATA,KEY>*		Clone()
	{
		IItem<DATA,KEY>* pGroup = new CItemGroup<DATA,KEY>();
		pGroup->m_keyName = m_keyName;
		pGroup->m_pParentItem = m_pParentItem;
		if ( m_ptItemData != NULL )
		{
			pGroup->SetItemData( m_ptItemData );
		}
		
		map<KEY,IItem<DATA,KEY>*>::iterator itr = m_pList.begin();
		
		while ( itr != m_pList.end() )
		{
			pGroup->AddItem( itr->first, itr->second );
			itr++;	
		}
		return pGroup;
	}
public:
	// 条目列表
	map<KEY,IItem<DATA,KEY>*> m_pList;
};

// template<class DATA,class KEY>
// class CNoSortItemGroup : public IItem<DATA,KEY>
// {
// public:
// 
// 	typedef list<IItem<DATA,KEY>*> ListType;
// 	typedef map<KEY,ListType::iterator> MapType;
// 	typedef ListType::iterator iterator;
// 	typedef IItem<DATA,KEY> ItemType;
// 	//参数为ItemType*，返回值为BOOL32的函数
// 	typedef FastDelegate1<ItemType*,BOOL32> Func;
// 
// 	CNoSortItemGroup()
// 	{
// 		m_emItemType = emGroup;
// 		m_ptItemData = NULL;
// 	}
// 	
// 	~CNoSortItemGroup()
// 	{
// 		DeleteAllItem();
// 	}
// 
// 	virtual BOOL32 AddItem( KEY key, IItem<DATA,KEY>* pItem )
// 	{
// 		return InsertItem( End(), key, pItem );
// 	}
// 	
// 	virtual IItem<DATA,KEY>* AddItem( KEY key, DATA* pData, EmItemType emType )
// 	{
// 		if ( GetItemByKey( key ) == NULL)
// 		{		
// 			IItem<DATA,KEY>* pTemp = NULL;
// 			if ( emType == emItem )
// 			{
// 				pTemp = new CItem<DATA,KEY>;
// 			}
// 			else if ( emType == emGroup )
// 			{
// 				pTemp = new CNoSortItemGroup<DATA,KEY>;
// 			}
// 			if ( pTemp == 0 )
// 			{
// 				return pTemp;
// 			}
// 		
// 			pTemp->m_pParentItem = this;
// 			pTemp->m_keyName = key;
// 		    pTemp->m_ptItemData = new DATA;
// 			*(pTemp->m_ptItemData) = *pData;
// 			m_map[key] = m_list.insert( End(), pTemp );
// 			return pTemp;
// 		}
// 		return 0;
// 	}
// 
// 	virtual BOOL32 InsertItem( iterator it, KEY key,IItem<DATA,KEY>* pItem )
// 	{
// 		if ( GetItemByKey(key) == NULL )
// 		{
// 			IItem<DATA,KEY>* pTemp = pItem->Clone();
// 			pTemp->m_pParentItem = this;
// 			pTemp->m_keyName = key;
// 			m_map[key] = m_list.insert( it, pTemp );
// 			return TRUE;
// 		}
// 		return FALSE;
// 	}
// 
// 	virtual BOOL32 InsertItem( u32 nIndex, KEY key, IItem<DATA,KEY>* pItem )
// 	{
// 		if (nIndex >= GetItemCount())
// 		{
// 			return FALSE;
// 		}
// 		return InsertItem( GetIterByIndex(nIndex), key, pItem );
// 	}
// 
// 	virtual iterator GetIterByKey(KEY key)
// 	{
// 		MapType::iterator it = m_map.find( key );
// 		return (it == m_map.end()) ? End() : it->second;
// 	}
// 	
// 	virtual iterator GetIterByIndex(u32 nIndex)
// 	{
// 		if ( nIndex >= GetItemCount() )
// 		{
// 			return End();
// 		}
// 		iterator it = Begin();
// 		if ( nIndex * 2 <= GetItemCount() )
// 		{				
// 			for( ; nIndex > 0; --nIndex )
// 			{
// 				++it;
// 			}
// 		}
// 		else
// 		{
// 			// it多减一次，由begin()变为end()
// 			for ( --it; nIndex <= GetItemCount(); ++nIndex )
// 			{
// 				--it;
// 			}
// 		}
// 		return it;
// 	}
// 
// 	//删除需要返回值吗？
// 	virtual BOOL32 DeleteItem( KEY key, BOOL32 bDeleteData = TRUE )
// 	{
// 		MapType::iterator itMap = m_map.find( key );
// 		if ( m_map.end() == itMap )
// 		{
// 			return FALSE;
// 		}
// 		ListType::iterator itList = itMap->second;
// 
// 		if ( bDeleteData == TRUE )
// 		{			
// 			delete *itList;
// 		}
// 		m_list.erase(itList);
// 		m_map.erase(itMap);		
// 		return TRUE;		
// 	}
// 
// 	virtual BOOL32 DeleteItem( iterator it, BOOL32 bDeleteData = TRUE )
// 	{
// 		if ( it == End() )
// 		{
// 			return FALSE;
// 		}
// 
// 		KEY keyTemp = (*it)->GetItemName();
// 		m_map.erase(keyTemp);
// 
// 		if (bDeleteData)
// 		{
// 			delete (*it);
// 		}
// 		m_list.erase(it);
// 		return TRUE;		
// 	}
// 
// 	virtual BOOL32 DeleteItemByIndex( u32 nIndex, BOOL32 bDeleteData = TRUE )
// 	{
// 		return DeleteItem( GetIterByIndex(nIndex), bDeleteData );
// 	}
// 
// 	virtual BOOL32	ModifyItemData( DATA* ptItemData )
// 	{ 
// 		if ( NULL == m_ptItemData )
// 		{
// 			return SetItemData( ptItemData );
// 		}
// 		
// 		if ( m_ptItemData == ptItemData )
// 		{
// 			return FALSE;
// 		}
// 		
// 		*m_ptItemData = *ptItemData;
// 		
// 		return TRUE;
// 	}
// 	virtual DATA*	GetItemData() { return m_ptItemData; }
// 	
// 	virtual BOOL32	SetItemData( DATA* ptItemData )
// 	{ 
// 		if ( ptItemData == m_ptItemData || ptItemData == NULL )
// 		{
// 			return FALSE;
// 		}
// 		if ( NULL == m_ptItemData )
// 		{
// 			m_ptItemData = new DATA;
// 		}
// 		*m_ptItemData = *ptItemData;
// 		return TRUE; 
// 	}
// 	
// 	virtual s32	GetItemCount()
// 	{	
// 		return m_list.size(); 
// 	}
// 	
// 	virtual iterator Begin()
// 	{
// 		return m_list.begin();
// 	}
// 	virtual iterator End()
// 	{
// 		return m_list.end();
// 	}
// 
// 	BOOL32 ForEach (Func& theFunc)
// 	{
// 		iterator it = m_list.begin();
// 		while ( it != m_list.end() )
// 		{
// 			theFunc(*it++);
// 		}
// 		return TRUE;
// 	}
// 
// 	BOOL32 FindIf( int times, Func& theFunc )
// 	{
// 		iterator it = m_list.begin();
// 		int i = 0;
// 		while ( it != m_list.end() && i < times )
// 		{
// 			if (theFunc(*it++) == TRUE)
// 			{
// 				i++;
// 			}
// 		}
// 		return i;
// 	}
// 
// 	virtual IItem<DATA,KEY>* GetItemByKey( KEY key )
// 	{
// 		MapType::iterator itMap = m_map.find( key );
// 		return (itMap == m_map.end()) ? NULL : *(itMap->second);
// 	}
// 	
// 	virtual IItem<DATA,KEY>* GetItemByIndex( s32 nIndex )
// 	{
// 		iterator it = GetIterByIndex(nIndex);
// 		return (it == End()) ? NULL : *it;
// 	}
// 	
// 	virtual BOOL32 DeleteAllItem( BOOL32 bDeleteData = TRUE )
// 	{
// 		if (bDeleteData)
// 		{
// 			iterator it = Begin();
// 			while( it!= End() )
// 			{
// 				delete (*it++);
// 			}
// 		}
// 		m_list.clear();
// 		m_map.clear();
// 		return TRUE;
// 	}
// 	/*	有序列化需求的时候添加
// 	virtual void	Serialize( CArchive& ar ) 
// 	{
// 		if (ar.IsStoring())
// 		{
// 			s32 nItem = m_pList.size();
// 			ar << nItem;
// 			m_keyName.Serialize( ar );
// 	  
// 			map<KEY,IItem<DATA,KEY>*>::iterator itr = m_pList.begin();		
// 			for ( ; itr != m_pList.end(); itr++ )
// 			{
// 				KEY key = (*itr).first;
// 				key.Serialize( ar );
// 				IItem<DATA,KEY> *pItem = itr->second;	
// 				ar << (WORD)pItem->m_emItemType;
// 				pItem->Serialize(ar);
// 			}
// 		}
// 		else
// 		{
// 			s32 nItem = 0;
// 			ar >> nItem;
// 			m_keyName.Serialize( ar );
// 
// 			for (s32 i = 0; i < nItem; i++ )
// 			{
// 				IItem<DATA,KEY> *pItem;
// 			
// 				KEY key;
// 				key.Serialize( ar );
// 			  
// 				WORD wType;
// 				ar >> wType;
// 				
// 				EmItemType emType = (EmItemType)wType;
// 				switch (emType)
// 				{
// 				case emItem:
// 					{
// 						pItem = new CItem<DATA,KEY>;
// 					}
// 					break;
// 				case emGroup:
// 					{
// 						pItem = new CItemGroup<DATA,KEY>;
// 					}
// 					break;
// 				default:
// 					{
// 						pItem = new CItem<DATA,KEY>;
// 					}	
// 					break;
// 				}
// 				pItem->Serialize(ar);
// 				AddItem(key,pItem);
// 			}
// 		}
// 	}
// 	*/
// 	virtual BOOL32	ModifyItemKey( KEY keyNew )
// 	{		
// 		if ( m_pParentItem == NULL)
// 		{
// 			m_keyName = keyNew;
// 			return FALSE;
// 		}
// 		else
// 		{
// 			m_pParentItem->DeleteItem( m_keyName, FALSE );
// 			m_keyName = keyNew;
// 			m_pParentItem->AddItem( keyNew, this );
// 		}
// 		
// 		return TRUE;
// 	}
// 	
// 	virtual IItem<DATA,KEY>* Clone()
// 	{
// 		IItem<DATA,KEY>* pGroup = new CNoSortItemGroup<DATA,KEY>;
// 		pGroup->m_keyName = m_keyName;
// 		pGroup->m_pParentItem = m_pParentItem;
// 		if ( m_ptItemData != NULL )
// 		{
// 			pGroup->SetItemData( m_ptItemData );
// 		}
// 		
// 		ListType::iterator it = m_list.begin();
// 		while ( it != m_list.end() )
// 		{
// 			pGroup->AddItem( (*it)->GetItemName(), *it );
// 			++it;	
// 		}
// 		return pGroup;
// 	}
// private:
// 	ListType m_list;
//    	MapType m_map;
// }; 


// template <typename KEY,typename VALUE>
// class linked_map
// {
// public:
// 	linked_map(){}
// 	typedef typename map<KEY,VALUE> MapType;
// 	typedef typename list<map<KEY,VALUE>::iterator> ListType;
// 
// 	class iterator
// 	{
// 		friend class linked_map;
// 	public:
// 		iterator(ListType::iterator& it):m_it(it){}
// 		iterator& operator++()
// 		{
// 			++m_it;
// 			return *this;
// 		}
// 		iterator& operator--()
// 		{
// 			--m_it;
// 			return *this;
// 		}
// 		iterator operator++(int)
// 		{
// 			iterator tmp = *this;
// 			++m_it;
// 			return tmp;
// 		}
// 		iterator operator--(int)
// 		{
// 			iterator tmp = *this;
// 			--m_it;
// 			return tmp;
// 		}
// 
// 		MapType::value_type operator*()
// 		{
// 			return *(*m_it);
// 		}
// 		MapType::value_type* operator->()
// 		{
// 			return &(*(*m_it));
// 		}
// 		bool operator ==(iterator& itr)
// 		{
// 			return (m_it == itr.GetIt());
// 		}
// 		bool operator != (iterator& itr)
// 		{
// 			return !operator==(itr);
// 		}
// 
// 		ListType::iterator GetIt(){return m_it;}
// 	private:
// 		ListType::iterator m_it;
// 	};
// 
// 	iterator begin()
// 	{
// 		iterator it(m_list.begin());
// 		return  it;
// 	}
// 
// 	iterator end()
// 	{
// 		iterator it(m_list.end());
// 		return it;
// 	}
// 	int size(){return m_list.size();}
// 
// 	void push_back(const pair<KEY,VALUE>& val)
// 	{
// 		insert( end(),val );
// 	}
// 	void push_front(const pair<KEY,VALUE>& val)
// 	{
// 		insert( begin(),val );
// 	}
// 
// 	void erase(iterator& it)
// 	{
// 		m_map.erase(*it);
// 		m_list.erase(it.GetIt());
// 	}
// 
// 	iterator find(KEY key)
// 	{
// 		MapType::iterator itMap = m_map.find(key);
// 		if ( itMap != m_map.end() )
// 		{
// 			return itMap->second;
// 		}
// 		else
// 		{
// 			return m_list.end();
// 		}
// 
// 	}
// 	pair<iterator,bool> insert(iterator pos, const pair<KEY,VALUE>& val)
// 	{
// 		pair<MapType::iterator,bool>& result = m_map.insert(val);
// 		if ( result.second )
// 		{
// 			ListType::iterator it = m_list.insert(pos.GetIt(),result.first);
// 			iterator itTmp( it );
// 			return make_pair( itTmp, true );
// 		}
// 		else
// 		{
// 			iterator it( m_list.end() );
// 			return make_pair( it, false );
// 		}
// 	}
// private:
// 	ListType m_list;
// 	MapType m_map;
// };

#endif // ITEM_20100726_H
