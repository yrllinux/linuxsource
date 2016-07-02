/*****************************************************************************
模块名      : KDV GK
文件名      : KDVGK.h
相关文件    : 
文件实现功能: 
作者        : 樊雷
版本        : V4.0  Copyright(C) 2004-2005 KDC, All rights reserved.
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2005-02-17  4.0         樊雷         创建
2006-11-16  4.0         郭忠军       提交会议计费代码
2007-02-28  4.0         郭忠军       Davinci 内置GK
******************************************************************************/

#ifndef __KDVGK_H
#define __KDVGK_H

#include "osp.h"
#include "kdvdef.h"
#include "eventid.h"
#include "cm.h"
#include "stkutils.h"

#ifdef _WINDOWS_
#include <io.h>
#include <direct.h>
#include <windef.h>
#endif // _WINDOWS_

#ifdef _VXWORKS_
#include "usrlib.h"
#include "Dirent.h"
#endif // _VXWORKS_

//////////////////////////////// GK APP ID ///////////////////////////////
#define AID_GK_STACKINIT		        (AID_GKS_BGN + 1)
#define AID_GK_RASOPERATION	            (AID_GKS_BGN + 2)
#define AID_GK_CONTROLSERVER            (AID_GKS_BGN + 3)
#define AID_GK_RADIUSCLIENT             (AID_GKS_BGN + 4)

//gk 连接 interworking APPID
#define AID_GK_INTERWORKING             (AID_GKS_BGN + 6)

////////////////////////////////// 宏定义 ////////////////////////////////
// 长度定义
#define CALLPORT			            (u16)1720	//呼叫端口
#define RASPORT				            (u16)1719	//RAS端口
#define MULTIRASPORT                    (u16)1718   //RAS多播端口
#define MAX_REGISTER_NUM                (u16)3000   //最大实体注册数
#define MAX_ALIAS_NUM		            (u16)128	//最大别名数

//add by yj for support all kinds of pcmts [20120924]
#define MAX_LEN_EPNAME					(u16)128	//最大授权实体名长度
#define MAX_PCMT_AUTH_NUM				(u16)16		//最大授权实体种类数		
//end [20120924]

#if defined(_DAVINCI_)
#define MAX_CALL_NUM                    (u16)14      //最大呼叫数(内置GK仅需支持6方)
#else
#define MAX_CALL_NUM                    (u16)528    //最大呼叫数
#endif
      
#define MAX_ENTITYALIAS_NUM             (u16)4      //实体认证中实体最大别名数      
#define MAX_CHANNEL_NUM                 (u16)0      //最大通道数
#define MAX_KEEPALIVE_TIME              (s32)60     //RRQ注册有效时间，单位：秒
#define MAX_CALLADDR_NUM                (u16)8      //最大呼叫信令地址数
#define MAX_ENCODERAS_LEN               (u16)0x1000 //存放压缩后的RAS消息的buf长度
#define MAX_E164_LEN                    (u16)128	//E164别名的最大长度
#define MAX_ALIASNAME_LEN               (u16)512	//其他别名的最大长度

//GK支持UTF8转码 [20130712]
#define MAX_ALIASNAME_UTF8_LEN          (u16)768	//其他别名的最大长度(MAX_ALIASNAME_LEN*3\2)
//end [20130712]

#define MAX_BUFFER_SIZE                 (u32)4096	//协议栈的最大发送缓冲
#define KDC_COUNTRYCODE		            (s32)38		//国家代码
#define KDC_T35EXTENSION	            (s32)0		//扩展代码
#define KDC_MANUCODE		            (s32)8888	//厂商代码
#define CONFRESPONSE_TIMEOUT            (u32)15     //15s
#define CONFCONNECT_TIMEOUT             (u32)50     //50s 
#define LEN_OID				            (u16)16		//OID长度
#define LEN_GUID                        (u16)16
#define LEN_256                         (u16)256
#define MAX_USER_LOGIN                  (u16)16     //最大用户登陆数
#define MAX_USERNAME_LEN                (u16)32     //最大用户名长度
#define MAX_PASSWORD_LEN                (u16)16     //最大密码长度
#define MAX_GKPREFIX_NUM                (u16)16     //最大前缀数
#define MAX_GKPREFIX_LEN                (u16)8      //前缀最大长度
#define MAX_AUTHIPRANGE_NUM             (u16)16     //IP认证范围的最大个数
#define MAX_BANDWIDTH_NUM               (u16)16     //网段带宽信息控制的最大个数
#define MAX_NEIGHBORGK_NUM              (u16)128    //最大邻居GK数
#define IRR_FREQUENCY                   (u16)30     //60s
#define TIMER_ALLOWOFFSET               (u16)10000   //定时器允许偏差10s
#define HBTIME                          (u16)60     //60s
#define HBNUM                           (u16)3

//add by yj for 460server
#define MAX_CONF_NUM					(u16)32                 //最大会议数
#define LEN_ALIAS						(u16)64				    //最大别名长度h323-id
#define LEN_1							(u16)1
#define MAX_SERVERCALL_NUM				(u16)128				//服务器最大支持呼叫数
#define SINGLECALL_CHANNUM				(u16)5					//h323通道数
#define SERVER_SINGLECHAN_PROTNUM		(u16)4					//服务器单个通道所分配的端口数
#define	Max_460CallPORT_NUM				(u16)(SINGLECALL_CHANNUM*SERVER_SINGLECHAN_PROTNUM*2)	//对应一个呼叫对(双向)的所有媒体通道的端口:单个通道数*单个通道所使用端口数*2
//end

//radius 认证/计费相关宏定义
#define MAX_RADIU_SECRET_LEN            (u16)16     //radius secret 的最大长度 
#define DEFAULT_RADIUS_SECRET           (s8*)"kdc"  //默认radius密钥
#define DEFAULT_ACCESS_PORT             (u16)1812   //默认认证端口
#define DEFAULT_ACCT_PORT               (u16)1813   //默认计费端口
#define DEFAULT_RETRY_TIME              (u16)1      //默认重发次数
#define DEFAULT_TIMEOUT_INTERVAL        (u32)10000   //默认超时间隔(毫秒)



//日志级别定义
#define LOG_EXP		        	        (u32)0		//异常
#define LOG_DEBUG	        	        (u32)1		//系统调试信息

#define LOG_REGISTER			        (u32)2		//RRQ/RCF/RRJ
#define LOG_ADMISSION		            (u32)4		//ARQ/ACF/ARJ
#define LOG_DISENGAGE                   (u32)6      //DRQ/DCF/DRJ 
#define LOG_UNREGISTER                  (u32)8      //URQ/UCF/URJ
#define LOG_INFORMATION                 (u32)10     //IRQ/IRR
#define LOG_BANDWIDTH                   (u32)12     //BRQ/BCF/BRJ
#define LOG_LOCATION                    (u32)14     //LRQ/LCF/LRJ
#define LOG_GATEKEEPER                  (u32)16     //GRQ/GCG/GRJ

#define LOG_DATABASE_DEBUG              (u32)20     //数据库操作的调试信息
#define LOG_CONTRLSERVER_DEBUG          (u32)21     //控制台操作的调试信息
#define LOG_DATABASE_ALL                (u32)22     //所有数据库操作的信息
#define LOG_CONTRLSERVER_ALL            (u32)23     //所有控制台操作的信息

#define LOG_ALL		        	        (u32)30     //所有信息

//GK控制台消息响应
#define CTRLMSGRSP_SUCCESS				(u32)0      // 成功
#define CTRLMSGRSP_UNKOWNERR 			(u32)1      // 未知错误
#define CTRLMSGRSP_INVALIDCTRLVER		(u32)2      // 控制台版本错误
#define CTRLMSGRSP_PARAMCONFLICT		(u32)3      // 参数不一致
#define CTRLMSGRSP_ACCESSDENIED			(u32)4      // 权限不允许
#define CTRLMSGRSP_USERNOTEXIST			(u32)5      // 用户名不存在
#define CTRLMSGRSP_INVALIDPASS			(u32)6      // 密码非法
#define CTRLMSGRSP_USEREXIST			(u32)7      // 用户已存在
#define CTRLMSGRSP_INVALIDUSER			(u32)8      // 用户名非法
#define CTRLMSGRSP_INVALIDDSTUSER		(u32)9      // 目标用户名非法
#define CTRLMSGRSP_INVALIDDSTPASS		(u32)10     // 目标密码非法
#define CTRLMSGRSP_INVALIDCALLID		(u32)11     // 非法call identifier
#define CTRLMSGRSP_INVALIDIPRNG			(u32)12     // 非法ip范围
#define CTRLMSGRSP_IPRNGEXIST			(u32)13     // ip范围已存在
#define CTRLMSGRSP_IPRNGNOTEXIST		(u32)14     // ip范围不存在
#define CTRLMSGRSP_EPNOTREG				(u32)15     // H.323实体没有注册
#define CTRLMSGRSP_ALIASEXIST			(u32)16     // E164号已存在
#define CTRLMSGRSP_INVALIDALIAS			(u32)17     // 非法alias
#define CTRLMSGRSP_INVALIDIP			(u32)18     // 非法ip
#define CTRLMSGRSP_EPNOTEXIST			(u32)19     // endpoint不存在
#define CTRLMSGRSP_INVALIDEPTYPE		(u32)20     // endpoint type非法
#define CTRLMSGRSP_INVALIDPARAM			(u32)21     // 参数非法
#define CTRLMSGRSP_INVALIDMSGBD			(u32)22     // 非法消息体，解包出现错误
#define CTRLMSGRSP_DBERROR				(u32)23     // DB出错
#define CTRLMSGRSP_ENCODEERR			(u32)24     // 编码出错
#define CTRLMSGRSP_DECODEERR			(u32)25     // 解码出错
#define CTRLMSGRSP_INVALIDUS			(u32)26     // 非法用户设置
#define CTRLMSGRSP_TOOSMALLBUF			(u32)27     // 缓冲区太小
#define CTRLMSGRSP_NORECORD				(u32)28     // 没有记录
#define CTRLMSGRSP_MAXUSER				(u32)29     // 达到控制台用户上限
#define CTRLMSGRSP_USERLOGINED			(u32)30     // 用户已登录
#define CTRLMSGRSP_USERNOTLOGIN			(u32)31     // 用户未登录
#define CTRLMSGRSP_ADMINFORBIDDEL		(u32)32     // 管理员用户不能删除
#define CTRLMSGRSP_BWEXIST				(u32)33     // 网段间带宽已存在
#define CTRLMSGRSP_BWNOTEXIST			(u32)34     // 网段间带宽不存在
#define CTRLMSGRSP_KEYEXIST				(u32)35     // 存在
#define CTRLMSGRSP_KEYNOTEXIST			(u32)36     // 不存在
#define CTRLMSGRSP_DSTKEYEXIST			(u32)37     // 目的key已存在 
#define CTRLMSGRSP_IPRNGERROR           (u32)38     // ip范围不对
#define CTRLMSGRSP_EPALIASLEN_INVALID	(u32)39     // 实体别名长度非法
#define CTRLMSGRSP_ENTITYEXIST          (u32)40     //实体已存在     //[12/27/2007 zhangzhihua]add 
#define CTRLMSGRSP_ERRVERSION           (u32)41     //GKC版本不匹配. //[12/28/2007 zhangzhihua]add 

//GKC版本号
#define GKC_VESION_NUM                   (u32)10000  //[12/28/2007 zhangzhihua]add.GKC版本号. 

//GK配置数据库的存放路径和命名字
#if defined (WIN32)
#define MakeDirectory(dirname) _mkdir((dirname))
#elif defined (_VXWORKS_)
#define MakeDirectory(dirname) mkdir(dirname)
#elif defined (_LINUX_)
#define MakeDirectory(dirname) mkdir(dirname, 0777)
#else
#error "This version only support windows and vxworks and linux"
#endif // _WINDOWS_

// #if defined(_HHPPC_82XX_) || defined(_HHPPC_85XX_)
#if defined(_LINUX_)

#define GKDB_PATH                   (s8*)"/usr/etc/gk/GKDB"
#define GKCONFIGDB_PATH             (s8*)"/usr/etc/gk/GKDB/GKConfigDB"

#define USERDB_PATH_NAME            (s8*)"/usr/etc/gk/GKDB/GKConfigDB/UserDB.db"
#define BASECONFIGDB_PATH_NAME      (s8*)"/usr/etc/gk/GKDB/GKConfigDB/BaseConfigDB.db"
#define BANDWIDTHDB_PATH_NAME       (s8*)"/usr/etc/gk/GKDB/GKConfigDB/BandwidthDB.db"
#define IPRANGEDB_PATH_NAME         (s8*)"/usr/etc/gk/GKDB/GKConfigDB/IPRangeDB.db"
#define ALIASDB_PATH_NAME           (s8*)"/usr/etc/gk/GKDB/GKConfigDB/EndpointAliasDB.db"
#define ENTITYDB_PATH_NAME            (s8*)"/usr/etc/gk/GKDB/GKConfigDB/EntityDB.db"         //[12/26/2007 zhangzhihua]add 

#else

#define GKDB_PATH                   (s8*)"GKDB"
#define GKCONFIGDB_PATH             (s8*)"GKDB/GKConfigDB"

#define USERDB_PATH_NAME            (s8*)"GKDB/GKConfigDB/UserDB"
#define BASECONFIGDB_PATH_NAME      (s8*)"GKDB/GKConfigDB/BaseConfigDB"
#define BANDWIDTHDB_PATH_NAME       (s8*)"GKDB/GKConfigDB/BandwidthDB"
#define IPRANGEDB_PATH_NAME         (s8*)"GKDB/GKConfigDB/IPRangeDB"
#define ALIASDB_PATH_NAME           (s8*)"GKDB/GKConfigDB/EndpointAliasDB"
#define ENTITYDB_PATH_NAME            (s8*)"GKDB/GKConfigDB/EntityDB"    //[12/26/2007 zhangzhihua]add 

#endif

#ifndef min
#define min(a,b) (((a)<(b))?(a):(b))
#endif
//////////////////////////////// OSP消息范围 /////////////////////////////
//////// RAS OSP消息范围: (GKRASOSPEVENT_START, GKRASOSPEVENT_END)////////
OSPEVENT( GKRASOSPEVENT_START, EV_RAS_BGN );

// 收到RRQ
OSPEVENT( EV_RAS_RECV_RRQ, GKRASOSPEVENT_START + 1 );
// 发送RRQ
OSPEVENT( EV_RAS_SEND_RRQ, GKRASOSPEVENT_START + 2 );

// 收到RCF
OSPEVENT( EV_RAS_RECV_RCF, GKRASOSPEVENT_START + 3 );
// 发送RCF
OSPEVENT( EV_RAS_SEND_RCF, GKRASOSPEVENT_START + 4 );

// 收到RRJ
OSPEVENT( EV_RAS_RECV_RRJ, GKRASOSPEVENT_START + 5 );
// 发送RRJ
OSPEVENT( EV_RAS_SEND_RRJ, GKRASOSPEVENT_START + 6 );

// 收到URQ
OSPEVENT( EV_RAS_RECV_URQ, GKRASOSPEVENT_START + 7 );
// 发送URQ
OSPEVENT( EV_RAS_SEND_URQ, GKRASOSPEVENT_START + 8 );

// 收到UCF
OSPEVENT( EV_RAS_RECV_UCF, GKRASOSPEVENT_START + 9 );
// 发送UCF
OSPEVENT( EV_RAS_SEND_UCF, GKRASOSPEVENT_START + 10 );

// 收到URJ
OSPEVENT( EV_RAS_RECV_URJ, GKRASOSPEVENT_START + 11 );
// 发送URJ
OSPEVENT( EV_RAS_SEND_URJ, GKRASOSPEVENT_START + 12 );

// 收到ARQ
OSPEVENT( EV_RAS_RECV_ARQ, GKRASOSPEVENT_START + 13 );
// 发送ARQ
OSPEVENT( EV_RAS_SEND_ARQ, GKRASOSPEVENT_START + 14 );

// 收到ACF
OSPEVENT( EV_RAS_RECV_ACF, GKRASOSPEVENT_START + 15 );
// 发送ACF
OSPEVENT( EV_RAS_SEND_ACF, GKRASOSPEVENT_START + 16 );

// 收到ARJ
OSPEVENT( EV_RAS_RECV_ARJ, GKRASOSPEVENT_START + 17 );
// 发送ARJ
OSPEVENT( EV_RAS_SEND_ARJ, GKRASOSPEVENT_START + 18 );

// 收到BRQ
OSPEVENT( EV_RAS_RECV_BRQ, GKRASOSPEVENT_START + 19 );
// 发送BRQ
OSPEVENT( EV_RAS_SEND_BRQ, GKRASOSPEVENT_START + 20 );

// 收到BCF
OSPEVENT( EV_RAS_RECV_BCF, GKRASOSPEVENT_START + 21 );
// 发送BCF
OSPEVENT( EV_RAS_SEND_BCF, GKRASOSPEVENT_START + 22 );

// 收到BRJ
OSPEVENT( EV_RAS_RECV_BRJ, GKRASOSPEVENT_START + 23 );
// 发送BRJ
OSPEVENT( EV_RAS_SEND_BRJ, GKRASOSPEVENT_START + 24 );

// 收到LRQ
OSPEVENT( EV_RAS_RECV_LRQ, GKRASOSPEVENT_START + 25 );
// 发送LRQ
OSPEVENT( EV_RAS_SEND_LRQ, GKRASOSPEVENT_START + 26 );

// 收到LCF
OSPEVENT( EV_RAS_RECV_LCF, GKRASOSPEVENT_START + 27 );
// 发送LCF
OSPEVENT( EV_RAS_SEND_LCF, GKRASOSPEVENT_START + 28 );

// 收到LRJ
OSPEVENT( EV_RAS_RECV_LRJ, GKRASOSPEVENT_START + 29 );
// 发送LRJ
OSPEVENT( EV_RAS_SEND_LRJ, GKRASOSPEVENT_START + 30 );

// 收到DRQ
OSPEVENT( EV_RAS_RECV_DRQ, GKRASOSPEVENT_START + 31 );
// 发送DRQ
OSPEVENT( EV_RAS_SEND_DRQ, GKRASOSPEVENT_START + 32 );

// 收到DCF
OSPEVENT( EV_RAS_RECV_DCF, GKRASOSPEVENT_START + 33 );
// 发送DCF
OSPEVENT( EV_RAS_SEND_DCF, GKRASOSPEVENT_START + 34 );

// 收到DRJ
OSPEVENT( EV_RAS_RECV_DRJ, GKRASOSPEVENT_START + 35 );
// 发送DRJ
OSPEVENT( EV_RAS_SEND_DRJ, GKRASOSPEVENT_START + 36 );

// 收到IRQ
OSPEVENT( EV_RAS_RECV_IRQ, GKRASOSPEVENT_START + 37 );
// 发送IRQ
OSPEVENT( EV_RAS_SEND_IRQ, GKRASOSPEVENT_START + 38 );

// 收到IRR
OSPEVENT( EV_RAS_RECV_IRR, GKRASOSPEVENT_START + 39 );
// 发送IRR
OSPEVENT( EV_RAS_SEND_IRR, GKRASOSPEVENT_START + 40 );

// 收到GRQ
OSPEVENT( EV_RAS_RECV_GRQ, GKRASOSPEVENT_START + 41 );
// 发送GCF
OSPEVENT( EV_RAS_SEND_GCF, GKRASOSPEVENT_START + 42 );
// 发送GRJ
OSPEVENT( EV_RAS_SEND_GRJ, GKRASOSPEVENT_START + 43 );

OSPEVENT( EV_RAS_RECV_IRQ_TIMEOUT, GKRASOSPEVENT_START + 44 );

// 收到Unsolicite IRR
OSPEVENT( EV_RAS_RECV_UNSOLICITED_IRR, GKRASOSPEVENT_START + 45);

//add by yj for NonStandardMessage [20120823]
//RAS非标消息(46,47,49被会商占用)
OSPEVENT(EV_RAS_RECV_NSM, GKRASOSPEVENT_START +48);
//end [20120823]

OSPEVENT( GKRASOSPEVENT_END, EV_RAS_END );

//////////// GK系统消息范围: (GKOSPEVENT_START, GKSYSOSPEVENT_END)////////
OSPEVENT( GKSYSOSPEVENT_START, GKOSPEVENT_START );

// GK初始化
OSPEVENT( EV_GKSYS_INIT, GKSYSOSPEVENT_START + 1 ); 

// GK退出
OSPEVENT( EV_GKSYS_QUIT, GKSYSOSPEVENT_START + 2 );

// 系统秒消息，用于定时器
OSPEVENT( EV_TIMER_ONE_SECOND, GKSYSOSPEVENT_START + 3 );

// Watchdog注册消息
OSPEVENT( WATCHDOG_CONNECT, GKSYSOSPEVENT_START + 4 );


//通知实时库更新数据库
OSPEVENT( EV_RELOAD_CONFIGDB, GKSYSOSPEVENT_START + 5 );
OSPEVENT( EV_RELOAD_IPRANGEDB, GKSYSOSPEVENT_START + 6 );
OSPEVENT( EV_RELOAD_ALIASDB, GKSYSOSPEVENT_START + 7 );
OSPEVENT( EV_RELOAD_ENTITYDB, GKSYSOSPEVENT_START + 8);

// 强制注销
OSPEVENT( EV_UNREG, GKSYSOSPEVENT_START + 10 );

// 注销全部
OSPEVENT( EV_UNREG_ALL, GKSYSOSPEVENT_START + 11 );

// 挂断呼叫
OSPEVENT( EV_DROPCALL, GKSYSOSPEVENT_START + 12 );

OSPEVENT( GKSYSOSPEVENT_END, GKSYSOSPEVENT_START + 20 );

/////GK控制台OSP消息范围: (GKCTRLOSPEVENT_START, GKCTRLOSPEVENT_END)//////
OSPEVENT( GKCTRLOSPEVENT_START, GKSYSOSPEVENT_END );

// 用户登录
OSPEVENT(EV_CG_USER_LOGIN_REQ,	GKCTRLOSPEVENT_START + 1);
// 用户登录响应
OSPEVENT(EV_GC_USER_LOGIN_RSP,	GKCTRLOSPEVENT_START + 2);

// 退出登录
OSPEVENT(EV_CG_USER_EXIT_REQ,	GKCTRLOSPEVENT_START + 3);
// 退出登录响应
OSPEVENT(EV_GC_USER_EXIT_RSP,	GKCTRLOSPEVENT_START + 4);

// 增加用户
OSPEVENT(EV_CG_USER_ADD_REQ,	GKCTRLOSPEVENT_START + 5);
// 增加用户响应
OSPEVENT(EV_GC_USER_ADD_RSP,	GKCTRLOSPEVENT_START + 6);

// 删除用户
OSPEVENT(EV_CG_USER_DEL_REQ,	GKCTRLOSPEVENT_START + 7);
// 删除用户响应
OSPEVENT(EV_GC_USER_DEL_RSP,	GKCTRLOSPEVENT_START + 8);

// 修改用户
OSPEVENT(EV_CG_USER_MODIFY_REQ,	GKCTRLOSPEVENT_START + 9);
// 修改用户响应
OSPEVENT(EV_GC_CTRLUSER_MODIFY_RSP,	GKCTRLOSPEVENT_START + 10);

// 修改密码
OSPEVENT(EV_CG_USER_MODIFYPASS_REQ,	GKCTRLOSPEVENT_START + 11);
// 修改密码响应
OSPEVENT(EV_GC_USER_MODIFYPASS_RSP,	GKCTRLOSPEVENT_START + 12);

// 获取用户信息列表
OSPEVENT(EV_CG_CTRLUSER_GETLIST_REQ,	GKCTRLOSPEVENT_START + 13);
// 获取用户信息列表响应
OSPEVENT(EV_GC_CTRLUSER_GETLIST_RSP,	GKCTRLOSPEVENT_START + 14);

// 获取一个用户
OSPEVENT(EV_CG_CTRLUSER_GETONE_REQ,	GKCTRLOSPEVENT_START + 15);
// 获取一个用户响应
OSPEVENT(EV_GC_CTRLUSER_GETONE_RSP,	GKCTRLOSPEVENT_START + 16);

// 设置GK基本配置
OSPEVENT(EV_CG_GKCFG_BASE_SET_REQ,	GKCTRLOSPEVENT_START + 17);
// 设置GK基本配置响应
OSPEVENT(EV_GC_GKCFG_BASE_SET_RSP,	GKCTRLOSPEVENT_START + 18);

// 获取GK基本配置
OSPEVENT(EV_CG_GKCFG_BASE_GET_REQ,	GKCTRLOSPEVENT_START + 19);
// 获取GK基本配置响应
OSPEVENT(EV_GC_GKCFG_BASE_GET_RSP,	GKCTRLOSPEVENT_START + 20);

// 增加GK前缀
OSPEVENT( EV_CG_GKCFG_PREFIX_ADD_REQ,	GKCTRLOSPEVENT_START + 21);
// 增加GK前缀响应
OSPEVENT( EV_GC_GKCFG_PREFIX_ADD_RSP,	GKCTRLOSPEVENT_START + 22);

// 获取GK前缀
OSPEVENT( EV_CG_GKCFG_PREFIX_GET_REQ,	GKCTRLOSPEVENT_START + 23);
// 获取GK前缀响应
OSPEVENT( EV_GC_GKCFG_PREFIX_GET_RSP,	GKCTRLOSPEVENT_START + 24);

// 删除GK前缀
OSPEVENT( EV_CG_GKCFG_PREFIX_DEL_REQ,	GKCTRLOSPEVENT_START + 25);
// 删除GK前缀响应
OSPEVENT( EV_GC_GKCFG_PREFIX_DEL_RSP,	GKCTRLOSPEVENT_START + 26);

// 增加认证IP范围
OSPEVENT(EV_CG_GKCFG_REGIPRNG_ADD_REQ,	GKCTRLOSPEVENT_START + 27);
// 增加认证IP范围响应
OSPEVENT(EV_GC_GKCFG_REGIPRNG_ADD_RSP,	GKCTRLOSPEVENT_START + 28);

// 删除认证IP范围
OSPEVENT(EV_CG_GKCFG_REGIPRNG_DEL_REQ,	GKCTRLOSPEVENT_START + 29);
// 删除认证IP范围响应
OSPEVENT(EV_GC_GKCFG_REGIPRNG_DEL_RSP,	GKCTRLOSPEVENT_START + 30);

// 修改认证IP范围
OSPEVENT(EV_CG_GKCFG_REGIPRNG_MODIFY_REQ,	GKCTRLOSPEVENT_START + 31);
// 修改认证IP范围响应
OSPEVENT(EV_GC_GKCFG_REGIPRNG_MODIFY_RSP,	GKCTRLOSPEVENT_START + 32);

// 获取认证允许IP范围列表
OSPEVENT(EV_CG_GKCFG_REGIPRNG_GETLIST_REQ,	GKCTRLOSPEVENT_START + 33);
// 获取认证允许IP范围列表响应
OSPEVENT(EV_GC_GKCFG_REGIPRNG_GETLIST_RSP,	GKCTRLOSPEVENT_START + 34);

// 增加网段间带宽信息
OSPEVENT(EV_CG_GKCFG_BETWEENSUBNETBANDINFO_ADD_REQ,	GKCTRLOSPEVENT_START + 35);
// 增加网段间带宽信息响应
OSPEVENT(EV_GC_GKCFG_BETWEENSUBNETBANDINFO_ADD_RSP,	GKCTRLOSPEVENT_START + 36);

// 删除网段间带宽信息
OSPEVENT(EV_CG_GKCFG_BETWEENSUBNETBANDINFO_DEL_REQ,	GKCTRLOSPEVENT_START + 37);
// 删除网段间带宽信息响应
OSPEVENT(EV_GC_GKCFG_BETWEENSUBNETBANDINFO_DEL_RSP,	GKCTRLOSPEVENT_START + 38);

// 修改网段间带宽信息
OSPEVENT(EV_CG_GKCFG_BETWEENSUBNETBANDINFO_MODIFY_REQ,	GKCTRLOSPEVENT_START + 39);
// 修改网段间带宽信息响应
OSPEVENT(EV_GC_GKCFG_BETWEENSUBNETBANDINFO_MODIFY_RSP,	GKCTRLOSPEVENT_START + 40);

// 获取网段间带宽信息列表
OSPEVENT(EV_CG_GKCFG_BETWEENSUBNETBANDINFO_GETLIST_REQ,	GKCTRLOSPEVENT_START + 41);
// 获取网段间带宽信息列表响应
OSPEVENT(EV_GC_GKCFG_BETWEENSUBNETBANDINFO_GETLIST_RSP,	GKCTRLOSPEVENT_START + 42);

// 获取一个网段间带宽信息
OSPEVENT(EV_CG_GKCFG_BETWEENSUBNETBANDINFO_GET_REQ,	GKCTRLOSPEVENT_START + 43);
// 获取一个网段间带宽信息响应
OSPEVENT(EV_GC_GKCFG_BETWEENSUBNETBANDINFO_GET_RSP,	GKCTRLOSPEVENT_START + 44);

// 增加H.323密码认证
OSPEVENT(EV_CG_ENDPOINT_ADD_REQ,	GKCTRLOSPEVENT_START + 45);
// 增加H.323密码认证响应
OSPEVENT(EV_GC_ENDPOINT_ADD_RSP,	GKCTRLOSPEVENT_START + 46);

// 删除H.323密码认证
OSPEVENT(EV_CG_ENDPOINT_DEL_REQ,	GKCTRLOSPEVENT_START + 47);
// 删除H.323密码认证响应
OSPEVENT(EV_GC_ENDPOINT_DEL_RSP,	GKCTRLOSPEVENT_START + 48);

// 修改H.323密码认证
OSPEVENT(EV_CG_ENDPOINT_MODIFY_REQ,	GKCTRLOSPEVENT_START + 49);
// 修改H.323密码认证响应
OSPEVENT(EV_GC_ENDPOINT_MODIFY_RSP,	GKCTRLOSPEVENT_START + 50);

// 获取H.323密码认证列表
OSPEVENT(EV_CG_GETENDPOINTLIST_REQ,	GKCTRLOSPEVENT_START + 51);
// 获取H.323密码认证列表响应
OSPEVENT(EV_GC_GETENDPOINTLIST_RSP,	GKCTRLOSPEVENT_START + 52);

// 重启GK
OSPEVENT(EV_CG_RESTARTGK_CMD,	GKCTRLOSPEVENT_START + 53);

// 强制呼叫结束
OSPEVENT(EV_CG_STOPCALL_REQ,	GKCTRLOSPEVENT_START + 54);
// 强制呼叫结束响应
OSPEVENT(EV_GC_STOPCALL_RSP,	GKCTRLOSPEVENT_START + 55);

// 强制H.323实体注销
OSPEVENT(EV_CG_UNREGENDPOINT_REQ,	GKCTRLOSPEVENT_START + 56);
// 强制H.323实体注销响应
OSPEVENT(EV_GC_UNREGENDPOINT_RSP,	GKCTRLOSPEVENT_START + 57);

// 获取已注册H.323实体列表
OSPEVENT(EV_CG_GETREGISTEREDENDPOINTLIST_REQ,	GKCTRLOSPEVENT_START + 58);
// 获取已注册H.323实体列表响应
OSPEVENT(EV_GC_GETREGISTEREDENDPOINTLIST_RSP,	GKCTRLOSPEVENT_START + 59);

// 获取特定H.323实体的呼叫列表
OSPEVENT(EV_CG_GETCALLLIST_REQ,	GKCTRLOSPEVENT_START + 60);
// 获取特定H.323实体的呼叫列表响应
OSPEVENT(EV_GC_GETCALLLIST_RSP,	GKCTRLOSPEVENT_START + 61);

// H.323实体注册通知
OSPEVENT(EV_GC_RTIMEMSG_REG_NTF,	GKCTRLOSPEVENT_START + 62);
// H.323实体注销通知
OSPEVENT(EV_GC_RTIMEMSG_UNREG_NTF,	GKCTRLOSPEVENT_START + 63);

// H.323实体呼叫通知
OSPEVENT(EV_GC_RTIMEMSG_CALL_NTF,	GKCTRLOSPEVENT_START + 64);
// H.323实体呼叫结束通知
OSPEVENT(EV_GC_RTIMEMSG_CALLFINISH_NTF,	GKCTRLOSPEVENT_START + 65);

// 带宽改变通知
OSPEVENT(EV_GC_RTIMEMSG_BANDWISTHCHANGED_NTF,	GKCTRLOSPEVENT_START + 66);
// 位置请求通知
OSPEVENT(EV_GC_RTIMEMSG_RECEIVELOCATIONREQ_NTF,	GKCTRLOSPEVENT_START + 67);
// IRR消息通知
OSPEVENT(EV_GC_RTIMEMSG_RECEIVEIRR_NTF,	GKCTRLOSPEVENT_START + 68);

// 增加邻居GK
OSPEVENT( EV_CG_GKCFG_NEIGHBORGK_ADD_REQ,	GKCTRLOSPEVENT_START + 69);
// 增加邻居GK响应
OSPEVENT( EV_GC_GKCFG_NEIGHBORGK_ADD_RSP,	GKCTRLOSPEVENT_START + 70);

// 获取邻居GK
OSPEVENT( EV_CG_GKCFG_NEIGHBORGK_GET_REQ,	GKCTRLOSPEVENT_START + 71);
// 获取邻居GK响应
OSPEVENT( EV_GC_GKCFG_NEIGHBORGK_GET_RSP,	GKCTRLOSPEVENT_START + 72);

// 删除邻居GK
OSPEVENT( EV_CG_GKCFG_NEIGHBORGK_DEL_REQ,	GKCTRLOSPEVENT_START + 73);
// 删除邻居GK响应
OSPEVENT( EV_GC_GKCFG_NEIGHBORGK_DEL_RSP,	GKCTRLOSPEVENT_START + 74);

// 设置认证方式
OSPEVENT( EV_CG_GKCFG_AUTHMETHOD_SET_REQ,	GKCTRLOSPEVENT_START + 75);
// 设置认证方式响应
OSPEVENT( EV_GC_GKCFG_AUTHMETHOD_SET_RSP,	GKCTRLOSPEVENT_START + 76);

// 获取认证方式
OSPEVENT( EV_CG_GKCFG_AUTHMETHOD_GET_REQ,	GKCTRLOSPEVENT_START + 77);
// 获取认证方式响应
OSPEVENT( EV_GC_GKCFG_AUTHMETHOD_GET_RSP,	GKCTRLOSPEVENT_START + 78);

// 设置认证密码
OSPEVENT( EV_CG_GKCFG_AUTHPASSWORD_SET_REQ,	GKCTRLOSPEVENT_START + 79);
// 设置认证密码响应
OSPEVENT( EV_GC_GKCFG_AUTHPASSWORD_SET_RSP,	GKCTRLOSPEVENT_START + 80);

// 获取认证密码
OSPEVENT( EV_CG_GKCFG_AUTHPASSWORD_GET_REQ,	GKCTRLOSPEVENT_START + 81);
// 获取认证密码响应
OSPEVENT( EV_GC_GKCFG_AUTHPASSWORD_GET_RSP,	GKCTRLOSPEVENT_START + 82);

// 设置网口配置
OSPEVENT( EV_CG_GKCFG_ETH_SET_REQ,	GKCTRLOSPEVENT_START + 83);
// 设置网口配置响应
OSPEVENT( EV_GC_GKCFG_ETH_SET_RSP,	GKCTRLOSPEVENT_START + 84);

// 获取网口配置
OSPEVENT( EV_CG_GKCFG_ETH_GET_REQ,	GKCTRLOSPEVENT_START + 85);
// 获取网口配置响应
OSPEVENT( EV_GC_GKCFG_ETH_GET_RSP,	GKCTRLOSPEVENT_START + 86);

// 获取GK RadiusAuth配置响应
//OSPEVENT( EV_RADIUSAUTH_CONFIG_GET_RSP, GKCTRLOSPEVENT_START + 87);


//add by guozhongjun 2006.07.05
//设置Radius认证配置
OSPEVENT( EV_CG_GKCFG_RADIUS_SET_REQ, GKCTRLOSPEVENT_START + 87);
//设置Radius认证配置响应
OSPEVENT( EV_GC_GKCFG_RADIUS_SET_RSP, GKCTRLOSPEVENT_START + 88);

// 获取GK RadiusAuth配置
OSPEVENT( EV_CG_GKCFG_RADIUS_GET_REQ,     GKCTRLOSPEVENT_START + 89);
// 获取GK RadiusAuth配置响应
OSPEVENT( EV_GC_GKCFG_RADIUS_GET_RSP,     GKCTRLOSPEVENT_START + 90);

//[12/26/2007 zhangzhihua]add 
// 增加H.323实体认证
OSPEVENT(EV_CG_ENTITY_ADD_REQ,	GKCTRLOSPEVENT_START + 91);
// 增加H.323实体认证响应
OSPEVENT(EV_GC_ENTITY_ADD_RSP,	GKCTRLOSPEVENT_START + 92);

// 删除H.323实体认证
OSPEVENT(EV_CG_ENTITY_DEL_REQ,	GKCTRLOSPEVENT_START + 93);
// 删除H.323实体认证响应
OSPEVENT(EV_GC_ENTITY_DEL_RSP,	GKCTRLOSPEVENT_START + 94);

// 修改H.323实体认证
OSPEVENT(EV_CG_ENTITY_MODIFY_REQ,	GKCTRLOSPEVENT_START + 95);
// 修改H.323实体认证响应
OSPEVENT(EV_GC_ENTITY_MODIFY_RSP,	GKCTRLOSPEVENT_START + 96);

// 获取H.323实体认证列表
OSPEVENT(EV_CG_GETENTITYLIST_REQ,	GKCTRLOSPEVENT_START + 97);
// 获取H.323实体认证列表响应
OSPEVENT(EV_GC_GETENTITYLIST_RSP,	GKCTRLOSPEVENT_START + 98);

//获取认证所有禁止IP范围列表
OSPEVENT(EV_CG_GKCFG_REGIPRNG_GETFORBIDLIST_REQ,	GKCTRLOSPEVENT_START + 99);
//获取认证所有禁止IP范围列表响应
OSPEVENT(EV_CG_GKCFG_REGIPRNG_GETFORBIDLIST_RSP,	GKCTRLOSPEVENT_START + 100);
//获取pcmt授权情况请求
OSPEVENT( EV_CG_GKCFG_PCMT_AUTH_STATUS_REQ , GKCTRLOSPEVENT_START + 101 );
//获取pcmt授权情况响应
OSPEVENT( EV_CG_GKCFG_PCMT_AUTH_STATUS_RSP , GKCTRLOSPEVENT_START + 102 );
//pcmt授权情况通知
OSPEVENT( EV_CG_GKCFG_PCMT_AUTH_STATUS_NTF , GKCTRLOSPEVENT_START + 103 ); 
//end add.

//add by yj [20120807]
//获取GK设备型号及MAC地址
OSPEVENT( EV_CG_GKCFG_GK_MACADDR_PRODUCTID_REQ , GKCTRLOSPEVENT_START + 104 );
//获取GK设备型号及MAC地址响应
OSPEVENT( EV_CG_GKCFG_GK_MACADDR_PRODUCTID_RSP , GKCTRLOSPEVENT_START + 105 ); 
//end [20120807]

//add by yj for support all kinds of mts [20120924]
//获取所有类型的mt授权情况请求
OSPEVENT( EV_CG_GKCFG_ALL_MT_AUTH_STATUS_REQ , GKCTRLOSPEVENT_START + 106 );
//获取所有类型的mt授权情况响应
OSPEVENT( EV_CG_GKCFG_ALL_MT_AUTH_STATUS_RSP , GKCTRLOSPEVENT_START + 107 );
//所有类型的mt授权情况通知
OSPEVENT( EV_CG_GKCFG_ALL_MT_AUTH_STATUS_NTF , GKCTRLOSPEVENT_START + 108 ); 
//end [20120924]

//add by yj for brdtype and update [20130227]
//获取单板类型请求
OSPEVENT( EV_CG_GKCFG_BRDTYPE_GET_REQ , GKCTRLOSPEVENT_START + 109 );
//获取单板类型响应
OSPEVENT( EV_GC_GKCFG_BRDTYPE_GET_RSP , GKCTRLOSPEVENT_START + 110 );
//升级版本请求
OSPEVENT( EV_CG_GKCFG_VESION_UPDATE_REQ , GKCTRLOSPEVENT_START + 111 );
//升级版本响应
OSPEVENT( EV_GC_GKCFG_VESION_UPDATE_RSP , GKCTRLOSPEVENT_START + 112 );
//end [20130227]

//内部消息
OSPEVENT( EV_PRINT_REG,                   	GKCTRLOSPEVENT_START + 140);
OSPEVENT( EV_PRINT_REGDB,     	            GKCTRLOSPEVENT_START + 141);
OSPEVENT( EV_PRINT_CALL,     	            GKCTRLOSPEVENT_START + 142);
OSPEVENT( EV_PRINT_CALLDB,     	            GKCTRLOSPEVENT_START + 143);
OSPEVENT( EV_PRINT_IRQDB,     	            GKCTRLOSPEVENT_START + 144);
OSPEVENT( EV_PRINT_TIMER,                   GKCTRLOSPEVENT_START + 145);
OSPEVENT( EV_PRINT_DELAYDB,                 GKCTRLOSPEVENT_START + 146);

OSPEVENT( EV_PRINT_CONFIGDB,                GKCTRLOSPEVENT_START + 147);


OSPEVENT( EV_INTER_AUTHENDPOINT,        	GKCTRLOSPEVENT_START + 150);
OSPEVENT( EV_INTER_ISNEIGHBORGKPREFIX,     	GKCTRLOSPEVENT_START + 151);
OSPEVENT( EV_INTER_ISBANDWIDRHALLOWED,     	GKCTRLOSPEVENT_START + 152);
OSPEVENT( EV_INTER_GETALLREGEP,          	GKCTRLOSPEVENT_START + 153);
OSPEVENT( EV_INTER_GETALLCALL,          	GKCTRLOSPEVENT_START + 154);
OSPEVENT( EV_INTER_SHOWALLREALTIMEBWINFO,  	GKCTRLOSPEVENT_START + 155);

OSPEVENT( EV_RADIUS_OFF,  	              GKCTRLOSPEVENT_START + 156);
OSPEVENT( EV_RADIUS_SELECT,     	      GKCTRLOSPEVENT_START + 157);
OSPEVENT( EV_RADIUS_REGACK,  	          GKCTRLOSPEVENT_START + 158);
OSPEVENT( EV_RADIUS_REGREJ,               GKCTRLOSPEVENT_START + 159);

//记帐开始
OSPEVENT( EV_RADIUS_ACCOUNT_START,    	      GKCTRLOSPEVENT_START + 161);
//记帐结束
OSPEVENT( EV_RADIUS_ACCOUNT_STOP,    	      GKCTRLOSPEVENT_START + 162);

//OSPEVENT( EV_RADIUS_ACCTTIMEOUT,  	      GKCTRLOSPEVENT_START + 163);

//认证请求
OSPEVENT( EV_RADIUS_ACCESS_REQ,                  GKCTRLOSPEVENT_START + 164);

//radius认证结果  add by guozhongjun 2006.06.21
OSPEVENT(EV_RADIUS_AUTH_RESULT,               GKCTRLOSPEVENT_START + 165);

OSPEVENT(EV_RADIUS_ACCESS_ACK,               GKCTRLOSPEVENT_START + 166);
OSPEVENT(EV_RADIUS_ACCESS_REJ,               GKCTRLOSPEVENT_START + 167);
OSPEVENT(EV_RADIUS_ACCOUNT_ACK,               GKCTRLOSPEVENT_START + 168);
OSPEVENT(EV_RADIUS_ACCOUNT_REJ,               GKCTRLOSPEVENT_START + 169);

//GK内部使用的响应消息
//会议认证结果
OSPEVENT(EV_GK_CONF_AUTH_RESULT, GKCTRLOSPEVENT_START + 170);
//会议计费结果
OSPEVENT(EV_GK_CONF_ACCT_RESULT, GKCTRLOSPEVENT_START + 171);

//GK内部使用的响应消息
//激活计费实例
OSPEVENT(EV_GK_CONF_ACTIVATE_INST, GKCTRLOSPEVENT_START + 172);  
//会议计费刷新响应
OSPEVENT(EV_GK_CONF_UPDATE_RESULT, GKCTRLOSPEVENT_START + 173); 


//GKInterReg 消息  add by shigubin
OSPEVENT( EV_INTERGK_START,     GKCTRLOSPEVENT_START + 174 );
OSPEVENT( EV_GKINTER_REGINFO,   GKCTRLOSPEVENT_START + 175 );
OSPEVENT( EV_GKINTER_UNREGINFO, GKCTRLOSPEVENT_START + 176 );
OSPEVENT( EV_INTERGK_UNREGALLINTERREG, GKCTRLOSPEVENT_START + 177 );
//GkInterReg 消息  Add End

//add by yj for 460server
OSPEVENT( EV_CALLER_SETUP_RCV,			GKCTRLOSPEVENT_START + 180 );
OSPEVENT( EV_CALLEE_SCR_RCV,			GKCTRLOSPEVENT_START + 181 );
OSPEVENT( EV_CALLEE_H225FACILITY_RCV,	GKCTRLOSPEVENT_START + 182 );
OSPEVENT( EV_CALL_H245STATECHANGE_RCV,	GKCTRLOSPEVENT_START + 183 );

OSPEVENT( EV_H225_FORWARD_MSG,			GKCTRLOSPEVENT_START + 184 );//直接转发225的消息
OSPEVENT( EV_H245_FORWARD_MSG,			GKCTRLOSPEVENT_START + 185 );//直接转发245的消息

OSPEVENT( EV_PRINT_460CALLDB,     			 GKCTRLOSPEVENT_START + 190);//打印460呼叫消息
OSPEVENT( EV_PRINT_460NOTCOMPLETE_H245MSG,   GKCTRLOSPEVENT_START + 191);//打印460呼叫H245消息队列
//end

OSPEVENT( GKCTRLOSPEVENT_END, GKCTRLOSPEVENT_START+ 200 );

//////////////////////////////////////////////////////////////////////////
OSPEVENT( GKTEST_START, EV_TEST_GKC_BGN );

OSPEVENT( EV_TEST_GETBASECFG,               GKTEST_START+1 );
OSPEVENT( EV_TEST_ISUSEREXIST,              GKTEST_START+2 );
OSPEVENT( EV_TEST_ISUSERLOGINED,            GKTEST_START+3 );
OSPEVENT( EV_TEST_ISFORBIDIPRNGEXIST,       GKTEST_START+4 );
OSPEVENT( EV_TEST_ISREGIPRNGEXIST,          GKTEST_START+5 );
OSPEVENT( EV_TEST_ISBSBEXIST,               GKTEST_START+6 );
OSPEVENT( EV_TEST_ISALLOWEDEPINDB,          GKTEST_START+7 );
OSPEVENT( EV_TEST_ISFORBIDEPINDB,           GKTEST_START+8 );

OSPEVENT( EV_TEST_GETONLINEENDPOINT_COUNT,  GKTEST_START+9 );
OSPEVENT( EV_TEST_ISENDPOINTREGISTERED,     GKTEST_START+10 );
OSPEVENT( EV_TEST_ISCALLEXIST,              GKTEST_START+11 );
OSPEVENT( EV_TEST_ISPREFIXEXIST,            GKTEST_START+12 );
OSPEVENT( EV_TEST_NEIGHBORGKINFO,           GKTEST_START+13 );
OSPEVENT( EV_TEST_GETEPALIASESBYIP,         GKTEST_START+14 );
OSPEVENT( EV_TEST_CLEARALL_CFG,             GKTEST_START+15 );

OSPEVENT( EV_TEST_GETLOGINUSERCOUNT,        GKTEST_START+16 );
OSPEVENT( EV_TEST_REBOOTGK,                 GKTEST_START+17 );

//Radiusconfig add by guozhongjun
OSPEVENT( EV_TEST_RADIUS_CONFIG,            GKTEST_START+18 );

OSPEVENT( GKTEST_END, EV_TEST_GKC_END );


//////////////////////////////// 枚举类型 //////////////////////////////


//函数执行结果
enum GKResult
{
    gk_err = -1,
    gk_ok
};

#ifndef CM_H
//实体类型
typedef enum
{
    cmEndpointTypeTerminal,
    cmEndpointTypeGateway,
    cmEndpointTypeMCU,
    cmEndpointTypeGK,
    cmEndpointTypeUndefined,
    cmEndpointTypeSET
} cmEndpointType;

typedef enum
{
    cmCallTypeP2P,
    cmCallTypeOne2N,
    cmCallTypeN2One,
    cmCallTypeN2Nw
} cmCallType;

typedef enum
{
    cmCallModelTypeDirect,
    cmCallModelTypeGKRouted
} cmCallModelType;

typedef enum
{
    cmRASDisengageReasonForcedDrop,
    cmRASDisengageReasonNormalDrop,
    cmRASDisengageReasonUndefinedReason
}cmRASDisengageReason;

typedef enum
{
    cmRASUnregReasonReregistrationRequired,
    cmRASUnregReasonTtlExpired,
    cmRASUnregReasonSecurityDenial,
    cmRASUnregReasonUndefinedReason,
    cmRASUnregReasonMaintenance
}cmRASUnregReason;

typedef enum
{
    cmRASReasonResourceUnavailable,             /* GRJ, RRJ, ARJ, LRJ - gatekeeper resources exhausted */
    cmRASReasonInsufficientResources,           /* BRJ */
    cmRASReasonInvalidRevision,                 /* GRJ, RRJ, BRJ */
    cmRASReasonInvalidCallSignalAddress,        /* RRJ */
    cmRASReasonInvalidRASAddress,               /* RRJ - supplied address is invalid */
    cmRASReasonInvalidTerminalType,             /* RRJ */
    cmRASReasonInvalidPermission,               /* ARJ - permission has expired */
                                                /* BRJ - true permission violation */
                                                /* LRJ - exclusion by administrator or feature */
    cmRASReasonInvalidConferenceID,             /* BRJ - possible revision */
    cmRASReasonInvalidEndpointID,               /* ARJ */
    cmRASReasonCallerNotRegistered,             /* ARJ */
    cmRASReasonCalledPartyNotRegistered,        /* ARJ - can't translate address */
    cmRASReasonDiscoveryRequired,               /* RRJ - registration permission has aged */
    cmRASReasonDuplicateAlias,                  /* RRJ - alias registered to another endpoint */
    cmRASReasonTransportNotSupported,           /* RRJ - one or more of the transports */
    cmRASReasonCallInProgress,                  /* URJ */
    cmRASReasonRouteCallToGatekeeper,           /* ARJ */
    cmRASReasonRequestToDropOther,              /* DRJ - can't request drop for others */
    cmRASReasonNotRegistered,                   /* DRJ, LRJ, INAK - not registered with gatekeeper */
    cmRASReasonUndefined,                       /* GRJ, RRJ, URJ, ARJ, BRJ, LRJ, INAK */
    cmRASReasonTerminalExcluded,                /* GRJ - permission failure, not a resource failure */
    cmRASReasonNotBound,                        /* BRJ - discovery permission has aged */
    cmRASReasonNotCurrentlyRegistered,          /* URJ */
    cmRASReasonRequestDenied,                   /* ARJ - no bandwidrg available */
                                                /* LRJ - cannot find location */
    cmRASReasonLocationNotFound,                /* LRJ - cannot find location */
    cmRASReasonSecurityDenial,                  /* GRJ, RRJ, URJ, ARJ, BRJ, LRJ, DRJ, INAK */
    cmRASReasonTransportQOSNotSupported,        /* RRJ */
    cmRASResourceUnavailable,                   /* Same as cmRASReasonResourceUnavailable */
    cmRASReasonInvalidAlias,                    /* RRJ - alias not consistent with gatekeeper rules */
    cmRASReasonPermissionDenied,                /* URJ - requesting user not allowed to unregister specified user */
    cmRASReasonQOSControlNotSupported,          /* ARJ */
    cmRASReasonIncompleteAddress,               /* ARJ, LRJ */
    cmRASReasonFullRegistrationRequired,        /* RRJ - registration permission has expired */
    cmRASReasonRouteCallToSCN,                  /* ARJ, LRJ */
    cmRASReasonAliasesInconsistent,             /* ARJ, LRJ - multiple aliases in request identify distinct people */
    cmRASReasonAdditiveRegistrationNotSupported,/* RRJ */
    cmRASReasonInvalidTerminalAliases,          /* RRJ */
    cmRASReasonExceedsCallCapacity,             /* ARJ - destination does not have the capacity for this call */
    cmRASReasonCollectDestination,              /* ARJ */
    cmRASReasonCollectPIN,                      /* ARJ */
    cmRASReasonGenericData,                     /* GRJ, RRJ, ARJ, LRJ */
    cmRASReasonNeededFeatureNotSupported,       /* GRJ, RRJ, ARJ, LRJ */
    cmRASReasonUnknownMessageResponse,          /* XRS message was received for the request */
    cmRASReasonHopCountExceeded,                /* LRJ */
    cmRASReasonNoH323ID,                        /* no h323id */
    cmRASReasonAuthFalse                        /* AuthFalse */

}cmRASReason;

#endif //CM_H

//认证选项
enum emAuthMethod
{
    emAuthMethod_NotAuth        = 0,       //不需要认证
    emAuthMethod_AuthIPAllow    = 1,       //IP认证，允许生效
    emAuthMethod_AuthIPForbid   = 2,       //IP认证，禁止生效

    emAuthMethod_AuthAliasAllow = 4,       //实体认证，允许生效
    emAuthMethod_AuthAliasForbid= 8,       //实体认证，禁止生效

    emAuthMethod_AuthPassword   = 16,       //密码认证
    emAuthMethod_AuthRadius     = 32        //Radius认证 add by guozhongjun 2006.06.21

};

// 数据库操作返回值
enum emGKDBRet
{
    emGKDBRet_Success = 0,        // 成功
    emGKDBRet_UnknownErr,         // 未知错误
    emGKDBRet_KeyExist,           // key存在
    emGKDBRet_DstKeyExist,        // key存在
    emGKDBRet_DBErr,              // 数据库出错
    emGKDBRet_InvalidData,        // 非法数据
    emGKDBRet_InvalidRcvParam,    // 接收参数不合法
    emGKDBRet_KeyNotFound,        // 未找到记录
    emGKDBRet_AllRemoved          // 所有记录都被删除
};

//GK设备型号
typedef enum
{
    cmGKPruduct_Unknown = 0,		//位置设备型号
	cmGKPruduct_GK,					//设备型号为GK
	cmGKPruduct_Win32				//设备型号为win32测试版本
}cmGKPruductID;

//add by yj for BrdType [20130227]
typedef enum
{
	cmGK_BrdType_Unknown,
	cmGK_BrdType_WIN32,			//win32调试版本
	cmGK_BrdType_NOTSINGLEBRD,	//内置GK(主要用于8000H和8000G内置GK)
	cmGK_BrdType_CRI,			//单板CRI板卡
	cmGK_BrdType_CRI2,			//单板CRI2板
	cmGK_BrdType_IS21,			//单板IS2.1板卡(此板卡再试不用做GK单板)
	cmGK_BrdType_IS22			//单板IS2.2板卡
} cmGKBrdType;
//end [20130227]

//////////////////////////////////////////////////////////////////////////
#define GK_ERROR_CODE_BASE                      20000

#define GK_ERROR_NEW_FAIL                       (GK_ERROR_CODE_BASE + 10)
#define GK_ERROR_TIMER_PARAM                    (GK_ERROR_CODE_BASE + 11)
#define GK_ERROR_TIMER_ALLOC                    (GK_ERROR_CODE_BASE + 12)
#define GK_ERROR_TIMER_NULL                     (GK_ERROR_CODE_BASE + 13)
#define GK_ERROR_TIMER_NOTFOUND                 (GK_ERROR_CODE_BASE + 14)
#define GK_ERROR_TIMER_STATE                    (GK_ERROR_CODE_BASE + 15)


//AUTH
#define GK_ERROR_AUTH_KEY_NOTFOUND              (GK_ERROR_CODE_BASE + 20)
#define GK_ERROR_BW_TOOBIG                      (GK_ERROR_CODE_BASE + 21)
#define GK_ERROR_REG_FORBID                     (GK_ERROR_CODE_BASE + 22)
#define GK_ERROR_REG_IP_FORBID                  (GK_ERROR_CODE_BASE + 23)
#define GK_ERROR_REG_ALIAS_FORBID               (GK_ERROR_CODE_BASE + 24)
#define GK_ERROR_REG_PASS_FORBID                (GK_ERROR_CODE_BASE + 25)

//RRQ
#define GK_ERROR_ALIAS_DUPLICATE                (GK_ERROR_CODE_BASE + 26)
#define GK_ERROR_EPID_NOTFOUND                  (GK_ERROR_CODE_BASE + 27)
#define GK_ERROR_INVALID_REQSN					(GK_ERROR_CODE_BASE + 28)

//RCF
#define GK_ERROR_RCF_SET_CALLADDR               (GK_ERROR_CODE_BASE + 35)
#define GK_ERROR_RCF_SET_ALIAS                  (GK_ERROR_CODE_BASE + 36)
#define GK_ERROR_RCF_SET_EPID                   (GK_ERROR_CODE_BASE + 37)
#define GK_ERROR_RCF_SET_WILLIRR                (GK_ERROR_CODE_BASE + 38)
#define GK_ERROR_RCF_SEND_RCF                   (GK_ERROR_CODE_BASE + 39)


//UCF
#define GK_ERROR_UCF_SEND_UCF                   (GK_ERROR_CODE_BASE + 40)

//ARQ
#define GK_ERROR_ARQ_STATE                      (GK_ERROR_CODE_BASE + 41)

//ACF
#define GK_ERROR_ACF_SET_BANDWIDTH              (GK_ERROR_CODE_BASE + 51)
#define GK_ERROR_ACF_SET_CALLMODEL              (GK_ERROR_CODE_BASE + 52)
#define GK_ERROR_ACF_SET_CALLADDR               (GK_ERROR_CODE_BASE + 53)
#define GK_ERROR_ACF_SET_RESPONDIRR             (GK_ERROR_CODE_BASE + 54)
#define GK_ERROR_ACF_SEND                       (GK_ERROR_CODE_BASE + 55)


//DCF
#define GK_ERROR_DCF_SEND                       (GK_ERROR_CODE_BASE + 60)

//BCF
#define GK_ERROR_BCF_SET_BANDWIDTH              (GK_ERROR_CODE_BASE + 70)
#define GK_ERROR_BCF_SEND                       (GK_ERROR_CODE_BASE + 71)



//LRQ
#define GK_ERROR_LRQ_SET_ALIAS                  (GK_ERROR_CODE_BASE + 80)
#define GK_ERROR_LRQ_SET_REPLAYADDR             (GK_ERROR_CODE_BASE + 81)
#define GK_ERROR_LRQ_SEND                       (GK_ERROR_CODE_BASE + 82)
//增加会商GK对通(CallId)
#define GK_ERROR_LRQ_SET_NONSTD                 (GK_ERROR_CODE_BASE + 83)
//end

//LCF
#define GK_ERROR_LCF_SET_CALLADDR               (GK_ERROR_CODE_BASE + 90)
#define GK_ERROR_LCF_SET_RASADDR                (GK_ERROR_CODE_BASE + 91)
#define GK_ERROR_LCF_SEND                       (GK_ERROR_CODE_BASE + 92)

//GCF
#define GK_ERROR_GCF_SET_RASADDR                (GK_ERROR_CODE_BASE + 100)
#define GK_ERROR_GCF_SEND                       (GK_ERROR_CODE_BASE + 101)
#define GK_ERROR_REJECT_MSG_NULL                (GK_ERROR_CODE_BASE + 102)

#define GK_ERROR_REJECT_SEND                    (GK_ERROR_CODE_BASE + 200)
#define GK_ERROR_MSG                            (GK_ERROR_CODE_BASE + 201)
#define GK_MSG_DELAY                            (GK_ERROR_CODE_BASE + 202)
#define GK_ERROR_GKID                           (GK_ERROR_CODE_BASE + 203)

/* DB ERROR CODE */
#define GK_ERROR_DB_BASE                   (GK_ERROR_CODE_BASE + 1000)
#define GK_ERROR_DB_UNKNOWN                (GK_ERROR_DB_BASE + emGKDBRet_UnknownErr)
#define GK_ERROR_DB_KEYEXIST               (GK_ERROR_DB_BASE + emGKDBRet_KeyExist)
#define GK_ERROR_DB_DSTKEYEXIST            (GK_ERROR_DB_BASE + emGKDBRet_DstKeyExist)
#define GK_ERROR_DB_DBERR                  (GK_ERROR_DB_BASE + emGKDBRet_DBErr)
#define GK_ERROR_DB_INVALIDDATA            (GK_ERROR_DB_BASE + emGKDBRet_InvalidData)
#define GK_ERROR_DB_INVALIDPARAM           (GK_ERROR_DB_BASE + emGKDBRet_InvalidRcvParam)
#define GK_ERROR_DB_KEYNOTFOUND            (GK_ERROR_DB_BASE + emGKDBRet_KeyNotFound)
#define GK_ERROR_DB_ALLREMOVED             (GK_ERROR_DB_BASE + emGKDBRet_AllRemoved)



//////////////////////////////// 结构类型 //////////////////////////////



//别名类型
typedef struct tagAliasAddr
{
private:
    cmAliasType 	m_emAliasType;
    s8			    m_achAliasName[MAX_ALIASNAME_LEN+1];
    u16             m_wAliasNameLen;

public:
    tagAliasAddr()
	{
		Clear();
	}

	BOOL operator==(tagAliasAddr& tAlias)
	{
		if ( GetAliasType() == tAlias.GetAliasType() )
		{
			s8 szSrcAliasName[MAX_ALIASNAME_LEN+1] = {0};
			s8 szDstAliasName[MAX_ALIASNAME_LEN+1] = {0};

			u16 wSrcLen = GetAliasName( szSrcAliasName, MAX_ALIASNAME_LEN );
			u16 wDstLen = tAlias.GetAliasName( szDstAliasName, MAX_ALIASNAME_LEN );

			if ( wSrcLen == wDstLen &&
				 memcmp( szSrcAliasName, szDstAliasName, wSrcLen ) == 0 )
			{
				return TRUE;
			}
		}

		return FALSE;
	}

	void Clear()
    {
        m_emAliasType = (cmAliasType)0;
        m_wAliasNameLen = 0;
        memset( m_achAliasName, 0, MAX_ALIASNAME_LEN+1 );
    }

    void SetAliasType(cmAliasType aliasType)
    {
        m_emAliasType = (cmAliasType)htonl(aliasType);
    }

    cmAliasType GetAliasType()
    {
        return (cmAliasType)ntohl(m_emAliasType);
    }
    
    u16 GetAliasNameLen()
    {
        return ntohs(m_wAliasNameLen);
    }

    BOOL SetAlias(cmAliasType aliasType, s8 *pszName, u16 wLen, BOOL bGetFromStack = FALSE)
    {
        SetAliasType(aliasType);
        return SetAliasName(pszName, wLen, bGetFromStack);
    }

    //在之前必须先设定别名类型
    //若为BMP类型的别名，则内部保存为BMPString，返回给上层时为String
	//GK支持UTF8转码 [20130712]
    //BOOL SetAliasName(s8 *pszName, u16 wLen, BOOL bGetFromStack = FALSE)
	BOOL SetAliasName(s8 *pszName, u16 wLen, BOOL bGetFromStack = FALSE, IN BOOL bUtf8 = FALSE)
	//end [20130712]
    {
        
        u16 maxLen = 0;
        cmAliasType aliasType = GetAliasType();
        
        if ( bGetFromStack == FALSE )
        {
            if ( aliasType == cmAliasTypeH323ID
                || aliasType == cmAliasTypeEndpointID
                || aliasType == cmAliasTypeGatekeeperID)
            {
                //转成BMPString
				//GK支持UTF8转码 [20130712]
				if ( bUtf8 )
				{
					u8 aszUTF8Str[MAX_ALIASNAME_UTF8_LEN+1] = {0};
					if( wLen > MAX_ALIASNAME_UTF8_LEN )
						return FALSE;

					memcpy( aszUTF8Str, pszName, wLen );
					m_wAliasNameLen = utlUTF82Uni( (s8*)aszUTF8Str, (BYTE*)m_achAliasName );

					maxLen = (aliasType == cmAliasTypeH323ID? MAX_ALIASNAME_LEN:MAX_E164_LEN);

					if ( m_wAliasNameLen > maxLen )
						return FALSE;

					m_achAliasName[m_wAliasNameLen] = '\0';
					m_wAliasNameLen = htons(m_wAliasNameLen);
				}
				else
				{
				//end [20130712]
                s8 chrStr[MAX_ALIASNAME_LEN+1] = {0};
                memcpy( chrStr, pszName, wLen );

                m_wAliasNameLen = utlChr2Bmp( chrStr, (BYTE*)m_achAliasName );

                maxLen = (aliasType == cmAliasTypeH323ID? MAX_ALIASNAME_LEN:MAX_E164_LEN);

                if (m_wAliasNameLen > maxLen) 
                    return FALSE;

                m_achAliasName[m_wAliasNameLen] = '\0';
                m_wAliasNameLen = htons(m_wAliasNameLen);
			//GK支持UTF8转码 [20130712]
				}  
			//end [20130712]

                return TRUE;
            }
        }

       
        maxLen = (aliasType == cmAliasTypeE164 ? MAX_E164_LEN:MAX_ALIASNAME_LEN);
        if (wLen > maxLen) 
            return FALSE;
        memset(m_achAliasName,0,MAX_ALIASNAME_LEN+1);
        memcpy(m_achAliasName, pszName, wLen);
        m_achAliasName[wLen] = '\0';

        m_wAliasNameLen = htons(wLen);
        return TRUE;
    }

    	//GK支持UTF8转码 [20130712]
    //u16 GetAliasName( INOUT s8 *pszName, IN u16 wNameLen, IN BOOL bToStack = FALSE )
	u16 GetAliasName( INOUT s8 *pszName, IN u16 wNameLen, IN BOOL bToStack = FALSE, IN BOOL bUtf8 = FALSE )
	//end [20130712]
    {
        if ( pszName == NULL )
            return 0;

        u16 wAliasNameLen = ntohs(m_wAliasNameLen);

        if ( bToStack == TRUE )
        {
            if ( wNameLen < wAliasNameLen )
                return 0;

            memcpy( pszName, m_achAliasName, wAliasNameLen );
            return wAliasNameLen;
        }

        cmAliasType aliasType = GetAliasType();

        if ( aliasType == cmAliasTypeH323ID
            || aliasType == cmAliasTypeEndpointID
            || aliasType == cmAliasTypeGatekeeperID)
        {
            //转换成单字节string
			//GK支持UTF8转码 [20130712]
			if ( bUtf8 )
			{
				u8 aszUTF8Str[MAX_ALIASNAME_UTF8_LEN+1] = {0};
				wAliasNameLen = utlUni2UTF8((u8*)m_achAliasName, wAliasNameLen, (s8*)aszUTF8Str);
				if ( wNameLen < wAliasNameLen )
					return 0;
				strncpy(pszName, (char *)aszUTF8Str, wAliasNameLen); 
			}
			else
			{
			//end [20130712]
				s8 chrStr[MAX_ALIASNAME_LEN+1] = {0};
				utlBmp2Chr( chrStr, (BYTE*)m_achAliasName, wAliasNameLen );
				wAliasNameLen = strlen( chrStr );
				if ( wNameLen < wAliasNameLen )
					return 0;
				
				strcpy( pszName, chrStr );
				//GK支持UTF8转码 [20130712]
			}  
			//end [20130712]
        }
        else
        {
            if ( wNameLen < wAliasNameLen )
                return 0;

            strcpy( pszName, m_achAliasName );
        }
		
        return wAliasNameLen;
    }

}TAliasAddr,*PTAliasAddr;

//transport地址
typedef struct tagNetAddr
{
private:
    u32 m_dwIP;
    u16 m_wPort;
public:
    void SetNetAddr(u32 dwIP, u16 wPort)
    {
        m_dwIP = dwIP;
        m_wPort = htons(wPort);
    }

    void SetIPAddr(u32 dwIP)
    {
        m_dwIP = dwIP;
    }
    u32 GetIPAddr()
    {
        return m_dwIP;
    }

    void SetIPPort(u16 wPort)
    {
        m_wPort = htons(wPort);
    }
    u16 GetIPPort()
    {
        return ntohs(m_wPort);
    }

	BOOL operator==(tagNetAddr& tAddr)
	{
		if ( GetIPAddr() == tAddr.GetIPAddr()
			&& GetIPPort() == tAddr.GetIPPort() )
		{
			return TRUE;
		}

		return FALSE;
	}

    tagNetAddr()
    {
        m_dwIP  = 0;
        m_wPort = 0;
    }    
}TNETADDR,*PTNETADDR;

//控制台操作通用结构
//定义待发送消息的公用头部，消息体为对应消息号所使用的结构体
typedef struct tagGKCtrlBufHead
{
private:
    u32     m_dwSequenceNumber;  //序列号
    u32     m_dwErrorCode;       //错误码
    u32     m_dwContentLen;      //消息体内容长度
    u32     m_dwStructNum;       //消息体的结构体数目
    BOOL    m_bStructLenChange;  //消息体的结构内容是否变化
    BOOL    m_bWillContinue;     //是否有后续的消息，用于消息体长度太长的情况    
  
public:
    tagGKCtrlBufHead()
    {
        m_dwSequenceNumber = 0;
        m_dwErrorCode = 0;
        m_dwContentLen = 0;
        m_dwStructNum = 0;
        m_bStructLenChange = FALSE;
        m_bWillContinue = FALSE;
    }

    void SetSequenceNumber(u32 dwSequenceNumber)
    {
        m_dwSequenceNumber = htonl(dwSequenceNumber);//zhx修改成网络序
    }
    u32 GetSequenceNumber()
    {
        return ntohl(m_dwSequenceNumber);
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

}TGKCtrlBufHead,*PTGKCtrlBufHead;

//定义待发送消息体为变长结构时每个变长结构的头部，
//消息体为:
//压缩后的长度为m_dwAliasBufLen的别名和依次排列的m_byCallAddrNum个呼叫信令地址
//其中别名个数包括实体别名数和实体的EndpointIdentifier
typedef struct tagGKCtrlBufBodyHead
{
private:
    cmEndpointType	m_emEndpointType;       //终端类型
    TNETADDR        m_tRasAddr;             //RAS地址
    u16				m_wCallAddrNum;         //呼叫信令地址的个数    
    u16				m_wAliasNum;            //别名个数
    u32             m_dwAliasBufLen;        //压缩后的别名buf长度
    u32             m_dwTimerId;
    u32             m_dwTTL;

public:
    void SetTimerId( u32 dwTimerId)
    {
        m_dwTimerId = htonl(dwTimerId);
    }
    u32 GetTimerId()
    {
        return ntohl(m_dwTimerId);
    }

    void SetTerminalType( cmEndpointType emEndpointType )
    {
        m_emEndpointType = (cmEndpointType)htonl(emEndpointType);
    }
    cmEndpointType GetTerminalType()
    {
        return (cmEndpointType)ntohl(m_emEndpointType);
    }

    void SetAliasNum( u16 wAliasNum )
    {
        m_wAliasNum = htons(wAliasNum);
    }
    u16 GetAliasNum()
    {
        return ntohs(m_wAliasNum);
    }
    
    void SetAliasBufLen( u32 dwAliasBufLen )
    {
        m_dwAliasBufLen = htonl(dwAliasBufLen);
    }
    u32 GetAliasBufLen()
    {
        return ntohl(m_dwAliasBufLen);
    }

    void SetRASAddr( TNETADDR &tRASAddr )
    {
        m_tRasAddr = tRASAddr;
    }
    TNETADDR* GetRASAddr()
    {
        return &m_tRasAddr;
    }

    void SetCallAddrNum( u16 wCallAddrNum )
    {
        m_wCallAddrNum = htons(wCallAddrNum);
    }
    u16 GetCallAddrNum()
    {
        return ntohs(m_wCallAddrNum);
    }
}TGKCtrlBufBodyHead,*PTGKCtrlBufBodyHead;

//控制台用户登陆信息数据库结构
//以用户名为key
typedef struct tagCtrlUserDBData
{
private:
    u8		m_abyUserName[MAX_USERNAME_LEN];
    u16		m_wUserNameLen;
    u8		m_abyPassword[MAX_PASSWORD_LEN];
    u16		m_wPasswordLen;
    BOOL 	m_bAdministrator;

public:
    BOOL SetUserName( IN u8 *pbyUserName, IN u16 wNameLen )
    {
        if ( pbyUserName == NULL || wNameLen > MAX_USERNAME_LEN )
            return FALSE;

        memcpy( m_abyUserName, pbyUserName, wNameLen );
        m_wUserNameLen = htons( wNameLen );
        return TRUE;
    }

    u16 GetUserNameLen()
    {
        return ntohs( m_wUserNameLen );
    }

    void GetUserName( OUT u8 *pbyUserName, OUT u16 &wNameLen )
    {
        if ( pbyUserName == NULL || wNameLen < ntohs( m_wUserNameLen ) )
            return;

        wNameLen = ntohs( m_wUserNameLen );
        memcpy( pbyUserName, m_abyUserName, wNameLen );
    }

    BOOL SetPassword( IN u8 *pbyPassword, IN u16 wPassLen )
    {
        if ( pbyPassword == NULL || wPassLen > MAX_PASSWORD_LEN )
            return FALSE;
        
        memcpy( m_abyPassword, pbyPassword, wPassLen );
        m_wPasswordLen = htons( wPassLen );
        return TRUE;
    }
  
    u16 GetPasswordLen()
    {
        return ntohs( m_wPasswordLen );
    }
    
    void GetPassword( OUT u8 *pbyPassword, OUT u16 &wPassLen )
    {
        if ( pbyPassword == NULL || wPassLen < ntohs( m_wPasswordLen ) )
            return;
        
        wPassLen = ntohs( m_wPasswordLen );
        memcpy( pbyPassword, m_abyPassword, wPassLen );
    }

    void SetAdministrator( IN BOOL bAdministrator )
    {
        m_bAdministrator = htonl( bAdministrator );
    }
    
    BOOL IsAdministrator()
    {
        return ntohl( m_bAdministrator );
    }
    
    tagCtrlUserDBData()
    {
        memset( m_abyUserName, 0, sizeof(m_abyUserName) );
        memset( m_abyPassword, 0, sizeof(m_abyPassword) );
        
        m_wUserNameLen = 0;
        m_wPasswordLen = 0;
        m_bAdministrator = FALSE;
    }    
}TCtrlUserDBData,*PTCtrlUserDBData;


#ifdef WIN32
#pragma comment( lib, "ws2_32.lib" ) 
#pragma pack( push )
#pragma pack( 1 )
#define window( x )	x
#define PACKED 
#else
#include <netinet/in.h>
#define window( x )
#define PACKED __attribute__((__packed__))	// 取消编译器的优化对齐
#endif


//GK前缀结构
typedef struct tagGKPrefix
{
private:
    u8		m_abyPrefix[MAX_GKPREFIX_LEN];
    u8		m_abyPrefixLen;

public:
    BOOL SetPrefix( IN u8 *pbyPrefix, IN u8 byPrefixLen )
    {
        if ( pbyPrefix == NULL || byPrefixLen > MAX_GKPREFIX_LEN )
            return FALSE;
        
        memset(m_abyPrefix,0,MAX_GKPREFIX_LEN);
        memcpy( m_abyPrefix, pbyPrefix, byPrefixLen );
        m_abyPrefixLen = byPrefixLen;
        return TRUE;
    }
    
    u8 GetPrefixLen()
    {
        return m_abyPrefixLen;
    }
    
    void GetPrefix( OUT u8 *pbyPrefix, OUT u8 &byPrefixLen )
    {
        if ( pbyPrefix == NULL || byPrefixLen < m_abyPrefixLen )
		{
			byPrefixLen = 0;
            return;
		}
        
        byPrefixLen = m_abyPrefixLen;
        memcpy( pbyPrefix, m_abyPrefix, byPrefixLen );
    }
    
    tagGKPrefix()
    {
        memset(m_abyPrefix,0,MAX_GKPREFIX_LEN);
        m_abyPrefixLen = 0;
    }
}
#if defined(_VXWORKS_) || defined(_LINUX_)
__attribute__ ( (packed) ) 
#endif // _VXWORKS_

TGKPrefix,*PTGKPrefix;


//邻居GK信息
typedef struct tagNeighborGKInfo
{
	//这个结构不需要网络序的转换，Ip进来为网络序，
	//TGKPrefix成员都是u8型(zhx 05-10-13 注)
    u32         m_dwNeighborGKIP;
    TGKPrefix   m_tNeighborGKPrefix;

    tagNeighborGKInfo()
    {
        m_dwNeighborGKIP = 0;
    }
}

#if defined(_VXWORKS_) || defined(_LINUX_)
__attribute__ ( (packed) ) 
#endif // _VXWORKS_

TNeighborGKInfo;

#ifdef WIN32
#pragma pack( pop )
#endif

//配置信息数据库中的GK基本配置信息数据库结构
//以AuthMethod为key 存 m_emAuthMethod
//以TNeighborGKInfo为key 存 NULL
//以Prefix为key 存 NULL
typedef struct tagConfigBaseGKDBData
{
private:
    emAuthMethod	  m_emAuthMethod;
    TNeighborGKInfo   m_atNeighborGKInfo[MAX_NEIGHBORGK_NUM];
	u16				  m_wNeighborGKInfoNum;
    TGKPrefix         m_atLocalPrefix[MAX_GKPREFIX_NUM];
	u16				  m_wLocalPrefixNum;
    s8                m_szAuthPassword[MAX_PASSWORD_LEN+1];
	u16				  m_wAuthPasswordLen;
    u8                m_byIsUseEthFront;
    
public:
    void SetAuthPassword( s8 *pszPwd, u16 wPwdLen )
    {
        u16 wLen = min(wPwdLen, MAX_PASSWORD_LEN);
        strncpy( m_szAuthPassword, pszPwd, wLen );
        m_szAuthPassword[wLen] = '\0';

		m_wAuthPasswordLen = htons( wLen );
    }

	u16 GetAuthPasswordLen()
	{
		return ntohs( m_wAuthPasswordLen );
	}

    s8* GetAuthPassword()
    {
        return m_szAuthPassword;
    }

    void SetAuthMethod( emAuthMethod authMethod )
    {
        m_emAuthMethod = (emAuthMethod)htonl(authMethod);
    }
    emAuthMethod GetAuthMethod()
    {
        return (emAuthMethod)ntohl(m_emAuthMethod);
    }
    
    void SetNeighborGKInfo( TNeighborGKInfo atNeighborGKInfo[], u16 wNum )
    {
        for( s32 nIndex = 0; nIndex < min(wNum, MAX_NEIGHBORGK_NUM); nIndex++ )
            m_atNeighborGKInfo[nIndex] = atNeighborGKInfo[nIndex];

		m_wNeighborGKInfoNum = htons( min(wNum, MAX_NEIGHBORGK_NUM) );
    }	
    TNeighborGKInfo& GetNeighborGKInfo( u16 wNo )
    {
        if ( wNo < MAX_NEIGHBORGK_NUM ) 
            return m_atNeighborGKInfo[wNo];

        return m_atNeighborGKInfo[0];
    }
	u16 GetNeighborGKInfoNum()
	{
		return ntohs( m_wNeighborGKInfoNum );
	}
	void SetNeighborGKInfoNum( u16 wNum )
	{
		m_wNeighborGKInfoNum = htons( wNum );
	}
    TNeighborGKInfo* GetNeighborGKInfo()
    {
        return m_atNeighborGKInfo;
    }

    void SetLocalPrefix( TGKPrefix atGKPrefix[], u16 wNum )
    {
        for( s32 nIndex = 0; nIndex < min(wNum, MAX_GKPREFIX_NUM); nIndex++ )
            m_atLocalPrefix[nIndex] = atGKPrefix[nIndex];

		m_wLocalPrefixNum = htons( min(wNum,MAX_GKPREFIX_NUM) );
    }
    TGKPrefix& GetLocalPrefix( u16 wNo )
    {
        if ( wNo < MAX_GKPREFIX_NUM ) 
            return m_atLocalPrefix[wNo];
        
        return m_atLocalPrefix[0];
    }
	u16 GetLocalPrefixNum()
	{
		return ntohs( m_wLocalPrefixNum );
	}
	void SetLocalPrefixNum( u16 wNum )
	{
		m_wLocalPrefixNum = htons( wNum );
	}
    TGKPrefix* GetLocalPrefix()
    {
        return m_atLocalPrefix;
    }

    void SetIsUseEthFront(u8 byIsUseFront)
    {
        m_byIsUseEthFront = byIsUseFront;
    }

    u8 GetIsUseEthFront()
    {
        return m_byIsUseEthFront;
    }

    tagConfigBaseGKDBData()
    {
        m_emAuthMethod  = emAuthMethod_NotAuth;
        memset( m_szAuthPassword, 0, MAX_PASSWORD_LEN );
		m_wLocalPrefixNum = 0;
		m_wNeighborGKInfoNum = 0;
		m_wAuthPasswordLen = 0;
        m_byIsUseEthFront = 0;
    } 
    
}TConfigBaseGKDBData,*PTConfigBaseGKDBData;

//add by yj for set IP by GKC [20130627]
typedef struct tagConfigIPDataFromGKC
{
private:
	u8			m_bIsUsedConstIP;
	u32			m_dwConstIP;
public:
	
	tagConfigIPDataFromGKC()
	{
		m_bIsUsedConstIP = FALSE;
		m_dwConstIP = 0;
	}
	
	void SetGKIPByNetAddr( u32 dwConstIP )
	{
		if( ( dwConstIP != 0 ) && ( dwConstIP != 0x7f000001 ) )
		{
			m_bIsUsedConstIP = TRUE;
			m_dwConstIP = dwConstIP;
		}
	}
	
	void SetGKIPByHostAddr( u32 dwConstIP )
	{
		if( ( dwConstIP != 0 ) && ( dwConstIP != 0x7f000001 ) )
		{
			m_bIsUsedConstIP = TRUE;
			m_dwConstIP = htonl( dwConstIP );
		}
	}
	
	u32 GetGKIP()
	{
		u32 dwConstIP = 0;
		if ( m_bIsUsedConstIP )
		{
			dwConstIP = ntohl( m_dwConstIP );
		}
		return dwConstIP;
	}
}TConfigIPDataFromGKC,*PTConfigIPDataFromGKC;
//end [20130627]

//IP范围认证
typedef struct tagAddrRng
{
private:
    u32 	m_dwIPAddrFrom;
    u32 	m_dwIPAddrTo;
public:
    tagAddrRng()
    {
        m_dwIPAddrFrom = 0;
        m_dwIPAddrTo = 0;
    }

    void SetIPAddrFrom( u32 dwIPAddr )
    {
        m_dwIPAddrFrom = dwIPAddr;
    }
    u32 GetIPAddrFrom()
    {
        return m_dwIPAddrFrom;
    }
    
    void SetIPAddrTo( u32 dwIPAddr )
    {
        m_dwIPAddrTo = dwIPAddr;
    }
    u32 GetIPAddrTo()
    {
        return m_dwIPAddrTo;
    }
}TAddrRng,*PTAddrRng;

//配置信息数据库中的IP范围认证数据库结构
//以Index为key
typedef struct tagConfigAuthIPDBData
{
private:
    TAddrRng 	m_tAddrRng;
    BOOL        m_bAllowed;     //是否允许生效

public:
    void SetAddrRng( TAddrRng &tAddrRng )
    {
        m_tAddrRng = tAddrRng;
    }
    TAddrRng* GetAddrRng()
    {
        return &m_tAddrRng;
    }    

    void SetAuthMethod( BOOL bAuthAllow )
    {
        m_bAllowed = htonl( bAuthAllow );
    }
    BOOL IsAuthAllow()
    {
        return ntohl( m_bAllowed );
    }

    tagConfigAuthIPDBData()
    {
        m_bAllowed = FALSE;
    }    
}TConfigAuthIPDBData,*PTConfigAuthIPDBData;

//IP 范围结构
typedef struct tagIPRange
{
private:
    u32		m_dwIPFrom;
    u32		m_dwIPFromMask;
    
    u32		m_dwIPTo;
    u32		m_dwIPToMask;
public:
    void SetIPFrom( u32 dwIPAddr, u32 dwIPMask )
    {
        m_dwIPFrom = dwIPAddr;
        m_dwIPFromMask = dwIPMask;
    }
    u32 GetIPFrom()
    {
        return m_dwIPFrom;
    }
    u32 GetIPFromMask()
    {
        return m_dwIPFromMask;
    }
    
    void SetIPTo( u32 dwIPAddr, u32 dwIPMask )
    {
        m_dwIPTo = dwIPAddr;
        m_dwIPToMask = dwIPMask;
    }
    u32 GetIPTo()
    {
        return m_dwIPTo;
    }
    u32 GetIPToMask()
    {
        return m_dwIPToMask;
    }
    
    tagIPRange()
    {
        m_dwIPFrom = 0;
        m_dwIPFromMask = 0;
        
        m_dwIPTo = 0;
        m_dwIPToMask = 0;
    }
}TIPRange,*PTIPRange;

//带宽管理的Data
typedef struct tagBandwidthCtrlData
{
private:
    u32			m_dwMaxBandwidth;           //某个网段允许的最大带宽 单位：kbps
    u32			m_dwPerCallMaxBandwidth;    //每个呼叫的双向最大带宽 单位：kbps

public:
    tagBandwidthCtrlData()
    {
        m_dwMaxBandwidth = 0;
        m_dwPerCallMaxBandwidth = 0;
    }

    void SetMaxBandwidth( u32 dwBandwidth )
    {
        m_dwMaxBandwidth = htonl( dwBandwidth );
    }
    u32 GetMaxBandwidth()
    {
        return ntohl( m_dwMaxBandwidth );
    }
    
    void SetPerCallMaxBandwidth( u32 dwBandwidth )
    {
        m_dwPerCallMaxBandwidth = htonl( dwBandwidth );
    }
    u32 GetPerCallMaxBandwidth()
    {
        return ntohl( m_dwPerCallMaxBandwidth );
    }
}TBandwidthCtrlData, *PTBandwidthCtrlData;

//配置信息数据库中的网段带宽信息数据库结构
//以TIPRange为key
typedef struct tagConfigBandWidthDBData
{
private:
    TIPRange	        m_tIPRange;
    TBandwidthCtrlData  m_tBWCtrlData;
public:
    void SetIPRange( TIPRange &tIPRange )
    {
        m_tIPRange = tIPRange;
    }
    PTIPRange GetIPRange()
    {
        return &m_tIPRange;
    }

    void SetBWCtrlData( TBandwidthCtrlData &tBWCtrlData )
    {
        m_tBWCtrlData = tBWCtrlData;
    }
    PTBandwidthCtrlData GetBWCtrlData()
    {
        return &m_tBWCtrlData;
    }
}TConfigBandWidthDBData,*PTConfigBandWidthDBData;

//配置信息数据库中的密码认证数据库的key
typedef struct tagAliasAuthKey
{
private:
    cmEndpointType  m_emEndpointType;
    TAliasAddr	    m_tAliasAddr;
public:
    void SetAliasAddr( TAliasAddr &tAliasAddr )
    {
        m_tAliasAddr = tAliasAddr;
    }
    PTAliasAddr GetAliasAddr()
    {
        return &m_tAliasAddr;
    }
    
    void SetEndpointType( cmEndpointType emEndpointType )
    {
        m_emEndpointType = (cmEndpointType)htonl(emEndpointType);
    }
    cmEndpointType GetEndpointType()
    {
        return (cmEndpointType)ntohl(m_emEndpointType);
    }
}TAliasAuthKey,*PTAliasAuthKey;

//[12/26/2007 zhangzhihua]add. 
//配置信息数据库中的实体认证数据库的Key.
typedef struct tagEntityAuthKey
{
private:
    cmEndpointType  m_emEndpointType;
	u16             m_wAliasNum;
    TAliasAddr	    m_atAliasAddr[MAX_ENTITYALIAS_NUM];
public:
	tagEntityAuthKey()
	{
		m_wAliasNum = 0;
		m_emEndpointType = cmEndpointTypeTerminal;
		memset(m_atAliasAddr, 0, sizeof(TAliasAddr)*MAX_ENTITYALIAS_NUM);

	}
	//设置实体别名.
    void SetAliasAddr( TAliasAddr atAliasAddr[], u16 wAliasNum )
    {
		if (wAliasNum > MAX_ENTITYALIAS_NUM)
		{
			return;
		}

		for (u16 wAliasIndex = 0; wAliasIndex < wAliasNum; wAliasIndex++)
		{
			m_atAliasAddr[wAliasIndex] = atAliasAddr[wAliasIndex];
		}

		m_wAliasNum = htons(wAliasNum);
    }
	//获取实体别名.
	TAliasAddr& GetAliasAddr(u16 wAliasNo)
	{
		if (wAliasNo < MAX_ENTITYALIAS_NUM)
		{
			return m_atAliasAddr[wAliasNo];
		}
		else
		{
           return m_atAliasAddr[0];
		}
	}

    //得到别名数.
	u16 GetAliasNum()
	{
		return ntohs(m_wAliasNum);
	}
    //设置终端类型.
    void SetEndpointType( cmEndpointType emEndpointType )
    {
        m_emEndpointType = (cmEndpointType)htonl(emEndpointType);
    }
	//获取终端类型.
    cmEndpointType GetEndpointType()
    {
        return (cmEndpointType)ntohl(m_emEndpointType);
    }

	void clear()
	{
		m_wAliasNum = 0;
		m_emEndpointType = cmEndpointTypeTerminal;
		memset(m_atAliasAddr, 0, sizeof(TAliasAddr)*MAX_ENTITYALIAS_NUM);
	}

}TEntityAuthKey,*PTEntityAuthKey;

// end add.
//配置信息数据库中的实体认证数据库的data
//emAuthMethod只能是以下几种：
//      emAuthMethod_AuthAliasAllow = 4,                            //实体认证，允许生效
//      emAuthMethod_AuthAliasForbid= 8,                            //实体认证，禁止生效
//      emAuthMethod_AuthAliasAllow|emAuthMethod_AuthPassword = 20  //实体密码认证
typedef struct tagAliasAuthData
{
private:
//    emAuthMethod    m_emAuthMethod;                 //[12/26/2007 zhangzhihua]del. 
    u8              m_byPasswordLen;
    u8              m_abyPassword[MAX_PASSWORD_LEN];
public:
    tagAliasAuthData()
    {
//         m_emAuthMethod = emAuthMethod_AuthAliasForbid;
        m_byPasswordLen = 0;
        memset( m_abyPassword, 0, sizeof(m_abyPassword));
    }
//     void SetAuthMethod( emAuthMethod method )
//     {
//         m_emAuthMethod = (emAuthMethod)htonl(method);
//     }
//     emAuthMethod GetAuthMethod()
//     {
//         return (emAuthMethod)ntohl(m_emAuthMethod);
//     }
//     
    BOOL SetPassword( u8 *pbyPwd, u8 byLen )
    {
        if ( pbyPwd != NULL && byLen < MAX_PASSWORD_LEN ) 
        {
            m_byPasswordLen = byLen;
            memcpy( m_abyPassword, pbyPwd, byLen );
            m_abyPassword[byLen] = 0;
            return TRUE;
        }
        
        return FALSE;
    }
    
    u8 GetPassword( u8 *pbyPwd, u8 byLen )
    {
        if ( pbyPwd == NULL || byLen < m_byPasswordLen )
            return 0;
        
        memcpy( pbyPwd, m_abyPassword, m_byPasswordLen );
        return m_byPasswordLen;
    }

    u8 GetPasswordLen()
    {
        return m_byPasswordLen;
    }
}TAliasAuthData,*PTAliasAuthData;

//配置信息数据库中的密码认证数据库结构
//以TAliasAuthKey为key
//以TAliasAuthData为data
typedef struct tagConfigAliasAuthDBData
{
private:
    TAliasAuthKey	m_tAliasAuthKey;
    TAliasAuthData  m_tAliasAuthData;
public:
    void SetAliasAuthKey( TAliasAuthKey &tAliasAuthKey )
    {
        m_tAliasAuthKey = tAliasAuthKey;
    }
    PTAliasAuthKey GetAliasAuthKey()
    {
        return &m_tAliasAuthKey;
    }
    
    void SetAliasAuthData( TAliasAuthData &tAliasAuthData )
    {
        m_tAliasAuthData = tAliasAuthData;
    }
    PTAliasAuthData GetAliasAuthData()
    {
        return &m_tAliasAuthData;
    }
}TConfigAliasAuthDBData,*PTConfigAliasAuthDBData;

//[12/26/2007 zhangzhihua]add. 
//配置信息数据库中的实体认证数据库结构
//以TEntityAuthKey为Key.
typedef struct tagConfigEntityAuthDBData
{
private:
    TEntityAuthKey	m_tEntityAuthKey;
// 	u8              m_abData;
public:
    void SetEntityAuthKey( TEntityAuthKey &tEntityAuthKey )
    {
        m_tEntityAuthKey = tEntityAuthKey;
    }
    PTEntityAuthKey GetEntityAuthKey()
    {
        return &m_tEntityAuthKey;
    }
// 	void SetEntityAuthData(u8 abData)
// 	{
// 		m_abData = abData;
// 	}
  
}TConfigEntityAuthDBData,*PTConfigEntityAuthDBData;
//end add.

//呼叫ID结构，控制台操作实时呼叫信息时使用该结构
typedef struct tagCtrlCallId
{
    s8  m_achCallId[LEN_GUID];
    
    tagCtrlCallId()
    {
        memset( m_achCallId, 0, LEN_GUID );
    }    
}TCtrlCallId,*PTCtrlCallId;

//呼叫通知,向控制台用户通知呼叫到来
typedef struct tagCallIncInfoNtf
{
 private:
    TNETADDR			m_tSrcCallAddr;
    TAliasAddr          m_tSrcAliasAddr;
    u32					m_dwSrcCRV;
    
    TNETADDR			m_tDstCallAddr;
    TAliasAddr          m_tDstAliasAddr;
    u32					m_dwDstCRV;
    
    u16					m_wSendBandwidth;       //单位：kbps
    u16					m_wRecvBandwidth;       //单位：kbps
    
    time_t				m_dwStartTime;

    u32                 m_dwIRRTimerId;

public:
    void SetIRRTimerId( u32 dwIRRTimerId )
    {
        m_dwIRRTimerId = htonl( dwIRRTimerId );
    }
    u32 GetIRRTimerId()
    {
        return ntohl( m_dwIRRTimerId );
    }
    void SetSrcCallAddr( TNETADDR &tAddr )
    {
        m_tSrcCallAddr = tAddr;
    }
    TNETADDR& GetSrcCallAddr()
    {
        return m_tSrcCallAddr;
    }
    void SetSrcAliasAddr(cmAliasType AliasType, s8 *pszName, u16 wLen )
    {
        m_tSrcAliasAddr.SetAlias( AliasType, pszName, wLen );
    }
    TAliasAddr& GetSrcAliasAddr()
    {
        return m_tSrcAliasAddr;
    }
    void SetSrcCRV( u32 dwCRV )
    {
        m_dwSrcCRV = htonl( dwCRV );
    }
    u32 GetSrcCRV()
    {
        return ntohl( m_dwSrcCRV );
    }

    void SetDstCallAddr( TNETADDR &tAddr )
    {
        m_tDstCallAddr = tAddr;
    }
    TNETADDR& GetDstCallAddr()
    {
        return m_tDstCallAddr;
    }
    void SetDstAliasAddr(cmAliasType AliasType, s8 *pszName, u16 wLen )
    {
        m_tDstAliasAddr.SetAlias(AliasType, pszName, wLen );
    }
    TAliasAddr& GetDstAliasAddr()
    {
        return m_tDstAliasAddr;
    }
    void SetDstCRV( u32 dwCRV )
    {
        m_dwDstCRV = htonl( dwCRV );
    }
    u32 GetDstCRV()
    {
        return ntohl( m_dwDstCRV );
    }

    void SetSendBandwidth( u16 wSend )
    {
        m_wSendBandwidth = htons(wSend);
    }

    void SetRecvBandwidth( u16 wReceive )
    {
        m_wRecvBandwidth = htons(wReceive);
    }

    u16 GetSendBandwidth()
    {
        return ntohs(m_wSendBandwidth);
    }
    u16 GetRecvBandwidth()
    {
        return ntohs(m_wRecvBandwidth);
    }

    void SetStartTime(time_t tStartTime)
    {
        m_dwStartTime = htonl(tStartTime);
    }
    time_t GetStartTime()
    {
        return ntohl(m_dwStartTime);
    }

    tagCallIncInfoNtf()
    {
        m_dwSrcCRV = 0;
        m_dwDstCRV = 0;
        
        m_wSendBandwidth = 0;
        m_wRecvBandwidth = 0;

        m_dwStartTime = 0;
        m_dwIRRTimerId = 0;
    }       
}TCallIncInfoNtf;

//实时呼叫信息数据库中以CallIdentifier为key的结构
typedef struct tagCallIdDBData
{
private:
    TNETADDR			m_tSrcCallAddr;
    TAliasAddr          m_tSrcEpId;
    TAliasAddr          m_tSrcAlias;
    u32					m_dwSrcCRV;
    
    TNETADDR			m_tDstCallAddr;
    TAliasAddr          m_tDstEpId;
    TAliasAddr          m_tDstAlias;
    u32					m_dwDstCRV;
    
    u16					m_wSendBandwidth;       //单位：kbps
    u16					m_wRecvBandwidth;       //单位：kbps
    
    time_t				m_dwStartTime;

    u32                 m_dwIRRTimerId;

    s8  			    m_achConferenceID[LEN_GUID];//会议ID

    u32                 m_dwIRRTTL;
    
    u32                m_dwSrcCall;
    u32                m_dwDstCall;

public:
    void SetIRRTTL(u32 dwTTL)
    {
        m_dwIRRTTL = htonl(dwTTL);
    }
    
    u32 GetIRRTTL()
    {
        return ntohl(m_dwIRRTTL);
    }
    
    void SetIRRTimerId( u32 dwIRRTimerId )
    {
        m_dwIRRTimerId = htonl( dwIRRTimerId );
    }
    u32 GetIRRTimerId()
    {
        return ntohl( m_dwIRRTimerId );
    }
    void SetSrcCallAddr( TNETADDR &tAddr )
    {
        m_tSrcCallAddr = tAddr;
    }
    TNETADDR& GetSrcCallAddr()
    {
        return m_tSrcCallAddr;
    }
    void SetSrcEpId( s8 *pszName, u16 wLen )
    {
        m_tSrcEpId.SetAlias( cmAliasTypeEndpointID, pszName, wLen );
    }
    TAliasAddr& GetSrcEpId()
    {
        return m_tSrcEpId;
    }

    void SetSrcAlias(TAliasAddr tAlias)
    {
        m_tSrcAlias = tAlias;
    }
    
    TAliasAddr& GetSrcAlias()
    {
        return m_tSrcAlias;
    }

    void SetSrcCRV( u32 dwCRV )
    {
        m_dwSrcCRV = htonl( dwCRV );
    }
    u32 GetSrcCRV()
    {
        return ntohl( m_dwSrcCRV );
    }

    void SetDstCallAddr( TNETADDR &tAddr )
    {
        m_tDstCallAddr = tAddr;
    }
    TNETADDR& GetDstCallAddr()
    {
        return m_tDstCallAddr;
    }
    void SetDstEpId( s8 *pszName, u16 wLen )
    {
        m_tDstEpId.SetAlias( cmAliasTypeEndpointID, pszName, wLen );
    }

    TAliasAddr& GetDstEpId()
    {
        return m_tDstEpId;
    }
    
    void SetDstAlias(TAliasAddr tAlias)
    {
        m_tDstAlias = tAlias;
    }

    TAliasAddr& GetDstAlias()
    {
        return m_tDstAlias;
    }

    void SetDstCRV( u32 dwCRV )
    {
        m_dwDstCRV = htonl( dwCRV );
    }
    u32 GetDstCRV()
    {
        return ntohl( m_dwDstCRV );
    }

    void SetSendBandwidth( u16 wSend )
    {
        m_wSendBandwidth = htons(wSend);
    }

    void SetRecvBandwidth( u16 wReceive )
    {
        m_wRecvBandwidth = htons(wReceive);
    }

    u16 GetSendBandwidth()
    {
        return ntohs(m_wSendBandwidth);
    }
    u16 GetRecvBandwidth()
    {
        return ntohs(m_wRecvBandwidth);
    }

    void SetStartTime(time_t tStartTime)
    {
        m_dwStartTime = htonl(tStartTime);
    }
    time_t GetStartTime()
    {
        return ntohl(m_dwStartTime);
    }

    void SetConferenceID(s8* pchConfId)
    {
        memcpy(m_achConferenceID, pchConfId, sizeof(m_achConferenceID));
    }

    s8* GetConferenceID()
    {
        return m_achConferenceID;
    }

    void SetDstCall(HCALL hsCall)
    {
        m_dwDstCall = htonl((u32)hsCall);
    }

    HCALL GetDstCall()
    {
        return (HCALL)ntohl(m_dwDstCall);
    }


    void SetSrcCall(HCALL hsCall)
    {
        m_dwSrcCall = htonl((u32)hsCall);
    }

    HCALL GetSrcCall()
    {
        return (HCALL)ntohl(m_dwSrcCall);
    }

    tagCallIdDBData()
    {
        m_dwSrcCRV = 0;
        m_dwDstCRV = 0;
        
        m_wSendBandwidth = 0;
        m_wRecvBandwidth = 0;

        m_dwStartTime = 0;
        
        m_dwIRRTimerId = 0;

        m_dwSrcCall = NULL;
        m_dwDstCall = NULL;
    }

}TCallIdDBData,*PTCallIdDBData;

//计费与认证
//Radius secret结构 
typedef struct tagRadiusSecret
{  
  private:
    u8  m_abySecret[MAX_RADIU_SECRET_LEN];       //secret : 不写入结束符  
    u16 m_wSecretLen;                           //length :  不包含结束符 
    
  public:
    tagRadiusSecret()
    {
        m_wSecretLen = 0;
        memset(m_abySecret,0,sizeof(m_abySecret));
    }

    /*====================================================================
    函数名	：   RadiusAccessRejectCB
    功能:		设置Secret
    算法实现	：（可选项）
    引用全局变量：
    输入参数说明：pbySecret: secret buf.
                  wLen: secret的长度.不包括结束符.
    返回值说明：  成功返回 TRUE, 否则返回 FALSE.
     ====================================================================*/
    BOOL SetSecret(IN u8* pbySecret, IN u16 wLen)
    {
        if(wLen > MAX_RADIU_SECRET_LEN)
        {
            return FALSE;
        }
        memset(m_abySecret, 0, sizeof(m_abySecret));
        memcpy(m_abySecret, pbySecret, wLen);
        m_wSecretLen = htons(wLen);        
        return TRUE;            
    }

    /*====================================================================
    函数名	：   GetSecret 
    功能:		 获取Secret
    算法实现	：（可选项）
    引用全局变量：
    输入参数说明：pOutBuf: 读取secret的Buf
                  wLen: Buf的长度.
    返回值说明：  成功返回Secret的长度(不包括结束符),否则返回0.
    ====================================================================*/
    u16 GetSecret(IN OUT u8* pOutBuf, IN u16 wBufLen)
    {
        u16 wLen = ntohs(m_wSecretLen);
        if((NULL == pOutBuf)||(wBufLen < wLen))
        {
            return 0;
        }        
        memcpy(pOutBuf,m_abySecret,wLen);
        return wLen;       
    }

    u16 GetSecretLen()
    {
        return ntohs(m_wSecretLen);
    }

}TRadiusSecret, *PTRadiusSecret;


//Radius配置信息 
typedef struct tagRadiusConfigInfo
{
private:        
    u32             m_dwServerIP;         //Radius服务器IP
    u32             m_dwTimeOutInterval;  //重发间隔(毫秒)    
    u16             m_wIsUseRadius;       //是否使用Radius认证    
    u16             m_wAccessPort;        //认证端口号
    u16             m_wAccountPort;       //计费端口号
    u16             m_wRetryNum;          //超时重发次数
    TRadiusSecret   m_tSecret;             //共享密钥

public:
    tagRadiusConfigInfo()
    {
        m_wIsUseRadius = 0;
        m_dwServerIP = inet_addr("127.0.0.1");
        m_wAccessPort = htons(DEFAULT_ACCESS_PORT);
        m_wAccountPort = htons(DEFAULT_ACCT_PORT);
        m_wRetryNum = htons(DEFAULT_RETRY_TIME);
        m_dwTimeOutInterval = htonl(DEFAULT_TIMEOUT_INTERVAL);
        m_tSecret.SetSecret((u8*)DEFAULT_RADIUS_SECRET,strlen(DEFAULT_RADIUS_SECRET));
    }     
    
    void SetUseRadius(u16 wIsUseRadius)
    {
        m_wIsUseRadius = htons(wIsUseRadius);
    }

    u16 GetUseRadius()
    {
        return ntohs(m_wIsUseRadius);
    }      
    
    void SetServerIP(u32 dwServerIP)
    {
        m_dwServerIP = htonl(dwServerIP);
    }

    u32 GetServerIP()
    {
        return ntohl(m_dwServerIP);
    }

    void SetAccessPort(u16 wAccessPort)
    {
        m_wAccessPort = htons(wAccessPort);
    }

    u16 GetAccessPort()
    {
        return ntohs(m_wAccessPort);
    }      

    void SetAccountPort(u16 wAccountPort)
    {
        m_wAccountPort = htons(wAccountPort);
    }

    u16 GetAccountPort()
    {
        return ntohs(m_wAccountPort);
    }

    void SetRetryNum(u16 wRetryNum)
    {
        m_wRetryNum = htons(wRetryNum);
    }

    u16 GetRetryNum()
    {
        return ntohs(m_wRetryNum);
    }

    void SetRetryInterval(u32 dwRetryInterval)
    {
        m_dwTimeOutInterval = htonl(dwRetryInterval);
    }

    u32 GetRetryInterval()
    {
        return ntohl(m_dwTimeOutInterval);
    }      

    void SetSecret(u8* pSecrect, u16 wLen)
    {
        m_tSecret.SetSecret(pSecrect,wLen);
    }     
    
    u16 GetSecret(u8* pBuf, u16 wLen)
    {
        return m_tSecret.GetSecret(pBuf,wLen);
    }                      
}TRadiusConfigInfo, *PTRadiusConfigInfo;

//add by daiqing for pcmt gk permision
typedef struct tagPcmtAuthStatus
{
private:
	u32 m_dwTotalNum;
	u32 m_dwUsedNum;

public:
	tagPcmtAuthStatus()
	{
		m_dwTotalNum = 0;
		m_dwUsedNum = 0;
	}

	void SetPcmtAuthStatus( u32 dwTotalNum , u32 dwUsedNum )
	{
		m_dwTotalNum = htonl(dwTotalNum);
		m_dwUsedNum = htonl(dwUsedNum);
	}

	void GetPcmtAuthStatus( u32* dwTotalNum , u32* dwUsedNum )
	{
		*dwTotalNum = ntohl(m_dwTotalNum);
		*dwUsedNum = ntohl(m_dwUsedNum);
	}

}TPcmtAuthStatus;

//add by yj for support all kinds of mts [20120924]
typedef struct tagmtAuth
{
	s8  m_abyEndpointName[MAX_LEN_EPNAME+1];
	u32 m_dwTotalNum;
	u32 m_dwUsedNum;
	
	tagmtAuth()
	{
		clear();
	}

	void clear()
	{
		memset( m_abyEndpointName , 0 , sizeof(m_abyEndpointName) );
		m_dwTotalNum = 0;
		m_dwUsedNum = 0;
	}

	void SetMtsAuth( u32 dwTotalNum, u32 dwUsedNum, s8* EndpointName )
	{
		m_dwTotalNum = htonl(dwTotalNum);
		m_dwUsedNum = htonl(dwUsedNum);
		memcpy( m_abyEndpointName, EndpointName, strlen( EndpointName )+1 );
	}

	void GetMtsAuth( u32* dwTotalNum, u32* dwUsedNum, s8* EndpointName )
	{
		*dwTotalNum = ntohl(m_dwTotalNum);
		*dwUsedNum = ntohl(m_dwUsedNum);
		memcpy( EndpointName, m_abyEndpointName, strlen( m_abyEndpointName )+1 );
	}

}TMtAuth;
//end [20120924]

//add by yj [20120807]
//获取GK设备型号及MAC地址
typedef struct tagMACAndProductid
{
	s8		m_abymacaddress[18];			//MAC地址
	cmGKPruductID		m_dwProductId;	//产品号

	tagMACAndProductid()
	{
		memset(m_abymacaddress, 0, 18);
		m_dwProductId = cmGKPruduct_Unknown;
	}

	void SetProductId( cmGKPruductID  productid )
	{
		m_dwProductId = (cmGKPruductID)htonl( productid );
	}
	
	cmGKPruductID GetProductID()
	{
		return (cmGKPruductID)ntohl( m_dwProductId );
	}

}TMACAndProductid;
//end [20120807]

//add by yj for BrdType [20130227]
//获取单板GK型号
typedef struct tagBrdType
{
	cmGKBrdType		m_dwBrdType;	//产品号
	
	tagBrdType()
	{
		m_dwBrdType = cmGK_BrdType_Unknown;
	}
	
	void SetBrdType( cmGKBrdType  dwBrdType )
	{
		m_dwBrdType = (cmGKBrdType)htonl( dwBrdType );
	}
	
	cmGKBrdType GetBrdType()
	{
		return (cmGKBrdType)ntohl( m_dwBrdType );
	}
	
}TBrdType;
//end [20130227]

//
//////////////////////////////// 函数接口 //////////////////////////////
API void setgklog(s32 nLevel);
//日志函数
void KdvGKRASLog(s32 nLevel, s8 *pchFormat,...);

//压缩别名
inline BOOL EncodeAlias(IN TAliasAddr *ptAlias, IN u16 wAliasNum, 
                        OUT u8 *pbyOutBuf, OUT u32 *pdwBufLen)
{
    if ( ptAlias == NULL || pbyOutBuf == NULL || pdwBufLen == NULL )
        return FALSE;

    u8 *pbyPos = pbyOutBuf;
    u32 bufLen = 0;

    for( u16 wIndex = 0; wIndex < wAliasNum; wIndex++)
    {
        s8 aliasName[MAX_ALIASNAME_LEN+1] = {0};
        
        u32 dwAliasType = ptAlias[wIndex].GetAliasType();
        
        u16 wAliasNameLen = ptAlias[wIndex].GetAliasName( aliasName, MAX_ALIASNAME_LEN );
        u16 wAliasLen = wAliasNameLen + sizeof(cmAliasType);

        //别名总长: 别名类型＋别名长度
        u16 aliasLen = htons(wAliasLen);
        memcpy(pbyPos, &aliasLen, sizeof(u16));
        pbyPos += sizeof(u16);

        //别名类型
        u32 aliasType = htonl(dwAliasType);
        memcpy(pbyPos, &aliasType, sizeof(u32));
        pbyPos += sizeof(u32);
        
        //别名长度
        memcpy(pbyPos, aliasName, wAliasNameLen);
        pbyPos += wAliasNameLen;

        bufLen += sizeof(u16) + wAliasLen;
    }

    *pdwBufLen = bufLen;
    return TRUE;
}

//恢复别名结构
inline BOOL DecodeAlias(IN u8 *pbyAliasBuf, IN u16 wAliasNum, 
                        IN u32 dwAliasBufLen, OUT TAliasAddr atAlias[])
{
    if (pbyAliasBuf == NULL || atAlias == NULL)
        return FALSE;

    u8 *ptPos = (u8*)pbyAliasBuf;
    u32 dwTotalLen = 0;

    for(u16 wIndex = 0; wIndex < wAliasNum; wIndex++)
    {
        //别名总长: 别名类型＋别名长度
        //for Davinci alignment error
        u16 wTmp = 0;
        memcpy(&wTmp,ptPos,sizeof(u16));
        //u16 wAliasLen = ntohs(*(u16*)ptPos);
        u16 wAliasLen = ntohs(wTmp);
        ptPos += sizeof(u16);

        //别名类型
        u32 dwTmp = 0;
        memcpy(&dwTmp,ptPos,sizeof(u32));
        u32 aliasType = ntohl(dwTmp);
        //u32 aliasType = ntohl(*(u32*)ptPos);
        ptPos += sizeof(u32);

        //别名长度
        u16 wAliasnameLen = wAliasLen - sizeof(cmAliasType);
                    
        atAlias[wIndex].SetAliasType((cmAliasType)aliasType);
        atAlias[wIndex].SetAliasName((s8*)ptPos, wAliasnameLen);

        ptPos += wAliasnameLen;

        dwTotalLen += sizeof(u16) + wAliasLen;
    }

    if ( dwTotalLen != dwAliasBufLen )
        return FALSE;

    return TRUE;
}

s32 MainStartGk( s8* pszGkFile);


#endif //__KDVGK_H
