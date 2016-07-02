#ifndef _KDVDNS_H
#define _KDVDNS_H

#include "kdvtype.h"
#include "osp.h"

/************************** DNS Trans *****************************************/
typedef enum
{
    emDNS_SUCCESS = 0,
	emDNS_TIMEOUT,
	emDNS_NOTFOUND,
	emDNS_FAILED,
} emDnsResult;

#define KDVDNSERROR		(s32)-1
#define KDVDNSOK		(s32)0

typedef s32 (*KdvDnsCallBackT)(s32 nUserID, emDnsResult eResult, s32 nIpNum, u32* adwIP);


/*====================================================================
函	 数   名: KdvDnsInit
功	      能: DNS模块初始化
算 法 实  现: 
引用全局变量: 
输入参数说明: 
返   回   值: 成功 - KDVDNSOK
			  失败 - KDVDNSERROR
====================================================================*/
int KdvDnsInit();

/*====================================================================
函	 数   名: KdvDnsEnd
功	      能: DNS模块结束
算 法 实  现: 
引用全局变量: 
输入参数说明: 
返   回   值: 无
====================================================================*/
void KdvDnsEnd();


/*====================================================================
函	 数   名: KdvDnsQuery
功	      能: 设置gk地址
算 法 实  现: 
引用全局变量: 
输入参数说明: pchName - 需查询的目标域名
			  dwDnsServer   - 查询的DNS服务器地址
              dwUserID	- 用户数据
			  ptDnsCallbackFunc - 回调函数指针
			  bSync - 信号量
返   回   值: 成功 - KDVDNSOK
			  失败 - KDVDNSERROR
====================================================================*/
API int KdvDnsQuery(s8* pchName, u32 dwDnsServer, u32 dwUserID, KdvDnsCallBackT ptDnsCallbackFunc, BOOL32 bSync);


/*====================================================================
函数名      : kdvdnshelp
功能        : 打印kdvdnshelp帮助和版本信息
算法实现    : 无
引用全局变量: 无
输入参数说明: 
返回值说明  : 无
====================================================================*/
API void kdvdnshelp();

/*====================================================================
函数名      : dnsloglevel
功能        : 设置调试打印级别
算法实现    : 无
引用全局变量: 无
输入参数说明: u8 byLevel 新调试级别
返回值说明  : 无
====================================================================*/
API void dnsloglevel(u8 byLevel);

/*====================================================================
函数名      : dnsfind
功能        : 调试查询
算法实现    : 无
引用全局变量: 无
输入参数说明:  
返回值说明  : 无
====================================================================*/
API void dnsfind(s8* pchName, s8* pchServer);

#endif//_KDVDNS_H