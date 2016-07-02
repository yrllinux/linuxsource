/********************************************************************
	filename: 	tree.h
	created:	2012/02/02 17:21
	modified:	2012/02/15 16:17
	author:		邹俊龙
	version:	0.2
	
	purpose:	基于节点的树形结构
	  
	todo:		提供更多种类的迭代器
*********************************************************************/

#pragma once
#include "assert.h"
#include <iterator>
namespace nms{

template< typename DATA >
class tree
{
public:

	// 迭代器基类
	class iterator_base_impl
	{
	public: 
		// 双向迭代器
		typedef std::bidirectional_iterator_tag iterator_category;
		typedef tree<DATA>						value_type;
		typedef size_t							size_type;
		typedef ptrdiff_t						difference_type;
		typedef difference_type					distance_type;
		typedef value_type&						reference;
		typedef value_type*						pointer;

	public:
		iterator_base_impl(void)
		{
			m_root = 0;
			m_node = 0;
		}
		iterator_base_impl(const iterator_base_impl& rhs)
		{
			m_root = rhs.m_root;
			m_node = rhs.m_node;
		}

		iterator_base_impl(reference root,reference node)
		{
			m_root = &root;
			m_node = &node;
		}

		iterator_base_impl(pointer root,pointer node)
		{
			m_root = root;
			m_node = node;
		}

		virtual ~iterator_base_impl(){};

		virtual reference operator*() const{ return *m_node; }
		virtual pointer operator->() const{ return m_node; }
		virtual bool operator==(const iterator_base_impl& rhs) const
		{
			return ( m_root == rhs.m_root && m_node == rhs.m_node );
		}
		virtual bool operator!=(const iterator_base_impl& rhs) const
		{
			return !(operator == (rhs));
		}

		virtual iterator_base_impl& operator=(const iterator_base_impl& rhs)
		{
			m_root = rhs.m_root;
			m_node = rhs.m_node;
			return *this;
		}

		virtual iterator_base_impl& operator++()
		{
			m_node = m_node->m_next;
			return *this;
		}
		
		virtual iterator_base_impl& operator--()
		{
			m_node = m_node->m_prev;
			return *this;
		}
		
		virtual iterator_base_impl operator++(int)
		{
			iterator_base_impl ret(*this);
			++(*this);
			return ret;
		}
		
		virtual iterator_base_impl operator--(int)
		{
			iterator_base_impl ret(*this);
			--(*this);
			return ret;
		}

		virtual tree<DATA>* parent()
		{
			return m_node->m_parent;
		}

		virtual tree<DATA>* root()
		{
			return m_root;
		}
		
	protected:
		pointer m_root;
		pointer m_node;

		friend class tree<DATA>;
	};

	// 自由迭代器
	// 提供横向(++/--)及纵向移动(up/down)
	class free_iterator : public iterator_base_impl
	{
	public:
		// 构造函数
		free_iterator(){}
		free_iterator(const iterator_base_impl& rhs):iterator_base_impl(rhs){}

		virtual iterator_base_impl& operator++()
		{
			m_node = m_node->m_next;
			return *this;
		}

		virtual iterator_base_impl& operator--()
		{
			m_node = m_node->m_prev;
			return *this;
		}

		virtual iterator_base_impl operator++(int)
		{
			free_iterator ret(*this);
			++(*this);
			return ret;
		}
		
		virtual iterator_base_impl operator--(int)
		{
			free_iterator ret(*this);
			--(*this);
			return ret;
		}

		virtual iterator_base_impl& up()
		{
			assert(m_node->m_parent != 0);
			if ( m_node->m_parent != 0 && m_node->m_parent != m_root )
			{
				m_node = m_node->m_parent;
			}
			return *this;
		}

		virtual iterator_base_impl& down()
		{
			//这里的返回值待定
			if ( m_node->m_end != 0 )
			{
				m_node = m_node->m_end->m_next;
			}
			return *this;
		}

		free_iterator begin()
		{
			return m_root->begin();
		}

		free_iterator end()
		{
			return m_root->end();
		}

	private:

		free_iterator( pointer pNode)
		{
			m_root = pNode->m_parent;
			m_node = pNode;
		}

		friend class tree<DATA>;
	};

	// 前序遍历迭代器
	// 提供前序遍历操作，不提供up、down操作
	class pre_order_iterator : public iterator_base_impl
	{
	public:
		pre_order_iterator(){};
		pre_order_iterator(const iterator_base_impl& rhs):iterator_base_impl(rhs){};
		
		// 自增操作
		iterator_base_impl& operator++()
		{
			// 没爸爸,正常情况下不会出现……
			
			if (m_node->m_parent != 0)
			{
				// 如果已是最后一个节点
				if ( m_node == m_root->m_end )
				{
					// 回到第一个子节点
					m_node = m_root->m_end->m_next;// m_root.begin();
				}
				//如果存在子节点
				else if ( m_node->m_count > 0 )
				{
					m_node = m_node->m_end->m_next;// m_node.begin();
				}
				// 如果是最后一个子节点,且父节点有兄弟节点
				else if ( m_node->m_next == m_node->m_parent->m_end )
				{
					if (m_node->m_parent->m_parent != 0
					 && m_node->m_parent->m_next != m_node->m_parent->m_parent->m_end )
					{
						//跳到叔叔节点
						m_node = m_node->m_parent->m_next;
					}
					else
					{
						m_node = m_root->m_end;
					}
				}
				else
				{
					m_node = m_node->m_next;
				}
			}
			return *this;
		}
		
		// 自减操作
		iterator_base_impl& operator--()
		{
			// 没爸爸，正常情况下不会出现……
			if ( m_node->m_parent != 0 )
			{
				// 如果第一个节点
				if ( m_node == m_root->m_end->m_next /* m_root.begin() */ )
				{
					// 回到end节点
					m_node = m_root->m_end;
				}
				else if( m_node == m_root->m_end )
				{
					m_node = &(m_root->last_leaf());
				}
				// 如果是第一个子节点
				else if ( m_node->m_prev == m_node->m_parent->m_end )
				{
					//跳到父节点
					m_node = m_node->m_parent;
				}
				else
				{
					m_node = &(m_node->m_prev->last_leaf());
				}
			}
			return *this;
		}
		
		iterator_base_impl operator++(int)
		{
			pre_order_iterator ret(*this);
			++(*this);
			return ret;
		}
		
		iterator_base_impl operator--(int)
		{
			pre_order_iterator ret(*this);
			--(*this);
			return ret;
		}

		pre_order_iterator begin()
		{
			return m_root->begin() ;
		}

		pre_order_iterator end()
		{
			return m_root->end();
		}
	
	private:
		pre_order_iterator( pointer root, pointer node )
			:iterator_base_impl( root, node ){};
		pre_order_iterator( reference root, reference node )
			:iterator_base_impl( root, node ){};
		friend class tree<DATA>;
	};

	// 默认构造函数
	tree()
	{
		init();
	}

	// 拷贝构造函数
	tree( const tree<DATA>& rhs )
	{
		make_end();
		operator=(rhs);
	}
	
	// 提供数据进行构造
	tree( const DATA& data )
	{
		// 初始化其他数据
		init();
		m_data	= data;
	}

	// 析构函数
	~tree()
	{
		// end节点的m_end为空
		if ( m_end != 0 )
		{
			remove_all();
			delete m_end;
		}
	}

	bool operator== (tree<DATA>& rhs)
	{
		if ( m_count == rhs.m_count && m_data == rhs.m_data )
		{
			free_iterator ita = free_it();
 			free_iterator itb = rhs.free_it();
 			for (size_t i = 0; i < m_count; i++)
 			{
 				if (*ita != *itb)
 				{
 					return false;
 				}
 			}
			return true;
		}
		return false;
	}

	bool operator!= (tree<DATA>& rhs)
	{
		return !operator==(rhs);
	}

	tree<DATA>& operator= (const tree<DATA>& rhs)
	{
		m_parent = /*rhs.m_parent*/ 0;
		m_prev = /*rhs.m_prev*/ 0;
		m_next = /*rhs.m_next*/ 0;
		m_count = 0;
		m_data = rhs.m_data;
		copy_child(rhs);

		return *this;
	}
	
	// 递归深拷贝
	void copy_child( const tree<DATA>& rhs )
	{
		tree<DATA>* pNode = rhs.m_end->m_next;
		while ( pNode != rhs.m_end )
		{
			push_back(new tree<DATA>(*pNode));
			pNode = pNode->m_next;
		}
	}

	// 删除所有子节点
	void clear_child()
	{
		if ( m_end != 0 )
		{
			remove_all();
			m_end->m_next = m_end;
			m_end->m_prev = m_end;
		}
	}
	
	void set_data( const DATA& data)
	{
		m_data = data;
	}

	// 返回父节点指针
	tree<DATA>* parent()
	{
		return m_parent;
	}

	iterator_base_impl& erase(iterator_base_impl& it)
	{
		if ( it.m_root == this )
		{
			// it是end节点
			if (it.m_node != it->m_next)
			{
				it->m_prev->m_next = it->m_next;
				it->m_next->m_prev = it->m_prev;
				--(it->m_parent->m_count);
				delete it.m_node;
			}
		}
		return it;
	}

	pre_order_iterator pre_order_it()
	{
		return begin();
	}

	free_iterator free_it()
	{
		return begin();
	}
	
	DATA& data()
	{
		return m_data;
	}

	// 本层第一个
	tree<DATA>& front()
	{
		return *(m_end->m_next);
	}

	tree<DATA>& back()
	{
		return *(m_end->m_prev);
	}

	size_t count(){ return m_count; }

	tree<DATA>& last_leaf()
	{
		tree* pNode = this;
		while ( pNode->m_count != 0 )
		{
			pNode = pNode->m_end->m_prev;
		}
		return *pNode;
	}

	// 子树连接
	iterator_base_impl push_back( tree<DATA>* child )
	{
		return insert( child,end() );
	}
	
	iterator_base_impl push_back( const DATA& data )
	{
		return insert( data, end() );
	}

	iterator_base_impl insert( const DATA& data, const iterator_base_impl& it )
	{
		tree<DATA>* pNode = new tree<DATA>(data);
		return insert(pNode,it);
	}
	// 子树连接
	iterator_base_impl insert(tree* child, const iterator_base_impl& it)
	{
		// 判断此迭代器指向的节点的父节点是否为本节点
		if (it.m_root != this)
		{
			return end();
		}
		
		// 调整节点中内容
		child->m_parent	= it.m_node->m_parent;
		child->m_next	= it.m_node;
		child->m_prev	= it.m_node->m_prev;
		
		it.m_node->m_prev->m_next	= child;
		it.m_node->m_prev			= child;

		++(child->m_parent->m_count);

		iterator_base_impl rtn( this, child );
		return rtn;
	}
protected:
	// 移除全部子节点
	void remove_all()
	{
		tree<DATA>* pNode = m_end->m_next;
		while( pNode != m_end )
		{
			tree<DATA>* pTemp = pNode->m_next;
			delete pNode;
			pNode = pTemp;
		}
	}

	// 构造end节点
	void make_end()
	{
		m_end = new tree<DATA>(this,0);
	}

	void init()
	{
		make_end();
		m_parent	= 0;
		m_prev		= 0;
		m_next		= 0;
		m_count		= 0;		
	}
	//用于构造end节点
	tree( tree* parent, tree* end )
	{
		m_parent	= parent;
		m_prev		= this;
		m_next		= this;
		m_end		= end;
		m_count		= 0;
	}

public:
	// 此迭代器并不完全，且不同迭代器之间，begin()和end()各不相同，仅供内部使用
	iterator_base_impl end()
	{
		return iterator_base_impl(this,m_end);
	}
	
	iterator_base_impl begin()
	{
		return iterator_base_impl(this,m_end->m_next);
	}

protected:
	tree<DATA>*	m_parent;	//父节点
	tree<DATA>*	m_prev;		//前一个节点
	tree<DATA>*	m_next;		//后一个节点
	tree<DATA>*	m_end;		//子节点结束
	size_t	m_count;	//子节点个数
	DATA	m_data;		//数据

	// 迭代器友元类
	friend class iterator_base_impl;
	friend class free_iterator;
	friend class pre_order_iterator;
};

}
