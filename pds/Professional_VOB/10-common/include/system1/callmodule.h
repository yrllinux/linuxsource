#ifndef _CALLMODULE_H
#define _CALLMODULE_H

#include "osp.h"
#include "h323adapter.h"

namespace CallModule
{
	//=======================================
	//常量定义
	//=======================================
	//返回值
	const u32 RET_OK = 1;
	const u32 RET_ERR = 0;

	//最大能力个数
	const u32 MAX_CAP_NUM	 = 16;

	

	//=======================================
	//枚举类型
	//========＝==============================
	//打印级别
	enum LOG_LEVLE
	{
		LOG_ERROR = 0,
		LOG_IMPORTANT = 1,
		LOG_HINT = 224,
		LOG_DETAIL = 255,
	};
	
	//=======================================
	//结构定义
	//=======================================
	//能力项描述
	typedef struct tagCapDes
	{
	public:
		//能力名字(enum PayloadType)
		u32 m_capName;
		//最大速率
		u32 m_maxBitrate;
	public:
		tagCapDes()
		{
			Reset();
		}
		void Reset()
		{
			m_capName = 0;
			m_maxBitrate = 0;
		}
	}TCapDes;
	//回调消息定义
	typedef struct tagCallBackEvent
	{
	public:
		//回调消息接收app id
		u32 m_callbackAid;	
		//协议栈初始化结果		消息体TRUE/FALSE
		u16 m_eventStackInit;	
		//注册结果通知			消息体TRUE/FALSE
		u16 m_eventRegResult;	
		//对端能力通知			消息体TParamPeerCap
		u16 m_eventPeerCapTable;
		//对端RTCP/RTP地址通知  消息体TParamPeerAddr
		u16 m_eventPeerDataAddr;
		//创建新呼叫			消息体TCallHandle
		u16 m_eventNewCallCreate;
		//新呼叫到来			消息体TParamNewCall
		u16 m_eventNewCallComing;
		//呼叫连接通知			消息体TCallHandle
		u16 m_eventCallConnected;
		//呼叫断开通知			消息体TCallHandle
		u16 m_eventCallDisconnected;
		//新的信道建立			消息体TCallHandle
		u16 m_eventNewChanCreate;

	public:
		tagCallBackEvent()
		{
			Reset();
		}
		void Reset()
		{
			m_callbackAid = 0;	
			m_eventStackInit = 0;
			m_eventRegResult = 0;
			m_eventPeerCapTable = 0;
			m_eventPeerDataAddr = 0;
			m_eventNewCallCreate = 0;
			m_eventNewCallComing = 0;
			m_eventCallConnected = 0;
			m_eventCallDisconnected = 0;
			m_eventNewChanCreate = 0;
		}
	}TCallBackEvent;

	//呼叫句柄
	typedef struct tagTCallHandle
	{
	private:
		HCALL		m_hsCall;
		HCHAN		m_hsChan;
		HAPPCALL	m_haCall;
		HAPPCHAN	m_haChan;
	public:
		tagTCallHandle()
		{
			m_hsCall = 0;
			m_hsChan = 0;
			m_haCall = 0;
			m_haChan = 0;
		}
	public:
		void SetHCall(HCALL hsCall)
		{
			m_hsCall = hsCall;
		}

		HCALL GetHCall() const
		{
			return m_hsCall;
		}
		
		void SetHaCall(HAPPCALL haCall)
		{
			m_haCall = haCall;
		}

		HAPPCALL GetHaCall() const
		{
			return m_haCall;
		}
		
		void SetHChan(HCHAN hsChan)
		{
			m_hsChan = hsChan;
		}

		HCHAN GetHChan() const
		{
			return m_hsChan;
		}
		void SetHaChan(HAPPCHAN haChan)
		{
			m_haChan = haChan;
		}

		HAPPCHAN GetHaChan() const
		{
			return m_haChan;
		}
	}TCallHandle;

	typedef struct tagParamNewCall : public TCallHandle
	{
	private:
		TCALLPARAM m_tCallParam;
	public:
		void SetCallParam(const TCALLPARAM&  tCallParam)
		{
			memcpy(&m_tCallParam,&tCallParam,sizeof(TCALLPARAM));
		}
		void  GetCallParam(TCALLPARAM& tCallParam) const 
		{
			memcpy(&tCallParam,&m_tCallParam,sizeof(TCALLPARAM));
		}
	}TParamNewCall;
	
	typedef struct tagParamPeerAddr : public TCallHandle
	{
	private :
		TNETADDR m_netAddr;
	public :
		void SetNetAddr(const TNETADDR&  tAddr)
		{
			m_netAddr = tAddr;
		}
		void  GetNetAddr(TNETADDR& tAddr) const 
		{
			tAddr = m_netAddr;
		}
	}TParamPeerAddr;

	typedef struct tagParamPeerCap : public TCallHandle
	{
	private :
		TCapSet m_tCap;
	public :
		void SetCap(const TCapSet&  tCap)
		{
			m_tCap = tCap;
		}
		void  GetCap(TCapSet& tCap) const 
		{
			tCap = m_tCap;
		}
	}TParamPeerCap;
	//=======================================
	//类定义
	//=======================================
	//呼叫信息
	class  CCallInfo
	{
	private:
		HCALL m_hsCall;
		BOOL  m_bCaller;
	public:
		CCallInfo()
		{
			Reset();
		}
		BOOL IsHcallMatch(HCALL hsCall)
		{
			if(hsCall == NULL)
			{
				return FALSE;
			}
			else
			{
				return hsCall == m_hsCall;
			}
		}
		BOOL IsCaller()
		{
			return m_bCaller;
		}
		void Reset()
		{
			m_bCaller = FALSE;
			m_hsCall = NULL;
		}
		BOOL IsNull()
		{
			if (m_hsCall == NULL)
				return TRUE;
			return FALSE;
		}

		void SetHCall(HCALL hsCall)
		{
			m_hsCall = hsCall;
		}
		void SetCaller(BOOL bCaller = TRUE)
		{
			m_bCaller = bCaller;
		}
	};

	class CCallInfoArray
	{
	private:
		CCallInfo* m_pCallInfoArray;
		u32 m_arraySize;
	public:
		CCallInfoArray(){m_pCallInfoArray = NULL;}
		BOOL NewArray(u32 size)
		{
			m_pCallInfoArray = new CCallInfo[size];
			m_arraySize = size;
			return (BOOL)m_pCallInfoArray;
		}
		void  DestoryArray(){delete[] m_pCallInfoArray;}
		BOOL IsCaller(HCALL hsCall)
		{
			if (m_pCallInfoArray == NULL)
			{
				return FALSE;
			}
			for (u32 i = 0 ; i < m_arraySize; i++)
			{
				if (m_pCallInfoArray[i].IsHcallMatch(hsCall))
				{
					return m_pCallInfoArray[i].IsCaller();
				}

			}
			return FALSE;
		}

		BOOL RemoveCallInfo(HCALL hsCall)
		{
			if (m_pCallInfoArray == NULL)
			{
				return FALSE;
			}
			for (u32 i = 0 ; i < m_arraySize; i++)
			{
				if (m_pCallInfoArray[i].IsHcallMatch(hsCall))
				{
					m_pCallInfoArray[i].Reset();
					return TRUE;
				}

			}
			return FALSE;
		}

		BOOL AddCallInfo(HCALL hsCall,BOOL bCaller)
		{
			if (m_pCallInfoArray == NULL)
			{
				return FALSE;
			}
			for (u32 i = 0 ; i < m_arraySize; i++)
			{
				if (m_pCallInfoArray[i].IsNull())
				{
					m_pCallInfoArray[i].SetHCall(hsCall);
					m_pCallInfoArray[i].SetCaller(bCaller);
					return TRUE;
				}

			}
			return FALSE;
		}

		BOOL  SetCaller(HCALL hsCall)
		{
			if (m_pCallInfoArray == NULL)
			{
				return FALSE;
			}
			for (u32 i = 0 ; i < m_arraySize; i++)
			{
				if (m_pCallInfoArray[i].IsHcallMatch(hsCall))
				{
					m_pCallInfoArray[i].SetCaller();
					return TRUE;
				}

			}
			return FALSE;
		}
		
	};

	//呼叫模块
	class CCallModule
	{
	protected:
		static TCallBackEvent m_tCallbackEvent;	//回调消息结构
		static BOOL			  m_bStackRun;		//协议栈是否运行
		static BOOL			  m_isRegistered;	//是否注册上GK
		static TCapSet 		  m_tCap;			//本地能力
		static TNETADDR		  m_tRtpAddr;		//本地T120RTP地址
		static CCallInfoArray m_cCallArray;		//呼叫信息
	public:
		CCallModule();
		~CCallModule();
	public:
		//====================================
		//初始化接口
		//====================================
		//初始化呼叫模块
		u32 InitCallModule();

		//====================================
		//设置接口
		//====================================
		//设置逻辑信道地址
		u32 SetChanAddress(const TNETADDR& rtpAddr);

		//设置能力集
		u32 SetCapabilityTable(const TCapSet& tCap);

		//设置回调应用和消息
		u32 SetCallbackEvent(const TCallBackEvent& tCallbackEvent);

		//====================================
		//控制接口
		//====================================
		//向GK注销
		u32 UnRegisterToGK();

		//向GK注册
		u32 RegisterToGK(TRRQINFO& rasRegisterInfo);

		//发起呼叫
		u32 MakeCall(const TParamNewCall & tCallParam);

		//接收呼叫
		u32 AnswerCall(const TCallHandle& tCallHandle);

		//挂断呼叫
		u32 HangUp(const TCallHandle& tCallHandle);

	public:
		//=====================================
		//协议栈函数,必须在单独线程中调用协议栈函数
		//=====================================
		//初始化协议栈
		static u32 InitH323Stack(u16 CallingPort=1720, u16 LocalRasPort=1719);
		//销毁协议栈
		static u32 DestoryH323Stack();

	public:
		//=====================================
		//适配回调函数
		//=====================================
		static int KDVCALLBACK EvChanCtrlMsg(HAPPCALL haCall,
												HCALL hsCall,
												HAPPCHAN haChan,
												HCHAN hsChan,
												u16 msgType, 
												const void* pBuf,
												u16 nBufLen
		);
		
		static int KDVCALLBACK EvConfCtrlMsg(HAPPCALL haCall, 
												HCALL hsCall,
												u16 msgType,
												const void* pBuf,
												u16 nBufLen);

		static int KDVCALLBACK EvNewCall(HCALL hsCall,
											LPHAPPCALL lphaCall);

		static int KDVCALLBACK EvNewChan(HAPPCALL haCall,
											HCALL hsCall,
											HCHAN hsChan,
											LPHAPPCHAN lphaChan);

		static int KDVCALLBACK EvCallCtrlMsg(HAPPCALL haCall,
												HCALL hsCall,
												u16 msgType,
												const void* pBuf,
												u16 nBufLen);

		static int KDVCALLBACK EvRasCtrlMsg(    HAPPCALL haCall,
												HCALL hsCall,
			                                    HAPPRAS haRAS,
												HRAS hsRas,
												u16 msgType,
												const void* pBuf,
												u16 nBufLen);


		//回调消息
		static void NotifyCallBackApp(u16 msgId,void* pContent,u32 len);




	};
}
#endif 
