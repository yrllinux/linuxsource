/*****************************************************************************
    模块名      : EqpAgt
    文件名      : eqpagtcommon.h
    相关文件    : 
    文件实现功能: EqpAgt共用
    作者        : liaokang
    版本        : V4r7  Copyright(C) 2011 KDC, All rights reserved.
-----------------------------------------------------------------------------
    修改记录:
    日  期      版本        修改人          修改内容
    2012/06/18  4.7         liaokang        创建
******************************************************************************/
#ifndef _EQPAGT_COMMON_H_
#define _EQPAGT_COMMON_H_

#include "osp.h"
#include "kdvtype.h"

// 错误码
// 0、成功
#define EQPAGT_SUCCESS                  (u16)0    // 成功
// 1、初始化错误码
#define EQPAGT_INITERR_CREATSEM         (u16)1    // 创建信号量错误
#define EQPAGT_INITERR_CFG              (u16)2    // 配置错误
#define EQPAGT_INITERR_SNMPINIT         (u16)3    // SNMP初始化错误
#define EQPAGT_INITERR_CREATAPP         (u16)4    // 创建线程错误
#define EQPAGT_INITERR_POSTMSG          (u16)5    // 发送消息错误

// 默认文件路径
#ifdef WIN32
#define DIR_CONFIG                      ( LPCSTR )"./conf"	
#endif

#ifdef _LINUX_
#define DIR_CONFIG                      ( LPCSTR )"/usr/etc/config/conf"
#endif

// 常量定义
#define EQPAGTCFGFILENAME               ( LPCSTR )"eqpagtcfg.ini"   // eqpagt配置文件 
#define MAXLEN_EQPAGTCFG_INI            (u32)5120                   // eqpagtcfg.ini 配置文件的最大长度

// 系统状态（NMS依u32解析）
#define EQP_SYSSTATE_UNKNOWN            (u32)0   /* 未知状态 */
#define EQP_SYSSTATE_RUNNING            (u32)1   /* 运行状态 */
#define EQP_SYSSTATE_REBOOT             (u32)2   /* 重起状态 */
#define EQP_SYSSTATE_STANDY             (u32)3   /* 待机状态 */

// 设备操作系统（NMS依u32解析）
#define EQP_OSTYPE_UNKNOWN              (u32)0
#define EQP_OSTYPE_VXWORKS              (u32)1
#define EQP_OSTYPE_LINUX                (u32)2
#define EQP_OSTYPE_WIN32                (u32)3
#define EQP_OSTYPE_VX_RAWDISK           (u32)4    //VX裸分区
#define EQP_OSTYPE_LINUX_RH		        (u32)5

// 设备子类型（NMS依u32解析）
#define EQP_SUBTYPE_UNKNOWN		        (u32)0	//未知 
#define EQP_SUBTYPE_KDV2K		        (u32)1	//KDV2000
#define EQP_SUBTYPE_KDV2KC		        (u32)2	//超然服务器
#define EQP_SUBTYPE_KDV2KE		        (u32)3	//KDV2000E
#define EQPSUBTYPE_KDV2K		        ( LPCSTR )"KDV2K"
#define EQPSUBTYPE_KDV2KC		        ( LPCSTR )"KDV2KC"
#define EQPSUBTYPE_KDV2KE		        ( LPCSTR )"KDV2KE"

#define	MAXLEN_COMMUNITY	            (u8)32	                    // community 最大长度(有效位32)
#define	MAXNUM_EQPAGTINFO_LIST	        (u8)16	                    // EqpAgt信息表最大长度

#define DEF_READCOM                     ( LPCSTR )"public"          // 读共同体 
#define DEF_WRITECOM	                ( LPCSTR )"kdv123"          // 写共同体
#define DEF_GSPORT                      (u16)161                    // 读取端口(主机序)
#define	DEF_TRAPPORT		            (u16)162	                // 发Trap端口(主机序)

// EqpAgt信息
struct TEqpAgtInfo
{
protected:
    u32  m_dwTrapIp;								// 接受Trap的网管服务器Ip(网络序)
    s8   m_achReadCom[MAXLEN_COMMUNITY+1];		    // 读共同体
    s8   m_achWriteCom[MAXLEN_COMMUNITY+1];         // 写共同体
    u16  m_wGetSetPort;								// 读取端口(主机序)
    u16  m_wSendTrapPort;							// 发Trap端口(主机序)
public:
    TEqpAgtInfo()
    {
        memset(this, 0, sizeof(TEqpAgtInfo)); 
        SetReadCom( DEF_READCOM );
        SetWriteCom( DEF_WRITECOM );
        SetGSPort( DEF_GSPORT );
        SetTrapPort( DEF_TRAPPORT );
    }
    void SetTrapIp(u32 dwTrapIp) { m_dwTrapIp = htonl(dwTrapIp);	}
    u32  GetTrapIp(void) const { return ntohl(m_dwTrapIp);	}
    void SetGSPort(u16 wPort) { m_wGetSetPort = wPort;	}
    u16  GetGSPort(void) const { return m_wGetSetPort;	}
    void SetTrapPort(u16 wPort) { m_wSendTrapPort = wPort;	}
    u16  GetTrapPort(void) const { return m_wSendTrapPort;	}
    
    void SetReadCom( LPCSTR lpszReadCom )
    {
        if ( NULL != lpszReadCom)
        {
            strncpy(m_achReadCom, lpszReadCom, sizeof(m_achReadCom));
            m_achReadCom[sizeof(m_achReadCom)-1] = '\0';
        }
    }
    LPCSTR GetReadCom(void){ return m_achReadCom; }
    
    void SetWriteCom(LPCSTR lpszWriteCom)
    {
        if ( NULL != lpszWriteCom )
        {
            strncpy(m_achWriteCom, lpszWriteCom, sizeof(m_achWriteCom));
            m_achWriteCom[sizeof(m_achWriteCom)-1] = '\0';
        }
    }
    LPCSTR GetWriteCom(void) { return m_achWriteCom;	}
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

#endif  // _EQPAGT_COMMON_H_