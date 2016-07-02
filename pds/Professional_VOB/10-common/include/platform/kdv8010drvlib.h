
#ifndef INCDriverInith
#define INCDriverInith


#ifdef __cplusplus 
extern "C" { 
#endif /* __cplusplus */

#include "time.h"
#include "timers.h"


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

/*==================================以太网模块设置===============================*/
/* 网络设备参数netEquipUnit宏定义 */
#define NETEQUIP_IS_ETH1    1    /* 以太网1： 对应8265的FCC2 或 x86上的启动网卡 */
#define NETEQUIP_IS_ETH2    2    /* 以太网2： 对应8265的FCC1 或 x86上的网关网卡*/
#define NETEQUIP_IS_ETH3    3    /* 以太网3： 对应8265的SCC1 */
#define NETEQUIP_IS_IPOH    4    /* IPOH  ：  对应8265的MCC1 */


/*设置网卡的IP地址、MAC地址和子网掩码*/
STATUS ethernetSet
    (
    int netEquipUnit,	 /* 见上 */
    char *pIpAddrStr,    /* ip地址 例:"100.1.1.1" */
    int   netmask  ,     /* 子网掩码 ：例:0xff000000 ;0 = 自动识别 */
    char *pEtherMacAddr  /* mac地址：null -> 不改变当前值 
                         例：unsigned char sysFccEnetAddr [6] = {0x08, 0x00, 0x33, 0x02, 0x02, 0x51};*/
    );

/*#ifdef BOARD_KDV8010A*/

/*==============================外围插卡模块部分====================================*/
/* ----------------------------1、 E1、2E1、4E1部分--------------------------------
/* 模块宏定义 */
#define KDV8010A_MODULE_E1     0        /* 外挂模块为: 单E1模块   */
#define KDV8010A_MODULE_4E1    1        /* 外挂模块为: 4E1模块    */
#define KDV8010A_MODULE_V35DTE 2        /* 外挂模块为: V35DTE模块 */
#define KDV8010A_MODULE_V35DCE 3        /* 外挂模块为: V35DCE模块 */
#define KDV8010A_MODULE_BRIDGE 4        /* 外挂模块为: 网桥模块   */
#define KDV8010A_MODULE_2E1    5        /* 外挂模块为: 2E1模块    */
#define KDV8010A_NO_MODULE     0xf     /* 没有外挂模块  */
/* 模块函数声明 */
UINT8 BrdExtModuleIdentify(void);   /* 外挂模块识别函数 */

/* 阻抗宏定义 */
#define KDV8010A_E1_75   0              /* E1模块阻抗为75  ohm */
#define KDV8010A_E1_120  0x40           /* E1模块阻抗为120 ohm */
/* 模块函数声明 */
UINT8  BrdE1ResGet(UINT8 no);           /* E1模块阻抗识别函数，no = 0~3,视当前使用的模块号而定 
                                           返回值如上宏定义 */


/* e1通道带宽设置,必需在设置ipoh的ip地址前完成，否则无效，建议从配置文件读取；
 * 目前暂不支持动态设置，要改变带宽必须重启系统使用该函数设置 。
*/
STATUS BrdE1ChanTabSet
    (
    UINT8 e1No,         /* e1设备号: 0~3,对于单e1模块只需对e1[0]设置 */
    UINT32 e1SlotMask   /* 时隙分配，由Ts31 Ts30...Ts0构成32位掩码，TSx为1时表示使用该时隙，为0时不使用
                           Ts0作为同步信号，用户不可使用，Ts16可作为数据或信令，本系统要求Ts16走信令，
                           用户不能使用，用户必需填0，Ts1必需使用，建议从低到高连续分配时隙，不要间隔，否则可能
                           引起通信系统不稳定。例: 使用5个时隙时e1SlotMask = 0x0000003e,注意:Ts0=Ts16=0;Ts1=1
                           此时带宽= 64x5 = 320kbit */
    );                                  

/* 设置ipoh路由,设置ipoh的ip 地址后必须设置，否则无法通信 */
STATUS BrdIpohRouteSet
    (
    char *pDevName,     /* always use "ipoh" */
    int unitNo,         /* always use 0 */
    char *pIpDstAddr,   /* 对端ipoh目的ip地址,例:"10.1.1.1"，建议从配置文件读取 */
    UINT8 mccChanId     /* 单e1模块使用1，其余保留 */
    );

/*==============================底层告警模块部分====================================*/
/* 底层E1告警信息结构定义 */
typedef struct{
    BOOL bAlarmE1RUA1;     /* E1全1告警 */
    BOOL bAlarmE1RLOS;     /* E1失同步告警 */
    BOOL bAlarmE1RRA;      /* E1远端告警 */
    BOOL bAlarmE1RCL;      /* E1失载波告警 */
}TBrdE1AlarmDesc;

/* 底层V35告警信息结构定义 */
typedef struct{
    BOOL bAlarmV35CDDown;     /* Carrier Detect, 载波检测失败 */
    BOOL bAlarmV35CTSDown;    /* Clear To Send, 清除发送失败 */
    BOOL bAlarmV35RTSDown;    /* Request To Send, 请求发送失败 */
    BOOL bAlarmV35DTRDown;    /* 数据终端未就绪 */
    BOOL bAlarmV35DSRDown;    /* 数据未准备好 */
}TBrdV35AlarmDesc;


/* 底层告警信息结构定义 */
typedef struct{
    BOOL bAlarmEth1LnkDown;     /* 以太网1无连接 */
    BOOL bAlarmEth2LnkDown;     /* 以太网2无连接 */
    BOOL bAlarmEth3LnkDown;     /* 以太网3无连接 */

    BOOL bAlarmDSP1FanStop;     /* 图像编码风扇停转 */
    BOOL bAlarmDSP2FanStop;     /* 图像解码风扇停转 */
    BOOL bAlarmDSP3FanStop;     /* 音频编解码风扇停转 */
    BOOL bAlarmSYS1FanStop;     /* 机箱风扇1停转 */
    BOOL bAlarmSYS2FanStop;     /* 机箱风扇2停转 */

    TBrdE1AlarmDesc tBrdE1AlarmDesc[4];     /* E1告警 */

    TBrdV35AlarmDesc tBrdV35AlarmDesc;    /* V.35告警 */
}TBrdMTAlarmAll;

/* 模块函数声明 */
STATUS BrdMTAlarmStateScan(TBrdMTAlarmAll *ptBrdMTAlarmAll);  /* 底层告警信息扫描 */


/*==============================闪灯模块部分====================================*/
/* 灯指示宏定义 */
#define BRD_ALARM_LED    1  /* 系统告警灯，有告警时可点该灯 */
#define BRD_LINK_LED     9  /* 系统连接灯，和MCU建链后可点该灯 */

#define BRD_LED_ON       1  /* 亮 */
#define BRD_LED_OFF      2  /* 灭 */
#define BRD_LED_QUICK    3  /* 快闪 */
#define BRD_LED_SLOW     4  /* 慢闪 */

/* 模块函数声明 */
void BrdMTLedFlash(UINT8 byLedID, UINT8 byState);   /* 闪灯控制 */


/*==============================风扇转速检测模块====================================*/
/* 风扇ID宏定义 */
#define KDV8010A_DSP1FAN       0         /* 图像编码风扇 */
#define KDV8010A_DSP2FAN       1         /* 图像解码风扇 */
#define KDV8010A_DSP3FAN       2         /* 音频编解码风扇 */
#define KDV8010A_SYS1FAN       3         /* 机箱风扇1 */
#define KDV8010A_SYS2FAN       4         /* 机箱风扇2 */

/* 模块函数声明 */
UINT32 BrdFanSpeedGet(UINT8 fanID); /* 风扇转速检测 */


/*==============================待机模块设置====================================*/
/* 待机宏定义 */
#define KDV8010A_WAKEUP       0         /* 唤醒系统 */
#define KDV8010A_SLEEP        1         /* 休眠系统 */

/* 模块函数声明 */
STATUS BrdRunStatSet(UINT8 state);      /* state如上 */

/*==============================视频切换矩阵模块设置=============================*/
/* 宏定义 */
#define KDV8010_MATRIX_OUTPUT_DISABLE      ((UINT8)0xfb)        /* 禁止音视频矩阵某输出端口 */
#define KDV8010_MATRIX_OUTPUT_ENABLE       ((UINT8)0xfc)        /* 启用音视频矩阵某输出端口 */

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

/*#endif   /* INC BOARD_KDV8010A */





/*#ifdef BOARD_MPC
/*==============================网同步=============================*/  
/* 同步模式宏定义 */
#define SYNC_MODE_FREERUN         0   /* 自由振荡，对于主MCU应设置为该模式，所有下级线路时钟与该MCU同步 */
#define SYNC_MODE_TRACK_2M_SLAVE  4   /* 跟踪模式，作为下级MCU应设置为该模式，跟踪DT来的从2M时钟 */
#define SYNC_CLOCK_UNLOCK         0   /* 跟踪模式时时钟未锁定 */
#define SYNC_CLOCK_LOCK           1   /* 跟踪模式时时钟锁定成功 */

/* 函数定义 */
void BrdNetSyncModeSet(UINT8 mode);                 /* 网同步模式设置
                                                       输入参数: 见同步模式宏定义 */
void BrdNetSyncSrcSelect(UINT8 dtiNo, UINT8 E1No);  /* 网同步源选择
                                                       输入参数: dtiNo 范围0~14；E1No范围0~7 */
int BrdNetSyncLock(void);                           /* 检测网同步跟踪模式时是否锁定时钟源
                                                       输出参数：#define SYNC_CLOCK_UNLOCK 0    跟踪模式时时钟未锁定 
                                                                #define SYNC_CLOCK_LOCK   1    跟踪模式时时钟锁定成功 
                                                       注：只有当前MCU配成跟踪模式时才有效，自由振荡时
                                                       MCU不会跟踪任何时钟，调该函数是无效的，永远返回未锁定 */



/*#endif   /* INC BOARD_MPC */




#ifdef __cplusplus 
} 
#endif /* __cplusplus */

#endif   /*INCDriverInith*/
