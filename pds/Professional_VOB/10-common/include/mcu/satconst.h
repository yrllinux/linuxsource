/*****************************************************************************
   模块名      : 卫星常量定义
   文件名      : satconst.h
   相关文件    : satmtstrc.cpp
   文件功能    : 
   作者        : 张明义
   版本        : V1.0  Copyright(C) 2001-2002 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   2004/11/02  1.0         张明义      创建
******************************************************************************/

#ifndef _SAT_CONST_H_
#define _SAT_CONST_H_

#include "kdvsys.h"

#define MAXLEN_SATMTALIAS     16
#define MAXNUM_SATMT         1024//最大终端数
#define MAXNUM_CONFLIST       16

//音频信道类型
#define SAT_AUDIO_G711A			 1	//g711alaw
#define SAT_AUDIO_G711U			 2	//g911ulaw
#define SAT_AUDIO_G722			3
#define SAT_AUDIO_G7231			 4	//g7231
#define SAT_AUDIO_G728			 5	//g728
#define SAT_AUDIO_G729			 6	//g728
#define SAT_AUDIO_MP3			 7	//mp3
//视频信道类型
#define SAT_VIDEO_H261			 1	//h261
#define SAT_VIDEO_H263			 2	//h263
#define SAT_VIDEO_H264			 3	//h263
#define SAT_VIDEO_MPEG2			 4	//mpeg4
#define SAT_VIDEO_MPEG4			 5	//mpeg4
//数据信道类型
#define DATA_T120			 201	//t120
//分辨率
/*#define TYPE_CIF			 1
#define TYPE_2CIF			 2
#define TYPE_4CIF			 3
*/

#define     TYPE_RES_INVALID				0
#define		TYPE_QCIF				1
#define		TYPE_CIF				2
#define		TYPE_2CIF				3
#define		TYPE_4CIF				4
#define		TYPE_16CIF				5
#define		TYPE_SQCIF				6




//画面合成方式定义--终端必须填的
#define   CONF_VMPMODE_NONE                0  //不进行画面合成
#define   CONF_VMPMODE_CTRL                1  //会控或主席选择成员合成
//画面合成风格定义
#define VMP_TYPE_ONE        1 //单画面
#define VMP_TYPE_VTWO       2 //两画面：左右分
#define VMP_TYPE_THREE      3//三画面
#define VMP_TYPE_FOUR       4//四画面：等大
#define VMP_TYPE_ITWO       5//两画面： 一大一小
#define VMP_TYPE_SFOUR      6//四画面：一大三小
#define VMP_TYPE_SIX        7//六画面

//使能禁止 
#define KDV_AUTO                   0 //根据当前使能处理 
#define KDV_ENABLE		   1 //使能
#define KDV_DISABLE		   2 //禁止
//调焦方式定义
#define FOCUS_AUTO			1		//自动调焦
#define FOCUS_MANUAL			2		//手动调焦
#define FOCUS_STOP			4		//停止调焦

//调焦方向定义
#define	DIRECT_FAR			2		//调远
#define	DIRECT_NEAR			3		//调近

//运动方向定义
#define	DIRECT_UP			1		//上	
#define	DIRECT_DOWN			2		//下
#define	DIRECT_LEFT			3		//左
#define DIRECT_RIGHT			4		//右
#define	DIRECT_UPLEFT			5		//左上
#define	DIRECT_UPRIGHT			6		//左下
#define	DIRECT_DOWNLEFT			7		//右上
#define	DIRECT_DOWNRIGHT		8		//右下
#define DIRECT_HOME			9		//回到正面平视位置
#define	DIRECT_STOP			10		//停止转动

//视野调节方式定义
#define	DIRECT_TELE			1		//调小，镜头拉近
#define	DIRECT_WIDE			2		//调大，镜头拉远

//背光增删
#define BACKLIGHT_ON			1		//加上背光
#define BACKLIGHT_OFF			2		//去掉背光

//画面亮度调节
#define BRIGHT_UP			1		//调亮
#define	BRIGHT_DOWN			2		//调暗
#define BRIGHT_RESET			3		//重置
#define  CAM_PANTILT_CMD		1		//摄像头移动
#define  CAM_FOCUS_CMD			2		//摄像头聚焦
#define  CAM_ZOOM_CMD			3		//摄像头视野
#define  CAM_BRIGHT_CMD			4		//摄像头亮度
#define  CAM_PRESET_SAVE_CMD	5		//预置位保存
#define  CAM_PRESET_MOVE_CMD	6		//预置位移动



#endif //!_SAT_CONST_H_