/********************************************************************
	模块名:      NMCLib
	文件名:      nmclibdef.h
	相关文件:    
	文件实现功能 NMCLib中错误码定义，消息定义，结构定义。
    作者：       朱允荣
	版本：       3.5
------------------------------------------------------------------------
	修改记录:
	日  期		版本		修改人		走读人    修改内容
	2004/08/03	3.5			朱允荣                  创建

*********************************************************************/
#pragma once

#include "kdvType.h"		// 公共类型
#include "kdvDef.h"			// 公共宏
#include "errorid.h"

// 定义错误码
#define ERR_NMC_AFFAIR_BGN		ERR_CLT_BGN		// 功能库错误码开始 = 10001~12000
enum
{
	No_Error = 0,
	Err_Success_With_Info	= ERR_CLT_END,				// 12000
	Err_Env_Already = ERR_NMC_AFFAIR_BGN + 0,			// 10001--环境已经准备好
	Err_Env_Not_Prepare,								// 10002--环境还没有准备好

	Err_Logon_Already = ERR_NMC_AFFAIR_BGN + 50,		// 10051--已经处于登录状态
	Err_Logoff_Already,									// 10052--已经处于注销状态
	Err_Logon_Not,										// 10053--功能库还没有登录
	Err_Logon_User_Pwd,									// 10054--用户密码错误

	Err_Event_Timeout = ERR_NMC_AFFAIR_BGN + 100,		// 10101--操作超时了
	
	Err_Osp_Create = ERR_NMC_AFFAIR_BGN + 150,			// 10151--连接服务器初始化失败
	Err_Osp_Connect,									// 10152--服务器连接不成功
	Err_Osp_ReSendCommad,								// 10153--已有消息在等待回应。
	Err_Osp_NoRevHandle,								// 10154--消息接收窗口未创建。
	
	Err_Db_Odbc_Create = ERR_NMC_AFFAIR_BGN + 200,		// 10201--odbc创建失败
	Err_Db_Odbc_Stmt,									// 10202--数据库语句句柄创建失败
	Err_Db_Odbc_Exec,									// 10203--直接执行失败
	Err_Db_Odbc_Produce,								// 10204--存储过程失败
	Err_Db_Odbc_Env,									// 10205--环境
	Err_Db_Odbc_Fetch,									// 10206--游标错误

	Err_Db_Odbc_Filter_Exists,							// 10207--设置的告警码已存在
	
	Err_Log_Read_End = ERR_NMC_AFFAIR_BGN + 300,		// 10301--日志已经读取完毕
	Err_Log_Type_Match,									// 10302--日志类型不匹配
	
	Err_Param_Invalid = ERR_NMC_AFFAIR_BGN + 400,		// 10401--参数不合法
	
	Err_Device_Exist_Not = ERR_NMC_AFFAIR_BGN + 500,	// 10501--设备不存在
	Err_Device_Type,									// 10502--设备类型非法
	Err_Device_Ip_Already,								// 10503--已经存在该地址的设备了
	Err_Mt_Catalog,										// 10504--终端分类非法
	
	Err_User_Exist_Not = ERR_NMC_AFFAIR_BGN + 600,		// 10601--用于不存在
	Err_User_Exist_Already,								// 10602--用户已经存在
	Err_User_Privilege,									// 10603--没有权限
	Err_User_Chg_Pwd_Other,								// 10604--不能修改他人口令
	Err_User_Group_Unlink,								// 10605--不能断开根用户和根组的关系
	Err_User_Del_Root,									// 10606--不能删除根用户

	Err_Group_Exist_Already = ERR_NMC_AFFAIR_BGN + 700,	// 10701--该组已经存在
	Err_Group_Exist_Not,								// 10702--该组不存在
	Err_Group_Destroy_Forbid,							// 10703--禁止销毁组
	Err_Group_Priv_Assign_Already,						// 10704--该组已经拥有该权限
	Err_Group_Priv_Assign_Not,							// 10705--该组还没有分配该权限
	Err_Group_User_Own_Already,							// 10706--该组已经拥有该成员
	Err_Group_User_Own_Not,								// 10707--该组还没有拥有该成员
	Err_Group_User_Unassign,							// 10708--不能断开根组和根用户的关系
	Err_Group_Del_Root,									// 10709--不能删除根组
	
	Err_Ftp_File_Exist = ERR_NMC_AFFAIR_BGN + 800,		// 10801--文件传输，文件不存在
	Err_Ftp_File_Type,									// 10802--文件类型错误
	Err_Ftp_File_Name,									// 10803--文件名称错误
	Err_Ftp_Get_Env,									// 10804--文件传输环境没有设置
	Err_Ftp_Break,										// 10805--文件传输被中断
	
	Err_Dir_Exists_Already = ERR_NMC_AFFAIR_BGN + 900,	// 10901--设备目录已经存在
	Err_Dir_Exists_Not,									// 10902--设备目录不存在
	
	Err_Unknown = ~0
};



// 定义长度
enum
{
	Max_OprMask_Len = 127, //最大操作掩码 1111 1111
	Max_Scheme_Len = 200,
    
	Max_Ip_Len = 16,
	Max_Object_Len = 16,

	Max_Date_Len = 16,
	Max_DateTime_Len = 24,
	Max_DateTime_Format_Len = 20,
    
	Max_AlarmValue_Len = 16,
	Max_Pwd_Len = 16,
	Max_Phone_Len = 32,
	Max_Addr_Len = 64,
	Max_Email_Len = 32,
	Max_Remark_Len = 64,
	Max_Serial_Len = 32,

	Max_Obj_Name_Len = 64,
	Max_Logon_Len = 16,
	Max_Opr_Type_Len = 16,
	Max_Opr_Ret_Len = 8,

	Max_Introduction_Len = 200,
	Max_Describing_Len = 200,

	Max_Alias_Len = 32,
	Max_Name_Len = 32,

	Max_Board_Amount = 128                     //最大槽位数
};


	// 定义无效的设备编号
#define INVALID_EQUIP_ID		0

	// 定义权限
#define PRIVILEGE_CM_SYNC_CFG		0x00000001	// 配置同步
#define PRIVILEGE_CM_SYNC_TIME		0x00000002	// 时间同步
#define PRIVILEGE_CM_FTP_DOWNLOAD	0x00000004	// 文件下载
#define PRIVILEGE_CM_FTP_UPLOAD		0x00000008	// 文件上传
#define PRIVILEGE_CM_RPC			0x00000010	// 远程控制
#define PRIVILEGE_FM_SYNC_ALARM		0x00000020	// 告警同步

	
	// 外部消息接口 发送到界面的windows消息
#define WM_NMC_BC_MESSAGE		WM_USER + 0x1000	// 广播
#define WM_NMC_FTP_MESSAGE		WM_USER + 0x1001	// 配置
#define WM_NMC_FM_MESSAGE		WM_USER + 0x1002	// 故障
#define WM_NMC_PM_MESSAGE		WM_USER + 0x1003	// 公共
#define WM_NMC_SM_MESSAGE		WM_USER + 0x1004	// 安全

#define WM_NMC_UPDATE_FAIL      WM_USER + 0x1005

	// 定义使用事件(功能库使用，不和网管服务器交互)
#define __EV_NMC_BGN			128 //EVENT_CTRL_END + 1	// 128开始
	
#define __CM_NMC_COMMON			__EV_NMC_BGN	// 128
#define __CM_DIR_MEMBER_ADD		1	// 设备目录添加成员
#define __CM_DIR_MEMBER_DEL		2	// 设备目录删除成员
#define __CM_DIR_MOVE			3	// 设备目录移动
#define __CM_DIR_CREATE			4	// 设备目录创建
#define __CM_DIR_DESTROY		5	// 设备目录销毁
#define __CM_DIR_RENAME			6	// 设备目录改名
#define __CM_DEVICE_ADD			7	// 设备增加
#define __CM_DEVICE_DEL			8	// 设备删除
#define __CM_DEVICE_ALIAS		9	// 更改设备别名
#define __CM_DEVICE_IP			10	// 更改设备地址
#define __CM_FTP_BREAK			11	// 中断文件传输
#define __CM_FTP_PROGRESS		12	// 文件传输进度
#define __CM_EQUIP_PROPS_UPDATE	13  // 设备属性查询
#define __CM_EQUIP_PROPS_QUERY  14  // 设备属性更新
#define __CM_DIR_MEMBER_POS_ADD		15	//增加或更改数据库设备坐标
#define __CM_DIR_GET_POS	16 //更改界面设备坐标
	
#define __FM_NMC_COMMON			__EV_NMC_BGN + 1	// 129
#define __FM_ALARM_LOG_CUR		1	// 查询当前告警
#define __FM_ALARM_LOG_HIS_Q	2	// 查询历史告警
#define __FM_ALARM_LOG_HIS_D	3	// 删除历史告警
#define __FM_MOCK_QUERY			4	// 查询机架构件
#define __FM_BOARD_QUERY		5	// 查询单板
#define __FM_ALARM_LOG_KONWLEDGE_BASE 6 // 查询告警知识库
#define __FM_ALARM_KNOWLEDGE_BASE_D 7	// 告警知识库删除
#define __FM_ALARM_KNOWLEDGE_BASE_A 8	// 告警知识库增加
#define __FM_ALARM_KNOWLEDGE_BASE_M 9	// 告警知识库修改
#define __FM_ALARM_SET_FILTER	10		// 告警过滤
	
#define __PM_NMC_COMMON			__EV_NMC_BGN + 2	// 130
#define __PM_LOGON				1	// 登录
#define __PM_LOGOFF				2	// 注销
	
#define __SM_NMC_COMMON			__EV_NMC_BGN + 3	// 131
#define __SM_USER_ADD			1		// 增加用户
#define __SM_USER_DEL			2		// 删除用户
#define __SM_USER_MODIFY		3		// 修改用户
#define __SM_USER_QUERY			4		// 查询用户信息
#define __SM_USER_ENUM			5		// 枚举用户
#define __SM_USER_LINK			6		// 用户关联权限组
#define __SM_USER_UNLINK		7		// 用户撤销权限组
#define __SM_USER_GROUP			8		// 查询用户权限组
#define __SM_GROUP_CREATE		9		// 创建组
#define __SM_GROUP_DESTROY		10		// 销毁组
#define __SM_GROUP_MODIFY		11		// 修改组
#define __SM_GROUP_PRIV			12		// 查询组权限
#define __SM_GROUP_ENUM			13		// 枚举组
#define __SM_GROUP_USER			14		// 查询组成员
#define __SM_GROUP_ASSIGN		15		// 添加组成员
#define __SM_GROUP_UNASSIGN		16		// 删除组成员
#define __SM_GROUP_ADD_PRIV		17		// 组添加权限
#define __SM_GROUP_DEL_PRIV		18		// 组删除权限
#define __SM_LOG_DEL			19		// 操作日志删除
#define __SM_LOG_QUERY			20		// 操作日志查询
#define __SM_USER_PRIV_STAT		21		// 统计用户的权限
#define __SM_GROUP_DESC_MODIFY	22		// 修改用户描述

#define __BC_NMC_COMMON			__EV_NMC_BGN + 4	// 132
#define __BC_ALARM				1	// 告警广播
#define __BC_LINK				2	// 链路广播
#define __BC_FM_MOCK			3	// 机架构件状态广播
#define __BC_FM_BOARD			4	// 单板告警广播
#define __BC_DEVICE				5	// 设备修改广播
#define __BC_USER				6	// 用户变更广播
#define __BC_GROUP				7	// 组变更广播

#define __EX_QUERY				__EV_NMC_BGN + 5
#define __EX_CONF_QUERY			1
#define __EX_MT_QUREY			2
#define __EV_NMC_END			__EV_NMC_BGN + 5		// 132结束
	
	
	// 定义告警级别
#define ALARM_LEVEL_NO_ALARM	(~0)	// 没有告警
#define ALARM_LEVEL_LINKDOWN	0		// 断链告警

	// 定义操作对象类型
#define OPR_OBJ_NONE		0
#define OPR_OBJ_MCU			0x0001		// MCU
#define OPR_OBJ_MT			0x0002
#define OPR_OBJ_PERI		0x0004
#define OPR_OBJ_EQP			OPR_OBJ_MCU | OPR_OBJ_MT | OPR_OBJ_PERI
#define OPR_OBJ_USER		0x0100
#define OPR_OBJ_GROUP		0x0200
#define OPR_OBJ_ALARM_HIS	0x0400
#define OPR_OBJ_ALARM_CUR	0x0800
#define OPR_OBJ_LOG			0x1000
#define OPR_OBJ_ALARM_INI	0x2000
#define OPR_OBJ_DIR			0x4000
#define OPR_OBJ_ALARM_KONWLEDGE_BASE	0x8000
#define OPR_OBJ_CONF		0x8000

	// 定义操作结果类型
#define OPR_RET_NONE		(~0)
	// 定义日志结果查询
#define OPR_LOG_RET_ALL		(-1)	// 查询所有
#define OPR_LOG_RET_FAIL	(-2)	// 查询失败的

	// 定义判断操作是否成功
#define NMCAFFAIR_SUCCEEDED(dwErr)	((dwErr) == No_Error)

	//////////////////////////////////////////////////////////////////////////
	// 定义事件在数据库的对应掩码值位置，定义从左边开始
#define OTM_ALL_OPR				128 // all operation
#define OTM_CM_CFG_SYNC			1	// 第01位是配置同步
#define OTM_CM_TIME_SYNC		2	// 第02位是时间同步

#define OTM_FTP_PUT_FILE		3	// 第03位是ftp上传文件
#define OTM_FTP_GET_FILE		4	// 第04个自己是FTP下载文件

#define OTM_RPC_RESTART			5	// 第05位是设备重启动
#define OTM_RPC_STOP			6	// 第06位是设备停止
#define OTM_RPC_SELFTEST		7	// 第07位是设备自检
#define OTM_RPC_RPC_BITERRTEST	8	// 第08位是误码测试
#define OTM_RPC_BOARD_RESTART	9	// 第09位是单板复位
#define OTM_RPC_BOARD_SYMC_TIME	10	// 第10位是单板时间同步
#define OTM_RPC_BOARD_BITERRTEST	11	// 第11位是单板误码测试
#define OTM_RPC_BOARD_SELF		12	// 第12位是单板自测

#define OTM_FM_ALARM_SYNC		16	// 第16位是告警同步

#define OTM_SM_PWD_CHG			19	// 第19位是修改口令
	
#define OTM_NMC_CM_DIR_MEMBER_ADD	21	// 第21位是设备目录添加成员
#define OTM_NMC_CM_DIR_MEMBER_DEL	22	// 第22位是设备目录删除成员
#define OTM_NMC_CM_DIR_MOVE			23	// 第23位是设备目录移动
#define OTM_NMC_CM_DIR_CREATE		24	// 第24位是设备目录创建
#define OTM_NMC_CM_DIR_DESTROY		25	// 第25位是设备目录销毁
#define OTM_NMC_CM_DIR_RENAME		26	// 第26位是设备目录改名
#define OTM_NMC_CM_DEVICE_ADD		27	// 第27位是设备新增
#define OTM_NMC_CM_DEVICE_DEL		28	// 第28位是设备删除
#define OTM_NMC_CM_DEVICE_ALIAS		29	// 第29位是修改设备别名
#define OTM_NMC_CM_DEVICE_IP		30	// 第30位是修改设备地址
#define OTM_NMC_CM_FTP_BREAK		31	// 第31位是中断文件传输

#define OTM_NMC_FM_ALARM_CUR_GET	35	// 第35位是查询当前告警
#define OTM_NMC_FM_ALARM_HIS_GET	36	// 第36位是查询历史告警
#define OTM_NMC_FM_ALARM_HIS_DEL	37	// 第37位是删除历史告警
#define OTM_NMC_FM_MOCK_QUERY		38	// 第38位是查询机架构件
#define OTM_NMC_FM_ALARM_FILTER_GET	39	// 第39位是获取告警过滤条件
#define OTM_NMC_FM_ALARM_FILTER_SET	40	// 第40位是设置告警过滤条件
	
#define OTM_NMC_PM_LOGON			45	// 第45位是用户登录系统
#define OTM_NMC_PM_LOGOFF			46	// 第46位是用户注销系统

#define OTM_NMC_SM_USER_ADD			50	// 第31位是增加用户
#define OTM_NMC_SM_USER_DEL			51	// 第32位是删除用户
#define OTM_NMC_SM_USER_MODIFY		52	// 第33位是修改用户
#define OTM_NMC_SM_USER_QUERY		53	// 第34位是查询用户资料
#define OTM_NMC_SM_USER_ENUM		54	// 第35位是枚举用户
#define OTM_NMC_SM_USER_LINK		55	// 第36位是设置用户权限组
#define OTM_NMC_SM_USER_UNLINK		56	// 第37位是取消用户权限组
#define OTM_NMC_SM_USER_GROUP		57	// 第38位是查询用户的权限组
#define OTM_NMC_SM_GROUP_CREATE		58	// 第39位是创建权限组
#define OTM_NMC_SM_GROUP_DESTROY	59	// 第40位是销毁权限组
#define OTM_NMC_SM_GROUP_MODIFY		60	// 第41位是修改权限组
#define OTM_NMC_SM_GROUP_PRIV		61	// 第42位是查询组的权限
#define OTM_NMC_SM_GROUP_ENUM		62	// 第43位是枚举组
#define OTM_NMC_SM_GROUP_USER		63	// 第44位是查询组成员用户
#define OTM_NMC_SM_GROUP_ASSIGN		64	// 第45位是组添加成员
#define OTM_NMC_SM_GROUP_UNASSIGN	65	// 第46位是删除组成员
#define OTM_NMC_SM_GROUP_ADD_PRIV	66	// 第47位是组增加权限
#define OTM_NMC_SM_GROUP_DEL_PRIV	67	// 第48位是组删除权限
#define OTM_NMC_SM_USER_PRIV_STAT	68	// 第49位是用户统计权限
#define OTM_NMC_SM_LOG_DEL			69	// 第50位是删除操作日志
#define OTM_NMC_SM_LOG_QUERY		70	// 第51位是查询操作日志
		
#define MAKETYPEMASKVALID(bit, val)	(val[(bit - 1)] = '1')
#define MAKETYPEMASKUNUSE(bit, val) (val[(bit - 1)] = '0')

#define MAKEEVENT(event, sub)	MAKEWORD(sub, event)
#define MAKEOBJTYPE(type, catalog) MAKEWORD(type, catalog)
#define MAKEBCTYPE(bc) MAKELONG(MAKEEVENT(__BC_NMC_COMMON, bc), 0)

#define GETBCTYPE(type) LOWORD(type)
#define GETEVENT(ev) HIBYTE(ev)
#define GETSUBEV(ev) LOBYTE(ev)
#define GETCATALOG(type) HIBYTE(type)
#define GETOBJECTTYPE(type) LOBYTE(type)
//////////////////////////////////////////////////////////////////////////
//常用结构定义

// 定义机架构件框件数据结构
typedef struct tagTSlotInfo
{
	u32	m_dwBoardIp;	// 板卡的ip地址
	u8	m_byLayerIndex;	// 单板的层索引
	u8	m_bySlotIndex;	// 单板的槽位索引
	u8	m_byBoardType;	// 板卡类型
	u8	m_byLinkStatus;	// 单板的链路状况
    u8  m_byOsType;     // OS类型
	s8	m_szPanelLed[64]; //单板的信号灯情况
	s8  m_szSoftwareVersion[255]; //单板软硬件版本 gl 2008.12.17
}TSlotInfo, *PTSlotInfo;


typedef struct tagTMockMachineData
{
	u8	m_byLayerAmount;	// 框件的层数，不包括电源
	u8	m_bySlotLayerAmount;	// 单层的槽位个数
	u16	m_wSlotAmount;		// 构件中单板的数量（实际插入的）
	TSlotInfo m_tSlotInfo[Max_Board_Amount]; // 槽位信息
}TMockMachineData, *PTMockMachineData;

//告警知识库基本信息
typedef struct tagTAlarmKBInfo
{
	u32 m_dwSerialNo;  // 序列号
	u32 m_dwAlarmCode; // 告警码
	u8  m_byLevel;	 // 告警级别
	u8  m_byObjType;	 // 告警对象类型
	s8  m_szDescription[MAX_PATH];
	s8  m_szSolution[MAX_PATH]; 
	s8  m_szProvider[32];
	s8  m_szMemo[MAX_PATH];
}TAlarmKBInfo, *PTAlarmKBInfo;

//告警原因或解决方案。
typedef struct tagTAlarmKBCause
{
	u32 m_dwSerialNo;		// 序列号
	u32 m_dwAlarmCode;	// 告警码
	s8  m_szCause[MAX_PATH];
}TAlarmKBCause, *PTAlarmKBCause;

// 定义告警数据描述
typedef struct tagTAlarmRemark
{
	u32	m_dwCode;				// 告警码
	s8	m_szRemark[64];			// 告警描述
	u8	m_byLevel;				// 告警级别
}TAlarmRemark, *PTAlarmRemark;

// 历史告警查询条件
typedef struct tagTHistoryAlarmCondition
{
	s8	m_szAlarmValue[Max_AlarmValue_Len];// 告警值
	s8	m_szDtGenBgn[Max_DateTime_Format_Len];	// 告警产生日期——开始
	s8	m_szDtGenEnd[Max_DateTime_Format_Len];	// 告警产生日期——结束
	s8	m_szDtResBgn[Max_DateTime_Format_Len];	// 告警恢复时间——开始
	s8	m_szDtResEnd[Max_DateTime_Format_Len];	// 告警恢复时间——结束
	u32	m_dwCode;	// 告警码
	u32	m_dwIpAddr;	// 设备的IP地址
	u32	m_dwDeviceID;// 设备的编号
	u8	m_byLevel;	// 告警级别--0x01表示一级告警，
						//			0x02表示二级告警，
						//			0x04表示三级告警
	u8	m_byObject;	// 告警对象，见告警对象宏
}THistoryAlarmCondition, *PTHistoryAlarmCondition;

// 告警数据定义
typedef struct tagTAlarmContent
{
	s8	m_szSerial[Max_Serial_Len]; // 告警流水号
	s8	m_szRemark[Max_Remark_Len]; // 告警描述
	s8	m_szGenDate[Max_DateTime_Len]; // 告警产生时间
	s8	m_szResDate[Max_DateTime_Len]; // 告警恢复时间
	s8	m_szScheme[Max_Scheme_Len];	// 解决方案
	s8	m_szAlarmValue[Max_AlarmValue_Len];	// 告警值
	s8	m_szProvider[32];		// 告警解决方案提供人
	s8	m_szMemo[MAX_PATH];		// 备注
	u32	m_dwCode;		// 告警码
	u32	m_dwDeviceID;	// 设备编号
	u32	m_dwIpAddr;		// 设备的IP地址
	u8	m_byLevel;		// 告警级别
	u8	m_byObject;		// 告警对象
}TAlarmContent, *PTAlarmContent;


// 告警诊断查询条件
typedef struct tagTAlarmDiagnoseCondition
{
	u32 m_dwCode;	// 告警码
	u8  m_byLevel;	// 告警级别 
	u32 m_dwDeviceID;//设备编号
}TAlarmDiagnoseCondition, *PTAlarmDiagnoseCondition;

// 告警过滤条件 
typedef struct tagTAlarmFilter
{
	u32	m_dwCode;	// 告警码
	u32	m_dwIpAddr;	// 设备的IP地址
	u32	m_dwAlarmCode;	// 告警码
	u16	m_wBoardId;		// 设备的BOARD编号(DTI)
	u16	m_wUnitId;		// 设备的UNIT编号(E1)
	u8	m_byAlarmObj;	// 告警对象
	u8	m_byAlarmLevel;	// 告警级别
}TAlarmFilter, *PTAlarmFilter;

// 用户基本信息
typedef struct tagTUserBaseInfo
{
	u32 m_dwUserID;		// 用户的编号
	s8  m_szLogon[Max_Logon_Len];	// 用户登录名称
	s8  m_szPwd[Max_Pwd_Len]; // 用户登录口令
	s8  m_szName[Max_Logon_Len];	// 用户姓名
	s8  m_szTelephone[Max_Phone_Len]; // 用户的电话
	s8  m_szMobile[Max_Phone_Len]; // 用户的手机
	s8  m_szBeep[Max_Phone_Len]; // 用户的呼机
	s8  m_szEmail[Max_Email_Len]; // 用户的电子信箱
	s8  m_szCompany[Max_Addr_Len]; // 用户的公司
	s8  m_szAddress[Max_Addr_Len]; // 用户的地址
	s8  m_szIntroduction[Max_Introduction_Len]; // 用户简介
	u8  m_bySex; // 用户性别，0表示男，1表示女
}TUserBaseInfo, *PTUserBaseInfo;

//用户组基本信息
typedef struct tagTGroupBaseInfo
{
    u32 m_dwGroupID;
    s8  m_szGroupName[Max_Name_Len];
    s8  m_szDescribing[Max_Describing_Len];
}TGroupBaseInfo, *PTGroupBaseInfo;


//基本信息
typedef struct tagTNameInfo
{
    u32 m_dwID;
    s8  m_szName[Max_Name_Len];
}TNameInfo, *PTNameInfo, *PTNameStruct;

//设备权限
typedef struct tagTPrivilege
{
    u32 m_dwDeviceID;
    u32 m_dwPrivilege;
}TPrivilege, *PTPrivilege;

// 用户操作日志查询条件
typedef struct tagTOprLogCondition
{
	s8	m_szLogon[MAX_PATH];	// 登录名称
	s8	m_szObjName[Max_Obj_Name_Len];	// 操作对象名称
	s8	m_szTmBgn[Max_DateTime_Format_Len];	// 操作日期——开始
	s8	m_szTmEnd[Max_DateTime_Format_Len];	// 操作日期——结束
	s8	m_szOprTypeMask[Max_OprMask_Len + 1];	// 操作类型掩码
	u32	m_dwObjID;		// 操作对象编号
	u16	m_wObjType;		// 用户操作对象类型
	u16	m_wErrorCode; // 操作结果
}TOprLogCondition, *PTOprLogCondition;


// 用户操作日志结果数据
typedef struct tagTOprLogResult
{
	s8	m_szSerial[Max_Serial_Len]; // 流水号ULONG64	m_ul64SerialNo 
	s8	m_szLogon[Max_Logon_Len]; // 登录名称
	s8	m_szOprTime[Max_DateTime_Len]; // 操作日期
	s8	m_szOprType[32]; // 操作类型u16	m_wOprType
	s8	m_szObjName[Max_Obj_Name_Len]; // 操作对象名称
	s8	m_szRemark[MAX_PATH]; // 备注
	s8	m_dwObjID;	// 操作对象编号
	u16	m_wObjType; // 操作对象类型
	u16	m_wErrorCode;	// 错误码
}TOprLogResult, *PTOprLogResult;

// 网管服务器的通讯参数
typedef struct tagTNmsEnvData
{
	u32	m_dwNmsIp;	// 网管服务器地址
	u16	m_wNmsPort;	// 网管服务器节点端口
}TNmsEnvData, *PTNmsEnvData;


//设备父节点参数 xts20080901
typedef struct tagParentEQPEnvData
{
	u32	m_dwParentEQPIp;	// 父节点地址
	u16	m_wParentEQPID;	// 父设备的编号
}TParentEQPEnvData, *PTParentEQPEnvData;
// 网管客户端本身的数据参数
typedef struct tagTNmcEnvData
{
	s8	m_szLogFile[MAX_PATH]; // 日志文件路径
	u16	m_wNodePort; // 本身节点单元测试端口
}TNmcEnvData, *PTNmcEnvData;


// 会议记录
typedef struct tagTConfRecord
{
	u32 m_dwSerialNo;
	s8 m_szMcuAlias[129];
	s8 m_szConfId[33];	
	s8 m_szConfName[33];  // 会议名称
	s8 m_szBeginTime[Max_DateTime_Format_Len];	// 会议开始时间
	s8 m_szEndTime[Max_DateTime_Format_Len];		// 会议结束时间
}TConfRecord, *PTConfRecord;

// 会议查询条件
typedef struct tagTConfQueryCondition
{
	s8 m_szMcuAlias[129]; // MCU别名
	s8 m_szConfName[33];  // 会议名称
	s8 m_szConfId[33];	// 会议ID
	s8 m_szBeginTime[Max_DateTime_Format_Len];	// 会议开始时间
	s8 m_szEndTime[Max_DateTime_Format_Len];		// 会议结束时间
}TConfQueryCondition, *PTConfQueryCondition; 


// 终端记录
typedef struct tagTMtRecord
{
	u32 m_dwSerialNo;
	s8 m_szMtAlias[129];
	s8 m_szConfID[33];
	s8 m_szOprTime[Max_DateTime_Format_Len];
	u32  m_dwOprType; 
}TMtRecord, *PTMtRecord;
// 终端查询条件
typedef struct tagMtQueryCondition
{
	s8 m_szMtAlias[129]; // Mt别名
	s8 m_szConfName[33];  // 会议名称
	s8 m_szConfID[33];	// 会议ID
	s8 m_szOprTime[Max_DateTime_Format_Len];	// 终端操作时间
	u32 m_dwOprType;		// 操作类型
}TMtQueryCondition, *PTMtQueryCondition; 


// 广播消息数据定义
typedef struct tagTBCMessageContent
{
	u32	m_dwBCType;		// 广播类型
	LPVOID	m_pContent;		// 广播内容: 根据广播类型而定，对于告警广播
}TBCMessageContent, *PTBCMessageContent;

// 定义对象变更广播
typedef struct tagTBCObjectChangeContent
{
	u32	m_dwObjectID;	// 变更的对象编号
	u32	m_dwTargetID;	// 变更对象相关
	u16	m_wEventID;		// 变更事件
}TBCObjectChangeContent, *PTBCObjectChangeContent;

// 告警广播数据类型
typedef struct tagTBCAlarmContent
{
	u32	m_dwAlarmCode;//告警码	
//	u32	m_dwCode;	// 告警码
	u32	m_dwDeviceID;	// 告警对象编号
	u32	m_dwIpAddr;	// 设备的IP地址
	s8	m_szTime[Max_DateTime_Len];	//告警时间
	s8	m_szAlarmValue[Max_AlarmValue_Len];	// 告警值
	u8	m_byAlarmType;	// 新产生的告警/告警恢复
	u8	m_byObjectTypeId;// 告警对象类型Id
	u8	m_byAlarmLevel; // 告警级别
}TBCAlarmContent, *PTBCAlarmContent;

// 链路广播和配置广播
typedef struct tagTBCCmContent
{
	u32	m_dwCode;	// 告警码
	u32	m_dwIpAddr;	// 设备的IP地址
	u8	m_byLinkStatus;
	u8	m_byReserved;	// 保留
}TBCCmContent, *PTBCCmContent;

// 配置命令消息数据定义
typedef struct tagTCmMessageContent
{
	u32	m_dwRspCode;	// 响应码
	u32	m_dwCode;	// 告警码
	u32	m_dwIpAddr;	// 设备的IP地址
	s8	m_szRemoteFile[MAX_PATH]; // 设备上的文件名称
	s8	m_szLocalFile[MAX_PATH]; // 本地文件名称
	u16	m_wMsgType;		// 消息类型
	u8	m_byReserved;	// 保留
}TCmMessageContent, *PTCmMessageContent;

// 故障命令消息数据定义
typedef struct tagTFmMessageContent
{
	u32	m_dwRspCode; // 响应码
	u32	m_dwCode;	// 告警码
	u32	m_dwIpAddr;	// 设备的IP地址
	u16	m_wMsgType;		// 消息类型
}TFmMessageContent, *PTFmMessageContent;

// 安全命令消息数据定义
typedef struct tagTSmMessageContent
{
	u32	m_dwRspCode;	// 相应码
	u16	m_wMsgType;		// 消息类型
	u16	m_wReserved;	// 保留
}TSmMessageContent, *PTSmMessageContent;

// 公共消息数据定义
typedef struct tagTPmMessageContent
{
	u32	m_dwRspCode; // 响应码
	u16	m_wMsgType; // 消息类型
	u16	m_wReserved; // 保留
}TPmMessageContent, *PTPmMessageContent;

typedef struct tagTFtpParameterStruct
{
	u32	m_dwDeviceID;		// 设备编号
	s8	m_szLocalFile[MAX_PATH];	// 本地文件路径名称
	s8	m_szRemoteFile[MAX_PATH];	// 远程文件路径名称
	s8	m_szObjName[Max_Obj_Name_Len];	// 设备名称
	u16	m_wDeviceType;		// 设备类型
	u16	m_wFtpEvent;		// 文件传输方向
	u16	m_wOprObjType;			// 操作对象类型
//	u8	m_byFileType;		// 文件类型--FILE_TYPE_ASC | FILE_TYPE_BIN
    u8  m_byOsType;         //操作系统类型
}TFtpParameterStruct, *PTFtpParameterStruct;

typedef struct tagTEquipProperty
{
	s8	m_szAlias[Max_Name_Len+1]; // 别名
	u32	m_dwEquipID;	// 设备编号
	u32	m_dwIpAddr;		// 设备ip地址
	u32 m_dwEquipParentID; // 父设备设备编号
	u8	m_byLinkStatus; // 链接状态
}TEquipProperty, *PTEquipProperty; 

typedef struct tagTDirInfo
{
    u32 m_dwDirID;                        //设备组ID
    u32 m_dwSuperID;                      //父设备组ID
    s8	m_szDirName[Max_Name_Len + 1];     //组名
}TDirInfo, *PTDirInfo;