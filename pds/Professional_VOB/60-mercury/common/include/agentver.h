 /*****************************************************************************
   模块名      : agentver
   文件名      : agentver.h
   相关文件    : 
   文件实现功能: 所有模块版本号定义
   作者        : 
   版本        : V3.5  Copyright(C) 2001-2002 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   04/12/13    3.5         李  博        创建
******************************************************************************/
/*
总的结构：mn.ss.mm.ii.cc.tttt
     如  Osp30.10.02.01.040318 表示
        模块名称Osp，系统3.0，1.0版本，接口0.2版本，实现0.1版本，04年3月18号发布。
1.	系统版本：当前KDV电视会议系统为3.0
2.	模块版本：每一次大的升级加一。
3.	接口版本：每修改一次接口加一。
4.	实现版本：接口没变的情况下每提交一次加1(修改BUG，或增加功能)
*/
#ifndef AGENTVER_H
#define AGENTVER_H

/* 形式：mn.ss.mm.ii.cc.tttt */

#define VER_MCUAGENT       ( const s8 * )"mcuagent35.01.01.11.041216"
#define VER_DRIAGENT       ( const s8 * )"driagent35.01.01.07.041222"
#define VER_CRIAGENT       ( const s8 * )"criagent35.01.01.07.041222"
#define VER_MMPAGENT       ( const s8 * )"mmpagent35.01.01.07.041222"
#define VER_VASAGENT       ( const s8 * )"vasagent35.01.01.07.041222"

#endif //!AGENTVER_H

