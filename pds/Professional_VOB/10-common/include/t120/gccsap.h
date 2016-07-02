/******************************************************************************
模块名      ：GccSap
文件名      ：GccSap.h
相关文件    ：GccSap.cpp
文件实现功能：实现GccSap
作者        ：赖齐
版本        ：4.0
-------------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      走读人      修改内容
24/05/05    4.0         赖齐        赖齐        创建
******************************************************************************/
#ifndef _GCC_SAP_H_
#define _GCC_SAP_H_

#include <mcssap.h>
#include <gccprimitives.h>


/*====================================================================
// 打印日志 宏定义
=====================================================================*/
#ifdef ENABLE_GCC_LOG
    static PMutex G_GCC_LOG;
    #define GCC_LOG(format, para1, para2, para3) \
        { \
            PWaitAndSignal lock(G_GCC_LOG); \
            printf("GCC__LOG:  "); \
            printf(format, para1, para2, para3); \
            printf("\n"); \
        }
/*
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
                sprintf(anFormat, "[%2.2d:%2.2d:%2.2d:%3.3d] GCC    : %s\r\n", tSystime.wHour, tSystime.wMinute, tSystime.wSecond, tSystime.wMilliseconds, format); \
                sprintf(anBuf, anFormat, para1, para2, para3); \
                WriteFile(handleFile, anBuf, strlen(anBuf), &dwRcv, NULL); \
                CloseHandle(handleFile); \
            } \
        }
*/
#else
    #define GCC_LOG(format, para1, para2, para3)
#endif


// Gcc内部线程数量定义
#ifdef GCC_THREAD
    #define GCC_THREAD_NUM GCC_THREAD
#else
    #define GCC_THREAD_NUM 3
#endif


class CGccThread;
class CGccProvider;
class CGccConference;

/*====================================================================
类名 ： CGccSap
描述 ： GccSap管理器
====================================================================*/
class CGccSap : public CSap, public CMqmSyncReceiver
{
private:
    // CMcsSap对象
    CMcsSap m_cMcsSap;
    // CMqm对象指针
    CMqmInterface* m_pcMqmInterface;
    // McsProvider队列资源
    TMessageContext m_tMcsProvider;
    // GccProvider队列资源
    TMessageContext m_tGccProvider;
    // CGccThread线程数组
    CGccThread* m_apGccThread[GCC_THREAD_NUM];
    // 队列读取允许信号量
    PMutex m_mutexQueue;
    // CGccProvider对象
    CGccProvider* m_pcGccProvider;
    // CGccDomain指针列表
    typedef std::list<CMcsDomain*> TConferenceList;
    TConferenceList m_listGccConference;

public:
    // 构造函数
    CGccSap(NodeType enumNodeType, PString strNodeName, PString strNodeEmail, PString strNodePhone, PString strNodeLocation);
    // 析构函数
    ~CGccSap();
    // 请求资源：请求指定资源的访问权
    void On_Pre_AccessRes(CResource* pcAccessRes, void* pReqData){};
    // 同步消息处理函数
    virtual bool OnRecvSyncMsg(const CMsg& cSendMsg, CMsg& cRecvMsg);
    // 得到MQM
    CMqmInterface* GetMqmInterface();
    // 得到GccProvider队列资源
    const TMessageContext& GetGccQueue();
    // 得到McsProvider队列资源
    const TMessageContext GetMcsQueue();

};

// Gcc接收者或发送者的类型
typedef enum
{
    // Provider
    e_GccProvider,
    // Conference
    e_GccConference
} TGccReceiverType;

#endif
