/*****************************************************************************
模块名      : KDV RS
文件名      : kdvrs.h
相关文件    : 
文件实现功能: radius server 外部接口定义
作者        : 郭忠军
版本        : V1.0  Copyright(C) 2004-2005 KDC, All rights reserved.
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2006-08-17  1.0         郭忠军         创建
2007-09-14  1.0         郭忠军         增加详细话单结构及消息接口 
******************************************************************************/
#ifndef _KDVRS_H
#define _KDVRS_H

#include "osp.h"
#include "eventid.h"
#include "kdvdef.h"
#include "kdvsys.h"

#ifdef _WINDOWS_
#include <direct.h>
#endif // _WINDOWS_

#ifdef _VXWORKS_
#include "usrlib.h"
#include "Dirent.h"
#endif // _VXWORKS_

////////////////////////////////// 宏定义 ////////////////////////////////
//版本号
#define VER_KDVRS              (const s8*)"KDVRS40.10.01.15.080923(Daily)"

//配置文件路径及名称
#if defined (WIN32)
#define MakeDirectory(dirname) _mkdir((dirname))
#elif defined (_VXWORKS_)
#define MakeDirectory(dirname) mkdir(dirname)
#elif defined (_LINUX_)
#define MakeDirectory(dirname) mkdir(dirname, 0777)
#else
#error "This version only support windows and vxworks and linux"
#endif // _WINDOWS_  

#define   RS_CONF_PATH             (s8*)"conf"                //配置文件路径
#define   RS_CONF_NAME             (s8*)"conf/rscfg.ini"      //配置文件名称
#define   RS_CR_PATH               (s8*)"cr"                   //话单文件路径
#define   RS_CR_NAME               (s8*)"cr/callrecord.txt"   //话单文件名称


//长度定义
#define MAX_RS_CTRL_USER_NUM        64         //控制台用户最大个数
#define MAX_RS_CLIENT_NUM           256        //Radius客户最大个数


#define MAX_CTRL_USRNAME_LEN        32         //控制台用户名最大长度
#define MAX_CTRL_PWD_LEN            32         //控制台用户密码最大长度
#define MAX_CTRL_FULLNAME_LEN       128         //控制台用户全名最大长度
#define MAX_CTRL_DETAIL_LEN         256         //控制台用户描述信息最大长度

#define MAX_LOGINED_USER_NUM        64         //控制台最大并发用户登陆数
#define MAX_SYS_USER_NUM            64         //最大系统用户帐号数.add by gzj.080624.
#define HBTIME                      (u16)60     //控制台HB间隔
#define HBNUM                       (u16)3      //HB次数
#define DEFAULT_ADMIN_NAME          (s8*)"admin"      //控制台默认管理员名称              
#define DEFAULT_ADMIN_PWD           (s8*)"admin"      //控制台默认管理员密码

#define MAX_CLIENT_AMOUNT           (u32)256   //系统支持的最大帐户数目
#define MAX_CLIENT_NAME_LEN         128         //客户名称最大长度
#define MAX_CLIENT_ACCT_LEN         16         //客户帐号最大长度
#define MAX_CLIENT_ACCT_PWD_LEN     16         //帐户密码最大长度
#define MAX_ACCT_POLICY_LEN         32         //计费策略名最大长度
#define MAX_ADDR_LEN                128         //客户地址最大长度
#define MAX_MAIL_CODE_LEN           32          //邮政编码最大长度
#define MAX_PHONE_NUM_LEN           32         //电话号码最大长度
#define MAX_EMAIL_LEN               32         //电子邮件地址最大长度
#define MAX_BANK_NAME_LEN           64         //客户开户行名称最大长度
#define MAX_BANK_ACCT_LEN           64         //客户银行帐户最大长度

//#define CR_ID_LEN                   16          //话单ID长度
#define MAX_CTRL_SENDBUF_LEN        8*1024     //控制台一次发送消息的最大长度
#define MAX_CTRL_DETAIL_CR_NUM      50         //一次发送的最大详单条目数

#define MAX_CTRL_CONF_NAME_LEN      32          //会议名称最大长度
#define MAX_CTRL_CONF_ID_LEN        64          //会议E164号最大长度

#define MAX_CTRL_VER_LEN            32         //版本信息最大程度
#define MAX_CTRL_IP_LEN             16         //字符串形式IP地址最大长度
     
//Radius Server 控制台消息响应结果代码
#define CTRLMSGRSP_SUCCESS				(u32)0x0000      // 成功
#define CTRLMSGRSP_UNKOWNERR 			(u32)0x0001      // 未知错误
#define CTRLMSGRSP_INVALIDCTRLVER		(u32)0x0002      // 控制台版本错误
#define CTRLMSGRSP_USER_NOT_EXIST		(u32)0x0003      // 用户名不存在
#define CTRLMSGRSP_INVALIDPASS			(u32)0x0004      // 密码错误
#define CTRLMSGRSP_ACCESSDENIED			(u32)0x0005      // 权限不允许
#define CTRLMSGRSP_DBERROR				(u32)0x0006     // Radius DB错误
#define CTRLMSGRSP_NORECORD				(u32)0x0007     // 没有找到相应记录
#define CTRLMSGRSP_MAXUSER				(u32)0x0008     // 达到控制台用户上限
#define CTRLMSGRSP_USERLOGINED			(u32)0x0009     // 用户已登录
#define CTRLMSGRSP_USERNOTLOGIN			(u32)0x000a     // 用户未登录
#define CTRLMSGRSP_DEFAULT_FORBIDDEL    (u32)0x000b     // 不能删除默认管理员用户
#define CTRLMSGRSP_USER_EXIST			(u32)0x000c     // 用户名已存在
#define CTRLMSGRSP_POLICY_NOT_EXIST		(u32)0x000e     // 计费策略不存在
#define CTRLMSGRSP_POLICY_EXIST		    (u32)0x000f     // 计费策略已存在
#define CTRLMSGRSP_POLICY_NOT_USED		(u32)0x0010     // 计费策略未被使用
#define CTRLMSGRSP_POLICY_DEL_FORBID    (u32)0x0011     //不能删除已被使用的计费策略
#define CTRLMSGRSP_ACCT_NOT_EXIST		(u32)0x0012     // 帐户不存在
#define CTRLMSGRSP_ACCT_EXIST		    (u32)0x0013     // 帐户已存在
#define CTRLMSGRSP_SET_SYS_TIME_FAILED	(u32)0x0014     // 设置服务器时间失败
#define CTRLMSGRSP_GET_SYS_TIME_FAILED	(u32)0x0015     // 获取服务器时间失败
#define CTRLMSGRSP_MAX_ACCT_REACHED		(u32)0x0016     // 达到最大帐户数目
#define CTRLMSGRSP_ACCT_IN_USE		    (u32)0x0017     // 帐户正在被使用
#define CTRLMSGRSP_MAX_SYS_USER_REACHED      (u32)0x0018    //达到最大系统用户帐号数目。add by gzj.080624.

//监听端口号
const u16 PORT_RADIUS_SERVER = PORT_RS;                        

//////////////////////////////// RS APP ID定义 /////////////////////////////
//#define RS_START                  AID_RS_BGN
#define AID_RS_STACKINIT          (AID_RS_BGN + 1)
#define AID_RS_RADOPERATION       (AID_RS_BGN + 2)
#define AID_RS_RADCONTROL         (AID_RS_BGN + 3)

//////////////////////////////// OSP消息范围 /////////////////////////////
OSPEVENT(EV_RS_EVENT_START, EV_RS_BGN);

///////////////////////// Radius Server 系统消息范围 //////////////////
//start
OSPEVENT(EV_RS_SYS_START, EV_RS_EVENT_START + 1);

//Server启动
OSPEVENT(EV_RS_SYS_INIT, EV_RS_SYS_START + 1);
//Server关闭
OSPEVENT(EV_RS_SYS_QUIT,EV_RS_SYS_START + 2);

//收到Access-Request
OSPEVENT(EV_RS_RECV_ACESREQ, EV_RS_SYS_START + 3);
//收到Accounting-Request
OSPEVENT(EV_RS_RECV_ACCTREQ, EV_RS_SYS_START + 4);

//会议计费超时
OSPEVENT(EV_RS_ACCT_SELECT, EV_RS_SYS_START + 5);

//定时暂存当前时间
OSPEVENT(EV_RS_WR_CURTIME, EV_RS_SYS_START + 6);

//end 
OSPEVENT(EV_RS_SYS_END, EV_RS_SYS_START + 20);

///////////////////////// Radius Server 控制台消息范围 ///////////////////////
OSPEVENT(EV_RS_CONTROL_START, EV_RS_SYS_END + 1);

OSPEVENT(EV_CTRL_USER_LOG_REQ, EV_RS_CONTROL_START + 1);   //用户登录
OSPEVENT(EV_CTRL_USER_LOG_RSP, EV_RS_CONTROL_START + 2);   //用户登录响应

OSPEVENT(EV_CTRL_ADD_USER_REQ, EV_RS_CONTROL_START + 3);   //增加用户
OSPEVENT(EV_CTRL_ADD_USER_RSP, EV_RS_CONTROL_START + 4);   //增加用户响应

OSPEVENT(EV_CTRL_MOD_USER_REQ, EV_RS_CONTROL_START + 5);   //修改用户
OSPEVENT(EV_CTRL_MOD_USER_RSP, EV_RS_CONTROL_START + 6);   //修改用户响应

OSPEVENT(EV_CTRL_DEL_USER_REQ, EV_RS_CONTROL_START + 7);   //删除用户
OSPEVENT(EV_CTRL_DEL_USER_RSP, EV_RS_CONTROL_START + 8);   //删除用户响应

OSPEVENT(EV_CTRL_USER_EXIT_REQ, EV_RS_CONTROL_START + 9);   //用户退出
OSPEVENT(EV_CTRL_USER_EXIT_RSP, EV_RS_CONTROL_START + 10);  //用户退出响应


OSPEVENT(EV_CTRL_OPEN_ACCT_REQ, EV_RS_CONTROL_START + 11);   //开户
OSPEVENT(EV_CTRL_OPEN_ACCT_RSP, EV_RS_CONTROL_START + 12);  //开户响应

OSPEVENT(EV_CTRL_MOD_ACCT_REQ, EV_RS_CONTROL_START + 13);   //修改帐户
OSPEVENT(EV_CTRL_MOD_ACCT_RSP, EV_RS_CONTROL_START + 14);   //修改帐户响应

OSPEVENT(EV_CTRL_DEL_ACCT_REQ, EV_RS_CONTROL_START + 15);   //销户
OSPEVENT(EV_CTRL_DEL_ACCT_RSP, EV_RS_CONTROL_START + 16);  //销户响应

OSPEVENT(EV_CTRL_ADD_POLICY_REQ, EV_RS_CONTROL_START + 17);   //增加计费策略
OSPEVENT(EV_CTRL_ADD_POLICY_RSP, EV_RS_CONTROL_START + 18);  //增加计费策略响应

OSPEVENT(EV_CTRL_MOD_POLICY_REQ, EV_RS_CONTROL_START + 19);   //修改计费策略
OSPEVENT(EV_CTRL_MOD_POLICY_RSP, EV_RS_CONTROL_START + 20);  //修改计费策略响应

OSPEVENT(EV_CTRL_DEL_POLICY_REQ, EV_RS_CONTROL_START + 21);   //删除计费策略
OSPEVENT(EV_CTRL_DEL_POLICY_RSP, EV_RS_CONTROL_START + 22);  //删除计费策略响应

OSPEVENT(EV_CTRL_QUERY_CR_REQ, EV_RS_CONTROL_START + 23);   //查询话单请求
OSPEVENT(EV_CTRL_QUERY_CR_RSP, EV_RS_CONTROL_START + 24);   //查询话单响应

OSPEVENT(EV_CTRL_EXP_CR_REQ, EV_RS_CONTROL_START + 25);   //导出话单请求
OSPEVENT(EV_CTRL_EXP_CR_RSP, EV_RS_CONTROL_START + 26);   //导出话单响应

OSPEVENT(EV_CTRL_DEL_CR_REQ, EV_RS_CONTROL_START + 27);   //删除话单请求
OSPEVENT(EV_CTRL_DEL_CR_RSP, EV_RS_CONTROL_START + 28);   //删除话单响应

OSPEVENT(EV_CTRL_CONF_LIST_REQ, EV_RS_CONTROL_START + 29);   //会议列表请求
OSPEVENT(EV_CTRL_CONF_LIST_RSP, EV_RS_CONTROL_START + 30);   //会议列表响应

OSPEVENT(EV_CTRL_SYS_STATUS_REQ, EV_RS_CONTROL_START + 31);   //系统状态请求
OSPEVENT(EV_CTRL_SYS_STATUS_RSP, EV_RS_CONTROL_START + 32);   //系统状态响应

OSPEVENT(EV_CTRL_REBOOT_RS_CMD, EV_RS_CONTROL_START + 33);   //重启RS命令

OSPEVENT(EV_CTRL_USER_LIST_REQ, EV_RS_CONTROL_START + 34);   //获取用户列表
OSPEVENT(EV_CTRL_USER_LIST_RSP, EV_RS_CONTROL_START + 35);   //获取用户列表响应

OSPEVENT(EV_CTRL_CLIENT_LIST_REQ, EV_RS_CONTROL_START + 36);   //获取客户列表
OSPEVENT(EV_CTRL_CLIENT_LIST_RSP, EV_RS_CONTROL_START + 37);   //获取客户列表响应

OSPEVENT(EV_CTRL_POLICY_LIST_REQ, EV_RS_CONTROL_START + 38);   //获取计费策略列表
OSPEVENT(EV_CTRL_POLICY_LIST_RSP, EV_RS_CONTROL_START + 39);   //获取计费策略列表响应

OSPEVENT(EV_CTRL_SET_SERVER_TIME_REQ, EV_RS_CONTROL_START + 40);     //设置服务器时间
OSPEVENT(EV_CTRL_SET_SERVER_TIME_RSP, EV_RS_CONTROL_START + 41);   //设置服务器时间响应

OSPEVENT(EV_CTRL_GET_SERVER_TIME_REQ, EV_RS_CONTROL_START + 42);     //获取服务器时间
OSPEVENT(EV_CTRL_GET_SERVER_TIME_RSP, EV_RS_CONTROL_START + 43);   //获取服务器时间响应

//add by gzj.070914
OSPEVENT(EV_CTRL_GET_DETAIL_CR_REQ, EV_RS_CONTROL_START + 44);   //获取详细话单请求. 话单id个数(u32) + 变长的话单id序列
OSPEVENT(EV_CTRL_GET_DETAIL_CR_RSP, EV_RS_CONTROL_START + 45);   //获取详细话单响应
//end add.

OSPEVENT(EV_RS_CONTROL_END, EV_RS_CONTROL_START+80);

////////////////////////Radius Server 测试消息范围 ///////////////////////////
OSPEVENT(EV_RS_TEST_START,EV_RS_CONTROL_END + 1);



OSPEVENT(EV_RS_TEST_END, EV_RS_TEST_START+50);

////////////////////////////////// 枚举定义 ////////////////////////////////
//控制台用户权限级别
enum ERSCtrlLevel
{
    emLevel_ADMIN = 0,    //管理员
    emLevel_USER  = 1,    //普通用户
    emLevel_OTHER,        //其它权限
};

//Radius数据库连接状态
enum EDBContStatus
{
    emDBStatus_UNCONNECT = 0,   //未连接
    emDBStatus_CONNECT ,        //已连接
};

//话单查询选项
enum emQueryOpt
{ 
    emQueryOpt_None = 0,             //不使用查询选项（查询所有）
    emQueryOpt_CRID = 2,             //按话单编号
    emQueryOpt_Client = 4,          //按客户名称
    emQueryOpt_TimeRange = 8,       //按话单时间
};

#define SAFEDEL(ptr)                {if(NULL != (ptr)) delete (ptr); (ptr) = NULL;}
////////////////////////////////// 结构定义 ////////////////////////////////
//控制台操作通用头部结构
//消息体为对应消息号所使用的结构体
typedef struct tagRSCtrlHead
{
private:
    u32     m_dwSeqNum;          //序列号
    u32     m_dwErrorCode;       //错误码
    u32     m_dwContentLen;      //消息体内容长度
    u32     m_dwStructNum;       //消息体的结构体数目
    BOOL    m_bStructLenChange;  //消息体的结构内容是否变化
    BOOL    m_bWillContinue;     //是否有后续的消息，用于消息体长度太长的情况    
  
public:
    tagRSCtrlHead()
    {
        m_dwSeqNum = 0;
        m_dwErrorCode = 0;
        m_dwContentLen = 0;
        m_dwStructNum = 0;
        m_bStructLenChange = FALSE;
        m_bWillContinue = FALSE;
    }

    void SetSequenceNumber(u32 dwSequenceNumber)
    {
        m_dwSeqNum = htonl(dwSequenceNumber);
    }
    u32 GetSequenceNumber()
    {
        return ntohl(m_dwSeqNum);
    }

    void SetErrorCode(u32 dwErrorCode)
    {
        m_dwErrorCode = htonl(dwErrorCode);
    }
    u32 GetErrorCode()
    {
        return ntohl(m_dwErrorCode);
    }

    void SetContentLen(u32 dwContentLen)
    {
        m_dwContentLen = htonl(dwContentLen);
    }
    u32 GetContentLen()
    {
        return ntohl(m_dwContentLen);
    }
    
    void SetStructNum(u32 dwStructNum)
    {
        m_dwStructNum = htonl(dwStructNum);
    }
    u32 GetStructNum()
    {
        return ntohl(m_dwStructNum);
    }

    void SetStructLenChange(BOOL bStructLenChange)
    {
        m_bStructLenChange = htonl(bStructLenChange);
    }
    BOOL IsStructLenChange()
    {
        return ntohl(m_bStructLenChange);
    }
    
    void SetWillContinue(BOOL bWillContinue)
    {
        m_bWillContinue = htonl(bWillContinue);
    }
    BOOL IsWillContinue()
    {
        return ntohl(m_bWillContinue);
    }

}TRSCtrlHead;


//控制台用户管理信息结构
typedef struct tagCtrlUserLogInfo
{
public:
    u8		m_abyUsrName[MAX_CTRL_USRNAME_LEN];         //用户名
    u16		m_wUsrnameLen;
    u8		m_abyPwd[MAX_CTRL_PWD_LEN];                 //密码
    u16		m_wPwdLen;
    u8      m_abyFullName[MAX_CTRL_FULLNAME_LEN];       //完整用户名
    u16     m_wFullnameLen;
    u16 	m_wCtrlLevel;                               //权限
    u8      m_abyDetail[MAX_CTRL_DETAIL_LEN];           //描述信息
    u16     m_wDetailLen;

public:    
    tagCtrlUserLogInfo()
    {
        Clear();
    }    
    
    BOOL SetUserName( IN u8 *pbyUserName, IN u16 wNameLen )
    {
        if ( pbyUserName == NULL || wNameLen > MAX_CTRL_USRNAME_LEN)
        {
            return FALSE;
        }
        memcpy( m_abyUsrName, pbyUserName, wNameLen );
        m_wUsrnameLen = htons( wNameLen );
        return TRUE;
    }

    BOOL SetFullName( IN u8 *pbyUserName, IN u16 wNameLen )
    {
        if ( pbyUserName == NULL || wNameLen > MAX_CTRL_FULLNAME_LEN)
        {
            return FALSE;
        }
        memcpy( m_abyFullName, pbyUserName, wNameLen );
        m_wFullnameLen = htons( wNameLen );
        return TRUE;
    }
    
    BOOL SetDetail( IN u8 *pbyDetail, IN u16 wLen )
    {
        if ( pbyDetail == NULL || wLen > MAX_CTRL_FULLNAME_LEN)
        {
            return FALSE;
        }
        memcpy( m_abyDetail, pbyDetail, wLen );
        m_wDetailLen = htons( wLen );
        return TRUE;
    }


    u16 GetUserNameLen()
    {
        return ntohs( m_wUsrnameLen );
    }

    u16 GetFullNameLen()
    {
        return ntohs( m_wFullnameLen );
    }

    void GetUserName( OUT u8 *pbyUserName, OUT u16 &wNameLen )
    {
        if ( pbyUserName == NULL || wNameLen < ntohs( m_wUsrnameLen ) )
        {
            return;
        }
        wNameLen = ntohs( m_wUsrnameLen );
        memcpy( pbyUserName, m_abyUsrName, wNameLen );
    }

    void GetFullName( OUT u8 *pbyUserName, OUT u16 &wNameLen )
    {
        if ( pbyUserName == NULL || wNameLen < ntohs( m_wFullnameLen ) )
        {
            return;
        }
        wNameLen = ntohs( m_wFullnameLen );
        memcpy( pbyUserName, m_abyFullName, wNameLen );
    }

    void GetDetail( OUT u8 *pbyDetail, OUT u16 &wLen )
    {
        if ( pbyDetail == NULL || wLen < ntohs( m_wDetailLen ) )
        {
            return;
        }
        wLen = ntohs( m_wDetailLen );
        memcpy( pbyDetail, m_abyDetail, wLen );
    }

    BOOL SetPassword( IN u8 *pbyPassword, IN u16 wPassLen )
    {
        if ( pbyPassword == NULL || wPassLen > MAX_CTRL_PWD_LEN )
        {
            return FALSE;
        }        
        memcpy( m_abyPwd, pbyPassword, wPassLen );
        m_wPwdLen = htons( wPassLen );
        return TRUE;
    }
  
    u16 GetPasswordLen()
    {
        return ntohs( m_wPwdLen );
    }
    
    void GetPassword( OUT u8 *pbyPassword, OUT u16 &wPassLen )
    {
        if ( pbyPassword == NULL || wPassLen < ntohs( m_wPwdLen ) )
            return;
        
        wPassLen = ntohs( m_wPwdLen );
        memcpy( pbyPassword, m_abyPwd, wPassLen );
    }

    void SetCtrlLevel( IN u16 wLevel )
    {
        m_wCtrlLevel = htons(wLevel);
    }
    
    u16 GetCtrlLevel()
    {
        return ntohs( m_wCtrlLevel);
    }

    BOOL IsAdministrator()
    {
        if(emLevel_ADMIN == GetCtrlLevel())
        {
            return TRUE;
        }
        else
        {
            return FALSE;
        }
    }    

    void Clear()
    {
        memset( m_abyUsrName, 0, sizeof(m_abyUsrName) );
        memset( m_abyPwd, 0, sizeof(m_abyPwd) );
        memset(m_abyFullName,0,sizeof(m_abyFullName));   
        memset(m_abyDetail,0,sizeof(m_abyDetail));
        m_wUsrnameLen = 0;
        m_wPwdLen = 0;
        m_wFullnameLen = 0;
        m_wCtrlLevel = 0; 
        m_wDetailLen = 0;  
    }
    
}TCtrlUserLogInfo,*PTCtrlUserLogInfo;

//客户管理信息
typedef struct tagCtrlClientInfo
{
    u8   m_abyClientName[MAX_CLIENT_NAME_LEN];   //客户名称
    u16  m_wClientNameLen;
    u8   m_abyDetail[MAX_CTRL_DETAIL_LEN];       //描述信息
    u16  m_wDetailLen;
    u8   m_abyAcctNum[MAX_CLIENT_ACCT_LEN];      //客户帐号
    u16  m_wAcctLen;
    u8   m_abyAcctPwd[MAX_CLIENT_ACCT_PWD_LEN];  //帐号密码
    u16  m_wAcctPwdLen;              
    u32  m_dwAcctType;                            //帐号类型（0:后付费/1:预付费）
    u32  m_dwAcctFlag;                            //计费标志 (0: 免费 /1: 收费)
    u8   m_abyAcctPolicy[MAX_ACCT_POLICY_LEN];   //计费策略
    u16  m_wPolicyLen;
    u8   m_abyAddr[MAX_ADDR_LEN];                //客户地址
    u16  m_wAddrLen; 
    u8   m_abyMailCode[MAX_MAIL_CODE_LEN];       //邮编
    u16  m_wMCLen;
    u8   m_abyContact[MAX_CTRL_USRNAME_LEN];     //联系人
    u16  m_wContLen;
    u8   m_abyPhone[MAX_PHONE_NUM_LEN];          //联系电话
    u16  m_wPhoneLen;
    u8   m_abyFax[MAX_PHONE_NUM_LEN];            //传真号码
    u16  m_wFaxLen;
    u8   m_abyEmail[MAX_EMAIL_LEN];              //email地址
    u16  m_wEmailLen;
    u8   m_abyBankName[MAX_BANK_NAME_LEN];       //开户银行
    u16  m_wBankNameLen;
    u8   m_abyBankAcct[MAX_BANK_ACCT_LEN];       //银行帐号
    u16  m_wBankAcctLen;
    
    tagCtrlClientInfo()
    {
        Clear();  
     }
    
    //客户名称
     BOOL SetClienName(IN u8* pInBuf, IN u16 wBufLen)
     {
         if((NULL == pInBuf) || (wBufLen > MAX_CLIENT_NAME_LEN))
         {
             return FALSE;
         }
         memcpy(m_abyClientName,pInBuf,wBufLen);
         m_wClientNameLen = htons(wBufLen);

         return TRUE;
     }

     BOOL GetClientName(IN OUT u8* pOutBuf, IN u16 wBufLen, OUT u16 &wOutLen)
     {
         u16 wRealLen = ntohs(m_wClientNameLen);
         if((NULL == pOutBuf) ||(wBufLen < wRealLen))
         {
             wOutLen = 0;
             return FALSE;
         }
         memcpy(pOutBuf,m_abyClientName,wRealLen);
         wOutLen = wRealLen;
         return TRUE;         
     }

     u16 GetClientNameLen()
     {
         return ntohs(m_wClientNameLen);
     }

     //描述信息
     BOOL SetDetail(IN u8* pInBuf, IN u16 wBufLen)
     {
         if((NULL == pInBuf) || (wBufLen > MAX_CTRL_DETAIL_LEN))
         {
             return FALSE;
         }
         memcpy(m_abyDetail,pInBuf,wBufLen);
         m_wDetailLen = htons(wBufLen);

         return TRUE;
     }

     BOOL GetDetail(IN OUT u8* pOutBuf, IN u16 wBufLen, OUT u16 &wOutLen)
     {
         u16 wRealLen = ntohs(m_wDetailLen);
         if((NULL == pOutBuf) ||(wBufLen < wRealLen))
         {
             wOutLen = 0;
             return FALSE;
         }
         memcpy(pOutBuf,m_abyDetail,wRealLen);
         wOutLen = wRealLen;
         return TRUE;         
     }

     u16 GetDetailLen()
     {
         return ntohs(m_wDetailLen);
     }

     //帐号
     BOOL SetAcctNum(IN u8* pInBuf, IN u16 wBufLen)
     {
         if((NULL == pInBuf) || (wBufLen > MAX_CLIENT_ACCT_LEN))
         {
             return FALSE;
         }
         memcpy(m_abyAcctNum,pInBuf,wBufLen);
         m_wAcctLen = htons(wBufLen);

         return TRUE;
     }

     BOOL GetAcctNum(IN OUT u8* pOutBuf, IN u16 wBufLen, OUT u16 &wOutLen)
     {
         u16 wRealLen = ntohs(m_wAcctLen);
         if((NULL == pOutBuf) ||(wBufLen < wRealLen))
         {
             wOutLen = 0;
             return FALSE;
         }
         memcpy(pOutBuf,m_abyAcctNum,wRealLen);
         wOutLen = wRealLen;
         return TRUE;         
     }

     u16 GetAcctNumLen()
     {
         return ntohs(m_wAcctLen);
     }

     //密码
     BOOL SetAcctPwd(IN u8* pInBuf, IN u16 wBufLen)
     {
         if((NULL == pInBuf) || (wBufLen > MAX_CLIENT_ACCT_PWD_LEN))
         {
             return FALSE;
         }
         memcpy(m_abyAcctPwd,pInBuf,wBufLen);
         m_wAcctPwdLen = htons(wBufLen);

         return TRUE;
     }

     BOOL GetAcctPwd(IN OUT u8* pOutBuf, IN u16 wBufLen, OUT u16 &wOutLen)
     {
         u16 wRealLen = ntohs(m_wAcctPwdLen);
         if((NULL == pOutBuf) ||(wBufLen < wRealLen))
         {
             wOutLen = 0;
             return FALSE;
         }
         memcpy(pOutBuf,m_abyAcctPwd,wRealLen);
         wOutLen = wRealLen;
         return TRUE;         
     }

     u16 GetAcctPwdLen()
     {
         return ntohs(m_wAcctPwdLen);
     }

     //帐号类型
     void SetAcctType(u32 dwAcctType)
     {
         m_dwAcctType = htonl(dwAcctType);         
     }

     u32 GetAcctType()
     {
         return ntohl(m_dwAcctType);
     }

     //计费标志
     void SetAcctFlag(u32 dwAcctFlag)
     {
         m_dwAcctFlag = htonl(dwAcctFlag);
     }

     u32 GetAcctFlag()
     {
         return ntohl(m_dwAcctFlag);
     }
     
     //计费策略
     BOOL SetAcctPolicy(IN u8* pInBuf, IN u16 wBufLen)
     {
         if((NULL == pInBuf) || (wBufLen > MAX_ACCT_POLICY_LEN))
         {
             return FALSE;
         }
         memcpy(m_abyAcctPolicy,pInBuf,wBufLen);
         m_wPolicyLen= htons(wBufLen);

         return TRUE;
     }

     BOOL GetAcctPolicy(IN OUT u8* pOutBuf, IN u16 wBufLen, OUT u16 &wOutLen)
     {
         u16 wRealLen = ntohs(m_wPolicyLen);
         if((NULL == pOutBuf) ||(wBufLen < wRealLen))
         {
             wOutLen = 0;
             return FALSE;
         }
         memcpy(pOutBuf,m_abyAcctPolicy,wRealLen);
         wOutLen = wRealLen;
         return TRUE;         
     }

     u16 GetAcctPolicyLen()
     {
         return ntohs(m_wPolicyLen);
     }

     //地址
     BOOL SetClientAddr(IN u8* pInBuf, IN u16 wBufLen)
     {
         if((NULL == pInBuf) || (wBufLen > MAX_ADDR_LEN))
         {
             return FALSE;
         }
         memcpy(m_abyAddr,pInBuf,wBufLen);
         m_wAddrLen = htons(wBufLen);

         return TRUE;
     }

     BOOL GetClientAddr(IN OUT u8* pOutBuf, IN u16 wBufLen, OUT u16 &wOutLen)
     {
         u16 wRealLen = ntohs(m_wAddrLen);
         if((NULL == pOutBuf) ||(wBufLen < wRealLen))
         {
             wOutLen = 0;
             return FALSE;
         }
         memcpy(pOutBuf,m_abyAddr,wRealLen);
         wOutLen = wRealLen;
         return TRUE;         
     }

     u16 GetAddrLen()
     {
         return ntohs(m_wAddrLen);
     }

     //邮政编码
     BOOL SetMailCode(IN u8* pInBuf, IN u16 wBufLen)
     {
         if((NULL == pInBuf) || (wBufLen > MAX_MAIL_CODE_LEN ))
         {
             return FALSE;
         }
         memcpy(m_abyMailCode,pInBuf,wBufLen);
         m_wMCLen = htons(wBufLen);

         return TRUE;
     }

     BOOL GetMailCode(IN OUT u8* pOutBuf, IN u16 wBufLen, OUT u16 &wOutLen)
     {
         u16 wRealLen = ntohs(m_wMCLen);
         if((NULL == pOutBuf) ||(wBufLen < wRealLen))
         {
             wOutLen = 0;
             return FALSE;
         }
         memcpy(pOutBuf,m_abyMailCode,wRealLen);
         wOutLen = wRealLen;
         return TRUE;         
     }

     u16 GetMailCodeLen()
     {
         return ntohs(m_wMCLen);
     }

     //联系人
     BOOL SetContact(IN u8* pInBuf, IN u16 wBufLen)
     {
         if((NULL == pInBuf) || (wBufLen > MAX_CTRL_USRNAME_LEN))
         {
             return FALSE;
         }
         memcpy(m_abyContact,pInBuf,wBufLen);
         m_wContLen = htons(wBufLen);

         return TRUE;
     }

     BOOL GetContact(IN OUT u8* pOutBuf, IN u16 wBufLen, OUT u16 &wOutLen)
     {
         u16 wRealLen = ntohs(m_wContLen);
         if((NULL == pOutBuf) ||(wBufLen < wRealLen))
         {
             wOutLen = 0;
             return FALSE;
         }
         memcpy(pOutBuf,m_abyContact,wRealLen);
         wOutLen = wRealLen;
         return TRUE;         
     }

     u16 GetContactLen()
     {
         return ntohs(m_wContLen);
     }

     //电话号码
     BOOL SetPhoneNum(IN u8* pInBuf, IN u16 wBufLen)
     {
         if((NULL == pInBuf) || (wBufLen >MAX_PHONE_NUM_LEN))
         {
             return FALSE;
         }
         memcpy(m_abyPhone,pInBuf,wBufLen);
         m_wPhoneLen = htons(wBufLen);

         return TRUE;
     }

     BOOL GetPhoneNum(IN OUT u8* pOutBuf, IN u16 wBufLen, OUT u16 &wOutLen)
     {
         u16 wRealLen = ntohs(m_wPhoneLen);
         if((NULL == pOutBuf) ||(wBufLen < wRealLen))
         {
             wOutLen = 0;
             return FALSE;
         }
         memcpy(pOutBuf,m_abyPhone,wRealLen);
         wOutLen = wRealLen;
         return TRUE;         
     }

     u16 GetPhoneNumLen()
     {
         return ntohs(m_wPhoneLen);
     }

     //传真号码
     BOOL SetFaxNum(IN u8* pInBuf, IN u16 wBufLen)
     {
         if((NULL == pInBuf) || (wBufLen >MAX_PHONE_NUM_LEN))
         {
             return FALSE;
         }
         memcpy(m_abyFax,pInBuf,wBufLen);
         m_wFaxLen = htons(wBufLen);

         return TRUE;
     }

     BOOL GetFaxNum(IN OUT u8* pOutBuf, IN u16 wBufLen, OUT u16 &wOutLen)
     {
         u16 wRealLen = ntohs(m_wFaxLen);
         if((NULL == pOutBuf) ||(wBufLen < wRealLen))
         {
             wOutLen = 0;
             return FALSE;
         }
         memcpy(pOutBuf,m_abyFax,wRealLen);
         wOutLen = wRealLen;
         return TRUE;         
     }
     
     u16 GetFaxNumLen()
     {
         return ntohs(m_wFaxLen);
     }

     //电子邮件
     BOOL SetEmailAddr(IN u8* pInBuf, IN u16 wBufLen)
     {
         if((NULL == pInBuf) || (wBufLen >MAX_EMAIL_LEN))
         {
             return FALSE;
         }
         memcpy(m_abyEmail,pInBuf,wBufLen);
         m_wEmailLen = htons(wBufLen);

         return TRUE;
     }

     BOOL GetEmailAddr(IN OUT u8* pOutBuf, IN u16 wBufLen, OUT u16 &wOutLen)
     {
         u16 wRealLen = ntohs(m_wEmailLen);
         if((NULL == pOutBuf) ||(wBufLen < wRealLen))
         {
             wOutLen = 0;
             return FALSE;
         }
         memcpy(pOutBuf,m_abyEmail,wRealLen);
         wOutLen = wRealLen;
         return TRUE;         
     }

     u16 GetEmailLen()
     {
         return ntohs(m_wEmailLen);
     }

     //开户银行
     BOOL SetBankName(IN u8* pInBuf, IN u16 wBufLen)
     {
         if((NULL == pInBuf) || (wBufLen >MAX_BANK_NAME_LEN))
         {
             return FALSE;
         }
         memcpy(m_abyBankName,pInBuf,wBufLen);
         m_wBankNameLen = htons(wBufLen);

         return TRUE;
     }

     BOOL GetBankName(IN OUT u8* pOutBuf, IN u16 wBufLen, OUT u16 &wOutLen)
     {
         u16 wRealLen = ntohs(m_wBankNameLen);
         if((NULL == pOutBuf) ||(wBufLen < wRealLen))
         {
             wOutLen = 0;
             return FALSE;
         }
         memcpy(pOutBuf,m_abyBankName,wRealLen);
         wOutLen = wRealLen;
         return TRUE;         
     }

     u16 GetBankNameLen()
     {
         return ntohs(m_wBankNameLen);
     }

     //银行帐户号码
     BOOL SetBankAcctNum(IN u8* pInBuf, IN u16 wBufLen)
     {
         if((NULL == pInBuf) || (wBufLen >MAX_BANK_ACCT_LEN))
         {
             return FALSE;
         }
         memcpy(m_abyBankAcct,pInBuf,wBufLen);
         m_wBankAcctLen = htons(wBufLen);

         return TRUE;
     }

     BOOL GetBankAcctNum(IN OUT u8* pOutBuf, IN u16 wBufLen, OUT u16 &wOutLen)
     {
         u16 wRealLen = ntohs(m_wBankAcctLen);
         if((NULL == pOutBuf) ||(wBufLen < wRealLen))
         {
             wOutLen = 0;
             return FALSE;
         }
         memcpy(pOutBuf,m_abyBankAcct,wRealLen);
         wOutLen = wRealLen;
         return TRUE;         
     }

     u16 GetBankAcctLen()
     {
         return ntohs(m_wBankAcctLen);
     }

     void Clear()
     {
        memset(m_abyClientName,0,sizeof(m_abyClientName));
        m_wClientNameLen = 0;

        memset(m_abyDetail,0,sizeof(m_abyDetail));
        m_wDetailLen = 0;
        
        memset(m_abyAcctNum,0,sizeof(m_abyAcctNum));
        m_wAcctLen = 0;

        memset(m_abyAcctPwd,0,sizeof(m_abyAcctPwd));
        m_wAcctPwdLen = 0;

        m_dwAcctType = 0;
        m_dwAcctFlag = 0;

        memset(m_abyAcctPolicy,0,sizeof(m_abyAcctPolicy));
        m_wPolicyLen = 0;
        
        memset(m_abyAddr,0,sizeof(m_abyAddr));
        m_wAddrLen= 0;

        memset(m_abyMailCode,0,sizeof(m_abyMailCode));
        m_wMCLen = 0;

        memset(m_abyContact,0,sizeof(m_abyContact));
        m_wContLen = 0;

        memset(m_abyPhone,0,sizeof(m_abyPhone));
        m_wPhoneLen = 0;
        
        memset(m_abyFax,0,sizeof(m_abyFax));
        m_wFaxLen = 0;
        
        memset(m_abyEmail,0,sizeof(m_abyEmail));
        m_wEmailLen = 0;

        memset(m_abyClientName,0,sizeof(m_abyClientName));
        m_wClientNameLen = 0;

        memset(m_abyBankName,0,sizeof(m_abyBankName));
        m_wBankNameLen = 0;

        memset(m_abyBankAcct,0,sizeof(m_abyBankAcct));
        m_wBankAcctLen = 0;             
     }


      
}TCtrlClientInfo, *PTCtrlClientInfo;


//计费策略信息结构
typedef struct tagCtrlAcctPolicy
{
    u8   m_abyPolicyName[MAX_ACCT_POLICY_LEN];  //策略名称
    u16  m_wPolicyLen;                          
    u32  m_dwUpBW;                              //带宽使用单价(yuan/kbs)
    u32  m_dwUpTerm;                            //终端使用单价
    u32  m_dwUpMulPic;                          //多画面使用单价
    u32  m_dwUpRateAdap;                        //速率使用单价
    u32  m_dwUpMulVoice;                        //混音使用单价
    u32  m_dwUpDataConf;                        //数据会议使用单价
    u32  m_dwUpStream;                          //流媒体使用单价
    u32  m_dwUpDVideo;                          //双流使用单价
    u32  m_dwUpEncrypt;                         //加密使用单价
    u32  m_dwUpCascade;                         //级联长途使用单价

    tagCtrlAcctPolicy()
    {
        Clear();
    }

     //计费策略
     BOOL SetAcctPolicy(IN u8* pInBuf, IN u16 wBufLen)
     {
         if((NULL == pInBuf) || (wBufLen > MAX_ACCT_POLICY_LEN))
         {
             return FALSE;
         }
         memcpy(m_abyPolicyName,pInBuf,wBufLen);
         m_wPolicyLen= htons(wBufLen);

         return TRUE;
     }

     BOOL GetAcctPolicy(IN OUT u8* pOutBuf, IN u16 wBufLen, OUT u16 &wOutLen)
     {
         u16 wRealLen = ntohs(m_wPolicyLen);
         if((NULL == pOutBuf) ||(wBufLen < wRealLen))
         {
             wOutLen = 0;
             return FALSE;
         }
         memcpy(pOutBuf,m_abyPolicyName,wRealLen);
         wOutLen = wRealLen;
         return TRUE;         
     }

     u16 GetPolicyNameLen()
     {
         return ntohs(m_wPolicyLen);
     }

     //带宽单价
     void SetUpBandWidth(u32 dwUnitPrice)
     {
         m_dwUpBW = htonl(dwUnitPrice);
     }

     u32 GetUpBandWidth()
     {
         return ntohl(m_dwUpBW);
     }
     
      //终端单价
     void SetUpTerminal(u32 dwUnitPrice)
     {
         m_dwUpTerm = htonl(dwUnitPrice);
     }

     u32 GetUpTerminal()
     {
         return ntohl(m_dwUpTerm);
     }

     //多画面单价
     void SetUpMulPic(u32 dwUnitPrice)
     {
         m_dwUpMulPic= htonl(dwUnitPrice);
     }

     u32 GetUpMulPic()
     {
         return ntohl(m_dwUpMulPic);
     }

     //速率适配单价
     void SetUpRateAdap(u32 dwUnitPrice)
     {
         m_dwUpRateAdap= htonl(dwUnitPrice);
     }

     u32 GetUpRateAdap()
     {
         return ntohl(m_dwUpRateAdap);
     }

     //混音单价
     void SetUpMulVoice(u32 dwUnitPrice)
     {
         m_dwUpMulVoice = htonl(dwUnitPrice);
     }

     u32 GetUpMulVoice()
     {
         return ntohl(m_dwUpMulVoice);
     }

     //数据会议使用单价
     void SetUpDataConf(u32 dwUnitPrice)
     {
         m_dwUpDataConf = htonl(dwUnitPrice);
     }

     u32 GetUpDataConf()
     {
         return ntohl(m_dwUpDataConf);
     }

     //流服务使用单价
     void SetUpStream(u32 dwUnitPrice)
     {
         m_dwUpStream= htonl(dwUnitPrice);
     }

     u32 GetUpStream()
     {
         return ntohl(m_dwUpStream);
     }

     //双流使用单价
     void SetUpDualVideo(u32 dwUnitPrice)
     {
         m_dwUpDVideo = htonl(dwUnitPrice);
     }

     u32 GetUpDualVideo()
     {
         return ntohl(m_dwUpDVideo);
     }

     //加密使用单价
     void SetUpEncrypt(u32 dwUnitPrice)
     {
         m_dwUpEncrypt = htonl(dwUnitPrice);
     }

     u32 GetUpEncrypt()
     {
         return ntohl(m_dwUpEncrypt);
     }

     //级联长途使用单价
     void SetUpCascade(u32 dwUnitPrice)
     {
         m_dwUpCascade = htonl(dwUnitPrice);
     }

     u32 GetUpCascade()
     {
         return ntohl(m_dwUpCascade);
     }

     void Clear()
     {
        memset(m_abyPolicyName,0,sizeof(m_abyPolicyName));
        m_wPolicyLen = 0;       
        m_dwUpMulVoice = 0;
        m_dwUpBW = 0;
        m_dwUpCascade = 0;
        m_dwUpDataConf = 0;
        m_dwUpDVideo = 0;
        m_dwUpEncrypt = 0;
        m_dwUpMulPic = 0;
        m_dwUpRateAdap = 0;
        m_dwUpStream = 0;
        m_dwUpTerm = 0;         
     }

}TCtrlAcctPolicy, *PTCtrlAcctPolicy;


//话单信息结构
typedef struct tagCtrlCRInfo
{
    //u8  m_abyCRID[CR_ID_LEN];                     //话单编号
    u32 m_dwCRId;                                   //话单编号  mod 0918    
    u8  m_abyClientName[MAX_CLIENT_NAME_LEN];     //客户名称
    u16 m_wClientLen;
    u8 m_abyAcctNum[MAX_CLIENT_ACCT_LEN];         //帐号
    u16 m_wAcctLen;
    u32 m_dwCRTime;                               //记录时间
    u32 m_dwServType;                             //服务类型
    u32 m_dwStartTime;                            //开始时间
    u32 m_dwStopTime;                             //结束时间
    u32 m_dwFee;                                  //费用

    tagCtrlCRInfo()
    {
        m_dwCRId = 0;
        memset(m_abyClientName,0,sizeof(m_abyClientName));
        m_wClientLen = 0;
        memset(m_abyAcctNum,0,sizeof(m_abyAcctNum));
        m_wAcctLen = 0;
        m_dwCRTime = 0;
        m_dwServType = 0;
        m_dwStartTime = 0;
        m_dwStopTime = 0;
        m_dwFee = 0;
    }
    
     //话单编号   
    void SetCRID(u32 dwCRID)
    {
        m_dwCRId = htonl(dwCRID);
    }
    
    u32 GetCRID()
    {
        return ntohl(m_dwCRId);
    }
     
     //客户名称
     BOOL SetClientName(IN u8* pInBuf, IN u16 wBufLen)
     {
         if((NULL == pInBuf) || (wBufLen >MAX_CLIENT_NAME_LEN))
         {
             return FALSE;
         }
         memcpy(m_abyClientName,pInBuf,wBufLen);
         m_wClientLen = htons(wBufLen);

         return TRUE;
     }

     BOOL GetClientName(IN OUT u8* pOutBuf, IN u16 wBufLen, OUT u16 &wOutLen)
     {
         u16 wRealLen = ntohs(m_wClientLen);
         if((NULL == pOutBuf) ||(wBufLen < wRealLen))
         {
             wOutLen = 0;
             return FALSE;
         }
         memcpy(pOutBuf,m_abyClientName,wRealLen);
         wOutLen = wRealLen;
         return TRUE;         
     }

     //帐号名称
     BOOL SetAcctNum(IN u8* pInBuf, IN u16 wBufLen)
     {
         if((NULL == pInBuf) || (wBufLen >MAX_CLIENT_ACCT_LEN))
         {
             return FALSE;
         }
         memcpy(m_abyAcctNum,pInBuf,wBufLen);
         m_wAcctLen = htons(wBufLen);

         return TRUE;
     }

     BOOL GetAcctNum(IN OUT u8* pOutBuf, IN u16 wBufLen, OUT u16 &wOutLen)
     {
         u16 wRealLen = ntohs(m_wAcctLen);
         if((NULL == pOutBuf) ||(wBufLen < wRealLen))
         {
             wOutLen = 0;
             return FALSE;
         }
         memcpy(pOutBuf,m_abyAcctNum,wRealLen);
         wOutLen = wRealLen;
         return TRUE;         
     }

     //记录时间
     void SetCRTime(u32 dwCRTime)
     {
         m_dwCRTime = htonl(dwCRTime);
     }
     u32 GetCRTime()
     {
         return ntohl(m_dwCRTime);
     }

     void SetStartTime(u32 dwStartTime)
     {
         m_dwStartTime = htonl(dwStartTime);
     }
     u32 GetStartTime()
     {
         return ntohl(m_dwStartTime);
     }

     void SetStopTime(u32 dwStopTime)
     {
         m_dwStopTime = htonl(dwStopTime);
     }
     u32 GetStopTime()
     {
         return ntohl(m_dwStopTime);
     }

     void SetServiceType(u32 dwServType)
     {
         m_dwServType = htonl(dwServType);
     }
     u32 GetServiceType()
     {
         return ntohl(m_dwServType);
     }

     void SetFee(u32 dwFee)
     {
         m_dwFee = htonl(dwFee);
     }

     u32 GetFee()
     {
         return ntohl(m_dwFee);
     }
    
}TCtrlCRInfo;

//话单查询请求结构
typedef struct tagCtrlCRQueryOpt
{
    //u8   m_abyCRID[CR_ID_LEN];     //按话单编号查询
    u32  m_dwCRId;                    
    BOOL  m_bCRIdUsed;

    u8   m_abyClient[MAX_CLIENT_NAME_LEN];    //按客户名称查询
    u16  m_wClientLen;
    BOOL  m_bClientUsed;

    u8   m_abyAcctNum[MAX_CLIENT_ACCT_LEN];    //按账号查询
    u16  m_wAcctNumLen;                        
    BOOL m_bAcctNumUsed; 

    u32  m_dwTimeFrom;                    //按服务时间段查询
    u32  m_dwTimeTo;                      //起始时刻
    BOOL  m_bTimeUsed;                     //终止时刻

    u16  m_wQueryOpt;      //查询选项

    tagCtrlCRQueryOpt()
    {
        m_dwCRId = 0;
        m_bCRIdUsed = htonl(FALSE);
        memset(m_abyClient,0,sizeof(m_abyClient));
        m_wClientLen = 0;
        m_bClientUsed = htonl(FALSE);
        memset(m_abyAcctNum,0,sizeof(m_abyAcctNum));
        m_wAcctNumLen = 0;
        m_bAcctNumUsed= htonl(FALSE);
        m_dwTimeFrom = 0;
        m_dwTimeTo = 0;
        m_bTimeUsed = htonl(FALSE);     
        m_wQueryOpt = 0;
    }

    void SetCRID(u32 dwCRId)
    {
        m_dwCRId = htonl(dwCRId);        
    }
    
    u32 GetCRID()
    {
        return ntohl(m_dwCRId);
        
     }

    BOOL SetClientName(IN u8 *pbyBuf, IN u16 wBufLen)
    {
        if((NULL == pbyBuf)||(wBufLen > MAX_CLIENT_NAME_LEN))
        {
            return FALSE;
        }
        memcpy(m_abyClient,pbyBuf,wBufLen);
        m_wClientLen = htons(wBufLen);
        return TRUE;
    }

    BOOL GetClientName(IN OUT u8 *pbyOutBuf, IN u16 wBufLen, OUT u16 &wOutLen)
    {
        if((NULL == pbyOutBuf)||(wBufLen < ntohs(m_wClientLen)))
        {
            return FALSE;
        }
        wOutLen = ntohs(m_wClientLen);
        memcpy(pbyOutBuf,m_abyClient,wOutLen);
        return TRUE;
    }

    BOOL SetAcctNum(IN u8 *pbyBuf, IN u16 wBufLen)
    {
        if((NULL == pbyBuf)||(wBufLen > MAX_CLIENT_ACCT_LEN))
        {
            return FALSE;
        }
        memcpy(m_abyAcctNum,pbyBuf,wBufLen);
        m_wAcctNumLen = htons(wBufLen);
        return TRUE;
    }
    
    BOOL GetAcctNum(IN OUT u8 *pbyOutBuf, IN u16 wBufLen, OUT u16 &wOutLen)
    {
        if((NULL == pbyOutBuf)||(wBufLen < ntohs(m_wAcctNumLen)))
        {
            return FALSE;
        }
        wOutLen = ntohs(m_wAcctNumLen);
        memcpy(pbyOutBuf,m_abyAcctNum,wOutLen);
        return TRUE;
    }
    

    void SetTimeRange(u32 dwTimeFrom, u32 dwTimeTo)
    {
        m_dwTimeFrom = htonl(dwTimeFrom);
        m_dwTimeTo = htonl(dwTimeTo);
    }    

    void GetTimeRange(u32 &dwTimeFrom, u32 &dwTimeTo)
    {
        dwTimeFrom = ntohl(m_dwTimeFrom);
        dwTimeTo = ntohl(m_dwTimeTo);
    }

    //设置查询选项
    void QueryByClientName(BOOL bUse)
    {
        m_bClientUsed = htonl(bUse);
    }

    void QueryByAcctNum(BOOL bUse)
    {
        m_bAcctNumUsed = htonl(bUse);
    }    

    void QueryByTimeRange(BOOL bUse)
    {
        m_bTimeUsed = htonl(bUse);
    }

    void QueryByCRId(BOOL bUse)
    {
        m_bCRIdUsed = htonl(bUse);
    }

    //获取查询选项
    BOOL IsQueryByClientName()
    {
        return ntohl(m_bClientUsed);
    }

    BOOL IsQueryByAcctNum()
    {
        return ntohl(m_bAcctNumUsed);
    }
    
    BOOL IsQueryByCRId()
    {
        return ntohl(m_bCRIdUsed);
    }

    BOOL IsQueryByTimeRange()
    {
        return ntohl(m_bTimeUsed);
    }


}TCtrlCRQueryOpt;

//控制台会议信息结构
typedef struct tagCtrlConfInfo
{
    u8 m_abyConfName[MAX_CTRL_CONF_NAME_LEN];    //会议名称
    u16 m_wConfNameLen;
    u8 m_abyConfID[MAX_CTRL_CONF_ID_LEN];         //会议ID(E164号)
    u16 m_wConfIDLen;
    u32 m_dwSiteRate;                     //速率
    u16 m_wTermNum;                       //终端数
    u32 m_dwStartTime;                    //开始时间
    u32  m_dwStopTime;                    //结束时间
    u16  m_wMulVoiceNum;                  //混音数
    u16  m_wMulPicNum;                    //多画面数
    u16  m_wRateAdapNum;                  //速率适配数
    u16  m_wCascade;                      //是否级联
    u16  m_wEncrypt;                      //是否加密
    u16  m_wDualVideo;                    //是否开启双流
    u16  m_wDataConf;                     //是否召开数据会议
    u16  m_wStream;                       //是否开启流媒体服务（组播）

    tagCtrlConfInfo()
    {
        memset(m_abyConfName,0,sizeof(m_abyConfName));
        m_wConfNameLen = 0;
        memset(m_abyConfID,0,sizeof(m_abyConfID));
        m_wConfIDLen = 0;
        m_dwSiteRate = 0;
        m_wTermNum = 0;
        m_dwStartTime = 0;
        m_dwStopTime = 0;
        m_wMulVoiceNum = 0;
        m_wMulPicNum = 0;
        m_wRateAdapNum = 0;
        m_wCascade = 0;
        m_wEncrypt = 0;        
        m_wDualVideo = 0;
        m_wDataConf = 0;
        m_wStream = 0;
    }

    BOOL SetConfName(IN u8* pBuf, IN u16 wBufLen)
    {
        if((NULL == pBuf)||(wBufLen > MAX_CTRL_CONF_NAME_LEN))
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

    BOOL SetConfID(IN u8* pBuf, IN u16 wBufLen)
    {
        if((NULL == pBuf)||(wBufLen > MAX_CTRL_CONF_ID_LEN))
        {
            return FALSE;
        }
        memcpy(m_abyConfID,pBuf,wBufLen);
        m_wConfIDLen = htons(wBufLen);
        return TRUE;
    }

    BOOL GetConfID(IN OUT u8 *pBuf, IN u16 wBufLen, OUT u16 &wOutLen)
    {
        if((wBufLen < ntohs(m_wConfIDLen))||(NULL == pBuf))
        {
            wOutLen = 0;
            return FALSE;
        }
        else
        {
            wOutLen = ntohs(m_wConfIDLen);
            memcpy(pBuf,m_abyConfID,wOutLen);
            return TRUE;
        }
    }

    void SetSiteRate(u32 dwSiteRate)
    {
        m_dwSiteRate = htonl(dwSiteRate);
    }

    void SetStartTime(u32 dwStartTime)
    {
        m_dwStartTime = htonl(dwStartTime);
    }

    void SetStopTime(u32 dwStopTime)
    {
        m_dwStopTime = htonl(dwStopTime);
    }

    void SetTermNum(u16 mTermNum)
    {
        m_wTermNum = htons(mTermNum) ;
    }

    void SetMulVoiceNum(u16 mMulVoiceNum)
    {
        m_wMulVoiceNum = htons(mMulVoiceNum)  ;
    }

    void SetMulPicNum(u16 mMulPicNum)
    {
        m_wMulPicNum = htons(mMulPicNum)  ;
    }

    void SetRateAdapNum(u16 mRateAdapNum)
    {
        m_wRateAdapNum = htons(mRateAdapNum)  ;
    }

    void SetCascade(u16 mCascade)
    {
        m_wCascade = htons(mCascade);
    }

    void SetEncrypt(u16 mEncrypt)
    {
        m_wEncrypt = htons(mEncrypt);
    }

//    void SetMulticast(u16 mMulticast)
//    {
//        m_wMulticast = mMulticast  ;
//    }

    void SetDualVideo(u16 mDualVideo)
    {
        m_wDualVideo = htons(mDualVideo);
    }

    void SetDataConf(u16 mDataConf)
    {
        m_wDataConf = htons(mDataConf)  ;
    }
    
    void SetStream(u16 mStream)
    {
         m_wStream =htons(mStream)  ;
    }

    u32 GetSiteRate()
    {
        return ntohl(m_dwSiteRate);
    }

    u32 GetStartTime()
    {
        return ntohl(m_dwStartTime);
    }

    u32 GetStopTime()
    {
        return ntohl(m_dwStopTime);
    }

    u16 GetTermNum()
    {
        return ntohs(m_wTermNum);
    }

    u16 GetMulVoiceNum()
    {
        return ntohs(m_wMulVoiceNum);
    }

    u16 GetMulPicNum()
    {
        return ntohs(m_wMulPicNum);
    }

    u16 GetRateAdapNum()
    {
        return ntohs(m_wRateAdapNum);
    }

    u16 GetCascade()
    {
        return ntohs(m_wCascade);
    }

    u16 GetEncrypt()
    {
        return ntohs(m_wEncrypt);
    }

    u16 GetDualVideo()
    {
        return ntohs(m_wDualVideo);
    }

    u16 GetDataConf()
    {
        return ntohs(m_wDataConf);
    }
    
    u16 GetStream()
    {
        return ntohs(m_wStream);
    }

}TCtrlConfInfo;

//系统配置信息
typedef struct tagCtrlSysCfgInfo
{
    u16  m_wCtrlPort;      //控制台监听端口
    u16  m_wAcesPort;      //认证端口
    u16  m_wAcctPort;      //计费端口
    u8   m_abyVerInfo[MAX_CTRL_VER_LEN];  //版本信息
    u16  m_wVerLen;

    tagCtrlSysCfgInfo()
    {
		memset(&m_abyVerInfo,0,sizeof(m_abyVerInfo));        
        m_wVerLen = htons(0);
        m_wCtrlPort = htons(0);
        m_wAcesPort = htons(0);
        m_wAcctPort = htons(0);
    }

    BOOL SetVersionInfo(IN u8 *pInBuf, IN u16 wInLen)
    {
        if((NULL == pInBuf) || (wInLen > MAX_CTRL_VER_LEN))
        {
            return FALSE;
        }
        memcpy(m_abyVerInfo,pInBuf,wInLen);
        m_wVerLen = htons(wInLen);
        return TRUE;
    }

    BOOL GetVersionInfo(IN OUT u8 *pBuf, IN u16 wBufLen, OUT u16 &wOutLen)
    {
        if((NULL == pBuf)||(wBufLen < ntohs(m_wVerLen)))
        {
            wOutLen = 0;
            return FALSE;
        }
        wOutLen = ntohs(m_wVerLen);
        memcpy(pBuf,m_abyVerInfo,wOutLen);
        return TRUE;
    }

    void SetAccessPort(u16 wAccessPort)
    {
        m_wAcesPort = htons(wAccessPort);
    }

    void SetAcctPort(u16 wAcctPort)
    {
        m_wAcctPort = htons(wAcctPort);
    }

    void SetListenPort(u16 wListenPort)
    {
        m_wCtrlPort = htons(wListenPort);
    }

    u16 GetAccessPort()
    {
        return ntohs(m_wAcesPort);
    }

    u16 GetAcctPort()
    {
        return ntohs(m_wAcctPort);
    }

    u16 GetCtrlPort()
    {
        return ntohs(m_wCtrlPort);
    }

}TCtrlSysCfgInfo;

//系统使用信息
typedef struct tagCtrlSysUseInfo
{
    u8   m_byCpuUsePercent;     //CPU使用率
    u8  m_byMemUsePercent;     //内存使用率
    u32  m_dwMemAllocSize;     //已用内存数(KB)
    u32  m_dwMemPhsySize;      //物理内存总数(KB)
    u32  m_dwMemFreeSize;      //可用内存数(KB)

    tagCtrlSysUseInfo()
    {
        m_byCpuUsePercent = 0;
        m_byMemUsePercent = 0;
        m_dwMemAllocSize = 0;
        m_dwMemPhsySize = 0;
        m_dwMemFreeSize = 0;
    }

    void SetCpuUsePercent(u8 byUsePert)
    {
        m_byCpuUsePercent = byUsePert;
    }

    void SetMemUsePercent(u8 byUsePert)
    {
        m_byMemUsePercent = byUsePert;
    }

    void SetMemUsedSize(u32 dwMemUsedSize)
    {
        m_dwMemAllocSize = htonl(dwMemUsedSize);
    }

    void SetTotalMemSize(u32 dwMemTotalSize)
    {
        m_dwMemPhsySize = htonl(dwMemTotalSize);
    }

    void SetFreeMemSize(u32 dwFreeMemSize)
    {
        m_dwMemFreeSize = htonl(dwFreeMemSize);
    }

    u8 GetCpuUsePercent()
    {
        return m_byCpuUsePercent;
    }
    u8 GetMemUsePercent()
    {
        return m_byMemUsePercent;
    }
    u32 GetUsedMemSize()
    {
        return ntohl(m_dwMemAllocSize);
    }
    u32 GetTotalMemSize()
    {
        return ntohl(m_dwMemPhsySize);
    }
    u32 GetFreeMemSize()
    {
        return ntohl(m_dwMemFreeSize);
    }
}TCtrlSysUseInfo;

//数据库连接信息
typedef struct tagCtrlDBContInfo
{
    u8  m_abyDBIP[MAX_CTRL_IP_LEN];    //数据库IP地址
    u16  m_wIpLen;
    u16  m_wDBListenPort;          //数据库监听端口
    u16  m_wConStatus;            //连接状态： 0/未连接 1/已连接   
    
    tagCtrlDBContInfo()
    {
        memset(m_abyDBIP,0,sizeof(m_abyDBIP));
        m_wIpLen = 0;
        m_wDBListenPort = 0;
        m_wConStatus = 0;
    }

    BOOL SetDBIP(u8 *pbyInBuf, u16 wInLen)
    {
        if((NULL == pbyInBuf) || (wInLen > MAX_CTRL_IP_LEN))
        {
            return FALSE;
        }
        memcpy(m_abyDBIP,pbyInBuf,wInLen);
        m_wIpLen = htons(wInLen);
        return TRUE;
    }

    BOOL GetDBIP(IN OUT u8 *pBuf, IN u16 wBufLen, OUT u16 &wOutLen)
    {
        if((NULL == pBuf)||(wBufLen < ntohs(m_wIpLen)))
        {
            wOutLen = 0;
            return FALSE;
        }
        wOutLen = ntohs(m_wIpLen);
        memcpy(pBuf,m_abyDBIP,wOutLen);
        return TRUE;
    }

    void SetDBListenPort(u16 wPort)
    {
        m_wDBListenPort = htons(wPort);
    }

    void SetDBContStatus(u16 wStatus)
    {
        m_wConStatus = htons(wStatus);
    }

    u16 GetDBListenPort()
    {
        return ntohs(m_wDBListenPort);
    }

    u16 GetDBContStatus()
    {
        return ntohs(m_wConStatus);
    }

}TCtrlDBContInfo;

//控制台系统状态信息结构
typedef struct tagCtrlSysStatus
{
    TCtrlSysCfgInfo m_tCfgInfo;    //系统配置信息
    TCtrlSysUseInfo m_tUseInfo;    //系统使用信息
    TCtrlDBContInfo m_tDBInfo;     //数据库连接信息
}TCtrlSysStatus;



//add by gzj.070914.
//详细话单结构
typedef struct tagCtrlDetailCRInfo
{
    u32 m_dwCRId;                                 //话单编号     
    u8  m_abyClientName[MAX_CLIENT_NAME_LEN];     //客户名称
    u16 m_wClientLen;
    u8 m_abyAcctNum[MAX_CLIENT_ACCT_LEN];         //帐号
    u16 m_wAcctLen;
    u32 m_dwCRTime;                               //记录时间
    u32 m_dwServType;                             //服务类型
    u32 m_dwStartTime;                            //开始时间
    u32 m_dwStopTime;                             //结束时间
    u32 m_dwFee;                                  //总费用

	u32 m_dwBWFee;                        //带宽费用
    u32 m_dwTermFee;                       //终端费用
    u32  m_dwMulVoiceFee;                  //混音费用
    u32  m_dwMulPicFee;                    //多画面费用
    u32  m_dwRateAdapFee;                  //速率适配费用
    u32  m_dwCascadeFee;                   //级联费用
    u32  m_dwEncryptFee;                   //加密费用
    u32  m_dwDualVideoFee;                 //双流费用
    u32  m_dwDataConfFee;                  //数据会议费用
    u32  m_dwStreamFee;                    //流媒体服务（组播）费用
	

    tagCtrlDetailCRInfo()
    {
        m_dwCRId = 0;
        memset(m_abyClientName,0,sizeof(m_abyClientName));
        m_wClientLen = 0;
        memset(m_abyAcctNum,0,sizeof(m_abyAcctNum));
        m_wAcctLen = 0;
        m_dwCRTime = 0;
        m_dwServType = 0;
        m_dwStartTime = 0;
        m_dwStopTime = 0;
        m_dwFee = 0;		

		m_dwBWFee = 0;
		m_dwTermFee = 0;
		m_dwMulVoiceFee = 0;
		m_dwMulPicFee = 0;
		m_dwRateAdapFee = 0;
		m_dwCascadeFee = 0;
		m_dwEncryptFee = 0;
		m_dwDualVideoFee = 0;
		m_dwDataConfFee = 0;
		m_dwStreamFee = 0;
		
	}
    
     //话单编号   
    void SetCRID(u32 dwCRID)
    {
        m_dwCRId = htonl(dwCRID);
    }
    
    u32 GetCRID()
    {
        return ntohl(m_dwCRId);
    }
     
     //客户名称
     BOOL SetClientName(IN u8* pInBuf, IN u16 wBufLen)
     {
         if((NULL == pInBuf) || (wBufLen >MAX_CLIENT_NAME_LEN))
         {
             return FALSE;
         }
         memcpy(m_abyClientName,pInBuf,wBufLen);
         m_wClientLen = htons(wBufLen);

         return TRUE;
     }

     BOOL GetClientName(IN OUT u8* pOutBuf, IN u16 wBufLen, OUT u16 &wOutLen)
     {
         u16 wRealLen = ntohs(m_wClientLen);
         if((NULL == pOutBuf) ||(wBufLen < wRealLen))
         {
             wOutLen = 0;
             return FALSE;
         }
         memcpy(pOutBuf,m_abyClientName,wRealLen);
         wOutLen = wRealLen;
         return TRUE;         
     }

     //帐号名称
     BOOL SetAcctNum(IN u8* pInBuf, IN u16 wBufLen)
     {
         if((NULL == pInBuf) || (wBufLen >MAX_CLIENT_ACCT_LEN))
         {
             return FALSE;
         }
         memcpy(m_abyAcctNum,pInBuf,wBufLen);
         m_wAcctLen = htons(wBufLen);

         return TRUE;
     }

     BOOL GetAcctNum(IN OUT u8* pOutBuf, IN u16 wBufLen, OUT u16 &wOutLen)
     {
         u16 wRealLen = ntohs(m_wAcctLen);
         if((NULL == pOutBuf) ||(wBufLen < wRealLen))
         {
             wOutLen = 0;
             return FALSE;
         }
         memcpy(pOutBuf,m_abyAcctNum,wRealLen);
         wOutLen = wRealLen;
         return TRUE;         
     }

     //记录时间
     void SetCRTime(u32 dwCRTime)
     {
         m_dwCRTime = htonl(dwCRTime);
     }
     u32 GetCRTime()
     {
         return ntohl(m_dwCRTime);
     }

     void SetStartTime(u32 dwStartTime)
     {
         m_dwStartTime = htonl(dwStartTime);
     }
     u32 GetStartTime()
     {
         return ntohl(m_dwStartTime);
     }

     void SetStopTime(u32 dwStopTime)
     {
         m_dwStopTime = htonl(dwStopTime);
     }
     u32 GetStopTime()
     {
         return ntohl(m_dwStopTime);
     }

     void SetServiceType(u32 dwServType)
     {
         m_dwServType = htonl(dwServType);
     }
     u32 GetServiceType()
     {
         return ntohl(m_dwServType);
     }

     void SetFee(u32 dwFee)
     {
         m_dwFee = htonl(dwFee);
     }
 
     u32 GetFee() //取总费用
     {
         return ntohl(m_dwFee);
     }

	 //以下是取分项费用接口
	u32 GetBwFee()
	{
		return ntohl(m_dwBWFee);
	}

	u32 GetTermFee()
	{
		return ntohl(m_dwTermFee);
	}
	u32 GetMulVoiceFee()
	{
		return ntohl(m_dwMulVoiceFee);
	}
	u32 GetMulPicFee()
	{
		return ntohl(m_dwMulPicFee);
	}
	u32 GetRateAdptFee()
	{
		return ntohl(m_dwRateAdapFee);
	}
	u32 GetCascadeFee()
	{
		return ntohl(m_dwCascadeFee);
	}
	u32 GetEncptFee()
	{
		return ntohl(m_dwEncryptFee);
	}
	u32 GetDualVideoFee()
	{
		return ntohl(m_dwDualVideoFee);
	}
	u32 GetDCFee()
	{
		return ntohl(m_dwDataConfFee);
	}
	u32 GetStreamFee()
	{
		return ntohl(m_dwStreamFee);
	}

    
}TCtrlDetailCRInfo;










#endif   //_KDVRS_H
