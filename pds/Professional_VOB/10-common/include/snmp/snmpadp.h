/*****************************************************************************
模块名      : SNMP适配模块
文件名      : snmpadp.h
相关文件    : snmpadp.cpp
文件实现功能: 
作者        : 李  博
版本        : V4.0  Copyright(C) 2005-2006 KDC, All rights reserved.
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2005/08/18  4.0         李  博      创建
******************************************************************************/ 

#ifndef _SNMPADP_H_
#define _SNMPADP_H_

#include "osp.h"
#include "snmpadp_nodes.h"

#define MAX_COMMUNITY_LEN                   32
#define MAXLEN_32							(int)32

#ifndef DEFAULT_WRITE_COMMUNITY
#define DEFAULT_WRITE_COMMUNITY	            "kdv123"
//#define DEFAULT_WRITE_COMMUNITY	        "private"
#endif
#ifndef DEFAULT_READ_COMMUNITY
#define DEFAULT_READ_COMMUNITY	            "public"
#endif


//SNMP++ General Errors Value Description
#define SNMP_SUCCESS                        (u16)0    //0  //Success Status
#define SNMP_GENERAL_ERROR                  (u16)1    //-1 //General Error
#define SNMP_RESOURCE_UNAVAIL               (u16)2    //-2 //New or malloc Failed
#define SNMP_INTERNAL_ERROR                 (u16)3    //-3 //Unexpected internal error
#define SNMP_UNSUPPORTED                    (u16)4    //-4 //Unsupported function
//Callback Reasons
#define SNMP_TIMEOUT                        (u16)10   //-5 //Outstanding request timed out
#define SNMP_ASYNC_RESPONSE                 (u16)11   //-6 //Received response
#define SNMP_NOTIFICATION                   (u16)12   //-7 //Received notification (trap/inform)
#define SNMP_SESSION_DESTROYED              (u16)13   //-8 //Snmp Object destroyed
//Snmp Class Errors
#define SNMP_INVALID                        (u16)20   //-10 //Snmp member function called on invalid instance
#define SNMP_INVALID_PDU                    (u16)21   //-11 //Invalid Pdu passed to mf
#define SNMP_INVALID_TARGET                 (u16)22   //-12 //Invalid target passed to mf
#define SNMP_INVALID_CALLBACK               (u16)23   //-13 //Invalid callback to mf
#define SNMP_INVALID_REQID                  (u16)24   //-14 //Invalid request id to cancel
#define SNMP_INVALID_NOTIFYID               (u16)25   //-15 //Missing trap/inform Oid
#define SNMP_INVALID_OPERATION              (u16)26   //-16 //Snmp operation not allowed for specified target
#define SNMP_INVALID_OID                    (u16)27   //-17 //Invalid Oid passed to mf
#define SNMP_INVALID_ADDRESS                (u16)28   //-18 //Invalid address passed to mf
#define SNMP_ERR_STATUS_SET                 (u16)29   //-19 //Agent returned response Pdu with error status set
#define SNMP_TL_UNSUPPORTED                 (u16)30   //-20 //Transport not supported
#define SNMP_TL_IN_USE                      (u16)31   //-21 //Transport in use
#define SNMP_TL_FAILED                      (u16)32   //-22 //Transport Failed


#define SNMP_ERR_TOO_BIG                    (u16)50   //1 Pdu Too Big, see error index
#define SNMP_ERR_NO_SUCH_NAME               (u16)51   //2 No Such Variable Binding name, see returned error index
#define SNMP_ERR_BAD_VALUE                  (u16)52   //3 Bad Variable Binding Value, see returned error index
#define SNMP_ERR_READ_ONLY                  (u16)53   //4 Variable Binding is read only, see returned error index
#define SNMP_ERR_GENERAL_VB_ERR             (u16)54   //5 General Variable Binding error, see returned error index
#define SNMP_ERR_NO_ACCESS                  (u16)55   //6 Operation Failure, No Access
#define SNMP_ERR_WRONG_TYPE                 (u16)56   //7 Operation Failure, Bad Type
#define SNMP_ERR_WRONG_LENGTH               (u16)57   //8 Operation Failure, Bad Length
#define SNMP_ERR_WRONG_ENCODING             (u16)58   //9 Operation Failure, Wrong Encoding
#define SNMP_ERR_WRONG_VALUE                (u16)59   //10 Operation Failure, Wrong Value
#define SNMP_ERR_NO_CREATION                (u16)60   //11 Operation Failure, No Creation
#define SNMP_ERR_INCONSIST_VAL              (u16)61   //12 Operation Failure, Inconsistent Value
#define SNMP_ERR_RESOURCE_UNAVAIL           (u16)62   //13 Operation Failure, Resource Unavailable
#define SNMP_ERR_COMITFAIL                  (u16)63   //14 Operation Failure, Commit Failure
#define SNMP_ERR_UNDO_FAIL                  (u16)64   //15 Operation Failure, Undo Failure
#define SNMP_ERR_AUTH_ERR                   (u16)65   //16 Operation Failure, Authorization Error
#define SNMP_ERR_NOT_WRITEABLE              (u16)66   //17 Operation Failure, Not Writable
#define SNMP_ERR_INCONSIS_NAME              (u16)67   //18 Operation Failure, Inconsistent Name


enum MibType
{
    MCUMIB = 1,
    MTMIB,
	CPSMIB,
	MPSMIB,
	APSMIB,
	COMMMIB,
	RCDMIB,
};

//被管设备操作系统版本号定义
#define SNMP_OS_TYPE_VXWORKS                (u8)1
#define SNMP_OS_TYPE_LINUX                  (u8)2
#define SNMP_OS_TYPE_WIN32                  (u8)3
#define SNMP_OS_TYPE_VX_RAWDISK             (u8)4    //VX裸分区
#define SNMP_OS_TYPE_LINUX_RH				(u8)5

//MCU子类型的snmp定义
#define SNMP_MCUSUBTYPE_UNKNOWN				(u8)0
#define SNMP_MCUSUBTYPE_8000A               (u8)1
#define SNMP_MCUSUBTYPE_8000B               (u8)2
#define SNMP_MCUSUBTYPE_8000C               (u8)3
#define SNMP_MCUSUBTYPE_8000G               (u8)4
#define SNMP_MCUSUBTYPE_8000H               (u8)5
#define	SNMP_MCUSUBTYPE_8000I				(u8)6
#define SNMP_MCUSUBTYPE_800L				(u8)7
#define SNMP_MCUSUBTYPE_8000H_M             (u8)8

//录播服务器snmp定义
#define SNMP_RCDSUBTYPE_UNKNOWN				(u8)0
#define	SNMP_RCDSUBTYPE_2000				(u8)1
#define SNMP_RCDSUBTYPE_2000C				(u8)2
#define SNMP_RCDSUBTYPE_2000E				(u8)3


//20121127sq
//用于传递补充的信息，该枚举放在mtmcu的LocalInfos中
enum EmLocalInfos
{
	emCharset = 1,
};
//文件上传状态
#define SNMP_FILE_UPDATE_INIT               (u8)0    //未更新过或更新刚开始
#define SNMP_FILE_UPDATE_INPROCESS          (u8)1    //正在上传更新中
#define SNMP_FILE_UPDATE_SUCCEED            (u8)2    //更新成功
#define SNMP_FILE_UPDATE_FAILED             (u8)3    //更新失败

#define MAXNUM_TRAP_ADDR                    (u8)16   //网管最大地址数
#define MAXNUM_ALARMTABLE_ROW               (u8)64   //SNMP一次获取的最大告警表项值

// 单板状态(NMS)
#define NMS_BOARD_OFFLINE                   (u8)0   // 0：断链
#define NMS_BOARD_ONLINE                    (u8)1   // 1：在线

/************************************************************************/
/* A = MibType(u8), B = TableRow(u8), C = NodeName        (u16)                 */
/* D = Constructed NodeName(u32)                                        */
/************************************************************************/
#define MAKE_NODENAME(A, C)  (((u32)(A) << 24) + ((u32)(C) & 0x0000FFFF))
#define MAKE_TABLE_NODENAME(A, B, C)  ((((u32)(B) << 16) & 0x00FF0000) + MAKE_NODENAME(A, C))

#define GET_MIB_TYPE(D)  ((u8)((D) >> 24))
#define GET_NODE_ROW(D)  ((u8)(((D) >> 16) & 0xFF))
#define GET_NODENAME(D)  ((D) & 0xFF00FFFF)

#define MAKE_MCU_NODENAME(C)  MAKE_NODENAME(MCUMIB, C)
#define MAKE_MCUTABLE_NODENAME(B, C)  MAKE_TABLE_NODENAME(MCUMIB, B, C)

#define MAKE_MT_NODENAME(C)  MAKE_NODENAME(MTMIB, C)
#define MAKE_MTTABLE_NODENAME(B, C)  MAKE_TABLE_NODENAME(MTMIB, B, C)



//SQ 111121
#define MAKE_CPS_NODENAME(C)	MAKE_NODENAME(CPSMIB, C)
#define MAKE_CPSTABLE_NODENAME(B, C)  MAKE_TABLE_NODENAME(CPSMIB, B, C)

#define MAKE_MPS_NODENAME(C)	MAKE_NODENAME(MPSMIB, C)
#define MAKE_MPSTABLE_NODENAME(B, C)  MAKE_TABLE_NODENAME(MPSMIB, B, C)

#define MAKE_APS_NODENAME(C)	MAKE_NODENAME(APSMIB, C)
#define MAKE_APSTABLE_NODENAME(B, C)  MAKE_TABLE_NODENAME(APSMIB, B, C)


#define MAKE_COMM_NODENAME(C)	MAKE_NODENAME(COMMMIB, C)
#define MAKE_COMMTABLE_NODENAME(B, C)  MAKE_TABLE_NODENAME(COMMMIB, B, C)

#define MAKE_RCD_NODENAME(C)	MAKE_NODENAME(RCDMIB, C)
#define MAKE_RCDTABLE_NODENAME(B, C)  MAKE_TABLE_NODENAME(RCDMIB, B, C)
/************************************************************************/
/*      Node Name Define --- MCU                                        */
/************************************************************************/
//Mcu System
#define NODE_MCUSYSSTATE                     MAKE_MCU_NODENAME(1)       //"1.3.6.1.4.1.8888.1.1.1.1.1.0"
#define NODE_MCUSYSTIME                      MAKE_MCU_NODENAME(2)       //"1.3.6.1.4.1.8888.1.1.1.1.2.0"
#define NODE_MCUSYSFTPFILENAME               MAKE_MCU_NODENAME(3)       //"1.3.6.1.4.1.8888.1.1.1.1.3.0"
#define NODE_MCUSYSCONFIGVERSION             MAKE_MCU_NODENAME(4)       //"1.3.6.1.4.1.8888.1.1.1.1.4.0"
#define NODE_MCUSYSSOFTWAREVERSION           MAKE_MCU_NODENAME(5)       //"1.3.6.1.4.1.8888.1.1.1.1.5.0"
#define NODE_MCUSYSFTPUSER                   MAKE_MCU_NODENAME(6)       //"1.3.6.1.4.1.8888.1.1.1.1.6.0"
#define NODE_MCUSYSFTPPASSWORD               MAKE_MCU_NODENAME(7)       //"1.3.6.1.4.1.8888.1.1.1.1.7.0"
#define NODE_MCUSYSCONFIGERRORSTRING         MAKE_MCU_NODENAME(8)       //"1.3.6.1.4.1.8888.1.1.1.1.8.0"
#define NODE_MCUSYSLAYER                     MAKE_MCU_NODENAME(9)       //"1.3.6.1.4.1.8888.1.1.1.1.9.0"
#define NODE_MCUSYSSLOT                      MAKE_MCU_NODENAME(10)      //"1.3.6.1.4.1.8888.1.1.1.1.10.0"
#define NODE_MCUSYSMGTSTATE                  MAKE_MCU_NODENAME(11)      //"1.3.6.1.4.1.8888.1.1.1.1.11.1.0"
#define NODE_MCUSYSMGTUPDATESOFTWARE         MAKE_MCU_NODENAME(12)      //"1.3.6.1.4.1.8888.1.1.1.1.11.2.0"
#define NODE_MCUSYSMGTBITERRORTEST           MAKE_MCU_NODENAME(13)      //"1.3.6.1.4.1.8888.1.1.1.1.11.3.0"
#define NODE_MCUSYSMGTTIMESYNC               MAKE_MCU_NODENAME(14)      //"1.3.6.1.4.1.8888.1.1.1.1.11.4.0"
#define NODE_MCUSYSMGTSELFTEST               MAKE_MCU_NODENAME(15)      //"1.3.6.1.4.1.8888.1.1.1.1.11.5.0"
#define NODE_MCUSYSMGTE1OPERATE              MAKE_MCU_NODENAME(16)      //"1.3.6.1.4.1.8888.1.1.1.1.11.6.0"
#define NODE_MCUSYSOSTYPE                    MAKE_MCU_NODENAME(17)      //"1.3.6.1.4.1.8888.1.1.1.1.11.7.0"
#define NODE_MCUSYSSUBTYPE                   MAKE_MCU_NODENAME(18)      //"1.3.6.1.4.1.8888.1.1.1.1.11.8.0"

//lukunpeng 新增
//设备自行去ftp服务器获取升级文件升级 Set
//结构定义如下
//对于mt(u8 u16 Reserved)
// u8 file num
// u8 Reserved
// u16 Reserved
//但是对于mcu
//u8 file num
//u8 层
//u8 槽
//u8 type

//以下是sequence, 升级文件的信息循环
// u32 版本服务器地址
// u16 版本服务器端口
// u8 版本服务器登陆用户名 长度
// s8 Array 版本服务器登陆用户名
// u8 版本服务器登陆密码 长度
// s8 Array 版本服务器登陆密码
// u8 版本文件所在的全路径 长度
// s8 Array 版本文件所在的全路径(包括文件名)

//设备自行去ftp服务器获取升级文件升级 Get
//u8 file num
//以下是sequence,文件路径长度和文件路径的循环
// u32 版本服务器地址
// u16 版本服务器端口
// u8 版本文件所在的全路径 长度
// s8 Array 版本文件所在的全路径(包括文件名)
// u8 当前升级进度（百分比）
// u8 当前升级状态 ：
//#define SNMP_FILE_UPDATE_INIT               (u8)0    //未更新过或更新刚开始
//#define SNMP_FILE_UPDATE_INPROCESS          (u8)1    //正在上传更新中
//#define SNMP_FILE_UPDATE_SUCCEED            (u8)2    //更新成功
//#define SNMP_FILE_UPDATE_FAILED             (u8)3    //更新失败
//以下字段仅mcu有
//u8 层
//u8 槽
//u8 type


#define NODE_MCUSYSSELFUPDATESOFTWARE        MAKE_MCU_NODENAME(19)      //"1.3.6.1.4.1.8888.1.1.1.1.11.9.0"

//Mcu Local
#define NODE_MCUID                           MAKE_MCU_NODENAME(20)      //"1.3.6.1.4.1.8888.1.1.1.2.1.0"
#define NODE_MCUALIAS                        MAKE_MCU_NODENAME(21)      //"1.3.6.1.4.1.8888.1.1.1.2.2.0"
#define NODE_MCUE164NUMBER                   MAKE_MCU_NODENAME(22)      //"1.3.6.1.4.1.8888.1.1.1.2.3.0"
#define NODE_MCUCOMPILETIME                  MAKE_MCU_NODENAME(23)      //"1.3.6.1.4.1.8888.1.1.1.2.4.0"
#define NODE_MCULOCALINFOS					 MAKE_MCU_NODENAME(24)      //"1.3.6.1.4.1.8888.1.1.1.2.5.0"
//Mcu Net
//lukunpeng 新增
#define NODE_MCUNETIPADDR					 MAKE_MCU_NODENAME(25)      //"1.3.6.1.4.1.8888.1.1.1.2.6.0"
#define NODE_MCUNETIPMASK					 MAKE_MCU_NODENAME(26)      //"1.3.6.1.4.1.8888.1.1.1.2.7.0"
#define NODE_MCUNETGATEWAY					 MAKE_MCU_NODENAME(27)      //"1.3.6.1.4.1.8888.1.1.1.2.8.0"
#define NODE_MCUNETPORTKIND					 MAKE_MCU_NODENAME(28)      //"1.3.6.1.4.1.8888.1.1.1.2.9.0"
#define NODE_MCUNETTEMPPORTKIND			     MAKE_MCU_NODENAME(29)      //"1.3.6.1.4.1.8888.1.1.1.2.10.0"

#define NODE_MCUNETGKIPADDR                  MAKE_MCU_NODENAME(30)      //"1.3.6.1.4.1.8888.1.1.1.3.1.0"
#define NODE_MCUNETMULTICASTIPADDR           MAKE_MCU_NODENAME(31)      //"1.3.6.1.4.1.8888.1.1.1.3.2.0"
#define NODE_MCUNETMULTICASTPORT             MAKE_MCU_NODENAME(32)      //"1.3.6.1.4.1.8888.1.1.1.3.3.0"
#define NODE_MCUNETSTARTRECVPORT             MAKE_MCU_NODENAME(33)      //"1.3.6.1.4.1.8888.1.1.1.3.4.0"
#define NODE_MCUNET225245STARTPORT           MAKE_MCU_NODENAME(34)      //"1.3.6.1.4.1.8888.1.1.1.3.5.0"
#define NODE_MCUNET225245MAXMTNUM            MAKE_MCU_NODENAME(35)      //"1.3.6.1.4.1.8888.1.1.1.3.6.0"
#define NODE_MCUNETUSEMPCTRANSDATA           MAKE_MCU_NODENAME(36)      //"1.3.6.1.4.1.8888.1.1.1.3.7.0"
#define NODE_MCUNETUSEMPCSTACK               MAKE_MCU_NODENAME(37)      //"1.3.6.1.4.1.8888.1.1.1.3.8.0"
//lukunpeng 新增
#define NODE_MCUNETMAC						 MAKE_MCU_NODENAME(38)      //"1.3.6.1.4.1.8888.1.1.1.3.9.0"
//miaoqingsong 新增
#define NODE_MCUNETGKUSED                    MAKE_MCU_NODENAME(39)      //"1.3.6.1.4.1.8888.1.1.1.3.10.0"

//Mcu Net - QOS Stub
#define NODE_MCUNETQOSTYPE                   MAKE_MCU_NODENAME(40)      //"1.3.6.1.4.1.8888.1.1.1.3.9.1.0"
#define NODE_MCUNETQOSIPSERVICETYPE          MAKE_MCU_NODENAME(41)      //"1.3.6.1.4.1.8888.1.1.1.3.9.2.0"
#define NODE_MCUNETQOSAUDIOLEVEL             MAKE_MCU_NODENAME(42)      //"1.3.6.1.4.1.8888.1.1.1.3.9.3.0"
#define NODE_MCUNETQOSVIDEOLEVEL             MAKE_MCU_NODENAME(43)      //"1.3.6.1.4.1.8888.1.1.1.3.9.4.0"
#define NODE_MCUNETQOSDATALEVEL              MAKE_MCU_NODENAME(44)      //"1.3.6.1.4.1.8888.1.1.1.3.9.5.0"
#define NODE_MCUNETQOSSIGNALLEVEL            MAKE_MCU_NODENAME(45)      //"1.3.6.1.4.1.8888.1.1.1.3.9.6.0"
//Mcu Net - Snmp Config Table
#define NODE_MCUNETSNMPCFGIPADDR             MAKE_MCU_NODENAME(50)      //"1.3.6.1.4.1.8888.1.1.1.3.10.1.1"
#define NODE_MCUNETSNMPCFGREADCOMMUNITY      MAKE_MCU_NODENAME(51)      //"1.3.6.1.4.1.8888.1.1.1.3.10.1.2"
#define NODE_MCUNETSNMPCFGWRITECOMMUNITY     MAKE_MCU_NODENAME(52)      //"1.3.6.1.4.1.8888.1.1.1.3.10.1.3"
#define NODE_MCUNETSNMPCFGGETSETPORT         MAKE_MCU_NODENAME(53)      //"1.3.6.1.4.1.8888.1.1.1.3.10.1.4"
#define NODE_MCUNETSNMPCFGTRAPPORT           MAKE_MCU_NODENAME(54)      //"1.3.6.1.4.1.8888.1.1.1.3.10.1.5"
#define NODE_MCUNETSNMPCFGEXIST              MAKE_MCU_NODENAME(55)      //"1.3.6.1.4.1.8888.1.1.1.3.10.1.6"
//Mcu Net - Board Config Table
#define NODE_MCUNETBRDCFGID                  MAKE_MCU_NODENAME(60)      //"1.3.6.1.4.1.8888.1.1.1.3.11.1.1"
#define NODE_MCUNETBRDCFGLAYER               MAKE_MCU_NODENAME(61)      //"1.3.6.1.4.1.8888.1.1.1.3.11.1.2"
#define NODE_MCUNETBRDCFGSLOT                MAKE_MCU_NODENAME(62)      //"1.3.6.1.4.1.8888.1.1.1.3.11.1.3"
#define NODE_MCUNETBRDCFGTYPE                MAKE_MCU_NODENAME(63)      //"1.3.6.1.4.1.8888.1.1.1.3.11.1.4"
#define NODE_MCUNETBRDCFGSTATUS              MAKE_MCU_NODENAME(64)      //"1.3.6.1.4.1.8888.1.1.1.3.11.1.5"
#define NODE_MCUNETBRDCFGVERSION             MAKE_MCU_NODENAME(65)      //"1.3.6.1.4.1.8888.1.1.1.3.11.1.6"
#define NODE_MCUNETBRDCFGIPADDR              MAKE_MCU_NODENAME(66)      //"1.3.6.1.4.1.8888.1.1.1.3.11.1.7"
#define NODE_MCUNETBRDCFGMODULE              MAKE_MCU_NODENAME(67)      //"1.3.6.1.4.1.8888.1.1.1.3.11.1.8"
#define NODE_MCUNETBRDCFGPANELLED            MAKE_MCU_NODENAME(68)      //"1.3.6.1.4.1.8888.1.1.1.3.11.1.9"
#define NODE_MCUNETBRDCFGOSTYPE              MAKE_MCU_NODENAME(69)      //"1.3.6.1.4.1.8888.1.1.1.3.11.1.10"

//Mcu Eqp
//Mcu Eqp - mixer table
#define NODE_MCUEQPMIXERID                   MAKE_MCU_NODENAME(70)      //"1.3.6.1.4.1.8888.1.1.1.4.1.1.1"
#define NODE_MCUEQPMIXERPORT                 MAKE_MCU_NODENAME(71)      //"1.3.6.1.4.1.8888.1.1.1.4.1.1.2"
#define NODE_MCUEQPMIXERSWITCHBRDID          MAKE_MCU_NODENAME(72)      //"1.3.6.1.4.1.8888.1.1.1.4.1.1.3"
#define NODE_MCUEQPMIXERALIAS                MAKE_MCU_NODENAME(73)      //"1.3.6.1.4.1.8888.1.1.1.4.1.1.4"
#define NODE_MCUEQPMIXERRUNNINGBRDID         MAKE_MCU_NODENAME(74)      //"1.3.6.1.4.1.8888.1.1.1.4.1.1.5"
#define NODE_MCUEQPMIXERIPADDR               MAKE_MCU_NODENAME(75)      //"1.3.6.1.4.1.8888.1.1.1.4.1.1.6"
#define NODE_MCUEQPMIXERMIXSTARTPORT         MAKE_MCU_NODENAME(76)      //"1.3.6.1.4.1.8888.1.1.1.4.1.1.7"
#define NODE_MCUEQPMIXERMAXMIXGROUPNUM       MAKE_MCU_NODENAME(77)      //"1.3.6.1.4.1.8888.1.1.1.4.1.1.8"
#define NODE_MCUEQPMIXERCONNSTATE            MAKE_MCU_NODENAME(78)      //"1.3.6.1.4.1.8888.1.1.1.4.1.1.9"
#define NODE_MCUEQPMIXEREXIST                MAKE_MCU_NODENAME(79)      //"1.3.6.1.4.1.8888.1.1.1.4.1.1.10"
//Mcu Eqp - recorder table
#define NODE_MCUEQPRECORDERID                MAKE_MCU_NODENAME(85)      //"1.3.6.1.4.1.8888.1.1.1.4.2.1.1"
#define NODE_MCUEQPRECORDERSWITCHBRDID       MAKE_MCU_NODENAME(86)      //"1.3.6.1.4.1.8888.1.1.1.4.2.1.2"
#define NODE_MCUEQPRECORDERPORT              MAKE_MCU_NODENAME(87)      //"1.3.6.1.4.1.8888.1.1.1.4.2.1.3"
#define NODE_MCUEQPRECORDERALIAS             MAKE_MCU_NODENAME(88)      //"1.3.6.1.4.1.8888.1.1.1.4.2.1.4"
#define NODE_MCUEQPRECORDERIPADDR            MAKE_MCU_NODENAME(89)      //"1.3.6.1.4.1.8888.1.1.1.4.2.1.5"
#define NODE_MCUEQPRECORDERRECVSTARTPORT     MAKE_MCU_NODENAME(90)      //"1.3.6.1.4.1.8888.1.1.1.4.2.1.6"
#define NODE_MCUEQPRECORDERCONNSTATE         MAKE_MCU_NODENAME(91)      //"1.3.6.1.4.1.8888.1.1.1.4.2.1.7"
#define NODE_MCUEQPRECORDEREXIST             MAKE_MCU_NODENAME(92)      //"1.3.6.1.4.1.8888.1.1.1.4.2.1.8"
//Mcu Eqp - TvWall table
#define NODE_MCUEQPTVWALLID                  MAKE_MCU_NODENAME(100)     //"1.3.6.1.4.1.8888.1.1.1.4.3.1.1"
#define NODE_MCUEQPTVWALLALIAS               MAKE_MCU_NODENAME(101)     //"1.3.6.1.4.1.8888.1.1.1.4.3.1.2"
#define NODE_MCUEQPTVWALLRUNNINGBRDID        MAKE_MCU_NODENAME(102)     //"1.3.6.1.4.1.8888.1.1.1.4.3.1.3"
#define NODE_MCUEQPTVWALLIPADDR              MAKE_MCU_NODENAME(103)     //"1.3.6.1.4.1.8888.1.1.1.4.3.1.4"
#define NODE_MCUEQPTVWALLVIDEOSTARTRECVPORT  MAKE_MCU_NODENAME(104)     //"1.3.6.1.4.1.8888.1.1.1.4.3.1.5"
#define NODE_MCUEQPTVWALLCONNSTATE           MAKE_MCU_NODENAME(105)     //"1.3.6.1.4.1.8888.1.1.1.4.3.1.6"
#define NODE_MCUEQPTVWALLEXIST               MAKE_MCU_NODENAME(106)     //"1.3.6.1.4.1.8888.1.1.1.4.3.1.7"
//Mcu Eqp - Bas table
#define NODE_MCUEQPBASID                     MAKE_MCU_NODENAME(110)     //"1.3.6.1.4.1.8888.1.1.1.4.4.1.1"
#define NODE_MCUEQPBASPORT                   MAKE_MCU_NODENAME(111)     //"1.3.6.1.4.1.8888.1.1.1.4.4.1.2"
#define NODE_MCUEQPBASSWITCHBRDID            MAKE_MCU_NODENAME(112)     //"1.3.6.1.4.1.8888.1.1.1.4.4.1.3"
#define NODE_MCUEQPBASALIAS                  MAKE_MCU_NODENAME(113)     //"1.3.6.1.4.1.8888.1.1.1.4.4.1.4"
#define NODE_MCUEQPBASRUNNINGBRDID           MAKE_MCU_NODENAME(114)     //"1.3.6.1.4.1.8888.1.1.1.4.4.1.5"
#define NODE_MCUEQPBASIPADDR                 MAKE_MCU_NODENAME(115)     //"1.3.6.1.4.1.8888.1.1.1.4.4.1.6"
#define NODE_MCUEQPBASSTARTRECVPORT          MAKE_MCU_NODENAME(116)     //"1.3.6.1.4.1.8888.1.1.1.4.4.1.7"
#define NODE_MCUEQPBASCONNSTATE              MAKE_MCU_NODENAME(117)     //"1.3.6.1.4.1.8888.1.1.1.4.4.1.8"
#define NODE_MCUEQPBASEXIST                  MAKE_MCU_NODENAME(118)     //"1.3.6.1.4.1.8888.1.1.1.4.4.1.9"
//Mcu Eqp - Vmp table
#define NODE_MCUEQPVMPID                     MAKE_MCU_NODENAME(120)     //"1.3.6.1.4.1.8888.1.1.1.4.5.1.1"
#define NODE_MCUEQPVMPPORT                   MAKE_MCU_NODENAME(121)     //"1.3.6.1.4.1.8888.1.1.1.4.5.1.2"
#define NODE_MCUEQPVMPSWITCHBRDID            MAKE_MCU_NODENAME(122)     //"1.3.6.1.4.1.8888.1.1.1.4.5.1.3"
#define NODE_MCUEQPVMPALIAS                  MAKE_MCU_NODENAME(123)     //"1.3.6.1.4.1.8888.1.1.1.4.5.1.4"
#define NODE_MCUEQPVMPRUNNINGBRDID           MAKE_MCU_NODENAME(124)     //"1.3.6.1.4.1.8888.1.1.1.4.5.1.5"
#define NODE_MCUEQPVMPIPADDR                 MAKE_MCU_NODENAME(125)     //"1.3.6.1.4.1.8888.1.1.1.4.5.1.6"
#define NODE_MCUEQPVMPSTARTRECVPORT          MAKE_MCU_NODENAME(126)     //"1.3.6.1.4.1.8888.1.1.1.4.5.1.7"
#define NODE_MCUEQPVMPENCODERNUM             MAKE_MCU_NODENAME(127)     //"1.3.6.1.4.1.8888.1.1.1.4.5.1.8"
#define NODE_MCUEQPVMPCONNSTATE              MAKE_MCU_NODENAME(128)     //"1.3.6.1.4.1.8888.1.1.1.4.5.1.9"
#define NODE_MCUEQPVMPEXIST                  MAKE_MCU_NODENAME(129)     //"1.3.6.1.4.1.8888.1.1.1.4.5.1.10"
//Mcu Eqp - DCS Stub
#define NODE_MCUEQPDCSIPADDR                 MAKE_MCU_NODENAME(130)     //"1.3.6.1.4.1.8888.1.1.1.4.6.1.0"
#define NODE_MCUEQPDCSCONNSTATE              MAKE_MCU_NODENAME(131)     //"1.3.6.1.4.1.8888.1.1.1.4.6.2.0"
//Mcu Eqp - PRS Stub
#define NODE_MCUEQPPRSID                     MAKE_MCU_NODENAME(135)     //"1.3.6.1.4.1.8888.1.1.1.4.7.1.0"
#define NODE_MCUEQPPRSMCUSTARTPORT           MAKE_MCU_NODENAME(136)     //"1.3.6.1.4.1.8888.1.1.1.4.7.2.0"
#define NODE_MCUEQPPRSSWITCHBRDID            MAKE_MCU_NODENAME(137)     //"1.3.6.1.4.1.8888.1.1.1.4.7.3.0"
#define NODE_MCUEQPPRSALIAS                  MAKE_MCU_NODENAME(138)     //"1.3.6.1.4.1.8888.1.1.1.4.7.4.0"
#define NODE_MCUEQPPRSRUNNINGBRDID           MAKE_MCU_NODENAME(139)     //"1.3.6.1.4.1.8888.1.1.1.4.7.5.0"
#define NODE_MCUEQPPRSIPADDR                 MAKE_MCU_NODENAME(140)     //"1.3.6.1.4.1.8888.1.1.1.4.7.6.0"
#define NODE_MCUEQPPRSSTARTPORT              MAKE_MCU_NODENAME(141)     //"1.3.6.1.4.1.8888.1.1.1.4.7.7.0"
#define NODE_MCUEQPPRSFIRSTTIMESPAN          MAKE_MCU_NODENAME(142)     //"1.3.6.1.4.1.8888.1.1.1.4.7.8.0"
#define NODE_MCUEQPPRSSECONDTIMESPAN         MAKE_MCU_NODENAME(143)     //"1.3.6.1.4.1.8888.1.1.1.4.7.9.0"
#define NODE_MCUEQPPRSTHIRDTIMESPAN          MAKE_MCU_NODENAME(144)     //"1.3.6.1.4.1.8888.1.1.1.4.7.10.0"
#define NODE_MCUEQPPRSREJECTTIMESPAN         MAKE_MCU_NODENAME(145)     //"1.3.6.1.4.1.8888.1.1.1.4.7.11.0"
//Mcu Eqp - Net Sync Stub
#define NODE_MCUEQPNETSYNCMODE               MAKE_MCU_NODENAME(146)     //"1.3.6.1.4.1.8888.1.1.1.4.8.1.0"
#define NODE_MCUEQPNETSYNCDTSLOT             MAKE_MCU_NODENAME(147)     //"1.3.6.1.4.1.8888.1.1.1.4.8.2.0"
#define NODE_MCUEQPNETSYNCE1INDEX            MAKE_MCU_NODENAME(148)     //"1.3.6.1.4.1.8888.1.1.1.4.8.3.0"
#define NODE_MCUEQPNETSYNCSTATE              MAKE_MCU_NODENAME(149)     //"1.3.6.1.4.1.8888.1.1.1.4.8.4.0"


//Mcu Alarm
#define NODE_MCUPFMALARMSTAMP                MAKE_MCU_NODENAME(155)     //"1.3.6.1.4.1.8888.1.1.1.5.1.0"
//Mcu Alarm Table
#define NODE_MCUPFMALARMSERIALNO             MAKE_MCU_NODENAME(156)     //"1.3.6.1.4.1.8888.1.1.1.5.2.1.1"
#define NODE_MCUPFMALARMALARMCODE            MAKE_MCU_NODENAME(157)     //"1.3.6.1.4.1.8888.1.1.1.5.2.1.2"
#define NODE_MCUPFMALARMOBJTYPE              MAKE_MCU_NODENAME(158)     //"1.3.6.1.4.1.8888.1.1.1.5.2.1.3"
#define NODE_MCUPFMALARMOBJECT               MAKE_MCU_NODENAME(159)     //"1.3.6.1.4.1.8888.1.1.1.5.2.1.4"
#define NODE_MCUPFMALARMTIME                 MAKE_MCU_NODENAME(160)     //"1.3.6.1.4.1.8888.1.1.1.5.2.1.5"


//Mcu Notifications value
#define NODE_MCUCONFSTATENOTIFY              MAKE_MCU_NODENAME(165)     //"1.3.6.1.4.1.8888.1.1.2.1"
#define NODE_MCUCONFIDNOTIFY                 MAKE_MCU_NODENAME(166)     //"1.3.6.1.4.1.8888.1.1.2.2"
#define NODE_MCUCONFNAMENOTIFY               MAKE_MCU_NODENAME(167)     //"1.3.6.1.4.1.8888.1.1.2.3"
#define NODE_MCUCONFSTARTTIMENOTIFY          MAKE_MCU_NODENAME(168)     //"1.3.6.1.4.1.8888.1.1.2.4"
#define NODE_MTSTATENOTIFY                   MAKE_MCU_NODENAME(169)     //"1.3.6.1.4.1.8888.1.1.2.5"
#define NODE_MTALIASNOTIFY                   MAKE_MCU_NODENAME(170)     //"1.3.6.1.4.1.8888.1.1.2.6"
#define NODE_MTJOINTIMENOTIFY                MAKE_MCU_NODENAME(171)     //"1.3.6.1.4.1.8888.1.1.2.7"


//lukunpeng 新增
//Mcu Performance Value
#define NODE_MCUPFMCPURATE                   MAKE_MCU_NODENAME(201)     //"1.3.6.1.4.1.8888.1.1.1.6.1.0"
#define NODE_MCUPFMMEMORY                    MAKE_MCU_NODENAME(202)     //"1.3.6.1.4.1.8888.1.1.1.6.2.0"
#define NODE_MCUPFMMTNUM					 MAKE_MCU_NODENAME(203)     //"1.3.6.1.4.1.8888.1.1.1.6.3.0"
#define NODE_MCUPFMCONFNUM					 MAKE_MCU_NODENAME(204)     //"1.3.6.1.4.1.8888.1.1.1.6.4.0"
#define NODE_MCUPFMEQPUSEDRATE               MAKE_MCU_NODENAME(205)     //"1.3.6.1.4.1.8888.1.1.1.6.5.0"
#define NODE_MCUPFMAUTHRATE					 MAKE_MCU_NODENAME(206)     //"1.3.6.1.4.1.8888.1.1.1.6.6.0"
#define NODE_MCUPFMMACHTEMPER			     MAKE_MCU_NODENAME(207)     //"1.3.6.1.4.1.8888.1.1.1.6.7.0"
#define NODE_MCUPFMTIME						 MAKE_MCU_NODENAME(208)     //"1.3.6.1.4.1.8888.1.1.1.6.8.0"
#define NODE_MCUPFMMCSCONNECTIONS			 MAKE_MCU_NODENAME(209)     //"1.3.6.1.4.1.8888.1.1.1.6.9.0"

//TrapType
//[2010/12/21 miaoqingsong modify] 网管5.0开发需要新增10 11 12
#define INVALID_TRAP_TYPE                   (u16)0
#define TRAP_COLD_RESTART                   (u16)1
#define TRAP_POWEROFF                       (u16)2
#define TRAP_ALARM_GENERATE                 (u16)3
#define TRAP_ALARM_RESTORE                  (u16)4
#define TRAP_LED_STATE                      (u16)5
#define TRAP_CONF_STATE                     (u16)6
#define TRAP_MT_STATE                       (u16)7
#define TRAP_BOARD_UPDATED                  (u16)8
#define TRAP_MCU_PFMINFO					(u16)9
#define TRAP_MT_LED_STATE                   (u16)10
#define TRAP_MT_PFMINFO			            (u16)11
#define TRAP_MT_CONF_STATE		            (u16)12


#define TRAP_COMMON_LED_STATE				(u16)30

/************************************************************************/
/* Node Name Define --- MT                                              */
/************************************************************************/
//mt system
#define NODE_MTSYSSTATE                      MAKE_MT_NODENAME(1)        //"1.3.6.1.4.1.8888.1.2.1.1.1.0"
#define NODE_MTSYSTIME                       MAKE_MT_NODENAME(2)        //"1.3.6.1.4.1.8888.1.2.1.1.2.0"
#define NODE_MTSYSFTPFILENAME                MAKE_MT_NODENAME(3)        //"1.3.6.1.4.1.8888.1.2.1.1.3.0"
#define NODE_MTSYSCONFIGVERSION              MAKE_MT_NODENAME(4)        //"1.3.6.1.4.1.8888.1.2.1.1.4.0"
#define NODE_MTSYSSOFTWAREVERSION            MAKE_MT_NODENAME(5)        //"1.3.6.1.4.1.8888.1.2.1.1.5.0"
#define NODE_MTSYSHARDWARETYPE               MAKE_MT_NODENAME(6)        //"1.3.6.1.4.1.8888.1.2.1.1.6.0"
#define NODE_MTSYSFTPUSERNAME                MAKE_MT_NODENAME(7)        //"1.3.6.1.4.1.8888.1.2.1.1.7.0"
#define NODE_MTSYSFTPPASSWORD                MAKE_MT_NODENAME(8)        //"1.3.6.1.4.1.8888.1.2.1.1.8.0"
#define NODE_MTSYSOSTYPE                     MAKE_MT_NODENAME(9)        //"1.3.6.1.4.1.8888.1.2.1.1.9.0"

//lukunpeng 新增
//设备自行去ftp服务器获取升级文件升级
//结构定义如下
//对于mt(u8 u16 Reserved)
// u8 file num
// u8 Reserved
// u16 Reserved
//但是对于mcu
//u8 file num
//u8 层
//u8 槽
//u8 type

//以下是sequence, 升级文件的信息循环
// u32 版本服务器地址
// u16 版本服务器端口
// u8 版本服务器登陆用户名 长度
// s8 Array 版本服务器登陆用户名
// u8 版本服务器登陆密码 长度
// s8 Array 版本服务器登陆密码
// u8 版本文件所在的全路径 长度
// s8 Array 版本文件所在的全路径(包括文件名)

#define NODE_MTSYSSELFUPDATESOFTWARE         MAKE_MT_NODENAME(10)        //"1.3.6.1.4.1.8888.1.2.1.1.10.0"

//-----mt networks
#define NODE_MTGKUSED                        MAKE_MT_NODENAME(11)       //"1.3.6.1.4.1.8888.1.2.1.2.1.1.0"
#define NODE_MTGKIP                          MAKE_MT_NODENAME(12)       //"1.3.6.1.4.1.8888.1.2.1.2.1.2.0"

//lukunpeng 新增
#define NODE_MTCOMPILETIME                   MAKE_MT_NODENAME(13)       //"1.3.6.1.4.1.8888.1.2.1.2.1.3.0"
//
#define NODE_MTQOSTYPE                       MAKE_MT_NODENAME(16)       //"1.3.6.1.4.1.8888.1.2.1.2.2.1.0"
#define NODE_MTQOSSIGNALLING                 MAKE_MT_NODENAME(17)       //"1.3.6.1.4.1.8888.1.2.1.2.2.2.0"
#define NODE_MTQOSDATA                       MAKE_MT_NODENAME(18)       //"1.3.6.1.4.1.8888.1.2.1.2.2.3.0"
#define NODE_MTQOSAUDIO                      MAKE_MT_NODENAME(19)       //"1.3.6.1.4.1.8888.1.2.1.2.2.4.0"
#define NODE_MTQOSVIDEO                      MAKE_MT_NODENAME(20)       //"1.3.6.1.4.1.8888.1.2.1.2.2.5.0"
#define NODE_MTQOSTOS                        MAKE_MT_NODENAME(21)       //"1.3.6.1.4.1.8888.1.2.1.2.2.6.0"
//
#define NODE_MTE1CONFIGNUMMODULEUSED         MAKE_MT_NODENAME(25)       //"1.3.6.1.4.1.8888.1.2.1.2.3.1.0"
#define NODE_MTE1CONFIGISBIND                MAKE_MT_NODENAME(26)       //"1.3.6.1.4.1.8888.1.2.1.2.3.2.0"
#define NODE_MTE1CONFIGIP                    MAKE_MT_NODENAME(27)       //"1.3.6.1.4.1.8888.1.2.1.2.3.3.0"
#define NODE_MTE1IPMASK                      MAKE_MT_NODENAME(28)       //"1.3.6.1.4.1.8888.1.2.1.2.3.4.0"
#define NODE_MTE1CONFIGAUTHENTICATIONTYPE    MAKE_MT_NODENAME(29)       //"1.3.6.1.4.1.8888.1.2.1.2.3.5.0"
#define NODE_MTE1CONFIGFRAGMINPACKAGELEN     MAKE_MT_NODENAME(30)       //"1.3.6.1.4.1.8888.1.2.1.2.3.6.0"
#define NODE_MTE1CONFIGSVRUSRNAME            MAKE_MT_NODENAME(31)       //"1.3.6.1.4.1.8888.1.2.1.2.3.7.0"
#define NODE_MTE1CONFIGSVRUSRPWD             MAKE_MT_NODENAME(32)       //"1.3.6.1.4.1.8888.1.2.1.2.3.8.0"
#define NODE_MTE1CONFIGSENTUSRNAME           MAKE_MT_NODENAME(33)       //"1.3.6.1.4.1.8888.1.2.1.2.3.9.0"
#define NODE_MTE1CONFIGSENTUSRPWD            MAKE_MT_NODENAME(34)       //"1.3.6.1.4.1.8888.1.2.1.2.3.10.0"

//#define NODE_MTE1CONFIGE1UNITENTRY           MAKE_MT_NODENAME(1)       //"1.3.6.1.4.1.8888.1.2.1.2.3.11.1"
#define NODE_MTE1UNITE1TSMASK                MAKE_MT_NODENAME(40)       //"1.3.6.1.4.1.8888.1.2.1.2.3.11.1.2"
#define NODE_MTE1UNITPROTOCOL                MAKE_MT_NODENAME(41)       //"1.3.6.1.4.1.8888.1.2.1.2.3.11.1.3"
#define NODE_MTE1UNITECHOINTERVAL            MAKE_MT_NODENAME(42)       //"1.3.6.1.4.1.8888.1.2.1.2.3.11.1.4"
#define NODE_MTE1UNITECHOMAXRETRY            MAKE_MT_NODENAME(43)       //"1.3.6.1.4.1.8888.1.2.1.2.3.11.1.5"
#define NODE_MTE1UNITSTATUS                  MAKE_MT_NODENAME(44)       //"1.3.6.1.4.1.8888.1.2.1.2.3.11.1.6"
//
#define NODE_MTEMBEDFWNATPROXYUSED           MAKE_MT_NODENAME(46)       //"1.3.6.1.4.1.8888.1.2.1.2.4.1.0"
#define NODE_MTEMBEDFWNATPROXYSERVIP         MAKE_MT_NODENAME(47)       //"1.3.6.1.4.1.8888.1.2.1.2.4.2.0"
#define NODE_MTEMBEDFWNATPROXYSERVLISTENPORT MAKE_MT_NODENAME(48)       //"1.3.6.1.4.1.8888.1.2.1.2.4.3.0"
#define NODE_MTEMBEDFWNATPROXYSTREAMBASEPORT MAKE_MT_NODENAME(49)       //"1.3.6.1.4.1.8888.1.2.1.2.4.4.0"
#define NODE_MTEMBEDFWNATPROXYSTATICNATADDR  MAKE_MT_NODENAME(50)       //"1.3.6.1.4.1.8888.1.2.1.2.4.5.0"

#define NODE_MTSNMPCFGTRAPSERVERIP           MAKE_MT_NODENAME(55)       //"1.3.6.1.4.1.8888.1.2.1.2.5.1.0"
#define NODE_MTSNMPCFGREADCOMMUNITY          MAKE_MT_NODENAME(56)       //"1.3.6.1.4.1.8888.1.2.1.2.5.2.0"
#define NODE_MTSNMPCFGWRITECOMMUNITY         MAKE_MT_NODENAME(57)       //"1.3.6.1.4.1.8888.1.2.1.2.5.3.0"
#define NODE_MTSNMPCFGGETSETPORT             MAKE_MT_NODENAME(58)       //"1.3.6.1.4.1.8888.1.2.1.2.5.4.0"
#define NODE_MTSNMPCFGTRAPPORT               MAKE_MT_NODENAME(59)       //"1.3.6.1.4.1.8888.1.2.1.2.5.5.0"

#define NODE_MTPPPOEUSED                     MAKE_MT_NODENAME(61)       //"1.3.6.1.4.1.8888.1.2.1.2.6.1.0"
#define NODE_MTPPPOEUSERNAME                 MAKE_MT_NODENAME(62)       //"1.3.6.1.4.1.8888.1.2.1.2.6.2.0"
#define NODE_MTPPPOEPASSWORD                 MAKE_MT_NODENAME(63)       //"1.3.6.1.4.1.8888.1.2.1.2.6.3.0"

//#define NODE_MTETHNETINFOENTRY               MAKE_MT_NODENAME(1)       //"1.3.6.1.4.1.8888.1.2.1.2.7.1"
#define NODE_MTETHNETINFOIP                  MAKE_MT_NODENAME(66)       //"1.3.6.1.4.1.8888.1.2.1.2.7.1.2"
#define NODE_MTETHNETINFOMASK                MAKE_MT_NODENAME(67)       //"1.3.6.1.4.1.8888.1.2.1.2.7.1.3"
#define NODE_MTETHNETINFOISUSEASSIGNBW       MAKE_MT_NODENAME(68)       //"1.3.6.1.4.1.8888.1.2.1.2.7.1.4"
#define NODE_MTETHNETINFOUPBANDWIDTH         MAKE_MT_NODENAME(69)       //"1.3.6.1.4.1.8888.1.2.1.2.7.1.5"
#define NODE_MTETHNETINFOENABLE              MAKE_MT_NODENAME(70)       //"1.3.6.1.4.1.8888.1.2.1.2.7.1.6"

//#define NODE_MTROUTECFGENTRY                 MAKE_MT_NODENAME(1)       //"1.3.6.1.4.1.8888.1.2.1.2.8.1"
#define NODE_MTROUTECFGDSTNET                MAKE_MT_NODENAME(75)       //"1.3.6.1.4.1.8888.1.2.1.2.8.1.2"
#define NODE_MTROUTECFGDSTNETMASK            MAKE_MT_NODENAME(76)       //"1.3.6.1.4.1.8888.1.2.1.2.8.1.3"
#define NODE_MTROUTECFGNEXTIP                MAKE_MT_NODENAME(77)       //"1.3.6.1.4.1.8888.1.2.1.2.8.1.4"
#define NODE_MTROUTECFGPRI                   MAKE_MT_NODENAME(78)       //"1.3.6.1.4.1.8888.1.2.1.2.8.1.5"
#define NODE_MTROUTECFGISSTATUS              MAKE_MT_NODENAME(79)       //"1.3.6.1.4.1.8888.1.2.1.2.8.1.6"

//---mt eqp
//#define NODE_MTCAMERACFGENTRY                MAKE_MT_NODENAME(1)       //"1.3.6.1.4.1.8888.1.2.1.3.1.1"
#define NODE_MTCAMERACFGCAMERA               MAKE_MT_NODENAME(81)       //"1.3.6.1.4.1.8888.1.2.1.3.1.1.2"
#define NODE_MTCAMERACFGADDR                 MAKE_MT_NODENAME(82)       //"1.3.6.1.4.1.8888.1.2.1.3.1.1.3"
#define NODE_MTCAMERACFGSPEEDLEVEL           MAKE_MT_NODENAME(83)       //"1.3.6.1.4.1.8888.1.2.1.3.1.1.4"
#define NODE_MTCAMERACFGSERIALTYPE           MAKE_MT_NODENAME(84)       //"1.3.6.1.4.1.8888.1.2.1.3.1.1.5"
#define NODE_MTCAMERACFGSERIALSERVERIP       MAKE_MT_NODENAME(85)       //"1.3.6.1.4.1.8888.1.2.1.3.1.1.6"
#define NODE_MTCAMERACFGSERIALSERVERPORT     MAKE_MT_NODENAME(86)       //"1.3.6.1.4.1.8888.1.2.1.3.1.1.7"
#define NODE_MTCAMERACFGSTATUS               MAKE_MT_NODENAME(87)       //"1.3.6.1.4.1.8888.1.2.1.3.1.1.8"

//#define NODE_MTSERIALCFGENTRY                MAKE_MT_NODENAME(1)       //"1.3.6.1.4.1.8888.1.2.1.3.2.1"
#define NODE_MTSERIALCFGBAUDRATE             MAKE_MT_NODENAME(90)       //"1.3.6.1.4.1.8888.1.2.1.3.2.1.2"
#define NODE_MTSERIALCFGBYTESIZE             MAKE_MT_NODENAME(91)       //"1.3.6.1.4.1.8888.1.2.1.3.2.1.3"
#define NODE_MTSERIALCFGCHECK                MAKE_MT_NODENAME(92)       //"1.3.6.1.4.1.8888.1.2.1.3.2.1.4"
#define NODE_MTSERIALCFGSTOPBITS             MAKE_MT_NODENAME(93)       //"1.3.6.1.4.1.8888.1.2.1.3.2.1.5"
#define NODE_MTSERIALCFGSTATUS               MAKE_MT_NODENAME(94)       //"1.3.6.1.4.1.8888.1.2.1.3.2.1.6"

//#define NODE_MTEQPMATRIXENTRY                MAKE_MT_NODENAME(1)       //"1.3.6.1.4.1.8888.1.2.1.3.3.1"
#define NODE_MTEQPMATRIXENTID                MAKE_MT_NODENAME(96)       //"1.3.6.1.4.1.8888.1.2.1.3.3.1.1"
#define NODE_MTEQPMATRIXENTTYPE              MAKE_MT_NODENAME(97)       //"1.3.6.1.4.1.8888.1.2.1.3.3.1.2"
#define NODE_MTEQPMATRIXENTSVRIPADDR         MAKE_MT_NODENAME(98)       //"1.3.6.1.4.1.8888.1.2.1.3.3.1.3"
#define NODE_MTEQPMATRIXENTPORT              MAKE_MT_NODENAME(99)       //"1.3.6.1.4.1.8888.1.2.1.3.3.1.4"
#define NODE_MTEQPMATRIXENTEXIST             MAKE_MT_NODENAME(100)      //"1.3.6.1.4.1.8888.1.2.1.3.3.1.5"

#define NODE_MTEQPNETENCODERID               MAKE_MT_NODENAME(102)      //"1.3.6.1.4.1.8888.1.2.1.3.4.1.0"

//---mt audio video
#define NODE_MTVIDEO1VIDEOBRIGHTNESS         MAKE_MT_NODENAME(106)      //"1.3.6.1.4.1.8888.1.2.1.4.1.1.1.0"
#define NODE_MTVIDEO1VIDEOCONTRAST           MAKE_MT_NODENAME(107)      //"1.3.6.1.4.1.8888.1.2.1.4.1.1.2.0"
#define NODE_MTVIDEO1VIDEOSATURATION         MAKE_MT_NODENAME(108)      //"1.3.6.1.4.1.8888.1.2.1.4.1.1.3.0"
#define NODE_MTVIDEO1INVIDEOSTANDARD         MAKE_MT_NODENAME(109)      //"1.3.6.1.4.1.8888.1.2.1.4.1.1.4.0"
#define NODE_MTVIDEO1OUTVIDEOSTANDARD        MAKE_MT_NODENAME(110)      //"1.3.6.1.4.1.8888.1.2.1.4.1.1.5.0"
#define NODE_MTVIDEO1PRIORSTRATEGY           MAKE_MT_NODENAME(111)      //"1.3.6.1.4.1.8888.1.2.1.4.1.1.6.0"
#define NODE_MTVIDEO1SOURCE                  MAKE_MT_NODENAME(112)      //"1.3.6.1.4.1.8888.1.2.1.4.1.1.7.0"
#define NODE_MTVIDEO1RESOLUTION              MAKE_MT_NODENAME(113)      //"1.3.6.1.4.1.8888.1.2.1.4.1.1.8.0"
#define NODE_MTVIDEO1BITRATE                 MAKE_MT_NODENAME(114)      //"1.3.6.1.4.1.8888.1.2.1.4.1.1.9.0"

#define NODE_MTVIDEO2VIDEOBRIGHTNESS         MAKE_MT_NODENAME(117)      //"1.3.6.1.4.1.8888.1.2.1.4.1.2.1.0"
#define NODE_MTVIDEO2VIDEOCONTRAST           MAKE_MT_NODENAME(118)      //"1.3.6.1.4.1.8888.1.2.1.4.1.2.2.0"
#define NODE_MTVIDEO2VIDEOSATURATION         MAKE_MT_NODENAME(119)      //"1.3.6.1.4.1.8888.1.2.1.4.1.2.3.0"
#define NODE_MTVIDEO2INVIDEOSTANDARD         MAKE_MT_NODENAME(120)      //"1.3.6.1.4.1.8888.1.2.1.4.1.2.4.0"
#define NODE_MTVIDEO2OUTVIDEOSTANDARD        MAKE_MT_NODENAME(121)      //"1.3.6.1.4.1.8888.1.2.1.4.1.2.5.0"
#define NODE_MTVIDEO2FORMATPRIORSTRATEGY     MAKE_MT_NODENAME(122)      //"1.3.6.1.4.1.8888.1.2.1.4.1.2.6.0"
#define NODE_MTVIDEO2SOURCE                  MAKE_MT_NODENAME(123)      //"1.3.6.1.4.1.8888.1.2.1.4.1.2.7.0"
#define NODE_MTVIDEO2RESOLUTION              MAKE_MT_NODENAME(124)      //"1.3.6.1.4.1.8888.1.2.1.4.1.2.8.0"
#define NODE_MTVIDEO2BITRATE                 MAKE_MT_NODENAME(125)      //"1.3.6.1.4.1.8888.1.2.1.4.1.2.9.0"

#define NODE_MTMAINIMAGEFRAMERATE            MAKE_MT_NODENAME(128)      //"1.3.6.1.4.1.8888.1.2.1.4.1.3.1.0"
#define NODE_MTMAINIMAGEIKEYRATE             MAKE_MT_NODENAME(129)      //"1.3.6.1.4.1.8888.1.2.1.4.1.3.2.0"
#define NODE_MTMAINIMAGEMETEBEGIN            MAKE_MT_NODENAME(130)      //"1.3.6.1.4.1.8888.1.2.1.4.1.3.3.0"
#define NODE_MTMAINIMAGEMETEEND              MAKE_MT_NODENAME(131)      //"1.3.6.1.4.1.8888.1.2.1.4.1.3.4.0"
#define NODE_MTMAINIMAGEQUALITY              MAKE_MT_NODENAME(132)      //"1.3.6.1.4.1.8888.1.2.1.4.1.3.5.0"

#define NODE_MTDUALIMAGEFRAMERATE            MAKE_MT_NODENAME(134)      //"1.3.6.1.4.1.8888.1.2.1.4.1.4.1.0"
#define NODE_MTDUALIMAGEIKEYRATE             MAKE_MT_NODENAME(135)      //"1.3.6.1.4.1.8888.1.2.1.4.1.4.2.0"
#define NODE_MTDUALIMAGEMETEBEGIN            MAKE_MT_NODENAME(136)      //"1.3.6.1.4.1.8888.1.2.1.4.1.4.3.0"
#define NODE_MTDUALIMAGEMETEEND              MAKE_MT_NODENAME(137)      //"1.3.6.1.4.1.8888.1.2.1.4.1.4.4.0"
#define NODE_MTDUALIMAGEQUALITY              MAKE_MT_NODENAME(138)      //"1.3.6.1.4.1.8888.1.2.1.4.1.4.5.0"

#define NODE_MTAUDIOENCSOURCE                MAKE_MT_NODENAME(141)      //"1.3.6.1.4.1.8888.1.2.1.4.2.1.0"
#define NODE_MTAUDIOENCFORMAT                MAKE_MT_NODENAME(142)      //"1.3.6.1.4.1.8888.1.2.1.4.2.2.0"
#define NODE_MTAUDIOENCVOLUME                MAKE_MT_NODENAME(143)      //"1.3.6.1.4.1.8888.1.2.1.4.2.3.0"
#define NODE_MTAUDIOAEC                      MAKE_MT_NODENAME(144)      //"1.3.6.1.4.1.8888.1.2.1.4.2.4.0"
#define NODE_MTAUDIOCFGAGCENABLE             MAKE_MT_NODENAME(145)      //"1.3.6.1.4.1.8888.1.2.1.4.2.5.0"
#define NODE_MTAUDIOCFGENABLEAEC             MAKE_MT_NODENAME(146)      //"1.3.6.1.4.1.8888.1.2.1.4.2.6.0"
#define NODE_MTAUDIOCFGOUTPUTVOLUME          MAKE_MT_NODENAME(147)      //"1.3.6.1.4.1.8888.1.2.1.4.2.7.0"
#define NODE_MTAUDIOCFGINPUTVOLUME           MAKE_MT_NODENAME(148)      //"1.3.6.1.4.1.8888.1.2.1.4.2.8.0"
#define NODE_MTAUDIOCFGFORMATPRIORSTRATEGY   MAKE_MT_NODENAME(149)      //"1.3.6.1.4.1.8888.1.2.1.4.2.9.0"

//---mt pfm
#define NODE_MTPFMALARMSTAMP                 MAKE_MT_NODENAME(151)      //"1.3.6.1.4.1.8888.1.2.1.5.1.0"

//#define NODE_MTPFMALARMENTRY                 MAKE_MT_NODENAME(1)      //"1.3.6.1.4.1.8888.1.2.1.5.2.1"
#define NODE_MTPFMALARMENTSERIALNO           MAKE_MT_NODENAME(152)      //"1.3.6.1.4.1.8888.1.2.1.5.2.1.1"
#define NODE_MTPFMALARMENTALARMCODE          MAKE_MT_NODENAME(153)      //"1.3.6.1.4.1.8888.1.2.1.5.2.1.2"
#define NODE_MTPFMALARMENTOBJTYPE            MAKE_MT_NODENAME(154)      //"1.3.6.1.4.1.8888.1.2.1.5.2.1.3"
#define NODE_MTPFMALARMENTOBJECT             MAKE_MT_NODENAME(155)      //"1.3.6.1.4.1.8888.1.2.1.5.2.1.4"
#define NODE_MTPFMALARMENTTIME               MAKE_MT_NODENAME(156)      //"1.3.6.1.4.1.8888.1.2.1.5.2.1.5"

#define NODE_MTPFMENCODERVIDEOFRAMERATE      MAKE_MT_NODENAME(158)      //"1.3.6.1.4.1.8888.1.2.1.5.3.1.1.0"
#define NODE_MTPFMENCODERVIDEOBITRATE        MAKE_MT_NODENAME(159)      //"1.3.6.1.4.1.8888.1.2.1.5.3.1.2.0"
#define NODE_MTPFMENCODERAUDIOBITRATE        MAKE_MT_NODENAME(160)      //"1.3.6.1.4.1.8888.1.2.1.5.3.1.3.0"
#define NODE_MTPFMENCODERVIDEOPACKLOSE       MAKE_MT_NODENAME(161)      //"1.3.6.1.4.1.8888.1.2.1.5.3.1.4.0"
#define NODE_MTPFMENCODERVIDEOPACKERROR      MAKE_MT_NODENAME(162)      //"1.3.6.1.4.1.8888.1.2.1.5.3.1.5.0"
#define NODE_MTPFMENCODERAUDIOPACKLOSE       MAKE_MT_NODENAME(163)      //"1.3.6.1.4.1.8888.1.2.1.5.3.1.6.0"
#define NODE_MTPFMENCODERAUDIOPACKERROR      MAKE_MT_NODENAME(164)      //"1.3.6.1.4.1.8888.1.2.1.5.3.1.7.0"

#define NODE_MTPFMDECODERVIDEOFRAMERATE      MAKE_MT_NODENAME(166)      //"1.3.6.1.4.1.8888.1.2.1.5.3.2.1.0"
#define NODE_MTPFMDECODERVIDEOBITRATE        MAKE_MT_NODENAME(167)      //"1.3.6.1.4.1.8888.1.2.1.5.3.2.2.0"
#define NODE_MTPFMDECODERAUDIOBITRATE        MAKE_MT_NODENAME(168)      //"1.3.6.1.4.1.8888.1.2.1.5.3.2.3.0"
#define NODE_MTPFMDECODERVIDEORECVFRAME      MAKE_MT_NODENAME(169)      //"1.3.6.1.4.1.8888.1.2.1.5.3.2.4.0"
#define NODE_MTPFMDECODERAUDIORECVFRAME      MAKE_MT_NODENAME(170)      //"1.3.6.1.4.1.8888.1.2.1.5.3.2.5.0"
#define NODE_MTPFMDECODERVIDEOLOSEFRAME      MAKE_MT_NODENAME(171)      //"1.3.6.1.4.1.8888.1.2.1.5.3.2.6.0"
#define NODE_MTPFMDECODERAUDIOLOSEFRAME      MAKE_MT_NODENAME(172)      //"1.3.6.1.4.1.8888.1.2.1.5.3.2.7.0"
#define NODE_MTPFMDECODERPACKERROR           MAKE_MT_NODENAME(173)      //"1.3.6.1.4.1.8888.1.2.1.5.3.2.8.0"
#define NODE_MTPFMDECODERINDEXLOSE           MAKE_MT_NODENAME(174)      //"1.3.6.1.4.1.8888.1.2.1.5.3.2.9.0"
#define NODE_MTPFMDECODERSIZELOSE            MAKE_MT_NODENAME(175)      //"1.3.6.1.4.1.8888.1.2.1.5.3.2.10.0"
#define NODE_MTPFMDECODERFULLLOSE            MAKE_MT_NODENAME(176)      //"1.3.6.1.4.1.8888.1.2.1.5.3.2.11.0"

#define NODE_MTPFMDSP1FANSPEED               MAKE_MT_NODENAME(178)      //"1.3.6.1.4.1.8888.1.2.1.5.3.3.1.0"
#define NODE_MTPFMDSP2FANSPEED               MAKE_MT_NODENAME(179)      //"1.3.6.1.4.1.8888.1.2.1.5.3.3.2.0"
#define NODE_MTPFMDSP3FANSPEED               MAKE_MT_NODENAME(180)      //"1.3.6.1.4.1.8888.1.2.1.5.3.3.3.0"
#define NODE_MTPFMSYS1FANSPEED               MAKE_MT_NODENAME(181)      //"1.3.6.1.4.1.8888.1.2.1.5.3.3.4.0"
#define NODE_MTPFMSYS2FANSPEED               MAKE_MT_NODENAME(182)      //"1.3.6.1.4.1.8888.1.2.1.5.3.3.5.0"

#define NODE_MTPFMLEDSTATE                   MAKE_MT_NODENAME(185)      //"1.3.6.1.4.1.8888.1.2.1.5.4.1.0"

#define NODE_MTLOCALINFOALIAS                MAKE_MT_NODENAME(187)      //"1.3.6.1.4.1.8888.1.2.1.6.1.0"
#define NODE_MTLOCALINFOE164                 MAKE_MT_NODENAME(188)      //"1.3.6.1.4.1.8888.1.2.1.6.2.0"
#define NODE_MTLOCALINFOH323ENABLE           MAKE_MT_NODENAME(189)      //"1.3.6.1.4.1.8888.1.2.1.6.3.0"
#define NODE_MTLOCALINFOH320ENABLE           MAKE_MT_NODENAME(190)      //"1.3.6.1.4.1.8888.1.2.1.6.4.0"
#define NODE_MTLOCALINFOSIPENABLE            MAKE_MT_NODENAME(191)      //"1.3.6.1.4.1.8888.1.2.1.6.5.0"
#define NODE_MTLOCALINFOH239ENABLE           MAKE_MT_NODENAME(192)      //"1.3.6.1.4.1.8888.1.2.1.6.6.0"
#define NODE_MTLOCALINFOENCRYPTENABLE        MAKE_MT_NODENAME(193)      //"1.3.6.1.4.1.8888.1.2.1.6.7.0"
#define NODE_MTLOCALINFOENCRYARITHMETIC      MAKE_MT_NODENAME(194)      //"1.3.6.1.4.1.8888.1.2.1.6.8.0"
#define NODE_MTLOCALINFOSWITCHENABLE         MAKE_MT_NODENAME(195)      //"1.3.6.1.4.1.8888.1.2.1.6.9.0"
#define NODE_MTLOCALINFOSWITCHTIMEOUT        MAKE_MT_NODENAME(196)      //"1.3.6.1.4.1.8888.1.2.1.6.10.0"
#define NODE_MTLOCALINFOLANGUAGE             MAKE_MT_NODENAME(197)      //"1.3.6.1.4.1.8888.1.2.1.6.12.0"
#define NODE_MTLOCALINFOUSERPASSENABLE       MAKE_MT_NODENAME(198)      //"1.3.6.1.4.1.8888.1.2.1.6.13.0"
#define NODE_MTLOCALINFOUSERPASS             MAKE_MT_NODENAME(199)      //"1.3.6.1.4.1.8888.1.2.1.6.14.0"
#define NODE_MTLOCALINFONETPASSENABLE        MAKE_MT_NODENAME(200)      //"1.3.6.1.4.1.8888.1.2.1.6.15.0"
#define NODE_MTLOCALINFONETPASS              MAKE_MT_NODENAME(201)      //"1.3.6.1.4.1.8888.1.2.1.6.16.0"
#define NODE_MTLOCALINFOS					 MAKE_MT_NODENAME(202)      //"1.3.6.1.4.1.8888.1.2.1.6.17.0"



#define NODE_MTDISPLAYCFGAUTOPIP             MAKE_MT_NODENAME(205)      //"1.3.6.1.4.1.8888.1.2.1.7.1.0"
#define NODE_MTDISPLAYCFGSHOWCONFLAPS        MAKE_MT_NODENAME(206)      //"1.3.6.1.4.1.8888.1.2.1.7.2.0"
#define NODE_MTDISPLAYCFGSYMBOLSHOWMODE      MAKE_MT_NODENAME(207)      //"1.3.6.1.4.1.8888.1.2.1.7.3.0"
#define NODE_MTDISPLAYCFGSYMBOLLEFT          MAKE_MT_NODENAME(208)      //"1.3.6.1.4.1.8888.1.2.1.7.4.0"
#define NODE_MTDISPLAYCFGSYMBOLTOP           MAKE_MT_NODENAME(209)      //"1.3.6.1.4.1.8888.1.2.1.7.5.0"
#define NODE_MTDISPLAYCFGDUALVIDEOSHOWMODE   MAKE_MT_NODENAME(210)      //"1.3.6.1.4.1.8888.1.2.1.7.6.0"
#define NODE_MTDISPLAYCFGSHOWSYSTIME         MAKE_MT_NODENAME(211)      //"1.3.6.1.4.1.8888.1.2.1.7.7.0"

#define NODE_MTCALLCFGANSWERMODE             MAKE_MT_NODENAME(215)      //"1.3.6.1.4.1.8888.1.2.1.8.1.0"
#define NODE_MTCALLCFGAUTOCALL               MAKE_MT_NODENAME(216)      //"1.3.6.1.4.1.8888.1.2.1.8.2.0"
#define NODE_MTCALLCFGCALLPEERNAME           MAKE_MT_NODENAME(217)      //"1.3.6.1.4.1.8888.1.2.1.8.3.0"
#define NODE_MTCALLCFGAUTOCALLRATE           MAKE_MT_NODENAME(218)      //"1.3.6.1.4.1.8888.1.2.1.8.4.0"
#define NODE_MTCALLCFGUDPSNDBASEPORT         MAKE_MT_NODENAME(219)      //"1.3.6.1.4.1.8888.1.2.1.8.5.0"
#define NODE_MTCALLCFGUDPRCVBASEPORT         MAKE_MT_NODENAME(220)      //"1.3.6.1.4.1.8888.1.2.1.8.6.0"
#define NODE_MTCALLCFGTCPBASEPORT            MAKE_MT_NODENAME(221)      //"1.3.6.1.4.1.8888.1.2.1.8.7.0"
#define NODE_MTCALLCFGLINKKEEPLIVETIMEOUT    MAKE_MT_NODENAME(222)      //"1.3.6.1.4.1.8888.1.2.1.8.8.0"
#define NODE_MTCALLCFGCALLRATEDEFAULT        MAKE_MT_NODENAME(223)      //"1.3.6.1.4.1.8888.1.2.1.8.9.0"

// NOTIFICATIONS
#define NODE_MTMEDIASTREAMSTATUS             MAKE_MT_NODENAME(230)      //"1.3.6.1.4.1.8888.1.2.2.1"
#define NODE_MTMEMORYSTATUS                  MAKE_MT_NODENAME(231)      //"1.3.6.1.4.1.8888.1.2.2.2"
#define NODE_MTMAPSTATUS                     MAKE_MT_NODENAME(232)      //"1.3.6.1.4.1.8888.1.2.2.3"
#define NODE_MTCCISTATUS                     MAKE_MT_NODENAME(233)      //"1.3.6.1.4.1.8888.1.2.2.4"
#define NODE_MTFANSTATUS                     MAKE_MT_NODENAME(234)      //"1.3.6.1.4.1.8888.1.2.2.5"
#define NODE_MTV35STATUS                     MAKE_MT_NODENAME(235)      //"1.3.6.1.4.1.8888.1.2.2.6"

//[2010/12/21 miaoqingsong add] 网管5.0开发需要新增
//MT Performance Value
#define NODE_MTPFMCPURATE					 MAKE_MT_NODENAME(251)		//"1.3.6.1.4.1.8888.1.2.1.9.1.0"
#define NODE_MTPFMMEMORY                     MAKE_MT_NODENAME(252)		//"1.3.6.1.4.1.8888.1.2.1.9.2.0"
#define NODE_MTPFMVIDEOLOSERATE				 MAKE_MT_NODENAME(253)		//"1.3.6.1.4.1.8888.1.2.1.9.3.0"
#define NODE_MTPFMAUDIOLOSERATE				 MAKE_MT_NODENAME(254)		//"1.3.6.1.4.1.8888.1.2.1.9.4.0"
#define NODE_MTPFMAVLOSERATE                 MAKE_MT_NODENAME(255)      //"1.3.6.1.4.1.8888.1.2.1.9.5.0"
#define NODE_MTPFMINSIGNAL                   MAKE_MT_NODENAME(256)      //"1.3.6.1.4.1.8888.1.2.1.9.6.0"
#define NODE_MTPFMOUTSIGNAL                  MAKE_MT_NODENAME(257)      //"1.3.6.1.4.1.8888.1.2.1.9.7.0"
#define NODE_MTCONFIND                       MAKE_MT_NODENAME(258)      //"1.3.6.1.4.1.8888.1.2.1.9.8.0"
#define NODE_MTCONFSTARTTIME                 MAKE_MT_NODENAME(259)      //"1.3.6.1.4.1.8888.1.2.1.9.9.0"
#define NODE_MTCONFTIME                      MAKE_MT_NODENAME(260)      //"1.3.6.1.4.1.8888.1.2.1.9.10.0"
#define NODE_MTLEDMODEL					     MAKE_MT_NODENAME(261)      //"1.3.6.1.4.1.8888.1.2.1.9.11.0"
#define NODE_MTLEDSTATUS					 MAKE_MT_NODENAME(262)      //"1.3.6.1.4.1.8888.1.2.1.9.12.0"
#define NODE_MTCONFNAME						 MAKE_MT_NODENAME(263)		//"1.3.6.1.4.1.8888.1.2.1.9.13.0" 
//lukunpeng新增
//NODE_MTETHNETPORTKIND 此Node表示设置当前的使用哪个网口
#define NODE_MTETHNETPORTKIND				MAKE_MT_NODENAME(271)       //"1.3.6.1.4.1.8888.1.2.1.10.1.0"

//NODE_MTETHNETTEMPPORTKIND 此Node表示设置当前网口配置信息的网口
//下面的Ip Mask等都是通过此Node的值作索引设置相应的网口配置
#define NODE_MTETHNETTEMPPORTKIND			MAKE_MT_NODENAME(272)       //"1.3.6.1.4.1.8888.1.2.1.10.2.0"
#define NODE_MTETHNETIP						MAKE_MT_NODENAME(273)       //"1.3.6.1.4.1.8888.1.2.1.10.3.0"
#define NODE_MTETHNETMASK					MAKE_MT_NODENAME(274)       //"1.3.6.1.4.1.8888.1.2.1.10.4.0"
#define NODE_MTETHNETGATEWAY				MAKE_MT_NODENAME(275)       //"1.3.6.1.4.1.8888.1.2.1.10.5.0"
#define NODE_MTETHNETENABLE					MAKE_MT_NODENAME(276)       //"1.3.6.1.4.1.8888.1.2.1.10.6.0"
#define NODE_MTETHNETDNS					MAKE_MT_NODENAME(277)       //"1.3.6.1.4.1.8888.1.2.1.10.7.0"
#define NODE_MTETHNETUDPPORT				MAKE_MT_NODENAME(278)       //"1.3.6.1.4.1.8888.1.2.1.10.8.0"
#define NODE_MTETHNETTCPPORT				MAKE_MT_NODENAME(279)       //"1.3.6.1.4.1.8888.1.2.1.10.9.0"
#define NODE_MTETHNETMAC					MAKE_MT_NODENAME(280)       //"1.3.6.1.4.1.8888.1.2.1.10.10.0"






//////////////////////////////////CPS定义////////////////////////////////////////
//status 1
#define NODE_CPSSYSSTATE				MAKE_CPS_NODENAME(1)	//"1.3.6.1.4.1.8888.1.3.1.1.1.0"
#define NODE_CPSCOMPILETIME             MAKE_CPS_NODENAME(2)	//"1.3.6.1.4.1.8888.1.3.1.1.2.0"
#define NODE_CPSAUTHUSBSTATE			MAKE_CPS_NODENAME(3)	//"1.3.6.1.4.1.8888.1.3.1.1.3.0"//usb授权使用（插入即授权）
//pfm 2
#define NODE_CPSCPURATE					MAKE_CPS_NODENAME(30)   //"1.3.6.1.4.1.8888.1.3.1.2.1.0" //cpu百分比   超过90%上报

//conference 3
#define NODE_CPSMTINCONF				MAKE_CPS_NODENAME(80)	//"1.3.6.1.4.1.8888.1.3.1.3.1.0" //当前入会终端数
#define NODE_CPSCONFCOUNT				MAKE_CPS_NODENAME(81)	//"1.3.6.1.4.1.8888.1.3.1.3.2.0" //当前cps上开的会议数
#define NODE_CPSSUPPORTCONFCOUNT		MAKE_CPS_NODENAME(82)	//"1.3.6.1.4.1.8888.1.3.1.3.3.0" //当前cps上支持最大会议数

//config 4

//alarm  5
#define NODE_CPSPFMALARMSTAMP                 MAKE_CPS_NODENAME(160)     //"1.3.6.1.4.1.8888.1.3.1.5.1.0"
//alarm table 5.2
#define NODE_CPSPFMALARMENTSERIALNO           MAKE_CPS_NODENAME(161)     //"1.3.6.1.4.1.8888.1.3.1.5.2.1.1"
#define NODE_CPSPFMALARMENTALARMCODE          MAKE_CPS_NODENAME(162)     //"1.3.6.1.4.1.8888.1.3.1.5.2.1.2"
#define NODE_CPSPFMALARMENTOBJTYPE            MAKE_CPS_NODENAME(163)     //"1.3.6.1.4.1.8888.1.3.1.5.2.1.3"
#define NODE_CPSPFMALARMENTOBJECT             MAKE_CPS_NODENAME(164)     //"1.3.6.1.4.1.8888.1.3.1.5.2.1.4"
#define NODE_CPSPFMALARMENTTIME               MAKE_CPS_NODENAME(165)     //"1.3.6.1.4.1.8888.1.3.1.5.2.1.5"
//////////////////////////////////MPS定义////////////////////////////////////////
//status 1
#define NODE_MPSSYSSTATE				     MAKE_MPS_NODENAME(1)	//"1.3.6.1.4.1.8888.1.4.1.1.1.0"
#define NODE_MPSCOMPILETIME                  MAKE_MPS_NODENAME(2)	//"1.3.6.1.4.1.8888.1.4.1.1.2.0"

//pfm 2
#define NODE_MPSCPURATE				    	 MAKE_MPS_NODENAME(30)	//"1.3.6.1.4.1.8888.1.4.1.2.1.0" //cpu百分比   超过90%上报
//conference 3
//config 4


//alarm  5
#define NODE_MPSPFMALARMSTAMP                 MAKE_MPS_NODENAME(160)     //"1.3.6.1.4.1.8888.1.4.1.5.1.0"
//alarm table 5.2
#define NODE_MPSPFMALARMENTSERIALNO           MAKE_MPS_NODENAME(161)     //"1.3.6.1.4.1.8888.1.4.1.5.2.1.1"
#define NODE_MPSPFMALARMENTALARMCODE          MAKE_MPS_NODENAME(162)     //"1.3.6.1.4.1.8888.1.4.1.5.2.1.2"
#define NODE_MPSPFMALARMENTOBJTYPE            MAKE_MPS_NODENAME(163)     //"1.3.6.1.4.1.8888.1.4.1.5.2.1.3"
#define NODE_MPSPFMALARMENTOBJECT             MAKE_MPS_NODENAME(164)     //"1.3.6.1.4.1.8888.1.4.1.5.2.1.4"
#define NODE_MPSPFMALARMENTTIME               MAKE_MPS_NODENAME(165)     //"1.3.6.1.4.1.8888.1.4.1.5.2.1.5"
///////////////////////////////////APS定义///////////////////////////////////////
//status 1
#define NODE_APSSYSSTATE					  MAKE_APS_NODENAME(1)	//"1.3.6.1.4.1.8888.1.5.1.1.1.0"
#define NODE_APSCOMPILETIME					  MAKE_APS_NODENAME(2)	//"1.3.6.1.4.1.8888.1.5.1.1.2.0"


//pfm 2
#define NODE_APSCPURATE						  MAKE_APS_NODENAME(30)	//"1.3.6.1.4.1.8888.1.5.1.2.1.0" //cpu百分比   超过90%上报
//conference 3
//config 4
//alarm  5
#define NODE_APSPFMALARMSTAMP				  MAKE_APS_NODENAME(160)     //"1.3.6.1.4.1.8888.1.5.1.5.1.0"
//alarm table 5.2
#define NODE_APSPFMALARMENTSERIALNO           MAKE_APS_NODENAME(161)     //"1.3.6.1.4.1.8888.1.5.1.5.2.1.1"
#define NODE_APSPFMALARMENTALARMCODE          MAKE_APS_NODENAME(162)     //"1.3.6.1.4.1.8888.1.5.1.5.2.1.2"
#define NODE_APSPFMALARMENTOBJTYPE            MAKE_APS_NODENAME(163)     //"1.3.6.1.4.1.8888.1.5.1.5.2.1.3"
#define NODE_APSPFMALARMENTOBJECT             MAKE_APS_NODENAME(164)     //"1.3.6.1.4.1.8888.1.5.1.5.2.1.4"
#define NODE_APSPFMALARMENTTIME               MAKE_APS_NODENAME(165)     //"1.3.6.1.4.1.8888.1.5.1.5.2.1.5"
//////////////////////////////////////////////////////////////////////////




///////////////////////////////通用节点///////////////////////////////////////////
//status 1
#define NODE_COMMSYSSTATE						MAKE_COMM_NODENAME(1)	//"1.3.6.1.4.1.8888.1.6.1.1.1.0" //设备状态
#define NODE_COMMCOMPILETIME					MAKE_COMM_NODENAME(2)	//"1.3.6.1.4.1.8888.1.6.1.1.2.0" //编译时间
#define NODE_COMMSOFTWAREVER					MAKE_COMM_NODENAME(3)	//"1.3.6.1.4.1.8888.1.6.1.1.3.0" //软件版本
#define NODE_COMMHARDWAREVER					MAKE_COMM_NODENAME(4)	//"1.3.6.1.4.1.8888.1.6.1.1.4.0" //硬件版本
#define	NODE_COMMOSTYPE							MAKE_COMM_NODENAME(5)	//"1.3.6.1.4.1.8888.1.6.1.1.5.0" //设备OS类型
#define	NODE_COMMSUBTYPE						MAKE_COMM_NODENAME(6)	//"1.3.6.1.4.1.8888.1.6.1.1.6.0" //设备子类型
#define NODE_COMMLOCALINFOS						MAKE_COMM_NODENAME(7)	//"1.3.6.1.4.1.8888.1.6.1.1.7.0" //设备的补充信息（比如字符串编码）
//pfm 2
#define NODE_COMMCPURATE						MAKE_COMM_NODENAME(30)  //"1.3.6.1.4.1.8888.1.6.1.2.1.0" //cpu百分比   超过90%上报
#define NODE_COMMMEMRATE						MAKE_COMM_NODENAME(31)  //"1.3.6.1.4.1.8888.1.6.1.2.2.0" //内存百分比   超过90%上报
#define	NODE_COMMDISKRATE						MAKE_COMM_NODENAME(32)  //"1.3.6.1.4.1.8888.1.6.1.2.3.0" //磁盘百分比   超过90%上报
#define NODE_COMMLEDSTATE						MAKE_COMM_NODENAME(33)  //"1.3.6.1.4.1.8888.1.6.1.2.4.0" //设备灯的状态
//conference 3

//config 4
#define NODE_COMMIPS							MAKE_COMM_NODENAME(80)  //"1.3.6.1.4.1.8888.1.6.1.4.1.0" //多个IP
#define	NODE_COMMMACS							MAKE_COMM_NODENAME(81)  //"1.3.6.1.4.1.8888.1.6.1.4.2.0" //多个MAC
#define NODE_COMMDEVTIME						MAKE_COMM_NODENAME(82)  //"1.3.6.1.4.1.8888.1.6.1.4.3.0" //设备时间
#define NODE_COMMTRAPIPS						MAKE_COMM_NODENAME(83)  //"1.3.6.1.4.1.8888.1.6.1.4.4.0" //多个trapIP
#define NODE_COMMIP								MAKE_COMM_NODENAME(84)  //"1.3.6.1.4.1.8888.1.6.1.4.5.0" //IP
#define NODE_COMMMASKIP							MAKE_COMM_NODENAME(85)  //"1.3.6.1.4.1.8888.1.6.1.4.6.0" //MASKIP
#define NODE_COMMGATEWAYIP						MAKE_COMM_NODENAME(86)  //"1.3.6.1.4.1.8888.1.6.1.4.7.0" //GatewayIP

//alarm  5
#define NODE_COMMALARMNOTIFY					MAKE_COMM_NODENAME(160) //"1.3.6.1.4.1.8888.1.6.1.5.1.0" //alarm的头

//table  6
//////////////////////////////////////////////////////////////////////////

///////////////////////////////recorder///////////////////////////////////////////
//status 1

//pfm 2

//conference 3

//config 4
#define NODE_RCD_PEOPLEONDEMAND					MAKE_RCD_NODENAME(80)   //"1.3.6.1.4.1.8888.1.7.1.4.1.0" //在线点播人数(高2字节)最大点播人数(低2字节)
//alarm  5
//table  6
//////////////////////////////////////////////////////////////////////////

enum AgentAdapterType
{
    EM_MCUADAPTER,
    EM_MTADAPTER
};

enum CommunityType
{
    READ_COMMUNITY,
    WRITE_COMMUNITY
};


extern BOOL32 g_bThreadExitFlag;

typedef struct tagSnmpAdpParam
{
    u32  dwLocalIp;                             //主机序
    u16  wGetSetPort;                           //默认为161
    u16  wTrapPort;                             //默认为162
    s8   achReadCommunity[MAX_COMMUNITY_LEN];   //默认为public
    s8   achWriteCommunity[MAX_COMMUNITY_LEN];  //默认为private
}TSnmpAdpParam;

//目标地址结构
typedef struct tagTarget
{
    u32  dwIp;      //主机序
    u16  wPort;
    s8   achCommunity[MAX_COMMUNITY_LEN];
}TTarget;

class CNodes : public CNodeCollection
{
public:
    u16  AddNodeValue(u32 dwNodeName);

    u16  AddNodeValue(u32 dwNodeName, void * pBuf, u16 wBufLen);

    u16  GetNodeValue(u32 dwNodeName, void * pBuf, u16 wBufLen);
    
    u16  GetNodeValue(u32 dwNodeName, void * pBuf, u16 wBufLen, s32 nIndex);

    u16  GetNodeNum(void);

    u16  SetTrapType(u16 wTrapType);

    u16  Clear(void);

};

#define READ_FLAG     0
#define WRITE_FLAG    1

/************************************************************************/
/* Agent读写回调函数                                                    */
/************************************************************************/
typedef u16 (*TAgentCallBack)(u32 dwNodeName, u8 byRWFlag, void * pBuf, u16 * pwBufLen);

/************************************************************************/
/* Manager读写回调函数                                                  */
/************************************************************************/
typedef u16 (*TManagerCallBack)(u16 wReason, TTarget & tTarget, CNodes & cNodes, void * pCBData);

/************************************************************************/
/* Manager告警回调函数                                                  */
/************************************************************************/
typedef u16 (*TSnmpTrapCallBack)(u16 wReason, TTarget & tTarget, CNodes & cNodes, void * pCBData);


class CSnmpAdapter;

class CAgentAdapter
{
public:
    CAgentAdapter();
    virtual ~CAgentAdapter();

    u16  Initialize(TAgentCallBack tSnmpCallBack, TSnmpAdpParam * ptSnmpAdpParam = NULL);     //初始化Agent Adapter
    
    u16  SetCommunity(s8 * pchCommunity, u8 byCommunityType);   //设置读写共同体

    u16  SetAgentCallBack(TAgentCallBack tSnmpCallBack);        //设置Agent端的读写回调函数

    u16  SendTrap(TTarget & tTrapRcvAddr, CNodes & cNodes);     //Agent端发TRAP的函数

    u16  SnmpTargetAdd(TTarget & tTrapRcvAddr);                 //[LINUX]下  对Trap目标地址的增加

    u16  SnmpTargetDel(TTarget & tTrapRcvAddr);                 //[LINUX]下  对Trap目标地址的删除
    
    u16  FreeNodes(CNodes & cNodes);                            //节点内存空间释放(linux)

    u16  Quit();                                                //退出Agent Adapter

protected:

private:
    CSnmpAdapter * m_pcSnmpAdp;

};

class CManagerAdapter
{

public:
    CManagerAdapter();
    virtual ~CManagerAdapter();

	u16  Initialize(TSnmpAdpParam * ptSnmpAdpParam = NULL);     //初始化Snmp Adapter

    u16  SetCommunity(s8 * pchCommunity, u8 byCommunityType);   //设置读写共同体
	u16  SetSendTimeOut(u16 timeout = 100);							//设置发送时的超时
    //同步
    //异步
    u16  GetAgentNodeValue(TTarget & tTargetAddr,
                           CNodes & cNodes,
                           TManagerCallBack tCallBack = NULL,
                           void * pCBData = NULL);

    //同步
    //异步
    u16  GetNextAgentNodeValue(TTarget & tTargetAddr,
                               CNodes & cNodes,
                               TManagerCallBack tCallBack = NULL,
                               void * pCBData = NULL);

    //同步
    //异步
    u16  SetAgentNodeValue(TTarget & tTargetAddr,
                           CNodes & cNodes,
                           TManagerCallBack tCallBack = NULL,
                           void * pCBData = NULL);
    //设置告警回调
    u16  SetSnmpTrapCallBack(TTarget & tTrapRcvAddr,
                             TSnmpTrapCallBack tTrapCallBack = NULL,
                             void * pCBData = NULL);          //

    //
    const s8 * GetErrorMsg(const u16 wErrorNo);

	void StopThread();

	void Release();

protected:

private:
    CSnmpAdapter * m_pcSnmpAdp;
};

#endif  //!_SNMP_AGENTADP_H_
