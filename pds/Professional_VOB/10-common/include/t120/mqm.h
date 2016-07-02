/******************************************************************************
模块名      ：Mqm
文件名      ：Mqm.h
相关文件    ：Mqm.cpp
文件实现功能：实现MQM
作者        ：王筛定
版本        ：4.0
-------------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      走读人      修改内容
23/05/05    0.1         王筛定      王筛定      创建
******************************************************************************/
#ifndef _MQM_H_
#define _MQM_H_

#ifdef WIN32
#pragma warning (disable : 4786)
#endif

#include <queue>
#include <map>
#include <list>
#include <kdvtype.h>
#include <ptlib.h>

/*====================================================================
// 打印日志 宏定义
=====================================================================*/
#ifdef ENABLE_MQM_LOG
    static PMutex G_MQM_LOG;
    #define MQM_LOG(format, para1, para2, para3) \
        { \
            PWaitAndSignal lock(G_MQM_LOG); \
            printf(format, para1, para2, para3); \
            printf("\n"); \
        }
/*
    #define MQM_LOG(format, para1, para2, para3) \
        { \
            CHAR anBuf[1024]; \
	        sprintf(anBuf, ".\\T120_LOG\\%ld_%ld.log", GetCurrentProcessId(), GetCurrentThreadId()); \
            HANDLE handleFile = CreateFile(anBuf, GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL); \
            if (handleFile != NULL) \
            { \
                SetFilePointer(handleFile, NULL, NULL, FILE_END); \
                DWORD dwRcv; \
                SYSTEMTIME tSystime; \
                GetLocalTime(&tSystime); \
                CHAR anFormat[256]; \
                sprintf(anFormat, "[%2.2d:%2.2d:%2.2d:%3.3d] MQM    : %s\r\n", tSystime.wHour, tSystime.wMinute, tSystime.wSecond, tSystime.wMilliseconds, format); \
                sprintf(anBuf, anFormat, para1, para2, para3); \
                WriteFile(handleFile, anBuf, strlen(anBuf), &dwRcv, NULL); \
                CloseHandle(handleFile); \
            } \
        }
*/
#else
    #define MQM_LOG(format, para1, para2, para3)
#endif


#ifndef NEW_WITH_CHECK
    #ifdef ENABLE_NEW_CHECK
        static PMutex G_NEW_CHECK_LOG;
        #define CHECK_NEW(pNewPoint) \
            if (pNewPoint == NULL) \
            { \
                PWaitAndSignal lock(G_NEW_CHECK_LOG); \
                printf("**** 严重错误:申请内存失败**** \n"); \
            }
        #define CHECK_DELETE(pDeletePoint) \
            if (pDeletePoint == NULL) \
            { \
                PWaitAndSignal lock(G_NEW_CHECK_LOG); \
                printf("**** 严重错误:删除内存失败**** \n"); \
            }
    #else
        #define CHECK_NEW(pNewPoint)
        #define CHECK_DELETE(pDeletePoint)
    #endif

    #define NEW_WITH_CHECK(pcNewPoint, NewTypeAndParas) \
        pcNewPoint = new NewTypeAndParas; \
        CHECK_NEW(pcNewPoint);
    #define DECLEAR_AND_NEW(CType, pcNewPoint, NewTypeAndParas) \
        CType* pcNewPoint = new NewTypeAndParas; \
        CHECK_NEW(pcNewPoint);
    #define DELETE_WITH_CHECK(pcDeletePoint) \
        CHECK_DELETE(pcDeletePoint); \
        delete pcDeletePoint;
    #define DELETE_ARRAY_WITH_CHECK(paDeleteArray) \
        CHECK_DELETE(paDeleteArray); \
        delete[] paDeleteArray;
#endif



    
/*====================================================================
// 宏定义
=====================================================================*/
#define MAX_QUEUE_SIZE	0xffffff // 队列最大长度定义
#define INVALID_QUEUEID	0		 //无效队列ID

class CMsg;
class CQueue;
class CMqmThread;
class CMqmSyncReceiver;

/*====================================================================
// 类型别名声明
=====================================================================*/
typedef u32 QueueID; //队列编号
typedef u32 MsgID; //消息编号

typedef std::map<QueueID,CQueue*> TQueueMap;
typedef std::pair<QueueID,CQueue*> TQueuePair;
typedef std::map<MsgID, CMsg*> TMsgMap;
typedef std::pair<MsgID,CMsg*> TMsgPair;

typedef struct tagMessageContext
{
    tagMessageContext();
    tagMessageContext(QueueID nQueueID, u8 nContextType=0, void* pcContextRes=NULL);

    // 等于操作
    bool operator == (const tagMessageContext tRightValue);
    // 不等于操作
    bool operator != (const tagMessageContext tRightValue);

    QueueID m_nQueueID; // 消息的发送队列
    u8 m_nContextType; // 消息的环境类型（发送者类型或者接收者类型）
    void* m_pcContextRes; // 消息的发送者或者接收者
}TMessageContext;


/*====================================================================
类名 ： CMsg
描述 ： 队列中的每项数据定义，实际的消息必须从此类派生
====================================================================*/
class CMsg
{
private:
    // 生成内部序列号
    MsgID GenerateMsgId();

public:
	// 构造
    CMsg(u8 nCatalog, u8 nType);
	// 析构
    virtual ~CMsg(){};
    // 生成回复消息
    void BuildAck(const CMsg* const pcMsg);
    // 生成转发消息
    void BuildPass();
    // 得到MsgID
    MsgID GetMsgID();
    u8 m_nMsgCatalog; // 消息类别(大)
    u8 m_nMsgType; // 消息类别(小)
    TMessageContext m_tMsgFromContext;    //消息的发送者
    TMessageContext m_tMsgToContext;  //消息的接收者
    
private:    
    MsgID m_nID; // 消息编号(唯一，MQM内部使用)

private:
};

/*====================================================================
类名 ： CQueue
描述 ： 一个队列
====================================================================*/
class CQueue
{
public:
	//构造和析构
	CQueue(u32 uMaxVal);
	~CQueue();

    QueueID GetQueueID();
	bool InsetrtMsg(CMsg* cMsg);
	CMsg* GetMsg();
    // 队列定义
    typedef std::queue<CMsg*> TMsgQueue;
    TMsgQueue m_queMsg;

    PSemaphore m_semQueueSize; // 目前队列中的消息数目
    PMutex m_mutexData; // 队列互斥锁保证本队列互斥操作
    CMqmSyncReceiver* m_pcSyncReceiver; // 该队列的同步消息处理者

private:
	QueueID m_nID; // 队列编号(唯一，MQM内部使用)
};

/*====================================================================
类名 ： CMqm
描述 ： 实现MQM的功能
====================================================================*/
class CMqm
{
public:
	CMqm();//构造函数
	~CMqm();//析构函数
    
	QueueID CreateQueue(u32 uMaxVal); // 创建队列
    bool SendMsg(const CMsg& cSendMsg, CMsg& cRecvMsg); // 发送消息
    bool SendMsg(CMsg& cSendMsg); // 发送消息（发送和接收用相同的缓冲区）
    bool PostMsg(CMsg* cPostMsg); // 传递消息

//private:
    // 所有队列定义：<消息队列指针>
    TQueueMap m_mapQueue;
	
    // 所有消息内存：<消息号,消息体指针>
    TMsgMap m_mapMsg;

	PMutex m_mutexMapQueue; //m_mapMsg的同步锁
	PMutex m_mutexMapMsg; //m_mapQueue的同步锁
};

// 消息类别(大)定义
typedef enum
{
    e_General,
    e_Tc,
    e_Mcs,
    e_Gcc,
    e_Ape,
    e_Adapter
} enumMsgCatalog;

// 通用消息类型(小)定义
typedef enum
{
    e_Terminate,
    e_DebugPrint
} enumGeneralMsgType;


/*====================================================================
类名 ： CGeneralTerminatePrimitive
描述 ： 结束线程运行命令
====================================================================*/
class CGeneralTerminatePrimitive : public CMsg
{
public:
    CGeneralTerminatePrimitive():CMsg(e_General, e_Terminate)
    {
    }
};



/*====================================================================
类名 ： CGeneralDebugPrintPrimitive
描述 ： 打印调试信息
====================================================================*/
class CGeneralDebugPrintPrimitive : public CMsg
{
public:
    CGeneralDebugPrintPrimitive():CMsg(e_General, e_DebugPrint)
    {
    }
};

#endif //_MQM_H_
