/********************************************************************
模块名:      会议控制台界面测试库
文件名:      McsGuiTestLib.h
相关文件:    McsGuiTestCom.h	GuiUnit.h	kdvType.h	kdvDef.h
文件实现功能 会议控制台集成测试界面库接口定义
作者：       王昊  
版本：       4.0
------------------------------------------------------------------------
修改记录:
日		期	版本	修改人	走读人	修改内容
2004/11/22	3.5		王昊			创建
2005/07/08  4.0     顾振华          新增4.0功能
*********************************************************************/

/*********************************************************************
							使用说明

1.启动。
	a.调用 GuiRunMcs(LPCTSTR szPath) 接口, 传入会议控制台应用程序路径名,
		若应用程序已经启动则跳到c, 否则转入b

	b.调用 ConnectMcu(LPCTSTR szIp, LPCTSTR szUser, LPCTSTR szPwd),
		传入会议控制台要连接的MCU的参数, 会议控制台会自动连接该MCU。然后转入d

	c.调用 GetMcuProcessIDByAlias(const s8 *pszMcuName, const s8 *pszProcessName = MCSUI_PROCESS_NAME),
		传入连接的MCU名和进程名, 自动查找已启动的会议控制台。然后转入d

	d.连接控制台成功后,再调用 GuiInitUIHandle()接口对界面元素进行初始化, 
		若c.中连接失败或d.中查找失败将会初始化失败。然后转入e或直接转入f

	e.可选。调用 SetOprDelay(BOOL bDelay = TRUE) 设置具体操作是否延时, 默认为有延时。然后转入f

	f.最后调用具体接口进行界面操作.

2.会议操作之前请切换到所操作会议

3.增加终端时, 直接在本级MCU上增加, 呼叫方式固定
*********************************************************************/

#ifndef	__MCSUITESTLIB_H
#define __MCSUITESTLIB_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <winsock2.h>
#include "osp.h"
#include "vccommon.h"
#include "mcuconst.h"
#include "McsGuiTestCom.h"
#include "GuiUnit.h"
#include "addrbook.h"

//	终端参数
typedef struct tagMtCfg
{
    tagMtCfg()
    {
        byAliasType = mtAliasTypeH323ID;
        ZeroMemory(aszAlias, sizeof (aszAlias));
        ZeroMemory(reserved, sizeof (reserved));
    }
	u8	byAliasType;		            //	终端别名类型, mtAliasTypeTransportAddress, mtAliasTypeE164, mtAliasTypeH323ID, mtAliasTypeH320ID
	s8	aszAlias[MAXLEN_DISP_ALIAS];	//	别名， H320是截断后的别名
    u32  reserved[16];                  //  保留, 当type是320ID，[0] = 层号，[1] = 槽号， [2] = 通道号
} TMtCfg;

//	H320终端参数
typedef struct tagH320MtCfg
{
    tagH320MtCfg()
    {
        byLayer = 0;
        bySlot = 0;
        byChannel = 0;
    }   
	s8  byLayer;                        // 层号
    s8  bySlot;                         // 槽号
    s8  byChannel;                      // 通道号

} TH320MtCfg;

// 添加终端时的参数
typedef struct tagAddMtCfg
{
    tagAddMtCfg()
    {
        ZeroMemory(aszMtAlias, sizeof (aszMtAlias));
        byAddToAddr = 0;
        wMtBitrate  = 0;
    }
    s8     aszMtAlias[MAXLEN_DISP_ALIAS];      //  终端别名
    u8     byAddToAddr;                        //  是否保存到地址簿, 0 - 不保存, 1- 保存
    u16    wMtBitrate;                         //  终端码率
} TAddMtCfg;

// 添加H320终端时的参数
typedef struct tagAddH320MtCfg
{
    tagAddH320MtCfg()
    {
        byAddToAddr = 0;
    }
    tagH320MtCfg m_tCfg;                               //  终端信息 
    u8           byAddToAddr;                          //  是否保存到地址簿, 0 - 不保存, 1- 保存
} TAddH320MtCfg;

//	终端操作参数
typedef struct tagMtList
{
    tagMtList()
    {
        byMtNum = 0;
        ZeroMemory(aszMtList, sizeof (aszMtList));
        ZeroMemory(reserved, sizeof(reserved));
    }
	u8	byMtNum;					        //	此次操作终端个数
	s8	aszMtList[16][MAXLEN_DISP_ALIAS];	//	终端别名, 当是H320终端时,是界面截断后的显示字符串
    u32 reserved [16];
} TMtList;

//	画面合成终端参数
typedef struct tagVmpMt
{
    tagVmpMt()
    {
        ZeroMemory(aszMtAlias, sizeof (aszMtAlias));
        byChannel       = 0;
        byMemberType    = VMP_MEMBERTYPE_MCSSPEC;
    }
	s8	aszMtAlias[MAXLEN_DISP_ALIAS];	//	终端别名
	u8	byChannel;					    //	放入哪一个通道
    u8	byMemberType;				    //	跟随方式
} TVmpMt;

typedef struct tagVideoSrc
{
    tagVideoSrc()
    {
        ZeroMemory(aszMtAlias, sizeof (aszMtAlias));
        byPort = 1;
    }
    s8  aszMtAlias[MAXLEN_DISP_ALIAS];      //  终端名
    u8  byPort;                             //  视频源端口
} TVideoSrc;

//	录像参数
typedef struct tagRecorderCfg
{
    tagRecorderCfg()
    {
        ZeroMemory(aszMtAlias,  sizeof (aszMtAlias));
        ZeroMemory(aszRecName,  sizeof (aszRecName));
        ZeroMemory(aszFileName, sizeof (aszFileName));
        byRecMode   = REC_MODE_NORMAL;
        byRecInLowBitrate   = 0;
        byPublishMode   = PUBLISH_MODE_NONE;
        byPublishLevel  = PUBLISH_LEVEL_NORMAL;
    }
	s8	aszMtAlias[MAXLEN_DISP_ALIAS];	//录像终端名
	s8	aszRecName[32];					//录像机名
	s8	aszFileName[MAX_PATH];			//录像文件名
	u8	byRecMode;						//REC_MODE_NORMAL, REC_MODE_SKIPFRAME
	u8	byRecInLowBitrate;				//是否录制低码率图像
	u8	byPublishMode;					//PUBLISH_MODE_NONE, PUBLISH_MODE_IMMED, PUBLISH_MODE_FIN
	u8	byPublishLevel;					//PUBLISH_LEVEL_NORMAL, PUBLISH_LEVEL_SECRET
} TRecorderCfg;

typedef struct tagVideoSrcPoll
{
    tagVideoSrcPoll()
    {
        ZeroMemory(aszMtAlias, sizeof (aszMtAlias));
        byPortNum = 0;
        ZeroMemory(aaszPort, sizeof (aaszPort));
        ZeroMemory(abyTime, sizeof (abyTime));
    }
    s8  aszMtAlias[MAXLEN_DISP_ALIAS];                          //  终端名
    u8  byPortNum;                                              //  端口总数
    s8  aaszPort[MATRIX_MAX_PORT][MATRIX_PORTNAME_LEN + 1];     //  端口名
    u8  abyTime[MATRIX_MAX_PORT];                               //  每个端口的轮询间隔
} TVideoSrcPoll;




// Mcu 配置向导的传入参数，，IP地址为网络序
typedef struct tagMcuWizardSetting
{
    tagMcuWizardSetting()
    {
        dwMcuIP    = 0;
        dwMcuMask  = 0;
        dwGatewayIP= 0;

        chRegGk    = 0;
        dwGkIP     = 0;
        
        ZeroMemory(&szMcuAlias, sizeof(szMcuAlias));
        ZeroMemory(&szMcuE164, sizeof(szMcuE164));
        
    }

    u32 dwMcuIP;                                                  // Mcu IP
    u32 dwMcuMask;                                                // Mcu 子网掩码
    u32 dwGatewayIP;                                              // 网关 IP

    s8  chRegGk;                                                  // 是否注册Gk, 
                                                                  // 0= 不修改, 1 = yes, -1 = no
    //  当注册到 Gk 时，需要填Gk Ip
    u32 dwGkIP;                                                   // Gk IP

    s8  szMcuAlias[MAXLEN_ALIAS];			                      // Mcu 别名
    s8  szMcuE164[MAXLEN_E164];			                          // E164 号

    BOOL32 bReboot;                                               // 是否立即重新启动
} TMcuWizardSetting;

// Mcu 网络配置参数，IP地址为网络序
typedef struct tagMcuNetworkSetting
{
    tagMcuNetworkSetting()
    {
        dwMcuIP    = 0;
        dwMcuMask  = 0;
        dwGatewayIP= 0;
            
        chRegGk    = 0;
        dwGkIP     = 0;

        dwMcAddr   = 0;
        dwMcPort   = 0;
        dwDPort    = 0;
        dwHPort    = 0;
            
        wMaxMtNum  = 0;

        ZeroMemory(&szMcuAlias, sizeof(szMcuAlias));
        ZeroMemory(&szMcuE164, sizeof(szMcuE164));
    }

    u32 dwMcuIP;                                                  // Mcu IP
    u32 dwMcuMask;                                                // Mcu 子网掩码
    u32 dwGatewayIP;                                              // 网关 IP

    s8  chRegGk;                                                  // 是否注册Gk, 
                                                                  // 0= 不修改, 1 = yes, -1 = no  
    //  当注册到 Gk 时，需要填Gk Ip
                                                                  
    u32 dwGkIP;                                                   // Gk IP
    s8  szMcuAlias[MAXLEN_ALIAS];			                      // Mcu 别名
    s8  szMcuE164[MAXLEN_E164];			                          // E164 号


    u32  dwMcAddr;                                                //  组播地址
    u32  dwMcPort;                                                //  组播起始端口
    u32  dwDPort;                                                 //  数据起始端口
    u32  dwHPort;                                                 //  H225起始端口   

    u16  wMaxMtNum;                                               //  最大连接终端数
  
} TMcuNetworkSetting;

// QoS  配置信息
typedef struct tagMcuQosSetting
{
    tagMcuQosSetting()
    {
        m_byQosType  = MCUQOS_SERV; 
        m_byAudLevel = 0;
        m_byVidLevel = 0;
        m_byDataLevel   = 0;
        m_bySignalLevel = 0;
        m_byIpServiceType = IPSERVICETYPE_LOW_DELAY;  
        
        m_bReset = FALSE;
    }

    u8  m_byQosType;					// Qos 类型 MCUQOS_SERV, MCUQOS_IP
    u8  m_byAudLevel;					// 音频等级
    u8  m_byVidLevel;					// 视频等级
    u8  m_byDataLevel;					// 数据等级
    u8  m_bySignalLevel;				// 信号等级
    u8  m_byIpServiceType;				// IP服务类型，由各个参数做或操作，
                                        // IPSERVICETYPE_LOW_DELAY
                                        // IPSERVICETYPE_HIGH_THROUGHPUT
                                        // IPSERVICETYPE_HIGH_RELIABILITY
                                        // IPSERVICETYPE_LOW_EXPENSE
                                        // IPSERVICETYPE_NONE 表示不填

    

    BOOL32 m_bReset;                    // TRUE 表示恢复到默认值, 除类型外其他字段可以不用填写
} TMcuQosSetting;

// SNMP  配置信息
typedef struct tagMcuSnmpSetting
{
    tagMcuSnmpSetting()
    {
        m_dwTrapIp = 0;
        m_wReadWritePort = 0;
        m_wSendTrapPort = 0;
        ZeroMemory(&m_szReadCom, MAXLEN_TRAP_COMMUNITY);
        ZeroMemory(&m_szWriteCom, MAXLEN_TRAP_COMMUNITY);
    }
    u32  m_dwTrapIp;							// Ip    
    u16  m_wReadWritePort;						// 读写端口
    u16  m_wSendTrapPort;						// 发Trap端口
    s8   m_szReadCom[MAXLEN_TRAP_COMMUNITY];	// 读共同体
    s8   m_szWriteCom[MAXLEN_TRAP_COMMUNITY];	// 写共同体

} TMcuSnmpSetting;

// Mcu 配置的杂项
typedef struct tagMcuNetMiscSetting
{
    tagMcuNetMiscSetting()
    {
        chSaveBw   = 0;
        chSmoothSend = 0;
        dwChainChkTime = 0;
        dwChainChkTimes = 0;
        dwMtRefTime = 0;
        dwAudRefTime = 0;
        dwVidRefTime = 0;

        dwDcsIp      = 0;


        bMPCTran   = FALSE;
        bMPCStack  = FALSE;

        byNetSynType = 0;
        byDri      = 0;
        byE1       = 0;

        bResetToDefault = FALSE;

    }

    s8  chSaveBw;                                                 // 是否节省无需发送码流的终端的网络带宽
                                                                  // 0= 不修改, 1 = yes, -1 = no
    s8  chSmoothSend;                                             // 是否平滑发送
                                                                  // 0= 不修改, 1 = yes, -1 = no
    u32 dwChainChkTime;                                           // 断链检测间隔时间(秒)
    u32 dwChainChkTimes;                                          // 断链检测次数
    u32 dwMtRefTime;                                              // 级联会议终端列表刷新间隔(秒)
    u32 dwAudRefTime;                                             // 级联会议音频源刷新间隔(秒)
    u32 dwVidRefTime;                                             // 级联会议视频源刷新间隔(秒)

    u32 dwDcsIp;                                                  // 数据会议服务器Ip (0 为不启用)
                                                                  // 网络序

    BOOL32 bMPCTran;                                              //  是否MPC 转发
    BOOL32 bMPCStack;                                             //  是否MPC内置协议栈

    u8   byNetSynType;                                            //  网络同步模式 
                                                                  //  MCUSYNMODE_VIBRATE ,  MCUSYNMODE_TRACK
                                                                  //  跟踪时需填下面2项
    u8   byDri;                                                   //  DRI 板号 
    u8   byE1;                                                    //  E1 通道号

    BOOL32 bResetToDefault;                      //  是否恢复到默认值，
                                                 //  如果填了此参数 TRUE，其他相关参数将被忽略

} TMcuNetMiscSetting;


typedef struct tagVmpStyleCfg
{
    tagVmpStyleCfg()
    {

        bySchemaId = 0;
        ZeroMemory(aszSchemaName, sizeof(aszSchemaName));

        cBgColor = 0;
        cMtColor = 0;
        cCmColor = 0;
        cSpColor = 0;
    }
/*
    u32      dwFontIndex;                                       // 字体索引值(未定)
    u32      dwAlphaIndex;                                      // 透明度索引(未定)
    u32      dwSizeIndex;                                       // 大小索引(未定)
    COLORREF cTextForeColor;                                    // 字体前景色
    COLORREF cTextBkColor;                                      // 字体背景色
*/
    u8       bySchemaId;                                        // 方案索引号 1-5
    s8       aszSchemaName[16];                                 // 方案名

    COLORREF cBgColor;                                          // 背景色
    COLORREF cMtColor;                                          // 一般终端边框色
    COLORREF cCmColor;                                          // 主席终端边框色
    COLORREF cSpColor;                                          // 发言人边框色
} TVmpStyleCfg;

// Mcu 包重传设备配置参数
typedef struct tagMcuPrsSetting
{
    tagMcuPrsSetting()
    {
        dwTime1 = 0;
        dwTime2 = 0;
        dwTime3 = 0;
        dwTime4 = 0;

        bResetToDefault = FALSE;
    }
    u32  dwTime1;                                                //  第1重传(ms)
    u32  dwTime2;                                                //  第2重传(ms)
    u32  dwTime3;                                                //  第3重传(ms)
    u32  dwTime4;                                                //  丢弃时间(ms)

    BOOL32 bResetToDefault;                                      //  是否恢复到默认值
} TMcuPrsSetting;


// 单板模块的配置，各个模块选填相应的部分。其他留空即可。
typedef struct tagMcuChipModuleCfg
{
    tagMcuChipModuleCfg()
    {
        byModuleType = 0;
        ZeroMemory(aszModuleName, sizeof(aszModuleName));
        dwMcuStartPort = 0;
        bySwitchBrdIdx = 0;
        dwRecvStartPort = 0;
        for (s32 nIndex = 0; nIndex < 5; nIndex ++)
        {
            bUseMap[nIndex] = FALSE;
        }
        byMaxMinChnl = 0;
        byVideoPipe = 1;
    }

    u8      byModuleType;                   // 模块类型, MIXER_MASK  REC_MASK
                                            // TVWALL_MASK, ADAPTER_MASK
                                            // VMP_MASK, PRS_MASK, MULTITVWALL_MASK
    TCHAR   aszModuleName[MAXLEN_NAME];     // 单板模块别名
    u32     dwMcuStartPort;                 // Mcu 起始端口号
    u8      bySwitchBrdIdx;                 // 交换板索引号
    u32     dwRecvStartPort;                // 接收数据起始端口号
    BOOL32  bUseMap[5];                     // 0-4是否启用相应的Map

    // 以下4个是一个union，最多只要填一个
    u8      byMaxMinChnl;                   // 最大混音通道数
    u8      byVideoPipe;                    // 1或2，视频编码路数
    TMcuPrsSetting tPrsCfg;                 // 丢包重传配置
    u32     dwRecIp;                        // 录像机IP地址
} TMcuChipModuleCfg;

// 单板配置信息，IP地址为网络序
typedef struct tagMcuChipCfg
{
    tagMcuChipCfg()
    {
        byChipType = -1;
        dwIP = 0;
        byLayer = 0;
        bySlot = 0;
        for (s32 nIndex = 0; nIndex < 5; nIndex++)
        {
            bUseMod[nIndex] = FALSE;
        }
        byLanIf = 0;
        bBroadcast = FALSE;

    }
    s8  byChipType;                       // 板类型 
                                          // MCUCHIP_MPC
                                          // MCUCHIP_APU
                                          // MCUCHIP_VPU  
                                          // MCUCHIP_DEC5 
                                          // MCUCHIP_CRI  
                                          // MCUCHIP_DRI  
                                          // MCUCHIP_MMP

    u32 dwIP;                             // IP地址
	u8  byLayer;                          // 层号
    u8  bySlot;                           // 槽号

    u8  byLanIf;                          // 网口选择，0:前网口，1: 后网口，8000B只能选前网口
    BOOL32 bBroadcast;                    // 是否组播，只对Mpc、Cri、Dri板有效，且每个网段必须要有一块板组播

    BOOL32 bUseMod[5];                    // 是否启用相应模块,TRUE 启用
    TMcuChipModuleCfg tModuleCfg[5];      // 模块配置信息
}TMcuChipCfg;



class CMcsIntTestUI
{
public:
	CMcsIntTestUI();
	virtual ~CMcsIntTestUI();

public:
	//-----------------------------------------------------------------------------
	//	创建程序初始化操作
	//-----------------------------------------------------------------------------

	/*=============================================================================
	函 数 名： GuiRunMcs
	功    能： 运行会议控制台程序
	参    数： LPCTSTR szPath					[in] 会议控制台路径名
	返 回 值： BOOL
	=============================================================================*/
	virtual BOOL GuiRunMcs(LPCTSTR szPath);

	/*=============================================================================
	函 数 名： SetGuiMainWindow
	功    能： 查找应用程序是否存在，若存在设置GuiUnit的主窗口句柄
				应该在函数GuiRunMcs调用后再调用此函数
	参    数： s32 nTimeOut					[in] 查找应用程序超时时间(时间单位：毫秒)
	说    明： 因为应用程序启动需要一定时间，若在此时间内还没有捕捉到应用程序将返回false
	返 回 值： BOOL
	=============================================================================*/
	virtual BOOL SetGuiMainWindow(s32 nTimeOut = 0);

	/*=============================================================================
	函 数 名： GuiInitUIHandle
	功    能： 登录后初始化界面元素句柄
	参    数：
	返 回 值： BOOL
	=============================================================================*/
	virtual BOOL GuiInitUIHandle();

	/*=============================================================================
	函 数 名： SetOprDelay
	功    能： 设置是否操作延时（默认为延时）
	参    数： BOOL bDelay					[in]	true-延时, false-不延时
	返 回 值： void
	=============================================================================*/
	inline void SetOprDelay(BOOL bDelay = TRUE)
	{
		m_bOprDelay = bDelay;
	}

	/*=============================================================================
	函 数 名： GuiActive
	功    能： 将会议控制台设为当前窗口 
	返 回 值： BOOL
	=============================================================================*/
	virtual BOOL GuiActive();

	/*=============================================================================
	函 数 名：GetMcuProcessIDByAlias
	功    能：根据已连接的MCU名和进程名设置进程ID
	参    数： 	pszMcuName					[in]	MCU名
				pszProcessName				[in]	进程名
	返 回 值：BOOL
	=============================================================================*/
	virtual BOOL GetMcuProcessIDByAlias(const s8 *pszMcuName, const s8 *pszProcessName = MCSUI_APP_EXE_NAME);

    /*=============================================================================
	函 数 名：CloseMcs
	功    能：关闭MCS
	参    数：无
	返 回 值：无
	=============================================================================*/
    virtual void CloseMcs(void);

public:
	//-----------------------------------------------------------------------------
	//	界面操作
	//-----------------------------------------------------------------------------

	//-----------------------------------------------------------------------------
	//	1. MCU操作
	//-----------------------------------------------------------------------------

	/*=============================================================================
	函 数 名： ConnectMcu
	功    能： 会议控制台连接MCU
	参    数：	LPCTSTR szIp				[in]	IP地址
				LPCTSTR szUser				[in]	用户名
				LPCTSTR szPwd				[in]	密码
	返 回 值： BOOL
	=============================================================================*/
	virtual BOOL ConnectMcu(LPCTSTR szIp, LPCTSTR szUser, LPCTSTR szPwd);

	/*=============================================================================
	函 数 名：DisconnectMcu
	功    能：断开MCU连接
	参    数：pszMcu						[in]	MCU名
	返 回 值：BOOL
	=============================================================================*/
	virtual BOOL DisconnectMcu(const s8 *pszMcu = NULL);

	/*=============================================================================
	函 数 名： SelectMcu
	功    能： 选择Mcu
	参    数： pszMcu					[in]	MCU名(会议控制台MCU选择框里的名字)
	返 回 值： BOOL
	=============================================================================*/
	virtual BOOL SelectMcu(const s8 *pszMcu);

	//-----------------------------------------------------------------------------
	//	2. 警告确认操作
	//-----------------------------------------------------------------------------

	/*=============================================================================
	函 数 名：KillWarningDlg
	功    能：关闭弹出的警告框
	参    数：hParent				[in]	父对话框句柄
	返 回 值：BOOL
	=============================================================================*/
	virtual BOOL KillWarningDlg(HWND hParent = NULL);

    /*=============================================================================
	函 数 名：KillAllWarningDlg
	功    能：关闭所有弹出的警告框
	参    数：无
	返 回 值：无
	=============================================================================*/
    virtual void KillAllWarningDlg();

	/*=============================================================================
	函 数 名：ConfirmOperation
	功    能：确认消息对话框点击确定或取消
	参    数：bConfirm				[in]	点击确定或取消。TRUE表示确定，FALSE取消
	返 回 值：BOOL
	=============================================================================*/
	virtual BOOL ConfirmOperation(BOOL bConfirm = TRUE);

	/*=============================================================================
	函 数 名：ConfirmPwd
	功    能：确认密码
	参    数：	pszPwd							[in]	密码
				hParent							[in]	父窗口句柄
	返 回 值：BOOL
	=============================================================================*/
	virtual BOOL ConfirmPwd(const s8 *pszPwd, HWND hParent = NULL);

	//-----------------------------------------------------------------------------
	//	3. 界面切换操作
	//-----------------------------------------------------------------------------

	/*=============================================================================
	函 数 名：GuiTemplate
	功    能：切换到模板管理界面
	参    数：无
	返 回 值：BOOL
	-----------------------------------------------------------------------------
	修改记录：
	日		期	版本	修改人	走读人	修改内容
	2004/12/06	3.5		王昊			创建
	=============================================================================*/
	virtual BOOL GuiTemplate();

	/*=============================================================================
	函 数 名：GuiConfList
	功    能：切换到会议列表界面
	参    数：无
	返 回 值：BOOL
	-----------------------------------------------------------------------------
	修改记录：
	日		期	版本	修改人	走读人	修改内容
	2004/12/06	3.5		王昊			创建
	=============================================================================*/
	virtual BOOL GuiConfList();

	/*=============================================================================
	函 数 名：GuiSimConf
	功    能：切换到模拟会场界面
	参    数：无
	返 回 值：BOOL
	-----------------------------------------------------------------------------
	修改记录：
	日		期	版本	修改人	走读人	修改内容
	2004/12/06	3.5		王昊			创建
	=============================================================================*/
	virtual BOOL GuiSimConf();

	/*=============================================================================
	函 数 名：GuiMonitor
	功    能：切换到监控界面
	参    数：无
	返 回 值：BOOL
	-----------------------------------------------------------------------------
	修改记录：
	日		期	版本	修改人	走读人	修改内容
	2004/12/13	3.5		王昊			创建
	=============================================================================*/
	virtual BOOL GuiMonitor();

	/*=============================================================================
	函 数 名：GuiAddrbook
	功    能：切换到地址簿界面
	参    数：无
	返 回 值：BOOL
	-----------------------------------------------------------------------------
	修改记录：
	日		期	版本	修改人	走读人	修改内容
	2004/12/13	3.5		王昊			创建
	=============================================================================*/
	virtual BOOL GuiAddrbook();

	/*=============================================================================
	函 数 名：GuiUser
	功    能：切换到用户管理界面
	参    数：无
	返 回 值：BOOL
	-----------------------------------------------------------------------------
	修改记录：
	日		期	版本	修改人	走读人	修改内容
	2004/12/13	3.5		王昊			创建
	=============================================================================*/
	virtual BOOL GuiUser();

	/*=============================================================================
	函 数 名：GuiDevManage
	功    能：切换到设备管理界面
	参    数：无
	返 回 值：BOOL
	-----------------------------------------------------------------------------
	修改记录：
	日		期	版本	修改人	走读人	修改内容
	2004/12/13	3.5		王昊			创建
	=============================================================================*/
	virtual BOOL GuiDevManage();

    /*=============================================================================
	函 数 名：GuiUserSetting
	功    能：切换到用户设置界面
	参    数：无
	返 回 值：BOOL
	-----------------------------------------------------------------------------
	修改记录：
	日		期	版本	修改人	走读人	修改内容
	2005/03/11	3.6		王昊			创建
	=============================================================================*/
    virtual BOOL32 GuiUserSetting();

    // 以下几个界面切换函数是4.0新增加的。

    /*============================================================================
    函 数 名：GuiMcuManage
    功    能：切换到Mcu基本信息管理界面
    参    数：无
    返 回 值：BOOL
    ============================================================================*/
    virtual BOOL GuiMcuManage();

    /*============================================================================
    函 数 名：GuiMcuConfig
    功    能：切换到Mcu配置界面
    参    数：无
    返 回 值：BOOL
    ============================================================================*/
    virtual BOOL GuiMcuConfig();


	/*=============================================================================
	函 数 名：ChangeViewType
	功    能：点击视图按钮, 切换视图模式
	参    数：byType							[in]	0:平铺大图标 1:树型 2:平铺小图标
	说    明：
	返 回 值：BOOL
	=============================================================================*/
	virtual BOOL ChangeViewType(u8 byType);

    
	//-----------------------------------------------------------------------------
	//	4.会议操作
	//-----------------------------------------------------------------------------

	/*=============================================================================
	函 数 名： SelectConf
	功    能： 在模拟会场中选择会议
	参    数：	pszConfName				[in]	会议名
	返 回 值： BOOL
	=============================================================================*/
	virtual BOOL SelectConf(const s8 *pszConfName);


	/*=============================================================================
	函 数 名：CreateTemplate
	功    能：创建会议模板
	参    数：	ptConfInfo							[in]	会议创建参数
				wMtNum								[in]	终端个数
				ptAddMtCfg							[in]	终端参数
                wH320MtNum                          [in]	H320终端个数
                ptAddH320MtCfg                      [in]	H320终端参数
				ptVmpModule							[in]	画面合成参数
                pszTwName                           [in]    启用的电视墙别名
				ptTvModule							[in]	电视墙参数
                bUseMainDlg                         [in]    TRUE表示尽量使用主对话框进行信息输入
                                                            FALSE表示都采用弹出式对话框输入
	说    明：
	返 回 值：BOOL32
	=============================================================================*/
	virtual BOOL32 CreateTemplate(const TConfInfo *ptConfInfo, 
                                u16 wMtNum, const TAddMtCfg *ptAddMtCfg,
                                u16 wH320MtNum, const TAddH320MtCfg *ptAddH320MtCfg,
                                const TVmpModule *ptVmpModule = NULL,
                                const s8 *pszTwName = _T(""),
                                const TTvWallModule *ptTvModule = NULL,
                                const BOOL32 bUseMainDlg = TRUE);
    

	/*=============================================================================
	函 数 名：CreateConf
	功    能：根据模板创建会议
	参    数：	pszConfName			[in]	会议名
                tStartTime          [in]    预约会议的时间
				bSchedule           [in]    TRUE 表示预约会议，FALSE 表示即时会议
                bUseWizard          [in]    TRUE 表示通过向导创建，FALSE表示模版界面创建
	说    明：即时会议不需要填 tStartTime
	返 回 值：BOOL
	=============================================================================*/
	virtual BOOL CreateConf(const s8 *pszConfName, const SYSTEMTIME tStartTime, 
                            BOOL32 bSchedule = FALSE, 
                            BOOL32 bUseWizard = FALSE);


	/*=============================================================================
	函 数 名： ReleaseConf
	功    能： 结束会议
	参    数：	ptszMcu				[in]	会议所在MCU名(会议控制台MCU选择框里的名字)
				pszConfName			[in]	会议名
				byTakeMode			[in]	会议模式 0:预约会议CONF_TAKEMODE_SCHEDULED
												 	 1:即时会议CONF_TAKEMODE_ONGOING
													 2:会议模版CONF_TAKEMODE_TEMPLATE
	说    明：会议模板，只会在模板管理界面结束。即时会议在模拟会场界面结束，若未找到，则在会议列表界面结束。预约会议只在会议列表界面结束
	返 回 值： BOOL
	=============================================================================*/
	virtual BOOL ReleaseConf(const s8 *pszConfName, u8 byTakeMode);



	/*=============================================================================
	函 数 名：ModifyConf
	功    能：修改预约会议
	参    数：	pszConfName							[in]	需要修改会议的会议名
                tStartTime                          [in]    新的预约会议的时间
	返 回 值：BOOL
	=============================================================================*/
	virtual BOOL ModifyConf(const s8 *pszConfName, const SYSTEMTIME tStartTime);

	/*=============================================================================
	函 数 名：SaveConf
	功    能：保存会议
	参    数：	pszConfName							[in]	会议名
				bSaveAsDefault						[in]	是否存为缺省会议
	返 回 值：BOOL
	=============================================================================*/
	virtual BOOL SaveConf(const s8 *pszConfName, BOOL bSaveAsDefault = FALSE);


    // 终端操作


	/*=============================================================================
	函 数 名：AddMt
	功    能：增加终端
	参    数：	pszConfName							[in]	会议名
				ptAddMtCfg							[in]	要添加的终端
				nMtNum								[in]	终端个数
				pszMcuAlias							[in]	需要增加终端的MCU, 为NULL则默认本级MCU
				bAddFromAddr						[in]	从地址簿添加
	说    明：
	返 回 值：BOOL
	=============================================================================*/


	virtual BOOL AddMt(const s8 *pszConfName, const TAddMtCfg *ptAddMtCfg, 
						s32 nMtNum = 1, const s8 *pszMcuAlias = NULL,
                        BOOL bAddFromAddr = FALSE);


    //以下函数是新增加的,如果需要从地址簿添加就直接用AddMt()就可以了.
    /*============================================================================
    函 数 名：AddH320Mt
    功    能：增加H320终端
    参    数：	pszConfName					[in]	会议名
			    ptH320MtCfg 				[in]	要添加的终端
			    nMtNum						[in]	终端个数
			    pszMcuAlias					[in]	需要增加终端的MCU, 为NULL则默认本级MCU
    说    明：
    返 回 值：BOOL
    ============================================================================*/
    virtual BOOL AddH320Mt(const s8 *pszConfName, const TAddH320MtCfg *ptH320MtCfg, 
					    s32 nMtNum = 1, const s8 *pszMcuAlias = NULL);

	/*=============================================================================
	函 数 名：DelMt
	功    能：删除终端
	参    数：	pszConfName								[in]	会议名
				ptMtAlias								[in]	要删除的终端别名，
                                                                对于H320是截断后的别名
				bySendWarning							[in]	是否通知被挂断终端
																1: 通知; 0: 不通知
	说    明：
	返 回 值：BOOL
	=============================================================================*/
	virtual BOOL DelMt(const s8 *pszConfName, const s8 *ptMtAlias, u8 bySendWarning = 1);



	/*=============================================================================
	函 数 名：CallMt
	功    能：呼叫终端
	参    数：	pszConfName								[in]	会议名
				ptMtAlias								[in]	要呼叫的终端
	说    明：
	返 回 值：BOOL
	=============================================================================*/
	virtual BOOL CallMt(const s8 *pszConfName, const s8 *ptMtAlias);

	/*=============================================================================
	函 数 名：DropMt
	功    能：挂断终端
	参    数：	pszConfName								[in]	会议名
				ptMtAlias								[in]	要挂断的终端
				bySendWarning							[in]	是否通知被挂断终端
																1: 通知; 0: 不通知
	说    明：
	返 回 值：BOOL
	=============================================================================*/
	virtual BOOL DropMt(const s8 *pszConfName, const s8 *ptMtAlias, u8 bySendWarning = 1);

	/*=============================================================================
	函 数 名：SpecChairman
	功    能：指定主席
	参    数：	pszConfName					[in]	会议名
				pszMtAlias					[in]	终端别名
				byType						[in]	0:在终端上点击右键选择指定;
													1:拖动终端到主席台指定
	说    明：
	返 回 值：BOOL
	=============================================================================*/
	virtual BOOL SpecChairman(const s8 *pszConfName, const s8 *pszMtAlias, u8 byType = 0);

	/*=============================================================================
	函 数 名：CancelChairman
	功    能：取消主席
	参    数：	pszConfName				[in]	会议名
				byType					[in]	0:在终端上点击右键选择取消;
												1:从到主席台拖动终端取消
	说    明：
	返 回 值：BOOL
	=============================================================================*/
	virtual BOOL CancelChairman(const s8 *pszConfName, u8 byType = 0);

	/*=============================================================================
	函 数 名：SpecSpeaker
	功    能：指定发言人
	参    数：	pszConfName					[in]	会议名
				pszMtAlias					[in]	终端别名
				byType						[in]	0:在终端上点击右键选择指定;
													1:拖动终端到主席台指定
	说    明：
	返 回 值：BOOL
	=============================================================================*/
	virtual BOOL SpecSpeaker(const s8 *pszConfName, const s8 *pszMtAlias, u8 byType = 0);

	/*=============================================================================
	函 数 名：CancelSpeaker
	功    能：取消发言人
	参    数：	pszConfName				[in]	会议名
				byType					[in]	0:在终端上点击右键选择取消;
												1:从到主席台拖动终端取消
	说    明：
	返 回 值：BOOL
	=============================================================================*/
	virtual BOOL CancelSpeaker(const s8 *pszConfName, u8 byType = 0);

	/*=============================================================================
	函 数 名：SendMsg
	功    能：发送短消息
	参    数：	pszConfName						[in]	会议名
				ptMtList						[in]	终端列表
				nRollTimes						[in]	滚动次数
				byMsgType						[in]	ROLLMSG_TYPE_SMS、ROLLMSG_TYPE_PAGETITLE、ROLLMSG_TYPE_ROLLTITLE
				byRollSpeed						[in]	ROLL_SPEED_1、ROLL_SPEED_2、ROLL_SPEED_3
                pszMsg							[in]	短消息内容
                pszFileName                     [in]    短消息文件
                pszLoadMsg                      [in]    选用短消息
                bSaveToFile                     [in]    存为常用短消息
	说    明：首先判断pszFileName, 不为NULL则文件载入;
              然后判断pszLoadMsg, 不为NULL则选用短消息;
              否则填入pszMsg
	返 回 值：BOOL
	=============================================================================*/
	virtual BOOL SendMsg(const s8 *pszConfName, const TMtList *ptMtList,
                        s32 nRollTimes = 1, u8 byMsgType = ROLLMSG_TYPE_SMS,
                        u8 byRollSpeed = ROLL_SPEED_1, const s8 *pszMsg = NULL, 
                        const s8 *pszFileName = NULL, const s8 *pszLoadMsg = NULL,
                        BOOL32 bSaveToFile = FALSE);

    /*=============================================================================
	函 数 名：DeleteMsg
	功    能：删除短消息
	参    数：	pszConfName						[in]	会议名
				pszMsg						    [in]	短消息内容
	说    明：
	返 回 值：BOOL
	=============================================================================*/
    virtual BOOL32 DeleteMsg(const s8 *pszConfName, const s8 *pszMsg);

	/*=============================================================================
	函 数 名：StartPoll
	功    能：开始轮询
	参    数：	pszConfName			[in]	会议名
				ptMtList						[in]	终端列表
				wPollTime						[in]	轮询间隔                
				bEnableAudio					[in]	是否带音频
                wPollTimes                      [in]    轮询次数
	说    明：
	返 回 值：BOOL
	=============================================================================*/
	virtual BOOL StartPoll(const s8 *pszConfName, const TMtList *ptMtList, 
                           u16 wPollTime, BOOL bEnableAudio,
                           u16 wPollTimes);

	/*=============================================================================
	函 数 名：StopPoll
	功    能：停止轮询
	参    数：pszConfName			[in]	会议名
	说    明：
	返 回 值：BOOL
	=============================================================================*/
	virtual BOOL StopPoll(const s8 *pszConfName);

    //4.0 支持电视墙轮询操作
	/*=============================================================================
	函 数 名：StartTwPoll
	功    能：开始电视墙轮询
	参    数：	pszConfName         			[in]	会议名
				ptMtList						[in]	终端列表
                byTvWallIdx                     [in]    选择ComboBox中哪个电视墙  
                byTvWallChannel                 [in]    选择哪个通道
				wPollTime						[in]	轮询间隔
				wPollTimes                      [in]    轮询次数
	说    明：
	返 回 值：BOOL32
    -----------------------------------------------------------------------------
    修改记录：
    日		期	版本	修改人	走读人	修改内容
    2005/12/26	4.0		顾振华			创建
	=============================================================================*/
	virtual BOOL32 StartTwPoll(const s8 *pszConfName, const TMtList *ptMtList, 
                               u8  byTvWallIdx, u8 byTvWallChannel,
                               u16 wPollTime,
                               u16 wPollTimes);

	/*=============================================================================
	函 数 名：StopTwPoll
	功    能：停止电视墙轮询
	参    数：pszConfName			[in]	会议名
	说    明：
	返 回 值：BOOL32
    -----------------------------------------------------------------------------
    修改记录：
    日		期	版本	修改人	走读人	修改内容
    2005/12/26	4.0		顾振华			创建
	=============================================================================*/
    virtual BOOL32 StopTwPoll(const s8 *pszConfName,
                              u8  byTvWallIdx, u8 byTvWallChannel);

	/*=============================================================================
	函 数 名：OpenConfCallDlg
	功    能：打开会议点名对话框
	参    数：pszConfName			[in]	会议名
	说    明：
	返 回 值：BOOL
	=============================================================================*/
	virtual BOOL OpenConfCallDlg(const s8 *pszConfName);

	/*=============================================================================
	函 数 名：SetCaller
	功    能：设置点名人
	参    数：pszMtAlias								[in]	点名人
	说    明：必须先打开点名对话框
	返 回 值：BOOL
	=============================================================================*/
	virtual BOOL SetCaller(const s8 *pszMtAlias);

	/*=============================================================================
	函 数 名：SetCallee
	功    能：设置被点名人
	参    数：pszMtAlias								[in]	被点名人
	说    明：必须先打开点名对话框
	返 回 值：BOOL
	=============================================================================*/
	virtual BOOL SetCallee(const s8 *pszMtAlias);

	/*=============================================================================
	函 数 名：StartConfCall
	功    能：开始点名
	参    数：	pszConfName								[in]	会议名
				pszCaller								[in]	点名人
				pszCallee								[in]	被点名人
	说    明：
	返 回 值：BOOL
	=============================================================================*/
	virtual BOOL StartConfCall(const s8 *pszConfName, const s8 *pszCaller, const s8 *pszCallee);

	/*=============================================================================
	函 数 名：StopConfCall
	功    能：停止点名
	参    数：无
	说    明：必须先打开点名对话框
	返 回 值：BOOL
	=============================================================================*/
	virtual BOOL StopConfCall();

	/*=============================================================================
	函 数 名：ChangeLockMode
	功    能：改变锁定模式
	参    数：	pszConfName						[in]	会议名
				byLockMode						[in]	CONF_LOCKMODE_NONE
														CONF_LOCKMODE_NEEDPWD
														CONF_LOCKMODE_LOCK
				aszOldPwd						[in]	旧密码
	说    明：
	返 回 值：BOOL
	=============================================================================*/
	virtual BOOL ChangeLockMode(const s8 *pszConfName, u8 byLockMode, const s8 *aszOldPwd = NULL);

    /*=============================================================================
	函 数 名：LockSMcu
	功    能：锁定下级MCU
	参    数：	pszConfName						[in]	会议名
                pszMcuName                      [in]    MCU名
				bLock						    [in]	锁定/解锁
	说    明：
	返 回 值：BOOL
	=============================================================================*/
    virtual BOOL LockSMcu(const s8 *pszConfName, const s8 *pszMcuName, BOOL32 bLock);

	/*=============================================================================
	函 数 名：ChangePwd
	功    能：更改密码
	参    数：	pszConfName							[in]	会议名
				pszOldPwd							[in]	旧会议密码
				pszNewPwd							[in]	新会议密码
	说    明：
	返 回 值：BOOL
	=============================================================================*/
	virtual BOOL ChangePwd(const s8 *pszConfName, const s8 *pszOldPwd, const s8 *pszNewPwd);

	/*=============================================================================
	函 数 名：StartDiscuss
	功    能：开始讨论
	参    数：pszConfName							[in]	会议名
	说    明：
	返 回 值：BOOL
	=============================================================================*/
	virtual BOOL StartDiscuss(const s8 *pszConfName);

	/*=============================================================================
	函 数 名：StopDiscuss
	功    能：停止讨论
	参    数：pszConfName							[in]	会议名
	说    明：
	返 回 值：BOOL
	=============================================================================*/
	virtual BOOL StopDiscuss(const s8 *pszConfName);

    /*=============================================================================
    函 数 名：MixDelaySet
    功    能：混音延时设置
    参    数：pszConfName							[in]	会议名
              wDelayTime                            [in]    延时时间,ms
    说    明：
    返 回 值：BOOL32
    =============================================================================*/
    virtual BOOL32 MixDelaySet(const s8 *pszConfName, u16 wDelayTime);

	/*=============================================================================
	函 数 名：ForceSpeaker
	功    能：强制发言人
	参    数：pszConfName							[in]	会议名
	说    明：
	返 回 值：BOOL
	=============================================================================*/
	virtual BOOL ForceSpeaker(const s8 *pszConfName);

	/*=============================================================================
	函 数 名：CancelForceSpeaker
	功    能：取消强制发言人
	参    数：pszConfName							[in]	会议名
	说    明：
	返 回 值：BOOL
	=============================================================================*/
	virtual BOOL CancelForceSpeaker(const s8 *pszConfName);

	/*=============================================================================
	函 数 名：VoiceVac
	功    能：语音激励
	参    数：pszConfName							[in]	会议名
	说    明：
	返 回 值：BOOL
	=============================================================================*/
	virtual BOOL VoiceVac(const s8 *pszConfName);

	/*=============================================================================
	函 数 名：CancelVoiceVac
	功    能：取消语音激励
	参    数：pszConfName							[in]	会议名
	说    明：
	返 回 值：BOOL
	=============================================================================*/
	virtual BOOL CancelVoiceVac(const s8 *pszConfName);


	/*=============================================================================
	函 数 名：ConfDelay
	功    能：会议延时
	参    数：	pszConfName							[in]	会议名
				wDelayTime							[in]	延长时间
	返 回 值：BOOL
	=============================================================================*/
	virtual BOOL ConfDelay(const s8 *pszConfName, u16 wDelayTime);

	/*=============================================================================
	函 数 名：VoiceVacTime
	功    能：语音激励时间设置
	参    数：	pszConfName								[in]	会议名
				byVacTime								[in]	激励时间 5, 10, 15, 20, 30
	返 回 值：BOOL
	=============================================================================*/
	virtual BOOL VoiceVacTime(const s8 *pszConfName, u8 byVacTime);

    /*=============================================================================
	函 数 名：LocalMtQuiet
	功    能：设置本级终端静音
	参    数：	pszConfName								[in]	会议名
				bQuiet								    [in]	静音/取消
	返 回 值：BOOL
	=============================================================================*/
    virtual BOOL32 LocalMtQuiet(const s8 *pszConfName, BOOL32 bQuiet);

    /*=============================================================================
	函 数 名：LocalMtMute
	功    能：设置本级终端哑音
	参    数：	pszConfName								[in]	会议名
				bMute								    [in]	哑音/取消
	返 回 值：BOOL
	=============================================================================*/
    virtual BOOL32 LocalMtMute(const s8 *pszConfName, BOOL32 bMute);

    /*=============================================================================
	函 数 名：ChangeCallType
	功    能：更改点名查看画面
	参    数：byCallType					[in]	0:点名人和被点名人合成；
                                                    1:点名人发言, 点名人选看被点名人
                                                    2:被点名人发言
	返 回 值：BOOL
	=============================================================================*/
    virtual BOOL32 ChangeCallType(u8 byCallType);

	//-----------------------------------------------------------------------------
	//	5.外设操作
	//-----------------------------------------------------------------------------

	/*=============================================================================
	函 数 名：StartRec
	功    能：开始录像
	参    数：	pszConfName						[in]	会议名
				tRecCfg							[in]	录像参数
	说    明：终端名为空则会议录像, 否则终端录像
	返 回 值：BOOL
	=============================================================================*/
	virtual BOOL StartRec(const s8 *pszConfName, const TRecorderCfg tRecCfg);

	/*=============================================================================
	函 数 名：PauseRec
	功    能：暂停录像
	参    数：	pszConfName						[in]	会议名
				pszRecName						[in]	录像机名
				pszMtAlias						[in]	为NULL或为空则会议录像, 否则终端录像
	说    明：
	返 回 值：BOOL
	=============================================================================*/
	virtual BOOL PauseRec(const s8 *pszConfName, const s8 *pszRecName, const s8 *pszMtAlias = NULL);

	/*=============================================================================
	函 数 名：ResumeRec
	功    能：恢复录像
	参    数：	pszConfName						[in]	会议名
				pszRecName						[in]	录像机名
				pszMtAlias						[in]	为NULL或为空则会议录像, 否则终端录像
	说    明：必须先暂停录像
	返 回 值：BOOL
	=============================================================================*/
	virtual BOOL ResumeRec(const s8 *pszConfName, const s8 *pszRecName, const s8 *pszMtAlias = NULL);

	/*=============================================================================
	函 数 名：StopRec
	功    能：停止录像
	参    数：	pszConfName						[in]	会议名
				pszRecName						[in]	录像机名
				pszMtAlias						[in]	为NULL或为空则会议录像, 否则终端录像
	说    明：
	返 回 值：BOOL
	=============================================================================*/
	virtual BOOL StopRec(const s8 *pszConfName, const s8 *pszRecName, const s8 *pszMtAlias = NULL);

	/*=============================================================================
	函 数 名：PublishRecFile
	功    能：发布录像文件
	参    数：	pszRecName						[in]	录像机名
				pszFileName						[in]	录像文件名
	说    明：
	返 回 值：BOOL
	=============================================================================*/
	virtual BOOL PublishRecFile(const s8 *pszRecName, const s8 *pszFileName);

	/*=============================================================================
	函 数 名：CancelPubRecFile
	功    能：取消录像文件发布
	参    数：	pszRecName						[in]	录像机名
				pszFileName						[in]	录像文件名
	说    明：
	返 回 值：BOOL
	=============================================================================*/
	virtual BOOL CancelPubRecFile(const s8 *pszRecName, const s8 *pszFileName);

	/*=============================================================================
	函 数 名：DeleteRecFile
	功    能：删除录像文件
	参    数：	pszRecName						[in]	录像机名
				pszFileName						[in]	录像文件名
	说    明：
	返 回 值：BOOL
	=============================================================================*/
	virtual BOOL DeleteRecFile(const s8 *pszRecName, const s8 *pszFileName);

    /*=============================================================================
	函 数 名：StartPlay
	功    能：开始放像
	参    数：	pszConfName						[in]	会议名
				pszRecName						[in]	录像机名
                pszFileName                     [in]    文件名
	说    明：
	返 回 值：BOOL
	=============================================================================*/
    virtual BOOL32 StartPlay(const s8 *pszConfName, const s8 *pszRecName,
                             const s8 *pszFileName);

    /*=============================================================================
	函 数 名：PausePlay
	功    能：暂停放像
	参    数：	pszConfName						[in]	会议名
				pszRecName						[in]	录像机名
	说    明：
	返 回 值：BOOL
	=============================================================================*/
    virtual BOOL32 PausePlay(const s8 *pszConfName, const s8 *pszRecName);

    /*=============================================================================
	函 数 名：ResumePlay
	功    能：恢复放像
	参    数：	pszConfName						[in]	会议名
				pszRecName						[in]	录像机名
	说    明：
	返 回 值：BOOL
	=============================================================================*/
    virtual BOOL32 ResumePlay(const s8 *pszConfName, const s8 *pszRecName);

    /*=============================================================================
	函 数 名：StopPlay
	功    能：停止放像
	参    数：	pszConfName						[in]	会议名
				pszRecName						[in]	录像机名
	说    明：
	返 回 值：BOOL
	=============================================================================*/
    virtual BOOL32 StopPlay(const s8 *pszConfName, const s8 *pszRecName);

    /*=============================================================================
	函 数 名：DragPlay
	功    能：放像拖动
	参    数：	pszConfName						[in]	会议名
				pszRecName						[in]	录像机名
                fProg                           [in]    拖动百分数
	说    明：
	返 回 值：BOOL
	=============================================================================*/
    virtual BOOL32 DragPlay(const s8 *pszConfName, const s8 *pszRecName,
                            float fProg);

	/*=============================================================================
	函 数 名：StartVmp
	功    能：开始画面合成
	参    数：	pszConfName			[in]	会议名
				byMtNum				[in]	参加画面合成的终端个数
				ptVmpMt				[in]	参加画面合成的终端
				byStyle				[in]	VMP_STYLE_DYNAMIC, VMP_STYLE_ONE,
											VMP_STYLE_VTWO, VMP_STYLE_HTWO,
											VMP_STYLE_THREE, VMP_STYLE_FOUR,
											VMP_STYLE_SIX, VMP_STYLE_EIGHT,
											VMP_STYLE_NINE, VMP_STYLE_TEN,
											VMP_STYLE_THIRTEEN, VMP_STYLE_SIXTEEN,
											VMP_STYLE_SPECFOUR, VMP_STYLE_SEVEN
                bySchemaId          [in]    合成方案1-5, 0 不起用
				bBroad				[in]	是否广播
	说    明：
	返 回 值：BOOL
	=============================================================================*/
	virtual BOOL StartVmp(const s8 *pszConfName, u8 byMtNum, const TVmpMt *ptVmpMt, 
                          u8 byStyle, u8 bySchemaId, 
                          BOOL bBroad = TRUE);

	/*=============================================================================
	函 数 名：ChangeVmpMt
	功    能：切换画面合成终端
	参    数：	pszConfName				[in]	会议名
				pszMtAlias				[in]	终端别名, 为NULL或为空则停止该通道的画面合成
				byChannel				[in]	which channel to replace
				byStyle					[in]	VMP_STYLE_DYNAMIC, VMP_STYLE_ONE,
												VMP_STYLE_VTWO, VMP_STYLE_HTWO,
												VMP_STYLE_THREE, VMP_STYLE_FOUR,
												VMP_STYLE_SIX, VMP_STYLE_EIGHT,
												VMP_STYLE_NINE, VMP_STYLE_TEN,
												VMP_STYLE_THIRTEEN, VMP_STYLE_SIXTEEN,
												VMP_STYLE_SPECFOUR, VMP_STYLE_SEVEN
                bySchemaId              [in]    合成方案1-5, 0 不起用
				bBroad					[in]	是否广播
				byMemberType			[in]	跟随方式
												VMP_MEMBERTYPE_MCSSPEC
												VMP_MEMBERTYPE_SPEAKER
												VMP_MEMBERTYPE_CHAIRMAN
												VMP_MEMBERTYPE_POLL
	说    明：
	返 回 值：BOOL
	=============================================================================*/
	virtual BOOL ChangeVmpMt(const s8 *pszConfName, const s8 *pszMtAlias, u8 byChannel, 
                             u8 byStyle, u8 bySchemaId, 
                             BOOL bBroad = TRUE, u8 byMemberType = VMP_MEMBERTYPE_MCSSPEC);

	/*=============================================================================
	函 数 名：StopVmp
	功    能：停止画面合成
	参    数：pszConfName			[in]	会议名
	说    明：
	返 回 值：BOOL
	=============================================================================*/
	virtual BOOL StopVmp(const s8 *pszConfName);

	/*=============================================================================
	函 数 名：StartTvWall
	功    能：开始电视墙
	参    数：	pszConfName				[in]	会议名
				ptMtList				[in]	进行电视墙的终端列表
				byTvNum					[in]	选择哪一个电视墙
	说    明：
	返 回 值：BOOL
	=============================================================================*/
	virtual BOOL StartTvWall(const s8 *pszConfName, const TMtList *ptMtList, u8 byTvNum);

	/*=============================================================================
	函 数 名：ChangeTvWallMt
	功    能：切换电视墙终端
	参    数：	pszConfName			    [in]	会议名
				pszMtAlias				[in]	终端别名, 为NULL或为空则停止该通道的电视墙
				byTvNum					[in]	which tvwall to switch
				byChannel				[in]	which channel to replace
	说    明：
	返 回 值：BOOL
	=============================================================================*/
	virtual BOOL ChangeTvWallMt(const s8 *pszConfName, const s8 *pszMtAlias, u8 byTvNum, u8 byChannel);

    /*=============================================================================
	函 数 名：ChangeTvWallMt
	功    能：切换电视墙跟随方式
	参    数：	pszConfName			    [in]	会议名
                byMemberType            [in]    跟随方式
				byTvNum					[in]	which tvwall to switch
				byChannel				[in]	which channel to replace
	说    明：
	返 回 值：BOOL
	=============================================================================*/
    virtual BOOL32 ChangeTvWallMt(const s8 *pszConfName, u8 byTvNum,
                                  u8 byChannel, u8 byMemberType);

	/*=============================================================================
	函 数 名：StopTvWall
	功    能：停止电视墙
	参    数：	pszConfName				[in]	会议名
				byTvNum					[in]	停止哪一个电视墙
	说    明：
	返 回 值：BOOL
	=============================================================================*/
	virtual BOOL StopTvWall(const s8 *pszConfName, u8 byTvNum);

    // 4.0 增加多画面电视墙操作 2005.10.24
	/*=============================================================================
	函 数 名：StartVmpTw
	功    能：开始多画面电视墙
	参    数：	pszConfName			[in]	会议名
				byMtNum				[in]	参加画面合成的终端个数
				ptVmpTwMt			[in]	参加画面合成的终端
				byStyle				[in]	VMP_STYLE_ONE,
											VMP_STYLE_VTWO, VMP_STYLE_HTWO,
											VMP_STYLE_THREE, VMP_STYLE_FOUR,
											VMP_STYLE_SIX, VMP_STYLE_EIGHT,
											VMP_STYLE_NINE, VMP_STYLE_TEN,
											VMP_STYLE_THIRTEEN, VMP_STYLE_SIXTEEN,
											VMP_STYLE_SPECFOUR, VMP_STYLE_SEVEN
	说    明：
	返 回 值：BOOL32
	=============================================================================*/
	virtual BOOL32 StartVmpTw(const s8 *pszConfName, u8 byMtNum, const TVmpMt *ptVmpTwMt, 
                            u8 byStyle);

	/*=============================================================================
	函 数 名：ChangeVmpTwMt
	功    能：切换多画面电视墙终端
	参    数：	pszConfName				[in]	会议名
				pszMtAlias				[in]	终端别名, 为NULL或为空则停止该通道的画面合成
				byChannel				[in]	要替换的通道
				byStyle					[in]	VMP_STYLE_ONE,
												VMP_STYLE_VTWO, VMP_STYLE_HTWO,
												VMP_STYLE_THREE, VMP_STYLE_FOUR,
												VMP_STYLE_SIX, VMP_STYLE_EIGHT,
												VMP_STYLE_NINE, VMP_STYLE_TEN,
												VMP_STYLE_THIRTEEN, VMP_STYLE_SIXTEEN,
												VMP_STYLE_SPECFOUR, VMP_STYLE_SEVEN
				byMemberType			[in]	跟随方式
												VMP_MEMBERTYPE_MCSSPEC
												VMP_MEMBERTYPE_SPEAKER
												VMP_MEMBERTYPE_CHAIRMAN
												VMP_MEMBERTYPE_POLL
	说    明：
	返 回 值：BOOL32
	=============================================================================*/
	virtual BOOL32 ChangeVmpTwMt(const s8 *pszConfName, const s8 *pszMtAlias, 
                                 u8 byChannel, u8 byStyle, 
                                 u8 byMemberType = VMP_MEMBERTYPE_MCSSPEC);

	/*=============================================================================
	函 数 名：StopVmpTw
	功    能：停止多画面电视墙
	参    数：pszConfName			[in]	会议名
	说    明：
	返 回 值：BOOL32
	=============================================================================*/
	virtual BOOL32 StopVmpTw(const s8 *pszConfName);
    

	/*=============================================================================
	函 数 名：StartMix
	功    能：开始混音
	参    数：	pszConfName				[in]	会议名
				ptMtList				[in]	进行混音的终端列表
	说    明：
	返 回 值：BOOL
	=============================================================================*/
	virtual BOOL StartMix(const s8 *pszConfName, const TMtList *ptMtList);

	/*=============================================================================
	函 数 名：ChangeMixMt
	功    能：切换混音终端
	参    数：	pszConfName					[in]	会议名
				pszMtAlias					[in]	终端别名, 为NULL或为空则停止该通道的混音
				byChannel					[in]	which channel to replace
	说    明：
	返 回 值：BOOL
	=============================================================================*/
	virtual BOOL ChangeMixMt(const s8 *pszConfName, const s8 *pszMtAlias, u8 byChannel);

	/*=============================================================================
	函 数 名：StopMix
	功    能：停止混音
	参    数：pszConfName			[in]	会议名
	说    明：
	返 回 值：BOOL
	=============================================================================*/
	virtual BOOL StopMix(const s8 *pszConfName);

	//-----------------------------------------------------------------------------
	//	6.监控
	//-----------------------------------------------------------------------------

	/*=============================================================================
	函 数 名：StartMonitor
	功    能：开始终端监控
	参    数：	pszConfName			[in]	会议名
				ptMtList			[in]	需要监控的终端列表
				byMonitorPos		[in]	监控位置 0:模拟会场 1:监控窗口
	说    明：从一号通道依次拖动
	返 回 值：BOOL
	=============================================================================*/
	virtual BOOL StartMonitor(const s8 *pszConfName, const TMtList * ptMtList, u8 byMonitorPos);

	/*=============================================================================
	函 数 名：ChangeMonitorMt
	功    能：切换监控终端
	参    数：	pszConfName				[in]	会议名
				pszMtAlias				[in]	终端别名
				byChannel				[in]	which channel to replace
				byMonitorPos			[in]	监控位置 0:模拟会场 1:监控窗口
	说    明：
	返 回 值：BOOL
	=============================================================================*/
	virtual BOOL ChangeMonitorMt(const s8 *pszConfName, const s8 *pszMtAlias, u8 byChannel, u8 byMonitorPos);

	/*=============================================================================
	函 数 名：RealMonitor
	功    能：实时监控
	参    数：	byChannel				[in]	实时监控通道
				byMonitorPos			[in]	监控位置 0:模拟会场 1:监控窗口
	说    明：
	返 回 值：BOOL
	=============================================================================*/
	virtual BOOL RealMonitor(u8 byChannel, u8 byMonitorPos);

	/*=============================================================================
	函 数 名：CancelRealMonitor
	功    能：取消实时监控终端
	参    数：	byChannel				[in]	实时监控通道
				byMonitorPos			[in]	监控位置 0:模拟会场 1:监控窗口
	说    明：
	返 回 值：BOOL
	=============================================================================*/
	virtual BOOL CancelRealMonitor(u8 byChannel, u8 byMonitorPos);

	/*=============================================================================
	函 数 名：QuietMonitor
	功    能：监控静音
	参    数：	byChannel				[in]	监控静音通道
				byMonitorPos			[in]	监控位置 0:模拟会场 1:监控窗口
	说    明：
	返 回 值：BOOL
	=============================================================================*/
	virtual BOOL QuietMonitor(u8 byChannel, u8 byMonitorPos);

	/*=============================================================================
	函 数 名：CancelQuietMonitor
	功    能：取消监控静音
	参    数：	byChannel				[in]	监控静音通道
				byMonitorPos			[in]	监控位置 0:模拟会场 1:监控窗口
	说    明：
	返 回 值：BOOL
	=============================================================================*/
	virtual BOOL CancelQuietMonitor(u8 byChannel, u8 byMonitorPos);

	/*=============================================================================
	函 数 名：MonitorInfo
	功    能：监控信息
	参    数：	byChannel				[in]	监控静音通道
				byMonitorPos			[in]	监控位置 0:模拟会场 1:监控窗口
				pszVideoSrc				[out]	视频源
				pszAudioSrc				[out]	音频源
	说    明：
	返 回 值：BOOL
	=============================================================================*/
	virtual BOOL MonitorInfo(u8 byChannel, u8 byMonitorPos, s8 *pszVideoSrc, s8 *pszAudioSrc);

	/*=============================================================================
	函 数 名：FullScreenMonitor
	功    能：全屏监控
	参    数：	byChannel				[in]	监控静音通道
				byMonitorPos			[in]	监控位置 0:模拟会场 1:监控窗口
	说    明：全屏监控2秒后自动切回原始界面
	返 回 值：BOOL
	=============================================================================*/
	virtual BOOL FullScreenMonitor(u8 byChannel, u8 byMonitorPos);

	/*=============================================================================
	函 数 名：SaveMonitorPic
	功    能：保存监控图片
	参    数：	byChannel				[in]	监控静音通道
				pszPicFullName			[in]	图片保存位置
				byMonitorPos			[in]	监控位置 0:模拟会场 1:监控窗口
	说    明：
	返 回 值：BOOL
	=============================================================================*/
	virtual BOOL SaveMonitorPic(u8 byChannel, const s8 *pszPicFullName, u8 byMonitorPos);

	/*=============================================================================
	函 数 名：MonitorCameraCtrl
	功    能：监控窗口摄像头遥控
	参    数：	byChannel				[in]	监控静音通道
				pszPicFullName			[in]	图片保存位置
				byMonitorPos			[in]	监控位置 0:模拟会场 1:监控窗口
	说    明：仅打开窗口然后关闭, 不做操作
	返 回 值：BOOL
	=============================================================================*/
	virtual BOOL MonitorCameraCtrl(u8 byChannel, u8 byMonitorPos);

	/*=============================================================================
	函 数 名：StopMonitor
	功    能：停止终端监控
	参    数：	byChannel			[in]	停止监控的控件位置
				byMonitorPos		[in]	监控位置 0:模拟会场 1:监控窗口
	说    明：只停止一个。若有多个, 需要多次调用该函数
	返 回 值：BOOL
	=============================================================================*/
	virtual BOOL StopMonitor(u8 byChannel, u8 byMonitorPos);

	//-----------------------------------------------------------------------------
	//	7.终端操作
	//-----------------------------------------------------------------------------

	/*=============================================================================
	函 数 名：MtInspect
	功    能：终端选看
	参    数：	pszConfName						[in]	会议名
				pszSrcMt						[in]	源终端
				pszDesMt						[in]	目的终端
				byInspectType					[in]	选看类型
				byOperationType					[in]	0:弹出选看界面 1:拖拉选看
	说    明：
	返 回 值：BOOL
	=============================================================================*/
	virtual BOOL MtInspect(const s8 *pszConfName, const s8 *pszSrcMt, const s8 *pszDesMt,
							u8 byInspectType = MODE_BOTH, u8 byOperationType = 1);

	/*=============================================================================
	函 数 名：StopInspect
	功    能：停止终端选看
	参    数：	pszConfName							[in]	会议名
				pszInspectMt						[in]	选看终端
	说    明：
	返 回 值：BOOL
	=============================================================================*/
	virtual BOOL StopInspect(const s8 *pszConfName, const s8 *pszInspectMt);

	/*=============================================================================
	函 数 名：MtSendBitrate
	功    能：修改终端发送码率
	参    数：	pszConfName								[in]	会议名
				pszMtAlias								[in]	终端名
				wBitrate								[in]	发送码率
	返 回 值：BOOL
	=============================================================================*/
	virtual BOOL MtSendBitrate(const s8 *pszConfName, const s8 *pszMtAlias, u16 wBitrate);

	/*=============================================================================
	函 数 名：MtQuiet
	功    能：终端静音
	参    数：	pszConfName								[in]	会议名
				pszMtAlias								[in]	终端名
				byType									[in]	0:点击按钮静音
																1:在终端上弹出菜单选择静音
	返 回 值：BOOL
	=============================================================================*/
	virtual BOOL MtQuiet(const s8 *pszConfName, const s8 *pszMtAlias, u8 byType = 0);

	/*=============================================================================
	函 数 名：MtCancelQuiet
	功    能：取消终端静音
	参    数：	pszConfName								[in]	会议名
				pszMtAlias								[in]	终端名
				byType									[in]	0:点击按钮静音
																1:在终端上弹出菜单选择静音
	返 回 值：BOOL
	=============================================================================*/
	virtual BOOL MtCancelQuiet(const s8 *pszConfName, const s8 *pszMtAlias, u8 byType = 0);

	/*=============================================================================
	函 数 名：MtMute
	功    能：终端哑音
	参    数：	pszConfName								[in]	会议名
				pszMtAlias								[in]	终端名
				byType									[in]	0:点击按钮哑音
																1:在终端上弹出菜单选择哑音
	返 回 值：BOOL
	=============================================================================*/
	virtual BOOL MtMute(const s8 *pszConfName, const s8 *pszMtAlias, u8 byType = 0);

	/*=============================================================================
	函 数 名：MtCancelMute
	功    能：取消终端哑音
	参    数：	pszConfName								[in]	会议名
				pszMtAlias								[in]	终端名
				byType									[in]	0:点击按钮哑音
																1:在终端上弹出菜单选择哑音
	返 回 值：BOOL
	=============================================================================*/
	virtual BOOL MtCancelMute(const s8 *pszConfName, const s8 *pszMtAlias, u8 byType = 0);

	/*=============================================================================
	函 数 名：UpdateAlias
	功    能：刷新别名
	参    数：	pszConfName								[in]	会议名
				pszMtAlias								[in]	终端名
	返 回 值：BOOL
	=============================================================================*/
	virtual BOOL UpdateAlias(const s8 *pszConfName, const s8 *pszMtAlias);

	/*=============================================================================
	函 数 名：SaveToAddr
	功    能：保存终端到地址簿
	参    数：	pszConfName						[in]	会议名
				pszMtAlias						[in]	终端名
	返 回 值：BOOL
	=============================================================================*/
	virtual BOOL SaveToAddr(const s8 *pszConfName, const s8 *pszMtAlias);

	/*=============================================================================
	函 数 名：ChangeCallMode
	功    能：改变呼叫方式
	参    数：	pszConfName						[in]	会议名
				pszMtAlias						[in]	终端名
				byCallMode						[in]	CONF_CALLMODE_NONE,
														CONF_CALLMODE_ONCE,
														CONF_CALLMODE_TIMER
	返 回 值：BOOL
	=============================================================================*/
	virtual BOOL ChangeCallMode(const s8 *pszConfName, const s8 *pszMtAlias, u8 byCallMode);

	/*=============================================================================
	函 数 名：ChangeVideoSrc
	功    能：切换视频源
	参    数：	pszConfName								[in]	会议名
				pszMtAlias								[in]	终端名
				byChannel								[in]	通道0: S端子
																1: 视频源1
																2: 视频源2
																3: 视频源3
																4: 视频源4
																5: 视频源5
																6: 视频源6
	返 回 值：BOOL
	=============================================================================*/
	virtual BOOL ChangeVideoSrc(const s8 *pszConfName, const s8 *pszMtAlias, u8 byChannel);

    /*=============================================================================
	函 数 名：ChangeVideoSrc
	功    能：切换视频源
	参    数：	pszConfName								[in]	会议名
				nMtNum								    [in]	终端个数
				ptVideoSrc								[in]	视频源结构
                bSave                                   [in]    是否保存方案
                pszScheName                             [in]    方案名
	返 回 值：BOOL
	=============================================================================*/
    virtual BOOL32 ChangeVideoSrc(const s8 *pszConfName, s32 nMtNum,
                                const TVideoSrc *ptVideoSrc, BOOL32 bSave = FALSE,
                                const s8 *pszScheName = NULL);

    /*=============================================================================
	函 数 名：LoadVideoSrcSche
	功    能：读取视频源方案切换视频源
	参    数：	pszConfName								[in]	会议名
                pszScheName                             [in]    方案名
	返 回 值：BOOL
	=============================================================================*/
    virtual BOOL32 LoadVideosrcSche(const s8 *pszConfName, const s8 *pszScheName);

    /*=============================================================================
	函 数 名：DeleteVideoSrcSche
	功    能：删除视频源方案
	参    数：	pszConfName								[in]	会议名
                pszScheName                             [in]    方案名
	返 回 值：BOOL
	=============================================================================*/
    virtual BOOL32 DeleteVideoSrcSche(const s8 *pszConfName, const s8 *pszScheName);

	/*=============================================================================
	函 数 名：ChangeMatrix
	功    能：更新矩阵
	参    数：	pszConfName							[in]	会议名
				pszMtAlias							[in]	终端名
				nSrcPort							[in]	源端口
				nDesPort							[in]	目的端口
				byMode								[in]	0: 视频, 1: 音频
	返 回 值：BOOL
	=============================================================================*/
	virtual BOOL ChangeMatrix(const s8 *pszConfName, const s8 *pszMtAlias, s32 nSrcPort, s32 nDesPort, u8 byMode = 0);

	/*=============================================================================
	函 数 名：SaveMatrix
	功    能：保存矩阵
	参    数：	pszConfName							[in]	会议名
				pszMtAlias							[in]	终端名
				nMatrixNo							[in]	矩阵方案序号
				pszMatrixName						[in]	方案名
				byMode								[in]	0: 视频, 1: 音频
	返 回 值：BOOL
	=============================================================================*/
	virtual BOOL SaveMatrix(const s8 *pszConfName, const s8 *pszMtAlias, s32 nMatrixNo, const s8 *pszMatrixName, u8 byMode);

	/*=============================================================================
	函 数 名：SetMatrixPortIcon
	功    能：更改矩阵端口图标
	参    数：	pszConfName							[in]	会议名
				pszMtAlias							[in]	终端名
				nPort								[in]	端口
				byMode								[in]	0: 视频输入, 1: 音频输入, 2: 视频输出, 3: 音频输出
				nIcon								[in]	图标号
				pszPortName							[in]	端口名称, NULL则使用默认名称
	返 回 值：BOOL
	=============================================================================*/
	virtual BOOL SetMatrixPortIcon(const s8 *pszConfName, const s8 *pszMtAlias, s32 nPort,
									u8 byMode, s32 nIcon = 0, const s8 *pszPortName = NULL);

    /*=============================================================================
	函 数 名：RefreshMcu
	功    能：刷新MCU
	参    数：	pszConfName							[in]	会议名
				pszMcuAlias							[in]	MCU名
	返 回 值：BOOL
	=============================================================================*/
    virtual BOOL RefreshMcu(const s8 *pszConfName, const s8 *pszMcuAlias);

    //-----------------------------------------------------------------------------
	//	8.摄像头操作
	//-----------------------------------------------------------------------------

	/*=============================================================================
	函 数 名：CameraUp
	功    能：摄像头向上
	参    数：	pszConfName							[in]	会议名
				pszMtAlias							[in]	终端名
	返 回 值：BOOL
	=============================================================================*/
	virtual BOOL CameraUp(const s8 *pszConfName, const s8 *pszMtAlias);

	/*=============================================================================
	函 数 名：CameraDown
	功    能：摄像头向下
	参    数：	pszConfName								[in]	会议名
				pszMtAlias								[in]	终端名
	返 回 值：BOOL
	=============================================================================*/
	virtual BOOL CameraDown(const s8 *pszConfName, const s8 *pszMtAlias);

	/*=============================================================================
	函 数 名：CameraLeft
	功    能：摄像头向左
	参    数：	pszConfName								[in]	会议名
				pszMtAlias								[in]	终端名
	返 回 值：BOOL
	=============================================================================*/
	virtual BOOL CameraLeft(const s8 *pszConfName, const s8 *pszMtAlias);

	/*=============================================================================
	函 数 名：CameraRight
	功    能：摄像头向右
	参    数：	pszConfName								[in]	会议名
				pszMtAlias								[in]	终端名
	返 回 值：BOOL
	=============================================================================*/
	virtual BOOL CameraRight(const s8 *pszConfName, const s8 *pszMtAlias);

	/*=============================================================================
	函 数 名：CameraAutoFocus
	功    能：摄像头自动调焦
	参    数：	pszConfName								[in]	会议名
				pszMtAlias								[in]	终端名
	返 回 值：BOOL
	=============================================================================*/
	virtual BOOL CameraAutoFocus(const s8 *pszConfName, const s8 *pszMtAlias);

	/*=============================================================================
	函 数 名：CameraFocus
	功    能：摄像头调焦
	参    数：	pszConfName						[in]	会议名
				pszMtAlias						[in]	终端名
				bIncrease						[in]	TRUE: increase, FALSE decrease
	返 回 值：BOOL
	=============================================================================*/
	virtual BOOL CameraFocus(const s8 *pszConfName, const s8 *pszMtAlias, BOOL bIncrease);

	/*=============================================================================
	函 数 名：CameraZoom
	功    能：摄像头调视野
	参    数：	pszConfName						[in]	会议名
				pszMtAlias						[in]	终端名
				bIncrease						[in]	TRUE: increase, FALSE decrease
	返 回 值：BOOL
	=============================================================================*/
	virtual BOOL CameraZoom(const s8 *pszConfName, const s8 *pszMtAlias, BOOL bIncrease);

	/*=============================================================================
	函 数 名：CameraBright
	功    能：摄像头调亮度
	参    数：	pszConfName						[in]	会议名
				pszMtAlias						[in]	终端名
				bIncrease						[in]	TRUE: increase, FALSE decrease
	返 回 值：BOOL
	=============================================================================*/
	virtual BOOL CameraBright(const s8 *pszConfName, const s8 *pszMtAlias, BOOL bIncrease);

	/*=============================================================================
	函 数 名：CameraSavePos
	功    能：摄像头位置保存
	参    数：	pszConfName						[in]	会议名
				pszMtAlias						[in]	终端名
				byChannel						[in]	which position to save the file
	返 回 值：BOOL
	=============================================================================*/
	virtual BOOL CameraSavePos(const s8 *pszConfName, const s8 *pszMtAlias, u8 byChannel);

	/*=============================================================================
	函 数 名：CameraLoadPos
	功    能：摄像头读取预置位置
	参    数：	pszConfName						[in]	会议名
				pszMtAlias						[in]	终端名
				byChannel						[in]	which position to load the file
	返 回 值：BOOL
	=============================================================================*/
	virtual BOOL CameraLoadPos(const s8 *pszConfName, const s8 *pszMtAlias, u8 byChannel);

	//-----------------------------------------------------------------------------
	//	9.用户操作
	//-----------------------------------------------------------------------------

	/*=============================================================================
	函 数 名：AddUser
	功    能：添加用户
	参    数：	pszUserName						[in]	用户名
				pszPwd							[in]	密码
				pszFullName						[in]	用户全名
				byUserType						[in]	用户类型 UM_ADMIN, UM_OPERATOR
				pszDescription					[in]	用户描述
	说    明：
	返 回 值：BOOL
	=============================================================================*/
	virtual BOOL AddUser(const s8 *pszUserName, const s8 *pszPwd, const s8 *pszFullName,
						u8 byUserType = UM_ADMIN, const s8 *pszDescription = NULL);

	/*=============================================================================
	函 数 名：ModifyUser
	功    能：修改用户
	参    数：	pszUserName						[in]	用户名
				pszPwd							[in]	修改密码
				pszFullName						[in]	修改用户全名
				byUserType						[in]	修改用户类型 UM_ADMIN, UM_OPERATOR
				pszDescription					[in]	修改用户描述
	说    明：
	返 回 值：BOOL
	=============================================================================*/
	virtual BOOL ModifyUser(const s8 *pszUserName, const s8 *pszPwd, const s8 *pszFullName,
							u8 byUserType = UM_ADMIN, const s8 *pszDescription = NULL);

	/*=============================================================================
	函 数 名：DelUser
	功    能：删除用户
	参    数：pszUserName						[in]	用户名
	说    明：
	返 回 值：BOOL
	=============================================================================*/
	virtual BOOL DelUser(const s8 *pszUserName);

	/*=============================================================================
	函 数 名：EmptyUser
	功    能：删除除Admin之外所有用户
	参    数：无
	说    明：
	返 回 值：无
	=============================================================================*/
	virtual void EmptyUser();

	//-----------------------------------------------------------------------------
	//	10.地址簿操作
	//-----------------------------------------------------------------------------

	/*=============================================================================
	函 数 名：AddMtToAddrbook
	功    能：增加地址簿终端
	参    数：ptAddrEntryInfo						[in]	加入终端信息
	说    明：
	返 回 值：BOOL
	=============================================================================*/
	virtual BOOL AddMtToAddrbook(const TADDRENTRYINFO *ptAddrEntryInfo);

	/*=============================================================================
	函 数 名：DelMtInAddrbook
	功    能：从地址簿删除终端
	参    数：ptAddrEntryInfo						[in]	删除终端
	说    明：
	返 回 值：BOOL
	=============================================================================*/
	virtual BOOL DelMtInAddrbook(const TADDRENTRYINFO *ptAddrEntryInfo);

	/*=============================================================================
	函 数 名：ModifyMtInAddrbook
	功    能：修改地址簿终端
	参    数：	ptSrcInfo							[in]	终端原始信息
				ptModifyInfo						[in]	修改信息
	说    明：
	返 回 值：BOOL
	=============================================================================*/
    virtual BOOL ModifyMtInAddrbook(const TADDRENTRYINFO *ptSrcInfo,
                                const TADDRENTRYINFO *ptModifyInfo);

	/*=============================================================================
	函 数 名：AddGroup
	功    能：添加会议组
	参    数：	ptAddrGroup						[in]	会议组信息
				ptEntryTable					[in]	需要加入会议组的条目
				nEntryNum						[in]	条目个数
	说    明：	1. ptEntryTable == NULL 或 nEntryNum == 0, 则不向会议组添加条目。
				2. 条目必须已存在于地址簿中
	返 回 值：BOOL
	=============================================================================*/
	virtual BOOL AddGroup(const TMULTISETENTRYINFO *ptAddrGroup, const TADDRENTRYINFO *ptEntryTable = NULL, s32 nEntryNum = 0);

	/*=============================================================================
	函 数 名：DelGroup
	功    能：删除会议组
	参    数：pszGroupName						[in]	会议组名称
	说    明：
	返 回 值：BOOL
	=============================================================================*/
	virtual BOOL DelGroup(const s8 *pszGroupName);

	/*=============================================================================
	函 数 名：ModifyGroup
	功    能：修改会议组
	参    数：	pszGroupName					[in]	需要修改的会议组名称
				ptAddrGroup						[in]	修改内容
	说    明：
	返 回 值：BOOL
	=============================================================================*/
	virtual BOOL ModifyGroup(const s8 *pszGroupName, const TMULTISETENTRYINFO *ptAddrGroup);

	/*=============================================================================
	函 数 名：GroupAddEntry
	功    能：向会议组添加条目
	参    数：	pszGroupName					[in]	会议组名称
				ptEntryTable					[in]	需要加入会议组的条目
				nEntryNum						[in]	条目个数
	说    明：条目必须已存在于地址簿中
	返 回 值：BOOL
	=============================================================================*/
	virtual BOOL GroupAddEntry(const s8 *pszGroupName, const TADDRENTRYINFO *ptEntryTable, s32 nEntryNum);

	/*=============================================================================
	函 数 名：GroupDelEntry
	功    能：从会议组中删除条目
	参    数：	pszGroupName					[in]	会议组名称
				ptEntryTable					[in]	需要删除会议组的条目
				nEntryNum						[in]	条目个数
	说    明：
	返 回 值：BOOL
	=============================================================================*/
	virtual BOOL GroupDelEntry(const s8 *pszGroupName, const TADDRENTRYINFO *ptEntryTable, s32 nEntryNum);

	/*=============================================================================
	函 数 名：EmptyAddrEntry
	功    能：删除所有地址簿条目
	参    数：无
	说    明：
	返 回 值：无
	=============================================================================*/
	virtual void EmptyAddrEntry();

	/*=============================================================================
	函 数 名：EmptyAddrGroup
	功    能：删除所有会议组
	参    数：无
	说    明：
	返 回 值：无
	=============================================================================*/
	virtual void EmptyAddrGroup();


    //-----------------------------------------------------------------------------
	//	11.MCU配置操作
	//-----------------------------------------------------------------------------

    /*============================================================================
    函 数 名：McuWizardCfg
    功    能：Mcu配置向导设置
    参    数：ptMcuWizardCfg       			[in]	传入的配置参数
    说    明：
    返 回 值：BOOL32
    ============================================================================*/
    virtual BOOL32 McuWizardCfg (const tagMcuWizardSetting *ptMcuWizardSetting);    

    /*============================================================================
    函 数 名：McuExportConfig
    功    能：导出Mcu配置
    参    数：pszFilename  					[in]	保存的目标文件名
    说    明：
    返 回 值：BOOL32
    ============================================================================*/
    virtual BOOL32 McuExportConfig (const s8 * pszFilename);

    /*============================================================================
    函 数 名：McuImportConfig
    功    能：导入Mcu配置
    参    数：pszFilename  					[in]	配置文件名
    说    明：
    返 回 值：BOOL
    ============================================================================*/
    virtual BOOL32 McuImportConfig (const s8 * pszFilename);


    /*============================================================================
    函 数 名：RefreshMcuStatus
    功    能：刷新Mcu信息
    参    数：无
    说    明：
    返 回 值：BOOL32
    ============================================================================*/
    virtual BOOL32 RefreshMcuStatus ();

    /*============================================================================
    函 数 名：SynMcuTime
    功    能：同步Mcu时间成本地时间
    参    数：无
    说    明：
    返 回 值：BOOL32
    ============================================================================*/
    virtual BOOL32 SynMcuTime ();

    /*============================================================================
    函 数 名：RebootMcu
    功    能：重启Mcu
    参    数：bConfirm                      [in] 是否真要重启,TRUE 表示确认
    说    明：
    返 回 值：BOOL32
    ============================================================================*/
    virtual BOOL32 RebootMcu (BOOL bConfirm = TRUE);


    /*============================================================================
    函 数 名：McuNetworkSet
    功    能：进行 Mcu 网络配置，并保存
    参    数：ptNetworkSetting       			[in]	网络配置的参数
    说    明：
    返 回 值：BOOL32
    ============================================================================*/
    virtual BOOL32 McuNetworkSet (const tagMcuNetworkSetting *ptNetworkSetting);


    /*============================================================================
    函 数 名：AddMcuSnmp
    功    能：添加SNMP服务器
    参    数：ptSnmpSetting 				[in]	SNMP 服务器设置
    说    明：
    返 回 值：BOOL32
    ============================================================================*/
    virtual BOOL32 AddMcuSnmp (const tagMcuSnmpSetting *ptSnmpSetting);

    /*============================================================================
    函 数 名：DelMcuSnmp
    功    能：删除SNMP服务器
    参    数：dwIP       					[in]	SNMP 的 IP 地址
    说    明：
    返 回 值：BOOL32
    ============================================================================*/
    virtual BOOL32 DelMcuSnmp (u32 dwIP);

    /*============================================================================
    函 数 名：EditMcuSnmp
    功    能：编辑SNMP服务器
    参    数：dwOldIP     					[in]	要编辑的SNMP 旧 IP
              ptSnmpSetting					[in]	新的SNMP 的配置信息
    说    明：
    返 回 值：BOOL32
    ============================================================================*/
    virtual BOOL32 EditMcuSnmp (u32 dwOldIP, const tagMcuSnmpSetting *ptSnmpSetting);
    

    /*============================================================================
    函 数 名：McuDeviceQos
    功    能：外设QoS配置
    参    数：tagMcuQosSetting              [in]    QoS参数

    说    明：等级数值按照规格书要求的范围填写
    返 回 值：BOOL32
    ============================================================================*/
    virtual BOOL32 McuDeviceQos (const tagMcuQosSetting *ptQosSetting);

    /*============================================================================
    函 数 名：McuNetMisc
    功    能：Mcu网络杂项配置
    参    数：tagMcuNetMiscSetting              [in]    Mcu网络杂项配置参数

    说    明：
    返 回 值：BOOL32
    ============================================================================*/
    virtual BOOL32 McuNetMisc (const tagMcuNetMiscSetting *ptNetMiscSetting);


    /*============================================================================
    函 数 名：McuVmpConfig
    功    能：配置画面合成
    参    数：ptVmpCfg              [in]    画面合成配置参数
    说    明：
    返 回 值：BOOL32
    ============================================================================*/
    virtual BOOL32 McuVmpConfig (const TVmpStyleCfg* ptVmpCfg);


    /*============================================================================
    函 数 名：McuChipAdd
    功    能：单板配置-增加
    参    数：ptMcuChipCfg                  [in]    增加的单板参数
    说    明：
    返 回 值：BOOL32
    ============================================================================*/
    virtual BOOL32 McuChipAdd (const TMcuChipCfg* ptMcuChipCfg);

    /*============================================================================
    函 数 名：McuChipModuleEdit
    功    能：单板配置-增加模块
    参    数：byLayer                       [in]    要配置的板所在层号 
              bySlot                        [in]    要配置的板所在槽号
              ptModuleCfg                   [in]    新的模块参数
    说    明：
    返 回 值：BOOL32
    ============================================================================*/
    virtual BOOL32 McuChipModuleAdd (u8 byLayer, u8 bySlot,
                                     const TMcuChipModuleCfg *ptModuleCfg);

    /*============================================================================
    函 数 名：McuRecAdd
    功    能：录像机增加
    参    数：ptRecCfg                       [in]    增加的录像机参数
    说    明：
    返 回 值：BOOL32
    ============================================================================*/
    virtual BOOL32 McuRecAdd (const TMcuChipModuleCfg* ptRecCfg);

    /*============================================================================
    函 数 名：McuChipDel
    功    能：单板配置-删除
    参    数：byLayer                       [in]    要删除的板所在层号 
              bySlot                        [in]    要删除的板所在槽号
              bConfirm                      [in]    TRUE表示确认删除
    说    明：
    返 回 值：BOOL32
    ============================================================================*/
    virtual BOOL32 McuChipDel (u8 byLayer, u8 bySlot, BOOL32 bConfirm = TRUE);

    /*============================================================================
    函 数 名：McuChipModuleDel
    功    能：单板配置-删除模块
    参    数：byLayer                       [in]    要删除的模块所在单板板所在层号 
              bySlot                        [in]    要删除的模块所在单板所在槽号
              cstrOldName                   [in]    该模块的别名
              bConfirm                      [in]    TRUE表示确认删除
    说    明：
    返 回 值：BOOL32
    ============================================================================*/
    virtual BOOL32 McuChipModuleDel (u8 byLayer, u8 bySlot, CString cstrOldName,
                                     BOOL32 bConfirm = TRUE);

    /*============================================================================
    函 数 名：McuRecDel
    功    能：单板配置-删除录像机
    参    数：dwIp                          [in]    录像机的IP地址
              bConfirm                      [in]    TRUE表示确认删除
    说    明：
    返 回 值：BOOL32
    ============================================================================*/
    virtual BOOL32 McuRecDel (u32 dwIp, BOOL32 bConfirm = TRUE);

    /*============================================================================
    函 数 名：McuChipEdit
    功    能：单板配置-修改
    参    数：ptMcuChipCfg                  [in]    新的单板参数
    说    明：单板的层槽号不能修改，单板类型不能修改，
              模块不进行修改(相关成员tModuleCfg不需要赋值)。
              录像机不能此方法修改，
    返 回 值：BOOL32
    ============================================================================*/
    virtual BOOL32 McuChipEdit (const TMcuChipCfg *ptMcuChipCfg);

    /*============================================================================
    函 数 名：McuChipReboot
    功    能：单板配置-重启
    参    数：ptMcuChipCfg                  [in]    单板参数,只需要填写层号和槽号
              bConfirm                      [in]    是否确认
    说    明：
    返 回 值：BOOL32
    ============================================================================*/
    virtual BOOL32 McuChipReboot (const TMcuChipCfg *ptMcuChipCfg, BOOL32 bConfirm = TRUE);

    /*============================================================================
    函 数 名：McuChipModuleEdit
    功    能：单板配置-修改模块
    参    数：byLayer                       [in]    要配置的板所在层号 
              bySlot                        [in]    要配置的板所在槽号
              cstrOldName                   [in]    该模块原来的别名
              ptModuleCfg                   [in]    新的模块参数
    说    明：模块类型不能修改，录像机不能用此方法修改
    返 回 值：BOOL32
    ============================================================================*/
    virtual BOOL32 McuChipModuleEdit (u8 byLayer, u8 bySlot, CString cstrOldName, 
                                      const TMcuChipModuleCfg *ptModuleCfg);

    /*============================================================================
    函 数 名：McuRecEdit
    功    能：单板配置-修改录像机模块
    参    数：dwIp                          [in]    要配置录像机原来的IP
              ptModuleCfg                   [in]    新的录像机参数
    说    明：录像机用此方法修改
    返 回 值：BOOL32
    ============================================================================*/
    virtual BOOL32 McuRecEdit (u32 dwIp, const TMcuChipModuleCfg *ptModuleCfg);

    /*============================================================================
    函 数 名：McuSaveAllCfg
    功    能：保存所有MCU配置
    参    数：bConfirm                      [in]    是否确定，TRUE表示确定
    说    明：
    返 回 值：BOOL32
    ============================================================================*/
    virtual BOOL32 McuSaveAllCfg (BOOL32 bConfirm = TRUE);
    
    //-----------------------------------------------------------------------------
	//	12.用户设置操作
	//-----------------------------------------------------------------------------


    /*=============================================================================
	函 数 名：SetCallingType
	功    能：设置点名选看画面
	参    数：byType                        [in]    0:点名人与被点名人画面合成
                                                    1:点名人看被点名人, 其他看点名人
                                                    2:被点名人看点名人, 其他看被点名人
	返 回 值：BOOL32
	=============================================================================*/
    virtual BOOL32 SetCallingType(u8 byType);


    /*=============================================================================
	函 数 名：SetLanguage
	功    能：设置语言
	参    数：byIndex                       [in]    LANG_CHS, LANG_EN
	返 回 值：BOOL32
	=============================================================================*/
    virtual BOOL32 SetLanguage(u8 byIndex);

    /*=============================================================================
	函 数 名：DisablePersonMenu
	功    能：取消用户自定义菜单，一般在测试模拟会场功能前都建议调用，
              否则会导致菜单选择因为用户自定义而失败
	参    数：无
	返 回 值：BOOL32
	=============================================================================*/
    virtual BOOL32 DisablePersonMenu();

    //-----------------------------------------------------------------------------
	//	13.终端视频源轮询操作
	//-----------------------------------------------------------------------------

    /*=============================================================================
	函 数 名：StartVideoSrcPoll
	功    能：开始视频源轮询
	参    数：pszConfName                       [in]    会议名
              nMtNum                            [in]    终端个数
              ptVideoSrcPoll                    [in]    终端信息
              bSaveToScheme                     [in]    是否保存为方案
              pszSchemeName                     [in]    方案名
    说    明：该函数不会退出轮询界面
	返 回 值：BOOL
	=============================================================================*/
    virtual BOOL32 StartVideoSrcPoll(const s8 *pszConfName, s32 nMtNum,
                                     TVideoSrcPoll *ptVideoSrcPoll,
                                     BOOL32 bSaveToScheme = FALSE,
                                     const s8 *pszSchemeName = NULL);

    /*=============================================================================
	函 数 名：StartVideoSrcPoll
	功    能：开始视频源轮询
	参    数：pszConfName                       [in]    会议名
              pszSchemeName                     [in]    方案名
    说    明：该函数不会退出轮询界面
	返 回 值：BOOL
	=============================================================================*/
    virtual BOOL32 StartVideoSrcPoll(const s8 *pszConfName,
                                     const s8 *pszSchemeName);

    /*=============================================================================
	函 数 名：DeleteVideoSrcPollScheme
	功    能：删除视频源轮询方案
	参    数：pszSchemeName                     [in]    方案名
    说    明：该函数不会退出轮询界面
	返 回 值：BOOL
	=============================================================================*/
    virtual BOOL32 DeleteVideoSrcPollScheme(const s8 *pszSchemeName);

    /*=============================================================================
	函 数 名：PauseVideoSrcPoll
	功    能：暂停视频源轮询
	参    数：pszMtAlias                        [in]    终端名, 为空则暂停整个轮询
    说    明：该函数不会退出轮询界面
	返 回 值：BOOL
	=============================================================================*/
    virtual BOOL32 PauseVideoSrcPoll(const s8 *pszMtAlias = NULL);

    /*=============================================================================
	函 数 名：ResumeVideoSrcPoll
	功    能：恢复视频源轮询
	参    数：pszMtAlias                        [in]    终端名, 为空则恢复整个轮询
    说    明：该函数不会退出轮询界面
              必须先暂停轮询
	返 回 值：BOOL
	=============================================================================*/
    virtual BOOL32 ResumeVideoSrcPoll(const s8 *pszMtAlias = NULL);

    /*=============================================================================
	函 数 名：StopVideoSrcPoll
	功    能：停止视频源轮询
	参    数：pszMtAlias                        [in]    终端名, 为空则停止整个轮询
    说    明：该函数不会退出轮询界面
	返 回 值：BOOL
	=============================================================================*/
    virtual BOOL32 StopVideoSrcPoll(const s8 *pszMtAlias = NULL);

    /*=============================================================================
	函 数 名：RestartVideoSrcPoll
	功    能：重新开始视频源轮询
	参    数：pszMtAlias                        [in]    终端名, 为空则重新开始整个轮询
    说    明：该函数不会退出轮询界面
	返 回 值：BOOL
	=============================================================================*/
    virtual BOOL32 RestartVideoSrcPoll(const s8 *pszMtAlias = NULL);

    /*=============================================================================
	函 数 名：QuitVideoSrcPollDlg
	功    能：退出视频源轮询界面
	参    数：无
	返 回 值：BOOL
	=============================================================================*/
    virtual BOOL32 QuitVideoSrcPollDlg();

public:
	//-----------------------------------------------------------------------------
	//	查询
	//-----------------------------------------------------------------------------


	/*=============================================================================
	函 数 名：GetUserName
	功    能：查询用户列表某一项的用户名
	参    数：	nItemCount							[in]	项序号
				aszUserName							[out]	用户名
	返 回 值：BOOL
	=============================================================================*/
	virtual BOOL GetUserName(s32 nItemCount, s8 aszUserName[32]);


    /*=============================================================================
	函 数 名：GetConfIDByName
	功    能：查询会议Id
	参    数：  pszConfName                         [in]   要查询会议名
	            cConfId                             [out]  会议Id 
	返 回 值：BOOL
	=============================================================================*/
	virtual BOOL GetConfIDByName(const s8 *pszConfName, CConfId &cConfId);

protected:
	//-----------------------------------------------------------------------------
	//	内部函数
	//-----------------------------------------------------------------------------

	//-----------------------------------------------------------------------------
	//	1.封装CGuiUnit函数
	//-----------------------------------------------------------------------------

	/*=============================================================================
	函 数 名： MoveBtnClick
	功    能： 模拟鼠标移动到按钮上单击 
	参    数： HWND hWnd					[in] 按钮句柄
	返 回 值： void
	=============================================================================*/
	void MoveBtnClick(HWND hWnd);

	/*=============================================================================
	函 数 名： MoveTextInput
	功    能： 模拟鼠标移动到Edit控件上输入文本 
	参    数：	CString strData				[in] 输入的文本
				HWND hWnd					[in] Edit控件句柄
	返 回 值： void
	=============================================================================*/
	void MoveTextInput(CString strData, HWND hWnd);	


    /*=============================================================================
	函 数 名：MoveBtnCheck
	功    能：采用点击方式选中/取消选中Checkbox
	参    数：  hCk                            [in]    Checkbox 句柄
                bCheck                         [in]    是否选中
	说    明：
	返 回 值：void
	=============================================================================*/
    void MoveBtnCheck(HWND hCk, BOOL32 bCheck);

	/*=============================================================================
	函 数 名： CbEditChange
	功    能： edit控件内容发生变化时发送消息通知(因为GuiUnit库中没有发送此消息)
	参    数：	UINT uCtrlId				[in]控件id
				UINT uMsg					[in]要发送的消息id
				HWND hCtrl					[in]控件句柄
				HWND hParent				[in]控件所在父窗口句柄
	返 回 值：	BOOL
	=============================================================================*/
	BOOL EditChange(UINT uCtrlId, UINT uMsg, HWND hCtrl, HWND hParent);

	/*=============================================================================
	函 数 名： MoveListCtrlClick
	功    能： 选择listctrl控件中的某一项
	参    数：	HWND hCtrl					[in]listctrl控件句柄
				s32  nItem					[in]选择项
	返 回 值：BOOL
	=============================================================================*/
	BOOL MoveListCtrlClick(HWND hWnd, s32 nItem);

	/*=============================================================================
	函 数 名：ListCtrlSelect
	功    能：在ListCtrl中选择某一项
	参    数：	HWND hWnd						[in]	listctrl控件句柄
				s32  nItem						[in]	选择项
	返 回 值：无
	=============================================================================*/
	void ListCtrlSelect(HWND hWnd, s32 nItem);

	/*=============================================================================
	函 数 名：SelectMcuChip
	功    能：在Mcu单板列表中中选择某一个层/槽
	参    数：	HWND hTabWnd					[in]	tab控件句柄
                HWND hListWnd					[in]	listctrl控件句柄
				u8  byLayer						[in]	层号
                u8  bySlot                      [in]    槽号
                u8  byType                      [in]    板类型
	返 回 值：BOOL32
	=============================================================================*/
    BOOL32 SelectMcuChip(HWND hTabWnd, HWND hListWnd, u8 byLayer, u8 bySlot, u8 byType);

	//-----------------------------------------------------------------------------
	//	2.会议创建操作
	//-----------------------------------------------------------------------------


    // 本函数4.0废弃
	/*=============================================================================
	函 数 名：CreateNormalConf
	功    能：创建正常会议
	参    数：	ptConfInfo							[in]	会议创建参数
				wMtNum								[in]	终端个数
				ptAddMtCfg							[in]	终端参数
				ptVmpModule							[in]	画面合成参数
				ptTvModule							[in]	电视墙参数
	说    明：
	返 回 值：BOOL
	=============================================================================
	virtual BOOL CreateNormalConf(const TConfInfo *ptConfInfo, u16 wMtNum,
                                  const TAddMtCfg *ptAddMtCfg,
                                  const TVmpModule *ptVmpModule = NULL,
                                  const TTvWallModule *ptTvModule = NULL);         */

	/*=============================================================================
	函 数 名：AddMtWhenCreateConf
	功    能：添加会议时向终端列表里写入终端
	参    数：	ptConfInfo                          [in]    会议信息
                wMtNum								[in]	普通终端个数
				ptAddMtCfg							[in]	普通终端参数
                wH320MtNum                          [in]	H320终端个数
                ptAddH320MtCfg                      [in]	H320终端参数
                hCallTypeBtn                        [in]    呼叫类型按钮
                hMtList                             [in]    终端列表
				hAliasEt							[in]	别名输入框
                hAddToAddrCk                        [in]    添加到地址簿
				hBtnAdd								[in]	添加按钮
                hBtnAddH320                         [in]    添加H320按钮 
	返 回 值：BOOL
	=============================================================================*/
	BOOL AddMtWhenCreateConf(const TConfInfo *ptConfInfo,
                             u16 wMtNum, const TAddMtCfg *ptAddMtCfg,
                             u16 wH320MtNum, const TAddH320MtCfg *ptAddH320MtCfg,
                             HWND hCallTypeBtn,  HWND hMtList, 
                             HWND hAliasEt, HWND hAddToAddrCk,
                             HWND hBtnAdd, HWND hBtnAddH320);

	/*=============================================================================
	函 数 名：SetMediaType
	功    能：设置媒体类型
	参    数：	hVideo								[in]	视频类型句柄
				hAudio								[in]	音频类型句柄
				byVideoType							[in]	视频类型
				byAudioType							[in]	音频类型
	返 回 值：BOOL
	=============================================================================*/
	BOOL SetMediaType(HWND hVideo, HWND hAudio, u8 byVideoType, u8 byAudioType);

	/*=============================================================================
	函 数 名：SetVideoFormat
	功    能：设置视频格式
	参    数：	hVideo								[in]	视频格式句柄
				byVideoFormat						[in]	视频格式
	返 回 值：BOOL
	=============================================================================*/
	BOOL SetVideoFormat(HWND hVideo, u8 byVideoFormat);

    /*=============================================================================
    函 数 名：SelectFrameRate
    功    能：设置视频整率
    参    数：	hVideoFr							[in]	视频帧率Combobox句柄
			    byVideoFr   						[in]	视频帧率

    说    明：内部使用
    返 回 值：BOOL32
    -----------------------------------------------------------------------------
    修改记录：
    日		期	版本	修改人	走读人	修改内容
    2005/12/27	4.0		顾振华			创建
    =============================================================================*/
    BOOL32 SelectFrameRate(HWND hVideoFr, u8 byVideoFr);

	//-----------------------------------------------------------------------------
	//	3.会议操作
	//-----------------------------------------------------------------------------

	/*=============================================================================
	函 数 名：ClickConfSettingBtn
	功    能：点击 会议设定 按钮, 并在弹出菜单里做选择
	参    数：	nMenuItem							[in]	选择菜单项
				uMenuState							[in]	预期菜单是否被CHECK
	返 回 值：BOOL
	=============================================================================*/
	BOOL ClickConfSettingBtn(s32 nMenuItem, UINT uMenuState = -1);

	/*=============================================================================
	函 数 名：SetVmpStyle
	功    能：选择画面合成模式
	参    数：	hStyleCb						[in]	画面合成模式下拉框句柄
				byStyle							[in]	画面合成模式
                bVmpTw                          [in]    TRUE 表示是多画面电视墙
                bMcu8000B                       [in]    TRUE 表示是8000B的MCU
	返 回 值：BOOL
	=============================================================================*/
	BOOL SetVmpStyle(HWND hStyleCb, u8 byStyle, BOOL32 bVmpTw = FALSE, 
                     BOOL32 bMcu8000B = FALSE);

	/*=============================================================================
	函 数 名：OpenRecDlg
	功    能：打开录像对话框
	参    数：	pszConfName						[in]	会议名
				pszMtAlias						[in]	终端名, 为NULL或空则会议录像
				bChecked						[in]	菜单是否需要被CHECK过
	返 回 值：BOOL
	=============================================================================*/
	BOOL OpenRecDlg(const s8 *pszConfName, const s8 *pszMtAlias, BOOL bChecked);

	/*=============================================================================
	函 数 名：Spec
	功    能：指定 主席/发言人
	参    数：	pszConfName						[in]	会议名
				pszMtAlias						[in]	终端别名
				byType							[in]	0:在终端上点击右键选择指定;
														1:拖动终端到主席台指定
				nIndex							[in]	菜单项位置
	说    明：
	返 回 值：无
	=============================================================================*/
	BOOL Spec(const s8 *pszConfName, const s8 *pszMtAlias, u8 byType, s32 nIndex);

	/*=============================================================================
	函 数 名：Cancel
	功    能：取消 主席/发言
	参    数：	pszConfName						[in]	会议名
				byType							[in]	0:在终端上点击右键选择取消;
														1:从到主席台拖动终端取消
				nIndex							[in]	菜单项位置
	说    明：
	返 回 值：无
	=============================================================================*/
	BOOL Cancel(const s8 *pszConfName, u8 byType, s32 nIndex);

	/*=============================================================================
	函 数 名：AddMtFromAddr
	功    能：从地址簿增加终端
	参    数：	ptAddMtCfg							[in]	要添加的终端
				nMtNum								[in]	终端个数
	说    明：
	返 回 值：BOOL
	=============================================================================*/
	BOOL AddMtFromAddr(const TAddMtCfg *ptMtCfg, s32 nMtNum);

    /*=============================================================================
	函 数 名：SaveScheme
	功    能：保存方案
	参    数：pszSchemeName                         [in]	方案名
	说    明：
	返 回 值：BOOL32
	=============================================================================*/
    BOOL32 SaveScheme(const s8 *pszSchemeName);

    /*=============================================================================
	函 数 名：McuChipModuleCfg
	功    能：进行MCU单板的模块配置，内部使用
	参    数：ptModuleCfg                           [in]    模块参数
	说    明：
	返 回 值：BOOL32
	=============================================================================*/
    BOOL32 McuChipModuleCfg(const TMcuChipModuleCfg *ptModuleCfg);
    
    
	//-----------------------------------------------------------------------------
	//	4.弹出界面句柄初始化
	//-----------------------------------------------------------------------------

	/*=============================================================================
	函 数 名：InitSimMonitorHandle
	功    能：初始化模拟会场监控部分句柄
	参    数：	hLeftBtn						[out]	向左按钮句柄
				hRightBtn						[out]	向右按钮句柄
				ahMonitorStatic					[out]	监控窗口句柄
	返 回 值：BOOL
	=============================================================================*/
	BOOL InitSimMonitorHandle(HWND &hLeftBtn, HWND &hRightBtn, HWND ahMonitorStatic[6]);

	/*=============================================================================
	函 数 名：InitMonitroHandle
	功    能：初始化监控窗口句柄
	参    数：	hConfCb							[out]	会议选择框句柄
				hMtList							[out]	终端列表句柄
				ahModeBtn						[out]	模式选择句柄
				ahMonitorStatic					[out]	监控窗口句柄
	返 回 值：BOOL
	=============================================================================*/
	BOOL InitMonitroHandle(HWND &hConfCb, HWND &hMtList, HWND ahModeBtn[6], HWND ahMonitorStatic[9]);

	/*=============================================================================
	函 数 名：InitVmpHandle
	功    能：初始化画面合成窗口句柄
	参    数：	hStyleCb						[out]	合成风格选择框句柄
                hSchemdIdCb                     [out]	合成方案选择框句柄
				hBroadCk						[out]	是否广播按钮句柄
				hStartBtn						[out]	开始合成按钮句柄
				hStopBtn						[out]	停止合成句柄
				hCancelBtn						[out]	退出按钮句柄
				ahVmpBtn						[out]	合成按钮句柄
	返 回 值：BOOL
	=============================================================================*/
	BOOL InitVmpHandle(HWND &hStyleCb, HWND &hSchemdIdCb, 
                       HWND &hBroadCk, HWND &hStartBtn, HWND &hStopBtn, 
                       HWND &hCancelBtn, HWND ahVmpBtn[16]);

	/*=============================================================================
	函 数 名：InitTvWallHandle
	功    能：初始化电视墙窗口句柄
	参    数：	hTvWallList						[out]	电视墙列表句柄
				hQuitBtn						[out]	退出按钮句柄
				ahTvBtn							[out]	电视墙按钮句柄
	返 回 值：BOOL
	=============================================================================*/
	BOOL InitTvWallHandle(HWND &hTvWallList, HWND &hQuitBtn, HWND ahTvBtn[16]);

	/*=============================================================================
	函 数 名：InitTvWallHandle
	功    能：初始化电视墙窗口句柄
	参    数：	hStyleCb						[out]	混音风格选择框句柄
				hStartBtn						[out]	开始混音按钮句柄
				hStopBtn						[out]	停止混音按钮句柄
				hQuitBtn						[out]	退出按钮句柄
				ahMixBtn						[out]	混音按钮句柄
	返 回 值：BOOL
	=============================================================================*/
	BOOL InitMixHandle(HWND &hStyleCb, HWND &hStartBtn, HWND &hStopBtn, HWND &hQuitBtn, HWND ahMixBtn[16]);

	/*=============================================================================
	函 数 名：OpenInitMatrix
	功    能：打开并初始化矩阵窗口句柄
	参    数：	pszConfName						[in]	会议名
				pszMtAlias						[in]	终端名
				hVideoSchCb						[out]	视频方案选择框句柄
				hVideoSaveBtn					[out]	保存视频矩阵按钮句柄
				hAudioSchCb						[out]	音频方案选择框句柄
				hAudioSaveBtn					[out]	保存音频矩阵按钮句柄
				ahVideoInBtn					[out]	视频输入端口句柄
				ahVideoOutBtn					[out]	视频输出端口句柄
				ahAudioInBtn					[out]	音频输入端口句柄
				ahAudioOutBtn					[out]	音频输出端口句柄
				hQuitBtn						[out]	退出按钮句柄
	返 回 值：BOOL
	=============================================================================*/
	BOOL OpenInitMatrix(const s8 *pszConfName, const s8 *pszMtAlias,
						HWND &hVideoSchCb, HWND &hVideoSaveBtn, HWND &hAudioSchCb,
						HWND &hAudioSaveBtn, HWND ahVideoInBtn[16], HWND ahVideoOutBtn[16],
						HWND ahAudioInBtn[16], HWND ahAudioOutBtn[16], HWND &hQuitBtn);


    /*=============================================================================
	函 数 名：InitVideoSrcHandle
	功    能：初始化切换终端视频源端口句柄
	参    数：	hScheNameCb						[out]	方案选择句柄
				hSaveScheBtn					[out]	保存方案按钮句柄
				hDelScheBtn						[out]	删除方案按钮句柄
				hPortCb					        [out]	视频源端口选择框句柄
				hConfMtList					    [out]	会议终端列表句柄
				hVideoMtList					[out]	设置好的终端列表句柄
				hAddOneBtn					    [out]	选择终端按钮句柄
				hOkBtn					        [out]	确定按钮句柄
				hCancelBtn						[out]	退出按钮句柄
                hLoadScheBtn                    [out]   载入方案按钮句柄
	返 回 值：BOOL32
	=============================================================================*/
    BOOL32 InitVideoSrcHandle(HWND &hScheNameCb, HWND &hSaveScheBtn, HWND &hDelScheBtn,
                              HWND &hPortCb, HWND &hConfMtList, HWND &hVideoMtList,
                              HWND &hAddOneBtn, HWND &hOkBtn, HWND &hCancelBtn,
                              HWND &hLoadScheBtn);


    // 以下初始化函数是4.0新增的

    /*=============================================================================
    函 数 名：InitMcuWizardHandle
    功    能：初始化MCU配置向导句柄
    参    数：	hCfgRdo                         [out]   进行Mcu配置Radiobox 句柄
                hImportRdo                      [out]   Import 配置Radiobox 句柄
                hExportRdo                      [out]   Export 配置Radiobox 句柄
                hMcuIp   						[out]	Mcu IP地址句柄
			    hMcuMaskIp    					[out]	Mcu 子网掩码句柄
			    hMcuGatewayIp   				[out]	Mcu 网关IP句柄
			    hRegGkCk					    [out]	是否注册到GK按钮句柄
                hGkIp                           [out]   GK IP句柄    
                hAliasEt                        [out]   Mcu 别名输入框句柄
                hE164Et                         [out]   Mcu E164输入框句柄
                hRebootCk                       [out]   立即Reboot Checkbox句柄
                hFilenameEt                     [out]   配置文件名输入框句柄
                hFileBrowseBtn                  [out]   浏览文件按钮句柄
                hNextBtn                        [out]   下一步按钮句柄
                hPrevBtn                        [out]   上一步按钮句柄    
			    hCancelBtn						[out]	退出按钮句柄

    返 回 值：BOOL32
    =============================================================================*/
    BOOL32 InitMcuWizardHandle(HWND &hCfgRdo, HWND &hImportRdo, HWND &hExportRdo,
                                   HWND &hMcuIp, HWND &hMcuMaskIp, HWND &hMcuGatewayIp,
                                   HWND &hRegGkCk, HWND &hGkIp,HWND &hAliasEt, HWND &hE164Et,  
                                   HWND &hRebootCk,
                                   HWND &hFilenameEt, HWND &hFileBrowseBtn, 
                                   HWND &hNextBtn, HWND &hPrevBtn, HWND &hCancelBtn
                                   );

    /*=============================================================================
	函 数 名：InitSnmpCfgHandle
	功    能：初始化Snmp服务器配置对话框句柄
	参    数：	hSnmpIp 						[out]	服务器Ip句柄
				hRCommEt    					[out]	读共同体Edit句柄
				hWCommEt    					[out]	写共同体Edit句柄
				hDPortEt    			        [out]	读写端口Edit句柄
				hTrapEt  					    [out]	发Trap端口Edit句柄
				hOkBtn					        [out]	确定按钮句柄
				hCancelBtn						[out]	退出按钮句柄
	返 回 值：BOOL32
	=============================================================================*/
    BOOL32 InitSnmpCfgHandle(HWND &hSnmpIp, HWND &hRCommEt, HWND &hWCommEt,
                               HWND &hDPortEt, HWND &hTrapEt,
                               HWND &hOkBtn, HWND &hCancelBtn
                               );
    
    
    /*=============================================================================
    函 数 名：InitVmpConfigHandle
    功    能：初始化画面合成配置句柄，废弃
    参    数：	hEnableCk    			        [out]	是否启用配置按钮句柄
                hSelBgBtn                       [out]   选择画面背景色按钮句柄
                hSelMtBtn                       [out]   选择画面一般终端边框色按钮句柄
                hSelCmBtn                       [out]   选择画面主席终端边框色按钮句柄    
                hSelSpBtn                       [out]   选择画面发言终端边框色按钮句柄
			    hOkBtn					        [out]	确定按钮句柄
			    hCancelBtn						[out]	退出按钮句柄
    返 回 值：BOOL32
    =============================================================================*/
    BOOL32 InitVmpConfigHandle(HWND &hEnableCk,
                               HWND &hSelBgBtn, HWND &hSelMtBtn, HWND &hSelCmBtn,
                               HWND &hSelSpBtn,
                               HWND &hOkBtn, HWND &hCancelBtn
                               );

    /*=============================================================================
    函 数 名：InitMcuChipCfgHandle
    功    能：初始化MCU 单板配置句柄
    参    数：	hMcuChipCb 						[out]	Mcu 单板类型选择框句柄
			    hLayerEt    					[out]	单板层号编辑框句柄
			    hSlotEt         				[out]	单板槽号编辑框句柄
			    hChipIp   						[out]	Mcu 单板IP地址句柄
			    hLanIfCb       					[out]	网口选择句柄
			    hBrdCk            				[out]	是否组播Checkbox句柄         
            
                hOkBtn
                hCancelBtn

    返 回 值：BOOL32
    =============================================================================*/
    BOOL32 InitMcuChipCfgHandle(HWND &hMcuChipCb, 
                                   HWND &hLayerEt, HWND &hSlotEt, 
                                   HWND &hChipIp, 
                                   HWND &hLanIfCb, HWND &hBrdCk, 
                                   HWND &hOkBtn, HWND &hCancelBtn
                                   );

    /*=============================================================================
    函 数 名：InitMcuChipModuleCfgHandle
    功    能：初始化MCU 单板模块配置对话框句柄
    参    数：	hModuleTypeCb                   [out]   模块类型Combobox句柄
                hModuleAliasEt    				[out]	模块别名编辑款句柄
			    hMcuStartPortEt					[out]	模块Mcu起始端口号编辑框句柄
			    hSwitchBrdIdxEt    				[out]	模块交换板索引号编辑框句柄
			    hRunningBrdIdxEt  				[out]	模块运行板索引号编辑框句柄
			    hRecvStartPortEt				[out]	模块接收数据起始端口号编辑框句柄
			    ahUseMapCk            		    [out]	使用Map Checkbox句柄
            
                hOkBtn
                hCancelBtn

    说    明：
    返 回 值：BOOL32
    =============================================================================*/
    BOOL32 InitMcuChipModuleCfgHandle(HWND &hModuleTypeCb, 
                                   HWND &hModuleAliasEt, 
                                   HWND &hMcuStartPortEt, 
                                   HWND &hSwitchBrdIdxEt, 
                                   HWND &hRunningBrdIdxEt, 
                                   HWND &hRecvStartPortEt,
                                   HWND ahUseMapCk[5],
                                   HWND &hOkBtn, HWND &hCancelBtn
                                   );

    /*=============================================================================
	函 数 名：InitModBasicHandle
	功    能：初始化模版基本信息对话框句柄
	参    数：	hConfNameEt                         [out]   会议名编辑框句柄
                hConfE164Et                         [out]   会议E164号编辑框句柄
                hBitrateCb                          [out]   会议码率Combo句柄
                hBitAdvBtn                          [out]   会议码率高级设置按钮句柄
                hBitDefaultBtn                      [out]   码率存为默认按钮句柄
                hDoubleBitCk                        [out]   双速会议Checkbox句柄
                hLowBtiBrdCk                        [out]   辅助码率组播Checkbox句柄
                hLowBitrateCb                       [out]   低速码率Combo句柄
                hConfHourEt                         [out]   会议持续小时编辑框句柄
                hConfMinuteEt                       [out]   会议持续分钟编辑框句柄
                hEndManualCk                        [out]   手工结束Checkbox句柄
                hAdvBtn                             [out]   打开高级配置对话框按钮句柄
                hOkBtn
                hCancelBtn

    说    明：仅内部使用
	返 回 值：BOOL32
	=============================================================================*/
    BOOL32 InitModBasicHandle(HWND &hConfNameEt, HWND &hConfE164Et, 
                              HWND &hBitrateCb, HWND &hBitAdvBtn, HWND &hBitDefaultBtn, 
                              HWND &hDoubleBitCk, HWND &hLowBitBrdCk, HWND &hLowBitrateCb,
                              HWND &hConfHourEt, HWND &hConfMinuteEt, HWND &hEndManualCk,
                              HWND &hAdvBtn,
                              HWND &hOkBtn, HWND &hCancelBtn);
                              


    /*=============================================================================
	函 数 名：InitModMediaHandle
	功    能：初始化模版媒体信息对话框句柄
	参    数：hMainVFmtCb	                         [out]   主视频格式Combo句柄
              hMainResCb                             [out]   主视频分辨率Combo句柄
              hMainFrCb                              [out]   主视频帧率Combo句柄
              hMainAFmtCb	                         [out]   主音频格式ombo句柄
              hSecVFmtCb	                         [out]   辅视频格式Combo句柄
              hSecResCb                              [out]   辅视频分辨率Combo句柄
              hSecFrCb                               [out]   辅视频帧率Combo句柄
              hSecAFmtCb	                         [out]   辅音频格式ombo句柄
              hDStreamVFmtCb                         [out]   双流格式Combo句柄
              hDStreamResCb                          [out]   双流分辨率Combo句柄
              hDsFrCb                                [out]   双流视频帧率Combo句柄
              hSensitivityCb                         [out]   语音激励敏感度Combo句柄
              hSpeakerSrcCb                          [out]   接收类型Combo句柄
              hSpeakerAudioCk                        [out]   是否接受音频Checkbox
              hMixDelayEt                            [out]   混音延时Edit
              hOkBtn
              hCancelBtn


    说    明：仅内部使用
	返 回 值：BOOL32
	=============================================================================*/
    BOOL32 InitModMediaHandle(HWND &hMainVFmtCb, HWND &hMainResCb, HWND &hMainFrCb, HWND &hMainAFmtCb, 
                              HWND &hSecVFmtCb, HWND &hSecResCb, HWND &hSecFrCb, HWND &hSecAFmtCb,
                              HWND &hDStreamVFmtCb, HWND &hDStreamResCb, HWND &hDsFrCb, 
                              HWND &hSensitivityCb, 
                              HWND &hSpeakerSrcCb, HWND &hSpeakerAudioCk,
                              HWND &hMixDelayEt,
                              HWND &hOkBtn, HWND &hCancelBtn);

    

    /*=============================================================================
	函 数 名：InitModMtHandle
	功    能：初始化模版终端信息对话框句柄
	参    数：	hMtAliasEt                          [out]   Mt别名输入编辑框句柄
                hAddMtBtn                           [out]   添加终端按钮句柄
                hAddH320MtBtn                       [out]   添加H320按钮句柄
                hAddToAddrCk                        [out]   是否同时添加到地址簿Checkbox句柄
                hAddrbookBtn                        [out]   打开地址簿按钮句柄
                hMtList                             [out]   终端列表 List句柄
                hDelMtBtn                           [out]   删除终端按钮句柄
                hCallTypeCfgBtn                     [out]   呼叫类型配置按钮句柄
                hVmpCfgBtn                          [out]   配置画面合成器/电视墙按钮句柄

                hUseVmpCk                           [out]   启用Vmp Checkbox句柄
                hBrdVmpCk                           [out]   广播Vmp 码流Checkbox句柄
                hVmpStyleCb                         [out]   画面合成风格Combobox句柄
                hVmpSchemaCb                        [out]   画面合成方案Combobox句柄
                ahVmpBtn[16]                        [out]   16个画面合成按钮句柄
                hUseTvwallCk                        [out]   启用电视墙Checkbox句柄
                hTvwallCb                           [out]   电视墙选择Combobox句柄
                ahTvwallBtn[16]                     [out]   16个电视墙按钮句柄

                hOkBtn
                hCancelBtn                

    说    明：仅内部使用
	返 回 值：BOOL32
	=============================================================================*/
    BOOL32 InitModMtHandle(HWND &hMtAliasEt, HWND &hAddMtBtn, HWND &hAddH320MtBtn, 
                           HWND &hAddToAddrCk, HWND &hAddrBookBtn,
                           HWND &hMtList, HWND &hDelMtBtn, 
                           HWND &hCallTypeCfgBtn, HWND &hVmpCfgBtn, 
                           HWND &hUseVmpCk, HWND &hBrdVmpCk,
                           HWND &hVmpStyleCb, HWND &hVmpSchemaCb, 
                           HWND ahVmpBtn[16], 
                           HWND &hUseTvwallCk, 
                           HWND &hTvwallCb,
                           HWND ahTvwallBtn[16],  
                           HWND &hOkBtn, HWND &hCancelBtn );

    /*=============================================================================
	函 数 名：InitModAdvHandle
	功    能：初始化模版高级信息对话框句柄
	参    数：	hConfCtrlCk                         [out]   是否会控加密句柄
                hOpenModeCb                         [out]   会议开放模式Combobox句柄
                hConfPwdEt                          [out]   会议密码Edit句柄
                hConfPwdCfmEt                       [out]   会议密码确认Edit句柄
                hEncryptTransCb                     [out]   加密传输Combobox句柄
                
                hCascadeModeCk                      [out]   级联Checkbox句柄
                hMulticastCk                        [out]   组播会议Checkbox句柄
                hDataConfCk                         [out]   含数据会议Checkbox句柄
                hQosCk                              [out]   Qos Checkbox句柄
                hVideoPrioityCk                     [out]   视频优先Checkbox句柄
                hAdapterCk                          [out]   启用适配器Checkbox句柄
                hRetransferCk                       [out]   丢包重传Checkbox句柄
                hUniformModeCk                      [out]   归一重整Checkbox句柄
                hConfMuteCk                         [out]   初始哑音Checkbox句柄
                hDiscussCk                          [out]   讨论会议Checkbox句柄
                hAutoEndCk                          [out]   自动结束Checkbox句柄
                hDataConfIp                         [out]   数据会议IP 控件句柄
                hOkBtn
                hCancelBtn
                
    说    明：仅内部使用
	返 回 值：BOOL32
	=============================================================================*/
    BOOL32 InitModAdvHandle(HWND &hConfCtrlCk, HWND &hOpenModeCb, 
                            HWND &hConfPwdEt, HWND &hConfPwdCfmEt, 
                            HWND &hEncryptTransCb, 
                            HWND &hCascadeModeCk, HWND &hMulticastCk,HWND &hDataConfCk, 
                            HWND &hQosCk, HWND &hVideoPriorityCk, 
                            HWND &hAdapterCk, HWND &hRetransferCk, HWND &hUniformModeCk, 
                            HWND &hConfMuteCk, HWND &hDiscussCk, 
                            HWND &hAutoEndCk,
                            HWND &hDataConfIp, 
                            HWND &hCallCb, HWND &hCallNumEt, HWND &hCallTimerEt,
                            HWND &hOkBtn, HWND &hCancelBtn);


    /*=============================================================================
	函 数 名：InitModMtAdvHandle
	功    能：初始化模版会议/终端呼叫方式对话框句柄,已废弃
	参    数：	hMtBitrateCk                        [out]   设置终端呼叫码率Check句柄
                hMtBitrateEt                        [out]   终端呼叫码率Edit句柄
                hMtCallCb                           [out]   呼叫方式Combo句柄
                hCallNumEt                          [out]   呼叫次数Edit句柄
                hCallTimerEt                        [out]   呼叫间隔Edit句柄
                hOkBtn
                hCancelBtn                

    说    明：仅内部使用
	返 回 值：BOOL32
	=============================================================================
    BOOL32 InitModMtAdvHandle(HWND &hMtBitrateCk, HWND &hMtBitrateEt, 
                              HWND &hMtCallCb, HWND &hCallNumEt, HWND &hCallTimerEt,
                              HWND &hOkBtn, HWND &hCancelBtn); */
    

    /*=============================================================================
	函 数 名：InitAddH320Handle
	功    能：初始化添加H320对话框控件句柄
	参    数：	hLayerEt                            [out]   层号输入编辑框句柄
                hSlotEt                             [out]   槽号输入编辑框句柄
                hChannelEt                          [out]   通道号输入编辑框句柄
                hAddToAddrCk                        [out]   添加到地址簿Check句柄
                hOkBtn            

    说    明：仅内部使用
	返 回 值：BOOL32
	=============================================================================*/
    BOOL32 InitAddH320Handle(HWND &hLayerEt, HWND &hSlotEt, HWND &hChannelEt,
                             HWND &hAddToAddrCk,
                             HWND &hOkBtn);

    
	//-----------------------------------------------------------------------------
	//	5.摄像头遥控
	//-----------------------------------------------------------------------------

	/*=============================================================================
	函 数 名：Camera
	功    能：摄像头遥控控制
	参    数：	pszConfName						[in]	会议名
				pszMtAlias						[in]	终端名
				nClickButton					[in]	按下按钮号
				byChannel						[in]	方案位置
	返 回 值：BOOL
	=============================================================================*/
	BOOL Camera(const s8 *pszConfName, const s8 *pszMtAlias, s32 nClickButton, u8 byChannel = 0);

	//-----------------------------------------------------------------------------
	//	6.监控操作准备
	//-----------------------------------------------------------------------------

	/*=============================================================================
	函 数 名：Monitor
	功    能：监控预操作
	参    数：	byChannel			[in]	停止监控的控件位置
				byMonitorPos		[in]	监控位置 0:模拟会场 1:监控窗口
				ahMonitorBtn		[out]	监控窗口句柄
	返 回 值：BOOL
	=============================================================================*/
	BOOL Monitor(u8 &byChannel, u8 byMonitorPos, HWND ahMonitorBtn[9]);

private:
	//-----------------------------------------------------------------------------
	//	7.其他
	//-----------------------------------------------------------------------------

	/*=============================================================================
	函 数 名：GetTextFromTMtAlias
	功    能：从TMtAlias结构里获取字符串
	参    数：	ptMtAlias						[in]	别名结构
	说    明：别名为IP地址时, 将u32转为字符串;否则返回别名结构里的字符串
	返 回 值：无
	=============================================================================*/
	CString GetTextFromTMtAlias(const TMtAlias *ptMtAlias);

	/*=============================================================================
	函 数 名：GetAudPlatHandle
	功    能：根据当前可见窗口, 获取模拟会场里观众席和主席台句柄
	参    数：无
	说    明：0: LISTCTRL; 1: TREECTRL;
	返 回 值：无
	=============================================================================*/
	s32 GetAudPlatHandle();

	/*=============================================================================
	函 数 名：GetSubItem
	功    能：根据传入别名类型确定ListCtrl里子项位置
	参    数：	byAliasType						[in]	别名类型
				bAddr							[in]	ListCtrl是否与地址簿有关
	说    明：
	返 回 值：子项位置
	=============================================================================*/
	s32 GetSubItem(u8 byAliasType, BOOL bAddr = FALSE);

    /*=============================================================================
	函 数 名：FindAddrEntry
	功    能：找出地址簿条目位置
	参    数：  hWnd                            [in]    条目控件句柄
                tAddrEntry						[in]	地址簿条目
	说    明：
	返 回 值：条目位置, 没有找到该条目则返回-1
	=============================================================================*/
    s32 FindAddrEntry(HWND hWnd, const TADDRENTRYINFO &tAddrEntry);


    /*=============================================================================
	函 数 名：ColorDialogInput
	功    能：在颜色选择对话框中选择颜色
	参    数：  color                            [in]    要选择的颜色RGB值
	说    明：
	返 回 值：BOOL32
	=============================================================================*/
    BOOL32 ColorDialogInput(COLORREF color);




protected:
	CGuiUnit			m_cGuiUnit;			//界面驱动库接口
	s32					m_nCurGuiPos;		//当前所在界面位置
	s32					m_nCurGuiSubPos;	//当前所在子界面位置
	BOOL				m_bOprDelay;		//是否延时的开关,默认为延时
	s32					m_nCurState;		//当前所处界面状态

	HWND				m_hCallingDlg;		//点名界面句柄
    HWND                m_hVideoSrcPollDlg; //视频源轮询界面句柄

//	HWND				m_hPollDlg;			//轮询界面句柄
//	HWND				m_hMsgDlg;			//短消息界面句柄
//	HWND				m_hVmpDlg;			//画面合成界面句柄
//	HWND				m_hTvWallDlg;		//电视墙界面句柄
//	HWND				m_hMixDlg;			//定制混音界面句柄
    
	HWND				m_hAudience;		//模拟会场观众席句柄
	HWND				m_hPlatform;		//模拟会场主席台句柄
	HWND				m_hAudience1;		//模拟会场观众席ListCtrl句柄
	HWND				m_hAudience2;		//模拟会场观众席TreeCtrl句柄
	HWND				m_hPlatform1;		//模拟会场平铺状态ListCtrl句柄
	HWND				m_hPlatform2;		//模拟会场树型状态ListCtrl句柄
	HWND				m_hHelpSt;			//帮助提示句柄, 用来点击销毁菜单

	HWND				m_hConfManageBtn;	//会议管理按钮句柄
	HWND				m_hSimConfBtn;		//模拟会场按钮句柄
	HWND				m_hMonitorBtn;		//监控按钮句柄
	HWND				m_hSysManageBtn;	//系统管理按钮句柄
	HWND				m_hMcuManageBtn;	//MCU设备管理按钮句柄
	HWND				m_hConfList;		//会议信息句柄

	HWND				m_hTemplateList;	//会议模板列表句柄

	HWND				m_hMcuCb;			//连接MCU下拉框句柄
	HWND				m_hConnectBtn;		//连接MCU按钮句柄
	HWND				m_hDisconnectBtn;	//断开MCU按钮句柄
	HWND				m_hConfCb;			//会议下拉框句柄
	HWND				m_hQueitCk;			//终端静音按钮句柄
	HWND				m_hMuteCk;			//终端哑音按钮句柄
	HWND				m_hConfSettingBtn;	//会议设置按钮句柄
	HWND				m_hConfCallingBtn;	//会议点名按钮句柄
	HWND				m_hConfPollBtn;		//会议轮询按钮句柄
	HWND				m_hShortMsgBtn;		//短消息按钮句柄
	HWND				m_hViewBigBtn;		//大图标显示按钮句柄
	HWND				m_hViewSmallBtn;	//小图标显示按钮句柄
	HWND				m_hViewTreeBtn;		//树型显示按钮句柄
    HWND                m_hLocalQuietBtn;      //本级终端静音按钮句柄
    HWND                m_hLocalCancelQuietBtn;//本级取消静音按钮句柄
    HWND                m_hLocalMuteBtn;       //本级终端哑音按钮句柄
    HWND                m_hLocalCancelMuteBtn; //本级取消哑音按钮句柄
	HWND				m_hTodayConfList;	//今日会议列表句柄
	HWND				m_hAddrbookList;	//地址簿列表句柄
	HWND				m_hAddrGroupList;	//地址簿会议组列表句柄
	HWND				m_hUserList;		//用户列表句柄
	HWND				m_hAddrBtn;			//地址簿按钮句柄
	HWND				m_hUserBtn;			//用户管理按钮句柄
	HWND				m_hUserAddBtn;		//添加用户按钮句柄
	HWND				m_hUserDelBtn;		//删除用户按钮句柄
	HWND				m_hUserEditBtn;		//修改用户按钮句柄
	HWND				m_hAddEntryBtn;		//添加地址簿条目按钮句柄
    HWND				m_hAddEntryH320Btn;	//添加地址簿H320条目按钮句柄
	HWND				m_hEditEntryBtn;	//修改地址簿条目按钮句柄
	HWND				m_hDelEntryBtn;		//删除地址簿条目按钮句柄
	HWND				m_hAddGroupBtn;		//添加地址簿会议组按钮句柄
	HWND				m_hEditGroupBtn;	//修改地址簿会议组按钮句柄
	HWND				m_hDelGroupBtn;		//删除地址簿会议组按钮句柄
	HWND				m_hAddMemberBtn;	//添加会议组条目按钮句柄
	HWND				m_hDelMemberBtn;	//删除会议组条目按钮句柄
    HWND                m_hCreateConfBtn;   //创建会议按钮句柄

    HWND                m_hUserSettingBtn;  //用户设置按钮句柄

    HWND                m_hCallVmpCk;       //点名人和被点名人合成Check box句柄
    HWND                m_hCallerCk;        //点名人Check box句柄
    HWND                m_hCalleeCk;        //被点名人Check box句柄

    HWND                m_hLangCb;          //语言选择Combobox
    HWND                m_hApplyLanBtn;     //应用语言
    HWND                m_hPersonMenuCk;    //使用个性化菜单Checkbox
    HWND                m_hPersonMenuBtn;   //配置个性化菜单按钮


    // 以下在Mcu管理界面中的控件    
    HWND                m_hMcuBasic;        // Mcu配置基本信息界面
    HWND                m_hMcuNetwork;      // Mcu配置网络配置界面
    HWND                m_hMcuChip;         // Mcu配置单板和外设配置界面
    HWND                m_hMcuDev;          // Mcu配置外设状态界面
    // HWND                m_hMcuWizard;       // Mcu配置向导对话框

    HWND                m_hRefreshMcuBtn;   // 刷新Mcu信息按钮
    HWND                m_hRebootMcuBtn;    // 重启Mcu按钮
    HWND                m_hSynTimeBtn;      // 同步时间按钮

    // Mcu 配置

    HWND                m_hMcuCfgTab;       // Mcu 配置Tab控件

    HWND                m_hMcuNetSaveBtn;   // 保存网络设置按钮
    HWND                m_hCfgWizardBtn;    // 配置向导按钮

    HWND                m_hMcuNetIp;        // Mcu Ip地址
    HWND                m_hMcuNetMask;      // Mcu 子网掩码
    HWND                m_hMcuNetGw;        // Mcu 网关地址
    HWND                m_hMcuRegGkCk;      // 是否注册到Gk的Checkbox
    HWND                m_hMcuRegGkIp;      // Gk Ip
    HWND                m_hMcuAliasEt;      // Mcu 别名
    HWND                m_hMcuE164Et;       // Mcu E164 号

    HWND                m_hMcuBrdIp;        // 组播IP地址
    HWND                m_hMcuBrdPortEt;    // 组播起始端口
    HWND                m_hMcuDataPortEt;   // 接受数据起始端口
    HWND                m_hMcuH225PortEt;   // H225起始端口

    HWND                m_hMcuMaxMtNumEt;   // 最大连接终端数


    // SNMP tab
    HWND                m_hSnmpLs;          // SNMP 服务器List
    HWND                m_hMcuSnmpAddBtn;   // Add Snmp
    HWND                m_hMcuSnmpDelBtn;   // Del Snmp
    HWND                m_hMcuSnmpEditBtn;  // Edit Snmp

    // QoS Tab
    HWND                m_hMcuQosTypeCb;    // QoS 类型 Combo

    HWND                m_hMcuQosIpLDCk;    // Qos IP Low Delay Check
    HWND                m_hMcuQosIpHTCk;    // Qos IP High Throu Check
    HWND                m_hMcuQosIpHRCk;    // Qos IP High Reliab Check
    HWND                m_hMcuQosIpLECk;    // Qos IP Low Expense Check

    HWND                m_hMcuQosIpAudioEt; // 音频服务等级
    HWND                m_hMcuQosIpVideoEt; // 视频服务等级
    HWND                m_hMcuQosIpCmdEt;   // 信令服务等级
    HWND                m_hMcuQosIpDataEt;  // 数据服务等级
    HWND                m_hMcuQosDefaultBtn;// 恢复到QoS默认值按钮

    // Misc Tab
    HWND                m_hMcuMiscSaveBwCk; // 是否节省码流Checkbox
    HWND                m_hMcuMiscChainCkEt;// 断链检测间隔
    HWND                m_hMcuMiscChainCkTimesEt;// 断链检测次数
    HWND                m_hMcuMiscMtRefRateEt; // 下级终端列表刷新间隔
    HWND                m_hMcuMiscAudRefRateEt;// 下级音频源刷新间隔
    HWND                m_hMcuMiscVidRefRateEt;// 下级视频源刷新间隔

    HWND                m_hMcuMpcCk;        // 使用Mpc转发数据Check
    HWND                m_hMcuMpcStackCk;   // 使用Mpc协议栈
    HWND                m_hMcuSymModeCb;    // 同步模式
    HWND                m_hMcuDriEt;        // Dri 板号
    HWND                m_hMcuE1Et;         // E1 通道号    

    HWND                m_hMcuMiscDcsCk;    // 启用DCS
    HWND                m_hMcuMiscDcsIp;    // DCS IP

    HWND                m_hMcuMiscResetBtn;    // 恢复到部分Misc选项默认值按钮

    // Vmp Tab
    HWND                m_hMcuVmpSchCb;     // 方案选择Combbo
    HWND                m_hMcuVmpSchSaveBtn;// 方案保存Btn
    HWND                m_hMcuVmpSchRenameBtn; // 方案改名Btn

    HWND                m_hMcuVmpBgCp;      // 背景色ColorPicker
    HWND                m_hMcuVmpMtCp;      // 终端色ColorPicker
    HWND                m_hMcuVmpSpCp;      // 发言人色ColorPicker
    HWND                m_hMcuVmpCmCp;      // 主席色ColorPicker
        
    // 单板配置
    HWND                m_hMcuChipTab;      // 单板信息切换Tab
    HWND                m_hMcuChipLs;       // 单板信息列表
    HWND                m_hMcuChipAddBtn;   // 添加单板按钮
    HWND                m_hMcuChipEditBtn;  // 编辑单板按钮
    HWND                m_hMcuChipDelBtn;   // 删除单板按钮
    HWND                m_hMcuChipRebootBtn;// 重启单板按钮

    HWND                m_hMcuChipModuleLs; // 单板运行模块列表
    HWND                m_hMcuChipModAddBtn;// 添加模块按钮
    HWND                m_hMcuChipModEditBtn; // 配置模块按钮
    HWND                m_hMcuChipModDelBtn;// 删除模块按钮
    
    HWND                m_hMcuRecLs;        // 录像机列表
    HWND                m_hMcuRedAddBtn;    // 添加录像机按钮
    HWND                m_hMcuRedEditBtn;   // 配置录像机按钮
    HWND                m_hMcuRedDelBtn;    // 删除录像机按钮
    
    
    // 外设管理
    HWND                m_hDevManageList;   // 外设列表List
   
};

#endif	//	#define __MCSUITESTLIB_H