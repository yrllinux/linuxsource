/*******************************************************************************            
 *  模块名   : MT                                                              *            
 *  文件名   : mtconst.h                                                       *            
 *  相关文件 :                                                                 *            
 *  实现功能 : 视频会议系统终端软件常量定义头文件                              *            
 *  作者     : 张明义                                                          *            
 *  版本     : V3.0  Copyright(c) 1997-2005 KDC, All rights reserved.          *            
 *                                                                             *            
 *    =======================================================================  *            
 *  修改记录:                                                                  *            
 *    日  期      版本        修改人      修改内容                             *            
 *    2005/8/6    4.0         张明义      创建                                 *            
 *                                                                             *            
 *******************************************************************************/           
                                                                                            
                                                                                            
                                                                                            
#ifndef _MT_CONST_H_                                                                        
#define _MT_CONST_H_                                                                        

#include "kdvtype.h"                                                                                            
#include "kdvdef.h"                                                                               
enum EmDataFormat                                                                           
{                                                                                           
	emStr  ,                                                                            
	emHex  ,                                                                            
	emBin  ,                                                                            
//	emBOOL ,                                                                            
	emOct  ,                                                                            
	emIP   ,                                                                            
	emDec  ,                                                                            
	emDW                                                                                
};                                                                                          
                                                                                            
                                                                                            
#define CALLRATE_64_SEL        0x00000001                                                   
#define CALLRATE_128_SEL       0x00000002                                                   
#define CALLRATE_192_SEL       0x00000004                                                   
#define CALLRATE_256_SEL       0x00000008                                                   
#define CALLRATE_320_SEL       0x00000010                                                   
#define CALLRATE_384_SEL       0x00000020                                                   
#define CALLRATE_448_SEL       0x00000040                                                   
#define CALLRATE_512_SEL       0x00000080                                                   
#define CALLRATE_576_SEL       0x00000100                                                   
#define CALLRATE_640_SEL       0x00000200                                                   
#define CALLRATE_704_SEL       0x00000400                                                   
#define CALLRATE_768_SEL       0x00000800                                                   
#define CALLRATE_832_SEL       0x00001000                                                   
#define CALLRATE_896_SEL       0x00002000                                                   
#define CALLRATE_960_SEL       0x00004000                                                   
#define CALLRATE_1024_SEL      0x00008000                                                   
#define CALLRATE_MASK          0x0000FFFF                                                   
                                                                                            
                                                                                             
//
// VOD Limited Condition 
//
#define VOD_DIRECTORY_SEPARATOR ( ( s8* ) "\\" )

const int VOD_MAX_CLIENT = 32; // 最多能连接的客户端数
const int VOD_MAX_USER_NAME_LENGTH = 32; // 用户名长度
const int VOD_MAX_PASSWORD_LENGTH = 32; // 密码长度
const int VOD_MAX_PATH_FILE_NAME_LENGTH = 128;  // 带路径的文件名长度
const int VOD_MAX_DIRECTIRY_NAME_LENGTH = 128; // 路径名长度
const int VOD_MAX_FILE_NAME_LENGTH = 50; // 文件名长度 
const int VOD_MAX_FILE_NUMBER = 64; // 同一文件夹下最多文件数                                                                                        
                                                                                            
                                                                                            
// 长度定义                                                                                 
const int    MT_MAX_NAME_LEN            =   32;                                                
const int    MT_MAX_H323ALIAS_LEN       =   32;            // 终端别名最大长度                 
const int    MT_MAX_E164NUM_LEN         =   16;            // 终端E164最大长度                 
//const int    MT_MAX_PORTNAME_LEN        =   16;            // 视频矩阵端口名最大长度           
const int    MT_MAX_AVMATRIX_PORT_NUM   =   64;            // 视频矩阵最大端口个数            
const int    MT_MAX_AVMATRIX_MODEL_NAME =   16; 
const int    MT_MAXLEN_AVMATRIX_SCHEMENAME  = 16;          //矩阵方案别名最大长度
const int    MT_MAX_AVMATRIX_PORTNAME_LEN = 16;            //矩阵端口名最大长度
const int    MT_MAX_COMM_LEN            =   32;            // SNMP共同体名最大长度             
const int    MT_MAX_PPPOE_USERNAME_LEN  =   16;            // PPPOE用户名最大长度              
const int    MT_MAX_CHANNEL_NAME_LEN    =   16;            // 流媒体频道名最大长度             
const int    MT_MAX_PASSWORD_LEN        =   16;            // 秘密最大长度                     
const int    MT_MAX_FULLFILENAME_LEN    =   128;           // 完整文件名最大长度               
const int    MT_MAX_FILEPATH_LEN        =   128;           // 路径名最大长度                   
const int    MT_MAX_CAMERA_NUM          =   6;             // 摄像头最大个数 
const int    MT_MAX_CAMERA_EX_NUM       =   64;            // 最大扩展摄像头数                  
const int    MT_MAX_CAMERA_NAME_LEN     =   16;            // 摄像头名字最大长度               
const int    MT_MAX_ETHNET_NUM          =   4;             // 终端以太网口最大值               
const int    MT_MAX_E1_NAME_LEN         =   32;            // E1名最大长度                     
const int    MT_MAX_E1UNIT_NUM          =   4;             // E1模块最多E1数                   
const int    MT_MAX_ROUTE_NUM           =   16;            // 最多路由条目                     
const int    MT_MAX_SERIAL_NUM          =   3 ;            //最多串口条目
const int    MT_MAX_VIDEO_NUM           =   2 ;            //最多视频路数
const int    MT_MAXNUM_VMP_MEMBER       =   16;            //画面合成成员数
const int    MT_MAXNUM_SMSDST           =   64;            //短消息发送目的成员最大值
const int    MT_MAXLEN_SMSTEXT          =   1024;          //短消息内容最大长度
const int    MT_MAX_IMAGEJUDGEPARAM_NUM =   2;             //画面调整参数表 
const int    MT_MAX_POLL_NUM		    =   128;    		//最大轮询终端数
const int    MT_MAX_ADDRENTRY_NUM	    =   128;           //地址簿最大条目个数
const int    MT_MAX_ADDRMULTISET_NUM    =   16;            //地址簿最大组个数
const int    MT_MAX_NETSND_DEST_NUM     =   5 ;            //双流盒最大发送的目的地数
const int    MT_MAX_CALLSITE_TERNINAL   =   16;              //最大发起的会议个数
const int    MT_MAX_ADDRUSERDEF_LEN     =   1024;          //地址簿用户自定义字段长度

//const int    MT_MAXNUM_VIDSOURCE        =   10;            //最大视频源数
const int    MT_MAXNUM_CALLADDR_RECORD  =   5;             //最大呼叫地址记录数

const int    MT_PPPoE_MAX_USENAME_LEN     =  255;            //PPPOE拨号时使用的用户名的最大长度
const int    MT_PPPoE_MAX_PASSWORD_LEN    =  255;            //PPPOE拨号时使用的密码的最大长度
const int    MT_PPPoE_MAX_SERVERNAME_LEN  =  255;            //PPPOE拨号时使用的指定ISP服务商名字的最大长度
const int    MT_PPPoE_MAX_VERSION_LEN     =  255;               //PPPOE版本信息的最大长度
const int    MT_MAX_UPDATASERVER_NAME_LEN =  64;              //升级服务器的域名的最大长度
const int    MT_MAX_LDAPSERVER_NAME_LEN   =  100;             //LDAP服务器域名的最大长度
const int    MT_MAX_PROXYSERVER_NAME_LEN  =  255;             //代理防火墙域名的最大长度
const int    MT_MAX_H323GKSERVER_NAME_LEN =  255;              //H323注册Gk域名的最大长度

//FILESRV: 升级服务器相关
const int    MT_MAX_FILESRV_BUG_REPORT_LEN   = 512;       //升级版本中的修复的bug描述0
const int    MT_MAX_FILESRV_FILENAME_LEN     = 64;        //最大的文件名长度
const int    MT_MAX_FILESRV_DEVFILE_NUM      = 4;         //最大的文件个数
const int    MT_MAX_FILESRV_SOFTWARE_VER_LEN = 32;        //软件版本号最大长度

const int    MT_MAX_PXYIP_NUM                =   5;            // 最多内置pxy支持的ip地址数   

//WiFi配置相关 
const int    MT_WIFI_MAX_NAME_LEN            =   32;
const int    MT_WIFI_MAX_PASSWORD_LEN        =   256;
const int    MT_WIFI_MAX_BSSIDCFG_NUM        =   16;  

//HD终端的视频输出口个数
const int    MT_HD_MAX_AV_OUTPORT_NUM  = 4;    //高清终端的最大视频输出口的个数


#define      MT_MAXNUM_MCTERMINAL        6    //终端MTC支持呼叫的最大终端数
#define      MT_MAXLEN_CAMERA_TYPE_NAME  32    //摄像头类型名最大长度                                                                                        
//==========================================================================================
//				任务相关常量定义                                            
//==========================================================================================
                                                                                            
////////////////////////////// 任务ID ////////////////////////////                          
const int    AID_MT_BEGIN        =   AID_MT_BGN;
const int    AID_MT_AGENT	     =   AID_MT_BEGIN + 1;    	//终端代理                          
const int    AID_MT_SERVICE      =   AID_MT_BEGIN + 2;        //终端业务应用ID                      
const int    AID_MT_GUARD	     = 	 AID_MT_BEGIN + 3;    	//终端守卫                          
const int    AID_MT_UE		     =	 AID_MT_BEGIN + 4;    	//业务接口实体                      
const int    AID_MT_STACKIN      =	 AID_MT_BEGIN + 5;    	//H323 协议栈输入                   
const int    AID_MT_STACKOUT	 =	 AID_MT_BEGIN + 6;    	//H323 协议栈输出                   
const int    AID_MT_RCP		     =	 AID_MT_BEGIN + 7;    	//MT RCP                            
const int    AID_MT_H320STACK    =   AID_MT_BEGIN + 8;        //H320 协议栈适配应用                    
const int    AID_MT_RCSIM        =	 AID_MT_BEGIN + 9;        //遥控器，模拟模块                  
const int    AID_MT_MC		     =	 AID_MT_BEGIN + 10;    	//MC                                
const int    AID_MT_SNMP	     =	 AID_MT_BEGIN + 11;        //SNMP                              
const int    AID_MT_CONFIG       =   AID_MT_BEGIN + 12;        //配置文件管理应用ID                  
const int    AID_MT_UI	         =	 AID_MT_BEGIN + 13;        //终端界面应用                
const int    AID_MT_DEVCTRL	     =	 AID_MT_BEGIN + 14;    	//设备控制                          
const int    AID_MT_CODEC	     =	 AID_MT_BEGIN + 15;    	//编解码                            
//const int    AID_ADDRBOOK	     =	 AID_MT_BEGIN + 16;    	//地址簿                            
const int    AID_MTCONSOLE	     =	 AID_MT_BEGIN + 17;    	//终端控制台 
const int    AID_MT_SYS          =   AID_MT_BEGIN + 18;        //终端系统应用ID                  
const int    AID_MT_H323SERVICE  =   AID_MT_BEGIN + 20;        //终端H323业务应用ID                  
const int    AID_MT_H320SERVICE  =   AID_MT_BEGIN + 21;        //终端H320业务应用ID                  
const int    AID_MT_SIPSERVICE   =   AID_MT_BEGIN + 22;        //终端SIP业务应用ID                                                                                            
const int    AID_MT_DVB          =   AID_MT_BEGIN + 23 ;        //终端双流盒业务应用ID
const int    AID_MT_DVBSSN       =   AID_MT_BEGIN + 24;        //双流盒会话应用
const int    AID_MT_VODCLIENT    =   AID_MT_BEGIN + 25;         //VOD客户端应用ID
const int    AID_MT_PCDUALVIDEO  =   AID_MT_BEGIN + 26;         //pc双流软件应用ID
const int    AID_MT_UPGRADE      =   AID_MT_BEGIN + 27;         //自动升级应用ID
const int    AID_MT_BATCHCONFIG  =   AID_MT_BEGIN + 28;         //配置文件批处理应用ID
const int    AID_MT_AUTOTEST     =   AID_MT_BEGIN + 29;         //终端硬件自动化测试应用ID   
const int    AID_MT_AUTOTESTCLIENT = AID_MT_BEGIN + 30;         // add by wangliang 2007-03-02
const int    AID_MT_REMOTESIM    =   AID_MT_BEGIN + 31;         // add by wangliang 2007-03-02
const int    AID_MT_THREADDETECT =   AID_MT_BEGIN + 32;         // add by qianshufeng 2008-01-07
//End

#define AID_MT_CTRL     AID_MT_SERVICE

#define AID_ADDRBOOK    46
////////////////////////////// 任务优先级 ////////////////////////////                      
                                                                                            
	                                                                                    
//==========================================================================================
//			     会议相关常量定义                                               
//==========================================================================================
                                                                                            
////////////////////////////// 消息结构常量 ////////////////////////////                    
                                                                                         
                                                                                            
#define     MT_MAX_CONF_ID_LEN 			16        //会议ID长度                         
#define     MT_MAX_CONF_NAME_LEN		64	      //会议名字长度                       
#define     MT_MAX_CONF_E164_LEN	 	16	      //会议号码长度                      
#define     MT_CONF_CAPACITY			192		  //会议终端容量      
#define     MT_MAXNUM_INVITEMT			32        //创建会议邀请的最大终端数
#define		MT_MAX_CONF_NUM				16		  //最大会议数

#define     MT_MAXLEN_ENCRYPTKEY    64 

const int   MT_MAXLEN_MESSAGE       =   1024*16;  //终端消息最大长度16K                                                                                            

#define     MT_MAXLEN_CONFGUID          16		  //会议guid最大长度
#define     MT_MAXLEN_PASSWORD          32		  //密码最大长度
#define     MT_NAME_LEN                 16		  //终端别名长度                      
//#define		CTRL_TERLABEL_UPPER		193			//无效的终端编号    
//#define     CTRL_TERMID_LEN			128			//会议终端ID长度


// 日志级别
#define		MT_LOG_ERROR		0
#define		MT_LOG_VITAL		1
#define		MT_LOG_WARNING		10
#define		MT_LOG_HINT			100
#define		MT_LOG_DETAIL		200
#define		MT_LOG_MAX			255

//定义mtmp错误码
//[xujinxing-2006-05-15]

#define   MTMP_NO_ERROR     (u16)0
#define   MTMP_ERROR_BASE   (u16)5000
#define   ERROR_DECODER_SNAPSHOT_FAIL_H261  (u16)(MTMP_ERROR_BASE + 1)
#define   ERROR_ENCODER_SNAPSHOT_FAIL_H261  (u16)(MTMP_ERROR_BASE + 2)

//定义跟mc_license相关的宏 
//[xujinxing-2006-10-13]

// License控制
#define MT_KEY_FILENAME                "kedalicense.key" // "mt.key"
#define MT_KEDA_AES_KEY				  "kedacom200610101"
#define MT_KEDA_COPYRIGHT			  "Suzhou Keda Technology Co.,ltd. Tel:86-512-68418188, Fax: 86-512-68412699, http://www.kedacom.com"
// 加密内容的长度
#define MT_LEN_KEYFILE_CONT              (u16)512

//add by xujinxing, telnet authority
#define MT_TEL_USRNAME  "admin"
#define MT_TEL_PWD      "admin"

#define MT_TELNET_PWD_MAX_LEN 32

/************************************************************************/
/* 终端硬件自动化测试 wl 2007/02/06
/************************************************************************/

// 自动化测试软件版本号
#define MT_AUTOTEST_VER 0x0100

// 测试失败原因
#define AUTOTEST_NACK_REASON_OK          0  //测试结果成功
#define AUTOTEST_NACK_REASON_HDERR       1  //硬件测试失败
#define AUTOTEST_NACK_REASON_TESTING     2  //测试正在进行
#define AUTOTEST_NACK_REASON_UNSUPPORT   3  //硬件不支持
#define AUTOTEST_NACK_REASON_STATUSERROR 4  //终端状态错误
#define AUTOTEST_NACK_REASON_VERERROR    5  //版本号错误

/* 自动化测试接口函数参数常量定义 */

// VLYNQ功能和性能诊断参数: 工作频率+工作状态+测试方案
const u8 AUTOTEST_VLYNQFUNC_FREQ_76 = 0;  // VLYNQ工作频率: 76MHZ
const u8 AUTOTEST_VLYNQFUNC_FREQ_99 = 1;  // VLYNQ工作频率: 99MHZ 

const u8 AUTOTEST_VLYNQFUNC_STATE_0 = 0;  // VLYNQ工作状态: 主模式
const u8 AUTOTEST_VLYNQFUNC_STATE_1 = 1;  // VLYNQ工作状态: 从模式

const u8 AUTOTEST_VLYNQFUNC_MODE_1  = 1;  // VLYNQ测试方案: 向对方DSP地址写递增数据
const u8 AUTOTEST_VLYNQFUNC_MODE_2  = 2;  // VLYNQ测试方案: 向对方DSP地址写递减数据
const u8 AUTOTEST_VLYNQFUNC_MODE_3  = 3;  // VLYNQ测试方案: 向自己DSP地址写递增数据
const u8 AUTOTEST_VLYNQFUNC_MODE_4  = 4;  // VLYNQ测试方案: 向自己DSP地址写递减数据
const u8 AUTOTEST_VLYNQFUNC_MODE_5  = 5;  // VLYNQ测试方案: 向自己DSP地址读递增数据
const u8 AUTOTEST_VLYNQFUNC_MODE_6  = 6;  // VLYNQ测试方案: 向自己DSP地址读递减数据
const u8 AUTOTEST_VLYNQFUNC_MODE_7  = 7;  // VLYNQ测试方案: 向对方DSP地址读递增数据
const u8 AUTOTEST_VLYNQFUNC_MODE_8  = 8;  // VLYNQ测试方案: 向对方DSP地址读递减数据

// EPLD寄存器读写诊断参数:  epld操作命令
const u8 AUTOTEST_EPLDFUNC_CMD_WRITECMD = 0;
const u8 AUTOTEST_EPLDFUNC_CMD_READCMD  = 1;

// 音频自环诊断参数: 采样率+音频数据格式
// 采样率使用时需要使用实际值
const u8 AUTOTEST_AUDIOFUNC_SAMPLERATE_8000  = 0; // 采样率: 8000
const u8 AUTOTEST_AUDIOFUNC_SAMPLERATE_32000 = 1; // 采样率: 32000
const u8 AUTOTEST_AUDIOFUNC_SAMPLERATE_44100 = 2; // 采样率: 44100
const u8 AUTOTEST_AUDIOFUNC_SAMPLERATE_48000 = 3; // 采样率: 48000

const u8 AUTOTEST_AUDIOFUNC_FORMAT_16 = 0; // 音频数据格式: S16_LE
const u8 AUTOTEST_AUDIOFUNC_FORMAT_32 = 1; // 音频数据格式: S32_LE

// 视频自环诊断参数: 输出格式+OSD显示+画中画显示

const u8 AUTOTEST_VIDEOFUNC_OUTPUT_0  = 0;  // 输出格式: D1
const u8 AUTOTEST_VIDEOFUNC_OUTPUT_1  = 1;  // 输出格式: CIF 
const u8 AUTOTEST_VIDEOFUNC_OUTPUT_2  = 2;  // 输出格式: ZOOM 
const u8 AUTOTEST_VIDEOFUNC_OUTPUT_3  = 3;  // 输出格式: VGA60 
const u8 AUTOTEST_VIDEOFUNC_OUTPUT_4  = 4;  // 输出格式: VGA75 
const u8 AUTOTEST_VIDEOFUNC_OUTPUT_5  = 5;  // 输出格式: VGA85
const u8 AUTOTEST_VIDEOFUNC_OUTPUT_6  = 6;  // 输出格式: SVGA60 
const u8 AUTOTEST_VIDEOFUNC_OUTPUT_7  = 7;  // 输出格式: SVGA75
const u8 AUTOTEST_VIDEOFUNC_OUTPUT_8  = 8;  // 输出格式: SVGA85
const u8 AUTOTEST_VIDEOFUNC_OUTPUT_9  = 9;  // 输出格式: XGA60 
const u8 AUTOTEST_VIDEOFUNC_OUTPUT_10 = 10; // 输出格式: XGA75

const u8 AUTOTEST_VIDEOFUNC_OSD_0 = 0; // OSD显示: 不支持
const u8 AUTOTEST_VIDEOFUNC_OSD_1 = 1; // OSD显示: 支持

const u8 AUTOTEST_VIDEOFUNC_PIP_0 = 0; // 画中画显示: 不支持
const u8 AUTOTEST_VIDEOFUNC_PIP_1 = 1; // 画中画显示: 支持

//////////////////////////////////////////////////////////////////////
/*HD Macro definition BEGIN*/
#define HD_PATCH_VER_A              0
#define HD_PATCH_VER_B              1

#define HD_SCREENMODE_SIMPLE        0
#define HD_SCREENMODE_NORMAL        1
#define HD_SCREENMODE_ADVANCE       2


/*WIFI Macro definition BEGIN*/
#define WIFI_MAX_BSS_NUM            16
#define WIFI_MAX_KEY_LEN            64
#define WIFI_MAX_NAME_LEN           64

#define WIFI_MAX_CFG_NUM            16

#define WIFI_NET_TYPE_INFRA          0
#define WIFI_NET_TYPE_ADHOC          1

#define WIFI_AUTH_TYPE_OPEN          0
#define WIFI_AUTH_TYPE_SHARE         1
#define WIFI_AUTH_TYPE_WPA_PSK       2
#define WIFI_AUTH_TYPE_WPA2_PSK      3

#define WIFI_ENCRYTO_TYPE_WEP        0
#define WIFI_ENCRYTO_TYPE_AES        1
#define WIFI_ENCRYTO_TYPE_TKIP       2
/*WIFI Macro definition END*/
#endif //!_MT_CONST_H_


