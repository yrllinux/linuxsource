/*****************************************************************************
   模块名      : 画面复合器(Video MultiPlexer)
   文件名      : VMPCfg.h
   创建时间    : 2003年 12月 4日
   实现功能    : 画面复合器参数配置
   作者        : zhangsh
   版本        : 
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
******************************************************************************/
#ifndef _VIDEO_MULTIPLEXER_CONFIG_H_
#define _VIDEO_MULTIPLEXER_CONFIG_H_

#include "kdvtype.h"
#include "osp.h"
#include "kdvsys.h"
#include "mcuconst.h"
#include "mcustruct.h"
#include "codeccommon.h"
#include "codeclib.h"
#include "kdvmulpic.h"

/***********************************************************************************************************************************/
/*  MEDIA_TYPE_H262,          0,            300,       31,      2,       2048,       0,       0,     25,    5,       576,     720, */
/***********************************************************************************************************************************/
const u16 EnVidFmt[6][12]=
{
//类型               压缩码率控制参数 最大关键帧 最大量化 最小量化 编码比特率K 图像制式 采集端口 帧率       图像质量         视频高度 视频宽度
//mpeg4
 MEDIA_TYPE_MP4,           0,            25,        31,      2,       1200,       0,       0,     25,    QC_MODE_QUALITY,       576,     720,
//261
 MEDIA_TYPE_H261,          0,            25,        31,      2,       1200,       0,       0,     25,    QC_MODE_QUALITY,       576,     720,
//262(mpeg-2)
 MEDIA_TYPE_H262,          0,            25,        31,      2,       2048,       0,       0,     25,    QC_MODE_QUALITY,       576,     720,
//263
 MEDIA_TYPE_H263,          1,            25,        31,      2,       1200,       0,       0,     25,    QC_MODE_QUALITY,       576,     720,
 //h263+
 MEDIA_TYPE_H263PLUS,      0,            300,       31,      2,       1200,       0,       0,     25,    QC_MODE_QUALITY,       576,     720,
//h264
 MEDIA_TYPE_H264,          0,            300,       51,      2,       2048,       0,       0,     25,    QC_MODE_QUALITY,       576,     720
};

// 作弊输出的最低编码码率
#define MIN_BITRATE_OUTPUT      (u16)128

#define		VMP_SMOOTH_OPEN		(u8)1
struct TVmpBitrateDebugVal
{
protected:
    BOOL32          m_bEnableBitrateCheat;          // 是否启用码率输出作弊
    u16             m_wDefault;                     // 缺省下调的百分比
public:
    void   SetEnableBitrateCheat(BOOL32 bEnable)
    {
        m_bEnableBitrateCheat = bEnable;
    }
    BOOL32 IsEnableBitrateCheat() const
    {
        return m_bEnableBitrateCheat;
    }
    u16    GetDecRateDebug(u16 wOriginRate) const
    {
        // guzh [10/19/2007] 没有则返回缺省值
        wOriginRate = wOriginRate*m_wDefault/100;

        if (wOriginRate > MIN_BITRATE_OUTPUT)
            return wOriginRate;
        else
            return MIN_BITRATE_OUTPUT;
    }
    
    void   SetDefaultRate(u16 wPctRate)
    {
        m_wDefault = wPctRate;
    }    
};

class TVMPCfg
{
public:
	TMulPicParam m_tMulPicParam;   //图像参数设置
    CKDVVMPParam m_tStatus;        //保存当前工作状态
	u32  dwPort;
    
public:
	//设备属性(可选参数)
	u8   byEqpType;                //外设类型
	u8   byEqpId;                  //本机外设号
	s8   szAlias[MAXLEN_ALIAS+1];  //外设别名
	u32  dwLocalIp;                //本地IP地址
	u8   byMcuId;		           //mcu iD

	//网络配置(必须参数), 本地配置或者Agent下发
	u32  dwMcuIP;                 //要连接的Mcu的IP地址
	u16  wMcuPort;                //要连接的Mcu的端口号
	u16  wRecvStartPort;          //接收起始端口号
	u32  dwMaxSendBand;           //最大网络发送带宽

	//是否打开平滑发送, zgc, 2007-09-28
	//不再区分单双编码，zgc, 2008-03-19
	u8   m_byIsUseSmoothSend;	  // 通过读mcueqp.ini进行配置
    
public:
    u32  m_dwMcuIdB;              //Mcu B ID
    u32  m_dwMcuIpB;              //Mcu B 的IP地址
    u16  m_wRecvStartPortB;       //Mcu B 接收起始端口号
    u16  m_wMcuPortB;             //Mcu B 的端口
    
    //图像编码参数(可选参数)
	TVideoEncParam m_tVideoEncParam[2];

    TVmpBitrateDebugVal     m_tDebugVal;                //作弊值
public:
    TVMPCfg();
	~TVMPCfg();
    void Clear();
	void PrintMulPicParam(void) const ;
	void PrintEncParam( u8 byVideoIdx ) const;
	
	BOOL ParseParam();
	void GetDefaultParam(u8 byEnctype,TVideoEncParam& TEncparam);
	void GetDisplayType( u8& byDisplayType );
    void ReadBitrateCheatValues();
    BOOL32 IsEnableBitrateCheat() const
    {
        return m_tDebugVal.IsEnableBitrateCheat();
    }
    u16 GetDecRateDebug(u16 wOriginRate) const
    {
        return m_tDebugVal.GetDecRateDebug(wOriginRate);
    }

	BOOL32 IsNoSmooth()
	{
		return ( 1 != m_byIsUseSmoothSend );
	}

	//  [1/11/2010 pengjie] Modify SetBackBoardOut
	BOOL32 ReadIsNeedBackBrdOut( void );
	// End
};

#endif //_BITRATE_ADAPTER_SERVER_CONFIG_H_
