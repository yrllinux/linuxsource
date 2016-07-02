/*****************************************************************************
   模块名      : GKCUI3.0
   文件名      : GKCInteTestMacro.h
   相关文件    : 无
   文件实现功能: GKConsole集成测试常量定义
   作者		   : 陈洪斌
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本     修改人      修改内容
   2004/09/10  1.0      陈洪斌      创建
******************************************************************************/
#ifndef GKC_TEST_MACRO_H_
#define GKC_TEST_MACRO_H_

#include "eventid.h"

#define GKC_INTE_TEST_APP_ID 234
//获取已注册实体数量
OSPEVENT( GKC_INTE_REG_ENDPOINT_COUNT_QUERY , EV_TEST_GKC_BGN + 1 );
//获取某项已注册实体的头信息
OSPEVENT( GKC_INTE_REG_ENDPOINT_HEAD_QUERY ,  EV_TEST_GKC_BGN + 2 );
//获取某项已注册实体的别名信息
OSPEVENT( GKC_INTE_REG_ENDPOINT_ALIAS_QUERY ,  EV_TEST_GKC_BGN + 3 );
//获取某项已注册实体的呼叫地址信息
OSPEVENT( GKC_INTE_REG_ENDPOINT_CALLADDR_QUERY ,  EV_TEST_GKC_BGN + 4 );
//获取实体呼叫的数量
OSPEVENT( GKC_INTE_CALL_COUNT_QUERY , EV_TEST_GKC_BGN + 5 );
//获取某项实体呼叫的具体信息
OSPEVENT( GKC_INTE_CALL_ITEM_QUERY , EV_TEST_GKC_BGN + 6 );
//判断某个已注册实体是否存在(别名匹配)
OSPEVENT( GKC_INTE_IS_REG_ENDPOINT_EXIST_QUERY_BY_ALIAS , EV_TEST_GKC_BGN + 7 );
//判断某个已注册实体是否存在(Ras地址匹配)
OSPEVENT( GKC_INTE_IS_REG_ENDPOINT_EXIST_QUERY_BY_RAS_ADDR , EV_TEST_GKC_BGN + 8 );
//判断某个实体呼叫是否存在
OSPEVENT( GKC_INTE_IS_CALL_EXIST_QUERY, EV_TEST_GKC_BGN + 9 );

//获取前缀（区号）数量
OSPEVENT( GKC_INTE_PREFIX_COUNT_QUERY , EV_TEST_GKC_BGN + 11 );
//获取某项前缀（区号）的具体信息
OSPEVENT( GKC_INTE_PREFIX_ITEM_QUERY , EV_TEST_GKC_BGN + 12 );
//获取临近GK服务器数量
OSPEVENT( GKC_INTE_NEIGHBOR_GK_COUNT_QUERY , EV_TEST_GKC_BGN + 13 );
//获取某项临近GK服务器的具体信息
OSPEVENT( GKC_INTE_NEIGHBOR_GK_ITEM_QUERY , EV_TEST_GKC_BGN + 14 );
//获取GK注册认证策略
OSPEVENT( GKC_INTE_AUTH_METHOD_QUERY , EV_TEST_GKC_BGN + 15 );
//检查某个前缀（区号）是否存在
OSPEVENT( GKC_INTE_IS_PREFIX_EXIST_QUERY , EV_TEST_GKC_BGN + 16);
//检查临近GK服务器是否存在
OSPEVENT( GKC_INTE_IS_NEIGHBOR_GK_EXIST_QUERY , EV_TEST_GKC_BGN + 17);
//检查GK注册认证策略是否匹配
OSPEVENT( GKC_INTE_IS_AUTH_METHOD_MATCH_QUERY , EV_TEST_GKC_BGN + 18);

//获取网段带宽数量
OSPEVENT( GKC_INTE_BANDWIDTH_COUNT_QUERY , EV_TEST_GKC_BGN + 21 );
//获取某项网段带宽的具体信息
OSPEVENT( GKC_INTE_BANDWIDTH_ITEM_QUERY , EV_TEST_GKC_BGN + 22 );
//检查某个网段带宽是否存在
OSPEVENT( GKC_INTE_IS_BANDWIDTH_EXIST_QUERY , EV_TEST_GKC_BGN + 23 );

//获取认证Ip范围数量
OSPEVENT( GKC_INTE_IP_RANGE_COUNT_QUERY , EV_TEST_GKC_BGN + 26 );
//获取某项认证Ip范围信息
OSPEVENT( GKC_INTE_IP_RANGE_ITEM_QUERY , EV_TEST_GKC_BGN + 27 );
//检查某个认证Ip范围是否存在
OSPEVENT( GKC_INTE_IS_IP_RANGE_EXIST_QUERY , EV_TEST_GKC_BGN + 28 );

//获取认证实体别名数量
OSPEVENT( GKC_INTE_ENDPOINT_ALIAS_COUNT_QUERY , EV_TEST_GKC_BGN + 31);
//获取某项认证实体别名具体信息
OSPEVENT( GKC_INTE_ENDPOINT_ALIAS_ITEM_QUERY , EV_TEST_GKC_BGN + 32 );
//检查某个认证实体别名是否存在
OSPEVENT( GKC_INTE_IS_ENDPOINT_ALIAS_EXIST_QUERY , EV_TEST_GKC_BGN + 33 );

//获取用户数量
OSPEVENT( GKC_INTE_USER_COUNT_QUERY , EV_TEST_GKC_BGN + 41);
//获取某项用户具体信息
OSPEVENT( GKC_INTE_USER_ITEM_QUERY , EV_TEST_GKC_BGN + 42 );
//检查某个用户是否存在,仅验证用户名称(仅管理员用户登录有效)
OSPEVENT( GKC_INTE_IS_USER_EXIST_QUERY , EV_TEST_GKC_BGN + 43 );
//检查某个用户信息是否匹配,需要验证用户名称和密码(仅管理员用户登录有效)
OSPEVENT( GKC_INTE_IS_USER_MATCH_QUERY , EV_TEST_GKC_BGN + 44 );

//是否登录Gkc
OSPEVENT( GKC_INTE_IS_GKC_LOGIN_QUERY , EV_TEST_GKC_BGN + 46 );
//获取当前登录的用户名称和密码
OSPEVENT( GKC_INTE_GET_LOGIN_USER_QUERY , EV_TEST_GKC_BGN + 47 );

#endif //GKC_INTE_TEST_MACRO_H_