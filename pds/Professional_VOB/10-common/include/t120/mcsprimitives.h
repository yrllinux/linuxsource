/******************************************************************************
模块名      ：McsPrimitives
文件名      ：McsPrimitives.h
相关文件    ：McsPrimitives.cpp
文件实现功能：定义所有MCS原语
作者        ：赖齐
版本        ：4.0
-------------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      走读人      修改内容
24/05/05    4.0         赖齐        赖齐        创建
******************************************************************************/
#ifndef _MCS_PRIMITIVES_H_
#define _MCS_PRIMITIVES_H_

#include <mqm.h>
#include <mcstypes.h>

// 码流控制定义：单位 Byte/s
#define  NO_FLOW_CONTROL        -1
#define  MIN_FLOW_MIN_VALUE     64*1024


// Mcs消息类型(小)定义
typedef enum
{
    // 绑定GccProvider和McsProvider
    e_Bind                          = 0,
    // 开始网络数据检测请求
    e_StartRequest                  = 1, 
    // 网络数据检测回复
    e_StartConfirm                  = 2,
    // 连接节点请求原语
    e_ConnectProviderRequest        = 3, 
    // 连接节点指示原语
    e_ConnectProviderIndication     = 4,
    // 连接节点回复原语
    e_ConnectProviderResponse       = 5,
    // 连接节点确认原语
    e_ConnectProviderConfirm        = 6,
    // 断开节点请求原语
    e_DisconnectProviderRequest     = 7, 
    // 断开节点指示原语
    e_DisconnectProviderIndication  = 8, 
    //创建Domain原语
    e_CreateDomainPrimitive         = 49,

    // 申请用户请求原语
    e_AttachUserRequest             = 9, 
    // 申请用户确认原语
    e_AttachUserConfirm             = 10, 
    // 删除用户请求原语
    e_DetachUserRequest             = 11, 
    // 删除用户指示原语
    e_DetachUserIndication          = 12, 

    // 加入信道请求原语
    e_ChannelJoinRequest            = 13, 
    // 加入信道确认原语
    e_ChannelJoinConfirm            = 14, 
    // 离开信道请求原语
    e_ChannelLeaveRequest           = 15, 
    // 离开信道指示原语
    e_ChannelLeaveIndication        = 16, 
    // 召集信道请求原语
    e_ChannelConveneRequest         = 17, 
    // 召集信道确认原语
    e_ChannelConveneConfirm         = 18, 
    // 解散信道请求原语
    e_ChannelDisbandRequest         = 19, 
    // 解散信道指示原语
    e_ChannelDisbandIndication      = 20, 
    // 允许用户加入信道请求原语
    e_ChannelAdmitRequest           = 21,  
    // 允许用户加入信道指示原语
    e_ChannelAdmitIndication        = 22, 
    // 不允许用户加入信道请求原语
    e_ChannelExpelRequest           = 23, 
    // 不允许用户加入信道指示原语
    e_ChannelExpelIndication        = 24, 

    // Send方式发送数据请求原语
    e_SendDataRequest               = 25, 
    // Send方式发送数据指示原语
    e_SendDataIndication            = 26, 
    // UniformSend方式发送数据请求原语
    e_UniformSendDataRequest        = 27, 
    // UniformSend方式发送数据指示原语
    e_UniformSendDataIndication     = 28, 

    // 申请创建令牌请求原语
    e_GenerateTokenRequest          = 29,
    // 申请创建令牌确认原语
    e_GenerateTokenConfirm          = 30,
    // 获得令牌请求原语
    e_TokenGrabRequest              = 31, 
    // 获得令牌确认原语
    e_TokenGrabConfirm              = 32, 
    // 抑制令牌请求原语
    e_TokenInhibitRequest           = 33, 
    // 抑制令牌确认原语
    e_TokenInhibitConfirm           = 34, 
    // 传递令牌请求原语
    e_TokenGiveRequest              = 35, 
    // 传递令牌指示原语
    e_TokenGiveIndication           = 36, 
    // 传递令牌回复原语
    e_TokenGiveResponse             = 37, 
    // 传递令牌确认原语
    e_TokenGiveConfirm              = 38, 
    // 请求令牌请求原语
    e_TokenPleaseRequest            = 39, 
    // 请求令牌指示原语
    e_TokenPleaseIndication         = 40, 
    // 释放令牌请求原语
    e_TokenReleaseRequest           = 41, 
    // 释放令牌确认原语
    e_TokenReleaseConfirm           = 42, 
    // 测试令牌请求原语
    e_TokenTestRequest              = 43, 
    // 测试令牌确认原语
    e_TokenTestConfirm              = 44,
    // 码流控制带宽设置
    e_SetFlowControl                = 45,
    // 码流控制带宽设置
    e_FlowControlConfirm            = 46,
    // 信道查询请求原语
    e_ChannelQueryRequest           = 47,
    // 信道查询确认原语
    e_ChannelQueryConfirm           = 48
} enumMcsMsgType;


/*====================================================================
类名 ： CBindPrimitive
描述 ： 绑定上层协议的队列、资源
====================================================================*/
class CMcsBindPrimitive : public CMsg
{
public:
    CMcsBindPrimitive():CMsg(e_Mcs, e_Bind)
    {
    }
};

/*====================================================================
类名 ： CMcsStartPrimitive
描述 ： MCS开始对网络进行监听
====================================================================*/
class CMcsStartRequest : public CMsg
{
public:
    CMcsStartRequest():CMsg(e_Mcs, e_StartRequest)
    {
    }
    
    // 启动地址
    PString m_strAddress;
};

/*====================================================================
类名 ： CMcsStartPrimitive
描述 ： MCS开始对网络进行监听
====================================================================*/
class CMcsStartConfirm : public CMsg
{
public:
    CMcsStartConfirm():CMsg(e_Mcs, e_StartConfirm)
    {
    }
    
    // 是否启动成功
    bool m_bResult;
};

/*====================================================================
类名 ： CMcsConnectProviderRequest
描述 ： 连接节点请求原语
====================================================================*/
class CMcsConnectProviderRequest : public CMsg
{
public:
    CMcsConnectProviderRequest():CMsg(e_Mcs, e_ConnectProviderRequest)
    {
    }

    // 呼叫方地址
    PString m_strCallingAddress;
    // 呼叫方Domain标识
    PString m_strCallingDomainSelector;
    // 被叫方地址
    PString m_strCalledAddress;
    // 被叫方Domain标识
    PString m_strCalledDomainSelector;
    // 呼叫方向标志：ture向上、false向下
    bool    m_bUpwardDownwardFlag;
    // Domain参数
    TDomainParameters m_cDomainParameters;
    // QoS类型
    TQos m_enumQos;
    // 用户数据，可能是Gcc PDU
    PBYTEArray m_cUserData;
};


/*====================================================================
类名 ： CMcsConnectProviderIndication
描述 ： 连接节点指示原语
====================================================================*/
class CMcsConnectProviderIndication : public CMsg
{
public:
    CMcsConnectProviderIndication():CMsg(e_Mcs, e_ConnectProviderIndication)
    {
    }

    // 该Tc的指针
    void* m_pTcPointer;
    // 呼叫方地址
    PString m_strCallingAddress;
    // 呼叫方Domain标识
    PString m_strCallingDomainSelector;
    // 被叫方地址
    PString m_strCalledAddress;
    // 被叫方Domain标识
    PString m_strCalledDomainSelector;
    // 呼叫方向标志：ture向上、false向下
    bool    m_bUpwardDownwardFlag;
    // Domain参数
    TDomainParameters m_cDomainParameters;
    // QoS类型
    TQos m_enumQos;
    // 用户数据，可能是Gcc PDU
    PBYTEArray m_cUserData;
};


/*====================================================================
类名 ： CMcsConnectProviderResponse
描述 ： 连接节点回复原语
====================================================================*/
class CMcsConnectProviderResponse : public CMsg
{
public:
    CMcsConnectProviderResponse():CMsg(e_Mcs, e_ConnectProviderResponse)
    {
    }

    // 该Tc的指针
    void* m_pTcPointer;
    // Domain参数
    TDomainParameters m_cDomainParameters;
    // QoS类型
    TQos m_enumQos;
    // 连接结果
    TResult m_enumResult;
    // 对应DOMANIN的MQM地址
    TMessageContext m_tMcsDomain;
    // 用户数据，可能是Gcc PDU
	PBYTEArray m_cUserData;
};

/*====================================================================
类名 ： CMcsCreateDomainPrimitive
描述 ： 主动加入远端会议时，创建与Conference对应Domain原语
====================================================================*/
class CMcsCreateDomainPrimitive: public CMsg
{
public:
    CMcsCreateDomainPrimitive():CMsg(e_Mcs, e_CreateDomainPrimitive)
    {
    }   
    //需要加入对方机器的地址
    PString m_strAddress;
};

/*====================================================================
类名 ： CMcsConnectProviderConfirm
描述 ： 连接节点确认原语
====================================================================*/
class CMcsConnectProviderConfirm : public CMsg
{
public:
    CMcsConnectProviderConfirm():CMsg(e_Mcs, e_ConnectProviderConfirm)
    {
    }

    // 被叫方地址
    PString m_strCalledAddress;
    // Domain参数
    TDomainParameters m_cDomainParameters;
    // QoS类型
    TQos m_enumQos;
    // 连接结果
    TResult m_enumResult;
    // 用户数据，可能是Gcc PDU
	PBYTEArray m_cUserData;
};


/*====================================================================
类名 ： CMcsDisconnectProviderRequest
描述 ： 断开节点请求原语
====================================================================*/
class CMcsDisconnectProviderRequest : public CMsg
{
public:
    CMcsDisconnectProviderRequest():CMsg(e_Mcs, e_DisconnectProviderRequest)
    {
    }
    
    // 请求Disconnect的Domain
    TMessageContext m_tMcsDomain;
};


/*====================================================================
类名 ： CMcsDisconnectProviderIndication
描述 ： 断开节点指示原语
====================================================================*/
class CMcsDisconnectProviderIndication : public CMsg
{
public:
    CMcsDisconnectProviderIndication():CMsg(e_Mcs, e_DisconnectProviderIndication)
    {
    }

    // 断开连接的原因
    TReason m_enumReason;
    // 是否是上级节点断开
    bool m_bAboveDisconnect;
};


/*====================================================================
类名 ： CMcsAttachUserRequest
描述 ： 申请用户请求原语
====================================================================*/
class CMcsAttachUserRequest : public CMsg
{
public:
    CMcsAttachUserRequest():CMsg(e_Mcs, e_AttachUserRequest)
    {
    }

    // Domain标识
    PString m_strDomainSelector;
};


/*====================================================================
类名 ： CMcsAttachUserConfirm
描述 ： 申请用户确认原语
====================================================================*/
class CMcsAttachUserConfirm : public CMsg
{
public:
    CMcsAttachUserConfirm():CMsg(e_Mcs, e_AttachUserConfirm)
    {
    }

    // 申请结果
    TResult  m_enumResult;
    // 得到的用户ID
    TUserId m_nUserId;
};


/*====================================================================
类名 ： CMcsDetachUserRequest
描述 ： 删除用户请求原语
====================================================================*/
class CMcsDetachUserRequest : public CMsg
{
public:
    CMcsDetachUserRequest():CMsg(e_Mcs, e_DetachUserRequest)
    {
    }

public:
    // 删除的用户ID列表
    TUserIdList m_listUserIds;
};


/*====================================================================
类名 ： CMcsDetachUserIndication
描述 ： 删除用户指示原语
====================================================================*/
class CMcsDetachUserIndication : public CMsg
{
public:
    CMcsDetachUserIndication():CMsg(e_Mcs, e_DetachUserIndication)
    {
    }

    // 删除结果
    TResult  m_enumResult;
    // 删除的用户ID列表
    TUserIdList m_listUserIds;
};


/*====================================================================
类名 ： CMcsChannelJoinRequest
描述 ： 加入信道请求原语
====================================================================*/
class CMcsChannelJoinRequest : public CMsg
{
public:
    CMcsChannelJoinRequest():CMsg(e_Mcs, e_ChannelJoinRequest)
    {
    }

    TUserId m_nUserId;
    // 请求加入的信道ID
    TChannelId m_nChannelToJoin;
};


/*====================================================================
类名 ： CMcsChannelJoinConfirm
描述 ： 加入信道确认原语
====================================================================*/
class CMcsChannelJoinConfirm : public CMsg
{
public:
    CMcsChannelJoinConfirm():CMsg(e_Mcs, e_ChannelJoinConfirm)
    {
    }

    // 请求用户
    TUserId m_nUserId;
    // 请求加入的信道ID
    TChannelId m_nChannelToJoin;
    // 加入的信道ID
    TChannelId m_nChannelJoined;
    // 处理结果
    TResult     m_enumResult;
};


/*====================================================================
类名 ： CMcsChannelLeaveRequest
描述 ： 离开信道请求原语
====================================================================*/
class CMcsChannelLeaveRequest : public CMsg
{
public:
    CMcsChannelLeaveRequest():CMsg(e_Mcs, e_ChannelLeaveRequest)
    {
    }

    // 请求离开的用户
    TUserId m_nUserId;
    // 请求离开的信道ID
    TChannelId m_nChannelToLeave;
};


/*====================================================================
类名 ： CMcsChannelLeaveIndication
描述 ： 离开信道指示原语
====================================================================*/
class CMcsChannelLeaveIndication : public CMsg
{
public:
    CMcsChannelLeaveIndication():CMsg(e_Mcs, e_ChannelLeaveIndication)
    {
    }

    // 请求离开的信道ID
    TChannelId m_nChannelToLeave;
    // 处理结果
    TResult     m_enumResult;
};


/*====================================================================
类名 ： CMcsChannelConveneRequest
描述 ： 召集信道请求原语
====================================================================*/
class CMcsChannelConveneRequest : public CMsg
{
public:
    CMcsChannelConveneRequest():CMsg(e_Mcs, e_ChannelConveneRequest)
    {
    }
    // 召集者
    TUserId m_nUserId;
};


/*====================================================================
类名 ： CMcsChannelConveneConfirm
描述 ： 召集信道确认原语
====================================================================*/
class CMcsChannelConveneConfirm : public CMsg
{
public:
    CMcsChannelConveneConfirm():CMsg(e_Mcs, e_ChannelConveneConfirm)
    {
    }

    // 处理结果
    TResult     m_enumResult;
    // 召集的信道
    TChannelId m_nConvenedChannelId;
};


/*====================================================================
类名 ： CMcsChannelDisbandRequest
描述 ： 解散信道请求原语
====================================================================*/
class CMcsChannelDisbandRequest : public CMsg
{
public:
    CMcsChannelDisbandRequest():CMsg(e_Mcs, e_ChannelDisbandRequest)
    {
    }

    // 请求解散的信道
    TChannelId m_nChannelToDisband;
    // 请求解散的用户
    TUserId m_nUserId;
};


/*====================================================================
类名 ： CMcsChannelDisbandIndication
描述 ： 解散信道指示原语
====================================================================*/
class CMcsChannelDisbandIndication : public CMsg
{
public:
    CMcsChannelDisbandIndication():CMsg(e_Mcs, e_ChannelDisbandIndication)
    {
    }

    // 被解散的信道
    TChannelId m_nChannelToDisband;
    // 解散结果
    TResult     m_enumResult;
};


/*====================================================================
类名 ： CMcsChannelAdmitRequest
描述 ： 允许用户加入信道请求原语
====================================================================*/
class CMcsChannelAdmitRequest : public CMsg
{
public:
    CMcsChannelAdmitRequest():CMsg(e_Mcs, e_ChannelAdmitRequest)
    {
    }

    // 私有信道ID
    TChannelId     m_nChannelToAdmit;
    // 允许加入该信道的用户列表
    TUserIdList m_listAdmitedUserIds;
    // 请求加入的用户
    TUserId m_nUserId;
};


/*====================================================================
类名 ： CMcsChannelAdmitIndication
描述 ： 允许用户加入信道指示原语
====================================================================*/
class CMcsChannelAdmitIndication : public CMsg
{
public:
    CMcsChannelAdmitIndication():CMsg(e_Mcs, e_ChannelAdmitIndication)
    {
    }

    // 私有信道ID
    TChannelId m_nChannelToAdmit;
    // 该私有信道的管理者
    TUserId    m_nManagerUserId;
};


/*====================================================================
类名 ： CMcsChannelExpelRequest
描述 ： 不允许用户加入信道请求原语
====================================================================*/
class CMcsChannelExpelRequest : public CMsg
{
public:
    CMcsChannelExpelRequest():CMsg(e_Mcs, e_ChannelExpelRequest)
    {
    }

    // 私有信道ID
    TChannelId         m_nChannelToExpel;
    // 不允许加入该信道的用户列表
    TUserIdList m_listExpeledUserIds;
    // 请求踢除的用户
    TUserId m_nUserId;
};


/*====================================================================
类名 ： CMcsChannelExpelIndication
描述 ： 不允许用户加入信道指示原语
====================================================================*/
class CMcsChannelExpelIndication : public CMsg
{
public:
    CMcsChannelExpelIndication():CMsg(e_Mcs, e_ChannelExpelIndication)
    {
    }

    // 私有信道ID
    TChannelId m_nChannelToExpel;
    // 被拒绝的原因
    TReason     m_enumReason;
};


/*====================================================================
类名 ： CSendDataRequest
描述 ： Send方式发送数据请求原语
====================================================================*/
class CSendDataRequest : public CMsg
{
public:
    CSendDataRequest():CMsg(e_Mcs, e_SendDataRequest)
    {
    }

    // 数据优先级
    TDataPriority   m_enumPriroty;
    // 发送数据到该信道
    TChannelId m_nChannelId;
    // 该数据的发送者
    TUserId    m_nSenderId;
    // 要发送的数据
    PBYTEArray m_cData;
};


/*====================================================================
类名 ： CSendDataIndication
描述 ： Send方式发送数据指示原语
====================================================================*/
class CSendDataIndication : public CMsg
{
public:
    CSendDataIndication():CMsg(e_Mcs, e_SendDataIndication)
    {
    }

    // 数据优先级
    TDataPriority   m_enumPriroty;
    // 数据从该信道得到
    TChannelId m_nChannelId;
    // 该数据的发送者
    TUserId    m_nSenderId;
    // 接收到的数据
    PBYTEArray m_cData;
};


/*====================================================================
类名 ： CUniformSendDataRequest
描述 ： UniformSend方式发送数据请求原语
====================================================================*/
class CUniformSendDataRequest : public CMsg
{
public:
    CUniformSendDataRequest():CMsg(e_Mcs, e_UniformSendDataRequest)
    {
    }

    // 数据优先级
    TDataPriority   m_enumPriroty;
    // 发送数据到该信道
    TChannelId m_nChannelId;
    // 数据的发送者
    TUserId m_nSenderId;
    // 要发送的数据
    PBYTEArray m_cData;
};


/*====================================================================
类名 ： CUniformSendDataIndication
描述 ： UniformSend方式发送数据指示原语
====================================================================*/
class CUniformSendDataIndication : public CMsg
{
public:
    CUniformSendDataIndication():CMsg(e_Mcs, e_UniformSendDataIndication)
    {
    }

    // 数据优先级
    TDataPriority   m_enumPriroty;
    // 数据从该信道得到
    TChannelId m_nChannelId;
    // 该数据的发送者
    TUserId    m_nSenderId;
    // 接收到的数据
    PBYTEArray m_cData;
};



/*====================================================================
类名 ： CMcsGenerateTokenIdRequest
描述 ： 创建令牌请求原语
====================================================================*/
class CMcsGenerateTokenRequest : public CMsg
{
public:
    CMcsGenerateTokenRequest():CMsg(e_Mcs, e_GenerateTokenRequest)
    {
    }

};



/*====================================================================
类名 ： CMcsGenerateTokenIdConfirm
描述 ： 创建令牌确认原语
====================================================================*/
class CMcsGenerateTokenConfirm : public CMsg
{
public:
    CMcsGenerateTokenConfirm():CMsg(e_Mcs, e_GenerateTokenConfirm)
    {
    }

    // 创建的令牌ID
    TTokenId m_nTokenId;
};


/*====================================================================
类名 ： CMcsTokenGrabRequest
描述 ： 获得令牌请求原语
====================================================================*/
class CMcsTokenGrabRequest : public CMsg
{
public:
    CMcsTokenGrabRequest():CMsg(e_Mcs, e_TokenGrabRequest)
    {
    }

    // 请求获取的令牌ID
    TTokenId m_nTokenId;
    // 请求者
    TUserId m_nGrabberId;
};


/*====================================================================
类名 ： CMcsTokenGrabConfirm
描述 ： 获得令牌确认原语
====================================================================*/
class CMcsTokenGrabConfirm : public CMsg
{
public:
    CMcsTokenGrabConfirm():CMsg(e_Mcs, e_TokenGrabConfirm)
    {
    }

    // 请求获取的令牌ID
    TTokenId m_nTokenId;
    // 处理结果
    TResult m_enumResult;
    // 令牌状态
    TTokenStatus m_enumStatus;
};


/*====================================================================
类名 ： CMcsTokenInhibitRequest
描述 ： 抑制令牌请求原语
====================================================================*/
class CMcsTokenInhibitRequest : public CMsg
{
public:
    CMcsTokenInhibitRequest():CMsg(e_Mcs, e_TokenInhibitRequest)
    {
    }

    // 请求抑制的令牌ID
    TTokenId m_nTokenId;
    // 请求者ID
    TUserId m_nInitiator;
};


/*====================================================================
类名 ： CMcsTokenInhibitConfirm
描述 ： 抑制令牌确认原语
====================================================================*/
class CMcsTokenInhibitConfirm : public CMsg
{
public:
    CMcsTokenInhibitConfirm():CMsg(e_Mcs, e_TokenInhibitConfirm)
    {
    }

    // 请求抑制的令牌ID
    TUserId m_nTokenId;
    // 处理结果
    TResult m_enumResult;
    // 令牌状态
    TTokenStatus m_enumStatus;
};


/*====================================================================
类名 ： CMcsTokenGiveRequest
描述 ： 传递令牌请求原语
====================================================================*/
class CMcsTokenGiveRequest : public CMsg
{
public:
    CMcsTokenGiveRequest():CMsg(e_Mcs, e_TokenGiveRequest)
    {
    }

    // 令牌的接收者ID
    TUserId  m_nRecverId;
    // 请求传递的令牌ID
    TTokenId m_nTokenId;
};


/*====================================================================
类名 ： CMcsTokenGiveIndication
描述 ： 传递令牌指示原语
====================================================================*/
class CMcsTokenGiveIndication : public CMsg
{
public:
    CMcsTokenGiveIndication():CMsg(e_Mcs, e_TokenGiveIndication)
    {
    }

    // 令牌的发送者ID
    TUserId  m_nSenderId;
    // 请求传递的令牌ID
    TTokenId m_nTokenId;
};


/*====================================================================
类名 ： CMcsTokenGiveResponse
描述 ： 传递令牌回复原语
====================================================================*/
class CMcsTokenGiveResponse : public CMsg
{
public:
    CMcsTokenGiveResponse():CMsg(e_Mcs, e_TokenGiveResponse)
    {
    }

    // 处理结果
    TResult m_enumResult;
    // 令牌状态
    TTokenStatus m_enumStatus;
};


/*====================================================================
类名 ： CMcsTokenGiveConfirm
描述 ： 传递令牌确认原语
====================================================================*/
class CMcsTokenGiveConfirm : public CMsg
{
public:
    CMcsTokenGiveConfirm():CMsg(e_Mcs, e_TokenGiveConfirm)
    {
    }

    // 传递的令牌ID
    TTokenId m_nTokenId;
    // 处理结果
    TResult m_enumResult;
    // 令牌状态
    TTokenStatus m_enumStatus;
};


/*====================================================================
类名 ： CMcsTokenPleaseRequest
描述 ： 请求令牌请求原语
====================================================================*/
class CMcsTokenPleaseRequest : public CMsg
{
public:
    CMcsTokenPleaseRequest():CMsg(e_Mcs, e_TokenPleaseRequest)
    {
    }

    // 请求的令牌ID
    TTokenId m_nTokenId;
};


/*====================================================================
类名 ： CMcsTokenPleaseIndication
描述 ： 请求令牌指示原语
====================================================================*/
class CMcsTokenPleaseIndication : public CMsg
{
public:
    CMcsTokenPleaseIndication():CMsg(e_Mcs, e_TokenPleaseIndication)
    {
    }

    // 请求者ID
    TUserId  m_nRequesterId;
    // 请求的令牌ID
    TTokenId m_nTokenId;
};


/*====================================================================
类名 ： CMcsTokenReleaseRequest
描述 ： 释放令牌请求原语
====================================================================*/
class CMcsTokenReleaseRequest : public CMsg
{
public:
    CMcsTokenReleaseRequest():CMsg(e_Mcs, e_TokenReleaseRequest)
    {
    }

    // 申请释放者ID
    TUserId  m_nReleaserId;
    // 要释放的令牌ID
    TTokenId m_nTokenId;
};


/*====================================================================
类名 ： CMcsTokenReleaseConfirm
描述 ： 释放令牌确认原语
====================================================================*/
class CMcsTokenReleaseConfirm : public CMsg
{
public:
    CMcsTokenReleaseConfirm():CMsg(e_Mcs, e_TokenReleaseConfirm)
    {
    }

    // 要释放的令牌ID
    TTokenId m_nTokenId;
    // 处理结果
    TResult m_enumResult;
    // 令牌状态
    TTokenStatus m_enumStatus;
};


/*====================================================================
类名 ： CMcsTokenTestRequest
描述 ： 测试令牌请求原语
====================================================================*/
class CMcsTokenTestRequest : public CMsg
{
public:
    CMcsTokenTestRequest():CMsg(e_Mcs, e_TokenTestRequest)
    {
    }

    // 要测试的令牌ID
    TTokenId m_nTokenId;
};


/*====================================================================
类名 ： CMcsTokenTestConfirm
描述 ： 测试令牌确认原语
====================================================================*/
class CMcsTokenTestConfirm : public CMsg
{
public:
    CMcsTokenTestConfirm():CMsg(e_Mcs, e_TokenTestConfirm)
    {
    }

    // 要测试的令牌ID
    TTokenId    m_nTokenId;
    // 处理结果
    TResult m_enumResult;
    // 该令牌的状态
    TTokenStatus m_enumTokenStatus;
};



/*====================================================================
类名 ： CMcsSetFlowControl
描述 ： 码流控制代码设置
====================================================================*/
class CMcsSetFlowControl : public CMsg
{
public:
    CMcsSetFlowControl():CMsg(e_Mcs, e_SetFlowControl)
    {
    }

    // 设置的码流带宽，单位 Byte/s
    s32 m_nFlowBand;
};

/*====================================================================
类名 ： CMcsSetFlowControl
描述 ： 码流控制代码设置
====================================================================*/
class CMcsFlowControlConfirm : public CMsg
{
public:
    CMcsFlowControlConfirm():CMsg(e_Mcs, e_FlowControlConfirm)
    {
    }

    // 已设置的码流带宽，单位 Byte/s
    s32 m_nFlowBand;
};


/*====================================================================
类名 ： CMcsChannelQueryRequest
描述 ： 信道信息查询请求
====================================================================*/
class CMcsChannelQueryRequest : public CMsg
{
public:
    CMcsChannelQueryRequest():CMsg(e_Mcs, e_ChannelQueryRequest)
    {
    }

    // 请求查询的信道ID
    TChannelId m_nQueryChannelId;
};


/*====================================================================
类名 ： CMcsChannelQueryConfirm
描述 ： 信道信息查询确认
====================================================================*/
class CMcsChannelQueryConfirm : public CMsg
{
public:
    CMcsChannelQueryConfirm():CMsg(e_Mcs, e_ChannelQueryConfirm)
    {
    }

    // 请求查询的信道ID
    TChannelId m_nQueryChannelId;
    // 处理结果
    TResult m_enumResult;
    // 该信道对于的连接来自IP地址
    PString m_strFrom;
    // 该连接是否被动接收的
    bool m_bIsPassive;
};


#endif
