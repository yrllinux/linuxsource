// singleton.h: interface for the CSingleTon class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SINGLETON_H__AEF4D8BA_C3E8_44C1_874D_854F10BE573A__INCLUDED_)
#define AFX_SINGLETON_H__AEF4D8BA_C3E8_44C1_874D_854F10BE573A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/*---------------------------------------------------------------------
* 类	名：Singleton
* 功    能：提供统一单件类接口
* 特殊说明：1. 类声明           class CData : public CSingleton<CData>
			2. 变量声明(在CData的cpp文件中) 
			template<> CData* CSingleton<CData>::m_psSingleton  = NULL;
			
			3. 在程序最开始的初始化点进行初始化
			new CData();

			4. 当初始化过后，就可以在程序其他地方使用
			
			CData* pData = CSingleton<CData>::GetSingleTonPtr();

			由于没有对CData的构造函数进行限制，所以切记在3的时候，确保CData只初始化一次

* 修改记录：
* 日期			版本		修改人		修改记录
* 2010/07/16	v1.0		牟兴茂		创建
----------------------------------------------------------------------*/
template<typename T> class CSingleton
{
protected:
	static T* m_psSingleton;
	
public:
	CSingleton()
	{
		m_psSingleton = static_cast<T*>(this);
	}
	
	virtual ~CSingleton()
	{
		m_psSingleton = NULL;
	}
	
	// 获取指针和对象
	static T& GetSingleton(){ return *m_psSingleton; }
	
	static T* GetSingletonPtr(){ return m_psSingleton; }
};

#endif // !defined(AFX_SINGLETON_H__AEF4D8BA_C3E8_44C1_874D_854F10BE573A__INCLUDED_)
