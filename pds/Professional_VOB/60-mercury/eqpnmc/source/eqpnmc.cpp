/*****************************************************************************
    模块名      : EqpAgt
    文件名      : eqpnmc.cpp
    相关文件    : 
    文件实现功能: 外设网管客户端
    作者        : liaokang
    版本        : V4r7  Copyright(C) 2011 KDC, All rights reserved.
-----------------------------------------------------------------------------
    修改记录:
    日  期      版本        修改人          修改内容
    2012/06/18  4.7         liaokang        创建
******************************************************************************/
#include "eqpnmc.h"
#include "eqpagt.h"

#ifdef WIN32
#include "Shlwapi.h"
#pragma comment (lib,"shlwapi.lib") 
#endif
// 隐藏控制窗口
// 在release版 project->setting->link->project option (下面的文本框)中
// 添加/subsystem:"windows" /entry:"mainCRTStartup"，相当于在代码中添加
/* #pragma comment(linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"") */

// 全局信号量
static SEMHANDLE g_tSemHandle;

#ifdef WIN32

// 判断是否为开机自动运行
API BOOL32 IsAutoRun(void)
{
    HKEY   hRegKey;   //注册key
    s8 achKeyName[256] = "Software\\Microsoft\\Windows\\CurrentVersion\\Run";//注册表
    if( ERROR_SUCCESS != RegOpenKey(HKEY_LOCAL_MACHINE, achKeyName, &hRegKey) )//打开注册表     
    {
        StaticLog( "[IsAutoRun] RegOpenKey wrong. Can not do the judgement.\n");
        return FALSE;   
    }
    if( ERROR_SUCCESS != RegQueryValueEx( hRegKey, "eqpnmc", NULL, NULL, NULL, NULL) )
    {
        StaticLog( "[IsAutoRun] The exe is not autorun.\n");
        RegCloseKey(hRegKey);   //关闭注册表key 
        return FALSE;
    }   
    //成功后执行释放内存
    RegCloseKey(hRegKey);
    StaticLog( "[IsAutoRun] The exe is autorun.\n");
    return TRUE;
}


//实现开机自启动
API BOOL32 AutoRun(void)
{
    BOOL bRe = TRUE; 
    s8   achCurPath[EQPAGT_MAX_LENGTH] = {'\0'}; //程序当前路径
    
    HKEY   hRegKey;   //注册key
    s8 achKeyName[256] = "Software\\Microsoft\\Windows\\CurrentVersion\\Run";//注册表
    GetModuleFileName( NULL,achCurPath, EQPAGT_MAX_LENGTH ); //获得运用程序路径
    StaticLog( "[AutoRun] The program current path is:%s.\n", achCurPath);
    sprintf( achCurPath + strlen(achCurPath), "%s", AUTORUN_FLAG );
    if( ERROR_SUCCESS != RegOpenKey(HKEY_LOCAL_MACHINE, achKeyName, &hRegKey) )//打开注册表     
    { 
        StaticLog( "[AutoRun] RegOpenKey wrong, start autorun failed.\n");
        bRe = FALSE;
        return bRe;   
    }   
    if( ERROR_SUCCESS != RegSetValueEx(hRegKey, "eqpnmc", 0,REG_SZ,   
        (const unsigned char *)achCurPath, EQPAGT_MAX_LENGTH))   //打开成功写信息到注册表
    { 
        StaticLog( "[AutoRun] Start autorun failed.\n");
        //写入失败
        RegCloseKey(hRegKey);   //关闭注册表key 
        bRe = FALSE;
        return bRe;
    }   
    //成功后执行释放内存
    RegCloseKey(hRegKey);  
    StaticLog( "[AutoRun] The program will be autorun.\n");
    return bRe;
}

//取消开机运行
API void CancelAutoRun(void)
{
    s8 achKeyName[256] = "Software\\Microsoft\\Windows\\CurrentVersion\\Run";//注册表
    if ( ERROR_SUCCESS != SHDeleteValue( HKEY_LOCAL_MACHINE, achKeyName, "eqpnmc" ) )
    {
        StaticLog( "[CancleAutoRun] SHDeleteValue wrong, cancel autorun failed.\n");
        return;
    } 
    StaticLog(  "[CancleAutoRun] Cancel autorun succecced.\n");
}

// 仅运行一个实例
BOOL32 OnlyStartOne()
{  
    HANDLE hSem =  CreateSemaphore(NULL, 1, 1, "eqpnmc.exe");   
    //   信号量已存在？   
    //   信号量存在，则程序已有一个实例运行   
    if (GetLastError()   ==   ERROR_ALREADY_EXISTS)   
    {
        //   关闭信号量句柄   
        CloseHandle(hSem);   
        //   寻找先前实例的主窗口   
        HWND   hWndPrevious = GetWindow( GetDesktopWindow(),GW_CHILD );   
        while( IsWindow(hWndPrevious) )   
        {   
            //   检查窗口是否有预设的标记?   
            //   有，则是我们寻找的主窗   
            if( GetProp(hWndPrevious, "eqpnmc.exe") )   
            {                    
                return   FALSE;   
            }   
            //   继续寻找下一个窗口   
            hWndPrevious = GetWindow( hWndPrevious,GW_HWNDNEXT ); 
        }   
        return FALSE;   
    }   
    return TRUE;
}
#endif

// 退出
API void quit(void) 
{
    // 退出前先flush
    OspDelay(20);
    logflush();
    OspDelay(20);
    
    // eqpagt退出
    g_cEqpAgt.EqpAgtQuit();
    // 退出OSP
    OspQuit();
    // 释放全局信号量
    OspSemGive( g_tSemHandle );
}

// 主函数
int main()
{
#ifdef WIN32
    if (!OnlyStartOne())
    {
        return 0;
	}
#endif

    // KdvLog模块初始化
    Err_t err = LogInit( EQPNMC_KDVLOG_FILE );
    if (LOG_ERR_NO_ERR != err)
    {
        printf("[main]LogInit() failed! ERRCODE = [%d]\n",err);
	}    
    
#if _DEBUG
    // 初始化Osp
    OspInit(TRUE, EQP_TELNET_PORT);
#else
    // 初始化Osp release版不显示telnet窗口
    OspInit(FALSE, EQP_TELNET_PORT);
    //Osp telnet 初始授权
    OspTelAuthor( EQP_TEL_USRNAME, EQP_TEL_PWD );
#endif

    // 初始化网元代理
    g_cEqpAgt.EqpAgtInit();

	// 创建信号量
    if ( !OspSemBCreate(&g_tSemHandle) )
    {
        printf( "[main] OspSemBCreate failed\n" );
        return 0;
	}
	// Take两次二元信号量，阻塞    
    if ( !OspSemTake( g_tSemHandle ) )
    {
        printf( "[main] OspSemTake 1 failed\n" );
        return 0;
    }
    if ( !OspSemTake( g_tSemHandle ) )
    {
        printf( "[main] OspSemTake 2 failed\n" );
        return 0;
    }
    return 0;
}