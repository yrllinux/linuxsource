/*****************************************************************************
   模块名      : KDVMCU Agent
   文件名      : mcuagentstruc.h
   相关文件    : 
   文件实现功能: KDVMCU agent struct
   作者        : 李屹
   版本        : V4.0  Copyright( C) 2006-2008 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本     修改人       修改内容
   2002/01/25  0.9      李屹         创建
   2003/07/15  1.1      jianghy      方案调整
   2003/11/11  3.0      jianghy      3.0实现
   2004/11/10  3.6      libo         新接口修改
   2004/11/29  3.6      libo         移植到Linux
   2005/08/17  4.0      liuhuiyun    更新
   2006/11/02  4.0      张宝卿       数据结构优化
******************************************************************************/
#ifndef H_MCUAGTSTRUC_H
#define H_MCUAGTSTRUC_H

#include "agentcommon.h"

#ifdef WIN32
#pragma pack( push )
#pragma pack( 1 )
#endif


#define  VERSION_CONFIGURE36            (u8)36   // 3.6版本
#define  VERSION_CONFIGURE40            (u8)40   // 4.0版本
#define  VERSION_CONFIGURE60			(u8)60	 // 6.0版本	

#define  LOG_OFF                        (u8)0    
#define  LOG_ERROR                      (u8)1
#define  LOG_WARN                       (u8)2
#define  LOG_INFORM                     (u8)3
#define  LOG_VERBOSE                    (u8)4

// 错误字符串
#define ERR_STRING_SYS                  (LPCSTR)"Fail to read system infomation"
#define ERR_STRING_LOACL                (LPCSTR)"Fail to read local infomation"
#define ERR_STRING_NETWORK              (LPCSTR)"Fail to read network infomation"
#define ERR_STRING_TRAP                 (LPCSTR)"Fail to read snmp infomation"
#define ERR_STRING_BOARD                (LPCSTR)"Fail to read board infomation"
#define ERR_STRING_MIX                  (LPCSTR)"Fail to read mixer infomation"
#define ERR_STRING_TVWALL               (LPCSTR)"Fail to read tvwall infomation"
#define ERR_STRING_RECORDER             (LPCSTR)"Fail to read recorder infomation" 
#define ERR_STRING_BAS                  (LPCSTR)"Fail to read bas infomation"
#define ERR_STRING_BASHD                (LPCSTR)"Fail to read basHD infomation"
#define ERR_STRING_VMP                  (LPCSTR)"Fail to read vmp infomation"
#define ERR_STRING_MPW                  (LPCSTR)"Fail to read mpw infomation"
#define ERR_STRING_PRS                  (LPCSTR)"Fail to read prs infomation"
#define ERR_STRING_DATACONF             (LPCSTR)"Fail to read data conf infomation"
#define ERR_STRING_NETSYSC              (LPCSTR)"Fail to read netsysc infomation"
#define ERR_STRING_QOS                  (LPCSTR)"Fail to read Qos infomation"
#define ERR_STRING_VMPATTACH            (LPCSTR)"Fail to read vmp attach infomation" 

// 内部错误码定义

#define     ERR_AGENT_BGN               (u8)100

#define  ERR_AGENT_READSYSCFG		    (ERR_AGENT_BGN + 7)  // 读系统信息错误
#define  ERR_AGENT_READNETCFG		    (ERR_AGENT_BGN + 8)  // 读网络参数错误
#define  ERR_AGENT_READLOCALCFG		    (ERR_AGENT_BGN + 9)  // 读本地信息错误
#define  ERR_AGENT_READTRAPCFG		    (ERR_AGENT_BGN + 10) // 读Trap信息错误
#define  ERR_AGENT_READBOARDCFG		    (ERR_AGENT_BGN + 11) // 读单板信息错误
#define  ERR_AGENT_READMIXCFG		    (ERR_AGENT_BGN + 12) // 读混音器错误
#define  ERR_AGENT_READTVCFG		    (ERR_AGENT_BGN + 13) // 读电视墙错误
#define  ERR_AGENT_READRECORDERCFG      (ERR_AGENT_BGN + 14) // 读录像机信息错误
#define  ERR_AGENT_READBASCFG           (ERR_AGENT_BGN + 15) // 读码流适配错误
#define  ERR_AGENT_READVMPCFG           (ERR_AGENT_BGN + 16) // 读画面复合错误
#define  ERR_AGENT_READMPWCFG           (ERR_AGENT_BGN + 17) // 读多画面复合器错误
#define  ERR_AGENT_READVMPATTACHCFG     (ERR_AGENT_BGN + 18) // 读画面复合附属信息错误
#define  ERR_AGENT_READPRSCFG           (ERR_AGENT_BGN + 19) // 读Prs信息错误
#define  ERR_AGENT_READQOSCFG           (ERR_AGENT_BGN + 20) // 读Qos信息错误
#define  ERR_AGENT_READNETSYSCCFG       (ERR_AGENT_BGN + 21) // 读网同步信息错误
#define  ERR_AGENT_READDATACONFCFG      (ERR_AGENT_BGN + 22) // 读数据会议信息错误
#define  ERR_AGENT_READ8000BINFO        (ERR_AGENT_BGN + 23) // 读8000B DSC配置错误
#define  ERR_AGENT_READBASHDCFG         (ERR_AGENT_BGN + 24) // 读高清bas错误
#define  ERR_AGENT_READHDUSCHEMECFG     (ERR_AGENT_BGN + 25) // 读hdu预案信息错误     //4.6   jlb
#define  ERR_AGENT_READHDUCFG		    (ERR_AGENT_BGN + 13) // 读hdu错误


#define  ERR_AGENT_BOARDDEXIST          (ERR_AGENT_BGN + 30) // 该单板已经存在
#define  ERR_AGENT_BOARDDNOTEXIST       (ERR_AGENT_BGN + 31) // 该单板不存在
#define  ERR_AGENT_BOARDDUPREG          (ERR_AGENT_BGN + 32) // 重复注册
#define  ERR_AGENT_CREATECFGFILE	    (ERR_AGENT_BGN + 33) // 创建配置文件失败
#define  ERR_AGENT_WRITECFGFILE		    (ERR_AGENT_BGN + 34) // 写配置文件失败
#define  ERR_AGENT_BRDNOTEXIST		    (ERR_AGENT_BGN + 35) // 单板不存在
#define  ERR_AGENT_EQPNOTEXIST		    (ERR_AGENT_BGN + 36) // 外设不存在
#define  ERR_AGENT_NULLPARAM		    (ERR_AGENT_BGN + 37) // 参数错误
#define  ERROR_AGENT_SNMP_GETVALUE	    (ERR_AGENT_BGN + 38) // Snmp取值错误
#define  ERR_AGENT_CREATECONFFILE	    (ERR_AGENT_BGN + 39) // 生成配置文件错误

// 内部宏定义
#define  ENTRY_KEY					    (LPCSTR)"Entry"      // Entry 字符串
#define  ENTRY_NUM					    (LPCSTR)"EntryNum"   // Entry num 字符串

#define  MAX_ERROR_NUM					(u8)20		// 最大错误数
#define  MAX_PRIEQP_NUM					(u8)16      // 最大外设数 
//4.6新加外设 jlb
#define  MAX_PERIHDU_NUM                (u8)70      // 最大HDU个数
//#define  MAX_PERIDVMP_NUM                (u8)8       //最大Dvmp个数
#ifndef _MINIMCU_
//  [12/6/2010 chendaiwei]把最大板数加一，以支持IS2.2板
#define  MAX_BOARD_NUM					(u8)(((16*4) + (7*10))+1)  // 最大单板数
#else
#define  MAX_BOARD_NUM					(u8)(16 + (7*10))      // 最大单板数
#endif

#define  MAX_ALIAS_LENGTH				(u8)32      // 最大别名长度
#define  MAX_PORT_LENGTH				(u8)5       // 最大端口字符串长度

#define  CONF_MONTH_OLD					(u8)10      // 旧配置文件月份
#define  CONF_YEAR_OLD					(u16)2005   // 旧配置文件年份


#define  MAX_SOFT_VER_LEN				(u8)128      // 软件版本信息
#define  MAX_CONFIGURE_LENGTH			(u8)64       // Mcu 配置文件版本长度

#define  MAX_IPSTR_LEN					(u8)64		 // 地址字符串最大长度
// change for IS2.2 [05/04/2012 liaokang]
#define  MAX_BOARD_LED_NUM				(u8)32       // 单板最大灯数目
#define  MAX_MODULE_NAME_LEN			(u8)256		 // 单板模块名长度
// add for IS2.2 [05/04/2012 liaokang]
#define  MAX_BOARD_ETHPORT_NUM			(u8)27		 // 单板最大网口数目

#define  QOS_IP_NOALL					(u8)0        // 全都不选
#define  QOS_IP_LEASTDELAY				(u8)1        // 最小延迟
#define  QOS_IP_THRUPUT					(u8)2        // 最高吞吐量
#define  QOS_IP_RELIABLITY				(u8)3        // 最高可靠性
#define  QOS_IP_EXPENSES				(u8)4        // 最小费用

#define  DATA_LVL_IP					(u8)0        // Qos为Ip 时数据默认等级
#define  VIDEO_LVL_IP					(u8)3        // Qos为Ip 时视频默认等级
#define  AUDIO_LVL_IP					(u8)4        // Qos为Ip 时音频默认等级
#define  SIGNAL_LVL_IP					(u8)7        // Qos为Ip 时信号默认等级

#define  MAX_IPSERVICE_LVL				(u8)4        // Qos为Ip 优先时Ip服务等级最大值
#define  MIN_IPSERVICE_LVL				(u8)0        // Qos为Ip 优先时Ip服务等级最小值
#define  MIN_IPSERVICE_STR_LEN			(u8)10		 // Qos为Ip 优先时Ip服务字符串长度

#define  MAX_ERROR_NUM			        (u8)20		  // 读配置错误数目
#define  MAX_ERROR_STR_LEN		        (u8)128		  // 单条错误最大长度

#define  MAX_ALARMOBJECT_NUM            (u8)5		  // 告警对象数目
#define  MAX_ALARM_TIME_LEN             (u8)32        // 告警时间长度
#define  MAX_FTPFILENAME_LEN            (u8)80	      // ftp 文件名最大长度
#define  MAXNUM_MCU_ALARM		        (u32)((64*32+128)/2) // MCU中的最大告警数(取实际最大告警数的一半)

#define  UPDATE_NMS                     (u8)1         // 网管发起单板升级
#define  UPDATE_MCS                     (u8)2         // 会控发起单板升级

#define  BRD_LAYER_CUSTOM               (u8)0xFE      // 特别的单板层号，表示非机框式单板(正常为0-3)

// 单板类型字符串
#define BOARD_TYPE_MPC                  (LPCSTR)"MPC"
//  [1/21/2011 chendaiwei]新增MPC2单板类型字符串
#define BOARD_TYPE_MPC2                 (LPCSTR)"MPC2"
#define BOARD_TYPE_CRI                  (LPCSTR)"CRI"
//  [1/21/2011 chendaiwei]新增CRI2单板类型字符串
#define BOARD_TYPE_CRI2                 (LPCSTR)"CRI2"
#define BOARD_TYPE_DTI                  (LPCSTR)"DTI"
#define BOARD_TYPE_DIC                  (LPCSTR)"DIC"
#define BOARD_TYPE_DRI                  (LPCSTR)"DRI"
//  [1/21/2011 chendaiwei]新增DRI2单板类型字符串
#define BOARD_TYPE_DRI2                 (LPCSTR)"DRI2"
#define BOARD_TYPE_DRI16                (LPCSTR)"DRI16"
#define BOARD_TYPE_MMP                  (LPCSTR)"MMP"
#define BOARD_TYPE_VPU                  (LPCSTR)"VPU"
#define BOARD_TYPE_DEC5                 (LPCSTR)"DEC5"
#define BOARD_TYPE_VAS                  (LPCSTR)"VAS"
#define BOARD_TYPE_IMT                  (LPCSTR)"IMT"
#define BOARD_TYPE_APU                  (LPCSTR)"APU"
#define BOARD_TYPE_EAPU                 (LPCSTR)"EAPU"
#define BOARD_TYPE_DSI                  (LPCSTR)"DSI"
#define BOARD_TYPE_MPCB                 (LPCSTR)"MPCB"
#define BOARD_TYPE_MPCC                 (LPCSTR)"MPCC"
#define BOARD_TYPE_MDSC			        (LPCSTR)"MDSC"
#define BOARD_TYPE_HDSC                 (LPCSTR)"HDSC"
#define BOARD_TYPE_DSC			        (LPCSTR)"DSC"
//4.6版本新加 jlb
#define BOARD_TYPE_HDU			        (LPCSTR)"HDU"
#define BOARD_TYPE_HDU_L			    (LPCSTR)"HDU_L"
#define BOARD_TYPE_MPU			        (LPCSTR)"MPU"
#define BOARD_TYPE_EBAP			        (LPCSTR)"EBAP"
#define BOARD_TYPE_EVPU			        (LPCSTR)"EVPU"
#define BOARD_TYPE_UNKNOW               (LPCSTR)"Unknow"
//[2011/01/25 zhushz]IS2.x单板支持
#define BOARD_TYPE_IS21					(LPCSTR)"IS21"
#define BOARD_TYPE_IS22					(LPCSTR)"IS22"
// 4.7支持 [1/31/2012 chendaiwei]
#define BOARD_TYPE_HDU2					(LPCSTR)"HDU2"
#define BOARD_TYPE_MPU2					(LPCSTR)"MPU2"
#define BOARD_TYPE_MPU2ECARD	        (LPCSTR)"MPU2_ECARD"
#define BOARD_TYPE_APU2					(LPCSTR)"APU2"
#define BOARD_TYPE_HDU2_L				(LPCSTR)"HDU2-L"
#define BOARD_TYPE_HDU2_S				(LPCSTR)"HDU2-S"

/* 单板身份结构定义 */
struct tTagBrdPos{
    u8 byBrdID;                      /* 板子ID号 */
    u8 byBrdLayer;                   /* 板子所在层号，设备不支持时返回0，当作0层处理 */
    u8 byBrdSlot;                    /* 板子所在槽位号，设备不支持时返回0，当作0槽处理;
									 部分设备有主从之分，用slot来区分：0=主；1=从 */
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;
typedef tTagBrdPos TBrdPos;

// add EthPort [05/04/2012 liaokang]
// 单板配置表项结构。 在非嵌入式(8260)情况下只有编号和IP有效
struct TEqpBrdCfgEntry : public TBoardInfo
{
protected:    
    s8  m_achVersion[MAX_SOFT_VER_LEN+1];		/*单板软件版本*/
    s8  m_achModule[MAX_MODULE_NAME_LEN+1];	    /*单板的模块信息*/
    s8  m_achPanelLed[MAX_BOARD_LED_NUM+1];		/*单板的灯状态 1 亮 2 灭 3 快闪 4 慢闪 5 每隔2秒钟闪*/    
    s8  m_achEthPort[MAX_BOARD_ETHPORT_NUM+1];  /*单板的网口双工状态*/  // add [05/04/2012 liaokang]
public:
    TEqpBrdCfgEntry() {	SetEntParamNull();	}
    void SetEntParamNull()
    {
        memset(m_achVersion, 0, sizeof(m_achVersion));
        memset(m_achModule, 0, sizeof(m_achModule));
        memset(m_achPanelLed, 0, sizeof(m_achPanelLed));
        memset(m_achEthPort, 0, sizeof(m_achEthPort));
    }
    void SetVersion(LPCSTR lpszVerion) 
    {
        if ( NULL != lpszVerion )
        {
            strncpy( m_achVersion, lpszVerion, sizeof(m_achVersion) );
            m_achVersion[sizeof(m_achVersion)-1] = '\0';
        }
    }
    LPCSTR GetVersion(void) { return m_achVersion;    }
    void SetModule(LPCSTR lpszModule) 
    {
        if ( NULL != lpszModule )
        {
            strncpy( m_achModule, lpszModule, sizeof(m_achModule) );
            m_achModule[sizeof(m_achModule)-1] = '\0';
        }
    }
    LPCSTR GetModule(void) { return m_achModule;    }      

    void SetPanelLed(LPCSTR lpszPanelLed) 
    {
        if ( NULL != lpszPanelLed )
        {
            /*strncpy( m_achPanelLed, lpszPanelLed, sizeof(m_achPanelLed) );*/
            memcpy( m_achPanelLed, lpszPanelLed, sizeof(m_achPanelLed) ); // MLNK初始为0 [2012/05/30 liaokang]
            m_achPanelLed[sizeof(m_achPanelLed)-1] = '\0';
        }
    }

	/*
	void SetPanelLed(u8 *pbyPanelLed, u8 byCount) 
    {
		if (byCount > MAX_BOARD_LED_NUM)
		{
			return;
		}

        if ( NULL != pbyPanelLed )
        {
            memcpy( m_achPanelLed, pbyPanelLed, byCount * sizeof(u8) );
            m_achPanelLed[MAX_BOARD_LED_NUM] = '\0';
        }
    }
	*/

    LPCSTR GetPanelLed(void) { return m_achPanelLed; }
    
    //  add EthPort [05/04/2012 liaokang]
    void SetEthPort(LPCSTR lpszNetPort) 
    {
        if ( NULL != lpszNetPort )
        {
            memcpy( m_achEthPort, lpszNetPort, sizeof(m_achEthPort) );  // 网口状态
            m_achEthPort[sizeof(m_achEthPort)-1] = '\0'; // 结束符            
        }
    }
    
    LPCSTR GetEthPort(void) { return m_achEthPort; }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

// 网同步配置部分 
struct TEqpNetSync : public TNetSyncInfo
{
protected:
	u8	m_bySourceType;		/*网同步源选择*/
    u8	m_byClockType;		/*时钟选择*/
    u8	m_byState;			/*网同步的状态*/	
public:
	TEqpNetSync() : m_bySourceType(0),
					m_byClockType(0),
					m_byState(0){}
	void SetSourceType(u8 byType) { m_bySourceType = byType;	}
	u8   GetSourceType(void) const { return m_bySourceType;	}
	void SetClockType(u8 byType){ m_byClockType = byType;	}
	u8   GetClockType(void) const { return m_byClockType;	}
	void SetState(u8 byState) { m_byState = byState;	}
	u8   GetState(void) const { return m_byState;	}
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;


/************************************************************************/
/* 外设部分                                                             */
/************************************************************************/

//外设表入口项参数基类
struct TEqpEntryParam
{
protected:
    u32 m_dwMcuIp;		    /*本外设要连接的MCU的IP地址 网络序*/
    u16 m_wMcuPort;			/*本外设要连接的MCU的端口号 网络序*/
    u8  m_byMcuId;		    /*本外设要连接的MCU编号*/
    
    u8  m_byType;			/*本外设的类型值*/
    u8  m_byConnectState;	/*本外设的连接状态*/
    u8  m_byExist;			/*是否使用该表项*/
    
public:
    TEqpEntryParam() : m_dwMcuIp(0),
                       m_wMcuPort(0),
                       m_byMcuId(0),
                       m_byType(0),
                       m_byConnectState(0),
                       m_byExist(0){}
    void SetMcuIp(u32 dwMcuIp) { m_dwMcuIp = htonl(dwMcuIp);   }
    u32  GetMcuIp(void) const { return ntohl(m_dwMcuIp);  }
    void SetMcuPort(u16 wMcuPort) { m_wMcuPort = htons(wMcuPort);  }
    u16  GetMcuPort(void) const { return ntohs(m_wMcuPort);   }
    void SetMcuId(u8 byMcuId) { m_byMcuId = byMcuId;   }
    u8   GetMcuId(void) const { return m_byMcuId;    }
    void SetType(u8 byType) { m_byType = byType;  }
    u8   GetType(void) const { return m_byType;   }
    void SetState( u8 byState ) {  m_byConnectState = byState;   }
    u8   GetState(void) const { return m_byConnectState;  }
    void SetExist(BOOL32 bExist) { m_byExist = bExist ? 1 : 0;   }
    BOOL32 GetExist(void) { return m_byExist == 1 ? TRUE : FALSE;  }

    void SetDefaultEntParam( u8 byEqpType, u32 dwMcuIp, u8 byState = 0 )
    {
        m_byType   = byEqpType;
        m_dwMcuIp  = dwMcuIp;
        m_byConnectState  = byState;
        m_byMcuId  = LOCAL_MCUID;
        m_wMcuPort = MCU_LISTEN_PORT;
        m_byExist  = 1;
    }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

// [1]. 码流适配器
struct TEqpBasEntry : public TEqpBasInfo, 
                      public TEqpEntryParam
{
protected:
    u8  m_byMaxAdpChannel;		/*码流适配器的每个MAP最大适配通道*/
public:
    TEqpBasEntry() : m_byMaxAdpChannel(0){}
    void SetAdpChnNum(u8 byChnNum){ m_byMaxAdpChannel = byChnNum;  }
    u8   GetAdpChnNum(void) const { return m_byMaxAdpChannel;  }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

// [1.1]. 高清bas
struct TEqpBasHDEntry : public TEqpBasHDInfo, 
                      public TEqpEntryParam
{
protected:
    u8  m_byMaxAdpChannel;		/*码流适配器的每个MAP最大适配通道*/
public:
    TEqpBasHDEntry() : m_byMaxAdpChannel(0){}
    void SetAdpChnNum(u8 byChnNum){ m_byMaxAdpChannel = byChnNum;  }
    u8   GetAdpChnNum(void) const { return m_byMaxAdpChannel;  }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

// [2]. 混音器
struct TEqpMixerEntry : public TEqpMixerInfo,
                        public TEqpEntryParam 
{
protected:
    u8  m_byMaxMixGroupNum;	/*最大混音组个数*/
	u8	m_byIsSendRedundancy;	// 是否冗余发送, zgc, 2007-0725
public:
    TEqpMixerEntry() : m_byMaxMixGroupNum(0){}
    void SetMaxMixGrpNum(u8 byMaxGrpNum) { m_byMaxMixGroupNum = byMaxGrpNum;   }
    u8   GetMaxMixGrpNum(void) const { return m_byMaxMixGroupNum;    }
	void SetIsSendRedundancy( BOOL32 IsRedundancy ) { m_byIsSendRedundancy = IsRedundancy ? 1 : 0; }
	BOOL32 IsSendRedundancy(void) const { return ( 1 == m_byIsSendRedundancy ) ? TRUE : FALSE; } 
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

// [3]. 多画面电视墙
struct TEqpMPWEntry : public TEqpMpwInfo,
                      public TEqpEntryParam
{
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

// [4]. 数字电视墙 
struct TEqpTVWallEntry : public TEqpTVWallInfo,
                         public TEqpEntryParam
{
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

// [5]. 包重传器
struct TEqpPrsEntry : public TEqpPrsInfo,
                      public TEqpEntryParam
{
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

// [6]. 图像合成器
struct TEqpVMPEntry : public TEqpVMPInfo,
                      public TEqpEntryParam
{
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

// [7]. 数字录放象机
struct TEqpRecEntry : public TEqpRecInfo,
                      public TEqpEntryParam
{
protected:
    u8  m_byRecordChannelNum;	/*数字放象、录象机的录象通道数*/
    u8  m_byPlayChannelNum;		/*数字放象、录象机的放象通道数*/
public:
    TEqpRecEntry() : m_byRecordChannelNum(0),
                     m_byPlayChannelNum(0){}
    void SetRecChnNum(u8 byRecChnNum) { m_byRecordChannelNum = byRecChnNum;   }
    u8   GetRecChnNum(void) const { return m_byRecordChannelNum;   }
    void SetPlayChnNum(u8 byPlayChnNum) { m_byPlayChannelNum = byPlayChnNum;   }
    u8   GetPlayChnnlNum(void) const { return m_byPlayChannelNum;   }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;


// [8]. 数字会议服务器
struct TEqpDCSEntry
{
public:
    TEqpDCSEntry() : m_dwDcsIp(0),
                     m_byDcsConnState(0){}
public:
	u32  m_dwDcsIp;			    /*数字会议服务器的设备ID*/
	u8   m_byDcsConnState;		/*数字会议服务器的连接状态*/
};

//4.6版新加外设表项结构
//HDU高清解码卡
struct TEqpHduEntry : public TEqpHduInfo,
                      public TEqpEntryParam
{
    
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//单VMP
struct TEqpSvmpEntry: public TEqpSvmpInfo,
                      public TEqpEntryParam
{

}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//双VMP
struct TEqpDvmpBasicEntry: public TEqpDvmpBasicInfo,
                      public TEqpEntryParam
{

}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//双VMP
struct TEqpMpuBasEntry: public TEqpMpuBasInfo,
                           public TEqpEntryParam
{

}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//Ebap
struct TEqpEbapEntry: public TEqpEbapInfo,
                      public TEqpEntryParam
{
	
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//Evpu
struct TEqpEvpuEntry: public TEqpEvpuInfo,
                      public TEqpEntryParam
{
	
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//Vrs录播服务器
struct TEqpVrsRecEntry: public TEqpVrsRecCfgInfo
{

}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;
/*------------------------------------------------------------- 
*系统组的配置信息 
*-------------------------------------------------------------*/
struct TMcuSystem
{
protected:
    u8  m_byState;										/*系统状态*/
    s8  m_achTime[MAX_ALARM_TIME_LEN+1];				/*系统时间*/
    s8  m_achFtpFileName[MAX_FTPFILENAME_LEN+1];		/*FTP文件名*/
    s8  m_achConfigVersion[MAX_CONFIGURE_LENGTH];	    /*配置版本*/
    s8  m_achSoftwareVersion[MAX_SOFT_VER_LEN+1];		/*软件版本*/
	u8  m_byIsMcuConfiged;						        /*MCU初次配置标识*/
	u8	m_byMcuCfgInfoLevel;							/*MCU配置信息级别, zgc, 2007-03-19*/
	s8  m_achCompileTime[MAX_ALARM_TIME_LEN+1];				/*系统时间*/
    
public:
    TMcuSystem(void){   memset(this, 0, sizeof(TMcuSystem));	}
    void SetState(u8 byState) { m_byState = byState;    }
    u8   GetState(void) const { return m_byState;    }

    void SetTime(s8 *pszTime)
    {
        if ( NULL != pszTime )
        {
            memcpy( &m_achTime, pszTime, sizeof(m_achTime) );
            m_achTime[sizeof(m_achTime)-1] = '\0';
        }
        return;
    }
    LPCSTR GetTime() { return m_achTime;    }
    void SetFtpFileName(LPCSTR lpszStr)
    {
        if ( NULL != lpszStr )
        {
            strncpy( m_achFtpFileName, lpszStr, sizeof(m_achFtpFileName) );
            m_achFtpFileName[sizeof(m_achFtpFileName)-1] = '\0';
        }
    }
    LPCSTR GetFtpFileName() { return m_achFtpFileName;    }
    void SetConfigVer(s8* lpszStr)
    {
        if ( NULL != lpszStr )
        {
            strncpy( m_achConfigVersion, lpszStr, sizeof(m_achConfigVersion) );
            m_achConfigVersion[sizeof(m_achConfigVersion)-1] = '\0';
        }
    }
    s8* GetConfigVer() { return m_achConfigVersion;    }
    void SetSoftWareVer(LPCSTR lpszStr)
    {
        if ( NULL != lpszStr )
        {
            strncpy( m_achSoftwareVersion, lpszStr, sizeof(m_achSoftwareVersion) );
            m_achSoftwareVersion[sizeof(m_achSoftwareVersion)-1] = '\0';
        }
    }
    LPCSTR GetSoftWareVer() { return m_achSoftwareVersion;    }

	//forMCU支持导航配置需求，zgc06-12-21
	void SetIsMcuConfiged(BOOL32 bIsConfiged)
	{
		m_byIsMcuConfiged = bIsConfiged ? 1 : 0;
		return;
	}
	BOOL32 IsMcuConfiged()
	{
		return ( 1 == m_byIsMcuConfiged ) ? TRUE : FALSE ;   
	}

	// 支持MCU配置文件备份功能，zgc , 2007-03-16
	void SetMcuCfgInfoLevel( u8 byLevel )
	{
		m_byMcuCfgInfoLevel = byLevel;
	}
	u8 GetMcuCfgInfoLevel()
	{
		return m_byMcuCfgInfoLevel;
	}

};    


/*------------------------------------------------------------- 
*性能组信息 
*-------------------------------------------------------------*/
struct TMcuPerformance
{
    u16 m_wAlarmStamp;   /*告警戳，增加删除告警时加1*/
public:
    TMcuPerformance() : m_wAlarmStamp(0) {}
};

/*------------------------------------------------------------- 
*MIB使用的告警结构 
*-------------------------------------------------------------*/
struct TMcupfmAlarmEntry
{
public:
    TMcupfmAlarmEntry()	: m_dwSerialNo(0),
                          m_dwAlarmCode(0),
                          m_byObjType(0)
    {
        memset( m_achTime, 0, sizeof(m_achTime));
        memset( m_achObject, 0, sizeof(m_achObject));
    }
public: 
	u32 m_dwSerialNo;				        /*告警的编号*/
    u32 m_dwAlarmCode;				        /*告警码*/
    u8  m_byObjType;				        /*告警对象类型*/
    s8  m_achObject[MAX_ALARMOBJECT_NUM];	/*告警对象*/
    s8  m_achTime[MAX_ALARM_TIME_LEN+1];	/*告警产生时间*/
};  


/*------------------------------------------------------------- 
*告警表中的告警结构 
*-------------------------------------------------------------*/
struct TMcupfmAlarmTable
{
public:
    TMcupfmAlarmTable() : m_dwAlarmCode(0),
                          m_byObjType(0),
                          m_bExist(FALSE)
    { 
        memset( m_achTime, 0, sizeof(m_achTime));
        memset( m_achObject, 0, sizeof(m_achObject));
    }
public:
	u32   m_dwAlarmCode;					/*告警码*/
    u8    m_byObjType;						/*告警对象类型*/
    s8    m_achObject[MAX_ALARMOBJECT_NUM];	/*告警对象*/
    s8    m_achTime[MAX_ALARM_TIME_LEN+1];	/*告警产生时间*/
    BOOL  m_bExist;							/*该告警是否有效*/
};

struct TMcuConfState
{
public:
    TMcuConfState()	: m_byState(0)
    { 
        memset( &m_abyTime, 0, sizeof(m_abyTime)); 
        memset( &m_abyConfId, 0, sizeof(m_abyConfId)); 
        memset( &m_abyConfName, 0, sizeof(m_abyConfName)); 
    }
public:
	u8  m_abyConfId[MAXLEN_CONFID+1];					/*会议Id*/
    u8  m_abyConfName[MAXLEN_CONFNAME];					/*会议名*/
    u8  m_abyTime[MAX_ALARM_TIME_LEN+1];				/*时间*/
    u8  m_byState;										/*状态*/
};

struct TMcuMtConfState
{
public:
    TMcuMtConfState() : m_byState(0)
    {
        memset( &m_abyTime, 0, sizeof(m_abyTime));
        memset( &m_abyConfId, 0, sizeof(m_abyConfId));
        memset( &m_abyMtAlias, 0, sizeof(m_abyMtAlias));
    }
public:
	u8  m_abyConfId[MAXLEN_CONFID+1];					/*会议Id*/
    u8  m_abyMtAlias[MAXLEN_ALIAS+1];					/*终端别名*/
    u8  m_abyTime[MAX_ALARM_TIME_LEN+1];				/*时间*/
    u8  m_byState;										/*状态*/
};


// MPC信息(含主备)
struct TMPCInfo
{
protected:
    u32 m_dwMpcIp;                  // 本地MPC地址(网络序)
    u16 m_wMpcPort;                 // 本地MPC端口(网络序)
    u8  m_byMpcPortChoice;          // 本地MPC端口选择
    u8  m_byMpcLayer;               // 本地MPC层号
    u8  m_byMpcSlot;                // 本地MPC槽号
    u8  m_byIsMpcMaster;            // 本地MPC是否主用(业务主用)
    u8  m_byMpcConfigVer;           // 本地MPC配置版本号
    
    u8  m_byOSType;                 // MPC的操作系统类型(默认主备操作系统类型一致)

    u8  m_byIsHaveOtherMpc;         // 是否有对端MPC
    u8  m_byOtherMpcStatus;         // 对端MPC状态
    
    u32 m_dwOtherMpcIp;             // 对端MPC地址(网络序)
    u16 m_wOtherMpcPort;            // 对端MPC端口(网络序)
    u8  m_byOtherMpcPortChoice;     // 对端MPC端口选择
    u8  m_byOtherMpcLayer;          // 对端MPC层号
    u8  m_byOtherMpcSlot;           // 对端MPC槽号
    
public:
    TMPCInfo() : m_dwMpcIp(0),
                 m_wMpcPort(0),
                 m_byMpcPortChoice(0),
                 m_byMpcLayer(0),
                 m_byMpcSlot(0),
                 m_byIsMpcMaster(0),
                 m_byMpcConfigVer(VERSION_CONFIGURE40),
                 m_byIsHaveOtherMpc(0),
                 m_byOtherMpcStatus(BOARD_STATUS_UNKNOW),
                 m_dwOtherMpcIp(0),
                 m_wOtherMpcPort(0),
                 m_byOtherMpcPortChoice(0),
                 m_byOtherMpcLayer(0),
                 m_byOtherMpcSlot(0){}
    void   SetLocalIp(u32 dwIp) { m_dwMpcIp = htonl(dwIp);    }
    u32    GetLocalIp(void) const { return ntohl(m_dwMpcIp);    }
    void   SetLocalPort(u16 wPort) { m_wMpcPort = htons(wPort);    }
    u16    GetLocalPort(void) const { return ntohs(m_wMpcPort);    }
    void   SetLocalPortChoice(u8 byPortChoice) { m_byMpcPortChoice = byPortChoice;    }
    u8     GetLocalPortChoice(void) const { return m_byMpcPortChoice;    }
    void   SetLocalLayer(u8 byLayer) { m_byMpcLayer = byLayer;    }
    u8     GetLocalLayer(void) const { return m_byMpcLayer;    }
    void   SetLocalSlot(u8 bySlot) { m_byMpcSlot = bySlot;    }
    u8     GetLocalSlot(void) const { return m_byMpcSlot;    }
    void   SetIsLocalMaster(BOOL32 bMaster) { m_byIsMpcMaster = bMaster ? 1 : 0; };
    BOOL32 GetIsLocalMaster(void) { return m_byIsMpcMaster == 1 ? TRUE : FALSE;    }
    void   SetLocalConfigVer(u8 byConfigVer) { m_byMpcConfigVer = byConfigVer;    }
    u8     GetLocalConfigVer(void) const { return m_byMpcConfigVer;    }
    void   SetIsHaveOtherMpc(BOOL32 bHave) { m_byIsHaveOtherMpc = bHave ? 1 : 0;    }
    BOOL32 GetIsHaveOtherMpc(void) { return m_byIsHaveOtherMpc == 1 ? TRUE : FALSE;    }
    void   SetOtherMpcStatus(u8 byStatus) { m_byOtherMpcStatus = byStatus;    }
    u8     GetOtherMpcStatus(void) const { return m_byOtherMpcStatus;    }
    void   SetOtherMpcIp(u32 dwIp) { m_dwOtherMpcIp = htonl(dwIp);    }
    u32    GetOtherMpcIp(void) const { return ntohl(m_dwOtherMpcIp);    }
    void   SetOtherMpcPort(u16 wPort) { m_wOtherMpcPort = htons(wPort);    }
    u16    GetOtherMpcPort(void) const { return ntohs(m_wOtherMpcPort);    }
    void   SetOtherPortChoice(u8 byChoice) { m_byOtherMpcPortChoice = byChoice;    }
    u8     GetOtherPortChoice(void) const { return m_byOtherMpcPortChoice;    }
    void   SetOtherMpcLayer(u8 byLayer) { m_byOtherMpcLayer = byLayer;    }
    u8     GetOtherMpcLayer(void) const { return m_byOtherMpcLayer;    }
    void   SetOtherMpcSlot(u8 bySlot) { m_byOtherMpcSlot = bySlot;    }
    u8     GetOtherMpcSlot(void) const { return m_byOtherMpcSlot;    }
    void   SetOSType(u8 byOSType) { m_byOSType = byOSType;    }
    u8     GetOSType(void) const { return m_byOSType;    }
    
    void Print() const
    {
        OspPrintf( TRUE, FALSE, "[TMPCInfo] Config File Version: %d \n", GetLocalConfigVer() );
        OspPrintf( TRUE, FALSE, "[TMPCInfo] OSType: %d <1:VX,2:LINUX,3:WIN32> \n", m_byOSType );
        OspPrintf( TRUE, FALSE, "[LocalMpc] MpcIp.0x%x, MpcPort.%d, MpcPortChoice.%d, \n\t   MpcLayer.%d, MpcSlot.%d, MpcMaster.%d \n",
                                 m_dwMpcIp, m_wMpcPort, m_byMpcPortChoice, 
                                 m_byMpcLayer, m_byMpcSlot, m_byIsMpcMaster );
        if ( TRUE == m_byIsHaveOtherMpc ) 
        {
            OspPrintf( TRUE, FALSE, "[OtherMpc] MpcIp.0x%x, MpcPort.%d, MpcPortChoice.%d, MpcLayer.%d, MpcSlot.%d \n",
                                     m_dwOtherMpcIp, m_wOtherMpcPort, m_byOtherMpcPortChoice, 
                                     m_byOtherMpcLayer, m_byOtherMpcSlot );
        }
        else
        {
            OspPrintf( TRUE, FALSE, "[OtherMpc] Not Configured! \n" );
        }
    }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

/************************************************************************/
/* McuDebug  信息                                                       */
/************************************************************************/
struct TMcuAgentDebugInfo
{
public:
    TMcuAgentDebugInfo() : 
        m_wListenPort(MCU_LISTEN_PORT),
        m_wBroadDiscTimeSpan(DEF_OSPDISC_HBTIME),
        m_byBroadDiscTimes(DEF_OSPDISC_HBNUM),
        m_byMpcRunMp(0)
    {
    }

    void SetListenPort(u16 wPort)
    {
        m_wListenPort = wPort;
    }
    u16  GetListenPort() const
    {
        return m_wListenPort;
    }
    void SetBoardDiscTimeSpan(u16 wTimeSpan)
    {
        m_wBroadDiscTimeSpan = wTimeSpan;
    }
    u16  GetBoardDiscTimeSpan() const
    {
        return m_wBroadDiscTimeSpan;
    }
    void SetBoardDiscTimes(u8 byTimes)
    {
        m_byBroadDiscTimes = byTimes;
    }
    u8 GetBoardDiscTimes() const
    {
        return m_byBroadDiscTimes;
    }
    void SetMpcRunMp(BOOL32 bRun)
    {
        m_byMpcRunMp = bRun ? 1 : 0;
    }
    BOOL32 IsMpcRunMp() const
    {
        return (m_byMpcRunMp == 1);
    }
	void SetMpcRunMtAdp( BOOL32 bRun )
	{
		m_byMpcRunMtAdp = bRun ? 1 : 0;
	}
	BOOL32 IsMpcRunMtAdp() const
	{
		return (m_byMpcRunMtAdp == 1);
	}

    void Print() const
    {
        OspPrintf(TRUE, FALSE, "Mcu Agent Debug Info\n");
        OspPrintf(TRUE, FALSE, "\tm_wListenPort %u\n", m_wListenPort);
        OspPrintf(TRUE, FALSE, "\tm_wBroadDiscTimeSpan %u\n", m_wBroadDiscTimeSpan);
        OspPrintf(TRUE, FALSE, "\tm_byBroadDiscTimes %u\n", m_byBroadDiscTimes);
        OspPrintf(TRUE, FALSE, "\tm_byMpcRunMp %u\n", m_byMpcRunMp);
		OspPrintf(TRUE, FALSE, "\tm_byMpcRunMtAdp %u\n", m_byMpcRunMtAdp);
    }

private:
    u16    m_wListenPort;               // MCU 监听端口
    u16    m_wBroadDiscTimeSpan;        // 单板断链检测间隔
    u8     m_byBroadDiscTimes;          // 单板断链检测次数
    u8     m_byMpcRunMp;                // MPC是否运行MP
	u8	   m_byMpcRunMtAdp;				// MPC是否运行MtAdp, zgc, 2007-07-30
};

#ifdef WIN32

#define SystemBasicInformation       0
#define SystemPerformanceInformation 2
#define SystemTimeInformation        3

#define Li2Double(x) ((double)((x).HighPart) * 4.294967296E9 + (double)((x).LowPart))

//以下错误Windows只有在linux下报错，屏蔽掉
//lint -save -e10 -e148 -e38 -e40 -e19
typedef struct
{
    DWORD   dwUnknown1;
    ULONG   uKeMaximumIncrement;
    ULONG   uPageSize;
    ULONG   uMmNumberOfPhysicalPages;
    ULONG   uMmLowestPhysicalPage;
    ULONG   uMmHighestPhysicalPage;
    ULONG   uAllocationGranularity;
    PVOID   pLowestUserAddress;
    PVOID   pMmHighestUserAddress;
    ULONG   uKeActiveProcessors;
    BYTE    bKeNumberProcessors;
    BYTE    bUnknown2;
    WORD    wUnknown3;
} SYSTEM_BASIC_INFORMATION;

typedef struct
{
    LARGE_INTEGER   liIdleTime;
    DWORD           dwSpare[76];
} SYSTEM_PERFORMANCE_INFORMATION;

typedef struct
{
    LARGE_INTEGER liKeBootTime;
    LARGE_INTEGER liKeSystemTime;
    LARGE_INTEGER liExpTimeZoneBias;
    ULONG         uCurrentTimeZoneId;
    DWORD         dwReserved;
} SYSTEM_TIME_INFORMATION;

typedef LONG (WINAPI *PROCNTQSI)(UINT,PVOID,ULONG,PULONG);
//lint -restore
#endif

#ifdef WIN32
#pragma pack( pop )
#endif

#endif /* End MCUAGTSTRUC_H */
