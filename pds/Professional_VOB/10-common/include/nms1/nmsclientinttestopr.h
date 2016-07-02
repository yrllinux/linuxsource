/********************************************************************
模块名:      网管客户端界面测试库
文件名:      NmsClientIntTestUiEx.h
相关文件:    NmcCom.h, GuiUnit.h, NMCLibDef.h
文件实现功能 网管客户端集成测试界面库接口定义
作者：       许世林
版本：       3.5
------------------------------------------------------------------------
修改记录:
日  期		版本		修改人		走读人    修改内容
2004/10/15	3.5			许世林                  创建
*********************************************************************/

/*********************************************************************
							使用说明

1.先调用 GuiRunNmsClient(char* szPath) 接口, 传入网管客户端应用程序路径名,
  此函数会自动处理应用程序是否已经启动的情况

2.再调用 SetGuiMainWindow(int nTimeOut = 0) 接口, 传入超时值,
  因为1.中启动应用程序到正式启动需要一定时间, 若在此时间内成功启动将返回true,
  否则返回false, 外部不用作延时处理

3.再调用ConnectNms(LPCTSTR szIp, DWORD dwPort, LPCTSTR szUser, LPCTSTR szPwd),
  传入连接参数连接网管客户端

4.连接客户端成功后,再调用GuiInitUIHandle()接口对界面元素进行初始化, 
  若3.中连接失败将会初始化失败

5.再调用 SetOprDelay(BOOL bDelay = TRUE) 设置具体操作是否延时,默认为有延时

6.最后调用具体接口进行界面操作.
*********************************************************************/

#ifndef	__NMSCLIENTINTTESTUIEX_H
#define __NMSCLIENTINTTESTUIEX_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "cppunit\cppunit\GuiUnit.h"
#include "NMCLibDef.h"
#include "NmcTestUiCom.h"

class CNmsClientIntTestUiEx
{
public:
	CNmsClientIntTestUiEx();
	virtual ~CNmsClientIntTestUiEx();

public:
    /*函数名： ClearConfInfo
    功    能： 
    参    数： 
    返 回 值： BOOL
    说    明： */
    BOOL ClearConfInfo();
    
    /*函数名： DeleteConfInfo
    功    能： 
    参    数： LPCTSTR lpConfID
    返 回 值： 
    说    明： BOOL*/
    BOOL DeleteConfInfo(LPCTSTR lpConfID);
/*=============================================================================
  函 数 名： SingleBoardOp
  功    能： 单板操作 
  参    数： 
  返 回 值： BOOL  
=============================================================================*/
    BOOL SingleBoardOp(const TSingleBoard& tBoard, int nOpType, BOOL bOK = TRUE);

/*=============================================================================
  函 数 名： SingleBoardOp
  功    能： 单板上传，下载 
  参    数： 
  返 回 值： BOOL  
=============================================================================*/
    BOOL SingleBoarUpLoad(const TSingleBoard& tBoard, CString strFileName, BOOL bOK = TRUE);
    BOOL SingleBoarDownLoad(const TSingleBoard& tBoard, CString strFileName, BOOL bOK = TRUE);

	BOOL CloseWindow();
/*=============================================================================
  函 数 名： GuiRunNmsClient
  功    能： 运行网管客户端程序 
  参    数： char* szPath [in] 网管客户端路径名
  返 回 值： BOOL  
=============================================================================*/
	BOOL GuiRunNmsClient(char* szPath);

/*=============================================================================
  函 数 名： SetGuiMainWindow
  功    能： 查找应用程序是否存在，若存在设置GuiUnit的主窗口句柄 
			 应该在函数GuiRunNmsClient调用后再调用此函数
  参    数： int nTimeOut [in] 查找应用程序超时时间(时间单位：毫秒)
  说明	  ： 因为应用程序启动需要一定时间，若在此时间内还没有捕捉到应用程序将返回false
  返 回 值： BOOL   
=============================================================================*/
	BOOL SetGuiMainWindow(int nTimeOut = 0);

/*=============================================================================
  函 数 名： ConnectNms
  功    能： 连接网管服务器  
  参    数： LPCTSTR szIp	[in] IP地址
             LPCTSTR szPort	[in] 端口号
             LPCTSTR szUser	[in] 用户名
             LPCTSTR szPwd	[in] 密码
  返 回 值： BOOL   
=============================================================================*/
	BOOL ConnectNms(LPCTSTR szIp, DWORD dwPort, LPCTSTR szUser, LPCTSTR szPwd);

/*=============================================================================
  函 数 名： GuiInitUIHandle
  功    能： 登录后初始化界面元素句柄
  参    数： 
  返 回 值： BOOL  
=============================================================================*/
	BOOL GuiInitUIHandle();

/*=============================================================================
  函 数 名： SetOprDelay
  功    能： 设置是否操作延时（默认为延时） 
  参    数： BOOL bDelay [in] true-延时, false-不延时
  返 回 值： void  
=============================================================================*/
inline void SetOprDelay(BOOL bDelay = TRUE)
{
	m_bOprDelay = bDelay;
}

/*=============================================================================
  函 数 名： DeviceAdd
  功    能： 增加设备
  参    数： TEqpAdd &tEqpadd	[in]要增加的设备信息             
             LPCTSTR szDir      [in]立刻分配到的设备组名，为空则表示不分配
  返 回 值： BOOL  
=============================================================================*/
    BOOL DeviceAdd(TEqpAdd &tEqpadd, LPCTSTR szDir = NULL, BOOL bOK = TRUE);
    BOOL DeviceAdd(const vector<TEqpAdd> &vecEqpadd, LPCTSTR szDir = NULL);

    BOOL DeviceAdd_EqpListPopMenu(TEqpAdd &tEqpadd, LPCTSTR szDir = NULL, BOOL bOK = TRUE);
    BOOL DeviceAdd_EqpListPopMenu(const vector<TEqpAdd> &vecEqpadd, LPCTSTR szDir = NULL);

    BOOL DeviceAdd_DirTreePopMenu(TEqpAdd &tEqpadd, LPCTSTR szDir = NULL, BOOL bOK = TRUE);
    BOOL DeviceAdd_DirTreePopMenu(const vector<TEqpAdd> &vecEqpadd, LPCTSTR szDir = NULL);
/*=============================================================================
  函 数 名： DirCreate
  功    能： 创建设备组 
  参    数： LPCTSTR szName	[in]设备组名称
             LPCTSTR szSuperName [in]父设备组名称
  返 回 值： BOOL  
=============================================================================*/
    BOOL DirCreate(LPCTSTR szName, LPCTSTR szSuperName = NULL, BOOL bOK = TRUE);

    BOOL DirCreate_DirTreePopMenu(LPCTSTR szName, LPCTSTR szSuperName = NULL, BOOL bOK = TRUE);

/*=============================================================================
  函 数 名： EqpMock
  功    能： mcu设备机架构件图显示和单板操作 (因ScrollView滚动操作还未实现，此功能目前不能实现)
  参    数： LPCTSTR szName [in] 要操作的mcu别名
			 BYTE byLayer [in] 单板所在层号 (自下而上: 1...4)
			 BYTE bySlot [in] 单板所在槽号 (自左而右: 1...17)
			 BYTE byOprId [in] 单板操作ID
  返 回 值： BOOL 
=============================================================================*/
	BOOL EqpMcuMock(LPCTSTR szName, BYTE byLayer, BYTE bySlot, BYTE byOprId);

/*=============================================================================
  函 数 名： SnmpTest
  功    能： SNMP测试 
  参    数： LPCTSTR szName [in] 设备别名
  返 回 值： BOOL   
=============================================================================*/
	BOOL SnmpTest(LPCTSTR szName);

/*=============================================================================
  函 数 名： SyncTime
  功    能： 时间同步 
  参    数： LPCTSTR szName [in] 设备别名
  返 回 值： BOOL   
=============================================================================*/
	BOOL SyncTime(LPCTSTR szName);
    BOOL SyncTime(const vector<CString>& vecName);

/*=============================================================================
  函 数 名： SyncAlarm
  功    能： 告警同步 
  参    数： LPCTSTR szName [in] 设备别名
  返 回 值： BOOL   
=============================================================================*/
	BOOL SyncAlarm(LPCTSTR szName);
	BOOL SyncAlarm(const vector<CString>& vecName);

/*=============================================================================
  函 数 名： EqpCurAlarm
  功    能： 选中设备的当前告警查询
  参    数： LPCTSTR szName [in] 设备别名
  返 回 值： BOOL   
=============================================================================*/
	BOOL EqpCurAlarm(LPCTSTR szName);

/*=============================================================================
  函 数 名： EqpHisAlarm
  功    能： 选中设备的历史告警查询
  参    数： LPCTSTR szName [in] 设备别名
  返 回 值： BOOL   
=============================================================================*/
	BOOL EqpHisAlarm(LPCTSTR szName);

/*=============================================================================
  函 数 名： RemoteReset
  功    能： 远程复位 
  参    数： LPCTSTR szName [in] 设备别名
  返 回 值： BOOL   
=============================================================================*/
	BOOL RemoteReset(LPCTSTR szName);
	BOOL RemoteReset(const vector<CString>& vecName);

/*=============================================================================
  函 数 名： FtpDownload
  功    能： 文件下载 
  参    数： TFileUpDown &tFileDown [in] 下载参数
  返 回 值： BOOL   
=============================================================================*/
	BOOL FtpDownload(TFileUpDown &tFileDown, BOOL bOK = TRUE);

/*=============================================================================
  函 数 名： FtpUpload
  功    能： 文件上传 
  参    数： TFileUpDown &tFileUp [in]上传参数
  返 回 值： BOOL   
=============================================================================*/
	BOOL FtpUpload(vector<CString> vecEqpName, vector<CString> vecFileName,
        CString strFilePath = _T(""), BOOL bOK = TRUE);

/*=============================================================================
  函 数 名： PingTest
  功    能： 线路测试 
  参    数： TPingTest &tPingTest		[in] ping测试参数			
  返 回 值： BOOL   
=============================================================================*/
	BOOL PingTest(TPingTest &tPingTest, BOOL bOK = TRUE);

/*=============================================================================
  函 数 名： EqpDelete
  功    能： 设备删除 
  参    数： LPCTSTR szName [in] 设备别名
  返 回 值： BOOL   
=============================================================================*/
	BOOL EqpDelete(LPCTSTR szName, BOOL bOK = TRUE);
    BOOL EqpDelete(const vector<CString>& vecName, BOOL bOK = TRUE); //批量  [zhuyr][2/22/2005]

/*=============================================================================
  函 数 名： EqpMcuProperty
  功    能： MCU设备属性 
  参    数： TMcuCfgInfo &tMcuCfg [in] mcu设备参数信息
  返 回 值： BOOL   
=============================================================================*/
	BOOL EqpMcuProperty(TMcuCfgInfo &tMcuCfg, BOOL bOK = TRUE);

/*=============================================================================
  函 数 名： GuiActive
  功    能： 将网管客户端设为当前窗口 
  返 回 值： BOOL  
=============================================================================*/
	BOOL GuiActive();

/*=============================================================================
  函 数 名： GuiToEqp
  功    能： 切换到设备管理界面
  返 回 值： BOOL  
=============================================================================*/
	BOOL GuiToEqp();

/*=============================================================================
  函 数 名： GuiToUser
  功    能： 切换到用户管理界面
  返 回 值： BOOL   
=============================================================================*/
	BOOL GuiToUser();

/*=============================================================================
  函 数 名： GuiToHisAlarm
  功    能： 切换到历史告警管理界面   
  返 回 值： BOOL   
=============================================================================*/
	BOOL GuiToHisAlarm();

/*=============================================================================
  函 数 名： GuiToLog
  功    能： 切换到日志管理界面 
  返 回 值： BOOL  
=============================================================================*/
	BOOL GuiToLog();

/*=============================================================================
  函 数 名： GuiToCurAlarm
  功    能： 当前告警查询  
  返 回 值： BOOL  
=============================================================================*/
	BOOL GuiToCurAlarm();

/*=============================================================================
  函 数 名： GuiToAlarmKB
  功    能： 告警知识库查询   
  返 回 值： BOOL  
=============================================================================*/
	BOOL GuiToAlarmKB();

    BOOL DeleteAlarmKB(DWORD dwAlarmCode, BOOL bOK = TRUE);
    BOOL AddAlarmKB(TAlarmKB& tAddAlarmKB, BOOL bOK = TRUE);
    BOOL ModifyAlarmKB(TAlarmKB& tModifyAlarmKB, BOOL bOK = TRUE);

/*=============================================================================
  函 数 名： NmcLogOff
  功    能： 注销  
  返 回 值： BOOL  
=============================================================================*/
	BOOL NmcLogOff(BOOL bOK = TRUE);

/*=============================================================================
  函 数 名： NmcExit
  功    能： 退出
  返 回 值： BOOL  
=============================================================================*/
	BOOL NmcExit(BOOL bOK = TRUE);

/*=============================================================================
  函 数 名： GuiToMock
  功    能： 切换到选定mcu设备的机架构件视图 
  参    数： LPCTSTR szName [in] 要操作的mcu别名
  返 回 值： BOOL  
=============================================================================*/
	BOOL GuiToMock(LPCTSTR szName);

/*=============================================================================
  函 数 名： HisAlarmQuery
  功    能： 历史告警查询  
  参    数： THisAlarmCondition &tCondition [in] 查询条件
  返 回 值： BOOL  
=============================================================================*/
	BOOL HisAlarmQuery(THisAlarmCondition &tCondition);

/*=============================================================================
  函 数 名： DeleteAllHisAlarm
  功    能： 删除历史告警  
  参    数： LPCTSTR lpszSerial [in] 告警索引号
  返 回 值： BOOL  
=============================================================================*/
    BOOL DeleteAllHisAlarm(BOOL bOK = TRUE);
    BOOL DeleteHisAlarm(LPCTSTR lpszSerial, BOOL bOK = TRUE);

/*=============================================================================
  函 数 名： HisAlarmAddToKB
  功    能： 告警添加到知识库  
  参    数： LPCTSTR lpszSerial [in] 告警索引号
             LPCTSTR lpszAlarmNo [in] 告警编号（与历史告警不同）
  返 回 值： BOOL  
=============================================================================*/
    BOOL HisAlarmAddToKB(LPCTSTR lpszSerial);
    BOOL CurAlarmAddToKB(LPCTSTR lpszAlarmNo);

/*=============================================================================
  函 数 名： LogQuery
  功    能： 操作日志查询  
  参    数： TLogCondition &tCondition	[in] 查询条件
  返 回 值： BOOL  
=============================================================================*/
	BOOL LogQuery(TLogCondition &tCondition);

/*=============================================================================
  函 数 名： DeleteAllLog
  功    能： 删除操作日志  
  参    数： s8* lpszSerial	[in] 日志索引号
  返 回 值： BOOL  
=============================================================================*/
	BOOL DeleteAllLog(BOOL bOK = TRUE);
    BOOL DeleteLog(s8* lpszSerial, BOOL bOK = TRUE);

/*=============================================================================
  函 数 名： UserAdd
  功    能： 添加用户  
  参    数： TUserBaseInfo &tUserInfo	[in]	要添加的用户信息
  返 回 值： BOOL  
=============================================================================*/
	BOOL UserAdd(TUserBaseInfo &tUserInfo);

/*=============================================================================
  函 数 名： UserModify
  功    能： 修改用户信息 
  参    数： TUserBaseInfo &tUserInfo [in] 要修改的用户信息
  返 回 值： BOOL  
=============================================================================*/
	BOOL UserModify(TUserBaseInfo &tUserInfo);

/*=============================================================================
  函 数 名： UserDel
  功    能： 删除指定用户  
  参    数： LPCTSTR szUserName [in] 要删除的用户名
  返 回 值： BOOL   
=============================================================================*/
	BOOL UserDel(LPCTSTR szUserName, BOOL bOK = TRUE);

	BOOL UserDel_UserTreePopMenu(LPCTSTR szUserName, BOOL bOK = TRUE);

/*=============================================================================
  函 数 名： UserAddPriv
  功    能： 为指定用户关联权限组   
  参    数： TUserWithGroup &tUserAddPriv				[in] 要关联权限的用户信息
  返 回 值： BOOL   
=============================================================================*/
	BOOL UserAddPriv(TUserWithGroup &tUserAddPriv, BOOL bOK = TRUE);

    BOOL UserAddPriv_UserTreePopMenu(TUserWithGroup &tUserAddPriv, BOOL bOK = TRUE);

/*=============================================================================
  函 数 名： UserDelPriv
  功    能： 为指定用户取消关联的权限组  
  参    数： TUserWithGroup &tUserDelPriv				[in]要取消关联的用户信息
  返 回 值： BOOL   
=============================================================================*/
	BOOL UserDelPriv(TUserWithGroup &tUserDelPriv);

	BOOL UserDelPriv_UserTreePopMenu(TUserWithGroup &tUserDelPriv, BOOL bOK = TRUE);

/*=============================================================================
  函 数 名： GroupAdd
  功    能： 添加用户组 
  参    数： TGroupBaseInfo &tGroupInfo [in] 要添加的用户组信息
  返 回 值： BOOL  
=============================================================================*/
	BOOL GroupAdd(TGroupBaseInfo &tGroupInfo);

/*=============================================================================
  函 数 名： GroupModify
  功    能： 修改指定用户组  
  参    数： TGroupBaseInfo &tGroupInfo [in] 要修改的用户组
  返 回 值： BOOL  
=============================================================================*/
	BOOL GroupModify(TGroupBaseInfo &tGroupInfo);

/*=============================================================================
  函 数 名： GroupDel
  功    能： 删除指定用户组 
  参    数： LPCTSTR szGroupName [in] 要删除的用户组名
  返 回 值： BOOL   
=============================================================================*/
	BOOL GroupDel(LPCTSTR szGroupName, BOOL bOK = TRUE);

/*=============================================================================
  函 数 名： GroupAddMember
  功    能： 为指定权限组添加用户成员 
  参    数： TUserWithGroup &tGroupAddMem		[in] 要添加用户成员的权限信息
  返 回 值： BOOL   
=============================================================================*/
	BOOL GroupAddMember(TUserWithGroup &tGroupAddMem, BOOL bOK = TRUE);

	BOOL GroupAddMember_UserTreePopMenu(TUserWithGroup &tGroupAddMem, BOOL bOK = TRUE);

/*=============================================================================
  函 数 名： GroupDelMember
  功    能： 删除指定权限组中的用户成员  
  参    数： TUserWithGroup &tGroupDelMem		[in] 要删除用户成员的权限信息
  返 回 值： BOOL  
=============================================================================*/
	BOOL GroupDelMember(TUserWithGroup &tGroupDelMem);

/*=============================================================================
  函 数 名： ModifyCurUserPwd
  功    能： 修改当前用户口令 
  参    数： TModifyUserPwd &tModifyPwd	[in] 口令信息
  返 回 值： BOOL   
=============================================================================*/
	BOOL ModifyCurUserPwd(TModifyUserPwd &tModifyPwd, BOOL bOK = TRUE);

/*=============================================================================
  函 数 名： AlarmFilter
  功    能： 告警过滤 
  参    数： TEqpAlarmFilter &tAlarmFilter  [in] 要过滤的告警信息
  返 回 值： BOOL   
=============================================================================*/
	BOOL AlarmFilter(TEqpAlarmFilter &tAlarmFilter, BOOL bOK = TRUE);

/*=============================================================================
  函 数 名： GroupPrivSet
  功    能： 用户组权限设备  
  参    数： TGroupPriv &tGroupPriv [in] 要设置的权限信息			 
  返 回 值： BOOL  
=============================================================================*/
	BOOL GroupPrivSet(TGroupPriv &tGroupPriv, BOOL bOK = TRUE);

	BOOL GroupPrivSet_UserTreePopMenu(TGroupPriv &tGroupPriv, BOOL bOK = TRUE);

/*=============================================================================
  函 数 名： DirAddMember,DirDelMember
  功    能： 设备组(取消)分配设备  
  参    数： LPCTSTR szDirName [in] 要设置的设备组
             LPCTSTR szDeviceName [in] 要设置的设备 NULL为全部
  返 回 值： BOOL  
=============================================================================*/
	BOOL DirAddMember(LPCTSTR szDirName, LPCTSTR szDeviceName, BOOL bOK = TRUE);
    BOOL DirAddMember(LPCTSTR vecDirName, const vector<CString>& szDeviceName);
	BOOL DirDelMember(LPCTSTR szDirName, LPCTSTR szDeviceName, BOOL bOK = TRUE);
    BOOL DirDelMember(LPCTSTR szDirName, const vector<CString>& szDeviceName);

/*=============================================================================
  函 数 名： DirDelete
  功    能： 删除设备组
  参    数： LPCTSTR szDirName [in] 要设置的设备组
  返 回 值： BOOL  
=============================================================================*/
    BOOL DirDelete(LPCTSTR szDirName, BOOL bOK = TRUE);

/*=============================================================================
  函 数 名： DirResresh
  功    能： 刷新设备组的设备  
  参    数： LPCTSTR szDirName [in] 要设置的设备组
  返 回 值： BOOL  
=============================================================================*/
    BOOL DirResresh(LPCTSTR szDirName);
    
    BOOL DirResresh_EqpListPopMenu(LPCTSTR szDirName);

/*=============================================================================
  函 数 名： DirArrange
  功    能： 刷新设备组的设备  
  参    数： LPCTSTR szDirName [in] 要排列的设备组
  返 回 值： BOOL  
=============================================================================*/
    BOOL DirArrange(LPCTSTR szDirName);
    
    BOOL DirArrange_EqpListPopMenu(LPCTSTR szDirName);


/*=============================================================================
  函 数 名： DirPros
  功    能： 设备组的属性
  参    数： LPCTSTR szDirName [in] 要设置的设备组
  返 回 值： BOOL  
=============================================================================*/
    BOOL DirPros(LPCTSTR szDirName, BOOL bOK = TRUE);
    BOOL DirPros_EqpListPopMenu(LPCTSTR szDirName, BOOL bOK = TRUE);
    
    BOOL ClearAllDir();
    BOOL ClearDevice();
protected:
/*=============================================================================
  函 数 名： MoveBtnClick
  功    能： 模拟鼠标移动到按钮上单击 
  参    数： HWND hWnd [in] 按钮句柄
  返 回 值： void  
=============================================================================*/
	BOOL MoveBtnClick(HWND hWnd);

/*=============================================================================
  函 数 名： MoveTextInput
  功    能： 模拟鼠标移动到Edit控件上输入文本 
  参    数： CString strData	[in] 输入的文本
             HWND hWnd			[in] Edit控件句柄
  返 回 值： void  
=============================================================================*/
	void MoveTextInput(CString strData, HWND hWnd);	

/*=============================================================================
  函 数 名： CbEditChange
  功    能： edit控件内容发生变化时发送消息通知(因为GuiUnit库中没有发送此消息)
  参    数： UINT uCtrlId	[in]控件id
             UINT uMsg		[in]要发送的消息id
             HWND hCtrl		[in]控件句柄
             HWND hParent	[in]控件所在父窗口句柄
  返 回 值： BOOL  
=============================================================================*/
	BOOL EditChange(UINT uCtrlId, UINT uMsg, HWND hCtrl, HWND hParent);

/*=============================================================================
  函 数 名： ErrorDeal
  功    能： 错误提示框处理，若有提示框则关掉
  参    数： HWND hParent
  返 回 值： BOOL [in]	父窗口句柄
=============================================================================*/
	BOOL ErrorDeal(const HWND hParent);

/*=============================================================================
  函 数 名： EqpRClickMenu
  功    能： 设备列表视图右键菜单操作  
  参    数：LPCTSTR szName	[in] 操作设备别名
			UINT uOprID		[in] 操作宏id
  返 回 值： BOOL 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2004/10/21    3.5			许世林                  创建
=============================================================================*/
	BOOL EqpRClickMenu(LPCTSTR szName, UINT uOprID, UINT nTime = 1, BOOL bOK = TRUE);

/*=============================================================================
  函 数 名： TreeDirRClickMenu
  功    能： 设备拓扑图右键菜单操作  
  参    数：LPCTSTR szName	[in] 操作设备别名
			UINT uOprID		[in] 操作宏id
  返 回 值： BOOL 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2004/12/02    3.5			朱允荣                  创建
=============================================================================*/
    BOOL TreeDirRClickMenu(LPCTSTR szName, UINT uOprID);

/*=============================================================================
  函 数 名： TreeDirSelectItem
  功    能： 设备目录树视图中选中某个item  
  参    数： LPCTSTR szName [in] 要选中item的文本, szName为空表示根
  返 回 值： BOOL  
=============================================================================*/
	BOOL TreeDirSelectItem(LPCTSTR szName = NULL);

/*=============================================================================
  函 数 名： UpdateUserInfo
  功    能： 添加或修改用户时更新用户信息   
  参    数： TUserBaseInfo &tUserInfo [in] 要更新的用户信息
  返 回 值： BOOL   
=============================================================================*/
	BOOL UpdateUserInfo(TUserBaseInfo &tUserInfo);

/*=============================================================================
  函 数 名： TreeUserRClickMenu
  功    能： 用户树视图中某个item的弹出菜单操作
  参    数： LPCTSTR szName [in] 要选中item的文本, szName 为空表示用户or组根
			 BOOL bUser [in] 用户or组操作，默认为用户
  返 回 值： BOOL  
=============================================================================*/
	BOOL TreeUserRClickMenu(LPCTSTR szName, UINT uOprID, BOOL bUser = TRUE);

/*=============================================================================
  函 数 名： TreeUserSelectItem
  功    能： 树视图中选中某个item  
  参    数： LPCTSTR szName [in] 要选中item的文本, szName 为空表示用户or组根
			 BOOL bUser [in] 用户or组操作，默认为用户
  返 回 值： BOOL  
=============================================================================*/
	BOOL TreeUserSelectItem(LPCTSTR szName, BOOL bUser = TRUE);

/*=============================================================================
  函 数 名： UpdateMcuProsPeri
  功    能： mcu属性配置中更新外设配置信息 
  参    数： BYTE byPeriType	[in] 外设类型
			 vector<CString> vtPeriInfo [in] 要更新的外设信息
  返 回 值： BOOL  
=============================================================================*/
	BOOL UpdateMcuProsPeri(BYTE byPeriType, vector<CString> &vtPeriInfo, BOOL bOK = TRUE);

/*=============================================================================
  函 数 名： MixerCfg2Str
  功    能： 将混音器参数转换为字符串数组 
  参    数： TMixerCfg &tMixerCfg [in] 混音器参数
			 vector<CString> [in/out] 字符串数组形式的混音器参数
  返 回 值：  
=============================================================================*/
	void MixerCfg2Str(TMixerCfg &tMixerCfg, vector<CString> &vtMixer);

/*=============================================================================
  函 数 名： RecCfg2Str
  功    能： 将录像机参数转换为字符串数组 
  参    数： TRecCfg &tRecCfg [in] 录像机参数
			 vector<CString>  [in/out] 字符串数组形式的录像机参数 
  返 回 值：  
=============================================================================*/
	void RecCfg2Str(TRecCfg &tRecCfg, vector<CString> &vtRec);

/*=============================================================================
  函 数 名： TWCfg2Str
  功    能： 将电视墙参数转换为字符串数组   
  参    数： TTVWallCfg &tTWCfg [in] 电视墙参数
			 vector<CString>   [in/out] 字符串数组形式的电视墙参数 
  返 回 值： 
=============================================================================*/
	void TWCfg2Str(TTVWallCfg &tTWCfg, vector<CString> &vtTW);

/*=============================================================================
  函 数 名： TBasVmpCfg2Str
  功    能： 码流适配器或画面合成器参数转换为字符串数组  
  参    数： BasVmpCfg &tBasVmpCfg [in] 适配器或画面合成器参数
			 vector<CString>  [in/out] 字符串数组形式的参数  
  返 回 值： 
=============================================================================*/
	void BasVmpCfg2Str(TVmpCfg &tBasVmpCfg, vector<CString> &vtBasVmp);
	void BasCfg2Str(TBasVmpCfg &tBasVmpCfg, vector<CString> &vtBasVmp);

/*=============================================================================
  函 数 名： McuProsBasicInfo
  功    能： mcu设备属性－基本信息配置 
  参    数： TMcuCfgInfo &cMcuCfg [in] mcu配置信息
  返 回 值： BOOL  
=============================================================================*/
	BOOL McuProsBasicInfo(TMcuCfgInfo &tMcuCfg);

/*=============================================================================
  函 数 名： McuProsNet
  功    能： mcu设备属性－网络配置 
  参    数： TMcuCfgInfo &cMcuCfg [in] mcu配置信息
  返 回 值： BOOL  
=============================================================================*/
	BOOL McuProsNet(TMcuCfgInfo &tMcuCfg);

/*=============================================================================
  函 数 名： McuProsBrd
  功    能： mcu设备属性－单板配置  
  参    数： TMcuCfgInfo &cMcuCfg [in] mcu配置信息
  返 回 值： BOOL  
=============================================================================*/
	BOOL McuProsBrd(TMcuCfgInfo &tMcuCfg);

/*=============================================================================
  函 数 名： McuProsPeri1
  功    能： mcu设备属性－外设1配置 
  参    数： TMcuCfgInfo &cMcuCfg [in] mcu配置信息
  返 回 值： BOOL   
=============================================================================*/
	BOOL McuProsPeri1(TMcuCfgInfo &tMcuCfg);

/*=============================================================================
  函 数 名： McuProsPeri2
  功    能： mcu设备属性－外设2配置 
  参    数： TMcuCfgInfo &cMcuCfg [in] mcu配置信息
  返 回 值： BOOL   
=============================================================================*/
	BOOL McuProsPeri2(TMcuCfgInfo &tMcuCfg);

/*=============================================================================
  函 数 名： McuProsPrs
  功    能： mcu设备属性－包重传配置  
  参    数： TMcuCfgInfo &cMcuCfg [in] mcu配置信息
  返 回 值： BOOL  
=============================================================================*/
	BOOL McuProsPrs(TMcuCfgInfo &tMcuCfg);

/*=============================================================================
  函 数 名： McuProsNetSync
  功    能： mcu设备属性－网同步配置 
  参    数： TMcuCfgInfo &cMcuCfg [in] mcu配置信息
  返 回 值： BOOL  
=============================================================================*/
	BOOL McuProsNetSync(TMcuCfgInfo &tMcuCfg);

/*=============================================================================
  函 数 名： TBMenuClick
  功    能： 实现鼠标点击工具栏菜单功能  
  参    数： BYTE byMenu	[in] 菜单id
             BYTE byOprId	[in] 要实现的操作id
  返 回 值： BOOL  
=============================================================================*/
	BOOL TBMenuClick(BYTE byMenu, BYTE byOprId);

/*=============================================================================
  函 数 名： ListCtrlMenuClick
  功    能： ListCtrl控件中右键菜单处理  
  参    数： CString csName	[in] 根据名称单击
			 BYTE byOprId   [in] 要实现的操作id
             HWND hListCtrl	[in] 控件句柄
  返 回 值： BOOL  
=============================================================================*/
	BOOL ListCtrlMenuClick(CString csName, BYTE byOprId, HWND hListCtrl);

/*=============================================================================
  函 数 名： ListCtrlMenuClick
  功    能： ListCtrl控件中右键菜单处理 
  参    数： int nIndex		[in] 根据索引单击
			 BYTE byOprId   [in] 要实现的操作id
             HWND hListCtrl [in] 控件句柄
  返 回 值： BOOL  
=============================================================================*/
	BOOL ListCtrlMenuClick(int nIndex, BYTE byOprId, HWND hListCtrl);

/*=============================================================================
  函 数 名： ListBoxSelect
  功    能： 选中某一项item 
  参    数： CString csText	[in] 要选中的item文本
             HWND hListBox	[in] listbox句柄
  返 回 值： BOOL  
=============================================================================*/
	BOOL ListBoxSelect(CString csText, HWND hListBox);
    
    BOOL DeleteTreeItem(HTREEITEM hItem, HWND hWnd, BOOL bDelThis = TRUE);

    BOOL SetAlarmKB(TAlarmKB& tModifyAlarmKB, BOOL bModify = TRUE, BOOL bOK = TRUE);
    BOOL SetAlarmKBList(TAlarmKB& tModifyAlarmKB);

//以下是一些操作的具体执行阶段（有别于触发阶段）。具体说明可参考相应的操作
    BOOL DoDeviceAdd(TEqpAdd &tEqpadd, BOOL bOK = TRUE);
    BOOL DoDeviceAdd(const vector<TEqpAdd> &vecEqpadd);
	BOOL DoDirCreate(LPCTSTR szName, BOOL bOK = TRUE);
	BOOL DoUserDel(LPCTSTR szUserName, BOOL bOK = TRUE);
	BOOL DoUserAddPriv(TUserWithGroup &tUserAddPriv, BOOL bOK = TRUE);
	BOOL DoUserDelPriv(TUserWithGroup &tUserAddPriv, BOOL bOK = TRUE);
	BOOL DoGroupAddMember(TUserWithGroup &tGroupAddMem, BOOL bOK = TRUE);
	BOOL DoGroupPrivSet(TGroupPriv &tGroupPriv, BOOL bOK = TRUE);
    BOOL DoDirPros(LPCTSTR szDirName, BOOL bOK = TRUE);

protected:
	//界面驱动库接口
	CGuiUnit			m_cGuiUnit;
	//当前所在界面位置
	int					m_nCurGuiPos;
	//是否延时的开关,默认为延时
	BOOL				m_bOprDelay;

    HWND                m_hMain;    //主窗口  

	//界面元素句柄---以下句柄在GuiInitUIHandle()中初始化
	HWND m_hTbMenu;			//菜单栏
	
	//设备管理界面
	HWND m_hTreeEqp;		//设备拓扑树-left
	HWND m_hListEqp;		//设备列表	-right

	//历史告警管理
	HWND m_hDlgHisAlarm;	//历史告警对话框
	HWND m_hCbAlmType;		//告警类别
	HWND m_hEtEqpId;		//设备编号
	HWND m_hCbtnAlmRand[3];	//告警级别
	HWND m_hRdAlmCrTime[4];	//告警产生时间选择
	HWND m_hEtCrLastMonth;	//前 N 个月
	HWND m_hEtCrLastDay;	//前 N 天
	HWND m_hDpCrStartDate;	//介于
	HWND m_hDpCrStartTime;
	HWND m_hDpCrEndDate;
	HWND m_hDpCrEndTime;
	HWND m_hRdAlmRsTime[4];	//告警恢复时间选择
	HWND m_hEtRsLastMonth;	//前 N 个月
	HWND m_hEtRsLastDay;	//前 N 天
	HWND m_hDpRsStartDate;	//介于
	HWND m_hDpRsStartTime;
	HWND m_hDpRsEndDate;
	HWND m_hDpRsEndTime;
	HWND m_hBtnAlmStartSch;	//告警搜索
	HWND m_hBtnAlmStopSch;	//停止搜索
	HWND m_hBtnAlmDel;		//删除

	//用户管理
	HWND m_hDlgUser;		//用户管理对话框
	HWND m_hTreeUser;		//TreeView 用户管理
	HWND m_hEtUserId;		//用户编号
	HWND m_hEtUserLog;		//登录名称
	HWND m_hEtUserPass[2];	//登录密码
	HWND m_hEtUserName;		//姓名
	HWND m_hRtUserSex[2];	//性别
	HWND m_hEtUserPho;		//电话
	HWND m_hEtUserMoPh;		//手机
	HWND m_hEtUserBeep;		//呼机
	HWND m_hEtUserEMail;	//E_mail
	HWND m_hEtUserDepart;	//单位
	HWND m_hEtUserAddr;		//住址
	HWND m_hEtUserDetail;	//备注
	HWND m_hBtnUserSave;	//保存
	HWND m_hBtnUserModify;	//修改
	HWND m_hBtnUserDel;		//删除
	HWND m_hListUserInfo;	//权限信息列表
	HWND m_hBtnUserAddGp;	//添加组
	HWND m_hBtnUserDelGp;	//删除组
	HWND m_hBtnUserCalcu;	//统计权限
	
	//组管理
	HWND m_hDlgGroup;		//组管理对话框
	HWND m_hEtGrpName;		//名称
	HWND m_hEtGrpDetail;	//描述
	HWND m_hBtnGrpCreat;	//创建
	HWND m_hBtnGrpModify;	//修改
	HWND m_hBtnGrpDel;		//删除
	HWND m_hListMembList;	//成员列表
	HWND m_hBtnMembAdd;		//添加
	HWND m_hBtnMembDel;		//删除
	HWND m_hListGrpDetail;	//权限信息
	HWND m_hBtnPwerSet;		//设置
	
	//日志管理
	HWND m_hDlgLog;			//日志管理对话框
	HWND m_hEtLogUserName;	//用户名
	HWND m_hBtnSelAll;		//全选
	HWND m_hBtnUnSelAll;	//全不选
	HWND m_hListBoxOprType; //操作类型列表框
	HWND m_hRdLogRsTime[4];	//告警恢复时间选择
	HWND m_hEtLogLastMonth;	//前 N 个月
	HWND m_hEtLogLastDay;	//前 N 天
	HWND m_hDpLogStartDate;	//介于
	HWND m_hDpLogStartTime;
	HWND m_hDpLogEndDate;
	HWND m_hDpLogEndTime;
	HWND m_hCbOprObj;		//操作对象类型
	HWND m_hEtOprName;		//操作对象名称
	HWND m_hCbtnOprId;		//操作对象编号
	HWND m_hEtOprId;		//操作对象编号
	HWND m_hCbtnOprRet[2];	//操作对象结果
	HWND m_hBtnLogStartSch;	//开始搜索
	HWND m_hBtnLogStopSch;	//停止搜索
	HWND m_hBtnLogDel;		//删除日志
};
#endif // #define __NMSCLIENTINTTESTUIEX_H
