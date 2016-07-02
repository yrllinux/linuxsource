/*****************************************************************************
   模块名      : CodecLib
   文件名      : OsdMap.h
   相关文件    : 
   文件实现功能: 硬件Codec 上层API 封装
   作者        : 胡昌威
   版本        : V0.9  Copyright(C) 2001-2002 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人         修改内容
   2003/03/23  0.9         Hu Changwei      创建
******************************************************************************/
#ifndef __COSDMAP_H_
#define __COSDMAP_H_

#include "eqtmap.h"

#define OSD_BUF_LEN          (u32)(1<<20)

/*************OSD CONTROL  0x7D0开始********************/	
#define	H2M_OSD_LABEL                   0x7D1/*OSD 台标，内容为数据长度*/
#define	H2M_OSD_CAPTION                 0x7D2/*OSD字幕，参数说明见接口实现*/	
#define	H2M_OSD_STATE                   0x7D3/*OSD状态标记，内容为数据长度*/	
#define	H2M_OSD_MENU                    0x7D4/*OSD菜单，参数说明见接口实现*/	
#define	H2M_OSD_VOLUME                  0x7D5/*OSD音量标志*/
#define	H2M_OSD_CLOSE                   0x7D7/*OSD关闭，内容为空*/
#define	H2M_OSD_NEED_BUF                0x7D8/*发送OSD Buffer 申请请求，参数为空*/
#define H2M_OSD_OTHERBMP	        0x7D3/*OSD 其它图像处理BMP*/

/*OSD关闭参数定义*/
#define	OSD_LABEL			0
#define	OSD_CAPTION			1
#define	OSD_STATE			2
#define	OSD_MENU			3
#define	OSD_VOLUME			4
#define	OSD_CYCLE			5

//#define OSD_TIMER

#ifdef OSD_TIMER
#ifdef _VXWORKS_
#include "timers.h"
#include "time.h"
#endif
#endif

#define MAX_OSD_NUM          100

#define OSD_OK               0
#define OSD_ERROR            -1

#define OSD_TYPE_STATIC      0
#define OSD_TYPE_SLIDE       1

typedef struct 
{
	BOOL  bValid;
	u8  byType; //类型：0--静态Osd, 1--滚动Osd.
	u32 dwXPos;
	u32 dwYPos;
	u32 dwWidth;
	u32 dwHeight;
}TOsdDesc;

/*Osd参数*/
typedef struct OsdParam
{
	u16 byOsdType;         /*Osd 类型*/
	u8 *pbyHdrBuf;        /*头Buffer*/
	u32 dwHdrLen;         /*头长度*/
	u8 *pbyDataBuf;       /*数据Buffer*/
	u32 dwDataLen;        /*数据长度*/
	u32 dwXPos;           /*X坐标*/
    u32 dwYPos;           /*Y坐标*/
    u32 dwHeight;         /*高度*/
    u32 dwWidth;          /*宽度*/
	u32 dwTansparentValue; /*透明度*/
	u32 dwOtherParam1;     /*其它参数1*/
	u32 dwOtherParam2;     /*其它参数2*/
}TOsdParam;

/*Osd关闭参数*/
typedef struct OsdCloseParam
{
    BOOL bNeedClose;
    u16 wLeftSecond;
}TOsdCloseParam;

class CHardCodec;

#define _TOPAN_
#ifndef _TOPAN_
class COsdMap
{
public:
    COsdMap();
    virtual ~COsdMap();

public:
	//初始化
    u16 Initialize(u8 byMapID);

	//向Map写Osd数据
    u16 WriteOsdDataToMap(WORD byOsdType,/*OSD类型*/
                           u8 *pbyHdrBuf,/*头起始地址*/
                           u32 dwHdrLen,/*头长度*/
                           u8 *pbyDataBuf,/*数据起始地址*/
                           u32 dwDataLen,/*数据长度*/
                           u32 dwXPos,/*X坐标*/
                           u32 dwYPos,/*Y坐标*/
                           u32 dwHeight,/*高度*/
                           u32 dwWidth,/*宽度*/
                           u32 dwTansparentValue,/*透明度*/
                           u32 dwOtherParam1,
                           u32 dwOtherParam2);
	//关闭Osd
    u16 OsdClose(u8 byOsdType);
	
    //Osd双buffer地址由Map发送到主机之后，通过本函数得到
	void OsdBufBasesProc(u32 dwOsdBufBase0, u32 dwOsdBufBase1);

    //friend void GetOsdMsgFunc(u32 dwMsg, u32 dwParam[6],u8 *pbyContext);
    
    #ifdef OSD_TIMER
	//时钟
    friend void Timer(timer_t timerid,int Param);
    #endif

protected:
    u32 m_dwMapBuffer[2];  //Osd Buffer
    u8 m_byCurBufNum;      //当前Buffer编号
    TOsdParam tOsdParam[10]; //Osd参数
    BOOL m_bShow[10];        //Osd是否显示
    BOOL m_bReset;           //是否刚Reset
    int nWaitTime;           //等待次数

    #ifdef OSD_TIMER
    TOsdCloseParam tOsdCloseParam[2];  //Osd关闭参数
    timer_t timerId;                   //时钟ID
    #endif

private:
	CHardCodec *m_pcCodecMap;
   
};

#else     /*define _TOPAN_*/

class COsdMap
{
public:
    COsdMap();
    virtual ~COsdMap();
	
public:
	//初始化
    u16 Initialize(u32 dwMapID);

	//显示OSD，成功返回本Osd的ID号；失败返回OSD_ERROR.
	int OsdShow(u8 *pbyBmp, 
		         u32 dwBmpLen, 
				 u32 dwXPos, 
				 u32 dwYPos, 
				 u32 dwWidth, 
				 u32 dwHeight, 
				 u8  byBgdColor, 
				 u32 dwClarity, 
				 BOOL  bSlide=FALSE,
				 u32 dwSlideTimes=1,
				 u32 dwSlideStride=10,
				 BOOL  bAddLogo=FALSE,  /*是否作码流融合*/
				 u32 dwChnNum=0  /*码流融合的通道号*/);

	//关闭Osd, 成功返回OSD_OK; 失败返回OSD_ERROR.
      int OsdClose(u32 dwXPos, u32 dwYPos, u32 dwWidth, u32 dwHeight, BOOL bSlide=FALSE, BOOL  bAddLogo=FALSE/*是否作码流融合*/, u32 dwChnNum=0  /*码流融合的通道号*/);	
   
    #ifdef OSD_TIMER
	int  StartTimeIconShow(u32 dwXPos, u32 dwYPos, u32 dwWidth, u32 dwHeight, 
	            		  u8  byBgdColor, u32 dwClarity, u32 dwChnNum);
	int  StopTimeIconShow(u32 dwChnNum);
	int  CheckTImeIconProc();
	//时间台标时钟处理函数
#ifdef _VXWORKS_
    friend void TimeIconTimerProcess(timer_t timerid, int Param);   
#endif
    #endif

private:
	TOsdDesc m_atOsdDesc[MAX_OSD_NUM];
    volatile u32 m_dwMapBuffer[2];  //Osd Buffer
    u8 m_byCurBufNum;      //当前Buffer编号
	int nWaitTime;           //等待次数
	
    #ifdef OSD_TIMER
	//时钟ID
#ifdef _VXWORKS_
	timer_t m_timerId;      
#endif
    #endif
	u32   m_dwAddLogoChn;

	CHardCodec *m_pcCodecMap;	
};
#endif // _TOPAN_




#endif





















