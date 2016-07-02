/*****************************************************************************
   模块名      : h323sitecall
   文件名      : h323sitecall.h
   相关文件    : h323sitecall
   文件实现功能: 给上层带外方式主叫呼集的API函数和回调函数
   作者        : ZHHE
   版本        : V4.0  Copyright(C) 2005 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
******************************************************************************/

#ifndef _H323SITECALL_H
#define _H323SITECALL_H


#include "osp.h"
#include "sitecall.h"


/*============================== 长度定义 ===================================*/
#define LEN_MEDIANAME		(u16)128			//流媒体名长度
#define SITECALL_MAXLEN_ALIAS		(u16)128	//终端别名
#define LEN_PWD				(u16)32				//密码长度
#define LEN_CONF			(u16)40				//会议名长度
#define	LEN_CARD			(u16)32				//帐号名长度
#define MAXROOMNUM			(u16)32				//最大会场个数
#define LEN_ROOM			(u16)30				//会场名称长度
#define LEN_E164			(u16)30				//E164号码长度
#define MAXMULTIPIC			(u8)32				//最大子画面个数


/*============================ 日志级别定义 =================================*/
#define SITECALL_LOG_EXP    (s32)0				//异常
#define SITECALL_LOG_IMT    (s32)1				//重要日志
#define SITECALL_LOG_DEBUG  (s32)2				//一级调试信息
#define SITECALL_LOG_ALL    (s32)3				//所有调试信息 


/*============================== 枚举定义 ===================================*/

/*返回值*/
enum SiteCallError
{
	sc_err = -1,
	sc_ok  = 0,
};

/*主叫呼集命令*/
enum
{
	sitecall_make = 0x02,						//[T-->G]: TSITECALLREQ
	sitecall_answer,							//[T<--G]: TSITECALLRES
	sitecall_drop								//[T-->G]: NULL
};


/*主叫呼集结果*/
typedef enum
{
	scr_success	 = 0xff,					/*固定为0xff*/
	scr_invalidUser = 0x16,					/*用户帐号不能通过验证*/
	scr_outOfMoney	 = 0x17,				/*预付费用户帐户余额不足*/
	scr_unkownReason= 0x19					/*其他错误*/
}SiteCallResult;


/*终端类型*/
typedef enum
{
	TerminalType_isdnVideo = 0,					/*(0)ISDN视频终端*/
	TerminalType_v35,							/*(1)V.35终端*/
	TerminalType_e1,							/*(2)E1终端*/
	TerminalType_ip,							/*(3)IP终端*/
	TerminalType_phone							/*(4)Phone终端*/
}TerminalType;

/*会议命令类型(0x02)*/
typedef enum
{
	conf_immediateBegin = 0,					/*(0)立即开始会议，不用填写会议时间*/
	conf_booking,								/*(1)预约会议*/
	conf_bookingCancel,							/*(2)取消预约会议*/
	conf_p2pprepay								/*(3)点对点呼*/
}ConfCmdType;

/*召集设备类型(0x0F)*/
typedef enum
{
	unkownMT	 = 0,							/*(0)保留*/
	meettingRoom = 0x01,						/*(1)会议室终端*/
	table,										/*(2)桌面终端*/
	otherMT										/*(3)其他终端*/
}CallingTerminalType;


/*会议模式(0x10)*/
typedef enum
{
	CustomizeMode = 0,							/*(0)定制:各会场的接入速率、视音频算法和格式、QOS 等级等完全按照统一的会议规定进行*/
	AdaptMode									/*(1)自适应:各会场可不一样，由MCU 进行相关匹配。*/
}ConfMode;


/*计费类型(0x0E)*/
typedef enum
{
	payTypeCradPay = 0,							/*(0)帐号付费*/
	payTypeOrganigerPay,						/*(1)发起终端付费*/
	payTypeTerminalPay							/*(2)每终端付费*/
}PayType;

/*传输速率(0x5A)*/
typedef enum
{
	rate_n64   = 0x20,							/*(0)64kbps*/
	rate_n2m64 = 0x21,							/*(1)2×64kbps*/
	rate_n3m64 = 0x22,							/*(2)3×64kbps*/
	rate_n4m64 = 0x23,							/*(3)4×64kbps*/
	rate_n5m64 = 0x24,							/*(4)5×64kbps*/
	rate_n6m64 = 0x25,							/*(5)6×64kbps*/
	rate_r384  = 0x26,							/*(6)384kbps*/
	rate_r1536 = 0x2B,							/*(7)1536kbps*/
	rate_r1920 = 0x2c,							/*(8)1920kbps*/
	rate_r128  = 0x2d,							/*(9)128kbps*/
	rate_r192  = 0x2e,							/*(10)192kbps*/
	rate_r256  = 0x2f,							/*(11)256kbps*/
	rate_r320  = 0x30,							/*(12)320kbps*/
	rate_r512  = 0x37,							/*(13)512kbps*/
	rate_r768  = 0x38,							/*(14)768kbps*/
	rate_r1152 = 0x3A,							/*(15)1152kbps*/
	rate_r1472 = 0x3D,							/*(16)1472kbps*/
	/*(注：V2.0版本中新增3M、4M、6M、8M速率)*/
	rate_r3M   = 0x3E,							/*(17)3M(2880kbps)*/
	rate_r4M   = 0x3F,							/*(18)4M(3840kbps)*/
	rate_r6M   = 0x40,							/*(19)6M(5760kbps)*/
	rate_r8M   = 0x41							/*(20)8M(7680kbps)*/
}DataRate;


/*视频算法子信息(0x5B)数据编码*/
enum SiteCallVideo
{
	sc_v_H261 = 0x41,							/*H.261*/
	sc_v_H263 = 0x42,							/*H.263*/
	sc_v_H264 = 0x43,							/*H.264*/
	sc_v_Real = 0x44,							/*Real格式*/
	sc_v_WindowsMedia = 0x45,					/* WindowsMedia格式*/
	sc_v_MPEG4 = 0x46							/* MPEG4格式*/
		
};


/*视频格式子信息(0x5C)数据编码*/
enum SiteCallVideoFormat
{
	sc_vf_H263QCIF		   = 0x00,				/*H.263 QCIF*/
	sc_vf_H263CIF_H261QCIF = 0x01,				/*H.263 CIF/H.261 QCIF*/
	sc_vf_H261CIF		   = 0x02,				/* H.261 CIF*/
	sc_vf_H263_4CIF		   = 0x03,				/*H.263 4CIF*/
	sc_vf_VGA			   = 0x04,				/*各种协议的Vga*/
	sc_vf_2CIF			   = 0x05,				/*各种协议的2cif*/
	sc_vf_4CIF             = 0x06,				/*各种协议的4CIF*/
	sc_vf_SVGA             = 0x07,				/*各种协议的Svga*/
	sc_vf_SXVGA			   = 0x08,				/*各种协议的sxvga*/
	sc_vf_16CIF			   = 0x09				/*各种协议的16CIF*/
		
};

/*视频帧率子信息(0x5D)数据编码*/
enum SiteCallVideoFrame
{
    sc_v_frameauto = 0x0,
	sc_v_frame30 = 0x01,						/*30帧/秒*/
	sc_v_frame15 = 0x02,						/*15帧/秒*/
	sc_v_frame10 = 0x03,						/*10帧/秒*/
	sc_v_frame75 = 0x04 						/*7.5帧/秒*/
};

/*音频算法子信息(0x5E)数据编码*/
enum SiteCallAudio
{
	sc_a_G7231	   = 0x0A,						/*G.723.1*/
	sc_a_G729	   = 0x0B,						/*G.729*/
	sc_a_G711A_56K = 0x12,						/*G.711A-56K*/
	sc_a_G711U_56K = 0x13,						/*G.711U-56K*/
	sc_a_G722_64K  = 0x17,						/*G.722-64K,(V2.0)*/
	sc_a_G722_56K  = 0x18,						/*G.722-56K*/
	sc_a_G722_48K  = 0x19,						/*G.722-48K*/
	sc_a_G728      = 0x1D,						/*G.728*/
	sc_a_G711A_64K = 0x1E,						/*G.711A-64K*/
	sc_a_G711U_64K = 0x1F,						/*G.711u-64K*/
	sc_a_Real	   = 0x20,						/*Real格式,(V2.0)*/
	sc_a_WindowsMedia =	0x21,					/*WindowsMedia格式,(V2.0)*/
	sc_a_MPEG4	   = 0x22						/*MPEG4格式,(V2.0)*/
};

/*============================== 结构定义 ===================================*/
typedef struct tagSiteCallAddress
{
	u32 m_dwIP;
	u16 m_wPort;

	tagSiteCallAddress()
	{
		Clear();
	}

	void Clear()
	{
		m_dwIP = 0;
		m_wPort = SITECALLPORT;
	}
	//设置transport地址
	void SetNetAddress(u32 dwIPAddr, u16 wIPPort=SITECALLPORT)
	{
		m_dwIP   = dwIPAddr;
		m_wPort  = wIPPort;
	}
	//获取IP地址
	u32 GetIP()
	{
		return m_dwIP;
	}
	//获取端口
	u16 GetPort()
	{
		return m_wPort;
	}
}SiteCallAddr;

typedef struct tagSiteCallNetConfig
{
	BOOL32 m_bIsGk;
	SiteCallAddr m_tLocalAddr;
	SiteCallAddr m_tGkAddr;
	
	tagSiteCallNetConfig()
	{
		Clear();
	}

	void Clear()
	{
		m_bIsGk = FALSE;
		m_tLocalAddr.Clear();
		m_tGkAddr.Clear();
	}

	void SetBeGk(BOOL32 bGk)
	{
		m_bIsGk = bGk;
	}
	BOOL32 IsGk()
	{
		return m_bIsGk;
	}

	void SetLocalAddr(u32 dwIp, u16 wPort = SITECALLPORT)
	{
		m_tLocalAddr.m_dwIP  = dwIp;
		m_tLocalAddr.m_wPort = wPort;
	}
	u32 GetLocalIP()
	{
		return m_tLocalAddr.GetIP();
	}
	u16 GetLocalPort()
	{
		return m_tLocalAddr.GetPort();
	}
	void SetGkAddr(u32 dwIp, u16 wPort = SITECALLPORT)
	{
		m_tGkAddr.m_dwIP = dwIp;
		m_tGkAddr.m_wPort = wPort;
	}
	u32 GetGkIP()
	{
		return m_tGkAddr.GetIP();
	}
	u16 GetGkPort()
	{
		return m_tGkAddr.GetPort();
	}
}TSCNetCfg, *PTSCNetCfg;

/*公共属性*/
typedef struct tagCommonInfo
{
	DataRate  m_emRate;							/*传输速率(u8),注：在枚举列表中选择!*/
	
	u8		  m_byVideoType;					//视频算法
	u8		  m_byVideoFormat;					//视频格式
	u8		  m_byVideoFrame;					//视频帧率
	u8		  m_byAudioType;					//音频算法	
												/*
												注：当前版本的华为终端，主叫呼集上也只有
												H261QCIF,H261CIF,H263QCIF,H23CIF
												G711_64K,G722_64K,G728_16K	
												*/
	tagCommonInfo()
	{
		Clear();
	}
	
	void Clear()
	{
		m_emRate		= rate_r384;
		m_byVideoType	= sc_v_H261;
		m_byVideoFormat = sc_vf_H261CIF;
		m_byVideoFrame  = sc_v_frame30;
		m_byAudioType	= sc_a_G711A_64K;
	}
	
	/*设置传输速率*/
	void SetRate(DataRate emRate)
	{
		m_emRate = emRate;
	}

	/*获得传输速率*/
	DataRate GetRate()
	{
		return m_emRate;
	}
	
	void SetVideoType(u8 byType)
	{
		m_byVideoType = byType;
	}

	void SetVideoFormat(u8 byFormat)
	{
		m_byVideoFormat = byFormat;
	}

	void SetVideoFrame(u8 byFrame)
	{
		m_byVideoFrame = byFrame;
	}

	void SetAudioType(u8 byType)
	{
		m_byAudioType = byType;
	}

}TCOMMONINFO, *PTCOMMONINFO;

/*帐号密码*/
typedef struct tagCardInfo
{
	s8 m_achCardNumber[LEN_CARD];				/*帐号*/
	s8 m_achCardPwd[LEN_PWD];					/*密码*/
	
	tagCardInfo()
	{
		Clear();
	}
	
	void Clear()
	{
		memset(m_achCardNumber, 0, sizeof(m_achCardNumber));
		memset(m_achCardPwd, 0, sizeof(m_achCardPwd));
	}
	
	/*设置帐号*/
	void SetCardNumber(s8* pCardNumber, u16 wLen)
	{		
		if (pCardNumber != NULL && wLen < LEN_CARD+1) 
		{
			memset(m_achCardNumber, 0, sizeof(m_achCardNumber));
			memcpy(m_achCardNumber, pCardNumber, wLen);
			m_achCardNumber[wLen] = '\0';
		}	
	}
	/*获取帐号*/
	u16 GetCardNumber(s8 *pCardNumber, u16 &wLen)
	{
		if (pCardNumber != NULL) 
		{
			u16 realLen = min( wLen, LEN_CONF);
			memcpy(pCardNumber, m_achCardNumber, realLen);
			return realLen;
		}
	}
	
	/*设置密码*/
	void SetCardPwd(s8* pCardPwd, u16 wLen)
	{		
		if (pCardPwd != NULL && wLen < LEN_PWD+1) 
		{
			memset(m_achCardPwd, 0, sizeof(m_achCardPwd));
			memcpy(m_achCardPwd, pCardPwd, wLen);
			m_achCardPwd[wLen] = '\0';
		}	
	}
	/*获取密码*/
	u16 GetCardPwd(s8 *pCardPwd, u16 &wLen)
	{
		if (pCardPwd != NULL) 
		{
			u16 realLen = min( wLen, LEN_CONF);
			memcpy(pCardPwd, m_achCardPwd, realLen);
			return realLen;
		}
	}
	
}TCARDINFO, *PTCARDINFO;

/*会议参数*/
typedef struct tagConferenceParam
{
	s8 m_achConfName[LEN_CONF];					/*会议名*/
	s8 m_achConfPwd[LEN_PWD];					/*会议控制密码,注：必须为数字字符串(HuaWei)*/
	ConfMode m_emConfMode;						/*会议模式*/
	
	tagConferenceParam()
	{
		Clear();
	}
	
	void Clear()
	{
		m_emConfMode = AdaptMode;
		memset(m_achConfName, 0, sizeof(m_achConfName));
		memset(m_achConfPwd, 0, sizeof(m_achConfPwd));
	}

	/*设置会议名*/
	s32 SetConfName(s8* pConfName)
	{
		if (pConfName == NULL)
		{
			return -1;
		}

		memset(m_achConfName, 0, sizeof(m_achConfName));
		u16 wLen = min(strlen(pConfName), LEN_CONF);
		strncpy(m_achConfName, pConfName, wLen);
		m_achConfName[wLen] = '\0';
		return wLen;
	}
	/*获取会议名*/
	s8* GetConfName()
	{
		return m_achConfName;
	}

	/*设置密码*/
	void SetConfPwd(s8* pConfPwd, u16 wLen)
	{		
		if (pConfPwd != NULL && wLen < LEN_PWD+1) 
		{
			memset(m_achConfPwd, 0, sizeof(m_achConfPwd));
			memcpy(m_achConfPwd, pConfPwd, wLen);
			m_achConfPwd[wLen] = '\0';
		}	
	}
	/*获取密码*/
	u16 GetCardPwd(s8 *pConfPwd, u16 &wLen)
	{
		if (pConfPwd != NULL) 
		{
			u16 realLen = min( wLen, LEN_CONF);
			memcpy(pConfPwd, m_achConfPwd, realLen);
			return realLen;
		}
	}
	
	/*设置会议模式*/
	void SetConfMode(ConfMode emConfMode)
	{
		m_emConfMode = emConfMode;
	}
	/*获得会议模式*/
	ConfMode GetConfMode()
	{
		return m_emConfMode;
	}
	
}TConfParam, *PTConfParam;


/*会议参数*/
typedef struct tagSiteCallConferenceInfo
{
	TConfParam m_tConfParam;
	ConfCmdType m_emConfCmdType;				/*会议命令类型,暂只取conf_immediateBegin*/
	BOOL32 m_bDataConf;							/*数据会议标志*/
	u8	 m_byMultiPic;							/*多画面资源,最大子画面个数，0表示无*/
	u16	 m_wRoomNum;							/*会场个数,注:包括自身，最小为2*/

	TCARDINFO m_tCardInfo;						/*帐号密码*/
	PayType m_emPayType;						/*计费类型*/

	CallingTerminalType m_emMtType;				/*召集设备类型*/
	TCOMMONINFO m_tCommInfo;					/*公共属性*/

	tagSiteCallConferenceInfo()
	{
		Clear();
	}
	
	void Clear()
	{
		m_byMultiPic = 0;
		m_wRoomNum   = 0;
		m_bDataConf  = FALSE;
		m_emPayType		 = payTypeCradPay;
		m_emConfCmdType	 = conf_immediateBegin;
		
		m_tCardInfo.Clear();		
		m_tConfParam.Clear();
		m_tCommInfo.Clear();
	}
	
	/*设置会议参数*/
	void SetConfParam(TConfParam &tConfParam)
	{
		m_tConfParam = tConfParam;
	}
	/*获取会议参数*/
	TConfParam* GetConfParam()
	{
		return &m_tConfParam;
	}
	
	/*设置会议命令类型*/
	void SetConfCmdType(ConfCmdType emCmdType)
	{
		m_emConfCmdType = emCmdType;
	}
	/*获得会议命令类型*/
	ConfCmdType GetConfCmdType()
	{
		return m_emConfCmdType;
	}


	/*设置最大子画面个数*/
	void SetMultiPicNum(u8 byNum)
	{
		m_byMultiPic = (byNum > MAXMULTIPIC ? MAXMULTIPIC : byNum);
	}
	/*设置最大子画面个数*/
	u8 GetMultiPicNum()
	{
		return m_byMultiPic;
	}

	/*设置会场个数*/
	void SetRoomNum(u16 wNum)
	{
		m_wRoomNum = wNum;
	}
	/*获得会场个数*/
	u16 GetRoomNum()
	{
		return m_wRoomNum;
	}

	/*设置帐号密码*/
	void SetCardInfo(TCARDINFO &tCardInfo)
	{
		m_tCardInfo = tCardInfo;
	}
	/*获得帐号密码*/
	TCARDINFO* GetCardInfo()
	{
		return &m_tCardInfo;
	}

	/*设置计费类型*/
	void SetPayType(PayType emPayType)
	{
		m_emPayType = emPayType;
	}
	/*获得计费类型*/
	PayType GetPayType()
	{
		return m_emPayType;
	}

	/*设置召集设备类型*/
	void SetMtType(CallingTerminalType emMtType)
	{
		m_emMtType = emMtType;
	}
	/*获得召集设备类型*/
	CallingTerminalType GetMtType()
	{
		return m_emMtType;
	}

	/*会议会场公共类子信息*/
	void SetCommonInfo(TCOMMONINFO tCommonInfo)
	{
		m_tCommInfo = tCommonInfo;
	}
	TCOMMONINFO& GetCommonInfo()
	{
		return m_tCommInfo;
	}

}TSiteCallConfInfo, *PTSiteCallConfInfo;

/*会场消息*/
typedef struct tagRoomInfo
{
	s8 m_achRoomName[LEN_ROOM];					/*会场名称*/
	s8 m_achAlias[LEN_E164];					/*终端号码*/

	TerminalType m_emMtType;					/*终端类型*/
	TCOMMONINFO m_tCommInfo;					/*公共属性*/

	
	tagRoomInfo()
	{
		Clear();
	}

	void Clear()
	{

		m_emMtType   = TerminalType_ip;
		m_tCommInfo.Clear();
		memset(m_achRoomName, 0, sizeof(m_achRoomName));
		memset(m_achAlias, 0, sizeof(m_achAlias));
	}

	/*设置会场名称*/
	BOOL32 SetRoomName(s8 *pName)
	{
		if (pName == NULL)
		{
			return FALSE;
		}

		u16 wLen = (u16)min(strlen(pName), LEN_ROOM);
		memset(m_achRoomName, 0, sizeof(m_achRoomName));
		strncpy(m_achRoomName, pName, wLen);
		m_achRoomName[wLen] = '\0';
		return TRUE;
	}
	/*获得会场名称*/
	s8* GetRoomName()
	{
		return m_achRoomName;
	}

	/*设置别名*/
	BOOL32 SetAlias(const s8* pAlias )
	{
		if (pAlias == NULL ) 
			return FALSE;
		
		memset( m_achAlias, 0, sizeof(m_achAlias) );
		
		u16 nLen = (u16)min( strlen(pAlias), LEN_E164);
		strncpy( m_achAlias, pAlias, nLen );
		m_achAlias[nLen] = '\0';
		return TRUE;
	}	
	/*获得别名*/
	s8* GetAlias()  
	{
		return m_achAlias;	
	}

	/*设置终端类型*/
	void SetMtType(TerminalType emType)
	{
		m_emMtType = emType;
	}
	/*获得终端类型*/
	TerminalType GetMtType()
	{
		return m_emMtType;
	}

	/*会议会场公共类子信息*/
	void SetCommonInfo(TCOMMONINFO tCommonInfo)
	{
		m_tCommInfo = tCommonInfo;
	}
	TCOMMONINFO& GetCommonInfo()
	{
		return m_tCommInfo;
	}
	
}TROOMINFO, *PTROOMINFO;

/*建立带外连接请求*/
typedef struct tagConnectRequest
{
	u32			m_dwIPAddr;						/*终端IP地址*/
	s8			m_achAlias[SITECALL_MAXLEN_ALIAS];		/*终端别名(不超过128字节)E164*/


	tagConnectRequest()
	{
		Clear();
	}

	void Clear()
	{
		m_dwIPAddr = 0;
		memset(m_achAlias, 0, sizeof(m_achAlias));
	}

	/*设置终端IP地址*/
    void SetIPAddr(u32 dwIPAddr)
    {
        m_dwIPAddr = dwIPAddr;
    }
    /*得到呼叫信令地址*/
    u32 GetCallAddr()
    {
        return m_dwIPAddr;
    }

	/*设置别名*/
	BOOL32 SetAlias(const s8* pAlias )
	{
		if (pAlias == NULL ) 
			return FALSE;
		
		memset( m_achAlias, 0, sizeof(m_achAlias) );
		
		u16 nLen = min( strlen(pAlias), SITECALL_MAXLEN_ALIAS);
		strncpy( m_achAlias, pAlias, nLen );
		m_achAlias[nLen] = '\0';
		return TRUE;
	}	
	/*获得别名*/
	s8* GetAlias()  
	{
		return m_achAlias;	
	}
	
}TCONNECTREQ, *PTCONNECTREQ;

/*建立带外连接响应*/
typedef struct tagConnectResponse
{
	u8		  m_byResponse;					/*主叫呼集连接结果*/

	tagConnectResponse()
	{
		Clear();
	}

	void Clear()
	{
		m_byResponse = 0;
	}

	/*设置主叫呼集连接结果*/
	void SetResponse(u8 byResponse)
	{
		m_byResponse = byResponse;
	}
	/*获得主叫呼集连接结果*/
	u8 GetResponse()
	{
		return m_byResponse;
	}

}TCONNECTRES, *PTCONNECTRES;

/*主叫呼集请求*/
typedef struct tagSiteCallRequest
{
	TSiteCallConfInfo	m_tConfInfo;					/*会议类子信息*/
	TROOMINFO   m_atRoom[MAXROOMNUM];			/*会场类子信息,m_atRoom[0]为主会场*/

	tagSiteCallRequest()
	{
		Clear();
	}

	void Clear()
	{
		m_tConfInfo.Clear();
		memset(m_atRoom, 0, sizeof(m_atRoom));
	}


	/*设置会议信息*/
    void SetConfInfo(TSiteCallConfInfo &tConfInfo)
    {      
        m_tConfInfo = tConfInfo;
    }
    /*得到会议信息*/
    TSiteCallConfInfo& GetConfInfo()
    {
		return m_tConfInfo;
    }

	
    /*设置会场信息*/
    void SetRoomInfo(TROOMINFO &tRoomInfo, u16 wRoomNo)
    {
        if (wRoomNo >= MAXROOMNUM )
            return;
        
        m_atRoom[wRoomNo] = tRoomInfo;
    }
    /*得到会场信息*/
    TROOMINFO& GetRoomInfo(u16 wRoomNo)
    {
        if ( wRoomNo < MAXROOMNUM ) 
            return m_atRoom[wRoomNo];
        else
            return m_atRoom[0];
    }
	
}TSITECALLREQ, *PTSITECALLREQ;

/*主叫呼集响应*/
typedef struct tagSiteCallResponse
{
	SiteCallResult m_emResult;					/*主叫呼集响应*/
	s8		   m_achReason[SITECALL_MAXLEN_ALIAS];		/*成功－会议号;失败－具体原因*/

	tagSiteCallResponse()
	{
		Clear();
	}

	void Clear()
	{
		m_emResult = scr_unkownReason;
		memset(m_achReason, 0, sizeof(m_achReason));
	}

	void SetResult(SiteCallResult emResult)
	{
		m_emResult = emResult;
	}
	SiteCallResult GetResult()
	{
		return m_emResult;
	}

	/*设置主叫呼集结果*/
	BOOL32 SetReason(const s8* pReason)
	{
		if (pReason == NULL ) 
			return FALSE;
		
		memset( m_achReason, 0, sizeof(m_achReason) );
		
		u16 wLen = min( strlen(pReason), SITECALL_MAXLEN_ALIAS);
		strncpy( m_achReason, pReason, wLen );
		m_achReason[wLen] = '\0';
		return TRUE;
	}	
	/*获得主叫呼集结果*/
	s8* GetReason()  
	{
		return m_achReason;	
	}
	
}TSITECALLRES, *PTSITECALLRES;


/*断开主叫呼集连接请求*/
/*注：V2.0版本中，对“断开主叫呼集连接请求”命令进行了扩展*/
typedef struct tagDisConnectRequest
{
}TDISCONNECTREQ, *PTDISCONNECTREQ;

/*断开主叫呼集连接响应*/
typedef struct tagDisConnectResponse
{
	u8	m_byReason;								/*断开主叫呼集连接结果*/		
	/*扩展信息......*/
	tagDisConnectResponse()
	{
		Clear();
	}
	
	void Clear()
	{
		m_byReason = 0;
	}
	
	void SetResult(u8 byReason)
	{
		m_byReason = byReason;
	}
	u8 GetResult()
	{
		return m_byReason;
	}
}TDISCONNECTRES, *PTDISCONNECTRES;


/*================================= 结构定义 ================================*/
typedef s32 (* NotifyNewSiteCallT)(HSITECALL hSiteCall);

typedef s32 (* NotifySiteCallMessageT)(HSITECALL hSiteCall, u16 msgType, const void* pBuf, u16 nBufLen);

typedef  struct
{
	NotifyNewSiteCallT					fpNotifyNewSiteCall;
	NotifySiteCallMessageT				fpNotifySiteCallMessage;
}SITECALLEVENTS, *LPSITECALLEVENTS;

/*====================================================================
    函数名      : sitecalllog
    功能        : 设置打印级别
    算法实现    : 无
    引用全局变量: 无
    输入参数说明: 
    返回值说明  : 无
====================================================================*/
API void setsitecalllog(u8 byLevel);

/*====================================================================
函	 数   名: SiteCallInit
功	      能: 初始化主叫呼集模块
算 法 实  现: 
引用全局变量: g_ptTrans
输入参数说明: 无			  
返   回   值: 成功 - TRUE
			  失败 - FALSE		
====================================================================*/
BOOL32 H323SiteCallInit(TSCNetCfg* ptConfig);

/*====================================================================
函	 数   名: SetCallBack
功	      能: 注册应用回调函数
算 法 实  现: 
引用全局变量: g_ptTrans
输入参数说明: lpEvent - 回调函数结构指针
返   回   值: 成功 - sc_ok
失败 - sc_err
====================================================================*/
s32 SCSetCallBack(LPSITECALLEVENTS lpEvent);

/*====================================================================
函	 数   名: SiteCallSetGkIp
功	      能: 
算 法 实  现: 
引用全局变量: 
输入参数说明: 无			  
返   回   值: void	
====================================================================*/
API void SiteCallSetGkIp(u32 gkIp, u16 gkPort = SITECALLPORT);

/*====================================================================
函	 数   名: CreateNewSiteCall
功	      能: 创建新的呼叫，必须先创建再呼叫
算 法 实  现: 
引用全局变量: 无
输 入 参  数: haCall -   应用程序呼叫句柄
输 出 参  数: lphsCall - 协议栈呼叫句柄
返   回   值: 成功 - act_ok
失败 - act_err
====================================================================*/
s32 H323CreateNewSiteCall(LPHSITECALL lphSiteCall, bool bSync = true);

/*====================================================================
函	 数   名: SendSiteCallMsg
功	      能: 主叫呼集命令消息分拣
算 法 实  现: 
引用全局变量: 无
输入参数说明: 
返   回   值: 成功 - sc_ok
			  失败 - sc_err
====================================================================*/
s32 SendSiteCallCommand(HSITECALL hSiteCall, u16 wMsgType, void* pBuf,u16 wBufLen, bool bSync = true);

/*====================================================================
函	 数   名: SiteCallClose
功	      能: 关闭主叫呼集
算 法 实  现: 
引用全局变量: g_ptTrans
输入参数说明: 无			  
返   回   值: void	
====================================================================*/
s32 SiteCallClose(HSITECALL hSiteCall, bool bSync);

/*====================================================================
函	 数   名: SiteCallEnd
功	      能: 关闭主叫呼集模块
算 法 实  现: 
引用全局变量: g_ptTrans
输入参数说明: 无			  
返   回   值: void	
====================================================================*/
s32 SiteCallEnd(bool bSync = true);

//debug print
API void setsitecalllog(u8 byLevel);
API void printsitecall();

#endif //_H323SITECALL_H










































