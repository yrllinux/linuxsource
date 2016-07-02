/*========================================
模块名	： MEDAPI
文件名	： MEDAPI.h
相关文件：  无
文件实现功能：编解码器底层API函数接口。
作者	：凌宏强
版本	：V1.0
日期    ：2004.03.23
===========================================*/
#ifndef __INCMEDAPIh
#define __INCMEDAPIh

#ifdef __cplusplus 
   #define  MEDAPI  extern "C"
#else if
   #define  MEDAPI  extern
#endif

/* 以太网参数结构 */
typedef struct{
    UINT32 dwIpAdrs;
    UINT32 dwIpMask;
    UINT8  byMacAdrs[6];
}TBrdEthParam;
/*wjh:由于增加了提供给了codece 层的函数需要增加的结构体/宏等的定义*/
/*wjh add */
#define BSP15_VGA_NONE  0    /*vga输入输出功能均没有*/
#define BSP15_VGA_IN    1    /*只有vga输入功能*/
#define BSP15_VGA_OUT   2    /*只有vga输出功能 */
#define BSP15_VGA_INOUT 3    /*vga输入输出功能都有*/
#define SAA7114_OUTPORT_NONE      0/*没有7114*/
#define SAA7114_OUTPORT_X         1/*从7114的X口输出*/
#define SAA7114_OUTPORT_I         2/*从7114d的I口输出*/
#define SAA7114_OUTPORT_XI        3/*从7114的X口和I口同时输出*/
#define CAPTURE_FROM_NONE            0/*没有采集芯片*/
#define CAPTURE_FROM_SAA7114         1/*从7114芯片采集信号*/
#define CAPTURE_FROM_SAA7113         2/*从7113采集信号*/
#define BSP15_VCAPTURE_FROM_NONE   0/*没有BSp15数据采集*/
#define BSP15_VCAPTURE_FROM_PORTA  1/*从BSP15的A口采集数据*/
#define BSP15_VCAPTURE_FROM_PORTB  2/*从BSP15的B口采集数据*/
#define BSP15_VCAPTURE_FROM_PORTAB  3/*从BSP15的A/B口采集数据*/

/* BSP15声音采集芯片BrdGetAudCapChipType返回值宏定义 */
#define CAPTURE_AUDIO_FROM_NONE    0       /* audio capture from chip none  */
#define CAPTURE_AUDIO_FROM_5331    1       /* audio capture from chip 5331  */
#define CAPTURE_AUDIO_FROM_AIC23B  2       /*                        AIC23B */

/* 系统复位回调注册函数部分 */
#define SYS_MAX_REBOOT_HOOK_NUM     10
typedef struct
{
    UINT32 dwFuncLogInNum;
    VOIDFUNCPTR pfRebootHookFuc[SYS_MAX_REBOOT_HOOK_NUM];
}TBrdRebootHookModule;
LOCAL UINT32 gdwMapFlashEnable; 
/* flash分区信息结构定义 */
#define DISK_MAX_NUMBER     8
typedef struct{
    UINT32  dwDiskBaseAddr;       /* flash盘的基地址 */
    UINT32  dwDiskSize;           /* flash盘的容量，以Byte为单位 */
    char diskNameStr[32];         /* flash盘的名称 */
}TBrdDiskInfoDesc;

typedef struct{
    UINT8  byDiskNum;           /* 实际flash盘的个数 */
    TBrdDiskInfoDesc tBrdDiskInfo[DISK_MAX_NUMBER];
}TBrdAllDiskInfo;

/*提供给codec层的函数声明*/
UINT8 BrdFastLoadEqtFileIsEn(void);
STATUS sysRebootHookAdd(VOIDFUNCPTR rebootHook);
STATUS BrdGetAllDiskInfo(TBrdAllDiskInfo *pBrdAllDiskInfo);
UINT32 BrdGetFullFileName(UINT8 byPutDiskId, char *pInFileName, char *pRtnFileName);
UINT8 BrdGetBSP15VGAConf(UINT8 byDevId);
void BrdStopVGACap(void);
void BrdOpenVGA(void);
UINT8 BrdGetSAA7114OutPort(UINT8 byBSP15ID);
void BrdSetVGACapMode(UINT8 byMode);
UINT8 BrdGetCaptureChipType(UINT8 byBSP15ID);
void BrdMapDevOpenPreInit(UINT8 byBSP15ID);
void BrdCloseVGA(void);
STATUS BrdAddEthBautRate(UINT8 byUnit, UINT32 dwBautRate);
BOOL  BrdEthSndUseTimer(void);
UINT8 BrdBsp15LedFlashMode(void);
UINT8 BrdGetBSP15Speed(UINT8 byDevId);
void BrdStartVGACap(UINT8 byMode);
void BrdMapDevClosePreInit(UINT8 byBSP15ID);
STATUS BrdMoveEthBautRate(UINT8 byUnit, UINT32 dwBautRate);
UINT8 BrdGetBSP15CapturePort(UINT8 byBSP15ID);


/*end wjh add*/
//初始化编解码器底层API函数接口
MEDAPI STATUS MEDAPI_Init();
//获取硬件版本
MEDAPI UINT8 MEDAPI_GetHardVer();
//获取EPLD版本
MEDAPI UINT8 MEDAPI_GetEpldVer();
//获取设备号
MEDAPI UINT8 MEDAPI_GetDeviceID();
//获取OS版本
MEDAPI UINT8 MEDAPI_GetOsVer();
//初始化串口
MEDAPI void MEDAPI_InitSerial(UINT8);
//串口端口号自动适配
MEDAPI UINT8 MEDAPI_SerialPortAdapt(UINT8);
//设置串口的波特率
MEDAPI void MEDAPI_SetSerialBaud(UINT8,UINT16);
//获取现场四路告警(锁存)
MEDAPI void MEDAPI_GetAlarmInput(UINT8*);
//输出现场报警
MEDAPI void MEDAPI_SetAlarmOutput(UINT8,UINT8);
//系统重启
MEDAPI void MEDAPI_SysReboot();
//设置系统守卫
MEDAPI void MEDAPI_SetSysGuard(UINT8);
//获取系统守卫
MEDAPI UINT8 MEDAPI_GetSysGuard();
//设置系统建链指示灯
MEDAPI void MEDAPI_SetSysLink(UINT8);
//设置系统告警指示灯
MEDAPI void MEDAPI_SetSysAlarm(UINT8);
//获取系统E1状态
MEDAPI UINT8 MEDAPI_GetE1State(void);
//获取风机运转状态电平值
MEDAPI UINT8 MEDAPI_GetMotorState(void);
//获得视频源输入状态
MEDAPI void MEDAPI_GetVidstate(UINT8* Vidstate);
//e1模块设置环回
MEDAPI void MEDAPI_e1Loop(UINT8 byType,UINT8 byState, UINT16 byMin);
//e1模块撤销环回
MEDAPI void MEDAPI_e1noLoop();
//压缩文件解压缩处理flash－>sdram
MEDAPI int  MEDAPI_Decompfile( char* compfile, char* uncompfile, UINT8 byMemNo );
//释放压缩文件申请的内存
MEDAPI void MEDAPI_DecompMemFree();
//设置NIP的running.cfg的命令函数
MEDAPI void MEDAPI_SetNIPConf(UINT8,char*,char*);
//设置以太网参数
MEDAPI STATUS MEDAPI_SetEthParam(UINT8 byEthId, UINT8 byIpOrMac, TBrdEthParam *ptBrdEthParam);
//获取以太网参数
MEDAPI STATUS MEDAPI_GetEthParam(UINT8 byEthId, TBrdEthParam *ptBrdEthParam);

#endif

