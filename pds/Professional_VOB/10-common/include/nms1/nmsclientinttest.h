// NmsClientIntTest.h: interface for the CNmsClientIntTest class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NMSCLIENTINTTEST_H__9D5C14B4_DE71_40A2_9B82_0D6781FEC96C__INCLUDED_)
#define AFX_NMSCLIENTINTTEST_H__9D5C14B4_DE71_40A2_9B82_0D6781FEC96C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afx.h>
#include <afxwin.h>
#include "NmcLibDef.h"
#include "nmsmacro.h"

typedef struct tagTConfQuery
{
	s8 m_szConfId[33];	// 会议ID
}TConfQuery, *PTConfQuery; 

class CNmsClientIntTest  
{
public:
	CNmsClientIntTest();
	virtual ~CNmsClientIntTest();

	//////////////////////////////////////////////////////////////////////////
	// 配置管理
	//////////////////////////////////////////////////////////////////////////
	
    //列举所有的设备
    virtual u32 CmDeviceEnum(PTEquipProperty patDevices,	 // [in/out] 设备的数组
					  u32 &dwDeviceCount);	// [in/out] 数组的大小
    //列举设备组中的设备
    virtual u32 CmDeviceOfDirEnum(u32 dwDirID,
                      PTEquipProperty patDevices,	// [in/out] 设备的数组
					  u32 &dwDeviceCount);	// [in/out] 数组的大小

    virtual u32 CmDirEnum(PTDirInfo patDirs, // [in/out] 设备组的数组
                u32 &dwDirCount);    // [in/out] 数组的大小

    virtual u32 CmGetChildDirs(u32 dwDirID,
                    PTDirInfo patDir,     // [in/out] 设备组的数组
                    u32 &dwDirCount);   // [in/out] 数组的大小

    //得到设备的当前最高告警级别
    virtual u32 CmGetDeviceTopAlarmlevel(u32 dwDeviceID, u32 &dwLevel);
    //得到当前最近告警
    virtual LPCTSTR CmGetDeviceAlarmDesc(u32 dwDeviceID); 
    
    //会议信息  [zhuyr][4/28/2005]
    virtual u32 CmGetConfoInfo(PTConfQuery pConfInfo,   // [in/out] 会议ID的数组
                        u32 &dwConfoCount);     // [in/out] 数组的大小
    virtual BOOL32 IsConfoInBase(TConfQuery& tConfID);    //会议是否存在
    virtual BOOL32 IsHasConf();                           //是否还有会议存在

	//////////////////////////////////////////////////////////////////////////
	// 故障管理

	// 查询设备的当前告警, ID为零表示查全部。
	virtual u32 FmCurrentAlarmQuery(u32 dwDeviceID,                         // [in] 设备数据
							u32 &dwRecordCount);				// [out] 记录总数
	
	// 历史告警查询
	virtual u32 FmHistoryAlarmQuery(THistoryAlarmCondition tCondition,	// [in] 查询条件
							  u32 &dwRecordCount);				// [out] 记录总数
	
	// 操作日志查询
	virtual u32 SmOprLogQuery(	TOprLogCondition tCondition, // 查询条件
								u32 &dwRecordCount);		// 记录总数
    
    virtual BOOL32 IsLogExist(LPCTSTR lpszLogSerial); //字符串长度不能超过32，不能为NULL
    /*=============================================================================
    函 数 名： GetNextBatchRecord
    功    能： 获取下一批记录, FmCurrentAlarmQuery和FmHistoryAlarmQuery后调用
    参    数： u32 dwRemainCount:          [out]实际返回的记录数量
               PVOID pContent:    [out/buffer] 记录列表内存
               u32 dwRecCount:             [in] 记录内存中可以保存多少条记录
    返 回 值： u32 :成功：0；失败：错误码。
    说明    ：PVOID类型：PTAlarmContent：FmCurrentAlarmQuery和FmHistoryAlarmQuery后调用，
                         PTOprLogResult：QueryOprLog后调用
    =============================================================================*/
    virtual u32 GetNextBatchRecord(u32& dwRemainCount, PTAlarmContent pContent, u32 dwRecCount);
    //得到日志记录
    virtual u32 GetNextBatchRecord(u32& dwRemainCount, PTOprLogResult pContent, u32 dwRecCount);
	
	// 查询设备的机架构件信息--框架和单板
	virtual u32 FmMockMachineQuery(u32 dwDeviceID,  TMockMachineData& tMockData);	// [out] 机架数据
	// 向服务器查询单板信息
	virtual u32 FmMockBoardRefresh(u32 dwDeviceID, TBoard &tBoard);	// [out] 单板数据

    //////////////////////////////////////////////////////////////////////////
    // 安全管理

	// 枚举用户
	virtual u32 SmUserEnum(PTNameStruct patUsers,		// [in/out/array] 用户列表
		u32 &dwBufCount);			// [in/out] 列表数目
	
	// 用户信息查询
	virtual u32 SmUserQueryInfo(u32 dwUserID, PTUserBaseInfo ptUserBaseInof);
	

	// 查询用户的权限
	virtual u32 SmUserPrivilege(	u32 dwUserID,	// [in] 用户ID
							u32 *pdwArrDevice,	// [in/out] 设备数组
							u32 *pdwArrMask,	// [in/out] 掩码数组
							u32 &dwCount);	// [in/out] 数组大小
	// 查询组的权限
//	virtual u32 SmGroupPrivilege(	u32 dwGroupID,	// [in] 组ID
//							u32 *pdwArrDevice,	// [in/out] 设备数组
//							u32 *pdwArrMask,	// [in/out] 掩码数组
//							u32 &dwCount);	// [in/out] 数组大小
	
	// 枚举系统中的组
	virtual u32 SmGroupEnum(PTNameStruct patGroups,	// [in/out] 组的数组
					  u32 &dwGroupCount);	// [in/out] 数组的大小

	
	// 查询组的用户成员
	virtual u32 SmGroupQueryUser(	u32 dwGroupID,	// [in] 组的编号
		PTNameStruct patUsers,	// [in/out] 用户的数组
		u32	&dwUserCount);	// [in/out] 数组的大小
	
	// 查询用户拥有的组
	virtual u32 SmUserQueryGroup(	u32 dwUserID,	// [in] 用户编号
		PTNameStruct patGroups,	// [in/out] 组的数组
		u32 &dwGroupCount);	// [in/out] 数组的大小

    //#define TYPE_DEVICE       1
    //#define TYPE_Dir          2
    //#define TYPE_USER         3
    //#define TYPE_GROUP        4
	
    //连接，断链。
    virtual BOOL32 LinkInstance(u32 dwIP, u16 dwPort = 2010);
    virtual void UnlinkInstance();

    //  [zhuyr][2/22/2005] 新增接口

         //用户是否存在
	virtual BOOL32 IsUserExist(LPCSTR lpszUserName);

	//用户组是否存在

	virtual BOOL32 IsGroupExist(LPCSTR lpszGroupName);

	//设备是否存在

	virtual BOOL32 IsDeviceExist(LPCSTR lpszDeviceName);

	//查询某组中该会员是否存在

	virtual BOOL32 IsUserInGroup(LPCSTR lpszGroupName, LPCSTR lpszUserName);

	//查询用户是否拥有该组

	virtual BOOL32 IsGroupInUser(LPCSTR lpszUserName, LPCSTR lpszGroupName);

	//查询设备组是否存在

	virtual BOOL32 IsDirExist(LPCSTR lpszDirName);

	//查询设备组中该设备是否存在

	virtual BOOL32 IsDeviceInDir(LPCSTR lpszDirName , LPCSTR lpszDeviceName);

	//查询设备组中该设备组是否存在

	virtual BOOL32 IsDirInDir(LPCSTR lpszMainDir, LPCSTR lpszChildDir);

	//通过别名获取id

	virtual BOOL32 GetIdByAlias(LPCSTR lpszAlias, s32 nIdType,u32 &dwID);

	//通过id获取别名

	virtual BOOL32 GetAliasById(u32 dwID, s32 nIdType, s8* pszAlias);
    
    //告警知识库  [zhuyr][5/17/2005]
    //告警码是否在知识库中  [zhuyr][5/17/2005]
    virtual BOOL32  IsAlarmCodeExist(u32 dwAlarmCode); //1000~15000
    //查询某条告警码的原因列表 nSize[in/out]
    virtual u32 GetAlarmKBCause(u32 dwAlarmCode, PTAlarmKBCause pCause, u32 &dwSize);
    //查询某条告警码的解决方案列表 nSize[in/out]
    virtual u32 GetAlarmKBSolution(u32 dwAlarmCode, PTAlarmKBCause pSolution, u32 &dwSize);

    //历史告警是否存在  [zhuyr][5/17/2005]
    virtual BOOL32  IsHisAlarmExist(u32 dwSerialNo); 
protected:
	virtual u32 SendEvent(u16 u16Event, const void *pBuf, u16 u16Size,
		void *pAckBuf, u16 u16AckSize, u16 &u16AckSizeRet);

    virtual BOOL32 IsLinked();

    virtual u32 IsElementExist(LPCSTR lpszName, u32 byType);


    u32 m_dwConnectedNode;
    s8  m_szDesc[201];
};

#endif // !defined(AFX_NMSCLIENTINTTEST_H__9D5C14B4_DE71_40A2_9B82_0D6781FEC96C__INCLUDED_)
