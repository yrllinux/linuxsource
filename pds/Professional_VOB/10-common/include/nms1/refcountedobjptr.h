#ifndef __REFCOUNTEDOBJPTR_H__
#define __REFCOUNTEDOBJPTR_H__

//引用计数对象指针，可放入容器中(如vector,list等)
//标准模板库STL中的智能指针auto_ptr不可以放入容器中

class ReferenceCount
{
public:
	ReferenceCount();
	ReferenceCount(const ReferenceCount&);
	ReferenceCount& operator = (const ReferenceCount&);
	~ReferenceCount()
	{
		Decrement();
		if(IsUnique())
		{
			delete m_pRefcnt;
		}
	}
	bool IsUnique() const
	{
		return *m_pRefcnt == 1;
	}
private:
	u32* m_pRefcnt;
	void Decrement();
};

inline ReferenceCount::ReferenceCount():m_pRefcnt(new u32(1))
{

}

inline ReferenceCount::ReferenceCount(const ReferenceCount& anRC):m_pRefcnt(anRC.m_pRefcnt)
{
	++*m_pRefcnt;
}

inline void ReferenceCount::Decrement()
{
	if(IsUnique())
	{
		delete m_pRefcnt;
	}
	else
	{
		--*m_pRefcnt;
	}
}

inline ReferenceCount& ReferenceCount::operator = (const ReferenceCount& rhs)
{
	++*rhs.m_pRefcnt;
	Decrement();
	m_pRefcnt = rhs.m_pRefcnt;
	return *this;
}

template<class T>
class CountedBuiltInPtr
{
public:
	CountedBuiltInPtr():the_p(NULL)
	{

	}
	CountedBuiltInPtr(T* just_newed):the_p(just_newed)
	{
		
	}
	CountedBuiltInPtr<T>& operator = (const CountedBuiltInPtr<T>&);
	CountedBuiltInPtr<T>& operator = (T*);
	virtual ~CountedBuiltInPtr();
	bool IsUnique()const
	{
		return refCount.IsUnique();
	}
	T& operator*()const
	{
		return *the_p;
	}
	bool IsNull()const
	{
		return the_p == NULL;
	}
	friend bool operator==(const CountedBuiltInPtr<T>&lhs,const CountedBuiltInPtr<T>&rhs)
	{
		return lhs.the_p != rhs.the_p;
	}
protected:
	T* the_p;
private:
	ReferenceCount refCount;
};

template<class T>
CountedBuiltInPtr<T>&
CountedBuiltInPtr<T>::operator = (const CountedBuiltInPtr<T>& rhs)
{
	if(the_p != rhs.the_p)
	{
		if(IsUnique())
		{
			delete the_p;
		}
		the_p = rhs.the_p;
		refCount = rhs.refCount;
	}
	return *this;
}

template<class T>
CountedBuiltInPtr<T>& CountedBuiltInPtr<T>::operator = (T* just_newed)
{
	if(IsUnique())
	{
		delete the_p;
	}
	the_p = just_newed;
	refCount = ReferenceCount();
	return *this;
}

template<class T>
CountedBuiltInPtr<T>::~CountedBuiltInPtr<T>()
{
	if(refCount.IsUnique())
	{
		delete the_p;
	}
}

template<class T>
class RefCountedObjPtr:public CountedBuiltInPtr<T>
{
public:
	RefCountedObjPtr()
	{

	}
	RefCountedObjPtr(T* just_newed):CountedBuiltInPtr<T>(just_newed)
	{

	}

	RefCountedObjPtr<T>& operator = (T* rhs)
	{
		CountedBuiltInPtr<T>::operator = (rhs);
		return *this;
	}
	RefCountedObjPtr<T> operator = (const CountedBuiltInPtr<T>& rhs)
	{
		CountedBuiltInPtr<T>::operator = (rhs);
		return *this;
	}
	T* operator->()const
	{
		return the_p;
	}
};

#endif