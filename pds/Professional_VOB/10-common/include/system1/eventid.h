/*****************************************************************************
   模块名      : KDV system
   文件名      : eventid.h
   相关文件    : 
   文件实现功能: KDV事件号统一划分
   作者        : 李屹
   版本        : V0.9  Copyright(C) 2001-2002 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   2002/03/01  0.9         李屹        创建
   2004/03/11  3.0         魏治兵      增加地址簿消息
   2005/12/26  4.0	   zhangsh     增加DCS与MCU通讯消息
******************************************************************************/
#ifndef _EVENT_ID_H_
#define _EVENT_ID_H_

#define EV_KDV_BASE     10000

/*网管客户端内部消息（10001-11000）*/
#define EV_CLT_BGN      EV_KDV_BASE + 1
#define EV_CLT_END      EV_CLT_BGN + 999

/*会议控制台内部消息（11001-12000）*/
#define EV_MC_BGN       EV_CLT_END + 1
#define EV_MC_END       EV_MC_BGN + 999

/*终端控制台内部消息（12001-13000）*/
#define EV_MTC_BGN      EV_MC_END + 1
#define EV_MTC_END      EV_MTC_BGN + 999

/*网管客户端服务器接口消息（13001-14000）*/
#define EV_CLTSVR_BGN   EV_MTC_END + 1
#define EV_CLTSVR_END   EV_CLTSVR_BGN + 999

/*网管服务器内部消息（14001-16000）*/
#define EV_SVR_BGN      EV_CLTSVR_END + 1
#define EV_SVR_END      EV_SVR_BGN + 1999

/*网管服务器代理接口消息（16001-18000）*/
#define EV_SVRAGT_BGN   EV_SVR_END + 1
#define EV_SVRAGT_END   EV_SVRAGT_BGN + 1999

/*代理内部消息（18001-20000）*/
#define EV_AGT_BGN      EV_SVRAGT_END + 1
#define EV_AGT_END      EV_AGT_BGN + 1999

/*代理和业务接口消息（20001-22000）*/
#define EV_AGTSVC_BGN   EV_AGT_END + 1
#define EV_AGTSVC_END   EV_AGTSVC_BGN + 1999

/*会议控制台和MCU业务接口消息（22001-23000）*/
#define EV_MCSVC_BGN    EV_AGTSVC_END + 1
#define EV_MCSVC_END    EV_MCSVC_BGN + 999

/*终端控制台和MT业务接口消息（23001-24000）*/
#define EV_MTCSVC_BGN   EV_MCSVC_END + 1
#define EV_MTCSVC_END   EV_MTCSVC_BGN + 999

/*MCU和MT间业务接口消息（25001-26000）*/
#define EV_MCUMT_BGN    EV_MTCSVC_END + 1001
#define EV_MCUMT_END    EV_MCUMT_BGN + 999

/*MCU和外设间业务接口消息（26001-27000）*/
#define EV_MCUEQP_BGN   EV_MCUMT_END + 1
#define EV_MCUEQP_END   EV_MCUEQP_BGN + 999

/*MCU内部业务消息（28001-29000）*/
#define EV_MCU_BGN      EV_MCUEQP_END + 1001
#define EV_MCU_END      EV_MCU_BGN + 999

/*MT内部业务消息（29001-30000）*/
#define EV_MT_BGN       EV_MCU_END + 1
#define EV_MT_END       EV_MT_BGN + 999

/*RECORDER内部业务消息（30001-30200）*/
#define EV_REC_BGN      EV_MT_END + 1
#define EV_REC_END      EV_REC_BGN + 199

/*混音器内部业务消息（30201-30400）*/
#define EV_MIXER_BGN    EV_REC_END + 1
#define EV_MIXER_END    EV_MIXER_BGN + 199

/*电视墙内部业务消息（30401-30500）*/
#define EV_TVWALL_BGN   EV_MIXER_END + 1
#define EV_TVWALL_END   EV_TVWALL_BGN + 99

/*HDU内部业务消息（30501-30600）*/
#define EV_HDU_BGN   EV_TVWALL_END + 1
#define EV_HDU_END   EV_HDU_BGN + 99

/*数据服务器内部业务消息（30601-30700）*/
#define EV_DCSINTERNAL_BGN      EV_HDU_END + 1
#define EV_DCSINTERNAL_END      EV_DCSINTERNAL_BGN + 99

/*码率适配服务器内部业务消息（30701-30800）*/
#define EV_BAS_BGN      EV_DCSINTERNAL_END + 1
#define EV_BAS_END      EV_BAS_BGN + 99

/*网守消息定义(30801 - 34900)*/
#define GKOSPEVENT_START      EV_BAS_END+1
#define GKOSPEVENT_END        GKOSPEVENT_START + 4099

/*用户管理消息定义(34901 - 35000)*/
#define EV_UM_BGN      GKOSPEVENT_END+1
#define EV_UM_END      EV_UM_BGN + 99

/*地址簿消息定义(35001 - 35100)*/
#define EV_ADDRBOOK_BGN    EV_UM_END+1
#define EV_ADDRBOOK_END    EV_ADDRBOOK_BGN + 99

/*VAS消息定义(35101-36100)*/
#define EV_VAS_BGN    EV_ADDRBOOK_END+1
#define EV_VAS_END    EV_VAS_BGN + 999

/*RAS消息定义(36101-36200)*/
#define EV_RAS_BGN    EV_VAS_END+1
#define EV_RAS_END    EV_RAS_BGN + 99

/*DCServer和DCConsole之间业务接口消息(36201-37000)*/
#define EV_DCSDCC_BGN         EV_RAS_END + 1
#define EV_DCSDCC_END         EV_DCSDCC_BGN + 799

/*MT外设控制消息(37001-37100)*/
#define EV_MTDEV_BGN        EV_DCSDCC_END  + 1
#define EV_MTDEV_END        EV_MTDEV_BGN   + 99

/*DCServer和DCSGuard之间业务接口消息(37101-37200)*/
#define EV_DCSDCSGUARD_BGN         EV_MTDEV_END + 1
#define EV_DCSDCSGUARD_END         EV_DCSDCSGUARD_BGN + 99

/*目录服务的业务消息(37201-37700)*/
#define EV_LDAP_BGN         EV_DCSDCSGUARD_END + 1
#define EV_LDAP_END         EV_LDAP_BGN + 499

//本消息段从卫星版本移植过来，与LDAP冲突，暂不予理睬

/*MCU和Modem之间业务接口消息(37201-37300)*/
#define EV_MODEM_BGN         EV_DCSDCSGUARD_END + 1
#define EV_MODEM_END         EV_DCSDCSGUARD_BGN + 99


/*数据会议终端业务消息(37701-38200)*/
#define EV_DCMT_BGN         EV_LDAP_END + 1
#define EV_DCMT_END         EV_DCMT_BGN + 499


/*VOD业务消息(38201-38300) zhangsh added for vod message (liliqun and hualiang)*/
#define EV_VOD_BGN         EV_DCMT_END + 1
#define EV_VOD_END         EV_VOD_BGN + 99

/*DCSMCU业务消息(38301-38500) xushilin asked*/
#define EV_DCSMCU_BGN         EV_VOD_END + 1
#define EV_DCSMCU_END         EV_DCSMCU_BGN + 199

/*mdsc hdsc watchdog 消息(38501-38600) hualiang add*/
#define EV_DSCCONSOLE_BGN         EV_DCSMCU_END + 1
#define EV_DSCCONSOLE_END         EV_DSCCONSOLE_BGN + 99

/*VMP 消息(38601-38700) xushiling add*/
#define EV_VMP_BGN         EV_DSCCONSOLE_END + 1
#define EV_VMP_END         EV_VMP_BGN + 99

/*PRS 消息(38701-38800) xushiling add*/
#define EV_PRS_BGN         EV_VMP_END + 1
#define EV_PRS_END         EV_PRS_BGN + 99

/*VMPTW 消息(38801-38900) xushiling add*/
#define EV_VMPTW_BGN         EV_PRS_END + 1
#define EV_VMPTW_END         EV_VMPTW_BGN + 99

/*VMPTW 消息(38901-39000) xushiling add*/
#define EV_MP_BGN         EV_VMPTW_END + 1
#define EV_MP_END         EV_MP_BGN + 99

/*Radius计费业务 消息(39001-39100) guozhongjun add */
#define EV_RS_BGN      EV_MP_END + 1
#define EV_RS_END      EV_RS_BGN + 99

//文件升级服务器和设备(MCU/MT/单板)业务消息(39101-39200)  wanghao add 2006/12/28
#define EV_FILESRVDEV_BGN					  EV_RS_END + 1
#define EV_FILESRVDEV_END					  EV_FILESRVDEV_BGN + 99

//文件升级服务器和客户端业务消息(39201-39400)  wanghao add 2006/12/28
#define EV_FILESRVCLIENT_BGN		    EV_FILESRVDEV_END + 1
#define EV_FILESRVCLIENT_END			  EV_FILESRVCLIENT_BGN + 199

//文件升级服务器内部消息(39401-39600)  wanghao add 2007/1/4
#define EV_SUS_BGN		              EV_FILESRVCLIENT_END + 1
#define EV_SUS_END			            EV_SUS_BGN + 199

//mpu业务消息分配 消息(39601-40000) zhangbaoqing add 2008/11/28
#define EV_MPU_BGN              EV_SUS_END + 1
#define EV_MPU_END              EV_MPU_BGN + 399

/* RTSP server 消息分配(40001-40100) xsl add*/
#define EV_RTSP_BGN				EV_MPU_END + 1 
#define EV_RTSP_END				EV_RTSP_BGN + 100

//VCS和MCU通信消息(40101-40300) fxh add          
#define EV_VCSVC_BGN                  EV_RTSP_END  + 1
#define EV_VCSVS_END                  EV_VCSVC_BGN + 199

//8000E WATCHDOG消息(40301- 40600) xueliang add
#define EV_WD8000E_BGN                EV_VCSVS_END + 1
#define EV_WD8000E_END                EV_WD8000E_BGN + 99

//pcdv2和MT之间接口消息(40601-40800)
#define EV_MTPCDV2_BGN              EV_WD8000E_END + 1
#define EV_MTPCDV2_END              EV_MTPCDV2_BGN + 199

//FIXME: 从卫星版本移植过来，与NMC测试消息有冲突，不予理睬

/* MT 与卫星网管通信消息，网管系统由第三方提供不可更改，其消息范围在 50200－502300*/
#define EV_SATMTNMS_BGN             50200
#define EV_SATMTNMS_END             50300

/* MCU与卫星网管通信消息，网管系统由第三方提供不可更改，其消息范围在 50400－50500*/
#define EV_SATMCUNMS_BGN            51100
#define EV_SATMCUNMS_END            51200	

//#define EV_SATMCUNMS_BGN            50400
//#define EV_SATMCUNMS_END            50500	



/* 以下为测试消息 */

#define EV_TEST_KDV_BASE     50000

/*网管客户端测试消息（50001-50500）*/
#define EV_TEST_CLT_BGN      EV_TEST_KDV_BASE + 1
#define EV_TEST_CLT_END      EV_TEST_CLT_BGN + 499

/*会议控制台测试消息（50501-51000）*/
#define EV_TEST_MC_BGN       EV_TEST_CLT_END + 1
#define EV_TEST_MC_END       EV_TEST_MC_BGN + 499

/*终端控制台测试消息（51001-51500）*/
#define EV_TEST_MTC_BGN      EV_TEST_MC_END + 1
#define EV_TEST_MTC_END      EV_TEST_MTC_BGN + 499

/*网管服务器测试消息（51501-52000）*/
#define EV_TEST_SVR_BGN      EV_TEST_MTC_END + 1
#define EV_TEST_SVR_END      EV_TEST_SVR_BGN + 499

/*代理测试消息（52001-52500）*/
#define EV_TEST_AGT_BGN      EV_TEST_SVR_END + 1
#define EV_TEST_AGT_END      EV_TEST_AGT_BGN + 499

/*MCU测试消息（52501-53500）*/
#define EV_TEST_MCU_BGN      EV_TEST_AGT_END + 1
#define EV_TEST_MCU_END      EV_TEST_MCU_BGN + 999

/*MT测试消息（53501-54500）*/
#define EV_TEST_MT_BGN       EV_TEST_MCU_END + 1
#define EV_TEST_MT_END       EV_TEST_MT_BGN + 999

/*RECORDER测试消息（54501-54700）*/
#define EV_TEST_REC_BGN      EV_TEST_MT_END + 1
#define EV_TEST_REC_END      EV_TEST_REC_BGN + 199

/*混音器测试消息（54701-54900）*/
#define EV_TEST_MIXER_BGN    EV_TEST_REC_END + 1
#define EV_TEST_MIXER_END    EV_TEST_MIXER_BGN + 199

/*电视墙测试消息（54901-55100）*/
#define EV_TEST_TVWALL_BGN   EV_TEST_MIXER_END + 1
#define EV_TEST_TVWALL_END   EV_TEST_TVWALL_BGN + 199

/*数据服务器测试消息（55101-55200）*/
#define EV_TEST_DCS_BGN     EV_TEST_TVWALL_END + 1
#define EV_TEST_DCS_END     EV_TEST_DCS_BGN + 99

/*码率适配服务器测试消息（55201-55300）*/
#define EV_TEST_BAS_BGN     EV_TEST_DCS_END + 1
#define EV_TEST_BAS_END     EV_TEST_BAS_BGN + 99

/*GK控制台测试消息（55301-55700）*/
#define EV_TEST_GKC_BGN     EV_TEST_BAS_END + 1
#define EV_TEST_GKC_END     EV_TEST_GKC_BGN + 399

/*数据会议控制台测试消息（55701-56100）*/
#define EV_TEST_DCC_BGN     EV_TEST_GKC_END + 1
#define EV_TEST_DCC_END     EV_TEST_DCC_BGN + 399

/*数据会议终端测试消息（56101-56600）*/
#define EV_TEST_DCMT_BGN     EV_TEST_DCC_END + 1
#define EV_TEST_DCMT_END     EV_TEST_DCMT_BGN + 499

/*开发测试消息（56601－57600）*/
#define EV_TEST_DEV_BGN     EV_TEST_DCMT_END + 1
#define EV_TEST_DEV_END     EV_TEST_DEV_BGN + 999

/*hdu自动检测消息（57601－57700）*/  // by jianglb
#define EV_HDUAUTOTEST_BGN     EV_TEST_DEV_END + 1
#define EV_HDUAUTOTEST_END     EV_HDUAUTOTEST_BGN + 99

/*Mpu 自动检测消息  (57701 - 57800) */
#define EV_MPUAUTOTEST_BGN	   EV_HDUAUTOTEST_END + 1
#define EV_MPUAUTOTEST_END	   EV_MPUAUTOTEST_BGN + 99

// #define EV_GKINTERWORK_BGN     EV_MPUAUTOTEST_END + 1
// #define EV_GKINTERWORK_END     EV_GKINTERWORK_BGN + 99

/* mtctrl 消息*/
#define EV_MTCTRL_BEG 60000
#define EV_MTCTRL_END EV_MTCTRL_BEG + 999


#endif /* _EVENT_ID_H_ */

/* end of file eventid.h */


