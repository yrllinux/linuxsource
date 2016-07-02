#ifndef SM_PUBLIC_MACRO_H_
#define SM_PUBLIC_MACRO_H_

#include "kdvtype.h"
#include "kdvdef.h"
#include "osp.h"
#include "mtsnmp.h"
#include "vccommon.h"
#include "errorid.h"

/////////////////////////////////////////////////////Linux 下异常捕获处理
#ifdef _LINUX_

#include <stdio.h>
#include <stdlib.h>
#include <elf.h>
#include <string.h>
#include <sys/types.h>
#endif

#ifdef WIN32
#include "time.h"
#endif


//Session 模块App ID定义
#define CSESSIONMANAGER_APPID       AID_SVR_BGN
#define CCMAPP_APPID                (AID_SVR_BGN + 1)		
#define CFMAPP_APPID                (AID_SVR_BGN + 2)

#define CRITICAL        1
#define IMPORTANT       2
#define APPLICATION     196
#define USERDEBUG       255

//前后台链路状况
#define	LINK_DOWN       0
#define LINK_ACTIVE     1
#define LINK_UNKNOWN    255

#define INVALID_IPADDR            (u32)0xffffffff

//数据库名
const u8 DB_NAME_MAX_LEN = 16;

//用户及密码的长度
const u8 USER_LEN = 17;

const u8 PWD_LEN = USER_LEN;
//指示灯状态长度
//const u8 LED_STATUS_LEN = 24;

//时间字符串的长度
const u8 TIME_LEN = 20;

//IP地址字符串的长度
const u8 IPADDR_LEN = 16;

//设备版本长度
const u8 DEVICE_VER_LEN = 20;

//DB连接超时（秒）
const u8 MAX_DBCONNECT_TIME = 30;

//软件版本长度
const u8 MAX_SOFTWARE_VER_LEN = 36;

//编译时间长度
const u8 MAX_COMPILE_TIME_LEN = 36;

//灯的状态程度
const u8 MAX_LED_STATUS_LEN = 64;

//会议名长度
const u8 MAX_CONFNAME_LEN = 32;

const u8 MAX_CONFID_LEN = MAX_CONFNAME_LEN;

//设备类型名称长度
const u8 SM_DEVICE_TYPENAME_LENGTH = 16;

//mcu机箱中最大的槽位数
const u8 SM_MCU_LAYER_SLOT_MAX = 64;

//最大mcu个数
const u8 SM_MCU_MANA_MAX = 200;

//单板buffer最大值
const u16 SM_BOARD_BUFFER_MAX = SM_MCU_MANA_MAX * SM_MCU_LAYER_SLOT_MAX;

//函数名最长长度
const u8 SM_MAX_PROC_NAME_LEN = 32;

//设备类型字符串长度
const u8 SM_DEVTYPE_NAME_LEN  = 31;

//别名长度
const u8 SM_MAX_ALIAS_LEN  = MAXLEN_ALIAS;

//164号长度
const u8 SM_MAX_E164_LEN  = MAXLEN_E164;

//外设使用率统计buffer长度
const u8 SM_MAX_EQPNODEBUFF_LEN  = 128;

//lukp[02/08/2010] 添加一个外设类型最大值，以后若再添加外设，此值需要增加
const u16 SM_MAX_EQP_TYPE = 12; //外设类型最大值

const u16 SM_MAX_UPDATE_LEN = 4 * 1024;

const u16 SM_MAX_UPDATE_FILENUM = 2;

const u16 SM_MAX_UPDATE_BUFFER = 100;

const u16 SM_MAX_UPDATEBOARD_BUFFER = 1000;

const u16 SM_MAX_SUFF_LEN = 10;

const u8 PERI_STATE_LEN         = 4;    //MCU外设设备状态长度

//实例名最长长度
const u8 SM_MAX_INS_NAME_LEN = SM_MAX_PROC_NAME_LEN;

//文件升级定时查询间隔
#define SM_UPDATE_QUERY_TIME       (u32)20 * 1000

//错误码
#define ERROR_CMD_SUCCESS               0	// 交易成功
#define ERROR_DEFAULT                   ERR_SVR_BGN + 1     //缺省错误
#define ERROR_SVR_RESTARTING            ERR_SVR_BGN + 2     //服务器正在启动中
#define ERROR_DB_UNKNOWD                ERR_SVR_BGN + 3     //数据库出错
#define ERROR_CMD_IN_RUNNING            ERR_SVR_BGN + 4     //操作正在进行中

#define	ERROR_NEED_MORE_PARAM           ERR_SVR_BGN + 5     //参数不全
#define ERROR_SM_BUSY					ERR_SVR_BGN + 6     //snmpmanager忙

#define ERROR_CMD_TIMEOUT               ERR_SVR_BGN + 10    //SessionManager超时
#define ERROR_CMD_TYPE_MISMATCHED       ERR_SVR_BGN + 11    //命令类型不匹配

#define	ERROR_NO_FREEINSTANCE           ERR_SVR_BGN + 20    //CM/FM应用没有可用的实例
#define	ERROR_CLIENT_MISMATCHED         ERR_SVR_BGN + 21    //客户端不匹配

#define	ERROR_FM_GETALARMSTAMP_FAIL     ERR_SVR_BGN + 30    //获取告警信息失败
#define	ERROR_FM_GETALARMSTAMP_TIMEOUT  ERR_SVR_BGN + 31    //获取告警信息超时

#define	ERROR_FM_GETALARM_FAIL          ERR_SVR_BGN + 32    //获取告警失败
#define	ERROR_FM_GETALARM_TIMEOUT       ERR_SVR_BGN + 33    //获取告警超时

#define ERROR_CM_RESTART_UPDATING       ERR_SVR_BGN + 34    //设备文件升级中，不允许重起

#define ERROR_SNMP_SEND_FAIL            ERR_SVR_BGN + 40    //发送SNMP命令失败
#define ERROR_SNMP_RCV_FAIL             ERR_SVR_BGN + 41    //发送SNMP命令失败

#define ERROR_FTP_RUNNING               ERR_SVR_BGN + 60    //Ftp操作正在执行
#define ERROR_FTP_DELETEFILE_FAIL       ERR_SVR_BGN + 61    //FTP删除文件失败
#define ERROR_FTP_PUTFILE_FAIL          ERR_SVR_BGN + 62    //FTP上载文件失败
#define ERROR_FTP_GETFILE_FAIL          ERR_SVR_BGN + 63    //FTP获取文件失败

#define ERROR_EQUIPMENT_MISMATCHED      ERR_SVR_BGN + 70    //设备类型不匹配

#define ERROR_LOGIN                     ERR_SVR_BGN + 80    //登录失败
#define ERROR_LOGIN_NO_USER             ERR_SVR_BGN + 81    //没有这个用户
#define ERROR_LOGIN_ERR_PWD             ERR_SVR_BGN + 82    //用户密码错误
#define ERROR_LOGIN_NO_ROLE             ERR_SVR_BGN + 83    //用户不从属于角色
#define ERROR_CHANGE_PWD                ERR_SVR_BGN + 84    //修改密码失败
#define ERROR_PWD_MISMATCHED            ERR_SVR_BGN + 85    //密码不符合
#define ERROR_USER_HAS_LOGON            ERR_SVR_BGN + 86    //用户已经登录				

//单板
#define ERROR_SET_BOARD_STATUS_FAILED       ERR_SVR_BGN + 100   //设置单板状态失败
#define ERROR_SYNC_BOARD_TIME_FAILED        ERR_SVR_BGN + 101   //同步单板时间失败
#define ERROR_BOARD_BITERROR_TEST_FAILED    ERR_SVR_BGN + 102   //误码测试失败
#define ERROR_BOARD_SELF_TEST_FAILED        ERR_SVR_BGN + 103   //自测失败
#define ERROR_BOARD_E1LINK_CFG_FAILED       ERR_SVR_BGN + 104   //E1链路设置失败



#define	DEVICE_LINK_DOWN            (u16)999
#define DEVICE_POWEROFF             (u16)998
#define DEVICE_RESTART              (u16)1000

//对象类型
#define OBJECT_TYPE_UNKNOWN     (u8)0   // 未知的设备
#define OBJECT_TYPE_MCU         (u8)0x1 // MCU设备
#define OBJECT_TYPE_MT          (u8)0x2 // 终端设备
#define OBJECT_TYPE_SM			(u8)0x3	// Snmp Manager也作为一个设备
#define OBJECT_TYPE_PERI        (u8)0x4 // 外设设备

//Mcu 分类
#define MCU_CATALOG_UNKNOWN     0x0 //未知Mcu
#define MCU_CATALOG_8000        0x1 //Mcu8000
#define MCU_CATALOG_8000B       0x2 //Mcu8000B
#define MCU_CATALOG_8000C       0x3 //MCU 8000C

// 终端分类
#define MT_CATALOG_UNKNOWN				MTSYSHARDWARETYPE_UNKNOWNMTMODEL	// 未知终端
#define MT_CATALOG_PC					MTSYSHARDWARETYPE_PCMT				// 桌面终端
#define MT_CATALOG_EMBED_MT8010			MTSYSHARDWARETYPE_8010				// MT8010终端
#define MT_CATALOG_EMBED_MT8010C		MTSYSHARDWARETYPE_8010C				// MT8010C终端
#define MT_CATALOG_EMBED_MT8010C1		MTSYSHARDWARETYPE_8010C1			// MT8010C终端
#define MT_CATALOG_EMBED_IMT			MTSYSHARDWARETYPE_IMT				// 嵌入终端IMT
#define MT_CATALOG_EMBED_MT8010A		MTSYSHARDWARETYPE_8010A				// MT8010A终端
#define MT_CATALOG_EMBED_MT8010APLUS    MTSYSHARDWARETYPE_8010APLUS			// MT8010A+终端
#define MT_CATALOG_EMBED_MT6610			MTSYSHARDWARETYPE_6610				// TS6610
#define MT_CATALOG_EMBED_MT6610E		MTSYSHARDWARETYPE_6610E				// TS6610E
#define MT_CATALOG_EMBED_MT6210			MTSYSHARDWARETYPE_6210				// TS6210
#define MT_CATALOG_EMBED_MT5210			MTSYSHARDWARETYPE_5210				// TS5210
#define MT_CATALOG_EMBED_MT3210			MTSYSHARDWARETYPE_3210				// TS3210
#define MT_CATALOG_EMBED_MTV5			MTSYSHARDWARETYPE_V5				// TS V5
#define MT_CATALOG_EMBED_MT8010A_2		MTSYSHARDWARETYPE_8010A_2			// 8010A 最大2M
#define MT_CATALOG_EMBED_MT8010A_4		MTSYSHARDWARETYPE_8010A_4			// 8010A 最大4M
#define MT_CATALOG_EMBED_MT8010A_8		MTSYSHARDWARETYPE_8010A_8			// 8010A 最大8M
#define MT_CATALOG_EMBED_MT7210			MTSYSHARDWARETYPE_7210				// TS7210
#define MT_CATALOG_EMBED_MT7610			MTSYSHARDWARETYPE_7610				// TS7610
#define MT_CATALOG_EMBED_MT7620_B		MTSYSHARDWARETYPE_7620_2			// TS7620_B
#define MT_CATALOG_EMBED_MT7620_A		MTSYSHARDWARETYPE_7620_4			// TS7620_A
#define MT_CATALOG_EMBED_MT5610         MTSYSHARDWARETYPE_5610              // MT5610终端
#define MT_CATALOG_EMBED_MT7810			MTSYSHARDWARETYPE_7810				// TS7810
#define MT_CATALOG_EMBED_MT7910			MTSYSHARDWARETYPE_7910				// TS7910
#define MT_CATALOG_EMBED_MT7820_A		MTSYSHARDWARETYPE_7820_A			// MT7820_A
#define MT_CATALOG_EMBED_MT7820_B		MTSYSHARDWARETYPE_7820_B			// MT7820_B
#define MT_CATALOG_EMBED_MT7920_A		MTSYSHARDWARETYPE_7920_A			// MT7920_A
#define MT_CATALOG_EMBED_MT7920_B		MTSYSHARDWARETYPE_7920_B			// MT7920_B


#endif
