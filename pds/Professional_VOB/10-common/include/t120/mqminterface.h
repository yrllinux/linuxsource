/******************************************************************************
模块名      ：MqmInterface
文件名      ：MqmInterface.h
相关文件    ：MqmInterface.cpp, Mqm.h
文件实现功能：实现MQM
作者        ：王筛定
版本        ：4.0
-------------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      走读人      修改内容
28/05/05    0.1         王筛定      王筛定      创建
******************************************************************************/
#ifndef _MQMINTERFACE_H_
#define _MQMINTERFACE_H_

#include "mqm.h"
//#include "stdafx.h"

class CMqmInterface
{
public:
    // 构造和析构
    CMqmInterface();
    ~CMqmInterface();

    //接口
    bool InitInstance(); //初始化
    void ExitInstance(); //退出
    QueueID CreateQueue(u32 uMaxVal = MAX_QUEUE_SIZE); // 创建队列
    bool SetSyncReceiver(CMqmSyncReceiver* pcMqmSyncReceiver, QueueID nQueueID);
    bool SendMsg(const CMsg& cSendMsg, CMsg& cRecvMsg); // 发送消息
    bool SendMsg(CMsg& cSendMsg); // 发送消息（发送和接收用相同的缓冲区）
    bool PostMsg(CMsg* pcPostMsg); // 传递消息
    bool RecvMsg(CMqmThread* pcMqmThread); // 接收消息

private:
    CMqm* m_pcMqm;  //消息队列管理器MQM的指针
};

/*====================================================================
类名 ： CMqmThread
描述 ： MQM队列的循环读取者
====================================================================*/
class CMqmThread : public PThread
{
    PCLASSINFO(CMqmThread, PThread);
public:
    CMqmThread();

    bool BindQueue(CMqmInterface* pcMqmInterface, QueueID tQueueID); // 将线程绑定到队列
    bool SendMsg(CMsg& cSendMsg, CMsg& cRecvMsg); // 发送消息
    bool SendMsg(CMsg& cSendMsg); // 发送消息（发送和接收用相同的缓冲区）
    bool PostMsg(CMsg* cPostMsg);// 传递消息
	QueueID GetQueueID();
    void ExitThread();
    // 纯虚函数：消息处理
    virtual void OnRecvMsg(CMsg& cPostMsg) = 0;

private:
    void Main();    //线程主函数

private:
    CMqmInterface* m_pcMqmInterface;    //Mqm接口实例
    QueueID m_nQueueID; //绑定的队列ID
    bool m_bExit;
	
};

/*====================================================================
类名 ： CMqmSyncReceiver
描述 ： 队列中的同步消息处理者
====================================================================*/
class CMqmSyncReceiver
{
public:
    CMqmSyncReceiver(); //构造函数
    bool BindQueueSync(CMqmInterface* pcMqmInterface, QueueID nQueueID); // 绑定队列
    virtual bool OnRecvSyncMsg(const CMsg& cSendMsg, CMsg& cRecvMsg) = 0; // 同步消息处理函数
};

#endif //_MQMINTERFACE_H_
