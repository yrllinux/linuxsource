
#ifndef INCApuDrvLibh
#define INCApuDrvLibh


#ifdef __cplusplus 
extern "C" { 
#endif /* __cplusplus */

#include "time.h"
#include "timers.h"

#define VER_APUVxNIP      ( const char * )"APUVxNIP 30.01.07.10.041016"

/*================================时间模块设置====================================*/
/* 时间结构说明 */
/*采用系统标准tm结构，可参考tornado相关帮助文档
int tm_sec;  seconds after the minute  - [0, 59] 
int tm_min;  minutes after the hour    - [0, 59] 
int tm_hour;  hours after midnight     - [0, 23] 
int tm_mday;  day of the month         - [1, 31] 
int tm_mon;  months since January      - [1-1, 12-1]    注意: 0 = 一月，参数需规范后传入
int tm_year;  years since 1900         - [1980-1900,2079-1900]  注意:参数需规范后传入
int tm_wday;  days since Sunday        - [0, 6]   不支持，输入0
int tm_yday;  days since January 1     - [0, 365] 不支持，输入0 
int tm_isdst;  Daylight Saving Time flag [ 0 ]    不支持，输入0

/* 时间函数声明 */

STATUS BrdTimeGet( struct tm* pGettm );        /* 读取RTC时间 */
STATUS BrdTimeSet( struct tm* pSettm );        /* 设置RTC和系统时间 */ 


/*==============================底层告警模块部分====================================*/
/* 底层告警信息结构定义 */
typedef struct{
    BOOL bAlarmDSP1FanStop;     /* 图像编码风扇停转 */
    BOOL bAlarmDSP2FanStop;     /* 图像解码风扇停转 */
    BOOL bAlarmDSP3FanStop;     /* 音频编解码风扇停转 */
}TBrdIMTAlarmAll;

/* 模块函数声明 */


/*==============================风扇转速检测模块====================================*/
/* 风扇ID宏定义 */
#define IMT_DSP1FAN       0         /* 图像编码风扇 */
#define IMT_DSP2FAN       1         /* 图像解码风扇 */
#define IMT_DSP3FAN       2         /* 音频编解码风扇 */

/* 模块函数声明 */


/* 模块函数声明 */
void BrdVideoMatrixSet
    (
    UINT8 vedioInSelect,    /* 视频输入源选择：1~7 = 7个视频输入接口，1号输入硬件内部
                                              已接至解码后的图像，对外屏蔽。
                                              开关相应的输出请使用上面定义的宏 */
    UINT8 vedioOutSelect   /* 视频输出端口选择：1~7 = 7个视频输出接口，1号输出硬件内部
                                              已接至编码数据源，对外屏蔽。其余保留。*/
    );


/*==============================底层硬件版本信息=============================*/  
#define BOARD_VERSION         ((UINT8)0x00)
#define EPLD_VERSION          ((UINT8)0x01)

STATUS BrdVersionGet(UINT8 module, UINT8 *pVersion);    


/*==============================视频源检测=============================*/  
/* 宏定义 */
#define VIDEOIN_CHKERROR   -1   /* 视频编码芯片MAP[1]没有创建并正常运行 */
#define VIDEOIN_OFF         0   /* 被选进编码的视频源无信号 */
#define VIDEOIN_ON          1   /* 被选进编码的视频源有信号 */

int BrdVideoInCheck(void);        /* 视频源检测,返回值如上宏定义 */

/* 指示灯状态数据结构 */
typedef struct{
    UINT8 byLedRun; 
    UINT8 byLedDSP1;
    UINT8 byLedDSP2;
    UINT8 byLedDSP3;
    UINT8 byLedSYSALM;
    UINT8 byLedMLINK;
    UINT8 byLedLNK0;
    UINT8 byLedLNK1;
}TBrdIMTLedStateDesc;

STATUS BrdIMTQueryLedState(TBrdIMTLedStateDesc *ptBrdIMTLedState);

#ifdef __cplusplus 
} 
#endif /* __cplusplus */

#endif   /*INCDriverInith*/
