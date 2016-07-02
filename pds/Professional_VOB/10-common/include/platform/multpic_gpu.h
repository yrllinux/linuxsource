/*****************************************************************************
  模块名      : 媒体控制模块
  文件名      : mediactrl.h
  相关文件    : 
  文件实现功能: 多画面合成器上层API 封装
  作者        : 胡小鹏
  版本        : V1.0  Copyright(C) 1997-2009 KDC, All rights reserved.
  -----------------------------------------------------------------------------
  修改记录:
  日  期      版本        修改人      修改内容
  2009/02/20  V1.0        胡小鹏       创建
  2009/08/13  V1.1        胡小鹏       创建
******************************************************************************/
#ifndef _MULPIC_GPU_H_
#define _MULPIC_GPU_H_

#include "codecwrapper_common.h"

class CSemaphoreCom;

class CMulPic;
typedef struct
{
    CMulPic* pcMulPic;            //画面合成类指针
    l32 l32EncNo;                 //编码器编号
}TProcParam;
#define MERGE_NUM (3)
//画面合成类型定义
typedef enum
{
        MERGE_M0 = 0,              //无画面合成
        MERGE_M1 = 100,            //一画面合成(原PIC_MERGE_ONE)
        MERGE_M2 = 200,            //两画面合成(原PIC_MERGE_VTWO)
        MERGE_M2_1_BR1,            //两画面合成(一大一小，小画面在大画面内的右下角，原PIC_MERGE_ITWO)
        MERGE_M3_T1 = 300,         //三画面合成(大小相等，一上两下，原PIC_MERGE_THREE)
        MERGE_M3_B1,               //三画面合成(大小相等，两上一下)
        MERGE_M3_1_B2,             //三画面合成(一大两小，小画面在下)
        MERGE_M3_1_T2,             //三画面合成(一大两小，小画面在上)
        MERGE_M3_1_R2,             //三画面合成(一大两小，左大画面，右小画面)
        MERGE_M4 = 400,            //四画面合成(原PIC_MERGE_FOUR)
        MERGE_M4_1_R3,             //四画面合成(一大三小，小画面在右，原PIC_MERGE_SFOUR)
        MERGE_M6 = 600,            //六画面合成(大小相等，上三下三)
        MERGE_M6_1_5,              //六画面合成(一大五小，原PIC_MERGE_SIX)
        MERGE_M6_2_B4,             //六画面(两大四小，小画面在下)
        MERGE_M7_3_TL4 = 700,      //七画面合成(三大四小，四个小画面在左上角)
        MERGE_M7_3_TR4,            //七画面合成(三大四小，四个小画面在右上角)
        MERGE_M7_3_BL4,            //七画面合成(三大四小，四个小画面在左下角)
        MERGE_M7_3_BR4,            //七画面合成(三大四小，四个小画面在右下角，原PIC_MERGE_SEVEN)
        MERGE_M7_3_BLR4,           //七画面合成(三大四小，四个小画面在下面左右两侧)
        MERGE_M7_3_TLR4,           //七画面合成(三大四小，四个小画面在上面左右两侧)
        MERGE_M8_1_7 = 800,        //八画面合成(一大七小，原PIC_MERGE_EIGHT)
        MERGE_M9 = 900,            //九画面合成(原PIC_MERGE_NINE)
        MERGE_M10_2_R8 = 1000,     //十画面合成(两大八小，八个小画面在右侧，原PIC_MERGE_TEN)
        MERGE_M10_2_B8,            //十画面合成(两大八小，八个小画面在下面，原PIC_MERGE_TEN_2_8)
        MERGE_M10_2_T8,            //十画面合成(两大八小，八个小画面在上面)
        MERGE_M10_2_L8,            //十画面合成(两大八小，八个小画面在左侧)
        MERGE_M10_2_TB8,           //十画面合成(两个大画面居中，上下各四个小画面）
        MERGE_M10_1_9,             //十画面合成(一大九小，原PIC_MERGE_TEN_1_9)
        MERGE_M12_1_11 = 1200,     //十二画面合成(一大十一小，原PIC_MERGE_TWELVE)
        MERGE_M13_TL1_12 = 1300,   //十三画面合成(大画面在左上角，PIC_MERGE_THIRTEEN)
        MERGE_M13_TR1_12,          //十三画面合成(大画面在右上角)
        MERGE_M13_BL1_12,          //十三画面合成(大画面在左下角)
        MERGE_M13_BR1_12,          //十三画面合成(大画面在右下角)
        MERGE_M13_1_ROUND12,       //十三画面合成(大画面居中，小画面环绕四周)
        MERGE_M14_1_13 = 1400,     //十四画面合成(一大十三小，原PIC_MERGE_FOURTEEN)
        MERGE_M14_TL2_12,          //十四画面合成(两个大画面在左上角)
        MERGE_M15_T3_12 = 1500,    //十五画面合成(上三下十二)
        MERGE_M16 = 1600,          //十六画面合成(原PIC_MERGE_SIXTEEN)
        MERGE_M16_1_15,            //十六画面合成(一大十五小，原PIC_MERGE_SIXTEEN_1_15)
        MERGE_M20 = 2000,          //二十画面合成
        MERGE_M25 = 2500,          //二十五画面合成(原PIC_MERGE_TWENTYFIVE)
        MERGE_M36 = 3600,          //三十六画面合成(原PIC_MERGE_THIRTYSIX)
        MERGE_M49 = 4900,          //四十九画面合成(原PIC_MERGE_FORTYNINE)
        MERGE_M64 = 6400,           //六十四画面合成(原PIC_MERGE_SIXTYFOUR)
        MERGE_MAX                  //画面合成方式最大值
} MergeStyle;

#define PIC_MERGE_NUM_MAX 64 //画面合成路数的最大值
#define PIC_ENC_NUM_MAX 10   //编码路数的最大值

enum enNoStreamBack
{
	NoStrm_PlyBlk = 0,		//黑色背景(默认)
	NoStrm_PlyLst = 1,		//播放上一帧
	NoStrm_PlyBmp_dft = 2,	//默认图片
	NoStrm_PlyBmp_usr = 3,	//用户定义图片
	NoStrm_Total
};
//画面合成初始化参数
typedef struct
{
    u32   dwType;           //画面合成类型
    u32   dwTotalMapNum;    //参与画面合成的通道数
}TMulPicParam;

//状态
typedef struct
{
    BOOL32  bMergeStart;     //是否开始画面合成？
    u32 dwMergeType;          //画面合成类型 
    u8 byChnNum;          //当前接收解码的通道数
    TNetAddress atNetRcvAddr[PIC_MERGE_NUM_MAX]; //各通道的网络接收地址

    u8 byEncNum;               //编码发送的通道数
    TVidEncParam atVidEncParam[PIC_ENC_NUM_MAX]; //图像编码参数
    TNetAddress atNetSndAddr[PIC_ENC_NUM_MAX]; //网络发送地址    
}TMulPicStatus;

typedef struct
{
    TVidRecvStatis atRecvStatis[PIC_MERGE_NUM_MAX];
    TVidSendStatis atSenderStatis[PIC_ENC_NUM_MAX];
}TMulPicStatis;    

typedef struct 
{
    u8 RColor;    //R背景
    u8 GColor;    //G背景
    u8 BColor;    //B背景	
}TMulPicColor;

typedef struct 
{
    TMulPicColor tMulPicColor; 
    BOOL32 bShow;     //是否显示边框	
}TDrawFrameColor;
#define MAX_MUL_NUM 10

class CODECWRAPPER_API CMulPic
{
public:
	CMulPic();
	~CMulPic();
	
    /*************************************************************
    函数名    : Initialize
    功能      ：合成初始化
    参数说明  ：tMulPicParam [I]合成参数            

    返回值说明：成功返回TRUE，失败返回FALSE    
    **************************************************************/
	BOOL32 Initialize(TMulPicParam tMulPicParam);

    /*************************************************************
    函数名    : Quit
    功能      ：合成退出
    参数说明  ：无            

    返回值说明：成功返回TRUE，失败返回FALSE    
    **************************************************************/
	BOOL32 Quit(); 
	
    /*************************************************************
    函数名    : SetChannelParam
    功能      ：指定合成输入通道
    参数说明  ：byChnNo         [I] 要设定的通道号
                tNetRcvAddr     [I] 指定接收数据的IP地址和端口            

    返回值说明：成功返回TRUE，失败返回FALSE
    **************************************************************/
	BOOL32 SetChannelParam(u8 byChnNo, TNetAddress tNetRcvAddr);

    /*************************************************************
    函数名    : SetChannelParam
    功能      ：指定合成输入通道
    参数说明  ：byChnNo         [I] 要设定的通道号
                tNetRcvAddr     [I] 指定接收数据的IP地址和端口  
                tRtcpNetRcvAddr [I] Rtcp的IP地址和端口 （指令）

    返回值说明：成功返回TRUE，失败返回FALSE
    **************************************************************/
    BOOL32 SetChannelParam(u8 byChnNo, TNetAddress tNetRcvAddr, TNetAddress tRtcpNetRcvAddr,  TNetAddress tRtcpNetLocalAddr);

    /*************************************************************
    函数名    : SetChannelActive
    功能      ：指定输入通道是否工作
    参数说明  ：byChnNo         [I] 要设定的通道号
                bActive         [I] TRUE为工作，FALSE为不工作

    返回值说明：成功返回TRUE，失败返回FALSE
    **************************************************************/
    BOOL32 SetChannelActive(u8 byChnNo, BOOL32 bActive);

    /*************************************************************
    函数名    : ClearChannelCache
    功能      ：清除指定输入通道的缓冲数据
    参数说明  ：byChnNo         [I] 要设定的通道号               

    返回值说明：成功返回TRUE，失败返回FALSE
    **************************************************************/
    BOOL32 ClearChannelCache(u8 byChnNo);

    /*************************************************************
    函数名    : SetEncActive
    功能      ：指定编码输出通道是否工作
    参数说明  ：byChnNo         [I] 要设定的通道号
                bActive         [I] TRUE为工作，FALSE为不工作

    返回值说明：成功返回TRUE，失败返回FALSE
    **************************************************************/
    BOOL32 SetEncActive(u8 byChnNo, BOOL32 bActive);

    /*************************************************************
    函数名    : SetAllEncActive
    功能      ：设置所有编码输出是否工作
    参数说明  ：bActive         [I] TRUE为工作，FALSE为不工作               

    返回值说明：成功返回TRUE，失败返回FALSE
    **************************************************************/
    BOOL32 SetAllEncActive(BOOL32 bActive);

    /*************************************************************
    函数名    : StartMerge
    功能      ：开始合成
    参数说明  ：ptVideoEncParam     [I] 编码参数组首指针
                ptNetSndAddr        [I] 发送目地地址和端口组首指针
                ptNetSndLocalAddr   [I] 本地地址和端口组首指针
                nNum                [I] 编码组个数

    返回值说明：成功返回TRUE，失败返回FALSE    
    **************************************************************/
    BOOL32 StartMerge(TVidEncParam* ptVideoEncParam, TNetAddress* ptNetSndAddr, TNetAddress* ptNetSndLocalAddr,  l32 nNum);

    /*************************************************************
    函数名    : SetEncParam
    功能      ：设置指定通道编码参数
    参数说明  ：byChnNo              [I]通道号
                tMulPicStatus        [I] 编码参数

    返回值说明：成功返回TRUE，失败返回FALSE   
    **************************************************************/
    BOOL32 SetEncParam(u8 byChnNo, TVidEncParam* ptVideoEncParam);

    /*************************************************************
    函数名    : StopMerge
    功能      ：停止合成
    参数说明  ：无

    返回值说明：成功返回TRUE，失败返回FALSE    
    **************************************************************/
	BOOL32 StopMerge(void);
   
    /*************************************************************
    函数名    : GetActiveStatus
    功能      ：取启动/停止 状态
    参数说明  ：无
    
    返回值说明：TRUE:为启动  FALSE:为停止
    **************************************************************/
    BOOL32 GetActiveStatus();
    
    /*************************************************************
    函数名    : GetStatus
    功能      ：取状态参数
    参数说明  ：tMulPicStatus        [IO] 状态参数
    
    返回值说明：成功返回TRUE，失败返回FALSE
    **************************************************************/
	BOOL32 GetStatus(TMulPicStatus &tMulPicStatus);

    /*************************************************************
    函数名    : GetStatis
    功能      ：取统计
    参数说明  ：tMulPicStatus        [IO] 状态参数
    
    返回值说明：成功返回TRUE，失败返回FALSE
    **************************************************************/
	BOOL32 GetStatis(TMulPicStatis &tMulPicStatis);

    /*************************************************************
    函数名    : SetMulPicType
    功能      ：设置画面合成方式
    参数说明  ：byNewType          [I]  画面合成方式（详见MergeStyle）
    支持动态切换
    
    返回值说明：成功返回TRUE，失败返回FALSE
    **************************************************************/
	BOOL32 SetMulPicType(u32 dwNewType);

    /*************************************************************
    函数名    : ChangeBitRate
    功能      ：设置编码发送的码率
    参数说明  ：byChnNo           [I] 编码发送通道号
                dwBitRate         [I] 码率值，单位 (Kbit/s)
    
    返回值说明：成功返回TRUE，失败返回FALSE
    **************************************************************/
	BOOL32 ChangeEncBitRate(u8 byChnNo, u32 dwBitRate);
    
    /*************************************************************
    函数名    : SetFastUpdata
    功能      ：设置编码器编关键帧
    参数说明  ：l32Num             [I]编码器编号
    
    返回值说明：成功返回TRUE，失败返回FALSE
    **************************************************************/
    BOOL32 SetFastUpdata(u8 byChnNo);
	
    /*************************************************************
    函数名    : GetVidRecvStatis
    功能      ：获取通道接收统计信息
    参数说明  ：byChnNo           [I] 通道号
                tKdvDecStatis     [IO] 统计信息
    
    返回值说明：成功返回TRUE，失败返回FALSE     
    **************************************************************/
	BOOL32 GetVidRecvStatis(u8 byChnNo, TVidRecvStatis &tVidRecvStatis);
    
    /*************************************************************
    函数名    : GetVidSendStatis
    功能      ：获取通道接收统计信息
    参数说明  ：byChnNo           [I] 通道号
                tKdvDecStatis     [IO] 统计信息
    
    返回值说明：成功返回TRUE，失败返回FALSE     
    **************************************************************/
	BOOL32 GetVidSendStatis(u8 byChnNo, TVidSendStatis &tVidSendStatis);

    /*************************************************************
    函数名    : SetNetRecvFeedbackVideoParam
    功能      ：设置所有图像的网络接收重传参数
    参数说明  ：TNetRSParam         [I] 重传参数
                bRepeatSnd          [I] 重传参数
    
    返回值说明：成功返回TRUE，失败返回FALSE
    **************************************************************/
	BOOL32  SetNetRecvFeedbackVideoParam(TNetRSParam tNetRSParam, BOOL32 bRepeatSnd = FALSE);

    /*************************************************************
    函数名    : SetNetSendFeedbackVideoParam
    功能      ：设置所有图像的网络发送重传参数
    参数说明  ：wBufTimeSpan         [I] 重传参数
                bRepeatSnd           [I] 重传参数
    
    返回值说明：成功返回TRUE，失败返回FALSE      
    **************************************************************/
	BOOL32  SetNetSendFeedbackVideoParam(u16 wBufTimeSpan, BOOL32 bRepeatSnd = FALSE);	

    /*************************************************************
    函数名    : SetBGDAndSidelineColor
    功能      ：设置画面合成背景颜色,以及边框颜色
    参数说明  ：tBGDColor          [I]  合成背景颜色
                tBGDFrameLineColor [I]  合成背景边框颜色
                TDrawFrameColor    [I]  前景边框颜色(数组)

    返回值说明：成功返回TRUE，失败返回FALSE
    **************************************************************/
    BOOL32 SetBGDAndSidelineColor(TMulPicColor tBGDColor, 
                                  TMulPicColor tBGDFrameLineColor,
                                  TDrawFrameColor atFramelineColor[PIC_MERGE_NUM_MAX]); 

    /*************************************************************
    函数名    : SetVideoActivePT
    功能      ：设置 动态视频载荷的 Playload值
    参数说明  ：byChnNo        [I] 接收解码通道号
                byRmtActivePT  [I] 
                byRealPT       [I] 

    返回值说明：成功返回TRUE，失败返回FALSE    
    **************************************************************/
    BOOL32 SetVideoActivePT(u8 byChnNo, u8 byRmtActivePT, u8 byRealPT);

    /*************************************************************
    函数名    : SetDecryptKey
    功能      ：设置 动态视频载荷的 Playload值
    参数说明  ：byChnNo        [I] 接收解码通道号
                pszKeyBuf      [I] 
                wKeySize       [I] 
                byDecryptMode  [I]

    返回值说明：成功返回TRUE，失败返回FALSE    
    **************************************************************/
    BOOL32 SetDecryptKey(u8 byChnNo, s8 *pszKeyBuf, u16 wKeySize, u8 byDecryptMode=DES_ENCRYPT_MODE);        
    
    //设置加密key字串及加密码流的动态载荷PT值, pszKeyBuf设置为NULL-表示不加密

    /*************************************************************
    函数名    : SetEncryptKey
    功能      ：设置加密key字串及加密码流的动态载荷PT值, pszKeyBuf设置为NULL-表示不加密
    参数说明  ：byChnNo        [I] 编码发送通道号
                pszKeyBuf      [I] 
                wKeySize       [I] 
                byEncryptMode  [I]

    返回值说明：成功返回TRUE，失败返回FALSE    
    **************************************************************/
    BOOL32 SetEncryptKey(u8 byChnNo, s8 *pszKeyBuf, u16 wKeySize, u8 byEncryptMode=DES_ENCRYPT_MODE);
    
    /*************************************************************
    函数名    : SetLogo
    功能      ：设置
    参数说明  ：byChnNo        [I] 编码发送通道号
                pszBuf         [I] 字符串首指针
                wSize          [I] 字符串长度

    返回值说明：成功返回TRUE，失败返回FALSE    
    **************************************************************/
    BOOL32 SetLogo(u8 byChnNo, s8 *pszBuf, u16 wSize);

    /*************************************************************
    函数名    : SetLogo
    功能      ：设置
    参数说明  ：byChnNo        [I] 编码发送通道号
                pszBuf         [I] BMP图片地址
                dwSize         [I] BMP图片长度
				dwLen          [I] BMP图片中保存的字符长度(按像素点数)

    返回值说明：成功返回TRUE，失败返回FALSE    
    **************************************************************/
	BOOL32 SetLogo(u8 byChnNo, u8* pbyBmpbuf, u32 dwSize, u32 dwLen);
    /*************************************************************
    函数名    : SetLogoActive
    功能      ：设置
    参数说明  ：byChnNo        [I] 通道号
                bActive        [I] 是否工作
    
    返回值说明：成功返回TRUE，失败返回FALSE    
    **************************************************************/
    BOOL32 SetLogoActive(u8 byChnNo, BOOL32 bActive);
    
    /*************************************************************
    函数名    : SaveDecIn
    功能      ：保存解码前码流
    参数说明  ：l32ChnNo         解码器通道号
                l32FrameNum      要保存的帧数
    
    返回值说明：成功返回TRUE，失败返回FALSE    
    **************************************************************/
    BOOL32 SaveDecIn(l32 l32ChnNo, l32 l32FrameNum);

    /*************************************************************
    函数名    : SaveDecOut
    功能      ：保存解码后YUV图像
    参数说明  ：l32ChnNo         解码器通道号
                l32FrameNum      要保存的帧数
    
    返回值说明：成功返回TRUE，失败返回FALSE    
    **************************************************************/
    BOOL32 SaveDecOut(l32 l32ChnNo, l32 l32FrameNum);

    /*************************************************************
    函数名    : SaveEncIn
    功能      ：保存编码前YUV
    参数说明  ：l32ChnNo         编码器通道号
                l32FrameNum      要保存的帧数
    
    返回值说明：成功返回TRUE，失败返回FALSE    
    **************************************************************/
    BOOL32 SaveEncIn(l32 l32ChnNo, l32 l32FrameNum);

    /*************************************************************
    函数名    : SaveEncOut
    功能      ：保存编码后码流
    参数说明  ：l32ChnNo         解码器通道号
                l32FrameNum      要保存的帧数
    
    返回值说明：成功返回TRUE，失败返回FALSE    
    **************************************************************/
    BOOL32 SaveEncOut(l32 l32ChnNo, l32 l32FrameNum);
    
	/*************************************************************
    函数名    : SetNoStreamBak
    功能      ：设置空闲背景
    参数说明	BakType      背景类型
                pbyBmp       图片路径
				dwChnNo      解码通道
    
    返回值说明：成功返回TRUE，失败返回FALSE    
    **************************************************************/
	s32 SetNoStreamBak(u32 BakType, s8 *pszBmp = NULL, s32 nChnNo = -1);

	/*************************************************************
    函数名    : SetResizeMode
    功能      ：设置前景填充模式 (适配器模式下:16:9 适配标清则强制使用加黑边模式)
    参数说明	0      加黑边
                1      裁边
				2      非等比拉伸 
				默认 1
    返回值说明：    
    **************************************************************/
	u16 SetResizeMode(u32 dwMode);


	 /*************************************************************
    函数名    : GetEncStatus
    功能      ：获取编码通道状态
    参数说明  ：byChnNo           [I] 编码发送通道号
                bEncActive        [IO] 
    
    返回值说明：成功返回TRUE，失败返回FALSE
    **************************************************************/
	BOOL32 GetEncStatus(u8 byChnNo, BOOL32& bEncActive);
    //////////////////////////////////////////////////////////////////////////
    //以下是非用户接口 
	BOOL32 SetBmp24Back4v3(u8* pData,  u32 dwWidth, u32  dwHeight, u16 wBitCount);
	BOOL32 SetBmp24Back16v9( u8* pData, u32 dwWidth, u32 dwHeight, u16 wBitCount);

	BOOL32 SetRcvInMarker(u8 byChNo, BOOL32 bMarker);       //设置协议收包组帧模式
    l32 m_al32FrontWidth[PIC_MERGE_NUM_MAX];    //前景宽度
    l32 m_al32FrontHeight[PIC_MERGE_NUM_MAX];   //前景高度   

    void* m_pListImg;
    void* m_pListAddLogo;
    void* m_pListEnc;                 //接收编码列表
    void* m_pListDec;                 //解码发送列表   

    void* m_ptProcessLock;           //线程锁        
    
    
    void* m_ptLock;                  //线程锁,合成
    void* m_ptCond;             
    
    void* m_aptLockEnc[PIC_ENC_NUM_MAX];     //线程锁，解码
    //void* m_aptCondEnc[PIC_ENC_NUM_MAX]; 
    CSemaphoreCom* m_aptCondEnc[PIC_ENC_NUM_MAX];
    
    void* m_pListDataBuf4v3;       
    void* m_pListDataBuf16v9;
    void* m_aptLockClsBuf[PIC_ENC_NUM_MAX];  //线程锁，回收数据
    void* m_aptCondClsBuf[PIC_ENC_NUM_MAX]; 
    void* m_ptEnc0Lock;
    void* m_ptEnc1Lock;
    
    void* m_pListVidEncParam;    
    BOOL32 m_abBG16v9[PIC_ENC_NUM_MAX];            //合成背景有16比9    

    void SetAllImgActive(BOOL32 bActive);
    BOOL32 SetMergeParam();
    u32 RecalculateBitRate(u32 u32BitRate);
	u32 GetLoopTime();
	u32 GetMaxFrameRate();
	void SetYUV420Back();
    
    l32 m_l32ShowPicNum;

	void* m_ptBackBmpLock; 
	u32 m_bUseBackGroundbmp;
	u8* m_pDataYUV420;
	u32 m_dwbmpWidth;
	u32 m_dwbmpHeight;
	u8* m_pDataYUV420WS; //宽屏版本
	u32 m_dwbmpWidthWS;
	u32 m_dwbmpHeightWS;
	u8* m_pDataYUV420WS1080; //宽屏版本1080p
	BOOL32 m_bUsrImgShow[PIC_MERGE_NUM_MAX];
	u8* m_pUsrImgYuv[PIC_MERGE_NUM_MAX];
	u8  m_byDecNoDataCount[PIC_MERGE_NUM_MAX];
	u32 m_dwUsrImgWidth[PIC_MERGE_NUM_MAX];
	u32 m_dwUsrImgHeight[PIC_MERGE_NUM_MAX];
	BOOL32 m_bClearChannelImmediat;
	BOOL32 m_bIsEncChanged;  //编码参数是否发生变化
	BOOL32 m_bIsBgChanged;   //空闲背景是否发生变化
	u32 m_enNoStreamBack;  //空闲背景填充模式
//add by taoz
	void* m_pGTime;
	BOOL32 m_bExitMergeLoop ;
	BOOL32 m_bExitMergeProcess ;
	BOOL32 m_bExitEncProcess[PIC_ENC_NUM_MAX];
	BOOL32 m_bExitClearBufProcess ;

    BOOL32 m_bExitMergeLoop1080P;
	BOOL32 m_bIsBaseGround;  //底图是否发生变化
	u32 m_TimerID ;//定义定时器句柄 
	u32 m_dwReszMode;
	u32 m_dwReszModeOld;
	u32 m_dwMaxEncWidth;
	u32 m_dwMaxEncHeight;
	u32 m_dwSelectMergebackID; // 0 : 标清 ； 1: 720p ; 2:1080p
	u32 m_dwEncMode;			// 0x01 : SD 0x10 HD
	void *m_pvImageZoomHandle;
#ifdef _LINUX_
	pthread_rwlock_t m_rwlock0;
	pthread_rwlock_t    m_rwlock1;

    pthread_rwlockattr_t  m_attr0; 
	pthread_rwlockattr_t  m_attr1;
#endif
private:
    BOOL32 AppendEnc(TVidEncParam* ptVideoEncParam, 
                TNetAddress* ptNetSndAddr,
                TNetAddress* ptNetSndLocalAddr,
                l32 nChannelNum);

    BOOL32 AdjustEncParam();    
	BOOL32 AdjustDecPostSize(l32 dwMergeType, u32 dwBackWidth, u32 dwBackHeight);
    BOOL32 SetAllChannelActive(BOOL32 bActive);
    void AdjustMergeBackSize(TVidEncParam* ptVideoEncParam = NULL, l32 nChannelNum = 0);
    l32 GetFormat(l32 l32Width, l32 l32Height);
    BOOL32 Get16v9Mode(l32 l32Width, l32 l32Height);
    void ConvertEncParam(TVidEncParam* ptVideoEncParam, void* ptVoid, l32 nChannelNum);

    BOOL32 m_bActive;
    l32 m_l32MergeType;    
    l32 m_al32BackWidth[MERGE_NUM];      //合成背景图宽度
    l32 m_al32BackHeight[MERGE_NUM];     //合成背景图高度    
    
    TMulPicColor m_tBGDColor;                   //合成背景色
    TMulPicColor m_tBGDFrameLineColor;          //合成背景边框色
    TDrawFrameColor m_atFramelineColor[PIC_MERGE_NUM_MAX]; //合成前景边框色  
	TProcParam m_tProcParam[PIC_ENC_NUM_MAX];

	u32 m_dwEncMaxFrameRate;

public:
	u32 m_dwLastTimerTick;
	u32 m_dwCalcTimer;
	
	u32 m_dwLastMergeLoopTick;
	u32 m_dwCalcMergeLoop;

	u32* m_pdwLastEncTick;
	u32* m_pdwCalcEncFrame;
};

#endif //end of _MULPIC_GPU_H_
