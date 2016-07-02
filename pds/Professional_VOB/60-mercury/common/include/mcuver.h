/*****************************************************************************
   模块名      : kdvversion
   文件名      : mcuver.h
   相关文件    : 
   文件实现功能: 所有模块版本号定义
   作者        : 
   版本        : V3.0  Copyright(C) 2001-2002 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   04/02/23    3.0         胡昌威        创建
******************************************************************************/
#ifndef _MCUVERSION_H_
#define _MCUVERSION_H_

/*
企业MCU版本演进说明：
Ver.Release.Build.Sp.Fix.Time
*/
#define KDV_MCU_PREFIX		"4.7.3.0.0."


/*
总的结构：mn.ss.mm.ii.cc.tttt
     如  Osp30.10.02.01.040318 表示
        模块名称Osp，系统3.0，1.0版本，接口0.2版本，实现0.1版本，04年3月18号发布。
1.	系统版蔽大的升级加一。
3.	接口版本：每修改一次接口加一。
4.	实现版本：接口没变的情况下每提交一次加1(修改BUG，或增加功能)
*/

/* 模块版本号，形式：mn.ss.mm.ii.cc.tttttt */
#define VER_MCU            ( const s8 * )"mcu40.05.13.1559.131126"

#define VER_MP             ( const s8 * )"mp40.01.01.36.130626"
#define VER_MTADP          ( const s8 * )"mtadp40.05.08.252.131112"
						
#define VER_MCUAGENT       ( const s8 * )"mcuagent40.01.06.138.131121"
#define VER_DRIAGENT       ( const s8 * )"driagent40.01.01.09.111102"
#define VER_CRIAGENT       ( const s8 * )"criagent40.01.02.10.130608"
#define VER_DSIAGENT       ( const s8 * )"dsiagent40.01.01.09.070517"
#define VER_MMPAGENT       ( const s8 * )"mmpagent40.01.01.12.081121"
#define VER_VASAGENT       ( const s8 * )"vasagent40.01.01.07.070517"
#define VER_DSCAGENT       ( const s8 * )"dscagent40.01.01.18.080715"
#define VER_IMTAGENT       ( const s8 * )"imtagent40.01.01.09.081121"
#define VER_BASICBOARDAGENT		(const s8 *)"basicboardagent40.01.02.11.130906"

#define VER_BAS            ( const s8 * )"bas40.05.02.22.130531"
#define VER_BASHD          ( const s8 * )"bas40.05.02.55.100925"
#define VER_VMPHD          ( const s8 * )"vmphd40.05.01.06.090603"
#define VER_PRS            ( const s8 * )"prs40.01.01.09.120831"
#define VER_RECORDER       ( const s8 * )"rec40.02.06.122.130902"
#define VER_HDU            ( const s8 * )"hdu40.01.01.08.20130626"
#define VER_MPU            ( const s8 * )"mpu40.01.01.61.130710"
#define VER_ISSLAVE        ( const s8 * )"isslavesys4.7.1.1.1.130608"

#define VER_TW             ( const s8 * )"tvwall40.01.01.12.120531"
#define VER_MIXER          ( const s8 * )"mixer40.01.01.22.080411"
#define VER_EMIXER         ( const s8 * )"emixer40.06.01.09.20130715"
#define VER_VMP            ( const s8 * )"vmp40.01.01.38.130923"
#define VER_MPW            ( const s8 * )"mpw40.01.01.15.080109"

#define VER_MMP            ( const s8 * )"mmp40.01.02.09.120831"
#define VER_MPCRI          ( const s8 * )"mpcri40.01.02.05.070726"
#define VER_MPDRI          ( const s8 * )"mpdri40.01.02.05.070726"
#define VER_BAP            ( const s8 * )"bap40.01.02.05.080726"

#define VER_SNMP           ( const s8 * )"snmp40.01.01.24.091028"
#define VER_HDU2           ( const s8 * )"hdu2.40.07.01.30.20131021"
#define VER_MPU2           ( const s8 * )"mpu2.40.07.01.36.20130917"
#define VER_APU2           ( const s8 * )"apu2.40.07.01.05.20130614"

// 校验用内部开发版本号
#define DEVVER_BAS          (u16)4504
#define DEVVER_HDBAS        (u16)4601
#define DEVVER_PRS          (u16)4501
#define DEVVER_RECORDER     (u16)4507
#define DEVVER_TW           (u16)4503
#define DEVVER_MIXER        (u16)4504
#define DEVVER_EMIXER       (u16)4601
#define DEVVER_VMP          (u16)4504
#define DEVVER_HDVMP        (u16)4501
#define DEVVER_MPW          (u16)4503
#define DEVVER_MTADP        (u16)4511
#define DEVVER_MP           (u16)4504
//[pengguofeng 5/11/2012]IPV6过渡版本号，上面2个不可与此相同 
#define DEVVER_MTADPV6      (u16)4509
#define DEVVER_MPV6         (u16)4504


#define DEVVER_MPU			(u16)4502  // xliang [12/31/2008] 
#define DEVVER_HDU			(u16)4501
#define DEVVER_HDU2			(u16)4701
#define DEVVER_MPU2         (u16)4702
#define DEVVER_APU2			(u16)4701 //TBD 待确定版本号 [1/31/2012 chendaiwei]
#define DEVVER_HDU2_L	    (u16)4702
#define DEVVER_HDU2_S	    (u16)4703

API void mpver();
API void mtadpver();

API void mcuagtver();
API void criagtver();
API void driagtver();
API void dsiagtver();
API void imtagtver();
API void mmpagtver();
API void vasagtver();
API void dscagtver();
API void getcompiletime(s8 *psztime);
#endif






















