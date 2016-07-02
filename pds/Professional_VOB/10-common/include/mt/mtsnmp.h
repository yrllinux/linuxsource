#include "mttype.h"



//终端型号
const u32  MTSYSHARDWARETYPE_UNKNOWNMTMODEL   = (u32)emUnknownMtModel ;
const u32  MTSYSHARDWARETYPE_PCMT             = (u32)emPCMT ;//桌面终端
const u32  MTSYSHARDWARETYPE_8010             = (u32)em8010 ;
const u32  MTSYSHARDWARETYPE_8010A            = (u32)em8010A ;
const u32  MTSYSHARDWARETYPE_8010APLUS        = (u32)em8010Aplus ;//8010A+
const u32  MTSYSHARDWARETYPE_8010C            = (u32)em8010C ;
const u32  MTSYSHARDWARETYPE_IMT              = (u32)emIMT ;
//xjx_080228
const u32  MTSYSHARDWARETYPE_8010A_2            = (u32)em8010A_2 ;
const u32  MTSYSHARDWARETYPE_8010A_4            = (u32)em8010A_4 ;
const u32  MTSYSHARDWARETYPE_8010A_8            = (u32)em8010A_8 ;
const u32  MTSYSHARDWARETYPE_8010C1            = (u32)em8010C1 ;

const u32  MTSYSHARDWARETYPE_7210             = (u32)em7210 ;
const u32  MTSYSHARDWARETYPE_7610             = (u32)em7610 ;
const u32  MTSYSHARDWARETYPE_5610             = (u32)emTS5610 ;
const u32  MTSYSHARDWARETYPE_6610             = (u32)emTS6610 ;
const u32  MTSYSHARDWARETYPE_7810             = (u32)em7810 ;
const u32  MTSYSHARDWARETYPE_7910             = (u32)em7910 ;
const u32  MTSYSHARDWARETYPE_7620_2           = (u32)em7620_B ;
const u32  MTSYSHARDWARETYPE_7620_4           = (u32)em7620_A ;

const u32  MTSYSHARDWARETYPE_6610E            = (u32)emTS6610E;
const u32  MTSYSHARDWARETYPE_6210             = (u32)emTS6210;
const u32  MTSYSHARDWARETYPE_5210             = (u32)em8220B;
const u32  MTSYSHARDWARETYPE_3210             = (u32)em8620A;
const u32  MTSYSHARDWARETYPE_V5               = (u32)em8220C;
const u32  MTSYSHARDWARETYPE_7820_A           = (u32)em7820_A ;
const u32  MTSYSHARDWARETYPE_7820_B           = (u32)em7820_B ;
const u32  MTSYSHARDWARETYPE_7920_A           = (u32)em7920_A ;
const u32  MTSYSHARDWARETYPE_7920_B           = (u32)em7920_B ;

const u32  MTUSED    = TRUE;
const u32  MTNOTUSED = FALSE;

//TOS 类型	
const u32  MTQOSTOS_TOSNONE           = (u32)emTOSNone;//不使用 ; 
const u32  MTQOSTOS_TOSMINCOST        = (u32)emTOSMinCost ; //最小开销
const u32  MTQOSTOS_TOSMAXRELIABLE    = (u32)emTOSMaxReliable ;// 最高可靠性
const u32  MTQOSTOS_TOSMAXTHRUPUT     = (u32)emTOSMaxThruput ; //最大吞吐量
const u32  MTQOSTOS_TOSMINDELAY       = (u32)emTOSMinDelay ;//最小延迟

//E1验证类型
const u32  MTE1CONFIGAUTHENTICATIONTYPE_PAP    = (u32)emPAP ;
const u32  MTE1CONFIGAUTHENTICATIONTYPE_CHAP   = (u32)emCHAP ;

//E1链路检测类型
const u32  MTE1UNITPROTOCOL_PPP                = (u32)emPPP ;
const u32  MTE1UNITPROTOCOL_HDLC               = (u32)emHDLC ;
const u32  MTE1UNITPROTOCOL_PPPOE              = (u32)emPPPOE ;
const u32  MTE1UNITPROTOCOL_MP                 = (u32)emMP ;

//串口类型
const u32  MTCAMERACFGSERIALTYPE_RS232           = (u32)emRS232 ;
const u32  MTCAMERACFGSERIALTYPE_RS422           = (u32)emRS422 ;
const u32  MTCAMERACFGSERIALTYPE_RS485           = (u32)emRS485 ;
const u32  MTCAMERACFGSERIALTYPE_SERIALTCPIP     = (u32)emSerialTcpip ;

//串口校验位类型
const u32  MTSERIALCFGCHECK_NOCHECK              = (u32)emNoCheck ;//无校验
const u32  MTSERIALCFGCHECK_ODDCHECK             = (u32)emOddCheck ;//奇校验
const u32  MTSERIALCFGCHECK_EVENCHECK            = (u32)emEvenCheck ;//偶校验

//串口停止位类型
const u32  MTSERIALCFGSTOPBITS_1STOPBIT          = (u32)em1StopBit ;
const u32  MTSERIALCFGSTOPBITS_1HALFSTOPBITS     = (u32)em1HalfStopBits ;
const u32  MTSERIALCFGSTOPBITS_2STOPBITS         = (u32)em2StopBits ;

//视频制式类型
const u32  MTVIDEO1INVIDEOSTANDARD_PAL           = (u32)emPAL ;
const u32  MTVIDEO1INVIDEOSTANDARD_NTSC          = (u32)emNTSC ;

//视频协议类型
const u32  MTVIDEO1PRIORSTRATEGY_VH261              = (u32)emVH261 ;
const u32  MTVIDEO1PRIORSTRATEGY_VH262              = (u32)emVH262 ;//MPEG2
const u32  MTVIDEO1PRIORSTRATEGY_VH263              = (u32)emVH263 ;
const u32  MTVIDEO1PRIORSTRATEGY_VH263PLUS          = (u32)emVH263plus ;
const u32  MTVIDEO1PRIORSTRATEGY_VH264              = (u32)emVH264 ;
const u32  MTVIDEO1PRIORSTRATEGY_VMPEG4             = (u32)emVMPEG4 ;
const u32  MTVIDEO1PRIORSTRATEGY_VEnd               = (u32)emVEnd ;

//分辨率类型
//const u32  MTVIDEO1RESOLUTION_VRESOLUTIONAUTO = (u32)emVResolutionAuto ;
//const u32  MTVIDEO1RESOLUTION_VQCIF           = (u32)emVQCIF ;
//const u32  MTVIDEO1RESOLUTION_VCIF            = (u32)emVCIF ;
//const u32  MTVIDEO1RESOLUTION_V2CIF           = (u32)emV2CIF ;
//const u32  MTVIDEO1RESOLUTION_V4CIF           = (u32)emV4CIF ;
//const u32  MTVIDEO1RESOLUTION_V16CIF          = (u32)emV16CIF ;
//const u32  MTVIDEO1RESOLUTION_VGA352240       = (u32)emVGA352240 ;
//const u32  MTVIDEO1RESOLUTION_VGA704480       = (u32)emVGA704480 ;
//const u32  MTVIDEO1RESOLUTION_VGA640480       = (u32)emVGA640480 ;
//const u32  MTVIDEO1RESOLUTION_VGA800600       = (u32)emVGA800600 ;
//const u32  MTVIDEO1RESOLUTION_VGA1024768      = (u32)emVGA1024768 ;
// const u32  MTVIDEO1RESOLUTION_VRESEND         = (u32)emVResEnd ;

//终端视频端口
const u32  MTVIDEO1SOURCE_MTVGA                  = (u32)emMtVGA ;//VGA 
const u32  MTVIDEO1SOURCE_MTSVID                 = (u32)emMtSVid ;//S 端子
const u32  MTVIDEO1SOURCE_MTPC                   = (u32)emMtPC ;//PC
const u32  MTVIDEO1SOURCE_MTC1VID                = (u32)emMtC1Vid ;//C1端子
const u32  MTVIDEO1SOURCE_MTC2VID                = (u32)emMtC2Vid ;//C2端子
const u32  MTVIDEO1SOURCE_MTC3VID                = (u32)emMtC3Vid ;//C3端子
const u32  MTVIDEO1SOURCE_MTC4VID                = (u32)emMtC4Vid ;//C4端子
const u32  MTVIDEO1SOURCE_MTC5VID                = (u32)emMtC5Vid ;//C5端子
const u32  MTVIDEO1SOURCE_MTC6VID                = (u32)emMtC6Vid ;//C6端子

//音频协议类型
const u32  MTAUDIOENCFORMAT_AG711A                = (u32)emAG711a ;
const u32  MTAUDIOENCFORMAT_AG711U                = (u32)emAG711u ;
const u32  MTAUDIOENCFORMAT_AG722                 = (u32)emAG722 ;
const u32  MTAUDIOENCFORMAT_AG7231                = (u32)emAG7231 ;
const u32  MTAUDIOENCFORMAT_AG728                 = (u32)emAG728 ;
const u32  MTAUDIOENCFORMAT_AG729                 = (u32)emAG729 ;
const u32  MTAUDIOENCFORMAT_AMP3                  = (u32)emAMP3 ;
const u32  MTAUDIOENCFORMAT_AEND                  = (u32)emAEnd ;

//加密算法
const u32  MTLOCALINFOENCRYARITHMETIC_ENCRYPTNONE      = (u32)emEncryptNone ;
const u32  MTLOCALINFOENCRYARITHMETIC_DES              = (u32)emDES ;
const u32  MTLOCALINFOENCRYARITHMETIC_AES              = (u32)emAES ;
const u32  MTLOCALINFOENCRYARITHMETIC_ENCRYPTAUTO      = (u32)emEncryptAuto ;

//语言类型
const u32  MTLOCALINFOLANGUAGE_ENGLISH             = (u32)emEnglish ;
const u32  MTLOCALINFOLANGUAGE_CHINESESB           = (u32)emChineseSB ;

//台标类型
const u32  MTDISPLAYCFGSYMBOLSHOWMODE_LABELAUTO      = (u32)emLabelAuto ;//自动
const u32  MTDISPLAYCFGSYMBOLSHOWMODE_LABELUSERDEF   = (u32)emLabelUserDef ;//自定义
const u32  MTDISPLAYCFGSYMBOLSHOWMODE_LABELOFF       = (u32)emLabelOff ; //关闭

//双流显示模式
const u32  MTDISPLAYCFGDUALVIDEOSHOWMODE_DUALVIDEOONSIGLESCREEN   = (u32)emDualVideoOnSigleScreen ;//单屏双显
const u32  MTDISPLAYCFGDUALVIDEOSHOWMODE_EACHVIDEOOWNSCREEN       = (u32)emEachVideoOwnScreen ;//双屏双显

//应答方式类型
const u32  MTCALLCFGANSWERMODE_TRIPMODENEGATIVE         = (u32)emTripModeNegative ;//拒绝、关闭
const u32  MTCALLCFGANSWERMODE_TRIPMODEAUTO             = (u32)emTripModeAuto ;//自动
const u32  MTCALLCFGANSWERMODE_TRIPMODEMANU             = (u32)emTripModeManu ;//手动



//////////////////////////告警信息///////////////////////////////////////////
//系统状态
const u32  MTSYSSTATE_RUNNING   = 1;//系统运行
const u32  MTSYSSTATE_REBOOT	= 2;//系统重起
const u32  MTSYSSTATE_DOWN   = 3;//系统断电




//E1线路状态改变,共分为4组
#define GETE1NUM4INFO(dwVal) (u8)(dwVal & 0xFF)
#define GETE1NUM3INFO(dwVal) (u8)((dwVal >> 8) & 0xFF)
#define GETE1NUM2INFO(dwVal) (u8)((dwVal >> 16) & 0xFF)
#define GETE1NUM1INFO(dwVal) (u8)((dwVal >> 24) & 0xFF)
//每组的含义如下,相应位为1则告警，为0则正常
const u8 MTE1_NORMAL = 0;
const u8 MTE1_TRAP   = 1;
// E1全1告警   
#define GET_MTE1LINE_ALL1(byVal) (u8)(byVal & 0x1) 
//E1失同步告警 
#define GET_MTE1LINE_UNSYNCHRONIZATION(byVal) (u8)(byVal &0x2)
//E1远端告警
#define GET_MTE1LINE_REMOTEALARM(byVal) (u8)(byVal & 0x4)
//E1失载波告警
#define GET_MTE1LINE_UNCARRIERWAVE(byVal) (u8)(byVal & 0x8)
//E1的CRC复帧错误告警
#define GET_MTE1LINE_UNCRCERROR(byVal) (u8)(byVal & 0x10)
//E1随路复帧错误告警
#define GET_MTE1LINE_RMERROR(byVal) (u8)(byVal & 0x20)



//任务状态改变
//高16位为任务id号
#define GETTASKID(dwVal) (u16)(dwVal >> 16)
//任务状态,为1则任务忙，为0则正常
const u16 MTTASK_RUNNING   =  1;//任务忙
const u16 MTTASK_STOP      =  0;//任务正常
#define GETTASKSTATUS(dwVal) (u16)(dwVal & 0xFFFF)



// MAP状态改变
//map号
#define GET_MAPID(dwVal) (u8)(dwVal >> 24)
//MAP状态
const u8 MTMAP_NATURAL    = 0;//正常
const u8 MTMAP_ABNOMIT    = 1;//死机
#define GET_MAPSTATUS(dwVal) (u8)((dwVal >> 16) & 0xFF)
// 低16位表示该MAP死机次数
#define GET_MAPSTOPTIMES(dwVal) (u16)(dwVal & 0xFFFF)


//以太网状态改变 
//获取网卡号
#define GET_ETHERNETID(dwVal) (u16)(dwVal >> 16)
//以太网状态，0 正常，1网线断开
const u16  MTETHERNET_NORMAL = 0;
const u16  MTETHERNET_DOWN   = 1;
#define GET_ETHERNETSTATUS(dwVal) (u16)(dwVal & 0xFFFF)


// 风扇状态改变
//获取风扇号
#define GET_FAINID(dwVal) (u8)(dwVal >> 16)
//风扇状态，响应位如果为1则表示正常，0则表示异常
const u8 MTTASKNOMAL = 1;
const u8 MTTASKABNOMAL = 0;
#define GET_FAIN_STATUS(dwVal) (u8)(dwVal & 0xFFFF)


// V35状态改变
//1为报警，0为正常
const u8 MTV35_TRAP = 1;
const u8 MTV35_NORMAL = 0;
//Carrier Detect, 载波检测失败
#define GET_MTV35_CARRIERDETECTDOWN(dwVal) (u8)(dwVal & 0x1)
//Clear To Send, 清除发送失败
#define GET_MTV35_CLEARTOSEND(dwVal)       (u8)(dwVal & 0x2)
//Request To Send, 请求发送失败
#define GET_MTV35_REQUESTTOSEND(dwVal)     (u8)(dwVal & 0x4)
//数据终端未就绪
#define GET_MTV35_DATAMTUNREADY(dwVal)     (u8)(dwVal & 0x8)
//数据未准备好
#define GET_MTV35_DATAUNREADY(dwVal)       (u8)(dwVal & 0x10)


// -------- 板子温度状态改变所需接口、定义 ---------
#define MT_BRD_TEMP_TRAP           (u8)1
#define MT_BRD_TEMP_NORMAL         (u8)0 
#define GET_BRD_TEMP_STATUS(dwVal) (u8)(dwVal & 0xFFFF)
// -------- 板子电压状态改变所需接口、定义 ---------
#define MT_BRD_VOLT_TRAP           (u8)1
#define MT_BRD_VOLT_NORAML         (u8)0
#define GET_BRD_VOLT_STATUS(dwVal) (u8)(dwVal & 0xFFFF)


//面板灯状态通知
//指示灯闪烁状态
const u8 BRD_LED_NOT_EXIST   =              0;      /* 不存在 */
const u8 BRD_LED_STATE_ON    =              1;      /* 亮 */
const u8 BRD_LED_STATE_OFF   =              2;      /* 灭 */
const u8 BRD_LED_STATE_QUICK =              3;      /* 快闪 */
const u8 BRD_LED_STATE_SLOW  =              4;      /* 慢闪 */

//面板灯
//保留老接口
#define GET_PANELLED_POWER(dwVal) (u8)((0x3   &  dwVal           ) + 1)
#define GET_PANELLED_ALM(dwVal)   (u8)((0x3   &  (dwVal >> 2)    ) + 1)
#define GET_PANELLED_RUN(dwVal)   (u8)((0x3   &  (dwVal >> 4)    ) + 1)
#define GET_PANELLED_LINK(dwVal)  (u8)((0x3   &  (dwVal >> 6)    ) + 1)
#define GET_PANELLED_DSP1(dwVal)  (u8)((0x3   &  (dwVal >> 8)    ) + 1)
#define GET_PANELLED_DSP2(dwVal)  (u8)((0x3   &  (dwVal >> 10)   ) + 1)
#define GET_PANELLED_DSP3(dwVal)  (u8)((0x3   &  (dwVal >> 12)   ) + 1)
#define GET_PANELLED_ETH1(dwVal)  (u8)((0x3   &  (dwVal >> 14)   ) + 1)
#define GET_PANELLED_ETH2(dwVal)  (u8)((0x3   &  (dwVal >> 16)   ) + 1)
#define GET_PANELLED_ETH3(dwVal)  (u8)((0x3   &  (dwVal >> 18)   ) + 1)
#define GET_PANELLED_IR(dwVal)    (u8)((0x3   &  (dwVal >> 20)   ) + 1)

//面板灯新接口
enum EmLedComponent
{
	emLedStatusPower = 0,
	emLedStatusAlarm,
	emLedStatusRun,
	emLedStatusLink,
	emLedStatusDSP1,
	emLedStatusDSP2,
	emLedStatusDSP3,     // 8010C此项不存在
	emLedStatusETH1,
	emLedStatusIR,
	emLedStatusEnd
};

//T2的LED面板灯新接口
//add by xujinxing,2007-5-9
enum  EmT2LedComponent
{
	emT2LedStatusGreen =0,
	emT2LedStatusOrange,
	emT2LedStatusEnd
};

inline u8 mtGetLedStatus(EmLedComponent emLedStatus, const s8* pbyBuf)
{
	if ( strlen(pbyBuf) > emLedStatusEnd + 1)
	{
		return BRD_LED_NOT_EXIST;
	}
	u8 byRel = pbyBuf[emLedStatus] - '0';
	return byRel;
}

//设备连接状态改变
//设备类型（待定）
#define GETEQPTYPE(dwVal) (u8)(dwVal >> 24)
//设备编号
#define GETEQPNUM(dwVal) (u8)((dwVal >> 16) & 0xFF)
//低16位表示设备状态 1 在线 2不在线 3不知道
const u32 MTEQPCONN_ONLINE   = 1;
const u32 MTEQPCONN_UNONLINE = 2;
const u32 MTEQPCONN_UNKNOWN  = 3;
#define GETEQPSTATUS(dwVal) (u16)(dwVal & 0xFFFF)

// CCI状态改变
//MAP号
#define GETMAPID(dwVal)   (u8)(dwVal >> 24)
//updwon状态
//0 上行 1 下行
const u8 MTCCI_UP = 0;
const u8 MTCCI_DWON = 1;
#define GETUPDOWN(dwVal)  (u8)((dwVal >> 16) & 0xFF)
//队列号
#define GETQUEUEID(dwVal) (u8)((dwVal >> 8) & 0xFF)
//队列状态
// 0 正常  1 满
const u8 MTCCI_NORMOL = 0;
const u8 MTCCI_FULL   = 1;
#define GETSTATUS(dwVal)  (u8)(dwVal & 0xFF)


// 媒体流状态
//媒体类型(待定）
#define GETMEDIATYPE(dwVal) (u16)(dwVal >> 16)
//流状态 0 接收无码流 1 发送丟包
const u16 MTMEDIARCVNOCODESTREAM = 0;
const u16 MTMEDIASNDLOSSPACKEGE  = 1;
#define GETMEDIASTREAMSTATUS(dwVal) (u16)(dwVal & 0xFFFF)

// 内存状态改变
/*
		待定
*/

// 文件系统状态改变
/*
		待定
*/

// 视频源状态改变
/*
		待定
*/






//以下为没有实现的

/*
//const u32  MTEQPMATRIXENTRY                = 1 ;       //"1.3.6.1.4.1.8888.1.2.1.3.3.1"
const u32  MTEQPMATRIXENTID                = 96 ;       //"1.3.6.1.4.1.8888.1.2.1.3.3.1.1"
const u32  MTEQPMATRIXENTTYPE              = 97 ;       //"1.3.6.1.4.1.8888.1.2.1.3.3.1.2"
const u32  MTEQPMATRIXENTSVRIPADDR         = 98 ;       //"1.3.6.1.4.1.8888.1.2.1.3.3.1.3"
const u32  MTEQPMATRIXENTPORT              = 99 ;       //"1.3.6.1.4.1.8888.1.2.1.3.3.1.4"
const u32  MTEQPMATRIXENTEXIST             = 100 ;      //"1.3.6.1.4.1.8888.1.2.1.3.3.1.5"

const u32  MTEQPNETENCODERID               = 102 ;      //"1.3.6.1.4.1.8888.1.2.1.3.4.1.0"
*/

/*


const u32  MTPFMENCODERVIDEOFRAMERATE      = 158 ;      //"1.3.6.1.4.1.8888.1.2.1.5.3.1.1.0"
const u32  MTPFMENCODERVIDEOBITRATE        = 159 ;      //"1.3.6.1.4.1.8888.1.2.1.5.3.1.2.0"
const u32  MTPFMENCODERAUDIOBITRATE        = 160 ;      //"1.3.6.1.4.1.8888.1.2.1.5.3.1.3.0"
const u32  MTPFMENCODERVIDEOPACKLOSE       = 161 ;      //"1.3.6.1.4.1.8888.1.2.1.5.3.1.4.0"
const u32  MTPFMENCODERVIDEOPACKERROR      = 162 ;      //"1.3.6.1.4.1.8888.1.2.1.5.3.1.5.0"
const u32  MTPFMENCODERAUDIOPACKLOSE       = 163 ;      //"1.3.6.1.4.1.8888.1.2.1.5.3.1.6.0"
const u32  MTPFMENCODERAUDIOPACKERROR      = 164 ;      //"1.3.6.1.4.1.8888.1.2.1.5.3.1.7.0"

const u32  MTPFMDECODERVIDEOFRAMERATE      = 166 ;      //"1.3.6.1.4.1.8888.1.2.1.5.3.2.1.0"
const u32  MTPFMDECODERVIDEOBITRATE        = 167 ;      //"1.3.6.1.4.1.8888.1.2.1.5.3.2.2.0"
const u32  MTPFMDECODERAUDIOBITRATE        = 168 ;      //"1.3.6.1.4.1.8888.1.2.1.5.3.2.3.0"
const u32  MTPFMDECODERVIDEORECVFRAME      = 169 ;      //"1.3.6.1.4.1.8888.1.2.1.5.3.2.4.0"
const u32  MTPFMDECODERAUDIORECVFRAME      = 170 ;      //"1.3.6.1.4.1.8888.1.2.1.5.3.2.5.0"
const u32  MTPFMDECODERVIDEOLOSEFRAME      = 171 ;      //"1.3.6.1.4.1.8888.1.2.1.5.3.2.6.0"
const u32  MTPFMDECODERAUDIOLOSEFRAME      = 172 ;      //"1.3.6.1.4.1.8888.1.2.1.5.3.2.7.0"
const u32  MTPFMDECODERPACKERROR           = 173 ;      //"1.3.6.1.4.1.8888.1.2.1.5.3.2.8.0"
const u32  MTPFMDECODERINDEXLOSE           = 174 ;      //"1.3.6.1.4.1.8888.1.2.1.5.3.2.9.0"
const u32  MTPFMDECODERSIZELOSE            = 175 ;      //"1.3.6.1.4.1.8888.1.2.1.5.3.2.10.0"
const u32  MTPFMDECODERFULLLOSE            = 176 ;      //"1.3.6.1.4.1.8888.1.2.1.5.3.2.11.0"



const u32  MTPFMDSP1FANSPEED               = 178 ;      //"1.3.6.1.4.1.8888.1.2.1.5.3.3.1.0"
const u32  MTPFMDSP2FANSPEED               = 179 ;      //"1.3.6.1.4.1.8888.1.2.1.5.3.3.2.0"
const u32  MTPFMDSP3FANSPEED               = 180 ;      //"1.3.6.1.4.1.8888.1.2.1.5.3.3.3.0"
const u32  MTPFMSYS1FANSPEED               = 181 ;      //"1.3.6.1.4.1.8888.1.2.1.5.3.3.4.0"
const u32  MTPFMSYS2FANSPEED               = 182 ;      //"1.3.6.1.4.1.8888.1.2.1.5.3.3.5.0"

const u32  MTPFMLEDSTATE                   = 185 ;      //"1.3.6.1.4.1.8888.1.2.1.5.4.1.0"


const u32                    = 233 ;      //"1.3.6.1.4.1.8888.1.2.3.8"

 */


