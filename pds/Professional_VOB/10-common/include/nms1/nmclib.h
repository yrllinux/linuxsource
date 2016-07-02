/********************************************************************
	模块名:      NMClib
	文件名:      nmclib.h
	相关文件:    
	文件实现功能 网管客户端接口声明。
    作者：       朱允荣
	版本：       3.5
------------------------------------------------------------------------
	修改记录:
	日  期		版本		修改人		走读人    修改内容
	2004/08/02	3.5			朱允荣                  创建

*********************************************************************/

#pragma once

#include "nmsmacro.h"
#include "NMCLibDef.h"
#include <vector>

using namespace std;

////////////////////////////////////////////////////////////////////////////////
// 接口定义
class INMCNotifyWnd
{
protected:
    HWND m_hNotifyWnd;
    void Post2Application(u32 uMsgType, WPARAM wParam = 0, LPARAM lParam = 0);
    void Send2Application(u32 uMsgType, WPARAM wParam = 0, LPARAM lParam = 0);

public:
    INMCNotifyWnd() {m_hNotifyWnd = NULL;};
	/*功能	获取通知消息目的窗口
	参数	无
	返回值	消息目的窗口
	说明	无*/
    HWND GetNotifyWnd() {return m_hNotifyWnd;};
	/*功能	设置通知消息目的窗口
	参数	hWnd	消息目的窗口
	返回值	无
	说明	无*/
    void SetNotifyWnd(HWND hWnd) {m_hNotifyWnd = hWnd;};

};

class INMCDevice 
{
public:
    //同步
    virtual u32 SyncCfg() = 0;
    virtual u32 SyncTime() = 0;
    virtual u32 SyncAlarm() = 0;    

    /*=============================================================================
    函 数 名： RemoteControl
    功    能： 远程控制
    参    数： u8 byCmdType :控制类型
               PTBoard pBoard = NULL：单板
    返 回 值： u32 :成功：0；失败：错误码。
    =============================================================================*/
    virtual u32 RemoteControl(u8 byCmdType, PTBoard pBoard = NULL) = 0;

    /*=============================================================================
    函 数 名： QueryMockMachine
    功    能： 查询设备机架构件属性
    参    数： u32 dwDeviceID：[in]设备ID
               TMockMachineData& tMockData: [out]机架结构信息
    返 回 值： u32 :成功：0；失败：错误码。
    =============================================================================*/
    virtual u32 QueryMockMachine(TMockMachineData& tMockData) = 0;
	virtual u32 QuerySonDevice( u32 dwDeviceID, vector<INMCDevice*>& vecpDevice) = 0;
    /*=============================================================================
    函 数 名： MockBoardRefresh
    功    能： 单板信息刷新
    参    数： TBoard& tBoard: [in]单板信息
    返 回 值： u32 :成功：0；失败：错误码。
    =============================================================================*/
    virtual u32 MockBoardRefresh(const TBoard &tBoard) = 0;

    /*=============================================================================
    函 数 名： FtpUpload
    功    能： 文件上传
    参    数： LPCTSTR lpszRemoteFile：文件在设备中的名字
               LPCTSTR lpszFilePath：本地文件路径及文件名
               u8 byOsType：操作系统类型
               PTBoard pBoard = NULL：单板
    返 回 值： u32 :成功：0；失败：错误码。
    =============================================================================*/
    virtual u32 FtpUpload(LPCTSTR lpszRemoteFile, LPCTSTR lpszFilePath, 
                          u8 byOsType, PTBoard pBoard = NULL) = 0;

    /*=============================================================================
    函 数 名:FtpFileRename
    功    能:上传文件结束后, 重名名
    参    数:vector<CString> vctRemoteFile  [in]    文件全名
             PTBoard ptBoard                [in]    单板
    注    意:无
    返 回 值:成功: 0; 失败: 错误码 
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/03/29  4.0     王昊    创建
    =============================================================================*/
    virtual u32 FtpFileRename(vector<CString> vctRemoteFile, PTBoard ptBoard = NULL) = 0;

    /*=============================================================================
    函 数 名： FtpDownload
    功    能： 文件下载
    参    数： LPCTSTR lpszRemoteFile：文件在设备中的名字
               LPCTSTR lpszFilePath：本地文件路径及文件名
               u8 byOsType：操作系统类型
               PTBoard pBoard = NULL：单板
    返 回 值： u32 :成功：0；失败：错误码。
    =============================================================================*/
    virtual u32 FtpDownload(LPCTSTR lpszRemoteFile, LPCTSTR lpszFilePath,
                            u8 byOsType, PTBoard pBoard = NULL) = 0;    

    // 更改设备的别名
    virtual u32 RenameAlias(LPCTSTR lpszAlias) = 0;

    virtual void SetPrivilege(u32 dwPrivilege) = 0;


    /*=============================================================================
    函 数 名： 
    功    能： 单板配置和操作(MCU)
    参    数： TBoard& tBoard:单板信息
    返 回 值： u32 :成功：0；失败：错误码。
    =============================================================================*/
    virtual u32 BoardReset(const TBoard& tBoard) = 0;
    virtual u32 BoardSyncTime(const TBoard& tBoard) = 0;
    virtual u32 BoardBitErrTest(const TBoard& tBoard) = 0;
    virtual u32 BoardSelfTest(const TBoard& tBoard) = 0;
    virtual u32 DSIBoardE1Cfg(const TDSIBoard& tBoard) = 0;

    //////////////////////////////////////////////////////////////////////////
    //调试
    virtual void ShowCurrentAlarm() = 0;
    
    //////////////////////////////////////////////////////////////////////////
    //属性
    virtual u32 GetDeviceID() = 0;
	virtual u32 GetDevicePID() = 0;
    virtual u16 GetDeviceType() = 0;
    virtual u8  GetTopAlarmLevel() = 0;
    virtual u32 GetPrivilege() = 0;
    virtual u32 GetDevicePos(u32& xPos, u32& yPos) = 0;
	virtual BOOL32 GetIpAddr(u32& dwIpAddr) = 0;
	virtual BOOL32 GetIpAddr(LPTSTR lpsIpAddr) = 0;
    virtual LPCTSTR GetAlias() = 0;
    virtual u8 GetLinkStatus() = 0;
    virtual LPCTSTR GetAlarmDesc() = 0;
    virtual LPCTSTR GetHardVer(void) const = 0; //  获取硬件版本号
    virtual LPCTSTR GetSoftVer(void) const = 0; //  获取软件版本号

    /*=============================================================================
    函 数 名:GetOsType
    功    能:获取操作系统类型
    参    数:无
    注    意:无
    返 回 值:操作系统类型
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/08/14  4.0     王昊    创建
    =============================================================================*/
    virtual u8 GetOsType(void) const = 0;
};


class INMCDeviceDir
{
public:
    /*=============================================================================
    函 数 名： ReNameDir
    功    能： 修改设备目录名。
    参    数： LPCTSTR lpszNewName：新目录名
    返 回 值： u32 :成功：0；失败：错误码。
    =============================================================================*/
    virtual u32 ReNameDir(LPCTSTR lpszNewName) = 0;

    /*=============================================================================
    函 数 名： MoveDir
    功    能： 移动设备目录
    参    数： u32 dwSuperID：目标目录ID，移动后的父目录
    返 回 值： u32 :成功：0；失败：错误码。
    =============================================================================*/
    virtual u32 MoveDir(u32 dwSuperID) = 0;

    /*=============================================================================
    函 数 名： DirMemberAdd
    功    能： 增加子目录成员
    参    数： u32 dwDirID：增加的设备ID
    返 回 值： u32 :成功：0；失败：错误码。
    =============================================================================*/
    virtual u32 DirMemberAdd(u32 dwDirID) = 0;

    /*=============================================================================
    函 数 名： DeleteDirMember
    功    能： 删除子目录成员
    参    数： u32 dwDirID：删除的设备ID
    返 回 值： u32 :成功：0；失败：错误码。
    =============================================================================*/
    virtual u32 DeleteDirMember(u32 dwDirID) = 0;

    /*=============================================================================
    函 数 名： AppendDevice
    功    能： 增加设备成员，只加入列表
    参    数： u32 dwDeviceID：增加的设备ID
    返 回 值： u32 :成功：0；失败：错误码。
    =============================================================================*/
    virtual u32 AppendDevice(u32 dwDeviceID) = 0;

    /*=============================================================================
    函 数 名： DeleteDirMember
    功    能： 删除设备成员，只删除列表
    参    数： u32 dwDeviceID：删除的设备ID
    返 回 值： u32 :成功：0；失败：错误码。
    =============================================================================*/
    virtual u32 DeleteDeviceMember(u32 dwDeviceID) = 0;

    /*=============================================================================
    函 数 名： SetDirMemberPos
    功    能： 设定成员设备的位置
    参    数： u32 dwDeviceID:设备ID
               u32 dwPosX:设备位置水平坐标
               u32 dwPosY:设备位置垂直坐标
    返 回 值： u32 :成功：0；失败：错误码。
    =============================================================================*/
    virtual u32 SetDirMemberPos(u32 dwDeviceID, u32 dwPosX, u32 dwPosY) = 0;
    virtual u32 GetDirMemberPos(u32 dwDeviceID, u32& dwPosX, u32& dwPosY) = 0;

    virtual u32 GetDirPos(u32& dwPosX, u32& dwPosY) = 0;
    virtual u32 SetDirPos(u32 dwPosX, u32 dwPosY) = 0;

    virtual u32 GetChildDir(vector<u32>& vecDirID) = 0;
    virtual u32 GetChildDir(vector<INMCDeviceDir*>& vecpDir) = 0;
    virtual u32 GetChildDevice(vector<u32>& vecDeviceID) = 0;
    virtual u32 GetChildDevice(vector<INMCDevice*>& vecpDevice) = 0;
    virtual INMCDeviceDir* GetSuperDir() = 0;
    virtual u32 GetDirID() = 0;
    virtual BOOL32 HasSubDeviceDir() = 0;
    virtual BOOL32 HasDeviceMember() = 0;
    virtual BOOL32 IsMemberExists(u32 dwDeviceID) = 0;
    virtual u8  GetTopAlarmLevel() = 0;
    virtual LPCTSTR GetDirName() = 0;

};

class INMCUser
{
public:
    /*=============================================================================
    函 数 名： ModifyUser
    功    能： 修改用户信息
    参    数： const TUserBaseInfo& tUserInfo: [in]输入具体用户信息。
    返 回 值： u32 :成功：0；失败：错误码。
    =============================================================================*/
    virtual u32 ModifyUser(const TUserBaseInfo& tUserInfo) = 0;
    
    /*=============================================================================
    函 数 名： QueryGroupsOfUser
    功    能： 查询用户所在的所有的组。
    参    数： u32 dwUserID:用户ID
               vector<PTNameInfo>& vecNameInfo：[out]所有用户所在的组的信息
    返 回 值： u32 :成功：0；失败：错误码。
    =============================================================================*/
    virtual u32 QueryGroupsOfUser(vector<PTNameInfo>& vecNameInfo) = 0;

    /*=============================================================================
    函 数 名： LinkGroup
    功    能： 加入（退出）用户组
    参    数： u32 dwGroupID：目标用户组
    返 回 值： u32 :成功：0；失败：错误码。
    =============================================================================*/
    virtual u32 LinkGroup(u32 dwGroupID) = 0;
    virtual u32 UnLinkGroup(u32 dwGroupID) = 0;
    
    /*=============================================================================
    函 数 名： GetUserPrivilege
    功    能： 查询用户权限
    参    数： vector<TPrivilege>& vecPrivilege：[out]权限信息
    返 回 值： u32 :成功：0；失败：错误码。
    =============================================================================*/
    virtual u32 GetUserPrivilege(vector<TPrivilege>& vecPrivilege) = 0;
    virtual u32 StatPrivilege() = 0;

    //////////////////////////////////////////////////////////////////////////
    //属性
    virtual u32 GetUserID() = 0;
    virtual LPCTSTR GetLogonName() = 0;
    virtual u32 GetInformation(TUserBaseInfo& tInfo) = 0;
    virtual u32 FindPrivilege(u32 dwDeviceID, u32& dwPrivilege) = 0;
    virtual BOOL32 IsAdministrator() = 0;
    virtual BOOL32 IsExistGroup(u32 dwGroupID) = 0;
};

class INMCGroup
{
public:    
    /*=============================================================================
    函 数 名： ModifyGroupInfo
    功    能： 修改用户组信息
    参    数： const TGroupBaseInfo& tGroupBaseInfo:[in]用户组信息
    返 回 值： u32:成功：0；失败：错误码。
    =============================================================================*/
    virtual u32 ModifyGroupInfo(const TGroupBaseInfo& tGroupBaseInfo) = 0;

    virtual u32 GetGroupID() = 0;
    virtual u32 GetGroupBaseInfo(TGroupBaseInfo& tGroupBaseInfo) = 0;
    
    /*=============================================================================
    函 数 名： SetPrivilege
    功    能： 设置（得到）用户组权限
    参    数： const TPrivilege& tPrivilege：[in]权限信息
    返 回 值： u32 :成功：0；失败：错误码。
    =============================================================================*/
    virtual u32 SetPrivilege(const TPrivilege& tPrivilege) = 0;
    virtual u32 GetPrivilege(TPrivilege& tPrivilege) = 0;

    /*=============================================================================
    函 数 名： AddUser
    功    能： 用户加入（退出）用户组
    参    数： u32 dwUserID:用户ID
    返 回 值： u32 :成功：0；失败：错误码。
    =============================================================================*/
    virtual u32 AddUser(u32 dwUserID) = 0;
    virtual u32 DeleteUser(u32 dwUserID) = 0;
    
    /*=============================================================================
    函 数 名： QueryGroupPrivilege
    功    能： 查询用户组权限
    参    数： vector<TPrivilege>& vecPrivilege：[out]权限信息
    返 回 值： u32 :成功：0；失败：错误码。
    =============================================================================*/
    virtual u32 QueryGroupPrivilege(vector<TPrivilege>& vecPrivilege) = 0;
    
    /*=============================================================================
    函 数 名： QueryUsersInGroup
    功    能： 查询用户组中的所有的用户
    参    数： vector<PTNameInfo>& vecNameInfo：[out]所有在组中的用户的信息
    返 回 值： u32 :成功：0；失败：错误码。
    =============================================================================*/
    virtual u32 QueryUsersInGroup(vector<PTNameInfo>& vecNameInfo) = 0;

    //////////////////////////////////////////////////////////////////////////
    //
    virtual BOOL32 IsExistUser(u32 dwUserID) = 0;
};

 
//系统管理接口（用户管理）
class ISysManager : public INMCNotifyWnd
{
public:
    virtual INMCUser* GetUser(u32 dwUserID) = 0;
    virtual INMCGroup* GetGroup(u32 dwGroupID) = 0;

    /*=============================================================================
    函 数 名： AddUser
    功    能： 增加用户
    参    数： TUserBaseInfo& tUserInfo: [in\out]输入具体用户信息,返回ID。
    返 回 值： u32 :成功：0；失败：错误码。
    =============================================================================*/
    virtual u32 AddUser(TUserBaseInfo& tUserInfo) = 0;

    /*=============================================================================
    函 数 名： DeleteUser
    功    能： 删除用户
    参    数： u32 dwUserID: [in]删除的用户ID。
    返 回 值： u32 :成功：0；失败：错误码。
    =============================================================================*/
    virtual u32 DeleteUser(u32 dwUserID) = 0;


    /*=============================================================================
    函 数 名： EnumUsers
    功    能： 得到所有的用户信息
    参    数： vector<PTNameInfo>& vecUserInfo:
    返 回 值： u32 :成功：0；失败：错误码。
    =============================================================================*/
    virtual u32 EnumUsers(vector<PTNameInfo>& vecUserInfo) = 0;


    /*=============================================================================
    函 数 名： CreateGroup
    功    能： 创建用户组
    参    数： TGroupBaseInfo& tGroupBaseInfo: [in\out]输入具体用户组信息,返回ID。
    返 回 值： u32 :成功：0；失败：错误码。
    =============================================================================*/
    virtual u32 CreateGroup(TGroupBaseInfo& tGroupBaseInfo) = 0;


    /*=============================================================================
    函 数 名： DestroyGroup
    功    能： 销毁用户组
    参    数： u32 dwGroupID：用户组ID
    返 回 值： u32 :成功：0；失败：错误码。
    =============================================================================*/
    virtual u32 DestroyGroup(u32 dwGroupID) = 0;

    /*=============================================================================
    函 数 名： EnumGroups
    功    能： 得到所有的用户组
    参    数： vector<PTNameInfo>& vecGroupInfo:[out]所有的用户组的信息
    返 回 值： u32:成功：0；失败：错误码。
    =============================================================================*/
    virtual u32 EnumGroups(vector<PTNameInfo>& vecGroupInfo) = 0;  

    /*=============================================================================
    函 数 名： ChangePWD
    功    能： 修改当前用户密码
    参    数： LPCTSTR lpszOldPSW：旧密码
               LPCTSTR lpszNewPSW：新密码
    返 回 值： u32 :成功：0；失败：错误码。
    =============================================================================*/
    virtual u32 ChangePWD(LPCTSTR lpszOldPSW, LPCTSTR lpszNewPSW) = 0;


    /*=============================================================================
    函 数 名： QueryOprLog
    功    能： 查询（删除）日志
    参    数： const TOprLogCondition& tCondition: [in]查询条件
               u32& dwRecCount: [out]查询结果数目
    返 回 值： u32 :成功：0；失败：错误码。
    =============================================================================*/
    virtual u32 QueryOprLog(const TOprLogCondition& tCondition, u32& dwRecCount) = 0;
    virtual u32 DelOprLog(const TOprLogCondition& tCondition) = 0;
    virtual u32 DelOprLog(u32 dwSerial) = 0;
    
    /*=============================================================================
    函 数 名： GetNextBatchRecord
    功    能： 获取下一批记录
    参    数： u32& dwRemainCount:         [out]还剩下的记录数量
               PTOprLogResult pContent:    [out/buffer] 记录列表内存
               u32 dwRecCount:             [in] 记录内存中可以保存多少条记录
    返 回 值： u32 :成功：0；失败：错误码。
    =============================================================================*/
    virtual u32 GetNextBatchRecord(u32& dwRemainCount, PTOprLogResult pContent,
        u32& dwRecCount) = 0;
};

//告警信息接口
class IAlarmInfo : public INMCNotifyWnd
{
public:
    //////////////////////////////////////////////////////////////////////////
    //告警知识库

    /*=============================================================================
    函 数 名： GetAlarmKBInfo
    功    能： 查询告警知识库，某个告警的基本信息。
    参    数： u32 dwAlarmCode: [in]报警码
               vector<PTAlarmKBInfo>& vecKBInfo：[out]知识库内容，可能不只一条。
    返 回 值： u32 :成功：0；失败：错误码。
    =============================================================================*/
    virtual u32 GetAlarmKBInfo(u32 dwAlarmCode, vector<PTAlarmKBInfo>& vecKBInfo) = 0;
    
    /*=============================================================================
    函 数 名： GetAlarmKBCause
    功    能： 查询告警知识库，某个告警原因。
    参    数： u32 dwAlarmCode: [in]报警码
               vector<PTAlarmKBCause>& vecKBCause：[out]原因内容，可能不只一条。
    返 回 值： u32 :成功：0；失败：错误码。
    =============================================================================*/
    virtual u32 GetAlarmKBCause(u32 dwAlarmCode, vector<PTAlarmKBCause>& vecKBCause) = 0;
    
    /*=============================================================================
    函 数 名： GetAlarmKBSolution
    功    能： 查询告警知识库，某个解决方案
    参    数： u32 dwAlarmCode: [in]报警码
               vector<PTAlarmKBCause>& vecKBSolution：[out]解决方案，可能不只一条。
    返 回 值： u32 :成功：0；失败：错误码。
    =============================================================================*/
    virtual u32 GetAlarmKBSolution(u32 dwAlarmCode, vector<PTAlarmKBCause>& vecKBSolution) = 0;


    /*=============================================================================
    函 数 名： InsertAlarmKBInfo etc.
    功    能： 增加告警知识库基本信息;修改;删除
    参    数： TAlarmKBInfo tAlarmKBInfo: [in]告警信息
    返 回 值： u32 :成功：0；失败：错误码。
    =============================================================================*/
    virtual u32 InsertAlarmKBInfo(const TAlarmKBInfo& tAlarmKBInfo) = 0;
    virtual u32 ModifyAlarmKBInfo(const TAlarmKBInfo& tAlarmKBInfo) = 0;
    virtual u32 DeleteAlarmKBInfo(u32 dwAlarmCode) = 0;

    /*=============================================================================
    函 数 名： InsertAlarmKBInfo etc.
    功    能： 增加告警知识库告警原因;修改;删除
    参    数： TAlarmKBCause& tAlarmKBCause: [in]告警原因
    返 回 值： u32 :成功：0；失败：错误码。
    =============================================================================*/
    virtual u32 InsertAlarmKBCause(const TAlarmKBCause& tAlarmKBCause) = 0;
    virtual u32 ModifyAlarmKBCause(const TAlarmKBCause& tAlarmKBCause) = 0;
    virtual u32 DeleteAlarmKBCause(u32 dwAlarmCode) = 0;

    /*=============================================================================
    函 数 名： InsertAlarmKBSolution etc.
    功    能： 增加告警知识库告警解决方案;修改;删除
    参    数： TAlarmKBCause& tAlarmKBSolution: [in]告警解决方案
    返 回 值： u32 :成功：0；失败：错误码。
    =============================================================================*/
    virtual u32 InsertAlarmKBSolution(const TAlarmKBCause& tAlarmKBSolution) = 0;
    virtual u32 ModifyAlarmKBSolution(const TAlarmKBCause& tAlarmKBSolution) = 0;
    virtual u32 DeleteAlarmKBSolution(u32 dwAlarmCode) = 0;

    //////////////////////////////////////////////////////////////////////////
    //告警

    /*=============================================================================
    函 数 名： QueryCurrentAlarm
    功    能： 查询设备的当前告警
    参    数： const u32 dwDeviceID：       [in]设备ID，（0为所有的设备）
               u32& dwRecCount：            [out]得到的告警数目
    返 回 值： u32 :成功：0；失败：错误码。
    =============================================================================*/
    virtual u32 QueryCurrentAlarm(const u32 dwDeviceID, u32& dwRecCount) = 0;

    /*=============================================================================
    函 数 名： QueryHistoryAlarm etc.
    功    能： 查询历史告警;删除
    参    数： const THistoryAlarmCondition& tHAlarmCond: [in]查询条件
               u32& dwRecCount：            [out]得到的告警数目
    返 回 值： u32 :成功：0；失败：错误码。
    =============================================================================*/
    virtual u32 QueryHistoryAlarm(const THistoryAlarmCondition& tHAlarmCond,
        u32& dwRecCount) = 0;
    virtual u32 DeleteHistoryAlarm(const THistoryAlarmCondition& tHAlarmCond) = 0;
    virtual u32 DeleteHistoryAlarm(u32 dwSerial) = 0;


    /*=============================================================================
    函 数 名： GetNextBatchRecord
    功    能： 获取下一批记录
    参    数： u32 dwRemainCount:          [out]还剩下的记录数量
               PTAlarmContent pContent:    [out/buffer] 记录列表内存
               u32 dwRecCount:             [in/out] 记录内存中可以保存多少条记录
    返 回 值： u32 :成功：0；失败：错误码。
    =============================================================================*/
    virtual u32 GetNextBatchRecord(u32& dwRemainCount, PTAlarmContent pContent,
        u32& dwRecCount) = 0;
    //////////////////////////////////////////////////////////////////////////
    //告警过滤

    /*=============================================================================
    函 数 名： SetAlarmFilter
    功    能： 设置告警过滤
    参    数： const TAlarmFilter& tAlarmFilter:告警过滤条件
    返 回 值： u32 :成功：0；失败：错误码。
    =============================================================================*/
    virtual u32 SetAlarmFilter(const TAlarmFilter& tAlarmFilter) = 0;

    /*=============================================================================
    函 数 名： QueryAlarmCodeFilter etc.
    功    能： 查询告警过滤条件。
    参    数： vector<u32>& vecAlarmFilter： [out] 告警过滤条件，查询结果
    返 回 值： u32 :成功：0；失败：错误码。
    =============================================================================*/
    virtual u32 QueryAlarmCodeFilter(vector<u32>& vecCodeFilter) = 0;
    virtual u32 QueryAlarmLevelFilter(vector<u8>& vecLevelFilter) = 0;


    /*=============================================================================
    函 数 名： DeleteAlarmCodeFilter
    功    能： 删除告警过滤条件
    参    数： 
    返 回 值： u32 :成功：0；失败：错误码。
    =============================================================================*/
    virtual u32 DeleteAlarmCodeFilter() = 0;
    virtual u32 DeleteAlarmLevelFilter() = 0;

    virtual u32 GetAlarmRemark(u32 dwCode, LPTSTR lpRemark) = 0;
    virtual u32 GetAllAlarmRemark(vector<PTAlarmRemark>& vecAlarmRemark) = 0;
};

//设备管理接口
class IDeviceManager : public INMCNotifyWnd
{
public:
    
    virtual INMCDevice* GetDevice(u32 dwDeviceID) = 0;
    virtual INMCDeviceDir* GetDeviceDir(u32 dwDirID) = 0;
    virtual INMCDeviceDir* GetRootDeviceDir() = 0;

    /*=============================================================================
    函 数 名： FtpBreak
    功    能： 中断文件传输
    参    数： 
    返 回 值： u32 :成功：0；失败：错误码。
    =============================================================================*/
    virtual u32 FtpBreak() = 0;
    
    /*=============================================================================
    函 数 名： AddDevice
    功    能： 增加一个设备
    参    数： u32 dwIPAddr：设备IP地址
               u16 wDevieType：设备类型
               LPCTSTR lpszAlias：设备类型
               u32& dwDeviceID：【out】新设备ID
    返 回 值： u32 :成功：0；失败：错误码。
    =============================================================================*/
    virtual u32 AddDevice(u32 dwIPAddr, u16 wDevieType, LPCTSTR lpszAlias,
        u32& dwDeviceID, u32 dwDevicePID) = 0;
    

    /*=============================================================================
    函 数 名： DeleteDevice
    功    能： 删除一个设备
    参    数： u32 dwDeviceID：设备ID
    返 回 值： u32 :成功：0；失败：错误码。
    =============================================================================*/
    virtual u32 DeleteDevice(u32 dwDeviceID) = 0;

    /*=============================================================================
    函 数 名： CreateDir
    功    能： 创建一个设备目录
    参    数： LPCTSTR lpszName：目录名
               u32 dwSuperID：上级目录ID
               u32& dwDirID：【out】新目录ID
    返 回 值： u32 :成功：0；失败：错误码。
    =============================================================================*/
    virtual u32 CreateDir(LPCTSTR lpszName, u32 dwSuperID, u32& dwDirID) = 0;

    /*=============================================================================
    函 数 名： DestroyDir
    功    能： 销毁设备目录
    参    数： u32 dwDirID：目录ID
    返 回 值： u32 :成功：0；失败：错误码。
    =============================================================================*/
    virtual u32 DestroyDir(u32 dwDirID) = 0;

    virtual u32 GetDeviceCount(u32& dwDeviceCount) = 0;
    virtual LPCTSTR GetFtpUser() = 0;
    virtual LPCTSTR GetFtpPwd() = 0;

    /*函数名： ExConfQuery
    功    能： 
    参    数： vector<PTConfRecord>& vecRecord
    返 回 值： u32 
    说    明： */
    virtual u32 ExConfQuery(vector<PTConfRecord>& vecRecord) = 0;
    virtual u32 ExMtQuery(LPCTSTR lpConfID, vector<PTMtRecord>& vecRecord) = 0;
	virtual u32 EQPQuery(u32 dwDirID, vector<PTEquipProperty>& vecRecord) = 0;

    virtual u32 DelConfInfo(LPCSTR lpConfID = NULL) = 0;
};

class IConnectSever : public INMCNotifyWnd
{
public:
	/*功能	登陆服务器
	参数	lpszUserName	用户名
			lpszPSW	密码
			dwIP	被连接服务器的IP地址，主机序
            dwPort  服务器端口
	返回值	成功返回0,失败返回非0错误码
	说明	若连接失败，发送*/
    virtual u32 LogonSever(LPCTSTR lpszUserName, LPCTSTR lpszPSW,
        u32 dwIP, u16 wPort = 2010) = 0;

	/*功能	断开与服务器的连接
	参数	无
	返回值	成功返回0,失败返回非0错误码
	说明	在没有连接到服务器时调用此方法，不起任何作用*/
    virtual u32 LogoffSever() = 0;

	/*功能	判断是否已登陆服务器
	参数	无
	返回值	已登陆返回TRUE,否则返回FALSE
	说明	无*/
    virtual BOOL32 IsLogonSever() = 0;


	/*功能	获取特定的控制接口
	参数	ppCtrl	控制接口
	返回值	成功返回0,失败返回非0错误码
	说明	在连接到终端时,可以获取这些接口,在断开连接后,所有这些接口立刻失效*/
    virtual u32 NMCGetInterFace(ISysManager** ppCtrl) = 0;
    virtual u32 NMCGetInterFace(IAlarmInfo** ppCtrl) = 0;
    virtual u32 NMCGetInterFace(IDeviceManager** ppCtrl) = 0;

    // 广播搜索网管服务器的地址
	virtual u32 BroadcastForSearchNms(vector<TNmsEnvData>& vecNmsData) = 0;

    //得到当前用户ID
    virtual u32 GetCurrUserID(u32& dwUserID) = 0;
    virtual INMCUser* GetCurrUser() = 0;
    
    //  wanghao 2006/01/05 方便切换双语时重新读数据
    virtual u32 ConstructAffairData() = 0;
};

////////////////////////////////////////////////////////////////////////////////

class CNMCLib
{
public:
	/*功能	创建实现了接口IConnectSever的对象
	参数	ppConnect	[out]成功返回IConnectSever接口,失败返回NULL
	返回值	成功返回0,失败返回非0错误码
	说明	接口使用完成后,调用DestroyInstance方法销毁对象*/
    static u32 CreateInstance(IConnectSever** ppConnect);

	/*功能	销毁实现了接口ppConnect的对象
	参数	ppConnect	[in, out]接口IMtcConnectMt,成功后返回NULL
	返回值	成功返回0,失败返回非0错误码
	说明	只能销毁由CreateInstance方法创建的对象*/
	static u32 DestroyInstance(IConnectSever **ppConnect);

	/*功能	格式化错误
	参数	dwError:	错误码
	返回值	错误码说明。
	说明	*/
    static LPCTSTR FormatErrorMsg(u32 dwError);

    static LPCTSTR ConvertIpAddr(DWORD dwIpAddr);
    static BOOL32 SplitFileName(LPCTSTR lpszFilePath, LPTSTR lpsFileName);

    enum EMLanguage
    {
        emEnglish = 0,
        emChinese = 1
    };

public:
    static EMLanguage s_emLanguage;
};

/*=============================================================================
函 数 名:nmclibhelp
功    能:nmclib打印帮助
参    数:无
注    意:无
返 回 值:无
-------------------------------------------------------------------------------
修改纪录:
日      期  版本    修改人  修改内容
2006/04/05  4.0     王昊    创建
=============================================================================*/
API void nmclibhelp(void);

/*=============================================================================
函 数 名:nmclibver
功    能:打印nmclib版本
参    数:无
注    意:无
返 回 值:无
-------------------------------------------------------------------------------
修改纪录:
日      期  版本    修改人  修改内容
2006/04/05  4.0     王昊    创建
=============================================================================*/
API void nmclibver(void);
