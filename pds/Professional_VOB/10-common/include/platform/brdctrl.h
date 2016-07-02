/*
 * brdctrl.h - Linux kernel module for kedacom product board control
 *
 * Copyright (C) 2013 zhangzhuan <zhangzhuan@kedacom.com>
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *   
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301 USA.
 */

#ifndef __BRDCTRL_H
#define __BRDCTRL_H

#ifdef __cplusplus 
extern "C" { 
#endif /* __cplusplus */

/*====================================================================
 *     结构体名     : TBrdCtrlInfo
 *     功能        ：板子信息
 *     成员说明：dwRUNLedStat RUN灯的状态-->
 *				-1 ：RUN灯状态获取错误
 *				 1 ：RUN灯亮
 *				 2 ：RUN灯灭
 *				 4 ：RUN灯慢闪
 *
 *     成员说明：dwALMLedStat ALM灯的状态-->
 *				-1 ：ALM灯状态获取错误
 *				 1 ：ALM灯亮
 *				 2 ：ALM灯灭
 *				 4 ：ALM慢闪
 *	                         ALM灯亮的条件-->
 *				 1 ：CPU温度>85度-------ALM灯常亮
 *				 2 ：风扇转速<100RPM----ALM灯慢闪
 *
 *     成员说明：dwCpuFanRPM CPU风扇转速-->
 *                               RPM值：每分钟风扇转的圈数
 *                               -1   : CPU风扇转速获取错误
 *
 *     成员说明：dwCpuFanRPM CPU温度-->
 *                               摄氏度：CPU温度
 *                               -1    : CPU温度获取错误
 *----------------------------------------------------------------------
 *     修改记录    ：
 *     日  期      版本        修改人        修改内容
 *     10/31/2013  1.0         张专            创建
 *====================================================================*/
typedef struct BrdCtrlInfo{
	int dwRUNLedStat;
	int dwALMLedStat;
	int dwCpuFanRPM;
	int dwCpuTemp;
}TBrdCtrlInfo;

/*====================================================================
 *     函数名      : GetBrdCtrlInfo
 *     功能        ：获取板子信息
 *     输入参数说明：TBrdCtrlInfo结构体指针
 *     返回值说明   ：成功返回0
 *		    ：失败返回-1
 *----------------------------------------------------------------------
 *     修改记录    ：
 *     日  期      版本        修改人        修改内容
 *     10/31/2013  1.0         张专            创建
 *====================================================================*/
int GetBrdCtrlInfo(TBrdCtrlInfo *pBrdCtrlInfo);
 
#ifdef __cplusplus 
}
#endif /* __cplusplus */

#endif
