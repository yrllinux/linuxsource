/*****************************************************************************
   模块名      : MpwLib多画面复合电视墙
   文件名      : mpwcfg.h
   相关文件    : 
   文件实现功能: 定义配置文件操作类
   作者        : john
   版本        : V4.0  Copyright(C) 2001-2005 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   2005/09/25  1.0         john         创建
******************************************************************************/
#ifndef _VIDEO_MPWCFG_H_
#define _VIDEO_MPWCFG_H_

#include "kdvtype.h"
#include "osp.h"
#include "kdvsys.h"
#include "mcuconst.h"
#include "mcustruct.h"
#include "codeccommon.h"
#include "kdvmulpic.h"


#define INVALID_VALUE      (u8)0  //无效的参数值
/************************************************************************/
/*  MEDIA_TYPE_H262,          0,            300,       31,      2,       2048,       0,       0,     25,    5,       576,     720, */
/************************************************************************/
const u16 wEnVidFmt[6][12]=
{
//类型               压缩码率控制参数 最大关键帧 最大量化 最小量化 编码比特率K 图像制式 采集端口 帧率       图像质量         视频高度 视频宽度
// mpeg4
 MEDIA_TYPE_MP4,           0,            25,        31,      3,       1200,       0,       0,     25,    QC_MODE_QUALITY,       576,     720,
// 261
 MEDIA_TYPE_H261,          0,            25,        31,      2,       1200,       0,       0,     25,    QC_MODE_QUALITY,       576,     720,
// 262(mpeg-2)
 MEDIA_TYPE_H262,          0,            25,        31,      2,       2048,       0,       0,     25,    QC_MODE_QUALITY,       576,     720,
// 263
 MEDIA_TYPE_H263,          1,            25,        13,      5,       1200,       0,       0,     25,    QC_MODE_QUALITY,       576,     720,
 // h263+
 MEDIA_TYPE_H263PLUS,      0,            300,       31,      3,       1200,       0,       0,     25,    QC_MODE_QUALITY,       576,     720,
// h264
 MEDIA_TYPE_H264,          0,            300,       30,      30,      2048,       0,       0,     25,    QC_MODE_QUALITY,       576,     720
};

class CMpwCfg
{
public:
	TMulPicParam m_tMulPicParam;   // 图像参数设置
	CKDVVMPParam m_tStatus;        // 保存当前工作状态
    TVideoEncParam tEncparamMp4;   // 解码参数
    TVideoEncParam tEncparamH261;
    TVideoEncParam tEncparamH262;
    TVideoEncParam tEncparamH263;
    TVideoEncParam tEncparamH263plus;
    TVideoEncParam tEncparamH264;

public:
	// 设备属性(可选参数)
	u8   byEqpType;                //外设类型
	u8   byEqpId;                  //本机外设号
	s8   szAlias[MAXLEN_ALIAS+1];  //外设别名
	u32  dwLocalIp;                //本地IP地址
	u16  wMcuId;		           //mcu ID

	// 网络配置(必须参数),本地配置或者Agent下发
	u32  dwMcuIP;                 //要连接的Mcu的IP地址
	u16  wMcuPort;                //要连接的Mcu的端口号
	u16  wRecvStartPort;          //接收起始端口号
	u32  dwMaxSendBand;           //最大网络发送带宽

public:
    u16  m_wMcuIdB;               //Mcu.B ID
    u32  m_dwMcuIpB;              //Mcu.B 的IP地址
    u16  m_wMcuPortB;             //Mcu.B 的端口
	
	TVideoEncParam m_tVideoEncParam[2];  //图像编码参数(可选参数)

public:
	CMpwCfg();
	virtual ~CMpwCfg();
	BOOL32 ParseParam(void);
    void ReadConfigureFile(void);
	void GetDefaultParam(u8 byEnctype, TVideoEncParam& TEncparam);
};

#endif //_VIDEO_MPWCFG_H_
