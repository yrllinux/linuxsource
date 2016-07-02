#ifndef TEMPLATECLASS_H
#define TEMPLATECLASS_H
/*---------------------------------------------------------------------
* 类	名：Singleton
* 功    能：单例类统一接口
* 特殊说明：
* 修改记录：
* 日期			版本		修改人		修改记录
* 2011/05/09	v1.0		牟兴茂		创建
----------------------------------------------------------------------*/
#pragma warning( disable : 4786 )
#pragma warning( disable : 4503 )
#pragma warning( disable : 4800 )
#include <assert.h>
#include <string>
#include <map>
namespace AutoUIFactory
{

template<class T> class Singleton
{
protected:
	static T* ms_pSingleton;
public:
	Singleton( void )
	{
		assert( !ms_pSingleton );
		ms_pSingleton = static_cast<T*>(this);
	}
	~Singleton( void )
	{
		assert( ms_pSingleton );
		ms_pSingleton = 0;
	}
	static T& GetSingleton( void )
	{
		assert( ms_pSingleton );
		return ( *ms_pSingleton );
	}
	static T* GetSingletonPtr( void )
	{
		return ms_pSingleton;
	}
};

class IArgs
{
public:
	IArgs( std::string strType ) : m_strType( strType ){}
	virtual ~IArgs(){}
	std::string GetType() const { return m_strType; } 
	virtual void Print(){}
protected:
	std::string m_strType;
};

class IObserver
{
public:
	virtual ~IObserver(){}
	virtual void Update( IArgs* pArgs ) = 0;
};

/*---------------------------------------------------------------------
* 类	名：ICallBackFuncBase
* 功    能：回调函数接口
* 特殊说明：
* 修改记录：
* 日期			版本		修改人		修改记录
* 2010/08/20	v1.0		牟兴茂		创建
----------------------------------------------------------------------*/
class ICallBackFuncBase_Args
{
public:
	virtual ~ICallBackFuncBase_Args(){}
	virtual bool operator()( const IArgs* args ) = 0;
};

/*---------------------------------------------------------------------
* 类	名：CMemberCallBackFunc
* 功    能：为成员函数定义的回调函数
* 特殊说明：
* 修改记录：
* 日期			版本		修改人		修改记录
* 2010/08/20	v1.0		牟兴茂		创建
----------------------------------------------------------------------*/
template<class T> class CMemberCallBackFunc_Args : public ICallBackFuncBase_Args
{
public:
    //定义回调函数的格式
    typedef bool (T::*MemberFunctionType)( const IArgs* args );
	
	// 构造的时候需要为函数赋值
    CMemberCallBackFunc_Args( MemberFunctionType pFunc, T* pObject ) : m_pObject(pObject), m_pFuc( pFunc ) {}
	
    virtual bool operator()( const IArgs* args )
    {
        return (m_pObject->*m_pFuc)( args );
    }
	
private:
	T* m_pObject;
    MemberFunctionType m_pFuc;	
};

/*---------------------------------------------------------------------
* 类	名：CMemberCallBackFunc
* 功    能：为成员函数定义的回调函数
* 特殊说明：
* 修改记录：
* 日期			版本		修改人		修改记录
* 2010/08/20	v1.0		牟兴茂		创建
----------------------------------------------------------------------*/
class CCallBackFunc_Args : public ICallBackFuncBase_Args
{
public:
	virtual bool operator()( const IArgs* args)
	{
		return m_pFuc(args);
	}
	
    //定义回调函数的格式
    typedef bool (CallBackFunc)( const IArgs* args);
	
	// 构造的时候需要为函数赋值
    CCallBackFunc_Args(CallBackFunc* pFunc) : m_pFuc( pFunc ) {}
	
private:
    CallBackFunc* m_pFuc;	
};

/*---------------------------------------------------------------------
* 类	名：INoncopyable
* 功    能：用于不可拷贝类的基类，比方说一些全局的资源
* 特殊说明：如果要使用，请用private继承
* 修改记录：
* 日期			版本		修改人		修改记录
* 2011/07/19	v1.0		牟兴茂		创建
----------------------------------------------------------------------*/
class INoncopyable
{
protected:
	INoncopyable() {}
	~INoncopyable() {}
private:  // emphasize the following members are private
	   INoncopyable( const INoncopyable& );
	   const INoncopyable& operator=( const INoncopyable& );
};

} // namespace AutoUIFactory end
#endif