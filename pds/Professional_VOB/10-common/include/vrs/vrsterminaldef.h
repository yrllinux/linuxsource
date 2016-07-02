/*****************************************************************************
模块名      : vrs
文件名      : vrsterminaldef.h
相关文件    : 
文件实现功能: 录播主控与其他模块公共定义
作者        : pengjie
版本        : V2R1  Copyright(C) 2013-2015 KDC, All rights reserved.
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2013/11/27  V2R1        zhangli       create
******************************************************************************/
#ifndef _VRSTERMINALDEF_H_
#define _VRSTERMINALDEF_H_

//录放像状态
enum ERPWorkState
{
	RP_STATE_STOP		= 0,	/*未开始*/
	RP_STATE_WORKING	= 1,	/*正在录/放像*/
	RP_STATE_PAUSE		= 2,	/*暂停录/放像*/
		
	RP_STATE_START_WAIT,
	RP_STATE_PAUSE_WAIT
};


#endif // _VRSTERMINALDEF_H_

// END OF FILE