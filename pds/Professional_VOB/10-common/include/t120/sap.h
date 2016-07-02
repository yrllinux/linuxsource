/******************************************************************************
模块名      ：Sap
文件名      ：Sap.h
相关文件    ：Sap.cpp
文件实现功能：实现Sap
作者        ：赖齐
版本        ：4.0
-------------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      走读人      修改内容
23/05/05    4.0         赖齐        赖齐        创建
******************************************************************************/
#ifndef _SAP_H_
#define _SAP_H_

#ifdef WIN32
#pragma warning (disable : 4786)
#endif
//#include "StdAfx.h"
#include <map>
#include <list>
#include <algorithm>
#include <kdvtype.h>
#include <ptlib.h>



/*====================================================================
// 打印日志 宏定义
=====================================================================*/
#ifdef ENABLE_SAP_LOG
    static PMutex G_SAP_LOG;
    #define SAP_LOG(format, para1, para2, para3) \
        { \
            PWaitAndSignal lock(G_SAP_LOG); \
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
                sprintf(anFormat, "[%2.2d:%2.2d:%2.2d:%3.3d] SAP    : %s\r\n", tSystime.wHour, tSystime.wMinute, tSystime.wSecond, tSystime.wMilliseconds, format); \
                sprintf(anBuf, anFormat, para1, para2, para3); \
                WriteFile(handleFile, anBuf, strlen(anBuf), &dwRcv, NULL); \
                CloseHandle(handleFile); \
            } \
        }
*/
#else
    #define SAP_LOG(format, para1, para2, para3)
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
类名 ： CResource
描述 ： Sap管理的资源
====================================================================*/
class CResource
{
public:
    virtual void OnAccess(void* pReqData, void* pAckData) = 0;
};

/*====================================================================
类名 ： CSap
描述 ： Sap管理器
====================================================================*/
class CSap
{
private:
    // 所有资源：<资源指针,互斥量>
    typedef std::map<CResource*, PMutex> TResourceMap;
    TResourceMap m_mapRes;
    // m_mapRes的互斥锁
    PMutex m_mutexRes;

    // 所有等待中的访问：<请求的资源,资源等待队列> 
    typedef std::list<PSemaphore*> TSemaphoreList;
    typedef std::map<CResource*, TSemaphoreList> TWaitMap;
    TWaitMap m_mapWait;
    // m_mapWait的互斥锁
    PMutex m_mutexWait;

public:
    // 构造函数
    CSap();
    // 析构函数
    virtual ~CSap(){};
    // 添加资源
    bool IncludeRes(CResource* pcIncludeRes);
    // 移除资源
    bool ExcludeRes(CResource* pcExcludeRes);
    // 请求资源：请求指定资源的访问权
    bool AccessRes(CResource* pcAccessRes, void* pReqData, void* pAckData);
    // 得到指定资源的访问权前，先调用该函数
    virtual void On_Pre_AccessRes(CResource* pcCreateRes, void* pReqData) = 0;

private:

    // 内部操作结果
    typedef enum
    {
        e_NoSuchRes,
        e_Fail,
        e_Success,
        e_Unexpect
    } TSapResult;

    // 对资源上锁
//    TSapResult LockRes(CResource* pcOperateRes);
    TSapResult LockRes(CResource* pcOperateRes, PSemaphore*& pThreadWaitSema);

    // 对资源解散
    TSapResult UnlockRes(CResource* pcOperateRes);
    // 申请等待信号量
    PSemaphore* NewThreadSema(CResource* pcOperateRes);
    // 删除等待信号量
    TSapResult DeleteThreadSema(CResource* pcOperateRes, PSemaphore* pThreadWaitSema);
    // 线程等待信号量
    TSapResult WaitThreadSema(CResource* pcOperateRes, PSemaphore* pThreadWaitSema);
    // 释放等待信号量
    TSapResult ReleaseOneThreadSema(CResource* pcOperateRes);

    // 对资源做Exclude操作或Access操作
    bool OperateRes(CResource* pcOperateRes, bool bType, void* pReqData);
};

#endif
