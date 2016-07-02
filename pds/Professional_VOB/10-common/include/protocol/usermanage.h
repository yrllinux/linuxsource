#ifndef _UMM_H_
#define _UMM_H_
#include "ummessagestruct.h"
#include "lanman.h"

#define VER_UM (const char *)"kdvum.40.06.02.131008"

#define AID_UMSERVER                    236
#define MAX_MSG_BUFF_SIZE               (64*1024)

// 定义统一移到头文件来 [pengguofeng 5/7/2013]
#if defined( WIN32 )    /* WIN32 */
#define PATH_DATA			( LPCSTR )"data"	//信息文件路径
#elif defined( VXWORKS_SIMU )
#define PATH_DATA			( LPCSTR )"data"	//信息文件路径
#elif defined(_LINUX_)
#define PATH_DATA			( LPCSTR )"/usr/conf"	//信息文件路径
#else                   /* vxWorks */
#define PATH_DATA			( LPCSTR )"/data"	//信息文件路径
#endif

// 编码方式 [pengguofeng 4/22/2013]
#define ENCODE_GBK						0
#define ENCODE_UTF8						1

// 文件名
#define USERINFOFILENAME				( LPCSTR )"login.usr"
// 文件头
#define UM_FILEHEAD						(const s8*)"kdvum"
// 版本号
#define UM_VER							(u32)20130422
#define MAX_FILEHEAD_LEN				32
#define USERINFO_MEMBERNUM				(u8)8			// CUserFullInfo的成员个数 [pengguofeng 4/24/2013]
//////////////////////////////////////////////////////////////////////////

//枚举定义
// 使用说明:
// emType[u8] + emBufLen[u32] + emBuf[s8* ]
// 允许使用的范围为1~254 [pengguofeng 5/6/2013]
enum emType
{
	emUmStart = (u8)0,	//禁止使用的值
	emUserInfo,
	//以下留待扩展，最大限到254
	emEnd
};

class CUsrManage
{
public:
	/*
	 *	构造函数使用说明：acFilePath： 保存目录，仍沿用以前的宏  不需要修改
	 *                    achFileName: 保存的文件名，仍沿用以前的宏 不需要修改
	 *					  byEncoding:  视当前MCU/MT平台所使用的编码方式 自己设定
	 *                    bIsMcu    :  仅MCU平台需要设成TRUE,其他(如MT)使用默认的FALSE即可
	 */
	CUsrManage(); //默认
	CUsrManage( u8* acFilePath, u8* achFileName = NULL, u8 byEncoding = ENCODE_GBK, BOOL32 bIsMcu = FALSE); //带存储用户信息文件路径
	~CUsrManage();
private:
	u32 m_uError; //消息错误码
	CUserFullInfo m_usrAllMembers[MAX_USERNUM]; //用户列表
	u8  m_byNumUsr; //用户个数
	char  m_acCurrentPath[256];//存储用户信息文件路径
	char  m_achFileName[MAX_UMFILENAME_LEN];   //老版本存储保存用户信息的文件名
	char  m_achCurFile[MAX_UMFILENAME_LEN];    //当前存储用户信息的新文件名
    s8 m_pMsgBuff[MAX_MSG_BUFF_SIZE];
    BOOL m_bSingle;
	u8	m_byPlatEncoding;                      //平台编码方式:使用上面宏定义 [pengguofeng 4/22/2013]
	u8	m_byFileEncoding;						//文件编码方式：只在开始的时候读过一回
	u8  m_byEmTypeNum;							//emtype数量
	u8  m_byIsMcu;								// mcu平台会用discription的特殊字段 [pengguofeng 7/15/2013]

public:
	CUserFullInfo m_usrCurLogin; //当前登录的用户
	//init and quit 
private:
	void	Reset(); //清空所有用户,包括管理员
protected:
	BOOL WriteAllUserBackToFile();//所有用户信息写入文件
	// lang [pengguofeng 4/22/2013]
	void SetPlatEncoding( const u8 &byEncoding = ENCODE_GBK); //设置平台编码方式
	u8   GetPlatEncoding(void);
	void SetFileEncoding(const u8 &byEncoding = ENCODE_GBK);
	u8   GetFileEncoding(void);
	void SetEmTypeNum(const u8 &byEmTypeNum);
	u8   GetEmTypeNum();
	void WriteUserInfo(FILE *fp);   //将用户信息写入文件，自用接口
	void ReadUserInfo(FILE *fp);    //从文件中解析用户信息，自用接口
	u32  GetBufLen();
	u32  GetHeadLen();
	void TransEncoding(); // 转码 [pengguofeng 4/22/2013],如果不带参数，则需要保存至成员中

	void UmPrint(s8 *pszFmt, ...);
	//operate
public:	
    void SetSingle(BOOL bSingle); //是否单用户

    BOOL GetAllUserFromFile();	//从文件中读入所有用户信息

    s32 GetUserNum(){return m_byNumUsr;};
    //获取第nIndex用户信息， nIndex从0开始
    BOOL GetUserFullInfo(CUserFullInfo* ptUserInfo, s32 nIndex);

	BOOL AddUser(CUserFullInfo* newusr); //添加一个用户
	BOOL DelUser(CUserFullInfo* oldinfo);//删除一个用户
	BOOL DelAllMembers();//删除所有用户
	BOOL ModifyInfo(CUserFullInfo* newinfo);//修改一个用户
	BOOL CheckLoginValid(CLoginRequest* curusr);//检查当前登录用户信息
	BOOL GetAllUser(CMessage* pcMsg); //得到所有用户信息
	void CmdLine(int cmdcode); //命令显示信息
	void SetPath(char* acFilePath); //设置保存文件路径
	u16 GetLastError() //当前操作的错误码
	{
		return (u16)m_uError;
	}
	void TestDelAllUser(); //集成测试删除所有用户

	// 读文件的固定信息 [pengguofeng 5/9/2013]
	void ReadFile();  //文件从成员变量里找
	BOOL32 IsMatchFileHead(FILE *fp, const s8 *pFileHead = NULL);
	void WriteFileHead(FILE *fp, const s8 *pFileHead = NULL);
	BOOL32 IsMatchSoftVer(FILE *fp, const u32 &dwVer = 0);
	void WriteSoftVer(FILE *fp, const u32 &dwVer = 0);
	void ReadFileAttr(FILE *fp);
	void WriteFileAttr(FILE *fp);
	void SkipUnknownType(FILE *fp);
	//message
private:
	void OnLigIn(CMessage* pcMsg);
	void OnAddUser(CMessage* pcMsg);
	void OnDelUser(CMessage* pcMsg);
	void OnModifyInfo(CMessage* pcMsg);
	void OnGetAllMembers(CMessage* pcMsg);
	void DisplayMenu();
	void OnDelAllMembers(CMessage* pcMsg);
	
	//interface
public:
	void ShowAll();//命令所有用户信息
	void ProcessMsg(CMessage* pcMsg);
	void TransferToOldFile(void); // 转换版本接口 [pengguofeng 4/24/2013]
}
#ifdef _VXWORKS_
__attribute__ ( (packed) ) 
#endif // _VXWORKS_
;

API BOOL32 InitUserManage();

#endif
