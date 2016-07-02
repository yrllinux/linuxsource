/*****************************************************************************
模块名	：
文件名	：Connon.h
相关文件	：
文件实现功能：相关的宏定义，消息定义
作者		：
版本		：
---------------------------------------------------------------------------------------------------------------------
修改记录:
日  期		版本		修改人		走读人    修改内容
04/10/2007	1.0          某某        某某      ------------
******************************************************************************/


#ifndef COMMON_H
#define COMMON_H

#include "osp.h"

//最大工序数
#define PT_PROCEDURE_MAX		(u8)16
#define PT_MAX_NO_PER_PROCEDURE   (u8)6

//可连接的最大生产测试控制台数
#define PT_CONFIG_MAX		(u8)1

//可连接的最大生产测试操作台数（工序总数）
#define PT_PROCEDURE_TOTAL_MAX		(u16)PT_PROCEDURE_MAX*PT_MAX_NO_PER_PROCEDURE

//创建结点端口号
#define PT_SERVRT_PORT     (u16)70000

//服务器APP
#define PT_SERVER_CONTROL_APP           (u16)1
#define PT_SERVER_CONTROL_INS_NUM       (u16)1
#define PT_SERVER_PROCEDURE_APP         (u16)2
#define PT_SERVER_PROCEDURE_INS_NUM     (u16)PT_PROCEDURE_TOTAL_MAX
#define PT_SERVER_CONFIG_APP            (u16)3
#define PT_SERVER_CONFIG_INS_NUM        (u16)1
#define PT_SERVER_APP_PRI               (u8)100

//操作台APP
#define PT_TEST_APP		(u16)4
#define PT_TEST_INS_NUM	(u16)1

//控制台APP
#define PT_CONFIG_APP		(u16)5
#define PT_CONFIG_INS_NUM	(u16)1


#define PT_MAX_SERIAL_LENGTH		(u8)32		//序列号的最大长度
#define PT_MAX_MAC_LENGTH			(u8)20		//MAC地址最大长度
#define PT_MAX_IP_LENGTH			(u8)16		//IP地址最大长度
#define PT_MAX_EMPLOYEENO_LENGTH	(u8)32		//工号最大长度
#define PT_MAX_DATETIME_LENGTH		(u8)20		//日期时间最大长度
#define PT_MAX_DATE_LENGTH		    (u8)12		//日期最大长度
#define PT_MAX_FILE_NAME_LENGTH	    (u8)32		//文件名最大长度
#define PT_MAX_NAME_LENGTH			(u8)32		//名称最大长度
#define PT_MAX_PATH_LENGTH          (u8)200		//路径最大长度
#define PT_MAX_SQL_LENGTH           (u16)1000	//sql语句最大长度
#define PT_MAX_TEST_ITEM            (u16)100	//测试项最大值
#define PT_MAX_DESC_LENGTH          (u8)32		//描述信息的长度
#define PT_MAX_DEVICE_TYPE          (u8)20		//最大设备类型数
#define PT_MAX_DEVTYPELEN           (u8)10      //设备类型的最大描述长度
#define PT_MAX_FILE_TYPE            (u8)8		//最大文件类型数
#define PT_RECORDNUM_PER_PACK       (u32)100    //传输测试结果的最大记录数
//测试类型定义
#define		TEST_TYPE_FXO               1			//FXO环路测试
#define		TEST_TYPE_SERIAL            2		    //串口测试
#define		TEST_TYPE_ETH               3			//网口功能测试
#define		TEST_TYPE_USB               4			//USB功能测试
#define		TEST_TYPE_AUDIO             5		    //音频环回功能测试
#define		TEST_TYPE_C1VGA             6		    //视频环回外置VGA输出功能测试
#define		TEST_TYPE_C1C               7           //视频环回外置C端子输出功能测试
#define		TEST_TYPE_C2VGA  	        8           //视频环回内置VGA输出功能测试
#define		TEST_TYPE_C2C  	    	    9           //视频环回内置C端子输出功能测试
#define		TEST_TYPE_LOUDSPEAKER       10	        //扬声器功能测试
#define		TEST_TYPE_MIC               11			//MIC功能测试
#define		TEST_TYPE_CAMERAROTA        12	        //摄像头转动测试
#define		TEST_TYPE_RTC               13			//RTC功能测试
#define		TEST_TYPE_LIGHT             14		    //指示灯测试
#define		TEST_TYPE_REMOTECTRL        15	        //红外接口功能测试
#define     TEST_TYPE_MAX               15

//文件类型定义
#define  PT_FILE_TYPE_SYS          1//sys文件类型
#define  PT_FILE_TYPE_MAX          1

//消息长度相关
#define      PT_SERV_MSG_LEN					0x7000  //消息长度
#define      PT_SERV_MSGHEAD_LEN				28      //消息头长度

//定义异常状态
#define PT_ERROR_OSP_DISCONNECT		(s32)1			//与设备的连接断了

#define PT_TIMEOUT_SECOND           (u16)6          //设定超时时间
/*****************************testClient<-->DBServer****************************/
//定义消息类型  testClient-->DBServer
//testClient发送登陆请求到DBServer，消息体为 ITestLoginRequest
OSPEVENT( EV_TESTCLIENT_TO_DBSERVER_MSGTYPE_LOGIN_REQ,		1 );

//testClient发送消息到DBServer获取配置信息，消息体为 无
OSPEVENT( EV_TESTCLIENT_TO_DBSERVER_MSGTYPE_CONFIG_REQ,	2 );

//testClient从DBServer获取分配给设备的IP、MAC，消息体为序列号 IDeviceAssign
OSPEVENT( EV_TESTCLIENT_TO_DBSERVER_MSGTYPE_IPMAC_REQ,	4 );

//testClient发送测试结果到DBServer，消息体为 IAutoTestResult
OSPEVENT( EV_TESTCLIENT_TO_DBSERVER_MSGTYPE_RESULT_SEND,	5 );


//定义消息类型  DBServer-->testClient
//DBServer发送登陆应答成功消息到testClient，消息体为 无
OSPEVENT( EV_DBSERVER_TO_TESTCLIENT_MSGTYPE_LOGIN_ACK,	1 );

//DBServer发送登陆应答失败消息到testClient，消息体为 ITestLoginNAck
OSPEVENT( EV_DBSERVER_TO_TESTCLIENT_MSGTYPE_LOGIN_NACK,	2 );

//DBServer发送应答客户端的配置信息请求到testClient，消息体为 TPTTestItem+IDeviceInfo
OSPEVENT( EV_DBSERVER_TO_TESTCLIENT_MSGTYPE_CONFIG_ACK,	3 );

//DBServer发送应答客户端的IPMAC请求到testClient，消息体为 IDeviceAssign
OSPEVENT( EV_DBSERVER_TO_TESTCLIENT_MSGTYPE_IPMAC_ACK,	4 );

 /***********************END testClient<-->DBServer****************************/



/*****************************configClient<-->DBServer****************************/
//定义消息类型  configClient-->DBServer
//configClient发送登陆/登出请求到DBServer，消息体为 IConfigLoginRequest
OSPEVENT( EV_CONFIGCLIENT_TO_DBSERVER_MSGTYPE_LOGIN_REQ,		1 );

//configClient发送配置信息到DBServer，消息体为 IServerConfigInfo
OSPEVENT( EV_CONFIGCLIENT_TO_DBSERVER_MSGTYPE_CONFIG_SET,	3 );

//configClient发送消息到DBServer，请求当前测试信息，消息体为 无
OSPEVENT( EV_CONFIGCLIENT_TO_DBSERVER_MSGTYPE_TESTINFO_QUERY,	4 );

//configClient发送消息到DBServer，请求当前配置信息，消息体为 无
OSPEVENT( EV_CONFIGCLIENT_TO_DBSERVER_MSGTYPE_CONFIG_QUERY,	5 );

//configClient发送消息到DBServer，请求符合条件的测试结果，消息体为 IQueryTestResultCondition
OSPEVENT( EV_CONFIGCLIENT_TO_DBSERVER_MSGTYPE_RESULT_QUERY,	6 );

//configClient发送消息到DBServer，请求初始化数据，消息体为 无
OSPEVENT( EV_CONFIGCLIENT_TO_DBSERVER_MSGTYPE_INITDATA_QUERY,	7 );

//定义消息类型  DBServer-->configClient
//DBServer发送登陆应答成功消息到configClient，消息体为 无
OSPEVENT( EV_DBSERVER_TO_CONFIGCLIENT_MSGTYPE_LOGIN_ACK,	1 );

//DBServer发送登陆应答失败消息到configClient，消息体为 ITestLoginNAck
OSPEVENT( EV_DBSERVER_TO_CONFIGCLIENT_MSGTYPE_LOGIN_NACK,	2 );

//configClient发送配置信息到DBServer，消息体为 IConfigSetAck
OSPEVENT( EV_DBSERVER_TO_CONFIGCLIENT_MSGTYPE_CONFIG_SET_ACK,	3 );

//DBServer发送应答测试信息到configClient，消息体为 IServerInfo
OSPEVENT( EV_DBSERVER_TO_CONFIGCLIENT_MSGTYPE_TESTINFO_ACK,	4 );

//DBServer发送应答配置信息到configClient，消息体为 IServerConfigInfo
OSPEVENT( EV_DBSERVER_TO_CONFIGCLIENT_MSGTYPE_CONFIG_QUERY_ACK,	5 );

//DBServer发送应答测试结果到configClient，消息体为 TTestNote  长度为sizeof(TTestNote)*n
OSPEVENT( EV_DBSERVER_TO_CONFIGCLIENT_MSGTYPE_RESULT_QUERY_ACK,	6 );

//DBServer发送应答初始化数据到configClient，消息体为 IInitConsoleInfo
OSPEVENT( EV_DBSERVER_TO_CONFIGCLIENT_MSGTYPE_INITDATA_QUERY_ACK,	7 );

/**************************END configClient<-->DBServer****************************/

//测试操作台登陆结构体
typedef struct tagTestLoginRequest
{
    private:
	    s8	m_szEmployeeNo[PT_MAX_EMPLOYEENO_LENGTH];	//工号
	    s8  m_szPassword[PT_MAX_NAME_LENGTH];	        //密码(暂时没有密码认证)
        u8  m_byProcedure;                              //工序(测试软件操作台登陆时附加的信息)
        u8  m_byState;                                  //状态 1 登陆  0登出
        u8  padding[2];//填充位
    public:
        tagTestLoginRequest()
        {
            memset(m_szEmployeeNo,0,PT_MAX_NAME_LENGTH);
            memset(m_szPassword,0,PT_MAX_NAME_LENGTH);
            m_byProcedure = 0;
            memset(padding,0,2);
        }

        void setEmployeeNo(s8 * szNo){ strcpy(m_szEmployeeNo, szNo);}
        s8 * getEmployeeNo(){ return m_szEmployeeNo;}
        
        void setPassword(s8 * szPassword){ strcpy(m_szPassword, szPassword);}
        s8 * getPassword(){ return m_szPassword;}

        void setProcedure(u8 byProcedure){ m_byProcedure = byProcedure;}
        u8 getProcedure(){ return m_byProcedure;}

        void setState(u8 byState){ m_byState = byState;}
        u8 getState(){ return m_byState;}
        
}ITestLoginRequest;

//配置控制台登陆结构体
typedef ITestLoginRequest IConfigLoginRequest;

//登陆应答失败结构体
typedef struct tagTestLoginNAck
{
    private:
        u8 m_byReason;//登陆失败的原因1 用户名密码错误 2 登陆用户满 3 服务器连接数据库失败
        u8 padding[3];//填充位
    public:
        tagTestLoginNAck()
        {
            m_byReason = 0;
            memset(padding,0,3);
        }

        void setReason(u8 byReason){ m_byReason = byReason;}
        u8 getReason(){ return m_byReason;}
}ITestLoginNAck;

//测试人员登陆信息
typedef struct tagTestLoginInfo 
{
	private:
		u32 m_dwIP;//操作台IP
	public:
		ITestLoginRequest m_ILoginReq;
		
		tagTestLoginInfo()
		{
			m_dwIP = 0;
		}
		
		void setIP(u32 dwIP){ m_dwIP = dwIP;}
		u32 getIP(){ return m_dwIP;}
}ITestLoginInfo;

//配置设置应答结构体
typedef struct tagConfigSetAck
{
    private:
        u8 m_byResult;//结果 1 成功 0 失败
        u8 m_byReason;//设置失败的原因1 操作数据库失败 2 正在测试中，不能修改配置
        u8 padding[2];//填充位
    public:
		ITestLoginInfo m_ILoginInfo[5];//正在测试中的人员信息，最多5个，再多已没意义

        tagConfigSetAck()
        {
            m_byResult = 0;
            m_byReason = 0;
            memset(padding,0,2);
        }

        void setResult(u8 byResult){ m_byResult = byResult;}
        u8 getResult(){ return m_byResult;}

        void setReason(u8 byReason){ m_byReason = byReason;}
        u8 getReason(){ return m_byReason;}
}IConfigSetAck;

//测试项
typedef struct tagPTTestItem
{
    public:
        u8      m_byTestItem[PT_MAX_TEST_ITEM+1];//下标表示测试项,其中0表示加载
		u8		padding[3];//填充位

        tagPTTestItem()
        {
            memset(m_byTestItem,0,PT_MAX_TEST_ITEM+1);
			memset(padding,0,3);
        }
}TPTTestItem;

//日期时间结构
typedef struct tagDateTime
{
	private:
		u16 m_wYear;	//年
		u8 m_byMonth;	//月
		u8 m_byDay;		//日
		u8 m_byHour;	//时
		u8 m_byMinute;	//分
		u8 m_bySecond;	//秒
		u8 padding;//填充位
	public:
		tagDateTime()
		{
			m_wYear = htons(2007);
			m_byMonth = 1;
			m_byDay = 1;
			m_byHour = 0;
			m_byMinute = 0;
			m_bySecond = 0;
			padding = 0;
		}

		void setYear(u16 wYear){ m_wYear = htons(wYear);}
        u16 getYear(){ return ntohs(m_wYear);}

		void setMonth(u8 byMonth){ m_byMonth = byMonth;}
        u8 getMonth(){ return m_byMonth;}

		void setDay(u8 byDay){ m_byDay = byDay;}
        u8 getDay(){ return m_byDay;}

		void setHour(u8 byHour){ m_byHour = byHour;}
        u8 getHour(){ return m_byHour;}

		void setMinute(u8 byMinute){ m_byMinute = byMinute;}
        u8 getMinute(){ return m_byMinute;}

		void setSecond(u8 bySecond){ m_bySecond = bySecond;}
        u8 getSecond(){ return m_bySecond;}
}IDateTime;

//测试结果查询条件
typedef struct tagQueryTestResultCondition
{
    private:
        u8 m_byCondition;			//0 查询全部结果，  1 根据设备类型查询  2 根据测试结果查询 3 根据设备类型和测试结果组合查询
		u8 m_byTestResult;			//测试结果 1 测试成功 2 测试失败
		u8 padding[2];//填充位
        u32	m_dwDeviceType;			//设备类型
		u32 m_dwStart;				//当前查询起始记录号，从1开始
        u32 m_dwCount;				//查询结果的数量
    public:
		IDateTime m_IStartDateTime;//查询起始时间
		IDateTime m_IEndDateTime;//查询结束时间

        tagQueryTestResultCondition()
        {
            m_byCondition = 0;
			memset(padding,0,2);
            m_dwDeviceType = 0;
			m_byTestResult = 0;
            m_dwStart = 0;
            m_dwCount = 0;
        }

        void setCondition(u8 byCondition){ m_byCondition = byCondition;}
        u8 getCondition(){ return m_byCondition;}

		void setTestResult(u8 byTestResult){ m_byTestResult = byTestResult;}
        u8 getTestResult(){ return m_byTestResult;}
		
		u8 getOpositTestResult()
		{
			if(1 == getTestResult())
			{
				return 2;
			}
			else if(2 == getTestResult())
			{
				return 1;
			}

		}
		
        void setDeviceType(u32 dwDeviceType){ m_dwDeviceType = htonl(dwDeviceType);}
        u32 getDeviceType(){ return ntohl(m_dwDeviceType);}

        void setStart(u32 dwStart){ m_dwStart = htonl(dwStart);}
        u32 getStart(){ return ntohl(m_dwStart);}

        void setCount(u32 dwCount){ m_dwCount = htonl(dwCount);}
        u32 getCount(){ return ntohl(m_dwCount);}
}IQueryTestResultCondition;
// 测试结果记录结构体
typedef struct tagTestNote
{
    private:
        s8		m_szSerial[PT_MAX_SERIAL_LENGTH];	    //序列号
        s8	    m_szIP[PT_MAX_IP_LENGTH];		        //IP
        s8	    m_szMACOne[PT_MAX_MAC_LENGTH];		    //第一网口MAC
        s8	    m_szMACTwo[PT_MAX_MAC_LENGTH];		    //第二网口MAC
        s8		m_szDateTime[PT_MAX_DATETIME_LENGTH];	//测试日期时间 格式: "2007-06-14 14-45-45\0"
        u32		m_dwDeviceType;				        //设备类型
    public:
        TPTTestItem	m_TTestItem;				    //0 未测  1 通过   2 不通过

        tagTestNote()
        {
            memset(m_szSerial,0,PT_MAX_SERIAL_LENGTH);
            memset(m_szIP,0,PT_MAX_IP_LENGTH);
            memset(m_szMACOne,0,PT_MAX_MAC_LENGTH);
            memset(m_szMACTwo,0,PT_MAX_MAC_LENGTH);
            memset(m_szDateTime,0,PT_MAX_DATETIME_LENGTH);
            m_dwDeviceType = 0;
        }

        void setSerial(s8 * szSerial){ strcpy(m_szSerial, szSerial);}
        s8 * getSerial(){ return m_szSerial;}

        void setIP(s8 * szIP){ strcpy(m_szIP, szIP);}
        s8 * getIP(){ return m_szIP;}

        void setMACOne(s8 * szMAC){ strcpy(m_szMACOne, szMAC);}
        s8 * getMACOne(){ return m_szMACOne;}

        void setMACTwo(s8 * szMAC){ strcpy(m_szMACTwo, szMAC);}
        s8 * getMACTwo(){ return m_szMACTwo;}

        void setDateTime(s8 * szDateTime){ strcpy(m_szDateTime, szDateTime);}
        s8 * getDateTime(){ return m_szDateTime;}

        void setDeviceType(u32 dwDeviceType){ m_dwDeviceType = htonl(dwDeviceType);}
        u32 getDeviceType(){ return ntohl(m_dwDeviceType);}
}TTestNote;

// 测试结果消息结构
typedef struct tagAutoTestResult
{
    private:
        //测试状态0 正常，1异常（指设备死机，网线未插好引起的设备掉线等情况，某项测试未通过不算异常）
        u8	    m_byTestState;
        u8      m_byIsLoad;                         //0 测试  1 加载
        u8		m_byProcedure;						//工序
        u8      padding;//填充位
    public:
        TTestNote m_tTestNote;

        tagAutoTestResult()
        {
            m_byTestState = 0;
            m_byIsLoad = 0;
            m_byProcedure = 0;
            padding = 0;
        }

        void setTestState(u8 byTestState){ m_byTestState = byTestState;}
        u8 getTestState(){ return m_byTestState;}

        void setIsLoad(u8 byIsLoad){ m_byIsLoad = byIsLoad;}
        u8 getIsLoad(){ return m_byIsLoad;}

        void setProcedure(u8 byProcedure){ m_byProcedure = byProcedure;}
        u8 getProcedure(){ return m_byProcedure;}
}IAutoTestResult;

//给设备分配IP、MAC的结构体
typedef struct tagDeviceAssign
{
    private:
        s8  m_szSerial[PT_MAX_SERIAL_LENGTH];	    //序列号
        s8	m_szIP[PT_MAX_IP_LENGTH];		    //IP
        s8	m_szMACOne[PT_MAX_MAC_LENGTH];	    //第一个网口的MAC
        s8  m_szMACTwo[PT_MAX_MAC_LENGTH];     //第二个网口的MAC
    public:
        tagDeviceAssign()
        {
            memset(m_szSerial,0,PT_MAX_SERIAL_LENGTH);
            memset(m_szIP,0,PT_MAX_IP_LENGTH);
            memset(m_szMACOne,0,PT_MAX_MAC_LENGTH);
            memset(m_szMACTwo,0,PT_MAX_MAC_LENGTH);
        }

        void setSerial(s8 * szSerial){ strcpy(m_szSerial, szSerial);}
        s8 * getSerial(){ return m_szSerial;}

        void setIP(s8 * szIP){ strcpy(m_szIP, szIP);}
        s8 * getIP(){ return m_szIP;}

        void setMACOne(s8 * szMACOne){ strcpy(m_szMACOne, szMACOne);}
        s8 * getMACOne(){ return m_szMACOne;}

        void setMACTwo(s8 * szMACTwo){ strcpy(m_szMACTwo, szMACTwo);}
        s8 * getMACTwo(){ return m_szMACTwo;}
}IDeviceAssign;

//加载的文件名
typedef struct tagLoadFileName
{
    private:
        u8 m_byFileType;//文件类型
		u8 padding[3];//填充位
        s8 m_szName[PT_MAX_NAME_LENGTH];//文件名称
    public:
        tagLoadFileName()
        {
            m_byFileType = 0;
            memset(m_szName,0,PT_MAX_NAME_LENGTH);
			memset(padding,0,3);
        }
        
        void setFileType(u8 byFileType){ m_byFileType = byFileType;}
        u8 getFileType(){ return m_byFileType;}
        
        void setName(s8 * szName){ strcpy(m_szName,szName);}
        s8 * getName(){ return m_szName;}
}ILoadFileName;

//设备信息结构体
typedef struct tagDeviceInfo
{
    private:
        s8	m_szFirstIP[PT_MAX_IP_LENGTH];		    //起始IP
        s8	m_szFirstMACOne[PT_MAX_MAC_LENGTH];	    //第一个网口的起始MAC
        s8  m_szFirstMACTwo[PT_MAX_MAC_LENGTH];     //第二个网口的起始MAC
        s8  m_szServerPath[PT_MAX_PATH_LENGTH];     //服务器端测试结果保存的目录,默认是保存在C盘根目录
        s8  m_szOperatorPath[PT_MAX_PATH_LENGTH];   //操作台端测试结果保存的目录,默认是保存在C盘根目录(暂时不用)
        s8	m_szDate[PT_MAX_DATE_LENGTH];		    //设备的生产日期 格式: "2007-06-14\0"
        u32 m_dwCount;						    //本次测试的设备总数
        u32	m_dwDeviceType;					    //设备类型
        u8	m_bySerialLength;					//序列号位数
        u8	m_byHardwareVersion;				//硬件版本
        u8	m_byE2promInfo;					    //E2prom信息
        u8	m_byE2promFlag;					    //E2prom标志
        u8  m_byAbort;                          //当遇到异常的情况 1--继续下一项测试 0--终止测试 默认是0
        u8  m_byTestDelay;                      //每一项测试结束后的延迟，方便操作人员观察测试结果
		u8  m_byResume;							//是否恢复默认出厂设置，1 恢复 0 不恢复
        u8  padding;//填充位
    public:
        ILoadFileName m_ILoadFile[PT_MAX_FILE_TYPE];    //待加载的文件(此文件信息现在不能配置，只能查询)

        tagDeviceInfo()
        {
            memset(m_szFirstIP,0,PT_MAX_IP_LENGTH);
            memset(m_szFirstMACOne,0,PT_MAX_MAC_LENGTH);
            memset(m_szFirstMACTwo,0,PT_MAX_MAC_LENGTH);
            memset(m_szServerPath,0,PT_MAX_PATH_LENGTH);
            memset(m_szOperatorPath,0,PT_MAX_PATH_LENGTH);
            memset(m_szDate,0,PT_MAX_DATE_LENGTH);
            m_dwCount = 0;
            m_dwDeviceType = 0;
            m_bySerialLength = 0;
            m_byHardwareVersion = 0;
            m_byE2promInfo = 0;
            m_byE2promFlag = 0;
            m_byAbort = 0;
            m_byTestDelay = 2;
			m_byResume = 0;
            padding = 0;
        }

		s8 * getLoadFile(u8 byFileType)
		{
			static s8 buf[2] = "";
			for(int i=0; i<PT_MAX_FILE_TYPE; i++)
			{
				if(m_ILoadFile[i].getFileType() == byFileType)
				{
					return m_ILoadFile[i].getName();
				}
			}
			return buf;
		}

        void setFirstIP(s8 * szFirstIP){ strcpy(m_szFirstIP, szFirstIP);}
        s8 * getFirstIP(){ return m_szFirstIP;}

        void setFirstMACOne(s8 * szFirstMACOne){ strcpy(m_szFirstMACOne, szFirstMACOne);}
        s8 * getFirstMACOne(){ return m_szFirstMACOne;}

        void setFirstMACTwo(s8 * szFirstMACTwo){ strcpy(m_szFirstMACTwo, szFirstMACTwo);}
        s8 * getFirstMACTwo(){ return m_szFirstMACTwo;}

        void setServerPath(s8 * szPath){ strcpy(m_szServerPath, szPath);}
        s8 * getServerPath(){ return m_szServerPath;}

        void setOperatorPath(s8 * szPath){ strcpy(m_szOperatorPath, szPath);}
        s8 * getOperatorPath(){ return m_szOperatorPath;}

        void setDate(s8 * szDate){ strcpy(m_szDate, szDate);}
        s8 * getDate(){ return m_szDate;}

        void setCount(u32 dwCount){ m_dwCount = htonl(dwCount);}
        u32 getCount(){ return ntohl(m_dwCount);}

        void setSerialLength(u8 bySerialLength){ m_bySerialLength = bySerialLength;}
        u8 getSerialLength(){ return m_bySerialLength;}

        void setDeviceType(u32 dwDeviceType){ m_dwDeviceType = htonl(dwDeviceType);}
        u32 getDeviceType(){ return ntohl(m_dwDeviceType);}

        void setHardwareVersion(u8 byHardwareVersion){ m_byHardwareVersion = byHardwareVersion;}
        u8 getHardwareVersion(){ return m_byHardwareVersion;}

        void setE2promInfo(u8 byE2promInfo){ m_byE2promInfo = byE2promInfo;}
        u8 getE2promInfo(){ return m_byE2promInfo;}

        void setE2promFlag(u8 byE2promFlag){ m_byE2promFlag = byE2promFlag;}
        u8 getE2promFlag(){ return m_byE2promFlag;}

        void setAbort(u8 byAbort){ m_byAbort = byAbort;}
        u8 getAbort(){ return m_byAbort;}

        void setTestDelay(u8 byTestDelay){ m_byTestDelay = byTestDelay;}
        u8 getTestDelay(){ return m_byTestDelay;}

		void setResume(u8 byResume){ m_byResume = byResume;}
        u8 getResume(){ return m_byResume;}
}IDeviceInfo;

//服务器信息配置结构体
typedef struct tagServerConfigInfo
{
    private:
        u8   m_byProcedureMax;					            //设置的最大工序数
        u8   padding[3];//填充位
    public:
        TPTTestItem m_IProcedureInfo[PT_PROCEDURE_MAX];		//工序配置信息
        IDeviceInfo   m_IDeviceInfo;                        //设备信息

        tagServerConfigInfo()
        {
            m_byProcedureMax = 0;
            memset(padding,0,3);
        }

        void setProcedureMax(u8 byProcedureMax){ m_byProcedureMax = byProcedureMax;}
        u8 getProcedureMax(){ return m_byProcedureMax;}
}IServerConfigInfo;

//服务器信息（暂未使用）
typedef struct tagServerInfo
{
    //BOOL m_bDBState;        //数据库状态
}IServerInfo;

//设备初始化信息
typedef struct tagDeviceTypeDesc
{
    private:
        u32 m_dwDeviceType;//设备类型
        s8  m_szDesc[PT_MAX_DESC_LENGTH];//设备描述
    public:
        u8  m_byFileType[PT_MAX_FILE_TYPE];//设备的文件加载信息0 不要加载 1要加载
        TPTTestItem m_ITestItemInfo; //设备的测试项信息0 不测 1 要测

        tagDeviceTypeDesc()
        {
            m_dwDeviceType = 0;
            memset(m_szDesc,0,PT_MAX_DESC_LENGTH);
            memset(m_byFileType,0,PT_MAX_FILE_TYPE);
        }

        void setDeviceType(u32 dwDeviceType){ m_dwDeviceType = dwDeviceType;}
        u32 getDeviceType(){ return m_dwDeviceType;}

        void setDesc(s8 * szDesc){ strcpy(m_szDesc,szDesc);}
        s8 * getDesc(){ return m_szDesc;}
}IDeviceTypeDesc;

//测试项信息
typedef struct tagTestItemDesc
{
    private:
        u8 m_byTestItem;//测试项
		u8 padding[3];//填充位
        s8 m_szDesc[PT_MAX_DESC_LENGTH];//测试项描述
    public:
        tagTestItemDesc()
        {
            m_byTestItem = 0;
			memset(padding,0,3);
            memset(m_szDesc,0,PT_MAX_DESC_LENGTH);
        }

        void setTestItem(u8 byTestItem){ m_byTestItem = byTestItem;}
        u8 getTestItem(){ return m_byTestItem;}
    
        void setDesc(s8 * szDesc){ strcpy(m_szDesc,szDesc);}
        s8 * getDesc(){ return m_szDesc;}
}ITestItemDesc;

//加载文件信息
typedef struct tagFileTypeDesc
{
    private:
        u8 m_byFileType;//文件类型
		u8 padding[3];//填充位
        s8 m_szDesc[PT_MAX_DESC_LENGTH];//文件描述
    public:
        tagFileTypeDesc()
        {
            m_byFileType = 0;
            memset(m_szDesc,0,PT_MAX_DESC_LENGTH);
			memset(padding,0,3);
        }

        void setFileType(u8 byFileType){ m_byFileType = byFileType;}
        u8 getFileType(){ return m_byFileType;}
        
        void setDesc(s8 * szDesc){ strcpy(m_szDesc,szDesc);}
        s8 * getDesc(){ return m_szDesc;}
}IFileTypeDesc;

//控制台的初始化信息结构体
typedef struct tagInitConsoleInfo
{
    private:
        u8 m_byDeviceTypeNum;//设备类型数
        u8 m_byTestItemMax;//最大测试项类型 测试项必须从1开始累加
        u8 m_byFileTypeMax;//最大文件类型 文件类型必须从1开始累加
		u8 padding;//填充位
    public:
        IDeviceTypeDesc m_IDevice[PT_MAX_DEVICE_TYPE];
        ITestItemDesc   m_ITestItem[PT_MAX_TEST_ITEM];
        IFileTypeDesc   m_IFileType[PT_MAX_FILE_TYPE];
        IServerConfigInfo m_IConfigInfo;

        tagInitConsoleInfo()
        {
            m_byDeviceTypeNum = 0;
            m_byTestItemMax = 0;
            m_byFileTypeMax = 0;
			padding = 0;
        }

        void setDeviceTypeNum(u8 byDeviceTypeNum){ m_byDeviceTypeNum = byDeviceTypeNum;}
        u8 getDeviceTypeNum(){ return m_byDeviceTypeNum;}

        void setTestItemMax(u8 byTestItemMax){ m_byTestItemMax = byTestItemMax;}
        u8 getTestItemMax(){ return m_byTestItemMax;}

        void setFileTypeMax(u8 byFileTypeMax){ m_byFileTypeMax = byFileTypeMax;}
        u8 getFileTypeMax(){ return m_byFileTypeMax;}
}IInitConsoleInfo;


//业务消息类，最大处理28K长度消息
class CPTServMsg
{
protected:
	u16     m_wSerialNO;            //流水号
	u16		m_wEventId;             //事件号
	u16		m_wErrorCode;           //错误码
	u16		m_wMsgBodyLen;          //消息体长度
    u32     m_byTotalPktNum;        //总包数（用于需要切包发送的消息）
    u32     m_byCurPktIdx;          //当前包索引（从0开始）
	u8      m_abyReserverd[12];     //保留字节
	u8      m_abyMsgBody[PT_SERV_MSG_LEN-PT_SERV_MSGHEAD_LEN];    //消息体
public:
	void  SetSerialNO(u16  wSerialNO){ m_wSerialNO = htons(wSerialNO);} 
    u16   GetSerialNO( void ) const { return ntohs(m_wSerialNO); }

    void  SetEventId(u16  wEventId){ m_wEventId = htons(wEventId);} 
    u16   GetEventId( void ) const { return ntohs(m_wEventId); }

    void  SetErrorCode(u16  wErrorCode){ m_wErrorCode = htons(wErrorCode);} 
    u16   GetErrorCode( void ) const { return ntohs(m_wErrorCode); }

    void  SetTotalPktNum(u32 byPktNum) { m_byTotalPktNum = htonl(byPktNum); }
    u32    GetTotalPktNum( void ) { return ntohl(m_byTotalPktNum); }
	
    void  SetCurPktIdx(u32 byPktIdx) { m_byCurPktIdx = htonl(byPktIdx); }
    u32    GetCurPktIdx( void ) { return ntohl(m_byCurPktIdx); }
	
	void Init( void );
	void SetMsgBodyLen( u16  wMsgBodyLen );
	CPTServMsg( void );//constructor
	CPTServMsg( u8   * const pbyMsg, u16  wMsgLen );//constructor
	~CPTServMsg( void );//distructor
	void ClearHdr( void );//消息头清零
	u16  GetMsgBodyLen( void ) const;//获取消息体长度信息
	u16  GetMsgBody( u8   * pbyMsgBodyBuf, u16  wBufLen ) const;//获取消息体，由用户申请BUFFER，如果过小做截断处理
	u8   * const GetMsgBody( void ) const;//获取消息体指针，用户不需提供BUFFER
	void SetMsgBody( u8   * const pbyMsgBody = NULL, u16  wLen = 0 );//设置消息体
	void CatMsgBody( u8   * const pbyMsgBody, u16  wLen );//添加消息体
	u16  GetServMsgLen( void ) const;//获取整个消息长度
	u16  GetServMsg( u8   * pbyMsgBuf, u16  wBufLen ) const;//获取整个消息，由用户申请BUFFER，如果过小做截断处理
	u8   * const GetServMsg( void ) const;//获取整个消息指针，用户不需提供BUFFER
	void SetServMsg( u8   * const pbyMsg, u16  wLen );//设置整个消息
	const CPTServMsg & operator= ( const CPTServMsg & cServMsg );//操作符重载
};

/*-------------------------------------------------------------------
                               CPTServMsg                              
--------------------------------------------------------------------*/

/*====================================================================
    函数名      ：CPTServMsg
    功能        ：constructor
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/07/15    1.0         LI Yi         创建
====================================================================*/
inline CPTServMsg::CPTServMsg( void )
{
	Init();
}

/*====================================================================
    函数名      ：CPTServMsg
    功能        ：constructor
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 * const pbyMsg, 要赋值的消息指针
				  u16 wMsgLen, 要赋值的消息长度，必须大于等于 PT_SERV_MSGHEAD_LEN
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/07/15    1.0         LI Yi         创建
====================================================================*/
inline CPTServMsg::CPTServMsg( u8 * const pbyMsg, u16 wMsgLen )
{
	Init();
	
	if( wMsgLen < PT_SERV_MSGHEAD_LEN || pbyMsg == NULL )
		return;
	
	wMsgLen = min( wMsgLen, PT_SERV_MSG_LEN );
	memcpy( this, pbyMsg, wMsgLen );
	
	//set length
	SetMsgBodyLen( wMsgLen - PT_SERV_MSGHEAD_LEN );
}

/*====================================================================
    函数名      ：~CPTServMsg
    功能        ：distructor
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/07/15    1.0         LI Yi         创建
====================================================================*/
inline CPTServMsg::~CPTServMsg( void )
{

}

/*====================================================================
    函数名      ：Init
    功能        ：BUFFER初始化
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/07/15    1.0         LI Yi         创建
====================================================================*/
inline void CPTServMsg::Init( void )
{
	memset( this, 0, PT_SERV_MSG_LEN );	//清零
}

/*====================================================================
    函数名      ：ClearHdr
    功能        ：消息头清零
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  ：
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/07/15    1.0         LI Yi         创建
====================================================================*/
inline void CPTServMsg::ClearHdr( void )
{
	u16		wBodyLen = GetMsgBodyLen();

	memset( this, 0, PT_SERV_MSGHEAD_LEN );	//清零
	SetMsgBodyLen( wBodyLen );
}

/*====================================================================
    函数名      ：GetMsgBodyLen
    功能        ：获取消息体长度信息
    算法实现    ：
    引用全局变量：
    输入参数说明：无
    返回值说明  ：消息体长度
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/07/15    1.0         LI Yi         创建
====================================================================*/
inline u16 CPTServMsg::GetMsgBodyLen( void ) const
{
	return( ntohs( m_wMsgBodyLen ) );
}

/*====================================================================
    函数名      ：SetMsgBodyLen
    功能        ：设置消息体长度信息
    算法实现    ：
    引用全局变量：
    输入参数说明：u16 wMsgBodyLen, 消息体长度
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/07/15    1.0         LI Yi         创建
====================================================================*/
inline void CPTServMsg::SetMsgBodyLen( u16 wMsgBodyLen )
{
	m_wMsgBodyLen = htons( wMsgBodyLen );
}

/*====================================================================
    函数名      ：GetMsgBody
    功能        ：获取消息体指针，用户不需提供BUFFER
    算法实现    ：
    引用全局变量：
    输入参数说明：无
    返回值说明  ：u8 * const指针
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/07/15    1.0         LI Yi         创建
====================================================================*/
inline u8 * const CPTServMsg::GetMsgBody( void ) const
{
	return( ( u8 * const )m_abyMsgBody );
}

/*====================================================================
    函数名      ：GetMsgBodyLen
    功能        ：获取整个消息长度
    算法实现    ：
    引用全局变量：
    输入参数说明：无
    返回值说明  ：整个消息长度
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/07/26    1.0         LI Yi         创建
====================================================================*/
inline u16 CPTServMsg::GetServMsgLen( void ) const
{
	return( GetMsgBodyLen() + PT_SERV_MSGHEAD_LEN );
}

/*====================================================================
    函数名      ：GetMsgBody
    功能        ：获取消息体，由用户申请BUFFER，如果过小做截断处理
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 * pbyMsgBodyBuf, 返回的消息体
				  u16 wBufLen, BUFFER大小
    返回值说明  ：实际返回的消息体长度
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/07/15    1.0         LI Yi         创建
====================================================================*/
inline u16 CPTServMsg::GetMsgBody( u8 * pbyMsgBodyBuf, u16 wBufLen ) const
{
    u16 wActLen = min( GetMsgBodyLen(), wBufLen );
	memcpy( pbyMsgBodyBuf, m_abyMsgBody, wActLen );
	return wActLen;
}

/*====================================================================
    函数名      ：SetMsgBody
    功能        ：设置消息体
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 * const pbyMsgBody, 传入的消息体，缺省为NULL
				  u16 wLen, 传入的消息体长度，缺省为0
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/07/15    1.0         LI Yi         创建
====================================================================*/
inline void CPTServMsg::SetMsgBody( u8 * const pbyMsgBody, u16 wLen )
{
    /*
    if (pbyMsgBody == NULL || wLen == 0)
    {
        SetMsgBodyLen( 0 );
        return;
    }
    */
	wLen = min( wLen, PT_SERV_MSG_LEN - PT_SERV_MSGHEAD_LEN );
	memcpy( m_abyMsgBody, pbyMsgBody, wLen );
	SetMsgBodyLen( wLen );
}

/*====================================================================
    函数名      ：CatMsgBody
    功能        ：添加消息体
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 * const pbyMsgBody, 传入的消息体，缺省为NULL
				  u16 wLen, 传入的消息体长度，缺省为0
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/11/07    1.0         Liaoweijiang  创建
====================================================================*/
inline void CPTServMsg::CatMsgBody( u8 * const pbyMsgBody, u16 wLen )
{
    /*
    if (pbyMsgBody == NULL || wLen == 0)
    {
        return;
    }
    */
	wLen = min( wLen, PT_SERV_MSG_LEN - PT_SERV_MSGHEAD_LEN - GetMsgBodyLen() );
	memcpy( m_abyMsgBody + GetMsgBodyLen(), pbyMsgBody, wLen );
	SetMsgBodyLen( GetMsgBodyLen() + wLen );
}

/*====================================================================
    函数名      ：GetMsgBody
    功能        ：获取整个消息指针，用户不需提供BUFFER
    算法实现    ：
    引用全局变量：
    输入参数说明：无
    返回值说明  ：u8 * const指针
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/07/26    1.0         LI Yi         创建
====================================================================*/
inline u8 * const CPTServMsg::GetServMsg( void ) const
{
	return( ( u8 * const )( this ) );
}

/*====================================================================
    函数名      ：GetServMsg
    功能        ：获取整个消息，由用户申请BUFFER，如果过小做截断处理
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 * pbyMsgBuf, 返回的消息
				  u16 wBufLen, BUFFER大小
    返回值说明  ：实际返回的消息长度
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/07/15    1.0         LI Yi         创建
====================================================================*/
inline u16 CPTServMsg::GetServMsg( u8 * pbyMsgBuf, u16 wBufLen ) const
{
	wBufLen = min(PT_SERV_MSG_LEN,wBufLen);
	memcpy( pbyMsgBuf, this, wBufLen );
	return( min( GetMsgBodyLen() + PT_SERV_MSGHEAD_LEN, wBufLen ) );
}

/*====================================================================
    函数名      ：SetServMsg
    功能        ：设置整个消息
    算法实现    ：
    引用全局变量：
    输入参数说明：u8 * const pbyMsg, 传入的整个消息
				  u16 wMsgLen, 传入的消息长度
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/07/15    1.0         LI Yi         创建
====================================================================*/
inline void CPTServMsg::SetServMsg( u8 * const pbyMsg, u16 wMsgLen )
{
	if( wMsgLen < PT_SERV_MSGHEAD_LEN )
	{
		OspPrintf( TRUE, FALSE, "CPTServMsg: SetServMsg() Exception -- invalid MsgLen!\n" );
		return;
	}

	wMsgLen = min( wMsgLen, PT_SERV_MSG_LEN );
	memcpy( this, pbyMsg, wMsgLen );
	SetMsgBodyLen( wMsgLen - PT_SERV_MSGHEAD_LEN );
}

/*====================================================================
    函数名      ：operator=
    功能        ：操作符重载
    算法实现    ：
    引用全局变量：
    输入参数说明：CPTServMsg & CPTServMsg, 赋值的消息引用
    返回值说明  ：CPTServMsg对象应用
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/07/26    1.0         LI Yi         创建
====================================================================*/
inline const CPTServMsg & CPTServMsg::operator= ( const CPTServMsg & cServMsg )
{
	u16	wLen = cServMsg.GetServMsgLen();

	memcpy( this, cServMsg.GetServMsg(), wLen );
	return( *this );
}

#endif





















