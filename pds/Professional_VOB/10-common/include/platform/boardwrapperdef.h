/******************************************************************************
模块名  ： BoardWrapper
文件名  ： BoardWrapperDef.h
相关文件：
文件实现功能：BoardWrapper模块对外提供的宏定义头文件
作者    ：王俊华
版本    ：1.0.0.0.0
---------------------------------------------------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
03/09/2006  1.0         王俊华      创建
******************************************************************************/
#ifndef _BOARDWRAPPERDEF_H
#define _BOARDWRAPPERDEF_H

#ifdef __cplusplus 
extern "C" { 
#endif /* __cplusplus */

/* 单板种类ID号宏定义 */  
#define DSL8000_BRD_MPC              0      /* MPC板 */
#define DSL8000_BRD_DTI              1      /* DTI板 */
#define DSL8000_BRD_DIC              2      /* DIC板 */
#define DSL8000_BRD_CRI              3      /* CRI板 */
#define DSL8000_BRD_VAS              4      /* VAS板 */
#define DSL8000_BRD_MMP              5      /* MMP板 */
#define DSL8000_BRD_DRI              6      /* DRI板 */
#define DSL8000_BRD_IMT              7      /* IMT板 */
#define DSL8000_BRD_APU              8      /* APU板 */
#define DSL8000_BRD_DSI              9      /* DSI板 */
#define DSL8000_BRD_VPU              10     /* VPU板 */
#define DSL8000_BRD_DEC5             11     /* DEC5板 */
#define DSL8000_BRD_VRI              12     /* VRI板 */
#define KDV8000B_MODULE              13     /* KDV8000B模块 */
#define KDV8000C_MODULE              19     /* KDV8000C模块 */
#define KDV8005_BOARD                14     /* KDV8005板 */
#define DSL8000_BRD_DRI16            15     /* DRI16板 */
#define DSL8000_BRD_MDSC             0x10     /* MDSC板 */
#define DSL8000_BRD_16E1             0x11     /* 16E1板 */
#define KDV8003_BOARD                0x12     /* KDV8003板 */
#define DSL8000_BRD_HDSC             0x14     /* HDSC板 */
#define KDV2400_BOARD                50     /* 2400终端 */
#define KDV2500_BOARD                51     /* 2500B终端 */
#define KDM2418_BOARD                52     /* 2418终端 */
#define KDM2518_BOARD                53     /* 2518终端 */
#define KDM2417_BOARD                54     /* 2417终端 */
#define KDM2428_BOARD               56     /* 2428终端 */

#define KDV8010_BOARD                100    /* KDV8010终端 */
#define KDV8018_BOARD                101    /* KDV8018终端 */
#define KDV8010A_BOARD               102    /* KDV8010A终端 */
#define KDV8010B_BOARD               103    /* KDV8010B终端 */
#define KDV8010C_BOARD               104    /* KDV8010C终端 */
#define KDV8010D_BOARD               105    /* KDV8010D终端 */
#define KDV8010E_BOARD               106    /* KDV8010E终端 */
#define KDV8000B_BOARD               107    /* KDV8000B终端 */
#define KDV8000BHD_BOARD              0x6E   /* KDV8000B-HD 高清配套产品 */
#define KDV8000C_BOARD               108    /* KDV8000C终端 */	
#define KDV8008_BOARD                109    /* KDV8008终端 */
#define KDV8010A_PLUS_BOARD         110    /* 第十一版KDV8010A终端 */
//#define KDV8010C1_BOARD             111    /* KDV8010C1终端 */    
#define KDV8010A_10_BOARD            254    /* 第十版KDV8010A终端 */  /*目前已废弃*/
#define UNKNOWN_BOARD                255   /*未知的板子类型*/


/*相关极值宏定义*/
#define IP_ROUTE_MAX_NUM       64 /* 最大支持的路由个数 */
#define KDV8005_V35__MAX_NUM   1 /* 最大支持的V35接口数 */
#define VRI_V35_MAX_NUM   	   6 /* 最大支持的V35接口数 */

/* flash分区信息宏定义 */
#define DISK_MAX_NUMBER             8      /* 最大支持的存储设备数量 */

#define BRD_LED_NUM_MAX     32      /* 最多灯的个数 */

#define BRD_BSP_NUM_MAX     5       /* 最多BSP的个数 */

/* BrdLedStatusSet函数中指示灯闪烁状态 */
#define BRD_LED_ON                  1      /* 亮 */
#define BRD_LED_OFF                 2      /* 灭 */
#define BRD_LED_QUICK               3      /* 快闪 */
#define BRD_LED_SLOW                4      /* 慢闪 */

/* BrdLedStatusSet函数中指示灯控制ID */
#define LED_E1_ID_BASE               0xd0
#define LED_E1_ID(ix)                (LED_E1_ID_BASE+ix)      /* 模块上0号E1告警灯*/
#define LED_E1_ID0                   LED_E1_ID(0)/*0号E1的告警灯*/
#define LED_E1_ID1                   LED_E1_ID(1)/*1号E1的告警灯*/
#define LED_E1_ID2                   LED_E1_ID(2)/*2号E1的告警灯*/
#define LED_E1_ID3                   LED_E1_ID(3)/*3号E1的告警灯*/
#define LED_E1_ID4                   LED_E1_ID(4)/*4号E1的告警灯*/
#define LED_E1_ID5                   LED_E1_ID(5)/*5号E1的告警灯*/
#define LED_E1_ID6                   LED_E1_ID(6)/*6号E1的告警灯*/
#define LED_E1_ID7                   LED_E1_ID(7)/*7号E1的告警灯*/
#define LED_E1_ID8                   LED_E1_ID(8)/*8号E1的告警灯*/
#define LED_E1_ID9                   LED_E1_ID(9)/*9号E1的告警灯*/
#define LED_E1_ID10                  LED_E1_ID(10)/*10号E1的告警灯*/
#define LED_E1_ID11                  LED_E1_ID(11)/*11号E1的告警灯*/
#define LED_E1_ID12                  LED_E1_ID(12)/*12号E1的告警灯*/
#define LED_E1_ID13                  LED_E1_ID(13)/*13号E1的告警灯*/
#define LED_E1_ID14                  LED_E1_ID(14)/*14号E1的告警灯*/
#define LED_E1_ID15                  LED_E1_ID(15)/*15号E1的告警灯*/
#define LED_SYS_ALARM                0xe0      /* 系统告警灯，所有板子有效 */
#define LED_SYS_LINK                 0xe1      /* 系统连接灯，所有板子有效 */
#define LED_BOARD_LED_NORM           0xe2      /* 灯板上正常指示灯，仅对KDV8000机框灯板有效 */
#define LED_BOARD_LED_NALM           0xe3      /* 灯板上一般告警指示灯，仅对KDV8000机框灯板有效 */
#define LED_BOARD_LED_SALM           0xe4      /* 灯板上严重告警指示灯，仅对KDV8000机框灯板有效 */
#define LED_CDMA_LED      	         0xe5      /* KDM2417上CDMA灯 */
#define LED_WLAN_LED      	         0xe6      /* KDM2417上WLAN灯 */
#define LED_DISK_LED      	         0xe7      /* KDM2417上存储盘指示灯 */
#define LED_MPC_OUS                  0xe8      /* mpc板的ous离线灯，数据同步用，同步后点灭 */

#define LED_BSP15_ID_BASE            0xf0
#define LED_BSP15_ID(ix)             (LED_BSP15_ID_BASE+ix)      /* bsp15指示灯 */
#define LED_BSP15_ID0                LED_BSP15_ID(0)      /* 0号bsp15指示灯 */
#define LED_BSP15_ID1                LED_BSP15_ID(1)      /* 1号bsp15指示灯 */
#define LED_BSP15_ID2                LED_BSP15_ID(2)      /* 2号bsp15指示灯 */
#define LED_BSP15_ID3                LED_BSP15_ID(3)      /* 3号bsp15指示灯 */
#define LED_BSP15_ID4                LED_BSP15_ID(4)      /* 4号bsp15指示灯 */
#define LED_BSP15_ID5                LED_BSP15_ID(5)      /* 5号bsp15指示灯 */

#define LED_V35_ID_BASE               0xc0
#define LED_V35_ID(ix)                (LED_V35_ID_BASE+ix)      /* 模块上0号V35告警灯*/
#define LED_V35_ID0                   LED_V35_ID(0)/*0号V35的告警灯*/
#define LED_V35_ID1                   LED_V35_ID(1)/*1号V35的告警灯*/
#define LED_V35_ID2                   LED_V35_ID(2)/*2号V35的告警灯*/
#define LED_V35_ID3                   LED_V35_ID(3)/*3号V35的告警灯*/
#define LED_V35_ID4                   LED_V35_ID(4)/*4号V35的告警灯*/
#define LED_V35_ID5                   LED_V35_ID(5)/*5号V35的告警灯*/


/* BrdSetEthParam函数中byIpOrMac输入值的宏定义 */
#define Brd_SET_IP_AND_MASK         1      /* 设置主IP地址和子网掩码 */
#define Brd_SET_MAC_ADDR            2      /* 设置MAC地址 */
#define Brd_SET_ETH_ALL_PARAM       3      /* 设置以太网的主IP地址、子网掩码和MAC地址 */

/* BrdSetSecondEthParam函数中byIpOrMac输入值的宏定义 */
#define Brd_SET_SEC_IP_AND_MASK     1      /* 设置从IP地址和子网掩码 */
#define Brd_DEL_SEC_IP_AND_MASK     2      /* 删除从IP地址和子网掩码 */
#define Brd_SET_SEC_ETH_ALL_PARAM   3      /* 设置以太网的从IP地址、子网掩码和MAC地址 */

/* ip地址配置状态 */
#define IP_SET_AND_UP	   1  /*某个指定IP地址已经配置并且端口处于UP状态 */
#define IP_SET_AND_DOWN    2  /*某个指定IP地址已经配置并且端口处于DOWN状态*/
#define IP_NOT_SET	       3  /*某个指定IP地址没有配置*/

/* SysSetAutoRunFile函数中用户程序启动参数设置 */
#define LOCAL_UNCOMPRESS_FILE       0      /* 本地未压缩的文件，一般以.out结尾 */ 
#define LOCAL_COMPRESS_FILE         1      /* 本地压缩的文件，一般以.z结尾 */ 
#define REMOTE_UNCOMPRESS_FILE      2      /* 远端未压缩的文件，一般以.out结尾 */ 
#define REMOTE_COMPRESS_FILE        3      /* 远端压缩的文件，一般以.z结尾 */ 
#define LOCAL_ZIP_FILE              4      /* 本地zip压缩包中的文件，一般以.out结尾 */ 
#define APP_MAX_NUM                     8   /* 允许装载应用程序最大个数 */
#define APP_NAME_MAX_LEN               31   /* 应用程序文件名最大长度 */

/* BrdSetWatchdogMode函数中看门狗宏定义 */
#define WATCHDOG_USE_CLK            0x00   /* 时钟硬件喂狗 */
#define WATCHDOG_USE_SOFT           0x01   /* 软件喂狗 */
#define WATCHDOG_STOP               0x02   /* 停止喂狗 */

/* BrdSetFanState函数中风扇运行状态宏定义 */
#define BRD_FAN_RUN                  0      /* 风扇转动 */
#define BRD_FAN_STOP                 1      /* 风扇停转 */

/* BrdFanIsStopped函数中风扇ID限制宏定义 */
#define BRD_FAN_MAX_NUM              8      /* 风扇最大个数 */


/*E1相关的宏定义*/
/* E1阻抗值 */
#define E1_75_OHM                   0      /* E1模块阻抗为75  ohm */
#define E1_120_OHM                  1      /* E1模块阻抗为120 ohm */
#define E1_ERR_OHM                  0xff    /* E1模块阻抗无法识别 */

/* 网同步时钟信号输出控制 */
#define E1_CLK_OUTPUT_DISABLE   0     /* 高阻禁止单板上参考时钟信号输出到MPC板上 */
#define E1_CLK_OUTPUT_ENABLE    1     /* 允许单板板上参考时钟信号输出到MPC板上 */

/* 继电器工作模式设置 */
#define BRD_RELAY_MODE_NORMAL        0     /* 继电器连通 */
#define BRD_RELAY_MODE_LOCLOOP       1     /* 继电器自环 */
#define BRD_RELAY_MODE_OUTLOOP       2     /* 继电器外环 */

/* 各单板最大E1个数 */
#define BRD_E1_MAXNUM                16    /* 一个设备最多有16个E1物理接口 */
#define BRD_DRI_E1_MAXNUM            8     /* DRI共有8条E1链路 */
#define BRD_DSI_E1_MAXNUM            4     /* DSI共有4条E1链路 */
#define BRD_MT_E1_MAXNUM             4     /* KDV8010共有4条E1链路 */
#define BRD_DRI16_E1_MAXNUM          16    /* DRI共有16条E1链路 */
#define BRD_16E1_E1_MAXNUM           16    /* 16E1共有16条E1链路 */
#define BRD_KDV8003_E1_MAXNUM        1     /* kdv8003共有1条E1链路 */
#define BRD_KDM2000_E1_MAXNUM        1     /* kdm2000最多有1条E1链路 */

/* E1线路NIP封装接口相关宏定义 */
#define E1_SINGLE_LINK_CHAN_MAX_NUM  32 /* 最大支持的E1单链路serial同步接口个数 */
#define E1_MULTI_LINK_CHAN_MAX_NUM   8 /* 最大支持的E1多链路捆绑连接通道(即virtual虚拟模板接口)个数 */

/*TBrdE1MultiLinkChanInfo结构体中dwProtocolType参数宏定义 */
#define INTERFACE_PROTOCOL_PPP       0 /* 接口协议PPP */
#define INTERFACE_PROTOCOL_HDLC      1 /* 接口协议HDLC */

/* TBrdE1MultiLinkChanInfo结构体中dwAuthenticationType参数宏定义 */
#define MP_AUTHENTICATION_NONE      0  /* 不设置验证方法 */
#define MP_AUTHENTICATION_PAP       1  /* PPP链接的验证方法PAP */
#define MP_AUTHENTICATION_CHAP      2  /* PPP链接的验证方法CHAP */

/* TBrdE1MultiLinkChanInfo结构体中dwFragMinPackageLen 参数范围宏定义 */
#define MP_FRAG_MIN_PACKAGE_LEN_MIN_VAL  20
#define MP_FRAG_MIN_PACKAGE_LEN_MAX_VAL  1500

#define MP_STRING_MAX_LEN            32  /* TBrdE1MultiLinkChanInfo结构体中字符串最大长度 */

/* TBrdE1SerialInfo结构体中dwSerialId 参数范围宏定义 */
#define E1_SERIAL_ID_MIN_VAL             16
#define E1_SERIAL_ID_MAX_VAL             62

/* TBrdE1SerialInfo结构体中wE1ChanGroupId 参数范围宏定义 */
#define E1_CHAN_GROUP_ID_MIN_VAL         0
#define E1_CHAN_GROUP_ID_MAX_VAL         7

/* TBrdE1SerialInfo结构体中dwEchoInterval dwEchoMaxRetry 参数最大值宏定义 */
#define E1_SERIAL_ECHO_MAX_VAL           256

/* dwFragMinPackageLen dwSerialId dwE1ChanGroupId 参数自动设置宏定义 */
#define E1_PARAM_AUTO_CONFIG            0xffffffff

/* 参数 wBufLength 范围定义 */
#define E1_TRANS_BUF_MAX_LEN   (u32)8000 /* 缓冲区队列中最大BUF的长度,单位字节 */
#define E1_TRANS_BUF_MIN_LEN   (u32)800  /* 缓冲区队列中最小BUF的长度,单位字节 */

#define E1_TRANS_CHAN_MAX_NUM  (u8)16    /* 最多支持的E1透明传输通道的个数，一个通道仅对应于一条物理E1线路 */

/* E1模块返回值及错误码定义 */
#define E1_RETURN_OK                           0                     /* 操作成功 */
#define E1_ERRCODE_BASE                        10                    /* 错误码基值 */
#define E1_ERR_UNKNOWN                         (E1_ERRCODE_BASE+0)   /* 未知错误 */
#define E1_ERR_PARAM_EXCEPTION                 (E1_ERRCODE_BASE+1)   /* 参数异常 */
#define E1_ERR_SERIAL_ID_INVALID               (E1_ERRCODE_BASE+2)   /* SERIAL同步接口号无效 */
#define E1_ERR_E1_ID_INVALID                   (E1_ERRCODE_BASE+3)   /* E1接口号无效 */
#define E1_ERR_E1_TSMASK_INVALID               (E1_ERRCODE_BASE+4)   /* E1时隙掩码无效 */
#define E1_ERR_E1_CHANGROUP_ID_INVALID         (E1_ERRCODE_BASE+5)   /* E1时隙分配所在的编组号无效 */
#define E1_ERR_SERIAL_PROTOCOL_TYPE_INVALID    (E1_ERRCODE_BASE+6) /* 接口协议封装类型无效 */
#define E1_ERR_SERIAL_ECHO_INTERVAL_INVALID    (E1_ERRCODE_BASE+7) /* 同步接口的echo请求时间间隔无效 */
#define E1_ERR_SERIAL_ECHO_MAXRETRY_INVALID    (E1_ERRCODE_BASE+8) /* 同步接口的echo最大重发次数无效 */
#define E1_ERR_STRING_LEN_OVERFLOW             (E1_ERRCODE_BASE+9) /* 字符串长度溢出 */
#define E1_ERR_NIP_OPT_ERR                     (E1_ERRCODE_BASE+10)/* nip接口调用失败 */
#define E1_ERR_CHAN_NOT_CONF                   (E1_ERRCODE_BASE+11)/* 指定的通道号未打开使用 */
#define E1_ERR_CHAN_CONFLICT                   (E1_ERRCODE_BASE+12)/* 通道号冲突，指定的通道号已被使用 */
#define E1_ERR_MP_AUTHEN_TYPE_INVALID          (E1_ERRCODE_BASE+13)/* PPP链接的验证方法类型无效 */
#define E1_ERR_MP_FRAG_MIN_PACKAGE_LEN_INVALID (E1_ERRCODE_BASE+14)/* 最小分片包长无效 */

/* E1透明传输模块返回值和错误码描述 */
#define E1_TRANS_SUCCESS       (s32)0                         /* 成功 */

#define E1_TRANS_FAILURE       (s32)-1                        /* 未知的异常失败 */
#define E1_TRANS_ERRCODE_BASE  (s32)1                         /* 错误码基值 */
#define E1_TRANS_NOT_INIT      (s32)(E1_TRANS_ERRCODE_BASE+0) /* 通道未打开 */
#define E1_TRANS_LENGTH_ERROR  (s32)(E1_TRANS_ERRCODE_BASE+1) /* 数据长度错误 */
#define E1_TRANS_NO_BUF        (s32)(E1_TRANS_ERRCODE_BASE+2) /* 没有可用的BUF */

/*所有带BSP15芯片单板的宏定义*/
/* BSP15图像采集端口宏定义 */
#define BSP15_VCAPTURE_FROM_NONE   0       /* 不从任何口采集视频数据 */
#define BSP15_VCAPTURE_FROM_PORTA  1       /*从A口采集数据*/
#define BSP15_VCAPTURE_FROM_PORTB  2      /*从B口采集数据*/
#define BSP15_VCAPTURE_FROM_PORTAB 3      /* 同时从A/B口采集数据*/

/* BSP15图像采集芯片BrdGetCaptureChipType返回值宏定义 */
#define CAPTURE_FROM_NONE           0       /*没有视频采集芯片 */
#define CAPTURE_FROM_SAA7114        1       /*通过SAA7114采集视频数据*/
#define CAPTURE_FROM_SAA7113        2       /*通过SAA7113采集视频数据*/
#define CAPTURE_FROM_AL240          3       /*通过AL240采集视频数据    */

/* BSP15声音采集芯片BrdGetAudCapChipType返回值宏定义 */
#define CAPTURE_AUDIO_FROM_NONE     0       /* 没有音频采集芯片*/
#define CAPTURE_AUDIO_FROM_5331     1       /* 从5331采集音频数据*/
#define CAPTURE_AUDIO_FROM_AIC23B   2       /*从AIC23B采集音频数据*/

/* SAA7114图像数据输出端口宏定义 */
#define SAA7114_OUTPORT_NONE        0       /* SAA7114 没有从任何口数据数据*/
#define SAA7114_OUTPORT_X           1       /* SAA7114从X口输出数据*/
#define SAA7114_OUTPORT_I           2       /* SAA7114从I口输出数据*/
#define SAA7114_OUTPORT_XI          3       /* SAA7114同时从X/I口输出数据*/

/* VGA输入输出配置的宏定义 */
#define BSP15_VGA_NONE              0       /* vga输入输出功能均没有 */
#define BSP15_VGA_IN                1       /* 只有vga输入功能 */
#define BSP15_VGA_OUT               2       /* 只有vga输出功能 */
#define BSP15_VGA_INOUT             3       /* vga输入输出功能都有 */

/* VGA输入模式宏定义 */
#define VGAMODE_SVGA_60HZ  0               /* 分辨率800x600  刷新率60Hz */
#define VGAMODE_SVGA_75HZ  1               /* 分辨率800x600  刷新率75Hz */
#define VGAMODE_SVGA_85HZ  2               /* 分辨率800x600  刷新率85Hz */
#define VGAMODE_VGA_60HZ   3               /* 分辨率640x480  刷新率60Hz */
#define VGAMODE_VGA_75HZ   4               /* 分辨率640x480  刷新率75Hz */
#define VGAMODE_VGA_85HZ   5               /* 分辨率640x480  刷新率85Hz */
#define VGAMODE_XGA_60HZ   6               /* 分辨率1024x768 刷新率60Hz */
#define VGAMODE_XGA_75HZ   7               /* 分辨率1024x768 刷新率75Hz */
/* 硬件第11版kdv8010a专用的gpio模式，BrdSetBsp15GPIOMode用 */
#define BSP_GPIO_MODE_1TO2  (0x00) /* 编码BSP15的视频                        输出口连接到解码BSP15的GPIO口，同时VGA编解码BSP15的视频                        输出口连接到SAA7121的视频输入口,仅对第11版kdv8010a有效 */
#define BSP_GPIO_MODE_3TO2  (0x01)  /* VGA编解码BSP15的                        视频输出口连接到解码BSP15的GPIO口 */

/*所有带音视频矩阵单板的宏定义*/
#define  MATRIX_OUTPUT_DISABLE  0xfb        /* 禁止音视频矩阵某输出端口 */
#define  MATRIX_OUTPUT_ENABLE   0xfc        /* 启用音视频矩阵某输出端口 */

/*单板的宏定义*/
/*MPC板相关的宏定义*/
/* 对端主处理机板是否在位宏定义 */
#define BRD_MPC_IN_POSITION         0      /* 对端主处理机板在位  */
#define BRD_MPC_OUTOF_POSITION      1      /* 对端主处理机板不在位 */

/* 当前主处理机板主备用状态 */
#define BRD_MPC_RUN_MASTER          0      /* 当前主处理机板主用 */
#define BRD_MPC_RUN_SLAVE           1      /* 当前主处理机板备用  */

/* 光模块种类 */
#define BRD_MPC_SDHMODULE_NONE      0x07   /* 当前主处理机板没有插模块 */
#define BRD_MPC_SDHMODULE_SOI1      0x00   /* 当前主处理机板插模块SOI1 */
#define BRD_MPC_SDHMODULE_SOI4      0x01   /* 当前主处理机板插模块SOI4  */

/* 锁相环工作模式 */
#define SYNC_MODE_FREERUN           0      /* 自由振荡，对于主MCU应设置为该模式，所有下级线路时钟与该MCU同步 */
#define SYNC_MODE_TRACK_SDH8K       1      /* 跟踪模式，作为下级MCU应设置为该模式，跟踪SDH来的8K时钟 */
#define SYNC_MODE_TRACK_SDH2M       2      /* 跟踪模式，作为下级MCU应设置为该模式，跟踪SDH来的2M时钟 */
#define SYNC_MODE_TRACK_DT2M        4      /* 跟踪模式，作为下级MCU应设置为该模式，跟踪DT来的2M时钟 */
#define SYNC_MODE_UNKNOWN           0xff   /* 未知或错误的模式 */

/* KDV8000机框扬声器开关宏定义 */
#define LED_BOARD_SPK_OFF           0x00   /* 关闭扬声器 */
#define LED_BOARD_SPK_ON            0x01   /* 开启扬声器 */

/*CRI板相关的宏定义*/
#define OS_CRI_FOR_MP               0 /*MP的OS版本*/
#define OS_CRI_FOR_GK               1/*GK的OS版本*/

/*VAS板相关的宏定义*/
#define  VAS_MATRIX_16X16           0        /* 16X16的矩阵板 */
#define  VAS_MATRIX_08X08           8        /* 8X8的矩阵板 */

/*KDV8010A板相关的宏定义*/
/* 模块宏定义 */
#define KDV8010A_MODULE_E1          0        /* 外挂模块为: 单E1模块   */
#define KDV8010A_MODULE_4E1         1        /* 外挂模块为: 4E1模块    */
#define KDV8010A_MODULE_V35DTE      2        /* 外挂模块为: V35DTE模块 */
#define KDV8010A_MODULE_V35DCE      3        /* 外挂模块为: V35DCE模块 */
#define KDV8010A_MODULE_BRIDGE      4        /* 外挂模块为: 网桥模块   */
#define KDV8010A_MODULE_2E1         5        /* 外挂模块为: 2E1模块    */
#define KDV8010A_MODULE_KDV8000B    6        /* 外挂模块为: KDV8000B模块    */
#define KDV8010A_MODULE_DSC         0xc      /* 外挂模块为: DSC(同KDV8000B)模块 */
#define KDV8010A_MODULE_MDSC         0xd      /* 外挂模块为: MDSC*/
#define KDV8010A_MODULE_HDSC         0xe      /* 外挂模块为: HDSC */
#define KDV8010A_NO_MODULE          0xf      /* 没有外挂模块  */

/*串口相关的宏定义*/
/* BrdOpenSerial函数打开串口类型宏定义 */
#define BRD_SERIAL_RS232             0
#define BRD_SERIAL_RS422             1
#define BRD_SERIAL_RS485             2
#define BRD_SERIAL_INFRARED          3

/* 485状态相关宏定义 */
#define RS485_SUCCESS                     0    /* 操作成功 */
#define RS485_NOT_OPENED                  1    /* RS485串口没有打开 */
#define RS485_NOT_CONNECTED               2    /* RS485设备没有在线 */
#define RS485_SND_TIMEOUT                 3    /* RS485发送操作超时 */
#define RS485_RCV_TIMEOUT                 4    /* RS485接收操作超时 */
#define RS485_RCV_LENERR                  5    /* RS485接收的数据长度错 */
#define RS485_RCV_ERRDATA                 6    /* RS485接收的错包 */
#define RS485_ERROR                       -1    /* 操作错误 */

#define RS485_SET_SND_TIMEOUT   4   /* 设置485串口操作的发送超时值 */
#define RS485_GET_SND_TIMEOUT   5   /* 查询485串口操作的发送超时值 */

/*通用串口相关宏定义*/
#define SIO_SET_BAUDRATE        0x2000   /* 设置串口的波特率 */
#define SIO_GET_BAUDRATE        0x2001   /* 查询串口的波特率 */
#define SIO_SET_STOPBIT         0x2002   /* 设置串口的停止位 */
#define SIO_GET_STOPBIT         0x2003   /* 查询串口的停止位 */
#define SIO_SET_DATABIT         0x2004   /* 设置串口的数据位 */
#define SIO_GET_DATABIT         0x2005   /* 查询串口的数据位 */
#define SIO_SET_PARITY          0x2006   /* 设置串口的奇偶校验位 */
#define SIO_GET_PARITY          0x2007   /* 查询串口的奇偶校验位 */
#define SIO_485_SET_SND_TIMEOUT 0x2008   /* 设置485串口操作的发送超时值 */
#define SIO_485_GET_SND_TIMEOUT 0x2009   /* 查询485串口操作的发送超时值 */

#define SIO_PARITY_NONE       0   /* 串口无奇偶校验位 */
#define SIO_PARITY_ODD        1   /* 串口奇校验 */
#define SIO_PARITY_EVEN       2   /* 串口偶校验 */
#define SIO_PARITY_MARK       3   /* 串口标记校验 */
#define SIO_PARITY_SPACE      4   /* 串口空格校验 */

#define SIO_STOPBIT_1         0   /* 串口一个停止位 */
#define SIO_STOPBIT_2         1   /* 串口两个停止*/

/* =485设备类型= */
#define RS485DEV_IRAY           0   /* IRAY设备 */

/* =红外遥控器类型= */
#define IRAY_CTRL_BLACK         0   /* 黑色旧版的遥控器 */
#define IRAY_CTRL_SILVERY       1   /* 银白色新版的遥控器 */
#define IRAY_CTRL_UNKNOWN       0xff   /* 未知的遥控器 */

#define IRAY_RCV_NOKEY          0xfe   /* iray没有收到遥控器信息 */

#define IRAY_ADDR0              0x02   /* IRAY设备地址1 */
#define IRAY_ADDR1              0x7f   /* IRAY设备地址2 */
#define HOST_485DEV_ADDR        0x01   /* 主设备地址 */

#define IRAY_CMD_QRY_LNKSTATE   0   /* 查询IRAY设备是否在线 */
#define IRAY_CMD_QRY_KEYCODE    1   /* 查询IRAY设备接收的键码 */

/*其他宏定义*/
#define BRD_ALM_NUM_MAX         256  /* 最大告警单元个数 */

/*RawFlash相关的宏定义*/
#define PARTITION_MAX_NUM           8       /* 数据分区最大个数，请注意：底层最多可以支持8个分区，但是分区表和Boot分区用户是不可见的，因此，用户最多可以设置6个分区*/

#define PARTITION_TYPE_NOASSIGN     0       /* 表示为未分配的分区 */
#define PARTITION_TYPE_AUXDATA      1       /* 表示为操作系统应用程序复合分区 */
#define PARTITION_TYPE_USRDATA      2       /* 表示为用户数据分区 */
#define PARTITION_TYPE_JFFS         4       /* 表示jffs文件系统分区 */
#define PARTITION_TYPE_BOOT         0x8     /* 表示BOOT分区 */
#define PARTITION_TYPE_PTABLE       0x10    /* 表示存放分区表的分区 */
#define PARTITION_TYPE_APP          0x20    /* 表示APP系统分区 */

#define PARTITION_TYPE_MAXNUM       6       /* 表示分区类型的总数 */

#define EXECODE_TYPE_NONE           0       /* 特殊可执行程序数据区不使用 */
#define EXECODE_TYPE_FPGA  0xfa002005       /* 特殊可执行程序数据区存放fpga数据 */



/*函数返回值定义*/
#define OK						0
#define ERROR					-1
#define STATUS					int

/*兼容VxWorks的类型定义*/
#define INT32 s32
#define UINT32 u32
#define INT16 s16
#define UINT16 u16
#define INT8 s8
#define UINT8 u8
#define BOOL BOOL32


/*业务程序数据存放路径*/
#define USER_PATH               "/usr/bin/"
/*业务程序备份数据的目录*/
#define USER_BAK_PATH           "/usr/bak/"
/*ramdisk路径*/
#define RAMDISK_PATH           "/ramdisk/"
/*RTC芯片类型定义*/
#define RTC_TYPE_NONE           0
#define RTC_TYPE_DS1337         1
#define  RTC_TYPE_DS12C887       2

/*CPU类型信息的最大长度*/
#define CPU_TYPE_MAX_LEN        128
/*u-boot版本信息的最大长度*/
#define UBOOT_VERSION_MAX_LEN   128
/*内核版本信息的最大长度*/
#define KERNEL_VERSION_MAX_LEN  128

#define IP_ADDR_MAX_NUM	         16 /*在Linux下一个网口可以配置的最多的IP地址*/

#ifdef __cplusplus 
} 
#endif /* __cplusplus */

#endif /*_BOARDWRAPPERDEF_H*/
