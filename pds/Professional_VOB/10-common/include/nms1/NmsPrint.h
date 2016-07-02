/***********************************************************************************************************
*											公用打印模块
*  目的: 主要为了管理Osp打印、
*  用法: 举例画面合成的打印
*		1、代码里面使用CNmsPrint::Print( "Vmp", "终端状态 MtId=%d", tMt.GetEqpID() ); 
*		2、需要一个注册了这个"Vmp"的User才能使用这个打印
*			两种方式注册。a、调用CNmsPrint的RegUser或RegUserS来注册，前面一个是数字用户，第二个是字符串
							两个函数第二个参数属于附加，通常如果你想要画面合成打印出如下效果的时候
							====Vmp==== 终端状态 MtId=1           这种打印需要注册CNmsPrint::RegUserS( "Vmp", "====Vmp====" );
						  b、第一种注册方式一般不常用，因为需要将函数放到公用的模块中调用，下面是第二种
							打开telnet，直接使用regusers reguser来进行注册，参数跟上面一样
							注册完了过后，记得用openlogs或openlog打开你注册的用户，这样就可以打印了
************************************************************************************************************/

#ifndef NMSPRINT_20100730
#define NMSPRINT_20100730

#pragma warning(disable:4244)  // conversion from 'unsigned long' to 'unsigned char', possible loss of data
#pragma warning(disable:4018)  // signed/unsigned mismatch
#pragma warning(disable:4786)  // identifier over 255

#include <map>
#include <string>
using namespace std;
#include "kdvtype.h"
#include "kdvdef.h"
#include "osp.h"
#pragma comment(lib,"osplib.lib")

#define MAX_NAME_LEN 1024

// 用户信息
typedef struct tagRegUserInfo
{
	BOOL32 bLogOpen; 
	s8 achName[MAX_NAME_LEN];
}TRegUserInfo,*PTRegUserInfo;

/*---------------------------------------------------------------------
* 类	名：CNmsPrint
* 功    能：提供一个统一的打印接口
* 特殊说明：
* 修改记录：
* 日期			版本		修改人		修改记录
* 2010/07/30	v1.0		牟兴茂		创建
----------------------------------------------------------------------*/
#define NMS_PRINT_ID_DEFAULT_OPEN "Default"

#define NMSAPI extern "C"			__declspec(dllexport)
class CNmsPrint
{
public:
	static BOOL32 Printf( s8* pUser, const s8* pScrip , ... );
public:
	// 默认打印描述
	static char* strNmsDefaultScrip;

	// 注册用户
//	static BOOL32 RegUser( u32 dwUserID , s8* pUserName );
	
	static BOOL32 RegUser( s8* pKey, s8* pUserName );
	/*---------------------------------------------------------------------
	* 函 数 名：OpenLog 
	* 功    能：打开开关 通过你所记住的ID，如果不记得，可以先调用showalluser
	* 参数说明：[in] dwUserID 唯一标识ID
	* 返 回 值：
	* 修改记录：
	* 日期			版本		修改人		修改记录
	* 2010/08/13	v1.0		牟兴茂		创建
	----------------------------------------------------------------------*/
	static BOOL32 OpenLog( u32 dwIndex );
	
	/*---------------------------------------------------------------------
	* 函 数 名：OpenLog
	* 功    能：通过用户名打开开关
	* 参数说明：
	* 返 回 值：
	* 修改记录：
	* 日期			版本		修改人		修改记录
	* 2010/08/13	v1.0		牟兴茂		创建
	----------------------------------------------------------------------*/
	static BOOL32 OpenLog( s8* pUser );
	
	
	/*---------------------------------------------------------------------
	* 函 数 名：CloseLog
	* 功    能：关闭开关
	* 参数说明：
	* 返 回 值：
	* 修改记录：
	* 日期			版本		修改人		修改记录
	* 2010/08/13	v1.0		牟兴茂		创建
	----------------------------------------------------------------------*/
	static BOOL32 CloseLog( u32 dwUserID );
	
	
	/*---------------------------------------------------------------------
	* 函 数 名：CloseLog
	* 功    能：关闭开关
	* 参数说明：
	* 返 回 值：
	* 修改记录：
	* 日期			版本		修改人		修改记录
	* 2010/08/13	v1.0		牟兴茂		创建
	----------------------------------------------------------------------*/
	static BOOL32 CloseLog( s8* pUser );
	
	/*---------------------------------------------------------------------
	* 函 数 名：IsLogOpen
	* 功    能：开关是否打开
	* 参数说明：
	* 返 回 值：
	* 修改记录：
	* 日期			版本		修改人		修改记录
	* 2010/08/13	v1.0		牟兴茂		创建
	----------------------------------------------------------------------*/
	static BOOL32 IsLogOpen( s8* pUserName );
	
	
	/*---------------------------------------------------------------------
	* 函 数 名：IsLogOpen
	* 功    能：开关是否打开
	* 参数说明：
	* 返 回 值：
	* 修改记录：
	* 日期			版本		修改人		修改记录
	* 2010/08/13	v1.0		牟兴茂		创建
	----------------------------------------------------------------------*/
	static BOOL32 IsLogOpen( u32 dwUserId );
	
	/*---------------------------------------------------------------------
	* 函 数 名：GetRegUserInfo
	* 功    能：获得注册用户信息
	* 参数说明：
	* 返 回 值：
	* 修改记录：
	* 日期			版本		修改人		修改记录
	* 2010/08/13	v1.0		牟兴茂		创建
	----------------------------------------------------------------------*/
	static PTRegUserInfo GetRegUserInfo( u32 dwIndex );
	static PTRegUserInfo GetRegUserInfo( s8* pUser );
	
	static map< string, PTRegUserInfo > m_mapUserS;
};

#endif // NMSPRINT_20100730