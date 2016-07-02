#include "kdvipdt.h"
#include "osp.h"
#include "mcuconst.h"
#include "kdvsys.h"
#include "charsetutility.h"

// #ifdef _LINUX_
// 	#include <stdio.h>
// 	#include <stdlib.h>
// 	#include <unistd.h>
// 	#include <string.h>
// #endif

#ifdef _LINUX_
    #ifdef _LINUX12_
        #include "brdwrapper.h"
        #include "brdwrapperdef.h"
        #include "nipwrapper.h"
		#include "nipwrapperdef.h"
	#else
		#include "boardwrapper.h"
	#endif
#endif

#ifdef _LINUX_
#ifdef _X86_  // Redhat
#define DIR_CONFIG               ( LPCSTR )"/opt/mcu/conf" 
#endif
#if defined(_PPC_) || defined(_ARM_)
#define DIR_CONFIG               ( LPCSTR )"/usr/etc/config/conf"
#endif
#endif

#if !defined QUADADDR
#define QUADADDR(ip) ((u8 *)&(ip))[0], \
	((u8 *)&(ip))[1], \
	((u8 *)&(ip))[2], \
((u8 *)&(ip))[3]
#endif

#ifdef _LINUX_  //linux start


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
u8	TransPid2BrdType(u32 dwPID, s32 swHasSubCard)
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
		//byBrdID = BRD_TYPE_MPC;
	//  [1/21/2011 chendaiwei] 支持MPC2单板类型上报
		byBrdID = BRD_TYPE_MPC2;
		break;
	//  [1/21/2011 chendaiwei] 支持CRI2单板类型上报
	case BRD_PID_KDV8000A_CRI2:
		//byBrdID = BRD_TYPE_CRI;
		byBrdID = BRD_TYPE_CRI2;
		break;
	//  [1/21/2011 chendaiwei] 支持DRI2单板类型上报
	case BRD_PID_KDV8000A_DRI2:
		byBrdID = BRD_TYPE_DRI2;
		break;
	//[2011/01/25 zhushz]IS2.x单板支持
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
			//s32 swResult = BrdMpu2HasEcard();

			if(swHasSubCard == 1)
			{
				byBrdID = BRD_TYPE_MPU2ECARD;
			}
			else if (swHasSubCard == 0)
			{
				byBrdID = BRD_TYPE_MPU2;
			}
			else if (swHasSubCard == -1)
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

//获取默认网关
u32 callbackKDTGetRoute( )
{
	u32 dwGW= 0;

#ifdef _LINUX12_ //linux1.2新板卡
	TBrdAllInfo tBrdAllInfo;
	if(ERROR == BrdGetAllInfo(&tBrdAllInfo))
	{
		printf("[detectee]callbackKDTGetRoute BrdGetAllInfo error!\n");
		
		return FALSE;
	}

	for ( u8 byIdx = 0; byIdx < tBrdAllInfo.brdHwInfo.byEthNum; byIdx++)
	{
		printf("[detectee] Eth[%d] Default Gw %d.%d.%d.%d\n",QUADADDR(tBrdAllInfo.brdNetInfo[byIdx].brdDefGateway));
	}

	dwGW = tBrdAllInfo.brdNetInfo[0].brdDefGateway;
	printf("[callbackKDTGetRoute.linux1.2] BrdGetDefGateway success!\n");
#else

	s8 achConfigFilePath[64]={0};
	sprintf(achConfigFilePath,"%s/%s",DIR_CONFIG,"kdvipdt.ini");
	
	GetRegKeyInt(achConfigFilePath,kdvipdtSecDev,"GW",0,(s32*)&dwGW);

#endif
				
	printf("[detectee] Default Gw %d.%d.%d.%d\n",QUADADDR(dwGW));

	return dwGW;
}

//回调函数实现
BOOL32 CallBackSetAliasAndType( s8* pchDevAlias, s8* pchDevType )
{
	s8 achDeviceName[64] = {0};
	s8 achDeviceType[64] ={0};
	s8 achConfigFilePath[64]={0};

#ifdef _LINUX12_ //linux1.2新板卡
	
	TBrdAllInfo tBrdAllInfo;

	if(ERROR == BrdGetAllInfo(&tBrdAllInfo))
	{
		printf("[detectee]CallBackSetAliasAndType BrdGetAllInfo error!\n");
		
		return FALSE;
	}
	
	u32 dwProductId = tBrdAllInfo.brdE2PromInfo.dwProductId;

	u8 byBrdID = TransPid2BrdType(dwProductId,tBrdAllInfo.brdHwInfo.brdSubCardStat);
	if (UNKNOWN_BOARD != byBrdID)
	{
		printf("[CallBackSetAliasAndType] byBrdID change from %d to %d!\n", tBrdAllInfo.brdBrdPosition.byBrdID, byBrdID);
		tBrdAllInfo.brdBrdPosition.byBrdID = byBrdID;			
	}	
	
	if(    tBrdAllInfo.brdBrdPosition.byBrdID == BRD_TYPE_HDU2
		|| tBrdAllInfo.brdBrdPosition.byBrdID == BRD_TYPE_HDU2_L
		|| tBrdAllInfo.brdBrdPosition.byBrdID == BRD_TYPE_HDU2_S
		|| tBrdAllInfo.brdBrdPosition.byBrdID == BRD_TYPE_HDU
		|| tBrdAllInfo.brdBrdPosition.byBrdID == BRD_TYPE_HDU_L)
	{
		//层号取配置文件
		s8    achProfileName[64] = {0};
		memset((void*)achProfileName, 0x0, sizeof(achProfileName));
		sprintf(achProfileName, "%s/%s", DIR_CONFIG, FILE_BRDCFGDEBUG_INI);
		//Layer
		s32  sdwLayer = 0;
		if(!GetRegKeyInt( achProfileName, "BoardConfig", "Layer", 0, &sdwLayer ))
		{
			printf( "[CallBackSetAliasAndType] Wrong profile while reading [BoardConfig] [Layer]!\n");
			return FALSE;
		}
		else
		{
			tBrdAllInfo.brdBrdPosition.byBrdLayer = (u8)sdwLayer;
		}
	}

	switch ( tBrdAllInfo.brdBrdPosition.byBrdID)
	{
	case BRD_TYPE_MPC2:
		strcpy(achDeviceType,BRD_ALIAS_MPC2);
		break;
	case BRD_TYPE_CRI2:
		strcpy(achDeviceType,BRD_ALIAS_CRI2);
		break;
	case BRD_TYPE_DRI2:
		strcpy(achDeviceType,BRD_ALIAS_DRI2);
		break;
	case BRD_TYPE_MPC:
		strcpy(achDeviceType,BRD_ALIAS_MPC);
		break;
	case BRD_TYPE_APU:
		strcpy(achDeviceType,BRD_ALIAS_APU);
		break;
	case BRD_TYPE_DRI:
		strcpy(achDeviceType,BRD_ALIAS_DRI);
		break;
	case BRD_TYPE_MPU:
		strcpy(achDeviceType,BRD_ALIAS_MPU);
		break;
	case BRD_TYPE_EAPU:
		strcpy(achDeviceType,BRD_ALIAS_EAPU);
		break;
	case BRD_TYPE_CRI:
		strcpy(achDeviceType,BRD_ALIAS_CRI);
		break;
	case BRD_TYPE_IS22:
		strcpy(achDeviceType,BRD_ALIAS_IS22);
		break;
	case BRD_TYPE_MPU2:
		strcpy(achDeviceType,BRD_ALIAS_MPU2);
		break;
	case BRD_TYPE_MPU2ECARD:
		strcpy(achDeviceType,BRD_ALIAS_MPU2_ECARD);
		break;
	case BRD_TYPE_HDU2:
		strcpy(achDeviceType,BRD_ALIAS_HDU2);
		break;
	case BRD_TYPE_HDU2_L:
		strcpy(achDeviceType,BRD_ALIAS_HDU2_L);
		break;
	case BRD_TYPE_HDU2_S:
		strcpy(achDeviceType,BRD_ALIAS_HDU2_S);
		break;
	case BRD_TYPE_APU2:
		strcpy(achDeviceType,BRD_ALIAS_APU2);
		break;
	case BRD_TYPE_HDU:
		strcpy(achDeviceType,BRD_ALIAS_HDU);
		break;
	case BRD_TYPE_HDU_L:
		strcpy(achDeviceType,BRD_ALIAS_HDU_L);
		break;
	default:
		break;
	}
	
	u8 byBrdTypeStrLen = strlen(achDeviceType);
	memcpy(&achDeviceName[0],achDeviceType,byBrdTypeStrLen);	
	sprintf(&achDeviceName[byBrdTypeStrLen],"(%d层%d槽)",tBrdAllInfo.brdBrdPosition.byBrdLayer,tBrdAllInfo.brdBrdPosition.byBrdSlot);
	
#else //linux1.1老板卡及X86系列MCU

//方案一：
	sprintf(achConfigFilePath,"%s/%s",DIR_CONFIG,"kdvipdt.ini");
	if(!GetRegKeyString(achConfigFilePath,kdvipdtSecDev,kdvipdtKeyAliase,NULL,achDeviceName,64))
	{
		printf("[detectee]CallBackSetAliasAndType error!\n");
		
		return FALSE;
	}
	else
	{
		printf("[detectee] DeviceName:%s\n",achDeviceName);
	}
	
	if(!GetRegKeyString(achConfigFilePath,kdvipdtSecDev,"TypeAlias",NULL,achDeviceType,64))
	{
		printf("[]CallBackSetAliasAndType error!\n");
		
		return FALSE;
	}
	else
	{
		printf("[detectee] DeviceType:%s\n",achDeviceType);
	}
	
// 	u32 dwBrdIp = 0;
// 
// 	if(GetRegKeyInt(achConfigFilePath,kdvipdtSecDev,"IP",0,(s32*)&dwBrdIp))
// 	{
// 		if(dwBrdIp != 0)
// 		{
// 			if(!AddPermitIP( dwBrdIp))
// 			{
// 				printf("[detectee] addPermitIP failed 0x%x\n",dwBrdIp);
// 			}
// 			else
// 			{
// 				u8 *p = (u8*)&dwBrdIp;
// 				
// 				printf("[detectee] addPermitIP success %d.%d.%d.%d\n",p[0],p[1],p[2],p[3]);
// 			}
// 		}
// 	}
#endif

	printf("DeviceType:%s,DeviceName:%s\n",achDeviceType,achDeviceName);

	char *pChType = NULL;
	char *pChName = NULL;
	char **pUtf8DeviceName = &pChType;
	char **pUtf8DeviceType = &pChName;
	u32 wUtf8NameLen =  gb2312_to_utf8( achDeviceName, pUtf8DeviceName );
	u32 wUtf8typeLen =  gb2312_to_utf8( achDeviceType, pUtf8DeviceType );

	if(*pUtf8DeviceName != NULL && wUtf8NameLen!= 0)
	{
		memcpy(pchDevAlias,*pUtf8DeviceName,strlen(*pUtf8DeviceName));

		free(*pUtf8DeviceName);
	}

	if( *pUtf8DeviceType != NULL && wUtf8typeLen!= 0)
	{
		memcpy(pchDevType,*pUtf8DeviceType,strlen(*pUtf8DeviceType));
		free(*pUtf8DeviceType);
	}

	return TRUE;
}


void userinit()
{
	if(!KDTRegSetAliasAndTypeCallback(CallBackSetAliasAndType))
	{
		printf("[detectee]KDTRegSetAliasAndTypeCallback failed\n");
	}

	if(!KDTRegGetRouteCallback(callbackKDTGetRoute))
	{
		printf("[detectee]KDTRegGetRouteCallback failed\n");
	}

	if(KDTStart(0))
	{
		printf("[detectee] KDTStart success!\n");
	}
	else
	{
		printf("[detectee] KDTStart failed!\n");
	}

	return;
}

int main( int argc, char** argv )
{
    if (argc > 0)
    {
		printf("[detectee]Starting: %s\n", argv[0]);
    }

    printf( "[detectee] userinit will begain! \n" );

	userinit();

	printf( "[detectee] userinit over! \n" );

    while (TRUE)
    {
        sleep(1000);
    }	

    printf("detectee quit normally!\n");
    return 0;
}
#endif //linux end

#if defined( WIN32 )
void main(void)
{
// 	while (true)
//     {
//         Sleep(INFINITE);
//     }
}
#endif
