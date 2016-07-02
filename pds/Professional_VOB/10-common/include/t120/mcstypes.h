#ifndef _MCS_TYPES_H
#define _MCS_TYPES_H

#include <list>
#include <mcspdu2.h>
#include <kdvtype.h>

#define MCS_MAX_TC_CONNECTIONS      (s32)4      // 最大TC连接数
#define MCS_MAX_STATIC_CHANNELID    (s32)1000   // 静态信道ID最大值
#define MCS_MAX_CHANNELID           (s32)65535  // 信道ID最大值
#define MCS_MIN_TOKENID             (s32)1      // 令牌ID最小值 $$$$$ 16384
#define MCS_MAX_TOKENID             (s32)65535  // 令牌ID最大值

// 数据优先级
typedef enum 
{
    DpTop       = 0,
    DpHigh      = 1,
    DpMedium    = 2,
    DpLow       = 3,
    DpLastPriority
} TDataPriority;

// 令牌的状态
typedef enum 
{
    TkNotInUse          = 0,
    TkSelfGrabbed       = 1,
    TkOtherGrabbed      = 2,
    TkSelfInhibited     = 3,
    TkOtherInhibited    = 4,
    TkSelfRecipient     = 5,
    TkSelfGiving        = 6,
    TkOtherGiving       = 7
} TTokenStatus;

// SendData/UniformSend时数据段标志
typedef enum
{
    SgBegin,
    SgEnd
} TSegmentation;

// PDU中的Result位含义
typedef enum 
{
	RtSuccessFull		    =  0, // 成功
	RtDomainMerging		    =  1, // Domian 在合并中
	RtDomainNotHierarchical	=  2, // Domian 不分等级
	RtNoSuchChannel		    =  3, // 信道不存在
	RtNoSuchDomain		    =  4, // Domian 不存在
	RtNoSuchUser		    =  5, // 令牌不存在
	RtNotAdmitted		    =  6, // 不允许加入私有信道,或者不是Manager
	RtOtherUserId		    =  7,
	RtParametersUnacceptable=  8, // 参数不被接受
	RtTokenNotAvaiable	    =  9, // 令牌无效
	RtTokenNotPossessed	    = 10, // 令牌没有被持有
	RtTooManyChannels	    = 11, // 信道过多
	RtTooManyTokens		    = 12, // 令牌过多
	RtTooManyUsers		    = 13, // 用户过多
	RtUnspecifiedFailure	= 14, // 未指定的错误
	RtUserRejected		    = 15  // 用户被拒绝
} TResult;

// PDU中的Reason位含义
typedef enum 
{
    RnDomainDisconnect	= 0,
    RnProviderInitiated	= 1,
    RnTokenPurged		= 2,
    RnUserRequested		= 3,
    RnChannelPurged		= 4
} TReason;

// PDU的Tag位含义
typedef enum
{
    PduPlumbDomainIndication        =  0, // 
    PduErectDomainRequest           =  1, // 
    PduMergeChannelsRequest         =  2, // 
    PduMergeChannelsConfirm         =  3, // 
    PduPurgeChannelsIndication      =  4, // 
    PduMergeTokensRequest           =  5, // 
    PduMergeTokensConfirm           =  6, // 
    PduPurgeTokensIndication        =  7, // 
    PduDisconnectProviderUltimatum  =  8, // 
    PduRejectMCSPDUUltimatum        =  9, // 
    
    PduAttachUserRequest            = 10, // 申请用户请求
    PduAttachUserConfirm            = 11, // 申请用户确认
    PduDetachUserRequest            = 12, // 删除用户申请
    PduDetachUserIndication         = 13, // 删除用户确认

    PduChannelJoinRequest           = 14, // 加入信道请求
    PduChannelJoinConfirm           = 15, // 加入信道确认
    PduChannelLeaveRequest          = 16, // 离开信道请求
    PduChannelConveneRequest        = 17, // 召集私有信道请求
    PduChannelConveneConfirm        = 18, // 召集私有信道确认
    PduChannelDisbandRequest        = 19, // 解散私有信道请求
    PduChannelDisbandIndication     = 20, // 解散私有信道指示
    PduChannelAdmitRequest          = 21, // 允许用户加入信道请求
    PduChannelAdmitIndication       = 22, // 允许用户加入信道确认
    PduChannelExpelRequest          = 23, // 
    PduChannelExpelIndication       = 24, // 

    PduSendDataRequest              = 25, // Send方式发送数据请求
    PduSendDataIndication           = 26, // Send方式发送数据指示
    PduUniformSendDataRequest       = 27, // Uniform Send方式发送数据请求
    PduUniformSendDataIndication    = 28, // Uniform Send方式发送数据指示
    
    PduTokenGrabRequest             = 29, // 
    PduTokenGrabConfirm             = 30, // 
    PduTokenInhibitRequest          = 31, // 
    PduTokenInhibitConfirm          = 32, // 
    PduTokenGiveRequest             = 33, // 
    PduTokenGiveIndication          = 34, // 
    PduTokenGiveResponse            = 35, // 
    PduTokenGiveConfirm             = 36, // 
    PduTokenPleaseRequest           = 37, // 
    PduTokenPleaseIndication        = 38, // 
    PduTokenReleaseRequest          = 39, // 
    PduTokenReleaseConfirm          = 40, // 
    PduTokenTestRequest             = 41, // 
    PduTokenTestConfirm             = 42  // 
    
} DomainPduTypes;	

// Domain参数
typedef struct DomainParameters 
{
    DomainParameters();
    DomainParameters(int, int, int, int, int, int, int, int);

    DomainParameters &operator=(const DomainParameters &); //赋值

    bool operator==(const DomainParameters &) const; //判断Domain参数是否相同
    bool operator!=(const DomainParameters &) const; //判断Domain参数是否不同
    void Truncate(); // 根据最大/最小值限制参数
	void ToProtocol(MCS_PROTOCOL_DomainParameters &) const; // 得到协议数据
    void FromProtocol(const MCS_PROTOCOL_DomainParameters &cProtocol); // 由协议数据得到数据
    void Negotiate(const MCS_PROTOCOL_DomainParameters &cProtocol); // 根据协议数据进行能力集交换
    void Negotiate(const DomainParameters &cDomaimPara); // 根据DomainPara数据进行能力集交换


    unsigned m_maxChannelIds; //最大Channel数
    unsigned m_maxUserIds; //最大User数
    unsigned m_maxTokenIds; //最大token数
    unsigned m_numPriorities; //优先级数
    unsigned m_minThroughput; //最小Throughput
    unsigned m_maxHeight; //最大级联高度
    unsigned m_maxMCSPDUsize; //最大MCS Pdu大小
    unsigned m_protocolVersion; //协议版本
} TDomainParameters;

static DomainParameters cDefaultDomainPara(1024,1024,1024,1,0,16,4128,2);
static DomainParameters cMinDomainPara(1,1,1,1,0,1,1056,2);
static DomainParameters cMaxDomainPara(65535,65535,65535,4,0,100,8121,2);

typedef struct AvaiableDomainParameters 
{
	AvaiableDomainParameters(const DomainParameters & = cDefaultDomainPara,
        const DomainParameters & = cMinDomainPara, 
        const DomainParameters & = cMaxDomainPara);
	
	DomainParameters m_target;
	DomainParameters m_minimum;
	DomainParameters m_maximum;
} AvaiableDomainParameters;

// 服务质量（Quality of Service）（码流控制）
typedef u32 TQos;


typedef s32 TUserId;
typedef s32 TChannelId;
typedef s32 TTokenId;

class CMcsConnection;
class CMcsUserChannel;
class CMcsChannel;
class CMcsToken;

typedef std::list<CMcsConnection*> TMcsConnectionList;
typedef std::list<CMcsUserChannel*> TUserList;
typedef std::list<TUserId> TUserIdList;
typedef std::list<TChannelId> TChannelIdList;
typedef std::map<TChannelId, CMcsChannel*> TMcsChannelMap;
typedef std::map<TTokenId, CMcsToken*> TMcsTokenMap;


#endif // _MCS_TYPES_H


