/*****************************************************************************
模块名      : Pcmt界面测试库操作接口
文件名      : PcmtGuiTestLib.h
相关文件    : 
文件实现功能: 单元测试
作者        : 张寒冰
版本        : V4.0  Copyright(C) 1997-2005 KDC, All rights reserved.
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2005/07/25  4.0         张寒冰        创建
2005/08/18  4.0         张寒冰        修改
2005/10/11  4.0         张寒冰        修改
******************************************************************************/

#ifndef PCMTGUITESTLIB_H
#define PCMTGUITESTLIB_H

#include "cppunit\GuiUnit.h"
#include "kdvType.h"		// 公共类型
#include "kdvDef.h"			// 公共宏
#include "osp.h"			// OSP业务
#include "mtstruct.h"		// mt类型结构
#include "mtsysapi.h"
#include "dcmtdefine.h"

#include "pcmtguitestlibdefine.h" //公共定义文件

#include <vector>
using std::vector;

//	画面合成终端参数
typedef struct tagVmpMt
{
    tagVmpMt()
    {
        ZeroMemory( aszMtAlias, sizeof ( aszMtAlias ) );
        byChannel       = 0;
        byMemberType    = emVMPMmbMCSspec;
    }
	s8	aszMtAlias[MT_MAX_H323ALIAS_LEN];	//	终端别名
	u8	byChannel;					    //	放入哪一个通道
    u8	byMemberType;				    //	跟随方式
} TVmpMt;

typedef struct tagMonitorMt
{
	CString strMtName;	                //	终端别名
	u8	byChannel;					    //	放入哪一个通道
} TMonitorMt;

class CPcmtGuiTestLib
{
public:
    CPcmtGuiTestLib();
    virtual ~CPcmtGuiTestLib();
private:
	/*=======================================================================
	函 数 名: GuiActiveWindow
	功    能：激活桌面终端窗口
	参    数：
	返 回 值：成功返回TRUE，失败返回FALSE
	说    明：
	=======================================================================*/
	BOOL32 GuiActiveWindow();
	
private:
    //*****内部用的操作接口************************************************************//

	/*=======================================================================
	函数名： MoveToBtn
    功    能： 移动并单击主界面的某个按钮
    参    数： HWND hWnd 按钮句柄
    返 回 值： BOOL32 是否成功
    说    明:
	=======================================================================*/
    BOOL32 MoveToBtn(HWND hWnd);    

    /*函数名： InitPCMTHandle
    功    能： 初始化句柄
    参    数： 
    返 回 值： BOOL32 
    说    明： */
    BOOL32 InitPcmtHandle();   

    /*函数名： ShowCfgDlg
    功    能： 显示配置页
    参    数： szPassword: 配置口令
    返 回 值： BOOL32 
    说    明： */
    BOOL32 ShowCfgDlg(const CString& szPassword = _T(""));

    BOOL32 ChangeView(u8 byIndex);

    BOOL32 MenuClickMt(s32 nListItem, s32 nMenuItem);

    u8 GetVmpStyle(u8 byType);

public:
    //*****提供给外部调用的接口******//

    //*****系统初始化的操作接口********************************************************//
   
	/*=============================================================================
	函 数 名： SetDelay
	功    能： 设置是否操作延时（默认为延时）
	参    数： BOOL bDelay					[in]	true-延时, false-不延时
	返 回 值： void
	=============================================================================*/
	inline void SetDelay(BOOL32 bDelay = TRUE)
	{
		m_bDelay = bDelay;
	}

    /*函数名： GuiInit
    功    能： 初始化PCMT
    参    数： CString szExePath   容器路径
    返 回 值： BOOL32 
    说    明： */
    BOOL32 GuiInit(CString szExePath);

    /*函数名： InitOP
    功    能： 再次初始化界面测试库
    参    数： CString szTitle 
    返 回 值： BOOL32 
    说    明： */
    BOOL32 InitOP(const CString& szTitle);
    
    
    /*函数名： close
    功    能： 关闭测试接口清理现场
    参    数： 
    返 回 值： void 
    说    明： 退出时最好调用以便关闭测试对象*/
    void Close();

    /*函数名： IsInP2PConf
    功    能： 是否在点对点会议中
    参    数： 
    返 回 值： BOOL32 
    说    明： */
    BOOL32 IsInP2PConf();

    /*函数名： IsInP2PConf
    功    能： 是否在点对点呼叫中
    参    数： 
    返 回 值： BOOL32 
    说    明： */
    BOOL32 IsCalling();

    /*函数名： IsInMConf
    功    能： 是否在多点会议中
    参    数： 
    返 回 值： BOOL32 
    说    明： */
    BOOL32 IsInMConf();

    /*函数名： IsChairman
    功    能： 是否主席
    参    数： 
    返 回 值： BOOL32 
    说    明： */
	BOOL32 IsChairman();

    /*函数名： IsSpeaker
    功    能： 是发言人
    参    数： u8 byIndex list控件中的索引
    返 回 值： BOOL32 
    说    明： */
	BOOL32 IsSpeaker(u8 byIndex);


    /*函数名： GetSpeaker
    功    能： 
    参    数： 无
    返 回 值： 
    说    明： */
    CString GetSpeaker();

    /*函数名： GetChairman
    功    能： 
    参    数： 无
    返 回 值：  
    说    明： */
    CString GetChairman();


    //**********主界面测试接口********************************************************//

    /*函数名： AboutVer
    功    能： 关于版本信息
    参    数： 
    返 回 值： 
    说    明： */
    BOOL32 AboutVer();
	
    /*函数名： AboutHelp
    功    能： 关于帮助
    参    数： 
    返 回 值： 
    说    明： */
    BOOL32 AboutHelp();
		
    /*函数名： Dummy
    功    能： 哑音
    参    数： BOOL32 bDummy TRUE 哑音 FALSE 取消哑音
    返 回 值： BOOL32 
    说    明： */
    BOOL32 Mute(BOOL32 bMute);

    /*函数名： Silent
    功    能： 静音
    参    数： BOOL32 bSilent TRUE 静音 FALSE 取消静音
    返 回 值： BOOL32 
    说    明： */
    BOOL32 Silent(BOOL32 bSilent);

    /*函数名： StartMonitor
    功    能： 开始监控
    参    数： vector< TMonitorMt >& vecMt 监控信息
    返 回 值： BOOL32 
    说    明： */
    BOOL32 StartMonitor(vector< TMonitorMt >& vecMt);

    /*函数名： StopMonitor
    功    能： 停止监控
    参    数： u8 byIndex:   监控通道索引(0 ~ 8)
    返 回 值： BOOL32 
    说    明： */
    BOOL32 StopMonitor(u8 byIndex);

    /*函数名： RealtimeMonitor
    功    能： 实时监控
    参    数： u8 byIndex:   监控通道索引(0 ~ 8)
    返 回 值： BOOL32 
    说    明： */
    BOOL32 RealtimeMonitor(u8 byIndex);

    /*函数名： StopRealtimeMonitor
    功    能： 停止实时监控
    参    数： u8 byIndex:   监控通道索引(0 ~ 8)
    返 回 值： BOOL32 
    说    明： */
    BOOL32 StopRealtimeMonitor(u8 byIndex);

    /*函数名： LocalFullScreen
    功    能： 本地全屏
    参    数： 
    返 回 值： BOOL32 
    说    明： */
    BOOL32 LocalFullScreen();

    /*函数名： LocalSnap
    功    能： 抓拍
    参    数： CString& lpszPath
    返 回 值： BOOL32 
    说    明： */
    BOOL32 LocalSnap(const CString& szPath = _T(""));

    //************点对点会议测试*****************************************************// 
    
    /*函数名： MakeCall
    功    能： 呼叫其它终端
    参    数： TMtAddr tTermAddr 终端呼叫结构 
	           u16 wBitRate 呼叫速率
               BOOL32 bAddrbook  是否通过地址簿来呼叫
			   u32 dwSleepTime   等待呼叫成功进入会议的时间，如果为0，则不等待而直接返回
    返 回 值： BOOL32 成功返回TRUE，失败返回FALSE
    说    明： */
	BOOL32 MakeCall(const TMtAddr& tTermAddr, u16 wBitRate, BOOL32 bAddrbook = FALSE,
		            u32 dwSleepTime = 20000);

    /*函数名： CancelCalling
    功    能： 取消呼叫
    参    数： 
    返 回 值： BOOL32 成功返回TRUE，失败返回FALSE
    说    明： */
	BOOL32 CancelCalling();
    
    /*函数名： EndP2PConf
    功    能： 结束点对点会议
    参    数： 
    返 回 值： BOOL32 成功返回TRUE，失败返回FALSE
    说    明： */
	BOOL32 EndP2PConf();

    
    //************多点会议操作接口************************************************// 

    /*函数名： CreateConf
    功    能： 终端创建会议
    参    数： const TMtAddr& tMcuInfo		Mcu的信息
			   u16 wBitRate                 呼叫速率
			   const TConfBaseInfo& info		会议信息
			   const vector<TMtAddr>& vMts	终端信息
               u32 dwSleepTime              创建后等待时间，如果为0则不等待而直接返回
    返 回 值： BOOL32 成功返回TRUE，失败返回FALSE
    说    明： */
   BOOL32 CreateConf(const TMtAddr& tMcuInfo, u16 wBitrate,const TConfBaseInfo &info, 
	                 const vector<TMtAddr>& vMts,const CString& szGroupName = "", 
					 BOOL32 bByAddrbook = FALSE, DWORD dwSleepTime = 20000); 
    
    /*函数名： JoinConf
    功    能： 终端申请加入会议
    参    数： const TMtAddr& mcu [in]MCU的地址
               u16 &wBitRate 呼叫速率
               CString szConfName 会议名称
               CString szPWD 会议密码
    返 回 值： BOOL32 
    说    明： */
	BOOL32 JoinConf(const TMtAddr& mcu, u16 wBitRate, const CString& szConfName=_T(""), 
		            const CString& szPWD = "123");

	/*函数名： InputConfPass
    功    能： 输入会议密码
    参    数： const CString& szPassword 密码
    返 回 值： BOOL32 
    说    明： */
    BOOL32 InputConfPwd(const CString& szPassword);		
	
    /*函数名： QuitConf
    功    能： 退出会议
    参    数： 
    返 回 值： BOOL32 
    说    明： */
	BOOL32 QuitConf();

    /*函数名： EndConf
    功    能： 结束会议
    参    数： 
    返 回 值： BOOL32 成功返回TRUE，失败返回FALSE
    说    明： */
	BOOL32 EndConf();

    /*函数名： DelayConf
    功    能： 延长会议
    参    数： u32 dwTime
    返 回 值： BOOL32 成功返回TRUE，失败返回FALSE
    说    明： */
	BOOL32 DelayConf(u32 dwTime);
	
    /*函数名： InviteMt
    功    能： 邀请终端
    参    数： const TMtAddr& tMt
               u16 &wBitRate
    返 回 值： BOOL32 
    说    明： */
    BOOL32 InviteMt(const TMtAddr& tMt, u16 wBitRate);

    /*函数名： RevokeMt
    功    能： 请退终端
    参    数： CString &szMt
    返 回 值： BOOL32 
    说    明： */
    BOOL32 RevokeMt(const CString& szMt);

    /*函数名： ReqSpeak
    功    能： 申请发言
    参    数： 
    返 回 值： BOOL32 成功返回TRUE，失败返回FALSE
    说    明： */
	BOOL32 ReqSpeak();

    /*函数名： ReqChairman
    功    能： 申请主席
    参    数： 
    返 回 值： BOOL32 
    说    明： */
	BOOL32 ReqChairman();

    /*函数名： ReqChimein
    功    能： 申请插话
    参    数： 
    返 回 值： BOOL32 
    说    明： */
    BOOL32 ReqChimein();

    /*函数名： SpecifySpeaker
    功    能： 指定发言终端
    参    数： CString &szMt
    返 回 值： BOOL32 
    说    明： */
	BOOL32 SpecifySpeaker(const CString& szMt);

    /*函数名： SpecifyChairman
    功    能： 指定主席终端
    参    数： CString &szMt
    返 回 值： BOOL32 
    说    明： */
	BOOL32 SpecifyChairman(const CString& szMt);

    /*函数名： RevokeSpeaker
    功    能： 取消当前发言终端的发言权
    参    数： 
    返 回 值： BOOL32 
    说    明： */
	BOOL32 RevokeSpeaker();

    /*函数名： RevokeChairman
    功    能： 取消当前主席终端的主席权
    参    数： 
    返 回 值： BOOL32 
    说    明： */
	BOOL32 RevokeChairman();

    /*函数名： StartDiscuss
    功    能： 开始讨论模式
    参    数： u8 byMax      讨论的深度
    返 回 值： BOOL32 
    说    明： */
	BOOL32 StartDiscuss(u8 byMax);

    /*函数名： StopDiscuss
    功    能： 停止讨论模式
    参    数： 
    返 回 值： BOOL32 
    说    明： */
	BOOL32 StopDiscuss();

    /*函数名： StartVAC
    功    能： 开始语音激励
    参    数： 
    返 回 值： BOOL32 
    说    明： */
    BOOL32 StartVAC();

    /*函数名： StopVAC
    功    能： 停止语音激励
    参    数： 
    返 回 值： BOOL32 
    说    明： */
    BOOL32 StopVAC();

    /*函数名： StartVMP
    功    能： 开始视频复合
    参    数： u8 byType 复合类型
               vector<CString> &vctMT 参与复合的终端列表
    返 回 值： BOOL32 
    说    明： */
	BOOL32 StartVMP(u8 byType, vector<CString> &vctMt);

    /*函数名： StopVMP
    功    能： 停止视频复合
    参    数： 
    返 回 值： BOOL32 
    说    明： */
	BOOL32 StopVMP();
    
    /*函数名： UpdateVMP
    功    能： 修改视频复合参数
    参    数： const TMtVMPParam &vmp
    返 回 值： BOOL32 
    说    明： */
	BOOL32 UpdateVMP(const TMtVMPParam &vmp);
    
    
    /*函数名： ChangeVmpType
    功    能： 修改视频复合参数
    参    数： u8 byIndex  窗口序号 从0开始
               u8 byType   复合类型
                                                0  //为关闭
               VMP_MEMBERTYPE_MCSSPEC           1  //会控指定 
               VMP_MEMBERTYPE_SPEAKER           2  //发言人跟随
               VMP_MEMBERTYPE_CHAIRMAN          3  //主席跟随
               VMP_MEMBERTYPE_POLL              4  //轮询视频跟随
    返 回 值： BOOL3232 
    说    明： */
	BOOL32 ChangeVmpType(u8 byIndex, u8 byType);
    
    /*函数名： DynSwitchVmpStyle
    功    能： 修改视频复合风格
    参    数： u8 byType :风格(VMP_STYLE_DYNAMIC等)
    返 回 值： 
    说    明： */
    BOOL32 DynSwitchVmpStyle(u8 byType);

    /*函数名： SelectMt
    功    能： 选看终端
    参    数： CString &szMt 
               u8 byType选看类型
    返 回 值： BOOL32 
    说    明： */
    BOOL32 SelectMt(const CString& szMt, u8 byType);

    /*函数名： CancelSelectMT
    功    能： 取消选看
    参    数： CString &szMt
    返 回 值： BOOL32 
    说    明： */
    BOOL32 CancelSelectMT(const CString& szMt);
    
    /*函数名： StartPoll
    功    能： 开始会议轮询
    参    数： u32 dwInterval 轮询间隔
               u8 byType      轮询类型
               vector<CString> &vctMt轮询终端
    返 回 值： BOOL32 
    说    明： */
    BOOL32 StartPoll(u32 dwInterval, u8 byType, vector<CString> &vctMt);

    /*函数名： StopPoll
    功    能： 停止会议轮询
    参    数： 
    返 回 值： BOOL32 
    说    明： */
    BOOL32 StopPoll();

    /*函数名： PausePoll
    功    能： 暂停会议轮询
    参    数： 
    返 回 值： BOOL32 
    说    明： */
    BOOL32 PausePoll();
	
    /*函数名： ResumePoll
    功    能： 恢复会议轮询
    参    数： 
    返 回 值： BOOL32 
    说    明： */
    BOOL32 ResumePoll();

    /*函数名： SendShortMessage
    功    能： 向与会终端发送短消息
    参    数： vector<CString> &vctMt 接收终端
               CString szMsg 消息内容
               BOOL32 bSelectAll 全选
    返 回 值： BOOL32 
    说    明： */
	BOOL32 SendShortMessage(const CString& vctMt, const CString& szMsg,
                            BOOL32 bSelectAll = FALSE);
	
    /*函数名： BroadcastMessage
    功    能： 向与会终端广播消息
    参    数： CString& vctMt 接收终端
               CString& szMsg 消息内容
               BOOL32 bSelectAll 全选
			   u32 dwTimes 发送次数
    返 回 值： BOOL32 
    说    明： */
	BOOL32 BroadcastMessage(vector<CString> &vctMt, const CString& szMsg,
                            BOOL32 bSelectAll = FALSE, u32 dwTimes = 1);

    /*函数名： AcceptMt
    功    能： 接受终端申请 适用于其他终端申请发言、主席、插话时弹出对话框的确认
    参    数： CString szMtName 终端名字
               BOOL32 bAccept  TRUE接受, FALSE拒绝
    返 回 值： BOOL32 
    说    明： */
	BOOL32 AcceptMt(const CString& szMtName, BOOL32 bAccept = TRUE);
    
    /*函数名： RemoteDummy
    功    能： 远端哑音
    参    数： CString szMt
    返 回 值： BOOL32 
    说    明： */
    BOOL32 RemoteDummy(const CString& szMt);

    /*函数名： RemoteSilent
    功    能： 远端静音
    参    数： CString szMt
    返 回 值： BOOL32 
    说    明： */
    BOOL32 RemoteSilent(const CString& szMt);


    //************终端配置************************************************// 

    /*函数名： SetCfgPassword
    功    能： 设置配置口令
    参    数： String& szNewPassword: 为空时表示取消口令
               String& szOldPassword: 旧口令
    返 回 值： BOOL32 
    说    明： */
    BOOL32 SetCfgPassword(const CString& szNewPassword = _T(""), 
		                  const CString& szOldPassword = _T(""));
    
    /*函数名： UpdateCfgLocal
    功    能： 本地配置
    参    数： const TH323Cfg &rCfg
    返 回 值： BOOL32 
    说    明： */
	BOOL32 UpdateCfgLocal(const TH323Cfg &rCfg, const CString& szPassword = _T(""));

    /*函数名： UpdateCfgOSD
    功    能： 台标配置
    参    数： const TOsdCfg *tCfg
    返 回 值： BOOL32 
    说    明： */
    BOOL32 UpdateCfgOSD(const TOsdCfg &rCfg, const CString& szPassword = _T(""));

    /*函数名： ChangeSign
    功    能： 更换台标
    参    数： CString szBMPPath 台标文件的全路径
    返 回 值： BOOL32 
    说    明： */
    BOOL32 ChangeSign(const CString& szBMPPath);

    /*函数名： UpdateCfgNet
    功    能： 网络配置
    参    数： const TEthnetInfo &rCfg
    返 回 值： BOOL32 
    说    明： */
	BOOL32 UpdateCfgNet(const TEthnetInfo &rCfg, const CString& szPassword = _T(""));

    /*函数名： SetOtherCfg
    功    能： 其余配置
    参    数： const TOtherCfg& tCfg
    返 回 值： BOOL32 
    说    明： */
	BOOL32 SetOtherCfg(const TOTHERCFG& tCfg, const CString& szPassword = _T(""));

    /*函数名： UpdateCfgSnmp
    功    能： SNMP配置
    参    数： const TSNMPCfg &tCfg
    返 回 值： BOOL32 
    说    明： */
    BOOL32 UpdateCfgSnmp(const TSNMPCfg &tCfg, const CString& szPassword = _T(""));

    /*函数名： UpdateCfgCall
    功    能： 呼叫配置
    参    数： const TUserCfg &rCfg
    返 回 值： BOOL32 
    说    明： */
	BOOL32 UpdateCfgCall(const TUserCfg &rCfg, const CString& szPassword = _T(""));

    /*函数名： UpdateCfgVEnc
    功    能： 视频编码配置
    参    数： const TVideoParam &rCfg
    返 回 值： BOOL32 
    说    明： */
	BOOL32 UpdateCfgVEnc(const TVideoParam &rCfg, const CString& szPassword = _T(""));

    /*函数名： UpdateCfgAVProtocol
    功    能： 视频编码配置
    参    数： const TAVPriorStrategy &rCfg
    返 回 值： BOOL32 
    说    明： */
	BOOL32 UpdateCfgAVProtocol(const TAVPriorStrategy &rCfg, const CString& szPassword = _T(""));

    /*函数名： UpdateCfgAVProtocol
    功    能： 视频编码配置
    参    数： const TVideoParam &rCfg
    返 回 值： BOOL32 
    说    明： */
	BOOL32 UpdateCfgH264(const TVideoParam &rCfg, const CString& szPassword = _T(""));

    /*函数名： UpdateCfgVCap
    功    能： 视频采集配置
    参    数： const _TPCCODEC &rCfg
    返 回 值： BOOL32 
    说    明： */
//	BOOL32 UpdateCfgVCap(const _TPCCODEC &rCfg, const CString& szPassword = _T(""));

    /*函数名： UpdateCfgSendRt
    功    能： 发送重传配置
    参    数： const TRetransCfg &rCfg
    返 回 值： BOOL32 
    说    明： */
	BOOL32 UpdateCfgSendRt(const TLostPackResend &rCfg, const CString& szPassword = _T(""));

    /*函数名： UpdateCfgPanel
    功    能： 更新流媒体配置
    参    数： const TStreamMedia &tCfg
    返 回 值： BOOL32 
    说    明： */
    BOOL32 UpdateCfgPanel(const TStreamMedia &tCfg, const CString& szPassword = _T(""));

       /*************系统检测接口*****************/
	/*函数名： SelfLoop
    功    能： 自环
    参    数： BOOL32 bStart TRUE 开始,FALSE 停止
    返 回 值： BOOL32 成功返回TRUE，失败返回FALSE
    说    明： */
	BOOL32 SelfLoop(BOOL32 bStart);


	/********************地址簿操作****************************************************/
    /*函数名： AddEntry
    功    能： 增加条目
    参    数： CString &szEntryName 条目名称
               u32 dwIP 主机序IP地址
               CString &szAlias
               CString &szE164
               u16 wBitRate
    返 回 值： BOOL32 成功返回TRUE，失败返回FALSE
    说    明： */
	BOOL32 AddEntry(const CString &szEntryName, u32 dwIP,const CString &szAlias,
		            const CString &szE164, u16 wBitRate);

    /*函数名： ModifyEntry
    功    能： 修改条目
    参    数： CString &szOldEntryName 原来的名称
               CString &szEntryName    修改后的名称
               DWORD dwIP
               CString &szAlias
               CString &szE164
               u16 wBitRate
    返 回 值： BOOL32 
    说    明： */
	BOOL32 ModifyEntry(const CString &szOldEntryName,const  CString &szEntryName, 
		               DWORD dwIP, const CString &szAlias,const  CString &szE164, 
					   u16 wBitRate);
	 
    /*函数名： DeleteEntry
    功    能： 删除条目
    参    数： CString &szEntryName 条目名称
    返 回 值： BOOL32 
    说    明： */
	BOOL32 DeleteEntry(const CString &szEntryName);

    /*函数名： AddGroup
    功    能： 增加组
    参    数： CString &szGroupName  组名
               DWORD dwIP IP
    返 回 值： BOOL32 
    说    明： */
	BOOL32 AddGroup(const CString &szGroupName, DWORD dwIP);

    /*函数名： ModifyGroup
    功    能： 修改组
    参    数： CString &szOldGroupName 原来的组名
               CString &szGroupName    修改后的组名
               DWORD dwIP
    返 回 值： BOOL32 
    说    明： */
	BOOL32 ModifyGroup(const CString &szOldGroupName, const CString &szGroupName, DWORD dwIP);

    /*函数名： DeleteGroup
    功    能： 删除组
    参    数： CString szGroupName
    返 回 值： BOOL32 
    说    明： */
	BOOL32 DeleteGroup(const CString &szGroupName);

    /*函数名： AddEntryToGroup
    功    能： 添加条目到组
    参    数： CString &szGroupName
               vector<CString> vctAddrEntry
    返 回 值： BOOL32 
    说    明： */
    BOOL32 AddEntryToGroup(const CString &szGroupName, vector<CString>& vctAddrEntry);

    /*函数名： DelEntryFromGroup
    功    能： 从组删除条目
    参    数： CString &szGroupName 
               vector<CString> vctAddrEntry 条目名
    返 回 值： BOOL32 
    说    明： */
    BOOL32 DelEntryFromGroup(const CString &szGroupName, vector<CString>& vctAddrEntry);

    /*函数名： ClearEntry
    功    能： 清空条目
    参    数： 
    返 回 值： BOOL32 
    说    明： */
    BOOL32 ClearEntry();

    /*函数名： ClearGroup
    功    能： 清空会议组
    参    数： 
    返 回 值： BOOL32 
    说    明： */
    BOOL32 ClearGroup();

    /*函数名： ModulateEncVolume
    功    能   调节输出音量
    参    数   u32 dwVolume 调节后的音量
    返 回 值   BOOL32
	说    明   */
	BOOL32 ModulateEncVolume(u32 dwVolume);

    /*函数名： ModulateDecVolume
    功    能   调节输入音量
    参    数   u32 dwVolume 调节后的音量
    返 回 值   BOOL32
	说    明   */
	BOOL32 ModulateDecVolume(u32 dwVolume);

    //************数据会议操作接口************************************************// 

    /*函数名： DCCreateConf
    功    能   创建数据会议
    参    数   szConfName：         会议的名称
			   szConfPwd :          会议密码（没有密码时，可设为NULL或者""）
			   bAppShareDisable:    其他与会者是否禁止发启共享
               bWhiteBoardDisable： 其他与会者是否禁止发启白板
               bFileTransDisable:   其他与会者是否禁止发启文件传输
               bChatDisable:        其他与会者是否禁止发启聊天
    返 回 值   BOOL32
			   TRUE：  成功主持会议 
			   FALSE： 失败
	说    明   */      
	BOOL32 DCCreateConf(const CString& szConfName, 
						const CString& szConfPwd, 
						BOOL32 bAppShareDisable,
						BOOL32 bWhiteBoardDisable, 
						BOOL32 bFileTransDisable, 
						BOOL32 bChatDisable);

    /*函数名： DCCall
    功    能   数据会议呼叫
    参    数   const CString& szIPAddress 被叫终端的IP地址
    返 回 值   BOOL32
	说    明   */
	BOOL32 DCCall( const CString& szIPAddress );

	/*函数名： DCEjectMt
    功    能   数据会议请退终端
    参    数   const CString& szIPAddress 要请退的终端的IP地址
    返 回 值   BOOL32
	说    明   */
	BOOL32 DCEjectMt (const CString& szMtName);

	/*函数名： DCJoinReqConfirm
    功    能   加入会议确认
    参    数   const CString& szMtName 申请加入的终端名称
			   BOOL32 bAccept 是否接受
    返 回 值   BOOL32
	说    明   */
	BOOL32 DCJoinReqConfirm(const CString& szMtName, BOOL32 bAccept);

	/*函数名： DCInviteReqConfirm
    功    能   加入会议确认
    参    数   const CString& szMtName 发出邀请的终端名称
			   BOOL32 bAccept 是否接受
    返 回 值   BOOL32
	说    明   */
	BOOL32 DCInviteReqConfirm(const CString& szMtName, BOOL32 bAccept);

	/*函数名： DCJoinConf
    功    能   接受邀请确认
    参    数   const CString& szConfName 会议名称
			   const CString& szConfPwd 会议密码
    返 回 值   BOOL32
	说    明   */
	BOOL32 DCJoinConf (const CString& szConfName, const CString& szConfPwd);

	/*函数名： DCInputConfPwd
    功    能   加入会议
    参    数   const CString& szConfPwd 会议密码
    返 回 值   BOOL32
	说    明   */
	BOOL32 DCInputConfPwd (const CString& lpszConfPwd);
	
	/*函数名： DCLeaveConf
    功    能   退出会议
    参    数   无
    返 回 值   BOOL32
	说    明   如果是本终端创建的会议，会结束本次会议*/
	BOOL32 DCLeaveConf();

	/*函数名： DCStartShare
    功    能   启动程序共享
    参    数   无
    返 回 值   BOOL32
	说    明   */
	BOOL32 DCStartShare();

	/*函数名： DCSwitchShareWindow
    功    能   切换到共享窗口
    参    数   无
    返 回 值   BOOL32
	说    明   */
	BOOL32 DCSwitchShareWindow();

	/*函数名： DCStartWhiteBoard
    功    能   开启白板应用
    参    数   无
    返 回 值   BOOL32
	说    明   */
	BOOL32 DCStartWhiteBoard();

	/*函数名： DCSwitchWhiteBoardWindow();
    功    能   切换到白板窗口
    参    数   无
    返 回 值   BOOL32
	说    明   */
	BOOL32 DCSwitchWhiteBoardWindow ();

	/*函数名： DCStartFileTransfer
    功    能   开启文件传输
    参    数   无
    返 回 值   BOOL32
	说    明   */
	BOOL32 DCStartFileTransfer();

	/*函数名： DCStartChat
    功    能   开启聊天
    参    数   无
    返 回 值   BOOL32
	说    明   */
	BOOL32 DCStartChat();

	/*函数名： DCStopChat
    功    能   关闭聊天
    参    数   无
    返 回 值   BOOL32
	说    明   */
	BOOL32 DCStopChat();

	/*函数名： DCSendPublicChatMsg
    功    能   发送公聊消息
    参    数   const CString& szMsg 消息内容
    返 回 值   BOOL32
	说    明   */
	BOOL32 DCSendPublicChatMsg (const CString& szMsg);

	/*函数名： DCSendPrivateChatMsg
    功    能   发送私聊消息
    参    数   const CString& szMtName 目的终端
			   const CString& szMsg 消息内容
    返 回 值   BOOL32
	说    明   */
	BOOL32 DCSendPrivateChatMsg(const CString& szMtName, const CString& szMsg);

	/*函数名： DCSetConfigInfo
    功    能   设置配置参数
    参    数   szDefaultFileReceivePath    文件传输接受文件的缺省保存路径
               szDefaultHostConfName       自动创建会议时，创建会议的缺省名称
               bDefaultStartColorShare     启动共享时缺省是否真彩色共享
    返 回 值   BOOL32
	说    明   */
	BOOL32 DCSetConfigInfo(const CString& szDefaultFileReceivePath,
						   const CString& szDefaultHostConfName,
						   BOOL32 bDefaultStartColorShare);

	/*函数名： DCSetMaxBandwidth
    功    能   设置终端最大码流(带宽)
    参    数   u32 dwBandwidth 带宽
    返 回 值   BOOL32
	说    明   */
	BOOL32 DCSetMaxBandwidth(u32 dwBandwidth);
	
	/*函数名： DCSwitchLanguage
    功    能   中英文切换
    参    数   u8 byType 0 中文 1 英文
    返 回 值   BOOL32
	说    明   */
	BOOL32 DCSwitchLanguage(u8 byType);

private:
	inline void SLEEP(u32 dwSeconds)
	{
		if(m_bDelay)
		{
			::Sleep(dwSeconds);
		}
	}

private:	
    CGuiUnit			m_cGuiUnit;   //界面操作接口类对象
    BOOL32              m_bResult;    //界面操作结果
    HWND                m_hWndIE;     //IE或者容器窗口句柄

private:
    u8                  m_byUIIndex;
    BOOL32              m_bDelay;
private:
    HWND                m_hMainFrame;

	HWND                m_hBtnApplyChairman;
	HWND                m_hBtnApplySpeaker;
	HWND                m_hBtnApplyChime;

    //button
    HWND                m_hBtnHelp;
    HWND                m_hBtnMonitor;
    HWND                m_hBtnAddressBook;
	
	//local & remote video
	HWND                m_hLocalVideo;
	HWND                m_hRemoteVideo;

    //分模块
	HWND                m_hTab;
    HWND                m_hBroadCastWnd;
    HWND                m_hSimRoomWnd;
    HWND                m_hDCShareWnd;
    HWND                m_hDCWhiteBoardWnd;
    HWND                m_hSMS;
    HWND                m_hMonitorPanel;
    HWND                m_hAddressbook;

    //p2p
    HWND                m_hBtnCall;
    HWND                m_hP2PAddress;

    //conflist
    HWND                m_hBtnCreate;
    HWND                m_hBtnEnd;
    HWND                m_hBtnJoin;
    HWND                m_hBtnInvite;
    HWND                m_hMtlist;

    HWND                m_hConfig;
    HWND                m_hStatistic;
    HWND                m_hFullScreen;
    HWND                m_hSnapshot;
    HWND                m_hSelfTest;
    HWND                m_hMute;
    HWND                m_hSilent;
    HWND                m_hPlayVol;
    HWND                m_hCaptureVol;
    HWND                m_hPlayWnd;

    //simroom
    HWND                m_hListPlatform;
    HWND                m_hListAudience;

    //short message
    HWND                m_hBtnSend;
    HWND                m_hComboMt;
    HWND                m_hEditMessage;
    HWND                m_hEditDisplay;

    //monitor dialog
    HWND                m_hSource1;
    HWND                m_hSource2;

    //monitor panel
    HWND                m_hMonitorWnd[9];
    HWND                m_hBtnMonitorStyle[6];
};

#endif