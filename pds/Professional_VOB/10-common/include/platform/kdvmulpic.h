 /*****************************************************************************
   模块名      : HardMulPic
   文件名      : KdvMulPic.h
   相关文件    : 
   文件实现功能: 硬件多画面合成器上层API 封装
   作者        : 向飞
   版本        : V3.0  Copyright(C) 2001-2002 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人         修改内容
   2003/12/03  3.0         向飞           创建
******************************************************************************/
#ifndef _KDVMULPIC_H_
#define _KDVMULPIC_H_

#include "codeccommon.h"

//错误码定义
#define HARD_MULPIC_OK         0
#define HARD_MULPIC_ERROR     -1

/*图像编码类型
#define  MEDIA_TYPE_MP4	     (u8)97//MPEG-4
#define  MEDIA_TYPE_H261	 (u8)31//H.261
#define  MEDIA_TYPE_H263	 (u8)34//H.263
*/

//画面合成类型定义
#define MULPIC_TYPE_VTWO       1 //两画面：左右分
#define MULPIC_TYPE_THREE      2//三画面
#define MULPIC_TYPE_FOUR       3//四画面
#define MULPIC_TYPE_SIX        4//六画面
#define MULPIC_TYPE_ONE        5//单画面（分成9份发送）
#define MULPIC_TYPE_ITWO       6//两画面： 一大一小（大画面分成9份发送）
#define MULPIC_TYPE_SFOUR      7//特殊四画面：一大三小（大画面分成4份发送）
#define MULPIC_TYPE_SEVEN      8//七画面：三大四小（大画面分成4份发送）
#define MULPIC_TYPE_EIGHT      9//八画面
#define MULPIC_TYPE_NINE       10//九画面
#define MULPIC_TYPE_TEN        11//十画面：二大八小（大画面分成4份发送）
#define MULPIC_TYPE_THIRTEEN   12//十三画面
#define MULPIC_TYPE_SIXTEEN    13//十六画面

//画面合成最大通道数
#define MULPIC_MAX_CHNS        16
#define MULTIPIC_MAX_DSP_NUM   5

//画面合成初始化参数
typedef struct
{
	u8    byType; //画面合成类型
	u32   dwTotalMapNum;//参与画面合成的通道数
	u32   dwCoreSpd; //Map内核速度
	u32   dwMemSpd;  //内存速度
	u32   dwMemSize; //内存大小
}TMulPicParam;
/* 注意：字段dwMapUseFlag为32位的位模式。*/

//状态
typedef struct
{
	BOOL32  bMergeStart; //是否开始画面合成？
	u8  byType; //画面合成类型
	u32 dwTotalMapNum; //Map使用标志
	u8   byCurChnNum; //当前参与合成的通道数
	TVideoEncParam tVideoEncParam; //图像编码参数
	TNetAddress atNetSndAddr; //网络发送地址
	TNetAddress atNetRcvAddr[MULPIC_MAX_CHNS]; //各通道的网络接收地址
}TMulPicStatus;

//统计
typedef struct
{
	u32  m_dwRecvBitRate;          //接收码率
	u32  m_dwRecvPackNum;         //收到的包数
	u32  m_dwRecvLosePackNum;    //网络接收缺少的包数
}TMulPicChnStatis;

typedef struct
{
	u32  m_dwSendBitRate;          //发送码率
	u32  m_dwSendPackNum;        //发送的包数
	u32  m_dwSendDiscardPackNum;  //发送丢包数
    TMulPicChnStatis atMulPicChnStatis[MULPIC_MAX_CHNS];
}TMulPicStatis;     

typedef struct
{
	u32   dwTotalMapNum;
	u32   dwCoreSpd; 
	u32   dwMemSpd;  
	u32   dwMemSize; 
	u8    bDoubleVidType;
	void * picMerger;
}TMulPicCheckParam;

typedef struct 
{
	u8 RColor;    //R背景
	u8 GColor;    //G背景
	u8 BColor;    //B背景	
}TMulPicColor;

typedef struct 
{
	BOOL32    bHasData;      //记录MapHalt前有操作
	u8 *  pbyBmp;        //以下为Osd信息
	u32   dwBmpLen; 
	u32   dwXPos; 
	u32   dwYPos; 
	u32   dwWidth; 
	u32   dwHeight; 
	TBackBGDColor    tBackBGDColor; 
	u32   dwClarity; 
}TIconRecordInfo;

class CVideoEncoder;
class CAudioEncoder;
class CVideoDecoder;
class CAudioDecoder;

class CHardMulPic
{
public:
	CHardMulPic();
	~CHardMulPic();
	
	int Initialize(TMulPicParam tMulPicParam, BOOL32 bDbVid = FALSE);
	int Quit(); 
	
	//int AddChannel(u8 byChnNo, TNetAddress  tNetRcvAddr);
	int AddChannel(u8 byChnNo, TNetAddress  tNetRcvAddr,TNetAddress tRtcpNetRcvAddr);
	int RemoveChannel(u8 byChnNo);
	
	int SetNetSndLocalAddr(TNetAddress atNetSndLocalAddr[2]);
	int StartMerge(TVideoEncParam tVideoEncParam[2], TNetAddress tNetSndAddr[2]);
	int StopMerge(void);
	
	int GetStatus(TMulPicStatus &tMulPicStatus);
	int GetStatis(TMulPicStatis &tMulPicStatis);

	int SetMulPicType(u8 byNewType);

	int ChangeBitRate(BOOL32 bSecondType, u16  wBitRate/*kbps*/);
	u16 SetFastUpdata();
	
	/*获取通道接收统计信息*/
	int GetVidRecvStatis(u8 byChn, TKdvDecStatis &tKdvDecStatis);	

	/*设置图像的网络接收重传参数*/
	u16  SetNetRecvFeedbackVideoParam(TNetRSParam tNetRSParam, BOOL32 bRepeatSnd = FALSE, u8 byTotalNum = 0);
	/*设置图像的网络发送重传参数*/
	u16  SetNetSendFeedbackVideoParam(u16 wBufTimeSpan, BOOL32 bRepeatSnd = FALSE);
	
	//视频是否使用前向纠错
	u16    SetVidFecEnable(u8 byChnNo, BOOL32 bEnableFec);
	//设置fec的切包长度
	u16    SetVidFecPackLen(u8 byChnNo, u16 wPackLen);
	//是否帧内fec
	u16    SetVidFecIntraFrame(u8 byChnNo, BOOL32 bIntraFrame);
	//设置fec算法 FEC_MODE_RAID5
	u16    SetVidFecMode(u8 byChnNo, u8 byAlgorithm);
	//设置fec的x包数据包 + y包冗余包
	u16    SetVidFecXY(u8 byChnNo, s32 nDataPackNum, s32 nCrcPackNum);
	
	//设置视频编码载荷
	u16  SetVidEncryptPT(u8 byChnNo, u8 byEncryptPT); 

	//设置视频编码加密key字串、以及 加密模式 Aes or Des
	u16  SetVidEncryptKey(u8 byChnNo, s8 *pszKeyBuf, u16 wKeySize, u8 byEncryptMode );  
	
	//设置 H.263+/H.264 等动态视频载荷的 Playload值
	u16  SetVideoActivePT(u8 byChnNo, u8 byRmtActivePT, u8 byRealPT );
	//设置 视频解码key字串 以及 解密模式 Aes or Des
    u16  SetVidDecryptKey(u8 byChnNo, s8 *pszKeyBuf, u16 wKeySize, u8 byDecryptMode); 
	
	/*非上层用户接口*/
	int SendVidFrameToMap(u8 byChn, u32 dwStreamType, u32 dwFrameSeq, u32 dwWidth,
		                  u32 dwHeight, u8 *pbyData, u32 dwDataLen);

	/*VPU板，先全部打开Map，再把电平拉低，最后再loadFile和运行MediaCtrl*/
	int VpuInitProc(TMulPicParam tMulPicParam, u32  dwFirstMapNum, u32 dwLastMapNum);

	/*设置画面合成背景颜色tBGDColor,以及边框颜色tSlidelineColor[MULPIC_MAX_CHNS]，
	需要改变边框颜色时，只要改变tSlidelineColor[]对应通道的值即可*/
	int SetBGDAndSidelineColor(TMulPicColor tBGDColor, TMulPicColor atSlidelineColor[MULPIC_MAX_CHNS], BOOL32 bAddSlideLine = FALSE);

	/*设置是否背板输出*/
	u16 SetBackBoardOut(BOOL32 bOut);

	/*开始加入台标*/
	int StartAddIcon(u8 byChnNo, u8 *pbyBmp, u32 dwBmpLen, u32 dwXPos, u32 dwYPos, u32 dwWidth,
								 u32 dwHeight, TBackBGDColor tBackBGDColor, u32 dwClarity);

	/* 停止加入台标 */
	int StopAddIcon(u8 byChnNo);

	/*设置显示模式 (video 或 vga), DISPLAY_TYPE_PAL : 1, DISPLAY_TYPE_VGA : 3*/
	u16 SetDisplayType(u8 byDisplayType);

private:
	int InitOneDsp(TMulPicParam tMulPicParam);
	int InitTwoDsp(TMulPicParam tMulPicParam);
	int InitThreeDsp(TMulPicParam tMulPicParam);
	int InitFourDsp(TMulPicParam tMulPicParam);
	int InitFiveDsp(TMulPicParam tMulPicParam);

	int OnInitMapOpr(u8 byMapNo, TMulPicParam tMulPicParam, u32 dwTotalMapNo, 
		             u32 dwVidEncNum, u32 dwVidDecNum, u32 dwStartChn, u32 dwEndChn, BOOL32 bDbVid = FALSE);

	u32 GetPicMergeChn(u8  byPicMergeType);
	u8  GetRealChnNo(u8  byChnNo);
	u8  Get3MapRealChnNum(u8  byChnNo);
	u8  Get4MapRealChnNum(u8  byChnNo);
	u8  Get5MapRealChnNum(u8  byChnNo);

	/* 得到画面合成通道号对应的map号和解码通道号 */
	int  GetDspNoAndVidDecChn(u8 byChnNo, u8 *pbyDspNo, u8  *pbyVidDecChnNo);
	int  Get5MapDspNoAndVidDecChn(u8 byChnNo, u8 *pbyDspNo, u8  *pbyVidDecChnNo);	
	int  Get4MapDspNoAndVidDecChn(u8 byChnNo, u8 *pbyDspNo, u8  *pbyVidDecChnNo);	
	int  Get3MapDspNoAndVidDecChn(u8 byChnNo, u8 *pbyDspNo, u8  *pbyVidDecChnNo);	
	int  Get2MapDspNoAndVidDecChn(u8 byChnNo, u8 *pbyDspNo, u8  *pbyVidDecChnNo);	
	int  Get1MapDspNoAndVidDecChn(u8 byChnNo, u8 *pbyDspNo, u8  *pbyVidDecChnNo);	

	CVideoDecoder    *m_pVidEncPlayer;
	CVideoEncoder    *m_pVideoEncoder;
	CVideoEncoder    *m_pSecondVideoEncoder;
	CVideoDecoder    *m_apVideoDecoder[MULPIC_MAX_CHNS];
	
public:
	u8  m_byMulPicType; 
	u32 m_dwTotalMapNum;
	u32 m_bPicMergerStart;
	u32 m_dwMaxVidChnNum;
	BOOL32  m_bDoubleVidType;
	TNetAddress m_atNetSndLocalAddr[2];
	TMulPicColor m_tBGDColor;
	TMulPicColor m_atSlidelineColor[MULPIC_MAX_CHNS];
	BOOL32	m_bAddSlideLine;
	BOOL32  m_bCheckEnd;

    BOOL32 m_bBackBoardOut;     //是否背板输出
	
private:
   	s64    m_qwLastFastupdataTime;     //最近一次FastUpdata的时间
	volatile u32 m_adwMulPicMapBuffer[4][4];  //Osd Buffer, 每块map有4路通道，最多4块map
	TIconRecordInfo  m_atAddLogoInfo[MULPIC_MAX_CHNS];       //台标融入码流信息
};

API void picmergever(u32 dwMapId);
API void setfilt(BOOL32 bFilt);

#ifndef TOSFUNC
#define TOSFUNC
//设置媒体TOS值 nType类型0:全部 1:音频 2:视频 3: 数据
API int SetMediaTOS(int nTOS, int nType);
API int GetMediaTOS(int nType);
#endif

#endif /*_KDVMULPIC_H_*/





















