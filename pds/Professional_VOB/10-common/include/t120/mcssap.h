/******************************************************************************
模块名      ：McsSap
文件名      ：McsSap.h
相关文件    ：McsSap.cpp
文件实现功能：实现McsSap
作者        ：赖齐
版本        ：4.0
-------------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      走读人      修改内容
24/05/05    4.0         赖齐        赖齐        创建
******************************************************************************/
#ifndef _MCS_SAP_H_
#define _MCS_SAP_H_

#include <sap.h>
#include "mqminterface.h"
#include <mcsprimitives.h>


/*====================================================================
// 打印日志 宏定义
=====================================================================*/
#ifdef ENABLE_MCS_LOG
    static PMutex G_MCS_LOG;
    #define MCS_LOG(format, para1, para2, para3) \
        { \
            PWaitAndSignal lock(G_MCS_LOG); \
            printf("MCS__LOG:  "); \
            printf(format, para1, para2, para3); \
            printf("\n"); \
        }
/*
        { \
            char anBuf[1024]; \
	        sprintf(anBuf, ".\\T120_LOG\\%ld_%ld.log", GetCurrentProcessId(), GetCurrentThreadId()); \
            HANDLE handleFile = CreateFile(anBuf, GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL); \
            if (handleFile != NULL) \
            { \
                SetFilePointer(handleFile, NULL, NULL, FILE_END); \
                DWORD dwRcv; \
                SYSTEMTIME tSystime; \
                GetLocalTime(&tSystime); \
                char anFormat[256]; \
                sprintf(anFormat, "[%2.2d:%2.2d:%2.2d:%3.3d] MCS    : %s\r\n", tSystime.wHour, tSystime.wMinute, tSystime.wSecond, tSystime.wMilliseconds, format); \
                sprintf(anBuf, anFormat, para1, para2, para3); \
                WriteFile(handleFile, anBuf, strlen(anBuf), &dwRcv, NULL); \
                CloseHandle(handleFile); \
            } \
        }
*/
#else
    #define MCS_LOG(format, para1, para2, para3)
#endif

// Mcs内部线程数量定义
#ifdef MCS_THREAD
    #define MCS_THREAD_NUM MCS_THREAD
#else
    #define MCS_THREAD_NUM 3
#endif

//class CMcsThread;
class CMcsProvider;
class CMcsDomain;
class CTcAgent;
class CConnectionManager;
/*====================================================================
类名 ： CMcsSap
描述 ： McsSap管理器
====================================================================*/
class CMcsSap : public CSap, public CMqmSyncReceiver
{
private:
    // CMqm对象
    CMqmInterface m_cMqmInterface;
    // McsProvder队列资源
    TMessageContext m_tMcsProvider;
    // 队列读取允许信号量
    PMutex m_mutexQueue;
    // CMcsProvider对象
    CMcsProvider* m_pcMcsProvider;
    // CMcsDomain指针列表
    typedef std::list<CMcsDomain*> TDomainList;
    TDomainList m_listMcsDomain;
    // 允许读取的互斥量
    PMutex m_mutexReadQueue;
    //ConnectionManger对象
    CConnectionManager *m_pcConnectionManager;
public:
    // 构造函数
    CMcsSap();
    // 构造函数
    ~CMcsSap();
    // 请求资源：请求指定资源的访问权
    void On_Pre_AccessRes(CResource* pcAccessRes, void* pReqData);
    // 同步消息处理函数
    virtual bool OnRecvSyncMsg(const CMsg& cSendMsg, CMsg& cRecvMsg);
    // 得到MQM Interface
    CMqmInterface* GetMqmInterface();
    // 得到队列指针 
    const TMessageContext& GetMcsQueue();
    CMcsProvider* GetMcsProvider();
    // 启动ConnectionManager，即启动网络功能
    bool StartConnectionManager(const PString& strAddress);
    //得到CMcsConnectionManager指针
    CConnectionManager* GetConnectionManager();
};


// Mcs接收者或发送者的类型
typedef enum
{
    // Provider
    e_McsProvider,
    // Conference
    e_McsDomain
} TMcsReceiverType;


#endif
