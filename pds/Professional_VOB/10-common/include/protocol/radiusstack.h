/*****************************************************************************
模块名      : radiusstack
文件名      : radiusstack.h
相关文件    : radiusstack.cpp
文件实现功能: 给上层应用提供API函数和回调函数原型
作者        : 
版本        : V4.0  Copyright(C) 2006 KDC, All rights reserved.
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2006.11.15  4.0         郭忠军      radius server 所需要的接口定义
******************************************************************************/

#ifndef _RADIUSSTACK_H_
#define _RADIUSSTACK_H_

#include "osp.h"


////////////////////////////////调试打印函数////////////////////////////////
//打印kdvradius帮助信息
API void kdvradiushelp();
//功能:打印kdvradius版本信息
API void kdvradiusver();
//功能：设置日志级别
API void setradiuslog(u8 level);
//打印协议栈配置
API void showradius();
//打印Session表
API void showradiustable();
//打印Session消息
API void showradiusmessage(s32 i);


////////////////////////////////宏定义/////////////////////////////////////
#define VER_KDVRADIUS "40.10.01.00.16.101108"

#define SERVERIP    inet_addr("127.0.0.1")
#define RADIUS_ACCESS_PORT	1812
#define RADIUS_ACCT_PORT	1813

#define RADIUS_ERROR 0
#define RADIUS_OK	 1
#define RADIUS_FAIL  -1

#define RAD_TRANSACTION_MAX 100
#define RAD_MESSAGE_POOL_SIZE RAD_TRANSACTION_MAX * 2

#define RAD_TRANS_TIMEOUT_LEN 15000 //传输第一次超时的时间长度
#define RAD_OPERATE_MSG_SAFEGUARD_TIME  10000  //操作应答消息的保护定时器，防止消息体被吊死
#define RAD_REQ_RETRY_MAX               2   //重发的次数，即=总共发送次数-1

#define SECRET_DATA_LEN_MAX 128


#ifndef DECLARE_OPAQUE
#define DECLARE_OPAQUE(name)    typedef struct { int unused; } name##__ ; \
	typedef const name##__ * name; \
typedef name*  LP##name
#endif


////////////////////////////////属性定义///////////////////////////////////

#define RAD_ATTR_NULL 0 //错误的属性
#define RAD_ATTR_USER_NAME 1 //User-Name(String)
#define RAD_ATTR_USER_PASSWORD 2 //User-Password(String)
#define RAD_ATTR_CHAP_PASSWORD 3 //CHAP-Password(1 octet + String)
#define RAD_ATTR_NAS_IP_ADDRESS 4 //NAS-IP-Address(Address)
#define RAD_ATTR_NAS_PORT 5 //NAS-Port(Value)
#define RAD_ATTR_SERVICE_TYPE 6 //Service-Type(Value)
#define RAD_ATTR_FRAMED_PROTOCOL 7 //Framed-Protocol(Value)
#define RAD_ATTR_FRAMED_IP_ADDRESS 8 //Framed-IP-Address(Address)
#define RAD_ATTR_FRAMED_IP_NETMASK 9 //Framed-IP-Netmask(Address)
#define RAD_ATTR_FRAMED_ROUTING 10 //Framed-Routing(Value)
#define RAD_ATTR_FILTER_ID 11 //Filter-Id(Text)
#define RAD_ATTR_FRAMED_MTU 12 //Framed-MTU(Value)
#define RAD_ATTR_FRAMED_COMPRESSION 13 //Framed-Compression(Value)
#define RAD_ATTR_LOGIN_IP_HOST 14 //Login-IP-Host(Address)
#define RAD_ATTR_LOGIN_SERVICE 15 //Login-Service(Value)
#define RAD_ATTR_LOGIN_TCP_PORT 16 //Login-TCP-Port(Value)

#define RAD_ATTR_REPLY_MESSAGE 18 //Reply-Message(Text)
#define RAD_ATTR_CALLBACK_NUMBER 19 //Callback-Number(String)
#define RAD_ATTR_CALLBACK_ID 20 //Callback-Id(String)

#define RAD_ATTR_FRAMED_ROUTE 22 //Framed-Route(Text)
#define RAD_ATTR_FRAMED_IPX_NETWORK 23 //Framed-IPX-Network(Value)
#define RAD_ATTR_STATE 24 //State(String)
#define RAD_ATTR_CLASS 25 //Class(String)
#define RAD_ATTR_VENDOR_SPECIFIC 26 //Vendor-Specific(...)
#define RAD_ATTR_SESSION_TIMEOUT 27 //Session-Timeout(Value)
#define RAD_ATTR_IDLE_TIMEOUT 28 //Idle-Timeout(Value)
#define RAD_ATTR_TERMINATION_ACTION 29 //Termination-Action(Value)
#define RAD_ATTR_CALLED_STATION_ID 30 //Called-Station-Id(String)
#define RAD_ATTR_CALLING_STATION_ID 31 //Calling-Station-Id(String)
#define RAD_ATTR_NAS_IDENTIFIER 32 //NAS-Identifier(String)
#define RAD_ATTR_PROXY_STATE 33 //Proxy-State(String)
#define RAD_ATTR_LOGIN_LAT_SERVICE 34 //Login-LAT-Service(String)
#define RAD_ATTR_LOGIN_LAT_NODE 35 //Login-LAT-Node(String)
#define RAD_ATTR_LOGIN_LAT_GROUP 36 //Login-LAT-Group(String)
#define RAD_ATTR_FRAMED_APPLETALK_LINK 37 //Framed-AppleTalk-Link(Value)
#define RAD_ATTR_FRAMED_APPLETALK_NETWORK 38 //Framed-AppleTalk-Network(Value)
#define RAD_ATTR_FRAMED_APPLETALK_ZONE 39 //Framed-AppleTalk-Zone(String)

#define RAD_ATTR_ACCT_STATUS_TYPE 40 //Acct-Status-Type(Value)
#define RAD_ATTR_ACCT_DELAY_TIME 41 //Acct-Delay-Time(Value)
#define RAD_ATTR_ACCT_INPUT_OCTETS 42 //Acct-Input-Octets(Value)
#define RAD_ATTR_ACCT_OUTPUT_OCTETS 43 //Acct-Output-Octets(Value)
#define RAD_ATTR_ACCT_SESSION_ID 44 //Acct-Session-Id(String)
#define RAD_ATTR_ACCT_AUTHENTIC 45 //Acct-Authentic(Value)
#define RAD_ATTR_ACCT_SESSION_TIME 46 //Acct-Session-Time(Value)
#define RAD_ATTR_ACCT_INPUT_PACKETS 47 //Acct-Input-Packets(Value)
#define RAD_ATTR_ACCT_OUTPUT_PACKETS 48 //Acct-Output-Packets(Value)
#define RAD_ATTR_ACCT_TERMINATE_CAUSE 49 //Acct-Terminate-Cause(Value)
#define RAD_ATTR_ACCT_MULTI_SESSION_ID 50 //Acct-Multi-Session-Id(String)
#define RAD_ATTR_ACCT_LINK_COUNT 51 //Acct-Link-Count(Value)

#define RAD_ATTR_CHAP_CHALLENGE 60 //CHAP-Challenge(String)
#define RAD_ATTR_NAS_PORT_TYPE 61 //NAS-Port-Type(Value)
#define RAD_ATTR_PORT_LIMIT 62 //Port-Limit(Value)
#define RAD_ATTR_LOGIN_LAT_PORT 63 //Login-LAT-Port(String)

#define RAD_ATTR_MESSAGE_AUTHENTICATOR 80 //Message-Authenticator(String)

#define RAD_ATTR_USER_CREDIT_AMOUNT 224 //User-credit-amount
#define RAD_ATTR_CALL_TYPE          225 //account-type

typedef u8 RadAttributeType;

////////////////////////////扩展属性定义(按“新视通”规范)/////////////////////////
#define RAD_ATTR_SPEC_USER_TYPE           160
#define RAD_ATTR_SPEC_SITE_RATE           164
#define RAD_ATTR_SPEC_CONFENCE_TYPE       165
#define RAD_ATTR_SPEC_RESOURCE_NUM        166
#define RAD_ATTR_SPEC_ACCT_BEGIN_TIME     169
#define RAD_ATTR_SPEC_ACCT_END_TIME       170
#define RAD_ATTR_SPEC_ACCT_ITEM           171
#define RAD_ATTR_SPEC_ACCT_TYPE           172
#define RAD_ATTR_SPEC_ACCT_FEE            173
#define RAD_ATTR_SPEC_ACCT_FLAG           175
#define RAD_ATTR_SPEC_DUAL_VIDEO          211
#define RAD_ATTR_SPEC_DATA_RATE           212
#define RAD_ATTR_SPEC_FLOW_MEDIA_TYPE     214   
#define RAD_ATTR_SPEC_VERSION             254
#define RAD_ATTR_SPEC_PRODUCTI_ID         255

#define RAD_ATTR_SPEC_CONFENCE_ID         180
#define RAD_ATTR_SPEC_TERMINAL_NUM        181
#define RAD_ATTR_SPEC_MULTIPIC_NUM        182
#define RAD_ATTR_SPEC_RATEADAPTER_NUM     183
#define RAD_ATTR_SPEC_MULTIVOICE_NUM      184
#define RAD_ATTR_SPEC_DATACONFENCE        185
#define RAD_ATTR_SPEC_ENCRYPT_USED        186
#define RAD_ATTR_SPEC_CASCADE_USED        187
#define RAD_ATTR_SPEC_CONFENCE_NAME       188
#define RAD_ATTR_SPEC_MULTICAST_USED      189  


//////////////////////////////////////枚举定义/////////////////////////////////////
//Radius Msg-Type
typedef enum
{
	RADMSG_UNKOWN = 0,
	RADMSG_ACCESS_REQUEST = 1, //Access-Request
	RADMSG_ACCESS_ACCEPT  = 2, //Access-Accept
	RADMSG_ACCESS_REJECT  = 3, //Access-Reject
	RADMSG_ACCOUNTING_REQUEST = 4, //Accounting-Request
	RADMSG_ACCOUNTING_RESPONSE = 5, //Accounting-Response
	RADMSG_ACCESS_CHALLENGE = 11 //Access-Challenge
}RadMsgType;

//Service-Type
typedef enum
{
    RAD_ST_LOGIN=1,
    RAD_ST_FRAMED,
    RAD_ST_CALLBACK_LOGIN,
    RAD_ST_CALLBACK_FRAMED,
    RAD_ST_OUTBOUND,
    RAD_ST_ADMINISTRATIVE,
    RAD_ST_NAS_PROMPT,
    RAD_ST_AUTHENTICATE_ONLY,
    RAD_ST_CALLBACK_NAS_PROMPT,
    RAD_ST_CALL_CHECK=10,
    RAD_ST_CALLBACK_ADMINISTRATIVE,
}RadServiceType;


//Acct-Terminate-Cause
typedef enum
{
    RAD_ATC_USER_REQUEST=1,
    RAD_ATC_LOST_CARRIER,
    RAD_ATC_LOST_SERVICE,
    RAD_ATC_IDLE_TIMEOUT,
    RAD_ATC_SESSION_TIMEOUT,
    RAD_ATC_ADMIN_RESET,
    RAD_ATC_ADMIN_REBOOT,
    RAD_ATC_PORT_ERROR,
    RAD_ATC_NAS_ERROR,
    RAD_ATC_NAS_REQUEST=10,
    RAD_ATC_NAS_REBOOT,
    RAD_ATC_PORT_UNNEEDED,
    RAD_ATC_PORT_PREEMPTED,
    RAD_ATC_PORT_SUSPENDED,
    RAD_ATC_SERVICE_UNAVAILABLE,
    RAD_ATC_CALLBACK,
    RAD_ATC_USER_ERROR,
    RAD_ATC_HOST_REQUEST,
}RadAcctTerminateCause;

//NAS-Port-Type
typedef enum
{
    RAD_NPT_ASYNC=0,
    RAD_NPT_SYNC,
    RAD_NPT_ISDN_SYNC,
    RAD_NPT_ISDN_ASYNC_V120,
    RAD_NPT_ISDN_ASYNC_V110,
    RAD_NPT_VIRTUAL,
    RAD_NPT_PIAFS,
    RAD_NPT_HDLC,// CLEAR CHANNEL,
    RAD_NPT_X25,
    RAD_NPT_X75,
    RAD_NPT_G3_FAX = 10,
    RAD_NPT_SDSL,// SYMMETRIC DSL,
    RAD_NPT_ADSL_CAP, //- ASYMMETRIC DSL, CARRIERLESS AMPLITUDE PHASE,MODULATION
    RAD_NPT_ADSL_DMT,//- ASYMMETRIC DSL, DISCRETE MULTI-TONE
    RAD_NPT_IDSL ,//- ISDN DIGITAL SUBSCRIBER LINE
    RAD_NPT_ETHERNET,
    RAD_NPT_XDSL,// - DIGITAL SUBSCRIBER LINE OF UNKNOWN TYPE
    RAD_NPT_CABLE,
    RAD_NPT_WIRELESS_OTHER,
    RAD_NPT_WIRELESS_IEEE802_11 =19,
}RadNASPortType;

//Login-Service-Type
typedef enum
{
	RAD_LST_Telnet = 0 ,
	RAD_LST_Rlogin = 1 ,
	RAD_LST_TCP_Clear = 2,
	RAD_LST_PortMaster = 3, //(proprietary)
	RAD_LST_LAT = 4,
	RAD_LST_X25_PAD = 5,
	RAD_LST_X25_T3POS = 6,
	RAD_LST_TCP_Clear_Quiet = 8 //(suppresses any NAS-generated connect string)
}RadLoginServiceType;

//Acct-Status-Type
typedef enum
{
    RAD_AST_Start=1,
    RAD_AST_Stop=2,
    RAD_AST_Interim_Update=3,
    RAD_AST_Accounting_On=4,
    RAD_AST_Accounting_Off=5,
       //...9-14 Reserved for Tunnel Accounting  //留作隧道计费
    RAD_AST_Reserved_for_Failed=15,
}RadAcctStatusType;

//Acct-Authentic  授权类型
//必须有过授权的用户才能生成记帐包
typedef enum
{
    RAD_AA_RADIUS=1,
    RAD_AA_Local,
    RAD_AA_Remote, 
}RadAcctAuthentic;

typedef enum
{
    RAD_ACCOUNT_CALL_TYPE_TELEPHONE,   //电话计费
    RAD_ACCOUNT_CALL_TYPE_VOIP,        //卡业务计费
}RadAcctCallType;

typedef enum
{
    RAD_CALL_TYPE_TELE=1,
    RAD_CALL_TYPE_VOIP,    
}RadCallType;


typedef enum
{
    RAD_TIMER_OPT,          //接收到回复后操作时间保护
    RAD_TIMER_SEND_REQ,     //请求重发定时器
}RadTimerId;

typedef enum
{
    PT_ERROR = 0,
    PT_WARN,
    PT_NORMAL,
	PT_ALL
} PT_LEVEL;


/////////////////////////////////////结构定义/////////////////////////////////////

DECLARE_OPAQUE(RadSessionHandle);

typedef u8 (*FuncRadRcvAcctResponse)(RadSessionHandle handle);
typedef u8 (*FuncRadRcvAccAccept)(RadSessionHandle handle);
typedef u8 (*FuncRadRcvAccReject)(RadSessionHandle handle);
typedef u8 (*FuncRadAcctTimeOut)(RadSessionHandle handle);
typedef u8 (*FuncRadAccTimeOut)(RadSessionHandle handle);

//为Radius服务端增加请求消息回调接口 
typedef u8 (*FuncRadRcvAcessRequest)(RadSessionHandle hSession);
typedef u8 (*FuncRadRcvAccountRequest)(RadSessionHandle hSession);

typedef struct 
{
    FuncRadRcvAcctResponse acctResponseHook;
    FuncRadRcvAccAccept accAcceptHook;
    FuncRadRcvAccReject accRejectHook;
    FuncRadAcctTimeOut acctTimeOutHook;
    FuncRadAccTimeOut accTimeOutHook;
    FuncRadRcvAcessRequest accessRequestHook;
    FuncRadRcvAccountRequest accountRequestHook;
} RadiusEvent;


typedef u8 UINT8;
typedef u16 UINT16;
//typedef int STATUS;


void lpt(s32 nLevel, s8 * format,...);


/////////////////////////////////////消息收发函数/////////////////////////////////////

/*====================================================================
函数名：radCreateSession 
功能：	创建一个新Session
算法实现：（可选项）
引用全局变量：
输 入 参  数: MsgType - Radius消息类型
输 出 参  数: handle  - Session句柄

返回值说明：成功返回RADIUS_OK，失败返回RADIUS_ERROR.
====================================================================*/
u8 radCreateSession(RadSessionHandle *handle, RadMsgType MsgType);

/*====================================================================
函数名： radReqAddAttr
功能：   Req添加特定属性
算法实现：（可选项）
引用全局变量：
输 入 参  数: handle  - Session句柄
			  type	  - 属性类型
			  date	  - 数据
			  len	  - 长度

返回值说明：成功返回RADIUS_OK，失败返回RADIUS_ERROR.
====================================================================*/
u8 radReqAddAttr(RadSessionHandle handle, RadAttributeType type, u8 *data, u8 len);

/*====================================================================
函数名：radReqGetAttr
功能:   读取请求消息中特定属性
输入:handle	会话句柄
   	type	要读取的属性
   	len		data的长度
   	index	表示第几个该种属性，基于1
输出:data	读出的属性
返回:成功返回data的实际长度
   	 失败或者出错返回ERROR
====================================================================*/
u8 radReqGetAttr(RadSessionHandle handle, RadAttributeType type, u8 *data, u8 len, u8 index = 1);


/*====================================================================
功能:读取应答消息中特定属性
输入:handle	会话句柄
   	type		要读取的属性
   	len		data的长度
   	index	表示第几个该种属性，基于1
输出:data	读出的属性
返回:成功返回data的实际长度
   	        失败或者出错返回ERROR
====================================================================*/
u8 radRspGetAttr(RadSessionHandle handle, RadAttributeType type, u8 *data, u8 len, u8 index = 1);//index 从1开始

/*====================================================================
函数名： radReqCheckAttr
功能：   检查REQ消息中是否有特定属性
算法实现：（可选项）
引用全局变量：
输入参数说明：handle  会话句柄
			  type	  要读取的属性

返回值说明：成功返回消息中属性出现的次数
			失败或者出错返回RADIUS_ERROR
====================================================================*/
u8 radReqCheckAttr(RadSessionHandle handle, RadAttributeType type);

/*====================================================================
函数名： radReqCheckAttr
功能：   检查RSP消息中是否有特定属性
算法实现：（可选项）
引用全局变量：
输入参数说明：handle  会话句柄
			  type	  要读取的属性

返回值说明：成功返回消息中属性出现的次数
			失败或者出错返回RADIUS_ERROR
====================================================================*/
u8 radRspCheckAttr(RadSessionHandle handle, RadAttributeType type);

/*====================================================================
函数名： radSendReq
功能：   发送请求消息
算法实现：（可选项）
引用全局变量：
输入参数说明：hSessionHandle	会话句柄
              pbyUserData       用户数据缓冲区指针
              wDataLen          用户数据长度
返回值说明：成功返回RADIUS_OK，失败返回RADIUS_ERROR.
====================================================================*/
u8 radSendReq(RadSessionHandle hSessionHandle, u8 *pbyUserData, u16 wDataLen);

/*====================================================================
函数名： radEndSession
功能：关闭Session
算法实现：（可选项）
引用全局变量：
输入参数说明：handle	会话句柄
返回值说明：成功返回RADIUS_OK，失败返回RADIUS_ERROR.
====================================================================*/
u8 radEndSession(RadSessionHandle handle);

/*====================================================================
函数名： radGetUserData
功能：获得Session的用户数据
算法实现：（可选项）
引用全局变量：
输入参数说明：handle	   会话句柄
              pbyInOutBuf  数据缓冲
返回值说明：成功返回TRUE,否则返回FALSE
====================================================================*/
BOOL32 radGetUserData(RadSessionHandle handle, u8 *pbyInOutBuf, u16 wInBufLen,u16 &wOutLen);

/*====================================================================
函数名： printSessionMsg
功能：打印session消息（包括Req和Rsp）
算法实现：（可选项）
引用全局变量：
输入参数说明：handle	会话句柄
返回值说明：
====================================================================*/
void printSessionMsg(RadSessionHandle handle);

/////////////////////////////////////协议栈设置函数/////////////////////////////////////

/*====================================================================
函数名： RadStackInit
功能：RADIUS初始化
算法实现：（可选项）
引用全局变量：
输入参数说明：ip	RADIUS服务器地址

返回值说明：成功返回TRUE，失败返回FALSE.
====================================================================*/
BOOL32 RadStackInit(u32 ip = SERVERIP);

/*====================================================================
函数名： radStackSetServerIp
功能：设置ServerIP
算法实现：（可选项）
引用全局变量：
输入参数说明：ip	IP地址

返回值说明：
====================================================================*/
u8 radStackSetServerIp(u32 ip);

/*====================================================================
函数名：radStackSetServerAccessPort 
功能：  设置Server的Access端口
算法实现：（可选项）
引用全局变量：
输入参数说明：

返回值说明：
====================================================================*/
u8 radStackSetServerAccessPort(u16 port = RADIUS_ACCESS_PORT);

/*====================================================================
函数名：radStackSetServerAcctPort 
功能：  设置Server的Account端口
算法实现：（可选项）
引用全局变量：
输入参数说明：

返回值说明：
====================================================================*/
u8 radStackSetServerAcctPort(u16 port = RADIUS_ACCT_PORT);

/*====================================================================
函数名：radSetSecret 
功能：设置secret
算法实现：（可选项）
引用全局变量：
输入参数说明：

返回值说明：成功返回RADIUS_OK，失败返回RADIUS_ERROR.
====================================================================*/
u8 radSetSecret(u8 *secret, u8 bySecretLen);

/*====================================================================
函数名：radStackSetMaxRetryTimes 
功能：设置发送请求最大重试次数
算法实现：（可选项）
引用全局变量：
输入参数说明：

返回值说明：成功返回RADIUS_OK，失败返回RADIUS_ERROR.
====================================================================*/
u8 radStackSetMaxRetryTimes(u8 maxRetryTimes = RAD_REQ_RETRY_MAX);

/*====================================================================
函数名：radStackSetTimeOutInterval 
功能：设置发送超时间隔
算法实现：（可选项）
引用全局变量：
输入参数说明：

返回值说明：成功返回RADIUS_OK，失败返回RADIUS_ERROR.
====================================================================*/
u8 radStackSetTimeOutInterval(u32 TimeOutInterval = RAD_TRANS_TIMEOUT_LEN);

/*====================================================================
函数名：radStackSetCallBack
功能：  设置radStack的回调函数
算法实现：（可选项）
引用全局变量：
输入参数说明：

返回值说明：
====================================================================*/
void radStackSetCallBack(RadiusEvent* ptUserCB);

/*====================================================================
函数名：radStackStart 
功能：  启动协议栈
算法实现：（可选项）
引用全局变量：
输入参数说明：bAutoSelect	是否自动select

返回值说明：
====================================================================*/
s32 radStackStart(BOOL32 bAutoSelect);

/*====================================================================
函数名： radStackSelect
功能：协议栈Select。
如果启动时不使用AutoSelect，则需要手动周期调用该函数进行消息收取
算法实现：（可选项）
引用全局变量：
输入参数说明：

返回值说明：
====================================================================*/
s32 radStackSelect();

/*====================================================================
函数名：radStackEnd 
功能：  关闭协议栈
算法实现：（可选项）
引用全局变量：
输入参数说明：

返回值说明：
====================================================================*/
s32 radStackEnd();



/*====================================================================
函数名：radStackServerStart 
功能：  Server端启动协议栈.创建两个socket(access/accout)
算法实现：（可选项）
引用全局变量：
输入参数说明：bAutoSelect	是否自动select

返回值说明：
====================================================================*/
s32 radStackServerStart(BOOL32 bAutoSelect);

/*====================================================================
函数名：radSendAccessAccept 
功能：  发送Access-Accept消息
算法实现：（可选项）
引用全局变量：
输入参数说明：hSession  相应请求会话的句柄

返回值说明：成功返回RADIUS_OK，失败返回RADIUS_ERROR.
====================================================================*/
u8 radSendAccessAccept(RadSessionHandle hSession);


/*====================================================================
函数名：radSendAccessReject 
功能：  发送Access-Reject消息
算法实现：（可选项）
引用全局变量：
输入参数说明：hSession  相应请求会话的句柄

返回值说明：成功返回RADIUS_OK，失败返回RADIUS_ERROR.
====================================================================*/
u8 radSendAccessReject(RadSessionHandle hSession);


/*====================================================================
函数名：radSendAccountResponse
功能：  发送Account-Response消息
算法实现：（可选项）
引用全局变量：
输入参数说明：hSession  相应请求会话的句柄

返回值说明：成功返回RADIUS_OK，失败返回RADIUS_ERROR.
====================================================================*/
u8 radSendAccountResponse(RadSessionHandle hSession);


/*====================================================================
函数名：radGetUserName
功能:读取 access-request 消息中username
输入:hSession	会话句柄
   	 pNameBuf	用于保存名字的buf
     byBufLen    buf长度
     wNameLen   名字的实际长度。读取失败该值为0。
输出:pNameBuf   
     wNameLen  
返回:
====================================================================*/
//void radGetUserName(RadSessionHandle hSession, u8 *pNameBuf, u8 byBufLen, u16 &wNameLen);
BOOL32 radGetUserData(RadSessionHandle handle, u8 *pbyInOutBuf, u16 wInBufLen,u16 &wOutLen);

/*====================================================================
函数名：radGetPassword
功能:  读取 access-request 消息中经过解密后的password
输入:  hSession	    会话句柄
       pPassword    保存password的buf
       byBufLen     buf长度
输出:  pPassword	
       wPasswordLen   password实际长度. 读取失败该值为0。
返回:
====================================================================*/
void radGetPassword(RadSessionHandle hSession, u8 *pPassword, u8 byBufLen, u16 &wPasswordLen);



/*====================================================================
函数名：radGetAccountType
功能: 获得 account-request 消息中记帐类型
输入:hSession	会话句柄
输出: 
返回：记帐类型 start(1)/stop(2). 失败或出错返回RADIUS_ERROR。 
====================================================================*/
u32 radGetAccountRequestType(RadSessionHandle hSession);


/*====================================================================
函数名：radCloseHandle
功能:关闭请求句柄
输入:hSession	会话句柄

返回:成功返回 RADIUS_OK
   	        失败或者出错返回 RADIUS_ERROR 
====================================================================*/
u8 radCloseHandle(RadSessionHandle hSession);

/*====================================================================
函数名： radRspAddAttr
功能：   向Response消息中添加特定属性
算法实现：（可选项）
引用全局变量：
输 入 参  数: hSession  - Session句柄
			  tAttrType	  - 属性类型
			  pbyDate	  - 数据
			  byLen	  - 长度

返回值说明：成功返回RADIUS_OK，失败返回RADIUS_ERROR.
====================================================================*/
u8 radRspAddAttr(RadSessionHandle hSession, RadAttributeType tAttrType, u8 *pbyData, u8 byLen);


//添加扩展属性头部
u8 radReqAddSpecAttrHead(RadSessionHandle hSession, u8 *pVenderId,u8 byLen);

//添加扩展属性内容（子属性）
u8 radReqAddSpecAttrContent(RadSessionHandle hSession,RadAttributeType tSubAttrType, u8 *pVenderId,u8 byLen);

//从request消息中获取扩展属性值
u8 radReqGetSpecAttr(RadSessionHandle hSession,RadAttributeType tSubAttrType,u8* pByValueBuf, u8 byBufLen);


#endif //_RADIUSSTACK_H_
