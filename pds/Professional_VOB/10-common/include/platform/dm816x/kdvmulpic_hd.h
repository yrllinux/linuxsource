/*****************************************************************************
模块名      : 
文件名      : kdvmulpic_hd.h
相关文件    : 
文件实现功能: 
作者        : 朱允荣
版本        : V1.0  Copyright(C) 2008-2009 KDC, All rights reserved.
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2008/11/12    1.0         朱允荣      创建
******************************************************************************/

#ifndef _KDVMULPIC_HD3_H_
#define _KDVMULPIC_HD3_H_

#include "codecwrapperdef_hd.h"
//////////////////////////////////////////////////////////////////////////
//错误码定义
#define HARD_MULPIC_OK         0
#define HARD_MULPIC_BASE       4000

enum enMPUErr
{
    MulPic_Success = HARD_MULPIC_OK,
		
	MulPic_Err_Base = HARD_MULPIC_BASE,
	MulPic_Error_NoMem,
	MulPic_Error_NODev,
	MulPic_Error_NOCreate,
	MulPic_Error_Param,
	MulPic_Error_CreateAlready,
	MulPic_Error_NoVPLink,
};
//////////////////////////////////////////////////////////////////////////
//画面合成最大通道数
#define MULPIC_MAX_CHNS			25
#define VMP_BASIC_ADAPT_NUM		3
#define VMP_ENHANCED_ADAPT_NUM	10

enum enMulPicType
{
    MULPIC_TYPE_ONE  = 0,	//0 -单画面
    MULPIC_TYPE_VTWO = 1,	//1 -两画面：左右分
    MULPIC_TYPE_THREE   ,	//2 -三画面
    MULPIC_TYPE_FOUR    ,	//3 -四画面
    MULPIC_TYPE_SIX     ,	//4 -六画面
    MULPIC_TYPE_ITWO    ,	//5 -两画面：
    MULPIC_TYPE_SFOUR   ,	//6 -特殊四画面：一大三小
    MULPIC_TYPE_SEVEN   ,	//7 -七画面：三大四小
    MULPIC_TYPE_EIGHT   ,	//8 -八画面
    MULPIC_TYPE_NINE    ,		//9 -九画面
    MULPIC_TYPE_TEN     ,		//10-十画面：二大八小
    MULPIC_TYPE_THIRTEEN,	//11-十三画面
    MULPIC_TYPE_SIXTEEN ,	//12-十六画面
    MULPIC_TYPE_TWENTY ,	//13-二十画面
    MULPIC_TYPE_TWENTYFIVE,	//14-二十五画面  
    MULPIC_TYPE_THREE_2IBR,		// 2个在底部靠右
    MULPIC_TYPE_THREE_2IB,		// 2 个在底部两边
    MULPIC_TYPE_THREE_2IBL,		// 2个在底部靠左
    MULPIC_TYPE_THREE_2ITL,		// 2个在顶部靠左
    
    MULPIC_TYPE_VTHREE,			//垂直等大3画面
    MULPIC_TYPE_FOUR_3IB,		// 3个在底部中央
    MULPIC_TYPE_TEN_MID_LR,	// 10画面，中间左右型
    MULPIC_TYPE_THIRTEEN_MID,	// 13画面，中间型
    MULPIC_TYPE_NUM,
    MULPIC_TYPE_ERROR
};
//////////////////////////////////////////////////////////////////////////
typedef struct tagMultiPicCreate
{
    u32  m_dwDevVersion;	//enBoardType产品版本号
    u32  m_dwMode;			//业务模式，如VMP_BASIC
}TMultiPicCreate;

//状态
typedef struct tagMulPicStatus
{
	BOOL32  bMergeStart;	//是否开始画面合成？
	s32  byType;			//画面合成类型
	u8   byCurChnNum;		//当前参与合成的通道数
	TVideoEncParam tVideoEncParam; //图像编码参数
}TMulPicStatus;

//统计
typedef struct tagMulPicChnStatis
{
	u32  m_dwBitRate;        //码率
	u32  m_dwPackNum;        //包数
	u32  m_dwLosePackNum;    //缺少的包数
    BOOL32 m_bWaitKeyFrame;
}TMulPicChnStatis;

typedef struct tagMulPicColor
{
	u8 RColor;    //R背景
	u8 GColor;    //G背景
	u8 BColor;    //B背景
    u8 byDefault;
}TMulPicColor;

class CMultiPic;
class CHardMulPic
{
public:
	CHardMulPic();
	~CHardMulPic();
public:
    /*函数名： Create
      功    能： 
	  参    数： const TMulCreate& tInit
	  返 回 值：	s32 
	  说    明： */
	s32 Create(const TMultiPicCreate &tInit);


    /*函数名： Destory
      功    能： 
	  返 回 值：s32 
	  说    明： */
    s32 Destory(); 
    
    s32 SetData(s32 nChnl, const TFrameHeader &tFrame);
	s32 SetVidDataCallback(s32 nChnl, FRAMECALLBACK fVidData, void* pContext); /*设置视频数据回调*/
	
	s32 AddChannel(s32 nChnNo);
	s32 RemoveChannel(s32 nChnNo);
	
	s32 SetMulPicType(s32 nNewType);
	/* MPU2中不要是有startmerge来设置编码参数，更改为SetVideoEncParam*/
	s32 StartMerge(TVideoEncParam* ptVideoEncParam, s32 nEncNum);	
	s32 StartMerge();
	s32 SetVideoEncParam(TVideoEncParam* ptVideoEncParam, s32 nEncNum);
	s32 StopMerge();

	s32 ChangeVideoEncParam(s32 nIndex, TVideoEncParam* ptVidEncParam);	//改变一个视频编码参数

	s32 ChangeBitRate(s32 nIndex, u16 wBitRate/*kbps*/);
	//只支持VMP_BASIC或VMP_ENHANCED与VMP_ADP_1080P30DEC互切,三者间不可互切
	s32 ChangeMode(u32 dwMode);
	s32 SetFastUpdata(s32 nIndex);
	
	s32 GetStatus(s32 nChnNo, TMulPicStatus &tMulPicStatus);
	s32 GetDecStatis(s32 nChnNo, TMulPicChnStatis &tMulPicStatis);
	s32 GetEncStatis(s32 nIndex, TMulPicChnStatis &tMulPicStatis);
	/*无码流时小窗口显示策略,目前支持:
		EN_PLAY_LAST(上一帧),
		EN_PLAY_BMP(pbyBmp 指定),
		EN_PLAY_BMP_USR(pbyBmp 指定),
		EN_PLAY_BLACK(黑屏)*/
	s32 SetNoStreamBak(u32 dwBakType ,/*见enplayMode定义*/ u8 *pbyBmp = NULL, u32 dwBmpLen = 0, u32 dwChnlId = MULPIC_MAX_CHNS);

	/*设置画面合成背景颜色tBGDColor,以及边框颜色tSlidelineColor[MULPIC_MAX_CHNS]，
	需要改变边框颜色时，只要改变tSlidelineColor[]对应通道的值即可*/
	s32 SetBGDAndSidelineColor(TMulPicColor tBGDColor,
           TMulPicColor atSlidelineColor[MULPIC_MAX_CHNS], BOOL32 bAddSlideLine = FALSE);

	//可单独设置vip通道边框的颜色，与SetBGDAndSidelineColor函数可以同时使用
	s32 SetVipChnl(TMulPicColor tVipSlideColor,u32 dwChnl);
	s32 SetResizeMode(u32 dwMode);//模式0:加黑边，1:裁边，2:非等比拉伸，默认0
	s32 SetEncResizeMode(u32 dwMode);//模式0:加黑边，1:裁边，2:非等比拉伸，默认0
	/*开始加入台标*/
	s32 StartAddIcon(s32 nChnNo, u8 *pbyBmp, u32 dwBmpLen, u32 dwXPos, u32 dwYPos, u32 dwWidth,
								 u32 dwHeight, TBackBGDColor tBackBGDColor, u32 dwClarity);

	/* 停止加入台标 */
	s32 StopAddIcon(s32 nChnNo);
	/*启停编码器*/
	void StartEnc(u32 dwEncChnlId);
	void StopEnc(u32 dwEncChnlId);
private:
    CMultiPic*  m_pMulPic;
};
/*
s32 HardMPUInit(u32 nMode); //nMode:enInitMode
s32 HardMPUInitDev(u32 nEncTos, u32 nDecTos, u32 nDspNum);
s32 HardMPUDestroy();

//DSP初始化、销毁，MCU用户不需要调用
s32 HardMPUInitDSP();
s32 HardMPUDestroy();
*/
#endif /*_KDVMULPIC_HD_H_*/





















