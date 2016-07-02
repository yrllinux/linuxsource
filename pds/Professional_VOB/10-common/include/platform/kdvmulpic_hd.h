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

#ifdef _MEDIACTRL_DM816x_

#include "dm816x/kdvmulpic_hd.h"

#else

#ifndef _KDVMULPIC_HD_H_
#define _KDVMULPIC_HD_H_



#include "codecwrapperdef_hd.h"

//错误码定义
#define HARD_MULPIC_OK         0
#define HARD_MULPIC_BASE       4000
#define MAX_VMP_DECCHANNEL       (u32)25
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

//画面合成最大通道数
#define MULPIC_MAX_CHNS        25
enum enMulPicType
{
    MULPIC_TYPE_ONE  = 0,	//0 -单画面 ****暂不支持
    MULPIC_TYPE_VTWO = 1,	//1 -两画面：左右分
    MULPIC_TYPE_THREE   ,	//2 -三画面
    MULPIC_TYPE_FOUR    ,	//3 -四画面
    MULPIC_TYPE_SIX     ,	//4 -六画面
    MULPIC_TYPE_ITWO    ,	//5 -两画面：
    MULPIC_TYPE_SFOUR   ,	//6 -特殊四画面：一大三小
    MULPIC_TYPE_SEVEN   ,	//7 -七画面：三大四小
    MULPIC_TYPE_EIGHT   ,	//8 -八画面
    MULPIC_TYPE_NINE    ,	//9 -九画面
    MULPIC_TYPE_TEN     ,	//10-十画面：二大八小
    MULPIC_TYPE_THIRTEEN,	//11-十三画面
    MULPIC_TYPE_SIXTEEN ,	//12-十六画面
    MULPIC_TYPE_TWENTY ,	//13-二十画面
	MULPIC_TYPE_TWENTYFIVE,	//14-二十五画面
    MULPIC_TYPE_NUM
};


typedef struct tagDspChnl
{
    u32  m_dwDspID;
    u32  m_dwVidEncNum;
    u32  m_dwVidDecNum;
    u32  m_dwAudEncNum;
    u32  m_dwAudDecNum;
}TDspChnl;

typedef struct tagMulCreate
{
    u32  m_dwStartDspID;  //使用的最开始的DSP编号，从0开始。
    u32  m_dwDSPNUM;      //使用的DSP个数 1～4

}TMulCreate;

//状态
typedef struct tagMulPicStatus
{
	BOOL32  bMergeStart; //是否开始画面合成？
	s32  byType; //画面合成类型
	u8   byCurChnNum; //当前参与合成的通道数
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
    u8 byDefualt;
}TMulPicColor;

typedef struct tagTIconRecordInfo
{
	BOOL32    bHasData;      //记录MapHalt前有操作
	u8 *  pbyBmp;        //以下为Osd信息
	u32   dwBmpLen; 
	u32   dwXPos; 
	u32   dwYPos; 
	u32   dwWidth; 
	u32   dwHeight; 
	TMulPicColor    tBackBGDColor; 
	u32   dwClarity; 
}TIconRecordInfo;

class CMulPic;
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
	s32 Create(const TMulCreate& tInit);


    /*函数名： Destory
      功    能： 
	  返 回 值：s32 
	  说    明： */
    s32 Destory(); 
    
    s32 SetData(s32 nChnl, const TFrameHeader& tFrame);
	s32 SetVidDataCallback(s32 nChnl, FRAMECALLBACK fVidData, void* pContext); /*设置视频数据回调*/
	
	s32 AddChannel(s32 nChnNo);
	s32 RemoveChannel(s32 nChnNo);
	
	s32 StartMerge(TVideoEncParam* ptVideoEncParam, s32 nEncNum);
	s32 StopMerge();
	
	s32 GetStatus(s32 nChnNo,TMulPicStatus &tMulPicStatus);
	s32 GetDecStatis(s32 nChnNo, TMulPicChnStatis &tMulPicStatis);
    s32 GetEncStatis(s32 nIndex, TMulPicChnStatis &tMulPicStatis);

	s32 SetMulPicType(s32 nNewType);

	s32 ChangeBitRate(s32 nIndex, u16  wBitRate/*kbps*/);
	s32 SetFastUpdata(s32 nIndex);

	/*设置画面合成背景颜色tBGDColor,以及边框颜色tSlidelineColor[MULPIC_MAX_CHNS]，
	需要改变边框颜色时，只要改变tSlidelineColor[]对应通道的值即可*/
	s32 SetBGDAndSidelineColor(TMulPicColor tBGDColor,
           TMulPicColor atSlidelineColor[MULPIC_MAX_CHNS], BOOL32 bAddSlideLine = FALSE);

	//可单独设置vip通道边框的颜色，与SetBGDAndSidelineColor函数可以同时使用
	s32 SetVipChnl(TMulPicColor tVipSlideColor,u32 dwChnl);

	/*开始加入台标*/
	s32 StartAddIcon(s32 nChnNo, u8 *pbyBmp, u32 dwBmpLen, u32 dwXPos, u32 dwYPos, u32 dwWidth,
								 u32 dwHeight, TBackBGDColor tBackBGDColor, u32 dwClarity);

	/* 停止加入台标 */
	s32 StopAddIcon(s32 nChnNo);

	/*无码流时播放类型, BakType(enNoStreamBack)*/
	s32 SetNoStreamBak(u32 BakType, u8 *pbyBmp = NULL, u32 dwChnlId=MAX_VMP_DECCHANNEL);

	u16 SetResizeMode(u32 dwMode);//模式0:加黑边，1:裁边，2:非等比拉伸，默认0
	u16 SetEncResizeMode(u32 dwMode);//模式0:加黑边，1:裁边，2:非等比拉伸，默认0
	u16 SetVidDropPolicy(u8 byDropPolicy);  //设定丢包处理策略
	
	//设置某路编码通道的编码参数，此处设置的编码宽高要求小于
	//调用StartMerge时设置的宽高
	s32 SetEncChnlParam(TVideoEncParam *ptEncParam, s32 nChannel);  

private:
    CMulPic*  m_pMulPic;
};

s32 HardMPUInit(u32 nMode); //nMode:enInitMode
s32 HardMPUInitDev(u32 nEncTos, u32 nDecTos, u32 nDspNum);
s32 HardMPUDestroy();

//DSP初始化、销毁，MCU用户不需要调用
s32 HardMPUInitDSP(TDspChnl* pChnl);
s32 HardMPUDestroy();
#endif
#endif /*_KDVMULPIC_HD_H_*/





















