/*****************************************************************************
模块名      : RadiusAgent
文件名      : radiusagent.h
相关文件    : 
文件实现功能: 公用结构及接口函数定义。
作者        : 郭忠军
版本        : V4.0  Copyright(C) 2004-2005 KDC, All rights reserved.
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2006-08-15  4.0         郭忠军      创建
******************************************************************************/
#ifndef __RADIUSAGENT_H
#define __RADIUSAGENT_H

#include "osp.h"
#include "radiusstack.h"
////////////////////////////////// 宏定义 ////////////////////////////////
#define VER_RA "KDV RadiusAgent 40.01.01.00.01.060815"
#define VENDOR_ID       (u8*)"kdc"  

#define MAX_CONF_E164_LEN           (u16)64           //会议E164号长度
#define MAX_CONF_NAME_LEN           (u16)32           //会议名称最大长度
#define MAX_RA_CLIENT_NAME_LEN      (u16)32           //客户名称最大长度
#define MAX_ACCOUNT_NUM_LEN         (u16)20           //帐号最大长度
#define MAX_ACCOUNT_PWD_LEN         (u16)16           //帐号密码最大长度

#define LEN_ACCT_SESSION_ID         16               //计费SessionID长度(定长)
#define MAX_CALLER_NUM_LEN          30               //主叫号码最大长度
#define MAX_CALLED_NUM_LEN          30               //被叫号码最大长度

//认证结果
#define RADIUS_AUTH_SUCCESS             0x00000000              //成功
#define RADIUS_AUTH_ACCT_NOT_EXIST      0x00000001              //帐号不存在
#define RADIUS_AUTH_PWD_NOT_MATCH       0x00000002              //帐号密码不匹配
#define RADIUS_AUTH_ACCT_FORBID         0x00000003              //帐号被禁用或锁定
#define RADIUS_AUTH_PACKET_INVALID      0x00000004              //认证包格式不正确
#define RADIUS_AUTH_TIME_OUT            0x000000ee              //服务器超时未响应
#define RADIUS_AUTH_DB_FAIL             0x000000ef              //数据库操作失败
#define RADIUS_AUTH_OTHER_ERRPR         0x000000ff              //其它

//计费结果
#define RADIUS_ACCT_SUCCESS             0x00000000              //成功
#define RADIUS_ACCT_LACK_AUTH_ITEM      0x00000006              //缺少需要的认证选项
#define RADIUS_ACCT_LACK_ACCT_ITEM      0x00000008              //缺少需要的计费选项
#define RADIUS_ACCT_POLICY_FAIL         0x0000000c              //策略服务器计费失败
#define RADIUS_ACCT_NUM_IN_USE          0x000000f2              //账号正在使用
#define RADIUS_ACCT_CONF_NOT_END        0x000000e0              //会议尚未结束 
#define RADIUS_ACCT_CONF_ENDED          0x000000e1              //会议已经结束 
#define RADIUS_ACCT_CONF_NOT_EXIST      0x000000ed              //会议不存在
#define RADIUS_ACCT_TIME_OUT            0x000000ee              //服务器超时未响应
#define RADIUS_ACCT_DB_FAIL             0x000000ef              //数据库操作失败

//业务类型
#define SERVICE_TYPE_ACCOUT_AUTH        0x0002              //帐号认证
#define SERVICE_TYPE_TERMIN_AUTH        0x0003              //终端号码认证
#define SERVICE_TYPE_CALL_AUTH          0x0004              //呼叫认证
#define SERVICE_TYPE_PRE_CALC           0x000a              //预算费用
#define SERVICE_TYPE_PRE_FEE            0x000b              //预扣费用
#define SERVICE_TYPE_REAL_FEE           0x000c              //实扣费用

//计费项目
#define ACCT_ITEM_P2P                   0x0007              //点对点

//计费类型
#define ACCT_TYPE_BY_TIME                0x0001             //按次收费
#define ACCT_TYPE_CALL_TRANSFOR          0x0005             //呼叫转移

//计费标志        
#define ACCT_FLAG_FREE                   0x0000             //免费
#define ACCT_FLAG_CHARGE                 0x0001             //收费
#define ACCT_FLAG_RETURN_MONEY           0x0002             //返还

//计费状态类型
#define ACCT_STATUS_TYPE_START           0x0001              //开始
#define ACCT_STATUS_TYPE_STOP            0x0002              //结束
#define ACCT_STATUS_TYPE_UPDATE          0x0003              //刷新

//会议类型
#define CONF_TYPE_APPOINT                0x0001              //预约会议
#define CONF_TYPE_CALLSET                0x0002              //主叫呼集会议
#define CONF_TYPE_P2P                    0x0005              //点对点会议
#define CONF_TYPE_THIRDPART              0x0006              //第三方调度会议

//////////////////////////////// 结构类型 //////////////////////////////

typedef u32  radAuthResult;
typedef u32  radAcctResult;

//会议名称
typedef struct tagConfName{
    u8   m_abyConfName[MAX_CONF_NAME_LEN+1];
    u16  m_wNameLen;
    
    tagConfName()
    {
        memset(m_abyConfName,0,sizeof(m_abyConfName));
        m_wNameLen = 0;
    }
}TConfName;

//客户信息（填入MCU E164号 或 H323别名）
typedef struct tagClientInfo
{
    u8 abyClientName[MAX_RA_CLIENT_NAME_LEN+1];
    u16 m_wNameLen;    

    tagClientInfo()
    {
        memset(abyClientName,0,sizeof(abyClientName));
        m_wNameLen = 0;
    }
}TClientInfo;

//帐户信息（填入Radius认证帐号）
typedef struct tagAccountInfo{
    u8   m_abyAccoutNum[MAX_ACCOUNT_NUM_LEN+1];
    u16  m_wAccountLen;
    u8   m_abyPwd[MAX_ACCOUNT_PWD_LEN+1];
    u16  m_wPwdLen;

    tagAccountInfo()
    {
        memset(m_abyAccoutNum, 0 ,sizeof(m_abyAccoutNum));
        memset(m_abyPwd,0,sizeof(m_abyPwd));
        m_wAccountLen = 0;
        m_wPwdLen = 0;
    }
}TAccountInfo;

//认证信息结构
typedef struct tagAuthInfo{
    u32 m_dwServType;         //业务类型(按新视通规范定义)
    TClientInfo m_tClient;    //客户名称/终端号码
    TAccountInfo m_tAccount;  //账号信息(账号/密码)   
     
    tagAuthInfo()
    {
        m_dwServType = SERVICE_TYPE_ACCOUT_AUTH;
    }
}TLibAuthInfo;

//认证请求响应结构
typedef struct tagAuthRspInfo{
    TClientInfo    m_tClient;
    radAuthResult  m_dwAuthResult;
    
    tagAuthRspInfo()
    {
        m_dwAuthResult = 0;
    }
    
}TLibAuthRspInfo;

//开始会议计费信息
typedef struct tagConfAcctStartInfo{
    u8  m_abyAcctNum[MAX_ACCOUNT_NUM_LEN];       //客户帐号
    u16 m_wAcctNumLen;
    u8  m_abyAcctPwd[MAX_ACCOUNT_PWD_LEN];       //帐号密码
    u16 m_wAcctPwdLen;
    u32 m_dwServiceType;                             //业务类别编号
    u8  m_abyConfID[MAX_CONF_E164_LEN];            //会议e164号
    u16 m_wConfIdLen;
    u8  m_abyConfName[MAX_CONF_NAME_LEN];        //会议名称
    u16 m_wConfNameLen;
    u8  m_abyAcctSessionId[LEN_ACCT_SESSION_ID];   //计费sessionid，由GK为各个会议分配。 

    u32  m_dwSiteRate;                   //会议码率
    u16  m_wTermNum;                     //终端数
    u32  m_dwStartTime;                  //开始时间 UTC
    u32  m_dwStopTime;                   //结束时间 UTC
    u16  m_wMultiVoiceNum;               //混音数
    u16  m_wMultiPicNum;                  //多画面数
    u16  m_wRateAdaptNum;                //码率适配数
    u16  m_wCascade;                     //是否使用级联
    u16  m_wEncrypt;                     //加密模式
    u16  m_wDualVideo;                   //是否使用双流
    u16  m_wDataConf;                    //是否召开数据会议
    u16  m_wStream;                      //流服务模式
    u32  m_dwAcctFlag;                   //计费标志
    u32  m_dwAcctStaus;                  //计费状态(开始1或结束2)
    u32  m_dwConfType;                   //会议类型

    tagConfAcctStartInfo()
    {
        memset(m_abyAcctNum,0,sizeof(m_abyAcctNum));
        m_wAcctNumLen = 0;
        memset(m_abyAcctPwd,0,sizeof(m_abyAcctNum));
        m_wAcctPwdLen = 0;
        m_dwServiceType = SERVICE_TYPE_REAL_FEE;
        memset(m_abyConfID,0,sizeof(m_abyConfID));
        m_wConfIdLen = 0;
        memset(m_abyConfName,0,sizeof(m_abyConfName));
        m_wConfNameLen = 0;
        memset(m_abyAcctSessionId,0,sizeof(m_abyAcctSessionId));
        m_dwSiteRate = 0;
        m_wTermNum = 0;
        m_dwStartTime = 0;
        m_dwStopTime = 0;
        m_wMultiVoiceNum = 0;
        m_wMultiPicNum = 0;
        m_wRateAdaptNum = 0;
        m_wCascade = 0;
        m_wEncrypt = 0;
        m_wDualVideo = 0;
        m_wDataConf = 0;
        m_wStream = 0;      
        m_dwAcctFlag = ACCT_FLAG_CHARGE;
        m_dwAcctStaus = ACCT_STATUS_TYPE_START;  
        m_dwConfType = CONF_TYPE_CALLSET;
    }

    BOOL SetConfName(u8* pbyInBuf, u16 wLen)
    {
        if((NULL == pbyInBuf)||(wLen > MAX_CONF_NAME_LEN))
        {
            return FALSE;
        }
        memcpy(m_abyConfName,pbyInBuf,wLen);
        m_wConfNameLen = wLen;
        return TRUE;
    }

    BOOL SetConfE164Id(u8* pbyInBuf, u16 wLen)
    {
        if((NULL == pbyInBuf)||(wLen > MAX_CONF_E164_LEN ))
        {
            return FALSE;
        }
        memcpy(m_abyConfID,pbyInBuf,wLen);
        m_wConfIdLen = wLen;
        return TRUE;
    }
    
    BOOL SetAcctNum(u8* pbyInBuf, u16 wLen)
    {
        if((NULL == pbyInBuf)||(wLen > MAX_ACCOUNT_NUM_LEN))
        {
            return FALSE;
        }
        memcpy(m_abyAcctNum,pbyInBuf,wLen);
        m_wAcctNumLen = wLen;
        return TRUE;
    }

    BOOL SetAcctPwd(u8* pbyInBuf, u16 wLen)
    {
        if((NULL == pbyInBuf)||(wLen > MAX_ACCOUNT_PWD_LEN))
        {
            return FALSE;
        }
        memcpy(m_abyAcctPwd,pbyInBuf,wLen);
        m_wAcctPwdLen = wLen;
        return TRUE;
    }

    BOOL SetAcctSessionId(u8* pbyInBuf, u16 wLen)
    {
        if((NULL == pbyInBuf)||(wLen > LEN_ACCT_SESSION_ID))
        {
            return FALSE;
        }
        memcpy(m_abyAcctSessionId,pbyInBuf,wLen);        
        return TRUE;
    }


    u16 GetAcctNumLen()
    {
        return m_wAcctNumLen;
    }
    
    u16 GetAcctPwdLen()
    {
        return m_wAcctPwdLen;
    }

    u16 GetConfIdLen()
    {
        return m_wConfIdLen;
    }

    u16 GetConfNameLen()
    {
        return m_wConfNameLen;
    }

    u16 GetAcctSessionId()
    {
        return LEN_ACCT_SESSION_ID;             
    }

}TLibConfAcctStartInfo;

//停止会议计费信息
typedef struct tagConfAcctStopInfo
{
    u8  m_abyAcctNum[MAX_ACCOUNT_NUM_LEN];       //客户帐号
    u16 m_wAcctNumLen;
    u8  m_abyAcctPwd[MAX_ACCOUNT_PWD_LEN];       //帐号密码     
    u16 m_wAcctPwdLen;
    u8  m_abyAcctSessionId[LEN_ACCT_SESSION_ID + 1];   //计费sessionid，由GK为各个会议分配。 

    tagConfAcctStopInfo()
    {
        memset(m_abyAcctNum,0,sizeof(m_abyAcctNum));
        m_wAcctNumLen = 0;
        memset(m_abyAcctPwd,0,sizeof(m_abyAcctNum));
        m_wAcctPwdLen = 0;
        memset(m_abyAcctSessionId,0,sizeof(m_abyAcctSessionId));
    }

    BOOL SetAcctNum(u8* pbyInBuf, u16 wLen)
    {
        if((NULL == pbyInBuf)||(wLen > MAX_ACCOUNT_NUM_LEN))
        {
            return FALSE;
        }
        memcpy(m_abyAcctNum,pbyInBuf,wLen);
        m_wAcctNumLen = wLen;
        return TRUE;
    }

    BOOL SetAcctPwd(u8* pbyInBuf, u16 wLen)
    {
        if((NULL == pbyInBuf)||(wLen > MAX_ACCOUNT_PWD_LEN))
        {
            return FALSE;
        }
        memcpy(m_abyAcctPwd,pbyInBuf,wLen);
        m_wAcctPwdLen = wLen;
        return TRUE;
    }

    BOOL SetAcctSessionId(u8* pbyInBuf, u16 wLen)
    {
        if((NULL == pbyInBuf)||(wLen > LEN_ACCT_SESSION_ID))
        {
            return FALSE;
        }
        memcpy(m_abyAcctSessionId,pbyInBuf,wLen);
        return TRUE;
    }

    u16 GetAcctNumLen()
    {
        return m_wAcctNumLen;
    }
    
    u16 GetAcctPwdLen()
    {
        return m_wAcctPwdLen;
    }

    u16 GetAcctSessionId()
    {
        return LEN_ACCT_SESSION_ID;             
    }
    
}TLibConfAcctStopInfo;


//刷新会议计费信息(保持计费进行)
typedef struct tagConfAcctUpdateInfo
{
    u8  m_abyAcctNum[MAX_ACCOUNT_NUM_LEN];       //客户帐号
    u16 m_wAcctNumLen;
    u8  m_abyAcctSessionId[LEN_ACCT_SESSION_ID];   //计费sessionid，由GK为各个会议分配。
    u16 m_wSessIdLen;
    
    tagConfAcctUpdateInfo()
    {
        memset(m_abyAcctNum,0,sizeof(m_abyAcctNum));
        m_wAcctNumLen = 0;
        memset(m_abyAcctSessionId,0,sizeof(m_abyAcctSessionId));
        m_wSessIdLen = 0;
    }
    
    BOOL SetAcctNum(u8* pbyInBuf, u16 wLen)
    {
        memset(m_abyAcctNum,0,sizeof(m_abyAcctNum));
        m_wAcctNumLen = 0;
        if((NULL == pbyInBuf)||(wLen > MAX_ACCOUNT_NUM_LEN))
        {
            return FALSE;
        }
        memcpy(m_abyAcctNum,pbyInBuf,wLen);
        m_wAcctNumLen = wLen;
        return TRUE;
    }    
   
    BOOL SetAcctSessionId(u8* pbyInBuf, u16 wLen)
    {
        memset(m_abyAcctSessionId,0,sizeof(m_abyAcctSessionId));
        m_wSessIdLen = 0;
        
        if((NULL == pbyInBuf)||(wLen > LEN_ACCT_SESSION_ID))
        {
            return FALSE;
        }
        memcpy(m_abyAcctSessionId,pbyInBuf,wLen);
        m_wSessIdLen = LEN_ACCT_SESSION_ID;
        return TRUE;
    }
    
    u16 GetAcctNumLen()
    {
        return m_wAcctNumLen;
    }
    
    u16 GetSessionIdLen()
    {
        return m_wSessIdLen;
    }    
    
}TLibConfAcctUpdateInfo;

//呼叫计费信息
typedef struct tagCallAcctInfo{
    u32 m_dwServiceType;                            //业务类别编号
    u32 m_dwAcctStatus;                             //计费状态(开始1/结束2)
    u8  m_abyUserName[MAX_RA_CLIENT_NAME_LEN + 1];         //用户名/终端号码
    u16 m_wUserNameLen;                              
    u8  m_abyAcctSessId[LEN_ACCT_SESSION_ID +1];   //一次计费ID
    u16  m_wSessIdLen;
    u8  m_abyMulSId[LEN_ACCT_SESSION_ID + 1];       //会议计费Id
    u16 m_wMulSIdLen;
    u8  m_abyCallerNum[MAX_CALLER_NUM_LEN + 1];         //主叫号码
    u16 m_wCallerLen;
    u8  m_abyCalledNum[MAX_CALLED_NUM_LEN + 1];         //被叫号码
    u16 m_wCalledLen;
    u32 m_dwStartTime;                              //计费开始时间
    u32 m_dwStopTime;                               //计费结束时间
    u32 m_dwAcctItem;                               //计费项目
    u32 m_dwAcctType;                               //计费类型
    u32 m_dwAcctFlay;                               //计费标志

    tagCallAcctInfo()
    {
        memset(m_abyUserName,0,sizeof(m_abyUserName));
        m_wUserNameLen = 0;
        m_dwServiceType = SERVICE_TYPE_REAL_FEE;
        memset(m_abyAcctSessId,0,sizeof(m_abyAcctSessId));
        m_wSessIdLen = 0;
        memset(m_abyMulSId,0,sizeof(m_abyMulSId));
        m_wMulSIdLen = 0;
        memset(m_abyCallerNum,0,sizeof(m_abyCallerNum));
        m_wCallerLen = 0;
        memset(m_abyCalledNum,0,sizeof(m_abyCalledNum));
        m_wCalledLen = 0;
        m_dwStartTime = 0;
        m_dwStopTime = 0;
        m_dwAcctItem = ACCT_ITEM_P2P;
        m_dwAcctType = ACCT_TYPE_BY_TIME ;
        m_dwAcctFlay = ACCT_FLAG_CHARGE;
        m_dwAcctStatus = ACCT_STATUS_TYPE_START;
    }

    BOOL SetUserName(u8* pbyBuf, u16 wBufLen)
    {
        if((NULL == pbyBuf)||(wBufLen > MAX_RA_CLIENT_NAME_LEN))
        {
            return FALSE;
        }
        memcpy(m_abyUserName,pbyBuf,wBufLen);
        m_wUserNameLen = wBufLen;
        return TRUE;
    }

    BOOL SetAcctSessionID(u8* pbyBuf, u16 wBufLen)
    {
        if((NULL == pbyBuf)||(wBufLen > LEN_ACCT_SESSION_ID))
        {
            return FALSE;
        }
        memcpy(m_abyAcctSessId,pbyBuf,wBufLen);
        m_wSessIdLen = wBufLen;
        return TRUE;
    }

    BOOL SetAcctMulSessionID(u8* pbyBuf, u16 wBufLen)
    {
        if((NULL == pbyBuf)||(wBufLen > LEN_ACCT_SESSION_ID))
        {
            return FALSE;
        }
        memcpy(m_abyMulSId,pbyBuf,wBufLen);
        m_wMulSIdLen = wBufLen;
        return TRUE;
    }

}TLibCallAcctInfo;

//计费请求响应结构
typedef struct tagAcctRspInfo{
    TClientInfo    m_tClientInfo;
    radAcctResult  m_dwAcctResult;   //计费请求结果
    u32 m_dwAcctType;                //请求类型:1-开始 2-结束 3-更新
    
    tagAcctRspInfo()
    {
        m_dwAcctResult = 0;
        m_dwAcctType = 1;
    }
}TLibAcctRspInfo;


//回调函数结构定义
typedef void (*CBAuthResponse)(RadSessionHandle handle, TLibAuthRspInfo *ptAuthResult);          
typedef void (*CBAcctStartResponse)(RadSessionHandle handle,TLibAcctRspInfo *ptAcctStartRsp);
typedef void (*CBAcctStopResponse)(RadSessionHandle handle,TLibAcctRspInfo *ptAcctStopRsp);
typedef void (*CBAcctUpdateResponse)(RadSessionHandle handle,TLibAcctRspInfo *ptAcctUpdateRsp);

typedef struct tagRadiusAgentEvent{
    CBAuthResponse         m_pfCBAuthRsp;                //认证响应
    CBAcctStartResponse    m_pfCBAcctStartRsp;           //计费开始响应
    CBAcctStopResponse     m_pfCBAcctStopRsp;            //计费结束响应
    CBAcctUpdateResponse   m_pfCBAcctUpdateRsp;          //计费更新响应

    tagRadiusAgentEvent()
    {
        m_pfCBAuthRsp = NULL;
        m_pfCBAcctStartRsp = NULL;
        m_pfCBAcctStopRsp = NULL;
        m_pfCBAcctUpdateRsp = NULL;
    }
}TRadiusAgentEvent;


//////////////////////////////// 接口函数//////////////////////////
/*====================================================================
函数名： InitRadiusStack
功能：   初始化Radius协议栈
算法实现：（可选项）
引用全局变量：
输入参数说明：
           dwLocalIP    本地IP
           dwServerIP   认证服务器IP
           pSecret      Radius共享密钥
           wSecretLen   密钥长度
           wAuthPort     认证端口
           wAcctPort     计费端口
返回值说明：成功返回TRUE；失败返回FALSE
====================================================================*/
BOOL InitRadiusAgent(u32 dwLocalIP,u32 dwServerIP, u8 *pSecret = NULL, u8 bySecretLen = 0, u16 wAuthPort = 1812, u16 wAcctPort = 1813);


/*====================================================================
函数名：  SetAgentCallBack
功能：    设置回调函数
算法实现：（可选项）
引用全局变量：
输入参数说明：
          ptAgentEvent 回调事件指针
返回值说明：
====================================================================*/
void SetAgentCallBack(TRadiusAgentEvent *ptAgentEvent);

/*====================================================================
函数名：  StartRadiusAgent
功能：    启动RadiusAgent
算法实现：（可选项）
引用全局变量：
输入参数说明：
          
返回值说明：
====================================================================*/
void StartRadiusAgent();


/*====================================================================
函数名： SendAuthRequest
功能：   发送认证请求
算法实现：（可选项）
引用全局变量：
输入参数说明：
          tAuthInfo   认证信息
返回值说明：发送成功返回TRUE；失败返回FALSE
====================================================================*/
BOOL SendAuthRequest(TLibAuthInfo *ptAuthInfo,
                     u8* pbyUserData = NULL, u16 wDateLen = 0);

/*====================================================================
函数名： SendConfAcctStartRequest
功能：   发送开始会议计费请求
算法实现：（可选项）
引用全局变量：
输入参数说明：
        tAcctStartInfo  会议计费信息
返回值说明：发送成功返回TRUE；失败返回FALSE
====================================================================*/
BOOL SendConfAcctStartRequest(TLibConfAcctStartInfo *ptAcctStartInfo,
                              u8* pbyUserData = NULL, u16 wDateLen = 0);


/*====================================================================
函数名： SendConfAcctStopRequest
功能：   停止会议计费请求
算法实现：（可选项）
引用全局变量：
输入参数说明：
          tAcctStopInfo  计费停止信息
返回值说明：发送成功返回TRUE；失败返回FALSE
====================================================================*/
BOOL SendConfAcctStopRequest(TLibConfAcctStopInfo *ptAcctStopInfo,
                             u8* pbyUserData = NULL, u16 wDateLen = 0);

/*====================================================================
函数名： SendConfAcctUpdateRequest
功能：   刷新会议计费请求
算法实现：（可选项）
引用全局变量：
输入参数说明：
          tAcctUpdateInfo  计费停止信息
返回值说明：发送成功返回TRUE；失败返回FALSE
====================================================================*/
BOOL SendConfAcctUpdateRequest(TLibConfAcctUpdateInfo *ptAcctUpdateInfo,
                             u8* pbyUserData = NULL, u16 wDateLen = 0);

/*====================================================================
函数名： GetUserData
功能：   从radius session中读出暂存的用户数据
算法实现：（可选项）
引用全局变量：
输出参数说明：
          
返回值说明：发送成功返回TRUE；失败返回FALSE
====================================================================*/
BOOL GetUserData(RadSessionHandle hSession, u8* pbyBuf, u16 wInLen, u16 &wOutLen);

/*====================================================================
函数名： SendCallAcctStartRequest
功能：   发送开始呼叫计费请求
算法实现：（可选项）
引用全局变量：
输入参数说明：
        tCallStartInfo  会议计费信息
返回值说明：发送成功返回TRUE；失败返回FALSE
====================================================================*/
BOOL SendCallAcctStartRequest(TLibCallAcctInfo *ptCallStartInfo);


/*====================================================================
函数名： SendCallAcctStoptRequest
功能：   发送结束呼叫计费请求
算法实现：（可选项）
引用全局变量：
输入参数说明：
        tCallStopInfo  会议计费信息
返回值说明：发送成功返回TRUE；失败返回FALSE
====================================================================*/
BOOL SendCallAcctStoptRequest(TLibCallAcctInfo *ptCallStopInfo);


/*====================================================================
函数名： 关闭Radius Session
功能：
算法实现：（可选项）
引用全局变量：
输入参数说明：
返回值说明：成功返回TRUE；失败返回FALSE
====================================================================*/
BOOL32 CloseSession(RadSessionHandle hSession);


/*====================================================================
函数名： 停止RadiusAgent
功能：
算法实现：（可选项）
引用全局变量：
输入参数说明：
返回值说明：成功返回TRUE；失败返回FALSE
====================================================================*/
BOOL StopRadiusAgent();








 


#endif //__RADIUSAGENT_H
