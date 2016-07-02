/******************************************************************************
模块名  ： BrdWrapper
文件名  ： brdwrapperdef.h
相关文件：
文件实现功能：BrdWrapper模块对外提供的宏定义头文件，该文件目前主要应用于监控设备板级设
            备驱动。
作者    : 张方明
版本    ：1.0
-------------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
12/27/2006  1.0        张方明      创建
******************************************************************************/
#ifndef __BRD_WRAPPER_DEF_H
#define __BRD_WRAPPER_DEF_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/* 单板种类ID号宏定义 */
#define UNKNOWN_BOARD               0xff  /* 未定义的设备 */

#define BRD_KDM2418                 0x02  /* 监控前端设备 */
#define BRD_KDM2518                 0x03  /* 监控前端设备 */

#define BRD_KDM2400P                0x10  /* 监控前端设备 */

#define BRD_KDM2464LS               0x22  /* 监控前端设备 */
#define BRD_KDM2110                 0x23  /* 监控前端设备 */
#define BRD_KDM2422S                0x24  /* 监控前端设备 */
#define BRD_KDM2422LS               0x25  /* 监控前端设备 */
#define BRD_KDM2421S                0x26  /* 监控前端设备 */
#define BRD_KDM2421LS               0x27  /* 监控前端设备 */
#define BRD_KDM2110L                0x28  /* 监控前端设备 */
#define BRD_KDM2300                 0x29  /* 监控前端设备 */
#define BRD_KDM2300P                0x2a  /* 监控前端设备 */

#define BRD_KDM2561                 0x30  /*  监控前端设备 */
#define BRD_KDM2401                 0x39   /* KDM2401编码器 */
#define BRD_KDM2401ES               0x3a   /* KDM2401ES编码器 */
#define BRD_KDM2424LS               0x3b  /*  监控前端设备 */
#define BRD_KDM2501                 0x3c   /* KDM2501编码器 */
#define BRD_KDM2401S                0x46   /* 2401S编码器 */
#define BRD_KDM2402                 0x47   /* 2402编码器 */
#define BRD_KDM2402S                0x48   /* 2402S编码器 */
#define BRD_KDM2401L                0x49   /* 2402S编码器 */
#define BRD_KDM2461                 0x4a   /* 2461编码器 */
#define BRD_KDM2461L                0x4b   /* 2461L编码器 */
#define BRD_KDM2401LS               0x4c   /* 2401LS编码器 */
#define BRD_KDM2402L                0x4d   /* 2402L编码器 */
#define BRD_KDM2402LS               0x4e   /* 2402LS编码器 */
#define BRD_KDM201C04               0x4f  /*  监控前端设备 */
#define BRD_KDM201C04L              0x50   /* KDM201C04L编码器 */
#define BRD_KDM201D04               0x51   /* KDM201D04编码器 */

#define BRD_KDM2440                 0x52   /* KDM2440编码器 */
#define BRD_KDM2440P                0x53   /* KDM2440P编码器 */
#define BRD_KDM2462                 0x54   /* 与KDM2402相同 */
#define BRD_KDM2462L                0x55   /* 与KDM2402L相同 */
#define BRD_KDM2462S                0x56   /* 与KDM2402S相同 */
#define BRD_KDM2462LS               0x57   /* 与KDM2402LS相同 */
#define BRD_KDM2461LS               0x58   /* 与KDM2401LS相同 */
#define BRD_KDM2461S                0x59   /* 与KDM2401S相同 */
#define BRD_KDM2100                 0x5a  /*  监控前端设备 */
#define BRD_KDM2100W                0x5b  /*  监控前端设备 */
#define BRD_KDM2100P                0x5c  /*  监控前端设备 */

#define BRD_KDM2820                 0x20   /* KDM2820 */
#define BRD_KDM2820_4               0x2B   /*KDM2820*/
#define BRD_KDM2820_9               0x2C   /*KDM2820*/
#define BRD_KDM2820_16              0x2D   /*KDM2820*/
#define BRD_KDM2820E_9              0x2E   /*KDM2820*/
#define BRD_KDM2820E_16             0x2F   /*KDM2820*/
#define BRD_KDM2404S                0x3b   /* KDM2404S编码器 */
#define BRD_KDM200_MPU              0x3d   /* KDM200机框中的MPU板 */

#define BRD_KDM200_APC              0x42   /* KDM200机框中的APC板 */

#define BRD_KDV8000BHD              0x6E   /* KDV8000B-HD 高清配套产品 */

#define BRD_TS6610                  0x70   /* TS6610视频终端, KDV8220A的最新命名 */
#define BRD_TS5210                  0x71   /* TS5210视频终端, KDV8220B的最新命名 */
#define BRD_V5                      0x72   /* V5视频终端, KDV8220C的最新命名 */
#define BRD_TS6610E                 0x73   /* TS6610E视频终端 */
#define BRD_TS6210                  0x74   /* TS6210视频终端 */
#define BRD_TS6210E                 0x80   /* TS6210E视频终端 */

#define BRD_TS3210                  0xa0   /* TS3210视频终端 */
#define BRD_TS5610                  0x75   /* TS5610视频终端 */
#define BRD_TS3610                  0x76   /* TS3610视频终端 */
#define BRD_TS7210                  0x77   /* TS7210视频终端 */
#define BRD_TS7610                  0x78   /* TS7610视频终端 */
#define BRD_KDV7810                 0x7b   /* KDV7810视频终端 */
#define BRD_KDV7910                 0x7c   /* KDV7910视频终端 */

#define BRD_KDVM26401               0xf0   /* KDVM26401设备 */
#define BRD_KDVM26402               0xf1   /* KDVM26402开发板，非公司定义，以后删掉 */
#define BRD_MPC8247_2DM642          0x96   /* MPC8247_2DM642开发板，非公司定义，以后删掉 */

/******************************************************************/
/***     New Board Type defines (PID/HID)                       ***/
/******************************************************************/
#define BRD_HWID_KDM2421E           0x003E
#define BRD_HWID_KDM2210            0x0040
#define BRD_HWID_KDM2700	    			0x0041  /*  监控前端设备 */
#define BRD_HWID_DSL8000_MPU        0x0042
#define BRD_HWID_EBAP               0x0043
#define BRD_HWID_EVPU               0x0044
#define BRD_HWID_MAU                0x0045
#define BRD_HWID_KDM200_HDU         0x0046
#define BRD_HWID_KDM2310            0x004A  /*  监控前端设备 */
#define BRD_HWID_KDM2311            0x004B  /*  监控前端设备 */
#define BRD_HWID_KDV7620            0x001F  /* KDV7620视频终端 */
#define BRD_HWID_KDV7820            0x0021  /* KDV7820视频终端 */
#define BRD_HWID_H600_H 	   		0x012c  /* KDV_H600高端视频终端 */
#define BRD_HWID_H600_L	   	  		0x012d  /* KDV_H600低端视频终端 */
#define BRD_HWID_H700 	            0x012E  /* KDV_H700视频终端 */
#define BRD_HWID_H900 	            0x012F  /* KDV_H900视频终端 */
#define BRD_HWID_MPU2				0x0134
#define BRD_HWID_HDU2				0x0135
#define BRD_HWID_APU2				0x0136	/* APU2板 */
#define BRD_HWID_KDV8000I			0x0147     /*KDV8000I*/
#define BRD_HWID_IPA100             0x0100  /*  NVR监控前端设备 */
#define BRD_HWID_IPA101             0x0067  /*  NVR监控前端设备 */
#define BRD_HWID_IPC201             0x006A  /*  NVR监控前端设备 */
#define BRD_HWID_NVR2860            0x0101  /*NVR2860*/
#define BRD_HWID_MPC2   		 	0x010C      /* MPC2板 */
#define BRD_HWID_DRI2   		 	0x010E      /* DRI2板 */
#define BRD_HWID_CRI2			 	0x010D 	    /*CRI2板*/
#define BRD_HWID_IS2_1   		 	0x010F  /* IS2.1板 对应MPC8313系统*/
#define BRD_HWID_IS2_2   		 	0x0110  /* IS2.2板 对应MPC8548系统*/
#define BRD_HWID_EAPU			 0x0111	/*EAPU板*/

/* 指示灯部分宏定义 */
#define BRD_LED_NUM_MAX             32     /* 指示灯极限个数 */

#define BRD_LED_ON                  1      /* 指示灯状态: 亮 */
#define BRD_LED_OFF                 2      /* 指示灯状态: 灭 */
#define BRD_LED_QUICK               3      /* 指示灯状态: 快闪(0.25秒亮->0.25秒灭->0.25秒亮...) */
#define BRD_LED_SLOW                4      /* 指示灯状态: 慢闪(1秒亮->1秒灭->1秒亮...) */
#define BRD_LED_STANDBY             5      /* 指示灯状态: standby(2秒亮->2秒灭->2秒亮...) */

/* BrdLedStatusSet函数中指示灯控制ID */
#define LED_E1_ID_BASE              0xd0
#define LED_E1_ID(ix)               (LED_E1_ID_BASE+ix)      /* 模块上0号E1告警灯*/
#define LED_E1_ID0                  LED_E1_ID(0)/*0号E1的告警灯*/
#define LED_E1_ID1                  LED_E1_ID(1)/*1号E1的告警灯*/
#define LED_E1_ID2                  LED_E1_ID(2)/*2号E1的告警灯*/
#define LED_E1_ID3                  LED_E1_ID(3)/*3号E1的告警灯*/
#define LED_E1_ID4                  LED_E1_ID(4)/*4号E1的告警灯*/
#define LED_E1_ID5                  LED_E1_ID(5)/*5号E1的告警灯*/
#define LED_E1_ID6                  LED_E1_ID(6)/*6号E1的告警灯*/
#define LED_E1_ID7                  LED_E1_ID(7)/*7号E1的告警灯*/
#define LED_E1_ID8                  LED_E1_ID(8)/*8号E1的告警灯*/
#define LED_E1_ID9                  LED_E1_ID(9)/*9号E1的告警灯*/
#define LED_E1_ID10                 LED_E1_ID(10)/*10号E1的告警灯*/
#define LED_E1_ID11                 LED_E1_ID(11)/*11号E1的告警灯*/
#define LED_E1_ID12                 LED_E1_ID(12)/*12号E1的告警灯*/
#define LED_E1_ID13                 LED_E1_ID(13)/*13号E1的告警灯*/
#define LED_E1_ID14                 LED_E1_ID(14)/*14号E1的告警灯*/
#define LED_E1_ID15                 LED_E1_ID(15)/*15号E1的告警灯*/

#define LED_SYS_ALARM               0xe0   /* 系统告警灯，所有板子有效 */
#define LED_SYS_LINK                0xe1   /* 系统连接灯(部分板子取名ACT)，所有板子有效 */
#define LED_BOARD_LED_NORM          0xe2   /* 灯板上正常指示灯，仅对KDV8000机框灯板有效 */
#define LED_BOARD_LED_NALM          0xe3   /* 灯板上一般告警指示灯，仅对KDV8000机框灯板有效 */
#define LED_BOARD_LED_SALM          0xe4   /* 灯板上严重告警指示灯，仅对KDV8000机框灯板有效 */
#define LED_CDMA_LED                0xe5   /* KDM2417上CDMA灯 */
#define LED_WLAN_LED                0xe6   /* KDM2417上WLAN灯 */
#define LED_DISK_LED                0xe7   /* KDM2417上存储盘指示灯 */
#define LED_MPC_OUS                 0xe8   /* mpc板的ous离线灯，数据同步用，同步后点灭 */
#define LED_VIDEO_IN0               0xe9   /* 视频输入指示灯0 */
#define LED_VIDEO_IN1               0xea   /* 视频输入指示灯1 */
#define LED_VIDEO_IN2               0xeb   /* 视频输入指示灯2 */
#define LED_VIDEO_IN3               0xec   /* 视频输入指示灯3 */
#define LED_SYS_RUN                 0xef   /* 系统运行灯，所有板子有效 */
#define LED_ENCODER_RUN             0xf0   /* KDV7810 ENCODER LED */
#define LED_DECODER_RUN             0xf1   /* KDV7810 DECODER LED */
#define LED_MPC_SYN				0xf2   /*mpc前面板的syn 指示灯 */
#define LED_CRI2_MS				0xf3   /*CRI2主备灯*/

/*-----------------T2指示灯说明-------------------------------
     |   长亮        快闪        慢闪           灭
-------------------------------------------------------------
绿灯    |   正常运行    遥控信号     启动过程中
橙灯    |   待机        错误         升级中         正常运行
*/
#define LED_GREEN                   0xed   /* 绿色指示灯 */
#define LED_ORANGE                  0xee   /* 橙色指示灯,当绿灯同时也亮时，被橙色掩盖 */
#define LED_RED                     0xef   /* 红色指示灯 */

#define LED_DSP_ID_BASE             0xf0
#define LED_DSP_ID(ix)              (LED_DSP_ID_BASE+ix) /* DSP指示灯 */
#define LED_DSP_ID0                 LED_DSP_ID(0)   /* 0号DSP指示灯 */
#define LED_DSP_ID1                 LED_DSP_ID(1)   /* 1号DSP指示灯 */
#define LED_DSP_ID2                 LED_DSP_ID(2)   /* 2号DSP指示灯 */
#define LED_DSP_ID3                 LED_DSP_ID(3)   /* 3号DSP指示灯 */
#define LED_DSP_ID4                 LED_DSP_ID(4)   /* 4号DSP指示灯 */
#define LED_DSP_ID5                 LED_DSP_ID(5)   /* 5号DSP指示灯 */

/*CRI2 前面板功能指示灯fun1 ~ fun8*/
#define LED_FUN_ID_BASE              0xc0
#define LED_FUN_ID(ix)               (LED_FUN_ID_BASE+ix)   /* 模块上0号E1告警灯*/
#define LED_FUN_ID1                  	LED_FUN_ID(0)		/*1号功能指示灯*/
#define LED_FUN_ID2                  	LED_FUN_ID(1)		/*2号功能指示灯*/
#define LED_FUN_ID3                  	LED_FUN_ID(2)		/*3号功能指示灯*/
#define LED_FUN_ID4                  	LED_FUN_ID(3)		/*4号功能指示灯*/
#define LED_FUN_ID5                  	LED_FUN_ID(4)		/*5号功能指示灯*/
#define LED_FUN_ID6                  	LED_FUN_ID(5)		/*6号功能指示灯*/
#define LED_FUN_ID7                  	LED_FUN_ID(6)		/*7号功能指示灯*/
#define LED_FUN_ID8                  	LED_FUN_ID(7)		/*8号功能指示灯*/



/*KDV8000A 机框风扇板指示灯1 ~ 8*/
#define FAN_LED_FUN_ID_BASE              0xa0
#define FAN_LED_FUN_ID(ix)               (FAN_LED_FUN_ID_BASE+ix)   /* 模块上0号E1告警灯*/
#define FAN_LED_FUN_ID1                  	FAN_LED_FUN_ID(0)		/*1号功能指示灯*/
#define FAN_LED_FUN_ID2                  	FAN_LED_FUN_ID(1)		/*2号功能指示灯*/
#define FAN_LED_FUN_ID3                  	FAN_LED_FUN_ID(2)		/*3号功能指示灯*/
#define FAN_LED_FUN_ID4                  	FAN_LED_FUN_ID(3)		/*4号功能指示灯*/
#define FAN_LED_FUN_ID5                  	FAN_LED_FUN_ID(4)		/*5号功能指示灯*/
#define FAN_LED_FUN_ID6                  	FAN_LED_FUN_ID(5)		/*6号功能指示灯*/
#define FAN_LED_FUN_ID7                  	FAN_LED_FUN_ID(6)		/*7号功能指示灯*/
#define FAN_LED_FUN_ID8                  	FAN_LED_FUN_ID(7)		/*8号功能指示灯*/

/* KDV8000机框扬声器开关宏定义 */
#define LED_BOARD_SPK_OFF           0x00   /* 关闭扬声器 */
#define LED_BOARD_SPK_ON            0x01   /* 开启扬声器 */


/*RTC芯片类型定义*/
#define RTC_TYPE_NONE               0
#define RTC_TYPE_DS1337             1
#define RTC_TYPE_DS12C887           2
#define RTC_TYPE_PCF8563            3

/* 函数返回值定义 */
#define OK                          0
#define ERROR                       -1
#define STATUS                      int

/* 常用的极限值定义 */
#define INTERFACE_NAME_MAX_LEN      10     /* 接口名称的最大长度 */
#define TTY_NAME_MAX_LEN            20     /* 串口设备名的最大长度 */
#define STR_NAME_MAX_LEN            16     /* 常用字符串的最大长度 */
#define ETH_MAC_MAX_NUM             8      /* 以太网MAC地址的最大个数 */
#define MPU_SUBCARD_NUM             7      /* MPU板支持的子卡个数 */

/* FXO模块控制命令ID */
#define FXO_SET_HANGUP              0x00   /* 挂机 */
#define FXO_SET_PICKUP2CALL         0x01   /* 摘机拨号 */
#define FXO_SET_TALK                0x02   /* 通话 */
#define FXO_SET_RCV_RING            0x03   /* 接收铃音 */
#define FXO_EN_REMOTE_SPEAK         0x04   /* 允许对端电话用户发言 */
#define FXO_DIS_REMOTE_SPEAK        0x05   /* 禁止对端电话用户发言 */
#define FXO_EN_REMOTE_LISTEN        0x06   /* 允许对端电话用户听到会场发言 */
#define FXO_DIS_REMOTE_LISTEN       0x07   /* 禁止对端电话用户听到会场发言 */
#define FXO_GET_STATE               0x80   /* 查询FXO当前状态 */
#define FXO_SET_RING_VOLUME_IN      0x09   /*环路音量输入(AD->环路)*/
#define FXO_SET_RING_VOLUME_OUT     0x0a   /* 环路音量输入(环路->AD)*/

/* FXO工作状态 */
#define FXO_STATE_RING              0x00   /* 振铃 */
#define FXO_STATE_PICKUP            0x01   /* 摘机 */
#define FXO_STATE_HANGUP            0x02   /* 挂机 */

/* 看门狗宏定义 */
#define WATCHDOG_USE_CLK            0x00   /* 时钟硬件喂狗 */
#define WATCHDOG_USE_SOFT           0x01   /* 软件喂狗 */
#define WATCHDOG_STOP               0x02   /* 停止喂狗 */

/* 系统告警宏定义 */
#define BRD_ALM_NUM_MAX             256    /* 最大告警单元个数 */

/*----- 扩展的视频接口宏定义,当dwInterface|VIDOUT_INTF_EXP表示有效----
  ----- 此时adwExtIntf变量可以使用来表示一个芯片支持的多种接口   ---- */
#define VID_INTF_EXP                  0x20000000  /* 扩展的物理视频接口的标志 */
#define VID_INTF_TYPE                 0x0f000000  /* 视频接口类型掩码 */
#define VID_INTF_ID                   0x000000ff  /* 视频接口索引掩码 */
#define VID_INTF_HDMI                 0x01000000  /* HDMI视频接口标识 */
#define VID_INTF_VGA                  0x02000000  /* VGA视频接口标识 */
#define VID_INTF_YPbPr                0x03000000  /* YPbPr视频接口标识 */
#define VID_INTF_SDI                  0x04000000  /* SDI视频接口标识 */
#define VID_INTF_C                    0x05000000  /* C端子视频接口标识 */
#define VID_INTF_S                    0x06000000  /* S端子视频接口标识 */
#define VID_INTF_DVI                  0x07000000  /* DVI视频接口标识 */

#define VID_INTF_HDMI_(x)             (VID_INTF_EXP | VID_INTF_HDMI  | (x&VID_INTF_ID))
#define VID_INTF_VGA_(x)              (VID_INTF_EXP | VID_INTF_VGA   | (x&VID_INTF_ID))
#define VID_INTF_YPbPr_(x)            (VID_INTF_EXP | VID_INTF_YPbPr | (x&VID_INTF_ID))
#define VID_INTF_SDI_(x)              (VID_INTF_EXP | VID_INTF_SDI   | (x&VID_INTF_ID))
#define VID_INTF_C_(x)                (VID_INTF_EXP | VID_INTF_C     | (x&VID_INTF_ID))
#define VID_INTF_S_(x)                (VID_INTF_EXP | VID_INTF_S     | (x&VID_INTF_ID))
#define VID_INTF_DVI_(x)              (VID_INTF_EXP | VID_INTF_DVI   | (x&VID_INTF_ID))
/* --------------------------------------------------------------- */


/* ---------------- 视频输入输出接口类型宏定义 ---------- */
/* 物理接口 */
#define VID_SHUT_DOWN                 0x00000000
#define VID_HDMI0                     0x00000001
#define VID_HDMI1                     0x00000002
#define VID_HDMI2                     0x00000004
#define VID_HDMI3                     0x00000008
#define VIDIN_HDMI4                   0x00800000  /* S端子极少使用，复用一下 */
#define VID_HDMI_81xx                 0x00400000


//#define VID_PE1005S                   0x0d    /* HD HDMI/VGA/YPbPr receiver */
//#define VID_SE600                     0x10    /* SE600机芯，对应H600-L型号 */

#define VID_VGA0                      0x00000010
#define VID_VGA1                      0x00000020
#define VID_VGA2                      0x00000040
#define VID_VGA3                      0x00000080
#define VID_YPbPr0                    0x00000100
#define VID_YPbPr1                    0x00000200
#define VID_YPbPr2                    0x00000400
#define VID_YPbPr3                    0x00000800
#define VID_SDI0                      0x00001000
#define VID_SDI1                      0x00002000
#define VID_SDI2                      0x00004000
#define VID_SDI3                      0x00008000
#define VID_C0                        0x00010000
#define VID_C1                        0x00020000
#define VID_C2                        0x00040000
#define VID_C3                        0x00080000
#define VID_S0                        0x00100000
#define VID_DVI0                      0x01000000
#define VID_DVI1                      0x02000000
#define VID_DVI2                      0x04000000
#define VID_DVI3                      0x08000000
#define VID_CAMERA0               0x10000000

/* 接口标志 */
#define VID_DIV2                      0x40000000  /* 需要做降帧处理时输入接口需要或上该宏 */
#define VID_DBL2                      0x40000000  /* 需要做升帧处理时输出接口需要或上该宏 */

/* 播放设备的VP口输出，如DSP的视频播放VP口 */
#define VID_PLAY0_VP0                 0x80000000   /* h900中，8168内置vout0输出口 */
#define VID_PLAY0_VP1                 0x80000001   /* h900中，8168内置HDMI输出口 */
#define VID_PLAY0_VP2                 0x80000002
#define VID_PLAY0_VP3                 0x80000003
#define VID_PLAY0_VP4                 0x80000004

#define VID_PLAY1_VP0                 0x80010000
#define VID_PLAY1_VP1                 0x80010001
#define VID_PLAY1_VP2                 0x80010002
#define VID_PLAY1_VP3                 0x80010003
#define VID_PLAY1_VP4                 0x80010004

#define VID_PLAY2_VP0                 0x80020000
#define VID_PLAY2_VP1                 0x80020001
#define VID_PLAY2_VP2                 0x80020002
#define VID_PLAY2_VP3                 0x80020003
#define VID_PLAY2_VP4                 0x80020004

#define VID_PLAY3_VP0                 0x80030000
#define VID_PLAY3_VP1                 0x80030001
#define VID_PLAY3_VP2                 0x80030002
#define VID_PLAY3_VP3                 0x80030003
#define VID_PLAY3_VP4                 0x80030004

/* 解码器出来的VP口 */
#define VID_DEC0_VP0                  0x80100000
#define VID_DEC0_VP1                  0x80100001
#define VID_DEC0_VP2                  0x80100002
#define VID_DEC0_VP3                  0x80100003
#define VID_DEC0_VP4                  0x80100004
#define VID_DEC0_VP5                  0x80100005
#define VID_DEC0_VP6                  0x80100006
#define VID_DEC0_VP7                  0x80100007

#define VID_DEC1_VP0                  0x80110000
#define VID_DEC1_VP1                  0x80110001
#define VID_DEC1_VP2                  0x80110002
#define VID_DEC1_VP3                  0x80110003
#define VID_DEC1_VP4                  0x80110004
#define VID_DEC1_VP5                  0x80110005
#define VID_DEC1_VP6                  0x80110006
#define VID_DEC1_VP7                  0x80110007

#define VID_DEC2_VP0                  0x80120000
#define VID_DEC2_VP1                  0x80120001
#define VID_DEC2_VP2                  0x80120002
#define VID_DEC2_VP3                  0x80120003
#define VID_DEC2_VP4                  0x80120004
#define VID_DEC2_VP5                  0x80120005
#define VID_DEC2_VP6                  0x80120006
#define VID_DEC2_VP7                  0x80120007

#define VID_DEC3_VP0                  0x80130000
#define VID_DEC3_VP1                  0x80130001
#define VID_DEC3_VP2                  0x80130002
#define VID_DEC3_VP3                  0x80130003
#define VID_DEC3_VP4                  0x80130004
#define VID_DEC3_VP5                  0x80130005
#define VID_DEC3_VP6                  0x80130006
#define VID_DEC3_VP7                  0x80130007

/* 进行OSD叠加后的VP口 */
#define VID_FPGAOSD_VP0               0x80200000    /* 至于哪些VP口参与合成视板子而定 */

/* Resizer后的VP口 */
#define VID_RESIZER_VP0               0x80300000

/* TITLE字幕叠加后的VP口 */
#define VID_TITLE_VP0                 0x80400000

/* 视频采集芯片索引 */
#define VID_CAP_VP_DSP                0x00000000    /* DSP视频采集芯片，DSP包含DM6437 DM647
                                                       如果同一块板子上同时存在多种DSP的话请在此特殊说明
                                                       每一片DSP的索引值，如果是单类型DSP的话都从0开始编号 */
#define VID_CAP_VP_ENC                0x00010000    /* 编码器视频采集芯片，编码器包含FPGA_ENCODER, TAOS;
                                                       如果同一块板子上同时存在多种编码器的话请在此特殊说明
                                                       每一片编码器的索引值，如果是单类型编码器的话都从0开始编号*/

#define VID_CAP_VP_DM8168           0x00020000    /* DM8168 */

#define VID_CAP_VP_DSP0               (VID_CAP_VP_DSP | 0x00) /* h900中，8168芯片号 */
#define VID_CAP_VP_DSP1               (VID_CAP_VP_DSP | 0x01)
#define VID_CAP_VP_DSP2               (VID_CAP_VP_DSP | 0x02)
#define VID_CAP_VP_DSP3               (VID_CAP_VP_DSP | 0x03)

#define VID_CAP_VP_ENC0               (VID_CAP_VP_ENC | 0x00)
#define VID_CAP_VP_ENC1               (VID_CAP_VP_ENC | 0x01)
#define VID_CAP_VP_ENC2               (VID_CAP_VP_ENC | 0x02)
#define VID_CAP_VP_ENC3               (VID_CAP_VP_ENC | 0x03)


/* 视频输入输出设备控制类型宏定义 */
#define VID_CTRL_SET_BRIGHTNESS         1   /* 设置亮度 */
#define VID_CTRL_SET_CONTRAST           2   /* 设置对比度 */
#define VID_CTRL_SET_HUE                3   /* 设置色度 */
#define VID_CTRL_SET_SATURATION         4   /* 设置饱和度 */
#define VID_CTRL_SET_STD                5   /* 设置视频格式，arg参数为：TVidStd */
#define VID_CTRL_GET_STD                6   /* 查询视频格式，arg参数为：TVidStd */
#define VID_CTRL_GET_INTF_TYPE          7   /* 查询复用接口当前的类型, 源必须开电并连接着相应的接口 */
#define VID_CTRL_SET_SHARPNESS          8   /* 设置锐度 */
#define VID_CTRL_SET_NOISEFILT          9   /* 设置降噪系数 */
                                            /* 以下OSD的命令dwInterface = VID_FPGAOSD_VP0 */
#define VID_CTRL_SET_OSD                10  /* 设置接口FPGA的OSD模块，arg参数为：TVidOsdParam */
#define VID_CTRL_START_OSD              11  /* 启动OSD模块合成输出，arg参数为：NULL */
#define VID_CTRL_STOP_OSD               12  /* 停止OSD模块合成输出，arg参数为：NULL */

                                            /* 以下Resizer的命令dwInterface = VID_RESIZER_VP0 */
#define VID_CTRL_SET_RESIZER            13  /* 设置接口FPGA的OSD模块，arg参数为：TVidResizeParam */
#define VID_CTRL_START_RESIZER          14  /* 启动RESIZER模块输出，arg参数为：NULL */
#define VID_CTRL_STOP_RESIZER           15  /* 停止RESIZER模块输出，arg参数为：NULL */

#define VID_CTRL_LOAD_TITLE_PIC         16  /* 加载字幕图片数据，arg参数为：TVidTitlePicLoad */
#define VID_CTRL_START_TITLE            17  /* 开始字幕叠加，arg参数为：TVidTitlePicDraw */
#define VID_CTRL_STOP_TITLE             18  /* 关闭字幕叠加，arg参数为：(u32 *)&dwPicId */
#define VID_CTRL_GET_TITLE_STAT         23  /* 查询字幕显示状态，arg参数为：TVidTitleStat */

#define VID_CTRL_SET_CLK_PHASE          19  /* 设置VGA输入模式时时钟相位，arg参数为：(u32 *)
                                               相位值,范围0-31；效果：增加清晰度，改善字体抖动，
                                               纠正色反问题 */
#define VID_CTRL_GET_CLK_PHASE          20  /* 查询VGA输入模式时时钟相位，arg参数为：(u32 *) */
#define VID_CTRL_SET_FILT               21  /* 设置图像滤波，arg参数为：(u32 *)
                                               0=关闭 非0=打开；打开后图像整体会变模糊一些，
                                               但是可以进一步改善字体抖动问题；关闭后图像清晰度最佳 */
#define VID_CTRL_GET_FILT               22  /* 查询图像滤波，arg参数为：(u32 *) */
/*#define VID_CTRL_GET_TITLE_STAT       23     查询字幕显示状态，arg参数为：TVidTitleStat */

#define VID_CTRL_GET_TV_INFO            24  /* 查询输出口连接的TV信息，arg参数为: TVidOutTVInfo */

#define VID_CTRL_INIT_VIDEO_INFO        25  /* initiate video configure to fpga */
#define VID_CTRL_SET_MUXOUT_MODE        26  /* 设置视频复用输出模式，arg参数为: TVidInMuxOutput */
#define VID_CTRL_SEL_CAMERA             27 /* Select different camera on different h600 board */
#define VID_CTRL_GET_FOCUS              28  /* 获取摄像头对焦 */
#define VID_CTRL_SET_FOCUS              29  /* 设置摄像头对焦 */
#define VID_CTRL_SET_BACKLIGHT          30  /* 设置摄像头背光补偿 */
#define VID_CTRL_SET_ZOOM               31  /* 设置摄像头缩放 */
#define VID_CTRL_GET_ZOOM               32  /* 获取摄像头缩放 */
#define VID_CTRL_SET_DENOISE            33  /* 设置摄像头去噪 */
#define VID_CTRL_SET_POWER              34  /* 控制摄像头电源开启或者关闭 */
#define VID_CTRL_CAN_FPGA_DO_TITILE_BANNER 35 /* 检测FPGA能否进行台标横幅叠加 */
#define VID_CTRL_SET_ISO                36  /* 设置机芯ISO */
#define VID_CTRL_GET_ISO                37  /* 获取机芯ISO */
#define VID_CTRL_SET_WHITE_BALANCE      38  /* 设置白平衡 */
#define VID_CTRL_GET_WHITE_BALANCE      39  /* 获取白平衡 */
#define VID_CTRL_SET_SCENE_MODE         40  /* 设置场景模式 */
#define VID_CTRL_GET_SCENE_MODE         41  /* 获取场景模式 */
#define VID_CTRL_SET_EXPOSURE_MODE      42  /* 设置曝光模式 */
#define VID_CTRL_GET_EXPOSURE_MODE      43  /* 获取曝光模式 */

/* 摄像头曝光模式控制 */
#define VID_EXP_MODE_MANUAL                0    /* 手动曝光 */
#define VID_EXP_MODE_PROGRAMMED_AE         1    /* 程式自动曝光 */
#define VID_EXP_MODE_APERTURE_AE           2    /* 光圈优先自动曝光 */
#define VID_EXP_MODE_SHUTTER_AE            3    /* 快门优先自动曝光 */


/* 摄像头场景模式定义 */
#define VID_SCENE_MODE_OFF                    0           /* 场景模式关闭 */
#define VID_SCENE_MODE_NIGHT_AUTO_CHANGE      1           /* 夜间 */
#define VID_SCENE_MODE_NIGHT_NO_AUTO_CHANGE   2           /* 夜间 */
#define VID_SCENE_MODE_AUTO                   3           /* 自动 */
#define VID_SCENE_MODE_WDR                    4           /* 宽动态 */
#define VID_SCENE_MODE_HI_COLOR_TMP           5           /* 高色温 */

/* 摄像头ISO设定 */
#define VID_ISO_MODE_AUTO             0
#define VID_ISO_MODE_MANUAL           1
#define VID_ISO_MKCMD(mode, val)      \
                    (((mode) & 0xffff) | (((val) & 0xffff) << 16))
#define VID_ISO_GET_MODE(cmd)         ((cmd) & 0xffff)
#define VID_ISO_GET_VAL(cmd)          (((cmd) >> 16) & 0xffff)

/* 摄像头白平衡设定 */
#define VID_WBA_MODE_AUTO             0       /* 自动 */
#define VID_WBA_MODE_DAY              1       /* 白天 */
#define VID_WBA_MODE_CLOUDY           2       /* 多云 */
#define VID_WBA_MODE_SHADE            3       /* 阴天 */
#define VID_WBA_MODE_FLUORESCENT_W    4       /* 荧光灯 */
#define VID_WBA_MODE_FLUORESCENT_N    5
#define VID_WBA_MODE_FLUORESCENT_D    6
#define VID_WBA_MODE_TUNGSTEN         7       /* 白炽灯 */




#define VID_CTRL_DBG_WR_REG             100 /* 调试手段，直接写指定寄存器 */
#define VID_CTRL_DBG_RD_REG             101 /* 调试手段，直接读指定寄存器 */

/* 视频格式掩码定义 */
#define VID_STD_MASK_EMBSYNC            0x80000000  /* 同步模式掩码，1=集成同步，0=外同步 */
#define VID_STD_MASK_SD_16BIT           0x40000000  /* 标清分辨率采用16位格式传输标志，针对576i/480i */
#define VID_STD_MASK_COLORSPACE         0x000000ff  /* 色度空间掩码，占用低8位 */
#define VID_COLORSPACE_YUV422           0           /* YUV422格式 */
#define VID_COLORSPACE_RGB888           1           /* RGB888格式 */
#define VID_COLORSPACE_RGB444           2           /* RGB444格式 */
#define VID_COLORSPACE_RAW16BIT         3           /* 16位裸数据格式 */

/* 音频输入输出接口类型宏定义 */
#define AUDIN_35                       0x00000040
#define AUDOUT_35                     0x00000080


/* 音频输入接口类型宏定义 */
#define AUD_NONE                       0x00000000
#define AUD_HDMI0                     0x00000001
#define AUD_HDMI1                     0x00000002
#define AUD_HDMI2                     0x00000004
#define AUD_HDMI3                     0x00000008
#define AUD_HDMI4                     0x00000080
#define AUD_LINE0                      0x00000010  /* RCA & 3.5/6.5立体声接口 */
#define AUD_LINE1                      0x00000020
#define AUD_LINE2                      0x00000040  /* 复用成6.35接口 */
#define AUD_MIC0                       0x00000100  /* 卡农&麦克风等接口 */
#define AUD_MIC1                       0x00000200
#define AUD_DMIC0                     0x00000400  /* 数字mic */
#define AUD_DMIC1                     0x00000800
#define AUD_FXO0                       0x00001000  /* 电话 */
#define AUD_SPEAKER0               0x00002000  /* 扬声器 */
#define AUD_SDI0                       0x00004000
#define AUD_SDI1                       0x00008000

/* digital mic detect: macro definition */
#define AUDIN_DIGIMIC_PLUGIN   0x1
#define AUDIN_DIGIMIC_UNPLUGIN 0x0

/* DSP设备的音频口定义，最高位D31为1区别于物理音频接口 */
#define AUDIN_DSP0_ASP0_S0              0x80000000  /*  */
#define AUDIN_DSP0_ASP0_S1              0x80000001
#define AUDIN_DSP0_ASP0_S3              0x80000003

/* DSP设备源切换宏定义说明
 * C6747_FPGA_AXR0 				AUDIN_DSPx_ASPi_Sn(1, 1, 0)
 * C6747_FPGA_AXR1 				AUDIN_DSPx_ASPi_Sn(1, 1, 1)
 * C6747_FPGA_AXR2 				AUDIN_DSPx_ASPi_Sn(1, 1, 2)
 * C6747_FPGA_AXR3 				AUDIN_DSPx_ASPi_Sn(1, 1, 3)
 * C6747_FPGA_AXR4 				AUDIN_DSPx_ASPi_Sn(1, 1, 4)
 * C6747_FPGA_AXR5 				AUDIN_DSPx_ASPi_Sn(1, 1, 5)
 * C6747_FPGA_AXR6 				AUDIN_DSPx_ASPi_Sn(1, 0, 13)
 * C6747_FPGA_AXR7 				AUDIN_DSPx_ASPi_Sn(1, 0, 12)
 * C6747_FPGA_AXR8 				AUDIN_DSPx_ASPi_Sn(1, 0, 10)
 */
#define AUDIN_DSPx_ASPi_Sn(x, i, n)     (0x80000000 | ((x&0xff)<<16) | ((i&0xff)<<8) | ((n&0xff)<<0))


/* 音频输入输出设备控制类型宏定义 */
#define AUD_CTRL_SET_STD                1   /* 设置音频格式，arg参数为：(TAudStd *) */
#define AUD_CTRL_GET_STD                2   /* 查询音频格式，arg参数为：(TAudStd *) */
#define AUD_CTRL_SET_MIC_VOL            3   /* 设置MIC音量， arg参数为：(int *) */
#define AUD_CTRL_GET_MIC_VOL            4   /* 查询MIC音量， arg参数为：(int *) */
#define AUD_CTRL_SEL_INTF               5   /* 选择音频接口，arg参数为：(int *)on=1 off=0 */
#define AUD_CTRL_SET_MIC_48V            6   /* 设置MIC的48V供电开关，arg参数为：(int *)on=1 off=0 */
#define AUD_CTRL_SET_LEFT_VOL           7   /* 设置左声道音量, arg参数为：(int *)0~100% */
#define AUD_CTRL_SET_RIGHT_VOL          8   /* 设置右声道音量, arg参数为：(int *)0~100% */
#define AUD_CTRL_GET_LEFT_VOL           9   /* 查询左声道音量, arg参数为：(int *)0~100% */
#define AUD_CTRL_GET_RIGHT_VOL          10   /* 查询右声道音量, arg参数为：(int *)0~100% */
#define AUD_CTRL_GET_DIGIMIC_STATUS 11
#define AUD_CTRL_GET_35IN_STATUS       12
#define AUD_CTRL_GET_35OUT_STATUS     13
#define AUD_CTRL_GET_635OUT_STATUS     14
#define AUD_CTRL_SEL_FPGA_INTF		   15	/* 切换h900FPGA音频接口 */

/* 扩展模块宏定义 */
#define EXT_MODULE_E1                   0   /* 外挂模块为: 单E1模块   */
#define EXT_MODULE_4E1                  1   /* 外挂模块为: 4E1模块    */
#define EXT_MODULE_V35DTE               2   /* 外挂模块为: V35DTE模块 */
#define EXT_MODULE_V35DCE               3   /* 外挂模块为: V35DCE模块 */
#define EXT_MODULE_BRIDGE               4   /* 外挂模块为: 网桥模块   */
#define EXT_MODULE_2E1                  5   /* 外挂模块为: 2E1模块    */
#define EXT_MODULE_KDV8000B             6   /* 外挂模块为: KDV8000B模块    */
#define EXT_MODULE_DSC                  0xc /* 外挂模块为: DSC(同KDV8000B)模块 */
#define EXT_MODULE_MDSC                 0xd /* 外挂模块为: MDSC*/
#define EXT_MODULE_HDSC                 0xe /* 外挂模块为: HDSC */
#define EXT_NO_MODULE                   0xf /* 没有外挂模块  */


/* -----------------------------以下是兼容老设备的一些宏定义---------------------- */
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
#define KDV8010A_MODULE_MDSC        0xd      /* 外挂模块为: MDSC*/
#define KDV8010A_MODULE_HDSC        0xe      /* 外挂模块为: HDSC */
#define KDV8010A_NO_MODULE          0xf      /* 没有外挂模块  */


/*串口相关的宏定义*/
/* BrdOpenSerial函数打开串口类型宏定义 */
#define BRD_SERIAL_RS232             0
#define BRD_SERIAL_RS422             1
#define BRD_SERIAL_RS485             2
#define BRD_SERIAL_INFRARED          3

/*T300对应的端口号*/
#define BRD_SERIAL_RS422_0	   4	
#define BRD_SERIAL_RS422_1            5	
#define BRD_SERIAL_RS422_2            6
#define BRD_SERIAL_232_422_485_0	7
#define BRD_SERIAL_232_422_485_1      8	
#define BRD_SERIAL_232_422_485_2      9	
#define BRD_SERIAL_232_422_485_3      10	
#define BRD_SERIAL_232_422_485_4      11	
#define BRD_SERIAL_232_422_485_5      12

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


/*KDV7810相关的宏定义*/
/* 模块宏定义 */
#define KDV7810_MODULE_E1          0        /* 外挂模块为: 单E1模块   */
#define KDV7810_MODULE_4E1         1        /* 外挂模块为: 4E1模块    */
#define KDV7810_MODULE_2E1         5        /* 外挂模块为: 2E1模块    */
#define KDV7810_NO_MODULE          0xf      /* 没有外挂模块  */

/* E2PROM 输出定义 */
#define EEPROM_LOG_OUTPUT_NULL  0x00
#define EEPROM_LOG_OUTPUT_PAL   0x02
#define EEPROM_LOG_OUTPUT_NTSC  0x04
#define EEPROM_LOG_OUTPUT_VGA   0x06

/* 网同步时钟信号输出控制 */
#define E1_CLK_OUTPUT_DISABLE   0     /* 高阻禁止单板上参考时钟信号输出到MPC板上 */
#define E1_CLK_OUTPUT_ENABLE    1     /* 允许单板板上参考时钟信号输出到MPC板上 */

#define BCSR_OPT_GET_E1ISNEW	     (BCSR_OPT_BASE + 0x33)  /*判断是否是新E1模组DS26514*/
#define BCSR_OPT_GET_E1ALMDTL	     (BCSR_OPT_BASE + 0x34)  /*获取E1告警细节*/
/*老E1模块告警位掩码宏定义*/
#define E1_ALARM_RUA1_OLD           0x08
#define E1_ALARM_RRA_OLD            0x04
#define E1_ALARM_RCL_OLD            0x02
#define E1_ALARM_RLOS_OLD           0x01
/*新E1模块告警位掩码宏定义*/
#define E1_ALARM_RRAI_MASK          0x08
#define E1_ALARM_RAIS_MASK          0x04
#define E1_ALARM_RLOS_MASK          0x02
#define E1_ALARM_RLOF_MASK          0x01

#define BCSR_OPT_SWITCH_PORT		(BCSR_OPT_BASE + 0x29)
#define BCSR_OPT_LCP2368_RESET                  (BCSR_OPT_BASE + 0x2a)


//FOR MPU2
#define BCSR_OPT_GET_ECARD_INFO		(BCSR_OPT_BASE + 0x28)

//for kdv8000i apu2
#define BCSR_CDCE62005_CLOCK_CONFIG 				(BCSR_OPT_BASE + 0x35)

//For t300
#define BCSR_LOGIC_IN 									(BCSR_OPT_BASE + 0x36)
#define BCSR_LOGIC_OUT								(BCSR_OPT_BASE + 0x37)

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __BRD_WRAPPER_DEF_H */
