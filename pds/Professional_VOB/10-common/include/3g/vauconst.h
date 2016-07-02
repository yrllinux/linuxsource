#ifndef _VAUCONST_H_
#define _VAUCONST_H_

#include "kdvdef.h"

#define VAU_CONFIG_FILENAME ((LPCSTR)("vau.ini"))
#define VAU_LISTEN_PORT 1726
//#define VAU_LISTEN_PORT 1727//zlg,add for test

#define AID_VAU_BGN (AID_RTSP_BGN)

enum EAID_VAU
{

    AID_VAU_RTSP= AID_VAU_BGN, // VAU与RTSP协议栈通讯

    AID_RTSP_VAU, // RTSP协议栈与VAU通讯

};

#define MAX_RTSP_SESSION_NUM 128

#define MAXLEN_REQ_URL 1024

#define VAU_MAX_FILE_NAME_LEN (256)

enum ENmcPri
{
	E_Nmc_Pri_None				= 0,
	E_Nmc_Pri_Log_Level			= 1,
	E_Nmc_Pri_All				= 0xFF,
};

enum EVauLogLevel
{
	E_Vau_Log_None = 0,
	E_Vau_Log_Error = 1,
	E_Vau_Log_Warning = 2,
	E_Vau_Log_Debug = 4,
	E_Vau_Log_All = 0xFF,
};

//输出媒体类型
enum EVauStreamType
{
	E_Vau_StreamType_Normal = 0,//0：Normal
	E_Vau_StreamType_CDMA1X = 1,//1：CDMA1x
	E_Vau_StreamType_EVDO = 2,  //2：EVDO	
	E_Vau_StreamType_RESERVED = 3,  //2：EVDO	
};
#endif // _VAUCONST_H_
