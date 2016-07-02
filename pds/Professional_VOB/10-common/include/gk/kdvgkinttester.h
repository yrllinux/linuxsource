/////////////////////////////////////////////////////////
// GKTester.h
//
// Interface of CGKTester.
// require header (s):	osp.h	kdvgk.h
/////////////////////////////////////////////////////////

#ifndef __GKTESTER_H
#define __GKTESTER_H

//接口查询失败返回值
#define GKINT_ERROR  (int)-1

//
// class CGKTester
// GK集成测试类
//
// @ 用法：
// 		# 调用Init()函数初始化
// 		# 调用UnInit()
//		# 调用相应的测试函数进行测试
//
class CGKTester
{
public:
	CGKTester();
	
public:
	// initialize
	// GKIP : 网络序
	BOOL Init(DWORD GKIP);
	// uninit
	void UnInit();

public:
    
/*=============================================================================
  函 数 名： GetBaseCfg
  功    能： 得到GK基本配置信息 
  算法实现： 
  全局变量： 
  参    数： TConfigBaseGKDBData tBaseConfig [in/out] GK基本配置
  返 回 值： FALSE : 查询失败 ， TRUE：查询成功
=============================================================================*/
	BOOL GetBaseCfg( TConfigBaseGKDBData &tBaseConfig );

/*=============================================================================
  函 数 名： IsUserExist
  功    能： 查询控制台用户是否存在  
  参    数： const char * User [in] 用户名
             BYTE UserLen		 [in] 用户名长度	
  返 回 值： GKINT_ERROR: 查询失败
			 0			: 不存在
			 1			：存在
=============================================================================*/
	int IsUserExist(const char * User, BYTE UserLen);
 
/*=============================================================================
  函 数 名： IsUserLoginged
  功    能： 查询用户是否登录  
  参    数： const char * User [in] 用户名
             BYTE UserLen	   [in] 用户名长度
  返 回 值： GKINT_ERROR: 查询失败
			 0			: 没有登录
			 1			：已经登录
=============================================================================*/
	int IsUserLoginged(const char * User,BYTE UserLen);

/*=============================================================================
  函 数 名： IsRegIPRngExist
  功    能： 查询可注册IP范围是否存在  
  参    数： const TAddrRng & tIPRng [in] IP范围
  返 回 值： GKINT_ERROR: 查询失败
			  0			: 不存在
			  1			：存在
=============================================================================*/
	int IsRegIPRngExist(const TAddrRng & tIPRng);

/*=============================================================================
  函 数 名： IsForbidIPRngExist
  功    能： 查询禁止IP范围是否存在
  算法实现： 
  全局变量： 
  参    数： const TAddrRng & tIPRng [in] IP范围
  返 回 值： GKINT_ERROR: 查询失败
			  0			: 不存在
			  1			：存在
=============================================================================*/
	int IsForbidIPRngExist(const TAddrRng & tIPRng);

/*=============================================================================
  函 数 名： IsBSBExist
  功    能： 查询网段间带宽是否存在  
  参    数： const TConfigBandWidthDBData & tCfgBWDBData [in] 网段带宽信息
  返 回 值： GKINT_ERROR: 查询失败
			 0			: 不存在
			 1			：存在
=============================================================================*/
	int IsBSBExist(const TConfigBandWidthDBData & tCfgBWDBData);

/*=============================================================================
  函 数 名： IsAllowedEPInDB
  功    能： 查询允许的H.323实体是否在DB中
  参    数： const TAliasAuthKey & tAliasArray [in] 实体认证信息
  返 回 值： GKINT_ERROR: 查询失败
			  0			: 不存在
			  1			：存在
=============================================================================*/
	int IsAllowedEPInDB(const TAliasAuthKey & tAliasArray);

/*=============================================================================
  函 数 名： IsForbidEPInDB
  功    能： 查询禁止的H.323实体是否在DB中
  参    数： const TAliasAuthKey & tAliasArray [in] 实体认证信息
  返 回 值： GKINT_ERROR: 查询失败
			  0			: 不存在
			  1			：存在
=============================================================================*/
	int IsForbidEPInDB(const TAliasAuthKey & tAliasArray);

/*=============================================================================
  函 数 名： GetOnlineEndPointCount
  功    能： 查询在线实体列表中的实体数量   
  参    数： DWORD dwCount [in/out]  在线实体个数
  返 回 值： FALSE : 查询失败 ， TRUE：查询成功
=============================================================================*/
	BOOL GetOnlineEndPointCount(DWORD &dwCount);

/*=============================================================================
  函 数 名： IsEndpointRegistered
  功    能： 查询H.323实体是否已注册, 即是否此实体在GK的在线实体列表中
  参    数： const TAliasAddr & tAlias [in] 实体别名
  返 回 值： GKINT_ERROR: 查询失败
			  0			: 不存在
			  1			：存在
=============================================================================*/
	int IsEndpointRegistered(const TAliasAddr & tAlias);

/*=============================================================================
  函 数 名： IsCallExist
  功    能： 查询呼叫是否存在
  参    数： const u32 dwTimerId   [in] 要查询的呼叫TimerId
  返 回 值： GKINT_ERROR: 查询失败
			  0			: 不存在
			  1			：存在
=============================================================================*/
	int IsCallExist( const u32 dwTimerId );
    
/*=============================================================================
  函 数 名： IsZoneExist
  功    能： 前缀(区号)是否存在   
  参    数： const char *pZone [in] 前缀
             BYTE byLen		 [in] 长度
  返 回 值： GKINT_ERROR: 查询失败
			 0			: 不存在
			 1			：存在
=============================================================================*/
	int IsPrefixExist(const char *pPrefix, BYTE byLen);

/*=============================================================================
  函 数 名： GetNeighborGKInfo
  功    能： 查询邻居GK信息
  参    数： TNeighborGKInfo &tNeighborGK   [in] 邻居GK信息
  返 回 值： FALSE : 查询失败 ， TRUE：查询成功
=============================================================================*/
	BOOL GetNeighborGKInfo(TNeighborGKInfo &tNeighborGK);

/*=============================================================================
  函 数 名： GetEpAliasByIP
  功    能： 由实体类型、ras地址和call地址查找别名列表 
  参    数： cmEndpointType &epType           [in]      实体类型
             TNETADDR &cRasAddr               [in]      ras地址
             TNETADDR &cCallAddr              [in]      call地址
             TAliasAddr *pByAliasBuf          [in/out]  别名
             u32 &dwAliasNum                  [in]      别名个数
             
  返 回 值： FALSE : 查询失败 ， TRUE：查询成功  
=============================================================================*/
    BOOL GetEpAliasByIP(cmEndpointType &epType, TNETADDR &cRasAddr, TNETADDR &cCallAddr,
                        TAliasAddr *pByAliasBuf, u32 &dwAliasNum);
	
/*=============================================================================
  函 数 名： ClearAllConfig
  功    能： 清空所有控制台配置信息（用户管理保留admin默认用户）
  参    数： 无
  返 回 值： FALSE : 失败 ， TRUE：成功
=============================================================================*/
	BOOL ClearAllConfig();

/**/
/*=============================================================================
  函 数 名： GetLoginedUserCount
  功    能： 查询当前已登录的控制台用户数量(主机序)
  参    数： DWORD dwCount [in/out]  用户数量
  返 回 值： FALSE : 查询失败 ， TRUE：查询成功
=============================================================================*/
	BOOL GetLoginedUserCount(DWORD &dwCount);

/*=============================================================================
  函 数 名： RebootGK
  功    能： 是否重启成功
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： FALSE : 操作失败 ， TRUE：操作成功			 
=============================================================================*/
	BOOL RebootGK();

private:
	BOOL ConnectToCtrlSessioner();
	BOOL DisconnectFromCtrlSessioner();
	BOOL SendMsg( WORD dwMsgId, const void *pContent, u16 wContentLen, 
		          void *pAckBuf, u16 &wAckBufLen, BOOL bSendToCtrl = TRUE );

private:
	DWORD 	m_GKIP;     // GK ip地址
	DWORD 	m_GKNodeId; // 节点id
};

#endif // __GKTESTER_H
