/*****************************************************************************
   模块名      : KDV system
   文件名      : errorid.h
   相关文件    : 
   文件实现功能: KDV错误号统一划分
   作者        : 李屹
   版本        : V0.9  Copyright(C) 2001-2002 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   2002/08/27  0.9         李屹        创建
   2005/09/14  for 4.0     zhangsh     add DCC(move from mcu to system1)
******************************************************************************/
#ifndef _ERROR_ID_H_
#define _ERROR_ID_H_

#define ERR_KDV_BASE     100

/*系统错误（101-1000）*/
#define ERR_KDVSYS_BGN   ERR_KDV_BASE + 1
#define ERR_KDVSYS_END   ERR_KDVSYS_BGN + 899

/*网管客户端错误（10001-12000）*/
#define ERR_CLT_BGN      ERR_KDV_BASE + 9901
#define ERR_CLT_END      ERR_CLT_BGN + 1999

/*会议控制台错误（12001-14000）*/
#define ERR_MC_BGN       ERR_CLT_END + 1
#define ERR_MC_END       ERR_MC_BGN + 1999

/*终端控制台错误（14001-16000）*/
#define ERR_MTC_BGN      ERR_MC_END + 1
#define ERR_MTC_END      ERR_MTC_BGN + 1999

/*网管服务器错误（16001－18000）*/
#define ERR_SVR_BGN      ERR_MTC_END + 1
#define ERR_SVR_END      ERR_SVR_BGN + 1999

/*代理内部错误（18001－20000）*/
#define ERR_AGT_BGN      ERR_SVR_END + 1
#define ERR_AGT_END      ERR_AGT_BGN + 1999

/*MCU错误（20001-22000）*/
#define ERR_MCU_BGN      ERR_AGT_END + 1
#define ERR_MCU_END      ERR_MCU_BGN + 1999

/*MT错误（22001-24000）*/
#define ERR_MT_BGN       ERR_MCU_END + 1
#define ERR_MT_END       ERR_MT_BGN + 1999

/*录像机错误（24001-24500）*/
#define ERR_REC_BGN      ERR_MT_END + 1
#define ERR_REC_END      ERR_REC_BGN + 499

/*混音器错误（24501-25000）*/
#define ERR_MIXER_BGN    ERR_REC_END + 1
#define ERR_MIXER_END    ERR_MIXER_BGN + 499

/*电视墙错误（25001-25500）*/
#define ERR_TVWALL_BGN   ERR_MIXER_END + 1
#define ERR_TVWALL_END   ERR_TVWALL_BGN + 499

/*数据服务器错误（25501-26000）*/
#define ERR_DCS_BGN      ERR_TVWALL_END + 1
#define ERR_DCS_END      ERR_DCS_BGN + 499

/*适配器错误码(260001-26500)*/

#define ERR_BAS_BGN      ERR_DCS_END + 1
#define ERR_BAS_END      ERR_BAS_BGN + 499

/*数据控制台错误（26501-27000）*/
#define ERR_DCC_BGN      ERR_BAS_END + 1
#define ERR_DCC_END      ERR_DCC_BGN + 499

/* Hdu错误码 （27001 - 27500）*/
#define ERR_HDU_BGN      ERR_DCC_END + 1
#define ERR_HDU_END      ERR_HDU_BGN + 499

#endif /* _ERROR_ID_H_ */

/* end of file errorid.h */
