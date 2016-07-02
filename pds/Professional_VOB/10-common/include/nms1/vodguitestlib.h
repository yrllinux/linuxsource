/********************************************************************
模块名:      VOD界面测试库模块
文件名:      VodGuiTestLib.h
相关文件:    
文件实现功能 VOD界面测试库接口定义 
作者：       任厚平  
版本：       4.0
------------------------------------------------------------------------
修改记录:
日		期	版本	修改人	走读人	修改内容
2005/08/11	4.0		任厚平			创建

*********************************************************************/


#if !defined(AFX_VODGUITESTLIB_H__487413F5_38EB_4320_B3A1_A911CEC17636__INCLUDED_)
#define AFX_VODGUITESTLIB_H__487413F5_38EB_4320_B3A1_A911CEC17636__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "guiunit.h"
#include "VodGuiTestDef.h"
//#include "vccommon.h"

typedef struct tagUserInfo
{
	BOOL32	m_bSex;				   //性别true 为男,反之为女
	LPTSTR m_szUserID;			//用户id
	LPTSTR m_szUserName;		//用户名
	LPTSTR m_szPwd;			//用户密码
	LPTSTR m_szRole;			//用户组别id
	LPTSTR m_szRoleName;		//用户组别名（管理员组...）
	LPTSTR m_szTel;			//
	LPTSTR m_szMobilePhone;	//
	LPTSTR m_szBP;			    //
	LPTSTR m_szEmail;			//
	LPTSTR m_szUnit;			//
	LPTSTR m_szAddr;			//
	BOOL32 m_bPassDate;		        //是否帐号过期 true:设定过期时间
	LPTSTR m_szDate; 		    // 帐号过期时间输入示例：2005－08－20；
	//SYSTEMTIEM *pSysDate;         //帐号过期时间
	LPTSTR m_szIfAvailAlways;	//
	LPTSTR m_szvod_Schedule1;	//     输入格式:   9:5:16, 0:23:6     错误格式:  09:05:16 
	LPTSTR m_szvod_Schedule2;	// 终止时间
	LPTSTR m_szStylePath;		//
	LPTSTR m_szLocalIP;			//用户登录ip限制
}TUserInfo;

typedef struct tagFileInfo
{
	LPTSTR	 m_szClassName;		//节目柜完整名称
	LPTSTR	 m_szFileFullName;	//节目完整名称（包含扩展名）
	LPTSTR	 m_szFileShortName;	//节目简洁名称（包含扩展名）
	LPTSTR	 m_szFilePath;		//节目完整路径（相对于mms根目录，包含ip）
	LPTSTR	 m_szFileType ;			//节目类型
	LPTSTR	 m_szFileSize ;		//节目大小
	LPTSTR	 m_szFileSecret ;		//节目加密等级		
	LPTSTR	 m_szFileDesc ;		//节目简要描述
}TFileInfo;

typedef struct tagFolderInfo
{

	LPTSTR	 m_szClassName;		//节目柜完整名称
	LPTSTR	 m_szParentClassName;	//父节目柜完整名称
	LPTSTR	 m_szClassDescribe;	//节目柜描述
}TFolderInfo;

typedef struct tagSecurityInfo
{
	BOOL bFolMag;
	BOOL bFilePub;
	BOOL bLogMag;
	BOOL bSecretFile;
}TSecurityInfo;

typedef struct tagPosMsg
{
	LPTSTR szFolder;
	LPTSTR szItem;
	LPRECT pRect;
}TPosMsg , *PTPosMsg;

class CVodGuiTestLib  
{
public:
	CVodGuiTestLib();

	virtual ~CVodGuiTestLib();
public:
	/*=============================================================================
	函 数 名：GetOutBarCtlInfo
	功    能：获得OutBarCtrl控件的子项信息
	参    数：strFolderName      [in]  夹子文本
			  strItemName	 [in]	子项文本
			  hWnd			   [in]	 控件句柄
			  cPosRect		   [out]  控件位置
	返 回 值：BOOL
	=============================================================================*/
	virtual BOOL GetOutBarCtlSubInfo(HWND hWnd, LPTSTR szFolderName, LPTSTR szItemName, RECT& rectPos);

		/*=============================================================================
	函 数 名：GetOutBarCtlMainInfo
	功    能：获得OutBarCtrl控件的子项信息
	参    数：strFolderName      [in]  夹子文本
			  hWnd			   [in]	 控件句柄
			  cPosRect		   [out]  控件位置
	返 回 值：BOOL
	=============================================================================*/
	virtual BOOL GetOutBarCtlMainInfo( HWND hWnd, LPCTSTR szFolderName, RECT& rectPos ) const;
    /*=============================================================================
	函 数 名：GuiInit
	功    能：初始化VOD测试库
	参    数：strExePath      [in]  应用程序路径
	返 回 值：BOOL
	=============================================================================*/
	virtual BOOL GuiInit(LPTSTR szExePath);	

	/*=============================================================================
	函 数 名：Login
	功    能：系统登录
	参    数：strName     [in]  用户名
	          strPwd      [in]  登录密码 
	返 回 值：BOOL
	=============================================================================*/
	virtual BOOL32 Login(const CString& strLoginName, const CString& strLoginPwd);

	/*=============================================================================
	函 数 名：GuiInitHandle
	功    能：初始化句柄变量
	参    数：无	          
	返 回 值：BOOL
	=============================================================================*/
	virtual BOOL32 GuiInitHandle();

	/*=============================================================================
	函 数 名：TransMainInterface
	功    能：界面切换
	参    数：strInfaceName     [in]  界面名称	          
	返 回 值：BOOL
	=============================================================================*/
    virtual BOOL32 TransMainInterface(const CString& strInfaceName);

	/*=============================================================================
	函 数 名：TransInterface
	功    能：界面切换
	参    数：strInfaceName     [in]  界面名称	          
	返 回 值：BOOL
	=============================================================================*/
    virtual BOOL32 TransInterface(const CString& strInfaceName);

	/*=============================================================================
	函 数 名：DemondFile
	功    能：文件点播
	参    数：strName     [in]  文件名
			  strFolName  [in]  所在文件柜名
	返 回 值：BOOL
	=============================================================================*/
	virtual BOOL32 DemondFile(LPTSTR strName, LPTSTR strFolName);

	/*=============================================================================
	函 数 名：DemondMeet
	功    能：会议直播
	参    数：strMCUName    [in]  MCU名称
			  strFileName   [in]  会议名称
	返 回 值：BOOL
	=============================================================================*/
	virtual BOOL32 DemondMeet(LPTSTR strMCUName, LPTSTR strFileName);

	/*=============================================================================
	函 数 名：AddFolder
	功    能：增加节目柜
	参    数：tFolderInfo     [in]  新节目柜信息	           
	返 回 值：BOOL
	=============================================================================*/
	virtual BOOL32 AddFolder(const TFolderInfo& tFolderInfo);

	/*=============================================================================
	函 数 名：DeleteFolder
	功    能：删除节目柜
	参    数：strName     [in]  要删除的节目柜的名称	           
	返 回 值：BOOL
	=============================================================================*/
	virtual BOOL32 DeleteFolder(LPTSTR strName);

	/*=============================================================================
	函 数 名：PubFile
	功    能：文件发布
	参    数：tFileInfo     [in]  新文件信息           
	返 回 值：BOOL
	=============================================================================*/
	virtual BOOL32 PubFile (const TFileInfo& tFileInfo);

	/*=============================================================================
	函 数 名：DeleteFile
	功    能：删除指定的文件
	参    数：strName     [in]  文件名	           
	返 回 值：BOOL
	=============================================================================*/
	virtual BOOL32 DeleteFile(LPTSTR strName);

	/*=============================================================================
	函 数 名：ModiFolder
	功    能：修改节目柜
	参    数：nIndex       [in]   节目柜索引
	          tFileInfo    [in]   节目柜新信息
	返 回 值：BOOL
	=============================================================================*/
	virtual BOOL32 ModiFolder(LPTSTR strFolName, const TFolderInfo& tFolderInfo);

	/*=============================================================================
	函 数 名：ModiFile
	功    能：修改文件
	参    数：strFileName     [in]  文件名称
	          tFileInfo      [in]  文件新信息
	返 回 值：BOOL
	=============================================================================*/
	virtual BOOL32 ModiFile(LPTSTR strFileName, const TFileInfo& tFileInfo);

	/*=============================================================================
	函 数 名：QueryFileTal
	功    能：文件查询
	参    数：strName     [in]  文件名	           
	返 回 值：BOOL
	=============================================================================*/
	virtual BOOL32 QueryFileTal(const CString& strName);

	/*=============================================================================
	函 数 名：SetCurFoll
	功    能：设置为当前文件柜
	参    数：strName     [in]  文件名	           
	返 回 值：BOOL
	=============================================================================*/
	virtual BOOL32 SetCurFoll(LPTSTR strName);

	/*=============================================================================
	函 数 名：QueryFileCur
	功    能：文件查询
	参    数：strName     [in]  文件名	           
	返 回 值：BOOL
	=============================================================================*/
	virtual BOOL32 QueryFileCur(const CString& strName);

	/*=============================================================================
	函 数 名：ChangePwd
	功    能：更改密码
	参    数：strOldPwd     [in]  旧密码
	          strNewPwd     [in]  新密码
			  strConfirPwd	[in]  确认密码
	返 回 值：BOOL
	=============================================================================*/
	virtual BOOL32 ChangePwd(const CString& strOldPwd, const CString& strNewPwd, 
		                 const CString& strConfirPwd);

	/*=============================================================================
	函 数 名：DeleteLog
	功    能：删除指定的日志
	参    数：nIndex     [in]  日志索引	           
	返 回 值：BOOL
	=============================================================================*/
	virtual BOOL32 DeleteLog(vector<s32> nIndex);

	/*=============================================================================
	函 数 名：RegistUser
	功    能：新用户注册
	参    数：tUserInfo     [in]  新用户信息	           
	返 回 值：BOOL
	=============================================================================*/
	virtual BOOL32 RegistUser(const TUserInfo& tUserInfo);

	/*=============================================================================
	函 数 名：RefreshList
	功    能：用户列表刷新
	参    数：无	           
	返 回 值：BOOL
	=============================================================================*/
	virtual BOOL32 RefreshList();

	/*=============================================================================
	函 数 名：DeleteUser
	功    能：删除指定的用户
	参    数：strUserId     [in]  用户ID	           
	返 回 值：BOOL
	=============================================================================*/
	virtual BOOL32 DeleteUser(LPTSTR strUserId);

	/*=============================================================================
	函 数 名：UnuseUser
	功    能：停用指定的用户
	参    数：strUserId     [in]  用户ID	           
	返 回 值：BOOL
	=============================================================================*/
	virtual BOOL32 UnuseUser(const CString& strUserId);

	/*=============================================================================
	函 数 名：ReuseUser
	功    能：重新启用指定的用户
	参    数：strUserId     [in]  用户ID	           
	返 回 值：BOOL
	=============================================================================*/
	virtual BOOL32 ReuseUser(const CString &strUserId);

	/*=============================================================================
	函 数 名：ModiUser
	功    能：用户资料修改
	参    数：strUserId     [in]  指定的用户ID
	          tUserInfo     [in]  用户新资料信息
	返 回 值：BOOL
	=============================================================================*/
	virtual BOOL32 ModiUser(const CString& strUserId, const TUserInfo& tUserInfo);

	/*=============================================================================
	函 数 名：ModiSecuritySet
	功    能：修改安全设置
	参    数：strGroupName     [in]  用户组名
			  TSecurityInfo	   [in]  新安全信息
	返 回 值：BOOL
	=============================================================================*/
	virtual BOOL32 ModiSecuritySet(LPTSTR strGroupName,
		                    const TSecurityInfo& tSecurityInfo);

	/*=============================================================================
	函 数 名：ResetSecurity
	功    能：重填安全设置信息
	参    数：strGroupName     [in]  用户组名	           
	返 回 值：BOOL
	=============================================================================*/
	virtual BOOL32 ResetSecurity(LPTSTR strGroupName);

	/*=============================================================================
	函 数 名：ToFirstPage
	功    能：返回到第一页
	参    数：无           
	返 回 值：BOOL
	=============================================================================*/
	virtual BOOL32 ToFirstPage ();

	/*=============================================================================
	函 数 名：ToPrePage
	功    能：返回到前一页
	参    数：无	           
	返 回 值：BOOL
	=============================================================================*/
	virtual BOOL32 ToPrePage();

	/*=============================================================================
	函 数 名：ToNextPage
	功    能：到下一页
	参    数：无	           
	返 回 值：BOOL
	=============================================================================*/
	virtual BOOL32 ToNextPage();

	/*=============================================================================
	函 数 名：ToLastPage
	功    能：到最后一页
	参    数：无	           
	返 回 值：BOOL
	=============================================================================*/
	virtual BOOL32 ToLastPage();

	/*=============================================================================
	函 数 名：ToAppointedPage
	功    能：到指定的页
	参    数：nIndex  [in]  页码索引	           
	返 回 值：BOOL
	=============================================================================*/
	virtual BOOL32 ToAppointedPage (s32 nIndex);

	/*=============================================================================
	函 数 名：LogOut
	功    能：注销并返回到登录界面
	参    数：无	           
	返 回 值：BOOL
	=============================================================================*/
	virtual BOOL32 LogOut();

	/*=============================================================================
	函 数 名：SysHelp
	功    能：系统帮助
	参    数：无	           
	返 回 值：BOOL
	=============================================================================*/
	virtual BOOL32 SysHelp();

	/*=============================================================================
	函 数 名：QuitSys
	功    能：离开系统
	参    数：无	           
	返 回 值：BOOL
	=============================================================================*/
	virtual BOOL32 QuitSys();

	/*=============================================================================
	函 数 名：GetPageNo
	功    能：取得列表页码
	参    数：无	           
	返 回 值：BOOL
	=============================================================================*/
	u8 GetPageNo(u8 uIndex);

	/*=============================================================================
	函 数 名：ClosePopWnd
	功    能：关闭弹出对话框
	参    数：无	           
	返 回 值：virtual BOOL
	=============================================================================
	BOOL32 ClosePopWnd();*/
	/*=============================================================================
	函 数 名：Close
	功    能：关闭测试接口，清理现场
	参    数：无	           
	返 回 值：无
	=============================================================================*/
    void Close();

	CGuiUnit			m_cGuiUnit;			//界面驱动库接口
	
protected:
	HWND				m_hOutBar;   //OutBarCtrl控件句柄
	HWND				m_hQuit;            //退出系统按钮句柄

	u8					m_byCurGuiPos;		//当前所在界面位置
	u8					m_byCurGuiSubPos;	//当前所在子界面位置
	u8					m_byFolderPage;       //标识当前页码
	u8					m_byFilePage;
	u8					m_byFilePubPage;
	u8					m_byPopWnd;			//标识弹出对话框
	s32*				m_pnCount;
	u8					m_byLanguge;			//当前语言

	// TSecurityInfo		m_tSecuInfo[GROUP_KIND]; //保存用户组设置


	HWND				m_hHelp;            //帮助按钮句柄
	HWND				m_hLogout;          //Logout按钮句柄
	
	/*//定义各个按钮，控件的句柄
	HWND				m_hVideoOnDemn;   //视频点播按钮句柄
	HWND				m_hFileOnDemn;    //文件点播按钮句柄
	HWND				m_hMeetOnDemn;    //会议直播按钮句柄
	HWND				m_hFolderMag;     //节目柜管理按钮句柄
	HWND				m_hFilePub;		  //文件发布按钮句柄
	HWND				m_hLogMag;        //日志维护按钮句柄
	HWND				m_hSysSet;        //系统设置按钮句柄
	HWND				m_hComDwn;        //组件下载按钮句柄
	HWND				m_hPwdChange;     //密码更改按钮句柄
	HWND				m_hAboutSys;      //关于系统按钮句柄
	HWND				m_hSecurityMag;   //安全管理按钮句柄
	HWND				m_hUserMag;       //用户管理按钮句柄
	HWND				m_hSecuritySet;   //安全设置按钮句柄*/


    /*文件点播界面上的控件句柄*/
	HWND				m_hFileFstPage;   //文件点播界面首页按钮句柄
	HWND				m_hFilePrePage;   //文件点播界面上一页按钮句柄
	HWND				m_hFileNxtPage;   //文件点播界面下一页按钮句柄
	HWND				m_hFileLstPage;   //文件点播界面最后一页按钮句柄
	HWND				m_hFileWhich;	  //跳转输入框句柄	
	HWND				m_hFileGo;        //文件点播界面跳转按钮句柄
	HWND				m_hQueryStartCur; //在当前节目柜中开始查找按钮句柄
	HWND				m_hQueryStartTal; //在整个节目库中开始查找按钮句柄
	HWND				m_hFileNameEdit;  //节目检索的Edit框句柄
	HWND				m_hFileDirTree;   //节目柜树句柄
	HWND				m_hFileList;      //节目列表句柄

	/*文件播放界面的控件按钮句柄*/
	
	HWND				m_hFilPlayClose;   //关闭
	HWND				m_hFilPlayFull;    //全屏
	HWND				m_hFilPlayFit;    //满屏
	HWND				m_hFilPlayNormal;  //原始大小

	/*会议直播界面控件句柄*/
	HWND				m_hMeetTree;	//会议直播界面的
	HWND				m_hMeetList;

	/*节目柜管理界面控件句柄*/
	HWND				m_hAddFolder;		//增加节目柜按钮句柄
	HWND				m_hDelFolder;		//删除节目柜按钮句柄
	HWND				m_hFolFstPage;      //首页按钮句柄
	HWND				m_hFolPrePage;      //上一页按钮句柄
	HWND				m_hFolNxtPage;      //下一页按钮句柄
	HWND				m_hFolLstPage;      //尾页按钮句柄
	HWND				m_hFolWhich;
	HWND				m_hFolGo;            //跳转按钮句柄

    /*HWND				m_hAddFoldName;       //节目柜名输入框句柄
	HWND				m_hAddParerntFold;     //父节目柜句柄
	HWND				m_hAddFoldDescr;       //节目柜描述框句柄
	HWND				m_hAddFoldClose;       //关闭按钮句柄、
	HWND				m_hAddFoldSave;        //保存按钮句柄

	HWND				m_hModFoldName;       //节目柜名输入框句柄
	HWND				m_hModParerntFold;     //父节目柜句柄
	HWND				m_hModFoldDescr;       //节目柜描述框句柄
	HWND				m_hModFoldClose;       //关闭按钮句柄、
	HWND				m_hModFoldSave;        //保存按钮句柄*/

	HWND				m_hFolFolTree;         //节目柜树句柄
	HWND				m_hFolFolList;		   //节目柜列表句柄
	





	/*节目发布界面控件句柄*/
	HWND				m_hAddFile;		       //增加节目按钮句柄
	HWND				m_hDelFile;	           //删除节目句柄按钮
	HWND				m_hFilpubFstPage;      //首页按钮句柄
	HWND				m_hFilpubPrePage;      //上一页按钮句柄
	HWND				m_hFilpubNxtPage;      //下一页按钮句柄
	HWND				m_hFilpubLstPage;      //尾页按钮句柄
	HWND				m_hFilpubWhich;
	HWND				m_hFilpubGo;           //跳转按钮句柄
	HWND				m_hFilpubTree;
	HWND				m_hFilpubList;

	/*HWND				m_hFileName;           //文件命输入框句柄
	HWND				m_hRecvFoldName;        //所属节目柜输入框句柄
	HWND				m_hFilePath;           //节目路径输入框句柄
	HWND				m_hFileTalName;        //文件全名输入框句柄
	HWND				m_hFileSize;           //文件大小输入框句柄
	HWND				m_hFileDescr;          //文件描述输入框句柄
	HWND				m_hFileType;           //文件类型句柄
	HWND				m_hFileProp;           //文件属性
	HWND				m_hModFileClose;       //关闭按钮句柄
	HWND				m_hModFileSave;        //保存按钮句柄*/

	/*日志维护界面控件按钮句柄*/
	HWND				m_hDelLog;             //删除日志按钮句柄
	HWND				m_hLogList;

	/*密码更改*/
	HWND				m_hOlePwdEdit;		   //旧密码Edit框句柄
	HWND				m_hNewPwdEdit;         //新密码Edit框句柄
	HWND				m_hConfirmEdit;        //确认框句柄

	/*用户管理界面*/
	HWND				m_hUserReg;            //用户注册按钮句柄
	HWND				m_hRefreshList;        //刷新列表按钮句柄
	HWND				m_hUserCount;          //人数框句柄
	HWND				m_hUserList;

	/*用户注册界面控件句柄*/
/*	HWND				m_hUserNextPage;       //下一页按钮句柄
	HWND				m_hUserPrePage;        //上一页按钮句柄
	HWND				m_hUserRegsit;         //注册按钮句柄
	HWND				m_hUserCancel;         //取消按钮句柄
	HWND				m_hUserRegistId;       //用户ID输入框句柄
	HWND				m_hUserRegistName;     //用户名输入框句柄
	HWND				m_hUserRegistMale;      //男单选框句柄
	HWND				m_hUserRegistFemale;    //女单选框句柄
	HWND				m_hUserRegistGroup;     //用户组控件句柄
	HWND				m_hUserRegistDate;    //日期框句柄
	HWND				m_hUserRegistNever;   //帐号不过期框句柄
	HWND				m_hUserRegistPwd;     //密码框句柄
	HWND				m_hUserRegistConf;    //确认框句柄
	HWND				m_hUserRegistIp;      //IP框句柄

	HWND				m_hTel;  //电话框句柄
	HWND				m_hMobil;  //手机框句柄
	HWND				m_hCall;   //拷机框句柄
	HWND				m_hEmail;  //Email框句柄
	HWND				m_hAddr;   //地质框句柄
	HWND				m_hCorp;   //公司名框句柄*/

	/*安全设置界面*/
	HWND			m_hSecuGroupEdit; //当前用户组输入框句柄
	HWND			m_hSecuFolder ;		//节目归管理
	HWND			m_hSecuFilePub;		//节目发布
	HWND			m_hSecuLogMag;		//日至维护
	HWND			m_hSecuSecret;    //观看保密文件、
	HWND			m_hSecuTree;		//用户组树。
	HWND			m_hSecuChange;		//更改
	HWND			m_hSecuReset;       //返回上次设置

};

#endif // !defined(AFX_VODGUITESTLIB_H__487413F5_38EB_4320_B3A1_A911CEC17636__INCLUDED_)
