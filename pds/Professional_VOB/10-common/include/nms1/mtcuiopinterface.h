/*****************************************************************************
  模块名      : 总的操作接口
  文件名      : MtcUIOPInterface.h
  相关文件    : 
  文件实现功能: 单元测试
  作者        : 王慧琛
  版本        : V3.5  Copyright(C) 1997-2005 KDC, All rights reserved.
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2004/11/9   3.5         王慧琛      创建
******************************************************************************/
#if !defined(AFX_UIOPINTERFACE_H__57F5B3B1_71A8_4327_8ADC_7BB80C75C02F__INCLUDED_)
#define AFX_UIOPINTERFACE_H__57F5B3B1_71A8_4327_8ADC_7BB80C75C02F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//分类接口
#include "cppunit\GuiUnit.h"

#include <vector>

#include "kdvType.h"		// 公共类型
#include "kdvDef.h"			// 公共宏
#include "osp.h"			// OSP业务
#include "mtstruct.h"		// 终端控制业务
#include "ummessagestruct.h"// 身份验证业务

#include "AddrBook.h"

#define DELAYTIME 500

#define MTCUILIBASSERT(bRet)            \
    {                                   \
        if (bRet == FALSE) return FALSE;\
    }

#define MTCUIHWNDASSERT(hWnd)           \
    {                                   \
        if (hWnd == NULL) return FALSE; \
    }


using namespace std;
typedef struct tagMtConfigInfo
{
	CString strName;
	CString strIP;
	CString strLocalAddr;
}TMtConfigInfo;

//总的操作接口
class CMtcUIOPInterface  
{
	
public:
	enum 
	{
		emBannerOne = 0, 
		emBannerTwo
	};

    CMtcUIOPInterface();
	virtual ~CMtcUIOPInterface();	

protected:
    //*****内部用的操作接口************************************************************//
    /*函数名： GuiActiveWindow
    功    能： 激活终端控制台窗口
    参    数： 
    返 回 值： BOOL 成功返回TRUE，失败返回FALSE
    说    明： */
    BOOL32 GuiActiveWindow(); 

    /*函数名： MoveToBtn
    功    能： 移动并单击主界面的某个按钮
    参    数： HWND hWnd 按钮句柄
    返 回 值： BOOL 是否成功
    说    明： */
    BOOL32 MoveToBtn(HWND hWnd); 
	
	/*函数名： MovetoTextInput
    功    能： 移动并在Text控件中输入文字
    参    数： HWND hWnd 按钮句柄
    返 回 值： BOOL 是否成功
    说    明： */
	BOOL32 MovetoTextInput(const CString &cs, HWND hWnd)
	{
		m_cGuiUnit.MouseMove(hWnd);
		m_cGuiUnit.TextInput(cs, hWnd);
		Sleep(DELAYTIME);
		return TRUE;
	}
	/*函数名： SpecialDeal
    功    能： 异常处理
    参    数： HWND hWnd 按钮句柄
    返 回 值： BOOL 是否成功
    说    明： */
	BOOL32 SpecialDeal()
	{
		HWND hPopEx = m_cGuiUnit.GetPopWindow();
		if (hPopEx != NULL)
		{
			HWND hWnd = m_cGuiUnit.GetHWnd(hPopEx, IDOK);
			MoveToBtn(hWnd);
			return FALSE;
		}
		return TRUE;
	}

    /*函数名： InitMtcHandle
    功    能： 初始化句柄
    参    数： 
    返 回 值： BOOL 
    说    明： */
    BOOL32 InitMtcHandle();    

    /*函数名： IsInP2PConf
    功    能： 是否在点对点会议中，或者在呼叫中
    参    数： 
    返 回 值： BOOL 
    说    明： */
    BOOL32 IsInP2PConf();

    /*函数名： IsInMultiConf
    功    能： 是否在多点会议中
    参    数： 
    返 回 值： BOOL 
    说    明： */
    BOOL32 IsInMultiConf();

	/*函数名： IsInConf
    功    能： 是否在会议中
    参    数： 
    返 回 值： BOOL 
    说    明： */
    BOOL32 IsInConf();

    /*函数名： IsChairMan
    功    能： 是否主席
    参    数： 
    返 回 值： bool 
    说    明： */
	BOOL32 IsChairMan();

    /*函数名： IsSpeaker
    功    能： 是发言人
    参    数： 
    返 回 值： bool 
    说    明： */
	BOOL32 IsSpeaker();

    /*函数名： GetConnectMt
    功    能： 获得当前所连着的终端的别名
    参    数： 
    返 回 值： CString 
    说    明： */
    CString GetConnectMt();

    /*函数名： GetSpeaker
    功    能： 
    参    数： CString &strSpeaker
    返 回 值： 
    说    明： */
    CString GetSpeaker();

    /*函数名： GetChairman
    功    能： 
    参    数： CString &strChairman
    返 回 值：  
    说    明： */
    CString GetChairman();

    /*函数名： SwitchSheet
    功    能： 切换到当前sheet
    参    数： int nCurSheet
    返 回 值： BOOL32 
    说    明： */
    BOOL32 SwitchSheet(INT nCurSheet);

    /*函数名： GetCurSheet
    功    能： 得到当前的sheet
    参    数： 
    返 回 值： INT 
    说    明： */
    INT GetCurSheet();
    /*函数名： ConfigE1Module
    功    能： 配置一个E1模块
    参    数： 
    返 回 值： BOOL32 
    说    明： */
	BOOL32 ConfigE1Module(const TE1Unit& tE1Module);
	/*函数名： ComboxSelect
    功    能： 下拉列表的选取
    参    数： 
    返 回 值： BOOL32 
    说    明： */
	BOOL32 ComboxSelect(HWND hWnd, u32 dwData);
	/*函数名： ComboxSelect
    功    能： 下拉列表的选取
    参    数： 
    返 回 值： BOOL32 
    说    明： */
	BOOL32 ComboxSelect(HWND hWnd, CString csData);

	/*函数名： MoveToCheck
    功    能： 移到复选框并单击
    参    数： 
    返 回 值： void 
    说    明： */
	void MoveToCheck(HWND hWnd, BOOL32 bCheck)
	{
		if (IsWindowEnabled(hWnd))
		{
			if (bCheck != m_cGuiUnit.ButtonGetCheck(hWnd))
			{
				MoveToBtn(hWnd);
				Sleep(DELAYTIME);
			}
		}
	}
	/*函数名： TextInPut
    功    能： 在编辑框输入文字
    参    数： 
    返 回 值： void 
    说    明： */
	void TextInPut(HWND hWnd, CString csIn)
	{
		if (::IsWindowEnabled(hWnd))
		{
			CString csGet;
			m_cGuiUnit.TextGet(csGet, hWnd);
			if (strcmp(csGet, csIn)) 
			{
				m_cGuiUnit.MouseMove(hWnd);
				Sleep(DELAYTIME);
				m_cGuiUnit.TextInput(csIn, hWnd);
				Sleep(DELAYTIME);
			}
		}
		
	}
	/*函数名： TextInPut
    功    能： 在编辑框输入文字
    参    数： 
    返 回 值： void 
    说    明： */
	void TextInPut(HWND hWnd, u32 dwData)
	{
		CString csIn;
		csIn.Format(_T("%d"), dwData);
		TextInPut(hWnd, csIn);
	}
	/*函数名： SetIpAddr
    功    能： 设置IP地址
    参    数： 
    返 回 值： void 
    说    明： */
	void SetIpAddr(HWND hWnd, u32 dwIp)
	{
		if (::IsWindowEnabled(hWnd))
		{
			u32 dwGetIP = 0;
			m_cGuiUnit.IpAddressGet(&dwGetIP, hWnd);
			if (dwGetIP != dwIp)
			{
				m_cGuiUnit.MouseMove(hWnd);
				Sleep(DELAYTIME);
				m_cGuiUnit.IpAddressSet(dwIp, hWnd);
				Sleep(DELAYTIME);
			}
		}
	}
	/*函数名： ConfigApply
    功    能： 在配置页点击应用
    参    数： 
    返 回 值： BOOL32
    说    明： */
	BOOL32 ConfigApply()
	{
		HWND hWnd = m_cGuiUnit.GetHWnd(20041, CBUTTON, 448);
		MTCUIHWNDASSERT(hWnd);
		MoveToBtn(hWnd);
		Sleep(DELAYTIME);
		return TRUE;
	}

	/*函数名： SetSliderPos
    功    能： 设置slide控件的位置
    参    数： 
    返 回 值： BOOL32
    说    明： */
	void SetSliderPos(HWND hWnd, s32 nPos)
	{
		s32 nGetPos;
		m_cGuiUnit.SliderGetPos(&nGetPos, hWnd);
		if (nGetPos != nPos)
		{
			m_cGuiUnit.MouseMove(hWnd);
			Sleep(DELAYTIME);
			m_cGuiUnit.SliderSetPos(nPos, hWnd);
			Sleep(DELAYTIME);
		}
	}
public:
    //*****提供给外部调用的接口******//

    //*****系统初始化的操作接口********************************************************//
   
    /*函数名： GuiInit
    功    能： 初始化MTC
    参    数： CString strExePath   容器路径
    返 回 值： BOOL32 
    说    明： */
    BOOL32 GuiInit(CString strExePath);
	
    /*函数名： LoginMt
    功    能： 登录终端
    参    数： CString szMtIP       要连接的终端IP
               CString strLoginName 登入用户名
               CString strLoginPwd  登入密码
               BOOL32  bNeverSleep  是否进入永不待机状态
    返 回 值： BOOL32 
    说    明： */
    BOOL32 LoginMt(const CString& szMtIP,const CString& strLoginName = _T("admin"),
		const CString& strLoginPwd  = _T("admin"),BOOL32 bNeverSleep = TRUE);
    
	/*函数名： InitOP
    功    能： 再次初始话界面库
    参    数： CString strTitle 窗口名称
    返 回 值： BOOL32 
    说    明： */
    BOOL32 InitOP(CString strTitle);
    
    /*函数名： LoginMtc
    功    能： 连接终控
    参    数： CString strLoginName  用户名
               CString strLoginPwd   用户密码
    返 回 值： BOOL32 
    说    明： */
 	BOOL32 LoginMtc(CString strLoginName, CString strLoginPwd);
    
    /*函数名： Refresh
    功    能： 重刷界面，以便重新登陆
    参    数： 
    返 回 值： BOOL32 
    说    明： */
    BOOL32 Refresh();

    /*函数名： close
    功    能： 关闭测试接口清理现场
    参    数： 
    返 回 值： void 
    说    明： 退出时最好调用以便关闭测试对象*/
    void Close();

    /*函数名： IsConnected
    功    能： 是否已经连上终端
    参    数： 
    返 回 值： BOOL 
    说    明： */
    BOOL IsConnected(); 
    
    //**********主界面测试接口********************************************************//

    //头部按钮功能
    /*函数名： AboutVer
    功    能： 关于版本信息
    参    数： 
    返 回 值： 
    说    明： */
    BOOL AboutVer();

    /*函数名： AboutHelp
    功    能： 关于帮助
    参    数： 
    返 回 值： 
    说    明： */
    BOOL AboutHelp();

    /*函数名： TerminalSnapshot
    功    能： 终端快照
    参    数： 
    返 回 值： BOOL32 
    说    明： */
    BOOL32 TerminalSnapshot();

    /*函数名： ClearMtSnapshot
    功    能： 清空终端上的快照
    参    数： 
    返 回 值： BOOL32 
    说    明： */
    BOOL32 ClearMtSnapshot();

    /*函数名： DeleteMtSnapshot
    功    能： 删除一条终端上的快照
    参    数： s32 nItem 快照索引
    返 回 值： BOOL32 
    说    明： */
    BOOL32 DeleteMtSnapshot(s32 nItem);

    /*函数名： AddrbookShow
    功    能： 地址薄管理
    参    数： 
    返 回 值： BOOL32 
    说    明： */
    BOOL32 AddrbookShow();
	
     /*函数名： DualStream
    功    能： 双流
    参    数： s32 nType = 双流类型 0～2 位各种双流类型 －1未停止双流
    返 回 值： BOOL32 
    说    明： */
    BOOL32 DualStream(s32 nType = 0);
	
	/*函数名： StopDualStream
    功    能： 双流
    参    数： s32 nType = 双流类型 0～2 位各种双流类型 －1未停止双流
    返 回 值： BOOL32 
    说    明： */
	BOOL32 StopStream();

    /*函数名： Reset
    功    能： 复位
    参    数： 
    返 回 值： BOOL32 
    说    明： */
    BOOL32 Reset();

    /*函数名： SleepMt
    功    能： 待机
    参    数： DWORD dwType 待机类型 0 立即 1－60分种后， 2－120分种后，
                                            3－180分种后， 4－永不待机 
               BOOL32 bSleep = TRUE 是否休眠
    返 回 值： BOOL32 
    说    明： */
    BOOL32 SleepMt(DWORD dwType= 0, BOOL32 bSleep = TRUE);

    /*函数名： LocalMonitor
    功    能： 本地监控
    参    数： 
    返 回 值： BOOL32 
    说    明： */
    BOOL32 LocalMonitor();

    /*函数名： Dummy
    功    能： 哑音
    参    数： 
    返 回 值： BOOL32 
    说    明： */
    BOOL32 Dummy();

    /*函数名： Silent
    功    能： 静音
    参    数： 
    返 回 值： BOOL32 
    说    明： */
    BOOL32 Silent();

    /*函数名： RemoteMonitor
    功    能： 远端监控
    参    数： 
    返 回 值： BOOL32 
    说    明： */
    BOOL32 RemoteMonitor();

    /*函数名： LocalFullScreen
    功    能： 本地全屏
    参    数： 
    返 回 值： BOOL32 
    说    明： */
    BOOL32 LocalFullScreen();

    /*函数名： RemoteFullScreen
    功    能： 远端全屏
    参    数： 
    返 回 值： BOOL32 
    说    明： */
    BOOL32 RemoteFullScreen();

    /*函数名： RemoteSnap
    功    能： 远端抓拍
    参    数： CString strPath=_T("") 保存路径，不填的话保存到默认路径
    返 回 值： BOOL32 
    说    明： */
    BOOL32 RemoteSnap(CString strPath=_T(""));

    /*函数名： LocalSnap
    功    能： 远端抓拍
    参    数： CString strPath=_T("") 保存路径，不填的话保存到默认路径
    返 回 值： BOOL32 
    说    明： */
    BOOL32 LocalSnap(CString strPath=_T(""));

    //************点对点会议测试*****************************************************// 
    
    /*函数名： CallbyMT()
    功    能： 通过地址条目呼叫其它终端
    参    数： TMtAddr tTermAddr 终端呼叫结构 
				u16               wBitRate 呼叫速率
               BOOL32 bAddrbook  TRUE 通过地址簿来呼叫 
			   DWORD dwSleepTime 休眠等待时间,如果休眠时间不为0，则会判断是否加入了会议
    返 回 值： BOOL32 成功返回TRUE，失败返回FALSE
    说    明： */
	BOOL32 CallbyMT(TMtAddr tTermAddr, u16 wBitRate, BOOL32 bAddrbook = FALSE, DWORD dwSleepTime=15000);
	
    
    /*函数名： CallbyMTEx()
    功    能： 通过地址条目呼叫其它终端
    参    数： TMtAddr tTermAddr 终端呼叫结构 u16 
               wBitRate 呼叫速率
               BOOL32 bAddrbook  TRUE 通过地址簿来呼叫 
    返 回 值： BOOL32 成功返回TRUE，失败返回FALSE
    说    明： */
	BOOL32 CallbyMTEx(TMtAddr tTermAddr, u16 wBitRate, BOOL32 bAddrbook = FALSE);

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
    参    数： u32 dwMcuIp					[in]Mcu的Ip地址
			   const TConfBaseInfo &info			[in]会议信息
			   const vector<TMtAddr>& vMts	[in]终端信息
               CString strGroupName = "" 组条目名称
               BOOL32 bByAddrbook 通过地址簿
			   DWORD dwSleepTime 休眠等待时间，默认值为20秒
    返 回 值： BOOL32 成功返回TRUE，失败返回FALSE
    说    明： */
	BOOL32 CreateConf(u32 dwMcuIp, CString csBitrate,
        const TConfBaseInfo &info, const vector<TMtAddr>& vMts,
        CString strGroupName = "", BOOL32 bByAddrbook = FALSE); 

   BOOL32 CreateConf(TMtAddr tMcuInfo, CString u16Bitrate,
   const  TConfBaseInfo &info, const vector<TMtAddr>& vMts,
   CString strGroupName = "", BOOL32 bByAddrbook = FALSE, DWORD dwSleepTime=20000); 
   
   //不等待回应就返回
   BOOL32 CreateConfEx(TMtAddr tMcuInfo, CString u16Bitrate,
   const TConfBaseInfo &info, const vector<TMtAddr>& vMts,
   CString strGroupName = "", BOOL32 bByAddrbook = FALSE);

    /*函数名： JoinConf
    功    能： 终端申请加入会议
    参    数： const TMtAddr& mcu [in]MCU的地址
               u16 &wBitRate 呼叫速率
               CString strConfName 会议名称
               CString strPWD 会议密码
    返 回 值： BOOL32 
    说    明： */
	BOOL32 JoinConf(const TMtAddr& mcu, u16 &wBitRate,
                    CString strConfName=_T(""), CString strPWD = "123");

    /*函数名： SelectConf
    功    能： 
    参    数： char *ConfName
    返 回 值： BOOL32 
    说    明： */

	BOOL32 SelectConf(CString strConfName);
	/*函数名： InputConfPass
    功    能： 
    参    数： char *strPass
    返 回 值： BOOL32 
    说    明： */
    BOOL32 InputConfPass(char *strPass);		
	
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
    /*函数名： InviteMt
    功    能： 邀请终端
    参    数： const TMtAddr& tMt
               u16 &wBitRate
    返 回 值： BOOL32 
    说    明： */
    BOOL32 InviteMt(const TMtAddr& tMt, u16 &wBitRate);

    /*函数名： RevokeMt
    功    能： 请退终端
    参    数： CString &szMt
    返 回 值： BOOL32 
    说    明： */
    BOOL32 RevokeMt(CString &szMt);

    /*函数名： AcceptInvite
    功    能： 接收邀请
    参    数： BOOL32 bAccepted == TRUE 接收 FALSE 拒绝
    返 回 值： BOOL32 
    说    明： */
    BOOL32 AcceptInvite(BOOL32 bAccepted = TRUE);

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
	BOOL32 SpecifySpeaker(CString &szMt);

    /*函数名： SpecifyChairman
    功    能： 指定主席终端
    参    数： CString &szMt
    返 回 值： BOOL32 
    说    明： */
	BOOL32 SpecifyChairman(CString &szMt);

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

    /*函数名： 调节画中画开启/关闭和位置移动
    功    能： 
    参    数： 
    返 回 值： BOOL32 
    说    明： */
	BOOL32 PipMove();
    /*函数名：  画中画的大小画面互换
    功    能： 
    参    数： 
    返 回 值： BOOL32 
    说    明： */
	BOOL32 PipSwitch();
	/*函数名： StartDiscuss
    功    能： 开始讨论模式
    参    数： int u8Max 讨论的深度
    返 回 值： BOOL32 
    说    明： */
	BOOL32 StartDiscuss(int u8Max);

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
    参    数： u8 nType 复合类型 如果未0的话就是自动复合
               vector<CString> &vctMT 参与复合的终端
    返 回 值： BOOL32 
    说    明： */
	BOOL32 StartVMP(u8 nType, vector<CString> &vctMt);

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

 
    /*函数名： ChangeVMPMember
    功    能： 修改视频复合参数
    参    数： int nIndex  窗口序号 从0开始
               int nType   复合成员类型
               0, 为关闭
               VMP_MEMBERTYPE_MCSSPEC           1  //会控指定 
               VMP_MEMBERTYPE_SPEAKER           2  //发言人跟随
               VMP_MEMBERTYPE_CHAIRMAN          3  //主席跟随
               VMP_MEMBERTYPE_POLL              4  //轮询视频跟随
    返 回 值： BOOL32 
    说    明： */
	BOOL32 ChangeVMPMember(int nIndex, int nType);

    /*函数名： SelectMt
    功    能： 选看终端
    参    数： CString &strMt 
               INT nType选看类型
    返 回 值： BOOL32 
    说    明： */
    BOOL32 SelectMt(CString &strMt, INT nType);

    /*函数名： CancelSelectMT
    功    能： 取消选看
    参    数： CString &strMt
    返 回 值： BOOL32 
    说    明： */
    BOOL32 CancelSelectMT(CString &strMt);
    
    /*函数名： StartPoll
    功    能： 开始轮询
    参    数： INT nInterval 轮询间隔
               INT nType     轮询类型
               vector<CString> &vctMt轮询终端
    返 回 值： BOOL32 
    说    明： */
    BOOL32 StartPoll(u8 byInterval, u8 byType, vector<CString> &vctMt);

    /*函数名： StopPoll
    功    能： 停止轮询
    参    数： 
    返 回 值： BOOL32 
    说    明： */
    BOOL32 StopPoll();

    /*函数名： PausePoll
    功    能： 暂停轮询
    参    数： 
    返 回 值： BOOL32 
    说    明： */
    BOOL32 PausePoll();

    /*函数名： ResumePoll
    功    能： 继续轮询
    参    数： 
    返 回 值： BOOL32 
    说    明： */
    BOOL32 ResumePoll();

    /*函数名： SendMessage
    功    能： 向与会终端发送短消息
    参    数： vector<CString> &vctMt 接收终端
               string strMsg 消息内容
               BOOL32 bSelectAll 全选
    返 回 值： BOOL32 
    说    明： */
	BOOL32 SendMessage(vector<CString> &vctMt, CString strMsg, BOOL32 bSelectAll = FALSE);

    /*函数名： AcceptMt
    功    能： 答应终端 适用于其他终端申请发言、主席、插话时弹出对话框的确认
    参    数： CString strMtName 终端名字
               BOOL32 bAccept  TRUE接受, FALSE拒绝
    返 回 值： BOOL32 
    说    明： */
	BOOL32 AcceptMt(CString strMtName, BOOL32 bAccept=TRUE);
    
    /*函数名： RemoteDummy
    功    能： 主席远端哑音
    参    数： CString strMt
    返 回 值： BOOL32 
    说    明： */
    BOOL32 RemoteDummy(CString strMt);

    /*函数名： RemoteSilent
    功    能： 主席远端静音
    参    数： CString strMt
    返 回 值： BOOL32 
    说    明： */
    BOOL32 RemoteSilent(CString strMt);


    //************终端配置************************************************// 

    /*函数名： UpdateCfgH323
    功    能： H323配置
    参    数： const TH323Cfg &tCfg(全部)
    返 回 值： BOOL32 
    说    明： */
	BOOL32 UpdateCfgH323(const TH323Cfg &tCfg);

	/*函数名： UpdateAVPrior
    功    能： 更新视音频优选协议
    参    数： const TAVPriorStrategy &tCfg(全部)
    返 回 值： BOOL32 
    说    明： */
	BOOL32 UpdateAVPrior(const TAVPriorStrategy &tCfg);
    /*函数名： UpdateCfgOSD
    功    能： 台标配置
    参    数： const TOsdCfg &tCfg(全部)
    返 回 值： BOOL32 
    说    明： */	
	BOOL32 UpdateCfgOSD(const TOsdCfg &tCfg);

   /*函数名： ChangeSign
    功    能： 更换台标
    参    数： CString strBMPPath
    返 回 值： BOOL32 
    说    明： */
    BOOL32 ChangeSign(CString strBMPPath);

    /*函数名： UpdateCfgNet
    功    能： 以太网配置
    参    数： const TEthnetInfo &tCfg(全部)
				u8  byIndex    网卡号(一般终端也只有一个网卡)
    返 回 值： BOOL32 
    说    明： */
	BOOL32 UpdateCfgNet(const TEthnetInfo &tCfg, u8 byIndex = 0);

	/*函数名： UpdateCfgTrap
    功    能： SNMP配置
    参    数： const TSNMPCfg &tCfg(全部)
    返 回 值： BOOL32 
    说    明： */
    BOOL32 UpdateCfgTrap(const TSNMPCfg &tCfg);

    /*函数名： UpdateCfgUser
    功    能： 用户配置
    参    数： const TUserCfg &rCfg(除了是否允许远摇的其他全部)
    返 回 值： BOOL32 
    说    明： */
	BOOL32 UpdateCfgUser(const TUserCfg &tCfg);

	/*函数名： UpdateCfgPPPoE
    功    能： PPPoE配置
    参    数： const TPPPOECfg &tCfg(全部)
    返 回 值： BOOL32 
    说    明： */
	BOOL32 UpdateCfgPPPoE(const TPPPOECfg &tCfg);

	/*函数名： updateCfgDHCP
    功    能： PPPoE配置
    参    数： const BOOL32 bUse
    返 回 值： BOOL32 
    说    明： */
	BOOL32 UpdateCfgDHCP(const BOOL32 bUse);

	/*函数名： UpdateCfgFireWall
    功    能： 防火墙配置
    参    数： const TEmbedFwNatProxy& tcfg(全部)
    返 回 值： BOOL32 
    说    明： */
	BOOL32 UpdateCfgFireWall(const TEmbedFwNatProxy& tcfg);
	
	/*函数名： UpdateCfgNapMap
    功    能： 静态映射
    参    数： const TNATMapAddr& tcfg(全部)
    返 回 值： BOOL32 
    说    明： */
	BOOL32 UpdateCfgNapMap(const TNATMapAddr& tcfg);
 
    /*函数名： UpdateCfgVEnc
    功    能： 视频编码配置
    参    数： const TVideoParam &rCfg(除了H264的选项外的全部)
				const EmVideoStandard emType  视频制式
    返 回 值： BOOL32 
    说    明： */
	BOOL32 UpdateCfgVEnc(const TVideoParam &tCfg, const EmVideoStandard emType);
    
    /*函数名： Updatecfg264VEnc
    功    能： 更新264配置
    参    数： const _TVENCPARAM &rCfg 其中只需要填写
    	        u16 wH264IKeyRate;		//h264的关键帧间隔
	            u8  byH264QualMaxValue;	//h264最大量化参数
	            u8  byH264QualMinValue;	//h264最小量化参数
                这三项
    返 回 值： BOOL32 
    说    明： */
    BOOL32 Updatecfg264VEnc(const TVideoParam &tCfg);
	/*函数名： UpdateCfgQos
    功    能： Qos配置
    参    数： const TIPQoS &rCfg
    返 回 值： BOOL32 
    说    明： */
	BOOL32 UpdateCfgQos(const TIPQoS &tCfg);

 	/*函数名： UpdateCfgAEnc
    功    能： 音频编码配置
    参    数： const u8 &byInputVol			输入音量
			   const u8 &byOutPutVol		输出音量
    返 回 值： BOOL 是否成功
    说    明： */
	BOOL32 UpdateCfgAEnc(const u8 &byInputVol);
	BOOL32 UpdateCfgADec(const u8 &byOutPutVol);
   /*函数名： UpdateCfgAEC
    功    能： AEC回声抵消配置
    参    数： BOOL32 bAEC		True 开启			FALSE		关闭
			   BOOL32 bAGC		True 开启			FALSE		关闭
    返 回 值： BOOL32 
    说    明： */
    BOOL32 UpdateCfgAEC(BOOL32 bAEC, BOOL32 bAGC);
	/*函数名： UpdateCfgSendRt
    功    能： 发送重传配置
    参    数： const TLostPackResend &rCfg
    返 回 值： BOOL32 
    说    明： */
	BOOL32 UpdateLostPackReSend(const TLostPackResend &tCfg);

    /*函数名： UpdateCfgPanel
    功    能： 更新流媒体配置
    参    数： const TStreamMedia &tCfg(全部)
    返 回 值： BOOL32 
    说    明： */
    BOOL32 UpdateCfgPanel(const TStreamMedia &tCfg);

    /*函数名： UpdateCfgMatrix
    功    能： 矩阵配置
    参    数： const _TDEVCFG
    返 回 值： BOOL32 
    说    明： */
    BOOL32 UpdateCfgMatrix(EmSerialType emType, TIPTransAddr &tIPAddr);

    /*函数名： UpdateCfgCamera
    功    能： 摄像头配置
    参    数： INT nIndex  摄像头序号 0 ~5
               const TCameraCfg &tCfg(全部)
    返 回 值： BOOL32 
    说    明： */
    BOOL32 UpdateCfgCamera(s32 nIndex, const TCameraCfg &tCfg);

    /*函数名： UpdateCfgTVOut
    功    能： 配置输出接口
    参    数： const TVideoSource &tCfg(全部)
    返 回 值： BOOL32 
    说    明： */
    BOOL32 UpdateCfgVSource(const TVideoSource &tCfg); 

    /*函数名： UpdateCfgSerialPort
    功    能： 串口配置
    参    数： const TSerialCfg &tCfg(全部)
    返 回 值： BOOL32 
    说    明： */
    BOOL32 UpdateCfgSerialPort(const EmSerialType &emType, const TSerialCfg &tCfg);
    
    /*函数名： UpdateCfgImage
    功    能： 图像设置
    参    数： const TImageAdjustParam & tCfg(全部)
    返 回 值： BOOL32 
    说    明： */
    BOOL32 UpdateCfgImage(const TImageAdjustParam & tCfg,BOOL32 bReset = FALSE);

    /*函数名： UpdaeteCfgDual
    功    能： 更新双流配置
    参    数： const _TVENCPARAM &tCfg 其中只有
               "图像最大最小量化参数"，"最大关键帧间隔"， 视频采集帧率，这四项有效
    返 回 值： 
    说    明： 结构没有定义*/
//    BOOL32 UpdateCfgDual(const TVideoParam &tCfg);

    /*函数名： UpdaeteCfgVGADual
    功    能： 更新VGA双流配置
    参    数： const  TVideoParam(除了H264的三项是无效的) 其中只有
				帧率
				帧率单位
               "最大关键帧间隔"
			   "图像最大最小量化参数"， 
               "编码方式"			emQualityFirst			0		// 质量优先
                                    emSpeedFirst			1		// 速度优先
               "帧率单位(emFrameSecond:帧/每秒 emSecondFrame:秒/每帧)"
               ，七项有效
    返 回 值： 
    说    明： */
    BOOL32 UpdateCfgVGADual(const TVideoParam &tCfg, const EmVideoStandard &emType);

    /*函数名： UpdateCfgCaption
    功    能： 更新横幅配置
    参    数： u8 byIndex		横幅1 还是横幅2
				const TBannerInfo &tCfg
               CString &strCaption  横幅内容
    返 回 值： 
    说    明： 结构没有定义*/
    BOOL32 UpdateCfgCaption(u8 byIndex, const TBannerInfo &tCfg, CString &strCaption);

    /*函数名： SetDefaultCfg
    功    能： 恢复默认值
    参    数： 
    返 回 值： BOOL32 
    说    明： */
    BOOL32 SetDefaultCfg();
	    /*函数名： AddRoute
    功    能： 增加一条路由配置 
    参    数： const tIpRouteParam& tIpRouteParam 要增加的路由配置信息(全部)
    返 回 值： BOOL32 
    说    明： */
	BOOL32 AddRoute(const TRouteCfg& tIpRouteParam);

    /*函数名： DeleteRoute
    功    能： 删除一条路由配置 
    参    数： u8 byIndex 要删除的路由配置索引
    返 回 值： BOOL32 
    说    明： */
	BOOL32 DeleteRoute(u8 byIndex);
	/*函数名： UpdateE1Chan
	功    能： 配置一条E1  通道
    参    数： const TE1Config(全部)
    返 回 值： BOOL32 
    说    明： */
	BOOL32 UpdateE1Chan(const TE1Config& tE1Cfg);
	/*函数名： DeleteE1Chan
	功    能： 删除当前E1 通道
    参    数： const 
    返 回 值： BOOL32 
    说    明： */
	BOOL32 DeleteE1Chan();
	
       /*************系统检测接口*****************/
	/*函数名： SelfLoop
    功    能： 自环
    参    数： BOOL bStart TRUE 开始,FALSE 停止
    返 回 值： BOOL32 成功返回TRUE，失败返回FALSE
    说    明： */
	BOOL32 SelfLoop(BOOL bStart);

	/*函数名： VolumnPowerTest
    功    能： 音量测试
    参    数： BOOL bStart TRUE 开始,FALSE 停止
    返 回 值： BOOL32 成功返回TRUE，失败返回FALSE
    说    明： */
	BOOL32 VolumePowerTest(BOOL bStart);

	/*函数名： ImageTest
    功    能： 图像测试
    参    数： BOOL bStart TRUE 开始,FALSE 停止
	           INT nType 测试类型
    返 回 值： BOOL32 成功返回TRUE，失败返回FALSE
    说    明： */
	BOOL32 ImageTest(BOOL bStart, INT nType);



	/*************设备控制接口*****************/

	/*函数名： RemoteCtrlClick
    功    能： 遥控器控制
    参    数： CString strBtnName按钮名称
    返 回 值： BOOL32 成功返回TRUE，失败返回FALSE
    说    明： */
	BOOL32 RemoteCtrlClick(CString strBtnName);

	/*函数名： SelectCamera
    功    能： 选择摄像头
    参    数： INT nCamera 摄像头号码 0 为远端
    返 回 值： BOOL32 成功返回TRUE，失败返回FALSE
    说    明： */
	BOOL32 SelectCamera(INT nCamera);

    /*函数名： UpdateCamPreSet
    功    能： 更新摄像头预存
    参    数： BOOL bSave TRUE保存 FALSE 载入
	           INT nPosition 预存位
    返 回 值： BOOL32 成功返回TRUE，失败返回FALSE
    说    明： */
	BOOL32 UpdateCamPreSet(BOOL bSave, INT nPosition , BOOL32 bRemoteControl = FALSE);

    /*函数名： SetMatrix
    功    能： 设置矩阵配置（连线）
    参    数： _TMATRIXINFO tMatrixInfo 只需要填写
          	             u8  byPortNum;	//端口个数
                         u8  abyConfig[MATRIX_MAX_PORT];//矩阵配置关系(索引为输出端口)
               BYTE byType=1  矩阵类型 默认为视频矩阵，目前只支持视频矩阵操作
    返 回 值： BOOL32 
    说    明： */
//    BOOL32 SetMatrix(_TMATRIXINFO tMatrixInfo, BYTE byType=1);


	/********************系统设置****************************************************/

	/*函数名： ModUser
    功    能： 修改用户
    参    数： 
    返 回 值： BOOL32 成功返回TRUE，失败返回FALSE
    说    明： */
	BOOL32 ModUser(const CUserFullInfo &user);

	/*函数名： AddUser
    功    能： 增加用户
    参    数： 
    返 回 值： BOOL32 成功返回TRUE，失败返回FALSE
    说    明： */
	BOOL32 AddUser(const CUserFullInfo &user);

	/*函数名： DelUser
    功    能： 删除用户
    参    数： 
    返 回 值： BOOL32 成功返回TRUE，失败返回FALSE
    说    明： */
	BOOL32 DelUser(const CUserFullInfo &user);

    /*函数名： AddEntry
    功    能： 增加条目
    参    数： TADDRENTRYINFO &tEntryInfo  地址条目信息
			   (里面的是否320终端和320终端别名不进行处理)
    返 回 值： BOOL32 成功返回TRUE，失败返回FALSE
    说    明： */
	BOOL32 AddEntry(const TADDRENTRYINFO &tEntryInfo);

    /*函数名： ModifyEntry
    功    能： 修改条目
    参    数： CString &strOldEntryName 原来的名称
               CString &strEntryName    修改后的名称
               DWORD dwIP
               CString &strAlias
               CString &strE164
               u16 wBitRate
    返 回 值： BOOL32 
    说    明： */
	BOOL32 ModifyEntry(const TADDRENTRYINFO& tOldInfo, const TADDRENTRYINFO& tNewInfo);

    /*函数名： DeleteEntry
    功    能： 删除条目
    参    数： CString &strEntryName 条目名称
    返 回 值： BOOL32 
    说    明： */
	BOOL32 DeleteEntry(const CString &strEntryName);
    BOOL32 DeleteEntry(const TADDRENTRYINFO &tEntyInfo);

    /*函数名： AddGroup
    功    能： 增加组
    参    数： CString &strGroupName  组名
               DWORD dwIP IP
               u16 wBitRate
    返 回 值： BOOL32 
    说    明： */
	BOOL32 AddGroup(const CString &strGroupName, DWORD dwIP, u16 wBitRate);
    /*函数名： ModifyGroup
    功    能： 修改组
    参    数： CString &strOldGroupName 原来的组名
               CString &strGroupName    修改后的组名
               DWORD dwIP
               u16 wBitRate
    返 回 值： BOOL32 
    说    明： */
	BOOL32 ModifyGroup(const CString &strOldGroupName, const CString &strGroupName, DWORD dwIP, u16 wBitRate);

    /*函数名： DeleteGroup
    功    能： 删除组
    参    数： CString strGroupName
    返 回 值： BOOL32 
    说    明： */
	BOOL32 DeleteGroup(const CString &strGroupName);

    /*函数名： AddEntryToGroup
    功    能： 添加条目到组
    参    数： CString &strGroupName
               vector<CString> vctAddrEntry
    返 回 值： BOOL32 
    说    明： */
    BOOL32 AddEntryToGroup(const CString &strGroupName,vector<TADDRENTRYINFO> &vtEntryInfo);

    /*函数名： FindEntry
    功    能： 查找条目
    参    数： CString &strGroupName(组名/MCU地址)
               CString &strEntryName(名称/别名/IP地址/号码)
    返 回 值： BOOL32 
    说    明： */
    BOOL32 FindEntry(const CString &strGroupName,const CString &strEntryName);

    /*函数名： DelEntryFromGroup
    功    能： 从组删除条目
    参    数： CString &strGroupName 
               vector<CString> vctAddrEntry 条目名
    返 回 值： BOOL32 
    说    明： */
    BOOL32 DelEntryFromGroup(const CString &strGroupName, vector<TADDRENTRYINFO> &vtEntryInfo);

    /*函数名： ClearAddrBook
    功    能： 清空条目和组
    参    数： 
    返 回 值： BOOL32 
    说    明： */
    BOOL32 ClearEntry();
    BOOL32 ClearGroup();

    /*函数名： ClearUser
    功    能： 清除所有的用户 
    参    数： 
    返 回 值： BOOL32 
    说    明： */
    BOOL32 ClearUser();

    /*函数名： DynSwitchVmpStyle
    功    能： 动态切换画面合成风格 
    参    数： u8 nType 欲切换到的画面合成风格
    返 回 值： BOOL32 
    说    明： */
	BOOL32 DynSwitchVmpStyle(u8 nType);

   
public:	
    CGuiUnit			m_cGuiUnit;   //界面操作接口类对象
    HWND                m_hWndIE;     //IE或者容器窗口句柄
    BOOL32              m_bResult;    //界面操作结果
protected:
	//终端控制台界面控键句柄
	bool bInitHandle;       //是否已经初始化句柄
    HWND m_hDgForm;			//主Dialog
    //顶头按钮
	HWND m_hBtSnapCap;		    //快照图片
    HWND m_hDualVideo;          //双流
	HWND m_hBtAddrBook;		    //地址簿
    HWND m_hSleepMt;            //待机
    HWND m_hResetMt;            //复位
    HWND m_hAbout;              //关于
	HWND m_hUpLoad;				//上传
	//导航按钮
    HWND m_hbtnManageSystem;    //系统管理 
    HWND m_hbtnManageDevice;    //设备管理
    HWND m_hbtnManageDetect;    //系统检测
    HWND m_hbtnManageConfig;    //设备设置
    HWND m_hbtnManageConf;      //会议管理
    HWND m_hstSheetTitle;       //页面提示
    //短消息
    HWND m_heditShortMsg;       //发送消息编辑窗           
    HWND m_heditMsgBoard;       //短消息显示窗          
    HWND m_heditMsgInfo;        //系统消息
    HWND m_hlbMsgMt;            //发送短消息的可选终端列表
    HWND m_hbtnPresetMsg;       //预制     
    HWND m_hbtnLoadMsg;         //载入     
    HWND m_hbtnSendSms;         //发送 

    //会议中的按钮    
    HWND m_hbtnPipSwitch;      //画面切换
    HWND m_hbtnPipMove;        //画面移动
    HWND m_hbtnVoiceSpark;     //语言激励
    HWND m_hbtnVMP;            //画面复合
    HWND m_hbtnConfDiscuss;    //会议讨论    
    HWND m_hbtnP2PCall;        //呼叫      
    HWND m_hbtnCreateConf;     //建会
    HWND m_hlsPlatform;        //主席台     
    HWND m_hlsAudience;        //观众台     
    HWND m_hstConfName;        //会议名称
    //监控
    HWND m_hbtnSnapshotRemote; //
    HWND m_hbtnSnapshotLocal;  //
    HWND m_hbtnFullRemote;     //     
    HWND m_hbtnFullLocal;      //     
    HWND m_hbtnMute;           //     
    HWND m_hbtnDumb;           //    
    HWND m_hbtnStartRemote;    //     
    HWND m_hbtnStartLocal;     // 
    HWND m_hstRemoteName;      //     
    HWND m_hstLocalMtName;     //
    HWND m_hstLocalMonitor;    //本地监控图像窗口
    HWND m_hstRemoteMonitor;   //远端监控图像窗口

    CString m_strChairman;     //主席
    CString m_strSpeaker;	   //发言人
    CString m_strMe;           //所连终端本身
};

#endif // !defined(AFX_UIOPINTERFACE_H__57F5B3B1_71A8_4327_8ADC_7BB80C75C02F__INCLUDED_)
