/*****************************************************************************
   模块名      : MP
   文件名      : bindmp.h
   相关文件    : 
   文件实现功能: MP模块接口定义
   作者        : 胡昌威
   版本        : V0.1  Copyright(C) 2001-2002 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   2003/07/25  0.1         胡昌威        创建
******************************************************************************/
#ifndef __BINDMP_H_
#define __BINDMP_H_

#include "cristruct.h"
typedef struct tagStartMpParm
{
    u32 dwMcuIp;
    u16 wMcuPort;
    tagStartMpParm()
    {
        dwMcuIp  = 0;
        wMcuPort = 0;
    }
}TStartMpParam;

//mpstart声明
BOOL32 mpstart(u32 dwMcuIp,
			   u16 wMcuPort, 
			   TMp *ptMp,
			   TStartMpParam* ptMpParamSlave = NULL); 

void mpstop(); //停止MP

//mpstart声明
BOOL32 mpstart(TMtIpMapTab *ptIpMapTab,
			   s8 *pchV6Ip,
			   u32 dwMcuTcpNode, 
			   u32 dwMcuIp,
			   u16 wMcuPort, 
			   TMp *ptMp,
			   TStartMpParam* ptMpParamSlave = NULL); 

#endif
