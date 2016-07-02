/*****************************************************************************
   模块名      : mcu
   文件名      : mcuinnerstruct.h
   相关文件    : 
   文件实现功能: MCU内部使用的结构
   作者        : 陈代伟
   版本        : V4.7  Copyright(C) 2001-2002 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   2012/05/17  4.7         chendaiei    创建
******************************************************************************/
#ifndef __MCUINNERSTRUCT_H_
#define __MCUINNERSTRUCT_H_

#include "osp.h"
#include <string.h>
#include <stdlib.h>
#include "kdvsys.h"
#include "mcuconst.h"
#include "kdvdef.h"
#include "vccommon.h"
#include "mcustruct.h"

#ifdef WIN32
    #pragma comment( lib, "ws2_32.lib" ) 
    #pragma pack( push )
    #pragma pack( 1 )
    #define window( x )	x
#else
    #include <netinet/in.h>
    #define window( x )
#endif

#if defined(_8KE_) || defined(_8KH_) || defined(_8KI_)
#define  MAXNUM_CONF_PRS_CHNNL			30				//会议使用的最大PRS通道
#else
#define  MAXNUM_CONF_PRS_CHNNL			32				//会议使用的最大PRS通道
#endif

#define  MPU2_BASCHN_MAXENCABILITY		3000				//MPU2 BAS最大编码能力
#define	 MPU2_BASCHN_DIVISION_ABILITY	1000				//MPU2 BAS能力分段区间(适应媒控层按每1000能力分段)
#define	 MPU2_BASCHN_ENCARRAY_NUM		3					//MPU2_BASCHN_MAXENCABILITY/MPU2_BASCHN_DIVISION_ABILITY
#define  MCS_MAXNUM_PLAN                8					//MCS最大预案数
#define  MAXNUM_CONF_VMP                16					//会议支持的最大vmp个数
#if defined _LINUX
#endif

#ifdef _VXWORKS_
	#ifdef _MINIMCU_
		#define     MAXNUM_MCU_SCHEDULE_CONF         4   //预约会议最大总和 
	#else
		#define     MAXNUM_MCU_SCHEDULE_CONF            4  //预约会议最大总和 
	#endif 
#elif defined(_LINUX_)
	#ifdef _MINIMCU_
		#define     MAXNUM_MCU_SCHEDULE_CONF            4 //预约会议最大总和 
	#else
		#ifdef _LINUX12_
			#define     MAXNUM_MCU_SCHEDULE_CONF        16 //MPC2 //预约会议最大总和 
		#else
			#define     MAXNUM_MCU_SCHEDULE_CONF            4  //预约会议最大总和 
		#endif 
	#endif
#else   // WIN32
	#define     MAXNUM_MCU_SCHEDULE_CONF                4 //预约会议最大总和 
#endif


enum emFakeMask                      //终端能力集作假的mask值
{
	emMainStreamFakedMask = 0x01,   //主视频能力集作假mask
		emDoubleStreamFakedMask = 0x02, //双流能力集作假mask
		emAudioDescFakedMask = 0x04     //音频能力集作假mask
};

#define IS_MAINSTREAMCAP_FAKE(byMask)   !((byMask & emMainStreamFakedMask) == 0)									
#define IS_DOUBLESTREAMCAP_FAKE(byMask)   !( (byMask & emDoubleStreamFakedMask) == 0)
#define IS_AUDIODESC_FAKE(byMask)  !((byMask & emAudioDescFakedMask) == 0)

#define SET_MAINSTREAM_FAKED(byMask)  byMask |= emMainStreamFakedMask;
#define SET_DOUBLESTREAM_FAKED(byMask)  byMask |= emDoubleStreamFakedMask;
#define SET_AUDIODESC_FAKED(byMask)  byMask |= emAudioDescFakedMask; 


/*------------------------------------------------------------- 
*网络组常量 */
#define KEY_mcunetIsGKRRQUsePwd					(const s8*)"mcunetIsGkUseRRQPwd"
#define KEY_mcunetGKRRQPwd					    (const s8*)"mcunetGKRRQPwd"

//DMZ主机配置常量
#define SECTION_ProxyLocalInfo                  (const s8*)"LocalInfo"
#define KEY_mcuUseDMZ                           (const s8*)"UseDmz"
#define KEY_mcuIpDmzAddr                        (const s8*)"IpDmzAddr"

// [pengguofeng 4/12/2013] 编码方式常量，适用于/conf/xxx.ini
#define SECTION_McuEncode						(const s8*)"mcuEncode"
#define KEY_encoding							(const s8*)"encoding"

//8KIVMP高清前适配通道数目
#define		MAXNUM_8KIVMP_ADPHDCHL_2				(u8)2	//8KIVMP高清前适配通道数目
#define		MAXNUM_8KIVMP_ADPHDCHL_4				(u8)4	//8KIVMP高清前适配通道数目
#define		MAXNUM_8KIVMP_ADPHDCHL_9				(u8)9	//8KIVMP高清前适配通道数目
#define		MAXNUM_8KIVMP_ADPHDCHL_16				(u8)16	//8KIVMP高清前适配通道数目

struct TMixMsgInfo
{
private:
	u8 m_byMtNum;
	u8 m_byIsMMcuMix;
	u8 m_byUseMixerEqpId;
	TMt *m_ptMtArray;
	u16 m_wErrorCode;
	u8  m_bySpecMixReplace;
	
public:
	TMixMsgInfo ( void )
	{
		Clear();
	}
	
	void Clear( void )
	{
		memset(this,0,sizeof(*this));
	}
	
	void SetMixMtNum ( u8 byMtNUm )
	{
		m_byMtNum = byMtNUm;
	}

	u8	GetMixMtNum ( void )
	{
		return m_byMtNum;
	}
	
	u8 GetEqpId ( void )
	{
		return m_byUseMixerEqpId;
	}

	void SetEqpId ( u8 byEqpId )
	{
		m_byUseMixerEqpId = byEqpId;
	}
	
	BOOL32 IsCascadeMixMsg ( void )
	{
		return (m_byIsMMcuMix == 1);
	}

	void SetCascadeMixMsg ( void )
	{
		m_byIsMMcuMix = 1;
	}
	
	emMcuMixMode GetMixMode ( void )
	{
		return m_byMtNum > 0 ? mcuPartMix:mcuWholeMix;
	}

	void SetMixMemberPointer ( TMt *pMt)
	{
		m_ptMtArray = pMt;
	}
	
	TMt * GetMixMemberPointer ( void )
	{
		return m_ptMtArray;
	}

	void SetErrorCode ( u16 wErrorcode)
	{
		m_wErrorCode = wErrorcode;
	}

	u16 GetErrorCode ( void )
	{
		return m_wErrorCode;
	}

	void SetReplaceMemberFlag( u8 bySpecMixReplace)
	{
		m_bySpecMixReplace = bySpecMixReplace;
	}

	u8 GetReplaceMemberFlag ( void )
	{
		return m_bySpecMixReplace;
	}
};

//MtAdp注册信息
struct TMtAdpReg
{
protected:    
	u8      m_byDriId;         //本MtAdp编号
	u8      m_byAttachMode;    //MtAdp的附属方式
	u8      m_byProtocolType;  //适配的协议类型//H323、H320、SIP
	u8      m_byMaxMtNum;      //适配的支持最大终端数
	u32     m_dwIpAddr;        //Ip地址，网络序
	s8      m_achMtAdpAlias[MAXLEN_ALIAS];  //MtAdp别名
    u16     m_wQ931Port;
    u16     m_wRasPort;
	//4.0R4扩展字段
    u16     m_wVersion;     //版本信息	ZGC	2007-09-28
    //4.0R5
	u8      m_byMaxHDMtNum;   //Mtadp支持的最大高清接入点数 //  [7/28/2011 chendaiwei]
	u8		m_abyMacAddr[6];  //Mtadp所属的单板的MAC地址[5/16/2012 chendaiwei]
	u16     m_wMaxAudMtNum;   //最大语音接入终端数

public:
	TMtAdpReg()
	{
		memset(this, 0, sizeof(TMtAdpReg)); 
		m_byProtocolType = PROTOCOL_TYPE_H323;
		m_byMaxMtNum = MAXNUM_DRI_MT;
	}
	
	VOID	SetDriId(u8 byDriId){ m_byDriId = byDriId;}
	u8  	GetDriId() const { return m_byDriId;}
	VOID	SetAttachMode(u8 byAttachMode){ m_byAttachMode = byAttachMode;} 
	u8  	GetAttachMode( void ) const { return m_byAttachMode; }
	VOID	SetIpAddr(u32 dwIp){m_dwIpAddr = htonl(dwIp);}
	u32   	GetIpAddr() const { return ntohl(m_dwIpAddr);}
	VOID	SetProtocolType(u8 byProtocolType){ m_byProtocolType = byProtocolType;}
	u8  	GetProtocolType() const { return m_byProtocolType;}
	VOID	SetMaxMtNum(u8 byMaxMtNum){ m_byMaxMtNum = byMaxMtNum;}
	u8  	GetMaxMtNum() const { return m_byMaxMtNum;}
    void    SetQ931Port(u16 wQ931Port){ m_wQ931Port = htons(wQ931Port);} 
    u16     GetQ931Port( void ) const { return ntohs(m_wQ931Port); }
    void    SetRasPort(u16 wRasPort){ m_wRasPort = htons(wRasPort);} 
    u16     GetRasPort( void ) const { return ntohs(m_wRasPort); }	

	VOID    SetMaxHDMtNum(u8 byMaxHdMtNum){ m_byMaxHDMtNum = byMaxHdMtNum; }
	u8      GetMaxHDMtNum() const{ return m_byMaxHDMtNum; }

	VOID    SetMaxAudMtNum(u16 wMaxAudMtNum){m_wMaxAudMtNum = wMaxAudMtNum;}
	u16     GetMaxAudMtNum(){ return m_wMaxAudMtNum; }

	VOID SetAlias(LPCSTR lpszAlias)
	{
		if(lpszAlias != NULL)
			strncpy(m_achMtAdpAlias, lpszAlias, MAXLEN_ALIAS - 1);
	}

	VOID GetAlias(s8 *pchBuf, u16  wBufLen)
	{
		if(pchBuf != NULL)
			strncpy(pchBuf, m_achMtAdpAlias, wBufLen);
	}

	u16 GetVersion(void) const { return ntohs(m_wVersion); }
	void SetVersion(u16 wVersion) { m_wVersion = htons(wVersion); }
	void GetMacAddr( u8 *pabyMacAddr) const{ if(pabyMacAddr != NULL) memcpy(pabyMacAddr,m_abyMacAddr,sizeof(m_abyMacAddr));}
	void SetMacAddr( u8 *pabyMacAddr ){ if(pabyMacAddr != NULL) memcpy(m_abyMacAddr,pabyMacAddr,sizeof(m_abyMacAddr));}
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;


struct TProxyDMZInfo
{
private:
	u8   m_byIsUseDMZ;						//是否启用DMZ
	u32  m_dwDMZIp;							//网络序（DMZ IP配置）
	u8   m_byEthIndx;						//DMZ配置的网口indx
public:
	TProxyDMZInfo ( void )
	{
		memset(this,0,sizeof(*this));
	}

	~TProxyDMZInfo ( void )
	{
		memset(this,0,sizeof(*this));
	}

	void SetIsUseDMZ ( u8 byIsUseDMZ )
	{
		m_byIsUseDMZ = byIsUseDMZ;
	}

	BOOL32 IsUseDMZ ( void )
	{
		return (m_byIsUseDMZ == 1);
	}
	
	//dwIP 主机序
	void SetDMZIpAddr ( u32 dwIp)
	{
		m_dwDMZIp = htonl(dwIp);
	}
	
	//返回主机序
	u32 GetDMZIpAddr ( void )
	{
		return ntohl(m_dwDMZIp);
	}

	void SetDMZEthIndx ( u8 byEthIndx)
	{
		m_byEthIndx = byEthIndx;
	}
	
	u8 GetDMZEthIndx ( void )
	{
		return m_byEthIndx;
	}

}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//MCU基本配置信息
//继承自MCU general信息，扩展GK用户名和密码[3/19/2013 chendaiwei]
struct TMcuNewGeneralCfg: public TMcuGeneralCfg
{
public:
	TMcuNewGeneralCfg ( void ){memset(this,0,sizeof(*this));}
	~TMcuNewGeneralCfg (){memset(this,0,sizeof(*this));}
	void SetGkRRQUsePwdFlag( u8 byIsUseGKPwd)
    {
		m_byIsGkUseRRQPwd = byIsUseGKPwd;
    }
    
    u8	GetGkRRQUsePwdFlag() const
    {
        return m_byIsGkUseRRQPwd;
	}
	
    void SetGkRRQPassword(LPCSTR lpszAlias)
    {
        if(lpszAlias != NULL)
            strncpy(m_achRRQPassword, lpszAlias, GK_RRQ_PWD_LEN);
    }
    
    LPCSTR GetGkRRQPassword() const
    {
        return m_achRRQPassword;
	}

	void SetMcuGeneralCfg(TMcuGeneralCfg *ptGenralCfg)
	{
		if( ptGenralCfg != NULL)
		{
			memcpy(this,ptGenralCfg,sizeof(*ptGenralCfg));
		}
	}

	void SetDMZInfo ( TProxyDMZInfo *ptDMz)
	{
		if(ptDMz != NULL)
		{
			memcpy(&m_tProxyDMZInfo,ptDMz,sizeof(m_tProxyDMZInfo));
		}
	}

	TProxyDMZInfo GetDMZInfo ( void )
	{
		return m_tProxyDMZInfo;
	}

	void Print( void )
	{
		TMcuGeneralCfg::Print();
		StaticLog("Is GK RRQ Use Pwd:%d\n",m_byIsGkUseRRQPwd);
		StaticLog("GK RRQ password:%s\n",m_achRRQPassword);
		StaticLog("IsUseDMZ:%d\n",m_tProxyDMZInfo.IsUseDMZ());
		StaticLog("DMZ IP:0x%x\n",m_tProxyDMZInfo.GetDMZIpAddr());
	}

public:
	u8	 m_byIsGkUseRRQPwd;					//是否启用GK密码RRQ注册
	s8	 m_achRRQPassword[GK_RRQ_PWD_LEN];	//GK RRQ密钥
	TProxyDMZInfo m_tProxyDMZInfo;
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//每个ip信息
struct TNetCfg:public TIpRouteCfg
{
protected:
    s8 m_achName[MAXLEN_NOTES];		//备注信息

public:
	TNetCfg()
    {
        Clear();
    }

	void Clear()
	{
		memset(this, 0, sizeof(*this));
	}

	void SetNotes( LPCSTR lpzNotes )
	{
		if(lpzNotes != NULL )
		{
            memset( m_achName, 0, sizeof(m_achName));
			strncpy( m_achName, lpzNotes, MAXLEN_NOTES -1 );
		}
	}
	
	const s8 *GetNotes(void)
	{
		return m_achName;
	}

	void Print(void)
	{
		u32 dwIp = GetIpAddr();
		if(dwIp != 0)
		{
			u32 dwMask = GetIpMask();
			u32 dwGW = GetGwIp();
			s8	achIp[32] = {0};
			sprintf(achIp, "%d.%d.%d.%d%c", (dwIp>>24)&0xFF, (dwIp>>16)&0xFF, (dwIp>>8)&0xFF, dwIp&0xFF, 0);
			s8	achMask[32] = {0};
			sprintf(achMask, "%d.%d.%d.%d%c", (dwMask>>24)&0xFF, (dwMask>>16)&0xFF, (dwMask>>8)&0xFF, dwMask&0xFF, 0);
			s8	achGw[32] = {0};
			sprintf(achGw, "%d.%d.%d.%d%c", (dwGW>>24)&0xFF, (dwGW>>16)&0xFF, (dwGW>>8)&0xFF, dwGW&0xFF, 0);
			
			OspPrintf( TRUE, FALSE, "Notes:%s\nachIp:%s\nachMask:%s\nachGw:%s\n",m_achName,achIp,achMask,achGw);
		}		
	}
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//每个网口配置
struct TEthCfg
{
protected:
	u32  m_dwDMZIp;								//网络序（DMZ IP配置）
	TNetCfg	 m_atNetCfg[MCU_MAXNUM_ADAPTER_IP];	//IP数目


public:
	TEthCfg()
    {
        Clear();
    }
	
	void Clear()
	{
		 memset(this, 0, sizeof(*this));
	}
	
	BOOL32 IsUseDMZ ( void )
	{
		return (m_dwDMZIp != 0);
	}
	
	//dwIP 主机序
	void SetDMZIpAddr ( u32 dwIp)
	{
		m_dwDMZIp = htonl(dwIp);
	}
	
	//返回主机序
	u32 GetDMZIpAddr ( void )
	{
		return ntohl(m_dwDMZIp);
	}

	void SetNetCfg(u8 byIdx, const TNetCfg &tNetCfg)
	{
		if( byIdx >= MCU_MAXNUM_ADAPTER_IP )
			return;
		
		m_atNetCfg[byIdx] = tNetCfg;
	}
	
	BOOL32 GetNetCfg(u8 byIdx, TNetCfg &tNetCfg) const
	{
		tNetCfg.Clear();
		if( byIdx >= MCU_MAXNUM_ADAPTER_IP )
			return  FALSE;

		tNetCfg = m_atNetCfg[byIdx];		
		return TRUE;
	}	

	void Print(void)
	{
		s8	achDMZIp[32] = {0};
		u32 dwDMZIp = GetDMZIpAddr();
		sprintf(achDMZIp, "%d.%d.%d.%d%c", (dwDMZIp>>24)&0xFF, (dwDMZIp>>16)&0xFF, (dwDMZIp>>8)&0xFF, dwDMZIp&0xFF, 0);
		
		OspPrintf( TRUE, FALSE, "m_dwDMZIp:%s\n",achDMZIp);
		for(u8 byIdx = 0; byIdx < MCU_MAXNUM_ADAPTER_IP; byIdx++)
		{
			if(m_atNetCfg[byIdx].GetIpAddr() == 0)
			{
				continue;
			}

			OspPrintf( TRUE, FALSE, "[Ip-%d]:\n", byIdx);
			m_atNetCfg[byIdx].Print();
		}
	}
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//新的网络配置
struct TNewNetCfg
{
protected:
	enUseModeType	m_enUseModeType;	// 3网口模式 || 备份模式
	enNetworkType	m_enNetworkType;    // 网络地址类型，LAN or WAN 
	BOOL32 m_bIsUseSip;					//是否启用sip
	u8				m_bySipInEthIdx;	// SipIp在哪个网口上
	u32				m_dwSipIpAddr;		// SipIp
	u32				m_dwSipMaskAddr;	// SipMask
	u32				m_dwSipGwAddr;		// SipGw
	TEthCfg	m_atEthCfg[MAXNUM_ETH_INTERFACE];	//网口配置
	
public:
	TNewNetCfg()
    {
        Clear();
    }

	void Clear()
	{
		memset(this, 0, sizeof(*this));
		m_enNetworkType = enLAN;			//默认设置为私网
		m_enUseModeType = MODETYPE_3ETH;	//默认设置为3网口模式
	}

	//模式设置
	void SetUseModeType (enUseModeType enType)
	{
		m_enUseModeType = enType;	
	}
	
	enUseModeType GetUseModeType( void ) const
	{
		return  m_enUseModeType;
	}

	// 设置网络类型
	void SetNetworkType(enNetworkType enType) 
    {

		m_enNetworkType = enType;

	}
	
	// 获取网络类型
    enNetworkType GetNetworkType() const
    {
        return m_enNetworkType;
    }
	
    BOOL32 IsWan() const
    {
        return (m_enNetworkType == enWAN);
    }
	
    BOOL32 IsLan() const
    {
        return (m_enNetworkType == enLAN);
    }
	
	BOOL32 IsLanAndWan() const
    {
        return (m_enNetworkType == enBOTH);
    }

	void SetIsUseSip ( BOOL32 bIsUseSip )
	{
		m_bIsUseSip = bIsUseSip;
	}
	
	BOOL32 IsUseSip ( void ) const
	{
		return m_bIsUseSip;
	}

	void SetSipInEthIdx( u8 bySipInEthIdx )
	{
		m_bySipInEthIdx = bySipInEthIdx;
	}
	
	u8 GetSipInEthIdx ( void ) const
	{
		return m_bySipInEthIdx;
	}	
	//SipIp设置
	void SetSipIpAddr(u32 dwSipIpAddr) { m_dwSipIpAddr = htonl(dwSipIpAddr); }
	u32  GetSipIpAddr()const {return ntohl(m_dwSipIpAddr); }

	void SetSipMaskAddr(u32 dwSipMaskAddr) { m_dwSipMaskAddr = htonl(dwSipMaskAddr); }
	u32  GetSipMaskAddr()const {return ntohl(m_dwSipMaskAddr); }
	
	void SetSipGwAddr(u32 dwSipGwAddr) { m_dwSipGwAddr = htonl(dwSipGwAddr); }
	u32  GetSipGwAddr()const {return ntohl(m_dwSipGwAddr); }

	//网口配置
	void SetEthCfg(u8 byIdx, const TEthCfg &tEthCfg)
    {
		if( byIdx >= MAXNUM_ETH_INTERFACE )
			return;
		m_atEthCfg[byIdx] = tEthCfg;
    }
	
    BOOL32 GetEthCfg(u8 byIdx, TEthCfg &tEthCfg) const
    {
		tEthCfg.Clear();
		if( byIdx >= MAXNUM_ETH_INTERFACE )
			return  FALSE;

		tEthCfg = m_atEthCfg[byIdx];
		return TRUE;
    }

	void Print(void)
	{
		s8	achSipIpAddr[32] = {0};
		u32 dwSipIpAddr = GetSipIpAddr();
		sprintf(achSipIpAddr, "%d.%d.%d.%d%c", (dwSipIpAddr>>24)&0xFF, (dwSipIpAddr>>16)&0xFF, (dwSipIpAddr>>8)&0xFF, dwSipIpAddr&0xFF, 0);

		OspPrintf( TRUE, FALSE, "\n[NewNetCfgInfo]:\nm_enUseModeType:%s\nm_enNetworkType:%s\nm_dwSipIpAddr:%s\n",	\
					(m_enUseModeType == MODETYPE_3ETH ? "MODETYPE_3ETH":"MODETYPE_BAK"),	\
					(m_enNetworkType == enLAN ? "enLAN":(m_enNetworkType == enWAN ? "enWAN":"enBOTH")),	\
					achSipIpAddr);
		for(u8 byIdx = 0; byIdx < MAXNUM_ETH_INTERFACE; byIdx++)
		{
			OspPrintf( TRUE, FALSE, "\n[8KI-Eth%d]:\n", byIdx);
			m_atEthCfg[byIdx].Print();
		}
	}
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//MtAdp注册响应信息(len:148)
struct TMtAdpRegRsp 
{
protected:
	u32  m_dwVcIpAddr;                      //业务IP地址，网络序
	u32  m_dwGkIpAddr;                      //GK IP地址，网络序
	s8   m_achMcuAlias[MAXLEN_ALIAS];       //MCU别名
	u8   m_byMcuNetId;
	u16  m_wHeartBeatInterval;              //检测终端断链时间间隔(sec)
	u8   m_byHeartBeatFailTimes;            //终端断链次数
	u8	 m_byMtAdpMaster;					//是否为主适配 1:是, 0:不是
//	u16  m_wQ931Port;
//	u16  m_wRasPort;
	u16  m_wH225H245Port;                   //H225H245 的建链起始端口
	u16  m_wH225H245MtNum;                  //H225H245 的建链终端数
    u8   m_byGkCharge;                      //是否进行GK计费
	u8	 m_byMcuType;						//Mcu类型, zgc, 2007-04-03
    u8   m_byCascadeAliasType;              //级联时MCU在会议中别名的显示方式
    u8   m_byCascadeAdminLevel;              //级联行政级别(1-4)	
    u8   m_byUseCallingMatch;               //呼叫遵循匹配约定

	u32  m_dwMsSSrc;                        //主备时：用于标注一次系统运行，只要该值变化就说明发生过主备同时重启
    s8   m_achMcuVersion[MAXLEN_ALIAS];     //MCU 版本号，用于初始化协议栈
	u8	 m_byIsGkUseRRQPwd;						//是否启用GKRRQ密码注册
	s8	 m_achRRQPassword[GK_RRQ_PWD_LEN];	    //GK RRQ密钥


public:
	TMtAdpRegRsp()
    {
		m_byCascadeAdminLevel = 4;
        memset(this, 0, sizeof(TMtAdpRegRsp));
    }

	VOID	SetVcIp(u32    dwIp){m_dwVcIpAddr = htonl(dwIp);}
	u32   	GetVcIp() const {return ntohl(m_dwVcIpAddr);}

	VOID	SetGkIp(u32    dwIp){m_dwGkIpAddr = htonl(dwIp);}
	u32   	GetGkIp() const {return ntohl(m_dwGkIpAddr);}

	VOID	SetAlias(LPCSTR lpszAlias)
	{
		if(lpszAlias != NULL)
			strncpy(m_achMcuAlias, lpszAlias, MAXLEN_ALIAS - 1);
	}

	VOID	GetAlias(char *pchBuf, u16  wBufLen) const
	{
		if(pchBuf != NULL)
			strncpy(pchBuf, m_achMcuAlias, wBufLen);
	}

    // guzh [9/3/2007] MCU 版本号，填充H323Config
    VOID	SetMcuVersion(LPCSTR lpszAlias)
    {
        if(lpszAlias != NULL)
            strncpy(m_achMcuVersion, lpszAlias, MAXLEN_ALIAS - 1);
    }
    
    LPCSTR	GetMcuVersion() const
    {
        return m_achMcuVersion;
	}
	
    void   SetMcuNetId(u8 byMcuId)	{ m_byMcuNetId = byMcuId; }	
    u8     GetMcuNetId() const	{ return m_byMcuNetId; }    
	
	void   SetHeartBeatInterval( u16 wHeartBeatInterval ){ m_wHeartBeatInterval = htons(wHeartBeatInterval); } 
	u16     GetHeartBeatInterval( void ) const { return ntohs(m_wHeartBeatInterval); }
	void   SetHeartBeatFailTimes(u8 byHeartBeatFailTimes){ m_byHeartBeatFailTimes = byHeartBeatFailTimes; } 
	u8     GetHeartBeatFailTimes( void ) const { return m_byHeartBeatFailTimes; }
//	void   SetQ931Port(u16 wQ931Port){ m_wQ931Port = htons(wQ931Port);} 
//	u16    GetQ931Port( void ) const { return ntohs(m_wQ931Port); }
//	void   SetRasPort(u16 wRasPort){ m_wRasPort = htons(wRasPort);} 
//	u16    GetRasPort( void ) const { return ntohs(m_wRasPort); }
	void   SetH225H245Port(u16 wH225H245Port){ m_wH225H245Port = htons(wH225H245Port);} 
	u16    GetH225H245Port( void ) const { return ntohs(m_wH225H245Port); }
	void   SetH225H245MtNum(u16 wH225H245MtNum){ m_wH225H245MtNum = htons(wH225H245MtNum);} 
	u16    GetH225H245MtNum( void ) const { return ntohs(m_wH225H245MtNum); }
    void   SetMtAdpMaster( BOOL32 bMtAdpMaster ){ m_byMtAdpMaster = (bMtAdpMaster ? 1:0);}
    BOOL32 IsMtAdpMaster() const { return (1 == m_byMtAdpMaster) ? TRUE:FALSE;}
    void   SetIsGKCharge(BOOL32 bCharge) { m_byGkCharge = bCharge ? 1 : 0;    }
    BOOL32 GetIsGKCharge( void ) { return m_byGkCharge == 1 ? TRUE : FALSE;   }
	
	//Mcu类型, zgc, 2007-04-03
	void	SetMcuType( u8 byMcuType ) { m_byMcuType = byMcuType; }
	u8		GetMcuType( void ) { return m_byMcuType; }

    //MCU级联别名显示方式
    void    SetCasAliasType(u8 byType) { m_byCascadeAliasType = byType; }
    u8      GetCasAliasType() const {return m_byCascadeAliasType;}

    void   SetAdminLevel(u8 byLevel){ m_byCascadeAdminLevel = byLevel;}
    u8     GetAdminLevel() const {return m_byCascadeAdminLevel; }

    void    SetUseCallingMatch(u8 byUseMatch) { m_byUseCallingMatch = byUseMatch; }
    u8      GetUseCallingMatch( void ) const { return m_byUseCallingMatch;    }

	void SetMSSsrc(u32 dwSSrc)
    {
        m_dwMsSSrc = htonl(dwSSrc);
    }
    u32 GetMSSsrc(void) const
    {
        return ntohl(m_dwMsSSrc);
    }

	void SetGkRRQUsePwdFlag( u8 byIsUseGKPwd)
    {
		m_byIsGkUseRRQPwd = byIsUseGKPwd;
    }
    
    u8	GetGkRRQUsePwdFlag() const
    {
        return m_byIsGkUseRRQPwd;
	}

    VOID	SetGkRRQPassword(LPCSTR lpszAlias)
    {
        if(lpszAlias != NULL)
            strncpy(m_achRRQPassword, lpszAlias, GK_RRQ_PWD_LEN);
    }
    
    LPCSTR	GetGkRRQPassword() const
    {
        return m_achRRQPassword;
	}
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

struct TPlayFileHpAttrib 
{
protected:
    emProfileAttrb m_emFirstVideoHpAttrib;
	emProfileAttrb m_emSecVideoHpAttrib;
	
public:
    TPlayFileHpAttrib(void)
    {
        m_emFirstVideoHpAttrib = emBpAttrb;
		m_emSecVideoHpAttrib = emBpAttrb;
    }
	
    void Reset()
    {
        m_emFirstVideoHpAttrib = emBpAttrb;
		m_emSecVideoHpAttrib = emBpAttrb;
    }
	
    emProfileAttrb GetFirstVideHpAttrib(void) const
    {
		//设置的不是emHpAttrb,都认为是bp
        return (ntohl(m_emFirstVideoHpAttrib) == 1) ? emHpAttrb :emBpAttrb ;
    }
    void SetFirstVideHpAttrib(emProfileAttrb emFirstVidHpAtt)
    {
        m_emFirstVideoHpAttrib = (emProfileAttrb)htonl(emFirstVidHpAtt);
    }
	
	emProfileAttrb GetSecVideHpAttrib(void) const
    {
		//设置的不是emHpAttrb,都认为是bp
        return (ntohl(m_emSecVideoHpAttrib) == 1)?emHpAttrb:emBpAttrb;
    }
    void SetSecVideHpAttrib(emProfileAttrb emSecVidHpAtt)
    {
        m_emSecVideoHpAttrib = (emProfileAttrb)htonl(emSecVidHpAtt);
    }
	
    void Print() const
    {
        StaticLog( "m_emFirstVideoHpAttrib: %d\n", ntohl(m_emFirstVideoHpAttrib));
		StaticLog( "m_emSecVideoHpAttrib: %d\n", ntohl(m_emSecVideoHpAttrib));
    }    
}
#ifndef WIN32
__attribute__ ((packed))
#endif
;

struct TVrsRecChnnlStatus : public TRecChnnlStatus
{
protected:
	u8		m_byVrsMtId;
	//支持别名扩容，扩到129. 129-64+1=66
	char	m_achRecordExName[MAXLEN_RECORD_NAME+2];//记录名
public:
	TVrsRecChnnlStatus(){ Clear(); }
	void Clear(){ memset(this, 0, sizeof(TVrsRecChnnlStatus)); }
	u8 GetVrsMtId() { return m_byVrsMtId; }
	void SetVrsMtId(u8 byVrsMtId) { m_byVrsMtId = byVrsMtId; }
	//获取记录名
	BOOL32 GetRecordName( char *szAlias, u16 wBufLen )	
	{
		if (wBufLen < KDV_NAME_MAX_LENGTH+1)
		{
			return FALSE;
		}
		strncpy( szAlias, m_achRecordName, sizeof(m_achRecordName)-1 );//不复制结束符
		strncpy( szAlias+sizeof(m_achRecordName)-1, m_achRecordExName, sizeof(m_achRecordExName) );
		return TRUE; 
	}
	//设置记录名
	void SetRecordName( LPCSTR lpszName ) 
	{
		u16 wStrLen = strlen(lpszName);
		strncpy( m_achRecordName, lpszName, sizeof( m_achRecordName ) );//先拷前面45个字符
		m_achRecordName[sizeof( m_achRecordName ) - 1] ='\0';//保证父类字符串正确
		memset(m_achRecordExName, 0, sizeof(m_achRecordExName));
		if (wStrLen >= MAXLEN_RECORD_NAME)
		{
			// 再从第46个字符开始，将剩余字符拷入m_achRecordExName中
			strncpy( m_achRecordExName, lpszName+sizeof( m_achRecordName )-1, min(wStrLen-sizeof( m_achRecordName )+1, sizeof(m_achRecordExName)-1));
		}
	}
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

struct TPlayEqpAttrib : public TPlayFileAttrib
{
	/*请求类型*/
	enum EPlayReqType
	{
		TYPE_NOREQ		= 0,
		TYPE_FILELIST	= 1,
		TYPE_STARTPLAY	= 2
	};
protected:
    u8 m_byIsDStreamPlay;
	u8 m_byAudTrackNum;
	s8 m_szRecName[255];
	u8 m_byNackTryNum;
	TEqp m_tEqp;
	u8 m_byReqType;//请求消息类型(0无请求/1文件请求/2开启请求)
	u32 m_dwGroupId;//分组id
	u32 m_dwPageNo;//第几页
	u32 m_dwListNum;//单页个数
	u32 m_dwFileId;//播放文件的文件ID,用于
	//s8 m_szVrsPlayData[135];//保存开启放像相关数据，vrs实体上线后发消息用，不做内容识别与修改,目前内容128+u32=132
	//u16 m_wVrsPlayDataLen;
	u16 m_wMStremBR;//记录主流码率，暂停放像时发Flowcontrol 0，恢复时需依赖此信息
	u8  m_byListRecordSrcSsnId;//记录当前请求文件列表的
	
public:
    TPlayEqpAttrib(void)
    {
        m_byIsDStreamPlay = 0;
		m_byAudTrackNum = 0;
		memset( m_szRecName,0,sizeof(m_szRecName) );
		m_byNackTryNum = 0;
		m_tEqp.SetNull();
		m_byReqType = 0;
		m_dwGroupId = 0;
		m_dwPageNo = 0;
		m_dwListNum = 0;
		m_dwFileId = 0;
		m_byListRecordSrcSsnId = 0;
    }
	
    void Reset()
    {
        m_byFileType = RECFILE_NORMAL;
        m_byAudioType = MEDIA_TYPE_NULL;
        m_byVideoType = MEDIA_TYPE_NULL;
        m_byDVideoType = MEDIA_TYPE_NULL;
        m_byIsDStreamPlay = 0;
		m_byAudTrackNum = 0;
		memset( m_szRecName,0,sizeof(m_szRecName) );
		m_byNackTryNum = 0;
		m_tEqp.SetNull();
		m_byReqType = 0;
		m_dwGroupId = 0;
		m_dwPageNo = 0;
		m_dwListNum = 0;
		m_dwFileId = 0;
		m_byListRecordSrcSsnId = 0;
    }
	
    BOOL32 IsDStreamPlay(void) const
    {
        return (m_byIsDStreamPlay == 1) ? TRUE : FALSE;
    }
    void SetDStreamPlay(u8 byIsDStreamPlay)
    {
        m_byIsDStreamPlay = byIsDStreamPlay;
    }

	u8 GetAudTrackNum(void) const
    {
        return m_byAudTrackNum;
    }
    void SetAudTrackNum(u8 byAudTrackNum)
    {
        m_byAudTrackNum = byAudTrackNum;
    }

	s8* GetRecName(void) 
    {
        return &m_szRecName[0];
    }
    void SetRecName(s8* pRecName)
    {
        u16 wStrLen = strlen(pRecName);
		if( wStrLen <= 255 )
		{
			strncpy( m_szRecName,pRecName,255 - 1 );
		}
    }
	
	u8 GetNackTryNum(void) const
    {
        return m_byNackTryNum;
    }

    void SetNackTryNum(u8 byNackTryNum)
    {
        m_byNackTryNum = byNackTryNum;
    }
	
	u8 GetListRecordSrcSsnId(void) const
    {
        return m_byListRecordSrcSsnId;
    }
	
    void SetListRecordSrcSsnId(u8 byListRecordSrcSsnId)
    {
        m_byListRecordSrcSsnId = byListRecordSrcSsnId;
    }

	TEqp GetEqp(void) 
    {
        return m_tEqp;
    }

    void SetEqp(TEqp tEqp)
    {
        m_tEqp = tEqp;
    }
	
	u8 GetReqType(void) const
    {
        return m_byReqType;
    }
    void SetReqType(u8 byReqType)
    {
        m_byReqType = byReqType;
    }
	
	u32 GetGroupId(void) const
    {
        return ntohl(m_dwGroupId);
    }
    void SetGroupId(u32 dwGroupId)
    {
        m_dwGroupId = htonl(dwGroupId);
    }
	
	u32 GetPageNo(void) const
    {
        return ntohl(m_dwPageNo);
    }
    void SetPageNo(u32 dwPageNo)
    {
        m_dwPageNo = htonl(dwPageNo);
    }
	
	u32 GetListNum(void) const
    {
        return ntohl(m_dwListNum);
    }
    void SetListNum(u32 dwListNum)
    {
        m_dwListNum = htonl(dwListNum);
    }
	
	u32 GetFileId(void) const
    {
        return ntohl(m_dwFileId);
    }
    void SetFileId(u32 dwFileId)
    {
        m_dwFileId = htonl(dwFileId);
    }
	
	u16 GetMStremBR(void) const
    {
        return ntohs(m_wMStremBR);
    }
    void SetMStremBR(u16 wMStremBR)
    {
        m_wMStremBR = htons(wMStremBR);
    }
	
    void Print() const
    {
        TPlayFileAttrib::Print();
        StaticLog( "m_byIsDStreamPlay: %d\n", m_byIsDStreamPlay);
		StaticLog( "m_byAudTrackNum: %d\n", m_byAudTrackNum);
		StaticLog( "m_szRecName: %s\n", m_szRecName);
		StaticLog( "m_byNackTryNum: %d\n", m_byNackTryNum);
		StaticLog( "m_tEqp: %d.%d\n", m_tEqp.GetMcuId(),m_tEqp.GetMtId() );
		StaticLog( "m_byReqType: %d\n", m_byReqType);
		StaticLog( "m_dwGroupId: %d\n", GetGroupId());
		StaticLog( "m_dwPageNo: %d\n", GetPageNo());
		StaticLog( "m_dwListNum: %d\n", GetListNum());
		StaticLog( "m_dwFileId: %d\n", GetFileId());
		StaticLog( "m_wMStremBR: %d\n", GetMStremBR());
    }    
}
#ifndef WIN32
__attribute__ ((packed))
#endif
;



//定义画面合成后适配列表 (len:35)
struct TKDVVMPOutParam
{
protected:
	TVideoStreamCap m_atVmpOutMember[MAXNUM_MPU2_OUTCHNNL];
public:
	void Initialize()
	{
		for (u8 byIdx=0;byIdx<MAXNUM_MPU2_OUTCHNNL;byIdx++)
		{
			m_atVmpOutMember[byIdx].Clear();
		}
	}
	TKDVVMPOutParam()
	{
		Initialize();
	}
	//返回指定通道的能力
	TVideoStreamCap GetVmpOutCapIdx(u8 byIdx) const
	{
		return m_atVmpOutMember[byIdx];
	}
	
	//返回后适配通道个数
	u8 GetVmpOutCount() const
	{
		u8 byCount = 0;
		for (; byCount<MAXNUM_MPU2_OUTCHNNL; byCount++)
		{
			if (MEDIA_TYPE_NULL == m_atVmpOutMember[byCount].GetMediaType())
			{
				break;
			}
		}
		return byCount;
	}

	/*==============================================================================
	函数名    :  GetCorrectChnnlByStrCap
	功能      :  根据能力获得最合适的mpu2后适配通道号(等于或小于且最接近此能力的通道号)针对hdu
	算法实现  :  
	参数说明  :  TVideoStreamCap &tStrCap	[in]
				 u8 byOnlyAttrb	[in] 限定只查找BP或HP类型的通道
	返回值说明:  u8 通道号
	-------------------------------------------------------------------------------
	修改记录  :  
	日  期       版本          修改人          走读人          修改记录
	2011-12-5				yanghuaizhi
	==============================================================================*/
	u8 GetCorrectChnnlByStrCap(const TVideoStreamCap &tStrCap, u8 byOnlyAttrb = 0xFF) const
	{
		u8 byChnNum = ~0;
		u8 byRes = tStrCap.GetResolution();
		u8 byFps = tStrCap.GetUserDefFrameRate();
		TVideoStreamCap tTmpCap;
		//仅支持H264
		if (MEDIA_TYPE_H264 != tStrCap.GetMediaType())
		{
			return byChnNum;
		}
		// 查找合适能力
		for (u8 byNum=0; byNum<MAXNUM_MPU2_OUTCHNNL; byNum++)
		{
			//结束未找到
			if (MEDIA_TYPE_H264 != m_atVmpOutMember[byNum].GetMediaType())
			{
				//是非264格式的通道
				if (MEDIA_TYPE_NULL != m_atVmpOutMember[byNum].GetMediaType())
				{
					byChnNum = byNum;
				}
				break;
			}
			tTmpCap = m_atVmpOutMember[byNum];
			//跳过码率为0的通道，8kivmp第0路不编码时码率为0
			if (tTmpCap.GetMaxBitRate() == 0)
			{
				continue;
			}
			//只查找HP或BP时,跳过不同格式,支持只匹配BP或HP
			if (0XFF != byOnlyAttrb && byOnlyAttrb != tTmpCap.GetH264ProfileAttrb())
			{
				continue;
			}
			// 找到一个小于等于此能力的通道
			if ((tTmpCap.IsH264CapEqual(byRes,byFps) && (tStrCap.IsSupportHP() || !tTmpCap.IsSupportHP()))
				|| tTmpCap.IsH264CapLower(byRes,byFps))//找到一个能力小于等于tStrCap的
			{
				byChnNum = byNum;
				break;
			}
		}
		
		return byChnNum;
	}

	/*==============================================================================
	函数名    :  GetVmpOutChnnlByStrCap
	功能      :  根据能力获得对应mpu2后适配通道号
	算法实现  :  
	参数说明  :  TVideoStreamCap &tStrCap	[in]
	返回值说明:  u8 通道号
	-------------------------------------------------------------------------------
	修改记录  :  
	日  期       版本          修改人          走读人          修改记录
	2011-12-5				yanghuaizhi
	==============================================================================*/
	u8 GetVmpOutChnnlByStrCap(const TVideoStreamCap &tStrCap) const
	{
		u8 byChnNum = ~0;
		BOOL32 bIs1080P = FALSE;
		// 1080需要比帧率,其余格式都不需要比较帧率
		if (MEDIA_TYPE_H264 == tStrCap.GetMediaType() &&
			VIDEO_FORMAT_HD1080 == tStrCap.GetResolution())
		{
			bIs1080P = TRUE;
		}
		TVideoStreamCap tTmpCap;
		// 查找相等能力
		for (u8 byNum=0; byNum<MAXNUM_MPU2_OUTCHNNL; byNum++)
		{
			//结束未找到
			if (MEDIA_TYPE_NULL == m_atVmpOutMember[byNum].GetMediaType())
			{
				break;
			}
			tTmpCap = m_atVmpOutMember[byNum]; 
			if (tTmpCap.GetMediaType() == tStrCap.GetMediaType() &&
				tTmpCap.GetResolution() == tStrCap.GetResolution() &&
				(!bIs1080P || tTmpCap.GetUserDefFrameRate() == tStrCap.GetUserDefFrameRate()) &&
				(tTmpCap.IsSupportHP() == tStrCap.IsSupportHP()))
			{
				byChnNum = byNum;
			}
		}

		return byChnNum;
	}
	
	/*==============================================================================
	函数名    :  GetStrCapByVmpOutChnnl
	功能      :  根据mpu2后适配通道号获得对应能力
	算法实现  :  
	参数说明  :  u8 byChnnl		[in]
				 TVideoStreamCap &tStrCap	[out]
	返回值说明:  BOOL32
	-------------------------------------------------------------------------------
	修改记录  :  
	日  期       版本          修改人          走读人          修改记录
	2011-12-5				yanghuaizhi
	==============================================================================*/
	BOOL32 GetStrCapByVmpOutChnnl(u8 byChnnl, TVideoStreamCap &tStrCap)
	{
		if (MEDIA_TYPE_NULL == m_atVmpOutMember[byChnnl].GetMediaType() || MAXNUM_MPU2_OUTCHNNL <= byChnnl)
		{
			LogPrint(LOG_LVL_ERROR, MID_MCU_VMP, "[GetStrCapByVmpOutChnnl]Err,the channel is null.\n");
			return FALSE;
		}

		tStrCap = m_atVmpOutMember[byChnnl];
		return TRUE;
	}
	
	/*==============================================================================
	函数名    :  AddVmpOutMember
	功能      :  添加后适配通道
	算法实现  :  
	参数说明  :  TVideoStreamCap &tStrCap		[in]
			 
	返回值说明:  BOOL32
	-------------------------------------------------------------------------------
	修改记录  :  
	日  期       版本          修改人          走读人          修改记录
	2011-12-5				yanghuaizhi
	==============================================================================*/
	BOOL32 AddVmpOutMember(const TVideoStreamCap &tStrCap)
	{
		// 将能力Idx加入后适配通道列表
		for (u8 byNum=0; byNum<MAXNUM_MPU2_OUTCHNNL; byNum++)
		{
			// 该Idx已经在列表中
			if (m_atVmpOutMember[byNum] == tStrCap)
			{
				LogPrint(LOG_LVL_WARNING, MID_MCU_VMP, "[AddVmpOutMember]Warning, this CapIdx is already exists.\n");
				return FALSE;
			}
			// 找到空位,将其加入
			else if (MEDIA_TYPE_NULL == m_atVmpOutMember[byNum].GetMediaType())
			{
				m_atVmpOutMember[byNum] = tStrCap;
				return TRUE;
			}	
		}
		LogPrint(LOG_LVL_ERROR, MID_MCU_VMP, "[AddVmpOutMember]Err,Add Member failed because channel is full.\n");
		return FALSE;
	}
	
	//更新指定通道能力
	BOOL32 UpdateVmpOutMember(u8 byNum, const TVideoStreamCap &tStrCap) 
	{ 
		m_atVmpOutMember[byNum] = tStrCap; 
		return TRUE;
	}

	// 清除指定通道能力,用于MPU2 BASIC vicp资源不足时,清除1080p60fps
	void ClearOneVmpOutChnnl(u8 byIdx)
	{
		if (byIdx >= MAXNUM_MPU2_OUTCHNNL)
		{
			return;
		}
		// 将后面的成员往迁移
		for (u8 byTmpIdx=byIdx+1; byTmpIdx < MAXNUM_MPU2_OUTCHNNL; byTmpIdx++)
		{
			m_atVmpOutMember[byTmpIdx-1] = m_atVmpOutMember[byTmpIdx];
		}
		// 清除最后一个通道
		m_atVmpOutMember[MAXNUM_MPU2_OUTCHNNL-1].Clear();

	}
	
	void print(void)//FIXME
	{
		u8 byOutNum = GetVmpOutCount();
		StaticLog("Vmp Out Chnnl Num: %u\n", byOutNum);
		StaticLog("---------Vmp Out Channel Members------------\n");
		for(u8 byLoop = 0; byLoop < byOutNum; byLoop++)
		{
			StaticLog("Channel[%u]:", byLoop);
			m_atVmpOutMember[byLoop].Print();
		}
	}
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//vmp各通道信息
struct TVmpChnnlInfo
{	
private:
	u8 m_byHDChnnlNum;		//高清通道被占数目
	u8 m_byMaxNumHdChnnl;	//最大前适配数目
	u8 m_byMaxStyleNum;		//支持最大风格数(即几风格以下无需降分辨率)
	//u8 m_byCifChnnlNum;		//目前不用
	//u8 m_byAllChnnlNum;		//所有被用的通道数，目前不用
	//u8 m_byHdChnnlIdx;		//通道idx
	TChnnlMemberInfo	m_tHdChnnlInfo[MAXNUM_VMP_MEMBER];

public:
	TVmpChnnlInfo(void)
	{
		clear();
	}
	void clear()
	{
		SetMaxNumHdChnnl(0);
		SetMaxStyleNum(0);
		clearHdChnnl();
	}
	void clearHdChnnl() //清除通道信息,不包含最大前适配个数,用于非停止vmp时
	{
		SetHDChnnlNum(0);
		memset(m_tHdChnnlInfo, 0, sizeof(m_tHdChnnlInfo));
	}
	void SetHDChnnlNum(u8 byHDChnnlNum){m_byHDChnnlNum = byHDChnnlNum;}
	u8	 GetHDChnnlNum( void ){ return m_byHDChnnlNum; }
	void SetMaxNumHdChnnl(u8 byMaxNumHdChnnl){m_byMaxNumHdChnnl = byMaxNumHdChnnl;}
	u8	 GetMaxNumHdChnnl( void ){ return m_byMaxNumHdChnnl; }
	void SetMaxStyleNum(u8 byMaxStyleNum){m_byMaxStyleNum = byMaxStyleNum;}
	u8	 GetMaxStyleNum( void ){ return m_byMaxStyleNum; }
	//void SetCifChnnlNum(u8 byCifChnnlNum){m_byCifChnnlNum = byCifChnnlNum;}
	//u8	 GetCifChnnlNum( void ){ return m_byCifChnnlNum; }
	//void SetAllChnnlNum(u8 byAllChnnlNum){m_byAllChnnlNum = byAllChnnlNum;}
	
	void SetHdChnnlInfo(u8 byChnnlIdx, const TChnnlMemberInfo *ptChnnlMemberInfo)
	{
		if (NULL == ptChnnlMemberInfo)
		{
			LogPrint(LOG_LVL_ERROR, MID_MCU_VMP, "[SetHdChnnlInfo]Err:ptChnnlMemberInfo is null.\n");
			return;
		}

		if (m_tHdChnnlInfo[byChnnlIdx].GetMt().IsNull() &&
			!ptChnnlMemberInfo->GetMt().IsNull())
		{
			m_byHDChnnlNum++;
		}
		else if (!m_tHdChnnlInfo[byChnnlIdx].GetMt().IsNull() &&
			ptChnnlMemberInfo->GetMt().IsNull())
		{
			m_byHDChnnlNum--;
		}
		m_tHdChnnlInfo[byChnnlIdx] = *ptChnnlMemberInfo;
	}
	
	/*==============================================================================
	函数名    :  GetHdChnnlInfo
	功能      :  按前适配Idx获得对应适配信息
	算法实现  :  
	参数说明  :  u8 byChnnlIdx		[in]
				 TChnnlMemberInfo *ptChnnlMemberInfo	[out]
	返回值说明:  void
	-------------------------------------------------------------------------------
	修改记录  :  
	日  期       版本          修改人          走读人          修改记录
	2011-12-5				yanghuaizhi
	==============================================================================*/
	void GetHdChnnlInfo(u8 byChnnlIdx, TChnnlMemberInfo *ptChnnlMemberInfo)
	{
		if (NULL == ptChnnlMemberInfo)
		{
			LogPrint(LOG_LVL_ERROR, MID_MCU_VMP, "[GetHdChnnlInfo]Err:ptChnnlMemberInfo is null.\n");
			return;
		}

		u8 byTmpIdx = 0;
		for(u8 byLoop=0; byLoop< MAXNUM_VMP_MEMBER; byLoop++)
		{
			if(!m_tHdChnnlInfo[byLoop].GetMt().IsNull())
			{
				if (byTmpIdx == byChnnlIdx)
				{
					*ptChnnlMemberInfo = m_tHdChnnlInfo[byLoop];
					return;
				}
				byTmpIdx++;
			}
		}
		//找不到设空
		ptChnnlMemberInfo->SetNull();
	}
	
	/*==============================================================================
	函数名    :  GetHdChnnlInfoByPos
	功能      :  按前适配Idx获得对应适配信息
	算法实现  :  
	参数说明  :  u8 byChnnlIdx		[in]
				 TChnnlMemberInfo *ptChnnlMemberInfo	[out]
	返回值说明:  void
	-------------------------------------------------------------------------------
	修改记录  :  
	日  期       版本          修改人          走读人          修改记录
	2011-12-5				yanghuaizhi
	==============================================================================*/
	void GetHdChnnlInfoByPos(u8 byPos, TChnnlMemberInfo *ptChnnlMemberInfo)
	{
		if (NULL == ptChnnlMemberInfo || byPos >= MAXNUM_VMP_MEMBER)
		{
			LogPrint(LOG_LVL_ERROR, MID_MCU_VMP, "[GetHdChnnlInfoByPos]Err:ptChnnlMemberInfo is null or byPos[%d] is wrong.\n",byPos);
			return;
		}

		*ptChnnlMemberInfo = m_tHdChnnlInfo[byPos];
	}
	
	/*找到第一个空闲的通道,没有则返回-1
	s32 FindFreeHdChnnl()
	{
		for(u8 byLoop=0; byLoop< MAXNUM_SVMPB_HDCHNNL; byLoop++)
		{
			if(m_tHdChnnlInfo[byLoop].GetMt().IsNull())
			{
				return byLoop;
			}
		}
		return -1;
	}*/
	/*==============================================================================
	函数名    :  GetChnlCountByMt
	功能      :  获得Mt占用前适配通道个数
	算法实现  :  
	参数说明  :  const TMt &tMt		[in]
			 
	返回值说明:  通道个数
	-------------------------------------------------------------------------------
	修改记录  :  
	日  期       版本          修改人          走读人          修改记录
	2011-12-5				yanghuaizhi
	==============================================================================*/
	u8 GetChnlCountByMt(const TMt &tMt)
	{
		u8 byCount = 0;
		if (tMt.IsNull())
		{
			return byCount;
		}

		for (u8 byLoop=0; byLoop<MAXNUM_VMP_MEMBER; byLoop++)
		{
			//跳过双流跟随通道
			if (m_tHdChnnlInfo[byLoop].IsAttrDstream())
			{
				continue;
			}

			if (m_tHdChnnlInfo[byLoop].GetMt().GetMcuIdx() == tMt.GetMcuIdx() &&
				m_tHdChnnlInfo[byLoop].GetMt().GetMtId() == tMt.GetMtId())
			{
				byCount++;
			}
		}

		return byCount;
	}

	/*==============================================================================
	函数名    :  UpdateInfoToHdChnl
	功能      :  按通道号更新前适配信息
	算法实现  :  
	参数说明  :  TChnnlMemberInfo *ptChnnlMemberInfo [in]
				 u8 byVmpPos [in]
			 
	返回值说明:  BOOL32 正常TRUE 异常FALSE
	-------------------------------------------------------------------------------
	修改记录  :  
	日  期		版本			修改人		走读人		修改记录
	2011-11-29					杨怀志
	==============================================================================*/
	BOOL32 UpdateInfoToHdChnl(TChnnlMemberInfo *ptChnnlMemberInfo, u8 byVmpPos)
	{
		if (NULL == ptChnnlMemberInfo || byVmpPos >= MAXNUM_VMP_MEMBER)
		{
			LogPrint(LOG_LVL_ERROR, MID_MCU_VMP, "[UpdateInfoToHdChnl]Err:ptChnnlMemberInfo is null or byVmpPos[%d] is wrong.\n",byVmpPos);
			return FALSE;
		}

		if (m_byHDChnnlNum >= m_byMaxNumHdChnnl &&
			m_tHdChnnlInfo[byVmpPos].GetMt().IsNull())
		{
			LogPrint(LOG_LVL_ERROR, MID_MCU_VMP, "[UpdateInfoToHdChnl]Err:HdChnl is fool.\n");
			return FALSE;
		}

		if (!m_tHdChnnlInfo[byVmpPos].GetMt().IsNull())//更新
		{
			m_tHdChnnlInfo[byVmpPos] = *ptChnnlMemberInfo;
		} 
		else { //占用该通道
			m_tHdChnnlInfo[byVmpPos] = *ptChnnlMemberInfo;
			m_byHDChnnlNum++;
		}

		return TRUE;
	}

	/*==============================================================================
	函数名    :  ClearChnlByMt
	功能      :  根据终端清除其所在通道,用于特权被取消
	算法实现  :  
	参数说明  :  const TMt &tMt		[in]
	返回值说明:  void
	-------------------------------------------------------------------------------
	修改记录  :  
	日  期		版本			修改人		走读人		修改记录
	2011-11-29					杨怀志
	==============================================================================*/
	void ClearChnlByMt(const TMt &tMt)
	{
		if (tMt.IsNull())
		{
			return;
		}

		for (u8 byLoop=0; byLoop<MAXNUM_VMP_MEMBER; byLoop++)
		{
			// 双流源不支持按终端清前适配通道
			if (m_tHdChnnlInfo[byLoop].IsAttrDstream())
			{
				continue;
			}

			if (m_tHdChnnlInfo[byLoop].GetMt().GetMcuIdx() == tMt.GetMcuIdx() &&
				m_tHdChnnlInfo[byLoop].GetMt().GetMtId() == tMt.GetMtId())
			{
				m_tHdChnnlInfo[byLoop].SetNull();
				m_byHDChnnlNum--;
			}
		}
	}

	/*==============================================================================
	函数名    :  ClearChnlByVmpPos
	功能      :  根据vmp通道号清除其所在前适配通道
	算法实现  :  
	参数说明  :  const TMt &tMt		[in]
	返回值说明:  BOOL32 清除成功TRUE 失败FALSE
	-------------------------------------------------------------------------------
	修改记录  :  
	日  期		版本			修改人		走读人		修改记录
	2011-11-29					杨怀志
	==============================================================================*/
	BOOL32 ClearChnlByVmpPos(u8 byVmpPos)
	{
		if (byVmpPos >= MAXNUM_VMP_MEMBER)
		{
			return FALSE;
		}
		if (m_tHdChnnlInfo[byVmpPos].GetMt().IsNull())
		{
			return FALSE;
		}
		else {
			m_tHdChnnlInfo[byVmpPos].SetNull();
			m_byHDChnnlNum--;
		}
		
		return TRUE;
	}
	
	/*==============================================================================
	函数名    :  GetChnlInfoList
	功能      :  获得按终端划分的前适配信息,同一终端可能占多个前适配通道
	算法实现  :  
	参数说明  :  u8 &byMtNum	[o] 终端个数
				 TChnnlMemberInfo *ptChnlInfo [o] 终端对应前适配信息
				 u8 *abyChnlNo	[o] 对应终端占用前适配通道个数
	返回值说明:  void
	-------------------------------------------------------------------------------
	修改记录  :  
	日  期		版本			修改人		走读人		修改记录
	2011-12-13					杨怀志
	==============================================================================*/
	void GetChnlInfoList(u8 &byMtNum,u8 *abyChnlNo, TChnnlMemberInfo *ptChnlInfo)
	{
		if (NULL == abyChnlNo || NULL == ptChnlInfo)
		{
			LogPrint(LOG_LVL_ERROR, MID_MCU_VMP, "[GetChnlInfoList]Err:abyChnlNo or ptChnlInfo is null.\n");
			return;
		}
		byMtNum = 0;
		TVmpChnnlInfo tTmpChnlInfo = *this;
		for(u8 byPos = 0; byPos < MAXNUM_VMP_MEMBER; byPos++)	//所有占用HD的通道的MT信息
		{
			TMt tTmpMt = m_tHdChnnlInfo[byPos].GetMt();
			if (!tTmpMt.IsNull())
			{
				// 已经被计算过
				if (0 == tTmpChnlInfo.GetChnlCountByMt(tTmpMt))
				{
					continue;
				}
				*ptChnlInfo = m_tHdChnnlInfo[byPos];
				*abyChnlNo = GetChnlCountByMt(tTmpMt);
				ptChnlInfo++;
				abyChnlNo++;
				byMtNum++;
				//缓存信息中清除已统计过的终端
				tTmpChnlInfo.ClearChnlByMt(tTmpMt);
			}
		}
	}
	
	/*==============================================================================
	函数名    :  GetWeakestVmpVipChl
	功能      :  获得最弱vip能力通道
	算法实现  :  
	参数说明  :  
	返回值说明:  
	-------------------------------------------------------------------------------
	修改记录  :  
	日  期       版本          修改人          走读人          修改记录
	2013-07					yanghuaizhi							
	==============================================================================*/
	u8 GetWeakestVmpVipChl()
	{
		u8 byWeakestChl = 0XFF;
		TChnnlMemberInfo tWeakestChlInfo;
		// 找到最弱前适配能力
		for (u8 byLoop=0; byLoop<MAXNUM_VMP_MEMBER; byLoop++)
		{
			if (!m_tHdChnnlInfo[byLoop].IsAttrNull())
			{
				// 选看最弱，找到选看vip即可停止
				if (m_tHdChnnlInfo[byLoop].IsAttrSelected())
				{
					byWeakestChl = byLoop;
					break;
				}

				// 发言人与不可降分辨率vip较量
				if (byWeakestChl >= MAXNUM_VMP_MEMBER)
				{
					tWeakestChlInfo = m_tHdChnnlInfo[byLoop];// 记录第一个，可能是发言人也可能是外厂商Mt
					byWeakestChl = byLoop;
				}
				else
				{
					// 不可降分辨率vip比发言人弱
					if (tWeakestChlInfo.IsAttrSpeaker() && !m_tHdChnnlInfo[byLoop].IsCanAdjResMt())
					{
						tWeakestChlInfo = m_tHdChnnlInfo[byLoop];
						byWeakestChl = byLoop;
					}
				}
			}
		}

		return byWeakestChl;
	}

	/*==============================================================================
	函数名    :  GetReplaceVmpVipChl
	功能      :  根据前适配通道信息，获得比该能力小的可占用通道
	算法实现  :  发言人vip > 不可降分辨率vip > 被选看vip
	参数说明  :  
	返回值说明:  
	-------------------------------------------------------------------------------
	修改记录  :  
	日  期       版本          修改人          走读人          修改记录
	2013-07					yanghuaizhi							
	==============================================================================*/
	void GetReplaceVmpVipChl(const TChnnlMemberInfo &tChlMemInfo, u8 &byChlPos, TChnnlMemberInfo &tReplaceMemInfo)
	{
		byChlPos = 0XFF;
		if (GetMaxNumHdChnnl() == 0 || tChlMemInfo.IsAttrNull())
		{
			return;
		}
		// 选看能力最低，不支持后台抢占
		if (tChlMemInfo.IsAttrSelected())
		{
			return;
		}
		u8 byWeakestChlPos = GetWeakestVmpVipChl();
		if (byWeakestChlPos < MAXNUM_VMP_MEMBER)
		{
			// 若最弱vip是选看则可直接返回
			if (m_tHdChnnlInfo[byWeakestChlPos].IsAttrSelected())
			{
				byChlPos = byWeakestChlPos;
				tReplaceMemInfo = m_tHdChnnlInfo[byWeakestChlPos];
			}
			// 发言人比不可降分辨率vip更强
			else if (tChlMemInfo.IsAttrSpeaker() && !m_tHdChnnlInfo[byWeakestChlPos].IsCanAdjResMt())
			{
				byChlPos = byWeakestChlPos;
				tReplaceMemInfo = m_tHdChnnlInfo[byWeakestChlPos];
			}
			else
			{
				// 找不到比传入能力弱的通道
			}
		}

		return;
	}

	void print(void)//FIXME
	{
		LogPrint(LOG_LVL_DETAIL, MID_MCU_VMP, "MaxStyleNum: %u\nMax HdChnnl Num: %u\nSeized HdChnnl Num: %u\n", 
			m_byMaxStyleNum, m_byMaxNumHdChnnl, m_byHDChnnlNum);
		LogPrint(LOG_LVL_DETAIL, MID_MCU_VMP, "---------Mt info in Hd Channels------------\n");
		for(u8 byLoop = 0; byLoop < MAXNUM_VMP_MEMBER; byLoop++)
		{
			LogPrint(LOG_LVL_DETAIL, MID_MCU_VMP, "Hd Channel[%u]-->Mt.(%u,%u)\n",
				byLoop, m_tHdChnnlInfo[byLoop].GetMt().GetMcuId(), m_tHdChnnlInfo[byLoop].GetMt().GetMtId()  );
		}

	}
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//记录抢占后续所要的操作
struct TVmpAdaptChnSeizeOpr
{
public:
	TMt m_tMt;		//抢占的终端
	u16 m_wEventId;
	TSwitchInfo m_tSwitchInfo;
	u8  m_bySrcSsnId;  //源会话号,用于区分选看是否是MCS拖拽选看,MCS拖拽界面需显示小眼睛
public:
	TVmpAdaptChnSeizeOpr(void)
	{
		Clear();
	}
	void Clear( void )
	{
		memset(this, 0, sizeof(TVmpAdaptChnSeizeOpr));
	}
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;


struct TSwitchChannelExt:public TSwitchChannel
{
    TSwitchChannelExt()
    {
        Clear( );
    }
protected:
	
	u32  m_dwMappedIP;        //将要发送的目标IP地址对应的源作假ip(网络序)
	u16  m_wMappedPort;       //将要发送的目标端口的源作假port (机器序)
	
public:
	
	void Clear( ){ memset( this, 0, sizeof(TSwitchChannelExt) ); }
	void SetMapIp(u32  dwMappedIp){m_dwMappedIP = htonl(dwMappedIp); }
	u32  GetMapIp()const { return ntohl(m_dwMappedIP); }
	void SetMapPort(u16 wMappedPort){m_wMappedPort = htons(wMappedPort);}
	u16  GetMapPort()const{return ntohs(m_wMappedPort);}
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//发哑音包结构
struct TSendMutePack
{
	u32 m_dwDstIp;//要发哑音包的目的ip
	u16 m_wDstPort;//要发哑音包的目的port
	u8  m_byConfIdx;//所属会议
	u8  m_byMtId;//所属会议的mtid
	u8  m_byPayloadType;    //要发哑音包的payloadtype
	u8  m_byAudioTrackNum;//声道数
	void Clear()
	{	
		m_dwDstIp = 0;
		m_wDstPort = 0;
		m_byConfIdx = 0xFF;
		m_byMtId = 0;
		m_byPayloadType = MEDIA_TYPE_NULL;
		m_byAudioTrackNum = 1;//默认单声道
	}
	TSendMutePack(){Clear();}
	~TSendMutePack(){Clear();}
	void SetConfIdx(const u8 &byConfIdx)
	{
		m_byConfIdx = byConfIdx;
	}
	u8 GetConfIdx()const
	{
		return m_byConfIdx;
	}
	void SetMtId(const u8 & byMtId)
	{
		m_byMtId = byMtId;
	}
	u8 GetMtId()const
	{
		return m_byMtId;
	}
	void SetDstIp(const u32 &dwDstIp)
	{
		m_dwDstIp = htonl(dwDstIp);
	}
	u32 GetDstIp()const
	{
		return ntohl(m_dwDstIp);
	}
	void SetDstPort(const u16 &wDstport)
	{
		m_wDstPort = htons(wDstport);
	}
	u16 GetDstPort() const
	{
		return ntohs(m_wDstPort);
	}

	void SetPayloadType(const u8 &byPayload)
	{
		m_byPayloadType = byPayload;
	}
	u8  GetPayloadType()const
	{
		return m_byPayloadType;
	} 
	void SetAudioTrackNum(const u8 &byAudioTrackNum)
	{
		m_byAudioTrackNum = byAudioTrackNum;
	}
	u8  GetAudioTrackNum()const
	{
		return m_byAudioTrackNum;
	} 

	BOOL32 IsNull()
	{
		if ( m_dwDstIp == 0 || m_wDstPort == 0 || m_byConfIdx > MAX_CONFIDX || m_byPayloadType == MEDIA_TYPE_NULL )
		{
			return TRUE;
		}
		return FALSE;
	}
	void OspPrint()
	{
		OspPrintf(TRUE,FALSE,"m_dwDstIp:0x%x\n",GetDstIp());
		OspPrintf(TRUE,FALSE,"m_wDstPort:%d\n",GetDstPort());
		OspPrintf(TRUE,FALSE,"m_byConfIdx:%d\n",GetConfIdx());
		OspPrintf(TRUE,FALSE,"m_byMtId:%d\n",GetMtId());
		OspPrintf(TRUE,FALSE,"m_byPayloadType:%d\n",GetPayloadType());
		OspPrintf(TRUE,FALSE,"m_byAudioTrackNum:%d\n",GetAudioTrackNum());
	}
	void Print()
	{
		StaticLog("m_dwDstIp:0x%x\n",GetDstIp());
		StaticLog("m_wDstPort:%d\n",GetDstPort());
		StaticLog("m_byConfIdx:%d\n",GetConfIdx());
		StaticLog("m_byMtId:%d\n",GetMtId());
		StaticLog("m_byPayloadType:%d\n",GetPayloadType());
		StaticLog("m_byAudioTrackNum:%d\n",GetAudioTrackNum());
	}
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;


//用于兼容V4R6B2和8000H封闭的预案别名结构[11/14/2012 chendaiwei]
struct TMtVCSPlanAliasV4R6B2
{
public:
    u8				m_AliasType;               //别名类型
    s8				m_achAlias[32];  //别名字符串
    TTransportAddr	m_tTransportAddr;          //传输地址
	u8				m_byReserve;
public:
	TMtVCSPlanAliasV4R6B2( void ){ memset( this, 0, sizeof(TMtVCSPlanAliasV4R6B2) ); }
	BOOL operator ==( const TMtVCSPlanAliasV4R6B2 & tObj ) const;
	BOOL IsNull( void ) const { if( m_AliasType == 0 )return TRUE; return FALSE; }
	void SetNull( void ){ memset(this, 0, sizeof(TMtVCSPlanAliasV4R6B2)); }
	BOOL IsAliasNull( void ) const { return 0 == strlen(m_achAlias); }
	void SetE164Alias( LPCSTR lpzAlias )
	{
		if( lpzAlias != NULL )
		{
			m_AliasType = mtAliasTypeE164;
            memset( m_achAlias, 0, sizeof(m_achAlias));
			strncpy( m_achAlias, lpzAlias, 32-1 );
		}
	}
	void SetH323Alias( LPCSTR lpzAlias )
	{
		if( lpzAlias != NULL )
		{
			m_AliasType = mtAliasTypeH323ID;
            memset( m_achAlias, 0, sizeof(m_achAlias));
			strncpy( m_achAlias, lpzAlias, 32-1);
		}
	}
	void SetH320Alias( u8 byLayer, u8 bySlot, u8 byChannel )
	{
		m_AliasType = mtAliasTypeH320ID;
		sprintf( m_achAlias, "层%d-槽%d-通道%d%c", byLayer, bySlot, byChannel, 0 );
	}
	BOOL GetH320Alias ( u8 &byLayer, u8 &bySlot, u8 &byChannel )
	{
		BOOL bRet = FALSE;
		byLayer   = 0;
		bySlot    = 0; 
		byChannel = 0; 
		if( mtAliasTypeH320ID != m_AliasType )
		{
			return bRet;
		}

		s8 *pachLayerPrefix   = "层";
		s8 *pachSlotPrefix    = "-槽";
		s8 *pachChannelPrefix = "-通道";
		s8 *pachAlias = m_achAlias;

		if( 0 == memcmp( pachAlias, pachLayerPrefix, strlen("层") ) )
		{
			s32  nPos = 0;
			s32  nMaxCHLen = 3; //一字节的最大字串长度
			s8 achLayer[4] = {0};

			//提取层号
			pachAlias += strlen("层");
			for( nPos = 0; nPos < nMaxCHLen; nPos++ )
			{
				if( '-' == pachAlias[nPos] )
				{
					break;
				}
				achLayer[nPos] = pachAlias[nPos];
			}
			if( 0 == nPos || nPos >= nMaxCHLen )
			{
				return bRet;
			}
			achLayer[nPos+1] = '\0';
			byLayer = atoi(achLayer);

			//提取槽号
			pachAlias += nPos;
			if( 0 == memcmp( pachAlias, pachSlotPrefix, strlen("-槽") ) )
			{
				pachAlias += strlen("-槽");
				for( nPos = 0; nPos < nMaxCHLen; nPos++ )
				{
					if( '-' == pachAlias[nPos] )
					{
						break;
					}
					achLayer[nPos] = pachAlias[nPos];
				}
				if( 0 == nPos || nPos >= nMaxCHLen )
				{
					return bRet;
				}
				achLayer[nPos+1] = '\0';
				bySlot = atoi(achLayer);
			
				//提取通道号
				pachAlias += nPos;
				if( 0 == memcmp( pachAlias, pachChannelPrefix, strlen("-通道") ) )
				{
					pachAlias += strlen("-通道");
					memcpy( achLayer, pachAlias, nMaxCHLen );
					achLayer[nMaxCHLen] = '\0';
					byChannel = atoi(achLayer);
					bRet = TRUE;
				}
			}
		}
		return bRet;
	}
	void Print( void ) const
	{
		if( m_AliasType == 0 )
		{
			StaticLog("null alias" );
		}
		else if( m_AliasType == mtAliasTypeTransportAddress )
		{
			StaticLog("IP: 0x%x:%d", 
				     m_tTransportAddr.GetIpAddr(), m_tTransportAddr.GetPort() );
		}		
		else if( m_AliasType == mtAliasTypeE164 )
		{
			StaticLog("E164: %s", m_achAlias ); 
		}
		else if( m_AliasType == mtAliasTypeH323ID )
		{
			StaticLog("H323ID: %s", m_achAlias ); 
		}
		else if( m_AliasType == mtAliasTypeH320ID )
		{
			StaticLog("H320ID: %s", m_achAlias ); 
		}
        else if( m_AliasType == mtAliasTypeH320Alias )
        {
            StaticLog("H320Alias: %s", m_achAlias );
        }
		else
		{
			StaticLog("Other type alias!" ); 
		}
		StaticLog("\n" );
	}
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

// mcu会议中使用vmp共通信息
struct TVmpCommonAttrb
{
	u8	m_byVmpStyle;	//合成风格
	u8	m_byMemberNum;	//成员数量
	u8	m_byNeedPrs;	//是否需要PRS
	TVMPMemberEx		m_atMtMember[MAXNUM_VMP_MEMBER];		//复合成员
	TMediaEncrypt		m_tVideoEncrypt[MAXNUM_VMP_MEMBER];     //视频加密参数
	TDoublePayload		m_tDoublePayload[MAXNUM_VMP_MEMBER];	//双载荷
	u8					m_abyRcvH264DependInMark[MAXNUM_VMP_MEMBER];//用于8ke/h/i vmp按特殊方式解码设置（针对华为终端）
	TVmpStyleCfgInfo	m_tVmpStyleCfgInfo;		//配置信息
	TVMPExCfgInfo		m_tVmpExCfgInfo;		//扩展配置信息
	
	TVmpCommonAttrb()
	{
		memset(this, 0, sizeof(TVmpCommonAttrb));
	}
	
    void Print(void)
    {
        StaticLog( "VmpStyle:%d, MemberNum:%d, NeedPrs:%d\n",
			m_byVmpStyle, m_byMemberNum, m_byNeedPrs);
        for(u8 byIndex = 0; byIndex < MAXNUM_VMP_MEMBER; byIndex++)
        {
            if(0 != m_atMtMember[byIndex].GetMcuId() && 0 != m_atMtMember[byIndex].GetMtId())
            {
                StaticLog( "McuId:%d, MtId:%d, MemType:%d, MemStatus:%d, RealPayLoad:%d, ActPayLoad:%d, EncrptMode:%d\n", 
					m_atMtMember[byIndex].GetMcuId(), m_atMtMember[byIndex].GetMtId(), 
					m_atMtMember[byIndex].GetMemberType(), m_atMtMember[byIndex].GetMemStatus(),
					m_tDoublePayload[byIndex].GetRealPayLoad(), m_tDoublePayload[byIndex].GetActivePayload(),
					m_tVideoEncrypt[byIndex].GetEncryptMode());
            }            
        }        
    }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

// mpu2外设使用共通能力信息
struct TMPU2CommonAttrb
{
	u8	m_byVmpStyle;	//合成风格
	u8	m_byMemberNum;	//成员数量
	u8	m_byNeedPrs;	//是否需要PRS
	TVMPMemberEx		m_atMtMember[MAXNUM_MPU2VMP_MEMBER];		//复合成员
	TMediaEncrypt		m_tVideoEncrypt[MAXNUM_MPU2VMP_MEMBER];     //视频加密参数
	TDoublePayload		m_tDoublePayload[MAXNUM_MPU2VMP_MEMBER];	//双载荷
	TVmpStyleCfgInfo	m_tVmpStyleCfgInfo;		//配置信息
	TVMPExCfgInfo		m_tVmpExCfgInfo;		//扩展配置信息
	
	TMPU2CommonAttrb()
	{
		memset(this, 0, sizeof(TMPU2CommonAttrb));
	}
	
    void Print(void)
    {
        StaticLog( "VmpStyle:%d, MemberNum:%d, NeedPrs:%d\n",
			m_byVmpStyle, m_byMemberNum, m_byNeedPrs);
        for(u8 byIndex = 0; byIndex < MAXNUM_MPU2VMP_MEMBER; byIndex++)
        {
            if(0 != m_atMtMember[byIndex].GetMcuId() && 0 != m_atMtMember[byIndex].GetMtId())
            {
                StaticLog( "McuId:%d, MtId:%d, MemType:%d, MemStatus:%d, RealPayLoad:%d, ActPayLoad:%d, EncrptMode:%d\n", 
					m_atMtMember[byIndex].GetMcuId(), m_atMtMember[byIndex].GetMtId(), 
					m_atMtMember[byIndex].GetMemberType(), m_atMtMember[byIndex].GetMemStatus(),
					m_tDoublePayload[byIndex].GetRealPayLoad(), m_tDoublePayload[byIndex].GetActivePayload(),
					m_tVideoEncrypt[byIndex].GetEncryptMode());
            }            
        }        
    }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

/* 画面合成成员结构(Mcu -- Vmp) 8kh/8ki下参重构准备
struct TVMPMember128Ex : public TVMPMember
{
public:
	void  SetMember( TVMPMember& tMember )
	{
		memcpy(this, &tMember, sizeof(tMember));
	}
	
    BOOL32  SetMbAlias( u8 byAliasLen, const s8 *pAlias ) 
    { 
        if( NULL == pAlias )
        {
            return FALSE;
        }
        memset(m_achMbAlias, 0, sizeof(m_achMbAlias));
        memcpy(m_achMbAlias, pAlias, min(byAliasLen, MAXLEN_ALIAS-1));
        m_achMbAlias[MAXLEN_ALIAS-1] = '\0';
        return TRUE; 
    }
	const s8* GetMbAlias(void) { return m_achMbAlias; }
	
protected:
	s8   m_achMbAlias[MAXLEN_ALIAS];
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

// 8ke8kh8ki外设使用共通能力信息,一体机不需要考虑兼容
struct T8KEVmpCommonAttrb
{
	u8	m_byVmpStyle;	//合成风格
	u8	m_byMemberNum;	//成员数量
	u8	m_byNeedPrs;	//是否需要PRS
	TVMPMember128Ex		m_atMtMember[MAXNUM_SDVMP_MEMBER];		//复合成员,别名长128
	TMediaEncrypt		m_tVideoEncrypt[MAXNUM_SDVMP_MEMBER];     //视频加密参数
	TDoublePayload		m_tDoublePayload[MAXNUM_SDVMP_MEMBER];	//双载荷
	u8					m_abyRcvH264DependInMark[MAXNUM_SDVMP_MEMBER];//用于8ke/h/i vmp按特殊方式解码设置（针对华为终端）
	TVmpStyleCfgInfo	m_tVmpStyleCfgInfo;		//配置信息
	TVMPExCfgInfo		m_tVmpExCfgInfo;		//扩展配置信息
	
	T8KEVmpCommonAttrb()
	{
		memset(this, 0, sizeof(T8KEVmpCommonAttrb));
	}
	
    void Print(void)
    {
        StaticLog( "VmpStyle:%d, MemberNum:%d, NeedPrs:%d\n",
			m_byVmpStyle, m_byMemberNum, m_byNeedPrs);
        for(u8 byIndex = 0; byIndex < MAXNUM_SDVMP_MEMBER; byIndex++)
        {
            if(0 != m_atMtMember[byIndex].GetMcuId() && 0 != m_atMtMember[byIndex].GetMtId())
            {
                StaticLog( "McuId:%d, MtId:%d, MemType:%d, MemStatus:%d, RealPayLoad:%d, ActPayLoad:%d, EncrptMode:%d\n", 
					m_atMtMember[byIndex].GetMcuId(), m_atMtMember[byIndex].GetMtId(), 
					m_atMtMember[byIndex].GetMemberType(), m_atMtMember[byIndex].GetMemStatus(),
					m_tDoublePayload[byIndex].GetRealPayLoad(), m_tDoublePayload[byIndex].GetActivePayload(),
					m_tVideoEncrypt[byIndex].GetEncryptMode());
            }            
        }        
    }
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;
*/

// Vmp追加信息的类型 [7/3/2013 liaokang]
enum emVmpCommType
{
    emVmpStart = (u8)0,
    emVmpMbAlias,          // 成员别名
};

//音频能力信息
struct TAudioCapInfo
{
private:
	TAudioTypeDesc m_tAudioTypeDesc;
	u8             m_byActivePayload;
	u32            reserved;
public:
	TAudioCapInfo( void )
	{
		Clear();
	}
	void Clear( void )
	{
		m_tAudioTypeDesc.Clear();
		m_byActivePayload = 0;
	}
	void SetAudioMediaType( u8 byAudioType)
	{
		m_tAudioTypeDesc.SetAudioMediaType(byAudioType);
	}
	void SetAudioTrackNum( u8 byAudioTrackNum)
	{
		m_tAudioTypeDesc.SetAudioTrackNum(byAudioTrackNum);
	}
	void SetActivePayLoad( u8 byActivePayload)
	{
		m_byActivePayload = byActivePayload;
	}
	u8 GetAudioMediaType( void )
	{
		return m_tAudioTypeDesc.GetAudioMediaType();
	}
	u8 GetAudioTrackNum( void )
	{
		return m_tAudioTypeDesc.GetAudioTrackNum();
	}
	u8 GetActivePayLoad( void )
	{
		return m_byActivePayload;
	}
}
#ifndef WIN32
__attribute__((packed)) 
#endif
;

//停止打洞所需信息
struct H460StopHoleInfo
{
public:
    u16 wLocalPort;
    u32 dwRemoteIp;
    u16 wRemotePort;    
    
public:
    u16  GetLocalPort(){ return ntohs(wLocalPort); }
    void SetLocalPort( u16 wPort ){ wLocalPort = htons(wPort); }
    
    u32  GetRemoteIp(){ return ntohl(dwRemoteIp); }
    void SetRemoteIp( u32 dwIp ){ dwRemoteIp = htonl(dwIp); }
    
    u16  GetRemotePort(){ return ntohs(wRemotePort); }
    void SetRemotePort( u16 wPort ){ wRemotePort = htons(wPort); }
    
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

#ifdef WIN32
#pragma pack( pop )
#endif

#endif
