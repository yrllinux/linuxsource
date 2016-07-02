/*****************************************************************************
   模块名      : mcu
   文件名      : evmcuvcs.h
   相关文件    : 
   文件实现功能: MCU与VCS接口消息定义
   作者        : 付秀华
   版本        : 
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   2008/11/20              付秀华      创建
******************************************************************************/
#ifndef _EVMCUVCS_H_
#define _EVMCUVCS_H_

#include "osp.h"
#include "eventid.h"

/*------------------------------------------------------------------------------
 MCU与VCS接口消息
------------------------------------------------------------------------------*/

//1 用户管理采取MCS与MCU交互的消息, 但消息体为CVCSUsrInfo
//2 用户组管理采取MCS与MCU交互的消息及消息体


//3 会议模板管理采取MCS与MCU交互的消息，但消息体如下
//VCS创建模板，     //消息体: TConfInfo + 2 byte(网络序,TMtAlias数组打包长度,即下面的n)
//                  + n 个(TMtAlias数组打包,依次为 1byte(别名类型)+1byte(别名长度)+xbyte(别名字符串)+2byte(呼叫码率)) 
//                  + TMultiTvWallModule(可选字段，由会议属性中的是否有电视墙模板决定) 
//                  + TVmpModule(可选字段，由会议属性中的是否有画面合成墙模板决定)
//                  + 1byte(u8: 0 1  是否配置了高清电视墙)
//                  + (可选, THDTvWall)
//                  + 1byte(u8: 会议配置HDU的通道总数)
//                  + (可选, THduModChnlInfo+...)
//                  + 1byte(是否为级联调度)＋(可选，2byte[u16 网络序，配置打包的总长度]+下级mcu[1byte(别名类型)+1byte(别名长度)+xbyte(别名字符串)+2byte(呼叫码率)...)])
//                  + 1byte(是否支持分组)＋(可选，2byte(u16 网络序，信息总长)+内容(1byte(组数)+n组[1TVCSGroupInfo＋m个TVCSEntryInfo])
//                  + 1个 TMtAlias （备份本地终端，用来实现本地终端1+1备份）

//4 创建会议
   //VCS在MCU上按会议模板创建一个即时会议, 无消息体                
    OSPEVENT( VCS_MCU_CREATECONF_BYTEMPLATE_REQ,		EV_VCSVC_BGN + 1 );
    //会议模板成功地转变成即时会议, 无消息体 
    OSPEVENT( MCU_VCS_CREATECONF_BYTEMPLATE_ACK,		EV_VCSVC_BGN + 2 );
    //会议模板转变成即时会议失败, 无消息体
    OSPEVENT( MCU_VCS_CREATECONF_BYTEMPLATE_NACK,       EV_VCSVC_BGN + 3 );

	//VCS为指定的会议模板配置分组信息,      消息体：分组内容
	OSPEVENT( VCS_MCU_SENDPACKINFO_REQ,                     EV_VCSVC_BGN + 4 );
	//允许VCS为指定的会议模板配置分组信息,   消息体：无
	OSPEVENT( MCU_VCS_SENDPCAKINFO_ACK,                     EV_VCSVC_BGN + 5 );
	//不允许VCS为指定的会议模板配置分组信息, 消息体：无(带错误码)
	OSPEVENT( MCU_VCS_SENDPCAKINFO_NACK,                    EV_VCSVC_BGN + 6 );
	//MCU给VCS的会议模板分组信息通告,        消息体：分组内容
	OSPEVENT( MCU_VCS_PACKINFO_NOTIFY,                      EV_VCSVC_BGN + 7 );

// 5 调度操作，会议中消息
	// VCS请求对某调度席开始操作       消息体：无(根据消息头中CConfId)
	OSPEVENT( VCS_MCU_OPRCONF_REQ,              EV_VCSVC_BGN + 10 );
	// VCS请求对某调度席开始操作的应答 消息体：无
	OSPEVENT( MCU_VCS_OPRCONF_ACK,              EV_VCSVC_BGN + 11 );
	// VCS请求对某调度席开始操作的应答 消息体：无
	OSPEVENT( MCU_VCS_OPRCONF_NACK,             EV_VCSVC_BGN + 12 );

	// VCS请求退出调度席               消息体：无(根据消息头中CConfId)
	OSPEVENT( VCS_MCU_QUITCONF_REQ,             EV_VCSVC_BGN + 13 );
	// VCS请求退出调度席的应答         消息体：无
	OSPEVENT( MCU_VCS_QUITCONF_ACK,             EV_VCSVC_BGN + 14 );
	// VCS请求退出调度席的应答         消息体：无
	OSPEVENT( MCU_VCS_QUITCONF_NACK,            EV_VCSVC_BGN + 15 );

	//VCS请求会议的调度模式,    消息体：无
	OSPEVENT( VCS_MCU_VCMODE_REQ,               EV_VCSVC_BGN + 20 );
	//VCS请求会议的调度模式应答,消息体：u8(VCS_SINGLE_MODE，VCS_MULTW_MODE，VCS_MULVMP_MODE)
	OSPEVENT( MCU_VCS_VCMODE_ACK,               EV_VCSVC_BGN + 21 );
	//VCS请求会议的调度模式应答,消息体：无
	OSPEVENT( MCU_VCS_VCMODE_NACK,              EV_VCSVC_BGN + 22 );

	//VCS更改会议的调度模式，   消息体：u8(VCS_SINGLE_MODE，VCS_MULTW_MODE，VCS_MULVMP_MODE)
    OSPEVENT( VCS_MCU_CHGVCMODE_REQ,            EV_VCSVC_BGN + 23 );
	//VCS更改会议的调度模式应答
	OSPEVENT( MCU_VCS_CHGVCMODE_ACK,            EV_VCSVC_BGN + 24 );
	//VCS更改会议的调度模式应答,消息体：无
	OSPEVENT( MCU_VCS_CHGVCMODE_NACK,           EV_VCSVC_BGN + 25 );
	//VCS更改会议的调度模式通告,消息体：u8
	OSPEVENT( MCU_VCS_CHGVCMODE_NOTIF,          EV_VCSVC_BGN + 26 );
	
	//VCS调度指定的终端,         消息体：u8(调度终端描述类型：VCS_DTYPE_MTID, VCS_DTYPE_MTALIAS) +
	//                                   TMt(调度终端的信息)或者u8(别名类型)+u8(别名长度)+xbyte(别名字符串)+u16(呼叫码率) +
	//                                   u8(操作类型:VCS_VCTYPE_DEFAULT, VCS_VCTYPE_DROPMT)
	//                                   u8(强拆类型:VCS_FORCECALL_REQ, VCS_FORCECALL_CMD)
	OSPEVENT( VCS_MCU_VCMT_REQ,                 EV_VCSVC_BGN + 27 );
	//VCS调度指定的终端的应答,   消息体：TMt(调度终端的信息) + u8(操作类型)
	//                                   u8(强拆类型:VCS_FORCECALL_REQ, VCS_FORCECALL_CMD)
	OSPEVENT( MCU_VCS_VCMT_ACK,                 EV_VCSVC_BGN + 28 );
	//VCS调度指定的终端应答,     消息体：仅ERR_MCU_VCS_VCMTING含消息体TMt(上一个调度终端的信息)
	OSPEVENT( MCU_VCS_VCMT_NACK,                EV_VCSVC_BGN + 29 );
	//VCS调度结果通告，          消息体：仅ERR_MCU_VCS_VCMTOVERTIME含消息体TMt(上一个调度终端的信息)(含错误码)
	OSPEVENT( MCU_VCS_VCMT_NOTIF,               EV_VCSVC_BGN + 30 );

	//VCS会议状态通告            消息体：CBasicVCCStatus
	OSPEVENT( MCU_VCS_CONFSTATUS_NOTIF,         EV_VCSVC_BGN + 31 );

	//VCS会议请求静音/哑音       消息体：u8(请求时的调度模式)+u8(开启(1)/关闭(0))
	//                                  +u8(VCS_OPR_LOCAL/VCS_OPR_REMOTE)+u8(VCS_AUDPROC_MUTE/VCS_AUDPROC_SILENCE)
	OSPEVENT( VCS_MCU_MUTE_REQ,                 EV_VCSVC_BGN + 40 );
	//VCS会议请求静音/哑音应答   消息体： 无
	OSPEVENT( MCU_VCS_MUTE_ACK,                 EV_VCSVC_BGN + 41 );
	//VCS会议请求静音/哑音应答   消息体： 无(含错误码，如模式已切换，操作取消)
	OSPEVENT( MCU_VCS_MUTE_NACK,                EV_VCSVC_BGN + 42 );

// 6 
	//软件名称自定义 
	OSPEVENT( VCS_MCU_GETSOFTNAME_REQ  ,        EV_VCSVC_BGN + 60 );
	OSPEVENT( MCU_VCS_GETSOFTNAME_ACK  ,        EV_VCSVC_BGN + 61 );
	OSPEVENT( MCU_VCS_GETSOFTNAME_NACK,         EV_VCSVC_BGN + 62 );

	OSPEVENT( VCS_MCU_CHGSOFTNAME_REQ  ,        EV_VCSVC_BGN + 63 );
	OSPEVENT( MCU_VCS_CHGSOFTNAME_ACK  ,        EV_VCSVC_BGN + 64 );
    OSPEVENT( MCU_VCS_CHGSOFTNAME_NACK ,        EV_VCSVC_BGN + 65 );

	OSPEVENT( VCS_MCU_SOFTNAME_NOTIF   ,        EV_VCSVC_BGN + 66 );


// 组呼操作, 会议中
	//VCS请求MCU批量增加并组呼终端,   消息体：  u8(组数)+u8(组名长度)+s8字符串(组名)+u8(终端数)+TMt数组+
	//                                          ...
	OSPEVENT( VCS_MCU_GROUPCALLMT_REQ,            EV_VCSVC_BGN + 70 );
	//MCU同意批量增加并组呼终端,      消息体：  无
	OSPEVENT( MCU_VCS_GROUPCALLMT_ACK,            EV_VCSVC_BGN + 71 );
	//MCU拒绝批量增加并组呼终端,      消息体：  无
	OSPEVENT( MCU_VCS_GROUPCALLMT_NACK,           EV_VCSVC_BGN + 72 );
	//通告                            消息体:   组名(含失败错误码)
	OSPEVENT( MCU_VCS_GROUPCALLMT_NOTIF ,           EV_VCSVC_BGN + 73 );

	//VCS请求MCU批量挂断组呼终端,     消息体： u8(组数)+u8(组名长度)+s8字符串(组名)+u8(终端数)+TMt数组+
	//                                          ...
	OSPEVENT( VCS_MCU_GROUPDROPMT_REQ,            EV_VCSVC_BGN + 75 );
	//MCU同意批量挂断组呼终端,        消息体：  无
	OSPEVENT( MCU_VCS_GROUPDROPMT_ACK,            EV_VCSVC_BGN + 76 );
	//MCU拒绝批量挂断呼终端,          消息体：  无
	OSPEVENT( MCU_VCS_GROUPDROPMT_NACK,           EV_VCSVC_BGN + 77 );

	//VCS请求MCU临时增加终端,    消息体：  TAddMtInfo
	OSPEVENT( VCS_MCU_ADDMT_REQ,                EV_VCSVC_BGN + 80 );
	//MCU同意增加终端,           消息体：  无
	OSPEVENT( MCU_VCS_ADDMT_ACK,                EV_VCSVC_BGN + 81 );
	//MCU拒绝增加终端,           消息体：  无
	OSPEVENT( MCU_VCS_ADDMT_NACK,               EV_VCSVC_BGN + 82 );

	//VCS请求MCU删除终端,        消息体：  TMt
	//VCS需要保证请求删除的终端非调度模式中的可调度资源
	OSPEVENT( VCS_MCU_DELMT_REQ,                EV_VCSVC_BGN + 85 );
	//MCU同意增加终端,           消息体：  无
	OSPEVENT( MCU_VCS_DELMT_ACK,                EV_VCSVC_BGN + 86 );
	//MCU拒绝增加终端,           消息体：  无
	OSPEVENT( MCU_VCS_DELMT_NACK,               EV_VCSVC_BGN + 87 );

	//VCS开启主席轮询,               消息体：  u8(轮询间隔)
	OSPEVENT( VCS_MCU_STARTCHAIRMANPOLL_REQ,     EV_VCSVC_BGN + 90 );
	//MCU同意开启主席轮询,           消息体：  无
	OSPEVENT( MCU_VCS_STARTCHAIRMANPOLL_ACK,      EV_VCSVC_BGN + 91 );
	//MCU拒绝开启主席轮询,           消息体：  无(含错误码)
	OSPEVENT( MCU_VCS_STARTCHAIRMANPOLL_NACK,     EV_VCSVC_BGN + 92 );

	//VCS停止主席轮询,               消息体：  无
	OSPEVENT( VCS_MCU_STOPCHAIRMANPOLL_REQ,     EV_VCSVC_BGN + 95 );
	//MCU同意停止主席轮询,           消息体：  无
	OSPEVENT( MCU_VCS_STOPCHAIRMANPOLL_ACK,      EV_VCSVC_BGN + 96 );
	//MCU拒绝停止主席轮询,           消息体：  无
	OSPEVENT( MCU_VCS_STOPCHAIRMANPOLL_NACK,     EV_VCSVC_BGN + 97 );
	
// 其它
    // 同意强拆请求                   消息体:   TMt(请求释放的终端) + s8[](请求抢占的会议名称)
	OSPEVENT( MCU_VCS_RELEASEMT_REQ,             EV_VCSVC_BGN + 100 );
    // 同意强拆                       消息体:   TMt(请求释放的终端) + s8[](请求抢占的会议名称)
	OSPEVENT( VCS_MCU_RELEASEMT_ACK,             EV_VCSVC_BGN + 101 );
	// 拒绝强拆                       消息体:　 TMt(请求释放的终端) + s8[](请求抢占的会议名称)　
	OSPEVENT( VCS_MCU_RELEASEMT_NACK,            EV_VCSVC_BGN + 102 );
	// 强拆执行通告                   消息体:   TMt(请求释放的终端) + s8[](请求抢占的会议名称)
	OSPEVENT( MCU_VCS_RELEASEMT_NOTIF,           EV_VCSVC_BGN + 103 );
 
// 7 为界面保存mcu不处理信息
    //VCS请求获取不处理信息，        消息体：  无
    OSPEVENT( VCS_MCU_GETUNPROCFGINFO_REQ,       EV_VCSVC_BGN + 150 );
    //请求获取不处理信息成功,        消息体：  s8[] 实际内容
    OSPEVENT( MCU_VCS_GETUNPROCFGINFO_ACK,       EV_VCSVC_BGN + 151 );
    //请求获取不处理信息失败,        消息体：  无
    OSPEVENT( MCU_VCS_GETUNPROCFGINFO_NACK ,     EV_VCSVC_BGN + 152 );

    //VCS请求mcu保存不处理信息，     消息体：  s8[] 实际内容
    OSPEVENT( VCS_MCU_SETUNPROCFGINFO_REQ,       EV_VCSVC_BGN + 153 );
    //请求保存mcu不处理信息成功,     消息体：  无
    OSPEVENT( MCU_VCS_SETUNPROCFGINFO_ACK,       EV_VCSVC_BGN + 154 );
    //请求保存mcu不处理信息失败,     消息体：  无  
    OSPEVENT( MCU_VCS_SETUNPROCFGINFO_NACK,      EV_VCSVC_BGN + 155 );
    //mcu不处理信息通告,             消息体：  s8[] 实际内容
    OSPEVENT( MCU_VCS_SETUNPROCFGINFO_NOTIF,     EV_VCSVC_BGN + 156 );

	//福建公安新需求增加的消息
	//zjj20091102改为只限于手动模式
	//VCS请求终端进入电视墙哪个通道(只限于手动模式和预览模式)	  消息体：TSwitchInfo个数(u8)+
	//    u8(调度终端描述类型：VCS_DTYPE_MTID, VCS_DTYPE_MTALIAS 若VCS_DTYPE_MTALIAS+u8(别名类型)+u8(别名长度)+xbyte(别名字符串)+u16(呼叫码率))
	//    +TSwitchInfo
	//    +...
	OSPEVENT( VCS_MCU_SETMTINTVWALL_REQ,		EV_VCSVC_BGN  + 157	);
	//MCU请求终端进入电视墙哪个通道成功(只限于手动模式和预览模式) 消息体: 无
	OSPEVENT( MCU_VCS_SETMTINTVWALL_ACK,		EV_VCSVC_BGN  + 158 );
	//MCU请求终端进入电视墙哪个通道失败(只限于手动模式和预览模式) 消息体: 无
	OSPEVENT( MCU_VCS_SETMTINTVWALL_NACK,		EV_VCSVC_BGN  + 159 );

	//zjj20091102
	//VCS添加和保存预案					消息体:		u8[]	预案名称长度(u8)+预案名+ + TMtVCSPlanAlias数组
	//如果 终端个数num(u8)为0,就为添加预案，如果大于0，就表示是保存预案终端别名数据
	OSPEVENT( VCS_MCU_ADDPLANNAME_REQ,			EV_VCSVC_BGN  + 160 );
	//MCU响应添加预案成功			消息体:无
	OSPEVENT( MCU_VCS_ADDPLANNAME_ACK,			EV_VCSVC_BGN  + 161 );
	//MCU响应添加预案失败			消息体:无
	OSPEVENT( MCU_VCS_ADDPLANNAME_NACK,			EV_VCSVC_BGN  + 162 );

	//VCS删除预案					消息体:		u8[]		预案名字(带\0)
	OSPEVENT( VCS_MCU_DELPLANNAME_REQ,			EV_VCSVC_BGN  + 163 );
	//MCU响应删除预案成功			消息体:无
	OSPEVENT( MCU_VCS_DELPLANNAME_ACK,			EV_VCSVC_BGN  + 164 );
	//MCU响应删除预案失败			消息体:无
	OSPEVENT( MCU_VCS_DELPLANNAME_NACK,			EV_VCSVC_BGN  + 165 );

	//VCS修改预案名称				消息体:		u8[]要修改的预案名长度(u8)+要修改的预案名+新预案名长度(u8)+新预案名
	OSPEVENT( VCS_MCU_MODIFYPLANNAME_REQ,		EV_VCSVC_BGN  + 166 );
	//MCU响应修改预案名称成功			消息体:无
	OSPEVENT( MCU_VCS_MODIFYPLANNAME_ACK,		EV_VCSVC_BGN  + 167 );
	//MCU响应修改预案名称失败			消息体:无
	OSPEVENT( MCU_VCS_MODIFYPLANNAME_NACK,		EV_VCSVC_BGN  + 168 );

	//MCU通知vcs所有的会议预案名字		消息体:TMtVcsPlanNames
	OSPEVENT( MCU_VCS_ALLPLANNAME_NOTIFY,		EV_VCSVC_BGN  + 169 );

	/*//VCS请求保存某个预案(添加或删除预案中的终端别名) 消息体: 预案名称长度(u8)+预案名+终端个数num(u8) + TMtVCSPlanAlias数组
	OSPEVENT( VCS_MCU_SAVEPLANDATA_REQ,			EV_VCSVC_BGN  + 170 );
	//MCU响应保存某个预案成功(添加或删除预案中的终端别名) 消息体:无
	OSPEVENT( MCU_VCS_SAVEPLANDATA_ACK,			EV_VCSVC_BGN  + 171 );
	//MCU响应保存某个预案失败(添加或删除预案中的终端别名) 消息体:无
	OSPEVENT( MCU_VCS_SAVEPLANDATA_NACK,		EV_VCSVC_BGN  + 172 );
	*/

	//MCU通知vcs某个预案的名称和别名数据		消息体:预案名称长度(u8)+预案名+终端个数num(u8) + TMtVcsPlanNames数组
	OSPEVENT( MCU_VCS_ONEPLANDATA_NOTIFY,		EV_VCSVC_BGN  + 170 );

	//VCS请求获得所有预案数据信息				消息体:ConfId结构
	OSPEVENT( VCS_MCU_GETALLPLANDATA_REQ,		EV_VCSVC_BGN  + 171 );
	//mcu响应vcs请求获得所有预案数据成功			消息体:无
	OSPEVENT( MCU_VCS_GETALLPLANDATA_ACK,		EV_VCSVC_BGN  + 172 );
	//mcu响应vcs请求获得所有预案数据失败			消息体:无
	OSPEVENT( MCU_VCS_GETALLPLANDATA_NACK,		EV_VCSVC_BGN  + 173 );

	//MCU通知vcs现有的所有的数据(因为所有数据一起发数据量大，所以实际发的时候也是一次发一条预案数据)
	//因为这条数据是响应VCS_MCU_GETALLPLANDATA_REQ消息的，所以虽然消息体和MCU_VCS_ONEPLANDATA_NOTIFY消息的消息体一样，但还是重新定义一个
	//消息体: 预案名称长度(u8)+预案名+终端别名个数num(u8) + TMtVCSPlanAlias数组
	OSPEVENT( MCU_VCS_ALLPLANDATA_SINGLE_NOTIFY,EV_VCSVC_BGN  + 174 );


	//vcs请求mcu改变电视墙模式		消息体:模式值(u8)VCS_TVWALLMANAGE_MANUAL_MODE等
	OSPEVENT( VCS_MCU_CHGTVWALLMODE_REQ,		EV_VCSVC_BGN  + 175 );
	//mcu响应vcs改变电视墙模式成功	消息体：无
	OSPEVENT( MCU_VCS_CHGTVWALLMODE_ACK,		EV_VCSVC_BGN  + 176 );
	//mcu响应vcs改变电视墙模式失败	消息体：无
	OSPEVENT( MCU_VCS_CHGTVWALLMODE_NACK,		EV_VCSVC_BGN  + 177 );

	//MCU通知vcs现在的电视墙模式	消息体：模式值(u8)VCS_TVWALLMANAGE_MANUAL_MODE等
	//OSPEVENT( MCU_VCS_TVWALLMODE_NOTIFY,		EV_VCSVC_BGN  + 181 );

	/*//VCS通知mcu终端进入电视墙通道(只限于在预览模式下),消息体：TSwitchInfo个数(u8)+TSwitchInfo+TSwitchInfo+...
	OSPEVENT( VCS_MCU_REVIEWMODEMTINTVWALL_REQ,	EV_VCSVC_BGN  + 182 );
	//mcu响应vcs终端进入电视墙通道成功(只限于在预览模式下),消息体：无
	OSPEVENT( MCU_VCS_REVIEWMODEMTINTVWALL_ACK,	EV_VCSVC_BGN  + 183 );
	//mcu响应vcs终端进入电视墙通道失败(只限于在预览模式下),消息体：无
	OSPEVENT( MCU_VCS_REVIEWMODEMTINTVWALL_NACK,EV_VCSVC_BGN  + 184 );
	*/

	//VCS通知mcu清除某个电视墙通道信息(只限于手动模式),消息体:eqpid(u8)+channelindex(u8)
	OSPEVENT( VCS_MCU_CLEARONETVWALLCHANNEL_CMD,EV_VCSVC_BGN  + 178  );
	//VCS通知mcu清除全部电视墙通道信息(只限于手动模式),消息体：无
	OSPEVENT( VCS_MCU_CLEARALLTVWALLCHANNEL_CMD,EV_VCSVC_BGN  + 179  );

	//vcs通知mcu设置电视墙通道终端混音状态 消息体:是否混音(u8)0-取消混音,1-开始混音
	OSPEVENT( VCS_MCU_TVWALLSTARMIXING_CMD,		EV_VCSVC_BGN  + 180  );
		
	//mcu通知vcs已经修改成功的预案名字 消息体:		u8[]要修改的预案名长度(u8)+要修改的预案名+新预案名长度(u8)+新预案名
	OSPEVENT( MCU_VCS_MODIFYPLANNAME_NOTIFY,	EV_VCSVC_BGN  + 181 );

	//多回传用，实际vcs不会发这个命令，只在多回传响应级联回传命令后标识选看动作用 消息体:无
	OSPEVENT( VCS_MCU_CONFSELMT_CMD,			EV_VCSVC_BGN  + 182 );
	/*//vcs请求锁定或解锁某个预案						消息体:预案名长度(u8)+预案名+加锁或解锁(u8)(0-解锁 1-加锁)
	OSPEVENT( VCS_MCU_LOCKPLAN_REQ,				EV_VCSVC_BGN  + 182 );
	//vcs请求锁定或解锁某个预案成功					消息体:预案名长度(u8)+预案名+加锁或解锁(u8)(0-解锁 1-加锁)
	OSPEVENT( MCU_VCS_LOCKPLAN_ACK,				EV_VCSVC_BGN  + 183 );
	//vcs请求锁定或解锁某个预案失败					消息体:预案名长度(u8)+预案名+加锁或解锁(u8)(0-解锁 1-加锁)
	OSPEVENT( MCU_VCS_LOCKPLAN_NACK,			EV_VCSVC_BGN  + 184 );

	//vcs解锁所有由这个vcs锁定的预案(在vcs断链和vcs退出调度席时由mcu自己发出给会议)//消息体:无	
	OSPEVENT( VCS_MCU_VCSUNLOCKPLAN_CMD,		EV_VCSVC_BGN  + 185 );
	*/

    //[11/18/2011 zhushengze]通知VCS下级终端开始发双流，驱动VCS建交换请求 消息体：TMt:双流源
    OSPEVENT( MCU_VCS_STARTDOUBLESTREAM_NOTIFY,   EV_VCSVC_BGN  + 184  );
    //VCS切换会议的双流状态: 消息体 u8：CONF_DUALSTATUS_DISABLE/CONF_DUALSTATUS_ENABLE
    OSPEVENT( VCS_MCU_CHGDUALSTATUS_REQ,          EV_VCSVC_BGN + 185 );
    OSPEVENT( MCU_VCS_CHGDUALSTATUS_ACK,          EV_VCSVC_BGN + 186 );
    OSPEVENT( MCU_VCS_CHGDUALSTATUS_NACK,         EV_VCSVC_BGN + 187 );

	// 监控联动 开始监控  消息体：TSwitchDstInfo 
    OSPEVENT( VCS_MCU_START_MONITOR_UNION_REQ,          EV_VCSVC_BGN + 188 );
    OSPEVENT( MCU_VCS_START_MONITOR_UNION_ACK,          EV_VCSVC_BGN + 189 );
    OSPEVENT( MCU_VCS_START_MONITOR_UNION_NACK,         EV_VCSVC_BGN + 190 );
	
	// MCU告知界面可以开始监控。消息体：
	// TSwitchDstInfo + TMediaEncrypt(视频加密信息) + TDoublePayload(视频载荷) +
	// TMediaEncrypt(音频加密信息) +  TDoublePayload(音频载荷) + TAudAACCap(针对音频是AACLC,AACLD会追加)
	// + TTransportAddr(video rtcp addr) + TTransportAddr(audio rtcp addr) 
	OSPEVENT( MCU_VCS_START_MONITOR_NOTIF,				EV_VCSVC_BGN + 191 );
	
	// 监控联动 停止监控  消息体：TSwitchDstInfo
    OSPEVENT( VCS_MCU_STOP_MONITOR_UNION_REQ,          EV_VCSVC_BGN + 192 );
    OSPEVENT( MCU_VCS_STOP_MONITOR_UNION_ACK,          EV_VCSVC_BGN + 193 );
    OSPEVENT( MCU_VCS_STOP_MONITOR_UNION_NACK,         EV_VCSVC_BGN + 194 );

		

	//终端主动加入会议  消息体：byEncrypt(1-加密,0-不加密)+bySrcDriId(u8)+TMtAlias(tFstAlias)+TMtAlias(tMtH323Alias)+TMtAlias(tMtE164Alias)+TMtAlias(tMtAddr)+U16(wCallRate)+u8(bySrcSsnId)
    OSPEVENT( MCU_VCS_MTJOINCONF_REQ,          EV_VCSVC_BGN + 195 );
    OSPEVENT( VCS_MCU_MTJOINCONF_ACK,          EV_VCSVC_BGN + 196 );
    OSPEVENT( VCS_MCU_MTJOINCONF_NACK,         EV_VCSVC_BGN + 197 );

	OSPEVENT( VCS_MCU_STOP_ALL_MONITORUNION_CMD,  EV_VCSVC_BGN + 198 );
    
    //[5/19/2011 zhushengze]vcs通知mcu组呼会议终端入会加锁或解锁 消息体:(u8)0-解锁,1-加锁
	OSPEVENT( VCS_MCU_GROUPMODELOCK_CMD,		EV_VCSVC_BGN  + 199  );
	
#endif /*_EVMCUVCS_H_*/


