/*****************************************************************************
模块名      : KDV RS
文件名      : radiusinterface.h
相关文件    : 
文件实现功能: Radius 会议认证/计费接口定义。供 mcu 以及 gk 调用。
作者        : 郭忠军
版本        : V4.0  Copyright(C) 2004-2005 KDC, All rights reserved.
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2006-10-17  4.0         郭忠军         创建
2007-03-13  4.0         郭忠军       增加sessionid error
******************************************************************************/
#ifndef ___RADIUSINTERFACE_H
#define ___RADIUSINTERFACE_H

#include "osp.h"
#include "kdvdef.h"
#include "eventid.h"

/*****************************长度定义**********************************************/
#define LEN_RADIUS_SESSION_ID    (u16)16          //会议计费会话id   
#define MAX_RADIUS_ACCT_NUM_LEN       (u16)20         //会议计费帐号最大长度
#define MAX_RADIUS_ACCT_PWD_LEN       (u16)16          //密码最大长度
#define MAX_CONF_NAME_LEN             (u16)32          //会议名称最大长度
#define MAX_CONF_E164_LEN             (u16)64     //E164号最大长度


/*****************************计费结果代码定义**********************************************/
#define RADIUS_INTER_ACCTING_SUCCESS          0x00000000              //计费请求成功
#define RADIUS_INTER_ACCT_NOT_EXIST           0x00000001              //帐号不存在
#define RADIUS_INTER_INVALID_PWD              0x00000002              //帐号密码不匹配
#define RADIUS_INTER_CONF_NOT_END             0x000000e0              //会议尚未结束
#define RADIUS_INTER_CONF_NOT_EXIST           0x000000ed              //会议不存在或已结束
#define RADIUS_INTER_TIME_OUT                 0x000000ee              //服务器超时未响应
#define RADIUS_INTER_DB_FAIL                  0x000000ef              //数据库操作失败
#define RADIUS_INTER_GK_NOT_SUPPORT_ACCT      0x000000f0              //GK未开启计费功能
#define RADIUS_INTER_GK_OP_FAILED             0x000000f1              //GK计费操作失败
#define RADIUS_INTER_ACCT_IN_USE              0x000000f2              //账号正在使用
#define RADIUS_INTER_SESSION_ID_NULL          0x000000f3              //计费会话ID为空
#define RADIUS_INTER_GK_INSTANCE_FULL         0x000000f4              //GK计费实例满
#define RADIUS_INTER__OTHER_ERRPR             0x000000ff              //其它错误


/*******************会议计费App号及端口定义**********************************************/
//GK端Radius应用号
#define AID_GK_RADIUS_CLIENT             (AID_GKS_BGN + 5)

#define  IN
#define  OUT
/*******************会议计费相关事件号定义**********************************************/
OSPEVENT(GKRADIUS_START, GKOSPEVENT_END - 100);

//mcu会议计费注册消息
OSPEVENT(EV_MG_CONF_ACCTING_REG, GKRADIUS_START + 1);
OSPEVENT(EV_GM_CONF_ACCTING_REG_RSP, GKRADIUS_START + 2);  

//开始会议计费请求
OSPEVENT(EV_MG_CONF_ACCTING_START, GKRADIUS_START + 3);  
//开始会议计费请求响应
OSPEVENT(EV_GM_CONF_ACCTING_START_RSP, GKRADIUS_START + 4);

//停止会议计费请求
OSPEVENT(EV_MG_CONF_ACCTING_STOP, GKRADIUS_START + 5);
//停止会议计费请求响应
OSPEVENT(EV_GM_CONF_ACCTING_STOP_RSP, GKRADIUS_START + 6);

//会议计费状态上报GK的通知，消息体 u8(计费会议个数 byConfNum) + TAcctSessionId[byConfNum]; 定时发送
OSPEVENT(EV_MG_CONF_ACCTING_STATUS_NTF, GKRADIUS_START + 7);

//会议计费状态异常通知，消息体 u8(计费状态异常会议个数 byConfNum) + TAcctSessionId[byConfNum]; 实时发送
OSPEVENT(EV_GM_CONF_ACCTING_STATUS_NTF, GKRADIUS_START + 8);


OSPEVENT(GKRADIUS_END, GKRADIUS_START + 50);



/*******************相关枚举常量定义**********************************************/
//计费请求类型
typedef enum 
{
    emRadReqType_START = (u32)1,  
    emRadReqype_STOP = (u32)2,
   emRadReqType_UPDATE = (u32)3,
}ERadiusReqType;


/*******************结构体定义**********************************************/
//计费请求会话ID结构(定长16B)
typedef struct tagAcctSessionId
{
    u8 m_abyContent[LEN_RADIUS_SESSION_ID];  //GK 分配的计费标示 
                                                   //用以区分各个会议计费请求以及匹配开始和结束请求

    tagAcctSessionId()
    {
        memset(m_abyContent,0,sizeof(m_abyContent));
    }

    BOOL SetAcctSessionId(IN u8* pBuf, IN u16 wBufLen)
    {
        if((NULL == pBuf)||(wBufLen > LEN_RADIUS_SESSION_ID))
        {
            return FALSE;
        }
        memcpy(m_abyContent,pBuf,wBufLen);
        return TRUE;
    }

    BOOL GetAcctSessionId(IN OUT u8 *pBuf, IN u16 wBufLen, OUT u16 &wOutLen)
    {
        if((wBufLen < LEN_RADIUS_SESSION_ID)||(NULL == pBuf))
        {
            wOutLen = 0;
            return FALSE;
        }
        else
        {
            wOutLen = LEN_RADIUS_SESSION_ID;
            memcpy(pBuf,m_abyContent,LEN_RADIUS_SESSION_ID);
            return TRUE;
        }
    }   

    u8* GetAcctSessionId()
    {
        return m_abyContent;
    }

    u16 GetAcctSessionIdLen()
    {
        return LEN_RADIUS_SESSION_ID;
    }
    
    //判断session id 是否为空
    BOOL IsSessionIdNull()  
    {
        u8 abyTmp[LEN_RADIUS_SESSION_ID] = {0};
        if(0 == memcmp(abyTmp,m_abyContent,LEN_RADIUS_SESSION_ID))  //全0的session id 视为null
        {
            return TRUE;
        }
        else
        {
            return FALSE;
        }  
    }
}TAcctSessionId;

//计费帐号结构
typedef struct tagConfAcctNum
{
    u8  m_abyAcctNum[MAX_RADIUS_ACCT_NUM_LEN];        
    u16 m_wAcctNumLen;
    
    tagConfAcctNum()
    {
        memset(m_abyAcctNum,0,sizeof(m_abyAcctNum));
        m_wAcctNumLen = 0;        
    }
    
    BOOL SetAcctNum(IN u8* pBuf, IN u16 wBufLen)
    {
        if((NULL == pBuf)||(wBufLen > MAX_RADIUS_ACCT_NUM_LEN))
        {
            return FALSE;
        }
        memset(m_abyAcctNum,0,sizeof(m_abyAcctNum));
        memcpy(m_abyAcctNum,pBuf,wBufLen);
        m_wAcctNumLen= htons(wBufLen);
        return TRUE;
    }

    BOOL GetAcctNum(IN OUT u8 *pBuf, IN u16 wBufLen, OUT u16 &wOutLen)
    {
        u16 wTmp = ntohs(m_wAcctNumLen);
        if((wBufLen < ntohs(m_wAcctNumLen))||(NULL == pBuf))
        {
            wOutLen = 0;
            return FALSE;
        }
        else
        {
            wOutLen = ntohs(m_wAcctNumLen);
            memcpy(pBuf,m_abyAcctNum,wOutLen);
            return TRUE;
        }
    }

    u16 GetAcctNumLen()
    {
        return ntohs(m_wAcctNumLen);   
    }   
}TConfAcctNum;

//计费帐号密码结构
typedef struct tagConfAcctPwd{
    u8  m_abyAcctPwd[MAX_RADIUS_ACCT_PWD_LEN];       
    u16 m_wAcctPwdLen;
    
    tagConfAcctPwd()
    {
        memset(m_abyAcctPwd,0,sizeof(m_abyAcctPwd));
        m_wAcctPwdLen = 0;               
    }
    
    BOOL SetAcctPwd(IN u8* pBuf, IN u16 wBufLen)
    {
        if((NULL == pBuf)||(wBufLen > MAX_RADIUS_ACCT_PWD_LEN))
        {
            return FALSE;
        }
        memset(m_abyAcctPwd,0,sizeof(m_abyAcctPwd));
        memcpy(m_abyAcctPwd,pBuf,wBufLen);
        m_wAcctPwdLen = htons(wBufLen);
        return TRUE;
    }

    BOOL GetAcctPwd(IN OUT u8 *pBuf, IN u16 wBufLen, OUT u16 &wOutLen)
    {
        if((wBufLen < ntohs(m_wAcctPwdLen))||(NULL == pBuf))
        {
            wOutLen = 0;
            return FALSE;
        }
        else
        {
            wOutLen = ntohs(m_wAcctPwdLen);
            memcpy(pBuf,m_abyAcctPwd,wOutLen);
            return TRUE;
        }
    }

    u16 GetAcctPwdLen()
    {
        return ntohs(m_wAcctPwdLen);   
    }  

}TConfAcctPwd;

//开始计费请求消息结构
typedef struct tagConfAcctStartReq
{
    u32 m_dwSeqNum;                        //计费请求序列号
    TConfAcctNum m_tAcctNum;               //计费帐号
    TConfAcctPwd m_tAcctPwd;               //计费帐号密码
    u8  m_abyConfName[MAX_CONF_NAME_LEN];   //会议名称
    u16 m_wConfNameLen;
    u8 m_abyConfId[MAX_CONF_E164_LEN];      //会议E164号
    u16 m_wConfIdLen;
    u32  m_dwSiteRate;                   //会议码率
    u16  m_wTermNum;                     //终端数
    u16  m_wMulVoiceNum;                 //混音数
    u16  m_wMulPicNum;                   //多画面数
    u16  m_wRateAdapNum;                 //码率适配数
    u16  m_wUseCasd;                     //是否使用级联(1:使用 0:不使用)
    u16  m_wUseEncrypt;                  //是否使用加密
    u16  m_wUseDualVideo;                //是否使用双流
    u16  m_wUseDataConf;                 //是否召开数据会议
    u16  m_wUseStream;                   //是否使用流媒体组播

    tagConfAcctStartReq()
    {
        m_dwSeqNum = 0;
        //m_emReqType = htonl(emRadReqType_START);
        memset(&m_tAcctNum,0,sizeof(m_tAcctNum));
        memset(&m_tAcctPwd,0,sizeof(m_tAcctPwd));
        memset(m_abyConfName,0,sizeof(m_abyConfName));
        m_wConfNameLen = 0;
        memset(m_abyConfId,0,sizeof(m_abyConfId));
        m_wConfIdLen = 0;
        m_dwSiteRate = 0;
        m_wTermNum = 0;
        m_wMulVoiceNum = 0;
        m_wMulPicNum = 0;
        m_wRateAdapNum = 0;
        m_wUseCasd = 0;
        m_wUseEncrypt = 0;
        m_wUseDualVideo = 0;
        m_wUseDataConf = 0;
        m_wUseStream = 0;      
    }

    //数据成员操作函数
    
    //消息序列号
    void SetSeqNum(u32 dwSeqNum)
    {
        m_dwSeqNum = htonl(dwSeqNum);
    }
    u32 GetSeqNum()
    {
        return ntohl(m_dwSeqNum);
    }

    BOOL SetAcctNum(IN u8* pbyBuf, IN u16 wBufLen)
    {
        return m_tAcctNum.SetAcctNum(pbyBuf,wBufLen);
    }

    TConfAcctNum & GetAcctNum()
    {
        return m_tAcctNum;
    }

    BOOL SetAcctPwd(IN u8* pbyBuf, IN u16 wBufLen)
    {
        return m_tAcctPwd.SetAcctPwd(pbyBuf,wBufLen);
    }
    
    TConfAcctPwd& GetAcctPwd()
    {
        return m_tAcctPwd;
    }

    //会议名称
    BOOL SetConfName(IN u8* pBuf, IN u16 wBufLen)
    {
        if((NULL == pBuf)||(wBufLen > MAX_CONF_NAME_LEN))
        {
            return FALSE;
        }
        memcpy(m_abyConfName,pBuf,wBufLen);
        m_wConfNameLen = htons(wBufLen);
        return TRUE;
    }
    BOOL GetConfName(IN OUT u8 *pBuf, IN u16 wBufLen, OUT u16 &wOutLen)
    {
        if((wBufLen < ntohs(m_wConfNameLen))||(NULL == pBuf))
        {
            wOutLen = 0;
            return FALSE;
        }
        else
        {
            wOutLen = ntohs(m_wConfNameLen);
            memcpy(pBuf,m_abyConfName,wOutLen);
            return TRUE;
        }
    }
    u16 GetConfNameLen()
    {
        return ntohs(m_wConfNameLen);   
    }

    //会议Id(E164)操作
    BOOL SetConfId(IN u8* pBuf, IN u16 wBufLen)
    {
        if((NULL == pBuf)||(wBufLen > MAX_CONF_E164_LEN))
        {
            return FALSE;
        }
        memcpy(m_abyConfId,pBuf,wBufLen);
        m_wConfIdLen = htons(wBufLen);
        return TRUE;
    }
    BOOL GetConfId(IN OUT u8 *pBuf, IN u16 wBufLen, OUT u16 &wOutLen)
    {
        if((wBufLen < ntohs(m_wConfIdLen))||(NULL == pBuf))
        {
            wOutLen = 0;
            return FALSE;
        }
        else
        {
            wOutLen = ntohs(m_wConfIdLen);
            memcpy(pBuf,m_abyConfId,wOutLen);
            return TRUE;
        }
    }
    u16 GetConfIdLen()
    {
        return ntohs(m_wConfIdLen);
    }

    //会议速率
    void SetSiteRate(u32 dwSiteRate)
    {
        m_dwSiteRate = htonl(dwSiteRate);
    }
    u32 GetSiteRate()
    {
        return ntohl(m_dwSiteRate);
    }

    //终端数
    void SetTermNum(u16 mTermNum)
    {
        m_wTermNum = htons(mTermNum) ;
    }
    u16 GetTermNum()
    {
        return ntohs(m_wTermNum);
    }

    //混音数
    void SetMulVoiceNum(u16 mMulVoiceNum)
    {
        m_wMulVoiceNum = htons(mMulVoiceNum)  ;
    }
    u16 GetMulVoiceNum()
    {
        return ntohs(m_wMulVoiceNum);
    }

    //多画面数
    void SetMulPicNum(u16 mMulPicNum)
    {
        m_wMulPicNum = htons(mMulPicNum)  ;
    }
    u16 GetMulPicNum()
    {
        return ntohs(m_wMulPicNum);
    }

    //速率适配数
    void SetRateAdapNum(u16 mRateAdapNum)
    {
        m_wRateAdapNum = htons(mRateAdapNum)  ;
    }
    u16 GetRateAdapNum()
    {
        return ntohs(m_wRateAdapNum);
    }

    //是否使用级联
    void SetUseCascade(u16 mCascade)
    {
        m_wUseCasd = htons(mCascade);
    }
    u16 GetUseCascade()
    {
        return ntohs(m_wUseCasd);
    }

    //是否使用加密
    void SetUseEncrypt(u16 mEncrypt)
    {
        m_wUseEncrypt = htons(mEncrypt);
    }
    u16 GetUseEncrypt()
    {
        return ntohs(m_wUseEncrypt);
    }

    //是否使用双流
    void SetUseDualVideo(u16 mDualVideo)
    {
        m_wUseDualVideo = htons(mDualVideo);
    }
    u16 GetUseDualVideo()
    {
        return ntohs(m_wUseDualVideo);
    }

    //是否使用数据会议
    void SetUseDataConf(u16 mDataConf)
    {
        m_wUseDataConf = htons(mDataConf)  ;
    }
    u16 GetUseDataConf()
    {
        return ntohs(m_wUseDataConf);
    }
    
    //是否使用流媒体组播
    void SetUseStream(u16 mStream)
    {
         m_wUseStream =htons(mStream)  ;
    }
    u16 GetUseStream()
    {
        return ntohs(m_wUseStream);
    }    
}TConfAcctStartReq;

//停止计费请求结构
typedef struct tagConfAcctStopReq
{
    u32 m_dwSeqNum;                                   //计费请求序列号
    TConfAcctNum   m_tAcctNum;                        //帐号
    TConfAcctPwd   m_tAcctPwd;                        //密码
    TAcctSessionId m_tSessionId;                      //计费会话id
    
 
    tagConfAcctStopReq()
    {
        m_dwSeqNum = 0;
        memset(&m_tSessionId,0,sizeof(TAcctSessionId));
        memset(&m_tAcctNum,0,sizeof(TConfAcctNum));
        memset(&m_tAcctPwd,0,sizeof(TConfAcctPwd));
    }

     //会话id
    BOOL SetSessionId(IN u8* pbyBuf, IN u16 wBufLen)
    {
        return m_tSessionId.SetAcctSessionId(pbyBuf,wBufLen);
    }

    TAcctSessionId& GetSessionId()
    {
        return m_tSessionId;
    }
    
    //消息序列号
    void SetSeqNum(u32 dwSeqNum)
    {
        m_dwSeqNum = htonl(dwSeqNum);
    }
    u32 GetSeqNum()
    {
        return ntohl(m_dwSeqNum);
    }

    BOOL SetAcctNum(IN u8* pbyBuf, IN u16 wBufLen)
    {
        return m_tAcctNum.SetAcctNum(pbyBuf,wBufLen);
    }

    TConfAcctNum & GetAcctNum()
    {
        return m_tAcctNum;
    }

    BOOL SetAcctPwd(IN u8* pbyBuf, IN u16 wBufLen)
    {
        return m_tAcctPwd.SetAcctPwd(pbyBuf,wBufLen);
    }
    
    TConfAcctPwd& GetAcctPwd()
    {
        return m_tAcctPwd;
    }
    
}TConfAcctStopReq;

//会议计费应答结构
typedef struct tagConfAcctRsp
{
    u32 m_dwSeqNum;                 //响应消息序列号(与请求序列号相同)
    u32 m_emReqType;               //对应的计费请求类型
    u32 m_dwRspVal;                //计费操作结果
    TAcctSessionId m_tSessionId;   //计费id. 对于计费开始请求，是新分配的，对于计费结束请求，同请求消息一致
                    

    tagConfAcctRsp()
    {
        m_dwSeqNum = 0;
        m_emReqType = htonl(1);
        m_dwRspVal = 0;
        memset(&m_tSessionId,0,sizeof(m_tSessionId));
    }

    //序列号
    void SetSeqNum(u32 dwSeqNum)
    {
        m_dwSeqNum = htonl(dwSeqNum);
    }
    u32 GetSeqNum()
    {
        return ntohl(m_dwSeqNum);
    }

    //请求类型
    void SetRequestType(u32 emReqType)
    {
        m_emReqType = htonl(emReqType);
    }
    ERadiusReqType GetRequsetType()
    {
        return ERadiusReqType(ntohl(m_emReqType));
    } 

    //会话id
    BOOL SetSessionId(IN u8 *pbyBuf, IN u16 wBufLen)
    {
        return m_tSessionId.SetAcctSessionId(pbyBuf,wBufLen);
    }

    TAcctSessionId& GetSessionId()
    {
        return m_tSessionId;
    }

    //计费结果
    void SetResponseValue(u32 dwRspVal)
    {
        m_dwRspVal = htonl(dwRspVal);
    }

    u32 GetResponseValue()
    {
        return  ntohl(m_dwRspVal);
    }
    
}TConfAcctRsp;











#endif //___RADIUSINTERFACE_H






















