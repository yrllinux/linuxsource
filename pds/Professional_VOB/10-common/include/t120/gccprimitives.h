/******************************************************************************
模块名      ：GccPrimitives
文件名      ：GccPrimitives.h
相关文件    ：GccPrimitives.cpp
文件实现功能：定义所有GCC原语
作者        ：赖齐
版本        ：4.0
-------------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      走读人      修改内容
24/05/05    4.0         赖齐        赖齐        创建
******************************************************************************/
#ifndef _GCC_PRIMITIVES_H_
#define _GCC_PRIMITIVES_H_

#include <mqm.h>
#include <gcctypes.h>
#include <gccregistry.h>

using namespace Gcc;

typedef u8 TApeId;

// Gcc消息类型(小)定义
typedef enum
{
    // 启动Gcc请求原语
    e_StartGccRequest,
    // 启动Gcc确认原语
    e_StartGccConfirm,
    // 加入NC到GCC原语
    e_NcAttachPrimitive,
    // 加入APE到GCC原语
    e_ApplicationAttachPrimitive,
    // 删除APE出GCC原语
    e_ApplicationDetachPrimitive,
    // 创建会议请求原语
    e_ConferenceCreateRequest,
    // 创建会议确认原语
    e_ConferenceCreateConfirm,
    // 会议查询请求原语
    e_ConferenceQueryRequest,
    // 会议查询确认原语
    e_ConferenceQueryConfirm,
    // 加入会议请求原语
    e_ConferenceJoinRequest,
    // 加入会议指示原语
    e_ConferenceJoinIndication,
    // 加入会议回复原语
    e_ConferenceJoinResponse,
    // 加入会议确认原语
    e_ConferenceJoinConfirm,
    // 会议邀请请求原语
    e_ConferenceInviteRequest,
    // 会议邀请指示原语
    e_ConferenceInviteIndication,
    // 会议邀请回复原语
    e_ConferenceInviteResponse,
    // 会议邀请确认原语
    e_ConferenceInviteConfirm,
    // 会议断开请求原语
    e_ConferenceDisconnectRequest,
    // 会议断开指示原语
    e_ConferenceDisconnectIndication,
    // 会议断开确认原语
    e_ConferenceDisconnectConfirm,
    // 结束会议请求原语
    e_ConferenceTerminateRequest,
    // 结束会议指示原语
    e_ConferenceTerminateIndication,
    // 结束会议确认原语
    e_ConferenceTerminateConfirm,
    // 挂断节点请求原语
    e_ConferenceEjectUserRequest,
    // 挂断节点指示原语
    e_ConferenceEjectUserIndication,
    // 挂断节点确认原语
    e_ConferenceEjectUserConfirm,
    // 宣布节点请求原语
    e_ConferenceAnnouncePresenceRequest,
    // 宣布节点确认原语
    e_ConferenceAnnouncePresenceConfirm,
    // 会议登记表汇报指示原语（未使用）
    e_ConferenceRosterReportIndication,
    // 会议中新增节点通知原语
    e_ConferenceNodeAddIndication,
    // 会议中删除节点通知原语
    e_ConferenceNodeRemoveIndication,
    // 指定应用掩码通知原语
    e_ConferenceApeMaskIndication,
    // 会议登记表查询请求原语
    e_ConferenceRosterInquireRequest,
    // 会议登记表查询确认原语（未使用）
    e_ConferenceRosterInquireConfirm,
    // 允许应用登记指示原语
    e_ApplicationPermissionToEnrollIndication,
    // 应用登记请求原语
    e_ApplicationEnrollRequest,
    // 应用登记确认原语
    e_ApplicationEnrollConfirm,
    // 应用登记表汇报指示原语（未使用）
    e_ApplicationRosterReportIndication,
    // 会议中新增应用通知原语 
    e_ConferenceApeAddIndication,
    // 会议中修改应用通知原语
    e_ConferenceApeReplaceIndication,
    // 会议中删除应用通知原语
    e_ConferenceApeRemoveIndication,
    // 应用登记表查询请求原语
    e_ApplicationRosterInquireRequest,
    // 应用登记表查询确认原语
    e_ApplicationRosterInquireConfirm,
    // 注册信道请求原语
    e_RegistryRegisterChannelRequest,
    // 注册信道确认原语
    e_RegistryRegisterChannelConfirm,
    // 申请注册令牌请求原语
    e_RegistryAssignTokenRequest,
    // 申请注册令牌确认原语
    e_RegistryAssignTokenConfirm,
    // 查询注册项请求原语
    e_RegistryRetrieveEntryRequest,
    // 查询注册项确认原语
    e_RegistryRetrieveEntryConfirm,
    // 删除注册项请求原语
    e_RegistryDeleteEntryRequest,
    // 删除注册项确认原语
    e_RegistryDeleteEntryConfirm,
    // 申请句柄请求原语
    e_RegistryAllocateHandleRequest,
    // 申请句柄确认原语
    e_RegistryAllocateHandleConfirm,
    // 指定会议主席请求(非标准)
    e_AssignConductorRequest,
    // 设置码流请求(非标准)
    e_ConfSetFlowControl,
    // 设置码流结果(非标准)
    e_ConfFlowControlConfirm
} enumGccMsgType;



/*====================================================================
类名 ： CStartGccRequest
描述 ： 启动Gcc请求原语
====================================================================*/
class CStartGccRequest : public CMsg
{
public:
    CStartGccRequest():CMsg(e_Gcc, e_StartGccRequest)
    {
    }

    // 启动地址
    PString m_strAddress;
};


/*====================================================================
类名 ： CStartGccConfirm
描述 ： 启动Gcc确认原语
====================================================================*/
class CStartGccConfirm : public CMsg
{
public:
    CStartGccConfirm():CMsg(e_Gcc, e_StartGccConfirm)
    {
    }

    // 启动结果
    bool m_bResult;
};


/*====================================================================
类名 ： CNcAttachPrimitive
描述 ： 加入NodeController到GCC原语
====================================================================*/
class CNcAttachPrimitive : public CMsg
{
public:
    CNcAttachPrimitive():CMsg(e_Gcc, e_NcAttachPrimitive)
    {
    }

};


/*====================================================================
类名 ： CApplicationAttachPrimitive
描述 ： 加入APE到GCC原语
====================================================================*/
class CApplicationAttachPrimitive : public CMsg
{
public:
    CApplicationAttachPrimitive():CMsg(e_Gcc, e_ApplicationAttachPrimitive)
    {
    }

    // 该Ape的SessionKey
    SessionKey m_cSessionKey;
};




/*====================================================================
类名 ： CApplicationDetachPrimitive
描述 ： 删除APE出GCC原语
====================================================================*/
class CApplicationDetachPrimitive : public CMsg
{
public:
    CApplicationDetachPrimitive():CMsg(e_Gcc, e_ApplicationDetachPrimitive)
    {
    }

    // 该Ape的SessionKey
    SessionKey m_cSessionKey;
};


/*====================================================================
类名 ： CGccConferenceCreateRequest
描述 ： 创建会议请求原语
====================================================================*/
class CGccConferenceCreateRequest : public CMsg
{
public:
    CGccConferenceCreateRequest():CMsg(e_Gcc, e_ConferenceCreateRequest)
    {
        m_enumQualityOfService = 0;
        m_strUserData.Empty();
    }

    // 会议描述子
    ConferenceDescriptor m_cConferenceDescriptor;
    // 会议对应的Mcs参数
    TDomainParameters m_cDomainParameters;
    // Qos类型
    TQos m_enumQualityOfService;
    // 用户数据
    PString m_strUserData;
};


/*====================================================================
类名 ： CGccConferenceCreateConfirm
描述 ： 创建会议确认原语
====================================================================*/
class CGccConferenceCreateConfirm : public CMsg
{
public:
    CGccConferenceCreateConfirm():CMsg(e_Gcc, e_ConferenceCreateConfirm)
    {
    }

    // 会议描述子
    ConferenceDescriptor m_cConferenceDescriptor;
    // 创建会议的会议ID
    u32 m_nConferenceID;
    // 该会议对应的Domain队列信息
    TMessageContext m_tMcsDomain;
    // 会议对应的Mcs参数
    TDomainParameters m_cDomainParameters;
    // Qos类型
    TQos m_enumQualityOfService;
    // 用户数据
    PString m_strUserData;
    // 创建结果
    ConferenceResponseResult m_enumResult;
    // 是否本地创建
    bool m_bIsLocalCreate;
};


/*====================================================================
类名 ： CConferenceQueryRequest
描述 ： 会议查询请求原语
====================================================================*/
class CConferenceQueryRequest : public CMsg
{
public:
    CConferenceQueryRequest():CMsg(e_Gcc, e_ConferenceQueryRequest)
    {
    }

    // 节点类型
    NodeType m_enumNodeType;
    // 被查询节点的地址
    PString m_strCalledAddress;
};


/*====================================================================
类名 ： CConferenceQueryConfirm
描述 ： 会议查询确认原语
====================================================================*/
class CConferenceQueryConfirm : public CMsg
{
public:
    CConferenceQueryConfirm():CMsg(e_Gcc, e_ConferenceQueryConfirm)
    {
    }

    // 查询结果
    ConferenceResponseResult m_enumResult; 
    // 节点类型
    NodeType m_enumNodeType;
    // 该节点上所有被列出的会议
    std::list<ConferenceDescriptor> m_listConferenceDescriptorList;
    // 被查询节点的地址
    PString m_strCalledAddress;
};


/*====================================================================
类名 ： CConferenceJoinRequest
描述 ： 加入会议请求原语
====================================================================*/
class CConferenceJoinRequest : public CMsg
{
public:
    CConferenceJoinRequest():CMsg(e_Gcc, e_ConferenceJoinRequest)
    {
        m_nConfId = 0;
        m_strPassword.MakeEmpty();
    }

    // 会议名称
    PString m_strConferenceName;
    // 会议ID（即数字名称）
    u32 m_nConfId;
    // 会议数字名称
    PString m_strConfNumericName;
    // 会议名称修饰符
    PString m_strCalledNodeConferenceNameModifier;
    // 会议密码
    PString m_strPassword;
    // 请求者名称
    PString m_strCallerIdentifier;
    // 请求加入会议所在节点的地址
    PString m_strCalledAddress;
    // 该会议分配的最大带宽（码流限制）
    u32 m_nFlowControl;
    // 会议对应的Mcs参数
    TDomainParameters m_cDomainParameters;
    // Qos类型
    TQos m_enumQualityOfService;
    // 用户数据
    PBYTEArray m_cUserData;
};


/*====================================================================
类名 ： CConferenceJoinIndication
描述 ： 加入会议指示原语
====================================================================*/
class CConferenceJoinIndication : public CMsg
{
public:
    CConferenceJoinIndication():CMsg(e_Gcc, e_ConferenceJoinIndication)
    {
    }

    // 会议ID
    u32 m_nConferenceId;
    // 会议密码
    PString m_strPassword;
    // 请求者名称
    PString m_strCallerIdentifier;
    // 是否直接加入本节点
    bool m_bIsLocalJoined;
    // 请求者的IP地址（直接加入有效，级联加入时是被加入节点信息）
    PString m_strCallingAddress;
    // 请求者的NodeId（级联加入时是被加入节点NodeId，直接加入时是本节点NodeId）
    TUserId m_nFromNodeId;
    // 请求Tag
    u32 m_nTag;
    // 用户数据
//    PBYTEArray m_cUserData;
};


/*====================================================================
类名 ： CConferenceJoinResponse
描述 ： 加入会议回复原语
====================================================================*/
class CConferenceJoinResponse : public CMsg
{
public:
    CConferenceJoinResponse():CMsg(e_Gcc, e_ConferenceJoinResponse)
    {
    }

    // 会议ID
    u32 m_nConferenceId;
    // 会议密码
    PString m_strPassword;
    // 用户数据
    PBYTEArray m_cUserData;
    // 请求Tag, 用来标识不同的请求
    u32 m_nTag;
    // 处理结果
    ConferenceResponseResult m_enumResult;
};


/*====================================================================
类名 ： CConferenceJoinConfirm
描述 ： 加入会议确认原语
====================================================================*/
class CConferenceJoinConfirm : public CMsg
{
public:
    CConferenceJoinConfirm():CMsg(e_Gcc, e_ConferenceJoinConfirm)
    {
    }

    // 会议描述子
    ConferenceDescriptor m_cConferenceDescriptor;
    // 创建会议的会议ID
    u32 m_nConferenceID;
    // 会议密码
    PString m_strPassword;
    // 加入地址
    PString m_strJoinAddr;
    // 会议对应的Mcs参数
    TDomainParameters m_strDomainParameters;
    // Qos类型
    TQos m_enumQualityOfService;
    // 用户数据
    PBYTEArray m_cUserData;
    // 处理结果
    ConferenceResponseResult m_enumResult;
};


/*====================================================================
类名 ： CConferenceInviteRequest
描述 ： 会议邀请请求原语
====================================================================*/
class CConferenceInviteRequest : public CMsg
{
public:
    CConferenceInviteRequest():CMsg(e_Gcc, e_ConferenceInviteRequest)
    {
    }

    // 会议ID
    u32 m_nConferenceID;
    // 请求者名称
    PString m_strCallerIdentifier;
    // 请求者地址
    PString m_strCallingAddress;
    // 被邀请节点地址
    PString m_strCalledAddress;
    // 用户数据
    PBYTEArray m_cUserData;
};


/*====================================================================
类名 ： CConferenceInviteIndication
描述 ： 会议邀请指示原语
====================================================================*/
class CConferenceInviteIndication : public CMsg
{
public:
    CConferenceInviteIndication():CMsg(e_Gcc, e_ConferenceInviteIndication)
    {
    }

    // 会议描述子
    ConferenceDescriptor m_cConferenceDescriptor;
    // 会议ID
    u32 m_nConferenceID;
    // 请求编号
    u32 m_nInviteTag;
    // 请求者名称
    PString m_strCallerIdentifier;
    // 请求者地址
    PString m_strCallingAddress;
    // 会议对应的Mcs参数
    TDomainParameters m_cDomainParameters;
    // Qos类型
    TQos m_enumQualityOfService;
    // 用户数据
    PBYTEArray m_cUserData;
};


/*====================================================================
类名 ： CConferenceInviteResponse
描述 ： 会议邀请回复原语
====================================================================*/
class CConferenceInviteResponse : public CMsg
{
public:
    CConferenceInviteResponse():CMsg(e_Gcc, e_ConferenceInviteResponse)
    {
    }

    // 请求编号
    u32 m_nInviteTag;
    // 会议ID
    u32 m_nConferenceID;
    // 会议名称
    PString m_strConferenceName;
    // 会议名称修饰符
    PString m_strConferenceNameModifier;
    // 该会议分配的最大带宽（码流限制）
    u32 m_nFlowControl;
    // 会议对应的Mcs参数
    TDomainParameters m_cDomainParameters;
    // Qos类型
    TQos m_enumQualityOfService;
    // 用户数据
    PBYTEArray m_cUserData;
    // 操作结果
    ConferenceResponseResult m_enumResult;
};


/*====================================================================
类名 ： CConferenceInviteConfirm
描述 ： 会议邀请确认原语
====================================================================*/
class CConferenceInviteConfirm : public CMsg
{
public:
    CConferenceInviteConfirm():CMsg(e_Gcc, e_ConferenceInviteConfirm)
    {
    }

    // 会议ID
    u32 m_nConferenceID;
    // 邀请节点地址
    PString m_strTargetAddr;
    // 用户数据
    PBYTEArray m_cUserData;
    // 操作结果
    ConferenceResponseResult m_enumResult;
};


/*====================================================================
类名 ： CConferenceDisconnectRequest
描述 ： 会议断开请求原语
====================================================================*/
class CConferenceDisconnectRequest : public CMsg
{
public:
    CConferenceDisconnectRequest():CMsg(e_Gcc, e_ConferenceDisconnectRequest)
    {
    }

    // 会议ID
    u32 m_nConferenceID;
    // GccConference的队列资源
    TMessageContext m_tGccConference;
};


/*====================================================================
类名 ： CConferenceDisconnectIndication
描述 ： 会议断开指示原语
====================================================================*/
class CConferenceDisconnectIndication : public CMsg
{
public:
    CConferenceDisconnectIndication():CMsg(e_Gcc, e_ConferenceDisconnectIndication)
    {
    }

    // 会议ID
    u32 m_nConferenceID;
    // 断开节点的NodeID
    TUserId m_nDisconnectingNodeID;
};


/*====================================================================
类名 ： CConferenceDisconnectConfirm
描述 ： 会议断开确认原语
====================================================================*/
class CConferenceDisconnectConfirm : public CMsg
{
public:
    CConferenceDisconnectConfirm():CMsg(e_Gcc, e_ConferenceDisconnectConfirm)
    {
    }

    // 会议ID
    u32 m_nConferenceID;
    // GccConference的队列资源
    TMessageContext m_tGccConference;
    // 操作结果
    ConferenceResponseResult m_enumResult;
};


/*====================================================================
类名 ： CConferenceTerminateRequest
描述 ： 结束会议请求原语
====================================================================*/
class CConferenceTerminateRequest : public CMsg
{
public:
    CConferenceTerminateRequest():CMsg(e_Gcc, e_ConferenceTerminateRequest)
    {
    }

    // 会议ID
    u32 m_nConferenceID;
    // 结束原因
    ConferenceTerminateReason m_enumReason;
};


/*====================================================================
类名 ： CConferenceTerminateIndication
描述 ： 结束会议指示原语
====================================================================*/
class CConferenceTerminateIndication : public CMsg
{
public:
    CConferenceTerminateIndication():CMsg(e_Gcc, e_ConferenceTerminateIndication)
    {
    }

    // 会议ID
    u32 m_nConferenceID;
    // 结束原因
    ConferenceTerminateReason m_enumReason;
};


/*====================================================================
类名 ： CConferenceTerminateConfirm
描述 ： 结束会议确认原语
====================================================================*/
class CConferenceTerminateConfirm : public CMsg
{
public:
    CConferenceTerminateConfirm():CMsg(e_Gcc, e_ConferenceTerminateConfirm)
    {
    }

    // 会议ID
    u32 m_nConferenceID;
    // 操作结果
    ConferenceResponseResult m_enumResult;
};


/*====================================================================
类名 ： CConferenceEjectUserRequest
描述 ： 挂断节点请求原语
====================================================================*/
class CConferenceEjectUserRequest : public CMsg
{
public:
    CConferenceEjectUserRequest():CMsg(e_Gcc, e_ConferenceEjectUserRequest)
    {
    }

    // 会议ID
    u32 m_nConferenceID;
    // 被挂断节点的NodeID
    TUserId m_nEjectedNodeID;
    // 挂断原因
    EjectUserReason m_enumReason;
};


/*====================================================================
类名 ： CConferenceEjectUserIndication
描述 ： 挂断节点指示原语
====================================================================*/
class CConferenceEjectUserIndication : public CMsg
{
public:
    CConferenceEjectUserIndication():CMsg(e_Gcc, e_ConferenceEjectUserIndication)
    {
    }

    // 会议ID
    u32 m_nConferenceID;
    // 被挂断节点的NodeID
    TUserId m_nEjectedNodeID;
    // 挂断原因
    EjectUserReason m_enumReason;
};


/*====================================================================
类名 ： CConferenceEjectUserConfirm
描述 ： 挂断节点确认原语
====================================================================*/
class CConferenceEjectUserConfirm : public CMsg
{
public:
    CConferenceEjectUserConfirm():CMsg(e_Gcc, e_ConferenceEjectUserConfirm)
    {
    }

    // 会议ID
    u32 m_nConferenceID;
    // 被挂断节点的NodeID
    TUserId m_nEjectedNodeID;
    // 操作结果
    EjectUserResult m_enumResult;
};


/*====================================================================
类名 ： CConferenceAnnouncePresenceRequest
描述 ： 宣布节点请求原语
====================================================================*/
class CConferenceAnnouncePresenceRequest : public CMsg
{
public:
    CConferenceAnnouncePresenceRequest():CMsg(e_Gcc, e_ConferenceAnnouncePresenceRequest)
    {
    }

    // 会议ID
    u32 m_nConferenceID;
    // 节点类型
    NodeType m_enumNodeType;
    // 节点属性
    NodeCategory m_enumNodeCategory;
    // 节点名称
    PString m_strNodeName;
    // 用户数据
    PBYTEArray m_cUserData;
};


/*====================================================================
类名 ： CConferenceAnnouncePresenceConfirm
描述 ： 宣布节点确认原语
====================================================================*/
class CConferenceAnnouncePresenceConfirm : public CMsg
{
public:
    CConferenceAnnouncePresenceConfirm():CMsg(e_Gcc, e_ConferenceAnnouncePresenceConfirm)
    {
    }

    // 会议ID
    u32 m_nConferenceID;
    // 操作结果
    ConferenceResponseResult m_enumResult;
};


/*====================================================================
类名 ： CConferenceRosterReportIndication（未使用）
描述 ： 会议登记表汇报指示原语
====================================================================*/
class CConferenceRosterReportIndication : public CMsg
{
public:
    CConferenceRosterReportIndication():CMsg(e_Gcc, e_ConferenceRosterReportIndication)
    {
    }

    // 会议ID
    u32 m_nConferenceID;
    // 会议登记表
//    CConferenceRoster m_cConferenceRoster;
};


/*====================================================================
类名 ： CConferenceNodeAddIndication
描述 ： 会议中新增节点通知原语
====================================================================*/
class CConferenceNodeAddIndication : public CMsg
{
public:
    CConferenceNodeAddIndication():CMsg(e_Gcc, e_ConferenceNodeAddIndication)
    {
    }

    // 该节点的NodeId
    TUserId m_nNodeId;
    // 该节点的上级节点NodeId
    TUserId m_nAboveNodeId;
    // 该节点的来源
    PString m_strFrom;
    // 该节点的类型(位置)
    ConfNodePos m_enumNodePos;
    // 该节点的类型(MCU/MT)
    NodeType m_enumNodeType;
    // 是否被动接收的连接
    bool m_bIsPassive;
    // 该节点的名称
    PString m_strNodeName;
    // 该节点的Email
    PString m_strNodeEmail;
    // 该节点的电话
    PString m_strNodePhone;
    // 该节点的地址
    PString m_strNodeLoc;
    // 用户数据
    void*   m_pUserData;
};


/*====================================================================
类名 ： CConferenceNodeAddIndication
描述 ： 会议中新增节点通知原语
====================================================================*/
class CConferenceNodeRemoveIndication : public CMsg
{
public:
    CConferenceNodeRemoveIndication():CMsg(e_Gcc, e_ConferenceNodeRemoveIndication)
    {
    }

    // 该节点的NodeId
    TUserId m_nRemovedNodeId;
};




/*====================================================================
类名 ： CConferenceApeMaskIndication
描述 ： 指定应用掩码通知原语
====================================================================*/
class CConferenceApeMaskIndication : public CMsg
{
public:
    CConferenceApeMaskIndication():CMsg(e_Gcc, e_ConferenceApeMaskIndication)
    {
    }

    // 应用掩码
    u8 m_nApeMask;
};


/*====================================================================
类名 ： CConferenceRosterInquireRequest
描述 ： 会议登记表查询请求原语
====================================================================*/
class CConferenceRosterInquireRequest : public CMsg
{
public:
    CConferenceRosterInquireRequest():CMsg(e_Gcc, e_ConferenceRosterInquireRequest)
    {
    }

    // 会议ID
    u32 m_nConferenceID;
};


/*====================================================================
类名 ： CConferenceRosterInquireConfirm
描述 ： 会议登记表查询确认原语（未使用）
====================================================================*/
class CConferenceRosterInquireConfirm : public CMsg
{
public:
    CConferenceRosterInquireConfirm():CMsg(e_Gcc, e_ConferenceRosterInquireConfirm)
    {
    }

    // 会议ID
    u32 m_nConferenceID;
    // 会议名称
    PString m_strConferenceName;
    // 会议描述
    PString m_strConferenceDescription;
    // 会议登记表
//    CConferenceRoster m_cConferenceRoster;
    // 操作结果
    ConferenceResponseResult m_enumResult;
};


/*====================================================================
类名 ： CApplicationPermissionToEnrollIndication
描述 ： 允许应用登记指示原语
====================================================================*/
class CApplicationPermissionToEnrollIndication : public CMsg
{
public:
    CApplicationPermissionToEnrollIndication():CMsg(e_Gcc, e_ApplicationPermissionToEnrollIndication)
    {
    }

    // 会议ID
    u32 m_nConferenceID;
    // 允许（Grant）或不允许（Revoke）
    EnrollPermission m_enumFlag;
};


/*====================================================================
类名 ： CApplicationEnrollRequest
描述 ： 应用登记请求原语
====================================================================*/
class CApplicationEnrollRequest : public CMsg
{
public:
    CApplicationEnrollRequest():CMsg(e_Gcc, e_ApplicationEnrollRequest)
    {
    }

    // 会议ID
    u32 m_nConferenceID;
    // 会话（Session）ID
    SessionKey m_cSessionKey;
    // Active标志
    bool m_bActiveFlag;
    // 该APE的ApeID
    TUserId m_nApplicationUserID;
    // 是否登记（true：登记，false：反登记）
    bool m_bEnrollFlag;
    // Ape的信息
    ApplicationRecord m_cAppRecord;
    // 能力级信息
    TApeCapabilitiesList m_listApeCapabilities;
};


/*====================================================================
类名 ： CApplicationEnrollConfirm
描述 ： 应用登记确认原语
====================================================================*/
class CApplicationEnrollConfirm : public CMsg
{
public:
    CApplicationEnrollConfirm():CMsg(e_Gcc, e_ApplicationEnrollConfirm)
    {
    }

    // 会议ID
    u32 m_nConferenceID;
    // 会话（Session）ID
    SessionKey m_cSessionKey;
    // 该APE的ApeID
    TApeId m_nApeId;
    // APE所在节点的NodeID
    TUserId m_nNodeID;
    // 操作结果
    ConferenceResponseResult m_enumResult;
};


/*====================================================================
类名 ： CApplicationRosterReportIndication
描述 ： 应用登记表汇报指示原语（未使用）
====================================================================*/
class CApplicationRosterReportIndication : public CMsg
{
public:
    CApplicationRosterReportIndication():CMsg(e_Gcc, e_ApplicationRosterReportIndication)
    {
    }

    // 会议ID
    u32 m_nConferenceID;
    // 应用登记表的更新部分
//    CUpdatedApplicationRoster m_cUpdatedApplicationRoster;
};



/*====================================================================
类名 ： CConferenceApeAddIndication
描述 ： 会议中新增应用通知原语
====================================================================*/
class CConferenceApeAddIndication : public CMsg
{
public:
    CConferenceApeAddIndication():CMsg(e_Gcc, e_ConferenceApeAddIndication)
    {
    }

    // 会议ID
    u32 m_nConferenceId;
    // 该Ape的SProtocolKey
    Key m_cProtocolKey;
    // 该Ape所在的节点NodeId
    TUserId m_nNodeId;
    // 该Ape在该节点上的Entity编号
    u32 m_nEntityId;
    // 该Ape的信息
    ApplicationRecord m_cApeRec;
    // 加入该Ape的总计数
    u32 m_nParticipantsNum;
};


/*====================================================================
类名 ： CConferenceApeReplaceIndication
描述 ： 会议中修改应用通知原语
====================================================================*/
class CConferenceApeReplaceIndication : public CMsg
{
public:
    CConferenceApeReplaceIndication():CMsg(e_Gcc, e_ConferenceApeReplaceIndication)
    {
    }

    // 会议ID
    u32 m_nConferenceId;
    // 该Ape的SProtocolKey
    Key m_cProtocolKey;
    // 该Ape所在的节点NodeId
    TUserId m_nNodeId;
    // 该Ape在该节点上的Entity编号
    u32 m_nEntityId;
    // 该Ape的信息
    ApplicationRecord m_cApeRec;
    // 加入该Ape的总计数
    u32 m_nParticipantsNum;
};




/*====================================================================
类名 ： CConferenceApeRemoveIndication
描述 ： 会议中新增应用通知原语
====================================================================*/
class CConferenceApeRemoveIndication : public CMsg
{
public:
    CConferenceApeRemoveIndication():CMsg(e_Gcc, e_ConferenceApeRemoveIndication)
    {
    }
    
    Key m_cProtocolKey;
    // 该Ape所在的节点NodeId
    TUserId m_nNodeId;
    // 该Ape在该节点上的Entity编号
    u32 m_nEntityId;
    // 加入该Ape的总计数
    u32 m_nParticipantsNum;
};



/*====================================================================
类名 ： CApplicationRosterInquireRequest
描述 ： 应用登记表查询请求原语
====================================================================*/
class CApplicationRosterInquireRequest : public CMsg
{
public:
    CApplicationRosterInquireRequest():CMsg(e_Gcc, e_ApplicationRosterInquireRequest)
    {
    }

    // 会议ID
    u32 m_nConferenceID;
    // 会话（Session）ID
    SessionKey m_cSessionKey;
    // 查询参加该Ape的实例个数
    bool m_bQuireSum;
    // 查询所有给Ape的能力级
    bool m_bQuireCap;
};


/*====================================================================
类名 ： CApplicationRosterInquireConfirm
描述 ： 应用登记表查询确认原语
====================================================================*/
class CApplicationRosterInquireConfirm : public CMsg
{
public:
    CApplicationRosterInquireConfirm():CMsg(e_Gcc, e_ApplicationRosterInquireConfirm)
    {
    }

    // 会议ID
    u32 m_nConferenceID;
    // 会话（Session）ID
    SessionKey m_cSessionKey;
    // 应用登记表<Key:节点Id，能力级信息>
    std::map<TUserId, ApplicationRecord> m_mapNodeCap;
    // 参加该应用的节点个数
    s32 m_nApeCount;
    // 操作结果
    ConferenceResponseResult m_enumResult;
};


/*====================================================================
类名 ： CRegistryRegisterChannelRequest
描述 ： 注册信道请求原语
====================================================================*/
class CRegistryRegisterChannelRequest : public CMsg
{
public:
    CRegistryRegisterChannelRequest():CMsg(e_Gcc, e_RegistryRegisterChannelRequest)
    {
    }

    // 会议ID
    u32 m_nConferenceID;
    // SessionKey
    SessionKey m_cSessionKey;
    // 注册Key
    PBYTEArray m_strRegistryKey;
    // 注册value：信道ID
    TChannelId m_nChannelID;
};


/*====================================================================
类名 ： CRegistryRegisterChannelConfirm
描述 ： 注册信道确认原语
====================================================================*/
class CRegistryRegisterChannelConfirm : public CMsg
{
public:
    CRegistryRegisterChannelConfirm():CMsg(e_Gcc, e_RegistryRegisterChannelConfirm)
    {
    }

    // 注册结果
    RegistryResponse m_cRegistryResponse;
};


/*====================================================================
类名 ： CRegistryAssignTokenRequest
描述 ： 申请注册令牌请求原语
====================================================================*/
class CRegistryAssignTokenRequest : public CMsg
{
public:
    CRegistryAssignTokenRequest():CMsg(e_Gcc, e_RegistryAssignTokenRequest)
    {
    }

    // 会议ID
    u32 m_nConferenceID;
    // 申请并获取的用户ID
    TUserId m_nRequestUserId;
    // SessionKey
    SessionKey m_cSessionKey;
    // 注册Key
    PBYTEArray m_strRegistryKey;
};


/*====================================================================
类名 ： CRegistryAssignTokenConfirm
描述 ： 申请注册令牌确认原语
====================================================================*/
class CRegistryAssignTokenConfirm : public CMsg
{
public:
    CRegistryAssignTokenConfirm():CMsg(e_Gcc, e_RegistryAssignTokenConfirm)
    {
    }

    // 注册结果
    RegistryResponse m_cRegistryResponse;
};


/*====================================================================
类名 ： CRegistryRetrieveEntryRequest
描述 ： 查询注册项请求原语
====================================================================*/
class CRegistryRetrieveEntryRequest : public CMsg
{
public:
    CRegistryRetrieveEntryRequest():CMsg(e_Gcc, e_RegistryRetrieveEntryRequest)
    {
    }

    // 会议ID
    u32 m_nConferenceID;
    // 注册Key
    PBYTEArray m_strRegistryKey;
};


/*====================================================================
类名 ： CRegistryRetrieveEntryConfirm
描述 ： 查询注册项确认原语
====================================================================*/
class CRegistryRetrieveEntryConfirm : public CMsg
{
public:
    CRegistryRetrieveEntryConfirm():CMsg(e_Gcc, e_RegistryRetrieveEntryConfirm)
    {
    }

    // 注册结果
    RegistryResponse m_cRegistryResponse;
};


/*====================================================================
类名 ： CRegistryDeleteEntryRequest
描述 ： 删除注册项请求原语
====================================================================*/
class CRegistryDeleteEntryRequest : public CMsg
{
public:
    CRegistryDeleteEntryRequest():CMsg(e_Gcc, e_RegistryDeleteEntryRequest)
    {
    }

    // 会议ID
    u32 m_nConferenceID;
    // 注册Key
    PBYTEArray m_strRegistryKey;
};


/*====================================================================
类名 ： CRegistryDeleteEntryConfirm
描述 ： 删除注册项确认原语
====================================================================*/
class CRegistryDeleteEntryConfirm : public CMsg
{
public:
    CRegistryDeleteEntryConfirm():CMsg(e_Gcc, e_RegistryDeleteEntryConfirm)
    {
    }

    // 注册结果
    RegistryResponse m_cRegistryResponse;
};


/*====================================================================
类名 ： CRegistryAllocateHandleRequest
描述 ： 申请句柄请求原语
====================================================================*/
class CRegistryAllocateHandleRequest : public CMsg
{
public:
    CRegistryAllocateHandleRequest():CMsg(e_Gcc, e_RegistryAllocateHandleRequest)
    {
    }

    // 会议ID
    u32 m_nConferenceID;
    // SessionKey
    SessionKey m_cSessionKey;
    // 申请句柄的数目
    u32 m_nNumberOfHandles;
};


/*====================================================================
类名 ： CRegistryAllocateHandleConfirm
描述 ： 申请句柄确认原语
====================================================================*/
class CRegistryAllocateHandleConfirm : public CMsg
{
public:
    CRegistryAllocateHandleConfirm():CMsg(e_Gcc, e_RegistryAllocateHandleConfirm)
    {
    }

    // 申请结果
    RegistryAllocateHandleResponse m_cAllocateResposne;
};


/*====================================================================
类名 ： CAssignConductorRequest
描述 ： 设置主席请求原语
====================================================================*/
class CAssignConductorRequest : public CMsg
{
public:
    CAssignConductorRequest():CMsg(e_Gcc, e_AssignConductorRequest)
    {
    }

    // 主席的网络地址
    PString m_strConductorAddr;
    // 主席的应用掩码
    BYTE m_byteConductorApeMask;
    // 非主席的应用掩码
    BYTE m_byteNormalApeMask;
};


/*====================================================================
类名 ： CGccSetFlowControl
描述 ： 码流控制代码设置
====================================================================*/
class CGccSetFlowControl : public CMsg
{
public:
    CGccSetFlowControl():CMsg(e_Gcc, e_ConfSetFlowControl)
    {
    }

    // 设置的码流带宽，单位 Byte/s
    s32 m_nFlowBand;
};


/*====================================================================
类名 ： CGccFlowControlConfirm
描述 ： 码流控制代码设置结果
====================================================================*/
class CGccFlowControlConfirm : public CMsg
{
public:
    CGccFlowControlConfirm():CMsg(e_Gcc, e_ConfFlowControlConfirm)
    {
    }

    // 已设置的码流带宽，单位 Byte/s
    s32 m_nFlowBand;
};


#endif
