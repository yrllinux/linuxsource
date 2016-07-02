 /*****************************************************************************
   模块名      : KDVMT
   文件名      : mtstruc.h
   相关文件    : kdvmt.mib
   文件实现功能: KDVMT struct
   作者        : 李屹
   版本        : V0.9  Copyright(C) 2001-2002 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   2002/02/18  0.9         李屹        创建
   2002/07/29  1.0         李屹        新方案调整
   2003/07/15  1.1         jianghy     方案调整
   2003/11/05  3.0         jianghy     3.0实现
******************************************************************************/
#if !defined( _MT_STRUC_H_ )
#define _MT_STRUC_H_ 

#include "kdvsys.h"
#include "mtleaf.h"

/* config file name*/
#define FILENAME_MTCFG      (LPCSTR)"mtcfg.ini"

/* maximum alarm number in MCU */
#define MAXNUM_MT_E1                    (u8   )4
#define MAXNUM_MT_ALARM                 MAXNUM_MT_E1 * 6 + 2

/* index length */
#define MTSYS_ALARM_TBL_INDEX_LEN		OID_ID_LEN
#define MTNET_TRAPRCV_TBL_INDEX_LEN		OID_IP_ADDR_LEN
#define MTNET_PERIEQP_TBL_INDEX_LEN		OID_ID_LEN
#define MTEQP_E1_TBL_INDEX_LEN			OID_ID_LEN
#define MTEQP_TS_TBL_INDEX_LEN			OID_ID_LEN * 2
#define MTNET_ETHERNET_CARD_TBL_INDEX_LEN OID_ID_LEN
#define MTNET_IP_ROUTE_TBL_INDEX_LEN		OID_IP_ADDR_LEN
#define MTEQP_CAMERA_TBL_INDEX_LEN			OID_ID_LEN
#define MTEQP_MATRIX_TBL_INDEX_LEN			OID_ID_LEN
#define MTPFM_ALARM_TBL_INDEX_LEN			OID_ID_LEN
#define MTNET_MAP_TBL_INDEX_LEN		OID_ID_LEN


/*---------------------------------------------------------
*本地信息结构
*--------------------------------------------------------*/
typedef struct 
{
    char            mtAlias[MAXSIZE_mtAlias + 1];            /*终端别名*/
	char			mtE164Number[MAXSIZE_mtE164Number + 1];  /*终端号码*/
	UCHAR			mtType;                                  /*终端类型*/
	UCHAR           mtPosition;                              /*终端地位*/
}STRUCT_mtLocalInfo, STRUCT_mtLocalInfo_mem;


/*---------------------------------------------------------
*网络配置部分
*--------------------------------------------------------*/
typedef struct 
{
	UCHAR			mtnetNmIpIndex;       /*终端网管IP索引*/
	UCHAR			mtnetVcIpIndex;       /*终端VC IP索引*/
	UCHAR			mtnetGwIpIndex;	      /*终端GW IP索引*/
	u32			mtnetGKIpAddr;	      /*GateKeeper的IP地址*/
	u32			mtnetBandwidth;		  /*网络带宽(单位kbps)*/
	u16 			mtnetRcvStartPort;    /*终端接收起始端口*/
	u16 			mtnetSendStartPort;	  /*终端发送起始端口*/
}STRUCT_mtNetwork,STRUCT_mtNetwork_mem;


/*---------------------------------------------------------
*网卡的相关配置，可能不要
*--------------------------------------------------------*/
typedef struct
{
	UCHAR			mtnetEthernetCardEntIndex;		/*网卡的索引*/
	u32			mtnetEthernetCardEntIpAddr;		/*网卡的IP地址*/
	u32			mtnetEthernetCardEntMask;		/*网卡的子网掩码*/
	PHYADDR_STRUCT	mtnetEthernetCardEntPhyAddr;	/*网卡的物理地址*/
	UCHAR			mtnetEthernetCardEntExist;		/*是否使用该网卡*/
}STRUCT_mtnetEthernetCardEntry, STRUCT_mtnetEthernetCardEntry_mem;


/*---------------------------------------------------------
*接收TRAP陷阱的服务器表
*--------------------------------------------------------*/
typedef struct
{
    u32           mtnetTrapRcvEntIpAddr;			/*TRAP服务器IP地址*/
    char            mtnetTrapRcvEntCommunity[32]; /*COMMUNITY*/
	UCHAR			mtnetTrapRcvEntExist;			/*是否使用该表项*/
} STRUCT_mtnetTrapRcvEntry,STRUCT_mtnetTrapRcvEntry_mem;

typedef struct
{
	u32 mtnetTrapEntryAddIp;
	char  mtnetTrapEntryAddCommunity[32];
	UCHAR mtnetTrapEntryAddEnable;
}STRUCT_mtnetTrapRcvTable;

/*---------------------------------------------------------
*摄像机配置
*--------------------------------------------------------*/
typedef struct
{
	UCHAR	mteqpCameraEntId;			/*摄象机编号*/
	UCHAR	mteqpCameraEntType;			/*摄象机类型(1:SONY摄像机;2:PELCO摄像机;3:SAMSUNG摄象机)*/
	u32	mteqpCameraEntSvrIpAddr;	/*摄象机对应的外设服务器IP地址(若无外设服务器则为0.0.0.0)*/
	u16 	mteqpCameraEntPort;			/*摄象机对应(控制)串口号*/
}STRUCT_mteqpCameraEntry_mem;

typedef struct
{
	UCHAR	mteqpCameraEntId;			/*摄象机编号*/
	UCHAR	mteqpCameraEntType;			/*摄象机类型(1:SONY摄像机;2:PELCO摄像机;3:SAMSUNG摄象机)*/
	u32	mteqpCameraEntSvrIpAddr;	/*摄象机对应的外设服务器IP地址(若无外设服务器则为0.0.0.0)*/
	u16 	mteqpCameraEntPort;			/*摄象机对应(控制)串口号*/
	UCHAR	mteqpCameraEntExist;		/*是否使用该项*/
}STRUCT_mteqpCameraEntry;

/*---------------------------------------------------------
*音视频矩阵配置
*--------------------------------------------------------*/
typedef struct
{
	UCHAR	mteqpMatrixEntId;			/*矩阵编号*/
	UCHAR	mteqpMatrixEntType;			/*矩阵类型(现在为1)*/
	u32	mteqpMatrixEntSvrIpAddr;	/*矩阵对应的外设服务器IP地址(若无外设服务器则为0.0.0.0)*/
	u16 	mteqpMatrixEntPort;			/*矩阵对应(控制)串口号*/
}STRUCT_mteqpMatrixEntry_mem;

typedef struct
{
	UCHAR	mteqpMatrixEntId;			/*矩阵编号*/
	UCHAR	mteqpMatrixEntType;			/*矩阵类型(现在为1)*/
	u32	mteqpMatrixEntSvrIpAddr;	/*矩阵对应的外设服务器IP地址(若无外设服务器则为0.0.0.0)*/
	u16 	mteqpMatrixEntPort;			/*矩阵对应(控制)串口号*/
	UCHAR	mteqpMatrixEntExist;		/*是否使用该项*/
}STRUCT_mteqpMatrixEntry;

/*---------------------------------------------------------
*网络编码器配置
*--------------------------------------------------------*/
typedef struct
{
	UCHAR	mteqpNetEncoderId;			/*网络编码器的设备编号*/
}STRUCT_mteqpNetEncoder,STRUCT_mteqpNetEncoder_mem;


/*---------------------------------------------------------
*视频配置部分
*--------------------------------------------------------*/
typedef struct 
{
	UCHAR	mtVideoEncSource;		/*视频源 1:COMP  2:S-VIDEO*/
	UCHAR	mtVideoEncSystem;		/*视频制式 1:PAL  2:NTSC (暂不可配置)*/
	UCHAR	mtVideoEncFormat;		/*视频编码格式 1:h2611  2:h263   3:mpeg4*/
	u32	mtVideoEncBitRate;		/*视频编码码率(单位：kbps)*/
	UCHAR	mtVideoEncResolution;	/*视频采集宽度*/
	UCHAR	mtVideoEncFrameRate;	/*视频采集帧率*/
	UCHAR	mtVideoEncIKeyRate;		/*视频压缩最大关键帧间隔*/
	UCHAR	mtVideoEncQualMaxValue;	/*视频压缩最大量化参数(1-31)*/
	UCHAR	mtVideoEncQualMinValue;	/*视频压缩最小量化参数(1-31)*/
	UCHAR	mtVideoEncImageQuality; /*视频图象质量(1-5)*/
}STRUCT_mtVideo, STRUCT_mtVideo_mem;


/*---------------------------------------------------------
*音频配置部分
*--------------------------------------------------------*/
typedef struct 
{
	UCHAR	mtAudioEncSource;	/*音频源   1:Line  2:Mic  (暂不可配置)*/
	UCHAR	mtAudioEncFormat;	/*音频编码格式	mp3(1-4)   5:g711u  6:g711a  7:g723  8:g729   9:g722*/
	UCHAR	mtAudioEncVolume;	/*编码音量 (范围0-255)*/
	UCHAR	mtAudioAEC;			/*回声抵消	1 开启  2 关闭*/
}STRUCT_mtAudio, STRUCT_mtAudio_mem;


/*---------------------------------------------------------
*显示配置部分
*--------------------------------------------------------*/
typedef struct 
{
	UCHAR mtDisplayLabelOnOff;		/*是否显示台标 1 显示 2 不显示*/
	UCHAR mtDisplayLabelMode;		/*台标显示模式*/
	UCHAR mtDisplayPIPMode;         /*画中画功能 1:自动开启  2:手动开启*/
}STRUCT_mtDisplay, STRUCT_mtDisplayOption_mem;


/*---------------------------------------------------------
*呼叫配置部分
*--------------------------------------------------------*/
typedef struct 
{
	UCHAR mtAnswerMode;			/*接受呼叫方式 1:自动 2:手动  3:免打扰*/
	UCHAR mtCallMode;			/*呼叫方式     1:自动 2:手动 */
	u32 mtAutoCallIpAddr;		/*自动呼叫的IP地址，设置成自动呼叫时使用*/
}STRUCT_mtCall, STRUCT_mtCallConfig_mem;


/*---------------------------------------------------------
*系统组
*--------------------------------------------------------*/
typedef struct
{
    u16     mtsysState;												/*运行状态*/
    char    mtsysTime[SIZE_mtsysTime + 1];							/*当前时间*/
    char    mtsysFtpFileName[MAXSIZE_mtsysFtpFileName + 1];			/*FTP文件名*/
    char    mtsysConfigVersion[SIZE_mtsysConfigVersion + 1];		/*配置版本*/
	char	mtsysSoftwareVersion[MAXSIZE_mtsysSoftwareVersion + 1];	/*软件版本*/
} STRUCT_mtSystem,STRUCT_mtSystem_mem;    


/*---------------------------------------------------------
*性能组
*--------------------------------------------------------*/
typedef struct
{
    u16     mtpfmAlarmStamp;	/*告警戳，增加删除告警时加1*/
}STRUCT_mtPerformance;


/*---------------------------------------------------------
*MIB使用的告警结构
*--------------------------------------------------------*/
typedef struct
{
    u32   mtpfmAlarmEntSerialNo;   /*告警的编号*/
    u32   mtpfmAlarmEntAlarmCode;  /*告警码*/
    UCHAR   mtpfmAlarmEntObjType;    /*告警对象类型*/
    UCHAR   mtpfmAlarmEntObject[3];  /*告警对象*/
    char    mtpfmAlarmEntTime[SIZE_mtpfmAlarmEntTime + 1];  /*告警产生时间*/
} STRUCT_mtpfmAlarmEntry; 


/*---------------------------------------------------------
*内存中的告警结构
*--------------------------------------------------------*/
typedef struct
{
    u32   mtpfmAlarmEntAlarmCode;		/*告警码*/
    UCHAR   mtpfmAlarmEntObjType;		/*告警对象类型*/
    UCHAR   mtpfmAlarmEntObject[3];		/*告警对象*/
    char    mtpfmAlarmEntTime[SIZE_mtpfmAlarmEntTime + 1];  /*告警产生时间*/
    BOOL  	bExist;						/*该告警是否有效*/
} STRUCT_mtpfmAlarmEntry_mem;     


/*---------------------------------------------------------
*编码器统计信息
*--------------------------------------------------------*/
typedef struct
{
	u8    mtpfmEncoderVideoFrameRate;   /*视频帧率*/
	u32 mtpfmEncoderVideoBitRate;    /*视频码流速度*/
	u32 mtpfmEncoderAudioBitRate;    /*语音码流速度*/
	u32 mtpfmEncoderVideoPackLose;   /*视频丢帧数*/
	u32 mtpfmEncoderVideoPackError;  /*视频错帧数*/
	u32 mtpfmEncoderAudioPackLose;   /*语音丢帧数*/
	u32 mtpfmEncoderAudioPackError;  /*语音错帧数*/
}STRUCT_mtpfmEncoderStatistics;


/*---------------------------------------------------------
*解码器统计信息
*--------------------------------------------------------*/
typedef struct
{
	u8    mtpfmDecoderVideoFrameRate;   /*视频解码帧率*/
	u16  mtpfmDecoderVideoBitRate;     /*视频解码码率*/
	u16  mtpfmDecoderAudioBitRate;     /*语音解码码率*/
	u32 mtpfmDecoderVideoRecvFrame;  /*收到的视频帧数*/
	u32 mtpfmDecoderAudioRecvFrame;  /*收到的语音帧数*/
	u32 mtpfmDecoderVideoLoseFrame;  /*丢失的视频帧数*/
	u32 mtpfmDecoderAudioLoseFrame;  /*丢失的语音帧数*/
	u32 mtpfmDecoderPackError;       /*乱帧数*/
	u32 mtpfmDecoderIndexLose;       /*序号丢帧*/
	u32 mtpfmDecoderSizeLose;        /*大小丢帧*/
	u32 mtpfmDecoderFullLose;        /*满丢帧*/
}STRUCT_mtpfmDecoderStatistics;


/*---------------------------------------------------------
*风扇转速统计信息
*--------------------------------------------------------*/
typedef struct 
{
	u32 mtpfmDSP1FanSpeed;    /*图像编码风扇转速*/
	u32 mtpfmDSP2FanSpeed;	/*图像解码风扇转速*/
	u32 mtpfmDSP3FanSpeed;	/*音频编解码风扇转速*/
	u32 mtpfmSYS1FanSpeed;	/*机箱风扇1转速*/
	u32 mtpfmSYS2FanSpeed;	/*机箱风扇2转速*/
}STRUCT_mtpfmFanSpeedStatistics;

/*---------------------------------------------------------
*风扇转速统计信息
*--------------------------------------------------------*/
typedef struct
{
	UCHAR mtpfmLedState[8];
}STRUCT_mtpfmPanelLed;

#endif /* _MT_STRUC_H_ */
/* end of file mtstruc.h */
