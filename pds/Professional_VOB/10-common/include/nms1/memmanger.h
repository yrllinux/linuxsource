// MemManger.h: interface for the CMemManger class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MEMMANGER_H__284F12CC_C2BA_49E7_A6FD_BFF084E12786__INCLUDED_)
#define AFX_MEMMANGER_H__284F12CC_C2BA_49E7_A6FD_BFF084E12786__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////
//  自定义内存管理器
//
//  [总空闲数(4 byte)]
//  [数据块(ItemSize byte)][是否占用(1 byte)][当前块索引(4 byte)][下块索引(4 byte)]
//  [数据块(ItemSize byte)][是否占用(1 byte)][当前块索引(4 byte)][下块索引(4 byte)]
//  ......
//  [数据块(ItemSize byte)][是否占用(1 byte)][当前块索引(4 byte)][下块索引(4 byte)]
//  [第1空闲块索引][第2空闲块索引]......[第N空闲块索引]
//  块索引 从1开始
/////////////////////////////////////////////////////////////////
#include <singleton.h>
class CMemoryManager : public CSingleton<CMemoryManager>
{
private:
	int               m_ItemCount;  //总块数
	int               m_ItemSize;   //每块字节数
	int               m_BlockSize;  //每项大小=m_ItemSize+5
	int               m_FreeIndexAddr;//空闲索引区首地址
	LPVOID            pData;        //全局内存区首地址
	HANDLE            m_WaitEvent;  //等待事件(空闲内存不足时等待)
	CRITICAL_SECTION  m_lock;       //数据互斥锁
	
	//初始化内存区（预留内存区）
	BOOL Init(int iItemSize,int iItemCount);
public:
	CMemoryManager(int iItemSize,int iItemCount);
	~CMemoryManager();
	
	void Lock();
	void UnLock();
	
	//申请内存引用
	LPVOID New(int Len);
	//释放内存引用
	BOOL Delete(LPVOID p);
	
	//获取数据块字节数
	int GetItemSize();
	//获取总块数
	int GetItemCount();
	//获取总空闲数
	int GetFreeCount();
	
	//获取数据区首地址
	int GetBeginAddr(){return (int)pData;};
	
};

#endif // !defined(AFX_MEMMANGER_H__284F12CC_C2BA_49E7_A6FD_BFF084E12786__INCLUDED_)
