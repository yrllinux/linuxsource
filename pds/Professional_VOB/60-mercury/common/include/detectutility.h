/*****************************************************************************
模块名      : detectutitlity.h
文件名      : detectutitlity.h
相关文件    : 
文件实现功能: detectee的功能函数
作者        : chendaiwei
版本        : V1.0  Copyright(C) 2006-2008 KDC, All rights reserved.
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人		走读人      修改内容
2013/04/18  4.7.2       chendaiwei				创建
******************************************************************************/
#ifndef _DETECTUTILITY_H_
#define _DETECTUTILITY_H_

#include "mcuconst.h"
#include "kdvsys.h"

#define KDVIPDT_SECTION_DEV ( LPCSTR )"Dev"
#define KDVIPDT_KEY_ALIAS (LPCSTR)"Alias"
#define KDVIPDT_KEY_TYPEALIAS (LPCSTR)"TypeAlias"

#ifdef _LINUX_
	#ifdef _LINUX12_
		#include "brdwrapper.h"
		#include "brdwrapperdef.h"
		#include "nipwrapper.h"
		#include "nipwrapperdef.h"
	#else
		#include "boardwrapper.h"
	#endif
/*=============================================================================
函 数 名： TransPid2BrdType
功    能： pid转换成brdtype
算法实现： 
全局变量： 
参    数： void
返 回 值： void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2010/06/01   4.0		周嘉麟                  创建
=============================================================================*/
u8	TransPid2BrdType(u32 dwPID)
{
	u8 byBrdID = BRD_TYPE_UNKNOW;
	switch (dwPID)
	{
	case BRD_PID_KDM200_HDU:
		byBrdID = BRD_TYPE_HDU;
		break;
	case BRD_PID_KDM200_HDU_L:
		byBrdID = BRD_TYPE_HDU_L;
		break;
	case BRD_PID_KDV_EAPU:
		byBrdID = BRD_TYPE_EAPU;
		break;		
	case BRD_PID_KDV_MPU:
	case 0x043b:
		byBrdID = BRD_TYPE_MPU;
		break;
	case BRD_PID_KDV_MAU:
		break;
	case BRD_PID_KDV8000A_MPC2:
		byBrdID = BRD_TYPE_MPC2;
		break;
	case BRD_PID_KDV8000A_CRI2:
		byBrdID = BRD_TYPE_CRI2;
		break;
	case BRD_PID_KDV8000A_DRI2:
		byBrdID = BRD_TYPE_DRI2;
		break;
	case BRD_PID_KDV8000A_IS21:
		byBrdID = BRD_TYPE_IS21;
		break;
	case BRD_PID_KDV8000A_IS22:
		byBrdID = BRD_TYPE_IS22;
		break;
	case BRD_PID_KDV8000A_HDU2:
		byBrdID = BRD_TYPE_HDU2;
		break;
	case BRD_PID_KDV8000A_HDU2_L:
		byBrdID = BRD_TYPE_HDU2_L;
		break;
	case BRD_PID_KDV8000A_HDU2_S:
		byBrdID = BRD_TYPE_HDU2_S;
		break;
	case BRD_PID_KDV8000A_MPU2:
		{
#if defined (_LINUX12_) && defined(_ARM_DM81_)		
			s32 swResult = BrdMpu2HasEcard();

			if(swResult == 1)
			{
				byBrdID = BRD_TYPE_MPU2ECARD;
			}
			else if (swResult == 0)
			{
				byBrdID = BRD_TYPE_MPU2;
			}
			else if (swResult == -1)
			{
				byBrdID = BRD_TYPE_UNKNOW;
				printf("Failed to invoke BrdMpu2HasEcard!\n");
			}
#endif
		}
		break;
	case BRD_PID_KDV8000A_APU2:
		byBrdID = BRD_TYPE_APU2;
		break;
	default:
		break;
	}
	return byBrdID;
}

void GetBrdTypeStrByBrdType(u8 byBrdType, s8* pachBrdTypeStr)
{
	if( pachBrdTypeStr == NULL )
	{
		return;
	}

	switch ( byBrdType)
	{
	case BRD_TYPE_MPC2:
		strcpy(pachBrdTypeStr,BRD_ALIAS_MPC2);
		break;
	case BRD_TYPE_CRI2:
		strcpy(pachBrdTypeStr,BRD_ALIAS_CRI2);
		break;
	case BRD_TYPE_DRI2:
		strcpy(pachBrdTypeStr,BRD_ALIAS_DRI2);
		break;
	case BRD_TYPE_MPC:
		strcpy(pachBrdTypeStr,BRD_ALIAS_MPC);
		break;
	case BRD_TYPE_APU:
		strcpy(pachBrdTypeStr,BRD_ALIAS_APU);
		break;
	case BRD_TYPE_DRI:
		strcpy(pachBrdTypeStr,BRD_ALIAS_DRI);
		break;
	case BRD_TYPE_MPU:
		strcpy(pachBrdTypeStr,BRD_ALIAS_MPU);
		break;
	case BRD_TYPE_EAPU:
		strcpy(pachBrdTypeStr,BRD_ALIAS_EAPU);
		break;
	case BRD_TYPE_CRI:
		strcpy(pachBrdTypeStr,BRD_ALIAS_CRI);
		break;
	case BRD_TYPE_IS22:
		strcpy(pachBrdTypeStr,BRD_ALIAS_IS22);
		break;
	case BRD_TYPE_MPU2:
		strcpy(pachBrdTypeStr,BRD_ALIAS_MPU2);
		break;
	case BRD_TYPE_MPU2ECARD:
		strcpy(pachBrdTypeStr,BRD_ALIAS_MPU2_ECARD);
		break;
	case BRD_TYPE_HDU2:
		strcpy(pachBrdTypeStr,BRD_ALIAS_HDU2);
		break;
	case BRD_TYPE_HDU2_L:
		strcpy(pachBrdTypeStr,BRD_ALIAS_HDU2_L);
		break;
	case BRD_TYPE_HDU2_S:
		strcpy(pachBrdTypeStr,BRD_ALIAS_HDU2_S);
		break;
	case BRD_TYPE_APU2:
		strcpy(pachBrdTypeStr,BRD_ALIAS_APU2);
		break;
	default:
		break;
	}
	
	return;
}

//记录当前设备信息，包括层槽号，类型
BOOL32 DetecteeGetDeviceInfo( char * pszDeviceName = NULL )
{
	s8 achDeviceName[64] = {0};
	s8 achDeviceType[64] ={0};
#if defined(_8KE_)
	strcpy(achDeviceName,"8000G");
	strcpy(achDeviceType,"8000G");
#elif defined(_8KH_)
	if( pszDeviceName!= NULL)
	{
		strcpy(achDeviceName,pszDeviceName);
		strcpy(achDeviceType,pszDeviceName);
	}
	else
	{
		strcpy(achDeviceName,"8000H");
		strcpy(achDeviceType,"8000H");
	}
#else
	TBrdPosition tBrdPosition; 
	STATUS nRet = BrdQueryPosition( &tBrdPosition );
	if(ERROR == nRet)
	{
		printf("[detectutility]BrdQueryPosition error!\n");
		
		return FALSE;
	}

	printf("[detectutility] brdId.%d slay.%d slot.%d\n",tBrdPosition.byBrdID,tBrdPosition.byBrdLayer,tBrdPosition.byBrdSlot);
	
#ifdef _LINUX12_
	
	TBrdE2PromInfo tBrdE2PromInfo;
	memset( &tBrdE2PromInfo, 0x0, sizeof(tBrdE2PromInfo) );
	nRet = BrdGetE2PromInfo(&tBrdE2PromInfo); 
	if (ERROR == nRet)
	{
		printf("[detectutility] BrdGetE2PromInfo error!\n");
		
		return FALSE;
	}
	
	tBrdPosition.byBrdID  = TransPid2BrdType(tBrdE2PromInfo.dwProductId);
#endif
	
	GetBrdTypeStrByBrdType(tBrdPosition.byBrdID,achDeviceName);
	
	u8 byBrdTypeStrLen = strlen(achDeviceName);
	memcpy(&achDeviceType[0],achDeviceName,byBrdTypeStrLen);	
	sprintf(&achDeviceName[byBrdTypeStrLen],"(%d层%d槽)",tBrdPosition.byBrdLayer,tBrdPosition.byBrdSlot);
	printf("[detectutility] device name:%s\n",achDeviceName);

#endif

	s8 achConfigFilePath[64]={0};
	sprintf(achConfigFilePath,"%s/%s",DIR_CONFIG,"kdvipdt.ini");
 
	FILE *hFile = fopen(achConfigFilePath,"a+");
	if( hFile == NULL)
	{
	 	printf("[detectutility]open or create kdvipdt.ini failed!\n");

		return FALSE;
	}

	if(!SetRegKeyString(achConfigFilePath,KDVIPDT_SECTION_DEV,KDVIPDT_KEY_TYPEALIAS,achDeviceType))
	{
		printf("[detectutility]set type alis failed!\n");
		
		return FALSE;
	}

	if(!SetRegKeyString(achConfigFilePath,KDVIPDT_SECTION_DEV,KDVIPDT_KEY_ALIAS,achDeviceName))
	{
		printf("[detectutility]set device alis failed!\n");
		
		return FALSE;
	}

	return TRUE;
}

#endif //linux

#endif //_DETECTUTILITY_H_
