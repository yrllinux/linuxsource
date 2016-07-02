/****************************************************************************** 
模块名: FTPC
文件名: ftpc_api.h 
功能  : 
ftp库头文件 版本： 
******************************************************************************/

#ifndef _FTPC_API_H
#define _FTPC_API_H 

#include"kdvtype.h"

#ifdef __cplusplus 
extern "C" { 
#endif 

//api返回值

#define FTPC_OK                 (s32)0  //正常
#define FTPC_PARAMERR           (s32)1  //传递的参数错误
#define FTPC_CREATESOCKERR      (s32)2  //创建socket失败
#define FTPC_GETOPTERR          (s32)3  //获取option选项是失败
#define FTPC_SETOPTERR          (s32)4  //设置option选项失败
#define FTPC_SELECTERR          (s32)5  //select调用失败
#define FTPC_CONNECTIMEOUT      (s32)6  //连接超时
#define FTPC_CONNECTREFUSEDERR  (s32)7  //连接被拒绝
#define FTPC_CONNECTFAILED      (s32)8  //获取server连接状况失败
#define FTPC_PASSERR            (s32)9  //密码错误
#define FTPC_USERNAMEERR        (s32)10 //用户名出错
#define FTPC_PASVMODEERR        (s32)11 //传递pasv信令错误
#define FTPC_SIZEGETERR         (s32)12 //获取文件size失败
#define FTPC_RETRCOMERR         (s32)13 //传递下载信令失败
#define FTPC_THREADERR          (s32)14 //创建线程失败
#define FTPC_SEMERR             (s32)15 //信号量操作失败
#define FTPC_FILESTATERR        (s32)16 //获取文件属性失败
#define FTPC_CWDCOMERR          (s32)17 //传递cwd信令失败
#define FTPC_STORCOMERR         (s32)18 //传递stor信令失败
#define FTPC_ALLOCOMERR         (s32)19 //传递allo信令失败
#define FTPC_MEMMALLOCERR       (s32)20 //malloc memory失败
#define FTPC_OPENFILEERR        (s32)21 //打开文件出错
#define FTPC_FILE_NOT_FINISH    (s32)22 //上一个文件未传输完成
#define FTPC_CONNECTERR         (s32)23 //connect调用出错

//回调消息类型(对应回调函数参数 s32 dwMsgType)
#define FTPC_TRANSFILEERR       (s32)100     //与服务器传输文件出错
#define FTPC_LOCALFILEERR       (s32)101     //写或读本地文件出错
#define FTPC_MEMSPACELACK       (s32)102     //分配的给内存不足
#define FTPC_TRANSPERCENT       (s32)103     //文件已传输百分比消息
#define FTPC_FINISHED           (s32)104     //文件传输完成消息


//回调函数参数结构定义
typedef struct 
{
	s32 dwMsgType;		        //消息类型
	union
	{
	    u32 dwTransPercent;     //对应 msgType = FTPC_TRANSPERCENT 数据结构	    
	    u32 dwMsgDescripton;    //获取的文件size
	}UMsgInfo;
}TFtpcCallback;			

//回调函数定义
typedef void (*CBFtpcCallbackFun)(TFtpcCallback *PTParam, void *pCbd);


/*=============================================================================
函 数 名：FTPCSetServAddr
功    能：设置服务器端的ip地址和端口

注    意：传入的第一个参数是网络地址字符串，是点分二进制形式
算法实现：将网络地址和端口设置到全局变量

全局变量：无
参数说明：
@param：s8  *pwServIp  -    服务器IP网络地址字符串
@param：u16 wServPort  -    服务器端口号
返 回 值： s32   FTPC_OK   成功； FTPC_PARAMERR   失败
=============================================================================*/
s32 FTPCSetServAddr(const s8 *paServIp, const u16 wServPort);

/*=============================================================================
函 数 名：FTPCSetLogId
功    能：设置登陆用户名和密码

注    意：如果是匿名(anonymous),任何密码皆可
算法实现：传登陆的用户名和密码设置模块的全局变量
全局变量：无
参    数：
@param: s8  *paUserName   -     登陆用户名
@param: s8  *pwPassword   -     登陆密码
返 回 值： s32   FTPC_OK   成功； FTPC_PARAMERR   失败
=============================================================================*/
s32  FTPCSetLogId(const s8 *paUserName, const s8 *paPassword);

/*=============================================================================
函 数 名： FTPCGetFileinMem
功    能： 1. 设置文件的下载模式为内存
           2. 设置服务器文件文件名和路径
           3. 下载文件并存放在内存中
注    意：
算法实现： 通过FTP RETR取得文件，将文件保存在pwMemBuf内存中
全局变量：
参数说明：
@param:   s8  *pwServpath   -   文件路径
@param:   s8  *pwFileName    -  文件名
@param:   s8  *pwMemBuf     - 存放文件起始地址
@param:   s32 dwMemSize     -本模块可用内存大小
@param:   s8    *paCbd  -     用户自定义回调参数，用来标记当前传输文件
返 回 值： s32   FTPC_OK   成功;失败见api返回值的宏定义

特殊说明： 文件传输采用异步模式，传输过程中会通过回调将文件传输的进度返回给上层
=============================================================================*/
s32  FTPCGetFileinMem(const s8 *paServpath, const s8 *paFileName, s8 *pwMemBuf, s32 dwMemSize, void *paCbd) ;

/*=============================================================================
函 数 名：  FTPCGetFileinFlash
功    能：  1. 设置文件的下载模式为flash
            2. 设置服务器文件文件名和路径
            3. 下载文件并存放在flash中

注    意：
算法实现：  通过FTP RETR取得文件，将文件保存在paLocalPath FLASH中
全局变量：
参数说明：
@param:   s8    *pwServpath   -     服务器文件路径
@param:   s8    *pwFileName   -     服务器文件名
@param:   s8    *pwLocalPath  -     本地存放路径，包含文件名
@param:   s8    *paCbd  -     用户自定义回调参数，用来标记当前传输文件
返 回 值：s32   FTPC_OK   成功   失败见api返回值的宏定义
特殊说明：文件传输采用异步模式，传输过程中会将传输的进度返回给上层
=============================================================================*/
s32  FTPCGetFileinFlash(const s8 *paServpath, const s8 *paFileName, const s8 * paLocalPathFile, void *paCbd) ;

/*=============================================================================
函 数 名：  FTPCUploadFile
功    能：  1、设置上传文件的路径文件名
            2、上传文件                        

注    意：
算法实现：  通过FTP STOR上传文件
全局变量：
参数说明：
@param:   s8    *pwServpath   -     服务器文件路径
@param:   s8    *paServFileName -   服务器存放的文件名
@param:   s8    *pwFileName   -     本地文件名
@param:   s8    *pwLocalPath  -     本地路径
@param:   s8    *paCbd  -     用户自定义回调参数，用来标记当前传输文件
返 回 值：s32   FTPC_OK   成功   失败见api返回值的宏定义
特殊说明：文件传输采用异步模式，传输过程中会将传输的进度返回给上层
=============================================================================*/
s32 FTPCPutFile(const s8 *paServpath, const s8 *paServFileName, const s8 *paFileName, const s8 * paLocalPath, void *paCbd) ;

/*=============================================================================
函 数 名：  FTPCUploadMemData
功    能：  1、设置上传内存数据地址
            2、上传文件                        

注    意：
算法实现：  通过FTP STOR上传文件
全局变量：
参数说明：
@param:   s8    *pwServpath   -     服务器文件路径
@param:   s8    *paServFileName -   服务器存放的文件名
@param:   s8    *pwMemAddr   -     本地数据地址
@param:   s8    *dwMemSize  -     本地路径
@param:   s8    *paCbd  -     用户自定义回调参数，用来标记当前传输文件
返 回 值：s32   FTPC_OK   成功   失败见api返回值的宏定义
=============================================================================*/
s32 FTPCPutMemData(const s8 *paServpath, const s8 *paServFileName, const s8 *paMemAddr, const s32 dwMemSize, void *paCbd) ;

/*=============================================================================
函 数 名：  FTPCReset
功    能：  清除模块所有初始设置

注    意：
算法实现：
全局变量：  无    
参    数：  无
返 回 值：  s32   FTPC_OK   成功； FTPC_ERROR   失败
=============================================================================*/
s32 FTPCReset(void);

/*====================================================================
函 数 名：  FTPCRegCallback
功    能：  注册分配地址的回调函数
算法实现：
全局变量：  无
参数说明：
@param:     CBFtpcCallbackFun fun  -   回调函数
返 回 值：  s32   FTPC_OK   成功；     FTPC_PARAMERR失败
=============================================================================*/
s32 FTPCRegCallback(CBFtpcCallbackFun  fun);

/*====================================================================
函 数 名：  FTPCUnRegCallback
功    能：  注销分配和收回地址的回调函数
算法实现：
全局变量：  无
参数说明：  N/A
返 回 值：  s32   FTPC_OK   成功
=============================================================================*/
s32 FTPCUnRegCallback(void);

/*=============================================================================
函 数 名：FtpcGetVersion
功    能：获取ftpc版本号

注    意：
算法实现：

全局变量：无
参数说明：
@param：s8 pVersion: -- 字符指针

返 回 值： s32    FTPC_OK   成功； FTPC_PARAMERR   失败
=============================================================================*/
s32 FtpcGetVersion(s8 *pVersion);

/*=============================================================================
函 数 名：  FTPCInit
功    能：  初始化FTPC模块
            
注    意：  
算法实现：  
全局变量：  无
参    数：  无
返 回 值：  u32  FTPC_OK   成功            FTPC_ERROR   失败
=============================================================================*/
s32  FTPCInit(void);

/*======================================================
函 数 名：	FTPCSetMem
功    能：	ftpc模块申请一块内存
算法实现：
参数说明：	无	 
返 回 值：	FTPC_OK  成功       FTPC_MEMMALLOCERR  失败	
--------------------------------------------------------
修改记录：
	日期	版本	修改人	走读人	修改记录
========================================================*/
s32 FTPCSetMem(u32 dwMemLen);

/*======================================================
函 数 名：	FTPCFreeMem
功    能：	ftpc模块释放内存
算法实现：
参数说明：	无	 
返 回 值：	FTPC_OK  成功       FTPC_ERROR  失败	
--------------------------------------------------------
修改记录：
	日期	版本	修改人	走读人	修改记录
========================================================*/
s32 FTPCFreeMem(void);




#ifdef __cplusplus
}
#endif

#endif


