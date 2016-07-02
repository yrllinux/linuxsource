/*****************************************************************************
  模块名      : 
  文件名      : osdmap_ti.h
  相关文件    : 
  文件实现功能: 
  作者        : 张飞
  版本        : V1.0  Copyright(C) 1997-2006 KDC, All rights reserved.
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2007/1/5  1.0         张飞      创建
******************************************************************************/

#ifndef COSDMAP_H
#define COSDMAP_H

#include "codecwrapperdef_ti.h"

class COsdActor;
class CVlynqTrans;


class COsdMap
{
public:
    COsdMap():
	m_pcVlynqTrans(NULL),
	m_pcOsdActor(NULL)
	{};
    virtual ~COsdMap();
	
public:
	//初始化
    BOOL Initialize(u32 dwMapID);

	//显示OSD，成功返回本Osd的ID号；失败返回OSD_ERROR.
	int OsdShow(
		u8 *pbyBmp, 
		u32 dwBmpLen, 
		u32 dwXPos, 
		u32 dwYPos,
		u32 dwWidth,
		u32 dwHeight, 
		u8  byBgdColor, 
		u32 dwClarity,
		BOOL32  bSlide=FALSE,
		u32 dwSlideTimes=1,
		u32 dwSlideStride=10);

	//关闭Osd, 成功返回OSD_OK; 失败返回OSD_ERROR.
    int OsdClose(u32 dwXPos, u32 dwYPos, u32 dwWidth, u32 dwHeight, BOOL32 bSlide=FALSE);	


	//开始本地显示滚动字幕(从下往上滚)
	int StartRunCaption(
		u8 *pbyBmp, 
		u32 dwBmpLen,
		u32 dwTimes,
		u32 dwXPos,
		u32 dwSpeed, 
		u32 dwWidth, 
		u32 dwHeight, 
		u8  byBgdColor, 
		u32 dwClarity, 
		u32 dwChnNum);

	//停止本地显示滚动字幕
	int StopRunCaption(u32 dwChnNum);
	//判断是否在显示滚动信息
	BOOL IsOsdStop();


	int  StartTimeIconShow(u32 dwXPos, u32 dwYPos, u32 dwWidth, u32 dwHeight, 
	            		  TBGColor tBackBGDColor, u32 dwClarity, u32 dwChnNum, u32 dwCapPrt = 2);
	int	 AlarmTimeIconShow(u32 dwChnNum,u32 dwCapPrt,u32 dwAlarmTime);
	int  StopTimeIconShow(u32 dwChnNum, u32 dwCapPrt = 2);
	int  TransRect(u8 byTrans,u32 dwXPos, u32 dwYPos, u32 dwWidth, u32 dwHeight);

	//查询与设置当前mapid
	void SetMapId(u32 dwMapId = CWTI_CPUMASK_ALL, BOOL byReDraw = FALSE);
	static u32 GetMapId();
	
protected:
   COsdActor*   m_pcOsdActor;
   CVlynqTrans* m_pcVlynqTrans;

   //20070930
   static u32      m_dwMapId;
private:

};



#endif
