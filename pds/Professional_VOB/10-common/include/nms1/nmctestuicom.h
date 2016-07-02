/********************************************************************
模块名:      网管客户端界面测试库
文件名:      NmcCom.h
相关文件:    
文件实现功能 网管客户端宏定义
作者：       许世林
版本：       3.5
------------------------------------------------------------------------
修改记录:
日  期		版本		修改人		走读人    修改内容
2004/10/15	3.5			许世林                  创建
*********************************************************************/
#if _MSC_VER	> 1000
#pragma	once
#endif	// _MSG_VER > 1000

#include <vector>
using namespace std;


//以下资源ID取自原网管客户端resource.h中的id
// 菜单资源ID
#define IDR_MENU_LOGON                  131		//工具栏菜单
#define IDR_MENU_POPUP                  137		//弹出菜单
// 登录界面资源ID----------------------------------------------------------
#define IDC_COMBO_NMS                   1231	//网管服务器选择	Combox
#define IDC_COMBO_USER_LOGON            1230	//登录用户名		Combox
#define IDC_EDIT_PWD_LOGON              1007	//登录口令			Edit
#define IDC_BTN_REFRESH_SERVER          1005	//刷新服务器		Button
#define IDC_BTN_LOGON                   1004	//登录				Button

// 故障查询界面资源ID--------------------------------------------------------
#define IDC_COMBO_ALARM_CODE            1241	//告警码类别		Combox
#define IDC_EDIT_DEVICE_ID              1226	//设备编号			Edit
#define IDC_CHECK_ALARM_LEVEL_1         1035	//告警级别：1级		CheckBox
#define IDC_CHECK_ALARM_LEVEL_2         1036	//告警级别：2级		CheckBox
#define IDC_CHECK_ALARM_LEVEL_3         1037	//告警级别：3级		CheckBox
//告警产生时间
#define IDC_RADIO_ALARM_HIS_GEN_TIME_ALL		1131	//所有时间			RadioButton
#define IDC_RADIO_ALARM_TIME_GENERATE_MONTH		1038	//前几个月			RadioButton
#define IDC_EDIT_ALARM_TIME_GENERATE_MONTH		1041	//前几个月			Edit
#define IDC_RADIO_ALARM_TIME_GENERATE_DAY		1039	//前几天			RadioButton
#define IDC_EDIT_ALARM_TIME_GENERATE_DAY		1043	//前几天			Edit
#define IDC_RADIO_ALARM_TIME_GENERATE_BETWEEN	1040	//介于某个时间	RadioButton
#define IDC_DATETIMEPICKER_ALARM_TIME_GENERATE_BGN_DATE 1045//开始日期	DateTimeCtrl
#define IDC_DATETIMEPICKER_ALARM_TIME_GENERATE_BGN_TIME 1046//开始时间	DateTimeCtrl
#define IDC_DATETIMEPICKER_ALARM_TIME_GENERATE_END_DATE 1047//结束日期	DateTimeCtrl
#define IDC_DATETIMEPICKER_ALARM_TIME_GENERATE_END_TIME 1048//结束时间	DateTimeCtrl
//告警恢复时间
#define IDC_RADIO_ALARM_HIS_RES_TIME_ALL		1130	//所有时间			RadioButton
#define IDC_RADIO_ALARM_TIME_RESTORE_MONTH		1049	//前几个月			RadioButton
#define IDC_EDIT_ALARM_TIME_RESTORE_MONTH		1052	//前几个月			Edit
#define IDC_RADIO_ALARM_TIME_RESTORE_DAY		1050	//前几天			RadioButton
#define IDC_EDIT_ALARM_TIME_RESTORE_DAY			1054	//前几天			Edit
#define IDC_RADIO_ALARM_TIME_RESTORE_BETWEEN	1051	//介于某个时间		RadioButton
#define IDC_DATETIMEPICKER_ALARM_TIME_RESTORE_BGN_DATE 1056//开始日期	DateTimeCtrl
#define IDC_DATETIMEPICKER_ALARM_TIME_RESTORE_BGN_TIME 1057//开始时间	DateTimeCtrl
#define IDC_DATETIMEPICKER_ALARM_TIME_RESTORE_END_DATE 1058//结束日期	DateTimeCtrl
#define IDC_DATETIMEPICKER_ALARM_TIME_RESTORE_END_TIME 1059//结束时间	DateTimeCtrl

//////////////////////////////////////////////////////////////////////////
//MCU会议日志对话框
#define IDC_LIST_CONFINFO               1000      //会议日志信息      ListCtrl


//日志查询界面资源ID----------------------------------------------------------------
#define IDC_EDIT_OPRLOG_USER_MATCH      1009	//用户名			Edit	
#define IDC_SEL_ALL_OPER                1345	//全选				Button
#define IDC_UNSEL_ALL_OPER              1347	//全不选			Button
#define IDC_LIST_OPRLOG_OPRTYPE         1047	//操作类型			CheckBoxList
//操作日期时间
#define IDC_RADIO_OPR_TIME_ALL          1125	//所有时间			RadioButton
#define IDC_RADIO_MONTH_LATEST          1011	//前几个月			RadioButton
#define IDC_EDIT_OPR_DT_LATEST_MONTH    1012	//前几个月			Edit
#define IDC_RADIO_DAY_LATEST            1014	//前几天			RadioButton
#define IDC_EDIT_OPR_DT_LATEST_DAY      1015	//前几天			Edit
#define IDC_RADIO_OPR_DT_BETWEEN        1017	//介于某个时间		RadioButton
#define IDC_DATETIMEPICKER_BGN_DATE     1018	//开始日期			DateTimeCtrl
#define IDC_DATETIMEPICKER_END_DATE     1019	//结束日期			DateTimeCtrl
#define IDC_DATETIMEPICKER_BGN_TIME     1020	//开始时间			DateTimeCtrl
#define IDC_DATETIMEPICKER_END_TIME     1021	//结束时间			DateTimeCtrl
//操作对象
#define IDC_COMBO_OPR_OBJ_TYPE          1022	//操作对象类型		Combox
#define IDC_EDIT_OBJ_TYPE_NAME          1235	//对象名称			Edit
#define IDC_CHECK_LABEL_OBJ_ID          1239	//对象编号			CheckButton
#define IDC_EDIT_OBJ_TYPE_ID            1237	//对象编号			Edit
//操作结果
#define IDC_CHECK_OPR_RESULT_SUCC       1048	//成功				CheckButton
#define IDC_CHECK_OPR_RESULT_FAIL       1105	//失败				CheckButton

//告警查询和日志查询--操作按钮-------------------
#define IDC_BTN_SEARCH_BEGIN            1002	//开始搜索			Button
#define IDC_BTN_SEARCH_STOP             1003	//停止搜索			Button
#define IDC_BTN_LOG_DELETE              1243	//全部删除			Button

//用户管理界面资源ID-------------------------------------------------------
//基本资料
#define IDC_EDIT_USER_ID                1233	//用户编号			Edit
#define IDC_EDIT_USER_INFO_LOGON_NAME   1065	//用户名			Edit
#define IDC_EDIT_USER_INFO_PWD_LOGON    1066	//用户密码			Edit	
#define IDC_EDIT_USER_INFO_PWD_CONFIRM  1067	//密码确认			Edit
#define IDC_EDIT_USER_INFO_NAME         1068	//用户信息用户名	Edit
#define IDC_RADIO_USER_INFO_SEX_MALE    1069	//性别男			RadioButton
#define IDC_RADIO_USER_INFO_SEX_FEMALE  1070	//性别女			RadioButton
#define IDC_EDIT_USER_INFO_PHONE        1071	//电话				Edit
#define IDC_EDIT_USER_INFO_MOBILE       1072	//手机				Edit
#define IDC_EDIT_USER_INFO_BEEP         1073	//呼机				Edit
#define IDC_EDIT_USER_INFO_EMAIL        1074	//Email				Edit
#define IDC_EDIT_USER_INFO_COMPANY      1075	//单位				Edit
#define IDC_EDIT_USER_INFO_ADDRESS      1076	//住址				Edit
#define IDC_EDIT_USER_INFO_REMARK       1082	//备注				Edit
#define IDC_BTN_USER_ADD_SAVE           1103	//用户添加（保存）	Button
#define IDC_BTN_USER_INFO_MODIFY        1079	//用户修改			Button
#define IDC_BTN_USER_INFO_DEL           1080	//用户删除			Button
//权限信息
#define IDC_LIST_USER_INFO_GROUPS       1063	//用户权限信息列表	ListBox
#define IDC_BTN_USER_INFO_GROUP_ADD     1077	//添加组			Button
#define IDC_BTN_USER_INFO_GROUP_DEL     1078	//删除组			Button	
#define IDC_BTN_USER_INFO_PRIV_STAT     1081	//统计权限			Button

//用户组管理界面资源ID---------------------------------------------------
#define IDC_EDIT_GROUPNAME              1264	//组名称			Edit
#define IDC_EDIT_GROUP_DESCRIBING       1184	//组描述			Edit
#define IDC_BTN_GROUP_ADD_SAVE          1183	//组创建			Button
#define IDC_BTN_GROUP_MODIFY            1185	//组修改			Button
#define IDC_BTN_GROUP_DESTROY           1182	//组删除			Button
#define IDC_LIST_GROUP_INFO_USERS       1083	//成员用户列表		List
#define IDC_BTN_GROUP_INFO_USER_ADD     1084	//添加成员用户		Button
#define IDC_BTN_GROUP_INFO_USER_DEL     1085	//删除成员用户		Button
#define IDC_BTN_GROUP_INFO_PRIV_SET     1087	//权限设置			Button
#define IDC_LIST_GROUP_INFO_PRIVILEGE   1086	//权限信息列表		List

//创建设备组对话框资源ID－－－－－－－－－－－－－－－－－－－－－－－－－－－－
#define IDC_EDIT_DIR_NAME               1213	//设备组名称		Edit
#define IDC_LIST_DEVICE_ALL             1214	//设备成员列表		List
#define IDC_BTN_DIR_CREATE_OK           1216	//确定创建设备组	Button
#define IDC_BTN_DIR_CREATE_CANCEL       1215	//取消创建设备组	Button
#define IDC_CLOSE_BTN                   1361	//关闭窗口			Button

//增加设备对话框资源ID------------------------------------------------------
#define IDC_COMBO_EQP_TYPE              1204	//设备类型			Combox
#define IDC_COMBO_CATALOG               1210	//设备子类型		Combox
#define IDC_IPADDRESS_SELF              1206	//IP地址			IpAddrCtrl
//#define IDC_EDIT_ALIAS_SELF             1207	//设备别名			Edit
#define IDC_BTN_EQUIP_CANCEL            1208	//取消增加			Button
#define IDC_BTN_EQUIP_ADD               1209	//增加				Button
//#define IDC_CLOSE_BTN                   1361	//关闭窗口			Button

//增加告警知识库对话框资源ID--------------------------------------------------
#define IDC_EDIT_ALARMCODE              1264	//告警码			Edit
#define IDC_EDIT_LEVEL                  1354	//告警级别			Edit
#define IDC_EDIT_DESCRIPTION            1265	//告警描述			Edit
#define IDC_COMBO_OBJTYPE               1355	//告警对象类型		Combox
#define IDC_EDIT_PROVIDER               1267	//方案提供这		Edit
#define IDC_EDIT_MEMO                   1268	//备注				Edit
#define IDC_LIST_ALARM_CAUSE            1348	//告警原因			List
#define IDC_LIST_ALARM_SOLUTION         1347	//解决方案			List
#define IDC_EDIT_CAUSE_SOLUTION         1350	//告警原因或方案	Edit
#define IDC_BTN_INPUT                   1353	//确定增加一条原因或方案	Button
#define IDC_BTN_ADD                     1281	//确定增加一条知识库Button
//#define IDC_CLOSE_BTN                   1361	//关闭窗口			Button

//成员选择对话框资源ID－－－－－－－－－－－－－－－－－－－－－－－－－－－－
#define IDC_LIST_MEMBER_ALL             1090	//设备成员列表		List
#define IDC_LIST_MEMBER_NEW             1091	//已选设备成员列表	List
#define IDC_BTN_MEMBER_ALL_ADD_ALL      1094	// >>				Button
#define IDC_BTN_MEMBER_ALL_ADD_ONE      1095	// >				Button
#define IDC_BTN_MEMBER_NEW_DEL_ONE      1096	// <				Button
#define IDC_BTN_MEMBER_NEW_DEL_ALL      1097	// <<				Button
#define IDC_BTN_MEMBER_MODIFY_OK        1092	//确定修改			Button
#define IDC_BTN_MEMBER_MODIFY_CANCEL    1093	//取消修改			Button
//#define IDC_CLOSE_BTN                   1361	//关闭窗口			Button

//外设配置对话框资源ID－－－－－－－－－－－－－－－－－－－－－－－－－－－－
#define IDC_EDIT_1                      1264	//					Edit
#define IDC_EDIT_2                      1265	//					Edit
#define IDC_EDIT_3                      1266	//					Edit
#define IDC_EDIT_4                      1267	//					Edit
#define IDC_EDIT_5                      1268	//					Edit
#define IDC_EDIT_6                      1269	//					Edit
#define IDC_EDIT_7                      1270	//					Edit
#define IDC_EDIT_8                      1271	//					Edit
//#define IDC_CLOSE_BTN                   1361	//关闭窗口			Button

//线路测试Ping对话框资源ID－－－－－－－－－－－－－－－－－－－－－－－－
#define IDC_EDIT_TIMES                  1247	//次数				Edit
#define IDC_EDIT_PACKAGE                1248	//包大小			Edit
#define IDC_BTN_PING_PROCESS            1155	//开始测试			Button
#define IDC_BTN_PING_STOP               1158	//中断测试			Button
#define IDC_BTN_PING_CANCEL             1156	//取消测试			Button
//#define IDC_CLOSE_BTN                   1361	//关闭窗口			Button

//设备操作权限信息设置对话框资源ID－－－－－－－－－－－－－－－－－－－－
#define IDC_LIST_EQUIP_PRIVILEGE        1197	//设备权限列表		CheckBoxList
//#define IDC_LIST_DEVICE_ALL             1214	//设备成员列表		List
#define IDC_BTN_PRIV_SET_OK             1195	//确定设置权限信息	Button
#define IDC_BTN_PRIV_SET_CANCEL         1196	//取消设置权限信息	Button
//#define IDC_CLOSE_BTN                   1361	//关闭窗口			Button

//修改密码对话框资源ID－－－－－－－－－－－－－－－－－－－－－－－－－－－－－
#define IDC_EDIT_PWD_OLD                1198	//旧密码			Edit
#define IDC_EDIT_PWD_NEW                1199	//新密码			Edit
#define IDC_EDIT_PWD_CONFIRM            1200	//密码确认			Edit
#define IDC_BTN_PWD_CHANGE              1201	//确定修改密码		Button
#define IDC_BTN_PWD_CANCEL              1202	//取消修改密码		Button
//#define IDC_CLOSE_BTN                   1361	//关闭窗口			Button

//设备操作结果－－基本信息资源ID－－－－－－－－－－－－－－－－－－－－－－－－
#define IDC_LIST_RESULT                 1389    //设备操作结果      List
#define IDC_TEXT_EDIT                   1375    //WarnDlg           Text

//设备属性页－－基本信息资源ID－－－－－－－－－－－－－－－－－－－－－－－－
#define IDC_EDIT_CFGVER                 1030	//配置版本号		Edit
#define IDC_EDIT_MCUID                  1031	//mcu编号			Edit
#define IDC_EDIT_MCUALIAS               1032	//mcu别名			Edit
#define IDC_EDIT_E164NUM                1033	//mcu的E164号		Edit
#define IDC_COMBO_NEEDBAS               1343	//级联时是否需要码流适配	Combox

//设备属性页－－单板配置资源ID－－－－－－－－－－－－－－－－－－－－－－－－
#define IDC_EDIT_LAYER                  1265	//层号				Edit
#define IDC_EDIT_SLOT                   1266	//槽号				Edit
#define IDC_COMBO_BOARDTYPE             1357	//板类型			Combox
#define IDC_IPADDRESS_IP                1287	//IP地址			IpAddrCtrl
#define IDC_BTN_CONFIRM                 1259	//确定修改			Button
#define IDC_BTN_CANCEL                  1260	//取消修改			Button
#define IDC_LIST_BOARD                  1021	//单板列表			List

//设备属性页－－网络配置资源ID－－－－－－－－－－－－－－－－－－－－－－－－
#define IDC_GK_IP                       1290	//GK的IP地址		IpAddrCtrl
#define IDC_MULTICAST_IP                1291	//mcu组播ip			IpAddrCtrl
#define IDC_EDIT_RECVSTARTPORT          1034	//接收数据起始端口	Edit
#define IDC_EDIT_MULTICASTPORT          1033	//mcu组播端口		Edit
#define IDC_CHECK_FW                    1048	//是否使用mpc转发	CheckBox
#define IDC_CHECK_PS                    1049	//是否使用mpc内置协议栈	CheckBox
#define IDC_LIST_TRAPRECV               1020	//结束trap服务器列表	List
//#define IDC_IPADDRESS_IP                1287	//IP地址			IpAddrCtrl
#define IDC_EDIT_COMMUNITY              1264	//community			Edit
//#define IDC_BTN_CONFIRM                 1259	//确定修改			Button
//#define IDC_BTN_CANCEL                  1260	//取消修改			Button

//设备属性页－－网络同步配置资源ID－－－－－－－－－－－－－－－－－－－－－－－－
#define IDC_COMBO_NETSYN_MODE           1342	//网络同步模式				Combox
#define IDC_EDIT_NETSYNCDTSLOT          1033	//同步跟踪时使用的DT板号	Edit
#define IDC_EDIT_NETSYNCE1INDEX         1034	//同步跟踪时使用的E1板号	Edit

//设备属性页－－外设配置1资源ID－－－－－－－－－－－－－－－－－－－－－－－－
#define IDC_LIST_MIXER                  1022	//混音器			List
#define IDC_LIST_RECORDER               1023	//录/放像机			List
#define IDC_LIST_TVWALL                 1024	//电视墙			List

//设备属性页－－外设配置2资源ID－－－－－－－－－－－－－－－－－－－－－－－－
#define IDC_EDIT_DCSERVERID             1039	//数据会议服务器编号Edit
#define IDC_LIST_BAS                    1025	//码流适配器		List
#define IDC_LIST_VMP                    1026	//画面合成器		List

//设备属性页－－包重传配置资源ID－－－－－－－－－－－－－－－－－－－－－－－－
#define IDC_EDIT_PRSID                  1036	//设备编号			Edit
#define IDC_EDIT_PRSMCUSTARTPORT        1037	//mcu起始端口号		Edit
#define IDC_EDIT_PRSSWITCHBRDID         1040	//交换板编号		Edit
#define IDC_EDIT_PRSALIAS               1039	//别名				Edit
#define IDC_EDIT_PRSRUNNINGBRDID        1045	//运行板索引号		Edit
#define IDC_EDIT_PRSSTARTPORT           1038	//Prs起始端口号		Edit
#define IDC_EDIT_PRSFIRSTTIMESPAN       1041	//第一重传检测点	Edit
#define IDC_EDIT_PRSSECONDTIMESPAN2     1044	//第二重传检测点	Edit
#define IDC_EDIT_PRSTHIRDTIMESPAN3      1042	//第三重传检测点	Edit
#define IDC_EDIT_PRSREJECTTIMESPAN4     1043	//时间跨度			Edit

//告警过滤
#define IDC_LIST_ALARM_CODE             1165	//告警码过滤		ListBox
//#define IDC_CHECK_ALARM_LEVEL_1         1035	//告警级别：1级		CheckBox
//#define IDC_CHECK_ALARM_LEVEL_2         1036	//告警级别：2级		CheckBox
//#define IDC_CHECK_ALARM_LEVEL_3         1037	//告警级别：3级		CheckBox

//mcu设备单板ID
//层号顺序为自下而上: 10001, 10002, 10003, 10004
#define IDC_BASIC_LAYER0				10000	//层号基id
//槽号为自左而右: 1001, 1002, ..., 1017
#define IDC_BASIC_SLOT0					1000	//槽号基id

//对话框资源ID－－－－－－－－－－－－－－－－－－－－－－－－－－－－－
#define IDD_ABOUTBOX                    100		//about				Dlg
#define IDD_DLG_ALARM_SOLUTION          192		//告警解决方案		Dlg
#define IDD_DLG_CONFINFO                193		//会议信息			Dlg
#define IDD_DLG_DEVICE_DIR_CREATE       170		//设备组创建		Dlg
#define IDD_DLG_DEVICE_PROPS_BASIC      178		//设备基本属性		Dlg
#define IDD_DLG_DIR_PROPS_BASIC         172		//文件夹基本属性	Dlg
#define IDD_DLG_EQUIP_ADD               168		//增加设备			Dlg
#define IDD_DLG_FTP_PROGRESS            150		//Ftp上传/下载		Dlg
#define IDD_DLG_KNOWLEDGE_BASE_ADD      197		//告警知识库添加	Dlg
#define IDD_DLG_MEMBER_SEL              141		//成员设置			Dlg
#define IDD_DLG_PERI_CFG                201		//外设配置			Dlg
#define IDD_DLG_PING                    151		//线路测试			Dlg
#define IDD_DLG_PRIVILEGE_INFO          160		//权限信息设置		Dlg
#define IDD_DLG_PWD_CHANGE              161		//修改密码			Dlg
#define IDD_DSI_E1_CFG                  196		//Dsi_E1配置		Dlg
#define IDD_NMCALARMQUERY_FORM          104		//告警查询条件		Dlg
#define IDD_NMCGROUP_FORM               115		//用户组			Dlg
#define IDD_NMCLOGQUERY_FORM            106		//日志查询条件		Dlg
#define IDD_NMCUSER_FORM                101		//用户信息			Dlg
#define IDD_NMCWELCOMEVIEW_FORM         117		//登录				Dlg
#define IDD_PAGE_ALARM_FILTER_CODE      152		//告警过滤－告警码	Dlg
#define IDD_PAGE_ALARM_FILTER_LEVEL     153		//告警过滤－告警级别Dlg
#define IDD_PAGE_ALARM_FILTER_OBJECT    154		//告警过滤－告警对象Dlg
#define IDD_PROPPAGE_PERICFG2           183		//设备属性－外设配置2		Dlg
#define IDD_PROPPAGE_BASEINFO           184		//设备属性－基本信息		Dlg
#define IDD_PROPPAGE_BOARDCFG           185		//设备属性－单板设置		Dlg
#define IDD_PROPPAGE_NETCFG             186		//设备属性－网络设置		Dlg
#define IDD_PROPPAGE_NETSYNCCFG         187		//设备属性－网络同步		Dlg
#define IDD_PROPPAGE_PERICFG1           188		//设备属性－外设配置1
#define IDD_PROPPAGE_PRSCFG             189		//设备属性－码流重传配置	Dlg
#define IDD_WARN                        307		//信息提示框				Dlg

//以下为菜单索引定义
//登录后菜单ID定义－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－
#define MENU_SYS						(u8)0		//系统
#define MENU_CM							(u8)1		//配置管理
#define MENU_FM							(u8)2		//故障管理
#define	MENU_SM							(u8)3		//安全管理

//系统
#define UI_SYS_LOGOFF					(u8)0		//注销
                                                    //1-Separator
#define UI_APP_EXIT                     (u8)2     //退出

//配置管理
#define UI_CM_DEVICE_ADD				(u8)0		//增加设备
#define UI_CM_DIR_CREATE				(u8)1		//创建设备组
													//2-Separator
#define UI_CM_DEVICE_DIR				(u8)3		//设备目录树

//故障管理
#define UI_FM_CUR_ALARM					(u8)0		//当前告警
#define UI_FM_HIS_ALARM					(u8)1		//历史告警
													//2-Separator
#define UI_FM_KB_ALARM					(u8)3		//告警知识库
#define UI_FM_FILTER_ALARM				(u8)4		//告警过滤
													//5-Separator
#define UI_FM_MOCK_ALARM				(u8)6		//机架构件图

//安全管理
#define UI_SM_USER						(u8)0		//用户管理
#define UI_SM_LOG						(u8)1		//操作日志管理
													//2-Separator
#define UI_SM_MODIFY_PWD				(u8)3		//修改口令
													//4-Separator
#define UI_SM_CONFINFO					(u8)5		//MCU会议日志信息

//设备操作菜单ID定义－－－－－－－－－－－－－－－－－－－－－－－－－
#define UI_DEVICE_MOCK					(u8)0		//机架构件图
													//1-Separator
#define UI_DEVICE_SNMP					(u8)2		//Snmp测试
#define UI_DEVICE_SYNCTIME				(u8)3		//时间同步
													//4-Separator
#define UI_DEVICE_RESET					(u8)5		//远程复位
													//6-separator
#define UI_DEVICE_SYNCALARM				(u8)7		//告警同步
#define UI_DEVICE_CURALARM				(u8)8		//当前告警
#define UI_DEVICE_HISALARM				(u8)9		//历史告警
													//10-separator
#define UI_DEVICE_DOWNLOAD				(u8)11	//文件下载
#define UI_DEVICE_UPLOAD				(u8)12	//文件上传
#define UI_DEVICE_UPLOADFILES           (u8)12    //文件装载
													//13-separator
#define UI_DEVICE_PING					(u8)14	//线路测试
													//15-separator
#define UI_DEVICE_DEL					(u8)16	//删除
													//17-separator
#define UI_DEVICE_DIAGNOSE				(u8)18	//告警诊断
#define UI_DEVICE_PROS					(u8)19	//设备属性

//设备组根节点操作菜单ID定义－－－－－－－－－－－－－－－－－－
#define UI_DIRROOT_OPEN                 (u8)0     //打开
                                                    //1-separator
#define UI_DIRROOT_CREATE               (u8)2     //创建设备组
                                                    //3-separator
#define UI_DIRROOT_REFRESH              (u8)4     //设备刷新
                                                    //5-separator
#define UI_DIRROOT_MEMBERADD            (u8)6     //增加设备

//设备组操作菜单ID定义－－－－－－－－－－－－－－－－－－－－－
#define UI_DIR_DISPALL					(u8)0		//显示所有设备
#define UI_DIR_OPEN						(u8)1		//打开
													//2-Separator
#define UI_DIR_CREATE					(u8)3		//创建设备组
#define UI_DIR_DEL						(u8)4		//删除设备组
													//5-separator
#define UI_DIR_ASSIGN					(u8)6		//分配设备
#define UI_DIR_UNASSIGN					(u8)7		//取消分配
													//8-separator
#define UI_DIR_REFRESH					(u8)9		//设备刷新
#define UI_DIR_ARRANGE                  (u8)10    //排列图标
													//11-separator
#define UI_DIR_MEMBERADD				(u8)12	//增加设备
#define UI_DIR_PROS						(u8)13	//设备组属性

//设备组根节点对应的设备列表的弹出菜单
#define UI_EQPLIST_ROOT_ADD          (u8)0     //增加设备
#define UI_EQPLIST_ROOT_REFRESH      (u8)1     //刷新设备
#define UI_EQPLIST_ROOT_ARRANGE      (u8)2     //排列图标

//设备组对应的设备列表的弹出菜单
#define UI_EQPLIST_SHOWALL          (u8)0     //显示所有设备
                                                //1
#define UI_EQPLIST_CREATE           (u8)2     //创建设备组
#define UI_EQPLIST_DEL              (u8)3     //删除设备组
                                                //4
#define UI_EQPLIST_ASSIGN           (u8)5     //分配设备
#define UI_EQPLIST_CANCEL           (u8)6     //取消设备
                                                //7
#define UI_EQPLIST_REFRESH          (u8)8     //设备刷新
#define UI_EQPLIST_ARRANGE          (u8)9     //排列图标
                                                //10
#define UI_EQPLIST_ADD              (u8)11    //增加设备
#define UI_EQPLIST_PROS             (u8)12    //设备组属性

//用户管理——用户操作菜单————————————————————
#define UI_USER_DEL                     (u8)0     //删除
                                                    //1
#define UI_USER_ADDPRIV                 (u8)2     //关联权限组
#define UI_USER_DELPRIV                 (u8)3     //撤销权限组
                                                    //4
#define UI_USER_PROP                    (u8)5     //属性

//用户管理——组操作菜单—————————————————————
#define UI_USERGROUP_DEL                (u8)0     //删除
                                                    //1
#define UI_USERGROUP_ADDMEMBER          (u8)2     //添加成员
#define UI_USERGROUP_DELMEMBER          (u8)3     //删除成员
                                                    //4
#define UI_USERGROUP_PRIVSET            (u8)5     //设置权限
                                                    //6
#define UI_USERGROUP_PROP               (u8)7     //属性

//单板操作菜单ID定义－－－－－－－－－－－－－－－－－－－－－－
#define UI_BOARD_REBOOT					(u8)0		//重启动
#define UI_BOARD_SYNCTIME				(u8)1		//时间同步
#define UI_BOARD_CODETEST				(u8)2		//误码测试
#define UI_BOARD_SELFTEST				(u8)3		//单板自检
#define UI_BOARD_LINKCFG				(u8)4		//链路设置
#define UI_BOARD_DOWNLOAD				(u8)5		//文件下载
#define UI_BOARD_UPLOAD					(u8)6		//文件上传
													//7-separator
#define UI_BOARD_REFRESH				(u8)8		//刷新

//mcu设备属性修改－网络配置接收trap的服务器列表菜单操作
#define RCVTRAP_ADD						(u8)0
#define RCVTRAP_MODIFY					(u8)1
#define RCVTRAP_DEL						(u8)2

//mcu设备属性修改－单板配置菜单操作
#define BRDCFG_ADD						(u8)0
#define BRDCFG_MODIFY					(u8)1
#define BRDCFG_DEL						(u8)2

//mcu设备属性修改－外设配置菜单操作
#define PERICFG_ADD						(u8)0
#define PERICFG_MODIFY					(u8)1
#define PERICFG_DEL						(u8)2



//检验得到的句柄合法性, 打印出错误所在文件名和行数
#define CHECK_HANDLE(hWnd)															\
	if(hWnd == NULL)																\
	{																				\
		CString strName = __FILE__;													\
		s32 nLeft = strName.ReverseFind('\\');										\
		TRACE("文件%s中第%d行---控件句柄非法!\n", strName.Mid(nLeft+1), __LINE__);	\
		return FALSE;																\
	}

//等待500ms
#define SLEEP_500	m_bOprDelay ? Sleep(500) : NULL

//等待1s
#define SLEEP_1000	m_bOprDelay ? Sleep(1000) : NULL

//为了实现界面正常操作而必须作的延时
#define SLEEP_SYS	Sleep(500)

//应用程序名
#define APP_TITLE_NAME	_T("网管客户端")
#define APP_EXE_NAME	_T("NmsClient.exe")

//系统默认用户和组
#define DEF_USER		_T("admin")
#define DEF_GROUP		_T("Administrators")

//定义当前所在操作界面id
#define POS_INVALID				(u8)0	//非法值
#define POS_HISALARM			(u8)1 //历史告警
#define POS_USER				(u8)2 //用户管理
#define POS_LOG					(u8)3 //日志管理
#define POS_EQP					(u8)4 //设备管理
#define POS_CURALARM			(u8)5	//当前告警
#define POS_ALARMKB				(u8)6	//告警知识库
#define POS_MOCK				(u8)7	//机架构件图

//  xsl [11/25/2004] 组权限设置宏定义--若设置多项权限将下面宏取"或"
#define GROUP_PRIV_SNMP			0x0001	//SNMP测试
#define GROUP_PRIV_SYNTIME		0x0002	//时间同步
#define GROUP_PRIV_DOWNLOAD		0x0004	//文件下载
#define GROUP_PRIV_UPLOAD		0x0008	//文件上传
#define GROUP_PRIV_REMOTECTRL	0x0010	//远程控制
#define GROUP_PRIV_SYNALARM		0x0020	//告警同步

//增加设备时类型定义
//设备类型
#define EQPADD_TYPE_MCU			(u8)0	//MCU
#define EQPADD_TYPE_MT			(u8)1	//终端
//终端设备子类型
#define EQPADD_SUBTYPE_PCMT         (u8)0	//桌面终端
#define EQPADD_SUBTYPE_8010         (u8)1	//嵌入式8010
#define EQPADD_SUBTYPE_8010C        (u8)2	//嵌入式8010C
#define EQPADD_SUBTYPE_IMT          (u8)3	//嵌入终端IMT
#define EQPADD_SUBTYPE_8010A        (u8)4   //嵌入式8010A
#define EQPADD_SYBTYPE_8010APLUS    (u8)5   //嵌入式8010A+

//mcu设备子类型
#define EQPADD_SUBTYPE_MCU8000	(u8)0
#define EQPADD_SUBTYPE_MCU8000B	(u8)1

//MCU属性配置时页面索引定义
#define MCUCFG_BASICINFO		(u8)0	//基本信息
#define MCUCFG_NET				(u8)1	//网络配置	
#define MCUCFG_BOARD			(u8)2	//单板配置	
#define MCUCFG_PERI1			(u8)3	//外设配置1
#define MCUCFG_PERI2			(u8)4	//外设配置2
#define MCUCFG_PRS				(u8)5	//包重传配置
#define MCUCFG_NETSYNC			(u8)6	//网同步配置

//告警(日志)产生(恢复)时间类型定义
#define DT_ALLTIME				(u8)0	//所有时间
#define DT_LATEST_MONTH			(u8)1	//前N月	
#define DT_LATEST_DAY			(u8)2	//前N天
#define DT_BETWEEN				(u8)3	//介于


/* MCU告警对象类型 */
#define ALARMOBJ_MCU						(u8)1
#define ALARMOBJ_MCU_BOARD					(u8)11
#define ALARMOBJ_MCU_LINK					(u8)12
#define ALARMOBJ_MCU_TASK					(u8)13
#define ALARMOBJ_MCU_SDH					(u8)14
#define ALARMOBJ_MCU_POWER					(u8)15
#define ALARMOBJ_MCU_MODULE					(u8)16
#define ALARMOBJ_MCU_SOFTWARE				(u8)17
#define ALARMOBJ_MCU_BRD_FAN				(u8)18

/* MT告警对象类型 */
#define ALARMOBJ_MT_BASE				(u8)128
#define ALARMOBJ_MT                     (u8)(ALARMOBJ_MT_BASE + 1)
#define ALARMOBJ_MT_E1                  (u8)(ALARMOBJ_MT_BASE + 11)
#define ALARMOBJ_MT_MAP					(u8)(ALARMOBJ_MT_BASE + 12)
#define ALARMOBJ_MT_CCI					(u8)(ALARMOBJ_MT_BASE + 13)
#define ALARMOBJ_MT_STREAM				(u8)(ALARMOBJ_MT_BASE + 14)
#define ALARMOBJ_MT_ETHERNET			(u8)(ALARMOBJ_MT_BASE + 15)
#define ALARMOBJ_MT_TASK				(u8)(ALARMOBJ_MT_BASE + 16)
#define ALARMOBJ_MT_FAN					(u8)(ALARMOBJ_MT_BASE + 17)
#define ALARMOBJ_MT_V35					(u8)(ALARMOBJ_MT_BASE + 18)

typedef struct tagSingleBoard
{
    CString csMcuName;      //设备名
    u8      bySlot;         //槽号
    u8      bylayer;        //层号
}TSingleBoard, *PTSingleBoard;

//历史告警查询条件结构定义
typedef struct tagHisAlarmCondition
{	
	CString csAlarmType;				//告警类别－告警所对应的告警描述	
	u32     dwEqpId;						//设备编号－默认为0	
	u8      byAlarmLevel;					//告警级别－001为一级告警，010为二级告警，100为三级告警

	//告警产生时间	
	u8      byGenType;					//类型－按上面定义 	
	u8      byGenMonth;					//前n月	
	u8      byGenDay;						//前n天	
	SYSTEMTIME timeGenBgn;				//介于
	SYSTEMTIME timeGenEnd;
	
	//告警恢复时间	
	u8      byResType;					//类型－按上面定义	
	u8      byResMonth;					//前n月	
	u8      byResDay;						//前n天	
	SYSTEMTIME timeResBgn;				//介于
	SYSTEMTIME timeResEnd;	

	//默认值
	tagHisAlarmCondition()
	{
		csAlarmType = "所有告警";
		dwEqpId = 0;
		byAlarmLevel = 7;
		byGenType = DT_ALLTIME;
		byGenMonth = 0;
		byGenDay = 0;
		memset(&timeGenBgn, 0, sizeof(SYSTEMTIME));
		memset(&timeGenEnd, 0, sizeof(SYSTEMTIME));
		byResType = DT_ALLTIME;
		byResMonth = 0;
		byResDay = 0;
		memset(&timeResBgn, 0, sizeof(SYSTEMTIME));
		memset(&timeResEnd, 0, sizeof(SYSTEMTIME));	
	};

}THisAlarmCondition;

//日志查询结构定义
typedef struct tagLogCondition
{
	CString csLogonName;				//登录用户名	
	BOOL32	bSelAll;					//是否全选
    vector<s32> vecIndex;                //操作类型索引，从0开始。bSelAll为FALSE时有效。
	u8      byOprTimeType;				//操作时间类型-按上面定义	
	u8      byDay;						//前n天	
	u8      byMonth;					//前n月	
	SYSTEMTIME	timeOprBgn;				//操作开始时间	
	SYSTEMTIME	timeOprEnd;				//操作结束时间 	
	CString csOprObj;					//操作对象	
	CString csObjName;					//操作对象名称	
	BOOL32	bCheckObjID;				//操作对象编号CheckButton	
	u32	    dwObjID;					//操作对象id
	BOOL32  bSuccess;					//操作结果－成功	
	BOOL32  bFail;						//失败

	//默认值
	tagLogCondition()
	{
		csLogonName = "";
		bSelAll = FALSE;
		byOprTimeType = DT_ALLTIME;
		byDay = 0; 
		byMonth = 0;
		memset(&timeOprBgn, 0, sizeof(SYSTEMTIME));	
		memset(&timeOprEnd, 0, sizeof(SYSTEMTIME));			
		csOprObj = "";
		csObjName = "";
		bCheckObjID = FALSE;
		dwObjID = 0;
		bSuccess = FALSE;
		bFail = FALSE;
        vecIndex.clear();
	};

}TLogCondition;

//mcu设备配置－外设类型定义
#define PERI_MIXER			(u8)0	//混音器
#define PERI_REC			(u8)1	//录放像机
#define PERI_TW				(u8)2	//电视墙
#define PERI_BAS			(u8)3	//码流适配器
#define PERI_VMP			(u8)4	//画面合成器

//mcu设备配置－网络配置－接收trap的服务器信息
typedef struct tagRcvTrapSvr
{
	CString csIP;
	CString csCommunity;
    CString csModifyIP;   //要修改的IP  [zhuyr][4/27/2005]
}TRcvTrapSvr;

//mcu设备配置－单板配置信息
typedef struct tagBrdCfg
{
	u16     wIndex;						//索引号
	u16     wLayer;						//层号
	u16     wSlot;						//槽号
	CString csType;						//板类型
	CString csIP;						//单板IP

}TBrdCfg;

//mcu设备配置－混音器配置信息
typedef struct tagMixerCfg
{
	u8  byNum;						//编号
	u32 dwMcuBgnPort;				//mcu起始端口号
	u32 dwSwitchIndex;				//交换板索引
	CString csAlias;				//别名
	u32 dwRunIndex;					//运行板索引
	u32 dwMixerBgnPort;				//混音器起始端口号
	u32 dwMaxGrpNum;				//每个map最大混音组数
	CString strMapId;				//map编号

}TMixerCfg;

//mcu设备配置－录像机配置信息
typedef struct tagRecCfg
{
	u8  byNum;						//编号
	u32 dwMcuBgnPort;				//mcu起始端口号
	u32 dwSwitchIndex;				//交换板索引
	CString csAlias;				//别名
	CString csIP;					//ip地址
	u32 dwRecBgnPort;				//录放像机起始端口号
	u32 dwRecChnn;					//录像通道数
	u32 dwPlayChnn;					//放像通道数
	
}TRecCfg;

//mcu设备配置－电视墙配置信息
typedef struct tagTVWallCfg
{
	u8  byNum;						//编号
	CString csAlias;				//别名
	u32 dwRunIndex;					//运行板索引
	u32 dwTWBgnPort;				//电视墙起始端口号
	u32 dwSplitType;				//分割方式
	u32 dwSplitNum;					//分割画面数
    CString strMapId;               //map编号

}TTVWallCfg;

//mcu设备配置－码流适配器配置信息
typedef struct tagBasVmpCfg
{
	u8 byNum;						//编号
	u32 dwMcuBgnPort;				//mcu起始端口
	u32 dwSwitchIndex;				//交换板索引
	CString csAlias;				//别名
	u32 dwRunIndex;					//运行板索引
	u32 dwBgnPort;					//起始端口
	CString strMapId;				//map编号

}TBasVmpCfg;

//mcu设备配置－画面合成器配置信息
typedef struct tagVmpCfg
{
	u8 byNum;						//编号
	u32 dwMcuBgnPort;				//mcu起始端口
	u32 dwSwitchIndex;				//交换板索引
	CString csAlias;				//别名
	u32 dwRunIndex;					//运行板索引
	u32 dwBgnPort;					//起始端口
    u32 dwCount;                    //码流路数
	CString strMapId;				//map编号

}TVmpCfg;

//mcu配置信息定义
typedef struct tagMcuCfgInfo
{
	//基本信息配置	
	CString	csCfgVer;					//配置版本号	
	CString csMcuId;					//mcu编号	
	CString csMcuAlias;					//mcu别名	
	CString csMcuE164;					//mcu的E164号	
	BOOL32  bNeedBas;					//级联时是否需要码流适配

	//网络配置	
	CString csNetGkIP;					//Gk的ip地址	
	CString csNetMcuMcIP;				//mcu组播地址	
	u32     dwNetRcvBgnPort;				//接收数据起始端口	
	u32     dwNetMcuMcPort;			    	//mcu组播端口	
	BOOL32 bNetMpcTrans;					//是否使用mpc转发数据	
	BOOL32 bNetMpcPS;						//是否使用mpc内置协议栈	
	vector<TRcvTrapSvr> vtTrapSvrAdd;	//增加的trap服务器	
	vector<TRcvTrapSvr> vtTrapSvrModify;//修改的trap服务器	
	vector<CString> vtTrapSvrDel;		//删除的trap服务器 - 根据ip地址删除

	//单板配置	
	vector<TBrdCfg>		vtBrdCfgAdd;	//增加单板	
	vector<TBrdCfg>		vtBrdCfgModify;	//修改单板	
	vector<u16>		    vtBrdCfgDel;	//删除单板 - 根据索引号删除

	//外设配置	
	vector<TMixerCfg>	vtMixerAdd;		//增加数字混音器	
	vector<TMixerCfg>	vtMixerModify;	//修改数字混音器	
	vector<u8>	    	vtMixerDel;		//删除数字混音器 － 根据编号删除
	
	vector<TRecCfg>		vtRecAdd;		//增加录/放像机
	vector<TRecCfg>		vtRecModify;	//修改录/放像机	
	vector<u8>	    	vtRecDel;		//删除录/放像机 - 根据编号删除
	
	vector<TTVWallCfg>	vtTWAdd;		//增加电视墙	
	vector<TTVWallCfg>	vtTWModify;		//修改电视墙	
	vector<u8>	    	vtTWDel;		//删除电视墙 - 根据编号删除	
	
	vector<TBasVmpCfg>	vtBasAdd;		//增加码流适配器		
	vector<TBasVmpCfg>	vtBasModify;	//修改码流适配器	
	vector<u8>	    	vtBasDel;		//删除码流适配器 - 根据编号删除
	
	vector<TVmpCfg>	    vtVmpAdd;		//增加画面合成器	
	vector<TVmpCfg>	    vtVmpModify;	//修改画面合成器	
	vector<u8>	    	vtVmpDel;		//删除画面合成器－根据编号删除

	//包重传配置	
	u32     dwPrsNum;					//设备编号		
	u32     dwPrsMcuBgnPort;			//mcu起始端口号	
	u32     dwPrsSwitchIndex;			//交换板编号	
	CString csPrsAlias;					//别名	
	u32     dwPrsRunIndex;				//运行板索引	
	u32     dwPrsBgnPort;				//Prs起始端口号	
	u32     dwPrsFstTime;				//第1重传检测点	
	u32     dwPrsSndTime;				//第2重传检测点	
	u32     dwPrsThdTime;				//第3重传检测点	
	u32     dwPrsTimeSpan;				//过期丢弃的时间跨度

	//网同步配置	
	CString csNetSyncMode;				//同步模式	
	u8	    byNetSyncDTNum;				//网同步跟踪时使用的DT板号	
	u8	    byNetSyncE1Num;				//网同步跟踪时使用的E1板号
	
}TMcuCfgInfo;

//用户组权限设置
typedef struct tagEqpPrivSet
{
	CString csEqpIP;					//设备ip地址
	u32  dwPriv;						//权限
}TEqpPrivSet;

typedef struct tagGroupPriv
{
	CString csGroupName;
	vector<TEqpPrivSet> vtEqpPriv;

}TGroupPriv;

//增加设备
typedef struct tagEqpAdd
{
	u8  byEqpType;						//设备类型
	u8  bySubType;						//设备子类型
	u32 dwIP;							//设备ip地址

}TEqpAdd;

//线路测试
typedef struct tagPingTest
{
	CString csEqpName;					//设备名称
	u16	wTimes;					    	//次数
	u16	wPackSize;				    	//包大小

}TPingTest;

//设备文件上传/下载
typedef struct tagFileUpDown
{
	CString csEqpName;					//设备名称
	CString csFileName;					//文件名

}TFileUpDown;

//用户关联/取消权限组, 权限组添加/删除用户成员
typedef struct tagUserWithGroup
{
	vector<CString> vtArray;		//权限数组或用户数组
	CString csName;					//用户名或权限组名
	
}TUserWithGroup;

//修改当前用户口令
typedef struct tagModifyUserPwd
{
	CString csOldPwd;
	CString csNewPwd;
    CString csNewComPwd;
	
}TModifyUserPwd;

//告警过滤
typedef struct tagEqpAlarmFilter
{
	u8  byLevel;
	vector<CString> vtCode;
	
}TEqpAlarmFilter;

typedef struct tagAlarmKBCauseTest
{
	u32     m_dwSerialNo;  // 序列号
    CString m_szCase;
}TAlarmKBCauseTest;

//告警知识库
typedef struct tagAlarmKB
{
	u32     m_dwAlarmCode;  // 告警码
	u8      m_byLevel;	    // 告警级别
	u8      m_byObjType;	// 告警对象类型(ALARMOBJ_MCU, ALARMOBJ_MT等)
	CString  m_szDescription;       //描述
	CString  m_szProvider;          //提供者
	CString  m_szMemo;              //备注
    vector<CString> m_vecCause;     //告警原因
    vector<CString> m_vecSolution;  //告警解决方案
    //以下为修改删除
    vector<TAlarmKBCauseTest> m_vecModifyCause;     //告警原因
    vector<u32> m_vecDelCause;
    vector<TAlarmKBCauseTest> m_vecModifySolution;  //告警解决方案
    vector<u32> m_vecDelSolution;
}TAlarmKB;