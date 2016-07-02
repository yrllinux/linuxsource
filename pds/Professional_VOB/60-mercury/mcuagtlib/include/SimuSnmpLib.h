
// simuSnmpLib.h

#ifndef _SIMUSNMPLIBTEST_
#define _SIMUSNMPLIBTEST_

#include "kdvtype.h"
#include "osp.h"
#include "mcuagtstruct.h"
#include "agtcomm.h"
#pragma  once


#define MAX_COMMUNITY_LEN   32

#define  MCUMIB  1
#define  MTMIB   2 


#define MAKE_NODENAME(A, C)  (((u32)(A) << 24) + ((u32)(C) & 0x0000FFFF))
#define MAKE_TABLE_NODENAME(A, B, C)  ((((u32)(B) << 16) & 0x00FF0000) + MAKE_NODENAME(A, C))

#define GET_NODE_ROW(D)  ((u8)(((D) >> 16) & 0xFF))
#define GET_NODENAME(D)  ((D) & 0xFF00FFFF)

#define MAKE_MCU_NODENAME(C)  MAKE_NODENAME(MCUMIB, C)
#define MAKE_MCUTABLE_NODENAME(B, C)  MAKE_TABLE_NODENAME(MCUMIB, B, C)

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

//Mcu Local
#define NODE_MCUID                           MAKE_MCU_NODENAME(20)      //"1.3.6.1.4.1.8888.1.1.1.2.1.0"
#define NODE_MCUALIAS                        MAKE_MCU_NODENAME(21)      //"1.3.6.1.4.1.8888.1.1.1.2.2.0"
#define NODE_MCUE164NUMBER                   MAKE_MCU_NODENAME(22)      //"1.3.6.1.4.1.8888.1.1.1.2.3.0"

//Mcu Net
#define NODE_MCUNETGKIPADDR                  MAKE_MCU_NODENAME(30)      //"1.3.6.1.4.1.8888.1.1.1.3.1.0"
#define NODE_MCUNETMULTICASTIPADDR           MAKE_MCU_NODENAME(31)      //"1.3.6.1.4.1.8888.1.1.1.3.2.0"
#define NODE_MCUNETMULTICASTPORT             MAKE_MCU_NODENAME(32)      //"1.3.6.1.4.1.8888.1.1.1.3.3.0"
#define NODE_MCUNETSTARTRECVPORT             MAKE_MCU_NODENAME(33)      //"1.3.6.1.4.1.8888.1.1.1.3.4.0"
#define NODE_MCUNET225245STARTPORT           MAKE_MCU_NODENAME(34)      //"1.3.6.1.4.1.8888.1.1.1.3.5.0"
#define NODE_MCUNET225245MAXMTNUM            MAKE_MCU_NODENAME(35)      //"1.3.6.1.4.1.8888.1.1.1.3.6.0"
#define NODE_MCUNETUSEMPCTRANSDATA           MAKE_MCU_NODENAME(36)      //"1.3.6.1.4.1.8888.1.1.1.3.7.0"
#define NODE_MCUNETUSEMPCSTACK               MAKE_MCU_NODENAME(37)      //"1.3.6.1.4.1.8888.1.1.1.3.8.0"
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
#define NODE_MCUEQPBRDCFGID                  MAKE_MCU_NODENAME(60)      //"1.3.6.1.4.1.8888.1.1.1.3.11.1.1"
#define NODE_MCUEQPBRDCFGLAYER               MAKE_MCU_NODENAME(61)      //"1.3.6.1.4.1.8888.1.1.1.3.11.1.2"
#define NODE_MCUEQPBRDCFGSLOT                MAKE_MCU_NODENAME(62)      //"1.3.6.1.4.1.8888.1.1.1.3.11.1.3"
#define NODE_MCUEQPBRDCFGTYPE                MAKE_MCU_NODENAME(63)      //"1.3.6.1.4.1.8888.1.1.1.3.11.1.4"
#define NODE_MCUEQPBRDCFGSTATUS              MAKE_MCU_NODENAME(64)      //"1.3.6.1.4.1.8888.1.1.1.3.11.1.5"
#define NODE_MCUEQPBRDCFGVERSION             MAKE_MCU_NODENAME(65)      //"1.3.6.1.4.1.8888.1.1.1.3.11.1.6"
#define NODE_MCUEQPBRDCFGIPADDR              MAKE_MCU_NODENAME(66)      //"1.3.6.1.4.1.8888.1.1.1.3.11.1.7"
#define NODE_MCUEQPBRDCFGMODULE              MAKE_MCU_NODENAME(67)      //"1.3.6.1.4.1.8888.1.1.1.3.11.1.8"
#define NODE_MCUEQPBRDCFGPANELLED            MAKE_MCU_NODENAME(68)      //"1.3.6.1.4.1.8888.1.1.1.3.11.1.9"

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
#define NODE_MCUEQPRECORDERRECORDCHANNELNUM  MAKE_MCU_NODENAME(91)      //"1.3.6.1.4.1.8888.1.1.1.4.2.1.7"
#define NODE_MCUEQPRECORDERPLAYCHANNELNUM    MAKE_MCU_NODENAME(92)      //"1.3.6.1.4.1.8888.1.1.1.4.2.1.8"
#define NODE_MCUEQPRECORDERCONNSTATE         MAKE_MCU_NODENAME(93)      //"1.3.6.1.4.1.8888.1.1.1.4.2.1.9"
#define NODE_MCUEQPRECORDEREXIST             MAKE_MCU_NODENAME(94)      //"1.3.6.1.4.1.8888.1.1.1.4.2.1.10"

//Mcu Eqp - TvWall table
#define NODE_MCUEQPTVWALLID                  MAKE_MCU_NODENAME(100)     //"1.3.6.1.4.1.8888.1.1.1.4.3.1.1"
#define NODE_MCUEQPTVWALLALIAS               MAKE_MCU_NODENAME(101)     //"1.3.6.1.4.1.8888.1.1.1.4.3.1.2"
#define NODE_MCUEQPTVWALLRUNNINGBRDID        MAKE_MCU_NODENAME(102)     //"1.3.6.1.4.1.8888.1.1.1.4.3.1.3"
#define NODE_MCUEQPTVWALLIPADDR              MAKE_MCU_NODENAME(103)     //"1.3.6.1.4.1.8888.1.1.1.4.3.1.4"
#define NODE_MCUEQPTVWALLDIVSTYLE            MAKE_MCU_NODENAME(104)     //"1.3.6.1.4.1.8888.1.1.1.4.3.1.5"
#define NODE_MCUEQPTVWALLDIVNUM              MAKE_MCU_NODENAME(105)     //"1.3.6.1.4.1.8888.1.1.1.4.3.1.6"
#define NODE_MCUEQPTVWALLVIDEOSTARTRECVPORT  MAKE_MCU_NODENAME(106)     //"1.3.6.1.4.1.8888.1.1.1.4.3.1.7"
#define NODE_MCUEQPTVWALLCONNSTATE           MAKE_MCU_NODENAME(107)     //"1.3.6.1.4.1.8888.1.1.1.4.3.1.8"
#define NODE_MCUEQPTVWALLEXIST               MAKE_MCU_NODENAME(108)     //"1.3.6.1.4.1.8888.1.1.1.4.3.1.9"

//Mcu Eqp - Bas table
#define NODE_MCUEQPBASID                     MAKE_MCU_NODENAME(110)     //"1.3.6.1.4.1.8888.1.1.1.4.4.1.1"
#define NODE_MCUEQPBASPORT                   MAKE_MCU_NODENAME(111)     //"1.3.6.1.4.1.8888.1.1.1.4.4.1.2"
#define NODE_MCUEQPBASSWITCHBRDID            MAKE_MCU_NODENAME(112)     //"1.3.6.1.4.1.8888.1.1.1.4.4.1.3"
#define NODE_MCUEQPBASALIAS                  MAKE_MCU_NODENAME(113)     //"1.3.6.1.4.1.8888.1.1.1.4.4.1.4"
#define NODE_MCUEQPBASRUNNINGBRDID           MAKE_MCU_NODENAME(114)     //"1.3.6.1.4.1.8888.1.1.1.4.4.1.5"
#define NODE_MCUEQPBASIPADDR                 MAKE_MCU_NODENAME(115)     //"1.3.6.1.4.1.8888.1.1.1.4.4.1.6"
#define NODE_MCUEQPBASSTARTRECVPORT          MAKE_MCU_NODENAME(116)     //"1.3.6.1.4.1.8888.1.1.1.4.4.1.7"
#define NODE_MCUEQPBASCONNSTATE              MAKE_MCU_NODENAME(117)     //"1.3.6.1.4.1.8888.1.1.1.4.4.1.9"
#define NODE_MCUEQPBASEXIST                  MAKE_MCU_NODENAME(118)     //"1.3.6.1.4.1.8888.1.1.1.4.4.1.10"

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


#define TRAP_COLD_RESTART       (u16)1
#define TRAP_POWEROFF           (u16)2
#define TRAP_ALARM_GENERATE     (u16)3
#define TRAP_ALARM_RESTORE      (u16)4
#define TRAP_LED_STATE          (u16)5
#define TRAP_CONF_STATE         (u16)6
#define TRAP_MT_STATE           (u16)7

// Agent读写回调函数  
typedef u16 (*TAgentCallBack)(u32 dwNodeName, u8 byRWFlag, void * pBuf, u16 wBufLen);

typedef struct 
{
    u32  dwLocalIp;                             //主机序
    u16  wGetSetPort;                           //默认为161
    u16  wTrapPort;                             //默认为162
    s8   achReadCommunity[MAX_COMMUNITY_LEN];   //默认为public
    s8   achWriteCommunity[MAX_COMMUNITY_LEN];  //默认为private
}TSnmpAdpParam;

//目标地址结构
typedef struct 
{
    u32  dwIp;      //主机序
    u16  wPort;
}TTarget;

extern TMcupfmAlarmEntry  achTest;
extern u8  byAlarmName;

class CNodeCollection
{
public:
    CNodeCollection(void);
    ~CNodeCollection();
};

class CNodes : public CNodeCollection
{
public:
//    u16  AddNodeValue(u32 dwNodeName);

    u16  AddNodeValue(u32 dwNodeName, void * pBuf, u16 wBufLen);

    u16  GetNodeValue(u32 dwNodeName, void * pBuf, u16 wBufLen);

    u16  GetNodeNum(void);

    u16  SetTrapType(u16 wTrapType);
    
    u16  GetTrapType();

    u16  Clear(void);

public:
    

};

extern TAgentCallBack g_AgentCallBack;

class CAgentAdapter
{

public:
    //u16  Initialize(TSnmpAdpParam * ptSnmpAdpParam = NULL);     //初始化Agent Adapter
	u16  Initialize(TAgentCallBack tSnmpCallBack, TSnmpAdpParam * ptSnmpAdpParam = NULL);     //初始化Agent Adapter

    u16  SetCommunity(s8 * pchCommunity, u8 byCommunityType);   //设置读写共同体

    u16  SetAgentCallBack(TAgentCallBack tSnmpCallBack);        //设置Agent端的读写回调函数

    u16  SendTrap(TTarget tTrapRcvAddr, CNodes & cNodes);       //Agent端发TRAP的函数
    
    u16  GetTrapType(CNodes cNodes);

    u16  GetTrapType();

    u16  GetAlarmCode();
    
protected:
    TAgentCallBack m_tAgentCallBack;

private:

};


#endif