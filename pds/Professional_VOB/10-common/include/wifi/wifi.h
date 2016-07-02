/*============================================================================*\
  模块名：WI-FI
  文件名：wifi.h
  功能：	    1、以Infra或Ad-hoc方式加入WI-FI网络，支持无密钥、WEP、
  			    WPA-PSK、WPA2-PSK身份认证/加密模式，不支持Enterprise方式
  			    的WPA/WPA2模式；以回调方式上报加入动作的结果；
  			    2、支持网络扫描及其结果上报；
  			    3、支持WI-FI接口验证、状态获取；
  			    4、支持对WI-FI接口的UP/DOWN状态的设置；
  			    5、支持对WI-FI接口的IP地址的设置、获取；
  			    6、支持对WI-FI接口的网关的设置、获取、删除。
  作者：王远涛/高祺
  版本：见宏WIFI__VERSION
  修改记录：
\*============================================================================*/

// ----------------------------------------------------------------------------
// Constants and Defines
// Notice: never modify default values, especially the enums!!!
// ----------------------------------------------------------------------------
#ifndef __WIFI_USER_IF_H
#define __WIFI_USER_IF_H

#ifdef __cplusplus
extern "C" {
#endif

#define	SSID_MAXLEN	32						/* SSID 最大长度 */
#define BSSID_LEN		6						/* SSID MAC 长度 */
#define PASSWD_LEN		64						/* 密码最大长度 
													(最大252位情况为57个字符，
													向上取整为64)*/
#define WEP_KEY_NUM	4						/* WEP密钥数量最大值*/
#define WIFI_IN			const
#define WIFI_OUT
#define WIFI_DEF
#define WIFI_OK			0
#define WIFI_ERROR		-1
//#define WIFI_DEBUG

#ifndef BOOL
typedef int BOOL;
#endif
#define PACKED __attribute__((packed))
#define ALIGN4 __attribute__((aligned(4)))

typedef enum tagSNetType {					/* 组网方式 */
	NT_ADHOC = 1,
	NT_INFRA,
	NT_INVALID
}SNetType;

typedef enum tagSWPAArithmetic {			/* WPA/WPA2-PSK加密算法 */
	AR_NONE = 0,
	AR_WEP40,
	AR_TKIP ,
	AR_WRAP,
	AR_CCMP,									/* AES */
	AR_WEP104,
	AR_INVALID
}SWPAArithmetic;

typedef struct tagSBSSIDItem {				/* 频道信息 */
	unsigned int		uChannel;					/* 频道号 */
	unsigned char		abyBSSIDMac[BSSID_LEN];		/* AP点的MAC地址 */
	unsigned char		abySSID[SSID_MAXLEN + 1];	/* AP点的名字 */
	unsigned int		wBeaconInterval;			/* BCN（省电参数） */
	SNetType		eNetType;					/* 组网方式(Infra / Ad-hoc) */

	BOOL			bEncrypt;					/* 是否加密(默认0表示不加密) */
	BOOL			bWepOn;					/* 是否使用WEP */
	BOOL			bWepPasswordOpen;			/* 是否按照开放系统方式认证 */
	BOOL			bWpaOn;					/* 是否使用WPA */
	BOOL			bWpa2On;					/* 使用WPA2 */
	SWPAArithmetic	bArithmetic;					/* 加密方法(AES / TKIP) */
	unsigned int		bPSKOff;						/* RSN模式(PSK / Enterprise) */
	unsigned int		uMaxRate;					/* AP点可连接最大速度(单位kbps) */
	unsigned int		uRSSI;						/* 信号强度(0 - 100) */
	BOOL			abPwdsValid[WEP_KEY_NUM];	/* 密钥有效性 */
	unsigned char		sPasswd[WEP_KEY_NUM][PASSWD_LEN];		/* 密钥表 */

	ALIGN4 struct tagSBSSIDItem	*next;
} SBSSIDItem;
/* 说明：各国频道号都是在WIFI频道1到14之间, 每个频道频率范围固定, 不存在兼容问题
 * bEncrypt为1后WEP和WPA的项才有效，bWepOn和bWpaOn和bWpa2On互斥。
 * bArithmetic是bWpaOn和bWpa2On的子项。
 * bPSKOff是WPA/WPA2的选择项，默认0表示使用WPA-PSK而不是WPA-Enterprise。
 */

typedef enum tagSJoinStatus{					/* 无线网卡状态 */
	DISCONNECTED = 0,							/* 断开 */
	CONNECTED,									/* 连接 */
	DEVICE_PULLIN,								/* USB插上*/
	DEVICE_PULLOUT								/* USB拔下*/
}SJoinStatus;
typedef void (*joinchanelCallback)(SJoinStatus stat);

typedef struct tagSCmdLinkStatus {			/* 无线终端连接状态 */
	PACKED BOOL		bLink;						/* 是否处于连接状态 */
	PACKED unsigned short	wBSSType;				/* Infra还是Ad-hoc */
	PACKED unsigned char	byState;					/* Ad-hoc状态: 未使用 */
	PACKED unsigned char	abyBSSIDMac[BSSID_LEN];	/* AP点mac地址 */
	PACKED unsigned char	abySSID[SSID_MAXLEN + 2];	/* AP点名字 */
	PACKED unsigned int	uChannel;				/* 频段数 */
	PACKED unsigned int	uLinkRate;				/* 连接速度(单位kbps) */
} SCmdLinkStatus, WIFI_DEF *PSCmdLinkStatus;
/* 说明：速度是所使用的802.11某个协议的速度，和距离无关.
 * byState为兼容性定义的一个域，没有意义.
 */

typedef struct tagSDot11MIBCount {			/* 802.11 MIB信息 */
	unsigned int		TransmittedFragmentCount;
	unsigned int		MulticastTransmittedFrameCount;
	unsigned int		FailedCount;
	unsigned int		RetryCount;
	unsigned int		MultipleRetryCount;
	unsigned int		RTSSuccessCount;
	unsigned int		RTSFailureCount;
	unsigned int		ACKFailureCount;
	unsigned int		FrameDuplicateCount;
	unsigned int		ReceivedFragmentCount;
	unsigned int		MulticastReceivedFrameCount;
	unsigned int		FCSErrorCount;
} SDot11MIBCount, WIFI_DEF *PSDot11MIBCount;

typedef enum tagSIfFlag{
	IF_DOWN = 0,	//Down the interface
	IF_UP			//Up it
}SIfFlag;

// ----------------------------------------------------------------------------
// APIs
// ----------------------------------------------------------------------------
// 1.1            检测Wi-Fi设备
// ----------------------------------------------------------------------------
// 接口形式：	int wifi_getif(WIFI_OUT char *if_name);
// 描述：		检查输入参数指定的接口是否是WI-FI网口，
//					状态是否正常
// 参数：		if_name--指向返回的Wi-Fi接口字符串的指针，
// 					如"eth1"
// 返回值：		0表示调用成功，其它值表示失败
// 编号：
int wifi_getif(WIFI_OUT char *if_name);

// ----------------------------------------------------------------------------
// 1.2            注册回调函数
// ----------------------------------------------------------------------------
// 接口形式：	int wifi_registerCallback(WIFI_IN joinchanelCallback jcb);
// 描述：		注册供在接口状态(UP/DOWN、PLUG/UNPLUG、连接
// 					成功/失败)发生变化时回调的函数
// 参数：		jcb--注册过程状态变化时调用的回调函数指针
// 返回值：		0表示调用成功，其它值表示失败
// 编号：
int wifi_registerCallback(WIFI_IN joinchanelCallback jcb);

// ----------------------------------------------------------------------------
// 1.3            扫描BSS节点
// ----------------------------------------------------------------------------
// 接口形式：	int wifi_scanbss(WIFI_IN char *if_name, 
// 						WIFI_OUT SBSSIDItem **plist);
// 描述：		在指定接口上进行扫描以发现临近的AP点或
//					Ad-hoc节点
// 参数：		if_name--指向Wi-Fi接口字符串的指针，如"eth1"；
//					plist--指向返回的BSS节点指针列表的指针，
// 					值-结果参数
// 返回值：		0表示调用成功，其它值表示失败
// 编号：
int wifi_scanbss(WIFI_IN char *if_name, WIFI_OUT SBSSIDItem **plist);

// ----------------------------------------------------------------------------
// 1.4            释放扫描到的BSS节点
// ----------------------------------------------------------------------------
// 接口形式：	int wifi_freebss(WIFI_OUT SBSSIDItem **plist);
// 描述：		释放指定的、扫描到的AP点或Ad-hoc节点列表
// 参数：		plist--指向BSS节点指针列表的指针
// 返回值：		0表示调用成功，其它值表示失败
// 编号：
int wifi_freebss(WIFI_OUT SBSSIDItem **plist);

// ----------------------------------------------------------------------------
// 1.5            获取无线接口连接状态
// ----------------------------------------------------------------------------
// 接口形式：	int wifi_getstat(WIFI_IN char *if_name, WIFI_OUT SCmdLinkStatus *stat);
// 描述：		获取指定接口的连接状态
// 参数：		if_name--指向Wi-Fi接口字符串的指针，如"eth1"；
// 					stat--指向无线接口连接状态数据结构的指针，
// 					值-结果参数
// 返回值：		0表示调用成功，其它值表示失败
// 编号：
int wifi_getstat(WIFI_IN char *if_name, WIFI_OUT SCmdLinkStatus *stat);

// ----------------------------------------------------------------------------
// 1.6            设置接口DOWN/UP状态
// ----------------------------------------------------------------------------
// 接口形式：	int wifi_change_if_flag(WIFI_IN char *if_name, 
// 						WIFI_IN SIfFlag act);
// 描述：		强制指定接口的DOWN/UP状态
// 参数：		if_name--指向Wi-Fi接口字符串的指针，如"eth1"；
// 					act--设置动作，SIfFlag类型，具体取值可为
// 					IF_DOWN 或IF_UP
// 返回值：		0表示调用成功，-1表示失败
// 编号：
int wifi_change_if_flag(WIFI_IN char *if_name, WIFI_IN SIfFlag act);

// ----------------------------------------------------------------------------
// 1.7            在指定接口以满参数形式加入指定AP或Ad-hoc节点
// ----------------------------------------------------------------------------
// 接口形式：	int wifi_join(WIFI_IN char *if_name, 
// 						WIFI_IN SBSSIDItem *item);
// 描述：		在指定接口以指定的满参数(即所有加入需
// 					要参数都手动提供)形式加入指定的AP(或
// 					Ad-hoc节点)。注意: 本接口不阻塞，即调用者
// 					需要通过已注册的回调函数通知自己接口
// 					加入成功与否
// 参数：		if_name--指向Wi-Fi接口字符串的指针，如"eth1"；
// 					item--欲加入AP的满参数的数据结构，包括对
// 					身份认证和加密方式的指定（如WEP、WPA-PSK、
// 					WPA2-PSK等）都在本参数指向的数据结构中
// 					设置
// 返回值：		0表示调用成功，其它值表示失败
// 编号：
int wifi_join(WIFI_IN char *if_name, WIFI_IN SBSSIDItem *item);

// ----------------------------------------------------------------------------
// 1.8            在指定接口以自动参数形式加入指定AP或Ad-hoc节点
// ----------------------------------------------------------------------------
// 接口形式：	int wifi_join_nonverbose(WIFI_IN char *if_name, 
// 						WIFI_IN char *SSID, 
// 						WIFI_IN char **apPassWd);
// 描述：		在指定接口以指定的自动参数(即仅需要提
// 					供指定加入AP的SSID和(必要时的)密钥)加入
// 					指定的AP(或Ad-hoc节点)；注意: 本接口不阻塞，
// 					即调用者需要通过已注册的回调函数通知
// 					自己加入动作的结果
// 参数：		if_name--指向Wi-Fi接口字符串的指针，如"eth1"；
// 					SSID--指向欲加入AP或Ad-hoc节点的SSID字符串的
// 					指针的指针；
// 					apPassWd--若欲加入AP或Ad-hoc有身份认证/加密
// 					设置，则本参数提供指向密钥字符串(组)的
// 					指针
// 返回值：		0表示调用成功，其它值表示失败
// 编号：
int wifi_join_nonverbose(WIFI_IN char *if_name, WIFI_IN char *SSID, WIFI_IN char **apPassWd);

// ----------------------------------------------------------------------------
// 1.9            在指定接口退出指定AP或Ad-hoc节点
// ----------------------------------------------------------------------------
// 接口形式：	int wifi_unjoin(WIFI_IN char *if_name);
// 描述：		在指定接口退出指定AP或Ad-hoc节点，即断开
// 					连接
// 参数：		if_name--指向Wi-Fi接口字符串的指针，如"eth1"
// 返回值：		0表示调用成功，其它值表示失败
// 编号：
int wifi_unjoin(WIFI_IN char *if_name);

// ----------------------------------------------------------------------------
// 1.10            设置接口IP地址
// ----------------------------------------------------------------------------
// 接口形式：	int wifi_setip(WIFI_IN char *if_name, 
// 						WIFI_IN unsigned int ip_addr, 
// 						WIFI_IN unsigned int ip_mask);
// 描述：		设置指定接口的IP地址及其掩码
// 参数：		if_name--指向Wi-Fi接口字符串的指针，如"eth1"；
// 					ip_addr--网络序无符号长整型接口IP地址；
// 					ip_mask--网络序无符号长整型接口掩码
// 返回值：		0表示调用成功，其它值表示失败
// 编号：
int wifi_setip(WIFI_IN char *if_name, WIFI_IN unsigned int ip_addr, WIFI_IN unsigned int ip_mask);

// ----------------------------------------------------------------------------
// 1.11            获取接口地址
// ----------------------------------------------------------------------------
// 接口形式：	int wifi_getip(WIFI_IN char *ifname, 
// 						WIFI_OUT unsigned int *ip, 
// 						WIFI_OUT unsigned int *mask, 
// 						WIFI_OUT char *mac);
// 描述：		获取指定接口的IP地址及其掩码以及MAC地址
// 参数：		if_name--指向Wi-Fi接口字符串的指针，如"eth1"；
// 					ip_addr--网络序无符号长整型接口IP地址，值-
// 					结果参数；
// 					ip_mask--网络序无符号长整型接口掩码，值
// 					-结果参数；
// 					mac_addr--指向接口MAC地址的指针，值-结果
// 					参数
// 返回值：		0表示调用成功，其它值表示失败
// 编号：
int wifi_getip(WIFI_IN char *ifname, WIFI_OUT unsigned int *ip, WIFI_OUT unsigned int *mask, WIFI_OUT char *mac);

// ----------------------------------------------------------------------------
// 1.12            设置默认网关
// ----------------------------------------------------------------------------
// 接口形式：	int wifi_setgw(WIFI_IN char *if_name, 
// 						WIFI_IN unsigned int gw_addr);
// 描述：		将指定地址设置为指定接口的默认网关
// 参数：		if_name--指向Wi-Fi接口字符串的指针，如"eth1"；
// 					gw_addr--网络序无符号长整型网关IP地址
// 返回值：		0表示调用成功，其它值表示失败
// 编号：
int wifi_setgw(WIFI_IN char *if_name, WIFI_IN unsigned int gw_addr);

// ----------------------------------------------------------------------------
// 1.13            清除默认网关
// ----------------------------------------------------------------------------
// 接口形式：	int wifi_clrgw(WIFI_IN unsigned int gw_addr);
// 描述：		删除指定的默认网关
// 参数：		gw_addr--网络序无符号长整型网关IP地址
// 返回值：		0表示调用成功，其它值表示失败
// 编号：
int wifi_clrgw(WIFI_IN unsigned int gw_addr);

// ----------------------------------------------------------------------------
// 1.14            获取默认网关
// ----------------------------------------------------------------------------
// 接口形式：	int wifi_getgw(WIFI_IN char *if_name, 
// 						WIFI_OUT unsigned int *gw_addr);
// 描述：		获取指定接口的默认网关
// 参数：		if_name--指向Wi-Fi接口字符串的指针，如"eth1"；
// 					gw_addr--网络序无符号长整型网关IP地址，
// 					值-结果参数
// 返回值：		0表示调用成功，其它值表示失败
// 编号：
int wifi_getgw(WIFI_IN char *if_name, WIFI_OUT unsigned int *gw_addr);

// ----------------------------------------------------------------------------
// 1.15            获取接口WI-FI MIB信息
// ----------------------------------------------------------------------------
// 接口形式：	int wifi_getmib(WIFI_IN char *if_name, WIFI_OUT SDot11MIBCount *mib);
// 描述：		获取指定接口的WI-FI MIB信息(目前是个占位函数，
// 					其功能暂未完全实现)
// 参数：		if_name--指向Wi-Fi接口字符串的指针，如"eth1"；
// 					mib--指向无线接口连接MIB信息数据结构的指针，
// 					值-结果参数
// 返回值：		0表示调用成功，其它值表示失败
// 编号：
int wifi_getmib(WIFI_IN char *if_name, WIFI_OUT SDot11MIBCount *mib);

// ----------------------------------------------------------------------------
// 1.16            获取模块版本信息
// ----------------------------------------------------------------------------
// 接口形式：	char *wifi_version();
// 描述：		获取模块版本(V/R及编译时间等)信息
// 参数：		无
// 返回值：		NULL表示获取失败，其它值为编译信息字符串
// 编号：
char *wifi_version();

#define WIFI__VERSION	"WIFI 1.8.3.20080730"
/***************************
模块版本：WIFI 1.1.1.20080228
增加功能：递交第一个wifi版本
修改缺陷：无
提交人：王远涛
*****************************/

/***************************
模块版本：WIFI 1.1.1.20071122
增加功能：递交第一个wifi版本
修改缺陷：无
提交人：王远涛
*****************************/

/***************************
模块版本：WIFI 1.1.2.20071129
增加功能：递交ssl, crypto, dl, m的静态lib文件
修改缺陷：无
提交人：王远涛
*****************************/

/***************************
模块版本：WIFI 1.1.2.20071220
增加功能：增加arm平台支持，增加wifi_setip接口，解决回调问题
修改缺陷：无
提交人：王远涛
*****************************/

/***************************
模块版本：WIFI 1.1.2.20080228
增加功能：修正加入ap的bug，及状态上报bug，新增接口获取ip/mask/mac
修改缺陷：无
提交人：王远涛
*****************************/

/***************************
模块版本：WIFI 1.1.2.20080305
增加功能：infra和ad-hoc对应的值不正确，由[0,1]改回[1,2]
修改缺陷：无
提交人：王远涛
*****************************/

/***************************
模块版本：WIFI 1.1.2.20080310
增加功能：修改bug：输入参数未作合法性检测，adhoc段链仍可以ping通等问题
修改缺陷：无
提交人：王远涛
*****************************/

/***************************
模块版本：WIFI 1.1.2.20080311
增加功能：修改ad-hoc对wep加密的支持，修改ad-hoc状态上报不正确的bug
修改缺陷：无
提交人：王远涛
*****************************/

/***************************
模块版本：WIFI 1.1.3.20080313
增加功能：无
修改缺陷：设置和获取WIFI接口IP地址的错误，以及WPA JOIN/UNJOIN的系列错误
提交人：高祺
*****************************/

/***************************
模块版本：WIFI 1.2.1.20080320
增加功能：设置/清除默认网关以完成经AP的路由功能
修改缺陷：反复快速加入/退出BSS的系列错误以及对无线射频信号的处理等
提交人：高祺
*****************************/

/***************************
模块版本：WIFI 1.3.1.20080326
增加功能：对VIA USB WIFI卡热插拔的支持（与驱动配合完成）
修改缺陷：无
提交人：高祺
*****************************/

/***************************
模块版本：WIFI 1.4.1.20080331
增加功能：WEP多密钥、获取默认网关ID地址
修改缺陷：无
提交人：高祺
*****************************/

/***************************
模块版本：WIFI 1.4.2.20080407
增加功能：无
修改缺陷：热插拔一系列错误，去除WEP方式下"key on"的设置等
提交人：高祺
*****************************/

/***************************
模块版本：WIFI 1.4.3.20080408
增加功能：无
修改缺陷：Ad-Hoc模式退出和重加入时的密钥清除问题
提交人：高祺
*****************************/

/***************************
模块版本：WIFI 1.4.4.20080409
增加功能：无
修改缺陷：退出AP时自动加入另一AP的问题
提交人：高祺
*****************************/

/***************************
模块版本：WIFI 1.4.5.20080415
增加功能：无
修改缺陷：不同AP，WEP多密钥的兼容性问题
提交人：高祺
*****************************/

/***************************
模块版本：WIFI 1.4.6.20080417
增加功能：无
修改缺陷：KDM2100密钥驱动问题
提交人：高祺
*****************************/

/***************************
模块版本：WIFI 1.4.7.20080422
增加功能：无
修改缺陷：帮助规避TP-LINK的WEP首密钥连接问题
提交人：高祺
*****************************/

/***************************
模块版本：WIFI 1.4.8.20080428
增加功能：无
修改缺陷：解决连接AP时因扫描失败而引发的异常问题
提交人：高祺
*****************************/

/***************************
模块版本：WIFI 1.4.8.20080507
增加功能：使能/关闭WIFI接口的API
修改缺陷：解决WEP方式连接AP时有时出现的连接不成功需再次连接问题
提交人：高祺
*****************************/

/***************************
模块版本：WIFI 1.5.1.20080513
增加功能：VIA在其驱动中增加了对STA主动断开与AP连接的支持，相应修改上层
修改缺陷：无
提交人：高祺
*****************************/

/***************************
模块版本：WIFI 1.5.2.20080522
增加功能：无
修改缺陷：连接WPA过程中热插拔后断开连接的错误打印
提交人：高祺
*****************************/

/***************************
模块版本：WIFI 1.5.3.20080526
增加功能：无
修改缺陷：尝试进行WPA连接时因没有判断接口DOWN/UP状态导致程序阻塞
提交人：高祺
*****************************/

/***************************
模块版本：WIFI 1.5.4.20080602
增加功能：无
修改缺陷：注册回调后链路状态变化时的一个临时ioctl fd未关闭错误
提交人：高祺
*****************************/

/***************************
模块版本：WIFI 1.6.1.20080612
增加功能：修改单个密码字符串容许长度(256 -> 64)
修改缺陷：无
提交人：高祺
*****************************/

/***************************
模块版本：WIFI 1.7.1.20080616
增加功能：WPA2-PSK
修改缺陷：无
提交人：高祺
*****************************/

/***************************
模块版本：WIFI 1.7.2.20080626
增加功能：无
修改缺陷：注释掉WPA信号处理调试信息
提交人：高祺
*****************************/

/***************************
模块版本：WIFI 1.8.1.20080711
增加功能：自动参数(仅需提供SSID和密钥)加入AP，并增加API注释
修改缺陷：无
提交人：高祺
*****************************/

/***************************
模块版本：WIFI 1.8.2.20080717
增加功能：无
修改缺陷：以WPA-PSK/WPA2-PSK方式加入AP出错时的无限等待问题
提交人：高祺
*****************************/

/***************************
模块版本：WIFI 1.8.3.20080730
增加功能：无
修改缺陷：WEP开放系统方式下自动加入AP失败的问题
提交人：高祺
*****************************/

#if defined(WIFI_DEBUG)
#define dsleep(x)	sleep(x)
#define dpline() printf("debug@%s: line %d...\n", __func__, __LINE__)
#define dprintfbin(buf, size)	do{	int i;						\
					printf("MAC: ");				\
					for(i = 0; i < size - 1; i++){			\
						printf("%02x ", ((char*)buf)[i]);	\
					}						\
					printf("%02x\n", ((char*)buf)[i]);		\
				}while(0)
#else
#define dpline()
#define dprintfbin(buf, size)
#define dsleep(x)
#endif

#ifdef __cplusplus
}
#endif

#endif

