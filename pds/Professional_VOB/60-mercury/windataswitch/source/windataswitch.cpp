
#include "windataswitch.h"

/*=============================================================================
  函 数 名： dsCreate
  功    能： dataswitch-win32 初始化
  算法实现： 
  全局变量： 
  参    数： u32 dwIfNum				IP个数
			 TDSNetAddr *ptDsNetAddr    IP地址
  返 回 值： DSID 
  ----------------------------------------------------------------------
  修改记录    ：
  日  期		版本		修改人		走读人    修改内容
  2012/06/11    4.7	        周嘉麟				  创建
=============================================================================*/
DSID dsCreate( u32 dwIfNum,  TDSNetAddr *ptDsNetAddr)
{
	u32 dwRet = DSOK;
	if (0 == dwIfNum)
	{
		printf("[dsCreate] dwIfNum = 0!\n");
		return DSERROR;
	}

	if (NULL == ptDsNetAddr)
	{
		return DSERROR;
	}

	if (DS_TYPE_IPV4 == ptDsNetAddr->GetType())
	{   
		u32 dwV4IP = htonl(ptDsNetAddr->GetV4IPAddress());
		dwRet = dsCreate(1, &dwV4IP);
	}
	else
	{
		printf("[dsCreate] unexpected socktype.%d!\n", ptDsNetAddr->GetType());
		return DSERROR;
	}

	return dwRet;
}

/*=============================================================================
  函 数 名： dsAdd
  功    能： dataswitch-win32 增加一对一规则
  算法实现： 
  全局变量： 
  参    数： DSID dsId					ds句柄
		     TDSNetAddr* ptRcvAddr		转发地址
		     TDSNetAddr* ptInLocalIP    本地接收
		     TDSNetAddr* ptSendtoAddr   目的地址
		     TDSNetAddr* ptOutLocalIP   本地发送
  返 回 值： u32 
  ----------------------------------------------------------------------
  修改记录    ：
  日  期		版本		修改人		走读人    修改内容
  2012/06/11    4.7	        周嘉麟				  创建
=============================================================================*/
u32  dsAdd(DSID dsId,
				TDSNetAddr* ptRcvAddr,
				TDSNetAddr* ptInLocalIP,
				TDSNetAddr* ptSendtoAddr,
				TDSNetAddr* ptOutLocalIP)
{
	if (NULL == ptRcvAddr)
	{
		OspPrintf(1, 0, "[dsAdd] ptRcvAddr is null!\n");
		return DSERROR;
	}

	if (NULL == ptSendtoAddr)
	{
		OspPrintf(1, 0, "[dsAdd] ptSendtoAddr is null!\n");
		return DSERROR;
	}
	
	u32 dwRcvIP  = ptRcvAddr->GetV4IPAddress();
	u16 wRcvPort = ptRcvAddr->GetPort();
	u32 dwDstIP  = ptSendtoAddr->GetV4IPAddress();
	u16 wDstPort = ptSendtoAddr->GetPort();

	u32 dwRet = DSOK;
	dwRet = dsAdd( dsId, htonl(dwRcvIP), wRcvPort, 0, 
						 htonl(dwDstIP), wDstPort, 0);
	return dwRet;
}

/*=============================================================================
  函 数 名： dsSetSendCallback
  功    能： dataswitch-win32 发送回调
  算法实现： 
  全局变量： 
  参    数： DSID dsId					ds句柄
		     TDSNetAddr*  ptRcvAddr		转发地址
		     TDSNetAddr*  ptSrcAddr,    源地址
			 SendCallback pfCallback    回调
  返 回 值： u32 
  ----------------------------------------------------------------------
  修改记录    ：
  日  期		版本		修改人		走读人    修改内容
  2012/06/11    4.7	        周嘉麟				  创建
=============================================================================*/
u32 dsSetSendCallback( DSID dsId, 
							TDSNetAddr* ptRcvAddr, 
							TDSNetAddr* ptSrcAddr,
							SendCallback pfCallback)
{
	if (NULL == ptRcvAddr)
	{
		OspPrintf(1, 0, "[dsSetSendCallback] ptRcvAddr is null!\n");
		return DSERROR;
	}

	u32 dwRcvIP  = ptRcvAddr->GetV4IPAddress();
	u16 wRcvPort = ptRcvAddr->GetPort();

	u32 dwRet = dsSetSendCallback( dsId, htonl(dwRcvIP), wRcvPort, 0, 0, pfCallback);
	return DSOK;
}

/*=============================================================================
  函 数 名： dsSetAppDataForSend
  功    能： dataswitch-win32 为发送目标设置一个自定义的指针 
  算法实现： 
  全局变量： 
  参    数： DSID dsId					ds句柄
		     TDSNetAddr*  ptRcvAddr		转发地址
		     TDSNetAddr*  ptSrcAddr,	源地址
			 TDSNetAddr*  ptDstAddr,    目的地址
			 void *       pAppData      自定义指针  
  返 回 值： u32 
  ----------------------------------------------------------------------
  修改记录    ：
  日  期		版本		修改人		走读人    修改内容
  2012/06/11    4.7	        周嘉麟				  创建
=============================================================================*/
u32 dsSetAppDataForSend( DSID dsId, 
							TDSNetAddr* ptRcvAddr,
							TDSNetAddr* ptSrcAddr,
							TDSNetAddr* ptDstAddr, 
							void * pAppData)
{
	if (NULL == ptRcvAddr)
	{
		OspPrintf(1, 0, "[dsSetAppDataForSend] ptRcvAddr is null!\n");
		return DSERROR;
	}

	if (NULL == ptDstAddr)
	{
		OspPrintf(1, 0, "[dsSetAppDataForSend] ptDstAddr is null!\n");
		return DSERROR;
	}

	u32 dwRcvIP  = ptRcvAddr->GetV4IPAddress();
	u16 wRcvPort = ptRcvAddr->GetPort();
	u32 dwDstIP  = ptDstAddr->GetV4IPAddress();
	u16 wDstPort = ptDstAddr->GetPort();

	u32 dwRet = DSOK;
	dwRet = ::dsSetAppDataForSend( dsId, htonl(dwRcvIP), wRcvPort, 0, 0,
										 htonl(dwDstIP), wDstPort, pAppData);
	
	return dwRet;
}

/*=============================================================================
  函 数 名： dsRemove
  功    能： dataswitch-win32 删除转发目标为指定地址的转发规则
  算法实现： 
  全局变量： 
  参    数： DSID       dsId					ds句柄
			TDSNetAddr* ptSendtoAddr			目的地址
  返 回 值： u32 
  ----------------------------------------------------------------------
  修改记录    ：
  日  期		版本		修改人		走读人    修改内容
  2012/06/11    4.7	        周嘉麟				  创建
=============================================================================*/
u32 dsRemove(DSID dsId, TDSNetAddr* ptSendtoAddr)
{
	if (NULL == ptSendtoAddr)
	{
		OspPrintf(1, 0, "[dsRemove] ptSendtoAddr is null!\n");
		return DSERROR;
	}
	u32 dwRcvIP  = ptSendtoAddr->GetV4IPAddress();
	u16 wRcvPort = ptSendtoAddr->GetPort();

	u32 dwRet = DSOK;
	dwRet = dsRemove( dsId, htonl(dwRcvIP), wRcvPort );
	return dwRet;
}

/*=============================================================================
  函 数 名： dsAddManyToOne
  功    能： dataswitch-win32 增加多对一规则
  算法实现： 
  全局变量： 
  参    数： DSID        dsId		    ds句柄
			 TDSNetAddr* ptRcvAddr		转发地址
			 TDSNetAddr* ptInLocalIP    本地接收
			 TDSNetAddr* ptSendtoAddr   目的地址
		     TDSNetAddr* ptOutLocalIP   本地发送
  返 回 值： u32 
  ----------------------------------------------------------------------
  修改记录    ：
  日  期		版本		修改人		走读人    修改内容
  2012/06/11    4.7	        周嘉麟				  创建
=============================================================================*/
u32 dsAddManyToOne(DSID dsId ,
						TDSNetAddr* ptRecvAddr,
						TDSNetAddr* ptInLocalIP,
					    TDSNetAddr* ptSendtoAddr,
						TDSNetAddr* ptOutLocalIP)
{
	if (NULL == ptRecvAddr)
	{
		OspPrintf(1, 0, "[dsAddManyToOne] ptRecvAddr is null!\n");
		return DSERROR;
	}

	if (NULL == ptSendtoAddr)
	{
		OspPrintf(1, 0, "[dsAddManyToOne] ptSendtoAddr is null!\n");
		return DSERROR;
	}
	
	u32 dwRcvIP  = ptRecvAddr->GetV4IPAddress();
	u16 wRcvPort = ptRecvAddr->GetPort();
	u32 dwDstIP  = ptSendtoAddr->GetV4IPAddress();
	u16 wDstPort = ptSendtoAddr->GetPort();

	u32 dwRet = DSOK;
	dwRet = ::dsAddManyToOne( dsId, htonl(dwRcvIP), wRcvPort, 0, 
                                    htonl(dwDstIP), wDstPort, 0 );
	return DSOK;
}

/*=============================================================================
  函 数 名： dsSpecifyFwdSrc
  功    能： dataswitch-win32 为指定接收地址设置转发数据包所填充的源地址
  算法实现： 
  全局变量： 
  参    数： DSID        dsId		    ds句柄
			 TDSNetAddr* ptOrigAddr,	原始地址
			 TDSNetAddr* ptMappedAddr	映射地址
  返 回 值： u32 
  ----------------------------------------------------------------------
  修改记录    ：
  日  期		版本		修改人		走读人    修改内容
  2012/06/11    4.7	        周嘉麟				  创建
=============================================================================*/
u32 dsSpecifyFwdSrc(DSID dsId, 
					     TDSNetAddr* ptOrigAddr, 
						 TDSNetAddr* ptMappedAddr)
{
	if (NULL == ptOrigAddr)
	{
		OspPrintf(1, 0, "[dsSpecifyFwdSrc] ptOrigAddr is null!\n");
		return DSERROR;
	}

	if (NULL == ptMappedAddr)
	{
		OspPrintf(1, 0, "[dsSpecifyFwdSrc] ptMappedAddr is null!\n");
		return DSERROR;
	}
	
	u32 dwRcvIP  = ptOrigAddr->GetV4IPAddress();
	u16 wRcvPort = ptOrigAddr->GetPort();
	u32 dwMapIP  = ptMappedAddr->GetV4IPAddress();
	u16 wMapPort = ptMappedAddr->GetPort();

	u32 dwRet = DSOK;
	dwRet = dsSpecifyFwdSrc( dsId, htonl(dwRcvIP), wRcvPort, htonl(dwMapIP), wMapPort);
	return dwRet;
}

/*=============================================================================
  函 数 名： dsRemoveManyToOne
  功    能： dataswitch-win32 移除多点到一点规则
  算法实现： 
  全局变量： 
  参    数： DSID        dsId		    ds句柄
			 TDSNetAddr* ptRecvAddr 	转发地址
			 TDSNetAddr* ptSendtoAddr	目的地址
  返 回 值： u32 
  ----------------------------------------------------------------------
  修改记录    ：
  日  期		版本		修改人		走读人    修改内容
  2012/06/11    4.7	        周嘉麟				  创建
=============================================================================*/
u32 dsRemoveManyToOne(DSID dsId ,
					       TDSNetAddr* ptRecvAddr,
                           TDSNetAddr* ptSendtoAddr)
{
	if (NULL == ptRecvAddr)
	{
		OspPrintf(1, 0, "[dsRemoveManyToOne] ptRecvAddr is null!\n");
		return DSERROR;
	}

	if (NULL == ptSendtoAddr)
	{
		OspPrintf(1, 0, "[dsRemoveManyToOne] ptSendtoAddr is null!\n");
		return DSERROR;
	}

	u32 dwRcvIP  = ptRecvAddr->GetV4IPAddress();
	u16 wRcvPort = ptRecvAddr->GetPort();
	u32 dwDstIP  = ptSendtoAddr->GetV4IPAddress();
	u16 wDstPort = ptSendtoAddr->GetPort();
	
	u32 dwRet = DSOK;
	dwRet = dsRemoveManyToOne( dsId, htonl(dwRcvIP), wRcvPort, htonl(dwDstIP), wDstPort);
	return dwRet;
}

/*=============================================================================
  函 数 名： dsRemoveAllManyToOne
  功    能： dataswitch-win32 删除所有转发目标为指定地址的多对一规则
  算法实现： 
  全局变量： 
  参    数： DSID        dsId		    ds句柄
			 TDSNetAddr* ptSendtoAddr	目的地址
  返 回 值： u32 
  ----------------------------------------------------------------------
  修改记录    ：
  日  期		版本		修改人		走读人    修改内容
  2012/06/11    4.7	        周嘉麟				  创建
=============================================================================*/
u32 dsRemoveAllManyToOne(DSID dsId, TDSNetAddr* ptSendtoAddr)
{
	if (NULL == ptSendtoAddr)
	{
		OspPrintf(1, 0, "[dsRemoveAllManyToOne] ptSendtoAddr is null!\n");
		return DSERROR;
	}
	
	u32 dwDstIP  = ptSendtoAddr->GetV4IPAddress();
	u16 wDstPort = ptSendtoAddr->GetPort();
	
	u32 dwRet = DSOK;
	dwRet = dsRemoveAllManyToOne(dsId, htonl(dwDstIP), wDstPort);
	return dwRet;
}

/*=============================================================================
  函 数 名： dsAddDump
  功    能： dataswitch-win32 增加dump规则
  算法实现： 
  全局变量： 
  参    数： DSID        dsId		    ds句柄
			 TDSNetAddr* ptRecvAddr     转发地址
			 TDSNetAddr* ptInLocalIP	本地地址
  返 回 值： u32 
  ----------------------------------------------------------------------
  修改记录    ：
  日  期		版本		修改人		走读人    修改内容
  2012/06/11    4.7	        周嘉麟				  创建
=============================================================================*/
u32 dsAddDump(DSID dsId, TDSNetAddr* ptRecvAddr, TDSNetAddr* ptInLocalIP)
{
	if (NULL == ptRecvAddr)
	{
		OspPrintf(1, 0, "[dsAddDump] ptRecvAddr is null!\n");
		return DSERROR;
	}

	u32 dwRcvIP  = ptRecvAddr->GetV4IPAddress();
	u16 wRcvPort = ptRecvAddr->GetPort();

	u32 dwRet = DSOK;
	dwRet = dsAddDump(dsId, htonl(dwRcvIP), wRcvPort, 0);
	return dwRet;
}

/*=============================================================================
  函 数 名： dsRemoveDump
  功    能： dataswitch-win32 移除dump规则
  算法实现： 
  全局变量： 
  参    数： DSID        dsId		    ds句柄
			 TDSNetAddr* ptRecvAddr     转发地址
  返 回 值： u32 
  ----------------------------------------------------------------------
  修改记录    ：
  日  期		版本		修改人		走读人    修改内容
  2012/06/11    4.7	        周嘉麟				  创建
=============================================================================*/
u32 dsRemoveDump(DSID dsId, TDSNetAddr* ptRecvAddr)
{
	if (NULL == ptRecvAddr)
	{
		OspPrintf(1, 0, "[dsRemoveDump] ptRecvAddr is null!\n");
		return DSERROR;
	}
	u32 dwRcvIP  = ptRecvAddr->GetV4IPAddress();
	u16 wRcvPort = ptRecvAddr->GetPort();
	u32 dwRet    = DSOK;
	dwRet = dsRemoveDump(dsId, htonl(dwRcvIP), wRcvPort);
	return dwRet;
}






