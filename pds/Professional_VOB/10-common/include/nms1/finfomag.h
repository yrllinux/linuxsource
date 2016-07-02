/********************************************************************
模块名:      VOD系统文件，文件夹，日志，用户等类库文件
文件名:      FInfoMag.h
相关文件:    
文件实现功能 库头文件
作者：       
版本：       4.0
------------------------------------------------------------------------
修改记录:
日		期	版本	修改人	走读人	修改内容
2005/08/23	4.0		

*********************************************************************/


#ifndef _FInfoMag_H
#define  _FInfoMag_H

/*宏定义*/
//UserInfoMag
#define  MAX_FREE_USER_PACKET_NUM		50
#define  DEFAULT_FREE_USER_PACKET_NUM		10

//SFolderInfoMag
#define  MAX_FREE_SFOLDER_PACKET_NUM		10
#define  DEFAULT_FREE_SFOLDER_PACKET_NUM	5

//PopInfoMag
#define  MAX_FREE_POP_PACKET_NUM		50
#define  DEFAULT_FREE_POP_PACKET_NUM		10

//LogInfoMag
#define  MAX_FREE_LOG_PACKET_NUM			100
#define  DEFAULT_FREE_LOG_PACKET_NUM		10

//FolderInfoMag
#define  MAX_FREE_FOLDER_PACKET_NUM		20
#define  DEFAULT_FREE_FOLDER_PACKET_NUM		10

//FileInfoMag
#define  MAX_FREE_FILE_PACKET_NUM		50
#define  DEFAULT_FREE_FILE_PACKET_NUM		10

/************************************************************************/
/*信息结构*/
//UserInfo文件信息
typedef struct tagUserInfo
{
	char m_szUserID[20+1];			//用户id
	char m_szUserName[10+1];		//用户名
	char m_szPwd[20+1];				//用户密码
	char m_szSex[10+1];				//用户性别
	char m_szRole[50+1];			//用户组别id
	char m_szRoleName[20+1];		//用户组别名（管理员组...）
	char m_szIfUnused[1+1];			//是否已停用
	char m_szTel[60+1];				//
	char m_szMobilePhone[60+1];		//
	char m_szBP[60+1];			//
	char m_szEmail[60+1];			//
	char m_szUnit[100+1];			//
	char m_szAddr[100+1];			//
	char m_szIfPassDate[1+1];		//是否帐号过期
	char m_szUnusedDate[20+1];		//
	char m_szIfAvailAlways[20+1];	//
	char m_szvod_Schedule1[20+1];	//
	char m_szvod_Schedule2[20+1];	//
	char m_szStylePath[10+1];		//
	char m_szLocalIP[20+1];			//用户登录ip限制
}TUserInfo;

//SFolderInfoMag
//会议直播源信息
typedef struct tagSFolderInfo
{
	int		 m_nClassID;
	char	 m_szSourceIP[20+1];		//会议直播源完整ip
	char	 m_szSourceDescribe[100+1];	//会议直播源描述
}TSFolderInfo;

//PopInfoMag
//文件信息
typedef struct tagPopInfo
{
	char m_szRole[10+1];			//用户组别名称（管理员组...）
	char m_szRoleName[20+1];		//用户组别英文简称
	char m_szPopedoms[200+1];	    //用户权限集
}TPopInfo;

//LogInfoMag
//文件信息
typedef struct tagLogInfo
{
	int  m_nRecordID;
	int  m_nFileID;
	char m_szUserID[20+1];		
	char m_szUserName[10+1];
	char m_szUserIP[15+1];
	char m_szFileName[50+1];
	char m_szBeginTime[20+1];
	char m_szEndTime[20+1];
}TLogInfo;

//FolderInfoMag
//文件夹信息
typedef struct tagFolderInfo
{
	int		 m_nClassID;
	int		 m_nParentClassID;
	char	 m_szClassName[20+1];		//文件夹完整名称
	char	 m_szParentClassName[20+1];	//父文件夹完整名称
	char	 m_szClassDescribe[100+1];	//文件夹描述
}TFolderInfo;


//FileInfoMag

//文件信息
typedef struct tagFileInfo
{
	int		 m_nFileID;
	int		 m_nClassID;
	char	 m_szClassName[20+1];		//文件夹完整名称
	char	 m_szFileFullName[50+1];	//文件完整名称（包含扩展名）
	char	 m_szFileShortName[50+1];	//文件简洁名称（包含扩展名）
	char	 m_szFilePath[100+1];		//文件完整路径（相对于mms根目录，包含ip）
	char	 m_szFileType[5+1];			//文件类型
	char	 m_szFileSize[20+1];		//文件大小
	char	 m_szFileSecret[4+1];		//文件加密等级	
	char	 m_szPublisherID[20+1];		//文件发布者ID
	char	 m_szPublisherName[10+1];	//文件发布者名称
	char	 m_szPublishTime[20+1];		//文件发布时间
	char	 m_szFileDesc[100+1];		//文件简要描述
}TFileInfo;



/************************************************************************/
/*用户信息管理的类定义*/
class CUserInfoList : public CList<TUserInfo*, TUserInfo*>
{
public:
	CUserInfoList();
	virtual ~CUserInfoList();
	
	void  InitPacket(TUserInfo* pPacket);
	DWORD GetTotalCount();
	void  DiscardAllPackets();
	
	//用于外部数据废弃后的回收
	TUserInfo* AllocPacket();
	void FreePacket(TUserInfo* pPacket);
	
	//用于外部数据的维护
	BOOL AddPacket(TUserInfo* pPacket, BOOL bDesc = FALSE);
	TUserInfo* GetPacket(BOOL bDesc = FALSE);
	
};

class CUserInfoMag  
{
public:
	CUserInfoMag();
	virtual ~CUserInfoMag();
	
public:
	
	DWORD GetTotalCount();
	void  DiscardAllPackets();
	
	//用于外部数据废弃后的回收
	TUserInfo* AllocPacket();
	void FreePacket(TUserInfo* pPacket);
	void FreeAllPackets();
	
	//用于外部数据的维护
	BOOL AddPacket(TUserInfo* pPacket, BOOL bDesc = TRUE);
	TUserInfo* GetPacket(BOOL bDesc = FALSE);
	
protected:
	CUserInfoList m_cUsedUserList;
	CUserInfoList m_cUnusedUserList;
	
private:
	CRITICAL_SECTION m_cs;
};

/************************************************************************/
/*//SFolderInfoMag的类定义*/

class CSFolderInfoList : public CList<TSFolderInfo*, TSFolderInfo*>
{
public:
	CSFolderInfoList();
	virtual ~CSFolderInfoList();
	
	void  InitPacket(TSFolderInfo* pPacket);
	DWORD GetTotalCount();
	void  DiscardAllPackets();
	
	//用于外部数据废弃后的回收
	TSFolderInfo* AllocPacket();
	void FreePacket(TSFolderInfo* pPacket);

	//用于外部数据的维护
	BOOL AddPacket(TSFolderInfo* pPacket, BOOL bDesc = TRUE);
	TSFolderInfo* GetPacket(BOOL bDesc = FALSE);
};

class CSFolderInfoMag  
{
public:
	CSFolderInfoMag();
	virtual ~CSFolderInfoMag();

public:
	
	DWORD GetTotalCount();
	void  DiscardAllPackets();

	//用于外部数据废弃后的回收
	TSFolderInfo* AllocPacket();
	void FreePacket(TSFolderInfo* pPacket);
	void FreeAllPackets();
	
	//用于外部数据的维护
	BOOL AddPacket(TSFolderInfo* pPacket, BOOL bDesc = TRUE);
	TSFolderInfo* GetPacket(BOOL bDesc = FALSE);
	
protected:
	CSFolderInfoList m_cUsedFolderList;
	CSFolderInfoList m_cUnusedFolderList;

private:
	CRITICAL_SECTION m_cs;
};

/************************************************************************/
/*PopInfoMag类定义*/

class CPopInfoList : public CList<TPopInfo*, TPopInfo*>
{
public:
	CPopInfoList();
	virtual ~CPopInfoList();
	
	void  InitPacket(TPopInfo* pPacket);
	DWORD GetTotalCount();
	void  DiscardAllPackets();
	
	//用于外部数据废弃后的回收
	TPopInfo* AllocPacket();
	void FreePacket(TPopInfo* pPacket);
	
	//用于外部数据的维护
	BOOL AddPacket(TPopInfo* pPacket, BOOL bDesc = FALSE);
	TPopInfo* GetPacket(BOOL bDesc = FALSE);
	
};

class CPopInfoMag  
{
public:
	CPopInfoMag();
	virtual ~CPopInfoMag();
	
public:
	
	DWORD GetTotalCount();
	void  DiscardAllPackets();
	
	//用于外部数据废弃后的回收
	TPopInfo* AllocPacket();
	void FreePacket(TPopInfo* pPacket);
	void FreeAllPackets();

	//用于外部数据的维护
	BOOL AddPacket(TPopInfo* pPacket, BOOL bDesc = TRUE);
	TPopInfo* GetPacket(BOOL bDesc = FALSE);
	
protected:
	CPopInfoList m_cUsedPopList;
	CPopInfoList m_cUnusedPopList;
	
private:
	CRITICAL_SECTION m_cs;
};


/************************************************************************/
/*LogInfoMag类定义*/


class CLogInfoList : public CList<TLogInfo*, TLogInfo*>
{
public:
	CLogInfoList();
	virtual ~CLogInfoList();
	
	void  InitPacket(TLogInfo* pPacket);
	DWORD GetTotalCount();
	void  DiscardAllPackets();
	
	//用于外部数据废弃后的回收
	TLogInfo* AllocPacket();
	void FreePacket(TLogInfo* pPacket);
	
	//用于外部数据的维护
	BOOL AddPacket(TLogInfo* pPacket, BOOL bDesc = FALSE);
	TLogInfo* GetPacket(BOOL bDesc = FALSE);
	
};

class CLogInfoMag  
{
public:
	CLogInfoMag();
	virtual ~CLogInfoMag();
	
public:
	
	DWORD GetTotalCount();
	void  DiscardAllPackets();
	
	//用于外部数据废弃后的回收
	TLogInfo* AllocPacket();
	void FreePacket(TLogInfo* pPacket);
	void FreeAllPackets();
	
	//用于外部数据的维护
	BOOL AddPacket(TLogInfo* pPacket, BOOL bDesc = TRUE);
	TLogInfo* GetPacket(BOOL bDesc = FALSE);
	
protected:
	CLogInfoList m_cUsedLogList;
	CLogInfoList m_cUnsedLogList;
	
private:
	CRITICAL_SECTION m_cs;
};

/************************************************************************/
/*FolderInfoMag类定义*/
class CFolderInfoList : public CList<TFolderInfo*, TFolderInfo*>
{
public:
	CFolderInfoList();
	virtual ~CFolderInfoList();
	
	void  InitPacket(TFolderInfo* pPacket);
	DWORD GetTotalCount();
	void  DiscardAllPackets();
	
	//用于外部数据废弃后的回收
	TFolderInfo* AllocPacket();
	void FreePacket(TFolderInfo* pPacket);

	//用于外部数据的维护
	BOOL AddPacket(TFolderInfo* pPacket, BOOL bDesc = TRUE);
	TFolderInfo* GetPacket(BOOL bDesc = FALSE);
};

class CFolderInfoMag  
{
public:
	CFolderInfoMag();
	virtual ~CFolderInfoMag();

public:
	
	DWORD GetTotalCount();
	void  DiscardAllPackets();

	//用于外部数据废弃后的回收
	TFolderInfo* AllocPacket();
	void FreePacket(TFolderInfo* pPacket);
	void FreeAllPackets();
	
	//用于外部数据的维护
	BOOL AddPacket(TFolderInfo* pPacket, BOOL bDesc = TRUE);
	TFolderInfo* GetPacket(BOOL bDesc = FALSE);
	
protected:
	CFolderInfoList m_cUsedFolderList;
	CFolderInfoList m_cUnusedFolderList;

private:
	CRITICAL_SECTION m_cs;
};


/************************************************************************/
/*FileInfoMag类定义*/


class CFileInfoList : public CList<TFileInfo*, TFileInfo*>
{
public:
	CFileInfoList();
	virtual ~CFileInfoList();
	
	void  InitPacket(TFileInfo* pPacket);
	DWORD GetTotalCount();
	void  DiscardAllPackets();
	
	//用于外部数据废弃后的回收
	TFileInfo* AllocPacket();
	void FreePacket(TFileInfo* pPacket);
	
	//用于外部数据的维护
	BOOL AddPacket(TFileInfo* pPacket, BOOL bDesc = TRUE);
	TFileInfo* GetPacket(BOOL bDesc = FALSE);

};

class CFileInfoMag  
{
public:
	CFileInfoMag();
	virtual ~CFileInfoMag();
	
public:
	
	DWORD GetTotalCount();
	void  DiscardAllPackets();
	
	//用于外部数据废弃后的回收
	TFileInfo* AllocPacket();
	void FreePacket(TFileInfo* pPacket);
	void FreeAllPackets();
	
	//用于外部数据的维护
	BOOL AddPacket(TFileInfo *pPacket, BOOL bDesc = TRUE);
	TFileInfo* GetPacket(BOOL bDesc = FALSE);
	
protected:
	CFileInfoList m_cUsedFileList;
	CFileInfoList m_cUnusedFileList;
	
private:
	CRITICAL_SECTION m_cs;
};
#endif