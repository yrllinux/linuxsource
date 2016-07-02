#ifndef OSAPI_V3_H
#define OSAPI_V3_H

#include "kdvtype.h"

enum
{
    TTY_232 = 0,
    TTY_422,
    TTY_485RX,
    TTY_485TX,
};

enum
{
    ALARM_ON,
    ALARM_OFF
};

enum {
     DSPRUN_WINK_1X = 0
    ,DSPRUN_WINK_2X
    ,DSPRUN_WINK_4X
    ,DSPRUN_WINK_MAXNUM
};

typedef struct
{
    u32 dwIpAdrs; //网络序
    u32 dwIpMask; //网络序
    u8  byMacAdrs[6];
}TBrdEthParam;

#if 0
typedef struct
{
    s32 nTotal;   //内存总量（单位KB）
    s32 nFree;    //空闲内存（单位KB）
    s32 nShared;  //共享内存（单位KB）
}TMemStatus;

typedef struct
{
    s32 nTotalPercent; //总的 CPU 占有率（百分比：0～100）
}TCpuStatus;
#endif

typedef struct
{
    s32 nEthLinked; //以太网接口的状态：有有效的以太网链接（1），没有有效的以太网链接（0）
}TEthStatus;

typedef struct
{
    s32 nRunLighten; //运行灯的状态：亮（1），灭（0）
}TLedStatus;

#ifdef __cplusplus
extern "C" {
#endif
    
    BOOL32 MEDAPI_GpioOpen();
    BOOL32 MEDAPI_GpioClose();
    
    u8     MEDAPI_GetEpldVer();
    void   MEDAPI_GetOsVer(s8 *pchVer, u32 dwBufLen, u32 *pdwVerLen);
    u8     MEDAPI_GetDeviceID();
    BOOL32 MEDAPI_SysReboot();
    
    BOOL32 MEDAPI_OpenSysGuard(u32 dwNoticeTimeout);
    BOOL32 MEDAPI_CloseSysGuard();
    BOOL32 MEDAPI_NoticeSysGuard();
    BOOL32 MEDAPI_GetSysGuard();
    
    s32    MEDAPI_OpenSerial(u8 byPort);
    BOOL32 MEDAPI_CloseSerial(s32 hCom);
    BOOL32 MEDAPI_SetSerialBaud(s32 hCom, u16 wBaud);
    BOOL32 MEDAPI_ReadSerial(s32 hCom, s8 *pbyBuf, u32 dwBytesToRead, u32 *pdwRByteLen);
    BOOL32 MEDAPI_WriteSerial(s32 hCom, s8 *pbyBuf, u32 dwBytesToWrite, u32 *pdwWByteLen);
    
    BOOL32 MEDAPI_GetAlarmInput(u8 byPort, u8* pbyState);
    BOOL32 MEDAPI_SetAlarmOutput(u8 byPort, u8 byState);
    
    BOOL32 MEDAPI_SetEthParam(u8 byEthId, TBrdEthParam *ptSysEthParam);
    BOOL32 MEDAPI_GetEthParam(u8 byEthId, TBrdEthParam *ptSysEthParam);
    BOOL32 MEDAPI_SetDefaultGateway(u32 dwGateway);
    BOOL32 MEDAPI_GetDefaultGateway(u32* pdwGateway);
    
    BOOL32 MEDAPI_SaveConfig();
    
    BOOL32 MEDAPI_SetTime(struct tm *ptSystime);
    BOOL32 MEDAPI_GetTime(struct tm *ptSystime);
    
//    BOOL32 MEDAPI_GetCpuStatus(TCpuStatus *ptStatus);
//    BOOL32 MEDAPI_GetMemStatus(TMemStatus *ptStatus);
    BOOL32 MEDAPI_GetNetStatus(TEthStatus *ptStatus);
    BOOL32 MEDAPI_GetLedStatus(TLedStatus *ptStatus);

    BOOL32 MEDAPI_Upgrade(u8 *abyBuf, s32 nLen);
    
    /*dwWink: 0:1X 1:2X 2:4X*/
    BOOL32 MEDAPI_SetLedDsprunWink(u32 dwWink);

#ifdef __cplusplus
}
#endif

#endif /*OSAPI_V3_H*/
