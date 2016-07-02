#ifndef _MESSAGESTRUCT_H_
#define _MESSAGESTRUCT_H_
#include "umconst.h"
#include "osp.h"
#include "md5.h"
#ifdef WIN32
    #pragma pack( push )
    #pragma pack( 1 )
#endif
API void umver();
class CLoginRequest
{
private:
	char		name[MAX_CHARLENGTH]; //用户名
	char		password[MAX_CHARLENGTH];//密码
	CMD5Encrypt m_cMd5;
public:
	CLoginRequest()
	{
		memset(name,0,MAX_CHARLENGTH);
		memset(password,0,MAX_CHARLENGTH);
	}
	//设置和得到用户名
	char* GetName()
	{
		return name;
	}
	void  SetName(char* buf)
	{
		if(buf == NULL)return;
		memset(name,0,MAX_CHARLENGTH);
		int length = (strlen(buf) >= MAX_CHARLENGTH -1 ? MAX_CHARLENGTH - 1: strlen(buf));
		memcpy(name,buf,length);
		name[length] = '\0';
	};
	//设置和得到密码
	char* GetPassword()
	{
		return password;
	}
	//输入密码为明文
	void  SetPassword(char* buf)
	{
		
		m_cMd5.GetEncrypted(buf,password);
		password[MAX_CHARLENGTH-1]='\0';
	}
	//输入的密码已经加密了
	void SetEncryptedPassword(char* buf)
	{
		memcpy(password,buf,MAX_CHARLENGTH);
		password[MAX_CHARLENGTH-1]='\0';
	}
	void Empty()
	{
		memset(name,0,MAX_CHARLENGTH);
		memset(password,0,MAX_CHARLENGTH);
	}
}
#if defined(_VXWORKS_) || defined(_LINUX_)
__attribute__ ( (packed) ) 
#endif // _VXWORKS_
;

class CUserFullInfo
{
// private:
public: // 方便计算成员大小 [pengguofeng 5/9/2013]
	CMD5Encrypt m_cMd5;
public:
	char		name[MAX_CHARLENGTH]; //用户名
	char		password[MAX_CHARLENGTH];//密码
	unsigned char	m_Actor;//权限
	char		fullname[MAX_CHARLENGTH];//完整用户名
	char		discription[2*MAX_CHARLENGTH];//用户描述
public:
	CUserFullInfo()
	{
		Empty();
	}
	~CUserFullInfo(){};
	//得到用户名
	char* GetName()
	{
		name[MAX_CHARLENGTH-1] = '\0';
		return name;
	}
	//设置用户名
	void  SetName(char* buf)
	{
		if(buf == NULL)return;
		memset(name,0,MAX_CHARLENGTH);
		int length = (strlen(buf) >= MAX_CHARLENGTH -1 ? MAX_CHARLENGTH - 1: strlen(buf));
		memcpy(name,buf,length);
		name[length] = '\0';
	}
	//得到密码
	char* GetPassword()
	{
		return password;
	}
	//设置密码
	void  SetPassword(char* buf)
	{
		m_cMd5.GetEncrypted(buf,password);
		password[MAX_CHARLENGTH-1]='\0';
	}
	//已经加密的密码
	void  SetEncryptedPassword(char* buf)
	{
		memcpy(password,buf,MAX_CHARLENGTH);
		password[MAX_CHARLENGTH-1]='\0';
	}
	//得到用户完整名
	char* GetFullName()
	{
		return fullname;
	}
	//设置用户完整名
	void SetFullName(char* buf)
	{
		if(buf == NULL)return;
		memset(fullname,0,MAX_CHARLENGTH);
		int length = (strlen(buf) >= MAX_CHARLENGTH -1 ? MAX_CHARLENGTH - 1: strlen(buf));
		memcpy(fullname,buf,length);
		fullname[length] = '\0';
	}
	//得到用户描述
	char* GetDiscription()
	{
		return discription;
	}
	
	//设置用户描述
	void SetDiscription(char* buf)
	{
		if(buf == NULL)return;
		memset(discription,0, 2*MAX_CHARLENGTH);
		int length = (strlen(buf) >= 2*MAX_CHARLENGTH -1 ?  2*MAX_CHARLENGTH - 1: strlen(buf));
		memcpy(discription,buf,length);
		discription[length] = '\0';
	}
	//得到权限
	unsigned char   GetActor(){return m_Actor;}
	//设置权限
	void  SetActor(unsigned char actor){ m_Actor = actor;}
	//对象赋值
	const CUserFullInfo& operator=(const CUserFullInfo& newInfo)
	{
		if (this != &newInfo)
		{
			memcpy(this, &newInfo, sizeof(CUserFullInfo));
		}
		return *this;
	}

	// 取所有成员的长度 [pengguofeng 5/14/2013]
	unsigned char GetMemLen( const unsigned char &byMemId )
	{
		u8 byRet = 0;
		switch ( byMemId)
		{
		case 0:
			byRet = sizeof(m_cMd5.context.state);
			break;
		case 1:
			byRet = sizeof(m_cMd5.context.count);
			break;
		case 2:
			byRet = sizeof(m_cMd5.context.buffer);
			break;
		case 3:
			byRet = sizeof(name);
			break;
		case 4:
			byRet = sizeof(password);
			break;
		case 5:
			byRet = sizeof(m_Actor);
			break;
		case 6:
			byRet = sizeof(fullname);
			break;
		case 7:
			byRet = sizeof(discription);
			break;
		case 8:
//			byRet = sizeof(add1);
			break;
		case 9:
//			byRet = sizeof(add2);
			break;
		default:
			break;
		}

		return byRet;
	}
	//判断某一个成员是否为字符串,0 1 2都是MD5的信息，不作修改
	bool IsMemString(const unsigned char &byMemId)
	{
		if ( byMemId == 3
			|| byMemId == 4
			|| byMemId == 6
			|| byMemId == 7
		/*	|| byMemId == 8
			|| byMemId == 9
		*/
			)
		{
			return TRUE;
		}
		return FALSE;
	}
	//判断用户名是否相同
	bool IsEqualName(char* newName)
	{
		if (strlen(name) != strlen(newName) || strlen(newName) == 0 )
			return FALSE;
		for (unsigned int i=0 ; i< strlen(name) ; i++)
		{
			if (name[i] != newName[i])
				return FALSE;
		}
		return TRUE;
	}	
	//判断口令是否相同
	bool IsPassWordEqual(char* newPass)
	{
		for (int i=0 ; i< MAX_CHARLENGTH-1; i++)
		{
			if (password[i] != newPass[i])
				return FALSE;
		}
		return TRUE;
	}
	//找到空的
	bool IsEmpty()
	{
		if ( strlen(password) > 16 )
		      return FALSE;
		return TRUE;
	}
	//清空
	void Empty()
	{
		memset(this, 0, sizeof(CUserFullInfo));
		m_Actor = UM_OPERATOR;
	}	

}
#if defined(_VXWORKS_) || defined(_LINUX_)
__attribute__ ( (packed) ) 
#endif // _VXWORKS_
;
class CLongMessage
{
public:
	u8 m_uTotal; //信息总帧数
	u8 m_uCurr;  //当前帧指示
	u8 m_uNumUsr;//当前帧中的用户数
public:
	CLongMessage()
	{
		Reset();
	}
	~CLongMessage(){};
public:
	void Reset()
	{
		m_uTotal = 0;
		m_uCurr =  0;
		m_uNumUsr = 0;
	}
}
#if defined(_VXWORKS_) || defined(_LINUX_)
__attribute__ ( (packed) ) 
#endif // _VXWORKS_
;
#ifdef WIN32
#pragma pack( pop )
#endif
#endif
