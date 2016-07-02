// nmsinstance.h: interface for the CNmsInstance class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NMSINSTANCE_H__20545268_BF94_4A22_869A_6C419B8FFAC3__INCLUDED_)
#define AFX_NMSINSTANCE_H__20545268_BF94_4A22_869A_6C419B8FFAC3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning(disable:4244)  // conversion from 'unsigned long' to 'unsigned char', possible loss of data
#pragma warning(disable:4018)  // signed/unsigned mismatch
#pragma warning(disable:4786)  // identifier over 255

#include <map>
#include <vector>
using namespace std;
#include "kdvtype.h"
#include "kdvdef.h"
#include "osp.h"
#include "kdvsys.h"


#pragma comment(lib,"osplib.lib")

/*---------------------------------------------------------------------
* 类	名：CObserver
* 功    能：观察者抽象接口，如果需要处理Instance产生的消息，需要进行注册
* 特殊说明：
* 修改记录：
* 日期			版本		修改人		修改记录
* 2010/08/11	v1.0		牟兴茂		创建
----------------------------------------------------------------------*/
class CInstanceAssist;
class CNmsInstance;
class CObserver
{
public:
	virtual ~CObserver(){}
	virtual void Report( CNmsInstance* pInstance, CInstanceAssist* pInstanceAssist, CMessage *const pMsg, void* pUserData = NULL ) = 0;
};

class CWndObserver : public CObserver
{
public:
	CWndObserver( HWND hWnd ) : m_hWnd( hWnd ){}
	virtual void Report( CNmsInstance* pInstance, CInstanceAssist* pInstanceAssist, CMessage *const pMsg, void* pUserData = NULL ){}
protected:
	HWND m_hWnd;
};

/*---------------------------------------------------------------------
* 类	名：ICallBackFuncBase
* 功    能：回调函数接口
* 特殊说明：
* 修改记录：
* 日期			版本		修改人		修改记录
* 2010/08/20	v1.0		牟兴茂		创建
----------------------------------------------------------------------*/
class ICallBackFuncBase
{
public:
	virtual ~ICallBackFuncBase(){}
	virtual BOOL32 operator()( CNmsInstance* pInstance, CInstanceAssist*, CMessage *const pMsg, void* pUserData = NULL ) = 0;
};

/*---------------------------------------------------------------------
* 类	名：CStaticCallBackFunc
* 功    能：静态函数的一种回调函数
* 特殊说明：
* 修改记录：
* 日期			版本		修改人		修改记录
* 2010/08/20	v1.0		牟兴茂		创建
----------------------------------------------------------------------*/
class CStaticCallBackFunc : public ICallBackFuncBase
{
public:
	virtual BOOL32 operator()( CNmsInstance* pInstance, CInstanceAssist* pInstanceAssist, CMessage *const pMsg, void* pUserData = NULL )
	{
		return m_pFuc( pInstance, pInstanceAssist, pMsg, pUserData );
	}
	
    //定义回调函数的格式
    typedef BOOL32 (CallBackFunc)( CNmsInstance* pInstance, CInstanceAssist*, CMessage *const, void* );
	
	// 构造的时候需要为函数赋值
    CStaticCallBackFunc(CallBackFunc* pFunc) :	m_pFuc( pFunc ) {}
	
private:
    CallBackFunc* m_pFuc;	
};

/*---------------------------------------------------------------------
* 类	名：CMemberCallBackFunc
* 功    能：为成员函数定义的回调函数
* 特殊说明：
* 修改记录：
* 日期			版本		修改人		修改记录
* 2010/08/20	v1.0		牟兴茂		创建
----------------------------------------------------------------------*/
template<class T> class CMemberCallBackFunc : public ICallBackFuncBase
{
public:
    //定义回调函数的格式
    typedef BOOL32 (T::*MemberFunctionType)( CNmsInstance* pInstance, CInstanceAssist*, CMessage *const, void* );
	
	// 构造的时候需要为函数赋值
    CMemberCallBackFunc( MemberFunctionType pFunc, T* pObject ) : m_pObject(pObject), m_pFuc( pFunc ) {}
	
    virtual BOOL32 operator()( CNmsInstance* pInstance, CInstanceAssist* pInstanceAssist, CMessage *const pMsg, void* pUserData = NULL )
    {
        return (m_pObject->*m_pFuc)( pInstance, pInstanceAssist, pMsg, pUserData );
    }
	
private:
	T* m_pObject;
    MemberFunctionType m_pFuc;	
};

/*---------------------------------------------------------------------
* 类	名：CInstanceAssist
* 功    能：真正的处理单元/负责逻辑的处理以及通知UI
* 特殊说明：
* 修改记录：
* 日期			版本		修改人		修改记录
* 2010/08/19	v1.0		牟兴茂		创建
----------------------------------------------------------------------*/
class CInstanceAssist
{
public:
	// 消息处理
	virtual void TransData( CNmsInstance* pInstance, CMessage *const pMsg ) = 0;
	// 拷贝，为NmsInstace创建处理单元提供接口
	virtual CInstanceAssist* Clone() = 0;

	// 发送消息给对端
	virtual BOOL32 PostMsg( u16 wEvent, void* pBody = NULL, u32 dwBodyLen = 0 );

	// 对msg进行一些常规检测
	BOOL32 CheckMsg( CMessage *const pMsg );

	virtual void SetInstanceParam(u32 dwDstAppId, u32 dwDstNode, u32 dwSrcAppId, u32 dwSrcNode );

	virtual ~CInstanceAssist(){}
	
public:
	BOOL32 m_bConnected;
	u32 m_dwDstAppId;
	u32 m_dwDstNode;
	u32 m_dwSrcAppId;
	u32 m_dwSrcNode;
	u32 m_dwTimerState;
};

/*---------------------------------------------------------------------
* 类	名：CNmsInstance
* 功    能：监控实例、接受消息
* 特殊说明：
* 修改记录：
* 日期			版本		修改人		修改记录
* 2010/08/11	v1.0		牟兴茂		创建
----------------------------------------------------------------------*/
class CNmsInstance : public CInstance
{
public:
	CNmsInstance(){ m_pAssist = NULL; }
	//守护实例入口函数
	virtual void DaemonInstanceEntry( CMessage *const, CApp* );
	
	//实例入口函数
	virtual void InstanceEntry( CMessage *const pMsg );

	virtual BOOL32 CanConnect( CMessage *const pMsg );

	virtual BOOL32 StartTimer( u32 nTimer, long nMilliSeconds, u32 nPara = 0 );
	virtual BOOL32 StopTimer( u32 nTimer );
public:
	// 设置Instance的类型
	static BOOL32 SetInstanceAssistType( CInstanceAssist* pType );
	// 用于创建Assist对象
	static CInstanceAssist* m_pBaseIntanceAssist;	
	// 通过m_pBaseIntanceAssist创建Assist
	BOOL32 CreateInstanceAssist();

	CInstanceAssist* m_pAssist;
};
#endif // !defined(AFX_NMSINSTANCE_H__20545268_BF94_4A22_869A_6C419B8FFAC3__INCLUDED_)
