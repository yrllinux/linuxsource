#ifndef SAFEADAPTER_H
#define SAFEADAPTER_H

class CLock
{
	HANDLE m_hMutex;
public:
	CLock()
	{
		m_hMutex = ::CreateMutex(NULL, FALSE, NULL);
	};
	~CLock()
	{
		::CloseHandle(m_hMutex);
	};
public:
	
	BOOL Lock(DWORD dwTimeout = INFINITE)
	{		
		return (::WaitForSingleObject(m_hMutex, dwTimeout) == WAIT_OBJECT_0);
	}
	
	BOOL UnLock()
	{
		return ::ReleaseMutex(m_hMutex);
	}
};

template<class T, class S = CLock>
class CTAapater
{
protected:
	class LockProxy
	{
	public:
		LockProxy(T* pT, S& Lock): m_pT(pT), m_rLock(Lock)
		{
			m_rLock.Lock();
		}
		~LockProxy() 
		{
			m_rLock.UnLock();
		}
		
		T* operator->()    {  return m_pT; }
		
	private:
		T* m_pT;
		S& m_rLock;
	};
	
	public:
		CTAapater()  {  m_pT = new T;  }
		~CTAapater() {  delete m_pT;  }
		LockProxy operator->()  
		{ 
			return LockProxy(m_pT, m_Lock); 
		}
		
	private:
		T* m_pT;
		S m_Lock;
};

class CWinCsLock
{
public:
    CWinCsLock()
    {
        InitializeCriticalSection(&m_cs);        
    }
    ~CWinCsLock()
    {
        DeleteCriticalSection(&m_cs);
    }
    void Lock()
    {        
        EnterCriticalSection(&m_cs);
    }
    void Unlock()
    {
        LeaveCriticalSection(&m_cs);
    }
private:
    CRITICAL_SECTION m_cs;    
};

/**
 * 局部锁.在作用域内进行lock.析构时自动unlock
 */

template <class TheLock>
class ScopedLock
{
public:
    explicit ScopedLock(TheLock& lock) : m_lock(lock)
    {
        m_lock.Lock();
    }
    explicit ScopedLock(TheLock* pLock) : m_lock(*pLock)
    {
        m_lock.Lock();
    }
    ~ScopedLock()
    {
        m_lock.Unlock();
    }
protected:
    TheLock& m_lock;

private:
	ScopedLock( const ScopedLock& );
	const ScopedLock& operator=( const ScopedLock& ){ return *this; }
};

#endif