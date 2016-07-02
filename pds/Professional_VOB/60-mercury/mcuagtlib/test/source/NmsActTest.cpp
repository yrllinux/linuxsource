

// NmsActTest.cpp
#include "NmsActTest.h"

TAgentCallBack g_AgentCallBack;

#define MCUID_TEST        192
#define MCUALIAS_TEST     "MCU16"
#define MCUE164_TEST     "88888"
#define CASTPORT_TEST     60100
#define RECVPORT_TEST     61000
#define H245PORT_TEST     60002
#define MCU_MTNUM         190
#define MCU_TRAPIPADDR    0x64010101    // 100.1.1.1
#define TRAPWCOMMUNITY    "private"
#define TRAPRCOMMUNITY    "public"

/*=============================================================================
  函 数 名： ActMcuId
  功    能： 操作Mcu Id
  算法实现： 
  全局变量： 
  参    数： void
  返 回 值： void 
=============================================================================*/
void CNmsActTest::ActMcuId(void)
{
    u16  wSrcId = 0;
    u16  wDstId = 193;
    u32  dwNode = NODE_MCUID;
    g_AgentCallBack(dwNode, 0, &wSrcId, sizeof(wSrcId));
    OspDelay(500);
    CPPUNIT_ASSERT( wSrcId == MCUID_TEST);

    g_AgentCallBack(dwNode, 1, wDstId, sizeof(wDstId));

    g_AgentCallBack(dwNode, 0, &wSrcId, sizeof(wSrcId));
    CPPUNIT_ASSERT(wSrcId == wDstId);
    return;
}

/*=============================================================================
  函 数 名： ActMcuAlias
  功    能： 操作mcu 别名
  算法实现： 
  全局变量： 
  参    数： void
  返 回 值： void 
=============================================================================*/
void CNmsActTest::ActMcuAlias(void)
{
    u8 achSrcAlias[32];
    u8 achDstAlias[32];
    memset(achSrcAlias, '\0', sizeof(achSrcAlias));
    memset(achDstAlias, '\0', sizeof(achDstAlias));
    
    u32 dwNode = NODE_MCUALIAS;
    g_AgentCallBack(dwNode, 0, &achSrcAlias, sizeof(achSrcAlias));
    OspDelay(500);
    CPPUNIT_ASSERT(0 == strcmp(achSrcAlias, MCUALIAS_TEST));
    
    memcpy(achDstAlias, "MCU15", sizeof("MCU15"));
    g_AgentCallBack(dwNode, 1, achDstAlias, sizeof(achDstAlias));

    g_AgentCallBack(dwNode, 0, &achSrcAlias, sizeof(achSrcAlias));
    CPPUNIT_ASSERT(0 == strcmp(achDstAlias, achSrcAlias));
    return;
    
} 
   
/*=============================================================================
  函 数 名： ActMcuE164
  功    能： 操作E164号
  算法实现： 
  全局变量： 
  参    数： void
  返 回 值： void 
=============================================================================*/
void CNmsActTest::ActMcuE164(void)
{
    u8 achSrcAlias[32];
    u8 achDstAlias[32];
    memset(achSrcAlias, '\0', sizeof(achSrcAlias));
    memset(achDstAlias, '\0', sizeof(achDstAlias));
    
    u32 dwNode = NODE_MCUE164NUMBER;
    g_AgentCallBack(dwNode, 0, &achSrcAlias, sizeof(achSrcAlias));
    OspDelay(500);
    CPPUNIT_ASSERT(0 == strcmp(achSrcAlias, MCUE164_TEST));
    
    memcpy(achDstAlias, "99999", sizeof("99999"));
    g_AgentCallBack(dwNode, 1, achDstAlias, sizeof(achDstAlias));

    g_AgentCallBack(dwNode, 0, &achSrcAlias, sizeof(achSrcAlias));
    CPPUNIT_ASSERT(0 == strcmp(achDstAlias, achSrcAlias));
    return;
    
}

void CNmsActTest::ActIsNeedBas(void)
{
    
}
 
// get/set network info
/*=============================================================================
  函 数 名： ActGk
  功    能： 操作Gk信息
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void 
=============================================================================*/
void CNmsActTest::ActGk(void)
{
    u32 dwSrcIp = 0;
    u32 dwDstIp = 0x01010101; // 1.1.1.1
    u32 dwNode = NODE_MCUNETGKIPADDR;

    g_AgentCallBack(dwNode, 0, &dwSrcIp, sizeof(dwSrcIp));
    CPPUNIT_ASSERT(dwSrcIp == 0);

    g_AgentCallBack(dwNode, 1, dwDstIp, sizeof(dwDstIp));
    OspDelay(1000);
    g_AgentCallBack(dwNode, 0, &dwSrcIp, sizeof(dwSrcIp));
    CPPUNIT_ASSERT(dwSrcIp == dwDstIp);

    return;

}
/*=============================================================================
  函 数 名： ActCast
  功    能： 操作组播Ip和端口
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void 
=============================================================================*/
void CNmsActTest::ActCast()
{
    u32 dwNode1 =  NODE_MCUNETMULTICASTIPADDR;
    u32 dwNode2 = NODE_MCUNETMULTICASTPORT;
    u32 dwSrcIp = 0;
    u32 dwDstIp = 0xff0a0a0a;  // 255.10.10.10
    
    u16 wSrcPort = 0;
    u16 wDstPort = CASTPORT_TEST + 10;
    
    // cast ip
    g_AgentCallBack(dwNode1, 0, &dwSrcIp, sizeof(dwSrcIp));
    CPPUNIT_ASSERT(dwSrcIp == 0);

    g_AgentCallBack(dwNode1, 1, dwDstIp, sizeof(dwDstIp));
    OspDelay(1000);
    g_AgentCallBack(dwNode1, 0, &dwSrcIp, sizeof(dwSrcIp));
    CPPUNIT_ASSERT(dwSrcIp == dwDstIp);

    // cast port
    g_AgentCallBack(dwNode2, 0, &wSrcPort, sizeof(wSrcPort));
    CPPUNIT_ASSERT(wSrcPort == CASTPORT_TEST);

    g_AgentCallBack(dwNode2, 1, wDstPort, sizeof(wDstPort));
    OspDelay(1000);
    g_AgentCallBack(dwNode2, 0, &wSrcPort, sizeof(wSrcPort));
    CPPUNIT_ASSERT(wDstPort == wSrcPort);
    return;

}

/*=============================================================================
  函 数 名： ActRecPort
  功    能： 操作接受端口
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void 
=============================================================================*/
void CNmsActTest::ActRecPort()
{
    u32 dwNode =  NODE_MCUNETSTARTRECVPORT;
    u16 wSrcPort = 0;
    u16 wDstPort = RECVPORT_TEST + 10;

    g_AgentCallBack(dwNode, 0, &wSrcPort, sizeof(wSrcPort));
    CPPUNIT_ASSERT(wSrcPort == CASTPORT_TEST);

    g_AgentCallBack(dwNode, 1, wDstPort, sizeof(wDstPort));
    OspDelay(1000);
    g_AgentCallBack(dwNode, 0, &wSrcPort, sizeof(wSrcPort));
    CPPUNIT_ASSERT(wDstPort == wSrcPort);
    return;

}

/*=============================================================================
  函 数 名： ActH225
  功    能： 操作H224、245端口
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void 
=============================================================================*/
void CNmsActTest::ActH225()
{
    u32 dwNode =  NODE_MCUNET225245STARTPORT;
    u16 wSrcPort = 0;
    u16 wDstPort = H245PORT_TEST + 10;

    g_AgentCallBack(dwNode, 0, &wSrcPort, sizeof(wSrcPort));
    CPPUNIT_ASSERT(wSrcPort == CASTPORT_TEST);

    g_AgentCallBack(dwNode, 1, wDstPort, sizeof(wDstPort));
    OspDelay(1000);
    g_AgentCallBack(dwNode, 0, &wSrcPort, sizeof(wSrcPort));
    CPPUNIT_ASSERT(wDstPort == wSrcPort);
    return;

}
/*=============================================================================
  函 数 名： ActMtNum
  功    能： 操作终端数
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void 
=============================================================================*/
void CNmsActTest::ActMtNum()
{
    u32 dwNode =  NODE_MCUNET225245MAXMTNUM;
    u16 wSrcValue = 0;
    u16 wDstValue = MCU_MTNUM;

    g_AgentCallBack(dwNode, 0, &wSrcValue, sizeof(wSrcValue));
    CPPUNIT_ASSERT(wSrcValue == 192);

    g_AgentCallBack(dwNode, 1, wDstValue, sizeof( wDstValue));
    OspDelay(1000);

    g_AgentCallBack(dwNode, 0, &wSrcValue, sizeof(wSrcValue));
    CPPUNIT_ASSERT(wSrcValue == wDstValue);
    return;

}

/*=============================================================================
  函 数 名： ActIsUseMpcTranData
  功    能： 操作IsUseMpcTranData字段
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void 
=============================================================================*/
void CNmsActTest::ActIsUseMpcTranData()
{
    u16 wSrcValue = 0;
    u16 wDstValue = 1;
    u32 dwNode = NODE_MCUNETUSEMPCTRANSDATA;

    g_AgentCallBack(dwNode, 0, &wSrcValue, sizeof(wSrcValue));
    CPPUNIT_ASSERT(wSrcValue == 0);

    g_AgentCallBack(dwNode, 1, wDstValue, sizeof( wDstValue));
    OspDelay(1000);

    g_AgentCallBack(dwNode, 0, &wSrcValue, sizeof(wSrcValue));
    CPPUNIT_ASSERT(wSrcValue == wDstValue);
    return;

}
/*=============================================================================
  函 数 名： ActIsUseMpcStack
  功    能： 操作IsUseMpcStack字段
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void 
=============================================================================*/
void CNmsActTest::ActIsUseMpcStack()
{
    u16 wSrcValue = 0;
    u16 wDstValue = 0;
    u32 wNode = NODE_MCUNETUSEMPCSTACK;

    g_AgentCallBack(dwNode, 0, &wSrcValue, sizeof(wSrcValue));
    CPPUNIT_ASSERT(wSrcValue == 1);

    g_AgentCallBack(dwNode, 1, wDstValue, sizeof( wDstValue));
    OspDelay(1000);

    g_AgentCallBack(dwNode, 0, &wSrcValue, sizeof(wSrcValue));
    CPPUNIT_ASSERT(wSrcValue == wDstValue);
    return;

}

// trap 


//  函 数 名： ActTrapNum
//  功    能： 保留
void CNmsActTest::ActTrapNum()
{
    
}

/*=============================================================================
  函 数 名： ActTrapIp
  功    能： 操作TrapIp字段
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void 
=============================================================================*/
void CNmsActTest::ActTrapIp()
{
    u32 dwSrcValue = 0;
    u32 dwDstValue = 0x01010101;   // 1.1.1.1
    u8  byRow = 1;
    u32 wNode = MAKE_MCUTABLE_NODENAME(byRow, NODE_MCUNETSNMPCFGIPADDR);

    g_AgentCallBack(dwNode, 0, &dwSrcValue, sizeof(dwSrcValue));
    CPPUNIT_ASSERT(wSrcValue == MCU_TRAPIPADDR);

    g_AgentCallBack(dwNode, 1, dwDstValue, sizeof( dwDstValue));
    OspDelay(1000);

    g_AgentCallBack(dwNode, 0, &dwSrcValue, sizeof(dwSrcValue));
    CPPUNIT_ASSERT( dwSrcValue == dwDstValue );
    return;
    
}

/*=============================================================================
  函 数 名： ActTrapCommunity
  功    能： 操作共同体字段
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void 
=============================================================================*/
void CNmsActTest::ActTrapCommunity()
{
    u8 achSrcValue[32];
    memset(achSrcValue, '\0', sizeof(achSrcValue));
    u8 achDstValue[32];
    memset(achDstValue, '\0', sizeof(achDstValue));
    u8  byRow = 1;

    // 读共同体
    u32 wNode = MAKE_MCUTABLE_NODENAME(byRow, NODE_MCUNETSNMPCFGREADCOMMUNITY);
    g_AgentCallBack(wNode, 0, achSrcValue, sizeof(achSrcValue));
    CPPUNIT_ASSERT(0 == strcmp(achSrcValue, TRAPRCOMMUNITY));

    memcpy(achDstValue, "public1", sizeof("public1"));
    g_AgentCallBack(wNode, 1, achDstValue, sizeof(achDstValue));
    OspDelay(1000);
    g_AgentCallBack(wNode, 0, achSrcValue, sizeof(achSrcValue));
    CPPUNIT_ASSERT( 0 == strcmp(achSrcValue, achDstValue));
    
    // 写共同体
    wNode = MAKE_MCUTABLE_NODENAME(byRow, NODE_MCUNETSNMPCFGWRITECOMMUNITY);
    g_AgentCallBack(wNode, 0, achSrcValue, sizeof(achSrcValue));
    CPPUNIT_ASSERT(0 == strcmp(achSrcValue, TRAPWCOMMUNITY));

    memcpy(achDstValue, "private1", sizeof("private1"));
    g_AgentCallBack(wNode, 1, achDstValue, sizeof(achDstValue));
    OspDelay(1000);
    g_AgentCallBack(wNode, 0, achSrcValue, sizeof(achSrcValue));
    CPPUNIT_ASSERT( 0 == strcmp(achSrcValue, achDstValue));

}
/*=============================================================================
  函 数 名： ActTrapPort
  功    能： 操作端口字段
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void 
=============================================================================*/
void CNmsActTest::ActTrapPort()
{
    u32 dwSrcValue = 0;
    u32 dwDstValue = 160;   // 1.1.1.1
    u8  byRow = 1;

    // snmp get/set port
    u32 wNode = MAKE_MCUTABLE_NODENAME(byRow, NODE_MCUNETSNMPCFGGETSETPORT);

    g_AgentCallBack(dwNode, 0, &dwSrcValue, sizeof(dwSrcValue));
    CPPUNIT_ASSERT(wSrcValue == 161);

    g_AgentCallBack(dwNode, 1, dwDstValue, sizeof( dwDstValue));
    OspDelay(1000);

    g_AgentCallBack(dwNode, 0, &dwSrcValue, sizeof(dwSrcValue));
    CPPUNIT_ASSERT( dwSrcValue == dwDstValue );

    // snmp send trap port
    wNode = MAKE_MCUTABLE_NODENAME(byRow, NODE_MCUNETSNMPCFGTRAPPORT);

    g_AgentCallBack(dwNode, 0, &dwSrcValue, sizeof(dwSrcValue));
    CPPUNIT_ASSERT(wSrcValue == 162);
    
    dwDstValue = 163;
    g_AgentCallBack(dwNode, 1, dwDstValue, sizeof( dwDstValue));
    OspDelay(1000);

    g_AgentCallBack(dwNode, 0, &dwSrcValue, sizeof(dwSrcValue));
    CPPUNIT_ASSERT( dwSrcValue == dwDstValue );
    return;

}

// board 
/*=============================================================================
  函 数 名： ActBrdId
  功    能： 操作BrdId字段
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void 
=============================================================================*/
void CNmsActTest::ActBrdId()
{
    u32 dwSrcValue = 0;
    u8  byRow = 1;
    u32 wNode = MAKE_MCUTABLE_NODENAME(byRow, NODE_MCUEQPBRDCFGID);

    g_AgentCallBack(dwNode, 0, &dwSrcValue, sizeof(dwSrcValue));
    CPPUNIT_ASSERT(wSrcValue == 1);

    return;  
}

void CNmsActTest::ActBrdNum()
{
    
}

/*=============================================================================
  函 数 名： ActBrdIp
  功    能： 操作BrdIp字段
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void 
=============================================================================*/
void CNmsActTest::ActBrdIp()
{
    u32 dwSrcValue = 0;
    u32 dwDstValue = 0x02020202;   // 1.1.1.1
    u8  byRow = 1;
    u32 wNode = MAKE_MCUTABLE_NODENAME(byRow, NODE_MCUEQPBRDCFGIPADDR);

    g_AgentCallBack(dwNode, 0, &dwSrcValue, sizeof(dwSrcValue));
    CPPUNIT_ASSERT(wSrcValue == 0x01010101);

    g_AgentCallBack(dwNode, 1, dwDstValue, sizeof( dwDstValue));
    OspDelay(1000);

    g_AgentCallBack(dwNode, 0, &dwSrcValue, sizeof(dwSrcValue));
    CPPUNIT_ASSERT( dwSrcValue == dwDstValue );
    return;
    
}
/*=============================================================================
  函 数 名： ActBrdType
  功    能： 操作BrdType字段
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void 
=============================================================================*/
void CNmsActTest::ActBrdType()
{
    u32 dwSrcValue = 0;
    u32 dwDstValue = 3;   
    u8  byRow = 1;
    u32 wNode = MAKE_MCUTABLE_NODENAME(byRow, NODE_MCUEQPBRDCFGTYPE);

    g_AgentCallBack(dwNode, 0, &dwSrcValue, sizeof(dwSrcValue));
    CPPUNIT_ASSERT(wSrcValue == 0);

    g_AgentCallBack(dwNode, 1, dwDstValue, sizeof( dwDstValue));
    OspDelay(1000);

    g_AgentCallBack(dwNode, 0, &dwSrcValue, sizeof(dwSrcValue));
    CPPUNIT_ASSERT( dwSrcValue == dwDstValue );
    return; 
}
/*=============================================================================
  函 数 名： ActBrdLayer
  功    能： 操作BrdLayer字段
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void 
=============================================================================*/
void CNmsActTest::ActBrdLayer()
{
    u32 dwSrcValue = 0;
    u32 dwDstValue = 3;   
    u8  byRow = 1;
    u32 wNode = MAKE_MCUTABLE_NODENAME(byRow, NODE_MCUEQPBRDCFGLAYER);

    g_AgentCallBack(dwNode, 0, &dwSrcValue, sizeof(dwSrcValue));
    CPPUNIT_ASSERT(wSrcValue == 1);

    g_AgentCallBack(dwNode, 1, dwDstValue, sizeof( dwDstValue));
    OspDelay(1000);

    g_AgentCallBack(dwNode, 0, &dwSrcValue, sizeof(dwSrcValue));
    CPPUNIT_ASSERT( dwSrcValue == dwDstValue );
    return; 

}

/*=============================================================================
  函 数 名： ActBrdSlot
  功    能： 操作BrdSlot字段
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void 
=============================================================================*/
void CNmsActTest::ActBrdSlot()
{
    u32 dwSrcValue = 0;
    u32 dwDstValue = 10;   
    u8  byRow = 1;
    u32 wNode = MAKE_MCUTABLE_NODENAME(byRow, NODE_MCUEQPBRDCFGSLOT);

    g_AgentCallBack(dwNode, 0, &dwSrcValue, sizeof(dwSrcValue));
    CPPUNIT_ASSERT(wSrcValue == 8);

    g_AgentCallBack(dwNode, 1, dwDstValue, sizeof( dwDstValue));
    OspDelay(1000);

    g_AgentCallBack(dwNode, 0, &dwSrcValue, sizeof(dwSrcValue));
    CPPUNIT_ASSERT( dwSrcValue == dwDstValue );
    return; 

}
/*=============================================================================
  函 数 名： ActBrdChoosePort
  功    能： 操作BrdchoosePort字段
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void 
=============================================================================*/
void CNmsActTest::ActBrdChoosePort()
{
    u32 dwSrcValue = 0;
    u32 dwDstValue = 0;   
    u8  byRow = 1;
    u32 wNode = MAKE_MCUTABLE_NODENAME(byRow, NODE_MCUEQPBRDCFGSLOT);

    g_AgentCallBack(dwNode, 0, &dwSrcValue, sizeof(dwSrcValue));
    CPPUNIT_ASSERT(wSrcValue == 1);

    g_AgentCallBack(dwNode, 1, dwDstValue, sizeof( dwDstValue));
    OspDelay(1000);

    g_AgentCallBack(dwNode, 0, &dwSrcValue, sizeof(dwSrcValue));
    CPPUNIT_ASSERT( dwSrcValue == dwDstValue );
    return; 

}
  
// mixer

/*=============================================================================
  函 数 名： ActMixId
  功    能： 操作Mixid字段
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void 
=============================================================================*/
void CNmsActTest::ActMixId()
{
    u32 dwSrcValue = 0;
    u8  byRow = 1;
    u32 wNode = MAKE_MCUTABLE_NODENAME(byRow, NODE_MCUEQPMIXERID);

    g_AgentCallBack(dwNode, 0, &dwSrcValue, sizeof(dwSrcValue));
    CPPUNIT_ASSERT(wSrcValue == 1);

    return;  
}

void CNmsActTest::ActMixNum()
{
    
}

/*=============================================================================
  函 数 名： ActMixToMcuPort
  功    能： 操作MixToMcuPort字段
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void 
=============================================================================*/
void CNmsActTest::ActMixToMcuPort()
{
    u32 dwSrcValue = 0;
    u32 dwDstValue = 10020;   
    u8  byRow = 1;
    u32 wNode = MAKE_MCUTABLE_NODENAME(byRow, NODE_MCUEQPMIXERPORT);

    g_AgentCallBack(dwNode, 0, &dwSrcValue, sizeof(dwSrcValue));
    CPPUNIT_ASSERT(wSrcValue == 10000);

    g_AgentCallBack(dwNode, 1, dwDstValue, sizeof( dwDstValue));
    OspDelay(1000);

    g_AgentCallBack(dwNode, 0, &dwSrcValue, sizeof(dwSrcValue));
    CPPUNIT_ASSERT( dwSrcValue == dwDstValue );
    return; 

}
/*=============================================================================
  函 数 名： ActMixSwitchNo
  功    能： 操作MixSwitchNo字段
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void 
=============================================================================*/
void CNmsActTest::ActMixSwitchNo()
{
    u32 dwSrcValue = 0;
    u32 dwDstValue = 3;   
    u8  byRow = 1;
    u32 wNode = MAKE_MCUTABLE_NODENAME(byRow, NODE_MCUEQPMIXERSWITCHBRDID);

    g_AgentCallBack(dwNode, 0, &dwSrcValue, sizeof(dwSrcValue));
    CPPUNIT_ASSERT(wSrcValue == 2);

    g_AgentCallBack(dwNode, 1, dwDstValue, sizeof( dwDstValue));
    OspDelay(1000);

    g_AgentCallBack(dwNode, 0, &dwSrcValue, sizeof(dwSrcValue));
    CPPUNIT_ASSERT( dwSrcValue == dwDstValue );
    return;  

}

/*=============================================================================
  函 数 名： ActMixAlias
  功    能： 操作MixAlias字段
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void 
=============================================================================*/
void CNmsActTest::ActMixAlias()
{
    u8 achSrcAlias[32];
    u8 achDstAlias[32];
    u8 byRow = 1;
    memset(achSrcAlias, '\0', sizeof(achSrcAlias));
    memset(achDstAlias, '\0', sizeof(achDstAlias));
    
    u32 dwNode = MAKE_MCUTABLE_NODENAME(byRow, NODE_MCUEQPMIXERALIAS);

    g_AgentCallBack(dwNode, 0, &achSrcAlias, sizeof(achSrcAlias));
    OspDelay(500);
    CPPUNIT_ASSERT(0 == strcmp(achSrcAlias, "Mixer1"));
    
    memcpy(achDstAlias, "Mix15", sizeof("Mix15"));
    g_AgentCallBack(dwNode, 1, achDstAlias, sizeof(achDstAlias));
    OspDelay(500);

    g_AgentCallBack(dwNode, 0, &achSrcAlias, sizeof(achSrcAlias));
    CPPUNIT_ASSERT(0 == strcmp(achDstAlias, achSrcAlias));
    return;
    
}
/*=============================================================================
  函 数 名： ActMixRunIndex
  功    能： 操作MixRunIdex字段
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void 
=============================================================================*/
void CNmsActTest::ActMixRunIndex()
{
    u32 dwSrcValue = 0;
    u32 dwDstValue = 4;   
    u8  byRow = 1;
    u32 wNode = MAKE_MCUTABLE_NODENAME(byRow, NODE_MCUEQPMIXERRUNNINGBRDID);

    g_AgentCallBack(dwNode, 0, &dwSrcValue, sizeof(dwSrcValue));
    CPPUNIT_ASSERT(wSrcValue == 2);

    g_AgentCallBack(dwNode, 1, dwDstValue, sizeof( dwDstValue));
    OspDelay(1000);

    g_AgentCallBack(dwNode, 0, &dwSrcValue, sizeof(dwSrcValue));
    CPPUNIT_ASSERT( dwSrcValue == dwDstValue );
    return;  

}

/*=============================================================================
  函 数 名： ActMixStartPort
  功    能： 操作MixStartPort字段
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void 
=============================================================================*/
void CNmsActTest::ActMixStartPort()
{
    u32 dwSrcValue = 0;
    u32 dwDstValue = 45010;   
    u8  byRow = 1;
    u32 wNode = MAKE_MCUTABLE_NODENAME(byRow, NODE_MCUEQPMIXERMIXSTARTPORT);

    g_AgentCallBack(dwNode, 0, &dwSrcValue, sizeof(dwSrcValue));
    CPPUNIT_ASSERT(wSrcValue == 45000);

    g_AgentCallBack(dwNode, 1, dwDstValue, sizeof( dwDstValue));
    OspDelay(1000);

    g_AgentCallBack(dwNode, 0, &dwSrcValue, sizeof(dwSrcValue));
    CPPUNIT_ASSERT( dwSrcValue == dwDstValue );
    return;  

}
/*=============================================================================
  函 数 名： ActMixChannel
  功    能： 操作MixChannel字段
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void 
=============================================================================*/
void CNmsActTest::ActMixChannel()
{
    u32 dwSrcValue = 0;
    u32 dwDstValue = 8;   
    u8  byRow = 1;
    u32 wNode = MAKE_MCUTABLE_NODENAME(byRow, NODE_MCUEQPMIXERMAXMIXGROUPNUM);

    g_AgentCallBack(dwNode, 0, &dwSrcValue, sizeof(dwSrcValue));
    CPPUNIT_ASSERT(wSrcValue == 10);

    g_AgentCallBack(dwNode, 1, dwDstValue, sizeof( dwDstValue));
    OspDelay(1000);

    g_AgentCallBack(dwNode, 0, &dwSrcValue, sizeof(dwSrcValue));
    CPPUNIT_ASSERT( dwSrcValue == dwDstValue );
    return;  

}
// 保留
void CNmsActTest::ActMixUseMap()
{
    
}

// recorder
/*=============================================================================
  函 数 名： ActRecId
  功    能： 操作RecId字段
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void 
=============================================================================*/
void CNmsActTest::ActRecId()
{
    u32 dwSrcValue = 0;
    u8  byRow = 1;
    u32 wNode = MAKE_MCUTABLE_NODENAME(byRow, NODE_MCUEQPRECORDERID);

    g_AgentCallBack(dwNode, 0, &dwSrcValue, sizeof(dwSrcValue));
    CPPUNIT_ASSERT(wSrcValue == 17);

    return;  

}

void CNmsActTest::ActRecNum()
{
    
}
/*=============================================================================
  函 数 名： ActRecToMcuPort
  功    能： 操作RecToMcuPort字段
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void 
=============================================================================*/
void CNmsActTest::ActRecToMcuPort()
{
    u32 dwSrcValue = 0;
    u32 dwDstValue = 60000;   
    u8  byRow = 1;
    u32 wNode = MAKE_MCUTABLE_NODENAME(byRow, NODE_MCUEQPRECORDERPORT);

    g_AgentCallBack(dwNode, 0, &dwSrcValue, sizeof(dwSrcValue));
    CPPUNIT_ASSERT(wSrcValue == 60000);
    return; 

}
/*=============================================================================
  函 数 名： ActRecSwitchNo
  功    能： 操作RecSwitchNo字段
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void 
=============================================================================*/
void CNmsActTest::ActRecSwitchNo()
{
    u32 dwSrcValue = 0;
    u32 dwDstValue = 3;   
    u8  byRow = 1;
    u32 wNode = MAKE_MCUTABLE_NODENAME(byRow, NODE_MCUEQPRECORDERSWITCHBRDID);

    g_AgentCallBack(dwNode, 0, &dwSrcValue, sizeof(dwSrcValue));
    CPPUNIT_ASSERT(wSrcValue == 2);

    g_AgentCallBack(dwNode, 1, dwDstValue, sizeof( dwDstValue));
    OspDelay(1000);

    g_AgentCallBack(dwNode, 0, &dwSrcValue, sizeof(dwSrcValue));
    CPPUNIT_ASSERT( dwSrcValue == dwDstValue );
    return;  

}
/*=============================================================================
  函 数 名： ActRecAlias
  功    能： 操作RecAlias字段
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void 
=============================================================================*/
void CNmsActTest::ActRecAlias()
{
    u8 achSrcAlias[32];
    u8 achDstAlias[32];
    u8 byRow = 1;
    memset(achSrcAlias, '\0', sizeof(achSrcAlias));
    memset(achDstAlias, '\0', sizeof(achDstAlias));
    
    u32 dwNode = MAKE_MCUTABLE_NODENAME(byRow, NODE_MCUEQPRECORDERALIAS);

    g_AgentCallBack(dwNode, 0, &achSrcAlias, sizeof(achSrcAlias));
    OspDelay(500);
    CPPUNIT_ASSERT(0 == strcmp(achSrcAlias, "Recorder1"));
    
    memcpy(achDstAlias, "Rec15", sizeof("Rec15"));
    g_AgentCallBack(dwNode, 1, achDstAlias, sizeof(achDstAlias));
    OspDelay(500);

    g_AgentCallBack(dwNode, 0, &achSrcAlias, sizeof(achSrcAlias));
    CPPUNIT_ASSERT(0 == strcmp(achDstAlias, achSrcAlias));
    return;

}
/*=============================================================================
  函 数 名： ActRecStartPort
  功    能： 操作RecStartPort字段
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void 
=============================================================================*/
void CNmsActTest::ActRecStartPort()
{
    u32 dwSrcValue = 0;
    u32 dwDstValue = 60000;   
    u8  byRow = 1;
    u32 wNode = MAKE_MCUTABLE_NODENAME(byRow, NODE_MCUEQPRECORDERPORT);

    g_AgentCallBack(dwNode, 0, &dwSrcValue, sizeof(dwSrcValue));
    CPPUNIT_ASSERT(wSrcValue == 60000);
    return; 

}
/*=============================================================================
  函 数 名： ActRecIp
  功    能： 操作RecIp字段
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void 
=============================================================================*/
void CNmsActTest::ActRecIp()
{
    u32 dwSrcValue = 0;
    u32 dwDstValue = 0x02020202;   // 2.2.2.2
    u8  byRow = 1;
    u32 wNode = MAKE_MCUTABLE_NODENAME(byRow, NODE_MCUEQPRECORDERIPADDR);

    g_AgentCallBack(dwNode, 0, &dwSrcValue, sizeof(dwSrcValue));
    CPPUNIT_ASSERT(wSrcValue == 0x01010101);

    g_AgentCallBack(dwNode, 1, dwDstValue, sizeof( dwDstValue));
    OspDelay(1000);

    g_AgentCallBack(dwNode, 0, &dwSrcValue, sizeof(dwSrcValue));
    CPPUNIT_ASSERT( dwSrcValue == dwDstValue );
    return;
    
}

// tvWall
/*=============================================================================
  函 数 名： ActTvId
  功    能： 操作TvId字段
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void 
=============================================================================*/
void CNmsActTest::ActTvId()
{
    u32 dwSrcValue = 0;
    u8  byRow = 1;
    u32 wNode = MAKE_MCUTABLE_NODENAME(byRow, NODE_MCUEQPTVWALLID);

    g_AgentCallBack(dwNode, 0, &dwSrcValue, sizeof(dwSrcValue));
    CPPUNIT_ASSERT(wSrcValue == 33);

    return;  

}

void CNmsActTest::ActTvNum()
{
    
}

/*=============================================================================
  函 数 名： ActTvAlias
  功    能： 操作TvAlias字段
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void 
=============================================================================*/
void CNmsActTest::ActTvAlias()
{
    u8 achSrcAlias[32];
    u8 achDstAlias[32];
    u8 byRow = 1;
    memset(achSrcAlias, '\0', sizeof(achSrcAlias));
    memset(achDstAlias, '\0', sizeof(achDstAlias));
    
    u32 dwNode = MAKE_MCUTABLE_NODENAME(byRow, NODE_MCUEQPTVWALLALIAS);

    g_AgentCallBack(dwNode, 0, &achSrcAlias, sizeof(achSrcAlias));
    OspDelay(500);
    CPPUNIT_ASSERT(0 == strcmp(achSrcAlias, "TVWall1"));
    
    memcpy(achDstAlias, "Tv15", sizeof("Tv15"));
    g_AgentCallBack(dwNode, 1, achDstAlias, sizeof(achDstAlias));
    OspDelay(500);

    g_AgentCallBack(dwNode, 0, &achSrcAlias, sizeof(achSrcAlias));
    CPPUNIT_ASSERT(0 == strcmp(achDstAlias, achSrcAlias));
    return;

}

/*=============================================================================
  函 数 名： ActTvRunIndex
  功    能： 操作TvRunIndex字段
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void 
=============================================================================*/
void CNmsActTest::ActTvRunIndex()
{
    u32 dwSrcValue = 0;
    u32 dwDstValue = 5;   
    u8  byRow = 1;
    u32 wNode = MAKE_MCUTABLE_NODENAME(byRow, NODE_MCUEQPTVWALLRUNNINGBRDID);

    g_AgentCallBack(dwNode, 0, &dwSrcValue, sizeof(dwSrcValue));
    CPPUNIT_ASSERT(wSrcValue == 4);

    g_AgentCallBack(dwNode, 1, dwDstValue, sizeof( dwDstValue));
    OspDelay(1000);

    g_AgentCallBack(dwNode, 0, &dwSrcValue, sizeof(dwSrcValue));
    CPPUNIT_ASSERT( dwSrcValue == dwDstValue );
    return;  
 
}

/*=============================================================================
  函 数 名： ActTvStartPort
  功    能： 操作TvStartPort字段
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void 
=============================================================================*/
void CNmsActTest::ActTvStartPort()
{
    u32 dwSrcValue = 0;
    u32 dwDstValue = 45010;   
    u8  byRow = 1;
    u32 wNode = MAKE_MCUTABLE_NODENAME(byRow, NODE_MCUEQPTVWALLSTARTPORT);

    g_AgentCallBack(dwNode, 0, &dwSrcValue, sizeof(dwSrcValue));
    CPPUNIT_ASSERT(wSrcValue == 45000);

    g_AgentCallBack(dwNode, 1, dwDstValue, sizeof( dwDstValue));
    OspDelay(1000);

    g_AgentCallBack(dwNode, 0, &dwSrcValue, sizeof(dwSrcValue));
    CPPUNIT_ASSERT( dwSrcValue == dwDstValue );
    return;  
  
}

// bas
/*=============================================================================
  函 数 名： ActBasId
  功    能： 操作BasId字段
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void 
=============================================================================*/
void CNmsActTest::ActBasId()
{
    u32 dwSrcValue = 0;
    u8  byRow = 1;
    u32 wNode = MAKE_MCUTABLE_NODENAME(byRow, NODE_MCUEQPBASID);

    g_AgentCallBack(dwNode, 0, &dwSrcValue, sizeof(dwSrcValue));
    CPPUNIT_ASSERT(wSrcValue == 49);

    return;  

}

void CNmsActTest::ActBasNum()
{
    
}

/*=============================================================================
  函 数 名： ActBasToMcuPort
  功    能： 操作BasToMcuPort字段
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void 
=============================================================================*/
void CNmsActTest::ActBasToMcuPort()
{
    u32 dwSrcValue = 0;
    u32 dwDstValue = 13020;   
    u8  byRow = 1;
    u32 wNode = MAKE_MCUTABLE_NODENAME(byRow, NODE_MCUEQPBASPORT);

    g_AgentCallBack(dwNode, 0, &dwSrcValue, sizeof(dwSrcValue));
    CPPUNIT_ASSERT(wSrcValue == 13000);

    g_AgentCallBack(dwNode, 1, dwDstValue, sizeof( dwDstValue));
    OspDelay(1000);

    g_AgentCallBack(dwNode, 0, &dwSrcValue, sizeof(dwSrcValue));
    CPPUNIT_ASSERT( dwSrcValue == dwDstValue );
    return; 

}

/*=============================================================================
  函 数 名： ActBasSwitchBrdIdx
  功    能： 操作BasSwitchBrdIdx字段
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void 
=============================================================================*/
void CNmsActTest::ActBasSwitchBrdIdx()
{
    u32 dwSrcValue = 0;
    u32 dwDstValue = 4;   
    u8  byRow = 1;
    u32 wNode = MAKE_MCUTABLE_NODENAME(byRow, NODE_MCUEQPBASSWITCHBRDID);

    g_AgentCallBack(dwNode, 0, &dwSrcValue, sizeof(dwSrcValue));
    CPPUNIT_ASSERT(wSrcValue == 2);

    g_AgentCallBack(dwNode, 1, dwDstValue, sizeof( dwDstValue));
    OspDelay(1000);

    g_AgentCallBack(dwNode, 0, &dwSrcValue, sizeof(dwSrcValue));
    CPPUNIT_ASSERT( dwSrcValue == dwDstValue );
    return; 
    
}
/*=============================================================================
  函 数 名： ActBasAlias
  功    能： 操作BasAlias字段
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void 
=============================================================================*/
void CNmsActTest::ActBasAlias()
{
    u8 achSrcAlias[32];
    u8 achDstAlias[32];
    u8 byRow = 1;
    memset(achSrcAlias, '\0', sizeof(achSrcAlias));
    memset(achDstAlias, '\0', sizeof(achDstAlias));
    
    u32 dwNode = MAKE_MCUTABLE_NODENAME(byRow, NODE_MCUEQPTVWALLALIAS);

    g_AgentCallBack(dwNode, 0, &achSrcAlias, sizeof(achSrcAlias));
    OspDelay(500);
    CPPUNIT_ASSERT(0 == strcmp(achSrcAlias, "Bas1"));
    
    memcpy(achDstAlias, "Bas15", sizeof("Bas15"));
    g_AgentCallBack(dwNode, 1, achDstAlias, sizeof(achDstAlias));
    OspDelay(500);

    g_AgentCallBack(dwNode, 0, &achSrcAlias, sizeof(achSrcAlias));
    CPPUNIT_ASSERT(0 == strcmp(achDstAlias, achSrcAlias));
    return;
    
}
/*=============================================================================
  函 数 名： ActBasRunBrd
  功    能： 操作BasRunBrd字段
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void 
=============================================================================*/
void CNmsActTest::ActBasRunBrd()
{
    u32 dwSrcValue = 0;
    u32 dwDstValue = 5;   
    u8  byRow = 1;
    u32 wNode = MAKE_MCUTABLE_NODENAME(byRow, NODE_MCUEQPBASRUNNINGBRDID);

    g_AgentCallBack(dwNode, 0, &dwSrcValue, sizeof(dwSrcValue));
    CPPUNIT_ASSERT(wSrcValue == 6);

    g_AgentCallBack(dwNode, 1, dwDstValue, sizeof( dwDstValue));
    OspDelay(1000);

    g_AgentCallBack(dwNode, 0, &dwSrcValue, sizeof(dwSrcValue));
    CPPUNIT_ASSERT( dwSrcValue == dwDstValue );
    return;  
 
}
/*=============================================================================
  函 数 名： ActBasStartPort
  功    能： 操作BasStartPort字段
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void 
=============================================================================*/
void CNmsActTest::ActBasStartPort()
{
    u32 dwSrcValue = 0;
    u32 dwDstValue = 45010;   
    u8  byRow = 1;
    u32 wNode = MAKE_MCUTABLE_NODENAME(byRow, NODE_MCUEQPBASSTARTRECVPORT);

    g_AgentCallBack(dwNode, 0, &dwSrcValue, sizeof(dwSrcValue));
    CPPUNIT_ASSERT(wSrcValue == 45000);

    g_AgentCallBack(dwNode, 1, dwDstValue, sizeof( dwDstValue));
    OspDelay(1000);

    g_AgentCallBack(dwNode, 0, &dwSrcValue, sizeof(dwSrcValue));
    CPPUNIT_ASSERT( dwSrcValue == dwDstValue );
    return;  
  
}

// vmp
/*=============================================================================
  函 数 名： ActVmpId
  功    能： 操作VmpId字段
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void 
=============================================================================*/
void CNmsActTest::ActVmpId()
{
    u32 dwSrcValue = 0;
    u8  byRow = 1;
    u32 wNode = MAKE_MCUTABLE_NODENAME(byRow, NODE_MCUEQPVMPID);

    g_AgentCallBack(dwNode, 0, &dwSrcValue, sizeof(dwSrcValue));
    CPPUNIT_ASSERT(wSrcValue == 65);

    return;  
 
}
void CNmsActTest::ActVmpNum()
{
    
}
/*=============================================================================
  函 数 名： ActVmpToMcuPort
  功    能： 操作VmpToMcuPort字段
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void 
=============================================================================*/
void CNmsActTest::ActVmpToMcuPort()
{
    u32 dwSrcValue = 0;
    u32 dwDstValue = 17010;   
    u8  byRow = 1;
    u32 wNode = MAKE_MCUTABLE_NODENAME(byRow, NODE_MCUEQPVMPPORT);

    g_AgentCallBack(dwNode, 0, &dwSrcValue, sizeof(dwSrcValue));
    CPPUNIT_ASSERT(wSrcValue == 17000);

    g_AgentCallBack(dwNode, 1, dwDstValue, sizeof( dwDstValue));
    OspDelay(1000);

    g_AgentCallBack(dwNode, 0, &dwSrcValue, sizeof(dwSrcValue));
    CPPUNIT_ASSERT( dwSrcValue == dwDstValue );
    return; 

}

/*=============================================================================
  函 数 名： ActVmpSwitchBrdIdx
  功    能： 操作VmpSwitchBrdIdx字段
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void 
=============================================================================*/
void CNmsActTest::ActVmpSwitchBrdIdx()
{
        u32 dwSrcValue = 0;
    u32 dwDstValue = 3;   
    u8  byRow = 1;
    u32 wNode = MAKE_MCUTABLE_NODENAME(byRow, NODE_MCUEQPVMPSWITCHBRDID);

    g_AgentCallBack(dwNode, 0, &dwSrcValue, sizeof(dwSrcValue));
    CPPUNIT_ASSERT(wSrcValue == 2);

    g_AgentCallBack(dwNode, 1, dwDstValue, sizeof( dwDstValue));
    OspDelay(1000);

    g_AgentCallBack(dwNode, 0, &dwSrcValue, sizeof(dwSrcValue));
    CPPUNIT_ASSERT( dwSrcValue == dwDstValue );
    return; 
    
}

/*=============================================================================
  函 数 名： ActVmpAlias
  功    能： 操作VmpAlias字段
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void 
=============================================================================*/
void CNmsActTest::ActVmpAlias()
{
    u8 achSrcAlias[32];
    u8 achDstAlias[32];
    u8 byRow = 1;
    memset(achSrcAlias, '\0', sizeof(achSrcAlias));
    memset(achDstAlias, '\0', sizeof(achDstAlias));
    
    u32 dwNode = MAKE_MCUTABLE_NODENAME(byRow, NODE_MCUEQPVMPALIAS);

    g_AgentCallBack(dwNode, 0, &achSrcAlias, sizeof(achSrcAlias));
    OspDelay(500);
    CPPUNIT_ASSERT(0 == strcmp(achSrcAlias, "VMP1"));
    
    memcpy(achDstAlias, "Vmp15", sizeof("Vmp15"));
    g_AgentCallBack(dwNode, 1, achDstAlias, sizeof(achDstAlias));
    OspDelay(500);

    g_AgentCallBack(dwNode, 0, &achSrcAlias, sizeof(achSrcAlias));
    CPPUNIT_ASSERT(0 == strcmp(achDstAlias, achSrcAlias));
    return;

}

/*=============================================================================
  函 数 名： ActVmpRunBrd
  功    能： 操作VmpRunBrd字段
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void 
=============================================================================*/
void CNmsActTest::ActVmpRunBrd()
{
    u32 dwSrcValue = 0;
    u32 dwDstValue = 5;   
    u8  byRow = 1;
    u32 wNode = MAKE_MCUTABLE_NODENAME(byRow, NODE_MCUEQPVMPRUNNINGBRDID);

    g_AgentCallBack(dwNode, 0, &dwSrcValue, sizeof(dwSrcValue));
    CPPUNIT_ASSERT(wSrcValue == 2);

    g_AgentCallBack(dwNode, 1, dwDstValue, sizeof( dwDstValue));
    OspDelay(1000);

    g_AgentCallBack(dwNode, 0, &dwSrcValue, sizeof(dwSrcValue));
    CPPUNIT_ASSERT( dwSrcValue == dwDstValue );
    return;  
  
}
/*=============================================================================
  函 数 名： ActVmpStartPort
  功    能： 操作VmpStartPort字段
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void 
=============================================================================*/
void CNmsActTest::ActVmpStartPort()
{
    u32 dwSrcValue = 0;
    u32 dwDstValue = 43010;   
    u8  byRow = 1;
    u32 wNode = MAKE_MCUTABLE_NODENAME(byRow, NODE_MCUEQPVMPSTARTRECVPORT);

    g_AgentCallBack(dwNode, 0, &dwSrcValue, sizeof(dwSrcValue));
    CPPUNIT_ASSERT(wSrcValue == 43000);

    g_AgentCallBack(dwNode, 1, dwDstValue, sizeof( dwDstValue));
    OspDelay(1000);

    g_AgentCallBack(dwNode, 0, &dwSrcValue, sizeof(dwSrcValue));
    CPPUNIT_ASSERT( dwSrcValue == dwDstValue );
    return;  
   
}
/*=============================================================================
  函 数 名： ActVmpCodeNum
  功    能： 操作VmpCodeNum字段
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void 
=============================================================================*/
void CNmsActTest::ActVmpCodeNum()
{
    u32 dwSrcValue = 0;
    u32 dwDstValue = 2;   
    u8  byRow = 1;
    u32 wNode = MAKE_MCUTABLE_NODENAME(byRow, NODE_MCUEQPVMPSTARTPORT);

    g_AgentCallBack(dwNode, 0, &dwSrcValue, sizeof(dwSrcValue));
    CPPUNIT_ASSERT(wSrcValue == 1);

    g_AgentCallBack(dwNode, 1, dwDstValue, sizeof( dwDstValue));
    OspDelay(1000);

    g_AgentCallBack(dwNode, 0, &dwSrcValue, sizeof(dwSrcValue));
    CPPUNIT_ASSERT( dwSrcValue == dwDstValue );
    return;  
  
}
void CNmsActTest::ActVmpRunIp()
{
    
}

// data conference Server
/*=============================================================================
  函 数 名： ActDscId
  功    能： 操作DscId字段
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void 
=============================================================================*/
void CNmsActTest::ActDscId()
{
    u16  wSrcId = 0;
    u16  wDstId = 1;
    u32  dwNode = NODE_MCUEQPDCSID;

    g_AgentCallBack(dwNode, 0, &wSrcId, sizeof(wSrcId));
    OspDelay(500);
    CPPUNIT_ASSERT( wSrcId == 0);

    g_AgentCallBack(dwNode, 1, wDstId, sizeof(wDstId));

    g_AgentCallBack(dwNode, 0, &wSrcId, sizeof(wSrcId));
    CPPUNIT_ASSERT(wSrcId == wDstId);
    return;

}

// Prs
/*=============================================================================
  函 数 名： ActPrsId
  功    能： 操作PrsId字段
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void 
=============================================================================*/
void CNmsActTest::ActPrsId()
{
    u16  wSrcId = 0;
    u16  wDstId = 91;
    u32  dwNode = NODE_MCUEQPPRSID;

    g_AgentCallBack(dwNode, 0, &wSrcId, sizeof(wSrcId));
    OspDelay(500);
    CPPUNIT_ASSERT( wSrcId == 0);

    g_AgentCallBack(dwNode, 1, wDstId, sizeof(wDstId));

    g_AgentCallBack(dwNode, 0, &wSrcId, sizeof(wSrcId));
    CPPUNIT_ASSERT(wSrcId == wDstId);
    return;

}
/*=============================================================================
  函 数 名： ActPrsToMcuPort
  功    能： 操作PrsToMcuPort字段
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void 
=============================================================================*/
void CNmsActTest::ActPrsToMcuPort()
{
    u16  wSrcValue = 0;
    u16  wDstValue = 18010;
    u32  dwNode = NODE_MCUEQPPRSMCUSTARTPORT;

    g_AgentCallBack(dwNode, 0, &wSrcValue, sizeof(wSrcValue));
    OspDelay(500);
    CPPUNIT_ASSERT( wSrcId == 18000);

    g_AgentCallBack(dwNode, 1, wDstValue, sizeof(wDstValue));

    g_AgentCallBack(dwNode, 0, &wSrcValue, sizeof(wSrcValue));
    CPPUNIT_ASSERT(wSrcValue == wDstValue);
    return;

}
/*=============================================================================
  函 数 名： ActPrsSwitchBrdIdx
  功    能： 操作PrsSwitchBrdIdx字段
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void 
=============================================================================*/
void CNmsActTest::ActPrsSwitchBrdIdx()
{
    u16  wSrcValue = 0;
    u16  wDstValue = 3;
    u32  dwNode = NODE_MCUEQPPRSSWITCHBRDID;

    g_AgentCallBack(dwNode, 0, &wSrcValue, sizeof(wSrcValue));
    OspDelay(500);
    CPPUNIT_ASSERT( wSrcId == 2);

    g_AgentCallBack(dwNode, 1, wDstValue, sizeof(wDstValue));

    g_AgentCallBack(dwNode, 0, &wSrcValue, sizeof(wSrcValue));
    CPPUNIT_ASSERT(wSrcValue == wDstValue);
    return;

}
/*=============================================================================
  函 数 名： ActPrsAlias
  功    能： 操作PrsAlias字段
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void 
=============================================================================*/
void CNmsActTest::ActPrsAlias()
{
    u8 achSrcAlias[32];
    u8 achDstAlias[32];
    memset(achSrcAlias, '\0', sizeof(achSrcAlias));
    memset(achDstAlias, '\0', sizeof(achDstAlias));
    
    u32 dwNode = NODE_MCUEQPPRSALIAS;
    g_AgentCallBack(dwNode, 0, &achSrcAlias, sizeof(achSrcAlias));
    OspDelay(500);
    CPPUNIT_ASSERT(0 == strcmp(achSrcAlias, "Prs1"));
    
    memcpy(achDstAlias, "Prs15", sizeof("Prs15"));
    g_AgentCallBack(dwNode, 1, achDstAlias, sizeof(achDstAlias));

    g_AgentCallBack(dwNode, 0, &achSrcAlias, sizeof(achSrcAlias));
    CPPUNIT_ASSERT(0 == strcmp(achDstAlias, achSrcAlias));
    return;
    
}
/*=============================================================================
  函 数 名： ActPrsRunBrdIdx
  功    能： 操作PrsRunBrdIdx字段
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void 
=============================================================================*/
void CNmsActTest::ActPrsRunBrdIdx()
{
    u16  wSrcValue = 0;
    u16  wDstValue = 3;
    u32  dwNode = NODE_MCUEQPPRSRUNNINGBRDID;

    g_AgentCallBack(dwNode, 0, &wSrcValue, sizeof(wSrcValue));
    OspDelay(500);
    CPPUNIT_ASSERT( wSrcValue == 2);

    g_AgentCallBack(dwNode, 1, wDstValue, sizeof(wDstValue));

    g_AgentCallBack(dwNode, 0, &wSrcValue, sizeof(wSrcValue));
    CPPUNIT_ASSERT(wSrcValue == wDstValue);
    return;

}
/*=============================================================================
  函 数 名： ActPrsStartPort
  功    能： 操作PrsStartPort字段
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void 
=============================================================================*/
void CNmsActTest::ActPrsStartPort()
{
    u16  wSrcValue = 0;
    u16  wDstValue = 44010;
    u32  dwNode = NODE_MCUEQPPRSSTARTPORT;

    g_AgentCallBack(dwNode, 0, &wSrcValue, sizeof(wSrcValue));
    OspDelay(500);
    CPPUNIT_ASSERT( wSrcValue == 44000);

    g_AgentCallBack(dwNode, 1, wDstValue, sizeof(wDstValue));

    g_AgentCallBack(dwNode, 0, &wSrcValue, sizeof(wSrcValue));
    CPPUNIT_ASSERT(wSrcValue == wDstValue);
    return;

}
/*=============================================================================
  函 数 名： ActPrsFirstTime
  功    能： 操作PrsFirstTime字段
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void 
=============================================================================*/
void CNmsActTest::ActPrsFirstTime()
{
    u16  wSrcValue = 0;
    u16  wDstValue = 60;
    u32  dwNode = NODE_MCUEQPPRSFIRSTTIMESPAN;

    g_AgentCallBack(dwNode, 0, &wSrcValue, sizeof(wSrcValue));
    OspDelay(500);
    CPPUNIT_ASSERT( wSrcValue == 40);

    g_AgentCallBack(dwNode, 1, wDstValue, sizeof(wDstValue));

    g_AgentCallBack(dwNode, 0, &wSrcValue, sizeof(wSrcValue));
    CPPUNIT_ASSERT(wSrcValue == wDstValue);
    return;

}
/*=============================================================================
  函 数 名： ActPrsSecTime
  功    能： 操作PrsSecTime字段
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void 
=============================================================================*/
void CNmsActTest::ActPrsSecTime()
{
    u16  wSrcValue = 0;
    u16  wDstValue = 100;
    u32  dwNode = NODE_MCUEQPPRSSECONDTIMESPAN;

    g_AgentCallBack(dwNode, 0, &wSrcValue, sizeof(wSrcValue));
    OspDelay(500);
    CPPUNIT_ASSERT( wSrcValue == 80);

    g_AgentCallBack(dwNode, 1, wDstValue, sizeof(wDstValue));

    g_AgentCallBack(dwNode, 0, &wSrcValue, sizeof(wSrcValue));
    CPPUNIT_ASSERT(wSrcValue == wDstValue);
    return;
}
/*=============================================================================
  函 数 名： ActPrsThdTime
  功    能： 操作PrsThdTime字段
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void 
=============================================================================*/
void CNmsActTest::ActPrsThdTime()
{
    u16  wSrcValue = 0;
    u16  wDstValue = 140;
    u32  dwNode = NODE_MCUEQPPRSTHIRDTIMESPAN;

    g_AgentCallBack(dwNode, 0, &wSrcValue, sizeof(wSrcValue));
    OspDelay(500);
    CPPUNIT_ASSERT( wSrcValue == 120);

    g_AgentCallBack(dwNode, 1, wDstValue, sizeof(wDstValue));

    g_AgentCallBack(dwNode, 0, &wSrcValue, sizeof(wSrcValue));
    CPPUNIT_ASSERT(wSrcValue == wDstValue);
    return;

}
/*=============================================================================
  函 数 名： ActPrsRejectTime
  功    能： 操作PrsRejectTime字段
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void 
=============================================================================*/
void CNmsActTest::ActPrsRejectTime()
{
    u16  wSrcValue = 0;
    u16  wDstValue = 180;
    u32  dwNode = NODE_MCUEQPPRSREJECTTIMESPAN;

    g_AgentCallBack(dwNode, 0, &wSrcValue, sizeof(wSrcValue));
    OspDelay(500);
    CPPUNIT_ASSERT( wSrcValue == 160);

    g_AgentCallBack(dwNode, 1, wDstValue, sizeof(wDstValue));

    g_AgentCallBack(dwNode, 0, &wSrcValue, sizeof(wSrcValue));
    CPPUNIT_ASSERT(wSrcValue == wDstValue);
    return;

}

// net sync
/*=============================================================================
  函 数 名： ActNetSyncMode
  功    能： 操作NetSyncMode字段
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void 
=============================================================================*/
void CNmsActTest::ActNetSyncMode()
{
    u16  wSrcId = 0;
    u16  wDstId = 2;
    u32  dwNode = NODE_MCUEQPNETSYNCMODE;

    g_AgentCallBack(dwNode, 0, &wSrcId, sizeof(wSrcId));
    OspDelay(500);
    CPPUNIT_ASSERT( wSrcId == 1);

    g_AgentCallBack(dwNode, 1, wDstId, sizeof(wDstId));

    g_AgentCallBack(dwNode, 0, &wSrcId, sizeof(wSrcId));
    CPPUNIT_ASSERT(wSrcId == wDstId);
    return;
  
}

/*=============================================================================
  函 数 名： ActNetSyncDTSlot
  功    能： 操作NetSyncDTSlot字段
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void 
=============================================================================*/
void CNmsActTest::ActNetSyncDTSlot()
{
    u16  wSrcId = 0;
    u16  wDstId = 14;
    u32  dwNode = NODE_MCUEQPNETSYNCDTSLOT;

    g_AgentCallBack(dwNode, 0, &wSrcId, sizeof(wSrcId));
    OspDelay(500);
    CPPUNIT_ASSERT( wSrcId == 10);

    g_AgentCallBack(dwNode, 1, wDstId, sizeof(wDstId));

    g_AgentCallBack(dwNode, 0, &wSrcId, sizeof(wSrcId));
    CPPUNIT_ASSERT(wSrcId == wDstId);
    return;

}

/*=============================================================================
  函 数 名： ActNetSyncE1Idx
  功    能： 操作NetSyncE1Idx字段
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void 
=============================================================================*/
void CNmsActTest::ActNetSyncE1Idx()
{
    u16  wSrcId = 0;
    u16  wDstId = 7;
    u32  dwNode = NODE_MCUEQPNETSYNCE1INDEX;

    g_AgentCallBack(dwNode, 0, &wSrcId, sizeof(wSrcId));
    OspDelay(500);
    CPPUNIT_ASSERT( wSrcId == 0);

    g_AgentCallBack(dwNode, 1, wDstId, sizeof(wDstId));

    g_AgentCallBack(dwNode, 0, &wSrcId, sizeof(wSrcId));
    CPPUNIT_ASSERT(wSrcId == wDstId);
    return;
 
}

// Qos
/*=============================================================================
  函 数 名： ActQosType
  功    能： 操作QosType字段
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void 
=============================================================================*/
void CNmsActTest::ActQosType()
{
    u16  wSrcValue = 0;
    u16  wDstValue = 1;
    u32  dwNode = NODE_MCUNETQOSTYPE;

    g_AgentCallBack(dwNode, 0, &wSrcValue, sizeof(wSrcValue));
    OspDelay(500);
    CPPUNIT_ASSERT( wSrcValue == 2);

    g_AgentCallBack(dwNode, 1, wDstValue, sizeof(wDstValue));

    g_AgentCallBack(dwNode, 0, &wSrcValue, sizeof(wSrcValue));
    CPPUNIT_ASSERT(wSrcValue == wDstValue);
    return;
}
/*=============================================================================
  函 数 名： ActAudioLevel
  功    能： 操作AudioLevel字段
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void 
=============================================================================*/
void CNmsActTest::ActAudioLevel()
{
    u16  wSrcValue = 0;
    u16  wDstValue = 3;
    u32  dwNode = NODE_MCUNETQOSAUDIOLEVEL;

    g_AgentCallBack(dwNode, 0, &wSrcValue, sizeof(wSrcValue));
    OspDelay(500);
    CPPUNIT_ASSERT( wSrcValue == 4);

    g_AgentCallBack(dwNode, 1, wDstValue, sizeof(wDstValue));

    g_AgentCallBack(dwNode, 0, &wSrcValue, sizeof(wSrcValue));
    CPPUNIT_ASSERT(wSrcValue == wDstValue);
    return;

}

/*=============================================================================
  函 数 名： ActVideoLevel
  功    能： 操作VideoLevel字段
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void 
=============================================================================*/
void CNmsActTest::ActVideoLevel()
{
    u16  wSrcValue = 0;
    u16  wDstValue = 4;
    u32  dwNode = NODE_MCUNETQOSVIDEOLEVEL;

    g_AgentCallBack(dwNode, 0, &wSrcValue, sizeof(wSrcValue));
    OspDelay(500);
    CPPUNIT_ASSERT( wSrcValue == 3);

    g_AgentCallBack(dwNode, 1, wDstValue, sizeof(wDstValue));

    g_AgentCallBack(dwNode, 0, &wSrcValue, sizeof(wSrcValue));
    CPPUNIT_ASSERT(wSrcValue == wDstValue);
    return;
}

/*=============================================================================
  函 数 名： ActDataLevel
  功    能： 操作DataLevel字段
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void 
=============================================================================*/
void CNmsActTest::ActDataLevel()
{
    u16  wSrcValue = 0;
    u16  wDstValue = 1;
    u32  dwNode = NODE_MCUNETQOSDATALEVEL;

    g_AgentCallBack(dwNode, 0, &wSrcValue, sizeof(wSrcValue));
    OspDelay(500);
    CPPUNIT_ASSERT( wSrcValue == 0);

    g_AgentCallBack(dwNode, 1, wDstValue, sizeof(wDstValue));

    g_AgentCallBack(dwNode, 0, &wSrcValue, sizeof(wSrcValue));
    CPPUNIT_ASSERT(wSrcValue == wDstValue);
    return;

}

/*=============================================================================
  函 数 名： ActSignalLevel
  功    能： 操作SignalLevel字段
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void 
=============================================================================*/
void CNmsActTest::ActSignalLevel()
{
    u16  wSrcValue = 0;
    u16  wDstValue = 6;
    u32  dwNode = NODE_MCUNETQOSSIGNALLEVEL;

    g_AgentCallBack(dwNode, 0, &wSrcValue, sizeof(wSrcValue));
    OspDelay(500);
    CPPUNIT_ASSERT( wSrcValue == 7);

    g_AgentCallBack(dwNode, 1, wDstValue, sizeof(wDstValue));

    g_AgentCallBack(dwNode, 0, &wSrcValue, sizeof(wSrcValue));
    CPPUNIT_ASSERT(wSrcValue == wDstValue);
    return; 

}

/*=============================================================================
  函 数 名： ActIpServiceType
  功    能： 操作IpServiceType字段
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void 
=============================================================================*/
void CNmsActTest::ActIpServiceType()
{
    u16  wSrcValue = 0;
    u16  wDstValue = 0;
    u32  dwNode = NODE_MCUNETQOSIPSERVICETYPE;

    g_AgentCallBack(dwNode, 0, &wSrcValue, sizeof(wSrcValue));
    OspDelay(500);
    CPPUNIT_ASSERT( wSrcValue == 1);

    g_AgentCallBack(dwNode, 1, wDstValue, sizeof(wDstValue));

    g_AgentCallBack(dwNode, 0, &wSrcValue, sizeof(wSrcValue));
    CPPUNIT_ASSERT(wSrcValue == wDstValue);
    return;
}