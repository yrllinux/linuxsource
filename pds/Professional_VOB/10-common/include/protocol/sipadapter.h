
#ifndef _SIPADAPTER_
#define _SIPADAPTER_

#ifdef _LINUX_
#include "osp.h"
// #include <stdarg.h>
// #include <sys/types.h>
// #include <sys/socket.h>
// #include <netinet/in.h>
// #include <netinet/tcp.h>
// #include <netdb.h>
// #include <arpa/inet.h>
// #include <sys/ioctl.h>
// #include <stdio.h>
// #include <unistd.h>
// 
#ifndef min
#define min(a,b) ((a)>(b)?(b):(a))
#endif
#ifndef max
#define max(a,b) ((a)>(b)?(a):(b))
#endif

#endif

#include "rvsipstack.h"
#include "rvsdpmsg.h"
#include "kdvtype.h"

#define LEN_MAXMT_NUM   	(u16)193				//每个会议中的最大呼叫数
#define LEN_MTNUM_PERCONF	(u16)193				//每个会议中的最大呼叫数
#define LEN_USERNAME    	(u16)64			    	//用户名长度
#define LEN_PASSWORD		(u16)16					//密码长度
#define LEN_DOMAINNAME    	(u16)64			    	//域名长度
#define LEN_ENCKEY_MAX		(u16)64					//(1-65535) 
#define SIP_CHAN_SPAN  10
#define SESSION_NAME_LEN     64
#define SESSION_ID_LEN       64
#define SESSION_USERNAME_LEN 64
#define SESSION_VERSION_LEN  64
#define SESSION_INFO_LEN     128

#define MAX_LEN_SESSION_DESCRIPTION 1024*10

#define MAX_LEN_LANGUAGE_HEADER 32
#define MAX_LEN_ENCODE_TYPE_HEADER 32
#define MAX_LEN_HEADER_NAME 64

#define MAX_NUM_TRANC_PERCALL 64
#define MAX_NUM_CALLID        128

#define MAX_NUM_CONTACT_ADDR  5
#define MAX_NUM_REGCLIENT     128
#define MAX_NUM_REGNAME_SIZE 64
#define MAX_NUM_CHAN	     20
#define MAX_NUM_LEN_VERSION			 256
#define MAX_XML_LEN			 65536
#define DEFAULT_BUFF_LEN     1024
#define MAX_AAC_CAP_NUM      2
#define MAX_SIP_REG_INFO_NUM	2
#define MAX_FLOOR_NUM  5
#define MAX_MEDIA_NUM_PER_FLOOR  5
#define MAX_CONF_NUM		(u16)32                 //最大会议数
#define LEN_GUID			(u8)16					//GIUD长度
#define LEN_1				(u16)1
#define LEN_TERID			(u16)128				//终端名字最大长度
#define MAX_H264CAP_NUM     (u8)10                  //H.264能力个数
#define LEN_ALIAS			(u16)64				    //最大别名长度h323-id
#define MAX_TERMINAL_NUM 32
#define MAX_SIP_REG_INFO_NUM_UMS	3
#define MAX_SIP_REG_INFO_NUM_CNS	20
#define SIP_CALL_ID_SIZE		128
#define MAX_NUM_REGNAME_SIZE_CNS 2
#define MAX_SIP_CONF_ID_LEN	16
#define MAX_SIP_CONF_NAME_LEN 96
#define MAX_SIP_CONF_E164_LEN 16
#define MAX_SIP_PASSWORD_LEN 32
#define MAX_SIP_TERMINAL_NUM 32

extern u32 g_dwLogOn;

typedef struct tagSipAdapterCfg
{
	BOOL    m_bIfRegister;
	u32     m_dwMaxCalls;      //最大呼叫数
    u32     m_dwLocalIP;             //本端地址.网络序
    u16     m_wLocalUdpPort;  //本地udp侦听地址
    u16     m_wLocalTcpPort;  //本地tcp侦听地址
	BOOL    m_bManualAck;
	u16     m_wRegClientNum;
	u32     m_dwMaxSendReciveBufferSize;

	s8      m_achProduct[64];  //产品信息
	s8      m_achTerVer[128];  //版本信息
public:
	tagSipAdapterCfg()
	{
		Clear();
	}
	void Clear()
	{
		memset(this , 0 , sizeof(tagSipAdapterCfg) );
		m_wRegClientNum = 2;
		m_dwMaxSendReciveBufferSize = 20480;
	}
}TSipAdapterCfg;


enum EmDualFailReason
{
    emMsgBodyLenError, 
    emMsgCapsetError,
	emNoSendingDual,
	emNoRecvingDual,
	emNoOpeningRequest,
	emNoClosingRequest,
	emPrevOperProcessing,

};


enum EmRegFailReason
{
    emSendFail, 
    emUnImplement,
	emExist,
};

//呼叫消息
enum EmSipCallMsgType 
{
	h_call_invalidInterface = 0,
    h_call_make,		            //发起呼叫	      [outgoing]: TCALLPARAM + CSipCapSet(optional)
	h_call_incoming,		        //呼叫到来		  [incoming]: TCALLPARAM + VendorProductId		  	  	
	h_call_answer,		            //接受(拒绝)呼叫  [outgoing]: TNODEINFO + CSipCapSet
	h_call_bye,                    //结束呼叫		  [outgoing]: 1 byte,reason
	h_call_connected,               //呼叫建立		  [incoming]: TNODEINFO	
	h_call_disconnected,            //呼叫断开		  [incoming]: 1 byte,reason	
	h_call_capsetnotify,            //能力级交互      [incoming]: 
	h_dual_open,                    //打开双流
	h_dual_open_accept,
	h_dual_close,                   //关闭双流
	h_dual_close_accept,                   
	h_dual_ack,
	h_dual_reject,
	h_dual_connected,
	h_dual_disconnected,
    h_dual_failed,
	h_call_roundtripdelay,          //roundtrip implemented by options method
	h_call_ack,
};


//信道控制消息
enum EmSipChanCtrlMsgType 
{
	h_chan_invalidInterface = 0,
  	h_chan_videofastupdate,
	h_chan_flowControlCmd,
	h_chan_flowControlInd,
	h_chan_fastupdatewithoutid,
};
//会议控制消息
enum EmSipConfCtrlMsgType
{
	h_conf_invalidInterface = 0,
	h_conf_optionsrequest,//options method request
	h_conf_optionsresponse, //options method response
	h_conf_nonstandard,
	h_conf_Multiple,
};
enum EmSipRegCtrlMsgType
{
	h_sip_registration   = 0, 
	h_sip_unregistration = 1,
	h_sip_online         = 2,
	h_sip_offline        = 3,
	h_sip_state2registered,
	h_sip_state2failed,
	h_sip_state2terminated,
	h_sip_state2redirected,
	h_sip_state2sendmsgfailed,
	
	h_sip_getreginfo,       //获得regserver上的所有注册信息请求
	h_sip_getreginfoack,    //regserver对注册信息请求的回应
	h_sip_getreginfoack_B2,    //regserver对注册信息请求的回应
};
enum EmSipUMSCtrlMsgType
{
	h_ums_invalidInterface = 0,
	h_ums_registrationcmd, //RegisterRequest 注册命令 
	h_ums_registrationind, //注册成功：EmRegisterReason+TRegisterResponse；注册失败：EmRegisterReason (emRegFail); 注册重名 EmRegisterReason(emRegNameDup)+sizeof(s32)+TSipRegName 注册响应
	h_ums_unregistrationcmd,//RegisterRequest+bool(if expire all) 取消注册命令
	h_ums_unregistrationind,//EmRegisterReason 取消注册响应,原因为取消成功或失败
	h_ums_query,       //char(别名) 查询
	h_ums_answer,      //EmRegisterReason+RegisterResponse 回应查询
	h_ums_option,      //服务器向注册终端发送查询指令， 发option消息， 回调也是option消息,用在regserver查询注册是否还在线

	h_ums_getregreq,          //ums或cns向regserver请求获得当前的别名, buf中依次为：s32 nAppid, EmSipGetRegInfoType
	h_ums_getregack,          //regserver的应答, buf中依次为，s32 nAppid, TSipRegPackXml
	h_ums_getregack_B2,          //regserver的应答, buf中依次为，s32 nAppid, TSipRegPackXmlB2, TSipRegInfoUms/TSipRegInfoCns


};
typedef struct tagRegOptionData  //regserver发送的查询指令
{
	u32 dwtime;      //注册句柄
	u32 ip;			 //终端的ip	
	u16 port;        //终端的port
	tagRegOptionData()
	{
		memset(this, 0, sizeof(tagRegOptionData));
	}
}TRegOptionData;

enum EmRegState
{
   emRegIdle,
   emRegRegistering,
   emRegRegistered,
   emRegFailed,
   emUnReg,
};

enum EmSipCallType
{
    emDirect = 0,         //直接呼叫
	emByRedirect,         //通过重定向服务器呼叫
	emByPxy,              //通过代理服务器呼叫
};

//会议操作:请求-应答方式返回结果
enum CtrlResult 
{
	ctrl_err = -1,
	ctrl_granted,		//成功
	ctrl_denied			//拒绝
};

//主机名类型
enum EmHostNameType
{
	emUrlIpv4,
	emUrlIpv6,
	emUrlDomain,
	emUrlAlias,
	emUrlE164Num,
    emUrlNone,
};

enum EmSipMediaType
{
	emSipMediaNone,
    emSipMediaAudio,
	emSipMediaVideo,
	emSipMediaH224,
	emSipMediaT120,
	emSipMediaBfcp,

};
enum  EmSipPayloadType 
{
	emSipPayloadTypeBegin = 0,
    //音频格式
     emAudioTypeBegin,
     emG711a,
     emG711u,
     emG722,
     emG7231,
     emG728,
     emG729,
     emMp3,  
     emG723,
     emG7221,
     emMpegaaclc,
     emMpegaacld,
	 emG719,
     emAudioTypeEnd = 49,
          
    //视频格式
     emVideoTypeBegin = 50,	
     emH261,
     emH263,
     emH263plus,
     emH264,
	 emH262,
	 emMpeg4,
     emVideoTypeEnd = 99,
        
	 emDataTypeBegin = 100,	
     emT120,
     emH224,
     emSipDataTypeEnd = 149,
                
    emSipPayloadTypeEnd = 255
};


//分辨率
enum EmSipResolution
{
	emSipResolutionBegin = 0,
	emSipSQCIF,
	emSipQCIF,
	emSipCIF,
	emSipCIF2,
	emSipCIF4,
	emSipCIF16,
		
	emSipSIF,
	emSipSIF4,
	emSipVGA,
	emSipSVGA,
	emSipXGA,
	
	emSipWCIF4,  //1024*576
	emSipHD720,  //1280*720
	emSipSXGA,   //1280*1024
    emSipUXGA,   //1600*1200
    emSipHD1080, //1920*1080
	emSipAuto,
	emSipResolutionEnd
};



enum EmSipG7221Rate
{
	emG7221Rate24   =  0x01,
	emG7221Rate32   =  0x02,
	emG7221Rate48   =  0x04
};


//加密类型
enum EmSipEncryptType
{
    emSipEncryptTypeNone = 1,//00000001
    emSipEncryptTypeDES  = 2,//00000010
    emSipEncryptTypeAES  = 4,//00000100
};


//设备厂商标识信息
enum EmSipVendorId
{
	emVendorUnknow = 0,
	emMicrosoftNetmeeting = 1,
	emKeda,
	emHuaWei,
	emPolycom,
    emTandberg,
	emRadvision,
	emVcon,
	emSony,
	emVtel,
	emZTE,
	emCodian,
};

enum EmRegisterReason
{
	emRegNone = 0,
		emRegSuc,          // 注册成功
		emRegFail,         //注册失败
		emRegNotFound,
		emRegNameDup,      //注册服务器重名时，服务器发送重名消息
		emRegTimeOut,      //注册服务器不可达，超时
};

typedef enum 
{
	emSendRecvBegin = 0,
		emSendOnly = 1,
		emRecvOnly = 2,
		emSendRecv = 3
}emSendRecvAttr;


enum EmSipConnection
{
	emSipConnectionType = 0,
		emSipTcpConnection,
		emSipUdpConnection,
		emSipSCTPConnection,
		emSipTLSConnection,
};
enum EmSipTerminalType
{
	emSipTerUnknown,
		emSipTerMCU,
		emSipTerMT,
		emSipTerRegister,
};
enum EmSipLevel
{
	emSipCounty,
	emSipCity,
	emSipProvince,
	emSipUnKnown = 255,
};

enum EmSipCapSign
{
	emSipCapBig,  
	emSipCapLeft,
	emSipCapRight,
	emSipCapEnd,
};

//终端编号参数
typedef struct tagTerminalLabel 
{
	u8 m_byMcuNo;
	u8 m_byTerNo;
	
	tagTerminalLabel()
	{
		Clear();		
	}
	
	void Clear()
	{
		m_byMcuNo = 0xff;
		m_byTerNo = 0xff;
	}
	
	bool operator ==(tagTerminalLabel& tLabel)
	{
		if (m_byMcuNo == tLabel.m_byMcuNo && m_byTerNo == tLabel.m_byTerNo)
			return true;
		else
			return false;
	}
	
	bool operator !=(tagTerminalLabel& tLabel)
	{
		if (m_byMcuNo != tLabel.m_byMcuNo || m_byTerNo != tLabel.m_byTerNo)
			return true;
		else
			return false;
	}
    //有效性检查
	bool IsValid()
	{
		return ( m_byMcuNo < LEN_MTNUM_PERCONF && m_byTerNo < LEN_MTNUM_PERCONF);
	}
    
    //设置TerminalLabel
	BOOL SetTerminalLabel( u8 byMcuNo, u8 byTerNo )
	{
		if( byMcuNo < LEN_MTNUM_PERCONF && byTerNo < LEN_MTNUM_PERCONF )
		{
			m_byMcuNo = byMcuNo;
			m_byTerNo = byTerNo;
            return TRUE;
		}
        return FALSE;
	}
    //获取TerminalLabel
	void GetTerminalLabel( u8 *pMcuNo, u8 *pTerNo )
	{
		*pMcuNo = m_byMcuNo;
		*pTerNo = m_byTerNo;
	}
    //获取mcu编号
	u8 GetMcuNo()
	{
		return m_byMcuNo;
	}
    //获取终端编号
	u8 GetTerNo()
	{
		return  m_byTerNo;
	}
}TTERLABEL,*PTTERLABEL;

//终端信息参数
typedef struct tagTerminalInformation 
{
	TTERLABEL	m_tTerLabel;
	s8			m_achTerID[LEN_TERID + 1];

	tagTerminalInformation()
	{
		Clear();
	}

	void Clear()
	{
		m_tTerLabel.Clear();
        memset( m_achTerID, 0, sizeof(m_achTerID) );        
	}	
	
	//设置终端编号
	void SetLabel(const PTTERLABEL ptLabel)
	{
		if (ptLabel != NULL)
		{
			memcpy( &m_tTerLabel, ptLabel, sizeof(m_tTerLabel) );
		}		
	}
	void SetLabel(u8 byMcuNo, u8 byTerNo)
	{
		if (byMcuNo < 193 && byTerNo <193)
		{
			m_tTerLabel.SetTerminalLabel(byMcuNo, byTerNo);
		}
	}
	//获得终端编号
	TTERLABEL& GetTerlabel()
	{
		return m_tTerLabel;
	}
	void GetLabel(u8& byMcuNo, u8& byTerNo)
	{
		m_tTerLabel.GetTerminalLabel( &byMcuNo, &byTerNo );
	}
	//获得Mcu号
	u8 GetMcuNo() 
	{ 
		return m_tTerLabel.GetMcuNo();	
	}
	//获得终端号
	u8 GetTerNo() 
	{ 
		return m_tTerLabel.GetTerNo();	
	}

	//设置终端名字
	BOOL SetName(const s8* pchName)
	{
		if (pchName != NULL)
		{
			strncpy( m_achTerID, pchName, LEN_TERID + 1 );
			m_achTerID[LEN_TERID] = '\0';
			return TRUE;
		}
		return FALSE;
	}
	//获得终端名字
	s8* GetName()
	{
		return m_achTerID;
	}

	void operator = (tagTerminalInformation& tTerInfo)
	{
		m_tTerLabel.SetTerminalLabel( tTerInfo.m_tTerLabel.GetMcuNo(), 
									  tTerInfo.m_tTerLabel.GetTerNo() );

		strncpy( m_achTerID,tTerInfo.GetName(), LEN_TERID + 1);
	}

	bool operator != (tagTerminalInformation& tTerInfo)
	{
		if ( m_tTerLabel.GetMcuNo() == tTerInfo.m_tTerLabel.GetMcuNo() && 
			 m_tTerLabel.GetTerNo() == tTerInfo.m_tTerLabel.GetTerNo() &&
			 strcmp( m_achTerID, tTerInfo.m_achTerID ) == 0 )
		{
			return false;
		}
		return true;
	}

	bool operator == (tagTerminalInformation& tTerInfo)
	{
		if ( m_tTerLabel.GetMcuNo() == tTerInfo.m_tTerLabel.GetMcuNo() && 
			 m_tTerLabel.GetTerNo() == tTerInfo.m_tTerLabel.GetTerNo() &&
			 strcmp( m_achTerID, tTerInfo.m_achTerID ) == 0 )
		{
			return true;
		}
		return false;
	}
}TTERINFO,*PTTERINFO;

//defined in rfc4796
enum EmSipContentAttr
{
	emSipContentNone = 0,
	emSipContentSlides,
	emSipContentSpeaker,
	emSipContentSl,
	emSipContentMain,
	emSipContentAlt,

};

//defined in rfc4145
enum EmSipSetupAttr
{
	emSipSetupBegin = 0,
	emSipActive = 1,
	emSipPassive = 2,
	emSipActpass = 3,
	emSipHoldconn = 4
};

//defined in rfc4145
enum EmSipConnectionAttr
{
	emSipConnectionBegin = 0,
	emSipNew = 1,
	emSipExisting = 2,
};

//defined in rfc4583
enum EmSipFloorcontrolAttr
{
	emSipFloorcontrolBegin = 0,
	emSipClientOnly = 1,
	emSipServerOnly = 2,
	emSipClientServer = 3
};

// enum EmMediaDualState
// {
// 	emDualNone,         //不是双流
// 	emDualStart,		//是双流，初始状态
// 	emDualSuc,			//双流能力匹配状体
// 	emDualFail,			//双流能力不匹配
// };

//加密同步结构  
typedef struct tagSipEncryptSync
{
	s32			  m_nSynchFlag;			         //shall be the Dynamic PayloadType for H.323
	u8			  m_abyKey[LEN_ENCKEY_MAX+1]; //H.235 encoded value(1-65535)
	s32           m_nKeyLen;

    
	void clear()
	{
        m_nSynchFlag = 0;
		m_nKeyLen = 0;
		memset( m_abyKey, 0, sizeof( m_abyKey ) );
	}
    //设置SynchFlag
	void SetSynchFlag(s32 nFlag)
	{ 
		m_nSynchFlag = nFlag; 
	}
    //获取SynchFlag
	s32  GetSynchFlag() 
	{
		return m_nSynchFlag; 
	}

    //设置H235Key
	s32 SetKey(u8 *pBuf, s32 nLen)
	{
		m_nKeyLen = min(max(0, nLen), LEN_ENCKEY_MAX);
		memcpy(m_abyKey, pBuf, m_nKeyLen);
		return m_nKeyLen;
	}	
    //获取H235Key的长度
	s32 GetKeyLen() 
	{
		return m_nKeyLen; 
	}
    //获取H235Key
	u8* GetKey()
	{
		return m_abyKey;
	}
}TSipEncryptSync;

//IP传输地址
typedef struct tagTransAddress
{
	u32 m_dwIP ;//网络序
	u16 m_wPort;//主机序
public:
	void Clear()
	{
		m_dwIP = 0;
		m_wPort = 0;
	}
}TIPTransAddress;


struct TSipCap
{
    u32  m_dwSampleRate;
    EmSipPayloadType m_emPayloadType;
    u16              m_wStaticPayload;
    u16 m_wDynamicPayload;    
public:
	TSipCap()
	{
		Clear();
	}

	void Clear()
	{
		m_emPayloadType = emSipPayloadTypeBegin;
		m_dwSampleRate = 8000;
		m_wDynamicPayload = 0;
	}
	
	void SetPayloadType( EmSipPayloadType emPt )
	{
        m_emPayloadType = emPt;
	}
	
	EmSipPayloadType GetPayloadType()
	{
		return m_emPayloadType;
	}
	
	void SetSampleRate( u32 dwSampleRate )
	{
        m_dwSampleRate = dwSampleRate;
	}
	
	u32 GetSampleRate()
	{
		return m_dwSampleRate;
	}
	
	void SetStaticPayload( u16 wPt )
	{
		m_wStaticPayload = wPt;
	}
	u16 GetStaticPayload()
	{
		return m_wStaticPayload;
	}
	
	void SetDynamicPayload( u16 wDPt )
	{
		m_wDynamicPayload = wDPt;
	}
	u16 GetDynamicPayload()
	{
		return m_wDynamicPayload;
	}
};
//音频能力集，不包括G.7231
struct TSipAudioCap : public TSipCap
{
	u8   m_byPackTime;     //每一RTP包的最大时长,单位ms(1-256); 

public:
	TSipAudioCap()
		:TSipCap()
	{
		m_byPackTime = 30;
	}

	void Clear()
	{
		m_byPackTime = 30;
		TSipCap::Clear();
	};

    //设置打包时间
	void SetPackTime(u8 byPackTime)
	{
		m_byPackTime = byPackTime;
	}
    //获取打包时间
	u8  GetPackTime()
	{
		return m_byPackTime;
	}	

};

//G7231的能力集
struct TSipG7231Cap : public TSipAudioCap
{
	u8				m_byMaxFrameNum;		//单包最大的帧数
	BOOL			m_bSilenceSuppression;	//是否静音抑制
public:
	TSipG7231Cap()
		:TSipAudioCap()
	{
		m_byMaxFrameNum  = 1;			    //目前我们只支持单帧
		m_bSilenceSuppression = TRUE;
	}

	void Clear()
	{
		m_byMaxFrameNum  = 1;			    //目前我们只支持单帧
		m_bSilenceSuppression = TRUE;	    //我们默认支持静音抑制
		TSipAudioCap::Clear();
	}

    //设置是否支持静音
    void SetIsSilenceSuppression(BOOL bSilenceSuppression)
    {
        m_bSilenceSuppression = bSilenceSuppression;
    }
    //是否支持静音
	BOOL IsSilenceSuppression()
	{
		return m_bSilenceSuppression;
	}
    
    //设置每包的最大帧数
	void SetMaxFrameNumPerPack(u8 byMaxFrameNum)
	{
		m_byMaxFrameNum = byMaxFrameNum;
	}
    //获取每包的最大帧数
	u8 GetMaxFrameNumPerPack()
	{
		return m_byMaxFrameNum;
	}
};

struct TSipG7221Cap: public TSipAudioCap
{
	u8  m_byBitRateType;         //encode bitrate
public:
	TSipG7221Cap()
		:TSipAudioCap()
	{
		m_byBitRateType = emG7221Rate24;
	}
	
	void Clear()
	{
		TSipAudioCap::Clear();
    	m_byBitRateType = emG7221Rate24;
	};
	
    //设置rate
	void SetRateType( u8 byRateType )
	{
		m_byBitRateType = byRateType;
	}
	void OrRateType( EmSipG7221Rate emRateType )
	{
	    m_byBitRateType = (m_byBitRateType | emRateType);
	}

	//获取rate
	u8 GetRateType()
	{
		return m_byBitRateType;
	}


	void SetBitRate( u32 dwBitRate )
	{
		m_byBitRateType = 0;
		if ( dwBitRate == 48000 )
			m_byBitRateType |= emG7221Rate48;
        else if ( dwBitRate == 32000 )
			m_byBitRateType |= emG7221Rate32;
        else
			m_byBitRateType |= emG7221Rate24;
	}

	u16 GetBitRate()
	{
		if( m_byBitRateType & emG7221Rate48 )
			return 48000;
		else if ( m_byBitRateType & emG7221Rate32 )
			return 32000;
		else 
			return 24000;
	}

};

//MPEG AAC能力集
struct TAacCap : public TSipAudioCap
{
    enum emAACSampleFreq                    //sampling frequency
    {
        emFs96000 = 0,
        emFs88200 = 1,
        emFs64000 = 2,
        emFs48000 = 3,
        emFs44100 = 4,
        emFs32000 = 5,
        emFs24000 = 6,
        emFs22050 = 7,
        emFs16000 = 8,
        emFs12000 = 9,
        emFs11025 = 10,
        emFs8000  = 11
    };
    
    enum emAACChnlCfg                       //channel config
    {
        emChnlCust   = 0,
        emChnl1      = 1,                   //单声道
        emChnl2      = 2,                   //双声道
        emChnl3      = 3,
        emChnl4      = 4,
        emChnl5      = 5,
        emChnl5dot1  = 6,                   //5.1声道
        emChnl7dot1  = 7                    //7.1声道
    };

    emAACSampleFreq m_emSampleFreq;
    emAACChnlCfg    m_emChnlCfg;
    u16             m_wBitrate;
    u8              m_byMaxFrameNum;

public:
    TAacCap()
    {
        Clear();
    }
    
    void Clear()
    {
        m_emSampleFreq = emFs32000;
        m_emChnlCfg = emChnl2;
        m_byMaxFrameNum  = 1;			    //目前我们只支持单帧
        m_wBitrate = 96;                    //kbps
		TSipAudioCap::Clear();
    }
    
    void SetSampleFreq(emAACSampleFreq emFs)
    {
        m_emSampleFreq= emFs;
    }

    emAACSampleFreq GetSampleFreq() const
    {
        return m_emSampleFreq;
    }

    void SetChnl(emAACChnlCfg emChnl)
    {
        m_emChnlCfg = emChnl;
    }

    emAACChnlCfg GetChnl() const
    {
        return m_emChnlCfg;
    }

    void SetBitrate(u16 wBitrate) 
    {
        m_wBitrate = wBitrate;
    }

    u16 GetBitrate() const
    {
        return m_wBitrate;
    }
};

/*********************关于帧率的设置******************
*1表示29.97, 2表示29.97/2, 3表示29.97/3,.....
*H261：cif(1-4) qcif(1-4)
*H263  sqcif,qcif,cif,4cif,16cif  (1-32)
*****************************************************/
//视频能力集
struct TSipVideoCap : public TSipCap
{
	u16              m_wMaxBitRate;						//单位kbps
	u8               m_abyResolution[emSipResolutionEnd];	//帧率 
    EmSipPayloadType m_emPayloadType;
	u16              m_wStaticPayload;
	u16              m_wDynamicPayload;

public:
	TSipVideoCap()
	{
	    Clear();
	}
	void Clear()
	{
		TSipCap::Clear();

		m_wMaxBitRate = 0;
		memset( m_abyResolution, 0, sizeof( m_abyResolution ) );
		m_dwSampleRate   = 90000;
	}

	//单位kbps
	void SetBitRate(u16 wBitRate)
	{
		m_wMaxBitRate = wBitRate;		
	}
	u16 GetBitRate() const
	{
        return m_wMaxBitRate;
	}
	/*
	 *function: 设置支持的分辨率、帧率
	 *param:	emRes 表示分辨率 byMPI表示帧率,参见结构头说明
     *return:   TRUE or FALSE 
	 */
	BOOL SetResolution( EmSipResolution emRes, u8 byMPI )
	{
		if(emRes <= emSipResolutionBegin || emRes >= emSipResolutionEnd)
		{
			return FALSE;
		}
		m_abyResolution[emRes] = byMPI;
	    return TRUE;
	}
    /*
	 *function: 得到支持的分辨率、帧率
	 *param:	emRes 表示分辨率 byMPI表示帧率,参见结构头说明
     *return:   帧率(0 表示不支持改分辨率)  
	 */
	u8 GetResolution( EmSipResolution emRes ) const
	{
		if(emRes <= emSipResolutionBegin || emRes >= emSipResolutionEnd)
		{
			return 0;
		}
		return m_abyResolution[emRes];
	}
};

//H.264视频能力集
/*********************关于帧率的设置******************
*1表示1fps, 60代表60fps，最大允许帧率可以达到理论值255
*****************************************************/
struct TSipH264VideoCap:public TSipVideoCap
{
public:
    // 协议 
    enum  emH264ProfileMask
    {
        emProfileBaseline = 64,
        emProfileMain = 32,
        emProfileExtend = 16,
        emProfileHigh = 8
    };

    TSipH264VideoCap();  
    void Clear();
        
	// 协议: 支持 emH264ProfileMask 各个参数和它们的逻辑与
    void SetSupportProfile(u8 byProfile)
	{
		m_byProfile = byProfile;
// 		if(m_byProfile < 64)
// 			m_byProfile = 64;
	}
	BOOL IsSupportProfile(u8 byReqProfile) const
	{
		return ((m_byProfile&byReqProfile) == byReqProfile);
	}
	
    u8   GetProfileValue() const
    {
        return m_byProfile;
    }
    
    // 能力集操作

     /*
	 *function: 获取特定分辨率下的帧率
	 *param:	u8 byRes                    分辨率：emResolution 或者 emResolutionHD
     *return:   u8 帧率 frame/s
	 */
    u8   GetResolution(EmSipResolution emRes) const;
    u8   GetResolution(u16 wWidth, u16 wHeight) const;

	/*
	 *function: 特定分辨率下，特定帧率(对于场模式，需要上层转化为帧率传入)是否支持
                至于对端是否支持场格式，要业务根据 IsSupportProfile 自行判断 
	 *param:	u8 byRes                    分辨率：emResolution 或者 emResolutionHD
                u8 byFps                    帧率frame/s
     *return:   TRUE/FALSE
	 */
    BOOL IsSupport(EmSipResolution emRes, u8 byFps) const;
    BOOL IsSupport(u16 wWidth, u16 wHeight, u8 byFps) const;

	/*
	 *function: 设置支持某种分辨率和帧率(对于场模式，需要上层转化为帧率传入)。多种分辨率可以叠加设置
	 *param:	u8 byRes                    分辨率：emResolution 或者 emResolutionHD
                u8 byFps                    帧率frame/s
	 */
    void  SetResolution(EmSipResolution emRes, u8 byFps) ;
    void  SetResolution(u16 wWidth, u16 wHeight, u8 byFps) ;

    // 内部使用函数
    void  SetMaxMBPS(s32 nMax)
    {
        m_nMaxMBPS = nMax;
    }
    s32   GetMaxMBPS() const
    {
        return m_nMaxMBPS;
    }
    void  SetMaxFS(s32 nMax)
    {
        m_nMaxFS = nMax;
    }
    s32   GetMaxFS() const
    {
        return m_nMaxFS;
    }
	void  SetMaxBRandCPB(s32 nMaxBRandCPB)
	{
		m_nMaxBRandCPB = nMaxBRandCPB;
	}
	s32   GetMaxBRandCPB() const
	{
		return m_nMaxBRandCPB;
	}

    //  Level，外部调用无效
    void SetLevel(u8 byLevel)
    {
        m_byLevel = byLevel;
    }
    u8 GetLevel() const
    {
        return m_byLevel;
    }

	s32 GetParaFlag()
	{
		return m_nParaFlag;
	}
    void SetParaFlag(s32 nParaFlag)
    {
        m_nParaFlag = nParaFlag;
    }

    // toolkit: Res enum to width*height
    BOOL  Res2WxH( EmSipResolution emRes, u16 &wWidth, u16 &wHeight) const;

	BOOL operator <= (const TSipH264VideoCap& tH264VideoCap);
	void operator = (const TSipH264VideoCap& tH264VideoCap);
	BOOL operator == (const TSipH264VideoCap& tH264VideoCap);


protected:

    // H.264 参数定义，参见 ITU H.241文档
    u8              m_byProfile;
    u8              m_byLevel;
    // 以下2个字段，取 Custom 与Level值默认的 max(单位均为宏块)
    s32             m_nMaxMBPS;
    s32             m_nMaxFS;
    // 以下保存的是绝对 Custom 的字段（暂时不支持设置）
    s32             m_nMaxDPB;
    s32             m_nMaxBRandCPB;
    s32             m_nMaxStaticMBPS;

	// 以下是标志字段,如果参数偏大相应位置位
	s32             m_nParaFlag;
public:
	BOOL32          m_bOld264Level;
	BOOL			m_bUsed;

};


struct TSipDataCap : public TSipCap
{
	u32   m_dwMaxBitRate;					//单位kbps; 

public:
	TSipDataCap()
	{
		Clear();
	}

	void Clear()
	{
		TSipCap::Clear();
		m_dwMaxBitRate = 64;
		m_dwSampleRate = 0;
	}

	//单位kbps
	void SetBitRate(u32 dwBitRate)
	{
		m_dwMaxBitRate = dwBitRate;		
	}
	u32 GetBitRate()
	{
        return m_dwMaxBitRate;
	}		

};

class CBfcpMediaStream
{
public:
	CBfcpMediaStream();
	
	void Clear();

	void operator = (const CBfcpMediaStream& cCopyMedia);
	
	void SetMediaType( EmSipMediaType emMediaType )
	{
		m_emMediaType = emMediaType;
	}

	EmSipMediaType GetMediaType()
	{
		return m_emMediaType;
	}

	void SetBfcpMediaPort( u16 wPort )
	{
		m_wPort = wPort;
	}
	
	u16 GetBfcpMediaPort()
	{
		return m_wPort;
	}

	void SetBfcpFloorcontrolAttr( EmSipFloorcontrolAttr emFloorcontrolAttr )
	{
		m_emFloorcontrolAttr = emFloorcontrolAttr;
	}
	
	EmSipFloorcontrolAttr GetBfcpFloorcontrolAttr()
	{
		return m_emFloorcontrolAttr;
	}

	void SetBfcpSetupAttr( EmSipSetupAttr emSetupAttr )
	{
		m_emSetupAttr = emSetupAttr;
	}

	EmSipSetupAttr GetBfcpSetupAttr()
	{
		return m_emSetupAttr;
	}

	void SetBfcpConnectionAttr( EmSipConnectionAttr emConnectionAttr )
	{
		m_emConnectionAttr = emConnectionAttr;
	}
	
	EmSipConnectionAttr GetBfcpConnectionAttr()
	{
		return m_emConnectionAttr;
	}

	void SetHaveFloorFlag( BOOL bFlag )
	{
		m_bHaveFloor = bFlag;
	}

	s32 GetHaveFloorFlag()
	{
		return m_bHaveFloor;
	}
	
	void SetConfId( s32 nConfId )
	{
		m_nConfId = nConfId;
	}

	s32 GetConfId()
	{
		return m_nConfId;
	}

	void SetUserId( s32 nUserId )
	{
		m_nUserId = nUserId;
	}

	s32 GetUserId()
	{
		return m_nUserId;
	}

	void SetFloorId( s32 nIndex, s32 nFloorId )
	{
		m_anFloorId[nIndex] = nFloorId;
	}

	s32 GetFloorId( s32 nIndex )
	{
		return m_anFloorId[nIndex];
	}

	void SetFloorMedia( s32 nRow, s32 nCol, s32 nMediaId )
	{
		m_anFloorMedia[nRow][nCol] = nMediaId;
	}

	s32 GetFloorMedia( s32 nRow, s32 nCol )
	{
		return m_anFloorMedia[nRow][nCol];
	}

	s32 GetIdleArrayIndex()
	{
		s32 nIndex = -1;
		for ( s32 i = 0; i < MAX_FLOOR_NUM; i++ )
		{
			if ( m_anFloorMedia[i][0] == -1)
			{
				nIndex = i;
				break;
			}
		}

		return nIndex;
	}

	s32 GetArrayIndexAccordFloorId( s32 nFloorId )
	{
		s32 nIndex = -1;
		for ( s32 i = 0; i < MAX_FLOOR_NUM; i++ )
		{
			if ( m_anFloorId[i] == nFloorId )
			{
				nIndex = i;
				break;
			}
		}

		return nIndex;
	}

	s32 GetIdleFloorId()
	{
		s32 j = 0;
		s32 nFloorId = 0;
		for ( s32 i = 0; i < MAX_FLOOR_NUM; i++ )
		{
			for ( j = 0; j < MAX_FLOOR_NUM; j++ )
			{
				if ( m_anFloorId[j] == i + 1 )
				{
					break;
				}
			}
			if ( j == MAX_FLOOR_NUM )
			{
				nFloorId = i + 1;
				break;
			}
		}

		return nFloorId;
	}
	
private:
	EmSipMediaType m_emMediaType;
		
	u16 m_wPort;
	EmSipFloorcontrolAttr m_emFloorcontrolAttr;
	EmSipSetupAttr m_emSetupAttr;
	EmSipConnectionAttr m_emConnectionAttr;
	
	BOOL m_bHaveFloor;
	s32 m_nConfId;
	s32 m_nUserId;
	s32 m_anFloorId[MAX_FLOOR_NUM];
	s32 m_anFloorMedia[MAX_FLOOR_NUM][MAX_MEDIA_NUM_PER_FLOOR];
	
};

class CFeccMediaStream
{
public:
	CFeccMediaStream();
	
	void Clear();

	void operator = (const CFeccMediaStream& cCopyMedia);
	
	void SetMediaType( EmSipMediaType emMediaType )
	{
		m_emMediaType = emMediaType;
	}

	EmSipMediaType GetMediaType()
	{
		return m_emMediaType;
	}
    
	void SetConnectAttribute( emSendRecvAttr emConnectAttr )
	{
		m_emSendRecvAttr = emConnectAttr;
	}

	emSendRecvAttr GetConnectAttribute()
	{
		return m_emSendRecvAttr;
	}
	
	void SetLocalIp( u32 dwIp )
	{
		m_dwLocalIp = dwIp;
	}

	u32 GetLocalIp()
	{
		return m_dwLocalIp;
	}

	void SetLocalSendAddress( u16 wPort )
	{
		m_tLocalSendAddress.m_dwIP = m_dwLocalIp;
		m_tLocalSendAddress.m_wPort = wPort;
	}

	TIPTransAddress* GetLocalSendAddress()
	{
		return &m_tLocalSendAddress;
	}
	
	void SetLocalRecvAddress( u16 wPort )
	{
		m_tLocalRecvAddress.m_dwIP = m_dwLocalIp;
		m_tLocalRecvAddress.m_wPort = wPort;
	}
	
	TIPTransAddress* GetLocalRecvAddress()
	{
		return &m_tLocalRecvAddress;
	}

	void SetRemoteIp( u32 dwIp )
	{
		m_dwRemoteIp = dwIp;
	}
	
	u32 GetRemoteIp()
	{
		return m_dwRemoteIp;
	}

	void SetRemotePort( u16 wPort )
	{
		m_wRemotePort = wPort;
	}
	
	u16 GetRemotePort()
	{
		return m_wRemotePort;
	}

	void SetFeccSampleRate( u16 wSampleRate )
	{
		m_wSampleRate = wSampleRate;
	}
	
	u16 GetFeccSampleRate()
	{
		return m_wSampleRate;
	}

private:
	EmSipMediaType m_emMediaType;
	emSendRecvAttr m_emSendRecvAttr;
	u32 m_dwLocalIp;
	u32 m_dwRemoteIp;
	u16 m_wRemotePort;
	TIPTransAddress	m_tLocalSendAddress;
	TIPTransAddress	m_tLocalRecvAddress;
	u16 m_wSampleRate;
};

//信道参数
class CSipMediaStream 
{       
public:
    enum { emAVCapNum = 20 };

	CSipMediaStream();
	CSipMediaStream( EmSipMediaType emMediaType );

	void operator = (const CSipMediaStream& cCopyMedia);

	void Clear();
	void ClearCapset();

    //设置RTP参数
	void SetLocalRtp( u16 wPort ) 
	{ 
		m_tLocalRtp.m_dwIP   = m_dwLocalIp;
		m_tLocalRtp.m_wPort  = wPort;
	}
    //得到RTP参数
	TIPTransAddress& GetLocalRtp() 
	{
		return m_tLocalRtp; 
	}

	void SetRemoteRtp( u32 dwIp, u16 wPort ) 
	{ 
		m_tRemoteRtp.m_dwIP =  dwIp;
		m_tRemoteRtp.m_wPort  =  wPort;
	}

	//得到RTP参数
	TIPTransAddress& GetRemoteRtp() 
	{
		return m_tRemoteRtp; 
	};

    //设置RTCP参数
	void SetRecvRtcp( u16 wPort ) 
	{
		m_tRecvRtcp.m_dwIP = m_dwLocalIp;
		m_tRecvRtcp.m_wPort = wPort;
	}
    //得到RTCP参数
	TIPTransAddress& GetRecvRtcp() 
	{
		return m_tRecvRtcp; 
	}
	
	 //设置RTCP参数
	void SetSendRtcp( u32 dwIp, u16 wPort ) 
	{
		m_tSendRtcp.m_dwIP = dwIp; 
        m_tSendRtcp.m_wPort = wPort;
	}
    //得到RTCP参数
	TIPTransAddress& GetSendRtcp() 
	{
		return m_tSendRtcp; 
	}

    //设置加密类型
    void SetEncryptType(EmSipEncryptType emType)
	{
		m_emEncryptType = emType; 
	}
    //得到加密类型
	EmSipEncryptType GetEncryptType()
	{
		return m_emEncryptType;
	}

	void SetActivePT( EmSipPayloadType  emActivePT )
	{
		m_emActivePT = emActivePT;
	}

	EmSipPayloadType GetActivePT()
	{
		return m_emActivePT;
	}

	void SetMediaType( EmSipMediaType emMediaType )
	{
		m_emMediaType = emMediaType;
	}

	EmSipMediaType GetMediaType()
	{
		return m_emMediaType;
	}
     //设置音视频、数据流参数
	BOOL AddMedia( EmSipPayloadType emPayloadType, TSipCap *ptCap );
    //设置音视频、数据流参数
    BOOL GetMedia( EmSipPayloadType emPayloadType, TSipCap **ptCap, u8 byIndex = 0);
	void RemoveAllMedia();

    BOOL IsSupport( EmSipPayloadType emPayloadType, TSipCap *ptCap, s32 nIndex = -1 );

	EmSipPayloadType GetMediaPT( u8 byIndex )
	{
        if ( byIndex < m_byMediaCount )
			return m_aemMediaPT[byIndex];
		else
			return emSipPayloadTypeEnd;
	}

	u8 GetMediaCount() 
	{
		return m_byMediaCount;
	}
    //设置加密同步信息
	void SetEncryptSync( TSipEncryptSync &tSync)
	{ 
		m_tEncryptSync = tSync; 
	}
    //得到加密同步信息
	TSipEncryptSync &GetEncryptSync() 
	{ 
		return m_tEncryptSync; 
	} 

	void SetBitRate( s32 nRate )
	{
		m_nBitRate = nRate;
		m_tH261Cap.SetBitRate( nRate );
		m_tH263Cap.SetBitRate( nRate );
		m_tH263plusCap.SetBitRate( nRate );
		m_tMpeg4Cap.SetBitRate( nRate );
		m_tH262Cap.SetBitRate( nRate ); 
		for ( s32 i = 0; i < MAX_H264CAP_NUM; i++ )
		{
			if ( m_atH264Cap[i].GetBitRate() == 0 )
			{
				m_atH264Cap[i].SetBitRate( nRate );
			}
		}
	}

	s32 GetBitRate()
	{
		return m_nBitRate;
	}

	void SetLocalIp( u32 dwIp )
	{
		m_dwLocalIp = dwIp;
	}
	u32 GetLocalIp()
	{
		return m_dwLocalIp;
	}

	BOOL IsActive()
	{
        return m_bActive;
	}

	void SetActive( BOOL bActive )
	{
		m_bActive = bActive;
	}
	void SetMediaPTArray( s32 nPos, EmSipPayloadType emSipPT )
	{
		if ( nPos < m_byMediaCount )
		{
			BOOL bFind = FALSE;
			for ( int i = 0; i< emAVCapNum; ++i )
			{
				if( m_aemMediaPT[i] == emSipPT && emSipPT != emH264 )
				{
					bFind = TRUE;
				}
				else
					continue;
			}
			if ( !bFind )
			{
				m_aemMediaPT[nPos] = emSipPT;
			}
		}
	}
	void SetMediaCount( s32 nCount )
	{
		m_byMediaCount = nCount;
	}
	
	s32 GetScreenNo()
	{
		return m_nScreenNum;
	}
	void SetScreenNo( s32 nScreenNo )
	{
		m_nScreenNum = nScreenNo;
	}
	s32 GetCapSignNum()
	{
		return m_nCapSignNum;
	}
	void SetCapSignNum( s32 nCapSignNum )
	{
		m_nCapSignNum = nCapSignNum;
	}
	EmSipCapSign GetCapSign( s32 nIndex = 0 )
	{
		return m_emSipCapSign[nIndex];
	}
	void SetCapSign( EmSipCapSign emCapSign, s32 nIndex = 0 )
	{
		m_emSipCapSign[nIndex] = emCapSign;
	}
	void SetAudMix(BOOL32 bAudMix)
	{
		m_bAudMix = bAudMix;
	}
	BOOL32 IsAudMix()
	{
		return m_bAudMix;
	}
	void SetTpRtpAddress( s32 nIndex, u32 dwIP, u16 wPort ) 
	{ 
		m_tTpRtpAddr[nIndex].m_dwIP = dwIP;
		m_tTpRtpAddr[nIndex].m_wPort = wPort;
	}
	TIPTransAddress& GetTpRtpAddress( s32 nIndex ) 
	{
		return m_tTpRtpAddr[nIndex]; 
	}
	void SetTpRtcpAddress( s32 nIndex, u32 dwIP, u16 wPort ) 
	{ 
		m_tTpRtcpAddr[nIndex].m_dwIP = dwIP;
		m_tTpRtcpAddr[nIndex].m_wPort = wPort;
	}
	TIPTransAddress& GetTpRtcpAddress( s32 nIndex ) 
	{
		return m_tTpRtcpAddr[nIndex]; 
	}

	void SetSipContentAttr( EmSipContentAttr emSipContentAttr)
	{
		m_emSipContent = emSipContentAttr;
	}

	EmSipContentAttr GetSipContentAttr()
	{
		return m_emSipContent;
	}

	void SetSipMediaLabel( s32 nSipMediaLabel )
	{
		m_nSipMediaLabel = nSipMediaLabel;
	}

	s32 GetSipMediaLabel()
	{
		return m_nSipMediaLabel;
	}
	
	void SetH264CapsetNumber( u8 byNum )
	{
		m_byH264CapNum = byNum;
	}
	
	u8 GetH264CapsetNumber()
	{
		return m_byH264CapNum;
	}

	void AddH264CapsetNumber()
	{
		m_byH264CapNum++;
	}

	void SetH264CapsetOrder( s32 nIndex, u8 byOrder )
	{
		m_abyH264CapOrder[nIndex] = byOrder;
	}
	
	u8 GetH264CapsetOrder( s32 nIndex )
	{
		return m_abyH264CapOrder[nIndex];
	}
private:

	BOOL m_bActive;
	EmSipMediaType    m_emMediaType;
  
	TIPTransAddress	  m_tLocalRtp;			
	TIPTransAddress	  m_tRemoteRtp;			
	
	TIPTransAddress	  m_tSendRtcp;   	
	TIPTransAddress   m_tRecvRtcp;

	EmSipPayloadType m_aemMediaPT[emAVCapNum];
	u8 m_byMediaCount;
	u8 m_byH264CapNum;	
	u8 m_abyH264CapOrder[MAX_H264CAP_NUM];

	TSipVideoCap m_tH261Cap;
	TSipVideoCap m_tH263Cap;
	TSipVideoCap m_tH263plusCap;
	TSipVideoCap m_tMpeg4Cap;
    TSipVideoCap m_tH262Cap;    
    TSipH264VideoCap m_atH264Cap[MAX_H264CAP_NUM];

	TSipAudioCap m_tG711u;   
	TSipAudioCap m_tG711a;   
	TSipAudioCap m_tG722;    
	TSipAudioCap m_tG728;	   
	TSipAudioCap m_tG729;
	TSipAudioCap m_tMP3;
	TSipAudioCap m_tG723;


	TAacCap m_taaclc[MAX_AAC_CAP_NUM];     //aaclc数组
	u8 m_bylcNum;                          //数组大小
	u8 m_bylcMediaToCap[emAVCapNum];	   //是当前m_byMediaCount的索引 对应到aaclc数组的索引
	TAacCap m_taacld[MAX_AAC_CAP_NUM];
	u8 m_byldNum;
	u8 m_byldMediaToCap[emAVCapNum];


	TSipAudioCap m_tG719;
     
	TSipG7231Cap  m_tG7231;
	TSipG7221Cap  m_tG7221;

	TSipDataCap  m_tT120;
	TSipDataCap  m_tH224;

	EmSipPayloadType  m_emActivePT;
        
	EmSipEncryptType m_emEncryptType;	   //加密类型
	TSipEncryptSync  m_tEncryptSync;       //master需设置此结构         
	s32 m_nBitRate;
	u32 m_dwLocalIp;
	
	//用于网真
	s32			 m_nScreenNum;
	s32			 m_nCapSignNum;
	BOOL32       m_bAudMix;        //是否混音
	EmSipCapSign m_emSipCapSign[emSipCapEnd];
	TIPTransAddress	  m_tTpRtpAddr[emSipCapEnd];			
	TIPTransAddress	  m_tTpRtcpAddr[emSipCapEnd];

	//用于BFCP，添加content和label属性
	EmSipContentAttr    m_emSipContent;
	s32                 m_nSipMediaLabel;
};

typedef struct tagMediaLine
{
	emSendRecvAttr emAttr;
	u8			   bySndPos;
	u8             byRcvPos;
}tMediaLine; 

/************************************
* 能力集结构
* 目前的音频支持:G711u(64k),G711a(64k),G722(64k),G7231,G728,G729.
* 目前的视频第一路支持:h261,h263,h264,mpeg4,h262
* 第一路支持:,h264,h263+  
* 
 *************************************/
class CSipCapSet
{
public:
	enum {  
		MAX_AUDIO_STREAM_COUNT = 6,
		MAX_VIDEO_STREAM_COUNT = 10,
	};
	
	CSipCapSet();

	void Clear();

	/*whether media support some format*/
    BOOL IsSupportCapset( EmSipPayloadType emPayloadType, TSipCap *ptSupportCapset, BOOL bRecv, s32 nVideoSequence=0 );
    
	/*add one format to a media*/
	void AddCapSetToMedia( EmSipPayloadType emPayloadType, TSipCap *ptCap, BOOL bRecv, int nVideoSequence = 0 );
	/*determine which payload to be transmitted*/
	void SetMediaActivePT( EmSipPayloadType emPayloadType, BOOL bRecv, int nVideoSequence = 0  );

	s32 GetVideoSendStreamCount(){ return m_nVideoSendStreamCount;	}
	s32 GetVideoRecvStreamCount(){ return m_nVideoRecvStreamCount;	}

	void SetVideoRecvStreamCount( s32 nCount )
	{
		if ( nCount > MAX_VIDEO_STREAM_COUNT )
		{
			m_nVideoRecvStreamCount = MAX_VIDEO_STREAM_COUNT;
			return;
		}
		m_nVideoRecvStreamCount = nCount;
	}

	void SetVideoSendStreamCount( s32 nCount )
	{
		if ( nCount > MAX_VIDEO_STREAM_COUNT )
		{
			m_nVideoSendStreamCount = MAX_VIDEO_STREAM_COUNT;
			return;
		}
		m_nVideoSendStreamCount = nCount;
	}

	void IncreasingSendVideoStream()
	{ 
		if ( m_nVideoSendStreamCount+1 <= MAX_VIDEO_STREAM_COUNT )
		{
			m_nVideoSendStreamCount++; 
		}
	}
	void IncreasingRecvVideoStream()
	{ 
		if ( m_nVideoRecvStreamCount+1 <= MAX_VIDEO_STREAM_COUNT )
		{
			m_nVideoRecvStreamCount++; 
		}
	}

	s32 GetAudioSendStreamCount(){ return m_nAudioSendStreamCount;	}

	s32 GetAudioRecvStreamCount(){ return m_nAudioRecvStreamCount;	}

	void SetAudioRecvStreamCount( s32 nCount )
	{
		if ( nCount > MAX_AUDIO_STREAM_COUNT )
		{
			m_nAudioRecvStreamCount = MAX_AUDIO_STREAM_COUNT;
			return;
		}
		m_nAudioRecvStreamCount = nCount;
	}
	
	void SetAudioSendStreamCount( s32 nCount )
	{
		if ( nCount > MAX_AUDIO_STREAM_COUNT )
		{
			m_nAudioSendStreamCount = MAX_AUDIO_STREAM_COUNT;
			return;
		}
		m_nAudioSendStreamCount = nCount;
	}

	void IncreasingSendAudioStream()
	{ 
		if ( m_nAudioSendStreamCount+1 <= MAX_AUDIO_STREAM_COUNT )
		{
			m_nAudioSendStreamCount++; 
		}
	}
	void IncreasingRecvAudioStream()
	{ 
		if ( m_nAudioRecvStreamCount+1 <= MAX_AUDIO_STREAM_COUNT )
		{
			m_nAudioRecvStreamCount++; 
		}
	}
	
	u32 GetMediaLinePosbySendPos(u8 bySndPos, BOOL bVideo = TRUE)
	{
		if(bVideo)
		{
			for(u32 dwIndex = 0; dwIndex< m_dwVideoNum; dwIndex++)
			{
				if(m_atVidSndRcv[dwIndex].emAttr == emSendOnly || m_atVidSndRcv[dwIndex].emAttr == emSendRecv)
				{
					if(bySndPos == m_atVidSndRcv[dwIndex].bySndPos )
						return dwIndex;	
				}
			}
		}
		else
		{
			for(u32 dwIndex = 0; dwIndex< m_dwAudioNum; dwIndex++)
			{
				if(m_atAudSndRcv[dwIndex].emAttr == emSendOnly || m_atAudSndRcv[dwIndex].emAttr == emSendRecv)
				{
					if(bySndPos == m_atAudSndRcv[dwIndex].bySndPos )
						return dwIndex;	
				}
			}
		}

		return -1;
	}

	u32 GetMediaLinePosbyRecvPos(u8 byRecvPos, BOOL bVideo = TRUE)
	{
		if(bVideo)
		{
			for(u32 dwIndex = 0; dwIndex< m_dwVideoNum; dwIndex++)
			{
				if(m_atVidSndRcv[dwIndex].emAttr == emRecvOnly || m_atVidSndRcv[dwIndex].emAttr == emSendRecv)
				{
					if(byRecvPos == m_atVidSndRcv[dwIndex].byRcvPos )
						return dwIndex;	
				}
			}
		}
		else 
		{
			for(u32 dwIndex = 0; dwIndex< m_dwAudioNum; dwIndex++)
			{
				if(m_atAudSndRcv[dwIndex].emAttr == emRecvOnly || m_atAudSndRcv[dwIndex].emAttr == emSendRecv)
				{
					if(byRecvPos == m_atAudSndRcv[dwIndex].byRcvPos )
						return dwIndex;	
				}
			}
		}

		
		return -1;
	}

	tMediaLine* GetMediaLine( u32 dwCount , BOOL bIfVideo = FALSE )
	{
		if ( bIfVideo)
		{
			if ( dwCount >= MAX_VIDEO_STREAM_COUNT*2 || dwCount < 0 )
				return NULL;
			else
				return &(m_atVidSndRcv[dwCount]);
		}
		else
		{
			if ( dwCount >= MAX_AUDIO_STREAM_COUNT*2 || dwCount < 0 )
				return NULL;
			else
				return &(m_atAudSndRcv[dwCount]);
		}
	
	}

	BOOL SetMediaLine( u32 dwCount , emSendRecvAttr emAttr , u8 bySndPos , u8 byRcvPos , BOOL bIfVideo = FALSE )
	{
		if ( bIfVideo)
		{
			if ( dwCount >= MAX_VIDEO_STREAM_COUNT*2 || dwCount < 0 )
				return FALSE;
			else{
					m_atVidSndRcv[dwCount].emAttr = emAttr;
					m_atVidSndRcv[dwCount].bySndPos = bySndPos;
					m_atVidSndRcv[dwCount].byRcvPos = byRcvPos;

				}
		}
		else
		{
			if ( dwCount >= MAX_AUDIO_STREAM_COUNT*2 || dwCount < 0 )
				return FALSE;
			else{
					m_atAudSndRcv[dwCount].emAttr = emAttr;
					m_atAudSndRcv[dwCount].bySndPos = bySndPos;
					m_atAudSndRcv[dwCount].byRcvPos = byRcvPos;
				}
		}
		return TRUE;
	}

	BOOL SetAudioNum(u32 AudioNum)
	{
		if ( AudioNum > MAX_AUDIO_STREAM_COUNT*2 || AudioNum < 0 )
			return FALSE;
		m_dwAudioNum = AudioNum;
		return TRUE;
	}
	u32 GetAudioNum()
	{
		return m_dwAudioNum;
	}
	BOOL SetVideoNum(u32 VideoNum)
	{
		if ( VideoNum > MAX_VIDEO_STREAM_COUNT*2 || VideoNum < 0 )
			return FALSE;
		m_dwVideoNum = VideoNum;
		return TRUE;
	}
	u32 GetVideoNum()
	{
		return m_dwVideoNum;
	}

	/*set dialog bandwidth*/
	void SetBandwidth( s32 nBandWidth );
    s32  GetBandwidth();

	void SetLocalIp( u32 dwIp );
	u32  GetLocalIp();

	void SetMediaLocalRtp( EmSipMediaType emMediaType, u16 wRtpPort, BOOL bRecv, int nSequence = 0 );
	TIPTransAddress GetMediaLocalRtp( EmSipMediaType emMediaType, BOOL bRecv, int nSequence=0 );

	void SetMediaRemoteRtp( EmSipMediaType emMediaType, u32 dwIp, u16 wRtpPort, BOOL bRecv, int nSequence = 0 );
	TIPTransAddress GetMediaRemoteRtp( EmSipMediaType emMediaType, BOOL bRecv, int nSequence = 0 );

	void SetMediaSendRtcp( EmSipMediaType emMediaType, u32 dwIp, u16 wPort, BOOL bRecv, int nSequence = 0 );
    TIPTransAddress GetMediaSendRtcp( EmSipMediaType emMediaType, BOOL bRecv, int nSequence = 0 );

	void SetMediaRecvRtcp( EmSipMediaType emMediaType, u16 wRtcpPort, BOOL bRecv, int nSequence = 0 );
	TIPTransAddress GetMediaRecvRtcp( EmSipMediaType emMediaType, BOOL bRecv, int nSequence = 0 );


	/*get some media stream of a dialog*/
	CSipMediaStream* GetSendMediaStream( EmSipMediaType emMediaType, int nSequence = 0 );
	CSipMediaStream* GetRecvMediaStream( EmSipMediaType emMediaType, int nSequence = 0 );
	BOOL SetLocalVedioIP( u32 dwIp, s32 nSequence );
	BOOL SetLocalAudioIP( u32 dwIp, s32 nSequence );

	void SetBfcpMedia( CBfcpMediaStream& tBfcpMedia );
	CBfcpMediaStream GetBfcpMedia();

	void SetFeccMedia( CFeccMediaStream& tFeccMedia );
	CFeccMediaStream GetFeccMedia();
 
private:

// 	void BuildSendStream( EmSipMediaType emMediaType, CSipMediaStream *pcLocalSendStream, 
// 						 CSipMediaStream *pcPeerRecvStream, CSipMediaStream &cCommonStream );
// 	
//     void BuildRecvStream( EmSipMediaType emMediaType, CSipMediaStream *pcLocalRecvStream, 
// 						 CSipMediaStream *pcPeerSendStream, CSipMediaStream &cCommonStream );

	CSipMediaStream m_atAudioSendStream[MAX_AUDIO_STREAM_COUNT];
	int m_nAudioSendStreamCount;
    CSipMediaStream m_atAudioRecvStream[MAX_AUDIO_STREAM_COUNT];
	int m_nAudioRecvStreamCount;

	tMediaLine m_atAudSndRcv[MAX_AUDIO_STREAM_COUNT*2];
	u32 m_dwAudioNum;

	CSipMediaStream m_atVideoSendStream[MAX_VIDEO_STREAM_COUNT];
	int m_nVideoSendStreamCount;
	CSipMediaStream m_atVideoRecvStream[MAX_VIDEO_STREAM_COUNT];
	int m_nVideoRecvStreamCount;

	tMediaLine m_atVidSndRcv[MAX_VIDEO_STREAM_COUNT*2];
	u32 m_dwVideoNum;
	
	CSipMediaStream m_tFeccSendStream;
	CSipMediaStream m_tFeccRecvStream;

	CBfcpMediaStream m_tBfcpMediaStream;
	CFeccMediaStream m_tFeccMediaStream;

	s32 m_nBandWidth;
	u32 m_dwLocalIp;


};

typedef struct tagHostName
{
	EmHostNameType   m_emType;                   //主机名类型
	char             m_achDomainName[LEN_DOMAINNAME+1];//域名
	u32              m_dwIpAddr;                   //IP地址
}THostName;


#define IsValidHostNameAddr( hostname ) \
( ( hostname.m_emType == emUrlIpv4 && hostname.m_dwIpAddr ) \
	|| ( hostname.m_emType == emUrlDomain && strlen( hostname.m_achDomainName ) ) ) \
	?TRUE:FALSE \
   

typedef struct tagSipUrl
{
	char             m_achDisplayName[LEN_USERNAME+1]; 
	char			 m_achUserName[LEN_USERNAME+1];      //用户名   
	char             m_achPassword[LEN_PASSWORD+1];      //密码
    THostName        m_tHostName;                        //主机名
	u16              m_wPort;                            //端口    
public:
	tagSipUrl()
	{
		memset( this, 0, sizeof( tagSipUrl ) );
	}
}TSipUrl;

typedef enum
{
    cmCreate,
    cmJoin,
    cmInvite,
    cmCapabilityNegotiation,
    cmCallIndependentSupplementaryService,
    cmLastCG
} EmConfGoalType;

// 呼叫地址类型
enum EmSipCallAddrType
{
	cat_err = -1,
	cat_ip  = 1, 	//IP呼叫
	cat_alias	   	//别名呼叫
};

//别名地址
typedef struct tagAliasTransAddress
{
	s8  m_achAlias[LEN_ALIAS + 1];			//h323-Id 别名后两位为空
	
	tagAliasTransAddress()
	{
		Clear();
	}
	
	void Clear()
	{
		memset( m_achAlias, 0, sizeof(m_achAlias) );
	}
	
	//设置别名
	BOOL SetAliasAddr( const s8* pAlias )
	{
		memset( m_achAlias, 0, sizeof(m_achAlias) );
		u16 nLen = min( strlen(pAlias), LEN_ALIAS );
		strncpy( m_achAlias, pAlias, nLen );
		m_achAlias[nLen] = '\0';
		return TRUE;
	}	
	
	//获得别名
	s8* GetAliasName()  
	{
		return m_achAlias;	
	}
}TAliasTransAddress,*PTAliasTransAddress;

//呼叫时的别名地址
typedef struct tagSipTransAddress
{
	EmSipCallAddrType    m_emType;		//呼叫类型
	TIPTransAddress		m_tNetAddr;		//IP呼叫时使用		
	TAliasTransAddress		m_tAlias;		//别名呼叫使用

	tagSipTransAddress()
	{
		Clear();
	}
	void Clear()
	{
		m_emType = cat_err;
		m_tNetAddr.Clear();
		m_tAlias.Clear();
	}

}TSipTransAddress,*PTSipTransAddress;

//基本会议信息
typedef struct tagTSipConfInfo
{
	s8	 achConfId[MAX_SIP_CONF_ID_LEN + 1];			// 会议ID
	s8	 achConfName[MAX_SIP_CONF_NAME_LEN + 1];		// 会议名
	s8	 achConfNumber[MAX_SIP_CONF_E164_LEN + 1];	    // 会议号码
	s8	 achConfPwd[MAX_SIP_PASSWORD_LEN + 1];			// 会议密码
	BOOL bNeedPassword;                             // 是否需要密码
	u16  wConfDuration;							    // 会议持续时间		
	EmSipPayloadType   emVideoFormat;				// 会议视频格式(VIDEO_H261等)
	EmSipPayloadType   emAudioFormat;				// 会议音频格式(AUDIO_MP3等)
	EmSipResolution  emResolution;					// 会议视频分辨率(VIDEO_CIF等)
	u8 byProfile;									// 主流profile
	u8 byVidMPI;					                // 主流帧率
	BOOL bIsAutoVMP;								// 是否自动画面合成
	BOOL bIsMix;	    							// 是否混音
	EmSipPayloadType	emSecVideoFormat;			// 双流格式
	EmSipResolution emSecVidRes;					// 双流分辨率
	u8 bySecProfile;								// 双流profile
	u8 bySecVidMPI;					                // 双流帧率

public:
	tagTSipConfInfo()
	{ 
		Clear();	
	}
	void Clear()
	{
		memset( this, 0, sizeof(tagTSipConfInfo) );
	}

}TSipConfInfo ,*PTSipConfInfo;

//发起呼叫参数
typedef struct tagSipCallParam 
{
	BOOL	m_bCreateConf;
	EmConfGoalType  m_emConfGoalType;  				  //会议的目标类型
	u32				 m_dwCallRate;                    //呼叫速率	(kbps)
	TSipUrl          m_tCalledAddress;                //被叫地址
	TSipUrl          m_tCallingAddress;               //主叫地址
	EmSipCallType    m_emCallType;                    //呼叫类型 (direct|by pxy|by redirect)
    THostName        m_tServerAddr;                    //pxy server| redirect server    
	u16              m_wServerPort;
	EmSipConnection  m_emSipConnection;               //信令连接类型 tcp udp default tcp
	s32					  m_nProductIdSize;
	u8					  m_abyProductId[MAX_NUM_LEN_VERSION];				//产品号
	s32					  m_nVerIdSize;
	u8					  m_abyVersionId[MAX_NUM_LEN_VERSION];				//版本号
	EmSipTerminalType     m_emSipTerType;
	
	//用于网真
	EmSipLevel	m_emSipLevel;                       //级别
	s32	m_nScreenNum;                               //屏数
	u8	m_abyConfId[LEN_GUID];		                //会议号,必须16 octet
	u16	m_wConfId;
	u8	m_byTerNum;			//被邀终端个数
	TSipTransAddress	m_atList[MAX_TERMINAL_NUM];	//被邀终端列表
	TSipConfInfo	m_tConfInfo;			        //会议信息
	
	BOOL m_bNatCall;	//呼叫是否过nat

public:
	tagSipCallParam()
	{
		memset( this, 0, sizeof( tagSipCallParam ) );
		m_emSipConnection = emSipTcpConnection;
	}

	//设置会议号
	void SetConfId(const u8* pchConfId,u8 byLen)
	{
		if( pchConfId == NULL || byLen <= 0 )
		{
			return;
		}
		
		s32 nReal = min(max(LEN_1, byLen), LEN_GUID);
		memcpy(m_abyConfId, pchConfId, nReal);
	}

}TSipCallParam;

typedef struct tagSipNodeInfo
{
    EmSipVendorId m_emVendorId;
	BOOL       m_bMt;
	char       m_achProductId[SESSION_USERNAME_LEN+1];
	char       m_achVersion[SESSION_VERSION_LEN+1];

	public:	    
	tagSipNodeInfo()
	{ 
		Clear();
	}

	void Clear()
	{
		memset(this, 0, sizeof(tagSipNodeInfo));
	}
}TSipNodeInfo;

typedef struct tagSdpSessionInfo
{
    s8      m_achSessionName[SESSION_NAME_LEN+1];
    //origin
    s8      m_achOriginSID[SESSION_ID_LEN+1];         //SessionId
    s8      m_achOriginUserName[SESSION_USERNAME_LEN+1];    //User name
    //s8      m_achOriginVer[SESSION_VERSION_LEN+1];          //session Version
	int     m_nOriginVer;
    s8      m_achOriginAddress[128];    //Address-url

    s8      m_achTerVer[SESSION_INFO_LEN+1];
    
    u32     m_dwBandWidth;
    u32     m_dwStartTime;
    u32     m_dwEndTime;
    u32     m_dwIP;
public:	    
	tagSdpSessionInfo(){ memset(this, 0, sizeof(tagSdpSessionInfo));}
} TSdpSessionInfo;

enum EmSipEndpointType
{
	emSipEndpointTypeUnknown  = 0, 
		emSipEndpointTypeCNS	     = 0x01,
		emSipEndpointTypeUMS		 = 0x02,
		emSipEndpointTypeRegister = 0x04
};

typedef struct tagSipRegistLocalInfo
{
	EmSipEndpointType  m_emEndpointType;   //类型是ums或cns
	u32 m_locolIP;                         //自身IP地址
	s8  m_softversion[LEN_USERNAME+1];     //软件版本号
	s8  m_compileTime[LEN_USERNAME+1];     //编译时间 
public:
    tagSipRegistLocalInfo() {memset(this, 0, sizeof(tagSipRegistLocalInfo));}
}TSipRegistLocalInfo, *PTSipRegistLocalInfo;

// typedef struct tagRegisterInfo
// {
// 	u32			dwHostIP;			
// 	u16			wHostPort;  
// 	u32			m_dwExpires;                    
// 	THostName	m_tRegistrar;		  //注册主机信息
// 									  //超时时间,单位秒
// 	TSipRegistLocalInfo			m_tSipRegLocalInfo;
// };


typedef struct tagRegisterInfo
{
	TSipUrl m_tFromUrl;			//From 字段URL
	TSipUrl m_tToUrl;			//To 字段URL
	THostName m_tRegistrar;		//注册主机信息
	u32 m_dwExpires;			//超时时间,单位秒
    TSipRegistLocalInfo			m_tSipRegLocalInfo;
	EmSipConnection	m_emConnection;	//注册地址类型
public:	
	tagRegisterInfo()
	{ 
		memset(this, 0, sizeof(tagRegisterInfo));
		m_emConnection = emSipUdpConnection;
	}
}TRegisterInfo;


typedef struct tagSipRegName
{
	EmHostNameType m_emSipAddrType;
	s8     m_achUserName[LEN_USERNAME+1];     //注册名
	s8     m_achPassword[LEN_PASSWORD+1];	  //注册密码
	s8     m_achDisplayName[LEN_USERNAME+1];  //显示名称
	
	tagSipRegName()
	{
		memset( this, 0, sizeof(tagSipRegName) );
	}
}TSipRegName,*PTSipRegName;

typedef struct tagRegisterRequest
{
	TSipRegName      m_atSipRegName[MAX_NUM_REGNAME_SIZE];
	s32				 m_nSipRegArraySize;
	
	TIPTransAddress  m_atContactAddr[MAX_NUM_CONTACT_ADDR];//注册地址
	
	u32 m_dwExpires;//超时时间,单位秒
	char m_aCallId[MAX_NUM_CALLID];//呼叫ID
	s32 m_nCSqence;//序列号 //m_tRegistrar m_dwExpires m_aCallId m_nCSqence 需要业务层保存，以便查询
	EmSipConnection emSipConnection;
    TSipRegistLocalInfo  m_tLocalInfo; 

public:
	tagRegisterRequest()
	{ 
		memset(this , 0 , sizeof(tagRegisterRequest));
		emSipConnection = emSipTcpConnection;
	}
}RegisterRequest;

typedef struct tagRegisterResponse
{
	TIPTransAddress  m_tContactAddr;
	EmSipConnection emSipConnection;
	BOOL32 bSerchPrefix;        //判断regserver是否用了前缀查找，如果用前缀查找，回复的300中就将achAlias写入到contact地址中
	s8 achAlias[LEN_USERNAME+1];
public:
	tagRegisterResponse()
	{
		memset(this , 0 , sizeof(tagRegisterResponse));
		emSipConnection = emSipTcpConnection;
	}
}RegisterResponse;

typedef struct tagRegClient
{
	RvSipRegClientHandle   m_hRegClient;
	EmRegState             m_emRegState;
	s32					   m_nAppHandle;
}TRegClient,*PTRegClient;
/*   
 *   h.281 encoded in host bit order:
 *   +-------+---------+--------+---------+--------+---------+--------+---------+
 *   +   P   | R=1/L=0 |    T   | U=1/D=0 |    Z   | I=1/O=0 |    F   | I=1/O=0 |
 *   +-------+---------+--------+---------+--------+---------+--------+---------+
 */
enum EmFeccAction
{
	emActionInvalid				    = 0,  
	emActionStart					= 0x01,  //开始
	emActionContinue				= 0x02,  //继续
	emActionStop					= 0x03,  //停止
	emActionSelectVideoSource		= 0x04,  //选择视频源
	emActionVideoSourceSwitched	    = 0x05,  //切换视频源
	emActionStoreAsPreset			= 0x06,  //存储预置位
	emActionActivatePreset	    	= 0x07,  //恢复到预置位
	emActionAutoFocus				= 0x11,  //自动调焦
};


//PTFZ codes, parameters for action Start, continue and stop.
enum EmFeccPTFZRequest
{
	emRequestInvalid				= 0,
	emRequestPanLeft				= 0x80,	//向左移动
	emRequestPanRight				= 0xc0,	//向右移动
	emRequestTiltUp			    	= 0x30,	//向上移动
	emRequestTiltDown				= 0x20,	//向下移动
	emRequestFocusIn				= 0x03,	//近焦
	emRequestFocusOut				= 0x02,	//远焦
	
	emRequestZoomIn				    = 0x0c,	//视野变小
	emRequestZoomOut				= 0x08,	//视野变大

	emRequestBrightnessUp			= 0x01,	//亮度加(nonstandard)
	emRequestBrightnessDown		    = 0x04  //亮度减(nonstandard)
};



//for tanderg
enum EmFeccVideoSourceNumber
{
    emVideoSourceDefault               = 0,    //当前视频源
    emVideoSourceMainCamera            = 1,    //主视频源
    emVideoSourceAuxCamera             = 2,    //辅助视频源
    emVideoSourceDocCamera             = 3,    //文件展示台
    emVideoSourceAuxDocCamera          = 4,    //辅助文件展示台，对于泰德为vcr
    emVideoSourceVideoPlaybackSource   = 5,    //vcr，对于泰德为PC
};

enum EmSipLogModule
{
	emSipMdlBegin,
	emSipMdlNone = emSipMdlBegin,
	emSipMdlAll,
	emSipMdlAllExceptFrequent,
	emSipMdlCall,
	emSipMdlCap,
	emSipMdlChannel,
	emSipMdlConf,
	emSipMdlRegister,
	emSipMdlFrequent,
	emSipMdlEnd = 30,
};
enum EmSipLogType
{
	emSipLogBegin,
		emSipLogNormal,
		emSipLogWarning,
		emSipLogError,
};

//终端编号参数
typedef struct tagTerLabel 
{
	u8 m_byMcuNo;
	u8 m_byTerNo;

	tagTerLabel()
	{
		Clear();		
	}

	void Clear()
	{
		m_byMcuNo = 0xff;
		m_byTerNo = 0xff;
	}

    //有效性检查
	bool IsValid()
	{
		return ( m_byMcuNo < LEN_MAXMT_NUM && m_byTerNo < LEN_MAXMT_NUM);
	}
    
    //设置TerminalLabel
	BOOL SetTerLabel( u8 byMcuNo, u8 byTerNo )
	{
		if( byMcuNo < LEN_MAXMT_NUM && byTerNo < LEN_MAXMT_NUM )
		{
			m_byMcuNo = byMcuNo;
			m_byTerNo = byTerNo;
            return TRUE;
		}
        return FALSE;
	}
    //获取TerminalLabel
	void GetTerLabel( u8 *pMcuNo, u8 *pTerNo )
	{
		*pMcuNo = m_byMcuNo;
		*pTerNo = m_byTerNo;
	}
    //获取mcu编号
	u8 GetMcuNo()
	{
		return m_byMcuNo;
	}
    //获取终端编号
	u8 GetTerNo()
	{
		return  m_byTerNo;
	}
}TTerLabel,*PTTerLabel;

typedef struct
{
	u8 m_byAction;			  //start, continue, stop, video source, preset
	
	union 
	{
		u8 m_byRequest;		  //for PTFZ operations
		u8 m_bySourceNo;      //for VIDEO SOURCE operations
		u8 m_byPresetNo;      //for PRESET operations
	}arguments;
	
	u8 m_byTimeout;			  //for start action only ,unit 50ms, 0表示800ms	
}TFeccInfo, *PTFeccInfo;

typedef struct tagSipFlowCtrl
{
	s32   m_nChannelId;
	u16   m_wBitRate;
	tagSipFlowCtrl()
	{
		m_nChannelId = -1;
		m_wBitRate = 0;
	}
}TSipFlowCtrl,*PTSipFlowCtrl;

enum EmSipAliasType
{
	sip_Alias_Unknow,
		sip_Alias_e164,
		sip_Alias_h323,
		sip_Alias_IP,
};

typedef struct tagTSipAlias
{
	EmSipAliasType	m_byType;
	s8				m_abyAlias[LEN_USERNAME + 1];
    tagTSipAlias()
	{
		memset( this, 0, sizeof(tagTSipAlias) );
	}
}TSipAlias;

enum EmSipGetRegInfoType
{
	emSipGetRegInfoType_CNS = 0,
		emSipGetRegInfoType_UMS,
		emSipGetRegInfoType_ALL
};

typedef struct tagSipGetRegInfo
{
	EmSipGetRegInfoType emType;
	u32    dwRegIP;         //注册服务器IP地址
	u16    wRegPort;        //注册服务器端口
	u32    dwLocalIP;       //本地ip地址  
	u16    wLocalPort;     //本地端口
	tagSipGetRegInfo()
	{
		memset( this, 0, sizeof(tagSipGetRegInfo) );
	}
}TSipGetRegInfo;



typedef struct tagTSipRegPackInfo
{
	u16 m_wPackSeq;       //包序号，从1开始，0为无效值
	u16 m_wTotalPackNum;  //包总数
	tagTSipRegPackInfo()
	{
		m_wPackSeq = 0;
		m_wTotalPackNum = 0;
	}
	
}TSipRegPackInfo, *PTSipRegPackInfo;

typedef struct tagTSipRegInfo
{
	u16		   m_wAliasNum;
	TSipAlias  m_atAlias[MAX_NUM_REGNAME_SIZE];       //暂时定为最大值为64个名称为一波返回
	EmSipEndpointType m_emSelfType;                   //注册方 的系统类型(UMS和CNS)    

	tagTSipRegInfo()
	{
		m_wAliasNum = 0;
		memset( m_atAlias, 0, sizeof( m_atAlias ) );
		m_emSelfType = emSipEndpointTypeUnknown;
	}
	
}TSipRegInfo;

typedef struct tagTSipRegPackXml
{
	TSipRegPackInfo tPackInfo;
	EmSipGetRegInfoType emType;
	TSipRegInfo  tRegInfo;

	tagTSipRegPackXml()
	{
		emType = emSipGetRegInfoType_ALL;
	}
}TSipRegPackXml;

typedef struct tagTSipRegInfoUms
{
	u16		   m_wAliasNum;
	TSipAlias  m_atAlias[MAX_NUM_REGNAME_SIZE];       //暂时定为最大值为64个名称为一波返回
	TIPTransAddress  m_atContactAddr[MAX_NUM_CONTACT_ADDR];//注册地址
	u32 m_dwExpires;//超时时间,单位秒
	EmSipConnection emSipConnection;
	TSipRegistLocalInfo m_tSipRegistLocalInfo;
	
	tagTSipRegInfoUms()
	{
		Clear();
	}

	void Clear()
	{
		memset(this, 0, sizeof(tagTSipRegInfoUms));
	}
	
}TSipRegInfoUms;

typedef struct tagTSipRegInfoCns
{
	u16		   m_wAliasNum;
	TSipAlias  m_atAlias[MAX_NUM_REGNAME_SIZE_CNS];       //暂时定为最大值为64个名称为一波返回
	TIPTransAddress  m_atContactAddr[MAX_NUM_CONTACT_ADDR];//注册地址
	u32 m_dwExpires;//超时时间,单位秒
	EmSipConnection emSipConnection;
	TSipRegistLocalInfo m_tSipRegistLocalInfo;
	
	tagTSipRegInfoCns()
	{
		Clear();
	}

	void Clear()
	{
		memset(this, 0, sizeof(tagTSipRegInfoCns));
	}
	
}TSipRegInfoCns;

typedef struct tagTSipRegPackXmlB2
{
	TSipRegPackInfo tPackInfo;
	EmSipGetRegInfoType emType;
	EmSipEndpointType  emEndpointType;
	u16	wTotalRegInfo;
	tagTSipRegPackXmlB2()
	{
		emType = emSipGetRegInfoType_ALL;
		wTotalRegInfo = 0;
	}
}TSipRegPackXmlB2;


// typedef struct tagUMSRegistration
// {
// // 	BOOL          m_bRegistration;//TRUE 注册 FALSE取消注册
// // 	BOOL		  m_bExpireAll;//取消注册时，别名所匹配的所有条目全都过期
// 	RegisterRequest m_tRegReq;//注册信息
// 	CSipCapSet	  m_tTPCNSCapSet;//注册能力集
// 	tagUMSRegistration()
// 	{
// 		Clear();
// 	}
// 	void Clear()
// 	{
// 		memset(this , 0 , sizeof(this));
// 	}
// }TUMSRegistration,*PTUMSRegistration;

RV_DECLARE_HANDLE(RvSipAppChanHandle);
RV_DECLARE_HANDLE(RvSipChanHandle);
RV_DECLARE_HANDLE(RvSipAppRASHandle);
RV_DECLARE_HANDLE(RvSipRASHandle);


/*====================================================================
函数名      : NotifyNewCallT
功能        : 交换呼叫句柄
算法实现    : 无
引用全局变量: 无
输入参数说明: hsCall   - 协议栈呼叫句柄
    		  lphaCall - 应用程序呼叫句柄
返回值说明  : 成功 - cb_ok
              失败 - cb_err
====================================================================*/
typedef s32 (RVCALLCONV* NotifyNewCallT)( RvSipCallLegHandle hsCall, LPRvSipAppCallLegHandle lphaCall, const void* pBuf,u16 nBufLen);


/*====================================================================
函数名      : NotifyCallCtrlMsgT
功能        : 呼叫控制通知函数
算法实现    : 无
引用全局变量: 无
输入参数说明: haCall  - 应用程序呼叫句柄
			  hsCall  - 协议栈呼叫句柄
			  msgType - 消息类型
			  pBuf    - 参数缓冲区
			  nBufLen - 参数长度
返回值说明  : 成功 - cb_ok
              失败 - cb_err
====================================================================*/
typedef s32 (RVCALLCONV* NotifyCallCtrlMsgT)(RvSipAppCallLegHandle haCall,RvSipCallLegHandle hsCall,u16 msgType,
											  const void* pBuf,u16 nBufLen);

/*====================================================================
函数名      : NotifyChanCtrlMsgT
功能        : 信道控制通知函数
算法实现    : 无
引用全局变量: 无
输入参数说明: haCall  - 应用程序呼叫句柄
              hsCall  - 协议栈呼叫句柄
              haChan  - 应用程序信道句柄
			  hsChan  - 协议栈信道句柄
			  msgType - 消息类型
			  pBuf    - 参数缓冲区
			  nBufLen - 参数长度
返回值说明  : 成功 - cb_ok
              失败 - cb_err
====================================================================*/
typedef s32 (RVCALLCONV* NotifyChanCtrlMsgT)(RvSipAppCallLegHandle haCall,RvSipCallLegHandle hsCall,
											 RvSipAppChanHandle haChan,RvSipChanHandle hsChan,
											 u16 msgType,const void* pBuf,u16 nBufLen);

/*====================================================================
函数名      : NotifyConfCtrlMsgT
功能        : 会议控制通知函数
算法实现    : 无
引用全局变量: 无
输入参数说明: haCall  - 应用程序呼叫句柄
              hsCall  - 协议栈呼叫句柄
			  msgType - 消息类型
			  pBuf    - 参数缓冲区
			  nBufLen - 参数长度
返回值说明  : 成功 - cb_ok
              失败 - cb_err
====================================================================*/
typedef s32 (RVCALLCONV* NotifyConfCtrlMsgT)(RvSipAppCallLegHandle haCall,RvSipCallLegHandle hsCall,
											 RvSipTranscHandle hTransc,u16 msgType,const void* pBuf,u16 nBufLen);
/*====================================================================
函数名      : NotifyRasCtrlMsgT
功能        : RAS信息通知函数
算法实现    : 无
引用全局变量: 无
输入参数说明: haCall  - 应用程序呼叫句柄
              hsCall  - 协议栈呼叫句柄
              haRas   - 应用程序RAS句柄
			  hsRas   - 协议栈RAS句柄
			  msgType - 消息类型
			  pBuf    - 参数缓冲区
			  nBufLen - 参数长度
返回值说明  : 成功 - cb_ok
              失败 - cb_err
====================================================================*/
typedef s32 (RVCALLCONV* NotifyRegCtrlMsgT)(IN  RvSipAppRegClientHandle         hAppRegClient,
											IN  RvSipRegClientHandle            hRegClient,
                                            u16 msgType, const void* pBuf,u16 nBufLen);

/*====================================================================
函数名      : NotifyFeccMsgT
功能        : H.224, H.281功能回调函数
算法实现    : 无
引用全局变量: 无
输入参数说明: hsCall     - 协议栈呼叫句柄
              hsChan     - 协议栈信道句柄
			  feccStruct - 远遥命令(TFeccStruct)
			  terSrc     - 源端终端列表
			  terDst     - 源端终端列表
返回值说明  : 成功 - cb_ok
              失败 - cb_err
====================================================================*/
typedef s32 (RVCALLCONV *NotifyFeccMsgT)( RvSipCallLegHandle hsCall, TFeccInfo feccStruct, TTerLabel terSrc, TTerLabel terDst);

/*====================================================================
函数名      : NotifyUMSCtrlMsgT
功能        : UMS信息通知函数
算法实现    : 无
引用全局变量: 无
输入参数说明: RvSipTranscHandle  - 应用程序呼叫句柄
              hsCall  - 协议栈呼叫句柄
              haRas   - 应用程序RAS句柄
			  hsRas   - 协议栈RAS句柄
			  msgType - 消息类型
			  pBuf    - 参数缓冲区
			  nBufLen - 参数长度
返回值说明  : 成功 - cb_ok
              失败 - cb_err
====================================================================*/
typedef s32 (RVCALLCONV* NotifyUMSCtrlMsgT)(IN RvSipTranscHandle hTransc, u16 msgType, const void* pBuf, u16 nBufLen);
//回调函数指针结构
typedef struct 
{
	NotifyRegCtrlMsgT	fpRegCtrlMsg;
    NotifyNewCallT		fpNewCall;
	NotifyCallCtrlMsgT  fpCallCtrlMsg;
    NotifyChanCtrlMsgT  fpChanCtrlMsg;
	NotifyFeccMsgT      fpFeccCtrlMsg;
	NotifyConfCtrlMsgT  fpConfCtrlMsg;
	NotifyUMSCtrlMsgT   fpUMSCtrlMsg;
}SIPEVENT,*LPSIPEVENT;

class CMediaStreamCtrl;

class CSipInterface
{
public:
   /*====================================================================
   函	 数   名: SipAdapterInit
   功	      能: 初始化Sip适配模块
   算 法 实  现: 
   引用全局变量: 
   输入参数说明: tSipAdapterCfg 

   返   回   值: 成功 - &g_hApp
   失败 - NULL		
	====================================================================*/
    static RvStatus AdapterInit( TSipAdapterCfg tSipAdapterCfg );
	
	/*====================================================================
	函	 数   名: SipAdapterQuit
	功	      能: 退出SIP适配模块
	算 法 实  现: 
	引用全局变量: 无
	输入参数说明: 无
	返   回   值: 无
	====================================================================*/
    static void AdapterQuit();

	/*====================================================================
	函	 数   名: SetSipAppCallBack
	功	      能: 注册应用回调函数
	算 法 实  现: 
	引用全局变量: 无
	输入参数说明: lpEvent - 回调函数结构指针
	返   回   值: 成功 - act_ok
	失败 - act_err
	====================================================================*/
	static void  SetAppCallBack(IN LPSIPEVENT lpEvent);

	/*====================================================================
	函	 数   名: SipCheckMessage
	功	      能: 检测消息
	算 法 实  现: 
	引用全局变量: 无
	输入参数说明: u32 - 阻塞时间(毫秒)
	返   回   值: 成功 - act_ok
	失败 - act_err
	====================================================================*/
	static void CheckMessage( u32 dwMiliSecond );



	/*====================================================================
	函	 数   名: CreateNewCall
	功	      能: 创建新的呼叫，主叫在发送呼叫消息前调用,必须先创建再呼叫
	算 法 实  现: 
	引用全局变量: 无
	输入参数说明: lpEvent - 回调函数结构指针
	返   回   值: 成功 - act_ok
	失败 - act_err
	====================================================================*/
	static RvStatus CreateNewCall(IN RvSipAppCallLegHandle haCall, OUT LPRvSipCallLegHandle lphsCall );

	/*====================================================================
	函	 数   名: CreateNewChan
	功	      能: 创建新的通道，打开逻辑前先创建
	算 法 实  现: 
	引用全局变量: 无
	输入参数说明: lpEvent - 回调函数结构指针
	返   回   值: 成功 - act_ok
	失败 - act_err
	====================================================================*/
	static RvStatus CreateNewChan( IN RvSipCallLegHandle hsCall, IN RvSipAppChanHandle haChan, 
	 	                      IN EmSipMediaType emMediaType, IN BOOL bSend, OUT LPRvSipChanHandle lphsChan );
	
	
	/*====================================================================
	函	 数   名: ReleaseChan
	功	      能: 释放一个通道
	算 法 实  现: 
	引用全局变量: 无
	输入参数说明: lpEvent - 回调函数结构指针
	返   回   值: 成功 - act_ok
	失败 - act_err
	====================================================================*/
	static RvStatus ReleaseChan(IN RvSipCallLegHandle hsCall, IN RvSipChanHandle hsChan );
	
	/*====================================================================
	函	 数   名: SendCallCtrlMsg
	功	      能: 发送呼叫控制消息
	算 法 实  现: 
	引用全局变量: 无
	输入参数说明: hsCall  - 呼叫句柄
	msgType - 消息类型
	pBuf    - 参数缓冲区
	nBufLen - 参数长度
	返   回   值: 成功 - act_ok
	失败 - act_err
	====================================================================*/
	static RvStatus SendCallCtrlMsg( RvSipCallLegHandle hsCall, u16 msgType, const u8* pBuf, u16 nBufLen );
	

	/*====================================================================
	函	 数   名: SendChanCtrlMsg
	功	      能: 发送信道控制消息
	算 法 实  现: 
	引用全局变量: 无
	输入参数说明: hsChan  - 信道句柄
	msgType - 消息类型
	pBuf    - 参数缓冲区
	nBufLen - 参数长度
	返   回   值: 成功 - act_ok
	失败 - act_err
	====================================================================*/
	static RvStatus SendChanCtrlMsg( RvSipCallLegHandle hsCall, IN RvSipChanHandle hsChan, 
		                        IN EmSipChanCtrlMsgType emMsgType, IN const u8* pBuf,IN u16 nBufLen );

	/*====================================================================
	函	 数   名: SetSdpSession
	功	      能: sdp session info
	算 法 实  现: 
	引用全局变量: 无
	输入参数说明: hsCall  - 呼叫句柄
	              msgType - 消息类型
	              pBuf    - 参数缓冲区
	              nBufLen - 参数长度
	返   回   值: 成功 - act_ok
	失败 - act_err
	====================================================================*/
	static void SetSdpSession( RvSipCallLegHandle hsCall, char* pchSessionName, 
					           u32 dwStartTime, u32 dwStopTime );

	/*====================================================================
	函	 数   名: SendRegCtrlMsg
	功	      能: 发送信道控制消息
	算 法 实  现: 
	引用全局变量: 无
	输入参数说明: hsChan  - 信道句柄
	msgType - 消息类型
	pBuf    - 参数缓冲区
	nBufLen - 参数长度
	返   回   值: 成功 - act_ok
	失败 - act_err
	====================================================================*/
	static RvStatus SendRegCtrlMsg( IN EmSipRegCtrlMsgType emMsgType, IN const u8* pBuf,IN u16 nBufLen );
	


	/*====================================================================
	函	 数   名: OpenFeccChannel
	功	      能: 打开h224通道
	算 法 实  现: 
	引用全局变量: 
	输入参数说明: hsCall - 对话句柄
                  bSend  - 发送/接收
	返   回   值: 成功 - 
	失败 - 小于0
	====================================================================*/
	static RvStatus OpenFeccChannel(IN RvSipCallLegHandle hsCall, const void* pBuf, u16 nBufLen);

	/*====================================================================
	函	 数   名: CloseFeccChannel
	功	      能: 关闭h224通道
	算 法 实  现: 
	引用全局变量: 
	输入参数说明: hsCall - 对话句柄
                  bSend  - 发送/接收
	返   回   值: 成功 - 
	失败 - 小于0
	====================================================================*/
	static RvStatus CloseFeccChannel(IN RvSipCallLegHandle hsCall, BOOL bSend  );

	/*====================================================================
	函	 数   名: FeccBind
	功	      能: 多点会议中绑定远遥对象，点对点呼叫时无需绑定
	算 法 实  现: 
	引用全局变量: 无
	输入参数说明: hsChan	 - h224通道句柄
	localTer   - 本地终端列表(TTERLABEL)
	remoteTer  - 远端终端列表(TTERLABEL)
	返   回   值: 成功 - 
	失败 - 小于0
	====================================================================*/
	static RvStatus FeccBind( IN RvSipCallLegHandle hsCall, IN TTerLabel tSrcLabel, IN TTerLabel tDstLabel );
		
	/*====================================================================
	函	 数   名: SendFeccMsg
	功	      能: 发送远遥消息
	算 法 实  现: 
	引用全局变量: 无
	输入参数说明: hsChan  - 信道句柄
	msgType - 消息类型
	pBuf    - 参数缓冲区
	nBufLen - 参数长度
	返   回   值: 成功 - act_ok
	失败 - act_err
	====================================================================*/
	static RvStatus SendFeccMsg( IN RvSipCallLegHandle hsCall, IN TFeccInfo feccStruct );

	/*====================================================================
	函	 数   名: NotifyFeccMsg
	功	      能: 远遥消息会调
	算 法 实  现: 
	引用全局变量: 无
	输入参数说明: hsCall  - 信道句柄
	              feccStruct - 消息类型
	              terSrc    - 参数缓冲区
	              terDst - 参数长度
	返   回   值: 成功 - act_ok
	失败 - act_err
	====================================================================*/
	static RvStatus RVCALLCONV NotifyFeccMsg( RvSipCallLegHandle hsCall, TFeccInfo feccStruct, TTerLabel terSrc, TTerLabel terDst); 



	static u16 GetPayload( EmSipPayloadType emPt );

	static RvStatus SendConfCtrlMsg( IN RvSipCallLegHandle hsCall, IN RvSipTranscHandle hTransc, IN EmSipConfCtrlMsgType emMsgType, IN const u8* pBuf,IN u16 nBufLen );

	static RvStatus SendUMSCtrlMsg( IN RvSipTranscHandle hTransc, IN EmSipUMSCtrlMsgType emMsgType, IN const u8* pBuf,IN u16 nBufLen );

	static RvStatus SetAppHanleToUMS( void *pAppHandle );

	static RvStatus SetAppHanleToCall(IN  RvSipCallLegHandle     hCallLeg,
                                      IN  RvSipAppCallLegHandle  hAppCallLeg);

	static void SetInterworking(BOOL32 bInter); //sipadapter是否用在interworking中，否则不发送option查询注册信令

	static s32 OnCreateSipRegister(IN  RvSipAppRegClientHandle hAppRegClient, OUT LPRvSipRegClientHandle lphRegClient);

	static char* GetMediaName( EmSipPayloadType emPayLoadType );
	static char* GetResolutionName( EmSipResolution emResType );
	static EmSipPayloadType GetPayloadByName( char *pchName );
	static EmSipResolution GetResolutionByName( char *pchName );
private:

		/*set stack callback*/
    static void SipAppSetCallBack();
	 //callback  function
	static SIPEVENT    m_tEventProcess;

   /*Handle to the stack manager. This parameter is returned when calling
    RvSipStackConstruct. You should supply this handle when using the stack
	manager API functions.*/
	static RvSipStackHandle      m_hStackMgr;
	
	/*Handle to the call-leg manager. You can get this handle by calling
	RvSipStackGetCallLegMgrHandle. You should supply this handle when
	using the call-leg manager API functions.*/
	static RvSipCallLegMgrHandle m_hCallLegMgr;
	
	/*Handle to the log-module. You can get this handle by calling
	RvSipStackGetLogHandle. You need this value in order to construct the application
	memory pool.*/
    static RV_LOG_Handle         m_hLog;;
	
	/*Handle to the application memory pool. The application should construct its own
	memory using rpool API. The pool is needed for encoding messages or message
	parts. (See AppPrintMessage() )*/
	static HRPOOL                m_appPool;

	/*Handle to the register-client manager. This value is returned when calling
   RvSipStackGetRegClientMgrHandle after constructing the stack. You should
   supply this handle when using the register- client manager API functions.*/
//     static RvSipRegClientHandle   m_hRegClient;
// 	static EmRegState             m_emRegState;
	static TRegClient			  m_atRegClient[MAX_NUM_REGCLIENT];
    

	/*Handle to the Authenticator manager. This value is returned when calling
  RvSipStackGetAuthenticationHandle. You should supply this handle when using the
  Authenticator manager API functions.*/
   static RvSipAuthenticatorHandle m_hAuthenticatorMgr;
   static s8 	 m_achAuthenUserName[LEN_USERNAME+1];      //验证用户名
   static s8     m_achAuthenPassword[LEN_PASSWORD+1];      //验证密码

    //call operation
    static s32 OnMakeCall( RvSipCallLegHandle hsCall, const void* pBuf,u16 nBufLen );
    static s32 OnAcceptCall( RvSipCallLegHandle hsCall, const void* pBuf,u16 nBufLen );
	static s32 OnCallDisConnect( RvSipCallLegHandle hsCall, const void* pBuf,u16 nBufLen );

// 	//call ctrl 
//     static int OnSendConfCtrlMsg( IN RvSipCallLegHandle hsCall, SipChanCtrlMsgType emMsgType );

	//dual operation
	static int OnOpenDual( IN RvSipCallLegHandle hsCall, const void* pBuf, u16 wBufLen );
	static int OnAcceptOpenDual( IN RvSipCallLegHandle hsCall, const void* pBuf, u16 wBufLen );
    static int OnRejectOpenDual( IN RvSipCallLegHandle hsCall, const void* pBuf, u16 wBufLen );
    static int OnCloseDual( IN RvSipCallLegHandle hsCall, const void* pBuf, u16 wBufLen );
    static int OnAcceptCloseDual( IN RvSipCallLegHandle hsCall, const void* pBuf, u16 wBufLen );	
	
	/*incoming call callback*/
    static void RVCALLCONV AppCallLegCreatedEvHandler(IN  RvSipCallLegHandle     hCallLeg,
		                                              OUT RvSipAppCallLegHandle  *phAppCallLeg);
	
	/*call status change callback*/
	static void RVCALLCONV AppCallLegStateChangedEvHandler( IN  RvSipCallLegHandle hCallLeg,
		                                                    IN  RvSipAppCallLegHandle  hAppCallLeg,
		                                                    IN  RvSipCallLegState      eState,
		                                                    IN  RvSipCallLegStateChangeReason eReason);
	
	/*dual status change callback*/
	static void RVCALLCONV AppCallLegModifyStateChangedEvHandler( IN  RvSipCallLegHandle hCallLeg,
		                                                          IN  RvSipAppCallLegHandle   hAppCallLeg,
		                                                          IN  RvSipCallLegModifyState eState,
		                                                          IN  RvSipCallLegStateChangeReason eReason);
	
	static RvStatus RVCALLCONV AppCallLegMsgToSendEvHandler( IN  RvSipCallLegHandle          hCallLeg,
		                                          IN  RvSipAppCallLegHandle       hAppCallLeg,
 		                                          IN  RvSipMsgHandle              hMsg);
	
	static RvStatus RVCALLCONV AppCallLegMsgReceivedEvHandler( IN  RvSipCallLegHandle       hCallLeg,
		                                                       IN  RvSipAppCallLegHandle    hAppCallLeg,
		                                                       IN  RvSipMsgHandle           hMsg);

	/*call ctrl incoming callback*/
	static void  RVCALLCONV AppCallLegTranscCreatedEvHandler( IN  RvSipCallLegHandle            hCallLeg,
                                                              IN  RvSipAppCallLegHandle         hAppCallLeg,
                                                              IN  RvSipTranscHandle             hTransc,
                                                              OUT RvSipAppTranscHandle          *hAppTransc,
                                                              OUT RvBool                       *bAppHandleTransc);

	static void RVCALLCONV AppCallLegTranscStateChangedEvHandler(	IN  RvSipCallLegHandle    hCallLeg,
		                                                IN  RvSipAppCallLegHandle hAppCallLeg,
		                                                IN  RvSipTranscHandle     hTransc,
		                                                IN  RvSipAppTranscHandle  hAppTransc,
		                                                IN  RvSipCallLegTranscState  eTranscState,
		                                                IN  RvSipTransactionStateChangeReason eReason);
/*register status change callback */
    static void RVCALLCONV AppRegClientStateChangedEvHandler(
                           IN  RvSipRegClientHandle            hRegClient,
                           IN  RvSipAppRegClientHandle         hAppRegClient,
                           IN  RvSipRegClientState             eState,
                           IN  RvSipRegClientStateChangeReason eReason);

    static RvStatus RVCALLCONV AppRegClientMsgReceivedEvHandler(
                           IN  RvSipRegClientHandle          hRegClient,
                           IN  RvSipAppRegClientHandle       hAppRegClient,
                           IN  RvSipMsgHandle                hMsg);

	static void RVCALLCONV AppAuthenticationMD5Ev(IN  RvSipAuthenticatorHandle  hAuthenticator,
                                              IN  RvSipAppAuthenticatorHandle    hAppAuthenticator,
                                              IN  RPOOL_Ptr                     *pRpoolMD5Input,
                                              IN  RvUint32                     length,
                                              OUT RPOOL_Ptr                     *pRpoolMD5Output);


	static void RVCALLCONV AppAuthenticationGetSharedSecretEv(
                                   IN  RvSipAuthenticatorHandle       hAuthenticator,
                                   IN  RvSipAppAuthenticatorHandle    hAppAuthenticator,
                                   IN  void*                          hObject,
                                   IN  void*                          peObjectType,
                                   IN  RPOOL_Ptr                     *pRpoolRealm,
                                   OUT RPOOL_Ptr                     *pRpoolUserName,
                                   OUT RPOOL_Ptr                     *pRpoolPassword);

	static void RVCALLCONV StackLog(IN void* context , IN RvSipLogFilters filter , IN const RvChar   *formattedText);

	static void OnGetRegInfo(s32 nAppId, TSipGetRegInfo tSipGetRegInfo, void *pHandle);  
	
	static void OnRegister( TSipUrl tFromUrl, TSipUrl tToUrl, THostName tRegistrar, u32 dwExpires, TSipRegistLocalInfo tSipRegLocalInfo, s32 nSameReg, void *pHandle, TSipRegName *patSipRegName, s32 nArraySize, EmSipConnection emConnection = emSipUdpConnection );

	static void OnUnregister( const s32 nAppId, void *pHandle );

//     static RvStatus RVCALLCONV AppRegClientMsgToSendEvHandler(
//                            IN  RvSipRegClientHandle          hRegClient,
//                            IN  RvSipAppRegClientHandle       hAppRegClient,
//                            IN  RvSipMsgHandle                hMsg);

	static const RvChar* AppGetRegClientStateName(IN  RvSipRegClientState  eState);
    //other
	static void ConstructURI( TSipUrl tUrlAddr, char *achOut, EmSipConnection emSipConnection = emSipTcpConnection, BOOL32 bAddPasswd = FALSE , BOOL bTo = FALSE );

    static void AppPrintMessage(IN RvSipMsgHandle hMsg);
	static void RVCALLCONV   AppSipTransactionStateChangedEv(	IN RvSipTranscHandle                 hTransc,
																IN RvSipTranscOwnerHandle            hTranscOwner,
																IN RvSipTransactionState             eState,
																IN RvSipTransactionStateChangeReason eReason);
	static void RVCALLCONV  AppSipTransactionCreatedEv(	IN    RvSipTranscHandle        hTransc,
														IN    void                     *pAppContext,
														OUT   RvSipTranscOwnerHandle   *hAppTransc,
														OUT   RvBool                  *b_handleTransc);
	static s32 OnSendAck( RvSipCallLegHandle hsCall, const void* pBuf,u16 nBufLen );
	static RvStatus RVCALLCONV AppRegserverMsgReceivedEvHandler( 
												IN RvSipTranscHandle      hTransc,
												IN RvSipTranscOwnerHandle hTranscOwner,
												IN RvSipMsgHandle         hMsgReceived );
	static RvStatus RVCALLCONV AppRegserverMsgToSendEvHandler(  
												IN RvSipTranscHandle       hTransc,
												IN RvSipTranscOwnerHandle  hTranscOwner,
												IN RvSipMsgHandle          hMsgToSend );
private:
	//begin sdp part
	//厂商号
	static s8 m_achVendorId[SESSION_USERNAME_LEN+1]; 
	//产品型号
	static s8  m_achProductId[SESSION_USERNAME_LEN+1]; 
	//版本号
	static s8  m_achVersion[SESSION_INFO_LEN+1];
	//设置厂商号和版本信息
    static void SetSdpInfo( s8* pchProductId, s8* pchVersionId );

	static  RvStatus AddDisConnectReason(IN RvSipCallLegHandle hsCall,IN RvSipMsgHandle hMsg, u8 byReason);

	static RvStatus AddScreenNum( IN RvSipCallLegHandle hsCall,IN RvSipMsgHandle hMsg, TSipCallParam &tSipCallParam );

	static RvStatus AddLevelHeader( IN RvSipCallLegHandle hsCall, IN RvSipMsgHandle hMsg, TSipCallParam &tSipCallParam );
	//设置User-Agent
	static RvStatus AddUserAgent( IN RvSipMsgHandle hMsg );

	static RvStatus AddUserAgent( IN RvSipCallLegHandle hsCall, IN RvSipMsgHandle hMsg, TSipCallParam &tSipCallParam );
	//设置allow header
	static RvStatus AddAllowHeader( IN RvSipMsgHandle hMsg );
	//设置supported header
	static RvStatus AddSupportedHeader( IN RvSipMsgHandle hMsg );
	//设置require header
	static RvStatus AddRequireHeader( IN RvSipMsgHandle hMsg );
	//add sdp msg to invite transaction
    static  RvStatus AddSdpBodyToMsg(IN RvSipCallLegHandle hCallLeg, CSipCapSet *pcCapSet , BOOL bSave = FALSE, TSipCallParam *ptSipParam = NULL );
// 	//add sdp msg to re-invite transaction
// 	static  RvStatus AddSdpBodyToReInvite(IN RvSipCallLegHandle hCallLeg, BOOL bOffer, CSipCapSet *pcCapSet );
	//uas retrieve call parameter from incoming invite transaction
	
	static  RvStatus RetrieveDisReason(IN RvSipCallLegHandle hCallLeg, OUT u8& byReason);
	
	static  RvStatus RetrieveParamFromCall( IN RvSipCallLegHandle hCallLeg, OUT TSipCallParam& tCallParam, BOOL bCaller = TRUE );
	//uas retrieve sdp msg from incoming invite transaction
    static  RvStatus RetrieveSDPBodyFromMsg( IN RvSipCallLegHandle hCallLeg, OUT CSipCapSet& tCapSet, INOUT TSipCallParam *ptSipParam = NULL );
	//uac retrieve sdp msg from outcoming invite 200 ok transaction
//	static  RvStatus RetrieveSDPBodyFromInvite2xxOk( IN RvSipCallLegHandle hCallLeg, OUT CSipCapSet& tCapSet );
	//uas retrieve sdp msg from incoming re-invite  transaction
//	static  RvStatus RetrieveSDPBodyFromReInvite( IN RvSipCallLegHandle hCallLeg, OUT CSipCapSet& tCapSet );
	//uac retrieve sdp msg from outcoming re-invite 200ok  transaction
//	static  RvStatus RetrieveSDPBodyFromReInvite2xxOk( IN RvSipCallLegHandle hCallLeg, OUT CSipCapSet& tCapSet );
	static BOOL BuildSdpBfcpFromCapset( IN RvSdpMsg* pSdpMsg, IN CBfcpMediaStream *pcMediaStream );
	
	static BOOL BuildSdpFeccFromCapset( IN RvSdpMsg* pSdpMsg, IN RvSdpConnectionMode rvMode, IN CFeccMediaStream *pcMediaStream );

    //sdp offer-answer mode build offer sdp msg
	static BOOL BuildSdpFromCapset( IN RvSdpMsg* pSdpMsg, BOOL bOffer, IN RvSdpConnectionMode rvMode, 
		                       IN CSipMediaStream *ptMediaStream, u16 wRecvRtcpPort = 0 );

	static BOOL BuildCapsetFromSdpBfcp( CBfcpMediaStream *pcStream, RvSdpMediaDescr *pMediaDesc );

	static BOOL BuildCapsetFromSdpFecc( CFeccMediaStream *pcStream, RvSdpMediaDescr *pMediaDesc );
    //build audio video capset according sdp msg
	static BOOL BuildCapsetFromSdp( CSipMediaStream *pcStream, RvSdpMediaDescr *pMediaDesc , u32 dwConnectionAddr );

	//build H264 capset
	static void BuildH264Capset( RvSdpRtpMap* pSdpRtpMap, TSipH264VideoCap &tH264Cap, 
		                         char *achAttr =NULL );
	//build video capset except h264 
	static void BuildCommonVideoCapset( RvSdpRtpMap* pSdpRtpMap, TSipVideoCap &tHCommonCap,
		                                char *achAttr =NULL );

	
	static u16 GetDynamicPayloadType( EmSipPayloadType emPt );
	static BOOL GetValueByFieldIds( char *pchString, char *pchField, int &nValue );
	static BOOL GetValueByFieldIds( char *pchString, char *pchField, char *pValue );
	static EmSipPayloadType GetFormat( u16 wPayload );
	static char*  GetFmtp( int nPayload, RvSdpMediaDescr *pMediaDesc, s32 nlcCap = 0, s32 nldCap = 0  );
	static s8*  GetFmtpH264( s32 nPayload, RvSdpMediaDescr *pMediaDesc, s32 nIndex );
	static s32 GetH264Bitrate( RvSdpMediaDescr *pMediaDesc, s8 *pchAttr );
	static RvStatus AppViewSDPMessage(IN RvSdpMsg* pSdpMsg );
	static s8*  GetProductName( EmSipVendorId emProductId );

	static BOOL CheckRequestUri( RvSipCallLegHandle hCall , RvSipAppCallLegHandle haCall );
	static BOOL CheckContentAndResponse( RvSipCallLegHandle hCallLeg , RvSipMsgHandle hMsg );
	static BOOL CheckExpiresAndResponse( RvSipCallLegHandle hCallLeg , RvSipMsgHandle hMsg );
	static BOOL SendRoundTrip( RvSipCallLegHandle hsCall  );
	static void InitRegClientArray();
	static s32  GetRegClientPosition( s32 nAppHandle, RvSipRegClientHandle pRegHandle );
	static s32 FindFreeRegClientPosition();
	static BOOL SetRegClientFree( s32 nPos );
	static BOOL SetRegClientIdle( s32 nPos);//
    static s32 isRegClientFailed( s32 nPos);//
	static BOOL SetRegClientFailed(s32 nPos);//

	static BOOL SetRegClientUnReg(s32 nPos);
	static s32  isRegClientUnreg(s32 nPos);

	static void ClearRegClientArray();
	static s32 GetRegClientPositionByAppHandle( s32 nAppHandle );
	static s32 GetFormatInRtpmaplistPosition( u16 wPayload, RvSdpMediaDescr *pMediaDesc );

	static s8*  GetContentAttrName( EmSipContentAttr emSipcontentAttr );
	static s8*  GetFloorControlAttrName( EmSipFloorcontrolAttr emFloorcontrolAttr );
	static s8*  GetSetupAttrName( EmSipSetupAttr emSetupAttr );
	static s8*  GetConnectionAttrName( EmSipConnectionAttr emConnectionAttr );
	static EmSipContentAttr  GetContentAttrValue( const s8 *pContentName );
	static EmSipFloorcontrolAttr  GetFloorControlAttrValue( const s8 *pFloorcontrolName );
	static EmSipSetupAttr  GetSetupAttrValue( const s8 *pSetupName );
	static EmSipConnectionAttr  GetConnectionAttrValue( const s8 *pConnectionName );
	static s8* GetTpCapsignName( EmSipCapSign emTpCapSign );
	static EmSipCapSign GetTpCapsignValue(  const s8 *pTpCapsign );
	
private:
   //通道管理
	static CMediaStreamCtrl *m_pcStreamCtrl;
}; 
  
void Sip_Printf( EmSipLogModule emPAModule, EmSipLogType emPALogType, const char* pFormat, ... );

BOOL32 isSipCallE164Number( s8 *pbyAlias,  u16 wLen );
#endif

